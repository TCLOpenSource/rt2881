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

extern RTK_DPRX_ADP* g_p_current_dprx_port;
extern edid_dev_t*   g_p_dprx_edid;
extern UINT8 g_mute_if_lane_dealign;
static dprx_hdcp2_message* hdcp2_handle_msg;


#include <dprx_vfe_api.h>
#include <dprx_vfe.h>
extern dprx_dev_t* _vfe_dprx_drv_get_dprx_adapter(unsigned char ch);
extern unsigned char g_multi_port_enable;

RTK_DPRX_ADP* _get_p_dprx_by_channel(UINT8 ch)
{
    dprx_dev_t* p_dev ;
    p_dev = _vfe_dprx_drv_get_dprx_adapter(ch);
    if(p_dev && p_dev->adp.p_private)
        return ((RTK_DPRX_ADP*) p_dev->adp.p_private);
    return NULL;
}

RTK_DPRX_ADP* GET_P_DPRX_BY_PORT(UINT8 port)
{
    RTK_DPRX_ADP* ret;
    if(!g_multi_port_enable)
    {
        return g_p_current_dprx_port;
    }
    else
    {
        int i=0;
        for(i = 0; i< MAX_DPRX_PORT;i++)
        {
            ret = _get_p_dprx_by_channel(i);
            if(ret)
                if(ret->mac_idx == port && ret->current_port)
                    return ret;
        }
    }
    return NULL;

}
edid_dev_t* GET_P_EDID_BY_PORT(UINT8 port)
{
    
    RTK_DPRX_ADP* ret;
    if(!g_multi_port_enable)
    {
        return g_p_dprx_edid;
    }else
    {
        int i=0;
        for(i = 0; i< MAX_DPRX_PORT;i++)
        {
            ret = _get_p_dprx_by_channel(i);
            if(ret)
                if(ret->mac_idx == port)
                    return ret->p_edid;
        }
    }
    return NULL;

}

//--------------------------------------------------
// Func  : dprx_ext_drvif_set_hotplug
//
// Desc  : set hpd level
//
// Param : port : dprx port
//         on   : 0 : hpd low, others : hpd high
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_set_hotplug(
    UINT8           port,
    UINT8           on
    )
{
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    if (p_dprx && p_dprx->p_hpd)
        dprx_hpd_set_hpd(p_dprx->p_hpd, on);
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_set_irq_hotplug
//
// Desc  : set irq hpd
//
// Param : port : dprx port
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_set_irq_hotplug(
    UINT8           port
    )
{
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    rtk_dprx_set_irq_hpd(p_dprx);
}


//--------------------------------------------------
// Func  : dprx_ext_drvif_get_hotplug_status
//
// Desc  : get hpd level
//
// Param : port : dprx port
//
// Retn  : 0 : hpd low, others : hpd high
//--------------------------------------------------
UINT8 dprx_ext_drvif_get_hotplug_status(
    UINT8           port
    )
{
    UINT8 ret = 0;
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    if (p_dprx && p_dprx->p_hpd)
        ret = dprx_hpd_get_hpd(p_dprx->p_hpd);

    return ret;
}


//--------------------------------------------------
// Func  : dprx_ext_drvif_get_cable_status
//
// Desc  : get cable connect status
//
// Param : port : dprx port
//
// Retn  : 0 : cable disconnect, others : cable connected
//--------------------------------------------------
UINT8 dprx_ext_drvif_get_cable_status(
    UINT8           port
    )
{
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    return (rtk_dprx_get_connect_status(p_dprx)) ? 1 : 0;
}


//--------------------------------------------------
// Func  : dprx_ext_drvif_ddc_reset
//
// Desc  : do ddc reset
//
// Param : port : dprx port
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_ddc_reset(
    UINT8           port
    )
{
    edid_dev_t* p_edid = GET_P_EDID_BY_PORT(port);
    if (p_edid)
        dprx_edid_reset(p_edid);
}



//--------------------------------------------------
// Func  : dprx_ext_drvif_set_hotplug_exint0
//
// Desc  : set hpd level
//
// Param : port : dprx port
//         on   : 0 : hpd low, others : hpd high
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_set_hotplug_exint0(
    UINT8           port,
    UINT8           on
    )
{
    dprx_ext_drvif_set_hotplug(port, on);
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_set_irq_hotplug_exint0
//
// Desc  : set irq hpd
//
// Param : port : dprx port
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_set_irq_hotplug_exint0(
    UINT8           port
    )
{
    dprx_ext_drvif_set_irq_hotplug(port);
}



//--------------------------------------------------
// Func  : dprx_ext_drvif_get_hotplug_status_exint0
//
// Desc  : get hpd level
//
// Param : port : dprx port
//
// Retn  : 0 : hpd low, others : hpd high
//--------------------------------------------------
UINT8 dprx_ext_drvif_get_hotplug_status_exint0(
    UINT8           port
    )
{
    return dprx_ext_drvif_get_hotplug_status(port);
}


//--------------------------------------------------
// Func  : dprx_ext_drvif_hdcp2_command
//
// Desc  : do hdcp2 handler
//
// Param : port : hdcp port id
//         send_buf : Send packet of hdcp messsage include command id and buffer.
//         send_size : packet size.
//         rev_buf : Recive packet.
//         rev_size : Recive packet size.
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_hdcp2_command(
    UINT8 port,
    UINT8 *send_buf,
    UINT32 send_size,
    UINT8 *rev_buf,
    UINT32 rev_size)
{
    //Fixed me
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_hdcp2_command_exint0
//
// Desc  : do hdcp2 handler for exint0
//
// Param : port : hdcp port id
//         send_buf : Send packet of hdcp messsage include command id and buffer.
//         send_size : packet size.
//         rev_buf : Recive packet.
//         rev_size : Recive packet size.
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_hdcp2_command_exint0(
    UINT8 port,
    UINT8 *send_buf,
    UINT32 send_size,
    UINT8 *rev_buf,
    UINT32 rev_size)
{
    //Fixed me
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_get_hdcp2_memory_exint0
//
// Desc  : get hdcp2 memory.
//
// Param : N/A
//
// Retn  : cert_rx (522 Bytes)
//--------------------------------------------------
dprx_hdcp2_message* dprx_ext_drvif_get_hdcp2_memory_exint0(void)
{
    if(hdcp2_handle_msg == NULL)
        hdcp2_handle_msg = dprx_osal_malloc(sizeof(dprx_hdcp2_message));

    return hdcp2_handle_msg;
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_get_hdcp2_memory
//
// Desc  : get hdcp2 memory.
//
// Param : N/A
//
// Retn  : cert_rx (522 Bytes)
//--------------------------------------------------
dprx_hdcp2_message* dprx_ext_drvif_get_hdcp2_memory(void)
{
    if(hdcp2_handle_msg == NULL)
        hdcp2_handle_msg = dprx_osal_malloc(sizeof(dprx_hdcp2_message));

    return hdcp2_handle_msg;
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_hdcp2_cmd
//
// Desc  : do hdcp2 handler
//
// Param : port : dprx port
//         dprx_hdcp2_message : hdcp2 msg
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_hdcp2_cmd(UINT8 port, dprx_hdcp2_message* msg)
{
#ifdef CONFIG_OPTEE_HDCP2
    if(*(msg->tx_msg_buf) == 8)
        msg->msg_id = HDCP2_CMD_POLLING_MESSAGE;
    else
        msg->msg_id = HDCP2_CMD_HANDLE_MESSAGE;

    optee_hdcp2_main(DP_OPTEE_HDCP2_MODE, msg->msg_id, port, msg->tx_msg_buf, msg->tx_length, msg->rx_msg_buf, msg->rx_length);
#endif
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_hdcp2_cmd_exint0
//
// Desc  : do hdcp2 handler for exint0
//
// Param : port : dprx port
//         dprx_hdcp2_message : hdcp2 msg
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_hdcp2_cmd_exint0(UINT8 port, dprx_hdcp2_message* msg)
{
#ifdef CONFIG_OPTEE_HDCP2
    if(*(msg->tx_msg_buf) == 8)
        msg->msg_id = HDCP2_CMD_POLLING_MESSAGE;
    else
        msg->msg_id = HDCP2_CMD_HANDLE_MESSAGE;

    optee_hdcp2_main(DP_OPTEE_HDCP2_MODE, msg->msg_id, port, msg->tx_msg_buf, msg->tx_length, msg->rx_msg_buf, msg->rx_length);
#endif
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_hdcp2_loadkey
//
// Desc  : load hdcp 2.2 key
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_hdcp2_loadkey(void)
{
#ifdef CONFIG_OPTEE_HDCP2
    UINT8 uc_lc128[16];
    optee_hdcp2_main(DP_OPTEE_HDCP2_MODE, HDCP2_CMD_LOAD_KEY, 0, uc_lc128, 16, uc_lc128, 16);
#endif
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_hdcp_loadkey
//
// Desc  : load hdcp 1.4 key
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_hdcp_loadkey(void)
{
#ifdef CONFIG_OPTEE_HDCP14
    optee_hdcp14_load_key(DP_OPTEE_HDCP_MODE, 1);
#endif
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_hdcp_get_key
//
// Desc  : load hdcp 1.4 key
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_hdcp_get_key(UINT8 *bksv, UINT8 bksv_len, UINT8* bkey, UINT16 bkey_len)
{
#ifdef CONFIG_OPTEE_HDCP14
    optee_hdcp14_get_key(bksv, bksv_len, bkey, bkey_len);
#endif
}

//--------------------------------------------------
// Func  : dprx_ext_drvif_get_video_common_ownership
//
// Desc  : Get Video Common ownership
//
// Param : port : dprx port
//
// Retn  : 0 : don't take ownership, 1: take ownership, others : failed
//--------------------------------------------------
UINT8 dprx_ext_drvif_get_video_common_ownership(UINT8 port)
{
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);

    if(dprx_video_common_check_ownership(p_dprx->p_vc))
        return 1;

    return 0;
}


#ifdef DPRX_ENABLE_EXT_EVENT_HANDLER

//--------------------------------------------------
// Func  : _proc_link_event
//
// Desc  : dprx link event handler
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
static void _proc_link_event(
    UINT8           port,
    DPRX_EVENT_T    event_id
    )
{
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    //DPRX_ADP_WARN("_proc_link_event (event=%04x)!!!\n", event_id);
    switch(GET_EVENT(event_id))
    {
    case LINK_TARIN_STATUS_UPDATE:
        if (p_dprx)
            rtk_dprx_handle_lt_state_update_event(p_dprx, GET_EVENT_PARM(event_id));
        break;

    case LINK_STATUS_UPDATE:
        if (g_mute_if_lane_dealign)
        {
            DPRX_ADP_WARN("lane align failed, do avmute!!!\n");
            dprx_drv_do_avmute(0);  // mute video
        }
        else
        {
            DPRX_ADP_WARN("lane align failed, do nothing!!!\n");
        }
        break;

    case LINK_REQUEST_HPD_IRQ:
        if (p_dprx)
            rtk_dprx_set_irq_hpd(p_dprx);
        break;
    default:
        break;
    }
}

//--------------------------------------------------
// Func  : _proc_sdp_event
//
// Desc  : dprx link event handler
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
static void _proc_sdp_event(
    UINT8           port,
    DPRX_EVENT_T    event_id
    )
{
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    DPRX_ADP_DBG("_proc_sdp_event (event=%04x)!!!\n", event_id);

    switch(GET_EVENT(event_id))
    {
    case SDP_UPDATE:
        rtk_dprx_handle_infoframe_sdp_update_event(p_dprx, GET_EVENT_PARM(event_id));
        break;

    case SDP_CHANGE:
        break;

    default:
        // do nothing
        break;
    }
}

//--------------------------------------------------
// Func  : _proc_hdcp_event
//
// Desc  : dprx link event handler
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
static void _proc_hdcp_event(
    UINT8           port,
    DPRX_EVENT_T    event_id
    )
{
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    UINT8 hdcp_event_id = 0;
    DPRX_ADP_WARN("_proc_hdcp_event (event=%04x)!!!\n", event_id);

    if(p_dprx == NULL)
    {
        DPRX_ADP_WARN("_proc_hdcp_event g_p_current_dprx_port = NULL !!!\n");
        return;
    }

    hdcp_event_id = (UINT8)(event_id&0xF);

#ifdef DPRX_EXT_HDCP_EMERGENCY_HANDLER
    if(hdcp_event_id == _HDCP22_AKE_INIT)
    {
        dprx_drv_set_hdcp_emergency_start_time(port, dprx_odal_get_system_time_ms());
        dprx_drv_set_hdcp_emergency_flag(port, 1);
        DPRX_ADP_WARN("dprx_drv_set_hdcp_emergency_flag = 1\n");
        if(g_multi_port_enable)
        {
            p_dprx->hdcp2x_event_rp = 0;
            p_dprx->hdcp2x_event_wp = 0;

        }else
        {
            // clear event rp/wp status
            g_p_current_dprx_port->hdcp2x_event_rp = 0;
            g_p_current_dprx_port->hdcp2x_event_wp = 0;
        }
    }
    if(hdcp_event_id == _HDCP22_SKE_TYPE_VALUE)
    {
        dprx_drv_set_hdcp_emergency_start_time(port, 0);
        dprx_drv_set_hdcp_emergency_flag(port, 0);
        DPRX_ADP_WARN("dprx_drv_set_hdcp_emergency_flag = 0\n");
    }
#endif

    p_dprx->hdcp2x_event_msg_id[p_dprx->hdcp2x_event_wp++] = hdcp_event_id;
    if(p_dprx->hdcp2x_event_wp >= DPRX_MAX_HDCP_EVENT_QUEUE)
    {
        p_dprx->hdcp2x_event_wp = 0;
    }
}

#ifdef DPRX_EVENT_TYPE_VIDEO_COMMON
//--------------------------------------------------
// Func  : _proc_video_common_event
//
// Desc  : dprx link event handler
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
static void _proc_video_common_event(
    UINT8           port,
    DPRX_EVENT_T    event_id
    )
{
#ifdef DPRX_EVENT_TYPE_VIDEO_COMMON
    RTK_DPRX_ADP* p_dprx = GET_P_DPRX_BY_PORT(port);
    UINT8 vc_event_id = 0;
    DPRX_ADP_WARN("_proc_video_common_event (event=%04x)!!!\n", event_id);

    vc_event_id = (UINT8)GET_EVENT_PARM(event_id);

    switch(GET_EVENT(event_id))
    {
    case VIDEO_COMMON_EVENT_YUV420_TO_444:
        dprx_video_common_set_yuv420_to_444(p_dprx->p_vc, vc_event_id);
        break;

    case VIDEO_COMMON_EVENT_OUT_PIXEL_MODE:
        if(vc_event_id == 1)
            dprx_video_common_set_output_pixel_mode(p_dprx->p_vc, DPRX_VC_PIXEL_MODE_ONE_PIXEL);
        else
            dprx_video_common_set_output_pixel_mode(p_dprx->p_vc, DPRX_VC_PIXEL_MODE_TWO_PIXEL);
        break;

    case VIDEO_COMMON_EVENT_IN_PIXEL_MODE:
        if(vc_event_id == 1)
            dprx_video_common_set_input_pixel_mode(p_dprx->p_vc, DPRX_VC_PIXEL_MODE_ONE_PIXEL);
        else
            dprx_video_common_set_input_pixel_mode(p_dprx->p_vc, DPRX_VC_PIXEL_MODE_TWO_PIXEL);
        break;

    case VIDEO_COMMON_EVENT_DSCD_IN:
        dprx_video_common_set_dsc_input_sel(p_dprx->p_vc, vc_event_id);
        break;

#ifdef VIDEO_COMMON_EVENT_SET_VSYNC_INVERSE
    case VIDEO_COMMON_EVENT_SET_VSYNC_INVERSE:
        dprx_video_common_set_vsync_inverse(p_dprx->p_vc,vc_event_id);
        break;
    case VIDEO_COMMON_EVENT_SET_HSYNC_INVERSE:
        dprx_video_common_set_hsync_inverse(p_dprx->p_vc,vc_event_id);
        break;
#endif

#ifdef VIDEO_COMMON_EVENT_V_DROP_EN
    case VIDEO_COMMON_EVENT_V_DROP_EN:
        dprx_video_common_set_line_drop_en(p_dprx->p_vc, vc_event_id);
        break;
#endif
#ifdef VIDEO_COMMON_EVENT_SET_COLOR_SPAEC
    case VIDEO_COMMON_EVENT_SET_COLOR_SPAEC:
        dprx_video_common_set_color_space(p_dprx->p_vc, vc_event_id);
        break;
#endif
#ifdef VIDEO_COMMON_EVENT_DP_TYPE
    case VIDEO_COMMON_EVENT_DP_TYPE:
        dprx_video_common_set_src_type(p_dprx->p_vc, vc_event_id);
        break;
#endif
#ifdef VIDEO_COMMON_EVENT_DP_CLK_DIV_EN
    case VIDEO_COMMON_EVENT_DP_CLK_DIV_EN:
        dprx_video_common_set_clk_div_en(p_dprx->p_vc, vc_event_id);
        break;
#endif
    default:
        // do nothing
        break;
    }
#endif
}
#endif

//--------------------------------------------------
// Func  : dprx_ext_drvif_event_handler
//
// Desc  : dprx event handler
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_drvif_event_handler(
    UINT8           port,
    DPRX_EVENT_T    event_id
    )
{
    // DPRX_ADP_WARN("dprx_ext_drvif_event_handler (event=%04x)!!!\n", event);

    switch(GET_EVENT_TYPE(event_id))
    {
    case DPRX_EVENT_TYPE_LINK:
        _proc_link_event(port, event_id);
        break;
    case DPRX_EVENT_TYPE_SDP:
        _proc_sdp_event(port, event_id);
        break;
    case DPRX_EVENT_TYPE_HDCP:
        _proc_hdcp_event(port, event_id);
        break;
#ifdef DPRX_EVENT_TYPE_VIDEO_COMMON
    case DPRX_EVENT_TYPE_VIDEO_COMMON:
        _proc_video_common_event(port, event_id);
        break;
#endif
    default:
        // do nothing
        break;
    }
}

#endif
