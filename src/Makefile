NAME	= firmware
MCU	= atmega328p
FORMAT	= ihex
TARGET	= $(NAME)

AVRDUDE = avrdude -p$(MCU) -cusbasp -Uflash:w:$(NAME).hex:a

THIRD_PARTY_PATH = ../thirdparty
UIP_PATH = $(THIRD_PARTY_PATH)/avr-uip-2
UMQTT_PATH = $(THIRD_PARTY_PATH)/umqtt

CSRC = main.c clock-arch.c nethandler.c oled.c ds18b20.c dht.c

CSRC += $(UIP_PATH)/uip/uip.c \
		$(UIP_PATH)/uip/uip_arp.c \
		$(UIP_PATH)/uip/psock.c \
		$(UIP_PATH)/uip/uiplib.c \
		$(UIP_PATH)/drivers/enc28j60/network.c \
		$(UIP_PATH)/drivers/enc28j60/enc28j60.c

CSRC += $(UMQTT_PATH)/umqtt/umqtt.c

CINC = -I. \
	   -I$(UIP_PATH)/uip \
	   -I$(UIP_PATH)/drivers/enc28j60 \
	   -I$(UIP_PATH)/drivers/interfaces \
	   -I$(UMQTT_PATH)

CFLAGS	= -include config.h
CFLAGS	+= -include uip-conf.h
CFLAGS	+= -O2
CFLAGS	+= -Wall -Wextra
CFLAGS	+= -std=gnu99
CFLAGS	+= -fpack-struct -fshort-enums -ffunction-sections \
		   -fdata-sections -funsigned-char -funsigned-bitfields
CFLAGS	+= -mmcu=$(MCU)
CFLAGS	+= $(CINC)

LDFLAGS       = -Wl,-Map,$(PRG).map,--gc-sections

CROSS	= avr-
SHELL	= sh
CC		= $(CROSS)gcc
OBJCOPY	= $(CROSS)objcopy
OBJDUMP	= $(CROSS)objdump
SIZE	= $(CROSS)size
NM		= $(CROSS)nm
RM		= rm -f

OBJ	= $(CSRC:.c=.o)
LST	= $(CSRC:.c=.lst)

all: $(NAME).elf hex

$(NAME).elf:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(CSRC)

text: hex bin srec

hex:	$(NAME).hex

avrdude: hex
	$(AVRDUDE)

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	$(RM) *.o *.elf *.hex
	$(RM) $(UIP_PATH)/uip/*.o
	$(RM) $(UIP_PATH)/drivers/enc28j60/*.o
	$(RM) $(UMQTT_PATH)/umqtt/*.o

.PHONY: $(NAME).elf
