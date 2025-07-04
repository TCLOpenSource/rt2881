#ifndef BUILD_QUICK_SHOW
#include <io.h>
#include <rtd_log/rtd_module_log.h>
#endif
#include <rbus/rbus_types.h>


#include <tvscalercontrol/hdmirx/hdmifun.h>

// hdmiHdmi
//#if IS_ENABLED(CONFIG_SUPPORT_SCALER)
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/scaler/state.h>
#include <tvscalercontrol/scalerdrv/mode.h>
//#endif

#include "hdmi_debug.h"
#include "hdmiPlatform.h"
#include "hdmiInternal.h"

#include "hdmirx_osal.h"

