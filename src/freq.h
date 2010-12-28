#ifndef QROLLE_FREQ_H
#define QROLLE_FREQ_H


// QROlle DDS board frequency related headers
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// 2008-03-01 initial version / AN


#include <inttypes.h>
#include <avr/io.h>

#include "freq.h"
#include "ad9835.h"
#include "util.h"


// Frequencies are represented as a signed number with
// unity equal to one hertz
typedef int32_t freq_t;


// Band selection relay control pin. High == 20 metres, Low == 80 metres
#define BAND_SEL_PORT PORTD
#define BAND_SEL_PIN  (1 << 1)

// AD9835 reference XTAL frequency
#define FREQ_XTAL_REF 50000000.0

// Intermediate frequency
#define FREQ_IF 5000000

// The lowest frequency at which the USB/20m filters are used
#define FREQ_USB_LOW 10000000

//! \short Calculate correct frequency word
//! Multiply the frequence to get the desired frequency word
freqword_t freq_mul(freq_t freq)
{
    const double freq_coef = 0xFFFFFFFF / FREQ_XTAL_REF;
    // const double freq_coef = 85.89934592
    const int32_t coef_table[] = {
        freq_coef,
        freq_coef * 10L,
        freq_coef * 100L,
        freq_coef * 1000L,
        freq_coef * 10000L,
        freq_coef * 100000L,
        freq_coef * 1000000L,
        freq_coef * 10000000L
    };

    freqword_t freqword = 0;
    for (unsigned char i = 0; i < 8; ++i)
    {
        int digit = freq % 10;
        freqword += digit * coef_table[i];
        // freq = (freq - digit) / 10;
		// int division
		freq /= 10;
    }
    
	// Debug.
	// Print the raw frequency word on the screen.
	/*
	char bufasd[17] = {0};
	int_to_str(bufasd, 15, freqword);
	lcd_row2();
	lcd_puts(bufasd);
	*/
	
    return freqword;
}


//! \short Calculate required VFO frequency for given radio frequency
inline freq_t freq_vfo(freq_t freq, int usb)
{
	if (usb)
		return freq - FREQ_IF;
	else
		return freq + FREQ_IF;
}




#endif // QROLLE_FREQ_H
