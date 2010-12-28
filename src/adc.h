#ifndef QROLLE_ADC_H
#define QROLLE_ADC_H

// Analog-to-digital converter helper functions
//
// Author(s):
// Antti Nilakari <anilakar@cc.hut.fi>
//
// Version history:
// 2009-08-09 Initial version / AN


#include <avr/io.h>


//! \short Init the ADC
void adc_init()
{
	// ADLAR == left-adjusted (8-bit accuracy with ADCH only)
	// MUX[3..0] == ADC input pin
	ADMUX = (1 << ADLAR) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3);
}

//! \short Get a single 8-bit reading from the ADC
//! \return uint8_t, where 0x00 = GND and 0xFF = V_ref
uint8_t adc_getval_8bit()
{
	return ADCH;
}



#endif // QROLLE_ADC_H