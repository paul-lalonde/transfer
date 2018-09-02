#ifndef ATTINY_84A_DEFS_H
#define ATTINY_84A_DEFS_H

#include <avr/io.h>

// RF Transmitter attached to PORTA2
#define RF_BIT PORTA2
#define RF_PORT PORTA
#define RF_DDR DDRA

// PCF2132 RTC Chip Enable SPI selector line on PORTB0
#define PCF_CE_PORT PORTB
#define PCF_CE_BIT PORTB0
#define PCF_CE_DDR DDRB

// ADXL362 power is on PORTA3. 
// TODO: On next spin of board, move power off of MCU; you can't
// turn off the ADXL without dropping its interrupt line, so this is a waste.
#define ADXL_POWER_PIN PORTA3
#define ADXL_POWER_PORT PORTA
#define ADXL_POWER_DDR DDRA

// SPI lines.
#define _U_SPI_PORT PORTA
#define _U_SPI_DDR DDRA
#define _U_SPI_PIN PINA
#define _U_DO (1 << PORTA5)
#define _U_DI (1 << PORTA6)
#define _U_SCK (1 << PORTA4)

// ADXL select
#define _U_SSADXL (1 << PORTA7) 

// Debug output select for our hideous protocol back to programmer.
#define _U_SSOFFBOARD (1 << PORTA6) // Reuse MISO

// Hall effect sensor on PORTA1
#define HALL_SENSOR_BIT PORTA1
#define HALL_SENSOR_PIN PINA
#define HALL_SENSOR_PORT PORTA
#define HALL_SENSOR_DDR DDRA
#define HALL_SENSOR_INTR PCINT1
#define HALL_SENSOR_INTR_MASK PCMSK0
#define HALL_SENSOR_INTR_ENABLE PCIE0
// Hall effect sensor power on/off.
#define HALL_POWER_BIT PORTA0

#endif