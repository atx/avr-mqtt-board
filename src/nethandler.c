/* 
 * This fil handles various network related tasks
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

#include <avr/interrupt.h>
#include <string.h>

#include "common.h"
#include "uip.h"
#include "uip_arp.h"
#include "network.h"

#define BUF (((struct uip_eth_hdr *)&uip_buf[0]))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

void nethandler_rx()
{
	uip_len = network_read();

	if (uip_len > 0) {
		switch (ntohs(BUF->type)) {
		case UIP_ETHTYPE_IP:
			uip_arp_ipin();
			uip_input();
			if (uip_len > 0) {
				uip_arp_out();
				network_send();
			}
		break;
		case UIP_ETHTYPE_ARP:
			uip_arp_arpin();
			if (uip_len > 0)
				network_send();
		break;
		}
	}
}

void nethandler_periodic()
{
	int i;

	times(UIP_CONNS, i) {
		uip_periodic(i);
		if (uip_len > 0) {
			uip_arp_out();
			network_send();
		}
	}
}

void nethandler_periodic_arp()
{
	uip_arp_timer();
}

void nethandler_umqtt_init(struct umqtt_connection *conn)
{
	struct uip_conn *uc;
	uip_ipaddr_t ip;

	uip_ipaddr(&ip, MQTT_IP0, MQTT_IP1, MQTT_IP2, MQTT_IP3);
	uc = uip_connect(&ip, htons(1883));
	if (uc == NULL)
		return;

	umqtt_init(conn);
	umqtt_circ_init(&conn->txbuff);
	umqtt_circ_init(&conn->rxbuff);

	umqtt_connect(conn, MQTT_KEEP_ALIVE, MQTT_CLIENT_ID);
	umqtt_subscribe(conn, MQTT_TOPIC_DATE);
	umqtt_subscribe(conn, MQTT_TOPIC_DATETIME);
	umqtt_subscribe(conn, MQTT_TOPIC_WEATHER);

	uc->appstate.conn = conn;
}

void nethandler_umqtt_appcall()
{
	struct umqtt_connection *conn = uip_conn->appstate.conn;
	uint8_t buff[uip_mss() > (unsigned int) conn->txbuff.datalen ?
		(unsigned int) conn->txbuff.datalen : uip_mss()];
	int ret;

	if (uip_newdata()) {
		umqtt_circ_push(&conn->rxbuff, uip_appdata, uip_datalen());
		umqtt_process(conn);
	}

	if (uip_poll() || uip_acked()) {
		ret = umqtt_circ_pop(&conn->txbuff, buff, sizeof(buff));
		if (!ret)
			return;
		uip_send(buff, ret);
	}
}
