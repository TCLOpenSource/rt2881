/*=============================================================
 * File:    dprx_video_common.h
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
#ifndef __DPRX_VIDEO_COMMON_H__
#define __DPRX_VIDEO_COMMON_H__

#include <dprx_osal.h>
#include <dprx_types.h>
#include <dprx_dev.h>  // dev_ctx_t

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DPRX_VC_PIXEL_MODE_ONE_PIXEL = 0,
    DPRX_VC_PIXEL_MODE_TWO_PIXEL = 1,
} DPRX_VC_PIXEL_MODE_E;

typedef enum
{
    DPRX_VC_COLOR_SPACE_RGB = 0,
    DPRX_VC_COLOR_SPACE_YUV422,
    DPRX_VC_COLOR_SPACE_YUV444,
    DPRX_VC_COLOR_SPACE_YUV420,
    DPRX_VC_COLOR_SPACE_YONLY,
    DPRX_VC_COLOR_SPACE_UNKNOWN,
} DPRX_VC_COLOR_SPACE_E;

typedef enum
{
    DPRX_VC_TYPE_DP1p4 = 0,
    DPRX_VC_TYPE_DP1p4_DSC,
    DPRX_VC_TYPE_DP2p0,
    DPRX_VC_TYPE_DP2p0_DSC,
} DPRX_VC_PORT_TYPE;

typedef struct
{
    int (*init)   (dev_ctx_t* p_ctx);
    int (*uninit) (dev_ctx_t* p_ctx);
    int (*check_ownership) (dev_ctx_t* p_ctx);
    int (*set_yuv420_to_444) (dev_ctx_t* p_ctx, unsigned char enable);
    int (*set_output_pixel_mode) (dev_ctx_t* p_ctx, DPRX_VC_PIXEL_MODE_E pixel_mode);
    int (*set_input_pixel_mode) (dev_ctx_t* p_ctx, DPRX_VC_PIXEL_MODE_E pixel_mode);
    int (*set_dsc_input_sel) (dev_ctx_t* p_ctx, unsigned char enable);
    int (*set_color_space) (dev_ctx_t* p_ctx, unsigned char color_space);
    int (*set_src_type) (dev_ctx_t* p_ctx, unsigned char type);
    int (*set_clk_div) (dev_ctx_t* p_ctx, unsigned char enable);
    int (*set_bypass_port) (dev_ctx_t* p_ctx, unsigned char enable);
    int (*set_hs_inverse) (dev_ctx_t* p_ctx, unsigned char enable);
    int (*set_vs_inverse) (dev_ctx_t* p_ctx, unsigned char enable);
    int (*set_line_drop) (dev_ctx_t* p_ctx, unsigned char enable);

}vc_dev_ops_t;

typedef struct
{
    dev_ctx_t      ctx;
    vc_dev_ops_t   ops;
}vc_dev_t;

//================= API of DPRX HPD ==============

extern vc_dev_t* alloc_dprx_video_common_device(void);
extern void destroy_dprx_video_common_device(vc_dev_t* p_dev);

extern int dprx_video_common_init(vc_dev_t* p_dev);
extern int dprx_video_common_uninit(vc_dev_t* p_dev);
extern int dprx_video_common_check_ownership(vc_dev_t* p_dev);
extern int dprx_video_common_set_yuv420_to_444(vc_dev_t* p_dev, unsigned char enable);
extern int dprx_video_common_set_output_pixel_mode(vc_dev_t* p_dev, DPRX_VC_PIXEL_MODE_E pixel_mode);
extern int dprx_video_common_set_input_pixel_mode(vc_dev_t* p_dev, DPRX_VC_PIXEL_MODE_E pixel_mode);
extern int dprx_video_common_set_dsc_input_sel(vc_dev_t* p_dev, unsigned char enable);
extern int dprx_video_common_set_color_space(vc_dev_t* p_dev, unsigned char color_space);
extern int dprx_video_common_set_src_type(vc_dev_t* p_dev, unsigned char type);
extern int dprx_video_common_set_clk_div_en(vc_dev_t* p_dev, unsigned char enable);
extern int dprx_video_common_set_bypass_port(vc_dev_t* p_dev, unsigned char enable);
extern int dprx_video_common_set_vsync_inverse(vc_dev_t* p_dev,unsigned char enable) ;
extern int dprx_video_common_set_hsync_inverse(vc_dev_t* p_dev,unsigned char enable) ;
extern int dprx_video_common_set_line_drop_en(vc_dev_t* p_dev, unsigned char enable);

//======== DPRX HPD debug message control ========
#define DPRX_VC_ERR(fmt, args...)         dprx_err("[DP][VC][ERR] " fmt, ## args)
#define DPRX_VC_WARN(fmt, args...)        dprx_warn("[DP][VC][WARN] " fmt, ## args)
#define DPRX_VC_INFO(fmt, args...)        dprx_info("[DP][VC][INFO] " fmt, ## args)
#define DPRX_VC_DBG(fmt, args...)         dprx_dbg("[DP][VC][DEBUG] " fmt, ## args)

#ifdef __cplusplus
}
#endif

#endif // __DPRX_VIDEO_COMMON_H__
