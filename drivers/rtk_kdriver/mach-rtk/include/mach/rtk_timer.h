#ifndef __RTK_TIMER_API__
#define __RTK_TIMER_API__

#include <rtk_kdriver/io.h>
#include <rbus/timer_reg.h>

#define rtk_time_cal(a,b)         \
         ((long)((b) - (a)))

#define rtk_timer_init_90k_ms_counter(timer)	\
({						\
	typecheck(unsigned long, timer);        \
	timer = rtk_timer_misc_90k_ms();	\
})

#define rtk_timer_diff_90k_ms_counter(timer)         	\
({                                              	\
	typecheck(unsigned long, timer);        	\
	timer = rtk_time_cal(timer, rtk_timer_misc_90k_ms());	\
})

#define rtk_timer_rst_90k_ms_counter rtk_timer_init_90k_ms_counter

#define rtk_timer_init_90k_us_counter(timer)        \
({                                              \
	typecheck(unsigned long, timer);	\
        timer = rtk_timer_misc_90k_us();        \
})

#define rtk_timer_diff_90k_us_counter(timer)         	\
({                                              	\
	typecheck(unsigned long, timer);        	\
	timer = rtk_time_cal(timer, rtk_timer_misc_90k_us());	\
})

#define rtk_timer_rst_90k_us_counter rtk_timer_init_90k_us_counter

/*******************************************************************/
// Belows (time_check) are for testing and profiling single function.
/*******************************************************************/

#define ___rtk_time_check(timer, func)                   	\
({                                                      	\
        rtk_timer_init_90k_ms_counter(timer);			\
               func;                                     	\
        rtk_timer_diff_90k_ms_counter(timer);                	\
})

#define rtk_time_check(timer, func)      \
({                                      \
        ___rtk_time_check(timer, func);  \
})

static inline __must_check unsigned long rtk_timer_misc_90k_getLO( void )
{
        unsigned long time = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        return time;
}

static inline __must_check unsigned long rtk_timer_misc_90k_ms( void )
{
        unsigned long time = rtd_inl(TIMER_SCPU_CLK90K_LO_reg)/90;
        return time;
}

static inline __must_check unsigned long rtk_timer_misc_90k_us( void )
{
	unsigned long time_90k_1 = 0;
    unsigned long time_90k_2 = 0;
    unsigned long time_27M_1 = 0;

    time_90k_1 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    time_27M_1 = rtd_inl(TIMER_SCPU_CLK27M_90K_reg);
    time_90k_2 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    if(time_90k_2 == time_90k_1)
        return (unsigned long)11*time_90k_1 + (unsigned long)(11*time_27M_1)/(unsigned long)300;
    else
        return 11*time_90k_2;        
}
#endif

