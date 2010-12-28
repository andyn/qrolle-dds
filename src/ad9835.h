#ifndef QROLLE_AD9835_H
#define QROLLE_AD9835_H

// Functions for controlling the AD9835 chip on QROlle DDS board.
// Implemented in the headers to let the compiler optimize code better.
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// 2009-03-01 initial version / AN
// 2009-07-05 added more macros. Almost all commands are implemented / AN
 

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h> 
#include <util/delay.h>
 
 
// Port and pins for the DDS chip
#define AD_PORT     PORTB
#define AD_PORT_DIR DDRB
#define AD_SCLK     (1 << 2)
#define AD_SDATA    (1 << 1)
#define AD_FSYNC    (1 << 0)


// Typedefs for frequency words and command words
typedef uint32_t freqword_t;
typedef uint16_t cmdword_t;


// Commands

// 8-bit commands write to the defer register. 16-bit commands write 8 bits
// to the actual register and the 8 previously written bits from the
// defer register. Refer to the AD9835 datasheet for more info.
#define AD_PHASE16BIT  0x0000
#define AD_PHASE8BIT   0x1000
#define AD_FREQ16BIT   0x2000
#define AD_FREQ8BIT    0x3000

// Choose the phase and frequency registers when SELSRC = 1. Use either
// PHASE, FREQ or BOTH with zero or more of PSEL0, PSEL1 and FSELECT.
#define AD_SEL_PHASE_REG 0x4000
#define AD_SEL_FREQ_REG  0x5000
#define AD_SEL_BOTH_REG  0x6000

#define AD_SEL_PSEL0     0x0200
#define AD_SEL_PSEL1     0x0400
#define AD_SEL_FSELECT   0x0800

// Set the chip into test mode. Undocumented.
#define AD_TESTMODE 0x7000


// DDS control

// SYNC and SELSRC. Use AD_SOURCE with any of the two other masks. Not setting
// a bit clears it.
// - Setting SELSRC allows user to choose current register
// with commands, otherwise hardware pins are used.
// - Setting SYNC reads PSEL0 / PSEL1 / FSELECT only when MCLK goes up
// but delays the operation 2 cycles, otherwise the registers are selected
// immediately
#define AD_SOURCE 0x8000

#define AD_SYNC   0x2000
#define AD_SELSRC 0x1000

// Sleep, Reset and/or Clear. Use AD_CTRL with any of the three masks
// below. Not setting a bit will clear it. AD_CLR is cleared
// automagically after it has been set.
// - AD_SLEEP shuts down output.
// - AD_RESET resets phase accumulator to zero.
// - AD_CLR clears the two flags above, then clears itself.
#define AD_CTRL  0xC000

#define AD_SLEEP 0x2000 // Power down the DDS until AD_SLEEP is unset.
#define AD_RESET 0x1000 // Reset the phase accumulator to zero.
#define AD_CLR   0x0800 // set SYNC & SELSRC to zero.


// Frequency registers

// Frequency register addresses, use by logical ORing
// with AD_CMD_FRQ* and AD_CMD_PHASE* commands above
#define AD_FREG0_LLSB 0x0000 
#define AD_FREG0_HLSB 0x0100
#define AD_FREG0_LMSB 0x0200
#define AD_FREG0_HMSB 0x0300



//! \short Put a 16-bit command to the DDS chip.
//! Big endian.
//! \param word a 16-bit command word
void dds_put_cmd(cmdword_t cmd)
{
	int i;
	
	// take low the FSYNC signal, clock in 16 bits on the falling edge of
	// SCLK and take FSYNC high
	cli();
	AD_PORT &= ~AD_FSYNC;
	for (i = 0; i < 16; ++i)
	{
		if (cmd & 0x8000)
			AD_PORT |= AD_SDATA;
		else
			AD_PORT &= ~AD_SDATA;
		AD_PORT &= ~AD_SCLK;
		_delay_us(15);
		AD_PORT |= AD_SCLK;
		cmd <<= 1;
	}
	AD_PORT |= AD_FSYNC;
	sei();
}


//! \short Put a frequency word to the DDS chip
void dds_put_freq(freqword_t freq)
{
	uint8_t llsb, hlsb, lmsb, hmsb;
	llsb = (freq >> 0) & 0xFF;
	hlsb = (freq >> 8) & 0xFF;
	lmsb = (freq >> 16) & 0xFF;
	hmsb = (freq >> 24) & 0xFF;

	// write using defer registers
	dds_put_cmd(AD_FREQ8BIT | AD_FREG0_LLSB | llsb);
	dds_put_cmd(AD_FREQ16BIT | AD_FREG0_HLSB | hlsb);
	dds_put_cmd(AD_FREQ8BIT | AD_FREG0_LMSB | lmsb);
	dds_put_cmd(AD_FREQ16BIT | AD_FREG0_HMSB | hmsb);
}


//! \short Initialize the DDS chip.
void dds_init(void)
{
	// As specified in the AD9835 datasheet
	
	// Set the DDS to sleep
	dds_put_cmd(AD_CTRL | AD_SLEEP | AD_RESET | AD_CLR);
	
	// Software source selection not needed in this application
	// dds_put_cmd(AD_SOURCE);
	
	// Set the initial frequency
	dds_put_freq(1);
	
	// Return the DDS online
	dds_put_cmd(AD_CTRL);
}


#endif // QROLLE_AD9835_H
