/*=============================================================
 * File:    rtk_hdmi_dp_common_config-rtd2819a.c
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
#include <rbus/M8P_hdmi_dp_p0_common_reg.h>
#include <rbus/M8P_hdmi_dp_p1_common_reg.h>
#include <rbus/M8P_hdmi_dp_common_misc_reg.h>
#include <rbus/M8P_sys_reg_reg.h>
#define COMMON_PORT_NUM    2

#ifndef uint32_t
typedef unsigned int __UINT32_T;
#define uint32_t __UINT32_T
#endif

/*--------------------------------------------------
 * Video Common Feature CONFIG
 *--------------------------------------------------*/
#define OWERNSHIP_SELECT_BY_AP

#define port_instance {\
    {\
        .src_in        = VIDEO_COMMON_DPRX,\
        .src_type      = DP_1p4,\
        .ch            = 0x00,\
        .dscd_in       = VIDEO_COMMON_DSCD_0,\
    },\
    {\
        .src_in        = VIDEO_COMMON_HDMIRX,\
        .src_type      = HDMI_2p0,\
        .ch            = 0x00,\
        .dscd_in       = VIDEO_COMMON_DSCD_0,\
    },\
}


struct hdmi_dp_common_reg_st
{
    uint32_t sys_clken;
    uint32_t sys_srst;

    uint32_t dp_hdmi_sel;

    uint32_t yuv422_cd;
    uint32_t vs_count;

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
    //sync inverse
    uint32_t vs_hs_inverse;

    // line drop
    uint32_t line_drop_bist_0;
    uint32_t line_drop_bist_1;
    uint32_t line_drop_ctrl_0;
    uint32_t line_drop_ctrl_1;

    // CRC
    uint32_t crc_ctrl;
    uint32_t crc_result;
    uint32_t des_crc;
    uint32_t crc_err_cnt0;

    // clock select
    uint32_t clock_sel;
};

typedef enum {
    SRC_IN_HDMIRX = 0,
    SRC_IN_DPRX = 1,
    SRC_IN_HDMI_DSCD = 2,
    SRC_IN_DPRX_DSCD = 3,
}VIDEO_COMMON_IN_SEL_REG;

static unsigned char hdmi_common_alloc_rule[] = {VIDEO_COMMON_2, VIDEO_COMMON_3, VIDEO_COMMON_0, VIDEO_COMMON_1};
static unsigned char dp_common_alloc_rule[] = {VIDEO_COMMON_0, VIDEO_COMMON_1, VIDEO_COMMON_2, VIDEO_COMMON_3};

/*--------------------------------------------------
 * Video Common register define
 *--------------------------------------------------*/
#define  COMMON_hdmi_dp_clken_reg                   (hdmi_dp_common[nport].sys_clken)
#define  COMMON_hdmi_dp_srst_reg                    (hdmi_dp_common[nport].sys_srst)

#define  COMMON_hdmi_dp_select_reg                  (hdmi_dp_common[nport].dp_hdmi_sel)

#define  COMMON_YUV422_CD_reg                       (hdmi_dp_common[nport].yuv422_cd)
#define  COMMON_VS_COUNT_reg                        (hdmi_dp_common[nport].vs_count)
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
#define  COMMON_VS_HS_INV                           (hdmi_dp_common[nport].vs_hs_inverse)
#define  COMMON_LINE_DROP_BIST_0_reg                (hdmi_dp_common[nport].line_drop_bist_0)
#define  COMMON_LINE_DROP_BIST_1_reg                (hdmi_dp_common[nport].line_drop_bist_1)
#define  COMMON_LINE_DROP_CTRL_0_reg                (hdmi_dp_common[nport].line_drop_ctrl_0)
#define  COMMON_LINE_DROP_CTRL_1_reg                (hdmi_dp_common[nport].line_drop_ctrl_1)
#define  COMMON_CRC_Ctrl_reg                        (hdmi_dp_common[nport].crc_ctrl)
#define  COMMON_CRC_Result_reg                      (hdmi_dp_common[nport].crc_result)
#define  COMMON_DES_CRC_reg                         (hdmi_dp_common[nport].des_crc)
#define  COMMON_CRC_ERR_CNT0_reg                    (hdmi_dp_common[nport].crc_err_cnt0)
#define  COMMON_CKL_SEL_reg                    (hdmi_dp_common[nport].clock_sel)

/*---------Clock Select Register Mapping--------*/
#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_CLK_SEL_mask           M8P_SYS_REG_SYS_CLKSEL_hdmirx0_2p_clk_sel_mask
#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_CLK_SEL(data)          M8P_SYS_REG_SYS_CLKSEL_hdmirx0_2p_clk_sel(data)
#define  COMMON_VIDEO_REG_GET_VIDEO_COMMON_0_CLK_SEL(data)      M8P_SYS_REG_SYS_CLKSEL_get_hdmirx0_2p_clk_sel(data)

#define  COMMON_VIDEO_REG_VIDEO_COMMON_1_CLK_SEL_mask           M8P_SYS_REG_SYS_DISPCLK_FRACT_2_hdmirx1_2p_clk_sel_mask
#define  COMMON_VIDEO_REG_VIDEO_COMMON_1_CLK_SEL(data)          M8P_SYS_REG_SYS_DISPCLK_FRACT_2_hdmirx1_2p_clk_sel(data)
#define  COMMON_VIDEO_REG_GET_VIDEO_COMMON_1_CLK_SEL(data)      M8P_SYS_REG_SYS_DISPCLK_FRACT_2_get_hdmirx1_2p_clk_sel(data)


/*---------HDMI_DP_COMMON CRT Register Mapping--------*/
#define  COMMON_VIDEO_CRT_REG                                         1
#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p3_mask           M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p3_mask
#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p3(data)          M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p3(data)
#define  COMMON_VIDEO_REG_SYS_SRST_get_rstn_hdmi_dp_420_p3(data)      M8P_SYS_REG_SYS_SRST6_get_rstn_hdmi_dp_420_p3(data)

#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p2_mask           M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p2_mask
#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p2(data)          M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p2(data)
#define  COMMON_VIDEO_REG_SYS_SRST_get_rstn_hdmi_dp_420_p2(data)      M8P_SYS_REG_SYS_SRST6_get_rstn_hdmi_dp_420_p2(data)

#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p1_mask           M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p1_mask
#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p1(data)          M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p1(data)
#define  COMMON_VIDEO_REG_SYS_SRST_get_rstn_hdmi_dp_420_p1(data)      M8P_SYS_REG_SYS_SRST6_get_rstn_hdmi_dp_420_p1(data)

#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p0_mask           M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p0_mask
#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p0(data)          M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_p0(data)
#define  COMMON_VIDEO_REG_SYS_SRST_get_rstn_hdmi_dp_420_p0(data)      M8P_SYS_REG_SYS_SRST6_get_rstn_hdmi_dp_420_p0(data)

#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_all_mask           M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_all_mask
#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_all(data)          M8P_SYS_REG_SYS_SRST6_rstn_hdmi_dp_420_all(data)
#define  COMMON_VIDEO_REG_SYS_SRST_get_rstn_hdmi_dp_420_all(data)      M8P_SYS_REG_SYS_SRST6_get_rstn_hdmi_dp_420_all(data)


#define  COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p1_mask         M8P_SYS_REG_SYS_CLKEN6_clken_hdmi_dp_420_p1_mask
#define  COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p1(data)        M8P_SYS_REG_SYS_CLKEN6_clken_hdmi_dp_420_p1(data)
#define  COMMON_VIDEO_REG_SYS_CLKEN_get_clken_hdmi_dp_420_p1(data)    M8P_SYS_REG_SYS_CLKEN6_get_clken_hdmi_dp_420_p1(data)

#define  COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p0_mask         M8P_SYS_REG_SYS_CLKEN6_clken_hdmi_dp_420_p0_mask
#define  COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p0(data)        M8P_SYS_REG_SYS_CLKEN6_clken_hdmi_dp_420_p0(data)
#define  COMMON_VIDEO_REG_SYS_CLKEN_get_clken_hdmi_dp_420_p0(data)    M8P_SYS_REG_SYS_CLKEN6_get_clken_hdmi_dp_420_p0(data)

#define  COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_all_mask         M8P_SYS_REG_SYS_CLKEN6_clken_hdmi_dp_420_all_mask
#define  COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_all(data)        M8P_SYS_REG_SYS_CLKEN6_clken_hdmi_dp_420_all(data)
#define  COMMON_VIDEO_REG_SYS_CLKEN_get_clken_hdmi_dp_420_all(data)    M8P_SYS_REG_SYS_CLKEN6_get_clken_hdmi_dp_420_all(data)

#define  COMMON_VIDEO_REG_SYS_CLKEN6_reg                               M8P_SYS_REG_SYS_CLKEN6_reg
#define  COMMON_VIDEO_REG_SYS_SRST6_reg                                M8P_SYS_REG_SYS_SRST6_reg

#define  COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_write_data(data)   M8P_SYS_REG_SYS_SRST6_write_data(data)
#define  COMMON_VIDEO_REG_SYS_CLK_clken_hdmi_dp_420_write_data(data)   M8P_SYS_REG_SYS_CLKEN6_write_data(data)

#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_VS_INV_mask             HDMI_DP_COMMON_P0_COMMON_VS_HS_INV_common_vs_inv_mask
#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_VS_INV(data)            HDMI_DP_COMMON_P0_COMMON_VS_HS_INV_common_vs_inv(data)
#define  COMMON_VIDEO_REG_GET_VIDEO_COMMON_0_COMMON_VS_INV(data)        HDMI_DP_COMMON_P0_COMMON_VS_HS_INV_get_common_vs_inv(data)

#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_HS_INV_mask             HDMI_DP_COMMON_P0_COMMON_VS_HS_INV_common_hs_inv_mask
#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_HS_INV(data)            HDMI_DP_COMMON_P0_COMMON_VS_HS_INV_common_hs_inv(data)
#define  COMMON_VIDEO_REG_GET_VIDEO_COMMON_0_COMMON_HS_INV(data)        HDMI_DP_COMMON_P0_COMMON_VS_HS_INV_get_common_hs_inv(data)

/*---------LINE DROP Register Mapping--------*/
#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_LINE_DROP_EN_mask             HDMI_DP_COMMON_P0_LINE_DROP_CTRL_0_line_drop_en_mask
#define  COMMON_VIDEO_REG_VIDEO_COMMON_0_LINE_DROP_EN(data)            HDMI_DP_COMMON_P0_LINE_DROP_CTRL_0_line_drop_en(data)
#define  COMMON_VIDEO_REG_GET_VIDEO_COMMON_0_LINE_DROP_EN(data)        HDMI_DP_COMMON_P0_LINE_DROP_CTRL_0_get_line_drop_en(data)

/*---------HDMI_DP_COMMON Register Mapping--------*/

#if 0  // disable port selection, port selection is control by phy setting
#define  COMMON_VIDEO_P0_hdmi_dp_select_mask                           M8P_SYS_REG_SYS_CLKSEL_hdmi_dp_p0_sel_mask
#define  COMMON_VIDEO_P0_hdmi_dp_select(data)                          M8P_SYS_REG_SYS_CLKSEL_hdmi_dp_p0_sel(data)
#define  COMMON_VIDEO_get_P0_hdmi_dp_select(data)                      M8P_SYS_REG_SYS_CLKSEL_get_hdmi_dp_p0_sel(data)

#define  COMMON_VIDEO_P1_hdmi_dp_select_mask                           M8P_SYS_REG_SYS_CLKSEL_hdmi_dp_p1_sel_mask
#define  COMMON_VIDEO_P1_hdmi_dp_select(data)                          M8P_SYS_REG_SYS_CLKSEL_hdmi_dp_p1_sel(data)
#define  COMMON_VIDEO_get_P1_hdmi_dp_select(data)                      M8P_SYS_REG_SYS_CLKSEL_get_hdmi_dp_p1_sel(data)
#endif 

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_no_422_444_repeat_mask        HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_no_422_444_repeat_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_no_422_444_repeat(data)       HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_no_422_444_repeat(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_no_422_444_repeat(data)   HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_no_422_444_repeat(data)

#define  COMMON_HDMIRX_YUV420_CTRL_0_yuv422_en_mask                    HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_0_yuv422_en_mask
#define  COMMON_HDMIRX_YUV420_CTRL_0_yuv422_en(data)                   HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_0_yuv422_en(data)
#define  COMMON_HDMIRX_YUV420_CTRL_0_get_yuv422_en(data)               HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_0_get_yuv422_en(data)

#define  COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask                 HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask
#define  COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en(data)                HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_0_yuv420_cr_en(data)
#define  COMMON_HDMIRX_YUV420_CTRL_0_get_yuv420_cr_en(data)            HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_0_get_yuv420_cr_en(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask            HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(data)           HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_yuv420_in_sel(data)       HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_yuv420_in_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel_mask              HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel(data)             HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_dscd_in_sel(data)         HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_dscd_in_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_audio_sel_mask                HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_audio_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_audio_sel(data)               HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_audio_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_audio_sel(data)           HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_audio_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask            HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(data)           HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_offms_out_sel(data)       HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_offms_out_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel_mask      HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel(data)     HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_dprx_pixel_mode_sel(data) HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_dprx_pixel_mode_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask       HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(data)      HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_transform_mode_sel(data)  HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_transform_mode_sel(data)

#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel_mask            HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel_mask
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel(data)           HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel(data)
#define  COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_out_sel(data)       HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_get_pixel_out_sel(data)

#define  COMMON_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en_mask          HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en_mask
#define  COMMON_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en(data)         HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en(data)
#define  COMMON_HDMIRX_YUV420_CTRL_1_get_yuv420_clk_valid_en(data)     HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_1_get_yuv420_clk_valid_en(data)

/*---------HDMI_DP_COMMON MISC Register Mapping--------*/
#define  COMMON_MISC_COMMON_PORT_SEL_reg                               M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_reg

#define  COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel_mask            M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel_mask
#define  COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel(data)           M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel(data)
#define  COMMON_MISC_COMMON_PORT_SEL_get_common_bypass_sel(data)       M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_get_common_bypass_sel(data)

#define  COMMON_MISC_COMMON_PORT_SEL_common_offms_sel_mask             M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_common_offms_sel_mask
#define  COMMON_MISC_COMMON_PORT_SEL_common_offms_sel(data)            M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_common_offms_sel(data)
#define  COMMON_MISC_COMMON_PORT_SEL_get_common_offms_sel(data)        M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_get_common_offms_sel(data)

#define  COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel_mask           M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel_mask
#define  COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel(data)          M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel(data)
#define  COMMON_MISC_COMMON_PORT_SEL_get_bypass_422_raw_sel(data)      M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_get_bypass_422_raw_sel(data)

#define  COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel_mask          M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel_mask
#define  COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel(data)         M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel(data)
#define  COMMON_MISC_COMMON_PORT_SEL_get_dscd2_dprx_1p2p_sel(data)     M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_get_dscd2_dprx_1p2p_sel(data)

#define  COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel_mask           M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel_mask
#define  COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel(data)          M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel(data)
#define  COMMON_MISC_COMMON_PORT_SEL_get_dscd_dprx_1p2p_sel(data)      M8P_HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_get_dscd_dprx_1p2p_sel(data)

/*--------------------------------------------------
 * Mac register setting
 *--------------------------------------------------*/
static struct hdmi_dp_common_reg_st hdmi_dp_common[COMMON_PORT_NUM] =
{
    // common 0
    {
        .sys_clken                           = M8P_SYS_REG_SYS_CLKEN6_reg,
        .sys_srst                            = M8P_SYS_REG_SYS_SRST6_reg,
        .dp_hdmi_sel                         = M8P_SYS_REG_SYS_CLKSEL_reg,
        .yuv422_cd                           = HDMI_DP_COMMON_P0_YUV422_CD_reg,
        .vs_count                            = HDMI_DP_COMMON_P0_VS_COUNT_reg,

        .mbist_mode3                         = HDMI_DP_COMMON_P0_MBIST_MODE3_reg,
        .mbist_misc3                         = HDMI_DP_COMMON_P0_MBIST_MISC3_reg,
        .mbist_misc3_1                       = HDMI_DP_COMMON_P0_MBIST_MISC3_1_reg,

        // yuv420 to 444
        .hdmirx_yuv420_ctrl_0                = HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_0_reg,
        .hdmirx_yuv420_ctrl_1                = HDMI_DP_COMMON_P0_HDMIRX_YUV420_CTRL_1_reg,

        .cts_fifo_ctrl                       = HDMI_DP_COMMON_P0_CTS_FIFO_CTRL_reg,
        .ctrl_flag                           = HDMI_DP_COMMON_P0_CTRL_FLAG_reg,
        .cp_mac_test_in                      = HDMI_DP_COMMON_P0_CP_MAC_TEST_IN_reg,
        .cp_mac_test_check                   = HDMI_DP_COMMON_P0_CP_MAC_TEST_CHECK_reg,

        .video_and_offms_patch_sel           = HDMI_DP_COMMON_P0_VIDEO_AND_OFFMS_PATH_SEL_reg,

        .vs_hs_inverse                       = HDMI_DP_COMMON_P0_COMMON_VS_HS_INV,

        // line drop
        .line_drop_bist_0                    = HDMI_DP_COMMON_P0_LINE_DROP_BIST_0_reg,
        .line_drop_bist_1                    = HDMI_DP_COMMON_P0_LINE_DROP_BIST_1_reg,
        .line_drop_ctrl_0                    = HDMI_DP_COMMON_P0_LINE_DROP_CTRL_0_reg,
        .line_drop_ctrl_1                    = HDMI_DP_COMMON_P0_LINE_DROP_CTRL_1_reg,

        // CRC
        .crc_ctrl                            = HDMI_DP_COMMON_P0_CRC_Ctrl_reg,
        .crc_result                          = HDMI_DP_COMMON_P0_CRC_Result_reg,
        .des_crc                             = HDMI_DP_COMMON_P0_DES_CRC_reg,
        .crc_err_cnt0                        = HDMI_DP_COMMON_P0_CRC_ERR_CNT0_reg,

        .clock_sel                           = M8P_SYS_REG_SYS_CLKSEL_reg,
    },

    //common 1
    {
        .sys_clken                           = M8P_SYS_REG_SYS_CLKEN6_reg,
        .sys_srst                            = M8P_SYS_REG_SYS_SRST6_reg,
        .dp_hdmi_sel                         = M8P_SYS_REG_SYS_CLKSEL_reg,
        .yuv422_cd                           = M8P_HDMI_DP_P1_COMMON_YUV422_CD_reg, 
        .vs_count                            = M8P_HDMI_DP_P1_COMMON_VS_COUNT_reg,

        .mbist_mode3                         = M8P_HDMI_DP_P1_COMMON_MBIST_MODE3_reg,
        .mbist_misc3                         = M8P_HDMI_DP_P1_COMMON_MBIST_MISC3_reg,
        .mbist_misc3_1                       = M8P_HDMI_DP_P1_COMMON_MBIST_MISC3_1_reg,

        // yuv420 to 444
        .hdmirx_yuv420_ctrl_0                = M8P_HDMI_DP_P1_COMMON_HDMIRX_YUV420_CTRL_0_reg,
        .hdmirx_yuv420_ctrl_1                = M8P_HDMI_DP_P1_COMMON_HDMIRX_YUV420_CTRL_1_reg,

        .cts_fifo_ctrl                       = M8P_HDMI_DP_P1_COMMON_CTS_FIFO_CTRL_reg,
        .ctrl_flag                           = M8P_HDMI_DP_P1_COMMON_CTRL_FLAG_reg,
        .cp_mac_test_in                      = M8P_HDMI_DP_P1_COMMON_CP_MAC_TEST_IN_reg,
        .cp_mac_test_check                   = M8P_HDMI_DP_P1_COMMON_CP_MAC_TEST_CHECK_reg,

        .video_and_offms_patch_sel           = M8P_HDMI_DP_P1_COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg,

        .vs_hs_inverse                       = M8P_HDMI_DP_P1_COMMON_COMMON_VS_HS_INV,

        // line drop
        .line_drop_bist_0                    = M8P_HDMI_DP_P1_COMMON_LINE_DROP_BIST_0_reg,
        .line_drop_bist_1                    = M8P_HDMI_DP_P1_COMMON_LINE_DROP_BIST_1_reg,
        .line_drop_ctrl_0                    = M8P_HDMI_DP_P1_COMMON_LINE_DROP_CTRL_0_reg,
        .line_drop_ctrl_1                    = M8P_HDMI_DP_P1_COMMON_LINE_DROP_CTRL_1_reg,
	
        // CRC
        .crc_ctrl                            = M8P_HDMI_DP_P1_COMMON_CRC_Ctrl_reg,
        .crc_result                          = M8P_HDMI_DP_P1_COMMON_CRC_Result_reg,
        .des_crc                             = M8P_HDMI_DP_P1_COMMON_DES_CRC_reg,
        .crc_err_cnt0                        = M8P_HDMI_DP_P1_COMMON_CRC_ERR_CNT0_reg,

        .clock_sel                           = M8P_SYS_REG_SYS_DISPCLK_FRACT_2_reg,
    },
};
