///////////////////////////////////////////////////////////////
//
// Bit-banged SPI functions.  
//
///////////////////////////////////////////////////////////////

#ifndef _SPI_H
#define _SPI_H

#include <avr/io.h>

void SPI_start();
void SPI_stop();
void SPI_init();
void SPI_done();

unsigned char SPI_transferByte(unsigned char data);
unsigned char SPI_transferByte_switched(unsigned char data);

#endif