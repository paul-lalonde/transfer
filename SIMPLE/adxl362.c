#include "pd2defs.h"
#include "pd2.h" // For EEPROM_TEMP_* defines
#include "spi.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "adxl362.h"

///////////////////////////////////////////////////////////////
//
// Communications with ADXL362
//
///////////////////////////////////////////////////////////////
static uint8_t sreg;
void ADXL_SPI_start()
{
     	SPI_start();
	sreg = SREG;
	cli();
 	_U_SPI_PORT &= ~_U_SSADXL; 	// Lower SS to start communication.
}

void ADXL_SPI_stop()
{
	SPI_stop();
	_U_SPI_PORT |= _U_SSADXL;                // Raise SS to end communication.
	SREG = sreg;
}

uint8_t ADXL_readbyteregister(uint8_t reg)
{
	ADXL_SPI_start();
	SPI_transferByte(0x0B);
	SPI_transferByte(reg);
	uint8_t ret = SPI_transferByte(0);
	ADXL_SPI_stop();
	
	return ret;
}

void ADXL_setbyteregister(uint8_t reg, uint8_t value)
{
	ADXL_SPI_start();
	SPI_transferByte(0x0A);
	SPI_transferByte(reg);
	SPI_transferByte(value);
	ADXL_SPI_stop();
}

void ADXL_setwordregister(uint8_t reg, uint16_t value)
{
	ADXL_SPI_start();
	SPI_transferByte(0x0A);
	SPI_transferByte(reg);
	SPI_transferByte(value & 0xFF );
	SPI_transferByte((value>>8) & 0xFF );
	ADXL_SPI_stop();
}

uint16_t ADXL_readwordregister(uint8_t reg)
{
	uint16_t value;
	ADXL_SPI_start();
	SPI_transferByte(0x0B);
	SPI_transferByte(reg);
	value = SPI_transferByte(0);
	value |= SPI_transferByte(0) << 8;
	ADXL_SPI_stop();
	return value;
}


static int16_t bias = 350, scale = 65, denom = 1000;

// Temperatures are in 10ths of a degree C
int16_t ADXL_temperature()
{
	int32_t temp = (int16_t)ADXL_readwordregister(0x14); // (1/scale)*denom
//	SEND_VARIABLE("Temperature register ", temp, 10);
	temp += bias;
	temp =  (temp* scale) / denom;
//	SEND_VARIABLE("temp =", temp, 10);
	return temp;
}

// Bias, Scale, and Scale denominator
void ADXL_setBiasAndScale(int16_t b, int16_t s, uint16_t d)
{
	bias = b;
	scale = s;
	denom = d;
}


// CurTemp in 10ths C
// Because we only have one temperature sample we assume that scale doesn't change - it's acutally 0.0025 (of 0.065) std dev.
// This is only called by the test build, as it must happen when the ambient temperature was measured by the programming
// jig.
void ADXL_calibrateTemperature(int16_t curTemp) 
{
	int16_t temp = (int16_t)ADXL_readwordregister(0x14); 
	// Need to evaluate bias and scale, which will require two samples, or an assumption (probably that scale is the same)
	
	// convert curTemp (in 10ths) to adxl
	int16_t ctemp = (int32_t)curTemp * (int32_t)denom / ((int32_t)scale * (int32_t)10);
	bias = ctemp - temp;

//SEND_VARIABLE("temp reg ", temp, 10);
//SEND_VARIABLE("eeprom temp (10ths C) ", curTemp, 10);
//SEND_VARIABLE("eeprom temp (adxl units) ", ctemp, 10);
//SEND_VARIABLE("bias ", bias, 10);
	// Write the bias back
	eeprom_write_word((uint16_t *)EEPROM_TEMP_BIAS, bias);
	eeprom_write_word((uint16_t *)EEPROM_TEMP_SCALE, scale);
	eeprom_write_word((uint16_t *)EEPROM_TEMP_SCALE_DENOM, denom);
}

volatile uint8_t ADXL_isLive = 0;

void ADXL_powerOff()
{
	uint8_t sreg = SREG;
	cli();
	ADXL_isLive = 0;
	ADXL_readbyteregister(0xB); // Clear any pending interrupts
	ADXL_setbyteregister(0x2D, 0); // Standby.
	SREG = sreg;
}

// Sets to just get activity interrupt, if running activity mode
void ADXL_enableIntr() 
{
	uint8_t sreg = SREG;
	cli();
	ADXL_setbyteregister(ADXL_ACT_INACT_CTL, 
		  0
	//	| ADXL_INACT_LOOP
		| ADXL_INACT_INACT_REF
		| ADXL_INACT_INACT_EN
		| ADXL_INACT_ACT_REF
		| ADXL_INACT_ACT_EN);
		
	ADXL_setbyteregister(ADXL_INTMAP2, 
		0
		| ADXL_INTMAP2_INT_LOW
		| ADXL_INTMAP2_INACT
		| ADXL_INTMAP2_ACT);
		
	ADXL_readbyteregister(0xB); // Clear any pending interrupts
	SREG = sreg;
}

void ADXL_measure()
{
	uint8_t sreg = SREG;
	cli();
	ADXL_setbyteregister(0x2D, 2); // Measurement
	ADXL_readbyteregister(0xB); // Clear any pending interrupts
	ADXL_isLive = 1;
	SREG = sreg;
}

