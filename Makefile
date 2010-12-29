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
RMDIR = rmdir
MKDIR = mkdir
CFLAGS = -std=c99 -pedantic -Wall -Wextra -DF_CPU=4000000UL -DREVISION=$(REVISION) -mmcu=atmega8 -Os

# Source files
SRCS = src/main.c
# Output directory and binary name
BUILD = bin
TARGET = qrolle
BUILD_TARGET = $(BUILD)/$(TARGET)

.PHONY: all debug clean

all: $(BUILD)
	$(CC) $(CFLAGS) $(FREQ_MATH) -o $(BUILD_TARGET).elf $(SRCS)
	$(OBJCOPY) -j .text -j .data -O ihex $(BUILD_TARGET).elf $(BUILD_TARGET).hex
	$(SIZE) --mcu=atmega8 $(BUILD_TARGET).hex

debug: $(BUILD)
	$(CC) $(CFLAGS) -g -o $(BUILD_TARGET)-debug.elf $(SRCS)

$(BUILD):
	$(MKDIR) $(BUILD)

clean:
	-$(RM) $(BUILD_TARGET).elf
	-$(RM) $(BUILD_TARGET)-debug.elf
	-$(RM) $(BUILD_TARGET).hex
	-$(RMDIR) $(BUILD)

