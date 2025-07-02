#pragma codeseg BANK0
#pragma constseg BANK0
#include "osal.h"
#include "power.h"
/************************************************************************
 *  Define
 ************************************************************************/
//UINT32 memory_count = 0;
//UINT32 log_memory[INT_LOG_SIZE];


unsigned int event_log_save_DW1(UINT32 DW1, UINT32 event_val, UINT32 module_reserved)
{
	UINT32 timestamp = 0;

	if(Get_PowerMode() != _ON) {		// Detect wakeup source when power off
		timestamp =  (UINT32) rtd_inl(TIMER_SCPU_CLK90K_LO)*11;
		timestamp += (UINT32) (11*rtd_inl(TIMER_SCPU_CLK27M_90K))/300;
		EV_EMERG("EVENTLOG:%x-%x-%x-%x\n",(UINT32) DW1, (UINT32) event_val, (UINT32) module_reserved, (UINT32)timestamp);
	}
	else
		EV_EMERG("EVENTLOG:%x-%x-%x (power off)\n",(UINT32) DW1, (UINT32) event_val, (UINT32) module_reserved);

	return 0;
}

/*unsigned int event_log_save(UINT32 fw_type, UINT32 event_type, UINT32 module, UINT32 event, UINT32 event_val, UINT32 module_reserved)
{
	UINT32 timestamp = 0;
	UINT32 DW1 = 0;

	if(fw_type >= FW_MAX || module >= MODULE_MAX || event >= EVENT_MAX || event_type >= EVENT_TYPE_MAX)
	{
		EV_EMERG("(fw_type module event event_type) > Specified range\n");
		return 1;
	}

	timestamp =  (UINT32) rtd_inl(TIMER_SCPU_CLK90K_LO)*11;
	timestamp += (UINT32) (11*rtd_inl(TIMER_SCPU_CLK27M_90K))/300;

	DW1 = (UINT32)(((EVENT_SYNCBYTE & (UINT32) 0xff) << 24) | ((fw_type & (UINT32) 0xf) << 20) \
									| ((event_type & (UINT32) 0xf) << 16) | (module & (UINT32) 0xff << 8) \
									| (event & (UINT32) 0xff));
	if(memory_count >= (INT_LOG_SIZE-1))
	{
		memory_count=0;
	}

	log_memory[memory_count++] = (UINT32)(((EVENT_SYNCBYTE & (UINT32) 0xff) << 24) | ((fw_type & (UINT32) 0xf) << 20) \
									| ((event_type & (UINT32) 0xf) << 16) | (module & (UINT32) 0xff << 8) \
									| (event & (UINT32) 0xff));

	log_memory[memory_count++] =(UINT32) event_val;

	log_memory[memory_count++] =(UINT32) module_reserved;

	log_memory[memory_count++] =(UINT32) timestamp;

	EV_EMERG("EVENTLOG:%x-%x-%x-%x\n",log_memory[memory_count-4],log_memory[memory_count-3],log_memory[memory_count-2],log_memory[memory_count-1]);
    return 0;
}*/


/*void eventlog_printf_all(void)
{
	UINT32 DW1 = 0;

	while  ((DW1+3) < memory_count)
	{
		EV_EMERG("EVENTLOG:%x-%x-%x-%x\n", log_memory[DW1+1], log_memory[DW1+1], log_memory[DW1+2], log_memory[DW1+3]);
		DW1 = DW1+4;
	}
}
*/