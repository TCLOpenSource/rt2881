/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2006
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for memory related functions.
 *
 * @author 	$Author$
 * @date		$Date$
 * @version	$Revision$
 * @ingroup	memory
 */

/**
 * @addtogroup memory
 * @{
 */

/*============================ Module dependency  ===========================*/
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
#include <uapi/linux/const.h>
#include <linux/mm.h>
#include <linux/auth.h>
#include <mach/platform.h>
#include <linux/math64.h>

#endif

#ifndef BUILD_QUICK_SHOW
 #include "tvscalercontrol/vdc/video.h"

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <VideoRPC_System.h>
	#include <VideoRPC_System_data.h>
#else
#include <rpc/VideoRPC_System.h>
#include <rpc/VideoRPC_System_data.h>
#endif
#include <tvscalercontrol/scalerdrv/scaler_hdr_ctrl.h>
#include <mach/rtk_platform.h>
#include <rtd_log/rtd_module_log.h>
#else
#include <sysdefs.h>
#include <mach/rtk_platform.h>
#include <no_os/semaphore.h>
#include <no_os/spinlock.h>
#include <no_os/spinlock_types.h>
#include <no_os/slab.h>
#include <timer.h>
#include <rtk_kdriver/rmm/rmm.h>
#include <rmm/rmm_common.h>
#include <rtd_log/rtd_module_log.h>
#include <no_os/pageremap.h>
#include "vby1/panel_api.h"

#endif

#include <rtk_dc_mt.h>

#include <rbus/timer_reg.h>
#include <rbus/de_xcxl_reg.h>

#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <scaler_vscdev.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <tvscalercontrol/vip/scalerColor.h>

#include <tvscalercontrol/io/ioregdrv.h>

#ifdef BUILD_QUICK_SHOW
#define  dvr_memory_alignment(x, y)  ((unsigned long)x)
#define  PAGE_ALIGN(x)  (x)
#define  dvr_to_phys(x) ((unsigned int)x)

#undef  BUG
#define BUG() do { \
	rtd_pr_scaler_memory_info("U-Boot BUG at %s:%d!\n", __FILE__, __LINE__); \
} while (0)
#endif
/*===================================  Types ================================*/
/*================================ Definitions ==============================*/
#define MDOMAIN_ORIGINAL_CARVEOUT_SIZE 0x1C00000 // original carveout size is 28MB
#define CMA_NEED_BUFFER_NUMBER_1 1
/*================================== Variables ==============================*/
StructMemBlock MemTag[MEMIDX_UNKNOW];	// memory tag information
// 3DDI memory usage control
static unsigned int bIpBtrMode=_DISABLE, bIpDma420Mode=_DISABLE;
static eMemCapAccessType enable_M_cap_mode_main = MEMCAPTYPE_FRAME;
static eMemCapAccessType enable_M_cap_mode_sub = MEMCAPTYPE_FRAME;

static UINT8 enable_M_compression_main = 0;
static UINT8 enable_M_compression_sub = 0;
static UINT8 M_compression_bits = COMPRESSION_NONE_BITS;
static UINT8 M_compression_mode = COMPRESSION_NONE_MODE;
static UINT8 Sub_M_compression_bits = COMPRESSION_NONE_BITS;
static UINT8 Sub_M_compression_mode = COMPRESSION_NONE_MODE;

static UINT8 enable_M_GameMode = _DISABLE; // driver use game mode or not
static UINT8 enable_M_GameMode_Dynamic = _DISABLE;//AP set game mode or not
static UINT8 enable_M_LowLatencyMode_Vdec_direct = _DISABLE;

static UINT8 Mdomain_multibuffer_enable = false;
static UINT8 Mdomain_capture_multibuffer_enable = false;
static UINT8 Mdomain_multibuffer_number = 0;
static unsigned int Mdomain_multibuffer_size = 0;
static UINT8 Mdomain_multibuffer_number_pre = 0;
static UINT8 Mdomain_multibuffer_changed = 0;

static unsigned char mdomain_driver_status = FALSE;//can reset m capture status
static void *m_domain_borrow_buffer_virtual_addr;
static unsigned int cma_borrow_size = 0;
static UINT32 M_fixed_virtual_memory_size = 0;
static unsigned char CMA_buffer_number = 0;
static unsigned int CMA_buffer_size = 0;
static unsigned long CMA_m_domain_original_buffer_physical_addr[MEMIDX_CMA_UNKNOWN] = {0};
static unsigned int CMA_m_domain_original_size[MEMIDX_CMA_UNKNOWN] = {0};
unsigned int m_domain_setting_err_status[MAX_DISP_CHANNEL_NUM];
static void *sub_m_domain_1st_buf_addr;
static void *sub_m_domain_2nd_buf_addr;
static void *sub_m_domain_3rd_buf_addr;

static struct semaphore Main_Mdomain_ctrl_Semaphore;//for 4k flow  protect m domain capture enable memory trash



extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr; //extern char run_iv2dv_slow_tuning_flag;//Run iv2dv slow tuning flag
/*================================== Function ===============================*/
void main_mdomain_ctrl_semaphore_init(void)
{//init Main_Mdomain_ctrl_Semaphore
	sema_init(&Main_Mdomain_ctrl_Semaphore, 1);
}

struct semaphore *get_main_mdomain_ctrl_semaphore(void)
{//get Main_Mdomain_ctrl_Semaphore
	return &Main_Mdomain_ctrl_Semaphore;
}

eMemCapAccessType dvrif_memory_get_cap_mode(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return enable_M_cap_mode_main;
	else
		return enable_M_cap_mode_sub;
}

void dvrif_memory_set_cap_mode(unsigned char display, eMemCapAccessType Cap_type)
{
	if(display == SLR_MAIN_DISPLAY)
		enable_M_cap_mode_main = Cap_type;
	else
		enable_M_cap_mode_sub = Cap_type;
}

unsigned char dvrif_memory_compression_get_enable(unsigned char display)
{
#ifdef CONFIG_RTK_KDRV_DV_IDK_DUMP
	return FALSE;
#endif
	if(display == SLR_MAIN_DISPLAY)
		return enable_M_compression_main;
	else
		return enable_M_compression_sub;
}
void dvrif_memory_compression_set_enable(unsigned char display, unsigned char enable)
{
	if(display == SLR_MAIN_DISPLAY)
		enable_M_compression_main = enable;
	else
		enable_M_compression_sub = enable;
}

unsigned char dvrif_memory_get_compression_bits(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return M_compression_bits;
	else
		return Sub_M_compression_bits;
}
void dvrif_memory_set_compression_bits(unsigned char display, unsigned char comp_bits)
{
	if(display == SLR_MAIN_DISPLAY)
		M_compression_bits=comp_bits;
	else
		Sub_M_compression_bits=comp_bits;
}

unsigned char dvrif_memory_get_compression_mode(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return M_compression_mode;
	else
		return Sub_M_compression_mode;
}
void dvrif_memory_set_compression_mode(unsigned char display, unsigned char comp_mode)
{
	if(display == SLR_MAIN_DISPLAY)
		M_compression_mode = comp_mode;
	else
		Sub_M_compression_mode = comp_mode;
}
#ifndef BUILD_QUICK_SHOW
void drvif_memory_compression_rpc(unsigned char display){

	M_DOMAIN_COMPRESSION_T *mdomain_compression;
	//unsigned int ulCount = 0;
	int ret;

	mdomain_compression = (M_DOMAIN_COMPRESSION_T *)Scaler_GetShareMemVirAddr(SCALERIOC_SET_MDOMAIN_COMPRESSION_ENABLE);
	//ulCount = sizeof(M_DOMAIN_COMPRESSION_T) / sizeof(unsigned int);

	mdomain_compression->display = display;
	mdomain_compression->bOnOff = dvrif_memory_compression_get_enable(display);
	mdomain_compression->compbit = dvrif_memory_get_compression_bits(display);
	mdomain_compression->GameMode = 0;//keep 3-buffer mode. (smooth_toggle_game_mode_timeout_trigger_flag ? 0 : drv_memory_get_game_mode());
	mdomain_compression->RGB444Mode =get_vsc_run_pc_mode();
	mdomain_compression->LivezoomMode = FALSE;
	mdomain_compression->compmode = dvrif_memory_get_compression_mode(display);

	//change endian
	mdomain_compression->display = htonl(mdomain_compression->display);
	mdomain_compression->bOnOff = htonl(mdomain_compression->bOnOff);
	mdomain_compression->compbit = htonl(mdomain_compression->compbit);
	mdomain_compression->GameMode = htonl(mdomain_compression->GameMode);
	mdomain_compression->RGB444Mode = htonl(mdomain_compression->RGB444Mode);
	mdomain_compression->LivezoomMode = htonl(mdomain_compression->LivezoomMode);
	mdomain_compression->compmode = htonl(mdomain_compression->compmode);

	if (0 != (ret = Scaler_SendRPC(SCALERIOC_SET_MDOMAIN_COMPRESSION_ENABLE,0,0))){
		rtd_pr_scaler_memory_debug("[MEMC]ret=%d, SCALERIOC_SET_MDOMAIN_COMPRESSION_ENABLE RPC fail !!!\n", ret);
	}
}
#endif

void set_mdomain_driver_status(unsigned char status)
{//set m domain driver ready or not
	mdomain_driver_status = status;
}

unsigned char get_mdomain_driver_status(void)
{//decide m domain driver finish or not
	return mdomain_driver_status;
}

// IP bit number control (saving bandwidth)
unsigned int drv_memory_get_ip_Btr_mode(void)
{
	return bIpBtrMode;
}


unsigned int drv_memory_get_ip_DMA420_mode(void)
{
	return bIpDma420Mode;
}


void drv_memory_set_ip_Btr_mode(unsigned int mode)
{
	bIpBtrMode = mode;
	return;
}


void drv_memory_set_ip_DMA420_mode(unsigned int mode)
{
	bIpDma420Mode = mode;
	return;
}

unsigned char drv_memory_get_DMA_Aligned_Val(void)
{
	unsigned char ret_aligned_val = 0;
#ifdef CONFIG_BW_96B_ALIGNED
	ret_aligned_val = 6;
#else
	ret_aligned_val = 0;
#endif
	return ret_aligned_val;
}

void drv_memory_set_game_mode(unsigned char enable)
{
	enable_M_GameMode = enable;
#ifndef BUILD_QUICK_SHOW
	drv_update_game_mode_frc_fs_flag();
#endif
}

unsigned char drv_memory_get_game_mode(void)
{
    //Update the game mode condition to game mode status driver @Crixus 20180825
    if(get_new_game_mode_condition())
        return enable_M_GameMode;
    else
        return FALSE;
}

void drv_memory_set_game_mode_dynamic(unsigned char enable){
	enable_M_GameMode_Dynamic = enable;
}

unsigned char drv_memory_get_game_mode_dynamic_flag(void){
	return enable_M_GameMode_Dynamic;
}

unsigned char drv_memory_get_game_mode_dynamic(void){
	return enable_M_GameMode_Dynamic;
}

static bool is_hdmi_vrr_or_freesync(void)
{
	return (get_vsc_src_is_hdmi_or_dp()) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag());
}

unsigned char drv_memory_get_low_delay_game_mode_dynamic(void)
{
	unsigned char low_delay_game =FALSE;

	low_delay_game = (drv_memory_get_game_mode_dynamic() || is_hdmi_vrr_or_freesync()) && !get_scaler_qms_mode_flag();
	return low_delay_game;
}

//only get game mode flag, no need to check input source & conditions.
unsigned char drv_memory_get_game_mode_flag(void)
{
	return enable_M_GameMode;
}

/*for livezoom and magnifier game mode keep i3ddma single buffer*/
unsigned char drv_I3ddmaMemory_get_game_mode(void)
{
       /*Only support HDMI and Component and AV*/
#if 0
 /*20170629 pinyen move conditions outside driver*/
       if(!((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_ADC)
               || (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
               ||((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_AVD)  && (get_AVD_Input_Source() != _SRC_TV))))
               return FALSE;
#endif
       return enable_M_GameMode;
}

void drv_memory_set_vdec_direct_low_latency_mode(unsigned char enable){
	enable_M_LowLatencyMode_Vdec_direct = enable;
}

unsigned char drv_memory_get_vdec_direct_low_latency_mode(void){
	return enable_M_LowLatencyMode_Vdec_direct;
}

void drv_memory_Set_multibuffer_flag(UINT8 enable){
	Mdomain_multibuffer_enable = enable;
}

UINT8 drv_memory_Get_multibuffer_flag(void){
	return Mdomain_multibuffer_enable;
}

void drv_memory_Set_multibuffer_number(UINT8 number){
	Mdomain_multibuffer_number = number;
}

UINT8 drv_memory_Get_multibuffer_number(void){
	return Mdomain_multibuffer_number;
}
void drv_memory_Set_multibuffer_number_pre(UINT8 number){
	Mdomain_multibuffer_number_pre = number;
}

UINT8 drv_memory_Get_multibuffer_number_pre(void){
	return Mdomain_multibuffer_number_pre;
}
void drv_memory_Set_capture_multibuffer_flag(UINT8 enable){
	Mdomain_capture_multibuffer_enable = enable;
}

UINT8 drv_memory_Get_capture_multibuffer_flag(void){
	return Mdomain_capture_multibuffer_enable;
}

void drv_memory_Set_multibuffer_size(unsigned int buffer_size){
	Mdomain_multibuffer_size = buffer_size;
}

unsigned int drv_memory_Get_multibuffer_size(void){
	return Mdomain_multibuffer_size;
}

void drv_memory_Set_multibuffer_changed(UINT8 enable){
	Mdomain_multibuffer_changed = enable;
}

UINT8 drv_memory_Get_multibuffer_changed(void){
	return Mdomain_multibuffer_changed;
}

void drv_memory_send_multibuffer_number_to_smoothtoogh(void)
{
#ifndef BUILD_QUICK_SHOW

	unsigned int *pulDataTemp = NULL;
	int ret = 0;

	pulDataTemp = (unsigned int*)Scaler_GetShareMemVirAddr(SCALERIOC_MULTIBUFFER_NUMBER);
	if (pulDataTemp) {
		*pulDataTemp=(unsigned int)drv_memory_Get_multibuffer_number();
		*pulDataTemp = htonl(*pulDataTemp);
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_MULTIBUFFER_NUMBER,0,0)))
		{
			rtd_pr_scaler_memory_emerg("ret=%d, SCALERIOC_MULTIBUFFER_NUMBER to smoothgooh fail !!!\n", ret);
		}
	}
#endif
}

#ifdef BUILD_QUICK_SHOW
static bool is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}
#endif

/*============================================================================*/
/**
 yyyrvif_memory_get_data_align
 * This function is used to do data alignment.
 *
 * @param <Value> { data needs alignment }
 * @param <unit> { unit of alignment }
 * @return { none }
 *
 */
unsigned int drvif_memory_get_data_align(unsigned int Value, unsigned int unit)
{
	unsigned int AlignVal;
#define N_BYTES 96

	AlignVal = Value;

    if (is_power_of_2(unit)) {
        unit -= 1;
        if (Value & unit)
            AlignVal = (Value & (~unit)) + unit + 1;
    } else {
        if ((AlignVal % unit) != 0)
            AlignVal = AlignVal + (unit - (AlignVal % unit));
#ifndef CONFIG_BW_96B_ALIGNED
        if ((unit % N_BYTES) == 0)
            AlignVal = Value; //output = input
#endif
    }
	return AlignVal;
}
/*============================================================================*/
/**
 * MemGetBlockAddr
 * Get start address of allocated memory
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { none }
 *
 */
unsigned int drvif_memory_get_block_addr(eMemIdx Idx)
{
	return MemTag[Idx].StartAddr;
}

/*============================================================================*/
/**
 * drvif_memory_get_block_size
 * Get size of allocated memory
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { block size }
 *
 */
unsigned int drvif_memory_get_block_size(eMemIdx Idx)
{
	return MemTag[Idx].Size;
}

//k4lp start to use query driver to get memory and address @Crixus 20170504
/*
	(1)M-domain
		a. main          => ((3840  12 + 256) / 128  2160 * 16) x 3 = 35.8 MB   (reserved memory)
		b. main-i2run  => ((1920  12 + 256) / 128  1080 * 16) x 3 = 9 MB

	(2)DI/RTNR
		a. DI_1 => (4k):     25.716 MB (reserved memory)
		b. DI_2 => (1080i): 14.09 MB (compression 10 bits)
		c. DMAtable => 2MB
*/

unsigned long get_query_start_address(unsigned char idx){
	unsigned long addr = 0 ;
	unsigned int size = 0 ;
	unsigned int buffer_size_main = 0;
#ifdef CONFIG_I2RND_ENABLE
	unsigned int buffer_size_main_i2r = MDOMAIN_MAIN_I2R_BUFFER_SIZE;
#endif
	//rtd_pr_scaler_memory_info("[%s]reserved memory start addr = 0x%x, size = 0x%x, MDOMAIN_MAIN_BUFFER_SIZE = 0x%x\n", __FUNCTION__, addr, size, MDOMAIN_MAIN_BUFFER_SIZE);
	//rtd_pr_scaler_memory_info("[%s]VIP_DI_1_BUFFER_SIZE = 0x%x, VIP_DI_2_BUFFER_SIZE = 0x%x, VIP_DMAto3DTable_BUFFER_SIZE = 0x%x\n", __FUNCTION__, VIP_DI_1_BUFFER_SIZE, VIP_DI_2_BUFFER_SIZE, VIP_DMAto3DTable_BUFFER_SIZE);
	//rtd_pr_scaler_memory_info("buffer_size_main=%x\n", buffer_size_main);
#ifdef CONFIG_I2RND_ENABLE
	if(idx == QUERY_IDX_MDOMAIN_MAIN){
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
		size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MDOMAIN, (void **)&addr) ;
#else
		size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MDOMAIN, (void **)&addr) ;
#endif
		if(carvedout_enable() == 0)  // do NOT use carvedout memory, use CMA malloc
			buffer_size_main = 0;
		else  // use carvedout memory
			buffer_size_main = MDOMAIN_MAIN_BUFFER_SIZE;

		buffer_size_main = drvif_memory_get_data_align(buffer_size_main, DMA_SPEEDUP_ALIGN_VALUE);

		buffer_size_main = PAGE_ALIGN(buffer_size_main);
		if(size == 0){
			BUG();
		}
	}
	else if(idx == QUERY_IDX_MDOMAIN_MAIN_I2R){
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
		size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MDOMAIN, (void **)&addr) ;
#else
		size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MDOMAIN, (void **)&addr) ;
#endif

		buffer_size_main = drvif_memory_get_data_align(buffer_size_main, DMA_SPEEDUP_ALIGN_VALUE);

		buffer_size_main = PAGE_ALIGN(buffer_size_main);
		if(size == 0){
			BUG();
		}

		addr = addr + buffer_size_main * 3;
	}
	else if(idx == QUERY_IDX_DI){
#ifndef UT_flag
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
		if (fwif_color_DIRTNR_Memory_From_CMA_Flag_Access(0, 0) == 0)	// if memory from CMA, set bnd on vsc connect
			size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_DI_NR, (void **)&addr) ;
		else
			fwif_color_DIRTNR_CMA_Memory_Get(&size, &addr);
#else
		if (fwif_color_DIRTNR_Memory_From_CMA_Flag_Access(0, 0) == 0)	// if memory from CMA, set bnd on vsc connect
			size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_DI_NR, (void **)&addr) ;
		else
			fwif_color_DIRTNR_CMA_Memory_Get(&size, &addr);
#endif
#endif
		if(size == 0){
			BUG();
		}
	}
	else if(idx == QUERY_IDX_DI_2){
#ifndef UT_flag
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
		if (fwif_color_DIRTNR_Memory_From_CMA_Flag_Access(0, 0) == 0)	// if memory from CMA, set bnd on vsc connect
			size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_DI_NR, (void **)&addr) ;
		else
			fwif_color_DIRTNR_CMA_Memory_Get(&size, &addr);
#else
		if (fwif_color_DIRTNR_Memory_From_CMA_Flag_Access(0, 0) == 0)	// if memory from CMA, set bnd on vsc connect
			size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_DI_NR, (void **)&addr) ;
		else
			fwif_color_DIRTNR_CMA_Memory_Get(&size, &addr);
#endif
#endif
		addr = addr + VIP_DI_1_BUFFER_SIZE;
		if(size == 0){
			BUG();
		}
	} else if (idx == QUERY_IDX_VIP_DMAto3DTABLE) {
		size = carvedout_buf_query(CARVEDOUT_SCALER_VIP, (void **)&addr);
		if (size == 0)
			BUG();
	} else if (idx == QUERY_IDX_VIP_DeMura) {
		size = carvedout_buf_query(CARVEDOUT_SCALER_VIP, (void **)&addr);
		if (size == 0)
			BUG();
		addr = addr + VIP_DMAto3DTable_BUFFER_SIZE;
	}

#else
	if(idx == QUERY_IDX_MDOMAIN_MAIN){
		//do nothing
		if(carvedout_enable() == 0)  // do NOT use carvedout memory, use CMA malloc
		{
			unsigned long CMA_m_domain_buffer;
			CMA_m_domain_buffer = CMA_get_m_domain_original_buffer(MEMIDX_CMA_FIRST);

			if(CMA_m_domain_buffer == INVALID_VAL)
			{
				addr = 0;
				set_m_domain_setting_err_status(SLR_MAIN_DISPLAY, MDOMAIN_SETTING_ERR_MAIN_CMA_ALLOC_FAIL); // set error status and don't enable Mcap_en later
				rtd_pr_scaler_memory_err("###[Bug] func:%s %d no memory from cma ##\n", __func__, __LINE__);
			}
			else
			{
				addr = CMA_m_domain_buffer;
			}
		}
		else  // use carvedout memory
		{
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
			size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MDOMAIN, (void **)&addr) ;
#else
			size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MDOMAIN, (void **)&addr) ;
#endif
			buffer_size_main = drvif_memory_get_data_align(buffer_size_main, DMA_SPEEDUP_ALIGN_VALUE);

			buffer_size_main = PAGE_ALIGN(buffer_size_main);
			if(size == 0){
				BUG();
			}
		}
		#ifdef HDMI_NO_PQDC_TEST
			#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
			size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MEMC, (void **)&addr) ;
			#else
			size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void **)&addr) ;
			#endif
			rtd_pr_scaler_memory_emerg("	HDMI_NO_PQDC_TEST  m-domain use CARVEDOUT_SCALER_MEMC size=%x, addr=%lx\n", size, addr);
		#endif

		if ((is_m_domain_data_frc_need_borrow_memory() && m_domain_4k_memory_from_type() == BORROW_FROM_MEMC)) {
			extern void drvif_memc_outBg_ctrl(bool enable);
			extern int Scaler_MEMC_MEMC_CLK_OnOff(unsigned char ME_enable, unsigned char MC_enable, unsigned char bMute);
#ifndef BUILD_QUICK_SHOW
            Check_smooth_toggle_update_flag(SLR_MAIN_DISPLAY);
#endif
#ifndef BUILD_QUICK_SHOW
			drvif_memc_outBg_ctrl(_ENABLE); /* memc mute black */
#endif
			//Scaler_MEMC_outMux(_ENABLE, _DISABLE); // db on, mux off
			//Scaler_MEMC_Bypass_On(1);
			Scaler_MEMC_MEMC_CLK_OnOff(0, 0, 1);	//bypass MEMC with mute on

			if(carvedout_enable() == 0)  // do NOT use carvedout memory, use CMA malloc
			{
				if(CMA_borrow_m_domain_buffer_from_MEMC(MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_3, &addr, &size) == TRUE)
				{
					rtd_pr_scaler_memory_emerg("[borrow memory] HDMI VRR  m-domain use MEMC CMA index=3 size=0x%x, addr=0x%lx\n", size, addr);
				}
				else
				{
					addr = 0;
					set_m_domain_setting_err_status(SLR_MAIN_DISPLAY, MDOMAIN_SETTING_ERR_MAIN_CMA_BORROW_FROM_MEMC_FAIL); // set error status and don't enable Mcap_en later
					rtd_pr_scaler_memory_err("###[Bug] func:%s %d no memory borrow from MEMC CMA index=3 ##\n", __func__, __LINE__);
				}
			}
			else  // use carvedout memory
			{
				size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void **)&addr) ;
				rtd_pr_scaler_memory_emerg("[borrow memory] HDMI VRR  m-domain use CARVEDOUT_SCALER_MEMC size=0x%x, addr=0x%lx\n", size, addr);
			}
		}
	} else if(idx == QUERY_IDX_DI) {
#ifndef UT_flag
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
		if (fwif_color_DIRTNR_Memory_From_CMA_Flag_Access(0, 0) == 0)	// if memory from CMA, set bnd on vsc connect
			size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_DI_NR, (void **)&addr) ;
		else
			fwif_color_DIRTNR_CMA_Memory_Get(&size, &addr);
#else
		if (fwif_color_DIRTNR_Memory_From_CMA_Flag_Access(0, 0) == 0)	// if memory from CMA, set bnd on vsc connect
			size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_DI_NR, (void **)&addr) ;
		else
			fwif_color_DIRTNR_CMA_Memory_Get(&size, &addr);
#endif
#endif
		if(size == 0){
			BUG();
		}
	} else if (idx == QUERY_IDX_VIP_NN) {
		size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_NN, (void **)&addr) ;

		if(size == 0){
			//BUG(); // k6lp does not support NN, size = 0, the NN driver will handle the error to disable NN.
		}

	} else if (idx == QUERY_IDX_VIP_DMAto3DTABLE) {
		size = carvedout_buf_query(CARVEDOUT_SCALER_VIP, (void **)&addr);

		if (size == 0)
			BUG();
	} else if (idx == QUERY_IDX_VIP_DeMura) {
		size = carvedout_buf_query(CARVEDOUT_SCALER_VIP, (void **)&addr);
		if (size == 0)
			BUG();
		addr = addr + VIP_DMAto3DTABLE_Total;
	} else if(idx == QUERY_IDX_I3DDMA) {
        size = carvedout_buf_query(CARVEDOUT_VDEC_VBM, (void **)&addr);
    }
#endif
	if (idx != QUERY_IDX_VIP_DMAto3DTABLE && idx != QUERY_IDX_VIP_DeMura)
		rtd_pr_scaler_memory_info("[%s]idx = %d, addr = 0x%lx, queried size=%x\n", __FUNCTION__, idx, addr, size);
	return addr;
}

//Will Add for smooth toggle.
unsigned int drvif_memory_get_memtag_startaddr(UINT8 index)
{
	return MemTag[index].StartAddr;

}

void drvif_memory_set_memtag_startaddr(unsigned int addr,UINT8 index)
{
	MemTag[index].StartAddr=addr;

}

unsigned long drvif_memory_get_memtag_virtual_startaddr(UINT8 index)
{
	return MemTag[index].Allocate_VirAddr;

}

StructMemBlock* drvif_memory_get_MemTag_addr(UINT8 index)
{
	return &MemTag[index];
}

void drvif_memory_set_memtag_virtual_startaddr(unsigned long addr)
{
	MemTag[MEMIDX_MAIN_THIRD].Allocate_VirAddr = addr;

}

void drvif_memory_free_fixed_virtual_memory(void)
{
	if((drvif_memory_get_memtag_virtual_startaddr(MEMIDX_MAIN_THIRD) == 0) || (M_fixed_virtual_memory_size == 0)){
		rtd_pr_scaler_memory_debug("fixed virtual address is NULL, not to free!!\n");
		return;
	}
	dvr_unmap_memory((void *)drvif_memory_get_memtag_virtual_startaddr(MEMIDX_MAIN_THIRD), M_fixed_virtual_memory_size);
}

void drvif_memory_allocate_fixed_virtual_memory(void)
{
	MemTag[MEMIDX_MAIN_THIRD].Allocate_VirAddr = (unsigned long)dvr_remap_cached_memory(MemTag[MEMIDX_MAIN_THIRD].StartAddr, M_fixed_virtual_memory_size, __builtin_return_address(0));
}

void drvif_memory_set_mtag_info(UINT32 buffer_size, UINT8 idx){
	MemTag[idx].Allocate_StartAddr = MemTag[idx].StartAddr;
	MemTag[idx].Size = buffer_size;
	MemTag[idx].Status = ALLOCATED_FROM_PLI;
	if((idx == MEMIDX_MAIN_SEC) || (idx == MEMIDX_MAIN))
		MemTag[idx].Allocate_VirAddr = 0;//not use
}

unsigned int drvif_memory_get_fixed_virtual_memory_size(void)
{
	return M_fixed_virtual_memory_size;
}

void drvif_memory_set_fixed_virtual_memory_size(unsigned int size)
{

	M_fixed_virtual_memory_size = size;
}

void set_m_domain_borrow_buffer(void *ptr, unsigned int size)
{//decide to free or save addr information
    if(ptr == NULL)
    {//request free memory case
        if (m_domain_borrow_buffer_virtual_addr != NULL)
        {//free memory case
            dvr_free(m_domain_borrow_buffer_virtual_addr);
            cma_borrow_size = 0;
            m_domain_borrow_buffer_virtual_addr = NULL;
        }
        return;
    }
    cma_borrow_size = size;//save size
    m_domain_borrow_buffer_virtual_addr = ptr; //save addr
}

void *get_m_domain_borrow_buffer(void)
{//get borrow memory addr from cma
    return m_domain_borrow_buffer_virtual_addr;
}

unsigned int get_m_domain_borrow_size(void)
{//get borrow size from cam
    return cma_borrow_size;
}

unsigned char m_domain_4k_memory_from_type(void)
{//4k memory size borrow type
    extern UINT8 get_vsc_gamemode_datafrc_mode_flag(void);

    VSC_INPUT_TYPE_T srctype;
    srctype = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);

    if ((srctype!= VSC_INPUTSRC_VDEC) && (srctype!= VSC_INPUTSRC_JPEG) && (srctype!= VSC_INPUTSRC_CAMERA) && (srctype!= VSC_INPUTSRC_AVD)
        && (0 == get_force_i3ddma_enable(SLR_MAIN_DISPLAY))) {
        return BORROW_FROM_VBM;//from vbm
    }
    else if((vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag()))
        return BORROW_FROM_MEMC;//from memc
    else if(get_vsc_gamemode_datafrc_mode_flag() && (is_game_mode_set_line_buffer() == false))
        return BORROW_FROM_MEMC;//from memc
    else if((Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID) > M_DOMAIN_WIDTH_2K) && (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN) > M_DOMAIN_LEN_2K)
            && ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_100000_mOFFSET ) )	// 3K 100/120 case borrow from MEMC
        return BORROW_FROM_MEMC;//from memc
    else
        return BORROW_FROM_CMA;//from CMA
}

bool is_m_domain_data_frc_need_borrow_memory(void)
{//3k 4k use data frc need borrow memory
    bool ret = false;
    extern bool is_caveout_memory_buffer_source(SCALER_DISP_CHANNEL display);
        calculate_carveout_size(SLR_MAIN_DISPLAY);
    if ((!Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC))
            && ((unsigned int)(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE) * Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)) >  (Get_Val_Max_Width() * Get_Val_Max_Len())
            && (is_caveout_memory_buffer_source(SLR_MAIN_DISPLAY) == false) /* only borrow memory when didn't scale down to caveout size */
                )
            )
    {//3k 4k use mdomain
        ret = true;
    }

    return ret;
}

bool vdec_data_frc_need_borrow_cma_buffer(unsigned char display)
{

	if(display != SLR_MAIN_DISPLAY)
		return false;

	if ((get_panel_res() == PANEL_RES_FHD) || (get_panel_res() == PANEL_RES_HD)) // 2k panel
		return false;

	//vdec 4k data frc need borrow memory
	if(check_sub_is_idle_for_srnn(display) && ((Get_DisplayMode_Src(display) == VSC_INPUTSRC_VDEC) || (Get_DisplayMode_Src(display) == VSC_INPUTSRC_JPEG)))
	{
		return true;
	}else{
		return false;
	}
}

void set_sub_m_domain_1st_buf_address(void *addr)
{
	sub_m_domain_1st_buf_addr = addr;
}

void *get_sub_m_domain_1st_buf_address(void)
{
	return sub_m_domain_1st_buf_addr;
}

void set_sub_m_domain_2nd_buf_address(void *addr)
{
	sub_m_domain_2nd_buf_addr = addr;
}

void *get_sub_m_domain_2nd_buf_address(void)
{
	return sub_m_domain_2nd_buf_addr;
}

void set_sub_m_domain_3rd_buf_address(void *addr)
{
	sub_m_domain_3rd_buf_addr = addr;
}

void *get_sub_m_domain_3rd_buf_address(void)
{
	return sub_m_domain_3rd_buf_addr;
}

/*
	API releated to CMA malloc, doesn't use carvedout memory.
	Start
*/

void set_m_domain_setting_err_status(unsigned char display, eMDomainSettingErrStatus err_status)
{// Set error status during M domain setting flow. If any error occurs, don't enable Mcap_en.
	m_domain_setting_err_status[display] = err_status;
}

unsigned int get_m_domain_setting_err_status(unsigned char display)
{// Get error status druing M domain setting flow. If any error occurs, don't enable Mcap_en.
	return m_domain_setting_err_status[display];
}

/* decide to free or save addr information */
void CMA_set_m_domain_original_buffer(unsigned long addr, unsigned int size, unsigned char CMA_buffer_index)
{
	rtd_pr_scaler_memory_info("[%s] buffer #%d %px -> %px\n", __func__, CMA_buffer_index, CMA_m_domain_original_buffer_physical_addr[CMA_buffer_index], addr);

	if (CMA_m_domain_original_buffer_physical_addr[CMA_buffer_index] != 0)
		pli_free(CMA_m_domain_original_buffer_physical_addr[CMA_buffer_index]);

	CMA_m_domain_original_buffer_physical_addr[CMA_buffer_index] = addr;
	if (addr == 0)
		CMA_m_domain_original_size[CMA_buffer_index] = 0;
	else
		CMA_m_domain_original_size[CMA_buffer_index] = size;
}

/* get origianl carvedout memory addr from cma */
unsigned long CMA_get_m_domain_original_buffer(unsigned char CMA_buffer_index)
{
	return CMA_m_domain_original_buffer_physical_addr[CMA_buffer_index];
}

/* get original carvedout size*/
unsigned int CMA_get_m_domain_original_size(unsigned char CMA_buffer_index)
{
	return CMA_m_domain_original_size[CMA_buffer_index];
}

void CMA_release_m_domain_original_buffer(void)
{
#ifndef UT_flag
	unsigned char CMA_buffer_index = 0;
	dcmt_cma_desc_t desc = {0};

	if(get_platform_model() == PLATFORM_MODEL_2K)
	{
		rtd_pr_scaler_memory_info("[%s] 2K model use CMA originally, no need release more buffer.\n", __func__);
		return;
	}

	if(CMA_buffer_number > MEMIDX_CMA_UNKNOWN)
	{
		rtd_pr_scaler_memory_err("[%s] ERROR! release buffer number(%d) > MEMIDX_CMA_UNKNOWN(%d), do nothing!\n", __func__, CMA_buffer_number, MEMIDX_CMA_UNKNOWN);
		return;
	}

	rtd_pr_scaler_memory_info("[%s] There are %d buffers need to be released.\n", __func__, CMA_buffer_number);

	// Disable main M domain DCMT
	dcmt_cma_mdomain_unset(&desc);  // Do unset DCMT before set DCMT

	for(CMA_buffer_index = 0; CMA_buffer_index < CMA_buffer_number ; CMA_buffer_index++)
		CMA_set_m_domain_original_buffer(0, 0, CMA_buffer_index);
#endif
}

unsigned char CMA_alloc_m_domain_original_buffer(void)
{
#ifndef UT_flag
	unsigned char CMA_buffer_index = 0;
	unsigned long CMA_m_domain_buffer = 0;
	dcmt_cma_desc_t desc = {0};

	if(get_platform_model() == PLATFORM_MODEL_2K)
	{
		rtd_pr_scaler_memory_info("[%s] 2K model use CMA originally, no need malloc more buffer.\n", __func__);
		return TRUE;
	}

	CMA_buffer_number = CMA_NEED_BUFFER_NUMBER_1;
	CMA_buffer_size = MDOMAIN_ORIGINAL_CARVEOUT_SIZE;

	if(CMA_buffer_number > MEMIDX_CMA_UNKNOWN)
	{
		rtd_pr_scaler_memory_err("[%s] ERROR! alloc buffer number(%d) > MEMIDX_CMA_UNKNOWN(%d), do nothing!\n", __func__, CMA_buffer_number, MEMIDX_CMA_UNKNOWN);
		return FALSE;
	}

	rtd_pr_scaler_memory_info("[%s] There are %d buffers need to be allocated, size:0x%x bytes\n", __func__, CMA_buffer_number, CMA_buffer_size);

	for(CMA_buffer_index = 0; CMA_buffer_index < CMA_buffer_number; CMA_buffer_index++)
	{
		CMA_m_domain_buffer = CMA_get_m_domain_original_buffer(CMA_buffer_index);
		if (CMA_m_domain_buffer == 0)
		{
			uint32_t time_stamp = IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg);

			CMA_m_domain_buffer = pli_malloc(CMA_buffer_size, GFP_DCU2_FIRST);
			rtd_pr_scaler_memory_info("[%s] allocate buffer #%d 0x%x bytes memory %px spend %03d msec\n",
					__func__,
					CMA_buffer_index,
					CMA_buffer_size,
					CMA_m_domain_buffer,
					(IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg) - time_stamp) / 90);

			CMA_set_m_domain_original_buffer(CMA_m_domain_buffer, CMA_buffer_size, CMA_buffer_index);
		}

		if (CMA_m_domain_buffer == INVALID_VAL)
		{
			rtd_pr_scaler_memory_err("###[Bug] func:%s no memory from cma ##\n", __func__);
			return FALSE;
		}
	}

	// Enable main M domain DCMT
	dcmt_cma_mdomain_unset(&desc);  // Do unset DCMT before set DCMT

	for(CMA_buffer_index = 0; CMA_buffer_index < CMA_buffer_number; CMA_buffer_index++)
	{
		desc.pa_start[CMA_buffer_index] = CMA_get_m_domain_original_buffer(CMA_buffer_index);
		desc.pa_size[CMA_buffer_index] = CMA_get_m_domain_original_size(CMA_buffer_index);
		desc.region_cnt++;
	}

	dcmt_cma_mdomain_set(&desc);  // Set DCMT
#endif

	return TRUE;
}

void CMA_reset_DCMT_impl(unsigned long start_addr, unsigned int size, eAdditional_CMA_DCMT reset_reason)
{// Add additional CMA buffer to DCMT. It may comes from i3ddma (VBM) or vdec data frc additional borrowed.
#ifndef UT_flag
	dcmt_cma_desc_t desc = {0};
	unsigned char CMA_buffer_index = 0;

	if((start_addr == 0) || (size == 0) || (reset_reason == ADDITIONAL_CMA_DCMT_UNKNOWN))
	{
		rtd_pr_scaler_memory_err("###[Bug] func:%s start_addr=0x%px, size=0x%x, reset_reason=%d ##\n", __func__, start_addr, size, reset_reason);
		return;
	}
	else if(reset_reason == ADDITIONAL_CMA_DCMT_I3DDMA)
	{
		rtd_pr_scaler_memory_info("[%s] Not use CMA alloc buffer(0x%px). Use i3ddma buffer borrowed from VBM. start_addr=0x%px, size=0x%x\n", __func__, CMA_get_m_domain_original_buffer(CMA_buffer_index), start_addr, size);
	}
	else if(reset_reason == ADDITIONAL_CMA_DCMT_VDEC_DATA_FRC)
	{
		rtd_pr_scaler_memory_info("[%s] Original CMA alloc buffer(0x%px). VDEC data frc additional main m domain borrowed. start_addr=0x%px, size=0x%x\n", __func__, CMA_get_m_domain_original_buffer(CMA_buffer_index), start_addr, size);
	}
	else if(reset_reason == ADDITIONAL_CMA_DCMT_VDEC_DATA_FRC_OR_MDOMAIN_CMA)
	{
		rtd_pr_scaler_memory_info("[%s] Original CMA alloc buffer(0x%px). VDEC data frc(%d)/m domain CMA(%d %d) additional borrowed. start_addr=0x%px, size=0x%x\n", __func__, CMA_get_m_domain_original_buffer(CMA_buffer_index), vdec_data_frc_need_borrow_cma_buffer(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)), is_m_domain_data_frc_need_borrow_memory(), m_domain_4k_memory_from_type(), start_addr, size);
	}
	else if(reset_reason == ADDITIONAL_CMA_DCMT_MDOMAIN_BORROW_MEMC_BUFFER)
	{
		rtd_pr_scaler_memory_info("[%s] Not use CMA alloc buffer(0x%px). Use MEMC buffer. start_addr=0x%px, size=0x%x\n", __func__, CMA_get_m_domain_original_buffer(CMA_buffer_index), start_addr, size);
	}

	// DCMT setting about the additional CMA buffer
	desc.pa_start[CMA_buffer_index] = start_addr;
	desc.pa_size[CMA_buffer_index] = size;
	desc.region_cnt++;

	// Set DCMT
	dcmt_cma_mdomain_reg_add(&desc);
#endif
}
void (*CMA_reset_DCMT)(unsigned long start_addr, unsigned int size, eAdditional_CMA_DCMT reset_reason) = CMA_reset_DCMT_impl;

/*
	Mdomain buffer borrow from MEMC case (ex: HDMI dolby VRR case)
	HDMI source with 120Hz panel, MEMC will alloc. 5 buffer from CMA (ref. to Scaler_MEMC_allocate_memory)
	MEMC buffer index 0 : 10MB for ME using
	MEMC buffer index 1 : 20MB for MC using
	MEMC buffer index 2 : 20MB for MC using
	MEMC buffer index 3 : 20MB for MC using
	MEMC buffer index 4 : 20MB for MC using
	Other source or panel please ref. to ML8QC-1258

	Mdomain will borrow MEMC buffer index 1 ~ 3

	input  par. : borrow_index , borrow which buffer in MEMC
	input  par. : borrow_addr  , borrow address
	input  par. : borrow_size  , borrow size
	output par. : TRUE means success, FALSE means fail
*/
unsigned char CMA_borrow_m_domain_buffer_from_MEMC(eMemIdx_CMA_MDOMAIN_BORROW_FROM_MEMC borrow_index, unsigned long *borrow_addr, unsigned int *borrow_size)
{
	extern unsigned int unconsistant_addr_aligned[9];
	extern unsigned int unconsistant_mem_size[9];

	if((unconsistant_addr_aligned[borrow_index] == INVALID_VAL) || (unconsistant_mem_size[borrow_index] < MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER))
	{
		rtd_pr_scaler_memory_info("[%s] ERROR! Borrow from MEMC buffer index(%d) fail. MEMC buffer addr.(0x%x) is invalid or size(0x%x < 0x%x)\n", __func__, borrow_index, unconsistant_addr_aligned[borrow_index], unconsistant_mem_size[borrow_index], MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER);
		return FALSE;
	}
	else
	{
		rtd_pr_scaler_memory_info("[%s] Borrow MEMC buffer index=%d, addr=0x%x, size=0x%x\n", __func__, borrow_index, unconsistant_addr_aligned[borrow_index], unconsistant_mem_size[borrow_index]);

		*borrow_addr = unconsistant_addr_aligned[borrow_index];
		*borrow_size = unconsistant_mem_size[borrow_index];

		CMA_reset_DCMT(*borrow_addr, *borrow_size, ADDITIONAL_CMA_DCMT_MDOMAIN_BORROW_MEMC_BUFFER);

		return TRUE;
	}
}
/*
	API releated to CMA malloc, doesn't use carvedout memory.
	End
*/
bool is_hdmi_dolby_vision_sink_led(void)
{
	bool result;

	if (get_HDMI_HDR_mode() != HDR_DOLBY_HDMI) {
		result = false;
	} else {
		if (get_HDMI_Dolby_VSIF_mode() == DOLBY_HDMI_VSIF_LL) {
			result = false;
		} else {
			result = true;
		}
	}

	return result;
}

#ifdef CONFIG_VBM_HEAP
int VBM_ReturnMemChunk(unsigned long dmabuf_hndl, unsigned long buffer_addr_in, unsigned long buffer_size_in);
int VBM_BorrowMemChunk(unsigned long *dmabuf_hndl, unsigned long *buffer_addr, unsigned long *buffer_size, UINT32 reqSize, UINT32 type);
#else
extern int VBM_ReturnMemChunk(unsigned long buffer_addr, unsigned long buffer_size);
extern int VBM_BorrowMemChunk(unsigned long *buffer_addr, unsigned long *buffer_size, UINT32 reqSize ,UINT32 type);
#endif
extern unsigned long I3DDMA_GetMemChunkAddr(unsigned int size);
extern struct semaphore VPQ_DeXC_MEM_Semaphore;/*For Dexc MEM*/

unsigned int  drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(unsigned char DI_MEM_IDX, unsigned int DI_Width, unsigned int DI_Height, unsigned char Force_Clr)
{
#ifndef BUILD_QUICK_SHOW
#if 0
	typedef enum _VIP_I_De_XC_MEM_BORROW_MODE{
		De_XC_MEM_UNKNOW = 0,
		De_XC_MEM_From_VBM_Directly,
		De_XC_MEM_From_I3D_DMA,	/* memory will release by itself in i3ddma*/
		I_De_XC_MEM_BORROW_MODE_Max,

	} VIP_I_De_XC_MEM_BORROW_MODE;

	typedef struct _VIP_I_De_XC_MEM {
		unsigned long phyaddr;
		/*unsigned int size;*/
		unsigned int getsize;
		unsigned char Borrow_Mode;

	} VIP_I_De_XC_MEM;

	static VIP_I_De_XC_MEM mem_ctrl[2] = {0};
#endif

	VIP_I_De_XC_MEM *mem_ctrl[2];
//	_system_setting_info* system_info_structure_table = NULL;
	unsigned char add_more = 0x10;
	unsigned int de_xc_size = 0x17CCCD0 + add_more; //  23.8MB   //0x18B8200 + add_more;	/*24.71923828 Mb*/
	unsigned int phyaddr = 0;
	unsigned int getSize = 0;

	unsigned long phyaddr_l = 0;
	unsigned long getSize_l = 0;
#ifdef CONFIG_VBM_HEAP
	static unsigned long dexc_dmabuf_hndl = 0;
#endif

	//_system_setting_info* system_info_structure_table = NULL;
	//system_info_structure_table = (_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_system_info_structure);
	_RPC_system_setting_info* RPC_system_info_structure_table = NULL;
	RPC_system_info_structure_table = (_RPC_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
	if (RPC_system_info_structure_table == NULL) {
		rtd_pr_scaler_memory_emerg("De-XC mem, system info structure NULL !!!\n");
		return 0;
	}
	mem_ctrl[0] = &RPC_system_info_structure_table->DE_XCXL_Memory_CTRL[0];
	mem_ctrl[1] = &RPC_system_info_structure_table->DE_XCXL_Memory_CTRL[1];
	down(&VPQ_DeXC_MEM_Semaphore);

	rtd_pr_scaler_memory_info("De-XC mem in VBM function,idx=%d,DI_w=%d,DI_h=%d,F_clr=%d\n",  DI_MEM_IDX, DI_Width, DI_Height, Force_Clr);

	if (Force_Clr == 1) {
		if ((mem_ctrl[DI_MEM_IDX]->phyaddr != 0) && (mem_ctrl[DI_MEM_IDX]->Borrow_Mode == De_XC_MEM_From_VBM_Directly)) {
			rtd_pr_scaler_memory_debug("[%s] call VBM_ReturnMemChunk\n",__FUNCTION__);
			rtd_pr_scaler_memory_info("De-XC mem force clear, return memroy from VBM = %x\n", mem_ctrl[DI_MEM_IDX]->phyaddr);
#ifdef CONFIG_VBM_HEAP
			VBM_ReturnMemChunk(dexc_dmabuf_hndl, mem_ctrl[DI_MEM_IDX]->phyaddr, mem_ctrl[DI_MEM_IDX]->getsize);
			dexc_dmabuf_hndl = 0;
#else
			VBM_ReturnMemChunk(mem_ctrl[DI_MEM_IDX]->phyaddr, mem_ctrl[DI_MEM_IDX]->getsize);
#endif
		}
		mem_ctrl[DI_MEM_IDX]->phyaddr = 0;
		mem_ctrl[DI_MEM_IDX]->getsize = 0;
		mem_ctrl[DI_MEM_IDX]->Borrow_Mode = De_XC_MEM_UNKNOW;
		rtd_pr_scaler_memory_info("De-XC mem force clear, mem tag reset\n");
	} else {
		if (DbgSclrFlgTkr.Main_Scaler_Stop_flag == FALSE) { /* connection check. */
			if (mem_ctrl[DI_MEM_IDX]->phyaddr == 0) {
				phyaddr = (unsigned int)I3DDMA_GetMemChunkAddr(de_xc_size);
				if ( phyaddr != 0) {
					mem_ctrl[DI_MEM_IDX]->phyaddr =  phyaddr;//(unsigned int)dvr_to_phys((void*)i3ddmaCtrl.cap_buffer[0].cache);
					mem_ctrl[DI_MEM_IDX]->getsize = de_xc_size;
					mem_ctrl[DI_MEM_IDX]->Borrow_Mode = De_XC_MEM_From_I3D_DMA;
					/*drv_memory_Set_I_De_XC_Mem_Ready_Flag(DI_MEM_IDX, 1);*/	/* num, len, water... is not ready*/
					rtd_pr_scaler_memory_info("De-XC mem alloc from i3ddma\n");

				} else {
					rtd_pr_scaler_memory_debug("[%s] call VBM_BorrowMemChunk\n",__FUNCTION__);
#ifdef CONFIG_VBM_HEAP
					if (VBM_BorrowMemChunk(&dexc_dmabuf_hndl, &phyaddr_l, &getSize_l, de_xc_size, 0) != 0) {
#else
					if (VBM_BorrowMemChunk(&phyaddr_l, &getSize_l, de_xc_size, 0) != 0) {
#endif
						mem_ctrl[DI_MEM_IDX]->phyaddr = 0;
						mem_ctrl[DI_MEM_IDX]->getsize = 0;
						mem_ctrl[DI_MEM_IDX]->Borrow_Mode = De_XC_MEM_UNKNOW;
						rtd_pr_scaler_memory_emerg("De-XC alloc idma video sequence memory failed\n");
					} else {
						phyaddr = (unsigned int)phyaddr_l;
						getSize = (unsigned int)getSize_l;
						if (getSize < de_xc_size) {
							mem_ctrl[DI_MEM_IDX]->phyaddr = 0;
							mem_ctrl[DI_MEM_IDX]->getsize = 0;
							mem_ctrl[DI_MEM_IDX]->Borrow_Mode = De_XC_MEM_UNKNOW;
							rtd_pr_scaler_memory_emerg("De-XC Memory alloc is not enough, get_szie=%x,size=%x\n", getSize, de_xc_size);
						} else {
							mem_ctrl[DI_MEM_IDX]->phyaddr =  (unsigned int)phyaddr ;//(unsigned int)dvr_to_phys((void*)i3ddmaCtrl.cap_buffer[0].cache);
							mem_ctrl[DI_MEM_IDX]->getsize = (unsigned int)getSize ;
							mem_ctrl[DI_MEM_IDX]->Borrow_Mode = De_XC_MEM_From_VBM_Directly;
							/*drv_memory_Set_I_De_XC_Mem_Ready_Flag(DI_MEM_IDX, 1);*/	/* num, len, water... is not ready*/
							rtd_pr_scaler_memory_info("De-XC mem alloc from VBM Directly = %x\n", (unsigned int)mem_ctrl[DI_MEM_IDX]->phyaddr);
						}
					}
				}
			} else {
				rtd_pr_scaler_memory_info("De-XC mem already get from %d\n", mem_ctrl[DI_MEM_IDX]->Borrow_Mode);
			}
		} else {
			if ((mem_ctrl[DI_MEM_IDX]->phyaddr != 0) && (mem_ctrl[DI_MEM_IDX]->Borrow_Mode == De_XC_MEM_From_VBM_Directly)) {
				rtd_pr_scaler_memory_debug("[%s] call VBM_ReturnMemChunk\n",__FUNCTION__);
				rtd_pr_scaler_memory_emerg("De-XC mem disconnect, return memroy from VBM = %x\n", (unsigned int)mem_ctrl[DI_MEM_IDX]->phyaddr);
#ifdef CONFIG_VBM_HEAP
				VBM_ReturnMemChunk(dexc_dmabuf_hndl, mem_ctrl[DI_MEM_IDX]->phyaddr, mem_ctrl[DI_MEM_IDX]->getsize);
				dexc_dmabuf_hndl = 0;
#else
				VBM_ReturnMemChunk(mem_ctrl[DI_MEM_IDX]->phyaddr, mem_ctrl[DI_MEM_IDX]->getsize);
#endif
			}
			mem_ctrl[DI_MEM_IDX]->phyaddr = 0;
			mem_ctrl[DI_MEM_IDX]->getsize = 0;
			mem_ctrl[DI_MEM_IDX]->Borrow_Mode = De_XC_MEM_UNKNOW;
			rtd_pr_scaler_memory_info("De-XC Memory alloc, VSC has disconnected\n");
		}
	}

	if (mem_ctrl[DI_MEM_IDX]->phyaddr == 0) {
		/*no memory get, turn off de-xc immediately*/
		IoReg_ClearBits(DE_XCXL_DE_XCXL_db_reg_ctl_reg, DE_XCXL_DE_XCXL_db_reg_ctl_db_en_mask);	/* turn off de-xc double buffer,  turn off de-xc immediately*/
		IoReg_ClearBits(DE_XCXL_De_XCXL_CTRL_reg, DE_XCXL_De_XCXL_CTRL_dexc_en_mask);	/* turn off de-xc */
		drv_memory_Set_I_De_XC_Mem_Ready_Flag(DI_MEM_IDX, 0);
	}

	up(&VPQ_DeXC_MEM_Semaphore);

	return (unsigned int)mem_ctrl[DI_MEM_IDX]->phyaddr;
#else
    return 0;
#endif
}

unsigned int  drv_memory_I_De_XC_DMA_Config(unsigned int DI_phyAddr, unsigned char deXC_mode)
{
	typedef struct _VIP_I_De_XC_DMA {
	    unsigned short  dexc_height;
	    unsigned short  dexc_width;
	    //unsigned char   dexc_pixel_bpp;
	    //unsigned char   dexc_frame_format;
	    unsigned char dexc_data_format;
	    unsigned int    dexc_mem_start;
	    unsigned int    dexc_dat_msize;
	    unsigned int    dexc_inf_msize;
	    //unsigned char   dexc_dat_water_r1;
	    unsigned char   dexc_dat_len_r1;
	    unsigned short  dexc_dat_num_r1;
	    //unsigned char   dexc_dat_water_r2;
	    unsigned char   dexc_dat_len_r2;
	    unsigned short  dexc_dat_num_r2;
	    //unsigned char   dexc_dat_water_w;
	    unsigned char   dexc_dat_len_w;
	    unsigned short  dexc_dat_num_w;
	    unsigned char   dexc_wdma_remain_en;
	    unsigned char   dexc_dat_remain_w;
	    unsigned char   dexc_rdma1_remain_en;
	    unsigned char   dexc_dat_remain_r1;
	    unsigned char   dexc_rdma2_remain_en;
	    unsigned char   dexc_dat_remain_r2;
	    //unsigned char   dexc_inf_water_r;
	    unsigned char   dexc_inf_len_r;
	    //unsigned short  dexc_inf_num_r;
	    //unsigned char   dexc_inf_water_w;
	    unsigned char   dexc_inf_len_w;
	    //unsigned short  dexc_inf_num_w;
	    //unsigned char   dexc_inf_remain_w_en;
	    //unsigned char   dexc_inf_remain_w;
	    //unsigned char   dexc_inf_remain_r_en;
	    //unsigned char   dexc_inf_remain_r;
	    unsigned char dexc_inf_cmp_enable;
	    unsigned char dexc_info_rolling_en;
	} VIP_I_De_XC_DMA;

	VIP_I_De_XC_DMA data;

	/*unsigned int phyAddr = NULL;*/
	unsigned int phyAddr = 0;
	unsigned int height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
	unsigned int width = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID);
	unsigned char is422;
	unsigned char is8bit;
	unsigned char data_format; // 0: y10c6, 1: y8c8
	unsigned char isInfo_Compress;
	unsigned char isInfo_rolling;
	unsigned char DI_MEM_IDX;
	// info comp. default on

	UINT32 field_size, info_size;
	UINT32 data_bit, info_bit;
	UINT32 mem_end = 0;

	int yuv_size;
	int data_len;
	// dma settings
	UINT32 bus_rw_bits = 128;
	UINT32 rw_num, num, remain, length, info_len, dat_len;
	//UINT32 total;
	unsigned short aligned_val;

#ifdef CONFIG_I2RND_DUAL_VO
	DI_MEM_IDX = (Scaler_I2rnd_get_display()<=1)?(0):(1);	// Scaler_I2rnd_get_display() ==> 0: i2r disable, 1:i2r s0, 2:i2r s1
#else
	DI_MEM_IDX = 0;
#endif

	if (deXC_mode == DeXCXL_Y10C6) {
		data_format = 0;
		isInfo_Compress = 0;
		isInfo_rolling =0;
	} else if (deXC_mode == DeXCXL_Y8C8) {
		data_format = 1;
		isInfo_Compress = 0;
		isInfo_rolling = 0;
	} else if (deXC_mode == DeXCXL_Y10C6_CMP) {
		data_format = 0;
		isInfo_Compress = 1;
		isInfo_rolling = 1;
	} else if (deXC_mode == DeXCXL_Y8C8_CMP) {
		data_format = 1;
		isInfo_Compress = 1;
		isInfo_rolling = 1;
	} else {
		drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(DI_MEM_IDX, 0, 0, 1);		/* force to release memory*/

		rtd_pr_scaler_memory_info("DE-XCXL, disable, deXC_mode=%d\n", deXC_mode);
		return 0;
	}

	phyAddr = drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(DI_MEM_IDX, width, height, 0);
	rtd_pr_scaler_memory_info("DE-XCXL, get memory, memStaAddr=%x\n", phyAddr);


	if(phyAddr == 0) {
		rtd_pr_scaler_memory_emerg("De-XC alloc memory failed, diable De-XC, return !!!!!!!!!!!\n");
		return 0;
	}
#if 0
	if( height > 540  || is_usbPlayback == 1)
		is422 = 1; // 422 for 1080p and usbplayback
	else
		is422 = 0;
#endif

#ifdef CONFIG_BW_96B_ALIGNED
	aligned_val = 6;
#else
	aligned_val = 0;
#endif

	is8bit = 1; // fixed to 422, 8-bit, Merlin4
	is422 = 1;

	yuv_size = is422 ? 2 : 3;
	data_len = is8bit ? 8 : 10;
	data_bit = yuv_size * data_len;
	info_bit = (isInfo_Compress == 0)?(22):(24);	// for m4, // normal case 16, worth case 24 for info compression
	info_len = 10;
	dat_len = 32;

	data.dexc_inf_cmp_enable = isInfo_Compress;
	data.dexc_info_rolling_en = isInfo_rolling;
	data.dexc_height = height;
	data.dexc_width = width;
	//data.dexc_pixel_bpp = is8bit;
	//data.dexc_frame_format = is422;
	data.dexc_data_format = data_format;

	// address calculation
	field_size = width * height * data_bit; // bit
	if( (field_size & 0x7F) > 0 )
		field_size = ( (field_size>>7)+1 ) << 4; // round up on 16-byte unit
	else
		field_size = field_size >> 3; // byte

	if (isInfo_rolling == 1) {
		//info_size = (ceil( ceil((height * (width>>1) * info_bit) / 128) *2)/ info_len) * info_len;
		length = info_len;
		if (aligned_val != 0)
			length = ((length%aligned_val)==0)?(length):(length + (aligned_val - (length % aligned_val)));	/* lenght aligned with "aligned_val" */

		info_size = ((height * (width>>1) * info_bit)+127)>>7;	// unit 128 bit, and ceil() with 128b
		info_size = ((info_size + (length-1)) / length) * length; // ceil() with "info_len"
		info_size = info_size<<4;	// unit byte

	} else {
		info_size = ((width+1)>>1) * height * info_bit; // bit
		if( (info_size & 0x7F) > 0 )
			info_size = ( (info_size>>7)+1 ) << 4; // round up on 16-byte unit
		else
			info_size = info_size >> 3; // byte
	}
	/*// test code for de_xcxl DMA settings, 20160722 jimmy
	if(phyAddr!=NULL)
		pli_free(phyAddr);

	// temp code for memory allocation
	phyAddr = pli_malloc( tot_size, GFP_DCU2_FIRST );*/

	/*info_size += info_size;*/
	/*field_size += 0x10;*/

	// add more for aligned used
	field_size = field_size + 1024;
	info_size = info_size + 1024;

	data.dexc_mem_start = phyAddr;
	data.dexc_dat_msize = field_size;
	data.dexc_inf_msize = info_size;

	// data dma
	length = dat_len;
	if (aligned_val != 0)
		length = ((length%aligned_val)==0)?(length):(length + (aligned_val - (length % aligned_val)));	/* lenght aligned with "aligned_val" */

	rw_num = width*height*data_bit;
	if( rw_num % bus_rw_bits > 0 )
		rw_num = rw_num / bus_rw_bits + 1;
	else
		rw_num = rw_num / bus_rw_bits;

	num = rw_num / length;
	remain= rw_num - (num*length);
	if( remain==0 )
	{
		remain = length;
		num--;
	}
	//total = 128;

	//data.dexc_dat_water_r1 = total-length;
	data.dexc_dat_len_r1 = length;
	data.dexc_dat_num_r1 = num;

	//data.dexc_dat_water_r2 = total-length;
	data.dexc_dat_len_r2 = length;
	data.dexc_dat_num_r2 = num;

	//data.dexc_dat_water_w = length;
	data.dexc_dat_len_w = length;
	data.dexc_dat_num_w = num;

	data.dexc_wdma_remain_en = 1;
	data.dexc_dat_remain_w = remain;
	data.dexc_rdma1_remain_en = 1;
	data.dexc_dat_remain_r1 = remain;
	data.dexc_rdma2_remain_en = 1;
	data.dexc_dat_remain_r2 = remain;

	// info dma
	length = info_len;
	if (aligned_val != 0)
		length = ((length%aligned_val)==0)?(length):(length + (aligned_val - (length % aligned_val)));	/* lenght aligned with "aligned_val" */

	rw_num = width*height*info_bit/2;
	if( rw_num % bus_rw_bits > 0 )
		rw_num = rw_num / bus_rw_bits + 1;
	else
		rw_num = rw_num / bus_rw_bits;

	num = rw_num / length;
	remain= rw_num - (num*length);
	if( remain==0 )
	{
		remain = length;
		num--;
	}
	//total = 32;

	//data.dexc_inf_water_r = total-length;
	data.dexc_inf_len_r = length;
	//data.dexc_inf_num_r = num;

	//data.dexc_inf_water_w = length;
	data.dexc_inf_len_w = length;
	//data.dexc_inf_num_w = num;

	//data.dexc_inf_remain_w_en = 1;
	//data.dexc_inf_remain_w = remain;
	//data.dexc_inf_remain_r_en = 1;
	//data.dexc_inf_remain_r = remain;

	mem_end = drv_memory_Set_I_De_XC_DMA( (void*)&data, DI_MEM_IDX);

	if (mem_end == 0)
		drv_memory_Set_I_De_XC_Mem_Ready_Flag(DI_MEM_IDX, 0);
	else
		drv_memory_Set_I_De_XC_Mem_Ready_Flag(DI_MEM_IDX, 1);

	/*de-xc need a few frame for memory stable after enable, enable control @ scalerMemory*/
	//Scaler_Set_I_De_XC(0);

	rtd_pr_scaler_memory_info("DE-XCXL, idx=%d,mem sta=%x,mem end=%x, Ready Flag=%d\n",
		DI_MEM_IDX, phyAddr, mem_end, drv_memory_Get_I_De_XC_Mem_Ready_Flag(DI_MEM_IDX));

	return mem_end;
}

unsigned int drv_memory_Set_I_De_XC_DMA(void *dma_data, unsigned char DI_MEM_IDX)
{
	typedef struct _VIP_I_De_XC_DMA {
	    unsigned short  dexc_height;
	    unsigned short  dexc_width;
	    //unsigned char   dexc_pixel_bpp;
	    //unsigned char   dexc_frame_format;
	    unsigned char  dexc_data_format;
	    unsigned int    dexc_mem_start;
	    unsigned int    dexc_dat_msize;
	    unsigned int    dexc_inf_msize;
	    //unsigned char   dexc_dat_water_r1;
	    unsigned char   dexc_dat_len_r1;
	    unsigned short  dexc_dat_num_r1;
	    //unsigned char   dexc_dat_water_r2;
	    unsigned char   dexc_dat_len_r2;
	    unsigned short  dexc_dat_num_r2;
	    //unsigned char   dexc_dat_water_w;
	    unsigned char   dexc_dat_len_w;
	    unsigned short  dexc_dat_num_w;
	    unsigned char   dexc_wdma_remain_en;
	    unsigned char   dexc_dat_remain_w;
	    unsigned char   dexc_rdma1_remain_en;
	    unsigned char   dexc_dat_remain_r1;
	    unsigned char   dexc_rdma2_remain_en;
	    unsigned char   dexc_dat_remain_r2;
	    //unsigned char   dexc_inf_water_r;
	    unsigned char   dexc_inf_len_r;
	    //unsigned short  dexc_inf_num_r;
	    //unsigned char   dexc_inf_water_w;
	    unsigned char   dexc_inf_len_w;
	    //unsigned short  dexc_inf_num_w;
	    //unsigned char   dexc_inf_remain_w_en;
	    //unsigned char   dexc_inf_remain_w;
	    //unsigned char   dexc_inf_remain_r_en;
	    //unsigned char   dexc_inf_remain_r;
	    unsigned char dexc_inf_cmp_enable;
		unsigned char dexc_info_rolling_en;
	} VIP_I_De_XC_DMA;
	unsigned int i;
	UINT32 baseAddr;
	//UINT32 nBaseAddr;
	//UINT32 nFldSize;
	//UINT32 nInfSize;
	UINT32 tmpAddr[8];
	UINT32 align_addr_aligned[8];
	UINT32 field_size, info_size, field_size_aligned, info_size_aligned;
	UINT32 mem_end;
	unsigned short aligned_val;
	//UINT32 data_bit, info_bit;
	VIP_I_De_XC_DMA *ptr = (VIP_I_De_XC_DMA*) dma_data;
	de_xcxl_de_xcxl_ctrl_RBUS de_xcxl_ctrl_reg;
	de_xcxl_frame_format_RBUS de_xcxl_frame_format_reg;
	//de_xcxl_datmemstartadd1_s0_RBUS 		de_xcxl_datmemstartadd1_s0_reg;
	de_xcxl_datmemstartadd2_s0_RBUS 		de_xcxl_datmemstartadd2_s0_reg;
	de_xcxl_datmemstartadd3_s0_RBUS 		de_xcxl_datmemstartadd3_s0_reg;
	de_xcxl_infmemstartadd_s0_RBUS		    de_xcxl_infmemstartadd_s0_reg;
	de_xcxl_infmemstartadd_1_s0_RBUS de_xcxl_infmemstartadd_1_s0_reg;		// m4
	//de_xcxl_datmemstartadd1_odd_s0_RBUS 	de_xcxl_datmemstartadd1_odd_s0_reg;
	de_xcxl_datmemstartadd2_odd_s0_RBUS 	de_xcxl_datmemstartadd2_odd_s0_reg;
	de_xcxl_datmemstartadd3_odd_s0_RBUS 	de_xcxl_datmemstartadd3_odd_s0_reg;
	de_xcxl_infmemstartadd_odd_s0_RBUS	    de_xcxl_infmemstartadd_odd_s0_reg;
	de_xcxl_infmemstartadd_1_odd_s0_RBUS de_xcxl_infmemstartadd_1_odd_s0_reg;		// m4

	//de_xcxl_datmemstartadd1_s1_RBUS 		de_xcxl_datmemstartadd1_s1_reg;
	de_xcxl_datmemstartadd2_s1_RBUS 		de_xcxl_datmemstartadd2_s1_reg;
	de_xcxl_datmemstartadd3_s1_RBUS 		de_xcxl_datmemstartadd3_s1_reg;
	de_xcxl_infmemstartadd_s1_RBUS		    de_xcxl_infmemstartadd_s1_reg;
	de_xcxl_infmemstartadd_1_s1_RBUS	de_xcxl_infmemstartadd_1_s1_reg;		// m4
	//de_xcxl_datmemstartadd1_odd_s1_RBUS 	de_xcxl_datmemstartadd1_odd_s1_reg;
	de_xcxl_datmemstartadd2_odd_s1_RBUS 	de_xcxl_datmemstartadd2_odd_s1_reg;
	de_xcxl_datmemstartadd3_odd_s1_RBUS 	de_xcxl_datmemstartadd3_odd_s1_reg;
	de_xcxl_infmemstartadd_odd_s1_RBUS	    de_xcxl_infmemstartadd_odd_s1_reg;
	de_xcxl_infmemstartadd_odd_1_s1_RBUS		de_xcxl_infmemstartadd_odd_1_s1_reg;		// m4

	de_xcxl_de_xcxl_db_reg_ctl_RBUS	de_xcxl_de_xcxl_db_reg_ctl_reg;
	de_xcxl_wrrd_bndaddr1_RBUS de_xcxl_wrrd_bndaddr1_reg;
	de_xcxl_wrrd_bndaddr2_RBUS de_xcxl_wrrd_bndaddr2_reg;
	de_xcxl_rdatdma_set1_RBUS de_xcxl_rdatdma_set1_reg;
	de_xcxl_rdatdma_set2_RBUS de_xcxl_rdatdma_set2_reg;
	de_xcxl_wdatdma_set_RBUS de_xcxl_wdatdma_set_reg;
	de_xcxl_datdma_rm_set_RBUS de_xcxl_datdma_rm_set_reg;
	de_xcxl_rinfdma_set_RBUS de_xcxl_rinfdma_set_reg;
	de_xcxl_winfdma_set_RBUS de_xcxl_winfdma_set_reg;
	//de_xcxl_infdma_rm_set_RBUS de_xcxl_infdma_rm_set_reg;
	de_xcxl_dma_multi_req_set_RBUS	de_xcxl_dma_multi_req_set_reg;

#ifdef CONFIG_BW_96B_ALIGNED
	aligned_val = 6;
#else
	aligned_val = 0;
#endif

	/* db enable*/
	IoReg_SetBits(DE_XCXL_DE_XCXL_db_reg_ctl_reg, DE_XCXL_DE_XCXL_db_reg_ctl_db_en_mask);

	// data format and CMP
	de_xcxl_ctrl_reg.regValue = IoReg_Read32(DE_XCXL_De_XCXL_CTRL_reg);
	de_xcxl_ctrl_reg.dexc_data_format = ptr->dexc_data_format;
	de_xcxl_ctrl_reg.dexc_info_comp = ptr->dexc_inf_cmp_enable;
	de_xcxl_ctrl_reg.dexc_info_rolling_en = ptr->dexc_info_rolling_en;
	IoReg_Write32( DE_XCXL_De_XCXL_CTRL_reg, de_xcxl_ctrl_reg.regValue );

	// frame format
	de_xcxl_frame_format_reg.regValue = IoReg_Read32(DE_XCXL_Frame_format_reg);

	de_xcxl_frame_format_reg.height         = ptr->dexc_height;
	de_xcxl_frame_format_reg.width          = ptr->dexc_width;
	//de_xcxl_frame_format_reg.pixel_bpp      = ptr->dexc_pixel_bpp;
	//de_xcxl_frame_format_reg.frame_format   = ptr->dexc_frame_format;

	IoReg_Write32( DE_XCXL_Frame_format_reg, de_xcxl_frame_format_reg.regValue );

	// address calculation
	baseAddr    = ptr->dexc_mem_start;
	field_size  = ptr->dexc_dat_msize;
	info_size   = ptr->dexc_inf_msize;

	if (aligned_val != 0) {
		field_size_aligned = dvr_size_alignment(field_size);
		info_size_aligned = dvr_size_alignment(info_size);
	} else {
		field_size_aligned = drvif_memory_get_data_align(field_size, 16);
		info_size_aligned = drvif_memory_get_data_align(info_size, 16);
	}
	//mem_end = baseAddr + field_size *6 + info_size *2;

	//nBaseAddr = (baseAddr>>3)&0x1fffffff;
	//nFldSize = (field_size>>3)&0x1fffffff; // 8-byte
	//nInfSize = (info_size>>3)&0x1fffffff;

	//remove addr1 from merlin3 @Crixus 20170515
	//de_xcxl_datmemstartadd1_s0_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd1_S0_reg);
	de_xcxl_datmemstartadd2_s0_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd2_S0_reg);
	de_xcxl_datmemstartadd3_s0_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd3_S0_reg);
	de_xcxl_infmemstartadd_s0_reg.regValue = IoReg_Read32(DE_XCXL_INFMemStartAdd_S0_reg);
	de_xcxl_infmemstartadd_1_s0_reg.regValue  = IoReg_Read32( DE_XCXL_INFMemStartAdd_1_S0_reg);
	//de_xcxl_datmemstartadd1_odd_s0_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd1_Odd_S0_reg);
	de_xcxl_datmemstartadd2_odd_s0_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd2_Odd_S0_reg);
	de_xcxl_datmemstartadd3_odd_s0_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd3_Odd_S0_reg);
	de_xcxl_infmemstartadd_odd_s0_reg.regValue = IoReg_Read32(DE_XCXL_INFMemStartAdd_Odd_S0_reg);
	de_xcxl_infmemstartadd_1_odd_s0_reg.regValue = IoReg_Read32( DE_XCXL_INFMemStartAdd_1_Odd_S0_reg);

	//de_xcxl_datmemstartadd1_s1_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd1_S1_reg);
	de_xcxl_datmemstartadd2_s1_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd2_S1_reg);
	de_xcxl_datmemstartadd3_s1_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd3_S1_reg);
	de_xcxl_infmemstartadd_s1_reg.regValue = IoReg_Read32(DE_XCXL_INFMemStartAdd_S1_reg);
	de_xcxl_infmemstartadd_1_s1_reg.regValue = IoReg_Read32( DE_XCXL_INFMemStartAdd_1_S1_reg);
	//de_xcxl_datmemstartadd1_odd_s1_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd1_Odd_S1_reg);
	de_xcxl_datmemstartadd2_odd_s1_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd2_Odd_S1_reg);
	de_xcxl_datmemstartadd3_odd_s1_reg.regValue = IoReg_Read32(DE_XCXL_DATMemStartAdd3_Odd_S1_reg);
	de_xcxl_infmemstartadd_odd_s1_reg.regValue = IoReg_Read32(DE_XCXL_INFMemStartAdd_Odd_S1_reg);
	de_xcxl_infmemstartadd_odd_1_s1_reg.regValue = IoReg_Read32( DE_XCXL_INFMemStartAdd_Odd_1_S1_reg);

	// Even
	tmpAddr[0] = baseAddr;
	tmpAddr[1] = tmpAddr[0] + field_size_aligned;
	tmpAddr[2] = tmpAddr[1] + field_size_aligned;
	if (ptr->dexc_inf_cmp_enable == 1)
		tmpAddr[3] = tmpAddr[2] + info_size_aligned;
	else
		tmpAddr[3] = tmpAddr[2];
	//Odd
	tmpAddr[4] = tmpAddr[3] + info_size_aligned;
	tmpAddr[5] = tmpAddr[4] + field_size_aligned;
	tmpAddr[6] = tmpAddr[5] + field_size_aligned;
	if (ptr->dexc_inf_cmp_enable == 1)
		tmpAddr[7] = tmpAddr[6] + info_size_aligned;
	else
		tmpAddr[7] = tmpAddr[6];

	if (aligned_val != 0) {
		// Even
		align_addr_aligned[0] = dvr_memory_alignment((unsigned long)tmpAddr[0], field_size_aligned);
		align_addr_aligned[1] = dvr_memory_alignment((unsigned long)tmpAddr[1], field_size_aligned);
		align_addr_aligned[2] = dvr_memory_alignment((unsigned long)tmpAddr[2], info_size_aligned);
		if (ptr->dexc_inf_cmp_enable == 1)
			align_addr_aligned[3] = dvr_memory_alignment((unsigned long)tmpAddr[3], info_size_aligned);
		else
			align_addr_aligned[3] = align_addr_aligned[2];
		// Odd
		align_addr_aligned[4] = dvr_memory_alignment((unsigned long)tmpAddr[4], field_size_aligned);
		align_addr_aligned[5] = dvr_memory_alignment((unsigned long)tmpAddr[5], field_size_aligned);
		align_addr_aligned[6] = dvr_memory_alignment((unsigned long)tmpAddr[6], info_size_aligned);
		if (ptr->dexc_inf_cmp_enable == 1)
			align_addr_aligned[7] = dvr_memory_alignment((unsigned long)tmpAddr[7], info_size_aligned);
		else
			align_addr_aligned[7] = align_addr_aligned[6];

		for (i=0;i<8;i++) {
			if (align_addr_aligned[i] == 0) {
				rtd_pr_scaler_memory_emerg("DE-XCXL, memory 96 byte alignment fail, addr=%x, index=%d, field_size_96=%d, info_size_96=%d\n",
					tmpAddr[i], i, field_size_aligned, info_size_aligned);
				BUG();
			}
		}
	} else {
		// Even
		align_addr_aligned[0] = drvif_memory_get_data_align(tmpAddr[0], 16);
		align_addr_aligned[1] = drvif_memory_get_data_align(tmpAddr[1], 16);
		align_addr_aligned[2] = drvif_memory_get_data_align(tmpAddr[2], 16);
		align_addr_aligned[3] = drvif_memory_get_data_align(tmpAddr[3], 16);
		// Odd
		align_addr_aligned[4] = drvif_memory_get_data_align(tmpAddr[4], 16);
		align_addr_aligned[5] = drvif_memory_get_data_align(tmpAddr[5], 16);
		align_addr_aligned[6] = drvif_memory_get_data_align(tmpAddr[6], 16);
		align_addr_aligned[7] = drvif_memory_get_data_align(tmpAddr[7], 16);
	}

	// memory end
	mem_end = align_addr_aligned[7] + info_size_aligned;

	if (DI_MEM_IDX == 0) {
		// even
		de_xcxl_datmemstartadd2_s0_reg.s0_dat_mstart_57_29 = (align_addr_aligned[0]>>3)&0x1fffffff;
		de_xcxl_datmemstartadd3_s0_reg.s0_dat_mstart_28_0 = (align_addr_aligned[1]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_s0_reg.s0_inf_mstart = (align_addr_aligned[2]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_1_s0_reg.s0_1_inf_mstart = (align_addr_aligned[3]>>3)&0x1fffffff;
		// Odd
		de_xcxl_datmemstartadd2_odd_s0_reg.s0_dat_mstart_odd_57_29 = (align_addr_aligned[4]>>3)&0x1fffffff;
		de_xcxl_datmemstartadd3_odd_s0_reg.s0_dat_mstart_odd_28_0 = (align_addr_aligned[5]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_odd_s0_reg.s0_inf_mstart_odd = (align_addr_aligned[6]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_1_odd_s0_reg.s0_1_inf_mstart_odd = (align_addr_aligned[7]>>3)&0x1fffffff;

		IoReg_Write32( DE_XCXL_DATMemStartAdd2_S0_reg, de_xcxl_datmemstartadd2_s0_reg.regValue );
		IoReg_Write32( DE_XCXL_DATMemStartAdd3_S0_reg, de_xcxl_datmemstartadd3_s0_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_S0_reg, de_xcxl_infmemstartadd_s0_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_1_S0_reg, de_xcxl_infmemstartadd_1_s0_reg.regValue );
		IoReg_Write32( DE_XCXL_DATMemStartAdd2_Odd_S0_reg, de_xcxl_datmemstartadd2_odd_s0_reg.regValue );
		IoReg_Write32( DE_XCXL_DATMemStartAdd3_Odd_S0_reg, de_xcxl_datmemstartadd3_odd_s0_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_Odd_S0_reg, de_xcxl_infmemstartadd_odd_s0_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_1_Odd_S0_reg, de_xcxl_infmemstartadd_1_odd_s0_reg.regValue );
	} else {
		// even
		de_xcxl_datmemstartadd2_s1_reg.s1_dat_mstart_57_29 = (align_addr_aligned[0]>>3)&0x1fffffff;
		de_xcxl_datmemstartadd3_s1_reg.s1_dat_mstart_28_0 = (align_addr_aligned[1]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_s1_reg.s1_inf_mstart = (align_addr_aligned[2]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_1_s1_reg.s1_1_inf_mstart = (align_addr_aligned[3]>>3)&0x1fffffff;
		// Odd
		de_xcxl_datmemstartadd2_odd_s1_reg.s1_dat_mstart_odd_57_29 = (align_addr_aligned[4]>>3)&0x1fffffff;
		de_xcxl_datmemstartadd3_odd_s1_reg.s1_dat_mstart_odd_28_0 = (align_addr_aligned[5]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_odd_s1_reg.s1_inf_mstart_odd = (align_addr_aligned[6]>>3)&0x1fffffff;
		de_xcxl_infmemstartadd_odd_1_s1_reg.s1_1_inf_mstart_odd = (align_addr_aligned[7]>>3)&0x1fffffff;

		IoReg_Write32( DE_XCXL_DATMemStartAdd2_S1_reg, de_xcxl_datmemstartadd2_s1_reg.regValue );
		IoReg_Write32( DE_XCXL_DATMemStartAdd3_S1_reg, de_xcxl_datmemstartadd3_s1_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_S1_reg, de_xcxl_infmemstartadd_s1_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_1_S1_reg, de_xcxl_infmemstartadd_1_s1_reg.regValue );
		IoReg_Write32( DE_XCXL_DATMemStartAdd2_Odd_S1_reg, de_xcxl_datmemstartadd2_odd_s1_reg.regValue );
		IoReg_Write32( DE_XCXL_DATMemStartAdd3_Odd_S1_reg, de_xcxl_datmemstartadd3_odd_s1_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_Odd_S1_reg, de_xcxl_infmemstartadd_odd_s1_reg.regValue );
		IoReg_Write32( DE_XCXL_INFMemStartAdd_Odd_1_S1_reg, de_xcxl_infmemstartadd_odd_1_s1_reg.regValue );
	}
	// boundary address

	de_xcxl_wrrd_bndaddr1_reg.regValue = IoReg_Read32(DE_XCXL_WRRD_BndAddr1_reg);
	de_xcxl_wrrd_bndaddr2_reg.regValue = IoReg_Read32(DE_XCXL_WRRD_BndAddr2_reg);

	de_xcxl_wrrd_bndaddr1_reg.dexcxl_wrrd_downlimit_adr = (tmpAddr[0]>>3)&0x1fffffff;
	de_xcxl_wrrd_bndaddr2_reg.dexcxl_wrrd_uplimit_adr = (mem_end>>3)&0x1fffffff;

	IoReg_Write32( DE_XCXL_WRRD_BndAddr1_reg, de_xcxl_wrrd_bndaddr1_reg.regValue );
	IoReg_Write32( DE_XCXL_WRRD_BndAddr2_reg, de_xcxl_wrrd_bndaddr2_reg.regValue );

	// dma settings
	de_xcxl_rdatdma_set1_reg.regValue = IoReg_Read32(DE_XCXL_RDATDMA_Set1_reg);
	//de_xcxl_rdatdma_set1_reg.dat_water_r1   = ptr->dexc_dat_water_r1;
	de_xcxl_rdatdma_set1_reg.dat_len_r1     = ptr->dexc_dat_len_r1;
	de_xcxl_rdatdma_set1_reg.dat_num_r1     = ptr->dexc_dat_num_r1;
	IoReg_Write32( DE_XCXL_RDATDMA_Set1_reg, de_xcxl_rdatdma_set1_reg.regValue );

	de_xcxl_rdatdma_set2_reg.regValue = IoReg_Read32(DE_XCXL_RDATDMA_Set2_reg);
	//de_xcxl_rdatdma_set2_reg.dat_water_r2   = ptr->dexc_dat_water_r2;
	de_xcxl_rdatdma_set2_reg.dat_len_r2     = ptr->dexc_dat_len_r2;
	de_xcxl_rdatdma_set2_reg.dat_num_r2     = ptr->dexc_dat_num_r2;
	IoReg_Write32( DE_XCXL_RDATDMA_Set2_reg, de_xcxl_rdatdma_set2_reg.regValue );

	de_xcxl_wdatdma_set_reg.regValue = IoReg_Read32(DE_XCXL_WDATDMA_Set_reg);
	//de_xcxl_wdatdma_set_reg.dat_water_w     = ptr->dexc_dat_water_w;
	de_xcxl_wdatdma_set_reg.dat_len_w       = ptr->dexc_dat_len_w;
	de_xcxl_wdatdma_set_reg.dat_num_w       = ptr->dexc_dat_num_w;
	IoReg_Write32( DE_XCXL_WDATDMA_Set_reg, de_xcxl_wdatdma_set_reg.regValue );

	de_xcxl_datdma_rm_set_reg.regValue = IoReg_Read32(DE_XCXL_DATDMA_RM_Set_reg);
	de_xcxl_datdma_rm_set_reg.wdma_remain_en    = ptr->dexc_wdma_remain_en;
	de_xcxl_datdma_rm_set_reg.dat_remain_w      = ptr->dexc_dat_remain_w;
	de_xcxl_datdma_rm_set_reg.rdma1_remain_en   = ptr->dexc_rdma1_remain_en;
	de_xcxl_datdma_rm_set_reg.dat_remain_r1     = ptr->dexc_dat_remain_r1;
	de_xcxl_datdma_rm_set_reg.rdma2_remain_en   = ptr->dexc_rdma2_remain_en;
	de_xcxl_datdma_rm_set_reg.dat_remain_r2     = ptr->dexc_dat_remain_r2;
	IoReg_Write32( DE_XCXL_DATDMA_RM_Set_reg, de_xcxl_datdma_rm_set_reg.regValue );

	de_xcxl_rinfdma_set_reg.regValue = IoReg_Read32(DE_XCXL_RINFDMA_Set_reg);
	//de_xcxl_rinfdma_set_reg.inf_water_r = ptr->dexc_inf_water_r;
	de_xcxl_rinfdma_set_reg.inf_len_r   = ptr->dexc_inf_len_r;
	//de_xcxl_rinfdma_set_reg.inf_num_r   = ptr->dexc_inf_num_r;
	IoReg_Write32( DE_XCXL_RINFDMA_Set_reg, de_xcxl_rinfdma_set_reg.regValue );

	de_xcxl_winfdma_set_reg.regValue = IoReg_Read32(DE_XCXL_WINFDMA_Set_reg);
	//de_xcxl_winfdma_set_reg.inf_water_w = ptr->dexc_inf_water_w;
	de_xcxl_winfdma_set_reg.inf_len_w   = ptr->dexc_inf_len_w;
	//de_xcxl_winfdma_set_reg.inf_num_w   = ptr->dexc_inf_num_w;
	IoReg_Write32( DE_XCXL_WINFDMA_Set_reg, de_xcxl_winfdma_set_reg.regValue );

	//de_xcxl_infdma_rm_set_reg.regValue = IoReg_Read32(DE_XCXL_INFDMA_RM_Set_reg);
	//de_xcxl_infdma_rm_set_reg.inf_remain_w_en   = ptr->dexc_inf_remain_w_en;
	//de_xcxl_infdma_rm_set_reg.inf_remain_w      = ptr->dexc_inf_remain_w;
	//de_xcxl_infdma_rm_set_reg.inf_remain_r_en   = ptr->dexc_inf_remain_r_en;
	//de_xcxl_infdma_rm_set_reg.inf_remain_r      = ptr->dexc_inf_remain_r;
	//IoReg_Write32( DE_XCXL_INFDMA_RM_Set_reg, de_xcxl_infdma_rm_set_reg.regValue );

	de_xcxl_dma_multi_req_set_reg.regValue = IoReg_Read32(DE_XCXL_DMA_Multi_Req_Set_reg);
	de_xcxl_dma_multi_req_set_reg.rdma_req_num = 8;
	de_xcxl_dma_multi_req_set_reg.wdma_req_num = 8;
	de_xcxl_dma_multi_req_set_reg.dma_info_first = 1;
	IoReg_Write32( DE_XCXL_DMA_Multi_Req_Set_reg, de_xcxl_dma_multi_req_set_reg.regValue );

	de_xcxl_de_xcxl_db_reg_ctl_reg.regValue = IoReg_Read32(DE_XCXL_DE_XCXL_db_reg_ctl_reg);
	de_xcxl_de_xcxl_db_reg_ctl_reg.db_apply = 1;
	IoReg_Write32( DE_XCXL_DE_XCXL_db_reg_ctl_reg, de_xcxl_de_xcxl_db_reg_ctl_reg.regValue );

	//mem_end = tmpAddr<<3;

	rtd_pr_scaler_memory_info("DE-XCXL, tmpAddr[0:7]=%x,%x,%x,%x,%x,%x,%x,%x,\n",
		tmpAddr[0], tmpAddr[1],tmpAddr[2],tmpAddr[3],tmpAddr[4],tmpAddr[5],tmpAddr[6],tmpAddr[7]);
	rtd_pr_scaler_memory_info("DE-XCXL, align_addr_96[0:7]=%x,%x,%x,%x,%x,%x,%x,%x,\n",
		align_addr_aligned[0], align_addr_aligned[1],align_addr_aligned[2],align_addr_aligned[3],align_addr_aligned[4],align_addr_aligned[5],align_addr_aligned[6],align_addr_aligned[7]);
	rtd_pr_scaler_memory_info("DE-XCXL,field_size_96=%x,info_size_96=%x,field_size=%d,info_size=%x,mem end=%x,aligned_val=%d,\n",
		field_size_aligned, info_size_aligned, field_size, info_size, mem_end, aligned_val);

	return mem_end;
}

char  drv_memory_I_De_XC_ON_OFF_Wait(unsigned char En)
{
	de_xcxl_de_xcxl_db_reg_ctl_RBUS de_xcxl_de_xcxl_db_reg_ctl_reg;

	IoReg_ClearBits(DE_XCXL_De_XCXL_CTRL_reg, DE_XCXL_De_XCXL_CTRL_dexc_en_mask);	/* turn off de-xc */
	IoReg_SetBits(DE_XCXL_DE_XCXL_db_reg_ctl_reg, DE_XCXL_DE_XCXL_db_reg_ctl_db_apply_mask);

	de_xcxl_de_xcxl_db_reg_ctl_reg.regValue = IoReg_Read32(DE_XCXL_DE_XCXL_db_reg_ctl_reg);
	while (de_xcxl_de_xcxl_db_reg_ctl_reg.db_apply == 1) {
		de_xcxl_de_xcxl_db_reg_ctl_reg.regValue = IoReg_Read32(DE_XCXL_DE_XCXL_db_reg_ctl_reg);
	}

	return TRUE;
}

unsigned char I_De_XC_Mem_Ready_Flag[2] = {0};
unsigned char drv_memory_Set_I_De_XC_Mem_Ready_Flag(unsigned char MEM_IDX, unsigned char Flag)
{
	if (MEM_IDX>=2)
		return 255;
	I_De_XC_Mem_Ready_Flag[MEM_IDX] = Flag;
	return 0;
}

unsigned char drv_memory_Get_I_De_XC_Mem_Ready_Flag(unsigned char MEM_IDX)
{
	if (MEM_IDX>=2)
		return 0;
	return I_De_XC_Mem_Ready_Flag[MEM_IDX];
}



/*======================== End of File =======================================*/
/**
 *
 * @}
 */
