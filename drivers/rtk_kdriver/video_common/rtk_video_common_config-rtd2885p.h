/*=============================================================
 * File:    rtk_hdmi_dp_common_config-rtd2885p.c
 *
 * Desc:    common reg
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
#include "rtk_video_common-osal.h"
#include "rbus/dprx_420_reg.h"
#define COMMON_PORT_NUM    1

#ifndef uint32_t
typedef unsigned int __UINT32_T;
#define uint32_t __UINT32_T
#endif

struct hdmi_dp_common_reg_st
{
    //dp or hdmi select
    uint32_t dp_hdmi_sel;

    uint32_t mbist_mode3;
    uint32_t mbist_misc3;
    uint32_t mbist_misc3_1;

    // yuv420 to 444
    uint32_t hdmirx_yuv420_ctrl_0;
    uint32_t hdmirx_yuv420_ctrl_1;

    uint32_t cts_fifo_ctrl;
    uint32_t ctrl_flag;
    uint32_t cp_mac_test_in;
    uint32_t cp_mac_test_check;

    uint32_t video_and_offms_patch_sel;

    // line drop
    uint32_t line_drop_bist_0;
    uint32_t line_drop_bist_1;
    uint32_t line_drop_ctrl_0;
    uint32_t line_drop_ctrl_1;
};

typedef enum {
    SRC_IN_HDMIRX = 0,
    SRC_IN_DPRX = 1,
    SRC_IN_HDMI_DSCD = 2,
    SRC_IN_DPRX_DSCD = 3,
}VIDEO_COMMON_IN_SEL_REG;

static unsigned char hdmi_common_alloc_rule[] = {VIDEO_COMMON_0, VIDEO_COMMON_0, VIDEO_COMMON_0, VIDEO_COMMON_0};
static unsigned char dp_common_alloc_rule[] = {VIDEO_COMMON_0, VIDEO_COMMON_0, VIDEO_COMMON_0, VIDEO_COMMON_0};

/*--------------------------------------------------
 * Video Common register define
 *--------------------------------------------------*/

#define  COMMON_hdmi_dp_select_reg                  (hdmi_dp_common[nport].dp_hdmi_sel)
#define  COMMON_MBIST_MODE3_reg                     (hdmi_dp_common[nport].mbist_mode3)
#define  COMMON_MBIST_MISC3_reg                     (hdmi_dp_common[nport].mbist_misc3)
#define  COMMON_MBIST_MISC3_1_reg                   (hdmi_dp_common[nport].mbist_misc3_1)
#define  COMMON_HDMIRX_YUV420_CTRL_0_reg            (hdmi_dp_common[nport].hdmirx_yuv420_ctrl_0)
#define  COMMON_HDMIRX_YUV420_CTRL_1_reg            (hdmi_dp_common[nport].hdmirx_yuv420_ctrl_1)
#define  COMMON_CTS_FIFO_CTRL_reg                   (hdmi_dp_common[nport].cts_fifo_ctrl)
#define  COMMON_CTRL_FLAG_reg                       (hdmi_dp_common[nport].ctrl_flag)
#define  COMMON_CP_MAC_TEST_IN_reg                  (hdmi_dp_common[nport].cp_mac_test_in)
#define  COMMON_CP_MAC_TEST_CHECK_reg               (hdmi_dp_common[nport].cp_mac_test_check)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg        (hdmi_dp_common[nport].video_and_offms_patch_sel)
#define  COMMON_LINE_DROP_BIST_0_reg                (hdmi_dp_common[nport].line_drop_bist_0)
#define  COMMON_LINE_DROP_BIST_1_reg                (hdmi_dp_common[nport].line_drop_bist_1)
#define  COMMON_LINE_DROP_CTRL_0_reg                (hdmi_dp_common[nport].line_drop_ctrl_0)
#define  COMMON_LINE_DROP_CTRL_1_reg                (hdmi_dp_common[nport].line_drop_ctrl_1)


/*---------HDMI_DP_COMMON Register Mapping--------*/
#define  COMMON_VIDEO_P0_hdmi_dp_select_mask                           SYS_REG_SYS_CLKSEL_hdmi_dp_sel_mask
#define  COMMON_VIDEO_P0_hdmi_dp_select(data)                          SYS_REG_SYS_CLKSEL_hdmi_dp_sel(data)
#define  COMMON_VIDEO_get_P0_hdmi_dp_select(data)                      SYS_REG_SYS_CLKSEL_get_hdmi_dp_sel(data)

#define  COMMON_YUV420_CTRL_0_yuv420_cr_en_mask                        DPRX_420_DPRX_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask
#define  COMMON_YUV420_CTRL_0_yuv420_cr_en(data)                       DPRX_420_DPRX_HDMIRX_YUV420_CTRL_0_yuv420_cr_en(data)
#define  COMMON_YUV420_CTRL_0_get_yuv420_cr_en(data)                   DPRX_420_DPRX_HDMIRX_YUV420_CTRL_0_get_yuv420_cr_en(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask            DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(data)           DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_yuv420_in_sel(data)       DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_get_yuv420_in_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask            DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(data)           DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_offms_out_sel(data)       DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_get_offms_out_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel_mask       DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel(data)      DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_dprx_pixel_mode_sel(data)  DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_get_dprx_pixel_mode_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask       DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(data)      DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_transform_mode_sel(data)  DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_transform_mode_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel_mask              DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel(data)             DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_out_sel(data)         DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_out_sel(data)



/*--------------------------------------------------
 * Mac register setting
 *--------------------------------------------------*/
static struct hdmi_dp_common_reg_st hdmi_dp_common[COMMON_PORT_NUM] =
{
    // common 0
    {
        .dp_hdmi_sel                         = SYS_REG_SYS_CLKSEL_reg,

        .mbist_mode3                         = DPRX_420_MBIST_DPRX_MODE3_reg,
        .mbist_misc3                         = DPRX_420_MBIST_DPRX_MISC3_reg,
        .mbist_misc3_1                       = DPRX_420_MBIST_DPRX_MISC3_1_reg,

        // yuv420 to 444
        .hdmirx_yuv420_ctrl_0                = DPRX_420_DPRX_HDMIRX_YUV420_CTRL_0_reg,
        .hdmirx_yuv420_ctrl_1                = DPRX_420_DPRX_HDMIRX_YUV420_CTRL_1_reg,

        .cts_fifo_ctrl                       = DPRX_420_DPRX_CTS_FIFO_CTRL_reg,
        .ctrl_flag                           = DPRX_420_DPRX_CTRL_FLAG_reg,
        .cp_mac_test_in                      = DPRX_420_CP_MAC_TEST_IN_reg,
        .cp_mac_test_check                   = DPRX_420_CP_MAC_TEST_CHECK_reg,

        .video_and_offms_patch_sel           = DPRX_420_VIDEO_AND_OFFMS_PATH_SEL_reg,

        // line drop
        .line_drop_bist_0                    = DPRX_420_LINE_DROP_BIST_0_reg,
        .line_drop_bist_1                    = DPRX_420_LINE_DROP_BIST_1_reg,
        .line_drop_ctrl_0                    = DPRX_420_DPRX_LINE_DROP_CTRL_0_reg,
        .line_drop_ctrl_1                    = DPRX_420_DPRX_LINE_DROP_CTRL_1_reg,
    },
};
