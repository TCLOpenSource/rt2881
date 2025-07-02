#ifndef __HDMI_DSC_REG_H
#define __HDMI_DSC_REG_H

#define DSCD_NUM            2

struct DSCD_reg_st {
    unsigned int  dscd_pps_dw00;
    unsigned int  dscd_pps_dw01;
    unsigned int  dscd_pps_dw02;
    unsigned int  dscd_pps_dw03;
    unsigned int  dscd_pps_dw04;
    unsigned int  dscd_pps_dw05;
    unsigned int  dscd_pps_dw06;
    unsigned int  dscd_pps_dw07;
    unsigned int  dscd_pps_dw08;
    unsigned int  dscd_pps_dw09;
    unsigned int  dscd_pps_dw10;
    unsigned int  dscd_pps_dw11;
    unsigned int  dscd_pps_dw12;
    unsigned int  dscd_pps_dw13;
    unsigned int  dscd_pps_dw14;
    unsigned int  dscd_pps_dw15;
    unsigned int  dscd_pps_dw16;
    unsigned int  dscd_pps_dw17;
    unsigned int  dscd_pps_dw18;
    unsigned int  dscd_pps_dw19;
    unsigned int  dscd_pps_dw20;
    unsigned int  dscd_pps_dw21;
    unsigned int  dscd_pps_dw22;
    unsigned int  dscd_pps_dw23;
    unsigned int  dscd_pps_dw24;
    unsigned int  dscd_pps_dw25;
    unsigned int  dscd_pps_dw26;
    unsigned int  dscd_pps_dw27;
    unsigned int  dscd_pps_dw28;
    unsigned int  dscd_pps_dw29;
    unsigned int  dscd_pps_dw30;
    unsigned int  dscd_pps_dw31;
    unsigned int  dscd_pps_dw32;
    unsigned int  dscd_pps_dw33;

    unsigned int  dscd_pps_dw00_cur;
    unsigned int  dscd_pps_dw01_cur;
    unsigned int  dscd_pps_dw02_cur;
    unsigned int  dscd_pps_dw03_cur;
    unsigned int  dscd_pps_dw04_cur;
    unsigned int  dscd_pps_dw05_cur;
    unsigned int  dscd_pps_dw06_cur;
    unsigned int  dscd_pps_dw07_cur;
    unsigned int  dscd_pps_dw08_cur;
    unsigned int  dscd_pps_dw09_cur;
    unsigned int  dscd_pps_dw10_cur;
    unsigned int  dscd_pps_dw11_cur;
    unsigned int  dscd_pps_dw12_cur;
    unsigned int  dscd_pps_dw13_cur;
    unsigned int  dscd_pps_dw14_cur;
    unsigned int  dscd_pps_dw15_cur;
    unsigned int  dscd_pps_dw16_cur;
    unsigned int  dscd_pps_dw17_cur;
    unsigned int  dscd_pps_dw18_cur;
    unsigned int  dscd_pps_dw19_cur;
    unsigned int  dscd_pps_dw20_cur;
    unsigned int  dscd_pps_dw21_cur;
    unsigned int  dscd_pps_dw22_cur;
    unsigned int  dscd_pps_dw23_cur;
    unsigned int  dscd_pps_dw24_cur;
    unsigned int  dscd_pps_dw25_cur;
    unsigned int  dscd_pps_dw26_cur;
    unsigned int  dscd_pps_dw27_cur;
    unsigned int  dscd_pps_dw28_cur;
    unsigned int  dscd_pps_dw29_cur;
    unsigned int  dscd_pps_dw30_cur;
    unsigned int  dscd_pps_dw31_cur;
    unsigned int  dscd_pps_dw32_cur;
    unsigned int  dscd_pps_dw33_cur;

    // dscd control_register
    unsigned int  dscd_db_ctrl;
    unsigned int  dscd_ctrl;

    // dscd status
    unsigned int  dscd_status;

    unsigned int  dscd_wde_dscd;
    unsigned int  dscd_inte_dscd;
    unsigned int  dscd_ints_dscd;
    unsigned int  dscd_dummy;
    unsigned int  dscd_debug;
    unsigned int  dscd_performance;


    unsigned int  dscd_inte_dscd_2;
    unsigned int  dscd_ints_dscd_2;

    unsigned int  dscd_out_iv_delay;
    unsigned int  dscd_out_vs_width;

    unsigned int  dscd_out_den_delay;

    unsigned int  dscd_out_swap;

    unsigned int  dscd_gt_cycle_measure_ctrl;

    unsigned int  dscd_gt_cycle_measure_result_0;

    unsigned int  dscd_gt_cycle_measure_result_1;
    unsigned int  dscd_gt_cycle_measure_result_2;
    unsigned int  dscd_out_measure;


    unsigned int  dscd_out_measure_outbuf1;

    unsigned int  dscd_dummy2;
    unsigned int  dscd_dummy3;

    unsigned int  dscd_timing_monitor_ctrl;
    unsigned int  dscd_timing_monitor_multi;
    unsigned int  dscd_timing_monitor_compare_th;
    unsigned int  dscd_timing_monitor_golden;
    unsigned int  dscd_timing_monitor_result1;
    unsigned int  dscd_timing_monitor_result2;
    unsigned int  dscd_timing_monitor_result3;
    unsigned int  dscd_timing_monitor_result4;
    unsigned int  dscd_timing_monitor_max;

    unsigned int  dscd_timing_monitor_min;

    unsigned int  dscd_timing_monitor_st;

    unsigned int  dscd_out_crc_en;
    unsigned int  dscd_out_crc_result;
    unsigned int  dscd_in_crc_en;
    unsigned int  dscd_in_crc_result;
    unsigned int  dscd_pps_crc_en;
    unsigned int  dscd_pps_crc_result;
    unsigned int  dscd_ext_crc_result;
    unsigned int  dscd_dbg_en;
    unsigned int  dscd_dbg1;


    unsigned int  dscd_dbg1_size;
    unsigned int  dscd_dbg_sram;
    unsigned int  dscd_sram_d0;
    unsigned int  dscd_sram_d1;

    unsigned int  dscd_sram_d2;
    unsigned int  dscd_sram_d3;
    unsigned int  dscd_sram_d4;
    unsigned int  dscd_sram_d5;
    unsigned int  dscd_sram_d6;
    unsigned int  dscd_sram_d7;
    unsigned int  dscd_dbg_sram_rw;
    unsigned int  dscd_sram_q0;
    unsigned int  dscd_sram_q1;
    unsigned int  dscd_sram_q2;
    unsigned int  dscd_sram_q3;
    unsigned int  dscd_sram_q4;
    unsigned int  dscd_sram_q5;
    unsigned int  dscd_sram_q6;
    unsigned int  dscd_sram_q7;
    unsigned int  dscd_sram_q8;
    unsigned int  dscd_sram_q9;
    unsigned int  dscd_sram_q10;
    unsigned int  dscd_sram_q11;

    unsigned int  dscd_timing_gen_ctrl;
    unsigned int  dscd_timing_gen_sta;
    unsigned int  dscd_timing_gen_size;
    unsigned int  dscd_timing_gen_end;
    unsigned int  dscd_timing_gen_frame_rate;

    unsigned int  dscd_pattern_gen_ctrl;
    unsigned int  dscd_pattern_gen_c0_rgb;
    unsigned int  dscd_pattern_gen_c1_rgb;
    unsigned int  dscd_pattern_gen_c2_rgb;
    unsigned int  dscd_pattern_gen_c3_rgb;

    unsigned int  dscd_pattern_gen_c4_rgb;
    unsigned int  dscd_pattern_gen_c5_rgb;
    unsigned int  dscd_pattern_gen_c6_rgb;
    unsigned int  dscd_pattern_gen_c7_rgb;
    unsigned int  dscd_pattern_gen_b_msb;


    unsigned int  dscd_mbist_read_margin_0;
    unsigned int  dscd_mbist_read_margin_1;
    unsigned int  dscd_bist_mode;
    unsigned int  dscd_drf_bist_resume;
    unsigned int  dscd_bist_done;
    unsigned int  dscd_bist_fail_group;
    unsigned int  dscd_bist_fail;
    unsigned int  dscd_drf_bist_mode;
    unsigned int  dscd_drf_bist_done;
    unsigned int  dscd_drf_bist_fail_group;
    unsigned int  dscd_drf_bist_fail;
    unsigned int  dscd_drf_bist_pause;
    unsigned int  dscd_bist_t12_test;
};
extern const struct DSCD_reg_st dscd[DSCD_NUM];


#define DSCD_PPS_DW00_REG        (dscd[dscd_port].dscd_pps_dw00)
#define DSCD_PPS_DW01_REG        (dscd[dscd_port].dscd_pps_dw01)
#define DSCD_PPS_DW02_REG        (dscd[dscd_port].dscd_pps_dw02)
#define DSCD_PPS_DW03_REG        (dscd[dscd_port].dscd_pps_dw03)
#define DSCD_PPS_DW04_REG        (dscd[dscd_port].dscd_pps_dw04)
#define DSCD_PPS_DW05_REG        (dscd[dscd_port].dscd_pps_dw05)
#define DSCD_PPS_DW06_REG        (dscd[dscd_port].dscd_pps_dw06)
#define DSCD_PPS_DW07_REG        (dscd[dscd_port].dscd_pps_dw07)
#define DSCD_PPS_DW08_REG        (dscd[dscd_port].dscd_pps_dw08)
#define DSCD_PPS_DW09_REG        (dscd[dscd_port].dscd_pps_dw09)
#define DSCD_PPS_DW10_REG        (dscd[dscd_port].dscd_pps_dw10)
#define DSCD_PPS_DW11_REG        (dscd[dscd_port].dscd_pps_dw11)
#define DSCD_PPS_DW12_REG        (dscd[dscd_port].dscd_pps_dw12)
#define DSCD_PPS_DW13_REG        (dscd[dscd_port].dscd_pps_dw13)
#define DSCD_PPS_DW14_REG        (dscd[dscd_port].dscd_pps_dw14)
#define DSCD_PPS_DW15_REG        (dscd[dscd_port].dscd_pps_dw15)
#define DSCD_PPS_DW16_REG        (dscd[dscd_port].dscd_pps_dw16)
#define DSCD_PPS_DW17_REG        (dscd[dscd_port].dscd_pps_dw17)
#define DSCD_PPS_DW18_REG        (dscd[dscd_port].dscd_pps_dw18)
#define DSCD_PPS_DW19_REG        (dscd[dscd_port].dscd_pps_dw19)
#define DSCD_PPS_DW20_REG        (dscd[dscd_port].dscd_pps_dw20)
#define DSCD_PPS_DW21_REG        (dscd[dscd_port].dscd_pps_dw21)
#define DSCD_PPS_DW22_REG        (dscd[dscd_port].dscd_pps_dw22)
#define DSCD_PPS_DW23_REG        (dscd[dscd_port].dscd_pps_dw23)
#define DSCD_PPS_DW24_REG        (dscd[dscd_port].dscd_pps_dw24)
#define DSCD_PPS_DW25_REG        (dscd[dscd_port].dscd_pps_dw25)
#define DSCD_PPS_DW26_REG        (dscd[dscd_port].dscd_pps_dw26)
#define DSCD_PPS_DW27_REG        (dscd[dscd_port].dscd_pps_dw27)
#define DSCD_PPS_DW28_REG        (dscd[dscd_port].dscd_pps_dw28)
#define DSCD_PPS_DW29_REG        (dscd[dscd_port].dscd_pps_dw29)
#define DSCD_PPS_DW30_REG        (dscd[dscd_port].dscd_pps_dw30)
#define DSCD_PPS_DW31_REG        (dscd[dscd_port].dscd_pps_dw31)
#define DSCD_PPS_DW32_REG        (dscd[dscd_port].dscd_pps_dw32)
#define DSCD_PPS_DW33_REG        (dscd[dscd_port].dscd_pps_dw33)

#define DSCD_PPS_DW00_CUR_REG        (dscd[dscd_port].dscd_pps_dw00_cur)
#define DSCD_PPS_DW01_CUR_REG        (dscd[dscd_port].dscd_pps_dw01_cur)
#define DSCD_PPS_DW02_CUR_REG        (dscd[dscd_port].dscd_pps_dw02_cur)
#define DSCD_PPS_DW03_CUR_REG        (dscd[dscd_port].dscd_pps_dw03_cur)
#define DSCD_PPS_DW04_CUR_REG        (dscd[dscd_port].dscd_pps_dw04_cur)
#define DSCD_PPS_DW05_CUR_REG        (dscd[dscd_port].dscd_pps_dw05_cur)
#define DSCD_PPS_DW06_CUR_REG        (dscd[dscd_port].dscd_pps_dw06_cur)
#define DSCD_PPS_DW07_CUR_REG        (dscd[dscd_port].dscd_pps_dw07_cur)
#define DSCD_PPS_DW08_CUR_REG        (dscd[dscd_port].dscd_pps_dw08_cur)
#define DSCD_PPS_DW09_CUR_REG        (dscd[dscd_port].dscd_pps_dw09_cur)
#define DSCD_PPS_DW10_CUR_REG        (dscd[dscd_port].dscd_pps_dw10_cur)
#define DSCD_PPS_DW11_CUR_REG        (dscd[dscd_port].dscd_pps_dw11_cur)
#define DSCD_PPS_DW12_CUR_REG        (dscd[dscd_port].dscd_pps_dw12_cur)
#define DSCD_PPS_DW13_CUR_REG        (dscd[dscd_port].dscd_pps_dw13_cur)
#define DSCD_PPS_DW14_CUR_REG        (dscd[dscd_port].dscd_pps_dw14_cur)
#define DSCD_PPS_DW15_CUR_REG        (dscd[dscd_port].dscd_pps_dw15_cur)
#define DSCD_PPS_DW16_CUR_REG        (dscd[dscd_port].dscd_pps_dw16_cur)
#define DSCD_PPS_DW17_CUR_REG        (dscd[dscd_port].dscd_pps_dw17_cur)
#define DSCD_PPS_DW18_CUR_REG        (dscd[dscd_port].dscd_pps_dw18_cur)
#define DSCD_PPS_DW19_CUR_REG        (dscd[dscd_port].dscd_pps_dw19_cur)
#define DSCD_PPS_DW20_CUR_REG        (dscd[dscd_port].dscd_pps_dw20_cur)
#define DSCD_PPS_DW21_CUR_REG        (dscd[dscd_port].dscd_pps_dw21_cur)
#define DSCD_PPS_DW22_CUR_REG        (dscd[dscd_port].dscd_pps_dw22_cur)
#define DSCD_PPS_DW23_CUR_REG        (dscd[dscd_port].dscd_pps_dw23_cur)
#define DSCD_PPS_DW24_CUR_REG        (dscd[dscd_port].dscd_pps_dw24_cur)
#define DSCD_PPS_DW25_CUR_REG        (dscd[dscd_port].dscd_pps_dw25_cur)
#define DSCD_PPS_DW26_CUR_REG        (dscd[dscd_port].dscd_pps_dw26_cur)
#define DSCD_PPS_DW27_CUR_REG        (dscd[dscd_port].dscd_pps_dw27_cur)
#define DSCD_PPS_DW28_CUR_REG        (dscd[dscd_port].dscd_pps_dw28_cur)
#define DSCD_PPS_DW29_CUR_REG        (dscd[dscd_port].dscd_pps_dw29_cur)
#define DSCD_PPS_DW30_CUR_REG        (dscd[dscd_port].dscd_pps_dw30_cur)
#define DSCD_PPS_DW31_CUR_REG        (dscd[dscd_port].dscd_pps_dw31_cur)
#define DSCD_PPS_DW32_CUR_REG        (dscd[dscd_port].dscd_pps_dw32_cur)
#define DSCD_PPS_DW33_CUR_REG        (dscd[dscd_port].dscd_pps_dw33_cur)



#define DSCD_DB_CTRL_REG                               (dscd[dscd_port].dscd_db_ctrl)
#define DSCD_CTRL_REG                                  (dscd[dscd_port].dscd_ctrl)

// dscd status
#define DSCD_STATUS_REG                                       (dscd[dscd_port].dscd_status)
#define DSCD_WDE_DSCD_REG                                    (dscd[dscd_port].dscd_wde_dscd)
#define DSCD_INTE_DSCD_REG                                   (dscd[dscd_port].dscd_inte_dscd)
#define DSCD_INTS_DSCD_REG                                   (dscd[dscd_port].dscd_ints_dscd)
#define DSCD_dummy_REG                                       (dscd[dscd_port].dscd_dummy)
#define DSCD_debug_REG                                       (dscd[dscd_port].dscd_debug)
#define DSCD_performance_REG                                    (dscd[dscd_port].dscd_performance))


#define DSCD_INTE_DSCD_2_REG                                    (dscd[dscd_port].dscd_inte_dscd_2)
#define DSCD_INTS_DSCD_2_REG                                    (dscd[dscd_port].dscd_ints_dscd_2)

#define DSCD_OUT_IV_DELAY_REG                                    (dscd[dscd_port].dscd_out_iv_delay)
#define DSCD_OUT_VS_WIDTH_REG                                    (dscd[dscd_port].dscd_out_vs_width)

#define DSCD_OUT_DEN_DELAY_REG                                    (dscd[dscd_port].dscd_out_den_delay)
#define DSCD_OUT_SWAP_REG                                         (dscd[dscd_port].dscd_out_swap)

#define DSCD_GT_CYCLE_MEASURE_CTRL_REG                          (dscd[dscd_port].dscd_gt_cycle_measure_ctrl)
#define DSCD_GT_CYCLE_MEASURE_RESULT_0_REG                      (dscd[dscd_port].dscd_gt_cycle_measure_result_0)
#define DSCD_GT_CYCLE_MEASURE_RESULT_1_REG                      (dscd[dscd_port].dscd_gt_cycle_measure_result_1)
#define DSCD_GT_CYCLE_MEASURE_RESULT_2_REG                      (dscd[dscd_port].dscd_gt_cycle_measure_result_2)
#define DSCD_OUT_MEASURE_REG                                    (dscd[dscd_port].dscd_out_measure)


#define DSCD_OUT_MEASURE_OUTBUF1_REG                             (dscd[dscd_port].dscd_out_measure_outbuf1)

#define DSCD_dummy2_REG                                           (dscd[dscd_port].dscd_dummy2)
#define DSCD_dummy3_REG                                           (dscd[dscd_port].dscd_dummy3)

#define DSCD_DSCD_Timing_monitor_ctrl_REG                             (dscd[dscd_port].dscd_timing_monitor_ctrl)
#define DSCD_DSCD_Timing_monitor_multi_REG                            (dscd[dscd_port].dscd_timing_monitor_multi)
#define DSCD_DSCD_Timing_monitor_compare_th_REG                      (dscd[dscd_port].dscd_timing_monitor_compare_th)
#define DSCD_DSCD_Timing_monitor_golden_REG                      (dscd[dscd_port].dscd_timing_monitor_golden)
#define DSCD_DSCD_Timing_monitor_result1_REG                      (dscd[dscd_port].dscd_timing_monitor_result1)
#define DSCD_DSCD_Timing_monitor_result2_REG                      (dscd[dscd_port].dscd_timing_monitor_result2)
#define DSCD_DSCD_Timing_monitor_result3_REG                      (dscd[dscd_port].dscd_timing_monitor_result3)
#define DSCD_DSCD_Timing_monitor_result4_REG                      (dscd[dscd_port].dscd_timing_monitor_result4)
#define DSCD_DSCD_Timing_monitor_max_REG                                    (dscd[dscd_port].dscd_timing_monitor_max)

#define DSCD_DSCD_Timing_monitor_min_REG                                    (dscd[dscd_port].dscd_timing_monitor_min)
#define DSCD_DSCD_Timing_monitor_st_REG                                    (dscd[dscd_port].dscd_timing_monitor_st)

#define DSCD_DSCD_out_crc_en_REG                                       (dscd[dscd_port].dscd_out_crc_en)
#define DSCD_DSCD_out_crc_result_REG                                    (dscd[dscd_port].dscd_out_crc_result)
#define DSCD_Dscd_in_crc_en_REG                                    (dscd[dscd_port].dscd_in_crc_en)
#define DSCD_Dscd_in_crc_result_REG                                    (dscd[dscd_port].dscd_in_crc_result)
#define DSCD_DSCD_pps_crc_en_REG                                    (dscd[dscd_port].dscd_pps_crc_en)
#define DSCD_DSCD_pps_crc_result_REG                             (dscd[dscd_port].dscd_pps_crc_result)
#define DSCD_DSCD_ext_crc_result_REG                             (dscd[dscd_port].dscd_ext_crc_result)
#define DSCD_DSCD_dbg_en_REG                                    (dscd[dscd_port].dscd_dbg_en)
#define DSCD_DSCD_dbg1_REG                                    (dscd[dscd_port].dscd_dbg1)


#define DSCD_DSCD_dbg1_size_REG                            (dscd[dscd_port].dscd_dbg1_size)
#define DSCD_DSCD_dbg_sram_REG                             (dscd[dscd_port].dscd_dbg_sram)
#define DSCD_DSCD_sram_d0_REG                             (dscd[dscd_port].dscd_sram_d0)
#define DSCD_DSCD_sram_d1_REG                             (dscd[dscd_port].dscd_sram_d1)

#define DSCD_DSCD_sram_d2_REG                             (dscd[dscd_port].dscd_sram_d2)
#define DSCD_DSCD_sram_d3_REG                             (dscd[dscd_port].dscd_sram_d3)
#define DSCD_DSCD_sram_d4_REG                             (dscd[dscd_port].dscd_sram_d4)
#define DSCD_DSCD_sram_d5_REG                             (dscd[dscd_port].dscd_sram_d5)
#define DSCD_DSCD_sram_d6_REG                             (dscd[dscd_port].dscd_sram_d6)
#define DSCD_DSCD_sram_d7_REG                             (dscd[dscd_port].dscd_sram_d7)
#define DSCD_DSCD_dbg_sram_rw_REG                         (dscd[dscd_port].dscd_dbg_sram_rw)
#define DSCD_DSCD_sram_q0_REG                             (dscd[dscd_port].dscd_sram_q0)
#define DSCD_DSCD_sram_q1_REG                             (dscd[dscd_port].dscd_sram_q1)
#define DSCD_DSCD_sram_q2_REG                             (dscd[dscd_port].dscd_sram_q2)
#define DSCD_DSCD_sram_q3_REG                             (dscd[dscd_port].dscd_sram_q3)
#define DSCD_DSCD_sram_q4_REG                             (dscd[dscd_port].dscd_sram_q4)
#define DSCD_DSCD_sram_q5_REG                             (dscd[dscd_port].dscd_sram_q5)
#define DSCD_DSCD_sram_q6_REG                             (dscd[dscd_port].dscd_sram_q6)
#define DSCD_DSCD_sram_q7_REG                             (dscd[dscd_port].dscd_sram_q7
#define DSCD_DSCD_sram_q8_REG                             (dscd[dscd_port].dscd_sram_q8)
#define DSCD_DSCD_sram_q9_REG                             (dscd[dscd_port].dscd_sram_q9)
#define DSCD_DSCD_sram_q10_REG                            (dscd[dscd_port].dscd_sram_q10)
#define DSCD_DSCD_sram_q11_REG                            (dscd[dscd_port].dscd_sram_q11)

#define DSCD_DSCD_TIMING_GEN_CTRL_REG                      (dscd[dscd_port].dscd_timing_gen_ctrl))
#define DSCD_DSCD_TIMING_GEN_STA_REG                       (dscd[dscd_port].dscd_timing_gen_sta)
#define DSCD_DSCD_TIMING_GEN_SIZE_REG                      (dscd[dscd_port].dscd_timing_gen_size)
#define DSCD_DSCD_TIMING_GEN_END_REG                       (dscd[dscd_port].dscd_timing_gen_end
#define DSCD_DSCD_TIMING_GEN_FRAME_RATE_REG                (dscd[dscd_port].dscd_timing_gen_frame_rate)

#define DSCD_DSCD_PATTERN_GEN_CTRL_REG                      (dscd[dscd_port].dscd_pattern_gen_ctrl)
#define DSCD_DSCD_PATTERN_GEN_C0_RGB_REG                    (dscd[dscd_port].dscd_pattern_gen_c0_rgb)
#define DSCD_DSCD_PATTERN_GEN_C1_RGB_REG                    (dscd[dscd_port].dscd_pattern_gen_c1_rgb)
#define DSCD_DSCD_PATTERN_GEN_C2_RGB_REG                    (dscd[dscd_port].dscd_pattern_gen_c2_rgb)
#define DSCD_DSCD_PATTERN_GEN_C3_RGB_REG                (dscd[dscd_port].dscd_pattern_gen_c3_rgb)
#define DSCD_DSCD_PATTERN_GEN_C4_RGB_REG                (dscd[dscd_port].dscd_pattern_gen_c4_rgb)
#define DSCD_DSCD_PATTERN_GEN_C5_RGB_REG                (dscd[dscd_port].dscd_pattern_gen_c5_rgb)
#define DSCD_DSCD_PATTERN_GEN_C6_RGB_REG                (dscd[dscd_port].dscd_pattern_gen_c6_rgb)
#define DSCD_DSCD_PATTERN_GEN_C7_RGB_REG                (dscd[dscd_port].dscd_pattern_gen_c7_rgb)
#define DSCD_DSCD_PATTERN_GEN_B_MSB_REG                 (dscd[dscd_port].dscd_pattern_gen_b_msb)


#define DSCD_DSCD_MBIST_READ_MARGIN_0_REG           (dscd[dscd_port].dscd_mbist_read_margin_0)
#define DSCD_DSCD_MBIST_READ_MARGIN_1_REG           (dscd[dscd_port].dscd_mbist_read_margin_1)
#define DSCD_DSCD_BIST_MODE_REG                     (dscd[dscd_port].dscd_bist_mode)
#define DSCD_DSCD_DRF_BIST_RESUME_REG               (dscd[dscd_port].dscd_drf_bist_resume)
#define DSCD_DSCD_BIST_FAIL_GROUP_REG               (dscd[dscd_port].dscd_bist_fail_group)
#define DSCD_DSCD_BIST_FAIL_REG                     (dscd[dscd_port].dscd_bist_fail)
#define DSCD_DSCD_DRF_BIST_MODE_REG                 (dscd[dscd_port].dscd_drf_bist_mode)
#define DSCD_DSCD_DRF_BIST_DONE_REG                 (dscd[dscd_port].dscd_drf_bist_done)
#define DSCD_DSCD_DRF_BIST_FAIL_GROUP_REG           (dscd[dscd_port].dscd_drf_bist_fail_group)
#define DSCD_DSCD_DRF_BIST_FAIL_REG                 (dscd[dscd_port].dscd_drf_bist_fail))
#define DSCD_DSCD_DRF_BIST_PAUSE_REG                (dscd[dscd_port].dscd_drf_bist_pause)
#define DSCD_DSCD_BIST_T12_TEST_REG                 (dscd[dscd_port].dscd_bist_t12_test)

#endif //__HDMI_DSC_REG_H
