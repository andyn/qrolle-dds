#ifndef QROLLE_LCD_H
#define QROLLE_LCD_H


// HD44780 LCD control functions for QROlle DDS board
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// 2008-03-01 initial version / AN


#include <util/delay.h>

#include "util.h"

 
// Port and pin definitions
#define LCD_DATA_PORT     PORTC
#define LCD_DATA_PORT_DIR DDRC 
#define LCD_DATA4         (1 << 3)
#define LCD_DATA5         (1 << 2)
#define LCD_DATA6         (1 << 1)
#define LCD_DATA7         (1 << 0)
#define LCD_DATA_PINS     (LCD_DATA4 | LCD_DATA5 | LCD_DATA6 | LCD_DATA7)

#define LCD_CTRL_PORT     PORTC
#define LCD_CTRL_PORT_DIR DDRC
#define LCD_EN            (1 << 4)
#define LCD_RS            (1 << 5)


// DDRAM addresses are used to print characters to to the display
#define LCD_DDRAM     0x80

// Address for row 2
#define LCD_ROW2	  0x40

// CGRAM (character generator ram) addresses are used to upload custom
// characters.
#define LCD_CGRAM     0x40


//! \short Put a nibble to the display
//! \param byte a byte whose upper 4 bits will be used
void lcd_putnibble(unsigned char byte)
{
	unsigned char nibble = 0;

	// The bits need to be reversed because of the unusual wiring
	if (byte & 128)
		nibble |= LCD_DATA7;
	if (byte & 64)
		nibble |= LCD_DATA6;
	if (byte & 32)
		nibble |= LCD_DATA5;
	if (byte & 16)
		nibble |= LCD_DATA4;
		
	// set (other bits | data bits)
	LCD_DATA_PORT = (LCD_DATA_PORT & ~LCD_DATA_PINS) |
	                (nibble & LCD_DATA_PINS);

	// Cycle the EN pin, the display should read in the data
	LCD_CTRL_PORT |= LCD_EN;
	_delay_ms(0.5);
	LCD_CTRL_PORT &= ~LCD_EN;
}


//! \short Print a character on the display
//! The character is printed in the current cursor position.
//! Cursor movement is dependent on additional configuration.
//! \param byte a character to print
void lcd_putchar(unsigned char byte)
{	
	LCD_CTRL_PORT |= LCD_RS;
	lcd_putnibble(byte);
	lcd_putnibble(byte << 4);
}


//! \short Send a raw command to the display.
//! Consult HD44780 documentation for additional instructions.
//! \param byte A raw command byte
void lcd_putcmd(unsigned char byte)
{
	LCD_CTRL_PORT &= ~LCD_RS;
	lcd_putnibble(byte);
	lcd_putnibble(byte << 4);
}


//! \short Clear the display.
//! The cursor is moved to home position.
inline void lcd_clr()
{
	lcd_putcmd(0x01);
}


//! \short Move the cursor to home position.
//! Faster than lcd_clr().
inline void lcd_home()
{
	lcd_putcmd(0x02);
}


//! \short Move the cursor to row2
inline void lcd_row2()
{
	lcd_putcmd(LCD_DDRAM + LCD_ROW2);
}


//! \short Put a string of characters on the display.
//! \param string A null-terminated C string
void lcd_puts(const char *string)
{
	while (*string)
		lcd_putchar(*string++);
}


//! \short Uploads a custom character to the display.
//! The display is left in CHGEN mode.
//! \param pixels A pointer to a const array of eight unsigned characters
//! whose 5 LSBs are used to determine which pixels should be lit. Eighth row
//! might or might not be supported.
//! \param slot The CGRAM slot (0-7 by standard) to which the character should be uploaded.
void lcd_custom_char(const unsigned char *pixels, uint8_t slot)
{
	lcd_putcmd(LCD_CGRAM | slot << 3);
	for (unsigned char i = 0; i < 8; ++i)
		lcd_putchar(pixels[i]);
}


//! \short Put the display in DDRAM mode.
inline void lcd_cmdmode()
{
	lcd_putcmd(LCD_DDRAM);
}


//! \short Initialize the display for use.
//! Leaves the display in DDRAM addressing mode.
void lcd_init()
{
	_delay_ms(15);       // 15 ms
	lcd_putnibble(0x30);
	_delay_ms(5);        // 4.1 ms
	lcd_putnibble(0x30); 
	_delay_ms(5);
	lcd_putnibble(0x30);  // 4.1 ms
	lcd_putnibble(0x20);  // set up 4-bit transfer
	lcd_putcmd(0x28);     // Set to use multiple lines
	lcd_putcmd(0x0C);     // display on, no cursor, no blink
}


#endif // QROLLE_LCD_H
