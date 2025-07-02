/*=============================================================
 * File:    dprx_video_common.c
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
#include <dprx_video_common.h>


//----------------------------------------------------------------------------------------
// DPRX Operations
//----------------------------------------------------------------------------------------
/*------------------------------------------------
 * Func : dprx_hpd_init
 *
 * Desc : init DPRX HPD device
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_init(vc_dev_t* p_dev)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.init(&p_dev->ctx);
    return 0;
}

/*------------------------------------------------
 * Func : dprx_video_common_uninit
 *
 * Desc : unint DPRX adapter. this function should
 *        be called when adapter to be destroyed
 *
 * Para : p_adp : handle of DPRX HPD HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_uninit(vc_dev_t* p_dev)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.uninit(&p_dev->ctx);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_check_ownership
 *
 * Desc : check ownership
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : don't take ownership, 1: take ownership, others : failed
 *-----------------------------------------------*/
int dprx_video_common_check_ownership(vc_dev_t* p_dev)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.check_ownership(&p_dev->ctx);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_set_yuv420_to_444
 *
 * Desc : set_yuv420_to_444
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_yuv420_to_444(vc_dev_t* p_dev, unsigned char enable)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_yuv420_to_444(&p_dev->ctx, enable);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_set_output_pixel_mode
 *
 * Desc  : set YUV420 pixel mode. on RTD2885P, YUV420
 *         is always running at 4 pixel mode. but DISP
 *         path only supports 1p / 2p mode.
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_output_pixel_mode(vc_dev_t* p_dev, DPRX_VC_PIXEL_MODE_E pixel_mode)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_output_pixel_mode(&p_dev->ctx, pixel_mode);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_set_input_pixel_mode
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
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_input_pixel_mode(vc_dev_t* p_dev, DPRX_VC_PIXEL_MODE_E pixel_mode)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_input_pixel_mode(&p_dev->ctx, pixel_mode);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_set_dsc_input_sel
 *
 * Desc  : set YUV420 input from DSC enable
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_dsc_input_sel(vc_dev_t* p_dev, unsigned char enable)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_dsc_input_sel(&p_dev->ctx, enable);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_set_color_space
 *
 * Desc  : set video common color space
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_color_space(vc_dev_t* p_dev, unsigned char color_space)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_color_space(&p_dev->ctx, color_space);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_set_src_type
 *
 * Desc  : set video common source tpye
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_src_type(vc_dev_t* p_dev, unsigned char type)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_src_type(&p_dev->ctx, type);
    return 0;
}

/*------------------------------------------------
 * Func : dprx_video_common_set_clk_div_en
 *
 * Desc  : set video common clk div enable
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_clk_div_en(vc_dev_t* p_dev, unsigned char enable)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_clk_div(&p_dev->ctx, enable);
    return 0;
}

/*------------------------------------------------
 * Func : dprx_video_common_set_bypass_port
 *
 * Desc  : set video common bypass port
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_bypass_port(vc_dev_t* p_dev, unsigned char enable)
{
    if (p_dev && p_dev->ops.init)
        return p_dev->ops.set_bypass_port(&p_dev->ctx, enable);
    return 0;
}


/*------------------------------------------------
 * Func : dprx_video_common_set_vsync_inverse
 *
 * Desc  : set video common vsync inverse
 *
 * Para : p_adp : handle of DPRX VC HW
 *        enable: 0:positive
 *                1:negative
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_vsync_inverse(vc_dev_t* p_dev, unsigned char enable)
{
    if (p_dev && p_dev->ops.set_vs_inverse)
        return p_dev->ops.set_vs_inverse(&p_dev->ctx, enable);
    return 0;
}

/*------------------------------------------------
 * Func : dprx_video_common_set_hsync_inverse
 *
 * Desc  : set video common hsync inverse
 *
 * Para : p_adp : handle of DPRX VC HW
 *        enable: 0:positive
 *                1:negative
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_hsync_inverse(vc_dev_t* p_dev, unsigned char enable)
{
    if (p_dev && p_dev->ops.set_hs_inverse)
        return p_dev->ops.set_hs_inverse(&p_dev->ctx, enable);
    return 0;
}



/*------------------------------------------------
 * Func : dprx_video_common_set_line_drop_en
 *
 * Desc  : set video common line drop enable
 *
 * Para : p_adp : handle of DPRX VC HW
 *
 * Retn : 0 : successed, others : failed
 *-----------------------------------------------*/
int dprx_video_common_set_line_drop_en(vc_dev_t* p_dev, unsigned char enable)
{
    if (p_dev && p_dev->ops.set_line_drop)
        return p_dev->ops.set_line_drop(&p_dev->ctx, enable);
    return 0;
}


//----------------------------------------------------------------------------------------
// Low Level API
//----------------------------------------------------------------------------------------


/*------------------------------------------------
 * Func : alloc_dprx_video_common_device
 *
 * Desc : alocate a dprx_video_common device
 *
 * Para : N/A
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
vc_dev_t* alloc_dprx_video_common_device(void)
{
    vc_dev_t* p_dev = dprx_osal_malloc(sizeof(vc_dev_t));

    if (p_dev)
        memset(p_dev, 0, sizeof(vc_dev_t));

    return p_dev;
}


/*------------------------------------------------
 * Func : destroy_dprx_video_common_device
 *
 * Desc : dsatroy a dprx video_common device
 *
 * Para : N/A
 *
 * Retn : 0 : successed, 0< : failed
 *-----------------------------------------------*/
void destroy_dprx_video_common_device(vc_dev_t* p_dev)
{
    if (p_dev)
    {
        dprx_video_common_uninit(p_dev);
        dprx_osal_mfree(p_dev);
    }
}

