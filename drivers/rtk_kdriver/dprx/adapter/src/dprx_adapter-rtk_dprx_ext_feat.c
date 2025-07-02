/*=============================================================
 * File:    dprx_adapter-rtk_dprx-wrapper.c
 *
 * Desc:    driver wrapper for DPRX low level driver
 *
 * AUTHOR:  kevin_wang@realtek.com
 *
 * Vresion: 0.0.1
 *
 *------------------------------------------------------------
 * Copyright (c) Realtek Semiconductor Corporation, 2022
 *
 * All rights reserved.
 *
 *============================================================*/
#include <dprx_platform.h>
#include <dprx_adapter-rtk_dprx.h>
#include <dprx_adapter-rtk_dprx-priv.h>
#ifdef CONFIG_OPTEE_HDCP14
#include <hdcp/hdcp14_optee.h>
#endif
#ifdef CONFIG_OPTEE_HDCP2
#include <hdcp/hdcp2_optee.h>
#endif

#ifndef CONFIG_OPTEE_HDCP2
#define optee_hdcp_set_protect(HDCP2_MODE, ch, enable)
#endif

static DPRX_HDCP_STATUS_T g_dprx_ext_hdcp_status[2];
//--------------------------------------------------
// Func  : dprx_ext_feat_clear_hdcp_status
//
// Desc  : clear hdcp status
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_feat_clear_hdcp_status(void)
{
    memset(&g_dprx_ext_hdcp_status[0], 0, sizeof(DPRX_HDCP_STATUS_T));
    memset(&g_dprx_ext_hdcp_status[1], 0, sizeof(DPRX_HDCP_STATUS_T));
    // optee_hdcp_set_protect(HDMI_DP_HDCP2_MODE_T hdmi_dp, unsigned char ch, unsigned short enable);
    DPRX_ADP_INFO("[HDCP]dprx_ext_feat_clear_hdcp_status(%d)\n", 0);
    optee_hdcp_set_protect(DP_OPTEE_HDCP2_MODE, 0, 0);
}

//--------------------------------------------------
// Func  : dprx_ext_feat_updata_hdcp_status
//
// Desc  : set hdcp protect
//
// Param : port : dprx port
//         enable   : 0 : protect 0, others : protect 1
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_feat_updata_hdcp_status(RTK_DPRX_ADP* p_dprx)
{
    DPRX_HDCP_STATUS_T hdcp_status;
    UINT8 idx;
    idx = p_dprx->mac_idx;

    rtk_dprx_get_hdcp_status(p_dprx, &hdcp_status);

    if(g_dprx_ext_hdcp_status[idx].mode != hdcp_status.mode)
    {
        g_dprx_ext_hdcp_status[idx].mode = hdcp_status.mode;
        // optee_hdcp_set_protect(HDMI_DP_HDCP2_MODE_T hdmi_dp, unsigned char ch, unsigned short enable);
        optee_hdcp_set_protect(DP_OPTEE_HDCP2_MODE, 0, (g_dprx_ext_hdcp_status[idx].mode > 0) ? 1 : 0);
        DPRX_ADP_INFO("[HDCP]dprx_ext_feat_updata_hdcp_status port = %d(%d)\n", idx, g_dprx_ext_hdcp_status[0].mode);
    }
}


