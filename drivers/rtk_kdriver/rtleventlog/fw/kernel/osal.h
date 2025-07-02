#ifndef __RTK_MODULE_EVENT_KERNEL_H__
#define __RTK_MODULE_EVENT_KERNEL_H__
#include <mach/rtk_timer.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>
/************************************************************************
 *  rtk log define
 ************************************************************************/
#ifdef CONFIG_RTK_EVENTLOG_NO_PRINTF
#define EV_EMERG(fmt, args...)

#define EV_ALERT(fmt, args...)

#define EV_CRIT(fmt, args...)

#define EV_ERR(fmt, args...)

#define EV_WARNING(fmt, args...)

#define EV_NOTICE(fmt, args...)

#define EV_INFO(fmt, args...)

#define EV_DEBUG(fmt, args...)
#else
#include <rtd_log/rtd_module_log.h>

#define EV_EMERG(fmt, args...)      \
       rtd_pr_eventlog_emerg(fmt , ## args)

#define EV_ALERT(fmt, args...)   \
       rtd_pr_eventlog_alert(fmt , ## args)

#define EV_CRIT(fmt, args...)   \
       rtd_pr_eventlog_crit(fmt , ## args)

#define EV_ERR(fmt, args...)   \
       rtd_pr_eventlog_err(fmt , ## args)

#define EV_WARNING(fmt, args...)   \
       rtd_pr_eventlog_warn(fmt , ## args)

#define EV_NOTICE(fmt, args...)   \
       rtd_pr_eventlog_notice(fmt , ## args)

#define EV_INFO(fmt, args...)   \
       rtd_pr_eventlog_info(fmt , ## args)

#define EV_DEBUG(fmt, args...)   \
       rtd_pr_eventlog_debug(fmt , ## args)
#endif

/************************************************************************
 * Define Save
 ************************************************************************/

#if defined(CONFIG_REALTEK_EVENT_LOG)
extern unsigned int kernel_event_save(unsigned int fw_type, unsigned int event_type, unsigned int module, unsigned int event, unsigned int event_val, unsigned int module_reserved);
#define rtd_kernel_event_log(event_type, module, event, event_val, module_reserved)    kernel_event_save(EVENT_FW_KERNEL, event_type, module, event, event_val, module_reserved)
#else /* CONFIG_REALTEK_EVENT_LOG */
#define event_log_kernel_save(fw_type, event_type, module, event, event_val, module_reserved)
#define rtd_kernel_event_log(event_type, module, event, event_val, module_reserved)
#endif /* CONFIG_REALTEK_EVENT_LOG */
#endif /*__RTK_MODULE_EVENT_KERNEL_H__*/