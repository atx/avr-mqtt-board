
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "umqtt/umqtt.h"
#include "nethandler.h"

#define _STR(x) #x
#define STR(x) _STR(x)

#define F_CPU		12500000UL

#define IPADDR0		192
#define IPADDR1		168
#define IPADDR2		10
#define IPADDR3		156

#define ETHADDR0		0x00
#define ETHADDR1		0xbd
#define ETHADDR2		0x3b
#define ETHADDR3		0x33
#define ETHADDR4		0x05
#define ETHADDR5		0x71

#define ENC28J60_SPI_PORT		PORTB
#define ENC28J60_SPI_DDR		DDRB
#define ENC28J60_SPI_SCK		PB5
#define ENC28J60_SPI_MOSI		PB3
#define ENC28J60_SPI_MISO		PB4
#define ENC28J60_SPI_SS			PB2
#define ENC28J60_CONTROL_PORT	PORTB
#define ENC28J60_CONTROL_DDR	DDRB
#define ENC28J60_CONTROL_CS		PB2

#define MQTT_IP0				192
#define MQTT_IP1				168
#define MQTT_IP2				10
#define MQTT_IP3				21

#define MQTT_KEEP_ALIVE			30
#define MQTT_CLIENT_ID			"avr-mqtt-" STR(IPADDR3)
#define MQTT_TOPIC_DATE			"time/date"
#define MQTT_TOPIC_DATETIME		"time/daytime"
#define MQTT_TOPIC_TEMP			"sensors/temperature/frontdoor"
#define MQTT_TOPIC_HUMIDITY		"sensors/humidity/frontdoor"
#define MQTT_TOPIC_WEATHER		"weather/local/general"

typedef struct nethandler_state uip_tcp_appstate_t;
#define UIP_APPCALL nethandler_umqtt_appcall

#endif /*__CONFIG_H__*/
