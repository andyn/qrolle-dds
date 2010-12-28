#ifndef QROLLE_INPUTS_H
#define QROLLE_INPUTS_H


// QROlle DDS board version interrupt functions
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// 2009-07-05 initial version / AN


#include <avr/interrupt.h>

#include "util.h"
#include "interrupt.h"


// Button port and pins
#define BUTTON_PORT_IN PIND
#define BUTTON_PORT_OUT PORTD
#define BUTTON_PORT_OUT_DIR DDRD
#define BUTTON_PIN (1 << 4)

// Time in dsecs required for a long button press
#define BUTTON_DELAY 50


// Helper macro
#define BUTTON_DOWN (!(BUTTON_PORT_IN & BUTTON_PIN))


//! \short Atomically read the encoder rotation.
//! \return number of steps the encoder has been turned,
//! negative means counterclockwise
int8_t read_encoder()
{
	int8_t rotation;
	
	cli();
	rotation = encoder_dir;
	encoder_dir = 0;
	sei();
	
	return rotation;
}


//! \short Read the button state.
//! Blocks for given number of deciseconds at maximum.
//! Returns 0 immediately if button is not down.
//! Returns 1 if the button is pressed and released shortly after.
//! Returns 2 if the button is pressed and held down until time runs out.
//! Returns -1 if interrupted by turning the encoder.
//! \param dsecs time in deciseconds required for long press (retval 2).
//! \return 0, 1, 2 or -1 depending on button state.
int8_t read_button(int8_t dsecs)
{
	// Button not down
	if (!BUTTON_DOWN)
		return 0;
	
	while (dsecs > 0)
	{
		// Delay
		_delay_ms(100);
		// Check if button has been raised
		if (!BUTTON_DOWN)
		{
			// debounce for 5 msecs
			_delay_ms(5);
			return 1;
		}
		// Check if encoder has been turned
		if (encoder_dir)
			return -1;
		
		--dsecs;
	}
	
	// Button held down for long enough
	return 2;
}


//! \short Is the button currently down?
inline int8_t button_down()
{
	return BUTTON_DOWN;
}


//! \short Block until the button has been raised
void button_wait(void)
{
	while (BUTTON_DOWN)
		_delay_ms(10);
	_delay_ms(25);
}


#endif // QROLLE_INPUTS_H