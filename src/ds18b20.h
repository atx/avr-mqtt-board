/* 
 * DS18B20 driver header file
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

#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "gpio.h"

struct ds18b20 {
	struct gpio gpio;
};

/* Returns true on presence pulse detected */
bool ds18b20_reset(struct ds18b20 *ds);
/* Converts temperature */
void ds18b20_convert(struct ds18b20 *ds);
/* Returns temperature in mC */
signed long ds18b20_read_temp(struct ds18b20 *ds);

#endif /* __DS18B20_H__ */
