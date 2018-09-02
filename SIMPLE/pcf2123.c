#include <avr/sleep.h>

#include "pcf2123.h"
#include "spi.h"
#include "debug_serial.h" // For PCF_dumpTime
#include <avr/interrupt.h>

#define PCF_SECONDS 0x02

#define PCF_MINUTE_ALARM 0x09
#define PCF_HOUR_ALARM 0x0A
#define PCF_DAY_ALARM 0x0B
#define PCF_WEEKDAY_ALARM 0x0C

#define PCF_OFFSET_REGISTER 0x0D

#define PCF_TIMER_CLKOUT 0x0E
#define PCF_COUNTDOWN_TIMER 0xF

static void raiseCE() {
	PCF_CE_PORT |= 1 << PCF_CE_BIT;
}

static void dropCE() {
	PCF_CE_PORT &= ~(1 << PCF_CE_BIT);
}

static uint8_t sreg;
static void PCF_SPI_start() {
	SPI_start();
	sreg = SREG;
	cli();
	_U_SPI_DDR |= _U_SCK | _U_DI;
	_U_SPI_DDR &= ~_U_DO;
	_U_SPI_PORT &= ~_U_DO; // Set it tri-state - let it float when not otherwise in use.  Other device might be driving.
	raiseCE();
}

static void PCF_SPI_stop() {
	dropCE();
	_U_SPI_DDR |= _U_SCK | _U_DO;
	_U_SPI_DDR &= ~_U_DI;
	_U_SPI_PORT &= ~_U_DI; // Set it tri-state - let it float when not otherwise in use.  Other device might be driving.
	SREG = sreg;
	SPI_stop();
}


void PCF_setRegs(uint8_t reg, const uint8_t *val, uint8_t nval) {
	reg |= 0x10;
	PCF_SPI_start();
	SPI_transferByte_switched(reg);
	for(uint8_t i = 0; i < nval; ++i) {
		SPI_transferByte_switched(*val++);
	}
	PCF_SPI_stop();
}

void PCF_readRegs(uint8_t reg, uint8_t *val, uint8_t nval) {
	reg |= 0x90;
	
	PCF_SPI_start();
	SPI_transferByte_switched(reg);

	for(uint8_t i = 0; i < nval; ++i) {
		*val++ = SPI_transferByte_switched(0);
	}
	PCF_SPI_stop();
}

void PCF_reset() {
	uint8_t val = 0x58;
	PCF_setRegs(PCF_CONTROL_1, &val, 1);
}

void PCF_setTime(time_t *t) {
	PCF_setRegs(PCF_SECONDS, (uint8_t*)t, sizeof(time_t));
}

void PCF_getTime(time_t *t) {
	PCF_readRegs(PCF_SECONDS, (uint8_t*)t, sizeof(time_t));
}

void PCF_dumpTime(time_t *t) {
	Debug_startMessage(Debug_MSG_CSTRING);
	Debug_sendNumber((t->hours & 0x30) >> 4, 10);
	Debug_sendNumber((t->hours & 0xf), 10);
	SEND_STRING(":");
	Debug_sendNumber((t->minutes & 0x70) >> 4, 10);
	Debug_sendNumber((t->minutes & 0xf), 10);
	SEND_STRING(":");
	Debug_sendNumber((t->seconds & 0x70) >> 4, 10);
	Debug_sendNumber((t->seconds & 0xf), 10);
	SEND_STRING(" ");
	Debug_sendNumber((t->days & 0x30) >> 4, 10);
	Debug_sendNumber((t->days & 0xf), 10);
	SEND_STRING(".");
	Debug_sendNumber((t->months & 0x10) >> 4, 10);
	Debug_sendNumber((t->months & 0xf), 10);
	SEND_STRING(".");
	Debug_sendNumber((t->years & 0xf0) >> 4, 10);
	Debug_sendNumber((t->years & 0xf), 10);
	Debug_endMessage();
}

volatile uint8_t PCF_nPulses;

void PCF_genInterruptCounted(uint8_t timer, uint8_t count)
{
	uint8_t sreg = SREG;
	cli();
	PCF_nPulses = 0;

	PCF_setRegs(PCF_COUNTDOWN_TIMER, &count, 1);
	
	uint8_t val = 1 << 3; // Countdown timer enabled
	val |= timer;
	PCF_setRegs(PCF_TIMER_CLKOUT, &val, 1);

	PCF_readRegs(PCF_CONTROL_2, &val, 1);
	val |= PCF_CTL2_TIE;
	PCF_setRegs(PCF_CONTROL_2, &val, 1);
	SREG = sreg;
}

void PCF_genInterrupt(uint8_t timer)
{
	PCF_genInterruptCounted(timer, 1);
}

void PCF_stopInterrupt()
{
	uint8_t sreg = SREG;
	cli();
	static uint8_t val0 = 0;
	PCF_readRegs(PCF_CONTROL_2, &val0, 1);
	val0 &= ~ PCF_CTL2_TIE; // Stop timer interrupt
	PCF_setRegs(PCF_CONTROL_2, &val0, 1); 
	static uint8_t val = PCF_60SEC;
	PCF_setRegs(PCF_TIMER_CLKOUT, &val, 1);
	SREG = sreg;
}

extern volatile uint8_t PCF_nPulses;

void PCF_WaitForClock() {
	while (1) {
		if (PCF_nPulses) {
			// Got one!
			PCF_nPulses = 0;
			return;
		}
		// not ours, continue

		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();  // We wake on the next interrupt.
	}
}


/*
static void WaitForClockIdle() {
	while (1) {
		if (PCF_nPulses) {
			// Got one!
			return;
		}
		// not ours, continue

		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();  // We wake on the next interrupt.
	}
}
*/

// Waits at least 0uSec and no more than 244uSec more
// The overhead of this function is about 3mSec due to SPI to the clock.
int32_t PCF_waitTicks(int16_t nticks)
{
	// Major power optimization possible here, due to power-up spike
	// use 60hz or 64hz timers for large enough delays.
	while (nticks > 0) {
		int16_t waitticks = nticks >> 4; // Every tick we have is 1/64 seconds, or 15.625 msec.
			// Specified ticks are in mSec, so we need to translate a bit; we approximate as 16 mSec per tick
		if (waitticks > 255) waitticks = 255; // Clamp to maximum wait.
		PCF_genInterruptCounted(PCF_64HZ, waitticks);
		PCF_WaitForClock();
		//WaitForClockIdle();
		nticks -= waitticks << 4;
	}

	return 0;
}

void PCF_setAlarm(uint8_t hour, uint8_t minute)
{
	uint8_t buf[2];
	buf[0] = toBCD(minute);
	buf[1] = toBCD(hour);
	PCF_setRegs(PCF_MINUTE_ALARM, buf, 2);
	PCF_readRegs(PCF_CONTROL_2, buf, 1);
	buf[0] |= PCF_CTL2_AIE; // alarm interrupt enable
	PCF_setRegs(PCF_CONTROL_2, buf, 1);
}

void PCF_getAlarm(uint8_t *hour, uint8_t *minute)
{
	uint8_t buf[2];
	PCF_readRegs(PCF_MINUTE_ALARM, buf, 2);
	*minute = fromBCD(buf[0] & 0x7F);
	*hour = fromBCD(buf[1] & 0x3F);
}


uint8_t PCF_clearAlarm()
{
	uint8_t v, r;
	PCF_readRegs(PCF_CONTROL_2, &v, 1);
	r = v & (PCF_CTL2_AF|PCF_CTL2_TF);
	v &= ~(PCF_CTL2_AF|PCF_CTL2_TF); //~0x4C; // Clear the AF flag and TF)
	PCF_setRegs(PCF_CONTROL_2, &v, 1);
	return r;
}

