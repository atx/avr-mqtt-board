/* 
 * GPIO utilities
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

#ifndef __GPIO_H__
#define __GPIO_H__

struct gpio {
	volatile unsigned char *ddr;
	volatile unsigned char *pin;
	volatile unsigned char *port;
	int n;
};

#define GPIO(b, p) \
	{ .ddr = &(DDR##b), .pin = &(PIN##b), .port = &(PORT##b), .n = p }

#define gpio_in(g) \
	(*(g)->ddr &= ~_BV((g)->n))

#define gpio_out(g) \
	(*(g)->ddr |= _BV((g)->n))

#define gpio_high(g) \
	(*(g)->port |= _BV((g)->n))

#define gpio_low(g) \
	(*(g)->port &= ~_BV((g)->n))

#define gpio_toggle(g) \
	(*(g)->port ^= _BV((g)->n))

#define gpio_value(g) \
	(!!(*(g)->pin & _BV((g)->n)))

#endif /* __GPIO_H__ */
