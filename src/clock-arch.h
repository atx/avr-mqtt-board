/* 
 * Header file for clock-arch.c
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

#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

#include <stdint.h>

typedef uint32_t clock_time_t; /* This should serve for some time */
#define CLOCK_CONF_SECOND		(clock_time_t)2

#define clock_time_seconds() (clock_time() / CLOCK_CONF_SECOND)
#define clock_set_seconds(x) clock_set((x) * CLOCK_CONF_SECOND)

void clock_set(clock_time_t t);
clock_time_t clock_time();

#include "clock.h"

#endif /* __CLOCK_ARCH_H__ */
