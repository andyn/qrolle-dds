#ifndef QROLLE_UTIL_H
#define QROLLE_UTIL_H


// Miscellaneous helper functions. Partly reimplementations of standard library code.
//
// Author(s):
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// Version history:
// 2009-07-03 initial version / AN


#include <avr/io.h>


//! \short Convert an integer to string representation. 
//! Writes into a preallocated string buffer. A pointer to the buffer
//! and the buffer size must be given.
//! \param buf a pointer to the buffer the string is written to
//! \param bufsize size of said buffer
//! \param num the integer to convert to string representation
void int_to_str(char *buf, int bufsize, int32_t num)
{
	for (int i = bufsize - 1; i >= 0; --i)
	{
		// leading zeros padded with space
		if (num == 0)
			buf[i] = ' ';
		
		// filled from the end
		else
			buf[i] = num % 10 + '0';
		
		// next digit
		num /= 10;
	}
}


#endif // QROLLE_UTIL_H