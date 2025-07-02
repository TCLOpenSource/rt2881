// TV Frontend OSAL Driver
//
//#include <stdlib.h>
//#include <windows.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <string.h>
#include <linux/slab.h>
#include "tv_osal.h"
#include <linux/time.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/err.h>
#include <linux/unistd.h>
#include <linux/file.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/random.h>

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <mach/rtk_timer.h>

void tv_osal_msleep_interruptible_ext(unsigned long ms)
{
	msleep_interruptible(ms);

}

void tv_osal_msleep_ext(unsigned long ms)
{
	if (ms > 20)
		msleep_interruptible(ms);
	else
		usleep_range(ms * 1000, ms * 1000);

}

void tv_osal_usleep_ext(unsigned long us)
{
	if (us > 20000)
		msleep_interruptible(us / 1000);
	else if ((us <= 20000) && (us > 10))
		usleep_range(us, us);
	else
		udelay(us);
}

unsigned long tv_osal_time_ext(void)
{

#if 0
	struct timeval new_time;
	do_gettimeofday(&new_time);
	return (new_time.tv_sec * 1000 + (new_time.tv_usec / 1000));
#else
	return rtk_timer_misc_90k_ms();
#endif



}

void* tv_osal_malloc_ext(unsigned long size)
{
	return (void*) kmalloc(size, GFP_KERNEL);
}

void tv_osal_free_ext(void *ptr)
{
	kfree(ptr);
}

void tv_mutex_create_ext(tv_mutex* p_mutex)
{
	mutex_init(p_mutex);
}


void tv_mutex_destroy_ext(tv_mutex* p_mutex)
{
	mutex_destroy(p_mutex);
}


void tv_mutex_lock_ext(tv_mutex* p_mutex)
{
	mutex_lock(p_mutex);
}


void tv_mutex_unlock_ext(tv_mutex* p_mutex)
{
	mutex_unlock(p_mutex);
}

#if 0
static void tv_thread(void *param)
{

	((TvThread*) param)->Run();

}
#endif

