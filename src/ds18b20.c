/* 
 * Driver for the DS18B20
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

#include <stdbool.h>
#include <util/delay.h>

#include "common.h"
#include "ds18b20.h"

#define ds18b20_drive(ds)	\
	do {					\
		gpio_out(&ds->gpio);	\
		gpio_low(&ds->gpio);	\
	} while (0)

#define ds18b20_release(ds)	\
	do {					\
		gpio_in(&ds->gpio);		\
		gpio_high(&ds->gpio);	\
	} while (0)

static void ds18b20_write(struct ds18b20 *ds, bool b)
{
	ds18b20_drive(ds);
	if (b) {
		_delay_us(1);
		ds18b20_release(ds);
		_delay_us(59);
	} else {
		_delay_us(60);
		ds18b20_release(ds);
	}
}

static bool ds18b20_read(struct ds18b20 *ds)
{
	bool ret;

	ds18b20_drive(ds);
	_delay_us(1);
	ds18b20_release(ds);
	_delay_us(14);
	ret = gpio_value(&ds->gpio);
	_delay_us(45);
	return ret;
}

static uint8_t ds18b20_read_byte(struct ds18b20 *ds)
{
	uint8_t ret = 0;
	int i;

	times(8, i)
		ret |= ds18b20_read(ds) << i;

	return ret;
}

static void ds18b20_write_byte(struct ds18b20 *ds, uint8_t data)
{
	int i;

	times(8, i) {
		ds18b20_write(ds, data & _BV(i));
		_delay_us(1);
	}
}

signed long ds18b20_read_temp(struct ds18b20 *ds)
{
	signed long ret;

	ds18b20_reset(ds);
	ds18b20_write_byte(ds, 0xcc);
	ds18b20_write_byte(ds, 0xbe); /* Read memory */
	ret = ds18b20_read_byte(ds);
	ret |= ds18b20_read_byte(ds) << 8;
	ds18b20_reset(ds); /* We really do not care about the rest of the memory */

	return ret * 625 / 10;
}

void ds18b20_convert(struct ds18b20 *ds)
{
	ds18b20_reset(ds);

	ds18b20_write_byte(ds, 0xcc); /* Skip ROM */
	ds18b20_write_byte(ds, 0x44); /* Convert */
}

bool ds18b20_reset(struct ds18b20 *ds)
{
	bool ret;

	ds18b20_drive(ds);
	_delay_us(480);
	ds18b20_release(ds);
	_delay_us(60);
	ret = gpio_value(&ds->gpio);
	_delay_us(420);
	return !ret; /* Low value - driven by ds18b20 */
}

