/*=============================================================
 * File:    dprx_adapter-rtk_dprx-priv.c
 *
 * Desc:    Internal driver for RTK DPRX adapter
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

#include <dprx_adapter-rtk_dprx.h>
#include <dprx_adapter-rtk_dprx-priv.h>
#ifdef CONFIG_DPRX_GET_IC_VERSION
#include <mach/platform.h>
#endif
#define DEFAULT_RESUME_GUARD_TIME_INTERVAL_PERIOD_MS       3000
UINT16 g_resume_guard_time_interval = DEFAULT_RESUME_GUARD_TIME_INTERVAL_PERIOD_MS;
edid_dev_t*   g_p_dprx_edid = NULL;
extern dprx_priv_ops_t g_rtk_priv_ex_ops;
RTK_DPRX_ADP* g_p_current_dprx_port = NULL;
DPRX_SDP_HANDLER g_dprx_sdp_handler;
UINT32 g_dprx_sdp_debug_ctrl = 0;
//legacy code. shall remove in following patch.
UINT8 g_force_demode = 0;
UINT8 g_force_timing_ext_flag_en = 0;  // for test
UINT8 g_force_timing_ext_flag = 0;     // for test
UINT8 g_force_max_lane_count = 0;  // disabled
UINT8 g_force_max_link_rate = 0; // disabled
UINT8 g_force_fake_tps1_lt = 0;  // disabled
UINT8 g_force_fake_tps2_lt = 0;  // disabled
UINT8 g_skip_source_detect = 0;  // disable detect flow to for debug
UINT8 g_mute_if_lane_dealign = 1;
UINT8 g_force_freesync_off = 0;
#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP
long (*fp_get_dolby_vision_dp_mode)(unsigned char);
#endif

//#define FORCE_DEFAULT_EDID
#ifdef FORCE_DEFAULT_EDID

// ARG Free Sync EDID
UINT8 default_edid[] =
{
    0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x06,0xb3,0x2a,0x27,0xff,0xff,0xff,0xff,
    0x27,0x1d,0x01,0x04,0xb5,0x3c,0x22,0x78,0x3b,0x0b,0x10,0xb0,0x4c,0x45,0xa8,0x26,
    0x0a,0x50,0x54,0xbf,0xcf,0x00,0x71,0x4f,0x81,0xc0,0x81,0x40,0x81,0x80,0xd1,0xc0,
    0xd1,0xfc,0x95,0x00,0xb3,0x00,0x4d,0xd0,0x00,0xa0,0xf0,0x70,0x3e,0x80,0x30,0x20,
    0x35,0x00,0x54,0x4f,0x21,0x00,0x00,0x1a,0x00,0x00,0x00,0xfd,0x0c,0x30,0x90,0x4a,
    0x4a,0x81,0x01,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfc,0x00,0x52,
    0x4f,0x47,0x20,0x58,0x47,0x32,0x37,0x55,0x51,0x0a,0x20,0x20,0x00,0x00,0x00,0xff,
    0x00,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x02,0xb4,
    0x02,0x03,0x3a,0xf2,0x50,0x60,0x61,0x01,0x02,0x03,0x11,0x12,0x13,0x04,0x0e,0x0f,
    0x1d,0x1e,0x1f,0x10,0x3f,0xe2,0x00,0xd5,0x23,0x09,0x07,0x07,0x83,0x01,0x00,0x00,
    0x65,0x03,0x0c,0x00,0x10,0x00,0xe3,0x05,0xc0,0x00,0xe6,0x06,0x05,0x01,0x70,0x70,
    0x07,0x68,0x1a,0x00,0x00,0x01,0x0b,0x30,0x90,0x00,0x56,0x5e,0x00,0xa0,0xa0,0xa0,
    0x29,0x50,0x30,0x20,0x35,0x00,0x54,0x4f,0x21,0x00,0x00,0x1a,0x86,0xbc,0x00,0x50,
    0xa0,0xa0,0x55,0x50,0x08,0x20,0x78,0x00,0x54,0x4f,0x21,0x00,0x00,0x1e,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x87,
    0x70,0x12,0x67,0x00,0x00,0x03,0x01,0x64,0x9b,0x12,0x01,0x04,0xff,0x0e,0x4f,0x00,
    0x07,0x00,0x1f,0x00,0x6f,0x08,0x52,0x00,0x44,0x00,0x07,0x00,0x15,0x4c,0x01,0x04,
    0xff,0x0e,0x3b,0x00,0x07,0x80,0x1f,0x00,0x6f,0x08,0x40,0x00,0x02,0x00,0x04,0x00,
    0x7b,0x5a,0x01,0x04,0xff,0x0e,0x4f,0x00,0x07,0x00,0x1f,0x00,0x6f,0x08,0x68,0x00,
    0x5a,0x00,0x07,0x00,0x3a,0x9b,0x01,0x04,0xff,0x0e,0x4f,0x00,0x07,0x80,0x1f,0x00,
    0x6f,0x08,0x4d,0x00,0x02,0x00,0x04,0x00,0x47,0xf8,0x01,0x04,0xff,0x0e,0x4f,0x00,
    0x07,0x00,0x1f,0x00,0x6f,0x08,0x7e,0x00,0x70,0x00,0x07,0x00,0x52,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,
};

#endif


#ifdef VFSM_0_WAIT_LINK_READY

const char* _get_dprx_vfsm_str(UINT8 vfsm)
{
    switch(vfsm)
    {
    case VFSM_0_WAIT_LINK_READY:        return "WAIT_LINK_READY";
    case VFSM_1_WAIT_VIDEO_STREAM_PRESENT:  return "WAIT_VIDEO_STREAM_PRESENT";
    case VFSM_2_WAIT_MSA_INFO:          return "WAIT_MSA_INFO";
    case VFSM_3_GET_VIDEO_INFO:         return "GET_VIDEO_INFO";
    case VFSM_4_GET_MEASURE_INFO:       return "MEASURE";
    case VFSM_5_SET_DISP_FORMAT:        return "SET_DISP_FORMAT";
    case VFSM_6_SET_STREAM_CLOCK_GEN:   return "SET_STREAM_CLOCK_GEN";
    case VFSM_7_ENABLE_VPLL_TRACKING:   return "ENABLE_PLL_TRACKING";
    case VFSM_8_CHK_VIDEO_FIFO:         return "CHK_VIDEO_FIFO";
    case VFSM_9_VIDEO_OUT_ENABLE:       return "VIDEO_ENABLE";
#ifdef VFSM_16_DSC_OUT_ENABLE
    case VFSM_10_DSC_SET_IPNUT:         return "DSC_SET_IPNUT";
    case VFSM_11_DSC_WAIT_PPS:          return "DSC_WAIT_PPS";
    case VFSM_12_DSC_GET_LINK_INFO:     return "DSC_GET_LINK_INFO";
    case VFSM_13_DSC_SET_PLL:           return "DSC_SET_PLL";
    case VFSM_14_DSC_DEC_ENABLE:        return "DSC_DEC_ENABLE";
    case VFSM_15_DSC_CHK_STATUS:        return "DSC_CHK_STATUS";
    case VFSM_16_DSC_OUT_ENABLE:        return "DSC_OUT_ENABLE";
#endif
    default:                            return "Unknwon";
    }
}

#endif


#ifdef CONFIG_ENABLE_TYPEC_DPRX
DPRX_PORT_CONFIG g_dprx_typec_cable_cfg[] =
{
    {.aux_pn_swap = 1, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 1, .lane2_mapping = 3, .lane1_mapping = 0, .lane0_mapping = 2},         //Case A
    {.aux_pn_swap = 1, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 1, .lane2_mapping = 0, .lane1_mapping = 3, .lane0_mapping = 2},         //Case B
    {.aux_pn_swap = 1, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 1, .lane2_mapping = 0, .lane1_mapping = 2, .lane0_mapping = 3},         //Case C
    {.aux_pn_swap = 1, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 1, .lane2_mapping = 0, .lane1_mapping = 2, .lane0_mapping = 3},         //Case D
    {.aux_pn_swap = 0, .lane3_pn_swap = 0, .lane2_pn_swap = 1, .lane1_pn_swap = 0, .lane0_pn_swap = 1, .lane3_mapping = 2, .lane2_mapping = 3, .lane1_mapping = 1, .lane0_mapping = 0},         //Case E

    {.aux_pn_swap = 0, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 3, .lane2_mapping = 1, .lane1_mapping = 2, .lane0_mapping = 0},         //Case A Flip
    {.aux_pn_swap = 0, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 3, .lane2_mapping = 2, .lane1_mapping = 1, .lane0_mapping = 0},         //Case B Flip
    {.aux_pn_swap = 0, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 3, .lane2_mapping = 2, .lane1_mapping = 0, .lane0_mapping = 1},         //Case C Flip
    {.aux_pn_swap = 0, .lane3_pn_swap = 1, .lane2_pn_swap = 0, .lane1_pn_swap = 1, .lane0_pn_swap = 0, .lane3_mapping = 3, .lane2_mapping = 2, .lane1_mapping = 0, .lane0_mapping = 1},         //Case D Flip
    {.aux_pn_swap = 1, .lane3_pn_swap = 0, .lane2_pn_swap = 1, .lane1_pn_swap = 0, .lane0_pn_swap = 1, .lane3_mapping = 0, .lane2_mapping = 1, .lane1_mapping = 3, .lane0_mapping = 2},         //Case E Flip
};
#endif

#ifdef DPRX_EXT_IC_VERSION
    #define DPRX_DRV_SET_IC_VERSION(port, val)              dprx_drv_set_ext_config(port, DPRX_EXT_IC_VERSION, val);
#else
    #define DPRX_DRV_SET_IC_VERSION(port, val)
#endif

void _set_dprx_ic_version(UINT8 port)
{
    #ifdef CONFIG_DPRX_GET_IC_VERSION
    switch(get_ic_version())
    {
        case VERSION_A:
            DPRX_DRV_SET_IC_VERSION(port, 0);
            break;
        case VERSION_B:
            DPRX_DRV_SET_IC_VERSION(port, 1);
            break;
        default:
            DPRX_DRV_SET_IC_VERSION(port, 0);
    }
    #endif
}

#ifdef CONFIG_ENABLE_TYPEC_DPRX

/*------------------------------------------------
 * Func : rtk_dprx_set_typec_port_config
 *
 * Desc : set typeC config
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_set_typec_port_config(RTK_DPRX_ADP* p_dprx, DPRX_CABLE_CFG* p_type_c_dp_cfg)
{
    int type_id = 0;
    if ((p_type_c_dp_cfg==NULL)||(p_dprx==NULL))
        return -1;

    switch(*p_type_c_dp_cfg)
    {
        case CABLE_CFG_TYPEC_CFG_A: type_id = 0; break;
        case CABLE_CFG_TYPEC_CFG_B: type_id = 1; break;
        case CABLE_CFG_TYPEC_CFG_C: type_id = 2; break;
        case CABLE_CFG_TYPEC_CFG_D: type_id = 3; break;
        case CABLE_CFG_TYPEC_CFG_E: type_id = 4; break;
        case CABLE_CFG_TYPEC_CFG_A_FLIP: type_id = 5; break;
        case CABLE_CFG_TYPEC_CFG_B_FLIP: type_id = 6; break;
        case CABLE_CFG_TYPEC_CFG_C_FLIP: type_id = 7; break;
        case CABLE_CFG_TYPEC_CFG_D_FLIP: type_id = 8; break;
        case CABLE_CFG_TYPEC_CFG_E_FLIP: type_id = 9; break;
        default:
            type_id = 0;
            break;
    }

    p_dprx->cable_type = *p_type_c_dp_cfg;
    p_dprx->port_cfg.aux_pn_swap = g_dprx_typec_cable_cfg[type_id].aux_pn_swap;
    p_dprx->port_cfg.lane0_pn_swap = g_dprx_typec_cable_cfg[type_id].lane0_pn_swap;
    p_dprx->port_cfg.lane1_pn_swap = g_dprx_typec_cable_cfg[type_id].lane1_pn_swap;
    p_dprx->port_cfg.lane2_pn_swap = g_dprx_typec_cable_cfg[type_id].lane2_pn_swap;
    p_dprx->port_cfg.lane3_pn_swap = g_dprx_typec_cable_cfg[type_id].lane3_pn_swap;
    p_dprx->port_cfg.lane0_mapping = g_dprx_typec_cable_cfg[type_id].lane0_mapping;
    p_dprx->port_cfg.lane1_mapping = g_dprx_typec_cable_cfg[type_id].lane1_mapping;
    p_dprx->port_cfg.lane2_mapping = g_dprx_typec_cable_cfg[type_id].lane2_mapping;
    p_dprx->port_cfg.lane3_mapping = g_dprx_typec_cable_cfg[type_id].lane3_mapping;

    DP_PLAT_INFO("lane0_phy_idx = %d, lane1_phy_idx = %d, lane2_phy_idx = %d, lane3_phy_idx = %d, \
                 lane0_pn_swap = %d,  lane1_pn_swap = %d, lane2_pn_swap = %d, lane3_pn_swap = %d\n" , \
                p_dprx->port_cfg.lane0_mapping, p_dprx->port_cfg.lane1_mapping, p_dprx->port_cfg.lane2_mapping, \
                p_dprx->port_cfg.lane3_mapping, p_dprx->port_cfg.lane0_pn_swap, p_dprx->port_cfg.lane0_pn_swap, \
                p_dprx->port_cfg.lane2_pn_swap, p_dprx->port_cfg.lane3_pn_swap);
    return 1;
}

#endif

const char* _get_dprx_hdcp_state_str(DP_HDCP_MODE mode)
{
    switch(mode)
    {
    case DP_HDCP_MODE_OFF:              return "HDCP_OFF";
    case DP_HDCP_MODE_HDCP_1P3:         return "HDCP_13";
    case DP_HDCP_MODE_HDCP_2P2:         return "HDCP_22";
    case DP_HDCP_MODE_HDCP_2P3:         return "HDCP_23";
    default:                            return "Unknwon";
    }
}

const char* sdp_type_str(SDP_TYPE_E sdp_type)
{
    switch(sdp_type)
    {
    case SDP_TYPE_AUD_TIMESTAMP:       return "AUDIO_TIME_STAMP";
    case SDP_TYPE_AUD_STREAM:          return "AUDIO_STREAM";
    case SDP_TYPE_EXTENSION:           return "EXT";
    case SDP_TYPE_AUD_COPY_MANAGEMENT: return "ACM";
    case SDP_TYPE_ISRC:                return "ISRC";
    case SDP_TYPE_VSC:                 return "VSC";
    case SDP_TYPE_PPS:                 return "PPS";
    case SDP_TYPE_VSC_EXT_VESA:        return "VSC_EXT_VESA";
    case SDP_TYPE_VSC_EXT_CEA:         return "VSC_EXT_CEA";
    case SDP_TYPE_ADAPTIVE_SYNC_SDP:   return "ADPT_SYNC";
    case SDP_TYPE_INFOFRAME_VENDOR_SPEC:  return "CTA-VSI";
    case SDP_TYPE_INFOFRAME_AUX_VID_INFO: return "CTA-AVI";
    case SDP_TYPE_INFOFRAME_SPD:       return "CTA-SPD";
    case SDP_TYPE_INFOFRAME_AUDIO:     return "CTA-AUDIO_INFO";
    case SDP_TYPE_INFOFRAME_HDR:       return "CTA-HDR";
    default:                           return "Unknwon";
    }
}

/*------------------------------------------------
 * Func : _get_source_check_intenal
 *
 * Desc : compute remain time for source check gard time
 *
 * Para : curr_time :
 *        pre_source_check_time_stamp :
 *        guard_time :
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
extern UINT8 _get_source_check_intenal(
    unsigned long curr_time,
    unsigned long pre_source_check_time_stamp,
    unsigned int  guard_time
    )
{
    #define UNSIGNED_LONG_MAX                       0xFFFFFFFF
    unsigned long time_diff = 0;
    if (guard_time==0)
        return 0;
    if (curr_time >= pre_source_check_time_stamp)
        time_diff = curr_time - pre_source_check_time_stamp;
    else
        time_diff = (UNSIGNED_LONG_MAX - pre_source_check_time_stamp) + curr_time;
    return (time_diff > guard_time) ? 0 : -1;
}

/*------------------------------------------------
 * Func : _get_max_lane_count
 *
 * Desc : get maximum lane count
 *
 * Para : p_adp  : handle of DPRX adapter
 *
 * Retn : lane count = min ( max adp lane count, max cable lane count)
 *-----------------------------------------------*/
extern UINT8 _get_max_lane_count(RTK_DPRX_ADP* p_dprx)
{
#ifdef CONFIG_ENABLE_TYPEC_DPRX
    if (p_dprx->type == DP_TYPE_USB_TYPE_C)
    {
        UINT8 cable_lane_count = 4;

        // for typec, the maximum lane count will related to cable config
        switch(p_dprx->cable_type)
        {
        case CABLE_CFG_TYPEC_CFG_B:
        case CABLE_CFG_TYPEC_CFG_B_FLIP:
        case CABLE_CFG_TYPEC_CFG_D:
        case CABLE_CFG_TYPEC_CFG_D_FLIP:
            cable_lane_count = 2;   // for config B/D, 2 lanes are available
            break;
        default:
            break;
        }

        if (cable_lane_count < p_dprx->sink_cap.max_lane_count)
            return cable_lane_count;
    }
#endif

    return p_dprx->sink_cap.max_lane_count;
}

DP_COLOR_SPACE_E _to_dp_color_space(
    UINT8           color_space
    )
{
    switch (color_space)
    {
    case _COLOR_SPACE_RGB:      return DP_COLOR_SPACE_RGB;
    case _COLOR_SPACE_YCBCR444: return DP_COLOR_SPACE_YUV444;
    case _COLOR_SPACE_YCBCR422: return DP_COLOR_SPACE_YUV422;
    case _COLOR_SPACE_YCBCR420: return DP_COLOR_SPACE_YUV420;
    case _COLOR_SPACE_Y_ONLY:   return DP_COLOR_SPACE_YONLY;
    default:
    case _COLOR_SPACE_RAW:
        DPRX_ADP_WARN("_to_dp_color_space failed, !!!!, unknow type - %d, force RGB\n", color_space);
        return DP_COLOR_SPACE_RGB;
    }
}


DP_COLORIMETRY_E _to_dp_colorimetry(
    UINT8           colorimetry
    )
{
    switch (colorimetry)
    {
    case _COLORIMETRY_RGB_SRGB :       return DP_COLORIMETRY_SRGB;
    case _COLORIMETRY_RGB_ADOBERGB :   return DP_COLORIMETRY_ADOBE_RGB;
    case _COLORIMETRY_RGB_XRRGB :      return DP_COLORIMETRY_XRGB;
    case _COLORIMETRY_RGB_SCRGB :      return DP_COLORIMETRY_SCRGB;
    case _COLORIMETRY_RGB_DCI_P3 :     return DP_COLORIMETRY_DCI_P3;
    case _COLORIMETRY_RGB_COLOR_PROFILE : return DP_COLORIMETRY_CUSTOM_COLOR_PROFILE;
    case _COLORIMETRY_YCC_ITUR_BT601 : return DP_COLORIMETRY_BT_601;
    case _COLORIMETRY_YCC_ITUR_BT709 : return DP_COLORIMETRY_BT_709;
    case _COLORIMETRY_YCC_XVYCC601 :   return DP_COLORIMETRY_XV_YCC_601;
    case _COLORIMETRY_YCC_XVYCC709 :   return DP_COLORIMETRY_XV_YCC_709;
    case _COLORIMETRY_Y_ONLY :         return DP_COLORIMETRY_YONLY;
    case _COLORIMETRY_RAW :            return DP_COLORIMETRY_RAW;
    default :                          return DP_COLORIMETRY_UNKNOWN;
    }
}


DP_COLORIMETRY_E _to_dp_colorimetry_ext(
    UINT8           colorimetry_ext
    )
{
    switch (colorimetry_ext)
    {
    case _COLORIMETRY_EXT_RGB_SRGB :     return  DP_COLORIMETRY_SRGB;
    case _COLORIMETRY_EXT_RGB_ADOBERGB : return  DP_COLORIMETRY_ADOBE_RGB;
    case _COLORIMETRY_EXT_RGB_XRRGB :    return  DP_COLORIMETRY_XRGB;
    case _COLORIMETRY_EXT_RGB_SCRGB :    return  DP_COLORIMETRY_SCRGB;
    case _COLORIMETRY_EXT_RGB_DCI_P3 :   return  DP_COLORIMETRY_DCI_P3;
    case _COLORIMETRY_EXT_RGB_CUSTOM_COLOR_PROFILE : return  DP_COLORIMETRY_CUSTOM_COLOR_PROFILE;
    case _COLORIMETRY_EXT_YCC_ITUR_BT601 : return  DP_COLORIMETRY_BT_601;
    case _COLORIMETRY_EXT_YCC_ITUR_BT709 : return  DP_COLORIMETRY_BT_709;
    case _COLORIMETRY_EXT_YCC_XVYCC601 : return  DP_COLORIMETRY_XV_YCC_601;
    case _COLORIMETRY_EXT_YCC_XVYCC709 : return  DP_COLORIMETRY_XV_YCC_709;
    case _COLORIMETRY_EXT_YCC_ITUR_BT2020_CL :  return  DP_COLORIMETRY_BT_2020_YCCBCCRC;
    case _COLORIMETRY_EXT_YCC_ITUR_BT2020_NCL : return  DP_COLORIMETRY_BT_2020_YCBCR;
    case _COLORIMETRY_EXT_RGB_ITUR_BT2020 :     return  DP_COLORIMETRY_BT_2020_RGB;
    case _COLORIMETRY_EXT_YCC_SYCC601 :         return  DP_COLORIMETRY_S_YCC_601;
    case _COLORIMETRY_EXT_YCC_ADOBEYCC601 :     return  DP_COLORIMETRY_OP_YCC_601;
    case _COLORIMETRY_EXT_Y_ONLY_DICOM_PART14 : return  DP_COLORIMETRY_YONLY;
    case _COLORIMETRY_EXT_RAW_CUSTOM_COLOR_PROFILE : return  DP_COLORIMETRY_RAW;
    default : return  DP_COLORIMETRY_UNKNOWN;
    }
}

DP_CONTENT_TYPE_E _to_dp_content_type(
    UINT8           content_type
    )
{
    switch(content_type)
    {
    case 0 :  return DP_CONTENT_TYPE_NOT_DEFINED;
    case 1 :  return DP_CONTENT_TYPE_GRAPHICS;
    case 2 :  return DP_CONTENT_TYPE_PHOTO;
    case 3 :  return DP_CONTENT_TYPE_VIDEO;
    case 4 :  return DP_CONTENT_TYPE_GAME;
    default : return DP_CONTENT_TYPE_NOT_DEFINED;
    }
}

DP_DYNAMIC_RANGE_E _to_dp_dynamic_range(
    UINT8           dynamic_range
    )
{
    switch(dynamic_range)
    {
    case _DP_COLOR_QUANTIZATION_FULL :  return DP_DYNAMIC_RANGE_VESA;
    case _DP_COLOR_QUANTIZATION_LIMIT : return DP_DYNAMIC_RANGE_CTA;
    default : return DP_DYNAMIC_RANGE_CTA;
    }
}

DP_DRR_MODE_E _to_dp_drr_mode(DPRX_DRR_MODE_E mode)
{
    switch(mode)
    {
    case DPRX_DRR_MODE_FRR:           return DP_DRR_MODE_FRR;
    case DPRX_DRR_MODE_AMD_FREE_SYNC: return DP_DRR_MODE_AMD_FREE_SYNC;
    case DPRX_DRR_MODE_ADAPTIVE_SYNC: return DP_DRR_MODE_ADAPTIVE_SYNC;
    default:                          return DP_DRR_MODE_UNKNWON;
    }
}

const char* _to_dp_drr_mode_str(DPRX_DRR_MODE_E mode)
{
    switch(mode)
    {
    case DPRX_DRR_MODE_FRR:           return "FRR";
    case DPRX_DRR_MODE_AMD_FREE_SYNC: return "Free Sync";
    case DPRX_DRR_MODE_ADAPTIVE_SYNC: return "Adp Sync";
    default:                          return "Unknown";
    }
}

int _check_sdp_buff_expiration(
    DPRX_SDP_PKT_BUFFER* p_sdp,
    UINT32               expire_time
    )
{
    if (p_sdp->pkt_count)
    {
        UINT32 elapsed_time = dprx_odal_get_system_time_ms();

        if (elapsed_time >= p_sdp->time_stamp)
            elapsed_time -= p_sdp->time_stamp;
        else  // time over run
            elapsed_time += (0xFFFFFFFF - p_sdp->time_stamp) ;

        if (elapsed_time > expire_time)
        {
            memset(p_sdp, 0, sizeof(DPRX_SDP_PKT_BUFFER));
            return 1;
        }
    }

    return 0;
}

/*------------------------------------------------
 * Func : _to_dp_pixel_mode
 *
 * Desc : convert DPRX_DRV_PIXEL_MODE_E pixel mode
 *        to DPRX_OUT_PIXEL_MODE
 *
 * Para : pixel_mode  : handle of DPRX adapter
 *
 * Retn : DPRX_OUT_PIXEL_MODE_1P
 *        DPRX_OUT_PIXEL_MODE_2P
 *        ...
 *-----------------------------------------------*/
unsigned char _to_dp_pixel_mode(DPRX_DRV_PIXEL_MODE_E pixel_mode)
{
    switch(pixel_mode) {
    case DPRX_DRV_ONE_PIXEL_MODE: return DPRX_OUT_PIXEL_MODE_1P;
    case DPRX_DRV_TWO_PIXEL_MODE: return DPRX_OUT_PIXEL_MODE_2P;
    case DPRX_DRV_FOUR_PIXEL_MODE: return DPRX_OUT_PIXEL_MODE_4P;
    default:
        DPRX_ADP_WARN("_to_dp_pixel_mode warning, unknow pixel mode %d, force 1p\n", pixel_mode);
        return DPRX_OUT_PIXEL_MODE_1P;
    }
}

#ifndef DPRX_ASYNCHRONOUS_DETECT_TASK
UINT32 g_check_source_detect_period = 0;
void _check_source_detect_period(RTK_DPRX_ADP* p_dprx, UINT32 frame_rate_hz)
{
    UINT32 elapsed_time = dprx_odal_get_system_time_ms();;
    UINT32 frame_preoid;
    UINT8 res;

    // get frame peroid
    if (frame_rate_hz < 20)
        frame_rate_hz = 20;    // Min frame rate
    else if (frame_rate_hz > 300)
        frame_rate_hz = 300;   // Max frame rate

    frame_preoid = ((1000 / frame_rate_hz) +1);  // frame perid in ms

    if (elapsed_time >= g_check_source_detect_period)
        elapsed_time -= g_check_source_detect_period;
    else  // time over run
        elapsed_time += (0xFFFFFFFF - g_check_source_detect_period) ;

    if (elapsed_time > frame_preoid)
    {
        rtk_dprx_source_check(p_dprx);
        g_check_source_detect_period = dprx_odal_get_system_time_ms();
    }
}
#endif

/*------------------------------------------------
 * Func : rtk_dprx_port_reset
 *
 * Desc :   Replace port_swap(p_dprx) & port_swap(NULL)+port_swap(p_dprx);
 *          After port_reset, p_dprx will become current port.
 *          and start low level driver.
 *          hpd will set to low or do toggle.
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_port_reset(
    RTK_DPRX_ADP*   p_dprx
)
{
    int ret=0;

    if(g_rtk_priv_ex_ops.reset)
        ret = g_rtk_priv_ex_ops.reset(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");

    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_port_swap
 *
 * Desc : change dprx port to dprx low level driver
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_port_swap(
    RTK_DPRX_ADP*   p_dprx
)
{
    int ret=0;
    if(g_rtk_priv_ex_ops.port_swap)
    {
        ret = g_rtk_priv_ex_ops.port_swap(p_dprx);
    }
    else
    {
        if(g_rtk_priv_ex_ops.reset)
            ret = g_rtk_priv_ex_ops.reset(p_dprx);
        else
            DPRX_ADP_WARN("Trying to use a Null function pointer.");
    }
    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_init
 *
 * Desc : init DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_init(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.init)
        ret = g_rtk_priv_ex_ops.init(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_open
 *
 * Desc : open DPRX adapter. after open, the DPRX
 *        adapter will become operational. caller
 *        should call open before any other operations
 *        excepts init/uninit
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_open(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.open)
        ret = g_rtk_priv_ex_ops.open(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_close
 *
 * Desc : close DPRX adapter. after close, the DPRX
 *        adapter will stop operation.
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_close(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.close)
        ret = g_rtk_priv_ex_ops.close(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");
    

    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_uninit
 *
 * Desc : unint DPRX adapter. this function should
 *        be called when adapter to be destroyed
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_uninit(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

        if(g_rtk_priv_ex_ops.uninit)
            ret = g_rtk_priv_ex_ops.uninit(p_dprx);
        else
            DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_connect
 *
 * Desc : connect DPRX adapter. after connect, the
 *        DPRX adapter will begin audio/video format
 *        detection and able to output audio/video
 *        after detect.
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_connect(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

        if(g_rtk_priv_ex_ops.connect)
            ret = g_rtk_priv_ex_ops.connect(p_dprx);
        else
            DPRX_ADP_WARN("Trying to use a Null function pointer.");

    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_disconnect
 *
 * Desc : disable DPRX adapter. after detect, the
 *        DPRX adapter detect/output audio/video data
 *
 * Para : p_dprx : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_disconnect(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.disconnect)
        ret = g_rtk_priv_ex_ops.disconnect(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_connect_status
 *
 * Desc : get cable connection status
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : disconnect, 1 : connected
 *-----------------------------------------------*/
int rtk_dprx_get_connect_status(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_connect_status)
        ret = g_rtk_priv_ex_ops.get_connect_status(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");

    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_aux_status
 *
 * Desc : get aux status
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : aux disconnect, 1 : aux connected
 *-----------------------------------------------*/
int rtk_dprx_get_aux_status(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_aux_status)
        ret = g_rtk_priv_ex_ops.get_aux_status(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_set_hpd
 *
 * Desc : set HPD status of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_set_hpd(
    RTK_DPRX_ADP*   p_dprx,
    UINT8           enable
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.set_hpd)
        ret = g_rtk_priv_ex_ops.set_hpd(p_dprx,enable);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_set_hpd_toggle
 *
 * Desc : set HPD toggle signal of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *        delay_ms  : the Interval of HPD toggle
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_set_hpd_toggle(
    RTK_DPRX_ADP*   p_dprx,
    UINT16           delay_ms
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.set_hpd_toggle)
        ret = g_rtk_priv_ex_ops.set_hpd_toggle(p_dprx,delay_ms);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_set_irq_hpd
 *
 * Desc : set IRQ HPD of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_set_irq_hpd(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.set_irq_hpd)
        ret = g_rtk_priv_ex_ops.set_irq_hpd(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_hpd
 *
 * Desc : get HPD status of the DPRX adapter
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : HPD low, 1 : HPD high
 *-----------------------------------------------*/
int rtk_dprx_get_hpd(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_hpd)
        ret = g_rtk_priv_ex_ops.get_hpd(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_typec_pd_ready_status
 *
 * Desc : get typec pd ready status
 *
 * Para : p_dev : handle of DPRX adapter
 *
 * Retn : 0 : typec pd is ready, 1 : typec pd is busy
 *-----------------------------------------------*/
int rtk_dprx_get_typec_pd_ready_status(
    RTK_DPRX_ADP*   p_dprx
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_typec_pd_ready_status)
        ret = g_rtk_priv_ex_ops.get_typec_pd_ready_status(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_set_edid
 *
 * Desc : set EDID of the DPRX adapter
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_edid : edid data
 *        edid_len : size of edid data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_set_edid(
    RTK_DPRX_ADP*   p_dprx,
    unsigned char*  p_edid,
    unsigned short  edid_len
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.set_edid)
        ret = g_rtk_priv_ex_ops.set_edid(p_dprx,p_edid,edid_len);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : _ops_dprx_get_edid
 *
 * Desc : get EDID of the DPRX adapter
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_edid : edid data
 *        edid_len : size of edid data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_edid(
    RTK_DPRX_ADP*   p_dprx,
    unsigned char*  p_edid,
    unsigned short  edid_len
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_edid)
        ret = g_rtk_priv_ex_ops.get_edid(p_dprx,p_edid,edid_len);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_stream_type
 *
 * Desc : get dprx port stream type
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_get_stream_type(
    RTK_DPRX_ADP*       p_dprx,
    DP_STREAM_TYPE_E*   p_type
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_stream_type)
        ret = g_rtk_priv_ex_ops.get_stream_type(p_dprx,p_type);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}


/*------------------------------------------------
 * Func : _ops_dprx_get_capability
 *
 * Desc : get dprx port hardware capability
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_get_capability(
    RTK_DPRX_ADP*       p_dprx,
    DP_PORT_CAPABILITY* p_cap
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_capability)
        ret = g_rtk_priv_ex_ops.get_capability(p_dprx,p_cap);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_link_status
 *
 * Desc : get dprx port link status
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int rtk_dprx_get_link_status(
    RTK_DPRX_ADP*       p_dprx,
    DP_LINK_STATUS_T*   p_status
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_link_status)
        ret = g_rtk_priv_ex_ops.get_link_status(p_dprx,p_status);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}


#ifdef CONFIG_SUPPORT_DOLBY_VISION_DP

int rtk_dprx_set_dolby_vision_check_mode_function(
    long (*get_dolby_vision_dp_mode)(unsigned char))
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.set_dolby_vision_check_mode_function)
        ret = g_rtk_priv_ex_ops.set_dolby_vision_check_mode_function(get_dolby_vision_dp_mode);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}
#endif

/*------------------------------------------------
 * Func : rtk_dprx_get_video_timing
 *
 * Desc : get dprx port video timing
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_timing : pointer of timing info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_video_timing(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_TIMING_INFO_T* p_timing
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_video_timing)
        ret = g_rtk_priv_ex_ops.get_video_timing(p_dprx,p_timing);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_stream_bypasss_timing
 *
 * Desc : get dprx port stream bypass timing
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_timing : pointer of timing info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_stream_bypasss_timing(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_TIMING_INFO_T* p_timing
    )
{
    return rtk_dprx_get_video_timing(p_dprx, p_timing);
}


/*------------------------------------------------
 * Func : rtk_dprx_get_pixel_encoding_info
 *
 * Desc : get dprx port pixel format info
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_pixel : pointer of pixel encoding format
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_pixel_encoding_info(
    RTK_DPRX_ADP*       p_dprx,
    DP_PIXEL_ENCODING_FORMAT_T*    p_pixel
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_pixel_encoding_info)
        ret = g_rtk_priv_ex_ops.get_pixel_encoding_info(p_dprx,p_pixel);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_avi_info
 *
 * Desc : get dprx port avi drm
 *
 * Para : p_adp : handle of DPRX adapter
 *        p_avi : pointer of avi info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_avi_info(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_AVI_INFO_T*    p_avi
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_avi_info)
        ret = g_rtk_priv_ex_ops.get_avi_info(p_dprx,p_avi);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_drm_info
 *
 * Desc : get dprx port drm drm
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_drm : pointer of drm info
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_drm_info(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_DRM_INFO_T*    p_drm
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_drm_info)
        ret = g_rtk_priv_ex_ops.get_drm_info(p_dprx,p_drm);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");



    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_vbid_data
 *
 * Desc : get vbid data from a given DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
*         p_msa  : SDP type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_vbid_data(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_VBID_INFO_T*   p_vbid
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_vbid_data)
        ret = g_rtk_priv_ex_ops.get_vbid_data(p_dprx,p_vbid);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_msa_data
 *
 * Desc : get msa data from a given DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
*         p_msa  : SDP type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_msa_data(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_MSA_INFO_T*    p_msa
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_msa_data)
        ret = g_rtk_priv_ex_ops.get_msa_data(p_dprx,p_msa);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_sdp_data
 *
 * Desc : get sdp data from a given DPRX Port
 *
 * Para : p_adp : handle of DPRX adapter
 *        type   : SDP type
 *        p_data : spd data output
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_sdp_data(
    RTK_DPRX_ADP*       p_dprx,
    DP_SDP_TYPE         type,
    DP_SDP_DATA_T*      p_sdp
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_sdp_data)
        ret = g_rtk_priv_ex_ops.get_sdp_data(p_dprx,type,p_sdp);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_pps_data
 *
 * Desc : get sdp data from a given DPRX Port
 *
 * Para : p_adp : handle of DPRX adapter
 *        p_data : spd data output
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_pps_data(
    RTK_DPRX_ADP*       p_dprx,
    DP_PPS_DATA_T*      p_pps_sdp
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.get_pps_data)
        ret = g_rtk_priv_ex_ops.get_pps_data(p_dprx,p_pps_sdp);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_source_check
 *
 * Desc : Check input source
 *
 * Para : port : dprx port
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_source_check(
    RTK_DPRX_ADP*       p_dprx
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.source_check)
        ret = g_rtk_priv_ex_ops.source_check(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_reset_signal_detect_flow
 *
 * Desc : reset signal detect flow of the given dprx port
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_reset_signal_detect_flow(
    RTK_DPRX_ADP*       p_dprx
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.reset_signal_detect_flow)
        ret = g_rtk_priv_ex_ops.reset_signal_detect_flow(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_handle_online_measure_error
 *
 * Desc : handle on line measure error
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_handle_online_measure_error(
    RTK_DPRX_ADP*       p_dprx
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.handle_online_measure_error)
        ret = g_rtk_priv_ex_ops.handle_online_measure_error(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_audio_status
 *
 * Desc : get dprx port audio status
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_audio_status : pointer of hdcp status
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_audio_status(
    RTK_DPRX_ADP*       p_dprx,
    DP_AUDIO_STATUS_T*  p_audio_status
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_audio_status)
        ret = g_rtk_priv_ex_ops.get_audio_status(p_dprx,p_audio_status);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");

    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_get_hdcp_status
 *
 * Desc : get dprx port hdcp status
 *
 * Para : p_adp  : handle of DPRX adapter
 *        p_hdcp_status : pointer of hdcp status
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_hdcp_status(
    RTK_DPRX_ADP*       p_dprx,
    DPRX_HDCP_STATUS_T* p_hdcp_status
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_hdcp_status)
        ret = g_rtk_priv_ex_ops.get_hdcp_status(p_dprx,p_hdcp_status);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


//===============================================================================
// DPCD related
//===============================================================================

/*------------------------------------------------
 * Func : rtk_dprx_set_dpcd
 *
 * Desc : set DPCD of the DPRX adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *        address : dpcd address
 *        p_dpcd  : dpcd data
 *        len     : size of dpcd data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_set_dpcd(
    RTK_DPRX_ADP*       p_dprx,
    unsigned long       address,
    unsigned char*      p_dpcd,
    unsigned short      len
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.set_dpcd)
        ret = g_rtk_priv_ex_ops.set_dpcd(p_dprx,address,p_dpcd,len);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_get_dpcd
 *
 * Desc : get DPCD of the DPRX adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *        address : dpcd address
 *        p_dpcd  : dpcd data
 *        len     : size of dpcd data
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_dpcd(
    RTK_DPRX_ADP*       p_dprx,
    unsigned long       address,
    unsigned char*      p_dpcd,
    unsigned short      len
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_dpcd)
        ret = g_rtk_priv_ex_ops.get_dpcd(p_dprx,address,p_dpcd,len);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

//===============================================================================
// Extension Control
//===============================================================================

/*------------------------------------------------
 * Func : rtk_dprx_set_ext_ctrl
 *
 * Desc : set ext control of a DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
 *        ctrl : ext control command
 *
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_set_ext_ctrl(
    RTK_DPRX_ADP*   p_dprx,
    ADP_EXCTL_ID    ctrl,
    unsigned int    val
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.set_ext_ctrl)
        ret = g_rtk_priv_ex_ops.set_ext_ctrl(p_dprx,ctrl,val);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

/*------------------------------------------------
 * Func : rtk_dprx_set_ext_ctrl
 *
 * Desc : set ext control of a DPRX Port
 *
 * Para : p_dprx : handle of DPRX adapter
 *        ctrl : ext control command
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_get_ext_ctrl(
    RTK_DPRX_ADP*   p_dprx,
    ADP_EXCTL_ID    ctrl
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.get_ext_ctrl)
        ret = g_rtk_priv_ex_ops.get_ext_ctrl(p_dprx,ctrl);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}

//----------------------------------------------------------------------------------------
// Power Management API
//----------------------------------------------------------------------------------------

/*------------------------------------------------
 * Func : rtk_dprx_suspend
 *
 * Desc : suspend a rtk dprx adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_suspend(
    RTK_DPRX_ADP*       p_dprx,
    UINT32              mode
    )
{
    int ret=0;

    if(g_rtk_priv_ex_ops.suspend)
        ret = g_rtk_priv_ex_ops.suspend(p_dprx, mode);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");

    return ret;
}


/*------------------------------------------------
 * Func : rtk_dprx_resume
 *
 * Desc : resume a rtk dprx adapter
 *
 * Para : p_dprx  : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
int rtk_dprx_resume(
    RTK_DPRX_ADP*       p_dprx
    )
{
    int ret=0;
     

    if(g_rtk_priv_ex_ops.resume)
        ret = g_rtk_priv_ex_ops.resume(p_dprx);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");


    return ret;
}


//----------------------------------------------------------------------------------------
// Event Handler API
//----------------------------------------------------------------------------------------

/*------------------------------------------------
 * Func : rtk_dprx_handle_infoframe_sdp_update_event
 *
 * Desc : get sdp data from a given DPRX Port
 *
 * Para : p_adp : handle of DPRX adapter
 *        sdp_type : sdp type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
void rtk_dprx_handle_infoframe_sdp_update_event(
    RTK_DPRX_ADP*       p_dprx,
    SDP_TYPE_E          sdp_type
    )
{
     

    if(g_rtk_priv_ex_ops.handle_infoframe_sdp_update_event)
        g_rtk_priv_ex_ops.handle_infoframe_sdp_update_event(p_dprx, sdp_type);
    else
        DPRX_ADP_WARN("Trying to use a Null function pointer.");

}

#ifdef ENABLE_DPRX_LT_EVENT_MONITOR

/*------------------------------------------------
 * Func : rtk_dprx_handle_lt_state_update_event
 *
 * Desc : handle lt state update event
 *
 * Para : p_adp : handle of DPRX adapter
 *        state : lt state type
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
void rtk_dprx_handle_lt_state_update_event(
    RTK_DPRX_ADP*       p_dprx,
    UINT8               state
    )
{
     

    if(g_rtk_priv_ex_ops.handle_lt_state_update_event)
        g_rtk_priv_ex_ops.handle_lt_state_update_event(p_dprx, state);
    else
        DPRX_ADP_WARN("Trying ti use a Null function pointer.");

}

#endif

//----------------------------------------------------------------------------------------
// DPRX TASK HANDLER
//----------------------------------------------------------------------------------------
#ifdef CONFIG_DPRX_ENABLE_TASK_EVENT_HANDLER
/*------------------------------------------------
 * Func : rtk_dprx_event_task_handler
 *
 * Desc : handle dprx event task
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
void rtk_dprx_port_event_task_handler(RTK_DPRX_ADP* p_dprx)
{
     

    if(g_rtk_priv_ex_ops.port_event_task_handler)
        g_rtk_priv_ex_ops.port_event_task_handler(p_dprx);
    else
        DPRX_ADP_WARN("Trying ti use a Null function pointer.");

}

/*------------------------------------------------
 * Func : rtk_dprx_event_task_handler
 *
 * Desc : handle dprx event task
 *
 * Para : p_adp : handle of DPRX adapter
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
void rtk_dprx_event_task_handler(void)
{
     

    if(g_rtk_priv_ex_ops.event_task_handler)
        g_rtk_priv_ex_ops.event_task_handler();
    else
        rtk_dprx_port_event_task_handler(NULL);

}
#endif

/*------------------------------------------------
 * Func : rtk_dprx_set_aux_int_crtl_rounting_enable
 *
 * Desc : AUX INT CTRL
 *
 * Para : enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_dprx_port_set_aux_int_crtl_rounting_enable(
    RTK_DPRX_ADP*       p_dprx,
    UINT8               enable
    )
{
     

    if(g_rtk_priv_ex_ops.port_set_aux_int_crtl_rounting_enable)
        g_rtk_priv_ex_ops.port_set_aux_int_crtl_rounting_enable(p_dprx,enable);
    else
        DPRX_ADP_WARN("Trying ti use a Null function pointer.");

}
/*------------------------------------------------
 * Func : rtk_dprx_set_aux_int_crtl_rounting_enable
 *
 * Desc : AUX INT CTRL
 *
 * Para : enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_dprx_set_aux_int_crtl_rounting_enable(
    UINT8               enable
    )
{

    if(g_rtk_priv_ex_ops.set_aux_int_crtl_rounting_enable)
        g_rtk_priv_ex_ops.set_aux_int_crtl_rounting_enable(enable);
    else
        rtk_dprx_port_set_aux_int_crtl_rounting_enable(NULL,enable);

}