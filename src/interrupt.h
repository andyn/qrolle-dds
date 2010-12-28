#ifndef QROLLE_INTERRUPT_H
#define QROLLE_INTERRUPT_H


// QROlle DDS board version interrupt functions
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// 2009-07-05 initial version / AN


#include <avr/interrupt.h>

#include "util.h"

// Encoder port and pins. Note PIN (input, not output)
#define ENCODER_PORT_OUT PORTD
#define ENCODER_PORT_IN  PIND
#define ENCODER_PORT_DIR DDRD
#define ENCODER_CW       (1 << 2) // INT0
#define ENCODER_CCW      (1 << 3) // INT1


// Encoder direction. Zero = not turned, negative = turned counterclockwise,
// positive = turned clockwise. A larger number means more steps.
// Needs to be global & volatile because used with interrupts
volatile char encoder_dir;


void interrupt_init()
{
	// INT0 on rising edge
	MCUCR |= (1 << ISC01) | (1 << ISC00);
	GIMSK |= (1 << INT0);
	
	sei();
}


// Interrupt function for handling encoder movement.
ISR(INT0_vect)
{
    // CCW pin high means that it was up when the interrupt triggered
    if (ENCODER_PORT_IN & ENCODER_CCW)
    {
        ++encoder_dir;
    }
    else
    {
        --encoder_dir;
    }
}


#endif // QROLLE_INTERRUPT_H