/*=============================================================
 * File:    dprx_video_common-rtktv.h
 *
 * Desc:    DPRX VIDEO COMMON
 *
 * AUTHOR:  xiao_tim@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2021
 *
 * All rights reserved.
 *
 *============================================================*/
#ifndef __DPRX_VIDEO_COMMON_RTK_TV_H__
#define __DPRX_VIDEO_COMMON_RTK_TV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <dprx_platform.h>
#include <dprx_video_common.h>

//================= API of DPRX HPD ==============

extern vc_dev_t* create_rtk_tv_dprx_video_common_device(unsigned char mac_id);
extern vc_dev_t* create_rtk_tv_dprx_video_common_device_ex(unsigned char mac_id, unsigned long flags);

#ifdef __cplusplus
}
#endif

#endif // __DPRX_VIDEO_COMMON_RTK_TV_H__
