// TV Frontend OSAL Driver
//

#ifndef __TV_OSAL_H__
#define __TV_OSAL_H__

//#include "OSAL.h"
#include "rtk_tv_fe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void tv_osal_msleep_interruptible_ext(unsigned long ms);
void tv_osal_msleep_ext(unsigned long ms);
void tv_osal_usleep_ext(unsigned long us);

unsigned long tv_osal_time_ext(void);

void* tv_osal_malloc_ext(unsigned long size);
void tv_osal_free_ext(void *ptr);

typedef struct mutex   tv_mutex;


void tv_mutex_create_ext(tv_mutex* p_mutex);
void tv_mutex_destroy_ext(tv_mutex* p_mutex);
void tv_mutex_lock_ext(tv_mutex* p_mutex);
void tv_mutex_unlock_ext(tv_mutex* p_mutex);

#ifdef __cplusplus
}
#endif

#endif //__TV_OSAL_H__
