/*=============================================================
 * File:    dprx_video_common-rtktv.c
 *
 * Desc:    DPRX video_common for RTK TV
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
#include <dprx_video_common-rtktv.h>
#include <rtk_kdriver/video_common/rtk_video_common.h>

typedef struct
{
    vc_dev_t      	vc_dev;
    unsigned char   idx;
}rtk_vc_dev_t;

//---------------------------------------------------------------
// OPS
//---------------------------------------------------------------

/*------------------------------------------------
 * Func : _ops_init
 *
 * Desc : init DPRX EDID device
 *
 * Para : p_ctx : handle of DPRX EDID HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_init(dev_ctx_t* p_ctx)
{
#if 0
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC init failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;
#endif

    rtk_video_common_crt_on();  // enable video common
    return 0;
}

/*------------------------------------------------
 * Func : _ops_uninit
 *
 * Desc : unint DPRX EDID device
 *
 * Para : p_ctx : handle of DPRX EDID HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_uninit(dev_ctx_t* p_ctx)
{
#if 0
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC uninit failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;
#endif

    return 0;
}

/*------------------------------------------------
 * Func : _ops_check_ownership
 *
 * Desc : check ownership
 *
 * Para : p_ctx : handle of video_common
 *
// Retn  : 0 : don't take ownership, 1: take ownership, others : failed
 *-----------------------------------------------*/
static int _ops_check_ownership(dev_ctx_t* p_ctx)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC check_ownership failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    if(rtk_video_check_common_ownership(VIDEO_COMMON_DPRX, p_dev->idx))
        return 1;
#endif
    return 0;
}


/*------------------------------------------------
 * Func : _ops_set_yuv420_to_444
 *
 * Desc : set_yuv420_to_444
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_yuv420_to_444(dev_ctx_t* p_ctx, unsigned char enable)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_yuv420_to_444 failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    rtk_video_common_set_color_space(VIDEO_COMMON_DPRX, p_dev->idx, (enable) ? COLOR_SPACE_YUV420:COLOR_SPACE_UNKNOWN);
#endif
    return 0;
}


/*------------------------------------------------
 * Func : _ops_set_output_pixel_mode
 *
 * Desc : set_output_pixel_mode
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_output_pixel_mode(dev_ctx_t* p_ctx, DPRX_VC_PIXEL_MODE_E pixel_mode)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_output_pixel_mode failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;


    switch(pixel_mode)
    {
        case DPRX_VC_PIXEL_MODE_ONE_PIXEL:
            rtk_video_common_set_output_pixel_mode(VIDEO_COMMON_DPRX, p_dev->idx, VIDEO_COMMON_1P_In);
            break;
        case DPRX_VC_PIXEL_MODE_TWO_PIXEL:
            rtk_video_common_set_output_pixel_mode(VIDEO_COMMON_DPRX, p_dev->idx, VIDEO_COMMON_2P_In);
            break;
        default:
            break;
    }
#endif
    return 0;
}


/*------------------------------------------------
 * Func : _ops_set_input_pixel_mode
 *
 * Desc  : set YUV420 pixel mode. on RTD2885P, YUV420
 *         is always running at 4 pixel mode. So it
 *         needs input pixel mode info to convert 1p/2p
 *         pixel to 4p mode.
 *
 *         for non compressed stream, it should be equal
 *         to DPRX pixel mode.
 *         for compressed stream, this value will be ignored
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_input_pixel_mode(dev_ctx_t* p_ctx, DPRX_VC_PIXEL_MODE_E pixel_mode)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_input_pixel_mode failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;


    switch(pixel_mode)
    {
        case DPRX_VC_PIXEL_MODE_ONE_PIXEL:
            rtk_video_common_set_dprx_pixel_mode(VIDEO_COMMON_DPRX, p_dev->idx, VIDEO_COMMON_1P_In);
            break;
        case DPRX_VC_PIXEL_MODE_TWO_PIXEL:
            rtk_video_common_set_dprx_pixel_mode(VIDEO_COMMON_DPRX, p_dev->idx, VIDEO_COMMON_2P_In);
            break;
        default:
            break;
    }
#endif
    return 0;
}


/*------------------------------------------------
 * Func : _ops_set_dsc_input_sel
 *
 * Desc  : set YUV420 input from DSC enable
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_dsc_input_sel(dev_ctx_t* p_ctx, unsigned char enable)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;
    rtk_video_common_set_input_source(VIDEO_COMMON_DPRX, p_dev->idx, (enable) ? DP_1p4_DSCD:DP_1p4);

#endif
    return 0;
}


/*------------------------------------------------
 * Func : _ops_set_color_space
 *
 * Desc  : set color space
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_color_space(dev_ctx_t* p_ctx, unsigned char color_space)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;
    VIDEO_COMMON_COLOR_DEPTH_INPUT param = COLOR_SPACE_UNKNOWN;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    switch(color_space)
    {
        case DPRX_VC_COLOR_SPACE_RGB:
            param = COLOR_SPACE_RGB;
            break;

        case DPRX_VC_COLOR_SPACE_YUV422:
            param = COLOR_SPACE_YUV422;
            break;

        case DPRX_VC_COLOR_SPACE_YUV444:
            param = COLOR_SPACE_YUV444;
            break;

        case DPRX_VC_COLOR_SPACE_YUV420:
            param = COLOR_SPACE_YUV420;
            break;

        case DPRX_VC_COLOR_SPACE_YONLY:
            param = COLOR_SPACE_YONLY;
            break;

        default:
            param = COLOR_SPACE_UNKNOWN;
            break;
    }

    rtk_video_common_set_color_space(VIDEO_COMMON_DPRX, p_dev->idx, param);
#endif
    return 0;
}


/*------------------------------------------------
 * Func : _ops_set_src_type
 *
 * Desc  : set source type
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_src_type(dev_ctx_t* p_ctx, unsigned char type)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;
    VIDEO_COMMON_SOURCE_TYPE param = DP_1p4;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    switch(type)
    {
        case DPRX_VC_TYPE_DP1p4:
            param = DP_1p4;
            break;

        case DPRX_VC_TYPE_DP1p4_DSC:
            param = DP_1p4_DSCD;
            break;

        case DPRX_VC_TYPE_DP2p0:
            param = DP_2p0;
            break;

        case DPRX_VC_TYPE_DP2p0_DSC:
            param = DP_2p0_DSCD;
            break;

        default:
            param = DP_1p4;
            break;
    }

    rtk_video_common_set_input_source(VIDEO_COMMON_DPRX, p_dev->idx, param);
#endif
    return 0;
}

/*------------------------------------------------
 * Func : _ops_set_clk_div
 *
 * Desc  : set clk div
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_clk_div(dev_ctx_t* p_ctx, unsigned char enable)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    rtk_video_common_set_output_clk_div(VIDEO_COMMON_DPRX, p_dev->idx, enable);
#endif
    return 0;
}

/*------------------------------------------------
 * Func : _ops_set_bypass_port
 *
 * Desc  : bypass DPRX video to HDMI TX
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_bypass_port(dev_ctx_t* p_ctx, unsigned char enable)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;
    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    if (enable==0)  // do nothing....
        return 0;

    if (p_dev->idx==0)
    {
        rtk_video_common_bypass_mode(VIDEO_COMMON_DPRX, p_dev->idx, PORT0_COMMON_BYPASS_TO_HDMITX);
        DPRX_VC_ERR("_ops_set_bypassport DP[%d] via HDMI DP Common %d\n", p_dev->idx, PORT0_COMMON_BYPASS_TO_HDMITX);
    }
    else
    {
        rtk_video_common_bypass_mode(VIDEO_COMMON_DPRX, p_dev->idx, PORT1_COMMON_BYPASS_TO_HDMITX);
        DPRX_VC_ERR("_ops_set_bypassport DP[%d] via HDMI DP Common %d\n", p_dev->idx, PORT1_COMMON_BYPASS_TO_HDMITX);
    }
#endif
    return 0;
}

/*------------------------------------------------
 * Func : _ops_set_vs_inverse
 *
 * Desc  : set vertical sync inverse
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_vs_inverse(dev_ctx_t* p_ctx, unsigned char enable)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    rtk_video_common_set_vs_inverse(VIDEO_COMMON_DPRX, p_dev->idx, enable);
#endif
    return 0;
}
/*------------------------------------------------
 * Func : _ops_set_hs_inverse
 *
 * Desc  : set horizon sync inverse
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_hs_inverse(dev_ctx_t* p_ctx, unsigned char enable)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    rtk_video_common_set_hs_inverse(VIDEO_COMMON_DPRX, p_dev->idx, enable);
#endif
    return 0;
}

/*------------------------------------------------
 * Func : _ops_set_line_drop
 *
 * Desc  : set line drop setting
 *
 * Para : p_ctx : handle of video_common
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
static int _ops_set_line_drop(dev_ctx_t* p_ctx, unsigned char enable)
{
#if defined(CONFIG_RTK_VIDEO_COMMON) || defined(CONFIG_RTK_VIDEO_COMMON_MODULE)
    rtk_vc_dev_t* p_dev;

    if (p_ctx==NULL)
    {
        DPRX_VC_ERR("RTK VC set_dsc_input_sel failed. invalid argument\n");
        return -1;
    }

    p_dev = (rtk_vc_dev_t*) p_ctx->p_private;

    rtk_video_common_set_line_drop(VIDEO_COMMON_DPRX, p_dev->idx, enable);
#endif
    return 0;
}

/*------------------------------------------------
 * Func : create_rtk_tv_dprx_video_common_device
 *
 * Desc : create rtk video common device
 *
 * Para : ch : channel
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
vc_dev_t* create_rtk_tv_dprx_video_common_device(unsigned char ch)
{
    return create_rtk_tv_dprx_video_common_device_ex(ch, 0);
}


/*------------------------------------------------
 * Func : create_rtk_tv_dprx_video_common_device_ex
 *
 * Desc : create rtk video common device_ex
 *
 * Para : ch : channel
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
vc_dev_t* create_rtk_tv_dprx_video_common_device_ex(unsigned char ch, unsigned long flags)
{
    rtk_vc_dev_t* p_dev = dprx_osal_malloc(sizeof(rtk_vc_dev_t));
    
    if (p_dev)
    {
        memset(p_dev, 0, sizeof(rtk_vc_dev_t));
        p_dev->idx = ch;
        p_dev->vc_dev.ctx.name = "RTK_VIDEO_COMMON";
        p_dev->vc_dev.ctx.p_private = (void*) p_dev;
        p_dev->vc_dev.ops.init   = _ops_init;
        p_dev->vc_dev.ops.uninit = _ops_uninit;
        p_dev->vc_dev.ops.check_ownership = _ops_check_ownership;
        p_dev->vc_dev.ops.set_yuv420_to_444 = _ops_set_yuv420_to_444;
        p_dev->vc_dev.ops.set_output_pixel_mode = _ops_set_output_pixel_mode;
        p_dev->vc_dev.ops.set_input_pixel_mode = _ops_set_input_pixel_mode;
        p_dev->vc_dev.ops.set_dsc_input_sel = _ops_set_dsc_input_sel;
        p_dev->vc_dev.ops.set_color_space = _ops_set_color_space;
        p_dev->vc_dev.ops.set_src_type = _ops_set_src_type;
        p_dev->vc_dev.ops.set_clk_div = _ops_set_clk_div;
        p_dev->vc_dev.ops.set_bypass_port = _ops_set_bypass_port;
        p_dev->vc_dev.ops.set_hs_inverse = _ops_set_hs_inverse;
        p_dev->vc_dev.ops.set_vs_inverse = _ops_set_vs_inverse;
        p_dev->vc_dev.ops.set_line_drop = _ops_set_line_drop;
    }
    return &p_dev->vc_dev;
}

