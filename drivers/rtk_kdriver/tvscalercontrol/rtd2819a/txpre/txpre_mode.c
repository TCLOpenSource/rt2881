/*============================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* ===========================================================================*/

//----------------------------------------------------------------------------
// ID Code      : txpre_mode.c No.0000
// Update Note  :
//----------------------------------------------------------------------------


#ifdef CONFIG_ENABLE_HDMIRX
//#include <scaler/scalerDrvCommon.h>
#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
//#include <tvscalercontrol/hdmirx/hdmi_emp.h>
#endif

#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/scalerdrv/mode.h>
#include <tvscalercontrol/io/ioregdrv.h>

#include <rbus/onms5_reg.h>
#include <rbus/ppoverlay_txpretg_reg.h>
#include <rbus/hdmitx_vgip_reg.h>

#include <rtd_log/rtd_module_log.h>

//*****************************************************************************
// type define
//
//*****************************************************************************


//****************************************************************************
// CODE TABLES
//****************************************************************************


/****************************************
// local variable declaration
****************************************/


/****************************************
// extern variable declaration
****************************************/
extern unsigned int current_hdmi_onms_watchdog_setting;

extern unsigned int get_hdmi_online_wd_int_setting(void);
extern unsigned int get_result_with_hdmi_mask(unsigned int original_value);
extern unsigned char Get_tv006_wb_pattern(void);
extern bool is_vrr_or_freesync_mode(void);


//****************************************************************************
// Implementation function define
//****************************************************************************

void txpre_onms5_disable(void)
{
    onms5_onmstx_ctrl_RBUS onms5_onmstx_ctrl_reg;
    onms5_onmstx_ctrl_4_RBUS onms5_onmstx_ctrl_4_reg;

    rtd_pr_hdmitx_info("#####Disable onms5 onlinemeasure######\r\n");
    onms5_onmstx_ctrl_reg.regValue =  IoReg_Read32(ONMS5_onmstx_ctrl_reg);
    onms5_onmstx_ctrl_reg.onmstx_start_ms = 0;
    onms5_onmstx_ctrl_reg.onmstx_ms_conti = 0;
    onms5_onmstx_ctrl_reg.onmstx_continuous_popup = 0;
    IoReg_Write32(ONMS5_onmstx_ctrl_reg, onms5_onmstx_ctrl_reg.regValue);

    /* hs_per_timeout */
    onms5_onmstx_ctrl_4_reg.regValue = IoReg_Read32(ONMS5_onmstx_ctrl_4_reg);
    onms5_onmstx_ctrl_4_reg.onmstx_hs_per_timeout_en = 0;
    IoReg_Write32(ONMS5_onmstx_ctrl_4_reg, onms5_onmstx_ctrl_4_reg.regValue);
}

void txpre_onms5_enable(StructTxPreInfo *txpre_info)
{
    onms5_onmstx_ctrl_RBUS onms5_onmstx_ctrl_reg;
    onms5_onmstx_ctrl_2_RBUS onms5_onmstx_ctrl_2_reg;
    onms5_onmstx_ctrl_3_RBUS onms5_onmstx_ctrl_3_reg;
    onms5_onmstx_ctrl_4_RBUS onms5_onmstx_ctrl_4_reg;
    onms5_onmstx_hsync_meas_ctrl_RBUS onms5_onmstx_hsync_meas_ctrl_reg;
    onms5_onmstx_hsync_meas_ctrl_2_RBUS onms5_onmstx_hsync_meas_ctrl_2_reg;
    onms5_onmstx_hsync_meas_result_RBUS onms5_onmstx_hsync_meas_result_reg;
    onms5_onmstx_demode_ctrl_RBUS onms5_onmstx_demode_ctrl_reg;
    onms5_onmstx_demode_vs_uf_RBUS onms5_onmstx_demode_vs_uf_reg;
    onms5_onmstx_demode_vs_of_RBUS onms5_onmstx_demode_vs_of_reg;
    onms5_onmstx_status_RBUS onms5_onmstx_status_reg;
    unsigned short Vtotal, Htotal, Vstart, Vactive;//input h v total & v start & v active
    unsigned int Vfreq;//input v freq

    Vtotal  = txpre_info->IVTotal;
    Htotal  = txpre_info->IHTotal;
    Vstart  = txpre_info->IPV_ACT_STA;
    Vactive = txpre_info->IPV_ACT_LEN;
    Vfreq   = txpre_info->IVFreq;

    onms5_onmstx_demode_ctrl_reg.regValue = IoReg_Read32(ONMS5_Onmstx_Demode_ctrl_reg);
    onms5_onmstx_demode_ctrl_reg.onmstx_demode_vtotal_delta = 0;
    IoReg_Write32(ONMS5_Onmstx_Demode_ctrl_reg, onms5_onmstx_demode_ctrl_reg.regValue);

	onms5_onmstx_hsync_meas_ctrl_reg.regValue = 0;
	IoReg_Write32(ONMS5_onmstx_hsync_meas_ctrl_reg, onms5_onmstx_hsync_meas_ctrl_reg.regValue);

	onms5_onmstx_hsync_meas_ctrl_2_reg.regValue = 0;
	IoReg_Write32(ONMS5_onmstx_hsync_meas_ctrl_2_reg, onms5_onmstx_hsync_meas_ctrl_2_reg.regValue);

	onms5_onmstx_ctrl_2_reg.regValue = IoReg_Read32(ONMS5_onmstx_ctrl_2_reg);
	if(Vtotal && Vfreq) {//3 line
		onms5_onmstx_ctrl_2_reg.onmstx_hgclk_gate_th = (unsigned int)( 81000000000UL / (unsigned long)Vtotal / (unsigned long)Vfreq );
	} else {
		onms5_onmstx_ctrl_2_reg.onmstx_hgclk_gate_th = 12900;//24hz 480i
	}
	IoReg_Write32(ONMS5_onmstx_ctrl_2_reg, onms5_onmstx_ctrl_2_reg.regValue);

	onms5_onmstx_ctrl_reg.regValue = 0;
	onms5_onmstx_ctrl_reg.onmstx_blk_2frame_en = 1;//mask the 3 frame first.
	if(Htotal <= 4096)
		onms5_onmstx_ctrl_reg.onmstx_htotal_of_th = 0;
	else if(Htotal <= 8192)
		onms5_onmstx_ctrl_reg.onmstx_htotal_of_th = 1;
	else if(Htotal <= 16384)
		onms5_onmstx_ctrl_reg.onmstx_htotal_of_th = 2;
	else
		onms5_onmstx_ctrl_reg.onmstx_htotal_of_th = 3;
	onms5_onmstx_ctrl_reg.onmstx_meas_to_sel= _ON_MEAS_TO_SEL_76MS;
	IoReg_Write32(ONMS5_onmstx_ctrl_reg, onms5_onmstx_ctrl_reg.regValue);

    onms5_onmstx_demode_vs_uf_reg.regValue = IoReg_Read32(ONMS5_Onmstx_Demode_Vs_UF_reg);
    onms5_onmstx_demode_vs_of_reg.regValue = IoReg_Read32(ONMS5_Onmstx_Demode_Vs_OF_reg);
#ifdef HDMI_VRR_ONLINEMEASURE_SUPPORT
    if(is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag())//vrr case input is hdmi 2.1 need HDMI provide the api
    {//qms case similar with vrr
        rtd_pr_hdmitx_info("[Eric][%s][%d]hdmi_framerate=%d\n", __FUNCTION__, __LINE__, Vfreq);

        if(Vfreq)
            onms5_onmstx_demode_vs_uf_reg.onmstx_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 90 / Vfreq * 10;//120 hz max 24300000 = 27000000*0.9 //HD SD suggest
        else
            onms5_onmstx_demode_vs_uf_reg.onmstx_de_vsper_uf_th = 0;

        onms5_onmstx_demode_vs_of_reg.onmstx_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/240;//24 hz minimax //29700000 = 27000000 * 1.1 //HD SD suggest
    }
    else
#endif
    {
        //V counter overflow judgment for ONMS     //H counter overflow judgment for ONMS
        //00: 2048                                 //00: 4096
        //01: 4096                                 //01: 8192
        //10: 8192                                 //10: 16384
        //11: 16383                                //11: 32768
        if(Vfreq) {
            onms5_onmstx_demode_vs_uf_reg.onmstx_de_vsper_uf_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 98 / Vfreq * 10;//for panel compability
            onms5_onmstx_demode_vs_of_reg.onmstx_de_vsper_of_th = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10;//29700000 = 27000000 * 1.1 //HD SD suggest
        } else {
            onms5_onmstx_demode_vs_uf_reg.onmstx_de_vsper_uf_th = 0;
            onms5_onmstx_demode_vs_of_reg.onmstx_de_vsper_of_th = ((unsigned int)CONFIG_ONLINE_XTAL_FREQ*(unsigned int)11)/600;
        }
    }
    IoReg_Write32(ONMS5_Onmstx_Demode_Vs_UF_reg, onms5_onmstx_demode_vs_uf_reg.regValue);
    IoReg_Write32(ONMS5_Onmstx_Demode_Vs_OF_reg, onms5_onmstx_demode_vs_of_reg.regValue);

    onms5_onmstx_ctrl_2_reg.regValue = IoReg_Read32(ONMS5_onmstx_ctrl_2_reg);
    onms5_onmstx_ctrl_3_reg.regValue = IoReg_Read32(ONMS5_onmstx_ctrl_3_reg);
    onms5_onmstx_ctrl_4_reg.regValue = IoReg_Read32(ONMS5_onmstx_ctrl_4_reg);
    /* vs_timeout */
    if (Vfreq != 0) {
        unsigned int framerate = (is_vrr_or_freesync_mode() || get_scaler_qms_mode_flag()) ? VRR_MIN_FRAMERATE : Vfreq;
        unsigned int threshold = (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / framerate * 10 / 2048;

        onms5_onmstx_ctrl_2_reg.onmstx_vs_timeout_th    = threshold;
        onms5_onmstx_ctrl_2_reg.onmstx_vs_timeout_th_en = (threshold > 0) ? 1 : 0;
    }

    /* vs_bporch_timeout */
    if (Vfreq != 0) {
        onms5_onmstx_ctrl_3_reg.onmstx_vs_bporch_timeout_th = (Vtotal != 0) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * 110 / Vfreq * 10 * Vstart / Vtotal ) : 0;
    }

    /* hs_per_timeout */
    if (Vfreq != 0) {
        unsigned int threshold = ( Vtotal > 0 ) ? ( (unsigned int)CONFIG_ONLINE_XTAL_FREQ * CONFIG_HS_PER_TIMEOUT_TH_TOLERANCE / Vfreq * 100 / Vtotal ) : 0;

        onms5_onmstx_ctrl_4_reg.onmstx_vact_num = Vactive;
        onms5_onmstx_ctrl_4_reg.onmstx_hs_per_timeout_th = threshold;
        onms5_onmstx_ctrl_4_reg.onmstx_hs_per_timeout_en = (Vactive > 0 && threshold > 0) ? 1 : 0;
    }

    IoReg_Write32(ONMS5_onmstx_ctrl_2_reg, onms5_onmstx_ctrl_2_reg.regValue);
    IoReg_Write32(ONMS5_onmstx_ctrl_3_reg, onms5_onmstx_ctrl_3_reg.regValue);
    IoReg_Write32(ONMS5_onmstx_ctrl_4_reg, onms5_onmstx_ctrl_4_reg.regValue);


    // enable online measure
    onms5_onmstx_ctrl_reg.onmstx_sycms_clk = 1;
    onms5_onmstx_ctrl_reg.onmstx_htotal_max_delta = 8;
	onms5_onmstx_ctrl_reg.onmstx_ms_conti = 1;
	onms5_onmstx_ctrl_reg.onmstx_start_ms = 0;

	onms5_onmstx_ctrl_reg.onmstx_continuous_popup = 1;//enable continue pop for demode
	onms5_onmstx_ctrl_reg.onmstx_hd21_ms_en = 1;//hdmi2.1

	IoReg_Write32(ONMS5_onmstx_ctrl_reg, onms5_onmstx_ctrl_reg.regValue);
	onms5_onmstx_ctrl_reg.onmstx_start_ms = 1;
	IoReg_Write32(ONMS5_onmstx_ctrl_reg, onms5_onmstx_ctrl_reg.regValue);

	onms5_onmstx_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS5_onmstx_hsync_meas_result_reg);
	onms5_onmstx_hsync_meas_result_reg.onmstx_hmax_delta_total = 0;	//write clear
	IoReg_Write32(ONMS5_onmstx_hsync_meas_result_reg, onms5_onmstx_hsync_meas_result_reg.regValue);

	//write clear status, if can not clear, need add delay
	onms5_onmstx_status_reg.regValue = IoReg_Read32(ONMS5_onmstx_status_reg);
	onms5_onmstx_status_reg.onmstx_vs_per_to_long = 1;	//Time-Out (76ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit20
	onms5_onmstx_status_reg.onmstx_vs_pol_out = 1; // VSYNC Polarity Indicator
	onms5_onmstx_status_reg.onmstx_vs_per_over_range = 1; //VSYNC_OVER_RANGE//Bit15
	onms5_onmstx_status_reg.onmstx_hs_per_over_range = 1; //HSYNC_OVER_RANGE//Bit14
	onms5_onmstx_status_reg.onmstx_vs_per_timeout = 1; //Time-Out (40ms) bit of Input VSYNC Period Measurement (No VSYNC occurred)//Bit13
	onms5_onmstx_status_reg.onmstx_vs_high_timeout = 1;	//Time-Out bit of Input VSYNC High Pulse Measurement//Bit12
	onms5_onmstx_status_reg.onmstx_vs_per_overflow = 1;	//Over-flow bit of Input HSYNC Period Measurement//Bit11
	onms5_onmstx_status_reg.onmstx_hs_per_overflow = 1; //Over-flow bit of Input VSYNC Period Measurement//Bit10
	onms5_onmstx_status_reg.onmstx_vs_pol_chg= 1; //Input VSYNC Polarity change//Bit9
	onms5_onmstx_status_reg.onmstx_vtotalde_chg = 1; // Total active line number changes
	onms5_onmstx_status_reg.onmstx_hact_chg = 1; // Total active pixel number of first 8 active lines changes
	onms5_onmstx_status_reg.onmstx_htotal_chg = 1; //Total average pixel number of first 8 lines changes
	onms5_onmstx_status_reg.onmstx_hs_active_pixel_var = 1;//Bit3
	onms5_onmstx_status_reg.onmstx_vs_per_underflow = 1;	//Under-flow bit of Input HSYNC Period Measurement//Bit2
	onms5_onmstx_status_reg.onmstx_hs_meas_over_range = 1;//Bit21 Hs period measure over range
	onms5_onmstx_status_reg.onmstx_hgclk_gate_over_range = 1;//BIT22 hgclk gate over range
	IoReg_Write32(ONMS5_onmstx_status_reg, onms5_onmstx_status_reg.regValue);
}

void txpre_onms5_setting(StructTxPreInfo *txpre_info, unsigned char wdgenable, unsigned char interruptenable)
{//This function is to set onlinemeasure watchdog and interrupt setting
    onms5_onmstx_ctrl_RBUS onms5_onmstx_ctrl_reg;
    onms5_onmstx_status_RBUS onms5_onmstx_status_reg;
    onms5_onmstx_hsync_meas_ctrl_RBUS onms5_onmstx_hsync_meas_ctrl_reg;
    onms5_onmstx_hsync_meas_result_RBUS onms5_onmstx_hsync_meas_result_reg;
    // onms5_onmstx_demode_hs_period_RBUS onms5_onmstx_demode_hs_period_reg;
    onms5_onmstx_watchdog_en_RBUS onms5_onmstx_watchdog_en_reg;
    onms5_onmstx_interrupt_en_RBUS onms5_onmstx_interrupt_en_reg;
    hdmitx_vgip_hdmitx_vgip_int_ctl_RBUS hdmitx_vgip_hdmitx_vgip_int_ctl_reg;
    ppoverlay_txpretg_txpretg_watchdog_ctrl_RBUS txpretg_watchdog_ctrl_reg;

#ifdef POLLING_ONLINE_STATUS_ENABLE//no need to enable interrupt when enable polling flow
    wdgenable = FALSE;
    interruptenable = FALSE;
#endif

    rtd_pr_hdmitx_info("###[%s(%d)] wdg:%d int:%d###\r\n",__FUNCTION__, __LINE__, wdgenable, interruptenable);

    if(_ENABLE == wdgenable)
    {
        onms5_onmstx_ctrl_reg.regValue = IoReg_Read32(ONMS5_onmstx_ctrl_reg);
        if(onms5_onmstx_ctrl_reg.onmstx_start_ms) {
            onms5_onmstx_hsync_meas_ctrl_reg.regValue = IoReg_Read32(ONMS5_onmstx_hsync_meas_ctrl_reg);
            // onms5_onmstx_demode_hs_period_reg.regValue = IoReg_Read32(ONMS5_Onmstx_Demode_Hs_period_reg);
            // B804A05C[13:0] = hotal
            // onms5_onmstx_hsync_meas_ctrl_reg.onmstx_target_measure = onms5_onmstx_demode_hs_period_reg.onmstx_de_hs_period;
            onms5_onmstx_hsync_meas_ctrl_reg.onmstx_target_measure = txpre_info->IHTotal;
            //B804A05C[19:16] = 2 or 3 (line average)
            onms5_onmstx_hsync_meas_ctrl_reg.onmstx_average_line = 3;
            //B804A05C[30:24] = 8 (line diff)
            onms5_onmstx_hsync_meas_ctrl_reg.onmstx_htotal_max_delta_new = 8;
            IoReg_Write32(ONMS5_onmstx_hsync_meas_ctrl_reg, onms5_onmstx_hsync_meas_ctrl_reg.regValue);

            onms5_onmstx_hsync_meas_result_reg.regValue = IoReg_Read32(ONMS5_onmstx_hsync_meas_result_reg);
            onms5_onmstx_hsync_meas_result_reg.onmstx_hmax_delta_total = 0;	//write clear
            IoReg_Write32(ONMS5_onmstx_hsync_meas_result_reg, onms5_onmstx_hsync_meas_result_reg.regValue);

            onms5_onmstx_status_reg.regValue = 0;
            onms5_onmstx_status_reg.onmstx_hs_meas_over_range = 1;//write clean bit 21
            IoReg_Write32(ONMS5_onmstx_status_reg, onms5_onmstx_status_reg.regValue);
        }

        onms5_onmstx_watchdog_en_reg.regValue = get_hdmi_online_wd_int_setting();
        {// Only for HDMI source, check particular device or debugging command
            unsigned int original_value = onms5_onmstx_watchdog_en_reg.regValue;
            onms5_onmstx_watchdog_en_reg.regValue = get_result_with_hdmi_mask(original_value);
            if(onms5_onmstx_watchdog_en_reg.regValue != original_value)
            {
                rtd_pr_hdmitx_info("[%s][HDMI] Update onms5 wd from %x to %x\n", __FUNCTION__, original_value, onms5_onmstx_watchdog_en_reg.regValue);
            }
        }

        onms5_onmstx_watchdog_en_reg.onmstx_wd_to_main = _TRUE;	   //Watch Dog select to Main Display
        IoReg_Write32(ONMS5_onmstx_watchdog_en_reg, onms5_onmstx_watchdog_en_reg.regValue);

        txpretg_watchdog_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg);
        txpretg_watchdog_ctrl_reg.wde_to_background = _ENABLE;//FrameSync Watch Dog Enable (Force-to-Background)
        txpretg_watchdog_ctrl_reg.wde_to_free_run = _ENABLE;	//FrameSync Watch Dog Enable (FreeRun)
        IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg, txpretg_watchdog_ctrl_reg.regValue);

    }
    else
    {
        onms5_onmstx_watchdog_en_reg.regValue = 0;
        onms5_onmstx_watchdog_en_reg.onmstx_wd_to_main = _FALSE;		//Watch Dog select to Main Display
        IoReg_Write32(ONMS5_onmstx_watchdog_en_reg, onms5_onmstx_watchdog_en_reg.regValue);

        txpretg_watchdog_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg);
        txpretg_watchdog_ctrl_reg.wde_to_background = _DISABLE;//FrameSync Watch Dog Enable (Force-to-Background)
        txpretg_watchdog_ctrl_reg.wde_to_free_run = _DISABLE;	//FrameSync Watch Dog Enable (FreeRun)
        IoReg_Write32(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg, txpretg_watchdog_ctrl_reg.regValue);
    }

    if(_ENABLE == interruptenable)
    {
        onms5_onmstx_interrupt_en_reg.regValue = get_hdmi_online_wd_int_setting();

        {// Only for HDMI source, check particular device or debugging command
            unsigned int original_value = onms5_onmstx_interrupt_en_reg.regValue;
            onms5_onmstx_interrupt_en_reg.regValue = get_result_with_hdmi_mask(original_value);
            if(onms5_onmstx_interrupt_en_reg.regValue != original_value)
            {
                rtd_pr_hdmitx_info("[%s][HDMI] Update onms int from %x to %x\n", __FUNCTION__, original_value, onms5_onmstx_interrupt_en_reg.regValue);
            }
        }

        onms5_onmstx_interrupt_en_reg.onmstx_ie_to_main = _TRUE;		//Watch Dog select to Main Display
        IoReg_Write32(ONMS5_onmstx_interrupt_en_reg, onms5_onmstx_interrupt_en_reg.regValue);

        hdmitx_vgip_hdmitx_vgip_int_ctl_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_INT_CTL_reg);
        hdmitx_vgip_hdmitx_vgip_int_ctl_reg.onms1_int_ie = 1;//Enable onlinemeasure interrupt
        IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_INT_CTL_reg, hdmitx_vgip_hdmitx_vgip_int_ctl_reg.regValue);
    }
    else
    {
        onms5_onmstx_interrupt_en_reg.regValue = 0;
        onms5_onmstx_interrupt_en_reg.onmstx_ie_to_main = _FALSE;		//Watch Dog select to Main Display
        IoReg_Write32(ONMS5_onmstx_interrupt_en_reg, onms5_onmstx_interrupt_en_reg.regValue);

        hdmitx_vgip_hdmitx_vgip_int_ctl_reg.regValue = IoReg_Read32(HDMITX_VGIP_HDMITX_VGIP_INT_CTL_reg);
        hdmitx_vgip_hdmitx_vgip_int_ctl_reg.onms1_int_ie = 0;//Disable onlinemeasure interrupt
        IoReg_Write32(HDMITX_VGIP_HDMITX_VGIP_INT_CTL_reg, hdmitx_vgip_hdmitx_vgip_int_ctl_reg.regValue);
    }
}

unsigned char txpre_onms5_status_check(void)
{//Return FALSE: Online measure has error status.
    onms5_onmstx_status_RBUS onms5_onmstx_status_reg;
    unsigned int check_status_mask = 0;
    unsigned int original_value = 0;


    if(ONMS5_onmstx_ctrl_get_onmstx_ms_conti(IoReg_Read32(ONMS5_onmstx_ctrl_reg)) == 0) {
        //Mean onms5 online no enable
        return TRUE;
    }

    onms5_onmstx_status_reg.regValue = IoReg_Read32(ONMS5_onmstx_status_reg);
    original_value = onms5_onmstx_status_reg.regValue;
    onms5_onmstx_status_reg.regValue = get_result_with_hdmi_mask(original_value);
    if(onms5_onmstx_status_reg.regValue != original_value) {
        rtd_pr_hdmitx_notice("[%s][HDMI] channel: onms5, Update onms status from %x to %x\n", __FUNCTION__, original_value, onms5_onmstx_status_reg.regValue);
    }

    check_status_mask = get_hdmi_online_wd_int_setting();
    if(onms5_onmstx_status_reg.regValue & check_status_mask)
    {
        rtd_pr_hdmitx_notice("[%s][HDMI] onms5 status=0x%x, mask=0x%x\n", __FUNCTION__, onms5_onmstx_status_reg.regValue, check_status_mask);
        return FALSE;
    }

    return TRUE;
}

void txpre_clear_onms5_status(void)
{
    onms5_onmstx_status_RBUS onms5_onmstx_status_reg;

    onms5_onmstx_status_reg.regValue = IoReg_Read32(ONMS5_onmstx_status_reg);
    IoReg_Write32(ONMS5_onmstx_status_reg, onms5_onmstx_status_reg.regValue);//Clear online dma status
    rtd_pr_hdmitx_notice("####onms5 online error status:%08x, wd_en:%08x, int_en::%08x ####\n",
        onms5_onmstx_status_reg.regValue,
        IoReg_Read32(ONMS5_onmstx_watchdog_en_reg),
        IoReg_Read32(ONMS5_onmstx_interrupt_en_reg));
}
