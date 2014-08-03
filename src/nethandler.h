/* 
 * Header file for nethandler.c
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

#ifndef __NETHANDLER_H__
#define __NETHANDLER_H__

void nethandler_rx();
void nethandler_periodic();
void nethandler_periodic_arp();

void nethandler_umqtt_init();
void nethandler_umqtt_appcall();

struct nethandler_state {
	struct umqtt_connection *conn;
	int slen; /* Length of data currently being sent (fox rxmit) */
};

#endif /* __NETHANDLER_H__ */
