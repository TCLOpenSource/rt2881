#define disablepart 0


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
//#include <mach/io.h>

//---header files-----
#include "tvscalercontrol/scaler/scalerstruct.h"
#include "tvscalercontrol/scalerdrv/pipmp.h"
#include "tvscalercontrol/scalerdrv/scalerdrv.h"
#include "tvscalercontrol/scalerdrv/scalerip.h"

///#include <tvscalercontrol/scaler/scalerlib.h>
#include <tvscalercontrol/scaler/state.h>//pthread_mutex_t			scalerMutex;				// scaler mutex

#include <tvscalercontrol/io/ioregdrv.h>
//vip
#include <scaler/vipCommon.h>
#include <tvscalercontrol/vip/nr.h>//drvif_color_iEdgeSmooth_init
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/vip/ultrazoom.h>

//#include "tvscalercontrol/vip/ultrazoom.h"
//end of vip

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
#include <tvscalercontrol/scalerdrv/scaler_nonlibdma.h>
//below are rbus files
#include "rbus/vodma_reg.h"
#include <rbus/vgip_reg.h>
#include <rbus/sub_vgip_reg.h>
#include <rbus/two_step_uzu_reg.h>

#include <rbus/di_reg.h>	//#include <rbus/rbusDiReg.h>
//#include <rbus/hdmi_clkdet_reg.h>	//#include <rbus/rbusHDMIReg.h>

//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/ppoverlay_reg.h>
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
#include <rbus/ptol_reg.h>
//#include <kernel/scaler/scalerInfo.h>
//#include <kernel/scaler/scalerDrv.h>
//#include <kernel/scaler/scalerIRQ.h>
//#include <kernel/scaler/smoothtoggle.h>
//#include "base_type.h"
#include <rtd_log/rtd_module_log.h>
#include <vo/rtk_vo.h>
#include <rbus/timer_reg.h>

extern struct semaphore* get_vsc_semaphore(void);
extern struct semaphore* get_force_i3ddma_semaphore(void);

extern unsigned char VSC_Check_Signal_Lock(unsigned char display);
extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr;
extern void zoom_smoothtoggle_display_only_shift_xy(unsigned char display);
extern void scaler_airModeSendDispSize(unsigned char display, KADP_VIDEO_RECT_T outregion);
#ifndef UT_flag
extern void h3ddma_nn_set_recfg_flag(unsigned char bVal);
extern unsigned int scalerAI_get_AIPQ_mode_enable_flag(void);
#endif // #ifndef UT_flag
//USER:LewisLee DATE:2015/09/26
//Porch limite is relate by HW
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


//--end of for kenel


SMOOTH_TOGGLE_INFO imdSmoothInfo[2]={{0},{0}};

//unsigned char Smooth_Toggle_dislay;
unsigned char frame_rate_change_reduce;
//unsigned char I_P_type;
unsigned char smooth_toggle_update_flag=0;
SLR_RATIO_TYPE ratio_type=SLR_RATIO_DISABLE;
SLR_RATIO_TYPE ratio_type_pre;
#define CONFIG_MDOMAIN_BURST_SIZE 512
//SLR_RATIO_TYPE ratio_type_pre;

///static StructSrcRect		custom_source_dispwin = {0, 0, 0, 0};//not used rika 20140929
StructSrcRect custom_dispwin = {0, 0, 0, 0};
///static unsigned char		custom_osd4k2k = 0;//not used rika 20140929

//static bool sm_go_2d_to_3d_flag = FALSE;

#if 0
//DI related flag
static unsigned char m_CP_TemporalEnable = 0;
static unsigned char m_CP_Temporal_XC_en = 0;
static unsigned char m_ChromaError_En = 0;
static unsigned char m_BTR_BlendTemporalRecoveryEn = 0;
static unsigned char m_HMC_follow_MA_result_En = 0;
//static unsigned char m_film = 0;
//static unsigned char m_fw_film = 0;


static unsigned char is_src_YPbPr_Component[2];//for main and sub
#endif

void smooth_toggle_print_smooth_info(unsigned char display);


#define CONFIG_CPU_XTAL_FREQ 27000000

//#define CONFIG_DCLK_CHECK 1

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

#if defined(IS_ARCH_ARM_COMMON)
#define ONE_MILISECOND_CLOCK 93500
#elif defined(IS_DARWIN)
#define ONE_MILISECOND_CLOCK 53300
#else //Mac
#define ONE_MILISECOND_CLOCK 28000
#endif

extern unsigned char vbe_disp_oled_orbit_enable;
extern unsigned char vbe_disp_oled_orbit_mode;

//------------begin of zoom fpp api rika 20140614

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

extern KADP_VIDEO_RECT_T air_mode_save_outregion_parm[MAX_DISP_CHANNEL_NUM];

unsigned char m_film_clear = 0;
int m_film = 0;
unsigned char g_fw_film_en = 1;

static unsigned int h_delay = 0;
static unsigned int v_delay = 0;
StructSrcRect newInput;

//zoomfunction list rika 20140614
void zoom_imd_smooth_decide_rtnr(unsigned char display); //rika 20140609 test
unsigned char zoom_imd_smooth_cal_vgip_capture(unsigned char display,
		unsigned int  un32_hposition, unsigned int  un32_vposition,
		unsigned int  un32_hsize, unsigned int  un32_vsize );

void zoom_imd_smooth_color_ultrazoom_config_scaling_down_di_hsd(unsigned char display);
void zoom_imd_smooth_color_ultrazoom_config_scaling_down(unsigned char display);
void zoom_imd_smooth_color_ultrazoom_set_scale_down_di_hsd(unsigned char channel, SMOOTH_TOGGLE_SIZE* ptInSize, SMOOTH_TOGGLE_SIZE* ptOutSize, UINT8 panorama);
void zoom_imd_smooth_color_ultrazoom_set_scale_down(unsigned char channel, SMOOTH_TOGGLE_SIZE* ptInSize, SMOOTH_TOGGLE_SIZE* ptOutSize, UINT8 panorama);
void zoom_imd_smooth_config_uzd_uzu_param(unsigned char display);
void zoom_imd_smooth_get_display_size(unsigned char display);
int zoom_check_size_error(unsigned char display);

int zoom_imd_smooth_set_info(unsigned char display, unsigned char rcflag,unsigned char rdflag);

void zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(unsigned char access,unsigned char display);//review
void zoom_imd_smooth_display_on(void);
unsigned char zoom_modestate_decide_double_dvs_enable(void);
void zoom_imd_smooth_color_ultrazoom_config_scaling_up(unsigned char display);
void zoom_imd_smooth_color_ultrazoom_setscaleup(unsigned char display, SMOOTH_TOGGLE_SIZE* ptInSize, SMOOTH_TOGGLE_SIZE* ptOutSize, unsigned char panorama);

unsigned char zoom_imd_smooth_wait_to_apply_sucessfully(unsigned char display);
void zoom_imd_smooth_setup_vgip_even_odd_toggle(void);
void zoom_imd_smooth_set_idomain(void);
void zoom_scalerip_bypass(unsigned char byPass,unsigned char display);



int zoom_imd_smooth_toggle_vo_proc
(unsigned char display,   unsigned int  crop_hpos, unsigned int  crop_vpos,
    unsigned int  crop_hsize, unsigned int  crop_vsize,
    unsigned int  disp_hpos, unsigned int  disp_vpos,
    unsigned int  disp_hsize, unsigned int  disp_vsize    );


void zoom_update_info_from_vo(void);


//---------------

//zoom_first_crop,zoom_first_dis
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


unsigned char fInputRegionSame[MAX_DISP_CHANNEL_NUM]={false};// = {false,false};
unsigned char fOutputRegionSame[MAX_DISP_CHANNEL_NUM]={false};// = {false,false};


extern unsigned short shape_wide,shape_high;



//extern
#if disablepart //to fix compile error for kenel rika 20140929
extern pthread_mutex_t 		scalerMutex;
#endif
extern bool drvif_color_get_scaleup_cuthor4line(void);
extern bool drvif_color_get_scaleup_cutver2line(void);
extern VO_VIDEO_PLANE Scaler_Get_3D_VoPlane(void);
extern VO_VIDEO_PLANE Scaler_Get_2D_VoPlane(void);


extern unsigned char fw_scalerip_get_DI_chroma_10bits(void);
extern void fw_scaler_double_buffer_maskl(unsigned char display,unsigned int addr,  unsigned int andmask, unsigned int ormask);
//extern void set_di_doublebuf_write(unsigned char diflag,unsigned char _2d_flag);

extern void vo_smoothtoggle_memory_alloc(unsigned char display);
extern void Check_smooth_toggle_update_flag(unsigned char display);
extern SOURCE_INFO_SMOOTH_TOGGLE get_vo_smoothtoggle_state(unsigned char display);
//extern unsigned char Scaler_Get3DMode(void);
extern VSC_INPUT_TYPE_T Get_DisplayMode_Src(unsigned char display);
//------------end of zoom fpp api
static unsigned char vo_zoom_st_enable[MAX_DISP_CHANNEL_NUM];
unsigned char vo_zoom_go_smooth_toggle_get(unsigned char display)
{
	return vo_zoom_st_enable[display];
}
void vo_zoom_go_smooth_toggle_set(unsigned char enable,unsigned char display)
{
	vo_zoom_st_enable[display]=enable;
}

bool imd_smooth_Rt_Delay(UINT32 milliSec)
{
	volatile UINT32 retVal=0;
	volatile UINT32 i,j;

	for (j=0; j<milliSec; j++) {
		// 1 millisecond for this whole loop
		for (i=0; i<ONE_MILISECOND_CLOCK; i++)
			retVal++;
	}

	return true;
}

static unsigned char zoom_smooth_toggle_finish = ZOOM_STATE_NONE ;//default none

extern unsigned int smooth_toggle_game_mode_timeout_trigger_flag;
extern unsigned int smooth_toggle_game_mode_check_cnt;

void zoom_smooth_toggle_set_finish(unsigned char enable)
{
	zoom_smooth_toggle_finish = enable;
}

unsigned char zoom_smooth_toggle_get_finish(void)
{
	return zoom_smooth_toggle_finish;
}

unsigned int DISP_WID=1920, DISP_LEN=1080;
unsigned int INPUT_WID=0,INPUT_LEN=0;

#define REDUCE_DI_MEMORY_LIMIT_THRESHOLD 720
#define REDUCE_DI_MEMORY_LIMIT_THRESHOLD_SIZE 920
#define REDUCE_NR_BANDWIDTH_LIMIT_HOR_THRESHOLD 	1440
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

#if 0
SCALER_DISP_CHANNEL imd_smooth_get_curr_display(void)
{
	return (SCALER_DISP_CHANNEL)Smooth_Toggle_dislay;
}
void imd_smooth_set_curr_display(unsigned char display)
{
	Smooth_Toggle_dislay=display;
}
#endif
/*
static VO_VIDEO_PLANE curVoPlane=VO_VIDEO_PLANE_V1;
void smooth_toggle_Scaler_Set_CurVoInfo_plane(VO_VIDEO_PLANE plane)
{
	//	if(curVoPlane != plane)
	//		rtd_pr_smt_debug("%s=%d\n", __FUNCTION__, plane);

	curVoPlane = plane;
	return;
}

VO_VIDEO_PLANE smooth_toggle_Scaler_Get_CurVoInfo_plane(void)
{
	return curVoPlane;
}
*/

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
	//mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;
	//mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	scaledown_ich1_uzd_db_ctrl_RBUS ich1_uzd_db_ctrl_reg;
	scaleup_dm_uzu_db_ctrl_RBUS dm_uzu_db_ctrl_reg;
	hsd_dither_di_uzd_db_ctrl_RBUS di_uzd_db_ctrl_reg;
	pll27x_reg_dpll_double_buffer_ctrl_RBUS dpll_double_buffer_ctrl_reg;
	iedge_smooth_edsm_db_ctrl_RBUS edsm_db_ctrl_reg;
	rgb2yuv_ich1_422to444_ctrl_db_RBUS rgb2yuv_ich1_422to444_ctrl_db_reg;
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;
	ptol_ptol_db_ctrl_RBUS  ptol_ptol_db_ctrl_reg;
	two_step_uzu_sr_db_ctrl_RBUS two_step_uzu_sr_db_ctrl_reg;
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
#ifdef CONFIG_I2RND_ENABLE
		//Eric@0508 I2rnd do not set DI Double buffer
		if(!Scaler_I2rnd_get_timing_enable()){
			db_reg_ctl_reg.db_en=1;
		}
		else{
			db_reg_ctl_reg.db_en=0;
		}
#else
		db_reg_ctl_reg.db_en=1;
#endif
		db_reg_ctl_reg.db_apply=0;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

		//rtd_pr_smt_debug("0.1----------\n");

		//m domain capture
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
			//cap_reg_doublbuffer_reg.cap1_db_en=1;
			//cap_reg_doublbuffer_reg.cap1_db_apply=0;
			//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
			nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_enable(0, 1);
		}

		//rtd_pr_smt_debug("0.2----------\n");

		//m domain display
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
			//ddr_mainsubctrl_reg.disp1_double_enable=1;
			//ddr_mainsubctrl_reg.disp1_double_apply=0;
			//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
			nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(0, 1);
		}

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
#ifdef CONFIG_I2RND_ENABLE
		//Eric@0508 I2rnd do not set DI Double buffer
		if(!Scaler_I2rnd_get_timing_enable()){
			db_reg_ctl_reg.db_en=1;
		}
		else{
			db_reg_ctl_reg.db_en=0;
		}
#else
		db_reg_ctl_reg.db_en=1;
#endif
		db_reg_ctl_reg.db_apply=1;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

		//rtd_pr_smt_debug("0.1----------\n");

		//m domain capture
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
			//cap_reg_doublbuffer_reg.cap1_db_en=0;
			//cap_reg_doublbuffer_reg.cap1_db_apply=0;
			//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
			nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_enable(0, 0);
		}

		//rtd_pr_smt_debug("0.2----------\n");

		//m domain display
		//if((get_ori_rotate_mode(SLR_MAIN_DISPLAY) == get_rotate_mode(SLR_MAIN_DISPLAY)) && (get_rotate_mode(SLR_MAIN_DISPLAY) == 0))
		{
			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
				//ddr_mainsubctrl_reg.disp1_double_enable=0;
				//ddr_mainsubctrl_reg.disp1_double_apply=0;
				//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
				nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(0, 0);
			}
		}
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue = rtd_inl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg);
			//mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_en = 0;
			//mdomain_vi_main_mdom_vi_doublebuffer_reg.vi_db_apply = 0;
			//rtd_outl(MDOMAIN_VI_MAIN_mdom_vi_doublebuffer_reg, mdomain_vi_main_mdom_vi_doublebuffer_reg.regValue);
			nonlibdma_set_vi_mdom_doublebuffer_reg_doublebuffer_enable(0, 0);
		}

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
			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
				//ddr_mainsubctrl_reg.disp2_double_enable=0;
				//ddr_mainsubctrl_reg.disp2_double_apply=0;
				//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
				nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(1, 0);
			}

			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
				//cap_reg_doublbuffer_reg.cap2_db_en=0;
				//cap_reg_doublbuffer_reg.cap2_db_apply=0;
				//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
				nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_enable(1, 0);
			}
		}
		ptol_ptol_db_ctrl_reg.regValue = rtd_inl(PTOL_PtoL_DB_CTRL_reg);
		ptol_ptol_db_ctrl_reg.double_buf_en = 0;//enable double buffer
		ptol_ptol_db_ctrl_reg.double_buf_set = 0;
		rtd_outl(PTOL_PtoL_DB_CTRL_reg, ptol_ptol_db_ctrl_reg.regValue);

		two_step_uzu_sr_db_ctrl_reg.regValue = rtd_inl(TWO_STEP_UZU_SR_DB_CTRL_reg);
		two_step_uzu_sr_db_ctrl_reg.db_en=0;
		two_step_uzu_sr_db_ctrl_reg.db_apply = 0;
		rtd_outl(TWO_STEP_UZU_SR_DB_CTRL_reg,two_step_uzu_sr_db_ctrl_reg.regValue);
		
		iv2dv_double_buffer_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
		iv2dv_double_buffer_reg.iv2dv_db_en = 0;
		iv2dv_double_buffer_reg.iv2dv_db_apply= 0;
		IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, iv2dv_double_buffer_reg.regValue);
	}
#endif
}

void imd_smooth_enable_sub_double_buffer(unsigned char bEnable)
{
#if 1
#ifdef CONFIG_DUAL_CHANNEL
	sub_vgip_vgip_chn2_double_buffer_ctrl_RBUS vgip_chn2_double_buffer_ctrl_reg;
	di_db_reg_ctl_RBUS db_reg_ctl_reg;
	//mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;
	//mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
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
#ifdef CONFIG_I2RND_ENABLE
		//Eric@0508 I2rnd do not set DI Double buffer
		if(!Scaler_I2rnd_get_timing_enable()){
			db_reg_ctl_reg.db_en=1;
		}
		else{
			db_reg_ctl_reg.db_en=0;
		}
#else
		db_reg_ctl_reg.db_en=1;
#endif
		db_reg_ctl_reg.db_apply=0;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);

		//m domain capture
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
			//cap_reg_doublbuffer_reg.cap2_db_en=1;
			//cap_reg_doublbuffer_reg.cap2_db_apply=0;
			//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
			nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_enable(1, 1);
		}

		//m domain display
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
			//ddr_mainsubctrl_reg.disp2_double_enable=1;
			//ddr_mainsubctrl_reg.disp2_double_apply=0;
			//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
			nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(1, 1);
		}

		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		double_buffer_ctrl_reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
		double_buffer_ctrl_reg.dsubreg_dbuf_en=1;
		double_buffer_ctrl_reg.dsubreg_dbuf_set=0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);
        m8p_dtg_set_sub_double_buffer_enable(DOUBLE_BUFFER_D3);
        m8p_dtg_set_sub_double_buffer_not_apply(DOUBLE_BUFFER_D3);

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
#if 0
		//di
		db_reg_ctl_reg.regValue =rtd_inl(DI_db_reg_ctl_reg);
#ifdef CONFIG_I2RND_ENABLE
		//Eric@0508 I2rnd do not set DI Double buffer
		if(!Scaler_I2rnd_get_enable()){
			db_reg_ctl_reg.db_en=1;
		}
		else{
			db_reg_ctl_reg.db_en=0;
		}
#else
		db_reg_ctl_reg.db_en=1;
#endif
		db_reg_ctl_reg.db_apply=1;
		rtd_outl(DI_db_reg_ctl_reg,db_reg_ctl_reg.regValue);
#endif
		//m domain capture
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
			//cap_reg_doublbuffer_reg.cap2_db_en=0;
			//cap_reg_doublbuffer_reg.cap2_db_apply=0;
			//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
			nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_enable(1, 0);
		}

		//m domain display
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
			//ddr_mainsubctrl_reg.disp2_double_enable=0;
			//ddr_mainsubctrl_reg.disp2_double_apply=0;
			//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
			nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_enable(1, 0);
		}
#if 0
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		//DTG
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		double_buffer_ctrl_reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
		double_buffer_ctrl_reg.dsubreg_dbuf_en=0;
		double_buffer_ctrl_reg.dsubreg_dbuf_set=0;
		rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);
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
#endif
}




static unsigned char				Hpos_Move_Factor = 2;
static unsigned char				Vpos_Move_Factor = 1;
static UINT8 ZOOM_H_Position_Adjust_Enable		= _ENABLE;
static UINT8 ZOOM_H_Position_Adjust_Max			= 10;
static UINT8 ZOOM_H_Position_Adjust_Min			= 0;
static UINT8 ZOOM_V_Position_Adjust_Max			= 10;
static UINT8 ZOOM_V_Position_Adjust_Min			= 0;
static unsigned char				Main_Channel_IVS2DVSDelay = 0;
static unsigned char 				ucIVS2DVSDelay = 0;
static unsigned char 				ucIHSDelay = 0;
static unsigned char 				ucIPH_Max_Margin =0;
static unsigned char 				ucIPH_Min_Margin =0;
static unsigned char 				ucIPV_Max_Margin =0;
static unsigned char 				ucIPV_Min_Margin =0;

static unsigned char H_Position=0x80;
static unsigned char V_Position=0x80;
static short VGIP_IHStart=0,VGIP_IVStart=0;

unsigned char smooth_toggle_Scaler_GetHPosMoveFactor(void)
{
	return Hpos_Move_Factor;
}

void smooth_toggle_Scaler_SetHPosMoveFactor(unsigned char factor)
{
	Hpos_Move_Factor = factor;
}

unsigned char smooth_toggle_Scaler_GetVPosMoveFactor(void)
{
	return Vpos_Move_Factor;
}

void smooth_toggle_Scaler_SetVPosMoveFactor(unsigned char factor)
{
	Vpos_Move_Factor = factor;
}
UINT8 Scaler_Get_ZOOM_Position_Adjust_Enable(void)
{
	return ZOOM_H_Position_Adjust_Enable;
}

void Scaler_Set_ZOOM_Position_Adjust_Enable(UINT8 ucEnable)
{


	if(ucEnable==0){
		rtd_pr_smt_debug("_rk ZOOM_H_Position_Adjust_Enable=0 so far");
	}
	ZOOM_H_Position_Adjust_Enable = ucEnable;
}

unsigned char  smooth_toggle_Scaler_Get_Adjust_H_Position_Move_Tolerance(void)
{
	unsigned char ucH_Move_Tolerance = 0;
	if(_ENABLE == Scaler_Get_ZOOM_Position_Adjust_Enable())
	{
		ucH_Move_Tolerance = (ZOOM_H_Position_Adjust_Max - ZOOM_H_Position_Adjust_Min);
	}
	else
		ucH_Move_Tolerance = 0x00;

	//rtd_pr_smt_debug("_rk %s:%d %d\n",__FUNCTION__,__LINE__,ucH_Move_Tolerance);
	return ucH_Move_Tolerance;
}
unsigned char  smooth_toggle_Scaler_Get_Adjust_V_Position_Move_Tolerance(void)
{
	unsigned char ucV_Move_Tolerance = 0;
	if(_ENABLE == Scaler_Get_ZOOM_Position_Adjust_Enable())
	{
		ucV_Move_Tolerance = (ZOOM_V_Position_Adjust_Max - ZOOM_V_Position_Adjust_Min);
	}
	else// if(_DISABLE == Scaler_Get_VO_Position_Adjust_Enable())
	{
		ucV_Move_Tolerance = 0x00;
	}
	return ucV_Move_Tolerance;
}
unsigned char smooth_toggle_Scaler_DispGetIvs2DvsDelay(void)
{
	return ucIVS2DVSDelay;
}

void smooth_toggle_Scaler_DispSetIvs2DvsDelay(unsigned char value)
{
	ucIVS2DVSDelay = value;
	return;
}

unsigned char smooth_toggle_Scaler_DispGetIhsDelay(void)
{
	return ucIHSDelay;
}

void smooth_toggle_Scaler_DispSetIhsDelay(unsigned char value)
{
	ucIHSDelay = value;
	return;
}
unsigned char smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_TYPE type)
{
	switch (type)
	{
		case SLR_IPMARGIN_H_MAX:
			return ucIPH_Max_Margin;
		case SLR_IPMARGIN_H_MIN:
			return ucIPH_Min_Margin;
		case SLR_IPMARGIN_V_MAX:
			return ucIPV_Max_Margin;
		case SLR_IPMARGIN_V_MIN:
			return ucIPV_Min_Margin;
		default:
			return 0;
	}
}

void smooth_toggle_Scaler_DispSetIpMargin(SLR_IPMARGIN_TYPE type, unsigned char value)
{
	switch (type)
	{
		case SLR_IPMARGIN_H_MAX:
			ucIPH_Max_Margin = value;
			break;
		case SLR_IPMARGIN_H_MIN:
			ucIPH_Min_Margin = value;
			break;
		case SLR_IPMARGIN_V_MAX:
			ucIPV_Max_Margin = value;
			break;
		case SLR_IPMARGIN_V_MIN:
			ucIPV_Min_Margin = value;
			break;
		default:
			break;
	}

	return;
}

unsigned char smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay(void)
{
	return Main_Channel_IVS2DVSDelay;
}

void smooth_toggle_Scaler_DispSetMainChannelIVS2DVSDelay(unsigned char value)
{
	Main_Channel_IVS2DVSDelay = value;
}
unsigned char smooth_toggle_Scaler_Get_Enable_Adjust_Position_Src(void)
{
	unsigned char ucEnable = _DISABLE;
	if(_ENABLE == Scaler_Get_ZOOM_Position_Adjust_Enable())
	{
		ucEnable = _ENABLE;
	}

	return ucEnable;
}



unsigned char smooth_toggle_GET_H_POSITION(void)
{
	return H_Position;
}
void smooth_toggle_SET_H_POSITION(unsigned char position)
{
	H_Position = position;
}

unsigned char smooth_toggle_GET_V_POSITION(void)
{
	return V_Position;
}
void smooth_toggle_SET_V_POSITION(unsigned char position)
{
	V_Position = position;
}
void smooth_toggle_scaler_set_vgip_capture_shift(unsigned char display, char shiftx, char shifty)
{
	short IHStart = 0, IVStart = 0, porch_region = 0;
	unsigned char usV_FrontPorch = 0, IVS_Delay = 0, IHS_Delay = 0, Compensate = 0;
	vgip_vgip_chn1_act_hsta_width_RBUS vgip_chn1_act_hsta_width_reg;
	vgip_vgip_chn1_act_vsta_length_RBUS vgip_chn1_act_vsta_length_reg;
	vgip_vgip_chn1_delay_RBUS vgip_chn1_delay_reg;
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	sub_vgip_vgip_chn2_act_hsta_width_RBUS vgip_chn2_act_hsta_width_reg;
	sub_vgip_vgip_chn2_act_vsta_length_RBUS vgip_chn2_act_vsta_length_reg;
	sub_vgip_vgip_chn2_delay_RBUS vgip_chn2_delay_reg;
#endif


	//	rtd_pr_smt_info("\033[1;34m [Peter] %s%d %s,X:%d,Y:%d\033[m\n",__FILE__, __LINE__,__FUNCTION__,shiftx, shifty);
	IHStart =imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA-(shiftx * 2); // must even

	porch_region =imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL- imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID;

	if(porch_region >= _VGIP_H_FRONT_PORCH_MIN)
	{
		if(IHStart >= (porch_region - _VGIP_H_FRONT_PORCH_MIN))
		{
			IHS_Delay = IHStart - (porch_region - _VGIP_H_FRONT_PORCH_MIN) + 1;
			IHStart -= IHS_Delay;
			rtd_pr_smt_debug("H front porch less than 3\n");
		}
	}
	else
	{
		rtd_pr_smt_debug("H porch region less than 3, abnormal case\n");
	}

	if(_CHANNEL1 == (SCALER_CHANNEL_NUM)display)
	{
		//H position
		vgip_chn1_act_hsta_width_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_ACT_HSTA_Width_reg);
		vgip_chn1_act_hsta_width_reg.ch1_ih_act_sta = IHStart;
		rtd_outl(VGIP_VGIP_CHN1_ACT_HSTA_Width_reg, vgip_chn1_act_hsta_width_reg.regValue);
		VGIP_IHStart=IHStart;

		if(0 == IHS_Delay)
		{
			//if not need protect H porch, set IHS_Delay as original
			vgip_chn1_delay_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_DELAY_reg);
			IHS_Delay = vgip_chn1_delay_reg.ch1_ihs_dly;
		}

		IVS_Delay = 0x00;
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else	// if(_CHANNEL2 == GET_INPUT_CHANNEL(display))
	{
		//H position
		vgip_chn2_act_hsta_width_reg.regValue = rtd_inl(SUB_VGIP_VGIP_CHN2_ACT_HSTA_Width_reg);
		vgip_chn2_act_hsta_width_reg.ch2_ih_act_sta = IHStart;
		rtd_outl(SUB_VGIP_VGIP_CHN2_ACT_HSTA_Width_reg, vgip_chn2_act_hsta_width_reg.regValue);

		if(0 == IHS_Delay)
		{
			//if not need protect H porch, set IHS_Delay as original
			vgip_chn2_delay_reg.regValue = rtd_inl(SUB_VGIP_VGIP_CHN2_DELAY_reg);
			IHS_Delay = vgip_chn2_delay_reg.ch2_ihs_dly;
		}

		//V position
		//		vgip_chn2_delay_reg.regValue = rtdf_inl(SUB_VGIP_VGIP_CHN2_DELAY_reg);
		//		IVS_Delay = vgip_chn2_delay_reg.ch2_ivs_dly;
		IVS_Delay = 0x00;
	}
#endif

	//rtd_pr_smt_debug("IPV_ACT_STA=%d, shifty=%d\n", imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_V_ACT_STA, shifty);

	if (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA < (shifty * 1))
	{
		rtd_pr_smt_debug("Error	SLR_INPUT_IPV_ACT_STA!!\n");
	}

	IVStart = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA-(shifty * 1);
	//frank@0916 solve mantis30020 ATV sometimes cannot show the live atv video
	//offline measure result fail,Vtotal <V active
	if( (imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL)<(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN))
	{
		rtd_pr_smt_debug("SLR_INPUT_V_LEN=%d, SLR_INPUT_IPV_ACT_LEN=%d\n", imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL, imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN);
		return;
	}

	porch_region = imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL- (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN);

	if(porch_region > IVStart)
	{
		usV_FrontPorch = (unsigned char)(porch_region - IVStart);
	}
	else
	{
		//IVST large than porch region
		usV_FrontPorch = _VGIP_V_FRONT_PORCH_MIN;

		IVS_Delay = IVStart - porch_region + 1 + usV_FrontPorch;

		if (porch_region > (usV_FrontPorch + 1 + 3) )//add by lichun zhao 20111212
			IVStart = porch_region - usV_FrontPorch - 1;
		else
			IVStart = 3;
	}


	//	usV_FrontPorch += IVS_Delay;

	//	rtd_pr_smt_info("IVStart:%x, usV_FrontPorch:%x\n",IVStart, usV_FrontPorch);

	// Adjust IVSt and IVS delay to avoid front portch length < _VGIP_V_FRONT_PORCH_MIN
	if(usV_FrontPorch < _VGIP_V_FRONT_PORCH_MIN)
	{
		Compensate = _VGIP_V_FRONT_PORCH_MIN - usV_FrontPorch;

		IVStart = IVStart - Compensate;
		IVS_Delay += Compensate;
	}

	//frank@0718 patch below code avoid IVStart<3 let HW fail
	if(IVStart <= 3)
		IVStart = 4;

	if(_CHANNEL1 == (SCALER_CHANNEL_NUM)display)
	{
		vgip_chn1_act_vsta_length_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg);
		vgip_chn1_act_vsta_length_reg.ch1_iv_act_sta = IVStart;
		rtd_outl(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg, vgip_chn1_act_vsta_length_reg.regValue);
		VGIP_IVStart=IVStart;
		//IHS Delay/ IVS Delay
		vgip_chn1_delay_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_DELAY_reg);
		vgip_chn1_delay_reg.ch1_ihs_dly = IHS_Delay;
		vgip_chn1_delay_reg.ch1_ivs_dly = IVS_Delay;
		rtd_outl(VGIP_VGIP_CHN1_DELAY_reg, vgip_chn1_delay_reg.regValue);

	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else// if(_CHANNEL2 == GET_INPUT_CHANNEL(display))
	{
		vgip_chn2_act_vsta_length_reg.regValue = rtd_inl(SUB_VGIP_VGIP_CHN2_ACT_VSTA_Length_reg);
		vgip_chn2_act_vsta_length_reg.ch2_iv_act_sta = IVStart;
		rtd_outl(SUB_VGIP_VGIP_CHN2_ACT_VSTA_Length_reg, vgip_chn2_act_vsta_length_reg.regValue);

		//IHS Delay/ IVS Delay
		vgip_chn2_delay_reg.regValue = rtd_inl(SUB_VGIP_VGIP_CHN2_DELAY_reg);
		vgip_chn2_delay_reg.ch2_ihs_dly = IHS_Delay;
		vgip_chn2_delay_reg.ch2_ivs_dly = IVS_Delay;
		rtd_outl(SUB_VGIP_VGIP_CHN2_DELAY_reg, vgip_chn2_delay_reg.regValue);
	}
#endif
}


void smooth_toggle_drvif_scaler_position_setting(unsigned char display, unsigned char is_init, char shiftx, char shifty)
{//modify
	unsigned short IHS_Delay = 0, IVS_Delay = 0;
	unsigned char ucHpositionTolerance = 0/*, ucVpositionTolerance = 0*/;
	unsigned char ucHpositionMoveFactor = 0;
	vgip_vgip_chn1_delay_RBUS vgip_chn1_delay_reg;
	sub_vgip_vgip_chn2_delay_RBUS vgip_chn2_delay_reg;
	sub_vgip_vgip_chn2_act_hsta_width_RBUS vgip_chn2_act_hsta_width_reg;
	sub_vgip_vgip_chn2_act_vsta_length_RBUS vgip_chn2_act_vsta_length_reg;

	if(smooth_toggle_Scaler_Get_Enable_Adjust_Position_Src()==_DISABLE)
		return;

	if(_TRUE == is_init)
	{
		//set VGIP capture as table value
		smooth_toggle_scaler_set_vgip_capture_shift(display, 0, 0); //set vgip_ihstart,vgip_ivstart
	}

	//	rtd_pr_smt_debug("shiftx=%d,shifty=%d\n",shiftx,shifty);
	ucHpositionTolerance = smooth_toggle_Scaler_Get_Adjust_H_Position_Move_Tolerance()>>1;//5 or 0
	//	ucVpositionTolerance = smooth_toggle_Scaler_Get_Adjust_V_Position_Move_Tolerance();
	ucHpositionMoveFactor = smooth_toggle_Scaler_GetHPosMoveFactor();// 2
	if(_CHANNEL1 == (SCALER_CHANNEL_NUM)display)// imd_smooth_get_curr_display()==SLR_MAIN_DISPLAY rika 20140610
	{
		//remove video fw calculation
		//IHS_Delay = imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_H_ACT_STA+_H_POSITION_MOVE_TOLERANCE+ucHpositionTolerance - VGIP_IHStart;


		switch(Scaler_InputSrcGetType(display))//rika 20140610 add system each src flow
		{
			case _SRC_VGA:
				//IHS_Delay = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA) + _H_POSITION_MOVE_TOLERANCE - vgip_chn1_act_hsta_width_reg.ch1_ih_act_sta;
				IHS_Delay = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA+_H_POSITION_MOVE_TOLERANCE+ucHpositionTolerance - VGIP_IHStart;
				rtd_pr_smt_debug("_rk %d,IHS_Delay:%d\n",__LINE__,IHS_Delay);

				break;

			case _SRC_YPBPR:
				//IHS_Delay = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA) + (_H_POSITION_MOVE_TOLERANCE + ucHpositionTolerance) - vgip_chn1_act_hsta_width_reg.ch1_ih_act_sta;
				IHS_Delay = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA+_H_POSITION_MOVE_TOLERANCE+ucHpositionTolerance - VGIP_IHStart;
				//IHS_Delay += ucHpositionTolerance*(ucHpositionMoveFactor - 1);
				IHS_Delay += ucHpositionTolerance*(ucHpositionMoveFactor - 1);
				//rtd_pr_smt_debug("_rk %d,IHS_Delay:%d\n",__LINE__,IHS_Delay);

				break;

			case _SRC_TV:
			case _SRC_CVBS:
			case _SRC_SV:
			case _SRC_HDMI:
			case _SRC_DVI:
				//IHS_Delay = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA) + ucHpositionTolerance - vgip_chn1_act_hsta_width_reg.ch1_ih_act_sta;
				IHS_Delay = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA+ucHpositionTolerance - VGIP_IHStart;
				//IHS_Delay += ucHpositionTolerance*(ucHpositionMoveFactor - 1);
				IHS_Delay += ucHpositionTolerance*(ucHpositionMoveFactor - 1);
				//rtd_pr_smt_debug("_rk %d,IHS_Delay:%d\n",__LINE__,IHS_Delay);


				break;
		}

		IHS_Delay = (unsigned short)(IHS_Delay + (shiftx * ucHpositionMoveFactor));

		//rtd_pr_smt_debug("_rk %d,IHS_Delay:%d\n",__LINE__,IHS_Delay);





		if(_TRUE == (imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC))
		{
			if(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA==VGIP_IVStart)
			{
				IVS_Delay = 0;
			}
			else
			{
				if(_TRUE == imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
				{
					if((smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >> 1) >= (0 - shifty))//need review
						IVS_Delay = (smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >> 1) + shifty;
					else
						IVS_Delay = 0;
				}
				else
				{
					if(smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >= (0 - shifty))
						IVS_Delay = smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() + shifty;
					else
						IVS_Delay = 0;
				}
			}
		}
		else
		{
			IVS_Delay = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA + smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() - VGIP_IVStart;

			//USER:LewisLee DATE:2011/01/14
			//to prevent IVS_Delay <0, picture abnormal
			if((shifty <= 0) || (IVS_Delay >= shifty))
				IVS_Delay = (unsigned short)((short)IVS_Delay - (shifty));
			else
				IVS_Delay = 0;
		}

		rtd_pr_smt_debug("_rk[vgip][pos][CH1] IHS_Delay:%x, IVS_Delay:%x\n",IHS_Delay, IVS_Delay);

		vgip_chn1_delay_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_DELAY_reg);
	//fix me frank@0916	vgip_chn1_delay_reg.ch1_ihs_dly = IHS_Delay;
		vgip_chn1_delay_reg.ch1_ivs_dly = IVS_Delay;
		rtd_outl(VGIP_VGIP_CHN1_DELAY_reg, vgip_chn1_delay_reg.regValue);
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else// if(_CHANNEL2 == GET_INPUT_CHANNEL(display))
	{
		//H position
		vgip_chn2_act_hsta_width_reg.regValue = rtd_inl(SUB_VGIP_VGIP_CHN2_ACT_HSTA_Width_reg);

		//V position
		vgip_chn2_act_vsta_length_reg.regValue = rtd_inl(SUB_VGIP_VGIP_CHN2_ACT_VSTA_Length_reg);

		switch(Scaler_InputSrcGetType(display))
		{
			case _SRC_VGA:
				IHS_Delay = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA + _H_POSITION_MOVE_TOLERANCE - vgip_chn2_act_hsta_width_reg.ch2_ih_act_sta;
				break;

			case _SRC_YPBPR:
				IHS_Delay = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA + (_H_POSITION_MOVE_TOLERANCE + ucHpositionTolerance) - vgip_chn2_act_hsta_width_reg.ch2_ih_act_sta;
				IHS_Delay += ucHpositionTolerance*(ucHpositionMoveFactor - 1);
				break;

			case _SRC_TV:
			case _SRC_CVBS:
			case _SRC_SV:
			case _SRC_HDMI:
			case _SRC_DVI:
			case _SRC_VO:
				IHS_Delay = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA + ucHpositionTolerance - vgip_chn2_act_hsta_width_reg.ch2_ih_act_sta;
				IHS_Delay += ucHpositionTolerance*(ucHpositionMoveFactor - 1);
#if 0
				//OSD middle value is (ucHpositionTolerance), beacuse shift value will plus 2, so need add ucHpositionTolerance again
				IHS_Delay += ucHpositionTolerance;
#endif //#if 0
				break;
		}

		IHS_Delay = (UINT16)(IHS_Delay + (shiftx * ucHpositionMoveFactor));

		if(_TRUE == (imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC))
		{
			if(_TRUE == imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
			{
				if((smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >> 1) >= (0 - shifty))
					IVS_Delay = (smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >> 1) + shifty;
				else
					IVS_Delay = 0;
			}
			else
			{
				if(smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >= (0 - shifty))
					IVS_Delay = smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() + shifty;
				else
					IVS_Delay = 0;
			}
		}
		else
		{
			IVS_Delay = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA + smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() - vgip_chn2_act_vsta_length_reg.ch2_iv_act_sta;

#if 0
			IVS_Delay =(unsigned short)(IVS_Delay - (shifty));
#else //#if 0
			//USER:LewisLee DATE:2011/01/14
			//to prevent IVS_Delay <0, picture abnormal
			if((shifty <= 0) || (IVS_Delay >= shifty))
				IVS_Delay = (unsigned short)((short)IVS_Delay - (shifty));
			else
				IVS_Delay = 0;
#endif //#if 0

		}

#if 0
		//OSD middle value is (ucVpositionTolerance>>1)
		if(IVS_Delay >= (ucVpositionTolerance>>1))
			IVS_Delay -= (ucVpositionTolerance>>1);
#endif //#if 0

		rtd_pr_smt_debug("[CH2] IHS_Delay:%x, IVS_Delay:%x\n",IHS_Delay, IVS_Delay);

		vgip_chn2_delay_reg.regValue = rtd_inl(SUB_VGIP_VGIP_CHN2_DELAY_reg);
	//fix me frank@0916 	vgip_chn2_delay_reg.ch2_ihs_dly = IHS_Delay;
		vgip_chn2_delay_reg.ch2_ivs_dly = IVS_Delay;
		rtd_outl(SUB_VGIP_VGIP_CHN2_DELAY_reg, vgip_chn2_delay_reg.regValue);
	}
#endif
}




void zoom_imd_smooth_decide_nr_path(unsigned char display)
{//for bootup,zoom
	if(imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_DOWN == 1)
	{  // go vuzd path
		imdSmoothInfo[display].IMD_SMOOTH_V_UZD_BUFFER_MODE = VUZD_BUFFER_VUZD_MODE;
	}else
	{//normal,uzu
		if(display == SLR_SUB_DISPLAY)
		{//sub,normal,uzu
			imdSmoothInfo[display].IMD_SMOOTH_V_UZD_BUFFER_MODE = VUZD_BUFFER_BYPASS_MODE;
		}
		else
		{//main,normal,uzu
			if(imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC)//WOSQRTK-6131 Patch
				imdSmoothInfo[display].IMD_SMOOTH_V_UZD_BUFFER_MODE = VUZD_BUFFER_FRAMESYNC_MODE;
			else
				imdSmoothInfo[display].IMD_SMOOTH_V_UZD_BUFFER_MODE = VUZD_BUFFER_BYPASS_MODE;
		}
	}
}



#if 1 //rika 20140924 marked //#include "tvscalercontrol/vip/ultrazoom.h"//kernel
/*
   In file included
   from ../../../Include/Platform_Lib/TVScalerControl/vip/peaking.h:8:0,
   from ../../../Include/Platform_Lib/TVScalerControl/scaler/scalerStruct.h:12,
   from ../../../Include/Platform_Lib/TVScalerControl/vip/di_ma.h:19,

*/
#else //enable this declaration //it's marked in ultrazoom.h //it's not in vipcommon.h
enum{
	SDFIR64_90_20 = 0,
	SDFIR64_70_20,
	SDFIR64_40_20,
	SDFIR64_35_20,
	SDFIR64_32_20,
	SDFIR64_50_25Hamm,
	SDFIR64_Blur,
	SDFIR64_Mid,
	SDFIR64_Sharp,
	SDFIR64_45_20Hamm,
	SDFIR64_30_20Hamm,
	SDFIR64_25_20Hamm,
	SDFIR64_25_15Hamm,
	SDFIR64_60_10Hamm,
	SDFIR64_2tap,
	SDFIR64_20_10Hamm,
	SDFIR64_20_10,
	SDFIR64_15_10Hamm,
	SDFIR64_MAXNUM
};
#endif

#if 0
static  signed short tScale_Down_Coef_SDFIR64_90_20[] = {
	64,  66,  70,  73,  81,  88,  97, 110,
	122, 138, 154, 170, 188, 206, 225, 243,
	263, 283, 302, 322, 339, 357, 374, 389,
	403, 415, 425, 434, 441, 446, 450, 454,
};

static  signed short tScale_Down_Coef_SDFIR64_70_20[] = {
	47,  49,  54,  59,  68,  77,  88, 102,
	113, 129, 146, 164, 183, 203, 223, 243,
	264, 286, 305, 326, 345, 364, 381, 397,
	412, 426, 437, 447, 456, 462, 466, 470,
};

static  signed short tScale_Down_Coef_SDFIR64_40_20[] = {
	13,  14,  20,  27,  36,  45,  58,  74,
	88, 106, 126, 146, 168, 191, 215, 238,
	262, 289, 312, 336, 359, 382, 403, 422,
	440, 457, 471, 483, 493, 501, 506, 511,
};

static  signed short tScale_Down_Coef_SDFIR64_35_20[] = {
	2,   4,  10,  17,  25,  36,  49,  65,
	80,  99, 119, 140, 163, 187, 212, 237,
	262, 289, 314, 339, 364, 387, 409, 430,
	449, 467, 482, 495, 505, 513, 519, 523,
};

static  signed short tScale_Down_Coef_SDFIR64_32_20[] = {
	-5,  -5,   1,   8,  18,  29,  42,  58,
	74,  93, 113, 136, 159, 184, 210, 235,
	261, 290, 316, 342, 367, 392, 415, 436,
	456, 474, 490, 503, 515, 523, 529, 533,
};

static  signed short tScale_Down_Coef_SDFIR64_50_25Hamm[] = {
	10,  16,  21,  22,  26,  31,  39,  48,
	58,  71,  86, 103, 122, 144, 169, 196,
	224, 254, 284, 317, 350, 382, 413, 444,
	474, 501, 525, 545, 563, 575, 585, 594,
};

static  signed short tScale_Down_Coef_Blur[] = {
	//2,   3,   5,   7,  10,  15,  20,  28,  38,  49,  64,  81, 101, 124, 150, 178,
	//209, 242, 277, 314, 351, 389, 426, 462, 496, 529, 556, 582, 602, 618, 629, 635,
	/*DFIR64gauss_Fc3_A0o8_Ext2*/
	213,  217,  221,  222,  224,  229,  232,  234,
	239,  242,  245,  249,  252,  255,  259,  262,
	265,  266,  267,  269,  271,  272,  273,  275,
	276,  277,  278,  280,  281,  281,  282,  284,
};

// [3D SBS] vertical scaling down 1.5~2x case (avoid vertical aliasing)
static  signed short tScale_Down_Coef_Mid[] = {
	//-2,  0,   1,   2,   5,   9,  15,  22,  32,  45,  60,  77,  98, 122, 149, 179,
	//211, 245, 281, 318, 356, 394, 431, 468, 502, 533, 561, 586, 606, 620, 630, 636,
	/*SDFIR64_defaultTab1*/
	131,  125,  124,  126,  130,  135,  142,  151,
	161,  172,  183,  195,  208,  221,  234,  245,
	260,  275,  290,  304,  318,  330,  341,  351,
	361,  369,  376,  381,  386,  389,  390,  388,
};

static  signed short tScale_Down_Coef_Sharp[] = {
	-2,   0,   1,   3,   6,  10,  15,  22,  32,  43,  58,  75,  95, 119, 145, 174,
	206, 240, 276, 314, 353, 391, 430, 467, 503, 536, 565, 590, 612, 628, 639, 646,
};

static  signed short tScale_Down_Coef_SDFIR64_45_20Hamm[] = {
	-1,   1,   5,   5,   8,  10,  13,  19,
	28,  38,  51,  66,  85, 107, 133, 162,
	194, 230, 266, 306, 346, 387, 429, 469,
	508, 544, 576, 604, 628, 646, 660, 669,
};

static  signed short tScale_Down_Coef_SDFIR64_30_20Hamm[] = {
	-1,  -1,   0,   1,   2,   4,   7,  12,
	19,  28,  40,  55,  74,  96, 122, 151,
	183, 221, 260, 301, 344, 388, 433, 476,
	517, 556, 592, 623, 648, 668, 682, 691,
};

static  signed short tScale_Down_Coef_SDFIR64_25_20Hamm[] = {
	-4,  -5,  -2,  -4,  -3,  -2,  -1,   3,
	10,  18,  29,  43,  62,  83, 110, 140,
	172, 212, 252, 296, 342, 389, 437, 483,
	528, 570, 608, 642, 670, 691, 707, 716,
};

static  signed short tScale_Down_Coef_SDFIR64_25_15Hamm[] = {
	-30, -24, -20, -19, -22, -25, -30, -31,
	-33, -32, -30, -23, -12,   6,  30,  60,
	96, 138, 186, 241, 303, 369, 439, 509,
	579, 647, 710, 766, 814, 852, 880, 898,
};

static  signed short StScale_Down_Coef_DFIR64_60_10Hamm[] = {
	-6,  -5,  -5,  -5,  -3,  -5,  -7,  -9,
	-16, -19, -22, -23, -21, -15,  -4,  13,
	40,  76, 122, 178, 242, 316, 396, 481,
	568, 654, 735, 808, 872, 922, 957, 977,
};


static  signed short tScale_Down_Coef_2tap[] = {
	0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,
	31,  95, 159, 223, 287, 351, 415, 479,
	543, 607, 671, 735, 799, 863, 927, 991,
};

static  signed short tScale_Down_Coef_SDFIR64_20_10Hamm[] = {
	-9,  -7,  -6,   0,   5,   5,   5,   2,
	-10, -22, -38, -57, -75, -90,-101,-103,
	-92, -65, -21,  40, 119, 217, 328, 451,
	581, 713, 840, 957,1059,1141,1197,1228,
};

static  signed short tScale_Down_Coef_SDFIR64_20_10[] = {
	60,  51,  39,  27,   7, -17, -43, -76,
	-105,-137,-164,-187,-202,-207,-201,-182,
	-146, -90, -20,  65, 166, 280, 404, 536,
	669, 800, 925,1038,1134,1212,1264,1292,
};

static  signed short tScale_Down_Coef_SDFIR64_15_10Hamm[] = {
	-36, -23, -10,   8,  16,  22,  27,  25,
	13,  -3, -25, -53, -84,-110,-133,-146,
	-147,-133, -98, -41,  43, 150, 275, 417,
	569, 725, 877,1018,1141,1242,1313,1353,
};

static short *tScaleDown_COEF_TAB[] =
{
	tScale_Down_Coef_SDFIR64_90_20, tScale_Down_Coef_SDFIR64_70_20, tScale_Down_Coef_SDFIR64_40_20,
	tScale_Down_Coef_SDFIR64_35_20, tScale_Down_Coef_SDFIR64_32_20, tScale_Down_Coef_SDFIR64_50_25Hamm,
	tScale_Down_Coef_Blur, tScale_Down_Coef_Mid, tScale_Down_Coef_Sharp,
	tScale_Down_Coef_SDFIR64_45_20Hamm, tScale_Down_Coef_SDFIR64_30_20Hamm, tScale_Down_Coef_SDFIR64_25_20Hamm,
	tScale_Down_Coef_SDFIR64_25_15Hamm, StScale_Down_Coef_DFIR64_60_10Hamm, tScale_Down_Coef_2tap,
	tScale_Down_Coef_SDFIR64_20_10Hamm, tScale_Down_Coef_SDFIR64_20_10, tScale_Down_Coef_SDFIR64_15_10Hamm
};

static unsigned char SD_H_Coeff_Sel[] = {SDFIR64_Blur, SDFIR64_Mid, SDFIR64_Sharp, SDFIR64_2tap}; // the size must match large table
#endif
#ifdef VPQ_COMPILER_ERROR_ENABLE_MERLIN6
static unsigned char SD_V_Coeff_Sel[] = {SDFIR64_Blur, SDFIR64_Mid, SDFIR64_Sharp, SDFIR64_2tap}; // the size must match large table
#endif



#if 0
void smooth_toggle_imd_smooth_memory_DMA420_config(unsigned char *bitNum)
{
	UINT32 width, lineSize_odd, lineSize_even;

	if(!bitNum){
		rtd_pr_smt_debug("ERR: NULL PTR@%s\n", __FUNCTION__);
		return;
	}

	// DI DMA 420 enable (saving bandwidth)
	if(bIpDma420Mode == _ENABLE){
		width =imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_DI_WID;
		//lineSize_even = (width * (*bitNum - DI_COLOR_BIT_NUM_C));
		lineSize_even = (width * (*bitNum - ((fw_scalerip_get_DI_chroma_10bits())?10:8)));//rika 20140624 10-bit flow

		if((lineSize_even%64)==0)
			lineSize_even /=64;
		else
			lineSize_even = (lineSize_even+64)/64;

		lineSize_odd = (width * *bitNum);
		if((lineSize_odd%64)==0)
			lineSize_odd /=64;
		else
			lineSize_odd = (lineSize_odd+64)/64;

		//rtd_pr_smt_debug("[DI] DMA 420 Enable, bit/line wid/even/odd[%d/%d/%d/%d]\n", *bitNum, width, lineSize_even, lineSize_odd);
		rtd_maskl(DI_IM_DI_DMA_VADDR,
				~(DI_DMA_420_EN_MASK| DI_DMA_LINESIZE_EVEN_MASK| DI_DMA_LINESIZE_ODD_MASK),
				_BIT30| (lineSize_even << DI_DMA_LINESIZE_EVEN_START_BIT)| lineSize_odd);
		// update bit number
		//	*bitNum -= 4;
	}
	else
		rtd_clearbits(DI_IM_DI_DMA_VADDR, _BIT30);

	return;
}

#endif
//	//rika 20140828 removed vip related functions



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

void zoom_imd_smooth_color_ultrazoom_config_scaling_up(unsigned char display)
{
	/*
	   unsigned char H8tap_en=1;
	   unsigned char hor8_table_sel=2;
	   unsigned char V6tap_en=0, v6_table_sel=0, v6_adap_en=0;
	   */

	// Load IC default table
	// H8tap_en=1; hor8_table_sel=1;
	// V6tap_en=1; v6_table_sel=1; v6_adap_en=0
	//scaleup_dm_uzu_v8ctrl_RBUS dm_uzu_V8CTRL_reg;
	//scaleup_dm_uzu_adaptv8ctrl_RBUS dm_uzu_AdaptV8CTRL_reg;
	scaleup_dm_uzu_ctrl_RBUS dm_uzu_Ctrl_REG;
	scaleup_dm_uzumain_h_partial_drop_RBUS h_partial_drop_reg;
	scaleup_dm_uzumain_v_partial_drop_RBUS v_partial_drop_reg;

	SMOOTH_TOGGLE_SIZE inSize, outSize;

	unsigned char V8tap_mode = 0;
	//unsigned char H8tap_en = 1;
	//unsigned char hor8_table_sel = 1;// if 2, need to init table //rika 20140716 fix 576i,480p,480i atv bad screen issue
	//unsigned char v6_table_sel = 0;
	//unsigned char V6tap_en = 0, v6_table_sel = 0, v6_adap_en = 0;
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
		dm_uzu_Ctrl_REG.v_zoom_en = imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_UP;//(GET_VSCALE_UP(info->display)!=0);
		dm_uzu_Ctrl_REG.h_zoom_en = imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_UP;//(GET_HSCALE_UP(info->display)!=0);

		if(dvrif_memory_compression_get_enable(display) == TRUE){
			dm_uzu_Ctrl_REG.in_fmt_conv = FALSE;
		}else{
			dm_uzu_Ctrl_REG.in_fmt_conv = !(imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC) && imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP;
		}

		dm_uzu_Ctrl_REG.video_comp_en = 0;
		dm_uzu_Ctrl_REG.bypassfornr = !imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_UP; // nr always available
		//dm_uzu_Ctrl_REG.bypassfornr = 1;//rika crop 1600x900  is normal when 1 => if V6tap_en ==0, it will be normal
		//	rtd_outl(SCALEUP_DM_UZU_CTRL_VADDR, dm_uzu_Ctrl_REG.regValue);  //rika 20140609 need review
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{	//rtd_pr_smt_debug("_rk UZU %d\n",__LINE__);
		scaleup_ds_uzu_ctrl_RBUS ds_uzu_Ctrl_REG;
		ds_uzu_Ctrl_REG.regValue = rtd_inl(SCALEUP_DS_UZU_Ctrl_reg);
		ds_uzu_Ctrl_REG.v_zoom_en = (imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_UP!=0);
		ds_uzu_Ctrl_REG.h_zoom_en = (imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_UP!=0);
		ds_uzu_Ctrl_REG.video_comp_en = 0;
		ds_uzu_Ctrl_REG.in_fmt_conv =(imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP ? 1 : 0 );

		if (imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE) {
			ds_uzu_Ctrl_REG.odd_inv = 1;/*fix sub di shaking when interlace*/
#ifdef CONFIG_I2RND_ENABLE
			//in i2rnd case, no need to do comp @Crixus 20160802
			if(Scaler_I2rnd_get_timing_enable())
				ds_uzu_Ctrl_REG.video_comp_en = 0;
			else
#endif
				ds_uzu_Ctrl_REG.video_comp_en = 1;

		} else {
			ds_uzu_Ctrl_REG.odd_inv = 0;
			ds_uzu_Ctrl_REG.video_comp_en = 0;
		}
		rtd_outl(SCALEUP_DS_UZU_Ctrl_reg, ds_uzu_Ctrl_REG.regValue);
	}
#endif//end of #ifdef CONFIG_DUAL_CHANNEL
	inSize.nLength = imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_LEN; //GET_MEMORY_DISP_LENGTH(info->display);
	inSize.nWidth = imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID; //GET_MEMORY_DISP_WIDTH(info->display);


	//outSize.nLength = zoom_disp_vsize; //info->DispLen;
	//outSize.nWidth = zoom_disp_hsize; //info->DispWid;

	outSize.nLength = imdSmoothInfo[display].IMD_SMOOTH_DispLen; //info->DispLen;
	outSize.nWidth = imdSmoothInfo[display].IMD_SMOOTH_DispWid; //info->DispWid;

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
				if(imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE)
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
			h_partial_drop_reg.hor_m_back_drop = 0;
			IoReg_Write32(SCALEUP_DM_UZUMAIN_H_PARTIAL_DROP_reg,h_partial_drop_reg.regValue);
			IoReg_Write32(SCALEUP_DM_UZUMAIN_V_PARTIAL_DROP_reg,v_partial_drop_reg.regValue);
		}
#endif


	//fix 3d

	//if (inSize.nWidth <= 720 && outSize.nWidth >= 1280)
	//	H8tap_en = 1;
	//else
		//H8tap_en = 1;
#if 1//pool test
	drop_v_len[display] = 0;
	//H8tap_en = 1;hor8_table_sel=2;
	//unsigned char V8tap_mode = 0;
	if (imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID<=1920) {
		V8tap_mode = 1; //v6_table_sel = 3;
	}
	if (display ==SLR_MAIN_DISPLAY)
	{
		scaleup_dm_uzu_v8ctrl_RBUS dm_uzu_V8CTRL_reg;
		dm_uzu_Ctrl_REG.hor_mode_sel = 2; //13.07.26 always hor_mode_sel set 4tap due to 8tap have issue
		dm_uzu_Ctrl_REG.hor12_table_sel = 0; //0:table-1, 1:table-2
		dm_uzu_Ctrl_REG.h_c_consttab_sel = 2;  // H_CONST_TABLE_SEL = 2 (blur)
		dm_uzu_Ctrl_REG.v_c_consttab_sel = 2;  // V_CONST_TABLE_SEL = 2 (blur)
		if(dm_uzu_Ctrl_REG.hor_mode_sel==2)
			smooth_toggle_drvif_color_ultrazoom_H12tapDeRing(1,3);
		rtd_outl(SCALEUP_DM_UZU_Ctrl_reg, dm_uzu_Ctrl_REG.regValue);

		#if 0 //remove from melrin3
		//scaleup_dm_uzu_adaptv8ctrl_RBUS dm_uzu_AdaptV8CTRL_reg;
		//scaleup_dm_uzumain_adapt12tap_RBUS dm_uzuMAIN_Adapt12tap;
		//dm_uzuMAIN_Adapt12tap.regValue = rtd_inl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr);
		dm_uzuMAIN_Adapt12tap.adapt12_upbnd = 0x10;
		dm_uzuMAIN_Adapt12tap.adapt12_sec3 = 0x00;
		dm_uzuMAIN_Adapt12tap.adapt12_sec2 = 0x00;
		dm_uzuMAIN_Adapt12tap.adapt12_sec1	= 0x00;
		rtd_outl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr, dm_uzuMAIN_Adapt12tap.regValue);

		dm_uzu_AdaptV8CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_AdaptV8CTRL_reg);
		dm_uzu_AdaptV8CTRL_reg.adaptv8_upbnd = 75;
		dm_uzu_AdaptV8CTRL_reg.adaptv8_sec3 = 56;
		dm_uzu_AdaptV8CTRL_reg.adaptv8_sec2 = 37;
		dm_uzu_AdaptV8CTRL_reg.adaptv8_sec1 = 18;
		rtd_outl(SCALEUP_DM_UZU_AdaptV8CTRL_reg, dm_uzu_AdaptV8CTRL_reg.regValue);
		#endif

		dm_uzu_V8CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_V8CTRL_reg);
			//---marked by Elsie---
			//dm_uzu_V8CTRL_reg.v6tap_en = V6tap_en;
			//dm_uzu_V8CTRL_reg.v6tap_adapt_en= v6_adap_en;
		dm_uzu_V8CTRL_reg.ver_mode_sel = V8tap_mode;
		dm_uzu_V8CTRL_reg.v8tap_table_sel = 0;
		rtd_outl(SCALEUP_DM_UZU_V8CTRL_reg, dm_uzu_V8CTRL_reg.regValue);
	}
#endif
#if 0
	if (display == SLR_MAIN_DISPLAY)
	{
		scaleup_dm_uzumain_adapt8tap_RBUS dm_uzuMAIN_Adapt8tap;
		dm_uzuMAIN_Adapt8tap.regValue = rtd_inl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr);
		dm_uzu_Ctrl_REG.hor_mode_sel = 4; //13.07.26 always hor_mode_sel set 4tap due to 8tap have issue
		dm_uzu_Ctrl_REG.hor10_table_sel = hor8_table_sel; //0:table-1, 1:table-2

		if(dm_uzu_Ctrl_REG.hor_mode_sel==4) smooth_toggle_drvif_color_ultrazoom_H12tapDeRing(1,3);

		dm_uzuMAIN_Adapt8tap.adapt8_upbnd = 0x10;
		dm_uzuMAIN_Adapt8tap.adapt8_sec3 = 0x00;
		dm_uzuMAIN_Adapt8tap.adapt8_sec2 = 0x00;
		dm_uzuMAIN_Adapt8tap.adapt8_sec1	= 0x00;
		rtd_outl(SCALEUP_DM_UZU_CTRL_VADDR, dm_uzu_Ctrl_REG.regValue);
		rtd_outl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr, dm_uzuMAIN_Adapt8tap.regValue);

		dm_uzu_V6CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_V8CTRL_VADDR);
		dm_uzu_AdaptV6CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_ADAPTV8CTRL_VADDR);
		//dm_uzu_V6CTRL_reg.v6tap_en = V6tap_en;
		//dm_uzu_V6CTRL_reg.v6tap_adapt_en= v6_adap_en;
		dm_uzu_V6CTRL_reg.v8tap_table_sel = v6_table_sel;
		dm_uzu_AdaptV6CTRL_reg.adaptv8_upbnd = 75;
		dm_uzu_AdaptV6CTRL_reg.adaptv8_sec3 = 56;
		dm_uzu_AdaptV6CTRL_reg.adaptv8_sec2 = 37;
		dm_uzu_AdaptV6CTRL_reg.adaptv8_sec1 = 18;
		rtd_outl(SCALEUP_DM_UZU_V8CTRL_VADDR, dm_uzu_V6CTRL_reg.regValue);
		rtd_outl(SCALEUP_DM_UZU_ADAPTV8CTRL_VADDR, dm_uzu_AdaptV6CTRL_reg.regValue);
	}
#endif
	zoom_imd_smooth_color_ultrazoom_setscaleup(display, &inSize, &outSize, panorama);	// Display scaling up control, linear or non-linear either
}

void magnifier_color_ultrazoom_config_scaling_up(unsigned char display)
{
	/*
	   unsigned char H8tap_en=1;
	   unsigned char hor8_table_sel=2;
	   unsigned char V6tap_en=0, v6_table_sel=0, v6_adap_en=0;
	   */

	// Load IC default table
	// H8tap_en=1; hor8_table_sel=1;
	// V6tap_en=1; v6_table_sel=1; v6_adap_en=0
//	scaleup_dm_uzu_v8ctrl_RBUS dm_uzu_V6CTRL_reg;
//	scaleup_dm_uzu_adaptv8ctrl_RBUS dm_uzu_AdaptV6CTRL_reg;
	scaleup_dm_uzu_ctrl_RBUS dm_uzu_Ctrl_REG;
	scaleup_dm_uzumain_h_partial_drop_RBUS h_partial_drop_reg;
	scaleup_dm_uzumain_v_partial_drop_RBUS v_partial_drop_reg;

	SMOOTH_TOGGLE_SIZE inSize, outSize;

	unsigned char V8tap_mode = 0;
	//unsigned char H8tap_en = 1;
	//unsigned char hor8_table_sel = 1;// if 2, need to init table //rika 20140716 fix 576i,480p,480i atv bad screen issue
	//unsigned char v6_table_sel = 0;
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
#ifdef CONFIG_I2RND_ENABLE
			//in i2rnd case, no need to do comp @Crixus 20160802
			if(Scaler_I2rnd_get_timing_enable())
				ds_uzu_Ctrl_REG.video_comp_en = 0;
			else
#endif
				ds_uzu_Ctrl_REG.video_comp_en = 1;

			//ds_uzu_Ctrl_REG.video_comp_en = 1;
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

	drop_v_len[display] = 0;
	//H8tap_en = 1;
	//hor8_table_sel=2;
	//unsigned char V8tap_mode = 0;
	if (Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID) <=1920) {
		V8tap_mode = 1;
		//v6_table_sel = 3;
	}

	if (display == SLR_MAIN_DISPLAY)
	{
		scaleup_dm_uzu_v8ctrl_RBUS dm_uzu_V8CTRL_reg;
		dm_uzu_Ctrl_REG.hor_mode_sel = 2; //13.07.26 always hor_mode_sel set 4tap due to 8tap have issue
		//dm_uzu_Ctrl_REG.hor10_table_sel = hor8_table_sel; //20151118 roger mark, control by PQ table	//0:table-1, 1:table-2

		dm_uzu_Ctrl_REG.hor12_table_sel = 0; //0:table-1, 1:table-2
		dm_uzu_Ctrl_REG.h_c_consttab_sel = 2;  // H_CONST_TABLE_SEL = 2 (blur)
		dm_uzu_Ctrl_REG.v_c_consttab_sel = 2;  // V_CONST_TABLE_SEL = 2 (blur)

		if(dm_uzu_Ctrl_REG.hor_mode_sel==2)
			smooth_toggle_drvif_color_ultrazoom_H12tapDeRing(1,3);

		rtd_outl(SCALEUP_DM_UZU_Ctrl_reg, dm_uzu_Ctrl_REG.regValue);


		dm_uzu_V8CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_V8CTRL_reg);
		//---marked by Elsie---
		//dm_uzu_V8CTRL_reg.v6tap_en = V6tap_en;
		//dm_uzu_V8CTRL_reg.v6tap_adapt_en= v6_adap_en;
		dm_uzu_V8CTRL_reg.ver_mode_sel = V8tap_mode;
		dm_uzu_V8CTRL_reg.v8tap_table_sel = 0;
		rtd_outl(SCALEUP_DM_UZU_V8CTRL_reg, dm_uzu_V8CTRL_reg.regValue);

		#if 0 //remove from merlin3
		scaleup_dm_uzu_adaptv8ctrl_RBUS dm_uzu_AdaptV8CTRL_reg;
		scaleup_dm_uzumain_adapt12tap_RBUS dm_uzuMAIN_Adapt12tap;
		dm_uzuMAIN_Adapt12tap.regValue = rtd_inl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr);
		dm_uzuMAIN_Adapt12tap.adapt12_upbnd = 0x10;
		dm_uzuMAIN_Adapt12tap.adapt12_sec3 = 0x00;
		dm_uzuMAIN_Adapt12tap.adapt12_sec2 = 0x00;
		dm_uzuMAIN_Adapt12tap.adapt12_sec1	= 0x00;
		rtd_outl(SCALEUP_DM_UZUMAIN_Adapt12tap_reg_addr, dm_uzuMAIN_Adapt12tap.regValue);
		dm_uzu_AdaptV8CTRL_reg.regValue = rtd_inl(SCALEUP_DM_UZU_AdaptV8CTRL_reg);
		dm_uzu_AdaptV8CTRL_reg.adaptv8_upbnd = 75;
		dm_uzu_AdaptV8CTRL_reg.adaptv8_sec3 = 56;
		dm_uzu_AdaptV8CTRL_reg.adaptv8_sec2 = 37;
		dm_uzu_AdaptV8CTRL_reg.adaptv8_sec1 = 18;
		rtd_outl(SCALEUP_DM_UZU_AdaptV8CTRL_reg, dm_uzu_AdaptV8CTRL_reg.regValue);
		#endif
	}

	zoom_imd_smooth_color_ultrazoom_setscaleup(display, &inSize, &outSize, panorama);	// Display scaling up control, linear or non-linear either
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
#ifdef CONFIG_I2RND_ENABLE
			//in i2rnd case, no need to do comp @Crixus 20160802
			if(Scaler_I2rnd_get_timing_enable())
				ds_uzu_Ctrl_REG.video_comp_en = 0;
			else
#endif
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

unsigned int smooth_get_capture_size(unsigned char display, eMemCapAccessType AccessType)
{
	unsigned int TotalSize;
	unsigned char ddrbit;
	unsigned int capLen = imdSmoothInfo[display].IMD_SMOOTH_CapLen;
	unsigned int capWid = imdSmoothInfo[display].IMD_SMOOTH_CapWid;
	unsigned char channel =display; //main

	ddrbit =  (channel ? 0 : (imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT) << 1 ) | ( !imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP);
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//TotalSize = memory_get_line_size(capWid, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
		TotalSize = nonlibdma_memory_get_line_size(capWid, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
	} else { //for pass compile
		TotalSize = 0;
	}
	TotalSize = drvif_memory_get_data_align(TotalSize, 0x04);	// it must be times of 4

	if (AccessType == MEMCAPTYPE_FRAME)
		TotalSize = TotalSize * capLen;	// multiple total_lines if frame-access
	//rtd_pr_smt_debug("[Memory]:TotalSize=0x%x,capWid=0x%x,10Bit=%d,422=%d\n", TotalSize,capWid,(imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT),imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP);
	return TotalSize;
}

unsigned int smooth_get_capture_size_compression(unsigned char display, eMemCapAccessType AccessType, unsigned char compression_bits)
{
	unsigned int TotalSize;
	//unsigned char ddrbit;
	unsigned int capLen = imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_CapLen;
	unsigned int capWid = imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_CapWid;
	//remove  unsigned char channel = SLR_MAIN_DISPLAY;  to prevent DEADCODE coverity issue 120159  rika 20151217

	if((capWid % 32) != 0)
		capWid = capWid + (32 - (capWid % 32));

	//ddrbit =  (display ? 0 : (imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_10BIT) << 1 ) | ( !imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_422CAP);
	TotalSize = drvif_memory_get_data_align((capWid * compression_bits), 64);// (pixels of per line * bits / 64), unit: 64bits
	TotalSize = (unsigned int)SHR(TotalSize, 6);
	//TotalSize = drvif_memory_get_data_align(TotalSize, 0x04);	// it must be times of 4

	if (AccessType == MEMCAPTYPE_FRAME)
		TotalSize = TotalSize * capLen;	// multiple total_lines if frame-access
	return TotalSize;
}



unsigned int zoom_memory_get_capture_max_size(unsigned char display, eMemCapAccessType AccessType)
{
	unsigned int TotalSize;
	unsigned char ddrbit;
	unsigned char dispInterlace = imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE;

	ddrbit = ((imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT)<< 1)|(!imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP);

	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//TotalSize = memory_get_line_size(((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE) > DISP_WID) ? DISP_WID:(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE), (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
		TotalSize = nonlibdma_memory_get_line_size(((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE) > DISP_WID) ? DISP_WID:(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE), (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
	} else { //for pass compile
		TotalSize = 0;
	}

	// it must be times of 4
	TotalSize = drvif_memory_get_data_align(TotalSize, 0x04);

	// multiple total_lines if frame-access
	if (AccessType == MEMCAPTYPE_FRAME) {
		if((imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE) > DISP_LEN)
			TotalSize *= DISP_LEN;
		else
			TotalSize *= (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE);

		if(dispInterlace)
			TotalSize = TotalSize * 2;
	}

	return TotalSize;
}

unsigned int zoom_memory_get_display_size(unsigned char display)
{
	unsigned int TotalSize;
	unsigned char ddrbit;

	ddrbit =  ((imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT) << 1)  | (!(imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP));
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//TotalSize = memory_get_line_size(imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
		TotalSize = nonlibdma_memory_get_line_size(imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
	} else { //for pass compile
		TotalSize = 0;
	}
	//	rtd_pr_smt_debug("[Memory display]:TotalSize=%x\n", TotalSize);
	return TotalSize;
}
UINT32 smooth_toggle_MemGetDDRLineWidth1(UINT8 display,UINT32 width)
{
	UINT8 ddrbit;
	UINT32 TotalSize;

	ddrbit =  (display ? 0 : ((imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT) << 1 ))  | ( !(imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP));
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//TotalSize = memory_get_line_size(width, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
		TotalSize = nonlibdma_memory_get_line_size(width, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
	} else { //for pass compile
		TotalSize = 0;
	}
	TotalSize = drvif_memory_get_data_align(TotalSize, 0x04);	// it must be times of 4
	return TotalSize;

}

void smooth_toggle_MemGetBlockShiftAddr(UINT8 display,UINT16 HOffset, UINT16 VOffset,UINT32 * StartAddr, UINT16 *DropAddr)
{
	UINT32 TotalSize;
	//	UINT8 ddrbit;
	//	UINT8 Q = 0;
	UINT8 ddrbit;
	UINT16 K, L,S;
	UINT16 m,n;
	UINT8 FIFOUnit = 64;
	//	UINT16 Shift_FIFOUnit;
	UINT16 hFIFOUnit;
	UINT8 InputFormat;
	UINT8 BitNum;

	TotalSize = smooth_toggle_MemGetDDRLineWidth1(display,imdSmoothInfo[display].IMD_SMOOTH_CapWid);
	ddrbit =  (display ? 0 : ((imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT) << 1 ))  | ( !(imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP));
	InputFormat = (ddrbit & _BIT0);
	BitNum = (ddrbit & _BIT1) >> 1;

	if(InputFormat){
		if(BitNum) {		// 4:4:4 format     10bit
			K = 1920;
			L = (HOffset * 30) / K;
			S = (HOffset * 30 - L * K) / 30;

			m = HOffset * 30 / FIFOUnit;
			n = HOffset * 30 - m * FIFOUnit;

			// rtd_pr_smt_info("\n 444_10 bits !");
		} else {			// 4:4:4 format     8bit
			K = 384;
			L = HOffset * 24 / K;
			S = (HOffset * 24 - L * K) / 24; //pixel

			m = HOffset * 24 / FIFOUnit;
			n = HOffset * 24 - m * FIFOUnit;


		}
	}else{
		if(BitNum) {		// 4:2:2 format     10bit
			K = 640;
			L = HOffset * 20 / K;
			S = (HOffset * 20 - L * K) / 20;

			m = HOffset * 20 / FIFOUnit;
			n = HOffset * 20 - m * FIFOUnit;

		} else {			// 4:2:2 format     8bit
			K = 128;
			L = HOffset * 16 / K;
			S = (HOffset * 16 - L * K) / 16;

			m = HOffset * 16 / FIFOUnit;
			n = HOffset * 16 - m * FIFOUnit;
		}
	}

	if (n != 0)
		m = m + 1;


	hFIFOUnit =  (L * K) / FIFOUnit;

	//	Shift_FIFOUnit = m - hFIFOUnit;

	TotalSize = TotalSize * VOffset + hFIFOUnit;
	TotalSize = SHL(TotalSize, 3); // unit conversion from 64bits to 8bits

	*StartAddr = TotalSize;
	*DropAddr = S;

	//	return Shift_FIFOUnit;

}
UINT32 smooth_toggle_memory_get_disp_line_size1(UINT8 display,UINT32 width)
{
	UINT32 TotalSize;
	UINT8 ddrbit;

	ddrbit =  (display ? 0 : ((imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT) << 1))  | ( !(imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP));
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//TotalSize = memory_get_line_size(width, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
		TotalSize = nonlibdma_memory_get_line_size(width, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
	} else { //for pass compile
		TotalSize = 0;
	}
	return TotalSize;
}

unsigned int imd_smooth_memory_cal_vflip_block_addr(unsigned int address, unsigned int linesize , unsigned int linenum)
{//From static unsigned int memory_cal_vflip_block_addr(unsigned int address, unsigned int linesize , unsigned int linenum)
	return ((address >> 3) + (linesize * (linenum - 1)) )<< 3;
}



///void imd_smooth_scalerdisplay_set_timing(void)


static UINT8 AP_HposValue = 5;
static UINT8 AP_VposValue = 5;
void smooth_toggle_Scaler_SetHPosition(unsigned char value)
{
	if(_ENABLE == smooth_toggle_Scaler_Get_Enable_Adjust_Position_Src())
	{
		value = (smooth_toggle_Scaler_Get_Adjust_H_Position_Move_Tolerance()>>1) + 0x80 - value;

		if(value > smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_H_MAX))
			value = smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_H_MAX);

		if(value < smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_H_MIN))
			value = smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_H_MIN);
		//	rtd_pr_smt_info("\033[1;34m[%s]:%d, (%s):value:%d\033[m\n",__FILE__, __LINE__,__FUNCTION__,value );
		smooth_toggle_SET_H_POSITION(value);

		smooth_toggle_drvif_scaler_position_setting(0, _FALSE, smooth_toggle_GET_H_POSITION() - 0x80, smooth_toggle_GET_V_POSITION() - 0x80);
	}
}

void smooth_toggle_Scaler_SetVPositionValue(unsigned char ucValue, unsigned char Apply)
{//5,true@smooth_toggle_Scaler_SetPosition_Initial
	unsigned short value = 0, osd_moddle_value = 0;
	if(_ENABLE == smooth_toggle_Scaler_Get_Enable_Adjust_Position_Src()){


		osd_moddle_value = (smooth_toggle_Scaler_Get_Adjust_V_Position_Move_Tolerance()>>1);//5

		if(osd_moddle_value > 0){
			if(AP_VposValue >= osd_moddle_value)
			{
				value = (AP_VposValue - osd_moddle_value)*(smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MAX) -0x80);
				value = 0x80 + (value/(osd_moddle_value<<1));
			}
			else
			{
				value = (osd_moddle_value - AP_VposValue)*(0x80 - smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MIN));
				value = 0x80 - (value/(osd_moddle_value<<1));
			}

			smooth_toggle_SET_V_POSITION((UINT8)value);
		}


	}
}
static char 				VPositionTemp = 0;//VPOSITION
//static char 				HPositionTemp = 0;//HPOSITION

unsigned char smooth_toggle_Scaler_GetVPosition(unsigned char display)
{
	//	unsigned short temp2;

	UINT16 temp2 = 0, osd_moddle_value = 0;


	if(_ENABLE == smooth_toggle_Scaler_Get_Enable_Adjust_Position_Src())
	{


		VPositionTemp = abs(smooth_toggle_GET_V_POSITION() -0x80);
		osd_moddle_value = (smooth_toggle_Scaler_Get_Adjust_V_Position_Move_Tolerance()>>1);

		if(_TRUE == imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC)
		{
			if(_TRUE == imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
				temp2 = VPositionTemp*osd_moddle_value;
			else// if(_FALSE == Scaler_DispGetStatus(display, SLR_DISP_THRIP))
				temp2 = VPositionTemp*(osd_moddle_value<<1);

			if (smooth_toggle_GET_V_POSITION()>= 0x80)
			{
				VPositionTemp = (temp2/(smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MAX) -0x80)) + osd_moddle_value;
			}
			else
			{
				VPositionTemp = osd_moddle_value - (temp2/(0x80 - smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MIN)));
			}
		}
		else// if(_FALSE == Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,  SLR_INPUT_FRAMESYNC))
		{
			if (smooth_toggle_GET_V_POSITION()>= 0x80)
			{
				temp2 = VPositionTemp*(osd_moddle_value<<1);
				VPositionTemp = (temp2/(smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MAX) -0x80)) + osd_moddle_value;
			}
			else
			{
				temp2 = VPositionTemp*(osd_moddle_value<<1);
				VPositionTemp = osd_moddle_value - (temp2/(0x80 - smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MIN)));
			}
		}

	}
	else
	{
		VPositionTemp = smooth_toggle_GET_V_POSITION() -0x80;
		VPositionTemp += 5;
	}

	return VPositionTemp;

}

void smooth_toggle_ScalerSetVPosition(unsigned char display)
{

		vgip_vgip_chn1_delay_RBUS vgip_chn1_delay_reg;

		//	unsigned short channel =(SCALER_DISP_CHANNEL)display;
		//	unsigned short index = Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_DATA_ARRAY_IDX);
		//	unsigned char ipvMin = smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MIN);
		//	unsigned char ipvMax = smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MAX);
		unsigned char usIVSDelay=0;
		INT8 shifty = 0;
		//	vgip_chn1_act_vsta_length_RBUS vgip_chn1_act_vsta_length_reg;
		//	UINT8 ucChangeFramesyncStatusFlag = _FALSE;

		//	rtd_pr_smt_debug("[debug3]%d %d %d\n",GET_V_POSITION(index),smooth_toggle_Scaler_DispGetIpMargin(SLR_IPMARGIN_V_MIN),smooth_toggle_(SLR_IPMARGIN_V_MAX));

#if 0//disablepart //to fix compile error //below is disabled in sirius
#ifdef ENABLE_ADJUST_POSITION_DEPEND_ON_OSD_VALUE
			if (stScreenModeCurrInfo[index].V_Position < 0x80 - _V_POSITION_MOVE_TOLERANCE)
				stScreenModeCurrInfo[index].V_Position = 0x80 - _V_POSITION_MOVE_TOLERANCE;

			if(stScreenModeCurrInfo[index].V_Position > 0x80 + _V_POSITION_MOVE_TOLERANCE)
				stScreenModeCurrInfo[index].V_Position = 0x80 + _V_POSITION_MOVE_TOLERANCE;



			if ((smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay()+0x80) < ipvMax)
				ipvMax = (smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay()+0x80);

			if(ipvMin != 0 && ipvMax != 0)
			{
				if(stScreenModeCurrInfo[index].V_Position < ipvMin)
					stScreenModeCurrInfo[index].V_Position = ipvMin;

				if(stScreenModeCurrInfo[index].V_Position > ipvMax)
					stScreenModeCurrInfo[index].V_Position = ipvMax;
			}
			shifty = GET_V_POSITION(index) - 0x80;

		/* MOD_END */
#else //#ifdef ENABLE_ADJUST_POSITION_DEPEND_ON_OSD_VALUE
		if(ipvMin != 0 && ipvMax != 0)
		{
			if(smooth_toggle_GET_V_POSITION() < ipvMin)
				smooth_toggle_SET_V_POSITION(ipvMin);

			if(smooth_toggle_GET_V_POSITION() > ipvMax)
				smooth_toggle_SET_V_POSITION(ipvMax);
		}

		shifty = smooth_toggle_GET_V_POSITION() - 0x80;
#endif //#ifdef ENABLE_ADJUST_POSITION_DEPEND_ON_OSD_VALUE
#endif //end of disablepart

		//USER:LewisLee DATE:2013/06/06
		//In Vflip mode, VO path will invert by VODMA
		if (imdSmoothInfo[display].IMD_SMOOTH_VFLIP)
		{
			shifty = 0 - shifty;
		}

		if(_TRUE == (imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC))
		{
			if(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA==VGIP_IVStart)
			{
				usIVSDelay =0;
			}
			else
			{
				if(imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
				{
					if((smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >> 1) >= shifty)
						usIVSDelay = (smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >> 1) - shifty;
					else
						usIVSDelay = 0;
				}
				else
				{
					if(smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() >= shifty)
						usIVSDelay = smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() - shifty;
					else
						usIVSDelay = 0;
				}
			}
		}
		else
		{
			//		vgip_chn1_act_vsta_length_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg);

			usIVSDelay = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA+ smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay() - VGIP_IVStart;

			//		rtd_pr_smt_debug("@@@@@@@@%d %d %d %d\n",Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA),smooth_toggle_Scaler_DispGetMainChannelIVS2DVSDelay(), vgip_chn1_act_vsta_length_reg.ch1_iv_act_sta,usIVSDelay);

			//USER:LewisLee DATE:2011/01/14
			//to prevent IVS_Delay <0, picture abnormal
			#if 0 //disablepart
			//coverity issue 120160  no shifty now, should not use shifty<=0
			if((shifty <= 0) || (usIVSDelay >= shifty))
			{
				usIVSDelay = (unsigned short)((short)usIVSDelay - (shifty));
			}
			else
			{
				usIVSDelay = 0;
			}
			#else
			//rika 20151217
			//remove shifty<=0 due to coverity issue 120160 DEADCODE
			//shifty must be 0, so do not use shifty<=0
			if((usIVSDelay >= shifty))
			{
				usIVSDelay = (unsigned short)((short)usIVSDelay - (shifty));
			}
			else
			{
				usIVSDelay = 0;
			}
			#endif
		}

		//	drv_scaler_adjust_ivs_delay(display, usIVSDelay/*-_PROGRAM_VDELAY*/);

		vgip_chn1_delay_reg.regValue = rtd_inl(VGIP_VGIP_CHN1_DELAY_reg);
		vgip_chn1_delay_reg.ch1_ivs_dly = usIVSDelay /*+_PROGRAM_VDELAY */;
		rtd_outl(VGIP_VGIP_CHN1_DELAY_reg, vgip_chn1_delay_reg.regValue);

	}

void smooth_toggle_Scaler_SetVPosition(unsigned char value,unsigned char display)
{
	unsigned char temp;
//	unsigned short disp = 0;

	//rtd_pr_smt_info("\033[1;34m[%s]:%d, (%s):value:%d\033[m\n",__FILE__, __LINE__,__FUNCTION__,value );
	if(_ENABLE == smooth_toggle_Scaler_Get_Enable_Adjust_Position_Src())
	{

		temp = smooth_toggle_GET_V_POSITION();
		//		rtd_pr_smt_debug("[Scaler_SetVPosition debug] %d %d %d\n",value,temp,VPositionTemp);
		temp = temp + (value - VPositionTemp);

		//		rtd_pr_smt_debug("temp=%d\n",temp);
		smooth_toggle_SET_V_POSITION(temp);
		//		rtd_pr_smt_debug("pos=%d %d\n",GET_V_POSITION(GET_USER_INPUT_SRC()),GET_USER_INPUT_SRC());
		smooth_toggle_ScalerSetVPosition(display);
	}
	else
	{

		temp = smooth_toggle_GET_V_POSITION();
		temp = temp + (value - VPositionTemp);

		if (imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC)//yymodify 07.12.25
			return;
		else
		{
			//rtd_pr_smt_info("oliver 7+smooth_toggle_scaler_set_vgip_capture_shift  sift_y=%d\n", value - 5);
			smooth_toggle_scaler_set_vgip_capture_shift(display, smooth_toggle_GET_H_POSITION()-0x80-5, value - 5);
		}

		smooth_toggle_SET_V_POSITION(temp);
	}
}

void smooth_toggle_Scaler_SetPosition_Initial(unsigned char display)
{

	if(ratio_type == SLR_RATIO_CUSTOM)
		return; // ration customer not do this process.

	if(_DISABLE == smooth_toggle_Scaler_Get_Enable_Adjust_Position_Src())
		return;


	AP_HposValue = (smooth_toggle_Scaler_Get_Adjust_H_Position_Move_Tolerance()>>1);//5,0
	AP_VposValue = (smooth_toggle_Scaler_Get_Adjust_V_Position_Move_Tolerance()>>1);//5,0

	//	Scaler_SetHPositionValue(AP_HposValue, _TRUE);

	smooth_toggle_Scaler_SetHPosition(AP_HposValue);

	smooth_toggle_Scaler_SetVPositionValue(AP_VposValue, _TRUE);
	smooth_toggle_Scaler_GetVPosition(display); //for VPositionTemp

	smooth_toggle_Scaler_SetVPosition(AP_VposValue,display);


	//rtd_pr_smt_debug("_rk %s %d:%d,%d\n",__FUNCTION__,__LINE__,smooth_toggle_GET_H_POSITION() - 0x80,smooth_toggle_GET_V_POSITION() - 0x80);
}

static StructSrcRect inputregion_pre[MAX_DISP_CHANNEL_NUM];
static StructSrcRect outputregion_pre[MAX_DISP_CHANNEL_NUM];
static StructSrcRect inputTiming_pre[MAX_DISP_CHANNEL_NUM];

void Set_Val_inputregion_cur(unsigned char display, StructSrcRect region)
{
	apply_crop_hpos[display] = region.srcx;
	apply_crop_vpos[display] = region.srcy;
	apply_crop_hsize[display] = region.src_wid;
	apply_crop_vsize[display] = region.src_height;
}

void Set_Val_outputregion_cur(unsigned char display, StructSrcRect region)
{
	apply_disp_hpos[display] = region.srcx;
	apply_disp_vpos[display] = region.srcy;
	apply_disp_hsize[display] = region.src_wid;
	apply_disp_vsize[display] = region.src_height;
}

void Set_Val_inputTiming_cur(unsigned char display, StructSrcRect region)
{
	apply_timing_hpos[display] = region.srcx;
	apply_timing_vpos[display] = region.srcy;
	apply_timing_hsize[display] = region.src_wid;
	apply_timing_vsize[display] = region.src_height;
}

void Set_Val_inputregion_pre(unsigned char display, StructSrcRect region)
{
	inputregion_pre[display] = region;
}

StructSrcRect Get_Val_inputregion_pre(unsigned char display)
{
	return inputregion_pre[display];
}

void Set_Val_outputregion_pre(unsigned char display, StructSrcRect region)
{
	outputregion_pre[display] = region;
}

StructSrcRect Get_Val_outputregion_pre(unsigned char display)
{
	return outputregion_pre[display];
}

void Set_Val_inputTiming_pre(unsigned char display, StructSrcRect region)
{
	inputTiming_pre[display] = region;
}

unsigned char Get_Val_check_only_xy_shift_change_to_videofw(unsigned char display)
{
	return check_only_xy_shift_change_to_videofw[display];
}

void Set_Val_check_only_xy_shift_change_to_videofw(unsigned char display, unsigned char val)
{
	check_only_xy_shift_change_to_videofw[display] = val;
}

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



void smooth_toggle_print_smooth_info(unsigned char display)
{
///return;

	rtd_pr_smt_debug("\33[35m\n===============================\33[m\n");
	rtd_pr_smt_debug("\33[35m\n_rk[zoom]H_ACT_WID_PRE=%d, V_ACT_LEN_PRE=%d, H_ACT_STA_PRE=%d, V_ACT_STA_PRE=%d,H_TOTAL=%d, V_TOTAL=%d\33[m\n",
			imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE,
			imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE,
			imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE,
			imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA_PRE,
			imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL,
			imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL);

	rtd_pr_smt_debug("\33[35m_rk[zoom]H_ACT_WID=%d, H_ACT_STA=%d, V_ACT_LEN=%d, V_ACT_STA=%d\33[m\n",
			imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID,
			imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA,
			imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN,
			imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA);

	rtd_pr_smt_debug("\33[35m_rk[zoom]IMD_SMOOTH_DI_WID=%d, IMD_SMOOTH_DI_LEN=%d \33[m\n",
			imdSmoothInfo[display].IMD_SMOOTH_DI_WID,
			imdSmoothInfo[display].IMD_SMOOTH_DI_LEN);

	rtd_pr_smt_debug("\33[35m_rk[zoom]SDNRH_ACT_WID=%d, SDNRH_ACT_STA=%d, SDNRV_ACT_LEN=%d, SDNRV_ACT_STA=%d\33[m\n",
			imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_WID,
			imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_STA,
			imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_LEN,
			imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_STA);

	rtd_pr_smt_debug("\33[35m_rk[zoom]CapWid=%d, CapLen=%d \33[m\n",
			imdSmoothInfo[display].IMD_SMOOTH_CapWid,
			imdSmoothInfo[display].IMD_SMOOTH_CapLen);

	rtd_pr_smt_debug("\33[35m_rk[zoom]MEM_ACT_HSTA=%d, MEM_ACT_VSTA=%d, MEM_ACT_WID=%d, MEM_ACT_LEN=%d\33[m\n",
			imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_HSTA,
			imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_VSTA,
			imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID,
			imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_LEN);

	rtd_pr_smt_debug("\33[35m_rk[zoom]DispWid=%d, DispLen=%d, H_TOTAL=%d, V_TOTAL=%d\33[m\n",
			imdSmoothInfo[display].IMD_SMOOTH_DispWid,
			imdSmoothInfo[display].IMD_SMOOTH_DispLen,
			imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL,
			imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL);

	rtd_pr_smt_debug("\33[35mDISP_RTNR:%d,DISP_10BIT:%d,DISP_422CAP:%d\33[m\n",imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR,imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT,imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP);
	rtd_pr_smt_debug("\33[35mDI_CUT_4_LINES:%d\33[m\n",imdSmoothInfo[display].IMD_SMOOTH_DI_CUT_4_LINES);
	rtd_pr_smt_debug("\33[35mINTERLACE:%d\33[m\n",imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE);
	rtd_pr_smt_debug("\33[35mTHRIP:%d\33[m\n",imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP);
	rtd_pr_smt_debug("\33[35mvflip:%d,vflip3:%d\33[m\n",imdSmoothInfo[display].IMD_SMOOTH_VFLIP, imdSmoothInfo[display].IMD_SMOOTH_DISP_VFLIP3);
#if disablepart //to fix compile error  kernel rika 20140929
	rtd_pr_smt_debug("\33[35mscaler vflip:%d,vflip3:%d\33[m\n",Scaler_Get_Vflip_Enable(), Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_VFLIP3));
#endif


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



//-----------------
#define _PROTECT_VDELAY 4
#define _PROTECT_MINI_VDELAY 2
unsigned char zoom_imd_smooth_cal_vgip_capture( unsigned char display,
	unsigned int  un32_hposition, unsigned int  un32_vposition,
	unsigned int  un32_hsize, unsigned int  un32_vsize )
{


	///unsigned short temp;//unused
	unsigned char result = TRUE;
	unsigned char colorspace=0;

	imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_HSTA = 0;
	imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_VSTA = 0;

	imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_WID = un32_hsize;
	imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_LEN = un32_vsize;
	imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_STA = un32_hposition;
	imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_STA = un32_vposition;

	rtd_pr_smt_info("\33[31m_rk[smooth]Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_COLOR_SPACE):%d\33[m\n",Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_COLOR_SPACE));
	colorspace=Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_COLOR_SPACE);
	if( ((Scaler_InputSrcGetType(display) == _SRC_HDMI) || (Scaler_InputSrcGetType(display) == _SRC_VO) )&&
		((colorspace!= SCALER_INPUT_COLOR_RGB) && (colorspace!= SCALER_INPUT_COLOR_YUV444)) )
	{

		if( (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA %2) != (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE %2) )
		{
				imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA -1;
				un32_hposition=imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA;//need to update un32_hposition for the following calculation rika20160204;

		}
	}
	//else rtd_pr_smt_info("_rk %s %d\n",__FUNCTION__,__LINE__);


	if (_SRC_HDMI == Scaler_InputSrcGetType(display) )
	{
		unsigned int len_pre=imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE;
		//rtd_pr_smt_debug("[pool test]len_pre:%d,interlace:%d,freq:%d\n",len_pre,(imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE),imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000);
	//for interlace case
		if((imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE) && (((len_pre==540)&&(imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000 > 49000))))
		{
			//1080i framerate>50hz,no need adjust portch
			h_delay = 0;
			v_delay = 0;
		}
		else if(imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE)
		{
			if (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA>(_PROTECT_VDELAY+_PROTECT_MINI_VDELAY)) {
				imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA-_PROTECT_VDELAY;
				v_delay = _PROTECT_VDELAY;
			} else {
				v_delay = 0;
			}
			h_delay = 0;
		}
	//for progressive case
		else if((len_pre == 768) && (imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000 > 70500))//for 768p framerate>70,need set delay value be 4
		{
			if (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA>(_PROTECT_VDELAY+_PROTECT_MINI_VDELAY)) {
				imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA-_PROTECT_VDELAY;
				v_delay = _PROTECT_VDELAY;
			} else {
				v_delay = 0;
			}
			h_delay = 0;
		}
		else if((len_pre!=480)&&(len_pre!=576)&&(len_pre!=768))// 480p and 576p need set protch by DIC suggester, others need set vdelay be 4
		{
			//add PC timing over 60Hz case @Crixus 20160401
			if(imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000 >= V_FREQ_60000_pOFFSET){
				if (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA > (_PROTECT_VDELAY_70Hz+_PROTECT_MINI_VDELAY)) {
					imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA - _PROTECT_VDELAY_70Hz;
					v_delay = _PROTECT_VDELAY_70Hz;
				} else {
					v_delay = 0;
				}
			}
			else{
				if (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA>(_PROTECT_VDELAY+_PROTECT_MINI_VDELAY)) {
					imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA-_PROTECT_VDELAY;
					v_delay = _PROTECT_VDELAY;
		} else {
					v_delay = 0;
				}
			}
			h_delay = 0;
		}
		else //for 480p 576p and 768p(framerate <700)
		{
			int vgip_hstar = 0,vgip_vstar = 0;
			vgip_hstar = (imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL - imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID + 1)/2;
			if(vgip_hstar -(int)un32_hposition > 0)
			{
				if(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA>8) {
					h_delay = 8;
					imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA -= 8;
				} else {
					h_delay = 0;
				}
			}
			else
			{
				h_delay = un32_hposition - vgip_hstar;
				imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA = vgip_hstar;
			}
			vgip_vstar = (imdSmoothInfo[display].IMD_SMOOTH_V_TOTAL - imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN + 1)/2;
			if(vgip_vstar -(int)un32_vposition > 0)
			{
				if(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA>(8+_PROTECT_MINI_VDELAY))		{
					v_delay =8;
					imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA -=8;
				} else {
					v_delay = 0;
				}
			}
			else
			{
				v_delay = un32_vposition - vgip_vstar;
				if(vgip_vstar>_PROTECT_MINI_VDELAY) {
					imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA = vgip_vstar;
				} else {
					v_delay=0;
				}
			}
		}
	}
	else
	{
		h_delay = 0;
		v_delay = 0;
	}


	#if 0 //rika mark below cause hratio/vratio is not used
	if(imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC == FALSE)
	{
		rtd_pr_smt_debug("file=%s, function=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__);
		if(vgip_overscan_hratio != 100)
		{
			ulResult = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID*(100-vgip_overscan_hratio)/200;
			if (ulResult%2)//for YUV422
				ulResult++;

			imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA + ulResult;
			imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID -(2*ulResult);
		}
		if(vgip_overscan_vratio != 100)
		{
			ulResult = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN*(100-vgip_overscan_vratio)/200;
			imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA =imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA	+ ulResult;
			imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN =imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN -(2*ulResult);
		}
	}

	#endif

	#if 0
	if(Scaler_PipGetInfo(SLR_PIP_ENABLE) && ((Scaler_PipGetInfo(SLR_PIP_TYPE) >= SUBTYPE_PIPMLSR) && (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_MP1L7B)))
		return result;
	#endif

	//rtd_pr_smt_debug("vgip_overscan_hratio=%d, ratio_type=%d\n", vgip_overscan_hratio, ratio_type);


	//to set picture in center
	#if 0
	// we get wrong hsta when CONFIG_ENABLE_VD_27MHZ_MODE is on. hwid is too large when 27m is on
	if((imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN)>(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE))
	{
		imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA=imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA+(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN-imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE)/2;
		imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN=imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE;
	}
	if((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID)>(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE))
	{
		imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA=imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA+(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID-imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE)/2;
		imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID= (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE);
	}
	#endif

	//USER:LewisLee DATE:2012/11/08
	//width need as even, to prevent picture abnormal
	if((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID) & _BIT0)
	{
		imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID= (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID) & (~_BIT0);
	}



	return result;
}


void zoom_imd_smooth_color_ultrazoom_config_scaling_down(unsigned char display)
{
	SMOOTH_TOGGLE_SIZE inSize, outSize;
	unsigned char panorama;
	//vgip_data_path_select_RBUS data_path_select_reg;
	scaledown_ich1_sdnr_444to422_ctrl_RBUS ich1_sdnr_444to422_ctrl_REG;
	scaledown_ich1_uzd_ctrl0_RBUS ich1_uzd_Ctrl0_REG;
	hsd_dither_di_hsd_ctrl0_RBUS di_hsd_ctrl0_reg;

	if(ratio_type == SLR_RATIO_PANORAMA)
	{
		panorama = TRUE;
	}
	else
	{
		panorama = FALSE;
	}

	//frank@03272013 add below code to adjust Hscale down position to solve custom mode display window fail.
/*	if (imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP || imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR) {	// will added in future
		data_path_select_reg.regValue = rtd_inl(VGIP_Data_Path_Select_reg);
		data_path_select_reg.uzd1_in_sel = _ENABLE;
		data_path_select_reg.xcdi_in_sel = _CHANNEL1;
		rtd_outl(VGIP_Data_Path_Select_reg, data_path_select_reg.regValue);
	}*/

//	smooth_toggle_Scaler_AdjustPathAndInfo(display);	// Move position
	//adjust di_hsd_flag and imdSmoothInfo[display].IMD_SMOOTH_DI_WID last time rika 20140612

	if(display == SLR_MAIN_DISPLAY)
	{
		// Scale down setup for Channel1

		ich1_uzd_Ctrl0_REG.regValue = rtd_inl(SCALEDOWN_ICH1_UZD_Ctrl0_reg);
		ich1_uzd_Ctrl0_REG.v_zoom_en = imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_DOWN;
		ich1_uzd_Ctrl0_REG.buffer_mode = imdSmoothInfo[display].IMD_SMOOTH_V_UZD_BUFFER_MODE;

		di_hsd_ctrl0_reg.regValue = rtd_inl(HSD_DITHER_DI_HSD_Ctrl0_reg);

		if (0/*di_hsd_flag*/)//By CONFIG_ARCH_RTK284X  HSD & UZD are independ each other
		{//go DI path
			ich1_uzd_Ctrl0_REG.h_zoom_en = 0;
			//di_hsd_ctrl0_reg.h_zoom_en = (imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_DOWN!=0);
		}
		else
		{
			ich1_uzd_Ctrl0_REG.h_zoom_en = (imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_DOWN!=0);
			//di_hsd_ctrl0_reg.h_zoom_en = 0;
		}
		ich1_uzd_Ctrl0_REG.sort_fmt =(imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC ||(!imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP || dvrif_memory_compression_get_enable(display)));
		ich1_uzd_Ctrl0_REG.out_bit =imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT;
		ich1_uzd_Ctrl0_REG.video_comp_en = 0;
		ich1_uzd_Ctrl0_REG.truncationctrl = 0;
		rtd_outl(SCALEDOWN_ICH1_UZD_Ctrl0_reg, ich1_uzd_Ctrl0_REG.regValue);
		rtd_outl(HSD_DITHER_DI_HSD_Ctrl0_reg, di_hsd_ctrl0_reg.regValue);

		// write the default of sdnr_444to422_ctrl: jyyang

		ich1_sdnr_444to422_ctrl_REG.regValue = rtd_inl(SCALEDOWN_ICH1_sdnr_444to422_CTRL_reg);
		ich1_sdnr_444to422_ctrl_REG.cr_dly_cb = 0;
		ich1_sdnr_444to422_ctrl_REG.drop = 1;
		rtd_outl(SCALEDOWN_ICH1_sdnr_444to422_CTRL_reg, ich1_sdnr_444to422_ctrl_REG.regValue);
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		scaledown_ich2_uzd_ctrl0_RBUS ich2_uzd_Ctrl0_REG;
		ich2_uzd_Ctrl0_REG.regValue = rtd_inl(SCALEDOWN_ICH2_UZD_Ctrl0_reg);
		ich2_uzd_Ctrl0_REG.v_zoom_en = (imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_DOWN!=0);
		ich2_uzd_Ctrl0_REG.buffer_mode = imdSmoothInfo[display].IMD_SMOOTH_V_UZD_BUFFER_MODE;
		ich2_uzd_Ctrl0_REG.h_zoom_en = (imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_DOWN!=0);
		ich2_uzd_Ctrl0_REG.output_fmt = ((imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP)? 0 : 1);
		ich2_uzd_Ctrl0_REG.video_comp_en = 0;
		rtd_outl(SCALEDOWN_ICH2_UZD_Ctrl0_reg, ich2_uzd_Ctrl0_REG.regValue);
	}
#endif

	inSize.nLength = imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_LEN;
	inSize.nWidth = imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_WID;

	outSize.nLength = imdSmoothInfo[display].IMD_SMOOTH_CapLen;
	outSize.nWidth = imdSmoothInfo[display].IMD_SMOOTH_CapWid;

	if(display == SLR_MAIN_DISPLAY)
	{
		scaledown_ich1_ibuff_ctrli_buff_ctrl_RBUS ich1_ibuff_ctrl_BUFF_CTRL;
		ich1_ibuff_ctrl_BUFF_CTRL.regValue = rtd_inl(SCALEDOWN_ICH1_IBUFF_CTRLI_BUFF_CTRL_reg);
		ich1_ibuff_ctrl_BUFF_CTRL.ibuf_h_size = outSize.nWidth; //rika 20140611 need review
		//ich1_ibuff_ctrl_BUFF_CTRL.ibuf_h_size = inSize.nWidth;
		ich1_ibuff_ctrl_BUFF_CTRL.ibuf_v_size = inSize.nLength;
		rtd_outl(SCALEDOWN_ICH1_IBUFF_CTRLI_BUFF_CTRL_reg, ich1_ibuff_ctrl_BUFF_CTRL.regValue);
	}
	rtd_pr_smt_emerg("_rk inSize.width:%d, inSize.len:%d, outSize.width:%d:, outSize.len:%d\n",
			inSize.nWidth, inSize.nLength,
			outSize.nWidth, outSize.nLength);


	zoom_imd_smooth_color_ultrazoom_set_scale_down(display,&inSize, &outSize, panorama);

//mark below due to need to review VIP code
//zoom_drvif_color_ultrazoom_scale_down_444_to_422(display, &inSize, &outSize);	//CSW+ 0970324 for RGB to do 444->422 color will have sample error phase

}


void zoom_imd_smooth_color_ultrazoom_config_scaling_down_di_hsd(unsigned char display)
{
	SMOOTH_TOGGLE_SIZE inSize, outSize;
	unsigned char panorama;
	//vgip_data_path_select_RBUS data_path_select_reg;
	scaledown_ich1_sdnr_444to422_ctrl_RBUS ich1_sdnr_444to422_ctrl_REG;
	scaledown_ich1_uzd_ctrl0_RBUS ich1_uzd_Ctrl0_REG;
	hsd_dither_di_hsd_ctrl0_RBUS di_hsd_ctrl0_reg;

	if(ratio_type == SLR_RATIO_PANORAMA)
	{
		panorama = TRUE;
	}
	else
	{
		panorama = FALSE;
	}

	//frank@03272013 add below code to adjust Hscale down position to solve custom mode display window fail.
/*	if (imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP || imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR) {	// will added in future
		data_path_select_reg.regValue = rtd_inl(VGIP_Data_Path_Select_reg);
		data_path_select_reg.uzd1_in_sel = _ENABLE;
		data_path_select_reg.xcdi_in_sel = _CHANNEL1;
		rtd_outl(VGIP_Data_Path_Select_reg, data_path_select_reg.regValue);
	}*/

//	smooth_toggle_Scaler_AdjustPathAndInfo(display);	// Move position
	//adjust di_hsd_flag and imdSmoothInfo[display].IMD_SMOOTH_DI_WID last time rika 20140612

	if(display == SLR_MAIN_DISPLAY)
	{
		// Scale down setup for Channel1
		di_hsd_ctrl0_reg.regValue = rtd_inl(HSD_DITHER_DI_HSD_Ctrl0_reg);

		if (di_hsd_flag)
		{//go DI path
			ich1_uzd_Ctrl0_REG.h_zoom_en = 0;
			di_hsd_ctrl0_reg.h_zoom_en = (imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_DOWN!=0);
		}
		else
		{
			di_hsd_ctrl0_reg.h_zoom_en = 0;
		}
		rtd_outl(HSD_DITHER_DI_HSD_Ctrl0_reg, di_hsd_ctrl0_reg.regValue);

		// write the default of sdnr_444to422_ctrl: jyyang
		ich1_sdnr_444to422_ctrl_REG.regValue = rtd_inl(SCALEDOWN_ICH1_sdnr_444to422_CTRL_reg);
		ich1_sdnr_444to422_ctrl_REG.cr_dly_cb = 0;
		ich1_sdnr_444to422_ctrl_REG.drop = 1;
		rtd_outl(SCALEDOWN_ICH1_sdnr_444to422_CTRL_reg, ich1_sdnr_444to422_ctrl_REG.regValue);
	}

	inSize.nLength = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN;
	inSize.nWidth = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID;
	outSize.nLength = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN;
	outSize.nWidth = imdSmoothInfo[display].IMD_SMOOTH_DI_WID;

	if(display == SLR_MAIN_DISPLAY)
	{
		scaledown_ich1_ibuff_ctrli_buff_ctrl_RBUS ich1_ibuff_ctrl_BUFF_CTRL;
		ich1_ibuff_ctrl_BUFF_CTRL.regValue = rtd_inl(SCALEDOWN_ICH1_IBUFF_CTRLI_BUFF_CTRL_reg);
		ich1_ibuff_ctrl_BUFF_CTRL.ibuf_h_size = outSize.nWidth; //rika 20140611 need review
		//ich1_ibuff_ctrl_BUFF_CTRL.ibuf_h_size = inSize.nWidth;
		ich1_ibuff_ctrl_BUFF_CTRL.ibuf_v_size = inSize.nLength;
		rtd_outl(SCALEDOWN_ICH1_IBUFF_CTRLI_BUFF_CTRL_reg, ich1_ibuff_ctrl_BUFF_CTRL.regValue);
	}
	rtd_pr_smt_emerg("_rk inSize.width:%d, inSize.len:%d, outSize.width:%d:, outSize.len:%d\n",
			inSize.nWidth, inSize.nLength,
			outSize.nWidth, outSize.nLength);


	zoom_imd_smooth_color_ultrazoom_set_scale_down_di_hsd(display,&inSize, &outSize, panorama);

//mark below due to need to review VIP code
//zoom_drvif_color_ultrazoom_scale_down_444_to_422(display, &inSize, &outSize);	//CSW+ 0970324 for RGB to do 444->422 color will have sample error phase

}


void zoom_imd_smooth_color_ultrazoom_set_scale_down(unsigned char channel, SMOOTH_TOGGLE_SIZE* ptInSize, SMOOTH_TOGGLE_SIZE* ptOutSize, UINT8 panorama)
{
#ifdef VPQ_COMPILER_ERROR_ENABLE_MERLIN6
	unsigned char SDRatio;
	unsigned char SDFilter=0;
//	unsigned int tmp_data;
	signed short *coef_pt;

//	unsigned char i;
	unsigned long long nFactor,D;
	unsigned long long ulparam1 = 0;
	unsigned long long ulparam2 = 0;
	unsigned char Hini, Vini, a;
	unsigned short S1,S2;
	unsigned long long ullinsize;
	unsigned long long ulloutsize;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	scaledown_ich1_uzd_ctrl0_RBUS 			ich1_uzd_Ctrl0_REG;
	scaledown_ich1_uzd_ctrl1_RBUS 			ich1_uzd_Ctrl1_REG;
	scaledown_ich1_uzd_scale_hor_factor_RBUS	ich1_uzd_Scale_Hor_Factor_REG;
	scaledown_ich1_uzd_scale_ver_factor_RBUS	ich1_uzd_Scale_Ver_Factor_REG;
	scaledown_ich1_uzd_initial_value_RBUS 		ich1_uzd_Initial_Value_REG;

	hsd_dither_di_hsd_ctrl0_RBUS					di_hsd_ctrl0_reg;
	hsd_dither_di_hsd_scale_hor_factor_RBUS		di_hsd_scale_hor_factor_reg;
	hsd_dither_di_hsd_initial_value_RBUS			di_hsd_initial_value_reg;
	hsd_dither_di_hsd_hor_segment_RBUS 		di_hsd_hor_segment_reg;

	#ifdef CONFIG_DUAL_CHANNEL
	scaledown_ich2_uzd_ctrl0_RBUS 			ich2_uzd_Ctrl0_REG;
	scaledown_ich2_uzd_ctrl1_RBUS 			ich2_uzd_Ctrl1_REG;
	scaledown_ich2_uzd_h_output_size_RBUS 	ich2_uzd_H_Output_Size_REG;
	scaledown_ich2_uzd_scale_hor_factor_RBUS	ich2_uzd_Scale_Hor_Factor_REG;
	scaledown_ich2_uzd_scale_ver_factor_RBUS	ich2_uzd_Scale_Ver_Factor_REG;
	scaledown_ich2_uzd_initial_value_RBUS 		ich2_uzd_Initial_Value_REG;
	#endif

	ich1_uzd_Ctrl0_REG.regValue 	= rtd_inl(SCALEDOWN_ICH1_UZD_Ctrl0_reg);
	ich1_uzd_Ctrl1_REG.regValue 	= rtd_inl(SCALEDOWN_ICH1_UZD_Ctrl1_reg);

	di_hsd_ctrl0_reg.regValue = rtd_inl(HSD_DITHER_DI_HSD_Ctrl0_reg);
	di_hsd_hor_segment_reg.regValue = rtd_inl(HSD_DITHER_DI_HSD_Hor_Segment_reg);
	ich1_uzd_Scale_Ver_Factor_REG.regValue=rtd_inl(SCALEDOWN_ICH1_UZD_Scale_Ver_Factor_reg);

	#ifdef CONFIG_DUAL_CHANNEL


	ich2_uzd_Ctrl0_REG.regValue 	= rtd_inl(SCALEDOWN_ICH2_UZD_Ctrl0_reg);
	ich2_uzd_Ctrl1_REG.regValue 	= rtd_inl(SCALEDOWN_ICH2_UZD_Ctrl1_reg);
	ich2_uzd_H_Output_Size_REG.regValue= rtd_inl(SCALEDOWN_ICH2_UZD_H_Output_Size_reg);
	#endif
	//	g_ucIsHScalerDown = 0;
	//	g_ucIsVScalerDown = 0;
	//	g_ucIsH444To422 = 0;

	#define TMPMUL	(16)

	//o============ H scale-down=============o
	if(ptInSize->nWidth > ptOutSize->nWidth) //fix 3d
	{
		if ( ptOutSize->nWidth == 0 ) {
			rtd_pr_smt_debug("output width = 0 !!!\n");
			SDRatio = 0;
		} else {
			SDRatio = (ptInSize->nWidth*TMPMUL) / ptOutSize->nWidth;
		}

		if(SDRatio <= ((TMPMUL*3)/2))	//<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) ) // Mid
			SDFilter = 1;
		else		// blurest, narrow bw
			SDFilter = 0;

		//o---------------- fill coefficients into access port--------------------o
		coef_pt = tScaleDown_COEF_TAB[SD_H_Coeff_Sel[SDFilter]];

		if(channel == _CHANNEL1)
		{
#if 0	/* do not change table in zoom smooth toggle, this filter will be set in PQ flow*/
			if(di_hsd_flag)
			{//go DI path
				di_hsd_ctrl0_reg.h_y_table_sel = 0; // TAB1
				di_hsd_ctrl0_reg.h_c_table_sel = 0; // TAB1
			}else
			{
				ich1_uzd_Ctrl0_REG.h_y_table_sel = 0;	// TAB1
				ich1_uzd_Ctrl0_REG.h_c_table_sel = 0;	// TAB1
			}
			for (i=0; i<16; i++)
			{
				tmp_data = ((unsigned int)(*coef_pt++)<<16);
				tmp_data += (unsigned int)(*coef_pt++);
				if(di_hsd_flag)
				{//go DI path
					rtd_outl(HSD_DITHER_DI_UZD_FIR_Coef_Tab1_C0_reg+ i*4, tmp_data);
				}else
				{
					rtd_outl(SCALEDOWN_ICH1_UZD_FIR_Coef_Tab1_C0_reg+ i*4, tmp_data);
				}
			}
			//		g_ucIsHScalerDown = 1;
			//		g_ucSDFHSel = SDFilter;
#else
			/*do nothing*/
			;
#endif

		}
	#ifdef CONFIG_DUAL_CHANNEL
		else
		{
			ich2_uzd_Ctrl1_REG.h_c_filter = (SDFilter+1)%3;
			ich2_uzd_Ctrl1_REG.h_y_filter = ich2_uzd_Ctrl1_REG.h_c_filter;
		}
	#endif
	}
	//o============ V scale-down=============o
	if (ptInSize->nLength > ptOutSize->nLength) {
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o

		//jeffrey 961231
		if ( ptOutSize->nLength == 0 ) {
			SDRatio = 0;
		} else {
			SDRatio = (ptInSize->nLength*TMPMUL) / ptOutSize->nLength;
		}

		if(SDRatio <= ((TMPMUL*3)/2))	//<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) ) // Mid
			SDFilter = 1;
		else		// blurest, narrow bw
			SDFilter = 0;

		//o---------------- fill coefficients into access port--------------------o
		coef_pt = tScaleDown_COEF_TAB[SD_V_Coeff_Sel[SDFilter]];

		if(channel==_CHANNEL1)
		{
#if 0	/* do not change scaling setting in zoom smooth toggle, this filter will be set in PQ flow*/
			ich1_uzd_Ctrl0_REG.v_y_table_sel = 1;	// TAB2
			ich1_uzd_Ctrl0_REG.v_c_table_sel = 1;	// TAB2

			for (i=0; i<16; i++)
			{
				tmp_data = ((unsigned int)(*coef_pt++)<<16);
				tmp_data += (unsigned int)(*coef_pt++);
				rtd_outl(SCALEDOWN_ICH1_UZD_FIR_Coef_Tab2_C0_reg+ i*4, tmp_data);
			}
			//	g_ucIsVScalerDown = 1;
			//	g_ucSDFVSel = SDFilter;
#else
			/*do nothing*/
			;

#endif
		}
	#ifdef CONFIG_DUAL_CHANNEL
		else
		{

		}
	#endif
	}
	else{//no need	V scale-down, use bypass filter

	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	/* for 4K scaler down grid line*/
	if (ptInSize->nWidth > 3000 && ptOutSize->nWidth <= 1366 && ich1_uzd_Ctrl0_REG.v_zoom_en == 1)
		ich1_uzd_Ctrl1_REG.four_tap_en = 1;
	else
		ich1_uzd_Ctrl1_REG.four_tap_en = 0;


	// Decide Output data format for scale down
	//============H Scale-down============
	if(channel == _CHANNEL1)
	{
		if (ptInSize->nWidth > ptOutSize->nWidth) //fix 3d
		{	 // H scale-down
			Hini = 0x80;//0xff;//0x78;

			ich1_uzd_Initial_Value_REG.hor_ini = Hini;
			ich1_uzd_Scale_Ver_Factor_REG.hor_ini_int=1;
			di_hsd_initial_value_reg.hor_ini = Hini;
			di_hsd_initial_value_reg.hor_ini_int= 1;
			a = 5;

			if(panorama && (!imdSmoothInfo[channel].IMD_SMOOTH_H_SCALE_UP))
			{  // CSW+ 0960830 Non-linear SD
				S1 = (ptOutSize->nWidth) >> 2;
				S2 = (ptOutSize->nWidth) - S1*2;
				//=vv==old from TONY, if no problem, use this=vv==
				////removed rika20150904 for 4k2k issue
				////nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13));
				ulparam1 = ((unsigned long long)(ptInSize->nWidth)<<21) - ((unsigned long long)Hini<<13);

				//=vv==Thur debugged=vv==
				//nFactor = ((((unsigned long long)(ptInSize->nWidth))*(ptOutSize->nWidth-1))<<21) / ptOutSize->nWidth;
				//===================
				////removed rika20150904 for 4k2k issue
				////nFactor = nFactor /((2*a+1)*S1 + (a+1)*S2 - a) * a;
				////D = nFactor / a / S1;
				// coverity 120588,120587 : rikahsu 20151217
				// transform to unsigned long long before - to prevent sign extension
				ulparam2=((2*a+1)*((unsigned long long)S1) + (a+1)*((unsigned long long)S2) - a);
				do_div(ulparam1,ulparam2);
				nFactor=ulparam1*a;

				ulparam1=nFactor;
				do_div(ulparam1,a);
				do_div(ulparam1,S1);
				D=ulparam1;

				nFactor = SHR((nFactor + 1), 1); //rounding
				D = SHR(D + 1, 1); //rounding

				if(nFactor < 1048576) {
					//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13)) / (ptOutSize->nWidth-1);
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)(((ptInSize->nWidth)<<21)) / (ptOutSize->nWidth);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<21;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
					nFactor = SHR(nFactor + 1, 1); //rounding
					S1 = 0;
					S2 = ptOutSize->nWidth;
					D = 0;
				}
			}
			else{

				//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13)) / (ptOutSize->nWidth-1);
				if ( ptOutSize->nWidth == 0 ) {
					rtd_pr_smt_debug("output width = 0 !!!\n");
					nFactor = 0;
				}
				else if(ptInSize->nWidth>4095){
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)((((ptInSize->nWidth-1)<<19) / (ptOutSize->nWidth-1))<<1);
					//ulparam1=(unsigned long long)(ptInSize->nWidth-1)<<19;
					//ulparam2=(unsigned long long)(ptOutSize->nWidth-1);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<19;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
					nFactor=nFactor<<1;
				}
				else if(ptInSize->nWidth>2047){
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)(((ptInSize->nWidth-1)<<20) / (ptOutSize->nWidth-1));
					//ulparam1=(unsigned long long)(ptInSize->nWidth-1)<<20;
					//ulparam2=(unsigned long long)(ptOutSize->nWidth-1);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<20;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
				}
				else {
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)(((ptInSize->nWidth-1)<<21) / (ptOutSize->nWidth-1));
					//ulparam1=(unsigned long long)(ptInSize->nWidth-1)<<21;
					//ulparam2=(unsigned long long)(ptOutSize->nWidth-1);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<21;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
					//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) / (ptOutSize->nWidth));//old
					nFactor = SHR(nFactor + 1, 1); //rounding
				}

				//rtd_pr_smt_debug("nFactor=%d, input Wid=%d, Out Wid=%d\n",nFactor, ptInSize->nWidth, ptOutSize->nWidth);
				S1 = 0;
				S2 = ptOutSize->nWidth;
				D = 0;
			}
		}else {
			nFactor = 0x100000;
			S1 = 0;
			S2 = ptOutSize->nWidth;
			D = 0;
		}

		if(0/*di_hsd_flag*/)	//By CONFIG_ARCH_RTK284X  HSD & UZD are independ each other
		{//go DI path
			di_hsd_hor_segment_reg.nl_seg2 = S2;
			di_n1_seg1=S1;
			di_n1_seg2=S2;
		}else
		{
			ich1_uzd_Scale_Hor_Factor_REG.hor_fac = nFactor;
		}

		//rtd_pr_smt_debug("nFactor=%x, input_Wid=%d, Output_Wid=%d\n",nFactor, ptInSize->nWidth, ptOutSize->nWidth);

		//================V Scale-down=================

		if (ptInSize->nLength > ptOutSize->nLength) {	 // V scale-down
			//			ich1_uzd_Ctrl0_REG.buffer_mode = 0x02;	// buffer assign to vertical uzd
			Vini = 0xff;//0x78;
			ich1_uzd_Initial_Value_REG.ver_ini = Vini;
			//nFactor = (unsigned int)((ptInSize->nLength <<20) - ((unsigned int)Vini<<12)) / (ptOutSize->nLength - 1);

			if ( ptOutSize->nLength == 0 ) {
				rtd_pr_smt_debug("output length = 0 !!!\n");
				nFactor = 0;
			}
			else {

				if((ptInSize->nLength>=1080) && (ptOutSize->nLength<=544)){
					//frank@1018 sync with pacific to solve 3d vide 1920X2205 scaler down fail
					//nFactor = (unsigned int)(((ptInSize->nLength-1)<<21) / (ptOutSize->nLength-1));
	#if disablepart
					//fixed rika 20141002 change to do_div
					//fix me rika 20141001 unsigned long long error
					//6559: undefined reference to `__aeabi_uldivmod'
					///nFactor = (unsigned int)(((unsigned long long)(ptInSize->nLength-1)<<21) / (ptOutSize->nLength-1));
	#endif

					ullinsize=((unsigned long long)(ptInSize->nLength-1)<<21);
					ulloutsize=(unsigned long long)(ptOutSize->nLength-1);
					do_div(ullinsize,ulloutsize);//sync with svn 742616 check history 2
					nFactor=ullinsize;


				}
				else{
					//frank@1018 sync with pacific to solve 3d vide 1920X2205 scaler down fail
					//nFactor = (unsigned int)(((ptInSize->nLength)<<21) / (ptOutSize->nLength));
	#if disablepart //fixed rika 20141002 change to do_div
					//fix me rika 20141001 unsigned long long error
					///nFactor = (unsigned int)(((unsigned long long)(ptInSize->nLength)<<21) / (ptOutSize->nLength));//6559: undefined reference to `__aeabi_uldivmod'
	#endif
					ullinsize=(unsigned long long)(ptInSize->nLength)<<21;
					ulloutsize=(unsigned long long)(ptOutSize->nLength);
					do_div(ullinsize,ulloutsize);//sync with svn 742616 check history 2
					nFactor=ullinsize;

				}

			}
			//rtd_pr_smt_debug("Ch1 Ver: CAP=%d, Disp=%d, factor=%d\n", ptInSize->nLength, ptOutSize->nLength, nFactor);
			nFactor = SHR(nFactor + 1, 1); //rounding
			ich1_uzd_Scale_Ver_Factor_REG.ver_fac = nFactor;
		} else {
			ich1_uzd_Scale_Ver_Factor_REG.ver_fac = 0x100000;
		}

		//	ich1_uzd_Ctrl0_REG.   = 1;
		//	ich1_uzd_Ctrl0_REG.sort_fmt = 1;
		//	CLR_422_Fr_SD();

		if(Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE()	)
		{
	#if 0   //fix 3d
			if(Scaler_Disp3dGetInfo(SLR_DISP_3D_3DTYPE)==SLR_3D_SIDE_BY_SIDE_HALF)
			{
				ich1_hsd_3d_boundary_RBUS ich1_hsd_3d_boundary_reg;
				ich1_hsd_3d_boundary_reg.regValue = rtd_inl(SCALEDOWN_ICH1_HSD_3D_BOUNDARY_VADDR);
				ich1_uzd_Ctrl0_REG.sel_3d_en=1;
				ich1_uzd_Ctrl1_REG.vsd_3d_mode = 1;
				ich1_uzd_Ctrl1_REG.vsd_3d_boundary = (ptOutSize->nWidth)>>1;
				ich1_hsd_3d_boundary_reg.hsd_3d_boundary = (ptOutSize->nWidth)>>1;
				rtd_outl(SCALEDOWN_ICH1_HSD_3D_BOUNDARY_VADDR, ich1_hsd_3d_boundary_reg.regValue);
			}
			else
	#endif
			{
				ich1_uzd_Ctrl0_REG.sel_3d_en=0;
//				ich1_uzd_Ctrl1_REG.vsd_3d_mode = 0;
			}
		}

		rtd_outl(SCALEDOWN_ICH1_UZD_Ctrl0_reg, ich1_uzd_Ctrl0_REG.regValue);
		rtd_outl(SCALEDOWN_ICH1_UZD_Ctrl1_reg, ich1_uzd_Ctrl1_REG.regValue);
		rtd_outl(SCALEDOWN_ICH1_UZD_Scale_Ver_Factor_reg, ich1_uzd_Scale_Ver_Factor_REG.regValue);
		rtd_outl(SCALEDOWN_ICH1_UZD_Initial_Value_reg, ich1_uzd_Initial_Value_REG.regValue);

		if(0/*di_hsd_flag*/)	//By CONFIG_ARCH_RTK284X  HSD & UZD are independ each other
		{//go DI path
			rtd_outl(HSD_DITHER_DI_HSD_Ctrl0_reg, di_hsd_ctrl0_reg.regValue);
			rtd_outl(HSD_DITHER_DI_HSD_Initial_Value_reg, di_hsd_initial_value_reg.regValue);
			rtd_outl(HSD_DITHER_DI_HSD_Scale_Hor_Factor_reg, di_hsd_scale_hor_factor_reg.regValue);
			rtd_outl(HSD_DITHER_DI_HSD_Hor_Segment_reg, di_hsd_hor_segment_reg.regValue);
		}else
		{
			rtd_outl(SCALEDOWN_ICH1_UZD_Scale_Hor_Factor_reg, ich1_uzd_Scale_Hor_Factor_REG.regValue);
		}
	}
	#ifdef CONFIG_DUAL_CHANNEL
	if(channel == _CHANNEL2)
	{
		//sub channel (jyyang)
		if (ptInSize->nWidth > ptOutSize->nWidth) {    // H scale-down
			Hini = 0xff;//0x78;
			ich2_uzd_Initial_Value_REG.hor_ini = Hini;
	#ifdef CONFIG_CUSTOMER_1
xxx
			if (COsdCtrlGetOverSampling() && (info->ucMode_Curr==_MODE_720P60 ||info->ucMode_Curr==_MODE_720P50 ||info->ucMode_Curr==_MODE_480P ||info->ucMode_Curr==_MODE_480I))
				//nFactor = (unsigned int)((ptInSize->nWidth<<19) - (Hini<<11)) / (ptOutSize->nWidth-1) *2;
				nFactor = (unsigned int)((ptInSize->nWidth<<19)) / (ptOutSize->nWidth) *2;
			else
	#endif
			{
				//frank@0108 add for code exception
				if ( ptOutSize->nWidth == 0 ) {
					rtd_pr_smt_debug("output width = 0 !!!\n");
					nFactor = 0;
				} else {
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)((ptInSize->nWidth<<21)) / (ptOutSize->nWidth);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<21;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
				}
			}
			nFactor = SHR(nFactor + 1, 1); //rounding
			ich2_uzd_Scale_Hor_Factor_REG.hor_fac = nFactor;
		}else {
			ich2_uzd_Scale_Hor_Factor_REG.hor_fac = 0x100000;
		}
		ich2_uzd_H_Output_Size_REG.h_output_size = ptOutSize->nWidth;

		if (ptInSize->nLength > ptOutSize->nLength) {	 // V scale-down
			//			ich2_uzd_Ctrl0_REG.buffer_mode = 0x02;
			Vini = 0xff;//0x78;
			ich2_uzd_Initial_Value_REG.ver_ini = Vini;
			//nFactor = (unsigned int)((ptInSize->nLength << 20) - (Vini<<12)) / (ptOutSize->nLength - 1);
			//frank@0108 add for code exception
			if ( ptOutSize->nLength == 0 ) {
				rtd_pr_smt_debug("output length = 0 !!!\n");
				nFactor = 0;
			} else {
				////removed rika20150904 for 4k2k issue
				////nFactor = (unsigned int)((ptInSize->nLength << 21)) / (ptOutSize->nLength);
				ulparam1=(unsigned long long)(ptInSize->nLength)<<21;
				ulparam2=(unsigned long long)(ptOutSize->nLength);
				do_div(ulparam1,ulparam2);
				nFactor=ulparam1;
			}
			nFactor = SHR(nFactor+1, 1); //rounding
			ich2_uzd_Scale_Ver_Factor_REG.ver_fac = nFactor;
		}else{
			//			ich2_uzd_Ctrl0_REG.buffer_mode = 0;
			ich2_uzd_Scale_Ver_Factor_REG.ver_fac = 0x100000;
		}

		//	ich2_uzd_Ctrl0_REG.output_fmt = 1;
		//	CLR_422_Fr_SD();

		rtd_outl(SCALEDOWN_ICH2_UZD_Ctrl0_reg, ich2_uzd_Ctrl0_REG.regValue);
		rtd_outl(SCALEDOWN_ICH2_UZD_Ctrl1_reg, ich2_uzd_Ctrl1_REG.regValue);
		rtd_outl(SCALEDOWN_ICH2_UZD_H_Output_Size_reg, ich2_uzd_H_Output_Size_REG.regValue);
		rtd_outl(SCALEDOWN_ICH2_UZD_Scale_Hor_Factor_reg, ich2_uzd_Scale_Hor_Factor_REG.regValue);
		rtd_outl(SCALEDOWN_ICH2_UZD_Scale_Ver_Factor_reg, ich2_uzd_Scale_Ver_Factor_REG.regValue);
		rtd_outl(SCALEDOWN_ICH2_UZD_Initial_Value_reg, ich2_uzd_Initial_Value_REG.regValue);
	}
	#endif//#ifdef CONFIG_DUAL_CHANNEL
#endif
}


void zoom_imd_smooth_color_ultrazoom_set_scale_down_di_hsd(unsigned char channel, SMOOTH_TOGGLE_SIZE* ptInSize, SMOOTH_TOGGLE_SIZE* ptOutSize, UINT8 panorama)
{
	unsigned char SDRatio;
	//unsigned char SDFilter=0;
//	unsigned int tmp_data;
	//signed short *coef_pt;

//	unsigned char i;
	unsigned long long nFactor,D;
	unsigned long long ulparam1 = 0;
	unsigned long long ulparam2 = 0;
	unsigned char Hini,  a;
	unsigned short S1,S2;
//	unsigned long long ullinsize;
//	unsigned long long ulloutsize;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	hsd_dither_di_hsd_ctrl0_RBUS					di_hsd_ctrl0_reg;
	hsd_dither_di_hsd_scale_hor_factor_RBUS		di_hsd_scale_hor_factor_reg;
	hsd_dither_di_hsd_initial_value_RBUS			di_hsd_initial_value_reg;
	hsd_dither_di_hsd_hor_segment_RBUS 		di_hsd_hor_segment_reg;

	di_hsd_ctrl0_reg.regValue = rtd_inl(HSD_DITHER_DI_HSD_Ctrl0_reg);
	di_hsd_hor_segment_reg.regValue = rtd_inl(HSD_DITHER_DI_HSD_Hor_Segment_reg);
    di_hsd_initial_value_reg.regValue = rtd_inl(HSD_DITHER_DI_HSD_Initial_Value_reg);

	//	g_ucIsHScalerDown = 0;
	//	g_ucIsVScalerDown = 0;
	//	g_ucIsH444To422 = 0;

	#define TMPMUL	(16)

	//o============ H scale-down=============o
	if(ptInSize->nWidth > ptOutSize->nWidth) //fix 3d
	{
		if ( ptOutSize->nWidth == 0 ) {
			rtd_pr_smt_debug("output width = 0 !!!\n");
			SDRatio = 0;
		} else {
			SDRatio = (ptInSize->nWidth*TMPMUL) / ptOutSize->nWidth;
		}

		if(SDRatio <= ((TMPMUL*3)/2))	//<1.5 sharp, wider bw
			;//SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) ) // Mid
			;//SDFilter = 1;
		else		// blurest, narrow bw
			;//SDFilter = 0;

		//o---------------- fill coefficients into access port--------------------o
		//coef_pt = tScaleDown_COEF_TAB[SD_H_Coeff_Sel[SDFilter]];

		if(channel == _CHANNEL1)
		{
#if 0	/* do not change table in zoom smooth toggle, this filter will be set in PQ flow*/
			if(di_hsd_flag)
			{//go DI path
				di_hsd_ctrl0_reg.h_y_table_sel = 0; // TAB1
				di_hsd_ctrl0_reg.h_c_table_sel = 0; // TAB1
			}
			for (i=0; i<16; i++)
			{
				tmp_data = ((unsigned int)(*coef_pt++)<<16);
				tmp_data += (unsigned int)(*coef_pt++);
				if(di_hsd_flag)
				{//go DI path
					rtd_outl(HSD_DITHER_DI_UZD_FIR_Coef_Tab1_C0_reg+ i*4, tmp_data);
				}
			}
			//		g_ucIsHScalerDown = 1;
			//		g_ucSDFHSel = SDFilter;
#else
			/*do nothing*/
			;
#endif

		}
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	// Decide Output data format for scale down
	//============H Scale-down============
	if(channel == _CHANNEL1)
	{
		if (ptInSize->nWidth > ptOutSize->nWidth) //fix 3d
		{	 // H scale-down
			Hini = 0x80;//0xff;//0x78;

			di_hsd_initial_value_reg.hor_ini = Hini;
			di_hsd_initial_value_reg.hor_ini_int= 1;
			a = 5;

			if(panorama && (!imdSmoothInfo[channel].IMD_SMOOTH_H_SCALE_UP))
			{  // CSW+ 0960830 Non-linear SD
				S1 = (ptOutSize->nWidth) >> 2;
				S2 = (ptOutSize->nWidth) - S1*2;
				//=vv==old from TONY, if no problem, use this=vv==
				////removed rika20150904 for 4k2k issue
				////nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13));
				ulparam1 = ((unsigned long long)(ptInSize->nWidth)<<21) - ((unsigned long long)Hini<<13);

				//=vv==Thur debugged=vv==
				//nFactor = ((((unsigned long long)(ptInSize->nWidth))*(ptOutSize->nWidth-1))<<21) / ptOutSize->nWidth;
				//===================
				////removed rika20150904 for 4k2k issue
				////nFactor = nFactor /((2*a+1)*S1 + (a+1)*S2 - a) * a;
				////D = nFactor / a / S1;
				// coverity 120588,120587 : rikahsu 20151217
				// transform to unsigned long long before - to prevent sign extension
				ulparam2=((2*a+1)*((unsigned long long)S1) + (a+1)*((unsigned long long)S2) - a);
				do_div(ulparam1,ulparam2);
				nFactor=ulparam1*a;

				ulparam1=nFactor;
				do_div(ulparam1,a);
				do_div(ulparam1,S1);
				D=ulparam1;

				nFactor = SHR((nFactor + 1), 1); //rounding
				D = SHR(D + 1, 1); //rounding

				if(nFactor < 1048576) {
					//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13)) / (ptOutSize->nWidth-1);
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)(((ptInSize->nWidth)<<21)) / (ptOutSize->nWidth);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<21;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
					nFactor = SHR(nFactor + 1, 1); //rounding
					S1 = 0;
					S2 = ptOutSize->nWidth;
					D = 0;
				}
			}
			else{

				//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) - ((unsigned int)Hini<<13)) / (ptOutSize->nWidth-1);
				if ( ptOutSize->nWidth == 0 ) {
					rtd_pr_smt_debug("output width = 0 !!!\n");
					nFactor = 0;
				}
				else if(ptInSize->nWidth>4095){
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)((((ptInSize->nWidth-1)<<19) / (ptOutSize->nWidth-1))<<1);
					//ulparam1=(unsigned long long)(ptInSize->nWidth-1)<<19;
					//ulparam2=(unsigned long long)(ptOutSize->nWidth-1);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<19;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
					nFactor=nFactor<<1;
				}
				else if(ptInSize->nWidth>2047){
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)(((ptInSize->nWidth-1)<<20) / (ptOutSize->nWidth-1));
					//ulparam1=(unsigned long long)(ptInSize->nWidth-1)<<20;
					//ulparam2=(unsigned long long)(ptOutSize->nWidth-1);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<20;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
				}
				else {
					////removed rika20150904 for 4k2k issue
					////nFactor = (unsigned int)(((ptInSize->nWidth-1)<<21) / (ptOutSize->nWidth-1));
					//ulparam1=(unsigned long long)(ptInSize->nWidth-1)<<21;
					//ulparam2=(unsigned long long)(ptOutSize->nWidth-1);
					ulparam1=(unsigned long long)(ptInSize->nWidth)<<21;
					ulparam2=(unsigned long long)(ptOutSize->nWidth);
					do_div(ulparam1,ulparam2);
					nFactor=ulparam1;
					//nFactor = (unsigned int)(((ptInSize->nWidth)<<21) / (ptOutSize->nWidth));//old
					nFactor = SHR(nFactor + 1, 1); //rounding
				}

				//rtd_pr_smt_debug("nFactor=%d, input Wid=%d, Out Wid=%d\n",nFactor, ptInSize->nWidth, ptOutSize->nWidth);
				S1 = 0;
				S2 = ptOutSize->nWidth;
				D = 0;
			}
		}else {
			nFactor = 0x100000;
			S1 = 0;
			S2 = ptOutSize->nWidth;
			D = 0;
		}

		if(di_hsd_flag)
		{//go DI path
			di_hsd_scale_hor_factor_reg.hor_fac = nFactor;
			di_hsd_hor_segment_reg.nl_seg2 = S2;
			di_n1_seg1=S1;
			di_n1_seg2=S2;
		}
		//rtd_pr_smt_debug("nFactor=%x, input_Wid=%d, Output_Wid=%d\n",nFactor, ptInSize->nWidth, ptOutSize->nWidth);

		if(di_hsd_flag)
		{//go DI path
			rtd_outl(HSD_DITHER_DI_HSD_Ctrl0_reg, di_hsd_ctrl0_reg.regValue);
			rtd_outl(HSD_DITHER_DI_HSD_Initial_Value_reg, di_hsd_initial_value_reg.regValue);
			rtd_outl(HSD_DITHER_DI_HSD_Scale_Hor_Factor_reg, di_hsd_scale_hor_factor_reg.regValue);
			rtd_outl(HSD_DITHER_DI_HSD_Hor_Segment_reg, di_hsd_hor_segment_reg.regValue);
		}
	}
}


void zoom_imd_smooth_config_uzd_uzu_param(unsigned char display)
{
	unsigned int usAfterDILen;
	unsigned short temp=0;

	rtd_pr_smt_debug("file=%s, function=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__);

	if(imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
	{
		if(imdSmoothInfo[display].IMD_SMOOTH_DI_CUT_4_LINES)
		{
			usAfterDILen  = (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN<< 1) - 4;
		}
		else
		{
			usAfterDILen  = (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN<< 1);
		}
	}
	else
	{
		usAfterDILen = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN;
	}

	if(apply_disp_vsize[display] > apply_crop_vsize[display])
		temp = zoom_crop_vsize[display];		// Scale Up
	else
		temp = (unsigned short)(apply_crop_vsize[display]);
	rtd_pr_smt_emerg("_rk [zoom] vsize temp=%d\n", temp);

	temp = (unsigned short)(apply_crop_vsize[display]);
	rtd_pr_smt_emerg("_rk [zoom] vsize temp=%d\n", temp);
	imdSmoothInfo[display].IMD_SMOOTH_DI_WID = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID;
//end of DI_WID
	imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_LEN
			= imdSmoothInfo[display].IMD_SMOOTH_CapLen = temp;
//end of MEM LEN

	if(apply_disp_hsize[display] > apply_crop_hsize[display])
		temp = zoom_crop_hsize[display];		// Scale Up
	else
		temp = (unsigned short)(apply_crop_hsize[display]);
	rtd_pr_smt_emerg("_rk [zoom] hsize temp=%d\n", temp);

#ifdef CONFIG_ENABLE_VD_27MHZ_MODE
			// VD 27M (1440 Scaling down to 960 to use DI-5A)
			if(VD_27M_OFF == fw_video_get_27mhz_mode(SLR_MAIN_DISPLAY))
			{
				//do nothing
			}
			else// if(VD_27M_OFF != fw_video_get_27mhz_mode(SLR_MAIN_DISPLAY))
			{
				if(temp > 960)
				{
					rtd_pr_smt_info("VDC 27M, set SLR_INPUT_CAP_WID, SLR_INPUT_MEM_ACT_WID from %d to 960\n", temp);
					temp = 960;
				}
			}
#endif //#ifdef CONFIG_ENABLE_VD_27MHZ_MODE

	imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID
				= imdSmoothInfo[display].IMD_SMOOTH_CapWid = temp;
//end of WID

//-------------------------------end of setting info
//begin of setting scale up/down info

/*
	  rtd_pr_smt_info("IMD_SMOOTH_CapWid = %d \n", imdSmoothInfo[display].IMD_SMOOTH_CapWid);
	  rtd_pr_smt_info("IMD_SMOOTH_CapLen = %d \n", imdSmoothInfo[display].IMD_SMOOTH_CapLen);
	  rtd_pr_smt_info("IMD_SMOOTH_DispWid = %d \n", imdSmoothInfo[display].IMD_SMOOTH_DispWid);
	  rtd_pr_smt_info("IMD_SMOOTH_DispLen = %d \n", imdSmoothInfo[display].IMD_SMOOTH_DispLen);
*/
	 if(imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_WID > imdSmoothInfo[display].IMD_SMOOTH_CapWid)
		  imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_DOWN = 1;
	  else
		  imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_DOWN = 0;

	  if(usAfterDILen > imdSmoothInfo[display].IMD_SMOOTH_CapLen)
		  imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_DOWN = 1;
	  else
		  imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_DOWN = 0;

	if(imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID <
			imdSmoothInfo[display].IMD_SMOOTH_DispWid)
		imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_UP = 1;
    else if((vbe_disp_oled_orbit_mode ==_VBE_PANEL_ORBIT_AUTO_MODE && vbe_disp_oled_orbit_enable &&  Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC)))
        imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_UP = 1;
	else
		imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_UP = 0;

	if(imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_LEN <
			imdSmoothInfo[display].IMD_SMOOTH_DispLen)
		imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_UP = 1;
    else if((vbe_disp_oled_orbit_mode ==_VBE_PANEL_ORBIT_AUTO_MODE && vbe_disp_oled_orbit_enable &&  Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC)))
        imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_UP = 1;
	else
		imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_UP = 0;

/*
	rtd_pr_smt_emerg("acthsta =%d,actvsta=%d\n", imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_STA, imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_STA);
	rtd_pr_smt_emerg("actwid =%d,actlen=%d\n", imdSmoothInfo[display].IMD_SMOOTH_SDNRH_ACT_WID, imdSmoothInfo[display].IMD_SMOOTH_SDNRV_ACT_LEN);
	rtd_pr_smt_emerg("DI_WID=(%d), DI_LEN=(%d)\n", imdSmoothInfo[display].IMD_SMOOTH_DI_WID);
	rtd_pr_smt_emerg("CapLen = (%d), CapWid = (%d)\n", imdSmoothInfo[display].IMD_SMOOTH_CapLen, imdSmoothInfo[display].IMD_SMOOTH_CapWid);
	rtd_pr_smt_emerg("MEM_ACT_LEN = (%d), MEM_ACT_WID =(%d)\n", imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_LEN, imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID);
	rtd_pr_smt_emerg("DispLen = (%d), DispWid = (%d)\n", imdSmoothInfo[display].IMD_SMOOTH_DispLen, imdSmoothInfo[display].IMD_SMOOTH_DispWid);

	rtd_pr_smt_emerg("IV_FREQ = 0x%x(%d)\n", imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000, imdSmoothInfo[display].IMD_SMOOTH_V_FREQ_1000);

	rtd_pr_smt_emerg("IMD_SMOOTH_H_SCALE_UP=%d,IMD_SMOOTH_V_SCALE_UP=%d\n",
	imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_UP, imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_UP);

	rtd_pr_smt_emerg("IMD_SMOOTH_H_SCALE_DOWN=%d,IMD_SMOOTH_V_SCALE_DOWN=%d\n",
	imdSmoothInfo[display].IMD_SMOOTH_H_SCALE_DOWN, imdSmoothInfo[display].IMD_SMOOTH_V_SCALE_DOWN);
	rtd_pr_smt_emerg("10bit:%d,422cap=%d,rtnr=%d\n",imdSmoothInfo[display].IMD_SMOOTH_DISP_10BIT,imdSmoothInfo[display].IMD_SMOOTH_DISP_422CAP,imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR);
	rtd_pr_smt_emerg("framesync=%d,thrip=%d\n",imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC,imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP);
  */
}



void zoom_imd_smooth_decide_rtnr(unsigned char display)
{

	//added by rika20151120
	//let disp_rtnr be the same as scaler setting
	imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = Scaler_DispGetStatus(display, SLR_DISP_RTNR);//Scaler_DispGetInputInfoByDisp(display, SLR_DISP_RTNR);
	return;
	#if 0
	//remove by rika 20151120
		if (display == SLR_SUB_DISPLAY) {
			imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
			return;
		}

		/*=================================*/
		//main not enter RTNR for sub use when main & sub show on PR panel
		if((get_mainsubonpr_fuc() == TRUE)){
			imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
			return;
		}


		//Elsie 20130314, force it to enter RTNR if source is CVBS.
		if(Scaler_InputSrcGetType(display) == _SRC_CVBS)
		{
			imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = TRUE;
			rtd_pr_smt_debug("[CVBS] RTNR ON\n");
			//frank@1008 mark below code to solve PIP flow error
			//IoReg_SetBits(VGIP_Data_Path_Select_reg, _BIT2);
			return;
		}
		#ifdef CONFIG_RTNR_4K2K
		// [#0048022] 4k1k/4k2k video ratio mode 4:3 will cause RTNR fail
		if((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID > 2048)
				&& (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN >= 1080))
		{	// 4k1k/4k2k
			//drv_memory_set_ip_DMA420_mode(SLR_DMA_422_mode);
			bIpDma420Mode= SLR_DMA_420_mode;

			//frank@20130604 add below patch code for mantis#40619 solve DI readstart bits not enough problem
			if((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID-imdSmoothInfo[display].IMD_SMOOTH_CapWid) < 960)
				imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = TRUE;
			else
				imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
			return;
		}
		#endif



		#if 0
		//rika 20140614 remove below because if(Scaler_InputSrcGetMainChType() != _SRC_VO) return false
		// [FIX-ME] if need to do vertical scaling up then speed up d-clock to avoid frame sync fail
		// disable DI/RTNR to reduce bandwidth usage
		if(_check_disable_RTNR_for_savingBW() == TRUE){//if(Scaler_InputSrcGetMainChType() != _SRC_VO) return false
			imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_RTNR=FALSE;
			rtd_pr_smt_debug("[DBG] NR OFF\n");
			return;
		}
		#endif



		#if 0//need review, maybe no need
		if (Scaler_PipGetInfo(SLR_PIP_ENABLE) && modestate_decide_double_dvs_enable()) { //avoid memory allocate is not enough
			// Only main display can enter GIBI and RTNR should be closed
			imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_RTNR=FALSE;
			return;
		}
		#endif

		#ifdef CONFIG_DUAL_CHANNEL //need review
		// Sub doesn't enter RTNR.
		if (display == SLR_SUB_DISPLAY) {
			imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_RTNR = FALSE;
			return;
		}
		#endif

		// [#0048022] 4k1k/4k2k video ratio mode 4:3 will cause RTNR fail
		if((imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN > 2160)||
				(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID > 3840) )
		{//3   		//input frame paking case avoid scaler memory not enough
			imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
			rtd_pr_smt_debug("[4k2k] RTNR OFF in 4k2k\n");
			return;
		}

		if (Scaler_InputSrcGetType(display) != _SRC_VGA) {
			if ( (Scaler_InputSrcGetFrom(display)==_SRC_FROM_TMDS) && (!drvif_IsHDMI()) ) // DVI don't enter RTNR
				imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
			else{
				///rtd_pr_smt_debug("_rk[zoom][rtnr] %s %d\n",__FUNCTION__,__LINE__);
				imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = TRUE;
			}
		} else { // VGA don't enter RTNR
			imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
		}

		//vflip rtnr check again
		#ifdef CONFIG_DUAL_CHANNEL
		// if main needs RTNR but sub allocates DI, we have to release sub.
		//if (Scaler_DispGetStatus(SLR_MAIN_DISPLAY,SLR_DISP_RTNR)) {
		if(imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_RTNR){
				//if (Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_THRIP)||Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_RTNR)) {
				if(imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_THRIP || imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_RTNR){
					///Scaler_ForceBg(SLR_SUB_DISPLAY, SLR_FORCE_BG_SYSTEM, TRUE);//
					fw_scalerip_disable_ip(SLR_SUB_DISPLAY);
					//frank@0528 Sove QCBUG10151 push below to set sub channel i,m,d domain to avoid sub set display but DI disable by main channel
					//flow_message_push(_MODE_MSG_SUB_SET_VGIP_AFTER_FOUND_MODE_SIGNAL);
					//flow_message_clearallType(_MODE_MSG_SUB_SETUP_BEFORE_SHOW_SIGNAL);
					//mark by frank@0528 flow_message_push(_MODE_MSG_SUB_SETUP_AFTER_FOUND_MODE_SIGNAL);
					//Scaler_DispSetStatus(SLR_SUB_DISPLAY,SLR_DISP_THRIP,FALSE);
					imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_THRIP = FALSE;
					//Scaler_DispSetStatus(SLR_SUB_DISPLAY,SLR_DISP_RTNR,FALSE);
					imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_RTNR = FALSE;
				}
			}
		#endif


	#endif //if 0

}

void zoom_imd_smooth_get_display_size(unsigned char display)
{
	unsigned int m_disp_wid = 0;
	imdSmoothInfo[display].IMD_SMOOTH_DispWid = zoom_disp_hsize[display];
	imdSmoothInfo[display].IMD_SMOOTH_DispLen = zoom_disp_vsize[display];

	m_disp_wid = imdSmoothInfo[display].IMD_SMOOTH_DispWid;
	if((m_disp_wid%2) > 0){ //fix m_disp_wid, need to be even rika 20140614
		m_disp_wid = (m_disp_wid & (~1))+2;
	}
	imdSmoothInfo[display].IMD_SMOOTH_DispWid = m_disp_wid;
}



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


	#if 0
	//no need to reset parameters now.
	if(rcflag == _ENABLE){
		rtd_pr_smt_debug("_rk[zoom]reset crop@set_info\n");
		zoom_crop_hsize[display] = INPUT_WID;
		zoom_crop_vsize[display] = INPUT_LEN;
		zoom_crop_hpos[display] = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPH_ACT_STA_PRE);
		zoom_crop_vpos[display] = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_IPV_ACT_STA_PRE);

	}
	if(rdflag == _ENABLE){
		rtd_pr_smt_debug("_rk[zoom]reset disp@set_info\n");
		zoom_disp_hsize[display] = DISP_WID;
		zoom_disp_vsize[display] = DISP_LEN;
		zoom_disp_hpos[display] = 0;
		zoom_disp_vpos[display] = 0;

	}

	#endif

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

/*
   void zoom_smooth_toggle_state_info_rpc
   added by rika 20140627
   for fpp tcl api vo source
   */

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
		//merlin8/8p mdomain one-bin
		//if ((display == SLR_MAIN_DISPLAY) && drv_memory_Get_multibuffer_flag()) {
		if ((display == SLR_MAIN_DISPLAY) && (get_mach_type() == MACH_ARCH_RTK2885P && nonlibdma_drv_memory_Get_multibuffer_flag())) {
			//mdomain_cap_ddr_in1ctrl_RBUS mdomain_cap_ddr_in1ctrl_reg;
			unsigned int time_out=40;
			unsigned char buffer_match=0;
			while (time_out) {
				//mdomain_cap_ddr_in1ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Ctrl_reg);
				//merlin8/8p mdomain one-bin
				if(get_mach_type() == MACH_ARCH_RTK2885P) {
					//buffer_match=((drv_memory_Get_multibuffer_number()-1) != mdomain_cap_ddr_in1ctrl_reg.in1_bufnum) ? 1: 0;
					buffer_match=((nonlibdma_drv_memory_Get_multibuffer_number()-1) != nonlibdma_get_cap_ddr_inctrl_reg_bufnum(0)) ? 1: 0;
				}
				//rtd_pr_smt_emerg("buffer_match=%d\n",buffer_match);
				//merlin8/8p mdomain one-bin
				//if ((buffer_match  == 0) && (drv_memory_Get_capture_multibuffer_flag() == 0)) {
				if ((buffer_match  == 0) && (get_mach_type() == MACH_ARCH_RTK2885P && nonlibdma_drv_memory_Get_capture_multibuffer_flag() == 0)) {
					break;
				}
				time_out--;
				msleep(10);
			}
			if (time_out == 0) {
				rtd_pr_smt_emerg("multibuffer caputure timeout\n");
			} else {
				//mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
				unsigned int time_out_double_buffer = 0x1fff;
				//ddr_mainsubctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
				//merlin8/8p mdomain one-bin
				if(get_mach_type() == MACH_ARCH_RTK2885P) {
					//if (ddr_mainsubctrl_reg.disp1_double_enable) {
					if (nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_enable(0)) {
						while(time_out_double_buffer) {
							//ddr_mainsubctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
							//if (ddr_mainsubctrl_reg.disp1_double_apply == 0) {
							if (nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_apply(0) == 0) {
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
		#if 0
		//rika20151109
		//do not calculate adjustment here anymore. this will reult in wrong cal when setinput comes before realvoinfo
		//this results in wrong cal when the voinfo is old. therefore we need to send setinput arguments directly to video fw

		zoom_info.zoom_crop_adj_hpos=crop_hpos-imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE;
		zoom_info.zoom_crop_adj_vpos=crop_vpos-imdSmoothInfo[display].IMD_SMOOTH_V_ACT_STA_PRE;
		if(get_zoom_vgip_adjust_enable())
		{
			zoom_info.zoom_crop_adj_hsize=crop_hsize-get_zoom_smoothtoggle_vo_hwid(display);
			zoom_info.zoom_crop_adj_vsize=crop_vsize-get_zoom_smoothtoggle_vo_vlen(display);
		}
		else
		{
			zoom_info.zoom_crop_adj_hsize=crop_hsize-imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE;
			zoom_info.zoom_crop_adj_vsize=crop_vsize-imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE;
		}
		#endif

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
		zoom_info.videoPlane=Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_VODMA_PLANE);//(Scaler_DispGetInputInfo(SLR_INPUT_THROUGH_I3DDMA) && (Scaler_Get_CurVoInfo_plane() == VO_VIDEO_PLANE_V1)? Scaler_Get_3D_VoPlane(): Scaler_Get_2D_VoPlane());
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
	//rtd_pr_smt_err("[kernel][rpc]zoom_crop:%d , %d, %d, %d\n",apply_crop_hpos[display],  apply_crop_vpos[display], apply_crop_hsize[display], apply_crop_vsize[display]);
	//rtd_pr_smt_err("[kernel][rpc]zoom_disp:%d , %d, %d, %d\n",disp_hpos,  disp_vpos, disp_hsize, disp_vsize);
	//rtd_pr_smt_err("[kernel][rpc]newAdaptive :%d\n",Scaler_AdaptiveStream_NewInputRegionFlagGet(display));
	//rtd_pr_smt_err("[kernel][rpc]display:%d, plane:%d\n",display,zoom_info.videoPlane);
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


#if 0
		rtd_pr_smt_debug("_rk crop_adj_hpos:%d\n",zoom_info.zoom_crop_adj_hpos);
		rtd_pr_smt_debug("_rk crop_adj_vpos:%d\n",zoom_info.zoom_crop_adj_vpos);

		rtd_pr_smt_debug("_rk crop_adj_hsize:%d\n",zoom_info.zoom_crop_adj_hsize);
		rtd_pr_smt_debug("_rk crop_adj_vsize:%d\n",zoom_info.zoom_crop_adj_vsize);
#endif
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
#if 0
	while((ret = Scaler_SendRPC(SCALERIOC_GETZOOMSTATE, 0, 1))==ZOOM_VOSTATE_NONE)
	{//wait to enter smooth toggle
		if(timeout==10)break;
		//rtd_pr_smt_debug("_rk[vo] waiting enter timeout:%d\n",timeout);
		timeout++;
		Rt_Sleep(50);

	}

	if((ret = Scaler_SendRPC(SCALERIOC_GETZOOMSTATE, 0, 1))==ZOOM_VOSTATE_NONE){
		rtd_pr_smt_debug("_rk[vo]check ret:%d\n",ret);
		ret = ZOOM_VOSTATE_OK;
		Scaler_Set_VOsmoothenable(_DISABLE);
		//if smoothtoggle doesn't start, it may be @ no-signal condition return ok anyway
		return ret;
	}


	timeout=0;

	while((ret = Scaler_SendRPC(SCALERIOC_GETZOOMSTATE, 0, 1))<0){
		//if ZOOM_VOSTATE_START, wait to get ok or fail
		rtd_pr_smt_debug("_rk[vo]waiting apply timeout:%d\n",timeout);
		timeout++;
		Rt_Sleep(50);

	}

	ret = Scaler_SendRPC(SCALERIOC_GETZOOMSTATE, 0, 1);
	rtd_pr_smt_debug("_rk[vo]check ret:%d\n",ret);
#if 0
	if(ret==ZOOM_STATE_FAIL)
		set_zoom_reset_double_state(_DISABLE);
#endif

	return ret;
#endif

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

//extern DISPLAY_DATA_SMOOTH_TOGGLE vo_force_data_mode_get_enable(unsigned char display);
extern SOURCE_INFO_SMOOTH_TOGGLE get_vo_smoothtoggle_state(unsigned char display);
//extern void Set_vo_smoothtoggle_changeratio_flag2(unsigned char enable, unsigned char display);
#if 0
void zoom_imd_smooth_toggle_vo_enable(unsigned char enable, unsigned char display)
{

	if(enable==_ENABLE){
		pre_vo_data_mode=vo_force_data_mode_get_enable(display);
		pre_smooth_state=get_vo_smoothtoggle_state(display);
		Set_vo_smoothtoggle_changeratio_flag2(_ENABLE, display);//enable_smooth_toggle_vo_changeratio=_ENABLE;

	}
	else{
		vo_force_data_mode_set_enable(pre_vo_data_mode, display);
		///set_vo_smoothtoggle_state(pre_smooth_state);
		set_zoom_reset_double_state(_ENABLE,display);// means clear double buffer later
		Set_vo_smoothtoggle_changeratio_flag2(_DISABLE, display);//enable_smooth_toggle_vo_changeratio=_DISABLE;
		//zoom_smooth_toggle_set_finish(ZOOM_STATE_NONE);
	}
}
#endif
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
		//need review, rika 20140711
		//pthread_mutex_lock(&scalerMutex); //mark this because it can't be run in scaler task ex. modestate.cpp active_process_msg

		//--------------

		//vo_smoothtoggle_memory_alloc();//rika marked this cause it's done by scaler init
		//panel_parameter_send_info();
//		zoom_imd_smooth_toggle_vo_init(display);


		///set_vo_smoothtoggle_state(SLR_SMOOTH_TOGGLE_ACTIVE);

		zoom_smooth_toggle_state_info_rpc(display,
				crop_hpos,  crop_vpos,
				crop_hsize, crop_vsize,
				disp_hpos,  disp_vpos,
				disp_hsize, disp_vsize );

		//--------------




		//pthread_mutex_unlock(&scalerMutex);

		//set_vo_smoothtoggle_state(SLR_SMOOTH_TOGGLE_INIT);

		return ZOOM_STATE_OK;
	}

void zoom_update_info_from_vo(void)
{
	//write 2 or 0 to buffer_mode so far rika 20141021
	if(Get_vo_smoothtoggle_dispstatus(SCALERIOC_GET_V_UZD_BUFFER_MODE))
	{
		///rtd_pr_smt_debug("_rk [zoom]buffer_mode:2\n");
		Scaler_DispSetStatus((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY, SLR_DISP_FSYNC_VUZD, VUZD_BUFFER_VUZD_MODE);

	}
	else{
		Scaler_DispSetStatus((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY, SLR_DISP_FSYNC_VUZD, VUZD_BUFFER_BYPASS_MODE);

	}

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

				//merlin8/8p mdomain one-bin
				//if((display == SLR_MAIN_DISPLAY) && ((is_scaler_input_2p_mode(display)) || (force_enable_mdomain_2disp_path())))
				if((display == SLR_MAIN_DISPLAY) && (get_mach_type() == MACH_ARCH_RTK2885P && ((is_scaler_input_2p_mode(display)) || (force_enable_mdomain_2disp_path()))))
				{//4k2k120 or VRR60Hz case copy m domain information to sub
					Scaler_DispSetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_WID, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID));
					Scaler_DispSetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN));
					Scaler_DispSetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_WID, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_WID));
					Scaler_DispSetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_LEN, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN));
					Scaler_DispSetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_HSTA, 0);
					Scaler_DispSetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_VSTA, 0);
					Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_V_DOWN, Scaler_DispGetScaleStatus(SLR_MAIN_DISPLAY, SLR_SCALE_V_DOWN));
					Scaler_DispSetScaleStatus(SLR_SUB_DISPLAY, SLR_SCALE_H_DOWN, Scaler_DispGetScaleStatus(SLR_MAIN_DISPLAY, SLR_SCALE_H_DOWN));
				}

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

#ifdef CONFIG_ALL_SOURCE_DATA_FS
extern unsigned char get_i3dma_uzd(void);
#endif
void zoom_imd_smooth_keep_applyinfo(unsigned int  un32_windowId,
		unsigned int  un32_hposition, unsigned int  un32_vposition,
		unsigned int  un32_hsize, unsigned int  un32_vsize,unsigned char zoomfunc)
{

//	apply_windowId=un32_windowId;
//	apply_zoomfunc=zoomfunc;

//	imd_smooth_set_curr_display(apply_windowId);
#ifdef CONFIG_ALL_SOURCE_DATA_FS
		VSC_INPUT_TYPE_T srctype = Get_DisplayMode_Src(un32_windowId);
		unsigned int hdmi_mode_wid=0,hdmi_mode_len=0;
#endif

	if(zoomfunc==ZOOM_CROP){
#ifdef CONFIG_ALL_SOURCE_DATA_FS
		if ((srctype == VSC_INPUTSRC_HDMI) && get_i3dma_uzd()) {
			hdmi_mode_wid = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHWID);
			hdmi_mode_len = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVHEIGHT);
			if (hdmi_mode_wid >1920) {
				un32_hposition = un32_hposition * 1920/hdmi_mode_wid;
				un32_hsize = un32_hsize * 1920/hdmi_mode_wid;
			}
			if (hdmi_mode_len >1080) {
				un32_vposition = un32_vposition * 1080/hdmi_mode_len;
				un32_vsize = un32_vsize * 1080/hdmi_mode_len;
			}
		}
#endif
		apply_crop_vsize_pre[un32_windowId]=apply_crop_vsize[un32_windowId];
		apply_crop_vpos_pre[un32_windowId]=apply_crop_vpos[un32_windowId];
		apply_crop_hpos[un32_windowId]=un32_hposition;
		apply_crop_vpos[un32_windowId]=un32_vposition;
		apply_crop_hsize[un32_windowId] =un32_hsize;
		apply_crop_vsize[un32_windowId]=un32_vsize;
		rtd_pr_smt_debug("apply_crop_vsize_pre=%d, apply_crop_vpos_pre=%d\n", apply_crop_vsize_pre[un32_windowId], apply_crop_vpos_pre[un32_windowId]);
		rtd_pr_smt_debug("apply_crop_hpos=%d, apply_crop_vpos=%d\n", apply_crop_hpos[un32_windowId], apply_crop_vpos[un32_windowId]);
		rtd_pr_smt_debug("apply_crop_hsize=%d, apply_crop_vsize=%d\n", apply_crop_hsize[un32_windowId], apply_crop_vsize[un32_windowId]);
	}else if(ZOOM_CROP_TIMING == zoomfunc){
		apply_timing_hpos[un32_windowId]=un32_hposition;
		apply_timing_vpos[un32_windowId]=un32_vposition;
		apply_timing_hsize[un32_windowId] =un32_hsize;
		apply_timing_vsize[un32_windowId]=un32_vsize;
		rtd_pr_smt_debug("apply_timing_hpos=%d, apply_timing_vpos=%d\n", apply_timing_hpos[un32_windowId], apply_timing_vpos[un32_windowId]);
		rtd_pr_smt_debug("apply_timing_hsize=%d, apply_timing_vsize=%d\n", apply_timing_hsize[un32_windowId], apply_timing_vsize[un32_windowId]);
	}else if (zoomfunc==ZOOM_DISP){
		apply_disp_hpos[un32_windowId]=un32_hposition;
		apply_disp_vpos[un32_windowId]=un32_vposition;
		apply_disp_hsize[un32_windowId] =un32_hsize;
		apply_disp_vsize[un32_windowId]=un32_vsize;
		rtd_pr_smt_debug("apply_disp_hpos=%d, apply_disp_vpos=%d\n", apply_disp_hpos[un32_windowId], apply_disp_vpos[un32_windowId]);
		rtd_pr_smt_debug("apply_disp_hsize=%d, apply_disp_vsize=%d\n", apply_disp_hsize[un32_windowId], apply_disp_vsize[un32_windowId]);
	}

	//disp width has to align 4 @Crixus 20150827
	if((apply_disp_hsize[un32_windowId]%4)>0){
		apply_disp_hsize[un32_windowId] = (apply_disp_hsize[un32_windowId]&(~3))+4;
	}
}

#if 0
void zoom_setcropflag(unsigned char enable)
{
	resetcropflag=enable;
}
unsigned char zoom_getcropflag(void)
{
	return resetcropflag;
}
void zoom_setdispflag(unsigned char enable)
{
	resetdispflag=enable;
}

unsigned char zoom_getdispflag(void)
{
	return resetdispflag;
}


void zoom_checkresetflag(unsigned char display, unsigned char zoomfunc)
{
	unsigned char current_linein, current_volinein;
	current_linein=0,current_volinein=0;
	current_linein = Scaler_InputSrcGetType(display);


	if(pre_linein != current_linein)
	{//if source changed, reset zoom_first_crop,zoom_fist_dis, to prevent wrong crop/disp size by rika 20140630
		rtd_pr_smt_debug("_rk pre_linein:%d\n",pre_linein);

		pre_linein = current_linein;

		//reset cropflag,dispflag when src is changed
		resetcropflag = 1;
		resetdispflag = 1;



		if(Scaler_InputSrcGetType(display) ==_SRC_VO){
			pre_volinein = Scaler_DispGetInputInfoByDisp(Get_DisplayMode_Port(display), SLR_INPUT_VO_SOURCE_TYPE);
		}


	}
	else if(Scaler_InputSrcGetType(display) == _SRC_VO)
	{ //under vo->vo case, need to check if directvo<->dtv

		current_volinein = Scaler_DispGetInputInfoByDisp(Get_DisplayMode_Port(display), SLR_INPUT_VO_SOURCE_TYPE);
		if(pre_volinein != current_volinein){ //if directvo<->dtv
			rtd_pr_smt_debug("_rk pre_volinein:%d\n", pre_volinein);
			pre_volinein = current_volinein;

			//reset cropflag,dispflag when src is changed
			resetcropflag = 1;
			resetdispflag = 1;

		}
	}



	if(zoomfunc == ZOOM_CROP)
		resetcropflag = 0;
	if(zoomfunc == ZOOM_DISP)
		resetdispflag = 0;

	rtd_pr_smt_debug("_rk current_linein:%d\n", current_linein);
	rtd_pr_smt_debug("_rk current_volinein:%d\n", current_volinein);
	rtd_pr_smt_debug("_rk resetcrop/resetdisp:%d,%d\n", resetcropflag, resetdispflag);

}
#endif

int zoom_imd_smooth_toggle_config(unsigned char display, SCALER_VSC_INPUT_REGION_TYPE inputType, StructSrcRect inputRegion, StructSrcRect inputTiming, StructSrcRect outputRegion)
{
	int failflag=ZOOM_STATE_OK;
	VSC_INPUT_TYPE_T srctype;
#ifdef CONFIG_ALL_SOURCE_DATA_FS
	unsigned int hdmi_mode_wid=0,hdmi_mode_len=0;
#endif
	srctype = Get_DisplayMode_Src(display);

	//rtd_pr_smt_emerg("[SMT] zoom_imd_smooth_toggle_config!!\n");
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

	if(Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) {// (VSC_Check_Signal_Lock(display) == TRUE) {
		//zoom_smooth_toggle_set_finish(ZOOM_STATE_DONOW);
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
		//zoom_smooth_toggle_set_finish(ZOOM_STATE_NONE);
		return ZOOM_STATE_FAIL;
	}
}



unsigned char zoom_vgip_adjust_enable;
stVGIP_ADJUSTMENT_INFO get_zoom_vgip_adjust_data(unsigned char display)
{
	stVGIP_ADJUSTMENT_INFO zoom_adjustment_data = {.h_start=0, .v_start=0, .h_width=0, .v_length=0};
	zoom_adjustment_data.h_start=zoom_crop_hpos[display]; //zoom_crop_hpos-imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_H_ACT_STA_PRE;
	zoom_adjustment_data.v_start=zoom_crop_vpos[display]; //zoom_crop_vpos-imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_V_ACT_STA_PRE;
	zoom_adjustment_data.h_width=zoom_crop_hsize[display]; //zoom_crop_hsize-imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_H_ACT_WID_PRE;
	zoom_adjustment_data.v_length=zoom_crop_vsize[display]; //zoom_crop_vsize-imdSmoothInfo[Smooth_Toggle_dislay].IMD_SMOOTH_V_ACT_LEN_PRE;
	return zoom_adjustment_data;
}
unsigned char get_zoom_vgip_adjust_enable(void)
{
	return zoom_vgip_adjust_enable;
}
void set_zoom_vgip_adjust_enable(unsigned char enable)
{
	zoom_vgip_adjust_enable=enable;
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
//	zoom_update_info_from_vo();
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

void zoom_smoothtoggle_gamemode_check(unsigned char display, unsigned char is_gamemode)
{
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
	rtd_pr_smt_emerg("[GAME][%s][%d] smooth_toggle_game_mode_check_cnt = %d\n",__FUNCTION__,__LINE__,smooth_toggle_game_mode_check_cnt);
	//send rpc to video fw
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//drvif_memory_compression_rpc(display);
		nonlibdma_drvif_memory_compression_rpc(display);
	}
	if(is_gamemode){
		if(!(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC)))
		{
			//change to 3-buffer
			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//drv_memory_GameMode_Switch_TripleBuffer();
				nonlibdma_drv_memory_GameMode_Switch_TripleBuffer();
			}
			//disable FLL tsk flag
			if(vbe_disp_gamemode_use_fixlastline()==TRUE){
				modestate_set_fll_running_flag(_DISABLE);
			}
			//change to timing free run
			drivf_scaler_reset_freerun();
		}else if(zoom_smoothtoggle_gamemode_get_smt_status(SLR_MAIN_DISPLAY) == SMT_SMALL_TO_FULL_SCREEN_GOING){
			//game mode small window to full screen, switch to 3 buffer first
			//merlin8/8p mdomain one-bin
			if(get_mach_type() == MACH_ARCH_RTK2885P) {
				//drv_memory_GameMode_Switch_TripleBuffer();
				nonlibdma_drv_memory_GameMode_Switch_TripleBuffer();
			}
		}else if(zoom_smoothtoggle_gamemode_get_smt_status(SLR_MAIN_DISPLAY) == SMT_FULL_TO_SMALL_SCREEN_GOING){
			//Synchronize game mode flag with vcpu
			drv_update_game_mode_frc_fs_flag();
		}
	}
}

void zoom_smoothtoggle_gamemode_set_smt_status(unsigned char display,unsigned char value)//value:1 set, 0 clear
{
	SMOOTH_TOGGLE_STATUS_UNDER_GAME_MODE* smt_status_under_game_mode;
	ppoverlay_main_active_v_start_end_RBUS main_active_v_start_end_reg;
	Check_smooth_toggle_update_flag(display);//wait smt done then update status
	if(display == SLR_MAIN_DISPLAY){
		smt_status_under_game_mode = (SMOOTH_TOGGLE_STATUS_UNDER_GAME_MODE *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTH_TOGGLE_STATUS_UNDER_GAME_MODE);
		if(smt_status_under_game_mode == NULL){
			rtd_pr_smt_emerg("smt_status_under_game_mode is NULL!!!\n");
			return;
		}
		if(value == 0){
			smt_status_under_game_mode->status = Scaler_ChangeUINT32Endian(SMT_NONE_STATUS);
			rtd_pr_smt_emerg("[SMT]clear screen change status\n");
			return;
		}
		main_active_v_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_MAIN_Active_V_Start_End_reg);
		if(((main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta) < _DISP_LEN)){
			if(Scaler_DispWindowGet().src_height ==  _DISP_LEN){//small to full screen
				smt_status_under_game_mode->status = Scaler_ChangeUINT32Endian(SMT_SMALL_TO_FULL_SCREEN_GOING);
				rtd_pr_smt_emerg("[SMT]small to full screen\n");
			}else{//small to another small screen
				smt_status_under_game_mode->status = Scaler_ChangeUINT32Endian(SMT_SMALL_TO_SMALL_SCREEN_GOING);
				rtd_pr_smt_emerg("[SMT]small to small screen\n");
			}
		}else if(((main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta) == _DISP_LEN)
			&& (Scaler_DispWindowGet().src_height < _DISP_LEN)){//full to small screen
			smt_status_under_game_mode->status = Scaler_ChangeUINT32Endian(SMT_FULL_TO_SMALL_SCREEN_GOING);
			rtd_pr_smt_emerg("[SMT]full to small screen\n");
		}else if(((main_active_v_start_end_reg.mv_act_end - main_active_v_start_end_reg.mv_act_sta) == _DISP_LEN)
			&& (Scaler_DispWindowGet().src_height == _DISP_LEN)){//full to full screen
			rtd_pr_smt_emerg("[SMT]full to full screen\n");
		}
	}
}

SMT_STATUS_UNDER_GAME_MODE zoom_smoothtoggle_gamemode_get_smt_status(unsigned char display)
{
	SMOOTH_TOGGLE_STATUS_UNDER_GAME_MODE* smt_status_under_game_mode;
	SMT_STATUS_UNDER_GAME_MODE status = SMT_NONE_STATUS;
	if(display == SLR_MAIN_DISPLAY){
		smt_status_under_game_mode = (SMOOTH_TOGGLE_STATUS_UNDER_GAME_MODE *)Scaler_GetShareMemVirAddr(SCALERIOC_SMOOTH_TOGGLE_STATUS_UNDER_GAME_MODE);
		if(smt_status_under_game_mode == NULL){
			rtd_pr_smt_emerg("smt_status_under_game_mode is NULL!!!\n");
			return status;
		}
		status = Scaler_ChangeUINT32Endian(smt_status_under_game_mode->status);
		//rtd_pr_smt_emerg("smt_status_under_game_mode status=%d\n",status);
	}
	return status;
}

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
            //merlin8/8p mdomain one-bin
            //((outputregion_pre[display].srcx != apply_disp_hpos[display]) || (outputregion_pre[display].srcy != apply_disp_vpos[display] && !drv_memory_get_low_delay_game_mode_dynamic()))
            ((outputregion_pre[display].srcx != apply_disp_hpos[display]) || (outputregion_pre[display].srcy != apply_disp_vpos[display] && (get_mach_type() == MACH_ARCH_RTK2885P && !nonlibdma_drv_memory_get_low_delay_game_mode_dynamic())))
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
#ifdef CONFIG_I2RND_ENABLE
		if (!Scaler_I2rnd_get_timing_enable())
#endif
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
#ifdef CONFIG_I2RND_ENABLE
		if (!Scaler_I2rnd_get_timing_enable())
#endif
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
#ifdef CONFIG_I2RND_ENABLE
		if (!Scaler_I2rnd_get_timing_enable())
#endif
		{
			//UZD
			ich2_uzd_db_ctrl_reg.regValue =rtd_inl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg);
			ich2_uzd_db_ctrl_reg.h_db_en=1;
			ich2_uzd_db_ctrl_reg.h_db_apply=0;
			ich2_uzd_db_ctrl_reg.v_db_en=1;
			ich2_uzd_db_ctrl_reg.v_db_apply=0;
			rtd_outl(SCALEDOWN_ICH2_UZD_DB_CTRL_reg,ich2_uzd_db_ctrl_reg.regValue);
		}

		ich2_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(SCALEDOWN_ich2_sdnr_cutout_range_hor_reg);
		ich2_sdnr_cutout_range_hor_reg.hor_front= SDNR_size.srcx;
		ich2_sdnr_cutout_range_hor_reg.hor_width= SDNR_size.src_wid; // chip setting need to +1
		IoReg_Write32(SCALEDOWN_ich2_sdnr_cutout_range_hor_reg, ich2_sdnr_cutout_range_hor_reg.regValue);

		ich2_sdnr_cutout_range_ver0_reg.regValue = IoReg_Read32(SCALEDOWN_ich2_sdnr_cutout_range_ver0_reg);
		ich2_sdnr_cutout_range_ver0_reg.ver_front= SDNR_size.srcy;
		ich2_sdnr_cutout_range_ver0_reg.ver_height= SDNR_size.src_height;// chip setting need to +1
		IoReg_Write32(SCALEDOWN_ich2_sdnr_cutout_range_ver0_reg, ich2_sdnr_cutout_range_ver0_reg.regValue);

		//ich1_sdnr_cutout_range_ver1_reg.regValue = IoReg_Read32(SCALEDOWN_ich2_sdnr_cutout_range_ver1_reg);
		//ich1_sdnr_cutout_range_ver1_reg.ver_mid_top=0;	// inregion.y;
		//ich1_sdnr_cutout_range_ver1_reg.ver_mid_bot=0;	// inregion.y;
		//IoReg_Write32(SCALEDOWN_ich2_sdnr_cutout_range_ver1_reg, ich1_sdnr_cutout_range_ver1_reg.regValue);

		scaledown_ich2_uzd_ctrl0_reg.regValue = IoReg_Read32(SCALEDOWN_ICH2_UZD_Ctrl0_reg);
		scaledown_ich2_uzd_ctrl0_reg.cutout_en = 1;
		IoReg_Write32(SCALEDOWN_ICH2_UZD_Ctrl0_reg, scaledown_ich2_uzd_ctrl0_reg.regValue);

#ifdef CONFIG_I2RND_ENABLE
		if (!Scaler_I2rnd_get_timing_enable())
#endif
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
		ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
        ppoverlay_sub_den_h_start_width_RBUS ppoverlay_sub_den_h_start_width_Reg;
	ppoverlay_sub_den_v_start_length_RBUS ppoverlay_sub_den_v_start_length_Reg;
        ppoverlay_sub_background_h_start_end_RBUS ppoverlay_sub_background_h_start_end_Reg;
	    ppoverlay_sub_background_v_start_end_RBUS ppoverlay_sub_background_v_start_end_Reg;
        //mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;

        //mdomain_disp_ddr_subprevstart_RBUS mdomain_disp_ddr_subprevstart_Reg;

        StructSrcRect subWindow = Scaler_SubDispWindowGet();

		unsigned int DispHSta = subWindow.srcx;
		unsigned int DispHEnd = subWindow.src_wid + subWindow.srcx;
		unsigned int DispVSta = subWindow.srcy;
		unsigned int DispVEnd = subWindow.src_height + subWindow.srcy;

        rtd_pr_smt_debug("[display-shiftxy]  only Display XY change !!!\n");

		Check_smooth_toggle_update_flag(display);

		//mdomain_disp_ddr_subprevstart_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubPreVStart_reg);
		//mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = (Get_DISP_DEN_STA_VPOS() - 3 + DispVSta);
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//mdomain_disp_ddr_subprevstart_Reg.sub_pre_rd_v_start = SUB_MDOM_PREREAD_START;//KTASKWBS-21489 k8 change to 7
			//IoReg_Write32(MDOMAIN_DISP_DDR_SubPreVStart_reg, mdomain_disp_ddr_subprevstart_Reg.regValue);
			nonlibdma_set_disp_prevstart_reg_pre_rd_v_start(1, SUB_MDOM_PREREAD_START);
		}
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//ddr_mainsubctrl_reg.regValue =IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
			//ddr_mainsubctrl_reg.disp2_double_enable=1;
			//ddr_mainsubctrl_reg.disp2_double_apply=1;
			//IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
			nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_enable(1);
			nonlibdma_get_disp_ddr_mainsubctrl_reg_doublebuffer_apply(1);
		}

		double_buffer_ctrl_reg.regValue =IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		//D0
		double_buffer_ctrl_reg.dsubreg_dbuf_en=1;
		double_buffer_ctrl_reg.dsubreg_dbuf_set=0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);

        m8p_dtg_set_sub_double_buffer_enable(DOUBLE_BUFFER_D3);
        m8p_dtg_set_sub_double_buffer_not_apply(DOUBLE_BUFFER_D3);

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
		double_buffer_ctrl_reg.regValue =IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		//D0
		double_buffer_ctrl_reg.dsubreg_dbuf_en=1;
		double_buffer_ctrl_reg.dsubreg_dbuf_set=1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, double_buffer_ctrl_reg.regValue);

        m8p_dtg_set_sub_double_buffer_enable(DOUBLE_BUFFER_D3);
        m8p_dtg_set_sub_double_buffer_apply(DOUBLE_BUFFER_D3);

		while(double_buffer_ctrl_reg.dsubreg_dbuf_set)
		{
			timeout --;
			mdelay(1); /*wait 1ms 30 counts for vsync */
			if(!timeout) break;
			double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
		}
		if(timeout == 0) rtd_pr_smt_debug("[display-shiftxy]  sub display apply time out !!!\n");

        m8p_dtg_wait_for_sub_double_buffer_apply(DOUBLE_BUFFER_D3);

	}
#endif
}



unsigned char zoom_check_same_region(unsigned char display)
{
#if 1
	if ((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_HSTA) != 0)
		|| (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_VSTA) != 0)
		|| (!force_enable_mdomain_2disp_path() && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN) != Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN)))
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
#endif
	return zoom_check_same_region_for_SDNR(display);
}

/*
 * @return value: 1 for failure, 0 for success

*/
extern void drvif_mode_disableonlinemeasure(unsigned char channel);
extern void drvif_mode_enableonlinemeasure(unsigned char channel);
extern void drvif_color_ultrazoom_config_scaling_down(unsigned char panorama);
extern unsigned char get_AVD_Input_Source(void);
int zoom_imd_smooth_toggle_proc(unsigned char rcflag, unsigned char rdflag, unsigned char display)
{//rika 20140605 //for DDK/generic/zoom.cpp
	//vgip_int_ctl_RBUS tVGIPIntCtl;
	unsigned char failflag = ZOOM_STATE_OK;
    unsigned int counter  = 10;//200 ms

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
		if ((display == SLR_MAIN_DISPLAY) && (TRUE == Get_rotate_function(SLR_MAIN_DISPLAY)))
			return ZOOM_STATE_OK;
#endif

	///rtd_pr_smt_debug("_rk [zoom3]3dmode:%d\n",Scaler_Get3DMode());
	if(Scaler_Get3DMode((SCALER_DISP_CHANNEL)display) != SLR_3DMODE_2D){ //skip 3d mode
		rtd_pr_smt_debug("_rk [3D] ByPass smooth toggle in 3D mode[%d]@%s\n", Scaler_Get3DMode((SCALER_DISP_CHANNEL)display), __FUNCTION__);
		return ZOOM_STATE_OK;
	}

	rtd_pr_smt_debug("0.----------------------------------\n");

	failflag = zoom_imd_smooth_set_info(display, rcflag, rdflag);

	if(failflag == ZOOM_STATE_FAIL){
		//zoom_smooth_toggle_set_finish(ZOOM_STATE_NONE);
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

	if(1)// always run video smooth toggle
	{
		//change to 3-buffer mode and timing free run first when under game mode.
		down(get_gamemode_check_semaphore());
		if(drv_memory_get_game_mode() && get_new_game_mode_condition() && (display == SLR_MAIN_DISPLAY)){
			/*K5LG-2134:wait last time smoothtoglge finish to avoid gamemode close single buffer flow conflict with smoothtoggle change*/
			while(counter && !(ScalerCheckOrbitFinish()))
			{//need first than Check_smooth_toggle_update_flag
				msleep(10);
				counter--;
			}
			if(counter == 0)
				rtd_pr_smt_err("### func:%s line:%d wait orbit timeout####\r\n", __FUNCTION__, __LINE__);
			Check_smooth_toggle_update_flag(SLR_MAIN_DISPLAY);
			zoom_smoothtoggle_gamemode_set_smt_status(SLR_MAIN_DISPLAY,1);
			zoom_smoothtoggle_gamemode_check(SLR_MAIN_DISPLAY, 1);
			rtd_pr_smt_notice("game mode preset before smt!\n");
		}
	#if 0
		if (imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC) {
			onms_onms1_ctrl_RBUS onms_onms1_ctrl_reg;
			onms_onms1_ctrl_reg.regValue =  IoReg_Read32(ONMS_onms1_ctrl_reg);
			if(onms_onms1_ctrl_reg.on1_start_ms) {
				drvif_mode_onlinemeasure_setting(display, _SRC_VO, FALSE, FALSE);
				drvif_mode_disableonlinemeasure(display);
				IoReg_SetBits(ONMS_onms1_status_reg,  0xffffffff);
			}
		}
	#endif
		#ifdef CONFIG_ALL_SOURCE_DATA_FS
			if ((Scaler_DispGetInputInfoByDisp(display,SLR_INPUT_FRAMESYNC)) &&(0==Scaler_get_data_framesync(display))) {
				Scaler_memory_setting_through_CMA();
				send_memory_mapping_for_adaptive_streaming();
			}
		#endif
		failflag = zoom_imd_smooth_toggle_vo_flow(display);
        up(get_gamemode_check_semaphore());
		//--------------------vo ends here---------------------------

	}
	else if (imdSmoothInfo[display].IMD_SMOOTH_FRAMESYNC)
	{// not VO &&  data fs
		rtd_pr_smt_debug("_rk framesync not support zoom in & zoom out\n");
		//zoom_smooth_toggle_set_finish(ZOOM_STATE_NONE);
		failflag = ZOOM_STATE_OK;
	}
	//---

	if((display == SLR_MAIN_DISPLAY)&&(DbgSclrFlgTkr.mainVscFreezeFlag)){
		Scaler_SetFreeze(SLR_MAIN_DISPLAY, 1);
	}

	return failflag;

#if 0	//go vo smoothtoggle flow, mark no use code.
	rtd_pr_smt_debug("1.----------------------------------\n");

	zoom_Check_Position_Adjust(display);



	if(display == SLR_MAIN_DISPLAY)
	{
		//rtd_pr_smt_debug("file=%s, function=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__);

		vgip_smooth_tog_ctrl_RBUS smooth_tog_ctrl_reg;
		smooth_tog_ctrl_reg.regValue = rtd_inl(VGIP_Smooth_tog_ctrl_reg);
	/*	if (imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE) {
			smooth_tog_ctrl_reg.main_iv2dv_sm_apply_auto_sel=1;
		} else */{
			smooth_tog_ctrl_reg.main_iv2dv_sm_apply_auto_sel=0;
		}
#ifdef CONFIG_DUAL_CHANNEL

		smooth_tog_ctrl_reg.main_imd_smooth_toggle_style = 1;//SMOOTH_TOGGLE_STYLE_VGIP_DI_UZD_M_D
		smooth_tog_ctrl_reg.main_imd_smooth_toggle_clk_time2switch_en = 0; //FIXME
#else

		smooth_tog_ctrl_reg.main_imd_smooth_toggle_style = 1;//SMOOTH_TOGGLE_STYLE_VGIP_DI_UZD_M_D
		smooth_tog_ctrl_reg.main_imd_smooth_toggle_clk_time2switch_en = 0; //FIXME

#endif
		rtd_outl(VGIP_Smooth_tog_ctrl_reg, smooth_tog_ctrl_reg.regValue);

		imd_smooth_enable_main_double_buffer(TRUE);//rktest


	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		//rtd_pr_smt_debug("file=%s, function=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__);
		vgip_smooth_tog_ctrl_RBUS smooth_tog_ctrl_reg;
		smooth_tog_ctrl_reg.regValue = rtd_inl(VGIP_Smooth_tog_ctrl_reg);
		smooth_tog_ctrl_reg.sub_imd_smooth_toggle_style = 1;//SMOOTH_TOGGLE_STYLE_VGIP_DI_UZD_M_D
		smooth_tog_ctrl_reg.sub_imd_smooth_toggle_clk_time2switch_en = 0; //FIXME
		rtd_outl(VGIP_Smooth_tog_ctrl_reg, smooth_tog_ctrl_reg.regValue);
		imd_smooth_enable_sub_double_buffer(TRUE);
	}
#endif

	///rtd_clearbits(PPOVERLAY_MAIN_DISPLAY_CONTROL_RSV_VADDR,_BIT15);  //close watch dog for data framesync flow
	rtd_pr_smt_debug("2.----------------------------------\n");


	//#if 0// drvif_color_iEdgeSmooth_init()@apply is fixed

	zoom_imd_smooth_cal_vgip_capture( display,
			zoom_crop_hpos[display],zoom_crop_vpos[display],zoom_crop_hsize[display],zoom_crop_vsize[display]);
	//smooth_toggle_print_smooth_info(display);
	//rtd_pr_smt_debug("3.file=%s, function=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__);
	rtd_pr_smt_debug("3.----------------------------------\n");



	imd_smooth_scaler_set_vgip_capture(display);//rika 20140605  review
	//fix me //add system src flow scalerdrv.cpp _H_POSITION_MOVE_TOLERANCE
	//smooth_toggle_fw_scaler_cal_vgip_and_ihv_delay(display); //in set_vgip_capture need review
	//smooth_toggle_print_smooth_info(display);

	rtd_pr_smt_debug("4.----------------------------------\n");


	//#if 0//smooth toggle fail when crop 720x484 why? //smooth toggle ok when crop 1920x1080
	zoom_imd_smooth_get_display_size(display);
	//bug fixed: strange screen, bottom<->top part
	//smooth_toggle_print_smooth_info(display);

	rtd_pr_smt_debug("5.----------------------------------\n");


	zoom_imd_smooth_decide_rtnr(display);

	smooth_toggle_Scaler_AdjustPathAndInfo(display);

	zoom_imd_smooth_cal_SDNR_capture(display,
			zoom_crop_hpos[display],zoom_crop_vpos[display],zoom_crop_hsize[display],zoom_crop_vsize[display]);

	zoom_imd_smooth_config_uzd_uzu_param(display);
	//smooth_toggle_print_smooth_info(display);
	rtd_pr_smt_debug("6.----------------------------------\n");


	scalerVIP_smoothtoggle_init(display, imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE, imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE,
		  imdSmoothInfo[display].IMD_SMOOTH_CapWid,imdSmoothInfo[display].IMD_SMOOTH_CapLen,
		  imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_WID,imdSmoothInfo[display].IMD_SMOOTH_MEM_ACT_LEN);

	zoom_imd_smooth_decide_nr_path(display);

	if (is_di_h_scalar_down(display))
		zoom_imd_smooth_color_ultrazoom_config_scaling_down_di_hsd(display);
	zoom_imd_smooth_color_ultrazoom_config_scaling_down(display);

	rtd_pr_smt_debug("7.----------------------------------\n");

	if((_SRC_YPBPR == Scaler_InputSrcGetType(display) ) ||(_SRC_COMPONENT == Scaler_InputSrcGetType(display) ))
		is_src_YPbPr_Component[display] = 1;
	else
		is_src_YPbPr_Component[display] = 0;

	if((!is_src_YPbPr_Component[display]) && (_SRC_HDMI != Scaler_InputSrcGetType(display) ) && (!imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A) && (imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE))//only 5a case mask those bits for register setting in di
		rtd_clearbits(VGIP_VGIP_CHN1_CTRL_reg,(_BIT24|_BIT25)); //frank@1016 test fix me

	//sync from smoothtoggle.cpp rika 20140625
	// Disable VGIP Interrupt and let DI can be enabled.

	tVGIPIntCtl.regValue = rtd_inl(VGIP_INT_CTL_reg);
	tVGIPIntCtl.vgip_int_ie = 0;
	rtd_outl(VGIP_INT_CTL_reg, tVGIPIntCtl.regValue);
#if 0	/* zoom flow DI setting should be same as scaler flow setting, marked this*/
	down(get_DI_semaphore());
	zoom_imd_smooth_scalerip_set_di(display);// rika 20140616 added
	up(get_DI_semaphore());
#endif
	//smooth_toggle_print_smooth_info(display);

	zoom_imd_decide_SDNR(display);

	rtd_pr_smt_debug("8.----------------------------------\n");

#if 0//ndef HDMI_YPBPR_SD_5A
	//sync from smoothtoggle.cpp rika 20140625
	// Enable VGIP Interrupt
	tVGIPIntCtl.vgip_int_ie = 1;
	rtd_outl(VGIP_INT_CTL_reg, tVGIPIntCtl.regValue);
#endif

	zoom_imd_smooth_set_framesync(display);
	//smooth_toggle_print_smooth_info(display);

	rtd_pr_smt_debug("9.----------------------------------\n");


	//before apply
	imd_smooth_toggle_apply(display);

	rtd_pr_smt_debug("10.----------------------------------\n");




	//	rtd_outl(I_EDGE_SMOOTH_EDGESMOOTH_EX_CTRL_VADDR,0x00000001);

	///	rtd_setbits(PPOVERLAY_MAIN_DISPLAY_CONTROL_RSV_VADDR,_BIT15); //open watch dog //remove this kernel does not use this rika 20141126

	failflag = zoom_imd_smooth_wait_to_apply_sucessfully(display);
#if 1//def HDMI_YPBPR_SD_5A
	//sync from smoothtoggle.cpp rika 20140625
	// Enable VGIP Interrupt
	tVGIPIntCtl.vgip_int_ie = 1;
	rtd_outl(VGIP_INT_CTL_reg, tVGIPIntCtl.regValue);

	if((!is_src_YPbPr_Component[display]) && (_SRC_HDMI != Scaler_InputSrcGetType(display) ) && (!imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A) && (imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE))
		rtd_setbits(VGIP_VGIP_CHN1_CTRL_reg,(_BIT24|_BIT25)); //frank@1016 test fix me

	if(m_CP_TemporalEnable){
		di_im_di_control_RBUS im_di_control_reg;
		m_CP_TemporalEnable = 0;
		im_di_control_reg.regValue = IoReg_Read32(DI_IM_DI_CONTROL_reg);
		im_di_control_reg.cp_temporalenable= 1;
		im_di_control_reg.write_enable_6 = 1;
		IoReg_Write32(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);
		//rtd_setbits(DI_IM_DI_CONTROL_reg, _BIT23);
	}

	if(m_CP_Temporal_XC_en){
		m_CP_Temporal_XC_en = 0;
		rtd_setbits(DI_IM_DI_TNR_XC_CONTROL_reg, _BIT0);
	}

	if(m_ChromaError_En){
		m_ChromaError_En = 0;
		rtd_setbits(DI_IM_DI_FRAMESOBEL_STATISTIC_reg, _BIT31);
	}

	if(m_BTR_BlendTemporalRecoveryEn){
		m_BTR_BlendTemporalRecoveryEn = 0;
		rtd_setbits(DI_IM_DI_BTR_CONTROL_reg, _BIT0);
	}

	if(m_HMC_follow_MA_result_En){
		m_HMC_follow_MA_result_En = 0;
		rtd_setbits(DI_IM_DI_HMC_ADJUSTABLE2_reg, _BIT15);
	}
#endif


	scalerVIP_smoothtoggle_film_reset();
#if 0
	if(display == SLR_MAIN_DISPLAY)
	{
		//rtd_pr_smt_debug("_rk no disable double buffer2\n");
		imd_smooth_enable_main_double_buffer(FALSE);
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		imd_smooth_enable_sub_double_buffer(FALSE);
	}
#endif
#endif
	set_zoom_reset_double_state(_ENABLE,display);//when reset mode, call imd_smooth_enable_main_double_buffer(FALSE);

	rtd_pr_smt_debug("11.----------------------------------\n");

	print_scaler_info(display);
	smooth_toggle_print_smooth_info(display);

	rtd_pr_smt_info("zoom finish.----------------------------------\n");

	if(display == SLR_MAIN_DISPLAY)
	{
		zoom_imd_smooth_check_di_buffer_mode();//Check DI buffer mode right or not. If wrong, recover it.
	}

	return failflag;

#endif
}

unsigned int zoom_memory_get_capture_line_size_compression(unsigned char display,unsigned char compression_bits)
{
	unsigned int TotalSize;
	unsigned int capLen = imdSmoothInfo[display].IMD_SMOOTH_CapLen;
	unsigned int capWid = imdSmoothInfo[display].IMD_SMOOTH_CapWid;

	if((capWid % 32) != 0)
		capWid = capWid + (capWid % 32);

	TotalSize = drvif_memory_get_data_align((capWid * compression_bits), 64);// (pixels of per line * bits / 64), unit: 64bits
	TotalSize = (unsigned int)SHR(TotalSize, 6);

	rtd_pr_smt_debug("_rk[zoom]memory_get_line_size LineSize=%d\n",TotalSize);

	//TotalSize = memory_get_line_size(capWid, (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));
	TotalSize = drvif_memory_get_data_align(TotalSize, 0x04);	// it must be times of 4
	rtd_pr_smt_debug("_rk[zoom]:TotalSize=%x,capWid=%x,capLen=%x\n", TotalSize,capWid,capLen);
	return TotalSize;
}

unsigned int zoom_memory_get_capture_frame_size_compression(unsigned char display, unsigned char compression_bits)
{
	unsigned int TotalSize;
	unsigned int capLen = imdSmoothInfo[display].IMD_SMOOTH_CapLen;
	unsigned int capWid = imdSmoothInfo[display].IMD_SMOOTH_CapWid;

	//rtd_pr_smt_emerg("[crixus]capWid div32 = %d\n", capWid);
	TotalSize = capWid * compression_bits;
	//rtd_pr_smt_emerg("[crixus]TotalSize compress = %d\n",TotalSize);
	TotalSize = drvif_memory_get_data_align(TotalSize, 64);// (pixels of per line * bits / 64), unit: 64bits
	TotalSize = TotalSize * capLen;

	//rtd_pr_smt_emerg("[crixus]memory_get_line_size LineSize=%d\n",TotalSize);
	//rtd_pr_smt_emerg("[crixus]:TotalSize=%x,capWid=%x,capLen=%x\n", TotalSize,capWid,capLen);
	return TotalSize;
}


extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr; //extern unsigned char JPEG_Scaler_SM;

//extern UINT32 drvif_memory_get_data_align(UINT32 Value, UINT32 unit);

void zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(unsigned char access,unsigned char display)
{
	//di_im_di_btr_control_RBUS BTR_Control_Reg;
	di_ddr_rdatdma_wtlennum_RBUS ddr_rdatdma_wtlennum;
	di_ddr_rinfdma_wtlennum_RBUS ddr_rinfdma_wtlennum;
	di_ddr_wdatdma_wtlennum_RBUS ddr_wdatdma_wtlennum;
	di_ddr_winfdma_wtlennum_RBUS ddr_winfdma_wtlennum;
	di_ddr_datdma_rm_RBUS ddr_datdma_rm;
	di_ddr_infdma_rm_RBUS ddr_infdma_rm;
	di_di_smd_control_candidatelist_RBUS di_smd_control_candiate_reg;
	//di_di_dma_multi_req_num_RBUS di_dma_multi_req_num_reg;
	//ddr_vrfifo1wtlvllennum_RBUS ddr_vrfifowtlvllennum;
	//ddr_vwfifo1wtlvllennum_RBUS ddr_vwfifowtlvllennum;
	//ddr_vfifo1rdwrremainder_RBUS ddr_vfifordwrremainder;

//	unsigned char display = Smooth_Toggle_dislay;

	unsigned int	DI_Width = 0, DI_Height = 0, size_per_frame = 0;
	unsigned char	data_bitNum = 0, info_bitNum = 0, /*i = 0,*/ DIAllocateBlock;
	unsigned int	data_value=0,  info_value=0,  MemStartAddr =0;
	typedef struct _MEMSET {
		unsigned int number;
		unsigned int length;
		unsigned int remain;
		unsigned int water_level;
	} MEMSET;
	MEMSET			data_r, data_w, info_r, info_w;

//	unsigned int di420_value = 0,di420_remain = 0;
	UINT32 lineSize_odd_128, lineSize_even_128;
	unsigned char DI_SMD_enable = 0;
//	unsigned int di420_width;

	unsigned char source = 255;
	unsigned char newRTNR_onoff = 0;

	//add ifdef to fix coverity 120161 DEADCODE //
	#ifdef ENABLE_DI_COMPRESS //
	unsigned char m_rtnrcompress = 0;
	#endif

	unsigned char DI_BTR_enable = 0;
	unsigned char DI_BTR_type = 0;

	//fixme:vip
#if 1
	extern Scaler_DI_Coef_t Scaler_DI_Coef_table[VIP_QUALITY_SOURCE_NUM];
	source = fwif_vip_source_check(3, 0);

	if(source >=VIP_QUALITY_SOURCE_NUM)
		source=0;

	//frank@1018 add below code to solve scan tool warning
	newRTNR_onoff = (unsigned char)Scaler_DI_Coef_table[source].newRTNR_style;

	//DI_SMD_enable = Scaler_getDI_SMD_en(source);
	di_smd_control_candiate_reg.regValue = rtd_inl(DI_DI_SMD_Control_CandidateList_reg);
	DI_SMD_enable= imdSmoothInfo[display].IMD_SMOOTH_DISP_INTERLACE;//di_smd_control_candiate_reg.smd_en;

/*
		//need review rika 20141021 should fix?
	unsigned char newRTNR_style[15]={0,0,5,5,5,5,0,0,0,5,5,5,5,0,0};  //depends on VIP_SOURCE_TIMING vo dtv,pvr and jpeg
	unsigned char DI_SMD[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	source = imd_smooth_vip_source_check(3,0);

	if(((source-VIP_QUALITY_DTV_480I)>=0) && ((source-VIP_QUALITY_DTV_480I)<15))//add array size check should be 0~15 rika 20140828
	{
		newRTNR_onoff=newRTNR_style[source-VIP_QUALITY_DTV_480I];
		DI_SMD_enable=DI_SMD[source-VIP_QUALITY_DTV_480I];
	}
	else
	{
		newRTNR_onoff =0;
		DI_SMD_enable =0;
	}

*/
	DI_BTR_enable = bIpBtrMode;

	//BTR_Control_Reg.regValue = rtd_inl(DI_IM_DI_BTR_CONTROL_VADDR);
	//DI_BTR_type = BTR_Control_Reg.btr_solutiontype;
	DI_BTR_type = (imdSmoothInfo[display].IMD_SMOOTH_DI_WID < 960)?0:1;

#endif

	DI_Width = imdSmoothInfo[display].IMD_SMOOTH_DI_WID;
	DI_Height = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN;

	rtd_pr_smt_debug("\n DI Memory set FIFO !");
	rtd_pr_smt_debug("\n Width:%d, Height:%d, DMA mode=%d", DI_Width, DI_Height, bIpDma420Mode?420:422);
	rtd_pr_smt_debug("\n Data status Y=%d C=%d", fw_scalerip_get_DI_chroma_10bits()?10:8, (fw_scalerip_get_DI_chroma_10bits())?10:8);
	rtd_pr_smt_debug("\n Info status BTR=%d(type:%d) SMD=%d RTNR style=%d", DI_BTR_enable, DI_BTR_type, DI_SMD_enable, newRTNR_onoff);

#if 1//def Mag2_New_Flow
	data_bitNum = 16; // for YUV subsample 422 ( now only using 2 typr : 422 & 420	)
	info_bitNum = 0;

	// assign data bits
	if (fw_scalerip_get_DI_chroma_10bits())
		data_bitNum += 2;
	if (fw_scalerip_get_DI_chroma_10bits())
		data_bitNum += 2;

#ifdef ENABLE_DI_COMPRESS
	if ((Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID) > 2048)
		&& (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN) > 1200)) {	// 4k2k
		drv_memory_set_ip_compress_bit(12);
		m_rtnrcompress = 1;
		data_bitNum = 12;
	}
#endif

	/*
		LearnRPG comment DI DMA how to use?
		from magellan/sirius/mac3
		FIFO length is 64bits, SD need 2(128bits), HD need 4(SD double), progressive need 6
		about READ(from DDR to FIFO) the best performance is using water_level+length = FIFO length
		about WRITE(from FIFO to DDR) must just MATCH, otherwise will have dummy data in FIFO
	*/
	// assign info bits
	if (access == SLR_DI_5A_mode) {
		rtd_pr_smt_debug("\n 5A mode \n");

		info_bitNum += (DI_BTR_enable?(DI_BTR_type?4:8):0); // type A:8 bits, type B:4 bits

		DIAllocateBlock = 4;

		data_r.water_level = 68;
		data_r.length = 60;

		data_w.water_level = 60;
		data_w.length = 60;

		info_r.water_level = 32;
		info_r.length = 32;

		info_w.water_level = 32;
		info_w.length = 32;
	} else if (access == SLR_DI_3A_mode) {
		rtd_pr_smt_debug("\n 3A mode \n");

		info_bitNum += (DI_BTR_enable?4:0); // type A:8 bits, type B:4 bits, 3A only type B
		info_bitNum += 4; // frame motion in 3A default need 4 bits

		DIAllocateBlock = 2;

		data_r.water_level = 196;
		data_r.length = 60;

		data_w.water_level = 60;
		data_w.length = 60;

		info_r.water_level = 32;
		info_r.length = 32;

		info_w.water_level = 32;
		info_w.length = 32;
	} else if (access == SLR_DI_progress_mode) {
		rtd_pr_smt_debug("\n progress mode \n");

		DIAllocateBlock = 2;

		data_r.water_level = 196;
		data_r.length = 60;

		data_w.water_level = 60;
		data_w.length = 60;

		info_r.water_level = 32;
		info_r.length = 32;

		info_w.water_level = 32;
		info_w.length = 32;
	} else {
		rtd_pr_smt_notice("\n Error accept parameter in memory fifo \n");
		return;
	}

	if (DI_SMD_enable)
		info_bitNum += 6; //4; //merlin2 need more 2bit->4+2=6

	// if k temporal function is enabled, new rtnr need 2 bits, otherwise still 0, please refer scalerColor.cpp:fwif_color_set_RTNR_style_adjust()
	if (newRTNR_onoff == 3 || newRTNR_onoff == 4)
		info_bitNum += 2;

	rtd_pr_smt_debug("\n Data BitNum=%d, Info BitNum=%d", data_bitNum, info_bitNum);

	// Calculate Data number/length/remain
	if (bIpDma420Mode==SLR_DMA_420_mode) {	// 420 mode
		lineSize_even_128	= DI_Width * (data_bitNum - (fw_scalerip_get_DI_chroma_10bits()?10:8)); // only Y componet
		lineSize_odd_128	= DI_Width * data_bitNum; // Y and C componet

		// linesize unit 64
		if (lineSize_even_128%64 == 0)
			lineSize_even_128 /= 64;
		else
			lineSize_even_128 = (lineSize_even_128+64)/64;
		if (lineSize_odd_128%64 == 0)
			lineSize_odd_128 /= 64;
		else
			lineSize_odd_128 = (lineSize_odd_128+64)/64;

		// frame unit 128, so linesize must divide by 2
		//if (DI_Height%2 == 0)
		//	size_per_frame = ((lineSize_even_128 + lineSize_odd_128 + 1)/2)*(DI_Height/2);
		//else
		size_per_frame = ((lineSize_even_128+1)/2)*(DI_Height/2) + ((lineSize_odd_128+1)/2)*((DI_Height+1)/2);

		data_r.number = (int)(size_per_frame/data_r.length);
		data_r.remain = size_per_frame%data_r.length;
		if (data_r.remain == 0 && data_r.number != 0) {
			data_r.number -= 1;
			data_r.remain = data_r.length;
		}

		data_w.number = (int)(size_per_frame/data_w.length);
		data_w.remain = size_per_frame%data_w.length;
		if (data_w.remain == 0 && data_w.number != 0) {
			data_w.number -= 1;
			data_w.remain = data_w.length;
		}


		if ((lineSize_even_128 > 0x7ff) || (lineSize_odd_128 > 0x7ff))
		{
			rtd_pr_smt_debug("Q-12237 data protect 02, Error data need to check!! (%d, %d, %d, %d, %d)\n",
				lineSize_even_128,
				lineSize_odd_128,
				DI_Width,
				data_bitNum,
				fw_scalerip_get_DI_chroma_10bits());
			lineSize_even_128 = lineSize_even_128 & 0x000007ff;
			lineSize_odd_128 = lineSize_odd_128 & 0x000007ff;
		}

		IoReg_Mask32(DI_IM_DI_DMA_reg,
			~(DI_DMA_420_EN_MASK| DI_DMA_LINESIZE_EVEN_MASK| DI_DMA_LINESIZE_ODD_MASK),
			_BIT30 | (lineSize_even_128 << DI_DMA_LINESIZE_EVEN_START_BIT) | lineSize_odd_128);

		rtd_pr_smt_debug("\n 420 mode: linesize_even=%d, linesize_odd=%d", lineSize_even_128, lineSize_odd_128);
	} else if (bIpDma420Mode==SLR_DMA_422_mode){ // 422 mode;
		size_per_frame = DI_Width * DI_Height * data_bitNum;
		data_r.number = (int)(size_per_frame/128/data_r.length);
		data_r.remain = (size_per_frame - data_r.number*128*data_r.length + 127)/128;
		if (data_r.remain == 0 && data_r.number != 0) {
			data_r.number -= 1;
			data_r.remain = data_r.length;
		}

		data_w.number = (int)(size_per_frame/128/data_w.length);
		data_w.remain = (size_per_frame - data_w.number*128*data_w.length + 127)/128;
		if (data_w.remain == 0 && data_w.number != 0) {
			data_w.number -= 1;
			data_w.remain = data_w.length;
		}

		rtd_clearbits(DI_IM_DI_DMA_reg, (_BIT30|_BIT31));
	} else{ // 400 mode
		//size_per_frame = DI_Width * DI_Height * data_bitNum;
		size_per_frame = DI_Width * DI_Height * (data_bitNum - (fw_scalerip_get_DI_chroma_10bits()?10:8));
		data_r.number = (int)(size_per_frame/128/data_r.length);
		data_r.remain = (size_per_frame - data_r.number*128*data_r.length + 127)/128;
		if (data_r.remain == 0 && data_r.number != 0) {
			data_r.number -= 1;
			data_r.remain = data_r.length;
		}

		data_w.number = (int)(size_per_frame/128/data_w.length);
		data_w.remain = (size_per_frame - data_w.number*128*data_w.length + 127)/128;
		if (data_w.remain == 0 && data_w.number != 0) {
			data_w.number -= 1;
			data_w.remain = data_w.length;
		}
		rtd_setbits(DI_IM_DI_DMA_reg, _BIT31);
		rtd_clearbits(DI_IM_DI_DMA_reg, _BIT30);
	}

	// Calculate Info number/length/remain
	size_per_frame = DI_Width * DI_Height * info_bitNum;
	if (DI_SMD_enable && access == SLR_DI_5A_mode) // Info image height must +1 for DMA setting when SMD enable in 5A case
		size_per_frame += DI_Width * 4;

	info_r.number = (int)(size_per_frame/128/info_r.length);
	info_r.remain = (size_per_frame - info_r.number*128*info_r.length + 127)/128;
	if (info_r.remain == 0 && info_r.number != 0) {
		info_r.number -= 1;
		info_r.remain = info_r.length;
	}

	info_w.number = (int)(size_per_frame/128/info_w.length);
	info_w.remain = (size_per_frame - info_w.number*128*info_w.length + 127)/128;
	if (info_w.remain == 0 && info_w.number != 0) {
		info_w.number -= 1;
		info_w.remain = info_w.length;
	}

	rtd_pr_smt_debug("\n Data number = %d, length = %d, remain = %d, water_level = %d in setting DI Read FIFO", data_r.number, data_r.length, data_r.remain, data_r.water_level);
	rtd_pr_smt_debug("\n Data number = %d, length = %d, remain = %d, water_level = %d in setting DI Write FIFO", data_w.number, data_w.length, data_w.remain, data_w.water_level);
	rtd_pr_smt_debug("\n Info number = %d, length = %d, remain = %d, water_level = %d in setting DI Read FIFO", info_r.number, info_r.length, info_r.remain, info_r.water_level);
	rtd_pr_smt_debug("\n Info number = %d, length = %d, remain = %d, water_level = %d in setting DI Write FIFO", info_w.number, info_w.length, info_w.remain, info_w.water_level);

	ddr_rdatdma_wtlennum.dat_water_r	= data_r.water_level;
	ddr_rdatdma_wtlennum.dat_len_r		= data_r.length;
	ddr_rdatdma_wtlennum.dat_num_r		= data_r.number;

	ddr_wdatdma_wtlennum.dat_water_w	= data_w.water_level;
	ddr_wdatdma_wtlennum.dat_len_w		= data_w.length;
	ddr_wdatdma_wtlennum.dat_num_w		= data_w.number;

	ddr_datdma_rm.dat_remain_r			= data_r.remain;
	ddr_datdma_rm.dat_remain_w			= data_w.remain;
	ddr_datdma_rm.rdma_remain_en		= 1;

	ddr_rinfdma_wtlennum.inf_water_r	= info_r.water_level;
	ddr_rinfdma_wtlennum.inf_len_r		= info_r.length;
	ddr_rinfdma_wtlennum.inf_num_r		= info_r.number;

	ddr_winfdma_wtlennum.inf_water_w	= info_w.water_level;
	ddr_winfdma_wtlennum.inf_len_w		= info_w.length;
	ddr_winfdma_wtlennum.inf_num_w		= info_w.number;;

	ddr_infdma_rm.inf_remain_r			= info_r.remain;
	ddr_infdma_rm.inf_remain_w			= info_w.remain;

	//remain and number both 0 will cause DMA error when 1080p switch to 576i DI will fliker eric_huang@realsil.com.cn
	if (info_r.number == 0 && info_r.remain == 0) {
		ddr_rinfdma_wtlennum.inf_num_r = 0x174;
		ddr_infdma_rm.inf_remain_r = 0x0b;
	}

	//remain and number both 0 will cause DMA error when 1080p switch to 576i DI will fliker eric_huang@realsil.com.cn
	if (info_w.number == 0 && info_w.remain == 0) {
		ddr_winfdma_wtlennum.inf_num_w = 0x174;
		ddr_infdma_rm.inf_remain_w = 0x0b;
	}

	rtd_outl(DI_DDR_RDATDMA_WTLENNUM_reg, ddr_rdatdma_wtlennum.regValue);
	rtd_outl(DI_DDR_WDATDMA_WTLENNUM_reg, ddr_wdatdma_wtlennum.regValue);
	rtd_outl(DI_DDR_RINFDMA_WTLENNUM_reg, ddr_rinfdma_wtlennum.regValue);
	rtd_outl(DI_DDR_WINFDMA_WTLENNUM_reg, ddr_winfdma_wtlennum.regValue);
	rtd_outl(DI_DDR_DATDMA_RM_reg, ddr_datdma_rm.regValue);
	rtd_outl(DI_DDR_INFDMA_RM_reg, ddr_infdma_rm.regValue);

	//frank@0514 change DI multi request to solve DI read underflow ++
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//drv_memory_disable_di_write_req(FALSE);
		nonlibdma_drv_memory_disable_di_write_req(FALSE);
	}
	//frank@0514 change DI multi request to solve DI read underflow --
#endif
//frank@1202 Change code to solve 1366x768 image fail problem
	{
		data_value = data_bitNum;
		info_value = info_bitNum;

		if((imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE) > DISP_WID)
		{
			data_value *= DISP_WID;
			info_value *= DISP_WID;
		}else
		{
			data_value *= imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE;
			info_value *= imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE;
		}

		data_value *=imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE;
		info_value *=imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN_PRE;

		data_value = (data_value >> 3);//byte unit
		info_value = (info_value >> 3);//byte unit
//		value =(info->IPV_ACT_LEN_PRE *info->IPH_ACT_WID_PRE * bitNum)>>4;
	}

	if (bIpDma420Mode) {
		//sync code from Mac2. Start
		di_im_di_dma_RBUS im_di_dma;
		unsigned int di420_width = 0;
		im_di_dma.regValue = rtd_inl(DI_IM_DI_DMA_reg);
		lineSize_even_128 = (imdSmoothInfo[display].IMD_SMOOTH_DI_WID * (data_bitNum - 8));
		if ((lineSize_even_128%128) == 0)
			lineSize_even_128 /= 128;
		else
			lineSize_even_128 = (lineSize_even_128+128)/128;

		lineSize_odd_128 = (imdSmoothInfo[display].IMD_SMOOTH_DI_WID * data_bitNum);
		if ((lineSize_odd_128%128) == 0)
			lineSize_odd_128 /= 128;
		else
			lineSize_odd_128 = (lineSize_odd_128+128)/128;

		if (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN%2 == 0) {
			di420_width = (lineSize_odd_128 + lineSize_even_128)*(DI_Height/2);
		} else {
			di420_width = lineSize_even_128*(DI_Height/2)+lineSize_odd_128*((DI_Height+1)/2);
		}
		data_value = di420_width<<4;
		//sync code from Mac2. End
	}

	//drvif_memory_free_block(MEMIDX_DI);

	// Set 32 avoid to cover other block memroy ([[h * v * bitnum] / (64* 2) ]* 16
	data_value = drvif_memory_get_data_align(data_value, 32);
	info_value = drvif_memory_get_data_align(info_value, 32);

#if 1
	//define at makeconfig
	//patch for DI/RTNR bug(overwrite 512Byte)
	if (access != 2) {
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value * DIAllocateBlock+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
			MemStartAddr = nonlibdma_drvif_memory_alloc_block(MEMIDX_DI, data_value * DIAllocateBlock+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
		}
	} else {
		//add ifdef to fix coverity 120161 DEADCODE
		//rika20151217
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
#ifdef ENABLE_DI_COMPRESS
			if (m_rtnrcompress) {//compress mode use 2 buffer
				//MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value*2+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
				MemStartAddr = nonlibdma_drvif_memory_alloc_block(MEMIDX_DI, data_value*2+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
			} else {
				//MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
				MemStartAddr = nonlibdma_drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
			}
#else
			//MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
			MemStartAddr = nonlibdma_drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
#endif
		}
	}
#else
	if (access != 2)
		MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value * DIAllocateBlock+info_value*2, MEMALIGN_ANY);
	else {
		if (m_rtnrcompress)//compress mode use 2 buffer
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value*2+info_value*2, MEMALIGN_ANY);
		else
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2, MEMALIGN_ANY);
	}
#endif
	rtd_pr_smt_debug("Get ip addr = %x, size=%x\n", MemStartAddr, (data_value * DIAllocateBlock+info_value*2));

#if 0
	// Set start address for deinterlace
	for(i = 0; i < DIAllocateBlock; i++) {
		rtd_pr_smt_debug(" IP address%x = %x\n", i, MemStartAddr);

		rtd_outl((DI_DI_DATMEMORYSTARTADD1_VADDR + (i << 2)), (MemStartAddr&0x7FFFFFFF));

		if ((access != SLR_DI_progress_mode)|| m_rtnrcompress)
			MemStartAddr += data_value;
	}

	if ((access == SLR_DI_progress_mode) && !m_rtnrcompress) {
		MemStartAddr += data_value;
	}

	for (i = 0; i < 2; i++) {
		rtd_pr_smt_debug(" Info IP address%x = %x\n", i, MemStartAddr);

		rtd_outl((DI_DI_INFMEMORYSTARTADD1_VADDR + (i << 2)), (MemStartAddr&0x7FFFFFFF));

		if (access != SLR_DI_progress_mode)
			MemStartAddr += info_value;
	}
#endif
	return;
}



#if 0	/* zoom flow DI setting should be same as scaler flow setting, marked this*/
void zoom_imd_smooth_scalerip_set_di(unsigned char display)
{
	//compare with vo smoothtoggle.c imd_smooth_scalerip_set_di
	unsigned char src;
	unsigned short temp;
	unsigned char m_3DDIFlag = 0;
	//	unsigned char diflag = 0;
	//	data_path_select_RBUS data_path_select_reg;
	di_im_di_active_window_control_RBUS	 reg_Active_Win_di_Reg;
	di_im_di_weave_window_control_RBUS im_di_weave_window_control_reg;
	vgip_vgip_chn1_ctrl_RBUS vgip_chn1_ctrl_reg;
	vgip_vgip_chn1_misc_RBUS vgip_chn1_misc_reg;
#ifdef CONFIG_DUAL_CHANNEL
	sub_vgip_vgip_chn2_misc_RBUS vgip_chn2_misc_reg;
	sub_vgip_vgip_chn2_ctrl_RBUS vgip_chn2_ctrl_reg;
	vgip_data_path_select_RBUS data_path_select_reg;//for sub channel @this function
#endif

	di_im_di_control_RBUS im_di_control_reg;
	di_im_di_btr_control_RBUS im_di_btr_control_reg;
	di_color_recovery_option_RBUS Color_Recovery_Option_Reg;

	Set_vo_smoothtoggle_changeratio_flag2(_ENABLE,display);//for DI mem allocation rika 20140829 added

	im_di_control_reg.regValue = rtd_inl(DI_IM_DI_CONTROL_reg);


	if(display > SLR_SUB_DISPLAY) display = SLR_MAIN_DISPLAY;//array size check rika 20140828

	src = Scaler_InputSrcGetType(display);

	if((imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP == 1) ||(imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR == 1))
	{
		//=======================================
		di_im_di_rtnr_control_RBUS RecursiveTemporalNRCtrl_reg;
		RecursiveTemporalNRCtrl_reg.regValue = rtd_inl(DI_IM_DI_RTNR_CONTROL_reg);
		if(!imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
		{
			RecursiveTemporalNRCtrl_reg.cp_rtnr_progressive = 0x1;
		}
		else
		{
			RecursiveTemporalNRCtrl_reg.cp_rtnr_progressive = 0x0;
		}
		rtd_outl(DI_IM_DI_RTNR_CONTROL_reg,RecursiveTemporalNRCtrl_reg.regValue);

		// DI 8 or 10 bits by LearnRPG
		Color_Recovery_Option_Reg.regValue = rtd_inl(DI_Color_Recovery_Option_reg);
		Color_Recovery_Option_Reg.y10bitsin = fw_scalerip_get_DI_chroma_10bits();
		Color_Recovery_Option_Reg.c10bitsin= fw_scalerip_get_DI_chroma_10bits();
		Color_Recovery_Option_Reg.cr_autoimg_enable=0;
		if(imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
			Color_Recovery_Option_Reg.cr_fh_switch=0;
		else//progressive => disable auto-balance
			Color_Recovery_Option_Reg.cr_fh_switch=1;
		rtd_outl(DI_Color_Recovery_Option_reg, Color_Recovery_Option_Reg.regValue);
		//============================

		//frank@0514 move this position let force 2d and disable DI double buffer register work
//		rtd_setbits(VGIP_Data_Path_Select_reg, _BIT3);


		// [Code Sync][LewisLee][0990226][1]
#ifdef CONFIG_DUAL_CHANNEL
		if(_CHANNEL2 == display)
		{
			data_path_select_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_reg);
			data_path_select_reg.uzd2_in_sel = _ENABLE;
			data_path_select_reg.xcdi_in_sel = _CHANNEL2;
			IoReg_Write32(VGIP_Data_Path_Select_reg, data_path_select_reg.regValue);
		}
		else// if(_CHANNEL1 == Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))
		{
			data_path_select_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_reg);
			data_path_select_reg.uzd1_in_sel = _ENABLE;
			data_path_select_reg.xcdi_clk_en = _ENABLE;//Enable clock. Will Add 20151218
			data_path_select_reg.di_i_clk_en = _ENABLE;
			IoReg_Write32(VGIP_Data_Path_Select_reg, data_path_select_reg.regValue);
		}
		// [Code Sync][LewisLee][0990226][1][End]

#endif


		im_di_btr_control_reg.regValue=rtd_inl(DI_IM_DI_BTR_CONTROL_reg);



		if(imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
		{
			#ifdef CONFIG_ENABLE_VD_27MHZ_MODE
			if(VD_27M_HSD960_DI5A == fw_video_get_27mhz_mode((SCALER_DISP_CHANNEL)display))
			{

					if(imdSmoothInfo[display].IMD_SMOOTH_CapWid > 960)
					{	//sync from pacific
						rtd_pr_smt_debug("_rk %s %d\n",__FUNCTION__,__LINE__);
						zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_3A_mode,display);
						im_di_btr_control_reg.btr_blendtemporalrecovery_dma_en = 0;
						im_di_control_reg.ma_3aenable = 1;
						imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A = TRUE;
						rtd_pr_smt_debug("\n_rk Set DI Memory Main 3A\n");						//rtd_pr_smt_debug("\nDI 3A\n");
					}
					else //motion adaptive 5A or 3A
					{	// 5A
						rtd_pr_smt_debug("_rk %s %d\n",__FUNCTION__,__LINE__);
						zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_5A_mode,display);

						im_di_btr_control_reg.btr_blendtemporalrecovery_dma_en = 1;
						im_di_control_reg.ma_3aenable = 0;
						imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A = FALSE;

						//		rtd_pr_smt_debug("\n_rk Set DI Memory Main 5A\n");

					}
			}
			else// if(VD_27M_HSD960_DI5A != fw_video_get_27mhz_mode((SCALER_DISP_CHANNEL)display))
			{
					if(imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A)//if((src!=_SRC_TV) &&(src!=_SRC_CVBS))
					{	//sync from pacific
						rtd_pr_smt_err("_rk %s %d\n",__FUNCTION__,__LINE__);
						zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_3A_mode,display);
						im_di_btr_control_reg.btr_blendtemporalrecovery_dma_en = 0;
						im_di_control_reg.ma_3aenable = 1;
						imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A = TRUE;
						rtd_pr_smt_debug("\n_rk Set DI Memory Main 3A\n");
						//rtd_pr_smt_debug("\nDI 3A\n");
					}
					else //motion adaptive 5A or 3A
					{	// 5A
						rtd_pr_smt_err("_rk %s %d\n",__FUNCTION__,__LINE__);
						zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_5A_mode,display);

						im_di_btr_control_reg.btr_blendtemporalrecovery_dma_en = 1;
						im_di_control_reg.ma_3aenable = 0;
						imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A = FALSE;
						//rtd_pr_smt_debug("\nDI 5A\n");
					}
			}
			#else
			if(imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A)
			{	//sync from pacific
				zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_3A_mode,display);
				//smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_3A_mode);
				//	rtd_clearbits(DI_IM_DI_BTR_CONTROL_VADDR, _BIT9);
				//	rtd_maskl(DI_IM_DI_CONTROL_reg, ~_BIT4, _BIT4);
				im_di_btr_control_reg.btr_blendtemporalrecovery_dma_en = 0;
				im_di_control_reg.ma_3aenable = 1;
				imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A = TRUE;
				rtd_pr_smt_debug("\n_rk Set DI Memory Main 3A\n");
			}
			else //motion adaptive 5A or 3A
			{	// 5A
				//smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_5A_mode);

				zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_5A_mode,display);

				//	rtd_setbits(DI_IM_DI_BTR_CONTROL_VADDR, _BIT9);
				//	rtd_maskl(DI_IM_DI_CONTROL_reg, ~_BIT4, 0);

				im_di_btr_control_reg.btr_blendtemporalrecovery_dma_en = 1;
				im_di_control_reg.ma_3aenable = 0;
				imdSmoothInfo[display].IMD_SMOOTH_DISP_MA3A = FALSE;

				//		rtd_pr_smt_debug("\n_rk Set DI Memory Main 5A\n");

			}
			#endif

#if 1//def HDMI_YPBPR_SD_5A
			if((!is_src_YPbPr_Component[display] ) && (_SRC_HDMI != Scaler_InputSrcGetType(display) ) && (!im_di_control_reg.ma_3aenable)){//5A case
				unsigned int timeout = 0x3ffff;
				while((rtd_inl(DI_IM_DI_CONTROL_reg)&_BIT23) && (timeout--)){
					m_CP_TemporalEnable = 1;
					rtd_clearbits(DI_IM_DI_CONTROL_reg, _BIT23);
				}
				if(timeout == 0)
					rtd_pr_smt_debug("CP_TemporalEnable timeout");

				timeout = 0x3ffff;
				while((rtd_inl(DI_IM_DI_TNR_XC_CONTROL_reg)&_BIT0)&& (timeout--)){
					m_CP_Temporal_XC_en = 1;
					rtd_clearbits(DI_IM_DI_TNR_XC_CONTROL_reg, _BIT0);
				}
				if(timeout == 0)
					rtd_pr_smt_debug("CP_Temporal_XC_en timeout");

				timeout = 0x3ffff;
				while((rtd_inl(DI_IM_DI_FRAMESOBEL_STATISTIC_reg)&_BIT31)&& (timeout--)){
					m_ChromaError_En = 1;
					rtd_clearbits(DI_IM_DI_FRAMESOBEL_STATISTIC_reg, _BIT31);
				}
				if(timeout == 0)
					rtd_pr_smt_debug("ChromaError_En timeout");

				timeout = 0x3ffff;
				while((rtd_inl(DI_IM_DI_BTR_CONTROL_reg)&_BIT0)&& (timeout--)){
					m_BTR_BlendTemporalRecoveryEn = 1;
					rtd_clearbits(DI_IM_DI_BTR_CONTROL_reg, _BIT0);
				}
				if(timeout == 0)
					rtd_pr_smt_debug("BTR_BlendTemporalRecoveryEn timeout");

				timeout = 0x3ffff;
				while((rtd_inl(DI_IM_DI_HMC_ADJUSTABLE2_reg)&_BIT15)&& (timeout--)){
					m_HMC_follow_MA_result_En = 1;
					rtd_clearbits(DI_IM_DI_HMC_ADJUSTABLE2_reg, _BIT15);
				}
				if(timeout == 0)
					rtd_pr_smt_debug("HMC_follow_MA_result_En disable timeout");
			}
#endif
		}
		else
		{
			zoom_smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_progress_mode,display);
			//smooth_toggle_imd_smooth_memory_set_ip_fifo(SLR_DI_progress_mode);
			//	rtd_clearbits(DI_IM_DI_BTR_CONTROL_VADDR, _BIT9);
			//	rtd_maskl(DI_IM_DI_CONTROL_reg, ~_BIT4, 0);//20080610 weihauo for progressive, jaosn9.ccc
			im_di_btr_control_reg.btr_blendtemporalrecovery_dma_en = 0;
			im_di_control_reg.ma_3aenable = 0;
			//	rtd_pr_smt_debug("\nSet RTNR Memory\n");
		}

		rtd_outl(DI_IM_DI_BTR_CONTROL_reg, im_di_btr_control_reg.regValue);
		rtd_outl(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);
		im_di_control_reg.regValue = rtd_inl(DI_IM_DI_CONTROL_reg);
		im_di_control_reg.ip_enable = 1;
		rtd_outl(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);

		//sync from smoothtoggle.cpp rika 20140625
		//frank@0311 Open vodma film mode control
		if (src == _SRC_VO)
			rtd_setbits(DI_IM_DI_MPEG_SEQUENCE_APP_reg, _BIT7);
		else
			rtd_clearbits(DI_IM_DI_MPEG_SEQUENCE_APP_reg, _BIT7);

		//frank@04262010 add below code avoid 3ddi write data to the address 0 let code haneg ++
		//fw_scaler_double_buffer_maskl((SCALER_DISP_CHANNEL)display,DI_IM_DI_WEAVE_WINDOW_CONTROL_VADDR, ~_BIT19, _BIT19);
		m_3DDIFlag = 0;

		//need to review more rika 20140625

		if(/*Scaler_PipGetInfo(SLR_PIP_ENABLE) &&*/ ((display == SLR_MAIN_DISPLAY) && ((src == _SRC_VGA) &&  ((imdSmoothInfo[display].IMD_SMOOTH_MODE_CURR == _MODE_1080I25) || (imdSmoothInfo[display].IMD_SMOOTH_MODE_CURR == _MODE_1080I30)))))
		{
#ifdef CONFIG_1080I_THROUGH_FRAMESYNC
			if ((Scaler_PipGetInfo(SLR_PIP_TYPE) >= SUBTYPE_PIPMLSR) && (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_PIPMLSR_4x3))
			{
				// fw_scaler_double_buffer_maskl((SCALER_DISP_CHANNEL)display, DI_IM_DI_WEAVE_WINDOW_CONTROL_VADDR, ~_BIT19, _BIT19);	// 2D DI
				//frank@0414 free DI memory to avoid memory overlay of main and sub channel
				///drvif_memory_free_block(MEMIDX_DI);	// do not free DI block during smooth toggle.
				imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
			}
			else
				m_3DDIFlag = 1;
#else
			// fw_scaler_double_buffer_maskl((SCALER_DISP_CHANNEL)display, DI_IM_DI_WEAVE_WINDOW_CONTROL_VADDR, ~_BIT19, _BIT19);	// 2D DI

			//rika20140818 marked
			//mark below to prevent IoReg_clearBits(BUS_TV_SB2_DCU1_ARB_CR1_VADDR, _BIT0);
			// do not free DI block during smooth toggle.
			//frank@0414 free DI memory to avoid memory overlay of main and sub channel
			///drvif_memory_free_block(MEMIDX_DI);
			imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
#endif
		}
		#if 0
		else if(Scaler_TVGetScanMode()) {
			// fw_scaler_double_buffer_maskl((SCALER_DISP_CHANNEL)display, DI_IM_DI_WEAVE_WINDOW_CONTROL_VADDR, ~_BIT19, _BIT19);
			imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR = FALSE;
		}
		#endif
		else if(display == SLR_SUB_DISPLAY) {
			if((((src == _SRC_HDMI || src == _SRC_DISPLAYPORT)||(Scaler_InputSrcGetFrom(display) == _SRC_FROM_ADC)||(Scaler_InputSrcGetFrom(display) == _SRC_FROM_VO))&&
						((imdSmoothInfo[display].IMD_SMOOTH_MODE_CURR >= _MODE_1080I25) && (imdSmoothInfo[display].IMD_SMOOTH_MODE_CURR <= _MODE_1080I30)) ) ||
					((src == _SRC_HDMI || src == _SRC_DISPLAYPORT) && drvif_Hdmi_GetInterlace() && (imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID >= 0x06F0))){
				// fw_scaler_double_buffer_maskl((SCALER_DISP_CHANNEL)display, DI_IM_DI_WEAVE_WINDOW_CONTROL_VADDR, ~_BIT19, _BIT19);
				//frank@0414 free DI memory to avoid memory overlay of main and sub channel
				///drvif_memory_free_block(MEMIDX_DI);
			}else{
				m_3DDIFlag = 1;
			}
		} else {
			m_3DDIFlag = 1; // 3D DI
		}

		//------------------------
		// Set active size to normalizing size in order to avoid to conflict
		reg_Active_Win_di_Reg.regValue = rtd_inl(DI_IM_DI_ACTIVE_WINDOW_CONTROL_reg);
		reg_Active_Win_di_Reg.hsize = imdSmoothInfo[display].IMD_SMOOTH_DI_WID;// Width;
		reg_Active_Win_di_Reg.vsize = imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN; // Height;
		reg_Active_Win_di_Reg.hblksize = imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL - reg_Active_Win_di_Reg.hsize; // Blank;
		rtd_outl(DI_IM_DI_ACTIVE_WINDOW_CONTROL_reg, reg_Active_Win_di_Reg.regValue);

		di_im_di_debug_mode_RBUS di_debug_mode_reg ;
		di_debug_mode_reg.regValue = rtd_inl(DI_IM_DI_DEBUG_MODE_reg);
		di_debug_mode_reg.auto_freeze = 0 ;
		rtd_outl(DI_IM_DI_DEBUG_MODE_reg, di_debug_mode_reg.regValue);

		//frank@0418 add below code for 4k2k input size ++//frank@0503 reset hsize_msb/vsize_msb/hblksize_msb avoid image noise
		im_di_weave_window_control_reg.regValue = rtd_inl(DI_IM_DI_WEAVE_WINDOW_CONTROL_reg);
		if(imdSmoothInfo[display].IMD_SMOOTH_DI_WID > 0x7FF){
			im_di_weave_window_control_reg.hsize_msb =(imdSmoothInfo[display].IMD_SMOOTH_DI_WID>>11);
		}else{
			im_di_weave_window_control_reg.hsize_msb = 0;
		}

		if(imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN > 0x3FF){
			im_di_weave_window_control_reg.vsize_msb = (imdSmoothInfo[display].IMD_SMOOTH_V_ACT_LEN>>10);
		}else{
			im_di_weave_window_control_reg.vsize_msb = 0;
		}
		if((imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL - imdSmoothInfo[display].IMD_SMOOTH_DI_WID)>0x7FF){
			im_di_weave_window_control_reg.hblksize_msb = 1;
		} else if((imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL - imdSmoothInfo[display].IMD_SMOOTH_DI_WID)>0x3FF){
			im_di_weave_window_control_reg.hblksize_msb = ((imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL - imdSmoothInfo[display].IMD_SMOOTH_DI_WID)>>10);
		}else{
			im_di_weave_window_control_reg.hblksize_msb = 0;
		}
		if(imdSmoothInfo[display].IMD_SMOOTH_DI_CUT_4_LINES)
		{
			im_di_weave_window_control_reg.dellineen = _ENABLE;
		}
		else
		{
			im_di_weave_window_control_reg.dellineen = _DISABLE;
		}
		im_di_weave_window_control_reg.freeze = 0;
		//im_di_weave_window_control_reg.force2d = !m_3DDIFlag;
		rtd_outl(DI_IM_DI_WEAVE_WINDOW_CONTROL_reg, im_di_weave_window_control_reg.regValue);
		rtd_pr_smt_debug("_rk !m_3DDIFlag:%d\n",(!m_3DDIFlag));
		//vo marked below cause vo always use 3ddi rika 20140710 //need review more
		//scalerip.cpp zoom_scalerip_set_di
		//below is same as set_di_doublebuf_write(diflag, !m_3DDIFlag);
		//marked below to fix ATV green screen rika 20140711
		//im_di_weave_window_control_reg.freeze =0;
		//zoom_imd_smooth_set_di_force2d(!m_3DDIFlag);
		//frank@0418 add below code for 4k2k input size --


		if(display == SLR_MAIN_DISPLAY)
		{
			vgip_chn1_ctrl_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_CTRL_reg);
			if(vgip_chn1_ctrl_reg.ch1_digital_mode) //digital mode need to set porch
			{
				vgip_chn1_misc_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_MISC_reg);
				if(Scaler_InputSrcGetType(display) == _SRC_VO)
				if ((get_vsc_run_adaptive_stream())&& ((imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL == VO_FIXED_4K2K_HTOTAL) ||(imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL == VO_FIXED_2K1K_HTOTAL)))
				{
					vgip_chn1_misc_reg.ch1_hporch_num = 520;
				}
				else if(Scaler_DispGetInputInfo(SLR_INPUT_H_LEN) > (Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) + VGIP_H_PORCH_MARGIN))
				{
					vgip_chn1_misc_reg.ch1_hporch_num = imdSmoothInfo[display].IMD_SMOOTH_H_TOTAL - imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE - VGIP_H_PORCH_MARGIN;
				}
				else//hdmi case
					vgip_chn1_misc_reg.ch1_hporch_num = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE;
				IoReg_Write32(VGIP_VGIP_CHN1_MISC_reg, vgip_chn1_misc_reg.regValue);
			}
			else//analog mode
			{
				vgip_chn1_misc_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_MISC_reg);
				vgip_chn1_misc_reg.ch1_hporch_num = 0;
				IoReg_Write32(VGIP_VGIP_CHN1_MISC_reg, vgip_chn1_misc_reg.regValue);
			}
		}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		else
		{
			vgip_chn2_ctrl_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_CTRL_reg);
			if(vgip_chn2_ctrl_reg.ch2_digital_mode)
			{
				vgip_chn2_misc_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_MISC_reg);
				if(Scaler_InputSrcGetType(display) == _SRC_VO)
					vgip_chn2_misc_reg.ch2_hporch_num = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE + 4;
				else//hdmi case
					vgip_chn2_misc_reg.ch2_hporch_num = imdSmoothInfo[display].IMD_SMOOTH_H_ACT_STA_PRE;
				IoReg_Write32(SUB_VGIP_VGIP_CHN2_MISC_reg, vgip_chn2_misc_reg.regValue);

			}
			else
			{
				vgip_chn2_misc_reg.regValue = IoReg_Read32(SUB_VGIP_VGIP_CHN2_MISC_reg);
				vgip_chn2_misc_reg.ch2_hporch_num = 0;
				IoReg_Write32(SUB_VGIP_VGIP_CHN2_MISC_reg, vgip_chn2_misc_reg.regValue);
			}
		}
#endif

		scalerip_modify_rdstart(display,imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP,imdSmoothInfo[display].IMD_SMOOTH_DISP_RTNR,is_di_h_scalar_down(display));

#if 1 //marked by rika 20140625 to test if DI affects
		if(imdSmoothInfo[display].IMD_SMOOTH_DISP_THRIP)
		{
			//	smooth_toggle_drvif_color_RTNR_flow((Manual_NR_TABLE+ Scaler_color_GetDNR_table()*DRV_NR_Level_MAX + DRV_NR_OFF));
		}
		else
		{
			//	smooth_toggle_drvif_color_RTNR_flow((Manual_NR_TABLE+ Scaler_color_GetDNR_table()*DRV_NR_Level_MAX + Scaler_color_GetDNR()));
			di_im_di_rtnr_new_control_RBUS rtnr_new_Ctrl;
			rtnr_new_Ctrl.regValue = rtd_inl(DI_IM_DI_RTNR_NEW_CONTROL_reg);
			if(imdSmoothInfo[display].IMD_SMOOTH_H_ACT_WID_PRE > 1920){
				rtnr_new_Ctrl.prtnr_4k2k = 0x1;
			}else{
				rtnr_new_Ctrl.prtnr_4k2k = 0x0;
			}
			rtd_outl(DI_IM_DI_RTNR_NEW_CONTROL_reg, rtnr_new_Ctrl.regValue);
		}
#endif




	}
	else
	{
		if(display == SLR_MAIN_DISPLAY) {
			//remove #ifdef CONFIG_RTNR_NONE rika 20140625 sync from scalerIp.cpp
			// hsliao 20090403, if sub enters DI/RTNR, do not close DI
			if(!(/*Scaler_PipGetInfo(SLR_PIP_ENABLE)&&*/(imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_THRIP||imdSmoothInfo[SLR_SUB_DISPLAY].IMD_SMOOTH_DISP_RTNR))) {
				///fw_scaler_double_buffer_maskl((SCALER_DISP_CHANNEL)display, DI_IM_DI_CONTROL_reg, ~_BIT28, 0); // Close DI
			}

		}

		zoom_scalerip_bypass(TRUE,display);
		sm_go_2d_to_3d_flag = FALSE;//should remove by rika 20140728 not used so far
		rtd_pr_smt_debug("Not Enter DI \n");
	}

	Set_vo_smoothtoggle_changeratio_flag2(_DISABLE,display);//for DI mem allocation rika 20140829 added

}
#endif

/*============================================================================*/
/** rika 20140625 sync from sclaerIp.cpp
 * scalerip_bypass
 * Set whether channel enter IP or not
 *
 * @param <info> { display-info struecture }
 * @param <byPass> { enter/no enter }
 * @return { none }
 *
 */
void zoom_scalerip_bypass(unsigned char byPass,unsigned char display)
{
	vgip_data_path_select_RBUS data_path_select_reg;
	data_path_select_reg.regValue = rtd_inl(VGIP_Data_Path_Select_reg);
	if(byPass){	// bypass
		//if(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)){
		if(display==SLR_SUB_DISPLAY){

			data_path_select_reg.uzd2_in_sel = 0;
		}else{
			data_path_select_reg.uzd1_in_sel = 0;
		}
	}else{
		if(display==SLR_SUB_DISPLAY){
			data_path_select_reg.xcdi_in_sel = 1;
			data_path_select_reg.uzd2_in_sel = 1;
		}else{
			data_path_select_reg.xcdi_in_sel = 0;
			data_path_select_reg.uzd1_in_sel = 1;
		}
	}
	rtd_outl(VGIP_Data_Path_Select_reg, data_path_select_reg.regValue);
}



void scalerVIP_smoothtoggle_init(unsigned char display, unsigned short the_width, unsigned short the_height,unsigned short Cap_width, unsigned short Cap_height,unsigned short MEM_ACT_WID, unsigned short MEM_ACT_LEN)
{

	di_im_di_control_RBUS im_di_control_reg;
	di_im_di_film_new_function_main_RBUS di_im_di_film_new_function_main_reg;
//	scaleup_dm_dir_uzu_ctrl_RBUS dm_dir_uzu_ctrl_Reg;

	#if 0
	_clues* SmartPic_clue=NULL;
	iedge_smooth_edgesmooth_ex_ctrl_RBUS edgesmooth_ex_ctrl_reg;
	SmartPic_clue = scaler_GetShare_Memory_SmartPic_Clue_Struct();

	//=== 20140704 CSFC for vip video fw infomation ====
	SmartPic_clue->S_NetFlix_info.Netflix_CapWid = Cap_width;
	SmartPic_clue->S_NetFlix_info.Netflix_CapLen = Cap_height;
	#endif
	//====================================

	//init. coef. re-setting by timing
	#if 0
	scalerVIP_hist_init(display, Cap_width, Cap_height);
	scalerVIP_MPEG_BLK_init(display, vo_width, vo_height);
	//frank@0904 change below code to solve 4k2k smooth toggle video fail issue
	if(display == _CHANNEL1){
		edgesmooth_ex_ctrl_reg.regValue = rtd_inl(IEDGE_SMOOTH_EDGESMOOTH_EX_CTRL_VADDR);
		if(vo_width <= 2048){
			edgesmooth_ex_ctrl_reg.lbuf_linemode = 2;//11 line spnr
		}else{
			edgesmooth_ex_ctrl_reg.lbuf_linemode = 0;//5 line spnr
		}
		rtd_outl(IEDGE_SMOOTH_EDGESMOOTH_EX_CTRL_VADDR,edgesmooth_ex_ctrl_reg.regValue);
	}
	rtdf_clearBits(DI_IM_DI_HMC_ADJUSTABLE2_VADDR, (_BIT15));
	#endif

	di_im_di_film_new_function_main_reg.regValue = rtd_inl(DI_IM_DI_FILM_NEW_FUNCTION_MAIN_reg);
	im_di_control_reg.regValue = rtd_inl(DI_IM_DI_CONTROL_reg);
	m_film = im_di_control_reg.film_enable;
	g_fw_film_en = di_im_di_film_new_function_main_reg.fw_film_en;

	im_di_control_reg.film_enable = 0;
	im_di_control_reg.write_enable_5=1;
	di_im_di_film_new_function_main_reg.fw_film_en = 0;

	rtd_outl(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);
	rtd_outl(DI_IM_DI_FILM_NEW_FUNCTION_MAIN_reg, di_im_di_film_new_function_main_reg.regValue);
	m_film_clear = 1;

	#if 0
	//auto function smooth toggle flag
	SmartPic_clue->S_NetFlix_info.NetFlix_smooth_Toggle_info.hist_init_flag = 1;
	SmartPic_clue->S_NetFlix_info.NetFlix_smooth_Toggle_info.TimingChange_flag_for_RTNR = 1;
	SmartPic_clue->S_NetFlix_info.NetFlix_smooth_Toggle_info.MPEG_BLK_init = 1;


	if((MA_print_count%debug_print_delay_time==0) && (Start_Print.DCC_3==TRUE))
	{
		rtd_pr_smt_info("the_width=%d\n", vo_width);
		rtd_pr_smt_info("Cap_width=%d\n", Cap_width);
		rtd_pr_smt_info("MEM_ACT_WID=%d\n", MEM_ACT_WID);
	}
	#endif
#if 0
	//directionalscaler ds phase setting for smooth toggle
	dm_dir_uzu_ctrl_Reg.regValue = rtd_inl(SCALEUP_DM_DIR_UZU_CTRL_VADDR);

	if( MEM_ACT_WID <= 960){
		dm_dir_uzu_ctrl_Reg.ds_phase_en = 1; // default setting enable!
	} else {
		dm_dir_uzu_ctrl_Reg.ds_phase_en = 0;
	}

	rtd_outl(SCALEUP_DM_DIR_UZU_CTRL_VADDR, dm_dir_uzu_ctrl_Reg.regValue);
#endif

}

void scalerVIP_smoothtoggle_film_reset(void)
{

	di_im_di_control_RBUS im_di_control_reg;
	di_im_di_film_new_function_main_RBUS di_im_di_film_new_function_main_reg;

	if(m_film_clear ) {

		WaitFor_DEN_STOP();

		rtd_pr_smt_debug("m_film:%d,g_fw_film_en:%d\n",m_film,g_fw_film_en);
	//&& !(smooth_toggle_update_flag_get_enable(SLR_MAIN_DISPLAY))){
		di_im_di_film_new_function_main_reg.regValue = rtd_inl(DI_IM_DI_FILM_NEW_FUNCTION_MAIN_reg);
		im_di_control_reg.regValue = rtd_inl(DI_IM_DI_CONTROL_reg);
		m_film_clear = 0;
		im_di_control_reg.film_enable = m_film;
		im_di_control_reg.write_enable_5=1;
		di_im_di_film_new_function_main_reg.fw_film_en = g_fw_film_en;
		m_film = 0;
		g_fw_film_en=0;
		rtd_outl(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);
		rtd_outl(DI_IM_DI_FILM_NEW_FUNCTION_MAIN_reg, di_im_di_film_new_function_main_reg.regValue);
	}

	return;

}

void imd_smooth_main_double_buffer_all_apply(void)
{
	vgip_vgip_chn1_double_buffer_ctrl_RBUS vgip_chn1_double_buffer_reg;
	di_db_reg_ctl_RBUS  db_reg_ctl_reg;
	//mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;
	//mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
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
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
		//cap_reg_doublbuffer_reg.cap1_db_en=1;
		//cap_reg_doublbuffer_reg.cap1_db_apply=1;
		//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
		nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_apply(0, 1);
	}

	//m domain display
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
		//ddr_mainsubctrl_reg.disp1_double_enable=1;
		//ddr_mainsubctrl_reg.disp1_double_apply=1;
		//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
		nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_apply(0, 1);
	}

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
	//mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;
	//mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
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
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
		//cap_reg_doublbuffer_reg.cap2_db_en=1;
		//cap_reg_doublbuffer_reg.cap2_db_apply=1;
		//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
		nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_apply(1, 1);
	}

	//m domain display
	//merlin8/8p mdomain one-bin
	if(get_mach_type() == MACH_ARCH_RTK2885P) {
		//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
		//ddr_mainsubctrl_reg.disp2_double_enable=1;
		//ddr_mainsubctrl_reg.disp2_double_apply=1;
		//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
		nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_apply(1, 1);
	}

	down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	//DTG
	spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
	double_buffer_ctrl_reg.regValue =rtd_inl(PPOVERLAY_Double_Buffer_CTRL_reg);
	//double_buffer_ctrl_reg.dsubreg_dbuf_en=1;
	double_buffer_ctrl_reg.dsubreg_dbuf_set=1;
	rtd_outl(PPOVERLAY_Double_Buffer_CTRL_reg,double_buffer_ctrl_reg.regValue);
    m8p_dtg_set_sub_double_buffer_apply(DOUBLE_BUFFER_D3);
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


void zoom_imd_smooth_check_di_buffer_mode(void)
{//This is for whether DI mode setting is worng. If wrong, recover the right setting. Recover API
	unsigned char DI_3a_flag = imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_MA3A;
	di_im_di_control_RBUS im_di_control_reg;
	_RPC_system_setting_info *VIP_RPC_system_info_structure_table = NULL;
	VIP_RPC_system_info_structure_table = (_RPC_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);

	if(imdSmoothInfo[SLR_MAIN_DISPLAY].IMD_SMOOTH_DISP_THRIP)
	{
		IoReg_ClearBits(DI_db_reg_ctl_reg, _BIT2);//Let read value be HW setting
		im_di_control_reg.regValue = IoReg_Read32(DI_IM_DI_CONTROL_reg);
		if(im_di_control_reg.ma_3aenable!= DI_3a_flag)
		{
			rtd_pr_smt_notice("\r\n#####DI buffer mode need recover(%d)####\r\n", DI_3a_flag);
			down(get_DI_semaphore());
			if(DI_3a_flag) {
				im_di_control_reg.ma_3aenable = 1;
				im_di_control_reg.write_enable_1 = 1;
				IoReg_Write32(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);

			} else {
				im_di_control_reg.ma_3aenable = 0;
				im_di_control_reg.write_enable_1 = 1;
				IoReg_Write32(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);
			}
			up(get_DI_semaphore());
		}
		IoReg_SetBits(DI_db_reg_ctl_reg, _BIT2);//Let read value be rbus setting

		if(VIP_RPC_system_info_structure_table == NULL){
			rtd_pr_smt_debug("[%s:%d] Warning here!! RPC_system_info=NULL! \n",__FILE__, __LINE__);
		}else{
			VIP_RPC_system_info_structure_table->SCPU_ISRIINFO_TO_VCPU.DI_info.DI_ma_3aenable = im_di_control_reg.ma_3aenable;
		}

	}
}

void imd_smooth_main_double_buffer_apply(zoom_double_buffer_apply_ip apply_ip)
{
	vgip_vgip_chn1_double_buffer_ctrl_RBUS vgip_vgip_chn1_double_buffer_ctrl_reg;
	di_db_reg_ctl_RBUS  db_reg_ctl_reg;
	//mdomain_cap_cap_reg_doublbuffer_RBUS cap_reg_doublbuffer_reg;
	//mdomain_disp_ddr_mainsubctrl_RBUS ddr_mainsubctrl_reg;
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
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//cap_reg_doublbuffer_reg.regValue =rtd_inl(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
			//cap_reg_doublbuffer_reg.cap1_db_apply=1;
			//rtd_outl(MDOMAIN_CAP_cap_reg_doublbuffer_reg,cap_reg_doublbuffer_reg.regValue);
			nonlibdma_set_cap_reg_doublbuffer_reg_doublebuffer_apply(0, 1);
		}
	  	break;
	  case ZOOM_M_DISP:
		//merlin8/8p mdomain one-bin
		if(get_mach_type() == MACH_ARCH_RTK2885P) {
			//ddr_mainsubctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
			//ddr_mainsubctrl_reg.disp1_double_apply=1;
			//rtd_outl(MDOMAIN_DISP_DDR_MainSubCtrl_reg,ddr_mainsubctrl_reg.regValue);
			nonlibdma_set_disp_ddr_mainsubctrl_reg_doublebuffer_apply(0, 1);
		}
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

