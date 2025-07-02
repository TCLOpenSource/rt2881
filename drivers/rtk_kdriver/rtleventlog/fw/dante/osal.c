#include "osal.h"
PRAGMA_CODESEG(BANK0);
PRAGMA_CONSTSEG(BANK0);

void event_log_save_DW1(UINT32 DW1, UINT32 event_val) __banked
{
	UINT32 timestamp = 0;

	timestamp =  (UINT32) rtd_inl(TIMER_SCPU_CLK90K_LO)*11;
	timestamp += (UINT32) (11*rtd_inl(TIMER_SCPU_CLK27M_90K))/300;
	EV_EMERG("EVENTLOG:%x-%x-%x-%x\n",(UINT32) DW1, (UINT32) event_val, 0, (UINT32)timestamp);
}

