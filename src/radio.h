#ifndef QROLLE_RADIO_H
#define QROLLE_RADIO_H

// Functions for controlling for controlling the radio part as a whole
//
// Author(s):
// Antti Nilakari <anilakar@cc.hut.fi>
//
// Version history:
// 2009-07-05 initial version / AN


#include "ad9835.h"
#include "freq.h"


// Minimum frequency for using 20m band relays
#define FREQ_20M_LOW 10000000


//! \short Select the correct band.
//! \param 0 for 80m, 1 for 20m
inline void set_band_20m(int8_t band)
{
	if (band)
		BAND_SEL_PORT |= BAND_SEL_PIN;
	else
		BAND_SEL_PORT &= ~BAND_SEL_PIN;
}


freq_t radio_setfreq(freq_t freq, int8_t usb)
{	
	if (freq < 100000L)
		freq = 100000L;
	else if (freq > 20000000L)
		freq = 20000000L;

	// Calculate the VFO frequency from the RX freq
	// LSB reception needs f_vfo > f_if
	// USB reception needs f_vfo < f_if
	freq_t vfo_freq = freq_vfo(freq, usb);
	
	// Calculate the frequency word that is sent to AD9835
	freqword_t freqword = freq_mul(vfo_freq);
	
	// Upload the frequency word to AD9835
	dds_put_freq(freqword);
	
	// Set the band relay according to frequency
	if (freq > FREQ_20M_LOW)
		set_band_20m(1);
	else
		set_band_20m(0);
		
	return freq;
}


void radio_init(void)
{
	dds_init();
}


#endif // QROLLE_RADIO_H