#ifndef QROLLEDDS_UI_H
#define QROLLEDDS_UI_H


// Encoder and button driven user interface for QROlle DDS.
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// Version history:
// 2009-04-15 Initial version / AN


#include <util/delay.h>
#include <stddef.h>
#include <avr/eeprom.h>

#include "lcd.h"
#include "radio.h"
#include "inputs.h"
#include "adc.h"

// Hardcoded number of supported VFOs and steps.
#define NUM_VFOS 2
#define NUM_STEPS 7

// Number of physical steps the encoder has to be turned in slow (step) mode
// to count as one logical step
#define SLOW_TRESHOLD 5


#define UI_FIRSTLINE (1 << 0)
#define UI_SECONDLINE (1 << 1)


#define UI_MAGIC_NUM 124


typedef struct step_s
{
	const char *name;
	freq_t step;
} step_t;


// Struct for UI settings
typedef struct ui_s
{
	int8_t magic_num;
	int8_t vfo;
	int8_t usb[NUM_VFOS];
	freq_t freq[NUM_VFOS];
	int8_t step[NUM_VFOS];
	int8_t smeter;
} ui_t;


const step_t steps[NUM_STEPS] =
{
	{"    U/L", 0},
	{"  10 Hz", 10},
	{" 100 Hz", 100},
	{"  1 kHz", 1000},
	{" 10 kHz", 10000},
	{"100 kHz", 100000},
	{"  1 MHz", 1000000}
};


ui_t EEMEM eeprom_settings_addr;


//! \short Print the first line.
//! Consists of frequency, sideband and vfo indicators
void ui_freqline(const freq_t *freq, int8_t usb, int8_t vfo)
{
	lcd_home();

	// Frequency, with two dots
	char buf[8];
	int_to_str(buf, 8, *freq);
	for (size_t i = 0; i < 7; ++i)
	{
		lcd_putchar(buf[i]);
		if (i == 1 || i == 4)
			lcd_putchar('.');
	}
	
	// Sideband indicator
	lcd_putchar(' ');
	if (usb)
		lcd_putchar('U');
	else
		lcd_putchar('L');
	
	// VFO
	lcd_puts(" VFO");
	lcd_putchar('A' + vfo);
}


//! \short Draw the S-meter, which is 8 chars wide.
void ui_smeter(uint8_t s_meter)
{
	// Draw the meter in 3-bar blocks
	// There are a total of 3 bars/char * 8 chars = 24 bars.
	uint16_t numblocks = s_meter * 24 / 256 + 1;
	for (int8_t i = 0; i < 8; ++i)
	{
		uint8_t decrement = numblocks;
		if (numblocks > 3)
			decrement = 3;
		lcd_putchar(decrement);
		numblocks -= decrement;
	}
}


//! \short Print the second line.
//! S-meter and step.
void ui_smeterline(uint8_t s_meter, const char *step)
{
	// S-meter
	lcd_row2();
	ui_smeter(s_meter);
	
	// Step indicator
	lcd_putchar(' ');
	lcd_puts(step);
}


void ui_draw(ui_t *ui, uint8_t lines)
{
	if (lines & UI_FIRSTLINE)
	{
		ui_freqline(&ui->freq[ui->vfo], ui->usb[ui->vfo], ui->vfo);
	}
	if (lines & UI_SECONDLINE)
	{
		ui_smeterline(ui->smeter, steps[ui->step[ui->vfo]].name);
	}
}


//! \short Handle the encoder rotation
//! \param rotation the number of steps the encoder has been turned.
//! Negative means counterclockwise.
int8_t encoder_turned(ui_t *ui, int8_t rotation, int8_t button_down)
{
	// These will be used later
	int8_t vfo = ui->vfo;
	freq_t step = steps[ui->step[vfo]].step;

	// Change step
	if (button_down)
	{
		// Some hysteresis
		if (rotation > -SLOW_TRESHOLD && rotation < SLOW_TRESHOLD)
			return rotation;
		
		// Change step
		if (rotation < 0)
		{
			--ui->step[vfo];
			if (ui->step[vfo] < 0)
				ui->step[vfo] = NUM_STEPS - 1;
		}
		else
		{
			++ui->step[vfo];
			if (ui->step[vfo] >= NUM_STEPS)
				ui->step[vfo] = 0;
		}

		ui_draw(ui, UI_SECONDLINE);
	}

	// Change freq or sb
	else
	{
		if (step)
		{
			if (rotation < 0)
			{
				ui->freq[ui->vfo] -= step;
				if (ui->freq[ui->vfo] < 100000)
					ui->freq[ui->vfo] = 100000;
			}
			else
			{
				ui->freq[ui->vfo] += step;
			}
		}
		else
		{
			ui->usb[vfo] = !ui->usb[vfo];
		}
		
		ui->freq[vfo] = radio_setfreq(ui->freq[vfo], ui->usb[vfo]);
		ui_draw(ui, UI_FIRSTLINE);
	}

	// really did something, position reset.
	return 0;
}


//! \short Handle short button presses for the UI
void button_shortpress(ui_t *ui)
{
	// Grow VFO number until it overflows
	++ui->vfo;
	if (ui->vfo >= NUM_VFOS)
		ui->vfo = 0;
		
	ui->freq[ui->vfo] = radio_setfreq(ui->freq[ui->vfo], ui->usb[ui->vfo]);
	ui_draw(ui, UI_FIRSTLINE | UI_SECONDLINE);
}


//! \short Handle long button presses for the UI
void button_longpress(ui_t *ui)
{
	eeprom_write_block(ui, &eeprom_settings_addr, sizeof(ui_t));
	lcd_row2();
	lcd_puts("-Settings saved-");
}


void ui_new(ui_t *ui)
{
	eeprom_read_block(ui, &eeprom_settings_addr, sizeof(ui_t));

	if (ui->magic_num != UI_MAGIC_NUM)
	{
		ui->magic_num = UI_MAGIC_NUM;
		ui->vfo = 0;
		ui->usb[0] = 0;
		ui->usb[1] = 1;
		ui->freq[0] = 3699000;
		ui->freq[1] = 14267000;
		ui->step[0] = 2;
		ui->step[1] = 2;
	}
	
	// initialize everything
	lcd_init();
	adc_init();
	radio_init();
	radio_setfreq(ui->freq[ui->vfo], ui->usb[ui->vfo]);
	
	const unsigned char zero[] = {21, 00, 00, 00, 00, 00, 21, 00};
	const unsigned char one[] = {21, 16, 16, 16, 16, 16, 21, 00};
	const unsigned char two[] = {21, 20, 20, 20, 20, 20, 21, 00};
	const unsigned char three[] = {21, 21, 21, 21, 21, 21, 21, 00};
	lcd_custom_char(zero, 0);
	lcd_custom_char(one, 1);
	lcd_custom_char(two, 2);
	lcd_custom_char(three, 3);
	lcd_cmdmode();
	
}


//! \short Run the UI loop until shut down
void ui_run(ui_t *ui)
{
	ui_draw(ui, UI_FIRSTLINE | UI_SECONDLINE);

	// local step counter and button state
	int8_t rotation = 0;
	int8_t button_state = 0;
	int8_t held_down = 0;
	
	// s-meter values
	int8_t smeter_last = 0;
	int8_t smeter_curr = 0;
	
	// Loop until the device gets shut down
	while(1)
	{
		// Normal state
		if (!held_down)
		{
			// Read the global encoder rotation var into the local one,
			// and act accordingly if needed
			rotation += read_encoder();
			if (rotation)
			{
				rotation = encoder_turned(ui, rotation, 0);
			}
			
			button_state = read_button(BUTTON_DELAY);
			if (button_state)
			{
				if (button_state == -1)
				{
					held_down = 1;
				}
				if (button_state == 1)
				{
					button_shortpress(ui);
				}
				if (button_state == 2)
				{
					button_longpress(ui);
					button_wait();
					ui_draw(ui, UI_SECONDLINE);
				}
				button_state = 0;		
			}
		}
			
		// Button down + encoder turned == special state until button up
		else
		{
			rotation += read_encoder();
			if (rotation)
			{
				rotation = encoder_turned(ui, rotation, 1);
			}
			if (!button_down())
			{
				held_down = 0;
				rotation = 0;
			}
		}
		
		smeter_curr = adc_getval_8bit();
		if (smeter_curr != smeter_last)
		{
			ui->smeter = smeter_last = smeter_curr;
			ui_draw(ui, UI_SECONDLINE);
		}
	}
}


#endif // QROLLEDDS_UI_H
