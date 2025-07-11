/*=============================================================
 * File:    dprx_edid.h
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
#ifndef __DPRX_EDID_RTK_DDC_H__
#define __DPRX_EDID_RTK_DDC_H__

#include <dprx_edid.h>

extern edid_dev_t* create_rtk_ddc_edid_device(unsigned char idx);
extern edid_dev_t* create_rtk_ddc_edid_device_ex(unsigned char idx, unsigned long flags);
#define RTK_DDC_EDID_QS_INIT        0x1             //for legacy code

#define RTK_DDC_FLAG_ENABLE_QS_INIT(data)               (0x1 & data)
#define RTK_DDC_FLAG_GET_ENABLE_QS_INIT(data)           (0x1 & data)

#define RTK_DDC_FLAG_AUX_ID(data)               (0x6 & (data<<1))
#define RTK_DDC_FLAG_GET_AUX_ID(data)           ((0x6 & data)>>1)
#endif // __DPRX_EDID_RTK_DDC_H__
