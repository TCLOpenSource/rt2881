/*=============================================================
 * File:    dprx_adapter-rtk_dprx.h
 *
 * Desc:    DPRX Adapter
 *
 * AUTHOR:  kevin_wang@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2021
 *
 * All rights reserved.
 *
 *============================================================*/
#ifndef __DPRX_ADAPTER_RTK_DPRX_H__
#define __DPRX_ADAPTER_RTK_DPRX_H__

#include <dprx_adapter.h>

extern dprx_dev_t* create_rtk_dprx_adapter(DP_PORT_CFG* p_cfg, DP_FLOW_CFG* p_flow_cfg);
#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
extern int set_dolby_vision_check_mode_function(long (*fp)(unsigned char));
#endif

#endif // __DPRX_ADAPTER_RTK_DPRX_H__
