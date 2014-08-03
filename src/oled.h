/* 
 * Header file for the SSD1306 I2C OLED driver
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

#ifndef OLED_HELTEC_H_
#define OLED_HELTEC_H_

#include <stdbool.h>
#include <avr/pgmspace.h>

#include "gpio.h"

struct oled {
	struct gpio sda;
	struct gpio scl;
};

void oled_init(struct oled *oled);

void oled_send_command(struct oled *oled, uint8_t commmans);
void oled_send_command2(struct oled *oled, int c, ...);

void oled_fill_screen(struct oled *oled, uint8_t data);
void oled_write_frame(struct oled *oled, uint8_t *data);
void oled_write_image(struct oled *oled,
		uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		uint8_t *data);
void oled_write_image_pgm(struct oled *oled,
		uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		const uint8_t *progmem);
void oled_contrast(struct oled *oled, uint8_t con);

#define oled_active(o, b) \
	oled_send_command(o, b ? 0xaf : 0xae)

#define oled_invert(o, b) \
	oled_send_command(o, b ? 0xa7 : 0xa6)

#define oled_contrast(o, n) \
	oled_send_command2(o, 2, 0x81, n);

#define oled_vertical_flip(o, b) \
	oled_send_command(o, (b) ? 0xc0 : 0xc8)

#define oled_vertical_offset(o, c) \
	oled_send_command(o, 0x40 + (c))

#define oled_horizontal_flip(o, b) \
	oled_send_command(o, (b) ? 0xa0 : 0xa1)

#endif /* OLED_HELTEC_H_ */
