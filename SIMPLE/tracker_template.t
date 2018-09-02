// This header generated in response to {{.CustomerId}}/{{.JobId}}
// In mSec
#define RF_PULSE_WIDTH {{.Lookup "PulseWidth"}}
#define BETWEEN_PULSE_PAUSE {{.Lookup "PulsePause"}}

// RF Pulse frequency
#define RF_PULSE_INTERVAL {{.Lookup "PulseInterval"}}

#define USE_ACCELEROMETER ({{.Lookup "UseMortality"}} || {{.Lookup "UseActivity"}})
#define USE_MORTALITY {{.Lookup "UseMortality"}}
#define USE_ACTIVITY {{.Lookup "UseActivity"}}
// On/Off times
#define USE_DIURNAL {{.Lookup "UseDiurnal" }}
#define ON_TIME {{.Lookup "OnTime" | TimeStrToAlarm }} // hour, minute
#define OFF_TIME {{.Lookup "OffTime" | TimeStrToAlarm }} // hour, minute

#define USE_HIBERNATION {{.Lookup "UseHibernation" }}
#define ON_DATE_DAY {{.Lookup "WakeDate" | DateStrToDayAlarm}}
#define OFF_DATE_DAY {{.Lookup "SleepDate" | DateStrToDayAlarm}}
#define ON_DATE_MONTH {{.Lookup "WakeDate" | DateStrToMonth}}
#define OFF_DATE_MONTH {{.Lookup "SleepDate"| DateStrToMonth}}

// 1000 LSB/g
#define INACTIVITY_THRESHHOLD {{.Lookup "InactivityThreshhold"}}
#define ACTIVITY_THRESHHOLD {{.Lookup "ActivityThreshhold"}}

// An Activity period is 1 hour - 45000 ticks at 12.5Hz
#define INACTIVITY_PERIODS_TO_MORTALITY ((int)({{.Lookup "InactivityPeriodsToMortality"}}))

// 2 minutes
#define MORTALITY_PERIOD_LENGTH ((int)((float)(120)*12.5))

// Activity periods before reporting an active state.  Will be on an interrupt for activity mode
#define ACTIVITY_PERIODS ((int)({{.Lookup "ActivityPeriods"}}))
#define INACTIVITY_PERIODS ((int)({{.Lookup "InactivityPeriods"}}))

#define REPORT_TEMPERATURE {{.Lookup "ReportTemperature" }}
#define TEMPERATURE_BIAS {{.Lookup "TemperatureBias"}}
#define TEMPERATURE_SCALE {{.Lookup "TemperatureScale"}}
#define TEMPERATURE_PULSE_SCALE {{.Lookup "TemperaturePulseScale"}}

