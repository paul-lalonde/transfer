#if TEST_BUILD
#include <util/delay.h>
#include "pd2defs.h"
#include "test_params.h"
#include "pd2.h"
#include "adxl362.h"
#include "hall.h"
#include "pcf2123.h"
#include "debug_serial.h"
#include <avr/eeprom.h>


static inline void delay_ms(uint16_t millis)
{
  while ( millis-- )
	_delay_ms(1);
}

int main() 
{

	SEND_ERROR("Starting up");
	SEND_ERROR("Test mode");

	PD2_init();


	if (!(PINB & (1<<PORTB2))) {
		SEND_ERROR("Interrup line (ATTiny84a pin 14, ADXL362 pin 9, PCF2123 pin 5) is shorted low.");
		Debug_startMessage(Debug_MSG_TARGET_FAIL);
		Debug_endMessage();
		while(1)
			;
	}	
		

#if USE_ACCELEROMETER
	Debug_startMessage(Debug_MSG_PAUSE_SESSION);
	Debug_endMessage();
	uint8_t status = SetupAccelerometer();
	_delay_ms(1000); // For accelerometer setup.

	if (status == 0) {
		SEND_ERROR("Acceleromter Not Responding");
		Debug_startMessage(Debug_MSG_TARGET_FAIL);
		Debug_endMessage();
		while (1)
			;
	} else {
		SEND_ERROR("Accelerometer Good");
	}
#endif


	// Should test PCF

	loadConstsFromEEPROM();

	// Calibrate temperature
	// During programming the jig's temperature was written to EEPROM, in tenths of a C
	SEND_ERROR("Calibrating temperature.");
	uint8_t temp = eeprom_read_byte((const uint8_t *)EEPROM_PROG_TEMP);

	int16_t bias = eeprom_read_word((const uint16_t *)EEPROM_TEMP_BIAS);

	if (bias == -1) {
		SEND_ERROR("Calibrating transmitter thermometer.");
		Debug_startMessage(Debug_MSG_PAUSE_SESSION);
		Debug_endMessage();
		ADXL_measure();
		_delay_ms(200);
		ADXL_calibrateTemperature(temp * 10);
		ADXL_powerOff();
		_delay_ms(500);
	}


	if (!Hall_isSensorGood()) {
		Debug_startMessage(Debug_MSG_TARGET_FAIL);
		Debug_endMessage();
		while (1)
			;
	}


	time_t t0, t1;
	SEND_ERROR("Testing clock.\n\rCurrent time (transmitter value) is:");
	Debug_startMessage(Debug_MSG_PAUSE_SESSION);
	Debug_endMessage();
	PCF_reset();
	PCF_getTime(&t0);
	delay_ms(600);
	PCF_dumpTime(&t0);

	// Now waste some time.
	delay_ms(2000);

	SEND_ERROR("Second clock sample is:");
	Debug_startMessage(Debug_MSG_PAUSE_SESSION);
	Debug_endMessage();
	PCF_getTime(&t1);
	delay_ms(600);
	PCF_dumpTime(&t1);

	if (!PCF_isAfter(&t1, &t0)) {
		SEND_ERROR("Clock not counting.  It it installed?  Its crystal?");
		Debug_startMessage(Debug_MSG_TARGET_FAIL);
		Debug_endMessage();
		while(1)
			;
	}


	SEND_ERROR("Test complete.  Part good.");

	Debug_startMessage(Debug_MSG_TARGET_GOOD);
	Debug_endMessage();
		
	Debug_startMessage(Debug_MSG_END_SESSION); // Done programming operation
	Debug_endMessage();
}


#endif