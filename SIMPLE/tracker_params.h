// This header generated in response to TEMPLT/SIMPLE
// In mSec
#define RF_PULSE_WIDTH 100
#define BETWEEN_PULSE_PAUSE 100

// RF Pulse frequency
#define RF_PULSE_INTERVAL 2000

#define USE_ACCELEROMETER (1 || 1)
#define USE_MORTALITY 1
#define USE_ACTIVITY 1
// On/Off times
#define USE_DIURNAL 1
#define ON_TIME {6,0} // hour, minute
#define OFF_TIME {18,0} // hour, minute

#define USE_HIBERNATION 1
#define ON_DATE_DAY 12
#define OFF_DATE_DAY 10
#define ON_DATE_MONTH 12
#define OFF_DATE_MONTH 12

// 1000 LSB/g
#define INACTIVITY_THRESHHOLD 100
#define ACTIVITY_THRESHHOLD 50

// An Activity period is 1 hour - 45000 ticks at 12.5Hz
#define INACTIVITY_PERIODS_TO_MORTALITY ((int)(2))

// 2 minutes
#define MORTALITY_PERIOD_LENGTH ((int)((float)(120)*12.5))

// Activity periods before reporting an active state.  Will be on an interrupt for activity mode
#define ACTIVITY_PERIODS ((int)(12))
#define INACTIVITY_PERIODS ((int)(60))

#define REPORT_TEMPERATURE 1
#define TEMPERATURE_BIAS 350
#define TEMPERATURE_SCALE 15
#define TEMPERATURE_PULSE_SCALE 50

