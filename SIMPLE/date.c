#include "./date.h"

uint8_t toBCD(uint8_t in) {
        uint8_t bcd = in / 10;
        bcd = (bcd << 4) | (in - (bcd * 10));
        return bcd;
}

uint8_t fromBCD(uint8_t bcd) {
        return (bcd & 0xf) + 10 * (bcd>>4);
}

uint8_t minutes(time_t *t) {
	return fromBCD(t->minutes & 0x7f);
}

uint8_t hours(time_t *t) {
	return fromBCD(t->hours & 0x3f);
}

// BCD compares are just fine here.
// LHS isBefore RHS

uint8_t PCF_isAfter(time_t *lhs, time_t *rhs)
{
	if (lhs->years > rhs->years && !(lhs->years == 0x99 || rhs->years == 0x99)) return 1;
	if (lhs->years == rhs->years && lhs->months > rhs->months && !(lhs->months == 0x99 || rhs->months == 0x99)) return 1;
	if (lhs->months == rhs->months && lhs->days > rhs->days && !(lhs->days == 0x99 || rhs->days == 0x99)) return 1;
	if (lhs->days == rhs->days && lhs->hours > rhs->hours) return 1;
	if (lhs->hours == rhs->hours && lhs->minutes > rhs->minutes) return 1;
	if (lhs->minutes == rhs->minutes && lhs->seconds > rhs->seconds) return 1;
	return 0;
}

uint16_t approxday(time_t *c) {
	return fromBCD(c->days) + fromBCD(c->months) * 31;
}

// The day approximation is monotonically increasing by date.  
uint8_t PCF_isBetweenDays(time_t *c, time_t *early, time_t *late) {
	uint16_t capproxday = approxday(c);
	uint16_t earlyapproxday = approxday(early);
	uint16_t lateapproxday = approxday(late);

	if (earlyapproxday < lateapproxday) {
		return  (capproxday >= earlyapproxday) && (capproxday < lateapproxday);
	} else {
		return (capproxday >= lateapproxday) || (capproxday < earlyapproxday);
	}
}