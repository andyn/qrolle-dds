# Makefile for DDS-QROlle
#
# Authors:
# Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
#
# 2008-03-01 initial version / AN
# 2010-12-27 debug binary separated from production binary / AN

# Revision number
REVISION = 1

# Options
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
RM = rm
CFLAGS = -std=c99 -pedantic -Wall -Wextra -DF_CPU=4000000UL -DREVISION=$(REVISION) -mmcu=atmega8 -Os

# Source files
SRCS = src/main.c
# Output binary file
TARGET = bin/qrolle

.PHONY: all debug clean

all:
	$(CC) $(CFLAGS) $(FREQ_MATH) -o $(TARGET).elf $(SRCS)
	$(OBJCOPY) -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	$(SIZE) --mcu=atmega8 $(TARGET).hex

debug:
	$(CC) $(CFLAGS) -g -o $(TARGET)-debug.elf $(SRCS)

clean:
	-$(RM) $(TARGET).elf
	-$(RM) $(TARGET)-debug.elf
	-$(RM) $(TARGET).hex
