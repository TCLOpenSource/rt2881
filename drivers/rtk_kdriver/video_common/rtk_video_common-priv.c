/*=============================================================
 * File:    rtk_video_common-priv.c
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
#include "rtk_video_common-priv.h"
#include "rtk_video_common-debug.h"

/**********************************************************************************************
*
*   Variables
*
**********************************************************************************************/



//****************************************************************************
// Function Definitions
//****************************************************************************
/*------------------------------------------------
 * Func : lib_video_common_crt_all_enable
 *
 * Desc : enable all video common port
 *
 * Para : N/A
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_crt_all_enable(void)
{
#ifdef COMMON_VIDEO_CRT_REG
    RTK_VIDEO_COMMON_INFO("%s\n",__func__);
    //----------------------------------------------------------------------------
    // Sync Reset flow
    // clock off -> rst on -> clock on -> clk off ->  rst off -> clock on
    //----------------------------------------------------------------------------
    rtd_outl(COMMON_VIDEO_REG_SYS_SRST6_reg, COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_all_mask | COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_write_data(1));   // reset off
    udelay(100);
    rtd_outl(COMMON_VIDEO_REG_SYS_CLKEN6_reg, COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_all_mask | COMMON_VIDEO_REG_SYS_CLK_clken_hdmi_dp_420_write_data(1));    // clk on
    udelay(100);
#endif // COMMON_VIDEO_CRT_REG
}

/*------------------------------------------------
 * Func : lib_video_common_crt_enable
 *
 * Desc : enable video common by port
 *
 * Para : nport : video common port
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_crt_enable(unsigned char nport)
{
#ifdef COMMON_VIDEO_CRT_REG
    unsigned int clk_val = 0;
    unsigned int reset_val = 0;
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

    RTK_VIDEO_COMMON_INFO("%s\n",__func__);

    switch (nport)
    {
        case 0:
#ifdef COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p0_mask
            clk_val = COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p0_mask;
            reset_val = COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p0_mask;
#endif // COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p0_mask
        break;

        case 1:
#ifdef COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p1_mask
            clk_val = COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p1_mask;
            reset_val = COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p1_mask;
#endif // COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p1_mask
        break;

        case 2:
#ifdef COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p2_mask
            clk_val = COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p2_mask;
            reset_val = COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p2_mask;
#endif // COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p2_mask
        break;

        case 3:
#ifdef COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p3_mask
            clk_val = COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p3_mask;
            reset_val = COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_p3_mask;
#endif // COMMON_VIDEO_REG_SYS_CLKEN_clken_hdmi_dp_420_p3_mask
        break;

        default:
            return;
    }

    //----------------------------------------------------------------------------
    // Sync Reset flow
    // clock off -> rst on -> clock on -> clk off ->  rst off -> clock on
    //----------------------------------------------------------------------------

    rtd_outl(COMMON_hdmi_dp_srst_reg, reset_val | COMMON_VIDEO_REG_SYS_SRST_rstn_hdmi_dp_420_write_data(1));   // reset off
    udelay(100);
    rtd_outl(COMMON_hdmi_dp_clken_reg, clk_val | COMMON_VIDEO_REG_SYS_CLK_clken_hdmi_dp_420_write_data(1));    // clk on
    udelay(100);
#endif // COMMON_VIDEO_CRT_REG
}


//--------------------------------------------------
// Func  : lib_video_common_get_hdmi_dp_sel
//
// Desc  : get hdmi dp_sel value
//
// Param : ch : channel port
//
// Retn  : 0 : HDMI, others: DP
//--------------------------------------------------
unsigned char lib_video_common_get_hdmi_dp_sel(unsigned char nport)
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return 0;
    }

    switch(nport)
    {
        case 0:
#ifdef COMMON_VIDEO_P0_hdmi_dp_select_mask
            return COMMON_VIDEO_get_P0_hdmi_dp_select(rtd_inl(COMMON_hdmi_dp_select_reg));
#endif
            break;
        case 1:
#ifdef COMMON_VIDEO_P1_hdmi_dp_select_mask
            return COMMON_VIDEO_get_P1_hdmi_dp_select(rtd_inl(COMMON_hdmi_dp_select_reg));
#endif
            break;
        case 2:
            break;
        case 3:
            break;
        default :
            break;
    }

    return 0;
}

//--------------------------------------------------
// Func  : lib_video_common_set_hdmi_dp_sel
//
// Desc  : set hdmi dp_sel value
//
// Param : 0 : HDMI, others: DP
//
// Retn  : N/A
//--------------------------------------------------
void lib_video_common_set_hdmi_dp_sel(
    unsigned char               nport,
    VIDEO_COMMON_PORT_IN_SEL    mux
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

     RTK_VIDEO_COMMON_INFO("lib_video_common_set_hdmi_dp_sel[%d] = %d\n", nport, mux);

    switch(nport)
    {
        case 0:
#ifdef COMMON_VIDEO_P0_hdmi_dp_select_mask
            rtd_maskl(COMMON_hdmi_dp_select_reg, ~(COMMON_VIDEO_P0_hdmi_dp_select_mask), COMMON_VIDEO_P0_hdmi_dp_select(mux));
#endif
            break;
        case 1:
#ifdef COMMON_VIDEO_P1_hdmi_dp_select_mask
            rtd_maskl(COMMON_hdmi_dp_select_reg, ~(COMMON_VIDEO_P1_hdmi_dp_select_mask), COMMON_VIDEO_P0_hdmi_dp_select(mux));
#endif
            break;
        case 2:
            break;
        case 3:
            break;
        default :
            break;
    }
}

/*---------HDMI_DP_COMMON Function--------*/

/*------------------------------------------------
 * Func : lib_video_common_set_color_space_input
 *
 * Desc : set color space input for video common
 *
 * Para : nport : video common port
 *        color_space : color space
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_color_space_input(
    unsigned char                   nport,
    VIDEO_COMMON_COLOR_DEPTH_INPUT  color_space
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

    //reset color space from video common
#ifdef COMMON_HDMIRX_YUV420_CTRL_0_yuv422_en_mask
    rtd_maskl(COMMON_HDMIRX_YUV420_CTRL_0_reg, ~(COMMON_HDMIRX_YUV420_CTRL_0_yuv422_en_mask), COMMON_HDMIRX_YUV420_CTRL_0_yuv422_en(0));
#endif

#ifdef COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask
    rtd_maskl(COMMON_HDMIRX_YUV420_CTRL_0_reg, ~(COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask), COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en(0));
#endif

    switch(color_space)
    {
        case COLOR_SPACE_YUV422:
            break;
        case COLOR_SPACE_YUV420:
#ifdef COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask
            rtd_maskl(COMMON_HDMIRX_YUV420_CTRL_0_reg, ~(COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask), COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en(1));
#endif
            break;
        case COLOR_SPACE_RGB:
        case COLOR_SPACE_YUV444:
        case COLOR_SPACE_YONLY:
        case COLOR_SPACE_UNKNOWN: 
        default:
            break;
    }    
}


/*------------------------------------------------
 * Func : lib_video_common_set_yuv422_repeat_mode
 *
 * Desc : set yuv422 repeat mode
 *
 * Para : nport : video common port
 *        enable : 1 : enable yuv422 repeat to yuv444 repeat mode
 *                 0 : bypass mode
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_yuv422_repeat_mode(
    unsigned char                   nport,
    unsigned char                   enable
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VIDEO_AND_OFFMS_PATH_SEL_no_422_444_repeat_mask
    rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_no_422_444_repeat_mask),
             COMMON_VIDEO_AND_OFFMS_PATH_SEL_no_422_444_repeat((enable)? 0:1));
#endif
}


/*------------------------------------------------
 * Func : lib_video_common_input_source_sel
 *
 * Desc : select video common data from which source
 *
 * Para : nport : video common port
 *        VIDEO_COMMON_SOURCE_TYPE : dp or hdmi type
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_input_source_sel(
    unsigned char               nport,
    VIDEO_COMMON_SOURCE_TYPE    src_type
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask
    switch(src_type)
    {
        case HDMI_2p0:
        case HDMI_2p1:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask),
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(SRC_IN_HDMIRX));
            break;
        case DP_1p4:
        case DP_2p0:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask),
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(SRC_IN_DPRX));
            break;
        case HDMI_2p0_DSCD:
        case HDMI_2p1_DSCD:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask),
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(SRC_IN_HDMI_DSCD));
            break;
        case DP_1p4_DSCD:
        case DP_2p0_DSCD:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel_mask),
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_yuv420_in_sel(SRC_IN_DPRX_DSCD));
            break;
        default:
            break;
    }
#endif
}


/*------------------------------------------------
 * Func : lib_video_common_dscd_in_sel
 *
 * Desc : select video common with which dscd module
 *
 * Para : nport : video common port
 *        VIDEO_COMMON_DSCD_IN_SEL : dscd port
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_dscd_in_sel(
    unsigned char               nport,
    VIDEO_COMMON_DSCD_IN_SEL    dscd_src
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }
#ifdef COMMON_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel_mask
    RTK_VIDEO_COMMON_INFO("%s, DSCD Sel = %d\n",__func__ ,dscd_src);
    switch(dscd_src)
    {
        case VIDEO_COMMON_DSCD_0:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel(0));
            break;
        case VIDEO_COMMON_DSCD_1:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_dscd_in_sel(1));
            break;
        default:
            break;
    }
#endif
}


/*------------------------------------------------
 * Func : lib_video_common_audio_in_sel
 *
 * Desc : select audio data from which source
 *
 * Para : nport : video common port
 *        VIDEO_COMMON_SOURCE_TYPE : dp or hdmi type
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_audio_in_sel(
    unsigned char               nport,
    VIDEO_COMMON_SOURCE_TYPE    src_type
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VIDEO_AND_OFFMS_PATH_SEL_audio_sel_mask
    switch(src_type)
    {
        case HDMI_2p0_DSCD:
        case HDMI_2p1_DSCD:
        case HDMI_2p0:
        case HDMI_2p1:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_audio_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_audio_sel(0));
            break;
        case DP_1p4:
        case DP_2p0:
        case DP_1p4_DSCD:
        case DP_2p0_DSCD:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_audio_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_audio_sel(1));
            break;
        default:
            break;
    }
#endif
}


/*------------------------------------------------
 * Func : lib_video_common_offms_out_sel
 *
 * Desc : select offms output sel
 *
 * Para : nport : video common port
 *        VIDEO_COMMON_SOURCE_TYPE : dp or hdmi type
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_offms_out_sel(
    unsigned char               nport,
    VIDEO_COMMON_SOURCE_TYPE    src_type
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask
    switch(src_type)
    {

        case HDMI_2p0:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(0));
            break;
        case HDMI_2p1:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(1));
            break;
        case DP_1p4:
        case DP_2p0:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(3));
            break;
        case HDMI_2p0_DSCD:
        case HDMI_2p1_DSCD:
        case DP_1p4_DSCD:
        case DP_2p0_DSCD:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_offms_out_sel(4));
            break;
        default:
            break;
    }
#endif
}


/*------------------------------------------------
 * Func : lib_video_common_set_dprx_pixel_mode_in_sel
 *
 * Desc : set dprx input pixel mode
 *
 * Para : nport : video common port
 *        VIDEO_COMMON_IN_PIXEL_MODE : 1 pixel and 2 pixel mode
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_dprx_pixel_mode_in_sel(
    unsigned char               nport,
    VIDEO_COMMON_IN_PIXEL_MODE  pixel_mode
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel_mask
    switch(pixel_mode)
    {
        case VIDEO_COMMON_1P_In:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel(1));
            break;
        case VIDEO_COMMON_2P_In:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel_mask), 
                     COMMON_VIDEO_AND_OFFMS_PATH_SEL_dprx_pixel_mode_sel(2));
            break;
        default :
            break;
    }

#endif
}

//--------------------------------------------------
// Func  : lib_video_common_set_output_pixel_mode
//
// Desc  : set YUV420 pixel mode. on RTD2885P, YUV420
//         is always running at 4 pixel mode. but DISP
//         path only supports 1p / 2p mode.
//
// Param : port : dprx port
//         pixel_mode : 0 : disable/ 1 : enable
//
// Retn  : N/A
//--------------------------------------------------
void lib_video_common_set_output_pixel_mode(
    unsigned char                nport,
    VIDEO_COMMON_IN_PIXEL_MODE   pixel_mode
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

    switch(pixel_mode)
    {
        case VIDEO_COMMON_1P_In:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask | COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel_mask),
                    COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(0));
            break;
        case VIDEO_COMMON_2P_In:
            rtd_maskl(COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg, ~(COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask | COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_out_sel_mask),
                    COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(1));
            break;
        default :
            break;
    }
}

/*------------------------------------------------
 * Func : lib_video_common_set_video_common_clk_sel
 *
 * Desc : set vidoe common clock select
 *
 * Para : nport : video common port
 *        enable : 0/1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_video_common_clk_sel(
    unsigned char               nport,
    unsigned char               enable
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VIDEO_REG_VIDEO_COMMON_0_CLK_SEL_mask
    switch(nport)
    {
        case 0:
            rtd_maskl(COMMON_CKL_SEL_reg, ~(COMMON_VIDEO_REG_VIDEO_COMMON_0_CLK_SEL_mask), 
                        COMMON_VIDEO_REG_VIDEO_COMMON_0_CLK_SEL(enable));
            break;
        case 1:
            rtd_maskl(COMMON_CKL_SEL_reg, ~(COMMON_VIDEO_REG_VIDEO_COMMON_1_CLK_SEL_mask), 
                        COMMON_VIDEO_REG_VIDEO_COMMON_1_CLK_SEL(enable));
            break;
#ifdef COMMON_VIDEO_REG_VIDEO_COMMON_2_CLK_SEL_mask
        case 2:
            rtd_maskl(COMMON_CKL_SEL_reg, ~(COMMON_VIDEO_REG_VIDEO_COMMON_2_CLK_SEL_mask), 
                        COMMON_VIDEO_REG_VIDEO_COMMON_2_CLK_SEL(enable));
            break;
#endif
#ifdef COMMON_VIDEO_REG_VIDEO_COMMON_3_CLK_SEL_mask
        case 3:
            rtd_maskl(COMMON_CKL_SEL_reg, ~(COMMON_VIDEO_REG_VIDEO_COMMON_3_CLK_SEL_mask), 
                        COMMON_VIDEO_REG_VIDEO_COMMON_3_CLK_SEL(enable));
            break;
#endif
        default :
            break;
    }

#endif
}


/*---------HDMI_DP_COMMON MISC Function--------*/

/*------------------------------------------------
 * Func : lib_video_common_bypass_mode_sel
 *
 * Desc : select offms output sel
 *
 * Para : COMMON_PORT_BYPASS_TO_HDMITX_E : bypass port
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_bypass_mode_sel(COMMON_PORT_BYPASS_TO_HDMITX_E bypass_port)
{
#ifdef COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel_mask
    rtd_maskl(COMMON_MISC_COMMON_PORT_SEL_reg, ~(COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel_mask), 
        COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel(bypass_port));
#endif
}


/*------------------------------------------------
 * Func : lib_video_common_offms_sel
 *
 * Desc : offms position sel
 *
 * Para : VIDEO_COMMON_PORT : Video Common Port
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_offms_sel(VIDEO_COMMON_PORT common_port)
{
#ifdef COMMON_MISC_COMMON_PORT_SEL_common_offms_sel_mask
    rtd_maskl(COMMON_MISC_COMMON_PORT_SEL_reg, ~(COMMON_MISC_COMMON_PORT_SEL_common_offms_sel_mask), 
            COMMON_MISC_COMMON_PORT_SEL_common_offms_sel(common_port));
#endif
}


/*------------------------------------------------
 * Func : lib_video_common_yuv422_raw_data_sel
 *
 * Desc : yuv422 raw data sel
 *
 * Para : COMMON_BYPASS_YUV422_RAW_SEL_E : Video Common Port
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_yuv422_raw_data_sel(COMMON_BYPASS_YUV422_RAW_SEL_E yuv422_raw)
{
#ifdef COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel_mask
    rtd_maskl(COMMON_MISC_COMMON_PORT_SEL_reg, ~(COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel_mask), 
            COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel(yuv422_raw));
#endif
}

/*------------------------------------------------
 * Func : lib_video_common_get_bypass_port
 *
 * Desc : get bypass port
 *
 * Para : N/A
 *
 * Retn : N/A
 *-----------------------------------------------*/
COMMON_PORT_BYPASS_TO_HDMITX_E lib_video_common_get_bypass_port(void)
{
    COMMON_PORT_BYPASS_TO_HDMITX_E res = PORT0_MAC_BYPASS_TO_HDMITX;
#ifdef COMMON_MISC_COMMON_PORT_SEL_bypass_422_raw_sel_mask
    unsigned char port;
    port = COMMON_MISC_COMMON_PORT_SEL_get_common_bypass_sel(rtd_inl(COMMON_MISC_COMMON_PORT_SEL_reg));

    switch(port)
    {
        case 0:
            res = PORT0_MAC_BYPASS_TO_HDMITX;
            break;
        case 1:
            res = PORT1_MAC_BYPASS_TO_HDMITX;
            break;
        case 2:
            res = PORT2_MAC_BYPASS_TO_HDMITX;
            break;
        case 3:
            res = PORT3_MAC_BYPASS_TO_HDMITX;
            break;
        case 4:
            res = PORT0_COMMON_BYPASS_TO_HDMITX;
            break;
        case 5:
            res = PORT1_COMMON_BYPASS_TO_HDMITX;
            break;
        case 6:
            res = PORT2_COMMON_BYPASS_TO_HDMITX;
            break;
        case 7 :
            res = PORT3_COMMON_BYPASS_TO_HDMITX;
            break;
        default :
            res = PORT0_MAC_BYPASS_TO_HDMITX;
            break;
    }

#endif
    return res;
}


/*------------------------------------------------
 * Func : lib_video_common_set_dscd_dp_source_clk
 *
 * Desc : set dscd dp source clk
 *
 * Para : nport : video common port
 *        VIDEO_COMMON_IN_PIXEL_MODE : 1 pixel and 2 pixel mode
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_dscd_dp_source_clk(
    unsigned char               nport,
    VIDEO_COMMON_DSCD_IN_SEL    dscd_sel,
    VIDEO_COMMON_IN_PIXEL_MODE  pixel_mode
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

    if(dscd_sel == VIDEO_COMMON_DSCD_0)
    {
        #ifdef COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel_mask
        switch(pixel_mode)
        {
        case VIDEO_COMMON_1P_In:
            
            rtd_maskl(COMMON_MISC_COMMON_PORT_SEL_reg, ~(COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel_mask), 
                     COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel(0));
            break;
        case VIDEO_COMMON_2P_In:
            rtd_maskl(COMMON_MISC_COMMON_PORT_SEL_reg, ~(COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel_mask), 
                     COMMON_MISC_COMMON_PORT_SEL_dscd_dprx_1p2p_sel(1));
            break;
        default :
            break;
        }
        #endif
    }
    else if(dscd_sel == VIDEO_COMMON_DSCD_1)
    {
        #ifdef COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel_mask
        switch(pixel_mode)
        {
        case VIDEO_COMMON_1P_In:
            
            rtd_maskl(COMMON_MISC_COMMON_PORT_SEL_reg, ~(COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel_mask), 
                     COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel(0));
            break;
        case VIDEO_COMMON_2P_In:
            rtd_maskl(COMMON_MISC_COMMON_PORT_SEL_reg, ~(COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel_mask), 
                     COMMON_MISC_COMMON_PORT_SEL_dscd2_dprx_1p2p_sel(1));
            break;
        default :
            break;
        }
        #endif
    }
}


/*------------------------------------------------
 * Func : lib_video_common_support_clk_gate
 *
 * Desc : using for non-continuous clk, (HDMI21 / DP20 or DSCD) 
 *
 * Para : nport : video common port
 *        enable : 0, 1
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_support_clk_gate(
    unsigned char               nport,
    unsigned char               enable
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

    rtd_maskl(COMMON_HDMIRX_YUV420_CTRL_1_reg, ~(COMMON_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en_mask), 
                         COMMON_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en(enable));
}
/*------------------------------------------------
 * Func : lib_video_common_set_hs_inverse
 *
 * Desc : set horizon sync inverse
 *
 * Para : nport : video common port
 *        enable : 1: inverse
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_hs_inverse(
    unsigned char nport,
    unsigned char enable
    ) 
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VS_HS_INV
    rtd_maskl(COMMON_VS_HS_INV, ~(COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_VS_INV_mask),
                         COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_VS_INV(enable));
#endif
}

/*------------------------------------------------
 * Func : lib_video_common_set_vs_inverse
 *
 * Desc : set vertical sync inverse
 *
 * Para : nport : video common port
 *        enable : 1: inverse
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_vs_inverse(
    unsigned char nport,
    unsigned char enable
    ) 
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }
#ifdef COMMON_VS_HS_INV
    rtd_maskl(COMMON_VS_HS_INV, ~(COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_HS_INV_mask),
                         COMMON_VIDEO_REG_VIDEO_COMMON_0_COMMON_HS_INV(enable));
#endif
}

/*------------------------------------------------
 * Func : lib_video_common_set_line_drop
 *
 * Desc : set port line drop
 *
 * Para : nport : video common port
 *        enable : 1/0 
 *
 * Retn : N/A
 *-----------------------------------------------*/
void lib_video_common_set_line_drop(
    unsigned char               nport,
    unsigned char               enable
    )
{
    if(nport >= COMMON_PORT_NUM)
    {
        RTK_VIDEO_COMMON_ERR("%s, Failed to get hdmi dp common = %d (Max Common = %d)\n",__func__ ,nport, COMMON_PORT_NUM);
        return;
    }

#ifdef COMMON_VIDEO_REG_VIDEO_COMMON_0_LINE_DROP_EN_mask
    rtd_maskl(COMMON_LINE_DROP_CTRL_0_reg, ~(COMMON_VIDEO_REG_VIDEO_COMMON_0_LINE_DROP_EN_mask), 
            COMMON_VIDEO_REG_VIDEO_COMMON_0_LINE_DROP_EN(enable));
#endif
}

MODULE_LICENSE("GPL");
