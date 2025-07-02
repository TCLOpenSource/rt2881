#include <sysdefs.h>
#include <scaler/scalerCommon.h>
#include <rbus/ppoverlay_reg.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <printf.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <scaler_vbedev.h>
#include <scaler_vscdev.h>
#include <rbus/vodma_reg.h>
#include <rbus/vodma2_reg.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/i3ddma/i3ddma_drv.h>
#include <rbus/timer_reg.h>

#include <tvscalercontrol/scalerdrv/scaler_nonlibdma.h>
#include <mach/rtk_platform.h>

extern unsigned int drvif_framesync_get_vo_clk(void);
extern unsigned int VODMA_get_datafs_increased_clock(unsigned int orig_ratio_n_off);
extern unsigned char drvif_framesync_gatting_do(void);


void drvif_scaler_set_display_mode_QS(UINT8 timing_mode)
{
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	ppoverlay_display_timing_ctrl2_RBUS display_timing_ctrl2_reg;

	if(DISPLAY_MODE_FRAME_SYNC == timing_mode)
	{
		ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
		display_timing_ctrl2_reg.regValue = rtd_inl(PPOVERLAY_Display_Timing_CTRL2_reg);
		display_timing_ctrl2_reg.frc2fsync_en = 0;
		rtd_outl(PPOVERLAY_Display_Timing_CTRL2_reg, display_timing_ctrl2_reg.regValue);

		display_timing_ctrl1_reg.regValue = rtd_inl(PPOVERLAY_Display_Timing_CTRL1_reg);
		display_timing_ctrl1_reg.disp_fsync_en = _ENABLE;
		rtd_outl(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);
		double_buffer_ctrl_reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
		if(double_buffer_ctrl_reg.dreg_dbuf_en == 0x01)
		{
			double_buffer_ctrl_reg.dreg_dbuf_set = 0x01;
			rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);
		}

		printf2("[%s %d]\n", __FUNCTION__, __LINE__);
	}
}

void drvif_framesync_gatting_QS(void)
{
	unsigned int clkgen_reg = 0;//, free_period = 0;
	static unsigned int ori_divider_num = 0;
	unsigned int ratio_num = 0;
	unsigned int voAddr = 0;
	//mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	//unsigned int source_clk = 0;
	unsigned int vo_clk = 0;
	SLR_VOINFO* pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
	unsigned int vodmapll = drvif_framesync_get_vo_clk();
	unsigned int tempratio_num=0;
	unsigned int ori_voclk_ratio_num=0;

	if(Scaler_Get_CurVoInfo_VoVideoPlane(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) == VO_VIDEO_PLANE_V2)
		voAddr = VODMA2_VODMA2_CLKGEN_reg;
	else
		voAddr = VODMA_VODMA_CLKGEN_reg;
	
	//Open VODMA gating
	clkgen_reg = IoReg_Read32(voAddr);
	ori_divider_num = (clkgen_reg&0x0f800000)>>23;
	if(Get_DisplayMode_Src(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) == VSC_INPUTSRC_VDEC) {
		tempratio_num=pVOInfo->pixel_clk/(vodmapll/64);
		if (tempratio_num>=64) {
			ori_voclk_ratio_num=0;
		} else {
			ori_voclk_ratio_num=64-tempratio_num;
		}
	} else {
		ori_voclk_ratio_num = (clkgen_reg&0x003f0000)>>16;
	}

	ratio_num = VODMA_get_datafs_increased_clock(ori_voclk_ratio_num);

	clkgen_reg = (clkgen_reg &(~0x0fBf0000))|(ori_divider_num<<23)|(ratio_num<<16);

	IoReg_Write32(voAddr, clkgen_reg); //increase clk
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
		//mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf = TRUE;
		//mdomain_cap_ich12_ibuff_status_reg.fsync_ovf = TRUE;
		//mdomain_cap_ich12_ibuff_status_reg.fsync_udf = TRUE;
		//IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg, mdomain_cap_ich12_ibuff_status_reg.regValue);
		nonlibdma_set_cap_ich12_status_reg_buffer_overflow_underflow(0);
	}

	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		IoReg_ClearBits(PPOVERLAY_Display_Timing_CTRL1_reg, _BIT26);
	}

	clkgen_reg = IoReg_Read32(voAddr);

	if(ori_voclk_ratio_num != 0){
		clkgen_reg = (clkgen_reg &(~0x0fBf0001))|(ori_divider_num<<23)|(ori_voclk_ratio_num<<16);
	}

	clkgen_reg = clkgen_reg&0xfffffffe;
	IoReg_Write32(voAddr, clkgen_reg); //disable gatting clk
	ori_voclk_ratio_num = 0;
	printf("################ drvif_framesync_gatting_QS (plane %d) => %x\n",  Scaler_Get_CurVoInfo_plane(), IoReg_Read32(voAddr));
}
unsigned char drvif_framesync_gatting_do_QS(void)
{
	ppoverlay_display_timing_ctrl2_RBUS display_timing_ctrl2_reg;

	unsigned int before_time = 0;
	unsigned int after_time = 0;
	unsigned int checkResult = 0;
	unsigned char result=0;

	//frank@0327 add below code to solve Data framesync DVS not sync IVS
	display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);
	display_timing_ctrl2_reg.frc2fsync_en = 1;
	//display_timing_ctrl2_reg.shpnr_line_mode = 2;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL2_reg, display_timing_ctrl2_reg.regValue);

	//merlin8/8p mdomain one-bin
	if(get_mach_type()==MACH_ARCH_RTK2885P) {
		nonlibdma_framesyncGattingDoQS_mainSubCtrl();
	}

	//merlin8/8p mdomain one-bin
	if(get_mach_type()==MACH_ARCH_RTK2885P) {
		nonlibdma_framesyncGattingDoQS_mainSubSdnrCxxto64();
	}

	drvif_scalerdisplay_set_dbuffer_en(SLR_MAIN_DISPLAY, FALSE);
	if((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) && (Get_DISPLAY_PANEL_CUSTOM_INDEX() == VBY_ONE_PANEL_COMMERCIAL_4K2K_600M_1S_8L_DUPLICATE)){
		return 0;
	}
	else{
		drvif_scaler_set_display_mode_QS(DISPLAY_MODE_FRAME_SYNC);
	}
	//frank@1121 mark do by later code fw_scaler_enable_fs_wdg(info->display, FALSE);
	return 0;
}

void scaler_quickshow_drvif_scaler_ddomain_switch_irq(unsigned char enable)	// add by hsliao 20081015
{
	ppoverlay_dtg_ie_RBUS	dtg_ie_reg;
	dtg_ie_reg.regValue = rtd_inl(PPOVERLAY_DTG_ie_reg);
	dtg_ie_reg.mv_den_sta_event_ie = enable;
	rtd_outl(PPOVERLAY_DTG_ie_reg, dtg_ie_reg.regValue);
}
void scaler_quickshow_drvif_scalerdisplay_set_forcebg_dbuffer(int En)//only for forcebg
{//disable main forcebg double buffer, becasue the double buffer is changed

	if(En) {
		rtd_setbits(PPOVERLAY_Double_Buffer_CTRL3_reg, _BIT2|_BIT0);
	} else {
		rtd_maskl(PPOVERLAY_Double_Buffer_CTRL3_reg, ~_BIT2, 0);
	}
}
void scaler_quickshow_drvif_scalerdisplay_enable_display(unsigned char bEnable)
{
	ppoverlay_main_display_control_rsv_RBUS reg_display_control_rsv;
	reg_display_control_rsv.regValue = rtd_inl(PPOVERLAY_Main_Display_Control_RSV_reg);
	reg_display_control_rsv.m_disp_en = bEnable;
	rtd_outl(PPOVERLAY_Main_Display_Control_RSV_reg, reg_display_control_rsv.regValue);
	scaler_quickshow_drvif_scalerdisplay_set_forcebg_dbuffer(FALSE);
}

void Scaler_disp_setting_QS(unsigned char display)
{
    extern void clear_online_status(unsigned char display);
    extern void Scaler_MEMC_CLK_Enable(void);
    extern int Scaler_MEMC_MEMC_CLK_OnOff(unsigned char ME_enable, unsigned char MC_enable, unsigned char bMute);
    extern void Scaler_MEMC_Bypass_On(unsigned char b_isBypass);
    extern void framesync_set_iv2dv_delay(unsigned int dispTimingMode);

    Scaler_MEMC_CLK_Enable();

    set_dtg_ivs_source();

    if(multiview_1p_input_in_2p_panel())
    {
        Scaler_MEMC_MEMC_CLK_OnOff(0, 1, 1);
        Scaler_MEMC_Bypass_On(0);
        printf("[%s][multiview] 1p input MC only mode\n", __FUNCTION__);
    }
    else if( get_panel_pixel_mode > PANEL_1_PIXEL )
    {
        if( jude_input_timing_is_hfr(display) || ( get_vsc_src_is_hdmi_or_dp() && is_scaler_input_2p_mode(display) ) )
        {
            Scaler_MEMC_Bypass_On(1);
            Scaler_MEMC_MEMC_CLK_OnOff(0, 0, 1);
            printf("[Scaler_disp_setting_QS] HFR bypass MEMC\n");
        }
        else if(vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag())
        {
            Scaler_MEMC_MEMC_CLK_OnOff(0, 0, 1);
            Scaler_MEMC_Bypass_On(1);
            printf("[vrr or freesync bypass memc][%d]\n", ((rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>13)&0x1));
        }
        else
        {
            Scaler_MEMC_MEMC_CLK_OnOff(0, 1, 1);
            Scaler_MEMC_Bypass_On(0);
            printf("[memc only mode][DD][%d]\n", ((rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>13)&0x1));
        }
    }
    else
    {
        if(vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag())
        {
            Scaler_MEMC_MEMC_CLK_OnOff(0, 0, 1);
            Scaler_MEMC_Bypass_On(1);
            printf("[vrr or freesync bypass memc][%d]\n", ((rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>13)&0x1));
        }
        else
        {
            Scaler_MEMC_MEMC_CLK_OnOff(0, 1, 1);
            Scaler_MEMC_Bypass_On(0);
            printf("[memc only mode][DD][%d]\n", ((rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>13)&0x1));
        }
    }

	if(Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC)){
		//gatting new flow : raise vo clock first -> set timing sync -> enable gatting after timing stable
		drvif_framesync_gatting_QS();
		drvif_framesync_gatting_do_QS();
	}
	if((get_vsc_src_is_hdmi_or_dp()) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag()))
	{
		printf("[%s %d]qs vrr flow\n", __FUNCTION__, __LINE__);
		vbe_disp_set_VRR_fractional_framerate_mode();
		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC))
		{//if game mode data framesync
			UINT32 id2dv_delay = 0;
			ppoverlay_fs_iv_dv_fine_tuning5_RBUS fs_iv_dv_fine_tuning5_reg;
			id2dv_delay = 2;
			fs_iv_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
			fs_iv_dv_fine_tuning5_reg.iv2dv_line = id2dv_delay;
			fs_iv_dv_fine_tuning5_reg.iv2dv_line_2 = id2dv_delay;
			IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, fs_iv_dv_fine_tuning5_reg.regValue);
		}
		else{
			framesync_set_iv2dv_delay(DISPLAY_MODE_FRAME_SYNC);
		}
		drvif_scaler_set_display_mode_QS(DISPLAY_MODE_FRAME_SYNC);
		vbe_disp_set_VRR_set_display_vtotal_60Hz();
		clear_online_status(SLR_MAIN_DISPLAY);
	}

	if(Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC)){
		IoReg_SetBits(VODMA_VODMA_CLKGEN_reg,VODMA_VODMA_CLKGEN_en_fifo_full_gate_mask);
		IoReg_ClearBits(VODMA_VODMA_REG_DB_CTRL_reg,VODMA_VODMA_REG_DB_CTRL_vodmavsg_db_en_mask);
		IoReg_SetBits(VODMA_VODMA_REG_DB_CTRL_reg,VODMA_VODMA_REG_DB_CTRL_vodmavsg_db_en_mask);
		drv_set_datafs_gatting_enable_done(TRUE);
	}
	scaler_quickshow_drvif_scaler_ddomain_switch_irq(_ENABLE);

	scaler_quickshow_drvif_scalerdisplay_enable_display( _ENABLE);
}

