/*=============================================================
 * File:    rtk_video_common.c
 *
 * Desc:    RTK hdmi dp common
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
#include <mach/rtk_platform.h>
#include <rtk_kdriver/video_common/rtk_video_common.h>
#include "rtk_video_common-priv.h"
/**********************************************************************************************
*
*   Variables
*
**********************************************************************************************/

HDMI_DPRX_PORT_INSTANCE g_vc_port_instance[] = port_instance;

/**********************************************************************************************
*
*   FAKE Dynamic VC MODULE
*
**********************************************************************************************/

#ifdef CONFIG_RTK_DYNAMIC_ENABLE_VIDEO_COMMON
static unsigned char vc_enable_dynamic = 0;
static unsigned char _get_module_vc_enable(void)
{
    return vc_enable_dynamic;
}
static void _set_module_vc_enable(void)
{
    RTK_VIDEO_COMMON_INFO("Try to set VC enable dynamiclly before CRT on.");
    switch(get_mach_type())
     {
        case MACH_ARCH_RTK2885P:
            RTK_VIDEO_COMMON_INFO("Now SoC is Merlin8, dynamic disable vc");
            vc_enable_dynamic = 0;
            break;
        case MACH_ARCH_RTK2885P2:
        case MACH_ARCH_RTK2885PP:
            RTK_VIDEO_COMMON_INFO("Now SoC is Merlin8P, dynamic enable vc");
            vc_enable_dynamic = 1;
            break;
        default:
            vc_enable_dynamic = 1;
            RTK_VIDEO_COMMON_WARNING("shouldn't set module enable in this SOC, please check config");
            break;
    }
}
#else
#define _get_module_vc_enable()      1
#define _set_module_vc_enable()
#endif

/**********************************************************************************************
*
*   FUNCTION Body
*
**********************************************************************************************/
const char* _get_video_common_port_str(VIDEO_COMMON_PORT vc_port)
{
    switch(vc_port)
    {
    case VIDEO_COMMON_0:         return "VIDEO_COMMON_0";
    case VIDEO_COMMON_1:         return "VIDEO_COMMON_1";
    case VIDEO_COMMON_2:         return "VIDEO_COMMON_2";
    case VIDEO_COMMON_3:         return "VIDEO_COMMON_3";
    default:                     return "VIDEO_COMMON_NONE";
    }
}


const char* _get_video_common_port_source_str(VIDEO_COMMON_PORT_IN_SEL src)
{
    switch(src)
    {
    case VIDEO_COMMON_HDMIRX:    return "VIDEO_COMMON_HDMIRX";
    case VIDEO_COMMON_DPRX:      return "VIDEO_COMMON_DPRX";
    default:                     return "VIDEO_COMMON_HDMIRX";
    }
}

const char* _get_video_common_source_type_str(VIDEO_COMMON_SOURCE_TYPE type)
{
    switch(type)
    {
    case HDMI_2p0:              return "HDMI_2p0";
    case HDMI_2p1:              return "HDMI_2p1";
    case DP_1p4:                return "DP_1p4";
    case DP_2p0:                return "DP_2p0";
    case HDMI_2p0_DSCD:         return "HDMI_2p0_DSCD";
    case HDMI_2p1_DSCD:         return "HDMI_2p1_DSCD";
    case DP_1p4_DSCD:           return "DP_1p4_DSCD";
    case DP_2p0_DSCD:           return "DP_2p0_DSCD";
    default:                    return "NO Source Type";
    }
}

const char* _get_video_common_color_space(VIDEO_COMMON_COLOR_DEPTH_INPUT color_space)
{
    switch(color_space)
    {
    case COLOR_SPACE_RGB:         return "COLOR_SPACE_RGB";
    case COLOR_SPACE_YUV422:      return "COLOR_SPACE_YUV422";
    case COLOR_SPACE_YUV444:      return "COLOR_SPACE_YUV444";
    case COLOR_SPACE_YUV420:      return "COLOR_SPACE_YUV420";
    case COLOR_SPACE_YONLY:       return "COLOR_SPACE_YONLY";
    default:                      return "COLOR_SPACE_UNKNOWN";
    }
}

/*------------------------------------------------
 * Func : _hdmi_dp_common_get_video_common_with_ch_port
 *
 * Desc : get video common port with channel port
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *
 * Retn : video common port
 *-----------------------------------------------*/
unsigned char _hdmi_dp_common_get_video_common_with_ch_port(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch)
{
    unsigned char nport;

    for(nport = 0 ; nport < HDMI_DP_CHANNLE_PORT_MAX; nport++)
    {
        if((g_vc_port_instance[nport].src_in == src) && (g_vc_port_instance[nport].ch == ch))
        {
            // RTK_VIDEO_COMMON_INFO("%s, Found video common port (src = %s, ch = %d, video_common = %s)\n", __func__, _get_video_common_port_source_str(src), ch, _get_video_common_port_str(nport));
            break;
        }
    }

    return nport;
}


//--------------------------------------------------
// Func  : rtk_video_common_crt_on
//
// Desc  : enable video common crt clk
//
// Para  : N/A
//
// Retn  : N/A
//--------------------------------------------------
void rtk_video_common_crt_on(void)
{
    unsigned char nport;
    _set_module_vc_enable();
    if(!_get_module_vc_enable())
        return;
    RTK_VIDEO_COMMON_INFO("%s\n",__func__);

    lib_video_common_crt_all_enable();

    for(nport = 0 ; nport < HDMI_DP_CHANNLE_PORT_MAX; nport++)
    {
        lib_video_common_crt_enable(nport);
    }
}
EXPORT_SYMBOL(rtk_video_common_crt_on);


//--------------------------------------------------
// Func  : rtk_video_check_common_ownership
//
// Desc  : check hdmi dp common resource owner ship
//
// * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
// *           ch : port channel
//
// Retn  : 0 : don't take ownership, 1: take ownership
//--------------------------------------------------
unsigned char rtk_video_check_common_ownership(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch)
{
    unsigned char res = 0;
    unsigned char nport = 0xf;
    if(!_get_module_vc_enable())
        return 0;

    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        // RTK_VIDEO_COMMON_ERR("%s, No allocate on common port(src = %s, ch = %d)\n", __func__, _get_video_common_port_source_str(src), ch);
        return 0;
    }

    res = lib_video_common_get_hdmi_dp_sel(nport);

    return res;
}
EXPORT_SYMBOL(rtk_video_check_common_ownership);


/*------------------------------------------------
 * Func : rtk_video_common_set_ownership
 *
 * Desc : set hdmi dp ownership
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           vc_port : video common
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_ownership(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_PORT vc_port)
{

// use default video common setting
#ifdef OWERNSHIP_SELECT_BY_AP
    unsigned char nport = 0xf;
    if(!_get_module_vc_enable())
        return;

    if(vc_port < VIDEO_COMMON_NONE)
    {
        RTK_VIDEO_COMMON_WARNING("%s, Allocate Video Common[%d] (src = %s, ch = %d)\n",__func__, vc_port, _get_video_common_port_source_str(src), ch);
        nport = vc_port;
        g_vc_port_instance[nport].src_in = src;
        g_vc_port_instance[nport].ch = ch;
        lib_video_common_set_hdmi_dp_sel(nport, g_vc_port_instance[nport].src_in);
    }
    else
    {
        RTK_VIDEO_COMMON_WARNING("%s, Video Common[%d] : Disconnect\n",__func__, vc_port);
        nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);

        // No find Video Common connect.
        if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
            return;

        g_vc_port_instance[nport].src_in = 0xff;
        g_vc_port_instance[nport].ch = 0xff;
    }
#endif
}
EXPORT_SYMBOL(rtk_video_common_set_ownership);


/*------------------------------------------------
 * Func : rtk_video_common_set_input_source
 *
 * Desc : select video common data from which source
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_input_source(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_SOURCE_TYPE src_type)
{
    unsigned char nport = 0xf;

    if(!_get_module_vc_enable())
        return;
    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        RTK_VIDEO_COMMON_ERR("%s, No allocate on common port(src = %s, ch = %d)\n",__func__, _get_video_common_port_source_str(src), ch);
        return;
    }

    RTK_VIDEO_COMMON_INFO("%s, VIDEO_COMMON[%d] Input Source Type = %s\n",__func__, nport, _get_video_common_source_type_str(src_type));
    // support yuv420 clock gate
    lib_video_common_support_clk_gate(nport, 1);

    lib_video_common_input_source_sel(nport, src_type);
    lib_video_common_audio_in_sel(nport, src_type);
    lib_video_common_offms_out_sel(nport, src_type);

    switch(src_type)
    {
        case HDMI_2p0:
        case HDMI_2p1:
        case DP_1p4:
        case DP_2p0:
            break;
        case HDMI_2p0_DSCD:
        case HDMI_2p1_DSCD:
        case DP_1p4_DSCD:
        case DP_2p0_DSCD:
            lib_video_common_dscd_in_sel(nport, g_vc_port_instance[nport].dscd_in);
            break;
        default:
            break;
    }
    g_vc_port_instance[nport].src_type = src_type;
}
EXPORT_SYMBOL(rtk_video_common_set_input_source);


/*------------------------------------------------
 * Func : rtk_video_common_set_color_space
 *
 * Desc : set color space input for video common
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           color_space : color space
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_color_space(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_COLOR_DEPTH_INPUT color_space)
{
    unsigned char nport = 0xf;

    if(!_get_module_vc_enable())
        return;
    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        RTK_VIDEO_COMMON_ERR("%s, No allocate on common port(src = %s, ch = %d)\n", __func__, _get_video_common_port_source_str(src), ch);
        return;
    }

    RTK_VIDEO_COMMON_INFO("%s, src = %s, ch = %d, color_space = %d(%s)\n", __func__, _get_video_common_port_source_str(src), ch, color_space, _get_video_common_color_space(color_space));

    //Disable yuv422 repeat mode
    lib_video_common_set_yuv422_repeat_mode(nport, 0);
    lib_video_common_set_color_space_input(nport, color_space);
}
EXPORT_SYMBOL(rtk_video_common_set_color_space);


/*------------------------------------------------
 * Func : rtk_video_common_set_dprx_pixel_mode
 *
 * Desc : set dp pixel mode in sel
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           pixel_mode : pixel mode
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_dprx_pixel_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_IN_PIXEL_MODE pixel_mode)
{
    unsigned char nport = 0xf;
    if(!_get_module_vc_enable())
        return;
    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        RTK_VIDEO_COMMON_ERR("%s, No allocate on common port(src = %s, ch = %d)\n", __func__, _get_video_common_port_source_str(src), ch);
        return;
    }

    RTK_VIDEO_COMMON_ERR("%s, rtk_video_common_set_dprx_pixel_mode(ch = %d)\n", __func__, pixel_mode);

    lib_video_common_set_dprx_pixel_mode_in_sel(nport, pixel_mode);

    switch(g_vc_port_instance[nport].src_type)
    {
        case HDMI_2p0_DSCD:
        case HDMI_2p1_DSCD:
        case DP_1p4_DSCD:
        case DP_2p0_DSCD:
            lib_video_common_set_dscd_dp_source_clk(nport, g_vc_port_instance[nport].dscd_in, pixel_mode);
            break;
        default:
            break;
    }
}
EXPORT_SYMBOL(rtk_video_common_set_dprx_pixel_mode);


/*------------------------------------------------
 * Func : rtk_video_common_set_output_pixel_mode
 *
// Desc  : set YUV420 pixel mode. on RTD2885P, YUV420
//         is always running at 4 pixel mode. but DISP
//         path only supports 1p / 2p mode.
//
// Param : port : dprx port
//         pixel_mode : 1p / 2p
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_output_pixel_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_IN_PIXEL_MODE pixel_mode)
{
    unsigned char nport = 0xf;
    if(!_get_module_vc_enable())
        return;
    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        RTK_VIDEO_COMMON_ERR("%s, No allocate on common port(src = %s, ch = %d)\n", __func__, _get_video_common_port_source_str(src), ch);
        return;
    }

    lib_video_common_set_output_pixel_mode(nport, pixel_mode);
}
EXPORT_SYMBOL(rtk_video_common_set_output_pixel_mode);


/*------------------------------------------------
 * Func : rtk_video_common_bypass_mode
 *
 * Desc : select offms output sel
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           COMMON_PORT_BYPASS_TO_HDMITX_E : bypass port
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_bypass_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, COMMON_PORT_BYPASS_TO_HDMITX_E bypass_port)
{
    if(!_get_module_vc_enable())
        return;
    RTK_VIDEO_COMMON_INFO("%s\n", __func__);
    lib_video_common_bypass_mode_sel(bypass_port);
}
EXPORT_SYMBOL(rtk_video_common_bypass_mode);


/*------------------------------------------------
 * Func : rtk_video_common_offms_sel
 *
 * Desc : offms position sel
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           HDMI_DP_VIDEO_COMMON : common port
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_offms_sel(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, VIDEO_COMMON_PORT vc_port)
{
    if(!_get_module_vc_enable())
        return;
    RTK_VIDEO_COMMON_INFO("%s\n", __func__);
    lib_video_common_offms_sel(vc_port);
}
EXPORT_SYMBOL(rtk_video_common_offms_sel);


/*------------------------------------------------
 * Func : rtk_video_common_yuv422_raw_data_mode
 *
 * Desc : yuv422 raw data sel
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           COMMON_BYPASS_YUV422_RAW_SEL_E : yuv422 raw
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_yuv422_raw_data_mode(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, COMMON_BYPASS_YUV422_RAW_SEL_E yuv422_raw)
{
    if(!_get_module_vc_enable())
        return;
    RTK_VIDEO_COMMON_INFO("%s\n", __func__);
    lib_video_common_yuv422_raw_data_sel(yuv422_raw);
}
EXPORT_SYMBOL(rtk_video_common_yuv422_raw_data_mode);


/*------------------------------------------------
 * Func : rtk_video_common_get_bypass_port
 *
 * Desc : get bypass port to HDMITX
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *
 * Retn : N/A
 *-----------------------------------------------*/
COMMON_PORT_BYPASS_TO_HDMITX_E rtk_video_common_get_bypass_port(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch)
{
    if(!_get_module_vc_enable())
        return -1;
    return lib_video_common_get_bypass_port();
}
EXPORT_SYMBOL(rtk_video_common_get_bypass_port);

/*------------------------------------------------
 * Func : rtk_video_common_set_output_clk_div
 *
 * Desc : set output clk div
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_output_clk_div(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable)
{
    unsigned char nport = 0xf;
    if(!_get_module_vc_enable())
        return;
    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        RTK_VIDEO_COMMON_ERR("%s, No allocate on common port(src = %s, ch = %d)\n", __func__, _get_video_common_port_source_str(src), ch);
        return;
    }

    lib_video_common_set_video_common_clk_sel(nport, enable);
}
EXPORT_SYMBOL(rtk_video_common_set_output_clk_div);


/*------------------------------------------------
 * Func : rtk_video_common_set_hs_inverse
 *
 * Desc : set input src horizon inverse
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_hs_inverse(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable)
{
    unsigned char nport = 0xf;

    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    lib_video_common_set_hs_inverse(nport,enable);
}
EXPORT_SYMBOL(rtk_video_common_set_hs_inverse);
/*------------------------------------------------
 * Func : rtk_video_common_set_vs_inverse
 *
 * Desc : set input src vertical inverse
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_vs_inverse(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable)
{
    unsigned char nport = 0xf;

    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    lib_video_common_set_hs_inverse(nport,enable);
}
EXPORT_SYMBOL(rtk_video_common_set_vs_inverse);
/*------------------------------------------------
 * Func : rtk_video_common_set_line_drop
 *
 * Desc : set line drop enable
 *
 * Para :  : VIDEO_COMMON_PORT_IN_SEL : dp or hdmi
 *           ch : port channel
 *           enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void rtk_video_common_set_line_drop(VIDEO_COMMON_PORT_IN_SEL src, unsigned char ch, unsigned char enable)
{
    unsigned char nport = 0xf;
    if(!_get_module_vc_enable())
        return;
    nport = _hdmi_dp_common_get_video_common_with_ch_port(src, ch);
    if(nport >= HDMI_DP_CHANNLE_PORT_MAX)
    {
        RTK_VIDEO_COMMON_ERR("%s, No allocate on common port(src = %s, ch = %d)\n", __func__, _get_video_common_port_source_str(src), ch);
        return;
    }

    lib_video_common_set_line_drop(nport,enable);
}
EXPORT_SYMBOL(rtk_video_common_set_line_drop);

MODULE_LICENSE("GPL");
