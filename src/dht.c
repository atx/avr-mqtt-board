/* 
 * Driver for DHT11 humidity sensor
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
#include <util/delay.h>

#include "clock-arch.h"
#include "dht.h"

/* TODO: Handle timeouts! */

int dht_humidity(struct dht *d)
{
	int ret = 0;
	int i;

	cli();

	gpio_out(&(d)->gpio);
	gpio_low(&(d)->gpio);
	_delay_ms(20);
	gpio_in(&(d)->gpio);
	gpio_high(&(d)->gpio);

	while (gpio_value(&d->gpio));
	while (!gpio_value(&d->gpio));
	while (gpio_value(&d->gpio));

	for (i = 0; i < 8; i++) {
		while (!gpio_value(&d->gpio))
			_delay_us(1); /* Latch onto the data signal */
		_delay_us(40);
		if (gpio_value(&d->gpio)) {
			ret = (ret << 1) | 1;
			while (gpio_value(&d->gpio))
				_delay_us(1);
		} else {
			ret = ret << 1;
		}
	}

	sei();

	return ret & 0xff;
}
