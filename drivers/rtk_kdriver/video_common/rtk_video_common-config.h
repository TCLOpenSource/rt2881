#ifndef __RTK_VIDEO_COMMON_CONFIG_H__
#define __RTK_VIDEO_COMMON_CONFIG_H__

// HW Capability
#ifdef CONFIG_ARCH_RTK2885P
#include "rtk_video_common_config-rtd2885p2.h"
#endif  // CONFIG_ARCH_RTK2885P

#ifdef CONFIG_ARCH_RTK2819A
#include "rtk_video_common_config-rtd2819a.h"
#endif  // CONFIG_ARCH_RTK2819A

#endif // __RTK_VIDEO_COMMON_CONFIG_H__
