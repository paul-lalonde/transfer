#include <inttypes.h>

// state machine functions
void RFPulse();
void RFPulseOff();

// hardware control
void RFOn();
void RFOff();

void PD2_init();
uint8_t SetupAccelerometer();

void loadConstsFromEEPROM();

extern uint32_t pulseInterval;

// EEPROM memory map
// These defines are duplicated in programmer for setting eeprom.
#define EEPROM_CLOCK_BASE 0x04
#define EEPROM_TEMP_BIAS (EEPROM_CLOCK_BASE + 7) // sizeof(time_t))
#define EEPROM_TEMP_SCALE (EEPROM_TEMP_BIAS + sizeof(uint16_t))
#define EEPROM_TEMP_SCALE_DENOM (EEPROM_TEMP_SCALE + sizeof(uint16_t))
#define EEPROM_PROG_TEMP (EEPROM_TEMP_SCALE_DENOM + sizeof(uint16_t))

