// This header generated in response to JobId {{.Lookup "JobId"}}
// In mSec
#define RF_PULSE_WIDTH {{.Lookup "PulseWidth"}}
#define BETWEEN_PULSE_PAUSE {{.Lookup "PulsePause"}}

// RF Pulse frequency
#define pulse_interval {{.Lookup "PulseInterval"}}

// We ping on this schedule, in minues, 12 hours on, 12 off.
//	#define PING_DAY_DURATION (12 * 60 )
//	#define PING_NIGHT_DURATION (12 * 60 )
// For debug we use 3 minutes on, 3 minutes off
	#define PING_DAY_DURATION {{.Lookup "DayDuration"}}
	#define PING_NIGHT_DURATION {{.Lookup "NightDuration"}}

// 1000 LSB/g
#define INACTIVITY_THRESHHOLD {{.Lookup "InactivityThreshhold"}}
#define ACTIVITY_THRESHHOLD {{.Lookup "ActivityThreshhold"}}

// ticks at 12.5Hz, 8/sec.  We start at 20 seconds inactive.
#define INACTIVITY_TIME {{.Lookup "InactivityTime"}}
// Start with any activity
#define ACTIVITY_TIME {{.Lookup "ActivityTime"}}

