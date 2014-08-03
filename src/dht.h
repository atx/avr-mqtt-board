/* 
 * Header file for the DHT11 driver
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

#ifndef __DHT_H__
#define __DHT_H__

#include "gpio.h"

struct dht {
	struct gpio gpio;
};

/* Returns 0 - 100 on correct read, < 0 on error */
int dht_humidity(struct dht *dht);

#endif /* __DHT_H__ */
