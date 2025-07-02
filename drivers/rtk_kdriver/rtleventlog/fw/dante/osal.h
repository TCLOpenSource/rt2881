#ifndef __RTK_EVENT_DANTE_OSAL_H__
#define __RTK_EVENT_DANTE_OSAL_H__
#include "module_list.h"
#include <arch.h>
#include <mach.h>
#include <rtk_io.h>
#include "rtk_kdriver/pm_printk.h"
/*
######################################################################################
# FW DEFINE
######################################################################################
*/
#define EVENT_FW_DANTE                  0x0b
#define EVENT_SYNCBYTE                  0x47
/*
######################################################################################
# MAX DEFINE
######################################################################################
*/
#define FW_MAX                      16
#define MODULE_MAX                 256
#define EVENT_MAX                  256
#define EVENT_TYPE_MAX              16
#define EVENT_LIMIT_MAX            256

/************************************************************************
 *  reg define
 ************************************************************************/

#define  TIMER_SCPU_CLK27M_90K                                                  0x1801B6B4
#define  TIMER_SCPU_CLK27M_90K_reg                                               0xB801B6B4


#define  TIMER_SCPU_CLK90K_LO                                                   0x1801B6B8
#define  TIMER_SCPU_CLK90K_LO_reg                                                0xB801B6B8

/************************************************************************
 *  RTD SIZE define
 ************************************************************************/
#define LOG_SIZE                        0x100

#define INT_LOG_SIZE                    (LOG_SIZE/4)

/************************************************************************
 *  rtk log define
 ************************************************************************/

#define EV_EMERG(fmt, args...)  pm_printk(0,fmt, ## args)

#define EV_ALERT(fmt, args...)   pm_printk(0,fmt, ## args)

#define EV_CRIT(fmt, args...)   pm_printk(0,fmt, ## args)

#define EV_ERR(fmt, args...)   pm_printk(0,fmt, ## args)

#define EV_WARNING(fmt, args...)   pm_printk(0,fmt, ## args)

#define EV_NOTICE(fmt, args...)   pm_printk(0,fmt, ## args)

#define EV_INFO(fmt, args...)   pm_printk(0,fmt, ## args)

#define EV_DEBUG (fmt, args...)   pm_printk(0,fmt, ## args)

/*
######################################################################################
# STATIC FUNCTION
######################################################################################
*/
//extern unsigned int event_log_save_DW1(UINT32 DW1, UINT32 event_val, UINT32 module_reserved);
extern void event_log_save_DW1(UINT32 DW1, UINT32 event_val) __banked;
/*
######################################################################################
# Define Save
######################################################################################
*/
#define MERGER_EVENT_DW1(fw,module,event_type,event) (((UINT32)(EVENT_SYNCBYTE) << 24) | ((UINT32)(fw) << 20)| ((UINT32)(event_type) << 16) | ((UINT32)module << 8)| ((UINT32)event))

#define rtd_dante_event_log(event_type, module, event, event_val, module_reserved)    event_log_save(EVENT_FW_EMCU, event_type, module, event, event_val, module_reserved)

#endif /* __RTK_EVENT_DANTE_OSAL_H__ */