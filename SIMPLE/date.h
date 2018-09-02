#ifndef DATE_H
#define DATE_H
#include "inttypes.h"

// Time value is in BCD
typedef struct  { 
	uint8_t seconds, minutes, hours, days, weekdays, months, years;
} time_t;

uint8_t toBCD(uint8_t in);
uint8_t fromBCD(uint8_t bcd);
uint8_t minutes(time_t *t);
uint8_t hours(time_t *t);

uint8_t PCF_isAfter(time_t *lhs, time_t *rhs);
uint8_t PCF_isBetweenDays(time_t *c, time_t *early, time_t *late);
#endif
