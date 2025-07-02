//----------------------------------------------------------------------------------------------------
// ID Code      : FrameSync.c No.0000
// Update Note  :
//
//----------------------------------------------------------------------------------------------------

#ifndef BUILD_QUICK_SHOW
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <rtk_kdriver/io.h>
#else
#include <mach/rtk_platform.h>
#include <no_os/printk.h>
#include <no_os/semaphore.h>
#include <no_os/spinlock.h>
#include <no_os/spinlock_types.h>
#include <no_os/slab.h>
#include <timer.h>
#include <rtk_kdriver/rmm/rmm.h>
#include <rtd_log/rtd_module_log.h>
#include <no_os/pageremap.h>
#include "vby1/panel_api.h"
#include <sed_types.h>
#include <div64.h>
#endif


//FIXME #include <rbus/rbusScaledownReg.h>
#include <rbus/di_reg.h>
#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdomain_disp_reg.h>
#include <rbus/mdom_sub_cap_reg.h>
#include <rbus/mdom_sub_disp_reg.h>
#include <rbus/vgip_reg.h>
#include <rbus/sub_vgip_reg.h>
//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/ppoverlay_reg.h>
//#include <rbus/dds_pll_reg.h>//Fix Me. ADC register spec out
#include <rbus/vodma_reg.h>
#include <rbus/vodma2_reg.h>
//#include <rbus/patterngen_reg.h>
//#include <rbus/scaler/rbusCRTReg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rbus/pll_reg_reg.h>
#include <rbus/offms_reg.h>
#include <rbus/timer_reg.h>
#include <rbus/hdr_all_top_reg.h>
//#include <rbus/lg_m_plus_reg.h>
#include <rbus/onms_reg.h>

#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
	#include <scaler/scalerDrvCommon.h>
#else
	#include <scalercommon/scalerCommon.h>
	#include <scalercommon/scalerDrvCommon.h>
#endif
#endif

#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
//#include <tvscalercontrol/scaler/source.h>
//#include <tvscalercontrol/scaler/scalerlib.h>
//#include <tvscalercontrol/scaler/modeState.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scalerdrv/framesync.h>
#include <tvscalercontrol/scalerdrv/scalerclock.h>
#include <tvscalercontrol/scalerdrv/adjust.h>
#include <tvscalercontrol/scalerdrv/syncproc.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/scalerdrv/scaler_dtg.h>
#include <tvscalercontrol/adcsource/vga.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>
#include <tvscalercontrol/scaler/scalercolorlib.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <tvscalercontrol/vdc/video.h>
#include <tvscalercontrol/scalerdrv/power.h>
#include <scaler_vfedev.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
#include <rbus/ppoverlay_reg.h>
#include "scaler_vpqmemcdev.h"
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <scaler_vscdev.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>
//#include <tvscalercontrol/scalerdrv/mode.h>
//#include <tvscalercontrol/vip/gibi_od.h>

//#include <tvscalercontrol/scaler/scalerAPI.h>

// Double DVS to solve 1080p24 & 1080p30 video flicker
//#include <tvscalercontrol/vip/db_DVS_gamma.h>
//#include <tvscalercontrol/vip/color.h>
//#include <tvscalercontrol/vdc/video.h>
#include <tvscalercontrol/panel/panelapi.h>
#if IS_ENABLED(CONFIG_RTK_KDRV_PWM)
#include <rtk_kdriver/rtk_pwm.h>
#endif

#ifndef UT_flag
#ifndef FLOAT64
typedef double  __FLOAT64;
#define FLOAT64 __FLOAT64
#endif

static unsigned char framesync_enterlastline_at_frc_mode_flg = FALSE;

static unsigned char framesync_change_dv_total_flg = _FALSE;

// VO source SD video frame sync enable control
static unsigned char framesync_vo_sd_fs_enable_flg = _DISABLE; // _ENABLE;

#define _MEASURE_2FRAME_AT_ONCE
#define CONTINUE_MODE	0
#define SINGLE_MODE		1
#define _SELECT_MEASURE_MODE	SINGLE_MODE
#define _DCLK_SPREAD_RANGE (0)

//USER:LewisLee
//to prevent porch too small, let Panel abnormal
#define DV_TOTAL_FRONT_PORCH_OFFSET	3

#define IV2DV_2K_PANEL_MIN 2

//USER:LewisLee DATE:2011/02/23
//I have modify new Double DVS algo
//but not test finish
//#define DOUBLE_DVS_ORIGINAL_ALGO	1

#ifdef DOUBLE_DVS_ORIGINAL_ALGO
	//need nothing
#else //#ifdef DOUBLE_DVS_ORIGINAL_ALGO
void drvif_measure_total_pixels(unsigned char ucEnable);
void drvif_measure_dvtotal_last_line_pixels(unsigned char ucEnable);
#endif  //#ifdef DOUBLE_DVS_ORIGINAL_ALGO

extern PANEL_PIXEL_MODE get_panel_pixel_mode(void);
extern unsigned int Get_DISP_VERTICAL_TOTAL(void);
extern unsigned int Get_DISPLAY_CLOCK_TYPICAL(void);
extern unsigned int Get_DISPLAY_REFRESH_RATE(void);
extern unsigned int Get_DISP_HORIZONTAL_TOTAL(void);
#ifndef BUILD_QUICK_SHOW
extern unsigned int (*Get_Force_run_fake_scaler_State)(void);
#endif

//0x1802_8034[21:0], total 22 bit
#define TOTAL_PIXEL_CNT_MAX				0x3FFFFF

//0x1802_803C[27:16], total 12 bit
#define TOTAL_LINE_CNT_MAX					0xFFF

//0x1802_803C[11:0], total 12 bit
#define TOTAL_LAST_LINE_PIXEL_CNT_MAX	0xFFF
#endif // #ifdef UT_flag
//USER:LewisLee DATE:2013/08/27
//for some panel, range have 50 / 60 Hz two size
static UINT8 Scaler_FixLastLine_DVTotal_ByVfreq_Limit_Enable_Flag = _ENABLE;
static UINT32 Scaler_FixLastLine_DVTotal_Limit_60Hz_Typical = 0;
static UINT32 Scaler_FixLastLine_DVTotal_Limit_60Hz_Upper = 0;
static UINT32 Scaler_FixLastLine_DVTotal_Limit_60Hz_Lower = 0;
static UINT32 Scaler_FixLastLine_DVTotal_Limit_50Hz_Typical = 0;
static UINT32 Scaler_FixLastLine_DVTotal_Limit_50Hz_Upper = 0;
static UINT32 Scaler_FixLastLine_DVTotal_Limit_50Hz_Lower = 0;
#ifndef UT_flag
static unsigned char Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Enable_Flag = _DISABLE;
static unsigned int Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Upper = 0;
static unsigned int Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Lower = 0;
#endif // #ifndef UT_flag

void Scaler_Set_FixLastLine_DVTotoal_Limit_ByVfreq_Enable_Flag(UINT8 ucEnable)
{
	//Get Get_DISP_VERTICAL_TOTAL() original value
	Scaler_FixLastLine_DVTotal_ByVfreq_Limit_Enable_Flag = _DISABLE;

	//set DV total limit Upper / Lower, Ap can adjust
	Scaler_FixLastLine_DVTotal_Limit_60Hz_Typical = Get_DISP_VERTICAL_TOTAL();
	Scaler_FixLastLine_DVTotal_Limit_60Hz_Upper = (Get_DISP_VERTICAL_TOTAL()<<2)/3;
	Scaler_FixLastLine_DVTotal_Limit_60Hz_Lower = (Get_DISP_VERTICAL_TOTAL()*3)>>2;

	Scaler_FixLastLine_DVTotal_Limit_50Hz_Typical = Get_DISP_VERTICAL_TOTAL();
	Scaler_FixLastLine_DVTotal_Limit_50Hz_Upper = (Get_DISP_VERTICAL_TOTAL()<<2)/3;
	Scaler_FixLastLine_DVTotal_Limit_50Hz_Lower = (Get_DISP_VERTICAL_TOTAL()*3)>>2;

	if(_DISABLE == ucEnable)
		return;

	Scaler_FixLastLine_DVTotal_ByVfreq_Limit_Enable_Flag = ucEnable;

}

void Scaler_Set_FixLastLine_DVTotal_Limit_60Hz_Typical(UINT32 ucValue)
{
	Scaler_FixLastLine_DVTotal_Limit_60Hz_Typical = ucValue;
}

UINT32 Scaler_Get_FixLastLine_DVTotal_Limit_60Hz_Typical(void)
{
	if(Scaler_FixLastLine_DVTotal_Limit_60Hz_Typical != 0)
		return Scaler_FixLastLine_DVTotal_Limit_60Hz_Typical;
	else
		return Get_DISP_VERTICAL_TOTAL();
}

void Scaler_Set_FixLastLine_DVTotal_Limit_60Hz_Upper(UINT32 ucValue)
{
	Scaler_FixLastLine_DVTotal_Limit_60Hz_Upper = ucValue;
}

UINT32 Scaler_Get_FixLastLine_DVTotal_Limit_60Hz_Upper(void)
{
	if(Scaler_FixLastLine_DVTotal_Limit_60Hz_Upper != 0)
		return Scaler_FixLastLine_DVTotal_Limit_60Hz_Upper;
	else
		return Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MAX();
}

void Scaler_Set_FixLastLine_DVTotal_Limit_60Hz_Lower(UINT32 ucValue)
{
	Scaler_FixLastLine_DVTotal_Limit_60Hz_Lower = ucValue;
}

UINT32 Scaler_Get_FixLastLine_DVTotal_Limit_60Hz_Lower(void)
{
	if(Scaler_FixLastLine_DVTotal_Limit_60Hz_Lower != 0)
		return Scaler_FixLastLine_DVTotal_Limit_60Hz_Lower;
	else
		return Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN();
}

void Scaler_Set_FixLastLine_DVTotal_Limit_50Hz_Typical(UINT32 ucValue)
{
	Scaler_FixLastLine_DVTotal_Limit_50Hz_Typical = ucValue;
}

UINT32 Scaler_Get_FixLastLine_DVTotal_Limit_50Hz_Typical(void)
{
	if(Scaler_FixLastLine_DVTotal_Limit_50Hz_Typical != 0)
		return Scaler_FixLastLine_DVTotal_Limit_50Hz_Typical;
	else
		return (Get_DISPLAY_CLOCK_TYPICAL() / (Get_DISP_HORIZONTAL_TOTAL()*50));
}

void Scaler_Set_FixLastLine_DVTotal_Limit_50Hz_Upper(UINT32 ucValue)
{
	Scaler_FixLastLine_DVTotal_Limit_50Hz_Upper = ucValue;
}

UINT32 Scaler_Get_FixLastLine_DVTotal_Limit_50Hz_Upper(void)
{
	if(Scaler_FixLastLine_DVTotal_Limit_50Hz_Upper != 0)
		return Scaler_FixLastLine_DVTotal_Limit_50Hz_Upper;
	else
		return Get_PANEL_DISP_VERTICAL_TOTAL_50Hz_MAX();
}

void Scaler_Set_FixLastLine_DVTotal_Limit_50Hz_Lower(UINT32 ucValue)
{
	Scaler_FixLastLine_DVTotal_Limit_50Hz_Lower = ucValue;
}

UINT32 Scaler_Get_FixLastLine_DVTotal_Limit_50Hz_Lower(void)
{
	if(Scaler_FixLastLine_DVTotal_Limit_50Hz_Lower != 0)
		return Scaler_FixLastLine_DVTotal_Limit_50Hz_Lower;
	else
		return Get_PANEL_DISP_VERTICAL_TOTAL_50Hz_MIN();
}

UINT8 Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Enable_Flag(void)
{

	// [PR to SG] for PR -> SG 3D TCON, 3D need timing free run@60Hz
	if(Get_PANEL_3D_PR_TO_SG_OUTPUT_FRC_ENABLE() && drvif_scaelr3d_decide_is_3D_display_mode())
		return FALSE;

	return Scaler_FixLastLine_DVTotal_ByVfreq_Limit_Enable_Flag;
}

unsigned int get_outputFramerate(unsigned int inputFrameRate, unsigned int removeRatio, unsigned int multipleRatio)
{
	unsigned int outputFrameRate = 0;

	if( removeRatio == 0 )
	{
		removeRatio = 1;
		rtd_pr_vbe_err("[%s] ERROR : removeRatio = 0, set default removeRatio = %d\n", removeRatio);
	}

	outputFrameRate = inputFrameRate * multipleRatio / removeRatio;

	if( outputFrameRate == 0 )
	{
		outputFrameRate = Get_DISPLAY_REFRESH_RATE() * 1000;
		rtd_pr_vbe_err("[%s] ERROR : outputFrameRate = 0, set default outputFrameRate\n", __FUNCTION__);
	}

	return outputFrameRate;
}

unsigned int vbe_disp_vtotal_panel_boundary_protection(unsigned int DVtotal)
{
	if( DVtotal <= Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN() )
	{
		rtd_pr_vbe_notice("ERROR : DVtotal(%d) is out of boundary\n", DVtotal);
		DVtotal = Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN();
		rtd_pr_vbe_notice("ERROR : Return DVtotal as minimum value : %d\n", DVtotal);
	}

	return DVtotal;
}

unsigned int decide_vbe_disp_vtotal(unsigned int vbe_dvtotal_status, unsigned int vbe_input_framerate_source, unsigned int check_update, unsigned int vfreq_for_vtotal, unsigned char display)
{
	unsigned int DVtotal = 0;
	unsigned int inputVsyncCount;
	unsigned int inputFrameRate;
	unsigned int removeRatio;
	unsigned int multipleRatio;
	unsigned int outputFrameRate;
	ppoverlay_dv_total_RBUS dv_total_reg;
	ppoverlay_dtg_m_remove_input_vsync_RBUS ppoverlay_dtg_m_remove_input_vsync_reg;
	ppoverlay_dtg_m_multiple_vsync_RBUS ppoverlay_dtg_m_multiple_vsync_reg;
	ppoverlay_uzudtg_fractional_fsync_RBUS uzudtg_fractional_fsync_reg;
	vodma_vodma_pvs_free_RBUS vodma_vodma_pvs_free_reg;

	rtd_pr_vbe_notice("[%s] Decision : %d\n", __FUNCTION__, vbe_dvtotal_status);

	if( ( Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI ) )
	{
		DVtotal = Get_DISP_VERTICAL_TOTAL();
		rtd_pr_vbe_notice("[%s] Convert board case, return final DVtotal by display(%d)\n", __FUNCTION__, DVtotal);
		return DVtotal;
	}

	if( vbe_dvtotal_status == VBE_DVTOTAL_BY_DISP )
	{
		DVtotal = Get_DISP_VERTICAL_TOTAL();
	}
	else if( vbe_dvtotal_status == VBE_DVTOTAL_PRECISE )
	{
//		rtd_pr_vbe_notice("[%s][line:%d] vbe_input_framerate_source : %d, check_update : %d\n", __FUNCTION__, __LINE__, vbe_input_framerate_source, check_update);

		// Get input frame rate
		if( vbe_input_framerate_source == VBE_INPUT_FRAMERATE_VOPERIOD )
		{
			vodma_vodma_pvs_free_reg.regValue = IoReg_Read32(VODMA_VODMA_PVS_Free_reg);
			inputVsyncCount = vodma_vodma_pvs_free_reg.pvs0_free_period;
			if( inputVsyncCount == 0 )
			{
				DVtotal = Get_DISP_VERTICAL_TOTAL();
				rtd_pr_vbe_err("[%s] ERROR : inputVsyncCount = 0, return final DVtotal by display(%d)\n", __FUNCTION__, DVtotal);
				return DVtotal;
			}
			inputFrameRate = (unsigned int)( 27000000000ULL / (unsigned long long)inputVsyncCount );	// multiple 1000
		}
		else
		{
			if( display == SLR_SUB_DISPLAY )
				inputFrameRate = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_V_FREQ_1000);
			else
				inputFrameRate = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000);
		}
//		rtd_pr_vbe_notice("[%s] inputFrameRate : %d\n", __FUNCTION__, inputFrameRate);

		// Get remove & multiple value ( ratio between input and output frame rate )
		if(get_MEMC_bypass_status_refer_platform_model())
		{
			ppoverlay_dtg_m_multiple_vsync_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_M_multiple_vsync_reg);
			ppoverlay_dtg_m_remove_input_vsync_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_M_Remove_input_vsync_reg);
			removeRatio = ppoverlay_dtg_m_remove_input_vsync_reg.remove_half_ivs_mode2 + 1;
			multipleRatio = ppoverlay_dtg_m_multiple_vsync_reg.dtg_m_multiple_vsync + 1;
		}
		else
		{
			uzudtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_fractional_fsync_reg);
			removeRatio = uzudtg_fractional_fsync_reg.uzudtg_multiple_vsync + 1;
			multipleRatio = uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode + 1;
		}
//		rtd_pr_vbe_notice("[%s] removeRatio : %d, multipleRatio : %d\n", __FUNCTION__, removeRatio, multipleRatio);

		// Calculate output frame rate
		if( removeRatio == 0 )
		{
			DVtotal = Get_DISP_VERTICAL_TOTAL();
			rtd_pr_vbe_err("[%s] ERROR : removeRatio = 0, return final DVtotal by display(%d)\n", __FUNCTION__, DVtotal);
			return DVtotal;
		}
		outputFrameRate = get_outputFramerate(inputFrameRate, removeRatio, multipleRatio);	// inputFrameRate * multipleRatio / removeRatio;
//		rtd_pr_vbe_notice("[%s] outputFrameRate : %d\n", __FUNCTION__, outputFrameRate);

		// Calculate Display Vtotal : multiple 2000 and shift 1 for rounding
		if( ( Get_DISP_HORIZONTAL_TOTAL() == 0 ) || ( outputFrameRate == 0 ) )
		{
			DVtotal = Get_DISP_VERTICAL_TOTAL();
			rtd_pr_vbe_err("[%s] ERROR : DHtotal = %d, outputFrameRate = %d, return final DVtotal by display(%d)\n", __FUNCTION__, Get_DISP_HORIZONTAL_TOTAL(), outputFrameRate, DVtotal);
			return DVtotal;
		}
		if( get_panel_pixel_mode() == PANEL_1_PIXEL )
			DVtotal = (unsigned int)( ( (unsigned long long)Get_DISPLAY_CLOCK_TYPICAL() * 2000ULL ) / ( (unsigned long long)Get_DISP_HORIZONTAL_TOTAL() * (unsigned long long)outputFrameRate ) );
		else
			DVtotal = (unsigned int)( ( (unsigned long long)Get_DISPLAY_CLOCK_TYPICAL() * 4000ULL ) / ( (unsigned long long)Get_DISP_HORIZONTAL_TOTAL() * (unsigned long long)outputFrameRate ) );
		DVtotal = ( ( DVtotal + 1 ) >> 1 );
//		rtd_pr_vbe_notice("[%s] DVtotal : %d\n", __FUNCTION__, DVtotal);

		// check current DVtotal need update or not
		if(check_update)
		{
			dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
			if( DVtotal == ( dv_total_reg.dv_total - 1 ) )
			{
				rtd_pr_vbe_notice("[%s] DVtotal is not changed, so return\n", __FUNCTION__);
				return FALSE;
			}
		}
	}
	else if( vbe_dvtotal_status == VBE_DVTOTAL_BY_VFREQ )
	{
		rtd_pr_vbe_notice("[%s][line:%d] vfreq_for_vtotal : %d\n", __FUNCTION__, __LINE__, vfreq_for_vtotal);

		if( ( Get_DISP_HORIZONTAL_TOTAL() == 0 ) || ( vfreq_for_vtotal == 0 ) )
		{
			DVtotal = Get_DISP_VERTICAL_TOTAL();
			rtd_pr_vbe_err("[%s] ERROR : DHtotal = %d, vfreq_for_vtotal = %d, return final DVtotal by display(%d)\n", __FUNCTION__, Get_DISP_HORIZONTAL_TOTAL(), vfreq_for_vtotal, DVtotal);
			return DVtotal;
		}
		if( ( get_panel_pixel_mode() > PANEL_1_PIXEL ) && ( vfreq_for_vtotal > 60 ) )
			DVtotal = Get_DISPLAY_CLOCK_TYPICAL() * 2 / Get_DISP_HORIZONTAL_TOTAL() / vfreq_for_vtotal;
		else
			DVtotal = Get_DISPLAY_CLOCK_TYPICAL() / Get_DISP_HORIZONTAL_TOTAL() / vfreq_for_vtotal;
//		rtd_pr_vbe_notice("[%s] DVtotal : %d\n", __FUNCTION__, DVtotal);
	}

	// Panel boundary protection
	DVtotal = vbe_disp_vtotal_panel_boundary_protection(DVtotal);

	rtd_pr_vbe_notice("[%s] Final DVtotal : %d\n", __FUNCTION__, DVtotal);

	return DVtotal;
}

unsigned char data_fsync_go_expand_uzudtg_htotal(void)
{
	//if( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC) && ( ( Get_DISP_ACT_END_HPOS() == 3440 ) && ( Get_DISP_ACT_END_VPOS() == 1440 ) && ( Get_DISPLAY_REFRESH_RATE() == 165 ) ) )
	//	return TRUE;

	return FALSE;
}

unsigned int decide_expand_uzudtg_htotal(void)
{
	sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;
	unsigned int uzudtg_htotal = Get_DISP_HORIZONTAL_TOTAL();
	unsigned int vo_clock = 0;
	unsigned int vo_active_rec = 0;
	unsigned int uzudtg_clock = 0;
	unsigned int fract_a = 0;
	unsigned int fract_b = 0;
	unsigned int uzudtg_active_rec = 0;

	// expand uzu_htotal if uzudtg_active < vo_active
	// vo_clock = vo_htotal * vo_vtotal * vo_framerate ( do not consider vo increasing clock )
	vo_clock = (unsigned int)( (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN) * (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN) * (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) / 1000ULL );
	// vo_active_rec = vo_clock / ( vo_htotal * vo_vactive ) : 2-digit accuracy
	if( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN) != 0 ) && ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_LEN) != 0 ) )
		vo_active_rec = (unsigned int)( ( (unsigned long long)vo_clock * 100ULL ) / ( (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN) * (unsigned long long)Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DI_LEN) ) );

	sys_dispclksel_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
	fract_a = sys_dispclksel_reg.dclk_fract_a + 1;
	fract_b = sys_dispclksel_reg.dclk_fract_b + 1;
	if( fract_b != 0 )
		uzudtg_clock = Get_DISPLAY_CLOCK_TYPICAL() * fract_a / fract_b;
	// uzudtg_active_rec = uzudtg_clock / ( uzudtg_htotal * uzudtg_vactive ) : 2-digit accuracy
	if( Get_DISP_HORIZONTAL_TOTAL() != 0 )
		uzudtg_active_rec = (unsigned int)( ( (unsigned long long)uzudtg_clock * 100ULL ) / ( (unsigned long long)Get_DISP_HORIZONTAL_TOTAL() * 1440ULL ) );

//	rtd_pr_vbe_notice("[%s] vo_clock = %d, vo_active_rec = %d, uzudtg_clock = %d, uzudtg_active_rec = %d\n", __FUNCTION__, (vo_clock/1000000), vo_active_rec, (uzudtg_clock/1000000), uzudtg_active_rec);

	if( ( vo_active_rec < uzudtg_active_rec ) && ( vo_active_rec > 0 ) )
	{
		uzudtg_htotal = uzudtg_htotal * uzudtg_active_rec / vo_active_rec;

		if( uzudtg_htotal % 2 )
			uzudtg_htotal += 1;
		else
			uzudtg_htotal += 2;
	}

	return uzudtg_htotal;
}

unsigned int decide_uzudtg_vtotal_by_expand_uzudtg_htotal(unsigned int vbe_input_framerate_source, unsigned char display, unsigned int uzudtg_htotal)
{
	vodma_vodma_pvs_free_RBUS vodma_vodma_pvs_free_reg;
	sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;
	unsigned int uzudtg_vtotal = 0;
	unsigned int inputVsyncCount;
	unsigned int inputFrameRate;
	unsigned int fract_a = 0;
	unsigned int fract_b = 0;

	// Get input frame rate
	if( vbe_input_framerate_source == VBE_INPUT_FRAMERATE_VOPERIOD )
	{
		vodma_vodma_pvs_free_reg.regValue = IoReg_Read32(VODMA_VODMA_PVS_Free_reg);
		inputVsyncCount = vodma_vodma_pvs_free_reg.pvs0_free_period;
		if( inputVsyncCount == 0 )
		{
			uzudtg_vtotal = Get_DISP_VERTICAL_TOTAL();
			rtd_pr_vbe_err("[%s] ERROR : inputVsyncCount = 0, return uzudtg_vtotal by display(%d)\n", __FUNCTION__, uzudtg_vtotal);
			return uzudtg_vtotal;
		}
		inputFrameRate = (unsigned int)( 27000000000ULL / (unsigned long long)inputVsyncCount );	// multiple 1000
	}
	else
	{
		if( display == SLR_SUB_DISPLAY )
			inputFrameRate = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_V_FREQ_1000);
		else
			inputFrameRate = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000);
	}

	sys_dispclksel_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
	fract_a = sys_dispclksel_reg.dclk_fract_a + 1;
	fract_b = sys_dispclksel_reg.dclk_fract_b + 1;
	if( ( fract_b == 0 ) || ( uzudtg_htotal == 0 ) || ( inputFrameRate == 0 ) )
	{
		uzudtg_vtotal = Get_DISP_VERTICAL_TOTAL();
		rtd_pr_vbe_err("[%s] ERROR : fract_b = %d, uzudtg_htotal = %d, inputFrameRate = %d, return final DVtotal by display(%d)\n", __FUNCTION__, fract_b, uzudtg_htotal, inputFrameRate, uzudtg_vtotal);
		return uzudtg_vtotal;
	}
	uzudtg_vtotal = (unsigned int)( ( (unsigned long long)Get_DISPLAY_CLOCK_TYPICAL() * (unsigned long long)fract_a * 2000ULL ) / ( (unsigned long long)fract_b * (unsigned long long)uzudtg_htotal * (unsigned long long)inputFrameRate ) );
	uzudtg_vtotal = ( ( uzudtg_vtotal + 1 ) >> 1 );

	return uzudtg_vtotal;
}

void decide_uzudtg_v_den_by_expand_uzudtg_htotal(unsigned int uzudtg_vtotal, unsigned int *den_v_start, unsigned int *den_v_end)
{
	unsigned int uzudtg_backporch;
	if(den_v_start == NULL || den_v_end == NULL)
	{
		rtd_pr_vbe_err("[%s %d] input parameter error\n", __FUNCTION__, __LINE__);
		return;
	}
	
	if( uzudtg_vtotal >= Get_DISP_ACT_END_VPOS() )
	{
		uzudtg_backporch = ( uzudtg_vtotal - Get_DISP_ACT_END_VPOS() ) / 2;
		*den_v_start = uzudtg_backporch;
		*den_v_end = uzudtg_backporch + Get_DISP_ACT_END_VPOS();
		rtd_pr_vbe_notice("[%s] uzu_dv_den change to uzu_dv_den_sta : %d, uzu_dv_den_end : %d\n", __FUNCTION__, uzudtg_backporch, ( uzudtg_backporch + Get_DISP_ACT_END_VPOS() ));
	}
	else
	{
		rtd_pr_vbe_err("[%s] ERROR : UZU Vtotal is smaller than UZU Vden\n");
	}
}

#ifndef UT_flag
UINT32 Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Typical(void)
{
	if( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),V_FREQ_50000) < 3000 )	// 47Hz~53Hz input
		return Scaler_Get_FixLastLine_DVTotal_Limit_50Hz_Typical();
	else	// if( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),V_FREQ_50000) >= 3000 )
		return Scaler_Get_FixLastLine_DVTotal_Limit_60Hz_Typical();
}

UINT32 Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Upper(void)
{
	if( ( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),V_FREQ_50000) < 3000 ) ||
		( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),V_FREQ_25000) < 2000 ) )	// 47Hz~53Hz input
		return Scaler_Get_FixLastLine_DVTotal_Limit_50Hz_Upper();
	else	// if( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),50000) >= 3000 )
		return Scaler_Get_FixLastLine_DVTotal_Limit_60Hz_Upper();
}

UINT32 Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Lower(void)
{
	if( ( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),V_FREQ_50000) < 3000 ) ||
		( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),V_FREQ_25000) < 2000 ) )	// 47Hz~53Hz input
		return Scaler_Get_FixLastLine_DVTotal_Limit_50Hz_Lower();
	else	// if( ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),V_FREQ_50000) >= 3000 )
		return Scaler_Get_FixLastLine_DVTotal_Limit_60Hz_Lower();
}

unsigned char Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Enable_Flag(void)
{
//#ifdef VBY_ONE_PANEL
	if(Get_DISPLAY_PANEL_TYPE() == P_VBY1_4K2K)
		return _TRUE;	// Millet3 4k2k timing don't use this flag
//#else
	if(Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES() && (Get_DISPLAY_2k1k_output_mode_status() == 0))
		return _FALSE;	// Millet3 4k2k timing don't use this flag
//#endif

	return Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Enable_Flag;
}

void Scaler_Set_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Enable_Flag(unsigned char ucEnable)
{
	Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Enable_Flag = ucEnable;
}

void Scaler_Set_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Upper(unsigned int ucValue)
{
	Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Upper = ucValue;
}

unsigned int Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Upper(void)
{
	if(_ENABLE == Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Enable_Flag()){
		return Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Upper();
	}
	return Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Upper;
}

void Scaler_Set_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Lower(unsigned int ucValue)
{
	Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Lower= ucValue;
}

unsigned int Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Lower(void)
{
	if(_ENABLE == Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Enable_Flag()){
		return Scaler_Get_FixLastLine_DVTotoal_Limit_ByVfreq_Lower();
	}
	return Scaler_FRC_Open_Fix_Lastline_FixDclk_DVTotal_Limit_Lower;
}

//USER: LewisLee DATE:2010/05/25
//in Fix Last lIne, Double DVS etc, maybe change Dv total
//we need set it to default value at mode reset
void drvif_framesync_reset_dv_total_flg(unsigned char flg)
{
	framesync_change_dv_total_flg = flg;
}

//User:LewisLee DATE:2012/11/26
//to prevent DV total too small, panel abnormal
unsigned int framesync_get_dvtotal_min(void)
{
	return (Get_DISP_DEN_END_VPOS() + DV_TOTAL_FRONT_PORCH_OFFSET);
}

//USER:LewisLee DATE:2012/11/29
//to dynamic adjust Panel Den position
//to prevent over panel spec or framesync error
unsigned int framesync_dynamic_adjust_dvtotal_den_relation(unsigned int CurDVtotal)
{
#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT_BY_ADJUST_DENVPOS
	UINT32 ModifyDvtotal = 0, DVtotalMin = 0;
	UINT32 OriginalDenStart = 0, OriginalDenEnd = 0, Porchoffset = 0;
	ppoverlay_dv_den_start_end_RBUS dv_den_start_end_reg;

	if(_ENABLE == Scaler_Get_Dynamic_Adjust_Display_DenVPos_Flag())
	{
		DVtotalMin = framesync_get_dvtotal_min();
		ModifyDvtotal = DVtotalMin; //initial value
		Scaler_Set_Dynamic_Adjust_Display_DenVPos_Start(0);
		Scaler_Set_Dynamic_Adjust_Display_DenVPos_End(0);

		if(CurDVtotal >= DVtotalMin)
		{
			// it is normal case
			return CurDVtotal;
		}

		OriginalDenStart = Get_DISP_DEN_STA_VPOS();
		OriginalDenEnd = Get_DISP_DEN_END_VPOS();

		if(DVtotalMin >= (OriginalDenEnd - OriginalDenStart
			+ Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min()
			+ Scaler_Get_Dynamic_Adjust_Display_DenVPos_FrontPorch_Min()))
		{
			//porch is enough, just DV start too large, let front porch not enough
			//move up Den position
			Porchoffset = DVtotalMin - CurDVtotal;

			if(OriginalDenStart >= Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min())
			{
				if((OriginalDenStart >= Porchoffset)
					&& ((OriginalDenStart - Porchoffset) >= Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min()))
				{
					//move up Porchoffset
					Scaler_Set_Dynamic_Adjust_Display_DenVPos_Start(OriginalDenStart - Porchoffset);
					Scaler_Set_Dynamic_Adjust_Display_DenVPos_End(OriginalDenEnd - Porchoffset);
				}
				else
				{
					//it should not happen in this case, just prevent error
					Porchoffset = OriginalDenStart - Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min();

					Scaler_Set_Dynamic_Adjust_Display_DenVPos_Start(Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min());
					Scaler_Set_Dynamic_Adjust_Display_DenVPos_End(OriginalDenEnd - Porchoffset);
				}

				dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DV_DEN_Start_End_reg);
				dv_den_start_end_reg.dv_den_sta = Get_DISP_DEN_STA_VPOS();
				dv_den_start_end_reg.dv_den_end = Get_DISP_DEN_END_VPOS();
				IoReg_Write32(PPOVERLAY_DV_DEN_Start_End_reg, dv_den_start_end_reg.regValue);

				ModifyDvtotal = Get_DISP_DEN_END_VPOS() + Scaler_Get_Dynamic_Adjust_Display_DenVPos_FrontPorch_Min();

				if(CurDVtotal >= ModifyDvtotal)
				{
					//Den move to save region, don't need to change DV total
					//return original value
					ModifyDvtotal = CurDVtotal;
				}
			}
			else// if(OriginalDenStart < Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min())
			{
				//it should not happen in this case, just prevent error
				//move Den down
				Porchoffset = Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min() - OriginalDenStart;

				Scaler_Set_Dynamic_Adjust_Display_DenVPos_Start(Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min());
				Scaler_Set_Dynamic_Adjust_Display_DenVPos_End(OriginalDenEnd + Porchoffset);

				dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DV_DEN_Start_End_reg);
				dv_den_start_end_reg.dv_den_sta = Get_DISP_DEN_STA_VPOS();
				dv_den_start_end_reg.dv_den_end = Get_DISP_DEN_END_VPOS();
				IoReg_Write32(PPOVERLAY_DV_DEN_Start_End_reg, dv_den_start_end_reg.regValue);

				ModifyDvtotal = Get_DISP_DEN_END_VPOS() + Scaler_Get_Dynamic_Adjust_Display_DenVPos_FrontPorch_Min();

				if(CurDVtotal >= ModifyDvtotal)
				{
					//Den move to save region, don't need to change DV total
					//return original value
					ModifyDvtotal = CurDVtotal;
				}
			}

			return ModifyDvtotal;
		}
		else
		{
			//porch is not enough, need to move Den position and DV total
			if(OriginalDenStart >= Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min())
			{
				//move up Den position
				Porchoffset = OriginalDenStart - Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min();
				Scaler_Set_Dynamic_Adjust_Display_DenVPos_Start(Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min());
				Scaler_Set_Dynamic_Adjust_Display_DenVPos_End(OriginalDenEnd - Porchoffset);
			}
			else// if(OriginalDenStart < Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min())
			{
				//move down Den position
				Porchoffset = Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min() - OriginalDenStart;
				Scaler_Set_Dynamic_Adjust_Display_DenVPos_Start(Scaler_Get_Dynamic_Adjust_Display_DenVPos_BackPorch_Min());
				Scaler_Set_Dynamic_Adjust_Display_DenVPos_End(OriginalDenEnd + Porchoffset);
			}

			dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DV_DEN_Start_End_reg);
			dv_den_start_end_reg.dv_den_sta = Get_DISP_DEN_STA_VPOS();
			dv_den_start_end_reg.dv_den_end = Get_DISP_DEN_END_VPOS();
			IoReg_Write32(PPOVERLAY_DV_DEN_Start_End_reg, dv_den_start_end_reg.regValue);

			ModifyDvtotal = Get_DISP_DEN_END_VPOS() + Scaler_Get_Dynamic_Adjust_Display_DenVPos_FrontPorch_Min();

			if(CurDVtotal >= ModifyDvtotal)
			{
				//Den move to save region, don't need to change DV total
				//return original value
				ModifyDvtotal = CurDVtotal;
			}

			return ModifyDvtotal;
		}
	}
#endif //#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT_BY_ADJUST_DENVPOS

	return framesync_get_dvtotal_min();
}

#define DYNAMIC_IV2DV_DELAY_RULE 1
#define PIXEL_SHIFT_RESOLUTION 100

extern unsigned char drv_memory_get_game_mode(void);
extern unsigned char vbe_disp_get_bringup_vbe_test(void);

#define SUPPORT_DISPLAY_FRAMERATE_60HZ_PANEL_MIN 46000
#define SUPPORT_DISPLAY_FRAMERATE_60HZ_PANEL_MAX 61000
#define SUPPORT_DISPLAY_FRAMERATE_165HZ_PANEL_MIN 46000
#define SUPPORT_DISPLAY_FRAMERATE_165HZ_PANEL_MAX 166000

void framesync_fixlastline_set_precision_vtotal(void)
{
	unsigned int verticalTotal = 0;
	unsigned int count = 0x3FFFFF;
	unsigned int i = 0;
	ppoverlay_ivs_cnt_1_RBUS ppoverlay_ivs_cnt_1_reg;
	ppoverlay_ivs_cnt_2_RBUS ppoverlay_ivs_cnt_2_reg;
	ppoverlay_dv_total_RBUS dv_total_reg;
	ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
	ppoverlay_memcdtg_dv_total_RBUS memcdtg_dv_total_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;

	if( ( Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI ) )
		return;

	framesync_set_iv2dv_delay(scaler_dtg_get_app_type(), DISPLAY_MODE_NEW_FLL_MODE);

	ppoverlay_ivs_cnt_1_reg.regValue = IoReg_Read32(PPOVERLAY_IVS_cnt_1_reg);
	ppoverlay_ivs_cnt_1_reg.measure_ivs_cnt_src_sel = 0;
	IoReg_Write32(PPOVERLAY_IVS_cnt_1_reg, ppoverlay_ivs_cnt_1_reg.regValue);
	for( i = 0 ; i < 5 ; i++ )
	{
		ppoverlay_ivs_cnt_1_reg.regValue = IoReg_Read32(PPOVERLAY_IVS_cnt_1_reg);
		ppoverlay_ivs_cnt_2_reg.regValue = IoReg_Read32(PPOVERLAY_IVS_cnt_2_reg);
		rtd_pr_vbe_debug("[%s] ivs1:%x, ivs2:%x\n", __FUNCTION__, ppoverlay_ivs_cnt_1_reg.ivs_cnt_even, ppoverlay_ivs_cnt_2_reg.ivs_cnt_odd);
		WaitFor_IVS1();
	}

	verticalTotal = decide_vbe_disp_vtotal(VBE_DVTOTAL_PRECISE,VBE_INPUT_FRAMERATE_INFO,FALSE,0,0);

	if(verticalTotal)
	{
		dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
		dv_total_reg.dv_total= verticalTotal - 1;
		IoReg_Write32(PPOVERLAY_DV_total_reg, dv_total_reg.regValue);
		vbe_disp_pwm_frequency_update(FALSE);
		uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
		uzudtg_dv_total_reg.uzudtg_dv_total = verticalTotal - 1;
		IoReg_Write32(PPOVERLAY_uzudtg_DV_TOTAL_reg, uzudtg_dv_total_reg.regValue);
		memcdtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_TOTAL_reg);
		memcdtg_dv_total_reg.memcdtg_dv_total = verticalTotal - 1;
		IoReg_Write32(PPOVERLAY_memcdtg_DV_TOTAL_reg, memcdtg_dv_total_reg.regValue);
	}
	else
	{
		rtd_pr_vbe_err("[%s] ERROR : verticalTotal = 0\n", __FUNCTION__);
		return;
	}

	double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
	double_buffer_ctrl_reg.dreg_dbuf_set = 1;	// D7 db apply
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);
	double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;	// D4 db apply
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

	while( ( double_buffer_ctrl_reg.dreg_dbuf_set || double_buffer_ctrl2_reg.uzudtgreg_dbuf_set ) && --count )
	{
		double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	}

	if( count == 0 )
		rtd_pr_vbe_notice("[%s] double buffer timeout !!!\n", __FUNCTION__);
}

void framesync_set_vtotal_by_vodma_freerun_period(void)
{
	unsigned int verticalTotal = 0;
	unsigned int uzudtg_vtotal = 0;
	unsigned int uzudtg_htotal = decide_expand_uzudtg_htotal();
	unsigned int count = 0x3fffff;
	ppoverlay_dv_total_RBUS dv_total_reg;
	ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
	ppoverlay_memcdtg_dv_total_RBUS memcdtg_dv_total_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;

	if( ( Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI ) )
		return;

	verticalTotal = decide_vbe_disp_vtotal(VBE_DVTOTAL_PRECISE,VBE_INPUT_FRAMERATE_VOPERIOD,FALSE,0,0);

	if(verticalTotal)
	{
		dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
		dv_total_reg.dv_total = verticalTotal - 1;
		IoReg_Write32(PPOVERLAY_DV_total_reg, dv_total_reg.regValue);

		vbe_disp_pwm_frequency_update(FALSE);

		uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
		if(data_fsync_go_expand_uzudtg_htotal())
		{
			uzudtg_vtotal = decide_uzudtg_vtotal_by_expand_uzudtg_htotal(VBE_INPUT_FRAMERATE_VOPERIOD, 0, uzudtg_htotal);

			if( uzudtg_vtotal != verticalTotal )
				rtd_pr_vbe_notice("[%s] shrink uzudtg_vtotal ( according expand_uzudtg_htotal ) from %d to %d\n", __FUNCTION__, verticalTotal, uzudtg_vtotal);
		}
		else
		{
			uzudtg_vtotal = verticalTotal;
		}
		uzudtg_dv_total_reg.uzudtg_dv_total = uzudtg_vtotal - 1;
		IoReg_Write32(PPOVERLAY_uzudtg_DV_TOTAL_reg, uzudtg_dv_total_reg.regValue);

		memcdtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_TOTAL_reg);
		memcdtg_dv_total_reg.memcdtg_dv_total = verticalTotal - 1;
		IoReg_Write32(PPOVERLAY_memcdtg_DV_TOTAL_reg, memcdtg_dv_total_reg.regValue);
	}
	else
	{
		rtd_pr_vbe_err("[%s] ERROR : verticalTotal = 0\n", __FUNCTION__);
		return;
	}

	double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
	double_buffer_ctrl_reg.dreg_dbuf_set = 1;	// D7 db apply
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);
	double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;	// D4 db apply
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

	while( ( double_buffer_ctrl_reg.dreg_dbuf_set || double_buffer_ctrl2_reg.uzudtgreg_dbuf_set ) && --count )
	{
		double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	}

	if( count == 0 )
		rtd_pr_vbe_notice("[%s] double buffer timeout !!!\n", __FUNCTION__);
}

void framesync_set_vtotal_by_vodma_freerun_period_isr(void)
{
	unsigned int verticalTotal = 0;
	unsigned int uzudtg_vtotal = 0;
	unsigned int uzudtg_htotal = decide_expand_uzudtg_htotal();
	ppoverlay_dv_total_RBUS dv_total_reg;
	ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
	ppoverlay_memcdtg_dv_total_RBUS memcdtg_dv_total_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;

	if( ( Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI ) || ( Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI ) )
		return;

	verticalTotal = decide_vbe_disp_vtotal(VBE_DVTOTAL_PRECISE,VBE_INPUT_FRAMERATE_VOPERIOD,TRUE,0,0);
	if( verticalTotal == FALSE )
	{
		rtd_pr_vbe_notice("[%s] check that verticalTotal does not need to refresh, or gets 0 value ...\n", __FUNCTION__);
		return;
	}

	double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
	if( VODMA_VODMA_PVS0_Gen_get_iv_src_sel(IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg)) == 0 )	// D7 db enable
		double_buffer_ctrl_reg.dreg_dbuf_en = 1;	// When VO is freerun, enable double buffer to apply Dvtotal in next frame
	else
		double_buffer_ctrl_reg.dreg_dbuf_en = 0;	// When VO is sync i3, disable double buffer to make vtotal apply immediately
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);

	double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 0;	// D4 db enable
	double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 0;	// D5 db enable
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

	dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
	dv_total_reg.dv_total= verticalTotal - 1;
	IoReg_Write32(PPOVERLAY_DV_total_reg, dv_total_reg.regValue);

	vbe_disp_pwm_frequency_update(FALSE);

	uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
	if(data_fsync_go_expand_uzudtg_htotal())
	{
		uzudtg_vtotal = decide_uzudtg_vtotal_by_expand_uzudtg_htotal(VBE_INPUT_FRAMERATE_VOPERIOD, 0, uzudtg_htotal);

		if( uzudtg_vtotal != verticalTotal )
			rtd_pr_vbe_notice("[%s] shrink uzudtg_vtotal ( according expand_uzudtg_htotal ) from %d to %d\n", __FUNCTION__, verticalTotal, uzudtg_vtotal);
	}
	else
	{
		uzudtg_vtotal = verticalTotal;
	}
	uzudtg_dv_total_reg.uzudtg_dv_total = uzudtg_vtotal - 1;
	IoReg_Write32(PPOVERLAY_uzudtg_DV_TOTAL_reg, uzudtg_dv_total_reg.regValue);

	memcdtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_TOTAL_reg);
	memcdtg_dv_total_reg.memcdtg_dv_total = verticalTotal - 1;
	IoReg_Write32(PPOVERLAY_memcdtg_DV_TOTAL_reg, memcdtg_dv_total_reg.regValue);

	// When VO is freerun, apply double buffer
	if( VODMA_VODMA_PVS0_Gen_get_iv_src_sel(IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg)) == 0 )
	{
		double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		double_buffer_ctrl_reg.dreg_dbuf_set = 1;	// D7 db apply
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);
		double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;	// D4 db apply
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);
	}
}

/********************************************************
      !  |----------------|  !
      -------------------------
         t1                  t3

	!  |----------------|  !
      ----------------------------
    iv2dv  t2               t4

t1:  vo_vsta
t2:  uzu den start
t3:  vo ivs
t4:  uzu den end

constraint:
1) t1 + vo_gatting_offset <= iv2dv_dly + t2
2) iv2dv + t4 < t3
********************************************************/
unsigned int framesync_get_video_line_buffer_delay(DTG_APP_TYPE app_type)
{
    // vodma_vodma_clkgen_RBUS vodma_vodma_clkgen_reg;
    vodma_vodma_v1sgen_RBUS vodma_vodma_v1sgen_reg;
    vodma_vodma_v1vgip_vact1_RBUS vodma_vodma_v1vgip_vact1_reg;
    unsigned int iv2dv_line = MAX_UZD_TO_MDOMAIN_LINE_DELAY;
    unsigned int vodma_pll = VO_Get_vodmapll();
    unsigned int vo_htotal = 0;
    unsigned int vo_den_start = 0;
    unsigned int iv2dv_based_clock = 27000000;
    unsigned int iv2dv_htotal = 0;
    unsigned int uzudtg_den_start = 0;
    DTG_TIMING_INFO timing_info = {0};

    vodma_vodma_v1vgip_vact1_reg.regValue = IoReg_Read32(VODMA_VODMA_V1VGIP_VACT1_reg);
    vo_den_start = vodma_vodma_v1vgip_vact1_reg.v_st;

    if(app_type == DTG_DISPLAY_MONITOR)
    {
        scaler_dtg_get_timing(DTG_UZU, &timing_info);
        uzudtg_den_start = timing_info.vstart;
        // iv2dv_dly is based on VO pixel clock
        // Assume that VO linerate is close to UZU linerate
        // uzudtg_den_start + iv2dv_dly >= VO_den_start + MAX_UZD_TO_MDOMAIN_LINE_DELAY ( HW value ) + MAX_UZU_LINE_BUFFER_LINE_CNT ( according Ver_mode_sel )
        // iv2dv_dly >= ( vo_den_start + MAX_UZD_TO_MDOMAIN_LINE_DELAY + MAX_UZU_LINE_BUFFER_LINE_CNT ) - uzudtg_den_start
        // iv2dv_dly > 0 ( thus, make iv2dv_dly >= MAX_UZD_TO_MDOMAIN_LINE_DELAY )
        if( vo_den_start + MAX_UZU_LINE_BUFFER_LINE_CNT > uzudtg_den_start )
        {
            iv2dv_line += ( vo_den_start + MAX_UZU_LINE_BUFFER_LINE_CNT - uzudtg_den_start );
        }

        if( ( Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC ) /*|| ( Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_CAMERA )*/ || ( Get_DisplayMode_Src(SLR_MAIN_DISPLAY) ==  VSC_INPUTSRC_JPEG ) )
        {
    #ifdef VODMA1_2P_MODE
            vodma_vodma_clkgen_reg.regValue = IoReg_Read32(VODMA_VODMA_CLKGEN_reg);
            if( vodma_vodma_clkgen_reg.vodma_2p_gate_sel == 0 )    // Merlin8 VO always using 2p mode : Vodma_2p_gate_sel = 0 ( VO 1x_2p_clk = VO 1x_1p_clk )
                vodma_pll *= 2;
    #endif
            vodma_vodma_v1sgen_reg.regValue = IoReg_Read32(VODMA_VODMA_V1SGEN_reg);
            vo_htotal = vodma_vodma_v1sgen_reg.h_thr + 1;
            iv2dv_htotal = framesync_get_iv2dv_ih_slf_width(app_type, Get_DisplayMode_Src(SLR_MAIN_DISPLAY));

            // iv2dv_dly is based on 27MHz
            // vo_backporch_period = ( vo_htotal * vo_den_start ) / vodma_pll
            // iv2dv_line_period = iv2dv_htotal / iv2dv_based_clock
            // iv2dv_line = vo_backporch_period / iv2dv_line_period
            if( ( vodma_pll == 0 ) || ( iv2dv_htotal == 0 ) )
            {
                rtd_pr_vbe_err("[%s][ERROR] abnormal vodma_pll = %d, or iv2dv_htotal = %d\n", __FUNCTION__, vodma_pll, iv2dv_htotal);
            }
            else
            {
                iv2dv_line = (unsigned int)( ( (unsigned long long)vo_htotal * (unsigned long long)vo_den_start * (unsigned long long)iv2dv_based_clock ) / ( (unsigned long long)vodma_pll * (unsigned long long)iv2dv_htotal ) );

                if( iv2dv_line >= 3 )
                {
                    iv2dv_line -= 1;    // Mdisp Vsync width : 1
                    iv2dv_line -= 1;    // Buffer for clock error and iv2dv_pixel : 1
                    iv2dv_line -= 1;    // Real iv2dv line delay : iv2dv_line + 1 : 1
                }
                else
                {
                    rtd_pr_vbe_notice("[%s][WARNING] calculated iv2dv_line = %d < 3, set iv2dv_line = 0\n", __FUNCTION__, iv2dv_line);
                    iv2dv_line = 0;
                }
            }
        }
    }

    return iv2dv_line;
}

unsigned int drv_memory_tran_iv2dv_for_vdec_lowdelay(DTG_APP_TYPE app_type, unsigned int ori_iv2dv)
{
	unsigned int iv2dv = 0;
	unsigned int vo_iv2dv_ih_slf_width = 0;
	SLR_VOINFO *pVOInfo = Scaler_VOInfoPointer(Get_DisplayMode_Port(SLR_MAIN_DISPLAY));
    IV2DV_DELAY_INFO iv2dv_info = {0};

    if(app_type == DTG_DISPLAY_MONITOR)
    {
        scaler_dtg_get_iv2dv(DTG_MASTER, &iv2dv_info);

        if(pVOInfo->pixel_clk == 0)
        {
            return ori_iv2dv;
        }

        if(iv2dv_info.iv2dv_xclk_en)
        {
            vo_iv2dv_ih_slf_width = pVOInfo->h_total / (pVOInfo->pixel_clk / 27000000);
            iv2dv = ori_iv2dv * vo_iv2dv_ih_slf_width / framesync_get_iv2dv_ih_slf_width(app_type, Get_DisplayMode_Src(SLR_MAIN_DISPLAY));
            rtd_pr_vbe_emerg("[drv_memory_tran_iv2dv_for_vdec_lowdelay]iv2dv_delay base on XCLK (final) = %d\n", iv2dv);
        }
        else
        {
            iv2dv = ori_iv2dv;
        }
    }

	return iv2dv;
}

unsigned int framesync_get_iv2dv_ih_slf_width(DTG_APP_TYPE app_type, VSC_INPUT_TYPE_T inputType)
{
    unsigned int iv2dv_ih_slf_width = Scaler_DispGetInputInfo(SLR_INPUT_H_LEN);

    if(app_type == DTG_DISPLAY_MONITOR)
    {
        if((inputType == VSC_INPUTSRC_VDEC) || (inputType == VSC_INPUTSRC_CAMERA) || (inputType == VSC_INPUTSRC_JPEG))
        {
            iv2dv_ih_slf_width = Get_DISP_HORIZONTAL_TOTAL() / (Get_DISPLAY_CLOCK_TYPICAL() / 27000000);
        }
        else
        {
            iv2dv_ih_slf_width = Scaler_DispGetInputInfo(SLR_INPUT_H_LEN);
        }
    }

    if(iv2dv_ih_slf_width == 0)
    {
        rtd_pr_vbe_emerg("[%s %d]ERROR i2dv ih slf width cal error\n", __FUNCTION__, __LINE__);
        iv2dv_ih_slf_width = Get_DISP_HORIZONTAL_TOTAL() ;
    }

    return iv2dv_ih_slf_width;
}

unsigned int framesync_get_iv2dv_delay(DTG_APP_TYPE app_type, unsigned int dispTimingMode)
{
    UINT32 iv2dv_delay = 2;

    if(app_type == DTG_DISPLAY_MONITOR)
    {
        if(dispTimingMode == DISPLAY_MODE_RESET_VODMA)
        {
            if(get_HD_FHD_platform_model())
            {
                iv2dv_delay = IV2DV_2K_PANEL_MIN;
            }
            else
            {
                iv2dv_delay = 0;
            }

        }
        else if(dispTimingMode == DISPLAY_MODE_NEW_FLL_MODE)
        {
            iv2dv_delay = 2;
        }
        else if(dispTimingMode == DISPLAY_MODE_FRAME_SYNC)
        {
            if(drv_memory_get_low_delay_game_mode_dynamic())
            {

                if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC))
                {
                    iv2dv_delay = framesync_get_video_line_buffer_delay(app_type);
                }
                else
                {
                    if((get_panel_pixel_mode() > PANEL_1_PIXEL) && (get_vsc_src_is_hdmi_or_dp(SLR_MAIN_DISPLAY)) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag())&& ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DISP_LEN)== _DISP_LEN))
                    {
                        if(vbe_disp_get_VRR_device_max_framerate()> _VRR_MIN_SUPPORT_FRAMERATE)
                        {
                            iv2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();
                        }
                        else
                        {
                            iv2dv_delay = ((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN)*(_VRR_MAX_SUPPORT_FRAMERATE-_VRR_MIN_SUPPORT_FRAMERATE))/_VRR_MAX_SUPPORT_FRAMERATE);
                        }
                    }
                    else if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC || Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_CAMERA) && (!drv_memory_get_vdec_direct_low_latency_mode()))
                    {
                        iv2dv_delay = 2;
                    }
                    else
                    {
                        iv2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();

                        if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY))
                        {
                            iv2dv_delay = drv_GameMode_iv2dv_delay_compress_margin(iv2dv_delay);
                        }

                        if (iv2dv_delay > Scaler_DispGetInputInfo(SLR_INPUT_V_LEN))
                        {
                            iv2dv_delay = Scaler_DispGetInputInfo(SLR_INPUT_V_LEN) - Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE);
                        }

                        if(iv2dv_delay < 8)
                        {
                            iv2dv_delay = 8;
                            iv2dv_delay = 8;
                        }

                        if(drv_memory_get_vdec_direct_low_latency_mode())
                        {
                            iv2dv_delay = drv_memory_tran_iv2dv_for_vdec_lowdelay(app_type, iv2dv_delay);
                        }
                    }
                }
            }
            else
            {
                if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC))
                {
                    iv2dv_delay = framesync_get_video_line_buffer_delay(app_type);
                }
                else
                {
                    if(drv_memory_get_vdec_direct_low_latency_mode())
                    {

                        iv2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();

                        if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY))
                        {
                            iv2dv_delay = drv_GameMode_iv2dv_delay_compress_margin(iv2dv_delay);
                        }

                        if (iv2dv_delay > Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN))
                        {
                            iv2dv_delay = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN) - Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA_PRE);
                        }

                        if(iv2dv_delay < 8)
                        {
                            iv2dv_delay = 8;
                            iv2dv_delay = 8;
                        }

                        iv2dv_delay = drv_memory_tran_iv2dv_for_vdec_lowdelay(app_type, iv2dv_delay);
                    }
                    else if((get_panel_pixel_mode() > PANEL_1_PIXEL) && (get_vsc_src_is_hdmi_or_dp(SLR_MAIN_DISPLAY)) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag())&& ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DISP_LEN)== _DISP_LEN))
                    {
                        if(vbe_disp_get_VRR_device_max_framerate() >= V_FREQ_120000_mOFFSET)
                        {
                            iv2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();
                        }
                        else if((Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= _VRR_MIN_SUPPORT_FRAMERATE) && (Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) <= (_VRR_MAX_SUPPORT_FRAMERATE-1000)))
                        {
                            iv2dv_delay = (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN)*(_VRR_MAX_SUPPORT_FRAMERATE-Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000)))/_VRR_MAX_SUPPORT_FRAMERATE;
                        }
                        else
                        {
                            iv2dv_delay = (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN)*(_VRR_MAX_SUPPORT_FRAMERATE-_VRR_MIN_SUPPORT_FRAMERATE))/_VRR_MAX_SUPPORT_FRAMERATE;
                        }
                    }
                    else if((get_panel_pixel_mode() == PANEL_1_PIXEL) && (get_vsc_src_is_hdmi_or_dp(SLR_MAIN_DISPLAY)) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag()))
                    {
                        iv2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();
                    }
                    else
                    {
                        iv2dv_delay = 2;
                    }
                }
            }
#ifndef BUILD_QUICK_SHOW
            if(Get_Force_run_fake_scaler_State())
            {
                iv2dv_delay = 2;
            }
#endif
        }
        else
        {
            iv2dv_delay = 2;
        }
    }

    rtd_pr_vbe_notice("[%s]iv2dv_delay:%d, timing:%s, app_type:%d\n", __FUNCTION__,
        iv2dv_delay, (dispTimingMode==1)?"FSYNC":((dispTimingMode==2)?"FLL":(dispTimingMode==3)?"ResetVODMA":"FRC"), app_type);

    return iv2dv_delay;
}

void framesync_set_iv2dv_delay(DTG_APP_TYPE app_type, unsigned int dispTimingMode)
{
    UINT32 iv2dv_delay = 0;
    IV2DV_DELAY_INFO iv2dv_info = {0};
    int ret = 0;
    unsigned int iv2dv_delay_orignal = 0;

    if(app_type == DTG_DISPLAY_MONITOR)
    {
        iv2dv_delay = framesync_get_iv2dv_delay(app_type, dispTimingMode);

        ret |= scaler_dtg_set_double_buffer_disable(DOUBLE_BUFFER_D2);
        ret |= scaler_dtg_get_iv2dv(DTG_MASTER, &iv2dv_info);

        iv2dv_delay_orignal = iv2dv_info.iv2dv_line;

        if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC) || (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_CAMERA)
            || (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_JPEG))
        {
            iv2dv_info.iv2dv_xclk_en = 1;
            iv2dv_info.iv2dv_ih_slf_width = framesync_get_iv2dv_ih_slf_width(app_type, Get_DisplayMode_Src(SLR_MAIN_DISPLAY));
        }
        else
        {
            iv2dv_info.iv2dv_xclk_en = 0;
            iv2dv_info.iv2dv_ih_slf_width = framesync_get_iv2dv_ih_slf_width(app_type, Get_DisplayMode_Src(SLR_MAIN_DISPLAY));
        }

        iv2dv_info.iv2dv_pixel = 20;
        iv2dv_info.iv2dv_pixel_2 = 20;
        iv2dv_info.iv2dv_line = iv2dv_delay;
        iv2dv_info.iv2dv_line_2 = iv2dv_delay;

        ret |= scaler_dtg_set_iv2dv(DTG_MASTER, &iv2dv_info);

        if(ABS(iv2dv_delay_orignal, iv2dv_delay) > 10)
        {
            Scaler_wait_for_input_one_frame(DTG_MASTER, SLR_MAIN_DISPLAY);
        }
    }

    if(ret != 0)
    {
        rtd_pr_vbe_notice("[%s %d]Error iv2dv delay failed ret:%d\n", __FUNCTION__, __LINE__, ret);
    }
}

//#define DATAFSYNC_DEBOUNCED_FLOW

#ifdef DATAFSYNC_DEBOUNCED_FLOW
#define DATAFSYNC_FLL_DEBOUNCED_TIMES 3
#endif

/**
* drvif_framesync_detect
*  This function will Detect frame sync status
* @param <none>
*@return <result>  	 { TRUE if frame sync is ok, FALSE while PLL mislock
*/
unsigned char drvif_framesync_detect(void)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
#endif // SCALER_ERROR

#ifdef DATAFSYNC_DEBOUNCED_FLOW
	static unsigned char fsync_debounced_count = 0;
#endif

#if 0//def SMOOTH_TOGGLE_DATAFRCFS_TEST
	if(vbe_disp_get_adaptivestream_fs_mode()){
		//rtd_pr_vbe_notice("fsync under smoothtoggle mode, don't check\n");
		return TRUE;//to test rika20151224
	}

	//VDEC 4k2k do not check framesync error
	if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC) == TRUE) &&
		(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE) > DATAFS_VDEC_RUN_DATAFS_WIDTH)
		&& (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE) > DATAFS_VDEC_RUN_DATAFS_HEIGHT))
		return TRUE;
#endif
    
	//check ibuff status when full gatting enabled and forcebg disabled
	if ((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC) == TRUE) && drv_get_datafs_gatting_enable_done()
		&& !(IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg) & PPOVERLAY_Main_Display_Control_RSV_m_force_bg_mask)) {
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change		
		mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

		if(_CHANNEL1 == Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))//If Framesync case, we just check the following flow
	{
		if(mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf | mdomain_cap_ich12_ibuff_status_reg.fsync_ovf | mdomain_cap_ich12_ibuff_status_reg.fsync_udf)
		{

#ifdef DATAFSYNC_DEBOUNCED_FLOW
			{ //data fsync err check debounced
				if(fsync_debounced_count<DATAFSYNC_FLL_DEBOUNCED_TIMES){
					mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf=1;
					mdomain_cap_ich12_ibuff_status_reg.fsync_ovf=1;
					mdomain_cap_ich12_ibuff_status_reg.fsync_udf=1;
					IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg,mdomain_cap_ich12_ibuff_status_reg.regValue);
					fsync_debounced_count++;
					rtd_pr_vbe_notice("[Debounced=%d]Frame Sync check error\n", fsync_debounced_count);
					return TRUE;
				}else{
					fsync_debounced_count=0;
				}
			}
#endif
			if(fwif_color_get_osd_Outer() ==1) /* it's for playback photo patch . rord.tsao */
			{
				//main_active_h_start_end_RBUS main_active_h_start_end_reg;
				//main_active_v_start_end_RBUS main_active_v_start_end_reg;
				fwif_color_set_osd_Outer(0);
				mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf=1;
				mdomain_cap_ich12_ibuff_status_reg.fsync_ovf=1;
				mdomain_cap_ich12_ibuff_status_reg.fsync_udf=1;
				IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg,mdomain_cap_ich12_ibuff_status_reg.regValue);
				rtd_pr_vbe_err("A ch1 AAA Frame Sync check error=%d %d %d\n", mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_udf);
				mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
				if(mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf | mdomain_cap_ich12_ibuff_status_reg.fsync_ovf | mdomain_cap_ich12_ibuff_status_reg.fsync_udf)
				{
					rtd_pr_vbe_err("A ch1 Frame Sync check error=%d %d %d\n", mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_udf);
				} else {
#if 0
					// VO decompress mode don't support empty gate
					if (((rtd_inl(VODMA_DECOMP_CTRL0_reg) & _BIT0) == 0) && ((IoReg_Read32(VODMA_VODMA_CLKGEN_reg)&_BIT3) ==0)){
						IoReg_Mask32(VODMA_VODMA_CLKGEN_reg, ~(_BIT3), _BIT3);
					}
#endif
					return TRUE;
				}
			}
			rtd_pr_vbe_err("ch1 framesync buffer check error [%d %d %d], onms1=%x, onms3=%x\n", mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_udf
					,IoReg_Read32(ONMS_onms1_status_reg), IoReg_Read32(ONMS_onms3_status_reg));
			framesync_clear_ibuffer_err_status();

			// for debug
			if(IoReg_Read32(VGIP_VGIP_VBISLI_reg)&_BIT17)
			{
				while(1){
					msleep(50);

					//for debug purpose
					if(vbe_disp_dataFramesync_use_fixlastline() == 1){
						drv_framesync_on_fixlastline_phaseErr_verify();
					}

					if(!(IoReg_Read32(VGIP_VGIP_VBISLI_reg)&_BIT17))
						break;
				}
			}

			return FALSE;
		}else{
#ifdef DATAFSYNC_DEBOUNCED_FLOW
				fsync_debounced_count=0;
#endif
		}
	}
#ifdef CONFIG_DUAL_CHANNEL
		else
		{
			if(mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf_sub | mdomain_cap_ich12_ibuff_status_reg.fsync_ovf_sub | mdomain_cap_ich12_ibuff_status_reg.fsync_udf_sub)
			{
				rtd_pr_vbe_err("ch2 Frame Sync check error=%d %d %d\n", mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf_sub, mdomain_cap_ich12_ibuff_status_reg.fsync_ovf_sub, mdomain_cap_ich12_ibuff_status_reg.fsync_udf_sub);
				return FALSE;
			}
		}
#endif

#endif // SCALER_ERROR
	}


	return TRUE;
 }

void framesync_clear_ibuffer_err_status(void){
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
	rtd_pr_vbe_err("[framesync_clear_ibuffer_err_status] %d %d %d\n", mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_ovf, mdomain_cap_ich12_ibuff_status_reg.fsync_udf);
	mdomain_cap_ich12_ibuff_status_reg.regValue = 0;
	mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf=1;
	mdomain_cap_ich12_ibuff_status_reg.fsync_ovf=1;
	mdomain_cap_ich12_ibuff_status_reg.fsync_udf=1;
	IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg,mdomain_cap_ich12_ibuff_status_reg.regValue);
#endif // SCALER_ERROR

}

// CSW+  0980429
// Double DVS to solve 1080p24 & 1080p30 video flicker
/**
* framesync_syncDoubleDVSReset
*  This function will reset DVS(double display V sync) setting
* @param <pDelta>       { display }
*@return <result>  	 { Return void}
*/
#define _DOUBLE_DVS_RESET_ALL_FLL		1	//LewisLee
void framesync_syncDoubleDVSReset(void)
{
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;

	// 1. disable frame sync last line timing
	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);

	#if 0	//fix me later by ben.
	if((_DISABLE == display_timing_ctrl1_reg.disp_fix_last_line) && (_DISABLE == display_timing_ctrl1_reg.multiple_dvs))
		return;
	#endif

	// 7. disable double DVS
	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);

	//WaitFor_DEN_STOP_Done();//Remove by Will. Ben Check no need.

//	display_timing_ctrl1_reg.multiple_dvs = _DISABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);

#ifdef _DOUBLE_DVS_RESET_ALL_FLL
	//USER:LewisLee DATE:2012/12/06
	//for novatek Tcon IC, too easy have white noise
	display_timing_ctrl1_reg.disp_frc_on_fsync = _FALSE;
	display_timing_ctrl1_reg.disp_fix_last_line_new = _DISABLE;
	display_timing_ctrl1_reg.disp_fsync_en = _DISABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);

	drvif_scaler_set_display_mode_frc2fsync_by_hw(_TRUE);
#else //#ifdef _DOUBLE_DVS_RESET_ALL_FLL
	display_timing_ctrl1_reg.disp_fix_last_line_new = _DISABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);
#endif //#ifdef _DOUBLE_DVS_RESET_ALL_FLL

	// 8. Disable DMA clock sel frank@04122011 add below for DMACLK
#if 0	//mag2 un-used dclk sel, move to dpll_o
	if(Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES() == 1){
		if (GET_PANEL_DCLK_DIV() == 1){
			drvif_scalerdisplay_set_dclk_sel(DISP_DMACLK_IS_DPLL_DIV_2, DISP_DCLK_IS_DPLL);
		}
		else
			drvif_scalerdisplay_set_dclk_sel(DISP_DMACLK_IS_DPLL_DIV_2, DISP_DCLK_IS_DPLL_DIV_2);
	}
	else
		drvif_scalerdisplay_set_dclk_sel(DISP_DMACLK_IS_DPLL_DIV_2, DISP_DCLK_IS_DPLL_DIV_2);
#endif
	// 8. disable gamma table switch
//	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
//	display_timing_ctrl1_reg.gat_sw_en = _DISABLE;
//	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);
}


#define _DCLK_FREQ_SYN_SLT  0x200

//USER:LewisLee DATE:2012/06/06
//it is help Double DVS timing IVS / DVS can sync
void framesync_syncDoubleDVS_Frc_Tracking_Framesync(unsigned char display, unsigned char ucEnable)
{
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;

#ifdef DOUBLE_DVS_FRC_TRACKING_FRAMESYNC_SOL
	return;
#endif //#ifdef DOUBLE_DVS_FRC_TRACKING_FRAMESYNC_SOL

	if(SLR_SUB_DISPLAY == display)
		return;

	if(_DISABLE == modestate_decide_double_dvs_enable())
		return;

	//WaitFor_DEN_STOP_Done();//Remove by Will. Ben Chenck no need
	//WaitFor_DEN_STOP_Done();//Remove by Will. Ben Chenck no need

	if(_ENABLE == ucEnable)
	{
		display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
		display_timing_ctrl1_reg.disp_frc_on_fsync = _ENABLE;
		IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);
	}
	else// if(_DISABLE == ucEnable)
	{
		display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
		display_timing_ctrl1_reg.disp_frc_on_fsync = _DISABLE;
		IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);
	}
}

void framesync_DoubleDvs_Wait_FS_Lock(unsigned char display)
{
#if 0
//#ifdef CONFIG_ENABLE_FRC_TO_FRAMESYNC_BY_HW
	display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	UINT32 OriginalMcodeValue = 0;
	UINT32 TimeOutCounterThreshold = 0;
	UINT32 TimeOutCounter = 0;

	TimeOutCounterThreshold = Scaler_Get_DoubleDVS_FreeRun_To_FrameSync_By_HW_WaitLock_TimeOut();

	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);

	while(_FALSE == display_timing_ctrl1_reg.disp_frc_fsync)
	{
		msleep(1);
		TimeOutCounter++;

		if(TimeOutCounter > TimeOutCounterThreshold)
		{
			rtd_pr_vbe_debug("Double DVS WL wait timeout......\n");
			break;
		}

		display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	}

	rtd_pr_vbe_debug("Double DVS WL time count = %d\n",TimeOutCounter);

	if(TimeOutCounter > TimeOutCounterThreshold)
	{
		/*WaitFor_DEN_STOP();*/
		fw_scaler_dtg_double_buffer_enable(1);
		drvif_scaler_set_display_mode_frc2fsync_by_hw(_TRUE);
		fw_scaler_dtg_double_buffer_apply();
		//frank@0903 reduce time fw_scaler_dtg_double_buffer_enable(0);
		rtd_pr_vbe_debug("Double DVS WL LineBufferMode HW set frc2framesync fail, disable it\n");
	}

	return;
#endif //#ifdef CONFIG_ENABLE_FRC_TO_FRAMESYNC_BY_HW
}

// [Code Sync][LewisLee][0990305][1]
void framesync_set_enterlastline_at_frc_mode_flg(unsigned char flg)
{
	framesync_enterlastline_at_frc_mode_flg = flg;
}

unsigned char framesync_get_enterlastline_at_frc_mode_flg(void)
{
	return framesync_enterlastline_at_frc_mode_flg;
}
// [Code Sync][LewisLee][0990305][1][End]

// [Code Sync][LewisLee][0990316][3]
void drvif_measure_total_pixels(unsigned char ucEnable)
{
	ppoverlay_measure_total_pixels_result1_RBUS measure_total_pixels_result1_reg;

	if(_ENABLE == ucEnable)
	{
#ifdef _MEASURE_2FRAME_AT_ONCE
		//Measure two frame at once
		measure_total_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_total_pixels_Result1_reg);
		measure_total_pixels_result1_reg.measure_topixels_mode = _SELECT_MEASURE_MODE;
		measure_total_pixels_result1_reg.measure_topixels_2f = _ENABLE;
		IoReg_Write32(PPOVERLAY_Measure_total_pixels_Result1_reg, measure_total_pixels_result1_reg.regValue);
#else //#ifdef _MEASURE_2FRAME_AT_ONCE
		measure_total_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_total_pixels_Result1_reg);
		measure_total_pixels_result1_reg.measure_topixels_mode = _SELECT_MEASURE_MODE;
		measure_total_pixels_result1_reg.measure_topixels_2f = _DISABLE;
		IoReg_Write32(PPOVERLAY_Measure_total_pixels_Result1_reg, measure_total_pixels_result1_reg.regValue);
#endif //#ifdef _MEASURE_2FRAME_AT_ONCE

		//enable measure
		measure_total_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_total_pixels_Result1_reg);
		measure_total_pixels_result1_reg.measure_topixels_ctr = _ENABLE;
		IoReg_Write32(PPOVERLAY_Measure_total_pixels_Result1_reg, measure_total_pixels_result1_reg.regValue);
	}
	else// if(DISABLE == ucEnable)
	{
		measure_total_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_total_pixels_Result1_reg);
		measure_total_pixels_result1_reg.measure_topixels_ctr = _DISABLE;
		IoReg_Write32(PPOVERLAY_Measure_total_pixels_Result1_reg, measure_total_pixels_result1_reg.regValue);
	}
}
// [Code Sync][LewisLee][0990316][3][End]

void drvif_measure_dvtotal_last_line_pixels(unsigned char ucEnable)
{
	ppoverlay_measure_dvtotal_last_line_pixels_result1_RBUS measure_dvtotal_last_line_pixels_result1_reg;

	if(_ENABLE == ucEnable)
	{
#ifdef _MEASURE_2FRAME_AT_ONCE
		//Measure two frame at once
		measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);
		measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_mode	= _SELECT_MEASURE_MODE;
		measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_2f = _ENABLE;
		IoReg_Write32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg, measure_dvtotal_last_line_pixels_result1_reg.regValue);
#else //#ifdef _MEASURE_2FRAME_AT_ONCE
		measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);
		measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_mode	= _SELECT_MEASURE_MODE;
		measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_2f = _DISABLE;
		IoReg_Write32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg, measure_dvtotal_last_line_pixels_result1_reg.regValue);
#endif //#ifdef _MEASURE_2FRAME_AT_ONCE

		//enable measure
		measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);
		measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_ctr = _ENABLE;
		IoReg_Write32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg, measure_dvtotal_last_line_pixels_result1_reg.regValue);
	}
	else// if(DISABLE == ucEnable)
	{
		measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);
		measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_ctr = _DISABLE;
		IoReg_Write32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg, measure_dvtotal_last_line_pixels_result1_reg.regValue);
	}
}

//USER:LewisLee DATE:2012/10/22
//fix FLL mode, DV total over range, let Vfreq change
void framesync_lastlinefinetune_at_frc_mode_dymanic_adjust_dhtotal(FLL_ADJUST_DVTOTAL_STATE state)
{
#ifdef CONFIG_ENABLE_FLL_DYNAMIC_ADJUST_DHTOTAL
	ppoverlay_dh_den_start_end_RBUS dh_den_start_end_reg;
	UINT32 DhTotalMin = 0;

	switch(state)
	{
		case FLL_DYMANIC_ADJUST_DHTOTAL_TO_MAX:
			Scaler_Set_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_CurrentValue(Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_MaxValue());
			scalerdisplay_set_dh_total(_TRUE);
			rtd_pr_vbe_debug("FLL_DYMANIC_ADJUST_DHTOTAL_TO_MAX : %x\n", Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_MaxValue());
		break;
		case FLL_DYMANIC_ADJUST_DHTOTAL_TO_MIN:
			dh_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_DH_DEN_Start_End_reg);

			if(Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_MinValue() >=
				(dh_den_start_end_reg.dh_den_end +Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_FrontPorchOffset()))
			{
				//Safe Region
				DhTotalMin = Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_MinValue();
			}
			else
			{
				DhTotalMin = (dh_den_start_end_reg.dh_den_end +Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_FrontPorchOffset());
			}

			Scaler_Set_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_CurrentValue(DhTotalMin);

			scalerdisplay_set_dh_total(_TRUE);
			rtd_pr_vbe_debug("FLL_DYMANIC_ADJUST_DHTOTAL_TO_MIN : %x, DhTotalMin : %x\n",
				Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_MinValue(), DhTotalMin);
		break;

		case FLL_DYMANIC_ADJUST_DHTOTAL_NONE:
		default:
			Scaler_Set_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_CurrentValue(0);
		break;
	}
#endif //#ifdef CONFIG_ENABLE_FLL_DYNAMIC_ADJUST_DHTOTAL
}

//#define XTAL_FREQ 27000000
//#define IVS_DVS_THRESHOLD 0x100
//#define IVS_XTAL_CNT  1
//#define IVS_XTAL_CNT2 2
//#define DVS_XTAL_CNT  3

void framesync_wait_vdc_stable(void)
{
	UINT8 cnt=0;
	unsigned char timeout = 10;
	unsigned short VHalfTotal = 0;
	unsigned char ucNoSignal = 0, ucVState = 0, ucVHalfTotalRet = 0;
	unsigned char mode= Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR);

	if((Scaler_InputSrcGetMainChType() == _SRC_TV) || (Scaler_InputSrcGetMainChType() == _SRC_CVBS))
	{
		down(get_avd_power_semaphore());
		if(get_AVD_Global_Status() == SRC_CONNECT_DONE)
		{
			ucNoSignal = drvif_video_status_reg(VDC_no_signal);
			ucVState = drvif_module_vdc_Read_Vstate();
		}
		up(get_avd_power_semaphore());

		//if(drvif_video_status_reg(VDC_no_signal))
		if(ucNoSignal)
		{
			rtd_pr_vbe_debug("VD no signal \n");
			return;
		}

		//while((drvif_module_vdc_Read_Vstate() != 3) && (++cnt <10)){
		while((ucVState != 3) && (++cnt <10)){

			//rtd_pr_vbe_info("###### drvif_module_vdc_Read_Vstate:%d , cnt (%d) ######\n", drvif_module_vdc_Read_Vstate(), cnt);


			//  unsigned short VHalfTotal;
	      //          unsigned char mode = Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR);
	  		timeout = 10;

			while(--timeout){
		              mode = Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR);

			down(get_avd_power_semaphore());
			if(get_AVD_Global_Status() == SRC_CONNECT_DONE)
			{
				ucVHalfTotalRet = drvif_module_vdc_GetVHalfTotalLen(&VHalfTotal) ;
				ucVState = drvif_module_vdc_Read_Vstate();
			}
			up(get_avd_power_semaphore());


				//if ( drvif_module_vdc_GetVHalfTotalLen(&VHalfTotal) )
			 if (ucVHalfTotalRet)
		              {
			               if (((mode==_MODE_480I)&&((VHalfTotal==263)||(VHalfTotal==262))) ||
			               ((mode==_MODE_576I)&&((VHalfTotal==313)||(VHalfTotal==312))))
			               {
			                   //����í�w�F
	                 		     //rtd_pr_vbe_info("drvif_module_vdc_GetVHalfTotalLen : check ok!\n");
			                  return;
			               }
				}
				msleep(50);
			}

		      if(timeout == 0){
				rtd_pr_vbe_debug("[framesync_wait_vdc_stable]check error, timeout !\n");
		      }
			msleep(200);
		}
	}

}
unsigned int framesync_generate_ivscnt_for_tv_nosignal(UINT32 xtal_cnt)
{
	UINT32 result = 0;

	if(Scaler_InputSrcGetMainChType() == _SRC_TV){
/*
		if((xtal_cnt > 0x66FF3) && (xtal_cnt < 0x75B5E)){	// 56Hz ~ 64Hz
			rtd_pr_vbe_info("## TV source resotre to NTSC format ##\n");
			result = 0x0006DF91;
		}else if ((xtal_cnt > 0x7A120) && (xtal_cnt < 0x8F4CC)){	// 46Hz ~ 54Hz
			rtd_pr_vbe_info("## TV source resotre to PAL format ##\n");
			result = 0x83D60;
		}else{
			rtd_pr_vbe_info("## TV source resotre to NTSC format ##\n");
			result = 0x0006DF91;
		}
*/
		// due to ivs_cnt of atv no signal is unstable, we use off-line measure result to give a standard ivs_cnt
		// for speed up fix last line algo.
		rtd_pr_vbe_debug("## TV No Signal : cnt = %d ##\n", xtal_cnt);

		if(ABS(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000),50000) < 3000)
			result = 0x83D60;	// framerate 50Hz ivs_cnt : 27000000/50 = 0x83D60;
		else
			result = 0x6DF91;	// framerate 60Hz ivs_cnt : 27000000/60 = 0x6DF91;


	}
	return result;
}

#define TIMEOUT_COUNT 10
UINT32 framesync_new_fixlastline_get_stable_xtal_cnt(UINT8 type)
{
	UINT8 cnt = 0;
	UINT8 loop_timeout = TIMEOUT_COUNT;
	UINT32 xtal_cnt = 0;
	static UINT32 xtal_cnt_previous=0;
	UINT32 xtal_cnt_avg = 0;

	//rtd_pr_vbe_info("initial 0xb801975c[1:0] : %x \n", (vdcf_inl(0xb801975c)&0x3));
	//rtd_pr_vbe_info(" 0xb8019008:%x  \n", vdcf_inl(0xb8019008));
	IoReg_Mask32(PPOVERLAY_IVS_cnt_1_reg, ~(_BIT27), _BIT27);

	do
	{
		if(type ==IVS_XTAL_CNT)
			 xtal_cnt = (IoReg_Read32(PPOVERLAY_IVS_cnt_1_reg) & 0xffffff);
		else if(type ==IVS_XTAL_CNT2)
			 xtal_cnt = (IoReg_Read32(PPOVERLAY_IVS_cnt_2_reg) & 0xffffff);
		else if(type ==DVS_XTAL_CNT)
			 xtal_cnt = (IoReg_Read32(PPOVERLAY_DVS_cnt_reg) & 0xffffff);

		xtal_cnt_avg += xtal_cnt;

		if(ABS(xtal_cnt_previous, xtal_cnt) < 0x0A){
			cnt++;
		}else{ //unstable
			xtal_cnt_previous = xtal_cnt;
			cnt = 0;
		}

		if(cnt >= 5){
			break;
		}

		//tv no signal, doesn't need measure.
		if((Scaler_InputSrcGetMainChType() == _SRC_TV)&&(drvif_video_status_reg(VDC_no_signal))){
			return framesync_generate_ivscnt_for_tv_nosignal(xtal_cnt);
		}

		// modify as wait ivs start, later
		msleep(20);
		//WaitFor_DEN_STOP_Done();

	}while(--loop_timeout);

	if(loop_timeout == 0){
		xtal_cnt = xtal_cnt_avg / TIMEOUT_COUNT;
		rtd_pr_vbe_debug("!!unstable!! Use average xtal_cnt : %x , type (%d)\n", xtal_cnt, type);
	}


	return xtal_cnt;
}

#ifdef NEW_FLL_METHOD
////////////////////////////////////////////////////////////////////////////////////

unsigned int framesync_new_fixlastline_check_vtotal_boundary(unsigned int vtotal);

unsigned char framesync_new_fixlastline_set_locked_threashold(unsigned short threshold_val)
{
	ppoverlay_dvs_phase_error_set_RBUS dvs_phase_error_set_reg;

	// set threa
	dvs_phase_error_set_reg.regValue = IoReg_Read32(PPOVERLAY_dvs_phase_error_set_reg);
	dvs_phase_error_set_reg.old_fll_unlocked_th = threshold_val;
	IoReg_Write32(PPOVERLAY_dvs_phase_error_set_reg, dvs_phase_error_set_reg.regValue);

	return 1;
}

unsigned char framesync_new_fixlastline_threashold_check_enable(unsigned char bEnable)
{
	measure_two_dvs_phase_error_result_RBUS measure_two_dvs_phase_error_result_reg;

	measure_two_dvs_phase_error_result_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_two_dvs_phase_error_Result_reg);
	measure_two_dvs_phase_error_result_reg.two_dvs_phase_error_cntr = bEnable;
	IoReg_Write32(PPOVERLAY_Measure_two_dvs_phase_error_Result_reg, measure_two_dvs_phase_error_result_reg.regValue);
	msleep(50);
	IoReg_Write32(PPOVERLAY_DTG_no_ie_pending_status_reg, 0x1);	//wclr pending status

	return 1;
}

unsigned char framesync_new_fixlastline_check_locked_status(void)
{
	UINT8 result = FALSE;

	result = (IoReg_Read32(PPOVERLAY_DTG_no_ie_pending_status_reg) & _BIT0)?FALSE:TRUE ;

	return result;
}

#define ERR_OVERFLOW 1
#define ERR_UNDERFLOW 2

UINT32 framesync_new_fixlastline_finetune_step_dclk_M_code(UINT8 bIncrease)
{
	unsigned int dclkVal=0;
	unsigned int M_Code=0;
	unsigned int N_Code=0;
	unsigned int dclkOffset=0;
	unsigned int temp=0;

	dclkVal = scalerdisplay_get_dclk()/10000;

	pll_disp1_RBUS pll_disp1_reg;
	pll_disp1_reg.regValue =  IoReg_Read32(SYSTEM_PLL_DISP1_VADDR);
	M_Code = pll_disp1_reg.dpll_m;
	N_Code = pll_disp1_reg.dpll_n;

	dclk_sel_RBUS dclk_sel_reg;
	dclk_sel_reg.regValue = IoReg_Read32(PPOVERLAY_DCLK_SEL_VADDR);

	rtd_pr_vbe_info("dclkVal:%d, M_Code:%x, N_Code:%x\n", dclkVal, M_Code, N_Code);

	if(bIncrease)
		M_Code++;
	else
		M_Code--;

	temp = ((32768*dclkVal*(N_Code+2)*(dclk_sel_reg.dclk_sel+1)) /(2700*(M_Code+2)));

	rtd_pr_vbe_info("temp : %x \n", temp);

	dclkOffset = (32768 - temp);

	rtd_pr_vbe_debug("+++ M_Code : %x, dclkOffset : %x \n", M_Code, dclkOffset);


	pll_disp1_reg.dpll_m = M_Code;
	IoReg_Write32(SYSTEM_PLL_DISP1_VADDR, pll_disp1_reg.regValue);
	drvif_clock_offset(dclkOffset);
	msleep(50);
}

UINT32 framesync_new_fixlastline_finetune_by_dclk_offset()
{
	unsigned char cnt = 10;
	unsigned int ivs_cnt=0;
	unsigned int dvs_cnt=0;
	static unsigned int dvs_cnt_pre=0;
	unsigned int dvs_diff=0;
	unsigned int nFactor=0;
	unsigned int dclkVal=0;
	unsigned int dclkOffset=0;
	unsigned char dclkOffsetStep = 0x8;
	pll27x_reg_sys_dclkss_RBUS dclkss_reg;
	UINT8 bSpeedUp = 1;
	UINT8 bErrorStatus = 0;


	rtd_pr_vbe_debug("enter framesync_new_fixlastline_finetune_by_dclk_offset\n");

	if(Scaler_Get_New_FLL_Alog_With_FixDclk() == TRUE){	//Fix Dclk cant dynamic tuning dclk, so skip step4
		rtd_pr_vbe_debug("Fix dclk can't tuning dclk, return.\n");
		return 0;
	}

	do{
		ivs_cnt = framesync_new_fixlastline_get_stable_xtal_cnt(IVS_XTAL_CNT);
		dvs_cnt = framesync_new_fixlastline_get_stable_xtal_cnt(DVS_XTAL_CNT);
		dclkVal = scalerdisplay_get_dclk();
		dclkss_reg.regValue = IoReg_Read32(SYSTEM_SYS_DCLKSS_VADDR);
		dclkOffset = dclkss_reg.dclk_offset_11_0;

		cnt--;

		if(modestate_decide_double_dvs_enable() == _ENABLE){
			rtd_pr_vbe_info("Double DVS enable!! ivs cnt /2 \n");
			ivs_cnt /= 2;
		}

		rtd_pr_vbe_debug("(cnt: %d) ivs_cnt = %x, dvs_cnt = %x, ABS(ivs,dvs) = %x, dclkOffset:%x, dclkVal = %d \n", cnt, ivs_cnt, dvs_cnt, ABS(ivs_cnt,dvs_cnt), dclkOffset, dclkVal);

		if(ABS(ivs_cnt, dvs_cnt) < 0x100){
			if((dvs_cnt > ivs_cnt) || (ABS(ivs_cnt, dvs_cnt) < 0x10)){
				dclkOffset -= dclkOffsetStep;	//let ivs_cnt little large than dvs_cnt
				dclkss_reg.dclk_offset_11_0 = dclkOffset;
				dclkss_reg.dclk_ss_load = 1;
				dclkss_reg.dclk_ss_en = _ENABLE;
				IoReg_Write32(SYSTEM_SYS_DCLKSS_VADDR, dclkss_reg.regValue);
			}
			rtd_pr_vbe_debug("final dclkOffset:%x \n", dclkOffset);
			break;
		}else{

			/*=== due to step a dclkoffset, we can get stable dvs cnt difference. ===*/
			/*===  we can use this rule for speed up dclk to acheive real offset  ===*/

			if(ABS(ivs_cnt, dvs_cnt) > 0x200){
				bSpeedUp = 1;
			}
			rtd_pr_vbe_info("bSpeedUp:%d \n", bSpeedUp);

			if(bSpeedUp == 1){ // for speed up
				if(dvs_cnt_pre != 0){
					dvs_diff = ABS(dvs_cnt_pre, dvs_cnt);
				}
				if(dvs_diff != 0){
					nFactor = (ABS(ivs_cnt, dvs_cnt) / dvs_diff);
				}
				dvs_cnt_pre = dvs_cnt;

				rtd_pr_vbe_info("nFactor:%d \n", nFactor);

				if(nFactor == 0){
					if(dvs_cnt > ivs_cnt){
						if(dclkOffsetStep > dclkOffset)
							bErrorStatus = ERR_UNDERFLOW;
						else
							dclkOffset -= dclkOffsetStep;
					}
					else{
						if( dclkOffset + dclkOffsetStep > 0xf00)
							bErrorStatus = ERR_OVERFLOW;
						else
							dclkOffset += dclkOffsetStep;
					}
				}else{
					if(dvs_cnt > ivs_cnt){
						if((nFactor*dclkOffsetStep) > dclkOffset)
							bErrorStatus = ERR_UNDERFLOW;
						else
							dclkOffset -= (nFactor*dclkOffsetStep);
					}
					else{
						if( dclkOffset + (nFactor*dclkOffsetStep) > 0xf00)
							bErrorStatus = ERR_OVERFLOW;
						else
							dclkOffset += (nFactor*dclkOffsetStep);
					}
					bSpeedUp = 0;
					dvs_cnt_pre = 0;
				}
			}
			else{
				if(dvs_cnt > ivs_cnt){
					if(dclkOffsetStep > dclkOffset)
						bErrorStatus = ERR_UNDERFLOW;
					else
						dclkOffset -= dclkOffsetStep;
				}
				else{
					if( dclkOffset + dclkOffsetStep > 0xf00)
						bErrorStatus = ERR_OVERFLOW;
					else
						dclkOffset += dclkOffsetStep;
				}
			}
		}

		drvif_clock_offset(dclkOffset);
		msleep(50);

		rtd_pr_vbe_info("dvs_diff : %d, nFactor : %d, dclkOffset:%x, bErrorStatus:%d \n", dvs_diff, nFactor, dclkOffset, bErrorStatus);

		if(bErrorStatus != 0){
			pll_disp1_RBUS pll_disp1_reg;

			pll_disp1_reg.regValue =  IoReg_Read32(SYSTEM_PLL_DISP1_VADDR);

			if(bErrorStatus == ERR_UNDERFLOW)
				framesync_new_fixlastline_finetune_step_dclk_M_code(1);
			else
				framesync_new_fixlastline_finetune_step_dclk_M_code(0);

			bSpeedUp = 1;
			bErrorStatus = 0;
			nFactor = 0;
			dvs_diff = 0;
			dvs_cnt_pre = 0;
		}

	}while((ABS(ivs_cnt, dvs_cnt) >= 0x100) && (cnt != 0));


	if(cnt== 0)
		rtd_pr_vbe_debug("[%s][line:%d] !!!!!!! measure error !!!!!!!!! \n", __FUNCTION__, __LINE__);
	else
		rtd_pr_vbe_debug("[%s][line:%d] !!!!!!! measure done !!!!!!!!! \n", __FUNCTION__, __LINE__);

	return 1;
}

unsigned int framesync_new_fixlastline_check_vtotal_boundary(unsigned int vtotal)
{
//	if(_DISABLE == Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Enable_Flag())
		return vtotal;
#if 0   //mark scalerlib control by qing_liu
	unsigned char result = TRUE;
	UINT32 DVTotal = vtotal;

	if(DVTotal > Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Upper()){
		DVTotal = Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Upper();
		result = FALSE;
	}
	else if(DVTotal < Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Lower()){
		DVTotal = Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Lower();
		result = FALSE;
	}

	if(result == FALSE){
		rtd_pr_vbe_info(" DVTotal, Over Spec, set to panel spec limit !!! \n");
		rtd_pr_vbe_info(" Limit_Upper()=%x",Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Upper());
		rtd_pr_vbe_info(" Limit_Lower()=%x",Scaler_Get_EnterFRC_LastLine_FixDclk_DVTotoal_Limit_Lower());
		rtd_pr_vbe_info(" DVTotal=%x \n",DVTotal);
	}
	return DVTotal;
#endif
}

#endif
///////////////////////////////////////////////////////////////////////////////////
//USER:LewisLee DATE:2011/04/22
//Fix Dclk, only Adjust DV total
#define FLL_NEW_MODE_SYNTHESISN 1
unsigned char framesync_lastlinefinetune_at_new_mode(void)
{
	unsigned char result = TRUE;
	unsigned int multiplier, y, htotal, vtotal;
	ppoverlay_fix_last_line_systhesisn_ratio_RBUS fix_last_line_systhesisn_ratio_reg;
	pll27x_reg_dpll_in_fsync_tracking_ctrl4_RBUS dpll_in_fsync_tracking_ctrl4_reg;
	ppoverlay_fix_last_line_mode_lock_status_RBUS ppoverlay_fix_last_line_mode_lock_status_reg;
//	dpll_in_fsync_tracking_ctrl_RBUS dpll_in_fsync_tracking_ctrl_reg;
	pll27x_reg_sys_pll_disp1_RBUS pll_disp1_reg;
	pll27x_reg_pll_ssc0_RBUS pll_ssc0_reg;
	pll27x_reg_sys_dclkss_RBUS sys_dclkss_reg;
	ppoverlay_dh_total_last_line_length_RBUS ppoverlay_dh_total_last_line_length_reg;
	ppoverlay_dv_total_RBUS ppoverlay_dv_total_reg;
	unsigned char fll_newmode_synt =0;
	ppoverlay_main_display_control_rsv_RBUS ppoverlay_main_display_control_rsv_reg;

	//framesync_set_iv2dv_delay();

	if ((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI)) {
		fll_newmode_synt = 5;
	}else{
		fll_newmode_synt = FLL_NEW_MODE_SYNTHESISN;
	}

	//FLL reset flow for merlin2 @Crixus 20160909
	ppoverlay_fix_last_line_mode_lock_status_reg.regValue = IoReg_Read32(PPOVERLAY_fix_last_line_mode_lock_status_reg);
	//ppoverlay_fix_last_line_mode_lock_status_reg.lock_th = 0xafc;//modify the fll threshold, ((xclk / 12) *n%)<<4
	ppoverlay_fix_last_line_mode_lock_status_reg.lock_th = 0xfff;//modify the fll threshold, ((xclk / 12) *n%)<<4
	ppoverlay_fix_last_line_mode_lock_status_reg.t_s = 1;//reset fll lock status
	IoReg_Write32(PPOVERLAY_fix_last_line_mode_lock_status_reg, ppoverlay_fix_last_line_mode_lock_status_reg.regValue);

	ppoverlay_dh_total_last_line_length_reg.regValue = IoReg_Read32(PPOVERLAY_DH_Total_Last_Line_Length_reg);
	ppoverlay_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
	htotal = ppoverlay_dh_total_last_line_length_reg.dh_total+1;
	vtotal = ppoverlay_dv_total_reg.dv_total+1;
	fix_last_line_systhesisn_ratio_reg.regValue = IoReg_Read32(PPOVERLAY_fix_last_line_systhesisN_ratio_reg);
	fix_last_line_systhesisn_ratio_reg.max_multiple_synthesisn = 6;
	fix_last_line_systhesisn_ratio_reg.synthesisn = fll_newmode_synt;
	fix_last_line_systhesisn_ratio_reg.synthesisn_ratio =
		(((125*htotal*vtotal)/100)*fll_newmode_synt)/40960;
	rtd_pr_vbe_debug("[%s]synthesisn_ratio = %x\n", __FUNCTION__, fix_last_line_systhesisn_ratio_reg.synthesisn_ratio);
	IoReg_Write32(PPOVERLAY_fix_last_line_systhesisN_ratio_reg, fix_last_line_systhesisn_ratio_reg.regValue);
	pll_disp1_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_PLL_DISP1_reg);
	pll_ssc0_reg.regValue = IoReg_Read32(PLL27X_REG_PLL_SSC0_reg);
	y = (2048*(pll_disp1_reg.dpll_m+3))+pll_ssc0_reg.fcode_t_ssc;
	multiplier = (y*4096)/4095;
	rtd_pr_vbe_debug("[%s]multiplier= %x\n", __FUNCTION__, multiplier);
	dpll_in_fsync_tracking_ctrl4_reg.regValue = IoReg_Read32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL4_reg);
	dpll_in_fsync_tracking_ctrl4_reg.new_fll_tracking_mul = multiplier;
	IoReg_Write32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL4_reg, dpll_in_fsync_tracking_ctrl4_reg.regValue);
	sys_dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_reg);
	sys_dclkss_reg.dclk_ss_en = 1;
	sys_dclkss_reg.dclk_ss_load= 0;
	IoReg_Write32(PLL27X_REG_SYS_DCLKSS_reg, sys_dclkss_reg.regValue);
	sys_dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_reg);
	sys_dclkss_reg.dclk_ss_en = 1;
	sys_dclkss_reg.dclk_ss_load= 1;
	IoReg_Write32(PLL27X_REG_SYS_DCLKSS_reg, sys_dclkss_reg.regValue);
	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	ppoverlay_main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
	ppoverlay_main_display_control_rsv_reg.main_wd_to_free_run=0;
	IoReg_Write32(PPOVERLAY_Main_Display_Control_RSV_reg, ppoverlay_main_display_control_rsv_reg.regValue);
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	return result;
}

//USER:LewisLee DATE:2010/09/02
//check framesync fix last line state
unsigned char framesync_sync_fixlastline_mode(void)
{
//mark scalerlib control by qing_liu
//	if(_FALSE == Scaler_Enter_FramesyncOpenFixLastLine_Condition())
		return _FALSE;
#if 0
	display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	if(_ENABLE == display_timing_ctrl1_reg.disp_fix_last_line)
		return _TRUE;

	return _FALSE;
#endif
}

/**
* framesync_testsync
*  This function will Apply Dclk frequency and get the overflow/underflow information
* @param <usOffset>  {Dclk offset}
* @param <retValue>  {under/over flow status}
*@return <result>  	 {Return FALSE if there is no timing change}
*/
static unsigned char framesync_testsync(unsigned short usOffset, unsigned char* retValue)
{
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;

	// Apply Dclk frequency setting
	drvif_clock_offset(usOffset);

	//IoReg_SetBits(SCALEDOWN_ICH1_IBUFF_STATUS_VADDR,  _BIT0|_BIT1|_BIT2);//write status

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
	mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf = TRUE;
	mdomain_cap_ich12_ibuff_status_reg.fsync_ovf = TRUE;
	mdomain_cap_ich12_ibuff_status_reg.fsync_udf = TRUE;
	IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg, mdomain_cap_ich12_ibuff_status_reg.regValue);

	//frank@09032010 wait one input frame
	WaitFor_IVS1_Done();
	WaitFor_IVS1_Done();
	//msleep(40);
 	// Save underflow/overflow information into pData[0]
	//*retValue = IoReg_ReadByte0(SCALEDOWN_ICH1_IBUFF_STATUS_VADDR);
	//*retValue = ((*retValue) & ( _BIT1 | _BIT0)) ;//frank@0625 add BIT2 to check frame sync error

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	if((TRUE == mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf) || (TRUE == mdomain_cap_ich12_ibuff_status_reg.fsync_ovf))
		*retValue = (*retValue) &  _BIT1;

	if(TRUE == mdomain_cap_ich12_ibuff_status_reg.fsync_udf)
		*retValue = (*retValue) &  _BIT0;
#endif // SCALER_ERROR

	return FALSE;                                          // Success
 }

/**
* framesync_spreadspectrumfinetune
*  This function will Finetune Dclk offset for spread spectrum
* @param <pDelta>       { Dclk offset}
*@return <result>  	 { Return FALSE if there is no timing change}
*/
#if 0
unsigned char Scaler_GetSpreadSpectrum(unsigned char src_idx)
{
	return fwif_color_get_spread_spectrum(src_idx);
}
#endif
static unsigned char framesync_spreadspectrumfinetune(unsigned short *pDelta)
{
	unsigned char Data = 0;
//fixme:vip
//	drv_adjust_spreadspectrumrange(Scaler_GetSpreadSpectrum(SLR_MAIN_DISPLAY));
	if(framesync_testsync(*pDelta, &Data)) {
		return 0;
	}
	if(Data & 0x03)
		return 2;

	return 0;
 }

int error_flag = 0;

/**
* drvif_framesync_offlinemeasureendevent
*  This function will Measure start/end polling event
* @param <None>
*@return <result>  	 {Return TRUE if measure finished}
*/
unsigned char drvif_framesync_offlinemeasureendevent(void)
{
	offms_sp_ms3stus_RBUS offms_sp_ms3stus_reg;
	offms_sp_ms3ctrl_RBUS offms_sp_ms3ctrl_reg;

	offms_sp_ms3stus_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Stus_reg);
	offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);

	if(_DISABLE == offms_sp_ms3stus_reg.off_ms_now) //measure ack finish
	{
		offms_sp_ms3stus_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Stus_reg);

		if(_OFF_MEAS_TO_SEL == _OFF_MEAS_TO_SEL_76MS)
		{
			if(offms_sp_ms3stus_reg.off_vs_per_to_long)
				goto RESTART_FRAMESYNC_OFFLINE_MEASURE;
			else
				return TRUE;  // off-line measure is ready
		}
		else// if(_OFF_MEAS_TO_SEL == _OFF_MEAS_TO_SEL_38MS)
		{
			if(offms_sp_ms3stus_reg.off_vs_per_to)
				goto RESTART_FRAMESYNC_OFFLINE_MEASURE;
			else
				return TRUE;  // off-line measure is ready
		}
	}

	return FALSE; //measure not finish

	RESTART_FRAMESYNC_OFFLINE_MEASURE:
	//have error status,restart measure
	error_flag++;
	rtd_pr_vbe_debug("ERROR measure times %d\n",error_flag);
	drv_adjust_syncprocessormeasurestart();
	return FALSE;
}

/**
* framesync_fast_get_sync_point
*  This function will Calculate the suitable framesync point
* @param <None>
*@return <result>  	 {Return Dclk offset}
*/
static unsigned short framesync_fast_get_sync_point(void)
{
	unsigned short spVal = 0;
#if 0 // unused
//CSW+ 0961214 new code for frame sync by tyan
	unsigned long long offset = 0;
	unsigned long aclkcnt = 0;

	unsigned long long faclkcnt;
	unsigned long long ulFreq;
	unsigned char div = 1, cpc, cnt = 0;
	unsigned short nMCode, nNCode;
	unsigned char timeoutcnt = 50;	// Tracking timeout 60ms
	unsigned int ulResult;
	unsigned long long ulparam1 =0;
	unsigned long ulparam2 =0;

	//msleep(1000);
	//frank@0109 odd field delay even field by 1 progressive line
	/* don't cut lines of di */
	IoReg_ClearBits(DI_IM_DI_MPEG_SEQUENCE_APP_reg, _BIT8);

	// Measure input active region by xtal clk & Enable active region measure
	IoReg_Mask32(OFFMS_SP_MS3EnAtvCtrl_reg, ~(_BIT6 | _BIT7), _BIT6);
	//frank@0630 mark IoReg_WriteByte1(SYNCPROCESSOR_SP1_CTRL_VADDR, 1);

	// Enable DPLL Frequence tuning
	IoReg_SetBits(PLL27X_REG_SYS_PLL_DISP2_reg, _BIT20 | _BIT21);
	//Enable DDS Spread Spectrum Output Function
	IoReg_SetBits(PLL27X_REG_SYS_DCLKSS_reg, _BIT0);

	if(Scaler_InputSrcGetMainChType()==_SRC_VO){
		SLR_VOINFO* pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
		//if ( Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_THRIP) )
		if(TRUE == fwif_scaler_decide_display_cut4line(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY))){
			ulparam1 = (unsigned long long)27000000*(unsigned long)(Scaler_DispGetInputInfo(SLR_INPUT_H_LEN)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)-3)+Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID));
    		ulparam2 = (unsigned long)pVOInfo->pixel_clk;
			//faclkcnt = 27000000*(unsigned long)(Scaler_DispGetInputInfo(SLR_INPUT_H_LEN)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)-3)+Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID))/(unsigned long)pVOInfo->pixel_clk;
			do_div(ulparam1, ulparam2);
			faclkcnt = ulparam1;
			// DI will remove 4 lines per frame ( 2 lines per field ), so we have to -2 more lines in active length.
		}else{
			ulparam1 = 27000000ULL*(unsigned long)(Scaler_DispGetInputInfo(SLR_INPUT_H_LEN)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)-1)+Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID));
    		ulparam2 = (unsigned long)pVOInfo->pixel_clk;
			//faclkcnt = 27000000*(unsigned long)(Scaler_DispGetInputInfo(SLR_INPUT_H_LEN)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)-1)+Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID))/(unsigned long)pVOInfo->pixel_clk;
			do_div(ulparam1, ulparam2);
			faclkcnt = ulparam1;
		}
		aclkcnt = faclkcnt;
	}

#ifdef CONFIG_INPUT_SOURCE_IPG
	else if(Scaler_InputSrcGetMainChType()==_SRC_IPG){
		//if ( Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_THRIP) ){
		if(TRUE == fwif_scaler_decide_display_cut4line(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)){
			faclkcnt = 27000000.0*(FLOAT64)(Scaler_DispGetInputInfo(SLR_INPUT_H_LEN)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)-3)+Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID))/
				(double)(13487760); //h total * v total * frame rate
			// DI will remove 4 lines per frame ( 2 lines per field ), so we have to -2 more lines in active length.
		}
		else{
				faclkcnt = 27000000.0*(double)(Scaler_DispGetInputInfo(SLR_INPUT_H_LEN)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)-1)+Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID))/
				(double)(13487760); //h total * v total * frame rate
		}
		aclkcnt = (unsigned long)faclkcnt;
	}
#endif
	else{
	    	// Off-line measure start
		drv_adjust_syncprocessormeasurestart();
		error_flag = 0;
		do {
	   	msleep(20);

	   	if(drvif_framesync_offlinemeasureendevent())
		 		break;
		}while(--timeoutcnt);
		//error happened
		if(timeoutcnt == 0){
			offset = 0xfff;
			rtd_pr_vbe_debug("Fast frame sync OffLineMeasure ERROR : off-line measure status polling time-out!!\n");
			IoReg_ClearBits(OFFMS_SP_MS3EnAtvCtrl_reg, _BIT6);
			return offset;
		}

		IoReg_SetBits(OFFMS_SP_MS3Ctrl_reg, _BIT12);  // off-line measure pop-up
		timeoutcnt = 3;
		do {
			msleep(10);
			ulResult = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
			if(!(ulResult & _BIT12)) // polling start bit
				break;
		} while (--timeoutcnt);
		//error happened
		if (timeoutcnt == 0x00) {
			rtd_pr_vbe_debug("measure aclkcount popup ERROR1\n");
			offset = 0xfff;
			return offset;
		}
		aclkcnt = IoReg_Read32(OFFMS_SP_MS3EnAtvCtrl_reg) & 0x00ffffff;
	}
	rtd_pr_vbe_debug("\n aclkcnt =%x  _DISP_LEN = %x !!\n", (unsigned int)aclkcnt,(unsigned int)_DISP_LEN);//test by tyan

	//rtd_pr_vbe_info("MeasureRead DPLL_M=%x, div=%x in FastDo\n", mcode, div);
	if((aclkcnt == 0)||(_DISP_LEN ==0)){
		offset = 0xfff;
		rtd_pr_vbe_debug("\n aclkcnt =%x  _DISP_LEN = %x !!\n", (unsigned int)aclkcnt, (unsigned int)_DISP_LEN);
		rtd_pr_vbe_debug("Fast frame sync OffLineMeasure ERROR : off-line measure status polling time-out!!\n");
		return offset;
	}

	IoReg_ClearBits(OFFMS_SP_MS3Ctrl_reg, _BIT9|_BIT10);  // clear off-line measureing flag

	rtd_pr_vbe_debug("\n\n  info->IPV_ACT_LEN=%x  info->IPH_ACT_WID = %x !!\n", Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN),Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID));
	rtd_pr_vbe_debug("disp_h_total=%d, disp_len=%d, disp_wid=%d\n",Get_DISP_HORIZONTAL_TOTAL(), _DISP_LEN, _DISP_WID);

//	ulFreq = (unsigned int)(((unsigned long long)(Get_DISP_HORIZONTAL_TOTAL()+1)*(_DISP_LEN-1)+_DISP_WID)*27000000/aclkcnt);
	//USER:LewisLee DATE:2010/09/15
	//we should not add 1 for DHtotal
	ulparam1 = (((unsigned long long)Get_DISP_HORIZONTAL_TOTAL())*(_DISP_LEN-1)+_DISP_WID)*27000000;
    ulparam2 = aclkcnt;
	//ulFreq = (unsigned int)(((unsigned long long)(Get_DISP_HORIZONTAL_TOTAL())*(_DISP_LEN-1)+_DISP_WID)*27000000/aclkcnt);
	do_div(ulparam1, ulparam2);
	ulFreq = ulparam1;
	rtd_pr_vbe_debug("\n ulFreq (dclk) = %lld Hz, max dclk = %d Hz", ulFreq, Get_DISPLAY_CLOCK_MAX()*1000000 );

	if ( ulFreq > (unsigned long long)Get_DISPLAY_CLOCK_MAX()*1000000 ) {
		rtd_pr_vbe_debug("\n !!!!! DClk > Panel Max DClk !!!!!" );
	}

	if (Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() == 0)
	{
		//frank@1231 avoid DCLK set uncorrect value let D-domain fail
		if((ulFreq > (unsigned long long)(Get_DISPLAY_CLOCK_MAX()*1000000ULL*3/2))||(ulFreq < (unsigned long long)(Get_DISPLAY_CLOCK_MAX()*1000000ULL*1/2))){
			offset = 0xfff;
			return offset;
		}
	}

	if((ulFreq<=234000000) && (ulFreq>=46800000))
		div=1;
	else if((ulFreq<=117000000) && (ulFreq>=23400000))
		div=2;
	else if((ulFreq<=58000000) && (ulFreq>=11700000))
		div=4;
	else if((ulFreq<=29200000) && (ulFreq>=5850000))
		div=8;

	rtd_pr_vbe_debug("\n DIV = %x",div);

	//Ncode/Mcode=2*16*Div*(DHtotal*DVactive-DH_porch)/(15*aclkcnt)
	//USER:LewisLee DATE:2010/09/15
	//we should not add 1 for DHtotal
	ulparam1 = (unsigned long long)2*16*div*1000*((Get_DISP_HORIZONTAL_TOTAL())*(_DISP_LEN-1)+_DISP_WID);
    ulparam2 = (15*aclkcnt);
//	offset = (unsigned long long)2*16*div*1000*((Get_DISP_HORIZONTAL_TOTAL())*(_DISP_LEN-1)+_DISP_WID)/(15*aclkcnt);
	do_div(ulparam1, ulparam2);
	offset = ulparam1;
	rtd_pr_vbe_debug("\n nMCode /nNCode= %x", (unsigned int)offset);
	rtd_pr_vbe_debug("\n offset = %x",(unsigned int)offset);

	for(cnt=3;cnt<=15;cnt++){
		nNCode=cnt;
		ulparam1 = nNCode*2*offset;
    	ulparam2 = 1000;
		//nMCode=nNCode*2*offset/1000;
		do_div(ulparam1, ulparam2);
		nMCode = ulparam1;
		nMCode = ((nMCode >> 1) + (nMCode & 0x01));
		if ((nMCode<=255)&&(nMCode>=3))
			break;
	}

	cpc = drvif_clock_cal_cpc(nMCode,nNCode);

	rtd_pr_vbe_debug("div=%d,n=%d,m=%d,aclkcnt=%d\n",div,nNCode,nMCode,(unsigned int)aclkcnt);
	drvif_clock_set_pll(nMCode, nNCode, div, cpc);
	rtd_pr_vbe_debug("Set Pll Reg = %x\n", IoReg_Read32(PLL27X_REG_SYS_PLL_DISP2_reg));
#if 0 //frank@0225 mark below code
	//DPLL WD Status register is Abnormal
	if(IoReg_ReadByte0(SYSTEM_PLL_WDOUT_VADDR)  & _BIT5){
		IoReg_ClearBits(SYSTEM_PLL_DISP2_VADDR, _BIT0);//DPll power down
		msleep(5);//frank@0430 Change busy delay to save time
		IoReg_SetBits(SYSTEM_PLL_DISP2_VADDR, _BIT0);//DPll power on
	}
#endif
	ulparam1 = 32768-(unsigned long long)32768*2*div*nNCode*((Get_DISP_HORIZONTAL_TOTAL())*(_DISP_LEN-1)+_DISP_WID);
    ulparam2 = (nMCode*aclkcnt);
	//offset =(unsigned long long)32768-(unsigned long long)32768*2*div*nNCode*((Get_DISP_HORIZONTAL_TOTAL())*(_DISP_LEN-1)+_DISP_WID)/(nMCode*aclkcnt);
	do_div(ulparam1, ulparam2);
	offset = ulparam1;

	rtd_pr_vbe_debug("\n DClock offset1 = %x\n",(unsigned short) offset);
	drvif_clock_offset((unsigned short) offset);

	rtd_pr_vbe_debug("offset = %x in FastDo\n", (unsigned short) offset);

	IoReg_ClearBits(OFFMS_SP_MS3EnAtvCtrl_reg, _BIT6);

	return (unsigned short)offset;
#endif
	return spVal;
}

//lewis 20090812, set syncprocessor off line measure source,
//to prevent PIP mode swap/on/off, the off line measure source switch to other path
void drv_framesync_set_offline_ms_src(void)
{
	offms_sp_ms3ctrl_RBUS offms_sp_ms3ctrl_reg;
	switch(Scaler_InputSrcGetMainChType())
	{
#ifdef CONFIG_SUPPORT_SRC_ADC
		case _SRC_YPBPR:
//		case _SRC_COMPONENT:
			offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
			offms_sp_ms3ctrl_reg.off_meas_source = _OFFLINE_MS_SRC_DEHV1;
			offms_sp_ms3ctrl_reg.off_sycms_mode = ANALOG_MODE_MEASUREMENT;
			IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
		break;
#ifdef CONFIG_SUPPORT_SRC_VGA
		case _SRC_VGA:
			if(SEPARATE_MODE == VGA_Detect_SyncTypeMode())
			{
				offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
				offms_sp_ms3ctrl_reg.off_meas_source = _OFFLINE_MS_SRC_RAWHV1;
				offms_sp_ms3ctrl_reg.off_sycms_mode = ANALOG_MODE_MEASUREMENT;
				IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
			}
			else
			{
				offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
				offms_sp_ms3ctrl_reg.off_meas_source = _OFFLINE_MS_SRC_DEHV1;
				offms_sp_ms3ctrl_reg.off_sycms_mode = ANALOG_MODE_MEASUREMENT;
				IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
			}
		break;
#endif//#ifdef CONFIG_SUPPORT_SRC_VGA
#endif//#ifdef CONFIG_SUPPORT_SRC_ADC
		case _SRC_HDMI:
			offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
			offms_sp_ms3ctrl_reg.off_meas_source = _OFFLINE_MS_SRC_HDMI;
			offms_sp_ms3ctrl_reg.off_sycms_mode = DIGITAL_MODE_MEASUREMENT;
			IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
		break;

		case _SRC_TV:
		case _SRC_CVBS:
		case _SRC_SV:
		case _SRC_SCART:
		case _SRC_SCART_RGB:
			offms_sp_ms3ctrl_reg.regValue = IoReg_Read32(OFFMS_SP_MS3Ctrl_reg);
			offms_sp_ms3ctrl_reg.off_meas_source = _OFFLINE_MS_SRC_VDC;
			offms_sp_ms3ctrl_reg.off_sycms_mode = ANALOG_MODE_MEASUREMENT;
			IoReg_Write32(OFFMS_SP_MS3Ctrl_reg, offms_sp_ms3ctrl_reg.regValue);
		break;

		default:
			//do nothing
		break;
	}
}




//--------------------------------------------------
// Description  : Adjust front porch size to meet frame sync
// Input Value  : None
// Output Value : Return 0 if sucess
//--------------------------------------------------
/**
* drvif_framesync_front_porch_check
*  This function will check and mpdify front porch
* @param <None>
*@return <None>
*/
void drvif_framesync_front_porch_check(void)
{
// [Code Sync][LewisLee][0990712][1]
	//USER:LewisLee DATE:2010/07/12
	//we need let front porch large than 4, or change IVS2DVS delay
	unsigned short _VGIP_V_Porch_temp = 0, porch_offset = 0;
	vgip_vgip_chn1_act_vsta_length_RBUS vgip_chn1_act_vsta_length_reg;
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
//	vgip_chn2_act_vsta_length_RBUS vgip_chn2_act_vsta_length_reg;
#endif
	unsigned short ivst;

	if (_FALSE == Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC))
		return;

	_VGIP_V_Porch_temp = Scaler_DispGetInputInfo(SLR_INPUT_V_LEN) - Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);

	ivst = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA);

	if(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_THRIP))
		_VGIP_V_Porch_temp = _VGIP_V_Porch_temp<<1;


	if(_VGIP_V_Porch_temp >= _PROGRAM_VDELAY)
	{
		if((_VGIP_V_Porch_temp - ivst) < _PROGRAM_VDELAY)
		{
			porch_offset = _PROGRAM_VDELAY - (_VGIP_V_Porch_temp - ivst);

			if(ivst > porch_offset)
				ivst -= porch_offset;

			rtd_pr_vbe_debug("framesync_update_ivs_position, change Vstart to :%x\n", ivst);

			if(_CHANNEL1 == Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))
			{
				vgip_chn1_act_vsta_length_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg);
				vgip_chn1_act_vsta_length_reg.ch1_iv_act_sta = ivst;
				IoReg_Write32(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg, vgip_chn1_act_vsta_length_reg.regValue);
			}

			Scaler_DispSetInputInfo(SLR_INPUT_IPV_ACT_STA,ivst);

		}
	}
	else
	{
		rtd_pr_vbe_debug("framesync_update_ivs_position, porch <= 5\n");
	}
// [Code Sync][LewisLee][0990712][1][End]
}

unsigned int VODMA_get_datafs_increased_clock(unsigned int orig_ratio_n_off)
{
	unsigned int ratio_n_off = 0;
	unsigned int vodmapll = 0;
	unsigned int pixel_clock=0;

	vodmapll = VO_Get_vodmapll();

    if ((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_VDEC) &&(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_CAMERA)&& (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE) > DATAFS_VDEC_RUN_DATAFS_WIDTH) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE) > DATAFS_VDEC_RUN_DATAFS_HEIGHT)){
			ratio_n_off = orig_ratio_n_off;
			pixel_clock = ((vodmapll/64) * (64 - ratio_n_off))/1000000;
			if((pixel_clock >= 230) && (pixel_clock < 250)){	//24Hz
				ratio_n_off -= 6;
			}
			else if((pixel_clock >= 250) && (pixel_clock < 300)){//25Hz
				ratio_n_off -= 7;
			}
			else if(pixel_clock >= 470)
			{
				ratio_n_off = 0;
			}
			else{
				if(ratio_n_off > 3)
					ratio_n_off -= 3;
			}
	}
	else{
		//2k1k use framesync with gatting flow
		ratio_n_off = orig_ratio_n_off;
		if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC || Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_CAMERA){
			if(ratio_n_off > 11)
			{
				if( ( ( Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000) >= V_FREQ_24000_mOFFSET ) && ( Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000) < V_FREQ_24500 ) ) 
||
					( ( Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000) >= V_FREQ_24500 ) && ( Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000) < V_FREQ_25000_pOFFSET ) ) ||
					( ( Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000) >= V_FREQ_30000_mOFFSET ) && ( Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000) < V_FREQ_30000_pOFFSET ) ) )
				{
					// modify 24/25/30hz no increase gating clock
					ratio_n_off = orig_ratio_n_off;
				}else{
					ratio_n_off -= 5;
				}
			}else
				ratio_n_off = 0;
		}
		else{
			ratio_n_off = orig_ratio_n_off;
			pixel_clock = ((vodmapll/64) * (64 - ratio_n_off))/1000000;
			if((pixel_clock >= 230) && (pixel_clock < 250)){	//24Hz
				ratio_n_off -= 6;
			}
			else if((pixel_clock >= 250) && (pixel_clock < 300)){//25Hz
				ratio_n_off -= 7;
			}
			else if(pixel_clock >= 470)
			{
				ratio_n_off = 0;
			}
			else{
				if(ratio_n_off > 3)
					ratio_n_off -= 3;
			}
		}

		rtd_pr_vbe_info("[VODMA_get_datafs_increased_clock] 2.ratio_n_off = %d \n",ratio_n_off);
	}
	return ratio_n_off;
}

extern void drvif_scaler_set_fixlastline_vtotal_speedup_protect_on(void);
extern void drvif_scaler_set_frc2fsync_vtotal_protect_off(void);
extern void drvif_scaler_set_frc2fsync_recovery_vtotal(unsigned int u32_vtotal);
extern unsigned char vsc_decide_fixlastline_speedup_enable(void);
unsigned int drvif_framesync_gatting_get_phaseErr_lower_bound(void)
{
	UINT32 targetLower = 0;

	// calculated by formula in the future
	if( ( Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_V_FREQ_1000) >= V_FREQ_50000_mOFFSET ) && ( Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_V_FREQ_1000) < V_FREQ_50000_pOFFSET ) )
		targetLower = 19850;
	else
		targetLower = 16505;

	return targetLower;
}

unsigned int drvif_framesync_gatting_get_phaseErr_upper_bound(void)
{
	UINT32 targetUpper = 0;

	// calculated by formula in the future
	if( ( Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_V_FREQ_1000) >= V_FREQ_50000_mOFFSET ) && ( Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_V_FREQ_1000) < V_FREQ_50000_pOFFSET ) )
		targetUpper = 19950;
	else
		targetUpper = 17000;

	return targetUpper;
}

#define IPQ_DELAY 15 // I-domain PQ delay line
#define FIXLASTLINE_TRACKING_RANGE 6 // +- 0.6%
#define LOWDELAY_MARGIN 2
//#define FRAME_LATENCY 50     //percentage
unsigned char drv_framesync_check_iv2dv_phaseErr_on_fixlastline(void){
	vgip_vgip_chn1_lc_RBUS vgip_chn1_lc_reg;
	ppoverlay_uzudtg_fractional_fsync_RBUS uzudtg_fractional_fsync_reg;
	ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
	ppoverlay_dv_total_RBUS dv_total_reg;
	ppoverlay_new_meas1_linecnt_real_RBUS new_meas1_linecnt_real_reg;
	ppoverlay_new_meas2_linecnt_real_RBUS new_meas2_linecnt_real_reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
	ppoverlay_new_meas0_linecnt_real_RBUS ppoverlay_new_meas0_linecnt_real_reg;
	ppoverlay_dtg_frame_cnt1_RBUS dtg_frame_cnt1_reg;
	//ppoverlay_double_buffer_ctrl2_RBUS ppoverlay_double_buffer_ctrl2_reg;
	UINT32 voHtotal;
	UINT32 voVtotal;
	UINT32 voClock;
	UINT32 dHtotal;
	UINT32 dVFreq;
	UINT32 timeStamp1, timeStamp2,timeStamp3;
	UINT32 temp1, temp2;
	UINT32 inputFramePeroid, uzuDtgdisplayFramePeroid;
	UINT32 frameDelay_lower = 0;
	UINT32 frameDelay_upper = 0;
	UINT32 frameDelay_Result = 0;
	UINT32 id2dv_delay=0;
	UINT32 frameLatency=0;
	//UINT32 timeout=0;
	unsigned char checkResult=FALSE;
	SLR_VOINFO *pVOInfo = NULL;
	UINT32 dCLK;
	unsigned char frame_idicator=0;
    	unsigned int count=0x3ffff, idx1,idx2;
    voHtotal = Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_H_LEN);
    voVtotal = Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_V_LEN);
    pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
    voClock = pVOInfo->pixel_clk;
    dHtotal = Get_DISP_HORIZONTAL_TOTAL();
    dCLK = Get_DISPLAY_CLOCK_TYPICAL();
	dVFreq = Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000);

	if(_TRUE == Scaler_DispGetInputInfoByDisp(_MAIN_DISPLAY,SLR_INPUT_FRAMESYNC)){
		//data framesync vo gatting using panel clock
		voClock = Get_DISPLAY_CLOCK_TYPICAL(); //594MHz
	}

	if(dVFreq == 0){
		rtd_pr_vbe_notice("[FLL]dVFreq=%d\n", dVFreq);
		return FALSE;
	}

	voVtotal = voClock / voHtotal * 1000 / dVFreq;

    dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
    uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    uzudtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_fractional_fsync_reg);
    temp1 = (voHtotal*10000)/(voClock/10000);         //ms * 100000000
    temp2 = ((dHtotal*10000)/(dCLK/10000))*(uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode+1)
    /(uzudtg_fractional_fsync_reg.uzudtg_multiple_vsync+1);
    //temp3 =  ((dHtotal*10000)/(dCLK/10000));


   // rtd_pr_vbe_emerg("[ben] temp1=%d, temp2=%d, uzudtg_fractional_fsync_reg.uzudtg_multiple_vsyn=%d, uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode=%d\n",
   // temp1, temp2, uzudtg_fractional_fsync_reg.uzudtg_multiple_vsync, uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode);

    inputFramePeroid = 1000000000 / dVFreq;    //ms * 1000000
    uzuDtgdisplayFramePeroid = (temp2*(uzudtg_dv_total_reg.uzudtg_dv_total+1))/100;     //ms * 1000000

    ppoverlay_new_meas0_linecnt_real_reg.regValue = rtd_inl(PPOVERLAY_new_meas0_linecnt_real_reg);
    vgip_chn1_lc_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_LC_reg);
    new_meas1_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas1_linecnt_real_reg);
	new_meas2_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg);
    ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);

    timeStamp1 = temp1*vgip_chn1_lc_reg.ch1_line_cnt/100;   //ms * 1000000
    timeStamp2 = inputFramePeroid - timeStamp1;                  //ms * 1000000

//	if(new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt > ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line)
	// [RL6543-298] read after delay uzudtg real time line count
	if (new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt > ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line)
//		timeStamp3 = uzuDtgdisplayFramePeroid - (temp2*(new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt-ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line))/100;
		timeStamp3 = uzuDtgdisplayFramePeroid - (temp2*(new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt-ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line))/100;
	else
		timeStamp3 = 0xffffffff; //force to ingore
		//timeStamp3 = uzuDtgdisplayFramePeroid - (temp2*(dv_total_reg.dv_total+new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt-ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line))/100;

//    frameDelay_lower = (inputFramePeroid*FRAME_LATENCY)/100;    //ms * 1000000
//    frameDelay_upper = (inputFramePeroid*FRAME_LATENCY)/100+(inputFramePeroid*LOWDELAY_MARGIN)/100;  //ms * 1000000

	if(Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC)== TRUE){
		//Data fsync calculate iv2dv phase error
		frameDelay_lower = drvif_framesync_gatting_get_phaseErr_lower_bound();
		frameDelay_upper = drvif_framesync_gatting_get_phaseErr_upper_bound();
	}else{
		//Game mode calculate iv2dv phase error
		if(IoReg_Read32(VGIP_VGIP_VBISLI_reg)&_BIT19){ //for debug only
			id2dv_delay = drv_GameMode_decided_iv2dv_delay_old_mode();
		}else{
			id2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();
		}

		//if compression enable, need to add margin. @Crixus 20170605
		if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY))
			id2dv_delay = drv_GameMode_iv2dv_delay_compress_margin(id2dv_delay);

		// Add idomain PQ delay (@SDNR point of view)
		id2dv_delay += IPQ_DELAY;

		frameLatency = (id2dv_delay*10000)/voVtotal;

		frameDelay_lower = (inputFramePeroid*frameLatency)/10000+(inputFramePeroid*FIXLASTLINE_TRACKING_RANGE)/1000; //ms * 1000000
		frameDelay_upper = frameDelay_lower+(inputFramePeroid*LOWDELAY_MARGIN)/100;  //ms * 1000000
	}

    if(timeStamp3>timeStamp2){
		frameDelay_Result = (timeStamp3-timeStamp2);
    }else{
   		frameDelay_Result = (timeStamp3+timeStamp1);
    }

	if((frameDelay_Result>=frameDelay_lower)&&(frameDelay_Result<=frameDelay_upper))
		checkResult = TRUE;

    uzudtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_fractional_fsync_reg);
    if(uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode>0){
        frame_idicator = uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode;
    }
    if(frame_idicator>0){
        count = 0x3fffff;
        dtg_frame_cnt1_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT1_reg);
        //make sure that we can have a full porch apply timing change settings
        while((dtg_frame_cnt1_reg.uzudtg_input_frame_cnt!= frame_idicator-1)&& --count){
            dtg_frame_cnt1_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT1_reg);
        }
        idx1 = dtg_frame_cnt1_reg.uzudtg_input_frame_cnt;
        count = 0x3fffff;
        dtg_frame_cnt1_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT1_reg);
        //make sure that we can have a full porch apply timing change settings
        while((dtg_frame_cnt1_reg.uzudtg_input_frame_cnt!= frame_idicator)&& --count){
            dtg_frame_cnt1_reg.regValue = IoReg_Read32(PPOVERLAY_DTG_FRAME_CNT1_reg);
        }
        idx2 = dtg_frame_cnt1_reg.uzudtg_input_frame_cnt;
    }else{
        idx1 = idx2 = 0;
	}

    if(checkResult==TRUE){
#if 1  // [NOTICE} This debug msg cannot print on terminal directly, or it will impact FLL phase timing
		rtd_pr_vbe_notice("[FLL]voHtotal=%d, voVtotal=%d, voClock=%d, dHtotal=%d, dCLK=%d\n", voHtotal, voVtotal, voClock, dHtotal, dCLK);
		rtd_pr_vbe_notice("[FLL]inputFramePeroid=%d, uzuDtgdisplayFramePeroid=%d\n", inputFramePeroid, uzuDtgdisplayFramePeroid);
//		rtd_pr_vbe_notice("[FLL]vgip_lcnt=%d, uzu_lcnt=%d, memc_lcnt=%d, dtgM=%d\n", vgip_chn1_lc_reg.ch1_line_cnt, new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt, new_meas1_linecnt_real_reg.memcdtg_line_cnt_rt,
		// [RL6543-298] read after delay uzudtg real time line count
		rtd_pr_vbe_notice("[FLL]vgip_lcnt=%d, uzu_dly_lcnt=%d, memc_lcnt=%d, dtgM=%d\n", vgip_chn1_lc_reg.ch1_line_cnt, new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt, new_meas1_linecnt_real_reg.memcdtg_line_cnt_rt,
        IoReg_Read32(PPOVERLAY_new_meas0_linecnt_real_reg));
        rtd_pr_vbe_notice("[FLL]id2dv_delay=%d, frameLatency=%d\n", id2dv_delay, frameLatency);
        rtd_pr_vbe_notice("[FLL]idx1=%d, idx2=%d\n", idx1, idx2);
        rtd_pr_vbe_notice("[FLL]timeStamp1=%d, timeStamp2=%d, timeStamp3=%d\n", timeStamp1, timeStamp2, timeStamp3);
        rtd_pr_vbe_notice("[FLL]frameDelay_lower=%d, frameDelay_upper=%d\n", frameDelay_lower, frameDelay_upper);
        rtd_pr_vbe_notice("[FLL]frameDelay_Result=%d\n", frameDelay_Result);
        rtd_pr_vbe_notice("[FLL]dtgm2uzuvs_line=%d\n", ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line);
//		rtd_pr_vbe_notice("[FLL]uzu_lcnt=%d, uzu_vtotal=%d\n", new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt, uzudtg_dv_total_reg.uzudtg_dv_total);
		// [RL6543-298] read after delay uzudtg real time line count
		rtd_pr_vbe_notice("[FLL]uzu_dly_lcnt=%d, uzu_vtotal=%d\n", new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt,
			uzudtg_dv_total_reg.uzudtg_dv_total);
        rtd_pr_vbe_notice("[FLL]checkResult=%d\n", checkResult);
#endif
    }
	//dummy register for debug
	#ifdef _VIP_Mer7_Compile_Error__
	if((Get_DISPLAY_PANEL_MPLUS_RGBW() != TRUE)&&(Get_DISPLAY_PANEL_BOW_RGBW() != TRUE))
		IoReg_Write32(LG_M_PLUS_M_plus_reg004_reg, frameDelay_Result);
	#endif

    return checkResult;
}

unsigned int drv_framesync_on_fixlastline_phaseErr_verify(void){
	vgip_vgip_chn1_lc_RBUS vgip_chn1_lc_reg;
	ppoverlay_uzudtg_fractional_fsync_RBUS uzudtg_fractional_fsync_reg;
	ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
//	ppoverlay_new_meas1_linecnt_real_RBUS new_meas1_linecnt_real_reg;
	ppoverlay_new_meas2_linecnt_real_RBUS new_meas2_linecnt_real_reg;
	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
	ppoverlay_new_meas0_linecnt_real_RBUS ppoverlay_new_meas0_linecnt_real_reg;
	UINT32 voHtotal;
	//UINT32 voVtotal;
	UINT32 voClock;
	UINT32 dHtotal;
	UINT32 dVFreq;
	UINT32 timeStamp1, timeStamp2,timeStamp3;
	UINT32 temp1, temp2;
	UINT32 inputFramePeroid, uzuDtgdisplayFramePeroid;
	UINT32 frameDelay_Result = 0;
//	UINT32 id2dv_delay=0;
	SLR_VOINFO *pVOInfo = NULL;
	 UINT32 dCLK;
	// unsigned char frame_idicator=0;
    //unsigned int count=0x3ffff, idx1,idx2;
    voHtotal = Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_H_LEN);
    //voVtotal = Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_V_LEN);
    pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
    voClock = pVOInfo->pixel_clk;
    dHtotal = Get_DISP_HORIZONTAL_TOTAL();
    dCLK = Get_DISPLAY_CLOCK_TYPICAL();
	dVFreq = Scaler_DispGetInputInfoByDisp(0,SLR_INPUT_V_FREQ_1000);

	if(_TRUE == Scaler_DispGetInputInfoByDisp(_MAIN_DISPLAY,SLR_INPUT_FRAMESYNC)){
		voClock = Get_DISPLAY_CLOCK_TYPICAL(); //594MHz
	}

	if(dVFreq == 0){
		rtd_pr_new_game_mode_notice("[Game Mode][%s][ERROR] dVFreq = %d \n", __FUNCTION__, dVFreq);
		return 0;
	}

	//voVtotal = (voClock / (voHtotal*dVFreq))*10;

    uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    uzudtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_fractional_fsync_reg);
    temp1 = (voHtotal*10000)/(voClock/10000);         //ms * 100000000
    temp2 = ((dHtotal*10000)/(dCLK/10000))*(uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode+1)
    /(uzudtg_fractional_fsync_reg.uzudtg_multiple_vsync+1);
    //temp3 =  ((dHtotal*10000)/(dCLK/10000));

	inputFramePeroid = 1000000000 / dVFreq;	//ms * 1000000
    uzuDtgdisplayFramePeroid = (temp2*(uzudtg_dv_total_reg.uzudtg_dv_total+1))/100;     //ms * 1000000

    ppoverlay_new_meas0_linecnt_real_reg.regValue = rtd_inl(PPOVERLAY_new_meas0_linecnt_real_reg);
    vgip_chn1_lc_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_LC_reg);
//    new_meas1_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas1_linecnt_real_reg);
	new_meas2_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas2_linecnt_real_reg);
    ppoverlay_uzudtg_control1_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_control1_reg);

    timeStamp1 = temp1*vgip_chn1_lc_reg.ch1_line_cnt/100;   //ms * 1000000
    timeStamp2 = inputFramePeroid - timeStamp1;                  //ms * 1000000

//	if(new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt > ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line)
	// [RL6543-298] read after delay uzudtg real time line count
	if(new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt > ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line)
//		timeStamp3 = uzuDtgdisplayFramePeroid - (temp2*(new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt-ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line))/100;
		timeStamp3 = uzuDtgdisplayFramePeroid - (temp2*(new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt - ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line))/100;
	else
		timeStamp3 = 0xffffffff; //force to ingore
		//timeStamp3 = uzuDtgdisplayFramePeroid - (temp2*(dv_total_reg.dv_total+new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt-ppoverlay_uzudtg_control1_reg.dtgm2uzuvs_line))/100;

    if(timeStamp3>timeStamp2){
		frameDelay_Result = (timeStamp3-timeStamp2);
    }else{
   		frameDelay_Result = (timeStamp3+timeStamp1);
    }

#if 1  // For debug
        //rtd_pr_vbe_notice("vgip_lcnt=%d, uzu_lcnt=%d, memc_lcnt=%d, dtgM=%d\n", vgip_chn1_lc_reg.ch1_line_cnt, new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt, new_meas1_linecnt_real_reg.memcdtg_line_cnt_rt,
        //IoReg_Read32(PPOVERLAY_new_meas0_linecnt_real_reg));
        //rtd_pr_vbe_notice("timeStamp1=%d, timeStamp2=%d, timeStamp3=%d\n", timeStamp1, timeStamp2, timeStamp3);
		if((Get_DISPLAY_PANEL_MPLUS_RGBW() != TRUE)&&(Get_DISPLAY_PANEL_BOW_RGBW() != TRUE)){
			#ifdef _VIP_Mer7_Compile_Error__
			IoReg_Write32(LG_M_PLUS_M_plus_reg008_reg, frameDelay_Result);
			IoReg_Write32(LG_M_PLUS_M_plus_reg00c_reg, timeStamp1);
			IoReg_Write32(LG_M_PLUS_M_plus_reg010_reg, timeStamp2);
			IoReg_Write32(LG_M_PLUS_M_plus_reg014_reg, timeStamp3);
			IoReg_Write32(LG_M_PLUS_M_plus_reg018_reg, vgip_chn1_lc_reg.ch1_line_cnt);
			//IoReg_Write32(0xb8028c1c, new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt);
			// [RL6543-298] read after delay uzudtg real time line count
			IoReg_Write32(LG_M_PLUS_M_plus_reg01c_reg, new_meas2_linecnt_real_reg.uzudtg_dly_line_cnt_rt);
			#endif
		}

#endif

	return frameDelay_Result;
}

//get line delay @ uzudtg line count
unsigned int drv_framesync_get_gamemode_iv2dv_linedelay(void)
{
    UINT32 iv2dv_delay=0;
    unsigned long long iDelay= 0;
    ppoverlay_uzudtg_dv_total_RBUS uzudtg_dv_total_reg;
    ppoverlay_uzudtg_fractional_fsync_RBUS uzudtg_fractional_fsync_reg;
    ppoverlay_fs_iv_dv_fine_tuning2_RBUS ppoverlay_fs_iv_dv_fine_tuning2_reg;
    ppoverlay_dctl_iv2dv_ihs_ctrl_RBUS ppoverlay_dctl_iv2dv_ihs_ctrl_reg;
    UINT32 dCLK;
    UINT32 dHtotal;
    UINT32 dOneLineTime;
    UINT32 line;
    SLR_VOINFO *pVOInfo = NULL;
    unsigned long long iClk = 0;
    UINT32 iHtotal = 0;

    UINT32 hdmi_vfreq = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVFREQ_1000);
    UINT32 hdmi_htotal = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHLEN);
    UINT32 hdmi_vtotal = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVLEN);

    UINT32 vfreq = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000);
    UINT32 htotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);
    UINT32 vtotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN);

    ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg);
    ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg);
    pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());

    if(!get_force_i3ddma_enable(SLR_MAIN_DISPLAY) && (Scaler_InputSrcGetType(SLR_MAIN_DISPLAY) != _SRC_VO))
    {
        if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) && ((hdmi_htotal != 0) && (hdmi_vtotal != 0) && (hdmi_vfreq != 0)))
        {
            iClk = hdmi_htotal;
            iClk = iClk * hdmi_vtotal * hdmi_vfreq / 1000;
            iHtotal = hdmi_htotal;
        }
        else
        {
            iClk = htotal;
            iClk =iClk * vtotal * vfreq / 1000;
            iHtotal = htotal;
        }
    }
    else
    {
        if((pVOInfo == NULL) || (pVOInfo->pixel_clk == 0) || (pVOInfo->h_total ==  0) || (pVOInfo->v_total == 0))
        {
            rtd_pr_new_game_mode_notice("[%s %d]ERROR vo info error\n", __FUNCTION__, __LINE__);
            return 0;
        }
        else
        {
            iHtotal = pVOInfo->h_total;
            iClk = pVOInfo->pixel_clk;
        }
    }

    //interlace case need mul 2
    if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_INTERLACE))
    {
        iClk = iClk * 2;
    }

    iv2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();

    if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY))
    {
        iv2dv_delay = drv_GameMode_iv2dv_delay_compress_margin(iv2dv_delay);
    }

    rtd_pr_new_game_mode_emerg("[Game Mode]iv2dv_delay (final2) = %d\n", iv2dv_delay);

    iDelay = iv2dv_delay + FIXLASTLINE_TRACKING_RANGE;
    //iDelay = iDelay * iHtotal * 10000 / (iClk / 10000);
    iDelay = iDelay * iHtotal * 10000;
    iClk = div64_u64(iClk, 10000);
    iDelay = div64_u64(iDelay, iClk);
    rtd_pr_new_game_mode_notice("[Game Mode][%s] iDelay = %d \n", __FUNCTION__, iDelay);

    dHtotal = Get_DISP_HORIZONTAL_TOTAL();
    dCLK = Get_DISPLAY_CLOCK_TYPICAL();
    uzudtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg);
    uzudtg_fractional_fsync_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_fractional_fsync_reg);

    if(ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_xclk_en == 1)
    {
        dOneLineTime = (ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_width * 10000) / (27000000 / 10000);

        rtd_pr_new_game_mode_notice("[%s %d]iv2dv delay base on 27M dOneLineTime:%d\n", __FUNCTION__, __LINE__ , dOneLineTime);
    }
    else
    {
        dOneLineTime = ((dHtotal * 10000) / (dCLK / 10000)) * (uzudtg_fractional_fsync_reg.uzudtg_remove_half_ivs_mode + 1)
            / (uzudtg_fractional_fsync_reg.uzudtg_multiple_vsync + 1);

        if(get_panel_pixel_mode() > PANEL_1_PIXEL)
        {
            dOneLineTime /= 2;
        }

        rtd_pr_new_game_mode_notice("[%s %d]iv2dv delay base on input clk dOneLineTime:%d\n", __FUNCTION__, __LINE__ , dOneLineTime);
    }

    //line  = iDelay / dOneLineTime;
    line = div64_u64(iDelay, dOneLineTime);
    rtd_pr_new_game_mode_notice("[Game Mode][%s] line = %d \n", __FUNCTION__, line);

    rtd_pr_new_game_mode_notice("[Game Mode][%s] uzudtg delay line = %d \n", __FUNCTION__, ((uzudtg_dv_total_reg.uzudtg_dv_total + 1) - line));

    return ((uzudtg_dv_total_reg.uzudtg_dv_total  + 1) - line);

}

#define DFSYS_FLL_TIMEOUT_CNT 5
#define DFSYS_FLL_PHASE_ERR_OFFSET 0x50


unsigned char drvif_framesync_gatting_do(void)
{
	//Lewis, move it to VIO_Interface.h
	//#define FIXED_DVST      22
	//#define FIXED_IV2DVDLY  7
	/* fix value for gatting clk */
//	dv_den_start_end_RBUS dvden_1802801c;
//	fs_iv_dv_fine_tuning5_RBUS fs_iv_dv_fine_tuning5;
	unsigned int cur_time = 0;
	ppoverlay_display_timing_ctrl2_RBUS display_timing_ctrl2_reg;

	//ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
	//ppoverlay_uzudtg_control2_RBUS ppoverlay_uzudtg_control2_reg;
	//ppoverlay_memcdtg_control3_RBUS ppoverlay_memcdtg_control3_reg;

	//int ii = 0;
	//fs_iv_dv_fine_tuning3_RBUS fs_iv_dv_fine_tuning3;
//	unsigned int iv2dv_dly;
//	unsigned int line_delay;
//	unsigned int fixed_dvst, iv2dv_dly;
//	unsigned int MLines = 0;
	unsigned int framerate = 60;
	// unsigned char result=0;
	//unsigned int voAddr = 0;
//	vodma_vodma_clkgen_RBUS vodma_vodma_clkgen_reg;

	framerate = ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) + 999 ) / 1000;

	//frank@0327 add below code to solve Data framesync DVS not sync IVS
	display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);
	display_timing_ctrl2_reg.frc2fsync_en = 1;
	//display_timing_ctrl2_reg.shpnr_line_mode = 2;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL2_reg, display_timing_ctrl2_reg.regValue);

	//frank@04082013 add below code close DTG pattern generator to solve Data FS fail.
	drvif_scalerdisplay_force_black_bg(0);

	fw_scaler_enable_fs_wdg(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), FALSE);
	//drvif_scaler_set_display_mode(DISPLAY_MODE_FRC);

//	SET_PANEL_DCLK_DIV(1); //mag2 un-used dclk sel, move to dpll_o
	drvif_clock_set_dclk2(Get_DISP_HORIZONTAL_TOTAL() , Get_DISP_VERTICAL_TOTAL(), framerate);

    if (Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1)
    {
        memory_set_display_us_path(SLR_MAIN_DISPLAY);

        // Apply double buffer
        IoReg_SetBits(MDOMAIN_DISP_Disp0_db_ctrl_reg,  MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
        if(get_mdomain_1p_for_hdmi_4k120_flag())
        {
            memory_set_capture_ds_path(SLR_SUB_DISPLAY);
            //apply double buffer
            IoReg_SetBits(MDOM_SUB_DISP_Disp1_db_ctrl_reg,  MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_apply_mask);
        }
    } else
    {
        memory_set_display_us_path(SLR_SUB_DISPLAY);
        IoReg_SetBits(MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_reg,  MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_disp1_fs_en_mask);
        
        //apply double buffer
        IoReg_SetBits(MDOM_SUB_DISP_Disp1_db_ctrl_reg,  MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_apply_mask);
    }
			

	cur_time = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);//save current time for timeout check
	while(IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg)& MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask)
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


	if(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1) { /*main channel*/

        memory_set_capture_ds_path(SLR_MAIN_DISPLAY);
        IoReg_SetBits(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg,  MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_disp0_fs_en_mask);
        
        // apply db
        IoReg_SetBits(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,  MDOMAIN_CAP_cap0_reg_doublbuffer_cap0_db_apply_mask);
		if(get_mdomain_1p_for_hdmi_4k120_flag()){
            memory_set_capture_ds_path(SLR_SUB_DISPLAY);
            IoReg_SetBits(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg,  MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_disp0_fs_en_mask);
            
            // apply db
            IoReg_SetBits(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,  MDOM_SUB_CAP_cap1_reg_doublbuffer_cap1_db_apply_mask); 
        }
	} else {
        memory_set_capture_ds_path(SLR_SUB_DISPLAY);
        IoReg_SetBits(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg,  MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_disp0_fs_en_mask);
        
        // apply db
        IoReg_SetBits(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,  MDOM_SUB_CAP_cap1_reg_doublbuffer_cap1_db_apply_mask);
	}

		//framesync_set_iv2dv_delay(DISPLAY_MODE_NEW_FLL_MODE);

//	UINT32 stableCount = 0;
//	ppoverlay_fix_last_line_mode_lock_status_RBUS ppoverlay_fix_last_line_mode_lock_status_reg;
	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	drvif_scalerdisplay_set_dbuffer_en(SLR_MAIN_DISPLAY, FALSE);
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//frank@1121 mark do by below code rtdf_clearBits(PPOVERLAY_Display_Timing_CTRL1_reg, _BIT29);

	fwif_color_safe_od_enable(0);
	drvif_scaler_set_display_mode(DISPLAY_MODE_FRAME_SYNC, scaler_dtg_get_app_type());
	Scaler_Set_Display_Timing_Mode(DISPLAY_MODE_FRAME_SYNC);
	fwif_color_safe_od_enable(1);

	//frank@1121 mark do by later code fw_scaler_enable_fs_wdg(info->display, FALSE);
	return 0;
}


// VO source interlaced SD video (480i/576i) frame sync timing control
unsigned char drvif_decide_VO_SD_video_frameSync_enable(void)
{
	return framesync_vo_sd_fs_enable_flg;
}

unsigned int ori_voclk_ratio_num = 0;
void drvif_framesync_gatting(unsigned char enable)
{
	unsigned int clkgen_reg = 0;//, free_period = 0;
	static unsigned int ori_divider_num = 0;
	unsigned int ratio_num = 0;
	unsigned int divider_num = 0;
	//unsigned int source_clk = 0;
	unsigned int voAddr = 0;
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
#endif // SCALER_ERROR

	//unsigned int source_clk = 0;
#ifndef CONFIG_ALL_SOURCE_DATA_FS
	unsigned int vo_clk = 0;
	SLR_VOINFO* pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
#endif

    unsigned int vodmapll = VO_Get_vodmapll();
    unsigned int tempratio_num=0;


	if(Scaler_Get_CurVoInfo_VoVideoPlane(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) == VO_VIDEO_PLANE_V2)
		voAddr = VODMA2_VODMA2_CLKGEN_reg;
	else
		voAddr = VODMA_VODMA_CLKGEN_reg;
	if(enable){

		if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){

			//chroma Error
			if(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN) < 500)
				IoReg_SetBits(DI_IM_DI_FRAMESOBEL_STATISTIC_reg, _BIT31);
			else
				IoReg_ClearBits(DI_IM_DI_FRAMESOBEL_STATISTIC_reg, _BIT31);

		//	UINT32 regVal = IoReg_Read32(VODMA_VODMA_PVS0_Gen_reg);
			IoReg_ClearBits(VODMA_VODMA_PVS0_Gen_reg, _BIT31);
			msleep(20);
			IoReg_Mask32(VODMA_VODMA_PVS0_Gen_reg, ~(_BIT24|_BIT25|_BIT26), _BIT25);
			msleep(20);
			IoReg_SetBits(VODMA_VODMA_PVS0_Gen_reg, _BIT31);
		}

		//source_clk = pVOInfo->pixel_clk/1000000;
		if(ori_voclk_ratio_num == 0)
		{
			//Open VODMA gating
			clkgen_reg = IoReg_Read32(voAddr);
			ori_divider_num = (clkgen_reg&0x0f800000)>>23;
			if(Get_DisplayMode_Src(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) == VSC_INPUTSRC_VDEC || Get_DisplayMode_Src(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)) == VSC_INPUTSRC_CAMERA) {
				tempratio_num=pVOInfo->pixel_clk/(vodmapll/64);
				if (tempratio_num>=64) {
					ori_voclk_ratio_num=0;
				} else {
					ori_voclk_ratio_num=64-tempratio_num;
				}
			} else {
				ori_voclk_ratio_num = (clkgen_reg&0x003f0000)>>16;
			}

#if 0
			ratio_num = ori_voclk_ratio_num;
			if(source_clk >= 490){
				ratio_num = 0;
			}
			else{
				if ((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) ==VSC_INPUTSRC_HDMI) && (ratio_num>30)) {//4// for  3440*1440@50 frameysnc fail
					ratio_num -= 1;
				}
				if(ratio_num > 5)
					ratio_num -= 5;
				else
					ratio_num = 0;
			}
#endif
			if(0)
			{ //1080i/P case
#ifdef CONFIG_ALL_SOURCE_DATA_FS
				//mac5p force vo clk for 2k1k datafs
				divider_num = 1;
				ratio_num = 0;
#else
				// [VO] frame sync timing clock control for VO source (reducing the bandwidth usage)
				if(Scaler_InputSrcGetMainChType() == _SRC_VO){
					// [DI] bandwidth not enough for DI, VO clock need do division
					if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY,SLR_DISP_INTERLACE) == TRUE)
					{
						unsigned int source_clk = 0;
					//	UINT32 picWidth = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);

						source_clk = pVOInfo->pixel_clk/1000000;
						vo_clk = (VO_Get_vodmapll()/1000000);

						if(vo_clk >= 300){
							ratio_num = 32-((32*source_clk)/150);
							rtd_pr_vbe_debug("################ source_clk=%d, ratio_num=%d\n", source_clk, ratio_num);
							if(ratio_num > 19){
								divider_num = 1;
								ratio_num = 0;
							}else if((ratio_num <= 19)&&(ratio_num >2)){
								divider_num = 0;
								ratio_num = 5;
							}else{
								divider_num = 0;
								ratio_num = 0;
							}
						}else if((vo_clk >= 250) && (vo_clk < 260)){
							ratio_num = 32-((32*source_clk)/(vo_clk/2));
							rtd_pr_vbe_debug("################ source_clk=%d, ratio_num=%d\n", source_clk, ratio_num);
							divider_num = 0;
							if(ratio_num >2){
								divider_num = 0;
								ratio_num = 3;
							}else{
								divider_num = 0;
								ratio_num = 0;
							}
						}else if((vo_clk >= 180) && (vo_clk < 200)){
							ratio_num = 32-((32*source_clk)/vo_clk);
							rtd_pr_vbe_debug("################ source_clk=%d, ratio_num=%d\n", source_clk, ratio_num);
							divider_num = 0;

							if(ratio_num >20){
								divider_num = 0;
								ratio_num = 2;
							}else{
								divider_num = 0;
								ratio_num = 0;
							}
						}else{
							rtd_pr_vbe_debug("###### [WARNING][%s] [line:%d]  No match vo gating clk !########  \n", __FUNCTION__, __LINE__);
						}

					}
					// [RTNR] bandwidth not enough for RTNR, VO clock need do division
					else {
						#define _VZU_GATTING_THRES	100
						unsigned int source_clk = 0;
						UINT32 picWidth = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
					//	UINT32 picLen = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
					//	UINT32 dispLen =Scaler_DispGetInputInfo(SLR_INPUT_DISP_LEN);

                                      source_clk = pVOInfo->pixel_clk/1000000;
						vo_clk = (VO_Get_vodmapll()/1000000);
						if(vo_clk >= 300){
						ratio_num = 32-((32*source_clk)/300);
						rtd_pr_vbe_debug("################ source_clk=%d, ratio_num=%d\n", source_clk, ratio_num);
							divider_num = 0;
							if(ratio_num > 27){
								divider_num = 2;
								ratio_num = 0;
							}else if((ratio_num <= 26)&&(ratio_num >17)){
								divider_num = 1;
								ratio_num = 0;
							}else if((ratio_num <= 17)&&(ratio_num >3)){
								divider_num = 0;
								ratio_num = 5;
							}else{
								divider_num = 0;
								ratio_num = 0;
							}
						}else if((vo_clk >= 250) && (vo_clk < 260)){
							ratio_num = 32-((32*source_clk)/vo_clk);
							rtd_pr_vbe_debug("################ source_clk=%d, ratio_num=%d\n", source_clk, ratio_num);
							divider_num = 0;
							if(ratio_num >3){
								divider_num = 0;
								ratio_num = 3;
							}else{
								divider_num = 0;
								ratio_num = 0;
							}
						}else if((vo_clk >= 180) && (vo_clk < 200)){
							ratio_num = 32-((32*source_clk)/vo_clk);
							rtd_pr_vbe_debug("################ source_clk=%d, ratio_num=%d\n", source_clk, ratio_num);
							divider_num = 0;

							if(ratio_num >20){
								divider_num = 0;
								ratio_num = 2;
							}else{
								divider_num = 0;
								ratio_num = 0;
							}
						}else{
							rtd_pr_vbe_debug("###### [WARNING][%s] [line:%d]  No match vo gating clk !########  \n", __FUNCTION__, __LINE__);
						}



						if(picWidth >=3000){//  4k2k case
							if(source_clk <280){ // 4k2k 24/25
								ratio_num = 0;
								divider_num = 1;
							}
							else{
								ratio_num = 0;
								divider_num = 0;
							}
						}

						//divider_num = 0;
						rtd_pr_vbe_debug("[vo] ratio_num For RTNR=%d\n", ratio_num);
					}
					if(Get_vo_smoothtoggle_timingchange_flag(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)))  {
						ratio_num = 0;
						divider_num = 0;
					}
				#if 0
					if((vo_force_data_mode_get_enable()>=SLR_DISPLAY_DATA_FRAME_SYNC)&&(vo_force_data_mode_get_enable()<=SLR_DISPLAY_DATA_FS_AND_CUSTOM_MODE))
					{
						divider_num = 0;

						if(vo_clk >= 300)
							ratio_num=5;
						else if((vo_clk >= 250) && (vo_clk < 260))
							ratio_num=3;
						else if((vo_clk >= 180) && (vo_clk < 200))
							ratio_num=0;
						// rika 20140519 set vo clock cause mac2 200,sirisu 330
					}
				#endif
					//Fix me later, only for experiment
					ratio_num = 0;
					divider_num = 0;
					rtd_pr_vbe_debug("################ div=%d, ratio_num=%d\n", divider_num, ratio_num);
				}else
				/*if(pVOInfo->pixel_clk < 200000000){//200MHZ
					if (Scaler_DispGetStatus(SLR_MAIN_DISPLAY,SLR_DISP_INTERLACE)) {
                                        	source_clk = pVOInfo->pixel_clk/1000000;
						ratio_num = 32-((32*source_clk)/150);
						ratio_num -= 3;
					}else{
						source_clk = pVOInfo->pixel_clk/1000000;
						ratio_num = 32-((32*source_clk)/300);
						ratio_num -= 3;
					}
				}else*/{ //300MHZ case
					ratio_num = 0;
				}
#endif
				clkgen_reg = (clkgen_reg &(~0x0f9f0000))|(divider_num<<23)|(ratio_num<<16);
			}
			else	//Merlin5 new flow
			{
				ratio_num = VODMA_get_datafs_increased_clock(ori_voclk_ratio_num);
			}

			clkgen_reg = (clkgen_reg &(~0x0fBf0000))|(ori_divider_num<<23)|(ratio_num<<16);

			/*pll_ssc4_reg.regValue = IoReg_Read32(PLL27X_REG_PLL_SSC4_reg);
			if(pll_ssc4_reg.en_ssc == 1){
				sscg_enable = 1;
				pll_ssc4_reg.en_ssc = 0;
				IoReg_Write32(PLL27X_REG_PLL_SSC4_reg,pll_ssc4_reg.regValue);
				msleep(20);
				rtd_pr_vbe_debug("[vo] gating, disable SSCG. \n");
			}*/
	//		rtd_pr_vbe_emerg("### (voAddr %x) => %x ### \n", voAddr, IoReg_Read32(voAddr));

#ifdef BUILD_QUICK_SHOW
			IoReg_Write32(voAddr, clkgen_reg); //increase clk
#endif
			//msleep(50);
			//move to idomain configure done
#if 0			//cal iv2pv delay after ratio_num ok
			if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
				&&(!drvif_i3ddma_triplebuf_flag())
				&&(get_HDMI_HDR_mode() != HDR_DOLBY_HDMI)
				){
				drv_I3ddmaVodma_GameMode_iv2pv_delay(TRUE);
			} else {
				drv_I3ddmaVodma_GameMode_iv2pv_delay(FALSE);
			}
#endif

			if (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_SUB_DISPLAY)
				clkgen_reg |= 0x2;

#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
			//Write Clear
			mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
			mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf = TRUE;
			mdomain_cap_ich12_ibuff_status_reg.fsync_ovf = TRUE;
			mdomain_cap_ich12_ibuff_status_reg.fsync_udf = TRUE;
			IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg, mdomain_cap_ich12_ibuff_status_reg.regValue);
#endif // SCALER_ERROR

			//IoReg_SetBits(SCALEDOWN_ICH1_IBUFF_STATUS_VADDR,  _BIT0|_BIT1|_BIT2);//write status
			//frank@09032010 wait one input frame
			//WaitFor_IVS1_Done();//Will mark no need
			//WaitFor_IVS1_Done();//Will mark no need
			/*if(sscg_enable == 1){
				pll_ssc4_reg.en_ssc = 1;
				IoReg_Write32(PLL27X_REG_PLL_SSC4_reg,pll_ssc4_reg.regValue);
				rtd_pr_vbe_debug("[vo] gating, enable SSCG. \n");
			}*/

		}
	}else{

		if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
			IoReg_ClearBits(PPOVERLAY_Display_Timing_CTRL1_reg, _BIT26);
		}

		clkgen_reg = IoReg_Read32(voAddr);

		if(ori_voclk_ratio_num != 0){
			clkgen_reg = (clkgen_reg &(~0x0fBf0001))|(ori_divider_num<<23)|(ori_voclk_ratio_num<<16);
			//WaitFor_DEN_STOP_Done();//will mark no need
			//IoReg_Mask32(IDTG_PVS_FREE_VADDR, 0xff000000, ori_free_period);
		}

		clkgen_reg = clkgen_reg&0xfffffffe;
		IoReg_Write32(voAddr, clkgen_reg); //disable gatting clk
		ori_voclk_ratio_num = 0;
	}
	rtd_pr_vbe_notice("################ drvif_framesync_gatting (%d, plane %d) => %x\n", enable, Scaler_Get_CurVoInfo_plane(), IoReg_Read32(voAddr));
}

void drvif_UHD_HDMI_increase_clk(void)
{
	unsigned int clkgen_reg = 0;//, free_period = 0;
	static unsigned int ori_divider_num = 0;
	unsigned int ratio_num = 0;
	unsigned int vodmapll = VO_Get_vodmapll();
	unsigned int tempratio_num=0;
	SLR_VOINFO* pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());

	if(ori_voclk_ratio_num == 0)
	{
		//Open VODMA gating
		clkgen_reg = IoReg_Read32(VODMA_VODMA_CLKGEN_reg);
		ori_divider_num = (clkgen_reg&0x0f800000)>>23;
		tempratio_num=pVOInfo->pixel_clk/(vodmapll/64);
		if (tempratio_num>=64) {
			ori_voclk_ratio_num=0;
		} else {
			ori_voclk_ratio_num=64-tempratio_num;
		}
		ratio_num = VODMA_get_datafs_increased_clock(ori_voclk_ratio_num);

		clkgen_reg = (clkgen_reg &(~0x0fBf0000))|(ori_divider_num<<23)|(ratio_num<<16);
		IoReg_Write32(VODMA_VODMA_CLKGEN_reg, clkgen_reg); //increase clk
		rtd_pr_vbe_emerg("### Force increase VO clock for UHD HDMI, CLKGEN => %x ### \n", IoReg_Read32(VODMA_VODMA_CLKGEN_reg));
	}
}

//--------------------------------------------------
// Description  : Adjust Dclk offset to meet frame sync
// Input Value  : None
// Output Value : Return 0 if sucess
//--------------------------------------------------
/**
* drvif_framesync_do
*  This function will djust Dclk offset to meet frame sync
* @param <None>
*@return <unsigned char>  	{Return 0 if sucess}
*/
unsigned char drvif_framesync_do(void)
{

	unsigned short buffer = 0, delta=0;
	unsigned short fine = 0, result = 0;
	unsigned char	Data[16] = {0};
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	unsigned int timeout = 0;
	mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;
//	ddr_mainstatus_RBUS ddr_mainstatus_reg;
    mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
    mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
#endif //SCALER_ERROR

	pll27x_reg_sys_dclkss_RBUS dclkss_reg;
//	display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	vgip_vgip_chn1_status_RBUS vgip_chn1_status_reg;
	//scaledown_ich1_ibuff_status_RBUS ich1_ibuff_status_reg;
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_disp_ddr_subctrl_RBUS ddr_subctrl_reg;
#endif //SCALER_ERROR
//	ddr_substatus_RBUS ddr_substatus_reg;
//	vgip_chn2_status_RBUS vgip_chn2_status_reg;
//	ich2_ibuff_STATUS_RBUS ich2_ibuff_STATUS_reg;
#endif
	unsigned short temp = 0;
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
#endif // SCALER_ERROR

	fw_scaler_enable_fs_wdg(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), FALSE);

	if(_CHANNEL1 == Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))
	{
		// Source_Sel[1:0]
		// 00 : use info. from Cap#1.
		// 01 : use info. from Cap#2.
		// 1x : frame sync mode.
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		ddr_mainctrl_reg.main_source_sel = 3;
		//ddr_mainctrl_reg.main_in_format = 1; // 444
		ddr_mainctrl_reg.main_in_format = !Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_422CAP);
		ddr_mainctrl_reg.main_bit_num = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT); // 8bit or 10bit
		IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);

		mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
		mdomain_disp_disp0_db_ctrl_reg.disp0_db_en = _ENABLE; // enable double buffer
		IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg, mdomain_disp_disp0_db_ctrl_reg.regValue);

		timeout = 0xffff;
		ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		while(((1 != ddr_mainctrl_reg.main_in_format) || (Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT) != ddr_mainctrl_reg.main_bit_num)) && (timeout))
		{
			ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
			timeout--;
			msleep(10);
		}
#endif //SCALER_ERROR
	}
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
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

		mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
		mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en = _ENABLE; // enable double buffer
		IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg, mdom_sub_disp_disp1_db_ctrl_reg.regValue);

		timeout = 0xffff;
		ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		while(((1 != ddr_subctrl_reg.sub_in_format) || (Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT) != ddr_subctrl_reg.sub_bit_num)) && (timeout))
		{
			ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
			timeout--;
		}
	}
#endif
#endif //SCALER_ERROR

	if(Scaler_InputSrcGetMainChType()!=_SRC_VO)
		drv_framesync_set_offline_ms_src();

	// Disable spread spectrum
	drv_adjust_spreadspectrumrange(0);

#if 0   //mark scalerlib control by qing_liu
// CSW+ 0961225 Disable Fix last line before open it
	if(_TRUE == Scaler_Get_FramesyncOpenFixLastLine())
	{
		//step 5, Disable F and F-NdF switch
		dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_reg);
		dclkss_reg.dclk_new_en = _ENABLE;
		IoReg_Write32(PLL27X_REG_SYS_DCLKSS_reg, dclkss_reg.regValue);

		display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
		display_timing_ctrl1_reg.disp_fix_last_line = _DISABLE;
		IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);
	}
#endif
	// // Enable DMA clock sel when disable double dvs frank@04122011 add below for DMACLK
#if 0 // move to dpll_o
	if(!modestate_decide_double_dvs_enable())
	{
		sys_dispclksel_RBUS sys_dispclksel_reg;
		sys_dispclksel_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DISPCLKSEL_reg);
		sys_dispclksel_reg.dispd_video_div= _ENABLE;
		IoReg_Write32(PLL27X_REG_SYS_DISPCLKSEL_reg, sys_dispclksel_reg.regValue);
	}
#endif

	{
		// Fast Framesync method
		delta = framesync_fast_get_sync_point();
	       //measure aclkcnt fail
		if(delta == 0xfff)
			return 2;
		drvif_clock_offset(delta);
	}

	drvif_framesync_front_porch_check();

	Scaler_DispSetIvs2DvsDelay(drv_adjust_measureivs2dvsdelay());
	if(Scaler_DispGetIvs2DvsDelay() == 0){
		rtd_pr_vbe_debug("Measure_IVS2DVSDelay FAIL in FrameSync_do\n");
		return 2;
	}

//	framesync_update_ivs_position(Scaler_DispGetIvs2DvsDelay());
	rtd_pr_vbe_debug("ret offset =%x fast do\n", delta);

	if(framesync_testsync(delta, &Data[0])) {
		rtd_pr_vbe_debug("FrameSyncTestSync error1:  HS/VS wrong assert(b8052414)");
		return 2;
	}

	if(Data[0] !=0x00){
	//	rtd_pr_vbe_info(" ret=%x after test0 : Frame sync not OK after FrameSyncFastDo!!\n", Data[0]);
	#if 0
		if(Scaler_InputSrcGetMainChFrom()  == _SRC_FROM_TMDS) {
			if (drvif_IsAVMute()) {	// HDMI Avmute
				return 2;
			}
		}
	#endif
		rtd_pr_vbe_debug("\n ret=%x after test0 : Frame sync not OK after FrameSyncFastDo!!\n", Data[0]);
	}else
		rtd_pr_vbe_debug("\n ret=%x after test0 : Frame sync OK after FrameSyncFastDo!!\n", Data[0]);

	if(Data[0] == 0)//frank@0326 framesync OK
	{
		goto _FRAME_SYNC_DO_END; //lewis 20091207, we need apply fix last line function.......
	}

	if(Data[0] == 0) {
		result |= _BIT0;

		if(framesync_testsync(delta + _OFFSET_STEP, &Data[0])) {
			rtd_pr_vbe_debug("FrameSyncTestSync error2:  HS/VS wrong assert(b8052414)");
			return 2;
		}

		rtd_pr_vbe_debug("\n ret=%x after test1 : offset + _OFFSET_STEP!!\n", Data[0]);
		if(Data[0] == 0)   result |= _BIT1;

		if(framesync_testsync(delta - _OFFSET_STEP, &Data[0])){
			rtd_pr_vbe_debug("FrameSyncTestSync error3:  HS/VS wrong assert(b8052414)");
			return 2;
		}

		rtd_pr_vbe_debug("\n ret=%x after test2 :  offset  - _OFFSET_STEP!!\n", Data[0]);
		if(Data[0] == 0)   result |= _BIT2;

		if((result & 0x07) != 0x07) {
			if(result == (_BIT0 | _BIT1)) {
				delta += (_OFFSET_STEP / 2);
				rtd_pr_vbe_debug("\n FrameSync test3 : FrameSync Success(offset + _OFFSET_STEP)==>offset value=%x\n",delta);
			}
			else if(result == (_BIT0 | _BIT2)) {
				delta -= (_OFFSET_STEP / 2);
				rtd_pr_vbe_debug("\n FrameSync test3 : FrameSync Success(offset -_OFFSET_STEP)==>offset value=%x\n",delta);
			}
		}
	}
	drvif_clock_offset(delta);

	buffer = delta;

 	if(result == 0) {   // Fast search fail, do iteration search
 	       rtd_pr_vbe_debug("\nFrameSync test4 : Fast Frame sync => do iteration search  !\n");

		// Apply Dclk frequency setting
		//drvif_clock_offset(buffer);

		// Search first satisfied DCLK setting for frame-sync
		result  = 0x00;
		fine    = _SEARCH_TIME;
		do {
			if(framesync_testsync(buffer, &Data[0])){
				rtd_pr_vbe_debug("FrameSyncTestSync error4:  HS/VS wrong assert(b8052414)");
				return 2;
			}

			if(Data[0] == 0) break;

			if(Data[0] & 0x02){
				buffer  -= (result & 0x01) ? (_OFFSET_STEP / 2) : _OFFSET_STEP;  // overflow
				//rtd_pr_vbe_info("overflow! => offset= %x !!",buffer);
			}
			else{
				buffer  += (result & 0x02) ? (_OFFSET_STEP / 2) : _OFFSET_STEP;  // underflow
				//rtd_pr_vbe_info("underflow! => offset= %x !!",buffer);
			}
			result  = Data[0];  // �|�x�s�W�@���O over or under
		}while(--fine);

		if(fine == 0x00){
			rtd_pr_vbe_debug("\nFrameSync test4 : Iteration search fail! => Frame sync fail !! ");
			return 1;
		}
		else
			rtd_pr_vbe_debug("\nFrameSync test4 : Iteration search success! => search times = %x !!",_SEARCH_TIME - fine);

		// Search most satisfied DCLK setting for frame-sync
		rtd_pr_vbe_debug("\nFrameSync test6 : Search most satisfied DCLK setting for frame-sync ");
		delta = buffer;
		fine  =2; //4;
		do {
			result  = Data[0];
			//unsigned short temp = (result & 0x01) ? delta + (_OFFSET_STEP / 2) : delta - (_OFFSET_STEP / 2);
			temp = (result & 0x01) ? (delta + (unsigned short)_OFFSET_STEP): (delta - (unsigned short)_OFFSET_STEP);

			if(framesync_testsync(temp, &Data[0])){
				rtd_pr_vbe_debug("\nFrameSyncTestSync error6:  HS/VS wrong assert(b8052414)");
				return 2;
			}

			if(Data[0]){
				delta = buffer; // restore the original DClk offset setting
				rtd_pr_vbe_debug("\nrestore the original DClk offset setting");
				break;
			}
			else // CSW+ 0970214 Save safe frame sync offset value
				buffer = temp;
			delta   = temp;
		}while(--fine);


	}
	_FRAME_SYNC_DO_END:
	rtd_pr_vbe_debug("\nFrameSync test7 : FrameSync SpreadSpectrum Finetune ");
	result = framesync_spreadspectrumfinetune(&delta);
	if(result != 0)     return result;
	rtd_pr_vbe_debug("\nFrameSync test8 : FrameSync LastLine Finetune ");
	dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_reg);
	dclkss_reg.dclk_ss_load = _ENABLE;
	dclkss_reg.dclk_ss_en = _ENABLE;
	IoReg_Write32(PLL27X_REG_SYS_DCLKSS_reg, dclkss_reg.regValue);
	//clear status
	if(_CHANNEL1 == Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) )
	{
		vgip_chn1_status_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_STATUS_reg);
		vgip_chn1_status_reg.ch1_vs_err = TRUE;
		vgip_chn1_status_reg.ch1_hs_err = TRUE;
		vgip_chn1_status_reg.ch1_field_err = TRUE;
		vgip_chn1_status_reg.ch1_vact_start = 0;
		vgip_chn1_status_reg.ch1_vact_end = 0;
		IoReg_Write32(VGIP_VGIP_CHN1_STATUS_reg, vgip_chn1_status_reg.regValue);

		//Write Clear
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
		mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
		mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf = TRUE;
		mdomain_cap_ich12_ibuff_status_reg.fsync_ovf = TRUE;
		mdomain_cap_ich12_ibuff_status_reg.fsync_udf = TRUE;
		IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg, mdomain_cap_ich12_ibuff_status_reg.regValue);
#endif // SCALER_ERROR

	}

	return 0;
}


unsigned char drvif_decide_frc_fixlastline_enable(void)
{
	if ((Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC)) || (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_SUB_DISPLAY))
		return FALSE;

	//if (modestate_decide_double_dvs_enable())
	//	return FALSE;

	if ( (Scaler_InputSrcGetMainChType() == _SRC_VGA) /*||
		((GET_INPUTSOURCE_FROM() == _SRC_FROM_TMDS) &&(!drvif_Hdmi_FormatDetect())) */
		)
		return FALSE;

	return TRUE;
}

#ifdef CONFIG_FS_HS_BIAS
//--------------------------------------------------
// Description  : When HS Bias, IC will tracking automatically for frame sync
// Input Value  : None
// Output Value : None
//--------------------------------------------------
/**
* drvif_framesync_hs_bias
* limitation: Timing: FS, Data:FS
* This function will enable IC tracking mechanism when HS Bias for FrameSync
* @param <None>
* @return <None>
*/
#define GENERAL_LINE_NUM	8
unsigned char drvif_framesync_hs_bias(void)
{
	float decimal_array[8] = {0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0 , 0};
	float expect_value = 0;
	unsigned char array_index = 0, integer_part = 0, decimal_part = 0, M_code = 0, N_code = 0, K_code = 0, vco_div = 0, result;
	unsigned int ICLK = 0,general_standard_num = 0;
	pll_setM_N_Register_RBUS pll_setM_N_reg;
	pllphase_ctrl_RBUS pllphase_ctrl_reg;
	fsync_general_ctrl1_RBUS fsync_general_ctrl1_reg;
	fsync_general_ctrl2_RBUS fsync_general_ctrl2_reg;
	dpll_in_fsync_tracking_ctrl_RBUS dpll_in_fsync_tracking_ctrl_reg;
	sys_dclkss_RBUS dclkss_reg;
	//scaledown_ich1_ibuff_status_RBUS ich1_ibuff_status_reg;
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_cap_ich12_ibuff_status_RBUS mdomain_cap_ich12_ibuff_status_reg;
#endif // SCALER_ERROR

	//only YPbPr / CVBS framesync mode and Go VGIP channel1 can support this function
	if(SLR_MAIN_DISPLAY != Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY))
		return TRUE;

	if(!Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC))
		return;

	if((Scaler_InputSrcGetMainChType() != _SRC_YPBPR) && (Scaler_InputSrcGetMainChType() != _SRC_CVBS))
		return;

	rtd_pr_vbe_debug("Enter HS Bias\n");
	// 1.calculate  general standard num
	if(Scaler_InputSrcGetMainChType() == _SRC_YPBPR){
		pll_setM_N_reg.regValue = IoReg_Read32(DDS_PLL_PLL_SETM_N);
		M_code = pll_setM_N_reg.pll_m;
		N_code = pll_setM_N_reg.pll_n;
		K_code = pll_setM_N_reg.pllsphnext;

		pllphase_ctrl_reg.regValue = IoReg_Read32(DDS_PLL_PLLPHASE_CTRL_VADDR);
		if(pllphase_ctrl_reg.pll2d2)
		vco_div = 4;
		else
		vco_div = 2;

		// [Code Sync][LewisLee][0990211][1]
				// for K code rule
				// K > 7 ==> M+1
				// K < -8 ==> M-1
				//so we need return K to original value
		if(K_code > 7)
			M_code -= 1;
		// [Code Sync][LewisLee][0990211][1][End]


		ICLK = (27000000*(M_code+3) + 27000000*K_code/16)/((N_code+2)*vco_div);
		rtd_pr_vbe_debug("==>M:%d N:%d K:%d vco:%d ICLK:%d\n",M_code,N_code,K_code,vco_div,ICLK);
	}
	else{//vdc
		ICLK = 13500000;
	}

	ICLK /= 1000;
	general_standard_num = (GENERAL_LINE_NUM*Scaler_DispGetInputInfo(SLR_INPUT_H_LEN))*27000/ICLK;

	fsync_general_ctrl1_reg.regValue = IoReg_Read32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR);
	fsync_general_ctrl1_reg.fsync_general_standard_num = general_standard_num;
	fsync_general_ctrl1_reg.fsync_general_line_num = GENERAL_LINE_NUM - 1;
	IoReg_Write32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR, fsync_general_ctrl1_reg.regValue);
	rtd_pr_vbe_debug("[==>reg:%x]\n",IoReg_Read32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR));
	rtd_pr_vbe_debug("==>IHtotal:%d\n",Scaler_DispGetInputInfo(SLR_INPUT_H_LEN));
	rtd_pr_vbe_debug("==> general_standard_num :0x%x\n",general_standard_num);

	// 2.calculate delta ratio
	expect_value = 409600/(general_standard_num*12.5);
	rtd_pr_vbe_debug("==> ratio:%f\n",expect_value);
	//integer part
	integer_part = (unsigned char)expect_value;
	//decimal part
	expect_value -=  (float)integer_part;
	rtd_pr_vbe_debug("==> int:%d decimal:%f\n",integer_part,expect_value);
	for(array_index = 0; array_index<6; array_index++){
		if (	(ABS(expect_value, decimal_array[array_index])) <=
			(ABS(expect_value, (decimal_array[array_index+1] + decimal_array[array_index+2])))){
			decimal_part |= 1<<(5 - array_index);
			if(decimal_array[array_index] > expect_value)
				break;
			else
				expect_value -= decimal_array[array_index];
		}
	}

	rtd_pr_vbe_debug("==> integer_part0:0x%x decimal:0x%x\n", integer_part, decimal_part);

	fsync_general_ctrl2_reg.regValue = IoReg_Read32(PEAKING_FSYNC_GENERAL_CTRL2_VADDR);
	fsync_general_ctrl2_reg.fsync_general_delta_ratio = (integer_part<<6)|(decimal_part);
	IoReg_Write32(PEAKING_FSYNC_GENERAL_CTRL2_VADDR, fsync_general_ctrl2_reg.regValue);

	// 3.enable function
	fsync_general_ctrl1_reg.regValue = IoReg_Read32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR);
	fsync_general_ctrl1_reg.fsync_general_en = _ENABLE;
	IoReg_Write32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR, fsync_general_ctrl1_reg.regValue);

	dpll_in_fsync_tracking_ctrl_reg.regValue = IoReg_Read32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_VADDR);
	dpll_in_fsync_tracking_ctrl_reg.in_frame_tracking_general_en = _ENABLE;
	IoReg_Write32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_VADDR, dpll_in_fsync_tracking_ctrl_reg.regValue);

	dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_VADDR);
	dclkss_reg.dclk_ss_load = 1;
	IoReg_Write32(PLL27X_REG_SYS_DCLKSS_VADDR, dclkss_reg.regValue);

	dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_VADDR);
	dclkss_reg.dclk_ss_en = 1;
	IoReg_Write32(PLL27X_REG_SYS_DCLKSS_VADDR, dclkss_reg.regValue);
	rtd_pr_vbe_debug("==>value:%x reg:%x\n",  (integer_part<<7)|(decimal_part),IoReg_Read32(PEAKING_FSYNC_GENERAL_CTRL2_VADDR));

	msleep(40);
#ifdef SCALER_ERROR // Scaler fix compiler error for mac9q  //register change
	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);
	mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf = 1;
	mdomain_cap_ich12_ibuff_status_reg.fsync_ovf = 1;
	mdomain_cap_ich12_ibuff_status_reg.fsync_udf = 1;
	IoReg_Write32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg, mdomain_cap_ich12_ibuff_status_reg.regValue);
	msleep(40);

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	mdomain_cap_ich12_ibuff_status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_ICH12_IBUFF_STATUS_reg);

	if((TRUE == mdomain_cap_ich12_ibuff_status_reg.ibuff_ovf) || (TRUE == mdomain_cap_ich12_ibuff_status_reg.fsync_ovf))
	{
		rtd_pr_vbe_debug("drvif_framesync_hs_bias : overflow\n");
		return FALSE;
	}
	else if(TRUE == mdomain_cap_ich12_ibuff_status_reg.fsync_udf)
	{
		rtd_pr_vbe_debug("drvif_framesync_hs_bias : underflow\n");
		return FALSE;
	}
#endif // SCALER_ERROR

	return TRUE;
}

void drvif_framesync_hs_bias_release(void)
{
	fsync_general_ctrl1_RBUS fsync_general_ctrl1_reg;
	dpll_in_fsync_tracking_ctrl_RBUS dpll_in_fsync_tracking_ctrl_reg;
	sys_dclkss_RBUS dclkss_reg;


	fsync_general_ctrl1_reg.regValue = IoReg_Read32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR);
	fsync_general_ctrl1_reg.fsync_general_en = _DISABLE;
	IoReg_Write32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR, fsync_general_ctrl1_reg.regValue);

	dpll_in_fsync_tracking_ctrl_reg.regValue = IoReg_Read32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_VADDR);
	dpll_in_fsync_tracking_ctrl_reg.in_frame_tracking_general_en = _DISABLE;
	IoReg_Write32(PLL27X_REG_DPLL_IN_FSYNC_TRACKING_CTRL_VADDR, dpll_in_fsync_tracking_ctrl_reg.regValue);

	dclkss_reg.regValue = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_VADDR);
	dclkss_reg.dclk_ss_load = 1;
	IoReg_Write32(PLL27X_REG_SYS_DCLKSS_VADDR, dclkss_reg.regValue);
}


// [Code Sync][lewislee][0990224][1]
//lewis 20100224, it is short time solution, to prevent Dclk jitter, let panel abnormal
void drvif_framesync_hs_bias_check(void)
{
	fsync_general_ctrl1_RBUS fsync_general_ctrl1_reg;
	SP_MS1Stus_RBUS SP_MS1Stus_reg;
//	SP_MS2Stus_RBUS SP_MS2Stus_reg;

//	if(_MAIN_DISPLAY != info->display)
//		return;

	fsync_general_ctrl1_reg.regValue = IoReg_Read32(PEAKING_FSYNC_GENERAL_CTRL1_VADDR);

	if(_ENABLE == fsync_general_ctrl1_reg.fsync_general_en)
	{
		{
			SP_MS1Stus_reg.regValue = IoReg_Read32(SYNCPROCESSOR_SP_MS1STUS_VADDR);

			if(SP_MS1Stus_reg.on1_vs_per_to || SP_MS1Stus_reg.on1_vs_over_range)
				drvif_framesync_hs_bias_release();
		}

	}
}
// [Code Sync][lewislee][0990224][1][End]

#endif


unsigned int hardware_measure_dvtotal(void)
{

	//unsigned int DVTotal;
	//unsigned short measure_timeout;

	ppoverlay_measure_dvtotal_last_line_pixels_result1_RBUS measure_dvtotal_last_line_pixels_result1_reg;
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;

	/*WaitFor_DEN_STOP();*/ /*avoid blinking line*/
	fw_scaler_dtg_double_buffer_enable(1);
	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	display_timing_ctrl1_reg.disp_fsync_en = 1;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);
	fw_scaler_dtg_double_buffer_apply();
	//frank@0903 reduce time fw_scaler_dtg_double_buffer_enable(0);

	measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);
	measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_ctr = _DISABLE;
	measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_mode = 0;
	measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_2f = 0;
	IoReg_Write32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg, measure_dvtotal_last_line_pixels_result1_reg.regValue);

	msleep(50);

	measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);
	measure_dvtotal_last_line_pixels_result1_reg.measure_dvllpixels_ctr = _ENABLE;
	IoReg_Write32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg, measure_dvtotal_last_line_pixels_result1_reg.regValue);

	//////////////////////////////////////
	//By Gilbert(2009/5/13)
	//�קK�o��Ū��DVTotal���~�H���]�w���~.�H���ͳ̫�e���Q����
	//WaitFor_DEN_STOP();
	////////////////////////////////////
	msleep(200);

	if(_CHANNEL1 == Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))
	{
		WaitFor_IVS1_Done();
		measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);

	}
	#ifdef CONFIG_DUAL_CHANNEL
	else// if(_CHANNEL2 == info->channel)
	{
		WaitFor_IVS2_Done();
		measure_dvtotal_last_line_pixels_result1_reg.regValue = IoReg_Read32(PPOVERLAY_Measure_dvtotal_last_line_pixels_Result1_reg);

#ifdef CONFIG_WATCHDOG_ENABLE
			IoReg_Write32(TC_WR_TR_VADDR, _BIT0);//tick system WD, to avoid system reset
			IoReg_Write32(TC_WR_TR_VADDR, 0); //kick watchdog
#endif //#ifdef CONFIG_WATCHDOG_ENABLE
//		}
	}
	#endif


	return measure_dvtotal_last_line_pixels_result1_reg.dv_total_value_first;

}

#ifdef CONFIG_5_4_FS

//--------------------------------------------------
// Description  : For DVD data 24frame/sec, DVD player will convert 24frame/sec -> 60 frame/sec. (This operation will cause lag)
//			 For Timing: FS, Data:FRC, IC will re-arrange VS => generate VS: 48frame/sec
// Input Value  : None
// Output Value : None
//--------------------------------------------------
/**
* drvif_framesync_5_4
* limitation: Timing: FS, Data:FRC
* This function will solve lag for DVD data is 24frame/sec
* @param <None>
* @return <None>
*/
void drvif_framesync_5_4(void)
{
	unsigned char  div=1;
	unsigned short nMCode,nNCode,usDclkOffset;
	unsigned int temp_val = 0,DHTotal;
	sys_pll_disp1_RBUS reg_disp1;
	sys_pll_disp2_RBUS reg_disp2;
	fs54_ctrl_RBUS ppoverlay_fs54_ctrl_Reg;


	if((Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC) == TRUE)||((IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg)& _BIT26)==0))
		return;

	rtd_pr_vbe_debug("====  drvif_framesync_5_4 ====\n");
	IoReg_SetBits(PPOVERLAY_Display_Timing_CTRL1_reg, _BIT29);
	temp_val = IoReg_Read32(PPOVERLAY_DH_Total_Last_Line_Length_reg);
	DHTotal = ((temp_val >>16) & 0xffffff)+1;
	IoReg_Mask32(PPOVERLAY_FS54_DH_VADDR, ~0xfff0000, DHTotal<<16);

	//get DCLK setting
	reg_disp1.regValue = IoReg_Read32(PLL27X_REG_SYS_PLL_DISP1_reg);
	reg_disp2.regValue = IoReg_Read32(PLL27X_REG_SYS_PLL_DISP2_VADDR);
	nMCode = reg_disp1.dpll_m;
	nNCode = reg_disp1.dpll_n;
	div = reg_disp2.dpll_o;

	switch(div){
		default:
		case 0:
			div = 1;
			break;
		case 1:
			div = 2;
			break;
		case 2:
			div = 4;
			break;
		case 3:
			div = 8;
			break;
	}

	temp_val = IoReg_Read32(PLL27X_REG_SYS_DCLKSS_VADDR);
	usDclkOffset = (temp_val>>16) & 0xfff;

	//Dclk = 27M *(M+2)/(div*(N+2)*2)*(1- offset/2^15)
    	temp_val = ((CONFIG_CPU_XTAL_FREQ/128 * (nMCode+2)) / (div * (nNCode+2)*2))*128;
	rtd_pr_vbe_debug("dclk:%d\n",temp_val);
	temp_val = temp_val - (temp_val>>15)*usDclkOffset;
	rtd_pr_vbe_debug("M:%d N:%d div:%d offset:%d DCLK:%d\n", nMCode, nNCode, div, usDclkOffset, temp_val);

	//DVTotal = Dclk/(DHTotal*48)
	//if(temp_val % (DHTotal*48))
		temp_val = temp_val/(DHTotal*48) + 1;
	//else
	//	temp_val = temp_val/(DHTotal*48);
	rtd_pr_vbe_debug("DHTotal:%d DVTotal:%d\n", DHTotal, temp_val);
	IoReg_Mask32(PPOVERLAY_FS54_DV_VADDR, ~0xfff, temp_val);
	IoReg_Mask32(PPOVERLAY_FS54_DCLK_M_VADDR, ~0x3ff, nMCode);
	IoReg_Mask32(PPOVERLAY_FS54_DCLK_N_VADDR, ~0xff, nNCode);
#if 0
	IoReg_Write32(PPOVERLAY_FS54_CTRL_VADDR, 0x60001110);//enable double buffer => load in setting
	IoReg_Write32(PPOVERLAY_FS54_CTRL_VADDR, 0xE0001110);//enable 54FS function
#else
	ppoverlay_fs54_ctrl_Reg.regValue = IoReg_Read32(PPOVERLAY_FS54_CTRL_VADDR);
	ppoverlay_fs54_ctrl_Reg.fs54_dclk_en = 0;
	ppoverlay_fs54_ctrl_Reg.fs54_dclk_slow_modify_en = 0;
	ppoverlay_fs54_ctrl_Reg.fs54_pseudo_fsync_en = 0;
	ppoverlay_fs54_ctrl_Reg.fs54_timing_en = 1;
	ppoverlay_fs54_ctrl_Reg.fs54_vs_align = 1;
	ppoverlay_fs54_ctrl_Reg.fs54_force = 1;
	ppoverlay_fs54_ctrl_Reg.fs54_main_sub_sel = 0;
	ppoverlay_fs54_ctrl_Reg.fs54_frc_fastslow = 1;
	ppoverlay_fs54_ctrl_Reg.fs54_db_load = 1;
	IoReg_Write32(PPOVERLAY_FS54_CTRL_VADDR,ppoverlay_fs54_ctrl_Reg.regValue);

	ppoverlay_fs54_ctrl_Reg.fs54_en = 1;
	IoReg_Write32(PPOVERLAY_FS54_CTRL_VADDR,ppoverlay_fs54_ctrl_Reg.regValue);
#endif
}

void drvif_framesync_5_4_release(void)
{
	fs54_ctrl_RBUS ppoverlay_fs54_ctrl_Reg;
	ppoverlay_fs54_ctrl_Reg.regValue = IoReg_Read32(PPOVERLAY_FS54_CTRL_VADDR);
	ppoverlay_fs54_ctrl_Reg.fs54_en = 0;
	IoReg_Write32(PPOVERLAY_FS54_CTRL_VADDR,ppoverlay_fs54_ctrl_Reg.regValue);
}
#endif

#define VGIP_VSYNC_DELAY 3
#define MULTIPLE 100
#define VO_VSTART_DEFAULT 20;
extern SCALER_DATA_FS_ALGO_RESULT g_tDataFsAlgoResult;
extern void UpdateDataFsAlgoRsult(void);
void scaler_update_vo_vstat_htotal(unsigned char a_bEnableVCrop, unsigned char a_bUpdateReg)
{
	vodma_vodma_v1sgen_RBUS  vodma_vodma_v1sgen_reg;
	vodma_vodma_v1vgip_vact1_RBUS vodma_vodma_v1vgip_vact1_reg;
	//vodma_vodma_v1vgip_vact2_RBUS vodma_vodma_v1vgip_vact2_reg;
	//vodma_vodma_v1vgip_vbg1_RBUS vodma_vodma_v1vgip_vbg1_reg;
	//vodma_vodma_v1vgip_vbg2_RBUS vodma_vodma_v1vgip_vbg2_reg;
	//vodma_vodma_reg_db_ctrl_RBUS vodma_vodma_reg_db_ctrl_reg;
	vodma_vodma_v1vgip_ivs1_RBUS    vodma_vodma_v1vgip_ivs1_reg;
	ppoverlay_dh_total_last_line_length_RBUS ppoverlay_dh_total_last_line_length_reg;
	ppoverlay_dv_den_start_end_RBUS ppoverlay_dv_den_start_end_reg;
	ppoverlay_dh_den_start_end_RBUS ppoverlay_dh_den_start_end_reg;
	ppoverlay_fs_iv_dv_fine_tuning2_RBUS ppoverlay_fs_iv_dv_fine_tuning2_reg;
	ppoverlay_fs_iv_dv_fine_tuning5_RBUS ppoverlay_fs_iv_dv_fine_tuning5_reg;
	ppoverlay_dctl_iv2dv_ihs_ctrl_RBUS ppoverlay_dctl_iv2dv_ihs_ctrl_reg;
	iedge_smooth_edgesmooth_ex_ctrl_RBUS edgesmooth_ex_ctrl_reg;
	vodma_vodma_smooth_toggle_RBUS vodma_smooth_toggle_reg;
	vgip_smooth_tog_ctrl_RBUS smooth_tog_ctrl_reg;
	hdr_all_top_top_d_buf_RBUS top_d_buf_reg;
	hdr_all_top_top_pic_size_RBUS top_pic_size_reg;
	hdr_all_top_top_pic_total_RBUS top_pic_total_reg;
	hdr_all_top_top_pic_sta_RBUS top_pic_sta_reg;

	unsigned int ulVoHtotal = 0, ulVoVStart = 0, ulVoVEnd = 0, /*ulVoActiveheight = 0,*/ ulVoVsyncStart = 0;
	unsigned long long ullparam1 =0;
	unsigned long ulparam2 =0;

	unsigned int ulDHTotal = 0, ulDVStar = 0, ulDVAct = 0, ulDHStar = 0;
	unsigned int ulVCropSize = 0;
	unsigned int ulDClk = 0, ulVoClk = 0;
	unsigned long long ulTemp1 = 0, ulTemp2 = 0, ulTemp4 = 0;
	unsigned int iv2dv_line = 0;
	unsigned int iv2dv_pixel = 0;
	unsigned int ulIv2Dv_delay = 0;
	unsigned int vo_start_offset = 0;
	unsigned int remove = 0, multiple = 0;
	unsigned int ratio_n_off = 0;

	unsigned long long ullTemp3 = 0;

	unsigned int ulDisableEdgeSmooth = 0;

	edgesmooth_ex_ctrl_reg.regValue = rtd_inl(IEDGE_SMOOTH_EdgeSmooth_EX_CTRL_reg);
	ulDisableEdgeSmooth = edgesmooth_ex_ctrl_reg.lbuf_linemode?0:1;

	vodma_vodma_v1vgip_ivs1_reg.regValue = rtd_inl(VODMA_VODMA_V1VGIP_IVS1_reg);

	ulVoVsyncStart = vodma_vodma_v1vgip_ivs1_reg.v_st;

	if( get_panel_pixel_mode() == PANEL_1_PIXEL )
		ulDClk = Get_DISPLAY_CLOCK_TYPICAL() / 1000;
	else
		ulDClk = Get_DISPLAY_CLOCK_TYPICAL() / 500;

	remove = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_REMOVE_RATIO);
	multiple = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_MULTIPLE_RATIO);

	if( multiple > 0 )
		ulDClk = ulDClk * remove / multiple;
	else
		rtd_pr_vbe_emerg("[Warning ]Wrong multiple value %d!!!\n", multiple);

	ulVoClk =(VO_Get_vodmapll() / 1000);
	ratio_n_off = VODMA_get_datafs_increased_clock(VO_VIDEO_PLANE_V1);
	ulVoClk = ulVoClk * (64 - ratio_n_off) / 64;

	//Scaler_DispGetInputInfoByDisp(SLR_INPUT_IPH_ACT_WID);
	//Scaler_DispGetInputInfoByDisp(SLR_INPUT_IPV_ACT_LEN);

	//ulVCropSize = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN);
	ulVCropSize = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_SDNRV_ACT_LEN);//main_input_size.src_height;

	ulDVAct = Scaler_DispWindowGet().src_height;

	if (0 == ulVCropSize || 0 == ulDVAct)
	{
		rtd_pr_vbe_emerg("Warning, scaler_update_vo_vstat_htotal(), ulVCropSize=%d, ulDVAct=%d\n", ulVCropSize, ulDVAct);

		if (0 == ulDVAct)
		{
			ulDVAct = Get_DISP_ACT_END_VPOS() - Get_DISP_ACT_STA_VPOS();
			rtd_pr_vbe_emerg("Set ulDVAct = DISP_LEN=%d\n", ulDVAct);
		}
	}
	//=========================================================================
	//=========================No crop===============================================
	if (FALSE == a_bEnableVCrop)
		ulVCropSize  = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN);//ulDVAct;

	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY,SLR_DISP_INTERLACE) == TRUE)
		ulVCropSize *= 2;

	ppoverlay_dv_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg);
	ulDVStar = ppoverlay_dv_den_start_end_reg.dv_den_sta;
	ulDVStar = ulDVStar+Scaler_DispWindowGet().srcy;

	ppoverlay_dh_den_start_end_reg.regValue = rtd_inl(PPOVERLAY_DH_DEN_Start_End_reg);
	ulDHStar = ppoverlay_dh_den_start_end_reg.dh_den_sta;

	ppoverlay_dh_total_last_line_length_reg.regValue = rtd_inl(PPOVERLAY_DH_Total_Last_Line_Length_reg);
	ulDHTotal =ppoverlay_dh_total_last_line_length_reg.dh_total + 1;

	vodma_vodma_v1sgen_reg.regValue = rtd_inl(VODMA_VODMA_V1SGEN_reg);
	ulVoHtotal = vodma_vodma_v1sgen_reg.h_thr + 1;

	vodma_vodma_v1vgip_vact1_reg.regValue = rtd_inl(VODMA_VODMA_V1VGIP_VACT1_reg);

	ulVoVStart = VO_VSTART_DEFAULT;
	ulVoVEnd = ulVoVStart + ulVCropSize;

	rtd_pr_vbe_emerg("old vo htotal=%d, vstart=%d, vend=%d, VoClk=%d, ulVoVsyncStar=%d\n",
									ulVoHtotal, ulVoVStart, ulVoVEnd, ulVoClk, ulVoVsyncStart);

	rtd_pr_vbe_emerg("DHTotal=%d, DVStar=%d, ulDHStar=%d, DVAct=%d, ulDClk=%d\n",
									ulDHTotal, ulDVStar, ulDHStar, ulDVAct, ulDClk);

	rtd_pr_vbe_emerg("ulVCropSize=%d, ulDisableEdgeSmooth=%d\n", ulVCropSize, ulDisableEdgeSmooth);

	//ulVoActiveheight = ulVoVEnd - ulVoVStart;

	ulVoVEnd = ulVoVStart + ulVCropSize;

	ulTemp1 = ulTemp4 = (unsigned long long)ulVoClk * (unsigned long long)ulDHTotal * (unsigned long long)ulDVAct;
	ulTemp2 = (unsigned long long)ulDClk * (unsigned long long)ulVCropSize;

	rtd_pr_vbe_emerg("ulTemp1=%lld, ulTemp2=%lld\n", ulTemp1, ulTemp2);

	//ulVoHtotal = ulVoClk * ulDHTotal * ulDVAct / (ulDClk * ulVCropSize);
	do_div(ulTemp4, ulTemp2);
	ulVoHtotal = (unsigned int)ulTemp4;
	if (ulVoHtotal*ulTemp2 < ulTemp1)
		ulVoHtotal++;

	rtd_pr_vbe_emerg("New vo htotal=%d, vstart=%d, vend=%d, v_sync start=%d\n",
								ulVoHtotal, ulVoVStart, ulVoVEnd, ulVoVsyncStart);

	// Update VO Htotal
	if (TRUE == a_bUpdateReg)
	{
		vodma_vodma_v1sgen_reg.h_thr = ulVoHtotal - 1;
		rtd_outl(VODMA_VODMA_V1SGEN_reg, vodma_vodma_v1sgen_reg.regValue);

		#if 1
		vodma_smooth_toggle_reg.regValue=rtd_inl(VODMA_VODMA_SMOOTH_TOGGLE_reg);
		//vodma_smooth_toggle_reg.smooth_en=_ENABLE;
		vodma_smooth_toggle_reg.vcnt_pre_num= ulVoHtotal - 1;
		rtd_outl(VODMA_VODMA_SMOOTH_TOGGLE_reg,vodma_smooth_toggle_reg.regValue);
		#endif
	}
	else
	{
	}

	rtd_pr_vbe_emerg("update h total 0xb8005078=%x\n", rtd_inl(VODMA_VODMA_V1SGEN_reg));

	// Calc vo_vstart and iv2dv_delay
	//vo_vstart - iv2dv_delay = (((((ulDVStar*ulDHTotal + ulDHStar) / ulDClk) + 350) * ulVoClk + 1920) / ulVoHtotal) + ulVoVsyncStart + VGIP_VSYNC_DELAY - 4 - 0.5;

	ullTemp3 = ((unsigned long long)(ulDVStar*ulDHTotal + ulDHStar) * ulVoClk * MULTIPLE);

	do_div(ullTemp3,  ulDClk);

	//rtd_pr_vbe_emerg("0  dTemp3=%lld\n", ullTemp3);

	//ullTemp3 = ullTemp3 +350 * ulVoClk * MULTIPLE/1000 + 1920 * MULTIPLE;
	ullTemp3 = ullTemp3 +350 * ulVoClk /10000 + 1920 * MULTIPLE;

	//rtd_pr_vbe_emerg("1  dTemp3=%lld\n", ullTemp3);

	do_div(ullTemp3, ulVoHtotal);

	//rtd_pr_vbe_emerg("2  dTemp3=%lld\n", ullTemp3);

	//ullTemp3 = ullTemp3 + (ulVoVsyncStart + VGIP_VSYNC_DELAY - 4) * MULTIPLE - 5 * MULTIPLE / 10;
	ullTemp3 = ullTemp3 + (ulVoVsyncStart + VGIP_VSYNC_DELAY - 4 + ulDisableEdgeSmooth) * MULTIPLE - 5 * MULTIPLE / 10;

	rtd_pr_vbe_emerg("dTemp3=%lld\n", ullTemp3);

	if(ulVCropSize < 1100)  //2k
		vo_start_offset = 1;
	else if(ulVCropSize < 1460) //2560*1440
		vo_start_offset = 6;
	else
		vo_start_offset = 2;
	if(vsc_get_adaptivestream_flag(SLR_MAIN_DISPLAY))
	{
		if(ulVCropSize < 1100)  //2k
			vo_start_offset = 1;
		else if(ulVCropSize < 1460) //2560*1440
			vo_start_offset = 5;
		else if(ulVCropSize > 1500){
			if( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) >= V_FREQ_24000_mOFFSET ) && ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) < V_FREQ_24500 ) )	// 24Hz
				vo_start_offset = 4;
			else
				vo_start_offset = 2;
		}
	}


	if (ullTemp3 > ulVoVStart * MULTIPLE)
	{
		ullparam1 = ullTemp3 - ulVoVStart * MULTIPLE;
		ulparam2 = MULTIPLE;

		do_div(ullparam1 , ulparam2);
		ulVoVStart =  ulVoVStart + (unsigned int)ullparam1 - vo_start_offset;

		//rtd_pr_vbe_emerg("new ulVoVStart=%d\n", ulVoVStart);

		ulIv2Dv_delay = (ulVoVStart + vo_start_offset + 2) * MULTIPLE - ullTemp3;

		//rtd_pr_vbe_emerg("ulIv2Dv_delay=%d/%d\n", ulIv2Dv_delay, MULTIPLE);

		iv2dv_line = ulIv2Dv_delay / MULTIPLE;

		iv2dv_pixel = (ulIv2Dv_delay - iv2dv_line*MULTIPLE)*ulVoHtotal / MULTIPLE;
	}
	else
	{
		// ToDo
	}

	ulVoVEnd = ulVoVStart + ulVCropSize;

	rtd_pr_vbe_emerg("new vo htotal=%d, ulVoVStart=%d\n", ulVoHtotal, ulVoVStart);
#if 0
	rtd_pr_vbe_emerg("iv2dv_line=%d, ullparam1=%lld, ulparam2=%d\n", iv2dv_line, ullparam1, (unsigned int)MULTIPLE);
	rtd_pr_vbe_emerg("iv2dv_pixel=%d, ullparam1=%lld, ulparam2=%d\n", iv2dv_pixel, ullparam1, (unsigned int)MULTIPLE);
#endif

	//rtd_pr_vbe_emerg("iv2dv_line=%d, iv2dv_pixel=%d\n", iv2dv_line, iv2dv_pixel);

	if (TRUE == a_bUpdateReg)
	{
		// 0xb8025c50
		ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue = rtd_inl(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg);
		ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel = iv2dv_pixel;
		ppoverlay_fs_iv_dv_fine_tuning2_reg.iv2dv_pixel2 = iv2dv_pixel;
		rtd_outl(PPOVERLAY_FS_IV_DV_Fine_Tuning2_reg, ppoverlay_fs_iv_dv_fine_tuning2_reg.regValue);

		// 0xb8025c58
		ppoverlay_fs_iv_dv_fine_tuning5_reg.regValue = rtd_inl(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
		ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line = iv2dv_line;
		ppoverlay_fs_iv_dv_fine_tuning5_reg.iv2dv_line_2= iv2dv_line;
		rtd_outl(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, ppoverlay_fs_iv_dv_fine_tuning5_reg.regValue);

		//0xb8025c64
		ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue = rtd_inl(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg);
		ppoverlay_dctl_iv2dv_ihs_ctrl_reg.iv2dv_ih_slf_width = ulVoHtotal;
		rtd_outl(PPOVERLAY_dctl_iv2dv_ihs_ctrl_reg, ppoverlay_dctl_iv2dv_ihs_ctrl_reg.regValue);

		// Update VO VStart, VEnd
		vodma_vodma_v1vgip_vact1_reg.v_st = ulVoVStart;
		vodma_vodma_v1vgip_vact1_reg.v_end = ulVoVEnd;
		rtd_outl(VODMA_VODMA_V1VGIP_VACT1_reg, vodma_vodma_v1vgip_vact1_reg.regValue);
#if 1
		// HDR
		top_pic_total_reg.regValue = rtd_inl(HDR_ALL_TOP_TOP_PIC_TOTAL_reg);
		top_pic_sta_reg.regValue = rtd_inl(HDR_ALL_TOP_TOP_PIC_STA_reg);
		top_pic_size_reg.regValue = rtd_inl(HDR_ALL_TOP_TOP_PIC_SIZE_reg);

		top_pic_total_reg.dolby_h_total = ulVoHtotal;
		top_pic_sta_reg.dolby_v_den_sta = ulVoVStart;
		top_pic_size_reg.dolby_vsize = ulVoVEnd - ulVoVStart;

		rtd_outl(HDR_ALL_TOP_TOP_PIC_SIZE_reg, top_pic_size_reg.regValue);
		rtd_outl(HDR_ALL_TOP_TOP_PIC_TOTAL_reg, top_pic_total_reg.regValue);
		rtd_outl(HDR_ALL_TOP_TOP_PIC_STA_reg, top_pic_sta_reg.regValue);
        smooth_tog_ctrl_reg.regValue = rtd_inl(VGIP_Smooth_tog_ctrl_reg);
        if(smooth_tog_ctrl_reg.main_imd_smooth_toggle_style ==1)//VGIP need to set HDR db_apply
        {
            top_d_buf_reg.regValue = rtd_inl(HDR_ALL_TOP_TOP_D_BUF_reg);
            top_d_buf_reg.dolby_double_apply = 1;//
            rtd_outl(HDR_ALL_TOP_TOP_D_BUF_reg, top_d_buf_reg.regValue);
        }
		//rtd_pr_vbe_info("HDR htotal=%d, v_den_sta=%d, dolby_vsize=%d\n", top_pic_total_reg.dolby_h_total, top_pic_sta_reg.dolby_v_den_sta, top_pic_size_reg.dolby_vsize);
#endif
	}
	else
	{
	}
}
#endif // #ifndef UT_flag
