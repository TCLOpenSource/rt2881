#ifndef _RTK_HDMI_DP_COMMON_DRVIF_H_
#define _RTK_HDMI_DP_COMMON_DRVIF_H_

#include "rtk_kdriver/video_common/rtk_video_common.h"


#if  defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
extern VIDEO_COMMON_PORT drvif_video_common_get_common_port(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch);

#else

#define drvif_video_common_get_common_port(...)          VIDEO_COMMON_NONE     

#endif
#endif
