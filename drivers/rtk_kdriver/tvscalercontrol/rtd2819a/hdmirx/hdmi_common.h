#ifndef __HDMI_COMMON_H_
#define __HDMI_COMMON_H_

#ifndef BUILD_QUICK_SHOW
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/semaphore.h>
//#include <mach/io.h>
#include <rtk_kdriver/io.h>
#include <rtd_log/rtd_module_log.h>
#include <rbus/rbus_types.h>
#else
#include <sysdefs.h>
#endif

#include <tvscalercontrol/hdmirx/hdmifun.h>

// hdmiHdmi
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/scaler/state.h>
#include <tvscalercontrol/scalerdrv/mode.h>

#include "hdmi_debug.h"
#include "hdmiPlatform.h"
#include "hdmiInternal.h"
#include "hdmirx_osal.h"
#include "hdmi_dp_common_sel.h"
#endif
