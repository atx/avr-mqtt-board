/* 
 * Common utilities
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

#ifndef __COMMON_H__
#define __COMMON_H__


#define ARRAY_SIZE(x) sizeof(x) / sizeof((x)[0])

#define times(m, x) \
	for((x) = 0; (x) < (m); (x)++)

#define iterate(a, x) \
	for((x) = 0; (x) < (ARRAY_SIZE(a)); (x)++)

#endif /* __COMMON_H__ */
