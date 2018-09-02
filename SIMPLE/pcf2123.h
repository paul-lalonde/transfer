#ifndef PCF2123_H
#define PCF2123_H

#include "pd2defs.h"
#include "date.h"

#define PCF_4096HZ 0b00
#define PCF_64HZ 0b01
#define PCF_1SEC 0b10
#define PCF_60SEC 0b11

#define PCF_64HZ_IN_MSEC 0.064

#define PCF_CONTROL_1 0x00
#define PCF_CONTROL_2 0x01

#define PCF_CTL2_TIE (1 << 0)
#define PCF_CTL2_AIE (1 << 1)
#define PCF_CTL2_TF (1 << 2)
#define PCF_CTL2_AF (1 << 3)
#define PCF_CTL2_TI_TP (1 << 4)
#define PCF_CTL2_MSF (1 << 5)


void PCF_reset();

void PCF_setTime(time_t *t);
void PCF_getTime(time_t *t);

void PCF_setRegs(uint8_t reg, const uint8_t *val, uint8_t nval);
void PCF_readRegs(uint8_t reg, uint8_t *val, uint8_t nval);

void PCF_dumpTime(time_t *t);


void PCF_genInterrupt(uint8_t timer);
void PCF_genInterruptCounted(uint8_t timer, uint8_t count);
void PCF_stopInterrupt();
int32_t PCF_waitTicks(int16_t ticks);

void PCF_setAlarm(uint8_t hour, uint8_t minute);
void PCF_getAlarm(uint8_t *hour, uint8_t *minute);

// Returns zero if the alarm wasn't set, 1 otherwise.
uint8_t PCF_clearAlarm();
void PCF_WaitForClock();

#endif
