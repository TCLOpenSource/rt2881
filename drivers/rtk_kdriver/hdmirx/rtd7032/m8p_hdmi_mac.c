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


#include <rbus/sys_reg_reg.h>
#include "hdmi_dsc.h"
#include <mach/rtk_platform.h>
#include "hdmi_vfe_config.h"
#include "hdmi_mac.h"
#include "m8p_hdmi_mac.h"
#include <rbus/M8P_sys_reg_reg.h>

extern HDMI_PORT_INFO_T hdmi_rx[HDMI_PORT_TOTAL_NUM];



const VIDEO_DPLL_RANGE_T video_dprxpll_others_param[] = {
    {23, 31, 0b000, 0b101},
    {32, 40, 0b000, 0b101},
    {41, 49, 0b000, 0b101},
    {50, 57, 0b000, 0b101},
};
const UINT8 video_dprxpll_others_table_size = sizeof(video_dprxpll_others_param) / sizeof(VIDEO_DPLL_RANGE_T);


//------------------------------------------------------------------
// CRT
//------------------------------------------------------------------

void m8p_lib_hdmi_mac_crt_perport_off(unsigned char nport)
{
    unsigned int clken_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre0_mask;
    unsigned int clken_hdcp_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre0_mask;
    unsigned int rstn_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre0_mask;
    unsigned int rstn_hdcp_pre_addr = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre0_mask;

    hdmi_mask(HDMI_hdmi_clken0_reg, ~(clken_pre_mask | clken_hdcp_pre_mask), 0x0);
    hdmi_out(HDMI_SOFT_1_reg, 0x0);     // AES clock done
    udelay(5);
    hdmi_mask(HDMI_hdmi_rst0_reg, ~(rstn_pre_mask | rstn_hdcp_pre_addr), 0x0);
    hdmi_out(HDMI_SOFT_0_reg, 0x0);     // AES reset on
}

void m8p_lib_hdmi_mac_crt_perport_on(unsigned char nport)
{
    // reset HDMI preport HW : including HDCP/SCDC.....

    // RL6583-524 : reset HDMI pre : H5 has only one HDMI MAC, so the port number is always 0
    // this function should be called to reset HDMI mac, every time cable off/on

    unsigned int clken_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre0_mask << nport;
    unsigned int clken_hdcp_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre0_mask << nport;
    unsigned int rstn_pre_mask = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre0_mask << nport;
    unsigned int rstn_hdcp_pre_addr = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre0_mask << nport;

    hdmi_mask(HDMI_hdmi_clken0_reg, ~(clken_pre_mask | clken_hdcp_pre_mask), 0); // Clock off
    hdmi_out(HDMI_SOFT_1_reg, 0x0);     // AES clock done
    udelay(5);

    hdmi_mask(HDMI_hdmi_rst0_reg, ~(rstn_pre_mask | rstn_hdcp_pre_addr), 0); // Rst Low
    hdmi_out(HDMI_SOFT_0_reg, 0x0);     // AES reset on
    udelay(5);

    hdmi_mask(HDMI_hdmi_clken0_reg, ~(clken_pre_mask | clken_hdcp_pre_mask), (clken_pre_mask | clken_hdcp_pre_mask)); // Clock on
    hdmi_out(HDMI_SOFT_1_reg, 0x1);     // AES clock on
    udelay(5);

    hdmi_mask(HDMI_hdmi_clken0_reg, ~(clken_pre_mask | clken_hdcp_pre_mask), 0); // Clock off
    hdmi_out(HDMI_SOFT_1_reg, 0x0);     // AES clock off
    udelay(5);

    hdmi_mask(HDMI_hdmi_rst0_reg, ~(rstn_pre_mask | rstn_hdcp_pre_addr), (rstn_pre_mask | rstn_hdcp_pre_addr)); // Rst = 1 (release)
    hdmi_out(HDMI_SOFT_0_reg, 0x1);     // AES reset off
    udelay(5);

    hdmi_mask(HDMI_hdmi_clken0_reg, ~(clken_pre_mask | clken_hdcp_pre_mask), (clken_pre_mask | clken_hdcp_pre_mask)); // Clock on
    hdmi_out(HDMI_SOFT_1_reg, 0x1);     // AES clock on
    udelay(5);
}




void m8p_lib_hdmi_crt_off(unsigned char nport)
{
    unsigned int val = 0;

    switch (nport) {
        case 0:
            val = HDMI_STB_clken_hdmi_stb_port0_mask;
            break;
        case 1:
            val = HDMI_STB_clken_hdmi_stb_port1_mask;
            break;
        case 2:
            val = HDMI_STB_clken_hdmi_stb_port2_mask;
            break;
        case 3:
            val = HDMI_STB_clken_hdmi_stb_port3_mask;
            break;
        default:
            return;
    }
    // Disable for HDMI STB MAC
    hdmi_mask(HDMI_STB_clken_hdmi_stb_reg, ~val, 0x0);
    udelay(5);

    switch (nport) {
        case 0:
            val = HDMI_STB_rst_n_hdmi_stb_port0_mask;
            break;
        case 1:
            val = HDMI_STB_rst_n_hdmi_stb_port1_mask;
            break;
        case 2:
            val = HDMI_STB_rst_n_hdmi_stb_port2_mask;
            break;
        case 3:
            val = HDMI_STB_rst_n_hdmi_stb_port3_mask;
            break;
        default:
            return;
    }
    hdmi_mask(HDMI_STB_rst_n_hdmi_stb_reg, ~val, 0x0);
    udelay(5);

    switch (nport) {
        case 0:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre0_mask;
            break;
        case 1:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre1_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre1_mask;
            break;
        case 2:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre2_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre2_mask;
            break;
        case 3:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre3_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre3_mask;
            break;
        default:
            return;
    }
    hdmi_mask(HDMI_hdmi_clken0_reg, ~val, 0x0);
    udelay(5);

    switch (nport) {
        case 0:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre0_mask;
            break;
        case 1:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre1_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre1_mask;
            break;
        case 2:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre2_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre2_mask;
            break;
        case 3:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre3_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre3_mask;
            break;
        default:
            return;
    }
    hdmi_mask(HDMI_hdmi_rst0_reg, ~val, 0x0);
    udelay(5);
}

void m8p_lib_hdmi_crt_on(unsigned char nport)
{
    unsigned int val = 0;

    switch (nport) {
        case 0:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre0_mask;
            break;
        case 1:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre1_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre1_mask;
            break;
        case 2:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre2_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre2_mask;
            break;
        case 3:
            val = M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_pre3_mask | M8P_HDMI_TOP_COMMON_hdmi_rst0_rstn_hdmi_hdcp_pre3_mask;
            break;
        default:
            return;
    }
    hdmi_mask(HDMI_hdmi_rst0_reg, ~val, val);
    udelay(5);

    switch (nport) {
        case 0:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre0_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre0_mask;
            break;
        case 1:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre1_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre1_mask;
            break;
        case 2:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre2_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre2_mask;
            break;
        case 3:
            val = M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_pre3_mask | M8P_HDMI_TOP_COMMON_hdmi_clken0_clken_hdmi_hdcp_pre3_mask;
            break;
        default:
            return;
    }
    hdmi_mask(HDMI_hdmi_clken0_reg, ~val, val);

    udelay(5);

    switch (nport) {
        case 0:
            val = HDMI_STB_rst_n_hdmi_stb_port0_mask;
            break;
        case 1:
            val = HDMI_STB_rst_n_hdmi_stb_port1_mask;
            break;
        case 2:
            val = HDMI_STB_rst_n_hdmi_stb_port2_mask;
            break;
        case 3:
            val = HDMI_STB_rst_n_hdmi_stb_port3_mask;
            break;
        default:
            return;
    }

    // reset for HDMI STB MAC
    hdmi_mask(HDMI_STB_rst_n_hdmi_stb_reg, ~val, val);
    udelay(5);

    switch (nport) {
        case 0:
            val = HDMI_STB_clken_hdmi_stb_port0_mask;
            break;
        case 1:
            val = HDMI_STB_clken_hdmi_stb_port1_mask;
            break;
        case 2:
            val = HDMI_STB_clken_hdmi_stb_port2_mask;
            break;
        case 3:
            val = HDMI_STB_clken_hdmi_stb_port3_mask;
            break;
        default:
            return;
    }

    hdmi_mask(HDMI_STB_clken_hdmi_stb_reg, ~val, val);
}


void m8p_lib_hdmi_crt_reset_hdmi_common_by_port(unsigned char nport)
{
    UINT32 hdmi_common_clk_mask[HDMI_PORT_TOTAL_NUM] = {
        M8P_HDMI_hdmi_clken0_clken_hdmi_common_mask,
        M8P_HDMI_hdmi_clken0_clken_hdmi_common1_mask,
        M8P_HDMI_hdmi_clken0_clken_hdmi_common2_mask,
        M8P_HDMI_hdmi_clken0_clken_hdmi_common3_mask
    };

    UINT32 hdmi_common_rst_mask[HDMI_PORT_TOTAL_NUM] = {
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common_mask,
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common1_mask,
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common2_mask,
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common3_mask
    };

    if (nport >= HDMI_PORT_TOTAL_NUM) {
        return;
    }

    // reset HDMI2.0 MAC common : including DEEP Color, YUV420, CRC, MD_Detect...
    HDMI_INFO("[CRT][p%d] lib_hdmi_crt_reset_hdmi_common_by_port\n", nport);

    // Clock = 0
    hdmi_mask(M8P_HDMI_hdmi_clken0_reg, ~hdmi_common_clk_mask[nport], 0x0);
    udelay(5);

    // RST = 0
    hdmi_mask(M8P_HDMI_hdmi_rst0_reg, ~hdmi_common_rst_mask[nport], 0x0);
    udelay(5);

    // Clock = 1 : start clock to make sure HW can detect reset=Low
    hdmi_mask(M8P_HDMI_hdmi_clken0_reg, ~hdmi_common_clk_mask[nport], hdmi_common_clk_mask[nport]);
    udelay(5);

    // Clock = 0 : stop clock (need to stop clock before release reset)
    hdmi_mask(M8P_HDMI_hdmi_clken0_reg, ~hdmi_common_clk_mask[nport], 0x0);
    udelay(5);

    // RST = 1   : release reset
    hdmi_mask(M8P_HDMI_hdmi_rst0_reg, ~hdmi_common_rst_mask[nport], hdmi_common_rst_mask[nport]);
    udelay(5);

    // Clock = 1 : enable clock
    hdmi_mask(M8P_HDMI_hdmi_clken0_reg, ~hdmi_common_clk_mask[nport], hdmi_common_clk_mask[nport]);
    udelay(5);
}

void m8p_lib_hdmi_crt_off_hdmi_common_by_port(unsigned char nport)
{
    UINT32 hdmi_common_clk_mask[HDMI_PORT_TOTAL_NUM] = {
        M8P_HDMI_hdmi_clken0_clken_hdmi_common_mask,
        M8P_HDMI_hdmi_clken0_clken_hdmi_common1_mask,
        M8P_HDMI_hdmi_clken0_clken_hdmi_common2_mask,
        M8P_HDMI_hdmi_clken0_clken_hdmi_common3_mask
    };

    UINT32 hdmi_common_rst_mask[HDMI_PORT_TOTAL_NUM] = {
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common_mask,
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common1_mask,
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common2_mask,
        M8P_HDMI_hdmi_rst0_rstn_hdmi_common3_mask
    };

    if (nport >= HDMI_PORT_TOTAL_NUM) {
        return;
    }

    // reset HDMI2.0 MAC common : including DEEP Color, YUV420, CRC, MD_Detect...
    HDMI_INFO("[CRT][p%d] lib_hdmi_crt_off_hdmi_common\n", nport);

    // Clock = 0
    hdmi_mask(M8P_HDMI_hdmi_clken0_reg, ~hdmi_common_clk_mask[nport], 0x0);
    udelay(5);

    // RST = 0
    hdmi_mask(M8P_HDMI_hdmi_rst0_reg, ~hdmi_common_rst_mask[nport], 0x0);
    udelay(5);
}

unsigned char m8p_lib_hdmi_crt_is_hdmi_common_enabled_by_port(unsigned char nport)
{
    if (nport >= HDMI_PORT_TOTAL_NUM) {
        return 0;
    }

    if ((nport == 0) && M8P_HDMI_hdmi_clken0_get_clken_hdmi_common(hdmi_in(M8P_HDMI_hdmi_clken0_reg)) &&
            M8P_HDMI_hdmi_rst0_get_rstn_hdmi_common(hdmi_in(M8P_HDMI_hdmi_rst0_reg))) {
        return 1;
    }
    else if ((nport == 1) && M8P_HDMI_hdmi_clken0_get_clken_hdmi_common1(hdmi_in(M8P_HDMI_hdmi_clken0_reg)) &&
             M8P_HDMI_hdmi_rst0_get_rstn_hdmi_common1(hdmi_in(M8P_HDMI_hdmi_rst0_reg))) {
        return 1;
    }
    else if ((nport == 2) && M8P_HDMI_hdmi_clken0_get_clken_hdmi_common2(hdmi_in(M8P_HDMI_hdmi_clken0_reg)) &&
             M8P_HDMI_hdmi_rst0_get_rstn_hdmi_common2(hdmi_in(M8P_HDMI_hdmi_rst0_reg))) {
        return 1;
    }
    else if ((nport == 3) && M8P_HDMI_hdmi_clken0_get_clken_hdmi_common3(hdmi_in(M8P_HDMI_hdmi_clken0_reg)) &&
             M8P_HDMI_hdmi_rst0_get_rstn_hdmi_common3(hdmi_in(M8P_HDMI_hdmi_rst0_reg))) {
        return 1;
    }

    return 0;
}


#define HDMI_SRAM_LS_MASK  (HDMI_LIGHT_SLEEP_linebuf_ls2_mask | \
                            HDMI_LIGHT_SLEEP_linebuf_ls1_mask | \
                            HDMI_LIGHT_SLEEP_linebuf_ls0_mask | \
                            HDMI_LIGHT_SLEEP_hdr_ls_1_mask | \
                            HDMI_LIGHT_SLEEP_hdr_ls_0_mask | \
                            HDMI_LIGHT_SLEEP_audio_ls_mask)



void m8p_lib_hdmi_stb_wakeup_by_tmds_init(unsigned char nport, unsigned char en)
{
    if (en) {
        hdmi_out(HDMI_STB_GDI_TMDSCLK_SETTING_00_reg, 0x10021); //Set TMDS 32 count (33-1)
        hdmi_out(HDMI_STB_GDI_TMDSCLK_SETTING_01_reg, 0x8cd0049); //Set XTAL Max./Min. range(2253~73)+/-10%
        hdmi_mask(HDMI_STB_CLKDETSR_reg, ~(_BIT1), _BIT1); //[1]clk_in_target IRQ Enable

        //hdmi_mask(HDMI_STB_CLKDETSR_reg, ~(_BIT6), 0); //[6]Clock detect disable
        //hdmi_mask(HDMI_STB_CLKDETSR_reg, ~(_BIT6), _BIT6); //[6]Clock detect enable
    }
    else {
        hdmi_mask(HDMI_STB_CLKDETSR_reg, ~(_BIT1), 0); //[1]clk_in_target IRQ disable
        //hdmi_mask(HDMI_STB_CLKDETSR_reg, ~(_BIT6), 0); //[6]Clock detect disable
    }

}





//------------------------------------------------------------------
// Async Phy FIFO control
//------------------------------------------------------------------

void m8p_lib_hdmi_mac_afifo_enable(unsigned char nport, unsigned char lane_mode)
{
    //ex mac
    if (nport == HDMI_PORT3) {
        hdmi_mask(HDMI_EX_P3_EX_PHY_FIFO_CR0_reg,
                  ~(HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_b_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_b_flush_mask |
                    HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_g_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_g_flush_mask |
                    HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_r_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_r_flush_mask),
                  (HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_b_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_b_flush_mask |
                   HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_g_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_g_flush_mask |
                   HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_r_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR0_port0_r_flush_mask));
        if (lane_mode == HDMI_4LANE)
            hdmi_mask(HDMI_EX_P3_EX_PHY_FIFO_CR2_reg,
                      ~(HDMI_EX_P3_EX_PHY_FIFO_CR2_port0_c_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR2_port0_c_flush_mask),
                      (HDMI_EX_P3_EX_PHY_FIFO_CR2_port0_c_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR2_port0_c_flush_mask));
        else {
            hdmi_mask(HDMI_EX_P3_EX_PHY_FIFO_CR2_reg, ~(HDMI_EX_P3_EX_PHY_FIFO_CR2_port0_c_afifo_en_mask | HDMI_EX_P3_EX_PHY_FIFO_CR2_port0_c_flush_mask), 0x0);
        }

    }

    //big mac
    hdmi_mask(HDMI_PHY_FIFO_CR0_reg,
              ~((HDMI_PHY_FIFO_CR0_port0_b_afifo_en_mask | HDMI_PHY_FIFO_CR0_port0_b_flush_mask |
                 HDMI_PHY_FIFO_CR0_port0_g_afifo_en_mask | HDMI_PHY_FIFO_CR0_port0_g_flush_mask |
                 HDMI_PHY_FIFO_CR0_port0_r_afifo_en_mask | HDMI_PHY_FIFO_CR0_port0_r_flush_mask) << (nport * 7)),
              ((HDMI_PHY_FIFO_CR0_port0_b_afifo_en_mask | HDMI_PHY_FIFO_CR0_port0_b_flush_mask |
                HDMI_PHY_FIFO_CR0_port0_g_afifo_en_mask | HDMI_PHY_FIFO_CR0_port0_g_flush_mask |
                HDMI_PHY_FIFO_CR0_port0_r_afifo_en_mask | HDMI_PHY_FIFO_CR0_port0_r_flush_mask) << (nport * 7)));
}


//------------------------------------------------------------------
// YUV 420 ti 444
//------------------------------------------------------------------

void m8p_lib_hdmi_420_en(unsigned char nport, unsigned char en)
{
    unsigned int addr = M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_0_reg + nport * 0x1000;

    if (!newbase_hdmi_is_dispsrc(SLR_MAIN_DISPLAY) && !newbase_hdmi_get_common_420_selection()) {
        return;
    }

    HDMI_INFO("[lib_hdmi_420_en nport:%d], en=%d\n", nport, en);
    //hdmi_mask(HDMI_YUV420_CR_reg, ~HDMI_YUV420_CR_en_mask, HDMI_YUV420_CR_en(en?1:0));//not use
    hdmi_mask(addr, ~M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en_mask, M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_0_yuv420_cr_en(en ? 1 : 0));
}


void m8p_lib_hdmi_set_420_vactive(unsigned char nport, unsigned int vactive)
{
    unsigned int addr = M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_reg + nport * 0x1000;

    //hdmi_mask(HDMI_YUV420_CR1_reg, ~HDMI_YUV420_CR1_mode_mask, HDMI_YUV420_CR1_mode_mask);// no use
    //hdmi_mask(HDMI_YUV420_CR1_reg, ~HDMI_YUV420_CR1_vactive_mask, HDMI_YUV420_CR1_vactive(vactive));// no use
    hdmi_mask(addr, ~M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_cr1_mode_mask, M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_cr1_mode_mask);
    hdmi_mask(addr, ~M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_cr1_vactive_mask, M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_cr1_vactive(vactive));
    HDMI_INFO("lib_hdmi_set_420_vactive, nport =%d, vactive=%d\n", nport, vactive);
}

void m8p_lib_hdmi_420_fva_ds_mode(unsigned char nport, unsigned char en)
{
    unsigned int addr = M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_reg + nport * 0x1000;

    HDMI_INFO("[lib_hdmi_420_fva_ds_mode nport:%d], en=%d\n", nport, en);
    //hdmi_mask(HDMI_YUV420_CR1_reg, ~HDMI_YUV420_CR1_fva_ds_mode_mask, HDMI_YUV420_CR1_fva_ds_mode(en?1:0));//no use
    hdmi_mask(addr, ~M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_cr1_fva_ds_mode_mask, M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_cr1_fva_ds_mode(en ? 1 : 0));
}

unsigned int m8p_lib_hdmi_get_420_fva_ds_mode(unsigned char nport)
{
    unsigned int addr = M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_reg + nport * 0x1000;
    //return HDMI_YUV420_CR1_get_fva_ds_mode(hdmi_in(HDMI_YUV420_CR1_reg));// no use
    return M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_get_yuv420_cr1_fva_ds_mode(hdmi_in(addr));
}


//------------------------------------------------------------------
// Video PLL Register (shared by all ports)
//------------------------------------------------------------------
unsigned char m8p_lib_hdmi_set_video_dprxpll(
            unsigned char           nport,
            unsigned int            b_clk,
            unsigned char           frl_mode,
            unsigned char           cd,
            unsigned char           is_interlace,
            unsigned char           is_420,
            unsigned long          *pll_pixelclockx1024
)
{
    unsigned char enable_2x = 0;
    unsigned long fvco = 0;
    unsigned char large_ratio = 0, m = 0, n = 0, o = 3;
    unsigned long pixel_clock;
    unsigned char i = 0;
    unsigned char dpll_rs = 0, dpll_ip = 0, dpll_sel_ipi = 0;
    unsigned long pll_temp_data = 0;
    unsigned int fvco_tolarence = 0;
    unsigned int addr = M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_reg + nport * 0x1000;

    //HDMI_INFO("lib_hdmi_set_video_pll, port=%d, b_clk=%d, cd=%d, interlace=%d, is_420=%d, ver=%d\n",
    //    nport, b_clk, cd, is_interlace, is_420, get_ic_version());
    //tmds clock = b * 27 / 256
    //tmds clock = pixel clock * SN/SM --> b* 27 / 256 = pixel clock * SN/SM
    //pixel clock = b * 27 * M / (SN * 256)
    //pixel clock * 1024 = b * 27 * M * 1024 / (N * 256)

#define PLL_FPFD_LEVEL    1350UL //Mhz
#define PLL_FVCO_MIN    32400UL
#define PLL_FVCO_MAX    76950UL
#define PLL_M_NUM    ((PLL_FVCO_MAX -PLL_FVCO_MIN) /PLL_FPFD_LEVEL)+1    // 34    //
#define PLL_OUTDIV_NUM    4

#define PLL_M_MIN    22

    lib_hdmi_video_pll_init(nport);
    // 1. HW init
    hdmi_mask(HDMI_MN_SCLKG_CTRL_reg, ~HDMI_MN_SCLKG_CTRL0_sclkg_dbuf_mask, HDMI_MN_SCLKG_CTRL0_sclkg_dbuf_mask);       //video PLL double buffer load //    rtd_outl(0xb8040410, 0x00000010);
    hdmi_mask(HDMI_HDMI_VPLLCR1_reg, ~(HDMI_HDMI_VPLLCR1_dpll_rstb_mask), HDMI_HDMI_VPLLCR1_dpll_rstb_mask);//    rtd_outl(0xb8040404, 0x00203854);
    hdmi_mask(HDMI_HDMI_VPLLCR1_reg, ~(HDMI_HDMI_VPLLCR1_dpll_rstb_mask), 0); //    rtd_outl(0xb8040404, 0x00203844);
    hdmi_mask(HDMI_HDMI_VPLLCR1_reg, ~(HDMI_HDMI_VPLLCR1_dpll_rstb_mask), HDMI_HDMI_VPLLCR1_dpll_rstb_mask); //    rtd_outl(0xb8040404, 0x00203854);
    hdmi_out(HDMI_HDMI_VPLLCR1_reg, 0x00203854);

    // 2. Pixel clock calculation
    if (frl_mode == MODE_TMDS) {
        pixel_clock = ((unsigned long)b_clk * 27UL * dpll_ratio[cd].SM * 100UL) / (dpll_ratio[cd].SN * 256UL); // 594M => 59400
        //pixel_clock = ((unsigned long)b_clk * 27UL  * 100UL) / ( 256UL); // 594M => 59400

        if (pixel_clock == 0) {
            //HDMI_EMG("[ERR] pixel_clockx1024 is zero=%d,%d\n", b_clk, cd);
            return FALSE;
        }
    }
    else {
        pixel_clock = GET_FLOW_CFG(HDMI_FLOW_CFG_HDMI21_LT2, HDMI_FLOW_CFG3_HD21_SETUP_PLL_6G) * 100;
    }
    HDMI_EMG("pixel_clock = %ld\n", pixel_clock);
    *pll_pixelclockx1024 = pixel_clock;

    if ((frl_mode == MODE_TMDS) && (is_420) && pixel_clock > 30000) {
        HDMI_EMG("[EMG] TMDS YUV420 with pixel_clock (%d) > 300 M, 4K120 @ YUV420 detected, eanable 4K120 YUV420 mode \n", pixel_clock);
        lib_hdmi_4k120_yu420_en(nport, 1);
        enable_2x = 0;
    }
    else if (is_420 || is_interlace) {
        pixel_clock = pixel_clock * 2;  //pixel_clockx1024 = 297 << 10;
        enable_2x = 1;
        //HDMI_EMG("[ERR] TMDS YUV420 with pixel_clock (%d) < 300 M, not YUV420 4K120, disable 4K120 YUV420 mode \n", pixel_clockx1024);
        lib_hdmi_4k120_yu420_en(nport, 0);
    }
    else {
        enable_2x = 0;
        lib_hdmi_4k120_yu420_en(nport, 0);
    }

    // 3. find o
    for (i = 1; i <= PLL_OUTDIV_NUM; i++) {
        pll_temp_data = pixel_clock << ((unsigned int)i);
        if (pll_temp_data <= PLL_FVCO_MAX) {
            continue;
        }
        else {
            // > 769.50M
            o = (i - 1);
            fvco = pixel_clock << (o);
            HDMI_EMG("Find o= %d,", o);
            HDMI_EMG("fvco= %d\n", fvco);
            break;
        }
    }

    // 4. find best m,n
    n = 0;
    for (i = 0; i < PLL_M_NUM; i++) {
        fvco_tolarence = (PLL_FPFD_LEVEL >> 1);

        pll_temp_data = PLL_FVCO_MIN + i * PLL_FPFD_LEVEL;
        if (pll_temp_data > fvco) {
            pll_temp_data = pll_temp_data - fvco;
        }
        else {
            pll_temp_data = fvco - pll_temp_data;
        }
        if (pll_temp_data <= fvco_tolarence) {
            m = PLL_M_MIN + i;
            HDMI_EMG("Find m= %d\n", m);

            break;
        }
    }

    // 5. find IP, RS
    for (i = 0 ; i < video_dprxpll_others_table_size ; i++) {
        if ((m >= video_dprxpll_others_param[i].min_M) && (m  <= video_dprxpll_others_param[i].max_M)) {
            dpll_ip = video_dprxpll_others_param[i].dpll_IP;
            dpll_rs = video_dprxpll_others_param[i].dpll_RS;
            break;
        }
    }

    if (fvco <= 77000 && fvco >= 60000) {
        dpll_sel_ipi = 0x3;
    }
    else if (fvco < 60000 && fvco >= 50000) {
        dpll_sel_ipi = 0x3;//dpll_sel_ipi = 0x2;
    }
    else if (fvco < 50000 && fvco >= 40000) {
        dpll_sel_ipi = 0x3;//dpll_sel_ipi = 0x1;
    }
    else if (fvco < 40000 && fvco >= 31500) {
        dpll_sel_ipi = 0x3;//dpll_sel_ipi = 0x0;
    }
    else {
        dpll_sel_ipi = 0x3;
        HDMI_EMG("[ERROR] invaild fvco=%d\n", fvco);
    }

    if (enable_2x) {
        hdmi_mask(HDMI_TMDS_CPS_reg, ~HDMI_TMDS_CPS_pll_div2_en_mask, HDMI_TMDS_CPS_pll_div2_en(1));
    }
    else {
        hdmi_mask(HDMI_TMDS_CPS_reg, ~HDMI_TMDS_CPS_pll_div2_en_mask, HDMI_TMDS_CPS_pll_div2_en(0));

    }

    if (cd || enable_2x) {
        lib_hdmi_set_dpc_bypass_dis(nport, 1);    // enable deep color
    }
    else {
        lib_hdmi_set_dpc_bypass_dis(nport, 0);    // bypass deep color
    }

    if (frl_mode == MODE_TMDS) {
        unsigned char rato = 1;
        if (enable_2x) {
            rato = 2;
        }
        n = 13;
        m = ((unsigned long)dpll_ratio[cd].SM * (n + 2) * (2 << o) * rato / 2 / dpll_ratio[cd].SN) - 4;
        HDMI_EMG("HDMI2.0 PLL by formula m = %d\n", m);
    }
    else {
        HDMI_EMG("HDMI2.1 PLL7290M\n");
        m = 50;
        n = 0;
        o = 0;
        dpll_ip =  0;
        dpll_rs = 5;
    }

    if (frl_mode == MODE_TMDS) {
        hdmi_mask(HDMI_MN_SCLKG_CTRL_reg, ~HDMI_MN_SCLKG_CTRL0_sclkg_pll_in_sel_mask, HDMI_MN_SCLKG_CTRL0_sclkg_pll_in_sel_mask);
    }
    else {
        hdmi_mask(HDMI_MN_SCLKG_CTRL_reg, ~HDMI_MN_SCLKG_CTRL0_sclkg_pll_in_sel_mask, 0x0);
    }

    hdmi_mask(HDMI_MN_SCLKG_CTRL_reg, ~HDMI_MN_SCLKG_CTRL0_sclkg_dbuf_mask, HDMI_MN_SCLKG_CTRL0_sclkg_dbuf_mask);       //video PLL double buffer load

    //Enable PLL Power

    if (frl_mode == MODE_TMDS) {
        hdmi_mask(M8P_HDMI_DPRXPLL_CLK_IN_MUX_SEL_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_CLK_IN_MUX_SEL_sclkg_pll_in_sel_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_CLK_IN_MUX_SEL_sclkg_pll_in_sel_mask);
    }
    else {
        hdmi_mask(M8P_HDMI_DPRXPLL_CLK_IN_MUX_SEL_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_CLK_IN_MUX_SEL_sclkg_pll_in_sel_mask, 0x0);
    }

    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_sel_prediv_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_sel_prediv(n));  // hdmi_out(0xB8040430, 0x10110500);  //N=0, O=0, RS=5, Icp=0, CP=0
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_o_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_o(o));
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_rs_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_rs(dpll_rs));
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_ip_mask,  M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_ip(dpll_ip));
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_cp_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_cp(0));
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_1_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_m_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_m(m));    // hdmi_out(0x18040434, 0x00320200);  //M=50
    //PLL_Out=27/(0+2)*(50+4)/2^0=729(MHz)
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_2_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_2_dprxpll_wdrst_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_2_dprxpll_wdrst_mask); // hdmi_out(0xB8040438, 0x00000002);  //BPN=0
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_ldopow_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_ldopow_mask); // hdmi_out(0xB8040430, 0x10810500);  //dprxpll_ldopow=1, dprxpll_ldo_ref2core=0, dprxpll_oeb=1 (disable)
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_ldo_ref2core_mask, 0);
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_oeb_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_oeb_mask);
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_1_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_pow_mask, 0); // hdmi_out(0xB8040434, 0x00320000);  //dprxpll_pow=0, dprxpll_rstb=0 (reset)
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_1_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_rstb_mask, 0);
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_1_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_pow_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_pow_mask); // hdmi_out(0xB8040434, 0x00320200);  //dprxpll_pow=1
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_1_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_rstb_mask, M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_dprxpll_rstb_mask);   // hdmi_out(0xB8040434, 0x00320300);  //dprxpll_pow=1, dprxpll_rstb=1 (normal)
    hdmi_mask(M8P_HDMI_DPRXPLL_SETTING_reg, ~M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_dprxpll_oeb_mask, 0); // hdmi_out(0xB8040430, 0x10800500);    //dprxpll_oeb=0 (enable)

#if 1    //debugging use
    HDMI_INFO("***************TMDS=%d\n", b_clk);
    HDMI_INFO("***************cd=%d\n", cd);
    HDMI_INFO("***************m=%d\n", m);
    HDMI_INFO("***************n=%d\n", n);
    HDMI_INFO("***************o=%d\n", o);
    HDMI_INFO("***************dpll_sel_ipi=%d\n", dpll_sel_ipi);
    //HDMI_PRINTF("***************fraction1=%d\n",fraction1);
    //HDMI_PRINTF("***************fraction2=%d\n",fraction2);
    HDMI_INFO("***************pixel_clockx1024=%d\n", pixel_clock);
    HDMI_INFO("***************fvco=%d MHz\n", fvco);
    HDMI_INFO("***************larget=%d\n", large_ratio);
    HDMI_INFO("***************ip=%d\n", dpll_ip);
    HDMI_INFO("***************rs=%d\n", dpll_rs);
#endif
    HDMI_INFO("410 HDMI_MN_SCLKG_CTRL_reg=0x%x\n", hdmi_in(HDMI_MN_SCLKG_CTRL_reg));

    HDMI_INFO("414 HDMI_MN_SCLKG_DIVS_reg=0x%x\n", hdmi_in(HDMI_MN_SCLKG_DIVS_reg));
    HDMI_INFO("430 HDMI_HDMI_VPLL_SETTING_reg=0x%x\n", hdmi_in(M8P_HDMI_DPRXPLL_SETTING_reg));
    HDMI_INFO("434 HDMI_HDMI_VPLL_SETTING_1_reg=0x%x\n", hdmi_in(M8P_HDMI_DPRXPLL_SETTING_1_reg));
    HDMI_INFO("438 HDMI_HDMI_VPLL_SETTING_2_reg=0x%x\n", hdmi_in(M8P_HDMI_DPRXPLL_SETTING_2_reg));
    HDMI_INFO("43c M8P_HDMI_DPRXPLL_CLK_IN_MUX_SEL_reg=0x%x\n", hdmi_in(M8P_HDMI_DPRXPLL_CLK_IN_MUX_SEL_reg));

    if (frl_mode != MODE_TMDS) {
        //TBD, FRL MODE should set dpc corresponding Deep color value. Need more test on non FRL mode.

        hdmi_mask(addr, ~M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en_mask, M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en_mask);
        if (cd != 0) {
            lib_hdmi_set_dpc_enable(nport, 1);    // enable deep color support
        }
    }
    else {
        hdmi_mask(addr, ~M8P_HDMI_DP_COMMON_HDMIRX_YUV420_CTRL_1_yuv420_clk_valid_en_mask, 0);
        lib_hdmi_set_dpc_enable(nport, 1); // enable deep color support
        udelay(1);
        lib_hdmi_420_clkgen_en(nport, (is_420) ? 1 : 0);
        lib_hdmi_420_en(nport, (is_420) ? 1 : 0); // enable YUV420 function
    }
    if (GET_H_COLOR_SPACE(nport) == COLOR_YUV422) {
        lib_hdmi_yuv422_cd_set_frame_cnt_thr(nport);
        lib_hdmi_yuv422_cd_set_pixel_cnt_thr(nport);
        lib_hdmi_yuv422_cd_det_en(nport, 1);
    }
    return TRUE;
}


void m8p_lib_hdmi_clear_avmute(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en){
        hdmi_mask(M8P_HD21_HDMI_SR_reg, ~(M8P_HDMI21_P0_HD21_HDMI_GPVS1_chg_avmute_mask), M8P_HDMI21_P0_HD21_HDMI_GPVS1_chg_avmute_mask);
    }else{
        hdmi_mask(M8P_HDMI_HDMI_SR_reg, ~(M8P_HDMI_P0_HDMI_GPVS1_chg_avmute_mask), M8P_HDMI_P0_HDMI_GPVS1_chg_avmute_mask);
    }
}

unsigned char m8p_lib_hdmi_get_avmute(unsigned char nport)
{
    if(g_hdmi_debug_force_avmute[nport] !=0)
    {//debug use
        return 1; //force avmute
    }

    if (hdmi_rx[nport].hdmi_2p1_en)
        return M8P_HDMI21_P0_HD21_HDMI_GPVS1_get_chg_avmute(hdmi_in(M8P_HD21_HDMI_SR_reg));

    return M8P_HDMI_P0_HDMI_GPVS1_get_chg_avmute(hdmi_in(M8P_HDMI_HDMI_SR_reg));
}

void m8p_lib_hdmi_clear_hdmi_mode_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en){
        ;// not support
    }else{
        hdmi_mask(M8P_HDMI_HDMI_SR_reg, ~(M8P_HDMI_P0_HDMI_GPVS1_chg_hdmi_md_mask), M8P_HDMI_P0_HDMI_GPVS1_chg_hdmi_md_mask);
    }
}

unsigned char m8p_lib_hdmi_get_hdmi_mode_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en)
        return 0;// hdmi2.1 not support

    return M8P_HDMI_P0_HDMI_GPVS1_get_chg_hdmi_md(hdmi_in(M8P_HDMI_HDMI_SR_reg));
}


void m8p_lib_hdmi_clear_hdmi_no_clk_in_chg_irq_en(unsigned char nport, unsigned char enable)
{
    if (hdmi_rx[nport].hdmi_2p1_en){
        hdmi_mask(M8P_HD21_CLKDETSR_reg, ~(M8P_HD21_CLKDETSR_no_clk_chg_irq_en_mask),M8P_HD21_CLKDETSR_no_clk_chg_irq_en(enable));
    }else{
        ;//
    }
}

void m8p_lib_hdmi_clear_hdmi_no_clk_in_chg(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en){
        hdmi_mask(M8P_HD21_CLKDETSR_reg, ~(M8P_HD21_CLKDETSR_no_clk_chg_irq_flag_mask),M8P_HD21_CLKDETSR_no_clk_chg_irq_flag_mask);
    }else{
        hdmi_mask(M8P_HDMI_TMDS_CPS_reg, ~(M8P_HDMI_TMDS_CPS_no_clk_in_mask), M8P_HDMI_TMDS_CPS_no_clk_in_mask);
    }
}

unsigned char m8p_lib_hdmi_get_hdmi_no_clk_in_chg(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en)
        return M8P_HD21_CLKDETSR_get_no_clk_chg_irq_flag(hdmi_in(M8P_HD21_CLKDETSR_reg));

    return M8P_HDMI_TMDS_CPS_get_no_clk_in(hdmi_in(M8P_HDMI_TMDS_CPS_reg));
}

void m8p_lib_hdmi_crc_continue_mode_en(unsigned char nport, unsigned char enable)
{
    if (enable) {
        hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~(M8P_HDMI_CRC_Ctrl_crc_conti_mask), M8P_HDMI_CRC_Ctrl_crc_conti(1));    //enable CRC_conti
    }
    else {
        hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~(M8P_HDMI_CRC_Ctrl_crc_conti_mask), M8P_HDMI_CRC_Ctrl_crc_conti(0));    //clr CRC_conti
    }
}



void m8p_lib_hdmi_crc_auto_cmp_en(unsigned char nport, unsigned char enable)
{
    if (enable) {
        hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~M8P_HDMI_CRC_Ctrl_crc_auto_cmp_en_mask, 0);
        udelay(100);
        hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~M8P_HDMI_CRC_Ctrl_crc_auto_cmp_en_mask, M8P_HDMI_CRC_Ctrl_crc_auto_cmp_en_mask);
        udelay(100);
    }
    else {
        hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~M8P_HDMI_CRC_Ctrl_crc_auto_cmp_en_mask, 0);
        udelay(100);
    }
}

int m8p_lib_hdmi_get_crc_auto_cmp_status(unsigned char nport)
{
    return M8P_HDMI_CRC_Ctrl_get_crc_auto_cmp_en(hdmi_in(M8P_HDMI_CRC_Ctrl_reg));
}

int m8p_lib_hdmi_read_video_common_crc(unsigned char nport, unsigned char res_sel, unsigned int *p_crc, unsigned char is_continue_mode)
{
    // Common crc
    if (is_continue_mode) {
        *p_crc = hdmi_in(M8P_HDMI_CRC_Result_reg);
        hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~(M8P_HDMI_CRC_Ctrl_crc_conti_mask | M8P_HDMI_CRC_Ctrl_crc_res_sel_mask | M8P_HDMI_CRC_Ctrl_crc_start_mask),
                  M8P_HDMI_CRC_Ctrl_crc_conti(1) | M8P_HDMI_CRC_Ctrl_get_crc_res_sel(res_sel) | M8P_HDMI_CRC_Ctrl_crc_start(1));
        if (*p_crc == 0) {
            return -1;
        }
        //HDMI_EMG("[%s]Common_crc = 0x%x\n", __FUNCTION__, *p_crc);
        return 0;
    }
    else {
        hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~(M8P_HDMI_CRC_Ctrl_crc_conti_mask), 0); //clr CRC_conti
        udelay(100);
        if (M8P_HDMI_CRC_Ctrl_get_crc_start(hdmi_in(M8P_HDMI_CRC_Ctrl_reg)) == 0) {
            // restart CRC
            *p_crc = hdmi_in(M8P_HDMI_CRC_Result_reg);
            hdmi_mask(M8P_HDMI_CRC_Ctrl_reg, ~(M8P_HDMI_CRC_Ctrl_crc_conti_mask | M8P_HDMI_CRC_Ctrl_crc_res_sel_mask | M8P_HDMI_CRC_Ctrl_crc_start_mask),
                      M8P_HDMI_CRC_Ctrl_crc_conti(0) | M8P_HDMI_CRC_Ctrl_get_crc_res_sel(res_sel) | M8P_HDMI_CRC_Ctrl_crc_start(1)); // When crc_start=1, it will clear CRC result to 0.
            return 0;
        }
        return -1;
    }
}




//------------------------------------------------------------------
// HD20/21 CTRL
//------------------------------------------------------------------
void m8p_lib_hdmi_set_hd20_21_ctrl_pixel_mode(unsigned char nport, unsigned char pixel_mode)
{
    //at video common, p0=p1=p2=p3
    UINT32 hdmirx_2p_clk_sel_addr[HDMI_PORT_TOTAL_NUM] = {
        M8P_SYS_REG_SYS_CLKSEL_reg,
        M8P_SYS_REG_SYS_DISPCLK_FRACT_2_reg,
        M8P_SYS_REG_SYS_DISPCLK_FRACT_2_reg,
        M8P_SYS_REG_SYS_DISPCLK_FRACT_2_reg
    };
    UINT32 hdmirx_2p_clk_sel_mask[HDMI_PORT_TOTAL_NUM] = {
        M8P_SYS_REG_SYS_CLKSEL_hdmirx0_2p_clk_sel_mask,
        M8P_SYS_REG_SYS_DISPCLK_FRACT_2_hdmirx1_2p_clk_sel_mask,
        /*M8P_SYS_REG_SYS_DISPCLK_FRACT_2_hdmirx2_2p_clk_sel_mask,
        M8P_SYS_REG_SYS_DISPCLK_FRACT_2_hdmirx3_2p_clk_sel_mask*/
    };

    if (GET_H_RUN_DSC(nport)) {
        lib_hdmi_dsc_clk_div_sel_by_pixel_mode(nport, pixel_mode);
    }

    switch (pixel_mode) {
        case 1: // 1 pixel mode
            hdmi_mask(HDMI_hdmi_20_21_ctrl_reg, ~HDMI_hdmi_20_21_ctrl_pixel_mode_mask, HDMI_hdmi_20_21_ctrl_pixel_mode(1)); //always 2p data sampling for vgip
            hdmi_mask(HDMI_hdmi_20_21_ctrl_reg, ~HDMI_hdmi_20_21_ctrl_out_pixel_clk_div2_mask, HDMI_hdmi_20_21_ctrl_out_pixel_clk_div2(1));
            hdmi_mask(hdmirx_2p_clk_sel_addr[nport], ~(hdmirx_2p_clk_sel_mask[nport]), hdmirx_2p_clk_sel_mask[nport]);
            hdmi_mask(M8P_HDMI_COMMON_BYPASS_PATH_CTRL_reg, ~(M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd21_bypass_pixel_trans_clk_div_mask | M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd20_bypass_pixel_trans_clk_div_mask),
                      (M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd21_bypass_pixel_trans_clk_div(1) | M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd20_bypass_pixel_trans_clk_div(1)));
            break;

        case 2: // 2 pixel mode
            hdmi_mask(HDMI_hdmi_20_21_ctrl_reg, ~HDMI_hdmi_20_21_ctrl_pixel_mode_mask, HDMI_hdmi_20_21_ctrl_pixel_mode(1));
            hdmi_mask(HDMI_hdmi_20_21_ctrl_reg, ~HDMI_hdmi_20_21_ctrl_out_pixel_clk_div2_mask, HDMI_hdmi_20_21_ctrl_out_pixel_clk_div2(0));
            hdmi_mask(hdmirx_2p_clk_sel_addr[nport], ~(hdmirx_2p_clk_sel_mask[nport]), 0x0);
            hdmi_mask(M8P_HDMI_COMMON_BYPASS_PATH_CTRL_reg, ~(M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd21_bypass_pixel_trans_clk_div_mask | M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd20_bypass_pixel_trans_clk_div_mask),
                      (M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd21_bypass_pixel_trans_clk_div(0) | M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_hd20_bypass_pixel_trans_clk_div(0)));
            break;
        case 4: // 4 pixel mode
            HDMI_ERR("[lib_hdmi_set_hd20_21_ctrl_pixel_mode port:%d] Not support 4 pixel mode on this Model\n", nport);
            break;
        default:
            HDMI_EMG("[lib_hdmi_set_hd20_21_ctrl_pixel_mode port:%d] invalid pixel mode =%d\n", nport, pixel_mode);
            break;
    }
    HDMI_INFO("[lib_hdmi_set_hd20_21_ctrl_pixel_mode nport=%d] pixel_mode=%d \n", nport, pixel_mode);
}

void m8p_lib_hdmi_dsc_clk_div_sel_by_pixel_mode(unsigned char nport, unsigned char pixel_mode)
{
    switch (pixel_mode) {
        case 1: // 1 pixel mode
            hdmi_out(M8P_HDMI_support_dscd_clken_reg, (M8P_HDMI_SUPPORT_DSCD_CLKEN_support_dscd_clken(1) |
                                                       M8P_HDMI_SUPPORT_DSCD_CLKEN_dscd_out_div(2) |
                                                       M8P_HDMI_SUPPORT_DSCD_CLKEN_dscd_out_yuv420_r_div(2) |
                                                       M8P_HDMI_PORT_COMMON_SUPPORT_DSCD_CLKEN_dscd_pixel_trans_clk_div(1)));
            break;
        case 2: // 2 pixel mode
        default:
            hdmi_out(M8P_HDMI_support_dscd_clken_reg, (M8P_HDMI_SUPPORT_DSCD_CLKEN_support_dscd_clken(1) |
                                                       M8P_HDMI_SUPPORT_DSCD_CLKEN_dscd_out_div(1) |
                                                       M8P_HDMI_SUPPORT_DSCD_CLKEN_dscd_out_yuv420_r_div(1)));
            break;
    }
    HDMI_INFO("[lib_hdmi_dsc_clk_div_sel_by_pixel_mode nport=%d] pixel_mode=%d \n", nport, pixel_mode);
}

//-------------------------------------------------------------------------
// Dummy registers that are reserved for FW debug usage.
//
// To easy to debug, HDMI driver provides serval reserved register bits
// to allow user to change behavior of the HDMI driver.
//
// Please note that these registers might vary on different SOC.
//
// On K5LP, the FW debug registers are show as below
//-------------------------------------------------------------------------

/*---------------------------------------------------
 * Func : lib_hdmi_get_fw_debug_bit
 *
 * Desc : get FW debug bits status.
 *
 * Para : debug_bit : debug bit mask
 *        val       : 0 : disable, others  enabled
 *
 * Retn : 0 : disabled, 1 : eanbled
 *--------------------------------------------------*/
unsigned char m8p_lib_hdmi_get_fw_debug_bit(unsigned char nport, unsigned int debug_bit)
{
    return ((hdmi_in(M8P_HDMI_FW_FUNC_reg) & debug_bit)) ? 1 : 0;
}

/*---------------------------------------------------
 * Func : lib_hdmi_set_fw_debug_bit
 *
 * Desc : set FW debug bits.
 *
 * Para : debug_bit : debug bit mask
 *        val       : 0 : disable, others  enabled
 *
 * Retn : N/A
 *--------------------------------------------------*/
void m8p_lib_hdmi_set_fw_debug_bit(unsigned char nport, unsigned int debug_bit, unsigned char on)
{
    hdmi_mask(M8P_HDMI_FW_FUNC_reg, ~debug_bit, (on) ? debug_bit : 0);
}


// hdmirx bypass pkt to hdmitx
void m8p_lib_hdmi_drop_pkt_enable(unsigned char nport, DROP_PKT_TYPE type, unsigned char enable)
{
    switch (type) {
        case DROP_PKT_AUDIO_DATA:
            HDMI_WARN("DROP_PKT_AUDIO_DATA = %d\n", enable);
            if (hdmi_rx[nport].hdmi_2p1_en) {
                hdmi_mask(M8P_HD21_PKT_reg, ~M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_aud_data_drop_en_mask, M8P_HDMI21_P0_HD21_HDMI_PKT_get_pkt_aud_data_drop_en(enable));
            }
            else {
                hdmi_mask(M8P_HDMI_PKT_reg, ~M8P_HDMI_P0_HDMI_PKT_pkt_aud_data_drop_en_mask, M8P_HDMI_P0_HDMI_PKT_get_pkt_aud_data_drop_en(enable));
            }
            break;
        case DROP_PKT_NON_AUDIO_DATA:
            HDMI_WARN("DROP_PKT_NON_AUDIO_DATA = %d\n", enable);
            if (hdmi_rx[nport].hdmi_2p1_en) {
                hdmi_mask(M8P_HD21_PKT_reg, ~M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_non_aud_data_drop_en_mask, M8P_HDMI21_P0_HD21_HDMI_PKT_get_pkt_non_aud_data_drop_en(enable));
            }
            else {
                hdmi_mask(M8P_HDMI_PKT_reg, ~M8P_HDMI_P0_HDMI_PKT_pkt_non_aud_data_drop_en_mask, M8P_HDMI_P0_HDMI_PKT_get_pkt_non_aud_data_drop_en(enable));
            }
            break;
        case DROP_PKT_HEAD:
            HDMI_WARN("DROP_PKT_HEAD = %d\n", enable);
            if (hdmi_rx[nport].hdmi_2p1_en) {
                hdmi_mask(M8P_HD21_PKT_reg, ~M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_head_drop_en_mask, M8P_HDMI21_P0_HD21_HDMI_PKT_get_pkt_head_drop_en(enable));
            }
            else {
                hdmi_mask(M8P_HDMI_PKT_reg, ~M8P_HDMI_P0_HDMI_PKT_pkt_head_drop_en_mask, M8P_HDMI_P0_HDMI_PKT_get_pkt_head_drop_en(enable));
            }
            break;

        case DROP_PKT_ALL:
            HDMI_WARN("DROP_PKT_ALL= %d\n", enable);
            if (hdmi_rx[nport].hdmi_2p1_en) {
                if (enable) {
                    hdmi_mask(M8P_HD21_PKT_reg, ~(M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_aud_data_drop_en_mask | M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_non_aud_data_drop_en_mask | M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_head_drop_en_mask),
                              (M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_aud_data_drop_en_mask | M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_non_aud_data_drop_en_mask | M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_head_drop_en_mask));
                }
                else {
                    hdmi_mask(M8P_HD21_PKT_reg, ~(M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_aud_data_drop_en_mask | M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_non_aud_data_drop_en_mask | M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_head_drop_en_mask), 0);
                }
            }
            else {
                if (enable) {
                    hdmi_mask(M8P_HDMI_PKT_reg, ~(M8P_HDMI_P0_HDMI_PKT_pkt_aud_data_drop_en_mask | M8P_HDMI_P0_HDMI_PKT_pkt_non_aud_data_drop_en_mask | M8P_HDMI_P0_HDMI_PKT_pkt_head_drop_en_mask),
                              (M8P_HDMI_P0_HDMI_PKT_pkt_aud_data_drop_en_mask | M8P_HDMI_P0_HDMI_PKT_pkt_non_aud_data_drop_en_mask | M8P_HDMI_P0_HDMI_PKT_pkt_head_drop_en_mask));
                }
                else {
                    hdmi_mask(M8P_HDMI_PKT_reg, ~(M8P_HDMI_P0_HDMI_PKT_pkt_aud_data_drop_en_mask | M8P_HDMI_P0_HDMI_PKT_pkt_non_aud_data_drop_en_mask | M8P_HDMI_P0_HDMI_PKT_pkt_head_drop_en_mask), 0);
                }
            }
            break;
        default:
            break;
    }
}

void m8p_lib_hdmi_pkt_to_hdmitx_output_enable(unsigned char nport, unsigned char enable)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_mask(M8P_HD21_PKT_reg, ~M8P_HDMI21_P0_HD21_HDMI_PKT_pkt_2hdmitx_output_en_mask, M8P_HDMI21_P0_HD21_HDMI_PKT_get_pkt_2hdmitx_output_en(enable));
    }
    else {
        hdmi_mask(M8P_HDMI_PKT_reg, ~M8P_HDMI_P0_HDMI_PKT_pkt_2hdmitx_output_en_mask, M8P_HDMI_P0_HDMI_PKT_get_pkt_2hdmitx_output_en(enable));
    }
}

void m8p_lib_hdmirx_to_hdmitx_pkt_port_sel(unsigned char port)
{
    hdmi_mask(M8P_HDMI_TOP_COMMON_hdmirx2hdmitx_reg, ~(M8P_HDMI_TOP_COMMON_hdmirx2hdmitx_pkt_port_sel_mask), M8P_HDMI_TOP_COMMON_hdmirx2hdmitx_pkt_port_sel(port));
}


void m8p_lib_hdmi_hd20_fec_measure_enable(unsigned char nport, unsigned char enable)
{
    hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_3_reg, ~M8P_HD20_MEAS_FRL_MEASURE_3_hs_sel_mask, 0); // //real h sync
    hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_3_reg, ~M8P_HD20_MEAS_FRL_MEASURE_3_ht_meas_sel_mask, 0); //hsync-to-hsync
    //hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_9_reg, ~M8P_HD20_MEAS_FRL_MEASURE_9_pol_det_mode_mask, M8P_HD20_MEAS_FRL_MEASURE_9_pol_det_mode_mask);

    hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_0_reg, ~M8P_HD20_MEAS_FRL_MEASURE_0_measure_en_mask, M8P_HD20_MEAS_FRL_MEASURE_0_measure_en((enable) ? 1 : 0));

}

unsigned char m8p_lib_hdmi_hd20_fec_is_measure_completed(unsigned char nport)
{
    return (M8P_HD20_MEAS_FRL_MEASURE_0_get_measure_en(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_0_reg)) == 0) ? 1 : 0;
}

void m8p_lib_hdmi_hd20_fec_measure_restart(unsigned char nport)
{
    m8p_lib_hdmi_hd20_fec_measure_enable(nport, FALSE);
    m8p_lib_hdmi_hd20_fec_measure_enable(nport, TRUE);
}


void m8p_lib_hdmi_hd20_meas_clear_interlace_reg(unsigned char nport)
{
    hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_A_reg, ~M8P_HD20_MEAS_FRL_MEASURE_A_field_tog_mask, M8P_HD20_MEAS_FRL_MEASURE_A_field_tog_mask); //write 1 clear
    hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_A_reg, ~M8P_HD20_MEAS_FRL_MEASURE_A_interlaced_err_mask, M8P_HD20_MEAS_FRL_MEASURE_A_interlaced_err_mask); //write 1 clear
}

HDMI_INT_PRO_CHECK_RESULT m8p_lib_hdmi_hd20_meas_get_interlace(unsigned char nport)
{
    return M8P_HD20_MEAS_FRL_MEASURE_A_get_field_tog(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_A_reg));
}

/*---------------------------------------------------
 * Func : lib_hdmi_hd20_calc_hfreq
 *
 * Desc : calculate hfreq by hs_period
 *
 * Para : nport : hdmi port id
 *
 * Retn : hfreq
 *--------------------------------------------------*/
unsigned int m8p_lib_hdmi_hd20_calc_hfreq(unsigned char nport)
{
    unsigned int hfreq = 0;
    unsigned long long temp_hfreq = 0;
    unsigned long long hs_period = (unsigned long long)M8P_HD20_MEAS_FRL_MEASURE_18_get_hs_period(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_18_reg));

    if (hs_period != 0) {
        temp_hfreq = 27000000ULL;
        do_div(temp_hfreq, hs_period);
        hfreq = (unsigned int)temp_hfreq;
        return hfreq;
    }
    else {
        HDMI_WARN("[HD20MS] Measure Fail!!Divide 0!! hs_period=%lld \n", hs_period);
        return 0;
    }
}
static unsigned int m8p_lib_hdmi20_measure_get_hs_sta(unsigned char nport)
{
    unsigned int hs_width = 0;
    unsigned int hback_cnt = 0;
    hs_width = M8P_HD20_MEAS_FRL_MEASURE_B_get_hs_width(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_B_reg));
    hback_cnt = M8P_HD20_MEAS_FRL_MEASURE_D_get_hback_cnt(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_D_reg));
    return (hs_width + hback_cnt);
}


static unsigned int m8p_lib_hdmi20_measure_get_vs_sta(unsigned char nport)
{
    unsigned int vback_cnt = 0;
    unsigned int vs_width = 0;
    vback_cnt = M8P_HD20_MEAS_FRL_MEASURE_14_get_vback_cnt(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_14_reg)); // vs back

    vs_width = M8P_HD20_MEAS_FRL_MEASURE_13_get_vs_width(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_13_reg)); // vs width
    return (vback_cnt + vs_width);
}

void m8p_lib_hdmi_hd20_set_polarity_meas_mode(unsigned char nport, HD20_MEASURE_POLARITY_MEAS_MODE mode)
{
    hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_9_reg, ~(M8P_HDMI20_P0_MEAS_FRL_MEASURE_9_polarity_meas_mask), M8P_HDMI20_P0_MEAS_FRL_MEASURE_9_polarity_meas(mode));
}

HD20_MEASURE_POLARITY_MEAS_MODE m8p_lib_hdmi_hd20_get_polarity_meas_mode(unsigned char nport)
{
    return M8P_HDMI20_P0_MEAS_FRL_MEASURE_9_get_polarity_meas(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_9_reg));
}

unsigned int m_hd20_measure_last_vs_cnt[4] = {0, 0, 0, 0};
HDMI20_MEASURE_RESULT m8p_lib_hdmi20_hd20_measure(unsigned char nport, MEASURE_TIMING_T *update_timing)
{
    unsigned int vs2vs_cnt = 0;
    unsigned char is_ps_measure_complete = FALSE;
    unsigned char is_fec_measure_complete = FALSE;
    memcpy(update_timing, &hdmi_rx[nport].timing_t, sizeof(MEASURE_TIMING_T));

    if ((hdmi_rx[nport].video_t.vs_valid != 0) && (hdmi_rx[nport].video_t.vs_cnt != m_hd20_measure_last_vs_cnt[nport])) {
        //frame count update, enter new frame
        is_ps_measure_complete = 1;
        m_hd20_measure_last_vs_cnt[nport] = hdmi_rx[nport].video_t.vs_cnt;
    }
    else {
        is_ps_measure_complete = 0;
    }
    is_fec_measure_complete = m8p_lib_hdmi_hd20_fec_is_measure_completed(nport);
    if (is_ps_measure_complete && is_fec_measure_complete) {
        vs2vs_cnt = hdmi_rx[nport].video_t.vs2vs_cnt;
        if (vs2vs_cnt != 0) {
            update_timing->v_freq = lib_measure_calc_vfreq(vs2vs_cnt);
        }
        else {
            HDMI_WARN("[HD20MS] port:%d, zero V count!!\n", nport);
            update_timing->v_freq = 0;
        }
    }
    else {
        update_timing->v_total = 0;
        update_timing->v_freq = 0;
        //HDMI_WARN("[HD20MS] port:%dis_vsync_get=%d,  is_ps_measure_complete=%d, is_fec_measure_complete=%d\n", nport, is_vsync_get, is_ps_measure_complete, is_fec_measure_complete);
        return HDMI20_MEASURE_WAIT_V_SYNC;
    }
    m8p_lib_hdmi_hd20_set_polarity_meas_mode(nport, HD20_POLARITY_AUDIO_MEAS);

    update_timing->h_freq = m8p_lib_hdmi_hd20_calc_hfreq(nport);
    update_timing->h_total = M8P_HDMI20_P0_MEAS_FRL_MEASURE_11_get_htotal_cnt(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_11_reg));
    update_timing->h_act_len = M8P_HD20_MEAS_FRL_MEASURE_F_get_hact_cnt(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_F_reg));
    update_timing->h_act_sta = m8p_lib_hdmi20_measure_get_hs_sta(nport); // hact_sta =hs _width + h_back

    update_timing->v_total = M8P_HD20_MEAS_FRL_MEASURE_5_get_vt_line_cnt1(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_5_reg));
    update_timing->v_act_len = M8P_HDMI20_P0_MEAS_FRL_MEASURE_16_get_vact_cnt(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_16_reg));
    update_timing->v_act_sta = m8p_lib_hdmi20_measure_get_vs_sta(nport);   // vact_sta = vs_width + v_back
    update_timing->polarity = 0;
    if (M8P_HDMI20_P0_MEAS_FRL_MEASURE_9_get_vs_polarity(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_9_reg)) == 0) {
        update_timing->polarity |= _BIT1;
    }
    if (M8P_HDMI20_P0_MEAS_FRL_MEASURE_9_get_hs_polarity(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_9_reg)) == 0) {
        update_timing->polarity |= _BIT0;
    }

    update_timing->IHSyncPulseCount = M8P_HD20_MEAS_FRL_MEASURE_B_get_hs_width(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_B_reg));
    update_timing->IVSyncPulseCount = M8P_HD20_MEAS_FRL_MEASURE_13_get_vs_width(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_13_reg));

    //Correction by Deep Color, Color Space, Pixel Repeat
    if (update_timing->pixel_repeat > 0) {
        update_timing->h_act_sta /= (update_timing->pixel_repeat + 1);
        update_timing->h_total /= (update_timing->pixel_repeat + 1);
        update_timing->h_act_len /= (update_timing->pixel_repeat + 1);
        update_timing->IHSyncPulseCount /= (update_timing->pixel_repeat + 1);
    }

    if (COLOR_YUV420 == update_timing->colorspace && (lib_hdmi_get_420_fva_ds_mode(nport) == 0)) {
        update_timing->h_act_sta <<= 1;
        update_timing->h_total <<= 1;
        update_timing->h_act_len <<= 1;
        update_timing->IHSyncPulseCount <<= 1;
    }

    //Reset status for next measure
    //lib_hdmi_hd20_clr_fec_frl_vs(nport);
    lib_hdmi_hd20_fec_measure_restart(nport);

    if (update_timing->is_interlace) {
        if (update_timing->v_total % 2) {
            HDMI_WARN("[HD20MS] interlace Corrected v total from %d to event num\n", update_timing->v_total);
            update_timing->v_total -= 1;
        }
        if (update_timing->v_act_sta % 2) {
            HDMI_WARN("[HD20MS] interlace Corrected v star from %d to event num\n", update_timing->v_act_sta);
            update_timing->v_act_sta -= 1;
        }
    }

    lib_measure_active_correction(&(update_timing->h_act_len), &(update_timing->v_act_len));
    if (GET_H_VIDEO_FSM(nport) < MAIN_FSM_HDMI_VIDEO_READY) {
        HDMI_WARN("[HD20MS] MEASURE_3: 0x%x, MEASURE_4: 0x%x\n", hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_3_reg), hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_4_reg));

        HDMI_WARN("[HD20MS] IHTotal: %d, ht_clk_cnt=%d\n", update_timing->h_total, M8P_HD20_MEAS_FRL_MEASURE_1_get_ht_clk_cnt(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_1_reg)));
        HDMI_WARN("[HD20MS] IVTotal: %d,IVTotal_3rd=%d\n", update_timing->v_total, M8P_HD20_MEAS_FRL_MEASURE_7_get_vt_line_cnt2(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_7_reg)));
        HDMI_WARN("[HD20MS] IHAct: %d\n", update_timing->h_act_len);
        HDMI_WARN("[HD20MS] IVAct: %d\n", update_timing->v_act_len);
        HDMI_WARN("[HD20MS] IVFreq: %d\n", update_timing->v_freq);
        HDMI_WARN("[HD20MS] IHFreq: %d\n", update_timing->h_freq);
        HDMI_WARN("[HD20MS] Polarity: %d(0:h/v-, 1:h+v-, 2:h-v+, 3:h/v +)\n", update_timing->polarity);
        HDMI_WARN("[HD20MS] Hstar: %d\n", update_timing->h_act_sta);
        HDMI_WARN("[HD20MS] Vstar: %d\n", update_timing->v_act_sta);
        HDMI_WARN("[HD20MS] IHsyncWidth %d\n", update_timing->IHSyncPulseCount);
        HDMI_WARN("[HD20MS] IVsyncWidth %d\n", update_timing->IVSyncPulseCount);
    }
    return HDMI20_MEASURE_SUCCESS;
}

void m8p_lib_hdmi_hd20_clr_measure_last_vs_cnt(unsigned char nport)
{
    m_hd20_measure_last_vs_cnt[nport] = 0;
}

// 0: 4 frame pop  1: 1 frame pop
void m8p_lib_hdmi_hd20_meas_set_frame_pop_mode(unsigned char nport, unsigned char mode)
{
    hdmi_mask(M8P_HD20_MEAS_FRL_MEASURE_A_reg, ~M8P_HD20_MEAS_FRL_MEASURE_A_frame_pop_mode_mask, M8P_HD20_MEAS_FRL_MEASURE_A_frame_pop_mode(mode ? 1 : 0));
}

// 0: 4 frame pop  1: 1 frame pop
unsigned char m8p_lib_hdmi_hd20_meas_get_frame_pop_mode(unsigned char nport)
{
    return M8P_HD20_MEAS_FRL_MEASURE_A_get_frame_pop_mode(hdmi_in(M8P_HD20_MEAS_FRL_MEASURE_A_reg));
}


void m8p_lib_hd20ms_timing_check_h(UINT8 port, MEASURE_TIMING_T *update_timing)
{
    if (update_timing->h_act_len != hdmi_rx[port].timing_t.h_act_len) {
        HDMI_WARN("[HD20MS]hact: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.h_act_len, update_timing->h_act_len);
        hdmi_rx[port].timing_t.h_act_len = update_timing->h_act_len;
    }
    if (update_timing->h_total != hdmi_rx[port].timing_t.h_total) {
        HDMI_WARN("[HD20MS]htotal: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.h_total, update_timing->h_total);
        hdmi_rx[port].timing_t.h_total = update_timing->h_total;
    }
    if (update_timing->h_act_sta != hdmi_rx[port].timing_t.h_act_sta) {
        HDMI_WARN("[HD20MS]hstar: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.h_act_sta, update_timing->h_act_sta);
        hdmi_rx[port].timing_t.h_act_sta = update_timing->h_act_sta;
    }
    if ((0 == hdmi_rx[port].timing_t.h_freq) \
            && (update_timing->h_freq != hdmi_rx[port].timing_t.h_freq)) {
        HDMI_WARN("[HD20MS]hfreq: offms %d -> hd20ms %d\n", (UINT32)hdmi_rx[port].timing_t.h_freq, update_timing->h_freq);
        hdmi_rx[port].timing_t.h_freq = update_timing->h_freq;
    }
    if (update_timing->IHSyncPulseCount != hdmi_rx[port].timing_t.IHSyncPulseCount) {
        HDMI_WARN("[HD20MS]hsync: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.IHSyncPulseCount, update_timing->IHSyncPulseCount);
        hdmi_rx[port].timing_t.IHSyncPulseCount = update_timing->IHSyncPulseCount;
    }
}
void m8p_lib_hd20ms_timing_check_v(UINT8 port, MEASURE_TIMING_T *update_timing)
{
    if (update_timing->v_act_len != hdmi_rx[port].timing_t.v_act_len) {
        HDMI_WARN("[HD20MS]vact: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.v_act_len, update_timing->v_act_len);
        hdmi_rx[port].timing_t.v_act_len = update_timing->v_act_len;
    }
    if (update_timing->v_act_sta != hdmi_rx[port].timing_t.v_act_sta) {
        HDMI_WARN("[HD20MS]vstar: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.v_act_sta, update_timing->v_act_sta);
        hdmi_rx[port].timing_t.v_act_sta = update_timing->v_act_sta;
    }
    if (update_timing->IVSyncPulseCount != hdmi_rx[port].timing_t.IVSyncPulseCount) {
        HDMI_WARN("[HD20MS]vsync: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.IVSyncPulseCount, update_timing->IVSyncPulseCount);
        hdmi_rx[port].timing_t.IVSyncPulseCount = update_timing->IVSyncPulseCount;
    }
    if ((hdmi_rx[port].timing_t.v_total != 0) \
            && (hdmi_rx[port].timing_t.v_freq != 0)
            && (hdmi_rx[port].timing_t.run_vrr != 0)) {
        return;
    }
    if (update_timing->v_total != hdmi_rx[port].timing_t.v_total) {
        HDMI_WARN("[HD20MS]vtotal: offms %d -> hd20ms %d\n", (UINT32)hdmi_rx[port].timing_t.v_total, update_timing->v_total);
        hdmi_rx[port].timing_t.v_total = update_timing->v_total;
    }
    if (update_timing->v_freq != hdmi_rx[port].timing_t.v_freq) {
        HDMI_WARN("[HD20MS]vfreq: offms %d -> hd20ms %d\n", (UINT32)hdmi_rx[port].timing_t.v_freq, (UINT32)update_timing->v_freq);
        hdmi_rx[port].timing_t.v_freq = update_timing->v_freq;
    }
}


void m8p_lib_hd20_measure(unsigned char port, MEASURE_TIMING_T *update_timing)
{
    HDMI20_MEASURE_RESULT result = HDMI20_MEASURE_SUCCESS;
    result = m8p_lib_hdmi20_hd20_measure(port, update_timing);
    if (result == HDMI20_MEASURE_SUCCESS) {
        m8p_lib_hd20ms_timing_check_h(port, update_timing);
        m8p_lib_hd20ms_timing_check_v(port, update_timing);
        if (update_timing->polarity != hdmi_rx[port].timing_t.polarity) {
            HDMI_WARN("[HD20MS]polarity: offms %d -> hd20ms %d\n", hdmi_rx[port].timing_t.polarity, update_timing->polarity);
            hdmi_rx[port].timing_t.polarity = update_timing->polarity;
        }
    }
    else {
        HDMI_WARN("[HD20MS]hd20 measure failed result = %d\n", result);
    }
}


void m8p_lib_hdmi_output_disable(unsigned char port)
{
    HDMI_INFO("%s: disable HDMI%d A/V output\n", __func__, port);
    lib_hdmi_video_output(port, 0);     // disable video output
    lib_hdmi_audio_output(port, 0);     // close audio
    lib_hdmi_ofms_clock_sel(port, 0);   // switch back to tmds clock
    lib_hdmi_set_dpc_enable(port, 0);   // disable dpc
}

