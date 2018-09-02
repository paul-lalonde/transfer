// This header generated in response to JobId /*FAILED LOOKUP*/
// In mSec
#define RF_PULSE_WIDTH 100
#define BETWEEN_PULSE_PAUSE 255

// RF Pulse frequency
#define pulse_interval 2500

// We ping on this schedule, in mSec, 12 hours on, 12 off.
//	#define PING_DAY_DURATION (12 * 60 * 60 * 1000)
//	#define PING_NIGHT_DURATION (12 * 60 * 60 * 1000)
// For debug we use 20 seconds on, 20 seconds off
	#define PING_DAY_DURATION 40000
	#define PING_NIGHT_DURATION 20000

// 1000 LSB/g
#define INACTIVITY_THRESHHOLD 100
#define ACTIVITY_THRESHHOLD 100

// ticks at 12.5Hz, 8/sec.  We start at 20 seconds inactive.
#define INACTIVITY_TIME 160
// Start with any activity
#define ACTIVITY_TIME 0

ITY_TIME 0
// Start with any activity
#define ACTIVITY_TIME 0

