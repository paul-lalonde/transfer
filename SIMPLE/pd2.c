#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "pd2.h"
#include "adxl362.h"
#include "pcf2123.h"
#include "spi.h"
#include "hall.h"
#include "pd2defs.h"
#include "params.h"


#define ABS(X) ( (X)<0 ? -(X) : (X) )

#define CLOCK_EEPROM_WITNESS 0xDE


#define TIMER_DEBUG(X) if (DEBUG_VERBOSITY & SHOW_TIMER_INNERS) { X; }

#include "pd2defs.h"
#include "debug_serial.h"


uint32_t pulseInterval = RF_PULSE_INTERVAL;

///////////////////////////////////////////////////////////////
//
// Activity/inactivity tracker
//
///////////////////////////////////////////////////////////////

// start active
volatile char gInactive = 1;


///////////////////////////////////////////////////////////////
//
// EEPROM-based data
//
//
// Memory map
// 4-0x0B Clock (time at which the program was uploaded)
// 0xC-0xD temperature_bias
// 0xE-0xF temperature_scale
// 0x10-0x11temperature_scale_denominator
// 0x12 temperature (byte, C)
///////////////////////////////////////////////////////////////

void loadConstsFromEEPROM()
{
	// Load date, set to PCF2123
	time_t t;
	eeprom_read_block(&t, (const void *)4, sizeof(t));
	SEND_ERROR("Current time set to");
	PCF_dumpTime(&t);
	PCF_reset();
	_delay_ms(1);                                                                                                                                                                                                                                                                                                                                                                                                                                    
	PCF_setTime(&t);
	
	int16_t bias = (int16_t)eeprom_read_word((const uint16_t *)EEPROM_TEMP_BIAS);
	int16_t scale = (int16_t)eeprom_read_word((const uint16_t *)EEPROM_TEMP_SCALE);
	int16_t denom = (int16_t)eeprom_read_word((const uint16_t *)EEPROM_TEMP_SCALE_DENOM);
//	int16_t temp = (int16_t)eeprom_read_word((const uint16_t *)EEPROM_PROG_TEMP);
//SEND_VARIABLE("EEPROM_PROG_TEMP ",temp,10);
SEND_VARIABLE("EEPROM_TEMP_BIAS ",bias,10);
SEND_VARIABLE("EEPROM_TEMP_SCALE ",scale,10);
SEND_VARIABLE("EEPROM_TEMP_SCALE_DENOM ",denom,10);
	if (scale != -1) ADXL_setBiasAndScale(bias, scale, denom);
}

///////////////////////////////////////////////////////////////
//
// Application accelerometer managment.  Includes interrupt handling 
// for activity monitor.
//
///////////////////////////////////////////////////////////////

/// Catch bad interrupts, instead of rebooting.
ISR(BADISR_vect)
{
	SEND_ERROR("INTERRUPT NOT CAUGHT");
}

uint8_t SetupAccelerometer()
{
	// Turn on power to accelerometer
	ADXL_POWER_DDR |= 1 << ADXL_POWER_PIN;
	ADXL_POWER_PORT |= 1 << ADXL_POWER_PIN;

	// Wait for ADXL to come online.  When it is, this read should be good.
	// If it doesn't come online we'll be sitting flashing the red light.
	_delay_ms(50);
	
	uint8_t id = ADXL_readbyteregister(0x00);
	int i;
	for (i=0; (i < 10)&&(id != 0xAD) ; ++i) {
		id = ADXL_readbyteregister(0x00);
	}
	if (i == 10) {
		SEND_ERROR("ADXL Not responding.");
		RFOn();
		_delay_ms(1000);
		while(1)
			;
		return 0;
	}

	// We're using absolue range here.
	// Soft reset.  Tremendous help in debugging.
	ADXL_setbyteregister(0x1f, 0x52);
	_delay_ms(2);

	// Set link/loop
	ADXL_setLowestDataRate();
	ADXL_enableIntr();  // setup our interrupts
	ADXL_measure();

	return 1;
}

///////////////////////////////////////////////////////////////
//
// Application functionality
//
///////////////////////////////////////////////////////////////

void PD2_init(void)
{
	cli(); // Make sure interrupts are off.

	// Turn off the ADC, which we don't use.
	ADCSRA &= 0x7F;
	// Turn off power to ADC
	PRR &= 0xE;
		
	// Set clock prescaler: divide by 8 to get 1.0MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS1) | (1 << CLKPS0); 

	// Configure MCU pins
	// 
	{
		RF_DDR |= (1 << RF_BIT);
		RF_PORT &= ~(1 << RF_BIT);

		// PCF enable is normally low.
		PCF_CE_DDR |= 1 << PCF_CE_BIT;
		PCF_CE_PORT &= ~(1 << PCF_CE_BIT);

		// The ADXL INT2 is on INT0/B2
		DDRB &= ~(1 << PORTB2); // make it an input
		PORTB |= 1 << PORTB2; // Pull it up.
		MCUCR &= ~3; // Low level triggers INT0
	}

	// Configure watchdog timer
	{
		/* Clear WDRF in MCUSR */
		MCUSR = 0x00;
		/* Write logical one to WDCE and WDE */
		WDTCSR |= (1<<WDCE) | (1<<WDE);
		/* Turn off WDT */
		WDTCSR = 0x00;
	}

	// These constants are set up in programming
	loadConstsFromEEPROM();

	// No longer using the onboard timer: TIM1_init();
	SPI_init();
	Hall_setup();
	PCF_reset();
	SetupAccelerometer();

	// enable INT0 external interrupt.
	GIMSK |= 1 << INT0;

	sei();   //<! Enable global interrupts
}

void RFOn() {
	RF_PORT |= (1 << RF_BIT);
}
void RFOff() {
	RF_PORT &= ~(1 << RF_BIT);
}

void temperature();
/*
#if REPORT_TEMPERATURE
// Scale is pulse interval milliseconds per C
void temperature(int scale) {
	int16_t temp = ADXL_temperature(TEMPERATURE_BIAS,TEMPERATURE_SCALE);
#if DEBUG
	Debug_startMessage(Debug_MSG_CSTRING);
	SEND_STRING("Temperature: ");
	Debug_sendNumber(temp, 10);
	Debug_endMessage();
#endif

	Timer_set(TEMPERATURE_TIMER, 1000); // Check often for testing.
	pulseInterval = RF_PULSE_INTERVAL+temp * scale;
#if DEBUG
	Debug_startMessage(Debug_MSG_CSTRING);
	SEND_STRING("Temperature: ");
	Debug_sendNumber(temp, 10);
	SEND_STRING(" pulseInterval: ");
	Debug_sendNumber(pulseInterval, 10);
	Debug_endMessage();
#endif
}
#endif
*/
