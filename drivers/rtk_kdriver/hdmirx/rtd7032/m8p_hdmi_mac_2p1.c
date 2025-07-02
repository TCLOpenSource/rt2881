/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file:    hdmi_mac.c
 *
 * author:  kevin_wang@realtek.com
 * date:
 * version:     3.0
 *
 *============================================================*/

/*========================Header Files============================*/
#include "hdmi_common.h"

#ifndef BUILD_QUICK_SHOW
    #include <linux/kernel.h>
    #include <linux/delay.h>
    #include <linux/slab.h>
    #include <linux/string.h>
#else
    #include "string.h"
#endif
#include <mach/platform.h>
#include "m8p_hdmi_reg.h"
#include "m8p_hdmi_mac_2p1.h"
#include "hdmi_vfe_config.h"
#include "hdmi_dsc.h"
#include "m8p_hdmi_reg_mac.h"
#include <rbus/M8P_hdmi21_p0_meas_reg.h>

void m8p_lib_hdmi_hd21_mac_crt_perport_off(unsigned char nport)
{
    //Alwasy use physical port 0 address
    unsigned int clken_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre0_mask << nport;
    unsigned int clken_hdcp_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre0_mask << nport;
    unsigned int rstn_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre0_mask << nport;
    unsigned int rstn_hdcp_pre_addr = M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre0_mask << nport;

    if (!lib_hdmi_is_hdmi_21_available(nport)) {
        return;
    }

    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_clken3_reg, ~(clken_pre_mask | clken_hdcp_pre_mask), 0x0);
    udelay(5);
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_rst3_reg, ~(rstn_pre_mask | rstn_hdcp_pre_addr), 0x0);
}

void m8p_lib_hdmi_hd21_mac_crt_perport_on(unsigned char nport)
{
    //Alwasy use physical port 0 address
    unsigned int clken_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre0_mask << nport;
    unsigned int clken_hdcp_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre0_mask << nport;
    unsigned int rstn_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre0_mask << nport;
    unsigned int rstn_hdcp_pre_addr = M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre0_mask << nport;

    if (!lib_hdmi_is_hdmi_21_available(nport)) {
        return;
    }

    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_rst3_reg, ~(rstn_pre_mask | rstn_hdcp_pre_addr), (clken_pre_mask | clken_hdcp_pre_mask));
    udelay(5);
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_clken3_reg, ~(clken_pre_mask | clken_hdcp_pre_mask), (clken_pre_mask | clken_hdcp_pre_mask));
}


void m8p_lib_hdmi_hd21_mac_crt_common_reset(unsigned char nport)
{
    UINT32 hd21_common_clk_mask[HDMI_PORT_TOTAL_NUM] = {
        M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_common0_mask,
        M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_common1_mask,
        M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_common2_mask,
        M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_common3_mask
    };
    UINT32 hd21_common_rst_mask[HDMI_PORT_TOTAL_NUM] = {
        M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_common0_mask,
        M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_common1_mask,
        M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_common2_mask,
        M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_common3_mask
    };

    HDMI_INFO("[CRT]lib_hdmi_hd21_mac_crt_common_reset, nport=%d\n", nport);

    if (!lib_hdmi_is_hdmi_21_available(nport)) {
        return ;
    }
    // clk en disable
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_clken3_reg, ~(hd21_common_clk_mask[nport]), 0x0);
    udelay(5);
    // reset 0
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_rst3_reg, ~(hd21_common_rst_mask[nport]), 0x0);
    udelay(5);
    // reset 1
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_rst3_reg, ~(hd21_common_rst_mask[nport]), hd21_common_rst_mask[nport]);
    udelay(5);
    // clk en enable
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_clken3_reg, ~(hd21_common_clk_mask[nport]), hd21_common_clk_mask[nport]);

}

void m8p_lib_hdmi_hd21_crt_off(void)
{
    //Alwasy use physical port 0 address
    // clk en disable
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_clken3_reg, ~(M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_common0_mask |
                                                     M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre1_mask |
                                                     M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre1_mask), 0x0);
    udelay(5);

    // reset 0
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_rst3_reg, ~(M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_common0_mask |
                                                   M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre1_mask |
                                                   M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre1_mask), 0x0);
}

void m8p_lib_hdmi_hd21_crt_on(void)
{
    //Alwasy use physical port 0 address
    // reset 1
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_rst3_reg, ~(M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_common0_mask |
                                                   M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre1_mask |
                                                   M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre1_mask),
              (M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_common0_mask |
               M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_pre1_mask |
               M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst3_rstn_hd21_hdcp_pre1_mask));
    udelay(5);
    // clk en enable
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmi_clken3_reg, ~(M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_common0_mask |
                                                     M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre1_mask |
                                                     M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre1_mask),
              (M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_common0_mask |
               M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_pre1_mask |
               M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken3_clken_hd21_hdcp_pre1_mask));
}

void m8p_lib_hdmi_hd21_timing_gen_en_vs_align(unsigned char nport, unsigned int delay_ms)
{
    // RL6951-237 : tg en check time to enable
    HDMI_INFO("timing gen vs align, %d ms in one frame, setting=%08x\n", delay_ms, hdmi_in(M8P_HD21_TIMING_GEN_CR_reg));

    if (delay_ms < HD21_TG_VS_ALIGN_THD_MS) {
        // 48~144hz need
        hdmi_mask(M8P_HD21_TIMING_GEN_CR_reg, ~M8P_HD21_TIMING_GEN_CR_tg_en_align_sel_mask, M8P_HD21_TIMING_GEN_CR_tg_en_align_sel_mask);
        mdelay(delay_ms);
    }
    else {
        // 24~47hz
        hdmi_mask(M8P_HD21_TIMING_GEN_CR_reg, ~M8P_HD21_TIMING_GEN_CR_tg_en_align_sel_mask, 0);
    }
}

void m8p_lib_hdmi_hd21_clk_div_sel(unsigned char nport, unsigned char pixel_mode)
{
    unsigned int addr = M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg + nport * 0x1000;

    switch (pixel_mode) {
        case 1: // 1 pixel mode
            hdmi_mask(HD21_CR_reg, ~HD21_CR_clk_div_sel_mask, HD21_CR_clk_div_sel(3));
            hdmi_mask(addr, ~(M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask),
                      M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(1));
            break;

        case 2: // 2 pixel mode
            hdmi_mask(HD21_CR_reg, ~HD21_CR_clk_div_sel_mask, HD21_CR_clk_div_sel(1));
            hdmi_mask(addr, ~(M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask),
                      M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(1));
            break;
        case 4: // 4 pixel mode for 4k240
            hdmi_mask(HD21_CR_reg, ~HD21_CR_clk_div_sel_mask, HD21_CR_clk_div_sel(0));
            hdmi_mask(addr, ~(M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel_mask),
                      M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_pixel_transform_mode_sel(1));
            break;
        default:
            HDMI_ERR("[lib_hdmi_hd21_set_clk_div_sel port:%d] invalid pixel mode =%d\n", nport, pixel_mode);
            break;
    }
    HDMI_INFO("[lib_hdmi_hd21_set_clk_div_sel nport=%d] pixel_mode=%d \n", nport, pixel_mode);
}



//------------------------------------------------------------------
// Error Status
//------------------------------------------------------------------


// 0: 4 frame pop  1: 1 frame pop
void m8p_lib_hdmi_hd21_meas_set_frame_pop_mode(unsigned char nport, unsigned char mode)
{
    if (!lib_hdmi_is_hdmi_21_available(nport)) {
        return;
    }

    hdmi_mask(M8P_HD21_MEAS_FRL_MEASURE_A_reg, ~M8P_HD21_MEAS_FRL_MEASURE_A_frame_pop_mode_mask, M8P_HD21_MEAS_FRL_MEASURE_A_frame_pop_mode(mode ? 1 : 0));
}

// 0: 4 frame pop  1: 1 frame pop
unsigned char m8p_lib_hdmi_hd21_meas_get_frame_pop_mode(unsigned char nport)
{
    if (!lib_hdmi_is_hdmi_21_available(nport)) {
        return 0;
    }

    return M8P_HD21_MEAS_FRL_MEASURE_A_get_frame_pop_mode(hdmi_in(M8P_HD21_MEAS_FRL_MEASURE_A_reg));
}

void m8p_lib_hdmi_hd21_set_polarity_meas_mode(unsigned char nport, HD21_MEASURE_POLARITY_MEAS_MODE mode)
{
    hdmi_mask(M8P_HD21_MEAS_FRL_MEASURE_9_reg, ~(M8P_HDMI21_P0_MEAS_FRL_MEASURE_9_polarity_meas_mask), M8P_HDMI21_P0_MEAS_FRL_MEASURE_9_polarity_meas(mode));
}

HD21_MEASURE_POLARITY_MEAS_MODE lib_hdmi_hd21_get_polarity_meas_mode(unsigned char nport)
{
    return M8P_HDMI21_P0_MEAS_FRL_MEASURE_9_get_polarity_meas(hdmi_in(M8P_HD21_MEAS_FRL_MEASURE_9_reg));
}

unsigned char m8p_newbase_hdmi_hd21_get_yuv420_in_sel(void)
{
    return M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_get_yuv420_in_sel(hdmi_in(M8P_HDMI_DP_COMMON_VIDEO_AND_OFFMS_PATH_SEL_reg));
}


void m8p_lib_hdmi_hd21_vdrop_set(unsigned char nport, unsigned char enable, unsigned int htotal)
{
    if(enable){
        hdmi_rx[nport].timing_t.is_v_drop = 1;
        hdmi_mask(M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_reg, ~(M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_line_drop_valid_delay_mask),
                    M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_line_drop_valid_delay(htotal/4/2));
        hdmi_mask(M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_reg, ~(M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_line_drop_en_mask), M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_line_drop_en_mask);
        HDMI_EMG("[Port %d]set line drop enable, line drop valid delay %d\n", nport, M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_get_line_drop_valid_delay(hdmi_in(M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_reg)));
    }else{
        hdmi_mask(M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_reg, ~(M8P_HDMI_DP_COMMON_LINE_DROP_CTRL_0_line_drop_en_mask), 0);
    }
}

