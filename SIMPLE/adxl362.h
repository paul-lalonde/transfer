#ifndef _ADXL362_H
#define _ADXL362_H

#include <inttypes.h>

int16_t ADXL_temperature();
void ADXL_calibrateTemperature(int16_t currTemp);
void ADXL_setBiasAndScale(int16_t bias, int16_t scale_numerator, uint16_t scale_denom);

uint8_t ADXL_readbyteregister(uint8_t reg);
void ADXL_setbyteregister(uint8_t reg, uint8_t value);
void ADXL_setwordregister(uint8_t reg, uint16_t value);
uint16_t ADXL_readwordregister(uint8_t reg);

enum ADXL {
	ADXL_STATUS_AWAKE = 1 << 6,
	ADXL_STATUS_INACT = 1 << 5,
	ADXL_STATUS_ACT = 1 << 4,

	ADXL_ACT_INACT_CTL = 0x27,
	ADXL_INACT_LINK = 1 << 4,
	ADXL_INACT_LOOP = 3 << 4,
	ADXL_INACT_INACT_REF = 1 << 3,
	ADXL_INACT_INACT_EN = 1 << 2,
	ADXL_INACT_ACT_REF = 1 << 1,
	ADXL_INACT_ACT_EN = 1 << 0,

	ADXL_INTMAP1 = 0x2A,
	ADXL_INTMAP1_INT_LOW = 1 << 7,
	ADXL_INTMAP1_AWAKE = 1 << 6,
	ADXL_INTMAP1_INACT = 1 << 5,
	ADXL_INTMAP1_ACT = 1 << 4,
	
	ADXL_INTMAP2 = 0x2B,
	ADXL_INTMAP2_INT_LOW = 1 << 7,
	ADXL_INTMAP2_INACT = 1 << 5,
	ADXL_INTMAP2_ACT = 1 << 4,
};

inline void ADXL_setLowestDataRate()
{
	// Set filter control register to
	// RANGE = 0; +- 2G
	// RES = 0; reserved
	// HALF_BW = 0; wider bandwidth
	// EXT_SAMPLE = 0; no external sample trigger
	// ODR = 000 = 12.5Hz
	ADXL_setbyteregister(0x2C, 0);
}

inline void ADXL_setActivityThreshhold(uint16_t thresh)
{
	ADXL_setwordregister(0x20, thresh);
}

inline void ADXL_setActivityTime(uint8_t time)
{
	ADXL_setbyteregister(0x22, time);
}

inline void ADXL_setInactivityThreshhold(uint16_t thresh)
{
	ADXL_setwordregister(0x23, thresh);
}

inline void ADXL_setInactivityTime(uint16_t time)
{
	ADXL_setwordregister(0x25, time);
}

inline uint8_t ADXL_readStatus() {
	return ADXL_readbyteregister(0x0B);
}

void ADXL_powerOff();

void ADXL_measure();
void ADXL_enableIntr();
#endif