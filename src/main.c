/* 
 *
 * Copyright (C) Josef Gajdusek <atx@atx.name>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * */

#include "timer.h"
#include "uip-conf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "uip.h"
#include "uip_arp.h"
#include "network.h"
#include "enc28j60.h"
#include "nethandler.h"
#include "oled.h"
#include "bitmaps.h"
#include "ds18b20.h"
#include "dht.h"
#include "umqtt/umqtt.h"

#define led_init()		(DDRC |= _BV(PC1))
#define led_on()		(PORTC |= _BV(PC1))
#define led_off()		(PORTC &= ~_BV(PC1))
#define led_toggle()	(PORTC ^= _BV(PC1))

/* This is time in LOCAL time zone. This allows elegant centralized handling 
 * of leap seconds/summer time or whatever weirdness is imposed on us
 * */

struct human_time {
	unsigned int hours;
	unsigned int minutes;
	unsigned int seconds;
};

/* The currently displayed time.
 * Set to impossible values to force rendering right from the start
 * */
static struct human_time time_displ = { 25, 61, 61 };
static unsigned long time_day; /* Seconds of day */
static unsigned long time_offset;

static volatile bool flag_packet_rx = true;

static struct oled oled = {
	.sda = GPIO(C, 4),
	.scl = GPIO(C, 5),
};

static struct ds18b20 therm = {
	.gpio = GPIO(B, 1),
};

static struct dht dht = {
	.gpio = GPIO(B, 0),
};

static int pointer_x = 0;

static int display_putchar(char c, FILE __attribute__((unused))*stream)
{
	if (c == '\n') {
		pointer_x = 0;
		return 0;
	}
	oled_write_image_pgm(&oled, pointer_x, 0, 6, 1, bitmaps_font6x8[c - 0x20]);
	pointer_x = (pointer_x + 6) % 128;
	return 0;
}

static void display_time()
{
	unsigned long clock = clock_time_seconds();
	unsigned long h;
	unsigned long m;
	unsigned long s;

	time_day = (time_day + (clock - time_offset));
	time_offset = clock;

	h = time_day / 3600;
	m = (time_day - h * 3600) / 60;
	s = (time_day - h * 3600 - m * 60);

	if (h != time_displ.hours) {
		oled_write_image_pgm(&oled, 16 * 0, 3, 12, 3,
				bitmaps_7seg15x24[h / 10]);
		oled_write_image_pgm(&oled, 16 * 1, 3, 12, 3,
				bitmaps_7seg15x24[h % 10]);
		time_displ.hours = h;
	}
	if (m != time_displ.minutes) {
		oled_write_image_pgm(&oled, 16 * 2, 3, 12, 3,
				bitmaps_7seg15x24[m / 10]);
		oled_write_image_pgm(&oled, 16 * 3, 3, 12, 3,
				bitmaps_7seg15x24[m % 10]);
		time_displ.minutes = m;
	}
	/* We should get called at most once per second */
	oled_write_image_pgm(&oled, 20 + 0, 6, 7, 2, bitmaps_7seg7x16[s / 10]);
	oled_write_image_pgm(&oled, 20 + 12, 6, 7, 2, bitmaps_7seg7x16[s % 10]);

	time_displ.seconds = s;
}

static void display_date(int m, int d)
{
	unsigned int i;
	char str[6];

	snprintf(str, sizeof(str), "--%d-%d", m, d);

	for (i = 0; str[i]; i++)
		oled_write_image_pgm(&oled, i * 8, 0, 7, 2,
				str[i] >= '0' && str[i] <= '9' ?
				bitmaps_7seg7x16[str[i] - 0x30] :
				bitmaps_7seg7x16[10]);
}

static void display_weather(char *str)
{
	const uint8_t *ptr;

	if (strcmp(str, "cloudy") == 0)
		ptr = bitmaps_cloudy;
	else if (strcmp(str, "lightning") == 0)
		ptr = bitmaps_lightning;
	else if (strcmp(str, "rain") == 0)
		ptr = bitmaps_rain;
	else if (strcmp(str, "snow") == 0)
		ptr = bitmaps_snow;
	else if (strcmp(str, "clear+cloud") == 0)
		ptr = bitmaps_suncloud;
	else
		ptr = bitmaps_sun;
	oled_write_image_pgm(&oled, 58, 0, 20, 2, ptr);
}

static void display_therm(signed long i)
{
	i /= 1000;
	oled_write_image_pgm(&oled, 16 * 5, 3, 12, 3, bitmaps_7seg15x24[i / 10]);
	oled_write_image_pgm(&oled, 16 * 6, 3, 12, 3, bitmaps_7seg15x24[i % 10]);
}

static void display_humidity(int i)
{
	oled_write_image_pgm(&oled, 128 - 10, 0, 9, 2, bitmaps_percent9x16);
	oled_write_image_pgm(&oled, 128 - 20, 0, 7, 2, bitmaps_7seg7x16[i % 10]);
	oled_write_image_pgm(&oled, 128 - 31, 0, 7, 2, bitmaps_7seg7x16[i / 10]);
}

static void handle_message(struct umqtt_connection __attribute__((unused))*conn,
		char *topic, uint8_t *data, int len)
{
	char str[len + 1];
	unsigned int h;
	unsigned int m;
	unsigned int s;

	memcpy(str, data, len);

	str[len] = 0;

	if (strcmp(topic, MQTT_TOPIC_DATETIME) == 0) {
		sscanf(str, "%d:%d:%d", &h, &m, &s);
		time_day = h * 3600l + m * 60l + s;
		time_offset = clock_time_seconds();
	} else if (strcmp(topic, MQTT_TOPIC_DATE) == 0) {
		sscanf(str, "%d-%d-%d", &h, &m, &s);
		display_date(m, s);
	} else if (strcmp(topic, MQTT_TOPIC_WEATHER) == 0) {
		display_weather(str);
	}
}

static FILE display_stream = FDEV_SETUP_STREAM(display_putchar, NULL,
		_FDEV_SETUP_WRITE);

static uint8_t mqtt_txbuff[200];
static uint8_t mqtt_rxbuff[150];

static struct umqtt_connection mqtt = {
	.txbuff = {
		.start = mqtt_txbuff,
		.length = sizeof(mqtt_txbuff),
	},
	.rxbuff = {
		.start = mqtt_rxbuff,
		.length = sizeof(mqtt_rxbuff),
	},
	.message_callback = handle_message,
};

static void sensors_send(char *topic, signed long val)
{
	char buff[20];
	int len;
	len = snprintf(buff, sizeof(buff), "%ld", val);
	umqtt_publish(&mqtt, topic, (uint8_t *)buff, len);
}

int main()
{
	struct uip_eth_addr mac;
	struct timer periodic_timer;
	struct timer arp_timer;
	struct timer kalive_timer;
	struct timer dis_time_timer;
	struct timer dis_sensors_timer;
	struct timer sensors_send_timer;
	uip_ipaddr_t ip;
	signed long temp;
	int hum;

	mac.addr[0] = ETHADDR0;
	mac.addr[1] = ETHADDR1;
	mac.addr[2] = ETHADDR2;
	mac.addr[3] = ETHADDR3;
	mac.addr[4] = ETHADDR4;
	mac.addr[5] = ETHADDR5;

	oled_init(&oled);
	oled_fill_screen(&oled, 0x00);
	oled_active(&oled, true);
	oled_contrast(&oled, 0xff);

	stdout = &display_stream;

	ds18b20_convert(&therm); /* Initial conversion - we won't have 85C on startup */

	network_init();

	/* Setup INT0 on rx packet pending */
	EIMSK |= _BV(INT0);
	EICRA |= _BV(ISC11);
	enc28j60_write(EIE, 0b11000000);

	/* Setup magjack LEDs */
	enc28j60_phy_write(PHLCON, 0b0000011101001010);
	/* 25MHz / 2 */
	enc28j60_write(ECOCON, 2);

	led_init();
	clock_init();
	uip_init();
	sei();

	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);
	timer_set(&kalive_timer, CLOCK_SECOND * MQTT_KEEP_ALIVE);

	timer_set(&dis_time_timer, CLOCK_SECOND * 1);
	timer_set(&dis_sensors_timer, CLOCK_SECOND * 5);

	timer_set(&sensors_send_timer, CLOCK_SECOND * 60);

	uip_setethaddr(mac);
	uip_ipaddr(ip, IPADDR0, IPADDR1, IPADDR2, IPADDR3);
	uip_sethostaddr(ip);

	while(1) {
		if (flag_packet_rx) {
			flag_packet_rx = false;
			nethandler_rx();
		}
		if (timer_expired(&periodic_timer)) {
			timer_restart(&periodic_timer);
			nethandler_periodic();
		}
		if (timer_expired(&arp_timer)) {
			timer_restart(&arp_timer);
			nethandler_periodic_arp();
		}
		if (timer_expired(&kalive_timer)) {
			timer_restart(&kalive_timer);
			umqtt_ping(&mqtt);
		}
		if (timer_expired(&dis_time_timer)) {
			timer_restart(&dis_time_timer);
			display_time();
		}
		if (timer_expired(&dis_sensors_timer)) {
			timer_restart(&dis_sensors_timer);

			temp = ds18b20_read_temp(&therm);
			ds18b20_convert(&therm);
			display_therm(temp);

			hum = dht_humidity(&dht);
			display_humidity(hum);

			if (timer_expired(&sensors_send_timer)) {
				timer_restart(&sensors_send_timer);

				sensors_send(MQTT_TOPIC_TEMP, temp);
				sensors_send(MQTT_TOPIC_HUMIDITY, hum);
			}
		}
		/* Just assume that the 0 connection is the MQTT one */
		if (!uip_conn_active(0) && clock_time_seconds() % 10 == 0)
			nethandler_umqtt_init(&mqtt);
		if (mqtt.state != UMQTT_STATE_CONNECTED)
			led_on();
	}
	return 0;
}

ISR(INT0_vect)
{
	flag_packet_rx = true;
}
