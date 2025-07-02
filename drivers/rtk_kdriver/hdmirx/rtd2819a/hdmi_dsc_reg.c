/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2024
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file:        hdmi_dsc_reg.c
 *
 * author:
 * date:
 * version:     1.0
 *
 *============================================================*/

/*========================Header Files============================*/
#include "hdmi_dsc_reg.h"
#include "rbus/dscd_reg.h"
#include "rbus/dscd2_reg.h"

const struct DSCD_reg_st dscd[DSCD_NUM] = {
    // dscd 1
    {
        .dscd_pps_dw00                                             = DSCD_PPS_DW00_reg,
        .dscd_pps_dw01                                             = DSCD_PPS_DW01_reg,
        .dscd_pps_dw02                                             = DSCD_PPS_DW02_reg,
        .dscd_pps_dw03                                             = DSCD_PPS_DW03_reg,
        .dscd_pps_dw04                                             = DSCD_PPS_DW04_reg,
        .dscd_pps_dw05                                             = DSCD_PPS_DW05_reg,
        .dscd_pps_dw06                                             = DSCD_PPS_DW06_reg,
        .dscd_pps_dw07                                             = DSCD_PPS_DW07_reg,
        .dscd_pps_dw08                                             = DSCD_PPS_DW08_reg,
        .dscd_pps_dw09                                             = DSCD_PPS_DW09_reg,
        .dscd_pps_dw10                                             = DSCD_PPS_DW10_reg,
        .dscd_pps_dw11                                             = DSCD_PPS_DW11_reg,
        .dscd_pps_dw12                                             = DSCD_PPS_DW12_reg,
        .dscd_pps_dw13                                             = DSCD_PPS_DW13_reg,
        .dscd_pps_dw14                                             = DSCD_PPS_DW14_reg,
        .dscd_pps_dw15                                             = DSCD_PPS_DW15_reg,
        .dscd_pps_dw16                                             = DSCD_PPS_DW16_reg,
        .dscd_pps_dw17                                             = DSCD_PPS_DW17_reg,
        .dscd_pps_dw18                                             = DSCD_PPS_DW18_reg,
        .dscd_pps_dw19                                             = DSCD_PPS_DW19_reg,
        .dscd_pps_dw20                                             = DSCD_PPS_DW20_reg,
        .dscd_pps_dw21                                             = DSCD_PPS_DW21_reg,
        .dscd_pps_dw22                                             = DSCD_PPS_DW22_reg,
        .dscd_pps_dw23                                             = DSCD_PPS_DW23_reg,
        .dscd_pps_dw24                                             = DSCD_PPS_DW24_reg,
        .dscd_pps_dw25                                             = DSCD_PPS_DW25_reg,
        .dscd_pps_dw26                                             = DSCD_PPS_DW26_reg,
        .dscd_pps_dw27                                             = DSCD_PPS_DW27_reg,
        .dscd_pps_dw28                                             = DSCD_PPS_DW28_reg,
        .dscd_pps_dw29                                             = DSCD_PPS_DW29_reg,
        .dscd_pps_dw30                                             = DSCD_PPS_DW30_reg,
        .dscd_pps_dw31                                             = DSCD_PPS_DW31_reg,
        .dscd_pps_dw32                                             = DSCD_PPS_DW32_reg,
        .dscd_pps_dw33                                             = DSCD_PPS_DW33_reg,

        .dscd_pps_dw00_cur                                      = DSCD_PPS_DW00_CUR_reg,
        .dscd_pps_dw01_cur                                      = DSCD_PPS_DW01_CUR_reg,
        .dscd_pps_dw02_cur                                      = DSCD_PPS_DW02_CUR_reg,
        .dscd_pps_dw03_cur                                      = DSCD_PPS_DW03_CUR_reg,
        .dscd_pps_dw04_cur                                      = DSCD_PPS_DW04_CUR_reg,
        .dscd_pps_dw05_cur                                      = DSCD_PPS_DW05_CUR_reg,
        .dscd_pps_dw06_cur                                      = DSCD_PPS_DW06_CUR_reg,
        .dscd_pps_dw07_cur                                      = DSCD_PPS_DW07_CUR_reg,
        .dscd_pps_dw08_cur                                      = DSCD_PPS_DW08_CUR_reg,
        .dscd_pps_dw09_cur                                      = DSCD_PPS_DW09_CUR_reg,
        .dscd_pps_dw10_cur                                      = DSCD_PPS_DW10_CUR_reg,
        .dscd_pps_dw11_cur                                      = DSCD_PPS_DW11_CUR_reg,
        .dscd_pps_dw12_cur                                      = DSCD_PPS_DW12_CUR_reg,
        .dscd_pps_dw13_cur                                      = DSCD_PPS_DW13_CUR_reg,
        .dscd_pps_dw14_cur                                      = DSCD_PPS_DW14_CUR_reg,
        .dscd_pps_dw15_cur                                      = DSCD_PPS_DW15_CUR_reg,
        .dscd_pps_dw16_cur                                      = DSCD_PPS_DW16_CUR_reg,
        .dscd_pps_dw17_cur                                      = DSCD_PPS_DW17_CUR_reg,
        .dscd_pps_dw18_cur                                      = DSCD_PPS_DW18_CUR_reg,
        .dscd_pps_dw19_cur                                      = DSCD_PPS_DW19_CUR_reg,
        .dscd_pps_dw20_cur                                      = DSCD_PPS_DW20_CUR_reg,
        .dscd_pps_dw21_cur                                      = DSCD_PPS_DW21_CUR_reg,
        .dscd_pps_dw22_cur                                      = DSCD_PPS_DW22_CUR_reg,
        .dscd_pps_dw23_cur                                      = DSCD_PPS_DW23_CUR_reg,
        .dscd_pps_dw24_cur                                      = DSCD_PPS_DW24_CUR_reg,
        .dscd_pps_dw25_cur                                      = DSCD_PPS_DW25_CUR_reg,
        .dscd_pps_dw26_cur                                      = DSCD_PPS_DW26_CUR_reg,
        .dscd_pps_dw27_cur                                      = DSCD_PPS_DW27_CUR_reg,
        .dscd_pps_dw28_cur                                      = DSCD_PPS_DW28_CUR_reg,
        .dscd_pps_dw29_cur                                      = DSCD_PPS_DW29_CUR_reg,
        .dscd_pps_dw30_cur                                      = DSCD_PPS_DW30_CUR_reg,
        .dscd_pps_dw31_cur                                      = DSCD_PPS_DW31_CUR_reg,
        .dscd_pps_dw32_cur                                      = DSCD_PPS_DW32_CUR_reg,
        .dscd_pps_dw33_cur                                      = DSCD_PPS_DW33_CUR_reg,

        // dscd control_register
        .dscd_db_ctrl                                             = DSCD_DB_CTRL_reg,
        .dscd_ctrl                                                = DSCD_CTRL_reg,

        // dscd status
        .dscd_status                                              = DSCD_STATUS_reg,

        .dscd_wde_dscd                                             = DSCD_WDE_DSCD_reg,
        .dscd_inte_dscd                                             = DSCD_INTE_DSCD_reg,
        .dscd_ints_dscd                                             = DSCD_INTS_DSCD_reg,
        .dscd_dummy                                             = DSCD_dummy_reg,
        .dscd_debug                                             = DSCD_debug_reg,
        .dscd_performance                                             = DSCD_performance_reg,


        .dscd_inte_dscd_2                                             = DSCD_INTE_DSCD_2_reg,
        .dscd_ints_dscd_2                                             = DSCD_INTS_DSCD_2_reg,

        .dscd_out_iv_delay                                             = DSCD_OUT_IV_DELAY_reg,
        .dscd_out_vs_width                                             = DSCD_OUT_VS_WIDTH_reg,

        .dscd_out_den_delay                                             = DSCD_OUT_DEN_DELAY_reg,

        .dscd_out_swap                                             = DSCD_OUT_SWAP_reg,

        .dscd_gt_cycle_measure_ctrl                                             = DSCD_GT_CYCLE_MEASURE_CTRL_reg,
        .dscd_gt_cycle_measure_result_0                                         = DSCD_GT_CYCLE_MEASURE_RESULT_0_reg,
        .dscd_gt_cycle_measure_result_1                                         = DSCD_GT_CYCLE_MEASURE_RESULT_1_reg,
        .dscd_gt_cycle_measure_result_2                                         = DSCD_GT_CYCLE_MEASURE_RESULT_2_reg,
        .dscd_out_measure                                                       = DSCD_OUT_MEASURE_reg,


        .dscd_out_measure_outbuf1                                             = DSCD_OUT_MEASURE_OUTBUF1_reg,

        .dscd_dummy2                                             = DSCD_dummy2_reg,
        .dscd_dummy3                                             = DSCD_dummy3_reg,

        .dscd_timing_monitor_ctrl                                             = DSCD_DSCD_Timing_monitor_ctrl_reg,
        .dscd_timing_monitor_multi                                            = DSCD_DSCD_Timing_monitor_multi_reg,
        .dscd_timing_monitor_compare_th                                             = DSCD_DSCD_Timing_monitor_compare_th_reg,
        .dscd_timing_monitor_golden                                             = DSCD_DSCD_Timing_monitor_golden_reg,
        .dscd_timing_monitor_result1                                             = DSCD_DSCD_Timing_monitor_result1_reg,
        .dscd_timing_monitor_result2                                             = DSCD_DSCD_Timing_monitor_result2_reg,
        .dscd_timing_monitor_result3                                             = DSCD_DSCD_Timing_monitor_result3_reg,
        .dscd_timing_monitor_result4                                             = DSCD_DSCD_Timing_monitor_result4_reg,
        .dscd_timing_monitor_max                                             = DSCD_DSCD_Timing_monitor_max_reg,

        .dscd_timing_monitor_min                                             = DSCD_DSCD_Timing_monitor_min_reg,

        .dscd_timing_monitor_st                                             = DSCD_DSCD_Timing_monitor_st_reg,

        .dscd_out_crc_en                                             = DSCD_DSCD_out_crc_en_reg,
        .dscd_out_crc_result                                             = DSCD_DSCD_out_crc_result_reg,
        .dscd_in_crc_en                                             = DSCD_Dscd_in_crc_en_reg,
        .dscd_in_crc_result                                             = DSCD_Dscd_in_crc_result_reg,
        .dscd_pps_crc_en                                             = DSCD_DSCD_pps_crc_en_reg,
        .dscd_pps_crc_result                                             = DSCD_DSCD_pps_crc_result_reg,
        .dscd_ext_crc_result                                             = DSCD_DSCD_ext_crc_result_reg,
        .dscd_dbg_en                                             = DSCD_DSCD_dbg_en_reg,
        .dscd_dbg1                                             = DSCD_DSCD_dbg1_reg,


        .dscd_dbg1_size                                             = DSCD_DSCD_dbg1_size_reg,
        .dscd_dbg_sram                                             = DSCD_DSCD_dbg_sram_reg,
        .dscd_sram_d0                                             = DSCD_DSCD_sram_d0_reg,
        .dscd_sram_d1                                             = DSCD_DSCD_sram_d1_reg,

        .dscd_sram_d2                                             = DSCD_DSCD_sram_d2_reg,
        .dscd_sram_d3                                             = DSCD_DSCD_sram_d3_reg,
        .dscd_sram_d4                                             = DSCD_DSCD_sram_d4_reg,
        .dscd_sram_d5                                             = DSCD_DSCD_sram_d5_reg,
        .dscd_sram_d6                                             = DSCD_DSCD_sram_d6_reg,
        .dscd_sram_d7                                             = DSCD_DSCD_sram_d7_reg,
        .dscd_dbg_sram_rw                                             = DSCD_DSCD_dbg_sram_rw_reg,
        .dscd_sram_q0                                             = DSCD_DSCD_sram_q0_reg,
        .dscd_sram_q1                                             = DSCD_DSCD_sram_q1_reg,
        .dscd_sram_q2                                             = DSCD_DSCD_sram_q2_reg,
        .dscd_sram_q3                                             = DSCD_DSCD_sram_q3_reg,
        .dscd_sram_q4                                             = DSCD_DSCD_sram_q4_reg,
        .dscd_sram_q5                                             = DSCD_DSCD_sram_q5_reg,
        .dscd_sram_q6                                             = DSCD_DSCD_sram_q6_reg,
        .dscd_sram_q7                                             = DSCD_DSCD_sram_q7_reg,
        .dscd_sram_q8                                             = DSCD_DSCD_sram_q8_reg,
        .dscd_sram_q9                                             = DSCD_DSCD_sram_q9_reg,
        .dscd_sram_q10                                             = DSCD_DSCD_sram_q10_reg,
        .dscd_sram_q11                                             = DSCD_DSCD_sram_q11_reg,

        .dscd_timing_gen_ctrl                                             = DSCD_DSCD_TIMING_GEN_CTRL_reg,
        .dscd_timing_gen_sta                                             = DSCD_DSCD_TIMING_GEN_STA_reg,
        .dscd_timing_gen_size                                             = DSCD_DSCD_TIMING_GEN_SIZE_reg,
        .dscd_timing_gen_end                                             = DSCD_DSCD_TIMING_GEN_END_reg,
        .dscd_timing_gen_frame_rate                                             = DSCD_DSCD_TIMING_GEN_FRAME_RATE_reg,

        .dscd_pattern_gen_ctrl                                             = DSCD_DSCD_PATTERN_GEN_CTRL_reg,
        .dscd_pattern_gen_c0_rgb                                             = DSCD_DSCD_PATTERN_GEN_C0_RGB_reg,
        .dscd_pattern_gen_c1_rgb                                             = DSCD_DSCD_PATTERN_GEN_C1_RGB_reg,
        .dscd_pattern_gen_c2_rgb                                             = DSCD_DSCD_PATTERN_GEN_C2_RGB_reg,
        .dscd_pattern_gen_c3_rgb                                             = DSCD_DSCD_PATTERN_GEN_C3_RGB_reg,
        .dscd_pattern_gen_c4_rgb                                             = DSCD_DSCD_PATTERN_GEN_C4_RGB_reg,
        .dscd_pattern_gen_c5_rgb                                             = DSCD_DSCD_PATTERN_GEN_C5_RGB_reg,
        .dscd_pattern_gen_c6_rgb                                             = DSCD_DSCD_PATTERN_GEN_C6_RGB_reg,
        .dscd_pattern_gen_c7_rgb                                             = DSCD_DSCD_PATTERN_GEN_C7_RGB_reg,
        .dscd_pattern_gen_b_msb                                             = DSCD_DSCD_PATTERN_GEN_B_MSB_reg,


        .dscd_mbist_read_margin_0                                             = DSCD_DSCD_MBIST_READ_MARGIN_0_reg,
        .dscd_mbist_read_margin_1                                             = DSCD_DSCD_MBIST_READ_MARGIN_1_reg,
        .dscd_bist_mode                                             = DSCD_DSCD_BIST_MODE_reg,
        .dscd_bist_done                                             = DSCD_DSCD_BIST_MODE_reg,
        .dscd_bist_fail_group                                             = DSCD_DSCD_BIST_FAIL_GROUP_reg,
        .dscd_bist_fail                                             = DSCD_DSCD_BIST_FAIL_reg,
        .dscd_drf_bist_mode                                             = DSCD_DSCD_DRF_BIST_MODE_reg,
        .dscd_drf_bist_resume                                           = DSCD_DSCD_DRF_BIST_RESUME_reg,
        .dscd_drf_bist_done                                             = DSCD_DSCD_DRF_BIST_DONE_reg,
        .dscd_drf_bist_fail_group                                             = DSCD_DSCD_DRF_BIST_FAIL_GROUP_reg,
        .dscd_drf_bist_fail                                             = DSCD_DSCD_DRF_BIST_FAIL_reg,
        .dscd_drf_bist_pause                                             = DSCD_DSCD_DRF_BIST_PAUSE_reg,
        .dscd_bist_t12_test                                             = DSCD_DSCD_BIST_T12_TEST_reg,
    },
    // dscd 2
    {
        .dscd_pps_dw00                                             = DSCD2_PPS_DW00_reg,
        .dscd_pps_dw01                                             = DSCD2_PPS_DW01_reg,
        .dscd_pps_dw02                                             = DSCD2_PPS_DW02_reg,
        .dscd_pps_dw03                                             = DSCD2_PPS_DW03_reg,
        .dscd_pps_dw04                                             = DSCD2_PPS_DW04_reg,
        .dscd_pps_dw05                                             = DSCD2_PPS_DW05_reg,
        .dscd_pps_dw06                                             = DSCD2_PPS_DW06_reg,
        .dscd_pps_dw07                                             = DSCD2_PPS_DW07_reg,
        .dscd_pps_dw08                                             = DSCD2_PPS_DW08_reg,
        .dscd_pps_dw09                                             = DSCD2_PPS_DW09_reg,
        .dscd_pps_dw10                                             = DSCD2_PPS_DW10_reg,
        .dscd_pps_dw11                                             = DSCD2_PPS_DW11_reg,
        .dscd_pps_dw12                                             = DSCD2_PPS_DW12_reg,
        .dscd_pps_dw13                                             = DSCD2_PPS_DW13_reg,
        .dscd_pps_dw14                                             = DSCD2_PPS_DW14_reg,
        .dscd_pps_dw15                                             = DSCD2_PPS_DW15_reg,
        .dscd_pps_dw16                                             = DSCD2_PPS_DW16_reg,
        .dscd_pps_dw17                                             = DSCD2_PPS_DW17_reg,
        .dscd_pps_dw18                                             = DSCD2_PPS_DW18_reg,
        .dscd_pps_dw19                                             = DSCD2_PPS_DW19_reg,
        .dscd_pps_dw20                                             = DSCD2_PPS_DW20_reg,
        .dscd_pps_dw21                                             = DSCD2_PPS_DW21_reg,
        .dscd_pps_dw22                                             = DSCD2_PPS_DW22_reg,
        .dscd_pps_dw23                                             = DSCD2_PPS_DW23_reg,
        .dscd_pps_dw24                                             = DSCD2_PPS_DW24_reg,
        .dscd_pps_dw25                                             = DSCD2_PPS_DW25_reg,
        .dscd_pps_dw26                                             = DSCD2_PPS_DW26_reg,
        .dscd_pps_dw27                                             = DSCD2_PPS_DW27_reg,
        .dscd_pps_dw28                                             = DSCD2_PPS_DW28_reg,
        .dscd_pps_dw29                                             = DSCD2_PPS_DW29_reg,
        .dscd_pps_dw30                                             = DSCD2_PPS_DW30_reg,
        .dscd_pps_dw31                                             = DSCD2_PPS_DW31_reg,
        .dscd_pps_dw32                                             = DSCD2_PPS_DW32_reg,
        .dscd_pps_dw33                                             = DSCD2_PPS_DW33_reg,

        .dscd_pps_dw00_cur                                      = DSCD2_PPS_DW00_CUR_reg,
        .dscd_pps_dw01_cur                                      = DSCD2_PPS_DW01_CUR_reg,
        .dscd_pps_dw02_cur                                      = DSCD2_PPS_DW02_CUR_reg,
        .dscd_pps_dw03_cur                                      = DSCD2_PPS_DW03_CUR_reg,
        .dscd_pps_dw04_cur                                      = DSCD2_PPS_DW04_CUR_reg,
        .dscd_pps_dw05_cur                                      = DSCD2_PPS_DW05_CUR_reg,
        .dscd_pps_dw06_cur                                      = DSCD2_PPS_DW06_CUR_reg,
        .dscd_pps_dw07_cur                                      = DSCD2_PPS_DW07_CUR_reg,
        .dscd_pps_dw08_cur                                      = DSCD2_PPS_DW08_CUR_reg,
        .dscd_pps_dw09_cur                                      = DSCD2_PPS_DW09_CUR_reg,
        .dscd_pps_dw10_cur                                      = DSCD2_PPS_DW10_CUR_reg,
        .dscd_pps_dw11_cur                                      = DSCD2_PPS_DW11_CUR_reg,
        .dscd_pps_dw12_cur                                      = DSCD2_PPS_DW12_CUR_reg,
        .dscd_pps_dw13_cur                                      = DSCD2_PPS_DW13_CUR_reg,
        .dscd_pps_dw14_cur                                      = DSCD2_PPS_DW14_CUR_reg,
        .dscd_pps_dw15_cur                                      = DSCD2_PPS_DW15_CUR_reg,
        .dscd_pps_dw16_cur                                      = DSCD2_PPS_DW16_CUR_reg,
        .dscd_pps_dw17_cur                                      = DSCD2_PPS_DW17_CUR_reg,
        .dscd_pps_dw18_cur                                      = DSCD2_PPS_DW18_CUR_reg,
        .dscd_pps_dw19_cur                                      = DSCD2_PPS_DW19_CUR_reg,
        .dscd_pps_dw20_cur                                      = DSCD2_PPS_DW20_CUR_reg,
        .dscd_pps_dw21_cur                                      = DSCD2_PPS_DW21_CUR_reg,
        .dscd_pps_dw22_cur                                      = DSCD2_PPS_DW22_CUR_reg,
        .dscd_pps_dw23_cur                                      = DSCD2_PPS_DW23_CUR_reg,
        .dscd_pps_dw24_cur                                      = DSCD2_PPS_DW24_CUR_reg,
        .dscd_pps_dw25_cur                                      = DSCD2_PPS_DW25_CUR_reg,
        .dscd_pps_dw26_cur                                      = DSCD2_PPS_DW26_CUR_reg,
        .dscd_pps_dw27_cur                                      = DSCD2_PPS_DW27_CUR_reg,
        .dscd_pps_dw28_cur                                      = DSCD2_PPS_DW28_CUR_reg,
        .dscd_pps_dw29_cur                                      = DSCD2_PPS_DW29_CUR_reg,
        .dscd_pps_dw30_cur                                      = DSCD2_PPS_DW30_CUR_reg,
        .dscd_pps_dw31_cur                                      = DSCD2_PPS_DW31_CUR_reg,
        .dscd_pps_dw32_cur                                      = DSCD2_PPS_DW32_CUR_reg,
        .dscd_pps_dw33_cur                                      = DSCD2_PPS_DW33_CUR_reg,

        // dscd control_register
        .dscd_db_ctrl                                             = DSCD2_DB_CTRL_reg,
        .dscd_ctrl                                                = DSCD2_CTRL_reg,

        // dscd status
        .dscd_status                                              = DSCD2_STATUS_reg,

        .dscd_wde_dscd                                             = DSCD2_WDE_DSCD_reg,
        .dscd_inte_dscd                                             = DSCD2_INTE_DSCD_reg,
        .dscd_ints_dscd                                             = DSCD2_INTS_DSCD_reg,
        .dscd_dummy                                             = DSCD2_dummy_reg,
        .dscd_debug                                             = DSCD2_debug_reg,
        .dscd_performance                                             = DSCD2_performance_reg,


        .dscd_inte_dscd_2                                             = DSCD2_INTE_DSCD_2_reg,
        .dscd_ints_dscd_2                                             = DSCD2_INTS_DSCD_2_reg,

        .dscd_out_iv_delay                                             = DSCD2_OUT_IV_DELAY_reg,
        .dscd_out_vs_width                                             = DSCD2_OUT_VS_WIDTH_reg,

        .dscd_out_den_delay                                             = DSCD2_OUT_DEN_DELAY_reg,

        .dscd_out_swap                                             = DSCD2_OUT_SWAP_reg,

        .dscd_gt_cycle_measure_ctrl                                             = DSCD2_GT_CYCLE_MEASURE_CTRL_reg,
        .dscd_gt_cycle_measure_result_0                                         = DSCD2_GT_CYCLE_MEASURE_RESULT_0_reg,
        .dscd_gt_cycle_measure_result_1                                         = DSCD2_GT_CYCLE_MEASURE_RESULT_1_reg,
        .dscd_gt_cycle_measure_result_2                                         = DSCD2_GT_CYCLE_MEASURE_RESULT_2_reg,
        .dscd_out_measure                                                       = DSCD2_OUT_MEASURE_reg,


        .dscd_out_measure_outbuf1                                             = DSCD2_OUT_MEASURE_OUTBUF1_reg,

        .dscd_dummy2                                             = DSCD2_dummy2_reg,
        .dscd_dummy3                                             = DSCD2_dummy3_reg,

        .dscd_timing_monitor_ctrl                                             = DSCD2_DSCD_Timing_monitor_ctrl_reg,
        .dscd_timing_monitor_multi                                            = DSCD2_DSCD_Timing_monitor_multi_reg,
        .dscd_timing_monitor_compare_th                                             = DSCD2_DSCD_Timing_monitor_compare_th_reg,
        .dscd_timing_monitor_golden                                             = DSCD2_DSCD_Timing_monitor_golden_reg,
        .dscd_timing_monitor_result1                                             = DSCD2_DSCD_Timing_monitor_result1_reg,
        .dscd_timing_monitor_result2                                             = DSCD2_DSCD_Timing_monitor_result2_reg,
        .dscd_timing_monitor_result3                                             = DSCD2_DSCD_Timing_monitor_result3_reg,
        .dscd_timing_monitor_result4                                             = DSCD2_DSCD_Timing_monitor_result4_reg,
        .dscd_timing_monitor_max                                             = DSCD2_DSCD_Timing_monitor_max_reg,

        .dscd_timing_monitor_min                                             = DSCD2_DSCD_Timing_monitor_min_reg,

        .dscd_timing_monitor_st                                             = DSCD2_DSCD_Timing_monitor_st_reg,

        .dscd_out_crc_en                                             = DSCD2_DSCD_out_crc_en_reg,
        .dscd_out_crc_result                                             = DSCD2_DSCD_out_crc_result_reg,
        .dscd_in_crc_en                                             = DSCD2_Dscd_in_crc_en_reg,
        .dscd_in_crc_result                                             = DSCD2_Dscd_in_crc_result_reg,
        .dscd_pps_crc_en                                             = DSCD2_DSCD_pps_crc_en_reg,
        .dscd_pps_crc_result                                             = DSCD2_DSCD_pps_crc_result_reg,
        .dscd_ext_crc_result                                             = DSCD2_DSCD_ext_crc_result_reg,
        .dscd_dbg_en                                             = DSCD2_DSCD_dbg_en_reg,
        .dscd_dbg1                                             = DSCD2_DSCD_dbg1_reg,


        .dscd_dbg1_size                                             = DSCD2_DSCD_dbg1_size_reg,
        .dscd_dbg_sram                                             = DSCD2_DSCD_dbg_sram_reg,
        .dscd_sram_d0                                             = DSCD2_DSCD_sram_d0_reg,
        .dscd_sram_d1                                             = DSCD2_DSCD_sram_d1_reg,

        .dscd_sram_d2                                             = DSCD2_DSCD_sram_d2_reg,
        .dscd_sram_d3                                             = DSCD2_DSCD_sram_d3_reg,
        .dscd_sram_d4                                             = DSCD2_DSCD_sram_d4_reg,
        .dscd_sram_d5                                             = DSCD2_DSCD_sram_d5_reg,
        .dscd_sram_d6                                             = DSCD2_DSCD_sram_d6_reg,
        .dscd_sram_d7                                             = DSCD2_DSCD_sram_d7_reg,
        .dscd_dbg_sram_rw                                             = DSCD2_DSCD_dbg_sram_rw_reg,
        .dscd_sram_q0                                             = DSCD2_DSCD_sram_q0_reg,
        .dscd_sram_q1                                             = DSCD2_DSCD_sram_q1_reg,
        .dscd_sram_q2                                             = DSCD2_DSCD_sram_q2_reg,
        .dscd_sram_q3                                             = DSCD2_DSCD_sram_q3_reg,
        .dscd_sram_q4                                             = DSCD2_DSCD_sram_q4_reg,
        .dscd_sram_q5                                             = DSCD2_DSCD_sram_q5_reg,
        .dscd_sram_q6                                             = DSCD2_DSCD_sram_q6_reg,
        .dscd_sram_q7                                             = DSCD2_DSCD_sram_q7_reg,
        .dscd_sram_q8                                             = DSCD2_DSCD_sram_q8_reg,
        .dscd_sram_q9                                             = DSCD2_DSCD_sram_q9_reg,
        .dscd_sram_q10                                             = DSCD2_DSCD_sram_q10_reg,
        .dscd_sram_q11                                             = DSCD2_DSCD_sram_q11_reg,

        .dscd_timing_gen_ctrl                                             = DSCD2_DSCD_TIMING_GEN_CTRL_reg,
        .dscd_timing_gen_sta                                             = DSCD2_DSCD_TIMING_GEN_STA_reg,
        .dscd_timing_gen_size                                             = DSCD2_DSCD_TIMING_GEN_SIZE_reg,
        .dscd_timing_gen_end                                             = DSCD2_DSCD_TIMING_GEN_END_reg,
        .dscd_timing_gen_frame_rate                                             = DSCD2_DSCD_TIMING_GEN_FRAME_RATE_reg,

        .dscd_pattern_gen_ctrl                                             = DSCD2_DSCD_PATTERN_GEN_CTRL_reg,
        .dscd_pattern_gen_c0_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C0_RGB_reg,
        .dscd_pattern_gen_c1_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C1_RGB_reg,
        .dscd_pattern_gen_c2_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C2_RGB_reg,
        .dscd_pattern_gen_c3_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C3_RGB_reg,
        .dscd_pattern_gen_c4_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C4_RGB_reg,
        .dscd_pattern_gen_c5_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C5_RGB_reg,
        .dscd_pattern_gen_c6_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C6_RGB_reg,
        .dscd_pattern_gen_c7_rgb                                             = DSCD2_DSCD_PATTERN_GEN_C7_RGB_reg,
        .dscd_pattern_gen_b_msb                                             = DSCD2_DSCD_PATTERN_GEN_B_MSB_reg,


        .dscd_mbist_read_margin_0                                             = DSCD2_DSCD_MBIST_READ_MARGIN_0_reg,
        .dscd_mbist_read_margin_1                                             = DSCD2_DSCD_MBIST_READ_MARGIN_1_reg,
        .dscd_bist_mode                                             = DSCD2_DSCD_BIST_MODE_reg,
        .dscd_bist_done                                             = DSCD2_DSCD_BIST_MODE_reg,
        .dscd_bist_fail_group                                             = DSCD2_DSCD_BIST_FAIL_GROUP_reg,
        .dscd_bist_fail                                             = DSCD2_DSCD_BIST_FAIL_reg,
        .dscd_drf_bist_mode                                             = DSCD2_DSCD_DRF_BIST_MODE_reg,
        .dscd_drf_bist_resume                                           = DSCD2_DSCD_DRF_BIST_RESUME_reg,
        .dscd_drf_bist_done                                             = DSCD2_DSCD_DRF_BIST_DONE_reg,
        .dscd_drf_bist_fail_group                                             = DSCD2_DSCD_DRF_BIST_FAIL_GROUP_reg,
        .dscd_drf_bist_fail                                             = DSCD2_DSCD_DRF_BIST_FAIL_reg,
        .dscd_drf_bist_pause                                             = DSCD2_DSCD_DRF_BIST_PAUSE_reg,
        .dscd_bist_t12_test                                             = DSCD2_DSCD_BIST_T12_TEST_reg,
    },
};

