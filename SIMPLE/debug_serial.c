// SPI-derived channel back to the programmer.

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "pd2defs.h"
#include "debug_serial.h"

///////////////////////////////////////////////////////////////
//
// Debug functions.
//
///////////////////////////////////////////////////////////////

//#define DO_NOTHING

void Debug_sendError(PGM_P s) {
	Debug_startMessage(Debug_MSG_CSTRING);
	Debug_sendString_P(s);
	Debug_endMessage();
}


// Fails if we aren't running at F_CPU=1000000
static inline void delay_us() {
	asm volatile ("nop;");
}

static void serialWrite(uint8_t data) {
#ifndef DO_NOTHING
  unsigned char bit = 0;
   for(int i=0; i < 100; ++i)     // Leave a timing gap so we don't lose the message on the host
    	asm volatile("nop;");

  for(bit = 0; bit < 8; bit++)          // Loop through 8 bits
  {  
    if(data & 0x80) _U_SPI_PORT |= _U_DO;   // If bit(7) of "data" is high
    else _U_SPI_PORT &= ~_U_DO;          // if bit(7) of "data" is low
    
     data <<= 1;                             // Shift "data" to the left by one bit
    _U_SPI_PORT |= _U_SCK;                  // Serial Clock Rising Edge 
    
    for(int i=0; i < 100; ++i)	// Very slow so our slow programmer doesn't alias on this signal.
    	asm volatile("nop;");
    	
    _U_SPI_PORT &= ~_U_SCK;              // Serial Clock Falling Edge
    for(int i=0; i < 100; ++i)
    	asm volatile("nop;");
    	
   }
   return;                      // Returns shifted data in value
#endif
}

// Saves our port setup so we can avoid interfering with other uses.
static uint8_t amInUse = 0;
uint8_t oldDDR, oldPORT;
uint8_t sreg;
void Debug_startMessage(Debug_MessageKind kind) {
#ifndef DO_NOTHING
   	 for(int i=0; i < 1000; ++i)     // Leave a timing gap so we don't lose the message on the host
    		asm volatile("nop;");
	
	sreg = SREG;
	cli();
	oldDDR = _U_SPI_DDR;
	oldPORT = _U_SPI_PORT;
	_U_SPI_DDR |= _U_SCK | _U_DO | _U_SSOFFBOARD;
    	_U_SPI_PORT &= ~_U_SCK;              // Serial Clock starts low
	_U_SPI_PORT &= ~_U_DO; 		
 	_U_SPI_PORT &= ~_U_SSOFFBOARD; 	// Lower the SS line, initiating comms.

	amInUse = 1;

	serialWrite(kind);
#endif
}

void Debug_endMessage() {
#ifndef DO_NOTHING
	_U_SPI_PORT |= _U_SSOFFBOARD;	// Raise the SS line now that we're done.  
   	 for(int i=0; i < 100; ++i)
    		asm volatile("nop;");
	_U_SPI_DDR = oldDDR;	// Restore old wire uses.
	_U_SPI_PORT = oldPORT; 
	amInUse = 0;
	SREG = sreg;
#endif
}

void Debug_sendUint8(uint8_t v) {
#ifndef DO_NOTHING
		serialWrite(v);
#endif
}

void Debug_sendString_P(PGM_P s)
{
#ifndef DO_NOTHING
	if (!amInUse) return;
	char c;
	while ((c = pgm_read_byte(s++)) != 0) {
		serialWrite(c);
	}
#endif
}

void Debug_sendString(char *s)
{
#ifndef DO_NOTHING
	if (!amInUse) return;
	while (*s) {
		serialWrite(*s++);
	}
#endif
}

void Debug_sendNumber(int32_t i, int base) {
#ifndef DO_NOTHING
	if (!amInUse) return;
	if (i < 0) {
		serialWrite('-');
		i = -i;
	}
	if (base == 16) {
		serialWrite('0');
		serialWrite('x');
	}
	if (base == 8) {
		serialWrite('0');
	}
	if (i==0) {
		serialWrite('0');
	}
	int32_t scale = 1;
	while (i / scale > 0) {
		scale *= base;
	}
	while (scale > 1) {
		scale /= base;
		int32_t digit = i / scale;
		if (digit < 10) {
			serialWrite('0' + digit);
		} else {
			serialWrite('A' + digit-10);
		}
		i -= digit * scale;
	}
#endif
}