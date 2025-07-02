#ifndef __RTK_EVENT_UBOOT_H__
#define __RTK_EVENT_UBOOT_H__
#include <configs/rtk_common.h>
#include <timer_reg.h>
#include <common.h>
#include <io.h>
/************************************************************************
 *  rtk log define
 ************************************************************************/
#define EV_EMERG   printf

#define EV_ALERT    printf

#define EV_CRIT     printf

#define EV_ERR      printf

#define EV_WARNING  printf

#define EV_NOTICE   printf

#define EV_INFO     printf

#define EV_DEBUG    printf
/*
######################################################################################
# STATIC FUNCTION
######################################################################################
*/

extern unsigned int event_log_save_DW1(unsigned int DW1, unsigned int event_val, unsigned int module_reserved);

#endif /* __RTK_EVENT_UBOOT_H__ */