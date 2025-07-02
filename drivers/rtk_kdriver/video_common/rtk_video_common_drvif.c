/*=============================================================
 * File:    rtk_hdmi_dp_common_drvif.c
 *
 * Desc:    RTK hdmi dp common drvif
 *
 * AUTHOR:  xiao_tim@realtek.com
 *
 * Vresion: 0.0.2
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2024
 *
 * All rights reserved.
 *============================================================*/
#include <rtk_kdriver/video_common/rtk_video_common.h>
#include <rtk_kdriver/video_common/rtk_video_common_drvif.h>
#include "rtk_video_common-priv.h"
/**********************************************************************************************
*
*   Variables
*
**********************************************************************************************/

/**********************************************************************************************
*
*   FUNCTION Body
*
**********************************************************************************************/
extern unsigned char _hdmi_dp_common_get_video_common_with_ch_port(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch);

/*------------------------------------------------
 * Func : drvif_video_common_get_common_port
 *
 * Desc : get video common port
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *
 * Retn : N/A
 *-----------------------------------------------*/
VIDEO_COMMON_PORT drvif_video_common_get_common_port(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch)
{
    unsigned char nport = 0xf;

    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        RTK_VIDEO_COMMON_ERR("%s ,VIDEO_COMMON_PORT_IN_SEL = (%d, %d), Don't allocate on common port.\n", __func__ , src, ch);
        return VIDEO_COMMON_NONE;
    }

    RTK_VIDEO_COMMON_ERR("%s ,VIDEO_COMMON_PORT_IN_SEL = (%d, %d), Allocate on common port = %d.\n", __func__ , src, ch, nport);

    return nport;
}
EXPORT_SYMBOL(drvif_video_common_get_common_port);


MODULE_LICENSE("GPL");
