#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "tracker_params.h"
#include "pd2.h"
#include "pcf2123.h"
#include "adxl362.h"
#include "hall.h"
#include "debug_serial.h"


static time_t hibernateSleep = {0, 0, 0, OFF_DATE_DAY, 0x99, OFF_DATE_MONTH, 0x99};
static time_t hibernateWake = {0, 0, 0, ON_DATE_DAY, 0x99, ON_DATE_MONTH, 0x99};

uint8_t ontime[2] = ON_TIME;
uint8_t offtime[2] = OFF_TIME;

volatile uint8_t hibernating = 0;
volatile uint8_t isDayTime = 1;
volatile uint8_t AlarmWentOff;
volatile uint8_t gotActivitySignal;
volatile uint8_t gotInactivitySignal;
volatile uint8_t amActive = 0;
extern volatile uint8_t ADXL_isLive;
extern volatile uint8_t PCF_nPulses;

// Return true if we should be running, false if we should turn off.
uint8_t DeviceIsOn() {
	static int amRunning = 1;

	Hall_powerOn();
	_delay_ms(HALL_STABILIZATION_TIME);
	int hallState = Hall_isMagnetDetected();
	Hall_powerOff();
	if (hallState == 1) { // Switching!
		if (amRunning) {
			// 5 pulses to show we're shutting down.
			for(int i=0; i < 5; ++i) {
				RFOn();
				_delay_ms(60);
				RFOff();
				_delay_ms(100);
			}
			amRunning = 0;
		} else {
			// 10 pulses to show we're starting up.
			for(int i=0; i < 10; ++i) {
				RFOn();
				_delay_ms(60);
				RFOff();
				_delay_ms(100);
			}
			amRunning = 1;
		}
	}			
	return amRunning;
}

ISR(EXT_INT0_vect)
{
	 if (ADXL_isLive) {
		uint8_t status = ADXL_readbyteregister(0xB); // Needed to clear interrupt status.
		if (status & ADXL_STATUS_ACT) {
#if USE_ACTIVITY
			if (amActive) {
				RFOn();
				_delay_ms(RF_PULSE_WIDTH/2);
				RFOff();
			}
#endif
			gotActivitySignal = 1;
			return;
		}
		if (status & ADXL_STATUS_INACT) {
			gotInactivitySignal = 1;
			return;
       		}
	}

	uint8_t alarms = PCF_clearAlarm();
#if USE_DIURNAL
	if (alarms & PCF_CTL2_AF) {
		AlarmWentOff = 1;
	}
#endif

	if (alarms & PCF_CTL2_TF) {
		PCF_nPulses ++;
	}
}

uint8_t amHibernating() {
	time_t t;
	PCF_getTime(&t);
	return PCF_isBetweenDays(&t, &hibernateSleep, &hibernateWake);
}

#if USE_DIURNAL
void WaitForAlarm() {
	while (!AlarmWentOff) {
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();  // We wake on the next interrupt.
	}
	AlarmWentOff = 0;
}
#endif

int main()
{
	// Start by sending our programmed data.
	SEND_ERROR("Device Variables");
	SEND_VARIABLE("offtime[0] Hour ", offtime[0], 10);
	SEND_VARIABLE("offtime[1] Minute ", offtime[1], 10);
	time_t t;
	PCF_getTime(&t);
	PCF_dumpTime(&t);
	
	PD2_init();
	
	// TODO: Use hall effect sensor to test on or off.  We only check every 5th pulse.  We never check at night
	// or during hibernation.

	// Send 10 blips to show we're alive.

	for(int i = 0; i < 10; ++i) {
		RFOn();
		_delay_ms(40);
		RFOff();
		_delay_ms(200);
	}
	
#if USE_DIURNAL
	// The next alarm is always the turn-off: it's good to always start up from the Hall effect sensor into
	// daytime mode.  A day spent there is not wasted.
	PCF_setAlarm(offtime[0], offtime[1]);
	
#endif

	ADXL_setInactivityThreshhold(INACTIVITY_THRESHHOLD);
	ADXL_setInactivityTime(MORTALITY_PERIOD_LENGTH); 
	ADXL_setActivityThreshhold(ACTIVITY_THRESHHOLD);
	ADXL_setActivityTime(ACTIVITY_PERIODS);

	// Reliable debug output can only happen before interrupts are enabled.  After that there is the potential of dropping
	// interrupts while doing output - we suspend all interrupts during debug output.

	PCF_genInterruptCounted(PCF_64HZ, (int)(RF_PULSE_INTERVAL/15.625)); // Get an interrupt every pulse interval.
	// For sanity, width, etc must be longer than this interval.
	// This needs to be turned off at night and back on at daybreak.
	
	int mortalityCount = 0;
	uint8_t amPoweredOn = 1;
	uint8_t hallLoopCount = 0;
#if REPORT_TEMPERATURE
	uint8_t tempLoopCount = 0;
#endif

	while (1) {
		PCF_WaitForClock(); // Power down, wait for next timer interrupt
		if (amPoweredOn) {
			if (isDayTime) {
				RFOn();
				_delay_ms(RF_PULSE_WIDTH);
				RFOff();
	#if USE_MORTALITY
				// Pulse control
				if (mortalityCount >= INACTIVITY_PERIODS_TO_MORTALITY) {
					_delay_ms(BETWEEN_PULSE_PAUSE);
					RFOn();
					_delay_ms(RF_PULSE_WIDTH);
					RFOff();
				}
	#endif
				
				// Process signals since last sleep
				if (gotActivitySignal) {
					mortalityCount = 0; // We were active, reset to alive
					gotActivitySignal = 0;
	#if USE_ACTIVITY
					amActive = 1;
					// Set the accelerometer to now to blip activity.
					ADXL_setActivityTime(2); // Blip every 6.25 times per second (half of 12.5Hz)
					// Set the accelerometer to wait for our inactive period.
					ADXL_setInactivityTime(INACTIVITY_PERIODS); 
	#endif
				}
				
				if (gotInactivitySignal) {
	#if USE_ACTIVITY
					if (amActive) { // Pass out of activity
						amActive = 0;
						// set accelerometer back to waiting for enough activity to pass to be active.
						ADXL_setActivityTime(ACTIVITY_PERIODS);
						// Set back to looking for mortality.
						ADXL_setInactivityTime(MORTALITY_PERIOD_LENGTH);
					}
	#endif
					mortalityCount++;
					gotInactivitySignal = 0;
				}
				
	#if USE_DIURNAL
				// In daytime, the next alarm is nighttime.
				if (AlarmWentOff) {
					AlarmWentOff = 0;
					isDayTime = 0;
				}
	#endif
	
	#if REPORT_TEMPERATURE
				// Check our temperature every 10 pulses.
				if (tempLoopCount++ >= 10) {
					tempLoopCount = 0;
					int32_t temperature = ADXL_temperature() ;
					int32_t pulseInterval = RF_PULSE_INTERVAL + (int32_t)(TEMPERATURE_PULSE_SCALE*temperature) ;
					if (pulseInterval > (int)(15.625 * 255.0))
						pulseInterval = (int)(15.625 * 255.0);
	SEND_VARIABLE("temperature =", temperature, 10);
	SEND_VARIABLE("pulseInterval ", pulseInterval, 10);
					PCF_stopInterrupt(); // Stop it before resetting.
					PCF_genInterruptCounted(PCF_64HZ, (uint8_t)(pulseInterval/15.625)); // Get an interrupt every pulse interval.
				}
	#endif
			} 
	#if USE_DIURNAL
			else {
	SEND_ERROR("Switching to night");
				PCF_stopInterrupt(); // We don't pulse at night, nor do we monitor the
							// accelerometer, nor the Hall Effect seonsor.
				ADXL_powerOff();
				PCF_setAlarm(ontime[0], ontime[1]);
				WaitForAlarm(); // Nothing else runs here. 
				// We only check for hibernation at start of each day.
				if (!USE_HIBERNATION || !amHibernating()) {
					isDayTime = 1;
					PCF_genInterruptCounted(PCF_64HZ, (int)(RF_PULSE_INTERVAL/15.625)); // Get an interrupt every pulse interval.
					PCF_setAlarm(offtime[0], offtime[1]);
					ADXL_measure(); // Start running the accelerometer again.
					Hall_powerOn();
				} 
			}
	#endif
		} // endif amPoweredOn
		
		// Do our housekeeping: Powered on
		// This is only reached during daytime.
		
		// Check our magnet every 4 pulses.
		if (hallLoopCount++ >= 4) {
			hallLoopCount = 0;
			amPoweredOn = DeviceIsOn();
		}
	}
}
