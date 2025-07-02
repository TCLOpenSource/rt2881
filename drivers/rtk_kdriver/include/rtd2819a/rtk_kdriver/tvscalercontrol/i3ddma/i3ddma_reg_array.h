#ifndef __I3DDMA_REG_ARRAY_H__
#define __I3DDMA_REG_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

//dma-vgip i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
extern const unsigned int vgip_ctrl_reg_addr[];
extern const unsigned int vgip_hsta_width_reg_addr[];
extern const unsigned int vgip_vsta_length_reg_addr[];
extern const unsigned int vgip_misc_reg_addr[];
extern const unsigned int vgip_htotal_reg_addr[];
extern const unsigned int vgip_delay_reg_addr[];
extern const unsigned int vgip_cts_fifo_ctl_reg_addr[];
extern const unsigned int vgip_regen_vs_ctrl1_reg_addr[];
extern const unsigned int vgip_regen_vs_ctrl2_reg_addr[];
//----------------------------------end


//iddma i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
extern const unsigned int iddma_db_reg_addr[];
extern const unsigned int iddma_dma_enable_reg_addr[];
extern const unsigned int iddma_cap1_ctrl_reg_addr[];
extern const unsigned int iddma_cap2_ctrl_reg_addr[];
extern const unsigned int iddma_cap2_1_ctrl_reg_addr[];
extern const unsigned int iddma_cap1_line_burst_unm_reg_addr[];
extern const unsigned int iddma_cap1_line_len_reg_addr[];
extern const unsigned int iddma_cap1_line_step_reg_addr[];
extern const unsigned int iddma_cap2_line_burst_unm_reg_addr[];
extern const unsigned int iddma_cap2_1_line_burst_unm_reg_addr[];
extern const unsigned int iddma_cap2_line_len_reg_addr[];
extern const unsigned int iddma_cap2_1_line_len_reg_addr[];
extern const unsigned int iddma_cap2_line_step_reg_addr[];
extern const unsigned int iddma_cap2_1_line_step_reg_addr[];
extern const unsigned int iddma_cap2_pixel_encode_reg_addr[];
extern const unsigned int iddma_lr_separate_ctrl3_reg_addr[];
extern const unsigned int iddma_cap1_dma_swap_reg_addr[];
extern const unsigned int iddma_cap2_dma_swap_reg_addr[];
extern const unsigned int iddma_cap2_1_dma_swap_reg_addr[];
extern const unsigned int iddma_interrupt_enable_reg_addr[];
extern const unsigned int iddma_cap1_pixel_encode_reg_addr[];
extern const unsigned int iddma_cap1_start_reg_addr[];
extern const unsigned int iddma_cap2_start_reg_addr[];
extern const unsigned int iddma_cap2_1_start_reg_addr[];
extern const unsigned int iddma_cap1_boundary_low_limit_reg_addr[];
extern const unsigned int iddma_cap1_boundary_up_limit_reg_addr[];
extern const unsigned int iddma_cap2_boundary_low_limit_reg_addr[];
extern const unsigned int iddma_cap2_boundary_up_limit_reg_addr[];
extern const unsigned int iddma_cap2_1_boundary_low_limit_reg_addr[];
extern const unsigned int iddma_cap2_1_boundary_up_limit_reg_addr[];
//----------------------------------end

//dither i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
extern const unsigned int i3ddma_dither_ctrl1_reg_addr[];
extern const unsigned int i3ddma_dither_seq_r_00_15_reg_addr[];
extern const unsigned int i3ddma_dither_seq_g_00_07_reg_addr[];
extern const unsigned int i3ddma_dither_seq_b_00_15_reg_addr[];
extern const unsigned int i3ddma_dither_seq_r_16_31_reg_addr[];
extern const unsigned int i3ddma_dither_seq_g_16_31_reg_addr[];
extern const unsigned int i3ddma_dither_seq_b_16_31_reg_addr[];
extern const unsigned int i3ddma_dither_tb_r_00_reg_addr[];
extern const unsigned int i3ddma_dither_tb_g_00_reg_addr[];
extern const unsigned int i3ddma_dither_tb_b_00_reg_addr[];
extern const unsigned int i3ddma_dither_tb_r_02_reg_addr[];
extern const unsigned int i3ddma_dither_tb_g_02_reg_addr[];
extern const unsigned int i3ddma_dither_tb_b_02_reg_addr[];
extern const unsigned int i3ddma_dither_tmp_tb_reg_addr[];

extern const unsigned int i3ddma_rgb2yuv_ctrl_reg_addr[];
extern const unsigned int i3ddma_422to444_ctrl_reg_addr[];
//----------------------------------end

//uzd i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
extern const unsigned int iddma_uzd_h_db_reg_addr[];
extern const unsigned int iddma_uzd_v_db_reg_addr[];
extern const unsigned int iddma_uzd_ctrl_reg_addr[];
extern const unsigned int iddma_uzd_ctrl1_reg_addr[];
extern const unsigned int iddma_uzd_hor_factor_reg_addr[];
extern const unsigned int iddma_uzd_ver_factor_reg_addr[];
extern const unsigned int iddma_uzd_init_value_reg_addr[];
extern const unsigned int iddma_uzd_init_int_reg_addr[];
extern const unsigned int iddma_uzd_ibuff_ctrl_reg_addr[];
extern const unsigned int iddma_uzd_fir_coef_tab1_c0_reg_addr[];
extern const unsigned int iddma_uzd_fir_coef_tab1_c14_reg_addr[];
extern const unsigned int iddma_uzd_fir_coef_tab1_c16_reg_addr[];
extern const unsigned int iddma_uzd_fir_coef_tab1_c30_reg_addr[];
extern const unsigned int iddma_uzd_fir_coef_tab2_c0_reg_addr[];
extern const unsigned int iddma_uzd_coef_db_ctrl_reg_addr[];
//----------------------------------end


#ifdef __cplusplus
}
#endif
#endif //__I3DDMA_REG_ARRAY_H__

