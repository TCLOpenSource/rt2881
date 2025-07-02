#ifndef __RTK_MEASURE_PRIV_H__
#define __RTK_MEASURE_PRIV_H__

#include "rtk_measure-config.h"
#include "rtk_measure-osal.h"
#include "rtk_measure-debug.h"

// define
#define ANALOG_MODE_MEASUREMENT     0
#define DIGITAL_MODE_MEASUREMENT    1
#define MEASURE_FROM_TMDS_CLOCK     0   // 0: PIXEL CLOCK  1:TMDS CLOCK

typedef enum {
    OFFLINE_MS_1_PIXEL_MODE = 0x0,
    OFFLINE_MS_2_PIXEL_MODE = 0x1,
    OFFLINE_MS_4_PIXEL_MODE = 0x3,
} OFFMS_PIXEL_MODE_SEL;

// macro
#define MEASURE_ABS(x, y)       ((x > y) ? (x-y) : (y-x))

#define PLL_PIXELCLOCK_X1024_160M    163840 // 160*1024

// ext module
#if HDMI_MEASURE_I3DDMA_SMART_FIT
    #include <tvscalercontrol/scalerdrv/scalerdrv.h>
    #include <rbus/dma_vgip_reg.h>
#endif

#ifdef CONFIG_SUPPORT_SCALER
    #include <tvscalercontrol/scalerdrv/scalerdrv.h>
    #define SCALERDRV_WAIT_VS()         WaitFor_IVS1()
#else
    #define SCALERDRV_WAIT_VS()
#endif

// low level API
extern void lib_measure_offms_crt_on(void);
extern void lib_measure_offms_crt_reset(void);
extern unsigned char lib_measure_offms_start(void);
extern unsigned char lib_measure_offms_is_measure_start_pending(void);
extern unsigned char lib_skiptiming_condition(MEASURE_TIMING_T *timing);

#include "rtk_online_measure.h"

#endif // __RTK_MEASURE_PRIV_H__
