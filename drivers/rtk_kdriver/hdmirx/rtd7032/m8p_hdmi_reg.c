/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file:        hdmi_reg.c
 *
 * author:  Falcon, Sam Wilson
 * date:
 * version:     3.0
 *
 *============================================================*/

/*========================Header Files============================*/
#include "hdmi_common.h"
#include "m8p_hdmi_reg.h"

/*--------------------------------------------------
 * Mac register setting
 *--------------------------------------------------*/
const struct m8p_hdmi_mac_reg_st m8p_mac[4] = {
    // mac 0
    {
        .tmds_cps                     = M8P_HDMI_P0_TMDS_CPS_reg,
        .hdmi_gpvs1                   = M8P_HDMI_P0_HDMI_GPVS1_reg,

        .hdmi_vpllcr0                 = 0,
        .hdmi_vpllcr1                 = 0,
        .hdmi_vpllcr2                 = 0,
        .mn_sclkg_ctrl                = 0,
        .hdmi_dprxpll_setting         = M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_reg,
        .hdmi_dprxpll_setting_1       = M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_1_reg,
        .hdmi_dprxpll_setting_2       = M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_SETTING_2_reg,
        .hdmi_dprxpll_clk_in_mux_sel  = M8P_HDMI_DPRXPLL_HDMI_DPRXPLL_CLK_IN_MUX_SEL_reg,
        .mn_sclkg_divs                = M8P_HDMI_PORT_COMMON_MN_SCLKG_DIVS1_reg,
        .mdd_cr                       = M8P_HDMI_PORT_COMMON_MDD_CR_reg,
        .mdd_sr                       = M8P_HDMI_PORT_COMMON_MDD_SR_reg,
        .fw_func                      = M8P_HDMI_PORT_COMMON_FW_FUNC_reg,
        .port_switch                  = M8P_HDMI_PORT_COMMON_PORT_SWITCH_reg,
        .power_saving                 = M8P_HDMI_PORT_COMMON_POWER_SAVING_reg,
        .crc_ctrl                     = M8P_HDMI_DP_COMMON_CRC_Ctrl_reg,
        .crc_result                   = M8P_HDMI_DP_COMMON_CRC_Result_reg,
        .des_crc                      = M8P_HDMI_DP_COMMON_DES_CRC_reg,
        .crc_err_cnt0                 = M8P_HDMI_DP_COMMON_CRC_ERR_CNT0_reg,
        .hdmi_20_21_ctrl              = M8P_HDMI_PORT_COMMON_hdmi_20_21_ctrl_reg, //524

        // YUV422_CD
        .yuv422_cd                    = M8P_HDMI_DP_COMMON_YUV422_CD_reg, //528

        // HDR Extended Metadata Packet for common port  //(0xXXXXX530~0xXXXXX5C8)only for 2nd common port (p0 register), 1st common port0 still use "Per port" r/w
        .common_hdrap                 = 0, // 530
        .common_hdrdp                 = 0, // 534
        .common_hdr_st                = 0, // 538
        .common_em_ct                 = 0, // 53c
        .common_fapa_ct               = 0, // 540
        .common_support_dscd_clken    = M8P_HDMI_PORT_COMMON_SUPPORT_DSCD_CLKEN_reg, // 544
        .common_bypass_path_ctrl      = M8P_HDMI_PORT_COMMON_BYPASS_PATH_CTRL_reg,//548
        .common_hdr_em_ct2            = 0,
        .common_hdmi_vsem_emc         = 0, // 54c
        .common_hdmi_vsem_emc2        = 0,  // 550
        .common_vsem_st               = 0, //554
        .common_hdmi_emap             = 0, //558
        .common_hdmi_emdp             = 0,
        .common_hd21_hdmi_hdrap       = 0,
        .common_hd21_hdmi_hdrdp       = 0,
        .common_hd21_hdr_em_st        = 0,
        .common_hd21_hdr_em_ct        = 0,
        .common_hd21_hdr_em_ct2       = 0, //578
        .common_hd21_hdmi_emap        = 0,
        .common_hd21_hdmi_emdp        = 0,
        .common_hd21_vsem_st          = 0, //584
        .common_hd21_cvtem_st         = 0,
        .common_hd21_hdmi_vsem_emc    = 0,
        .common_hd21_hdmi_cvtem_emc   = 0,
        .common_hd21_hdmi_vsem_emc2   = 0,

        // HDMI DSCD DBG Ctrl for common port
        .common_hd21_dscd_dbg         = 0,
        .common_hd21_dscd_dbg_ap      = 0,
        .common_hd21_dscd_dbg_dp      = 0,

        // Audio Control for common port
        .common_hdmi_audcr            = 0,
        .common_hdmi_audsr            = 0,
        .common_hd21_hdmi_audcr       = 0,
        .common_hd21_hdmi_audsr       = 0,

        // HDMI Packet Status Register for common port
        .common_hdmi_gpvs             = 0,
        .common_hd21_hdmi_gpvs        = 0,

        // RM Zero Pading for common port
        .common_hd21_rmzp             = 0,

        // Video Timing Generator Status Register for common port
        .common_hd21_timing_gen_sr    = 0, // 5c8

        // One Time Register  (No Function for MAC P1,P2,P3)
        .ot                           = M8P_HDMI_TOP_COMMON_ot_reg, // 600
        .irq                          = M8P_HDMI_TOP_COMMON_IRQ_reg,
        .mp                           = M8P_HDMI_TOP_COMMON_mp_reg,
        .hdmi_v_bypass                = M8P_HDMI_TOP_COMMON_hdmi_v_bypass_reg,
        .hdmi_mp                      = M8P_HDMI_TOP_COMMON_hdmi_mp_reg,
        .debug_port                   = M8P_HDMI_TOP_COMMON_debug_port_reg,
        .aud_common_port_switch       = M8P_HDMI_TOP_COMMON_AUD_COMMON_PORT_SWITCH_reg, //61c

        .hdmi_rst0                    = M8P_HDMI_TOP_COMMON_hdmi_rst0_reg,
        .hdmi_rst3                    = M8P_HDMI_TOP_COMMON_hdmi_rst3_reg,
        .hdmi_clken0                  = M8P_HDMI_TOP_COMMON_hdmi_clken0_reg,
        .hdmi_clken3                  = M8P_HDMI_TOP_COMMON_hdmi_clken3_reg,
    },
    // mac 1
    {
    
        .tmds_cps                     = M8P_HDMI_P1_TMDS_CPS_reg,
        .hdmi_gpvs1                   = M8P_HDMI_P1_HDMI_GPVS1_reg,

        .hdmi_vpllcr0                 = 0,
        .hdmi_vpllcr1                 = 0,
        .hdmi_vpllcr2                 = 0,
        .mn_sclkg_ctrl                = 0,
        .hdmi_dprxpll_setting         = M8P_HDMI_P1_DPRXPLL_HDMI_DPRXPLL_SETTING_reg,
        .hdmi_dprxpll_setting_1       = M8P_HDMI_P1_DPRXPLL_HDMI_DPRXPLL_SETTING_1_reg,
        .hdmi_dprxpll_setting_2       = M8P_HDMI_P1_DPRXPLL_HDMI_DPRXPLL_SETTING_2_reg,
        .hdmi_dprxpll_clk_in_mux_sel  = M8P_HDMI_P1_DPRXPLL_HDMI_DPRXPLL_CLK_IN_MUX_SEL_reg,
        .mn_sclkg_divs                = M8P_HDMI_PORT1_COMMON_MN_SCLKG_DIVS1_reg,
        .mdd_cr                       = M8P_HDMI_PORT1_COMMON_MDD_CR_reg,
        .mdd_sr                       = M8P_HDMI_PORT1_COMMON_MDD_SR_reg,
        .fw_func                      = M8P_HDMI_PORT1_COMMON_FW_FUNC_reg,
        .port_switch                  = M8P_HDMI_PORT1_COMMON_PORT_SWITCH_reg,
        .power_saving                 = M8P_HDMI_PORT1_COMMON_POWER_SAVING_reg,
        .crc_ctrl                     = M8P_HDMI_DP_P1_COMMON_CRC_Ctrl_reg,
        .crc_result                   = M8P_HDMI_DP_P1_COMMON_CRC_Result_reg,
        .des_crc                      = M8P_HDMI_DP_P1_COMMON_DES_CRC_reg,
        .crc_err_cnt0                 = M8P_HDMI_DP_P1_COMMON_CRC_ERR_CNT0_reg,
        .hdmi_20_21_ctrl              = M8P_HDMI_PORT1_COMMON_hdmi_20_21_ctrl_reg,

        // YUV422_CD
        .yuv422_cd                    = M8P_HDMI_DP_P1_COMMON_YUV422_CD_reg, //528

        // HDR Extended Metadata Packet for common port  //(0xXXXXX530~0xXXXXX5C8)only for 2nd common port (p0 register), 1st common port0 still use "Per port" r/w
        .common_hdrap                 = 0, // 530
        .common_hdrdp                 = 0, // 534
        .common_hdr_st                = 0, // 538
        .common_em_ct                 = 0, // 53c
        .common_fapa_ct               = 0, // 540
        .common_support_dscd_clken    = M8P_HDMI_PORT1_COMMON_SUPPORT_DSCD_CLKEN_reg, // 544
        .common_bypass_path_ctrl      = M8P_HDMI_PORT1_COMMON_BYPASS_PATH_CTRL_reg,//548
        .common_hdr_em_ct2            = 0,
        .common_hdmi_vsem_emc         = 0, // 54c
        .common_hdmi_vsem_emc2        = 0,  // 550
        .common_vsem_st               = 0, //554
        .common_hdmi_emap             = 0, //558
        .common_hdmi_emdp             = 0,
        .common_hd21_hdmi_hdrap       = 0,
        .common_hd21_hdmi_hdrdp       = 0,
        .common_hd21_hdr_em_st        = 0,
        .common_hd21_hdr_em_ct        = 0,
        .common_hd21_hdr_em_ct2       = 0, //578
        .common_hd21_hdmi_emap        = 0,
        .common_hd21_hdmi_emdp        = 0,
        .common_hd21_vsem_st          = 0, //584
        .common_hd21_cvtem_st         = 0,
        .common_hd21_hdmi_vsem_emc    = 0,
        .common_hd21_hdmi_cvtem_emc   = 0,
        .common_hd21_hdmi_vsem_emc2   = 0,

        // HDMI DSCD DBG Ctrl for common port
        .common_hd21_dscd_dbg         = 0,
        .common_hd21_dscd_dbg_ap      = 0,
        .common_hd21_dscd_dbg_dp      = 0,

        // Audio Control for common port
        .common_hdmi_audcr            = 0,
        .common_hdmi_audsr            = 0,
        .common_hd21_hdmi_audcr       = 0,
        .common_hd21_hdmi_audsr       = 0,

        // HDMI Packet Status Register for common port
        .common_hdmi_gpvs             = 0,
        .common_hd21_hdmi_gpvs        = 0,

        // RM Zero Pading for common port
        .common_hd21_rmzp             = 0,

        // Video Timing Generator Status Register for common port
        .common_hd21_timing_gen_sr    = 0, // 5c8

        // One Time Register  (No Function for MAC P1,P2,P3)
        .ot                           = 0, // 600
        .irq                          = 0,
        .mp                           = 0,
        .hdmi_v_bypass                = 0,
        .hdmi_mp                      = 0,
        .debug_port                   = 0,
        .aud_common_port_switch       = 0, //61c

        // CRT
        .hdmi_rst0                    = 0,
        .hdmi_rst3                    = 0,
        .hdmi_clken0                  = 0,
        .hdmi_clken3                  = 0,
    },
    // mac 2
    {
    
        .tmds_cps                     = M8P_HDMI_P2_TMDS_CPS_reg,
        .hdmi_gpvs1                   = M8P_HDMI_P2_HDMI_GPVS1_reg,

        .hdmi_vpllcr0                 = 0,
        .hdmi_vpllcr1                 = 0,
        .hdmi_vpllcr2                 = 0,
        .mn_sclkg_ctrl                = 0,
        .hdmi_dprxpll_setting         = 0,
        .hdmi_dprxpll_setting_1       = 0,
        .hdmi_dprxpll_setting_2       = 0,
        .hdmi_dprxpll_clk_in_mux_sel  = 0,
        .mn_sclkg_divs                = 0,
        .mdd_cr                       = 0,
        .mdd_sr                       = 0,
        .fw_func                      = 0,
        .port_switch                  = 0,
        .power_saving                 = 0,
        .crc_ctrl                     = 0,
        .crc_result                   = 0,
        .des_crc                      = 0,
        .crc_err_cnt0                 = 0,
        .hdmi_20_21_ctrl              = 0,

        // YUV422_CD
        .yuv422_cd                    = 0, //528

        // HDR Extended Metadata Packet for common port  //(0xXXXXX530~0xXXXXX5C8)only for 2nd common port (p0 register), 1st common port0 still use "Per port" r/w
        .common_hdrap                 = 0, // 530
        .common_hdrdp                 = 0, // 534
        .common_hdr_st                = 0, // 538
        .common_em_ct                 = 0, // 53c
        .common_fapa_ct               = 0, // 540
        .common_support_dscd_clken    = 0, // 544
        .common_bypass_path_ctrl      = 0,//548
        .common_hdr_em_ct2            = 0,
        .common_hdmi_vsem_emc         = 0, // 54c
        .common_hdmi_vsem_emc2        = 0,  // 550
        .common_vsem_st               = 0, //554
        .common_hdmi_emap             = 0, //558
        .common_hdmi_emdp             = 0,
        .common_hd21_hdmi_hdrap       = 0,
        .common_hd21_hdmi_hdrdp       = 0,
        .common_hd21_hdr_em_st        = 0,
        .common_hd21_hdr_em_ct        = 0,
        .common_hd21_hdr_em_ct2       = 0, //578
        .common_hd21_hdmi_emap        = 0,
        .common_hd21_hdmi_emdp        = 0,
        .common_hd21_vsem_st          = 0, //584
        .common_hd21_cvtem_st         = 0,
        .common_hd21_hdmi_vsem_emc    = 0,
        .common_hd21_hdmi_cvtem_emc   = 0,
        .common_hd21_hdmi_vsem_emc2   = 0,

        // HDMI DSCD DBG Ctrl for common port
        .common_hd21_dscd_dbg         = 0,
        .common_hd21_dscd_dbg_ap      = 0,
        .common_hd21_dscd_dbg_dp      = 0,

        // Audio Control for common port
        .common_hdmi_audcr            = 0,
        .common_hdmi_audsr            = 0,
        .common_hd21_hdmi_audcr       = 0,
        .common_hd21_hdmi_audsr       = 0,

        // HDMI Packet Status Register for common port
        .common_hdmi_gpvs             = 0,
        .common_hd21_hdmi_gpvs        = 0,

        // RM Zero Pading for common port
        .common_hd21_rmzp             = 0,

        // Video Timing Generator Status Register for common port
        .common_hd21_timing_gen_sr    = 0, // 5c8

        // One Time Register  (No Function for MAC P1,P2,P3)
        .ot                           = 0, // 600
        .irq                          = 0,
        .mp                           = 0,
        .hdmi_v_bypass                = 0,
        .hdmi_mp                      = 0,
        .debug_port                   = 0,
        .aud_common_port_switch       = 0, //61c

        // CRT
        .hdmi_rst0                    = 0,
        .hdmi_rst3                    = 0,
        .hdmi_clken0                  = 0,
        .hdmi_clken3                  = 0,
    },
    // mac 3
    {
    
        .tmds_cps                     = M8P_HDMI_P3_TMDS_CPS_reg,
        .hdmi_gpvs1                   = M8P_HDMI_P3_HDMI_GPVS1_reg,

        .hdmi_vpllcr0                 = 0,
        .hdmi_vpllcr1                 = 0,
        .hdmi_vpllcr2                 = 0,
        .mn_sclkg_ctrl                = 0,
        .hdmi_dprxpll_setting         = 0,
        .hdmi_dprxpll_setting_1       = 0,
        .hdmi_dprxpll_setting_2       = 0,
        .hdmi_dprxpll_clk_in_mux_sel  = 0,
        .mn_sclkg_divs                = 0,
        .mdd_cr                       = 0,
        .mdd_sr                       = 0,
        .fw_func                      = 0,
        .port_switch                  = 0,
        .power_saving                 = 0,
        .crc_ctrl                     = 0,
        .crc_result                   = 0,
        .des_crc                      = 0,
        .crc_err_cnt0                 = 0,
        .hdmi_20_21_ctrl              = 0,

        // YUV422_CD
        .yuv422_cd                    = 0, //528

        // HDR Extended Metadata Packet for common port  //(0xXXXXX530~0xXXXXX5C8)only for 2nd common port (p0 register), 1st common port0 still use "Per port" r/w
        .common_hdrap                 = 0, // 530
        .common_hdrdp                 = 0, // 534
        .common_hdr_st                = 0, // 538
        .common_em_ct                 = 0, // 53c
        .common_fapa_ct               = 0, // 540
        .common_support_dscd_clken    = 0, // 544
        .common_bypass_path_ctrl      = 0,//548
        .common_hdr_em_ct2            = 0, // 548
        .common_hdmi_vsem_emc         = 0, // 54c
        .common_hdmi_vsem_emc2        = 0,  // 550
        .common_vsem_st               = 0, //554
        .common_hdmi_emap             = 0, //558
        .common_hdmi_emdp             = 0,
        .common_hd21_hdmi_hdrap       = 0,
        .common_hd21_hdmi_hdrdp       = 0,
        .common_hd21_hdr_em_st        = 0,
        .common_hd21_hdr_em_ct        = 0,
        .common_hd21_hdr_em_ct2       = 0, //578
        .common_hd21_hdmi_emap        = 0,
        .common_hd21_hdmi_emdp        = 0,
        .common_hd21_vsem_st          = 0, //584
        .common_hd21_cvtem_st         = 0,
        .common_hd21_hdmi_vsem_emc    = 0,
        .common_hd21_hdmi_cvtem_emc   = 0,
        .common_hd21_hdmi_vsem_emc2   = 0,

        // HDMI DSCD DBG Ctrl for common port
        .common_hd21_dscd_dbg         = 0,
        .common_hd21_dscd_dbg_ap      = 0,
        .common_hd21_dscd_dbg_dp      = 0,

        // Audio Control for common port
        .common_hdmi_audcr            = 0,
        .common_hdmi_audsr            = 0,
        .common_hd21_hdmi_audcr       = 0,
        .common_hd21_hdmi_audsr       = 0,

        // HDMI Packet Status Register for common port
        .common_hdmi_gpvs             = 0,
        .common_hd21_hdmi_gpvs        = 0,

        // RM Zero Pading for common port
        .common_hd21_rmzp             = 0,

        // Video Timing Generator Status Register for common port
        .common_hd21_timing_gen_sr    = 0, // 5c8

        // One Time Register  (No Function for MAC P1,P3,P3)
        .ot                           = 0, // 600
        .irq                          = 0,
        .mp                           = 0,
        .hdmi_v_bypass                = 0,
        .hdmi_mp                      = 0,
        .debug_port                   = 0,
        .aud_common_port_switch       = 0, //61c

        // CRT
        .hdmi_rst0                    = 0,
        .hdmi_rst3                    = 0,
        .hdmi_clken0                  = 0,
        .hdmi_clken3                  = 0,
    },
};

/*--------------------------------------------------
 * HDMI2.1 Mac register setting
 *--------------------------------------------------*/
const struct m8p_hdmi21_mac_reg_st m8p_hd21_mac[HDMI21_MAC_NUM] = {
    // HDMI2.1 mac0
    {
        // HDMI_2p1 exist or not
        .valid = 1,

        // HDMI_2p1_control_register
        .cr = M8P_HDMI21_P0_HD21_CR_reg,

        // Video Part
        .phy_fifo_cr0 = M8P_HDMI21_P0_HD21_PHY_FIFO_CR0_reg,
        .phy_fifo_cr1 = M8P_HDMI21_P0_HD21_PHY_FIFO_CR1_reg,
        .phy_fifo_sr0 = M8P_HDMI21_P0_HD21_PHY_FIFO_SR0_reg,
        .phy_fifo_sr1 = 0,

        // Data Align
        .channel_align_c = M8P_HDMI21_P0_hd21_channel_align_c_reg,
        .channel_align_r = M8P_HDMI21_P0_hd21_channel_align_r_reg,
        .channel_align_g = M8P_HDMI21_P0_hd21_channel_align_g_reg,
        .channel_align_b = M8P_HDMI21_P0_hd21_channel_align_b_reg,

        // Channel Sync
        .ch_sync_ctrl = M8P_HDMI21_P0_hd21_ch_sync_ctrl_reg,
        .ch_sync_status = M8P_HDMI21_P0_hd21_ch_sync_status_reg,
        .ch_sync_r = M8P_HDMI21_P0_hd21_ch_sync_r_reg,
        .ch_sync_g = M8P_HDMI21_P0_hd21_ch_sync_g_reg,
        .ch_sync_b = M8P_HDMI21_P0_hd21_ch_sync_b_reg,
        .ch_sync_c = M8P_HDMI21_P0_hd21_ch_sync_c_reg,

        // Error Detect Function
        .errdet_r_ctrl = M8P_HDMI21_P0_hd21_errdet_r_ctrl_reg,
        .errdet_g_ctrl = M8P_HDMI21_P0_hd21_errdet_g_ctrl_reg,
        .errdet_b_ctrl = M8P_HDMI21_P0_hd21_errdet_b_ctrl_reg,
        .errdet_c_ctrl = M8P_HDMI21_P0_hd21_errdet_c_ctrl_reg,
        .errdet_r_disparity = M8P_HDMI21_P0_hd21_errdet_r_disparity_reg,
        .errdet_g_disparity = M8P_HDMI21_P0_hd21_errdet_g_disparity_reg,
        .errdet_b_disparity = M8P_HDMI21_P0_hd21_errdet_b_disparity_reg,
        .errdet_c_disparity = M8P_HDMI21_P0_hd21_errdet_c_disparity_reg,
        .errdet_r_char = M8P_HDMI21_P0_hd21_errdet_r_char_reg,
        .errdet_g_char = M8P_HDMI21_P0_hd21_errdet_g_char_reg,
        .errdet_b_char = M8P_HDMI21_P0_hd21_errdet_b_char_reg,
        .errdet_c_char = M8P_HDMI21_P0_hd21_errdet_c_char_reg,
        .disp_ctrl = M8P_HDMI21_P0_hd21_disp_ctrl_reg,

        // FRL Status
        .mm_cr = M8P_HDMI21_P0_HD21_MM_CR_reg,
        .mm_sr = M8P_HDMI21_P0_HD21_MM_SR_reg,

        // Extended Metadata Packet
        .hdmi_hdrap = M8P_HDMI21_P0_hd21_HDMI_HDRAP_reg,
        .hdmi_hdrdp = M8P_HDMI21_P0_hd21_HDMI_HDRDP_reg,
        .fapa_ct2 = M8P_HDMI21_P0_HD21_fapa_ct2_reg,
        .hdr_em_st = M8P_HDMI21_P0_HD21_HDR_EM_ST_reg,
        .hdr_em_ct = M8P_HDMI21_P0_HD21_HDR_EM_CT_reg,
        .hdmi_emap = M8P_HDMI21_P0_HD21_HDMI_EMAP_reg,
        .hdmi_emdp = M8P_HDMI21_P0_hd21_HDMI_EMDP_reg,
        .hdmi_emap_pp = M8P_HDMI21_P0_HD21_HDMI_EMAP_PP_reg,
        .hdmi_emdp_pp = M8P_HDMI21_P0_hd21_HDMI_EMDP_PP_reg,
        .fapa_ct = M8P_HDMI21_P0_HD21_fapa_ct_reg,
        .hdr_em_ct2 = M8P_HDMI21_P0_HD21_HDR_EM_CT2_reg,

        // Error DET
        .cercr = M8P_HDMI21_P0_hd21_cercr_reg,
        .cersr0 = M8P_HDMI21_P0_HD21_CERSR0_reg,
        .cersr1 = M8P_HDMI21_P0_HD21_CERSR1_reg,
        .cersr2 = M8P_HDMI21_P0_HD21_CERSR2_reg,

        .rmzp = M8P_HDMI21_P0_HD21_rmzp_reg,

        // Extended Metadata Packet 2
        .vsem_st = M8P_HDMI21_P0_HD21_VSEM_ST_reg,
        .vrrem_st = M8P_HDMI21_P0_HD21_VRREM_ST_reg,
        .cvtem_st = M8P_HDMI21_P0_HD21_CVTEM_ST_reg,
        .hdmi_vsem_emc = M8P_HDMI21_P0_HD21_HDMI_VSEM_EMC_reg,
        .hdmi_vrr_emc = M8P_HDMI21_P0_HD21_HDMI_VRR_EMC_reg,
        .hdmi_cvtem_emc = M8P_HDMI21_P0_HD21_HDMI_CVTEM_EMC_reg,
        .hdmi_vsem_emc2 = M8P_HDMI21_P0_HD21_HDMI_VSEM_EMC2_reg,

        // CRC Control Register
        .crc_ctrl                     = M8P_HDMI21_P0_HD21_CRC_Ctrl_reg,
        .crc_result                   = M8P_HDMI21_P0_HD21_CRC_Result_reg,
        .des_crc                      = M8P_HDMI21_P0_HD21_DES_CRC_reg,
        .crc_err_cnt0                 = M8P_HDMI21_P0_HD21_CRC_ERR_CNT0_reg,

        // Video Timing Generator
        .timing_gen_cr = M8P_HDMI21_P0_HD21_TIMING_GEN_CR_reg,
        .timing_gen_sr = M8P_HDMI21_P0_HD21_TIMING_GEN_SR_reg,

        // Channel Status
        .ch_st = M8P_HDMI21_P0_HD21_CH_ST_reg,

        // PRBS
        .prbs_r_ctrl = M8P_HDMI21_P0_HD21_PRBS_R_CTRL_reg,
        .prbs_g_ctrl = M8P_HDMI21_P0_HD21_PRBS_G_CTRL_reg,
        .prbs_b_ctrl = M8P_HDMI21_P0_HD21_PRBS_B_CTRL_reg,
        .prbs_c_ctrl = M8P_HDMI21_P0_HD21_PRBS_C_CTRL_reg,
        .prbs_r_status = M8P_HDMI21_P0_HD21_PRBS_R_STATUS_reg,
        .prbs_g_status = M8P_HDMI21_P0_HD21_PRBS_G_STATUS_reg,
        .prbs_b_status = M8P_HDMI21_P0_HD21_PRBS_B_STATUS_reg,
        .prbs_c_status = M8P_HDMI21_P0_HD21_PRBS_C_STATUS_reg,

        // DSCD_DBG
        .dscd_dbg = M8P_HDMI21_P0_hd21_DSCD_DBG_reg,
        .dscd_dbg_ap = M8P_HDMI21_P0_HD21_DSCD_DBG_AP_reg,
        .dscd_dbg_dp = M8P_HDMI21_P0_hd21_DSCD_DBG_DP_reg,

        // LINK TRAINING
        .lt_r = M8P_HDMI21_P0_HD21_LT_R_reg,
        .lt_g = M8P_HDMI21_P0_HD21_LT_G_reg,
        .lt_b = M8P_HDMI21_P0_HD21_LT_B_reg,
        .lt_c = M8P_HDMI21_P0_HD21_LT_C_reg,
        .lt_all = M8P_HDMI21_P0_HD21_LT_ALL_reg,

        // Debug Selection
        .udc = M8P_HDMI21_P0_HD21_UDC_reg,

        //Deep Color
        .tmds_dpc0 = M8P_HDMI21_P0_HD21_TMDS_DPC0_reg,
        .tmds_dpc1 = M8P_HDMI21_P0_HD21_TMDS_DPC1_reg,
        .tmds_dpc_set0 = M8P_HDMI21_P0_HD21_TMDS_DPC_SET0_reg,

        // Packet Status
        .hdmi_sr = M8P_HDMI21_P0_HD21_HDMI_SR_reg,
        .hdmi_gpvs = M8P_HDMI21_P0_HD21_HDMI_GPVS_reg,
        .hdmi_gpvs1 = M8P_HDMI21_P0_HD21_HDMI_GPVS1_reg,
        .hdmi_psap = M8P_HDMI21_P0_HD21_HDMI_PSAP_reg,
        .hdmi_psdp = M8P_HDMI21_P0_HD21_HDMI_PSDP_reg,

        // Mode Control
        .hdmi_scr = M8P_HDMI21_P0_HD21_HDMI_SCR_reg,

        // BCH Control
        .hdmi_bchcr = M8P_HDMI21_P0_HD21_HDMI_BCHCR_reg,

        // AV Mute Control
        .hdmi_avmcr = M8P_HDMI21_P0_HD21_HDMI_AVMCR_reg,
        .hdmi_wdcr = M8P_HDMI21_P0_HD21_HDMI_WDCR_reg,

        // Packet Variation
        .hdmi_pamicr = M8P_HDMI21_P0_HD21_HDMI_PAMICR_reg,
        .hdmi_ptrsv1 = M8P_HDMI21_P0_HD21_HDMI_PTRSV1_reg,
        .hdmi_ptrsv2 = M8P_HDMI21_P0_HD21_HDMI_PTRSV2_reg,
        .hdmi_ptrsv3 = M8P_HDMI21_P0_HD21_HDMI_PTRSV3_reg,
        .hdmi_ptrsv4 = M8P_HDMI21_P0_HD21_HDMI_PTRSV4_reg,
        .hdmi_ptrsv5 = M8P_HDMI21_P0_HD21_HDMI_PTRSV5_reg,
        .hdmi_ptrsv6 = M8P_HDMI21_P0_HD21_HDMI_PTRSV6_reg,
        .hdmi_pvgcr0 = M8P_HDMI21_P0_HD21_HDMI_PVGCR0_reg,
        .hdmi_pvsr0 = M8P_HDMI21_P0_HD21_HDMI_PVSR0_reg,

        // Video Flow control
        .hdmi_vcr = M8P_HDMI21_P0_HD21_HDMI_VCR_reg,

        // Audio Clock Regeneration Packet
        .hdmi_acrcr = M8P_HDMI21_P0_HD21_HDMI_ACRCR_reg,
        .hdmi_acrsr0 = M8P_HDMI21_P0_HD21_HDMI_ACRSR0_reg,
        .hdmi_acrsr1 = M8P_HDMI21_P0_HD21_HDMI_ACRSR1_reg,

        // IRQ
        .hdmi_intcr = M8P_HDMI21_P0_HD21_HDMI_INTCR_reg,

        //Packet/Guard Band Error
        .hdmi_bcsr = M8P_HDMI21_P0_HD21_HDMI_BCSR_reg,

        // Packet Content
        .hdmi_asr0 = M8P_HDMI21_P0_HD21_HDMI_ASR0_reg,
        .hdmi_asr1 = M8P_HDMI21_P0_HD21_HDMI_ASR1_reg,
        .hdmi_video_format = M8P_HDMI21_P0_HD21_HDMI_VIDEO_FORMAT_reg,
        .hdmi_3d_format = M8P_HDMI21_P0_HD21_HDMI_3D_FORMAT_reg,
        .hdmi_fvs = M8P_HDMI21_P0_HD21_HDMI_FVS_reg,
        .hdmi_drm = M8P_HDMI21_P0_HD21_HDMI_DRM_reg,
        .hdmi_pcmc = M8P_HDMI21_P0_HD21_HDMI_PCMC_reg,

        // Audio Control
        .audio_sample_rate_change_irq = M8P_HDMI21_P0_HD21_Audio_Sample_Rate_Change_IRQ_reg,
        .high_bit_rate_audio_packet = M8P_HDMI21_P0_HD21_High_Bit_Rate_Audio_Packet_reg,
        .hdmi_audcr = M8P_HDMI21_P0_HD21_HDMI_AUDCR_reg,
        .hdmi_audsr = M8P_HDMI21_P0_HD21_HDMI_AUDSR_reg,
        .audio_cts_up_bound = M8P_HDMI21_P0_HD21_AUDIO_CTS_UP_BOUND_reg,
        .audio_cts_low_bound = M8P_HDMI21_P0_HD21_AUDIO_CTS_LOW_BOUND_reg,
        .audio_n_up_bound = M8P_HDMI21_P0_HD21_AUDIO_N_UP_BOUND_reg,
        .audio_n_low_bound = M8P_HDMI21_P0_HD21_AUDIO_N_LOW_BOUND_reg,

        // CLKDET part
        .clkdetcr = M8P_HDMI21_P0_HD21_CLKDETCR_reg,
        .clkdetsr = M8P_HDMI21_P0_HD21_CLKDETSR_reg,
        .clk_setting_00 = M8P_HDMI21_P0_HD21_CLK_SETTING_00_reg,
        .clk_setting_01 = M8P_HDMI21_P0_HD21_CLK_SETTING_01_reg,
        .clk_setting_02 = M8P_HDMI21_P0_HD21_CLK_SETTING_02_reg,
        .clkdet2cr = M8P_HDMI21_P0_HD21_CLKDET2CR_reg,
        .clkdet2sr = M8P_HDMI21_P0_HD21_CLKDET2SR_reg,
        .clk2_setting_00 = M8P_HDMI21_P0_HD21_CLK2_SETTING_00_reg,
        .clk2_setting_01 = M8P_HDMI21_P0_HD21_CLK2_SETTING_01_reg,
        .clk2_setting_02 = M8P_HDMI21_P0_HD21_CLK2_SETTING_02_reg,

        // CLK High Width Measure
        .clk_hwm_setting_00 = M8P_HDMI21_P0_HD21_CLK_HWM_SETTING_00_reg,
        .clk_hwm_setting_01 = M8P_HDMI21_P0_HD21_CLK_HWM_SETTING_01_reg,
        .clk_hwm_setting_02 = M8P_HDMI21_P0_HD21_CLK_HWM_SETTING_02_reg,
        .clk_hwn_one_st_00 = M8P_HDMI21_P0_HD21_CLK_HWN_ONE_ST_00_reg,
        .clk_hwn_cont_st_01 = M8P_HDMI21_P0_HD21_CLK_HWN_CONT_ST_01_reg,

        // HDMI TMP part
        .fapa_ct_tmp = M8P_HDMI21_P0_HD21_fapa_ct_tmp_reg,
        .ps_ct = M8P_HDMI21_P0_HD21_ps_ct_reg,
        .ps_st = M8P_HDMI21_P0_HD21_ps_st_reg,
        .irq_all_status = M8P_HDMI21_P0_hd21_irq_all_status_reg,

        // hdcp status
        .hdcp_status = M8P_HDMI21_P0_hd21_hdcp_status_reg,

        // XTAL CNT
        .xtal_1s = M8P_HDMI21_P0_hd21_xtal_1s_reg,
        .xtal_280ms = M8P_HDMI21_P0_hd21_xtal_280ms_reg,
        .xtal_10ms = M8P_HDMI21_P0_hd21_xtal_10ms_reg,

        .hdmi_pkt  = M8P_HDMI21_P0_HD21_HDMI_PKT_reg,

        // Power Saving Measure
        .ps_measure_ctrl = M8P_HDMI21_P0_hd21_ps_measure_ctrl_reg,
        .ps_measure_xtal_clk0 = M8P_HDMI21_P0_hd21_ps_measure_xtal_clk0_reg,
        .ps_measure_xtal_clk1 = M8P_HDMI21_P0_hd21_ps_measure_xtal_clk1_reg,
        .ps_measure_xtal_clk2 = M8P_HDMI21_P0_hd21_ps_measure_xtal_clk2_reg,

        // Packet Absence
        .hdmi_pa_irq_en = M8P_HDMI21_P0_HD21_HDMI_PA_IRQ_EN_reg,
        .hdmi_pa_irq_sta = M8P_HDMI21_P0_HD21_HDMI_PA_IRQ_STA_reg,
        .hdmi_pa_frame_cnt = M8P_HDMI21_P0_HD21_HDMI_PA_FRAME_CNT_reg,
        .hdmi_sbtm_emc     = M8P_HDMI21_P0_HD21_HDMI_SBTM_EMC_reg,
        .sbtm_st           = M8P_HDMI21_P0_HD21_SBTM_ST_reg
    },
    // HDMI2.1 mac 1
    {
        // HDMI_2p1 exist or not
        .valid = 1,

        // HDMI_2p1_control_register
        .cr = M8P_HDMI21_P1_HD21_CR_reg,

        // Video Part
        .phy_fifo_cr0 = M8P_HDMI21_P1_HD21_PHY_FIFO_CR0_reg,
        .phy_fifo_cr1 = M8P_HDMI21_P1_HD21_PHY_FIFO_CR1_reg,
        .phy_fifo_sr0 = M8P_HDMI21_P1_HD21_PHY_FIFO_SR0_reg,
        .phy_fifo_sr1 = 0,

        // Data Align
        .channel_align_c = M8P_HDMI21_P1_hd21_channel_align_c_reg,
        .channel_align_r = M8P_HDMI21_P1_hd21_channel_align_r_reg,
        .channel_align_g = M8P_HDMI21_P1_hd21_channel_align_g_reg,
        .channel_align_b = M8P_HDMI21_P1_hd21_channel_align_b_reg,

        // Channel Sync
        .ch_sync_ctrl = M8P_HDMI21_P1_hd21_ch_sync_ctrl_reg,
        .ch_sync_status = M8P_HDMI21_P1_hd21_ch_sync_status_reg,
        .ch_sync_r = M8P_HDMI21_P1_hd21_ch_sync_r_reg,
        .ch_sync_g = M8P_HDMI21_P1_hd21_ch_sync_g_reg,
        .ch_sync_b = M8P_HDMI21_P1_hd21_ch_sync_b_reg,
        .ch_sync_c = M8P_HDMI21_P1_hd21_ch_sync_c_reg,

        // Error Detect Function
        .errdet_r_ctrl = M8P_HDMI21_P1_hd21_errdet_r_ctrl_reg,
        .errdet_g_ctrl = M8P_HDMI21_P1_hd21_errdet_g_ctrl_reg,
        .errdet_b_ctrl = M8P_HDMI21_P1_hd21_errdet_b_ctrl_reg,
        .errdet_c_ctrl = M8P_HDMI21_P1_hd21_errdet_c_ctrl_reg,
        .errdet_r_disparity = M8P_HDMI21_P1_hd21_errdet_r_disparity_reg,
        .errdet_g_disparity = M8P_HDMI21_P1_hd21_errdet_g_disparity_reg,
        .errdet_b_disparity = M8P_HDMI21_P1_hd21_errdet_b_disparity_reg,
        .errdet_c_disparity = M8P_HDMI21_P1_hd21_errdet_c_disparity_reg,
        .errdet_r_char = M8P_HDMI21_P1_hd21_errdet_r_char_reg,
        .errdet_g_char = M8P_HDMI21_P1_hd21_errdet_g_char_reg,
        .errdet_b_char = M8P_HDMI21_P1_hd21_errdet_b_char_reg,
        .errdet_c_char = M8P_HDMI21_P1_hd21_errdet_c_char_reg,
        .disp_ctrl = M8P_HDMI21_P1_hd21_disp_ctrl_reg,

        // FRL Status
        .mm_cr = M8P_HDMI21_P1_HD21_MM_CR_reg,
        .mm_sr = M8P_HDMI21_P1_HD21_MM_SR_reg,

        // Extended Metadata Packet
        .hdmi_hdrap = M8P_HDMI21_P1_hd21_HDMI_HDRAP_reg,
        .hdmi_hdrdp = M8P_HDMI21_P1_hd21_HDMI_HDRDP_reg,
        .fapa_ct2 = 0, // Only avaliable on P0
        .hdr_em_st = M8P_HDMI21_P1_HD21_HDR_EM_ST_reg,
        .hdr_em_ct = M8P_HDMI21_P1_HD21_HDR_EM_CT_reg,
        .hdmi_emap = M8P_HDMI21_P1_HD21_HDMI_EMAP_reg,
        .hdmi_emdp = M8P_HDMI21_P1_hd21_HDMI_EMDP_reg,
        .hdmi_emap_pp = M8P_HDMI21_P1_HD21_HDMI_EMAP_PP_reg,
        .hdmi_emdp_pp = M8P_HDMI21_P1_hd21_HDMI_EMDP_PP_reg,
        .fapa_ct = M8P_HDMI21_P1_HD21_fapa_ct_reg,
        .hdr_em_ct2 = M8P_HDMI21_P1_HD21_HDR_EM_CT2_reg,

        // Error DET
        .cercr = M8P_HDMI21_P1_hd21_cercr_reg,
        .cersr0 = M8P_HDMI21_P1_HD21_CERSR0_reg,
        .cersr1 = M8P_HDMI21_P1_HD21_CERSR1_reg,
        .cersr2 = M8P_HDMI21_P1_HD21_CERSR2_reg,

        .rmzp = M8P_HDMI21_P1_HD21_rmzp_reg,

        // Extended Metadata Packet 2
        .vsem_st = M8P_HDMI21_P1_HD21_VSEM_ST_reg,
        .vrrem_st = M8P_HDMI21_P1_HD21_VRREM_ST_reg,
        .cvtem_st = M8P_HDMI21_P1_HD21_CVTEM_ST_reg,
        .hdmi_vsem_emc = M8P_HDMI21_P1_HD21_HDMI_VSEM_EMC_reg,
        .hdmi_vrr_emc = M8P_HDMI21_P1_HD21_HDMI_VRR_EMC_reg,
        .hdmi_cvtem_emc = M8P_HDMI21_P1_HD21_HDMI_CVTEM_EMC_reg,
        .hdmi_vsem_emc2 = M8P_HDMI21_P1_HD21_HDMI_VSEM_EMC2_reg,

        // CRC Control Register
        .crc_ctrl                     = M8P_HDMI21_P1_HD21_CRC_Ctrl_reg,
        .crc_result                   = M8P_HDMI21_P1_HD21_CRC_Result_reg,
        .des_crc                      = M8P_HDMI21_P1_HD21_DES_CRC_reg,
        .crc_err_cnt0                 = M8P_HDMI21_P1_HD21_CRC_ERR_CNT0_reg,

        // Video Timing Generator
        .timing_gen_cr = M8P_HDMI21_P1_HD21_TIMING_GEN_CR_reg,
        .timing_gen_sr = M8P_HDMI21_P1_HD21_TIMING_GEN_SR_reg,

        // Channel Status
        .ch_st = M8P_HDMI21_P1_HD21_CH_ST_reg,

        // PRBS
        .prbs_r_ctrl = M8P_HDMI21_P1_HD21_PRBS_R_CTRL_reg,
        .prbs_g_ctrl = M8P_HDMI21_P1_HD21_PRBS_G_CTRL_reg,
        .prbs_b_ctrl = M8P_HDMI21_P1_HD21_PRBS_B_CTRL_reg,
        .prbs_c_ctrl = M8P_HDMI21_P1_HD21_PRBS_C_CTRL_reg,
        .prbs_r_status = M8P_HDMI21_P1_HD21_PRBS_R_STATUS_reg,
        .prbs_g_status = M8P_HDMI21_P1_HD21_PRBS_G_STATUS_reg,
        .prbs_b_status = M8P_HDMI21_P1_HD21_PRBS_B_STATUS_reg,
        .prbs_c_status = M8P_HDMI21_P1_HD21_PRBS_C_STATUS_reg,

        // DSCD_DBG
        .dscd_dbg = M8P_HDMI21_P1_hd21_DSCD_DBG_reg,
        .dscd_dbg_ap = M8P_HDMI21_P1_HD21_DSCD_DBG_AP_reg,
        .dscd_dbg_dp = M8P_HDMI21_P1_hd21_DSCD_DBG_DP_reg,

        // LINK TRAINING
        .lt_r = M8P_HDMI21_P1_HD21_LT_R_reg,
        .lt_g = M8P_HDMI21_P1_HD21_LT_G_reg,
        .lt_b = M8P_HDMI21_P1_HD21_LT_B_reg,
        .lt_c = M8P_HDMI21_P1_HD21_LT_C_reg,
        .lt_all = M8P_HDMI21_P1_HD21_LT_ALL_reg,

        // Debug Selection
        .udc = M8P_HDMI21_P1_HD21_UDC_reg,

        //Deep Color
        .tmds_dpc0 = M8P_HDMI21_P1_HD21_TMDS_DPC0_reg,
        .tmds_dpc1 = M8P_HDMI21_P1_HD21_TMDS_DPC1_reg,
        .tmds_dpc_set0 = M8P_HDMI21_P1_HD21_TMDS_DPC_SET0_reg,

        // Packet Status
        .hdmi_sr = M8P_HDMI21_P1_HD21_HDMI_SR_reg,
        .hdmi_gpvs = M8P_HDMI21_P1_HD21_HDMI_GPVS_reg,
        .hdmi_gpvs1 = M8P_HDMI21_P1_HD21_HDMI_GPVS1_reg,
        .hdmi_psap = M8P_HDMI21_P1_HD21_HDMI_PSAP_reg,
        .hdmi_psdp = M8P_HDMI21_P1_HD21_HDMI_PSDP_reg,

        // Mode Control
        .hdmi_scr = M8P_HDMI21_P1_HD21_HDMI_SCR_reg,

        // BCH Control
        .hdmi_bchcr = M8P_HDMI21_P1_HD21_HDMI_BCHCR_reg,

        // AV Mute Control
        .hdmi_avmcr = M8P_HDMI21_P1_HD21_HDMI_AVMCR_reg,
        .hdmi_wdcr = M8P_HDMI21_P1_HD21_HDMI_WDCR_reg,

        // Packet Variation
        .hdmi_pamicr = M8P_HDMI21_P1_HD21_HDMI_PAMICR_reg,
        .hdmi_ptrsv1 = M8P_HDMI21_P1_HD21_HDMI_PTRSV1_reg,
        .hdmi_ptrsv2 = M8P_HDMI21_P1_HD21_HDMI_PTRSV2_reg,
        .hdmi_ptrsv3 = M8P_HDMI21_P1_HD21_HDMI_PTRSV3_reg,
        .hdmi_ptrsv4 = M8P_HDMI21_P1_HD21_HDMI_PTRSV4_reg,
        .hdmi_ptrsv5 = M8P_HDMI21_P1_HD21_HDMI_PTRSV5_reg,
        .hdmi_ptrsv6 = M8P_HDMI21_P1_HD21_HDMI_PTRSV6_reg,
        .hdmi_pvgcr0 = M8P_HDMI21_P1_HD21_HDMI_PVGCR0_reg,
        .hdmi_pvsr0 = M8P_HDMI21_P1_HD21_HDMI_PVSR0_reg,

        // Video Flow control
        .hdmi_vcr = M8P_HDMI21_P1_HD21_HDMI_VCR_reg,

        // Audio Clock Regeneration Packet
        .hdmi_acrcr = M8P_HDMI21_P1_HD21_HDMI_ACRCR_reg,
        .hdmi_acrsr0 = M8P_HDMI21_P1_HD21_HDMI_ACRSR0_reg,
        .hdmi_acrsr1 = M8P_HDMI21_P1_HD21_HDMI_ACRSR1_reg,

        // IRQ
        .hdmi_intcr = M8P_HDMI21_P1_HD21_HDMI_INTCR_reg,

        //Packet/Guard Band Error
        .hdmi_bcsr = M8P_HDMI21_P1_HD21_HDMI_BCSR_reg,

        // Packet Content
        .hdmi_asr0 = M8P_HDMI21_P1_HD21_HDMI_ASR0_reg,
        .hdmi_asr1 = M8P_HDMI21_P1_HD21_HDMI_ASR1_reg,
        .hdmi_video_format = M8P_HDMI21_P1_HD21_HDMI_VIDEO_FORMAT_reg,
        .hdmi_3d_format = M8P_HDMI21_P1_HD21_HDMI_3D_FORMAT_reg,
        .hdmi_fvs = M8P_HDMI21_P1_HD21_HDMI_FVS_reg,
        .hdmi_drm = M8P_HDMI21_P1_HD21_HDMI_DRM_reg,
        .hdmi_pcmc = M8P_HDMI21_P1_HD21_HDMI_PCMC_reg,

        // Audio Control
        .audio_sample_rate_change_irq = M8P_HDMI21_P1_HD21_Audio_Sample_Rate_Change_IRQ_reg,
        .high_bit_rate_audio_packet = M8P_HDMI21_P1_HD21_High_Bit_Rate_Audio_Packet_reg,
        .hdmi_audcr = M8P_HDMI21_P1_HD21_HDMI_AUDCR_reg,
        .hdmi_audsr = M8P_HDMI21_P1_HD21_HDMI_AUDSR_reg,
        .audio_cts_up_bound = M8P_HDMI21_P1_HD21_AUDIO_CTS_UP_BOUND_reg,
        .audio_cts_low_bound = M8P_HDMI21_P1_HD21_AUDIO_CTS_LOW_BOUND_reg,
        .audio_n_up_bound = M8P_HDMI21_P1_HD21_AUDIO_N_UP_BOUND_reg,
        .audio_n_low_bound = M8P_HDMI21_P1_HD21_AUDIO_N_LOW_BOUND_reg,

        // CLKDET part
        .clkdetcr = M8P_HDMI21_P1_HD21_CLKDETCR_reg,
        .clkdetsr = M8P_HDMI21_P1_HD21_CLKDETSR_reg,
        .clk_setting_00 = M8P_HDMI21_P1_HD21_CLK_SETTING_00_reg,
        .clk_setting_01 = M8P_HDMI21_P1_HD21_CLK_SETTING_01_reg,
        .clk_setting_02 = M8P_HDMI21_P1_HD21_CLK_SETTING_02_reg,
        .clkdet2cr = M8P_HDMI21_P1_HD21_CLKDET2CR_reg,
        .clkdet2sr = M8P_HDMI21_P1_HD21_CLKDET2SR_reg,
        .clk2_setting_00 = M8P_HDMI21_P1_HD21_CLK2_SETTING_00_reg,
        .clk2_setting_01 = M8P_HDMI21_P1_HD21_CLK2_SETTING_01_reg,
        .clk2_setting_02 = M8P_HDMI21_P1_HD21_CLK2_SETTING_02_reg,

        // CLK High Width Measure
        .clk_hwm_setting_00 = M8P_HDMI21_P1_HD21_CLK_HWM_SETTING_00_reg,
        .clk_hwm_setting_01 = M8P_HDMI21_P1_HD21_CLK_HWM_SETTING_01_reg,
        .clk_hwm_setting_02 = M8P_HDMI21_P1_HD21_CLK_HWM_SETTING_02_reg,
        .clk_hwn_one_st_00 = M8P_HDMI21_P1_HD21_CLK_HWN_ONE_ST_00_reg,
        .clk_hwn_cont_st_01 = M8P_HDMI21_P1_HD21_CLK_HWN_CONT_ST_01_reg,

        // HDMI TMP part
        .fapa_ct_tmp = M8P_HDMI21_P1_HD21_fapa_ct_tmp_reg,
        .ps_ct = M8P_HDMI21_P1_HD21_ps_ct_reg,
        .ps_st = M8P_HDMI21_P1_HD21_ps_st_reg,
        .irq_all_status = M8P_HDMI21_P1_hd21_irq_all_status_reg,

        // hdcp status
        .hdcp_status = M8P_HDMI21_P1_hd21_hdcp_status_reg,

        // XTAL CNT
        .xtal_1s = M8P_HDMI21_P1_hd21_xtal_1s_reg,  // Only avaliable on P0
        .xtal_280ms = M8P_HDMI21_P1_hd21_xtal_280ms_reg,  // Only avaliable on P0
        .xtal_10ms = M8P_HDMI21_P1_hd21_xtal_10ms_reg,  // Only avaliable on P0

        .hdmi_pkt  = M8P_HDMI21_P1_HD21_HDMI_PKT_reg,

        // Power Saving Measure
        .ps_measure_ctrl = M8P_HDMI21_P1_hd21_ps_measure_ctrl_reg,
        .ps_measure_xtal_clk0 = M8P_HDMI21_P1_hd21_ps_measure_xtal_clk0_reg,
        .ps_measure_xtal_clk1 = M8P_HDMI21_P1_hd21_ps_measure_xtal_clk1_reg,
        .ps_measure_xtal_clk2 = M8P_HDMI21_P1_hd21_ps_measure_xtal_clk2_reg,

        // Packet Absence
        .hdmi_pa_irq_en = M8P_HDMI21_P1_HD21_HDMI_PA_IRQ_EN_reg,
        .hdmi_pa_irq_sta = M8P_HDMI21_P1_HD21_HDMI_PA_IRQ_STA_reg,
        .hdmi_pa_frame_cnt = M8P_HDMI21_P1_HD21_HDMI_PA_FRAME_CNT_reg,
        .hdmi_sbtm_emc     = M8P_HDMI21_P1_HD21_HDMI_SBTM_EMC_reg,
        .sbtm_st           = M8P_HDMI21_P1_HD21_SBTM_ST_reg
    },
    // HDMI2.1 mac 2
    {
        // HDMI_2p1 exist or not
        .valid = 1,

        // HDMI_2p1_control_register
        .cr = M8P_HDMI21_P2_HD21_CR_reg,

        // Video Part
        .phy_fifo_cr0 = M8P_HDMI21_P2_HD21_PHY_FIFO_CR0_reg,
        .phy_fifo_cr1 = M8P_HDMI21_P2_HD21_PHY_FIFO_CR1_reg,
        .phy_fifo_sr0 = M8P_HDMI21_P2_HD21_PHY_FIFO_SR0_reg,
        .phy_fifo_sr1 = 0,

        // Data Align
        .channel_align_c = M8P_HDMI21_P2_hd21_channel_align_c_reg,
        .channel_align_r = M8P_HDMI21_P2_hd21_channel_align_r_reg,
        .channel_align_g = M8P_HDMI21_P2_hd21_channel_align_g_reg,
        .channel_align_b = M8P_HDMI21_P2_hd21_channel_align_b_reg,

        // Channel Sync
        .ch_sync_ctrl = M8P_HDMI21_P2_hd21_ch_sync_ctrl_reg,
        .ch_sync_status = M8P_HDMI21_P2_hd21_ch_sync_status_reg,
        .ch_sync_r = M8P_HDMI21_P2_hd21_ch_sync_r_reg,
        .ch_sync_g = M8P_HDMI21_P2_hd21_ch_sync_g_reg,
        .ch_sync_b = M8P_HDMI21_P2_hd21_ch_sync_b_reg,
        .ch_sync_c = M8P_HDMI21_P2_hd21_ch_sync_c_reg,

        // Error Detect Function
        .errdet_r_ctrl = M8P_HDMI21_P2_hd21_errdet_r_ctrl_reg,
        .errdet_g_ctrl = M8P_HDMI21_P2_hd21_errdet_g_ctrl_reg,
        .errdet_b_ctrl = M8P_HDMI21_P2_hd21_errdet_b_ctrl_reg,
        .errdet_c_ctrl = M8P_HDMI21_P2_hd21_errdet_c_ctrl_reg,
        .errdet_r_disparity = M8P_HDMI21_P2_hd21_errdet_r_disparity_reg,
        .errdet_g_disparity = M8P_HDMI21_P2_hd21_errdet_g_disparity_reg,
        .errdet_b_disparity = M8P_HDMI21_P2_hd21_errdet_b_disparity_reg,
        .errdet_c_disparity = M8P_HDMI21_P2_hd21_errdet_c_disparity_reg,
        .errdet_r_char = M8P_HDMI21_P2_hd21_errdet_r_char_reg,
        .errdet_g_char = M8P_HDMI21_P2_hd21_errdet_g_char_reg,
        .errdet_b_char = M8P_HDMI21_P2_hd21_errdet_b_char_reg,
        .errdet_c_char = M8P_HDMI21_P2_hd21_errdet_c_char_reg,
        .disp_ctrl = M8P_HDMI21_P2_hd21_disp_ctrl_reg,

        // FRL Status
        .mm_cr = M8P_HDMI21_P2_HD21_MM_CR_reg,
        .mm_sr = M8P_HDMI21_P2_HD21_MM_SR_reg,

        // Extended Metadata Packet
        .hdmi_hdrap = M8P_HDMI21_P2_hd21_HDMI_HDRAP_reg,
        .hdmi_hdrdp = M8P_HDMI21_P2_hd21_HDMI_HDRDP_reg,
        .fapa_ct2 = M8P_HDMI21_P2_HD21_fapa_ct2_reg,
        .hdr_em_st = M8P_HDMI21_P2_HD21_HDR_EM_ST_reg,
        .hdr_em_ct = M8P_HDMI21_P2_HD21_HDR_EM_CT_reg,
        .hdmi_emap = M8P_HDMI21_P2_HD21_HDMI_EMAP_reg,
        .hdmi_emdp = M8P_HDMI21_P2_hd21_HDMI_EMDP_reg,
        .hdmi_emap_pp = M8P_HDMI21_P2_HD21_HDMI_EMAP_PP_reg,
        .hdmi_emdp_pp = M8P_HDMI21_P2_hd21_HDMI_EMDP_PP_reg,
        .fapa_ct = M8P_HDMI21_P2_HD21_fapa_ct_reg,
        .hdr_em_ct2 = M8P_HDMI21_P2_HD21_HDR_EM_CT2_reg,

        // Error DET
        .cercr = M8P_HDMI21_P2_hd21_cercr_reg,
        .cersr0 = M8P_HDMI21_P2_HD21_CERSR0_reg,
        .cersr1 = M8P_HDMI21_P2_HD21_CERSR1_reg,
        .cersr2 = M8P_HDMI21_P2_HD21_CERSR2_reg,

        .rmzp = M8P_HDMI21_P2_HD21_rmzp_reg,

        // Extended Metadata Packet 2
        .vsem_st = M8P_HDMI21_P2_HD21_VSEM_ST_reg,
        .vrrem_st = M8P_HDMI21_P2_HD21_VRREM_ST_reg,
        .cvtem_st = M8P_HDMI21_P2_HD21_CVTEM_ST_reg,
        .hdmi_vsem_emc = M8P_HDMI21_P2_HD21_HDMI_VSEM_EMC_reg,
        .hdmi_vrr_emc = M8P_HDMI21_P2_HD21_HDMI_VRR_EMC_reg,
        .hdmi_cvtem_emc = M8P_HDMI21_P2_HD21_HDMI_CVTEM_EMC_reg,
        .hdmi_vsem_emc2 = M8P_HDMI21_P2_HD21_HDMI_VSEM_EMC2_reg,

        // CRC Control Register
        .crc_ctrl                     = M8P_HDMI21_P2_HD21_CRC_Ctrl_reg,
        .crc_result                   = M8P_HDMI21_P2_HD21_CRC_Result_reg,
        .des_crc                      = M8P_HDMI21_P2_HD21_DES_CRC_reg,
        .crc_err_cnt0                 = M8P_HDMI21_P2_HD21_CRC_ERR_CNT0_reg,

        // Video Timing Generator
        .timing_gen_cr = M8P_HDMI21_P2_HD21_TIMING_GEN_CR_reg,
        .timing_gen_sr = M8P_HDMI21_P2_HD21_TIMING_GEN_SR_reg,

        // Channel Status
        .ch_st = M8P_HDMI21_P2_HD21_CH_ST_reg,

        // PRBS
        .prbs_r_ctrl = M8P_HDMI21_P2_HD21_PRBS_R_CTRL_reg,
        .prbs_g_ctrl = M8P_HDMI21_P2_HD21_PRBS_G_CTRL_reg,
        .prbs_b_ctrl = M8P_HDMI21_P2_HD21_PRBS_B_CTRL_reg,
        .prbs_c_ctrl = M8P_HDMI21_P2_HD21_PRBS_C_CTRL_reg,
        .prbs_r_status = M8P_HDMI21_P2_HD21_PRBS_R_STATUS_reg,
        .prbs_g_status = M8P_HDMI21_P2_HD21_PRBS_G_STATUS_reg,
        .prbs_b_status = M8P_HDMI21_P2_HD21_PRBS_B_STATUS_reg,
        .prbs_c_status = M8P_HDMI21_P2_HD21_PRBS_C_STATUS_reg,

        // DSCD_DBG
        .dscd_dbg = M8P_HDMI21_P2_hd21_DSCD_DBG_reg,
        .dscd_dbg_ap = M8P_HDMI21_P2_HD21_DSCD_DBG_AP_reg,
        .dscd_dbg_dp = M8P_HDMI21_P2_hd21_DSCD_DBG_DP_reg,

        // LINK TRAINING
        .lt_r = M8P_HDMI21_P2_HD21_LT_R_reg,
        .lt_g = M8P_HDMI21_P2_HD21_LT_G_reg,
        .lt_b = M8P_HDMI21_P2_HD21_LT_B_reg,
        .lt_c = M8P_HDMI21_P2_HD21_LT_C_reg,
        .lt_all = M8P_HDMI21_P2_HD21_LT_ALL_reg,

        // Debug Selection
        .udc = M8P_HDMI21_P2_HD21_UDC_reg,

        //Deep Color
        .tmds_dpc0 = M8P_HDMI21_P2_HD21_TMDS_DPC0_reg,
        .tmds_dpc1 = M8P_HDMI21_P2_HD21_TMDS_DPC1_reg,
        .tmds_dpc_set0 = M8P_HDMI21_P2_HD21_TMDS_DPC_SET0_reg,

        // Packet Status
        .hdmi_sr = M8P_HDMI21_P2_HD21_HDMI_SR_reg,
        .hdmi_gpvs = M8P_HDMI21_P2_HD21_HDMI_GPVS_reg,
        .hdmi_gpvs1 = M8P_HDMI21_P2_HD21_HDMI_GPVS1_reg,
        .hdmi_psap = M8P_HDMI21_P2_HD21_HDMI_PSAP_reg,
        .hdmi_psdp = M8P_HDMI21_P2_HD21_HDMI_PSDP_reg,

        // Mode Control
        .hdmi_scr = M8P_HDMI21_P2_HD21_HDMI_SCR_reg,

        // BCH Control
        .hdmi_bchcr = M8P_HDMI21_P2_HD21_HDMI_BCHCR_reg,

        // AV Mute Control
        .hdmi_avmcr = M8P_HDMI21_P2_HD21_HDMI_AVMCR_reg,
        .hdmi_wdcr = M8P_HDMI21_P2_HD21_HDMI_WDCR_reg,

        // Packet Variation
        .hdmi_pamicr = M8P_HDMI21_P2_HD21_HDMI_PAMICR_reg,
        .hdmi_ptrsv1 = M8P_HDMI21_P2_HD21_HDMI_PTRSV1_reg,
        .hdmi_ptrsv2 = M8P_HDMI21_P2_HD21_HDMI_PTRSV2_reg,
        .hdmi_ptrsv3 = M8P_HDMI21_P2_HD21_HDMI_PTRSV3_reg,
        .hdmi_ptrsv4 = M8P_HDMI21_P2_HD21_HDMI_PTRSV4_reg,
        .hdmi_ptrsv5 = M8P_HDMI21_P2_HD21_HDMI_PTRSV5_reg,
        .hdmi_ptrsv6 = M8P_HDMI21_P2_HD21_HDMI_PTRSV6_reg,
        .hdmi_pvgcr0 = M8P_HDMI21_P2_HD21_HDMI_PVGCR0_reg,
        .hdmi_pvsr0 = M8P_HDMI21_P2_HD21_HDMI_PVSR0_reg,

        // Video Flow control
        .hdmi_vcr = M8P_HDMI21_P2_HD21_HDMI_VCR_reg,

        // Audio Clock Regeneration Packet
        .hdmi_acrcr = M8P_HDMI21_P2_HD21_HDMI_ACRCR_reg,
        .hdmi_acrsr0 = M8P_HDMI21_P2_HD21_HDMI_ACRSR0_reg,
        .hdmi_acrsr1 = M8P_HDMI21_P2_HD21_HDMI_ACRSR1_reg,

        // IRQ
        .hdmi_intcr = M8P_HDMI21_P2_HD21_HDMI_INTCR_reg,

        //Packet/Guard Band Error
        .hdmi_bcsr = M8P_HDMI21_P2_HD21_HDMI_BCSR_reg,

        // Packet Content
        .hdmi_asr0 = M8P_HDMI21_P2_HD21_HDMI_ASR0_reg,
        .hdmi_asr1 = M8P_HDMI21_P2_HD21_HDMI_ASR1_reg,
        .hdmi_video_format = M8P_HDMI21_P2_HD21_HDMI_VIDEO_FORMAT_reg,
        .hdmi_3d_format = M8P_HDMI21_P2_HD21_HDMI_3D_FORMAT_reg,
        .hdmi_fvs = M8P_HDMI21_P2_HD21_HDMI_FVS_reg,
        .hdmi_drm = M8P_HDMI21_P2_HD21_HDMI_DRM_reg,
        .hdmi_pcmc = M8P_HDMI21_P2_HD21_HDMI_PCMC_reg,

        // Audio Control
        .audio_sample_rate_change_irq = M8P_HDMI21_P2_HD21_Audio_Sample_Rate_Change_IRQ_reg,
        .high_bit_rate_audio_packet = M8P_HDMI21_P2_HD21_High_Bit_Rate_Audio_Packet_reg,
        .hdmi_audcr = M8P_HDMI21_P2_HD21_HDMI_AUDCR_reg,
        .hdmi_audsr = M8P_HDMI21_P2_HD21_HDMI_AUDSR_reg,
        .audio_cts_up_bound = M8P_HDMI21_P2_HD21_AUDIO_CTS_UP_BOUND_reg,
        .audio_cts_low_bound = M8P_HDMI21_P2_HD21_AUDIO_CTS_LOW_BOUND_reg,
        .audio_n_up_bound = M8P_HDMI21_P2_HD21_AUDIO_N_UP_BOUND_reg,
        .audio_n_low_bound = M8P_HDMI21_P2_HD21_AUDIO_N_LOW_BOUND_reg,

        // CLKDET part
        .clkdetcr = M8P_HDMI21_P2_HD21_CLKDETCR_reg,
        .clkdetsr = M8P_HDMI21_P2_HD21_CLKDETSR_reg,
        .clk_setting_00 = M8P_HDMI21_P2_HD21_CLK_SETTING_00_reg,
        .clk_setting_01 = M8P_HDMI21_P2_HD21_CLK_SETTING_01_reg,
        .clk_setting_02 = M8P_HDMI21_P2_HD21_CLK_SETTING_02_reg,
        .clkdet2cr = M8P_HDMI21_P2_HD21_CLKDET2CR_reg,
        .clkdet2sr = M8P_HDMI21_P2_HD21_CLKDET2SR_reg,
        .clk2_setting_00 = M8P_HDMI21_P2_HD21_CLK2_SETTING_00_reg,
        .clk2_setting_01 = M8P_HDMI21_P2_HD21_CLK2_SETTING_01_reg,
        .clk2_setting_02 = M8P_HDMI21_P2_HD21_CLK2_SETTING_02_reg,

        // CLK High Width Measure
        .clk_hwm_setting_00 = M8P_HDMI21_P2_HD21_CLK_HWM_SETTING_00_reg,
        .clk_hwm_setting_01 = M8P_HDMI21_P2_HD21_CLK_HWM_SETTING_01_reg,
        .clk_hwm_setting_02 = M8P_HDMI21_P2_HD21_CLK_HWM_SETTING_02_reg,
        .clk_hwn_one_st_00 = M8P_HDMI21_P2_HD21_CLK_HWN_ONE_ST_00_reg,
        .clk_hwn_cont_st_01 = M8P_HDMI21_P2_HD21_CLK_HWN_CONT_ST_01_reg,

        // HDMI TMP part
        .fapa_ct_tmp = M8P_HDMI21_P2_HD21_fapa_ct_tmp_reg,
        .ps_ct = M8P_HDMI21_P2_HD21_ps_ct_reg,
        .ps_st = M8P_HDMI21_P2_HD21_ps_st_reg,
        .irq_all_status = M8P_HDMI21_P2_hd21_irq_all_status_reg,

        // hdcp status
        .hdcp_status = M8P_HDMI21_P2_hd21_hdcp_status_reg,

        // XTAL CNT
        .xtal_1s = M8P_HDMI21_P2_hd21_xtal_1s_reg,
        .xtal_280ms = M8P_HDMI21_P2_hd21_xtal_280ms_reg,
        .xtal_10ms = M8P_HDMI21_P2_hd21_xtal_10ms_reg,

        .hdmi_pkt  = M8P_HDMI21_P2_HD21_HDMI_PKT_reg,

        // Power Saving Measure
        .ps_measure_ctrl = M8P_HDMI21_P2_hd21_ps_measure_ctrl_reg,
        .ps_measure_xtal_clk0 = M8P_HDMI21_P2_hd21_ps_measure_xtal_clk0_reg,
        .ps_measure_xtal_clk1 = M8P_HDMI21_P2_hd21_ps_measure_xtal_clk1_reg,
        .ps_measure_xtal_clk2 = M8P_HDMI21_P2_hd21_ps_measure_xtal_clk2_reg,

        // Packet Absence
        .hdmi_pa_irq_en = M8P_HDMI21_P2_HD21_HDMI_PA_IRQ_EN_reg,
        .hdmi_pa_irq_sta = M8P_HDMI21_P2_HD21_HDMI_PA_IRQ_STA_reg,
        .hdmi_pa_frame_cnt = M8P_HDMI21_P2_HD21_HDMI_PA_FRAME_CNT_reg,
        .hdmi_sbtm_emc     = M8P_HDMI21_P2_HD21_HDMI_SBTM_EMC_reg,
        .sbtm_st           = M8P_HDMI21_P2_HD21_SBTM_ST_reg
    },
    // HDMI2.1 mac 3
    {
        // HDMI_2p1 exist or not
        .valid = 1,

        // HDMI_2p1_control_register
        .cr = M8P_HDMI21_P3_HD21_CR_reg,

        // Video Part
        .phy_fifo_cr0 = M8P_HDMI21_P3_HD21_PHY_FIFO_CR0_reg,
        .phy_fifo_cr1 = M8P_HDMI21_P3_HD21_PHY_FIFO_CR1_reg,
        .phy_fifo_sr0 = M8P_HDMI21_P3_HD21_PHY_FIFO_SR0_reg,
        .phy_fifo_sr1 = 0,

        // Data Align
        .channel_align_c = M8P_HDMI21_P3_hd21_channel_align_c_reg,
        .channel_align_r = M8P_HDMI21_P3_hd21_channel_align_r_reg,
        .channel_align_g = M8P_HDMI21_P3_hd21_channel_align_g_reg,
        .channel_align_b = M8P_HDMI21_P3_hd21_channel_align_b_reg,

        // Channel Sync
        .ch_sync_ctrl = M8P_HDMI21_P3_hd21_ch_sync_ctrl_reg,
        .ch_sync_status = M8P_HDMI21_P3_hd21_ch_sync_status_reg,
        .ch_sync_r = M8P_HDMI21_P3_hd21_ch_sync_r_reg,
        .ch_sync_g = M8P_HDMI21_P3_hd21_ch_sync_g_reg,
        .ch_sync_b = M8P_HDMI21_P3_hd21_ch_sync_b_reg,
        .ch_sync_c = M8P_HDMI21_P3_hd21_ch_sync_c_reg,

        // Error Detect Function
        .errdet_r_ctrl = M8P_HDMI21_P3_hd21_errdet_r_ctrl_reg,
        .errdet_g_ctrl = M8P_HDMI21_P3_hd21_errdet_g_ctrl_reg,
        .errdet_b_ctrl = M8P_HDMI21_P3_hd21_errdet_b_ctrl_reg,
        .errdet_c_ctrl = M8P_HDMI21_P3_hd21_errdet_c_ctrl_reg,
        .errdet_r_disparity = M8P_HDMI21_P3_hd21_errdet_r_disparity_reg,
        .errdet_g_disparity = M8P_HDMI21_P3_hd21_errdet_g_disparity_reg,
        .errdet_b_disparity = M8P_HDMI21_P3_hd21_errdet_b_disparity_reg,
        .errdet_c_disparity = M8P_HDMI21_P3_hd21_errdet_c_disparity_reg,
        .errdet_r_char = M8P_HDMI21_P3_hd21_errdet_r_char_reg,
        .errdet_g_char = M8P_HDMI21_P3_hd21_errdet_g_char_reg,
        .errdet_b_char = M8P_HDMI21_P3_hd21_errdet_b_char_reg,
        .errdet_c_char = M8P_HDMI21_P3_hd21_errdet_c_char_reg,
        .disp_ctrl = M8P_HDMI21_P3_hd21_disp_ctrl_reg,

        // FRL Status
        .mm_cr = M8P_HDMI21_P3_HD21_MM_CR_reg,
        .mm_sr = M8P_HDMI21_P3_HD21_MM_SR_reg,

        // Extended Metadata Packet
        .hdmi_hdrap = M8P_HDMI21_P3_hd21_HDMI_HDRAP_reg,
        .hdmi_hdrdp = M8P_HDMI21_P3_hd21_HDMI_HDRDP_reg,
        .fapa_ct2 = M8P_HDMI21_P3_HD21_fapa_ct2_reg,
        .hdr_em_st = M8P_HDMI21_P3_HD21_HDR_EM_ST_reg,
        .hdr_em_ct = M8P_HDMI21_P3_HD21_HDR_EM_CT_reg,
        .hdmi_emap = M8P_HDMI21_P3_HD21_HDMI_EMAP_reg,
        .hdmi_emdp = M8P_HDMI21_P3_hd21_HDMI_EMDP_reg,
        .hdmi_emap_pp = M8P_HDMI21_P3_HD21_HDMI_EMAP_PP_reg,
        .hdmi_emdp_pp = M8P_HDMI21_P3_hd21_HDMI_EMDP_PP_reg,
        .fapa_ct = M8P_HDMI21_P3_HD21_fapa_ct_reg,
        .hdr_em_ct2 = M8P_HDMI21_P3_HD21_HDR_EM_CT2_reg,

        // Error DET
        .cercr = M8P_HDMI21_P3_hd21_cercr_reg,
        .cersr0 = M8P_HDMI21_P3_HD21_CERSR0_reg,
        .cersr1 = M8P_HDMI21_P3_HD21_CERSR1_reg,
        .cersr2 = M8P_HDMI21_P3_HD21_CERSR2_reg,

        .rmzp = M8P_HDMI21_P3_HD21_rmzp_reg,

        // Extended Metadata Packet 2
        .vsem_st = M8P_HDMI21_P3_HD21_VSEM_ST_reg,
        .vrrem_st = M8P_HDMI21_P3_HD21_VRREM_ST_reg,
        .cvtem_st = M8P_HDMI21_P3_HD21_CVTEM_ST_reg,
        .hdmi_vsem_emc = M8P_HDMI21_P3_HD21_HDMI_VSEM_EMC_reg,
        .hdmi_vrr_emc = M8P_HDMI21_P3_HD21_HDMI_VRR_EMC_reg,
        .hdmi_cvtem_emc = M8P_HDMI21_P3_HD21_HDMI_CVTEM_EMC_reg,
        .hdmi_vsem_emc2 = M8P_HDMI21_P3_HD21_HDMI_VSEM_EMC2_reg,

        // CRC Control Register
        .crc_ctrl                     = M8P_HDMI21_P3_HD21_CRC_Ctrl_reg,
        .crc_result                   = M8P_HDMI21_P3_HD21_CRC_Result_reg,
        .des_crc                      = M8P_HDMI21_P3_HD21_DES_CRC_reg,
        .crc_err_cnt0                 = M8P_HDMI21_P3_HD21_CRC_ERR_CNT0_reg,

        // Video Timing Generator
        .timing_gen_cr = M8P_HDMI21_P3_HD21_TIMING_GEN_CR_reg,
        .timing_gen_sr = M8P_HDMI21_P3_HD21_TIMING_GEN_SR_reg,

        // Channel Status
        .ch_st = M8P_HDMI21_P3_HD21_CH_ST_reg,

        // PRBS
        .prbs_r_ctrl = M8P_HDMI21_P3_HD21_PRBS_R_CTRL_reg,
        .prbs_g_ctrl = M8P_HDMI21_P3_HD21_PRBS_G_CTRL_reg,
        .prbs_b_ctrl = M8P_HDMI21_P3_HD21_PRBS_B_CTRL_reg,
        .prbs_c_ctrl = M8P_HDMI21_P3_HD21_PRBS_C_CTRL_reg,
        .prbs_r_status = M8P_HDMI21_P3_HD21_PRBS_R_STATUS_reg,
        .prbs_g_status = M8P_HDMI21_P3_HD21_PRBS_G_STATUS_reg,
        .prbs_b_status = M8P_HDMI21_P3_HD21_PRBS_B_STATUS_reg,
        .prbs_c_status = M8P_HDMI21_P3_HD21_PRBS_C_STATUS_reg,

        // DSCD_DBG
        .dscd_dbg = M8P_HDMI21_P3_hd21_DSCD_DBG_reg,
        .dscd_dbg_ap = M8P_HDMI21_P3_HD21_DSCD_DBG_AP_reg,
        .dscd_dbg_dp = M8P_HDMI21_P3_hd21_DSCD_DBG_DP_reg,

        // LINK TRAINING
        .lt_r = M8P_HDMI21_P3_HD21_LT_R_reg,
        .lt_g = M8P_HDMI21_P3_HD21_LT_G_reg,
        .lt_b = M8P_HDMI21_P3_HD21_LT_B_reg,
        .lt_c = M8P_HDMI21_P3_HD21_LT_C_reg,
        .lt_all = M8P_HDMI21_P3_HD21_LT_ALL_reg,

        // Debug Selection
        .udc = M8P_HDMI21_P3_HD21_UDC_reg,

        //Deep Color
        .tmds_dpc0 = M8P_HDMI21_P3_HD21_TMDS_DPC0_reg,
        .tmds_dpc1 = M8P_HDMI21_P3_HD21_TMDS_DPC1_reg,
        .tmds_dpc_set0 = M8P_HDMI21_P3_HD21_TMDS_DPC_SET0_reg,

        // Packet Status
        .hdmi_sr = M8P_HDMI21_P3_HD21_HDMI_SR_reg,
        .hdmi_gpvs = M8P_HDMI21_P3_HD21_HDMI_GPVS_reg,
        .hdmi_gpvs1 = M8P_HDMI21_P3_HD21_HDMI_GPVS1_reg,
        .hdmi_psap = M8P_HDMI21_P3_HD21_HDMI_PSAP_reg,
        .hdmi_psdp = M8P_HDMI21_P3_HD21_HDMI_PSDP_reg,

        // Mode Control
        .hdmi_scr = M8P_HDMI21_P3_HD21_HDMI_SCR_reg,

        // BCH Control
        .hdmi_bchcr = M8P_HDMI21_P3_HD21_HDMI_BCHCR_reg,

        // AV Mute Control
        .hdmi_avmcr = M8P_HDMI21_P3_HD21_HDMI_AVMCR_reg,
        .hdmi_wdcr = M8P_HDMI21_P3_HD21_HDMI_WDCR_reg,

        // Packet Variation
        .hdmi_pamicr = M8P_HDMI21_P3_HD21_HDMI_PAMICR_reg,
        .hdmi_ptrsv1 = M8P_HDMI21_P3_HD21_HDMI_PTRSV1_reg,
        .hdmi_ptrsv2 = M8P_HDMI21_P3_HD21_HDMI_PTRSV2_reg,
        .hdmi_ptrsv3 = M8P_HDMI21_P3_HD21_HDMI_PTRSV3_reg,
        .hdmi_ptrsv4 = M8P_HDMI21_P3_HD21_HDMI_PTRSV4_reg,
        .hdmi_ptrsv5 = M8P_HDMI21_P3_HD21_HDMI_PTRSV5_reg,
        .hdmi_ptrsv6 = M8P_HDMI21_P3_HD21_HDMI_PTRSV6_reg,
        .hdmi_pvgcr0 = M8P_HDMI21_P3_HD21_HDMI_PVGCR0_reg,
        .hdmi_pvsr0 = M8P_HDMI21_P3_HD21_HDMI_PVSR0_reg,

        // Video Flow control
        .hdmi_vcr = M8P_HDMI21_P3_HD21_HDMI_VCR_reg,

        // Audio Clock Regeneration Packet
        .hdmi_acrcr = M8P_HDMI21_P3_HD21_HDMI_ACRCR_reg,
        .hdmi_acrsr0 = M8P_HDMI21_P3_HD21_HDMI_ACRSR0_reg,
        .hdmi_acrsr1 = M8P_HDMI21_P3_HD21_HDMI_ACRSR1_reg,

        // IRQ
        .hdmi_intcr = M8P_HDMI21_P3_HD21_HDMI_INTCR_reg,

        //Packet/Guard Band Error
        .hdmi_bcsr = M8P_HDMI21_P3_HD21_HDMI_BCSR_reg,

        // Packet Content
        .hdmi_asr0 = M8P_HDMI21_P3_HD21_HDMI_ASR0_reg,
        .hdmi_asr1 = M8P_HDMI21_P3_HD21_HDMI_ASR1_reg,
        .hdmi_video_format = M8P_HDMI21_P3_HD21_HDMI_VIDEO_FORMAT_reg,
        .hdmi_3d_format = M8P_HDMI21_P3_HD21_HDMI_3D_FORMAT_reg,
        .hdmi_fvs = M8P_HDMI21_P3_HD21_HDMI_FVS_reg,
        .hdmi_drm = M8P_HDMI21_P3_HD21_HDMI_DRM_reg,
        .hdmi_pcmc = M8P_HDMI21_P3_HD21_HDMI_PCMC_reg,

        // Audio Control
        .audio_sample_rate_change_irq = M8P_HDMI21_P3_HD21_Audio_Sample_Rate_Change_IRQ_reg,
        .high_bit_rate_audio_packet = M8P_HDMI21_P3_HD21_High_Bit_Rate_Audio_Packet_reg,
        .hdmi_audcr = M8P_HDMI21_P3_HD21_HDMI_AUDCR_reg,
        .hdmi_audsr = M8P_HDMI21_P3_HD21_HDMI_AUDSR_reg,
        .audio_cts_up_bound = M8P_HDMI21_P3_HD21_AUDIO_CTS_UP_BOUND_reg,
        .audio_cts_low_bound = M8P_HDMI21_P3_HD21_AUDIO_CTS_LOW_BOUND_reg,
        .audio_n_up_bound = M8P_HDMI21_P3_HD21_AUDIO_N_UP_BOUND_reg,
        .audio_n_low_bound = M8P_HDMI21_P3_HD21_AUDIO_N_LOW_BOUND_reg,

        // CLKDET part
        .clkdetcr = M8P_HDMI21_P3_HD21_CLKDETCR_reg,
        .clkdetsr = M8P_HDMI21_P3_HD21_CLKDETSR_reg,
        .clk_setting_00 = M8P_HDMI21_P3_HD21_CLK_SETTING_00_reg,
        .clk_setting_01 = M8P_HDMI21_P3_HD21_CLK_SETTING_01_reg,
        .clk_setting_02 = M8P_HDMI21_P3_HD21_CLK_SETTING_02_reg,
        .clkdet2cr = M8P_HDMI21_P3_HD21_CLKDET2CR_reg,
        .clkdet2sr = M8P_HDMI21_P3_HD21_CLKDET2SR_reg,
        .clk2_setting_00 = M8P_HDMI21_P3_HD21_CLK2_SETTING_00_reg,
        .clk2_setting_01 = M8P_HDMI21_P3_HD21_CLK2_SETTING_01_reg,
        .clk2_setting_02 = M8P_HDMI21_P3_HD21_CLK2_SETTING_02_reg,

        // CLK High Width Measure
        .clk_hwm_setting_00 = M8P_HDMI21_P3_HD21_CLK_HWM_SETTING_00_reg,
        .clk_hwm_setting_01 = M8P_HDMI21_P3_HD21_CLK_HWM_SETTING_01_reg,
        .clk_hwm_setting_02 = M8P_HDMI21_P3_HD21_CLK_HWM_SETTING_02_reg,
        .clk_hwn_one_st_00 = M8P_HDMI21_P3_HD21_CLK_HWN_ONE_ST_00_reg,
        .clk_hwn_cont_st_01 = M8P_HDMI21_P3_HD21_CLK_HWN_CONT_ST_01_reg,

        // HDMI TMP part
        .fapa_ct_tmp = M8P_HDMI21_P3_HD21_fapa_ct_tmp_reg,
        .ps_ct = M8P_HDMI21_P3_HD21_ps_ct_reg,
        .ps_st = M8P_HDMI21_P3_HD21_ps_st_reg,
        .irq_all_status = M8P_HDMI21_P3_hd21_irq_all_status_reg,

        // hdcp status
        .hdcp_status = M8P_HDMI21_P3_hd21_hdcp_status_reg,

        // XTAL CNT
        .xtal_1s = M8P_HDMI21_P3_hd21_xtal_1s_reg,
        .xtal_280ms = M8P_HDMI21_P3_hd21_xtal_280ms_reg,
        .xtal_10ms = M8P_HDMI21_P3_hd21_xtal_10ms_reg,

        .hdmi_pkt  = M8P_HDMI21_P3_HD21_HDMI_PKT_reg,

        // Power Saving Measure
        .ps_measure_ctrl = M8P_HDMI21_P3_hd21_ps_measure_ctrl_reg,
        .ps_measure_xtal_clk0 = M8P_HDMI21_P3_hd21_ps_measure_xtal_clk0_reg,
        .ps_measure_xtal_clk1 = M8P_HDMI21_P3_hd21_ps_measure_xtal_clk1_reg,
        .ps_measure_xtal_clk2 = M8P_HDMI21_P3_hd21_ps_measure_xtal_clk2_reg,

        // Packet Absence
        .hdmi_pa_irq_en = M8P_HDMI21_P3_HD21_HDMI_PA_IRQ_EN_reg,
        .hdmi_pa_irq_sta = M8P_HDMI21_P3_HD21_HDMI_PA_IRQ_STA_reg,
        .hdmi_pa_frame_cnt = M8P_HDMI21_P3_HD21_HDMI_PA_FRAME_CNT_reg,
        .hdmi_sbtm_emc     = M8P_HDMI21_P3_HD21_HDMI_SBTM_EMC_reg,
        .sbtm_st           = M8P_HDMI21_P3_HD21_SBTM_ST_reg
    }
};

/*--------------------------------------------------
 * HDMI2.1 MEAS register setting
 *--------------------------------------------------*/
const struct m8p_hdmi21_meas_reg_st m8p_hd21_meas[M8P_HDMI21_MAC_NUM] = {
    // HDMI2.1 MEAS0
    {
        .frl_measure_0 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI21_P0_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI21_P0_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI21_P0_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI21_P0_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI21_P0_MEAS_FRL_MEASURE_18_reg
    },
    // HDMI2.1 MEAS1
    {
        .frl_measure_0 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI21_P1_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI21_P1_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI21_P1_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI21_P1_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI21_P1_MEAS_FRL_MEASURE_18_reg
    },
    // HDMI2.1 MEAS2
    {
        .frl_measure_0 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI21_P2_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI21_P2_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI21_P2_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI21_P2_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI21_P2_MEAS_FRL_MEASURE_18_reg
    },
    // HDMI2.1 MEAS3
    {
        .frl_measure_0 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI21_P3_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI21_P3_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI21_P3_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI21_P3_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI21_P3_MEAS_FRL_MEASURE_18_reg
    }
};

/*--------------------------------------------------
 * HDMI2.0 MEAS register setting
 *--------------------------------------------------*/
const struct m8p_hdmi20_meas_reg_st m8p_hd20_meas[M8P_HDMI20_MAC_NUM] = {
    // HDMI2.0 MEAS0
    {
        .frl_measure_0 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI20_P0_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI20_P0_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI20_P0_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI20_P0_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI20_P0_MEAS_FRL_MEASURE_18_reg
    },
    // HDMI2.0 MEAS1
    {
        .frl_measure_0 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI20_P1_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI20_P1_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI20_P1_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI20_P1_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI20_P1_MEAS_FRL_MEASURE_18_reg
    },
    // HDMI2.0 MEAS2
    {
        .frl_measure_0 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI20_P2_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI20_P2_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI20_P2_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI20_P2_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI20_P2_MEAS_FRL_MEASURE_18_reg
    },
    // HDMI2.0 MEAS3
    {
        .frl_measure_0 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_0_reg,
        .frl_measure_1 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_1_reg,
        .frl_measure_3 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_3_reg,
        .frl_measure_4 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_4_reg,
        .frl_measure_5 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_5_reg,
        .frl_measure_7 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_7_reg,
        .frl_measure_9 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_9_reg,
        .frl_measure_a = M8P_HDMI20_P3_MEAS_FRL_MEASURE_A_reg,
        .frl_measure_b = M8P_HDMI20_P3_MEAS_FRL_MEASURE_B_reg,
        .frl_measure_d = M8P_HDMI20_P3_MEAS_FRL_MEASURE_D_reg,
        .frl_measure_f = M8P_HDMI20_P3_MEAS_FRL_MEASURE_F_reg,
        .frl_measure_11 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_11_reg,
        .frl_measure_13 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_13_reg,
        .frl_measure_14 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_14_reg,
        .frl_measure_16 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_16_reg,
        .frl_measure_18 = M8P_HDMI20_P3_MEAS_FRL_MEASURE_18_reg
    }
};

