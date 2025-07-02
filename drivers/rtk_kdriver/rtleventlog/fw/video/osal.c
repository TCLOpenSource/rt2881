#include <osal.h>

unsigned int eventlog_time_unit = 1;
unsigned int memory_count = 0;
unsigned int log_memory[INT_LOG_SIZE];
unsigned int time_memory[INT_LOG_SIZE/4];


unsigned int rtk_timer_misc_90k_ms( void )
{
        unsigned int time = (unsigned int) rtd_inl(TIMER_SCPU_CLK90K_LO_reg)/90;
        return time;
}


unsigned int rtk_timer_misc_90k_us( void )
{
        unsigned int time = (unsigned int) rtd_inl(TIMER_SCPU_CLK90K_LO_reg)*11;
        return time;
}


unsigned int event_log_save(unsigned int fw_type, unsigned int event_type, unsigned int module, unsigned int event, unsigned int event_val, unsigned int module_reserved)
{
    struct event_format event_format;
    int time_memory_size = sizeof(time_memory);
    int log_memory_size = sizeof(log_memory);

    if(fw_type >= FW_MAX || module >= MODULE_MAX || event >= EVENT_MAX || event_type >= EVENT_TYPE_MAX)
    {
        ModuleDirectPrintf(MODULE_DEBUG, "(fw_type module event event_type) > Specified range\n");
        return EVENT_ERROR;
    }

    /* clean event_format */
    VP_memset(&event_format, 0, sizeof(event_format));

    event_format.syncbyte = EVENT_SYNCBYTE;
    event_format.fw_type = fw_type;
    event_format.event_type = event_type;
    event_format.module = module;
    event_format.event = event;
    event_format.event_value = event_val;
    event_format.module_reserved = module_reserved;

    if(eventlog_time_unit)
    {
        event_format.timestamp = (unsigned int) rtk_timer_misc_90k_us();
    }
    else
    {
        event_format.timestamp = (unsigned int) rtk_timer_misc_90k_ms();
    }

    if(memory_count >= (INT_LOG_SIZE-1))
    {
        memory_count=0;
    }

    time_memory[((memory_count%time_memory_size)/4)] = eventlog_time_unit;

    /*DW1*/
    log_memory[(memory_count++)%log_memory_size] = ((event_format.syncbyte) << 24) | ((event_format.fw_type) << 20) \
                                    | ((event_format.event_type) << 16) | (event_format.module << 8) \
                                    | (event_format.event);
    /*DW2*/
    log_memory[(memory_count++)%log_memory_size] = event_format.event_value;

    /*DW3*/
    log_memory[(memory_count++)%log_memory_size] = event_format.module_reserved;

    /*DW4*/
    log_memory[(memory_count++)%log_memory_size] = event_format.timestamp;


    ModuleDirectPrintf (MODULE_DEBUG, "%08x-%08x-%08x-%08x\n",((event_format.syncbyte) << 24) | ((event_format.fw_type) << 20) \
                                                            | ((event_format.event_type) << 16) | (event_format.module << 8) \
                                                            | (event_format.event), event_format.event_value, event_format.module_reserved, event_format.timestamp);
    return EVENT_OK;
}


void eventlog_printf(unsigned int DW1,unsigned int DW2, unsigned int DW3 ,unsigned int DW4)
{
    struct event_format event_format;

    event_format.syncbyte = (DW1 & 0xff000000)>>24;
    event_format.fw_type = (DW1 & 0x00f00000)>>20;
    event_format.module = (DW1 & 0x0000ff00)>>8;
    event_format.event_type = (DW1 & 0x000f0000)>>16;
    event_format.event = DW1 & 0x000000ff;
    event_format.event_value = DW2;
    event_format.module_reserved = DW3;
    /* time stamp */
    event_format.timestamp = DW4;

    ModuleDirectPrintf (MODULE_DEBUG, "%08x-%08x-%08x-%08x\n",((event_format.syncbyte) << 24) | ((event_format.fw_type) << 20) \
                                                            | ((event_format.event_type) << 16) | (event_format.module << 8) \
                                                            | (event_format.event), event_format.event_value, event_format.module_reserved, event_format.timestamp);
}


void eventlog_printf_all(void)
{
    int DW1 = 0;

    while  ((DW1+3) < memory_count)
    {
        eventlog_printf(log_memory[DW1], log_memory[DW1+1], log_memory[DW1+2], log_memory[DW1+3]);
        DW1 = DW1+4;
    }
}
