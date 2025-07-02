#include <linux/delay.h>
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <rtk_kdriver/RPCDriver.h>
#include <linux/pageremap.h>
#include <uapi/linux/const.h>
#include <linux/mm.h>
//#include <mach/io.h>-
#include "tvscalercontrol/scaler/scalerstruct.h"
#include "tvscalercontrol/scalerdrv/pipmp.h"
#include "tvscalercontrol/scalerdrv/scalerdrv.h"
#include "tvscalercontrol/scalerdrv/scalerip.h"
///#include <tvscalercontrol/scaler/scalerlib.h>
#include <tvscalercontrol/scaler/state.h>//pthread_mutex_t			scalerMutex;				// scaler mutex
#include <tvscalercontrol/io/ioregdrv.h>
#include <scaler/vipCommon.h>
#include <tvscalercontrol/vip/nr.h>//drvif_color_iEdgeSmooth_init
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/vip/ultrazoom.h>
#include "tvscalercontrol/scalerdrv/scalerdisplay.h"
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <tvscalercontrol/scalerdrv/scalerclock.h>//drvif_clock_set_pll rika 20140616
#include <tvscalercontrol/scalerdrv/mode.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>//for drvif_Hdmi_GetInterlace
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include "tvscalercontrol/panel/panelapi.h"//rika need review
#include <tvscalercontrol/scalerdrv/framesync.h>
#ifdef CONFIG_ENABLE_VD_27MHZ_MODE
#include <tvscalercontrol/scaler/scalervideo.h>
#endif
#include "tvscalercontrol/scalerdrv/adjust.h"
#include "scaler_vscdev.h"
#include <tvscalercontrol/scaler/scalercolorlib.h>
#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
#include <tvscalercontrol/scalerdrv/scaler_pst.h>
//below are rbus files
#include "rbus/vodma_reg.h"
#include <rbus/vgip_reg.h>
#include <rbus/sub_vgip_reg.h>
#include <rbus/di_reg.h>	//#include <rbus/rbusDiReg.h>
//#include <rbus/hdmi_clkdet_reg.h>	//#include <rbus/rbusHDMIReg.h>
//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdomain_disp_reg.h>
#include <rbus/mdom_sub_cap_reg.h>
#include <rbus/mdom_sub_disp_reg.h>
//#include <rbus/scaler/rbusMac2_I_Edge_SmoothReg.h>///#include <rbusIedge_smoothReg.h>
#include <rbus/iedge_smooth_reg.h>	//#include <rbus/rbusIedge_smoothReg.h>
#include <rbus/scaledown_reg.h>
#include <rbus/scaleup_reg.h>	//#include <rbus/rbusScaleupReg.h>
//#include <rbusHsd_ditherReg.h>
///#include <rbusSYSTEMReg.h>///not in mac3? rika 20140924 //mac3check
#include <rbus/histogram_reg.h>	//#include <rbus/rbusHistogramReg.h>
#include <rbus/dma_vgip_reg.h>
#include <rbus/hsd_dither_reg.h>	//#include <rbus/rbusHsd_ditherReg.h>
#include <vo/rtk_vo.h>
#include <rbus/sys_reg_reg.h>	//#include <rbus/crt_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rbus/pll_reg_reg.h>
#include <rbus/onms_reg.h>
#include <rbus/rgb2yuv_reg.h>
#include <rtd_log/rtd_module_log.h>
#include <vo/rtk_vo.h>
#include <rbus/timer_reg.h>

extern struct semaphore* get_vsc_semaphore(void);
extern struct semaphore* get_force_i3ddma_semaphore(void);
extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr;
extern KADP_VIDEO_RECT_T air_mode_save_outregion_parm[MAX_DISP_CHANNEL_NUM];
extern void zoom_smoothtoggle_display_only_shift_xy(unsigned char display);
extern void scaler_airModeSendDispSize(unsigned char display, KADP_VIDEO_RECT_T outregion);
#ifndef UT_flag
extern void h3ddma_nn_set_recfg_flag(unsigned char bVal);
extern unsigned int scalerAI_get_AIPQ_mode_enable_flag(void);
#endif // #ifndef UT_flag

#define disablepart 0
#define _VGIP_V_FRONT_PORCH_MIN	0x05
//#define _VGIP_H_FRONT_PORCH_MIN	0x03
#define _VGIP_H_FRONT_PORCH_MIN	0x07
//#define _H_POSITION_MOVE_TOLERANCE      0x50 // +- 80 pixels //need review
//#define _H_POSITION_MOVE_TOLERANCE      0xa0 // +- 160 pixels
#define _H_POSITION_MOVE_TOLERANCE      0x80 // +- 128 pixels
#define _V_POSITION_MOVE_TOLERANCE      0x32 // 50 lines
#define VO_FIXED_2K1K_HTOTAL 2480
#define VO_FIXED_4K2K_HTOTAL 4400
#ifdef ENABLE_DRIVER_I3DDMA
	#undef ENABLE_DRIVER_I3DDMA
#endif
//#define CONFIG_SMOOTH_PROFILE
#undef rtd_outl
#define rtd_outl(x, y)     								IoReg_Write32(x,y)
#undef rtd_inl
#define rtd_inl(x)     									IoReg_Read32(x)
#undef rtd_maskl
#define rtd_maskl(x, y, z)     							IoReg_Mask32(x,y,z)
#undef rtd_setbits
#define rtd_setbits(offset, Mask) rtd_outl(offset, (rtd_inl(offset) | Mask))
#undef rtd_clearbits
#define rtd_clearbits(offset, Mask) rtd_outl(offset, ((rtd_inl(offset) & ~(Mask))))
#define Rt_Sleep(mSec)					msleep(mSec)
#define Rt_Delay(mSec)					msleep(mSec)

SMOOTH_TOGGLE_INFO imdSmoothInfo[2]={{0},{0}};
SLR_RATIO_TYPE ratio_type=SLR_RATIO_DISABLE;
SLR_RATIO_TYPE ratio_type_pre;
StructSrcRect custom_dispwin = {0, 0, 0, 0};

unsigned char smooth_toggle_dclkmax_zoom = 0;
unsigned int ratiotype_state=RT_STATE_INIT;
unsigned int display_h_total=0;
unsigned int display_v_total=0;
unsigned int display_clock_max=0;
unsigned int display_clock_min=0;
unsigned int display_clock_typical=0;
unsigned int display_den_start_vpos=0;
unsigned int display_act_start_vpos=0;
unsigned int display_act_end_vpos=0;
unsigned int display_act_start_hpos=0;
unsigned int display_act_end_hpos=0;
unsigned int drop_v_len[MAX_DISP_CHANNEL_NUM];
unsigned int drop_v_position[MAX_DISP_CHANNEL_NUM];
unsigned char frame_rate_change_reduce;
unsigned char smooth_toggle_update_flag=0;

#if defined(IS_ARCH_ARM_COMMON)
#define ONE_MILISECOND_CLOCK 93500
#elif defined(IS_DARWIN)
#define ONE_MILISECOND_CLOCK 53300
#else //Mac
#define ONE_MILISECOND_CLOCK 28000
#endif

extern unsigned char vbe_disp_oled_orbit_enable;
extern unsigned char vbe_disp_oled_orbit_mode;

unsigned char  pre_linein=0;
unsigned char  pre_volinein=0;
//DISPLAY_DATA_SMOOTH_TOGGLE pre_vo_data_mode=SLR_DISPLAY_DEFAULT;
SOURCE_INFO_SMOOTH_TOGGLE  pre_smooth_state=SLR_SMOOTH_TOGGLE_NONE;
//unsigned int  apply_windowId=0;
unsigned char apply_hal_input_type[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_crop_hpos[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_disp_hpos[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_crop_vpos[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_disp_vpos[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_crop_hsize[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_disp_hsize[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_crop_vsize[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_disp_vsize[MAX_DISP_CHANNEL_NUM];
//unsigned char apply_zoomfunc=0;
unsigned int  apply_timing_hpos[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_timing_vpos[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_timing_hsize[MAX_DISP_CHANNEL_NUM];
unsigned int  apply_timing_vsize[MAX_DISP_CHANNEL_NUM];
unsigned int apply_crop_vsize_pre[MAX_DISP_CHANNEL_NUM];
unsigned int apply_crop_vpos_pre[MAX_DISP_CHANNEL_NUM];
unsigned char check_only_xy_shift_change_to_videofw[MAX_DISP_CHANNEL_NUM];
unsigned char fInputRegionSame[MAX_DISP_CHANNEL_NUM]={false};// = {false,false};
unsigned char fOutputRegionSame[MAX_DISP_CHANNEL_NUM]={false};// = {false,false};
unsigned char m_film_clear = 0;
int m_film = 0;
unsigned char g_fw_film_en = 1;
StructSrcRect newInput;

static unsigned char resetcropflag=0;
static unsigned char resetdispflag=0;
static unsigned int	zoom_crop_hpos[MAX_DISP_CHANNEL_NUM];
static unsigned int	zoom_crop_vpos[MAX_DISP_CHANNEL_NUM];
static unsigned int	zoom_crop_hsize[MAX_DISP_CHANNEL_NUM];
static unsigned int	zoom_crop_vsize[MAX_DISP_CHANNEL_NUM];
static unsigned int  zoom_disp_hpos[MAX_DISP_CHANNEL_NUM];
static unsigned int  zoom_disp_vpos[MAX_DISP_CHANNEL_NUM];
static unsigned int  zoom_disp_hsize[MAX_DISP_CHANNEL_NUM];
static unsigned int  zoom_disp_vsize[MAX_DISP_CHANNEL_NUM];

unsigned char is_di_h_scalar_down(unsigned char display);
int zoom_check_size_error(unsigned char display);
int zoom_imd_smooth_set_info(unsigned char display, unsigned char rcflag,unsigned char rdflag);
void zoom_imd_smooth_color_ultrazoom_config_scaling_up(unsigned char display);
void zoom_imd_smooth_color_ultrazoom_setscaleup(unsigned char display, SMOOTH_TOGGLE_SIZE* ptInSize, SMOOTH_TOGGLE_SIZE* ptOutSize, unsigned char panorama);

int zoom_imd_smooth_toggle_vo_proc
(unsigned char display,   unsigned int  crop_hpos, unsigned int  crop_vpos,
    unsigned int  crop_hsize, unsigned int  crop_vsize,
    unsigned int  disp_hpos, unsigned int  disp_vpos,
    unsigned int  disp_hsize, unsigned int  disp_vsize    );


extern unsigned short shape_wide,shape_high;
//extern
#if disablepart //to fix compile error for kenel rika 20140929
extern pthread_mutex_t 		scalerMutex;
#endif
extern bool drvif_color_get_scaleup_cuthor4line(void);
extern bool drvif_color_get_scaleup_cutver2line(void);
//extern void set_di_doublebuf_write(unsigned char diflag,unsigned char _2d_flag);
extern void vo_smoothtoggle_memory_alloc(unsigned char display);
extern void Check_smooth_toggle_update_flag(unsigned char display);
extern SOURCE_INFO_SMOOTH_TOGGLE get_vo_smoothtoggle_state(unsigned char display);
//extern unsigned char Scaler_Get3DMode(void);
extern VSC_INPUT_TYPE_T Get_DisplayMode_Src(unsigned char display);
//------------end of zoom fpp api
static unsigned char vo_zoom_st_enable[MAX_DISP_CHANNEL_NUM];
extern unsigned int smooth_toggle_game_mode_timeout_trigger_flag;
extern unsigned int smooth_toggle_game_mode_check_cnt;

unsigned int DISP_WID=1920, DISP_LEN=1080;
unsigned int INPUT_WID=0,INPUT_LEN=0;

#define REDUCE_DI_BANDWIDTH_LIMIT_THRESHOLD 	1920//960//1440
#define VODMACLK_REDUCE_PANEL_LENGTH_THRESHOLD 		800 //for 1366X768 panel

#define VUZD_BUFFER_FRAMESYNC_MODE			_BIT0
#define VUZD_BUFFER_VUZD_MODE				_BIT1
#define VUZD_BUFFER_BYPASS_MODE				0
#define DI_COLOR_BIT_NUM_C					8
#define DI_DMA_420_EN_START_BIT			30
#define DI_DMA_420_EN_MASK				(0x3 << DI_DMA_420_EN_START_BIT)
#define DI_DMA_LINESIZE_EVEN_START_BIT	12
#define DI_DMA_LINESIZE_EVEN_MASK			(0x7ff << DI_DMA_LINESIZE_EVEN_START_BIT)
#define DI_DMA_LINESIZE_ODD_START_BIT		0
#define DI_DMA_LINESIZE_ODD_MASK			(0x7ff << DI_DMA_LINESIZE_ODD_START_BIT)

static bool di_hsd_flag = FALSE;
#define MAC2_HSD_DI_LINE_BUF_PIX_LIMIT 960
static unsigned short di_n1_seg1,di_n1_seg2;
static unsigned int bIpBtrMode=_ENABLE, bIpDma420Mode=_DISABLE;

#define CONFIG_DDR_COL_BITS 9
#define _DDR_BITS						CONFIG_DDR_COL_BITS
#define _DUMMYADDR					0xFFFFFFFF

unsigned int voscalermemory_endaddr,voscalermemory_startaddr,voscalermemory_size;
unsigned int vodimemory_startaddr,vo_main_third_startaddr,vo_main_sec_startaddr,vo_main_fisrt_startaddr;
unsigned int vodimemory_size,vomain_size;

#ifdef CONFIG_SMOOTH_PROFILE
unsigned int before_time;
unsigned int after_time;
unsigned char beforefunc=0,afterfunc=1;

void zoom_profile(unsigned char thefunc)
{
	if(thefunc==beforefunc){
		before_time=IoReg_Read32(SCPU_CLK90K_LO_reg)/90;
	}
	else if (thefunc==afterfunc){
			after_time=IoReg_Read32(SCPU_CLK90K_LO_reg)/90;
			rtd_pr_smt_emerg("profile time=%d ms\n", after_time-before_time);
	}
}
#endif

unsigned char vo_zoom_go_smooth_toggle_get(unsigned char display)
{
	return vo_zoom_st_enable[display];
}
void vo_zoom_go_smooth_toggle_set(unsigned char enable,unsigned char display)
{
	vo_zoom_st_enable[display]=enable;
}

void Set_Val_new_input_region(StructSrcRect InputRegion)
{
    newInput = InputRegion;
}

StructSrcRect Get_Val_new_input_region(void)
{
    return newInput;
}

void smooth_toggle_decide_DI(unsigned char display)
{//change to zoom_modestate_decide_DI
	//imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_DISP_10BIT =0 ; //vo 8bit //rika 20140609 need review
	// Elsie 20131106: playback always does not use RTNR 10-bit mode.
	// 20131210 sync from Mac2
	UINT32 picWidth = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE;
	unsigned char src = Scaler_InputSrcGetType(display);
	imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT=Scaler_DispGetStatus(display, SLR_DISP_10BIT);
#if 0
	if(display==SLR_MAIN_DISPLAY) {
		fw_scalerip_set_DI_chroma_10bits(TRUE);
	}
	#ifdef CONFIG_DUAL_CHANNEL
	else {
		fw_scalerip_set_DI_chroma_10bits(FALSE);
	}
	#endif
#endif

#if 1//def REDUCE_DIBW_ENABLE
#ifdef CONFIG_ENABLE_VD_27MHZ_MODE
		//USER:LewisLee DATE:2012/12/20
		//fix VDC 27MHz mode, picture abnormal
		if(VD_27M_OFF != fw_video_get_27mhz_mode((SCALER_DISP_CHANNEL)display))
		{
			picWidth = picWidth>>1;
		}
#endif //#ifdef CONFIG_ENABLE_VD_27MHZ_MODE

	//Elsie 20131210: Enable BTR mode when source is 1080i. 20140211: Always disable BTR when source is playback.
	if(src != _SRC_VO && (imdSmoothInfo[display].IMD_SMOOTH_MODE_CURR == _MODE_1080I25 || imdSmoothInfo[display].IMD_SMOOTH_MODE_CURR == _MODE_1080I30))
	{
		bIpBtrMode=_ENABLE;
		bIpDma420Mode= SLR_DMA_422_mode;

	}
	else if (picWidth > REDUCE_DI_BANDWIDTH_LIMIT_THRESHOLD)
	{
		bIpBtrMode=_DISABLE;

		if(src == _SRC_VO){
			bIpDma420Mode= SLR_DMA_420_mode;
		}
		//frank@0904 add below code to avoid DI memory trash
		else if(Get_PANEL_VFLIP_ENABLE()){
			bIpDma420Mode= SLR_DMA_420_mode;
		}
		else
		{
			bIpDma420Mode = SLR_DMA_422_mode;
		}

		rtd_pr_smt_debug("_rk %s,%d ipdma:%d\n", __FUNCTION__, __LINE__, bIpDma420Mode);

	}
	else
	{
		bIpBtrMode = _ENABLE;
		bIpDma420Mode = SLR_DMA_422_mode;
	}

#else//else of #ifdef REDUCE_DIBW_ENABLE
	//if (picWidth > REDUCE_DI_BANDWIDTH_LIMIT_THRESHOLD){
	if (imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_H_ACT_WID_PRE> REDUCE_DI_BANDWIDTH_LIMIT_THRESHOLD)
	{
		if((Scaler_InputSrcGetMainChType() == _SRC_HDMI)&&((_MODE_1080I25 == imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_MODE_CURR )||(_MODE_1080I30 == imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_MODE_CURR)) )
		{
			bIpBtrMode=(_ENABLE);//for 1080i video book Di 1 #chapter_10
			bIpDma420Mode=(_DISABLE);
		}
		else
		{
			bIpBtrMode=(_DISABLE);
			bIpDma420Mode=(_DISABLE);
#if 0 // [IDMA] IDMA 3D reduce bandwidth issue (Sensio 3D mode)
			if((Scaler_InputSrcGetMainChType() == _SRC_VO) && modestate_I3DDMA_get_In3dMode() && (Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= REDUCE_DI_BW_VFREQ_THRESHOLD))
				drv_memory_set_ip_DMA420_mode(SLR_DMA_420_mode);
			else
				drv_memory_set_ip_DMA420_mode(SLR_DMA_422_mode);

			rtd_pr_smt_debug("[DBG] V_Freq=%d\n", Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000));
#endif
		}
		//MODESTATE_PRINTF("[DI] DI BTR/420[%d/%d], width=%d!!\n", drv_memory_get_ip_Btr_mode(), drv_memory_get_ip_DMA420_mode(), picWidth);
	}
	else{
		bIpBtrMode=(_ENABLE);
		bIpDma420Mode=(_DISABLE);
	}

#endif //end of #ifdef REDUCE_DIBW_ENABLE



#ifdef CONFIG_DUAL_CHANNEL
	if (display == SLR_SUB_DISPLAY) {
		//sub always don't go DI
		imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP = FALSE;
		return;
	}
#endif
	// now handle main display
	//if(Scaler_DispGetStatus(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_INTERLACE)) {
	if(imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE) {
#ifdef CONFIG_DUAL_CHANNEL
			if (imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_THRIP||imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_RTNR) {
				fw_scalerip_disable_ip(SLR_SUB_DISPLAY);
				imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_THRIP = FALSE;
				imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_RTNR = FALSE;
			}
#endif
		imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP = TRUE;

	}
	else
	{
		imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP = FALSE;
	}



}


void imd_smooth_enable_main_double_buffer(unsigned char bEnable)
{
#if 1
	vgip_vgip_chn1_double_buffer_ctrl_RBUS vgip_chn1_double_buffer_reg;
	di_db_reg_ctl_RBUS  db_reg_ctl_reg;
	mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;
    mdom_sub_cap_cap1_reg_doublbuffer_RBUS mdom_sub_cap_cap1_reg_doublbuffer_reg;
    mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
	mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	scaledown_ich1_uzd_db_ctrl_RBUS ich1_uzd_db_ctrl_reg;
	scaleup_dm_uzu_db_ctrl_RBUS dm_uzu_db_ctrl_reg;
	hsd_dither_di_uzd_db_ctrl_RBUS di_uzd_db_ctrl_reg;
	pll27x_reg_dpll_double_buffer_ctrl_RBUS dpll_double_buffer_ctrl_reg;
	iedge_smooth_edsm_db_ctrl_RBUS edsm_db_ctrl_reg;
	rgb2yuv_ich1_422to444_ctrl_db_RBUS rgb2yuv_ich1_422to444_ctrl_db_reg;
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;
	// ptol_ptol_db_ctrl_RBUS  ptol_ptol_db_ctrl_reg;
	//two_step_uzu_sr_db_ctrl_RBUS two_step_uzu_sr_db_ctrl_reg;
	ppoverlay_iv2dv_double_buffer_ctrl_RBUS iv2dv_double_buffer_reg;
	//mdomain_vi_main_mdom_vi_doublebuffer_RBUS mdomain_vi_main_mdom_vi_doublebuffer_reg;
	unsigned long flags;//for spin_lock_irqsave

	if(bEnable){
		//vgip
		vgip_chn1_double_buffer_reg.regValue =rtd_inl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg);
		vgip_chn1_double_buffer_reg.ch1_db_en=1;
		vgip_chn1_double_buffer_reg.ch1_db_rdy=0;
		rtd_outl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg,vgip_chn1_double_buffer_reg.regValue);

		//rtd_pr_smt_debug("enable0.0----------\n");
#if 1
		//di
		db_reg_ctl_reg.regValue =rtd_inl(DI_db_reg_ctl_reg);
		if (imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_RTNR ||imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_THRIP) {
			db_reg_ctl_reg.sm_tog_apply_disable=0;
		}
		else {
			db_reg_ctl_reg.sm_tog_apply_disable=1;
		}
		db_reg_ctl_reg.sm_tog_apply_disable=1;
	/*	if (imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_INTERLACE) {
			db_reg_ctl_reg.sm_tog_3d_mode=1;
		} else*/ {
			db_reg_ctl_reg.sm_tog_3d_mode=0;
		}

		db_reg_ctl_reg.db_en=1;
 		db_reg_ctl_reg.db_apply=0;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

		//rtd_pr_smt_debug("0.1----------\n");

		//m domain capture
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en=1;
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply=0;
		rtd_outl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);

		//rtd_pr_smt_debug("0.2----------\n");

		//m domain display
		mdomain_disp_disp0_db_ctrl_reg.regValue = rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
		mdomain_disp_disp0_db_ctrl_reg.disp0_db_en=1;
		mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply=0;
		rtd_outl(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);

		//rtd_pr_smt_debug("0.3----------\n");
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		double_buffer_ctrl_reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
		//D7
		double_buffer_ctrl_reg.dreg_dbuf_en=1;
		double_buffer_ctrl_reg.dreg_dbuf_set=0;
		//D0
		double_buffer_ctrl_reg.dmainreg_dbuf_en=1;
		double_buffer_ctrl_reg.dmainreg_dbuf_set=0;
		//D8
		double_buffer_ctrl_reg.drm_multireg_dbuf_en = 1;
        double_buffer_ctrl_reg.drm_multireg_dbuf_set = 0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);
		spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG4
		double_buffer_ctrl2_reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL2_reg);
		//D4
		double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
		double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 0;
		//D5
		double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 1;
		double_buffer_ctrl2_reg.memc_dtgreg_dbuf_set = 0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

		//rtd_pr_smt_debug("0.4----------\n");

		//UZD
		ich1_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
		ich1_uzd_db_ctrl_reg.h_db_en=1;
		ich1_uzd_db_ctrl_reg.h_db_apply=0;
		ich1_uzd_db_ctrl_reg.v_db_en=1;
		ich1_uzd_db_ctrl_reg.v_db_apply=0;
		rtd_outl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg,ich1_uzd_db_ctrl_reg.regValue);

		//rtd_pr_smt_debug("0.5----------\n");

		//UZU
		dm_uzu_db_ctrl_reg.regValue =rtd_inl(SCALEUP_DM_UZU_DB_CTRL_reg);
		dm_uzu_db_ctrl_reg.db_en=1;
		dm_uzu_db_ctrl_reg.db_apply=0;
		rtd_outl(SCALEUP_DM_UZU_DB_CTRL_reg,dm_uzu_db_ctrl_reg.regValue);

		//rtd_pr_smt_debug("0.6----------\n");

		//DI HSD
		di_uzd_db_ctrl_reg.regValue =rtd_inl(HSD_DITHER_DI_UZD_DB_CTRL_reg);
		di_uzd_db_ctrl_reg.h_db_en=1;
		di_uzd_db_ctrl_reg.h_db_apply=0;
		rtd_outl(HSD_DITHER_DI_UZD_DB_CTRL_reg,di_uzd_db_ctrl_reg.regValue);

		//rtd_pr_smt_debug("0.7----------\n");

		//DPLL
		dpll_double_buffer_ctrl_reg.regValue =rtd_inl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg);
		dpll_double_buffer_ctrl_reg.double_buf_en=1;
		dpll_double_buffer_ctrl_reg.double_buf_apply=0;
		///rtd_outl(SYSTEM_DPLL_DOUBLE_BUFFER_CTRL_VADDR,dpll_double_buffer_ctrl_reg.regValue);
		rtd_outl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg,dpll_double_buffer_ctrl_reg.regValue);

		//rtd_pr_smt_debug("0.8----------\n");
		//iedge
		edsm_db_ctrl_reg.regValue =rtd_inl(IEDGE_SMOOTH_EDSM_DB_CTRL_reg);
		edsm_db_ctrl_reg.edsm_db_en=1;
		///edsm_db_ctrl_reg.edsm_db_apply=0;
		rtd_outl(IEDGE_SMOOTH_EDSM_DB_CTRL_reg,edsm_db_ctrl_reg.regValue);

		rgb2yuv_ich1_422to444_ctrl_db_reg.regValue = rtd_inl(RGB2YUV_ICH1_422to444_CTRL_DB_reg);
		//rgb2yuv_ich1_422to444_ctrl_db_reg.db_en = 1;
		rgb2yuv_ich1_422to444_ctrl_db_reg.db_apply = 0;
		rtd_outl(RGB2YUV_ICH1_422to444_CTRL_DB_reg,rgb2yuv_ich1_422to444_ctrl_db_reg.regValue);
#endif
	}
	else{
		//vgip
		vgip_chn1_double_buffer_reg.regValue =rtd_inl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg);
		vgip_chn1_double_buffer_reg.ch1_db_en=0;
		vgip_chn1_double_buffer_reg.ch1_db_rdy=0;
		rtd_outl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg,vgip_chn1_double_buffer_reg.regValue);
		//rtd_pr_smt_debug("disble0.0----------\n");
#if 1
		//di
		db_reg_ctl_reg.regValue =rtd_inl(DI_db_reg_ctl_reg);

		db_reg_ctl_reg.db_en=1;
 		db_reg_ctl_reg.db_apply=1;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

		//rtd_pr_smt_debug("0.1----------\n");

		//m domain capture
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en=0;
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply=0;
		rtd_outl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);

		//rtd_pr_smt_debug("0.2----------\n");

		//m domain display
		//if((get_ori_rotate_mode(SLR_MAIN_DISPLAY) == get_rotate_mode(SLR_MAIN_DISPLAY)) && (get_rotate_mode(SLR_MAIN_DISPLAY) == 0))
		{
			mdomain_disp_disp0_db_ctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_en=0;
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply=0;
			rtd_outl(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
		}
		//mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);
		//mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_en = 0;
		//mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_apply = 0;
		//rtd_outl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg, mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue);

		//rtd_pr_smt_debug("0.3----------\n");
		//down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG
		//spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		//double_buffer_ctrl_reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
		//D7
		//double_buffer_ctrl_reg.dreg_dbuf_en=0;
		//double_buffer_ctrl_reg.dreg_dbuf_set=0;
		//D0
		//double_buffer_ctrl_reg.dmainreg_dbuf_en=0;
		//double_buffer_ctrl_reg.dmainreg_dbuf_set=0;
		//D8
		//double_buffer_ctrl_reg.drm_multireg_dbuf_en = 0;
        //double_buffer_ctrl_reg.drm_multireg_dbuf_set = 0;
		//rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);
		//spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		//up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG4
		//double_buffer_ctrl2_reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL2_reg);
		//D4
		//double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 0;
		//double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 0;
		//D5
		//double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 0;
		//double_buffer_ctrl2_reg.memc_dtgreg_dbuf_set = 0;
		//rtd_outl(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);
		//rtd_pr_smt_debug("0.4----------\n");

		//UZD
		ich1_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
		ich1_uzd_db_ctrl_reg.h_db_en=0;
		ich1_uzd_db_ctrl_reg.h_db_apply=0;
		ich1_uzd_db_ctrl_reg.v_db_en=0;
		ich1_uzd_db_ctrl_reg.v_db_apply=0;
		rtd_outl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg,ich1_uzd_db_ctrl_reg.regValue);
		//rtd_pr_smt_debug("0.5----------\n");

		//UZU
		dm_uzu_db_ctrl_reg.regValue =rtd_inl(SCALEUP_DM_UZU_DB_CTRL_reg);
		dm_uzu_db_ctrl_reg.db_en=0;
		dm_uzu_db_ctrl_reg.db_apply=0;
		rtd_outl(SCALEUP_DM_UZU_DB_CTRL_reg,dm_uzu_db_ctrl_reg.regValue);
		//rtd_pr_smt_debug("0.6----------\n");

		//DI HSD
		di_uzd_db_ctrl_reg.regValue =rtd_inl(HSD_DITHER_DI_UZD_DB_CTRL_reg);
		di_uzd_db_ctrl_reg.h_db_en=0;
		di_uzd_db_ctrl_reg.h_db_apply=0;
		rtd_outl(HSD_DITHER_DI_UZD_DB_CTRL_reg,di_uzd_db_ctrl_reg.regValue);
		//rtd_pr_smt_debug("0.7----------\n");

		//DPLL
		dpll_double_buffer_ctrl_reg.regValue =rtd_inl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg);
		dpll_double_buffer_ctrl_reg.double_buf_en=0;
		dpll_double_buffer_ctrl_reg.double_buf_apply=0;
		rtd_outl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg,dpll_double_buffer_ctrl_reg.regValue);
		//rtd_pr_smt_debug("0.8----------\n");

		//iedge
		//vip echang marked //sync from mac3 generic
		///edsm_db_ctrl_reg.regValue =rtd_inl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR);
		///edsm_db_ctrl_reg.edsm_db_en=0;
		///edsm_db_ctrl_reg.edsm_db_apply=0;
		///rtd_outl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR,edsm_db_ctrl_reg.regValue);
		rgb2yuv_ich1_422to444_ctrl_db_reg.regValue = rtd_inl(RGB2YUV_ICH1_422to444_CTRL_DB_reg);
		rgb2yuv_ich1_422to444_ctrl_db_reg.db_en = 0;
		rgb2yuv_ich1_422to444_ctrl_db_reg.db_apply = 0;
		rtd_outl(RGB2YUV_ICH1_422to444_CTRL_DB_reg,rgb2yuv_ich1_422to444_ctrl_db_reg.regValue);
#endif
		if (Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE) {
		//m domain display
			mdom_sub_disp_disp1_db_ctrl_reg.regValue =rtd_inl(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
			mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en=0;
			mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply=0;
			rtd_outl(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

			mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue =rtd_inl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
			mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en=0;
			mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply=0;
			rtd_outl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
		}
		// ptol_ptol_db_ctrl_reg.regValue = rtd_inl(PTOL_PtoL_DB_CTRL_reg);
		// ptol_ptol_db_ctrl_reg.double_buf_en = 0;//enable double buffer
		// ptol_ptol_db_ctrl_reg.double_buf_set = 0;
		// rtd_outl(PTOL_PtoL_DB_CTRL_reg, ptol_ptol_db_ctrl_reg.regValue);

		//two_step_uzu_sr_db_ctrl_reg.regValue = rtd_inl(TWO_STEP_UZU_SR_DB_CTRL_reg);
		//two_step_uzu_sr_db_ctrl_reg.db_en=0;
		//two_step_uzu_sr_db_ctrl_reg.db_apply = 0;
		//rtd_outl(TWO_STEP_UZU_SR_DB_CTRL_reg,two_step_uzu_sr_db_ctrl_reg.regValue);

		iv2dv_double_buffer_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
		iv2dv_double_buffer_reg.iv2dv_db_en = 0;
		iv2dv_double_buffer_reg.iv2dv_db_apply= 0;
		IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, iv2dv_double_buffer_reg.regValue);
	}
#endif
}

void imd_smooth_enable_sub_double_buffer(unsigned char bEnable)
{
#ifdef CONFIG_DUAL_CHANNEL
	sub_vgip_vgip_chn2_double_buffer_ctrl_RBUS vgip_chn2_double_buffer_ctrl_reg;
	di_db_reg_ctl_RBUS db_reg_ctl_reg;
    mdom_sub_cap_cap1_reg_doublbuffer_RBUS mdom_sub_cap_cap1_reg_doublbuffer_reg;
    mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
	ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;
	scaledown_ich2_uzd_db_ctrl_RBUS ich2_uzd_db_ctrl_reg;
	scaleup_ds_uzu_db_ctrl_RBUS ds_uzu_db_ctrl_reg;
	pll27x_reg_dpll_double_buffer_ctrl_RBUS dpll_double_buffer_ctrl_reg;
	iedge_smooth_edsm_db_ctrl_RBUS edsm_db_ctrl_reg;
	rgb2yuv_ich2_422to444_ctrl_RBUS rgb2yuv_ich2_422to444_ctrl_reg;
	unsigned long flags;//for spin_lock_irqsave

	if(bEnable){
		//vgip
		vgip_chn2_double_buffer_ctrl_reg.regValue =rtd_inl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg);
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_en=1;
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_rdy=0;
		rtd_outl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg,vgip_chn2_double_buffer_ctrl_reg.regValue);

		//di
		db_reg_ctl_reg.regValue =rtd_inl(DI_db_reg_ctl_reg);

		db_reg_ctl_reg.db_en=1;
 		db_reg_ctl_reg.db_apply=0;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

		//m domain capture
		mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue =rtd_inl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
		mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en=1;
		mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply=0;
		rtd_outl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);

		//m domain display
		mdom_sub_disp_disp1_db_ctrl_reg.regValue =rtd_inl(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
		mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en=1;
		mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply=0;
		rtd_outl(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		ppoverlay_double_buffer_ctrl4_Reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL4_reg);
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en=1;
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set=0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL4_reg,ppoverlay_double_buffer_ctrl4_Reg.regValue);
		spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//UZD
		ich2_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg);
		ich2_uzd_db_ctrl_reg.h_db_en=1;
		ich2_uzd_db_ctrl_reg.h_db_apply=0;
		ich2_uzd_db_ctrl_reg.v_db_en=1;
		ich2_uzd_db_ctrl_reg.v_db_apply=0;
		rtd_outl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg,ich2_uzd_db_ctrl_reg.regValue);

		//UZU
		ds_uzu_db_ctrl_reg.regValue =rtd_inl(SCALEUP_DS_UZU_DB_CTRL_reg);
		ds_uzu_db_ctrl_reg.db_en=1;
		ds_uzu_db_ctrl_reg.db_apply=0;
		rtd_outl(SCALEUP_DS_UZU_DB_CTRL_reg,ds_uzu_db_ctrl_reg.regValue);

		//DPLL
		dpll_double_buffer_ctrl_reg.regValue =rtd_inl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg);
		dpll_double_buffer_ctrl_reg.double_buf_en=1;
		dpll_double_buffer_ctrl_reg.double_buf_apply=0;
		rtd_outl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg,dpll_double_buffer_ctrl_reg.regValue);

		//iedge
		edsm_db_ctrl_reg.regValue =rtd_inl(IEDGE_SMOOTH_EDSM_DB_CTRL_reg);
		edsm_db_ctrl_reg.edsm_db_en=1;
		//edsm_db_ctrl_reg.edsm_db_apply=0;
		rtd_outl(IEDGE_SMOOTH_EDSM_DB_CTRL_reg,edsm_db_ctrl_reg.regValue);

		rgb2yuv_ich2_422to444_ctrl_reg.regValue = rtd_inl(RGB2YUV_ICH2_422to444_CTRL_reg);
		rgb2yuv_ich2_422to444_ctrl_reg.db_en = 1;
		rgb2yuv_ich2_422to444_ctrl_reg.db_apply = 0;
		rtd_outl(RGB2YUV_ICH2_422to444_CTRL_reg,rgb2yuv_ich2_422to444_ctrl_reg.regValue);
	}
	else{
		//vgip
		vgip_chn2_double_buffer_ctrl_reg.regValue =rtd_inl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg);
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_en=0;
		vgip_chn2_double_buffer_ctrl_reg.ch2_db_rdy=0;
		rtd_outl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg,vgip_chn2_double_buffer_ctrl_reg.regValue);
 		//m domain capture
		mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue =rtd_inl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
		mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en=0;
		mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply=0;
		rtd_outl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);

		//m domain display
		mdom_sub_disp_disp1_db_ctrl_reg.regValue =rtd_inl(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
		mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en=0;
		mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply=0;
		rtd_outl(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);
#if 0
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		ppoverlay_double_buffer_ctrl4_Reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL4_reg);
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en=0;
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set=0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL4_reg,ppoverlay_double_buffer_ctrl4_Reg.regValue);
		spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
#endif
		//UZD
		ich2_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg);
		ich2_uzd_db_ctrl_reg.h_db_en=0;
		ich2_uzd_db_ctrl_reg.h_db_apply=0;
		ich2_uzd_db_ctrl_reg.v_db_en=0;
		ich2_uzd_db_ctrl_reg.v_db_apply=0;
		rtd_outl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg,ich2_uzd_db_ctrl_reg.regValue);

		//UZU
		ds_uzu_db_ctrl_reg.regValue =rtd_inl(SCALEUP_DS_UZU_DB_CTRL_reg);
		ds_uzu_db_ctrl_reg.db_en=0;
		ds_uzu_db_ctrl_reg.db_apply=0;
		rtd_outl(SCALEUP_DS_UZU_DB_CTRL_reg,ds_uzu_db_ctrl_reg.regValue);
#if 0
		//DPLL
		//rika 20150420
		dpll_double_buffer_ctrl_reg.regValue =rtd_inl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg);
		dpll_double_buffer_ctrl_reg.double_buf_en=0;
		dpll_double_buffer_ctrl_reg.double_buf_apply=0;
		rtd_outl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg,dpll_double_buffer_ctrl_reg.regValue);
#endif
		//iedge
		//vip echang marked //sync from mac3 generic
		///edsm_db_ctrl_reg.regValue =rtd_inl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR);
		///edsm_db_ctrl_reg.edsm_db_en=0;
		///edsm_db_ctrl_reg.edsm_db_apply=0;
		///rtd_outl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR,edsm_db_ctrl_reg.regValue);

		rgb2yuv_ich2_422to444_ctrl_reg.regValue = rtd_inl(RGB2YUV_ICH2_422to444_CTRL_reg);
		rgb2yuv_ich2_422to444_ctrl_reg.db_en = 0;
		rgb2yuv_ich2_422to444_ctrl_reg.db_apply = 0;
		rtd_outl(RGB2YUV_ICH2_422to444_CTRL_reg,rgb2yuv_ich2_422to444_ctrl_reg.regValue);
	}
#endif
}

#ifdef VPQ_COMPILER_ERROR_ENABLE_MERLIN6
static unsigned char SD_V_Coeff_Sel[] = {SDFIR64_Blur, SDFIR64_Mid, SDFIR64_Sharp, SDFIR64_2tap}; // the size must match large table
#endif

void scalerip_modify_rdstart(unsigned char channel, unsigned char di_f, unsigned char rtnr_f, unsigned char hscaledown_f)
{
	unsigned short hsd_act_large = 0, hsd_act_small = 0;
	di_im_di_debug_mode_RBUS reg_input_di_ctrl_REG;
	di_im_di_active_window_control_RBUS reg_active_window_REG;
	unsigned int m_readstart = 0;

	if(rtnr_f)
	{
		if(!hscaledown_f)
			return;
	}
	else if( (!di_f) || (!hscaledown_f))
		return;

	//Elsie 20130308: Using hsd dither instead of UZD in Magellan
	if (channel == _CHANNEL1)
	{
		hsd_dither_di_hsd_scale_hor_factor_RBUS		di_hsd_scale_hor_factor_REG;
		hsd_dither_di_hsd_initial_value_RBUS			di_hsd_initial_value_REG;
		//	di_hsd_hor_segment_RBUS 		di_hsd_hor_segment_REG;
		//hsd_dither_di_hsd_hor_delta1_RBUS			di_hsd_hor_delta1_REG;

		di_hsd_scale_hor_factor_REG.regValue	= rtd_inl(HSD_DITHER_DI_HSD_Scale_Hor_Factor_reg);
		di_hsd_initial_value_REG.regValue		= rtd_inl(HSD_DITHER_DI_HSD_Initial_Value_reg);
		//	di_hsd_hor_segment_REG.regValue 		= rtd_inl(HSD_DITHER_DI_HSD_HOR_SEGMENT_VADDR);
		//di_hsd_hor_delta1_REG.regValue		= rtd_inl(HSD_DITHER_DI_HSD_Hor_Delta1_reg);
#if 0//frank@0724 sync from magellan
		hsd_act_large = 1+(((di_hsd_initial_value_REG.hor_ini << 12) + di_hsd_scale_hor_factor_REG.hor_fac *
					(di_hsd_hor_segment_REG.nl_seg1 *2 + di_hsd_hor_segment_REG.nl_seg2) +
					(di_hsd_hor_delta1_REG.nl_d1 * di_hsd_hor_segment_REG.nl_seg1) *
					(di_hsd_hor_segment_REG.nl_seg1 + di_hsd_hor_segment_REG.nl_seg2) -
					di_hsd_scale_hor_factor_REG.hor_fac)>>20);

#else
		hsd_act_large = imdSmoothInfo[channel].IMD_SMOOTH_H_ACT_WID;
#endif
		hsd_act_small = di_n1_seg1 *2 + di_n1_seg2;
		//	rtd_pr_smt_debug("h_ini=%d, h_fac=%x, Seg1=%d, Seg2=%d, NL_D1=%d\n",di_hsd_initial_value_REG.hor_ini,
		//		di_hsd_scale_hor_factor_REG.hor_fac, di_hsd_hor_segment_REG.nl_seg1,
		//		di_hsd_hor_segment_REG.nl_seg2, di_hsd_hor_delta1_REG.nl_d1);
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{
		hsd_dither_di_hsd_hor_segment_RBUS 		di_hsd_hor_segment_REG;
		di_hsd_hor_segment_REG.regValue 		= rtd_inl(HSD_DITHER_DI_HSD_Hor_Segment_reg);

		hsd_act_large =imdSmoothInfo[channel].IMD_SMOOTH_H_ACT_WID;//Thur added, recommended by Fishtail 20080423
		hsd_act_small = di_n1_seg1 *2 + di_n1_seg2;
	}
#endif



	reg_input_di_ctrl_REG.regValue = rtd_inl(DI_IM_DI_DEBUG_MODE_reg);
	reg_active_window_REG.regValue = rtd_inl(DI_IM_DI_ACTIVE_WINDOW_CONTROL_reg);


	//m_readstart = hsd_act_large - hsd_act_small;
	m_readstart = hsd_act_large - hsd_act_small + 1;        //      Readstart = Line size before HSD - line size after HSD (reg_hsize)+1
	#if 0
	if(m_readstart > 2047)
		m_readstart = 2047;
	#endif
	reg_input_di_ctrl_REG.readstart = m_readstart;

	reg_active_window_REG.hsize = hsd_act_small;

	//for freeze framesync underflow case
	reg_active_window_REG.hblksize = 64;
	reg_active_window_REG.vsize = imdSmoothInfo[channel].IMD_SMOOTH_V_ACT_LEN; // Height;

	rtd_outl(DI_IM_DI_DEBUG_MODE_reg, reg_input_di_ctrl_REG.regValue);
	rtd_outl(DI_IM_DI_ACTIVE_WINDOW_CONTROL_reg, reg_active_window_REG.regValue);

	//	rtd_pr_smt_debug("Modify_DI_readstart\n");
}
unsigned char is_di_h_scalar_down(unsigned char display)
{
	return di_hsd_flag;
}




void imd_smooth_scalerdisplay_set_main_act_window(UINT16 DispHSta, UINT16 DispHEnd, UINT16 DispVSta, UINT16 DispVEnd)
{
	ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_reg;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_reg;

	DispHSta += Get_DISP_ACT_STA_BIOS();//need review rika 20140611
	DispHEnd += Get_DISP_ACT_STA_BIOS();

	// Main Active H pos
	main_active_h_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_H_Start_End_reg);
	main_active_h_start_end_reg.mh_act_sta = DispHSta & 0x1fff;
	main_active_h_start_end_reg.mh_act_end = DispHEnd & 0x1fff;
	rtd_outl(PPOVERLAY_MAIN_Active_H_Start_End_reg, main_active_h_start_end_reg.regValue);

	// Main Active V pos
	main_active_v_start_end_reg.regValue = rtd_inl(PPOVERLAY_MAIN_Active_V_Start_End_reg);
	main_active_v_start_end_reg.mv_act_sta = DispVSta & 0x1fff;
	main_active_v_start_end_reg.mv_act_end = DispVEnd & 0x1fff;
	rtd_outl(PPOVERLAY_MAIN_Active_V_Start_End_reg, main_active_v_start_end_reg.regValue);
}

StructSrcRect Scaler_SubDispWindowGet(void);

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels

void imd_smooth_scalerdisplay_set_sub_display_window(unsigned short DispHSta, unsigned short DispHEnd, unsigned short DispVSta,
	unsigned short DispVEnd, unsigned char Border)
{
   /* sync from pipmp.c : PipmpSetSubDisplayWindow */

	ppoverlay_sub_den_h_start_width_RBUS ppoverlay_sub_den_h_start_width_Reg;
	ppoverlay_sub_den_v_start_length_RBUS ppoverlay_sub_den_v_start_length_Reg;
	ppoverlay_sub_background_h_start_end_RBUS ppoverlay_sub_background_h_start_end_Reg;
	ppoverlay_sub_background_v_start_end_RBUS ppoverlay_sub_background_v_start_end_Reg;
    mdom_sub_disp_disp1_pre_rd_ctrl_RBUS mdom_sub_disp_disp1_pre_rd_ctrl_reg;

	//StructSrcRect s_dispwin;
	//s_dispwin = Scaler_SubDispWindowGet();

	mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg);
#if 0 //K3L Q8401 frame tear issue
		if(s_dispwin.srcy <= 100) {
			mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = Get_DISP_DEN_STA_VPOS()-7;
		} else {
			mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = (s_dispwin.srcy+Get_DISP_DEN_STA_VPOS()-4);
		}
#else //KTASKWBS-21489 k8 change to 7
		mdom_sub_disp_disp1_pre_rd_ctrl_reg.disp1_pre_rd_v_start = SUB_MDOM_PREREAD_START;
#endif

	IoReg_Write32(MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg, mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue);

	IoReg_Mask32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,~(MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_apply_mask),MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_apply_mask);	//Enable memory double buffer write-in
	//drvif_memory_set_dbuffer_write(SLR_MAIN_DISPLAY);

	ppoverlay_sub_den_h_start_width_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_H_Start_Width_reg);
	ppoverlay_sub_den_v_start_length_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_V_Start_Length_reg);
	ppoverlay_sub_background_h_start_end_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_H_Start_End_reg);
	ppoverlay_sub_background_v_start_end_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_V_Start_End_reg);

	ppoverlay_sub_den_h_start_width_Reg.sh_den_sta = (DispHSta + Get_DISP_ACT_STA_BIOS());
	ppoverlay_sub_den_h_start_width_Reg.sh_den_width = (DispHEnd - DispHSta - 1);

	ppoverlay_sub_den_v_start_length_Reg.sv_den_sta = DispVSta;
	ppoverlay_sub_den_v_start_length_Reg.sv_den_length = (DispVEnd -DispVSta);

	ppoverlay_sub_background_h_start_end_Reg.sh_bg_sta = Border;
	ppoverlay_sub_background_h_start_end_Reg.sh_bg_end = (DispHEnd - DispHSta - Border);

	ppoverlay_sub_background_v_start_end_Reg.sv_bg_sta = Border;
	ppoverlay_sub_background_v_start_end_Reg.sv_bg_end = (DispVEnd - DispVSta - Border);

	IoReg_Write32(PPOVERLAY_SUB_DEN_H_Start_Width_reg, ppoverlay_sub_den_h_start_width_Reg.regValue);
	IoReg_Write32(PPOVERLAY_SUB_DEN_V_Start_Length_reg, ppoverlay_sub_den_v_start_length_Reg.regValue);

	IoReg_Write32(PPOVERLAY_SUB_Background_H_Start_End_reg, ppoverlay_sub_background_h_start_end_Reg.regValue);
	IoReg_Write32(PPOVERLAY_SUB_Background_V_Start_End_reg, ppoverlay_sub_background_v_start_end_Reg.regValue);
}

void imd_smooth_scalerdisplay_set_sub_act_window(UINT16 DispHSta, UINT16 DispHEnd, UINT16 DispVSta, UINT16 DispVEnd)
{
	/* sync from pipmp.c void PipmpSetSubActiveWindow */
	ppoverlay_sub_active_h_start_end_RBUS ppoverlay_sub_active_h_start_end_Reg;
	ppoverlay_sub_active_v_start_end_RBUS ppoverlay_sub_active_v_start_end_Reg;
	DispHSta += Get_DISP_ACT_STA_BIOS();
	DispHEnd += Get_DISP_ACT_STA_BIOS();


	ppoverlay_sub_active_h_start_end_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_H_Start_End_reg);
	ppoverlay_sub_active_v_start_end_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_V_Start_End_reg);

	ppoverlay_sub_active_h_start_end_Reg.sh_act_sta = DispHSta;
	ppoverlay_sub_active_h_start_end_Reg.sh_act_end = DispHEnd;

	ppoverlay_sub_active_v_start_end_Reg.sv_act_sta = DispVSta;
	ppoverlay_sub_active_v_start_end_Reg.sv_act_end = DispVEnd;

	IoReg_Write32(PPOVERLAY_SUB_Active_H_Start_End_reg, ppoverlay_sub_active_h_start_end_Reg.regValue);
	IoReg_Write32(PPOVERLAY_SUB_Active_V_Start_End_reg, ppoverlay_sub_active_v_start_end_Reg.regValue);

}
#endif
typedef struct
{
	int h_factor_offset;
	int v_factor_offset;
	unsigned char h_initial;
	unsigned char v_initial;
	unsigned char user_defined;
}UZU_SETTINGS;
static UZU_SETTINGS uzu_coeff = {0,0,0,0,0};



VIP_Two_Step_UZU_MODE SmoothToggle_Two_Step_UZU_Mode = Two_Step_UZU_Disable;

void smooth_toggle_color_Set_Two_Step_UZU_Mode(VIP_Two_Step_UZU_MODE Mode)
{
	SmoothToggle_Two_Step_UZU_Mode = Mode;
}

unsigned char smooth_toggle_color_Get_Two_Step_UZU_Mode(void)
{
	return (unsigned char)SmoothToggle_Two_Step_UZU_Mode;
}
void smooth_toggle_color_twostep_scaleup_set(unsigned char display, SIZE *ptInSize, SIZE *ptOutSize)/*add for Merlin2 (jyyang_2016/06/15)*/
{
	/*int uzu_en, tsuzu_en, tsuzu_mode, tsuzu_ratio;*/
	unsigned char tsuzu_en;//tsuzu_mode, tsuzu_ratio;
	/* uzu_en = 0:disable, 1:enable for UZU
	tsuzu_en  = 0:disable, 1:enable for TWO_STEP UZU
	tsuzu_mode = 0:[YUV domain] uzu1->uzu2->2d3d,  1:[RGB domain] uzu1->2d3d-> !K ->uzu2
	tsuzu_ratio = 0:2x, 1:1.5x*/

	ppoverlay_uzudtg_control1_RBUS ppoverlay_uzudtg_control1_reg;
	ppoverlay_uzudtg_dvs_cnt_RBUS ppoverlay_uzudtg_dvs_cnt_REG;
	/*scaleup_dm_uzu_ctrl_RBUS dm_uzu_ctrl_REG;*/	/* control by flow, don't control uzu enable*/
	//two_step_uzu_two_step_uzu_ctrl_RBUS two_step_uzu_two_step_uzu_ctrl_reg;// remove from merlin3, only support 2x UZU
	ppoverlay_double_buffer_ctrl2_RBUS ppoverlay_double_buffer_ctrl2_reg;
//	ppoverlay_memcdtg_control3_RBUS ppoverlay_memcdtg_control3_reg;
	ppoverlay_memc_mux_ctrl_RBUS ppoverlay_memc_mux_ctrl_reg;

	ppoverlay_uzudtg_dvs_cnt_REG.regValue = rtd_inl(PPOVERLAY_uzudtg_DVS_cnt_reg);
	/*frame_rate = 27000000/cnt; // check with Ben_Wang
	120Hz: cnt=27000000/120=225000
	118Hz: cnt=27000000/118=228813
	=== Reduce 50% the 1st Scaleup Size ===*/

	if(ptInSize->nWidth <=960 && ptInSize->nLength<=576 && ptOutSize->nWidth>1920 && ptOutSize->nLength>1152) // jyyang:width<960&&ratio>2
	{	/*do twostep scaleup*/
		tsuzu_en = 1;
		//tsuzu_mode = 0; // YUV domain
		//tsuzu_ratio = 0; // 2x
		smooth_toggle_color_Set_Two_Step_UZU_Mode(Two_Step_UZU_Enable);
	}
#if 0	/* input 120Hz need to check woth ben wang*/
	else if(ppoverlay_uzudtg_dvs_cnt_REG.uzudtg_dvs_cnt<228813) // ~ 120Hz
	{
		if(ptInSize->nWidth ==1920 && ptInSize->nLength==1080 && ptOutSize->nWidth==3840 && ptOutSize->nLength==2160)
		{
			uzu_en = 0;
			tsuzu_en = 1;
			tsuzu_mode = 1; // RGB domain
			tsuzu_ratio = 0; // 2x
		}
		else if(ptInSize->nWidth ==2560 && ptInSize->nLength==1440 && ptOutSize->nWidth==3840 && ptOutSize->nLength==2160)
		{
			uzu_en = 0;
			tsuzu_en = 1;
			tsuzu_mode = 1; // RGB domain
			tsuzu_ratio = 1; // 1.5x
		}
		drvif_color_Set_Two_Step_UZU_Mode(Two_Step_UZU_Input_120Hz);
	}
#endif
	else
	{
		tsuzu_en = 0;
		//tsuzu_mode = 0; // YUV domain
		//tsuzu_ratio = 0; // 2x
		smooth_toggle_color_Set_Two_Step_UZU_Mode(Two_Step_UZU_Disable);
	}

	rtd_pr_smt_emerg("2-step in live zoom flow, InSize, Wid=%d, Len=%d, OutSize, Wid=%d, Leng=%d\n",
		ptInSize->nWidth, ptInSize->nLength, ptOutSize->nWidth, ptOutSize->nLength);

	/*dm_uzu_ctrl_REG.regValue = IoReg_Read32(SCALEUP_DM_UZU_CTRL_VADDR);*/
	//two_step_uzu_two_step_uzu_ctrl_reg.regValue = rtd_inl(TWO_STEP_UZU_Two_Step_uzu_Ctrl_reg);
	ppoverlay_uzudtg_control1_reg.regValue = rtd_inl(PPOVERLAY_uzudtg_control1_reg);

	/* 2 step uzu enable*//* 120 Hz input setting*/
	ppoverlay_uzudtg_control1_reg.two_step_uzu_en = tsuzu_en; // uzu2 = 0:disable, 1:enable
	//remove two_step_uzu_mode from merlin3
	//ppoverlay_uzudtg_control1_reg.two_step_uzu_mode = tsuzu_mode; // uzu2 position = 0:[YUV domain] uzu1->uzu2->2d3d,  1:[RGB domain] uzu1->2d3d-> !K ->uzu2
	//two_step_uzu_two_step_uzu_ctrl_reg.two_step_uzu_ratio = tsuzu_ratio; //0:2x, 1:1.5x

	/* UZU enable*/
	/*dm_uzu_ctrl_REG.h_zoom_en = uzu_en;
	dm_uzu_ctrl_REG.v_zoom_en = uzu_en;*/

	/*IoReg_Write32(SCALEUP_DM_UZU_CTRL_VADDR, dm_uzu_ctrl_REG.regValue);*/
	//rtd_outl(TWO_STEP_UZU_Two_Step_uzu_Ctrl_reg, two_step_uzu_two_step_uzu_ctrl_reg.regValue);
	rtd_outl(PPOVERLAY_uzudtg_control1_reg, ppoverlay_uzudtg_control1_reg.regValue);

	ppoverlay_memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
	if(ppoverlay_memc_mux_ctrl_reg.memcdtg_golden_vs == 1){
		ppoverlay_double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		ppoverlay_double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 1;
		ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
#if 0
		ppoverlay_memcdtg_control3_reg.regValue = IoReg_Read32(PPOVERLAY_MEMCDTG_CONTROL3_reg);
		if(tsuzu_en){
			ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_line = 0x38;
			ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_pixel = 0x61c;
		}else{
			ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_line = 0x3f;
			ppoverlay_memcdtg_control3_reg.dtgm2goldenpostvs_pixel = 0x65f;
		}
		IoReg_Write32(PPOVERLAY_MEMCDTG_CONTROL3_reg, ppoverlay_memcdtg_control3_reg.regValue);
#endif
		ppoverlay_double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		ppoverlay_double_buffer_ctrl2_reg.memc_dtgreg_dbuf_set = 1;
		ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
	}else{

		/* double buffer control*/
		ppoverlay_double_buffer_ctrl2_reg.regValue = rtd_inl(PPOVERLAY_Double_Buffer_CTRL2_reg);
		ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);/*only using in live zoom, scaler and cp smooth toggel is controlled by IMD double buffer.*/
	}
}


void zoom_imd_smooth_color_ultrazoom_setscaleup(unsigned char display, SMOOTH_TOGGLE_SIZE* ptInSize, SMOOTH_TOGGLE_SIZE* ptOutSize, unsigned char panorama)
{
	unsigned int D1, D2;
	unsigned short S1, S2, S3;
	unsigned char Hini, Vini, r;
	scaleup_dm_uzu_input_size_RBUS	dm_uzu_Input_Size_REG;
	scaleup_dm_uzu_scale_hor_factor_RBUS		dm_uzu_Scale_Hor_Factor_REG;
	scaleup_dm_uzu_scale_ver_factor_RBUS		dm_uzu_Scale_Ver_Factor_REG;
	scaleup_dm_uzu_initial_value_RBUS			dm_uzu_Initial_Value_REG;
	scaleup_dm_uzu_hor_delta1_RBUS			dm_uzu_Hor_Delta1_REG;
	scaleup_dm_uzu_hor_delta2_RBUS			dm_uzu_Hor_Delta2_REG;
	scaleup_dm_uzu_hor_segment1_RBUS		dm_uzu_Hor_Segment1_REG;
	scaleup_dm_uzu_hor_segment2_RBUS		dm_uzu_Hor_Segment2_REG;
	scaleup_dm_uzu_hor_segment3_RBUS		dm_uzu_Hor_Segment3_REG;
	scaleup_ds_uzu_input_size_RBUS	ds_uzu_Input_Size_REG;
	unsigned long long ulparam1 = 0, nFactor;
	unsigned long long ulparam2 = 0;
	unsigned char two_step_factor_shift = 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		S1 = 0;
		S2 = 0;
		S3 = ptOutSize->nWidth;
		D1 = 0;
		D2 = 0;

		dm_uzu_Scale_Ver_Factor_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Scale_Ver_Factor_reg);
		dm_uzu_Scale_Hor_Factor_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Scale_Hor_Factor_reg);
		dm_uzu_Hor_Delta1_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Hor_Delta1_reg);
		dm_uzu_Hor_Delta2_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Hor_Delta2_reg);
		dm_uzu_Hor_Segment1_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Hor_Segment1_reg);
		dm_uzu_Hor_Segment2_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Hor_Segment2_reg);
		dm_uzu_Hor_Segment3_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Hor_Segment3_reg);
		dm_uzu_Initial_Value_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Initial_Value_reg);

#if 0
		smooth_toggle_color_twostep_scaleup_set(display, ptInSize, ptOutSize);
		two_step_factor_shift = (smooth_toggle_color_Get_Two_Step_UZU_Mode() ==  Two_Step_UZU_Enable)?1:0;
#else	/* 2 step db is not in imf smooth topggle, only turn on 2 step in CVBS, YPP-SD, HDMI-SD, DTV-SD, avoid smooth toggle transient noise, elieli */
		two_step_factor_shift = drvif_color_Get_Two_Step_UZU_Mode();	/* same as scaler flow*/
#endif
		if (ptOutSize->nWidth > ptInSize->nWidth)	 // H scale-up, set h scaling factor
		{
			//Hini = 0xff;
			Hini = (UINT8)(((ptInSize->nWidth)*255)/(2*ptOutSize->nWidth) + 127);//for symmertic setting
			if(uzu_coeff.user_defined == 1)
				dm_uzu_Initial_Value_REG.hor_ini = uzu_coeff.h_initial;
			else
				dm_uzu_Initial_Value_REG.hor_ini = Hini;
			if (ptInSize->nWidth > 1024)
				r = 12; // CSW+ 0970104 Restore to original setting
			else
				r = 6; // CSW+ 0961112 enlarge non-linear scaling result

			if(panorama) {	// CSW+ 0960830 Non-linear SU
				if (ptInSize->nWidth > 1024)
					S1 = (ptOutSize->nWidth)  / 6; // CSW+ 0970104 Restore to original setting
				else
					S1 = (ptOutSize->nWidth)  / 5; // CSW+ 0961112 enlarge non-linear scaling result
				S2 = S1;
				S3 = (ptOutSize->nWidth) - 2*S1 - 2*S2;
				//=vv==old from TONY, if no problem, use this=vv==
				ulparam1 = (((unsigned long long)(ptInSize->nWidth)<< 21) - ((unsigned long long)Hini<<13));
				// coverity 120590,120589 : rikahsu 20151217
				// transform to unsigned long long before - to prevent sign extension
				ulparam2 = ((7+4*r)*((unsigned long long)S1) + (3+r)*((unsigned long long)S3) - r);
				do_div(ulparam1, ulparam2);
				nFactor = ulparam1;
				//nFactor = (((UINT32)(ptInSize->nWidth)<< 21) - ((UINT32)Hini<<13)) / ((7+4*r)*S1 + (3+r)*S3 - r);
				//==vv==Thur debugged==vv==
				//nFactor = ((((UINT64)(ptInSize->nWidth))*(ptOutSize->nWidth-1))<<21) / ptOutSize->nWidth;
				//nFactor = nFactor / ((7+4*r)*S1 + (3+r)*S3 - r);
				//=====================
				nFactor = nFactor * r;
				D1 = 2*(UINT32)nFactor / S1 / r;
				D2 = (UINT32)nFactor / S1 / r;
				//rtd_pr_smt_debug("PANORAMA1 HScale up factor before ROUND = %x, In Wid=%x, Out Wid=%x \n", nFactor, ptInSize->nWidth, ptOutSize->nWidth);
				nFactor = SHR((nFactor + 1), 1); //rounding
				D1 = SHR(D1+1,1); //rounding
				D2 = SHR(D2+1,1); //rounding

				if ((nFactor + (3*(UINT32)nFactor/r)) >= 1048576)	{	// Can't do nonlinear SU
					//		rtd_pr_smt_debug("PANORAMA1 Can't do nonlinear SU \n");
					//			nFactor =(((UINT32)(ptInSize->nWidth)<< 21) - ((UINT32)Hini<<13))  / (ptOutSize->nWidth-1);
					ulparam1 = ((unsigned long long)(ptInSize->nWidth)<< 21);
					ulparam2 = (unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1, ulparam2);
					nFactor = ulparam1;
					//nFactor =(((UINT32)(ptInSize->nWidth)<< 21))  / (ptOutSize->nWidth);//Thur 20071227 modified
					//		rtd_pr_smt_debug("PANORAMA1 HScale up factor before ROUND = %x, In Wid=%x, Out Wid=%x \n", nFactor, ptInSize->nWidth, ptOutSize->nWidth);
					nFactor = SHR((nFactor + 1), 1); //rounding
					S1 = 0;
					S2 = 0;
					S3 = ptOutSize->nWidth;
					D1 = 0;
					D2 = 0;
				}
			} else{
				ulparam1 = (((unsigned long long)(ptInSize->nWidth)<< 21));
				ulparam2 = (unsigned long long)(ptOutSize->nWidth);
				do_div(ulparam1, ulparam2);
				nFactor = ulparam1;
				nFactor = (unsigned long long)nFactor + (unsigned long long)uzu_coeff.h_factor_offset;
				//nFactor = (((unsigned int)(ptInSize->nWidth)<< 21))  / (ptOutSize->nWidth);
				//	rtd_pr_smt_info("HScale up factor before ROUND = %x, in Wid=%x, out Wid=%x \n", nFactor, ptOutSize->nLength, ptOutSize->nWidth);
				nFactor = (unsigned long long)SHR((nFactor + 1), 1); //rounding
				#if 0
				nFactor = ((unsigned long long)(ptInSize->nWidth)<< 21)  / ((unsigned long long)(ptOutSize->nWidth));//Thur 20071228 modified
				nFactor =(unsigned long long)nFactor + (unsigned long long)uzu_coeff.h_factor_offset;
				nFactor = (unsigned long long)SHR((nFactor + 1), 1); //rounding
				#endif
			}
			//			rtd_pr_smt_info("HScale up factor = %x\n", nFactor);
		}
		else {
			nFactor = 0xffffff;
		}

#ifdef RUN_ON_TVBOX
		if((scalerdisplay_get_display_tve_mode() == _MODE_480P) && (ptInSize->nWidth >= 704) && (ptInSize->nWidth < 720))
			dm_uzu_Scale_Hor_Factor_REG.regValue = 1048575<<two_step_factor_shift;
		else
			dm_uzu_Scale_Hor_Factor_REG.regValue = nFactor<<two_step_factor_shift;
#else
		dm_uzu_Scale_Hor_Factor_REG.regValue = nFactor<<two_step_factor_shift;
#endif

		dm_uzu_Hor_Delta1_REG.hor_delta1 = D1<<two_step_factor_shift;
		dm_uzu_Hor_Delta2_REG.hor_delta2 = D2<<two_step_factor_shift;
		dm_uzu_Hor_Segment1_REG.hor_segment1 = S1<<two_step_factor_shift;
		dm_uzu_Hor_Segment2_REG.hor_segment2 = S2<<two_step_factor_shift;
		dm_uzu_Hor_Segment3_REG.hor_segment3 = S3<<two_step_factor_shift;

		if (ptOutSize->nLength > ptInSize->nLength) {	// V scale-up, set v scaling factor

			// CSW- 0961128 for frame sync V scale up initial position change
			//	Vini = 0x78;
			// CSW+ 0961128 for frame sync V scale up initial position change by tyan

			//Thur- for symmetric
			//Vini = ((ptInSize->nLength*(0xFF - 0x7F))/(ptOutSize->nLength*2))+(1*(0xFF - 0x7F)/2);
			//Thur+ for symmetric
			Vini = (unsigned char)(((ptInSize->nLength)*255)/(2*ptOutSize->nLength) + 127);//for symmertic setting

			//			rtd_pr_smt_info("====> Vini:%x ptInSize->nLength:%x ptOutSize->nLength:%x\n",Vini, ptInSize->nLength,ptOutSize->nLength);

			//=======
			if(uzu_coeff.user_defined == 1)
				dm_uzu_Initial_Value_REG.ver_ini = uzu_coeff.v_initial;
			else
				dm_uzu_Initial_Value_REG.ver_ini = Vini;
			//nFactor = ((UINT32)((ptInSize->nLength) << 20) - (Vini<<12)) / ((ptOutSize->nLength)-1);
			ulparam1 = (unsigned long long)(ptInSize->nLength) << 21;
			ulparam2 = (unsigned long long)(ptOutSize->nLength);
			do_div(ulparam1, ulparam2);
			nFactor = ulparam1;
			//nFactor = ((unsigned int)((ptInSize->nLength) << 21)) / ((ptOutSize->nLength));
			nFactor = (unsigned long long)nFactor + (unsigned long long)uzu_coeff.v_factor_offset;
			nFactor = (unsigned long long)SHR((nFactor + 1), 1); //rounding
			//			rtd_pr_smt_info("VScale up factor = %x\n", nFactor);
		} else {
			nFactor = 0xffffff;
		}
		dm_uzu_Scale_Ver_Factor_REG.regValue = nFactor<<two_step_factor_shift;

		rtd_outl(SCALEUP_DM_UZU_Scale_Ver_Factor_reg, dm_uzu_Scale_Ver_Factor_REG.regValue);
		rtd_outl(SCALEUP_DM_UZU_Scale_Hor_Factor_reg, dm_uzu_Scale_Hor_Factor_REG.regValue);
		rtd_outl(SCALEUP_DM_UZU_Hor_Delta1_reg, dm_uzu_Hor_Delta1_REG.regValue);
		rtd_outl(SCALEUP_DM_UZU_Hor_Delta2_reg, dm_uzu_Hor_Delta2_REG.regValue);
		rtd_outl(SCALEUP_DM_UZU_Hor_Segment1_reg, dm_uzu_Hor_Segment1_REG.regValue);
		rtd_outl(SCALEUP_DM_UZU_Hor_Segment2_reg, dm_uzu_Hor_Segment2_REG.regValue);
		rtd_outl(SCALEUP_DM_UZU_Hor_Segment3_reg, dm_uzu_Hor_Segment3_REG.regValue);
		rtd_outl(SCALEUP_DM_UZU_Initial_Value_reg, dm_uzu_Initial_Value_REG.regValue);


		dm_uzu_Input_Size_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Input_Size_reg);

		dm_uzu_Input_Size_REG.hor_input_size = ptInSize->nWidth;
		dm_uzu_Input_Size_REG.ver_input_size = ptInSize->nLength;
		rtd_outl(SCALEUP_DM_UZU_Input_Size_reg, dm_uzu_Input_Size_REG.regValue);
	       //smooth_toggle_color_twostep_scaleup_set(display, ptInSize, ptOutSize);
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
#if 1
		scaleup_ds_uzu_scale_hor_factor_RBUS		ds_uzu_Scale_Hor_Factor_REG;
		scaleup_ds_uzu_scale_ver_factor_RBUS		ds_uzu_Scale_Ver_Factor_REG;
		scaleup_ds_uzu_initial_value_RBUS			ds_uzu_Initial_Value_REG;

		ds_uzu_Initial_Value_REG.regValue = rtd_inl(SCALEUP_DS_UZU_Initial_Value_reg);
		ds_uzu_Scale_Ver_Factor_REG.regValue = rtd_inl(SCALEUP_DS_UZU_Scale_Ver_Factor_reg);
		ds_uzu_Scale_Hor_Factor_REG.regValue = rtd_inl(SCALEUP_DS_UZU_Scale_Hor_Factor_reg);

		if (ptOutSize->nWidth > ptInSize->nWidth)    // H scale-up, set h scaling factor
		{
			Hini = 0xff;
			ds_uzu_Initial_Value_REG.hor_ini = Hini;
			//nFactor = (((unsigned int)(ptInSize->nWidth)<< 21) - ((unsigned int)Hini<<13))  / (ptOutSize->nWidth-1);
			ulparam1 = (unsigned long long)(ptInSize->nWidth)<< 21;
			ulparam2 = (unsigned long long)(ptOutSize->nWidth);
			do_div(ulparam1, ulparam2);
			nFactor = ulparam1;
			//nFactor = (((unsigned int)(ptInSize->nWidth)<< 21))  / (ptOutSize->nWidth);
			//	rtd_pr_smt_info("HScale up factor before ROUND = %x, in Wid=%x, out Wid=%x \n", nFactor, ptOutSize->nLength, ptOutSize->nWidth);
			nFactor = (unsigned long long)SHR((nFactor + 1), 1); //rounding
			//	rtd_pr_smt_info("HScale up factor = %x\n", nFactor);
		} else {
			nFactor = 0xffffff;
		}
		ds_uzu_Scale_Hor_Factor_REG.regValue = nFactor;



		if (ptOutSize->nLength > ptInSize->nLength) {   // V scale-up, set v scaling factor
			if (imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE)
				Vini = 0xC0;//0x78;
			else
				Vini = 0xFF;
			ds_uzu_Initial_Value_REG.ver_ini = Vini;
			//nFactor = ((unsigned int)((ptInSize->nLength) << 20) - ((unsigned int)Vini<<12)) / ((ptOutSize->nLength)-1);
			ulparam1 = (unsigned long long)(ptInSize->nLength) << 21;
			ulparam2 = (unsigned long long)(ptOutSize->nLength);
			do_div(ulparam1, ulparam2);
			nFactor = ulparam1;
			//nFactor = ((unsigned int)((ptInSize->nLength) << 21)) / ((ptOutSize->nLength));
			//	rtd_pr_smt_info("VScale up factor before ROUND= %x, in Len=%x, out Len=%x\n", nFactor, ptInSize->nLength, ptOutSize->nLength);
			nFactor = (unsigned long long)SHR((nFactor + 1), 1); //rounding

			//	rtd_pr_smt_info("VScale up factor = %x\n", nFactor);
		} else {
			nFactor = 0xffffff;
		}
		ds_uzu_Scale_Ver_Factor_REG.regValue = nFactor;


		rtd_outl(SCALEUP_DS_UZU_Scale_Ver_Factor_reg, ds_uzu_Scale_Ver_Factor_REG.regValue);
		rtd_outl(SCALEUP_DS_UZU_Scale_Hor_Factor_reg, ds_uzu_Scale_Hor_Factor_REG.regValue);
		rtd_outl(SCALEUP_DS_UZU_Initial_Value_reg, ds_uzu_Initial_Value_REG.regValue);


		ds_uzu_Input_Size_REG.regValue = rtd_inl(SCALEUP_DS_UZU_Input_Size_reg);
		ds_uzu_Input_Size_REG.hor_input_size = ptInSize->nWidth;
		//frank@0602 solve IC BUG to avoid main channel bandwidth not enough
		//ds_uzu_Input_Size_REG.ver_input_size = ptInSize->nLength;
		ds_uzu_Input_Size_REG.ver_input_size = ptInSize->nLength;
		rtd_outl(SCALEUP_DS_UZU_Input_Size_reg, ds_uzu_Input_Size_REG.regValue);
#endif
	}
#endif //end of #ifdef CONFIG_DUAL_CHANNEL
}
void smooth_toggle_drvif_color_ultrazoom_H12tapDeRing(unsigned char bEnable,unsigned char Level)
{
	int su_dering_set[6][9] = {
		{1, 1, 60, 1, 1, 2, 150, 0, 4}, /*level 0*/ //rord.tsao modify from 95->150
		{1, 1, 70, 1, 1, 2, 105, 0, 4},/*level 1*/
		{1, 1, 80, 1, 1, 2, 115, 0, 4},/*level 2*/
		{1, 1, 90, 1, 1, 2, 125, 0, 4},/*level 3*/
		{1, 1, 100, 1, 1, 2, 135, 0, 4},/*level 4*/
		{1, 1, 110, 1, 1, 2, 140, 0, 4},/*level 5*/
	};

	scaleup_dm_uzu_12tap_dering_RBUS dm_uzu_12tap_dering_REG;
	scaleup_dm_uzu_12tap_dering_cross_RBUS dm_uzu_12tap_dering_cross_REG;

	dm_uzu_12tap_dering_REG.regValue = rtd_inl(SCALEUP_DM_UZU_12tap_dering_reg);
	dm_uzu_12tap_dering_cross_REG.regValue = rtd_inl(SCALEUP_DM_UZU_12tap_dering_cross_reg);

	dm_uzu_12tap_dering_REG.h4h12_blend_en = su_dering_set[Level][0];
	dm_uzu_12tap_dering_REG.h4h12_blend_range = su_dering_set[Level][1]; /*128, 64, 32, 16*/
	dm_uzu_12tap_dering_REG.h4h12_blend_lowbound = su_dering_set[Level][2];

	dm_uzu_12tap_dering_REG.lpf_blend_en = su_dering_set[Level][3];
	dm_uzu_12tap_dering_REG.lpf_blend_mask_sel = su_dering_set[Level][4];
	dm_uzu_12tap_dering_REG.lpf_blend_range = su_dering_set[Level][5]; /*128, 64, 32, 16*/
	dm_uzu_12tap_dering_REG.lpf_blend_lowbound = su_dering_set[Level][6];

	dm_uzu_12tap_dering_REG.level_flatness_coeff = su_dering_set[Level][7];
	dm_uzu_12tap_dering_REG.level_maxmin_coeff = su_dering_set[Level][8];

	dm_uzu_12tap_dering_REG.dering_dbg_mode = 0;

	dm_uzu_12tap_dering_cross_REG.dering_cross_en = 0;
	dm_uzu_12tap_dering_cross_REG.dering_cross_lowbd = 8; // lowbd = (0~31)*8
	dm_uzu_12tap_dering_cross_REG.dering_cross_slope = 6; // slope = (0~7 +2)/16
	dm_uzu_12tap_dering_cross_REG.dering_edgelevel_gain = 0; // 0:255, 1~7:(3~9)*Eedge

	rtd_outl(SCALEUP_DM_UZU_12tap_dering_reg, dm_uzu_12tap_dering_REG.regValue);
	rtd_outl(SCALEUP_DM_UZU_12tap_dering_cross_reg, dm_uzu_12tap_dering_cross_REG.regValue);
}


void avdmdsmooth_color_ultrazoom_config_scaling_up(unsigned char display)
{
	/*
	   unsigned char H8tap_en=1;
	   unsigned char hor8_table_sel=2;
	   unsigned char V6tap_en=0, v6_table_sel=0, v6_adap_en=0;
	   */

	// Load IC default table
	// H8tap_en=1; hor8_table_sel=1;
	// V6tap_en=1; v6_table_sel=1; v6_adap_en=0
	scaleup_dm_uzu_v8ctrl_RBUS dm_uzu_V6CTRL_reg;
	scaleup_dm_uzu_ctrl_RBUS dm_uzu_Ctrl_REG;
	scaleup_dm_uzumain_h_partial_drop_RBUS h_partial_drop_reg;
	scaleup_dm_uzumain_v_partial_drop_RBUS v_partial_drop_reg;

	SMOOTH_TOGGLE_SIZE inSize, outSize;

	//unsigned char H8tap_en = 1;
//	unsigned char hor8_table_sel = 1;// if 2, need to init table //rika 20140716 fix 576i,480p,480i atv bad screen issue
//	unsigned char v6_table_sel = 0;
	///unsigned char V6tap_en = 0, v6_table_sel = 0, v6_adap_en = 0;
	//rika 20140617 V6tap_en should be 0 when H_WID > 960

	unsigned int  panorama;


	if(ratio_type == SLR_RATIO_PANORAMA)
	{
		panorama = TRUE;
	}
	else
	{
		panorama = FALSE;
	}

	//cal scale up parameter and set after
	dm_uzu_Ctrl_REG.regValue = rtd_inl(SCALEUP_DM_UZU_Ctrl_reg);
	if (display == SLR_MAIN_DISPLAY)
	{	//rtd_pr_smt_debug("_rk UZU %d\n",__LINE__);
		dm_uzu_Ctrl_REG.v_zoom_en = (Scaler_DispGetScaleStatus(display, SLR_SCALE_V_UP) != 0);//(GET_VSCALE_UP(info->display)!=0);
		dm_uzu_Ctrl_REG.h_zoom_en = (Scaler_DispGetScaleStatus(display, SLR_SCALE_H_UP) != 0);//(GET_HSCALE_UP(info->display)!=0);
		if(dvrif_memory_compression_get_enable(display) == TRUE){
			dm_uzu_Ctrl_REG.in_fmt_conv = FALSE;
		}else{
			dm_uzu_Ctrl_REG.in_fmt_conv = !(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC)) && Scaler_DispGetStatus(display, SLR_DISP_422CAP);
		}
		dm_uzu_Ctrl_REG.video_comp_en = 0;
		dm_uzu_Ctrl_REG.bypassfornr = !Scaler_DispGetScaleStatus(display, SLR_SCALE_V_UP); // nr always available
		//dm_uzu_Ctrl_REG.bypassfornr = 1;//rika crop 1600x900  is normal when 1 => if V6tap_en ==0, it will be normal
		//	rtd_outl(SCALEUP_DM_UZU_CTRL_VADDR, dm_uzu_Ctrl_REG.regValue);  //rika 20140609 need review
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{	//rtd_pr_smt_debug("_rk UZU %d\n",__LINE__);
		scaleup_ds_uzu_ctrl_RBUS ds_uzu_Ctrl_REG;
		ds_uzu_Ctrl_REG.regValue = rtd_inl(SCALEUP_DS_UZU_Ctrl_reg);
		ds_uzu_Ctrl_REG.v_zoom_en = (Scaler_DispGetScaleStatus(display, SLR_SCALE_V_UP) != 0);
		ds_uzu_Ctrl_REG.h_zoom_en = (Scaler_DispGetScaleStatus(display, SLR_SCALE_H_UP) != 0);
		ds_uzu_Ctrl_REG.video_comp_en = 0;
		ds_uzu_Ctrl_REG.in_fmt_conv = !(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC)) && Scaler_DispGetStatus(display, SLR_DISP_422CAP);

		if (Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_INTERLACE)) {
			ds_uzu_Ctrl_REG.odd_inv = 1;/*fix sub di shaking when interlace*/
 				ds_uzu_Ctrl_REG.video_comp_en = 1;

		} else {
			ds_uzu_Ctrl_REG.odd_inv = 0;
			ds_uzu_Ctrl_REG.video_comp_en = 0;
		}
		rtd_outl(SCALEUP_DS_UZU_Ctrl_reg, ds_uzu_Ctrl_REG.regValue);
	}
#endif//end of #ifdef CONFIG_DUAL_CHANNEL
	inSize.nLength = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN); //GET_MEMORY_DISP_LENGTH(info->display);
	inSize.nWidth = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID); //GET_MEMORY_DISP_WIDTH(info->display);


	//outSize.nLength = zoom_disp_vsize; //info->DispLen;
	//outSize.nWidth = zoom_disp_hsize; //info->DispWid;

	outSize.nLength = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_LEN); //info->DispLen;
	outSize.nWidth = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_WID); //info->DispWid;

	//sync from ultrazoom.cpp to fix bad screen through uzu rika 20141009
#if 1//#ifdef TV013UI_1 // [3D] horizontal overscan in SBS 3D mode to avoid dirty lines in some content
	//init value
	IoReg_Write32(SCALEUP_DM_UZUMAIN_H_PARTIAL_DROP_reg,0x0);
	IoReg_Write32(SCALEUP_DM_UZUMAIN_V_PARTIAL_DROP_reg, 0x0);
	h_partial_drop_reg.regValue = IoReg_Read32(SCALEUP_DM_UZUMAIN_H_PARTIAL_DROP_reg);
	v_partial_drop_reg.regValue = IoReg_Read32(SCALEUP_DM_UZUMAIN_V_PARTIAL_DROP_reg);
	// 3D mode UZU overscan handler
#if 0//remove cause no 3d @zoom_smooth rika 20141009
#ifdef ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE
	xxxx
		// update H/V scaling up enable control bit for 3D overscan
		if(drvif_color_ultrazoom_set_3D_overscan(&inSize) != 0)
		{
			scaleup_dm_uzu_ctrl_RBUS dm_uzu_Ctrl_REG;
			dm_uzu_Ctrl_REG.regValue = IoReg_Read32(SCALEUP_DM_UZU_CTRL_VADDR);

			Scaler_DispSetScaleStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_SCALE_V_UP,(outSize.nLength > inSize.nLength? TRUE: FALSE));
			Scaler_DispSetScaleStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_SCALE_H_UP,(outSize.nWidth > inSize.nWidth? TRUE: FALSE));

			dm_uzu_Ctrl_REG.v_zoom_en = (Scaler_DispGetScaleStatus(display,SLR_SCALE_V_UP)!=0);
			dm_uzu_Ctrl_REG.h_zoom_en = (Scaler_DispGetScaleStatus(display,SLR_SCALE_H_UP)!=0);
			//Elsie 20140529: 4k2k should not use VCTI
			if((Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR) == _MODE_4k2kI30) || (Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR) == _MODE_4k2kP30))
				dm_uzu_Ctrl_REG.bypassfornr = 0;
			else
				dm_uzu_Ctrl_REG.bypassfornr = !Scaler_DispGetScaleStatus(display,SLR_SCALE_V_UP); // nr always available
			IoReg_Write32(SCALEUP_DM_UZU_CTRL_VADDR, dm_uzu_Ctrl_REG.regValue);
			rtd_pr_smt_info("[3D] dm_uzu_Ctrl_REG.regValue=%x\n", dm_uzu_Ctrl_REG.regValue);
		}
		else
#endif // #ifdef ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE
#endif
		{


			if(drvif_color_get_scaleup_cuthor4line())
			{
				h_partial_drop_reg.hor_m_back_drop = 4;
				h_partial_drop_reg.hor_m_front_drop = 4;
				inSize.nWidth = (inSize.nWidth) - 8;
			}

			if(drvif_color_get_scaleup_cutver2line())
			{
				if (Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_INTERLACE))
				{
					v_partial_drop_reg.ver_m_back_drop = 4;
					v_partial_drop_reg.ver_m_front_drop = 4;
					inSize.nLength = (inSize.nLength) - 8;
					rtd_pr_smt_debug("_rk %s %d -8 yo\n",__FUNCTION__,__LINE__);
				}
				else
				{
					v_partial_drop_reg.ver_m_back_drop = 2;
					v_partial_drop_reg.ver_m_front_drop = 2;
					inSize.nLength = (inSize.nLength) - 4;
				}
			}
			IoReg_Write32(SCALEUP_DM_UZUMAIN_H_PARTIAL_DROP_reg,h_partial_drop_reg.regValue);
			IoReg_Write32(SCALEUP_DM_UZUMAIN_V_PARTIAL_DROP_reg,v_partial_drop_reg.regValue);
		}
#endif


	//fix 3d

	//if (inSize.nWidth <= 720 && outSize.nWidth >= 1280)
	//	H8tap_en = 1;
	//else
		//H8tap_en = 1;


	if (display == SLR_MAIN_DISPLAY)
	{
		dm_uzu_Ctrl_REG.hor_mode_sel = 2; //13.07.26 always hor_mode_sel set 4tap due to 8tap have issue
		//dm_uzu_Ctrl_REG.hor10_table_sel = hor8_table_sel; //20151118 roger mark, control by PQ table	//0:table-1, 1:table-2

		if(dm_uzu_Ctrl_REG.hor_mode_sel==2)
			smooth_toggle_drvif_color_ultrazoom_H12tapDeRing(1,3);

		rtd_outl(SCALEUP_DM_UZU_Ctrl_reg, dm_uzu_Ctrl_REG.regValue);

		dm_uzu_V6CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_V8CTRL_reg);
		//dm_uzu_V6CTRL_reg.v6tap_en = V6tap_en;
		//dm_uzu_V6CTRL_reg.v6tap_adapt_en= v6_adap_en;
		//dm_uzu_V6CTRL_reg.v8tap_table_sel = v6_table_sel;	//20151118 roger mark, control by PQ table
		rtd_outl(SCALEUP_DM_UZU_V8CTRL_reg, dm_uzu_V6CTRL_reg.regValue);

		#if 0//remove from merlin3
		scaleup_dm_uzu_adaptv8ctrl_RBUS dm_uzu_AdaptV6CTRL_reg;
		scaleup_dm_uzumain_adapt12tap_RBUS dm_uzuMAIN_Adapt12tap;
		dm_uzuMAIN_Adapt12tap.regValue = rtd_inl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr);
		dm_uzuMAIN_Adapt12tap.adapt12_upbnd = 0x10;
		dm_uzuMAIN_Adapt12tap.adapt12_sec3 = 0x00;
		dm_uzuMAIN_Adapt12tap.adapt12_sec2 = 0x00;
		dm_uzuMAIN_Adapt12tap.adapt12_sec1	= 0x00;
		rtd_outl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr, dm_uzuMAIN_Adapt12tap.regValue);
		dm_uzu_AdaptV6CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_AdaptV8CTRL_reg);
		dm_uzu_AdaptV6CTRL_reg.adaptv8_upbnd = 75;
		dm_uzu_AdaptV6CTRL_reg.adaptv8_sec3 = 56;
		dm_uzu_AdaptV6CTRL_reg.adaptv8_sec2 = 37;
		dm_uzu_AdaptV6CTRL_reg.adaptv8_sec1 = 18;
		rtd_outl(SCALEUP_DM_UZU_AdaptV8CTRL_reg, dm_uzu_AdaptV6CTRL_reg.regValue);
		#endif
	}

	zoom_imd_smooth_color_ultrazoom_setscaleup(display, &inSize, &outSize, panorama);	// Display scaling up control, linear or non-linear either
}

static StructSrcRect inputregion_pre[MAX_DISP_CHANNEL_NUM];
static StructSrcRect outputregion_pre[MAX_DISP_CHANNEL_NUM];
static StructSrcRect inputTiming_pre[MAX_DISP_CHANNEL_NUM];

void zoom_record_input_output_pre_size(unsigned char display,StructSrcRect inputsize,StructSrcRect inputTiming,StructSrcRect outputsize)
{

	inputregion_pre[display].srcx = inputsize.srcx;
	inputregion_pre[display].srcy = inputsize.srcy;
	inputregion_pre[display].src_wid = inputsize.src_wid;
	inputregion_pre[display].src_height = inputsize.src_height;

	inputTiming_pre[display].srcx = inputTiming.srcx;
	inputTiming_pre[display].srcy = inputTiming.srcy;
	inputTiming_pre[display].src_wid = inputTiming.src_wid;
	inputTiming_pre[display].src_height = inputTiming.src_height;

	outputregion_pre[display].srcx = outputsize.srcx;
	outputregion_pre[display].srcy = outputsize.srcy;
	outputregion_pre[display].src_wid = outputsize.src_wid;
	outputregion_pre[display].src_height = outputsize.src_height;

	if((outputregion_pre[display].src_wid%4)>0){
		outputregion_pre[display].src_wid = (outputregion_pre[display].src_wid&(~3))+4;
	}
	//rtd_pr_smt_emerg("[record region]pre:input wid:%d,len:%d\n output wid:%d,len:%d\n",inputregion_pre[display].src_wid,inputregion_pre[display].src_height,outputregion_pre[display].src_wid ,outputregion_pre[display].src_height);
}

void zoom_clear_input_output_region_record(unsigned char display)
{
	inputregion_pre[display].srcx = 0;
	inputregion_pre[display].srcy = 0;
	inputregion_pre[display].src_height = 0;
	inputregion_pre[display].src_wid = 0;

	inputTiming_pre[display].srcx = 0;
	inputTiming_pre[display].srcy = 0;
	inputTiming_pre[display].src_height = 0;
	inputTiming_pre[display].src_wid = 0;

	outputregion_pre[display].srcx = 0;
	outputregion_pre[display].srcy = 0;
	outputregion_pre[display].src_height = 0;
	outputregion_pre[display].src_wid = 0;

	//rtd_pr_smt_emerg("[clear record]pre:input wid:%d,len:%d\n output wid:%d,len:%d\n",inputregion_pre[display].src_wid,inputregion_pre[display].src_height,outputregion_pre[display].src_wid ,outputregion_pre[display].src_height);
}

unsigned char check_rotate_ratio_the_same_size(unsigned char display,StructSrcRect input_size,StructSrcRect input_timing,StructSrcRect output_size)
{//True: The same. False: The different
	unsigned char result = TRUE;
	if(display == SLR_MAIN_DISPLAY) {
		if((inputregion_pre[display].srcx != input_size.srcx) || (inputregion_pre[display].srcy != input_size.srcy) ||
			(inputregion_pre[display].src_height != input_size.src_height) || (inputregion_pre[display].src_wid != input_size.src_wid)) {
			result = FALSE;
		}
		#ifdef NEW_INPUT_REGION_BASE_ON_HAL_ORIGINAL_INPUT
		if((inputTiming_pre[display].srcx != input_timing.srcx) || (inputTiming_pre[display].srcy != input_timing.srcy) ||
			(inputTiming_pre[display].src_height != input_timing.src_height) || (inputTiming_pre[display].src_wid != input_timing.src_wid)) {
			result = FALSE;
		}
		#endif
		if((outputregion_pre[display].srcx != output_size.srcx) || (outputregion_pre[display].srcy != output_size.srcy) ||
			(outputregion_pre[display].src_height != output_size.src_height) || (outputregion_pre[display].src_wid != output_size.src_wid)) {
			result = FALSE;
		}
	}

	rtd_pr_smt_debug("input:[%d,%d,%d,%d]->[%d,%d,%d,%d],timing:[%d,%d,%d,%d]->[%d,%d,%d,%d],output:[%d,%d,%d,%d]->[%d,%d,%d,%d],result=%d\n",
	inputregion_pre[display].srcx,inputregion_pre[display].srcy,inputregion_pre[display].src_wid,inputregion_pre[display].src_height,
	input_size.srcx,input_size.srcy,input_size.src_wid,input_size.src_height,
	inputTiming_pre[display].srcx,inputTiming_pre[display].srcy,inputTiming_pre[display].src_wid,inputTiming_pre[display].src_height,
	input_timing.srcx,input_timing.srcy,input_timing.src_wid,input_timing.src_height,
	outputregion_pre[display].srcx,outputregion_pre[display].srcy,outputregion_pre[display].src_wid,outputregion_pre[display].src_height,
	output_size.srcx,output_size.srcy,output_size.src_wid,output_size.src_height,result);

	return result;
}

void print_scaler_info(unsigned char display)
{

	rtd_pr_smt_debug("\33[35m\n_rk[scaler]H_ACT_WID_PRE=%d, V_ACT_LEN_PRE=%d, H_ACT_STA_PRE=%d, V_ACT_STA_PRE=%d,H_TOTAL=%d, V_TOTAL=%d\33[m\n",
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID_PRE),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN_PRE),

		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA_PRE),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA_PRE),

		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_H_LEN),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_V_LEN)

      );

	rtd_pr_smt_debug("\33[35m_rk[scaler]H_ACT_WID=%d, H_ACT_STA=%d, V_ACT_LEN=%d, V_ACT_STA=%d\33[m\n",

		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA),

		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA)

      );

	rtd_pr_smt_debug("\33[35m_rk[scaler]CapWid=%d, CapLen=%d, DI_WID=%d\33[m\n",
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_WID)

      );

	rtd_pr_smt_debug("\33[35m_rk[scaler]MEM_ACT_HSTA=%d, MEM_ACT_VSTA=%d, MEM_ACT_WID=%d, MEM_ACT_LEN=%d\33[m\n",
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA),
		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA),

		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID),

		Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN));


	rtd_pr_smt_debug("\33[35m_rk[scaler]DISP_RTNR:%d,DISP_10BIT:%d,DISP_422CAP:%d\33[m\n",Scaler_DispGetStatus(display, SLR_DISP_RTNR), Scaler_DispGetStatus(display, SLR_DISP_10BIT),Scaler_DispGetStatus(display, SLR_DISP_422CAP));

}

#define _PROTECT_VDELAY 4
#define _PROTECT_MINI_VDELAY 2

int zoom_check_size_error(unsigned char display)
{	//added by rika 20140627
	//need to review more20140627
	int failflag = ZOOM_STATE_OK;
	UINT32 vgip_h_delay = 0;

	if(zoom_crop_hsize[display] > INPUT_WID){
		zoom_crop_hsize[display] = INPUT_WID;
		rtd_pr_smt_debug("_rk[warn] zoom_crop_hsize can't be >INPUT_WID\n");
	}

	if (Scaler_DispGetStatus(display, SLR_DISP_INTERLACE)) {
		if(zoom_crop_vsize[display] > (INPUT_LEN<<1)){
			zoom_crop_vsize[display] = (INPUT_LEN<<1);
			rtd_pr_smt_debug("_rk[warn]zoom_crop_vsize can't be >INPUT_LEN\n");
		}
	}
	else	{
		if(zoom_crop_vsize[display] > INPUT_LEN){
			zoom_crop_vsize[display] = INPUT_LEN;
			rtd_pr_smt_debug("_rk[warn]zoom_crop_vsize can't be >INPUT_LEN\n");
		}
	}

	if(zoom_disp_hsize[display] > DISP_WID){
		rtd_pr_smt_debug("_rk[warn] zoom_disp_hsize>DISP_WID \n");
		zoom_disp_hsize[display] = DISP_WID;
		///failflag=ZOOM_STATE_FAIL;
	}

	if(zoom_disp_vsize[display] > DISP_LEN){
		rtd_pr_smt_debug("_rk[warn] zoom_disp_vsize>DISP_LEN \n");
		zoom_disp_vsize[display] = DISP_LEN;
		///failflag=ZOOM_STATE_FAIL;
	}
	//----end of too large case
	if(zoom_disp_hsize[display] == 0){
		rtd_pr_smt_debug("_rk[warn] zoom_disp_hsize=0\n");
		zoom_disp_hsize[display] = DISP_WID;
	}

	if(zoom_disp_vsize[display] == 0){
		rtd_pr_smt_debug("_rk[warn] zoom_disp_vsize=0\n");
		zoom_disp_vsize[display] = DISP_LEN;
	}


	if(zoom_crop_hsize[display] == 0){
		rtd_pr_smt_debug("_rk zoom_crop_hsize can't be 0 error:1st\n");
		zoom_crop_hsize[display] = INPUT_WID;
		rtd_pr_smt_debug("_rk zoom_crop_hsize reset:%d\n", zoom_crop_hsize[display]);

	}

	if(zoom_crop_vsize[display] == 0){
		rtd_pr_smt_debug("_rk zoom_crop_vsize can't be 0 error 1st\n");
		zoom_crop_vsize[display] = INPUT_LEN;
		rtd_pr_smt_debug("_rk zoom_crop_vsize reset:%d\n", zoom_crop_vsize[display]);


	}
	//---end of zero case

	rtd_pr_smt_debug("_rk display:%d ,set_info:zoom_crop_hpos:%d,zoom_crop_vpos:%d,zoom_crop_hsize:%d,zoom_crop_vsize:%d\n", display,zoom_crop_hpos[display], zoom_crop_vpos[display], zoom_crop_hsize[display], zoom_crop_vsize[display]);
	rtd_pr_smt_debug("_rk display:%d ,set_info:zoom_disp_hpos:%d,zoom_disp_vpos:%d,zoom_disp_hsize:%d,zoom_disp_vsize:%d\n", display,zoom_disp_hpos[display], zoom_disp_vpos[display], zoom_disp_hsize[display], zoom_disp_vsize[display]);



	//-----------end of >

	//this condition should not fail so remove this part rika 20141209
#if 0
	//sync from sirius
	//rika 20141013 fix
	//when crop zoom_crop_hsize:1768,zoom_crop_vsize[display]:498
	//DISP panel is 1366x768



	if(zoom_crop_hsize[display] > DISP_WID){//or h_total?
		rtd_pr_smt_debug("_rk zoom_crop_hsize[display] > DISP_WID error\n");
		failflag=ZOOM_STATE_FAIL;
	}

	if(zoom_crop_vsize[display] > DISP_LEN){//or v_total?
		rtd_pr_smt_debug("_rk zoom_crop_vsize[display] > DISP_LEN error\n");
		failflag = ZOOM_STATE_FAIL;
	}
#endif


  //int law_h_end = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE + imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE;
  //int law_v_end = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA_PRE + imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE;
	  if(Get_DisplayMode_Src(display) == VSC_INPUTSRC_AVD){
		  //if input source is AVD, it should check H_delay.
		  vgip_h_delay = IoReg_Read32(display ? SUB_VGIP_VGIP_CHN2_CTRL_reg : VGIP_VGIP_CHN1_DELAY_reg);
		  vgip_h_delay = (vgip_h_delay & 0x3fff0000)>>16;
  //	  if((zoom_crop_hpos[display] + zoom_crop_hsize[display] - vgip_h_delay) > imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL){
		  if((zoom_crop_hpos[display] + zoom_crop_hsize[display] - _H_POSITION_MOVE_TOLERANCE) > imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL){

			  rtd_pr_smt_debug("_rk[zoom][warn][AVD] crop_hpos+crop_hsize - vgip_h_delay > H_TOTAL \n");
			  zoom_crop_hpos[display] = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA_PRE);
			  //failflag = ZOOM_STATE_FAIL;
		  }
	  }
	  else{
		  if((zoom_crop_hpos[display]+zoom_crop_hsize[display]) > imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL){
			  rtd_pr_smt_debug("_rk[zoom][warn] crop_hpos+crop_hsize>H_TOTAL \n");
			  zoom_crop_hpos[display] = 0;
			  //failflag = ZOOM_STATE_FAIL;
		  }
	  }


		if (Scaler_DispGetStatus(display, SLR_DISP_INTERLACE)) {
			if((zoom_crop_vpos[display]+zoom_crop_vsize[display]) > (imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL<<1)){
				rtd_pr_smt_debug("_rk[zoom][warn] crop_vpos+crop_vsize>V_TOTAL \n");
				zoom_crop_vpos[display] = 0;
			   ///failflag=ZOOM_STATE_FAIL;
			}
		}
		else	{
		  if((zoom_crop_vpos[display]+zoom_crop_vsize[display]) > imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL){
			  rtd_pr_smt_debug("_rk[zoom][warn] crop_vpos+crop_vsize>V_TOTAL \n");
			  zoom_crop_vpos[display] = 0;
		 	 ///failflag=ZOOM_STATE_FAIL;
		  }
	  }

	//---------end of > total
#if 0
	if(zoom_crop_hsize[display] == 0){
		rtd_pr_smt_debug("_rk zoom_crop_hsize[display] can't be 0 error 2nd\n");
		failflag = ZOOM_STATE_FAIL;

	}

	if(zoom_crop_vsize[display] == 0){
		rtd_pr_smt_debug("_rk zoom_crop_vsize can't be 0 error 2nd\n");
		failflag = ZOOM_STATE_FAIL;

	}
#endif

	//--------end of ==0
#if 0 //removed and need more review rika 20140630
	if(zoom_crop_hpos[display]+zoom_crop_hsize[display] > DISP_WID){//or h_total?
		rtd_pr_smt_debug("_rk zoom_crop_hpos[display]+zoom_crop_hsize[display] > DISP_WID error\n");
		failflag = ZOOM_STATE_FAIL;

	}

	if(zoom_crop_vpos[display]+zoom_crop_vsize[display] > DISP_LEN){////or v_total?
		rtd_pr_smt_debug("_rk zoom_crop_vpos[display]+zoom_crop_vsize[display] > DISP_LEN error\n");
		failflag = ZOOM_STATE_FAIL;

	}
#endif

	return failflag;

}
void zoom_imd_get_inputinfo(unsigned char display)
{
	if (Scaler_InputSrcGetType(display) == _SRC_VO)
	{//only vo fs need use rpc to get the real vo src size
		INPUT_WID=get_zoom_smoothtoggle_vo_hwid(display);
		INPUT_LEN=get_zoom_smoothtoggle_vo_vlen(display);

	}
	else //for vo frc,atv, other src
	{
		INPUT_WID=Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_IPH_ACT_WID_PRE);//Scaler_DispGetInputInfo(SLR_INPUT_DISP_WID);
		INPUT_LEN=Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_IPV_ACT_LEN_PRE);//Scaler_DispGetInputInfo(SLR_INPUT_DISP_LEN);
	}
}
void zoom_imd_get_dispinfo(void)
{

	DISP_LEN=Get_DISP_ACT_END_VPOS() - Get_DISP_ACT_STA_VPOS();
	DISP_WID=Get_DISP_ACT_END_HPOS() - Get_DISP_ACT_STA_HPOS();

}

unsigned char scaler_smoothtoggle_info_samecheck(unsigned char display)
{
	if ((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID_PRE))
		&& (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN_PRE))
		&& (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA_PRE))
		&& (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA_PRE == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA_PRE))

		&& (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID))
		&& (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN))
		&& (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA))
		&& (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA))

		&& (imdSmoothInfo[display].IMD_SMOOTH_CapWid ==Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID))
		&& (imdSmoothInfo[display].IMD_SMOOTH_CapLen == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN))

		&& (imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_HSTA == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA))
		&& (imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_VSTA == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA))
		&& (imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID))
		&& (imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_LEN == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN))

		&& (imdSmoothInfo[display].IMD_SMOOTH_DispLen == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_LEN))
		&& (imdSmoothInfo[display].IMD_SMOOTH_DispWid == Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DISP_WID)))
		return TRUE;
	else
		return FALSE;

}
#ifdef SMOOTH_TOGGLE_DATAFRCFS_TEST
void zoom_decide_datafsmode(unsigned char display)
{

	if(display==SLR_MAIN_DISPLAY){
		if (Scaler_InputSrcGetType(display) == _SRC_VO){

			if (get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY) && vbe_disp_get_adaptivestream_fs_mode()/*&&
				vo_force_data_mode_get_enable(display)==SLR_DISPLAY_DATA_FRC_FS_4K2K*/) {

				//needto checkagain!!! rika20151223
				if( ( zoom_disp_vsize[display] == _DISP_LEN ) && ( imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE >= 3000 ) && ( imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000 >= V_FREQ_30000_pOFFSET ) ){
					imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC= TRUE;

				} else {
					imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC= FALSE;
				}
				//smooth_toggle_mode=SLR_DISPLAY_DATA_FRC_FS_4K2K; //let adaptive stream always use the same data mode
				rtd_pr_smt_emerg("_rk %s %d [zoom][vo]adaptive always data FRC_FS4K2K\n",__FUNCTION__,__LINE__);

			}
			else
				imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC);
		}

		else{ //not vo
			imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC);

		}
	}
	else { //SUB_DISPLAY
		imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC);

	}


}
#endif
//extern void imd_smooth_modestate_decide_rtnr(void);
int zoom_imd_smooth_set_info(unsigned char display, unsigned char rcflag, unsigned char rdflag)
{
	unsigned short mode;
	int failflag = ZOOM_STATE_OK;
	unsigned int* panorama_info = NULL;
	///unsigned char thestatus=zoom_smooth_toggle_get_finish();//unused
	//unsigned char current_linein;

	//current_linein=Scaler_InputSrcGetMainChType();
	ratio_type = Scaler_DispGetRatioMode();
	rtd_pr_smt_debug("_rk %d ratio_type:%d\n",__LINE__,ratio_type);

	panorama_info = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_PANORAMA);
	if(panorama_info != NULL)
	{
		if(ratio_type == SLR_RATIO_PANORAMA)
			*panorama_info = PANORAMA_NONLINEAR;
		else
			*panorama_info = PANORAMA_LINEAR;
	}

	zoom_imd_get_inputinfo(display);
	zoom_imd_get_dispinfo();

	imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID_PRE);
	imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN_PRE);
	imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA_PRE);
	imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA_PRE = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA_PRE);

	imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID);
	imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN);
	imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA);
	imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA);

	/*
	rtd_pr_smt_emerg("<=%s=%d=> \n", __FUNCTION__, __LINE__);
	rtd_pr_smt_emerg("IMD_SMOOTH_H_ACT_WID_PRE = %d \n", imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE);
	rtd_pr_smt_emerg("IMD_SMOOTH_V_ACT_LEN_PRE = %d \n", imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE);
	rtd_pr_smt_emerg("IMD_SMOOTH_H_ACT_STA_PRE = %d \n", imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE);
	rtd_pr_smt_emerg("IMD_SMOOTH_V_ACT_STA_PRE = %d \n", imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA_PRE);
	rtd_pr_smt_emerg("IMD_SMOOTH_H_ACT_WID = %d \n", imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID);
	rtd_pr_smt_emerg("IMD_SMOOTH_V_ACT_LEN = %d \n", imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN);
	rtd_pr_smt_emerg("IMD_SMOOTH_H_ACT_STA = %d \n", imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA);
	rtd_pr_smt_emerg("IMD_SMOOTH_V_ACT_STA = %d \n", imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA);
	*/

	//update hpos/vpos calculation according to [harmony] RTWTV-1412 rika 20150105
	if (Scaler_DispGetStatus(display, SLR_DISP_INTERLACE)) {

		if (apply_crop_hpos[display] >= apply_crop_hsize[display])
			zoom_crop_hpos[display] = 0;
		else
			zoom_crop_hpos[display] = apply_crop_hpos[display];

		if ((apply_crop_hpos[display]==0) || (apply_crop_hsize[display] > Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID)))
			rtd_pr_smt_err("CropX(%d) > SourceW(%d) <=%s=%d=> \n", apply_crop_hsize[display], Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID), __FUNCTION__, __LINE__);
		zoom_crop_hsize[display] = apply_crop_hsize[display];

//		rtd_pr_smt_emerg("zoom_crop_hsize[%d] = %d \n", display, zoom_crop_hsize[display]);
//		rtd_pr_smt_emerg("zoom_crop_hpos[%d] = %d \n", display, zoom_crop_hpos[display]);

		if (apply_crop_vpos[display] >= apply_crop_vsize[display])
			zoom_crop_vpos[display] = 0x00;
		else
			zoom_crop_vpos[display] = apply_crop_vpos[display];

		if ((apply_crop_vpos[display]==0) || (apply_crop_vsize[display] > Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN)<<1))
			rtd_pr_smt_err("CropY(%d) > SourceH(%d) <=%s=%d=> \n", apply_crop_vsize[display], Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN), __FUNCTION__, __LINE__);
		zoom_crop_vsize[display] = apply_crop_vsize[display];

		if(VD_27M_OFF == fw_video_get_27mhz_mode(display) &&(Get_DisplayMode_Src(display) == VSC_INPUTSRC_AVD) ){
				//rtd_pr_smt_emerg(" Timing info refine<=%s=%d=> \n", __FUNCTION__, __LINE__);
			if ((zoom_crop_hpos[display] >=zoom_crop_hsize[display])
			||(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE<=(zoom_crop_hpos[display]+zoom_crop_hsize[display]))){
				zoom_crop_hpos[display]=0;	// protect
				zoom_crop_hsize[display]=imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE;
			}
			if ((zoom_crop_vpos[display] >=zoom_crop_vsize[display])
			||(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE<=(zoom_crop_vpos[display]+zoom_crop_vsize[display]))){
				zoom_crop_vpos[display]=0;	// protect
				if ((display == SLR_MAIN_DISPLAY)&&(Scaler_DispGetStatus(display, SLR_DISP_THRIP)))
					zoom_crop_vsize[display]=imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE<<1;	// protect
				else
					zoom_crop_vsize[display]=imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE;	// protect
			}
		}
//		rtd_pr_smt_emerg("zoom_crop_vsize[%d] = %d \n", display, zoom_crop_vsize[display]);
//		rtd_pr_smt_emerg("zoom_crop_vpos[%d] = %d \n", display, zoom_crop_vpos[display]);
	}
	else		{

		if (apply_crop_hpos[display] >= apply_crop_hsize[display])
			zoom_crop_hpos[display] = 0x00;
		else
			zoom_crop_hpos[display] = apply_crop_hpos[display];

		if ((apply_crop_hpos[display]==0) || (apply_crop_hsize[display] > Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID)))
			rtd_pr_smt_err("OffsetX(%d) > SourceW(%d) <=%s=%d=> \n", apply_crop_hsize[display], Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID), __FUNCTION__, __LINE__);
		zoom_crop_hsize[display] = apply_crop_hsize[display];

//		rtd_pr_smt_emerg("IMD_SMOOTH_SDNRH_ACT_WID_PRE = %d \n", imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_WID_PRE);
//		rtd_pr_smt_emerg("IMD_SMOOTH_SDNRH_ACT_WID = %d \n", zoom_crop_hsize[display]);
//		rtd_pr_smt_emerg("IMD_SMOOTH_SDNRH_ACT_STA = %d \n", zoom_crop_hpos[display]);

		if (apply_crop_vpos[display] >= apply_crop_vsize[display])
			zoom_crop_vpos[display] = 0x00;
		else
			zoom_crop_vpos[display] = apply_crop_vpos[display];

		if ((apply_crop_vpos[display]==0) || (apply_crop_vsize[display] > Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN)))
			rtd_pr_smt_err("OffsetY(%d) > SourceH(%d) <=%s=%d=> \n", apply_crop_vsize[display], Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN), __FUNCTION__, __LINE__);
		zoom_crop_vsize[display] = apply_crop_vsize[display];

//		rtd_pr_smt_emerg("IMD_SMOOTH_SDNRV_ACT_LEN_PRE = %d \n", imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_LEN_PRE);
//		rtd_pr_smt_emerg("IMD_SMOOTH_SDNRV_ACT_LEN = %d \n", imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_LEN);
//		rtd_pr_smt_emerg("IMD_SMOOTH_SDNRV_ACT_STA = %d \n", imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_STA);
	}

	drop_v_len[display]=0;
	drop_v_position[display]=0;

	zoom_disp_hpos[display] = apply_disp_hpos[display];
	zoom_disp_vpos[display] = apply_disp_vpos[display];
	zoom_disp_hsize[display] = apply_disp_hsize[display];
	zoom_disp_vsize[display] = apply_disp_vsize[display];

	imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_H_LEN);
	imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_V_LEN);
	imdSmoothInfo[display].IMD_SMOOTH_DispLen = zoom_disp_vsize[display];
	imdSmoothInfo[display].IMD_SMOOTH_DispWid = zoom_disp_hsize[display];

	if (Scaler_DispGetStatus(display, SLR_DISP_INTERLACE)) {
		imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE = 1;
	} else {
		imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP = 0;
		imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE = 0;
	}

	//imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_DISP_THRIP=Scaler_DispGetStatus(SLR_MAIN_DISPLAY,SLR_DISP_THRIP);


	//need fix for 2.1 rika 20140702 main,sub
	imdSmoothInfo[display].IMD_SMOOTH_DISP_422 = Scaler_DispGetStatus(display, SLR_DISP_422) ;
	imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP = Scaler_DispGetStatus(display, SLR_DISP_422CAP);


	imdSmoothInfo[display].IMD_SMOOTH_INPUT_AFD = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_AFD);
	imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000 = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_V_FREQ_1000);
	imdSmoothInfo[display].IMD_SMOOTH_H_FREQ = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_H_FREQ);


#ifdef SMOOTH_TOGGLE_DATAFRCFS_TEST
	zoom_decide_datafsmode(display);
#else
	imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC);
#endif

	rtd_pr_smt_debug("_rk fs:%d\n", imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC);
	rtd_pr_smt_debug("_rk AFD:%d\n", imdSmoothInfo[display].IMD_SMOOTH_INPUT_AFD);


	mode = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MODE_CURR);
	imdSmoothInfo[display].IMD_SMOOTH_MODE_CURR = mode;
	rtd_pr_smt_debug("_rk mode:%d\n", mode);


	imdSmoothInfo[display].IMD_SMOOTH_DI_CUT_4_LINES = fwif_scaler_decide_display_cut4line(display);

	smooth_toggle_decide_DI(display);//IMD_SMOOTH_DISP_10BIT = 0 in this rika 20140609 need review

	if(Scaler_DispGetStatus(display,SLR_DISP_MA3A)) {
		imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A=TRUE;
	} else {
		imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A=FALSE;
	}

//	imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A = FALSE;//need review
	//imdSmoothInfo[display].IMD_SMOOTH_DISP_VFLIP3 = FALSE;//need review
	imdSmoothInfo[display].IMD_SMOOTH_VFLIP = Get_PANEL_VFLIP_ENABLE();//reference : svn 758238 scaler_vscdev.c

	//only kernel-photo and kernel(HDMI,YPP) check size error
	//if vo(not photo), video smoothtoggle check size error instead
	if ( (Scaler_InputSrcGetType(display) != _SRC_VO) ||
    	 ((Scaler_InputSrcGetType(display) == _SRC_VO) && (Scaler_DispGetInputInfoByDisp(Get_DisplayMode_Port(display), SLR_INPUT_VO_SOURCE_TYPE) == 1) )
	   )
	{
		rtd_pr_smt_debug("_rk check_size error\n");
		failflag = zoom_check_size_error(display);
	}
	else
		rtd_pr_smt_debug("_rk do no check_size error\n");

#if 0
	if(Get_PANEL_VFLIP_ENABLE())
	{
		if (Get_DISP_ACT_END_VPOS() >= (zoom_disp_vpos[display]+zoom_disp_vsize[display]))
			zoom_disp_vpos[display] = Get_DISP_ACT_END_VPOS()-(zoom_disp_vpos[display]+zoom_disp_vsize[display]);
		else
			zoom_disp_vpos[display] = 0;
	}
#endif

	if(TRUE == Scaler_DispCheckRatio(SLR_RATIO_POINTTOPOINT))
	{
		rtd_pr_smt_debug("_rk P2P not support zoom in & zoom out\n");
		failflag = ZOOM_STATE_FAIL;

	}

	rtd_pr_smt_debug("_rk failflag:%d", failflag);

	#if 0
	//should not change webos parameters here @ rika20150824
	//we need to compare later @ check_same_region
	//H_ACT_STA will be updated only for HDMI and VO later @ set_vgip_capture
	if(zoom_crop_hpos[display]%2)
		zoom_crop_hpos[display] = zoom_crop_hpos[display] - 1;//Need even
	#endif

	return failflag;

}

void wait_vo_share_memory_finish(void)
{
	unsigned int timeout=0x1fffff;//change timeout time = 200ms @Crixus 20151217 ;
	unsigned int *sync_flag;
	sync_flag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTHTOGGLE_SHAREMEMORY_SYNC_FALG);
	while(timeout) {
		  if(*sync_flag == 0)
		  	break;
		  else
		  	timeout--;
		   sync_flag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTHTOGGLE_SHAREMEMORY_SYNC_FALG);
	}
	sync_flag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTHTOGGLE_SHAREMEMORY_SYNC_FALG);
	*sync_flag = 1;
	if(timeout == 0)
		rtd_pr_smt_err("\r\n####wait_vo_share_memory_finish timeout####\r\n");
}

void send_vo_freeze_shiftxy_share_memory(void)
{
	unsigned int *sync_flag = NULL;
	unsigned int *pulTemp = NULL;
	unsigned int i = 0, ulItemCount = 0;
	unsigned int timeout=0x1fffff;//change timeout time = 200ms @Crixus 20151217 ;

	unsigned int vgip_en = IoReg_Read32(VODMA_VODMA_V1INT_reg) & _BIT18;
	unsigned int vodma_go = IoReg_Read32(VODMA_VODMA_V1_DCFG_reg) & _BIT0;
	if(vgip_en && vodma_go & (!Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_INTERLACE))){//only main supports shiftxy
		return;
	}
	sync_flag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTHTOGGLE_SHIFTXY_SHAREMEMORY_SYNC_FALG);
	if(!sync_flag){
		rtd_pr_smt_debug("[warning]point is NULL @ func:%s, line:%d\n",__FUNCTION__,__LINE__);
		return;
	}

	*sync_flag = 1;

	ulItemCount = sizeof(unsigned int) /  sizeof(UINT32);
	pulTemp = sync_flag;
	for (i = 0; i < ulItemCount; i++)
		pulTemp[i] = Scaler_ChangeUINT32Endian(pulTemp[i]);

	while(timeout) {
		  if(*sync_flag == 0)
		  	break;
		  else
		  	timeout--;
		   //sync_flag = (unsigned int *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTHTOGGLE_SHIFTXY_SHAREMEMORY_SYNC_FALG);
	}

	if(timeout == 0) rtd_pr_smt_debug("[warning] timeout when wait VO->DI freeze for shiftxy\n");
	return;
}

extern int sub_shape_type;
extern unsigned short true_wide;
extern unsigned short true_high;
extern void wait_dispd_stage1_smooth_toggle_apply_done(void);

void zoom_smooth_toggle_state_info_rpc
	(unsigned char display,   unsigned int  crop_hpos, unsigned int  crop_vpos,
	    unsigned int  crop_hsize, unsigned int  crop_vsize,
	    unsigned int  disp_hpos, unsigned int   disp_vpos,
	    unsigned int  disp_hsize, unsigned int  disp_vsize    )
{
		int ret = 0;
		unsigned int ratio=0;
		unsigned int *pulDataTemp = NULL;
		unsigned int ulCount = 0, i = 0;
		SCALER_ZOOM_INFO zoom_info = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		unsigned char timeout = 0;
		Check_smooth_toggle_update_flag(display);
		wait_dispd_stage1_smooth_toggle_apply_done();//before smooth toggle, wait dispd_stage1_smooth_toggle_apply_done
	#if 1//def CONFIG_REALTEK_2K_MODEL_ENABLED
		if ((display == SLR_MAIN_DISPLAY) && drv_memory_Get_multibuffer_flag()) {
            mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
			unsigned int time_out=40;
			unsigned char buffer_match=0;
			while (time_out) {
                mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);
				buffer_match=((drv_memory_Get_multibuffer_number()-1) != mdomain_cap_cap0_buf_control_reg.cap0_buf_num) ? 1: 0;
				//rtd_pr_smt_emerg("buffer_match=%d\n",buffer_match);
				if ((buffer_match  == 0) && (drv_memory_Get_capture_multibuffer_flag() == 0)) {
					break;
				}
				time_out--;
				msleep(10);
			}
			if (time_out == 0) {
				rtd_pr_smt_emerg("multibuffer caputure timeout\n");
			} else {
                mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
				unsigned int time_out_double_buffer = 0x1fff;
				mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
				if (mdomain_disp_disp0_db_ctrl_reg.disp0_db_en) {
					while(time_out_double_buffer) {
					mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
						if (mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply == 0) {
							break;
						}
						timeout--;
					}
					if (time_out_double_buffer == 0) {
						rtd_pr_smt_emerg("multibuffer disp1_double_apply timeout\n");
					}
				}
			}
		}
	#endif

		wait_vo_share_memory_finish();//This is for wait sharememory already read
		if(Check_AP_Set_Enable_ForceBG(SLR_MAIN_DISPLAY)) {
				ppoverlay_main_display_control_rsv_RBUS main_display_control_rsv_reg;
				main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
				timeout = 30;
				while(!main_display_control_rsv_reg.m_force_bg && timeout) {
					timeout -- ;
					msleep(10);
					main_display_control_rsv_reg.regValue = IoReg_Read32(PPOVERLAY_Main_Display_Control_RSV_reg);
				}
				rtd_pr_smt_info("zoom_smooth_toggle_state_info_rpc,need wait forcebg ok timeout=%d\n",timeout);
		}
		rtd_pr_smt_debug("_rk %s %d\n",__FUNCTION__,__LINE__);
		ratio=SCALER_VO_AR_16_9_CUSTOMER2;

		//ratio_type already setted in zoom_imd_smooth_set_info? but want to use SLR_RATIO_CUSTOM in vo smooth toggle
		//SLR_RATIO_TYPE ratio_type = Scaler_DispGetRatioMode2();

		zoom_info.vo_aspect_ratio_enable=_DISABLE;//VO.c _DISABLE==smooth_toggle_get_vo_ratiotype_enable()
		//zoom_info.state=(unsigned int)get_vo_smoothtoggle_state();//setted as active in zoom_imd_smooth_toggle_vo_proc
		//for SDNR
		zoom_info.zoom_crop_adj_hpos = apply_crop_hpos[display];
		zoom_info.zoom_crop_adj_vpos = apply_crop_vpos[display];
		zoom_info.zoom_crop_adj_hsize = apply_crop_hsize[display];
		zoom_info.zoom_crop_adj_vsize = apply_crop_vsize[display];

		zoom_info.zoom_hal_orgInput_hpos = apply_timing_hpos[display];
		zoom_info.zoom_hal_orgInput_vpos = apply_timing_vpos[display];
		zoom_info.zoom_hal_orgInput_hsize = apply_timing_hsize[display];
		zoom_info.zoom_hal_orgInput_vsize = apply_timing_vsize[display];

		zoom_info.bNewAdaptiveInputFlag = apply_hal_input_type[display];
		//begin of data fs parameters
		zoom_info.customerX=zoom_info.zoom_crop_adj_hpos;//use crop_hpos directly //calculate adj in video fw for data fs
		zoom_info.customerY=zoom_info.zoom_crop_adj_vpos;//crop_vpos;//for data fs
		zoom_info.customerW=crop_hsize;//for data fs
		zoom_info.customerH=crop_vsize;//for data fs
		zoom_info.zoom_disp_hpos=disp_hpos;
		zoom_info.zoom_disp_vpos=disp_vpos;
		zoom_info.zoom_disp_hsize=disp_hsize;
		zoom_info.zoom_disp_vsize=disp_vsize;

		zoom_info.zoom_fs=imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC;
		zoom_info.videoPlane=Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_VODMA_PLANE);
		zoom_info.ScalerARMode = (VO_SCALER_AR_MODE)ratio ;//move VO_SCALER_AR_MODE to zoom_smooth_lib.h
		//rtd_pr_smt_debug("_rk %s ScalerARMode:%d\n",__FUNCTION__,zoom_info.ScalerARMode);

		zoom_info.source_type =Scaler_DispGetInputInfoByDisp(Get_DisplayMode_Port(display), SLR_INPUT_VO_SOURCE_TYPE);
		zoom_info.curr_smooth_toggle_enable=vo_zoom_go_smooth_toggle_get(display);
		zoom_info.zoom_func =ZOOM_DISP;
		zoom_info.channel=display;
		zoom_info.check_only_xy_shift_change=check_only_xy_shift_change_to_videofw[display];
		//zoom_info.overscan_flag=!fInputRegionSame[display];
		zoom_info.drop_v_position=drop_v_position[display];
		zoom_info.drop_v_len=drop_v_len[display];

		if(display == SLR_SUB_DISPLAY){
		       zoom_info.Subwindowshape = sub_shape_type;/*circle or retangle*/
		       zoom_info.SubwindowWidth = shape_wide;
		       zoom_info.SubwindowLength = shape_high;
		}else{
		       zoom_info.Subwindowshape = VSC_MGF_TYPE_RECTANGLE;/*retangle,main no need use this flag*/
		       zoom_info.SubwindowWidth = shape_wide;
		       zoom_info.SubwindowLength = shape_high;
		}

		pulDataTemp = (unsigned int*)Scaler_GetShareMemVirAddr(SCALERIOC_SENDZOOMINFO);
		ulCount = sizeof(SCALER_ZOOM_INFO) / sizeof(unsigned int);
		rtd_pr_smt_notice("[kernel][rpc]zoom_crop:%d , %d, %d, %d\n",apply_crop_hpos[display],  apply_crop_vpos[display], apply_crop_hsize[display], apply_crop_vsize[display]);
		rtd_pr_smt_notice("[kernel][rpc]zoom_disp:%d , %d, %d, %d\n",disp_hpos,  disp_vpos, disp_hsize, disp_vsize);
		// copy to RPC share memory

#ifdef CONFIG_ARM64
		memcpy_toio(pulDataTemp, &zoom_info, sizeof(SCALER_ZOOM_INFO));
#else
		memcpy(pulDataTemp, &zoom_info, sizeof(SCALER_ZOOM_INFO));
#endif
		for (i = 0; i < ulCount; i++)
			pulDataTemp[i] = Scaler_ChangeUINT32Endian(pulDataTemp[i]);

		if (0 != (ret = Scaler_SendRPC(SCALERIOC_SENDZOOMINFO,0,0)))//add 0,0 to fix compile error
		{
			rtd_pr_smt_debug("ret=%d, send VO's zoom info to driver fail !!!\n", ret);
		}
}

unsigned char Scaler_Set_VOsmoothenable(unsigned char display)
{
	//added by rika 20150108


	int ret = 0;

	if (0 != (ret =  Scaler_SendRPC(SCALERIOC_SETSMOOTHENABLE,display, RPC_SEND_VALUE_ONLY)))
	{
		rtd_pr_smt_debug("_rk[zoom]SCALERIOC_SET_VOsmoothenable fail. ret=%d\n",ret);
		return FALSE;
	}

	return TRUE;


}

unsigned char Check_vo_smooth_toggle_waitingapply_flag(unsigned char display)
{//smooth_toggle_set_waitingapply


	int ret = 0;
//	Rt_Sleep(40);
	Check_smooth_toggle_update_flag(display);
	//ret = Scaler_SendRPC(SCALERIOC_GETZOOMSTATE , 0 , 1);
//	ret = Scaler_SendRPC(SCALERIOC_GETZOOMSTATE,display, RPC_SEND_VALUE_ONLY);

//	Scaler_Set_VOsmoothenable(display);
	return ret;
}




unsigned char Get_vo_smoothtoggle_dispstatus(unsigned int rpcidx)
{
	int ret = 0;
	///unsigned int timeout=0;
	//if(rpcidx==SCALERIOC_GET_V_UZD_BUFFER_MODE)
	//	rtd_pr_smt_debug("_rk SCALERIOC_GET_V_UZD_BUFFER_MODE\n");

	if((ret = Scaler_SendRPC(rpcidx, 0, 1))<0)
	{
		return _DISABLE;
	}
	else{
		return _ENABLE;
	}

}

void zoom_imd_smooth_toggle_vo_init(unsigned char display)
{

	if(Scaler_InputSrcGetType(display)!=_SRC_VO)
		return;


	if(Get_vo_smoothtoggle_timingchange_flag(display))
	{//usb
		rtd_pr_smt_debug("_rk %s %d\n",__FUNCTION__,__LINE__);
		//usb @ sirius already initiliaze
	}
	else
	{//dtv

		//rtd_pr_smt_debug("_rk %s %d\n",__FUNCTION__,__LINE__);
		vo_smoothtoggle_memory_alloc(display);//DTV need to allocate memory
		panel_parameter_send_info();
	}

}

extern SOURCE_INFO_SMOOTH_TOGGLE get_vo_smoothtoggle_state(unsigned char display);
/*
   int zoom_imd_smooth_toggle_vo_proc
   added by rika 20140627
   for fpp tcl api vo source
   */
int zoom_imd_smooth_toggle_vo_proc
	(unsigned char display,   unsigned int  crop_hpos, unsigned int  crop_vpos,
	    unsigned int  crop_hsize, unsigned int  crop_vsize,
	    unsigned int  disp_hpos, unsigned int  disp_vpos,
	    unsigned int  disp_hsize, unsigned int  disp_vsize    )
{
	zoom_smooth_toggle_state_info_rpc(display,
			crop_hpos,  crop_vpos,
			crop_hsize, crop_vsize,
			disp_hpos,  disp_vpos,
			disp_hsize, disp_vsize );
	return ZOOM_STATE_OK;
}

void zoom_update_scaler_info_from_vo_smooth_toggle(unsigned char display,unsigned char scaler_before)
{
	unsigned int *pulTemp;
	unsigned int i = 0, ulItemCount = 0;
	unsigned int pre_height = Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_IPV_ACT_LEN);
	unsigned int pre_width = Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_IPH_ACT_WID);
	SMOOTH_TOGGLE_UPDATE_FWINFO_PKG *sm_info_pkg = NULL;
	SMOOTH_TOGGLE_UPDATE_FWINFO_PKG update_info;
	sm_info_pkg = (SMOOTH_TOGGLE_UPDATE_FWINFO_PKG *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTHTOGGLE_SHARE_INFO);

	if (sm_info_pkg) {
		memcpy(&update_info, sm_info_pkg, sizeof(SMOOTH_TOGGLE_UPDATE_FWINFO_PKG));
		ulItemCount = sizeof(SMOOTH_TOGGLE_UPDATE_FWINFO_PKG) /  sizeof(UINT32);
		pulTemp = (unsigned int *)&update_info;
		for (i = 0; i < ulItemCount; i++)
			pulTemp[i] = Scaler_ChangeUINT32Endian(pulTemp[i]);

        if(true == update_info.sm_info[display].done_flag){
			sm_info_pkg->sm_info[display].done_flag = false; //clear this , set it after vo smooth toggle
			if (0==scaler_before) {//after scaler need to update smoothtoggle
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_H_ACT_WID);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_V_ACT_LEN);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_STA,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_H_ACT_STA);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_STA,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_V_ACT_STA);

				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_FRAMESYNC,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_FRAMESYNC);

				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_SDNRH_ACT_STA,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_SDNR_H_ACT_STA);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_SDNRV_ACT_STA,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_SDNR_V_ACT_STA);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_SDNRH_ACT_WID,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_SDNR_H_ACT_WID);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_SDNRV_ACT_LEN,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_SDNR_V_ACT_LEN);

				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_WID,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DI_WID);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_LEN,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DI_LEN);

				/*disp*/
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DISP_LEN,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DispLen);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DISP_WID,update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DispWid);

				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_CAP_WID, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_CapWid);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_CAP_LEN, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_CapLen);

				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_MEM_ACT_HSTA, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_MEM_ACT_HSTA);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_MEM_ACT_VSTA, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_MEM_ACT_VSTA);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_MEM_ACT_LEN, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_MEM_ACT_LEN);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_MEM_ACT_WID, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_MEM_ACT_WID);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_V_FREQ_1000, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_V_FREQ_1000);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_FRAC_FS_REMOVE_RATIO, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_FRAC_FS_REMOVE_RATIO);
				Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display, SLR_INPUT_FRAC_FS_MULTIPLE_RATIO, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_FRAC_FS_MULTIPLE_RATIO);



				/* RTNR,10BIT,DISP_THRIP,MA3A*/
				Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_RTNR, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DISP_RTNR);
				Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_10BIT, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DISP_10BIT);
				Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_THRIP, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DISP_THRIP);
				Scaler_DispSetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_MA3A, update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_DISP_MA3A);
			}
#ifndef UT_flag
			//vdec source new voinfo,re-config i3ddma nn cap
			if((display == SLR_MAIN_DISPLAY) && ((Get_DisplayMode_Src(display) == VSC_INPUTSRC_VDEC) || (Get_DisplayMode_Src(display) == VSC_INPUTSRC_JPEG) || (Get_DisplayMode_Src(display) == VSC_INPUTSRC_CAMERA)) && (scalerAI_get_AIPQ_mode_enable_flag()) &&
				((pre_height != update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_V_ACT_LEN) || (pre_width != update_info.sm_info[display].smooth_toggle_info.IMD_SMOOTH_H_ACT_WID)))
				h3ddma_nn_set_recfg_flag(TRUE);
#endif // #ifndef UT_flag
		}
	}
}

int zoom_imd_smooth_toggle_config(unsigned char display, SCALER_VSC_INPUT_REGION_TYPE inputType, StructSrcRect inputRegion, StructSrcRect inputTiming, StructSrcRect outputRegion)
{
	int failflag=ZOOM_STATE_OK;
	VSC_INPUT_TYPE_T srctype;
#ifdef CONFIG_ALL_SOURCE_DATA_FS
	unsigned int hdmi_mode_wid=0,hdmi_mode_len=0;
#endif
	srctype = Get_DisplayMode_Src(display);

	//rtd_pr_smt_emerg("[SMT] zoom_imd_smooth_toggle_config!!\n");
	//change to 3-buffer mode and timing free run first when under game mode.
	if(drv_memory_get_game_mode() && get_new_game_mode_condition() && display == SLR_MAIN_DISPLAY){
		/*K5LG-2134:wait last time smoothtoglge finish to avoid gamemode close single buffer flow conflict with smoothtoggle change*/
		Check_smooth_toggle_update_flag(SLR_MAIN_DISPLAY);
		down(get_gamemode_check_semaphore());
		//trigger the timeout checking tsk
		if(smooth_toggle_game_mode_timeout_trigger_flag == _DISABLE){
			smooth_toggle_game_mode_timeout_trigger_flag = _ENABLE;
			//if run smt after scaler in game mode, do not ignore cmd check
			game_mode_set_ignore_cmd_check(_DISABLE);
			//smooth_toggle_game_mode_check_cnt = 0; //reset the timeout counter
			rtd_pr_smt_emerg("[crixus]ST game mode start!!\n");
		}
		if(smooth_toggle_game_mode_check_cnt == 0xffffffff)
			smooth_toggle_game_mode_check_cnt = 0;
		else
			smooth_toggle_game_mode_check_cnt = smooth_toggle_game_mode_check_cnt + 1;
		//rtd_pr_smt_emerg("[GAME][%s][%d] smooth_toggle_game_mode_check_cnt = %d\n",__FUNCTION__,__LINE__,smooth_toggle_game_mode_check_cnt);

		//send rpc to video fw
		drvif_memory_compression_rpc(display);
		if(!(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC)))
		{
			//change to 3-buffer
			drv_memory_GameMode_Switch_TripleBuffer();
			//disable FLL tsk flag
			if(vbe_disp_gamemode_use_fixlastline()==TRUE){
				modestate_set_fll_running_flag(_DISABLE);
			}
			//change to timing free run
			drivf_scaler_reset_freerun(DTG_MASTER);
		}
		up(get_gamemode_check_semaphore());
	}

	if((get_vsc_run_adaptive_stream(display) == ADAPTIVE_AIR_MODE))
	{
		scaler_airModeSendDispSize(display, air_mode_save_outregion_parm[display]);
	}

	apply_hal_input_type[display] = inputType;
	apply_crop_vsize_pre[display]=apply_crop_vsize[display];
	apply_crop_vpos_pre[display]=apply_crop_vpos[display];
	apply_crop_hpos[display] = inputRegion.srcx;
	apply_crop_vpos[display] = inputRegion.srcy;
	apply_crop_hsize[display] = inputRegion.src_wid;
	apply_crop_vsize[display] = inputRegion.src_height;

	apply_timing_hpos[display] = inputTiming.srcx;
	apply_timing_vpos[display] = inputTiming.srcy;
	apply_timing_hsize[display] = inputTiming.src_wid;
	apply_timing_vsize[display] = inputTiming.src_height;

	apply_disp_hpos[display] = outputRegion.srcx;
	apply_disp_vpos[display] = outputRegion.srcy;
	apply_disp_hsize[display] = outputRegion.src_wid;
	apply_disp_vsize[display] = outputRegion.src_height;

	if((apply_hal_input_type[display] == INPUT_BASE_ON_TIMING) && (srctype == VSC_INPUTSRC_AVD)){
		apply_crop_hpos[display] = Scaler_CalAVD27MWidth(display, apply_crop_hpos[display]);
		apply_crop_hsize[display] = Scaler_CalAVD27MWidth(display, apply_crop_hsize[display]);
	}
#ifdef CONFIG_ALL_SOURCE_DATA_FS
	if(apply_hal_input_type[display] == INPUT_BASE_ON_TIMING) {
		if ((srctype == VSC_INPUTSRC_HDMI) && get_i3dma_uzd()) {
			hdmi_mode_wid = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHWID);
			hdmi_mode_len = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVHEIGHT);
			if (hdmi_mode_wid >1920) {
				apply_crop_hpos[display] = apply_crop_hpos[display] * 1920/hdmi_mode_wid;
				apply_crop_hsize[display]  = apply_crop_hsize[display]  * 1920/hdmi_mode_wid;
			}
			if (hdmi_mode_len >1080) {
				apply_crop_vpos[display]  = apply_crop_vpos[display]  * 1080/hdmi_mode_len;
				apply_crop_vsize[display]  = apply_crop_vsize[display]  * 1080/hdmi_mode_len;
			}
		}
	}
#endif
	rtd_pr_smt_debug("apply_crop_vsize_pre=%d, apply_crop_vpos_pre=%d\n", apply_crop_vsize_pre[display], apply_crop_vpos_pre[display]);
	rtd_pr_smt_debug("apply_crop_hpos=%d, apply_crop_vpos=%d\n", apply_crop_hpos[display], apply_crop_vpos[display]);
	rtd_pr_smt_debug("apply_crop_hsize=%d, apply_crop_vsize=%d\n", apply_crop_hsize[display], apply_crop_vsize[display]);
	rtd_pr_smt_debug("apply_timing_hpos=%d, apply_timing_vpos=%d\n", apply_timing_hpos[display], apply_timing_vpos[display]);
	rtd_pr_smt_debug("apply_timing_hsize=%d, apply_timing_vsize=%d\n", apply_timing_hsize[display], apply_timing_vsize[display]);
	rtd_pr_smt_debug("apply_disp_hpos=%d, apply_disp_vpos=%d\n", apply_disp_hpos[display], apply_disp_vpos[display]);
	rtd_pr_smt_debug("apply_disp_hsize=%d, apply_disp_vsize=%d\n", apply_disp_hsize[display], apply_disp_vsize[display]);

	//disp width has to align 4 @Crixus 20150827
	if((apply_disp_hsize[display]%4)>0){
		apply_disp_hsize[display] = (apply_disp_hsize[display]&(~3))+4;
	}

	if(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) {
		///zoom_checkresetflag(display, zoomfunc);
		//down(get_vsc_semaphore());
		failflag = zoom_imd_smooth_toggle_proc(resetcropflag, resetdispflag, display);

//#ifdef CONFIG_ORBIT_PIXEL_SHIFT_ALGO
//		if(vbe_disp_get_orbit()==TRUE){
//			Scaler_reset_orbit();
//		}
//#endif
		//up(get_vsc_semaphore());
		return failflag;

	} else {
		rtd_pr_smt_debug("%s scaler is not active status ,can't run smt \n",__FUNCTION__);
		return ZOOM_STATE_FAIL;
	}
}

unsigned char zoom_imd_smooth_toggle_vo_flow(unsigned char display)
{
	unsigned char failflag;

	failflag=ZOOM_STATE_OK;

//	if (!Get_vo_smoothtoggle_timingchange_flag(display)) {
//		Set_vo_smoothtoggle_changeratio_flag2(_ENABLE, display);
//	}
	//print_vo_data_mode();
	if (imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC){
		//do data fs smooth toggle in vo
		rtd_pr_smt_debug("_rk VO & DATA FS\n");
	#if 0
		if (!Get_vo_smoothtoggle_timingchange_flag(display)) {
			vo_force_data_mode_set_enable(SLR_DISPLAY_DATA_FRAME_SYNC,display);
		}
	#endif
		if(imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE){
			//under data fs condition
			//we cant send 1920x540i directly to crop VO size
			//cause it will be 1920x270 @ VODMA_SyncGen_Check
			//under data frc condition,we crop vgip size, not vo size.
			//to crop vosize 1920x540i,  we need to set crop_hszie=1920,crop_visze=1080
			zoom_crop_vsize[display]*=2;
		}
		zoom_imd_smooth_toggle_vo_proc(display,
				zoom_crop_hpos[display],zoom_crop_vpos[display],zoom_crop_hsize[display],zoom_crop_vsize[display],
				zoom_disp_hpos[display],zoom_disp_vpos[display],zoom_disp_hsize[display],zoom_disp_vsize[display]
				);

	}
	else{
		rtd_pr_smt_debug("_rk VO & DATA FRC\n");
	#if 0
		//working on rika 20140628
		if (!Get_vo_smoothtoggle_timingchange_flag(display)) {
			vo_force_data_mode_set_enable(SLR_DISPLAY_DATA_FRC, display);
		}
	#endif
		zoom_imd_smooth_toggle_vo_proc(display,
				zoom_crop_hpos[display],zoom_crop_vpos[display],zoom_crop_hsize[display],zoom_crop_vsize[display],
				zoom_disp_hpos[display],zoom_disp_vpos[display],zoom_disp_hsize[display],zoom_disp_vsize[display]
				);

	}
//	failflag = Check_vo_smooth_toggle_waitingapply_flag(display);
	if (!Get_vo_smoothtoggle_timingchange_flag(display)) {
//		Set_vo_smoothtoggle_changeratio_flag2(_DISABLE, display);
		set_zoom_reset_double_state(_ENABLE,display);
	}
#if 0
	if (!Get_vo_smoothtoggle_timingchange_flag(display)) {
		zoom_imd_smooth_toggle_vo_enable(_DISABLE, display);
	}
#endif
	return failflag;
}

extern unsigned char Scaler_main_imd_pst_get_enable(void);
extern StructSrcRect get_main_dispwin(void);
extern StructSrcRect get_main_input_size(void);
extern void set_shiftxy_main_inputpre(StructSrcRect input_size );
extern void set_shiftxy_main_dispwinpre(StructSrcRect out_size );

extern void vbe_disp_orbit_position_update(unsigned char tableIdx,unsigned char force_update);
extern void vbe_disp_orbit_reset_position_overscan(void);
extern unsigned char vbe_disp_orbit_get_vo_overscan(void);
extern void vbe_disp_set_orbit_shift_info_sharememory(SCALER_ORBIT_SHIFT_INFO scalerOrbitInfo);
extern void vbe_disp_orbit_set_vo_overscan_state(unsigned char bEnable);
#ifndef UT_flag
extern StructSrcRect vbe_orbit_input_size;
extern StructSrcRect vbe_orbit_output_size;
extern StructSrcRect vbe_orbit_sub_input_size;
extern StructSrcRect vbe_orbit_sub_input_timing_size;
extern StructSrcRect vbe_orbit_sub_output_size;
#endif // #ifndef UT_flag

unsigned char zoom_check_same_region_for_SDNR(unsigned char display)
{

	StructSrcRect input_size = {apply_crop_hpos[display], apply_crop_vpos[display],apply_crop_hsize[display],apply_crop_vsize[display]};
	StructSrcRect output_size = {apply_disp_hpos[display],apply_disp_vpos[display],apply_disp_hsize[display],apply_disp_vsize[display]};

	if(vsc_get_adaptivestream_flag(display) == ADAPTIVE_AIR_MODE)
	{
		rtd_pr_smt_emerg("[SMT]display:%d, airplay always run smooth toggle\n",display);
        return ZOOM_STATE_FAIL;
	}

    if((vbe_disp_oled_orbit_mode ==_VBE_PANEL_ORBIT_AUTO_MODE) &&  vbe_disp_orbit_get_sub_overscan()&& vbe_disp_oled_orbit_enable && (SLR_SUB_DISPLAY == display))
    {

        inputregion_pre[display].srcx = apply_crop_hpos[display];
        inputregion_pre[display].srcy = apply_crop_vpos[display];
        inputregion_pre[display].src_height = apply_crop_vsize[display];
        inputregion_pre[display].src_wid = apply_crop_hsize[display];

        inputTiming_pre[display].srcx = apply_timing_hpos[display];
        inputTiming_pre[display].srcy = apply_timing_vpos[display];
        inputTiming_pre[display].src_height = apply_timing_vsize[display];
        inputTiming_pre[display].src_wid = apply_timing_hsize[display];

        outputregion_pre[display].srcx = apply_disp_hpos[display];
        outputregion_pre[display].srcy = apply_disp_vpos[display];
        outputregion_pre[display].src_height = apply_disp_vsize[display];
        outputregion_pre[display].src_wid = apply_disp_hsize[display];
        check_only_xy_shift_change_to_videofw[display] = 0;

		rtd_pr_smt_emerg("[SMT]display:%d, orbit overscan mode\n",display);
        return ZOOM_STATE_FAIL;
    }
#ifndef UT_flag
	else if((vbe_disp_oled_orbit_mode ==_VBE_PANEL_ORBIT_AUTO_MODE|| vbe_disp_oled_orbit_mode ==_VBE_PANEL_ORBIT_STORE_MODE) && vbe_disp_oled_orbit_enable)
	{
		if(SLR_MAIN_DISPLAY ==display)
		{
          /*
			vbe_disp_orbit_set_vo_overscan_state
(1);
			vbe_orbit_input_size.srcx = apply_crop_hpos[display];
			vbe_orbit_input_size.srcy = apply_crop_vpos[display];
			vbe_orbit_input_size.src_height = apply_crop_vsize[display];
			vbe_orbit_input_size.src_wid = apply_crop_hsize[display];
			vbe_orbit_output_size.srcx = apply_disp_hpos[display];
			vbe_orbit_output_size.srcy = apply_disp_vpos[display];
			vbe_orbit_output_size.src_height = apply_disp_vsize[display];
			vbe_orbit_output_size.src_wid = apply_disp_hsize[display];

			scalerOrbitMemInfo.orbit_enable = 1;
			scalerOrbitMemInfo.orbit_mode = _VBE_PANEL_OREBIT_MODE_MAX;
			scalerOrbitMemInfo.shift_range_x = 0;
			scalerOrbitMemInfo.shift_range_y = 0;
			scalerOrbitMemInfo.shift_x = 0;
			scalerOrbitMemInfo.shift_y = 0;
			scalerOrbitMemInfo.ori_input_x = apply_crop_hsize[display] > apply_disp_hsize[display]? apply_disp_hsize[display]:apply_crop_hsize[display] ;//for 4:3 ratio
			scalerOrbitMemInfo.ori_input_y = (apply_crop_vsize[display]/8)*8 ;
			vbe_disp_set_orbit_shift_info_sharememory(scalerOrbitMemInfo);*/
		}
		else if(SLR_SUB_DISPLAY == display)
		{
			vbe_orbit_sub_input_size.srcx = apply_crop_hpos[display];
			vbe_orbit_sub_input_size.srcy = apply_crop_vpos[display];
			vbe_orbit_sub_input_size.src_height = apply_crop_vsize[display];
			vbe_orbit_sub_input_size.src_wid = apply_crop_hsize[display];

			vbe_orbit_sub_input_timing_size.srcx = apply_timing_hpos[display];
			vbe_orbit_sub_input_timing_size.srcy = apply_timing_vpos[display];
			vbe_orbit_sub_input_timing_size.src_height = apply_timing_vsize[display];
			vbe_orbit_sub_input_timing_size.src_wid = apply_timing_hsize[display];

			vbe_orbit_sub_output_size.srcx = apply_disp_hpos[display];
			vbe_orbit_sub_output_size.srcy = apply_disp_vpos[display];
			vbe_orbit_sub_output_size.src_height = apply_disp_vsize[display];
			vbe_orbit_sub_output_size.src_wid = apply_disp_hsize[display];
		}
    }
    else
    {

        if(SLR_MAIN_DISPLAY ==display)
        {
            vbe_orbit_input_size.srcx = apply_crop_hpos[display];
            vbe_orbit_input_size.srcy = apply_crop_vpos[display];
            vbe_orbit_input_size.src_height = apply_crop_vsize[display];
            vbe_orbit_input_size.src_wid = apply_crop_hsize[display];
            vbe_orbit_output_size.srcx = apply_disp_hpos[display];
            vbe_orbit_output_size.srcy = apply_disp_vpos[display];
            vbe_orbit_output_size.src_height = apply_disp_vsize[display];
            vbe_orbit_output_size.src_wid = apply_disp_hsize[display];
        } else if(SLR_SUB_DISPLAY == display)
	{
		vbe_orbit_sub_input_size.srcx = apply_crop_hpos[display];
		vbe_orbit_sub_input_size.srcy = apply_crop_vpos[display];
		vbe_orbit_sub_input_size.src_height = apply_crop_vsize[display];
		vbe_orbit_sub_input_size.src_wid = apply_crop_hsize[display];

		vbe_orbit_sub_input_timing_size.srcx = apply_timing_hpos[display];
		vbe_orbit_sub_input_timing_size.srcy = apply_timing_vpos[display];
		vbe_orbit_sub_input_timing_size.src_height = apply_timing_vsize[display];
		vbe_orbit_sub_input_timing_size.src_wid = apply_timing_hsize[display];

		vbe_orbit_sub_output_size.srcx = apply_disp_hpos[display];
		vbe_orbit_sub_output_size.srcy = apply_disp_vpos[display];
		vbe_orbit_sub_output_size.src_height = apply_disp_vsize[display];
		vbe_orbit_sub_output_size.src_wid = apply_disp_hsize[display];
	}
    }
#endif // #ifndef UT_flag
#ifdef NEW_INPUT_REGION_BASE_ON_HAL_ORIGINAL_INPUT
	#if 0
	rtd_pr_smt_emerg("input region type:%d\n",Scaler_InputRegionType_Get(display));
	rtd_pr_smt_emerg("inputTimingPre:%d, %d\n",inputTiming_pre[display].src_wid,inputTiming_pre[display].src_height);
	rtd_pr_smt_emerg("inputregion_pre:%d, %d\n",inputregion_pre[display].src_wid,inputregion_pre[display].src_height);
	rtd_pr_smt_emerg("outputregion_pre:%d, %d\n",outputregion_pre[display].src_wid,outputregion_pre[display].src_height);
	rtd_pr_smt_emerg("apply crop:%d, %d, %d ,%d \n",apply_crop_hpos[display],apply_crop_vpos[display],apply_crop_hsize[display],apply_crop_vsize[display]);
	rtd_pr_smt_emerg("apply timing:%d, %d, %d ,%d \n",apply_timing_hpos[display],apply_timing_vpos[display],apply_timing_hsize[display],apply_timing_vsize[display]);
	rtd_pr_smt_emerg("apply disp:%d, %d, %d ,%d \n",apply_disp_hpos[display],apply_disp_vpos[display],apply_disp_hsize[display],apply_disp_vsize[display]);
	#endif

	if(Scaler_InputRegionType_Get(display) == INPUT_BASE_ON_HAL_ORIGINAL_INPUT){
		if(!inputTiming_pre[display].src_height || !inputTiming_pre[display].src_wid || !inputregion_pre[display].src_height || !inputregion_pre[display].src_wid ||  !outputregion_pre[display].src_height || !outputregion_pre[display].src_wid){
			inputregion_pre[display].srcx = apply_crop_hpos[display];
			inputregion_pre[display].srcy = apply_crop_vpos[display];
			inputregion_pre[display].src_height = apply_crop_vsize[display];
			inputregion_pre[display].src_wid = apply_crop_hsize[display];

			inputTiming_pre[display].srcx = apply_timing_hpos[display];
			inputTiming_pre[display].srcy = apply_timing_vpos[display];
			inputTiming_pre[display].src_height = apply_timing_vsize[display];
			inputTiming_pre[display].src_wid = apply_timing_hsize[display];

			outputregion_pre[display].srcx = apply_disp_hpos[display];
			outputregion_pre[display].srcy = apply_disp_vpos[display];
			outputregion_pre[display].src_height = apply_disp_vsize[display];
			outputregion_pre[display].src_wid = apply_disp_hsize[display];
			check_only_xy_shift_change_to_videofw[display] = 0;
			rtd_pr_smt_emerg("[SMT][new hal orgInput]display:%d, the first time to get SDNR input and output size\n",display);
            return ZOOM_STATE_FAIL;
		}

		if((inputTiming_pre[display].srcx != apply_timing_hpos[display]) || (inputTiming_pre[display].srcy != apply_timing_vpos[display])
			|| (inputTiming_pre[display].src_height != apply_timing_vsize[display]) ||(inputTiming_pre[display].src_wid != apply_timing_hsize[display]) )
		{
			// all parameters are the same
			if((outputregion_pre[display].srcx == apply_disp_hpos[display])
				&& (outputregion_pre[display].srcy == apply_disp_vpos[display])
				&& (outputregion_pre[display].src_height == apply_disp_vsize[display])
				&& (outputregion_pre[display].src_wid == apply_disp_hsize[display])

				&& (inputTiming_pre[display].src_wid * apply_crop_hpos[display] == apply_timing_hsize[display] * inputregion_pre[display].srcx)
				&& (inputTiming_pre[display].src_height * apply_crop_vpos[display] == apply_timing_vsize[display] * inputregion_pre[display].srcy)
				&& (inputTiming_pre[display].src_height * apply_crop_vsize[display] == apply_timing_vsize[display] * inputregion_pre[display].src_height)
				&& (inputTiming_pre[display].src_wid * apply_crop_hsize[display] == apply_timing_hsize[display] * inputregion_pre[display].src_wid)){
				// all parameters are the same, update input and output
				inputregion_pre[display].srcx = apply_crop_hpos[display];
				inputregion_pre[display].srcy = apply_crop_vpos[display];
				inputregion_pre[display].src_height = apply_crop_vsize[display];
				inputregion_pre[display].src_wid = apply_crop_hsize[display];

				inputTiming_pre[display].srcx = apply_timing_hpos[display];
				inputTiming_pre[display].srcy = apply_timing_vpos[display];
				inputTiming_pre[display].src_height = apply_timing_vsize[display];
				inputTiming_pre[display].src_wid = apply_timing_hsize[display];
				check_only_xy_shift_change_to_videofw[display] = 0;
				rtd_pr_smt_emerg("[SMT][new hal orgInput]the same ratio(input/intputTiming)\n");
				return ZOOM_STATE_OK;
			} else {
				rtd_pr_smt_emerg("HAL input timing changed:(%d,%d,%d,%d)->(%d,%d,%d,%d) \n",inputTiming_pre[display].srcx,inputTiming_pre[display].srcy,inputTiming_pre[display].src_wid,inputTiming_pre[display].src_height,
					apply_timing_hpos[display],apply_timing_vpos[display],apply_timing_hsize[display],apply_timing_vsize[display]);
				rtd_pr_smt_emerg("input_region:(%d,%d,%d,%d)->(%d,%d,%d,%d) \n",inputregion_pre[display].srcx,inputregion_pre[display].srcy,inputregion_pre[display].src_wid,inputregion_pre[display].src_height,
					apply_crop_hpos[display],apply_crop_vpos[display],apply_crop_hsize[display],apply_crop_vsize[display]);
				rtd_pr_smt_emerg("output_region:(%d,%d,%d,%d)->(%d,%d,%d,%d) \n",outputregion_pre[display].srcx,outputregion_pre[display].srcy,outputregion_pre[display].src_wid,outputregion_pre[display].src_height,
					apply_disp_hpos[display],apply_disp_vpos[display],apply_disp_hsize[display],apply_disp_vsize[display]);
				inputregion_pre[display].srcx = apply_crop_hpos[display];
				inputregion_pre[display].srcy = apply_crop_vpos[display];
				inputregion_pre[display].src_height = apply_crop_vsize[display];
				inputregion_pre[display].src_wid = apply_crop_hsize[display];

				inputTiming_pre[display].srcx = apply_timing_hpos[display];
				inputTiming_pre[display].srcy = apply_timing_vpos[display];
				inputTiming_pre[display].src_height = apply_timing_vsize[display];
				inputTiming_pre[display].src_wid = apply_timing_hsize[display];

				outputregion_pre[display].srcx = apply_disp_hpos[display];
				outputregion_pre[display].srcy = apply_disp_vpos[display];
				outputregion_pre[display].src_height = apply_disp_vsize[display];
				outputregion_pre[display].src_wid = apply_disp_hsize[display];
				check_only_xy_shift_change_to_videofw[display] = 0;
				return ZOOM_STATE_FAIL;
			}
		}
	}
#endif
	if(!inputregion_pre[display].src_height || !inputregion_pre[display].src_wid ||  !outputregion_pre[display].src_height || !outputregion_pre[display].src_wid){
		inputregion_pre[display].srcx = apply_crop_hpos[display];
		inputregion_pre[display].srcy = apply_crop_vpos[display];
		inputregion_pre[display].src_height = apply_crop_vsize[display];
		inputregion_pre[display].src_wid = apply_crop_hsize[display];
		outputregion_pre[display].srcx = apply_disp_hpos[display];
		outputregion_pre[display].srcy = apply_disp_vpos[display];
		outputregion_pre[display].src_height = apply_disp_vsize[display];
		outputregion_pre[display].src_wid = apply_disp_hsize[display];
		check_only_xy_shift_change_to_videofw[display] = 0;
		rtd_pr_smt_emerg("[SMT]display:%d, the first time to get SDNR input and output size\n",display);
		return ZOOM_STATE_FAIL;
	}

	if((inputregion_pre[display].src_height != apply_crop_vsize[display]) || (inputregion_pre[display].src_wid != apply_crop_hsize[display])
		|| (outputregion_pre[display].srcx != apply_disp_hpos[display]) || (outputregion_pre[display].srcy != apply_disp_vpos[display])
		|| (outputregion_pre[display].src_height != apply_disp_vsize[display]) ||(outputregion_pre[display].src_wid != apply_disp_hsize[display])
#ifdef CONFIG_PST_ENABLE
		|| (Scaler_main_imd_pst_get_enable()==TRUE)
#endif
		){
#if 0
        if((vbe_disp_oled_orbit_mode ==_VBE_PANEL_ORBIT_AUTO_MODE|| vbe_disp_oled_orbit_mode ==_VBE_PANEL_ORBIT_STORE_MODE) && vbe_disp_oled_orbit_enable &&  !Scaler_get_data_framesync(display))
        {//only for FRC mode
            vbe_disp_orbit_position_update(0xff);
            rtd_pr_smt_emerg("ARC change\n");
        }
#endif
		//rtd_pr_smt_notice("run smooth toggle ,not the same reigon\n");
		if(
#ifdef CONFIG_DUAL_CHANNEL
          (display == SLR_SUB_DISPLAY || display == SLR_MAIN_DISPLAY) &&
#else
          (display == SLR_MAIN_DISPLAY) &&
#endif
            ((outputregion_pre[display].srcx != apply_disp_hpos[display]) || (outputregion_pre[display].srcy != apply_disp_vpos[display] && !drv_memory_get_low_delay_game_mode_dynamic()))
            && (outputregion_pre[display].src_height == apply_disp_vsize[display]) && (outputregion_pre[display].src_wid == apply_disp_hsize[display])
            && ( ((Scaler_InputRegionType_Get(display) == INPUT_BASE_ON_HAL_ORIGINAL_INPUT) && (inputTiming_pre[display].src_wid * apply_crop_hpos[display] == apply_timing_hsize[display] * inputregion_pre[display].srcx)
			&& (inputTiming_pre[display].src_height * apply_crop_vpos[display] == apply_timing_vsize[display] * inputregion_pre[display].srcy)
			&& (inputTiming_pre[display].src_height * apply_crop_vsize[display] == apply_timing_vsize[display] * inputregion_pre[display].src_height)
			&& (inputTiming_pre[display].src_wid * apply_crop_hsize[display] == apply_timing_hsize[display] * inputregion_pre[display].src_wid))
		||	((Scaler_InputRegionType_Get(display) != INPUT_BASE_ON_HAL_ORIGINAL_INPUT) && (apply_crop_hpos[display] == inputregion_pre[display].srcx)
			&& (apply_crop_vpos[display] == inputregion_pre[display].srcy)
			&& (apply_crop_vsize[display] == inputregion_pre[display].src_height)
			&& (apply_crop_hsize[display] == inputregion_pre[display].src_wid))
			)) {
            outputregion_pre[display].srcx = apply_disp_hpos[display];
			outputregion_pre[display].srcy = apply_disp_vpos[display];
			outputregion_pre[display].src_height = apply_disp_vsize[display];
			outputregion_pre[display].src_wid = apply_disp_hsize[display];
            zoom_smoothtoggle_display_only_shift_xy(display);
            rtd_pr_smt_emerg("go display only shift xy\n");
            rtd_pr_smt_emerg("[%s %d]display:%d, input_region:(%d,%d,%d,%d)->(%d,%d,%d,%d) \n",__FUNCTION__, __LINE__, display,inputregion_pre[display].srcx,inputregion_pre[display].srcy,inputregion_pre[display].src_wid,inputregion_pre[display].src_height,
    			apply_crop_hpos[display],apply_crop_vpos[display],apply_crop_hsize[display],apply_crop_vsize[display]);
    		rtd_pr_smt_emerg("[%s %d]display:%d, output_region:(%d,%d,%d,%d)->(%d,%d,%d,%d) \n",__FUNCTION__, __LINE__,display,outputregion_pre[display].srcx,outputregion_pre[display].srcy,outputregion_pre[display].src_wid,outputregion_pre[display].src_height,
    			apply_disp_hpos[display],apply_disp_vpos[display],apply_disp_hsize[display],apply_disp_vsize[display]);
            check_only_xy_shift_change_to_videofw[display] = 0;

            set_input_for_crop(display, input_size);
			set_input_timing_for_crop(display, output_size);
			smooth_toggle_state_info_rpc(display);
            return ZOOM_STATE_OK;
        } else {
    		rtd_pr_smt_emerg("[%s %d]display:%d, input_region:(%d,%d,%d,%d)->(%d,%d,%d,%d) \n",__FUNCTION__, __LINE__,display,inputregion_pre[display].srcx,inputregion_pre[display].srcy,inputregion_pre[display].src_wid,inputregion_pre[display].src_height,
    			apply_crop_hpos[display],apply_crop_vpos[display],apply_crop_hsize[display],apply_crop_vsize[display]);
    		rtd_pr_smt_emerg("[%s %d]display:%d, output_region:(%d,%d,%d,%d)->(%d,%d,%d,%d) \n",__FUNCTION__, __LINE__,display,outputregion_pre[display].srcx,outputregion_pre[display].srcy,outputregion_pre[display].src_wid,outputregion_pre[display].src_height,
    			apply_disp_hpos[display],apply_disp_vpos[display],apply_disp_hsize[display],apply_disp_vsize[display]);
    		inputregion_pre[display].srcx = apply_crop_hpos[display];
    		inputregion_pre[display].srcy = apply_crop_vpos[display];
    		inputregion_pre[display].src_height = apply_crop_vsize[display];
    		inputregion_pre[display].src_wid = apply_crop_hsize[display];
    		outputregion_pre[display].srcx = apply_disp_hpos[display];
    		outputregion_pre[display].srcy = apply_disp_vpos[display];
    		outputregion_pre[display].src_height = apply_disp_vsize[display];
    		outputregion_pre[display].src_wid = apply_disp_hsize[display];
    		check_only_xy_shift_change_to_videofw[display] = 0;
    		return ZOOM_STATE_FAIL;
        }
	}
	else if ((inputregion_pre[display].srcx != apply_crop_hpos[display]) || (inputregion_pre[display].srcy != apply_crop_vpos[display]))
	{
	#if 1 //def CONFIG_ALL_SOURCE_DATA_FS
		if (Scaler_get_data_framesync(display)) {
			inputregion_pre[display].srcx = apply_crop_hpos[display];
			inputregion_pre[display].srcy = apply_crop_vpos[display];
			inputregion_pre[display].src_height = apply_crop_vsize[display];
			inputregion_pre[display].src_wid = apply_crop_hsize[display];
			outputregion_pre[display].srcx = apply_disp_hpos[display];
			outputregion_pre[display].srcy = apply_disp_vpos[display];
			outputregion_pre[display].src_height = apply_disp_vsize[display];
			outputregion_pre[display].src_wid = apply_disp_hsize[display];
			if (Get_DisplayMode_Src(display) == VSC_INPUTSRC_VDEC || Get_DisplayMode_Src(display) == VSC_INPUTSRC_CAMERA) {
				check_only_xy_shift_change_to_videofw[display] = 1;
				rtd_pr_smt_emerg("[SMT]vdec source only shift xy case--vo do overscan\n");
			} else {
				check_only_xy_shift_change_to_videofw[display] = 0;
				rtd_pr_smt_emerg("[SMT]non-vdec only shift xy case--vo do overscan\n");
			}
			return ZOOM_STATE_FAIL;
		}
	#endif
		check_only_xy_shift_change_to_videofw[display] = 0;
		rtd_pr_smt_emerg("[SMT]smooth toggle only shift xy case\n");
		set_shiftxy_main_inputpre(get_main_input_size());
		set_shiftxy_main_dispwinpre(get_main_dispwin());

#if 0	//KTASKWBS-5348  move update info after execution
		//update pre info
		inputregion_pre[display].srcx = apply_crop_hpos[display];
		inputregion_pre[display].srcy = apply_crop_vpos[display];
		inputregion_pre[display].src_height = apply_crop_vsize[display];
		inputregion_pre[display].src_wid = apply_crop_hsize[display];
		outputregion_pre[display].srcx = apply_disp_hpos[display];
		outputregion_pre[display].srcy = apply_disp_vpos[display];
		outputregion_pre[display].src_height = apply_disp_vsize[display];
		outputregion_pre[display].src_wid = apply_disp_hsize[display];
#endif
		if(Get_DisplayMode_Src(display) == VSC_INPUTSRC_VDEC || Get_DisplayMode_Src(display) == VSC_INPUTSRC_CAMERA)
			send_vo_freeze_shiftxy_share_memory();
		//if(Get_DisplayMode_Src(display) == VSC_INPUTSRC_AVD){
		//	return ZOOM_STATE_FAIL;//atv always run smooth toggle flow
		//}else
		{
			zoom_smoothtoggle_only_shift_xy(display);

#if 1
			//update pre info
			inputregion_pre[display].srcx = apply_crop_hpos[display];
			inputregion_pre[display].srcy = apply_crop_vpos[display];
			inputregion_pre[display].src_height = apply_crop_vsize[display];
			inputregion_pre[display].src_wid = apply_crop_hsize[display];
			outputregion_pre[display].srcx = apply_disp_hpos[display];
			outputregion_pre[display].srcy = apply_disp_vpos[display];
			outputregion_pre[display].src_height = apply_disp_vsize[display];
			outputregion_pre[display].src_wid = apply_disp_hsize[display];
#endif


			return ZOOM_STATE_OK;
		}

    }
    else{
		rtd_pr_smt_err("[SMT]the same SDNR region, don't run smooth toggle\n");
#if 0
		rtd_pr_smt_emerg("[SMT] srcx pre=%d, new=%d \n", inputregion_pre[display].srcx, apply_crop_hpos[display]);
		rtd_pr_smt_emerg("[SMT] srcy pre=%d, new=%d \n", inputregion_pre[display].srcy, apply_crop_vpos[display]);
		rtd_pr_smt_emerg("[SMT] srch pre=%d, new=%d \n", inputregion_pre[display].src_height, apply_crop_vsize[display]);
		rtd_pr_smt_emerg("[SMT] srcv pre=%d, new=%d \n", inputregion_pre[display].src_wid, apply_crop_hsize[display]);

		rtd_pr_smt_emerg("[SMT] dstx pre=%d, new=%d \n", outputregion_pre[display].srcx, apply_disp_hpos[display]);
		rtd_pr_smt_emerg("[SMT] dsty pre=%d, new=%d \n", outputregion_pre[display].srcy, apply_disp_vpos[display]);
		rtd_pr_smt_emerg("[SMT] dsth pre=%d, new=%d \n", outputregion_pre[display].src_height, apply_disp_vsize[display]);
		rtd_pr_smt_emerg("[SMT] dstv pre=%d, new=%d \n", outputregion_pre[display].src_wid, apply_disp_hsize[display]);
#endif
		check_only_xy_shift_change_to_videofw[display] = 0;
		return ZOOM_STATE_OK;
	}
}

static void shiftxy_decide_di_hsd(unsigned char display)
{
	unsigned int uDI_WID=0x00;

	/*In Mac2, use hsd in path to reduce bandwidth.
	 * But hardware has line buffer limit,
	 * so SLR_INPUT_DI_WID value depends on the conditions.
	 */
	vgip_data_path_select_RBUS data_path_select_reg;
	data_path_select_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_reg);

	Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_STA,Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_STA_PRE));
	Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID,Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE));
	Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_STA, Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE));
	Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN, Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE));

	uDI_WID = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID);
	di_hsd_flag = FALSE;

	if (data_path_select_reg.uzd1_in_sel && (display ==SLR_MAIN_DISPLAY))	{
		if ((VD_27M_HSD960_DI5A == fw_video_get_27mhz_mode(display))&&(Get_DisplayMode_Src(display) == VSC_INPUTSRC_AVD)&&
			(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID) == AVD_27M_VGIP_WIDTH))	{
			uDI_WID = AVD_DI_5A_WIDTH;
			di_hsd_flag = TRUE;
		}
		else if (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID) > REDUCE_DI_HSD_WIDTH_LIMIT_THRESHOLD)
		{
			uDI_WID = REDUCE_DI_HSD_WIDTH_LIMIT_THRESHOLD;
			di_hsd_flag = TRUE;
		}
	}
	Scaler_DispSetInputInfoByDisp(display, SLR_INPUT_DI_WID, uDI_WID);

	if(display ==SLR_SUB_DISPLAY){
		Scaler_DispSetInputInfoByDisp(display,SLR_INPUT_DI_WID, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID));

		Scaler_DispSetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_LEN, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_IPV_ACT_LEN));

	}else if ((Scaler_DispGetStatus(display, SLR_DISP_INTERLACE))&&(display==SLR_MAIN_DISPLAY)) {
		if (Scaler_DispGetStatus(display, SLR_DISP_INTERLACE)) {// if timing change, SLR_DISP_THRIP isn't updated
				Scaler_DispSetInputInfoByDisp(display,SLR_INPUT_DI_LEN, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN)<<1);
		} else {
			Scaler_DispSetInputInfoByDisp(display,SLR_INPUT_DI_LEN, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN));
		}
	}
	else
		Scaler_DispSetInputInfoByDisp(display,SLR_INPUT_DI_LEN, Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_LEN));

#if 1
	rtd_pr_smt_emerg("[sihftxy]di_hsd_flag=%d, display=%d\n", di_hsd_flag, display);
	rtd_pr_smt_emerg("[sihftxy] SLR_INPUT_DI_WID=%d\n", Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_WID));
	rtd_pr_smt_emerg("[sihftxy] SLR_INPUT_DI_LEN=%d\n", Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_LEN));
#endif
}

StructSrcRect shiftxy_cal_new_inputRegion_base_on_hal_orgInput(unsigned char display,StructSrcRect inputRegion,StructSrcRect inputTiming)
{
	StructSrcRect NewInput2OldInput = inputRegion;
	//rtd_pr_smt_debug("[pool test]input info:x:%d,y:%d,wid:%d,len:%d\n",m_input_size.srcx,m_input_size.srcy,m_input_size.src_wid,m_input_size.src_height);
	shiftxy_decide_di_hsd(display);

	//check 4k2k input and output ,overflow
	if(inputTiming.src_wid && inputTiming.src_height){
		NewInput2OldInput.srcx = (Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_WID) * inputRegion.srcx *100/inputTiming.src_wid + 50)/100;
		NewInput2OldInput.srcy = (Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_LEN) * inputRegion.srcy *100/inputTiming.src_height + 50)/100;
		NewInput2OldInput.src_wid = (Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_WID) * inputRegion.src_wid*100/inputTiming.src_wid + 50)/100;
		NewInput2OldInput.src_height = (Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_LEN) *inputRegion.src_height * 100/inputTiming.src_height + 50)/100;
	}

	if((NewInput2OldInput.srcx + NewInput2OldInput.src_wid) > Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_WID)){
		rtd_pr_smt_err("[shiftxy][WARNING]new adaptive stream, wid is over size\n");
		NewInput2OldInput.srcx = Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_WID) - NewInput2OldInput.src_wid;
	}

	if((NewInput2OldInput.srcy + NewInput2OldInput.src_height) > Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_LEN)){
		rtd_pr_smt_err("[shiftxy][WARNING]new adaptive stream, len is over size\n");
		NewInput2OldInput.srcy = Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DI_LEN)  - NewInput2OldInput.src_height;
	}

	if(NewInput2OldInput.srcx + NewInput2OldInput.src_wid > REDUCE_SDNR_WIDTH_LIMIT_THRESHOLD){
		scaledown_ich1_sdnr_cutout_range_hor_RBUS ich1_sdnr_cutout_range_hor_reg;
		ich1_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(SCALEDOWN_ich1_sdnr_cutout_range_hor_reg);
		NewInput2OldInput.srcx = REDUCE_SDNR_WIDTH_LIMIT_THRESHOLD -( ich1_sdnr_cutout_range_hor_reg.hor_width);
	}
	return NewInput2OldInput;
}

StructSrcRect drvif_cal_SDNR_size(unsigned char display, StructSrcRect input, StructSrcRect inputTiming,StructSrcRect output)
{
	StructSrcRect new_SDNR_size = input;
//	unsigned int uAfterDILenPre = 0;
	unsigned int den_width = 0;

	if(Scaler_InputRegionType_Get(display) == INPUT_BASE_ON_DISP_WIN){
		new_SDNR_size = cal_new_input_base_on_display_win(display);
		input = new_SDNR_size;
	}else if(Scaler_InputRegionType_Get(display) == INPUT_BASE_ON_HAL_ORIGINAL_INPUT){
		new_SDNR_size = shiftxy_cal_new_inputRegion_base_on_hal_orgInput(display,input,inputTiming);
		return new_SDNR_size;
	}else{
		new_SDNR_size = input;
	}

	if (Scaler_Get_di_hsd_flag())	{
		if ((VD_27M_HSD960_DI5A == fw_video_get_27mhz_mode(display))&&(Get_DisplayMode_Src(display) == VSC_INPUTSRC_AVD)&&
			(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE == AVD_27M_VGIP_WIDTH))	{
			new_SDNR_size.src_wid = input.src_wid;
			new_SDNR_size.srcx = input.srcx;
			new_SDNR_size.src_height = input.src_height;
			new_SDNR_size.srcy = input.srcy;
		}
		else	{
			den_width = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_WID);
			if(den_width > 0){
				new_SDNR_size.src_wid = (input.src_wid * REDUCE_SDNR_WIDTH_LIMIT_THRESHOLD * 100/den_width + 55)/100;
				new_SDNR_size.srcx = (input.srcx * REDUCE_SDNR_WIDTH_LIMIT_THRESHOLD * 100 / den_width + 55) / 100;
				new_SDNR_size.src_height = input.src_height;
				new_SDNR_size.srcy = input.srcy;
			}
			else{
				rtd_pr_smt_notice("input wid size error(%d)\n",den_width);
			}
		}
	}
	else	{
		new_SDNR_size.src_wid = input.src_wid;
		new_SDNR_size.srcx = input.srcx;
		new_SDNR_size.src_height = input.src_height;
		new_SDNR_size.srcy = input.srcy;
	}

	if (((new_SDNR_size.srcx + new_SDNR_size.src_wid)>REDUCE_SDNR_WIDTH_LIMIT_THRESHOLD)  && (new_SDNR_size.srcx >2) ){
		new_SDNR_size.srcx = new_SDNR_size.srcx - 2;
	}

	if ((new_SDNR_size.src_wid)>Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_WID)){
		new_SDNR_size.src_wid = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_WID);
		rtd_pr_smt_notice("sdnr wid size over di wid(%d)\n",Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_WID));
	}
	if ((new_SDNR_size.srcx + new_SDNR_size.src_wid)>Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_WID)){
		new_SDNR_size.srcx = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_WID) - new_SDNR_size.src_wid;
		rtd_pr_smt_notice("sdnr x+wid size over di wid(%d)\n",Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_WID));
	}
	if ((new_SDNR_size.src_height)>Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_LEN)){
		new_SDNR_size.src_height = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_LEN);
		rtd_pr_smt_notice("sdnr height size over di len(%d)\n",Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_LEN));
	}
	if ((new_SDNR_size.srcy + new_SDNR_size.src_height)>Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_LEN)){
		new_SDNR_size.srcy = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_LEN) - new_SDNR_size.src_height;
		rtd_pr_smt_notice("sdnr y+height size over di len(%d)\n",Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_DI_LEN));
	}

	return new_SDNR_size;
}

void zoom_smoothtoggle_only_shift_xy(unsigned char display)
{
//for SDNR overscan
	int timeout = 30;
	scaledown_ich1_uzd_db_ctrl_RBUS ich1_uzd_db_ctrl_reg;
	scaledown_ich1_sdnr_cutout_range_hor_RBUS ich1_sdnr_cutout_range_hor_reg;
	scaledown_ich1_sdnr_cutout_range_ver0_RBUS ich1_sdnr_cutout_range_ver0_reg;
	//scaledown_ich1_sdnr_cutout_range_ver1_RBUS ich1_sdnr_cutout_range_ver1_reg;
	scaledown_ich1_uzd_ctrl0_RBUS 	ich1_uzd_Ctrl0_REG;
	scaledown_ich2_uzd_ctrl0_RBUS scaledown_ich2_uzd_ctrl0_reg;
	StructSrcRect SDNR_size;
	//int mid = 0;
	StructSrcRect input,output,inputTiming;
	if(Scaler_InputRegionType_Get(display) == INPUT_BASE_ON_HAL_ORIGINAL_INPUT){
		#if 0
		rtd_pr_smt_emerg("[shiftxy]apply crop:%d, %d, %d ,%d \n",apply_crop_hpos[display],apply_crop_vpos[display],apply_crop_hsize[display],apply_crop_vsize[display]);
		rtd_pr_smt_emerg("[shiftxy]apply timing:%d, %d, %d ,%d \n",apply_timing_hpos[display],apply_timing_vpos[display],apply_timing_hsize[display],apply_timing_vsize[display]);
		rtd_pr_smt_emerg("[shiftxy]apply disp:%d, %d, %d ,%d \n",apply_disp_hpos[display],apply_disp_vpos[display],apply_disp_hsize[display],apply_disp_vsize[display]);
		#endif
		input.srcx = apply_crop_hpos[display];
		input.srcy = apply_crop_vpos[display];
		input.src_wid = apply_crop_hsize[display];
		input.src_height = apply_crop_vsize[display];
		inputTiming.srcx = apply_timing_hpos[display];
		inputTiming.srcy = apply_timing_vpos[display];
		inputTiming.src_wid = apply_timing_hsize[display];
		inputTiming.src_height = apply_timing_vsize[display];
	}else{
		input.srcx = apply_crop_hpos[display];
		input.srcy = apply_crop_vpos[display];
		input.src_wid = apply_crop_hsize[display];
		input.src_height = apply_crop_vsize[display];
		inputTiming.srcx = apply_timing_hpos[display];
		inputTiming.srcy = apply_timing_vpos[display];
		inputTiming.src_wid = apply_timing_hsize[display];
		inputTiming.src_height = apply_timing_vsize[display];
	}
	output.srcx = apply_disp_hpos[display];
	output.srcy = apply_disp_vpos[display];
	output.src_wid = apply_disp_hsize[display];
	output.src_height = apply_disp_vsize[display];
	SDNR_size = input;
	//zoom_record_input_output_pre_size(display,main_input_size,main_dispwin);

	//if color space is 422 format, it should be even number. @ Crixus
	if( imdSmoothInfo[display].IMD_SMOOTH_DISP_422){
		if((input.srcx % 2) != (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE %2))
			input.srcx  -= 1;//This means that shift value need even
	}

	SDNR_size = drvif_cal_SDNR_size(display,input,inputTiming,output);
	rtd_pr_smt_emerg("[shiftxy][SDNR]x:%d,y:%d,wid:%d,len:%d\n",SDNR_size.srcx,SDNR_size.srcy,SDNR_size.src_wid,SDNR_size.src_height);

	ich1_uzd_db_ctrl_reg.regValue =IoReg_Read32(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
	//wait smooth toggle flow
	//wait_vo_share_memory_finish();//This is for wait sharememory already read
	Check_smooth_toggle_update_flag(display);

	if(SLR_MAIN_DISPLAY == display){
 		{
			//uzu double buffer enbale
			ich1_uzd_db_ctrl_reg.regValue =IoReg_Read32(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
			ich1_uzd_db_ctrl_reg.h_db_en=1;
			ich1_uzd_db_ctrl_reg.h_db_apply=0;
			ich1_uzd_db_ctrl_reg.v_db_en=1;
			ich1_uzd_db_ctrl_reg.v_db_apply=0;
			IoReg_Write32(SCALEDOWN_ICH1_UZD_DB_CTRL_reg,ich1_uzd_db_ctrl_reg.regValue);
		}
		ich1_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(SCALEDOWN_ich1_sdnr_cutout_range_hor_reg);
		ich1_sdnr_cutout_range_hor_reg.hor_front = SDNR_size.srcx;
		ich1_sdnr_cutout_range_hor_reg.hor_width = SDNR_size.src_wid; // chip setting need to +1
		IoReg_Write32(SCALEDOWN_ich1_sdnr_cutout_range_hor_reg, ich1_sdnr_cutout_range_hor_reg.regValue);

		ich1_sdnr_cutout_range_ver0_reg.regValue = IoReg_Read32(SCALEDOWN_ich1_sdnr_cutout_range_ver0_reg);
		ich1_sdnr_cutout_range_ver0_reg.ver_front = SDNR_size.srcy;
		ich1_sdnr_cutout_range_ver0_reg.ver_height = SDNR_size.src_height;// chip setting need to +1
		IoReg_Write32(SCALEDOWN_ich1_sdnr_cutout_range_ver0_reg, ich1_sdnr_cutout_range_ver0_reg.regValue);

		//mid = ich1_sdnr_cutout_range_ver0_reg.ver_back;
		//ich1_sdnr_cutout_range_ver1_reg.regValue = IoReg_Read32(SCALEDOWN_ich1_sdnr_cutout_range_ver1_reg);
		//ich1_sdnr_cutout_range_ver1_reg.ver_mid_top = mid;
		//ich1_sdnr_cutout_range_ver1_reg.ver_mid_bot = mid - 1;
		//IoReg_Write32(SCALEDOWN_ich1_sdnr_cutout_range_ver1_reg, ich1_sdnr_cutout_range_ver1_reg.regValue);

		ich1_uzd_Ctrl0_REG.regValue = IoReg_Read32(SCALEDOWN_ICH1_UZD_Ctrl0_reg);
		ich1_uzd_Ctrl0_REG.cutout_en = 1;
		IoReg_Write32(SCALEDOWN_ICH1_UZD_Ctrl0_reg, ich1_uzd_Ctrl0_REG.regValue);
 		{
			//uzu double buffer apply
			ich1_uzd_db_ctrl_reg.regValue =IoReg_Read32(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
			ich1_uzd_db_ctrl_reg.h_db_apply=1;
			ich1_uzd_db_ctrl_reg.v_db_apply=1;
			IoReg_Write32(SCALEDOWN_ICH1_UZD_DB_CTRL_reg,ich1_uzd_db_ctrl_reg.regValue);

			while(ich1_uzd_db_ctrl_reg.h_db_apply || ich1_uzd_db_ctrl_reg.v_db_apply)
			{
				timeout --;
				mdelay(1); /*wait 1ms 30 counts for vsync */
				if(!timeout) break;
				ich1_uzd_db_ctrl_reg.regValue = IoReg_Read32(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
			}
		}
		if(timeout == 0) rtd_pr_smt_debug("[shiftxy] uzd main aplly time out !!! :%d\n",timeout);

	}else{
		scaledown_ich2_sdnr_cutout_range_hor_RBUS ich2_sdnr_cutout_range_hor_reg;
		scaledown_ich2_sdnr_cutout_range_ver0_RBUS ich2_sdnr_cutout_range_ver0_reg;
		//scaledown_ich2_sdnr_cutout_range_ver1_RBUS ich1_sdnr_cutout_range_ver1_reg;
		scaledown_ich2_uzd_db_ctrl_RBUS ich2_uzd_db_ctrl_reg;
 		{
			//UZD
			ich2_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg);
			ich2_uzd_db_ctrl_reg.h_db_en=1;
			ich2_uzd_db_ctrl_reg.h_db_apply=0;
			ich2_uzd_db_ctrl_reg.v_db_en=1;
			ich2_uzd_db_ctrl_reg.v_db_apply=0;
			rtd_outl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg,ich2_uzd_db_ctrl_reg.regValue);
		}

		ich2_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(SCALEDOWN_ICH2_sdnr_cutout_range_hor_reg);
		ich2_sdnr_cutout_range_hor_reg.hor_front= SDNR_size.srcx;
		ich2_sdnr_cutout_range_hor_reg.hor_width= SDNR_size.src_wid; // chip setting need to +1
		IoReg_Write32(SCALEDOWN_ICH2_sdnr_cutout_range_hor_reg, ich2_sdnr_cutout_range_hor_reg.regValue);

		ich2_sdnr_cutout_range_ver0_reg.regValue = IoReg_Read32(SCALEDOWN_ICH2_sdnr_cutout_range_ver0_reg);
		ich2_sdnr_cutout_range_ver0_reg.ver_front= SDNR_size.srcy;
		ich2_sdnr_cutout_range_ver0_reg.ver_height= SDNR_size.src_height;// chip setting need to +1
		IoReg_Write32(SCALEDOWN_ICH2_sdnr_cutout_range_ver0_reg, ich2_sdnr_cutout_range_ver0_reg.regValue);

		//ich1_sdnr_cutout_range_ver1_reg.regValue = IoReg_Read32(SCALEDOWN_ich2_sdnr_cutout_range_ver1_reg);
		//ich1_sdnr_cutout_range_ver1_reg.ver_mid_top=0;	// inregion.y;
		//ich1_sdnr_cutout_range_ver1_reg.ver_mid_bot=0;	// inregion.y;
		//IoReg_Write32(SCALEDOWN_ich2_sdnr_cutout_range_ver1_reg, ich1_sdnr_cutout_range_ver1_reg.regValue);

		scaledown_ich2_uzd_ctrl0_reg.regValue = IoReg_Read32(SCALEDOWN_ICH2_UZD_Ctrl0_reg);
		scaledown_ich2_uzd_ctrl0_reg.cutout_en = 1;
		IoReg_Write32(SCALEDOWN_ICH2_UZD_Ctrl0_reg, scaledown_ich2_uzd_ctrl0_reg.regValue);

 		{
			//uzu double buffer apply
			ich2_uzd_db_ctrl_reg.regValue =IoReg_Read32(SCALEDOWN_ICH2_UZD_DB_CTRL_reg);
			ich2_uzd_db_ctrl_reg.h_db_apply=1;
			ich2_uzd_db_ctrl_reg.v_db_apply=1;
			IoReg_Write32(SCALEDOWN_ICH2_UZD_DB_CTRL_reg,ich2_uzd_db_ctrl_reg.regValue);

			while(ich2_uzd_db_ctrl_reg.h_db_apply || ich2_uzd_db_ctrl_reg.v_db_apply)
			{
				timeout --;
				mdelay(1); /*wait 1ms 30 counts for vsync */
				if(!timeout) break;
				ich2_uzd_db_ctrl_reg.regValue = IoReg_Read32(SCALEDOWN_ICH2_UZD_DB_CTRL_reg);
			}
		}
		if(timeout == 0) rtd_pr_smt_debug("[shiftxy] uzd sub aplly time out !!! :%d\n",timeout);
	}
}

void zoom_smoothtoggle_display_only_shift_xy(unsigned char display)
{
	int timeout = 30;

	if (display == SLR_MAIN_DISPLAY) {
		ppoverlay_main_active_h_start_end_RBUS main_active_h_start_end_reg;
		ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_reg;
		ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;

		unsigned int DispHSta = Get_DISP_ACT_STA_BIOS()+Get_DISP_ACT_STA_HPOS() + apply_disp_hpos[display];
		unsigned int DispHEnd = DispHSta;
		unsigned int DispVSta =Get_DISP_ACT_STA_VPOS()+ apply_disp_vpos[display];
		unsigned int DispVEnd =DispVSta;

		Check_smooth_toggle_update_flag(display);

		double_buffer_ctrl_reg.regValue =IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		//D0
		double_buffer_ctrl_reg.dmainreg_dbuf_en=1;
		double_buffer_ctrl_reg.dmainreg_dbuf_set=0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);
		// Main Active H pos
		main_active_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_H_Start_End_reg);
		DispHEnd +=main_active_h_start_end_reg.mh_act_end-main_active_h_start_end_reg.mh_act_sta;
		main_active_h_start_end_reg.mh_act_sta = DispHSta & 0x1fff;
		main_active_h_start_end_reg.mh_act_end = DispHEnd & 0x1fff;
		IoReg_Write32(PPOVERLAY_MAIN_Active_H_Start_End_reg, main_active_h_start_end_reg.regValue);

		// Main Active V pos
		main_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_V_Start_End_reg);
		DispVEnd += main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta;
		main_active_v_start_end_reg.mv_act_sta = DispVSta & 0x1fff;
		main_active_v_start_end_reg.mv_act_end = DispVEnd & 0x1fff;
		IoReg_Write32(PPOVERLAY_MAIN_Active_V_Start_End_reg, main_active_v_start_end_reg.regValue);

		//do double buffer apply
		double_buffer_ctrl_reg.regValue =IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		//D0
		double_buffer_ctrl_reg.dmainreg_dbuf_en=1;
		double_buffer_ctrl_reg.dmainreg_dbuf_set=1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);

		while(double_buffer_ctrl_reg.dmainreg_dbuf_set)
		{
			timeout --;
			mdelay(1); /*wait 1ms 30 counts for vsync */
			if(!timeout) break;
			double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		}
		if(timeout == 0) rtd_pr_smt_debug("[display-shiftxy]  main display apply time out !!!\n");
	}
#ifdef CONFIG_DUAL_CHANNEL
    else
    {
		ppoverlay_sub_active_h_start_end_RBUS sub_active_h_start_end_reg;
		ppoverlay_sub_active_v_start_end_RBUS sub_active_v_start_end_reg;
		ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;
        ppoverlay_sub_den_h_start_width_RBUS ppoverlay_sub_den_h_start_width_Reg;
    	ppoverlay_sub_den_v_start_length_RBUS ppoverlay_sub_den_v_start_length_Reg;
        ppoverlay_sub_background_h_start_end_RBUS ppoverlay_sub_background_h_start_end_Reg;
	    ppoverlay_sub_background_v_start_end_RBUS ppoverlay_sub_background_v_start_end_Reg;
        mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;

        mdom_sub_disp_disp1_pre_rd_ctrl_RBUS mdom_sub_disp_disp1_pre_rd_ctrl_reg;

        StructSrcRect subWindow = Scaler_SubDispWindowGet();

		unsigned int DispHSta = subWindow.srcx;
		unsigned int DispHEnd = subWindow.src_wid + subWindow.srcx;
		unsigned int DispVSta = subWindow.srcy;
		unsigned int DispVEnd = subWindow.src_height + subWindow.srcy;

        rtd_pr_smt_debug("[display-shiftxy]  only Display XY change !!!\n");

		Check_smooth_toggle_update_flag(display);

    	mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg);
		//mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = (Get_DISP_DEN_STA_VPOS() - 3 + DispVSta);
		mdom_sub_disp_disp1_pre_rd_ctrl_reg.disp1_pre_rd_v_start = SUB_MDOM_PREREAD_START;//KTASKWBS-21489 k8 change to 7
    	IoReg_Write32(MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg, mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue);

		mdom_sub_disp_disp1_db_ctrl_reg.regValue =IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
		mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en=1;
		mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply=1;
		IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

		ppoverlay_double_buffer_ctrl4_Reg.regValue =IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
		//D0
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en=1;
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set=0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_Reg.regValue);

        ppoverlay_sub_background_h_start_end_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_H_Start_End_reg);
        ppoverlay_sub_background_v_start_end_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Background_V_Start_End_reg);

        ppoverlay_sub_background_h_start_end_Reg.sh_bg_sta = 0;
	    ppoverlay_sub_background_h_start_end_Reg.sh_bg_end = (DispHEnd - DispHSta);

	    ppoverlay_sub_background_v_start_end_Reg.sv_bg_sta = 0;
	    ppoverlay_sub_background_v_start_end_Reg.sv_bg_end = (DispVEnd - DispVSta);

        IoReg_Write32(PPOVERLAY_SUB_Background_H_Start_End_reg, ppoverlay_sub_background_h_start_end_Reg.regValue);
	    IoReg_Write32(PPOVERLAY_SUB_Background_V_Start_End_reg, ppoverlay_sub_background_v_start_end_Reg.regValue);

    	ppoverlay_sub_den_h_start_width_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_H_Start_Width_reg);
    	ppoverlay_sub_den_v_start_length_Reg.regValue = IoReg_Read32(PPOVERLAY_SUB_DEN_V_Start_Length_reg);

    	ppoverlay_sub_den_h_start_width_Reg.sh_den_sta = DispHSta + Get_DISP_ACT_STA_BIOS();
    	ppoverlay_sub_den_h_start_width_Reg.sh_den_width = DispHEnd - DispHSta;

    	ppoverlay_sub_den_v_start_length_Reg.sv_den_sta = DispVSta;
    	ppoverlay_sub_den_v_start_length_Reg.sv_den_length = DispVEnd - DispVSta;


        IoReg_Write32(PPOVERLAY_SUB_DEN_H_Start_Width_reg, ppoverlay_sub_den_h_start_width_Reg.regValue);
        IoReg_Write32(PPOVERLAY_SUB_DEN_V_Start_Length_reg, ppoverlay_sub_den_v_start_length_Reg.regValue);
		if(vbe_disp_oled_orbit_enable)
		{
			extern void ScalerForceUpdateOrbit(unsigned char force_update);
			ScalerForceUpdateOrbit(1);
		}

        // Main Active H pos
		sub_active_h_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_H_Start_End_reg);
		//DispHEnd +=main_active_h_start_end_reg.mh_act_end-main_active_h_start_end_reg.mh_act_sta;
		sub_active_h_start_end_reg.sh_act_sta = 0;
		sub_active_h_start_end_reg.sh_act_end = subWindow.src_wid;
		IoReg_Write32(PPOVERLAY_SUB_Active_H_Start_End_reg, sub_active_h_start_end_reg.regValue);

		// Main Active V pos
		sub_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_SUB_Active_V_Start_End_reg);
		//DispVEnd += main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta;
		sub_active_v_start_end_reg.sv_act_sta = 0;
		sub_active_v_start_end_reg.sv_act_end = subWindow.src_height;
		IoReg_Write32(PPOVERLAY_SUB_Active_V_Start_End_reg, sub_active_v_start_end_reg.regValue);

		//do double buffer apply
		ppoverlay_double_buffer_ctrl4_Reg.regValue =IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
		//D0
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en=1;
		ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set=1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_Reg.regValue);

		while(ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set)
		{
			timeout --;
			mdelay(1); /*wait 1ms 30 counts for vsync */
			if(!timeout) break;
			ppoverlay_double_buffer_ctrl4_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
		}
		if(timeout == 0) rtd_pr_smt_debug("[display-shiftxy]  sub display apply time out !!!\n");
	}
#endif
}

unsigned char zoom_check_same_region(unsigned char display)
{
	if ((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_HSTA) != 0)
		|| (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_VSTA) != 0)
		|| (!get_hdmi_vrr_4k60_mode() && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN) != Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN)))
		|| (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_WID) != Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID))) {
		rtd_pr_smt_notice("SLR_INPUT_MEM_ACT_HSTA:%d;SLR_INPUT_MEM_ACT_VSTA:%d\n",Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_HSTA), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_VSTA));
		rtd_pr_smt_notice("SLR_INPUT_MEM_ACT_LEN:%d;SLR_INPUT_MEM_ACT_WID:%d\n",Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_WID));
		rtd_pr_smt_notice("SLR_INPUT_CAP_LEN:%d;SLR_INPUT_CAP_WID:%d\n",Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID));

		Scaler_DispSetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_HSTA, 0);
		Scaler_DispSetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_VSTA, 0);
		Scaler_DispSetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN));
		Scaler_DispSetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_WID, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID));
		rtd_pr_smt_notice("######MAIN Smoothtoggle flowrun smooth toggle again. for livezoom do overscan in m domain######\r\n");
		return ZOOM_STATE_FAIL;
	}

	return zoom_check_same_region_for_SDNR(display);
}

int zoom_imd_smooth_toggle_proc(unsigned char rcflag, unsigned char rdflag, unsigned char display)
{
	unsigned char failflag = ZOOM_STATE_OK;

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
		if ((display == SLR_MAIN_DISPLAY) && (TRUE == Get_rotate_function(SLR_MAIN_DISPLAY)))
			return ZOOM_STATE_OK;
#endif

	if(Scaler_Get3DMode((SCALER_DISP_CHANNEL)display) != SLR_3DMODE_2D){ //skip 3d mode
		rtd_pr_smt_debug("_rk [3D] ByPass smooth toggle in 3D mode[%d]@%s\n", Scaler_Get3DMode((SCALER_DISP_CHANNEL)display), __FUNCTION__);
		return ZOOM_STATE_OK;
	}

	failflag = zoom_imd_smooth_set_info(display, rcflag, rdflag);

	if(failflag == ZOOM_STATE_FAIL){
		return failflag;//return fail directly
	}

	if(zoom_check_same_region(display) == ZOOM_STATE_OK ) {
		if (Scaler_InputSrcGetType(display) == _SRC_VO)
   			Scaler_SendDisplayReadyIrc(Scaler_Get_CurVoInfo_VoVideoPlane(display));
		if((IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg)&PPOVERLAY_Display_Timing_CTRL1_disp_en_mask) ==0) {
			rtd_pr_smt_info("power off,don't need to do smoothtoggle\n");
			return ZOOM_STATE_OK;
		}
		return ZOOM_STATE_OK;
	}

	if((display == SLR_MAIN_DISPLAY)&&(DbgSclrFlgTkr.mainVscFreezeFlag)){
		Scaler_SetFreeze(SLR_MAIN_DISPLAY, 0);
	}

	#ifdef CONFIG_ALL_SOURCE_DATA_FS
		if ((Scaler_DispGetInputInfoByDisp(display,SLR_INPUT_FRAMESYNC)) &&(0==Scaler_get_data_framesync(display))) {
			Scaler_memory_setting_through_CMA();
			send_memory_mapping_for_adaptive_streaming();
		}
	#endif
	failflag = zoom_imd_smooth_toggle_vo_flow(display);

	if((display == SLR_MAIN_DISPLAY)&&(DbgSclrFlgTkr.mainVscFreezeFlag)){
		Scaler_SetFreeze(SLR_MAIN_DISPLAY, 1);
	}

	return failflag;
}

void imd_smooth_main_double_buffer_all_apply(void)
{
	vgip_vgip_chn1_double_buffer_ctrl_RBUS vgip_chn1_double_buffer_reg;
	di_db_reg_ctl_RBUS  db_reg_ctl_reg;
	mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;
	mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	scaledown_ich1_uzd_db_ctrl_RBUS ich1_uzd_db_ctrl_reg;
	scaleup_dm_uzu_db_ctrl_RBUS dm_uzu_db_ctrl_reg;
	hsd_dither_di_uzd_db_ctrl_RBUS di_uzd_db_ctrl_reg;
	pll27x_reg_dpll_double_buffer_ctrl_RBUS dpll_double_buffer_ctrl_reg;
//	iedge_smooth_edsm_db_ctrl_RBUS edsm_db_ctrl_reg;
	rgb2yuv_ich1_422to444_ctrl_db_RBUS rgb2yuv_ich1_422to444_ctrl_db_reg;
	unsigned long flags;//for spin_lock_irqsave

	//vgip
	vgip_chn1_double_buffer_reg.regValue =rtd_inl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg);
	//vgip_chn1_double_buffer_reg.ch1_db_en=1;
	vgip_chn1_double_buffer_reg.ch1_db_rdy=1;
	rtd_outl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg,vgip_chn1_double_buffer_reg.regValue);

	//di
	db_reg_ctl_reg.regValue =rtd_inl(DI_db_reg_ctl_reg);
	db_reg_ctl_reg.db_apply=1;
	rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

	//m domain capture
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	//mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en=1;
	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply=1;
	rtd_outl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);

	//m domain display
	mdomain_disp_disp0_db_ctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
	//mdomain_disp_disp0_db_ctrl_reg.disp0_db_en=1;
	mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply=1;
	rtd_outl(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);

	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//DTG
	spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
	double_buffer_ctrl_reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
	//double_buffer_ctrl_reg.dreg_dbuf_en=1;
	double_buffer_ctrl_reg.dreg_dbuf_set=1;
	rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);
	spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//UZD
	ich1_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
	//ich1_uzd_db_ctrl_reg.h_db_en=1;
	ich1_uzd_db_ctrl_reg.h_db_apply=1;
	//ich1_uzd_db_ctrl_reg.v_db_en=1;
	ich1_uzd_db_ctrl_reg.v_db_apply=1;
	rtd_outl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg,ich1_uzd_db_ctrl_reg.regValue);

	//UZU
	dm_uzu_db_ctrl_reg.regValue =rtd_inl(SCALEUP_DM_UZU_DB_CTRL_reg);
	//dm_uzu_db_ctrl_reg.db_en=1;
	dm_uzu_db_ctrl_reg.db_apply=1;
	rtd_outl(SCALEUP_DM_UZU_DB_CTRL_reg,dm_uzu_db_ctrl_reg.regValue);

	//DI HSD
	di_uzd_db_ctrl_reg.regValue =rtd_inl(HSD_DITHER_DI_UZD_DB_CTRL_reg);
	//di_uzd_db_ctrl_reg.h_db_en=1;
	di_uzd_db_ctrl_reg.h_db_apply=1;
	rtd_outl(HSD_DITHER_DI_UZD_DB_CTRL_reg,di_uzd_db_ctrl_reg.regValue);

	//DPLL
	dpll_double_buffer_ctrl_reg.regValue =rtd_inl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg);
	//dpll_double_buffer_ctrl_reg.double_buf_en=1;
	dpll_double_buffer_ctrl_reg.double_buf_apply=1;
	rtd_outl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg,dpll_double_buffer_ctrl_reg.regValue);

	//iedge
	//edsm_db_ctrl_reg.regValue =rtd_inl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR);
	//edsm_db_ctrl_reg.edsm_db_en=1;
	//edsm_db_ctrl_reg.edsm_db_apply=0;
	//rtd_outl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR,edsm_db_ctrl_reg.regValue);
	rgb2yuv_ich1_422to444_ctrl_db_reg.regValue = rtd_inl(RGB2YUV_ICH1_422to444_CTRL_DB_reg);
	//rgb2yuv_ich1_422to444_ctrl_reg.db_en = 0;
	rgb2yuv_ich1_422to444_ctrl_db_reg.db_apply = 1;
	rtd_outl(RGB2YUV_ICH1_422to444_CTRL_DB_reg,rgb2yuv_ich1_422to444_ctrl_db_reg.regValue);
}

void imd_smooth_sub_double_buffer_all_apply(void)
{
#ifdef CONFIG_DUAL_CHANNEL
	sub_vgip_vgip_chn2_double_buffer_ctrl_RBUS vgip_chn2_double_buffer_ctrl_reg;
	di_db_reg_ctl_RBUS db_reg_ctl_reg;
    mdom_sub_cap_cap1_reg_doublbuffer_RBUS mdom_sub_cap_cap1_reg_doublbuffer_reg;
    mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
    ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;
	scaledown_ich2_uzd_db_ctrl_RBUS ich2_uzd_db_ctrl_reg;
	scaleup_ds_uzu_db_ctrl_RBUS ds_uzu_db_ctrl_reg;
	pll27x_reg_dpll_double_buffer_ctrl_RBUS dpll_double_buffer_ctrl_reg;
//	iedge_smooth_edsm_db_ctrl_RBUS edsm_db_ctrl_reg;
	rgb2yuv_ich2_422to444_ctrl_RBUS rgb2yuv_ich2_422to444_ctrl_reg;
	unsigned long flags;//for spin_lock_irqsave
	//vgip
	vgip_chn2_double_buffer_ctrl_reg.regValue =rtd_inl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg);
	//vgip_chn2_act_hsta_width_reg.ch2_db_en=1;
	vgip_chn2_double_buffer_ctrl_reg.ch2_db_rdy=1;
	rtd_outl(SUB_VGIP_VGIP_CHN2_DOUBLE_BUFFER_CTRL_reg,vgip_chn2_double_buffer_ctrl_reg.regValue);

	//di
	db_reg_ctl_reg.regValue =rtd_inl(DI_db_reg_ctl_reg);
	//db_reg_ctl_reg.db_en=1;
	db_reg_ctl_reg.db_apply=1;
	rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

	//m domain capture
	mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue =rtd_inl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
	//mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en=1;
	mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply=1;
	rtd_outl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);

	//m domain display
	mdom_sub_disp_disp1_db_ctrl_reg.regValue =rtd_inl(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
	//mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en=1;
	mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply=1;
	rtd_outl(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//DTG
	spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
	ppoverlay_double_buffer_ctrl4_Reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL4_reg);
	//ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en=1;
	ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set=1;
	rtd_outl(PPOVERLAY_Double_Buffer_CTRL4_reg,ppoverlay_double_buffer_ctrl4_Reg.regValue);
	spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//ulock double buffer spinlock
	up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//UZD
	ich2_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg);
	//ich2_uzd_db_ctrl_reg.h_db_en=1;
	ich2_uzd_db_ctrl_reg.h_db_apply=1;
	//ich2_uzd_db_ctrl_reg.v_db_en=1;
	ich2_uzd_db_ctrl_reg.v_db_apply=1;
	rtd_outl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg,ich2_uzd_db_ctrl_reg.regValue);

	//UZU
	ds_uzu_db_ctrl_reg.regValue =rtd_inl(SCALEUP_DS_UZU_DB_CTRL_reg);
	//ds_uzu_db_ctrl_reg.db_en=1;
	ds_uzu_db_ctrl_reg.db_apply=1;
	rtd_outl(SCALEUP_DS_UZU_DB_CTRL_reg,ds_uzu_db_ctrl_reg.regValue);

	//DPLL
	dpll_double_buffer_ctrl_reg.regValue =rtd_inl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg);
	//dpll_double_buffer_ctrl_reg.double_buf_en=1;
	dpll_double_buffer_ctrl_reg.double_buf_apply=1;
	rtd_outl(PLL27X_REG_dpll_Double_Buffer_CTRL_reg,dpll_double_buffer_ctrl_reg.regValue);

	//iedge
	//edsm_db_ctrl_reg.regValue =rtd_inl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR);
	//edsm_db_ctrl_reg.edsm_db_en=1;
	//edsm_db_ctrl_reg.edsm_db_apply=0;
	//rtd_outl(IEDGE_SMOOTH_EDSM_DB_CTRL_VADDR,edsm_db_ctrl_reg.regValue);
	rgb2yuv_ich2_422to444_ctrl_reg.regValue = rtd_inl(RGB2YUV_ICH2_422to444_CTRL_reg);
	//rgb2yuv_ich2_422to444_ctrl_reg.db_en = 0;
	rgb2yuv_ich2_422to444_ctrl_reg.db_apply = 1;
	rtd_outl(RGB2YUV_ICH2_422to444_CTRL_reg,rgb2yuv_ich2_422to444_ctrl_reg.regValue);
#endif
}

void imd_smooth_main_double_buffer_apply(zoom_double_buffer_apply_ip apply_ip)
{
	vgip_vgip_chn1_double_buffer_ctrl_RBUS vgip_vgip_chn1_double_buffer_ctrl_reg;
	di_db_reg_ctl_RBUS  db_reg_ctl_reg;
	mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;
	mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
	scaledown_ich1_uzd_db_ctrl_RBUS ich1_uzd_db_ctrl_reg;
	scaleup_dm_uzu_db_ctrl_RBUS dm_uzu_db_ctrl_reg;
	hsd_dither_di_uzd_db_ctrl_RBUS di_uzd_db_ctrl_reg;

	switch(apply_ip)
	{
	  case ZOOM_VGIP:
		vgip_vgip_chn1_double_buffer_ctrl_reg.regValue =rtd_inl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg);
		vgip_vgip_chn1_double_buffer_ctrl_reg.ch1_db_rdy=1;
		rtd_outl(VGIP_VGIP_CHN1_DOUBLE_BUFFER_CTRL_reg,vgip_vgip_chn1_double_buffer_ctrl_reg.regValue);
	  	break;
	  case ZOOM_DI:
	  	db_reg_ctl_reg.regValue =rtd_inl(DI_db_reg_ctl_reg);
		db_reg_ctl_reg.db_apply=1;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);
		break;
	  case ZOOM_DI_HSD:
	  	di_uzd_db_ctrl_reg.regValue =rtd_inl(HSD_DITHER_DI_UZD_DB_CTRL_reg);
		di_uzd_db_ctrl_reg.h_db_apply=1;
		rtd_outl(HSD_DITHER_DI_UZD_DB_CTRL_reg,di_uzd_db_ctrl_reg.regValue);
	  	break;
	  case ZOOM_UZD:
	  	ich1_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg);
		ich1_uzd_db_ctrl_reg.h_db_apply=1;
		ich1_uzd_db_ctrl_reg.v_db_apply=1;
		rtd_outl(SCALEDOWN_ICH1_UZD_DB_CTRL_reg,ich1_uzd_db_ctrl_reg.regValue);
	  	break;
	  case ZOOM_M_CAP:
	  	mdomain_cap_cap0_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply=1;
		rtd_outl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);
	  	break;
	  case ZOOM_M_DISP:
	  	mdomain_disp_disp0_db_ctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
		mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply=1;
		rtd_outl(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
	  	break;
	  case ZOOM_UZU:
	  	dm_uzu_db_ctrl_reg.regValue =rtd_inl(SCALEUP_DM_UZU_DB_CTRL_reg);
		dm_uzu_db_ctrl_reg.db_apply=1;
		rtd_outl(SCALEUP_DM_UZU_DB_CTRL_reg,dm_uzu_db_ctrl_reg.regValue);
	  	break;
	  default:
	    break;
	}
}

