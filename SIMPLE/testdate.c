#ifdef UNIT_TEST

#include <stdio.h>
#include "./date.h"

extern uint16_t approxday(time_t *c);
int TestIsBetweenDays() {
	// First validate approxday().
	time_t c;
	uint8_t *z = (uint8_t*)&c;
	for(int i = 0; i < sizeof(c); ++i) { z[i] = 0; }
	
	uint16_t prevApproxDay = 0;
	for(int month = 1; month <= 12; month++) {
		uint8_t bcdmonth = toBCD(month);
		for(int day = 0; day < 31; day++) {
			uint8_t bcdday = toBCD(day);
			c.days = bcdday;
			c.months = bcdmonth;
			
			uint16_t aday = approxday(&c);
			if (aday <= prevApproxDay) return 0;
			else prevApproxDay = aday;
		}
	}
	
	return 1;
}

int main() {
	if (!TestIsBetweenDays()) {
		printf("FAILED");
	}
}

#endif