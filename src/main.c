// QROlle DDS board control software
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// 2008-03-01 initial version / AN


#include "ui.h"
#include "settings.h"
#include "interrupt.h"
#include "adc.h"


void io_init()
{
	// IO init

	// PB2 out - AD98xx SCLK
	// PB1 out - AD98xx SDATA
	// PB0 out - AD98xx FSYNC
	DDRB = (1 << 2) | (1 << 1) | (1 << 0);
	
	// FSYNC and SCLK are high when no data is being sent
	PORTB = AD_SCLK | AD_FSYNC;
	
	// PC7 empty
	// PC6 empty
	// PC5 out - LCD r/s
	// PC4 out - LCD enable
	// PC3 out - LCD data4
	// PC2 out - LCD data5
	// PC1 out - LCD data6
 	// PC0 out - LCD data7
	DDRC = (1 << 5) | (1 << 4) | (1 << 3) |
	       (1 << 2) |(1 << 1) |(1 << 0);
	
	// PD7 empty
	// PD6 empty
	// PD5 empty
	// PD4 in - encoder button
	// PD3 in - encoder rotation / INT1
	// PD2 in - encoder rotation / INT0
	// PD1 out - band selection relay, USART TX
	// PD0 empty, USART RX
	DDRD = (1 << 1);
	
	// Internal pull-up enabled
	PORTD = (1 << 4) | (1 << 3) | (1 << 2);
}


int main(void)
{
	io_init();
	interrupt_init();

	// Start a new running UI
	ui_t ui;
	ui_new(&ui);
	ui_run(&ui);
}

