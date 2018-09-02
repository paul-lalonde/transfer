#include "pd2defs.h"

#define HALL_STABILIZATION_TIME 150

void Hall_setup();

inline void Hall_powerOn() {
	HALL_SENSOR_PORT |= 1 << HALL_POWER_BIT;
}

inline void Hall_powerOff() {
	HALL_SENSOR_PORT &= ~(1 << HALL_POWER_BIT);
}

inline int8_t Hall_isMagnetDetected() {
	return (HALL_SENSOR_PIN & (1 << HALL_SENSOR_BIT)) == 0; // pulls low on active
}

int8_t Hall_isSensorGood();
