
/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2024
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for scaler non-libdma register control.
 *
 * @author 	$Author$
 * @date		$Date$
 * @version	$Revision$
 * @ingroup	clock
 */

/**
 * @addtogroup clock
 * @{
 */

#ifndef _SCALER_NONLIBDMA_H_
#define _SCALER_NONLIBDMA_H_

/*============================ Module dependency  ===========================*/
//#include <linux/types.h>
//#include <ioctrl/scaler/vsc_cmd_id.h>
//#include <base_types.h>
#include <tvscalercontrol/scalerdrv/nonlibdma_scalermemory.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
/*================================ Definitions ==============================*/
#define _NONLIBDMA_DUMMYADDR					0xFFFFFFFF
#define NONLIBDMA_CONFIG_DDR_DATA_BITS 16
/*===================================  Types ================================*/
/*================================== Variables ==============================*/
/*================================== Function ===============================*/
// access mdomain related register
//void nonlibdma_set_mdomain_reg(char* str_input_reg, char* str_set_type, unsigned int input_bits, unsigned int input_bits2, unsigned int timeoutcnt);
//unsigned int nonlibdma_get_mdomain_reg(char* str_get_reg, char* str_get_type, char* str_get_bits);



// scalermemory wrapper macro
unsigned int nonlibdma_get_main_mdisp_register_range(unsigned int addr);
unsigned int nonlibdma_get_main_mdisp_ddr_range(unsigned int addr);
unsigned int nonlibdma_get_main_mdisp_start_register(void);
unsigned int nonlibdma_get_main_mdisp_end_register(void);
unsigned int nonlibdma_get_pst_dispm_block_size(void);
unsigned int nonlibdma_get_pst_idomm_block_size(void);
unsigned int nonlibdma_get_sub_mdisp_register_range(unsigned int addr);
unsigned int nonlibdma_get_sub_mdisp_ddr_range(unsigned int addr);
unsigned int nonlibdma_get_sub_mdisp_start_register(void);
unsigned int nonlibdma_get_sub_mdisp_end_register(void);
unsigned int nonlibdma_get_main_mdisp_register_num(void);
unsigned int nonlibdma_get_mdomain_cap_register_num_0(void);
unsigned int nonlibdma_get_mdomain_cap_register_num_1(void);
unsigned int nonlibdma_get_mdomain_main_buffer_size_16bits_4k120_co_buffer(void);
unsigned int nonlidbam_get_mdomain_main_buffer_size_16bits_4k(void);
unsigned int nonlibdma_get_mdomain_sub_buffer_size(void);
unsigned int nonlibdma_get_m_domain_width_carveout_2k(void);
unsigned int nonlibdma_get_m_domain_len_carveout_2k(void);
unsigned int nonlibdma_get_m_domain_width_business_display(void);
unsigned int nonlibdma_get_m_domain_len_business_display(void);
unsigned int nonlibdma_get_m_domain_width_carveout(void);
unsigned int nonlibdma_get_m_domain_len_carveout(void);
unsigned int nonlibdma_get_m_domain_width_2k(void);
unsigned int nonlibdma_get_m_domain_len_2k(void);
unsigned int nonlibdma_get_game_mode_frame_sync(void);
unsigned int nonlibdma_get_game_mode_new_fll(void);
//unsigned int nonlibdma_get_dma_speedup_align_value(void);
unsigned int nonlibdma_get_ddr_bank_size(void);
unsigned int nonlibdma_get_ddr_bank_addr_alignment(void);
unsigned int nonlibdma_get_fifolength(void);
unsigned int nonlibdma_get_fifolength2(void);
unsigned int nonlibdma_get_burstlength(void);
unsigned int nonlibdma_get_burstlength2(void);


// scalermemory wrapper function
void nonlibdma_drv_memory_set_game_mode_dynamic(unsigned char enable);
unsigned char nonlibdma_drv_memory_get_game_mode_dynamic(void);
unsigned char nonlibdma_drv_memory_get_game_mode_dynamic_flag(void);
void nonlibdma_set_main_pre_read_v_start(unsigned int m_pre_read);
unsigned int nonlibdma_drv_Calculate_m_pre_read_value(void);
void nonlibdma_drv_memory_display_set_input_fast(unsigned char display, unsigned char enable);
void nonlibdma_drv_memory_set_vdec_direct_low_latency_mode(unsigned char enable);
unsigned char nonlibdma_drv_memory_get_vdec_direct_low_latency_mode(void);
unsigned char nonlibdma_drv_memory_get_low_delay_game_mode_dynamic(void);
bool nonlibdma_vdec_data_frc_need_borrow_cma_buffer(unsigned char display);
void nonlibdma_drvif_memory_compression_rpc(unsigned char display);
void nonlibdma_drv_memory_set_limit_boundary_mode(unsigned char display, unsigned char enable);
void nonlibdma_drv_memory_set_sub_gate_vo1(UINT8 a_bEnable);
void nonlibdma_dvrif_memory_setting_for_data_fs(void);
void nonlibdma_drvif_memory_set_fs_display_fifo(void);
void nonlibdma_dvrif_memory_comp_setting(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio);
void nonlibdma_dvrif_memory_comp_setting_sub(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio);
void nonlibdma_dvrif_memory_handler(unsigned char display);
void nonlibdma_drv_memory_wait_mdom_vi_last_write_done(unsigned char display, unsigned char wait_frame);
void nonlibdma_drv_memory_Set_multibuffer_flag(UINT8 enable);
UINT8 nonlibdma_drv_memory_Get_multibuffer_flag(void);
void nonlibdma_drv_memory_wait_cap_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case);
void nonlibdma_drv_memory_Set_multibuffer_number(UINT8 number);
UINT8 nonlibdma_drv_memory_Get_multibuffer_number(void);
void nonlibdma_drv_memory_Set_multibuffer_number_pre(UINT8 number);
UINT8 nonlibdma_drv_memory_Get_multibuffer_number_pre(void);
void nonlibdma_drvif_memory_EnableMDisp_Or_DisableMDisp(unsigned char display, bool enable);
void nonlibdma_drvif_memory_set_dbuffer_write(unsigned char display);
void nonlibdma_drvif_memory_set_memtag_startaddr(unsigned int addr,UINT8 index);
unsigned int nonlibdma_drvif_memory_get_memtag_startaddr(UINT8 index);
char nonlibdma_drv_memory_MA_SNR_Disable(unsigned char MEM_idx);
struct semaphore *nonlibdma_get_main_mdomain_ctrl_semaphore(void);
void nonlibdma_drvif_memory_DisableMCap(unsigned char display);
unsigned long nonlibdma_drvif_memory_get_mainblock_viraddr(void);
unsigned long nonlibdma_drvif_memory_get_subblock_viraddr(void);
unsigned int nonlibdma_drvif_memory_get_block_size(eMemIdx Idx);
unsigned int nonlibdma_drvif_memory_get_block_addr(eMemIdx Idx);
void nonlibdma_drv_memory_Set_multibuffer_changed(UINT8 enable);
UINT8 nonlibdma_drv_memory_Get_multibuffer_changed(void);
void nonlibdma_drv_memory_send_multibuffer_number_to_smoothtoogh(void);
unsigned char nonlibdma_drv_memory_get_game_mode_iv2dv_slow_enable(void);
void nonlibdma_drv_memory_GameMode_Switch_TripleBuffer(void);
void nonlibdma_drv_memory_GameMode_Switch_SingleBuffer(void);
void nonlibdma_drvif_memory_EnableMCap(unsigned char display);
void nonlibdma_drvif_memory_init(void);
void nonlibdma_drvif_memory_release(void);
unsigned int nonlibdma_drv_memory_blk_decontour_Config(unsigned int phyAddr);
unsigned char nonlibdma_drv_memory_Get_MASNR_Mem_Ready_Flag(unsigned char MEM_IDX);
unsigned char nonlibdma_drv_memory_Get_I_BLK_decontour_Mem_Ready_Flag(unsigned char MEM_IDX);
char nonlibdma_drv_memory_di_Set_PQC_init(void);
unsigned char nonlibdma_drv_memory_get_DMA_Aligned_Val(void);
//unsigned long nonlibdma_get_query_start_address(unsigned char idx);
void nonlibdma_drv_memory_disable_di_write_req(unsigned char disableDiWr);
void nonlibdma_drv_memory_set_ip_fifo(unsigned char access);
unsigned int nonlibdma_drvif_memory_alloc_block(eMemIdx Idx, unsigned int space, unsigned char Align);
void nonlibdma_drvif_memory_free_block(eMemIdx Idx);
void nonlibdma_drvif_memory_set_3D_LR_swap(unsigned char lr_swap, unsigned char type_3d);
void nonlibdma_drvif_memory_set_3D_LR_swap_for_SG_frc_in_mDomain(unsigned char bInvMode);
void nonlibdma_drvif_memory_set_MVC_FRC_LR_swap(unsigned char bInvMode);
unsigned char nonlibdma_m_domain_4k_memory_from_type(void);
unsigned int nonlibdma_drv_GameMode_decided_iv2dv_delay_old_mode(void);
unsigned int nonlibdma_drv_GameMode_decided_iv2dv_delay_new_mode(void);
unsigned char nonlibdma_CMA_alloc_m_domain_original_buffer(void);
void nonlibdma_CMA_release_m_domain_original_buffer(void);
void nonlibdma_set_mdomain_driver_status(unsigned char status);
void nonlibdma_mdomain_handler_onlycap(void);
void nonlibdma_memory_set_main_displaywindow_change(void);
void nonlibdma_memory_set_sub_displaywindow_change(unsigned char changetomain);
bool nonlibdma_is_hdmi_dolby_vision_sink_led(void);
void nonlibdma_drv_game_mode_timing_Dynamic(unsigned char enable);
void nonlibdma_dvrif_LowDelay_memory_handler(unsigned char b_switchToSingleBuffer);
void nonlibdma_drv_game_mode_disp_smooth_variable_setting(UINT8 disp_type);
void nonlibdma_main_mdomain_ctrl_semaphore_init(void);
void nonlibdma_drv_GameMode_iv2dv_delay(unsigned char enable);
unsigned int nonlibdma_drv_GameMode_iv2dv_delay_compress_margin(unsigned int iv2dv_ori);
void nonlibdma_dvrif_memory_set_frc_style(UINT8 Display, UINT8 ucEnable);
void nonlibdma_set_sub_m_domain_1st_buf_address(void *addr);
void nonlibdma_set_sub_m_domain_2nd_buf_address(void *addr);
void nonlibdma_set_sub_m_domain_3rd_buf_address(void *addr);
void *nonlibdma_get_sub_m_domain_1st_buf_address(void);
void *nonlibdma_get_sub_m_domain_2nd_buf_address(void);
void *nonlibdma_get_sub_m_domain_3rd_buf_address(void);
void nonlibdma_wait_sub_disp_double_buffer_apply(void);
void nonlibdma_drvif_sub_mdomain_for_srnn(unsigned char enable);
unsigned int nonlibdma_drv_memory_di_buffer_get(void);
void nonlibdma_drvif_memory_set_3d_startaddr(unsigned int addr, UINT8 index);
UINT8 nonlibdma_drv_memory_Get_capture_multibuffer_flag(void);
#ifdef CONFIG_MPIXEL_SHIFT
void nonlibdma_memory_black_buf_init(unsigned int framesize, unsigned int upperPhyAddress);
#endif
StructMemBlock* nonlibdma_drvif_memory_get_MemTag_addr(UINT8 index);
unsigned char nonlibdma_get_mdomain_driver_status(void);
void nonlibdma_clear_m_cap_done_status(unsigned char display);
void nonlibdma_record_current_m_cap_block(unsigned char display);
void nonlibdma_drv_memory_debug_print_buf_idx(void);
unsigned int nonlibdma_memory_get_line_size(unsigned int LineSize, unsigned char InputFormat, unsigned char BitNum);
void nonlibdma_drv_run_GameMode_iv2dv_slow_tuning(void);
void nonlibdma_memory_division(unsigned int Dividend, unsigned char Divisor, unsigned int* Quotient, unsigned char* Remainder);
void nonlibdma_drv_memory_capture_multibuffer_switch_buffer_number(void);
void nonlibdma_drv_memory_display_multibuffer_switch_buffer_number(void);
#ifdef BUILD_QUICK_SHOW
void nonlibdma_scaler_quickshow_memery_init(void);
unsigned long nonlibdma_qs_get_memory_from_vbm(MDOMAIN_MEMORY_QUERY_ID id, unsigned long allocSize);
#endif



void nonlibdma_framesyncGattingDoQS_mainSubCtrl(void);
void nonlibdma_framesyncGattingDoQS_mainSubSdnrCxxto64(void);
void nonlibdma_framesyncGattingDo_mainSubCtrl(void);
void nonlibdma_framesyncGattingDo_mainSubSdnrCxxto64(void);
void nonlibdma_framesyncDo_mainSubCtrl(void);



void nonlibdma_memory_set_displaysubwindow_DispVT(VIDEO_RECT_T s_dispwin, unsigned long phyaddr);
void nonlibdma_memory_set_sub_displaywindow_DispVT(VIDEO_RECT_T s_dispwin);



void nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_enable(int mainSub, unsigned int enable);
void nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_apply(int mainSub, unsigned int apply);
void nonlibdma_set_cap_inctrl_reg_in_freeze_enable(int mainSub, unsigned int enable);
void nonlibdma_set_cap_inctrl_reg_in_freeze_option(int mainSub, unsigned int enable);
void nonlibdma_set_cap_inctrl_reg_double_enable(int mainSub, unsigned int enable);
void nonlibdma_set_cap_inctrl_reg_v_flip_3buf_en(int mainSub, unsigned int enable);
void nonlibdma_set_cap_inctrl_reg_freeze_enable(int mainSub, unsigned int enable);
void nonlibdma_set_cap_ich12_status_reg_buffer_overflow_underflow(int mainSub);
void nonlibdma_set_cap_ie_wd_ctrl_reg_in_cap_last_wr_ie(int mainSub, unsigned int enable);
void nonlibdma_set_cap_boundaryaddr_ctrl2_reg_reset(int mainSub);
void nonlibdma_set_cap_smooth_tog_ctrl_0_reg_pqc_clken(int mainSub, unsigned int disable);
void nonlibdma_set_cap_pq_cmp_reg_cmp_en(int mainSub, unsigned int enable);
void nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(int mainSub, unsigned int enable);
void nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_apply(int mainSub, unsigned int apply);
void nonlibdma_set_disp_enable_reg_disp_en(int mainSub, unsigned int enable);
void nonlibdma_set_disp_mainctrl_reg_single_buffer_select(int mainSub, unsigned int block_sel);
void nonlibdma_set_disp_ctrl_reg_double_en(int mainSub, unsigned int enable);
void nonlibdma_set_disp_ctrl_reg_v_flip_3buf_en(int mainSub, unsigned int enable);
void nonlibdma_set_disp_ctrl_reg_frc_style(int mainSub, unsigned int frc_style);
void nonlibdma_set_disp_prevstart_reg_pre_rd_v_start(int mainSub, unsigned int preread);
void nonlibdma_set_disp_ddr_bm_dma_reg_dma_mode(int mainSub, unsigned int dmaMode);
void nonlibdma_set_disp_dispm_pq_decmp_reg_decmp_en(int mainSub, unsigned int enable);
void nonlibdma_set_vi_mdom_doublebuffer_reg_doublebuffer_enable(int mainSub, unsigned int enable);
void nonlibdma_set_vi_mdom_doublebuffer_reg_doublebuffer_apply(int mainSub, unsigned int enable);
void nonlibdma_set_vi_dmactl_reg_dmaen1(int mainSub, unsigned int enable);
void nonlibdma_set_vi_dmactl_reg_seq_blk_sel(int mainSub, unsigned int dmaMode);
void nonlibdma_set_vi_gctl_reg_vsce1(int mainSub, unsigned int enable);
void nonlibdma_set_vi_inten_reg_interrupt(int mainSub, unsigned int enable);
void nonlibdma_set_vi_intst_reg_status(int mainSub, unsigned int enable);



unsigned int nonlibdma_get_mdomain_register_addr(char* str_input_reg);

unsigned int nonlibdma_get_cap_ddr_inctrl_reg_bufnum(int mainSub);
unsigned int nonlibdma_get_cap_doublebuffer_reg_doublebuffer_enable(int mainSub);
unsigned int nonlibdma_get_cap_doublebuffer_reg_doublebuffer_apply(int mainSub);
unsigned int nonlibdma_get_cap_ich12_ibuff_status_reg_ibuff_ovf(int mainSub);
unsigned int nonlibdma_get_cap_ich12_ibuff_status_reg_fsync_ovf(int mainSub);
unsigned int nonlibdma_get_cap_ich12_ibuff_status_reg_fsync_udf(int mainSub);
unsigned int nonlibdma_get_cap_instatus_reg_freeze_enable_2nd(int mainSub);
unsigned int nonlibdma_get_disp_ddr_prevstart_reg_pre_rd_v_start(int mainSub);
unsigned int nonlibdma_get_disp_status_reg_block_select(int mainSub);
unsigned int nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_enable(int mainSub);
unsigned int nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_apply(int mainSub);
unsigned int nonlibdma_get_disp_ddr_ctrl_reg_frc_style(int mainSub);
unsigned int nonlibdma_get_disp_ddr_ctrl_reg_source_sel(int mainSub);
unsigned int nonlibdma_get_vi_mdom_doublebuffer_reg_doublebuffer_enable(int mainSub);
unsigned int nonlibdma_get_vi_mdom_doublebuffer_reg_doublebuffer_apply(int mainSub);
unsigned int nonlibdma_get_vi_dmactl_reg_dmaen1(int mainSub);
unsigned int nonlibdma_get_vi_dmactl_reg_seq_blk_sel(int mainSub);

void nodlibamd_checkBit_cap_instatus_freeze_ok(int mainSub, unsigned int count);
//unsigned int nonlibdma_get_cap_pqc_en(int mainSub);

#endif // _SCALER_LIBDMA_H_
/*======================== End of File =======================================*/
/**
*
* @}
*/
