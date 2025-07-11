/*Kernel Header file*/
#ifndef BUILD_QUICK_SHOW
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
#include <linux/kthread.h>
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include <linux/string.h>/*memset*/
#include <linux/init.h>
#include <linux/spinlock_types.h>/*For spinlock*/
#include <linux/suspend.h>
#else
#include <sysdefs.h>
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
#endif

#include "memc_isr/Platform/memc_change_size.h"


/*RBUS Header file*/

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
	#include <scaler/scalerDrvCommon.h>
#else
#include <scalercommon/scalerCommon.h>
#include <scalercommon/scalerDrvCommon.h>
#endif


#include <rbus/mdomain_cap_reg.h>
#include <rbus/memc_mux_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/di_reg.h>
#include <rbus/efuse_reg.h>
#include <tvscalercontrol/scaler/scalercolorlib.h>
#include <tvscalercontrol/scaler/scalercolorlib_tv001.h>
#include <rbus/sys_reg_reg.h>
//#include <rbus/mc_dma_reg.h>
//#include <rbus/me_share_dma_reg.h>
#ifndef CONFIG_MEMC_NOTSUPPORT
#include <memc_reg_def.h>
#endif
#include "memc_isr/HAL/hal_dm.h"
#include "memc_isr/MID/mid_mode.h"
#ifndef BUILD_QUICK_SHOW
#include "memc_isr/PQL/PQLPlatformDefs.h"
#endif
#include <rbus/mdomain_disp_reg.h>

/*TVScaler Header file*/
#include "scaler_vpqmemcdev.h"
#include "ioctrl/vpq/vpq_memc_cmd_id.h"
#include "scaler_vscdev.h"
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/vip/memc_mux.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scalerdrv/scaler_dtg.h>
#include <tvscalercontrol/panel/panelapi.h>
#include "scaler_vbedev.h"
#include <scaler/vipRPCCommon.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/framesync.h>
#include <vo/rtk_vo.h>

#ifndef CONFIG_MEMC_NOTSUPPORT
#include <rbus/dbus_wrapper_reg.h>
#endif

#define TAG_NAME "MEMC"

//#define RUN_MERLIN_MEMC_ENABLE
#define MEMC_DEBUG

#ifdef MEMC_DEBUG
#define MEMC_DBG_PRINT(s, args...) rtd_pr_memc_debug( s, ## args)
#else
#define MEMC_DBG_PRINT(s, args...)
#endif

#include "memc_isr/scalerMEMC.h"
#include "memc_isr/Common/memc_type.h"
#include "memc_isr/MID/mid_misc.h"

#if RTK_MEMC_Performance_tunging_from_tv001
#include "memc_isr/Platform/memc_change_size.h"
#endif


#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
#ifndef CONFIG_MEMC_NOTSUPPORT
#include "memc_reg_def.h"
static unsigned char m_bForce24to48=FALSE;
#endif
#endif


#if 0
#define bisr_rstn_addr_kme 	(0xb8000f00)
#define IP_rstn_bits_kme 	(_BIT10)
#define bisr_remap_addr_kme	(0xb8000f10)
#define IP_remap_bits_kme 	(_BIT10)
#define bisr_done_addr_kme 	(0xb8000f20)
#define IP_done_bits_kme 	(_BIT10)
#define bisr_repair_addr_kme (0xb809d0e4)
#define IP_repair_check_bits_kme (_BIT26)
#define bisr_fail_addr_kme 	(0xb809d0e4)
#define IP_fail_bits_kme 	(_BIT24)

#define bisr_rstn_addr_kmc 	(0xb8000f00)
#define IP_rstn_bits_kmc 	(_BIT8|_BIT7|_BIT4|_BIT3|_BIT2|_BIT1)
#define bisr_remap_addr_kmc	(0xb8000f10)
#define IP_remap_bits_kmc 	(_BIT8|_BIT7|_BIT4|_BIT3|_BIT2|_BIT1)
#define bisr_done_addr_kmc 	(0xb8000f20)
#define IP_done_bits_kmc 	(_BIT8|_BIT7|_BIT4|_BIT3|_BIT2|_BIT1)
#define bisr_repair_addr_kmc (0xb8099098)
#define IP_repair_check_bits_kmc (_BIT8|_BIT7|_BIT6|_BIT5|_BIT4|_BIT3|_BIT2|_BIT1|_BIT0)
#define bisr_fail_addr_kmc 	(0xb809909c)
#define IP_fail_bits_kmc 	(_BIT7|_BIT6|_BIT3|_BIT2|_BIT1|_BIT0)
#endif

#ifndef BUILD_QUICK_SHOW
static struct cdev vpq_memc_cdev;
#endif

int vpq_memc_major   = 0;
int vpq_memc_minor   = 0 ;
int vpq_memc_nr_devs = 1;
unsigned char g_MEMC_DMA_state = 1;
//Reset flow : CLKEN = "0" 'RSTN = "0" 'RSTN = "1" ' CLKEN = "1"
unsigned int HDMI_PowerSaving_stage = 0;
unsigned int DTV_PowerSaving_stage = 0;

static struct semaphore vpq_memc_Semaphore;
static struct semaphore livezoom_memcclose_Semaphore;
extern struct semaphore Memc_Realcinema_Semaphore;
extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr;//extern char memc_realcinema_run_flag;//Run memc mode
#ifndef BUILD_QUICK_SHOW
module_param(vpq_memc_major, int, S_IRUGO);
module_param(vpq_memc_minor, int, S_IRUGO);
module_param(vpq_memc_nr_devs, int, S_IRUGO);
#endif
static struct class *vpq_memc_class;
unsigned char MEMC_First_Run_Done=0;//using for booting to avoid second initial
unsigned char MEMC_First_Run_force_SetInOutUseCase_Done=0;
unsigned char MEMC_First_Run_FBG_enable=0;//using for booting fbg checking
static unsigned char MEMC_instanboot_resume_Done=0;
static unsigned char MEMC_snashop_resume_Done=0;

static VPQ_MEMC_TYPE_T MEMC_motion_type = VPQ_MEMC_TYPE_OFF;
static char AVD_Unstable_Check_MEMC_OutBG_flag = FALSE;
extern unsigned char fwif_color_get_cinema_mode_en(void);
extern bool is_QS_scaler_enable(void);
unsigned char MEMC_instanboot_resume_check[10]={0};

#if 1//RTK_MEMC_Performance_tunging_from_tv001
extern unsigned int vpqex_project_id;
bool g_RESET_MEMC_FLG = false;
bool g_KERNEL_AFTER_QS_MEMC_FLG = false;
bool g_QS_PCMODE_FLG = false;	//Scaler_VPQ_check_PC_RGB444
static unsigned char g_MEMC_powersaving_on = 0;
//static unsigned char glb_vpq_lowdelay_flag = 0;
static unsigned char MEMC_Performance_Checking_Database_index = 0;
unsigned char g_scaler_vpq_memc_cadence[_FILM_MEMC_NUM] = {0};

MEMC_INFO_FROM_SCALER glb_Scaler_InfoFromScaler;
#endif
static unsigned int DISP_DHTotal = 0;//k23

#ifndef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
static unsigned int gphy_addr_kme, gphy_addr_kmc00, gphy_addr_kmc01;
static unsigned char m_bForce24to48=FALSE;
static unsigned char MEMC_resume_check=0;
#endif

bool g_new_sport_mode = false;

#if 1  // MEMC CLK OFF CTRL
unsigned char bMemcClkTask_ForceExitFlag=0, bMemcClkTask_memcClkEn=0, bMemcClkTask_memcMuteFlag=0;
struct semaphore MEMC_CLK_Semaphore;
#endif

extern unsigned int HDMI_PowerSaving_stage;
extern int g_memc_hdmi_switch_state;
extern int g_memc_switch_state;
extern unsigned int DTV_PowerSaving_stage;
extern unsigned char g_PCMode_flag;
extern VOID Mid_MISC_SetInINTEnable(INT_TYPE enIntType, BOOL bEnable);
extern VOID Mid_MISC_SetOutINTEnable(INT_TYPE enIntType, BOOL bEnable);
extern VOID Mid_MISC_SetInputIntp_en(BOOL bEnable);
extern VOID Mid_MISC_SetOnefiftIntp_en(BOOL bEnable);
extern VOID Mid_MISC_SetOutputIntp_en(BOOL bEnable);
extern unsigned char drvif_color_Get_Two_Step_UZU_Mode(void);
extern SCALER_DATA_FS_ALGO_RESULT g_tDataFsAlgoResult;
extern unsigned char get_MEMC_bypass_status_refer_platform_model(void);
extern DIRECT_VO_FRAME_ORIENTATION get_rotate_mode(unsigned char display);
extern int Scaler_MEMC_MEMC_CLK_OnOff(unsigned char ME_enable, unsigned char MC_enable, unsigned char bMute);
extern VOID Scaler_MEMC_Set_EnableToBypass_Flag(unsigned char u1_enable);
extern void MEMC_LibSet_MCDMA_DB_apply(void);
extern VOID FRC_phTable_set_LineModeCtrl(VOID);
extern unsigned int Get_DISPLAY_REFRESH_RATE(void);
extern void Scaler_set_D1_Vporch_shrink(unsigned char mode, unsigned int line);
extern bool MEMC_Lib_Get_QS_PCmode_Flg(void);
extern void MEMC_Lib_Freeze(unsigned char enable);

#ifndef BUILD_QUICK_SHOW
/*******************************************************************************
****************************VPQ MEMC DRIVER************************************
*******************************************************************************/
//#ifdef CONFIG_ENABLE_MEMC
void HAL_VPQ_MEMC_Initialize(void){
	if(MEMC_First_Run_Done == 0){
		rtd_pr_memc_debug( "[MEMC][init]%s %d[%d]\n",__func__,__LINE__,MEMC_First_Run_Done);
		MEMC_First_Run_Done = 1;
		MEMC_First_Run_FBG_enable = _ENABLE;
		Scaler_MEMC_output_force_bg_enable(TRUE,__func__,__LINE__);
		Scaler_MEMC_initial_Case();
		rtd_pr_memc_debug( "[MEMC][init]HAL_VPQ_MEMC_Initialize Done!![%d]\n",MEMC_First_Run_Done);
	}
}

void HAL_VPQ_MEMC_Uninitialize(void){

}

void memc_setting_displayDtiming_framerate(void)
{
	unsigned int vfreq_ori = 0;
	vfreq_ori = Scaler_DispGetInputInfo(SLR_INPUT_DTG_MASTER_V_FREQ);
	modestate_decide_dtg_m_mode();
	if(vfreq_ori != Scaler_DispGetInputInfo(SLR_INPUT_DTG_MASTER_V_FREQ))
		scaler_disp_smooth_variable_settingByDisp(0);
}

unsigned int memc_realcinema_get_current_framerate(void)
{
	ppoverlay_double_buffer_ctrl_RBUS ppoverlay_double_buffer_ctrl_reg;
	ppoverlay_dv_total_RBUS dv_total_reg;
	unsigned int vtotal;
	unsigned int frameRates;
	unsigned long flags;//for spin_lock_irqsave

	//down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
	ppoverlay_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
	ppoverlay_double_buffer_ctrl_reg.dreg_dbuf_read_sel = 1;  //D7_read_sel
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL_reg, ppoverlay_double_buffer_ctrl_reg.regValue);
	dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_DV_total_reg);
	vtotal = dv_total_reg.dv_total;
	spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
	//up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	frameRates = (((Get_DISPLAY_CLOCK_TYPICAL()/Get_DISP_HORIZONTAL_TOTAL())*1000)/vtotal);
	frameRates /= 1000;

	if((frameRates>=47) && (frameRates<=49)){
		frameRates = 48;
	}else if((frameRates>49) && (frameRates<51)){
		frameRates = 50;
	}else if(((frameRates>=51) && (frameRates<=59))||
		((frameRates>=45) && (frameRates<=47))){
		// apply it.
	}
	else if((frameRates>59) && (frameRates<61)){
		frameRates = 60;
	}else{
		frameRates = 60;
	}

	rtd_pr_memc_err( "memc_realcinema_get_current_framerate=%d!!\n", frameRates);
	return frameRates;
}

/**
 *@brief memc_realcinema_framerate Set framerate while turnning on/off real-cinema
 *@return VOID
**/
void memc_realcinema_framerate(void)
{
	extern void scaler_config_video_latency_info_for_memc(KADP_VSC_INPUT_TYPE_T inputSrctype);

	/*scaler timing setting is finish?*/
	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_STATE) != _MODE_STATE_ACTIVE)
		return;

	//120hz panel no real cinema dtg mode
	if(Get_DISPLAY_REFRESH_RATE() == 120)
		return;

	if((Scaler_get_realcinema_mode_condition()
		 && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ) == 48))
		 || (!Scaler_get_realcinema_mode_condition()
		 && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ) == 60)))
	{
		 rtd_pr_memc_err( "[Real_Cinema] The same mode, do not change settings. mode=%d, DTG_M=%d \n", Scaler_get_realcinema_mode_condition(), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ));
		 return;
	}

	/*input frame rate must be 24hz or output is 48hz*/
	if( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 48 ) ||
		( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) >= V_FREQ_24000_mOFFSET ) && ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) < V_FREQ_24500 ) ) )
	{
		unsigned int vfreq_ori = 0;
		vfreq_ori = memc_realcinema_get_current_framerate(); //Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ);
		modestate_decide_dtg_m_modeByDisp((unsigned char)SLR_MAIN_DISPLAY);
		//CP or VDEC data fs mode, run cinema change at VCPU
		if((vbe_disp_get_adaptivestream_fs_mode())
			||((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC)
			&& (get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) == 1))
			||(vbe_get_VDEC4K_run_datafs_without_gatting_condition())){
			if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 48){
				Scaler_Set_Cinema_Mode(_ENABLE, _ENABLE);
			}
			else{
				Scaler_Set_Cinema_Mode(_DISABLE, _ENABLE);
			}
		}
		else{
			if(1){//Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_FRAMESYNC) == FALSE)	{
				if(vfreq_ori != Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ)){
					extern void scaler_set_I2D_tracking_in_realcinema(unsigned char bEnable);
					scaler_set_I2D_tracking_in_realcinema(TRUE);
					scaler_disp_smooth_variable_settingByDisp((unsigned char)SLR_MAIN_DISPLAY);
		        }
			}
		}
		//update memc video latency info
		scaler_config_video_latency_info_for_memc((KADP_VSC_INPUT_TYPE_T)Get_DisplayMode_Src(SLR_MAIN_DISPLAY));
		rtd_pr_memc_err( "vfreq_ori=%d, DTG_M=%d \n", vfreq_ori, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ));
	}
}

//k23
void memc_realcinema_cinematic_framerate(void)
{
    unsigned int source_vfreq = 0;
	extern void scaler_config_video_latency_info_for_memc(KADP_VSC_INPUT_TYPE_T inputSrctype);

	/*scaler timing setting is finish?*/
	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_STATE) != _MODE_STATE_ACTIVE)
		return;

	if( ( ( Scaler_get_realcinema_mode_condition() /*|| Scaler_get_cinematic_mode_condition()*/ )
		&& ( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 48 ) || ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 96 ) ) )
	|| ( ( !Scaler_get_realcinema_mode_condition() /*&& !Scaler_get_cinematic_mode_condition()*/ )
		&& ( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 60 ) || ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 120 ) ) ) )
	{
		 rtd_pr_memc_err( "[Real_Cinema] The same mode, do not change settings. mode=%d, DTG_M=%d \n", Scaler_get_realcinema_mode_condition(), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ));
		 return;
	}

    if(0/*Scaler_get_cinematic_mode_condition()*/)
    {
        SLR_VOINFO *pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
        if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI){
            source_vfreq = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVFREQ_1000);
        }
        else{
            source_vfreq = pVOInfo->src_v_freq_1000;
        }
    }
    else
    {
        source_vfreq = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000);
    }

	/*input frame rate must be 24hz or output is 48hz*/
	if( ( ( source_vfreq >= V_FREQ_24000_mOFFSET ) && ( source_vfreq < V_FREQ_24500 ) )
	|| ( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 48 ) || ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 96 ) ) )
	{
		unsigned int vfreq_ori = 0;
		vfreq_ori = memc_realcinema_get_current_framerate(); //Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ);
		modestate_decide_dtg_m_modeByDisp((unsigned char)SLR_MAIN_DISPLAY);
		//CP or VDEC data fs mode, run cinema change at VCPU
		if((vbe_disp_get_adaptivestream_fs_mode())
			||((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC)
			&& (get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) == 1))
			||(vbe_get_VDEC4K_run_datafs_without_gatting_condition())){
			if( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 48 ) || ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_DTG_MASTER_V_FREQ) == 96 ) )
			{
				rtd_pr_memc_err("[Real_Cinema]Enable RealCinema/Cinematic");
				Scaler_Set_Cinema_Mode(_ENABLE, _ENABLE);
			}
			else
			{
				rtd_pr_memc_err("[Real_Cinema]Disable RealCinema/Cinematic");
				Scaler_Set_Cinema_Mode(_DISABLE, _ENABLE);
			}
		}
		else{
			if(1){//Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_FRAMESYNC) == FALSE)	{
				if(vfreq_ori != Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ)){
					extern void scaler_set_I2D_tracking_in_realcinema(unsigned char bEnable);
					scaler_set_I2D_tracking_in_realcinema(TRUE);
					scaler_disp_smooth_variable_settingByDisp((unsigned char)SLR_MAIN_DISPLAY);
		        }
			}
		}
		//update memc video latency info
		scaler_config_video_latency_info_for_memc((KADP_VSC_INPUT_TYPE_T)Get_DisplayMode_Src(SLR_MAIN_DISPLAY));
		rtd_pr_memc_err( "vfreq_ori=%d, DTG_M=%d \n", vfreq_ori, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_DTG_MASTER_V_FREQ));
	}
}
#endif

extern BOOL MEMC_LibGetMEMCLowDelayModeEnable(VOID);
long vpq_memc_set_lowDelayMode(UINT8 type)
{

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else

	int ret = 0;
	unsigned int semaphoreCnt = 0;
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC	
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
	#endif
#endif

	while((IoReg_Read32(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg)&_BIT30) && semaphoreCnt < 100000) {
		semaphoreCnt++;
	}
	if(semaphoreCnt >= 100000) {
		rtd_pr_memc_debug( "[MEMC]HAL_VPQ_MEMC_LowDelayMode timed out!\n");
		return -1;
	}
	IoReg_SetBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_LOWDELAYMODE, type, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_LOWDELAYMODE RPC fail !!!\n", ret);
	}
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return 0;
	Scaler_MEMC_LowDelayMode(type);
#else

#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
		task.type = SCALERIOC_MEMC_LOWDELAYMODE;
		task.data.value = type;
		Scaler_MEMC_PushTask(&task);
#else
		if((vbe_disp_get_stop_memc_push_flag() == 1) && ((rtd_inl(0xB800011c)>>30)==0)){
			rtd_pr_memc_notice("[%s][%d]VRR Status Modify!\n",__FUNCTION__, __LINE__);
			Scaler_SetMEMCLowDelay(type);
			Scaler_MEMC_LowDelayMode(type);
		}else{
			task.type = SCALERIOC_MEMC_LOWDELAYMODE;
			task.data.value = type;
			Scaler_SetMEMCLowDelay(type);
			Scaler_MEMC_PushTask(&task);
		}
#endif


#endif
#endif
#endif

	IoReg_ClearBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
	return ret;
#endif	
}

unsigned char VR360_en = 0;
unsigned char VR360_stage = 0;
unsigned char VR360_wait_flag = 0;
extern unsigned char u8_MEMCMode;
extern unsigned int dejudder;
extern VOID Scaler_MEMC_Set_BlueScreen(unsigned char is_BlueScreen);
extern _system_setting_info* scaler_GetShare_Memory_system_setting_info_Struct(void);

#ifndef BUILD_QUICK_SHOW

unsigned char LiveZoom_MEMCClose_Flag = FALSE;

struct semaphore* get_livezoom_memcclose_Semaphore(void)
{
	return &livezoom_memcclose_Semaphore;
}

void MEMC_LoadScriptDMA(void)
{

#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
	#if 0
	rtd_pr_memc_notice("[MEMC_LoadScriptDMADisable]\n");
	rtd_setbits(MC_DMA_MC_WDMA_DB_CTRL_reg, _BIT0); //db_en
	rtd_setbits(MC_DMA_MC_RDMA_DB_CTRL_reg, _BIT0); //db_en

	//rtd_outl(MC_DMA_MC_LF_DMA_WR_Ctrl_reg,0x00000300); //dma_off mc w
	//rtd_outl(MC_DMA_MC_HF_DMA_WR_Ctrl_reg,0x00000300); //dma_off mc w
	rtd_clearbits(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg, _BIT0); //dma_off mc r
	rtd_clearbits(MC_DMA_MC_HF_I_DMA_RD_Ctrl_reg, _BIT0); //dma_off mc r
	//rtd_outl(MC_DMA_MC_LF_P_DMA_RD_Ctrl_reg,0x00000300); //dma_off mc r
	//rtd_outl(MC_DMA_MC_HF_P_DMA_RD_Ctrl_reg,0x00000300); //dma_off mc r

	rtd_clearbits(0xb809B794, _BIT0); //dma_off  me share w
	rtd_clearbits(0xb809B7E0, _BIT0); //dma_off me sharer
	//rtd_outl(KME_DM_TOP0_KME_00_AGENT_reg,0x0021001d); //00 [20]
	//rtd_outl(KME_DM_TOP0_KME_01_AGENT_reg,0x0001001d); //01
	rtd_clearbits(KME_DM_TOP0_KME_02_AGENT_reg, _BIT20); //02
	rtd_clearbits(KME_DM_TOP0_KME_03_AGENT_reg, _BIT20); //03
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_14_reg, _BIT20); //08
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_1C_reg, _BIT20); //09
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_38_reg, _BIT20); //10
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_40_reg, _BIT20); //11
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_5C_reg, _BIT20); //12
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_64_reg, _BIT20); //13
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_80_reg, _BIT20); //14
	rtd_clearbits(KME_DM_TOP1_KME_DM_TOP1_88_reg, _BIT20); //15

	rtd_clearbits(KME_DM_TOP2_MV01_AGENT_reg, _BIT20);  //mv01 [20]
	rtd_clearbits(KME_DM_TOP2_MV02_AGENT_reg, _BIT20);  //mv02 [20]
	rtd_clearbits(KME_DM_TOP2_MV03_AGENT_reg, _BIT20);  //mv03 [20]
	rtd_clearbits(KME_DM_TOP2_MV04_AGENT_reg, _BIT20);  //mv04
	rtd_clearbits(KME_DM_TOP2_MV05_AGENT_reg, _BIT20);  //mv05
	rtd_clearbits(KME_DM_TOP2_MV06_AGENT_reg, _BIT20);  // ..
	rtd_clearbits(KME_DM_TOP2_MV07_AGENT_reg, _BIT20);
	rtd_clearbits(KME_DM_TOP2_MV08_AGENT_reg, _BIT20);
	rtd_clearbits(KME_DM_TOP2_MV09_AGENT_reg, _BIT20);
	rtd_clearbits(KME_DM_TOP2_MV10_AGENT_reg, _BIT20);
	rtd_clearbits(KME_DM_TOP2_MV11_AGENT_reg, _BIT20);  //mv11
	rtd_clearbits(KME_DM_TOP2_MV12_AGENT_reg, _BIT20);  //mv12

	rtd_setbits(MC_DMA_MC_WDMA_DB_CTRL_reg, _BIT2); //db_apply
	rtd_setbits(MC_DMA_MC_RDMA_DB_CTRL_reg, _BIT2); //db_apply

	rtd_clearbits(MC_DMA_MC_WDMA_DB_CTRL_reg, _BIT0); //db_off
	rtd_clearbits(MC_DMA_MC_RDMA_DB_CTRL_reg, _BIT0); //db_off
#endif
#else
	rtd_pr_memc_notice("[jerry_MEMC_LoadScriptDMA]\n");
	rtd_setbits(0xb80994F0, _BIT0); //db_en
	rtd_setbits(0xb809AEA8, _BIT0); //db_en

	rtd_setbits(0xb8099460, _BIT0); //dma_off mc w
	rtd_setbits(0xb80994dc, _BIT0); //dma_off mc w
	rtd_setbits(0xb809ae1c, _BIT0); //dma_off mc r
	rtd_setbits(0xb809ae3c, _BIT0); //dma_off mc r
	rtd_setbits(0xb809ae5c, _BIT0); //dma_off mc r
	rtd_setbits(0xb809ae7c, _BIT0); //dma_off mc r
	rtd_setbits(0xb809B794, _BIT0); //dma_off  me share w
	rtd_setbits(0xb809B7E0, _BIT0); //dma_off me sharer

	rtd_setbits(0xb809c038, _BIT20); //00 [20]
	rtd_setbits(0xb809c040, _BIT20); //01
	rtd_setbits(0xb809c048, _BIT20); //02
	rtd_setbits(0xb809c050, _BIT20); //03
	rtd_setbits(0xb809c114, _BIT20); //08
	rtd_setbits(0xb809c11c, _BIT20); //09
	rtd_setbits(0xb809c138, _BIT20); //10
	rtd_setbits(0xb809c140, _BIT20); //11
	rtd_setbits(0xb809c15c, _BIT20); //12
	rtd_setbits(0xb809c164, _BIT20); //13
	rtd_setbits(0xb809c180, _BIT20); //14
	rtd_setbits(0xb809c188, _BIT20); //15

	rtd_setbits(0xb809c210, _BIT20);  //mv01 [20]
	rtd_setbits(0xb809c224, _BIT20);  //mv02 [20]
	rtd_setbits(0xb809c228, _BIT20);  //mv03 [20]
	rtd_setbits(0xb809c244, _BIT20);  //mv04
	rtd_setbits(0xb809c260, _BIT20);  //mv05
	rtd_setbits(0xb809c264, _BIT20);  // ..
	rtd_setbits(0xb809c268, _BIT20);
	rtd_setbits(0xb809c26c, _BIT20);
	rtd_setbits(0xb809c270, _BIT20);
	rtd_setbits(0xb809c274, _BIT20);
	rtd_setbits(0xb809c278, _BIT20);  //mv11
	rtd_setbits(0xb809c28c, _BIT20);  //mv12

	rtd_setbits(0xb80994F0, _BIT2); //db_apply
	rtd_setbits(0xb809AEA8, _BIT2); //db_apply

	rtd_clearbits(0xb80994F0, _BIT0); //db_off
	rtd_clearbits(0xb809AEA8, _BIT0); //db_off
#endif
	


}

void MEMC_LoadScriptDMADisable(void)
{
	rtd_pr_memc_notice("[jerry_MEMC_LoadScriptDMADisable]\n");
	rtd_setbits(0xb80994F0, _BIT0); //db_en
	rtd_setbits(0xb809AEA8, _BIT0); //db_en

	//rtd_outl(0xb8099460,0x00000300); //dma_off mc w
	//rtd_outl(0xb80994dc,0x00000300); //dma_off mc w
	rtd_clearbits(0xb809ae1c, _BIT0); //dma_off mc r
	rtd_clearbits(0xb809ae3c, _BIT0); //dma_off mc r
	//rtd_outl(0xb809ae5c,0x00000300); //dma_off mc r
	//rtd_outl(0xb809ae7c,0x00000300); //dma_off mc r

	rtd_clearbits(0xb809B794, _BIT0); //dma_off  me share w
	rtd_clearbits(0xb809B7E0, _BIT0); //dma_off me sharer
	//rtd_outl(0xb809c038,0x0021001d); //00 [20]
	//rtd_outl(0xb809c040,0x0001001d); //01
	rtd_clearbits(0xb809c048, _BIT20); //02
	rtd_clearbits(0xb809c050, _BIT20); //03
	rtd_clearbits(0xb809c114, _BIT20); //08
	rtd_clearbits(0xb809c11c, _BIT20); //09
	rtd_clearbits(0xb809c138, _BIT20); //10
	rtd_clearbits(0xb809c140, _BIT20); //11
	rtd_clearbits(0xb809c15c, _BIT20); //12
	rtd_clearbits(0xb809c164, _BIT20); //13
	rtd_clearbits(0xb809c180, _BIT20); //14
	rtd_clearbits(0xb809c188, _BIT20); //15

	rtd_clearbits(0xb809c210, _BIT20);  //mv01 [20]
	rtd_clearbits(0xb809c224, _BIT20);  //mv02 [20]
	rtd_clearbits(0xb809c228, _BIT20);  //mv03 [20]
	rtd_clearbits(0xb809c244, _BIT20);  //mv04
	rtd_clearbits(0xb809c260, _BIT20);  //mv05
	rtd_clearbits(0xb809c264, _BIT20);  // ..
	rtd_clearbits(0xb809c268, _BIT20);
	rtd_clearbits(0xb809c26c, _BIT20);
	rtd_clearbits(0xb809c270, _BIT20);
	rtd_clearbits(0xb809c274, _BIT20);
	rtd_clearbits(0xb809c278, _BIT20);  //mv11
	rtd_clearbits(0xb809c28c, _BIT20);  //mv12

	rtd_setbits(0xb80994F0, _BIT2); //db_apply
	rtd_setbits(0xb809AEA8, _BIT2); //db_apply

	rtd_clearbits(0xb80994F0, _BIT0); //db_off
	rtd_clearbits(0xb809AEA8, _BIT0); //db_off
}

//static unsigned int gphy_addr_kme, gphy_addr_kmc00, gphy_addr_kmc01;
extern unsigned int GS_MC_8_buffer;
void MEMC_ChangeMCBufferAddr(unsigned char u1_is_changed_to_6_buffer)
{

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC


	extern BOOL MEMC_LibSetInOutMode(MEMC_INOUT_MODE emom,BOOL bForce);
	extern VOID MEMC_MidSetPCOff(VOID);
	extern VOID Mid_Mode_SetMEMC_PCModeEnable(BOOL bEnable);

	if(u1_is_changed_to_6_buffer){
		//set cadence = auto
		rtd_clearbits(SOFTWARE_SOFTWARE_00_reg, _BIT0);
		//b-size
		if(Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN){
			if(g_PCMode_flag == 1){// from VR360 to PC freerun mode
				MEMC_LibSetInOutMode(MEMC_RGB_IN_PC_FREERUN_OUT,TRUE);
				Mid_Mode_SetMEMC_PCModeEnable(TRUE);
				rtd_pr_memc_notice("$$$   [jerry_VR360_test01_pc_buffer_freerun_mode]   $$$");
			}
		}else{
			if(g_PCMode_flag == 1){// from VR360 to PC
				MEMC_LibSetInOutMode(MEMC_RGB_IN_PC_OUT,TRUE);
				Mid_Mode_SetMEMC_PCModeEnable(TRUE);
				rtd_pr_memc_notice("$$$   [jerry_VR360_test01_pc_buffer_mode]   $$$");
			}else{//from VR360 to normal
				MEMC_Lib_SetKphaseME1IndexBsize(8);
				MEMC_Lib_SetKphaseME2IndexBsize(8);
				if(GS_MC_8_buffer){
					MEMC_Lib_SetKphaseMCIndexBsize(8);
				}else{
					MEMC_Lib_SetKphaseMCIndexBsize(6);
				}
				rtd_pr_memc_notice("$$$   [jerry_VR360_test01_6_buffer_mode]   $$$");
			}
		}
		//rtd_setbits(0xb809d410, _BIT13);
	}
	else{
		if(g_PCMode_flag == 1){// from pc to VR360, reset DMA to normal mode first.
			if(Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN){
				MEMC_LibSetInOutMode(MEMC_RGB_IN_VIDEO_FREERUN_OUT,FALSE);
			}else{
				MEMC_LibSetInOutMode(MEMC_RGB_IN_VIDEO_OUT,FALSE);
			}
			MEMC_MidSetPCOff();
			Mid_Mode_SetMEMC_PCModeEnable(FALSE);
		}
		//set cadence = 0
		rtd_setbits(SOFTWARE_SOFTWARE_00_reg, _BIT0);
		//b-size
		MEMC_Lib_SetKphaseME1IndexBsize(4);
		MEMC_Lib_SetKphaseME2IndexBsize(4);
		MEMC_Lib_SetKphaseMCIndexBsize(4);
		//rtd_clearbits(0xb809d410, _BIT13);
		rtd_pr_memc_notice("$$$   [jerry_VR360_test02_4_buffer_mode]   $$$");
	}
#endif
#endif
}


void MEMC_Set_malloc_address(UINT8 status)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
				return ;
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

#if 1
	unsigned int time1 = 0;
	unsigned int time_out = 30;
	static unsigned int last_mode = 0;
	static unsigned char me_clock_state_pre = 0, mc_clock_state_pre = 0;


#if RTK_MEMC_Performance_tunging_from_tv001
		//static unsigned int last_dejudder = 0;
#else
		static unsigned int last_dejudder = 0;
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
	//unsigned int height = 0;
	//unsigned int width = 0;
#else
	unsigned int height = 0;
	unsigned int width = 0;
#endif

	_system_setting_info* system_info_structure_table = NULL;
	system_info_structure_table = scaler_GetShare_Memory_system_setting_info_Struct();
	if(system_info_structure_table == NULL){
		return;
	}

#if RTK_MEMC_Performance_tunging_from_tv001
	//height = system_info_structure_table->I_Height;
	//width = system_info_structure_table->I_Width;
#else
	height = system_info_structure_table->I_Height;
	width = system_info_structure_table->I_Width;
#endif

	

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return;
//#endif

	while(((g_memc_hdmi_switch_state != 0) || (HDMI_PowerSaving_stage != 0) || (g_memc_switch_state != 0) || (DTV_PowerSaving_stage != 0)) && (--time_out != 0)){
		//msleep(1);
		mdelay(1);
	}

	//Mid_MISC_SetInINTEnable(INT_TYPE_VSYNC, FALSE);
	//Mid_MISC_SetOutINTEnable(INT_TYPE_VSYNC, FALSE);
	Mid_MISC_SetInputIntp_en(FALSE);
	Mid_MISC_SetOnefiftIntp_en(FALSE);
	Mid_MISC_SetOutputIntp_en(FALSE);
	rtd_pr_memc_notice("[%s][AAA]d5fc=[%x]\n", __FUNCTION__, rtd_inl(0xb809d5fc));
	rtd_pr_memc_notice("[%s][AAA][%x, %x]\n", __FUNCTION__, rtd_inl(0xb8099018), rtd_inl(0xb809d008));

	time1 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

	//mute

#if RTK_MEMC_Performance_tunging_from_tv001
//	Scaler_MEMC_Set_BlueScreen(1);
#else
//	MEMC_LibBlueScreen(1);
#endif

	if(status == TRUE){
		//turn on
		rtd_pr_memc_notice("[MEMC_Set_malloc_address_01][,pre_ME_clock=%d,pre_MC_clock=%d,pre_mode=%d][pc=%d]\n", me_clock_state_pre, mc_clock_state_pre, last_mode, g_PCMode_flag);
		VR360_en = 0;
		VR360_stage = 0;

		if(Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN){
			// Enable MC DMA
			MEMC_Lib_MCDMA_Control(1, 0, 1);
			rtd_pr_memc_notice("[%s][Enable MC DMA][%x, %x, %x, %x, %x, %x]\n\r", __FUNCTION__, rtd_inl(0xb8099460), rtd_inl(0xb80994dc), rtd_inl(0xb809ae1c), rtd_inl(0xb809ae3c), rtd_inl(0xb809ae5c), rtd_inl(0xb809ae7c));
		}
#if 0 // set PZ
		rtd_clearbits(HARDWARE__HARDWARE_57_ADDR, _BIT17);//u1_mc2_var_lpf_wrt_en
		rtd_clearbits(MC2__MC2_50_ADDR, _BIT0);//WriteRegister(FRC_TOP__MC2__reg_mc_var_lpf_en_ADDR, 0, 1, 0x0);
		rtd_clearbits(MC2__MC2_50_ADDR, _BIT1);//WriteRegister(FRC_TOP__MC2__reg_mc_var_lpf_en_ADDR, 0, 1, 0x0);
		rtd_clearbits(MC2__MC2_98_ADDR, _BIT8);//WriteRegister(FRC_TOP__MC2__reg_mc_logo_vlpf_en_ADDR, 8, 8, 0x0);
		rtd_clearbits(KME_DEHALO__KME_DEHALO_10_ADDR, _BIT8);//WriteRegister(FRC_TOP__KME_DEHALO__reg_dh_en_ADDR, 0, 0, 0x0);
		rtd_setbits(MC__MC_28_ADDR, _BIT14); //0xb8099628

		//set PZ
		if(MEMC_LibGetMEMC_PCModeEnable()){
			Mid_Mode_SetMCBlend(RGB_IN_PC_OUT);
		}else{
			MEMC_LibSetMCBlend(2);
		}
		MEMC_LibSetMEMCFrameRepeatEnable(1, 0);

		//set memc DMA malloc and DMA on
		MEMC_LoadScriptDMA();
		MEMC_ChangeMCBufferAddr(status);
#else // set bld
		#if 0
		//set memc DMA malloc and DMA on
		//MEMC_LoadScriptDMA();
		MEMC_ChangeMCBufferAddr(status);

		//set Bld
		if(last_mode == MEMC_TYPE_OFF){
			//OFF memc
			MEMC_LibSetMEMCMode(MEMC_OFF);
			MEMC_LibSetMEMCFrameRepeatEnable(1, 1);
			rtd_pr_memc_notice("[MEMC_Set_malloc_address_01][AA][%d, %d][%d]\n", last_mode, last_dejudder, rtd_inl(0xb80996b0)&0x7);
		}
		else if(last_mode == MEMC_TYPE_LOW){
			//Set low memc
			MEMC_LibSetMEMCMode(MEMC_CLEAR);
			MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
			rtd_pr_memc_notice("[MEMC_Set_malloc_address_01][BB][%d, %d][%d]\n", last_mode, last_dejudder, rtd_inl(0xb80996b0)&0x7);
		}
		else if(last_mode == MEMC_TYPE_HIGH){
			//Set high memc
			MEMC_LibSetMEMCMode(MEMC_SMOOTH);
			MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
			rtd_pr_memc_notice("[MEMC_Set_malloc_address_01][CC][%d, %d][%d]\n", last_mode, last_dejudder, rtd_inl(0xb80996b0)&0x7);
		}
		else if(last_mode == MEMC_TYPE_NORMAL){
			//Set high memc
			MEMC_LibSetMEMCMode(MEMC_NORMAL);
			MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
			rtd_pr_memc_notice("[MEMC_Set_malloc_address_01][DD][%d, %d][%d]\n", last_mode, last_dejudder, rtd_inl(0xb80996b0)&0x7);
		}
		else if(last_mode == MEMC_TYPE_USER){
			//Set user memc
			MEMC_LibSetMEMCMode(MEMC_USER);
			MEMC_LibSetMEMCDejudderLevel(last_dejudder);
			MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
			rtd_pr_memc_notice("[MEMC_Set_malloc_address_01][EE][%d, %d][%d]\n", last_mode, last_dejudder, rtd_inl(0xb80996b0)&0x7);
		}
		else if(last_mode == MEMC_TYPE_55_PULLDOWN){
			//On 5:5 Pull-down for 24Hz Film source
			MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
		}
//		rtd_setbits(HARDWARE__HARDWARE_57_ADDR, _BIT17);//u1_mc2_var_lpf_wrt_en
//		rtd_setbits(MC2__MC2_98_ADDR, _BIT8);//WriteRegister(FRC_TOP__MC2__reg_mc_logo_vlpf_en_ADDR, 8, 8, 0x0);
//		rtd_setbits(KME_DEHALO__KME_DEHALO_10_ADDR, _BIT8);//WriteRegister(FRC_TOP__KME_DEHALO__reg_dh_en_ADDR, 0, 0, 0x0);
//		rtd_clearbits(MC__MC_28_ADDR, _BIT14); //0xb8099628
		#endif
#endif
	}else{
		rtd_pr_memc_notice("[MEMC_Set_malloc_address_02][%d]\n", (rtd_inl(0xb800011c)>>29)&0x7);
		VR360_en = 1;
		VR360_stage = 1;
		VR360_wait_flag = 1;
		last_mode = u8_MEMCMode;

#if RTK_MEMC_Performance_tunging_from_tv001
		//last_dejudder = dejudder;
#else
		last_dejudder = dejudder;
#endif
		
		me_clock_state_pre = (rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>30)&0x1;
		mc_clock_state_pre = (rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>31)&0x1;


//		MEMC_LibSetMEMCMode(MEMC_OFF);

//		Mid_Mode_SetMEMCFrameRepeatEnable(1);

//		MEMC_ChangeMCBufferAddr(status);

		if(Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN){
			// Disable MC DMA
			MEMC_Lib_MCDMA_Control(0, 0, 1);
			rtd_pr_memc_notice("[%s][Disable MC DMA][%x, %x, %x, %x, %x, %x]\n\r", __FUNCTION__, rtd_inl(0xb8099460), rtd_inl(0xb80994dc), rtd_inl(0xb809ae1c), rtd_inl(0xb809ae3c), rtd_inl(0xb809ae5c), rtd_inl(0xb809ae7c));
		}
	}
	rtd_pr_memc_debug( "[MEMC_Set_malloc_address_03][%d]\n", rtd_inl(0xB801B6B8)-time1);
	Mid_MISC_SetInputIntp_en(TRUE);
	Mid_MISC_SetOnefiftIntp_en(TRUE);
	Mid_MISC_SetOutputIntp_en(TRUE);
	rtd_pr_memc_notice("[%s][BBB]d5fc=[%x]\n", __FUNCTION__, rtd_inl(0xb809d5fc));
	rtd_pr_memc_notice("[%s][BBB][%x, %x]\n", __FUNCTION__, rtd_inl(0xb8099018), rtd_inl(0xb809d008));
#endif
#endif
#endif
}

int MEMC_Get_DMA_malloc_address(unsigned char status, unsigned int *DMA_release_start, unsigned int *DMA_release_size)
{

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else

	//UINT32 time_out = 30;
	unsigned int time3 = 0;
	unsigned char timeout_flag = 0;

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return 0;
//#endif

	rtd_pr_memc_notice("[MEMC_Get_DMA_malloc_address_start]\n");
	rtd_pr_memc_notice("[%s][AAA]d5fc=[%x]\n", __FUNCTION__, rtd_inl(0xb809d5fc));
	rtd_pr_memc_notice("[%s][AAA][%x, %x]\n", __FUNCTION__, rtd_inl(0xb8099018), rtd_inl(0xb809d008));

	//enable VR360 function
	if(status == TRUE){
		unsigned char check_BSize_status = 0;
		check_BSize_status = (rtd_inl(KPHASE_kphase_10_reg)>>12)&0x7;

		if(check_BSize_status == 4){
			time3 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			do{
				if(VR360_wait_flag == 1){
					if((rtd_inl(TIMER_SCPU_CLK90K_LO_reg)-time3)>=7200){
						timeout_flag = 1;
					}
				}
			}while((timeout_flag == 0) && (VR360_wait_flag == 1));

			*DMA_release_start = 0x46c98f60; // (rtd_inl(MEMC_MC_DMA_MC_HF_WDMA_MEND3_reg));
			*DMA_release_size = 0x132ea00; // 20MB
			rtd_pr_memc_notice("[MEMC_Get_DMA_malloc_address_01]\n");
			rtd_pr_memc_notice("[start, size][%x, %x]\n", *DMA_release_start, *DMA_release_size);
		}
		else{
			*DMA_release_start = 0;
			*DMA_release_size = 0;
			rtd_pr_memc_notice("[FRC is On !!MEMC will not release memory !!]\n");
		}
	}else{
		//NOTE : if MEMC status = off, return size  = 0, and print some hint for debug.
		*DMA_release_start = 0;
		*DMA_release_size = 0; // 20MB
	}

	rtd_pr_memc_notice("[%s][BBB]d5fc=[%x]\n", __FUNCTION__, rtd_inl(0xb809d5fc));
	rtd_pr_memc_notice("[%s][BBB][%x, %x]\n", __FUNCTION__, rtd_inl(0xb8099018), rtd_inl(0xb809d008));
	rtd_pr_memc_notice("[MEMC_Get_DMA_malloc_address_end]\n");

	return 0;
#endif	
}


#if 1//RTK_MEMC_Performance_tunging_from_tv001
VPQ_MEMC_TYPE_T UniformMEMCType(VPQ_MEMC_TYPE_T motion)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else

	// because the protocol defined by tv006 is different from others, it is necessary to switch the motion
	if( vpqex_project_id == 0x00060000){
		//re-mapping
		if(motion == 0/*V4L2_EXT_MEMC_TYPE_OFF*/){
			motion = VPQ_MEMC_TYPE_OFF;
		}
		else if(motion == 1/*V4L2_EXT_MEMC_TYPE_LOW*/){
			motion = VPQ_MEMC_TYPE_LOW;
		}
		else if(motion == 2/*V4L2_EXT_MEMC_TYPE_HIGH*/){
			motion = VPQ_MEMC_TYPE_HIGH;
		}
		else if(motion == 3/*V4L2_EXT_MEMC_TYPE_USER*/){
			motion = VPQ_MEMC_TYPE_USER;
		}
		else if(motion == 4/*V4L2_EXT_MEMC_TYPE_55_PULLDOWN*/){
			motion = VPQ_MEMC_TYPE_55_PULLDOWN;// 5
		}
		else if(motion == 5/*V4L2_EXT_MEMC_TYPE_MEDIUM*/){
			motion = VPQ_MEMC_TYPE_MEDIUM;// 6
		}
		else{
			motion = VPQ_MEMC_TYPE_OFF;
		}
	}
	else{
		//re-mapping
		if(motion == 0/*KADP_MEMC_TYPE_OFF*/){
			motion = VPQ_MEMC_TYPE_OFF;
		}
		else if(motion == 1/*KADP_MEMC_TYPE_HIGH*/){
			motion = VPQ_MEMC_TYPE_HIGH;
		}
		else if(motion == 2/*KADP_MEMC_TYPE_LOW*/){
			motion =  VPQ_MEMC_TYPE_NORMAL;//for osd level mapping 
		}
		else if(motion == 3/*KADP_MEMC_TYPE_NORMAL*/){
			motion = VPQ_MEMC_TYPE_LOW;//for osd level normal->MEMC clear
		}
		else if(motion == 4/*KADP_MEMC_TYPE_USER*/){
			motion = VPQ_MEMC_TYPE_USER;
		}
		else if(motion == 5/*KADP_MEMC_TYPE_55_PULLDOWN*/){
			motion = VPQ_MEMC_TYPE_55_PULLDOWN;
		}
		else if(motion == 6/*KADP_MEMC_TYPE_MEDIUM*/){
			motion = VPQ_MEMC_TYPE_MEDIUM;
		}
		else{
			motion = VPQ_MEMC_TYPE_OFF;
		}
	}

	return motion;
#endif
}

#endif


long HAL_VPQ_MEMC_SetMotionComp(unsigned char blurLevel, unsigned char judderLevel, VPQ_MEMC_TYPE_T motion){
	

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else

	VPQ_MEMC_SETMOTIONCOMP_T *set_motion_comp_info;	
#if Pro_tv010
	unsigned int u8_tmp;
#endif

#if 1//RTK_MEMC_Performance_tunging_from_tv001
	//unsigned int ulCount = 0;
#else
	unsigned int ulCount = 0;	
#endif
	int ret = 0;
	unsigned int semaphoreCnt = 0;
	
	//return 0;//k23
//#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC	
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
	#endif
//#endif
	
#if Pro_tv0302875P
	SLR_VOINFO* pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
#endif

	//Elsie 20151120, add semaphore
	while((IoReg_Read32(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg)&_BIT30) && semaphoreCnt < 100000) {
		semaphoreCnt++;
	}
	if(semaphoreCnt >= 100000) {
		rtd_pr_memc_debug( "[MEMC]HAL_VPQ_MEMC_SetMotionComp timed out!\n");
		return -1;
	}
	IoReg_SetBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg, _BIT30);

	if(VR360_en == 1){
		rtd_pr_memc_notice("[HAL_VPQ_MEMC_SetMotionComp][VR360 is  on !!Don't change MEMC motion type !!][%d,%d,%d]\n", blurLevel, judderLevel, motion);
		return 0;
	}

#if 1//RTK_MEMC_Performance_tunging_from_tv001
	if(motion >= VPQ_MEMC_TYPE_MAX)
	{
		//RTD_LOG_DEBUG("[MEMC]VPQ_MEMC_SetMotionComp, re-trigger org(%d,%d,%d)\n",blurLevel,judderLevel,motion);
		blurLevel = 0;
		judderLevel = 0;
		motion = 0;
	}
#endif

#if Pro_tv010
	if(motion == 2){ //new sport mode
		g_new_sport_mode = 1;
	}else {
		g_new_sport_mode = 0;
	}
	if(g_new_sport_mode){
		motion = 4; //user mode
		ReadRegister(SOFTWARE2_SOFTWARE2_44_reg, 0,  7, &u8_tmp);
		blurLevel = (u8_tmp<128) ? u8_tmp : 128;
		ReadRegister(SOFTWARE2_SOFTWARE2_44_reg, 8, 15, &u8_tmp);
		judderLevel = (u8_tmp<128) ? u8_tmp : 128;
	}
#else
	g_new_sport_mode = 0;
#endif

	rtd_pr_memc_emerg( "[MEMC]HAL_VPQ_MEMC_SetMotionComp\n");
	rtd_pr_memc_emerg( "blurLevel = %d\n judderLevel = %d\n motion = %d\n",blurLevel, judderLevel, motion);
	//	rtd_pr_memc_emerg( "HAL_VPQ_MEMC_SetMotionComp  blurLevel = %d  judderLevel = %d	motion = %d   new_sport_mode =%d	vpqex_project_id=%d\n",blurLevel, judderLevel, motion, g_new_sport_mode, vpqex_project_id);

	set_motion_comp_info = (VPQ_MEMC_SETMOTIONCOMP_T *)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_SETMOTIONCOMP);

#if 1//RTK_MEMC_Performance_tunging_from_tv001
	//ulCount = sizeof(VPQ_MEMC_SETMOTIONCOMP_T) / sizeof(unsigned int);
#else
	ulCount = sizeof(VPQ_MEMC_SETMOTIONCOMP_T) / sizeof(unsigned int);
#endif
	

	set_motion_comp_info->blurLevel = blurLevel;
	set_motion_comp_info->judderLevel = judderLevel;
#if 1//RTK_MEMC_Performance_tunging_from_tv001
	motion = UniformMEMCType(motion);
#else
	//re-mapping
	if(motion == 0/*V4L2_EXT_MEMC_TYPE_OFF*/){
		motion = VPQ_MEMC_TYPE_OFF;
	}
	else if(motion == 1/*V4L2_EXT_MEMC_TYPE_LOW*/){
		motion = VPQ_MEMC_TYPE_LOW;
	}
	else if(motion == 2/*V4L2_EXT_MEMC_TYPE_HIGH*/){
		motion = VPQ_MEMC_TYPE_HIGH;
	}
	else if(motion == 3/*V4L2_EXT_MEMC_TYPE_USER*/){
		motion = VPQ_MEMC_TYPE_USER;
	}
	else if(motion == 4/*V4L2_EXT_MEMC_TYPE_55_PULLDOWN*/){
		motion = VPQ_MEMC_TYPE_55_PULLDOWN;
	}
	else if(motion == 5/*V4L2_EXT_MEMC_TYPE_MEDIUM*/){
		motion = VPQ_MEMC_TYPE_MEDIUM;
	}
	else{
		motion = VPQ_MEMC_TYPE_OFF;
	}
#endif


	rtd_pr_memc_notice("[scaler_vpqmemcdev]blurLevel = %d, judderLevel = %d, motion = %d\n",blurLevel, judderLevel, motion);
	set_motion_comp_info->motion = motion;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	//change endian
	set_motion_comp_info->blurLevel = htonl(set_motion_comp_info->blurLevel);
	set_motion_comp_info->judderLevel = htonl(set_motion_comp_info->judderLevel);
	set_motion_comp_info->motion = htonl(set_motion_comp_info->motion);
#endif

	down(&Memc_Realcinema_Semaphore);
	//save current MEMC motion type+	Scaler_MEMC_SetMotionType(motion);
	Scaler_MEMC_SetMotionType(motion);
	//set frame rate

	DbgSclrFlgTkr.memc_realcinema_run_flag = TRUE;//Run memc mode
	//memc_realcinema_framerate();

	up(&Memc_Realcinema_Semaphore);
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETMOTIONCOMP,0,0))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETMOTIONCOMP RPC fail !!!\n", ret);
	}
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

#ifndef CONFIG_MEMC_TASK_QUEUE
	if(!Scaler_MEMC_GetMEMC_Enable())
		return 0;
	#ifndef CONFIG_MEMC_DEFAULT_REPEAT_MODE
		Scaler_MEMC_SetMotionComp(set_motion_comp_info->blurLevel, set_motion_comp_info->judderLevel, set_motion_comp_info->motion);
	#else
		Scaler_MEMC_SetMotionComp(0,0,0);
	#endif
#else

#if Pro_tv0302875P
		if (pVOInfo->isJPEG) {//picture force set MEMC off for reducing bandwidth
			task.data.memc_set_motion_comp.motion = 0;
			WriteRegister(SOFTWARE3_SOFTWARE3_45_reg, 16, 17, 1);//0xB809D9B4
		}
		else{
			WriteRegister(SOFTWARE3_SOFTWARE3_45_reg, 16, 17, 2);//0xB809D9B4
		}
#endif


#if 1//RTK_MEMC_Performance_tunging_from_tv001
		task.type = SCALERIOC_MEMC_SETMOTIONCOMP;
		memcpy(&task.data,set_motion_comp_info,sizeof(task.data));
		#ifdef CONFIG_MEMC_DEFAULT_REPEAT_MODE
		task.data.memc_set_motion_comp.motion = 0;
		#endif
		Scaler_MEMC_PushTask(&task);
#else
		if((vbe_disp_get_stop_memc_push_flag() == 1) && ((rtd_inl(0xB800011c)>>30)==0)){
				rtd_pr_memc_notice("[%s][%d]VRR Status Modify!\n",__FUNCTION__, __LINE__);
				Scaler_MEMC_SetMotionComp(set_motion_comp_info->blurLevel, set_motion_comp_info->judderLevel, set_motion_comp_info->motion);
		}else{
			task.type = SCALERIOC_MEMC_SETMOTIONCOMP;
			memcpy(&task.data,set_motion_comp_info,sizeof(task.data));
			#ifdef CONFIG_MEMC_DEFAULT_REPEAT_MODE
			task.data.memc_set_motion_comp.motion = 0;
			#endif
			Scaler_MEMC_PushTask(&task);
		}
#endif


		
//		if(task.data.memc_set_motion_comp.motion == 0){
//			MEMC_Set_malloc_address(0);
//			Scaler_MEMC_LowDelayMode(1);
//		}
//		else{
//			MEMC_Set_malloc_address(1);
//			Scaler_MEMC_LowDelayMode(0);
//		}
#endif
#endif
#endif

	IoReg_ClearBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);

	return ret;
#endif
}

long HAL_VPQ_MEMC_SetMotionPro(BOOLEAN bMotionPro){
	int ret = 0;

	rtd_pr_memc_debug( "[MEMC]HAL_VPQ_MEMC_SetMotionPro\n");

	return ret;
}

void HAL_VPQ_MEMC_SetBlurLevel(unsigned char blurLevel){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	//int ret;

#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
#endif

	MEMC_DBG_PRINT("[MEMC][%s]blurLevel = %d\n",__FUNCTION__,blurLevel);

	if(blurLevel > (1 << 7))
		blurLevel = (1 << 7);
	else if(blurLevel < 0)
		blurLevel = 0;

	//MEMC_DBG_PRINT("[VPQ_MEMC_SetBlurLevel]deblur_lvl = %d \n",blurLevel);
#ifdef CONFIG_MEMC_TASK_QUEUE
	task.type = SCALERIOC_MEMC_SETBLURLEVEL;
	task.data.value = blurLevel;
	Scaler_MEMC_PushTask(&task);
#endif


#if 0
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETBLURLEVEL, blurLevel, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETBLURLEVEL RPC fail !!!\n", ret);
	}
#endif
#endif
#endif

}

void HAL_VPQ_MEMC_SetJudderLevel(unsigned char judderLevel){

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	//int ret;

#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
#endif

	MEMC_DBG_PRINT("[MEMC][%s]judderLevel = %d\n",__FUNCTION__,judderLevel);

	if(judderLevel > (1 << 7))
		judderLevel = (1 << 7);
	else if(judderLevel < 0)
		judderLevel = 0;

	//MEMC_DBG_PRINT("[VPQ_MEMC_SetBlurLevel]deblur_lvl = %d \n",blurLevel);
#ifdef CONFIG_MEMC_TASK_QUEUE
	task.type = SCALERIOC_MEMC_SETJUDDERLEVEL;
	task.data.value = judderLevel;
	Scaler_MEMC_PushTask(&task);
#endif

#if 0
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETJUDDERLEVEL, judderLevel, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETJUDDERLEVEL RPC fail !!!\n", ret);
	}
#endif
#endif
#endif

}
#endif

#if 1//RTK_MEMC_Performance_tunging_from_tv001
bool get_reset_MEMC_flg(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else

	return g_RESET_MEMC_FLG;
#endif
}

void set_reset_MEMC_flg(bool enable)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else

	g_RESET_MEMC_FLG = enable;
#endif
}

#ifndef BUILD_QUICK_SHOW
#if RTK_MEMC_Performance_tunging_from_tv001//xj//m7 ui下設定是透過gVIP_Table 兩邊DejudderLevel,DeblurLevel有些微差異
extern SLR_VIP_TABLE *fwif_colo_get_AP_vip_table_gVIP_Table(void);

//blurUILevel = 0~10
void VPQ_MEMC_SetBlurUILevel(unsigned char blurUILevel)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return ;
#else

	unsigned char blurValue = 0;
	unsigned char MaxLevel = 0;
	SLR_VIP_TABLE *gVip_Table = fwif_colo_get_AP_vip_table_gVIP_Table();

	if (gVip_Table == NULL) {
		MEMC_DBG_PRINT("~get vipTable Error return, %s->%d, %s~\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	MaxLevel = gVip_Table->MEMC_PARAMS.BlurTable[0];
	if(MaxLevel==0){
		rtd_pr_memc_notice("[%s] MaxLevel==0\n", MaxLevel);
		blurValue = 0;
	}else{
		blurUILevel = blurUILevel>MaxLevel-1 ? MaxLevel-1:(blurUILevel<0 ? 0:blurUILevel);//Out-of-bounds protect
		blurValue = gVip_Table->MEMC_PARAMS.BlurTable[blurUILevel+1];
		rtd_pr_memc_emerg("[VPQ_MEMC_SetBlurUILevel] blur_UI = %d, blurValue = %d\n", blurUILevel, blurValue);
	}
	
	#if 0
	VPQ_MEMC_SetBlurLevel(blurLevel);
	#else
	HAL_VPQ_MEMC_SetBlurLevel(blurValue);
	#endif
#endif	
}

//judderUILevel = 0~10
void VPQ_MEMC_SetJudderUILevel(unsigned char judderUILevel)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		return ;
#else

	unsigned char judderValue = 0;
	unsigned char MaxLevel = 0;
	SLR_VIP_TABLE *gVip_Table = fwif_colo_get_AP_vip_table_gVIP_Table();

	if (gVip_Table == NULL) {
		MEMC_DBG_PRINT("~get vipTable Error return, %s->%d, %s~\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	MaxLevel = gVip_Table->MEMC_PARAMS.JudderTable[0];
	if(MaxLevel==0){
		rtd_pr_memc_notice("[%s] MaxLevel==0\n", MaxLevel);
		judderValue = 0;
	}else{
		judderUILevel = judderUILevel>MaxLevel-1 ? MaxLevel-1:(judderUILevel<0 ? 0:judderUILevel);//Out-of-bounds protect
		judderValue = gVip_Table->MEMC_PARAMS.JudderTable[judderUILevel+1];
		rtd_pr_memc_emerg("[VPQ_MEMC_SetJudderUILevel] judder_UI = %d, judderValue = %d\n", judderUILevel, judderValue);
	}

	#if 0
	VPQ_MEMC_SetJudderLevel(judderLevel);
	#else
	HAL_VPQ_MEMC_SetJudderLevel(judderValue);
	#endif
#endif	
}

//blurMotionLevel = 0~3 Off/LOW/MID/HIGH
void VPQ_MEMC_SetBlurMotionLevel(unsigned char blurMotionLevel)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return ;
#else

	unsigned char blurValue = 0;
	SLR_VIP_TABLE *gVip_Table = fwif_colo_get_AP_vip_table_gVIP_Table();

	if (gVip_Table == NULL) {
		MEMC_DBG_PRINT("~get vipTable Error return, %s->%d, %s~\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	blurMotionLevel = blurMotionLevel>=VPQ_MEMC_LEVEL_MAX ? VPQ_MEMC_LEVEL_MAX-1:(blurMotionLevel<VPQ_MEMC_LEVEL_OFF ? VPQ_MEMC_LEVEL_OFF:blurMotionLevel);//Out-of-bounds protect
	blurValue = gVip_Table->MEMC_PARAMS.BlurMotionTable[blurMotionLevel];
	rtd_pr_memc_emerg("[VPQ_MEMC_SetBlurMotionLevel] blur_Motion = %d, blurValue = %d\n", blurMotionLevel, blurValue);
	
	HAL_VPQ_MEMC_SetBlurLevel(blurValue);
#endif
}

//judderMotionLevel = 0~3 Off/LOW/MID/HIGH
void VPQ_MEMC_SetJudderMotionLevel(unsigned char judderMotionLevel)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		return ;
#else

	unsigned char juddervalue = 0;
	SLR_VIP_TABLE *gVip_Table = fwif_colo_get_AP_vip_table_gVIP_Table();

	if (gVip_Table == NULL) {
		MEMC_DBG_PRINT("~get vipTable Error return, %s->%d, %s~\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	judderMotionLevel = judderMotionLevel>=VPQ_MEMC_LEVEL_MAX ? VPQ_MEMC_LEVEL_MAX-1:(judderMotionLevel<VPQ_MEMC_LEVEL_OFF ? VPQ_MEMC_LEVEL_OFF:judderMotionLevel);//Out-of-bounds protect
	juddervalue = gVip_Table->MEMC_PARAMS.JudderMotionTable[judderMotionLevel];
	rtd_pr_memc_emerg("[VPQ_MEMC_SetJudderMotionLevel] judder_Motion = %d, judderValue = %d\n", judderMotionLevel, juddervalue);
	
	HAL_VPQ_MEMC_SetJudderLevel(juddervalue);
#endif	
}

#endif



long HAL_VPQ_MEMC_MotionCompOnOff(BOOLEAN bOnOff){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else

	int ret = 0;

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	unsigned int semaphoreCnt = 0;
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
	#endif

	rtd_pr_memc_debug( "[MEMC]HAL_VPQ_MEMC_SetMotionComp, bOnOff = %d\n", bOnOff);

	while((IoReg_Read32(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg)&_BIT30) && semaphoreCnt < 100000) {
		semaphoreCnt++;
	}
	if(semaphoreCnt >= 100000) {
		rtd_pr_memc_debug( "[MEMC]HAL_VPQ_MEMC_SetMotionComp timed out!\n");
		return -1;
	}
	IoReg_SetBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_MOTIONCOMPONOFF, bOnOff, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_MOTIONCOMPONOFF RPC fail !!!\n", ret);
	}
#else
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return 0;
	Scaler_MEMC_MotionCompOnOff(bOnOff);
#else

		task.type = SCALERIOC_MEMC_MOTIONCOMPONOFF;
		task.data.value = bOnOff;
		Scaler_MEMC_PushTask(&task);
	#endif

#endif
	IoReg_ClearBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
#endif	
	return ret;
#endif	
}

#endif
unsigned char MEMC_LOWDELAY_ENABLE = 0;
void Scaler_SetMEMCLowDelay(unsigned char enable)
{
	MEMC_LOWDELAY_ENABLE = enable;
}

unsigned char Scaler_GetMEMCLowDelay(void)
{
	return MEMC_LOWDELAY_ENABLE;
}
#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_MEDIA_SUPPORT
extern unsigned char vsc_get_main_win_apply_done_event_subscribe(void);
#endif
long HAL_VPQ_MEMC_LowDelayMode(unsigned char type){
	int ret = 0;

#if 1//#ifdef KID23_TO_WEBOS	
//#ifndef CONFIG_MEMC_BYPASS

	
#if RTK_MEMC_Performance_tunging_from_tv001
	//sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	//ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
#else
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;

#endif

//	SLR_VOINFO* pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());
//#endif
	rtd_pr_memc_notice("[%s][%d][LowDelayMode]", __FUNCTION__, __LINE__);
	if(!(((drv_memory_get_low_delay_game_mode_dynamic() == _ENABLE) && (get_new_game_mode_condition() || (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC))) || drv_memory_get_vdec_direct_low_latency_mode()))
	{
		rtd_pr_memc_emerg("\n func:%s, line:%d  \n",__FUNCTION__,__LINE__);
		vpq_memc_set_lowDelayMode(type);
	}
	else{
		rtd_pr_memc_notice("Game mode keep MEMC low delay settings!");
	}


#if RTK_MEMC_Performance_tunging_from_tv001
#ifdef ENABLE_IMD_PST_SMOOTHTOGGLE
		if(type == 1){
			if((scaler_vsc_get_adaptive_pst_lowdelay_mode()==0)&&((vsc_get_adaptivestream_flag(SLR_MAIN_DISPLAY))||(scaler_vsc_get_force_pst_lowdelay_mode()==1)))
				scaler_vsc_set_adaptive_pst_lowdelay_mode(1);
		}else{
	//#ifndef CONFIG_MEMC_BYPASS
		//if(get_MEMC_bypass_status_refer_platform_model() == FALSE || get_platform() == PLATFORM_K8HP){
		if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
			ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
			memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
			if(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY){
				if(memc_mux_ctrl_reg.memc_outmux_sel)
				{
					if (scaler_vsc_get_adaptive_pst_lowdelay_mode() == 1)
						scaler_vsc_set_keep_vo_framerate_control(1);
					scaler_vsc_set_adaptive_pst_lowdelay_mode(0);
				}
			}
		}else{
	//#else
			if (scaler_vsc_get_adaptive_pst_lowdelay_mode() == 1)
				scaler_vsc_set_keep_vo_framerate_control(1);
			scaler_vsc_set_adaptive_pst_lowdelay_mode(0);
			}
	//#endif
	
		}
#endif

#else
#if 1//#ifdef ENABLE_IMD_PST_SMOOTHTOGGLE
	//#ifndef CONFIG_MEMC_BYPASS
		//if(get_MEMC_bypass_status_refer_platform_model() == FALSE || get_platform() == PLATFORM_K8HP){
		if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
			memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
			if(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY){
				if(memc_mux_ctrl_reg.memc_outmux_sel)
				{
					//K6LP : CP & pst_low_delay enable always MEMC on for film video
					if (scaler_vsc_get_adaptive_pst_lowdelay_mode() == 1 && !drv_memory_get_vdec_direct_low_latency_mode()){
						//when disabling MEMC low delay in MC only mode, need to set MEMC on to do judder free.
						sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
						if((sys_reg_sys_clken3_reg.clken_memc_me == 0)/* && (pVOInfo->src_v_freq_1000 <= 47000)*/
							&& !(drv_memory_get_vdec_direct_low_latency_mode())){
							rtd_pr_memc_notice("MEMC disable LowDelay, set MEMC enable!\n");
							//enable MEMC clock
							MEMC_Lib_set_memc_bypass_to_enable_flag(1);//add by jerry wang;only for memc use
							Scaler_MEMC_MEMC_CLK_OnOff(1, 1, 0);
						}
					}
				}
			}
		}
	//#endif
#endif
#endif
#endif




#if 0
#ifdef ENABLE_IMD_PST_SMOOTHTOGGLE
	if(type == 1){
		if((Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY) &&	//main source
			(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE) &&	//main not in active state
			(scaler_vsc_get_adaptive_pst_lowdelay_mode()==0)&&
			((get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY))||(scaler_vsc_get_force_pst_lowdelay_mode()==1)) &&
			(vsc_get_main_win_apply_done_event_subscribe()))//enter pst low delay only when webos called callback function
			scaler_vsc_set_adaptive_pst_lowdelay_mode(1);
	}else{
//#ifndef CONFIG_MEMC_BYPASS
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
		memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
		if(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY){
			if(memc_mux_ctrl_reg.memc_outmux_sel)
			{
				if (scaler_vsc_get_adaptive_pst_lowdelay_mode() == 1)
					scaler_vsc_set_keep_vo_framerate_control(1);
				scaler_vsc_set_adaptive_pst_lowdelay_mode(0);
			}
		}
		}else{
//#else
		if(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY){
			if (scaler_vsc_get_adaptive_pst_lowdelay_mode() == 1)
				scaler_vsc_set_keep_vo_framerate_control(1);
			scaler_vsc_set_adaptive_pst_lowdelay_mode(0);
		}
        }
//#endif
	}
#endif
#endif
	return ret;
}

long HAL_VPQ_MEMC_SetRGBYUVMode(unsigned char mode){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		return 0;
#else

	int ret = 0;
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
	#endif
#endif

	//if(mode == 0)
	//	mode = VPQ_MEMC_RGB_IN_VIDEO_OUT;
	//else
	//	mode = VPQ_MEMC_YUV_IN_VIDEO_OUT;
	unsigned int semaphoreCnt = 0;

	while((IoReg_Read32(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg)&_BIT30) && semaphoreCnt < 100000) {
		semaphoreCnt++;
	}
	if(semaphoreCnt >= 100000) {
		rtd_pr_memc_debug( "[MEMC]HAL_VPQ_MEMC_SetRGBYUVMode timed out!\n");
		return -1;
	}
	IoReg_SetBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETRGBYUVMode, mode, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETRGBYUVMode RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return 0;
		Scaler_MEMC_SetRGBYUVMode(mode);
	#else


#if RTK_MEMC_Performance_tunging_from_tv001
		task.type = SCALERIOC_MEMC_SETRGBYUVMode;
		task.data.value = mode;
		Scaler_MEMC_PushTask(&task);
#else
		if((vbe_disp_get_stop_memc_push_flag() == 1) && ((rtd_inl(0xB800011c)>>30)==0)){
				Scaler_MEMC_SetRGBYUVMode(mode);
				rtd_pr_memc_notice("[%s][%d]VRR Status Modify!\n",__FUNCTION__, __LINE__);
		}else{
			task.type = SCALERIOC_MEMC_SETRGBYUVMode;
			task.data.value = mode;
			Scaler_MEMC_PushTask(&task);
		}
#endif
#endif
#endif
#endif
	IoReg_ClearBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
	return ret;
#endif	
}

unsigned int HAL_VPQ_MEMC_GetFrameDelay(void){
	unsigned int FrameDelay = 0;
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS			
	FrameDelay = 0;
#else
	FrameDelay= (unsigned int)Scaler_MEMC_GetAVSyncDelay();
#endif
	rtd_pr_memc_notice("[%s] FrameDelay:%u\n", __FUNCTION__, FrameDelay);
return FrameDelay;

//	*pFrameDelay = FrameDelay;

	/*
	int ret;

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_GETFRAMEDELAY, 1, 1))){
		rtd_pr_memc_debug("[MEMC]ret=%d, SCALERIOC_MEMC_GETFRAMEDELAY RPC fail !!!\n", ret);
	}
	*/
}

void HAL_VPQ_MEMC_SetVideoBlock(VPQ_MEMC_MUTE_TYPE_T type, BOOLEAN bOnOff) {
	set_display_forcebg_mask(SLR_MAIN_DISPLAY, SLR_FORCE_BG_TYPE_3D, bOnOff);
	mute_control(SLR_MAIN_DISPLAY, bOnOff);
}


void HAL_VPQ_MEMC_SetTrueMotionDemo(BOOLEAN bOnOff){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC


#if 1

#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
#endif

	//MEMC_DBG_PRINT("[VPQ_MEMC_SetBlurLevel]deblur_lvl = %d \n",blurLevel);
#ifdef CONFIG_MEMC_TASK_QUEUE
	task.type = SCALERIOC_MEMC_SETTRUEMOTIONDEMO;
	task.data.value = bOnOff;
	Scaler_MEMC_PushTask(&task);
#else
	Scaler_MEMC_SetTrueMotionDemo(task.data.value);
#endif

#else

	int ret;

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETTRUEMOTIONDEMO, bOnOff, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETTRUEMOTIONDEMO RPC fail !!!\n", ret);
	}
#endif
#endif	
#endif	
}

unsigned int HAL_VPQ_MEMC_GetFirmwareVersion(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else

	unsigned int u32_Version = DATE_TIME + (DATE_DATE<<3)+ (DATE_MONTH<<8) + (DATE_YEAR<<12) + (MEMC_VER_MINOR<<19) + (MEMC_VER_MAJOR<<26);
	//rtd_pr_memc_emerg("Ver:%d.%d Date:20%d.%d.%d.%d!!\n", ((u32_Version>>26)&0x3f), ((u32_Version>>19)&0x7f), ((u32_Version>>12)&0x7f), ((u32_Version>>8)&0xf), ((u32_Version>>3)&0x1f), (u32_Version&0x7));
	return u32_Version;
#endif
}

void HAL_VPQ_MEMC_SetBypassRegion(BOOLEAN bOnOff, VPQ_MEMC_BYPASS_REGION_T region, UINT32 x, UINT32 y, UINT32 w, UINT32 h){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else


	VPQ_MEMC_SETBYPASSREGION_T*set_bypass_region_info;


#if RTK_MEMC_Performance_tunging_from_tv001
	//unsigned int ulCount = 0;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	int ret=0;
#endif
#else
	unsigned int ulCount = 0;
	int ret;
#endif

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

       #ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
       #endif
#endif	   
	MEMC_DBG_PRINT("region = %d\n",region);
	MEMC_DBG_PRINT("rect.x = %d, rect.y = %d, rect.w = %d, rect.h = %d\n",x, y, w, h);

	MEMC_DBG_PRINT("bOnOff = %d\n",bOnOff);

#if RTK_MEMC_Performance_tunging_from_tv001
	
#else
	ret=0;
#endif
	
	set_bypass_region_info = (VPQ_MEMC_SETBYPASSREGION_T *)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_SETBYPASSREGION);

#if RTK_MEMC_Performance_tunging_from_tv001
	//ulCount = sizeof(VPQ_MEMC_SETBYPASSREGION_T) / sizeof(unsigned int);	
#else
	ulCount = sizeof(VPQ_MEMC_SETBYPASSREGION_T) / sizeof(unsigned int);
#endif

	set_bypass_region_info->bOnOff = bOnOff;
	set_bypass_region_info->region = region;
	set_bypass_region_info->x = x;
	set_bypass_region_info->y = y;
	set_bypass_region_info->w = w;
	set_bypass_region_info->h = h;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	//change endian
	set_bypass_region_info->bOnOff = htonl(set_bypass_region_info->bOnOff);
	set_bypass_region_info->region = htonl(set_bypass_region_info->region);
	set_bypass_region_info->x = htonl(set_bypass_region_info->x);
	set_bypass_region_info->y = htonl(set_bypass_region_info->y);
	set_bypass_region_info->w = htonl(set_bypass_region_info->w);
	set_bypass_region_info->h = htonl(set_bypass_region_info->h);


	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETBYPASSREGION,0,0))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETBYPASSREGION RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return ;
	Scaler_MEMC_SetBypassRegion(set_bypass_region_info->bOnOff, set_bypass_region_info->region, set_bypass_region_info->x, set_bypass_region_info->y, set_bypass_region_info->w, set_bypass_region_info->h);
#else

		task.type = SCALERIOC_MEMC_SETBYPASSREGION;
		memcpy(&task.data,set_bypass_region_info,sizeof(SCALER_MEMC_SETBYPASSREGION_T));
		Scaler_MEMC_PushTask(&task);
	#endif
#endif
#endif
#endif
}

void HAL_VPQ_MEMC_SetReverseControl(unsigned char u8Mode){
	int ret;

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETREVERSECONTROL, u8Mode, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETREVERSECONTROL RPC fail !!!\n", ret);
	}
}

void HAL_VPQ_MEMC_Freeze(unsigned char type){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else

#if 1
	MEMC_Lib_Freeze(type);
#else
	int ret;

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_FREEZE, type, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_FREEZE RPC fail !!!\n", ret);
	}
#endif
#endif
}

/* not use in WebOS
void HAL_VPQ_MEMC_SetDemoBar(BOOLEAN bOnOff, unsigned char r, unsigned char g, unsigned char b){
	VPQ_MEMC_SETDEMOBAR_T*set_demo_bar_info;
	unsigned int ulCount = 0;
	int ret;

	set_demo_bar_info = (VPQ_MEMC_SETDEMOBAR_T *)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_SETDEMOBAR);
	ulCount = sizeof(VPQ_MEMC_SETDEMOBAR_T) / sizeof(unsigned int);

	set_demo_bar_info->bOnOff = bOnOff;
	set_demo_bar_info->r = r;
	set_demo_bar_info->g = g;
	set_demo_bar_info->b = b;

	//change endian
	set_demo_bar_info->bOnOff = htonl(set_demo_bar_info->bOnOff);
	set_demo_bar_info->r = htonl(set_demo_bar_info->r);
	set_demo_bar_info->g = htonl(set_demo_bar_info->g);
	set_demo_bar_info->b = htonl(set_demo_bar_info->b);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETDEMOBAR,0,0))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETDEMOBAR RPC fail !!!\n", ret);
	}
}
void HAL_VPQ_MEMC_DEBUG(void){

}
*/
#endif

#endif
void Scaler_MEMC_SetInOutUseCase(VPQ_MEMC_INPUT_RESOLUTION input_re, VPQ_MEMC_OUTPUT_RESOLUTION output_re, VPQ_MEMC_INPUT_FORMAT input_for, VPQ_MEMC_OUTPUT_FORMAT output_for){

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

	SCALER_MEMC_SETINOUTUSECASE_T* set_inout_usecase_info;

#if RTK_MEMC_Performance_tunging_from_tv001
	//unsigned int ulCount = 0;

#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	int ret=0;
#endif

#else
	unsigned int ulCount = 0;
	int ret;
#endif

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC	
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
    #endif
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
		
#else
		ret=0;
#endif

	set_inout_usecase_info = (SCALER_MEMC_SETINOUTUSECASE_T*)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_SETINOUTUSECASE);

#if RTK_MEMC_Performance_tunging_from_tv001
	//ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#else
	ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#endif

	set_inout_usecase_info->input_re = input_re;
	set_inout_usecase_info->output_re = output_re;
	set_inout_usecase_info->input_for = input_for;
	set_inout_usecase_info->output_for = output_for;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	//change endian
	set_inout_usecase_info->input_re = htonl(set_inout_usecase_info->input_re);
	set_inout_usecase_info->output_re = htonl(set_inout_usecase_info->output_re);
	set_inout_usecase_info->input_for = htonl(set_inout_usecase_info->input_for);
	set_inout_usecase_info->output_for = htonl(set_inout_usecase_info->output_for);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETINOUTUSECASE,0,0))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETINOUTUSECASE RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return ;
	Scaler_MEMC_SetInOutputUseCase(set_inout_usecase_info->input_re, set_inout_usecase_info->output_re, set_inout_usecase_info->input_for, set_inout_usecase_info->output_for);
#else


#if RTK_MEMC_Performance_tunging_from_tv001
		task.type = SCALERIOC_MEMC_SETINOUTUSECASE;
		memcpy(&task.data,set_inout_usecase_info,sizeof(SCALER_MEMC_SETINOUTUSECASE_T));
		Scaler_MEMC_PushTask(&task);
#else
		if((vbe_disp_get_stop_memc_push_flag() == 1) && ((rtd_inl(0xB800011c)>>30)==0))
		{
			rtd_pr_memc_notice("[%s][%d]VRR don't aplly!\n",__FUNCTION__, __LINE__);
			return;
		}else{
			task.type = SCALERIOC_MEMC_SETINOUTUSECASE;
			memcpy(&task.data,set_inout_usecase_info,sizeof(SCALER_MEMC_SETINOUTUSECASE_T));
			Scaler_MEMC_PushTask(&task);
		}
#endif

	#endif
#endif
#endif
#endif
}

#ifndef BUILD_QUICK_SHOW
void Scaler_MEMC_SetInOutputFormat(VPQ_MEMC_INPUT_FORMAT input_for, VPQ_MEMC_OUTPUT_FORMAT output_for){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

	SCALER_MEMC_SETINOUTPUTFORMAT_T* set_inputput_format_info;

#if RTK_MEMC_Performance_tunging_from_tv001
	//unsigned int ulCount = 0;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	int ret=0;
#endif
#else
	unsigned int ulCount = 0;
	int ret;
#endif

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
    #endif
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
			
#else
	ret=0;
#endif

	set_inputput_format_info = (SCALER_MEMC_SETINOUTPUTFORMAT_T*)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_SETINPUTOUTPUTFORMAT);

#if RTK_MEMC_Performance_tunging_from_tv001
		//ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#else
		ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#endif


	set_inputput_format_info->input_for = input_for;
	set_inputput_format_info->output_for = output_for;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	//change endian
	set_inputput_format_info->input_for = htonl(set_inputput_format_info->input_for);
	set_inputput_format_info->output_for = htonl(set_inputput_format_info->output_for);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETINPUTOUTPUTFORMAT,0,0))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETINPUTOUTPUTFORMAT RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return ;
	Scaler_MEMC_SetInputOutputFormat(set_inputput_format_info->input_for, set_inputput_format_info->output_for);
#else

		task.type = SCALERIOC_MEMC_SETINPUTOUTPUTFORMAT;
		memcpy(&task.data,set_inputput_format_info,sizeof(SCALER_MEMC_SETINOUTPUTFORMAT_T));
		Scaler_MEMC_PushTask(&task);
	#endif
#endif
#endif
#endif
}

void Scaler_MEMC_SetInOutputResolution(VPQ_MEMC_INPUT_RESOLUTION input_re, VPQ_MEMC_OUTPUT_RESOLUTION output_re){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

	SCALER_MEMC_SETINOUTPUTRESOLUTION_T* set_inoutput_resolution_info;

#if RTK_MEMC_Performance_tunging_from_tv001
		//unsigned int ulCount = 0;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
		int ret=0;
#endif
#else
		unsigned int ulCount = 0;
		int ret;
#endif

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
       #endif
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
			
#else
	ret=0;
#endif

	set_inoutput_resolution_info = (SCALER_MEMC_SETINOUTPUTRESOLUTION_T*)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_SETINPUTOUTPUTRESOLUTION);

#if RTK_MEMC_Performance_tunging_from_tv001
		//ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#else
		ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#endif

	set_inoutput_resolution_info->input_re = input_re;
	set_inoutput_resolution_info->output_re = output_re;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	//change endian
	set_inoutput_resolution_info->input_re = htonl(set_inoutput_resolution_info->input_re);
	set_inoutput_resolution_info->output_re = htonl(set_inoutput_resolution_info->output_re);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETINPUTOUTPUTRESOLUTION,0,0))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETINPUTOUTPUTRESOLUTION RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return ;
	Scaler_MEMC_SetInputOutputResolution(set_inoutput_resolution_info->input_re, set_inoutput_resolution_info->output_re);
#else

		task.type = SCALERIOC_MEMC_SETINPUTOUTPUTRESOLUTION;
		memcpy(&task.data,set_inoutput_resolution_info,sizeof(SCALER_MEMC_SETINOUTPUTRESOLUTION_T));
		Scaler_MEMC_PushTask(&task);
	#endif
#endif
#endif
#endif
}
#endif


#if 1
void Scaler_MEMC_SetInOutputFrameRate(VPQ_MEMC_INPUT_FRAMERATE input_frame, VPQ_MEMC_OUTPUT_FRAMERATE output_frame){

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

	SCALER_MEMC_INOUTFRAMERATE_T* set_inoutput_framerate_info;

#if RTK_MEMC_Performance_tunging_from_tv001
		//unsigned int ulCount = 0;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
		int ret=0;
#endif
#else
		unsigned int ulCount = 0;
		int ret;
#endif
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
       #endif
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
			
#else
			ret=0;
#endif

	set_inoutput_framerate_info = (SCALER_MEMC_INOUTFRAMERATE_T*)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_SETINOUTPUTFRAMERATE);

#if RTK_MEMC_Performance_tunging_from_tv001
		//ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#else
		ulCount = sizeof(SCALER_MEMC_SETINOUTUSECASE_T) / sizeof(unsigned int);
#endif

	set_inoutput_framerate_info->input_frame_rate= input_frame;
	set_inoutput_framerate_info->output_frame_rate= output_frame;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	//change endian
	set_inoutput_framerate_info->input_frame_rate = htonl(set_inoutput_framerate_info->input_frame_rate);
	set_inoutput_framerate_info->output_frame_rate = htonl(set_inoutput_framerate_info->output_frame_rate);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETINOUTPUTFRAMERATE,0,0))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETINOUTPUTFRAMERATE RPC fail !!!\n", ret);
	}
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return ;
	Scaler_MEMC_SetInputFrameRate(set_inoutput_framerate_info->input_frame_rate);
	Scaler_MEMC_SetOutputFrameRate(set_inoutput_framerate_info->output_frame_rate);
#else

#ifdef BUILD_QUICK_SHOW
	Scaler_MEMC_SetInputFrameRate(set_inoutput_framerate_info->input_frame_rate);
	Scaler_MEMC_SetOutputFrameRate(set_inoutput_framerate_info->output_frame_rate);
#else //!BUILD_QUICK_SHOW
#if RTK_MEMC_Performance_tunging_from_tv001
		task.type = SCALERIOC_MEMC_SETINOUTPUTFRAMERATE;
		memcpy(&task.data,set_inoutput_framerate_info,sizeof(SCALER_MEMC_INOUTFRAMERATE_T));
		rtd_pr_memc_notice("[%s][%d][%d,%d]\n",__FUNCTION__, __LINE__, set_inoutput_framerate_info->input_frame_rate, set_inoutput_framerate_info->output_frame_rate);
		Scaler_MEMC_PushTask(&task);
#else
		if((vbe_disp_get_stop_memc_push_flag() == 1) && ((rtd_inl(0xB800011c)>>30)==0))
		{
			rtd_pr_memc_notice("[%s][%d]VRR doesn't aplly!\n",__FUNCTION__, __LINE__);
			return;
		}else{
			task.type = SCALERIOC_MEMC_SETINOUTPUTFRAMERATE;
			memcpy(&task.data,set_inoutput_framerate_info,sizeof(SCALER_MEMC_INOUTFRAMERATE_T));
			rtd_pr_memc_notice("[%s][%d][%d,%d]\n",__FUNCTION__, __LINE__, set_inoutput_framerate_info->input_frame_rate, set_inoutput_framerate_info->output_frame_rate);
			Scaler_MEMC_PushTask(&task);
		};
#endif//end RTK_MEMC_Performance_tunging_from_tv001
#endif//end BUILD_QUICK_SHOW


	#endif
#endif
#endif
#endif
}
#else
void Scaler_MEMC_SetInputFrameRate(VPQ_MEMC_INPUT_FRAMERATE input_frame){
	int ret;

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETINPUTFRAMERATE, input_frame, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETINPUTFRAMERATE RPC fail !!!\n", ret);
	}
}

void Scaler_MEMC_SetOutputFrameRate(VPQ_MEMC_OUTPUT_FRAMERATE output_frame){
	int ret;

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETOUTPUTFRAMERATE, output_frame, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETOUTPUTFRAMERATE RPC fail !!!\n", ret);
	}
}
#endif

#ifndef BUILD_QUICK_SHOW

long Scaler_MEMC_SetFrameRepeatEnable(BOOL bEnable){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS	
		return 0;
#else

	int ret = 0;
	unsigned int semaphoreCnt = 0;
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC	
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
       #endif
#endif
	while((IoReg_Read32(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg)&_BIT30) && semaphoreCnt < 100000) {
		semaphoreCnt++;
	}
	if(semaphoreCnt >= 100000) {
		rtd_pr_memc_debug( "[MEMC]Scaler_MEMC_SetFrameRepeatEnable timed out!\n");
		return -1;
	}
	IoReg_SetBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETMEMCFRAMEREPEATENABLE, bEnable, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETMEMCFRAMEREPEATENABLE RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return 0;
	Scaler_MEMC_SetMEMCFrameRepeatEnable(bEnable);
#else

		task.type = SCALERIOC_MEMC_SETMEMCFRAMEREPEATENABLE;
		task.data.value = bEnable;
		Scaler_MEMC_PushTask(&task);
	#endif
#endif
	IoReg_ClearBits(DI_IM_DI_BLEND_AND_WEIGHTING_METHOD_reg,_BIT30);
	return ret;
#endif	
#endif
}

void Scaler_MEMC_SetInOutMode(VPQ_MEMC_INOUT_MODE emom){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
       #endif
#endif	   
	int ret;

	ret=0;

#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
#if 0//RTK_MEMC_Performance_tunging_from_tv001//xj///寫法不同但通用
	//int ret=0;
#endif
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETMEMCINOUTMODE, emom, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_SETMEMCINOUTMODE RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return ;
	ISR_Scaler_MEMC_SetInOutMode(emom);
#else

		task.type = SCALERIOC_MEMC_SETMEMCINOUTMODE;
		task.data.value = emom;
		Scaler_MEMC_PushTask(&task);
	#endif
#endif
#endif
#endif
}
#endif

void Scaler_MEMC_Instanboot_InitPhaseTable(BOOL bEnable){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

	int ret;
	ret=0;

#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_INSTANBOOT_INITPHASETABLE, bEnable, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_INSTANBOOT_INITPHASETABLE RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	Scaler_MEMC_SetInstantBootInitPhaseTable();
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_INSTANBOOT_INITPHASETABLE, bEnable, 1))){
		MEMC_DBG_PRINT("[MEMC]ret=%d, SCALERIOC_MEMC_INSTANBOOT_INITPHASETABLE RPC fail !!!\n", ret);
	}
#endif
#endif
#endif
}

#define LBMC_prefetch_line_cnt 80

void Scaler_MEMC_SetInOutFrameRate(void){
	unsigned char memc_input_frame_rate;
	unsigned char memc_output_frame_rate;
	unsigned int memc_v_porch_count;

	ppoverlay_memcdtg_dv_total_RBUS memcdtg_dv_total_reg;
	ppoverlay_memcdtg_dv_den_start_end_RBUS memcdtg_dv_den_start_end_reg;

	SLR_VOINFO *pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());//k24
	if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) && (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_SUB_DISPLAY)){
		rtd_pr_memc_err( "[%s]SLR_SUB_DISPLAY don't config memc inout framerate.\n",__FUNCTION__);
		return;
	}

	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		rtd_pr_memc_err( "get_MEMC_bypass_status_refer_platform_model=1, no need set memc in/out framerate.\n");
		return;
	}

	if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_24000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_24500 ) )					// 24Hz
	{
		memc_input_frame_rate = VPQ_INPUT_24HZ;
//		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){

#if RTK_MEMC_Performance_tunging_from_tv001
			if((Scaler_MEMC_GetMotionType() == VPQ_MEMC_TYPE_55_PULLDOWN) && (fwif_color_get_cinema_mode_en() == 1))
#else
			if(Scaler_get_realcinema_mode_condition())//if(Scaler_get_cinematic_mode_condition())//k23//
#endif
			{
				memc_output_frame_rate = VPQ_OUTPUT_96HZ; //24Hz => 96Hz	//tru-motion off + real cinema
			}else{
				if((vbe_disp_get_adaptivestream_fs_mode()) && (0 == vbe_disp_get_adaptivestream_force_framerate()))
					memc_output_frame_rate = VPQ_OUTPUT_96HZ;
				else if(Get_DISPLAY_REFRESH_RATE() == 240)
					memc_output_frame_rate = VPQ_OUTPUT_240HZ; //24Hz => 240Hz
				else if(Get_DISPLAY_REFRESH_RATE() == 165)
					memc_output_frame_rate = VPQ_OUTPUT_144HZ; //24Hz => 144Hz
                else if(Get_DISPLAY_REFRESH_RATE() == 144)
					memc_output_frame_rate = VPQ_OUTPUT_144HZ; //24Hz => 144Hz
				else
					memc_output_frame_rate = VPQ_OUTPUT_120HZ; //24Hz => 120Hz
			}
		}
		else{
#if RTK_MEMC_Performance_tunging_from_tv001
			if((Scaler_MEMC_GetMotionType() == VPQ_MEMC_TYPE_55_PULLDOWN) && (fwif_color_get_cinema_mode_en() == 1))
#else
			if(Scaler_get_realcinema_mode_condition())//if(Scaler_get_realcinema_mode_condition() || Scaler_get_cinematic_mode_condition())//
#endif
			{
				memc_output_frame_rate = VPQ_OUTPUT_48HZ; //24Hz => 48Hz	//tru-motion off + real cinema
			}else{
				if((vbe_disp_get_adaptivestream_fs_mode()) && (0 == vbe_disp_get_adaptivestream_force_framerate()))
					memc_output_frame_rate = VPQ_OUTPUT_48HZ;
				else
					memc_output_frame_rate = VPQ_OUTPUT_60HZ; //24Hz => 60Hz
			}
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_24500 ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_25000_pOFFSET ) )				// 25Hz
	{
		memc_input_frame_rate = VPQ_INPUT_25HZ;
//		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_225HZ; //25Hz => 225Hz
            else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_150HZ; //25Hz => 150Hz
			else if(Get_DISPLAY_REFRESH_RATE() == 144)
				memc_output_frame_rate = VPQ_OUTPUT_125HZ; //25Hz => 125Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_100HZ; //25Hz => 100Hz
		}
		else{
			if(Scaler_DispGetInputInfo(SLR_INPUT_DTG_MASTER_V_FREQ) == 60 && (pVOInfo && pVOInfo->film_mode))
				memc_output_frame_rate = VPQ_OUTPUT_60HZ; //25Hz => 60Hz
			else
			    memc_output_frame_rate = VPQ_OUTPUT_50HZ; //25Hz => 50Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_30000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_30000_pOFFSET ) )		// 30Hz
	{
		memc_input_frame_rate = VPQ_INPUT_30HZ;
//		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_240HZ; //30Hz => 240Hz
            else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_150HZ; //30Hz => 150Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_120HZ; //30Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //30Hz => 60Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_48000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_48000_pOFFSET ) )		// 48Hz
	{
		memc_input_frame_rate = VPQ_INPUT_48HZ;
//		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_240HZ; //48Hz => 240Hz
			else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_144HZ; //48Hz => 144Hz
			else if(Get_DISPLAY_REFRESH_RATE() == 144)
				memc_output_frame_rate = VPQ_OUTPUT_144HZ; //48Hz => 144Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_96HZ; //48Hz => 96Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_48HZ; //48Hz => 48Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_50000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_50000_pOFFSET ) )		// 50Hz
	{
		memc_input_frame_rate = VPQ_INPUT_50HZ;
//		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_200HZ; //50Hz => 200Hz
            else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_150HZ; //50Hz => 150Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_100HZ; //50Hz => 100Hz
		}
		else{
			if(Scaler_DispGetInputInfo(SLR_INPUT_DTG_MASTER_V_FREQ) == 60 && (pVOInfo && pVOInfo->film_mode))
			    memc_output_frame_rate = VPQ_OUTPUT_60HZ; //50Hz => 60Hz
			else
                memc_output_frame_rate = VPQ_OUTPUT_50HZ; //50Hz => 50Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_60000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_60000_pOFFSET ) )		// 60Hz
	{
		memc_input_frame_rate = VPQ_INPUT_60HZ;
//		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_240HZ; //60Hz => 240Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_120HZ; //60Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //60Hz => 60Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_100000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_100000_pOFFSET ) )	// 100Hz
	{
		memc_input_frame_rate = VPQ_INPUT_100HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_100HZ; //100Hz => 100Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_50HZ; //100Hz => 50Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_120000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_120000_pOFFSET ) )	// 120Hz
	{
		memc_input_frame_rate = VPQ_INPUT_120HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_120HZ; //120Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_144000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_144000_pOFFSET ) )	// 144Hz
	{
		memc_input_frame_rate = VPQ_INPUT_144HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
            if(Get_DISPLAY_REFRESH_RATE() >= 144)
            {
                memc_output_frame_rate = VPQ_OUTPUT_144HZ;
            }
            else
            {
                memc_output_frame_rate = VPQ_OUTPUT_120HZ; //120Hz => 120Hz
            }
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_165000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_165000_pOFFSET ) )	// 165Hz
	{
		memc_input_frame_rate = VPQ_INPUT_165HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_165HZ; //165Hz => 165Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_240000_mOFFSET ) && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < V_FREQ_240000_pOFFSET ) )	// 240Hz
	{
		memc_input_frame_rate = VPQ_INPUT_240HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_240HZ; //240Hz => 240Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else																																									// others
	{
		memc_input_frame_rate = VPQ_INPUT_60HZ;
//		if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_120HZ; //60Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //60Hz => 60Hz
		}
	}

	//k23
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI)
		//|| (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN))
	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) /*|| (Get_DISPLAY_PANEL_TYPE() == P_STB_OUTPUT_TYPE)*/
		|| (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN))

	{
		memc_output_frame_rate = VPQ_OUTPUT_60HZ; //60Hz => 60Hz
	}

	rtd_pr_memc_notice( "[MEMC_BRING_UP][%s][%d][%d,%d]\n",__FUNCTION__, __LINE__, memc_output_frame_rate, Get_DISPLAY_REFRESH_RATE());
	rtd_pr_memc_err( "[MEMC]UZU DTG input frame_rate = %d\n",memc_input_frame_rate);
	rtd_pr_memc_err( "[MEMC]MEMC DTG output frame_rate = %d\n",memc_output_frame_rate);

	//set MEMC in/out frame rate
	//Scaler_MEMC_SetInputFrameRate(memc_input_frame_rate);
	//Scaler_MEMC_SetOutputFrameRate(memc_output_frame_rate);
	Scaler_MEMC_SetInOutputFrameRate(memc_input_frame_rate, memc_output_frame_rate);

	// [ML8BU-1089] MEMC LBMC limit
	memcdtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_TOTAL_reg);
	memcdtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg);

	if( memcdtg_dv_total_reg.memcdtg_dv_total > memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end )
	{
		memc_v_porch_count = memcdtg_dv_total_reg.memcdtg_dv_total - memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end + memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta;

		if( memc_v_porch_count >= LBMC_prefetch_line_cnt )
			Scaler_set_D1_Vporch_shrink( 0, memc_v_porch_count );
		else
			Scaler_set_D1_Vporch_shrink( 1, memc_v_porch_count );
	}
}

void Scaler_MEMC_SetInOutFrameRateByDisp(unsigned char _channel)
{
	SCALER_DISP_CHANNEL channel = (SCALER_DISP_CHANNEL)_channel;
	unsigned char memc_input_frame_rate;
	unsigned char memc_output_frame_rate;
	//unsigned int vfreq = 0;//frame rate
	unsigned int memc_v_porch_count;
	SLR_VOINFO *pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());

	ppoverlay_memcdtg_dv_total_RBUS memcdtg_dv_total_reg;
	ppoverlay_memcdtg_dv_den_start_end_RBUS memcdtg_dv_den_start_end_reg;

#if 1//#if RTK_MEMC_Performance_tunging_from_tv001//k24
	 unsigned int ulVFreq = 0;
        
    if(get_scaler_qms_mode_flag() && (_channel == SLR_MAIN_DISPLAY))
	{
		ulVFreq = get_qms_current_framerate();
		if(ulVFreq == 0)
		{
			rtd_pr_memc_emerg("Scaler_MEMC_SetInOutFrameRateByDisp Err QMS vfreq is zero, fallback to using disp framerate \n");
		}
	}

    if (ulVFreq == 0)
	{
      	ulVFreq = Scaler_DispGetInputInfoByDisp(channel,SLR_INPUT_V_FREQ_1000);

#ifdef CONFIG_MDOMAIN_FORCE_FRC
       if (TRUE == modestate_get_mdomain_frc_flag(_channel))
        {
               rtd_pr_memc_emerg("modestate_get_mdomain_frc_flag = TRUE\n");

                // double input frame rate
            ulVFreq = 	modestate_get_mdomain_frc_vfreq(_channel) * 100;
       	}
#endif
    }        
#endif

	if( _channel == SLR_SUB_DISPLAY )
	{
		if( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE )
		{
			rtd_pr_memc_err("[%s] SLR_SUB_DISPLAY don't config memc inout framerate.\n", __FUNCTION__);
			return;
		}
		else if( Get_Val_first_boot_run_main() == TRUE )
		{
			ulVFreq = V_FREQ_60000;
			rtd_pr_memc_notice("[%s] sub path only : set input framerate = %d\n", __FUNCTION__, ulVFreq);
		}
	}

	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		rtd_pr_memc_err( "get_MEMC_bypass_status_refer_platform_model=1, no need set memc in/out framerate.\n");
		return;
	}

	if( ( ulVFreq >= V_FREQ_24000_mOFFSET ) && ( ulVFreq < V_FREQ_24500 ) )						// 24Hz
	{
		memc_input_frame_rate = VPQ_INPUT_24HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(0/*Scaler_get_cinematic_mode_condition()*/)
			{
				memc_output_frame_rate = VPQ_OUTPUT_96HZ; //24Hz => 96Hz	//tru-motion off + real cinema
			}else{
				if((vbe_disp_get_adaptivestream_fs_mode()) && (0 == vbe_disp_get_adaptivestream_force_framerate()))
					memc_output_frame_rate = VPQ_OUTPUT_96HZ;
				else if(Get_DISPLAY_REFRESH_RATE() == 240)
					memc_output_frame_rate = VPQ_OUTPUT_240HZ; //24Hz => 240Hz
				else if(Get_DISPLAY_REFRESH_RATE() == 165)
					memc_output_frame_rate = VPQ_OUTPUT_144HZ; //24Hz => 144Hz
                else if(Get_DISPLAY_REFRESH_RATE() == 144)
					memc_output_frame_rate = VPQ_OUTPUT_144HZ; //24Hz => 144Hz
				else
					memc_output_frame_rate = VPQ_OUTPUT_120HZ; //24Hz => 120Hz
			}
		}
		else{
#if RTK_MEMC_Performance_tunging_from_tv001
			if((Scaler_MEMC_GetMotionType() == VPQ_MEMC_TYPE_OFF) && (fwif_color_get_cinema_mode_en() == 1))
#else
			if(Scaler_get_realcinema_mode_condition())//if(Scaler_get_realcinema_mode_condition() || Scaler_get_cinematic_mode_condition())//k23//
#endif						
			{
				memc_output_frame_rate = VPQ_OUTPUT_48HZ; //24Hz => 48Hz	//tru-motion off + real cinema
			}else{
				if ((vbe_disp_get_adaptivestream_fs_mode()) && (0 == vbe_disp_get_adaptivestream_force_framerate()))
					memc_output_frame_rate = VPQ_OUTPUT_48HZ;
				else
					memc_output_frame_rate = VPQ_OUTPUT_60HZ; //24Hz => 60Hz
			}
		}
	}
	else if( ( ulVFreq >= V_FREQ_24500 ) && ( ulVFreq < V_FREQ_25000_pOFFSET ) )				// 25Hz
	{
		memc_input_frame_rate = VPQ_INPUT_25HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_225HZ; //25Hz => 225Hz
            else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_150HZ; //25Hz => 150Hz
			else if(Get_DISPLAY_REFRESH_RATE() == 144)
				memc_output_frame_rate = VPQ_OUTPUT_125HZ; //25Hz => 125Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_100HZ; //25Hz => 100Hz
		}
		else{
			if(Scaler_DispGetInputInfo(SLR_INPUT_DTG_MASTER_V_FREQ) == 60 && (pVOInfo && pVOInfo->film_mode))
				memc_output_frame_rate = VPQ_OUTPUT_60HZ; //25Hz => 60Hz
			else
			    memc_output_frame_rate = VPQ_OUTPUT_50HZ; //25Hz => 50Hz
		}
	}
	else if( ( ulVFreq >= V_FREQ_30000_mOFFSET ) && ( ulVFreq < V_FREQ_30000_pOFFSET ) )		// 30Hz
	{
		memc_input_frame_rate = VPQ_INPUT_30HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_240HZ; //30Hz => 240Hz
            else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_150HZ; //30Hz => 150Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_120HZ; //30Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //30Hz => 60Hz
		}
	}
#if RTK_MEMC_Performance_tunging_from_tv001
	#ifdef CONFIG_MDOMAIN_FORCE_FRC
	else if( ( ulVFreq >= V_FREQ_48000_mOFFSET ) && ( ulVFreq < V_FREQ_48000_pOFFSET ) )		// 48Hz
	{
		memc_input_frame_rate = VPQ_INPUT_48HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_240HZ; //48Hz => 240Hz
			else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_144HZ; //48Hz => 144Hz
			else if(Get_DISPLAY_REFRESH_RATE() == 144)
				memc_output_frame_rate = VPQ_OUTPUT_144HZ; //48Hz => 144Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_96HZ; //48Hz => 96Hz
		}
		else{
			if((Scaler_MEMC_GetMotionType() == VPQ_MEMC_TYPE_OFF) && (fwif_color_get_cinema_mode_en() == 1)){
				memc_output_frame_rate = VPQ_OUTPUT_48HZ; //24Hz => 48Hz	//tru-motion off + real cinema
			}else{
				if ((vbe_disp_get_adaptivestream_fs_mode()) && (0 == vbe_disp_get_adaptivestream_force_framerate()))
					memc_output_frame_rate = VPQ_OUTPUT_48HZ;
				else
					memc_output_frame_rate = VPQ_OUTPUT_60HZ; //48Hz => 60Hz
			}
		}
	}
	#endif
#endif
	else if( ( ulVFreq >= V_FREQ_50000_mOFFSET ) && ( ulVFreq < V_FREQ_50000_pOFFSET ) )		// 50Hz
	{
		memc_input_frame_rate = VPQ_INPUT_50HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
            if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_200HZ; //50Hz => 200Hz
            else if(Get_DISPLAY_REFRESH_RATE() == 165)
				memc_output_frame_rate = VPQ_OUTPUT_150HZ; //50Hz => 150Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_100HZ; //50Hz => 100Hz
		}
		else{
			if(Scaler_DispGetInputInfo(SLR_INPUT_DTG_MASTER_V_FREQ) == 60 && (pVOInfo && pVOInfo->film_mode))
			    memc_output_frame_rate = VPQ_OUTPUT_60HZ; //50Hz => 60Hz
			else
                memc_output_frame_rate = VPQ_OUTPUT_50HZ; //50Hz => 50Hz
		}
	}
	else if( ( ulVFreq >= V_FREQ_60000_mOFFSET ) && ( ulVFreq < V_FREQ_60000_pOFFSET ) )		// 60Hz
	{
		memc_input_frame_rate = VPQ_INPUT_60HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			if(Get_DISPLAY_REFRESH_RATE() == 240)
				memc_output_frame_rate = VPQ_OUTPUT_240HZ; //60Hz => 240Hz
			else
				memc_output_frame_rate = VPQ_OUTPUT_120HZ; //60Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //60Hz => 60Hz
		}
	}
	else if( ( ulVFreq >= V_FREQ_100000_mOFFSET ) && ( ulVFreq < V_FREQ_100000_pOFFSET ) )		// 100Hz
	{
		memc_input_frame_rate = VPQ_INPUT_100HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_100HZ; //100Hz => 100Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_50HZ; //100Hz => 50Hz
		}
	}
	else if( ( ulVFreq >= V_FREQ_120000_mOFFSET ) && ( ulVFreq < V_FREQ_120000_pOFFSET ) )		// 120Hz
	{
		memc_input_frame_rate = VPQ_INPUT_120HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_120HZ; //120Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else if( ( ulVFreq >= V_FREQ_144000_mOFFSET ) && ( ulVFreq < V_FREQ_144000_pOFFSET ) )		// 144Hz
	{
		memc_input_frame_rate = VPQ_INPUT_144HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
            if(Get_DISPLAY_REFRESH_RATE() >= 144)
            {
                memc_output_frame_rate = VPQ_OUTPUT_144HZ;
            }
            else
            {
                memc_output_frame_rate = VPQ_OUTPUT_120HZ; //120Hz => 120Hz
            }
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else if( ( ulVFreq >= V_FREQ_165000_mOFFSET ) && ( ulVFreq < V_FREQ_165000_pOFFSET ) )		// 165Hz
	{
		memc_input_frame_rate = VPQ_INPUT_165HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_165HZ; //165Hz => 165Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else if( ( ulVFreq >= V_FREQ_240000_mOFFSET ) && ( ulVFreq < V_FREQ_240000_pOFFSET ) )		// 240Hz
	{
		memc_input_frame_rate = VPQ_INPUT_240HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_240HZ; //240Hz => 240Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //120Hz => 60Hz
		}
	}
	else																						// others
	{
		memc_input_frame_rate = VPQ_INPUT_60HZ;
		//if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_PIXEL_MODE) == PIXEL_MODE_2_PIXEL){
		if(Get_DISPLAY_REFRESH_RATE() > 60){
			memc_output_frame_rate = VPQ_OUTPUT_120HZ; //60Hz => 120Hz
		}
		else{
			memc_output_frame_rate = VPQ_OUTPUT_60HZ; //60Hz => 60Hz
		}
	}

	//k23
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI)
		//|| (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN))
	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) /*|| (Get_DISPLAY_PANEL_TYPE() == P_STB_OUTPUT_TYPE)*/)
	{
		memc_output_frame_rate = VPQ_OUTPUT_60HZ; //60Hz => 60Hz
	}

	rtd_pr_memc_notice( "[%s][%d][%d,%d]\n",__FUNCTION__, __LINE__, memc_output_frame_rate, Get_DISPLAY_REFRESH_RATE());
	rtd_pr_memc_err( "[MEMC]UZU DTG input frame_rate = %d\n",memc_input_frame_rate);
	rtd_pr_memc_err( "[MEMC]MEMC DTG output frame_rate = %d\n",memc_output_frame_rate);

	//set MEMC in/out frame rate
	//Scaler_MEMC_SetInputFrameRate(memc_input_frame_rate);
	//Scaler_MEMC_SetOutputFrameRate(memc_output_frame_rate);
	Scaler_MEMC_SetInOutputFrameRate(memc_input_frame_rate, memc_output_frame_rate);

	// [ML8BU-1089] MEMC LBMC limit
	memcdtg_dv_total_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_TOTAL_reg);
	memcdtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg);

	if( memcdtg_dv_total_reg.memcdtg_dv_total > memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end )
	{
		memc_v_porch_count = memcdtg_dv_total_reg.memcdtg_dv_total - memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end + memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta;

		if( memc_v_porch_count >= LBMC_prefetch_line_cnt )
			Scaler_set_D1_Vporch_shrink( 0, memc_v_porch_count );
		else
			Scaler_set_D1_Vporch_shrink( 1, memc_v_porch_count );
	}
}

void Scaler_MEMC_ForceBg(unsigned char bEnable)
{
	ppoverlay_memc_mux_ctrl_bg_RBUS memc_mux_ctrl_bg_reg;
	ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;

	memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
	memc_mux_ctrl_bg_reg.memc_out_bg_en = bEnable;
	IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, memc_mux_ctrl_bg_reg.regValue);

	//set apply
	ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
	ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set = 1;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
	ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
}

#if 1//RTK_MEMC_Performance_tunging_from_tv001
/**
 * @brief This function get the MEMC output size informtion base on the scaler Disp. information.
 * @retval unsigned char, return the information of the panel size 	
*/
unsigned char Scaler_MEMC_GetPanelSizeByDisp(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else

	unsigned char memc_output_panelsize = 0;
	unsigned int DISP_HPOS_ACT_END = 0;
	static unsigned char print_cnt = 0;
	static unsigned char pre_PanelSize = MEMC_SIZE_NUM;

	DISP_HPOS_ACT_END = Get_DISP_ACT_END_HPOS();
	
	if(DISP_HPOS_ACT_END>=7680){
		memc_output_panelsize = MEMC_SIZE_8K;
		
	}else if(DISP_HPOS_ACT_END>=3840){
		memc_output_panelsize = MEMC_SIZE_4K;
		
	}else if(DISP_HPOS_ACT_END>=2560){
		memc_output_panelsize = MEMC_SIZE_3K;
		
	}else if(DISP_HPOS_ACT_END>=1920){
		memc_output_panelsize = MEMC_SIZE_2K;		
		
	}else{
		memc_output_panelsize = MEMC_SIZE_4K;
	}

	pre_PanelSize = memc_output_panelsize;

	if( (DISP_HPOS_ACT_END==7680) || (DISP_HPOS_ACT_END==3840) || (DISP_HPOS_ACT_END==2560) || (DISP_HPOS_ACT_END==1920)){
		print_cnt=1;
	}else{
		if(print_cnt<255) 
			print_cnt++;
		else
			print_cnt=255;
	}	

	//Normal : it will print when the  input size changed
	//Warning : it will print when the print_cnt = 2~10, when the HACT doesn't  match the golden size defined
	if( ((print_cnt<=10) && (print_cnt>1)) || (memc_output_panelsize!=pre_PanelSize) ){
		rtd_pr_memc_notice( "[%s][%d][ACTEND,H=%d,V=%d,PANELTYPE=%d,][lbmc,hact=%d,vact=%d,][%d]\n",__FUNCTION__, __LINE__,
											Get_DISP_ACT_END_HPOS(),Get_DISP_ACT_END_VPOS(),Get_DISPLAY_PANEL_TYPE(),
											(rtd_inl(LBMC_LBMC_20_reg)&&LBMC_LBMC_20_lbmc_h_act_mask),(rtd_inl(LBMC_LBMC_20_reg)&&LBMC_LBMC_20_lbmc_v_act_mask),
											print_cnt);
	}

	
	
	return memc_output_panelsize;
#endif
}
#endif

#ifndef BUILD_QUICK_SHOW
void MEMC_SYS_Delay(unsigned char msecs)
{
	    unsigned long usecs = (unsigned long)(msecs * 1000);
    if (usecs <= 20000)
        usleep_range(usecs, usecs);
    else
        msleep(msecs);
}
#endif //BUILD_QUICK_SHOW

#ifndef BUILD_QUICK_SHOW
void Scaler_MEMC_PowerSaving_Disable(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else
#if 1//#ifdef KID23_TO_WEBOS

	//enable me sram
	rtd_clearbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
	rtd_clearbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

	//enable more reg - 0514
	rtd_clearbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
	rtd_clearbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
	rtd_clearbits(KME_BIST_dm_misc_reg, _BIT13);
	rtd_clearbits(KME_BIST_dm_misc_reg, _BIT21);
	rtd_clearbits(KME_BIST_dm_drf_fail0_reg, _BIT13);
#endif		
#endif	
}
#endif

void Scaler_MEMC_CLK_Enable(void){

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else

//#ifndef CONFIG_MEMC_BYPASS
#if 1//#ifdef KID23_TO_WEBOS

#if RTK_MEMC_Performance_tunging_from_tv001
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
	sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	sys_reg_sys_srst4_RBUS sys_reg_sys_srst4_reg;
	sys_reg_sys_clken4_RBUS sys_reg_sys_clken4_reg;
	sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
	sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
	
	sys_reg_sys_srst4_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST4_reg);
	sys_reg_sys_clken4_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN4_reg);

	rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
	rtd_pr_memc_notice("CLK Status >> ( %d ,%d ,%d ,%d) ",(IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>27),(IoReg_Read32(SYS_REG_SYS_SRST3_reg)>>27), IoReg_Read32(SYS_REG_SYS_CLKEN4_reg), IoReg_Read32(SYS_REG_SYS_SRST4_reg));

//	if(!((sys_reg_sys_srst3_REG.regValue>>27 & BIT0) && (sys_reg_sys_srst3_REG.regValue>>30 & BIT0) && (sys_reg_sys_srst3_REG.regValue>>31 & BIT0))){
	if(((sys_reg_sys_srst3_reg.regValue>>29) != 7) || (sys_reg_sys_srst3_reg.rstn_memc_mc == 0) || (sys_reg_sys_srst4_reg.rstn_memc_mc_func == 0)){
		//SYS_RSTN b800010C
		if(sys_reg_sys_srst3_reg.rstn_memc_rbus != 1){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
			sys_reg_sys_srst3_reg.regValue = 0;
			sys_reg_sys_srst3_reg.rstn_memc_rbus = 1;
			sys_reg_sys_srst3_reg.write_data = 1;
			IoReg_Write32(SYS_REG_SYS_SRST3_reg,sys_reg_sys_srst3_reg.regValue);
		}

		sys_reg_sys_srst3_reg.regValue = 0;
		sys_reg_sys_srst3_reg.rstn_memc= 1;
		sys_reg_sys_srst3_reg.rstn_memc_me = 1;
		sys_reg_sys_srst3_reg.rstn_memc_mc = 1;
		sys_reg_sys_srst3_reg.write_data = 1;
		IoReg_Write32(SYS_REG_SYS_SRST3_reg,sys_reg_sys_srst3_reg.regValue);
		sys_reg_sys_srst4_reg.regValue = 0;
		sys_reg_sys_srst4_reg.rstn_memc_mc_func = 1;
		sys_reg_sys_srst4_reg.write_data = 1;
		IoReg_Write32(SYS_REG_SYS_SRST4_reg,sys_reg_sys_srst4_reg.regValue);
	}

//	if(!((sys_reg_sys_clken3_reg.regValue>>27 & BIT0) && (sys_reg_sys_clken3_reg.regValue>>30 & BIT0) && (sys_reg_sys_clken3_reg.regValue>>31 & BIT0))){
	if(((sys_reg_sys_clken3_reg.regValue>>29) != 7) || (sys_reg_sys_clken3_reg.clken_memc_mc == 0) || (sys_reg_sys_clken4_reg.clken_memc_mc_func == 0)){
		//SYS_CLKEN3 b800011C
		if(sys_reg_sys_clken3_reg.clken_memc_rbus != 1){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
			sys_reg_sys_clken3_reg.regValue = 0;
			sys_reg_sys_clken3_reg.clken_memc_rbus = 1;
			sys_reg_sys_clken3_reg.write_data = 1;
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg,sys_reg_sys_clken3_reg.regValue);
		}

		sys_reg_sys_clken3_reg.regValue = 0;
		sys_reg_sys_clken3_reg.clken_memc = 1;
		sys_reg_sys_clken3_reg.clken_memc_me= 1;
		sys_reg_sys_clken3_reg.clken_memc_mc= 1;
		sys_reg_sys_clken3_reg.write_data = 1;
		IoReg_Write32(SYS_REG_SYS_CLKEN3_reg,sys_reg_sys_clken3_reg.regValue);
		sys_reg_sys_clken4_reg.regValue = 0;
		sys_reg_sys_clken4_reg.clken_memc_mc_func = 1;
		sys_reg_sys_clken4_reg.write_data = 1;
		IoReg_Write32(SYS_REG_SYS_CLKEN4_reg,sys_reg_sys_clken4_reg.regValue);
		}
	}
#else
	sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	sys_reg_sys_srst4_RBUS sys_reg_sys_srst4_reg;
	sys_reg_sys_clken4_RBUS sys_reg_sys_clken4_reg;

	//if(get_MEMC_bypass_status_refer_platform_model() == FALSE || get_platform() == PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
		sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
		sys_reg_sys_srst4_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST4_reg);
		sys_reg_sys_clken4_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN4_reg);

		rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
		rtd_pr_memc_notice("CLK Status >> ( %d ,%d ,%d ,%d) ",(IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>27),(IoReg_Read32(SYS_REG_SYS_SRST3_reg)>>27), IoReg_Read32(SYS_REG_SYS_CLKEN4_reg), IoReg_Read32(SYS_REG_SYS_SRST4_reg));

		if(((sys_reg_sys_srst3_reg.regValue>>29) != 7) || (sys_reg_sys_srst3_reg.rstn_memc_mc == 0) || (sys_reg_sys_srst4_reg.rstn_memc_mc_func == 0)){
			//SYS_RSTN b800010C
			if(sys_reg_sys_srst3_reg.rstn_memc_rbus != 1){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
				sys_reg_sys_srst3_reg.regValue = 0;
				sys_reg_sys_srst3_reg.rstn_memc_rbus = 1;
				sys_reg_sys_srst3_reg.write_data = 1;
				IoReg_Write32(SYS_REG_SYS_SRST3_reg,sys_reg_sys_srst3_reg.regValue);
			}

			sys_reg_sys_srst3_reg.regValue = 0;
			sys_reg_sys_srst3_reg.rstn_memc= 1;
			sys_reg_sys_srst3_reg.rstn_memc_me = 1;
			sys_reg_sys_srst3_reg.rstn_memc_mc = 1;
			sys_reg_sys_srst3_reg.write_data = 1;
			IoReg_Write32(SYS_REG_SYS_SRST3_reg,sys_reg_sys_srst3_reg.regValue);
			sys_reg_sys_srst4_reg.regValue = 0;
			sys_reg_sys_srst4_reg.rstn_memc_mc_func = 1;
			sys_reg_sys_srst4_reg.write_data = 1;
			IoReg_Write32(SYS_REG_SYS_SRST4_reg,sys_reg_sys_srst4_reg.regValue);
		}

		if(((sys_reg_sys_clken3_reg.regValue>>29) != 7) || (sys_reg_sys_clken3_reg.clken_memc_mc == 0) || (sys_reg_sys_clken4_reg.clken_memc_mc_func == 0)){
			//SYS_CLKEN3 b800011C
			if(sys_reg_sys_clken3_reg.clken_memc_rbus != 1){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
				sys_reg_sys_clken3_reg.regValue = 0;
				sys_reg_sys_clken3_reg.clken_memc_rbus = 1;
				sys_reg_sys_clken3_reg.write_data = 1;
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg,sys_reg_sys_clken3_reg.regValue);
			}

			sys_reg_sys_clken3_reg.regValue = 0;
			sys_reg_sys_clken3_reg.clken_memc = 1;
			sys_reg_sys_clken3_reg.clken_memc_me= 1;
			sys_reg_sys_clken3_reg.clken_memc_mc= 1;
			sys_reg_sys_clken3_reg.write_data = 1;
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg,sys_reg_sys_clken3_reg.regValue);
			sys_reg_sys_clken4_reg.regValue = 0;
			sys_reg_sys_clken4_reg.clken_memc_mc_func = 1;
			sys_reg_sys_clken4_reg.write_data = 1;
			IoReg_Write32(SYS_REG_SYS_CLKEN4_reg,sys_reg_sys_clken4_reg.regValue);
		}
  }
#endif
#endif
#endif
}

#ifndef BUILD_QUICK_SHOW
extern unsigned char g_MEMC_powersaving_on;

void Scaler_MEMC_Set_PowerSaving_Status(unsigned char mode)
{
	g_MEMC_powersaving_on = mode;//0:off, 1:on
}

unsigned char Scaler_MEMC_Get_PowerSaving_Status(void)
{
	return g_MEMC_powersaving_on;
}

#if 1//#if RTK_MEMC_Performance_tunging_from_tv001//k24
extern MEMC_INFO_FROM_SCALER *Get_Info_from_Scaler(VOID);
MEMC_INFO_FROM_SCALER *Scaler_Get_Info_from_Scaler(VOID)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	glb_Scaler_InfoFromScaler.in_frame_rate=0;
	glb_Scaler_InfoFromScaler.out_frame_rate=0;
	glb_Scaler_InfoFromScaler.game_mode_flag=0;
	glb_Scaler_InfoFromScaler.adaptive_stream_flag=0;
	glb_Scaler_InfoFromScaler.YT_callback_flag=0;
	glb_Scaler_InfoFromScaler.hdmi_flag=0;
	glb_Scaler_InfoFromScaler.vdec_flag=0;
	return (MEMC_INFO_FROM_SCALER *)&glb_Scaler_InfoFromScaler;
#else
	return Get_Info_from_Scaler();
#endif
}
#endif
#endif
static unsigned char glb_vpq_lowdelay_flag = 0;

void Scaler_MEMC_Set_VpqLowDelayToMEMC_Flag(unsigned char u1_mode)
{
	glb_vpq_lowdelay_flag = u1_mode;
}

unsigned char Scaler_MEMC_Get_VpqLowDelayToMEMC_Flag(VOID)
{
	return glb_vpq_lowdelay_flag;
}

#ifndef BUILD_QUICK_SHOW
void Scaler_MEMC_MC_CLK_Enable(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return ;
#else

//#ifndef CONFIG_MEMC_BYPASS
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
		sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
		sys_reg_sys_srst4_RBUS sys_reg_sys_srst4_reg;
		sys_reg_sys_clken4_RBUS sys_reg_sys_clken4_reg;
		sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
		sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
		
		sys_reg_sys_srst4_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST4_reg);
		sys_reg_sys_clken4_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN4_reg);

		rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
		rtd_pr_memc_notice("CLK Status >> ( %d ,%d ,%d ,%d) ",(IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>27),(IoReg_Read32(SYS_REG_SYS_SRST3_reg)>>27), IoReg_Read32(SYS_REG_SYS_CLKEN4_reg), IoReg_Read32(SYS_REG_SYS_SRST4_reg));

		if((sys_reg_sys_srst3_reg.rstn_memc_rbus != 1) || (sys_reg_sys_clken3_reg.clken_memc_rbus != 1)){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x20000000);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x20000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x20000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x20000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x20000001);
		}
		if((sys_reg_sys_srst3_reg.rstn_memc == 0) || (sys_reg_sys_clken3_reg.clken_memc == 0)){
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000000);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);
		}
		if((sys_reg_sys_srst3_reg.rstn_memc_mc == 0) || (sys_reg_sys_clken3_reg.clken_memc_mc == 0)){
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x08000000);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x08000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x08000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x08000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x08000001);
		}
		if((sys_reg_sys_srst4_reg.rstn_memc_mc_func == 0) || (sys_reg_sys_clken4_reg.clken_memc_mc_func == 0)){
			IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);
			IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000005);
			IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);
			IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000005);
			IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000005);
		}
	}
//#endif
#endif
}

void Scaler_MEMC_MC_CLK_Disable(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return ;
#else

//#ifndef CONFIG_MEMC_BYPASS
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
		sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
		sys_reg_sys_srst4_RBUS sys_reg_sys_srst4_reg;
		sys_reg_sys_clken4_RBUS sys_reg_sys_clken4_reg;
		sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
		sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
		
		sys_reg_sys_srst4_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST4_reg);
		sys_reg_sys_clken4_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN4_reg);

		rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
		rtd_pr_memc_notice("CLK Status >> ( %d ,%d ,%d ,%d) ",(IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>27),(IoReg_Read32(SYS_REG_SYS_SRST3_reg)>>27), IoReg_Read32(SYS_REG_SYS_CLKEN4_reg), IoReg_Read32(SYS_REG_SYS_SRST4_reg));

		if((sys_reg_sys_srst3_reg.rstn_memc_rbus == 1) || (sys_reg_sys_clken3_reg.clken_memc_rbus == 1)){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x20000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x20000000);
		}
		if((sys_reg_sys_srst3_reg.rstn_memc == 1) || (sys_reg_sys_clken3_reg.clken_memc == 1)){
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000000);
		}
		if((sys_reg_sys_srst3_reg.rstn_memc_mc == 1) || (sys_reg_sys_clken3_reg.clken_memc_mc == 1)){
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x08000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x08000000);
		}
		if((sys_reg_sys_srst4_reg.rstn_memc_mc_func == 0) || (sys_reg_sys_clken4_reg.clken_memc_mc_func == 1)){
			IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);
			IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);
		}
	}
//#endif
#endif
}

void Scaler_MEMC_ME_CLK_Enable(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return ;
#else

//#ifndef CONFIG_MEMC_BYPASS
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
		sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;

		sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
		sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);

		rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
		rtd_pr_memc_notice("CLK Status >> ( %d ,%d ,%d ,%d) ",(IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>27),(IoReg_Read32(SYS_REG_SYS_SRST3_reg)>>27), IoReg_Read32(SYS_REG_SYS_CLKEN4_reg), IoReg_Read32(SYS_REG_SYS_SRST4_reg));

		if((sys_reg_sys_srst3_reg.rstn_memc_me == 0) || (sys_reg_sys_clken3_reg.clken_memc_me == 0)){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);
		}
	}
//#endif
#endif
}

void Scaler_MEMC_ME_CLK_Disable(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return ;
#else

//#ifndef CONFIG_MEMC_BYPASS
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
		sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;

		sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
		sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);

		rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
		rtd_pr_memc_notice("CLK Status >> ( %d ,%d ,%d ,%d) ",(IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>27),(IoReg_Read32(SYS_REG_SYS_SRST3_reg)>>27), IoReg_Read32(SYS_REG_SYS_CLKEN4_reg), IoReg_Read32(SYS_REG_SYS_SRST4_reg));

		if((sys_reg_sys_srst3_reg.rstn_memc_me == 1) || (sys_reg_sys_clken3_reg.clken_memc_me == 1)){ //Clock enable bit for rbus clock in MEMC , should be set beforce CLKEN_MEMC/CLKEN_MEME_ME
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
		}
	}
//#endif
#endif
}
#endif

/**
 * @brief This function get CLK status of MEMC
 * @retval 1		MEMC All CLK Enable
 * @retval 0		One of the "MEMC CLK" disable
*/
unsigned char Scaler_MEMC_CLK_Check(void){

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return FALSE;
#else

#if 1//#ifdef KID23_TO_WEBOS

	//rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);

//#ifdef CONFIG_MEMC_BYPASS
//	return TRUE;
//#else
	sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	sys_reg_sys_srst4_RBUS sys_reg_sys_srst4_reg;
	sys_reg_sys_clken4_RBUS sys_reg_sys_clken4_reg;

	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		return FALSE;//KTASKWBS-24448
	}else{
		sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
		sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
		sys_reg_sys_srst4_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST4_reg);
		sys_reg_sys_clken4_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN4_reg);

		if((sys_reg_sys_srst3_reg.rstn_memc == 0) || (sys_reg_sys_srst3_reg.rstn_memc_me == 0) || (sys_reg_sys_srst3_reg.rstn_memc_rbus == 0) || (sys_reg_sys_srst3_reg.rstn_memc_mc == 0) || (sys_reg_sys_srst4_reg.rstn_memc_mc_func == 0)\
			|| (sys_reg_sys_clken3_reg.clken_memc == 0) || (sys_reg_sys_clken3_reg.clken_memc_me == 0) || (sys_reg_sys_clken3_reg.clken_memc_rbus == 0) || (sys_reg_sys_clken3_reg.clken_memc_mc == 0) || (sys_reg_sys_clken4_reg.clken_memc_mc_func == 0))
		{
			rtd_pr_memc_notice("[MEMC][%s] Warning !! CLK not enable!!( %x,%x,%x,%x )", __func__, sys_reg_sys_clken3_reg.regValue, sys_reg_sys_srst3_reg.regValue, sys_reg_sys_clken4_reg.regValue, sys_reg_sys_srst4_reg.regValue);
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
#endif
#endif

}

unsigned char Scaler_ME_CLK_Check(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		return FALSE;
#else

#ifdef KID23_TO_WEBOS

	//rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);

//#ifdef CONFIG_MEMC_BYPASS
//	return FALSE;
//#else
	sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_REG;
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	//sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;

	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		return FALSE;
	}else{
		//MEMC in/out clock enable
		//sys_dispclksel_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
		//MEMC/me clock enable
		sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
		//MEMC/me clock reset
		sys_reg_sys_srst3_REG.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);

#if 1 //FIXME: remove CLKEN_DISP_MEMC_IN (0x18000208[6])
		if( (((sys_reg_sys_clken3_reg.regValue>>29)&0x7) != 7) || (((sys_reg_sys_srst3_REG.regValue>>29)&0x7) != 7) ){
			rtd_pr_memc_notice("[MEMC][%s] Warning !! CLK not enable!!( %d,%d )",__func__, ((sys_reg_sys_clken3_reg.regValue>>29)&0x7),((sys_reg_sys_srst3_REG.regValue>>29)&0x7));
			return FALSE;
		}
#endif
		{
			return TRUE;
		}
	}
#else
return 0;
#endif
#endif

}

/**
 * @brief This function enable the DispCLK of MEMC
*/
void Scaler_MEMC_CLK(void){
#if 1//#ifdef KID23_TO_WEBOS

	sys_reg_sys_dispclksel_RBUS sys_dispclksel_reg;

	//MEMC lantency temp setting
	//rtd_outl(MEMC_LATENCY01_ADDR, 0x00000000);
	//rtd_outl(MEMC_LATENCY02_ADDR, 0x00000000);

	//MEMC in/out clock enable
	sys_dispclksel_reg.regValue = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
#if 0 //FIXME: remove CLKEN_DISP_MEMC_IN (0x18000208[6])
	sys_dispclksel_reg.clken_disp_memc_in= 1;
#endif
	sys_dispclksel_reg.clken_disp_memc= 1;
	IoReg_Write32(SYS_REG_SYS_DISPCLKSEL_reg, sys_dispclksel_reg.regValue);
#endif

}

#ifndef BUILD_QUICK_SHOW

void Scaler_MEMC_Mux(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else

//#ifndef CONFIG_MEMC_BYPASS
	//MEMC mux
	ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
//#endif
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl_reg;

	double_buffer_ctrl_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl_reg.uzudtgreg_dbuf_en = 1;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl_reg.regValue);

//#ifndef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == FALSE || get_platform() == PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		//DTG MEMC MUX control
		memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);

		memc_mux_ctrl_reg.memc_outmux_sel = 1;
		//memc_mux_ctrl_reg.memcdtg_golden_vs = 0;//disable memc golden vsync
		IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_reg, memc_mux_ctrl_reg.regValue);
	}
//#endif
	//set apply
	double_buffer_ctrl_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl_reg.uzudtgreg_dbuf_set = 1;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl_reg.regValue);
#if 0
	if(vbe_disp_decide_memc_mux_bypass()==TRUE){
		//disable memc clk
		Scaler_MEMC_Bypass_CLK_OnOff(0);
		if(Scaler_VOFromVDEC(Scaler_Get_CurVoInfo_plane()) == SOURCE_VIDEO){//DTV case
			rtd_pr_memc_notice("DTV 4K, bypass PQ Lv2\n");
			Scaler_color_Set_PQ_ByPass_Lv(2);
		}else if(Scaler_VOFromVDEC(Scaler_Get_CurVoInfo_plane()) == SOURCE_DIRECTVO){
			rtd_pr_memc_notice("Playback 4K, bypass PQ Lv3\n");
			Scaler_color_Set_PQ_ByPass_Lv(3);
		}
	}else{
		Scaler_MEMC_Bypass_CLK_OnOff(1);
	}
#else
#if 0
	if(vbe_disp_decide_PQ_power_saveing() == TRUE){
		if(Scaler_VOFromVDEC(Scaler_Get_CurVoInfo_plane()) == SOURCE_VIDEO){//DTV case
			rtd_pr_memc_notice("DTV 4K, bypass PQ Lv2\n");
			Scaler_color_Set_PQ_ByPass_Lv(3);
		}else if(Scaler_VOFromVDEC(Scaler_Get_CurVoInfo_plane()) == SOURCE_DIRECTVO){
			rtd_pr_memc_notice("Playback 4K, bypass PQ Lv3\n");
			Scaler_color_Set_PQ_ByPass_Lv(3);
		}
	}
#endif
#endif
#endif
}

void Scaler_MEMC_outMux(UINT8 dbuf_en,UINT8 outmux_sel){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else

//#ifndef CONFIG_MEMC_BYPASS
	//MEMC mux
	ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
//#endif
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl_reg;

	double_buffer_ctrl_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl_reg.uzudtgreg_dbuf_en = dbuf_en;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl_reg.regValue);

//#ifndef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == FALSE || get_platform() == PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		//DTG MEMC MUX control
		memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);

		memc_mux_ctrl_reg.memc_outmux_sel = outmux_sel;
		//memc_mux_ctrl_reg.memcdtg_golden_vs = 0;//disable memc golden vsync
		IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_reg, memc_mux_ctrl_reg.regValue);
	}
//#endif
	if(dbuf_en == TRUE)
	{
		//set apply
		double_buffer_ctrl_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		double_buffer_ctrl_reg.uzudtgreg_dbuf_set = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl_reg.regValue);
	}
#endif
}


void Scaler_MEMC_HDMI_ME_CLK_OnOff(unsigned char is_clk_on, unsigned char mode)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	unsigned char force_MEMC_on_en = 0, MEMC_clock_state = 0;
	unsigned int time1 = 0;//, time2 = 0;
	//unsigned char wait_cnt = 0;
	//unsigned char timeout_flag = 0;

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return;
//#endif
#ifdef MEMC_BRING_UP
	return;
#endif

	force_MEMC_on_en = ((rtd_inl(SOFTWARE1_SOFTWARE1_63_reg)>>29)&0x1);
	MEMC_clock_state = (rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>31)&0x1;
	time1 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
	rtd_pr_memc_debug( "[MEMC_LibHDMIPowerSaving][Scaler_MEMC_HDMI_ME_CLK_OnOff][00][%d]\n", time1);

	if(MEMC_clock_state == 0){
		rtd_pr_memc_notice("[%s][MEMC_clock_state is off !!!]\n", __FUNCTION__);
		return;
	}

	if(force_MEMC_on_en){
		is_clk_on = 1;
	}

	if(is_clk_on == 0){
		rtd_pr_memc_debug( "[Scaler_MEMC_HDMI_ME_CLK_OnOff][01][%x]\n", rtd_inl(0xb800011c));
		rtd_pr_memc_notice("[MEMC_LibHDMIPowerSaving][Scaler_MEMC_HDMI_ME_CLK_OnOff][01][%x]\n", rtd_inl(0xb800011c));
		MEMC_Lib_set_memc_PowerSaving_Mode(2); // DTV PS flow would force cadence to video, but HDMI need to free the crtl
#if 1 //MEClkOff_MCClkOn
		//set IZ
		//rtd_setbits(MC__MC_28_ADDR, _BIT14); //0xb8099628
		if( mode == 0 ){
			if(HDMI_PowerSaving_stage == 0){
				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);

				HDMI_PowerSaving_stage = 1;
				rtd_pr_memc_notice("[MEMC_LibHDMIPowerSaving][Scaler_MEMC_HDMI_ME_CLK_OnOff][ME_CLK_off][%d][%x]\n", HDMI_PowerSaving_stage, rtd_inl(0xb809aea8));
			}else if(HDMI_PowerSaving_stage == 1){
				//disable me sram
				rtd_setbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
				rtd_setbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

				//disable more reg - 0514
				rtd_setbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
				rtd_setbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
				rtd_setbits(KME_BIST_dm_misc_reg, _BIT13);
				rtd_setbits(KME_BIST_dm_misc_reg, _BIT21);
				rtd_setbits(KME_BIST_dm_drf_fail0_reg, _BIT13);

				//ME CLK off
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
				IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
				IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);
				IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);

				HDMI_PowerSaving_stage = 0;
				rtd_pr_memc_notice("[MEMC_LibHDMIPowerSaving][Scaler_MEMC_HDMI_ME_CLK_OnOff][ME_CLK_off][%d][%x]\n", HDMI_PowerSaving_stage, rtd_inl(0xb809aea8));
			}
		}else{
			//set Repeat Mode
			Mid_Mode_SetMEMCFrameRepeatEnable(1);

			//disable me sram
			rtd_setbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
			rtd_setbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

			//disable more reg - 0514
			rtd_setbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
			rtd_setbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
			rtd_setbits(KME_BIST_dm_misc_reg, _BIT13);
			rtd_setbits(KME_BIST_dm_misc_reg, _BIT21);
			rtd_setbits(KME_BIST_dm_drf_fail0_reg, _BIT13);

			//ME CLK off
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
			IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);
			IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);
			HDMI_PowerSaving_stage = 0;
			rtd_pr_memc_notice("[MEMC_LibHDMIPowerSaving][Scaler_MEMC_HDMI_ME_CLK_OnOff][ME_CLK_off][DD][%x]\n", rtd_inl(0xb809aea8));
		}
#endif
	}
	else{
		rtd_pr_memc_debug( "[Scaler_MEMC_HDMI_ME_CLK_OnOff][02][%x]\n", rtd_inl(0xb800011c));
		rtd_pr_memc_notice("[MEMC_LibHDMIPowerSaving][Scaler_MEMC_HDMI_ME_CLK_OnOff][MEMC clk all on!][%x]\n", rtd_inl(0xb800011c));
		MEMC_Lib_set_memc_PowerSaving_Mode(0);
#if 1 //def MEClkOff_MCClkOn
		//set Repeat Mode
		//Mid_Mode_SetMEMCFrameRepeatEnable(1);

		//CLK on
		IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);//rtd_outl(0xb800010c, 0xc0000000);
		IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);//rtd_outl(0xb800011c, 0xc0000001);
		IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);//rtd_outl(0xb800011c, 0xc0000000);
		IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000001);//rtd_outl(0xb800010c, 0xc0000001);
		IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);//rtd_outl(0xb800011c, 0xc0000001);
		IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);//rtd_outl(0xb800010c, 0xc0000000);
		IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000005);//rtd_outl(0xb800011c, 0xc0000001);
		IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);//rtd_outl(0xb800011c, 0xc0000000);
		IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000005);//rtd_outl(0xb800010c, 0xc0000001);
		IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000005);//rtd_outl(0xb800011c, 0xc0000001);

		//enable me sram
		rtd_clearbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
		rtd_clearbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

		//enable more reg - 0514
		rtd_clearbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
		rtd_clearbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
		rtd_clearbits(KME_BIST_dm_misc_reg, _BIT13);
		rtd_clearbits(KME_BIST_dm_misc_reg, _BIT21);
		rtd_clearbits(KME_BIST_dm_drf_fail0_reg, _BIT13);

		//DMA on
		//MEMC_LibEnableDMA();
		//set Blending Mode
		Mid_Mode_SetMEMCFrameRepeatEnable(0);

		//MEMC mux enable
		drvif_memc_mux_en(TRUE);

		HDMI_PowerSaving_stage = 0;
#endif

	}
	rtd_pr_memc_notice("[MEMC_LibHDMIPowerSaving][Scaler_MEMC_HDMI_ME_CLK_OnOff][04][time = %d]\n",(rtd_inl(TIMER_SCPU_CLK90K_LO_reg)-time1)/90);
#endif	
#endif

}


int Scaler_MEMC_DTV_ME_CLK_OnOff(unsigned char enable, unsigned char bMode)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else

#if 0
	//unsigned int ip29;
	unsigned int time1 = 0;
	unsigned int time2 = 0;
	unsigned int time3 = 0;
	//unsigned char is_clk_on = 0;
	unsigned char do_power_saving = 0;
	unsigned char Me_clk_state;
	unsigned int wait_cnt = 0;
	unsigned int crtc1_frc_hact_state = 0;
	unsigned int MEMC_drvif_mux_state = 0;
	unsigned char timeout_flag = 0;
	unsigned char MC_sram_state = 0;

#ifdef CONFIG_MEMC_BYPASS
	return 0;
#endif
#ifdef MEMC_BRING_UP
	return 0;
#endif

	time1 = rtd_inl(0xB801B6B8);

#if 1 // MEMC CLK OFF handler
	Me_clk_state = ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x1);
	crtc1_frc_hact_state = rtd_inl(CRTC1_CRTC1_14_reg)&0xfff;
	MEMC_drvif_mux_state = rtd_inl(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg)&0x1;
	MC_sram_state = (rtd_inl(MC_MC_34_reg)>>28)&0x1;
	rtd_pr_memc_notice("[%s] state=[,%d,%d,%d][,%x,]\n\r", __FUNCTION__, Me_clk_state, crtc1_frc_hact_state, MEMC_drvif_mux_state, rtd_inl(0xB80282e4));
#if 0
	if(pre_clk_state == enable){ // sleep when buffer empty
		//rtd_pr_memc_notice("[%s] memcClk/enable=%d/%d\n", __FUNCTION__, IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>31, enable);
		rtd_pr_memc_notice("[%s] memcClk/enable=%d/%d\n", __FUNCTION__, ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x3), enable);
		return 0;
	}
#else
	if(enable){ // sleep when buffer empty
		if((Me_clk_state == 0) || (crtc1_frc_hact_state == 0) || (MEMC_drvif_mux_state == 0) || (MC_sram_state == 1)){
			do_power_saving = 0;
		}else{
			rtd_pr_memc_notice("[%s] memcClk/enable=%d/%d\n", __FUNCTION__, ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x1), enable);
			return 0;
		}
	}else{
		if(Me_clk_state > 0){
			do_power_saving = 1;
		}else{
			rtd_pr_memc_notice("[%s] memcClk/enable=%d/%d\n", __FUNCTION__, ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x1), enable);
			return 0;
		}
	}
#endif

	rtd_pr_memc_notice("[%s] MUTEX DOWN@%x\n", __FUNCTION__, rtd_inl(0xB801B6B8));
	time3 = rtd_inl(0xB801B6B8);
	down(&MEMC_CLK_Semaphore);
		if(do_power_saving == 1){ //turn off clk
			//mute
			if(bMode == 1){
				MEMC_LibBlueScreen(1);

				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);

				rtd_setbits(MC_MC_34_reg, _BIT28); //rtd_setbits(0xb8099634, _BIT28); //[28], MC_SRAM sleep
				rtd_outl(MC_MC_D8_reg, 0x80000000); //rtd_outl(0xb80996d8, 0x80000000); //force mv = 0

				//CLK off
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
				IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);

				rtd_pr_memc_notice("[Scaler_MEMC_DTV_ME_CLK_OnOff_01][AAA][%d][%d,%d][%x]\n", (rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>12)&0x1, wait_cnt, (rtd_inl(0xB801B6B8)-time2), rtd_inl(0xb809E060));
			}else{
					/**** move to Patch_Manage.c ****
					if(DTV_PowerSaving_stage == 0){
						//set IZ
						rtd_setbits(MC__MC_28_ADDR, _BIT14); //0xb8099628
						if(MEMC_LibGetMEMC_PCModeEnable()){
							Mid_Mode_SetMCBlend(RGB_IN_PC_OUT);
						}else{
							MEMC_LibSetMCBlend(1);
						}
						DTV_PowerSaving_stage = 1;
						rtd_pr_memc_notice("[Scaler_MEMC_DTV_ME_CLK_OnOff][ME_CLK_off][AA][%x]\n", rtd_inl(0xb809aea8));
					}else if(DTV_PowerSaving_stage == 2){
						//DMA off
						rtd_setbits(0xb809aea8, _BIT0);
						MEMC_LibDisableMEDMA();
						rtd_setbits(0xb809aea8, _BIT2);

						DTV_PowerSaving_stage = 3;
						rtd_pr_memc_notice("[Scaler_MEMC_DTV_ME_CLK_OnOff][ME_CLK_off][BB][%x]\n", rtd_inl(0xb809aea8));
					}else if(DTV_PowerSaving_stage == 3){
						rtd_clearbits(0xb809aea8, _BIT0);

						rtd_outl(MC_MC_D8_reg, 0x80000000); //rtd_outl(0xb80996d8, 0x80000000); //force mv = 0
						rtd_setbits(MC_MC_34_reg, _BIT28); //rtd_setbits(0xb8099634, _BIT28); //[28], MC_SRAM sleep

						//ME CLK off
						IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
						IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
						DTV_PowerSaving_stage = 0;
						rtd_pr_memc_notice("[Scaler_MEMC_DTV_ME_CLK_OnOff][ME_CLK_off][CC][%x]\n", rtd_inl(0xb809aea8));
					}
					*******************************/
				//wait ME CLK off
				DTV_PowerSaving_stage = 1;
				MEMC_Lib_set_memc_freeze_status_flag(1);
				do{
					if(MEMC_Lib_get_memc_freeze_status_flag()==1){
						if((rtd_inl(0xB801B6B8)-time3)>=7200){
							timeout_flag = 1;
						}
					}
				}while((timeout_flag == 0) && (MEMC_Lib_get_memc_freeze_status_flag() == 1));
				rtd_pr_memc_notice("[Scaler_MEMC_DTV_ME_CLK_OnOff_01][BBB][%d][%d,%d][%d][%d]\n", (rtd_inl(0xb800011c)>>30)&0x1, timeout_flag, (rtd_inl(0xB801B6B8)-time3),MEMC_Lib_get_memc_freeze_status_flag(), rtd_inl(0xB801B6B8));
			}
		}
		else{ //turn on clk
			//mute
			if(bMode)
				MEMC_LibBlueScreen(1);

			rtd_pr_memc_debug( "[Scaler_MEMC_DTV_ME_CLK_OnOff_02][%d]\n", (rtd_inl(0xb800011c)>>29));

			//CLK on
			rtd_pr_memc_debug( "[Scaler_MEMC_DTV_ME_CLK_OnOff_03][%d]\n", g_MEMC_DMA_state);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);//rtd_outl(0xb800010c, 0xc0000000);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);//rtd_outl(0xb800011c, 0xc0000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);//rtd_outl(0xb800011c, 0xc0000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000001);//rtd_outl(0xb800010c, 0xc0000001);
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);//rtd_outl(0xb800011c, 0xc0000001);

			rtd_clearbits(MC_MC_34_reg, _BIT28); //rtd_setbits(0xb8099634, _BIT28); //[28], MC_SRAM sleep
			rtd_outl(MC_MC_D8_reg, 0x00000000); //rtd_outl(0xb80996d8, 0x80000000); //force mv = 0

			//DMA on
			MEMC_LibEnableDMA();

			//MEMC mux enable
			drvif_memc_mux_en(TRUE);

			//if(!bMute)
			//	msleep(20);

			//enable memc interrupt
			IoReg_Write32(0xb8099018, 0x0200403f);
			IoReg_Write32(0xb809d008, 0x00000040);

			//set blending
			if(MEMC_LibGetMEMC_PCModeEnable() || VR360_en == 1 || (u8_MEMCMode == 0) || (u8_MEMCMode == 4 && dejudder == 0)){
				Mid_Mode_SetMEMCFrameRepeatEnable(1);
			}else{
				Mid_Mode_SetMEMCFrameRepeatEnable(0);
			}
			rtd_pr_memc_notice("[Scaler_MEMC_DTV_ME_CLK_OnOff_01][CCC][%d][%d,%d][%x]\n", (rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>12)&0x1, wait_cnt, (rtd_inl(0xB801B6B8)-time3),MEMC_Lib_get_memc_freeze_status_flag());
		}
		//pre_clk_state = enable;
		up(&MEMC_CLK_Semaphore);
		rtd_pr_memc_notice("[%s] MUTEX UP@%x\n", __FUNCTION__, rtd_inl(0xB801B6B8));
#endif
	timeout_flag = 0;
	rtd_pr_memc_notice("[Scaler_MEMC_DTV_ME_CLK_OnOff_04][time = %d][%d,%d][%d]\n", (rtd_inl(0xB801B6B8)-time1)/90, enable, bMode, ((rtd_inl(0xb800011c)>>30)&0x1));

#endif
	return 0;
#endif
}

void Scaler_MEMC_HDMI_Power_Ctrl(unsigned char is_hdmi)
{
	rtd_pr_memc_notice("[HDMI_PS][Scaler_MEMC_HDMI_Power_Ctrl]\n\r");
	//MEMC_LibPowerSaveing(is_hdmi);
}

void Scaler_MEMC_Set_Clock_ForceExit(void)
{
	rtd_pr_memc_notice("[Scaler_MEMC_Set_Clock_ForceExit] MEMC CLK CTRL TASK FORCE EXIT\n\r");
	bMemcClkTask_ForceExitFlag = 1;
	return;
}


void Scaler_MEMC_Set_Clock_Enable(unsigned char enable, unsigned char bMute)
{
	rtd_pr_memc_notice("[Scaler_MEMC_Set_Clock_Enable]memcClkEn=%d->%d\n\r", bMemcClkTask_memcClkEn,  enable);

	rtd_pr_memc_notice("[%s] MUTEX DOWN@%x\n", __FUNCTION__, rtd_inl(0xB801B6B8));
	down(&MEMC_CLK_Semaphore);
	bMemcClkTask_memcClkEn= enable;
	bMemcClkTask_memcMuteFlag = bMute;
	up(&MEMC_CLK_Semaphore);
	rtd_pr_memc_notice("[%s] MUTEX UP@%x\n", __FUNCTION__, rtd_inl(0xB801B6B8));

	return;
}

int Scaler_MEMC_Get_Clock_Enable(unsigned char *enable, unsigned char *bMute)
{
	if(!enable || !bMute){
		rtd_pr_memc_notice("[ERR] NULL PTR@%s\n", __FUNCTION__);
		return -1;
	}

	*enable = bMemcClkTask_memcClkEn;
	*bMute = bMemcClkTask_memcMuteFlag;
	return 0;
}
#endif

#if 1//#if RTK_MEMC_Performance_tunging_from_tv001//k23
void Scaler_MEMC_Set_DISP_DHTOTAL(unsigned int DHTotal)
{
	rtd_pr_memc_emerg("[%s] DHTotal =%d \n", __FUNCTION__, DHTotal);
	DISP_DHTotal= DHTotal;

	return;
}

extern unsigned int Get_DISP_HORIZONTAL_TOTAL(void);
unsigned int Scaler_MEMC_Get_DISP_DHTOTAL(void)
{
	//rtd_pr_memc_emerg("[%s] DISP_DHTotal =%d \n", __FUNCTION__, DISP_DHTotal);
	return Get_DISP_HORIZONTAL_TOTAL(); // DISP_DHTotal;
}
#endif




extern int vbe_dtg_memec_bypass_switch(void);

#if 1//#if RTK_MEMC_Performance_tunging_from_tv001 //k24
extern unsigned char g_MEMC_powersaving_on;
#endif

int Scaler_MEMC_MEMC_CLK_OnOff(unsigned char ME_enable, unsigned char MC_enable, unsigned char bMute)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else

#if 1//#ifdef KID23_TO_WEBOS


	//unsigned int ip29;
	unsigned int time1 = 0;
	unsigned int time2 = 0;
	unsigned int time3 = 0;
	//unsigned char is_clk_on = 0;
	unsigned char do_power_saving = 0;
	unsigned char Me_clk_state;
	unsigned char Mc_clk_state;
	unsigned char Mc_func_clk_state = 0;
	unsigned int wait_cnt = 0;
	unsigned int crtc1_frc_hact_state = 0;
	unsigned int MEMC_drvif_mux_state = 0;
	unsigned char timeout_flag = 0;
	unsigned char MC_sram_sleep_state = 0;
	unsigned char MC_DMA_read_i_state = 0;
	unsigned int force_MEMC_on_en = 0;

	//#ifdef CONFIG_MEMC_BYPASS
		//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
		if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
			return 0;
	//#endif

#if 1//#if RTK_MEMC_Performance_tunging_from_tv001 //k24
	if(g_MEMC_powersaving_on == 2){
		// IoReg_Mask32(0xb802847c, 0xfffbffff, 0x0); // For test, Disable double buffer
		// IoReg_Mask32(0xb8028100, 0xffffdfff, 0x0); // For test, MEMC bypass
		ME_enable=0;
		MC_enable=0;
		rtd_pr_memc_emerg("VPQ MEMC thermal protect, keep ME=0, MC=0.\n");
	}else if(g_MEMC_powersaving_on == 1){
		ME_enable=0;
		MC_enable=1;
		rtd_pr_memc_emerg("VPQ MEMC thermal protect, keep ME=0, MC=1.\n");
	} 
#endif


#ifdef MEMC_BRING_UP
	return 0;
#endif

	if(Get_DISPLAY_PANEL_TYPE()== P_VBY1_TO_HDMI_MEMC_FREERUN){
		ME_enable = 0;
		MC_enable = 1;	
		rtd_pr_memc_emerg("P_VBY1_TO_HDMI_MEMC_FREERUN, keep me=0, mc=1.\n");
	}
	
	if(get_product_type() == PRODUCT_TYPE_DIAS && ME_enable == 1){
		ME_enable = 0;
		rtd_pr_memc_emerg("PRODUCT_TYPE_DIAS, keep me=0\n");
	}

#ifdef RTK_MT_FLOW_ENABLE
	ME_enable = 1 ;
	MC_enable = 1 ;
	rtd_pr_memc_emerg("PRODUCT_TYPE_MT, keep me=1, mc=1.\n");
#endif

	time1 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);


#if 1 // MEMC CLK OFF handler
	Me_clk_state = ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x1);
	Mc_clk_state = ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>31)&0x1);
	Mc_func_clk_state = ((IoReg_Read32(SYS_REG_SYS_CLKEN4_reg)>>2)&0x1);
	crtc1_frc_hact_state = rtd_inl(CRTC1_CRTC1_14_reg)&0xfff;
	MEMC_drvif_mux_state = rtd_inl(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg)&0x1;
	MC_sram_sleep_state = (rtd_inl(MC_MC_34_reg)>>28)&0x1;
	MC_DMA_read_i_state = rtd_inl(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg)&0x1;
	force_MEMC_on_en = ((rtd_inl(SOFTWARE1_SOFTWARE1_63_reg)>>29)&0x1);
	rtd_pr_memc_notice("[%s] state=[,%d,%d,%d,%d][,%x,][%d][%d,%d,%d]\n\r", __FUNCTION__, Mc_clk_state, Me_clk_state, crtc1_frc_hact_state, MEMC_drvif_mux_state, rtd_inl(0xB80282e4),MEMC_Lib_get_memc_PowerSaving_Mode(), ME_enable, MC_enable,bMute);


#if 0//for se_film test
	ME_enable = 1 ;
	MC_enable = 1 ;
	rtd_pr_memc_emerg("PRODUCT_TYPE_MT, keep me=1, mc=1.\n");
#endif


#if 0
	if(pre_clk_state == enable){ // sleep when buffer empty
		//rtd_pr_memc_notice("[%s] memcClk/enable=%d/%d\n", __FUNCTION__, IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>31, enable);
		rtd_pr_memc_notice("[%s] memcClk/enable=%d/%d\n", __FUNCTION__, ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x3), enable);
		return 0;
	}
#else
	if((VR360_en == 1) && (ME_enable || MC_enable))
	{
		rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff] VR360 is on, memory is used. MEMC CLK cannot be enabled. %d, %d, %d\n", VR360_en, ME_enable, MC_enable);
		return 0;
	}
	else if(ME_enable == 1){ // sleep when buffer empty
		MEMC_Lib_set_memc_PowerSaving_Mode(0);
		if((Me_clk_state == 0) || (Mc_clk_state == 0) || (crtc1_frc_hact_state == 0) || (MEMC_drvif_mux_state == 0) || (MC_sram_sleep_state == 1)){
			do_power_saving = 0;  //turn on all clk
		}else{
			rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff][AA][clock status doesn't change, will not mute !!][MEMC_Clk=%d][enable=%d/%d]\n", ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x3), ME_enable, MC_enable);
			return 0;
		}
	}else if(MC_enable == 1){
		MEMC_Lib_set_memc_PowerSaving_Mode(2);
		if((Me_clk_state == 1) || (Mc_clk_state == 0) || (crtc1_frc_hact_state == 0) || (MEMC_drvif_mux_state == 0) || (MC_sram_sleep_state == 0) || (VR360_stage != 0) || (VR360_en == 0 && MC_DMA_read_i_state == 0)){
			do_power_saving = 2;  //turn on MC clk
		}else{
			rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff][BB][clock status doesn't change, will not mute !!][MEMC_Clk=%d][enable=%d/%d]\n", ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x3), ME_enable, MC_enable);
			return 0;
		}
	}else{
		MEMC_Lib_set_memc_PowerSaving_Mode(1);
		if((Me_clk_state == 1) || (Mc_clk_state == 1)){
			do_power_saving = 1;  //turn off all clk
		}else{
			rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff][CC][clock status doesn't change, will not mute !!][MEMC_Clk=%d][enable=%d/%d]\n", ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>30)&0x3), ME_enable, MC_enable);
			return 0;
		}
	}
#endif

	if(force_MEMC_on_en){
		do_power_saving = 0;
		MEMC_Lib_set_memc_PowerSaving_Mode(0);
	}

	rtd_pr_memc_notice("[%s] MUTEX DOWN@%x, %d\n", __FUNCTION__, rtd_inl(0xB801B6B8),do_power_saving);
	down(&MEMC_CLK_Semaphore);
		if(do_power_saving == 2){  //MC clk on, ME clk off
			g_MEMC_DMA_state = 0;

			if(bMute == 1){
				if(Mc_clk_state == 0){
#if RTK_MEMC_Performance_tunging_from_tv001
					Scaler_MEMC_Set_BlueScreen(1);
#else
					MEMC_LibBlueScreen(1);
#endif
					
					rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff][MEMC off to MC only][Mute !!]");
				}
				else{
#if RTK_MEMC_Performance_tunging_from_tv001
					//					if(Get_DISPLAY_PANEL_TYPE()== P_VBY1_TO_HDMI_MEMC_FREERUN){
					//						Scaler_MEMC_Set_BlueScreen(1);
					//					}	

#else
					//					if(Get_DISPLAY_PANEL_TYPE()== P_VBY1_TO_HDMI_MEMC_FREERUN){
					//						MEMC_LibBlueScreen(1);
					//					}
#endif

					rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff][MEMC on to MC only][No Mute !!]");
				}

				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);

				if(Mc_clk_state == 0){
					IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000000);//rtd_outl(0xb800010c, 0xc0000000);
					IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);//rtd_outl(0xb800011c, 0xc0000001);
					IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000000);//rtd_outl(0xb800011c, 0xc0000000);
					IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000001);//rtd_outl(0xb800010c, 0xc0000001);
					IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);//rtd_outl(0xb800011c, 0xc0000001);
				}

				//enable Arb_en
				if((rtd_inl(DBUS_WRAPPER_MEMC_DCU1_ARB_CR1_reg)&0xfff)!=0xfff){
					rtd_outl(DBUS_WRAPPER_MEMC_DCU1_ARB_CR1_reg, 0xfff);
				}

				//MEMC mux enable
				if(MEMC_drvif_mux_state == 0){
					drvif_memc_mux_en(TRUE);
				}

				if(crtc1_frc_hact_state == 0){
					IoReg_Mask32(KME_BIST_MEMC_clk_sub_en_reg, ~0x0000001f, 0x1f);				//	rtd_outl(0xb809f004, tmp_val);
					IoReg_Mask32(CRTC1_CRTC1_14_reg, ~0x00000fff, 0x3c0);	//	rtd_outl(CRTC1__CRTC1_14_ADDR, tmp_crtc1_frc_hact);
					IoReg_Mask32(CRTC1_CRTC1_44_reg, ~0x00001fff, 0x438);	//	rtd_outl(CRTC1__CRTC1_44_ADDR, tmp_crtc1_mc_vact);
				}

				//disable me sram
				rtd_setbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
				rtd_setbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

				//disable more reg - 0514
				rtd_setbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
				rtd_setbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
				rtd_setbits(KME_BIST_dm_misc_reg, _BIT13);
				rtd_setbits(KME_BIST_dm_misc_reg, _BIT21);
				rtd_setbits(KME_BIST_dm_drf_fail0_reg, _BIT13);

				//ME CLK off
				if(Me_clk_state == 1){
					IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
					IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
				}

				//MC func CLK off
				if(Mc_func_clk_state == 1){
					IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);
					IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);
				}

				rtd_pr_memc_notice("[%s][%d][%d][%d][%x]\n", __FUNCTION__, __LINE__, (rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>12)&0x1, (rtd_inl(0xB801B6B8)-time2), rtd_inl(0xb809E060));
			}
			else{
				//set Repeat Mode
				Mid_Mode_SetMEMCFrameRepeatEnable(1);

				if(Mc_clk_state == 0){
					rtd_pr_memc_debug( "[Scaler_MEMC_MEMC_CLK_MCOn_MEOff][BB][%d]\n", g_MEMC_DMA_state);
					IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000000);//rtd_outl(0xb800010c, 0xc0000000);
					IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);//rtd_outl(0xb800011c, 0xc0000001);
					IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000000);//rtd_outl(0xb800011c, 0xc0000000);
					IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000001);//rtd_outl(0xb800010c, 0xc0000001);
					IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);//rtd_outl(0xb800011c, 0xc0000001);
				}

				//enable Arb_en
				if((rtd_inl(DBUS_WRAPPER_MEMC_DCU1_ARB_CR1_reg)&0xfff)!=0xfff){
					rtd_outl(DBUS_WRAPPER_MEMC_DCU1_ARB_CR1_reg, 0xfff);
				}

				//MEMC mux enable
				if(MEMC_drvif_mux_state == 0){
					drvif_memc_mux_en(TRUE);
				}

				if(crtc1_frc_hact_state == 0){
					IoReg_Mask32(KME_BIST_MEMC_clk_sub_en_reg, ~0x0000001f, 0x1f);//				rtd_outl(0xb809f004, tmp_val);
					IoReg_Mask32(CRTC1_CRTC1_14_reg, ~0x00000fff, 0x3c0);//				rtd_outl(CRTC1__CRTC1_14_ADDR, tmp_crtc1_frc_hact);
					IoReg_Mask32(CRTC1_CRTC1_44_reg, ~0x00001fff, 0x438);//				rtd_outl(CRTC1__CRTC1_44_ADDR, tmp_crtc1_mc_vact);
				}

				//disable me sram
				rtd_setbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
				rtd_setbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

				//disable more reg - 0514
				rtd_setbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
				rtd_setbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
				rtd_setbits(KME_BIST_dm_misc_reg, _BIT13);
				rtd_setbits(KME_BIST_dm_misc_reg, _BIT21);
				rtd_setbits(KME_BIST_dm_drf_fail0_reg, _BIT13);

				//wait ME CLK off
				//**** move to Patch_Manage.c ****//
				DTV_PowerSaving_stage = 1;
#if RTK_MEMC_Performance_tunging_from_tv001
				Scaler_MEMC_Set_MEMC_FreezeStatus_Flag(1);
#else
				MEMC_Lib_set_memc_freeze_status_flag(1);
#endif
				time3 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

#if RTK_MEMC_Performance_tunging_from_tv001
				do{
					if(Scaler_MEMC_Get_MEMC_FreezeStatus_Flag()==1){
						if((rtd_inl(TIMER_SCPU_CLK90K_LO_reg)-time3)>=7500){
							timeout_flag = 1;
						}
					}
				}while((timeout_flag == 0) && (Scaler_MEMC_Get_MEMC_FreezeStatus_Flag() == 1));
#else
				do{
					if(MEMC_Lib_get_memc_freeze_status_flag()==1){
						if((rtd_inl(0xB801B6B8)-time3)>=7500){
							timeout_flag = 1;
						}
					}
				}while((timeout_flag == 0) && (MEMC_Lib_get_memc_freeze_status_flag() == 1));	
#endif			
				rtd_pr_memc_notice("[%s][%d][%d][%d,%d][%d][%d]\n", __FUNCTION__, __LINE__, (rtd_inl(0xb800011c)>>30)&0x1, timeout_flag, (rtd_inl(0xB801B6B8)-time3),MEMC_Lib_get_memc_freeze_status_flag(), rtd_inl(0xB801B6B8));
			}
		}
		else if(do_power_saving == 1){  //turn off all clk
			g_MEMC_DMA_state = 0;

			//set Repeat Mode
			Mid_Mode_SetMEMCFrameRepeatEnable(1);

			//MEMC mux disable
			drvif_memc_mux_en(FALSE);

			//disable me sram
			rtd_setbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
			rtd_setbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

			//disable more reg - 0514
			rtd_setbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
			rtd_setbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
			rtd_setbits(KME_BIST_dm_misc_reg, _BIT13);
			rtd_setbits(KME_BIST_dm_misc_reg, _BIT21);
			rtd_setbits(KME_BIST_dm_drf_fail0_reg, _BIT13);

			//DMA off
			#if 1
				MEMC_Lib_MEDMA_Control(0,0,0,1);
				MEMC_Lib_MCDMA_Control(0,0,1);
			#else
			MEMC_LibDisableDMA();
			#endif

			//wait
			time2 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			wait_cnt = 0;
#if 0
			msleep(42);
#else
			rtd_pr_memc_notice("[%s][%d][%d][%d,%d][clock,%x,%x,]\n", __FUNCTION__, __LINE__, (rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>12)&0x1, wait_cnt, (rtd_inl(0xB801B6B8)-time2), IoReg_Read32(SYS_REG_SYS_CLKEN3_reg), IoReg_Read32(SYS_REG_SYS_SRST3_reg));
			rtd_outl(DBUS_WRAPPER_MEMC_DCU1_ARB_CR1_reg, 0x0);
			while(rtd_inl(DBUS_WRAPPER_MEMC_DCU1_ARB_SR0_reg) != 0){
				hw_msleep(1);
				if(wait_cnt++>30)
					break;
			}
			rtd_pr_memc_notice("[%s][%d][%d][%d,%d]\n", __FUNCTION__, __LINE__, (rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>12)&0x1, wait_cnt, (rtd_inl(0xB801B6B8)-time2));

#endif
			//CLK off
			IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0xc0000000);
			IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0xc0000000);
			IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);
			IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);
		}
		else if(do_power_saving == 0){  //turn on all clk

			//mute
			if(bMute){
				if(Mc_clk_state == 0){
#if RTK_MEMC_Performance_tunging_from_tv001
					Scaler_MEMC_Set_BlueScreen(1);
#else
					MEMC_LibBlueScreen(1);
#endif					
					
					rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff][MEMC off to MEMC on][Mute !!]");
				}
				else{
					rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff][MC only to MEMC on][No Mute !!]");
				}
			}
			//else{//Only Mute in "MEMC off to MEMC on" case, other case no need to mute}

			g_MEMC_DMA_state = 1;
			rtd_pr_memc_debug( "[Scaler_MEMC_MEMC_CLK_On_01][%d]\n", (rtd_inl(0xb800011c)>>29));
			rtd_pr_memc_debug( "[resume_GGG][%x, %x, %x, %x, %x]\n\r", rtd_inl(0xb8099414), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(0xb809d7fc));

			//CLK on
			if(Mc_clk_state == 0){
				rtd_pr_memc_debug( "[%s][%d][%d]\n", __FUNCTION__, __LINE__, g_MEMC_DMA_state);
				IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000000);//rtd_outl(0xb800010c, 0xc0000000);
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);//rtd_outl(0xb800011c, 0xc0000001);
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000000);//rtd_outl(0xb800011c, 0xc0000000);
				IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x80000001);//rtd_outl(0xb800010c, 0xc0000001);
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x80000001);//rtd_outl(0xb800011c, 0xc0000001);
			}
			if(Me_clk_state == 0){
				rtd_pr_memc_debug( "[%s][%d][%d]\n", __FUNCTION__, __LINE__, g_MEMC_DMA_state);
				IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);//rtd_outl(0xb800010c, 0xc0000000);
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);//rtd_outl(0xb800011c, 0xc0000001);
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);//rtd_outl(0xb800011c, 0xc0000000);
				IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000001);//rtd_outl(0xb800010c, 0xc0000001);
				IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000001);//rtd_outl(0xb800011c, 0xc0000001);
			}
			if(Mc_func_clk_state == 0){
				rtd_pr_memc_debug( "[%s][%d][%d]\n", __FUNCTION__, __LINE__, g_MEMC_DMA_state);
				IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000004);//rtd_outl(0xb800010c, 0xc0000000);
				IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000005);//rtd_outl(0xb800011c, 0xc0000001);
				IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000004);//rtd_outl(0xb800011c, 0xc0000000);
				IoReg_Write32(SYS_REG_SYS_SRST4_reg, 0x00000005);//rtd_outl(0xb800010c, 0xc0000001);
				IoReg_Write32(SYS_REG_SYS_CLKEN4_reg, 0x00000005);//rtd_outl(0xb800011c, 0xc0000001);
			}

			//enable Arb_en
			if((rtd_inl(DBUS_WRAPPER_MEMC_DCU1_ARB_CR1_reg)&0xfff)!=0xfff){
				rtd_outl(DBUS_WRAPPER_MEMC_DCU1_ARB_CR1_reg, 0xfff);
			}

			//enable me sram
			rtd_clearbits(KME_LBME_TOP_KME_LBME_TOP_04_reg, _BIT23);
			rtd_clearbits(KME_ME2_VBUF_TOP_KME_ME2_VBUF_TOP_18_reg, _BIT22);

			//enable more reg - 0514
			rtd_clearbits(KME_DEHALO_KME_DEHALO_10_reg, _BIT5);
			rtd_clearbits(KME_TOP_KME_TOP_9C_DH_OCCL_EXTEND_MBIST_RW_reg, _BIT5);
			rtd_clearbits(KME_BIST_dm_misc_reg, _BIT13);
			rtd_clearbits(KME_BIST_dm_misc_reg, _BIT21);
			rtd_clearbits(KME_BIST_dm_drf_fail0_reg, _BIT13);

//			//DMA on
//			MEMC_LibEnableDMA();

			//MEMC mux enable
			if(MEMC_drvif_mux_state == 0){
				drvif_memc_mux_en(TRUE);
			}

			if(crtc1_frc_hact_state == 0){
				IoReg_Mask32(KME_BIST_MEMC_clk_sub_en_reg, ~0x0000001f, 0x1f);//				rtd_outl(0xb809f004, tmp_val);
				IoReg_Mask32(CRTC1_CRTC1_14_reg, ~0x00000fff, 0x3c0);//				rtd_outl(CRTC1__CRTC1_14_ADDR, tmp_crtc1_frc_hact);
				IoReg_Mask32(CRTC1_CRTC1_44_reg, ~0x00001fff, 0x438);//				rtd_outl(CRTC1__CRTC1_44_ADDR, tmp_crtc1_mc_vact);
			}

			//enable memc interrupt
			if(Mid_MISC_GetInINTEnable(INT_TYPE_VSYNC) == 0){
				MEMC_LibInputVSInterruptEnable(TRUE); // rtd_setbits(KMC_TOP_kmc_top_18_reg, _BIT25);
			}
			if(Mid_MISC_GetOutINTEnable(INT_TYPE_HSYNC) == 0){
				MEMC_LibOutpuHSInterruptEnable(TRUE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT5);
			}
			if(Mid_MISC_GetOutINTEnable(INT_TYPE_VSYNC) == 0){
				MEMC_LibOutputVSInterruptEnable(TRUE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT6);
			}

			//set blending
			if(bMute)
			{
				if(MEMC_LibGetMEMC_PCModeEnable() || VR360_en == 1 || (u8_MEMCMode == 0) || (u8_MEMCMode == 4 && dejudder == 0)){
					Mid_Mode_SetMEMCFrameRepeatEnable(1);
				}else{
					Mid_Mode_SetMEMCFrameRepeatEnable(0);
				}
			}
			
			rtd_pr_memc_notice("[%s][%d][%d][%d,%d][%x]\n", __FUNCTION__, __LINE__, (rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg)>>12)&0x1, wait_cnt, (rtd_inl(0xB801B6B8)-time3),MEMC_Lib_get_memc_freeze_status_flag());
		}
		
#if RTK_MEMC_Performance_tunging_from_tv001
		rtd_clearbits(KME_BIST_dm_misc_reg, _BIT13);
		rtd_clearbits(KME_BIST_dm_misc_reg, _BIT21);
#endif
		//pre_clk_state = enable;

		up(&MEMC_CLK_Semaphore);

		rtd_pr_memc_notice("[%s] MUTEX UP@%x\n", __FUNCTION__, rtd_inl(0xB801B6B8));
#endif
	rtd_pr_memc_notice("[Scaler_MEMC_MEMC_CLK_OnOff_04][time = %d][,%d,%d,%d,][%d][%d]\n", (rtd_inl(0xB801B6B8)-time1)/90, ME_enable, MC_enable, bMute, ((rtd_inl(0xb8028100)>>13)&0x1),MEMC_Lib_get_memc_PowerSaving_Mode());

#endif

// quick show close me clock
#ifdef BUILD_QUICK_SHOW
	IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
	IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
#else
	if(g_KERNEL_AFTER_QS_MEMC_FLG){
		IoReg_Write32(SYS_REG_SYS_CLKEN3_reg, 0x40000000);
		IoReg_Write32(SYS_REG_SYS_SRST3_reg, 0x40000000);
	}
#endif

	return 0;
#endif	
}

#define MEMC_BYPASS_VO_VFREQ_50HZ 0x89544	//48Hz
#define MEMC_BYPASS_VO_VFREQ_60HZ 0x77D9D	//55Hz
void Scaler_adjust_VO_framerate_for_MEMC_bypass(unsigned char b_enable){
	vodma_vodma_pvs_free_RBUS vodma_vodma_pvs_free_reg;
	unsigned int timeout = 10;
	static unsigned int pvs0_free_period_pre = 0;
	//unsigned int t1=0, t2=0;
	vodma_vodma_pvs_free_reg.regValue = IoReg_Read32(VODMA_VODMA_PVS_Free_reg);
	if(b_enable){
		pvs0_free_period_pre = vodma_vodma_pvs_free_reg.pvs0_free_period;
		if( ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) >= V_FREQ_50000_mOFFSET ) && ( Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000) < V_FREQ_50000_pOFFSET ) )	// 50Hz
		{
			//adjust VO freerun period to 48Hz
			vodma_vodma_pvs_free_reg.pvs0_free_period = MEMC_BYPASS_VO_VFREQ_50HZ;
		}
		else{//60Hz
			//adjust VO freerun period to 55Hz
			vodma_vodma_pvs_free_reg.pvs0_free_period = MEMC_BYPASS_VO_VFREQ_60HZ;
		}
	}
	else{
		//recover VO freerun period
		vodma_vodma_pvs_free_reg.pvs0_free_period = pvs0_free_period_pre;
	}
	vodma_vodma_pvs_free_reg.pvs0_free_period_update = 1;
	IoReg_Write32(VODMA_VODMA_PVS_Free_reg, vodma_vodma_pvs_free_reg.regValue);
	//t1 = IoReg_Read32(0xb801b6d0);
	while((--timeout)&&(vodma_vodma_pvs_free_reg.pvs0_free_period_update == 1)){
		msleep(0);
		vodma_vodma_pvs_free_reg.regValue = IoReg_Read32(VODMA_VODMA_PVS_Free_reg);
	}
	//wait 1 frame to make VO freerun period apply
	Scaler_wait_for_input_one_frame(DTG_MASTER, SLR_MAIN_DISPLAY);
	//t2 = IoReg_Read32(0xb801b6d0);
	//rtd_pr_memc_emerg("[%s] pre_90k = (%x), after_90k (%x), t=%d\n", __FUNCTION__, t1, t2,(t2-t1)/90);
	rtd_pr_memc_emerg("[%s] VO free_period (%x)\n", __FUNCTION__, IoReg_Read32(VODMA_VODMA_PVS_Free_reg));
}

extern UINT8 TV006_WB_Pattern_En;
void Scaler_MEMC_Bypass_On(unsigned char b_isBypass){
//#ifndef CONFIG_MEMC_BYPASS
	ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
	//unsigned int count = 100;
	unsigned char needUpdate = 0;

	//if(get_MEMC_bypass_status_refer_platform_model() == FALSE || get_platform() == PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE){
		ppoverlay_double_buffer_ctrl2_RBUS ppoverlay_double_buffer_ctrl2_reg;
		ppoverlay_double_buffer_ctrl2_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);

		rtd_pr_memc_emerg("Scaler_MEMC_Bypass_On (%d)\n", b_isBypass);
		//#ifndef CONFIG_MEMC_BYPASS
		if(Get_DISPLAY_PANEL_TYPE()== P_VBY1_TO_HDMI_MEMC_FREERUN){
			b_isBypass = 0;
			rtd_pr_memc_emerg("P_VBY1_TO_HDMI_MEMC_FREERUN, keep memc_mux = 1.\n");
		}

		memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);

		//check need update or not to reduce time
		if((b_isBypass && (memc_mux_ctrl_reg.memc_outmux_sel == 1)) ||
		(!b_isBypass && (memc_mux_ctrl_reg.memc_outmux_sel == 0))){
			needUpdate = 1;
		}

		if(!(Scaler_MEMC_Get_First_Run_FBG_enable() || Scaler_MEMC_Get_instanboot_resume_Done() || Scaler_MEMC_Get_snashop_resume_Done()))
		{//memc initial ready case need to run
			if(b_isBypass && (memc_mux_ctrl_reg.memc_outmux_sel == 1)){
				if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC)//VDEC source
				&& vsc_get_adaptivestream_flag(SLR_MAIN_DISPLAY)//CP case
				&& PPOVERLAY_Display_Timing_CTRL1_get_disp_fsync_en(IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg))//timing sync
				&& !((PPOVERLAY_uzudtg_control1_get_dtgm2uzuvs_line(IoReg_Read32(PPOVERLAY_uzudtg_control1_reg))==0)
				&& (PPOVERLAY_MEMCDTG_CONTROL3_get_dtgm2goldenpostvs_line(IoReg_Read32(PPOVERLAY_MEMCDTG_CONTROL3_reg))==0))
				)
				{
					rtd_pr_memc_notice("MEMC mux bypass under timing sync case.\n");
					//adjust VO framerate
					Scaler_adjust_VO_framerate_for_MEMC_bypass(1);
					ppoverlay_double_buffer_ctrl2_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
					ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
					ppoverlay_double_buffer_ctrl2_reg.memcdtgreg_dbuf_en = 1;
					IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
					memc_mux_ctrl_reg.memc_outmux_sel = 0;
					IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_reg, memc_mux_ctrl_reg.regValue);
					scaler_disp_dtg_set_align_output_timing(b_isBypass);
					vbe_disp_set_dtgM2uzu_lowdelay_mode(2);
					//Configure s1ip and time borrow when MEMC mux change
					scaler_disp_dtg_set_s1ip_delay(1);
					//recover VO framerate
					Scaler_adjust_VO_framerate_for_MEMC_bypass(0);

					return;
				}
			vbe_disp_set_dtgM2uzu_lowdelay_mode(0);
		}
		else if ((b_isBypass == 0) && (memc_mux_ctrl_reg.memc_outmux_sel == 0)){
			vbe_disp_set_dtgM2uzu_lowdelay_mode(0);
		}
		}

		if(needUpdate)
        {
            WaitFor_DEN_STOP_MEMCDTG();
            ppoverlay_double_buffer_ctrl2_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
            ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 0;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);

			if(b_isBypass)
				memc_mux_ctrl_reg.memc_outmux_sel = 0;
			else
				memc_mux_ctrl_reg.memc_outmux_sel = 1;
			IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_reg, memc_mux_ctrl_reg.regValue);

            ppoverlay_double_buffer_ctrl2_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
            ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
            IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);
		}
		//#endif
		if(needUpdate){
#if 0
			//set apply
			ppoverlay_double_buffer_ctrl2_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
			ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
			IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, ppoverlay_double_buffer_ctrl2_reg.regValue);

			while((ppoverlay_double_buffer_ctrl2_reg.uzudtgreg_dbuf_set)&& --count){
			ppoverlay_double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg);
			msleep(10);
			}
#endif

			scaler_disp_dtg_set_align_output_timing(b_isBypass);
		}

		if(b_isBypass==0){
			vbe_disp_set_dtgM2uzu_lowdelay_mode(1);
		}
		else{
			vbe_disp_set_dtgM2uzu_lowdelay_mode(2);
		}

		//Configure s1ip and time borrow when MEMC mux change
		scaler_disp_dtg_set_s1ip_delay(1);


#if 0//def ENABLE_IMD_PST_SMOOTHTOGGLE
		if((b_isBypass) && (get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY))){
		rtd_printk(KERN_NOTICE, "VSC","Scaler_MEMC_Bypass_On, set pst lowdelay mode\n");
		scaler_vsc_set_adaptive_pst_lowdelay_mode(1);
		}
#endif

		//fix me: temprory hack code, due to uncertainly rule about uzu120,
		//only confirm if memc enable, we dont enter this mode.
		if(b_isBypass==0){
#ifndef BUILD_QUICK_SHOW            
		extern void drvif_color_ultrazoom_uzu_4k_120_mode(unsigned char x);
		drvif_color_ultrazoom_uzu_4k_120_mode(0);
 #endif       
		}
	}else{
//#else
	//MEMC Bypass case, default set uzu/memc line pixel delay to 0
	vbe_disp_set_dtgM2uzu_lowdelay_mode(2);
    //scaler_disp_dtg_set_align_output_timing(TRUE);

#if RTK_MEMC_Performance_tunging_from_tv001
	scaler_disp_dtg_set_s1ip_delay(1);
#endif
//#endif
	}
}

#ifndef BUILD_QUICK_SHOW

static unsigned int b_memc_unmte_ready_flag = 1;
void Scaler_set_MEMC_unmute_ready_flag(unsigned char enable)
{
	b_memc_unmte_ready_flag = enable;
	rtd_pr_memc_notice("[%s] enable = %d.\n", __FUNCTION__, enable);
}

unsigned int Scaler_get_MEMC_unmute_ready_flag(void)
{
	return b_memc_unmte_ready_flag;
}
#endif
void Scaler_set_MEMC_gamemode_lowdelay(unsigned char enable){
	return;
}
#ifndef BUILD_QUICK_SHOW
unsigned char Scaler_MEMC_get_Bypass_Status(void)
{
	ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
	memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
	if(memc_mux_ctrl_reg.memc_outmux_sel)
		return FALSE;
	else
		return TRUE;
}


#if RTK_MEMC_Performance_tunging_from_tv001
#if 0
void Scaler_MEMC_MC_Set_MEMBoundary(unsigned int  nDownlimit,unsigned int nUplimit)
{
	//rtd_pr_memc_emerg("Scaler_MEMC_MC_Set_MEMBoundary= %x, %x\n", nDownlimit, nUplimit);
	#if 0
	IoReg_Write32(MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_up_reg, MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg, MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg, MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_low_reg, MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));
	#else
	IoReg_Write32(MC_DMA_MC_LF_DMA_WR_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_HF_DMA_WR_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_reg, nDownlimit);
	#endif
}
#endif

#else
void Scaler_MEMC_MC_Set_MEMBoundary(unsigned int  nDownlimit,unsigned int nUplimit)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	//rtd_pr_memc_emerg("Scaler_MEMC_MC_Set_MEMBoundary= %x, %x\n", nDownlimit, nUplimit);
	#if 0
	IoReg_Write32(MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_up_reg, MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg, MEMC_MC_DMA_MC_LF_DMA_WR_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg, MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_low_reg, MEMC_MC_DMA_MC_HF_DMA_WR_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));

	IoReg_Write32(MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_reg, MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_dma_up_limit(nUplimit));
	IoReg_Write32(MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_reg, MEMC_MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_dma_low_limit(nDownlimit));
	#else
	IoReg_Write32(MC_DMA_MC_LF_DMA_WR_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_HF_DMA_WR_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_reg, nDownlimit);
	#endif
#endif	
#endif
}

#endif

#endif
void Scaler_MEMC_allocate_memory(void){

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else
	
	unsigned int memc_size = 0;
	unsigned long memc_addr = 0;
	//#ifdef CONFIG_BW_96B_ALIGNED
	unsigned long memc_addr_aligned = 0;
	//#endif
	
#if CONFIG_MEMC_BOUNDARY_AUTO
	memc_size = Scaler_MEMC_Get_Size_Addr(&memc_addr_aligned, &memc_addr);
	if(memc_size==0) {
		return;
	}
#else

#if 0
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
		memc_size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MEMC, (void **)&memc_addr);
#else
		memc_size = carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void *)&memc_addr);
#endif
#endif

#ifdef CONFIG_BW_96B_ALIGNED
	#if 0
	#if CONFIG_MC_8_BUFFER
		if(Get_DISPLAY_REFRESH_RATE() <= 60){
			memc_addr_aligned = dvr_memory_alignment((unsigned long)memc_addr, KMEMC_TOTAL_SIZE_8buf_8bit/*KMEMC_TOTAL_SIZE_8buf_8bit_8K*/);
		}else{
			memc_addr_aligned = dvr_memory_alignment((unsigned long)memc_addr, KMEMC_TOTAL_SIZE_8buf_10bit/*KMEMC_TOTAL_SIZE_8buf_10bit_8K*/);
		}
	#else
		memc_addr_aligned = dvr_memory_alignment((unsigned long)memc_addr, KMEMC_TOTAL_SIZE_6buf_10bit/*KMEMC_TOTAL_SIZE_6buf_10bit_8K*/);
	#endif
	#else
		memc_addr_aligned = dvr_memory_alignment((unsigned long)memc_addr, Get_KMEMC_TOTAL_SIZE());
	#endif
		if (memc_addr == 0 || memc_size == 0  || memc_addr_aligned==0) {
			rtd_pr_memc_emerg("[%s %d] ERR : %lx %x %lx\n", __func__, __LINE__, memc_addr, memc_size,memc_addr_aligned);
			return;
		}
#else
		if (memc_addr == 0 || memc_size == 0) {
			rtd_pr_memc_emerg("[%s %d] ERR : %lx %x\n", __func__, __LINE__, memc_addr, memc_size);
			return;
		}
#endif
	
#endif
	
#ifdef CONFIG_BW_96B_ALIGNED
	gphy_addr_kme = memc_addr_aligned;
#else
	gphy_addr_kme = memc_addr;
#endif
	gphy_addr_kmc00 = gphy_addr_kme + KME_TOTAL_SIZE;
	gphy_addr_kmc01 = gphy_addr_kmc00 + MEMC_KMC_00_START_TOTAL_SIZE;
	rtd_pr_memc_emerg("KME start address = %x, size = %x, (%x)\n", gphy_addr_kme,memc_size ,KMEMC_TOTAL_SIZE);
	//rtd_pr_memc_emerg("KMC00 start address = %x\n", gphy_addr_kmc00);
	//rtd_pr_memc_emerg("KMC01 start address = %x\n", gphy_addr_kmc01);
	
	if(KME_TOTAL_SIZE > memc_size){
		rtd_pr_memc_emerg("[KME_TOTAL_SIZE] setting error(%x, %x)",memc_addr,memc_size );
		return;
	}
	
	if(g_KERNEL_AFTER_QS_MEMC_FLG==false){
		if(Scaler_MEMC_CLK_Check() == FALSE){
			Scaler_MEMC_CLK_Enable();
		}
		rtd_pr_memc_emerg("MEMC Clock : 010c=%x, 011c=%x\n", rtd_inl(SYS_REG_SYS_SRST3_reg), rtd_inl(SYS_REG_SYS_CLKEN3_reg));

#if CONFIG_MEMC_BOUNDARY_AUTO
		//set mc boundary with memc max memory size
		Scaler_MEMC_Set_ME_MEMBoundary(memc_addr, memc_addr+memc_size);
		Scaler_MEMC_Set_MC_MEMBoundary(memc_addr, memc_addr+memc_size);
#else
		
		if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_2K){
			
			//ME memory boundary
			Scaler_MEMC_Set_ME_MEMBoundary(gphy_addr_kme, (gphy_addr_kme + KME_TOTAL_SIZE));
			//MC memory boundary
		#if 0
		#if CONFIG_MC_8_BUFFER
			if(Get_DISPLAY_REFRESH_RATE() <= 60){
				Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_8bit));	
			}else{
				Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_10bit)); 
			}
		#else
			Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_6buf_10bit)); 
		#endif
		#else
			Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + Get_KMC_TOTAL_SIZE()));	
		#endif
		
		}else{
		
			//ME memory boundary
			Scaler_MEMC_Set_ME_MEMBoundary(gphy_addr_kme, (gphy_addr_kme + KME_TOTAL_SIZE));
			//MC memory boundary
		#if 0
		#if CONFIG_MC_8_BUFFER
			if(Get_DISPLAY_REFRESH_RATE() <= 60){
				Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_8bit));	
			}else{
				Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_10bit)); 
			}
		#else
			Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_6buf_10bit)); 
		#endif
		#else
			Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + Get_KMC_TOTAL_SIZE()));	
		#endif
		}
#endif //CONFIG_MEMC_BOUNDARY_AUTO
	}
		//IoReg_Write32(MEMC_DBUS_WRAP_Mc_downlimit_addr_reg, gphy_addr_kmc00);
		//IoReg_Write32(MEMC_DBUS_WRAP_Mc_uplimit_addr_reg, (gphy_addr_kmc00 + KMC_TOTAL_SIZE));
		//IoReg_Write32(MEMC_DBUS_WRAP_Mc_uplimit_addr_reg, (gphy_addr_kme + memc_size));

#endif

}

void Scaler_MEMC_initial_Case(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

	// MEMC 4k60 in, 4k60 out
#if RTK_MEMC_Performance_tunging_from_tv001
	#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	int ret=0;
	#endif
#else
	int ret;
#endif


	
	SCALER_MEMC_DMASTARTADDRESS_T* memc_dma_address_info;
#if RTK_MEMC_Performance_tunging_from_tv001
	//unsigned int ulCount = 0;
#else
	unsigned int ulCount = 0;
#endif

	VPQ_MEMC_INPUT_FORMAT input_format;
	VPQ_MEMC_OUTPUT_FORMAT output_format;
	ppoverlay_memcdtg_dv_den_start_end_RBUS memcdtg_dv_den_start_end_reg;

#if RTK_MEMC_Performance_tunging_from_tv001
	
#else
	ret=0;
#endif

	memcdtg_dv_den_start_end_reg.regValue = IoReg_Read32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg);
//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
	//memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta = Get_DISP_DEN_STA_VPOS();
	//memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end = Get_DISP_DEN_END_VPOS();
		return;
	}
	else if (Get_DISPLAY_REFRESH_RATE() > 120){//k23
			memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta = Get_DISP_DEN_STA_VPOS();
			memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end = Get_DISP_DEN_END_VPOS();
	}else{
//#else // memcdtg start position should sync to MEMC HW
		memcdtg_dv_den_start_end_reg.memcdtg_dv_den_sta = Get_DISP_DEN_STA_VPOS();
		memcdtg_dv_den_start_end_reg.memcdtg_dv_den_end = Get_DISP_DEN_END_VPOS();
//#endif
	}

	IoReg_Write32(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg, memcdtg_dv_den_start_end_reg.regValue);

	fw_scaler_dtg_double_buffer_apply();

	if(g_KERNEL_AFTER_QS_MEMC_FLG==false){
		//MEMC CLK Init
		Scaler_MEMC_CLK();
	}
	memc_dma_address_info = (SCALER_MEMC_DMASTARTADDRESS_T*)Scaler_GetShareMemVirAddr(SCALERIOC_MEMC_INITIALIZATION);

#if RTK_MEMC_Performance_tunging_from_tv001
	//ulCount = sizeof(SCALER_MEMC_DMASTARTADDRESS_T) / sizeof(unsigned int);
#else
	ulCount = sizeof(SCALER_MEMC_DMASTARTADDRESS_T) / sizeof(unsigned int);
#endif

	//MEMC CLK check
	if(g_KERNEL_AFTER_QS_MEMC_FLG==false && Scaler_ME_CLK_Check() == FALSE ){
		rtd_pr_memc_notice("/***   [%s][%d]MEMC_clock_check = false !! Do Scaler_MEMC_CLK_Enable   ***/\n\r", __FUNCTION__, __LINE__);
		Scaler_MEMC_CLK_Enable();
	}
	memc_dma_address_info->ME_start_addr= gphy_addr_kme;
	memc_dma_address_info->MC00_start_addr = gphy_addr_kmc00;
	memc_dma_address_info->MC01_start_addr = gphy_addr_kmc01;
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	//change endian
	memc_dma_address_info->ME_start_addr = htonl(memc_dma_address_info->ME_start_addr);
	memc_dma_address_info->MC00_start_addr = htonl(memc_dma_address_info->MC00_start_addr);
	memc_dma_address_info->MC01_start_addr = htonl(memc_dma_address_info->MC01_start_addr);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_INITIALIZATION, 0, 0))){
		rtd_pr_memc_debug( "[MEMC]ret=%d, SCALERIOC_MEMC_INITIALIZATION RPC fail !!!\n", ret);
	}
#else

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	Scaler_MEMC_Initialize(memc_dma_address_info->ME_start_addr, memc_dma_address_info->MC00_start_addr, memc_dma_address_info->MC01_start_addr);
#endif
#endif
	input_format = VPQ_INPUT_2D;
	output_format = VPQ_OUTPUT_2D;
	//set MEMC in,out use case
#if RTK_MEMC_Performance_tunging_from_tv001
	if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_2K)
#else
	if(Get_DISPLAY_PANEL_TYPE() == P_LVDS_2K1K)
#endif	
	{
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_1920X1080, VPQ_OUTPUT_1920X1080, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
#if RTK_MEMC_Performance_tunging_from_tv001//xj//3k
	else if(0/*Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_3K*/)
	{
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_2560X1440, VPQ_OUTPUT_2560X1440, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
#endif
#if ENABLE_4K1K
	else if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_4K1K) //4k1k
	{
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X1080, VPQ_OUTPUT_3840X1080, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
#endif
	else
	{
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X2160, VPQ_OUTPUT_3840X2160, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
	//set MEMC in/out frame rate
	Scaler_MEMC_SetInOutFrameRateByDisp(SLR_MAIN_DISPLAY);

#ifdef BUILD_QUICK_SHOW
	// mc only mode
	Scaler_MEMC_MEMC_CLK_OnOff(0, 1, 0);
	Scaler_MEMC_Set_EnableToBypass_Flag(1);
	Scaler_MEMC_LowDelayMode(1);
	if(MEMC_Lib_Get_QS_PCmode_Flg()){
		MEMC_LibSetMEMC_PCModeEnable(1);
	}
#endif //BUILD_QUICK_SHOW


#endif
}

unsigned char Get_MEMC_Enable_Dynamic(void);
extern unsigned char get_vt_EnableFRCMode(void);

/*api which have protect by forcebg_semaphore*/
void Scaler_MEMC_fbg_control(char const* pfunc,unsigned int line)
{

        // [ML3RTANDN-525]follow DIC Yan-De's suggestion to delay four frame
        // to get MEME fully ready after DC resume
        unsigned int frame_delay = 0;
        if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000) > 0)
            frame_delay = (1000000/Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_V_FREQ_1000)) << 2;

#if 0	//always disable memc mux bg after scaler
	if(MEMC_First_Run_FBG_enable == _ENABLE){
		// msleep(50);
		msleep(frame_delay);
		Scaler_MEMC_output_force_bg_enable(FALSE,__func__,__LINE__);
		MEMC_First_Run_FBG_enable = _DISABLE;
		rtd_pr_memc_notice("[MEMC]MEMC_First_Run_FBG_enable disable FBG!! Delay = %dms\n",frame_delay);
	}

	if(MEMC_instanboot_resume_Done == 1){
		// msleep(50);
		msleep(frame_delay);
		Scaler_MEMC_output_force_bg_enable(FALSE,__func__,__LINE__);
		MEMC_instanboot_resume_Done = 0;
		rtd_pr_memc_notice("[MEMC]MEMC_instanboot_resume_Done disable FBG!! Delay = %dms\n",frame_delay);
	}

	if(MEMC_snashop_resume_Done == 1){
		Scaler_MEMC_output_force_bg_enable(FALSE,__func__,__LINE__);
		MEMC_snashop_resume_Done = 0;
		rtd_pr_memc_notice("[MEMC]MEMC_snashop_resume_Done disable FBG!!\n");
	}
#else
#ifndef BUILD_QUICK_SHOW
	if(get_vt_EnableFRCMode() == TRUE)
#endif
    {
		msleep(frame_delay);
		Scaler_MEMC_output_force_bg_enable(FALSE,__func__,__LINE__);
		MEMC_First_Run_FBG_enable = 0;
		MEMC_instanboot_resume_Done = 0;
		MEMC_snashop_resume_Done = 0;
	}
#endif

	//set MEMC in, out frame rate
	//Scaler_MEMC_SetInOutFrameRateByDisp(SLR_MAIN_DISPLAY);//Can not send RPC here
    rtd_pr_memc_notice("#####[%s] disable MEMC mux fbg, call by [%s(line %d)] \n", __FUNCTION__, pfunc, line);
}

extern int vbe_dtg_memec_bypass_switch(void);
void Scaler_MEMC_Handler(void){

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		return;
#else

	VPQ_MEMC_INPUT_FORMAT input_format;
	VPQ_MEMC_OUTPUT_FORMAT output_format;

#if RTK_MEMC_Performance_tunging_from_tv001
	//unsigned char Mc_clk_state;
	//Mc_clk_state = ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>31)&0x1);
#else
	unsigned char Mc_clk_state;
	Mc_clk_state = ((IoReg_Read32(SYS_REG_SYS_CLKEN3_reg)>>31)&0x1);
#endif

#if 1//RTK_MEMC_Performance_tunging_from_tv001
	if(MEMC_First_Run_Done == 0 || get_reset_MEMC_flg()==true)
#else
	if(MEMC_First_Run_Done == 0)
#endif
	{
		rtd_pr_memc_debug( "[MEMC][init]%s %d [%d]\n",__func__,__LINE__,MEMC_First_Run_Done);
		//memc initial RPC
		MEMC_First_Run_Done = 1;
		MEMC_First_Run_FBG_enable = _ENABLE;

#if 1//RTK_MEMC_Performance_tunging_from_tv001
		set_reset_MEMC_flg(false);
#endif
		#ifndef BUILD_QUICK_SHOW
		if(g_KERNEL_AFTER_QS_MEMC_FLG==false){
			Scaler_MEMC_output_force_bg_enable(TRUE,__func__,__LINE__);
		}
		#endif
		Scaler_MEMC_initial_Case();
		if(MEMC_First_Run_force_SetInOutUseCase_Done == 0){
			MEMC_First_Run_force_SetInOutUseCase_Done=1;
		}
		rtd_pr_memc_debug( "[MEMC][init]initial in handler!![%d]\n",MEMC_First_Run_Done);
	}
#if 0
	// 2D, 3D switch
	if((get_3D_mode_enable() == true) && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == true)){
		//K2L on support PR 3D
		input_format = VPQ_INPUT_3D_LBL;
		output_format = VPQ_OUTPUT_3D_PR;
	}
	else{
		input_format = VPQ_INPUT_2D;
		output_format = VPQ_OUTPUT_2D;
	}

#else
	input_format = VPQ_INPUT_2D;
	output_format = VPQ_OUTPUT_2D;
#endif

#if 1//RTK_MEMC_Performance_tunging_from_tv001
	if(MEMC_instanboot_resume_Done == 1){
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X2160, VPQ_OUTPUT_3840X2160, VPQ_INPUT_3D_LBL, VPQ_OUTPUT_3D_PR);
		//msleep(100);//Remove by Will for performance
		rtd_pr_memc_debug( "[MEMC]reset usecase (instanboot)!!\n");
	}	
#else
#if 0
	if(MEMC_instanboot_resume_Done == 1){
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X2160, VPQ_OUTPUT_3840X2160, VPQ_INPUT_3D_LBL, VPQ_OUTPUT_3D_PR);
		//msleep(100);//Remove by Will for performance
		rtd_pr_memc_debug( "[MEMC]reset usecase (instanboot)!!\n");
	}
#endif
#endif

	//set MEMC in,out use case
	if(MEMC_instanboot_resume_Done == 1){

#if RTK_MEMC_Performance_tunging_from_tv001
		if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_2K)
#else
		if(Get_DISPLAY_PANEL_TYPE() == P_LVDS_2K1K)
#endif			
		{
			Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_1920X1080, VPQ_OUTPUT_1920X1080, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
		}
#if RTK_MEMC_Performance_tunging_from_tv001
		else if(/*Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_3K*/0){
			Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_2560X1440, VPQ_OUTPUT_2560X1440, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
		}
#endif
#if ENABLE_4K1K
		else if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_4K1K){ //4k1k
			Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X1080, VPQ_OUTPUT_3840X1080, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
		}
#endif
		else
		{
			Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X2160, VPQ_OUTPUT_3840X2160, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
		}
		//msleep(100);//Remove by Will for performance
		rtd_pr_memc_debug( "[MEMC]reset usecase (instanboot)!!\n");
	}
	//Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X2160, VPQ_OUTPUT_3840X2160, input_format, output_format);

	//Instanboot to initial phase table
	if(MEMC_instanboot_resume_Done == 1){
		Scaler_MEMC_Instanboot_InitPhaseTable(1);
		rtd_pr_memc_debug( "[MEMC]initial phase table (instanboot)!!\n");
	}
	//set MEMC in, out frame rate
	//Scaler_MEMC_SetInOutFrameRateByDisp(SLR_MAIN_DISPLAY);//Move after disable force bg
#ifdef BUILD_QUICK_SHOW
	MEMC_LibSet_MCDMA_DB_apply();
#if 1
	FRC_phTable_set_LineModeCtrl();
	if(MEMC_Lib_get_LineMode_flag() == 1)
		MEMC_Lib_SetLineMode(1);
	else
		MEMC_Lib_SetLineMode(0);
#endif
#endif
	//MEMC mux
	//if (Mc_clk_state == 1)
	//	Scaler_MEMC_outMux(TRUE,TRUE);

	rtd_pr_memc_debug( "[MEMC]Scaler_MEMC_Handler Done!!\n");
#endif	
}

#ifndef BUILD_QUICK_SHOW
#ifdef MEMC_DYNAMIC_ONOFF
void Scaler_ME_OnOff(unsigned char enable)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
				return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	unsigned int enable_bit;
	if(enable == TRUE)
		enable_bit = 0x00100000;
	else
		enable_bit = 0x0;

	#ifndef BUILD_QUICK_SHOW
		if(g_KERNEL_AFTER_QS_MEMC_FLG==false|| (get_product_type() != PRODUCT_TYPE_DIAS)){
			IoReg_Mask32(KME_DM_TOP0_KME_00_AGENT_reg, 0xffefffff, enable_bit);
			IoReg_Mask32(KME_DM_TOP0_KME_01_AGENT_reg, 0xffefffff, enable_bit);
		}
	#endif
	IoReg_Mask32(KME_DM_TOP0_KME_02_AGENT_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP0_KME_03_AGENT_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP0_KME_04_AGENT_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP0_KME_05_AGENT_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP0_KME06AGENT_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP0_KME_07_AGENT_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_14_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_1C_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_38_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_40_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_5C_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_64_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_80_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(KME_DM_TOP1_KME_DM_TOP1_88_reg, 0xffefffff, enable_bit);
#endif	
#endif
}

void Scaler_MC_OnOff(UINT8 enable){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
					return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	unsigned int enable_bit;
	if(enable == TRUE)
		enable_bit = 0x00100000;
	else
		enable_bit = 0x0;

	IoReg_Mask32(MC_DMA_MC_LF_WDMA_MEND1_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(MC_DMA_MC_LF_WDMA_MEND1_reg, 0xffefffff, enable_bit);
	//IoReg_Mask32(mc_01_write_enable_ADDR, 0xffefffff, enable_bit); //register remove
	IoReg_Mask32(MC_DMA_MC_HF_WDMA_NUM_BL_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(MC_DMA_MC_HF_WDMA_MSTART1_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(MC_DMA_MC_HF_WDMA_MSTART3_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(MC_DMA_MC_HF_WDMA_MSTART5_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(MC_DMA_MC_HF_WDMA_MSTART7_reg, 0xffefffff, enable_bit);
	IoReg_Mask32(MC_DMA_MC_HF_WDMA_MEND1_reg, 0xffefffff, enable_bit);
#endif
#endif

}

#if 1//#ifdef KID23_TO_WEBOS
static unsigned char no_signal_MEMC_outputbg_check = 0;
#endif

void Scaler_MEMC_Booting_NoSignal_Check_OutputBG(unsigned char enable){
#if 1//#ifdef KID23_TO_WEBOS

	//check MEMC out bg when not run scaler
	ppoverlay_memc_mux_ctrl_RBUS memc_mux_ctrl_reg;
	ppoverlay_memc_mux_ctrl_bg_RBUS memc_mux_ctrl_bg_reg;

	ppoverlay_double_buffer_ctrl2_RBUS memc_double_buffer_ctrl_reg;
	ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;
	memc_mux_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
	memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);

	//add checking no signal status @Crixus 20160112
	if(Scaler_MEMC_Get_First_Run_FBG_enable() || Scaler_MEMC_Get_instanboot_resume_Done() || Scaler_MEMC_Get_snashop_resume_Done()){
		if((enable == 0) && (memc_mux_ctrl_bg_reg.memc_out_bg_en == 1)){
			memc_mux_ctrl_bg_reg.memc_out_bg_en = 0;
			no_signal_MEMC_outputbg_check = 1;
			Scaler_MEMC_Handler();
			memc_mux_ctrl_reg.memcdtg_golden_vs = 0;
			rtd_pr_memc_notice("\r\n####[MEMC]No signal and disable MEMC out bg\n");
		}
		else{
			if(no_signal_MEMC_outputbg_check == 1){
				if((enable == 1) && (memc_mux_ctrl_bg_reg.memc_out_bg_en == 0)){
					memc_mux_ctrl_bg_reg.memc_out_bg_en = 1;
#if RTK_MEMC_Performance_tunging_from_tv001
					memc_mux_ctrl_reg.memcdtg_golden_vs = 1;
#else
					if(Get_DISPLAY_PANEL_TYPE() != P_VBY1_TO_HDMI_MEMC_FREERUN)
						memc_mux_ctrl_reg.memcdtg_golden_vs = 1;
#endif					
					no_signal_MEMC_outputbg_check = 0;
					rtd_pr_memc_notice("\r\n####[MEMC]No signal and enable MEMC out bg\n");
				}
			}
		}
		IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_reg, memc_mux_ctrl_reg.regValue);
		IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, memc_mux_ctrl_bg_reg.regValue);

		//set apply
		memc_double_buffer_ctrl_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		memc_double_buffer_ctrl_reg.uzudtgreg_dbuf_set = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, memc_double_buffer_ctrl_reg.regValue);

		ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
		ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
	}
#endif	
}


static unsigned char MEMC_Dynamic_OnOff = 1;//default MEMC enable
extern VSC_INPUT_TYPE_T Get_DisplayMode_Src(unsigned char display);
void Scaler_MEMC_Dynamic_OnOff(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else	
	//MDOMAIN_CAP_Smooth_tog_ctrl_0_reg[1] => dummy bit to control dynamic MEMC on/off.

	//MEMC change to disable
	if((IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg) & _BIT1) && (MEMC_Dynamic_OnOff == 1)){
		rtk_run_scaler(SLR_MAIN_DISPLAY, (KADP_VSC_INPUT_TYPE_T)Get_DisplayMode_Src(SLR_MAIN_DISPLAY), KADP_VSC_OUTPUT_DISPLAY_MODE);
		Scaler_ME_OnOff(FALSE);
		Scaler_MC_OnOff(FALSE);
		Scaler_MEMC_outMux(TRUE,FALSE);
		MEMC_Dynamic_OnOff = 0;
		rtd_pr_memc_debug( "[MEMC]Dynamic off!!!!\n");
	}
	//MEMC change to enable
	else if(!(IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg) & _BIT1) && (MEMC_Dynamic_OnOff == 0)){
		rtk_run_scaler(SLR_MAIN_DISPLAY, (KADP_VSC_INPUT_TYPE_T)Get_DisplayMode_Src(SLR_MAIN_DISPLAY), KADP_VSC_OUTPUT_DISPLAY_MODE);
		Scaler_ME_OnOff(TRUE);
		Scaler_MC_OnOff(TRUE);
		MEMC_Dynamic_OnOff = 1;
		rtd_pr_memc_debug( "[MEMC]Dynamic on!!!!\n");
	}
#endif	
}

unsigned char Scaler_MEMC_GetDynamicOnOff(void){
	return MEMC_Dynamic_OnOff;
}
#endif
#endif

/*api which have protect by forcebg_semaphore*/
void Scaler_MEMC_output_force_bg_enable(unsigned char enable,char const* pfunc,unsigned int line){
	//MEMC enable, using MEMC out fbg
	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl_reg;
	ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;
	ppoverlay_memc_mux_ctrl_bg_RBUS memc_mux_ctrl_bg_reg;

	rtd_pr_memc_err( "#####[%s(%d)]call %s, MEMC out FBG = %d\n",pfunc,line,__FUNCTION__, enable);
	memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
	if(memc_mux_ctrl_bg_reg.memc_out_bg_en != enable)
	{//chnage value when  memc_out_bg_en is not the same
		//enable UZU DTG double buffer.
		double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		double_buffer_ctrl_reg.uzudtg_dbuf_vsync_sel = 3;//using uzudtg vsync
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl_reg.regValue);

		ppoverlay_double_buffer_ctrl3_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
		ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
		//enable MEMC output fbg
		memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
		memc_mux_ctrl_bg_reg.memc_out_bg_en = enable;
		IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, memc_mux_ctrl_bg_reg.regValue);

		//set apply
		ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
		ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_set = 1;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);
		ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
	}
#if 0//remove msleep
	while(double_buffer_ctrl_reg.uzudtgreg_dbuf_set) {
		timeout++;
		msleep(10);
		double_buffer_ctrl_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		if(timeout >= 5)
		break;
	}
	//disable UZU DTG double buffer.
	double_buffer_ctrl_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl_reg.uzudtgreg_dbuf_en = 0;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl_reg.regValue);
#endif
}

#ifndef BUILD_QUICK_SHOW
void Scaler_MEMC_set_output_bg_color(unsigned short color_r, unsigned short color_g, unsigned short color_b){
	ppoverlay_memc_mux_out1_color_value_gb_RBUS memc_mux_out1_color_value_gb_reg;
	ppoverlay_memc_mux_out1_color_value_r_RBUS memc_mux_out1_color_value_r_reg;

	memc_mux_out1_color_value_gb_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_OUT1_COLOR_VALUE_GB_reg);
	memc_mux_out1_color_value_r_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_OUT1_COLOR_VALUE_R_reg);

	memc_mux_out1_color_value_gb_reg.out_y_g_value = color_g;
	memc_mux_out1_color_value_gb_reg.out_cb_b_value = color_b;
	memc_mux_out1_color_value_r_reg.out_cr_r_value = color_r;

	IoReg_Write32(PPOVERLAY_MEMC_MUX_OUT1_COLOR_VALUE_GB_reg, memc_mux_out1_color_value_gb_reg.regValue);
	IoReg_Write32(PPOVERLAY_MEMC_MUX_OUT1_COLOR_VALUE_R_reg, memc_mux_out1_color_value_r_reg.regValue);
}
#endif

void Scaler_MEMC_SetMotionType(VPQ_MEMC_TYPE_T motion)
{
	MEMC_motion_type = motion;
}
VPQ_MEMC_TYPE_T Scaler_MEMC_GetMotionType(void)
{
	return MEMC_motion_type;
}
void Scaler_MEMC_Set_CheckMEMC_Outbg(unsigned char mode)
{
	AVD_Unstable_Check_MEMC_OutBG_flag = mode;
}
unsigned char Scaler_MEMC_Get_CheckMEMC_Outbg(void)
{
	return AVD_Unstable_Check_MEMC_OutBG_flag;
}
unsigned char Scaler_MEMC_Get_First_Run_FBG_enable(void)
{
	return MEMC_First_Run_FBG_enable;
}
unsigned char Scaler_MEMC_Get_instanboot_resume_Done(void)
{
	return MEMC_instanboot_resume_Done;
}
unsigned char Scaler_MEMC_Get_snashop_resume_Done(void)
{
	return MEMC_snashop_resume_Done;
}

unsigned int Scaler_MEMC_Get_rotate_mode(void)
{
	return get_rotate_mode(SLR_MAIN_DISPLAY);
}


#if 1//RTK_MEMC_Performance_tunging_from_tv001
/*void Scaler_MEMC_Set_PowerSaving_Status(unsigned char mode)
{
	g_MEMC_powersaving_on = mode;//0:off, 1:on
}
unsigned char Scaler_MEMC_Get_PowerSaving_Status(void)
{
	return g_MEMC_powersaving_on;
}*/
void Scaler_MEMC_Set_BlueScreen(unsigned char mode)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else
	MEMC_LibBlueScreen(mode);//0:off, 1:on
#endif	
}


void Scaler_MEMC_Set_Performance_Checking_Database_index(unsigned char index)
{
	MEMC_Performance_Checking_Database_index = index;
}

unsigned char Scaler_MEMC_Get_Performance_Checking_Database_index(void)
{
	return MEMC_Performance_Checking_Database_index;
}

#if RTK_MEMC_Performance_tunging_from_tv001
unsigned char * Scaler_vpq_MEMC_GetCadence(void)
{
	return g_scaler_vpq_memc_cadence;
}
#endif



extern void MEMC_Lib_Freeze(unsigned char enable);
void Scaler_MEMC_Set_Lib_Freeze(unsigned char enable)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else
	MEMC_Lib_Freeze(enable);
#endif	
}

void Scaler_MEMC_Set_MEMC_FreezeStatus_Flag(unsigned char u1_enable)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else

	MEMC_Lib_set_memc_freeze_status_flag(u1_enable);//glb_memc_freeze_status_flag = u1_enable;
#endif
}

unsigned char Scaler_MEMC_Get_MEMC_FreezeStatus_Flag(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else

    return MEMC_Lib_get_memc_freeze_status_flag();//glb_memc_freeze_status_flag;
#endif
}

extern unsigned char MEMC_Lib_UltraLowDelayMode_Judge(void);
unsigned char Scaler_MEMC_Lib_UltraLowDelayMode_Judge(void)
{
	if(get_MEMC_bypass_status_refer_platform_model())
	{
		return 0;
	}
#if defined(BUILD_QUICK_SHOW) && !defined(CONFIG_RTK_SUPPORT_MEMC)	
    return 0;
#else
	return MEMC_Lib_UltraLowDelayMode_Judge();//_output_frc_sceneAnalysis.u1_still_frame;
#endif
}

#ifndef BUILD_QUICK_SHOW
extern BOOL GetStillFrameFlag(void);
BOOL Scaler_PQLContext_GetStillFrameFlag(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else

	return GetStillFrameFlag();//_output_frc_sceneAnalysis.u1_still_frame;
#endif
}


extern BOOL MEMC_LibSetMEMCFrameRepeatEnable(BOOL bEnable, BOOL bForce);
BOOL Scaler_MEMC_LibSetMEMCFrameRepeatEnable(BOOL bEnable, BOOL bForce)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else
		
	return MEMC_LibSetMEMCFrameRepeatEnable(bEnable,bForce);//_output_frc_sceneAnalysis.u1_still_frame;
#endif

}

extern unsigned int Scaler_MEMC_GetAVSyncDelay(void);
unsigned int Scaler_Vpqmemcdev_MEMC_GetAVSyncDelay(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else
	return Scaler_MEMC_GetAVSyncDelay();
#endif
}


extern VOID Set_Info_from_Scaler(MEMC_INFO_FROM_SCALER *info_from_scaler);
VOID Scaler_Set_Info_from_Scaler(MEMC_INFO_FROM_SCALER *info_from_scaler){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else
	MEMC_INFO_FROM_SCALER scaler_info_from_scaler;
 	scaler_info_from_scaler.in_frame_rate = info_from_scaler->in_frame_rate;
	scaler_info_from_scaler.out_frame_rate = info_from_scaler->out_frame_rate;
	scaler_info_from_scaler.game_mode_flag = info_from_scaler->game_mode_flag;
	scaler_info_from_scaler.adaptive_stream_flag = info_from_scaler->adaptive_stream_flag;
	scaler_info_from_scaler.YT_callback_flag = info_from_scaler->YT_callback_flag;
	scaler_info_from_scaler.hdmi_flag = info_from_scaler->hdmi_flag;
	scaler_info_from_scaler.vdec_flag = info_from_scaler->vdec_flag;

	Set_Info_from_Scaler(&scaler_info_from_scaler);
#endif
}


#endif

VOID Scaler_MEMC_Set_EnableToBypass_Flag(unsigned char u1_enable)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else
	MEMC_Lib_set_memc_enable_to_bypass_flag(u1_enable);//glb_memc_enable_to_bypass_flag = u1_enable;
#endif
}

unsigned char Scaler_MEMC_Get_EnableToBypass_Flag(VOID)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return 0;
#else
	return MEMC_Lib_get_memc_enable_to_bypass_flag();//glb_memc_enable_to_bypass_flag;
#endif
}

VOID Scaler_MEMC_Set_BypassToEnable_Flag(unsigned char u1_enable)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
	return;
#else
	MEMC_Lib_set_memc_bypass_to_enable_flag(u1_enable);//glb_memc_bypass_to_enable_flag = u1_enable;
#endif
}

unsigned char Scaler_MEMC_Get_BypassToEnable_Flag(VOID)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
				return 0;
#else
	return MEMC_Lib_get_memc_bypass_to_enable_flag();//glb_memc_bypass_to_enable_flag;
#endif
}

#ifndef BUILD_QUICK_SHOW

int Scaler_MEMC_set_cinema_mode_en(unsigned char bEn)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else

	int ret = 0;
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifdef CONFIG_MEMC_TASK_QUEUE
	SCALER_MEMC_TASK_T task;
	#endif
#endif

	rtd_pr_memc_debug( "[MEMC]fwif_color_set_cinema_mode_en, bEn = %d\n", bEn);
#ifdef CONFIG_RTK_KDEV_DEBUG_ISR
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_MEMC_SETCINEMAMODE, bEn, 1))){
		rtd_pr_memc_debug( "[MEMC]ret=%d, SCALERIOC_MEMC_SETCINEMAMODE RPC fail !!!\n", ret);
	}
#else
#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	#ifndef CONFIG_MEMC_TASK_QUEUE
		if(!Scaler_MEMC_GetMEMC_Enable())
			return 0;
	Scaler_MEMC_SetCinemaMode(bEn);
#else
	if((vbe_disp_get_stop_memc_push_flag() == 1) && ((rtd_inl(0xB800011c)>>30)==0)){
		rtd_pr_memc_notice("[%s][%d]VRR Status Modify!\n",__FUNCTION__, __LINE__);
		Scaler_MEMC_SetCinemaMode(bEn);
	}else{
		task.type = SCALERIOC_MEMC_SETCINEMAMODE;
		task.data.value = bEn;
		Scaler_MEMC_PushTask(&task);
	}
	#endif
#endif
#endif
	return ret;
#endif
}

BOOL ScalerMEMC_EnablePowerSave(void)
{

#if RTK_MEMC_Performance_tunging_from_tv001
	_clues* SmartPic_clue = fwif_color_Get_SmartPic_clue();
#else
	_clues* SmartPic_clue = (_clues *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_SMARTPIC_CLUS_REMOVED);
#endif
	
	_RPC_clues* RPC_SmartPic_clue = (_RPC_clues *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_SMARTPIC_CLUS);

	if(SmartPic_clue == NULL || RPC_SmartPic_clue == NULL)
		return 1;

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC
	if(RPC_SmartPic_clue->prePowerSaveStatus == PS_OFF)
	{
#ifndef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS	
		if(Scaler_MEMC_SetPowerSaveOn())
			return 1;

		if(Scaler_MEMC_GetMEMC_Mode() != MEMC_OFF)
		{
			Scaler_MEMC_SetMEMCFrameRepeatEnable(1);
		}
#endif
		RPC_SmartPic_clue->prePowerSaveStatus = PS_ON;
	}
#endif

	return 0;

}

BOOL ScalerMEMC_DisablePowerSave(void)
{

	//_clues* SmartPic_clue = (_clues *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_SMARTPIC_CLUS);
	_RPC_clues* RPC_SmartPic_clue = (_RPC_clues *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_SMARTPIC_CLUS);

	if(RPC_SmartPic_clue == NULL)
		return 1;

#if 1//#ifdef CONFIG_HW_SUPPORT_MEMC

	if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
	{
#ifndef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		if(Scaler_MEMC_SetPowerSaveOff())
			return 1;

		if(Scaler_MEMC_GetMEMC_Mode() != MEMC_OFF)
		{
			Scaler_MEMC_SetMEMCFrameRepeatEnable(0);
		}
#endif
		RPC_SmartPic_clue->prePowerSaveStatus = PS_OFF;
	}
#endif
	return 0;

}

#endif
/**
 * @brief This function get RbusAccess stataus of MEMC
 * @retval TRUE		MEMC Rbus access Ready!		
 * @retval FALSE	MEMC Rbus can't be accessed!  
*/
extern int  rtk_otp_field_read_int_by_name(const char *name);
unsigned char Scaler_MEMC_RbusAccess_Check(void)//KTASKWBS-24448
{
	static unsigned char RbusAccess_Check = 0;
	unsigned int MEMC_disable_ori = 0;
	//unsigned int state_clk = 255;	
	//static unsigned int pre_MEMC_OTP_status= 0;
	static bool get_otp_status=false;

	//== get the MEMC clk status ==
	//state_clk = scalerVIP_Get_MEMC_status();
	
	//== brief This function get opt status of MEMC ==
	// retval 1->disable ; retval 0->enable 

	if(get_otp_status==false)
	{
#ifndef BUILD_QUICK_SHOW
		MEMC_disable_ori = rtk_otp_field_read_int_by_name("chip_type_memc_disable");
#else
		// FIXME: quick-show not support linux otp driver
		MEMC_disable_ori = IoReg_Read32(EFUSE_DATAO_37_reg )&_BIT30;
#endif

		/*if(pre_MEMC_OTP_status != MEMC_disable_ori){
			rtd_pr_memc_notice("[%s][MEMC] 0xB8038074[9], MEMC_disable_ori=,%d \n", __func__, MEMC_disable_ori);
		}
		pre_MEMC_OTP_status = MEMC_disable_ori;*/
		
		if(MEMC_disable_ori == 0){
			RbusAccess_Check = 1;
		}else{
			RbusAccess_Check = 0;
		}
		get_otp_status=true;
	}
	
	return RbusAccess_Check;
	
}


#ifndef BUILD_QUICK_SHOW

/*******************************************************************************
****************************VPQ MEMC DEVICE************************************
*******************************************************************************/
#ifdef CONFIG_PM

#if RTK_MEMC_Performance_tunging_from_tv001
extern unsigned int loadscript_cnt_2;
extern unsigned int loadscript_output_cnt;
#ifndef CONFIG_MEMC_NOTSUPPORT
static unsigned int In_cnt_status = 0;
static unsigned int Out_cnt_status = 0;
#endif
#endif


static int vpq_memc_suspend (struct device *p_dev)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else


#if RTK_MEMC_Performance_tunging_from_tv001
	#if 0
	//MCDMA__MC DMA
	HAL_DM_SetKmc00WriteEnable(FALSE);//vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc01WriteEnable(FALSE);//vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc04ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc05ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc06ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc07ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl & 0xfffffffe;

	//MESHARE__ME_DMA
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT0);
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT1);
	rtd_clearbits(ME_SHARE_DMA_ME_DMA_WR_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl & 0xfffffffe;
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT0);
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT1);
	rtd_clearbits(ME_SHARE_DMA_ME_DMA_RD_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl & 0xfffffffe;

	//MVinfo DMA disable
	rtd_clearbits(MVINFO_DMA_MVINFO_WR_Ctrl_reg, _BIT0);
	rtd_clearbits(MVINFO_DMA_MVINFO_RD_Ctrl_reg, _BIT0);

	//MV DMA disable
	HAL_DM_SetKmv01WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT & 0xffefffff;   IoReg_Read32(KME_DM_TOP2__MV01_AGENT_ADDR);
	HAL_DM_SetKmv02WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV02_AGENT_ADDR);
	HAL_DM_SetKmv03ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV03_AGENT_ADDR);
	HAL_DM_SetKmv04WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV04_AGENT_ADDR);
	HAL_DM_SetKmv05WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV05_AGENT_ADDR);
	HAL_DM_SetKmv06ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV06_AGENT_ADDR);
	HAL_DM_SetKmv07ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV07_AGENT_ADDR);
	HAL_DM_SetKmv08ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV08_AGENT_ADDR);
	HAL_DM_SetKmv09ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV09_AGENT_ADDR);
	HAL_DM_SetKmv10ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV10_AGENT_ADDR);
	HAL_DM_SetKmv11ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV11_AGENT_ADDR);
	HAL_DM_SetKmv12ReadEnable(FALSE);

	//ME DMA disable
	HAL_DM_SetKme00WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = IoReg_Read32(KME_DM_TOP0__KME_00_AGENT_ADDR);
	HAL_DM_SetKme01ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = IoReg_Read32(KME_DM_TOP0__KME_01_AGENT_ADDR);
	HAL_DM_SetKme02ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = IoReg_Read32(KME_DM_TOP0__KME_02_AGENT_ADDR);
	HAL_DM_SetKme03ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = IoReg_Read32(KME_DM_TOP0__KME_03_AGENT_ADDR);
	HAL_DM_SetKme04WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = IoReg_Read32(KME_DM_TOP0__KME_04_AGENT_ADDR);
	HAL_DM_SetKme05WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = IoReg_Read32(KME_DM_TOP0__KME_05_AGENT_ADDR);
	HAL_DM_SetKme06ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT & 0xffefffff;		vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT =   IoReg_Read32(KME_DM_TOP0__KME06AGENT_ADDR);
	HAL_DM_SetKme07ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = IoReg_Read32(KME_DM_TOP0__KME_07_AGENT_ADDR);
	HAL_DM_SetKme08WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = IoReg_Read32(KME_DM_TOP1__KME_08_AGENT_ADDR);
	HAL_DM_SetKme09ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = IoReg_Read32(KME_DM_TOP1__KME_09_AGENT_ADDR);
	HAL_DM_SetKme10WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = IoReg_Read32(KME_DM_TOP1__KME_10_AGENT_ADDR);
	HAL_DM_SetKme11ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = IoReg_Read32(KME_DM_TOP1__KME_11_AGENT_ADDR);
	HAL_DM_SetKme12WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = IoReg_Read32(KME_DM_TOP1__KME_12_AGENT_ADDR);
	HAL_DM_SetKme13ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = IoReg_Read32(KME_DM_TOP1__KME_13_AGENT_ADDR);
	HAL_DM_SetKme14WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = IoReg_Read32(KME_DM_TOP1__KME_14_AGENT_ADDR);
	HAL_DM_SetKme15ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = IoReg_Read32(KME_DM_TOP1__KME_15_AGENT_ADDR);
	#endif
	In_cnt_status = loadscript_cnt_2;
	Out_cnt_status = loadscript_output_cnt;
#endif

	rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
	return 0;
#endif	
}

static int vpq_memc_resume (struct device *p_dev)
{
	rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
	return 0;
}
#ifdef CONFIG_HIBERNATION
static int vpq_memc_suspend_snashop (struct device *p_dev)
{		
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)//KTASKWBS-24448
		return 0;
	
	MEMC_First_Run_Done = 0;
	MEMC_First_Run_force_SetInOutUseCase_Done=0;
	rtd_pr_memc_notice("%s %d [,%x,%x,%x,]\n",__func__,__LINE__, rtd_inl(0xb8099018), rtd_inl(0xb809d008), rtd_inl(0xb809d5fc));
	return 0;
}

static int vpq_memc_resume_snashop (struct device *p_dev)
{
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)//KTASKWBS-24448
		return 0;

	rtd_pr_memc_notice("%s %d [,%x,%x,%x,][%d][%d]\n",__func__,__LINE__, rtd_inl(0xb8099018), rtd_inl(0xb809d008), rtd_inl(0xb809d5fc),MEMC_First_Run_Done,MEMC_First_Run_force_SetInOutUseCase_Done);
	if(MEMC_First_Run_Done == 0){
		//memc initial RPC
		MEMC_First_Run_Done = 1;
		MEMC_First_Run_FBG_enable = _ENABLE;
		Scaler_MEMC_output_force_bg_enable(TRUE,__func__,__LINE__);
		Scaler_MEMC_initial_Case();
		if(MEMC_First_Run_force_SetInOutUseCase_Done == 0){//need to trigger the Scaler_MEMC_SetInOutUseCase again when the snapshop on!!
			MEMC_First_Run_force_SetInOutUseCase_Done=1;
		}
		rtd_pr_memc_debug( "[MEMC]initial in handler!!\n");
	}
	rtd_pr_memc_notice("%s %d [,%x,%x,%x,][%d][%d]\n",__func__,__LINE__, rtd_inl(0xb8099018), rtd_inl(0xb809d008), rtd_inl(0xb809d5fc), MEMC_First_Run_Done,MEMC_First_Run_force_SetInOutUseCase_Done);
	return 0;
}
#endif

VPQ_MEMC_SUSPEND_RESUME_T vpq_memc_suspend_resume;
void vpq_memc_disp_suspend_snashop(void){	
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		return;
#else

#if RTK_MEMC_Performance_tunging_from_tv001
	#if CONFIG_MEMC_BOUNDARY_AUTO
			unsigned int u32_MC_Boundary = MEMC_Lib_Get_MC_Boundary(); // mc dma boundary is generated by memc_change_size
	#endif
#endif

#if 1//#ifdef KID23_TO_WEBOS

#if 1	
		//MEMC mux
		vpq_memc_suspend_resume.MEMC_MUX_MEMC_MUX_FUNC_CTRL_SUSPEND = IoReg_Read32(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg);
		//FBG
		vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
		//clock
		vpq_memc_suspend_resume.SYS_REG_SYS_DISPCLKSEL_SUSPEND = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
		//vpq_memc_suspend_resume.MEMC_LATENCY01 = IoReg_Read32(MEMC_LATENCY01_ADDR);
		//vpq_memc_suspend_resume.MEMC_LATENCY02 = IoReg_Read32(MEMC_LATENCY02_ADDR);	//	remove DC2
	
		if(get_MEMC_bypass_status_refer_platform_model() != TRUE){//KTASKWBS-24448


#if RTK_MEMC_Performance_tunging_from_tv001
		MEMC_LibSet_MCDMA_DB_en(0);
		//MEMC_LibDisableDMA();
#endif	
			if(Scaler_MEMC_CLK_Check() == FALSE){
				Scaler_MEMC_CLK_Enable();
			}
			rtd_pr_memc_notice("%s %d [,%x,%x,%x,]\n",__func__,__LINE__, rtd_inl(0xb8099018), rtd_inl(0xb809d008), rtd_inl(0xb809d5fc));



#if RTK_MEMC_Performance_tunging_from_tv001
		//ME memory boundary
		IoReg_Write32(DBUS_WRAPPER_Me_downlimit_addr_reg, gphy_addr_kme);
		IoReg_Write32(DBUS_WRAPPER_Me_uplimit_addr_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
		//ME_SHARE memory boundary
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
		//MVINFO memory boundary
		IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
		IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
		IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_up_reg, gphy_addr_kmc00);
		IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_up_reg, gphy_addr_kmc00);

		//MC memory boundary
#if CONFIG_MEMC_BOUNDARY_AUTO
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00+u32_MC_Boundary));	
#else

		#if 0
		#if CONFIG_MC_8_BUFFER
		if(Get_DISPLAY_REFRESH_RATE() <= 60){
			Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_8bit));	
		}else{
			Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_10bit));	
		}
		#else
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_6buf_10bit));	
		#endif
		#else
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + Get_KMC_TOTAL_SIZE())); 
		#endif
#endif			
#endif
			//ME memory boundary
			vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_DOWNLIMIT = IoReg_Read32(DBUS_WRAPPER_Me_downlimit_addr_reg);
			vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_UPLIMIT = IoReg_Read32(DBUS_WRAPPER_Me_uplimit_addr_reg);
			//ME_SHARE memory boundary
			vpq_memc_suspend_resume.ME_SHARE_WR_low_limit = IoReg_Read32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_low_reg);
			vpq_memc_suspend_resume.ME_SHARE_WR_up_limit = IoReg_Read32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_up_reg);
			vpq_memc_suspend_resume.ME_SHARE_RD_low_limit = IoReg_Read32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_low_reg);
			vpq_memc_suspend_resume.ME_SHARE_RD_up_limit = IoReg_Read32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_up_reg);
			//MV_INFO memory boundary
			vpq_memc_suspend_resume.MV_INFO_WR_low_limit = IoReg_Read32(MVINFO_DMA_MVINFO_WR_Rule_check_low_reg);
			vpq_memc_suspend_resume.MV_INFO_WR_up_limit = IoReg_Read32(MVINFO_DMA_MVINFO_RD_Rule_check_low_reg);
			vpq_memc_suspend_resume.MV_INFO_RD_low_limit = IoReg_Read32(MVINFO_DMA_MVINFO_WR_Rule_check_up_reg);
			vpq_memc_suspend_resume.MV_INFO_RD_up_limit = IoReg_Read32(MVINFO_DMA_MVINFO_RD_Rule_check_up_reg);
			//MC memory boundary
			vpq_memc_suspend_resume.MC_dma_low_limit = IoReg_Read32(MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg);
			vpq_memc_suspend_resume.MC_dma_up_limit = IoReg_Read32(MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg);	
		
			rtd_pr_memc_notice("%s %d [,%x,%x,%x,%x]\n",__func__,__LINE__,vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_DOWNLIMIT,vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_UPLIMIT,
						vpq_memc_suspend_resume.MC_dma_low_limit,vpq_memc_suspend_resume.MC_dma_up_limit);
		}
#endif
#endif
#endif
}

void vpq_memc_disp_resume_snashop(void){
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	ppoverlay_double_buffer_ctrl2_RBUS double_buffer_ctrl2_reg;
	int timeout = 0xff;
	//unsigned int i = 0;
#if RTK_MEMC_Performance_tunging_from_tv001
	#if CONFIG_MEMC_BOUNDARY_AUTO
	unsigned int u32_MC_Boundary = MEMC_Lib_Get_MC_Boundary(); // mc dma boundary is generated by memc_change_size
	#endif
#endif

	//MEMC mux
	IoReg_Write32(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg, vpq_memc_suspend_resume.MEMC_MUX_MEMC_MUX_FUNC_CTRL_SUSPEND);
	//FBG
	double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 1;
	double_buffer_ctrl2_reg.uzudtg_dbuf_vsync_sel = 3;
	//enable double buffer
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND = vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND;
	}else{
//#else
		vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND = vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND | 0x00001000;
//#endif
	}
	IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_reg, vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND);

	double_buffer_ctrl2_reg.uzudtgreg_dbuf_set = 1;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);
	//wait double buffer done
	double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
	while(double_buffer_ctrl2_reg.uzudtgreg_dbuf_set && timeout--){
		double_buffer_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL2_reg);
		mdelay(1);
	}
	//disable double buffer
	double_buffer_ctrl2_reg.uzudtgreg_dbuf_en = 0;
	IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL2_reg, double_buffer_ctrl2_reg.regValue);

	//clock
	IoReg_Write32(SYS_REG_SYS_DISPCLKSEL_reg, vpq_memc_suspend_resume.SYS_REG_SYS_DISPCLKSEL_SUSPEND);
	//IoReg_Write32(MEMC_LATENCY01_ADDR, vpq_memc_suspend_resume.MEMC_LATENCY01);
	//IoReg_Write32(MEMC_LATENCY02_ADDR, vpq_memc_suspend_resume.MEMC_LATENCY02);  // remove DC2

	if(get_MEMC_bypass_status_refer_platform_model() != TRUE){//KTASKWBS-24448
		if(Scaler_MEMC_CLK_Check() == FALSE){
			Scaler_MEMC_CLK_Enable();
		}
		rtd_pr_memc_notice("%s %d [,%x,%x,%x,]\n",__func__,__LINE__, rtd_inl(0xb8099018), rtd_inl(0xb809d008), rtd_inl(0xb809d5fc));

		MEMC_LibSet_MCDMA_DB_en(0);
		#if 1
		#if 1
			MEMC_Lib_MEDMA_Control(0,0,0,1);
			MEMC_Lib_MCDMA_Control(0,0,1);
		#else
		MEMC_LibDisableDMA();
		#endif
		#else
		rtd_clearbits(0xb8099460, _BIT0); //dma_off mc w
		rtd_clearbits(0xb80994dc, _BIT0); //dma_off mc w
		rtd_clearbits(0xb809ae1c, _BIT0); //dma_off mc r
		rtd_clearbits(0xb809ae3c, _BIT0); //dma_off mc r
		rtd_clearbits(0xb809ae5c, _BIT0); //dma_off mc r
		rtd_clearbits(0xb809ae7c, _BIT0); //dma_off mc r
		#endif

		rtd_pr_memc_notice("===== === %s %d  ======== \n",__func__,__LINE__);
		rtd_pr_memc_notice("[MEMC][,%x,%x,%x,%x]\n",vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_DOWNLIMIT,vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_UPLIMIT,
				vpq_memc_suspend_resume.MC_dma_low_limit,vpq_memc_suspend_resume.MC_dma_up_limit);
		rtd_pr_memc_notice("[MEMC][db]=[,%x,%x]\n",IoReg_Read32(0xb80994F0),IoReg_Read32(0xb809AEA8));
		rtd_pr_memc_notice("[MEMC][%x, %x, %x, %x, %x, %x]\n\r", rtd_inl(0xb8099460), rtd_inl(0xb80994dc), rtd_inl(0xb809ae1c), rtd_inl(0xb809ae3c), rtd_inl(0xb809ae5c), rtd_inl(0xb809ae7c));

	#if 0
		//b8099414~b8099450 (LF_WDMA)
		for (i = 0xb8099414; i <=0xb8099450; i=i+4)
		{
			printk(KERN_ERR "%x=%x\n", i, IoReg_Read32(i));
		}
		//b8099484~b80994C0 (HF_WDMA)
		for (i = 0xb8099484; i <=0xb80994C0; i=i+4)
		{
			printk(KERN_ERR "%x=%x\n", i, IoReg_Read32(i));
		}
	#endif

		#if 1
		//ME memory boundary
		IoReg_Write32(DBUS_WRAPPER_Me_downlimit_addr_reg, vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_DOWNLIMIT);
		IoReg_Write32(DBUS_WRAPPER_Me_uplimit_addr_reg, vpq_memc_suspend_resume.MEMC_DBUS_WRAP_ME_UPLIMIT);
		//ME_SHARE memory boundary
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_low_reg, vpq_memc_suspend_resume.ME_SHARE_WR_low_limit);
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_low_reg, vpq_memc_suspend_resume.ME_SHARE_RD_low_limit);
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_up_reg, vpq_memc_suspend_resume.ME_SHARE_WR_up_limit);
		IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_up_reg, vpq_memc_suspend_resume.ME_SHARE_RD_up_limit);
		//MVINFO memory boundary
		IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_low_reg, vpq_memc_suspend_resume.MV_INFO_WR_low_limit);
		IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_low_reg, vpq_memc_suspend_resume.MV_INFO_RD_low_limit);
		IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_up_reg, vpq_memc_suspend_resume.MV_INFO_WR_up_limit);
		IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_up_reg, vpq_memc_suspend_resume.MV_INFO_RD_up_limit);

#if RTK_MEMC_Performance_tunging_from_tv001
		#if CONFIG_MEMC_BOUNDARY_AUTO
		Scaler_MEMC_Set_MC_MEMBoundary(vpq_memc_suspend_resume.MC_dma_low_limit, (vpq_memc_suspend_resume.MC_dma_low_limit+u32_MC_Boundary)); 
		#else
		//MC memory boundary
		Scaler_MEMC_Set_MC_MEMBoundary(vpq_memc_suspend_resume.MC_dma_low_limit, vpq_memc_suspend_resume.MC_dma_up_limit);
		#endif
#else
		//MC memory boundary
		Scaler_MEMC_MC_Set_MEMBoundary(vpq_memc_suspend_resume.MC_dma_low_limit, vpq_memc_suspend_resume.MC_dma_up_limit);
#endif
		#endif

		
	}
	MEMC_snashop_resume_Done = 1;

#if RTK_MEMC_Performance_tunging_from_tv001
	if(get_MEMC_bypass_status_refer_platform_model() != TRUE){
		rtd_pr_memc_err( "[%s][%d][010c,%x,011c,%x,][e0b4,%x,e0b8,%x,][c038,%x,c040,%x,c048,%x,c050,%x,]\n",__func__,__LINE__, rtd_inl(SYS_REG_SYS_SRST3_reg), rtd_inl(SYS_REG_SYS_CLKEN3_reg), rtd_inl(DBUS_WRAPPER_Me_uplimit_addr_reg), rtd_inl(DBUS_WRAPPER_Me_downlimit_addr_reg),
			rtd_inl(KME_DM_TOP0_KME_00_AGENT_reg), rtd_inl(KME_DM_TOP0_KME_01_AGENT_reg), rtd_inl(KME_DM_TOP0_KME_02_AGENT_reg), rtd_inl(KME_DM_TOP0_KME_03_AGENT_reg));
		rtd_pr_memc_err( "[MEMC]Snashop Resume done !!!\n");
	}
#else
	rtd_pr_memc_err( "[MEMC]Snashop Resume done !!!\n");
#endif	
	
#endif
#endif
}

unsigned char runtime_suspend_flag = 0;
VOID MEMC_Lib_set_runtime_suspend_flag(unsigned char u1_enable){
	rtd_pr_memc_notice("[%s][%d]", __FUNCTION__, u1_enable);
	runtime_suspend_flag = u1_enable;
}

unsigned char MEMC_Lib_get_runtime_suspend_flag(VOID){
	//rtd_pr_memc_notice("[%s][%d]", __FUNCTION__, g_LineMode_flag);
	return runtime_suspend_flag;
}

extern void vbe_dtg_memec_bypass_switch_set(int mode);
void vpq_memc_disp_suspend_instanboot(void)
{

#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	unsigned int u32_interrupt_reg = 0;
	unsigned int u32_i,u32_j;
	unsigned int i=0;

#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
	unsigned int time3 = 0;
	unsigned char timeout_flag = 0;
#endif	

	
	rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return;
//#endif

rtd_pr_memc_notice("[Scaler_MEMC_Bypass_CLK_OnOff_JJJ][%x]\n", rtd_inl(0xb802e800));
	if(Scaler_MEMC_CLK_Check() == FALSE){
		Scaler_MEMC_CLK_Enable();
		Scaler_MEMC_PowerSaving_Disable();//PowerSaving Enable when MEMC CLK off
		//Scaler_MEMC_MEMC_CLK_OnOff(1, 1, 1); // MEMC must CLK on when DC off
	}

	// disable interrupt
	//reg_kmc_int_en
	u32_interrupt_reg = IoReg_Read32(KMC_TOP_kmc_top_18_reg);
	u32_interrupt_reg &= 0xf0ffffff;
	IoReg_Write32(KMC_TOP_kmc_top_18_reg, u32_interrupt_reg);

	//reg_post_int_en
	#if 1
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_HSYNC) == 1){
		MEMC_LibOutpuHSInterruptEnable(FALSE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT5);
	}
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_VSYNC) == 1){
		MEMC_LibOutputVSInterruptEnable(FALSE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT6);
	}
	#else
	u32_interrupt_reg = IoReg_Read32(KPOST_TOP_KPOST_TOP_08_reg);
	u32_interrupt_reg &= 0xfffffe1f;
	IoReg_Write32(KPOST_TOP_KPOST_TOP_08_reg, u32_interrupt_reg);
	#endif

	rtd_pr_memc_notice("[suspend_FFF][%x, %x, %x, %x, %x]\n\r", rtd_inl(0xb802e800), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(KPOST_TOP_KPOST_TOP_08_reg));

#if 1
	//MEMC mux
	vpq_memc_suspend_resume.MEMC_MUX_MEMC_MUX_FUNC_CTRL_SUSPEND = IoReg_Read32(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg);
	//FBG
	vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
	//clock
	vpq_memc_suspend_resume.SYS_REG_SYS_DISPCLKSEL_SUSPEND = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
	//vpq_memc_suspend_resume.MEMC_LATENCY01 = IoReg_Read32(MEMC_LATENCY01_ADDR);
	//vpq_memc_suspend_resume.MEMC_LATENCY02 = IoReg_Read32(MEMC_LATENCY02_ADDR);   //  remove DC2


#if RTK_MEMC_Performance_tunging_from_tv001
#if 1
	Mid_Mode_DisableMCDMA();
	Mid_Mode_DisableMEDMA(0);
#else
	//IP_enable W, need to set enable after ME_WDMA enable ( delay ~2000ns)
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT0);
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT1);
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT2);
	//IP_enable R, need to set enable after ME_RDMA enable ( delay ~2000ns)
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT0);
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT1);
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT2);
	//rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT3);
	
	//MCDMA__MC DMA
	HAL_DM_SetKmc00WriteEnable(FALSE);//vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc01WriteEnable(FALSE);//vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc04ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc05ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc06ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc07ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl & 0xfffffffe;

	//MESHARE__ME_WDMA enable, need to enable after ME_WDMA IP enable
	rtd_clearbits(ME_SHARE_DMA_ME_DMA_WR_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl & 0xfffffffe;
	//MESHARE__ME_RDMA enable, need to enable after ME_RDMA IP enable
	rtd_clearbits(ME_SHARE_DMA_ME_DMA_RD_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl & 0xfffffffe;

	//MVinfo DMA disable
	rtd_clearbits(MVINFO_DMA_MVINFO_WR_Ctrl_reg, _BIT0);
	rtd_clearbits(MVINFO_DMA_MVINFO_RD_Ctrl_reg, _BIT0);

	//MV DMA disable
	HAL_DM_SetKmv01WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT & 0xffefffff;   IoReg_Read32(KME_DM_TOP2__MV01_AGENT_ADDR);
	HAL_DM_SetKmv02WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV02_AGENT_ADDR);
	HAL_DM_SetKmv03ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV03_AGENT_ADDR);
	HAL_DM_SetKmv04WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV04_AGENT_ADDR);
	HAL_DM_SetKmv05WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV05_AGENT_ADDR);
	HAL_DM_SetKmv06ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV06_AGENT_ADDR);
	HAL_DM_SetKmv07ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV07_AGENT_ADDR);
	HAL_DM_SetKmv08ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV08_AGENT_ADDR);
	HAL_DM_SetKmv09ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV09_AGENT_ADDR);
	HAL_DM_SetKmv10ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV10_AGENT_ADDR);
	HAL_DM_SetKmv11ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV11_AGENT_ADDR);
	HAL_DM_SetKmv12ReadEnable(FALSE);

	//ME DMA disable
	HAL_DM_SetKme00WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = IoReg_Read32(KME_DM_TOP0__KME_00_AGENT_ADDR);
	HAL_DM_SetKme01ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = IoReg_Read32(KME_DM_TOP0__KME_01_AGENT_ADDR);
	HAL_DM_SetKme02ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = IoReg_Read32(KME_DM_TOP0__KME_02_AGENT_ADDR);
	HAL_DM_SetKme03ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = IoReg_Read32(KME_DM_TOP0__KME_03_AGENT_ADDR);
	HAL_DM_SetKme04WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = IoReg_Read32(KME_DM_TOP0__KME_04_AGENT_ADDR);
	HAL_DM_SetKme05WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = IoReg_Read32(KME_DM_TOP0__KME_05_AGENT_ADDR);
	HAL_DM_SetKme06ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT & 0xffefffff;		vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT =   IoReg_Read32(KME_DM_TOP0__KME06AGENT_ADDR);
	HAL_DM_SetKme07ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = IoReg_Read32(KME_DM_TOP0__KME_07_AGENT_ADDR);
	HAL_DM_SetKme08WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = IoReg_Read32(KME_DM_TOP1__KME_08_AGENT_ADDR);
	HAL_DM_SetKme09ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = IoReg_Read32(KME_DM_TOP1__KME_09_AGENT_ADDR);
	HAL_DM_SetKme10WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = IoReg_Read32(KME_DM_TOP1__KME_10_AGENT_ADDR);
	HAL_DM_SetKme11ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = IoReg_Read32(KME_DM_TOP1__KME_11_AGENT_ADDR);
	HAL_DM_SetKme12WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = IoReg_Read32(KME_DM_TOP1__KME_12_AGENT_ADDR);
	HAL_DM_SetKme13ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = IoReg_Read32(KME_DM_TOP1__KME_13_AGENT_ADDR);
	HAL_DM_SetKme14WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = IoReg_Read32(KME_DM_TOP1__KME_14_AGENT_ADDR);
	HAL_DM_SetKme15ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = IoReg_Read32(KME_DM_TOP1__KME_15_AGENT_ADDR);
#endif	
#else
	//IP_enable W, need to set enable after ME_WDMA enable ( delay ~2000ns)
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT0);
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT1);
	rtd_clearbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT2);
	//IP_enable R, need to set enable after ME_RDMA enable ( delay ~2000ns)
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT0);
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT1);
	rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT2);
	//rtd_clearbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT3);


	//MCDMA__MC DMA
	HAL_DM_SetKmc00WriteEnable(FALSE);//vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc01WriteEnable(FALSE);//vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc04ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc05ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc06ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc07ReadEnable(FALSE); //vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl 	= 	vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl & 0xfffffffe;

	//MESHARE__ME_WDMA enable, need to enable after ME_WDMA IP enable
	rtd_clearbits(ME_SHARE_DMA_ME_DMA_WR_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl & 0xfffffffe;
	//MESHARE__ME_RDMA enable, need to enable after ME_RDMA IP enable
	rtd_clearbits(ME_SHARE_DMA_ME_DMA_RD_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl & 0xfffffffe;

	//MVinfo DMA disable
	rtd_clearbits(MVINFO_DMA_MVINFO_WR_Ctrl_reg, _BIT0);
	rtd_clearbits(MVINFO_DMA_MVINFO_RD_Ctrl_reg, _BIT0);

	//MV DMA disable
	HAL_DM_SetKmv01WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT & 0xffefffff;   IoReg_Read32(KME_DM_TOP2__MV01_AGENT_ADDR);
	HAL_DM_SetKmv02WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV02_AGENT_ADDR);
	HAL_DM_SetKmv03ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV03_AGENT_ADDR);
	HAL_DM_SetKmv04WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT & 0xffefffff; 	IoReg_Read32(KME_DM_TOP2__MV04_AGENT_ADDR);
	HAL_DM_SetKmv05WriteEnable(FALSE);//vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV05_AGENT_ADDR);
	HAL_DM_SetKmv06ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV06_AGENT_ADDR);
	HAL_DM_SetKmv07ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV07_AGENT_ADDR);
	HAL_DM_SetKmv08ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV08_AGENT_ADDR);
	HAL_DM_SetKmv09ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV09_AGENT_ADDR);
	HAL_DM_SetKmv10ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV10_AGENT_ADDR);
	HAL_DM_SetKmv11ReadEnable(FALSE); //vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT & 0xffefffff;	IoReg_Read32(KME_DM_TOP2__MV11_AGENT_ADDR);
	HAL_DM_SetKmv12ReadEnable(FALSE);

	//ME DMA disable
	HAL_DM_SetKme00WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = IoReg_Read32(KME_DM_TOP0__KME_00_AGENT_ADDR);
	HAL_DM_SetKme01ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = IoReg_Read32(KME_DM_TOP0__KME_01_AGENT_ADDR);
	HAL_DM_SetKme02ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = IoReg_Read32(KME_DM_TOP0__KME_02_AGENT_ADDR);
	HAL_DM_SetKme03ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = IoReg_Read32(KME_DM_TOP0__KME_03_AGENT_ADDR);
	HAL_DM_SetKme04WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = IoReg_Read32(KME_DM_TOP0__KME_04_AGENT_ADDR);
	HAL_DM_SetKme05WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = IoReg_Read32(KME_DM_TOP0__KME_05_AGENT_ADDR);
	HAL_DM_SetKme06ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT & 0xffefffff;		vpq_memc_suspend_resume.KME_DM_TOP0__KME06AGENT =   IoReg_Read32(KME_DM_TOP0__KME06AGENT_ADDR);
	HAL_DM_SetKme07ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = IoReg_Read32(KME_DM_TOP0__KME_07_AGENT_ADDR);
	HAL_DM_SetKme08WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = IoReg_Read32(KME_DM_TOP1__KME_08_AGENT_ADDR);
	HAL_DM_SetKme09ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = IoReg_Read32(KME_DM_TOP1__KME_09_AGENT_ADDR);
	HAL_DM_SetKme10WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = IoReg_Read32(KME_DM_TOP1__KME_10_AGENT_ADDR);
	HAL_DM_SetKme11ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = IoReg_Read32(KME_DM_TOP1__KME_11_AGENT_ADDR);
	HAL_DM_SetKme12WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = IoReg_Read32(KME_DM_TOP1__KME_12_AGENT_ADDR);
	HAL_DM_SetKme13ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = IoReg_Read32(KME_DM_TOP1__KME_13_AGENT_ADDR);
	HAL_DM_SetKme14WriteEnable(FALSE);	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = IoReg_Read32(KME_DM_TOP1__KME_14_AGENT_ADDR);
	HAL_DM_SetKme15ReadEnable(FALSE); 	//vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT & 0xffefffff;	vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = IoReg_Read32(KME_DM_TOP1__KME_15_AGENT_ADDR);
#endif


#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
	time3 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
	do{
		if(((loadscript_cnt_2 - In_cnt_status) < 1) || ((loadscript_output_cnt - Out_cnt_status) < 1)){
			if((rtd_inl(TIMER_SCPU_CLK90K_LO_reg)-time3)>=9000){
				timeout_flag = 1;
			}
		}
	}while((timeout_flag == 0) && ((loadscript_cnt_2-In_cnt_status < 1)||((loadscript_output_cnt - Out_cnt_status) < 1)));

	rtd_pr_memc_notice("[%s][%d][010c,%x,011c,%x,][e0b4,%x,e0b8,%x,][9018,%x, d008,%x,][cnt=,%d,%d,][cnt_diff = %d,%d][time_diff = %d]\n",__func__,__LINE__, rtd_inl(SYS_REG_SYS_SRST3_reg), rtd_inl(SYS_REG_SYS_CLKEN3_reg), rtd_inl(DBUS_WRAPPER_Me_uplimit_addr_reg), rtd_inl(DBUS_WRAPPER_Me_downlimit_addr_reg),
		rtd_inl(KMC_TOP_kmc_top_18_reg), rtd_inl(KPOST_TOP_KPOST_TOP_08_reg), loadscript_cnt_2, loadscript_output_cnt, (loadscript_cnt_2-In_cnt_status), (loadscript_output_cnt - Out_cnt_status), (rtd_inl(TIMER_SCPU_CLK90K_LO_reg)-time3));

	for(u32_i = 0; u32_i < 99; u32_i++){
		for(u32_j = 0; u32_j < 64; u32_j++){
			unsigned int tmp_reg = 0;
			tmp_reg = KMC_TOP_kmc_top_00_reg + (u32_i*256) + (u32_j*4);

			if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \
				|| (u32_i >= 0x20 && u32_i <= 0x2f) || (u32_i >= 0x30 && u32_i <= 0x3e) \
				|| u32_i == 0x40 || u32_i == 0x41 || u32_i == 0x44 || u32_i == 0x45 || u32_i == 0x46 || u32_i == 0x47 || u32_i == 0x48 || u32_i == 0x49\
				|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x56 || u32_i == 0x57 || u32_i == 0x58 || u32_i == 0x59 \
				|| u32_i == 0x5b || u32_i == 0x5c || u32_i == 0x5d || u32_i == 0x5e || u32_i == 0x5f \
				|| u32_i == 0x60 || u32_i == 0x61 || u32_i == 0x62 || u32_i == 0x63){
				vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = IoReg_Read32(tmp_reg);
			}else{
				vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = 0;
			}
		}
	}
#else
	if(MEMC_Lib_get_runtime_suspend_flag() == 1){
		MEMC_Lib_set_runtime_suspend_flag(0);
	}
	else{//k24
		for(u32_i = 0; u32_i <= MAX_PAGE_NUMBER; u32_i++){
			for(u32_j = 0; u32_j < 64; u32_j++){
				unsigned int tmp_reg = 0;
				tmp_reg = KMC_TOP_kmc_top_00_reg + (u32_i*256) + (u32_j*4);

				if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \
					|| (u32_i >= 0x20 && u32_i <= 0x2f) \
					|| (u32_i >= 0x30 && u32_i <= 0x3e) \
					|| (u32_i >= 0x40 && u32_i <= 0x49) \
					|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x53 || u32_i == 0x54 || (u32_i >= 0x56 && u32_i <= 0x5f) \
					|| (u32_i >= 0x60 && u32_i <= 0x6a)){
					vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = IoReg_Read32(tmp_reg);
				}else{
					vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = 0;
				}								
			}
		}
		rtd_pr_memc_notice("[MEMC] suspend test [%x,%x] \n", IoReg_Read32(SYS_REG_SYS_CLKEN3_reg),  IoReg_Read32(SYS_REG_SYS_SRST3_reg));
	}
#endif

	for(i=0;i<10;i++){
		MEMC_instanboot_resume_check[i]=0;
	}
	MEMC_resume_check = 0;
	
	rtd_pr_memc_notice("[suspend_EEE][%x, %x, %x, %x, %x]\n\r", rtd_inl(0xb8099414), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(KPOST_TOP_KPOST_TOP_08_reg));
	rtd_pr_memc_notice("[suspend_K25844][%x, %x, %x, %x, %x][%x, %x, %x, %x, %x]\n\r",MEMC_instanboot_resume_check[0],MEMC_instanboot_resume_check[1],MEMC_instanboot_resume_check[2],MEMC_instanboot_resume_check[3],MEMC_instanboot_resume_check[4]
																			,MEMC_instanboot_resume_check[5],MEMC_instanboot_resume_check[6],MEMC_instanboot_resume_check[7],MEMC_instanboot_resume_check[8],MEMC_instanboot_resume_check[9]);		
	rtd_pr_memc_notice("[MEMC]disp_suspend_done[%x,%x] \n", IoReg_Read32(SYS_REG_SYS_CLKEN3_reg),  IoReg_Read32(SYS_REG_SYS_SRST3_reg));

#endif
#endif
#endif
}

void vpq_memc_runtime_suspend_flow(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
				return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	unsigned int u32_interrupt_reg = 0;
	unsigned int u32_i, u32_j;
	unsigned int i=0;
	rtd_pr_memc_notice("[%s][%d]\n", __func__, __LINE__);

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return;
//#endif

	if(Scaler_MEMC_CLK_Check() == FALSE){
		Scaler_MEMC_CLK_Enable();
		Scaler_MEMC_PowerSaving_Disable();//PowerSaving Enable when MEMC CLK off
		//Scaler_MEMC_MEMC_CLK_OnOff(1, 1, 1); // MEMC must CLK on when DC off
	}

	// disable interrupt
	//reg_kmc_int_en
	u32_interrupt_reg = IoReg_Read32(KMC_TOP_kmc_top_18_reg);
	u32_interrupt_reg &= 0xf0ffffff;
	IoReg_Write32(KMC_TOP_kmc_top_18_reg, u32_interrupt_reg);

	//reg_post_int_en
	#if 1
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_HSYNC) == 1){
		MEMC_LibOutpuHSInterruptEnable(FALSE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT5);
	}
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_VSYNC) == 1){
		MEMC_LibOutputVSInterruptEnable(FALSE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT6);
	}
	#else
	u32_interrupt_reg = IoReg_Read32(KPOST_TOP_KPOST_TOP_08_reg);
	u32_interrupt_reg &= 0xfffffe1f;
	IoReg_Write32(KPOST_TOP_KPOST_TOP_08_reg, u32_interrupt_reg);
	#endif

#if 1
	//MEMC mux
	vpq_memc_suspend_resume.MEMC_MUX_MEMC_MUX_FUNC_CTRL_SUSPEND = IoReg_Read32(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg);
	//FBG
	vpq_memc_suspend_resume.PPOVERLAY_MEMC_MUX_CTRL_SUSPEND = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_reg);
	//clock
	vpq_memc_suspend_resume.SYS_REG_SYS_DISPCLKSEL_SUSPEND = IoReg_Read32(SYS_REG_SYS_DISPCLKSEL_reg);
	//vpq_memc_suspend_resume.MEMC_LATENCY01 = IoReg_Read32(MEMC_LATENCY01_ADDR);
	//vpq_memc_suspend_resume.MEMC_LATENCY02 = IoReg_Read32(MEMC_LATENCY02_ADDR);   //  remove DC2

	Mid_Mode_DisableMCDMA();
	Mid_Mode_DisableMEDMA(0);

	rtd_pr_memc_notice("[%s][%d][,%x,]\n", __FUNCTION__, __LINE__, rtd_inl(SOFTWARE2_SOFTWARE2_63_reg));
	for(u32_i = 0; u32_i <= MAX_PAGE_NUMBER; u32_i++){
		for(u32_j = 0; u32_j < 64; u32_j++){
			unsigned int tmp_reg = 0;
			tmp_reg = KMC_TOP_kmc_top_00_reg + (u32_i*256) + (u32_j*4);

#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
			if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \
				|| (u32_i >= 0x20 && u32_i <= 0x2f) || (u32_i >= 0x30 && u32_i <= 0x3e) \
				|| u32_i == 0x40 || u32_i == 0x41 || u32_i == 0x44 || u32_i == 0x45 || u32_i == 0x46 || u32_i == 0x47 || u32_i == 0x48 || u32_i == 0x49\
				|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x56 || u32_i == 0x57 || u32_i == 0x58 || u32_i == 0x59 \
				|| u32_i == 0x5b || u32_i == 0x5c || u32_i == 0x5d || u32_i == 0x5e || u32_i == 0x5f \
				|| u32_i == 0x60 || u32_i == 0x61 || u32_i == 0x62 || u32_i == 0x63){
				vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = IoReg_Read32(tmp_reg);
			}else{
				vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = 0;
			}
#else//k24
			if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \
				|| (u32_i >= 0x20 && u32_i <= 0x2f) \
				|| (u32_i >= 0x30 && u32_i <= 0x3e) \
				|| (u32_i >= 0x40 && u32_i <= 0x49) \
				|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x53 || u32_i == 0x54 || (u32_i >= 0x56 && u32_i <= 0x5f) \
				|| (u32_i >= 0x60 && u32_i <= 0x6a)){
				vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = IoReg_Read32(tmp_reg);
			}else{
				vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j] = 0;
			}
#endif

		}
	}
	rtd_pr_memc_notice("[%s][%d][,%x,]\n", __FUNCTION__, __LINE__, rtd_inl(SOFTWARE2_SOFTWARE2_63_reg));

	MEMC_Lib_set_runtime_suspend_flag(1);

	for(i=0;i<10;i++){
		MEMC_instanboot_resume_check[i]=0;
	}
	MEMC_resume_check = 0;

		rtd_pr_memc_notice("[suspend_K25844][%x, %x, %x, %x, %x][%x, %x, %x, %x, %x]\n\r",MEMC_instanboot_resume_check[0],MEMC_instanboot_resume_check[1],MEMC_instanboot_resume_check[2],MEMC_instanboot_resume_check[3],MEMC_instanboot_resume_check[4]
																			,MEMC_instanboot_resume_check[5],MEMC_instanboot_resume_check[6],MEMC_instanboot_resume_check[7],MEMC_instanboot_resume_check[8],MEMC_instanboot_resume_check[9]);	
	//rtd_pr_memc_notice("[%s][%x, %x, %x, %x, %x]\n\r", __FUNCTION__, rtd_inl(0xb8099414), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(0xb809d7fc));
	rtd_pr_memc_notice("[%s]runtime_suspend_done[%x,%x] \n", __FUNCTION__, IoReg_Read32(SYS_REG_SYS_CLKEN3_reg),  IoReg_Read32(SYS_REG_SYS_SRST3_reg));

#endif
#endif
#endif
}

unsigned char MEMC_instanboot_resume_done_jerry = 0;
extern unsigned char u1_DoPowerSaving;
void vpq_memc_disp_resume_instanboot(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else

#if 1//#ifdef KID23_TO_WEBOS

	ppoverlay_memc_mux_ctrl_bg_RBUS memc_mux_ctrl_bg_reg;
	unsigned int u32_interrupt_reg = 0;
	unsigned int u32_i,u32_j;	
	rtd_pr_memc_notice("[MEMC]%s %d\n",__func__,__LINE__);

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return;
//#endif

	//for MEMC CLK check
	if(Scaler_MEMC_CLK_Check() == FALSE){
		Scaler_MEMC_CLK_Enable();
	}

	rtd_pr_memc_notice("[resume_DDD][%x, %x, %x, %x, %x]\n\r", rtd_inl(0xb8099414), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(SOFTWARE2_SOFTWARE2_63_reg));

#if 1   //Load part registers dump

	for(u32_i = 0; u32_i <= MAX_PAGE_NUMBER; u32_i++){
		for(u32_j = 0; u32_j < 64; u32_j++){
			unsigned int tmp_reg = 0;
			tmp_reg = KMC_TOP_kmc_top_00_reg + (u32_i*256) + (u32_j*4);

#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
			if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \

				|| (u32_i >= 0x20 && u32_i <= 0x2f) || (u32_i >= 0x30 && u32_i <= 0x3e) \
				|| u32_i == 0x40 || u32_i == 0x41 || u32_i == 0x44 || u32_i == 0x45 || u32_i == 0x46 || u32_i == 0x47 || u32_i == 0x48 || u32_i == 0x49\
				|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x56 || u32_i == 0x57 || u32_i == 0x58 || u32_i == 0x59 \
				|| u32_i == 0x5b || u32_i == 0x5c || u32_i == 0x5d || u32_i == 0x5e || u32_i == 0x5f \
				|| u32_i == 0x60 || u32_i == 0x61 || u32_i == 0x62 || u32_i == 0x63){

				IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}else{
				//IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}
#else//k24
			if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \
				|| (u32_i >= 0x20 && u32_i <= 0x2f) \
				|| (u32_i >= 0x30 && u32_i <= 0x3e) \
				|| (u32_i >= 0x40 && u32_i <= 0x49) \
				|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x53 || u32_i == 0x54 || (u32_i >= 0x56 && u32_i <= 0x5f) \
				|| (u32_i >= 0x60 && u32_i <= 0x6a)){
				IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}else{
				//IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}
#endif


		}
	}
	rtd_pr_memc_notice("[resume_CCC][%x, %x, %x, %x, %x]\n\r", rtd_inl(0xb8099414), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(SOFTWARE2_SOFTWARE2_63_reg));

#endif


	//MEMC resume setting
	Scaler_MEMC_CLK();

	//disable double buffer
	{
		ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;
		ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
		ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en = 0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);

		//enable MEMC output fbg
		memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
		memc_mux_ctrl_bg_reg.memc_out_bg_en = TRUE;
		IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, memc_mux_ctrl_bg_reg.regValue);
	}


#if RTK_MEMC_Performance_tunging_from_tv001
	#if CONFIG_MEMC_BOUNDARY_AUTO
	// already set the boundary by vpq_memc_suspend_resume
#else
	#if 0
	//ME memory boundary
	IoReg_Write32(DBUS_WRAPPER_Me_downlimit_addr_reg, gphy_addr_kme);
	IoReg_Write32(DBUS_WRAPPER_Me_uplimit_addr_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	//ME_SHARE memory boundary
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	//MVINFO memory boundary
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_up_reg, gphy_addr_kmc00);
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_up_reg, gphy_addr_kmc00);
	//MC memory boundary
	#if 0
	#if CONFIG_MC_8_BUFFER
	if(Get_DISPLAY_REFRESH_RATE() <= 60){
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_8bit));	
	}else{
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_10bit));	
	}
	#else
	Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_6buf_10bit));	
	#endif
	#else
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + Get_KMC_TOTAL_SIZE())); 
	#endif
	#endif
#endif
#else
	
	#if 0
	//ME memory boundary
	IoReg_Write32(DBUS_WRAPPER_Me_downlimit_addr_reg, gphy_addr_kme);
	IoReg_Write32(DBUS_WRAPPER_Me_uplimit_addr_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	//ME_SHARE memory boundary
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	//MVINFO memory boundary
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_up_reg, gphy_addr_kmc00);
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_up_reg, gphy_addr_kmc00);
	//MC memory boundary
	#if CONFIG_MC_8_BUFFER
	if(Get_DISPLAY_REFRESH_RATE() <= 60 && !vbe_disp_get_force_60hz_panel_type()){
		Scaler_MEMC_MC_Set_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_8bit));
	}else{
		Scaler_MEMC_MC_Set_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_10bit));
	}
	#else
	Scaler_MEMC_MC_Set_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_6buf_10bit));
	#endif
	#endif
#endif

	rtd_pr_memc_notice("[resume_BBB][%x, %x, %x, %x, %x]\n\r", rtd_inl(0xb8099414), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(0xb809d7fc));


#if RTK_MEMC_Performance_tunging_from_tv001
	#if 1
	Mid_Mode_EnableMCDMA();
	Mid_Mode_EnableMEDMA();
#else
	//MCDMA__MC DMA disable
	HAL_DM_SetKmc00WriteEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc01WriteEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc04ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc05ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc06ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc07ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl & 0xfffffffe;

	//MESHARE__ME_WDMA enable, need to enable after ME_WDMA IP enable
	rtd_setbits(ME_SHARE_DMA_ME_DMA_WR_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl & 0xfffffffe;
	//rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT0);
	//rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT1);
	//rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT2);	

	//MESHARE__ME_RDMA enable, need to enable after ME_RDMA IP enable
	rtd_setbits(ME_SHARE_DMA_ME_DMA_RD_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl & 0xfffffffe;
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT0);
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT1);
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT2);
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT3);

	//MVinfo DMA disable
	rtd_setbits(MVINFO_DMA_MVINFO_WR_Ctrl_reg, _BIT0);
	rtd_setbits(MVINFO_DMA_MVINFO_RD_Ctrl_reg, _BIT0);


	//MV DMA disable
	HAL_DM_SetKmv01WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT & 0xffefffff;
	HAL_DM_SetKmv02WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT & 0xffefffff;
	HAL_DM_SetKmv03ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT & 0xffefffff;
	HAL_DM_SetKmv04WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT & 0xffefffff;
	HAL_DM_SetKmv05WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT & 0xffefffff;
	HAL_DM_SetKmv06ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT & 0xffefffff;
	HAL_DM_SetKmv07ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT & 0xffefffff;
	HAL_DM_SetKmv08ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT & 0xffefffff;
	HAL_DM_SetKmv09ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT & 0xffefffff;
	HAL_DM_SetKmv10ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT & 0xffefffff;
	HAL_DM_SetKmv11ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT & 0xffefffff;
	HAL_DM_SetKmv12ReadEnable(TRUE);

	//ME DMA disable
	HAL_DM_SetKme00WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT & 0xffefffff;
	HAL_DM_SetKme01ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT & 0xffefffff;
	HAL_DM_SetKme02ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT & 0xffefffff;
	HAL_DM_SetKme03ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT & 0xffefffff;
	HAL_DM_SetKme04WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT & 0xffefffff;
	HAL_DM_SetKme05WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT & 0xffefffff;
	HAL_DM_SetKme06ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_06_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_06_AGENT & 0xffefffff;
	HAL_DM_SetKme07ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT & 0xffefffff;
	HAL_DM_SetKme08WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT & 0xffefffff;
	HAL_DM_SetKme09ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT & 0xffefffff;
	HAL_DM_SetKme10WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT & 0xffefffff;
	HAL_DM_SetKme11ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT & 0xffefffff;
	HAL_DM_SetKme12WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT & 0xffefffff;
	HAL_DM_SetKme13ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT & 0xffefffff;
	HAL_DM_SetKme14WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT & 0xffefffff;
	HAL_DM_SetKme15ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT & 0xffefffff;

	//IP_enable W, need to set enable after ME_WDMA enable ( delay ~2000ns)
	rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT0);
	rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT1);
	rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT2);
	
	//IP_enable R,  need to set enable after ME_WDMA enable ( delay ~2000ns)
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT0);
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT1);
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT2);
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT3);
	
#endif

#else
	#if 1
	//MCDMA__MC DMA disable
	HAL_DM_SetKmc00WriteEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_LF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc01WriteEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_HF_DMA_WR_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc04ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_LF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc05ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_HF_I_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc06ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_LF_P_DMA_RD_Ctrl & 0xfffffffe;
	HAL_DM_SetKmc07ReadEnable(TRUE); // vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl = vpq_memc_suspend_resume.MCDMA__MC_HF_P_DMA_RD_Ctrl & 0xfffffffe;

	//MESHARE__ME_WDMA enable, need to enable after ME_WDMA IP enable
	rtd_setbits(ME_SHARE_DMA_ME_DMA_WR_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_WR_Ctrl & 0xfffffffe;
	//rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT0);
	//rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT1);
	//rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT2);	

	//MESHARE__ME_RDMA enable, need to enable after ME_RDMA IP enable
	rtd_setbits(ME_SHARE_DMA_ME_DMA_RD_Ctrl_reg, _BIT0);//vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl 	=	vpq_memc_suspend_resume.MESHARE__ME_DMA_RD_Ctrl & 0xfffffffe;
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT0);
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT1);
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT2);
	//rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT3);
	

	//MVinfo DMA disable
	rtd_setbits(MVINFO_DMA_MVINFO_WR_Ctrl_reg, _BIT0);
	rtd_setbits(MVINFO_DMA_MVINFO_RD_Ctrl_reg, _BIT0);

	//MV DMA enable
	HAL_DM_SetKmv01WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV01_AGENT & 0xffefffff;
	HAL_DM_SetKmv02WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV02_AGENT & 0xffefffff;
	HAL_DM_SetKmv03ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV03_AGENT & 0xffefffff;
	HAL_DM_SetKmv04WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV04_AGENT & 0xffefffff;
	HAL_DM_SetKmv05WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV05_AGENT & 0xffefffff;
	HAL_DM_SetKmv06ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV06_AGENT & 0xffefffff;
	HAL_DM_SetKmv07ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV07_AGENT & 0xffefffff;
	HAL_DM_SetKmv08ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV08_AGENT & 0xffefffff;
	HAL_DM_SetKmv09ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV09_AGENT & 0xffefffff;
	HAL_DM_SetKmv10ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV10_AGENT & 0xffefffff;
	HAL_DM_SetKmv11ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP2__MV11_AGENT & 0xffefffff;
	HAL_DM_SetKmv12ReadEnable(TRUE);

	//ME DMA enable
	HAL_DM_SetKme00WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_00_AGENT & 0xffefffff;
	HAL_DM_SetKme01ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_01_AGENT & 0xffefffff;
	HAL_DM_SetKme02ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_02_AGENT & 0xffefffff;
	HAL_DM_SetKme03ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_03_AGENT & 0xffefffff;
	HAL_DM_SetKme04WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_04_AGENT & 0xffefffff;
	HAL_DM_SetKme05WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_05_AGENT & 0xffefffff;
	HAL_DM_SetKme06ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_06_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_06_AGENT & 0xffefffff;
	HAL_DM_SetKme07ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT = vpq_memc_suspend_resume.KME_DM_TOP0__KME_07_AGENT & 0xffefffff;
	HAL_DM_SetKme08WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_08_AGENT & 0xffefffff;
	HAL_DM_SetKme09ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_09_AGENT & 0xffefffff;
	HAL_DM_SetKme10WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_10_AGENT & 0xffefffff;
	HAL_DM_SetKme11ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_11_AGENT & 0xffefffff;
	HAL_DM_SetKme12WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_12_AGENT & 0xffefffff;
	HAL_DM_SetKme13ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_13_AGENT & 0xffefffff;
	HAL_DM_SetKme14WriteEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_14_AGENT & 0xffefffff;
	HAL_DM_SetKme15ReadEnable(TRUE); // vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT = vpq_memc_suspend_resume.KME_DM_TOP1__KME_15_AGENT & 0xffefffff;

	//IP_enable W, need to set enable after ME_WDMA enable ( delay ~2000ns)
	rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT0);
	rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT1);
	rtd_setbits(ME_SHARE_DMA_KME_WR_client_en_reg, _BIT2);
	
	//IP_enable R,  need to set enable after ME_WDMA enable ( delay ~2000ns)
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT0);
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT1);
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT2);
	rtd_setbits(ME_SHARE_DMA_KME_RD_client_en_reg, _BIT3);
	
	#endif
#endif



	rtd_pr_memc_notice("[resume_AAA][%x, %x, %x, %x, %x]\n\r", rtd_inl(0xb802e800), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(0xb809d7fc));

	//MEMC mux enable
	if((rtd_inl(0xb802e800)&0x1) == 0){
		drvif_memc_mux_en(TRUE);
	}
	MEMC_instanboot_resume_check[0]=1;
	MEMC_instanboot_resume_Done = 1;
	MEMC_instanboot_resume_done_jerry = 1;
	u1_DoPowerSaving = 0;
	rtd_pr_memc_notice("[PS_resume_vpqmemc]\n");

	// enable interrupt
	//reg_kmc_int_en
	u32_interrupt_reg = IoReg_Read32(KMC_TOP_kmc_top_18_reg);
	u32_interrupt_reg |= 0x02000000;
	IoReg_Write32(KMC_TOP_kmc_top_18_reg, u32_interrupt_reg);

	//reg_post_int_en		
#if 0
	rtd_pr_memc_notice("[resume_A][%x, %x, %x, %x, %x][%x]\n\r", rtd_inl(0xb802e800), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(KPOST_TOP_KPOST_TOP_08_reg),u32_interrupt_reg);
	
	u32_interrupt_reg = IoReg_Read32(KPOST_TOP_KPOST_TOP_08_reg);
	u32_interrupt_reg |= 0x0000001e; // wclr
	IoReg_Write32(KPOST_TOP_KPOST_TOP_08_reg, u32_interrupt_reg);
#endif
	rtd_pr_memc_notice("[resume_AA][%x, %x, %x, %x, %x][%x][%d]\n\r", rtd_inl(0xb802e800), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(KPOST_TOP_KPOST_TOP_08_reg),u32_interrupt_reg,MEMC_resume_check);

	#if 1
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_HSYNC) == 0){
		MEMC_LibOutpuHSInterruptEnable(TRUE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT5);
	}
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_VSYNC) == 0){
		MEMC_LibOutputVSInterruptEnable(TRUE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT6);
	}
	#else
	u32_interrupt_reg = IoReg_Read32(KPOST_TOP_KPOST_TOP_08_reg);
	u32_interrupt_reg &= 0xffffffe1; // wclr
	u32_interrupt_reg |= 0x00000060;
	IoReg_Write32(KPOST_TOP_KPOST_TOP_08_reg, u32_interrupt_reg);
	#endif

	MEMC_resume_check = 1;
	rtd_pr_memc_notice("[resume_AAA][%x, %x, %x, %x, %x][%x]\n\r", rtd_inl(0xb802e800), rtd_inl(0xb8099484), rtd_inl(0xb809d400), rtd_inl(0xb809d5fc), rtd_inl(KPOST_TOP_KPOST_TOP_08_reg),u32_interrupt_reg);
	rtd_pr_memc_notice("[resume_K25844][%x, %x, %x, %x, %x][%x, %x, %x, %x, %x][%d]\n\r",MEMC_instanboot_resume_check[0],MEMC_instanboot_resume_check[1],MEMC_instanboot_resume_check[2],MEMC_instanboot_resume_check[3],MEMC_instanboot_resume_check[4]
																			,MEMC_instanboot_resume_check[5],MEMC_instanboot_resume_check[6],MEMC_instanboot_resume_check[7],MEMC_instanboot_resume_check[8],MEMC_instanboot_resume_check[9]
																			,MEMC_resume_check);	
	

	//enable lbmc db
	MEMC_LibSet_LBMC_DB_en(1);
	//PQC DB enable
	MEMC_LibSet_PQC_DB_en(1);
	//PQC DB apply
	MEMC_LibSet_PQC_DB_apply();
	//MCDMA DB enable
	MEMC_LibSet_MCDMA_DB_en(1);
	//rtd_pr_memc_notice("[MEMC]Instanboot Resume done !!!\n");
	rtd_pr_memc_notice("[MEMC][%s]Instanboot Resume done\n", __FUNCTION__);
#endif
#endif
}

void vpq_memc_runtime_resume_flow(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return;
#else


#if 1//#ifdef KID23_TO_WEBOS

	ppoverlay_memc_mux_ctrl_bg_RBUS memc_mux_ctrl_bg_reg;
	UINT32 u32_interrupt_reg = 0;
	UINT32 u32_i,u32_j;
	rtd_pr_memc_notice("[%s][%d]\n", __func__, __LINE__);

//#ifdef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP)
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return;
//#endif

	//for MEMC CLK check
	if(Scaler_MEMC_CLK_Check() == FALSE){
		Scaler_MEMC_CLK_Enable();
	}

	rtd_pr_memc_notice("[%s][%d][,%x,]\n", __FUNCTION__, __LINE__, rtd_inl(SOFTWARE2_SOFTWARE2_63_reg));
#if 1   //Load part registers dump
	for(u32_i = 0; u32_i <= MAX_PAGE_NUMBER; u32_i++){
		for(u32_j = 0; u32_j < 64; u32_j++){
			unsigned int tmp_reg = 0;
			tmp_reg = KMC_TOP_kmc_top_00_reg + (u32_i*256) + (u32_j*4);
		
#if 0//#if RTK_MEMC_Performance_tunging_from_tv001
			if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \
				|| (u32_i >= 0x20 && u32_i <= 0x2f) || (u32_i >= 0x30 && u32_i <= 0x3e) \
				|| u32_i == 0x40 || u32_i == 0x41 || u32_i == 0x44 || u32_i == 0x45 || u32_i == 0x46 || u32_i == 0x47 || u32_i == 0x48 || u32_i == 0x49\
				|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x56 || u32_i == 0x57 || u32_i == 0x58 || u32_i == 0x59 \
				|| u32_i == 0x5b || u32_i == 0x5c || u32_i == 0x5d || u32_i == 0x5e || u32_i == 0x5f \
				|| u32_i == 0x60 || u32_i == 0x61 || u32_i == 0x62 || u32_i == 0x63){
				IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}else{
				//IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}
#else//k24
			if(u32_i <= 0xc || u32_i == 0xf || u32_i == 0x1e \
				|| (u32_i >= 0x20 && u32_i <= 0x2f) \
				|| (u32_i >= 0x30 && u32_i <= 0x3e) \
				|| (u32_i >= 0x40 && u32_i <= 0x49) \
				|| u32_i == 0x50 || u32_i == 0x51 || u32_i == 0x53 || u32_i == 0x54 || (u32_i >= 0x56 && u32_i <= 0x5f) \
				|| (u32_i >= 0x60 && u32_i <= 0x6a)){
				IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}else{
				//IoReg_Write32(tmp_reg, vpq_memc_suspend_resume.MEMC_reg[u32_i][u32_j]);
			}
#endif


		}
	}
#endif
	rtd_pr_memc_notice("[%s][%d][,%x,]\n", __FUNCTION__, __LINE__, rtd_inl(SOFTWARE2_SOFTWARE2_63_reg));

	//MEMC resume setting
	Scaler_MEMC_CLK();

	//disable double buffer
	{
		ppoverlay_double_buffer_ctrl3_RBUS ppoverlay_double_buffer_ctrl3_reg;
		ppoverlay_double_buffer_ctrl3_reg.regValue =  IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL3_reg);
		ppoverlay_double_buffer_ctrl3_reg.d2bg_dtgreg_dbuf_en = 0;
		IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL3_reg, ppoverlay_double_buffer_ctrl3_reg.regValue);

		//enable MEMC output fbg
		memc_mux_ctrl_bg_reg.regValue = IoReg_Read32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg);
		memc_mux_ctrl_bg_reg.memc_out_bg_en = TRUE;
		IoReg_Write32(PPOVERLAY_MEMC_MUX_CTRL_BG_reg, memc_mux_ctrl_bg_reg.regValue);
	}

#if RTK_MEMC_Performance_tunging_from_tv001
	#if CONFIG_MEMC_BOUNDARY_AUTO
	// already set the boundary by vpq_memc_suspend_resume
#else
	#if 0
	//ME memory boundary
	IoReg_Write32(DBUS_WRAPPER_Me_downlimit_addr_reg, gphy_addr_kme);
	IoReg_Write32(DBUS_WRAPPER_Me_uplimit_addr_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	//ME_SHARE memory boundary
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	//MVINFO memory boundary
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_up_reg, gphy_addr_kmc00);
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_up_reg, gphy_addr_kmc00);
	//MC memory boundary
	#if 0
	#if CONFIG_MC_8_BUFFER
	if(Get_DISPLAY_REFRESH_RATE() <= 60){
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_8bit));
	}else{
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_10bit));
	}
	#else
	Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_6buf_10bit));
	#endif
	#else
		Scaler_MEMC_Set_MC_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + Get_KMC_TOTAL_SIZE())); 
	#endif
	#endif
#endif
#else
	#if 0
	//ME memory boundary
	IoReg_Write32(DBUS_WRAPPER_Me_downlimit_addr_reg, gphy_addr_kme);
	IoReg_Write32(DBUS_WRAPPER_Me_uplimit_addr_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	//ME_SHARE memory boundary
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_WR_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(ME_SHARE_DMA_ME_DMA_RD_Rule_check_up_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	//MVINFO memory boundary
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_low_reg, (gphy_addr_kme + ME_TOTAL_SIZE + ME_SHARE_TOTAL_SIZE));
	IoReg_Write32(MVINFO_DMA_MVINFO_WR_Rule_check_up_reg, gphy_addr_kmc00);
	IoReg_Write32(MVINFO_DMA_MVINFO_RD_Rule_check_up_reg, gphy_addr_kmc00);
	//MC memory boundary
	#if CONFIG_MC_8_BUFFER
	if(Get_DISPLAY_REFRESH_RATE() <= 60 && !vbe_disp_get_force_60hz_panel_type()){
		Scaler_MEMC_MC_Set_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_8bit));
	}else{
		Scaler_MEMC_MC_Set_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_8buf_10bit));
	}
	#else
	Scaler_MEMC_MC_Set_MEMBoundary(gphy_addr_kmc00, (gphy_addr_kmc00 + KMC_TOTAL_SIZE_6buf_10bit));
	#endif
	#endif
#endif


	Mid_Mode_EnableMCDMA();
	Mid_Mode_EnableMEDMA();

	//MEMC mux enable
	if((rtd_inl(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg)&0x1) == 0){
		drvif_memc_mux_en(TRUE);
	}

	MEMC_instanboot_resume_check[6]=1;
	MEMC_instanboot_resume_Done = 1;
	MEMC_instanboot_resume_done_jerry = 1;
	u1_DoPowerSaving = 0;
	rtd_pr_memc_notice("[PS_resume_vpqmemc]\n");

	// enable interrupt
	//reg_kmc_int_en
	u32_interrupt_reg = IoReg_Read32(KMC_TOP_kmc_top_18_reg);
	u32_interrupt_reg |= 0x02000000;
	IoReg_Write32(KMC_TOP_kmc_top_18_reg, u32_interrupt_reg);

	//reg_post_int_en
	#if 1
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_HSYNC) == 0){
		MEMC_LibOutpuHSInterruptEnable(TRUE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT5);
	}
	if(Mid_MISC_GetOutINTEnable(INT_TYPE_VSYNC) == 0){
		MEMC_LibOutputVSInterruptEnable(TRUE); // rtd_setbits(KPOST_TOP_KPOST_TOP_08_reg, _BIT6);
	}
	#else
	u32_interrupt_reg = IoReg_Read32(KPOST_TOP_KPOST_TOP_08_reg);
	u32_interrupt_reg |= 0x00000060;
	IoReg_Write32(KPOST_TOP_KPOST_TOP_08_reg, u32_interrupt_reg);
	#endif

	rtd_pr_memc_notice("[resume_K25844][%x, %x, %x, %x, %x][%x, %x, %x, %x, %x][%d]\n\r",MEMC_instanboot_resume_check[0],MEMC_instanboot_resume_check[1],MEMC_instanboot_resume_check[2],MEMC_instanboot_resume_check[3],MEMC_instanboot_resume_check[4]
																			,MEMC_instanboot_resume_check[5],MEMC_instanboot_resume_check[6],MEMC_instanboot_resume_check[7],MEMC_instanboot_resume_check[8],MEMC_instanboot_resume_check[9]
																			,MEMC_resume_check);	

	//enable lbmc db
	MEMC_LibSet_LBMC_DB_en(1);
	//PQC DB enable
	MEMC_LibSet_PQC_DB_en(1);
	//PQC DB apply
	MEMC_LibSet_PQC_DB_apply();
	//MCDMA DB enable
	MEMC_LibSet_MCDMA_DB_en(1);

	MEMC_Lib_set_runtime_suspend_flag(0);

	//rtd_pr_memc_notice("[MEMC]Instanboot Resume done !!!\n");
	rtd_pr_memc_notice("[%s]Instanboot Resume done\n", __FUNCTION__);
	
#endif
#endif

}
#endif

unsigned char memc_ioctl_cmd_stop[32] = {0};
unsigned char vpq_memc_ioctl_get_stop_run(unsigned int cmd)
{
	return (memc_ioctl_cmd_stop[_IOC_NR(cmd)&0x1f]|memc_ioctl_cmd_stop[0]);
}

unsigned char vpq_memc_ioctl_get_stop_run_by_idx(unsigned char cmd_idx)
{
	return memc_ioctl_cmd_stop[cmd_idx];
}

void vpq_memc_ioctl_set_stop_run_by_idx(unsigned char cmd_idx, unsigned char stop)
{
	memc_ioctl_cmd_stop[cmd_idx] = stop;
}

int vpq_memc_open(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t  vpq_memc_read(struct file *filep, char *buffer, size_t count, loff_t *offp)
{
	return 0;
}

ssize_t vpq_memc_write(struct file *filep, const char *buffer, size_t count, loff_t *offp)
{
	return 0;
}

int vpq_memc_release(struct inode *inode, struct file *filep)
{
	return 0;
}

long vpq_memc_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else


	int retval = 0;
	//VPQ_MEMC_SETVIDEOBLOCK_T set_video_block_info;
	//rtd_pr_memc_debug( "VPQ MEMC: vpq_memc_ioctl, %x;;agr:%x\n", cmd,*((unsigned int *)arg));
	if (_IOC_TYPE(cmd) != VPQ_IOC_MEMC_MAGIC || _IOC_NR(cmd) > VPQ_MEMC_IOC_MAXNR) return -ENOTTY ;

	if (vpq_memc_ioctl_get_stop_run(cmd))
		return 0;

	switch (cmd)
	{
		case VPQ_IOC_MEMC_INITILIZE:
		{
			rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_INITILIZE\n");
			HAL_VPQ_MEMC_Initialize();

			break;
		}
		case VPQ_IOC_MEMC_UNINITILIZE:
		{
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_UNINITILIZE\n");
			HAL_VPQ_MEMC_Uninitialize();

			break;
		}
		case VPQ_IOC_MEMC_SETMOTIONCOMP:
		{
			VPQ_MEMC_SETMOTIONCOMP_T set_motion_comp_info;
			rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_SETMOTIONCOMP\n");
			if(copy_from_user((void *)&set_motion_comp_info, (const void __user *)arg, sizeof(VPQ_MEMC_SETMOTIONCOMP_T)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETMOTIONCOMP failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				retval = HAL_VPQ_MEMC_SetMotionComp(set_motion_comp_info.blurLevel, set_motion_comp_info.judderLevel, set_motion_comp_info.motion);
			}

			break;
		}
		case VPQ_IOC_MEMC_SETBLURLEVEL:
		{
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_SETBLURLEVEL\n");
			unsigned char blurLevel;
			if(copy_from_user((void *)&blurLevel, (const void __user *)arg, sizeof(UINT8)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETBLURLEVEL failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_SetBlurLevel(blurLevel);
			}

			break;
		}
		case VPQ_IOC_MEMC_SETJUDDERLEVEL:
		{
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_SETJUDDERLEVEL\n");
			unsigned char judderLevel;
			if(copy_from_user((void *)&judderLevel, (const void __user *)arg, sizeof(UINT8)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETJUDDERLEVEL failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_SetJudderLevel(judderLevel);
			}

			break;
		}
#if RTK_MEMC_Performance_tunging_from_tv001//xj//m7 ui下設定是透過gVIP_Table 兩邊DejudderLevel,DeblurLevel有些微差異
		case VPQ_IOC_MEMC_SETBLUR_UILEVEL: 
		{
			//rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_SETBLUR_UILEVEL\n");
			unsigned char blurLevel;
			if(copy_from_user((void *)&blurLevel, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETBLUR_UILEVEL failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				rtd_pr_memc_notice("##############[MEMC]VPQ_IOC_MEMC_SETBLUR_UILEVEL, UI_blurLevel = %d\n", blurLevel);
				VPQ_MEMC_SetBlurUILevel(blurLevel);
			}

			break;
		}
		case VPQ_IOC_MEMC_SETJUDDER_UILEVEL:
		{
			//rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_SETJUDDER_UILEVEL\n");
			unsigned char judderLevel;
			if(copy_from_user((void *)&judderLevel, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETJUDDER_UILEVEL failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				rtd_pr_memc_notice("##############[MEMC]VPQ_IOC_MEMC_SETJUDDER_UILEVEL, UI_judderLevel = %d\n", judderLevel);
				VPQ_MEMC_SetJudderUILevel(judderLevel);
			}

			break;
		}
		case VPQ_IOC_MEMC_SETBLUR_MOTIONLEVEL:
		{
			//rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_SETBLUR_MOTIONLEVEL\n");
			unsigned char blurLevel;
			if(copy_from_user((void *)&blurLevel, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETBLUR_MOTIONLEVEL failed!!!!!!!!!!!!!!!\n");
			}
			else
			{	
				rtd_pr_memc_notice("##############[MEMC]VPQ_IOC_MEMC_SETBLUR_MOTIONLEVEL, MOTION_blurLevel = %d\n", blurLevel);
				VPQ_MEMC_SetBlurMotionLevel(blurLevel);
			}

			break;
		}
		case VPQ_IOC_MEMC_SETJUDDER_MOTIONLEVEL:
		{
			//rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_SETJUDDER_MOTIONLEVEL\n");
			unsigned char judderLevel;
			if(copy_from_user((void *)&judderLevel, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETJUDDER_MOTIONLEVEL failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				rtd_pr_memc_notice("##############[MEMC]VPQ_IOC_MEMC_SETJUDDER_MOTIONLEVEL, MOTION_judderLevel = %d\n", judderLevel);
				VPQ_MEMC_SetJudderMotionLevel(judderLevel);
			}

			break;
		}
#endif
		case VPQ_IOC_MEMC_MOTIONCOMPONOFF:
		{
			//rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_MOTIONCOMPONOFF\n");
			BOOLEAN bOnOff;
			if(copy_from_user((void *)&bOnOff, (const void __user *)arg, sizeof(BOOLEAN)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_MOTIONCOMPONOFF failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				MEMC_DBG_PRINT( "##############[MEMC]not_support please use VPQ_IOC_MEMC_SETMOTIONCOMP!!\n");
				retval = 0;
				HAL_VPQ_MEMC_MotionCompOnOff(bOnOff);
			}

			break;
		}
		case VPQ_IOC_MEMC_LOWDELAYMODE:
		{
			unsigned char type;

			rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_LOWDELAYMODE\n");
			if(copy_from_user((void *)&type, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_LOWDELAYMODE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				retval = HAL_VPQ_MEMC_LowDelayMode(type);
			}

			break;
		}
		case VPQ_IOC_MEMC_SETRGBYUVMode:
		{
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_SETRGBYUVMode\n");
			unsigned char mode;
			if(copy_from_user((void *)&mode, (const void __user *)arg, sizeof(mode)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETRGBYUVMode failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				retval = HAL_VPQ_MEMC_SetRGBYUVMode(mode);
			}

			break;
		}

		case VPQ_IOC_MEMC_GETFRAMEDELAY:
		{
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_GETFRAMEDELAY\n");
			unsigned short FrameDelay = (unsigned short)HAL_VPQ_MEMC_GetFrameDelay();
			if(copy_to_user((void __user *)arg, (void *)&FrameDelay, sizeof(unsigned short))) 
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_GETFRAMEDELAY failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				retval = 0;
			}
			break;
		}

		case VPQ_IOC_MEMC_SETVIDEOBLOCK:
		{

#if	1
			VPQ_MEMC_SETVIDEOBLOCK_T set_video_block_info;
			//rtd_pr_memc_debug( "\r\n###[MEMC]VPQ_IOC_MEMC_SETVIDEOBLOCK####\r\n");

			if(copy_from_user((void *)&set_video_block_info, (const void __user *)arg, sizeof(VPQ_MEMC_SETVIDEOBLOCK_T)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETVIDEOBLOCK failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_SetVideoBlock(set_video_block_info.type, set_video_block_info.bOnOff);
			}
#else
			MEMC_DBG_PRINT( "##############[MEMC]not_support !!\n");
#endif

			break;
		}
		case VPQ_IOC_MEMC_SETTRUEMOTIONDEMO:
		{
#if 1
			unsigned char type;
			//rtd_pr_memc_debug("##############[MEMC]VPQ_IOC_MEMC_SETTRUEMOTIONDEMO\n");
			if(copy_from_user((void *)&type, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETTRUEMOTIONDEMO failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_SetTrueMotionDemo(type);
			}
#else
			MEMC_DBG_PRINT( "##############[MEMC]not_support !!\n");
#endif

			break;
		}

		case VPQ_IOC_MEMC_GETFIRMWAREVERSION:
		{
			unsigned int u32_Version = 0;
			u32_Version = HAL_VPQ_MEMC_GetFirmwareVersion();
			//rtd_pr_memc_emerg("Ver:%d.%d Date:20%d.%d.%d.%d!!\n", ((u32_Version>>26)&0x3f), ((u32_Version>>19)&0x7f), ((u32_Version>>12)&0x7f), ((u32_Version>>8)&0xf), ((u32_Version>>3)&0x1f), (u32_Version&0x7));
			if(copy_to_user((void __user *)arg, (void *)&u32_Version, sizeof(unsigned int))) 
			{
				retval = -1;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_GETFIRMWAREVERSION failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				retval = 0;
				MEMC_DBG_PRINT("Ver:%d.%d Date:20%d.%d.%d.%d!!\n", ((u32_Version>>26)&0x3f), ((u32_Version>>19)&0x7f), ((u32_Version>>12)&0x7f), ((u32_Version>>8)&0xf), ((u32_Version>>3)&0x1f), (u32_Version&0x7));
			}
			break;

		}
		case VPQ_IOC_MEMC_SETBYPASSREGION:
		{
#if 1
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_SETBYPASSREGION\n");
			VPQ_MEMC_SETBYPASSREGION_T set_bypass_region_info;
			if(copy_from_user((void *)&set_bypass_region_info, (const void __user *)arg, sizeof(VPQ_MEMC_SETBYPASSREGION_T)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETBYPASSREGION failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_SetBypassRegion(set_bypass_region_info.bOnOff, set_bypass_region_info.region, set_bypass_region_info.x, set_bypass_region_info.y, set_bypass_region_info.w, set_bypass_region_info.h);
			}
#else
			MEMC_DBG_PRINT( "##############[MEMC]not_support !!\n");
#endif

			break;
		}

		// for Roku, add by JerryWang
		case VPQ_IOC_MEMC_GETMOTION:
		{
#if 1//#if 0//k23//k24		
			unsigned char mode = 0;
			rtd_pr_memc_emerg("##############[MEMC]VPQ_IOC_MEMC_GETMOTION\n");
			mode = Scaler_MEMC_GetMEMC_Mode();
			rtd_pr_memc_emerg("[MEMC_GetMode][,%d,]\n", mode);
			if (copy_to_user((void __user *)arg, (void *)&mode, sizeof(unsigned char))) {
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_GETMOTION failed!!!!!!!!!!!!!!!\n");
				retval = -1;
			} else
				retval = 0;
#endif
			break;
		}

		case VPQ_IOC_MEMC_SETREVERSECONTROL:
		{

#if 1//#if 0//k23//k24		

			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_SETREVERSECONTROL\n");
			unsigned char u8Mode;
			if(copy_from_user((void *)&u8Mode, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETREVERSECONTROL failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_SetReverseControl(u8Mode);
			}
#else
			MEMC_DBG_PRINT( "##############[MEMC]not_support !!\n");
#endif
			break;
		}
		case VPQ_IOC_MEMC_FREEZE:
		{

#if	1
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_FREEZE\n");
			unsigned char type;
			if(copy_from_user((void *)&type, (const void __user *)arg, sizeof(unsigned char)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_FREEZE failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_Freeze(type);
			}
#else
						MEMC_DBG_PRINT( "##############[MEMC]not_support !!\n");
#endif

			break;
		}

		case VPQ_IOC_MEMC_SETDEMOBAR:
		{
#if	0
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_SETDEMOBAR\n");
			VPQ_MEMC_SETDEMOBAR_T set_demo_bar_info;
			if(copy_from_user((void *)&set_demo_bar_info, (const void __user *)arg, sizeof(VPQ_MEMC_SETDEMOBAR_T)))
			{
				retval = -EFAULT;
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_SETDEMOBAR failed!!!!!!!!!!!!!!!\n");
			}
			else
			{
				HAL_VPQ_MEMC_SetDemoBar(set_demo_bar_info.bOnOff, set_demo_bar_info.r, set_demo_bar_info.g, set_demo_bar_info.b);
			}
#else
						MEMC_DBG_PRINT( "##############[MEMC]not_support !!\n");
#endif
			break;
		}

		case VPQ_IOC_MEMC_DEBUG:
		{
#if	0
			//rtd_pr_memc_debug( "##############[MEMC]VPQ_IOC_MEMC_DEBUG\n");
			HAL_VPQ_MEMC_DEBUG();
#else
			MEMC_DBG_PRINT( "##############[MEMC]not_support !!\n");
#endif

			break;
		}

		case VPQ_IOC_MEMC_GET_ENABLE:
		{
#if 1//#if 0//k23//k24	
			unsigned char enable = 0;
			rtd_pr_memc_debug("VPQ_IOC_MEMC_GET_ENABLE\n");

			if (Scaler_GetMemcEnable(&enable) == FALSE) {
				rtd_pr_memc_err( "kernel Scaler_GetMemcEnable fail\n");
				retval = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&enable, sizeof(unsigned char))) {
					rtd_pr_memc_err( "kernel VPQ_IOC_MEMC_GET_ENABLE fail\n");
					retval = -1;
				} else
					retval = 0;
			}
#endif			
			break;
		}
		case VPQ_IOC_MEMC_SET_LEVEL:
		{
#if 1//#if 0//k23//k24		
			unsigned char  args = 0;
			rtd_pr_memc_debug("VPQ_IOC_MEMC_SET_LEVEL\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned char ))) {

				retval = -1;
			} else {
				retval = Scaler_SetMemcLevel((TV001_LEVEL_E)args);
			}
#endif			
			break;
		}
		case VPQ_IOC_MEMC_GET_RANGE:
		{
#if 1//#if 0//k23//k24		
			TV001_MEMC_RANGE_S range ;
			rtd_pr_memc_debug("VPQ_IOC_MEMC_GET_RANGE\n");

			if (Scaler_GetMemcRange(&range) == FALSE) {
				rtd_pr_memc_err( "kernel Scaler_GetMemcRange fail\n");
				retval = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&range, sizeof(TV001_MEMC_RANGE_S))) {
					rtd_pr_memc_err( "kernel VPQ_IOC_MEMC_GET_RANGE fail\n");
					retval = -1;
				} else
					retval = 0;
			}
#endif			
			break;
		}
		case VPQ_IOC_MEMC_FORCE_24TO48:
		{
			unsigned int args = 0;
			rtd_pr_memc_debug("VPQ_IOC_MEMC_FORCE_24TO48\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				retval = -1;
			} else {
				retval = 0;
				m_bForce24to48 = args;
			}
			break;
		}
		case VPQ_IOC_MEMC_GET_UI_SUPPORT:
		{
			unsigned char bMemcUISupport = 0;
			bMemcUISupport=(unsigned char)HAL_VBE_DISP_Get_MEMC_UI_Status();
			//rtd_pr_memc_emerg( "[VPQ_IOC_MEMC_GET_UI_SUPPORT DIA2DEV-35 MEMC_bMemcUISupport][,%d,]\n", bMemcUISupport);
			if (copy_to_user((void __user *)arg, (void *)&bMemcUISupport, sizeof(unsigned char))) 
			{
				MEMC_DBG_PRINT("scaler vpq memc ioctl code=VPQ_IOC_MEMC_GET_UI_SUPPORT failed!!!!!!!!!!!!!!!\n");
				retval = -1;
			}
			else
			{
				retval = 0;
			}
			break;
		}
		default:
			rtd_pr_memc_debug( "Scaler vpq memc: ioctl code = %d is invalid!!!!!!!!!!!!!!!1\n", cmd);
			break ;
		}
	return retval;
#endif	
}
//#endif

#if RTK_MEMC_Performance_tunging_from_tv001
#if defined(CONFIG_RTK_8KCODEC_INTERFACE)
#include "kernel/rtk_codec_interface.h"
extern struct rtk_codec_interface *rtk_8k;
//rtk_8k->vpqled->info();
void VPQMEMC_rlink_MEMC_ID_enable(void)
{
	if(rtk_8k == NULL)
	{
		rtd_pr_memc_info("%s, null pointer: rtk_8k\n", __FUNCTION__);
		return;
	}
		
	rtk_8k->vpqmemc->R8k_VPQEX_MEMC_ID_enable();
	
	rtd_pr_memc_info("%s, VPQMEMC_rlink_MEMC_ID_enable:set ! \n", __FUNCTION__);
}

void VPQMEMC_rlink_MEMC_ID(void)
{
	extern unsigned char DynamicOptimizeMEMC[255];
	
	if(rtk_8k == NULL)
	{
		rtd_pr_memc_info("%s, null pointer: rtk_8k\n", __FUNCTION__);
		return;
	}
		
	rtk_8k->vpqmemc->R8k_VPQEX_MEMC_Idenfification_Info(DynamicOptimizeMEMC);
	
	rtd_pr_memc_info("%s, DynamicOptimizeMEMC  set ! \n", __FUNCTION__);
}
#endif

#endif


struct file_operations vpq_memc_fops= {
	.owner =    THIS_MODULE,
	.open  =    vpq_memc_open,
	.release =  vpq_memc_release,
	.read  =    vpq_memc_read,
	.write = 	vpq_memc_write,
	.unlocked_ioctl =    vpq_memc_ioctl,
#ifdef CONFIG_ARM64
#ifdef CONFIG_COMPAT
	.compat_ioctl = vpq_memc_ioctl,
#endif
#endif
};

#ifdef CONFIG_PM
static const struct dev_pm_ops vpq_memc_pm_ops =
{
	.suspend    = vpq_memc_suspend,
	.resume     = vpq_memc_resume,
#ifdef CONFIG_HIBERNATION
	.freeze 	= vpq_memc_suspend_snashop,
	.thaw 		= vpq_memc_resume_snashop,
	.poweroff 	= vpq_memc_suspend_snashop,
	.restore 	= vpq_memc_resume_snashop,
#endif
};
#endif // CONFIG_PM


static struct platform_device *vpq_memc_platform_devs;

static struct platform_driver vpq_memc_device_driver = {
	.driver =
	{
        .name         = VPQ_MEMC_DEVICE_NAME,
        .bus          = &platform_bus_type,
#ifdef CONFIG_PM
	 .pm         = &vpq_memc_pm_ops,
#endif

	},
};

#ifdef CONFIG_ARM64 //ARM32 compatible
static char *vpq_memc_devnode(struct device *dev, umode_t *mode)
#else
static char *vpq_memc_devnode(struct device *dev, mode_t *mode)
#endif
{
	return NULL;
}
#if 0
static void memc_isr_disable (void)
{
	unsigned int  u32_interrupt_reg = 0;

	rtd_pr_memc_notice("%s %d\n",__func__,__LINE__);
//#ifdef CONFIG_MEMC_BYPASS
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return;
//#endif
	if(Scaler_MEMC_CLK_Check() == FALSE){
		Scaler_MEMC_CLK_Enable();
	}

	// disable interrupt
	//reg_kmc_int_en
	u32_interrupt_reg = IoReg_Read32(KMC_TOP_kmc_top_18_reg); //MC_TOP__REG_KMC_IN_INT_SEL_ADDR
	u32_interrupt_reg &= 0xf0ffffff;
	IoReg_Write32(KMC_TOP_kmc_top_18_reg, u32_interrupt_reg);

	//reg_post_int_en
	u32_interrupt_reg = IoReg_Read32(KPOST_TOP_KPOST_TOP_08_reg); //KPOST_TOP__REG_POST_INT_SOURCE_SEL_ADDR
	u32_interrupt_reg &= 0xfffffe1f;
	IoReg_Write32(KPOST_TOP_KPOST_TOP_08_reg, u32_interrupt_reg);
}
#endif
static int memc_pm_event (struct notifier_block *this, unsigned long event, void *ptr)
{
        switch (event) {
    case PM_SUSPEND_PREPARE:
        rtd_pr_memc_debug("%s %d\n",__func__,__LINE__);
       // memc_isr_disable();
        break;
        default:
                break;
        }

    return NOTIFY_DONE;
}

static struct notifier_block memc_notifier = {
  .notifier_call = memc_pm_event,
};


int vpq_memc_module_init(void)
{
	int result;
	int vpq_memc_devno;
	dev_t devno = 0;//vbe device number
	//PANEL_CONFIG_PARAMETER *panel_parameter = NULL;

	result = alloc_chrdev_region(&devno, vpq_memc_minor, vpq_memc_nr_devs,VPQ_MEMC_DEVICE_NAME);
	vpq_memc_major = MAJOR(devno);
	if (result < 0) {
		rtd_pr_memc_debug("VPQ_MEMC_DEVICE: can't get major %d\n", vpq_memc_major);
		return result;
	}

	rtd_pr_memc_debug( "VPQ_MEMC_DEVICE init module major number = %d\n", vpq_memc_major);
	vpq_memc_devno = MKDEV(vpq_memc_major, vpq_memc_minor);

	vpq_memc_class = class_create(THIS_MODULE, VPQ_MEMC_DEVICE_NAME);

	if (IS_ERR(vpq_memc_class))
	{
		rtd_pr_memc_debug( "scalevpqmemc: can not create class...\n");
	    result=PTR_ERR(vpq_memc_class);
		goto fail_class_create;
	}
	vpq_memc_class->devnode = vpq_memc_devnode;
	vpq_memc_platform_devs = platform_device_register_simple(VPQ_MEMC_DEVICE_NAME, -1, NULL, 0);
    if((result=platform_driver_register(&vpq_memc_device_driver)) != 0)
	{
		rtd_pr_memc_debug( "scaler vpqmemc: can not register platform driver...\n");
    	goto fail_platform_driver_register;
    }

	cdev_init(&vpq_memc_cdev, &vpq_memc_fops);
	vpq_memc_cdev.owner = THIS_MODULE;
   	vpq_memc_cdev.ops = &vpq_memc_fops;
	result = cdev_add (&vpq_memc_cdev, vpq_memc_devno, 1);
	if (result)
	{
		MEMC_DBG_PRINT("Error %d adding VPQ_MEMC_DEVICE!\n", result);
		goto fail_cdev_init;
	}
	device_create(vpq_memc_class, NULL, MKDEV(vpq_memc_major, 0), NULL,VPQ_MEMC_DEVICE_NAME);
	sema_init(&vpq_memc_Semaphore, 1);
	sema_init(&livezoom_memcclose_Semaphore, 1);
	sema_init(&MEMC_CLK_Semaphore, 1);
	//panel_parameter =  (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;
	//Panel_InitParameter(panel_parameter);
	//Panel_LVDS_To_Hdmi_Converter_Parameter();

        register_pm_notifier(&memc_notifier);

	return 0;//Success

fail_cdev_init:
	platform_driver_unregister(&vpq_memc_device_driver);
fail_platform_driver_register:
	platform_device_unregister(vpq_memc_platform_devs);
	vpq_memc_platform_devs = NULL;
	class_destroy(vpq_memc_class);
fail_class_create:
	vpq_memc_class = NULL;
	unregister_chrdev_region(vpq_memc_devno, vpq_memc_nr_devs);
	return result;
}

void __exit vpq_memc_module_exit(void)
{
	dev_t devno = MKDEV(vpq_memc_major, vpq_memc_minor);
	MEMC_DBG_PRINT("vbe clean module vpq_memc_major = %d\n", vpq_memc_major);

  	device_destroy(vpq_memc_class, MKDEV(vpq_memc_major, 0));
  	class_destroy(vpq_memc_class);
	vpq_memc_class=NULL;
	cdev_del(&vpq_memc_cdev);

   	/* device driver removal */
	if(vpq_memc_platform_devs)
	{
		platform_device_unregister(vpq_memc_platform_devs);
		vpq_memc_platform_devs=NULL;
	}
  	platform_driver_unregister(&vpq_memc_device_driver);

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, vpq_memc_nr_devs);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(vpq_memc_module_init);
module_exit(vpq_memc_module_exit);
#endif
#endif //BUILD_QUICK_SHOW
#endif

void Scaler_MEMC_SetInOutUseCase_QS(void){

	if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_2K){//if(Get_DISPLAY_PANEL_TYPE() == P_LVDS_2K1K){
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_1920X1080, VPQ_OUTPUT_1920X1080, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
	else if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_3K){
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_2560X1440, VPQ_OUTPUT_2560X1440, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
#if ENABLE_4K1K
	else if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_4K1K){
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X1080, VPQ_OUTPUT_3840X1080, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
#endif
	else{
		Scaler_MEMC_SetInOutUseCase(VPQ_INPUT_3840X2160, VPQ_OUTPUT_3840X2160, VPQ_INPUT_2D, VPQ_OUTPUT_2D);
	}
}

#ifndef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
extern unsigned int MEMC_Lib_s16_speed_is_not_normal(void);
unsigned int Scaler_MEMC_s16_speed_is_not_normal(void)
{
	unsigned int i_Scaler_MEMC_s16_speed_is_not_normal=0;
	i_Scaler_MEMC_s16_speed_is_not_normal=MEMC_Lib_s16_speed_is_not_normal();
	return i_Scaler_MEMC_s16_speed_is_not_normal;
}
#endif



#ifndef CONFIG_MEMC_NOTSUPPORT
extern VOID MEMC_Lib_Set_UltraLowDelayMode(unsigned char u1_enable);
#endif
VOID Scaler_MEMC_SetUltraLowDelayMode(unsigned char u1_enable){
#ifdef CONFIG_MEMC_NOTSUPPORT
	return;
#else
	return MEMC_Lib_Set_UltraLowDelayMode(u1_enable);
#endif
}

#ifndef CONFIG_MEMC_NOTSUPPORT
extern VOID HAL_CRTC_SetFRCVtrigDelay(unsigned int u32FRC_Vtrig_dly);
#endif

VOID Scaler_MEMC_HAL_CRTC_SetFRCVtrigDelay(unsigned int u32FRC_Vtrig_dly)
{
#ifdef CONFIG_MEMC_NOTSUPPORT
		return;
#else
		return HAL_CRTC_SetFRCVtrigDelay(u32FRC_Vtrig_dly);
#endif
}

#ifndef CONFIG_MEMC_NOTSUPPORT
extern VOID HAL_CRTC_SetMCVtrigDelayLong(unsigned int u32MC_Vtrig_dly_long);
#endif

VOID Scaler_MEMC_HAL_CRTC_SetMCVtrigDelayLong(unsigned int u32MC_Vtrig_dly_long)
{
#ifdef CONFIG_MEMC_NOTSUPPORT
		return;
#else
		return HAL_CRTC_SetMCVtrigDelayLong(u32MC_Vtrig_dly_long);
#endif
}

#ifndef CONFIG_MEMC_NOTSUPPORT
extern unsigned char *Scaler_MEMC_GetCadence(void);
#endif

unsigned char *Scaler_VPQ_MEMC_GetCadence(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT
		return NULL;
#else
		return Scaler_MEMC_GetCadence();
#endif
}

#ifndef CONFIG_MEMC_NOTSUPPORT
extern MEMC_DEMO_MODE MEMC_LibGetMEMCDemoMode(VOID);
#endif

MEMC_DEMO_MODE Scaler_MEMC_LibGetMEMCDemoMode(VOID)
{
#ifdef CONFIG_MEMC_NOTSUPPORT
		return 0;
#else
		return MEMC_LibGetMEMCDemoMode();
#endif
}

#ifndef CONFIG_MEMC_NOTSUPPORT
extern int Scaler_register_MEMC_thermal_handler(void);
#endif

int Scaler_MEMC_register_MEMC_thermal_handler(VOID)
{
#ifdef CONFIG_MEMC_NOTSUPPORT
		return 0;
#else
		return Scaler_register_MEMC_thermal_handler();
#endif
}



