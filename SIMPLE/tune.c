#if TUNE_BUILD
#include <util/delay.h>
#include "tune_params.h"
#include "pd2.h"
#include "hall.h"
#include "pcf2123.h"
#include "debug_serial.h"
#include "adxl362.h"

extern void RFOn() ;
extern void RFOff() ;


int main()
{
	SEND_ERROR("Starting up");
	SEND_ERROR("Pulse width 30ms on 1500ms interval");
		
	Debug_startMessage(Debug_MSG_PAUSE_SESSION);
	Debug_endMessage();
	PD2_init();

	ADXL_measure();

	SEND_ERROR("RunCalibrationTimers");

	PCF_genInterruptCounted(PCF_64HZ, (int)(1500/15.625)); // Get an interrupt every pulse interval.
}

#endif
