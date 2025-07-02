#ifndef __RTK_TYPE_DEBUG_H__
#define __RTK_TYPE_DEBUG_H__
#include <mach/rtk_log.h>
#include <rtd_log/rtd_module_log.h>

#define RTK_TYPEC_DBG(fmt, args...)           rtd_pr_usb_hub_info("[Info]" fmt, ##args)

#define RTK_TYPEC_INFO(fmt, args...)       rtd_pr_usb_hub_info("[Info]" fmt, ##args)
#define RTK_TYPEC_ERR(fmt, args...)       rtd_pr_usb_hub_info("[Info]" fmt, ##args)

#endif
