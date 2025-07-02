/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2024
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for scaler libdma register control.
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

/*============================ Module dependency  ===========================*/
#include <tvscalercontrol/scalerdrv/scaler_nonlibdma.h>

#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdomain_disp_reg.h>
#include <rbus/mdomain_vi_main_reg.h>
#include <rbus/mdomain_vi_sub_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/timer_reg.h>
#include <rbus/scaleup_reg.h>
#include <rbus/two_step_uzu_reg.h>
#include <rbus/lc_reg.h>
#include <rbus/srnn_pre_post_reg.h>
#include <rbus/pqmask_srnn_reg.h>
#include <rbus/srnn_reg.h>

#include <rtk_kdriver/io.h>
#include <tvscalercontrol/io/ioregdrv.h>

#include <tvscalercontrol/scaler/scalerstruct.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/nonlibdma_scalermemory.h>
#include <scaler/nonlibdma_scalerDrvCommon.h>
#include <ioctrl/scaler/vt_cmd_id.h>
#include <rtd_log/rtd_module_log.h>
#ifdef BUILD_QUICK_SHOW
#include <string.h>
#include <timer.h>
#else
#include <linux/string.h>
#endif

/***********************************REGISTER RANGE*****************************************************/
//main IMD pst range from vodma to mtg
//30 parts
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_VODMA.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_vgip.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_color_mb_peaking.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_De_xcxl.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Dfilter.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_di.h>//5 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Histogram.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Hsd_dither.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_IDCTI.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_iedge_smooth.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_ipq_decontour.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Main_Dither.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Mpegnr.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_nr.h>//2 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Peaking.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Profile.h>//2 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_rgb2yuv.h>//2 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_ringfilter.h>//1 part
#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Scaledown.h>//2 part
//#include <tvscalercontrol/scalerdrv/i2rnd/i2rnd_Mdomain_cap.h>//3 part
/*================================ Definitions ==============================*/

/*===================================  Types ================================*/
//#define NONLIBDMA_DEBUG
/*================================== Variables ==============================*/
extern VT_CAPTURE_CTRL_T CaptureCtrl_VT;
/*================================== Function ===============================*/

unsigned int nonlibdma_get_mdomain_register_addr(char* str_input_reg)
{
	unsigned int mdomain_reg;

	if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In1Ctrl_reg", 27) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In1Ctrl_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2Ctrl_reg", 27) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2Ctrl_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In1Status_reg", 29) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In1Status_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2Status_reg", 29) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2Status_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In1_3rdAddr_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In1_3rdAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In1_2ndAddr_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In1_2ndAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In1Addr_reg", 27) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In1Addr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2_3rdAddr_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2_3rdAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2_2ndAddr_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2_2ndAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2Addr_reg", 27) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2Addr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_cap_reg_doublbuffer_reg", 35) == 0) {
		mdomain_reg = MDOMAIN_CAP_cap_reg_doublbuffer_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_Cap_In1_enable_reg", 30) == 0) {
		mdomain_reg = MDOMAIN_CAP_Cap_In1_enable_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_CapMain_BoundaryAddr1_reg", 37) == 0) {
		mdomain_reg = MDOMAIN_CAP_CapMain_BoundaryAddr1_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_CapMain_BoundaryAddr2_reg", 37) == 0) {
		mdomain_reg = MDOMAIN_CAP_CapMain_BoundaryAddr2_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DI1Cap_WRRD_BoundaryAdd_Status_reg", 46) == 0) {
		mdomain_reg = MDOMAIN_CAP_DI1Cap_WRRD_BoundaryAdd_Status_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_Main_SDNR_cxxto64_reg", 33) == 0) {
		mdomain_reg = MDOMAIN_CAP_Main_SDNR_cxxto64_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_Sub_SDNR_cxxto64_reg", 33) == 0) {
		mdomain_reg = MDOMAIN_CAP_Sub_SDNR_cxxto64_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In1WTLVL_Num_reg", 28) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In1WTLVL_Num_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2WTLVL_reg", 28) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2WTLVL_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In1WrLen_Rem_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In1WrLen_Rem_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2WrLen_Rem_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2WrLen_Rem_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2LineStep_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2LineStep_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_DDR_In2LineNum_reg", 30) == 0) {
		mdomain_reg = MDOMAIN_CAP_DDR_In2LineNum_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg", 34) == 0) {
		mdomain_reg = MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_Capture1_byte_swap_reg", 34) == 0) {
		mdomain_reg = MDOMAIN_CAP_Capture1_byte_swap_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_CAP_PQ_CMP_reg", 26) == 0) {
		mdomain_reg = MDOMAIN_CAP_CAP_PQ_CMP_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_CAP_PQ_CMP_BIT_reg", 30) == 0) {
		mdomain_reg = MDOMAIN_CAP_CAP_PQ_CMP_BIT_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_CAP_PQ_CMP_PAIR_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_CAP_CAP_PQ_CMP_PAIR_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_Cap_IE_WD_CTRL_reg", 30) == 0) {
		mdomain_reg = MDOMAIN_CAP_Cap_IE_WD_CTRL_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_CapMain_BoundaryAddr_CTRL2_reg", 42) == 0) {
		mdomain_reg = MDOMAIN_CAP_CapMain_BoundaryAddr_CTRL2_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_CapSub_BoundaryAddr_CTRL2_reg", 41) == 0) {
		mdomain_reg = MDOMAIN_CAP_CapSub_BoundaryAddr_CTRL2_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_CAP_Smooth_tog_ctrl_0_reg", 33) == 0) {
		mdomain_reg = MDOMAIN_CAP_Smooth_tog_ctrl_0_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainStatus_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainStatus_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainCtrl_reg", 29) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainCtrl_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubCtrl_reg", 28) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubCtrl_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainSubCtrl_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainSubCtrl_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainBlkStep_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainBlkStep_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubBlkStep_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubBlkStep_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubPreVStart_reg", 33) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubPreVStart_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainRdNumLenRem_reg", 36) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainRdNumLenRem_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubRdNumLenRem_reg", 35) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubRdNumLenRem_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubBankLimitEndCol_reg", 39) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubBankLimitEndCol_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubMPEn_reg", 28) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubMPEn_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubMPMode_reg", 30) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubMPMode_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubMPLineNum_reg", 33) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubMPLineNum_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainAddr_reg", 29) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_Main2ndAddr_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_Main2ndAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_Main3rdAddr_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_Main3rdAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubAddr_reg", 28) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_Sub2ndAddr_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_Sub2ndAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_Sub3rdAddr_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_Sub3rdAddr_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainAddrDropBits_reg", 37) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainAddrDropBits_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainNumWTL_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainNumWTL_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubPixNumWTL_reg", 33) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubPixNumWTL_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainLineNum_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainLineNum_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubLineNum_reg", 31) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubLineNum_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainLineStep_reg", 33) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainLineStep_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_SubLineStep_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_SubLineStep_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_Main_BM_DMA_reg", 32) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_Main_BM_DMA_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DDR_MainPreVStart_reg", 34) == 0) {
		mdomain_reg = MDOMAIN_DISP_DDR_MainPreVStart_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DISPM_MAIN_PQ_DECMP_reg", 36) == 0) {
		mdomain_reg = MDOMAIN_DISP_DISPM_MAIN_PQ_DECMP_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DISPM_MAIN_PQ_DECMP_PAIR_reg", 41) == 0) {
		mdomain_reg = MDOMAIN_DISP_DISPM_MAIN_PQ_DECMP_PAIR_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_DISPM_SUB_PQ_DECMP_reg", 35) == 0) {
		mdomain_reg = MDOMAIN_DISP_DISPM_SUB_PQ_DECMP_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_DISP_Display_byte_channel_swap_reg", 42) == 0) {
		mdomain_reg = MDOMAIN_DISP_Display_byte_channel_swap_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg", 40) == 0) {
		mdomain_reg = MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_VI_MAIN_DMACTL_reg", 26) == 0) {
		mdomain_reg = MDOMAIN_VI_MAIN_DMACTL_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_VI_MAIN_GCTL_reg", 24) == 0) {
		mdomain_reg = MDOMAIN_VI_MAIN_GCTL_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_VI_MAIN_INTEN_reg", 25) == 0) {
		mdomain_reg = MDOMAIN_VI_MAIN_INTEN_reg;
	} else if (strncmp(str_input_reg, "MDOMAIN_VI_MAIN_INTST_reg", 25) == 0) {
		mdomain_reg = MDOMAIN_VI_MAIN_INTST_reg;
	} else {
		rtd_pr_vsc_err("[%s-%d] access reg %s error\n", __func__, __LINE__, str_input_reg);
		return 0;
	}

	return mdomain_reg;
}



/*
unsigned int nonlibdma_get_mdomain_cap_ddr_in1ctrl_reg_in1_bufnum(void)
{
	mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;
	mdomain_cap_ddr_in1ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Ctrl_reg);

	return mdomain_cap_ddr_in1ctrl_reg.in1_bufnum;
}

unsigned int nonlibdma_get_mdomain_cap_ddr_in2status_reg_in2_block_select(void)
{
	mdomain_cap_ddr_in2status_RBUS ddr_in2status_reg;
	ddr_in2status_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In2Status_reg);

	return ddr_in2status_reg.in2_block_select;
}

unsigned int nonlibdma_get_mdomain_cap_ddr_in1status_reg_in1_freeze_enable_2nd(void)
{
	mdomain_cap_ddr_in1status_RBUS mdomain_cap_ddr_in1status_reg;
	mdomain_cap_ddr_in1status_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Status_reg);

	return mdomain_cap_ddr_in1status_reg.in1_freeze_enable_2nd;
}

unsigned int nonlibdma_get_mdomain_disp_ddr_mainctrl_reg_main_source_sel(void)
{
	mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;
	ddr_mainctrl_reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_MainCtrl_reg);

	return ddr_mainctrl_reg.main_source_sel;
}

unsigned int nonlibdma_get_mdomain_cap_ich12_ibuff_status_reg_ibuff_ovf(void)
{
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	return mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf;
}

unsigned int nonlibdma_get_mdomain_cap_ich12_ibuff_status_reg_fsync_ovf(void)
{
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	return mdomain_cap_ich12_ibuff_status_reg.fsync_ovf;
}

unsigned int nonlibdma_get_mdomain_cap_ich12_ibuff_status_reg_fsync_udf(void)
{
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	return mdomain_cap_ich12_ibuff_status_reg.fsync_udf;
}

unsigned int nonlibdma_get_mdomain_cap_ich12_ibuff_status_reg_ibuff_ovf_sub(void)
{
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	return mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf_sub;
}

unsigned int nonlibdma_get_mdomain_cap_ich12_ibuff_status_reg_fsync_ovf_sub(void)
{
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	return mdomain_cap_ich12_ibuff_status_reg.fsync_ovf_sub;
}

unsigned int nonlibdma_get_mdomain_cap_ich12_ibuff_status_reg_fsync_udf_sub(void)
{
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	return mdomain_cap_ich12_ibuff_status_reg.fsync_udf_sub;
}

unsigned int nonlibdma_get_mdomain_disp_ddr_mainprevstart_reg_main_pre_rd_v_start(void)
{
	mdomain_disp_ddr_mainprevstart_RBUS mdomain_disp_ddr_mainprevstart_reg;
	mdomain_disp_ddr_mainprevstart_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainPreVStart_reg);

	return mdomain_disp_ddr_mainprevstart_reg.main_pre_rd_v_start;
}

unsigned int nonlibdma_get_mdomain_cap_smooth_tog_ctrl_0_reg_in2_pqc_clken(void)
{
	mdomain_cap_smooth_tog_ctrl_0_RBUS mdomain_cap_smooth_tog_ctrl_0_reg;
	mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);

	return mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken;
}

unsigned int nonlibdma_get_mdomain_cap_ddr_in2addr_reg(void)
{
	return rtd_inl(MDOMAIN_CAP_DDR_In2Addr_reg);
}

unsigned int nonlibdma_get_mdomain_cap_ddr_in2_2ndaddr_reg(void)
{
	return rtd_inl(MDOMAIN_CAP_DDR_In2_2ndAddr_reg);
}

unsigned int nonlibdma_get_mdomain_cap_ddr_in2_3rdaddr_reg(void)
{
	return rtd_inl(MDOMAIN_CAP_DDR_In2_3rdAddr_reg);
}

unsigned int nonlibdma_get_mdomain_cap_ich12_ibuff_status_reg(void)
{
	return IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
}

unsigned int nonlibdma_get_mdomain_disp_ddr_mainstatus_reg(void)
{
	return rtd_inl(MDOMAIN_DISP_DDR_MainStatus_reg);
}

void nonlibdma_set_mdomain_disp_ddr_mainsubctrl_reg_disp2_double_enable(unsigned int input_disp2_double_enable)
{
	mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;

	mdomain_disp_ddr_mainsubctrl_Reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
	mdomain_disp_ddr_mainsubctrl_Reg.disp2_double_enable = input_disp2_double_enable;

	rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg, mdomain_disp_ddr_mainsubctrl_Reg.regValue);
}

void nonlibdma_set_mdomain_disp_disp_sub_enable_reg(unsigned int input_reg)
{
	rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, input_reg);
}

void nonlibdma_set_mdomain_cap_ich12_ibuff_status_reg(unsigned int input_reg)
{
	rtd_outl(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg, input_reg);
}

void nonlibdma_setbits_mdomain_cap_ddr_in1ctrl_reg(unsigned int input_bits)
{
	IoReg_SetBits(MDOMAIN_CAP_DDR_In1Ctrl_reg, input_bits);
}

void nonlibdma_setbits_mdomain_cap_ddr_in2ctrl_reg(unsigned int input_bits)
{
	IoReg_SetBits(MDOMAIN_CAP_DDR_In2Ctrl_reg, input_bits);
}

void nonlibdma_setbits_mdomain_cap_ich12_ibuff_status_reg(unsigned int input_bits)
{
	IoReg_SetBits(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg, input_bits);
}

void nonlibdma_setbits_mdomain_cap_cap_reg_doublbuffer_reg(unsigned int input_bits)
{
	IoReg_SetBits(MDOMAIN_CAP_cap_reg_doublbuffer_reg, input_bits);
}

void nonlibdma_busycheckregbit_mdomain_cap_ddr_in1status_reg(unsigned int input_bits, unsigned int timeoutcnt)
{
	IoReg_BusyCheckRegBit(MDOMAIN_CAP_DDR_In1Status_reg, input_bits, timeoutcnt);
}

void nonlibdma_busycheckregbit_mdomain_cap_ddr_in2status_reg(unsigned int input_bits, unsigned int timeoutcnt)
{
	IoReg_BusyCheckRegBit(MDOMAIN_CAP_DDR_In2Status_reg, input_bits, timeoutcnt);
}
*/


#if 0 //remove nonlibdma write/read m domain register
void nonlibdma_set_mdomain_reg(char* str_input_reg, char* str_set_type, unsigned int input_bits, unsigned int input_bits2, unsigned int timeoutcnt)
{
	unsigned int mdomain_reg = 0;

	mdomain_reg = nonlibdma_get_mdomain_register_addr(str_input_reg);

	if(!mdomain_reg) {
		rtd_pr_vsc_err("[%s-%d] str_set_reg %s error\n", __func__, __LINE__, str_set_type);
		return;
	}

	if (strncmp(str_set_type, "setbits", 7) == 0) {
		IoReg_SetBits(mdomain_reg, input_bits);
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] IoReg_SetBits(%x, %x)\n", __func__, __LINE__, mdomain_reg, input_bits);
#endif
	} else if(strncmp(str_set_type, "set", 3) == 0) {
		rtd_outl(mdomain_reg, input_bits);
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] rtd_outl(%x, %x)\n", __func__, __LINE__, mdomain_reg, input_bits);
#endif
	} else if (strncmp(str_set_type, "busycheckregbit", 15) == 0) {
		IoReg_BusyCheckRegBit(mdomain_reg, input_bits, timeoutcnt);
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] IoReg_BusyCheckRegBit(%x, %x, %u)\n", __func__, __LINE__, mdomain_reg, input_bits, timeoutcnt);
#endif
	} else if (strncmp(str_set_type, "clearbits", 9) == 0) {
		IoReg_ClearBits(mdomain_reg, input_bits);
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] IoReg_ClearBits(%x, %x)\n", __func__, __LINE__, mdomain_reg, input_bits);
#endif
	} else if (strncmp(str_set_type, "mask32", 6) == 0) {
		IoReg_Mask32(mdomain_reg, input_bits, input_bits2);
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] IoReg_Mask32(%x, %x, %x)\n", __func__, __LINE__, mdomain_reg, input_bits, input_bits2);
#endif
	} else if (strncmp(str_set_type, "writeword0", 10) == 0) {
		IoReg_WriteWord0(mdomain_reg, (unsigned short)input_bits);
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] IoReg_WriteWord0(%x, %x)\n", __func__, __LINE__, mdomain_reg, input_bits);
#endif
	} else if (strncmp(str_set_type, "writebyte0", 10) == 0) {
		IoReg_WriteByte0(mdomain_reg, (unsigned char)input_bits);
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] IoReg_WriteByte0(%x, %x)\n", __func__, __LINE__, mdomain_reg, input_bits);
#endif
	} else {
		rtd_pr_vsc_err("[%s-%d] str_set_type %s error\n", __func__, __LINE__, str_set_type);
		return;
	}
}

unsigned int nonlibdma_get_mdomain_reg(char* str_get_reg, char* str_get_type, char* str_get_bits)
{
	unsigned int mdomain_reg = 0;
	unsigned int reg_value = 0;

	mdomain_reg = nonlibdma_get_mdomain_register_addr(str_get_reg);
	reg_value = rtd_inl(mdomain_reg);

	if(!mdomain_reg) {
		rtd_pr_vsc_err("[%s-%d] str_get_reg %s error\n", __func__, __LINE__, str_get_type);
		return 0;
	}

	if (strncmp(str_get_type, "getbit", 6) == 0) {
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] IoReg_SetBits(%x, %x)\n", __func__, __LINE__, mdomain_reg, input_bits);
#endif
		if (strncmp(str_get_reg, "MDOMAIN_CAP_DDR_In1Ctrl_reg", 27) == 0) {
			mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;
			mdomain_cap_ddr_in1ctrl_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "in1_bufnum", 10) == 0)
				return mdomain_cap_ddr_in1ctrl_reg.in1_bufnum;

		} else if (strncmp(str_get_reg, "MDOMAIN_CAP_DDR_In1Status_reg", 29) == 0) {
			mdomain_cap_ddr_in1status_RBUS ddr_in1status_reg;
			ddr_in1status_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "in1_field_polarity_error", 24) == 0)
				return ddr_in1status_reg.in1_field_polarity_error;
			else if(strncmp(str_get_bits, "in1_freeze_enable_2nd", 21) == 0)
				return ddr_in1status_reg.in1_freeze_enable_2nd;
		} else if (strncmp(str_get_reg, "MDOMAIN_CAP_DDR_In2Status_reg", 29) == 0) {
			mdomain_cap_ddr_in2status_RBUS ddr_in2status_reg;
			ddr_in2status_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "in2_block_select", 16) == 0)
				return ddr_in2status_reg.in2_block_select;
			else if(strncmp(str_get_bits, "in2_field_polarity_error", 24) == 0)
				return ddr_in2status_reg.in2_field_polarity_error;
		} else if (strncmp(str_get_reg, "MDOMAIN_CAP_cap_reg_doublbuffer_reg", 35) == 0) {
			mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;
			cap_reg_doublbuffer_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "cap1_db_apply", 13) == 0)
				return cap_reg_doublbuffer_reg.cap1_db_apply;
			else if(strncmp(str_get_bits, "cap2_db_apply", 13) == 0)
				return cap_reg_doublbuffer_reg.cap2_db_apply;
			else if(strncmp(str_get_bits, "cap1_db_en", 10) == 0)
				return cap_reg_doublbuffer_reg.cap1_db_en;
		} else if (strncmp(str_get_reg, "MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg", 34) == 0) {
			mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
			mdomain_cap_ich12_ibuff_status_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "ibuff_ovf", 9) == 0)
				return mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf;
			else if(strncmp(str_get_bits, "fsync_ovf", 9) == 0)
				return mdomain_cap_ich12_ibuff_status_reg.fsync_ovf;
			else if(strncmp(str_get_bits, "fsync_udf", 9) == 0)
				return mdomain_cap_ich12_ibuff_status_reg.fsync_udf;
			else if(strncmp(str_get_bits, "ibuff_ovf_sub", 13) == 0)
				return mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf_sub;
			else if(strncmp(str_get_bits, "fsync_ovf_sub", 13) == 0)
				return mdomain_cap_ich12_ibuff_status_reg.fsync_ovf_sub;
			else if(strncmp(str_get_bits, "fsync_udf_sub", 13) == 0)
				return mdomain_cap_ich12_ibuff_status_reg.fsync_udf_sub;
		} else if (strncmp(str_get_reg, "MDOMAIN_CAP_Smooth_tog_ctrl_0_reg", 33) == 0) {
			mdomain_cap_smooth_tog_ctrl_0_RBUS mdomain_cap_smooth_tog_ctrl_0_reg;
			mdomain_cap_smooth_tog_ctrl_0_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "in2_pqc_clken", 13) == 0)
				return mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken;
		} else if (strncmp(str_get_reg, "MDOMAIN_DISP_DDR_MainStatus_reg", 31) == 0) {
			mdomain_disp_ddr_mainstatus_RBUS mdomain_disp_ddr_mainstatus_reg;
			mdomain_disp_ddr_mainstatus_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "main_block_select", 17) == 0)
				return mdomain_disp_ddr_mainstatus_reg.main_block_select;
		} else if (strncmp(str_get_reg, "MDOMAIN_DISP_DDR_MainBlkStep_reg", 32) == 0) {
			mdomain_disp_ddr_mainblkstep_RBUS ddr_mainblkstep_reg;
			ddr_mainblkstep_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "main_block_step", 15) == 0)
				return ddr_mainblkstep_reg.main_block_step;
		} else if (strncmp(str_get_reg, "MDOMAIN_DISP_DDR_MainCtrl_reg", 29) == 0) {
			mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl;
			ddr_mainctrl.regValue = reg_value;

			if(strncmp(str_get_bits, "main_frc_style", 14) == 0)
				return ddr_mainctrl.main_frc_style;
			else if(strncmp(str_get_bits, "main_source_sel", 15) == 0)
				return ddr_mainctrl.main_source_sel;
		} else if (strncmp(str_get_reg, "MDOMAIN_DISP_DDR_SubCtrl_reg", 28) == 0) {
			mdomain_disp_ddr_subctrl_RBUS mdomain_disp_ddr_subctrl_reg;
			mdomain_disp_ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);

			if(strncmp(str_get_bits, "sub_source_sel", 14) == 0)
				return mdomain_disp_ddr_subctrl_reg.sub_source_sel;
		} else if (strncmp(str_get_reg, "MDOMAIN_DISP_DDR_MainSubCtrl_reg", 32) == 0) {
			mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
			ddr_mainsubctrl_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "disp1_double_enable", 19) == 0)
				return ddr_mainsubctrl_reg.disp1_double_enable;
			else if(strncmp(str_get_bits, "disp1_double_apply", 18) == 0)
				return ddr_mainsubctrl_reg.disp1_double_apply;
		} else if (strncmp(str_get_reg, "MDOMAIN_DISP_DDR_MainPreVStart_reg", 34) == 0) {
			mdomain_disp_ddr_mainprevstart_RBUS mdomain_disp_ddr_mainprevstart_reg;
			mdomain_disp_ddr_mainprevstart_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "main_pre_rd_v_start", 19) == 0)
				return mdomain_disp_ddr_mainprevstart_reg.main_pre_rd_v_start;
		} else if (strncmp(str_get_reg, "MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg", 40) == 0) {
			mdomain_vi_main_mdom_vi_doublebuffer_RBUS mdomain_vi_main_mdom_vi_doublebuffer_reg;
			mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "vi_db_en", 8) == 0)
				return mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_en;
			else if(strncmp(str_get_bits, "vi_db_apply", 11) == 0)
				return mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_apply;
		} else if (strncmp(str_get_reg, "MDOMAIN_VI_MAIN_DMACTL_reg", 26) == 0) {
			mdomain_vi_main_dmactl_RBUS mdomain_vi_main_dmactl_reg;
			mdomain_vi_main_dmactl_reg.regValue = reg_value;

			if(strncmp(str_get_bits, "dmaen1", 6) == 0)
				return mdomain_vi_main_dmactl_reg.dmaen1;
			else if(strncmp(str_get_bits, "seq_blk_sel", 11) == 0)
				return mdomain_vi_main_dmactl_reg.seq_blk_sel;
		}

		rtd_pr_vsc_err("[%s-%d] reg %s bit %s not match\n", __func__, __LINE__, str_get_reg, str_get_bits);
		return 0;
	} else if(strncmp(str_get_type, "get", 3) == 0) {
#ifdef NONLIBDMA_DEBUG
		rtd_pr_vsc_info("[%s-%d] rtd_inl(%x)\n", __func__, __LINE__, mdomain_reg);
#endif
		return reg_value;
	}

	rtd_pr_vsc_err("[%s-%d] str_get_type %s error\n", __func__, __LINE__, str_get_type);
	return 0;

}
#endif

unsigned int nonlibdma_get_mdomain_main_buffer_size_16bits_4k120_co_buffer(void)
{
	return MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER;
}

unsigned int nonlidbam_get_mdomain_main_buffer_size_16bits_4k(void)
{
	return MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K;
}

unsigned int nonlibdma_get_mdomain_sub_buffer_size(void)
{
	return MDOMAIN_SUB_BUFFER_SIZE;
}

unsigned int nonlibdma_get_m_domain_width_carveout_2k(void)
{
	return M_DOMAIN_WIDTH_CARVEOUT_2K;
}

unsigned int nonlibdma_get_m_domain_len_carveout_2k(void)
{
	return M_DOMAIN_LEN_CARVEOUT_2K;
}

unsigned int nonlibdma_get_m_domain_width_business_display(void)
{
	return M_DOMAIN_WIDTH_BUSINESS_DISPLAY;
}

unsigned int nonlibdma_get_m_domain_len_business_display(void)
{
	return M_DOMAIN_LEN_BUSINESS_DISPLAY;
}

unsigned int nonlibdma_get_m_domain_width_carveout(void)
{
	return M_DOMAIN_WIDTH_CARVEOUT;
}

unsigned int nonlibdma_get_m_domain_len_carveout(void)
{
	return M_DOMAIN_LEN_CARVEOUT;
}

unsigned int nonlibdma_get_m_domain_width_2k(void)
{
	return M_DOMAIN_WIDTH_2K;
}

unsigned int nonlibdma_get_m_domain_len_2k(void)
{
	return M_DOMAIN_LEN_2K;
}

unsigned int nonlibdma_get_game_mode_frame_sync(void)
{
	return GAME_MODE_FRAME_SYNC;
}

unsigned int nonlibdma_get_game_mode_new_fll(void)
{
	return GAME_MODE_NEW_FLL;
}

unsigned int nonlibdma_get_ddr_bank_size(void)
{
	return _DDR_BANK_SIZE;
}

unsigned int nonlibdma_get_ddr_bank_addr_alignment(void)
{
	return _DDR_BANK_ADDR_ALIGNMENT;
}

unsigned int nonlibdma_get_fifolength(void)
{
	return _FIFOLENGTH;
}

unsigned int nonlibdma_get_fifolength2(void)
{
	return _FIFOLENGTH2;
}

unsigned int nonlibdma_get_burstlength(void)
{
	return _BURSTLENGTH;
}

unsigned int nonlibdma_get_burstlength2(void)
{
	return _BURSTLENGTH2;
}



void nonlibdma_drv_memory_set_game_mode_dynamic(unsigned char enable)
{
	drv_memory_set_game_mode_dynamic(enable);
}

unsigned char nonlibdma_drv_memory_get_game_mode_dynamic(void)
{
	return drv_memory_get_game_mode_dynamic();
}

unsigned char nonlibdma_drv_memory_get_game_mode_dynamic_flag(void)
{
	return drv_memory_get_game_mode_dynamic_flag();
}

void nonlibdma_set_main_pre_read_v_start(unsigned int m_pre_read)
{
	set_main_pre_read_v_start(m_pre_read);
}

unsigned int nonlibdma_drv_Calculate_m_pre_read_value(void)
{
	return drv_Calculate_m_pre_read_value();
}

void nonlibdma_drv_memory_display_set_input_fast(unsigned char display, unsigned char enable)
{
	drv_memory_display_set_input_fast(display, enable);
}

void nonlibdma_drv_memory_set_vdec_direct_low_latency_mode(unsigned char enable)
{
	drv_memory_set_vdec_direct_low_latency_mode(enable);
}

unsigned char nonlibdma_drv_memory_get_vdec_direct_low_latency_mode(void)
{
	return drv_memory_get_vdec_direct_low_latency_mode();
}

unsigned char nonlibdma_drv_memory_get_low_delay_game_mode_dynamic(void)
{
	return drv_memory_get_low_delay_game_mode_dynamic();
}

bool nonlibdma_vdec_data_frc_need_borrow_cma_buffer(unsigned char display)
{
	return vdec_data_frc_need_borrow_cma_buffer(display);
}

void nonlibdma_drv_memory_set_limit_boundary_mode(unsigned char display, unsigned char enable)
{
	drv_memory_set_limit_boundary_mode(display, enable);
}

void nonlibdma_drvif_memory_compression_rpc(unsigned char display)
{
	drvif_memory_compression_rpc(display);
}

void nonlibdma_drv_memory_set_sub_gate_vo1(UINT8 a_bEnable)
{
	drv_memory_set_sub_gate_vo1(a_bEnable);
}

void nonlibdma_dvrif_memory_setting_for_data_fs(void)
{
	dvrif_memory_setting_for_data_fs();
}

void nonlibdma_drvif_memory_set_fs_display_fifo(void)
{
	drvif_memory_set_fs_display_fifo();
}

void nonlibdma_dvrif_memory_comp_setting(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio)
{
	dvrif_memory_comp_setting(enable_compression, comp_wid, comp_len, comp_ratio);
}

void nonlibdma_dvrif_memory_comp_setting_sub(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio)
{
	dvrif_memory_comp_setting_sub(enable_compression, comp_wid, comp_len, comp_ratio);
}

void nonlibdma_dvrif_memory_handler(unsigned char display)
{
	dvrif_memory_handler(display);
}

void nonlibdma_drv_memory_wait_mdom_vi_last_write_done(unsigned char display, unsigned char wait_frame)
{
	drv_memory_wait_mdom_vi_last_write_done(display, wait_frame);
}

void nonlibdma_drv_memory_Set_multibuffer_flag(UINT8 enable)
{
	drv_memory_Set_multibuffer_flag(enable);
}

UINT8 nonlibdma_drv_memory_Get_multibuffer_flag(void)
{
	return drv_memory_Get_multibuffer_flag();
}

void nonlibdma_drv_memory_wait_cap_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case)
{
	drv_memory_wait_cap_last_write_done(display, wait_frame, game_mode_case);
}

void nonlibdma_drv_memory_Set_multibuffer_number(UINT8 number)
{
	drv_memory_Set_multibuffer_number(number);
}

UINT8 nonlibdma_drv_memory_Get_multibuffer_number(void)
{
	return drv_memory_Get_multibuffer_number();
}

void nonlibdma_drv_memory_Set_multibuffer_number_pre(UINT8 number)
{
	drv_memory_Set_multibuffer_number_pre(number);
}

UINT8 nonlibdma_drv_memory_Get_multibuffer_number_pre(void)
{
	return drv_memory_Get_multibuffer_number_pre();
}

void nonlibdma_drvif_memory_EnableMDisp_Or_DisableMDisp(unsigned char display, bool enable)
{
	drvif_memory_EnableMDisp_Or_DisableMDisp(display, enable);
}

void nonlibdma_drvif_memory_set_dbuffer_write(unsigned char display)
{
	drvif_memory_set_dbuffer_write(display);
}

void nonlibdma_drvif_memory_set_memtag_startaddr(unsigned int addr,UINT8 index)
{
	drvif_memory_set_memtag_startaddr(addr, index);
}

unsigned int nonlibdma_drvif_memory_get_memtag_startaddr(UINT8 index)
{
	return drvif_memory_get_memtag_startaddr(index);
}

char nonlibdma_drv_memory_MA_SNR_Disable(unsigned char MEM_idx)
{
	return drv_memory_MA_SNR_Disable(MEM_idx);
}

struct semaphore *nonlibdma_get_main_mdomain_ctrl_semaphore(void)
{
	return get_main_mdomain_ctrl_semaphore();
}

void nonlibdma_drvif_memory_DisableMCap(unsigned char display)
{
	drvif_memory_DisableMCap(display);
}

unsigned long nonlibdma_drvif_memory_get_mainblock_viraddr(void)
{
	return drvif_memory_get_mainblock_viraddr();
}

unsigned long nonlibdma_drvif_memory_get_subblock_viraddr(void)
{
	return drvif_memory_get_subblock_viraddr();
}

unsigned int nonlibdma_drvif_memory_get_block_size(eMemIdx Idx)
{
	return drvif_memory_get_block_size((eMemIdx)Idx);
}

unsigned int nonlibdma_drvif_memory_get_block_addr(eMemIdx Idx)
{
	return drvif_memory_get_block_addr((eMemIdx)Idx);
}

void nonlibdma_drv_memory_Set_multibuffer_changed(UINT8 enable)
{
	drv_memory_Set_multibuffer_changed(enable);
}

UINT8 nonlibdma_drv_memory_Get_multibuffer_changed(void)
{
	return drv_memory_Get_multibuffer_changed();
}

void nonlibdma_drv_memory_send_multibuffer_number_to_smoothtoogh(void)
{
	drv_memory_send_multibuffer_number_to_smoothtoogh();
}

unsigned char nonlibdma_drv_memory_get_game_mode_iv2dv_slow_enable(void)
{
	return drv_memory_get_game_mode_iv2dv_slow_enable();
}

void nonlibdma_drv_memory_GameMode_Switch_TripleBuffer(void)
{
	drv_memory_GameMode_Switch_TripleBuffer();
}

void nonlibdma_drv_memory_GameMode_Switch_SingleBuffer(void)
{
	drv_memory_GameMode_Switch_SingleBuffer();
}

void nonlibdma_drvif_memory_EnableMCap(unsigned char display)
{
	drvif_memory_EnableMCap(display);
}

void nonlibdma_drvif_memory_init(void)
{
	drvif_memory_init();
}

void nonlibdma_drvif_memory_release(void)
{
	drvif_memory_release();
}

unsigned int nonlibdma_drv_memory_blk_decontour_Config(unsigned int phyAddr)// ml8_add
{
	return drv_memory_blk_decontour_Config(phyAddr);
}

unsigned char nonlibdma_drv_memory_Get_MASNR_Mem_Ready_Flag(unsigned char MEM_IDX)
{
	return drv_memory_Get_MASNR_Mem_Ready_Flag(MEM_IDX);
}

unsigned char nonlibdma_drv_memory_Get_I_BLK_decontour_Mem_Ready_Flag(unsigned char MEM_IDX)
{
	return drv_memory_Get_I_BLK_decontour_Mem_Ready_Flag(MEM_IDX);
}

char nonlibdma_drv_memory_di_Set_PQC_init(void)
{
	return drv_memory_di_Set_PQC_init();
}

unsigned char nonlibdma_drv_memory_get_DMA_Aligned_Val(void)
{
	return drv_memory_get_DMA_Aligned_Val();
}

unsigned long nonlibdma_get_query_start_address(unsigned char idx)
{
	return get_query_start_address(idx);
}

void nonlibdma_drv_memory_disable_di_write_req(unsigned char disableDiWr)
{
	drv_memory_disable_di_write_req(disableDiWr);
}

void nonlibdma_drv_memory_set_ip_fifo(unsigned char access)
{
	drv_memory_set_ip_fifo(access);
}

unsigned int nonlibdma_drvif_memory_alloc_block(eMemIdx Idx, unsigned int space, unsigned char Align)
{
	return drvif_memory_alloc_block((eMemIdx)Idx, space, Align);
}

void nonlibdma_drvif_memory_free_block(eMemIdx Idx)
{
	drvif_memory_free_block((eMemIdx)Idx);
}

void nonlibdma_drvif_memory_set_3D_LR_swap(unsigned char lr_swap, unsigned char type_3d)
{
	drvif_memory_set_3D_LR_swap(lr_swap, type_3d);
}

#ifdef ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE // SG 3D PANEL SUPPORT ALL SOURCE 3D MODE
void nonlibdma_drvif_memory_set_3D_LR_swap_for_SG_frc_in_mDomain(unsigned char bInvMode)
{
	drvif_memory_set_3D_LR_swap_for_SG_frc_in_mDomain(bInvMode);
}
#endif

void nonlibdma_drvif_memory_set_MVC_FRC_LR_swap(unsigned char bInvMode)
{
	return drvif_memory_set_MVC_FRC_LR_swap(bInvMode);
}

unsigned char nonlibdma_m_domain_4k_memory_from_type(void)
{
	return m_domain_4k_memory_from_type();
}

unsigned int nonlibdma_drv_GameMode_decided_iv2dv_delay_old_mode(void)
{
	return drv_GameMode_decided_iv2dv_delay_old_mode();
}

unsigned int nonlibdma_drv_GameMode_decided_iv2dv_delay_new_mode(void)
{
	return drv_GameMode_decided_iv2dv_delay_new_mode();
}

unsigned char nonlibdma_CMA_alloc_m_domain_original_buffer(void)
{
	return CMA_alloc_m_domain_original_buffer();
}

void nonlibdma_CMA_release_m_domain_original_buffer(void)
{
	CMA_release_m_domain_original_buffer();
}

void nonlibdma_set_mdomain_driver_status(unsigned char status)
{
	set_mdomain_driver_status(status);
}

void nonlibdma_mdomain_handler_onlycap(void)
{
	mdomain_handler_onlycap();
}

void nonlibdma_memory_set_sub_displaywindow_change(unsigned char changetomain)
{
	memory_set_sub_displaywindow_change(changetomain);
}

void nonlibdma_memory_set_main_displaywindow_change(void)
{
	memory_set_main_displaywindow_change();
}

bool nonlibdma_is_hdmi_dolby_vision_sink_led(void)
{
	return is_hdmi_dolby_vision_sink_led();
}

void nonlibdma_drv_game_mode_timing_Dynamic(unsigned char enable)
{
	drv_game_mode_timing_Dynamic(enable);
}

void nonlibdma_dvrif_LowDelay_memory_handler(unsigned char b_switchToSingleBuffer)
{
	dvrif_LowDelay_memory_handler(b_switchToSingleBuffer);
}

void nonlibdma_drv_game_mode_disp_smooth_variable_setting(UINT8 disp_type)
{
	drv_game_mode_disp_smooth_variable_setting(disp_type);
}

void nonlibdma_main_mdomain_ctrl_semaphore_init(void)
{
	main_mdomain_ctrl_semaphore_init();
}

void nonlibdma_drv_GameMode_iv2dv_delay(unsigned char enable)
{
	drv_GameMode_iv2dv_delay(enable);
}

unsigned int nonlibdma_drv_GameMode_iv2dv_delay_compress_margin(unsigned int iv2dv_ori)
{
	return drv_GameMode_iv2dv_delay_compress_margin(iv2dv_ori);
}

void nonlibdma_dvrif_memory_set_frc_style(UINT8 Display, UINT8 ucEnable)
{
	dvrif_memory_set_frc_style(Display, ucEnable);
}

void nonlibdma_set_sub_m_domain_1st_buf_address(void *addr)
{
	set_sub_m_domain_1st_buf_address(addr);
}

void nonlibdma_set_sub_m_domain_2nd_buf_address(void *addr)
{
	set_sub_m_domain_2nd_buf_address(addr);
}

void nonlibdma_set_sub_m_domain_3rd_buf_address(void *addr)
{
	set_sub_m_domain_3rd_buf_address(addr);
}

void *nonlibdma_get_sub_m_domain_1st_buf_address(void)
{
	return get_sub_m_domain_1st_buf_address();
}

void *nonlibdma_get_sub_m_domain_2nd_buf_address(void)
{
	return get_sub_m_domain_2nd_buf_address();
}

void *nonlibdma_get_sub_m_domain_3rd_buf_address(void)
{
	return get_sub_m_domain_3rd_buf_address();
}

void nonlibdma_wait_sub_disp_double_buffer_apply(void)
{
	wait_sub_disp_double_buffer_apply();
}

void nonlibdma_drvif_sub_mdomain_for_srnn(unsigned char enable)
{
	drvif_sub_mdomain_for_srnn(enable);
}

unsigned int nonlibdma_drv_memory_di_buffer_get(void)
{
	return drv_memory_di_buffer_get();
}

void nonlibdma_drvif_memory_set_3d_startaddr(unsigned int addr, UINT8 index)
{
	drvif_memory_set_3d_startaddr(addr, index);
}

UINT8 nonlibdma_drv_memory_Get_capture_multibuffer_flag(void)
{
	return drv_memory_Get_capture_multibuffer_flag();
}

#ifdef CONFIG_MPIXEL_SHIFT
void nonlibdma_memory_black_buf_init(unsigned int framesize, unsigned int upperPhyAddress)
{
	memory_black_buf_init(framesize, upperPhyAddress);
}
#endif

StructMemBlock* nonlibdma_drvif_memory_get_MemTag_addr(UINT8 index)
{
	return (StructMemBlock*)drvif_memory_get_MemTag_addr(index);
}

unsigned char nonlibdma_get_mdomain_driver_status(void)
{
	return get_mdomain_driver_status();
}

void nonlibdma_clear_m_cap_done_status(unsigned char display)
{
	clear_m_cap_done_status(display);
}

void nonlibdma_record_current_m_cap_block(unsigned char display)
{
	record_current_m_cap_block(display);
}

void nonlibdma_drv_memory_debug_print_buf_idx(void)
{
	drv_memory_debug_print_buf_idx();
}

unsigned int nonlibdma_memory_get_line_size(unsigned int LineSize, unsigned char InputFormat, unsigned char BitNum)
{
	return memory_get_line_size(LineSize, InputFormat, BitNum);
}

void nonlibdma_drv_run_GameMode_iv2dv_slow_tuning(void)
{
	extern void drv_run_GameMode_iv2dv_slow_tuning(void);
	drv_run_GameMode_iv2dv_slow_tuning();
}

void nonlibdma_memory_division(unsigned int Dividend, unsigned char Divisor, unsigned int* Quotient, unsigned char* Remainder)
{
	memory_division(Dividend, Divisor, Quotient, Remainder);
}

void nonlibdma_drv_memory_capture_multibuffer_switch_buffer_number(void)
{
	drv_memory_capture_multibuffer_switch_buffer_number();
}

void nonlibdma_drv_memory_display_multibuffer_switch_buffer_number(void)
{
	drv_memory_display_multibuffer_switch_buffer_number();
}

#ifdef BUILD_QUICK_SHOW
void nonlibdma_scaler_quickshow_memery_init(void)
{
	scaler_quickshow_memery_init();
}

unsigned long nonlibdma_qs_get_memory_from_vbm(MDOMAIN_MEMORY_QUERY_ID id, unsigned long allocSize)
{
	return qs_get_memory_from_vbm((MDOMAIN_MEMORY_QUERY_ID)id, allocSize);
}
#endif



void nonlibdma_framesyncGattingDoQS_mainSubCtrl(void)
{
	unsigned int cur_time = 0;

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	if (Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_422CAP))
	{
		// 0xb8027328[17]=0; 422
		IoReg_Write32(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg : MDOMAIN_DISP_DDR_MainCtrl_reg,  _BIT2|_BIT1|(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_10BIT) ? _BIT16 : 0));
	}
	else
	{
		// 0xb8027328[17]=1; 444
		IoReg_Write32(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg : MDOMAIN_DISP_DDR_MainCtrl_reg,  _BIT2|_BIT1|_BIT17|(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_10BIT) ?_BIT16 : 0));
	}
	// Apply double buffer
	IoReg_SetBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg, _BIT0);
	cur_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);//save current time for timeout check
	while(IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg) & _BIT0)
	{
		if(!PPOVERLAY_Display_Timing_CTRL1_get_disp_en(IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg)))
		{
			rtd_pr_vsc_err("###[ERR]func:%s 0xb8028000[24] disable ###\r\n", __FUNCTION__);
			break;//0xb8028000[24] not set
		}

		if(drvif_wait_timeout_check_by_counter(cur_time, 100))
		{
			rtd_pr_vsc_err("###[ERR]func:%s timeout ###\r\n", __FUNCTION__);
			break;//wait 100ms timeout
		}
	}
#else
	if (Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_422CAP))
	{
		IoReg_Write32(/*Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg : */MDOMAIN_DISP_DDR_MainCtrl_reg,  _BIT2|_BIT1|(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_10BIT) ? _BIT16 : 0));
	}
	else
	{
		IoReg_Write32(/*Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_DISP_DDR_SubCtrl_reg : */MDOMAIN_DISP_DDR_MainCtrl_reg,  _BIT2|_BIT1|_BIT17|(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_10BIT) ? _BIT16 : 0));
	}
	// write data & enable double buffe
	IoReg_SetBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg, _BIT0);
	cur_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);//save current time for timeout check
	while(IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg) & _BIT0)
	{
		if(!PPOVERLAY_Display_Timing_CTRL1_get_disp_en(IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg)))
		{
			rtd_pr_vsc_err("###[ERR]func:%s 0xb8028000[24] disable ###\r\n", __FUNCTION__);
			break;//0xb8028000[24] not set
		}

		if(drvif_wait_timeout_check_by_counter(cur_time, 100))
		{
			rtd_pr_vsc_err("###[ERR]func:%s timeout ###\r\n", __FUNCTION__);
			break;//wait 100ms timeout
		}
	}
#endif

}

void nonlibdma_framesyncGattingDoQS_mainSubSdnrCxxto64(void)
{
	mdomain_cap_main_sdnr_cxxto64_RBUS mdomain_cap_main_sdnr_cxxto64_reg;
#ifdef CONFIG_DUAL_CHANNEL
	mdomain_cap_sub_sdnr_cxxto64_RBUS mdomain_cap_sub_sdnr_cxxto64_reg;
#endif

#ifdef CONFIG_DUAL_CHANNEL  /*SDNR output bit and sort format, new add in magellan2 @Crixus 20150330*/
	if(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1) { /*main channel*/
		mdomain_cap_main_sdnr_cxxto64_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg);
		mdomain_cap_main_sdnr_cxxto64_reg.out_bit = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_10BIT);
		mdomain_cap_main_sdnr_cxxto64_reg.sort_fmt = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_422CAP) ? 0 : 1;
		IoReg_Write32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg, mdomain_cap_main_sdnr_cxxto64_reg.regValue);
	} else {
		mdomain_cap_sub_sdnr_cxxto64_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Sub_SDNR_cxxto64_reg);
		mdomain_cap_sub_sdnr_cxxto64_reg.sub_out_bit = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_10BIT);
		mdomain_cap_sub_sdnr_cxxto64_reg.sub_sort_fmt = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_422CAP) ? 0 : 1;
		IoReg_Write32(MDOMAIN_CAP_Sub_SDNR_cxxto64_reg, mdomain_cap_sub_sdnr_cxxto64_reg.regValue);
	}
#else
	mdomain_cap_main_sdnr_cxxto64_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg);
	mdomain_cap_main_sdnr_cxxto64_reg.out_bit = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_10BIT);
	mdomain_cap_main_sdnr_cxxto64_reg.sort_fmt = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_422CAP) ? 0 : 1;
	IoReg_Write32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg, mdomain_cap_main_sdnr_cxxto64_reg.regValue);
#endif
}

void nonlibdma_framesyncGattingDo_mainSubCtrl(void)
{
	unsigned int cur_time = 0;
	mdomain_disp_ddr_mainctrl_RBUS disp_ddr_mainctrl_reg;

	disp_ddr_mainctrl_reg.regValue =IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
	disp_ddr_mainctrl_reg.main_source_sel = 3;
#ifdef CONFIG_DATA_FRAMESYNC_FORCE_422_CAP
	// Force 422
	disp_ddr_mainctrl_reg.main_in_format = !(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_422CAP));
#else
	disp_ddr_mainctrl_reg.main_in_format = (!(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_422CAP)) || dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY));
#endif
	disp_ddr_mainctrl_reg.main_bit_num = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT);
	disp_ddr_mainctrl_reg.main_v_flip_3buf_en = 0;
	disp_ddr_mainctrl_reg.main_double_en = 0;
	disp_ddr_mainctrl_reg.main_decompression_clk_en = 0;
	disp_ddr_mainctrl_reg.main_cap2disp_distance = 0;
	disp_ddr_mainctrl_reg.main_bufnum = 0;
	IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg,disp_ddr_mainctrl_reg.regValue);
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels

	// Apply double buffer
	IoReg_SetBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg,  MDOMAIN_DISP_DDR_MainSubCtrl_disp1_double_apply_mask);
	if(is_scaler_input_2p_mode(SLR_MAIN_DISPLAY))
	{
		IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg));
		//apply double buffer
		IoReg_SetBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg,  MDOMAIN_DISP_DDR_MainSubCtrl_disp2_double_apply_shift);
	}
	cur_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);//save current time for timeout check
	while(IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg)& MDOMAIN_DISP_DDR_MainSubCtrl_disp1_double_apply_mask)
	{
		if(!PPOVERLAY_Display_Timing_CTRL1_get_disp_en(IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg)))
		{
			rtd_pr_vbe_err("###[ERR]func:%s 0xb8028000[24] disable ###\r\n",__FUNCTION__);
			break;//0xb8028000[24] not set
		}

		if(drvif_wait_timeout_check_by_counter(cur_time,100))
		{
			rtd_pr_vbe_err("###[ERR]func:%s timeout ###\r\n",__FUNCTION__);
			break;//wait 100ms timeout
		}
	}

#else
	// write data & enable double buffe
	IoReg_SetBits(MDOMAIN_DISP_DDR_MainSubCtrl_reg ,	_BIT0);
	cur_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);//save current time for timeout check
	while(IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg)& _BIT0)
	{
		if(!PPOVERLAY_Display_Timing_CTRL1_get_disp_en(IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg)))
		{
			rtd_pr_vbe_err("###[ERR]func:%s 0xb8028000[24] disable ###\r\n",__FUNCTION__);
			break;//0xb8028000[24] not set
		}

		if(drvif_wait_timeout_check_by_counter(cur_time,100))
		{
			rtd_pr_vbe_err("###[ERR]func:%s timeout ###\r\n",__FUNCTION__);
			break;//wait 100ms timeout
		}
	}

#endif
}

void nonlibdma_framesyncGattingDo_mainSubSdnrCxxto64(void)
{
	mdomain_cap_main_sdnr_cxxto64_RBUS mdomain_cap_main_sdnr_cxxto64_reg;
#ifdef CONFIG_DUAL_CHANNEL
	mdomain_cap_sub_sdnr_cxxto64_RBUS mdomain_cap_sub_sdnr_cxxto64_reg;
#endif

#ifdef CONFIG_DUAL_CHANNEL  /*SDNR output bit and sort format, new add in magellan2 @Crixus 20150330*/
	if(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1) { /*main channel*/
		mdomain_cap_main_sdnr_cxxto64_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg);
		mdomain_cap_main_sdnr_cxxto64_reg.out_bit = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_10BIT);
		mdomain_cap_main_sdnr_cxxto64_reg.sort_fmt = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_422CAP) ? 0 : 1;
		IoReg_Write32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg, mdomain_cap_main_sdnr_cxxto64_reg.regValue);
		if(is_scaler_input_2p_mode(SLR_MAIN_DISPLAY))
			IoReg_Write32(MDOMAIN_CAP_Sub_SDNR_cxxto64_reg, IoReg_Read32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg));
	} else {
		mdomain_cap_sub_sdnr_cxxto64_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Sub_SDNR_cxxto64_reg);
		mdomain_cap_sub_sdnr_cxxto64_reg.sub_out_bit = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_10BIT);
		mdomain_cap_sub_sdnr_cxxto64_reg.sub_sort_fmt = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_422CAP) ? 0 : 1;
		IoReg_Write32(MDOMAIN_CAP_Sub_SDNR_cxxto64_reg, mdomain_cap_sub_sdnr_cxxto64_reg.regValue);
	}
#else
	mdomain_cap_main_sdnr_cxxto64_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg);
	mdomain_cap_main_sdnr_cxxto64_reg.out_bit = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_10BIT);
	mdomain_cap_main_sdnr_cxxto64_reg.sort_fmt = Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL), SLR_DISP_422CAP) ? 0 : 1;
	IoReg_Write32(MDOMAIN_CAP_Main_SDNR_cxxto64_reg, mdomain_cap_main_sdnr_cxxto64_reg.regValue);
#endif
}

void nonlibdma_framesyncDo_mainSubCtrl(void)
{
	unsigned int timeout = 0;
	mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;
	mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	mdomain_disp_ddr_subctrl_RBUS ddr_subctrl_reg;
#endif

	if(_CHANNEL1 == Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))
	{
		// Source_Sel[1:0]
		// 00 : use info. from Cap#1.
		// 01 : use info. from Cap#2.
		// 1x : frame sync mode.
		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		ddr_mainctrl_reg.main_source_sel = 3;
		//ddr_mainctrl_reg.main_in_format = 1; // 444
		ddr_mainctrl_reg.main_in_format = !Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_422CAP);
		ddr_mainctrl_reg.main_bit_num = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT); // 8bit or 10bit
		IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);

		ddr_mainsubctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
		ddr_mainsubctrl_reg.disp1_double_enable = _ENABLE; // enable double buffer
		IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_reg, ddr_mainsubctrl_reg.regValue);

		timeout = 0xffff;
		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		while(((1 != ddr_mainctrl_reg.main_in_format) || (Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT) != ddr_mainctrl_reg.main_bit_num)) && (timeout))
		{
			ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
			timeout--;
			msleep(10);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else// if(_CHANNEL2 == info->channel)
	{
		// Source_Sel[1:0]
		// 00 : use info. from Cap#1.
		// 01 : use info. from Cap#2.
		// 1x : frame sync mode.
		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		ddr_subctrl_reg.sub_source_sel = 2;
		ddr_subctrl_reg.sub_in_format = 1; // 444
		ddr_subctrl_reg.sub_bit_num = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT); // 8bit or 10bit
		IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, ddr_subctrl_reg.regValue);


		ddr_mainsubctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
		ddr_mainsubctrl_reg.disp2_double_enable = _ENABLE; // enable double buffer
		IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_reg, ddr_mainsubctrl_reg.regValue);

		timeout = 0xffff;
		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		while(((1 != ddr_subctrl_reg.sub_in_format) || (Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT) != ddr_subctrl_reg.sub_bit_num)) && (timeout))
		{
			ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
			timeout--;
		}
	}
#endif

}

void nonlibdma_memory_set_sub_displaywindow_DispVT(VIDEO_RECT_T s_dispwin)
{
	mdomain_disp_ddr_sublinestep_RBUS mdomain_disp_ddr_sublinestep_reg;
	mdomain_disp_ddr_sublinenum_RBUS mdomain_disp_ddr_sublinenum_reg;
	mdomain_disp_ddr_subrdnumlenrem_RBUS mdomain_disp_ddr_subrdnumlenrem_reg;
	mdomain_disp_ddr_subpixnumwtl_RBUS mdomain_disp_ddr_subpixnumwtl_reg;
	SCALER_DISP_CHANNEL display = SLR_SUB_DISPLAY;
	unsigned int TotalSize;
	unsigned int Quotient;
	unsigned char Remainder;
	unsigned int fifoLen = _FIFOLENGTH2;
	unsigned int burstLen = _BURSTLENGTH2;
	unsigned int MemShiftAddr = 0;
	unsigned short droppixel = 0;
	unsigned int subctrltemp = 0;
	mdomain_disp_ddr_subprevstart_RBUS mdomain_disp_ddr_subprevstart_Reg;
	mdomain_disp_display_sub_byte_channel_swap_RBUS display_sub_byte_channel_swap_reg;

	mdomain_disp_ddr_subprevstart_Reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_SubPreVStart_reg);
	if(s_dispwin.y <= 100) {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = 7;
	} else {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = (s_dispwin.y-4);
	}
	rtd_outl(MDOMAIN_DISP_DDR_SubPreVStart_reg, mdomain_disp_ddr_subprevstart_Reg.regValue);


	MemGetBlockShiftAddr(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA),Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA), &MemShiftAddr, &droppixel);
	TotalSize = memory_get_disp_line_size1(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) + droppixel);	// calculate the memory size of capture
	mdomain_disp_ddr_subpixnumwtl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg);
	mdomain_disp_ddr_subpixnumwtl_reg.sub_pixel_num = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) +droppixel) & 0x3FFF;
	mdomain_disp_ddr_subpixnumwtl_reg.sub_wtlvl = ((fifoLen - (burstLen>>1))>>1);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg, mdomain_disp_ddr_subpixnumwtl_reg.regValue);

	memory_division(TotalSize, burstLen, &Quotient, &Remainder);	// Do a division

	// remainder is not allowed to be zero
	if (Remainder == 0) {
		Remainder = burstLen;
		Quotient -= 1;
	}


	if((dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE) && (dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_FRAME_MODE))
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = 0xffff;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = burstLen;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}
	else
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = Quotient;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = Remainder;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}

	TotalSize = memory_get_capture_size(display, MEMCAPTYPE_LINE);
	/*TotalSize = Quotient * burstLen + Remainder; 	// count one line, 64bits unit
	TotalSize = drvif_memory_get_data_align(TotalSize, 4);	// times of 4*/

	mdomain_disp_ddr_sublinestep_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineStep_reg);
	mdomain_disp_ddr_sublinestep_reg.sub_line_step = (TotalSize + TotalSize%2);//bit 3 need to be 0. rbus rule
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineStep_reg, mdomain_disp_ddr_sublinestep_reg.regValue);

	TotalSize = SHL(TotalSize, 3); // unit conversion from 64bits to 8bits

	mdomain_disp_ddr_sublinenum_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineNum_reg);
	mdomain_disp_ddr_sublinenum_reg.sub_line_number = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN) & 0x0FFF);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineNum_reg, mdomain_disp_ddr_sublinenum_reg.regValue);

	rtd_outl(MDOMAIN_DISP_DDR_SubAddr_reg, (CaptureCtrl_VT.cap_buffer[0].phyaddr + MemShiftAddr) &0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub2ndAddr_reg, (CaptureCtrl_VT.cap_buffer[1].phyaddr + MemShiftAddr) &0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub3rdAddr_reg, (CaptureCtrl_VT.cap_buffer[2].phyaddr + MemShiftAddr) &0xfffffff0);

	rtd_outl(MDOMAIN_DISP_DDR_SubAddrDropBits_reg,droppixel);

	subctrltemp = _BIT6;
	subctrltemp |=_BIT7;//set Mdomain display triple buffer
	//subctrltemp |= (_BIT1);
	//disp ctrl default setting
	subctrltemp |= (_BIT25 | _BIT28);
	subctrltemp |= Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) ? 0 : _BIT17;
	//subctrltemp |= (_BIT19);/*for VT data is RGB*/
	rtd_outl(MDOMAIN_DISP_DDR_SubCtrl_reg, subctrltemp);

	display_sub_byte_channel_swap_reg.regValue = rtd_inl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg);
	display_sub_byte_channel_swap_reg.sub_1byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_2byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_4byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_8byte_swap = 0;
	display_sub_byte_channel_swap_reg.sub_channel_swap = 4;
	rtd_outl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg, display_sub_byte_channel_swap_reg.regValue);
	rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, _BIT0);
}

void nonlibdma_memory_set_displaysubwindow_DispVT(VIDEO_RECT_T s_dispwin, unsigned long phyaddr)
{
	mdomain_disp_ddr_sublinestep_RBUS mdomain_disp_ddr_sublinestep_reg;
	mdomain_disp_ddr_sublinenum_RBUS mdomain_disp_ddr_sublinenum_reg;
	mdomain_disp_ddr_subrdnumlenrem_RBUS mdomain_disp_ddr_subrdnumlenrem_reg;
	mdomain_disp_ddr_subpixnumwtl_RBUS mdomain_disp_ddr_subpixnumwtl_reg;
	SCALER_DISP_CHANNEL display = SLR_SUB_DISPLAY;
	unsigned int TotalSize;
	unsigned int Quotient;
	unsigned char Remainder;
	unsigned int fifoLen = _FIFOLENGTH2;
	unsigned int burstLen = _BURSTLENGTH2;
	unsigned int MemShiftAddr = 0;
	unsigned short droppixel = 0;
	unsigned int subctrltemp = 0;
	mdomain_disp_ddr_subprevstart_RBUS mdomain_disp_ddr_subprevstart_Reg;
	mdomain_disp_display_sub_byte_channel_swap_RBUS display_sub_byte_channel_swap_reg;

	mdomain_disp_ddr_subprevstart_Reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_SubPreVStart_reg);
	if(s_dispwin.y <= 100) {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = 7;
	} else {
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = (s_dispwin.y-4);
	}
	rtd_outl(MDOMAIN_DISP_DDR_SubPreVStart_reg, mdomain_disp_ddr_subprevstart_Reg.regValue);


	MemGetBlockShiftAddr(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA),Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA), &MemShiftAddr, &droppixel);
	TotalSize = memory_get_disp_line_size1(display, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) + droppixel);	// calculate the memory size of capture

	mdomain_disp_ddr_subpixnumwtl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg);
	mdomain_disp_ddr_subpixnumwtl_reg.sub_pixel_num = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) +droppixel) & 0x3FFF;
	mdomain_disp_ddr_subpixnumwtl_reg.sub_wtlvl = ((fifoLen - (burstLen>>1))>>1);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg, mdomain_disp_ddr_subpixnumwtl_reg.regValue);

	memory_division(TotalSize, burstLen, &Quotient, &Remainder);	// Do a division

	// remainder is not allowed to be zero
	if (Remainder == 0) {
		Remainder = burstLen;
		Quotient -= 1;
	}

	if((dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE) && (dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_FRAME_MODE))
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = 0xffff;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = burstLen;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}
	else
	{
		mdomain_disp_ddr_subrdnumlenrem_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg);
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_num1 = Quotient;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_len = burstLen;
		mdomain_disp_ddr_subrdnumlenrem_reg.sub_read_remain = Remainder;
		IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, mdomain_disp_ddr_subrdnumlenrem_reg.regValue);
	}


	TotalSize = memory_get_capture_size(display, MEMCAPTYPE_LINE);
	/*TotalSize = Quotient * burstLen + Remainder; 	// count one line, 64bits unit
	TotalSize = drvif_memory_get_data_align(TotalSize, 4);	// times of 4*/


	mdomain_disp_ddr_sublinestep_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineStep_reg);
	mdomain_disp_ddr_sublinestep_reg.sub_line_step = (TotalSize + TotalSize%2);//bit 3 need to be 0. rbus rule
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineStep_reg, mdomain_disp_ddr_sublinestep_reg.regValue);

	TotalSize = SHL(TotalSize, 3); // unit conversion from 64bits to 8bits

	mdomain_disp_ddr_sublinenum_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubLineNum_reg);
	mdomain_disp_ddr_sublinenum_reg.sub_line_number = (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN) & 0x0FFF);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineNum_reg, mdomain_disp_ddr_sublinenum_reg.regValue);

#if 1
	rtd_outl(MDOMAIN_DISP_DDR_SubAddr_reg, (phyaddr + MemShiftAddr) &0xfffffff0);
#else
	rtd_outl(MDOMAIN_DISP_DDR_SubAddr_reg, (CaptureCtrl_VT.cap_buffer[0].phyaddr + MemShiftAddr) & 0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub2ndAddr_reg, (CaptureCtrl_VT.cap_buffer[1].phyaddr + MemShiftAddr) & 0xfffffff0);
	rtd_outl(MDOMAIN_DISP_DDR_Sub3rdAddr_reg, (CaptureCtrl_VT.cap_buffer[2].phyaddr + MemShiftAddr) & 0xfffffff0);
#endif

	rtd_outl(MDOMAIN_DISP_DDR_SubAddrDropBits_reg,droppixel);

//	subctrltemp = _BIT6;
//	subctrltemp |=_BIT7;//set Mdomain display triple buffer
	//subctrltemp |= (_BIT1);
	//disp ctrl default setting
	subctrltemp |= (_BIT25 | _BIT28);
	subctrltemp |= Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) ? 0 : _BIT17;
	//subctrltemp |= (_BIT19);/*for VT data is RGB*/
	rtd_outl(MDOMAIN_DISP_DDR_SubCtrl_reg, subctrltemp);

	display_sub_byte_channel_swap_reg.regValue = rtd_inl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg);
	display_sub_byte_channel_swap_reg.sub_1byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_2byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_4byte_swap = 1;
	display_sub_byte_channel_swap_reg.sub_8byte_swap = 0;
	display_sub_byte_channel_swap_reg.sub_channel_swap = 4;
	rtd_outl(MDOMAIN_DISP_Display_sub_byte_channel_swap_reg, display_sub_byte_channel_swap_reg.regValue);
	rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, _BIT0);
}



void nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_enable(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;

	cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
	if(mainSub == 1) {//sub
		cap_reg_doublbuffer_reg.cap2_db_en = enable;
	} else if(mainSub == 0) { //main
		cap_reg_doublbuffer_reg.cap1_db_en = enable;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}

	rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
}

void nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_apply(int mainSub, unsigned int apply)
//main: 0, sub: 1
{
	mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;

	cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
	if(mainSub == 1) { //sub
		cap_reg_doublbuffer_reg.cap2_db_apply = apply;
	} else if(mainSub == 0) { //main
		cap_reg_doublbuffer_reg.cap1_db_apply = apply;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}

	rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
}

void nonlibdma_set_cap_inctrl_reg_in_freeze_enable(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub in2
		mdomain_cap_ddr_in2ctrl_RBUS mdomain_cap_ddr_in2ctrl_reg;

		mdomain_cap_ddr_in2ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In2Ctrl_reg);
		mdomain_cap_ddr_in2ctrl_reg.in2_freeze_enable = enable;

		IoReg_Write32(MDOMAIN_CAP_DDR_In2Ctrl_reg, mdomain_cap_ddr_in2ctrl_reg.regValue);
	} else if(mainSub == 0) { //main in1
		mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;

		mdomain_cap_ddr_in1ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Ctrl_reg);
		mdomain_cap_ddr_in1ctrl_reg.in1_freeze_enable = enable;

		IoReg_Write32(MDOMAIN_CAP_DDR_In1Ctrl_reg, mdomain_cap_ddr_in1ctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_cap_inctrl_reg_in_freeze_option(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub in2
		mdomain_cap_ddr_in2ctrl_RBUS mdomain_cap_ddr_in2ctrl_reg;

		mdomain_cap_ddr_in2ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In2Ctrl_reg);
		mdomain_cap_ddr_in2ctrl_reg.in2_freeze_option = enable;

		IoReg_Write32(MDOMAIN_CAP_DDR_In2Ctrl_reg, mdomain_cap_ddr_in2ctrl_reg.regValue);
	} else if(mainSub == 0) { //main in1
		mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;

		mdomain_cap_ddr_in1ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Ctrl_reg);
		mdomain_cap_ddr_in1ctrl_reg.in1_freeze_option = enable;

		IoReg_Write32(MDOMAIN_CAP_DDR_In1Ctrl_reg, mdomain_cap_ddr_in1ctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_cap_inctrl_reg_double_enable(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub in2
		mdomain_cap_ddr_in2ctrl_RBUS mdomain_cap_ddr_in2ctrl_reg;

		mdomain_cap_ddr_in2ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In2Ctrl_reg);
		mdomain_cap_ddr_in2ctrl_reg.in2_double_enable = enable;

		rtd_outl(MDOMAIN_CAP_DDR_In2Ctrl_reg,mdomain_cap_ddr_in2ctrl_reg.regValue);
	} else if(mainSub == 0) { //main in1
		mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;

		mdomain_cap_ddr_in1ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Ctrl_reg);
		mdomain_cap_ddr_in1ctrl_reg.in1_double_enable = enable;

		rtd_outl(MDOMAIN_CAP_DDR_In1Ctrl_reg,mdomain_cap_ddr_in1ctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_cap_inctrl_reg_v_flip_3buf_en(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub in2
		mdomain_cap_ddr_in2ctrl_RBUS mdomain_cap_ddr_in2ctrl_reg;

		mdomain_cap_ddr_in2ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In2Ctrl_reg);
		mdomain_cap_ddr_in2ctrl_reg.in2_v_flip_3buf_en = enable;

		rtd_outl(MDOMAIN_CAP_DDR_In2Ctrl_reg,mdomain_cap_ddr_in2ctrl_reg.regValue);
	} else if(mainSub == 0) { //main in1
		mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;

		mdomain_cap_ddr_in1ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Ctrl_reg);
		mdomain_cap_ddr_in1ctrl_reg.in1_v_flip_3buf_en = enable;

		rtd_outl(MDOMAIN_CAP_DDR_In1Ctrl_reg,mdomain_cap_ddr_in1ctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_cap_inctrl_reg_freeze_enable(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub in2
		mdomain_cap_ddr_in2ctrl_RBUS mdomain_cap_ddr_in2ctrl_reg;

		mdomain_cap_ddr_in2ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In2Ctrl_reg);
		mdomain_cap_ddr_in2ctrl_reg.in2_freeze_enable = enable;

		rtd_outl(MDOMAIN_CAP_DDR_In2Ctrl_reg,mdomain_cap_ddr_in2ctrl_reg.regValue);
	} else if(mainSub == 0) { //main in1
		mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;

		mdomain_cap_ddr_in1ctrl_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Ctrl_reg);
		mdomain_cap_ddr_in1ctrl_reg.in1_freeze_enable = enable;

		rtd_outl(MDOMAIN_CAP_DDR_In1Ctrl_reg,mdomain_cap_ddr_in1ctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_cap_ich12_status_reg_buffer_overflow_underflow(int mainSub)
{
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
	if(mainSub == 1) { //sub
		mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf_sub = 1;
		mdomain_cap_ich12_ibuff_status_reg.fsync_ovf_sub = 1;
		mdomain_cap_ich12_ibuff_status_reg.fsync_udf_sub = 1;
		IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg,mdomain_cap_ich12_ibuff_status_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf = 1;
		mdomain_cap_ich12_ibuff_status_reg.fsync_ovf = 1;
		mdomain_cap_ich12_ibuff_status_reg.fsync_udf = 1;
		IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg,mdomain_cap_ich12_ibuff_status_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_cap_ie_wd_ctrl_reg_in_cap_last_wr_ie(int mainSub, unsigned int enable)
{
	mdomain_cap_cap_ie_wd_ctrl_RBUS mdomain_cap_ie_wd_ctrl_reg;

	mdomain_cap_ie_wd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap_IE_WD_CTRL_reg);
	if(mainSub == 1) { //sub in2
		mdomain_cap_ie_wd_ctrl_reg.in1_cap_last_wr_ie = enable;
	} else if(mainSub == 0) { //main in1
		mdomain_cap_ie_wd_ctrl_reg.in2_cap_last_wr_ie = enable;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}

	IoReg_Write32(MDOMAIN_CAP_Cap_IE_WD_CTRL_reg, mdomain_cap_ie_wd_ctrl_reg.regValue);
}

void nonlibdma_set_cap_boundaryaddr_ctrl2_reg_reset(int mainSub)
{
	if(mainSub == 1) { //sub in2
		IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr_CTRL2_reg, 0x00000000);
	} else if(mainSub == 0) { //main in1
		IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr_CTRL2_reg, 0x00000000);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_cap_smooth_tog_ctrl_0_reg_pqc_clken(int mainSub, unsigned int disable)
{
	mdomain_cap_smooth_tog_ctrl_0_RBUS mdomain_cap_smooth_tog_ctrl_0_reg;

	mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
	if(mainSub == 1) { //sub
		mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken = disable;
	} else if(mainSub == 0) { //main
		mdomain_cap_smooth_tog_ctrl_0_reg.in1_pqc_clken = disable;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}

	IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);
}

void nonlibdma_set_cap_pq_cmp_reg_cmp_en(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub
		mdomain_cap_caps_pq_cmp_RBUS mdomain_cap_caps_pq_cmp_reg;

		mdomain_cap_caps_pq_cmp_reg.regValue = IoReg_Read32(MDOMAIN_CAP_CAPS_PQ_CMP_reg);
		mdomain_cap_caps_pq_cmp_reg.s_cmp_en = enable;

		IoReg_Write32(MDOMAIN_CAP_CAPS_PQ_CMP_reg, mdomain_cap_caps_pq_cmp_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_cap_cap_pq_cmp_RBUS mdomain_cap_cap_pq_cmp_reg;

		mdomain_cap_cap_pq_cmp_reg.regValue = IoReg_Read32(MDOMAIN_CAP_CAP_PQ_CMP_reg);
		mdomain_cap_cap_pq_cmp_reg.cmp_en = enable;

		IoReg_Write32(MDOMAIN_CAP_CAP_PQ_CMP_reg, mdomain_cap_cap_pq_cmp_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	mdomain_disp_ddr_mainsubctrl_RBUS disp_ddr_mainsubctrl_reg;

	disp_ddr_mainsubctrl_reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
	if(mainSub == 1) { //sub
		disp_ddr_mainsubctrl_reg.disp2_double_enable = enable;
	} else if(mainSub == 0) { //main
		disp_ddr_mainsubctrl_reg.disp1_double_enable = enable;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}

	rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg, disp_ddr_mainsubctrl_reg.regValue);
}

void nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_apply(int mainSub, unsigned int apply)
//main: 0, sub: 1
{
	mdomain_disp_ddr_mainsubctrl_RBUS disp_ddr_mainsubctrl_reg;

	disp_ddr_mainsubctrl_reg.regValue = rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
	if(mainSub == 1) { //sub
		disp_ddr_mainsubctrl_reg.disp2_double_apply = apply;
	} else if(mainSub == 0) { //main
		disp_ddr_mainsubctrl_reg.disp1_double_apply = apply;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}

	rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg, disp_ddr_mainsubctrl_reg.regValue);
}

void nonlibdma_set_disp_enable_reg_disp_en(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
		mdomain_disp_disp_sub_enable_RBUS disp_sub_enable_reg;

		disp_sub_enable_reg.regValue = rtd_inl(MDOMAIN_DISP_Disp_sub_enable_reg);
		disp_sub_enable_reg.sub_disp_en = enable;

		rtd_outl(MDOMAIN_DISP_Disp_sub_enable_reg, disp_sub_enable_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_disp_main_enable_RBUS disp_main_enable_reg;

		disp_main_enable_reg.regValue = rtd_inl(MDOMAIN_DISP_Disp_main_enable_reg);
		disp_main_enable_reg.main_disp_en = enable;

		rtd_outl(MDOMAIN_DISP_Disp_main_enable_reg, disp_main_enable_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}

}

void nonlibdma_set_disp_mainctrl_reg_single_buffer_select(int mainSub, unsigned int block_sel)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
		mdomain_disp_ddr_subctrl_RBUS ddr_subctrl_reg;

		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		ddr_subctrl_reg.sub_double_en = 0;
		ddr_subctrl_reg.sub_single_buffer_select = block_sel;

		IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, ddr_subctrl_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;

		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		ddr_mainctrl_reg.main_double_en = 0;
		ddr_mainctrl_reg.main_single_buffer_select = block_sel;

		IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_disp_ctrl_reg_double_en(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
		mdomain_disp_ddr_subctrl_RBUS ddr_subctrl_reg;

		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		ddr_subctrl_reg.sub_double_en = enable;

		IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, ddr_subctrl_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;

		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		ddr_mainctrl_reg.main_double_en = enable;

		IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_disp_ctrl_reg_v_flip_3buf_en(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
		mdomain_disp_ddr_subctrl_RBUS ddr_subctrl_reg;

		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		ddr_subctrl_reg.sub_v_flip_3buf_en = enable;

		IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, ddr_subctrl_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;

		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		ddr_mainctrl_reg.main_v_flip_3buf_en = enable;

		IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_disp_ctrl_reg_frc_style(int mainSub, unsigned int frc_style)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
		mdomain_disp_ddr_subctrl_RBUS ddr_subctrl_reg;

		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		ddr_subctrl_reg.sub_frc_style = frc_style;

		IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, ddr_subctrl_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;

		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		ddr_mainctrl_reg.main_frc_style = frc_style;

		IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_disp_prevstart_reg_pre_rd_v_start(int mainSub, unsigned int preread)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
		mdomain_disp_ddr_subprevstart_RBUS mdomain_disp_ddr_subprevstart_Reg;

		mdomain_disp_ddr_subprevstart_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubPreVStart_reg);
		mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = preread;

		IoReg_Write32(MDOMAIN_DISP_DDR_SubPreVStart_reg, mdomain_disp_ddr_subprevstart_Reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_ddr_mainprevstart_RBUS mdomain_disp_ddr_mainprevstart_Reg;

		mdomain_disp_ddr_mainprevstart_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainPreVStart_reg);
		mdomain_disp_ddr_mainprevstart_Reg.main_pre_rd_v_start = preread;

		IoReg_Write32(MDOMAIN_DISP_DDR_MainPreVStart_reg, mdomain_disp_ddr_mainprevstart_Reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_disp_ddr_bm_dma_reg_dma_mode(int mainSub, unsigned int dmaMode)
{
	if(mainSub == 1) { //sub
		mdomain_disp_ddr_sub_bm_dma_RBUS disp_ddr_sub_dm_dma_reg;

		disp_ddr_sub_dm_dma_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_Sub_BM_DMA_reg);
		disp_ddr_sub_dm_dma_reg.sub_dma_mode = dmaMode;

		IoReg_Write32(MDOMAIN_DISP_DDR_Sub_BM_DMA_reg, disp_ddr_sub_dm_dma_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_ddr_main_bm_dma_RBUS disp_ddr_main_dm_dma_reg;

		disp_ddr_main_dm_dma_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_Main_BM_DMA_reg);
		disp_ddr_main_dm_dma_reg.dma_mode = dmaMode;

		IoReg_Write32(MDOMAIN_DISP_DDR_Main_BM_DMA_reg, disp_ddr_main_dm_dma_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_disp_dispm_pq_decmp_reg_decmp_en(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub
		mdomain_disp_dispm_sub_pq_decmp_RBUS mdomain_disp_dispm_sub_pq_decmp_reg;

		mdomain_disp_dispm_sub_pq_decmp_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISPM_SUB_PQ_DECMP_reg);
		mdomain_disp_dispm_sub_pq_decmp_reg.s_decmp_en = enable;

		IoReg_Write32(MDOMAIN_DISP_DISPM_SUB_PQ_DECMP_reg, mdomain_disp_dispm_sub_pq_decmp_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_disp_dispm_main_pq_decmp_RBUS mdomain_disp_dispm_main_pq_decmp_reg;

		mdomain_disp_dispm_main_pq_decmp_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISPM_MAIN_PQ_DECMP_reg);
		mdomain_disp_dispm_main_pq_decmp_reg.decmp_en = enable;

		IoReg_Write32(MDOMAIN_DISP_DISPM_MAIN_PQ_DECMP_reg, mdomain_disp_dispm_main_pq_decmp_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_vi_mdom_doublebuffer_reg_doublebuffer_enable(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
		mdomain_vi_sub_mdom_sub_vi_doublebuffer_RBUS mdomain_vi_sub_mdom_vi_doublebuffer_reg;

		mdomain_vi_sub_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_mdom_sub_vi_doublebuffer_reg);
		mdomain_vi_sub_mdom_vi_doublebuffer_reg.vi_db_en = enable;

		rtd_outl(MDOMAIN_VI_SUB_mdom_sub_vi_doublebuffer_reg, mdomain_vi_sub_mdom_vi_doublebuffer_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_vi_main_mdom_vi_doublebuffer_RBUS mdomain_vi_main_mdom_vi_doublebuffer_reg;

		mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);
		mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_en = enable;

		rtd_outl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg, mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("Not use vi sub double buffer\n");
	}
}

void nonlibdma_set_vi_mdom_doublebuffer_reg_doublebuffer_apply(int mainSub, unsigned int enable)
//main: 0, sub: 1
{
	if(mainSub == 1) { //sub
	} else if(mainSub == 0) { //main
		mdomain_vi_main_mdom_vi_doublebuffer_RBUS mdomain_vi_main_mdom_vi_doublebuffer_reg;

		mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);
		mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_apply = enable;

		rtd_outl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg, mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("Not use vi sub double buffer\n");
	}
}

void nonlibdma_set_vi_dmactl_reg_dmaen1(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub
		mdomain_vi_sub_sub_dmactl_RBUS vi_dmactl_reg;

		vi_dmactl_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_SUB_DMACTL_reg);
		vi_dmactl_reg.dmaen1 = enable;

		rtd_outl(MDOMAIN_VI_SUB_SUB_DMACTL_reg, vi_dmactl_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_vi_main_dmactl_RBUS vi_dmactl_reg;

		vi_dmactl_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_DMACTL_reg);
		vi_dmactl_reg.dmaen1 = enable;

		rtd_outl(MDOMAIN_VI_MAIN_DMACTL_reg, vi_dmactl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_vi_dmactl_reg_seq_blk_sel(int mainSub, unsigned int dmaMode)
{
	if(mainSub == 1) { //sub
		mdomain_vi_sub_sub_dmactl_RBUS vi_dmactl_reg;

		vi_dmactl_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_SUB_DMACTL_reg);
		vi_dmactl_reg.seq_blk_sel = dmaMode;

		rtd_outl(MDOMAIN_VI_SUB_SUB_DMACTL_reg, vi_dmactl_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_vi_main_dmactl_RBUS vi_dmactl_reg;

		vi_dmactl_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_DMACTL_reg);
		vi_dmactl_reg.seq_blk_sel = dmaMode;

		rtd_outl(MDOMAIN_VI_MAIN_DMACTL_reg, vi_dmactl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_vi_gctl_reg_vsce1(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub
		mdomain_vi_sub_sub_gctl_RBUS vi_gctl_reg;

		vi_gctl_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_SUB_GCTL_reg);
		vi_gctl_reg.vsce1 = enable;

		rtd_outl(MDOMAIN_VI_SUB_SUB_GCTL_reg, vi_gctl_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_vi_main_gctl_RBUS vi_gctl_reg;

		vi_gctl_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_GCTL_reg);
		vi_gctl_reg.vsce1 = enable;

		rtd_outl(MDOMAIN_VI_MAIN_GCTL_reg, vi_gctl_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_vi_inten_reg_interrupt(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub
		mdomain_vi_sub_sub_inten_RBUS vi_inten_reg;

		vi_inten_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_SUB_INTEN_reg);
		vi_inten_reg.video_in_last_wr_y_flag_ie = enable;
		vi_inten_reg.video_in_last_wr_c_flag_ie = enable;
		vi_inten_reg.vsinte1 = enable;

		rtd_outl(MDOMAIN_VI_SUB_SUB_INTEN_reg, vi_inten_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_vi_main_inten_RBUS vi_inten_reg;

		vi_inten_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_INTEN_reg);
		vi_inten_reg.video_in_last_wr_y_flag_ie = enable;
		vi_inten_reg.video_in_last_wr_c_flag_ie = enable;
		vi_inten_reg.vsinte1 = enable;

		rtd_outl(MDOMAIN_VI_MAIN_INTEN_reg, vi_inten_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}

void nonlibdma_set_vi_intst_reg_status(int mainSub, unsigned int enable)
{
	if(mainSub == 1) { //sub
		mdomain_vi_sub_sub_intst_RBUS vi_intst_reg;

		vi_intst_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_SUB_INTST_reg);
		vi_intst_reg.video_in_last_wr_y_flag = enable;
		vi_intst_reg.video_in_last_wr_c_flag = enable;
		vi_intst_reg.vsints1 = enable;

		rtd_outl(MDOMAIN_VI_SUB_SUB_INTST_reg, vi_intst_reg.regValue);
	} else if(mainSub == 0) { //main
		mdomain_vi_main_intst_RBUS vi_intst_reg;

		vi_intst_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_INTST_reg);
		vi_intst_reg.video_in_last_wr_y_flag = enable;
		vi_intst_reg.video_in_last_wr_c_flag = enable;
		vi_intst_reg.vsints1 = enable;

		rtd_outl(MDOMAIN_VI_MAIN_INTST_reg, vi_intst_reg.regValue);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
		return;
	}
}



unsigned int nonlibdma_get_cap_ddr_inctrl_reg_bufnum(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) {//main
		mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;

		mdomain_cap_ddr_in1ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Ctrl_reg);

		retVal = mdomain_cap_ddr_in1ctrl_reg.in1_bufnum;
	} else if(mainSub == 1) { //sub
		mdomain_cap_ddr_in2ctrl_RBUS mdomain_cap_ddr_in2ctrl_reg;

		mdomain_cap_ddr_in2ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In2Ctrl_reg);

		retVal = mdomain_cap_ddr_in2ctrl_reg.in2_bufnum;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_cap_doublebuffer_reg_doublebuffer_enable(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;

	cap_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
	if(!mainSub) { //main
		retVal = cap_reg_doublbuffer_reg.cap1_db_en;
	} else if(mainSub == 1) { //sub
		retVal = cap_reg_doublbuffer_reg.cap2_db_en;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_cap_doublebuffer_reg_doublebuffer_apply(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;

	cap_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
	if(!mainSub) { //main
		retVal = cap_reg_doublbuffer_reg.cap1_db_apply;
	} else if(mainSub == 1) { //sub
		retVal = cap_reg_doublbuffer_reg.cap2_db_apply;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_cap_ich12_ibuff_status_reg_ibuff_ovf(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
	if(!mainSub) { //main
		retVal = mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf;
	} else if(mainSub == 1) { //sub
		retVal = mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf_sub;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_cap_ich12_ibuff_status_reg_fsync_ovf(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
	if(!mainSub) { //main
		retVal = mdomain_cap_ich12_ibuff_status_reg.fsync_ovf;
	} else if(mainSub == 1) { //sub
		retVal = mdomain_cap_ich12_ibuff_status_reg.fsync_ovf_sub;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_cap_ich12_ibuff_status_reg_fsync_udf(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
	if(!mainSub) { //main
		retVal = mdomain_cap_ich12_ibuff_status_reg.fsync_udf;
	} else if(mainSub == 1) { //sub
		retVal = mdomain_cap_ich12_ibuff_status_reg.fsync_udf_sub;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_cap_instatus_reg_freeze_enable_2nd(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) {//main
		mdomain_cap_ddr_in1status_RBUS mdomain_cap_ddr_in1status_reg;

		mdomain_cap_ddr_in1status_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In1Status_reg);

		retVal = mdomain_cap_ddr_in1status_reg.in1_freeze_enable_2nd;
	} else if(mainSub == 1) { //sub
		mdomain_cap_ddr_in2status_RBUS mdomain_cap_ddr_in2status_reg;

		mdomain_cap_ddr_in2status_reg.regValue = rtd_inl(MDOMAIN_CAP_DDR_In2Status_reg);

		retVal = mdomain_cap_ddr_in2status_reg.in2_freeze_enable_2nd;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_disp_ddr_prevstart_reg_pre_rd_v_start(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) {//main
		mdomain_disp_ddr_mainprevstart_RBUS mdomain_disp_ddr_mainprevstart_reg;

		mdomain_disp_ddr_mainprevstart_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainPreVStart_reg);

		retVal = mdomain_disp_ddr_mainprevstart_reg.main_pre_rd_v_start;
	} else if(mainSub == 1) { //sub
		mdomain_disp_ddr_subprevstart_RBUS mdomain_disp_ddr_subprevstart_reg;

		mdomain_disp_ddr_subprevstart_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubPreVStart_reg);

		retVal = mdomain_disp_ddr_subprevstart_reg.sub_pre_rd_v_start;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_disp_status_reg_block_select(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) {//main
		mdomain_disp_ddr_mainstatus_RBUS mdomain_disp_ddr_mainstatus_reg;

		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);

		retVal =  mdomain_disp_ddr_mainstatus_reg.main_block_select;
	} else if(mainSub == 1) { //sub
		mdomain_disp_ddr_substatus_RBUS mdomain_disp_ddr_substatus_reg;

		mdomain_disp_ddr_substatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubStatus_reg);

		retVal =  mdomain_disp_ddr_substatus_reg.sub_block_select;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_enable(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_disp_ddr_mainsubctrl_RBUS disp_ddr_mainsubctrl_reg;

	disp_ddr_mainsubctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
	if(!mainSub) { //main
		retVal = disp_ddr_mainsubctrl_reg.disp1_double_enable;
	} else if(mainSub == 1) { //sub
		retVal = disp_ddr_mainsubctrl_reg.disp2_double_enable;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_apply(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_disp_ddr_mainsubctrl_RBUS disp_ddr_mainsubctrl_reg;

	disp_ddr_mainsubctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
	if(!mainSub) { //main
		retVal = disp_ddr_mainsubctrl_reg.disp1_double_apply;
	} else if(mainSub == 1) { //sub
		retVal = disp_ddr_mainsubctrl_reg.disp2_double_apply;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_disp_ddr_ctrl_reg_frc_style(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) { //main
		mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;

		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);

		retVal = ddr_mainctrl_reg.main_frc_style;
	} else if(mainSub == 1) { //sub
		mdomain_disp_ddr_subctrl_RBUS ddr_subctrl_reg;

		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);

		retVal = ddr_subctrl_reg.sub_frc_style;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}
unsigned int nonlibdma_get_disp_ddr_ctrl_reg_source_sel(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) { //main
		mdomain_disp_ddr_mainctrl_RBUS mdomain_disp_ddr_mainctrl_reg;
		mdomain_disp_ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		retVal = mdomain_disp_ddr_mainctrl_reg.main_source_sel;
	} else if(mainSub == 1) { //sub
		mdomain_disp_ddr_subctrl_RBUS mdomain_disp_ddr_subctrl_reg;
		mdomain_disp_ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		retVal = mdomain_disp_ddr_subctrl_reg.sub_source_sel;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_vi_mdom_doublebuffer_reg_doublebuffer_enable(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) { //main
		mdomain_vi_main_mdom_vi_doublebuffer_RBUS mdomain_vi_main_mdom_vi_doublebuffer_reg;

		mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);

		retVal = mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_en;
	} else if(mainSub == 1) { //sub
		mdomain_vi_sub_mdom_sub_vi_doublebuffer_RBUS mdomain_vi_sub_mdom_vi_doublebuffer_reg;

		mdomain_vi_sub_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_mdom_sub_vi_doublebuffer_reg);

		retVal = mdomain_vi_sub_mdom_vi_doublebuffer_reg.vi_db_en;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_vi_mdom_doublebuffer_reg_doublebuffer_apply(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) { //main
		mdomain_vi_main_mdom_vi_doublebuffer_RBUS mdomain_vi_main_mdom_vi_doublebuffer_reg;

		mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);

		retVal = mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_apply;
	} else if(mainSub == 1) { //sub
		mdomain_vi_sub_mdom_sub_vi_doublebuffer_RBUS mdomain_vi_sub_mdom_vi_doublebuffer_reg;

		mdomain_vi_sub_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_SUB_mdom_sub_vi_doublebuffer_reg);

		retVal = mdomain_vi_sub_mdom_vi_doublebuffer_reg.vi_db_apply;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_vi_dmactl_reg_dmaen1(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) { //main
		mdomain_vi_main_dmactl_RBUS mdomain_vi_main_dmactl_reg;

		mdomain_vi_main_dmactl_reg.regValue = IoReg_Read32(MDOMAIN_VI_MAIN_DMACTL_reg);

		retVal = mdomain_vi_main_dmactl_reg.dmaen1;
	} else if(mainSub == 1) { //sub
		mdomain_vi_sub_sub_dmactl_RBUS mdomain_vi_sub_dmactl_reg;

		mdomain_vi_sub_dmactl_reg.regValue = IoReg_Read32(MDOMAIN_VI_SUB_SUB_DMACTL_reg);

		retVal = mdomain_vi_sub_dmactl_reg.dmaen1;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}

unsigned int nonlibdma_get_vi_dmactl_reg_seq_blk_sel(int mainSub)
{
	unsigned int retVal = 0;

	if(!mainSub) { //main
		mdomain_vi_main_dmactl_RBUS mdomain_vi_main_dmactl_reg;

		mdomain_vi_main_dmactl_reg.regValue = IoReg_Read32(MDOMAIN_VI_MAIN_DMACTL_reg);

		retVal = mdomain_vi_main_dmactl_reg.seq_blk_sel;
	} else if(mainSub == 1) { //sub
		mdomain_vi_sub_sub_dmactl_RBUS mdomain_vi_sub_dmactl_reg;

		mdomain_vi_sub_dmactl_reg.regValue = IoReg_Read32(MDOMAIN_VI_SUB_SUB_DMACTL_reg);

		retVal = mdomain_vi_sub_dmactl_reg.seq_blk_sel;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}



void nodlibamd_checkBit_cap_instatus_freeze_ok(int mainSub, unsigned int count)
//main: 0, sub: 1
{
	if(!mainSub) { //main in1
		IoReg_BusyCheckRegBit(MDOMAIN_CAP_DDR_In1Status_reg, _BIT5, count);
	} else if(mainSub == 1) { //sub in2
		IoReg_BusyCheckRegBit(MDOMAIN_CAP_DDR_In2Status_reg, _BIT5, count);
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}
}




/*
unsigned int nonlibdma_get_cap_pqc_en(int mainSub)
{
	unsigned int retVal = 0;
	mdomain_cap_smooth_tog_ctrl_0_RBUS mdomain_cap_smooth_tog_ctrl_0_reg;
	mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
	if(mainSub) { //main
		retVal = mdomain_cap_smooth_tog_ctrl_0_reg.in1_pqc_clken;
	} else if(mainSub == 1) { //sub
		retVal = mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken;
	} else {
		rtd_pr_scaler_memory_err("[%s-%d] argument error - mainSub\n", __func__, __LINE__);
	}

	return retVal;
}
*/

/*======================== End of File =======================================*/
/**


*
* @}
*/
