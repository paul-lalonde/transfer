#include <util/delay.h>
#include "hall.h"
#include "debug_serial.h"

void Hall_setup() {
	HALL_SENSOR_DDR &= ~(1 << HALL_SENSOR_BIT); // input.
	HALL_SENSOR_DDR |= (1 << HALL_POWER_BIT); // output.
	HALL_SENSOR_PORT |= 1 << HALL_SENSOR_BIT;// Set pullup
}

int8_t Hall_isSensorGood() {
	Hall_setup();
	Hall_powerOn();
	_delay_ms(HALL_STABILIZATION_TIME);
	
	if (Hall_isMagnetDetected()) {
		SEND_ERROR("Hall sensor should be off - bad sensor");
		return 0;
	}
	
	SEND_ERROR("Place magnet in next 3 seconds");
	
	int timeout = 3000;
	while (timeout > 0) {
		if (Hall_isMagnetDetected()) {
			SEND_ERROR("Magnet detected.  Good sensor.");
			return !0;
		}
		timeout -= 10;
		_delay_ms(10);
	}
	
	SEND_ERROR("Magnet not detected - hall sensor bad.");
	return 0;
}