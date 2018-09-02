#include <avr/interrupt.h>
#include "spi.h"
#include "pd2defs.h"

///////////////////////////////////////////////////////////////
//
// Bit-banged SPI functions.  
//
///////////////////////////////////////////////////////////////

void SPI_start()
{
     	_U_SPI_PORT &= ~_U_SCK;              // Serial Clock starts low
	_U_SPI_PORT &= ~_U_DO; 		// mosi low.
}

void SPI_stop()
{
}

void SPI_init()
{
	_U_SPI_DDR |= _U_SCK | _U_DO;
	_U_SPI_DDR &= ~_U_DI;
	_U_SPI_PORT &= ~_U_DI; // Set it tri-state - let it float when not otherwise in use.  Other device might be driving.
	_U_SPI_DDR |= _U_SSADXL;
	PCF_CE_DDR |= 1 << PCF_CE_BIT; 
	PCF_CE_PORT &= ~(1 << PCF_CE_BIT); // Active high, unlike usual SPI convention, so drop it low.
	SPI_stop();
}

void SPI_done() 
{
}

unsigned char SPI_transferByte(unsigned char data)
{
#if 1
  unsigned char bit = 0;

  uint8_t CurrentInterruptState = SREG; // save interrupt state. 
  cli(); // disable interrupts. 

  for(bit = 0; bit < 8; bit++)          // Loop through 8 bits
  {  
    if(data & 0x80) _U_SPI_PORT |= _U_DO;   // If bit(7) of "data" is high
    else _U_SPI_PORT &= ~_U_DO;          // if bit(7) of "data" is low
    _U_SPI_PORT |= _U_SCK;                  // Serial Clock Rising Edge 

    data <<= 1;                             // Shift "data" to the left by one bit
    if(_U_SPI_PIN & _U_DI) data |= 0x01;    // If bit of slave data is high
    else data &= ~0x01;                  // if bit of slave data is low
    _U_SPI_PORT &= ~_U_SCK;              // Serial Clock Falling Edge
  }

  SREG = CurrentInterruptState; // restore interrupts.
  return data;                      // Returns shifted data in value
#else
  uint8_t CurrentInterruptState = SREG; // save interrupt state. 
  cli(); // disable interrupts. 
  USICR = (1<<USIWM0)|(0<<USICS1)|(1<<USICLK)|(1<<USITC);
  USIDR = data;
  USISR = ( 1 << USIOIF);
  while ( !(USISR & ( 1 << USIOIF)) ) {
  	USICR |= (1<<USIWM0)|(0<<USICS1)|(1<<USICLK)|(1<<USITC);
  }

  SREG = CurrentInterruptState; // restore interrupts.
  return USIDR;
#endif

} 

unsigned char SPI_transferByte_switched(unsigned char data)
{
#if 1
  unsigned char bit = 0;

  uint8_t CurrentInterruptState = SREG; // save interrupt state. 
  cli(); // disable interrupts. 

  for(bit = 0; bit < 8; bit++)          // Loop through 8 bits
  {  
    if(data & 0x80) _U_SPI_PORT |= _U_DI;   // If bit(7) of "data" is high
    else _U_SPI_PORT &= ~_U_DI;          // if bit(7) of "data" is low
    _U_SPI_PORT |= _U_SCK;                  // Serial Clock Rising Edge 

    data <<= 1;                             // Shift "data" to the left by one bit
    if(_U_SPI_PIN & _U_DO) data |= 0x01;    // If bit of slave data is high
    else data &= ~0x01;                  // if bit of slave data is low
    _U_SPI_PORT &= ~_U_SCK;              // Serial Clock Falling Edge

  }

  SREG = CurrentInterruptState; // restore interrupts.
  return data;                      // Returns shifted data in value
#else
  uint8_t CurrentInterruptState = SREG; // save interrupt state. 
  cli(); // disable interrupts. 
  USICR &= ~(_BV(USISIE) | _BV(USIOIE) | _BV(USIWM1));
  USICR |= _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
  USIDR = data;
  USISR = ( 1 << USIOIF);
  while ( !(USISR & ( 1 << USIOIF)) ) {
  	USICR |= 1<<USITC;
  }

  SREG = CurrentInterruptState; // restore interrupts.
  return USIDR;
#endif
}
