// This header generated in response to JobId 1990.000
// In mSec
#define RF_PULSE_WIDTH 30
#define BETWEEN_PULSE_PAUSE 100
#define USE_ACCELEROMETER 1
#define USE_HIBERNATION 1
#define REPORT_TEMPERATURE 1
#define USE_DIURNAL 1

// RF Pulse frequency
#define RF_PULSE_INTERVAL 1000 

// We ping on this schedule, in minues, 12 hours on, 12 off.
//	#define PING_DAY_DURATION (12 * 60 * )
//	#define PING_NIGHT_DURATION (12 * 60 )
// For debug we use 3 minutes on, 3 minutes off
	#define PING_DAY_DURATION 0
	#define PING_NIGHT_DURATION 3

// 1000 LSB/g
#define INACTIVITY_THRESHHOLD 100
#define ACTIVITY_THRESHHOLD 100

// An Activity period is 1 hour - 45000 ticks at 12.5Hz
#define INACTIVITY_PERIODS_TO_MORTALITY ((int)(12))
#define INACTIVITY_PERIOD_LENGTH ((int)((float)(3600)*12.5))
// Activity periods before reporting an active state.  Will be on an interrupt for activity mode
#define ACTIVITY_PERIODS ((int)(1))

// temperature guesses from room and hand temps.
// 17c = -215; 37c = -15; => 20c = 200units; postulate res is 0.1c
#define TEMPERATURE_BIAS 0
#define TEMPERATURE_SCALE 10

