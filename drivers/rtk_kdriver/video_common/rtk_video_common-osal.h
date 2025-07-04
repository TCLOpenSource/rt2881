#ifndef __RTK_HDMI_DP_COMMON_OSAL_H__
#define __RTK_HDMI_DP_COMMON_OSAL_H__

#include <io.h>  // for rtd_inl / rtd_outl

#ifdef BUILD_QUICK_SHOW
    #include <no_os/export.h>
    #include <div64.h>
    #include <malloc.h>
    #include <timer.h>
    #include <string.h>
#else
    #include <linux/delay.h>
    #include <linux/uaccess.h>
    #include <mach/rtk_timer.h>
    #include <linux/module.h>
#endif

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#ifndef NULL
#define NULL            ((void *)0)
#endif

#endif // __RTK_HDMI_DP_COMMON_OSAL_H__
