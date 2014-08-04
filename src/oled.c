/* 
 * SSD1306 driver
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

#include <stdarg.h>

#include "common.h"
#include "oled.h"
#include "gpio.h"

#define oled_scl_high(o) \
	gpio_high(&(o)->scl)

#define oled_scl_low(o) \
	gpio_low(&(o)->scl)

#define oled_sda_high(o) \
	gpio_high(&(o)->sda)

#define oled_sda_low(o) \
	gpio_low(&(o)->sda)


static void oled_i2c_start(struct oled *oled)
{
	oled_scl_high(oled);
	oled_sda_high(oled);
	oled_sda_low(oled);
	oled_scl_low(oled);
}

static void oled_i2c_stop(struct oled *oled)
{
	oled_scl_low(oled);
	oled_sda_low(oled);
	oled_scl_high(oled);
	oled_sda_high(oled);
}

static void oled_i2c_write_byte(struct oled *oled, uint8_t byte)
{
	int i;

	times(8, i) { /* From MSB to LSB */
		/* Set bit */
		if ((byte << i) & 0x80)
			oled_sda_high(oled);
		else
			oled_sda_low(oled);
		/* Clock */
		oled_scl_high(oled);
		oled_scl_low(oled);
	}
	/* ACK cycle */
	oled_sda_high(oled);
	oled_scl_high(oled);
	oled_scl_low(oled);
}

static void oled_i2c_start_data(struct oled *oled)
{
	oled_i2c_start(oled);
	oled_i2c_write_byte(oled, 0x78);
	oled_i2c_write_byte(oled, 0x40);
}

void oled_send_command(struct oled *oled, uint8_t command)
{
	oled_i2c_start(oled);
	oled_i2c_write_byte(oled, 0x78); /* Write address */
	oled_i2c_write_byte(oled, 0x00); /* Write command */
	oled_i2c_write_byte(oled, command);
	oled_i2c_stop(oled);
}

void oled_send_command2(struct oled *oled, int c, ...)
{
	va_list args;

	oled_i2c_start(oled);
	va_start(args, c);
	oled_i2c_write_byte(oled, 0x78);
	oled_i2c_write_byte(oled, 0x00);
	for (; c > 0; c--)
		oled_i2c_write_byte(oled, va_arg(args, int));
	oled_i2c_stop(oled);
}

static void oled_limit_write(struct oled *oled, int x, int y, int w, int h)
{
	oled_send_command2(oled, 3, 0x21, x, x + w - 1);
	oled_send_command2(oled, 3, 0x22, y, y + h - 1);
}

void oled_fill_screen(struct oled *oled, uint8_t data)
{
	int i;

	oled_limit_write(oled, 0, 0, 128, 8);

	oled_i2c_start_data(oled);
	times(128 * 8, i)
		oled_i2c_write_byte(oled, data);
	oled_i2c_stop(oled);
}

void oled_write_image(struct oled *oled,
		uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		uint8_t *data)
{
	int i;

	oled_limit_write(oled, x, y, w, h);

	oled_i2c_start_data(oled);
	times(w * h, i)
		oled_i2c_write_byte(oled, data[i]);
	oled_i2c_stop(oled);
}

void oled_write_image_pgm(struct oled *oled,
		uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		const uint8_t *progmem)
{
	int i;

	oled_limit_write(oled, x, y, w, h);

	oled_i2c_start_data(oled);
	times(w * h, i)
		oled_i2c_write_byte(oled, pgm_read_byte(progmem + i));
	oled_i2c_stop(oled);
}

void oled_init(struct oled *oled)
{
	gpio_out(&oled->sda);
	gpio_out(&oled->scl);
	/* Copied from the manufacturers code */
	/* Some of the comments do not seem to correspond to the datasheet */
	oled_send_command(oled, 0xae); /* Display off */
	oled_send_command(oled, 0x00); /* Set memory addressing */
	oled_send_command(oled, 0x10); /* Page memory addressing */
	oled_send_command(oled, 0x40); /* Set page start */
	oled_send_command(oled, 0x81); /* Set COM output scan direction */
	oled_send_command(oled, 0xcf); /* Set low column address */
	oled_send_command(oled, 0xa1); /* Set high column address */
	oled_send_command(oled, 0xc8); /* Set start line address */
	oled_send_command(oled, 0xa6); /* Set contrast control register */
	oled_send_command(oled, 0xa8);
	oled_send_command(oled, 0x3f); /* Set segment re-map 0 to 127 */
	oled_send_command(oled, 0xd3); /* Set normal display */
	oled_send_command(oled, 0x00); /* Set multiplex ratio 1 to 64 */
	oled_send_command(oled, 0xd5);
	oled_send_command(oled, 0x80);
	oled_send_command(oled, 0xd9); /* Set display offset */
	oled_send_command(oled, 0xf1); /* Not offset */
	oled_send_command(oled, 0xda); /* Set clock divider */
	oled_send_command(oled, 0x12); /* Set divider ratio */
	oled_send_command(oled, 0xdb); /* Set precharge */
	oled_send_command(oled, 0x40);
	oled_send_command(oled, 0x20); /* Set com HW config */
	oled_send_command(oled, 0x02);
	oled_send_command(oled, 0x8d); /* Set vcomh */
	oled_send_command(oled, 0x14); /* Some sort of voltage control */
	oled_send_command(oled, 0xa4); /* DC-DC enable */
	oled_send_command(oled, 0xa6);
	oled_send_command2(oled, 2, 0x20, 0x00);
}
