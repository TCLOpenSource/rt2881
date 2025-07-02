#include <fw/uboot/osal.h>
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


unsigned int event_log_save_DW1(unsigned int DW1, unsigned int event_val, unsigned int module_reserved)
{
	unsigned int timestamp = 0;

	timestamp =  (unsigned int) rtd_inl(TIMER_SCPU_CLK90K_LO)*11;
	timestamp += (unsigned int) (11*rtd_inl(TIMER_SCPU_CLK27M_90K))/300;
	EV_EMERG("EVENTLOG:%x-%x-%x-%x\n",(unsigned int) DW1, (unsigned int) event_val, (unsigned int) module_reserved, (unsigned int)timestamp);


	return 0;
}
