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

//#include <unistd.h>			// 'close()'
//#include <fcntl.h>			// 'open()'
//#include <stdio.h>
//#include <string.h>
//#include <sys/mman.h>
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
#endif
//#include <mach/io.h>
//#include "rtd_regs.h"
//#include "rtd_macro.h"
#include <rbus/mdomain_cap_reg.h>
#include <rbus/mdomain_disp_reg.h>
#include <rbus/mdom_sub_cap_reg.h>
#include <rbus/mdom_sub_disp_reg.h>
#include <rbus/di_reg.h>
#include <rbus/ipq_decontour_reg.h>
#include <rbus/ppoverlay_reg.h>
//#include <rbus/mdomain_vi_main_reg.h>
//#include <rbus/mdomain_vi_sub_reg.h>
#include <rbus/vodma_reg.h>

//#include <rbus/scaler/rbustv-sb2-dcu1reg.h>
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
	#include <rtk_kdriver/scalercommon/scalerCommon.h>
#endif
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/io/ioregdrv.h>
//#include <tvscalercontrol/scaler/scalerstruct.h>
//#include <tvscalercontrol/scaler/modeState.h>
//#include <tvscalercontrol/scaler/scalerTimer.h>
//#include "tvscalercontrol/scaler/scalerLib.h"
//#include <tvscalercontrol/scaler/scalerColorLib.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/vip/srnn.h>
#include <tvscalercontrol/scalerdrv/scalerclock.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scaler/scalercolorlib.h>
//#include "tvscalercontrol/scalerDrv/scalerDisplay.h"
#include <tvscalercontrol/scalerdrv/mode.h>
#include <tvscalercontrol/scalerdrv/pipmp.h>
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/scalerdrv/scalerip.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/scalerdrv/scaler_i2rnd.h>
//#include "kernel/scaler/scalerDrv.h"	// kernel driver
#include "tvscalercontrol/vdc/video.h"
#include "tvscalercontrol/hdmirx/hdmifun.h"
//#include "Platform_Lib/panel/panelAPI.h"
//#include "tvscalercontrol/scaler/scalerAPI.h"
//#include "tvscalercontrol/scaler/state.h" //for drvif_memory_sw_buffer_swap_action()
#include <tvscalercontrol/scalerdrv/scaler_2dcvt3d_ctrl.h>
#include <tvscalercontrol/vip/di_ma.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>

#include <rbus/demura_reg.h>
// #include <rbus/de_xcxl_reg.h> //k25lp removed





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

#endif


#include <rbus/scaleup_reg.h>
#include <rbus/scaledown_reg.h>
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle_lib.h>
#include <scaler_vscdev.h>
#include <scaler_vscdev.h>
#include <scaler/vipRPCCommon.h>

#if IS_ENABLED(CONFIG_RTK_KDRV_PWM)
#include <rtk_kdriver/rtk_pwm.h>
#endif

#include <rbus/timer_reg.h>

// for register dump
#include <tvscalercontrol/io/ioregdrv.h>
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


#ifdef BUILD_QUICK_SHOW
#define  dvr_memory_alignment(x, y)  ((unsigned long)x)
#define  PAGE_ALIGN(x)  (x)
#define  dvr_to_phys(x) ((unsigned long)x)

#undef  BUG
#define BUG() do { \
	rtd_pr_scaler_memory_info("U-Boot BUG at %s:%d!\n", __FILE__, __LINE__); \
} while (0)
static bool is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}
#endif

/*===================================  Types ================================*/
#ifdef USE_RTK_DEV_MEM
#define DEV_MEM     "/dev/rtkmem"
#else
#define DEV_MEM     "/dev/mem"
#endif

//#ifdef DUMP_SCALER_MEMORY
//#define CONFIG_DARWIN_SCALER
//#define CONFIG_MPIXEL_SHIFT
/*================================ Definitions ==============================*/
// DI bit number control (saving bandwidth)
#define DI_DMA_420_EN_START_BIT			30
#define DI_DMA_420_EN_MASK				(0x3 << DI_DMA_420_EN_START_BIT)
#define DI_DMA_LINESIZE_EVEN_START_BIT	12
#define DI_DMA_LINESIZE_EVEN_MASK			(0x7ff << DI_DMA_LINESIZE_EVEN_START_BIT)
#define DI_DMA_LINESIZE_ODD_START_BIT		0
#define DI_DMA_LINESIZE_ODD_MASK			(0x7ff << DI_DMA_LINESIZE_ODD_START_BIT)

// Set Vo gatting (threshold 0,1,2,3)
// th0 : 128, clk gating 25%
// th1 : 256, clk gating 50%
// th2 : 384, clk gating 75%
// th3 : 500, clk gating 97.6%
#define MAIN_FIFO_GATE_TH   0x400  // DIC suggestion
#define MAIN_FIFO_GATE_TH_1 0x800
#define MAIN_FIFO_GATE_TH_2 0xc00
#define MAIN_FIFO_GATE_TH_3 0xf70

//FBUS region
#define FBOUS_ADDRESS_LOW	0x1dc00000
#define FBOUS_ADDRESS_HIGH	0x20000000

#define Mag2_New_Flow // LearnRPG@20150430 new API for clear coding style

//PQC & PQDC
#define M_DOMAIN_PQC_LINE_MARGIN 6
/*================================== Variables ==============================*/
#define VPQ_COMPILER_ERROR_ENABLE_MERLIN6

static StructMemBlock MemTag[MEMIDX_UNKNOW];	// memory tag information
static unsigned int	AllocStartAddr = 0x00000000;
static unsigned int	AllocEndAddr = _DDR_END_ADDR;
static unsigned int	AllocStartAddrBlock[MEM_BLOCK_TOTAL_NUM];
static unsigned int	AllocEndAddrBlock[MEM_BLOCK_TOTAL_NUM];
//static unsigned int scaler_mem_virtualuncacheAddr=0;
unsigned char	*m_pCacheStartAddr = NULL;
unsigned long 	m_PhyStartAddr;
unsigned int	mMallocSize = 0x2000000;//0x1800000;//24MB//0x2100000;//33MB
//BYTE			*m_pSubMCacheStartAddr = NULL;
//BYTE			*m_pVDCCacheStartAddr = NULL;

static unsigned int id2dv_delay_target = 0;
extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr; //extern char run_iv2dv_slow_tuning_flag;//Run iv2dv slow tuning flag

extern unsigned char flag_3d;
// 3DDI memory usage control
static unsigned int bIpBtrMode=_DISABLE, bIpDma420Mode=_DISABLE;
static unsigned int MemDispStartAddr_3D [3];	//record memory disp start address for 3D
//frank@1023 add flag to indicate memory only DCU1 memory
//static bool m_only_dcu1=_FALSE;

static eMemCapAccessType enable_M_cap_mode_main = MEMCAPTYPE_FRAME;
static eMemCapAccessType enable_M_cap_mode_sub = MEMCAPTYPE_FRAME;

static UINT8 enable_M_compression_main = 0;
static UINT8 enable_M_compression_sub = 0;
static UINT8 M_compression_bits = COMPRESSION_NONE_BITS;
static UINT8 M_compression_mode = COMPRESSION_NONE_MODE;
static UINT8 Sub_M_compression_bits = COMPRESSION_NONE_BITS;
static UINT8 Sub_M_compression_mode = COMPRESSION_NONE_MODE;
static UINT32 M_fixed_virtual_memory_size = 0;
static UINT8 enable_M_GameMode = _DISABLE; // driver use game mode or not
static UINT8 enable_M_GameMode_Dynamic = _DISABLE;//AP set game mode or not
static UINT8 enable_M_LowLatencyMode_Vdec_direct = _DISABLE;
static unsigned int mdomain_main_buffer_size = 0;
static unsigned int mdomain_sub_buffer_size = 0;
static unsigned int mdomain_main_buffer_number = 0;
static unsigned int mdomain_sub_buffer_number = 0;
static unsigned char main_block_mode_enable = FALSE;
static unsigned char sub_block_mode_enable = FALSE;

extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr; // debug scaler flag tracker
extern unsigned int smooth_toggle_game_mode_timeout_trigger_flag;

static struct semaphore Main_Mdomain_ctrl_Semaphore;//for 4k flow  protect m domain capture enable memory trash

/*=================== Local Functions Phototype ==============*/

/*=================== Local Functions Phototype ==============*/
#ifndef Mag2_New_Flow
static void drv_memory_DMA420_config(unsigned char *bitNum);
#endif
/*======================M-domain Multi-Buffer Controlling=====================*/
static UINT8 Mdomain_multibuffer_enable = false;
static UINT8 Mdomain_capture_multibuffer_enable = false;
static UINT8 Mdomain_multibuffer_number = 0;
static unsigned int Mdomain_multibuffer_size = 0;
static UINT8 Mdomain_multibuffer_number_pre = 0;
static UINT8 Mdomain_multibuffer_changed = 0;
unsigned int get_borrowed_VBM_Size = 0;
#ifdef CONFIG_VBM_HEAP
unsigned long memory_multibuf_dmabuf_hndl = 0;
#endif

extern unsigned char get_sub_OutputVencMode(void);
extern unsigned char drvif_color_Get_Two_Step_UZU_Mode(void);
extern unsigned char get_MEMC_bypass_status_refer_platform_model(void);
extern PANEL_PIXEL_MODE get_panel_pixel_mode(void);
extern unsigned int Get_DISP_VERTICAL_TOTAL(void);
extern unsigned int Get_DISPLAY_CLOCK_TYPICAL(void);
extern unsigned int Get_DISPLAY_REFRESH_RATE(void);
extern unsigned int Get_DISP_HORIZONTAL_TOTAL(void);

void drv_memory_Set_multibuffer_flag(UINT8 enable);
UINT8 drv_memory_Get_multibuffer_flag(void);
void drv_memory_Set_multibuffer_number(UINT8 number);
UINT8 drv_memory_Get_multibuffer_number(void);
void drv_memory_Set_multibuffer_number_pre(UINT8 number);
UINT8 drv_memory_Get_multibuffer_number_pre(void);
void drv_memory_Set_capture_multibuffer_flag(UINT8 enable);
UINT8 drv_memory_Get_capture_multibuffer_flag(void);
void drv_memory_Set_multibuffer_size(unsigned int buffer_size);
unsigned int drv_memory_Get_multibuffer_size(void);
void drv_memory_send_multibuffer_number_to_smoothtoogh(void);
void drv_memory_capture_multibuffer_update_addr(unsigned int buffer_size, unsigned int start_addr);
void drv_memory_capture_multibuffer_update_boundary(unsigned char buf_num, unsigned int buffer_size);
void drv_memory_capture_multibuffer_control(unsigned char buf_num, unsigned char disp2cap_distance);
void drv_memory_capture_multibuffer_address(unsigned int buf4_addr, unsigned int buf5_addr,unsigned int buf6_addr,unsigned int buf7_addr,unsigned int buf8_addr);
void drv_memory_display_multibuffer_control(unsigned char buf_num, unsigned char cap2disp_distance);
void drv_memory_capture_multibuffer_switch_buffer_number(void);
void drv_memory_display_multibuffer_switch_buffer_number(void);
void drv_memory_display_multibuffer_address(unsigned int buf4_addr, unsigned int buf5_addr,unsigned int buf6_addr,unsigned int buf7_addr,unsigned int buf8_addr);

void main_mdomain_ctrl_semaphore_init(void)
{//init Main_Mdomain_ctrl_Semaphore
	sema_init(&Main_Mdomain_ctrl_Semaphore, 1);
}

struct semaphore *get_main_mdomain_ctrl_semaphore(void)
{//get Main_Mdomain_ctrl_Semaphore
	return &Main_Mdomain_ctrl_Semaphore;
}

#ifdef UT_flag
I3DDMA_3DDMA_CTRL_T* drvif_memory_GetI3ddmaCtrl(void)
{
	return &i3ddmaCtrl[I3DDMA_DEV];
}
#endif // #ifdef UT_flag

unsigned int drvif_memory_GetMemTag_size(unsigned int a_ulIndex)
{
	return MemTag[a_ulIndex].Size;
}

unsigned int drvif_memory_GetMemTag_addr(unsigned int a_ulIndex)
{
	return MemTag[a_ulIndex].StartAddr;
}


unsigned int Get_Val_drvif_memory_MemTag_VirAddr(unsigned int a_ulIndex)
{
	return MemTag[a_ulIndex].Allocate_VirAddr;
}

void Set_Val_drvif_memory_MemTag_VirAddr(unsigned int a_ulIndex, unsigned int a_ulAddr)
{
	MemTag[a_ulIndex].Allocate_VirAddr = a_ulAddr;
}

static unsigned char mdomain_driver_status = FALSE;//can reset m capture status

void set_mdomain_driver_status(unsigned char status)
{//set m domain driver ready or not
	mdomain_driver_status = status;
}

unsigned char get_mdomain_driver_status(void)
{//decide m domain driver finish or not
	return mdomain_driver_status;
}

bool vdec_data_frc_need_borrow_cma_buffer(unsigned char display)
{

	if(display != SLR_MAIN_DISPLAY)
		return false;

    if( ( get_panel_res() == PANEL_RES_FHD ) || ( get_panel_res() == PANEL_RES_HD ) || ( get_panel_res() == PANEL_RES_720p1440p ) || ( get_panel_res() == PANEL_RES_1K2K ) ) // 2k panel
		return false;

	//vdec 4k data frc need borrow memory
	if(check_sub_is_idle_for_srnn(display) && (Get_DisplayMode_Src(display) == VSC_INPUTSRC_VDEC))
	{
		return true;
	}else{
		return false;
	}
}

static void *m_domain_borrow_buffer_virtual_addr;
static unsigned int cma_borrow_size = 0;

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

/*================================== Function ===============================*/

/*============================================================================*/
#if 0
/**
 * memory_clear_mem_tag
 * Clear the specific memory tag
 *
 * @param <Idx> { index of DDR memory space, see eMemIdx for details }
 * @return { none }
 *
 */
//void __init memory_clear_mem_tag(eMemIdx Idx)
void memory_clear_mem_tag(eMemIdx Idx)
{
	unsigned char i;

	switch (Idx) {
		case MEMIDX_ALL:	//clear free block information
			for (i = 0; i < MEMIDX_ALL; i++) {	// clear all tag information
				MemTag[i].StartAddr = _DUMMYADDR;	// clear all tags
				MemTag[i].Size = 0x00;		// clear all tags
				MemTag[i].Status = 0;
			}
			break;
		default:
			drvif_memory_free_block(Idx);
			MemTag[Idx].StartAddr = _DUMMYADDR;	//clear the specific tag
			MemTag[Idx].Size = 0x0000;		//clear the specific tag
			MemTag[Idx].Status = 0;
			break;
	}
}
#endif

void mcap_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int cap_outstanding = 0;

	cap_outstanding = enable ? _WDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		mdomain_cap_frc_cap0_wr_ctrl_RBUS mdomain_cap_frc_cap0_wr_ctrl_reg;

		mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);

		if(mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_max_outstanding != cap_outstanding)
		{
			if(mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_enable)
			{
				drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main cap_en, before enable main cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);
			mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_max_outstanding = cap_outstanding;//max
			IoReg_Write32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, mdomain_cap_frc_cap0_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main cap_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		mdom_sub_cap_frc_cap1_wr_ctrl_RBUS mdom_sub_cap_frc_cap1_wr_ctrl_reg;

		mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);

		if(mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_max_outstanding != cap_outstanding)
		{
			if(mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_enable)
			{
				drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub cap_en, before enable sub cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);
			mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_max_outstanding = cap_outstanding;//max
			IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg, mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub cap_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_max_outstanding);
		}
	}
#endif
}

void mcap_dmac_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int cap_outstanding = 0;

	cap_outstanding = enable ? _WDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		mdomain_cap_frc_cap0c_wr_ctrl_RBUS mdomain_cap_frc_cap0c_wr_ctrl_reg;

		mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);

		if(mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_max_outstanding != cap_outstanding)
		{
			if(mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_enable)
			{
				drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main cap_en, before enable main cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
			mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_max_outstanding = cap_outstanding;//max
			IoReg_Write32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main cap_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		mdom_sub_cap_frc_cap1c_wr_ctrl_RBUS mdom_sub_cap_frc_cap1c_wr_ctrl_reg;

		mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);

		if(mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_max_outstanding != cap_outstanding)
		{
			if(mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_enable)
			{
				drvif_memory_DisableMCap(display);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub cap_en, before enable sub cap_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
			mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_max_outstanding = cap_outstanding;//max
			IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub cap_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_max_outstanding);
		}
	}
#endif
}

void mdisp_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int disp_outstanding = 0;

	disp_outstanding = enable ? _RDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		mdomain_disp_frc_disp0_rd_ctrl_RBUS mdomain_disp_frc_disp0_rd_ctrl_reg;

		mdomain_disp_frc_disp0_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg);

		if(mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_max_outstanding != disp_outstanding)
		{
			if(mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_enable)
			{
				drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main disp_en, before enable main disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_disp_frc_disp0_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg);
			mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_max_outstanding = disp_outstanding;
			IoReg_Write32(MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, mdomain_disp_frc_disp0_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main disp_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		mdom_sub_disp_frc_disp1_rd_ctrl_RBUS mdom_sub_disp_frc_disp1_rd_ctrl_reg;

		mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg);

		if(mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_max_outstanding != disp_outstanding)
		{
			if(mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_enable)
			{
				drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub disp_en, before enable sub disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg);
			mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_max_outstanding = disp_outstanding;
			IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub disp_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_max_outstanding);
		}
	}
#endif
}

void mdisp_dmac_outstanding_ctrl(unsigned char display, unsigned char enable)
{
	unsigned int disp_outstanding = 0;

	disp_outstanding = enable ? _RDMA_MAX_OUTSTANDING : 0;

	if(display == SLR_MAIN_DISPLAY)
	{
		mdomain_disp_frc_disp0c_rd_ctrl_RBUS mdomain_disp_frc_disp0c_rd_ctrl_reg;

		mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);

		if(mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_max_outstanding != disp_outstanding)
		{
			if(mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_enable)
			{
				drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable main disp_en, before enable main disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);
			mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_max_outstanding = disp_outstanding;
			IoReg_Write32(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set main disp_outstanding %d\n",__FUNCTION__, __LINE__, mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_max_outstanding);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else
	{//sub
		mdom_sub_disp_frc_disp1c_rd_ctrl_RBUS mdom_sub_disp_frc_disp1c_rd_ctrl_reg;

		mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg);

		if(mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_max_outstanding != disp_outstanding)
		{
			if(mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_enable)
			{
				drvif_memory_EnableMDisp_Or_DisableMDisp(display, _DISABLE);
				mdelay(1);
				rtd_pr_scaler_memory_err("[%s] %d disable sub disp_en, before enable sub disp_outstanding\n",__FUNCTION__, __LINE__);
			}

			mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg);
			mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_max_outstanding = disp_outstanding;
			IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg, mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue);
			rtd_pr_scaler_memory_info("[%s] %d set sub disp_outstanding %d\n",__FUNCTION__, __LINE__, mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_max_outstanding);
		}
	}
#endif
}

/*=======================================================================*/
/**
 * drvif_memory_init
 * Init Memory control module. Collect the register, which should be insert only once.
 *
 * @param  { none }
 * @return { none }
 *
 * @Note
 * Mclk is separated into 32 phases
 */
//void __init drvif_memory_init(void)
void drvif_memory_init(void)
{
#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	unsigned char i;

	for ( i=MEMIDX_VDC ; i<MEMIDX_ALL ; ++i ) {	// clear all tag information
		MemTag[i].Allocate_StartAddr = 0;
		MemTag[i].StartAddr = _DUMMYADDR;
		MemTag[i].Size = 0;
		MemTag[i].Status = UNDEFINED;
	}
#else

	unsigned int address=0;
	unsigned int size=0;
	//unsigned int  TotalSize;
	//unsigned char				*m_pNonCacheStartAddr;

	SCALERDRV_ALLOCBUFFER_INFO stAllocBufferInfo;
	if(!drvif_scaler_getValidMemInfo(&stAllocBufferInfo)){
		rtd_pr_scaler_memory_debug("fail to get memory information\n");
		return;
	}

	AllocStartAddrBlock[MEM_BLOCK_USE_NORMAL] = stAllocBufferInfo.normal_mem_addr;
	AllocEndAddrBlock[MEM_BLOCK_USE_NORMAL] = AllocStartAddrBlock[MEM_BLOCK_USE_NORMAL] + stAllocBufferInfo.normal_mem_size;

	AllocStartAddrBlock[MEM_BLOCK_USE_OPTIMIZE] = stAllocBufferInfo.optimize_mem_addr;
	AllocEndAddrBlock[MEM_BLOCK_USE_OPTIMIZE] = AllocStartAddrBlock[MEM_BLOCK_USE_OPTIMIZE] + stAllocBufferInfo.optimize_mem_size;

	address = stAllocBufferInfo.normal_mem_addr;
	size = stAllocBufferInfo.normal_mem_size;

	AllocStartAddr = address;
	AllocEndAddr = AllocStartAddr  + size;

	drvif_memory_free_block(MEMIDX_ALL);

	//Disable channel1, channel2 field merge function
	drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	drvif_memory_DisableMCap(SLR_SUB_DISPLAY);
#endif

	//frank@0121 Disable function display main,  down mask window, display sub, MP function
	//if(Scaler_Get_Keep_In_BootUp_Mode_flag()==FALSE)
	drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);
	IoReg_ClearBits(MDOMAIN_DISP_DDR_MainMaskCtrl_reg,_BIT0);

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	IoReg_ClearBits(MDOMAIN_DISP_DDR_SubCtrl_reg, _BIT0);
	IoReg_ClearBits(MDOMAIN_DISP_DDR_SubMPMode_reg,_BIT0);
#endif

	drvif_memory_set_dbuffer_write(SLR_MAIN_DISPLAY);
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	drvif_memory_set_dbuffer_write(SLR_SUB_DISPLAY);
#endif

	// Reset boundary pretection to global boundary
	IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr_CTRL2_reg, 0x00000000);

#ifdef VBY_ONE_PANEL
	if((MemTag[MEMIDX_MAIN_THIRD].StartAddr!=NULL) && (MemTag[MEMIDX_MAIN_THIRD].StartAddr < AllocStartAddr))
		IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr1_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr & 0xfffffff0));
	else
		IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr1_reg, (AllocStartAddr & 0xfffffff0));
#else
	IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr1_reg,(AllocStartAddr& 0xfffffff0));
#endif
	IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr2_reg,(AllocEndAddr-CONFIG_MDOMAIN_BURST_SIZE) & 0xfffffff0);

	//set cap sub boundary. 1  unit = 8 bytes
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
//for sub cap/disp use in DCU2
	// Reset boundary pretection to global boundary
	IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr_CTRL2_reg, 0x00000000);

	IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr1_reg,(AllocStartAddr & 0xfffffff0));
	IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr2_reg,(AllocEndAddr-CONFIG_MDOMAIN_BURST_SIZE)& 0xfffffff0);
#endif
#endif
	// Set Vo gatting threshold
	drv_memory_set_vo_gating_threshold();

	rtd_pr_scaler_memory_debug("Initial Memory is completed. \n");
}

void drvif_memory_release(void)
{
	unsigned char i;
	for ( i=MEMIDX_VDC ; i<MEMIDX_ALL ; ++i ) {	// clear all tag information
		if(drvif_memory_get_block_addr((eMemIdx)i)!=_DUMMYADDR)
			rtd_pr_scaler_memory_debug("[ERROR]Idx:%d,Memory Not be release\n", i);
	}
	drvif_scaler_releaseValidMem();
	AllocStartAddr = 0;
	AllocEndAddr = _DDR_END_ADDR;
}

void drvif_memory_reallocate(void)
{
#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	return;
#else
	unsigned int address=0;
	unsigned int size=0;
	SCALERDRV_ALLOCBUFFER_INFO stAllocBufferInfo;

#if 0
	if(!drvif_scaler_getValidMemInfo(&address, &size)){
		rtd_pr_scaler_memory_debug("fail to get memory information\n");
		return;
	}
#else
	if(!drvif_scaler_getValidMemInfo(&stAllocBufferInfo)){
		rtd_pr_scaler_memory_debug("fail to get memory information\n");
		return;
	}

	AllocStartAddrBlock[MEM_BLOCK_USE_NORMAL] = stAllocBufferInfo.normal_mem_addr;
	AllocEndAddrBlock[MEM_BLOCK_USE_NORMAL] = AllocStartAddrBlock[MEM_BLOCK_USE_NORMAL] + stAllocBufferInfo.normal_mem_size;

	AllocStartAddrBlock[MEM_BLOCK_USE_OPTIMIZE] = stAllocBufferInfo.optimize_mem_addr;
	AllocEndAddrBlock[MEM_BLOCK_USE_OPTIMIZE] = AllocStartAddrBlock[MEM_BLOCK_USE_OPTIMIZE] + stAllocBufferInfo.optimize_mem_size;

	address = stAllocBufferInfo.normal_mem_addr;
	size = stAllocBufferInfo.normal_mem_size;

#endif
	AllocStartAddr = address;
	AllocEndAddr = AllocStartAddr  + size;
#ifndef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	//set capture upper boundary is end address -512 size,avoid capture fail address
	IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr1_reg, (AllocStartAddr & 0xfffffff0));
	IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr2_reg,(AllocEndAddr-CONFIG_MDOMAIN_BURST_SIZE) & 0xfffffff0);

	//set cap sub boundary. 1  unit = 8 bytes
#ifdef CONFIG_DUAL_CHANNEL
	IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr1_reg,(AllocStartAddr & 0xfffffff0));
	IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr2_reg,(AllocEndAddr-CONFIG_MDOMAIN_BURST_SIZE) & 0xfffffff0);
#endif
#endif
#endif//#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY

}



 int drvif_scaler_getValidMemInfo_fromVideo(SCALERDRV_ALLOCBUFFER_INFO *pAllocBufferInfo)
{
	int ret=0;
	unsigned int i = 0, ulAllocBufInfoItemCount = 0;
	unsigned int *pulTemp;
	SCALERDRV_ALLOCBUFFER_INFO stAllocBufferInfo;

	memset(pAllocBufferInfo, 0, sizeof(SCALERDRV_ALLOCBUFFER_INFO));
	memset(&stAllocBufferInfo, 0, sizeof(SCALERDRV_ALLOCBUFFER_INFO));
	// send RPC to request allocate buffer address
	if ((ret = Scaler_SendRPC(SCALERIOC_ALLOBUFFER,0,0)) < 0)
	{
		rtd_pr_scaler_memory_debug("[IDMA] ret=%d, get alloc buffer address failed!!!\n", ret );
		pAllocBufferInfo->normal_mem_addr = 0;
		pAllocBufferInfo->normal_mem_size = 0;
		pAllocBufferInfo->optimize_mem_addr = 0;
		pAllocBufferInfo->optimize_mem_size = 0;
	}else{
		// copy from RPC share memory
		memcpy(&stAllocBufferInfo, (unsigned char*)Scaler_GetShareMemVirAddr(SCALERIOC_ALLOBUFFER), sizeof(SCALERDRV_ALLOCBUFFER_INFO));

		// change endian
		ulAllocBufInfoItemCount = sizeof(SCALERDRV_ALLOCBUFFER_INFO) / sizeof(unsigned int);
		pulTemp = (unsigned int *)&stAllocBufferInfo;
		for (i = 0; i < ulAllocBufInfoItemCount; i++)
		        pulTemp[i] = Scaler_ChangeUINT32Endian(pulTemp[i]);

		pAllocBufferInfo->normal_mem_addr = stAllocBufferInfo.normal_mem_addr;
		pAllocBufferInfo->normal_mem_size = stAllocBufferInfo.normal_mem_size;
		pAllocBufferInfo->optimize_mem_addr = stAllocBufferInfo.optimize_mem_addr;
		pAllocBufferInfo->optimize_mem_size = stAllocBufferInfo.optimize_mem_size; // reserve 8 bytes for FRC memory issue
	}

	rtd_pr_scaler_memory_debug("[IDMA][RPC][Addr:%x][Size:%d KB]\n", pAllocBufferInfo->normal_mem_addr, pAllocBufferInfo->normal_mem_size >> 10);
	rtd_pr_scaler_memory_debug("[IDMA][RPC][Addr_opt:%x][Size_opt:%d KB]\n", pAllocBufferInfo->optimize_mem_addr, pAllocBufferInfo->optimize_mem_size >> 10);

	return ret;
}



//#define nonCacheMem(a)	(a+DEF_MEM_SIZE)
unsigned char drvif_scaler_getValidMemInfo(SCALERDRV_ALLOCBUFFER_INFO *pAllocBufferInfo)
{
	//int ret;
	//unsigned int ulAllocBufInfoItemCount = 0;
	//unsigned int *pulTemp;
	//SCALERDRV_ALLOCBUFFER_INFO stAllocBufferInfo;
#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	return FALSE;
#else
	unsigned char		*m_pNonCacheStartAddr;
	unsigned long 		m_PhyStartAddr;

	if (pAllocBufferInfo == NULL)
		return FALSE;

	pAllocBufferInfo->normal_mem_addr = 0;
	pAllocBufferInfo->normal_mem_size = 0;
	pAllocBufferInfo->optimize_mem_addr = 0;
	pAllocBufferInfo->optimize_mem_size = 0;

	if (pAllocBufferInfo->normal_mem_size == 0 || pAllocBufferInfo->optimize_mem_addr == 0) // if video fw doesn't give the valid memory address
	{
#if 0
		// alloc memory
		m_PhyStartAddr = pli_allocOverlappedMemory(mMallocSize);
		if(m_PhyStartAddr == 0){
			m_pCacheStartAddr = (BYTE *)pli_allocContinuousMemorySpecific(mMallocSize, (char**)&m_pNonCacheStartAddr, &m_PhyStartAddr, GFP_DCU1);
			if(m_pCacheStartAddr == NULL){
				rtd_pr_scaler_memory_debug("[ERROR]Allocate DCU1 size=%x fail\n",mMallocSize);
				return FALSE;
			}
		}
#else
		m_pCacheStartAddr = dvr_malloc(mMallocSize);
		//m_PhyStartAddr = (unsigned int)virt_to_phys((void*)m_pCacheStartAddr);
		m_PhyStartAddr = (unsigned int)dvr_to_phys((void*)m_pCacheStartAddr);
		if(m_PhyStartAddr == 0){
				rtd_pr_scaler_memory_debug("[ERROR]Allocate DCU1 size=%x fail\n",mMallocSize);
				return FALSE;
		}

#endif
		pAllocBufferInfo->normal_mem_addr = (unsigned int)m_PhyStartAddr;
		pAllocBufferInfo->normal_mem_size = mMallocSize;
		pAllocBufferInfo->optimize_mem_addr = (unsigned int)m_PhyStartAddr;
		pAllocBufferInfo->optimize_mem_size = mMallocSize;//0x1000000; // 16MB

		rtd_pr_scaler_memory_debug("[SLR_MEMORY3][PLI][Cache:%x][Phy:%x][Size:%x]\n", (unsigned int)m_pCacheStartAddr, (unsigned int)m_PhyStartAddr, mMallocSize);
		rtd_pr_scaler_memory_debug("[SLR_MEMORY3][PLI][Addr:%x][Size:%x]\n", pAllocBufferInfo->normal_mem_addr, pAllocBufferInfo->normal_mem_size);
		rtd_pr_scaler_memory_debug("[SLR_MEMORY3][PLI][Addr_opt:%x][Size_opt:%x]\n", pAllocBufferInfo->optimize_mem_addr, pAllocBufferInfo->optimize_mem_size);
		rtd_pr_scaler_memory_debug("[SLR_MEMORY3][PLI][nonCache:%x]\n", (unsigned int)m_pNonCacheStartAddr);
	}
	//scaler_mem_virtualuncacheAddr = nonCacheMem(pAllocBufferInfo->normal_mem_addr);

	return TRUE;
#endif
}

unsigned char drvif_scaler_releaseValidMem(void)
{
#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	drvif_memory_free_block(MEMIDX_ALL);
#else
#if 0

	if(m_pCacheStartAddr == NULL){
		if(m_PhyStartAddr != 0){
			pli_freeOverlappedMemory(m_PhyStartAddr);
		}else{
			return FALSE;
		}
	}else{
		pli_freeContinuousMemory(m_pCacheStartAddr);
		m_pCacheStartAddr = NULL;
	}
#else
	if(m_pCacheStartAddr == NULL){
		return FALSE;
	}
	else
	{
		dvr_free((void *)m_pCacheStartAddr);
		m_pCacheStartAddr = NULL;
	}
#endif
#endif

	return TRUE;
}

#if 0
static unsigned int drv_memory_get_mem_remap_addr(unsigned int phyAddr, unsigned int length)
{
	int mem_fd;
	volatile void* pMem;
	unsigned int result = 0;
	if((mem_fd=open(DEV_MEM, O_RDWR)) < 0){
		rtd_pr_scaler_memory_debug("Err: can't open DEV_MEM.\n");
		return 0;
	}

	pMem = mmap(0, length, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, phyAddr);

	if(pMem == MAP_FAILED){
		rtd_pr_scaler_memory_debug("Err: can't get the mmap adress pointer\n");
		close(mem_fd);
		return 0;
	}

	result = (unsigned int)pMem;
	close(mem_fd);
	return result;
}
#endif
#if 0
unsigned int get_scaler_memory_map_addr()
{
#if 1
	return drv_memory_get_mem_remap_addr(AllocStartAddr,  AllocEndAddr - AllocStartAddr);
#else
	return scaler_mem_virtualAddr;
#endif
}
#endif

unsigned char drvif_memory_switch_mem_block(eMemBlockUsage mem_block_usage)
{
	if (mem_block_usage >= MEM_BLOCK_TOTAL_NUM)
	{
		rtd_pr_scaler_memory_debug("[SLR_MEMORY][unknown mem block][%x~%x]\n", AllocStartAddr, AllocEndAddr);
		return FALSE;
	}

	AllocStartAddr = AllocStartAddrBlock[mem_block_usage];
	AllocEndAddr = AllocEndAddrBlock[mem_block_usage];
#ifndef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	//set cap main boundary. 1  unit = 8 bytes
	IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr1_reg, (AllocStartAddr & 0xfffffff0));
	IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr2_reg,(AllocEndAddr-CONFIG_MDOMAIN_BURST_SIZE) & 0xfffffff0);

	//set cap sub boundary. 1  unit = 8 bytes
#ifdef CONFIG_DUAL_CHANNEL
	IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr1_reg,(AllocStartAddr & 0xfffffff0));
	IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr2_reg,(AllocEndAddr-CONFIG_MDOMAIN_BURST_SIZE) & 0xfffffff0);
#endif
#endif

	rtd_pr_scaler_memory_debug("[SLR_MEMORY][switch to mem block %d][%x~%x](%d MB)\n", mem_block_usage, AllocStartAddr, AllocEndAddr, (AllocEndAddr-AllocStartAddr)>>20);
	return TRUE;
}
#if 0
unsigned int get_scaler_memory_map_uncacheaddr()
{
	return scaler_mem_virtualuncacheAddr;
}
#endif


unsigned int get_scaler_memory_base_addr(void)
{
	return AllocStartAddr;

}

unsigned int memory_get_pixel_size(PixelSizeInfo *pPixelSizeInfo)
{
    unsigned int PixelSize;

    if (pPixelSizeInfo == NULL){
        rtd_pr_scaler_memory_err("pPixelSizeInfo is NULL! -- %s\n", __FUNCTION__);
        return 0;
    }

    if (pPixelSizeInfo->YCMode == MEM_YC_COMBINE){

        if(pPixelSizeInfo->Format == MEM_444){
            if(pPixelSizeInfo->BitNum == MEM_10_BIT) {		// 4:4:4 format     10bit
                PixelSize = 30;
            } else {			// 4:4:4 format     8bit
                PixelSize = 24;
            }
        }else{
            if(pPixelSizeInfo->BitNum == MEM_10_BIT) {		// 4:2:2 format     10bit
                PixelSize = 20;
            } else {			// 4:2:2 format     8bit
                PixelSize = 16;
            }
        }
    } else {
        if(pPixelSizeInfo->BitNum == MEM_10_BIT) {		// yc separate     10bit
            PixelSize = 10;
        } else {			// yc separate     8bit
            PixelSize = 8;
        }
    }

    return PixelSize;
}

unsigned int memory_get_line_size(unsigned int Width, PixelSizeInfo *pPixelSizeInfo)
{
    unsigned int LineSize;

    LineSize = drvif_memory_get_data_align(Width * memory_get_pixel_size(pPixelSizeInfo), 128);
    LineSize = (unsigned int)SHR(LineSize, 7);

    rtd_pr_scaler_memory_info("LineSize=%d -- %s\n", LineSize, __FUNCTION__);
    return LineSize;
}


/*============================================================================*/
/**
 * memory_get_capture_size
 * Calculate the memory size of capture. The size is counted by 4:2:2/4:4:4 data format,
 * and frame-access/line access.
 *
 * @param <info> { disp-info structure }
 * @param <AccessType> { Access_Type  }
 * @param <TotalSize> { size of capture }
 * @return { none }
 *
 */
unsigned int memory_get_capture_size(unsigned char display)
{
    unsigned int TotalSize;
    unsigned int Width;
    unsigned int Length;
    PixelSizeInfo pixelSizeInfo;

    Width = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);
    Length = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN);

    if (dvrif_memory_get_block_mode_enable(display)){
        pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
        pixelSizeInfo.BitNum = MEM_8_BIT;
        pixelSizeInfo.Format = MEM_420;
    } else {
        pixelSizeInfo.YCMode = MEM_YC_COMBINE;
        pixelSizeInfo.BitNum = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_422CAP);
    }


    TotalSize = memory_get_line_size(Width, &pixelSizeInfo);


    if (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME)
        TotalSize = TotalSize * Length;	// multiple total_lines if frame-access

    rtd_pr_scaler_memory_emerg("[Mdomain]: TotalSize=%x,capWid=%d,capLen=%d,10Bit=%d,422=%d\n", TotalSize, Width, Length, pixelSizeInfo.BitNum, pixelSizeInfo.Format);

    return TotalSize;
}

unsigned int memory_get_display_size(unsigned char display)
{
    unsigned int TotalSize;
    unsigned int Width;
    unsigned int Length;
    PixelSizeInfo pixelSizeInfo;

    Width = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID);
    Length = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN);

    if (dvrif_memory_get_block_mode_enable(display)){
        pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
        pixelSizeInfo.BitNum = MEM_8_BIT;
        pixelSizeInfo.Format = MEM_420;
    } else {
        pixelSizeInfo.YCMode = MEM_YC_COMBINE;
        pixelSizeInfo.BitNum = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_422CAP);
    }


    TotalSize = memory_get_line_size(Width, &pixelSizeInfo);


    if (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME)
        TotalSize = TotalSize * Length;	// multiple total_lines if frame-access

    rtd_pr_scaler_memory_emerg("[Mdomain]: TotalSize=%x,capWid=%d,capLen=%d,10Bit=%d,422=%d\n", TotalSize, Width, Length, pixelSizeInfo.BitNum, pixelSizeInfo.Format);

    return TotalSize;
}

unsigned int memory_get_capture_line_size_compression(unsigned char display, unsigned char compression_bits)
{
	unsigned short cmp_line_sum_bit;
    unsigned int capWid = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);


	rtd_pr_scaler_memory_info("[Modmain] compression_bits=%x, capWid=%x\n", compression_bits, capWid);

	//width need to align 32 for new PQC @Crixus 20170615
	if((capWid % 32) != 0)
		capWid = capWid + (32 - (capWid % 32));

    cmp_line_sum_bit = calc_comp_line_sum_bit(capWid, compression_bits);

	rtd_pr_scaler_memory_info("[Modmain] memory_get_capture_line_size_compression LineSize=%d\n", cmp_line_sum_bit);

	return cmp_line_sum_bit;
}

unsigned int memory_get_capture_frame_size_compression(unsigned char display, unsigned char compression_bits)
{
	unsigned int TotalSize;
	unsigned int capLen = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN);
	unsigned int capWid = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);

	//rtd_pr_scaler_memory_emerg("[crixus]capWid div32 = %d\n", capWid);
	TotalSize = capWid * compression_bits;
	//rtd_pr_scaler_memory_emerg("[crixus]TotalSize compress = %d\n",TotalSize);
	TotalSize = drvif_memory_get_data_align(TotalSize, 64);// (pixels of per line * bits / 64), unit: 64bits
	TotalSize = TotalSize * capLen;

	//rtd_pr_scaler_memory_emerg("[crixus]memory_get_line_size LineSize=%d\n",TotalSize);
	//rtd_pr_scaler_memory_emerg("[crixus]:TotalSize=%x,capWid=%x,capLen=%x\n", TotalSize,capWid,capLen);
	return TotalSize;
}

#ifndef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
static void memory_check_enough(void)
{
	unsigned int i = 0, sum_size = 0;
	for(i = MEMIDX_VDC ; i<MEMIDX_ALL ; i++) {
		if(MemTag[i].StartAddr != _DUMMYADDR){
			if(i != MEMIDX_DI)	// DI use another memory area, skip check
				sum_size += MemTag[i].Size;
		}
	}

	if(sum_size > (AllocEndAddr-AllocStartAddr)){
		rtd_pr_scaler_memory_debug("\n Scaler Memory Allocate ERROR!!(Overflow Memory Size),Start =%x,End=%x\n",AllocStartAddr,AllocEndAddr);
		for(i = MEMIDX_VDC ; i<MEMIDX_ALL ; i++) {
			rtd_pr_scaler_memory_debug("Allocate Index %d, StartAddress = %x, Size=%d\n", i, MemTag[i].StartAddr, MemTag[i].Size);
		}
	}
}


/*============================================================================*/
/**
 * memory_set_mem_tag
 * Set start address and size of a specific memory tag
 *
 * @param <index> { index of memory tag }
 * @param <space> { size }
 * @return { none }
 *
 */
static void memory_set_mem_tag(eMemIdx Idx, unsigned int StartAddr, unsigned int Space)
{
	//unsigned char	*m_pNonCacheStartAddr;
	if(MemTag[Idx].Status == ALLOCATED_FROM_HIGH)
		MemTag[Idx].StartAddr = (StartAddr & ~((1 << _DDR_BITS) - 1));
	else
		MemTag[Idx].StartAddr = drvif_memory_get_data_align(StartAddr, (1 << _DDR_BITS));

	if(MEMIDX_DI == Idx){
		IoReg_Write32(DI_DI1_WRRD_BOUNDARYADDR1_VADDR, (MemTag[Idx].StartAddr & 0xfffffff0));
#if 1
		//patch for DI/RTNR bug(overwrite 512Byte)
		IoReg_Write32(DI_DI1_WRRD_BOUNDARYADDR2_VADDR, (MemTag[Idx].StartAddr+Space-CONFIG_MDOMAIN_BURST_SIZE) & 0xfffffff0);
#else
		IoReg_Write32(DI_DI1_WRRD_BOUNDARYADDR2_VADDR, (MemTag[Idx].StartAddr+Space) & 0x7ffffff0);
#endif
	}
	MemTag[Idx].Size = Space;
	//Add below code to check memory allocate not enough check
	memory_check_enough();
}
#endif

/*============================================================================*/
/**
 * drvif_memory_free_block
 * Free memory of the specific tag and insert it to free block information
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { none }
 *
 */
void drvif_memory_free_block(eMemIdx Idx)
{

	unsigned char i;
#ifdef PLI_FAIL
	return;
#endif

#if 0 // do not free scaler mem for vo smooth toggle
	unsigned char display;
	if (Idx >= MEMIDX_SUB && Idx <= MEMIDX_SUB_THIRD) {
		display = SLR_SUB_DISPLAY;
	} else {
		display = SLR_MAIN_DISPLAY;
	}
#if 0
	if((Scaler_InputSrcGetType(display) == _SRC_VO)&&(Get_vo_smoothtoggle_changeratio_flag(display)||Get_vo_smoothtoggle_timingchange_flag(display)))
	{
		rtd_pr_scaler_memory_debug("[smooth toggle]do not free %d memory\n",Idx);
		return;
	}
#endif
#endif

#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	if ( (Idx>=MEMIDX_VDC) && (Idx<MEMIDX_ALL) ) {
		if(Idx == MEMIDX_DI){
			{
#if 0	/* DI use rolling mode, can't disable DI*/
				// Close IP enable
				di_im_di_control_RBUS im_di_control_reg;
				im_di_control_reg.regValue = IoReg_Read32(DI_IM_DI_CONTROL_reg);
				im_di_control_reg.ip_enable= 0;
				im_di_control_reg.write_enable_8 = 1;
				IoReg_Write32(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);
#endif
			}
			//IoReg_Mask32(DI_IM_DI_CONTROL_reg, ~_BIT28, 0);

#ifndef ENABLE_SMOOTH_TOGGLE_PR3D
			//IoReg_ClearBits(BUS_TV_SB2_DCU1_ARB_CR1_VADDR, _BIT0);
			// avoid DC_SYS2 client separate enable fail issue due to DI send zero write command to bus
			drv_memory_disable_di_write_req(TRUE);
#endif
		}else if(Idx == MEMIDX_MAIN){
			//disable main Mdomain cature
			drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
#if 0 //frank@0903 reduce scaler time
			//frank@0503 add below delay to confirm M capture have disable
			msleep(20);
#endif
		}else if(Idx == MEMIDX_SUB){
			//disable sub Mdomain cature
			//Crixus@20140722 - Mac3 no in2
			//drvif_memory_DisableMCap(SLR_SUB_DISPLAY);
#if 0 //frank@0903 reduce scaler time
			//frank@0503 add below delay to confirm M capture have disable
			msleep(20);
#endif
		}

		if(MemTag[Idx].StartAddr != _DUMMYADDR){
			#if 0
			if((MemTag[Idx].StartAddr >= FBOUS_ADDRESS_LOW)&&(MemTag[Idx].StartAddr <= FBOUS_ADDRESS_HIGH)){//FBUS region
				if(MemTag[Idx].Allocate_StartAddr != 0)
					pli_freeOverlappedMemory(MemTag[Idx].Allocate_StartAddr);
			}else{
				if(MemTag[Idx].Allocate_StartAddr != 0)
					pli_freeContinuousMemory((BYTE *)MemTag[Idx].Allocate_StartAddr);
			}
			#else

			if(Idx == MEMIDX_DI){
//DI,RTNR use reserved memory, no need to free memory @Crixus 20170505
#ifndef CONFIG_MDOMAIN_RESERVED_MEMORY
					if(MemTag[Idx].Allocate_StartAddr != 0)
						free_overlapped_memory((unsigned long)MemTag[Idx].Allocate_StartAddr);
#endif
			}else{
				if((MemTag[Idx].Allocate_StartAddr != 0) && (MemTag[Idx].Allocate_VirAddr != 0))
					dvr_free((void *)MemTag[Idx].Allocate_VirAddr);
			}

			#endif
			rtd_pr_scaler_memory_debug("[SCALER]FREE address %x\n",MemTag[Idx].Allocate_StartAddr);
			MemTag[Idx].Allocate_StartAddr = 0;
			MemTag[Idx].StartAddr = _DUMMYADDR;
			MemTag[Idx].Size = 0;
			MemTag[Idx].Status = UNDEFINED;
			MemTag[Idx].Allocate_VirAddr = 0;
		}
	} else if ( Idx == MEMIDX_ALL ) {
		for ( i=MEMIDX_VDC ; i<MEMIDX_ALL ; ++i ) {	// clear all tag information
			if(i == MEMIDX_DI){
#if 0	/* DI use rolling mode, can't disable DI*/
				// Close IP enable
				di_im_di_control_RBUS im_di_control_reg;
				im_di_control_reg.regValue = IoReg_Read32(DI_IM_DI_CONTROL_reg);
				im_di_control_reg.ip_enable= 0;
				im_di_control_reg.write_enable_8 = 1;
				IoReg_Write32(DI_IM_DI_CONTROL_reg, im_di_control_reg.regValue);
#endif
				//IoReg_Mask32(DI_IM_DI_CONTROL_reg, ~_BIT28, 0);
#ifndef ENABLE_SMOOTH_TOGGLE_PR3D
				//IoReg_ClearBits(BUS_TV_SB2_DCU1_ARB_CR1_VADDR, _BIT0);
				// avoid DC_SYS2 client separate enable fail issue due to DI send zero write command to bus
				drv_memory_disable_di_write_req(TRUE);
#endif
			}else if(i == MEMIDX_MAIN){
				//disable main Mdomain cature
				drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
			}else if(i == MEMIDX_SUB){
				//disable sub Mdomain cature
				//Crixus@20140722 - Mac3 no in2
				//drvif_memory_DisableMCap(SLR_SUB_DISPLAY);
			}

			if(MemTag[i].StartAddr != _DUMMYADDR){
				#if 0
				if((MemTag[Idx].StartAddr >= FBOUS_ADDRESS_LOW)&&(MemTag[Idx].StartAddr <= FBOUS_ADDRESS_HIGH)){//FBUS region
					if(MemTag[Idx].Allocate_StartAddr != 0)
						pli_freeOverlappedMemory(MemTag[Idx].Allocate_StartAddr);
				}else{
					if(MemTag[Idx].Allocate_StartAddr != 0)
						pli_freeContinuousMemory((BYTE *)MemTag[Idx].Allocate_StartAddr);
				}
				#else
				if((MemTag[i].Allocate_StartAddr != 0) && (MemTag[i].Allocate_VirAddr != 0))
					dvr_free((void *)MemTag[i].Allocate_VirAddr);
				#endif
				rtd_pr_scaler_memory_debug("[SCALER]FREE address %x\n",MemTag[i].Allocate_StartAddr);
				MemTag[i].Allocate_StartAddr = 0;
				MemTag[i].StartAddr = _DUMMYADDR;
				MemTag[i].Size = 0;
				MemTag[i].Status = UNDEFINED;
				MemTag[i].Allocate_VirAddr = 0;
			}
		}
	}

	if(Idx == MEMIDX_MAIN_SEC)
	{
		set_m_domain_borrow_buffer(NULL, 0);//free memory
	}
	if ((Idx == MEMIDX_ALL) || (Idx == MEMIDX_MAIN_THIRD) || (Idx == MEMIDX_MAIN_SEC)) {
		if(MemTag[MEMIDX_MAIN_SEC].Allocate_StartAddr != 0) {
			MemTag[MEMIDX_MAIN_SEC].Allocate_StartAddr = 0;
		}
	}
#else
	if ( (Idx>=MEMIDX_VDC) && (Idx<MEMIDX_ALL) ) {
		MemTag[Idx].StartAddr = _DUMMYADDR;
		MemTag[Idx].Size = 0;
		MemTag[Idx].Status = UNDEFINED;
	} else if ( Idx == MEMIDX_ALL ) {
		for ( i=MEMIDX_VDC ; i<MEMIDX_ALL ; ++i ) {	// clear all tag information
			MemTag[i].StartAddr = _DUMMYADDR;
			MemTag[i].Size = 0;
			MemTag[i].Status = UNDEFINED;
		}
	}
#endif
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


unsigned long drvif_memory_get_mainblock_viraddr(void)
{
#if 0
	unsigned int virstaaddr = 0;
	unsigned int phystaaddr = 0;
	if(MemTag[MEMIDX_MAIN_THIRD].Allocate_VirAddr !=0) {
		virstaaddr = MemTag[MEMIDX_MAIN_THIRD].Allocate_VirAddr;
		phystaaddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr;
	} else if (MemTag[MEMIDX_MAIN_SEC].Allocate_VirAddr !=0) {
		virstaaddr = MemTag[MEMIDX_MAIN_SEC].Allocate_VirAddr;
		phystaaddr = MemTag[MEMIDX_MAIN_SEC].StartAddr;
	} else if (MemTag[MEMIDX_MAIN].Allocate_VirAddr !=0) {
		virstaaddr = MemTag[MEMIDX_MAIN].Allocate_VirAddr;
		phystaaddr = MemTag[MEMIDX_MAIN].StartAddr;
	}

	return (virstaaddr + MemTag[MEMIDX_MAIN].StartAddr - phystaaddr);
#else
	return MemTag[MEMIDX_MAIN_THIRD].Allocate_VirAddr;
#endif
}

unsigned long drvif_memory_get_subblock_viraddr(void)
{
#if 0
	unsigned int virstaaddr = 0;
	unsigned int phystaaddr = 0;
	if(MemTag[MEMIDX_SUB_THIRD].Allocate_VirAddr !=0) {
		virstaaddr = MemTag[MEMIDX_SUB_THIRD].Allocate_VirAddr;
		phystaaddr = MemTag[MEMIDX_SUB_THIRD].StartAddr;
	} else if (MemTag[MEMIDX_SUB_SEC].Allocate_VirAddr !=0) {
		virstaaddr = MemTag[MEMIDX_SUB_SEC].Allocate_VirAddr;
		phystaaddr = MemTag[MEMIDX_SUB_SEC].StartAddr;
	} else if (MemTag[MEMIDX_SUB].Allocate_VirAddr !=0) {
		virstaaddr = MemTag[MEMIDX_SUB].Allocate_VirAddr;
		phystaaddr = MemTag[MEMIDX_SUB].StartAddr;
	}

	return (virstaaddr + MemTag[MEMIDX_SUB].StartAddr - phystaaddr);
#else
	return MemTag[MEMIDX_SUB_THIRD].Allocate_VirAddr;
#endif
}

/*============================================================================*/
/**
 * MemAlignBank
 * Memory alignment for DDR bank0
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { none }
 *
 */
unsigned int drvif_memory_alignbank(unsigned int StartAddr, unsigned char AlignBank, unsigned char Frm)
{
	unsigned int getValue;
	//rtd_pr_scaler_memory_debug("1. Start Addr = %x in Memory_AlignBank\n",StartAddr);
	if(AlignBank < MEMALIGN_ANY){
		getValue = (StartAddr & ((1 << CONFIG_DDR_BANK_BITS) - 1) << (CONFIG_DDR_COL_BITS+1)) >> (CONFIG_DDR_COL_BITS+1);
		getValue = getValue % (1 << CONFIG_DDR_BANK_BITS);
		if(Frm == ALLOCATED_FROM_LOW) {
			getValue = (1 << CONFIG_DDR_BANK_BITS) - getValue + AlignBank;

			StartAddr += (getValue * (1 << (CONFIG_DDR_COL_BITS+1)));
			//rtd_pr_scaler_memory_debug("2. L.Start Addr = %x getValue = %x in Memory_AlignBank\n",StartAddr,getValue);
		}
		else if(Frm == ALLOCATED_FROM_HIGH) {
			if(getValue > AlignBank)
				getValue -= AlignBank;
			else
				getValue = AlignBank - getValue;

			StartAddr -= (getValue * (1 << (CONFIG_DDR_COL_BITS+1)));
			//rtd_pr_scaler_memory_debug("2. H.Start Addr = %x getValue = %x in Memory_AlignBank\n",StartAddr,getValue);
		}
	}

	return StartAddr;
}

#ifdef UT_flag
unsigned int g_ulsMdomainAddr[QUERY_IDX_MAX] = {0, 0, 0, 0, 0, 0, 0, 0};

void memory_set_mdomain_query_addr_for_UT(unsigned char idx, unsigned int a_ulAddr)
{
	g_ulsMdomainAddr[idx] = a_ulAddr;
}

unsigned int memory_get_mdomain_query_addr_for_UT(unsigned char idx)
{
	return g_ulsMdomainAddr[idx];
}
#endif // #ifdef UT_flag
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
	unsigned int buffer_size_main = MDOMAIN_MAIN_BUFFER_SIZE;
	//rtd_pr_scaler_memory_info("[%s]reserved memory start addr = 0x%x, size = 0x%x, MDOMAIN_MAIN_BUFFER_SIZE = 0x%x\n", __FUNCTION__, addr, size, MDOMAIN_MAIN_BUFFER_SIZE);
	//rtd_pr_scaler_memory_info("[%s]VIP_DI_1_BUFFER_SIZE = 0x%x, VIP_DI_2_BUFFER_SIZE = 0x%x, VIP_DMAto3DTable_BUFFER_SIZE = 0x%x\n", __FUNCTION__, VIP_DI_1_BUFFER_SIZE, VIP_DI_2_BUFFER_SIZE, VIP_DMAto3DTable_BUFFER_SIZE);
	//rtd_pr_scaler_memory_info("buffer_size_main=%x\n", buffer_size_main);

	if(idx == QUERY_IDX_MDOMAIN_MAIN){
  		//do nothing
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

			size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void **)&addr) ;
			rtd_pr_scaler_memory_emerg("[borrow memory] HDMI VRR  m-domain use CARVEDOUT_SCALER_MEMC size=%x, addr=%lx\n", size, addr);
		}
	} else if(idx == QUERY_IDX_DI) {
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

	if (idx != QUERY_IDX_VIP_DMAto3DTABLE && idx != QUERY_IDX_VIP_DeMura)
		rtd_pr_scaler_memory_info("[%s]idx = %d, addr = 0x%lx, queried size=%x\n", __FUNCTION__, idx, addr, size);
	return addr;
}


/*============================================================================*/
/**
 * drvif_memory_alloc_block
 * allocate memory of the specific tag
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
  * @param <space> { size of DDR memory space,uint:byte}
 * @return { start address of the allocated memory }
 *
 * @Note:
 *  You must drvif_memory_free_block to free block you allocated before calling this function to allocate memory space.
 *  It possible wrong operatoin if you don't observer the front.
 */
//extern unsigned char Get_zoom_smoothtoggle_changeratio_flag(void);
unsigned int drvif_memory_alloc_block(eMemIdx Idx, unsigned int space, unsigned char Align)
{
	unsigned int m_allocate_memory = 0;
	unsigned int buffer_size = 0;

#if 0 // do not allocate scaler mem again for vo smooth toggle
	unsigned char display;
	if (Idx >= MEMIDX_SUB && Idx <= MEMIDX_SUB_THIRD) {
		display = SLR_SUB_DISPLAY;
	} else {
		display = SLR_MAIN_DISPLAY;
	}
#if 0
	if((Get_vo_smoothtoggle_changeratio_flag(display)||Get_vo_smoothtoggle_timingchange_flag(display)))
	{
		if(MemTag[Idx].StartAddr!=_DUMMYADDR)
		{
			rtd_pr_scaler_memory_debug("[smooth toggle]do not alloc %d memory:%x\n",Idx,MemTag[Idx].StartAddr);
			return MemTag[Idx].StartAddr;
		}
		else
		{
			if(Idx==MEMIDX_MAIN_THIRD)
			{
				if((MemTag[MEMIDX_MAIN_SEC].StartAddr!=_DUMMYADDR)||(MemTag[MEMIDX_MAIN].StartAddr!=_DUMMYADDR))
				{
				     rtd_pr_scaler_memory_debug("[smooth toggle]MEMIDX_MAIN_THIRD do not alloc memory\n");
				     return MemTag[Idx].StartAddr;
				}
			} else {
				if(!Get_vo_smoothtoggle_timingchange_flag(display)) {
					rtd_pr_scaler_memory_debug("[change ratio smooth toggle]%d do not alloc memory:%x\n",Idx,MemTag[Idx].StartAddr);
					//memory protection @Crixus 20160101
					if((MemTag[Idx].StartAddr != 0x0) && (MemTag[Idx].StartAddr != 0xffffffff))
						return MemTag[Idx].StartAddr;
				}
		   }
		}
	}
#endif
#endif

#if 0
	if(Get_zoom_smoothtoggle_changeratio_flag())
	{// do not allocate DI mem again rika 20140829
		rtd_pr_scaler_memory_info("_rk [MEM] Idx[%d]=%x\n", Idx, MemTag[Idx].StartAddr);
		return MemTag[Idx].StartAddr;
	}
#endif

//	static unsigned int dcu2_address = 0;
#ifndef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	if(AllocStartAddr == 0)
		return _DUMMYADDR;
#endif
	//unsigned int StartAddr;
	if((Idx >= MEMIDX_VDC) && (Idx < MEMIDX_ALL)) {
#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
			if(MEMIDX_MAIN_THIRD == Idx){
				space = drvif_memory_get_data_align(space*3+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
			}else if(MEMIDX_MAIN_SEC == Idx){
				if(MemTag[MEMIDX_MAIN_THIRD].StartAddr != _DUMMYADDR){ //triple buffer case
					MemTag[Idx].Allocate_StartAddr = 0;
					MemTag[Idx].Size = space;
					MemTag[Idx].Status = MemTag[MEMIDX_MAIN_THIRD].Status;
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
					MemTag[MEMIDX_MAIN_THIRD].Size -= space;
					#ifdef CONFIG_ARM64
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#else
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#endif
					return MemTag[Idx].StartAddr;
				}else{ //double buffer case
					space = drvif_memory_get_data_align(space*2+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
				}
			}else if(MEMIDX_MAIN == Idx){
				if(MemTag[MEMIDX_MAIN_THIRD].StartAddr != _DUMMYADDR){ //triple buffer case
					MemTag[Idx].Allocate_StartAddr = 0;
					MemTag[Idx].Size = space;
					MemTag[Idx].Status = MemTag[MEMIDX_MAIN_THIRD].Status;
					//sync k8?
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+space*2, DMA_SPEEDUP_ALIGN_VALUE);
					MemTag[MEMIDX_MAIN_THIRD].Size -= space;
					#ifdef CONFIG_ARM64
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#else
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#endif
					return MemTag[Idx].StartAddr;
				}else if(MemTag[MEMIDX_MAIN_SEC].StartAddr != _DUMMYADDR){//double buffer case
					MemTag[Idx].Allocate_StartAddr = 0;
					MemTag[Idx].Size = space;
					MemTag[Idx].Status = MemTag[MEMIDX_MAIN_SEC].Status;
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
					MemTag[MEMIDX_MAIN_SEC].Size -= space;
					#ifdef CONFIG_ARM64
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#else
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#endif
					return MemTag[Idx].StartAddr;
				}else{
					space = drvif_memory_get_data_align(space+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
				}
			}else if(MEMIDX_SUB_THIRD == Idx){
				if(sub_use_main_buffer_case() && (MemTag[MEMIDX_MAIN_THIRD].StartAddr != _DUMMYADDR))
                {//co buffer
                    MemTag[Idx].Allocate_StartAddr = 0;
                    MemTag[Idx].Size = space;
                    MemTag[Idx].Status = MemTag[MEMIDX_MAIN_THIRD].Status;
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
                    //MemTag[MEMIDX_MAIN_THIRD].Size -= space;
#ifdef CONFIG_ARM64
                    rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
#else
                    rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
#endif
                    return MemTag[Idx].StartAddr;
                }
				space = space*3+CONFIG_MDOMAIN_BURST_SIZE;
			}else if(MEMIDX_SUB_SEC == Idx){
				if(sub_use_main_buffer_case() && (MemTag[MEMIDX_MAIN_SEC].StartAddr != _DUMMYADDR))
                {//co buffer
                    MemTag[Idx].Allocate_StartAddr = 0;
                    MemTag[Idx].Size = space;
                    MemTag[Idx].Status = MemTag[MEMIDX_MAIN_SEC].Status;
                    MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+space*2, DMA_SPEEDUP_ALIGN_VALUE);
                    //MemTag[MEMIDX_MAIN_THIRD].Size -= space;
#ifdef CONFIG_ARM64
                    rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
#else
                    rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
#endif
                    return MemTag[Idx].StartAddr;
                }else if(MemTag[MEMIDX_SUB_THIRD].StartAddr != _DUMMYADDR){ //triple buffer case
					MemTag[Idx].Allocate_StartAddr = 0;
					MemTag[Idx].Size = space;
					MemTag[Idx].Status = MemTag[MEMIDX_SUB_THIRD].Status;
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_SUB_THIRD].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
					MemTag[MEMIDX_SUB_THIRD].Size -= space;
					#ifdef CONFIG_ARM64
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#else
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#endif
					return MemTag[Idx].StartAddr;
				}else{ //double buffer case
					space = drvif_memory_get_data_align(space*2+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
				}
			}else if(MEMIDX_SUB == Idx){
				if(sub_use_main_buffer_case() && (MemTag[MEMIDX_MAIN_SEC].StartAddr != _DUMMYADDR))
                {//co buffer
                    MemTag[Idx].Allocate_StartAddr = 0;
                    MemTag[Idx].Size = space;
					MemTag[Idx].Status = MemTag[MEMIDX_MAIN_SEC].Status;
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
#ifdef CONFIG_ARM64
                    rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
#else
                    rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
#endif
                    return MemTag[Idx].StartAddr;
                }else if(MemTag[MEMIDX_SUB_THIRD].StartAddr != _DUMMYADDR){ //triple buffer case
					MemTag[Idx].Allocate_StartAddr = 0;
					MemTag[Idx].Size = space;
					MemTag[Idx].Status = MemTag[MEMIDX_SUB_THIRD].Status;
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_SUB_THIRD].StartAddr+space*2, DMA_SPEEDUP_ALIGN_VALUE);
					MemTag[MEMIDX_SUB_THIRD].Size -= space;
					#ifdef CONFIG_ARM64
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#else
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#endif
					return MemTag[Idx].StartAddr;
				}else if(MemTag[MEMIDX_SUB_SEC].StartAddr != _DUMMYADDR){//double buffer case
					MemTag[Idx].Allocate_StartAddr = 0;
					MemTag[Idx].Size = space;
					MemTag[Idx].Status = MemTag[MEMIDX_SUB_SEC].Status;
					MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_SUB_SEC].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
					MemTag[MEMIDX_SUB_SEC].Size -= space;
					#ifdef CONFIG_ARM64
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#else
					rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
					#endif
					return MemTag[Idx].StartAddr;
				}else{
					space = drvif_memory_get_data_align(space+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
				}
			}

		if(MEMIDX_MAIN_2_THIRD == Idx){
			space = drvif_memory_get_data_align(space*3+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
		}else if(MEMIDX_MAIN_2_SEC == Idx){
			if(MemTag[MEMIDX_MAIN_2_THIRD].StartAddr != _DUMMYADDR){ //triple buffer case
				MemTag[Idx].Allocate_StartAddr = 0;
				MemTag[Idx].Size = space;
				MemTag[Idx].Status = MemTag[MEMIDX_MAIN_2_THIRD].Status;
				MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_2_THIRD].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
				MemTag[MEMIDX_MAIN_2_THIRD].Size -= space;
				#ifdef CONFIG_ARM64
				rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
				#else
				rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
				#endif
				return MemTag[Idx].StartAddr;
			}else{ //double buffer case
				space = drvif_memory_get_data_align(space*2+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
			}
		}else if(MEMIDX_MAIN_2 == Idx){
			if(MemTag[MEMIDX_MAIN_2_THIRD].StartAddr != _DUMMYADDR){ //triple buffer case
				MemTag[Idx].Allocate_StartAddr = 0;
				MemTag[Idx].Size = space;
				MemTag[Idx].Status = MemTag[MEMIDX_MAIN_2_THIRD].Status;
				MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_2_THIRD].StartAddr+space*2, DMA_SPEEDUP_ALIGN_VALUE);
				MemTag[MEMIDX_MAIN_2_THIRD].Size -= space;
				#ifdef CONFIG_ARM64
				rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
				#else
				rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
				#endif
				return MemTag[Idx].StartAddr;
			}else if(MemTag[MEMIDX_MAIN_SEC].StartAddr != _DUMMYADDR){//double buffer case
				MemTag[Idx].Allocate_StartAddr = 0;
				MemTag[Idx].Size = space;
				MemTag[Idx].Status = MemTag[MEMIDX_MAIN_SEC].Status;
				MemTag[Idx].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+space, DMA_SPEEDUP_ALIGN_VALUE);
				MemTag[MEMIDX_MAIN_SEC].Size -= space;
				#ifdef CONFIG_ARM64
				rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
				#else
				rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
				#endif
				return MemTag[Idx].StartAddr;
			}else{
				space = drvif_memory_get_data_align(space+CONFIG_MDOMAIN_BURST_SIZE, DMA_SPEEDUP_ALIGN_VALUE);
			}
		}


		// Pli Fail
#ifdef PLI_FAIL
		if(Idx ==MEMIDX_MAIN || Idx ==MEMIDX_MAIN_SEC || Idx ==MEMIDX_MAIN_THIRD)
		{
			m_allocate_memory = 0x1d000000;
		}
		else if (Idx == MEMIDX_VDC)
		{
			m_allocate_memory = 0x1c000000;
		}
		else if (Idx == MEMIDX_DI)
		{
			m_allocate_memory = 0x1e000000;
		}
		else
		{
			rtd_pr_scaler_memory_debug("Error, can't handle reserverd memory, mem Idx=%d\n", Idx);
		}
#else
		//m_allocate_memory = pli_allocOverlappedMemory(space + (1 << 5));
#endif
#if 0
		if(m_allocate_memory == 0){
			unsigned char				*m_pNonCacheStartAddr;
			rtd_pr_scaler_memory_debug("Allocate RBUS index %d memory fail,allocate by pli\n",Idx);
			#if 0//qiang_zhou:fixme memory alloc
			m_pCacheStartAddr = (BYTE *)pli_allocContinuousMemorySpecific((space + (1 << 5)), (char**)&m_pNonCacheStartAddr, &m_PhyStartAddr, GFP_DCU1);
			#endif
			if(m_pCacheStartAddr == NULL){
				rtd_pr_scaler_memory_debug("[ERROR]Allocate DCU1 size=%x fail\n",(space + (1 << 5)));
				return FALSE;
			}else{
				rtd_pr_scaler_memory_debug("[SCALER]PLI Allocate address %x\n",m_PhyStartAddr);
				MemTag[Idx].Allocate_StartAddr = (unsigned int)m_pCacheStartAddr;
				MemTag[Idx].Size = space;
				MemTag[Idx].Status = ALLOCATED_FROM_PLI;
				MemTag[Idx].StartAddr = drvif_memory_get_data_align(m_PhyStartAddr, (1 << 5));
			}
		}else{
			rtd_pr_scaler_memory_debug("[SCALER]Allocate address %x - %x\n",m_allocate_memory, (m_allocate_memory+space + (1 << 5)));
			MemTag[Idx].Allocate_StartAddr = m_allocate_memory;
			MemTag[Idx].Size = space;
			MemTag[Idx].Status = ALLOCATED_FROM_PLI;
			MemTag[Idx].StartAddr = drvif_memory_get_data_align(m_allocate_memory, (1 << 5));
		}
#else

		if(MEMIDX_DI == Idx){//DI, RTNR allocate memory by overlap memory @Crixus 20151122
			//change to use query driver @Crixus 20170505
			m_allocate_memory = get_query_start_address(QUERY_IDX_DI);
			//rtd_pr_scaler_memory_emerg("[crixus]DI non-i2rnd memory address = %x, space = %x\n", m_allocate_memory, space);
		}
		else{//M-domain allocate memory by DCU2
#ifndef BUILD_QUICK_SHOW
			buffer_size = dvr_size_alignment_cma(space + (1 << 5));
			m_pCacheStartAddr = (unsigned char *)dvr_malloc_specific(buffer_size, GFP_DCU2_FIRST);
			if(m_pCacheStartAddr == NULL){
				rtd_pr_scaler_memory_notice("[ERROR]Allocate DCU1 size=%x fail\n",buffer_size);
				BUG();//add kernel crash function when cma not enough
				return FALSE;
			}
			m_allocate_memory = (unsigned int)dvr_to_phys((void*)m_pCacheStartAddr);

			m_allocate_memory = dvr_memory_alignment((unsigned long)m_allocate_memory, buffer_size);
#endif
		}

		rtd_pr_scaler_memory_debug("[SCALER]Allocate address %x - %x\n",m_allocate_memory, (m_allocate_memory+buffer_size));
		MemTag[Idx].Allocate_StartAddr = m_allocate_memory;
		MemTag[Idx].Size = space;
		MemTag[Idx].Status = ALLOCATED_FROM_PLI;
		MemTag[Idx].StartAddr = drvif_memory_get_data_align(m_allocate_memory, DMA_SPEEDUP_ALIGN_VALUE);
		MemTag[Idx].Allocate_VirAddr = (unsigned long)m_pCacheStartAddr;
#endif

		//rtd_pr_scaler_memory_emerg("[crixus]MemTag[Idx].StartAddr = %x\n", MemTag[Idx].StartAddr);
		//rtd_pr_scaler_memory_emerg("[crixus]DI_MULTIVIEW_ADDRESS_SUB = %x\n", DI_MULTIVIEW_ADDRESS_SUB);
		//rtd_pr_scaler_memory_emerg("[crixus](DI_MULTIVIEW_ADDRESS_SUB + space - CONFIG_MDOMAIN_BURST_SIZE) = %x\n", (DI_MULTIVIEW_ADDRESS_SUB + space - CONFIG_MDOMAIN_BURST_SIZE));
		//rtd_pr_scaler_memory_emerg("[crixus]space = %x\n", space);
#else
		if((MemTag[Idx].StartAddr != _DUMMYADDR) && (Idx != MEMIDX_VDC)) {
			if(space <= MemTag[Idx].Size) {
				return MemTag[Idx].StartAddr;
			} else {
				memory_set_mem_tag(Idx, _DUMMYADDR, 0);
				return _DUMMYADDR;
			}
		}
		space = drvif_memory_get_data_align(space, (1 << _DDR_BITS));
		switch(Idx) {
			case MEMIDX_VDC:
				if(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) {
					MemTag[Idx].Status = ALLOCATED_FROM_LOW;
					memory_set_mem_tag(Idx, AllocStartAddr, space);
				} else {
					MemTag[Idx].Status = ALLOCATED_FROM_HIGH;
					memory_set_mem_tag(Idx, AllocEndAddr - space + 1, space);  // add a bank gap avoid Main sec Blk cover Bitmap
				}
				break;

			case MEMIDX_TT:
				if(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) {
					MemTag[Idx].Status = ALLOCATED_FROM_LOW;
					if(MemTag[MEMIDX_VDC].Status == ALLOCATED_FROM_LOW) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr + MemTag[MEMIDX_VDC].Size, space + (1 << _DDR_BITS));
					} else {
						memory_set_mem_tag(Idx, AllocStartAddr, space + (1 << _DDR_BITS));
					}
				} else {
					MemTag[Idx].Status = ALLOCATED_FROM_HIGH;
					if(MemTag[MEMIDX_VDC].Status ==  ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr - space + 1, space);
					} else {
						memory_set_mem_tag(Idx, AllocEndAddr - space + 1, space);
					}
				}
				break;

			case MEMIDX_JPEG:
				if(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) {
					MemTag[Idx].Status = ALLOCATED_FROM_LOW;
					memory_set_mem_tag(Idx, AllocStartAddr, space);
				} else {
					MemTag[Idx].Status = ALLOCATED_FROM_HIGH;
					memory_set_mem_tag(Idx, AllocEndAddr - space + 1, space);  // add a bank gap avoid Main sec Blk cover Bitmap
				}
				break;


			case MEMIDX_DI:
				if(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) {
					MemTag[Idx].Status = ALLOCATED_FROM_LOW;

					if(MemTag[MEMIDX_TT].Status ==  ALLOCATED_FROM_LOW) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_TT].StartAddr + MemTag[MEMIDX_TT].Size, space + (1 << _DDR_BITS));
					}
					else if(MemTag[MEMIDX_VDC].Status ==  ALLOCATED_FROM_LOW) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr + MemTag[MEMIDX_VDC].Size, space + (1 << _DDR_BITS));
					}
					else if(MemTag[MEMIDX_JPEG].Status == ALLOCATED_FROM_LOW) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_JPEG].StartAddr + MemTag[MEMIDX_JPEG].Size, space + (1 << _DDR_BITS));
					}
					else
					{
						memory_set_mem_tag(Idx, AllocStartAddr, space + (1 << _DDR_BITS));
					}
				} else {
					MemTag[Idx].Status = ALLOCATED_FROM_HIGH;
					if(MemTag[MEMIDX_TT].Status ==  ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_TT].StartAddr - space + 1, space);
					}else if(MemTag[MEMIDX_VDC].Status ==  ALLOCATED_FROM_HIGH) {
							//memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr - MemTag[MEMIDX_VDC].Size + 1, space);
							//////////////////////////////////
							//By Gilbert(2008/9/23)
							///////////////////////////////////
							memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr - space + 1, space);
					}else if  (MemTag[MEMIDX_JPEG].Status ==  ALLOCATED_FROM_HIGH) {
							memory_set_mem_tag(Idx, MemTag[MEMIDX_JPEG].StartAddr - space + 1, space);
					} else {
							rtd_pr_scaler_memory_debug("[qiang_zhou debug]:MemTag[MEMIDX_DI].Status==%d\n",MemTag[MEMIDX_DI].Status);
							memory_set_mem_tag(Idx, AllocEndAddr - space + 1, space);
					}
				}
				break;

			case MEMIDX_MAIN:
				MemTag[Idx].Status = ALLOCATED_FROM_HIGH;
				if(MemTag[MEMIDX_MAIN_SEC].StartAddr != _DUMMYADDR) {
					if((Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID) > 2048)
						&& (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN) > 1200)&& !Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_422CAP)){	// 4k2k
						// [IDMA] IDMA capture buffer share memory with video decoder
						SCALERDRV_ALLOCBUFFER_INFO stAllocBufferInfo_idma;
						if(drvif_scaler_getValidMemInfo_fromVideo(&stAllocBufferInfo_idma) < 0){
							//unsigned char				*m_pNonCacheStartAddr;
							rtd_pr_scaler_memory_debug("[IDMA_M domain] fail to get memory information\n");
							#if 0//qiang_zhou:fixme memory alloc
							m_pCacheStartAddr = (BYTE *)pli_allocContinuousMemorySpecific(space, (char**)&m_pNonCacheStartAddr, &m_PhyStartAddr, GFP_DCU1);
							#else
							m_pCacheStartAddr=dvr_malloc(space);
							//m_PhyStartAddr = (unsigned int)virt_to_phys((void*)m_pCacheStartAddr);
							m_PhyStartAddr = (unsigned int)dvr_to_phys((void*)m_pCacheStartAddr);
							#endif
							if(m_pCacheStartAddr == NULL){
								rtd_pr_scaler_memory_debug("[ERROR]Allocate M domain DCU1 size=%x fail\n",mMallocSize);
								return FALSE;
							}else{
								//physical address
								MemTag[Idx].StartAddr = m_PhyStartAddr;
								MemTag[Idx].Size = space;
								rtd_pr_scaler_memory_debug("[SLR_MEMORY5][PLI][Cache:%x][Phy:%x][Size:%x]\n", (unsigned int)m_pCacheStartAddr, (unsigned int)m_PhyStartAddr, space);

							}
						}else{
							MemTag[Idx].StartAddr = stAllocBufferInfo_idma.normal_mem_addr&0xffffffff;
							MemTag[Idx].Size = space;
							rtd_pr_scaler_memory_debug("[IDMA_M domain] get memory information OK,addr=%x\n",MemTag[Idx].StartAddr);
						}

						if(MemTag[Idx].StartAddr < AllocStartAddr){
							IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr1_reg, MemTag[Idx].StartAddr);
						}else if(MemTag[Idx].StartAddr > AllocEndAddr){
							IoReg_Write32(MDOMAIN_CAP_CapMain_BoundaryAddr2_reg,MemTag[Idx].StartAddr+space-CONFIG_MDOMAIN_BURST_SIZE);
						}
					}else{
					memory_set_mem_tag(Idx, MemTag[MEMIDX_MAIN_SEC].StartAddr  - space + 1, space);
					}
				} else {

					if(MemTag[MEMIDX_DI].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_DI].StartAddr - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else if(MemTag[MEMIDX_TT].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_TT].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else if(MemTag[MEMIDX_VDC].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else if(MemTag[MEMIDX_JPEG].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_JPEG].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else {
						memory_set_mem_tag(Idx, AllocEndAddr - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					}
				}
				break;
			case MEMIDX_MAIN_SEC:
				MemTag[Idx].Status = ALLOCATED_FROM_HIGH;
				if(MemTag[MEMIDX_MAIN_THIRD].StartAddr != _DUMMYADDR) {
					memory_set_mem_tag(Idx, MemTag[MEMIDX_MAIN_THIRD].StartAddr - space + 1, space);  // add a bank gap avoid Main sec Blk cover Bitmap
				} else {
					if(MemTag[MEMIDX_DI].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_DI].StartAddr - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					}
					else if(MemTag[MEMIDX_TT].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_TT].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else if(MemTag[MEMIDX_VDC].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else if(MemTag[MEMIDX_JPEG].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_JPEG].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else {
						memory_set_mem_tag(Idx, AllocEndAddr - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					}
				}
				break;


			case MEMIDX_MAIN_THIRD:
				MemTag[Idx].Status = ALLOCATED_FROM_HIGH;
				if(MemTag[MEMIDX_DI].Status == ALLOCATED_FROM_HIGH) {
					memory_set_mem_tag(Idx, MemTag[MEMIDX_DI].StartAddr - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
				} else {
					if(MemTag[MEMIDX_TT].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_TT].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else if(MemTag[MEMIDX_VDC].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else if(MemTag[MEMIDX_JPEG].Status == ALLOCATED_FROM_HIGH) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_JPEG].StartAddr  - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					} else {
						memory_set_mem_tag(Idx, AllocEndAddr - (space + (1 << _DDR_BITS)) + 1, (space + (1 << _DDR_BITS)));  // add a bank gap avoid Main sec Blk cover Bitmap
					}
				}
				break;


			case MEMIDX_SUB:
				MemTag[Idx].Status = ALLOCATED_FROM_LOW;
				if(MemTag[MEMIDX_DI].Status == ALLOCATED_FROM_LOW) {
					memory_set_mem_tag(Idx, MemTag[MEMIDX_DI].StartAddr + MemTag[MEMIDX_DI].Size, space);
				} else {
					if(MemTag[MEMIDX_TT].Status == ALLOCATED_FROM_LOW) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_TT].StartAddr + MemTag[MEMIDX_TT].Size, space);
					} else if(MemTag[MEMIDX_VDC].Status == ALLOCATED_FROM_LOW) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_VDC].StartAddr + MemTag[MEMIDX_VDC].Size, space);
					} else if(MemTag[MEMIDX_JPEG].Status == ALLOCATED_FROM_LOW) {
						memory_set_mem_tag(Idx, MemTag[MEMIDX_JPEG].StartAddr + MemTag[MEMIDX_JPEG].Size, space);
					} else {
						memory_set_mem_tag(Idx, AllocStartAddr, space);
					}
				}
				IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr1_reg,(MemTag[Idx].StartAddr & 0xfffffff0));
				break;

			case MEMIDX_SUB_SEC:
				if(MemTag[MEMIDX_SUB].StartAddr != _DUMMYADDR) {
					MemTag[Idx].Status = ALLOCATED_FROM_LOW;
					memory_set_mem_tag(Idx, MemTag[MEMIDX_SUB].StartAddr + MemTag[MEMIDX_SUB].Size, space);
				} else
					memory_set_mem_tag(Idx, _DUMMYADDR, 0);
				IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr2_reg,(MemTag[Idx].StartAddr+MemTag[Idx].Size) & 0xfffffff0);
				break;


			case MEMIDX_SUB_THIRD:
				if(MemTag[MEMIDX_SUB_SEC].StartAddr != _DUMMYADDR) {
					MemTag[Idx].Status = ALLOCATED_FROM_LOW;
					memory_set_mem_tag(Idx, MemTag[MEMIDX_SUB_SEC].StartAddr + MemTag[MEMIDX_SUB_SEC].Size, space);
				} else
					memory_set_mem_tag(Idx, _DUMMYADDR, 0);
				IoReg_Write32(MDOMAIN_CAP_CapSub_BoundaryAddr2_reg,(MemTag[Idx].StartAddr+MemTag[Idx].Size) & 0xfffffff0);
				break;
			default:
				return _DUMMYADDR;
		};
#endif
	}

	#ifdef CONFIG_ARM64
	rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
	#else
	rtd_pr_scaler_memory_debug("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
	#endif

	if(MEMIDX_DI_2 == Idx){

		#ifdef CONFIG_ARM64
		rtd_pr_scaler_memory_emerg("[MEM] Idx[%d]=%lx(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
		#else
		rtd_pr_scaler_memory_emerg("[MEM] Idx[%d]=%x(%d KB)\n", Idx, MemTag[Idx].StartAddr, space >> 10);
		#endif
	}
	return MemTag[Idx].StartAddr;
}


#ifdef CONFIG_MPIXEL_SHIFT
void memory_v_shift(int v)
{
	unsigned int ucStartAddress;
	unsigned int ucStartAddress2;
	unsigned int TotalSize;
	ucStartAddress = drvif_memory_get_block_addr(MEMIDX_MAIN);
	ucStartAddress2 = drvif_memory_get_block_addr(MEMIDX_MAIN_SEC);
	TotalSize = memory_get_capture_size(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), MEMCAPTYPE_LINE);		// calculate capture line size
	TotalSize = SHL(TotalSize, 3); // unit conversion from 64bits to 8bits

	if (v >= 0)
	{
		TotalSize = TotalSize * v;
		ucStartAddress +=TotalSize;
		ucStartAddress2 +=TotalSize;
	}
	else
	{
		TotalSize = TotalSize * (-v);
		ucStartAddress -=TotalSize;
		ucStartAddress2 -=TotalSize;
	}

	rtd_pr_scaler_memory_debug("%d %x %x \n",v , ucStartAddress,ucStartAddress2);

	IoReg_Write32(MDOMAIN_DISP_DDR_MainAddr_reg, ucStartAddress& 0xfffffff0);
		//rtd_pr_scaler_memory_debug("After %s %d IoReg_Read32(MDOMAIN_DISP_DDR_MainAddr_reg)=%x\n",__FUNCTION__,__LINE__,IoReg_Read32(MDOMAIN_DISP_DDR_MainAddr_reg));
	IoReg_Write32(MDOMAIN_DISP_DDR_Main2ndAddr_reg, ucStartAddress2& 0xfffffff0);
	IoReg_SetBits(MDOMAIN_DISP_Disp0_db_ctrl_reg, MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
}

void memory_black_buf_init(unsigned int framesize,unsigned int  upperPhyAddress)
{
	unsigned int ucStartAddress;
	unsigned int ucStartAddress2;
	unsigned int upperVirtualAddr;
	unsigned int lowerPhyAddress,lowerVirtualAddr;
	unsigned int *lowerCurAddr,*upperCurAddr;
	unsigned int TotalSize;
	unsigned int i = 0;
	//upperPhyAddress = drvif_memory_get_block_addr(MEMIDX_MAIN);
	TotalSize = memory_get_capture_size(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), MEMCAPTYPE_LINE);		// calculate capture line size
	TotalSize = SHL(TotalSize, 3); // unit conversion from 64bits to 8bits
	TotalSize = TotalSize * 32;

	rtd_pr_scaler_memory_debug("address %x %x %d\n",upperPhyAddress,AllocStartAddr,get_scaler_memory_map_addr());

	upperVirtualAddr = get_scaler_memory_map_addr() + (upperPhyAddress - AllocStartAddr);

	lowerPhyAddress = upperPhyAddress + TotalSize + framesize;

	lowerVirtualAddr = get_scaler_memory_map_addr() + (lowerPhyAddress - AllocStartAddr);


	upperCurAddr  = (unsigned int*)upperVirtualAddr;
	lowerCurAddr  = (unsigned int*)lowerVirtualAddr;

	rtd_pr_scaler_memory_debug("address %x %x\n",lowerPhyAddress,lowerVirtualAddr);

	for(i = 0 ; i < TotalSize ; i= i + 4)
	{

		*upperCurAddr = 0x80008000;
		*lowerCurAddr = 0x80008000;

		upperCurAddr++;
		lowerCurAddr++;
	}

}


#endif

#undef ABS
#define ABS(X)			((X)<0 ? (-X) : (X))

extern unsigned char Get_vscFilmMode(void);
extern void filmmode_videofw_config(unsigned int filmmode, unsigned int buf4addr);

#if 0
static unsigned int memory_get_capture_max_size(unsigned char display, eMemCapAccessType AccessType)
{
	unsigned int TotalSize;
	unsigned char ddrbit;
	unsigned char dispInterlace = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display, SLR_DISP_INTERLACE);

	ddrbit =  (Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_CHANNEL) ? 0 : ((Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_10BIT)) << 1 ))  | ( !Scaler_DispGetStatus((SCALER_DISP_CHANNEL)display,SLR_DISP_422CAP));

	TotalSize = memory_get_line_size((Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID_PRE) > _DISP_WID) ? _DISP_WID :  Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPH_ACT_WID_PRE), (ddrbit & _BIT0), ((ddrbit & _BIT1) >> 1));

	// it must be times of 4
	TotalSize = drvif_memory_get_data_align(TotalSize, 0x04);

	// multiple total_lines if frame-access
	if (AccessType == MEMCAPTYPE_FRAME) {
			if(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN_PRE) > _DISP_LEN)
				TotalSize *= _DISP_LEN;
			else
				TotalSize *= Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)display,SLR_INPUT_IPV_ACT_LEN_PRE);

			if(dispInterlace)
				TotalSize = TotalSize * 2;
	}

	return TotalSize;
}
#endif

unsigned int g_ulRealBufSize[MAX_DISP_CHANNEL_NUM];

void drvif_memory_SetRealBufSize(unsigned char display, unsigned int a_ulSize)
{
	g_ulRealBufSize[display] = a_ulSize;
}

unsigned int drvif_memory_GetRealBufSize(unsigned char display)
{
	return g_ulRealBufSize[display];
}

unsigned int drv_get_caveout_buffer_size_by_platform(unsigned int pixel_bits)
{
	unsigned int width;
	unsigned int len;
	unsigned int buffersize = 0;
    calculate_carveout_size(SLR_MAIN_DISPLAY);
    width = Get_Val_Max_Width();
    len = Get_Val_Max_Len();

    if((width % 32) != 0)
    {
        width = width + (32 - (width % 32));
    }

	buffersize = MDOMAIN_CMP_LINE_MODE_BUFFER_SIZE(width, len, pixel_bits);

	return buffersize;
}

unsigned long mdomain_2k_memory_addr = 0;
unsigned long Get_Val_domain_2k_memory_addr_impl(void)
{
	return mdomain_2k_memory_addr;
}
unsigned long (*Get_Val_domain_2k_memory_addr)(void) = Get_Val_domain_2k_memory_addr_impl;

void Set_Val_domain_2k_memory_addr(unsigned long value)
{
	mdomain_2k_memory_addr=value;
}

#define REDUCE_DI_MEMORY_LIMIT_THRESHOLD 		720
/*============================================================================*/
/**
 * MemSetCapture
 * Set memory capture registers. It can be #1 or #2, based on current info structure
 *
 * @param <info> { display-info struecture }
 * @return { none }
 *
 */

static void *sub_m_domain_1st_buf_addr;
static void *sub_m_domain_2nd_buf_addr;
static void *sub_m_domain_3rd_buf_addr;

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

unsigned int m_domain_setting_err_status[MAX_DISP_CHANNEL_NUM];
void set_m_domain_setting_err_status(unsigned char display, eMDomainSettingErrStatus err_status)
{// Set error status during M domain setting flow. If any error occurs, don't enable Mcap_en.
	m_domain_setting_err_status[display] = err_status;
}

unsigned int get_m_domain_setting_err_status(unsigned char display)
{// Get error status druing M domain setting flow. If any error occurs, don't enable Mcap_en.
	return m_domain_setting_err_status[display];
}

#if 0
/*============================================================================*/
/**
 * MemSetCapture
 * Set memory capture registers. It can be #1 or #2, based on current info structure
 *
 * @param <info> { display-info struecture }
 * @return { none }
 *
 */
void jpeg_memory_alloc_capture2(eMemCapAccessType Cap_type,eMemIdx idx)
{
	unsigned int  TotalSize;
	unsigned int  TotalSize1;
	//unsigned int  nStartAddr;
	unsigned int  Quotient;
	unsigned char  Remainder;
	unsigned int burstLen = (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY)?_BURSTLENGTH2: _BURSTLENGTH);

	//frank@0325 double the M domain burst length to avoid 444 10 bits peak BW not enough
	if((_CHANNEL1 ==  Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) && ( !Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP))){
		burstLen=0xf0;
	}

	TotalSize = memory_get_capture_size(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), Cap_type);		// calculate the memory size of capture
	memory_division(TotalSize, burstLen, &Quotient, &Remainder);	// Do a division
	Remainder = drvif_memory_get_data_align(Remainder, 4);	// multiple of 4
	if (Remainder == 0) {	// remainder is not allowed to be zero
		Remainder = burstLen;
		Quotient -= 1;
	}

	TotalSize = SHL(Quotient * burstLen + Remainder, 3); //unit conversion. 64bits to 8bits

	TotalSize1 = memory_get_capture_max_size(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), Cap_type);

	TotalSize1 = SHL(TotalSize1, 3); //unit conversion. 64bits to 8bits

	if(Cap_type == MEMCAPTYPE_LINE){
		TotalSize1 *=Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_INPUT_IPV_ACT_LEN_PRE);
		TotalSize *=Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_INPUT_CAP_LEN);
	}


	if (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY) == SLR_MAIN_DISPLAY)	 {
		// set capture memory for main display
		if((TotalSize1 > TotalSize) &&
			((Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_PIPMLSR) && (Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_PIPMLSR_4x3) &&
			(Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_MP1C12B) && (Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_MP1L7B)))
			TotalSize1 = (TotalSize1 - TotalSize);
		else
			TotalSize1 = 0;


		//nStartAddr = drvif_memory_alloc_block(idx, TotalSize+TotalSize1+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);//space unit is byte
		//rtd_pr_scaler_memory_debug("[JPEG] %s MemTag[MEMIDX_SUB].StartAddr=%x\n",__FUNCTION__,nStartAddr);
	}
}
#endif
void jpeg_memory_free_capture2(void)
{
	drvif_memory_free_block(MEMIDX_SUB);
}

void memory_set_main_activewindow_change(void)
{
    memory_main_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        memory_main_update_display_C_buffer(3, dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY));
    }
}

void memory_set_main_displaywindow_change(void)
{
    memory_frc_set_pre_read_v_start(SLR_MAIN_DISPLAY, drv_Calculate_m_pre_read_value());

    memory_set_display_us_path(SLR_MAIN_DISPLAY);
    memory_main_set_rdma();
    memory_main_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        memory_main_set_rdma_c();
        memory_main_update_display_C_buffer(3, dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY));
    }
}

extern StructSrcRect Scaler_SubDispWindowGet(void);
void memory_set_sub_displaywindow_change(unsigned char changetomain)
{
    memory_frc_set_pre_read_v_start(SLR_SUB_DISPLAY, drv_Calculate_m_pre_read_value());

    memory_set_display_us_path(SLR_SUB_DISPLAY);
    memory_sub_set_rdma();
    memory_sub_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        memory_sub_set_rdma_c();
        memory_sub_update_display_C_buffer(3, dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY));
    }
}

#ifdef CONFIG_ARM64
unsigned long FilmModeCachaddr = 0;
unsigned long FilmModePhyadddr = 0;
#else
unsigned int FilmModeCachaddr = 0;
unsigned int FilmModePhyadddr = 0;
#endif
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

extern bool rtk_hal_vsc_GetRGB444Mode(void);
extern unsigned char get_AVD_Input_Source(void);

void mdomain_handler_onlycap(void)
{
	unsigned char display;
	//mdomain_vi_sub_sub_gctl_RBUS vi_gctl_reg;

	display = Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY);

	set_m_domain_setting_err_status(display, MDOMAIN_SETTING_NO_ERR); // reset M domain setting error status
	
    dvrif_memory_set_block_mode_enable(display, FALSE);
    dvrif_memory_set_cap_mode(display, MEMCAPTYPE_FRAME);
    dvrif_memory_set_buffer_number(display, 3);
    
    memory_frc_main_set_capture();

    /*
	vi_gctl_reg.regValue = IoReg_Read32(MDOMAIN_VI_SUB_SUB_GCTL_reg);
	if(vi_gctl_reg.vsce1 == 0) {
		vi_gctl_reg.vsce1 = 1;
		IoReg_Write32(MDOMAIN_VI_SUB_SUB_GCTL_reg, vi_gctl_reg.regValue);
	}
    */

	if(get_m_domain_setting_err_status(display) == MDOMAIN_SETTING_NO_ERR)
		drvif_memory_EnableMCap(display);
	else
		rtd_pr_scaler_memory_emerg("[%s] %d ERROR! Don't enable Mcap_en. (display=%d, err_status=%d)\n", __FUNCTION__, __LINE__, display, get_m_domain_setting_err_status(display));

}

/*============================================================================*/
/**
 * drvif_memory_get_data_align
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

#ifndef Mag2_New_Flow
// DI bit number control (saving bandwidth)
static void drv_memory_DMA420_config(unsigned char *bitNum)
{
	UINT32 width, lineSize_odd, lineSize_even;

	if(!bitNum){
		rtd_pr_scaler_memory_debug("ERR: NULL PTR@%s\n", __FUNCTION__);
		return;
	}

	// DI DMA 420 enable (saving bandwidth)
	if(drv_memory_get_ip_DMA420_mode() == SLR_DMA_420_mode){
		width = Scaler_DispGetInputInfo(SLR_INPUT_DI_WID);
		lineSize_even = (width * (*bitNum - ((fw_scalerip_get_DI_chroma_10bits())?10:8)));
		if((lineSize_even%64)==0)
			lineSize_even /=64;
		else
			lineSize_even = (lineSize_even+64)/64;
		lineSize_odd = (width * *bitNum);
		if((lineSize_odd%64)==0)
			lineSize_odd /=64;
		else
			lineSize_odd = (lineSize_odd+64)/64;

		rtd_pr_scaler_memory_debug("[DI] DMA 420 Enable, bit/c-bit/line wid/even/odd[%d/%d/%d/%d/%d]\n", *bitNum, fw_scalerip_get_DI_chroma_10bits(),width, lineSize_even, lineSize_odd);
		IoReg_Mask32(DI_IM_DI_DMA_VADDR,
			~(DI_DMA_420_EN_MASK| DI_DMA_LINESIZE_EVEN_MASK| DI_DMA_LINESIZE_ODD_MASK),
			_BIT30| (lineSize_even << DI_DMA_LINESIZE_EVEN_START_BIT)| lineSize_odd);
		// update bit number
		//*bitNum -= 4;
	}
	else
		IoReg_ClearBits(DI_IM_DI_DMA_VADDR, _BIT30);

	return;
}
#endif
#if 0
static void drv_memory_set_ip_compress_bit(unsigned char bit_num)
{
#if 0	//Elsie 20131206: [FIXME] all of the registers below do not exist.
	dicmp_cmp_resolution_RBUS dicmp_cmp_resolution_reg;
	dicmp_comp_setting_RBUS dicmp_comp_setting_reg;
	dicmp_decmp_resolution_RBUS dicmp_decmp_resolution_reg;
	dicmp_decomp_setting_RBUS dicmp_decomp_setting_reg;
	dicmp_cmp_resolution_reg.regValue = 0;
	dicmp_cmp_resolution_reg.cmp_enable = 1;
	dicmp_cmp_resolution_reg.cmp_width = drvif_memory_get_data_align(Scaler_DispGetInputInfo(SLR_INPUT_CAP_WID), 32)/32;
	dicmp_cmp_resolution_reg.cmp_height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
	dicmp_decmp_resolution_reg.regValue = 0;
	dicmp_decmp_resolution_reg.decmp_width = dicmp_cmp_resolution_reg.cmp_width;
	dicmp_decmp_resolution_reg.decmp_height = dicmp_cmp_resolution_reg.cmp_height;
	dicmp_comp_setting_reg.regValue = IoReg_Read32(DI_DICMP_COMP_SETTING_VADDR);
	dicmp_comp_setting_reg.cmp_rgb_yc_sel = 1;// Ycbcr
	dicmp_comp_setting_reg.cmp_channel_bits = !(Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT));
	dicmp_comp_setting_reg.short_term_bits = bit_num;
	dicmp_comp_setting_reg.frame_bits = bit_num;
	dicmp_comp_setting_reg.max_pixel_bits = bit_num;
	dicmp_comp_setting_reg.cmp_a_ch_mark = 1; // a channel is not used
	dicmp_decomp_setting_reg.regValue = IoReg_Read32(DI_DICMP_DECOMP_SETTING_VADDR);
	dicmp_decomp_setting_reg.decmp_frame_bits = dicmp_comp_setting_reg.frame_bits;
	dicmp_decomp_setting_reg.decmp_channel_bits = dicmp_comp_setting_reg.cmp_channel_bits;
	dicmp_decomp_setting_reg.decmp_a_ch_mark = 1; // a channel is not used
	dicmp_decomp_setting_reg.decmp_rgb_yc_sel = 1; // Ycbcr
	dicmp_decomp_setting_reg.decmp_fl_mode = 1; //frame mode
	IoReg_Write32(DI_DICMP_COMP_SETTING_VADDR, dicmp_comp_setting_reg.regValue);
	IoReg_Write32(DI_DICMP_DECOMP_SETTING_VADDR, dicmp_decomp_setting_reg.regValue);
	IoReg_Write32(DI_DICMP_DECMP_RESOLUTION_VADDR, dicmp_decmp_resolution_reg.regValue);
	IoReg_Write32(DI_DICMP_CMP_RESOLUTION_VADDR, dicmp_cmp_resolution_reg.regValue);
#endif
}
#endif

/*============================================================================*/
/**
 * drv_memory_disable_di_write_req
 * Set DI DMA multi-request num
 *
 * @param <disableDiWr> {1: disable DI request, 0: enable DI request}
 * @return { none }
 * @note
 * disable DI request when 3DDI state change
 *
 */
void drv_memory_disable_di_write_req(unsigned char disableDiWr)
{
	di_di_dma_multi_req_num_RBUS di_dma_multi_req_num_reg;

	di_dma_multi_req_num_reg.regValue = IoReg_Read32(DI_DI_DMA_Multi_Req_Num_reg);
	di_dma_multi_req_num_reg.rdma_req_num = 3;//(enable? 3: 0);
	di_dma_multi_req_num_reg.wdma_req_num = (disableDiWr? 0: 2);
	IoReg_Write32(DI_DI_DMA_Multi_Req_Num_reg, di_dma_multi_req_num_reg.regValue);

	return;
}

/*============================================================================*/
/**
 * drv_memory_set_ip_fifo
 * Set IP (video process) memory FIFO registers according to its FIFO-type
 *
 * @param <info> { display-info struecture }
 * @param <bits> { 10bit => 1, 8bit => 0}
 * @param <access> { 5A => 1, 3A => 0, rtnr => 2}
 * @return { none }
 * @note
 * Field size(32 Bits) = [20 * h_size * v_size] / 64 * 2  for 3A
 * Field size(32 Bits) = [16 * h_size * v_size] / 64 * 2  for 5A
 *
 */
//#define ENABLE_DI_COMPRESS
void drv_memory_set_ip_fifo(unsigned char access)
{
#ifdef Mer2_MEMORY_ALLOC
	drv_memory_set_ip_fifo_Mer2(access);
	return;
#else
	typedef struct _MEMSET {
		unsigned int number;
		unsigned int length;
		unsigned int remain;
		unsigned int water_level;
	} MEMSET;
	//di_im_di_btr_control_RBUS BTR_Control_Reg;
	di_ddr_rdatdma_wtlennum_RBUS ddr_rdatdma_wtlennum;
	di_ddr_rinfdma_wtlennum_RBUS ddr_rinfdma_wtlennum;
	di_ddr_wdatdma_wtlennum_RBUS ddr_wdatdma_wtlennum;
	di_ddr_winfdma_wtlennum_RBUS ddr_winfdma_wtlennum;
	di_ddr_datdma_rm_RBUS ddr_datdma_rm;
	di_ddr_infdma_rm_RBUS ddr_infdma_rm;
	//di_di_dma_multi_req_num_RBUS di_dma_multi_req_num_reg;
	//ddr_vrfifo1wtlvllennum_RBUS ddr_vrfifowtlvllennum;
	//ddr_vwfifo1wtlvllennum_RBUS ddr_vwfifowtlvllennum;
	//ddr_vfifo1rdwrremainder_RBUS ddr_vfifordwrremainder;

	unsigned int	DI_Width = 0, DI_Height = 0, size_per_frame = 0;
	unsigned char	data_bitNum = 0, info_bitNum = 0, i = 0, DIAllocateBlock;
	unsigned int	data_value, info_value=0, MemStartAddr;
	MEMSET			data_r, data_w, info_r, info_w;

	UINT32 lineSize_odd_128, lineSize_even_128;
	unsigned char DI_SMD_enable = 0;

	unsigned char source=255;
	unsigned char newRTNR_onoff = 0;
	unsigned char m_rtnrcompress = 0;
	unsigned char DI_BTR_enable = 0;
	unsigned char DI_BTR_type = 0;
	unsigned int buffer_alloc = 0;
	unsigned int data_size = 0;
	unsigned int info_size = 0;

//fixme:vip
#if 1
	extern Scaler_DI_Coef_t Scaler_DI_Coef_table[VIP_QUALITY_SOURCE_NUM];
	source = fwif_vip_source_check(3, 0);
	if(source >= VIP_QUALITY_SOURCE_NUM)
	{
		rtd_pr_scaler_memory_err("\r\n#####drv_memory_set_ip_fifo source nunber %d error####\r\n", source);
		source = VIP_QUALITY_SOURCE_NUM-1;
	}
	//frank@1018 add below code to solve scan tool warning
	newRTNR_onoff = (unsigned char)Scaler_DI_Coef_table[source].newRTNR_style;
	//DI_SMD_enable = Scaler_getDI_SMD_en(source);
	DI_SMD_enable = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_INTERLACE);
		DI_BTR_enable = drv_memory_get_ip_Btr_mode();

	//BTR_Control_Reg.regValue = rtd_inl(DI_IM_DI_BTR_CONTROL_VADDR);
	//DI_BTR_type = BTR_Control_Reg.btr_solutiontype;
	DI_BTR_type = (Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID) < 960)?0:1;
#endif

		DI_Width = Scaler_DispGetInputInfo(SLR_INPUT_DI_WID);
		DI_Height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);

	rtd_pr_scaler_memory_debug("\n DI Memory set FIFO !");
	rtd_pr_scaler_memory_debug("\n Width:%d, Height:%d, DMA mode=%d", DI_Width, DI_Height, drv_memory_get_ip_DMA420_mode()?420:422);
	rtd_pr_scaler_memory_debug("\n Data status Y=%d C=%d", fw_scalerip_get_DI_chroma_10bits()?10:8, fw_scalerip_get_DI_chroma_10bits()?10:8);
	rtd_pr_scaler_memory_debug("\n Info status BTR=%d(type:%d) SMD=%d RTNR style=%d", DI_BTR_enable, DI_BTR_type, DI_SMD_enable, newRTNR_onoff);



	rtd_pr_scaler_memory_emerg("\n DI Memory set FIFO !");
	rtd_pr_scaler_memory_emerg("\n Width:%d, Height:%d, DMA mode=%d", DI_Width, DI_Height, drv_memory_get_ip_DMA420_mode()?420:422);
	rtd_pr_scaler_memory_emerg("\n Data status Y=%d C=%d", fw_scalerip_get_DI_chroma_10bits()?10:8, fw_scalerip_get_DI_chroma_10bits()?10:8);
	rtd_pr_scaler_memory_emerg("\n Info status BTR=%d(type:%d) SMD=%d RTNR style=%d", DI_BTR_enable, DI_BTR_type, DI_SMD_enable, newRTNR_onoff);





#ifdef Mag2_New_Flow
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
		rtd_pr_scaler_memory_debug("\n 5A mode \n");
		rtd_pr_scaler_memory_emerg("\n 5A mode \n");

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
		rtd_pr_scaler_memory_debug("\n 3A mode \n");
		rtd_pr_scaler_memory_emerg("\n 3A mode \n");

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
		rtd_pr_scaler_memory_debug("\n progress mode \n");

		rtd_pr_scaler_memory_emerg("\n progress mode \n");
		DIAllocateBlock = 2;

		//Waterlevel = 0x180 - length at PRTNR 2A case, but this value over 8bit(0xff), so give 0xff
		data_r.water_level = 0xff;
		data_r.length = 60;

		if (fwif_vip_increase_DI_water_lv())		//20150813 roger, change dat_len_r for 4k2k60p
			data_r.length = 96;

		data_w.water_level = 60;
		data_w.length = 60;

		info_r.water_level = 32;
		info_r.length = 32;

		info_w.water_level = 32;
		info_w.length = 32;
	} else {
		rtd_pr_scaler_memory_debug("\n Error accept parameter in memory fifo \n");
		return;
	}

	if (DI_SMD_enable)
		info_bitNum += 6; //4; //merlin2 need more 2bit->4+2=6

	// if k temporal function is enabled, new rtnr need 2 bits, otherwise still 0, please refer scalerColor.cpp:fwif_color_set_RTNR_style_adjust()
	if (newRTNR_onoff == 3 || newRTNR_onoff == 4)
		info_bitNum += 2;

	rtd_pr_scaler_memory_debug("\n Data BitNum=%d, Info BitNum=%d", data_bitNum, info_bitNum);
	rtd_pr_scaler_memory_emerg("\n Data BitNum=%d, Info BitNum=%d", data_bitNum, info_bitNum);

	// Calculate Data number/length/remain
	if (drv_memory_get_ip_DMA420_mode() == SLR_DMA_422_mode) { // 422 mode
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

		IoReg_ClearBits(DI_IM_DI_DMA_reg, _BIT31);
		IoReg_ClearBits(DI_IM_DI_DMA_reg, _BIT30);
	}
	else if (drv_memory_get_ip_DMA420_mode() == SLR_DMA_420_mode) { // 420 mode
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
		//	size_per_frame = ((lineSize_even_128 + lineSize_odd_128+1)/2)*(DI_Height/2);
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

		IoReg_Mask32(DI_IM_DI_DMA_reg,
			~(DI_DMA_420_EN_MASK| DI_DMA_LINESIZE_EVEN_MASK| DI_DMA_LINESIZE_ODD_MASK),
			_BIT30 | (lineSize_even_128 << DI_DMA_LINESIZE_EVEN_START_BIT) | lineSize_odd_128);

		rtd_pr_scaler_memory_debug("\n 420 mode: linesize_even=%d, linesize_odd=%d", lineSize_even_128, lineSize_odd_128);
	}
	else { // 400 mode
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
		IoReg_SetBits(DI_IM_DI_DMA_reg, _BIT31);
		IoReg_ClearBits(DI_IM_DI_DMA_reg, _BIT30);
	}
	IoReg_SetBits(DI_db_reg_ctl_reg, _BIT0);

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

	rtd_pr_scaler_memory_debug("\n Data number = %d, length = %d, remain = %d, water_level = %d in setting DI Read FIFO", data_r.number, data_r.length, data_r.remain, data_r.water_level);
	rtd_pr_scaler_memory_debug("\n Data number = %d, length = %d, remain = %d, water_level = %d in setting DI Write FIFO", data_w.number, data_w.length, data_w.remain, data_w.water_level);
	rtd_pr_scaler_memory_debug("\n Info number = %d, length = %d, remain = %d, water_level = %d in setting DI Read FIFO", info_r.number, info_r.length, info_r.remain, info_r.water_level);
	rtd_pr_scaler_memory_debug("\n Info number = %d, length = %d, remain = %d, water_level = %d in setting DI Write FIFO", info_w.number, info_w.length, info_w.remain, info_w.water_level);

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

	IoReg_Write32(DI_DDR_RDATDMA_WTLENNUM_reg, ddr_rdatdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_WDATDMA_WTLENNUM_reg, ddr_wdatdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_RINFDMA_WTLENNUM_reg, ddr_rinfdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_WINFDMA_WTLENNUM_reg, ddr_winfdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_DATDMA_RM_reg, ddr_datdma_rm.regValue);
	IoReg_Write32(DI_DDR_INFDMA_RM_reg, ddr_infdma_rm.regValue);

	//frank@0514 change DI multi request to solve DI read underflow ++
	drv_memory_disable_di_write_req(FALSE);
	//frank@0514 change DI multi request to solve DI read underflow --
#else

//=======YUV subsample setting  ===========
	data_bitNum = 16; // for YUV subsample 422 ( now only using 2 typr : 422 & 420  )

	if (access == SLR_DI_5A_mode) {
		info_bitNum+= (drv_memory_get_ip_Btr_mode() == _ENABLE? 8: 0); // type A:8 bits, type B:4 bits, default is type A
	} else if (access == SLR_DI_3A_mode) {
		info_bitNum+= (drv_memory_get_ip_Btr_mode() == _ENABLE? 4: 0); // type A:8 bits, type B:4 bits, default is type A
	}

	// if k temporal function is enabled, new rtnr need 2 bits, otherwise still 0, please refer scalerColor.cpp:fwif_color_set_RTNR_style_adjust()
	if (newRTNR_onoff == 3 || newRTNR_onoff == 4) {
		info_bitNum +=2;
	}


	if(access == SLR_DI_5A_mode) {	// DI 5A
		rtd_pr_scaler_memory_debug("\n 5A mode \n");
		//frank@0310 10bits mode
		if (fw_scalerip_get_DI_chroma_10bits()) {
			data_bitNum += 2;
		}
		//Elsie 20140117: DI chroma 10-bit needs additional memory space.
		if(fw_scalerip_get_DI_chroma_10bits())
			data_bitNum += 2;
		DIAllocateBlock = 4;
	} else if(access == SLR_DI_3A_mode){	// DI 3A
		rtd_pr_scaler_memory_debug("\n 3A mode \n");
		//if(!drv_memory_get_ip_Btr_mode())//frank@0607 if enable BTR,info_num have add 8 bits
			info_bitNum += 4; // bitNum default + 4 in 3A mode
		if (fw_scalerip_get_DI_chroma_10bits()) {
			data_bitNum += 2;
		}
		//Elsie 20140117
		if(fw_scalerip_get_DI_chroma_10bits())
			data_bitNum += 2;

		DIAllocateBlock = 2;
	}else if(access == SLR_DI_progress_mode){
		rtd_pr_scaler_memory_debug("\n progress mode \n");

		if (fw_scalerip_get_DI_chroma_10bits()) {
			data_bitNum += 2;
		}
		//Elsie 20140117
		if(fw_scalerip_get_DI_chroma_10bits())
			data_bitNum += 2;

	//	bitNum = bitNum;
		rtd_pr_scaler_memory_debug("\n Data.bitNum=%x,info.bitNum=%x\n",data_bitNum,info_bitNum);

		DIAllocateBlock = 2;	//roll back to 18480. set 1.gde buffer will be writen by something and cause gde fail. josh@20090423
	} else {
		rtd_pr_scaler_memory_debug("\n Error accept parameter in memory fifo \n");
		return;
	}

	//Elsie 20131210 sync from Mac2
	//if(DI_SMD_enable)
	//{
	//	rtd_pr_scaler_memory_debug("\n DI_SMD_enable \n");
	//	info_bitNum += 4;
	//}
	//Elsie 20140116: DO_SMD always on
	rtd_pr_scaler_memory_debug("\n DI_SMD_enable \n");
	info_bitNum += 4;

#ifdef ENABLE_DI_COMPRESS
	if((Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID) > 2048)
			&& (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN) > 1200)){	// 4k2k
		drv_memory_set_ip_compress_bit(12);
		m_rtnrcompress = 1;
		data_bitNum = 12;
	}
#endif

	// [DI] DI DMA 420 config
	drv_memory_DMA420_config(&data_bitNum);

	//rtd_pr_scaler_memory_debug("\n=============\n");
	rtd_pr_scaler_memory_debug("\n B.Width:%d, Height:%d, data bitNum=0x%x, info bitNum=0x%x, BTR/420=%d/%d\n",Scaler_DispGetInputInfo(SLR_INPUT_DI_WID), Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN), data_bitNum, info_bitNum, drv_memory_get_ip_Btr_mode(), drv_memory_get_ip_DMA420_mode());
	rtd_pr_scaler_memory_debug("\n Y/C bits:%d/%d\n",fw_scalerip_get_DI_chroma_10bits(), fw_scalerip_get_DI_chroma_10bits());
	//sync code from Mac2. Start
	if(access == SLR_DI_5A_mode) {
		//frank@0725 change below water level to solve peak BW not enough from Fishtail
	//	ddr_rdatdma_wtlennum.dat_water_r = 0x20;// DI total FIFO = 256, 0x100, Read Watel = Total - length
		ddr_rdatdma_wtlennum.dat_water_r = 68;// DI total FIFO = 256, 0x100, Read Watel = Total - length
		ddr_rdatdma_wtlennum.dat_len_r= 60;   //Change only support max127 0x7F
		ddr_rinfdma_wtlennum.inf_water_r = 0x20;// DI total FIFO = 256, 0x100, Read Watel = Total - length
		ddr_rinfdma_wtlennum.inf_len_r = 0x20;   //Change only support max127 0x7F
		//DI Read Water Level, Length, Number, Remain
		data_value = (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum) >>12; //AT:(128*32);pacific:(64*32)
	       data_tmp = (((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum)-(data_value<<12)) + (128 - 1))>>7;//AT:128;pacific:64,force remain + 1 , if (remain / fifo size) != 0 , avoid to occur dirty line in last line of video [chengying  2010,4/16]
	       info_value = (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum) >>12; //AT:(128*32);pacific:(64*32)
	       info_tmp = (((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum)-(info_value<<12)) + (128 - 1))>>7;//AT:128;pacific:64,force remain + 1 , if (remain / fifo size) != 0 , avoid to occur dirty line in last line of video [chengying  2010,4/16]
	}else{
		//frank@0725 change below water level to solve peak BW not enough from Fishtail
		// if(access == SLR_DI_3A_mode)
		// 	ddr_rdatdma_wtlennum.dat_water_r = 0xe0;//0x20;// DI total FIFO = 128, 0x100, Read Watel = Total - length
		// else
		// 	ddr_rdatdma_wtlennum.dat_water_r = 0xff;

		//frank@0531 change below water level to solve peak BW not enough
		// sync from Sirius
		ddr_rdatdma_wtlennum.dat_water_r = 196;//0x60;//0x20;// DI total FIFO = 128, 0x100, Read Watel = Total - length
		ddr_rdatdma_wtlennum.dat_len_r= 60;   //Change only support max127 0x7F

		//ddr_rdatdma_wtlennum.dat_water_r = 0x60;//0x20;// DI total FIFO = 128, 0x100, Read Watel = Total - length
		// ddr_rdatdma_wtlennum.dat_len_r = 0x20;   //Change only support max127 0x7F
		ddr_rinfdma_wtlennum.inf_water_r = 0x20;// DI total FIFO = 128, 0x100, Read Watel = Total - length
		ddr_rinfdma_wtlennum.inf_len_r= 0x20;   //Change only support max127 0x7F
		//DI Read Water Level, Length, Number, Remain
		data_value = (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum) >>12; //AT:(128*32);pacific:(64*32)
	       data_tmp = (((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum)-(data_value<<12)) + (128 - 1))>>7;//AT:128;pacific:64,force remain + 1 , if (remain / fifo size) != 0 , avoid to occur dirty line in last line of video [chengying  2010,4/16]
	       info_value = (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum) >>12; //AT:(128*32);pacific:(64*32)
	       info_tmp = (((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum)-(info_value<<12)) + (128 - 1))>>7;//AT:128;pacific:64,force remain + 1 , if (remain / fifo size) != 0 , avoid to occur dirty line in last line of video [chengying  2010,4/16]
	}
	//sync code from Mac2. End
	if(drv_memory_get_ip_DMA420_mode()){
		lineSize_even_128 = (Scaler_DispGetInputInfo(SLR_INPUT_DI_WID) * (data_bitNum - ((fw_scalerip_get_DI_chroma_10bits())?10:8)));
		if((lineSize_even_128%128)==0)
			lineSize_even_128 /=128;
		else
			lineSize_even_128 = (lineSize_even_128+128)/128;

		lineSize_odd_128 = (Scaler_DispGetInputInfo(SLR_INPUT_DI_WID) * data_bitNum);
		if((lineSize_odd_128%128)==0)
			lineSize_odd_128 /=128;
		else
			lineSize_odd_128 = (lineSize_odd_128+128)/128;

		if (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)%2 == 0) {
			di420_width = (lineSize_odd_128 + lineSize_even_128)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)/2);
		} else {
			di420_width = lineSize_even_128*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)/2)+lineSize_odd_128*((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)+1)/2);
		}
		di420_value = di420_width>>5;
		di420_remain = (di420_width)%32;

		data_value = di420_value;
		data_tmp = di420_remain;
	}

	if(data_tmp == 0 && data_value!=0) { //remain can't be zero
		data_value -= 1;
		data_tmp = 60;//0x20;
	}

	if(info_tmp == 0 && info_value!=0) { //remain can't be zero
		info_value -= 1;
		info_tmp = 0x20;
	}

	rtd_pr_scaler_memory_debug("\n Data value = %x, tmp = %x in setting DI Read FIFO \n", data_value, data_tmp);
	rtd_pr_scaler_memory_debug("\n Info value = %x, tmp = %x in setting DI Read FIFO \n", info_value, info_tmp);


	ddr_rdatdma_wtlennum.dat_num_r=data_value;
	ddr_datdma_rm.dat_remain_r=data_tmp;
	ddr_datdma_rm.rdma_remain_en = 1;
	//remain and number both 0 will cause DMA error when 1080p switch to 576i DI will fliker eric_huang@realsil.com.cn
	if (info_value == 0 && info_tmp == 0) {
		ddr_rinfdma_wtlennum.inf_num_r=0x174;
		ddr_infdma_rm.inf_remain_r=0x0b;
	} else {
		ddr_rinfdma_wtlennum.inf_num_r=info_value;
		ddr_infdma_rm.inf_remain_r=info_tmp;
	}

	IoReg_Write32(DI_DDR_RDATDMA_WTLENNUM_reg, ddr_rdatdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_RINFDMA_WTLENNUM_reg, ddr_rinfdma_wtlennum.regValue);
	#if 1
	//sync code from Mac2. Start
	//DI Write Water Level, Length, Number, Remain
	data_value = (  Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum)>>12; //AT:(128*32);pacific(64*64)

	//force remain + 1 , if (remain / fifo size) != 0 , avoid to occur dirty line in last line of video [chengying  2010,4/16]
       data_tmp = ((( Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum)-(data_value<<12)) + (128 - 1)) >>7;//AT:128;pacific:64
	//DI Write Water Level, Length, Number, Remain
	info_value = (  Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum)>>12; //AT:(128*32);pacific(64*64)

	//force remain + 1 , if (remain / fifo size) != 0 , avoid to occur dirty line in last line of video [chengying  2010,4/16]
       info_tmp = ((( Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum)-(info_value<<12)) + (128 - 1)) >>7;//AT:128;pacific:64

	if(drv_memory_get_ip_DMA420_mode()){
		data_value = di420_value;
		data_tmp = di420_remain;
	}

        if(data_tmp == 0 && data_value!=0) { //remain can't be zero
           	data_value -= 1;
		data_tmp = 0x20;
       }

	if(info_tmp == 0 && info_value!=0) { //remain can't be zero
           	info_value -= 1;
		info_tmp = 0x20;
       }
	//sync code from Mac2. End
	#endif

	rtd_pr_scaler_memory_debug("Write data value = %x, tmp = %x in setting DI Write FIFO\n", data_value, data_tmp);
	rtd_pr_scaler_memory_debug("Info data value = %x, tmp = %x in setting DI Write FIFO\n", info_value, info_tmp);
	ddr_wdatdma_wtlennum.dat_num_w=data_value;
	ddr_datdma_rm.dat_remain_w=data_tmp;
	ddr_wdatdma_wtlennum.dat_water_w = 0x3c;
	ddr_wdatdma_wtlennum.dat_len_w = 0x3c;
	//remain and number both 0 will cause DMA error when 1080p switch to 576i DI will fliker eric_huang@realsil.com.cn
	if (info_value == 0 && info_tmp == 0) {
		ddr_winfdma_wtlennum.inf_num_w=0x174;
		ddr_infdma_rm.inf_remain_w=0x0b;
	} else {
		ddr_winfdma_wtlennum.inf_num_w=info_value;
		ddr_infdma_rm.inf_remain_w=info_tmp;
	}
	ddr_winfdma_wtlennum.inf_water_w = 0x20;
	ddr_winfdma_wtlennum.inf_len_w = 0x20;

	IoReg_Write32(DI_DDR_WDATDMA_WTLENNUM_reg, ddr_wdatdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_DATDMA_RM_reg, ddr_datdma_rm.regValue);
	IoReg_Write32(DI_DDR_WINFDMA_WTLENNUM_reg, ddr_winfdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_INFDMA_RM_reg, ddr_infdma_rm.regValue);

//frank@0514 change DI multi request to solve DI read underflow ++
	drv_memory_disable_di_write_req(FALSE);
//frank@0514 change DI multi request to solve DI read underflow --

#endif

//frank@1202 Change code to solve 1366x768 image fail problem

#if 1/*qiangzhou: added DI memory allocate with VGIP PRE info for max size start*/
#if(CONFIG_DDR_DATA_BITS == 16)
	/*if ((Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_PIPMLSR) && (Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_PIPMLSR_4x3) &&
		(Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_MP1C12B) && (Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_MP1L7B)) {*/
		data_value = data_bitNum;
		info_value = info_bitNum;
		if(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) > _DISP_WID){
			data_value *= _DISP_WID;
			info_value *= _DISP_WID;
		}else{
			data_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
			info_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
		}


		if(((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status()) {
			data_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
			info_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
		} else {
			data_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
			info_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
		}

		data_value = (data_value >> 3);//byte unit
		info_value = (info_value >> 3);//byte unit
//		value =(info->IPV_ACT_LEN_PRE *info->IPH_ACT_WID_PRE * bitNum)>>4;
	/*} else {
		data_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum)>>3;	//byte unit
		info_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum)>>3;	//byte unit
	}*/
#else
	data_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)*Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE)*data_bitNum)>>3;	//byte unit
	info_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)*Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE)*info_bitNum)>>3;	//byte unit
#endif

	if (drv_memory_get_ip_DMA420_mode()) {
		//sync code from Mac2. Start
		di_im_di_dma_RBUS im_di_dma;
		unsigned int di420_width;
		im_di_dma.regValue = IoReg_Read32(DI_IM_DI_DMA_reg);
		lineSize_even_128 = (Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) * (data_bitNum - 8));
		if((lineSize_even_128%128)==0)
			lineSize_even_128 /=128;
		else
			lineSize_even_128 = (lineSize_even_128+128)/128;

		lineSize_odd_128 = (Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) * data_bitNum);
		if((lineSize_odd_128%128)==0)
			lineSize_odd_128 /=128;
		else
			lineSize_odd_128 = (lineSize_odd_128+128)/128;

		if (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)%2 == 0) {
			di420_width = (lineSize_odd_128 + lineSize_even_128)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)/2);
		} else {
			di420_width = lineSize_even_128*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)/2)+lineSize_odd_128*((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)+1)/2);
		}
		data_value = di420_width<<4;
		//sync code from Mac2. End
	}
#endif/*qiangzhou: added DI memory allocate with VGIP PRE info for max size end*/

	drvif_memory_free_block(MEMIDX_DI);

	// Set 32 avoid to cover other block memroy ([[h * v * bitnum] / (64* 2) ]* 16
	data_value = drvif_memory_get_data_align(data_value, 32);
	info_value = drvif_memory_get_data_align(info_value, 32);
	//frank@0707 when 3D mode DI must allocate 2 X memory

#if 1//defined(ENABLE_DRIVER_I3DDMA)|| defined(ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE)
	//3D mode DI memory allocate x2: 3D frame sequence input video
	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())) {
		data_value *= 2;
		info_value *= 2;
	}
#endif

#if 1
	drv_memory_di_alloc_buffer_set(Scaler_InputSrcGetMainChType(),fw_scalerip_get_DI_chroma_10bits());
	buffer_alloc=drv_memory_di_alloc_buffer_get();
	data_size=drv_memory_di_data_size_get(access,Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE));
	info_size=drv_memory_di_info_size_get(access);
	//define at makeconfig
	//patch for DI/RTNR bug(overwrite 512Byte)
	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())){

		if(access != 2){
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value * DIAllocateBlock+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
		}
		else{
#ifdef ENABLE_DI_COMPRESS
			if(m_rtnrcompress){//compress mode use 2 buffer
				MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value*2+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
			}
			else
#endif
			{
				MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
			}
		}
	} else {
		MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, buffer_alloc, MEMALIGN_ANY);
	}
	if(MemStartAddr == _DUMMYADDR){
		rtd_pr_scaler_memory_emerg("set MEMIDX_DI  to size fail !!!\n");
	}
#else
	if(access != 2)
		MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value * DIAllocateBlock+info_value*2, MEMALIGN_ANY);
	else{
		if(m_rtnrcompress)//compress mode use 2 buffer
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value*2+info_value*2, MEMALIGN_ANY);
		else
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2, MEMALIGN_ANY);
	}
#endif
	//rtd_pr_scaler_memory_debug("Get ip addr = %x, size=%x\n", MemStartAddr, (data_value * DIAllocateBlock+info_value*2));

#if 1//defined(ENABLE_DRIVER_I3DDMA)|| defined(ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE)
	//3D mode DI memory allocate x2: 3D frame sequence input video
	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())) {
		data_value /= 2;
		info_value /= 2;
	}
#endif

	if(drvif_scaler3d_get_IDMA3dMode()
			|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
				&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
				&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status()))
	{
		// Set start address for deinterlace
		for(i = 0; i < 4; i++) {
			rtd_pr_scaler_memory_debug("IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((DI_DI_DATMemoryStartAdd1_reg+ (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			if((access != SLR_DI_progress_mode)|| m_rtnrcompress)
				MemStartAddr += data_value;//data_value;
		}

		if((access == SLR_DI_progress_mode)&&!m_rtnrcompress){
			MemStartAddr += data_value;
		}

		for(i = 0; i < 2; i++) {
			rtd_pr_scaler_memory_debug("Info IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((DI_DI_INFMemoryStartAdd1_reg+ (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			if(access != SLR_DI_progress_mode)
				MemStartAddr += info_value;
		}
	}else {
		for(i = 0; i < 4; i++) {
			rtd_pr_scaler_memory_debug("IP address%x = %x\n", i, MemStartAddr);
			IoReg_Write32((DI_DI_DATMemoryStartAdd1_reg+ (i << 2)),  (MemStartAddr&0x7FFFFFFF) );
			if((access != SLR_DI_progress_mode)|| m_rtnrcompress)
				MemStartAddr += data_size;//data_value;
		}
		if((access == SLR_DI_progress_mode)&&!m_rtnrcompress){
			MemStartAddr += data_size;
		}

		for(i = 0; i < 2; i++) {
			rtd_pr_scaler_memory_debug("Info IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((DI_DI_INFMemoryStartAdd1_reg+ (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			if(access != SLR_DI_progress_mode)
				MemStartAddr += info_size;
		}
	}

#if 1//defined(ENABLE_DRIVER_I3DDMA)|| defined(ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE)
	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())) {
		for(i = 0; i < 4; i++) {
			rtd_pr_scaler_memory_debug("3D IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((DI_DI_DATMemoryStartAdd1_3D_reg+ (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			if((access != SLR_DI_progress_mode)|| m_rtnrcompress)
				MemStartAddr += data_value;
		}

		if((access == SLR_DI_progress_mode) && !m_rtnrcompress){
			MemStartAddr += data_value;
		}

		for(i = 0; i < 2; i++) {
			rtd_pr_scaler_memory_debug("3D Info IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((DI_DI_INFMemoryStartAdd1_3D_reg+ (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			if(access != SLR_DI_progress_mode)
				MemStartAddr += info_value;
		}
	}
#endif

	return;
#endif
}

/*============================================================================*/
/**
 * drvif_memory_set_dbuffer_write
 * Enable double buffer write-in.
 *
 * @param <display> { main/sub display }
 * @return { none }
 * @note
 * Memory main/sub display are double buffered, those registers willbe updated
 * at the edge of Vsync of display out.
 *
 */
void drvif_memory_set_dbuffer_write(unsigned char display)
{
	// write data & apply double buffer
	if(display == SLR_MAIN_DISPLAY)
		IoReg_SetBits(MDOMAIN_DISP_Disp0_db_ctrl_reg, MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
	else if(display == SLR_SUB_DISPLAY)
		IoReg_SetBits(MDOM_SUB_DISP_Disp1_db_ctrl_reg, MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_apply_mask);
}


#if 0
/**
 * memory_get_jpeg_display_size
 * Calculate the one line memory size of JPEG display.
 * The size is counted by 4:2:2/4:4:4 data format, and frame-access/line access.
 *
 * @param <info> { disp-info structure }
 * @return { line total size }
 */
static unsigned int memory_get_osd_display_size(SCALER_DISP_CHANNEL disp, StructJPEGDisplayInfo *jpeg_info)
{
	unsigned int TotalSize;
	unsigned int LineSize;
	//unsigned char ddrbit;

	if ( jpeg_info == NULL ) {
		rtd_pr_scaler_memory_debug("[memory_get_jpeg_display_size] jpeg_info cannot be NULL.\n");
		return 0;
	}
	//ddrbit = (Scaler_DispGetStatus((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_DISP_10BIT) << 1)  | ( !Scaler_DispGetStatus((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_DISP_422CAP));
	LineSize = jpeg_info->MemWid;
	LineSize = drvif_memory_get_data_align(LineSize*32, 64);
	TotalSize = (unsigned int)SHR(LineSize, 6);

	return TotalSize;
}
#endif

/*
void drvif_memory_set_osd_disp_mem(StructJPEGDisplayInfo *jpeg_info)
{
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	unsigned int TotalSize;
	unsigned int Quotient;
	unsigned char Remainder;
//	unsigned int LineStep;
	unsigned int fifoLen = _FIFOLENGTH2;
	unsigned int burstLen = _BURSTLENGTH2;

	if ( jpeg_info == NULL ) {
		rtd_pr_scaler_memory_debug("[drvif_memory_set_jpeg_disp_mem] jpeg_info cannot be NULL.\n");
		return;
	}

	// set width and length
	//frank@0323 change waterlevel/2 for AT,because water level nit is 128 bits
	IoReg_Mask32(MDOMAIN_DISP_DDR_SubPixNumWTL_reg, 0xfff00000, ((jpeg_info->MemWid & 0x0FFF) << 8) | (fifoLen-burstLen));
	IoReg_Mask32(MDOMAIN_DISP_DDR_SubLineNum_reg, 0xfffff000, (jpeg_info->MemLen & 0x0FFF));

	// set DDR_MainRdNumLenRem
	TotalSize = memory_get_osd_display_size(SLR_SUB_DISPLAY, jpeg_info); // calculate the memory size of capture
	memory_division(TotalSize, burstLen, &Quotient, &Remainder); // Do a division

	if ( Remainder == 0 ) { // remainder is not allowed to be zero
		Remainder = burstLen;
		Quotient -= 1;
	}

	IoReg_Write32(MDOMAIN_DISP_DDR_SubRdNumLenRem_reg, (Quotient << 16) | (burstLen << 8) | Remainder);
	TotalSize = Quotient * burstLen + Remainder; 	// count one line, 64bits unit
	TotalSize = drvif_memory_get_data_align(TotalSize, 4);	// times of 4
	IoReg_Write32(MDOMAIN_DISP_DDR_SubLineStep_reg,  (TotalSize + TotalSize%2) << 3);//bit 3 need to be 0. rbus rule

	// set memory address
//--	WaitFor_DEN_STOP();
	IoReg_Write32(MDOMAIN_DISP_DDR_SubAddr_reg, jpeg_info->MemAddr);
	IoReg_Write32(MDOMAIN_DISP_DDR_SubBlkStep_reg, 0);
	IoReg_WriteWord0(MDOMAIN_DISP_DDR_SubMPEn_reg,0);
	IoReg_WriteByte0(MDOMAIN_DISP_DDR_SubMPMode_reg, _BIT3);
	IoReg_SetBits(MDOMAIN_DISP_DDR_SubCtrl_reg, _BIT6);
	drvif_memory_set_dbuffer_write(SLR_SUB_DISPLAY);
#endif
}
*/
/*unsigned char drvif_memory_regAvPresetTable(unsigned short *ptr)
{
	if(!ptr)
		return FALSE;

	tAV_PRESET_TABLE = ptr;

	return TRUE;
}*/


#if 0
void get_adc_dump_addr_size(unsigned int *PhyAddr, unsigned int *VirStartAddr,unsigned int *Size , unsigned int *Width, unsigned int* Height)
{

	if (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY))
	{
		PhyAddr = (unsigned int*)MemTag[MEMIDX_SUB].StartAddr;
		*VirStartAddr = get_scaler_memory_map_addr() + (MemTag[MEMIDX_SUB].StartAddr - AllocStartAddr);
		#ifndef CONFIG_ADC_DMA
		*Size = MemTag[MEMIDX_SUB].Size;

		#endif
	}
	else
	{
		*PhyAddr = MemTag[MEMIDX_MAIN].StartAddr;
		*VirStartAddr = get_scaler_memory_map_addr() + (MemTag[MEMIDX_MAIN].StartAddr - AllocStartAddr);
		#ifndef CONFIG_ADC_DMA
		*Size = MemTag[MEMIDX_MAIN].Size;
		#endif
	}

	//rtd_pr_scaler_memory_debug("alloc start addr=%x %x\n",AllocStartAddr,MemTag[MEMIDX_MAIN].StartAddr);

	#ifndef CONFIG_ADC_DMA
	*Width = Scaler_DispGetInputInfo(SLR_INPUT_CAP_WID);
	*Height = Scaler_DispGetInputInfo(SLR_INPUT_CAP_LEN);
	#else
	*Width = Scaler_DispGetInputInfo(SLR_INPUT_H_LEN);
	*Height = Scaler_DispGetInputInfo(SLR_INPUT_V_LEN) * DUMP_VSYNC; // 3 vsync
	*Size= (*Height) * (*Width) * 30 / 8;
	*Size = ((*Size) & 0xfffffffc); // align to 4
	#endif

	if (*Size >= (16 * 1024 * 1024)) // scaler memory 16M
		rtd_pr_scaler_memory_debug("Error !! memory size is not enough !!!\n");

}



void get_frc_buf_addr_size(unsigned int *PhyAddr, unsigned int *VirStartAddr,unsigned int *Size , unsigned int *Width, unsigned int* Height)
{

	int is_10bit = 0,is_422 = 0;
	if (Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY))
	{
		PhyAddr = (unsigned int*)MemTag[MEMIDX_SUB].StartAddr;
		*VirStartAddr = get_scaler_memory_map_addr() + (MemTag[MEMIDX_SUB].StartAddr - AllocStartAddr);
		*Size = MemTag[MEMIDX_SUB].Size;

	}
	else
	{
		*PhyAddr = MemTag[MEMIDX_MAIN].StartAddr;
		*VirStartAddr = get_scaler_memory_map_addr() + (MemTag[MEMIDX_MAIN].StartAddr - AllocStartAddr);
		*Size = MemTag[MEMIDX_MAIN].Size;

	}

	//rtd_pr_scaler_memory_debug("alloc start addr=%x %x\n",AllocStartAddr,MemTag[MEMIDX_MAIN].StartAddr);


	is_10bit = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_10BIT);
	is_422 = Scaler_DispGetStatus((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY),SLR_DISP_422CAP);

	if (is_422)
	{
		if(is_10bit == 0)  // 422 8bit
		{
			rtd_pr_scaler_memory_debug("@@@422 8bit...\n");
			*Width = drvif_memory_get_data_align(Scaler_DispGetInputInfo(SLR_INPUT_CAP_WID),16);
		}
		else	 //422 10bit
		{
			rtd_pr_scaler_memory_debug("@@@not support 422 10bit...\n");
			*Width = Scaler_DispGetInputInfo(SLR_INPUT_CAP_WID);
		}
	}
	else
	{
		if(is_10bit == 0) // 444 8bit
		{
			rtd_pr_scaler_memory_debug("@@@444 8bit...\n");
			*Width = drvif_memory_get_data_align(Scaler_DispGetInputInfo(SLR_INPUT_CAP_WID),192);
		}
		else
		{
			rtd_pr_scaler_memory_debug("@@@not support 444 10bit...\n");
			*Width = Scaler_DispGetInputInfo(SLR_INPUT_CAP_WID);
		}
	}

	*Height = Scaler_DispGetInputInfo(SLR_INPUT_CAP_LEN);

}


void dump_frc_rawdata_start(void)
{
	if (Scaler_DispGetInputInfo(SLR_INPUT_FRAMESYNC))
	{
		rtd_pr_scaler_memory_debug("ERROR !!  framesync mode, can't dump data\n");
		return;
	}

	WaitFor_IEN_STOP1();
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	IoReg_SetBits(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : MDOMAIN_CAP_DDR_In1Ctrl_reg, _BIT1);	// disable Capture
#else
	IoReg_SetBits(/*Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : */MDOMAIN_CAP_DDR_In1Ctrl_reg, _BIT1);	// disable Capture
#endif
		//frank@0122 solve  sub ATV switch channel flick problem
//-- check		Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? IoReg_ClearBits(BUS_SB3_ARB_CR_VADDR, _BIT3) : IoReg_ClearBits(BUS_SB3_ARB_CR_VADDR, _BIT6);
		//--			ROS_EXIT_CRITICAL();
		//fwif_scaler_wait_for_event(0xb8028100, _BIT10);//<===WaitFor_DEN_STOP();
	WaitFor_DEN_STOP();
	//test for  switch TV channel have white image --
	drvif_scaler_vactive_end_irq(_DISABLE, Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)); // CSW+ 0970710 Close auto MA in freeze mode
	drvif_scaler_vactive_sta_irq(_DISABLE, Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)); // CSW+ 0970710 Close auto MA in freeze mode

}

void dump_frc_rawdata_release(void)
{
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	IoReg_ClearBits(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : MDOMAIN_CAP_DDR_In1Ctrl_reg, _BIT1);
#else
	IoReg_ClearBits(/*Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : */MDOMAIN_CAP_DDR_In1Ctrl_reg, _BIT1);
#endif
	drvif_scaler_vactive_end_irq(_ENABLE, Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)); // CSW+ 0970710 Open auto MA in unfreeze mode
	drvif_scaler_vactive_sta_irq(_ENABLE, Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)); // CSW+ 0970710 Open auto MA in unfreeze mode

}

void get_vdc_dump_addr(unsigned int *PhyAddr, unsigned int *VirStartAddr)
{

	*PhyAddr  = MemTag[MEMIDX_VDC].StartAddr;

	*VirStartAddr = get_scaler_memory_map_addr() + (MemTag[MEMIDX_VDC].StartAddr - AllocStartAddr);

	rtd_pr_scaler_memory_debug("dump addr = %x %x\n", *PhyAddr,*VirStartAddr);

}
#endif


#ifdef CONFIG_DUMP_MEMTAG
void drvif_memory_dump_MemTag(void)
{
	rtd_pr_scaler_memory_debug("*****Dump MemTag*****\n");
	unsigned char i;
	for ( i=0 ; i<MEMIDX_ALL ; ++i ) {
		rtd_pr_scaler_memory_debug("idx: %d, status:%d, start addr:%d, size:%d\n",
			i, MemTag[i].Status, MemTag[i].StartAddr, MemTag[i].Size);
	}
}
#endif


#ifdef ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE // SG 3D PANEL SUPPORT ALL SOURCE 3D MODE
void drvif_memory_set_3D_LR_swap_for_SG_frc_in_mDomain(unsigned char bInvMode)
{
	unsigned int Addr1 = Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_MAIN_ADDR);
	unsigned int Addr2 = Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_MAIN_SEC_ADDR);
	unsigned int Addr3= Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_MAIN_THIRD_ADDR);
	unsigned int rFrameOffset = (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_BLKSTEP) << 3);
	unsigned char ucLRseq;
	ucLRseq = Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_LR_SEQUENCE) ^ bInvMode;

	if(modestate_get_vo3d_in_framePacking_FRC_mode()){
		unsigned int LineStep, TotalSize;
		LineStep = memory_get_capture_size(Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), MEMCAPTYPE_LINE);		// calculate capture line size
		TotalSize = LineStep * Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_MEM_ACT_LEN);
		rFrameOffset = TotalSize << 3;
	}

	// non-IDMA SG 3D mode L/R order need do invert
	if(ucLRseq == FALSE){
		IoReg_Write32(MDOMAIN_DISP_DDR_MainAddr_reg, (Addr1 + rFrameOffset)& 0xfffffff0);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main2ndAddr_reg, (Addr2 + rFrameOffset)& 0xfffffff0);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main3rdAddr_reg, (Addr3 + rFrameOffset)& 0xfffffff0);

		IoReg_Write32(MDOMAIN_DISP_DDR_Main_R_1ST_Addr_reg, Addr1);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main_R_2ND_Addr_reg, Addr2);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main_R_3RD_Addr_reg, Addr3);
	}
	else{
		IoReg_Write32(MDOMAIN_DISP_DDR_MainAddr_reg, Addr1& 0xfffffff0);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main2ndAddr_reg, Addr2& 0xfffffff0);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main3rdAddr_reg, Addr3& 0xfffffff0);

		IoReg_Write32(MDOMAIN_DISP_DDR_Main_R_1ST_Addr_reg, Addr1 + rFrameOffset);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main_R_2ND_Addr_reg, Addr2 + rFrameOffset);
		IoReg_Write32(MDOMAIN_DISP_DDR_Main_R_3RD_Addr_reg, Addr3 + rFrameOffset);
	}

	drvif_memory_set_dbuffer_write(SLR_MAIN_DISPLAY);
	return;
}
#endif

//USER:LewisLee DATE:2012/08/23
//to maje sure have set correct setting
//to prevent M domain setting error with video FW
void dvrif_memory_set_frc_style(UINT8 Display, UINT8 ucEnable)
{
	mdomain_cap_cap0_pxl_wrap_ctrl_0_RBUS mdomain_cap_cap0_pxl_wrap_ctrl_0_reg;

	if(SLR_MAIN_DISPLAY == Display)
	{
		mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);

		if(_ENABLE == ucEnable)
		{
			if(_DISABLE == mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en)
			{
				mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = _ENABLE;
				IoReg_Write32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

				drvif_memory_set_dbuffer_write(Display);
				rtd_pr_scaler_memory_debug("dvrif_memory_set_frc_style, set input fast than display\n");
			}
		}
		else// if(_DISABLE == ucEnable)
		{
			if(_ENABLE == mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en)
			{
				mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = _DISABLE;
				IoReg_Write32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

				drvif_memory_set_dbuffer_write(Display);
				rtd_pr_scaler_memory_debug("dvrif_memory_set_frc_style, set input slower than display\n");
			}
		}
	}
}

#if 0
//USER:LewisLee DATE:2012/08/16
//for some case, we need switch buffer by SW
//to fix frame tear
void drvif_memory_3D_mode_sw_buffer_swap_action(SCALER_DISP_CHANNEL ucDisplay, eMemCap_3D_Mode_SwBufferSwapState state)
{
	mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;
	mdomain_cap_ddr_in1ctrl_RBUS ddr_in1ctrl_reg;

	switch(state)
	{
		case MEM_3D_SWBUFFER_SWAP_CONDITION_CHECK:
			Scaler_Set_Enable_3D_Mode_3Buffer_SW_Switch_Condition(ucDisplay, Scaler_InputSrcGetMainChType());
		break;

		case MEM_3D_SWBUFFER_SWAP_ENABLE:
			if(_DISABLE == Scaler_Get_Enable_3D_Mode_3Buffer_SW_Switch_Condition())
				return;

#ifdef CONFIG_DUAL_CHANNEL
			if(SLR_MAIN_DISPLAY == ucDisplay)
			{
				ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
				ddr_mainctrl_reg.main_frc_style = _ENABLE;
				IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);
			}
			else// if(SLR_SUB_DISPLAY == ucDisplay)
			{
				IoReg_Mask32(MDOMAIN_DISP_DDR_SubCtrl_reg, ~_BIT3, _BIT3);
			}
#else //#ifdef CONFIG_DUAL_CHANNEL
			ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
			ddr_mainctrl_reg.main_frc_style = _ENABLE;
			IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, ddr_mainctrl_reg.regValue);
#endif //#ifdef CONFIG_DUAL_CHANNEL

			ddr_in1ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Ctrl_reg);
			ddr_in1ctrl_reg.in1_v_flip_3buf_en = _DISABLE;
			ddr_in1ctrl_reg.in1_double_enable = _DISABLE;
			IoReg_Write32(MDOMAIN_CAP_DDR_In1Ctrl_reg, ddr_in1ctrl_reg.regValue);
		break;

		case MEM_3D_SWBUFFER_SWAP_DISABLE:
//			if(SLR_SUB_DISPLAY == ucDisplay)
//				return;

			if(_DISABLE == Scaler_Get_Enable_3D_Mode_3Buffer_SW_Switch_Condition())
				return;

#ifdef CONFIG_DUAL_CHANNEL
			if(SLR_MAIN_DISPLAY == ucDisplay)
			{
				dvrif_memory_set_frc_style(SLR_MAIN_DISPLAY, _DISABLE);
			}
			else// if(SLR_SUB_DISPLAY == ucDisplay)
			{
				IoReg_Mask32(MDOMAIN_DISP_DDR_SubCtrl_reg, ~_BIT3, 0);
			}
#else //#ifdef CONFIG_DUAL_CHANNEL
			dvrif_memory_set_frc_style(SLR_MAIN_DISPLAY, _DISABLE);
#endif //#ifdef CONFIG_DUAL_CHANNEL
		break;

		case MEM_3D_SWBUFFER_SWAP_UPDATE_INFO:
			if(_DISABLE == Scaler_Get_Enable_3D_Mode_3Buffer_SW_Switch_Condition())
				return;

			state_update_disp3d_info();
		break;

		default:
		break;
	}
}
#endif
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

unsigned int drvif_memory_get_3d_startaddr(UINT8 index)
{
	return MemDispStartAddr_3D[index];
}

void drvif_memory_set_3d_startaddr(unsigned int addr,UINT8 index)
{

	MemDispStartAddr_3D[index]=addr;
}

StructMemBlock* drvif_memory_get_MemTag_addr(UINT8 index)
{
	return &MemTag[index];
}

void drvif_memory_set_memtag_virtual_startaddr(unsigned long addr)
{
	MemTag[MEMIDX_MAIN_THIRD].Allocate_VirAddr = addr;

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

#if 0//for M-domain block mode read verify
//DCU index
#include "../../../common_macarthur3/include/dc_sys_reg.h"
#define PAGE_WIDTH_SHIFT        8
#define PAGE_WIDTH             (1 << PAGE_WIDTH_SHIFT)
#define DCU_WIDTH_SCALE(width) (((width) + (PAGE_WIDTH - 1)) >> PAGE_WIDTH_SHIFT)

//#define PAGE_SHIFT             (PAGE_WIDTH_SHIFT + gPage_Size + 3)

void VBM_SetDCU (int index, int width_scale, int init_page, int x_offset, int y_offset, unsigned int *DCU_data)
{
	//rtd_pr_scaler_memory_debug("setDCU i %d ws %d p %d xy %d %d\n", index, width_scale, init_page, x_offset, y_offset);

  	unsigned int t0 = DC_PICT_SET_pict_width_scale (width_scale) |DC_PICT_SET_pict_init_page   (init_page) ;

  	unsigned int t1 =
    	DC_PICT_SET_OFFSET_pict_set_num      (index)
  	| DC_PICT_SET_OFFSET_pict_set_offset_x (x_offset >> 4)
  	| DC_PICT_SET_OFFSET_pict_set_offset_y (y_offset >> 2) ;

  	if ( (int)DCU_data == DC_PICT_SET_reg )
  	{
    		//VP_GetSwMutex(MUTEX_ID_DCU);
    		*(volatile unsigned int *)DC_PICT_SET_reg        = t0 ;
    		*(volatile unsigned int *)DC_PICT_SET_OFFSET_reg = t1 ;
    		//VP_PutSwMutex(MUTEX_ID_DCU);
  	}
  	else
  	{
    		DCU_data[0] = t0 ;
    		DCU_data[1] = t1 ;
  	}
}

int  VBM_GetDCUInitDataSeq (unsigned short width, unsigned int *pMemory, int index, unsigned int *DCU_data, int x_offset, int y_offset)
{
	//VBM_MEMORY_CHECK
  	unsigned int PAGE_SHIFT;
  	unsigned int gPage_Size;

  	/* check alignment */
  	//if ( (int)pMemory & ROUND_UP_DCU_SIZE )
    	//VBM_RETURN (VBM_ERR_MEMORY_UNALIGNED)
    	gPage_Size = DC_SYS_MISC_get_page_size(*(volatile unsigned int *)DC_SYS_MISC_reg);
	PAGE_SHIFT = (PAGE_WIDTH_SHIFT + gPage_Size + 3);

  	if (x_offset & 7) rtd_pr_scaler_memory_debug("X offset: %d should align to 8-bytes in VBM_GetDCUInitDataSeq().\n", x_offset);
  	if (y_offset & 3) rtd_pr_scaler_memory_debug("Y offset: %d should align to 4-bytes in VBM_GetDCUInitDataSeq().\n", y_offset);

  	VBM_SetDCU (index, DCU_WIDTH_SCALE(width), ((int)pMemory & 0x3FFFFFFF) >> PAGE_SHIFT, x_offset, y_offset, DCU_data);

  	//VBM_RETURN (VBM_SUCCESS)
}
#endif


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

unsigned int dvrif_memory_get_buffer_size(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return mdomain_main_buffer_size;
	else
		return mdomain_sub_buffer_size;
}

void dvrif_memory_set_buffer_size(unsigned char display, unsigned int buffer_size)
{
	if(display == SLR_MAIN_DISPLAY)
		mdomain_main_buffer_size = buffer_size;
	else
		mdomain_sub_buffer_size = buffer_size;
}

unsigned int dvrif_memory_get_buffer_number(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return mdomain_main_buffer_number;
	else
		return mdomain_sub_buffer_number;
}

void dvrif_memory_set_buffer_number(unsigned char display, unsigned int buffer_number)
{
	if(display == SLR_MAIN_DISPLAY)
		mdomain_main_buffer_number = buffer_number;
	else
		mdomain_sub_buffer_number = buffer_number;
}

unsigned char dvrif_memory_get_block_mode_enable(unsigned char display)
{
	if(display == SLR_MAIN_DISPLAY)
		return main_block_mode_enable;
	else
		return sub_block_mode_enable;
}

void dvrif_memory_set_block_mode_enable(unsigned char display, unsigned char enable)
{
	if(display == SLR_MAIN_DISPLAY)
		main_block_mode_enable = enable;
	else
		sub_block_mode_enable = enable;
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

//128bit unit
unsigned int calc_comp_line_sum_bit(unsigned int comp_wid, unsigned int comp_ratio)
{
	unsigned short cmp_line_sum_bit, sub_pixel_num;
	unsigned int cmp_line_sum_bit_pure_rgb, cmp_line_sum_bit_pure_a, cmp_line_sum_bit_32_dummy, cmp_line_sum_bit_128_dummy, cmp_line_sum_bit_real;
	unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;


	if((comp_wid % 32) != 0){
		//drop bits
		comp_wid = comp_wid + (32 - (comp_wid % 32));
	}

	frame_limit_bit = comp_ratio << 2;
	cmp_width_div32 = comp_wid / 32;

	sub_pixel_num = 3;
    cmp_line_sum_bit_pure_rgb = (cmp_width_div32 * 32) * (frame_limit_bit>>2);
	cmp_line_sum_bit_pure_a = 0;
	cmp_line_sum_bit_32_dummy = (6+1) * 32 * sub_pixel_num;
	cmp_line_sum_bit_128_dummy = 0;
	cmp_line_sum_bit_real = cmp_line_sum_bit_pure_rgb + cmp_line_sum_bit_pure_a + cmp_line_sum_bit_32_dummy + cmp_line_sum_bit_128_dummy;
	cmp_line_sum_bit = ((cmp_line_sum_bit_real+128)>>8)*2;

	rtd_pr_scaler_memory_info("cmp_line_sum_bit=0x%x function=%s, line=%d\n", cmp_line_sum_bit, __FUNCTION__, __LINE__);

	return cmp_line_sum_bit;
}

void dvrif_memory_comp_setting(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio){
	//M-domain use PQC and PQDC from merlin3 @Crixus 20170515
	mdomain_cap_dispm0_pq_cmp_RBUS mdomain_cap_dispm0_pq_cmp_reg;
	mdomain_cap_dispm0_pq_cmp_pair_RBUS mdomain_cap_dispm0_pq_cmp_pair_reg;
	mdomain_cap_dispm0_pq_cmp_bit_RBUS mdomain_cap_dispm0_pq_cmp_bit_reg;
	mdomain_cap_dispm0_pq_cmp_enable_RBUS mdomain_cap_dispm0_pq_cmp_enable_reg;
	mdomain_cap_dispm0_pq_cmp_allocate_RBUS mdomain_cap_dispm0_pq_cmp_allocate_reg;
	mdomain_cap_dispm0_pq_cmp_poor_RBUS mdomain_cap_dispm0_pq_cmp_poor_reg;
	mdomain_cap_dispm0_pq_cmp_blk0_add0_RBUS mdomain_cap_dispm0_pq_cmp_blk0_add0_reg;
	mdomain_cap_dispm0_pq_cmp_blk0_add1_RBUS mdomain_cap_dispm0_pq_cmp_blk0_add1_reg;
	mdomain_cap_dispm0_pq_cmp_balance_RBUS mdomain_cap_dispm0_pq_cmp_balance_reg;
	mdomain_cap_dispm0_pq_cmp_smooth_RBUS mdomain_cap_dispm0_pq_cmp_smooth_reg;
	mdomain_cap_dispm0_pq_cmp_guarantee_RBUS mdomain_cap_dispm0_pq_cmp_guarantee_reg;
	mdomain_cap_dispm0_pq_cmp_bit_llb_RBUS mdomain_cap_dispm0_pq_cmp_bit_llb_reg;
	mdomain_cap_dispm0_pq_cmp_st_RBUS mdomain_cap_dispm0_pq_cmp_st_reg;

	mdomain_disp_disp0_pq_decmp_RBUS mdomain_disp_disp0_pq_decmp_reg;
	mdomain_disp_disp0_pq_decmp_pair_RBUS mdomain_disp_disp0_pq_decmp_pair_reg;
	mdomain_disp_disp0_pq_decmp_sat_en_RBUS mdomain_disp_disp0_pq_decmp_sat_en_reg;
	//mdomain_disp_ddr_mainctrl_RBUS mdomain_disp_ddr_mainctrl_reg;
	//mdomain_cap_smooth_tog_ctrl_0_RBUS mdomain_cap_smooth_tog_ctrl_0_reg;
	//unsigned int TotalSize = 0;
	extern unsigned char pc_mode_run_422(void);
	extern unsigned char pc_mode_run_444(void);
	unsigned short cmp_line_sum_bit;
	unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;
	unsigned int llb_tolerance = 0, llb_x_blk_sta = 0, Llb_init = 0, Llb_step = 0;

	rtd_pr_scaler_memory_debug("[crixus]enable_compression = %d, comp_wid = %d, comp_len = %d, comp_ratio = %d\n",enable_compression, comp_wid, comp_len, comp_ratio);

	if(enable_compression == TRUE){

		// Enable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in1_pqc_clken = 0;  // 0: enable
		//IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);

		/*
			width need to align 32

			line sum bit = (width * compression bit + 256) / 128

			for new PQC and PQDC @Crixus 20170615
		*/

		if((comp_wid % 32) != 0){
			//drop bits
			//IoReg_Write32(MDOMAIN_DISP_DDR_MainAddrDropBits_reg, (32 - (comp_wid % 32)));
			comp_wid = comp_wid + (32 - (comp_wid % 32));
		}


		#if 0
		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
			TotalSize = (comp_wid * comp_ratio + 256) * 10 / 128;
			if((TotalSize % 10) != 0){
				TotalSize = TotalSize / 10 + 1;
			}else{
				TotalSize = TotalSize / 10;
			}
		}
		#endif
		//rtd_pr_scaler_memory_emerg("[crixus]comp_wid = %d, comp_wid/32 = %d\n", comp_wid, (comp_wid/32));
		//rtd_pr_scaler_memory_emerg("[crixus]comp_len = %d\n", comp_len);
		//rtd_pr_scaler_memory_emerg("[crixus]TotalSize = %d\n", TotalSize);


		/*=======================PQC Setting======================================*/
		cmp_width_div32 = comp_wid / 32;
		frame_limit_bit = comp_ratio << 2;
		//capture compression setting
		mdomain_cap_dispm0_pq_cmp_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_reg);
		mdomain_cap_dispm0_pq_cmp_reg.cmp_height = comp_len;//set length
		mdomain_cap_dispm0_pq_cmp_reg.cmp_width_div32 = comp_wid / 32;//set width
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_reg, mdomain_cap_dispm0_pq_cmp_reg.regValue);

		//compression bits setting
		mdomain_cap_dispm0_pq_cmp_bit_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_reg);
        mdomain_cap_dispm0_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
		mdomain_cap_dispm0_pq_cmp_bit_reg.block_limit_bit = 0x3f;//PQC fine tune setting
		mdomain_cap_dispm0_pq_cmp_bit_reg.first_line_more_bit = 0x20;
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_reg, mdomain_cap_dispm0_pq_cmp_bit_reg.regValue);

		//compression other setting
		mdomain_cap_dispm0_pq_cmp_pair_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_PAIR_reg);
		#if 0
		if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_10BIT) == TRUE)
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits
		else
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_bit_width = 0;//8 bits
		#endif

		mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits



		if(get_vpq_check_pc_rgb444_mode_flag() == _ENABLE)
		{
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 0; // RGB , pure RGB case
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0; // 444 , pure RGB case
		}
		else
		{
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 1; // YUV
			if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE))
			{
				if(pc_mode_run_422())
				{
					mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 1; // 422
				}
				else if(pc_mode_run_444())
				{
					mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0; // 444
				}
				else
				{
					mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 1; // 422
				}
			}
			else
			{
				mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0; // 422
			}
		}
		mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_pair_para = 1;

#if 0
		//if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == TRUE){
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0;// 444, let hw dynamic switch 444 or 422.
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 1;
		//}else{
		//	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_format = 0;//444
		//	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_data_color = 1;//currently no case rgb channel
		//}
#endif
		//Cal line sum bit
		cmp_line_sum_bit = calc_comp_line_sum_bit(comp_wid, comp_ratio);


		// Both line mode & frame mode need to set cmp_line_sum_bit;
		mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_sum_bit = cmp_line_sum_bit;

		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
			//use line mode
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_mode = 1;
			//mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_mode = 0;
			//mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_line_sum_bit = 0;
		}

	if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
        || get_mdomain_vflip_flag(SLR_MAIN_DISPLAY)
#endif
        )
	{
		mdomain_cap_dispm0_pq_cmp_pair_reg.two_line_prediction_en = 0;
	}
	else
	{
		mdomain_cap_dispm0_pq_cmp_pair_reg.two_line_prediction_en = 1;
	}
	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_qp_mode = 0;
	//mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_dic_mode_en = 1;//Mark2 removed
	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_jump4_en = 1;
	mdomain_cap_dispm0_pq_cmp_pair_reg.cmp_jump6_en = 1;
	IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_PAIR_reg, mdomain_cap_dispm0_pq_cmp_pair_reg.regValue);

	//compression llb setting
	llb_tolerance = 6;
	llb_x_blk_sta = 0;
	Llb_init = frame_limit_bit * 4 + llb_tolerance * 32;
	Llb_step = (Llb_init *16 - frame_limit_bit * 64) / cmp_width_div32;
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_LLB_reg);
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.llb_x_blk_sta = llb_x_blk_sta;
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.llb_init = Llb_init;
	mdomain_cap_dispm0_pq_cmp_bit_llb_reg.llb_step = Llb_step;
	IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_BIT_LLB_reg, mdomain_cap_dispm0_pq_cmp_bit_llb_reg.regValue);

	//compression st setting
	mdomain_cap_dispm0_pq_cmp_st_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_ST_reg);
	//mdomain_cap_dispm0_pq_cmp_st_reg.cmp_readro_sel = //default;
	IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_ST_reg, mdomain_cap_dispm0_pq_cmp_st_reg.regValue);

	//compression PQC constrain @Crixus 20170626
	mdomain_cap_dispm0_pq_cmp_enable_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_ENABLE_reg);
	if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
		mdomain_cap_dispm0_pq_cmp_enable_reg.first_line_more_en = 0;
	}
	else{
		mdomain_cap_dispm0_pq_cmp_enable_reg.first_line_more_en = 1;
	}
	//RGB444 setting for PQC @Crixus 20170718
	rtd_pr_scaler_memory_info("SLR_DISP_422CAP=%x, pc_mode_run_422=%x, pc_mode_run_444=%x\n",Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP), pc_mode_run_422(), pc_mode_run_444());
	if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE)){
		if(pc_mode_run_422())
		{
			mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 16;
			mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 1;//422 format
		}
		else if(pc_mode_run_444())
		{
			mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 12;
			mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 2;//444 format
		}
		else
		{
			mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 16;
			mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 1;//422 format
		}
	}
	else{
		mdomain_cap_dispm0_pq_cmp_enable_reg.g_ratio = 14;
		mdomain_cap_dispm0_pq_cmp_enable_reg.do_422_mode = 0;//422 format
	}

		mdomain_cap_dispm0_pq_cmp_enable_reg.first_predict_nc_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.blk0_add_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.blk0_add_half_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.add_last2blk_over_curve_bit = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.balance_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.variation_maxmin_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.dynamic_allocate_ratio_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.dynamic_sum_line_rst = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.not_rich_do_422_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.rb_lossy_do_422_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.not_enough_bit_do_422_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.poor_limit_min_qp_en = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.first_predict_nc_mode = 1;
		mdomain_cap_dispm0_pq_cmp_enable_reg.overcurve_lossy_en = 1;
		//mdomain_cap_dispm0_pq_cmp_enable_reg.master_qp_en = 1;//fix work around need to check by dic
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_ENABLE_reg, mdomain_cap_dispm0_pq_cmp_enable_reg.regValue);

		mdomain_cap_dispm0_pq_cmp_allocate_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_ALLOCATE_reg);
		//RGB444 setting for PQC @Crixus 20170718
		if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE)){
			if(pc_mode_run_422())
			{
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
			else if(pc_mode_run_444())
			{
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 13;
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 9;
			}
			else
			{
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
		}
		else{
			mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
			mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
		}

		mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_less = 4;
		mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_more = 2;
		mdomain_cap_dispm0_pq_cmp_allocate_reg.dynamic_allocate_line = 2;
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_ALLOCATE_reg, mdomain_cap_dispm0_pq_cmp_allocate_reg.regValue);

		mdomain_cap_dispm0_pq_cmp_poor_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_POOR_reg);
		#if 0
		if(pc_mode_run_444()){
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp3=0x06;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp2=0x03;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		} else {
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp3=0x0c;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp2=0x06;
			mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		}

	if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC) && (!get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY)))
	{
		mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=0x13; // for issue KTASKWBS-10111 for normal VDEC
	}
	#endif
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp4=12;
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp3=9;
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp2=4;
	mdomain_cap_dispm0_pq_cmp_poor_reg.poor_limit_th_qp1=3;

	IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_POOR_reg, mdomain_cap_dispm0_pq_cmp_poor_reg.regValue);

	//Update PQC constrain setting for PQC @Crixus 20170725
	mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD0_reg);
	mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD1_reg);
	if(comp_ratio == COMPRESSION_10_BITS){// minmal compression bit setting
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value32 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value16 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value8 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value4 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value1 = 0;
		//mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
	}
	else{
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value32 = 16;
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value16 = 8;
		mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.blk0_add_value0 = 0;
		//mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;

		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value8 = 4;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value4 = 2;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
		mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.blk0_add_value1 = 1;
	}
	IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD0_reg, mdomain_cap_dispm0_pq_cmp_blk0_add0_reg.regValue);
	IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_BLK0_ADD1_reg, mdomain_cap_dispm0_pq_cmp_blk0_add1_reg.regValue);


		mdomain_cap_dispm0_pq_cmp_balance_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_BALANCE_reg);
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_rb_ov_th = 0;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_rb_ud_th = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_rb_give = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_g_ud_th = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_g_ov_th = 0;
		mdomain_cap_dispm0_pq_cmp_balance_reg.balance_g_give = 3;
		mdomain_cap_dispm0_pq_cmp_balance_reg.variation_maxmin_th = 30;
		mdomain_cap_dispm0_pq_cmp_balance_reg.variation_maxmin_th2 = 3;
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_BALANCE_reg, mdomain_cap_dispm0_pq_cmp_balance_reg.regValue);


		mdomain_cap_dispm0_pq_cmp_smooth_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_SMOOTH_reg);
		mdomain_cap_dispm0_pq_cmp_smooth_reg.variation_near_num_th = 4;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.variation_value_th = 3;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.variation_num_th = 3;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.not_enough_bit_do_422_qp = 5;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.not_rich_do_422_th = 4;
		mdomain_cap_dispm0_pq_cmp_smooth_reg.rb_lossy_do_422_qp_level = 0;
		//mdomain_cap_dispm0_pq_cmp_smooth_reg.dic_mode_entry_th = 15;//Mark2 removed

		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_SMOOTH_reg, mdomain_cap_dispm0_pq_cmp_smooth_reg.regValue);

		mdomain_cap_dispm0_pq_cmp_guarantee_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_Guarantee_reg);
		//mdomain_cap_dispm0_pq_cmp_guarantee_reg.dic_mode_qp_th_g = 1;//Mark2 removed
		//mdomain_cap_dispm0_pq_cmp_guarantee_reg.dic_mode_qp_th_rb = 1;//Mark2 removed
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_Guarantee_reg, mdomain_cap_dispm0_pq_cmp_guarantee_reg.regValue);


	/*=======================PQDC Setting======================================*/

    if (get_hdmi_vrr_4k60_mode()) {
        comp_len=comp_len/2;
    }
	//display de-compression setting
	mdomain_disp_disp0_pq_decmp_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_PQ_DECMP_reg);
	mdomain_disp_disp0_pq_decmp_reg.decmp_height = comp_len;
	mdomain_disp_disp0_pq_decmp_reg.decmp_width_div32 = comp_wid / 32;
	IoReg_Write32(MDOMAIN_DISP_DISP0_PQ_DECMP_reg, mdomain_disp_disp0_pq_decmp_reg.regValue);

	//de-compression bits setting
	mdomain_disp_disp0_pq_decmp_pair_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_PQ_DECMP_PAIR_reg);
	#if 0
	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_10BIT) == TRUE)
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits
	else
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_bit_width = 0;//8 bits
	#endif

	mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits

	#if 0
	//decompression YUV444 @Crixus 20160902
	//if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == TRUE){
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0;//444
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 1;
	//}else{
	//	mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0;//444
	//	mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 1;
	//}
	#endif


	if(get_vpq_check_pc_rgb444_mode_flag() == _ENABLE)
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 0; // RGB , pure RGB case
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0; // 444 , pure RGB case
	}
	else
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_color = 1; // YUV
		if ((Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP) == FALSE))
		{
			if(pc_mode_run_422())
			{
				mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
			else if(pc_mode_run_444())
			{
				mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0; // 444
			}
			else
			{
				mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
		}
		else
		{
			mdomain_disp_disp0_pq_decmp_pair_reg.decmp_data_format = 0; // 422
		}
	}

		// Both line mode & frame mode need to set cmp_line_sum_bit;
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_sum_bit = cmp_line_sum_bit;

		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
			//default use line mode
			mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_mode = 1;
			//mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_mode = 0;
			//mdomain_disp_disp0_pq_decmp_pair_reg.decmp_line_sum_bit = 0;
		}

	if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
        || get_mdomain_vflip_flag(SLR_MAIN_DISPLAY)
#endif
        )
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
	} else
	{
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_two_line_prediction_en = 1;
	}
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_qp_mode = 0;
		//mdomain_disp_disp0_pq_decmp_pair_reg.decmp_dic_mode_en = 1; //Mark2 removed
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_jump4_en = 1;
		mdomain_disp_disp0_pq_decmp_pair_reg.decmp_jump6_en = 1;

		IoReg_Write32(MDOMAIN_DISP_DISP0_PQ_DECMP_PAIR_reg, mdomain_disp_disp0_pq_decmp_pair_reg.regValue);

		//enable saturation for PQ
		mdomain_disp_disp0_pq_decmp_sat_en_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_PQ_DECMP_SAT_EN_reg);
		mdomain_disp_disp0_pq_decmp_sat_en_reg.saturation_en = 1;
		//mdomain_disp_disp0_pq_decmp_sat_en_reg.decmp_ctrl_para_1 = //reserved;
		//mdomain_disp_disp0_pq_decmp_sat_en_reg.decmp_ctrl_para_0 = //reserved;
		mdomain_disp_disp0_pq_decmp_sat_en_reg.saturation_type = 0;
		IoReg_Write32(MDOMAIN_DISP_DISP0_PQ_DECMP_SAT_EN_reg, mdomain_disp_disp0_pq_decmp_sat_en_reg.regValue);

		//enable PQDC clock
		//mdomain_disp_ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		//mdomain_disp_ddr_mainctrl_reg.main_decompression_clk_en = 1;
		//IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, mdomain_disp_ddr_mainctrl_reg.regValue);

		//enable compression
		mdomain_cap_dispm0_pq_cmp_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_reg);
		mdomain_cap_dispm0_pq_cmp_reg.cmp_en = 1;
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_reg, mdomain_cap_dispm0_pq_cmp_reg.regValue);

		//enable de-compression
		mdomain_disp_disp0_pq_decmp_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_PQ_DECMP_reg);
		mdomain_disp_disp0_pq_decmp_reg.decmp_en = 1;
		IoReg_Write32(MDOMAIN_DISP_DISP0_PQ_DECMP_reg, mdomain_disp_disp0_pq_decmp_reg.regValue);
	}
	else{
		//disable compression
		mdomain_cap_dispm0_pq_cmp_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DISPM0_PQ_CMP_reg);
		mdomain_cap_dispm0_pq_cmp_reg.cmp_en = 0;
		IoReg_Write32(MDOMAIN_CAP_DISPM0_PQ_CMP_reg, mdomain_cap_dispm0_pq_cmp_reg.regValue);

		//disable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in1_pqc_clken = 1; // 1: disable
		//IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);

		//disable de-compression
		mdomain_disp_disp0_pq_decmp_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_PQ_DECMP_reg);
		mdomain_disp_disp0_pq_decmp_reg.decmp_en = 0;
		IoReg_Write32(MDOMAIN_DISP_DISP0_PQ_DECMP_reg, mdomain_disp_disp0_pq_decmp_reg.regValue);

		//disable PQDC clock
		//mdomain_disp_ddr_mainctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainCtrl_reg);
		//mdomain_disp_ddr_mainctrl_reg.main_decompression_clk_en = 0;
		//IoReg_Write32(MDOMAIN_DISP_DDR_MainCtrl_reg, mdomain_disp_ddr_mainctrl_reg.regValue);
	}
}


void dvrif_memory_comp_setting_sub(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio){

	mdom_sub_cap_dispm1_pq_cmp_RBUS mdom_sub_cap_dispm1_pq_cmp_reg;
	mdom_sub_cap_dispm1_pq_cmp_pair_RBUS mdom_sub_cap_dispm1_pq_cmp_pair_reg;
	mdom_sub_cap_dispm1_pq_cmp_bit_RBUS mdom_sub_cap_dispm1_pq_cmp_bit_reg;
	mdom_sub_cap_dispm1_pq_cmp_enable_RBUS mdom_sub_cap_dispm1_pq_cmp_enable_reg;
	mdom_sub_cap_dispm1_pq_cmp_allocate_RBUS mdom_sub_cap_dispm1_pq_cmp_allocate_reg;
	mdom_sub_cap_dispm1_pq_cmp_poor_RBUS mdom_sub_cap_dispm1_pq_cmp_poor_reg;
	mdom_sub_cap_dispm1_pq_cmp_blk0_add0_RBUS mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg;
	mdom_sub_cap_dispm1_pq_cmp_blk0_add1_RBUS mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg;
	mdom_sub_cap_dispm1_pq_cmp_balance_RBUS mdom_sub_cap_dispm1_pq_cmp_balance_reg;
	mdom_sub_cap_dispm1_pq_cmp_smooth_RBUS mdom_sub_cap_dispm1_pq_cmp_smooth_reg;
	mdom_sub_cap_dispm1_pq_cmp_guarantee_RBUS mdom_sub_cap_dispm1_pq_cmp_guarantee_reg;
	//mdomain_cap_smooth_tog_ctrl_0_RBUS mdomain_cap_smooth_tog_ctrl_0_reg;
	mdom_sub_cap_dispm1_pq_cmp_bit_llb_RBUS mdom_sub_cap_dispm1_pq_cmp_bit_llb_reg;

	mdom_sub_disp_disp1_pq_decmp_RBUS mdom_sub_disp_disp1_pq_decmp_reg;
	mdom_sub_disp_disp1_pq_decmp_pair_RBUS mdom_sub_disp_disp1_pq_decmp_pair_reg;
	mdom_sub_disp_disp1_pq_decmp_sat_en_RBUS mdom_sub_disp_disp1_pq_decmp_sat_en_reg;
	//mdomain_disp_ddr_subctrl_RBUS mdomain_disp_ddr_subctrl_reg;

	//unsigned int TotalSize = 0;
    unsigned char s_cmp_alpha_en = 0;
    unsigned short s_cmp_line_sum_bit, s_sub_pixel_num;
	unsigned int s_cmp_line_sum_bit_pure_rgb, s_cmp_line_sum_bit_pure_a, s_cmp_line_sum_bit_32_dummy, s_cmp_line_sum_bit_128_dummy, s_cmp_line_sum_bit_real;
	extern unsigned char pc_mode_run_422(void);
	extern unsigned char pc_mode_run_444(void);
	unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;
	unsigned int llb_tolerance = 0, llb_x_blk_sta = 0, Llb_init = 0, Llb_step = 0;

	rtd_pr_scaler_memory_emerg("[crixus]enable_compression = %d, comp_wid = %d, comp_len = %d, comp_ratio = %d\n",enable_compression, comp_wid, comp_len, comp_ratio);

	if(enable_compression == TRUE){

		// Enable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken = 0; // 0: enable
		//IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);

		/*
			width need to align 32

			line sum bit = (width * compression bit + 256) / 128

			for new PQC and PQDC @Crixus 20170615
		*/

		if((comp_wid % 32) != 0){
			//drop bits
			//IoReg_Write32(MDOMAIN_DISP_DDR_SubAddrDropBits_reg, (32 - (comp_wid % 32)));
			comp_wid = comp_wid + (32 - (comp_wid % 32));
		}


		//if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
		//	TotalSize = (comp_wid * comp_ratio + 256) * 10 / 128;
		//	if((TotalSize % 10) != 0){
		//		TotalSize = TotalSize / 10 + 1;
		//	}else{
		//		TotalSize = TotalSize / 10;
		//	}
		//}
		//rtd_pr_scaler_memory_emerg("[crixus]comp_wid = %d, comp_wid/32 = %d\n", comp_wid, (comp_wid/32));
		//rtd_pr_scaler_memory_emerg("[crixus]comp_len = %d\n", comp_len);
		//rtd_pr_scaler_memory_emerg("[crixus]TotalSize = %d\n", TotalSize);


		/*=======================PQC Setting======================================*/
		cmp_width_div32 = comp_wid / 32;
		frame_limit_bit = comp_ratio << 2;
		//capture compression setting
		mdom_sub_cap_dispm1_pq_cmp_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_reg);
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_height = comp_len;//set length
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_width_div32 = comp_wid / 32;//set width
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_reg, mdom_sub_cap_dispm1_pq_cmp_reg.regValue);

		//compression bits setting
		mdom_sub_cap_dispm1_pq_cmp_bit_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BIT_reg);
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			mdom_sub_cap_dispm1_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
			//mdom_sub_cap_dispm1_pq_cmp_bit_reg.line_limit_bit = comp_ratio;//Mark2 removed
		}
		else{//frame mode, line limit bit need to add margin to avoid the peak bandwidth
			mdom_sub_cap_dispm1_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
			//mdom_sub_cap_dispm1_pq_cmp_bit_reg.line_limit_bit = comp_ratio + M_DOMAIN_PQC_LINE_MARGIN;
		}
		mdom_sub_cap_dispm1_pq_cmp_bit_reg.block_limit_bit = 0x3f;//PQC fine tune setting
		mdom_sub_cap_dispm1_pq_cmp_bit_reg.first_line_more_bit = 0x20;
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BIT_reg, mdom_sub_cap_dispm1_pq_cmp_bit_reg.regValue);

		//compression other setting
		mdom_sub_cap_dispm1_pq_cmp_pair_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_PAIR_reg);
		#if 0
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT) == TRUE)
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits
		else
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_bit_width = 0;//8 bits
		#endif
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits

        mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_color = 1;		
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE){
			if(pc_mode_run_422())
			{
				mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 1; // 422
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 0; // 444
			}
			else
			{
				mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 1; // 422
			}
		}else{
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_data_format = 1; // 422
		}
		
        //Cal line sum bit
		s_sub_pixel_num = (s_cmp_alpha_en)? 4 : 3;
	    s_cmp_line_sum_bit_pure_rgb = (cmp_width_div32 * 32) * (frame_limit_bit>>2);
		s_cmp_line_sum_bit_pure_a = (s_cmp_alpha_en)?((cmp_width_div32 * 32)* (frame_limit_bit>>2)):(0);
		s_cmp_line_sum_bit_32_dummy = (6+1) * 32 * s_sub_pixel_num;
		s_cmp_line_sum_bit_128_dummy = 0;
		s_cmp_line_sum_bit_real = s_cmp_line_sum_bit_pure_rgb + s_cmp_line_sum_bit_pure_a + s_cmp_line_sum_bit_32_dummy + s_cmp_line_sum_bit_128_dummy;
		s_cmp_line_sum_bit = ((s_cmp_line_sum_bit_real+128)>>8)*2;//ceil((cmp_line_sum_bit_real)/128)*2;

		rtd_pr_scaler_memory_info("s_sub_pixel_num=%x, s_cmp_line_sum_bit_pure_rgb=%x, s_cmp_line_sum_bit_pure_a=%x, s_cmp_line_sum_bit_32_dummy=%x, s_cmp_line_sum_bit_128_dummy=%x\n", s_sub_pixel_num, s_cmp_line_sum_bit_pure_rgb, s_cmp_line_sum_bit_pure_a, s_cmp_line_sum_bit_32_dummy, s_cmp_line_sum_bit_128_dummy);
		rtd_pr_scaler_memory_info("s_cmp_line_sum_bit_real=%x\n", s_cmp_line_sum_bit_real);
		rtd_pr_scaler_memory_info("s_cmp_line_sum_bit=%x\n", s_cmp_line_sum_bit);

        // Both line mode & frame mode need to set cmp_line_sum_bit;
        mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_sum_bit = s_cmp_line_sum_bit;
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			//use line mode
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_mode = 1;
			//mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_mode = 0;
			//mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_line_sum_bit = 0;
		}

		if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
            || get_mdomain_vflip_flag(SLR_SUB_DISPLAY)
#endif
            )
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.two_line_prediction_en = 0;
		else
			mdom_sub_cap_dispm1_pq_cmp_pair_reg.two_line_prediction_en = 1;

		mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_qp_mode = 0;
		//mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_dic_mode_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_jump4_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_pair_reg.cmp_jump6_en = 1;
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_PAIR_reg, mdom_sub_cap_dispm1_pq_cmp_pair_reg.regValue);

		//compression llb setting
		llb_tolerance = 6;
		llb_x_blk_sta = 0;
		Llb_init = frame_limit_bit * 4 + llb_tolerance * 32;
		Llb_step = (Llb_init *16 - frame_limit_bit * 64) / cmp_width_div32;
		mdom_sub_cap_dispm1_pq_cmp_bit_llb_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BIT_LLB_reg);
		mdom_sub_cap_dispm1_pq_cmp_bit_llb_reg.llb_x_blk_sta = llb_x_blk_sta;
		mdom_sub_cap_dispm1_pq_cmp_bit_llb_reg.llb_init = Llb_init;
		mdom_sub_cap_dispm1_pq_cmp_bit_llb_reg.llb_step = Llb_step;
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BIT_LLB_reg, mdom_sub_cap_dispm1_pq_cmp_bit_llb_reg.regValue);

		//compression PQC constrain @Crixus 20170626
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_ENABLE_reg);
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.first_line_more_en = 0;
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.first_line_more_en = 1;
		}
		//RGB444 setting for PQC @Crixus 20170718
		if ((Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE)){
			if(pc_mode_run_422())
			{
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 14;
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 1;//422 format
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 12;
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 2;//444 format
			}
			else
			{
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 14;
				mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 1;//422 format
			}
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.g_ratio = 14;
			mdom_sub_cap_dispm1_pq_cmp_enable_reg.do_422_mode = 1;//dynamic format
		}

		mdom_sub_cap_dispm1_pq_cmp_enable_reg.first_predict_nc_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.blk0_add_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.blk0_add_half_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.add_last2blk_over_curve_bit = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.balance_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.variation_maxmin_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.dynamic_allocate_ratio_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.dynamic_sum_line_rst = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.not_rich_do_422_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.rb_lossy_do_422_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.not_enough_bit_do_422_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.poor_limit_min_qp_en = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.first_predict_nc_mode = 1;
		mdom_sub_cap_dispm1_pq_cmp_enable_reg.overcurve_lossy_en = 1;
		//mdom_sub_cap_dispm1_pq_cmp_enable_reg.master_qp_en = 1;//fix work around need to check by dic
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_ENABLE_reg, mdom_sub_cap_dispm1_pq_cmp_enable_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_ALLOCATE_reg);
		//RGB444 setting for PQC @Crixus 20170718
		if ((Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE)){
			if(pc_mode_run_422())
			{
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 13;
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 9;
			}
			else
			{
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
				mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
			}
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
			mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
		}
		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_less = 4;
		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_more = 2;
		mdom_sub_cap_dispm1_pq_cmp_allocate_reg.dynamic_allocate_line = 2;

		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_ALLOCATE_reg, mdom_sub_cap_dispm1_pq_cmp_allocate_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_poor_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_POOR_reg);
		#if 0
		if(pc_mode_run_444()){
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp3=0x06;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp2=0x03;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		} else {
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=0x14;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp3=0x0c;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp2=0x06;
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp1=0x03;
		}
		if((Get_DisplayMode_Src(SLR_SUB_DISPLAY) == VSC_INPUTSRC_VDEC) && (!get_vsc_run_adaptive_stream(SLR_SUB_DISPLAY)))
		{
			mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=0x13; // for issue KTASKWBS-10111 for normal VDEC
		}
		#endif

		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp4=12;
		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp3=9;
		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp2=4;
		mdom_sub_cap_dispm1_pq_cmp_poor_reg.poor_limit_th_qp1=3;

		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_POOR_reg, mdom_sub_cap_dispm1_pq_cmp_poor_reg.regValue);

		//Update PQC constrain setting for PQC @Crixus 20170725
		mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD0_reg);
		mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD1_reg);
		if(comp_ratio == COMPRESSION_10_BITS){// minmal compression bit setting
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value32 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value16 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value8 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value4 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value1 = 0;
			//mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
		}
		else{
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value32 = 16;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value16 = 8;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.blk0_add_value0 = 0;
			//mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value8 = 4;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value4 = 2;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
			mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.blk0_add_value1 = 1;
		}
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD0_reg, mdom_sub_cap_dispm1_pq_cmp_blk0_add0_reg.regValue);
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BLK0_ADD1_reg, mdom_sub_cap_dispm1_pq_cmp_blk0_add1_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_balance_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BALANCE_reg);
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_rb_ov_th = 0;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_rb_ud_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_rb_give = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_g_ud_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_g_ov_th = 0;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.balance_g_give = 3;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.variation_maxmin_th = 30;
		mdom_sub_cap_dispm1_pq_cmp_balance_reg.variation_maxmin_th2 = 3;
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_BALANCE_reg, mdom_sub_cap_dispm1_pq_cmp_balance_reg.regValue);


		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_SMOOTH_reg);
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.variation_near_num_th = 4;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.variation_value_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.variation_num_th = 3;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.not_enough_bit_do_422_qp = 5;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.not_rich_do_422_th = 4;
		mdom_sub_cap_dispm1_pq_cmp_smooth_reg.rb_lossy_do_422_qp_level = 0;
		//mdom_sub_cap_dispm1_pq_cmp_smooth_reg.dic_mode_entry_th = 15;

		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_SMOOTH_reg, mdom_sub_cap_dispm1_pq_cmp_smooth_reg.regValue);

		mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_Guarantee_reg);
		//mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.dic_mode_qp_th_g = 1;
		//mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.dic_mode_qp_th_rb = 1;
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_Guarantee_reg, mdom_sub_cap_dispm1_pq_cmp_guarantee_reg.regValue);

		/*=======================Sub PQDC Setting======================================*/

		//display de-compression setting
		mdom_sub_disp_disp1_pq_decmp_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_PQ_DECMP_reg);
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_height = comp_len;
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_width_div32 = comp_wid / 32;
		IoReg_Write32(MDOM_SUB_DISP_DISP1_PQ_DECMP_reg, mdom_sub_disp_disp1_pq_decmp_reg.regValue);

		//de-compression bits setting
		mdom_sub_disp_disp1_pq_decmp_pair_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_PQ_DECMP_PAIR_reg);
		#if 0
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT) == TRUE)
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits
		else
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_bit_width = 0;//8 bits
		#endif
		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits

		#if 0
		//decompression YUV444 @Crixus 20160902
		//if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == TRUE){
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0;//444
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_color = 1;
		/*}else{
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0;//444
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_color = 1;
		}*/
		#endif



		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_color = 1; // YUV
		if(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP) == FALSE)
		{
			if(pc_mode_run_422())
			{
				mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
			else if(pc_mode_run_444())
			{
				mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0; // 444
			}
			else
			{
				mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 1; // 422
			}
		}
		else
		{
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_data_format = 0; // 422
		}
		


        mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_sum_bit = s_cmp_line_sum_bit;
		if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
			//default use line mode
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_mode = 1;
			//mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_sum_bit = TotalSize;
		}
		else{
			//use frame mode
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_mode = 0;
			//mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_line_sum_bit = 0;
		}

		if(get_hdmi_vrr_4k60_mode()
#ifndef BUILD_QUICK_SHOW
            || get_mdomain_vflip_flag(SLR_SUB_DISPLAY)
#endif
            )
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;
		else
			mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_two_line_prediction_en = 1;

		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_qp_mode = 0;
		//mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_dic_mode_en = 1;
		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_jump4_en = 1;
		mdom_sub_disp_disp1_pq_decmp_pair_reg.decmp_jump6_en = 1;
		IoReg_Write32(MDOM_SUB_DISP_DISP1_PQ_DECMP_PAIR_reg, mdom_sub_disp_disp1_pq_decmp_pair_reg.regValue);

		//enable saturation for PQ
		mdom_sub_disp_disp1_pq_decmp_sat_en_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_PQ_DECMP_SAT_EN_reg);
		mdom_sub_disp_disp1_pq_decmp_sat_en_reg.saturation_en = 1;
		mdom_sub_disp_disp1_pq_decmp_sat_en_reg.saturation_type = 0;
		IoReg_Write32(MDOM_SUB_DISP_DISP1_PQ_DECMP_SAT_EN_reg, mdom_sub_disp_disp1_pq_decmp_sat_en_reg.regValue);

		//enable clock
		//mdomain_disp_ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
		//mdomain_disp_ddr_subctrl_reg.sub_decompression_clk_en = 1;
		//IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, mdomain_disp_ddr_subctrl_reg.regValue);

		//enable compression
		mdom_sub_cap_dispm1_pq_cmp_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_reg);
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_en = 1;
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_reg, mdom_sub_cap_dispm1_pq_cmp_reg.regValue);

		//enable de-compression
		mdom_sub_disp_disp1_pq_decmp_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_PQ_DECMP_reg);
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_en = 1;
		IoReg_Write32(MDOM_SUB_DISP_DISP1_PQ_DECMP_reg, mdom_sub_disp_disp1_pq_decmp_reg.regValue);

	}
	else{
		//disable compression & de-compression
		//disable compression
		mdom_sub_cap_dispm1_pq_cmp_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_DISPM1_PQ_CMP_reg);
		mdom_sub_cap_dispm1_pq_cmp_reg.cmp_en = 0;
		IoReg_Write32(MDOM_SUB_CAP_DISPM1_PQ_CMP_reg, mdom_sub_cap_dispm1_pq_cmp_reg.regValue);

		// Disable PQC clk
		//mdomain_cap_smooth_tog_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg);
		//mdomain_cap_smooth_tog_ctrl_0_reg.in2_pqc_clken = 1; // 1: disable
		//IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg, mdomain_cap_smooth_tog_ctrl_0_reg.regValue);


		//disable de-compression
		mdom_sub_disp_disp1_pq_decmp_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_PQ_DECMP_reg);
		mdom_sub_disp_disp1_pq_decmp_reg.decmp_en = 0;
		IoReg_Write32(MDOM_SUB_DISP_DISP1_PQ_DECMP_reg, mdom_sub_disp_disp1_pq_decmp_reg.regValue);

		//disable clock
//		mdomain_disp_ddr_subctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_SubCtrl_reg);
//		mdomain_disp_ddr_subctrl_reg.sub_decompression_clk_en = 0;
//		IoReg_Write32(MDOMAIN_DISP_DDR_SubCtrl_reg, mdomain_disp_ddr_subctrl_reg.regValue);
	}
}


unsigned int di_buffer_alloc_size,di_buffer_size;
#define DI_422_10BIT_BUFFER_SIZE (3840 * 2160 * 20 / 8)
#define DI_420_10BIT_BUFFER_SIZE (3840 * 2160 * 15 / 8)
#define DI_422_8BIT_BUFFER_SIZE (3840 * 2160 * 16 / 8)
#define DI_420_8BIT_BUFFER_SIZE (3840 * 2160 * 12 / 8)

unsigned int drv_memory_di_alloc_buffer_get(void)
{
	return di_buffer_alloc_size;
}

void drv_memory_di_alloc_buffer_set(unsigned char source, unsigned char data_10bit)
{
	di_buffer_size = DI_422_10BIT_BUFFER_SIZE;
	di_buffer_alloc_size = di_buffer_size+512;

#if 0
	if (source == _SRC_VO) {
		di_buffer_alloc_size=DI_420_10BIT_BUFFER_SIZE+512;
		di_buffer_size=DI_420_10BIT_BUFFER_SIZE;
	} else {
		di_buffer_alloc_size=DI_422_10BIT_BUFFER_SIZE+512;
		di_buffer_size=DI_422_10BIT_BUFFER_SIZE;
	}
#endif
	return;
}
unsigned int drv_memory_di_buffer_get(void)
{
	return di_buffer_size;
}

unsigned int drv_memory_di_data_size_get(unsigned char access,unsigned int source_width)
{
	unsigned int buffer_size;
	if(access==SLR_DI_progress_mode) {
		buffer_size=di_buffer_size;
	} else {
		buffer_size=0x280000;	// (1920*1080/2)*20/8+n
	}
	return buffer_size;
}
unsigned int drv_memory_di_info_size_get(unsigned char access)
{
	unsigned int buffer_size;
	if(access==SLR_DI_progress_mode) {
		buffer_size=0;
	} else {
		buffer_size=0x200000;		// (1920*1080/2)*16/8+n
	}
	return buffer_size;
}

#ifdef Mer2_MEMORY_ALLOC
#define I2R_Index_NUM 2
unsigned char DI_Compression_Bit_Num[I2R_Index_NUM] = {0};
unsigned char DI_Compression_Mode[I2R_Index_NUM] = {0};
static unsigned char VIP_PQ_DI_RTNR_PQC_Mode_TBL[VIP_PQ_DI_RTNR_PQC_Mode_Timing_Max] =
{
	/* DI_RTNR_PQC_3A */		PQC_Disable,
	/* DI_RTNR_PQC_5A, */		PQC_Disable,
	/* DI_RTNR_PQC_SD, */		PQC_Disable,
	/* DI_RTNR_PQC_720p, */		PQC_Disable,
	/* DI_RTNR_PQC_1080p, */	PQC_Frame_Mode,
	/* DI_RTNR_PQC_4k, */		PQC_Line_Mode_Rolling,
	/* DI_RTNR_PQC_8k, */		PQC_Line_Mode_Rolling,

};

char drv_memory_di_Set_Compression_Bit_Num(unsigned char Bit_Num, unsigned char index)
{
	if (index >= I2R_Index_NUM)
		index = 0;

	DI_Compression_Bit_Num[index] = Bit_Num; /* 0 = disable compression mode*/
	return 0;
}

unsigned char drv_memory_di_Get_Compression_Bit_Num(unsigned char index)
{
	if (index >= I2R_Index_NUM)
		index = 0;

	return DI_Compression_Bit_Num[index];
}

char drv_memory_di_Set_PQC_init(void)
{
	_RPC_system_setting_info* RPC_system_info_structure_table;

	RPC_system_info_structure_table = (_RPC_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
	if(RPC_system_info_structure_table == NULL){
		rtd_pr_scaler_memory_emerg("drv_memory_di_Set_PQC_init, NULL table \n");
		return -1;
	}

	//set PQC initial value
	RPC_system_info_structure_table->PQ_DI_RTNR_PQC_CTRL.cmp_alpha_en = 0;
	//RPC_system_info_structure_table->PQ_DI_RTNR_PQC_CTRL.PQC_Mode = PQC_Line_Mode_Rolling;
	memcpy(&RPC_system_info_structure_table->PQ_DI_RTNR_PQC_CTRL.PQC_Mode[0], &VIP_PQ_DI_RTNR_PQC_Mode_TBL[0], sizeof(char)*VIP_PQ_DI_RTNR_PQC_Mode_Timing_Max);

	return 0;
}

char drv_memory_di_Set_Compression_Mode(unsigned char Mode, unsigned char index)
{
	if (Mode >= VIP_DI_PQC_MODE_MAX)
		Mode = PQC_Disable;

	if (index >= I2R_Index_NUM)
		index = 0;

	DI_Compression_Mode[index] = Mode; /* 0 = disable compression mode*/
	return 0;

}

unsigned char drv_memory_di_Get_Compression_Mode(unsigned char index)
{
	if (index >= I2R_Index_NUM)
		index = 0;

	return DI_Compression_Mode[index];
}

unsigned char drv_memory_di_Decide_PQC_alpha_En(void)
{
	unsigned char en_flag;
	_RPC_system_setting_info* RPC_system_info_structure_table;

	RPC_system_info_structure_table = (_RPC_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
	if(RPC_system_info_structure_table == NULL){
		rtd_pr_scaler_memory_emerg("drv_memory_di_Decide_PQC_mode, NULL table \n");
		return 0;
	}

	en_flag = RPC_system_info_structure_table->PQ_DI_RTNR_PQC_CTRL.cmp_alpha_en;

	return en_flag;
}

unsigned char drv_memory_get_di_PQC_timing(unsigned char src_idx, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char index)
{
	unsigned char timing = 0;
	if (access == SLR_DI_3A_mode)
		timing = DI_RTNR_PQC_3A;
	else if (access == SLR_DI_5A_mode)
		timing = DI_RTNR_PQC_5A;
	else if (access == SLR_DI_progress_mode && DI_Height <= 600)	// 576p
		timing = DI_RTNR_PQC_SD;
	else if (access == SLR_DI_progress_mode && DI_Height <= 750)	// 720p
		timing = DI_RTNR_PQC_720p;
	else if (access == SLR_DI_progress_mode && DI_Height <= 1100)	// 1080p
		timing = DI_RTNR_PQC_1080p;
	else if (access == SLR_DI_progress_mode && DI_Height <= 2200)	// 2160
		timing = DI_RTNR_PQC_4k;
	else if (access == SLR_DI_progress_mode && DI_Height <= 4350)	// 720p
		timing = DI_RTNR_PQC_SD;
	else
		rtd_pr_scaler_memory_info("get_di_PQC_timing, input error, access=%d, DI_Width=%d, DI_Height=%d \n", access, DI_Width, DI_Height);

	rtd_pr_scaler_memory_info("get_di_PQC_timing,timing=%d, access=%d, DI_Width=%d, DI_Height=%d \n", timing, access, DI_Width, DI_Height);
	return timing;
}

unsigned char drv_memory_di_Decide_PQC_mode(unsigned char src_idx, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_DMA_Mode, unsigned char index)
{
	unsigned char timing;
	unsigned char PQC_Mode;
	_RPC_system_setting_info* RPC_system_info_structure_table;

	RPC_system_info_structure_table = (_RPC_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
	if(RPC_system_info_structure_table == NULL){
		rtd_pr_scaler_memory_emerg("drv_memory_di_Decide_PQC_mode, NULL table \n");
		return 0;
	}

	//PQC_Mode = RPC_system_info_structure_table->PQ_DI_RTNR_PQC_CTRL.PQC_Mode;

	timing = drv_memory_get_di_PQC_timing(0, access, DI_Width, DI_Height, 0);

	PQC_Mode = RPC_system_info_structure_table->PQ_DI_RTNR_PQC_CTRL.PQC_Mode[timing];

	return PQC_Mode;
}

unsigned char drv_memory_di_Decide_PQC_Limit_Bit(unsigned char src_idx, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_DMA_Mode, unsigned char index)
{
	unsigned char PQC_Bit_Limit;
#if 0
	INPUT_TIMING_INDEX patternIdx = get_current_pattern_index();

	if ((patternIdx == VO_4K2K_30) || (patternIdx == VO_4K2K_60) /*|| (patternIdx == HDMI_4K2K_60)*/)
		PQC_Bit_Limit = 10;
	else
		PQC_Bit_Limit = 10;

	PQC_Bit_Limit = get_current_driver_pattern(DRIVER_DIPQC_BIT_PATTERN);

	rtd_pr_scaler_memory_info("PQC Bit, patternIdx=%d \n", patternIdx);

#else
	PQC_Bit_Limit = get_current_driver_pattern(DRIVER_DIPQC_BIT_PATTERN);
	rtd_pr_scaler_memory_info("PQC Bit, PQC_Bit_Limit=%d \n", PQC_Bit_Limit);

	if (PQC_Bit_Limit == 0)	//6bit (off) / 10bit(on) / 9bit(2) / 8bit(3) / 7bit(4) / 6bit(5) / 5bit(6) / 4bit(7)
		PQC_Bit_Limit = 6;
	else if (PQC_Bit_Limit == 2)
		PQC_Bit_Limit = 9;
	else if (PQC_Bit_Limit == 3)
		PQC_Bit_Limit = 8;
	else if (PQC_Bit_Limit == 4)
		PQC_Bit_Limit = 7;
	else if (PQC_Bit_Limit == 5)
		PQC_Bit_Limit = 6;
	else if (PQC_Bit_Limit == 6)
		PQC_Bit_Limit = 5;
	else if (PQC_Bit_Limit == 7)
		PQC_Bit_Limit = 4;
	else
		PQC_Bit_Limit = 10;
#endif

	return PQC_Bit_Limit;
}

unsigned int drv_memory_di_PQC_Line_Sum_Bit(unsigned int Val, unsigned char index, unsigned char isSetFlag)
{
	static unsigned int PQC_line_sun_bit[2];	//  unit: 128bit

	if (index > 1)
		index = 0;

	if (isSetFlag == 1)
		PQC_line_sun_bit[index] = Val;

	return PQC_line_sun_bit[index];
}

char drv_memory_di_Compression_Config(unsigned char src_idx, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_DMA_Mode, unsigned char index)
{
//#ifdef VPQ_COMPILER_ERROR_ENABLE_MERLIN6
	//unsigned char cmp_alpha_en;
	unsigned short cmp_line_sum_bit;
	unsigned char PQC_Mode;
	unsigned char PQC_Bit_Limit;
	unsigned char PQC_Data_Format;
	unsigned int PQC_Width;
#if 1
	di_di_pq_cmp_RBUS di_di_pq_cmp_reg;
	di_di_pq_decmp_RBUS di_di_pq_decmp_reg;
	di_di_pq_cmp_pair_RBUS di_di_pq_cmp_pair_reg;
	di_di_pq_decmp_pair_RBUS di_di_pq_decmp_pair_reg;
	di_ddr_datdma_rm_RBUS di_ddr_datdma_rm_reg;
	di_di_pq_decmp_sat_en_RBUS di_di_pq_decmp_sat_en_reg;
	di_di_pq_cmp_bit_RBUS di_di_pq_cmp_bit_reg;
	di_di_pq_cmp_enable_RBUS di_di_pq_cmp_enable_reg;
	di_di_pq_cmp_allocate_RBUS di_di_pq_cmp_allocate_reg;
	di_di_dma_multi_req_num_RBUS di_di_dma_multi_req_num_reg;
	di_di_pq_cmp_blk0_add0_RBUS di_di_pq_cmp_blk0_add0_reg;
	di_di_pq_cmp_blk0_add1_RBUS di_di_pq_cmp_blk0_add1_reg;
	di_di_pq_cmp_balance_RBUS di_di_pq_cmp_balance_reg;
	di_di_pq_cmp_poor_RBUS di_di_pq_cmp_poor_reg;
	di_di_pq_cmp_smooth_RBUS di_di_pq_cmp_smooth_reg;
	di_di_pq_cmp_guarantee_RBUS di_di_pq_cmp_guarantee_reg;
	di_di_pq_cmp_qp_st_RBUS di_di_pq_cmp_qp_st_reg;
	di_di_pq_cmp_fifo_st1_RBUS di_di_pq_cmp_fifo_st1_reg;

//di_im_di_dma_RBUS di_im_di_dma_reg;
	/* double buffer*/
	di_db_reg_ctl_RBUS di_db_reg_ctl_reg;

	/* compression only in p mode*/	/* 1080p and 4K*/
	if (access != SLR_DI_progress_mode || DI_Height<960) {
		di_di_pq_cmp_reg.regValue= IoReg_Read32(DI_DI_PQ_CMP_reg);
		di_di_pq_decmp_reg.regValue = IoReg_Read32(DI_DI_PQ_DECMP_reg);
		di_di_pq_cmp_reg.cmp_en = 0;
		di_di_pq_decmp_reg.decmp_en = 0;
		IoReg_Write32(DI_DI_PQ_CMP_reg, di_di_pq_cmp_reg.regValue);
		IoReg_Write32(DI_DI_PQ_DECMP_reg, di_di_pq_decmp_reg.regValue);

		di_di_dma_multi_req_num_reg.regValue = IoReg_Read32(DI_DI_DMA_Multi_Req_Num_reg);
//		di_di_dma_multi_req_num_reg.rdma_addr_rolling = 0;
//		di_di_dma_multi_req_num_reg.wdma_addr_rolling = 0;
		IoReg_Write32(DI_DI_DMA_Multi_Req_Num_reg, di_di_dma_multi_req_num_reg.regValue);

		IoReg_SetBits(DI_db_reg_ctl_reg, DI_db_reg_ctl_db_apply_mask);

		drv_memory_di_Set_Compression_Bit_Num(0, index);
		drv_memory_di_Set_Compression_Mode(PQC_Disable, index);

		rtd_pr_scaler_memory_info("PQC Disable, return\n");

		return 1;
	}

	PQC_Mode = drv_memory_di_Decide_PQC_mode(src_idx,  access, DI_Width, DI_Height, DI_DMA_Mode, index);
	//cmp_alpha_en = drv_memory_di_Decide_PQC_alpha_En();
	PQC_Bit_Limit = drv_memory_di_Decide_PQC_Limit_Bit(src_idx,  access, DI_Width, DI_Height, DI_DMA_Mode, index);
	PQC_Data_Format = get_current_driver_pattern(DRIVER_RTNR_PATTERN);

	PQC_Width =  ((DI_Width%32)==0)?(DI_Width):(DI_Width + (32 - (DI_Width%32)));

	/* set flag*/
	drv_memory_di_Set_Compression_Bit_Num(PQC_Bit_Limit, index);
	drv_memory_di_Set_Compression_Mode(PQC_Mode, index);

	/* rdma_remain_en = 0, in compression mode*/
	di_ddr_datdma_rm_reg.regValue = IoReg_Read32(DI_DDR_DATDMA_RM_reg);
	di_ddr_datdma_rm_reg.rdma_remain_en = 0;
	IoReg_Write32(DI_DDR_DATDMA_RM_reg, di_ddr_datdma_rm_reg.regValue);

	/* com and decom setting*/
	di_di_pq_cmp_reg.regValue= IoReg_Read32(DI_DI_PQ_CMP_reg);
	di_di_pq_decmp_reg.regValue = IoReg_Read32(DI_DI_PQ_DECMP_reg);
	di_di_pq_cmp_pair_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_PAIR_reg);
	di_di_pq_decmp_pair_reg.regValue = IoReg_Read32(DI_DI_PQ_DECMP_PAIR_reg);
	di_di_pq_cmp_bit_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_BIT_reg);

	di_di_pq_cmp_reg.cmp_en = 1;
	di_di_pq_cmp_reg.cmp_height = DI_Height;
	di_di_pq_cmp_reg.cmp_width_div32 = PQC_Width >> 5;

	di_di_pq_cmp_pair_reg.cmp_jump4_en = 1;
	di_di_pq_cmp_pair_reg.cmp_dic_mode_en = 1;
	di_di_pq_cmp_pair_reg.cmp_data_bit_width = 1;
	di_di_pq_cmp_pair_reg.cmp_data_color = 1;
	di_di_pq_cmp_pair_reg.cmp_pair_para = 1; 	// m6
	di_di_pq_cmp_pair_reg.cmp_qp_mode = 0;		// m6
	if(Get_PANEL_VFLIP_ENABLE())
		di_di_pq_cmp_pair_reg.two_line_prediction_en = 0;	// m6
	else
		di_di_pq_cmp_pair_reg.two_line_prediction_en = 1;	// m6
	if (PQC_Data_Format == DI_RTNR_420 || DI_DMA_Mode == SLR_DMA_420_mode)
		di_di_pq_cmp_pair_reg.cmp_data_format = 2;	/*PQC format = 420 */
	else if (PQC_Data_Format == DI_RTNR_400)
		di_di_pq_cmp_pair_reg.cmp_data_format = 3;
	else
		di_di_pq_cmp_pair_reg.cmp_data_format = 1;

	/* frame mode or line mode*/
	if (PQC_Mode == PQC_Line_Mode || PQC_Mode == PQC_Line_Mode_Rolling || PQC_Mode == PQC_Line_Mode_Single_Buffer)
		di_di_pq_cmp_pair_reg.cmp_line_mode = 1;
	else
		di_di_pq_cmp_pair_reg.cmp_line_mode = 0;

	cmp_line_sum_bit = ((PQC_Width * PQC_Bit_Limit + 256)+127)>>7;	// unit: 128bit
	di_di_pq_cmp_pair_reg.cmp_line_sum_bit = cmp_line_sum_bit;
	drv_memory_di_PQC_Line_Sum_Bit(cmp_line_sum_bit, index, 1);

#if 0
	if (PQC_Mode == PQC_Frame_Mode) {
		di_di_pq_cmp_bit_reg.frame_limit_bit = PQC_Bit_Limit;
		di_di_pq_cmp_bit_reg.line_limit_bit = PQC_Bit_Limit + 4;	/*recommand by skyred for mer3*/
		di_di_pq_cmp_bit_reg.block_limit_bit = 0x3f;	/*recommand by skyred for mer3*/
	} else {
		di_di_pq_cmp_bit_reg.frame_limit_bit = PQC_Bit_Limit;	/* compress to 10 bit for frame mode*/
		di_di_pq_cmp_bit_reg.line_limit_bit = PQC_Bit_Limit + 4;		/* cmpression to 10 bit for line mode*/
		di_di_pq_cmp_bit_reg.block_limit_bit = 0x3f;	/*recommand by skyred for mer3*/
	}
#else
	di_di_pq_cmp_bit_reg.frame_limit_bit = PQC_Bit_Limit;
	di_di_pq_cmp_bit_reg.line_limit_bit = PQC_Bit_Limit + 4;	/*recommand by skyred for mer3*/
	di_di_pq_cmp_bit_reg.block_limit_bit = 0x3f;	/*recommand by skyred for mer3*/
	di_di_pq_cmp_bit_reg.first_line_more_bit = 32; 	// m6
#endif

	di_di_pq_decmp_reg.decmp_en = 1;
	di_di_pq_decmp_reg.decmp_height = DI_Height;
	di_di_pq_decmp_reg.decmp_width_div32 = PQC_Width >> 5;

	di_di_pq_decmp_pair_reg.decmp_jump4_en = 1;
	di_di_pq_decmp_pair_reg.decmp_dic_mode_en = 1;
	di_di_pq_decmp_pair_reg.decmp_data_bit_width = 1;
	di_di_pq_decmp_pair_reg.decmp_data_color = 1;
	di_di_pq_decmp_pair_reg.decmp_pair_para = 1;		//  m6
	di_di_pq_decmp_pair_reg.decmp_qp_mode = 0;		// m6
	if(Get_PANEL_VFLIP_ENABLE())
		di_di_pq_decmp_pair_reg.decmp_two_line_prediction_en = 0;	// m6
	else
		di_di_pq_decmp_pair_reg.decmp_two_line_prediction_en = 1;	// m6

	if (PQC_Data_Format == DI_RTNR_420 || DI_DMA_Mode == SLR_DMA_420_mode)
		di_di_pq_decmp_pair_reg.decmp_data_format = 2;	/*PQC format = 420 */
	else if (PQC_Data_Format == DI_RTNR_400)
		di_di_pq_decmp_pair_reg.decmp_data_format = 3;
	else
		di_di_pq_decmp_pair_reg.decmp_data_format = 1;

	/* frame mode or line mode*/
	if (PQC_Mode == PQC_Line_Mode || PQC_Mode == PQC_Line_Mode_Rolling || PQC_Mode == PQC_Line_Mode_Single_Buffer)
		di_di_pq_decmp_pair_reg.decmp_line_mode = 1;
	else
		di_di_pq_decmp_pair_reg.decmp_line_mode = 0;

	di_di_pq_decmp_pair_reg.decmp_line_sum_bit = cmp_line_sum_bit;

	IoReg_Write32(DI_DI_PQ_CMP_reg, di_di_pq_cmp_reg.regValue);
	IoReg_Write32(DI_DI_PQ_DECMP_reg, di_di_pq_decmp_reg.regValue);
	IoReg_Write32(DI_DI_PQ_CMP_PAIR_reg, di_di_pq_cmp_pair_reg.regValue);
	IoReg_Write32(DI_DI_PQ_DECMP_PAIR_reg, di_di_pq_decmp_pair_reg.regValue);
	IoReg_Write32(DI_DDR_DATDMA_RM_reg, di_ddr_datdma_rm_reg.regValue);
	IoReg_Write32(DI_DI_PQ_CMP_BIT_reg, di_di_pq_cmp_bit_reg.regValue);

	/* for decom setting*/
	di_di_pq_decmp_sat_en_reg.regValue = IoReg_Read32(DI_DI_PQ_DECMP_SAT_EN_reg);
	di_di_pq_decmp_sat_en_reg.saturation_en = 1;
	di_di_pq_decmp_sat_en_reg.saturation_type = 1;
	//di_di_pq_decmp_sat_en_reg.decmp_ctrl_para = 1;	//	m6
	di_di_pq_decmp_sat_en_reg.decmp_ctrl_para = 0x4; //	m7
	IoReg_Write32(DI_DI_PQ_DECMP_SAT_EN_reg, di_di_pq_decmp_sat_en_reg.regValue);

	/*recommand by skyred for mer3*/
	di_di_pq_cmp_enable_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_ENABLE_reg);
	di_di_pq_cmp_enable_reg.g_ratio = 14;		/*422 and 420 is same*/
	di_di_pq_cmp_enable_reg.tail_dummy_throughput = 1;
	di_di_pq_cmp_enable_reg.first_predict_nc_mode = 1;
	//di_di_pq_cmp_enable_reg.cmp_ctrl_para0 = 5;	// m6
	di_di_pq_cmp_enable_reg.cmp_ctrl_para0 = 0x10; 	// m7
	di_di_pq_cmp_enable_reg.variation_maxmin_en = 0;		// m6
	di_di_pq_cmp_enable_reg.do_422_mode = 0;	//  m6
	IoReg_Write32(DI_DI_PQ_CMP_ENABLE_reg, di_di_pq_cmp_enable_reg.regValue);

	/*recommand by skyred for mer3*/
	di_di_pq_cmp_allocate_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_ALLOCATE_reg);
	di_di_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;/*422 and 420 is same*/
	di_di_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;/*422 and 420 is same*/
	di_di_pq_cmp_allocate_reg.dynamic_allocate_line = 4; 	// m6
	di_di_pq_cmp_allocate_reg.dynamic_allocate_more = 2;	// m6
	di_di_pq_cmp_allocate_reg.dynamic_allocate_less = 2;	//  m6
	IoReg_Write32(DI_DI_PQ_CMP_ALLOCATE_reg, di_di_pq_cmp_allocate_reg.regValue);

	/*recommand by skyred for mer3*/
	di_di_pq_cmp_blk0_add0_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_BLK0_ADD0_reg);
	di_di_pq_cmp_blk0_add1_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_BLK0_ADD1_reg);
	di_di_pq_cmp_blk0_add0_reg.blk0_add_value32 = 16;
	di_di_pq_cmp_blk0_add0_reg.blk0_add_value16 = 8;
	di_di_pq_cmp_blk0_add1_reg.blk0_add_value8 = 4;
	di_di_pq_cmp_blk0_add1_reg.blk0_add_value4 = 2;
	di_di_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
	di_di_pq_cmp_blk0_add1_reg.blk0_add_value1 = 1;
	di_di_pq_cmp_blk0_add0_reg.blk0_add_value0 = 1;
	di_di_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
	IoReg_Write32(DI_DI_PQ_CMP_BLK0_ADD0_reg, di_di_pq_cmp_blk0_add0_reg.regValue);
	IoReg_Write32(DI_DI_PQ_CMP_BLK0_ADD1_reg, di_di_pq_cmp_blk0_add1_reg.regValue);

	di_di_pq_cmp_balance_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_BALANCE_reg);
	di_di_pq_cmp_balance_reg.balance_g_give = 3;
	di_di_pq_cmp_balance_reg.balance_g_ud_th = 3;	// m6, 0->3 ?
	di_di_pq_cmp_balance_reg.balance_g_ov_th = 0;	// m6, 3->0 ?
	di_di_pq_cmp_balance_reg.balance_rb_give = 3;
	di_di_pq_cmp_balance_reg.balance_rb_ud_th = 3;	// m6, 0->3 ?
	di_di_pq_cmp_balance_reg.balance_rb_ov_th = 0;	// m6, 3->0 ?
	IoReg_Write32(DI_DI_PQ_CMP_BALANCE_reg, di_di_pq_cmp_balance_reg.regValue);

	di_di_pq_cmp_poor_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_POOR_reg);
	di_di_pq_cmp_poor_reg.poor_limit_th_qp1 = 3;
	di_di_pq_cmp_poor_reg.poor_limit_th_qp2 = 6;
	di_di_pq_cmp_poor_reg.poor_limit_th_qp3 = 10;
	di_di_pq_cmp_poor_reg.poor_limit_th_qp4 = 16;
	IoReg_Write32(DI_DI_PQ_CMP_POOR_reg, di_di_pq_cmp_poor_reg.regValue);

	di_di_pq_cmp_smooth_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_SMOOTH_reg);
	di_di_pq_cmp_smooth_reg.dic_mode_entry_th = 15;
	IoReg_Write32(DI_DI_PQ_CMP_SMOOTH_reg, di_di_pq_cmp_smooth_reg.regValue);

	di_di_pq_cmp_guarantee_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_Guarantee_reg);
	di_di_pq_cmp_guarantee_reg.dic_mode_qp_th_g = 1;
	di_di_pq_cmp_guarantee_reg.dic_mode_qp_th_rb = 1;
	IoReg_Write32(DI_DI_PQ_CMP_Guarantee_reg, di_di_pq_cmp_guarantee_reg.regValue);

	di_di_pq_cmp_qp_st_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_QP_ST_reg);
	di_di_pq_cmp_qp_st_reg.cmp_ctrl_para1 = 0x44; //	m7
	di_di_pq_cmp_qp_st_reg.cmp_ctrl_para2 = 0xD0; // m7
	IoReg_Write32(DI_DI_PQ_CMP_QP_ST_reg, di_di_pq_cmp_qp_st_reg.regValue);

	di_di_pq_cmp_fifo_st1_reg.regValue = IoReg_Read32(DI_DI_PQ_CMP_FIFO_ST1_reg);
	di_di_pq_cmp_fifo_st1_reg.cmp_ctrl_para3 = 0; //	m7
	IoReg_Write32(DI_DI_PQ_CMP_FIFO_ST1_reg, di_di_pq_cmp_fifo_st1_reg.regValue);

#if 0	// move to end of DI flow
	/* enable rolling */
	di_di_dma_multi_req_num_reg.regValue = IoReg_Read32(DI_DI_DMA_Multi_Req_Num_reg);
	if (PQC_Mode == PQC_Line_Mode_Rolling) {
		di_di_dma_multi_req_num_reg.rdma_addr_rolling = 1;
		di_di_dma_multi_req_num_reg.wdma_addr_rolling = 1;
	} else {
		di_di_dma_multi_req_num_reg.rdma_addr_rolling = 0;
		di_di_dma_multi_req_num_reg.wdma_addr_rolling = 0;
	}
	IoReg_Write32(DI_DI_DMA_Multi_Req_Num_reg, di_di_dma_multi_req_num_reg.regValue);
#endif
#if 0
	/* some special case setting*/
	if (di_di_pq_cmp_pair_reg.cmp_data_format == 2) {	/* cmp_data_format = 420*/
		di_im_di_dma_reg.regValue = IoReg_Read32(DI_IM_DI_DMA_reg);
		di_im_di_dma_reg.dma_420_en = 1;/* 420 */
		IoReg_Write32(DI_IM_DI_DMA_reg, di_im_di_dma_reg.regValue);

		/*IoReg_ClearBits(DI_IM_DI_DMA_reg, _BIT31);
		IoReg_SetBits(DI_IM_DI_DMA_reg, _BIT30);*/
	}
#endif

	rtd_pr_scaler_memory_info("PQC, PQC_Mode=%d, PQC_Bit_Limit=%d, PQC_Data_Format=%d, vflip=%d,\n",
		PQC_Mode, PQC_Bit_Limit, PQC_Data_Format, Get_PANEL_VFLIP_ENABLE());

	/* double buffer?*/
	di_db_reg_ctl_reg.regValue = IoReg_Read32(DI_db_reg_ctl_reg);
	di_db_reg_ctl_reg.db_apply = 1;
	IoReg_Write32(DI_db_reg_ctl_reg, di_db_reg_ctl_reg.regValue);
	/*system_info->I_Height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
	system_info->I_Width = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID);*/
#endif
//#endif
return 0;
}

unsigned int drv_memory_Get_di_data_size_Static(unsigned char src_idx, unsigned char isCompression, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_MEM_IDX)
{
	unsigned int line_sum_bit=0;
	unsigned int buffer_size=0;
	unsigned int offset = 1024;
	//unsigned int PQC_Width = 0;
#if 1	// calculate memory
	unsigned char data_bit=0;

	if ((access == SLR_DI_progress_mode) && (isCompression != PQC_Disable)) {	/* compression mode, */
		line_sum_bit = drv_memory_di_PQC_Line_Sum_Bit(0, DI_MEM_IDX, 0);	// unit 128b
		buffer_size = ((line_sum_bit<<7) * DI_Height)>>3;
	} else {	/*3A, 5A, p mode with no PQC */
		data_bit = (fw_scalerip_get_DI_chroma_10bits())?(10):(8);
		if (drv_memory_get_ip_DMA420_mode() == SLR_DMA_422_mode)
			data_bit = data_bit + (data_bit>>1) + (data_bit>>1);	//Y + Cb + Cr
		else if (drv_memory_get_ip_DMA420_mode() == SLR_DMA_420_mode)
			data_bit = data_bit + ((data_bit + data_bit)>>2);
		buffer_size = (DI_Width * DI_Height * data_bit)>>3;
	}
	buffer_size = buffer_size + offset;
	rtd_pr_scaler_memory_info("di_data_size_cal, buffer_size=%d, data_bit=%d, line_sum_bit=%d,\n", buffer_size, data_bit, line_sum_bit);

#else	// static memory
	/*K4*/
	/*1080p and 4k is compression, other is non compression*/
	/* DI 1 = 13Mb, 10Mb for DI, 3Mb for MA-SNR @ m4, use rolling mode*/
	if (((isCompression == PQC_Frame_Mode) || (isCompression == PQC_Line_Mode)) && access == SLR_DI_progress_mode) {	/* compression mode, */
		buffer_size = 0xA00000; /*10Mb+10Mb, 2 buffer allocation, same setting between 1080p and 4k*/
	} else if (isCompression == PQC_Line_Mode_Rolling && access == SLR_DI_progress_mode) { /* p mode, non-compression and compression Line mode Rolling */
		buffer_size = 0xA00000; /*10Mb, 1 buffer allocation, same setting between 1080p and 4k*/
	} else if (access == SLR_DI_progress_mode) { /* p mode, non-compression and compression Line mode Rolling */
		//buffer_size = 0x1400000; /*20Mb, 1 buffer allocation, same setting between 1080p and 4k*/
		buffer_size = 0xA00000; /*10Mb, 1 buffer allocation, for 720p*/
	} else if (access == SLR_DI_3A_mode) {	/* i mode, 3A */
		buffer_size = 0x280000;  /*2.5 MB + 2.5MB , 2 buffer allocation*/
	} else {	/** i mode ,5A */
		buffer_size = 0x100000;  /*1MB + 1MB + 1MB + 1MB , 4 buffer allocation*/
	}

	if (DI_MEM_IDX == 1 /*&& get_platform() == PLATFORM_KXLP*/) {	/* index = 1, no suppport 4k*/
		if (((isCompression == PQC_Frame_Mode) || (isCompression == PQC_Line_Mode)) && access == SLR_DI_progress_mode) {	/* compression mode, */
			buffer_size = 0x280000; /*2.5Mb+2.5Mb, 2 buffer allocation, same setting between 1080p*/
		} else if (isCompression == PQC_Line_Mode_Rolling && access == SLR_DI_progress_mode) { /* p mode, non-compression and compression Line mode Rolling */
			buffer_size = 0x280000; /*2.5Mb, 1 buffer allocation, same setting between 1080p*/
		} else if (access == SLR_DI_progress_mode) { /* p mode, non-compression and compression Line mode Rolling */
			buffer_size = 0x500000; /*5 Mb, 1 buffer allocation, for 1080p*/
		} else if (access == SLR_DI_3A_mode) {	/* i mode, 3A */
			buffer_size = 0x280000;  /*2.5 MB + 2.5MB , 2 buffer allocation*/
		} else {	/** i mode ,5A */
			buffer_size = 0x100000;  /*1MB + 1MB + 1MB + 1MB , 4 buffer allocation*/
		}
	}
	buffer_size = buffer_size - offset;
#endif

	return buffer_size;
}

unsigned int drv_memory_Get_di_alloc_buffer_Static(unsigned char src_idx, unsigned char isCompression, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_MEM_IDX)
{
	unsigned int buffer_size;
//	unsigned int  add_more = 0x100;
#if 0
	/* no RTNR_INFO memory, p mode same as compression*/
	if (access == SLR_DI_progress_mode) { /* p mode, non-compression */
		if (DI_Height <= 1200) {	/* 1080p case*/
			buffer_size = 0x4F2B00 + add_more; /* only data = 4.947998047 MB in 1080p + n bytes */
		} else {
			buffer_size = 0x13C8800 + add_more; /* only data = 19.78320313 MB in 4K  + n bytes */
		}
	} else {	/* i mode */
		buffer_size = 0x1400000; /*data+info = 8.404541016 MB in 1080i, de-xc only need 16.43 MB, get more size(20MB) for all 5A allocation*/
	}
#else
#ifndef BUILD_QUICK_SHOW
	if (get_platform() == PLATFORM_KXLP) {		/* KxLp*/
		/* K3lp total = 34Mb(total) for 29(DI+MA_SNR in 4k main) + 4(DI+MA_SNR in 576i sub APVR) + 1(Demura)*/
		if (DI_MEM_IDX == 1) /* sub for APVR*/
			buffer_size = 0x400000;	/* 4Mb*/
		else
			buffer_size = 0x1D00000;	/* 29Mb*/
	} else
#endif
	{		/* KxL*/
		/* k3lp total 82Mb(total) for 40.5(DI+MA_SNR+DeXc in 1080p main) + 40.5(DI+MA_SNR+DeXc in 1080p sub) + 1(Demura) */
		buffer_size = 0x2880000;	/*40.5Mb*/
	}

	#if 0
	if (DI_MEM_IDX == 1 && get_platform() == PLATFORM_KXLP)
		buffer_size = 0x2EAB80;	/*k3lp, sub for apvr, only 2.916870117 MB for DI+MA_SNR*/
	else
		buffer_size = 	0x27CD580;	/* 39.80212402 MB for DI/RTNR+MA_SNR+DeXC*/
	#endif
#endif

#if 0 /* for test*/
	buffer_size = 0x1400000;
#endif

	return buffer_size;
}


unsigned int drv_memory_Get_di_info_size_Static(unsigned char src_idx, unsigned char isCompression, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_MEM_IDX)
{
	unsigned int buffer_size;
	unsigned int offset = 1024;
#if 1	// calculate memory
	unsigned char info_bit = 16;

	if (access == SLR_DI_progress_mode) { /* p mode */
		buffer_size = 0;
	} else {
		buffer_size = (DI_Width * DI_Height *  info_bit)>>3;
	}
	buffer_size = buffer_size + offset;
	rtd_pr_scaler_memory_info("di_info_size cal, buffer_size=%d,\n", buffer_size);

#else	// static memory
	/* no RTNR_INFO memory, p mode same as compression*/
	if (access == SLR_DI_progress_mode) { /* p mode */
		buffer_size = offset;
	} else if (access == SLR_DI_3A_mode) {	/* i mode, 3A */
		buffer_size = 0x280000;  /*2.5 MB + 2.5MB , 2 buffer allocation*/
	} else {
		buffer_size = 0x300000;  /*3 MB + 3MB , 2 buffer allocation*/
	}

	if (DI_MEM_IDX == 1 /*&& get_platform() == PLATFORM_KXLP*/) {
		if (access == SLR_DI_progress_mode) { /* p mode */
			buffer_size = 0;
		} else {	/* i mode */
			buffer_size = 0x1BAF80; /* info  = 3.460693359 MB in 1080i, 1.7303466795 MB per frame,  2 frame allocation*/
		}
	}
	buffer_size = buffer_size - offset;
#endif
	return buffer_size;
}
#endif

unsigned int drv_memory_set_ip_fifo_Mer2(unsigned char access)
{
	typedef struct _MEMSET {
		unsigned int number;
		unsigned int length;
		unsigned int remain;
		unsigned int water_level;
	} MEMSET;
	//di_im_di_btr_control_RBUS BTR_Control_Reg;
	di_ddr_rdatdma_wtlennum_RBUS ddr_rdatdma_wtlennum;
	di_ddr_rinfdma_wtlennum_RBUS ddr_rinfdma_wtlennum;
	di_ddr_wdatdma_wtlennum_RBUS ddr_wdatdma_wtlennum;
	di_ddr_winfdma_wtlennum_RBUS ddr_winfdma_wtlennum;
	di_ddr_datdma_rm_RBUS ddr_datdma_rm;
	di_ddr_infdma_rm_RBUS ddr_infdma_rm;
	//di_di_dma_multi_req_num_RBUS di_dma_multi_req_num_reg;
	//ddr_vrfifo1wtlvllennum_RBUS ddr_vrfifowtlvllennum;
	//ddr_vwfifo1wtlvllennum_RBUS ddr_vwfifowtlvllennum;
	//ddr_vfifo1rdwrremainder_RBUS ddr_vfifordwrremainder;

	unsigned int	DI_Width = 0, DI_Height = 0, size_per_frame = 0;
	unsigned char	data_bitNum = 0, info_bitNum = 0, i = 0/*, DIAllocateBlock = 0*/;
	unsigned int	data_value, info_value=0, MemStartAddr, MemEndtAddr;
	MEMSET			data_r, data_w, info_r, info_w;

	UINT32 lineSize_odd_128, lineSize_even_128;
	unsigned char DI_SMD_enable = 0;

	unsigned char source=255;
//	unsigned char newRTNR_onoff = 0;
//	unsigned char m_rtnrcompress = 0;
	unsigned char DI_BTR_enable = 0;
	unsigned char DI_BTR_type = 0;
	unsigned int buffer_alloc = 0;
	unsigned int data_size = 0;
	unsigned int info_size = 0;
	unsigned int data_size_aligned = 0;
	unsigned int info_size_aligned = 0;
	unsigned int MemAddr[6], MemAddr_aligned[6];
	unsigned short aligned_val = 0;
	unsigned char isCompression_Mode = 0, Compression_bit = 0;
	//unsigned char  data_num/*, info_num*/;
	unsigned char index;
	unsigned int data_addr_reg, info_addr_reg/*, data_addr_reg_3D, info_addr_reg_3D*/;
//fixme:vip
#if 1
	//extern Scaler_DI_Coef_t Scaler_DI_Coef_table[VIP_QUALITY_SOURCE_NUM];
	source = fwif_vip_source_check(3, 0);
	/*if(source >= VIP_QUALITY_SOURCE_NUM)
	{
		rtd_pr_scaler_memory_err("\r\n#####drv_memory_set_ip_fifo source nunber %d error####\r\n", source);
		source = VIP_QUALITY_SOURCE_NUM-1;
	}*/
	//frank@1018 add below code to solve scan tool warning
	//newRTNR_onoff = 0; //(unsigned char)Scaler_DI_Coef_table[source].newRTNR_style;
	//DI_SMD_enable = Scaler_getDI_SMD_en(source);
	DI_SMD_enable = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_INTERLACE);
	DI_BTR_enable = drv_memory_get_ip_Btr_mode();

	//BTR_Control_Reg.regValue = rtd_inl(DI_IM_DI_BTR_CONTROL_VADDR);
	//DI_BTR_type = BTR_Control_Reg.btr_solutiontype;
	DI_BTR_type = (Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID) < 960)?0:1;
#endif

	DI_Width = Scaler_DispGetInputInfo(SLR_INPUT_DI_WID);
	DI_Height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);

#ifdef CONFIG_BW_96B_ALIGNED
	aligned_val = 6;
#else
	aligned_val = 0;
#endif

	rtd_pr_scaler_memory_info("\n DI Memory set FIFO !, adaptivestream=%d, aligned_val=%d,\n", vsc_get_adaptivestream_flag(SLR_MAIN_DISPLAY), aligned_val);
	rtd_pr_scaler_memory_info("Width:%d, Height:%d, DMA mode=%d,access=%d\n ", DI_Width, DI_Height, drv_memory_get_ip_DMA420_mode(), access);
	rtd_pr_scaler_memory_info("Data status Y=%d C=%d\n ", fw_scalerip_get_DI_chroma_10bits()?10:8, fw_scalerip_get_DI_chroma_10bits()?10:8);
	rtd_pr_scaler_memory_info("Info status BTR=%d(type:%d) SMD=%d\n ", DI_BTR_enable, DI_BTR_type, DI_SMD_enable);
	//rtd_pr_scaler_memory_info("Info status BTR=%d(type:%d) SMD=%d RTNR style=%d\n ", DI_BTR_enable, DI_BTR_type, DI_SMD_enable, newRTNR_onoff);

	data_bitNum = 16; // for YUV subsample 422 ( now only using 2 typr : 422 & 420	)
	info_bitNum = 0;

#ifdef CONFIG_I2RND_DUAL_VO
	index = (Scaler_I2rnd_get_display()<=1)?(0):(1);	// Scaler_I2rnd_get_display() ==> 0: i2r disable, 1:i2r s0, 2:i2r s1
#else
	index = 0;
#endif
	/* new DI/RTNR compression for Mer2*/
	drv_memory_di_Compression_Config(source, access, DI_Width, DI_Height, drv_memory_get_ip_DMA420_mode(), index);
	//isCompression_Mode = drv_memory_di_Get_Compression_Bit_Num();
	Compression_bit = drv_memory_di_Get_Compression_Bit_Num(index);
	isCompression_Mode = drv_memory_di_Get_Compression_Mode(index);

	rtd_pr_scaler_memory_info("index=%d, Compression_bit=%d, isCompression_Mode=%d\n", index, Compression_bit, isCompression_Mode);

	// assign data bits
	if (isCompression_Mode == PQC_Disable) {
		if (fw_scalerip_get_DI_chroma_10bits())
			data_bitNum += 2;
		if (fw_scalerip_get_DI_chroma_10bits())
			data_bitNum += 2;
	} else {
		data_bitNum = Compression_bit;
	}
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
		//rtd_pr_scaler_memory_debug("\n 5A mode \n");
		//rtd_pr_scaler_memory_emerg("\n 5A mode \n");

		info_bitNum += (DI_BTR_enable?(DI_BTR_type?4:8):0); // type A:8 bits, type B:4 bits

		//DIAllocateBlock = 4;

		data_r.water_level = 98; /*68;*/	//WOSQRTK-12128 DMA underflow, modify water value from 68 to 98, recommand by Jimmy Xu
		data_r.length = 30; /*60;*/	//WOSQRTK-12128 DMA underflow, modify water value from 68 to 98, recommand by Jimmy Xu

		data_w.water_level = 60;
		data_w.length = 60;

		info_r.water_level = 32;
		info_r.length = 32;

		info_w.water_level = 32;
		info_w.length = 32;

		//data_num = 4;
		//info_num = 2;		/* BTR = on*/

	} else if (access == SLR_DI_3A_mode) {
		//rtd_pr_scaler_memory_debug("\n 3A mode \n");
		//rtd_pr_scaler_memory_emerg("\n 3A mode \n");

		info_bitNum += (DI_BTR_enable?4:0); // type A:8 bits, type B:4 bits, 3A only type B
		info_bitNum += 4; // frame motion in 3A default need 4 bits

		//DIAllocateBlock = 2;

		data_r.water_level = 196;
		data_r.length = 60;

		data_w.water_level = 60;
		data_w.length = 60;

		info_r.water_level = 32;
		info_r.length = 32;

		info_w.water_level = 32;
		info_w.length = 32;

		//data_num = 2;	/* use "2" for m4*/
		//info_num = 2;		/* BTR = on*/

	} else if (access == SLR_DI_progress_mode) {
		//rtd_pr_scaler_memory_debug("\n progress mode \n");

		//rtd_pr_scaler_memory_emerg("\n progress mode \n");
		//DIAllocateBlock = 2;

		//Waterlevel = 0x180 - length at PRTNR 2A case, but this value over 8bit(0xff), so give 0xff
		data_r.water_level = 0xff;
		data_r.length = 60;

		if (fwif_vip_increase_DI_water_lv())		//20150813 roger, change dat_len_r for 4k2k60p
			data_r.length = 96;

		if (isCompression_Mode == PQC_Line_Mode_Rolling) {
			if (DI_Height > 2100) {	// 4k
				data_w.water_level = 102;	// for PQC rolling mode and 96 aligned
				data_w.length = 102;
			} else {	// 2k
				data_w.water_level = 78;	// for PQC rolling mode and 96 aligned
				data_w.length = 78;
			}
		} else {
			data_w.water_level = 60;
			data_w.length = 60;
		}

		info_r.water_level = 32;
		info_r.length = 32;

		info_w.water_level = 32;
		info_w.length = 32;

		if(isCompression_Mode == PQC_Line_Mode_Rolling || isCompression_Mode == PQC_Line_Mode_Single_Buffer) {
			//data_num = 1;
			//info_num = 0;
			//DIAllocateBlock = 1;
		} else {
			//data_num = 2;
			//info_num = 0;
			//DIAllocateBlock = 2;
		}

	} else {
		rtd_pr_scaler_memory_debug("\n Error accept parameter in memory fifo \n");
		return false;
	}

	if (aligned_val != 0) {
		/* data burst len should be more after 96 byte aligned, recommand from xq_wu */
		data_r.length = ((data_r.length%aligned_val)==0)?(data_r.length):(data_r.length + (aligned_val - (data_r.length % aligned_val)));
		data_w.length = ((data_w.length%aligned_val)==0)?(data_w.length):(data_w.length + (aligned_val - (data_w.length % aligned_val)));
		/* info burst len should be less after 96 byte aligned, recommand from xq_wu */
		info_r.length = ((info_r.length%aligned_val)==0)?(info_r.length):(info_r.length - (info_r.length % aligned_val));
		info_w.length = ((info_w.length%aligned_val)==0)?(info_w.length):(info_w.length - (info_w.length % aligned_val));
	}

	if (DI_SMD_enable)
		info_bitNum += 4; //mac = 4, melin = 8

	// if k temporal function is enabled, new rtnr need 2 bits, otherwise still 0, please refer scalerColor.cpp:fwif_color_set_RTNR_style_adjust()
	//if (newRTNR_onoff == 3 || newRTNR_onoff == 4)
	//	info_bitNum += 2;

	//rtd_pr_scaler_memory_debug("\n Data BitNum=%d, Info BitNum=%d", data_bitNum, info_bitNum);
	//rtd_pr_scaler_memory_emerg("\n Data BitNum=%d, Info BitNum=%d", data_bitNum, info_bitNum);

	// Calculate Data number/length/remain
	if (drv_memory_get_ip_DMA420_mode() == SLR_DMA_422_mode) { // 422 mode
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

		IoReg_ClearBits(DI_IM_DI_DMA_reg, _BIT31);
		IoReg_ClearBits(DI_IM_DI_DMA_reg, _BIT30);
	}
	else if (drv_memory_get_ip_DMA420_mode() == SLR_DMA_420_mode) { // 420 mode
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
		//	size_per_frame = ((lineSize_even_128 + lineSize_odd_128+1)/2)*(DI_Height/2);
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
			rtd_pr_scaler_memory_emerg("Q-12237 data protect 01, Error data need to check!! (%d, %d, %d, %d, %d)\n",
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

		rtd_pr_scaler_memory_info("420 mode: linesize_even=%d, linesize_odd=%d\n", lineSize_even_128, lineSize_odd_128);
	}
	else { // 400 mode
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
		IoReg_SetBits(DI_IM_DI_DMA_reg, _BIT31);
		IoReg_ClearBits(DI_IM_DI_DMA_reg, _BIT30);
	}
	//IoReg_SetBits(DI_db_reg_ctl_reg, _BIT0);

	// Calculate Info number/length/remain
	size_per_frame = DI_Width * DI_Height * info_bitNum;
	if (DI_SMD_enable && access == SLR_DI_5A_mode) // Info image height must +1 for DMA setting when SMD enable in 5A case
		size_per_frame += DI_Width * info_bitNum;

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

	//rtd_pr_scaler_memory_debug("\n Data number = %d, length = %d, remain = %d, water_level = %d in setting DI Read FIFO", data_r.number, data_r.length, data_r.remain, data_r.water_level);
	//rtd_pr_scaler_memory_debug("\n Data number = %d, length = %d, remain = %d, water_level = %d in setting DI Write FIFO", data_w.number, data_w.length, data_w.remain, data_w.water_level);
	//rtd_pr_scaler_memory_debug("\n Info number = %d, length = %d, remain = %d, water_level = %d in setting DI Read FIFO", info_r.number, info_r.length, info_r.remain, info_r.water_level);
	//rtd_pr_scaler_memory_debug("\n Info number = %d, length = %d, remain = %d, water_level = %d in setting DI Write FIFO", info_w.number, info_w.length, info_w.remain, info_w.water_level);

	ddr_rdatdma_wtlennum.dat_water_r	= data_r.water_level;
	ddr_rdatdma_wtlennum.dat_len_r		= data_r.length;
	ddr_rdatdma_wtlennum.dat_num_r		= data_r.number;

	ddr_wdatdma_wtlennum.dat_water_w	= data_w.water_level;
	ddr_wdatdma_wtlennum.dat_len_w		= data_w.length;
	ddr_wdatdma_wtlennum.dat_num_w		= data_w.number;

	ddr_datdma_rm.dat_remain_r			= data_r.remain;
	ddr_datdma_rm.dat_remain_w			= data_w.remain;
	if (isCompression_Mode == PQC_Disable)
		ddr_datdma_rm.rdma_remain_en		= 1;
	else
		ddr_datdma_rm.rdma_remain_en		= 0;

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

	IoReg_Write32(DI_DDR_RDATDMA_WTLENNUM_reg, ddr_rdatdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_WDATDMA_WTLENNUM_reg, ddr_wdatdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_RINFDMA_WTLENNUM_reg, ddr_rinfdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_WINFDMA_WTLENNUM_reg, ddr_winfdma_wtlennum.regValue);
	IoReg_Write32(DI_DDR_DATDMA_RM_reg, ddr_datdma_rm.regValue);
	IoReg_Write32(DI_DDR_INFDMA_RM_reg, ddr_infdma_rm.regValue);

	IoReg_SetBits(DI_db_reg_ctl_reg, DI_db_reg_ctl_db_apply_mask);	/* HW do not clear this bit, alway turn on, setting will be apply while v_sync*/
	//IoReg_SetBits(DI_db_reg_ctl_reg, DI_db_reg_ctl_db_apply_1_mask);	// use D1 DB apply
	//frank@0514 change DI multi request to solve DI read underflow ++
	drv_memory_disable_di_write_req(FALSE);
	//frank@0514 change DI multi request to solve DI read underflow --

//frank@1202 Change code to solve 1366x768 image fail problem

#if 1/*qiangzhou: added DI memory allocate with VGIP PRE info for max size start*/
#if(CONFIG_DDR_DATA_BITS == 16)
	/*if ((Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_PIPMLSR) && (Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_PIPMLSR_4x3) &&
		(Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_MP1C12B) && (Scaler_PipGetInfo(SLR_PIP_TYPE) != SUBTYPE_MP1L7B)) {*/
		data_value = data_bitNum;
		info_value = info_bitNum;
		if(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) > _DISP_WID){
			data_value *= _DISP_WID;
			info_value *= _DISP_WID;
		}else{
			data_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
			info_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE);
		}

#ifndef BUILD_QUICK_SHOW
		if(((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status()) {
			data_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
			info_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
		} else
#endif
		{
			data_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
			info_value *= Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE);
		}

		data_value = (data_value >> 3);//byte unit
		info_value = (info_value >> 3);//byte unit
//		value =(info->IPV_ACT_LEN_PRE *info->IPH_ACT_WID_PRE * bitNum)>>4;
	/*} else {
		data_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*data_bitNum)>>3;	//byte unit
		info_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN)*Scaler_DispGetInputInfo(SLR_INPUT_DI_WID)*info_bitNum)>>3;	//byte unit
	}*/
#else
	data_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)*Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE)*data_bitNum)>>3;	//byte unit
	info_value=(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)*Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE)*info_bitNum)>>3;	//byte unit
#endif

	if (drv_memory_get_ip_DMA420_mode()) {
		//sync code from Mac2. Start
		//di_im_di_dma_RBUS im_di_dma;
		unsigned int di420_width;
		//im_di_dma.regValue = IoReg_Read32(DI_IM_DI_DMA_reg);
		lineSize_even_128 = (Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) * (data_bitNum - 8));
		if((lineSize_even_128%128)==0)
			lineSize_even_128 /=128;
		else
			lineSize_even_128 = (lineSize_even_128+128)/128;

		lineSize_odd_128 = (Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID_PRE) * data_bitNum);
		if((lineSize_odd_128%128)==0)
			lineSize_odd_128 /=128;
		else
			lineSize_odd_128 = (lineSize_odd_128+128)/128;

		if (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)%2 == 0) {
			di420_width = (lineSize_odd_128 + lineSize_even_128)*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)/2);
		} else {
			di420_width = lineSize_even_128*(Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)/2)+lineSize_odd_128*((Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN_PRE)+1)/2);
		}
		data_value = di420_width<<4;
		//sync code from Mac2. End
	}
#endif/*qiangzhou: added DI memory allocate with VGIP PRE info for max size end*/

	drvif_memory_free_block(MEMIDX_DI);

	// Set 32 avoid to cover other block memroy ([[h * v * bitnum] / (64* 2) ]* 16
	data_value = drvif_memory_get_data_align(data_value, 32);
	info_value = drvif_memory_get_data_align(info_value, 32);
	//frank@0707 when 3D mode DI must allocate 2 X memory

#if 1
	buffer_alloc = drv_memory_Get_di_alloc_buffer_Static(source, isCompression_Mode, access, DI_Width, DI_Height, index);
	data_size = drv_memory_Get_di_data_size_Static(source, isCompression_Mode, access, DI_Width, DI_Height, index);
	info_size = drv_memory_Get_di_info_size_Static(source, isCompression_Mode, access, DI_Width, DI_Height, index);

	if (aligned_val != 0) {
		data_size_aligned = dvr_size_alignment(data_size);
		///info_size_aligned = dvr_size_alignment(info_size);
	} else {
		data_size_aligned = data_size;
		//info_size_aligned = info_size;
	}

	if (index == 1) {
#ifdef CONFIG_I2RND_DUAL_VO
		MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI_2, buffer_alloc, MEMALIGN_ANY);
		data_addr_reg = DI_DI_I2R_DATMemoryStartAdd1_reg;
		info_addr_reg = DI_DI_I2R_INFMemoryStartAdd1_reg;
		data_addr_reg_3D = DI_DI_I2R_DATMemoryStartAdd1_3D_reg;
		info_addr_reg_3D = DI_DI_I2R_INFMemoryStartAdd1_3D_reg;
#else
		MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, buffer_alloc, MEMALIGN_ANY);
		data_addr_reg = DI_DI_DATMemoryStartAdd1_reg;
		info_addr_reg = DI_DI_INFMemoryStartAdd1_reg;
		//data_addr_reg_3D = DI_DI_DATMemoryStartAdd1_3D_reg;
		//info_addr_reg_3D = DI_DI_INFMemoryStartAdd1_3D_reg;
#endif
	} else{
		MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, buffer_alloc, MEMALIGN_ANY);
		data_addr_reg = DI_DI_DATMemoryStartAdd1_reg;
		info_addr_reg = DI_DI_INFMemoryStartAdd1_reg;
		//data_addr_reg_3D = DI_DI_DATMemoryStartAdd1_3D_reg;
		//info_addr_reg_3D = DI_DI_INFMemoryStartAdd1_3D_reg;
	}

	if(MemStartAddr == _DUMMYADDR){
		rtd_pr_scaler_memory_emerg("set MEMIDX_DI  to size fail !!!\n");
	}

	if (access == SLR_DI_progress_mode) {
		if (isCompression_Mode == PQC_Line_Mode_Single_Buffer) {
			MemAddr[0] = MemStartAddr;
			MemAddr[1] = MemStartAddr;
			MemAddr[2] = MemStartAddr + data_size;
			MemAddr[3] = MemStartAddr + data_size;
			MemAddr[4] = MemStartAddr + data_size;
			MemAddr[5] = MemStartAddr + data_size;

			MemEndtAddr = MemStartAddr + data_size;
		} else if (isCompression_Mode == PQC_Line_Mode_Rolling) {
			MemAddr[0] = MemStartAddr;
			MemAddr[1] = MemStartAddr + data_size;
			MemAddr[2] = MemStartAddr + data_size;
			MemAddr[3] = MemStartAddr + data_size;
			MemAddr[4] = MemStartAddr + data_size;
			MemAddr[5] = MemStartAddr + data_size;

			MemEndtAddr = MemStartAddr + data_size;
		} else {
			MemAddr[0] = MemStartAddr;
			MemAddr[1] = MemStartAddr + data_size;
			MemAddr[2] = MemStartAddr + data_size + data_size;
			MemAddr[3] = MemStartAddr + data_size + data_size;
			MemAddr[4] = MemStartAddr + data_size + data_size;
			MemAddr[5] = MemStartAddr + data_size + data_size;

			MemEndtAddr = MemStartAddr + data_size + data_size;
		}
	} else if (access == SLR_DI_3A_mode) {
		MemAddr[0] = MemStartAddr;
		MemAddr[1] = MemStartAddr + data_size;
		MemAddr[2] = MemStartAddr + data_size + data_size;
		MemAddr[3] = MemStartAddr + data_size + data_size;
		MemAddr[4] = MemStartAddr + data_size + data_size;
		MemAddr[5] = MemStartAddr + data_size + data_size + info_size;

		MemEndtAddr = MemStartAddr + data_size + data_size + info_size + info_size;
	} else {
		MemAddr[0] = MemStartAddr;
		MemAddr[1] = MemStartAddr + data_size;
		MemAddr[2] = MemStartAddr + data_size + data_size;
		MemAddr[3] = MemStartAddr + data_size + data_size + data_size;
		MemAddr[4] = MemStartAddr + data_size + data_size + data_size + data_size;
		MemAddr[5] = MemStartAddr + data_size + data_size + data_size + data_size + info_size;

		MemEndtAddr = MemStartAddr + data_size + data_size + data_size + data_size + info_size + info_size;
	}

	for (i=0;i<4;i++) {
		if (aligned_val != 0) {
			MemAddr_aligned[i] = dvr_memory_alignment((unsigned long)MemAddr[i], data_size_aligned);
			if (MemAddr_aligned[i] == 0) {
				rtd_pr_scaler_memory_emerg("DI, memory 96 byte alignment fail, data addr=%x, index=%d, data_size_aligned=%d\n",
					MemAddr[i], i,  data_size_aligned);
				BUG();
			}
		} else {
			MemAddr_aligned[i] = ((MemAddr[i]+15)>>4)<<4;//16 byte alignment
		}
	}

	for (i=4;i<6;i++) {
		if (aligned_val != 0 && access != SLR_DI_progress_mode) {
			MemAddr_aligned[i] = dvr_memory_alignment((unsigned long)MemAddr[i], info_size_aligned);
			if (MemAddr_aligned[i] == 0) {
				rtd_pr_scaler_memory_emerg("DI, memory 96 byte alignment fail, data addr=%x, index=%d, data_size_aligned=%d\n",
					MemAddr[i], i,  data_size_aligned);
				BUG();
			}
		} else {
			MemAddr_aligned[i] = ((MemAddr[i]+15)>>4)<<4;//16 byte alignment
		}
	}

	IoReg_Write32((data_addr_reg + 0),  (MemAddr_aligned[0]&0x7FFFFFFF));
	IoReg_Write32((data_addr_reg + 4),  (MemAddr_aligned[1]&0x7FFFFFFF));
	IoReg_Write32((data_addr_reg + 8),  (MemAddr_aligned[2]&0x7FFFFFFF));
	IoReg_Write32((data_addr_reg + 12),  (MemAddr_aligned[3]&0x7FFFFFFF));
	IoReg_Write32((info_addr_reg + 0),  (MemAddr_aligned[4]&0x7FFFFFFF) );
	IoReg_Write32((info_addr_reg + 4),  (MemAddr_aligned[5]&0x7FFFFFFF) );

	//IoReg_SetBits(DI_db_reg_ctl_reg, DI_db_reg_ctl_db_apply_1_mask);	// use D1 DB apply

	rtd_pr_scaler_memory_info("DI_BND=%x,%x, DI MVF Bnd=%x,%x, DI add base=%x,%x,\n",
		IoReg_Read32(DI_DI1_WRRD_BoundaryAddr1_reg), IoReg_Read32(DI_DI1_WRRD_BoundaryAddr2_reg), IoReg_Read32(DI_MVF_WR_BoundaryAddr1_reg),
		IoReg_Read32(DI_MVF_WR_BoundaryAddr2_reg), data_addr_reg, info_addr_reg);
	rtd_pr_scaler_memory_info("DI mem_addr=%x~%x,info_size=%d,data_size=%d,info_size_aligned=%d,data_size_aligned=%d,\n",
		MemStartAddr, MemEndtAddr, info_size, data_size, info_size_aligned, data_size_aligned);
	rtd_pr_scaler_memory_info("MEM addr=%x,%x,%x,%x,%x,%x\n", MemAddr[0], MemAddr[1], MemAddr[2], MemAddr[3], MemAddr[4], MemAddr[5]);
	rtd_pr_scaler_memory_info("MEM_aligned addr=%x,%x,%x,%x,%x,%x\n", MemAddr_aligned[0], MemAddr_aligned[1], MemAddr_aligned[2], MemAddr_aligned[3], MemAddr_aligned[4], MemAddr_aligned[5]);

	MemEndtAddr = MemEndtAddr + 1024; /* avoid trash between DI and MA-SNR*/
	MemEndtAddr = drv_memory_MA_SNR_Config(access, MemEndtAddr, index);
	rtd_pr_scaler_memory_info("MA-SNR mem_end=%x\n", MemEndtAddr);

	return MemStartAddr;
#else
#if 1//defined(ENABLE_DRIVER_I3DDMA)|| defined(ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE)
	//3D mode DI memory allocate x2: 3D frame sequence input video
	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())) {
		data_value *= 2;
		info_value *= 2;
	}
#endif

#if 1
		//define at makeconfig
		//patch for DI/RTNR bug(overwrite 512Byte)

	buffer_alloc = drv_memory_Get_di_alloc_buffer_Static(source, isCompression_Mode, access, DI_Width, DI_Height, index);
	data_size = drv_memory_Get_di_data_size_Static(source, isCompression_Mode, access, DI_Width, DI_Height, index);
	info_size = drv_memory_Get_di_info_size_Static(source, isCompression_Mode, access, DI_Width, DI_Height, index);

	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())){

			if(access != 2){
				MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value * DIAllocateBlock+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
			}
			else{
#ifdef ENABLE_DI_COMPRESS
				if(m_rtnrcompress){//compress mode use 2 buffer
					MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value*2+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
				}
				else
#endif
				{
					MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2+CONFIG_MDOMAIN_BURST_SIZE, MEMALIGN_ANY);
				}
			}
		} else {
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, buffer_alloc, MEMALIGN_ANY);
		}
		if(MemStartAddr == _DUMMYADDR){
			rtd_pr_scaler_memory_emerg("set MEMIDX_DI  to size fail !!!\n");
		}
		rtd_pr_scaler_memory_info("DI, mem_sta=%x,platform=%d\n", MemStartAddr, get_platform());
#else
	if(access != 2)
		MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value * DIAllocateBlock+info_value*2, MEMALIGN_ANY);
	else{
		if(m_rtnrcompress)//compress mode use 2 buffer
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value*2+info_value*2, MEMALIGN_ANY);
		else
			MemStartAddr = drvif_memory_alloc_block(MEMIDX_DI, data_value+info_value*2, MEMALIGN_ANY);
	}
#endif
	//rtd_pr_scaler_memory_debug("Get ip addr = %x, size=%x\n", MemStartAddr, (data_value * DIAllocateBlock+info_value*2));
#if 1//defined(ENABLE_DRIVER_I3DDMA)|| defined(ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE)
	//3D mode DI memory allocate x2: 3D frame sequence input video
	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())) {
		data_value /= 2;
		info_value /= 2;
	}
#endif

	if (index == 1) {
		data_addr_reg = DI_DI_I2R_DATMemoryStartAdd1_reg;
		info_addr_reg = DI_DI_I2R_INFMemoryStartAdd1_reg;
		data_addr_reg_3D = DI_DI_I2R_DATMemoryStartAdd1_3D_reg;
		info_addr_reg_3D = DI_DI_I2R_INFMemoryStartAdd1_3D_reg;

	} else {
		data_addr_reg = DI_DI_DATMemoryStartAdd1_reg;
		info_addr_reg = DI_DI_INFMemoryStartAdd1_reg;
		data_addr_reg_3D = DI_DI_DATMemoryStartAdd1_3D_reg;
		info_addr_reg_3D = DI_DI_INFMemoryStartAdd1_3D_reg;
	}

	if(drvif_scaler3d_get_IDMA3dMode()
			|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
				&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
				&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status()))
	{
		// Set start address for deinterlace
		for(i = 0; i < 4; i++) {
			//rtd_pr_scaler_memory_debug("IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((data_addr_reg + (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			/*if((access != SLR_DI_progress_mode)|| m_rtnrcompress)*/
			if (i < (data_num-1))
				MemStartAddr += data_value;//data_value;
		}

		/*if((access == SLR_DI_progress_mode)&&!m_rtnrcompress){
			MemStartAddr += data_value;
		}*/

		if (data_num != 0)
			MemStartAddr += data_value;

		for(i = 0; i < 2; i++) {
			//rtd_pr_scaler_memory_debug("Info IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((info_addr_reg + (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			/*if(access != SLR_DI_progress_mode)*/
			if (i < (info_num-1))
				MemStartAddr += info_value;
		}

		if (info_num != 0)
			MemStartAddr += info_value;

	}else {
		for(i = 0; i < 4; i++) {
			//rtd_pr_scaler_memory_debug("IP address%x = %x\n", i, MemStartAddr);
			IoReg_Write32((data_addr_reg + (i << 2)),  (MemStartAddr&0x7FFFFFFF) );
			/*if((access != SLR_DI_progress_mode)|| m_rtnrcompress)*/
			if (i < (data_num-1))
				MemStartAddr += data_size;//data_value;
		}
		/*if((access == SLR_DI_progress_mode)&&!m_rtnrcompress){
			MemStartAddr += data_size;
		}*/

		if (data_num != 0)
			MemStartAddr += data_size;

		for(i = 0; i < 2; i++) {
			//rtd_pr_scaler_memory_debug("Info IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((info_addr_reg + (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			/*if(access != SLR_DI_progress_mode)*/
			if (i < (info_num-1))
				MemStartAddr += info_size;
		}

		if (info_num != 0)
			MemStartAddr += info_size;

	}

#if 1//defined(ENABLE_DRIVER_I3DDMA)|| defined(ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE)
	if(drvif_scaler3d_get_IDMA3dMode()
		|| (((Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && (Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE() == TRUE)
			&& (Scaler_Disp3dGetInfo((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), SLR_DISP_3D_3DTYPE)==SLR_3D_FRAME_PACKING)) || (drvif_scaler3d_decide_3d_SG_Disable_IDMA(SLR_3D_FRAME_PACKING) == TRUE))
			&& drvif_scaler3d_decide_HDMI_framePacking_interlaced_status())) {
		for(i = 0; i < 4; i++) {
			//rtd_pr_scaler_memory_debug("3D IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((data_addr_reg_3D+ (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			/*if((access != SLR_DI_progress_mode)|| m_rtnrcompress)*/
			if (i < (data_num-1))
				MemStartAddr += data_value;
		}

		/*if((access == SLR_DI_progress_mode) && !m_rtnrcompress){
			MemStartAddr += data_value;
		}*/

		if (data_num != 0)
			MemStartAddr += data_value;

		for(i = 0; i < 2; i++) {
			//rtd_pr_scaler_memory_debug("3D Info IP address%x = %x\n", i, MemStartAddr);

			IoReg_Write32((info_addr_reg_3D + (i << 2)),  (MemStartAddr&0x7FFFFFFF) );

			/*if(access != SLR_DI_progress_mode)*/
			if (i < (info_num-1))
				MemStartAddr += info_value;
		}
	}
#endif
	rtd_pr_scaler_memory_info("DI mem_end=%x\n", MemStartAddr);
	MemStartAddr = drv_memory_MA_SNR_Config(access, MemStartAddr, index);
	rtd_pr_scaler_memory_info("info_size=%d,data_size=%d,mem_end=%x\n", info_size, data_size,MemStartAddr);
	/* move to "void fw_scalerip_set_deXC_XL(void);"*/
	/*MemStartAddr = drv_memory_I_De_XC_DMA_Config(MemStartAddr, 0, source);*/

	return MemStartAddr;
#endif
}

void drv_memory_set_De_Mura(unsigned int memory_Addr_sta)
{
	/*demura_demura_dma_start_addr_RBUS demura_demura_dma_start_addr_reg;*/

	//remove from merlin3
	//IoReg_Write32(DEMURA_DEMURA_DMA_START_ADDR_reg, memory_Addr_sta);
}

char  drv_memory_I_De_XC_ON_OFF_Wait(unsigned char En)
{
#if 0
	de_xcxl_de_xcxl_db_reg_ctl_RBUS de_xcxl_de_xcxl_db_reg_ctl_reg;

	IoReg_ClearBits(DE_XCXL_De_XCXL_CTRL_reg, DE_XCXL_De_XCXL_CTRL_dexc_en_mask);	/* turn off de-xc */
	IoReg_SetBits(DE_XCXL_DE_XCXL_db_reg_ctl_reg, DE_XCXL_DE_XCXL_db_reg_ctl_db_apply_mask);

	de_xcxl_de_xcxl_db_reg_ctl_reg.regValue = IoReg_Read32(DE_XCXL_DE_XCXL_db_reg_ctl_reg);
	while (de_xcxl_de_xcxl_db_reg_ctl_reg.db_apply == 1) {
		de_xcxl_de_xcxl_db_reg_ctl_reg.regValue = IoReg_Read32(DE_XCXL_DE_XCXL_db_reg_ctl_reg);
	}
#endif
	return TRUE;
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
#ifdef HW_SUPPORT_DE_XC
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
#else
	return 0;
#endif
}

unsigned int  drv_memory_I_De_XC_DMA_Config(unsigned int DI_phyAddr, unsigned char deXC_mode)
{
#ifdef HW_SUPPORT_DE_XC
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
#else
	return 0;
#endif
}

unsigned int drv_memory_Set_I_De_XC_DMA(void *dma_data, unsigned char DI_MEM_IDX)
{
#ifdef HW_SUPPORT_DE_XC
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
#else
	return 0;
#endif
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

unsigned char drv_memory_DeXCXL_Memory_Alloc_Condition_Check(void)
{
	unsigned char En_Flag = 0;
	unsigned char isAdaptivestream_flag = 0, I2R_Enable = 0;
	unsigned char DE_CONTOUR_I2R = 0;
	unsigned int height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
	unsigned int width = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID);
	unsigned int DRAM_size;
	unsigned char input_src_type;
	unsigned int EnByAP_table;
	unsigned char i2r_index;
	//unsigned char MEM_IDX;
	//unsigned char vip_src;

	/* I2R check */
#ifdef CONFIG_I2RND_DUAL_VO
	I2R_Enable = Scaler_I2rnd_get_timing_enable();
	i2r_index = (Scaler_I2rnd_get_display()<=1)?(0):(1);	// Scaler_I2rnd_get_display() ==> 0: i2r disable, 1:i2r s0, 2:i2r s1
#else
	I2R_Enable = 0;
	i2r_index = 0;
#endif

#ifdef ENABLE_DE_CONTOUR_I2R
	DE_CONTOUR_I2R = (rtd_inl(VGIP_VGIP_VBISLI_reg) & _BIT20);
#endif

	input_src_type = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);

	if (input_src_type == VSC_INPUTSRC_VDEC || input_src_type == VSC_INPUTSRC_CAMERA)
		isAdaptivestream_flag = get_vsc_run_adaptive_stream(SLR_MAIN_DISPLAY);
#ifndef BUILD_QUICK_SHOW
#ifndef UT_flag
	/* Dram size check*/
	DRAM_size = get_memory_size_total();
#endif // #ifndef UT_flag
#endif // #ifndef BUILD_QUICK_SHOW
	EnByAP_table = get_current_driver_pattern(DRIVER_DEXC_PATTERN);

      if((DE_CONTOUR_I2R == 1) || ((input_src_type != VSC_INPUTSRC_VDEC) && (input_src_type != VSC_INPUTSRC_CAMERA) && (input_src_type != VSC_INPUTSRC_HDMI)) || (height > 1200) || (i2r_index == 1) ||
	  (DRAM_size <= 0x40000000) || (I2R_Enable == 1) || (EnByAP_table == 0) || (isAdaptivestream_flag == 1))
		En_Flag = DeXCXL_Disable;
	else		/* condition check is ok,  DE-XC memory alloc*/
		En_Flag = DeXCXL_Y10C6_CMP;

	rtd_pr_scaler_memory_info("DE-XC,condition check,En=%d,h=%d,w=%d,Adaptivestream=%d,I2R_En=%d,src_type=%d,DRAM_size=%d,i2rIdx=%d,EnByAP=%d\n",
		En_Flag, height, width ,isAdaptivestream_flag,I2R_Enable, input_src_type, DRAM_size,i2r_index, EnByAP_table);

	return En_Flag;
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

unsigned char _I_BLK_decontourMem_Ready_Flag[2] = {0};
unsigned char drv_memory_Set_I_BLK_decontour_Mem_Ready_Flag(unsigned char MEM_IDX, unsigned char Flag)
{
	if (MEM_IDX>=2)
		return 255;
	_I_BLK_decontourMem_Ready_Flag[MEM_IDX] = Flag;
	return 0;
}

unsigned char drv_memory_Get_I_BLK_decontour_Mem_Ready_Flag(unsigned char MEM_IDX)
{
	if (MEM_IDX>=2)
		return 0;
	return _I_BLK_decontourMem_Ready_Flag[MEM_IDX];
}

unsigned int drv_memory_MA_SNR_Config(unsigned char access ,unsigned int phyAddr, unsigned char DI_MEM_IDX)
{
	/* DI part. */
	di_mvf_watmemorystart_s0_add0_RBUS di_mvf_watmemorystart_s0_add0_reg;
	di_mvf_watmemorystart_s0_add1_RBUS di_mvf_watmemorystart_s0_add1_reg;
	di_mvf_watmemorystart_s0_add2_RBUS di_mvf_watmemorystart_s0_add2_reg;

	di_mvf_ddr_wdma_wtlennum_RBUS di_mvf_ddr_wdma_wtlennum_reg;
	di_mvf_ddr_wdatdma_rm_RBUS di_mvf_ddr_wdatdma_rm_reg;
	di_db_reg_ctl_RBUS di_db_reg_ctl_reg;

	/* SNR part*/
	iedge_smooth_mvf_dma_dat_format_RBUS iedge_smooth_mvf_dma_dat_format_reg;
	iedge_smooth_mvf_datmemorystart_s0_add0_RBUS iedge_smooth_mvf_datmemorystart_s0_add0_reg;
	iedge_smooth_mvf_datmemorystart_s0_add1_RBUS iedge_smooth_mvf_datmemorystart_s0_add1_reg;
	iedge_smooth_mvf_datmemorystart_s0_add2_RBUS iedge_smooth_mvf_datmemorystart_s0_add2_reg;
	iedge_smooth_mvf_rddr_dma_wtlennum_RBUS iedge_smooth_mvf_rddr_dma_wtlennum_reg;
	iedge_smooth_mvf_rddr_datdma_rm_RBUS iedge_smooth_mvf_rddr_datdma_rm_reg;
	iedge_smooth_edsm_db_ctrl_RBUS iedge_smooth_edsm_db_ctrl_reg;
	iedge_smooth_mvf_dma_rd_ctrl_RBUS iedge_smooth_mvf_dma_rd_ctrl_reg;

	unsigned int height = Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN);
	unsigned int width = Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID);
	unsigned int pixel_total = height * width;
	unsigned short water, len, remain, num;
	unsigned int mem[4], mem_aligned[4], mem_end;
	unsigned int buffer, buffer_aligned;
	unsigned short aligned_val;
	unsigned short i;

	if (drv_memory_MA_SNR_Enable_Check(access, phyAddr, DI_MEM_IDX, width, height) == 0) {
		// for Decontour, need to set width even in disable case.
		iedge_smooth_mvf_dma_dat_format_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_DMA_Dat_format_reg);
		iedge_smooth_mvf_dma_dat_format_reg.mvf_height = height;
		iedge_smooth_mvf_dma_dat_format_reg.mvf_width = width;
		IoReg_Write32(IEDGE_SMOOTH_MVF_DMA_Dat_format_reg, iedge_smooth_mvf_dma_dat_format_reg.regValue);

		drv_memory_Set_MASNR_Mem_Ready_Flag(DI_MEM_IDX, 0);
		return phyAddr;
	}

	// enable DB
	IoReg_SetBits(DI_db_reg_ctl_reg, DI_db_reg_ctl_db_en_mask);
	IoReg_SetBits(IEDGE_SMOOTH_EDSM_DB_CTRL_reg, IEDGE_SMOOTH_EDSM_DB_CTRL_edsm_db_en_mask);
	IoReg_SetBits(NR_SNR_DB_CTRL_reg, NR_SNR_DB_CTRL_snr_db_en_mask);

#ifdef CONFIG_BW_96B_ALIGNED
	aligned_val = 6;
#else
	aligned_val = 0;
#endif

	/* DI part. */
	di_mvf_watmemorystart_s0_add0_reg.regValue = IoReg_Read32(DI_MVF_WATMemoryStart_S0_Add0_reg);
	di_mvf_watmemorystart_s0_add1_reg.regValue = IoReg_Read32(DI_MVF_WATMemoryStart_S0_Add1_reg);
	di_mvf_watmemorystart_s0_add2_reg.regValue = IoReg_Read32(DI_MVF_WATMemoryStart_S0_Add2_reg);
	di_mvf_ddr_wdma_wtlennum_reg.regValue = IoReg_Read32(DI_MVF_DDR_WDMA_WTLENNUM_reg);
	di_mvf_ddr_wdatdma_rm_reg.regValue = IoReg_Read32(DI_MVF_DDR_WDATDMA_RM_reg);

	/* SNR part*/
	iedge_smooth_mvf_dma_dat_format_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_DMA_Dat_format_reg);
	iedge_smooth_mvf_datmemorystart_s0_add0_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_DATMemoryStart_S0_Add0_reg);
	iedge_smooth_mvf_datmemorystart_s0_add1_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_DATMemoryStart_S0_Add1_reg);
	iedge_smooth_mvf_datmemorystart_s0_add2_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_DATMemoryStart_S0_Add2_reg);
	iedge_smooth_mvf_rddr_dma_wtlennum_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_RDDR_DMA_WTLENNUM_reg);
	iedge_smooth_mvf_rddr_datdma_rm_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_RDDR_DATDMA_RM_reg);
	iedge_smooth_mvf_dma_rd_ctrl_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_MVF_DMA_RD_Ctrl_reg);

	buffer = drv_memory_Get_MA_SNR_Info_Size_Static(access, width, height, DI_MEM_IDX);

	if (aligned_val != 0) {
		buffer_aligned = dvr_size_alignment(buffer);
	} else {
		buffer_aligned = buffer;
	}

	//Merlin4 5A use 4ddr, 3A&P use 3ddr
	if(access == SLR_DI_5A_mode)
	{
		mem[0] = (phyAddr);
		mem[1] = (phyAddr + buffer_aligned);
		mem[2] = (phyAddr + buffer_aligned * 2);
		mem[3] = (phyAddr + buffer_aligned * 3);
		mem_end = phyAddr + buffer_aligned * 4;

		if (aligned_val != 0) {
			mem_aligned[0] = dvr_memory_alignment((unsigned long)mem[0], buffer_aligned);
			mem_aligned[1] = dvr_memory_alignment((unsigned long)mem[1], buffer_aligned);
			mem_aligned[2] = dvr_memory_alignment((unsigned long)mem[2], buffer_aligned);
			mem_aligned[3] = dvr_memory_alignment((unsigned long)mem[3], buffer_aligned);
			for (i=0;i<4;i++) {
				if (mem_aligned[i] == 0) {
					rtd_pr_scaler_memory_emerg("MA-SNR, memory 96 byte alignment fail, addr=%x, index=%d, buffer_aligned=%d\n",
						mem[i], i,  buffer_aligned);
					BUG();
				}
			}

		} else {
			mem_aligned[0] = ((mem[0]+15)>>4)<<4;//16 byte alignment
			mem_aligned[1] = ((mem[1]+15)>>4)<<4;//16 byte alignment
			mem_aligned[2] = ((mem[2]+15)>>4)<<4;//16 byte alignment
			mem_aligned[3] = ((mem[3]+15)>>4)<<4;//16 byte alignment
		}
	}
	else
	{
		mem[0] = (phyAddr);
		mem[1] = (phyAddr + buffer_aligned);
		mem[2] = (phyAddr + buffer_aligned * 2);
		mem[3] = mem[2];
		mem_end = phyAddr + buffer_aligned * 3;

		if (aligned_val != 0) {
			mem_aligned[0] = dvr_memory_alignment((unsigned long)mem[0], buffer_aligned);
			mem_aligned[1] = dvr_memory_alignment((unsigned long)mem[1], buffer_aligned);
			mem_aligned[2] = dvr_memory_alignment((unsigned long)mem[2], buffer_aligned);
			mem_aligned[3] = mem_aligned[2];
			for (i=0;i<4;i++) {
				if (mem_aligned[i] == 0) {
					rtd_pr_scaler_memory_emerg("MA-SNR, memory 96 byte alignment fail, addr=%x, index=%d, buffer_aligned=%d\n",
						mem[i], i,  buffer_aligned);
					BUG();
				}
			}

 		} else {
			mem_aligned[0] = ((mem[0]+15)>>4)<<4;//16 byte alignment
			mem_aligned[1] = ((mem[1]+15)>>4)<<4;//16 byte alignment
			mem_aligned[2] = ((mem[2]+15)>>4)<<4;//16 byte alignment
			mem_aligned[3] = ((mem[3]+15)>>4)<<4;//16 byte alignment
		}
	}

	len = 16;
	if (aligned_val != 0)
		len = ((len%aligned_val)==0)?(len):(len - (len % aligned_val)); /* info burst len should be less after 96 byte aligned, recommand from xq_wu */

	water = 16;

	num = (pixel_total>>6) / len;	/* number*lenth+remain = pixel number*2/128, 128b per num */
	remain = (pixel_total>>6) - len * num;
	if (remain == 0) { /* remain can't be 0*/
		num -= 1;
		remain = len;
	}

	/* DI part. */
	di_mvf_ddr_wdma_wtlennum_reg.mvf_inf_len_w = len;
	di_mvf_ddr_wdma_wtlennum_reg.mvf_inf_water_w = water;
	di_mvf_ddr_wdma_wtlennum_reg.mvf_inf_num_w = num;
	di_mvf_ddr_wdatdma_rm_reg.mvf_inf_remain_w = remain;

	if (DI_MEM_IDX == 0) {
		di_mvf_watmemorystart_s0_add0_reg.s0_mstart_86_58 = mem_aligned[0]>>3;	/* bit 0~2 is reserved*/
		di_mvf_watmemorystart_s0_add1_reg.s0_mstart_57_29 = mem_aligned[1]>>3;	/* bit 0~2 is reserved*/
		di_mvf_watmemorystart_s0_add2_reg.s0_mstart_28_0 = mem_aligned[2]>>3;	/* bit 0~2 is reserved*/
	} else {
	}

	rtd_pr_scaler_memory_info("MA-SNR,water=%d, len=%d,num=%d,remain=%d, mem_end=%x,buffer=%x, buffer_aligned=%x,aligned_val=%d,\n",
		water, len ,num ,remain, mem_end, buffer, buffer_aligned,aligned_val);

	rtd_pr_scaler_memory_info("MA-SNR,mem=%x,%x,%x,%x, mem_aligned=%x%x,%x,%x,\n",
		mem[0], mem[1], mem[2], mem[3], mem_aligned[0], mem_aligned[1], mem_aligned[2], mem_aligned[3]);

	rtd_pr_scaler_memory_info("MA-SNR,access=%d, heigth=%d,width=%d, DI_mem_idx=%d\n", access, height, width, DI_MEM_IDX);

	/* SNR part*/
	/* same as DI part ? */
	iedge_smooth_mvf_rddr_dma_wtlennum_reg.rd_dat_len = len;
	iedge_smooth_mvf_rddr_dma_wtlennum_reg.rd_dat_num = num;
	iedge_smooth_mvf_rddr_datdma_rm_reg.rd_dat_remain = remain;
	iedge_smooth_mvf_rddr_datdma_rm_reg.rd_dma_remain_en = 1;

	iedge_smooth_mvf_dma_dat_format_reg.mvf_progressive = (access == SLR_DI_progress_mode)?1:0;
	iedge_smooth_mvf_dma_dat_format_reg.delay_frame_num = (access == SLR_DI_5A_mode)?(2):((access == SLR_DI_3A_mode)?(1):(0));
	iedge_smooth_mvf_dma_dat_format_reg.mvf_height = height;
	iedge_smooth_mvf_dma_dat_format_reg.mvf_width = width;
	iedge_smooth_mvf_dma_rd_ctrl_reg.rd_dma_enable = 1;

	if (DI_MEM_IDX == 0) {
		iedge_smooth_mvf_datmemorystart_s0_add0_reg.s0_mstart_83_56= mem[0]>>4;	/* bit 0~2 is reserved*/
		iedge_smooth_mvf_datmemorystart_s0_add1_reg.s0_mstart_55_28= mem[1]>>4;	/* bit 0~2 is reserved*/
		iedge_smooth_mvf_datmemorystart_s0_add2_reg.s0_mstart_27_0 = mem[2]>>4;	/* bit 0~2 is reserved*/
	} else {
	}
	// boundary
	IoReg_Write32(IEDGE_SMOOTH_MVF_DMA_Rule_check_up_reg, mem_end);
	IoReg_Write32(IEDGE_SMOOTH_MVF_DMA_Rule_check_low_reg, phyAddr);

	/* DI part. */
	IoReg_Write32(DI_MVF_DDR_WDMA_WTLENNUM_reg, di_mvf_ddr_wdma_wtlennum_reg.regValue);
	IoReg_Write32(DI_MVF_DDR_WDATDMA_RM_reg, di_mvf_ddr_wdatdma_rm_reg.regValue );

	if (DI_MEM_IDX == 0) {
		IoReg_Write32(DI_MVF_WATMemoryStart_S0_Add0_reg, di_mvf_watmemorystart_s0_add0_reg.regValue);
		IoReg_Write32(DI_MVF_WATMemoryStart_S0_Add1_reg, di_mvf_watmemorystart_s0_add1_reg.regValue);
		IoReg_Write32(DI_MVF_WATMemoryStart_S0_Add2_reg, di_mvf_watmemorystart_s0_add2_reg.regValue);
	} else {
	}
	/* SNR part*/
	IoReg_Write32(IEDGE_SMOOTH_MVF_DMA_Dat_format_reg, iedge_smooth_mvf_dma_dat_format_reg.regValue);
	IoReg_Write32(IEDGE_SMOOTH_MVF_RDDR_DMA_WTLENNUM_reg, iedge_smooth_mvf_rddr_dma_wtlennum_reg.regValue);
	IoReg_Write32(IEDGE_SMOOTH_MVF_RDDR_DATDMA_RM_reg, iedge_smooth_mvf_rddr_datdma_rm_reg.regValue);
	IoReg_Write32(IEDGE_SMOOTH_MVF_DMA_RD_Ctrl_reg, iedge_smooth_mvf_dma_rd_ctrl_reg.regValue);

	if (DI_MEM_IDX == 0) {
		IoReg_Write32(IEDGE_SMOOTH_MVF_DATMemoryStart_S0_Add0_reg, iedge_smooth_mvf_datmemorystart_s0_add0_reg.regValue);
		IoReg_Write32(IEDGE_SMOOTH_MVF_DATMemoryStart_S0_Add1_reg, iedge_smooth_mvf_datmemorystart_s0_add1_reg.regValue );
		IoReg_Write32(IEDGE_SMOOTH_MVF_DATMemoryStart_S0_Add2_reg, iedge_smooth_mvf_datmemorystart_s0_add2_reg.regValue);
	} else {
	}

	/* double buffer*/
	di_db_reg_ctl_reg.regValue = IoReg_Read32(DI_db_reg_ctl_reg);
	//di_db_reg_ctl_reg.db_apply = 1;
	di_db_reg_ctl_reg.db_apply_1 = 1;
	IoReg_Write32(DI_db_reg_ctl_reg, di_db_reg_ctl_reg.regValue);

	iedge_smooth_edsm_db_ctrl_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_EDSM_DB_CTRL_reg);
	iedge_smooth_edsm_db_ctrl_reg.edsm_db_apply = 1;
	IoReg_Write32(IEDGE_SMOOTH_EDSM_DB_CTRL_reg, iedge_smooth_edsm_db_ctrl_reg.regValue);

	drv_memory_Set_MASNR_Mem_Ready_Flag(DI_MEM_IDX, 1);
	return mem_end;
}

char drv_memory_MA_SNR_Enable_Check(unsigned char access ,unsigned int phyAddr, unsigned char DI_MEM_IDX, unsigned int DI_Width, unsigned int DI_Height)
{
	unsigned char ret;
	unsigned char decide_by_AP, vsc_src, adaptvie_stream_flag, DISP_THRIP, DISP_RTNR;
	unsigned char is2kModel = 0;
	decide_by_AP = get_current_driver_pattern(DRIVER_MASNR_PATTERN);
	vsc_src = Get_DisplayMode_Src(SLR_MAIN_DISPLAY);
	adaptvie_stream_flag = vsc_get_adaptivestream_flag(SLR_MAIN_DISPLAY);
	DISP_THRIP = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_THRIP);
	DISP_RTNR = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_RTNR);

	/* diable MA-SNR */
	/*1.DI is disable, */
	/*2. adaptive stream since "nr_motion_en", "IESM_Motion_En", "AVLPF_Motion_En" is not double buffer */
	/*3. .xml table */
	if(get_platform_model() == PLATFORM_MODEL_2K) {
		is2kModel = 1;
	}
	if ((decide_by_AP == 0) || ((vsc_src == VSC_INPUTSRC_VDEC) && (adaptvie_stream_flag >= ADAPTIVE_STREAM_ON)) || ((DISP_THRIP  == FALSE) && (DISP_RTNR == FALSE)) || (is2kModel == 1)) {
		ret = 0;
	} else {
		ret = 1;
	}

	rtd_pr_scaler_memory_info("MA-SNR, decide_by_AP=%d, vsc_src=%d, adaptvie_stream_flag=%d, DISP_THRIP=%d, DISP_RTNR=%d,\n",
		decide_by_AP, vsc_src, adaptvie_stream_flag, DISP_THRIP, DISP_RTNR);

	return ret;
}

char  drv_memory_MA_SNR_Disable(unsigned char MEM_idx)
{
	di_rtnr_ma_isnr_getmotion_main_RBUS getMotion_main_reg;
	nr_motion_weight_ctrl_RBUS nr_motion_weight_ctrl_reg;
	iedge_smooth_eng_clamp_RBUS eng_clamp_reg;
	iedge_smooth_vfilter_4_RBUS vfilter_4_reg;

	IoReg_ClearBits(DI_db_reg_ctl_reg, DI_db_reg_ctl_db_en_mask);
	IoReg_ClearBits(IEDGE_SMOOTH_EDSM_DB_CTRL_reg, IEDGE_SMOOTH_EDSM_DB_CTRL_edsm_db_en_mask);
	IoReg_ClearBits(NR_SNR_DB_CTRL_reg, NR_SNR_DB_CTRL_snr_db_en_mask);

	getMotion_main_reg.regValue = IoReg_Read32(DI_RTNR_MA_ISNR_GETMOTION_MAIN_reg);
	getMotion_main_reg.rtnr_isnr_getmotion_en = 0;
	IoReg_Write32(DI_RTNR_MA_ISNR_GETMOTION_MAIN_reg, getMotion_main_reg.regValue);

	nr_motion_weight_ctrl_reg.regValue = IoReg_Read32(NR_MOTION_WEIGHT_ctrl_reg);
	nr_motion_weight_ctrl_reg.nr_motion_en = 0;
	IoReg_Write32(NR_MOTION_WEIGHT_ctrl_reg, nr_motion_weight_ctrl_reg.regValue);

	eng_clamp_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_ENG_CLAMP_reg);
	eng_clamp_reg.iedsm_motion_en = 0;
	IoReg_Write32(IEDGE_SMOOTH_ENG_CLAMP_reg, eng_clamp_reg.regValue);

	vfilter_4_reg.regValue = IoReg_Read32(IEDGE_SMOOTH_VFILTER_6_reg);
	vfilter_4_reg.egsm_avlpf_motion_en = 0;
	IoReg_Write32(IEDGE_SMOOTH_VFILTER_6_reg, vfilter_4_reg.regValue);

	rtd_pr_scaler_memory_info("MA-SNR, Disable, idx=%d,\n", MEM_idx);

	drv_memory_Set_MASNR_Mem_Ready_Flag(MEM_idx, 0);
	return 0;
}

unsigned char MASNR_Mem_Ready_Flag[2] = {0};
unsigned char drv_memory_Set_MASNR_Mem_Ready_Flag(unsigned char MEM_IDX, unsigned char Flag)
{
	if (MEM_IDX>=2)
		return 255;
	MASNR_Mem_Ready_Flag[MEM_IDX] = Flag;
	return 0;
}

unsigned char drv_memory_Get_MASNR_Mem_Ready_Flag(unsigned char MEM_IDX)
{
	if (MEM_IDX>=2)
		return 0;
	return MASNR_Mem_Ready_Flag[MEM_IDX];
}

unsigned int  drv_memory_Get_MA_SNR_Info_Size_Static(unsigned char access, unsigned int I_Width, unsigned int I_Height, unsigned char DI_MEM_IDX)
{
	unsigned int buffer_size;
	unsigned int  offset = 1024;	/* avoid trash other memory*/
#if 1	//calculate memory
	unsigned int width, height, data_bit;

	data_bit = 2;
	if (access == SLR_DI_progress_mode) {
		width = I_Width>>1;
		height = I_Height;
	} else {
		width = I_Width;
		height = I_Height;
	}
	buffer_size = (width * height * data_bit)>>3;
	buffer_size = buffer_size + offset;

	rtd_pr_scaler_memory_info("masnr_size_cal, buffer_size=%d,\n", buffer_size);

#else	// static memory
	/* DI 1 = 13Mb, 10Mb for DI, 3Mb for MA-SNR @ m4, use rolling mode*/
	if (access == SLR_DI_5A_mode) { /* 5A, 4 buffer alloc*/
		buffer_size = 0xC0000 - offset ;	/* 0.75MB + 0.75MB + 0.75MB + 0.75MB = 3MB, 4 buffer alloc*/
	} else {	/* 3A and P mode, 3 buffer alloc*/
		buffer_size = 0x100000 - offset ;	/* 1MB + 1MB + 1MB = 3MB, 3 buffer alloc*/
	}

	if (DI_MEM_IDX == 1 /*&& get_platform() == PLATFORM_KXLP*/) {	/* i2r need to confirm*/
		if (access == SLR_DI_5A_mode) { /* 5A, 4 buffer alloc*/
			buffer_size = 0xC0000 - offset ;	/* 0.75MB + 0.75MB + 0.75MB + 0.75MB = 3MB, 4 buffer alloc*/
		} else {	/* 3A and P mode, 3 buffer alloc*/
			buffer_size = 0x100000 - offset ;	/* 1MB + 1MB + 1MB = 3MB, 3 buffer alloc*/
		}
	}
#endif
	return buffer_size;
}

unsigned int mdomain_input_fast_flag[MAX_DISP_CHANNEL_NUM] = {0};
void check_and_set_mdomain_input_fast(unsigned char display)
{
    mdomain_input_fast_flag[display] = 0;
#ifndef BUILD_QUICK_SHOW
    if(display == SLR_MAIN_DISPLAY)
    {
        // Input source frame rate faster than display (main_frc_style = 1)
        // M-display IVS refer to front of VGIP signal (for HDMI FP 3D, IVS refer to the source v-sync)
        if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) > ( Get_DISPLAY_REFRESH_RATE() * 1000 + 500 ) ) &&
            (Get_vscFilmMode() == FALSE) && !get_mdomain_1p_for_hdmi_4k120_flag() && !get_mdomain_1p_for_4k60_vrr_flag() )
        {
            mdomain_input_fast_flag[display] = 1;
            rtd_pr_scaler_memory_info("main input fast case\n");
        }
    
        if(Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_FRAMESYNC) == FALSE)
        {
            if(Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE)
            {
                drv_memory_display_check_sub_frc_style();
                if(mdomain_input_fast_flag[SLR_SUB_DISPLAY])
                    drv_memory_display_set_input_fast(SLR_SUB_DISPLAY, TRUE);
                else
                    drv_memory_display_set_input_fast(SLR_SUB_DISPLAY, FALSE);
            }
        }
    }
    else
    {
        drv_memory_display_check_sub_frc_style();
    }
#endif

    if((FALSE ==Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_FRAMESYNC)) && (mdomain_input_fast_flag[display]))
    {
        drv_memory_display_set_input_fast(display, TRUE);
        mdomain_input_fast_flag[display] = 0;
    }
    else
    {
        drv_memory_display_set_input_fast(display, FALSE);
    }
}

bool get_mdomain_input_fast_status(unsigned char display)
{
	bool input_fast_flag = FALSE;

	if(display == SLR_MAIN_DISPLAY)
	{
		input_fast_flag = MDOMAIN_CAP_Cap0_buf_control_get_cap0_input_fast(IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg));
	}
	else if(display == SLR_SUB_DISPLAY)
	{
		input_fast_flag = MDOM_SUB_CAP_Cap1_buf_control_get_cap1_input_fast(IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg));
	}

	rtd_pr_scaler_memory_info("[%s] %d display=%d, input_fast=%d\n", __FUNCTION__, __LINE__, display, input_fast_flag);

	return input_fast_flag;
}

void drv_memory_wait_disp_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case)
{
	//game_mode_case means call by game mode flow
	unsigned int count;
	unsigned char pre_disp_idx;
	unsigned char cur_disp_idx;

	if(display == SLR_MAIN_DISPLAY)
	{
		do
		{
			if((0 == MDOMAIN_DISP_FRC_disp0_buffer_select_control_get_disp0_buf_num(IoReg_Read32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg))) || (1 == MDOMAIN_DISP_FRC_disp0_buffer_select_control_get_disp0_buf_force_en(IoReg_Read32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg))))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}
			if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
			{
				rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
				return;
			}
			count = 40;
			pre_disp_idx = MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg));
			//rtd_pr_scaler_memory_info("[%s] %d M domain main done-0, disp pre idx = %d\n", __FUNCTION__, __LINE__, pre_disp_idx);

			while((MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg))) && --count)
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_disp_idx = MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain main done-1, disp pre_index=%d, new_idx=%d, count=%d, mdisp_apply=%d\n", __FUNCTION__, __LINE__, pre_disp_idx, cur_disp_idx, count, (MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg))));

			if(count == 0)
			{
#ifdef BUILD_QUICK_SHOW
				printf2("M domain main timeout-1 !\n");
#else
				rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-1 !\n", __FUNCTION__, __LINE__);
#endif
			}

			// [KTASKWBS-5316] wait M-display write done if display block index == 0
			// if(MDOMAIN_DISP_DDR_MainStatus_get_main_block_select(IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg)) == pre_disp_idx)
			if(cur_disp_idx == pre_disp_idx)
			{
				count = 40;

				while((MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg)) == pre_disp_idx) && --count)
				{
					if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
					{
						rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
						return;
					}
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_disp_idx = MDOMAIN_DISP_disp0_buf_status_0_get_disp0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain main done-2(check again), disp new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_disp_idx, count);
				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain main timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;
		}while(wait_frame != 0);
	}
	else
	{
		do
		{
			if ((0 == MDOM_SUB_DISP_FRC_disp1_buffer_select_control_get_disp1_buf_num(IoReg_Read32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg))) || (1 == MDOM_SUB_DISP_FRC_disp1_buffer_select_control_get_disp1_buf_force_en(IoReg_Read32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg))))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}

			count = 40;
			pre_disp_idx = MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg));

			while((MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg))) && --count)
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_disp_idx = MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain sub done-1, disp pre_index=%d, new_idx=%d, count=%d, mdisp_apply=%d\n", __FUNCTION__, __LINE__, pre_disp_idx, cur_disp_idx, count, (MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg))));

			if(count == 0)
				rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-1 !\n", __FUNCTION__, __LINE__);

			// [KTASKWBS-5316] wait M-display write done if display block index == 0
			// if(MDOMAIN_DISP_DDR_SubStatus_get_sub_block_select(IoReg_Read32(MDOMAIN_DISP_DDR_SubStatus_reg)) == pre_disp_idx)
			if(cur_disp_idx == pre_disp_idx)
			{
				count = 40;

				while((MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg)) == pre_disp_idx) && --count)
				{
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_disp_idx = MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain sub done-2(check again), disp new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_disp_idx, count);

				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain sub timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;
		}while(wait_frame != 0);
	}

}

void drv_memory_wait_cap_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case)
{
	//game_mode_case means call by game mode flow
	mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;
    mdom_sub_cap_cap1_ddr_fifostatus_RBUS mdom_sub_cap_cap1_ddr_fifostatus_reg;
	unsigned int count;
	unsigned char pre_cap_idx;
	unsigned char cur_cap_idx;

	//Add input fast case. When input dast case, need to wait Mdisp write done. (Not wait Mcap write done)
	if(get_mdomain_input_fast_status(display) == TRUE)
	{
		rtd_pr_scaler_memory_info("[%s] %d input fast case, wait disp last write done\n", __FUNCTION__, __LINE__);
        //input fast case need wait less than 2 frame
        if(wait_frame < 2)
        {
            wait_frame = 2;
        }

		drv_memory_wait_disp_last_write_done(display, wait_frame, game_mode_case);
		return;
	}

	if(display == SLR_MAIN_DISPLAY)
	{
		do
		{
			if(0 == MDOMAIN_CAP_Cap0_buf_control_get_cap0_buf_num(IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg)))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}
			if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
			{
				rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
				return;
			}
			count = 40;
			while(count && (MDOMAIN_CAP_Cap0_DDR_FIFOStatus_get_cap0_first_wr_flag(IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg)) == 0))
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
				count --;
			}

			pre_cap_idx = MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg));

			rtd_pr_scaler_memory_info("[%s] %d main wait first write finish count=%d, pre_cap_idx=%d \n", __FUNCTION__, __LINE__, count, pre_cap_idx);

			count = 40;
			//rtd_pr_scaler_memory_info("[%s] %d M domain main done-0, cap pre idx = %d\n", __FUNCTION__, __LINE__, pre_cap_idx);

			/*check memory cap down*/
			IoReg_Write32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg));

			while((MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_en(IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg))) && (MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg))) && --count)
			{
                if ((MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_get_cap0_dma_enable(IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg)) == 0) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
                {
                    rtd_pr_scaler_memory_notice("%s m cap is disable, return!", __FUNCTION__);
                    return;
                }
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_cap_idx = MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain main done-1, cap pre_index=%d, new_id x=%d, count=%d, mdisp_db_apply=%d,mdisp_db_en=%d\n", __FUNCTION__, __LINE__, pre_cap_idx, cur_cap_idx, count, (MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_apply(IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg))),(MDOMAIN_DISP_Disp0_db_ctrl_get_disp0_db_en(IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg))));

			if(count == 0)
			{
#ifdef BUILD_QUICK_SHOW
				printf2("M domain main timeout-1 !\n");
#else
				rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-1 !\n", __FUNCTION__, __LINE__);
#endif
			}

			// [KTASKWBS-5316] wait M-capture write done if capure block index == 0
			if(cur_cap_idx == pre_cap_idx)
			{
				count = 40;
				/*check memory cap down*/
				mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
				mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
				IoReg_Write32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);

				while((MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg)) == pre_cap_idx) && --count)
				{
					if(game_mode_case && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
					{
						rtd_pr_scaler_memory_info("[%s] %d game mode case not active return\n", __FUNCTION__, __LINE__);
						return;
					}
                    if ((MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_get_cap0_dma_enable(IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg)) == 0) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) != _MODE_STATE_ACTIVE))
                    {
                        rtd_pr_scaler_memory_notice("%s m cap is disable, return!", __FUNCTION__);
                        return;
                    }
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_cap_idx = MDOMAIN_DISP_disp0_buf_status_0_get_cap0_buf_index_cur(IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain main done-2(check again), cap new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_cap_idx, count);
				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain main timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain main timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;

			//if wait_frame > 0, clear again
			if(wait_frame > 0)
			{
                mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
				mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
				IoReg_Write32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);
			}
		}while(wait_frame != 0);
	}
	else
	{
		do
		{
			if(0 == MDOM_SUB_CAP_Cap1_buf_control_get_cap1_buf_num(IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg)))
			{
				rtd_pr_scaler_memory_info("[%s(%d)] single buffer no need wait block status\n", __FUNCTION__, __LINE__);
				return;
			}

			count = 40;
			while(count && (MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_get_cap1_first_wr_flag(IoReg_Read32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg)) == 0))
			{
				msleep(0);//no busy delay 10 ms
				count --;
			}
			pre_cap_idx = MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg));

			rtd_pr_scaler_memory_info("[%s] %d sub wait first write finish count=%d, pre_cap_idx=%d \n", __FUNCTION__, __LINE__, count, pre_cap_idx);

			count = 40;

			/*check memory cap down*/
			//clear last write flag before wait cap index jump
			mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
			mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
			IoReg_Write32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);

			while((MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg))) && --count)
			{
#ifdef BUILD_QUICK_SHOW
				msleep(10);//no busy delay 10 ms
#else
				msleep(0);//no busy delay 10 ms
#endif
			}

			cur_cap_idx = MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg));
			rtd_pr_scaler_memory_info("[%s] %d M domain sub done-1, cap pre_index=%d, new_idx=%d, count=%d, mdisp_apply=%d\n", __FUNCTION__, __LINE__, pre_cap_idx, cur_cap_idx, count, (MDOM_SUB_DISP_Disp1_db_ctrl_get_disp1_db_apply(IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg))));

			if(count == 0)
				rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-1 !\n", __FUNCTION__, __LINE__);

			// [KTASKWBS-5316] wait M-capture write done if capure block index == 0
			// if(MDOMAIN_CAP_DDR_In2Status_get_in2_block_select(IoReg_Read32(MDOMAIN_CAP_DDR_In2Status_reg)) == pre_cap_idx)
			if(cur_cap_idx == pre_cap_idx)
			{
				count = 40;
				/*check memory cap down*/
                mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
                mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
                IoReg_Write32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);

				while((MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg)) == pre_cap_idx) && --count)
				{
#ifdef BUILD_QUICK_SHOW
					msleep(10);//no busy delay 10 ms
#else
					msleep(0);//no busy delay 10 ms
#endif
				}
				cur_cap_idx = MDOM_SUB_DISP_disp1_buf_status_0_get_cap1_buf_index_cur(IoReg_Read32(MDOM_SUB_DISP_disp1_buf_status_0_reg));
				rtd_pr_scaler_memory_info("[%s] %d M domain sub done-2(check again), cap new_idx=%d, count=%d\n", __FUNCTION__, __LINE__, cur_cap_idx, count);

				if(count == 0)
				{
#ifdef BUILD_QUICK_SHOW
					printf2("M domain sub timeout-2 !\n");
#else
					rtd_pr_scaler_memory_err("[%s] %d M domain sub timeout-2 !\n", __FUNCTION__, __LINE__);
#endif
				}
			}
			wait_frame--;
		}while(wait_frame != 0);
	}
}


//==============Gamd mode==============//


extern unsigned char Get_Factory_SelfDiagnosis_Mode(void);
extern unsigned char get_AVD_Input_Source(void);
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
	return (get_vsc_src_is_hdmi_or_dp(SLR_MAIN_DISPLAY)) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag());
}

unsigned char drv_memory_get_low_delay_game_mode_dynamic(void)
{
	unsigned char low_delay_game =FALSE;

	low_delay_game = (drv_memory_get_game_mode_dynamic() || is_hdmi_vrr_or_freesync()) && !get_scaler_qms_mode_flag();
	return low_delay_game;
}

void drv_memory_set_game_mode(unsigned char enable)
{
	enable_M_GameMode = enable;
#ifndef BUILD_QUICK_SHOW
	drv_update_game_mode_frc_fs_flag();
#endif
}


//only get game mode flag, no need to check input source & conditions.
unsigned char drv_memory_get_game_mode_flag(void)
{
	return enable_M_GameMode;
}

unsigned char drv_memory_get_game_mode(void){
    //Update the game mode condition to game mode status driver @Crixus 20180825
    if(get_new_game_mode_condition())
        return enable_M_GameMode;
    else
        return FALSE;
}

/*for livezoom and magnifier game mode keep i3ddma single buffer*/
unsigned char drv_I3ddmaMemory_get_game_mode(void){
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



unsigned char drv_memory_get_game_mode_iv2dv_slow_enable(void){
#ifdef BUILD_QUICK_SHOW
	return FALSE;
#else
	/*Always use iv2dv slow tuning flow*/
	if(Get_DISPLAY_PANEL_OLED_TYPE()== TRUE)
		return TRUE;

	return TRUE;
#endif
}
extern unsigned char get_vo_camera_flow_flag(unsigned char ch);
void drv_memory_gamemode_sub_switch_single_buffer(unsigned char enable)
{//enable: true = single   false: triple  4k120 case
    unsigned int timeout = 0x3ffff;
    mdom_sub_cap_cap1_reg_doublbuffer_RBUS mdom_sub_cap_cap1_reg_doublbuffer_reg;
    mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
    mdom_sub_cap_cap1_buf_control_RBUS mdom_sub_cap_cap1_buf_control_reg;
    mdom_sub_disp_frc_disp1_buffer_select_control_RBUS mdom_sub_disp_frc_disp1_buffer_select_control_reg;
    mdom_sub_cap_cap1_ddr_fifostatus_RBUS mdom_sub_cap_cap1_ddr_fifostatus_reg;
    mdom_sub_cap_cap1_buffer_change_line_num_0_RBUS mdom_sub_cap_cap1_buffer_change_line_num_0_reg;

    UINT8 cap_db_en = 0;
    unsigned int remove = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_REMOVE_RATIO);
    unsigned int multiple = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_MULTIPLE_RATIO);
    if(enable)
    {//one buffer
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        cap_db_en = mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en;
        mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 1;
        IoReg_Write32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);

        //enable M-disp double buffer
        mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        //disp_db_en = mdom_sub_disp_disp1_db_ctrl_reg.disp1_double_enable;
        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en = 1;
        IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

    /////////////////////////////////////////////////////////////////////////////////////////
        timeout = 0x3ffff;

        //first change M-disp.
        if (remove < multiple)
        {
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_distance = 0;
            IoReg_Write32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);
        } else {
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_en = 1;
            mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_addr = 2;
            IoReg_Write32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);

        }

        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
        IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        while((mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply == 1) && (timeout--)){
            mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        }

        timeout = 0x3ffff;

        if(!get_hdmi_vrr_4k60_mode()){
            //change M-cap
            if (remove < multiple)
            {
                mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg);
                mdom_sub_cap_cap1_buf_control_reg.cap1_buf_change_mode = 3;
                IoReg_Write32(MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

                mdom_sub_cap_cap1_buffer_change_line_num_0_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_cap1_buffer_change_line_num_0_reg);
                mdom_sub_cap_cap1_buffer_change_line_num_0_reg.cap1_buf_change_line_num_sw = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN) * (multiple - remove) / multiple;
                IoReg_Write32(MDOM_SUB_CAP_cap1_buffer_change_line_num_0_reg, mdom_sub_cap_cap1_buffer_change_line_num_0_reg.regValue);
            } else {
                mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg);
                mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_en = 1;
                mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_addr = 2;
                IoReg_Write32(MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);
            }

            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply = 1;
            IoReg_Write32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
            while((mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply == 1) && (timeout--)){
                mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
            }

        }
        rtd_pr_new_game_mode_notice("[Game Mode]4k120 sub M-domain game mode enable!!\n");

        /////////////////////////////////////////////////////////////////////////////////////////

        //resume M-cap double buffer setting
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = rtd_inl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 0;
        } else {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = cap_db_en;
        }
        IoReg_Write32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg, mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
    }
    else
    {//three buffer
        timeout = 0x3ffff;
        //enable M-cap double buffer
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = rtd_inl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        cap_db_en = mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en;
        mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 1;
        IoReg_Write32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg,mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);

        //enable M-disp double buffer
        mdom_sub_disp_disp1_db_ctrl_reg.regValue = rtd_inl(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        //disp_db_en = ddr_mainsubctrl_reg.disp1_double_enable;
        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en = 1;
        IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg, mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        /////////////////////////////////////////////////////////////////////////////////////////

        if(!get_hdmi_vrr_4k60_mode()){
            //first set M-cap to triple buffer
            mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg);
            mdom_sub_cap_cap1_buf_control_reg.cap1_buf_change_mode = 0;
            mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_en = 0;
            mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_addr = 0;
            IoReg_Write32(MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply = 1;
            IoReg_Write32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg, mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
            while((mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_apply == 1) && (timeout--)){
                mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
            }
        }

#if 0//for debug
        if(index_check_flag){
            mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
            mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
            rtd_pr_scaler_memory_emerg("[Game Mode]2.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
        }
#endif

        timeout = 0x3ffff;

        //wait M-cap write latest 1 frame.
        /*check memory cap down*/
        mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
        IoReg_Write32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);

        mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        while(!(mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end) && --timeout){
            mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        }

#if 0//for debug
        if(index_check_flag){
            mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
            mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
            rtd_pr_scaler_memory_emerg("[Game Mode]3.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
        }
#endif

        timeout = 0x3ffff;

        //change M-disp.
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_distance = 1;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_en = 0;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_addr = 0;
        IoReg_Write32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);


        mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
        IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        while((mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply == 1) && (timeout--)){
            mdom_sub_disp_disp1_db_ctrl_reg.regValue =IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        }

#if 0//for debug
        if(index_check_flag){
            mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
            mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
            rtd_pr_scaler_memory_emerg("[Game Mode]4.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
        }
#endif
        //if(Get_DISPLAY_PANEL_OLED_TYPE()== TRUE){
        //  msleep(100);//wait M-cap write latest 2 frame.
        //}

        rtd_pr_new_game_mode_notice("[Game Mode]4k120 sub M-domain game mode disable!!\n");

        /////////////////////////////////////////////////////////////////////////////////////////
        //resume M-cap double buffer setting
        mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue = rtd_inl(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg);
        if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = 0;
        } else {
            mdom_sub_cap_cap1_reg_doublbuffer_reg.cap1_db_en = cap_db_en;
        }
        IoReg_Write32(MDOM_SUB_CAP_cap1_reg_doublbuffer_reg, mdom_sub_cap_cap1_reg_doublbuffer_reg.regValue);
    }

}

void drv_memory_GameMode_Switch_SingleBuffer(void){
	unsigned int timeout = 0x3ffff;
	mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;
	mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
//	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    mdomain_cap_cap0_buffer_change_line_num_0_RBUS mdomain_cap_cap0_buffer_change_line_num_0_reg;
	mdomain_disp_frc_disp0_buffer_select_control_RBUS mdomain_disp_frc_disp0_buffer_select_control_reg;

	UINT8 cap_db_en = 0;
    unsigned int remove = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_REMOVE_RATIO);
    unsigned int multiple = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAC_FS_MULTIPLE_RATIO);
	//UINT8 disp_db_en = 0;
	//UINT8 display_dtg_db_en = 0;

	//enable M-cap double buffer
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	cap_db_en = mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en;
	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 1;
	IoReg_Write32(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);

	//enable M-disp double buffer
	mdomain_disp_disp0_db_ctrl_reg.regValue = rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
	//disp_db_en = ddr_mainsubctrl_reg.disp1_double_enable;
	mdomain_disp_disp0_db_ctrl_reg.disp0_db_en = 1;
	IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);

/////////////////////////////////////////////////////////////////////////////////////////
	timeout = 0x3ffff;

	//first change M-disp.
    if (remove < multiple)
    {
        mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_distance = 0;
        IoReg_Write32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);
    }
    else {
        mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_en = 1;
        if(get_vo_camera_flow_flag(Get_DisplayMode_Port(SLR_MAIN_DISPLAY))==1)
            mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 0;
        else
            mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 2;
        IoReg_Write32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);
    }


	mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
	IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
	while((mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply == 1) && (timeout--)){
		mdomain_disp_disp0_db_ctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
	}

	timeout = 0x3ffff;

	//change M-cap
    if (remove < multiple)
    {
        mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_change_mode = 3;
        IoReg_Write32(MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

        mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg);
        mdomain_cap_cap0_buffer_change_line_num_0_reg.cap0_buf_change_line_num_sw = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN) * (multiple - remove) / multiple;
        IoReg_Write32(MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg, mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue);
    } else {
        mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 1;
        if(get_vo_camera_flow_flag(Get_DisplayMode_Port(SLR_MAIN_DISPLAY))==1)
            mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 0;
        else
            mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 2;
        IoReg_Write32(MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);
    }

	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply = 1;
	IoReg_Write32(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);
	while((mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply == 1) && (timeout--)){
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue = rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	}
	rtd_pr_new_game_mode_notice("[Game Mode]M-domain game mode enable!!\n");

	/////////////////////////////////////////////////////////////////////////////////////////

	//resume M-cap double buffer setting
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 0;
	} else {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = cap_db_en;
	}
	IoReg_Write32(MDOMAIN_CAP_cap0_reg_doublbuffer_reg, mdomain_cap_cap0_reg_doublbuffer_reg.regValue);



}
#ifndef BUILD_QUICK_SHOW

void drv_memory_debug_print_buf_idx(void)
{
#if 0
	static unsigned char capIdx_pre = 0xf;
	static unsigned char dispIdx_pre = 0xf;
	mdomain_cap_ddr_in1status_RBUS mdomain_cap_ddr_in1status_reg;
	mdomain_disp_ddr_mainstatus_RBUS mdomain_disp_ddr_mainstatus_reg;
	vgip_vgip_chn1_lc_RBUS vgip_chn1_lc_reg;
	ppoverlay_new_meas1_linecnt_real_RBUS new_meas1_linecnt_real_reg;

	mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
	mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
	vgip_chn1_lc_reg.regValue = IoReg_Read32(VGIP_VGIP_CHN1_LC_reg);
	new_meas1_linecnt_real_reg.regValue = IoReg_Read32(PPOVERLAY_new_meas1_linecnt_real_reg);

	rtd_pr_scaler_memory_notice("[M-BUF-IDX] cap idx = %d, disp idx = %d (%d)\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select,
		IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
	rtd_pr_scaler_memory_notice("vgip line_cnt %d, uzu line_cnt (%d)\n", vgip_chn1_lc_reg.ch1_line_cnt, new_meas1_linecnt_real_reg.uzudtg_line_cnt_rt);

	if((capIdx_pre != 0xff)||(dispIdx_pre != 0xff)){
		// m domain cap-check 1
		if((mdomain_cap_ddr_in1status_reg.in1_block_select == 0)&&(capIdx_pre == 2)){
			// ok case
		}
		else if((mdomain_cap_ddr_in1status_reg.in1_block_select - capIdx_pre)!=1){
			rtd_pr_scaler_memory_notice("[M-BUF-IDX-CHECK-ERR1] cap idx = %d, pre cap-idx = %d (%d)\n", mdomain_cap_ddr_in1status_reg.in1_block_select, capIdx_pre,
				IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
		}else{
			//rtd_pr_scaler_memory_notice("[M-BUF-IDX] cap check OK. \n");
		}
		// m domain cap-check 2
		if((mdomain_disp_ddr_mainstatus_reg.main_block_select== 0)&&(dispIdx_pre == 1)){
			rtd_pr_scaler_memory_notice("[M-BUF-IDX-CHECK-ERR2] disp idx = %d, pre disp idx = %d (%d)\n", mdomain_disp_ddr_mainstatus_reg.main_block_select, dispIdx_pre,
				IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
		}
		else if((mdomain_disp_ddr_mainstatus_reg.main_block_select == 1)&&(dispIdx_pre == 2)){
			rtd_pr_scaler_memory_notice("[M-BUF-IDX-CHECK-ERR2] disp idx = %d, pre disp idx = %d (%d)\n", mdomain_disp_ddr_mainstatus_reg.main_block_select, dispIdx_pre,
				IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
		}else if((mdomain_disp_ddr_mainstatus_reg.main_block_select == 2)&&(dispIdx_pre == 0)){
			rtd_pr_scaler_memory_notice("[M-BUF-IDX-CHECK-ERR2] disp idx = %d, pre disp idx = %d (%d)\n", mdomain_disp_ddr_mainstatus_reg.main_block_select, dispIdx_pre,
				IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg));
		}
		else{
			//rtd_pr_scaler_memory_notice("[M-BUF-IDX] disp check OK. \n");
		}
	}
	capIdx_pre= mdomain_cap_ddr_in1status_reg.in1_block_select;
	dispIdx_pre= mdomain_disp_ddr_mainstatus_reg.main_block_select;
#endif
}

void drv_memory_GameMode_Switch_TripleBuffer(void){
	unsigned int timeout = 0x3ffff;
	mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;
	mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
//	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
	mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    mdomain_disp_frc_disp0_buffer_select_control_RBUS mdomain_disp_frc_disp0_buffer_select_control_reg;
	mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;

#if 0//for debug
	mdomain_cap_ddr_in1status_RBUS mdomain_cap_ddr_in1status_reg;
	mdomain_disp_ddr_mainstatus_RBUS mdomain_disp_ddr_mainstatus_reg;
	UINT8 index_check_flag = 0;//for debug
#endif

	UINT8 cap_db_en = 0;
	//UINT8 disp_db_en = 0;
	//UINT8 display_dtg_db_en = 0;
	//unsigned int count = 0x3fffff;

	//enable M-cap double buffer
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	cap_db_en = mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en;
	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 1;
	IoReg_Write32(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);

	//enable M-disp double buffer
	mdomain_disp_disp0_db_ctrl_reg.regValue = rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
	//disp_db_en = ddr_mainsubctrl_reg.disp1_double_enable;
	mdomain_disp_disp0_db_ctrl_reg.disp0_db_en  = 1;
	IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]1.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif
	/////////////////////////////////////////////////////////////////////////////////////////

	//first set M-cap to triple buffer
	mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);
	mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 0;
    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 0;
    mdomain_cap_cap0_buf_control_reg.cap0_buf_change_mode = 0;
	IoReg_Write32(MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

	mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply = 1;
	IoReg_Write32(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);
	while((mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_apply == 1) && (timeout--)){
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue = rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	}

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]2.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif

	timeout = 0x3ffff;

	//wait M-cap write latest 1 frame.
    /*check memory cap down*/
	mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
    mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
    IoReg_Write32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);

	mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
	while(!(mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end) && --timeout){
		mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
	}

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]3.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif

	timeout = 0x3ffff;

	//change M-disp.
	mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
    mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_distance = 1;
    mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_en = 0;
    mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 0;
    IoReg_Write32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);


	mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply  = 1;
	IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
	while((mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply == 1) && (timeout--)){
		mdomain_disp_disp0_db_ctrl_reg.regValue =rtd_inl(MDOMAIN_DISP_Disp0_db_ctrl_reg);
	}

#if 0//for debug
	if(index_check_flag){
		mdomain_cap_ddr_in1status_reg.regValue = IoReg_Read32(MDOMAIN_CAP_DDR_In1Status_reg);
		mdomain_disp_ddr_mainstatus_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainStatus_reg);
		rtd_pr_scaler_memory_emerg("[Game Mode]4.cap idx = %d, disp idx = %d\n", mdomain_cap_ddr_in1status_reg.in1_block_select, mdomain_disp_ddr_mainstatus_reg.main_block_select);
	}
#endif
	//if(Get_DISPLAY_PANEL_OLED_TYPE()== TRUE){
	//	msleep(100);//wait M-cap write latest 2 frame.
	//}

	rtd_pr_new_game_mode_notice("[Game Mode]M-domain game mode disable!!\n");

	/////////////////////////////////////////////////////////////////////////////////////////
	//resume M-cap double buffer setting
	mdomain_cap_cap0_reg_doublbuffer_reg.regValue = rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_SEARCH) {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 0;
	} else {
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = cap_db_en;
	}
	IoReg_Write32(MDOMAIN_CAP_cap0_reg_doublbuffer_reg, mdomain_cap_cap0_reg_doublbuffer_reg.regValue);



}

void dvrif_LowDelay_memory_handler(unsigned char b_switchToSingleBuffer)
{
	UINT8 display = Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY);

	if (display== SLR_MAIN_DISPLAY)
	{
		rtd_pr_scaler_memory_debug("dvrif_smoothtoggle_memory_handler = %d \n", b_switchToSingleBuffer);
		if(b_switchToSingleBuffer) {
			drv_I3ddmaVodma_SingleBuffer_GameMode(_ENABLE);
			if(!((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI)))
				drv_memory_GameMode_Switch_SingleBuffer();
		}else{
			drv_I3ddmaVodma_SingleBuffer_GameMode(_DISABLE);
			if(!((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI)))
				drv_memory_GameMode_Switch_TripleBuffer();
		}
	}
}

void drv_game_mode_disp_smooth_variable_setting(UINT8 disp_type)
{
//	ppoverlay_dispd_stage1_sm_ctrl_RBUS dispd_stage1_sm_ctrl_reg;
//	ppoverlay_dispd_smooth_toggle1_RBUS dispd_smooth_toggle1_reg;

	rtd_pr_scaler_memory_debug("[Game Mode]scaler_disp_smooth_variable_setting\n");
#if 0
	dispd_stage1_sm_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_stage1_sm_ctrl_reg);
	dispd_stage1_sm_ctrl_reg.dispd_stage1_smooth_toggle_protect = 1;
	IoReg_Write32(PPOVERLAY_dispd_stage1_sm_ctrl_reg, dispd_stage1_sm_ctrl_reg.regValue);

	drivf_scaler_reset_freerun(DTG_MASTER);
	dispd_smooth_toggle1_reg.dispd_stage1_smooth_toggle_apply=1;
	IoReg_Write32(PPOVERLAY_dispd_smooth_toggle1_reg, dispd_smooth_toggle1_reg.regValue);
	//modestate_set_fractional_framesync();
	fw_scaler_dtg_double_buffer_apply();

	dispd_stage1_sm_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_dispd_stage1_sm_ctrl_reg);
	dispd_stage1_sm_ctrl_reg.dispd_stage1_smooth_toggle_protect = 0;
	IoReg_Write32(PPOVERLAY_dispd_stage1_sm_ctrl_reg, dispd_stage1_sm_ctrl_reg.regValue);
#endif
	if(disp_type == GAME_MODE_FRAME_SYNC){
		//if(Get_DISPLAY_PANEL_OLED_TYPE()==TRUE)
		//	modestate_set_fll_running_flag(_DISABLE);
		//else{
			modestate_set_fll_running_flag(_DISABLE);//Need to disable FLL flag first.
            drvif_scaler_set_display_mode(DISPLAY_MODE_FRAME_SYNC, scaler_dtg_get_app_type());
			Scaler_Set_Display_Timing_Mode(DISPLAY_MODE_FRAME_SYNC);
		//}
	}else{
		modestate_set_fll_running_flag(_ENABLE);
		Scaler_Set_Display_Timing_Mode(DISPLAY_MODE_NEW_FLL_MODE);
	}

	fw_scaler_dtg_double_buffer_apply();
}

unsigned int drv_GetIv2DvDelayTarget(void)
{
	return id2dv_delay_target;
}

void drv_SetIv2DvDelayTarget(unsigned int a_ulIv2Dv)
{
	id2dv_delay_target = a_ulIv2Dv;
}

void drv_run_GameMode_iv2dv_slow_tuning(void)
{
	ppoverlay_fs_iv_dv_fine_tuning5_RBUS fs_iv_dv_fine_tuning5_reg;
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	ppoverlay_fs_iv_dv_fine_tuning1_RBUS tuning1_reg;
	ppoverlay_iv2dv_double_buffer_ctrl_RBUS iv2dv_double_buffer_ctrl_reg;
	UINT32 id2dv_delay = 0;
	UINT32 timeout = 10;
	UINT32 iv2dv_db_vsync_sel;


	if(id2dv_delay_target > 1){

		display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
		if(display_timing_ctrl1_reg.disp_frc_fsync==0){
			rtd_pr_new_game_mode_notice("[Game Mode]fsync is not ready\n");
			return;
		}
		//enable iv2dv toggle_en
		tuning1_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning1_reg);
		tuning1_reg.iv2dv_toggle_en = _ENABLE;
		IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning1_reg, tuning1_reg.regValue);

		fs_iv_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
		id2dv_delay = fs_iv_dv_fine_tuning5_reg.iv2dv_line;

		if(id2dv_delay == 1)
			id2dv_delay=0;

		if(id2dv_delay < id2dv_delay_target){
			if((id2dv_delay_target-id2dv_delay)<16)
				id2dv_delay += (id2dv_delay_target-id2dv_delay);
			else
				id2dv_delay += 16;
			iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
//			IoReg_SetBits(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg,_BIT2);
			iv2dv_db_vsync_sel = iv2dv_double_buffer_ctrl_reg.iv2dv_db_vsync_sel;
			iv2dv_double_buffer_ctrl_reg.iv2dv_db_vsync_sel = 1;
			iv2dv_double_buffer_ctrl_reg.iv2dv_db_en = 1;
			IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, iv2dv_double_buffer_ctrl_reg.regValue);

			fs_iv_dv_fine_tuning5_reg.iv2dv_line = id2dv_delay;
			fs_iv_dv_fine_tuning5_reg.iv2dv_line_2 = id2dv_delay;
			IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, fs_iv_dv_fine_tuning5_reg.regValue);

//			IoReg_SetBits(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg,_BIT0);
			iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply = 1;
			IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, iv2dv_double_buffer_ctrl_reg.regValue);

			iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
			while(iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply &&--timeout){
				msleep(10);
				iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
			}

//			IoReg_ClearBits(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg,_BIT2);
			iv2dv_double_buffer_ctrl_reg.iv2dv_db_vsync_sel = iv2dv_db_vsync_sel;
			iv2dv_double_buffer_ctrl_reg.iv2dv_db_en = 0;
			IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, iv2dv_double_buffer_ctrl_reg.regValue);

			rtd_pr_new_game_mode_notice("[Game Mode] iv2dv line = %d\n", PPOVERLAY_FS_IV_DV_Fine_Tuning5_get_iv2dv_line(IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg)));
			return;
		}
		else{
			//change to 1-buffer after iv2dv done @Crixus 20170527
			if(!Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC))
			{
				if(!((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI))){
					drv_memory_GameMode_Switch_SingleBuffer();
					rtd_pr_scaler_memory_emerg("[Game Mode][%s]Switch single buffer\n", __FUNCTION__);
				}
			}
			rtd_pr_scaler_memory_emerg("[Game Mode] slow tuning iv2dv done\n");
		}

	}
	DbgSclrFlgTkr.run_iv2dv_slow_tuning_flag = FALSE;
}
#endif




#define IDOMAIN_IP_DELAY_MARGIN 10
#define MDOMAIN_COMPRESSION_MARGIN 1	//DMA latency
#define SRNN_DELAY 10
//#define I3DDMA_DRAM_MARGIN 10

unsigned int drv_GameMode_iv2dv_delay_compress_margin(unsigned int iv2dv_ori){
	scaledown_ich1_sdnr_cutout_range_hor_RBUS scaledown_ich1_sdnr_cutout_range_hor_reg;
	unsigned int active_size = 0;
	scaledown_ich1_sdnr_cutout_range_hor_reg.regValue = IoReg_Read32(SCALEDOWN_ich1_sdnr_cutout_range_hor_reg);
	active_size = scaledown_ich1_sdnr_cutout_range_hor_reg.hor_width;//merlin4 sdnr size use active size.

	//margin = (fifo depth * burst length) / compress bit / den size
	iv2dv_ori = iv2dv_ori + ((_FIFOLENGTH * _WDMA_BURSTLENGTH / dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY)) / active_size) + MDOMAIN_COMPRESSION_MARGIN;
	//rtd_pr_scaler_memory_emerg("[Game Mode]margin = %d\n", ((_FIFOLENGTH * _BURSTLENGTH / dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY)) / active_size + MDOMAIN_COMPRESSION_MARGIN));

	return iv2dv_ori;
}

unsigned int drv_GameMode_get_snr_vstart(void)
{
	unsigned int snr_result1 = 0;

	nr_snr_timing_monitor_ctrl_RBUS nr_snr_timing_monitor_ctrl_reg;
	unsigned int timeout = 20;
	//Set SPNR timing monitor
	//Set 1 : measure back porch, vs rising to vact rising
	nr_snr_timing_monitor_ctrl_reg.regValue = IoReg_Read32(NR_SNR_Timing_monitor_ctrl_reg);
	if(nr_snr_timing_monitor_ctrl_reg.timing_monitor_en == 0){
		nr_snr_timing_monitor_ctrl_reg.timing_monitor_src = 1;	//src output
		nr_snr_timing_monitor_ctrl_reg.timing_monitor_unit = 0;	//unit clk cycle
		nr_snr_timing_monitor_ctrl_reg.timing_monitor_multi_mode = 1;	//multi target. 6 target with Result x 1.
		nr_snr_timing_monitor_ctrl_reg.timing_monitor_sta0 = 0;	//vs rising
		nr_snr_timing_monitor_ctrl_reg.timing_monitor_end0 = 2;	//vact rising
		nr_snr_timing_monitor_ctrl_reg.timing_monitor_en = 1;	//Timing_monitor enable
		IoReg_Write32(NR_SNR_Timing_monitor_ctrl_reg,nr_snr_timing_monitor_ctrl_reg.regValue);
		//wait for timing monitor update
		msleep(40);
	}
	snr_result1 = IoReg_Read32(NR_SNR_Timing_monitor_result1_reg);
	while(!snr_result1 && timeout){
		timeout --;
		msleep(0);
		snr_result1 = IoReg_Read32(NR_SNR_Timing_monitor_result1_reg);
	}
	if(!snr_result1)
	{
		rtd_pr_scaler_memory_err("\r### func:%s error snr_result1no result ### \r\n",__FUNCTION__);
		snr_result1 =  Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA_PRE);
		nr_snr_timing_monitor_ctrl_reg.regValue = IoReg_Read32(NR_SNR_Timing_monitor_ctrl_reg);
		nr_snr_timing_monitor_ctrl_reg.timing_monitor_en = 0;	//Timing_monitor disable
		IoReg_Write32(NR_SNR_Timing_monitor_ctrl_reg,nr_snr_timing_monitor_ctrl_reg.regValue);
	}

	return snr_result1;
}
#ifndef UT_flag
extern ORBIT_PIXEL_SHIFT_STRUCT orbit_move_range[_VBE_PANEL_OREBIT_MODE_MAX];
#endif // #ifndef UT_flag
unsigned int drv_GameMode_calculate_iv2dv_delay(void)
{
	UINT32 iv2dv_delay = 0;

/*==================================================================================
The iv2dv delay formula need match (T2 >= T1) and (T4 >= T3)
T1 = (vodma_htotal * (vodma_vsta+N)) * 1/ (vodma_clk ) ,
T2 = (iv2dv_delay  * (1/vodoma_clk)* vodma_htotal) + (ddomain_htotal * (mdisp_preRead) * 1/(ddomain_clk) ,
                                                                                                       note: ddomain Pre_read= (den_start �V 5)
T3 =  (vodma_htotal * (N+vodma_vsta+ vodma_vact)) * 1/ (vodma_clk) ,
T4 = (iv2dv_delay  * (1/vodoma_clk)* vodma_htotal ) +  (ddomain_htotal * (ddomain_vsta+ddomain_vact)) * 1/(ddomain_clk)
====================================================================================*/
	UINT32 voHtotal;
	UINT32 voVactive;
	UINT32 voVstart;
	UINT32 voClock;
	UINT32 iv2dv_delay1;
	UINT32 iv2dv_delay2;
	UINT32 dHtotal;
	UINT32 dCLK;
	UINT32 dDen_start;
	UINT32 dMv_start;
	UINT32 dMv_act;
	UINT32 mdisp_preRead;
	StructSrcRect m_dispwin;
	SLR_VOINFO *pVOInfo = NULL;
//	ppoverlay_fs_iv_dv_fine_tuning5_RBUS fs_iv_dv_fine_tuning5_reg;
	UINT32 temp1;
	UINT32 temp2;
	UINT32 temp3;
	scaledown_ich1_sdnr_cutout_range_ver0_RBUS scaledown_ich1_sdnr_cutout_range_ver0_reg;
	UINT32 orbit_shift_range;
	UINT32 orbit_front;
	UINT32 snrVstart;
	UINT32 hdmi_vfreq = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVFREQ_1000);
	UINT32 hdmi_htotal = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IHLEN);
	UINT32 hdmi_vtotal = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVLEN);
	UINT32 hdmi_vstart = Scaler_ModeGet_Hdmi_ModeInfo(SLR_MODE_IVSTARTPOS);
	UINT32 vfreq = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_FREQ_1000);
	UINT32 htotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_H_LEN);
	UINT32 vtotal = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN);
	UINT32 vstart = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA_PRE);
	//UINT32 srnn_delay_line = 0;
	//UINT32 srnn_en = SCALEUP_D_UZU_Globle_Ctrl_get_srnn_func_en_eco(IoReg_Read32(SCALEUP_D_UZU_Globle_Ctrl_reg));
	scaledown_ich1_sdnr_cutout_range_ver0_reg.regValue = IoReg_Read32(SCALEDOWN_ich1_sdnr_cutout_range_ver0_reg);
	voHtotal = Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_H_LEN);
	voVactive = scaledown_ich1_sdnr_cutout_range_ver0_reg.ver_height;//merlin4 sdnr size use active size.
	pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());

	if(!get_force_i3ddma_enable(SLR_MAIN_DISPLAY) && (Scaler_InputSrcGetType(SLR_MAIN_DISPLAY) !=_SRC_VO))
	{

		if((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) &&
			((hdmi_vfreq == 0)|| (hdmi_vtotal == 0) || (hdmi_htotal == 0)))
		{
			rtd_pr_scaler_memory_emerg("[IV2DV]Invalid HDMI Info!!!!\n");
			return 0;
		}
		else if((vfreq == 0)|| (vtotal == 0) || (htotal == 0))
		{
			rtd_pr_scaler_memory_emerg("[IV2DV]Invalid Input Info!!!!\n");
			return 0;
		}
	}
	else if((pVOInfo == NULL) || (pVOInfo->h_total == 0)){
		rtd_pr_scaler_memory_emerg("[IV2DV]Invalid VOInfo!!!!\n");
		return 0;
	}

	if(!get_force_i3ddma_enable(SLR_MAIN_DISPLAY) && (Scaler_InputSrcGetType(SLR_MAIN_DISPLAY) !=_SRC_VO))
	{
		if(Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI)
		{
			snrVstart = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA_PRE) / hdmi_htotal;
			if(snrVstart < hdmi_vstart)
				snrVstart = hdmi_vstart;
		}
		else
		{
			snrVstart = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_STA_PRE) / htotal;
			if(snrVstart < vstart)
				snrVstart = vstart;
		}
	}
	else
	{
		snrVstart = drv_GameMode_get_snr_vstart() / pVOInfo->h_total;
		if(snrVstart < pVOInfo->v_start)
			snrVstart = pVOInfo->v_start;
	}

	if((Get_DISPLAY_PANEL_OLED_TYPE() == TRUE))
	{
#ifndef UT_flag
		orbit_shift_range = orbit_move_range[_VBE_PANEL_ORBIT_STORE_MODE].y*2;
		orbit_front = ((Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_SDNRV_ACT_LEN)*orbit_shift_range) +  (orbit_shift_range+Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_DISP_LEN))/2)/(orbit_shift_range+Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_DISP_LEN)) ;
		//voVstart = snrVstart +scaledown_ich1_sdnr_cutout_range_ver0_reg.ver_front+IDOMAIN_IP_DELAY_MARGIN + orbit_front;//Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_SDNRV_ACT_STA);
		voVstart = snrVstart +scaledown_ich1_sdnr_cutout_range_ver0_reg.ver_front + orbit_front;//Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_SDNRV_ACT_STA);
#endif // #ifndef UT_flag
	}
	else
	{
		voVstart = snrVstart +scaledown_ich1_sdnr_cutout_range_ver0_reg.ver_front;//Scaler_DispGetInputInfoByDisp(0, SLR_INPUT_SDNRV_ACT_STA);
	}

	if(!get_force_i3ddma_enable(SLR_MAIN_DISPLAY) && (Scaler_InputSrcGetType(SLR_MAIN_DISPLAY) !=_SRC_VO))
	{
        if( ( Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI ) && ( ( hdmi_htotal != 0 ) && ( hdmi_vtotal != 0 ) && ( hdmi_vfreq != 0 ) ) )
        {
            voClock = (unsigned int)( (unsigned long long)hdmi_htotal * (unsigned long long)hdmi_vtotal * (unsigned long long)hdmi_vfreq / 1000ULL );
        }
        else
        {
            voClock = (unsigned int)( (unsigned long long)htotal * (unsigned long long)vtotal * (unsigned long long)vfreq / 1000ULL );
        }
	}
	else
	{
		voClock = pVOInfo->pixel_clk;
	}
	dHtotal = Get_DISP_HORIZONTAL_TOTAL();
	dCLK = Get_DISPLAY_CLOCK_TYPICAL();
	//den shift with time borrow, we consider worse case.
	dDen_start =  Get_DISP_DEN_STA_VPOS();//-_DTG_TIME_BORROW_LINE;
	m_dispwin = Scaler_DispWindowGet();
	dMv_start = m_dispwin.srcy;
	dMv_act = m_dispwin.src_height;

	/*Fix ML3RTANDN-651
		when interlace signal , game mode calculate iv2dv delay with vo activity after DI size, but using with interlace clock
		it will cause game mode can not wait for phase error and mute screen for long time.
		BenWang discuss with DIC and double VO interlace clock value can avoid wait phase error for long time*/
	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY,SLR_DISP_INTERLACE))
		voClock= voClock*2;

	if( ( get_panel_pixel_mode() > PANEL_1_PIXEL ) && PPOVERLAY_uzudtg_control1_get_uzu_4k_120_mode(rtd_inl(PPOVERLAY_uzudtg_control1_reg)) )	// 120hz panel run 2pixel mode, dCLK need multiple 2 for calculate.
		dCLK *= 2;

	mdisp_preRead = MDOMAIN_DISP_Disp0_pre_rd_ctrl_get_disp0_pre_rd_v_start(IoReg_Read32(MDOMAIN_DISP_Disp0_pre_rd_ctrl_reg));

	/* Formula
	(iv2dv_delay  * (1/vodoma_clk)* vodma_htotal) + (ddomain_htotal * (mdisp_preRead) * 1/(ddomain_clk) >=
	(vodma_htotal * (vodma_vsta+N)) * 1/ (vodma_clk )
       =>  iv2dv_delay + (ddomain_htotal/vodma_htotal)*(mdisp_preRead) * (vodoma_clk/(ddomain_clk)) >= ((vodma_vsta+N))
	*/
	//rtd_pr_scaler_memory_emerg("[ben]snrVstart = %d, voHtotal = %d, voVactive = %d, voVstart = %d ,VoClock = %d\n", snrVstart, voHtotal, voVactive, voVstart, voClock);
	//rtd_pr_scaler_memory_emerg("[ben]dHtotal = %d, dCLK = %d, mdisp_preRead = %d, IDOMAIN_IP_DELAY_MARGIN = %d\n", dHtotal, dCLK,
	//	mdisp_preRead, IDOMAIN_IP_DELAY_MARGIN);
	//rtd_pr_scaler_memory_emerg("[ben]dDen_start = %d, dMv_start = %d, dMv_act = %d\n", dDen_start, dMv_start, dMv_act);

	temp1 = (dHtotal*100000)/voHtotal; //multiple : 100000
	temp1 /= 100;	//multiple : 1000
	temp2 = (voClock/(dCLK/10000)); //multiple : 10000
	temp3= (temp1*temp2)/100; //multiple : 100000
	//rtd_pr_scaler_memory_emerg("[ben]temp1=%d,temp2=%d,temp3=%d\n", temp1,temp2,temp3);
	if((voVstart+IDOMAIN_IP_DELAY_MARGIN)*100000 > (temp3*mdisp_preRead))
		iv2dv_delay1 = (voVstart+IDOMAIN_IP_DELAY_MARGIN)*100000 - (temp3*mdisp_preRead);
	else
		iv2dv_delay1 = 0;
	iv2dv_delay1 = (iv2dv_delay1/100000)+1;
	//rtd_pr_scaler_memory_emerg("[ben]iv2dv_delay1 = %d\n", iv2dv_delay1);

	/* Formula
	(iv2dv_delay  * (1/vodoma_clk)* vodma_htotal ) +  (ddomain_htotal * (ddomain_vsta+ddomain_vact)) * 1/(ddomain_clk) >=
	(vodma_htotal * (N+vodma_vsta+ vodma_vact)) * 1/ (vodma_clk)
       => iv2dv_delay+ (ddomain_htotal/vodma_htotal) * (ddomain_vsta+ddomain_vact) * (vodoma_clk/(ddomain_clk)) >=
       (N+vodma_vsta+ vodma_vact)
	*/
	//rtd_pr_scaler_memory_emerg("[ben](voVstart+voVactive+IDOMAIN_IP_DELAY_MARGIN) = %d\n", (voVstart+voVactive+IDOMAIN_IP_DELAY_MARGIN)*100000);
	//rtd_pr_scaler_memory_emerg("[ben](dDen_start+dMv_start+dMv_act) = %d\n", (dDen_start+dMv_start+dMv_act));
	//rtd_pr_scaler_memory_emerg("[ben]temp3*(dDen_start+dMv_start+dMv_act) = %d\n", (temp3*(dDen_start+dMv_start+dMv_act)));
	if((temp3*(dDen_start+dMv_start+dMv_act)) >= (voVstart+voVactive+IDOMAIN_IP_DELAY_MARGIN)*100000){
		iv2dv_delay2 = 0;
	}else{
		iv2dv_delay2 = (voVstart+voVactive+IDOMAIN_IP_DELAY_MARGIN)*100000 - (temp3*(dDen_start+dMv_start+dMv_act));
		iv2dv_delay2 = (iv2dv_delay2/100000)+1;
	}
	//rtd_pr_scaler_memory_emerg("[ben]iv2dv_delay2 = %d\n", iv2dv_delay2);

	iv2dv_delay = ((iv2dv_delay1>iv2dv_delay2)?iv2dv_delay1:iv2dv_delay2);

	if(Scaler_DispGetStatus(SLR_MAIN_DISPLAY,SLR_DISP_INTERLACE))
		iv2dv_delay= iv2dv_delay*2;
/*
	//for align output case, it may reduce video delay depends on different NNSR delay,
        //now we use worse case.
	if((srnn_en == 0) && (iv2dv_delay > SRNN_DELAY)){
		srnn_delay_line = SRNN_DELAY * voVactive / dMv_act;
		//rtd_pr_scaler_memory_emerg("[ben]srnn_delay_line = %d\n", srnn_delay_line);
		iv2dv_delay -= srnn_delay_line;
	}
*/
	rtd_pr_scaler_memory_emerg("[ben]iv2dv_delay (final) = %d\n", iv2dv_delay);

	return iv2dv_delay;
}


unsigned int drv_GameMode_decided_iv2dv_delay_old_mode(void)
{
	UINT32 id2dv_delay = 0;

	rtd_pr_scaler_memory_emerg("[ben]drv_GameMode_decided_iv2dv_delay_old_mode \n");

	switch (Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR)) {
		case _MODE_480I: //for 720*240 also use this index
			id2dv_delay = 0xc0;
			break;
		case _MODE_640x350_70HZ:			// Mode 00:
		case _MODE_640x350_85HZ:				// Mode 01:
		case _MODE_640x400_56HZ:				// Mode 02:
		case _MODE_640x400_70HZ:				// Mode 03:
		case _MODE_640x400_85HZ:				// Mode 04:
		case _MODE_720x400_50HZ:				// Mode 05:
		case _MODE_720x400_70HZ:				// Mode 06:
		case _MODE_720x400_85HZ:				// Mode 07:
		case _MODE_720x480_60HZ:				// Mode 08:
		case _MODE_640x480_50HZ:				// Mode 09:
		case _MODE_640x480_60HZ:				// Mode 10:
		case _MODE_640x480_67HZ:				// Mode 11:
		case _MODE_640x480_70HZ:				// Mode 12:
		case _MODE_640x480_72HZ:				// Mode 13:
		case _MODE_640x480_75HZ:				// Mode 14:
		case _MODE_640x480_85HZ:				// Mode 15:
		case _MODE_800x600_50HZ:				// Mode 16:
		case _MODE_800x600_56HZ:				// Mode 17:
		case _MODE_800x600_60HZ:				// Mode 18:
		case _MODE_800x600_72HZ:				// Mode 19:
		case _MODE_800x600_75HZ:				// Mode 20:
		case _MODE_800x600_85HZ:				// Mode 21:
		case _MODE_832x624_75HZ:				// Mode 22:
		case _MODE_1024x768_50HZ:				// Mode 23:
		case _MODE_1024x768_60HZ:				// Mode 24:
		case _MODE_1024x768_66HZ:				// Mode 25:
		case _MODE_1024x768_70HZ:				// Mode 26:
		case _MODE_1024x768_75HZ:				// Mode 27:
		case _MODE_1024x768_85HZ:				// Mode 28:
		case _MODE_1152x864_60HZ:				// Mode 29:
		case _MODE_1152x864_75HZ:				// Mode 30:
		case _MODE_1152x870_75HZ:				// Mode 31:
		case _MODE_1152x900_66HZ:				// Mode 32:
		case _MODE_1280x720_50HZ:				// Mode 33:
		case _MODE_1280x720_60HZ:				// Mode 34:
		case _MODE_1280x720_60HZ_RB:			// Mode 35:
		case _MODE_1280x720_85HZ:				// Mode 36:
		case _MODE_VGA_480P:					// Mode 84: RGB 480p
		case _MODE_VGA_576P:					// Mode 85: RGB 576p
		case _MODE_VGA_720P50:					// Mode 86: RGB 720px50Hz
		case _MODE_VGA_720P60:					// Mode 87: RGB 720px60Hz
		case _MODE_480P:
		case _MODE_576I:
		case _MODE_576P:
		case _MODE_720P50:
		case _MODE_720P60:
		case _MODE_1024x576_60HZ:
			id2dv_delay = 0x100;
			break;
		case _MODE_VGA_1080I25: 			// Mode 88: RGB 1080ix50Hz
		case _MODE_VGA_1080I30: 			// Mode 89: RGB 1080ix60Hz
		case _MODE_1080I25:// Mode 100: YPbPr 1920x1080ix50HZ (802R)
		case _MODE_1080I30:// Mode 101: YPbPr 1920x1080ix60HZ (802R)
		case _MODE_VGA_480I://for pc 1440*480@60 1440*240p
		case _MODE_VGA_576I://for pc 1440*576i@50 1440*288p
			id2dv_delay = 0x180;
			break;
		case _MODE_1280x768_50HZ:				// Mode 37:
		case _MODE_1280x768_60HZ:				// Mode 38:
		case _MODE_1280x768_60HZ_RB:			// Mode 39:
		case _MODE_1280x768_75HZ:				// Mode 40:
		case _MODE_1280x768_85HZ:				// Mode 41:
		case _MODE_1280x800_60HZ:				// Mode 42:
		case _MODE_1280x800_60HZ_RB:			// Mode 43:
		case _MODE_1280x800_75HZ:				// Mode 44:
		case _MODE_1280x960_50HZ:				// Mode 45:
		case _MODE_1280x960_60HZ:				// Mode 46:
		case _MODE_1280x960_75HZ:				// Mode 47:
		case _MODE_1280x960_85HZ:				// Mode 48:
		case _MODE_1280x1024_50HZ:			// Mode 49:
		case _MODE_1280x1024_60HZ:			// Mode 50:
		case _MODE_1280x1024_60HZ_RB:			// Mode 51:
		case _MODE_1280x1024_72HZ:			// Mode 52:
		case _MODE_1280x1024_75HZ:			// Mode 53:
		case _MODE_1280x1024_85HZ:			// Mode 54:
		case _MODE_1360x768_60HZ:				// Mode 55:
		case _MODE_1360x768_60HZ_RB:		// Mode 56:
		case _MODE_1366x768_60HZ:				// Mode 57: //VGA1366x768@60
		case _MODE_1400x1050_50HZ:			// Mode 58:
		case _MODE_1400x1050_60RHZ: 		// Mode 59:(Reduced Blanking)
		case _MODE_1400x1050_60HZ:			// Mode 60:
		case _MODE_1400x1050_75HZ:			// Mode 61:
		case _MODE_1440x900_60HZ:				// Mode 62:
		case _MODE_1440x900_60RHZ:			// Mode 63:(Reduced Blanking)
		case _MODE_1440x900_75HZ:				// Mode 64:
		case _MODE_1440x900_85HZ:				// Mode 65:
		case _MODE_1600x1200_50HZ:			// Mode 66:
		case _MODE_1600x1200_60HZ:			// Mode 67:
		case _MODE_1600x1200_65HZ:			// Mode 68:
		case _MODE_1600x1200_70HZ:			// Mode 69:
		case _MODE_1600x1200_75HZ:			// Mode 70:
		case _MODE_1600x1200_85HZ:			// Mode 71:
		case _MODE_1680x1050_50HZ:			// Mode 72:
		case _MODE_1680x1050_60HZ:			// Mode 73:
		case _MODE_1680x1050_60RHZ: 		// Mode 74:(Reduced Blanking)
		case _MODE_1680x1050_75HZ:			// Mode 75:
		case _MODE_1680x1050_85HZ:			// Mode 76:
		case _MODE_1920x1080_50HZ:			// Mode 77:
		case _MODE_1920x1080_60HZ:			// Mode 78:
		case _MODE_1920x1080_60HZ_138:	// Mode 79:(Reduced Blanking)
		case _MODE_1920x1080_60HZ_148:		// Mode 80:(Reduced Blanking)
		case _MODE_1920x1080_75HZ:			// Mode 81:
		case _MODE_1080P50:
		case _MODE_1080P60:
		case _MODE_1080P23:
		case _MODE_1080P24:
		case _MODE_1080P25:
		case _MODE_1080P29:
		case _MODE_1080P30:
		case _MODE_VGA_1080P50: 				// Mode 90: VGA 1920x1080ix50HZ (802R) //CSW+ 0971001
		case _MODE_VGA_1080P60: 			// Mode 91: VGA 1920x1080ix60HZ (802R) //CSW+ 0971001
		case _MODE_1920x1080_60Hz_173MAC:
			id2dv_delay = 0x1c0;
			break;
		//for 4k2k
		case _MODE_4k2kP24:
		case _MODE_4k2kP25:
		case _MODE_4k2kP30:
		case _MODE_4k2kP60:
		case _MODE_4k2kP50:
		case _MODE_NEW://for 4096*2160@50
			id2dv_delay = 0x2e0;
			/*if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)>=2160)
				id2dv_delay = 0x2e0;
			else if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)>=768)
				&&(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)<2160))
				id2dv_delay = 0x3c0;
			else if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)>=600)
				&&(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)<760))
				id2dv_delay = 0x380;
			else if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)<570)
				id2dv_delay = 0x300;*/
			break;
		default:
			id2dv_delay = 2;//default setting
			break;
	}

	return id2dv_delay;

}

unsigned int drv_GameMode_decided_iv2dv_delay_new_mode(void)
{
	unsigned int id2dv_delay = drv_GameMode_calculate_iv2dv_delay();

	return id2dv_delay;
}

void drv_GameMode_iv2dv_delay(unsigned char enable){
	vgip_data_path_select_RBUS data_path_select_reg;
	ppoverlay_fs_iv_dv_fine_tuning5_RBUS fs_iv_dv_fine_tuning5_reg;
	ppoverlay_iv2dv_double_buffer_ctrl_RBUS ppoverlay_iv2dv_double_buffer_ctrl_reg;
	UINT32 id2dv_delay = 0;
	UINT32 timeout;
	unsigned long flags;//for spin_lock_irqsave
	ppoverlay_fs_iv_dv_fine_tuning1_RBUS tuning1_reg;
	//rtd_pr_scaler_memory_debug("[pool test]enable:%d,CurMode:%d,modeIdx:%d\n",enable,Scaler_DispGetInputInfo(SLR_INPUT_MODE_CURR),Get_GamemodeSaveI3ddmaGetcurMode());
	//fwif_color_safe_od_enable(0);
	if(enable == _ENABLE){
#if 0
		//ivdv delay decision
		switch (Get_GamemodeSaveI3ddmaGetcurMode()) {
			case _MODE_480I: //for 720*240 also use this index
				id2dv_delay = 0xc0;
				break;
			case _MODE_640x350_70HZ:			// Mode 00:
			case _MODE_640x350_85HZ:				// Mode 01:
			case _MODE_640x400_56HZ:				// Mode 02:
			case _MODE_640x400_70HZ:				// Mode 03:
			case _MODE_640x400_85HZ:				// Mode 04:
			case _MODE_720x400_50HZ:				// Mode 05:
			case _MODE_720x400_70HZ:				// Mode 06:
			case _MODE_720x400_85HZ:				// Mode 07:
			case _MODE_720x480_60HZ:				// Mode 08:
			case _MODE_640x480_50HZ:				// Mode 09:
			case _MODE_640x480_60HZ:				// Mode 10:
			case _MODE_640x480_67HZ:				// Mode 11:
			case _MODE_640x480_70HZ:				// Mode 12:
			case _MODE_640x480_72HZ:				// Mode 13:
			case _MODE_640x480_75HZ:				// Mode 14:
			case _MODE_640x480_85HZ:				// Mode 15:
			case _MODE_800x600_50HZ:				// Mode 16:
			case _MODE_800x600_56HZ:				// Mode 17:
			case _MODE_800x600_60HZ:				// Mode 18:
			case _MODE_800x600_72HZ:				// Mode 19:
			case _MODE_800x600_75HZ:				// Mode 20:
			case _MODE_800x600_85HZ:				// Mode 21:
			case _MODE_832x624_75HZ:				// Mode 22:
			case _MODE_1024x768_50HZ:				// Mode 23:
			case _MODE_1024x768_60HZ:				// Mode 24:
			case _MODE_1024x768_66HZ:				// Mode 25:
			case _MODE_1024x768_70HZ:				// Mode 26:
			case _MODE_1024x768_75HZ:				// Mode 27:
			case _MODE_1024x768_85HZ:				// Mode 28:
			case _MODE_1152x864_60HZ:				// Mode 29:
			case _MODE_1152x864_75HZ:				// Mode 30:
			case _MODE_1152x870_75HZ:				// Mode 31:
			case _MODE_1152x900_66HZ:				// Mode 32:
			case _MODE_1280x720_50HZ:				// Mode 33:
			case _MODE_1280x720_60HZ:				// Mode 34:
			case _MODE_1280x720_60HZ_RB:			// Mode 35:
			case _MODE_1280x720_85HZ:				// Mode 36:
			case _MODE_VGA_480P:					// Mode 84: RGB 480p
			case _MODE_VGA_576P:					// Mode 85: RGB 576p
			case _MODE_VGA_720P50:					// Mode 86: RGB 720px50Hz
			case _MODE_VGA_720P60:					// Mode 87: RGB 720px60Hz
			case _MODE_480P:
			case _MODE_576I:
			case _MODE_576P:
			case _MODE_720P50:
			case _MODE_720P60:
                     case _MODE_1024x576_60HZ:
				id2dv_delay = 0x100;
				break;
			case _MODE_VGA_1080I25:				// Mode 88: RGB 1080ix50Hz
			case _MODE_VGA_1080I30:				// Mode 89: RGB 1080ix60Hz
			case _MODE_1080I25:// Mode 100: YPbPr 1920x1080ix50HZ (802R)
			case _MODE_1080I30:// Mode 101: YPbPr 1920x1080ix60HZ (802R)
			case _MODE_VGA_480I://for pc 1440*480@60 1440*240p
			case _MODE_VGA_576I://for pc 1440*576i@50 1440*288p
				id2dv_delay = 0x180;
				break;
			case _MODE_1280x768_50HZ:				// Mode 37:
			case _MODE_1280x768_60HZ:				// Mode 38:
			case _MODE_1280x768_60HZ_RB:			// Mode 39:
			case _MODE_1280x768_75HZ:				// Mode 40:
			case _MODE_1280x768_85HZ:				// Mode 41:
			case _MODE_1280x800_60HZ:				// Mode 42:
			case _MODE_1280x800_60HZ_RB:			// Mode 43:
			case _MODE_1280x800_75HZ:				// Mode 44:
			case _MODE_1280x960_50HZ:				// Mode 45:
			case _MODE_1280x960_60HZ:				// Mode 46:
			case _MODE_1280x960_75HZ:				// Mode 47:
			case _MODE_1280x960_85HZ:				// Mode 48:
			case _MODE_1280x1024_50HZ:			// Mode 49:
			case _MODE_1280x1024_60HZ:			// Mode 50:
			case _MODE_1280x1024_60HZ_RB:			// Mode 51:
			case _MODE_1280x1024_72HZ:			// Mode 52:
			case _MODE_1280x1024_75HZ:			// Mode 53:
			case _MODE_1280x1024_85HZ:			// Mode 54:
			case _MODE_1360x768_60HZ:				// Mode 55:
			case _MODE_1360x768_60HZ_RB:		// Mode 56:
			case _MODE_1366x768_60HZ:				// Mode 57:	//VGA1366x768@60
			case _MODE_1400x1050_50HZ:			// Mode 58:
			case _MODE_1400x1050_60RHZ:			// Mode 59:(Reduced Blanking)
			case _MODE_1400x1050_60HZ:			// Mode 60:
			case _MODE_1400x1050_75HZ:			// Mode 61:
			case _MODE_1440x900_60HZ:				// Mode 62:
			case _MODE_1440x900_60RHZ:			// Mode 63:(Reduced Blanking)
			case _MODE_1440x900_75HZ:				// Mode 64:
			case _MODE_1440x900_85HZ:				// Mode 65:
			case _MODE_1600x1200_50HZ:			// Mode 66:
			case _MODE_1600x1200_60HZ:			// Mode 67:
			case _MODE_1600x1200_65HZ:			// Mode 68:
			case _MODE_1600x1200_70HZ:			// Mode 69:
			case _MODE_1600x1200_75HZ:			// Mode 70:
			case _MODE_1600x1200_85HZ:			// Mode 71:
			case _MODE_1680x1050_50HZ:			// Mode 72:
			case _MODE_1680x1050_60HZ:			// Mode 73:
			case _MODE_1680x1050_60RHZ:			// Mode 74:(Reduced Blanking)
			case _MODE_1680x1050_75HZ:			// Mode 75:
			case _MODE_1680x1050_85HZ:			// Mode 76:
			case _MODE_1920x1080_50HZ:			// Mode 77:
			case _MODE_1920x1080_60HZ:			// Mode 78:
			case _MODE_1920x1080_60HZ_138:	// Mode 79:(Reduced Blanking)
			case _MODE_1920x1080_60HZ_148:		// Mode 80:(Reduced Blanking)
			case _MODE_1920x1080_75HZ:			// Mode 81:
			case _MODE_1080P50:
			case _MODE_1080P60:
			case _MODE_1080P23:
			case _MODE_1080P24:
			case _MODE_1080P25:
			case _MODE_1080P29:
			case _MODE_1080P30:
			case _MODE_VGA_1080P50:  				// Mode 90: VGA 1920x1080ix50HZ (802R) //CSW+ 0971001
			case _MODE_VGA_1080P60:	 			// Mode 91: VGA 1920x1080ix60HZ (802R) //CSW+ 0971001
			case _MODE_1920x1080_60Hz_173MAC:
				id2dv_delay = 0x1c0;
				break;
			//for 4k2k
			case _MODE_4k2kP24:
			case _MODE_4k2kP25:
			case _MODE_4k2kP30:
			case _MODE_4k2kP60:
			case _MODE_4k2kP50:
			case _MODE_NEW://for 4096*2160@50
				id2dv_delay = 0x2e0;
				/*if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)>=2160)
					id2dv_delay = 0x2e0;
				else if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)>=768)
					&&(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)<2160))
					id2dv_delay = 0x3c0;
				else if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)>=600)
					&&(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)<760))
					id2dv_delay = 0x380;
				else if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE)<570)
					id2dv_delay = 0x300;*/
				break;
			default:
				id2dv_delay = 1;//default setting
				break;
		}
#else
		if(IoReg_Read32(VGIP_VGIP_VBISLI_reg)&_BIT19){ //for debug only
			id2dv_delay = drv_GameMode_decided_iv2dv_delay_old_mode();
		}else{
			id2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();
		}

		//if compression enable, need to add margin. @Crixus 20170605
		if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY))
			id2dv_delay = drv_GameMode_iv2dv_delay_compress_margin(id2dv_delay);
#if 0
		//120Hz panel VRR dtg sets 2pixel mode, fix iv2dv half frame
		if((Get_DISPLAY_REFRESH_RATE() == 120) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag())){
			if(vbe_disp_get_VRR_device_max_framerate()> 119000){ // 120hz
				id2dv_delay = drv_GameMode_decided_iv2dv_delay_new_mode();
			}
			else{
				id2dv_delay = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_V_LEN) / 2;
			}
		}
#else
		if((get_panel_pixel_mode() > PANEL_1_PIXEL) && (get_vsc_src_is_hdmi_or_dp(SLR_MAIN_DISPLAY)) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag())){
			return;
		}

#endif


#endif
	}
	else{
		//iv2dv delay
		id2dv_delay = 2;//default setting
	}

	if (id2dv_delay > Scaler_DispGetInputInfo(SLR_INPUT_V_LEN))
		id2dv_delay = Scaler_DispGetInputInfo(SLR_INPUT_V_LEN) - Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_STA_PRE);

	ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
	ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_en = 1;
	IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);

	if(drv_memory_get_game_mode_iv2dv_slow_enable() == TRUE){

		if(id2dv_delay != 2){
			id2dv_delay_target = id2dv_delay;
			DbgSclrFlgTkr.run_iv2dv_slow_tuning_flag = TRUE;
		}else{
			//enable iv2dv toggle_en
			tuning1_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning1_reg);
			tuning1_reg.iv2dv_toggle_en = _ENABLE;
			IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning1_reg, tuning1_reg.regValue);

			timeout=10;
			down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
			IoReg_SetBits(PPOVERLAY_Double_Buffer_CTRL_reg,_BIT2|_BIT0);
			spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
			up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			drivf_scaler_reset_freerun(DTG_MASTER);
			while((IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg)&_BIT0)&&--timeout){
				msleep(10);
			}
			down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
			IoReg_ClearBits(PPOVERLAY_Double_Buffer_CTRL_reg,_BIT2);
			spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
			up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
			IoReg_ClearBits(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg,_BIT2);
			id2dv_delay_target = 1;
			DbgSclrFlgTkr.run_iv2dv_slow_tuning_flag = FALSE;

			//iv2dv delay
			fs_iv_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
			fs_iv_dv_fine_tuning5_reg.iv2dv_line = id2dv_delay;
			fs_iv_dv_fine_tuning5_reg.iv2dv_line_2 = id2dv_delay;
			IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, fs_iv_dv_fine_tuning5_reg.regValue);
		}
		rtd_pr_new_game_mode_notice("[Game Mode] iv2dv delay = %d\n", id2dv_delay);
	}else{

		//iv2dv delay
		fs_iv_dv_fine_tuning5_reg.regValue = IoReg_Read32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg);
		fs_iv_dv_fine_tuning5_reg.iv2dv_line = id2dv_delay;
		fs_iv_dv_fine_tuning5_reg.iv2dv_line_2 = id2dv_delay;
		IoReg_Write32(PPOVERLAY_FS_IV_DV_Fine_Tuning5_reg, fs_iv_dv_fine_tuning5_reg.regValue);
		rtd_pr_scaler_memory_debug("[Game Mode] iv2dv delay = %d\n", id2dv_delay);
	}
	ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply = 1;
	IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);

	data_path_select_reg.regValue = IoReg_Read32(VGIP_Data_Path_Select_reg);
	if(data_path_select_reg.s1_clk_en)
	{//if no s1 clock no need check apply
		timeout = 0x3fffff;
		ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
		while((ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply) && (--timeout)){
			ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
		}
		ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply = 1;
		IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);
		timeout = 0x3fffff;
		ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
		while((ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_apply) && (--timeout)){
			ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue = IoReg_Read32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg);
		}
	}
	ppoverlay_iv2dv_double_buffer_ctrl_reg.iv2dv_db_en = 0;
	IoReg_Write32(PPOVERLAY_iv2dv_Double_Buffer_CTRL_reg, ppoverlay_iv2dv_double_buffer_ctrl_reg.regValue);

	//fwif_color_safe_od_enable(1);
}
#ifndef BUILD_QUICK_SHOW


extern void drvif_framesync_gatting(unsigned char enable);
extern unsigned char drvif_framesync_gatting_do(void);
void drv_game_mode_timing_Dynamic(unsigned char enable){
//	unsigned int timeout = 0x3ffff;
	#if 0
	/*20170629 pinyen remove driver conditions to new_game_mode_tsk*/
	mdomain_disp_ddr_mainctrl_RBUS ddr_mainctrl_reg;
	#endif
//	ppoverlay_double_buffer_ctrl_RBUS double_buffer_ctrl_reg;
//	UINT8 display_dtg_db_en = 0;
//	unsigned long flags;//for spin_lock_irqsave

	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC)
		//VRR always go timing fsync,  do not check timing mode here
		|| (((get_vsc_src_is_hdmi_or_dp(SLR_MAIN_DISPLAY)) && (vbe_disp_get_VRR_timingMode_flag() || vbe_disp_get_freesync_mode_flag()))==1)){
		return;
	}


	if(enable == TRUE){

		if(vbe_disp_gamemode_use_fixlastline()){
			if(get_new_game_mode_small_window_condition() && (get_vsc_data_path_indicator(DRIVER_FREERUN_PATTERN) == 1)){
				fwif_color_safe_od_enable(0);
				drvif_scaler_set_display_mode(DISPLAY_MODE_FRAME_SYNC, scaler_dtg_get_app_type());
				Scaler_Set_Display_Timing_Mode(DISPLAY_MODE_FRAME_SYNC);
				modestate_set_fll_running_flag(FALSE);
				fwif_color_safe_od_enable(1);
			}else{
				drivf_scaler_reset_freerun(DTG_MASTER);
				drv_game_mode_disp_smooth_variable_setting(GAME_MODE_NEW_FLL);
			}
		}else{
			fwif_color_safe_od_enable(0);
			drv_game_mode_disp_smooth_variable_setting(GAME_MODE_FRAME_SYNC);
			//drv_GameMode_iv2dv_delay(_ENABLE);//move to frc2fs driver @Crixus 20170712
			fwif_color_safe_od_enable(1);
		}
		rtd_pr_new_game_mode_notice("[Game Mode]D-domain game mode enable!!\n");
	}
	else{
//		if(Get_DISPLAY_PANEL_OLED_TYPE()== TRUE){
//			msleep(100);//wait M-cap write latest 2 frame.
//		}
		//drivf_scaler_reset_freerun(DTG_MASTER);
                //when exit game mode, we need to check if it use timing fsync condition
                //ex: small window enter->exit game mode
		if(vbe_get_HDMI_run_timing_framesync_condition()){
			fwif_color_safe_od_enable(0);
			drvif_scaler_set_display_mode(DISPLAY_MODE_FRAME_SYNC, scaler_dtg_get_app_type());
			Scaler_Set_Display_Timing_Mode(DISPLAY_MODE_FRAME_SYNC);
			fwif_color_safe_od_enable(1);
		}else{
			drv_GameMode_iv2dv_delay(_DISABLE);
			drv_game_mode_disp_smooth_variable_setting(GAME_MODE_NEW_FLL);
		}
		rtd_pr_new_game_mode_notice("[Game Mode]D-domain game mode disable!!\n");
	}
}
#ifdef CONFIG_RTK_FEATURE_FOR_GKI
#ifdef DUMP_SCALER_MEMORY
#define SCALER_SUB1_FILE_PATH "/tmp/scaler_sub_dump1.raw"
#define SCALER_SUB2_FILE_PATH "/tmp/scaler_sub_dump2.raw"
#define SCALER_SUB3_FILE_PATH "/tmp/scaler_sub_dump3.raw"
#include <linux/fs.h>
static struct file* file_open(const char* path, int flags, int rights) {
	struct file* filp = NULL;
	mm_segment_t oldfs;
	int err = 0;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	filp = filp_open(path, flags, rights);
	set_fs(oldfs);
	if(IS_ERR(filp)) {
		err = PTR_ERR(filp);
		return NULL;
	}
	return filp;
}

static void file_close(struct file* file) {
	filp_close(file, NULL);
}

static int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	ret = kernel_read(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}

static int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	ret = kernel_write(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}

static int file_sync(struct file* file) {
	vfs_fsync(file, 0);
	return 0;
}

/*void drvif_sub_memory_free_fixed_virtual_memory(void)
{
	if((drvif_memory_get_memtag_virtual_startaddr(MEMIDX_SUB_THIRD) == 0) ){
		rtd_pr_scaler_memory_debug("fixed virtual address is NULL, not to free!!\n");
		return;
	}
	dvr_unmap_memory((void *)drvif_memory_get_memtag_virtual_startaddr(MEMIDX_SUB_THIRD), M_fixed_virtual_memory_size);
}*/

int scaler_dump_sub_data_to_file(unsigned int bufferindex)
{
	struct file* filp = NULL;
	unsigned int buffer_size = MDOMAIN_SUB_BUFFER_SIZE;;
	unsigned int Allocate_VirAddr;

	//free virtual memory
	//drvif_sub_memory_free_fixed_virtual_memory();
	//set virtual memory size
	buffer_size = PAGE_ALIGN(buffer_size);

	if (bufferindex == 1) {
		rtd_pr_scaler_memory_emerg("\n\n\n\n ***************** scaler_dump_data_to_file start1  ******%x***************\n\n\n\n",IoReg_Read32(MDOMAIN_DISP_DDR_SubAddr_reg));
		//virtual memory allocation	and just use one buffer
		//buffer_size = MemTag[MEMIDX_SUB].Size;
		Allocate_VirAddr = ((unsigned long)dvr_remap_uncached_memory(drvif_memory_get_memtag_startaddr(MEMIDX_SUB), buffer_size, __builtin_return_address(0)));

		struct file* filp = file_open(SCALER_SUB1_FILE_PATH, O_RDWR | O_CREAT, 0);
		if (filp == NULL) {
			rtd_pr_scaler_memory_emerg("file open fail\n");
			return FALSE;
		}
		unsigned long outfileOffset = 0;
		if(!Allocate_VirAddr){
			rtd_pr_scaler_memory_emerg("***[error]addr %d is zero****");
			return FALSE;
		}

		file_write(filp, outfileOffset, (unsigned char*)Allocate_VirAddr, 704*240*2);
		file_sync(filp);
		file_close(filp);
		rtd_pr_scaler_memory_emerg("\n\n\n\n *****************  scaler_dump_data_to_file end  *********************\n\n\n\n");
		return TRUE;
	} else if (bufferindex == 2) {
		rtd_pr_scaler_memory_emerg("\n\n\n\n ***************** scaler_dump_data_to_file start2  ******%x***************\n\n\n\n",IoReg_Read32(MDOMAIN_DISP_DDR_Sub2ndAddr_reg));
		//buffer_size = MemTag[MEMIDX_SUB_SEC].Size;
		Allocate_VirAddr = ((unsigned int)dvr_remap_uncached_memory(drvif_memory_get_memtag_startaddr(MEMIDX_SUB_SEC), buffer_size, __builtin_return_address(0)));
		struct file* filp = file_open(SCALER_SUB2_FILE_PATH, O_RDWR | O_CREAT, 0);
		if (filp == NULL) {
			rtd_pr_scaler_memory_emerg("file open fail\n");
			return FALSE;
		}
		unsigned long outfileOffset = 0;
		if(!Allocate_VirAddr){
			rtd_pr_scaler_memory_emerg("***[error]addr %d is zero****");
			return FALSE;
		}

		file_write(filp, outfileOffset, (unsigned char*)Allocate_VirAddr, 704*240*2);
		file_sync(filp);
		file_close(filp);
		rtd_pr_scaler_memory_emerg("\n\n\n\n *****************  scaler_dump_data_to_file end  *********************\n\n\n\n");
		return TRUE;
	}else if (bufferindex == 3) {
		rtd_pr_scaler_memory_emerg("\n\n\n\n ***************** scaler_dump_data_to_file start3  ******%x***************\n\n\n\n",IoReg_Read32(MDOMAIN_DISP_DDR_Sub3rdAddr_reg));
		//buffer_size = MemTag[MEMIDX_SUB_THIRD].Size;
		Allocate_VirAddr = ((unsigned int)dvr_remap_uncached_memory(drvif_memory_get_memtag_startaddr(MEMIDX_SUB_THIRD), buffer_size, __builtin_return_address(0)));
		struct file* filp = file_open(SCALER_SUB3_FILE_PATH, O_RDWR | O_CREAT, 0);
		if (filp == NULL) {
			rtd_pr_scaler_memory_emerg("file open fail\n");
			return FALSE;
		}
		unsigned long outfileOffset = 0;
		if(!Allocate_VirAddr){
			rtd_pr_scaler_memory_emerg("***[error]addr %d is zero****");
			return FALSE;
		}

		file_write(filp, outfileOffset, (unsigned char*)Allocate_VirAddr, 704*240*2);
		file_sync(filp);
		file_close(filp);
		rtd_pr_scaler_memory_emerg("\n\n\n\n *****************  scaler_dump_data_to_file end  *********************\n\n\n\n");
		return TRUE;
	}
	rtd_pr_scaler_memory_emerg("***[error] wrong memory index****\n");
	return FALSE;
}
#endif
#endif

#endif

/*======================M-domain Multi-Buffer Controlling=====================*/
//static UINT8 Mdomain_multibuffer_enable = false;
//static UINT8 Mdomain_capture_multibuffer_enable = false;
//static UINT8 Mdomain_multibuffer_number = 0;
//static unsigned int Mdomain_multibuffer_size = 0;
//static UINT8 Mdomain_multibuffer_number_pre = 0;

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


#ifndef BUILD_QUICK_SHOW

unsigned char cur_main_m_cap_block = 0;//record current m domain capture block. record at vgip start isr

void record_current_m_cap_block(unsigned char display)
{//call by vgip start. we need record at vgip start and use at vgip end. avoid the status has risk at vgip end
	if(display == SLR_MAIN_DISPLAY)
	{//main path
        mdomain_disp_disp0_buf_status_0_RBUS mdomain_disp_disp0_buf_status_0_reg;
        mdomain_disp_disp0_buf_status_0_reg.regValue = IoReg_Read32(MDOMAIN_DISP_disp0_buf_status_0_reg);
		cur_main_m_cap_block = mdomain_disp_disp0_buf_status_0_reg.cap0_buf_index_cur;
	}
}

unsigned char get_current_m_cap_block(unsigned char display)
{//return vgip start cpture blcok
	if(display == SLR_MAIN_DISPLAY)
	{//main path
		return cur_main_m_cap_block;
	}
	else
	{//sub
		return 0;
	}
}

void clear_m_cap_done_status(unsigned char display)
{//use to clear capture status at vgip start
	if(display == SLR_MAIN_DISPLAY)
	{
        mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;
        mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
        mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
        IoReg_Write32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);
	}
}

static void wait_m_cap_done(unsigned char display)
{//use for freeze wait cature done at vgip end
	unsigned int timeout = 0x3ffff;
	unsigned int vgip_linecount = 0;
	mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;

	if(display == SLR_MAIN_DISPLAY)
	{
		vgip_linecount = SCALEDOWN_ICH1_line_cnt_get_sdnr_ch1_line_cnt(IoReg_Read32(SCALEDOWN_ICH1_line_cnt_reg));//record current vgip line count
        mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
		while(!(mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end) && (SCALEDOWN_ICH1_line_cnt_get_sdnr_ch1_line_cnt(IoReg_Read32(SCALEDOWN_ICH1_line_cnt_reg)) >= vgip_linecount)
			&& (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) && --timeout){//check m domain capture done or vgip over vsync or not active state
			
            mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
			if(drvif_mode_check_dma_onlinemeasure_status() == FALSE)//online fail
				break;
		}
	}
	else
	{

	}

}

//M-domain freeze firmware mode @Crixus 20170512, sync from k3 elieli
void drv_memory_freeze_fw_mode(unsigned char display, unsigned char enable, unsigned char freeze_block)
{
    mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);

	//game mode case, disable cap to freeze
	if(display == SLR_MAIN_DISPLAY)
	{
		if(!get_mdomain_driver_status())//m domain not ready no need to freeze
			return;

		if((drv_memory_get_game_mode_flag() == 1) && (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_VDEC) &&
            (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) != VSC_INPUTSRC_CAMERA)){//seamless only hdmi has game mode case
			   if(enable == _ENABLE)
			       wait_m_cap_done(SLR_MAIN_DISPLAY);//disable cap need wait capture done
			   if(enable == _ENABLE){
					   drvif_memory_DisableMCap(SLR_MAIN_DISPLAY);
			   }
			   else{
					   drvif_memory_EnableMCap(SLR_MAIN_DISPLAY);
			   }

		} else {
                //capture change to 1-buffer mode to freeze disp
                if(enable == _ENABLE){
                    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 1;
                    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = freeze_block;
                }
                else{
                    mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 0;
                }
                IoReg_Write32(MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);
		}
	}
}
#endif

void drv_memory_display_set_input_fast(unsigned char display, unsigned char enable)
{
	mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
    mdom_sub_cap_cap1_buf_control_RBUS mdom_sub_cap_cap1_buf_control_reg;
    mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;
	mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;

	if(display == SLR_MAIN_DISPLAY) {
		if(enable == true) {
            mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);
            mdomain_cap_cap0_buf_control_reg.cap0_input_fast = 1;
            IoReg_Write32(MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

//			mdomain_disp_main_disp_blocksel_option_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Main_disp_blocksel_option_reg);
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_en = 1;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_mode = 1;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_line = Get_DISP_DEN_END_VPOS();
//			IoReg_Write32(MDOMAIN_DISP_Main_disp_blocksel_option_reg,  mdomain_disp_main_disp_blocksel_option_reg.regValue);

			mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
			IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,  mdomain_disp_disp0_db_ctrl_reg.regValue);
		} else {
            mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);
            mdomain_cap_cap0_buf_control_reg.cap0_input_fast = 0;
            IoReg_Write32(MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

//			mdomain_disp_main_disp_blocksel_option_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Main_disp_blocksel_option_reg);
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_en = 0;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_mode = 0;
//			mdomain_disp_main_disp_blocksel_option_reg.main_force_blocksel_toggle_line = 0;
//			IoReg_Write32(MDOMAIN_DISP_Main_disp_blocksel_option_reg,  mdomain_disp_main_disp_blocksel_option_reg.regValue);

			mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
			IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,  mdomain_disp_disp0_db_ctrl_reg.regValue);
		}
	} else {
		if(enable == true) {
            mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg);
            mdom_sub_cap_cap1_buf_control_reg.cap1_input_fast = 1;
            IoReg_Write32(MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

//			mdomain_disp_sub_disp_blocksel_option_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Sub_disp_blocksel_option_reg);
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_en = 1;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_mode = 1;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_line = Get_DISP_DEN_END_VPOS();
//			IoReg_Write32(MDOMAIN_DISP_Sub_disp_blocksel_option_reg,  mdomain_disp_sub_disp_blocksel_option_reg.regValue);

			mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
			mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
			IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,  mdom_sub_disp_disp1_db_ctrl_reg.regValue);
		} else {
            mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg);
            mdom_sub_cap_cap1_buf_control_reg.cap1_input_fast = 0;
            IoReg_Write32(MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

//			mdomain_disp_sub_disp_blocksel_option_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Sub_disp_blocksel_option_reg);
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_en = 0;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_mode = 0;
//			mdomain_disp_sub_disp_blocksel_option_reg.sub_force_blocksel_toggle_line = 0;
//			IoReg_Write32(MDOMAIN_DISP_Sub_disp_blocksel_option_reg,  mdomain_disp_sub_disp_blocksel_option_reg.regValue);

			mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
			mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
			IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,  mdom_sub_disp_disp1_db_ctrl_reg.regValue);
		}
	}
}

void drv_memory_set_vdec_direct_low_latency_mode(unsigned char enable){
	enable_M_LowLatencyMode_Vdec_direct = enable;
}

unsigned char drv_memory_get_vdec_direct_low_latency_mode(void){
	return enable_M_LowLatencyMode_Vdec_direct;
}

bool is_panel_size_bigger_than_m_size(unsigned char display)
{
    bool ret = false;
    calculate_carveout_size(display);
    if (display == SLR_MAIN_DISPLAY
        && ((_DISP_WID * _DISP_LEN) > (Get_Val_Max_Width() * Get_Val_Max_Len())))
        ret = true;

    return ret;
}

bool is_2k_memory_buffer_source(SCALER_DISP_CHANNEL display)
{//return ture: means m domain use 2k size.   return False: means m domain use 4k size. So need to borrow from i3ddma size
	VSC_INPUT_TYPE_T srctype;

	srctype = Get_DisplayMode_Src(display);

	if(get_platform_model() == PLATFORM_MODEL_2K)
		return TRUE;

	if ((display == SLR_MAIN_DISPLAY) && is_panel_size_bigger_than_m_size(display)){
		if (!get_force_i3ddma_enable(SLR_MAIN_DISPLAY))
				return FALSE;
	}

	return TRUE;
}

unsigned char sub_use_main_buffer_case(void)
{//return true: use
	return FALSE;//mac9q does not need two path
}

#ifndef BUILD_QUICK_SHOW
unsigned char pc_mode_mdomain_addr_must_use_di_start_addr(void)
{//return TRUE:must use di start address.
	/*
	if(get_vsc_run_pc_mode() && pc_mode_borrow_di_memory_case() && !check_mdomain_di_memory_connection())
		return TRUE;
	*/
	return FALSE;//k8 di is connect m domain caveout
}


unsigned char pc_mode_borrow_di_memory_case(void)
{//return false: only use main m domain(12bit shiif is enough)   return true: need to use DI memory(16bit shiif)
	//small than 3k no need to borrow DI memory
	if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPH_ACT_WID_PRE) <= 2560) && (Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_IPV_ACT_LEN_PRE) <= 1440))
		return FALSE;
	return TRUE;

}

unsigned char check_mdomain_di_memory_connection(void)
{//return false: di and m domain memory is not connected. return true:connect
#if 0
	if (carvedout_buf_get_layout_idx() == MM_LAYOUT_HP_2MC)
		return FALSE;
	else
#endif
		return TRUE;
}

#endif

void drv_memory_display_check_sub_frc_style(void)
{
	unsigned int uzudtg_output_framerate = 0;
	unsigned int uzudtg_dh_total = PPOVERLAY_uzudtg_DH_TOTAL_get_uzudtg_dh_total(IoReg_Read32(PPOVERLAY_uzudtg_DH_TOTAL_reg)) + 1;
	unsigned int uzudtg_dv_total = PPOVERLAY_uzudtg_DV_TOTAL_get_uzudtg_dv_total(IoReg_Read32(PPOVERLAY_uzudtg_DV_TOTAL_reg)) + 1;
	unsigned int remove = PPOVERLAY_DTG_M_Remove_input_vsync_get_remove_half_ivs_mode2(IoReg_Read32(PPOVERLAY_DTG_M_Remove_input_vsync_reg)) + 1;
	unsigned int multiple = PPOVERLAY_DTG_M_multiple_vsync_get_dtg_m_multiple_vsync(IoReg_Read32(PPOVERLAY_DTG_M_multiple_vsync_reg)) + 1;

	if (!get_sub_OutputVencMode())
	{//sub m domain iterrupt open so don't need wait lastwrite @qing_liu --s4ap
		//Update the M-domain last done @Crixus 20161013
		drv_memory_wait_cap_last_write_done(SLR_SUB_DISPLAY, 1, FALSE);

		if( get_panel_pixel_mode() > PANEL_1_PIXEL )
			uzudtg_output_framerate = Get_DISPLAY_CLOCK_TYPICAL() / uzudtg_dh_total * 2000 / uzudtg_dv_total * remove / multiple;
		else
			uzudtg_output_framerate = Get_DISPLAY_CLOCK_TYPICAL() / uzudtg_dh_total * 1000 / uzudtg_dv_total * remove / multiple;

		if(
			( uzudtg_output_framerate < ( Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_V_FREQ_1000) - 500 ) ) &&
			(Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_INTERLACE) == FALSE)) // If sub path is interlace timing, always set sub input slow and CH2_FDROP_EN = 1 - WOSQRTK-17868
			mdomain_input_fast_flag[SLR_SUB_DISPLAY] = 1;
		else
			mdomain_input_fast_flag[SLR_SUB_DISPLAY] = 0;

		rtd_pr_scaler_memory_info("[%s] check sub input style, sub_frc_style=%d, main_fr=%d, sub_vfreq=%d\n", __FUNCTION__, mdomain_input_fast_flag[SLR_SUB_DISPLAY], uzudtg_output_framerate, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_V_FREQ));
	}
}

unsigned int drv_Calculate_m_pre_read_value(void)
{
	//dic suggest mac9q pre_read need set 1
	unsigned int m_pre_read = 1;
#if 0
	unsigned int DV_den_start = Get_DISP_DEN_STA_VPOS();
	unsigned int orbit_mv_extend_max_half = 0;
	unsigned int time_borrow = 0;
	unsigned int total_interval = 0;
	ppoverlay_uzudtg_orbit_main_ctrl0_RBUS ppoverlay_uzudtg_orbit_main_ctrl0_reg;
	ppoverlay_uzudtg_stage1_time_borrow_RBUS ppoverlay_uzudtg_stage1_time_borrow_reg;

	if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC))
	{
		m_pre_read = _M_DATA_FS_PRE_READ;
	}
	else
	{
		ppoverlay_uzudtg_stage1_time_borrow_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_stage1_time_borrow_reg);
		time_borrow = ppoverlay_uzudtg_stage1_time_borrow_reg.stage1_time_borrow_v;
		rtd_pr_scaler_memory_notice("[%s] DV_den_start = %d, total_interval += time_borrow : %d\n", __FUNCTION__, DV_den_start, time_borrow);
		total_interval += time_borrow;

		// ===== sync from K24 ( version : read settings from register ), merline8 doese not have orbit function now - EricTang 20230928 =====
		ppoverlay_uzudtg_orbit_main_ctrl0_reg.regValue = IoReg_Read32(PPOVERLAY_uzudtg_orbit_main_ctrl0_reg);
		if( ( Get_DISPLAY_PANEL_OLED_TYPE() == TRUE ) && ( ppoverlay_uzudtg_orbit_main_ctrl0_reg.orbit_m_overscan_en == 1 ) )	// orbit_mv_extend_max only valid in OLED overscan case
		{
			orbit_mv_extend_max_half = ppoverlay_uzudtg_orbit_main_ctrl0_reg.orbit_mv_extend_max / 2;	// Note : orbit_mv_extend_max must be 2N
			rtd_pr_scaler_memory_notice("[%s] total_interval += orbit_mv_extend_max_half : %d\n", __FUNCTION__, orbit_mv_extend_max_half);
			total_interval += orbit_mv_extend_max_half;
		}
		// ===================================================================================================================================

		if( ( get_panel_res() == PANEL_RES_FHD ) || ( get_panel_res() == PANEL_RES_HD ) )
		{
			rtd_pr_scaler_memory_notice("[%s] total_interval += _M_PRE_READ_MARGIN_FHD : %d\n", __FUNCTION__, _M_PRE_READ_MARGIN_FHD);
			total_interval += _M_PRE_READ_MARGIN_FHD;	// _M_PRE_READ_MARGIN_FHD need to be at least larger than 4 ( m_pre_read <=2=> db_latch_line <=2=> DV_den_start )
		}
		else
		{
			rtd_pr_scaler_memory_notice("[%s] total_interval += _M_PRE_READ_MARGIN : %d\n", __FUNCTION__, _M_PRE_READ_MARGIN);
			total_interval += _M_PRE_READ_MARGIN;	// _M_PRE_READ_MARGIN need to be at least larger than 4 ( m_pre_read <=2=> db_latch_line <=2=> DV_den_start )
		}

		if( DV_den_start >= total_interval )
		{
			m_pre_read = DV_den_start - total_interval;
		}
		else
		{
			m_pre_read = 0;
			rtd_pr_scaler_memory_err("[%s][Error] DV_den_start - total_interval < 0\n", __FUNCTION__);
		}
	}

	rtd_pr_scaler_memory_notice("[%s] m_pre_read = %d!!\n", __FUNCTION__, m_pre_read);
#endif

	return m_pre_read;
}

void memory_frc_set_pre_read_v_start(unsigned char display, unsigned int pre_read)
{
    if (display == SLR_MAIN_DISPLAY){
        mdomain_disp_disp0_pre_rd_ctrl_RBUS mdomain_disp_disp0_pre_rd_ctrl_reg;
        mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;

    #ifndef BUILD_QUICK_SHOW
        Check_smooth_toggle_update_flag(SLR_MAIN_DISPLAY);
    #endif

        mdomain_disp_disp0_pre_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_pre_rd_ctrl_reg);
        rtd_pr_scaler_memory_info("[%s] change main_pre_rd_v_start from %d -> %d\n", __FUNCTION__, mdomain_disp_disp0_pre_rd_ctrl_reg.disp0_pre_rd_v_start, pre_read);
        mdomain_disp_disp0_pre_rd_ctrl_reg.disp0_pre_rd_v_start = pre_read;
        IoReg_Write32(MDOMAIN_DISP_Disp0_pre_rd_ctrl_reg, mdomain_disp_disp0_pre_rd_ctrl_reg.regValue);

        mdomain_disp_disp0_db_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
        if( mdomain_disp_disp0_db_ctrl_reg.disp0_db_en == 1 )
        {
            mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply = 1;
            IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
        }

        //drvif_srnn_set_pre_rd_start();
        scaler_disp_dtg_set_db_latch_line();
    } else {
        mdom_sub_disp_disp1_pre_rd_ctrl_RBUS mdom_sub_disp_disp1_pre_rd_ctrl_reg;
        mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;

        mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg);
         rtd_pr_scaler_memory_info("[%s] change sub_pre_rd_v_start from %d -> %d\n", __FUNCTION__, mdom_sub_disp_disp1_pre_rd_ctrl_reg.disp1_pre_rd_v_start, pre_read);
        mdom_sub_disp_disp1_pre_rd_ctrl_reg.disp1_pre_rd_v_start = pre_read;
        IoReg_Write32(MDOM_SUB_DISP_Disp1_pre_rd_ctrl_reg, mdom_sub_disp_disp1_pre_rd_ctrl_reg.regValue);

        mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
        if (mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_en == 1)
        {
            mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
            IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg, mdom_sub_disp_disp1_db_ctrl_reg.regValue);
        }
    }
}

void drv_memory_set_vo_gating_threshold(void)
{
    mdomain_disp_disp0_fsync_ctrl_1_RBUS mdomain_disp_disp0_fsync_ctrl_1_reg;
    mdomain_disp_disp0_fsync_ctrl_0_RBUS mdomain_disp_disp0_fsync_ctrl_0_reg;

    mdom_sub_disp_disp1_fsync_ctrl_1_RBUS mdom_sub_disp_disp1_fsync_ctrl_1_reg;
    mdom_sub_disp_disp1_fsync_ctrl_0_RBUS mdom_sub_disp_disp1_fsync_ctrl_0_reg;

    //main
    mdomain_disp_disp0_fsync_ctrl_1_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_FSYNC_CTRL_1_reg);
    mdomain_disp_disp0_fsync_ctrl_1_reg.disp0_fifo_gate_th1 = MAIN_FIFO_GATE_TH_1;
    mdomain_disp_disp0_fsync_ctrl_1_reg.disp0_fifo_gate_th0 = MAIN_FIFO_GATE_TH;
    IoReg_Write32(MDOMAIN_DISP_DISP0_FSYNC_CTRL_1_reg, mdomain_disp_disp0_fsync_ctrl_1_reg.regValue);

    mdomain_disp_disp0_fsync_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_FSYNC_CTRL_0_reg);
    mdomain_disp_disp0_fsync_ctrl_0_reg.disp0_fifo_gate_th3 = MAIN_FIFO_GATE_TH_3;
    mdomain_disp_disp0_fsync_ctrl_0_reg.disp0_fifo_gate_th2 = MAIN_FIFO_GATE_TH_2;
    IoReg_Write32(MDOMAIN_DISP_DISP0_FSYNC_CTRL_0_reg, mdomain_disp_disp0_fsync_ctrl_0_reg.regValue);

    //sub
    mdom_sub_disp_disp1_fsync_ctrl_1_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_FSYNC_CTRL_1_reg);
    mdom_sub_disp_disp1_fsync_ctrl_1_reg.disp1_fifo_gate_th1 = MAIN_FIFO_GATE_TH_1;
    mdom_sub_disp_disp1_fsync_ctrl_1_reg.disp1_fifo_gate_th0 = MAIN_FIFO_GATE_TH;

    IoReg_Write32(MDOM_SUB_DISP_DISP1_FSYNC_CTRL_1_reg, mdom_sub_disp_disp1_fsync_ctrl_1_reg.regValue);
    mdom_sub_disp_disp1_fsync_ctrl_0_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_FSYNC_CTRL_0_reg);
    mdom_sub_disp_disp1_fsync_ctrl_0_reg.disp1_fifo_gate_th3 = MAIN_FIFO_GATE_TH_3;
    mdom_sub_disp_disp1_fsync_ctrl_0_reg.disp1_fifo_gate_th2 = MAIN_FIFO_GATE_TH_2;
    IoReg_Write32(MDOM_SUB_DISP_DISP1_FSYNC_CTRL_0_reg, mdom_sub_disp_disp1_fsync_ctrl_0_reg.regValue);

}

void wait_sub_disp_double_buffer_apply(void)
{

	unsigned int timeout = 50;
    mdom_sub_disp_disp1_db_ctrl_RBUS mdom_sub_disp_disp1_db_ctrl_reg;

	mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply = 1;
	IoReg_Write32(MDOM_SUB_DISP_Disp1_db_ctrl_reg,mdom_sub_disp_disp1_db_ctrl_reg.regValue);

	mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
	while ((mdom_sub_disp_disp1_db_ctrl_reg.disp1_db_apply == 1) && (timeout--)) {
		mdom_sub_disp_disp1_db_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_db_ctrl_reg);
		usleep_range(5000, 5000);
	}

}

void drvif_memory_EnableMDisp_Or_DisableMDisp(unsigned char display, bool enable)
{
    mdomain_disp_disp0_db_ctrl_RBUS mdomain_disp_disp0_db_ctrl_reg;
	unsigned char use_db_disable = FALSE;//if data frame sync and gatting enable need to using db apply
	int apply_timeout_cnt = 10;

	rtd_pr_scaler_memory_info("[%s] %d (90K=%d) display=%d, enable=%d\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), display, enable);

	if(enable == _ENABLE)
	{
		//enable main/sub disp
		IoReg_SetBits(display? MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg: MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, display? MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask: MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_disp0_dma_enable_mask);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            IoReg_SetBits(display? MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg: MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, display? MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_disp1c_dma_enable_mask: MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_disp0c_dma_enable_mask);
        }
		//wait double buffer apply
		drvif_memory_set_dbuffer_write(display);
	}
	else
	{
		if(display == SLR_MAIN_DISPLAY)
		{
			mdomain_cap_cap0_pxl_wrap_ctrl_0_RBUS mdomain_cap_cap0_pxl_wrap_ctrl_0_reg;
			mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);
			if(mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en && (VODMA_VODMA_CLKGEN_get_en_fifo_full_gate(IoReg_Read32(VODMA_VODMA_CLKGEN_reg))))
				use_db_disable = TRUE;//gatting enable using db apply

			if(use_db_disable)
			{
				//enable double buffer
				mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
				mdomain_disp_disp0_db_ctrl_reg.disp0_db_en=1;
				IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
			}

			//FRC
			mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);
			mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = 0;
			IoReg_Write32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

			//disable main disp
			IoReg_ClearBits(MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_disp0_dma_enable_mask);
            IoReg_ClearBits(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_disp0c_dma_enable_mask);

			//wait double buffer apply
			drvif_memory_set_dbuffer_write(SLR_MAIN_DISPLAY);

			if(use_db_disable || mdomain_disp_disp0_db_ctrl_reg.disp0_db_en)
			{
				mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
				while(apply_timeout_cnt-- && (mdomain_disp_disp0_db_ctrl_reg.disp0_db_apply))
				{
					mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
					msleep(10);
				}
				if(!apply_timeout_cnt)
					rtd_pr_scaler_memory_err("[%s] %d using db to disable wait timeout!!\n", __FUNCTION__, __LINE__);
				else
					rtd_pr_scaler_memory_info("[%s] %d using db to disable\n", __FUNCTION__, __LINE__);
			}
			mdomain_disp_disp0_db_ctrl_reg.regValue =IoReg_Read32(MDOMAIN_DISP_Disp0_db_ctrl_reg);
			mdomain_disp_disp0_db_ctrl_reg.disp0_db_en=0;
			IoReg_Write32(MDOMAIN_DISP_Disp0_db_ctrl_reg,mdomain_disp_disp0_db_ctrl_reg.regValue);
		}
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
		else
		{
			//disable sub disp
            IoReg_ClearBits(MDOM_SUB_DISP_Disp1_db_ctrl_reg, MDOM_SUB_DISP_Disp1_db_ctrl_disp1_db_en_mask);
			IoReg_ClearBits(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask);

			//wait double buffer apply
			drvif_memory_set_dbuffer_write(SLR_SUB_DISPLAY);
		}
#endif
	}
}

void drvif_memory_EnableMCap(unsigned char display)
{
	rtd_pr_scaler_memory_info("[%s] %d (90K=%d) display=%d\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), display);

	IoReg_SetBits(display ? MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg : MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, display? MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_cap1_dma_enable_mask: MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_cap0_dma_enable_mask);
    if (dvrif_memory_get_block_mode_enable(display) == TRUE){
        IoReg_SetBits(display ? MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg : MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, display? MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_cap1c_dma_enable_mask: MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_cap0c_dma_enable_mask);
    }
 
	if (display == SLR_MAIN_DISPLAY)
	{
		/*wclr cap buf done status*/
        mdomain_cap_cap0_ddr_fifostatus_RBUS mdomain_cap_cap0_ddr_fifostatus_reg;
        mdomain_cap_cap0_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg);
        mdomain_cap_cap0_ddr_fifostatus_reg.cap0_dma_frame_end = 1;
        mdomain_cap_cap0_ddr_fifostatus_reg.cap0_first_wr_flag = 1;
        IoReg_Write32(MDOMAIN_CAP_Cap0_DDR_FIFOStatus_reg, mdomain_cap_cap0_ddr_fifostatus_reg.regValue);
	}
	else
	{
        mdom_sub_cap_cap1_ddr_fifostatus_RBUS mdom_sub_cap_cap1_ddr_fifostatus_reg;
        mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg);
        mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_dma_frame_end = 1;
        mdom_sub_cap_cap1_ddr_fifostatus_reg.cap1_first_wr_flag = 1;
        IoReg_Write32(MDOM_SUB_CAP_Cap1_DDR_FIFOStatus_reg, mdom_sub_cap_cap1_ddr_fifostatus_reg.regValue);
	}

}

void drvif_memory_DisableMCap(unsigned char display)
{
    mdomain_cap_frc_cap0_wr_ctrl_RBUS mdomain_cap_frc_cap0_wr_ctrl_reg;
    mdom_sub_cap_frc_cap1_wr_ctrl_RBUS mdom_sub_cap_frc_cap1_wr_ctrl_reg;
    mdomain_cap_frc_cap0c_wr_ctrl_RBUS mdomain_cap_frc_cap0c_wr_ctrl_reg;
    mdom_sub_cap_frc_cap1c_wr_ctrl_RBUS mdom_sub_cap_frc_cap1c_wr_ctrl_reg;

    rtd_pr_scaler_memory_info("[%s] %d (90K=%d) display=%d\n", __FUNCTION__, __LINE__, IoReg_Read32(TIMER_SCPU_CLK90K_LO_reg), display);

    if (SLR_MAIN_DISPLAY == display)
    {
        mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);

        if (TRUE == mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_enable)
        {
            //Disable MCap
            rtd_pr_scaler_memory_info("[Mdomain] Disable cap main dma and clear db\n");
            IoReg_ClearBits(MDOMAIN_CAP_cap0_reg_doublbuffer_reg, MDOMAIN_CAP_cap0_reg_doublbuffer_cap0_db_en_mask);

            mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_enable = 0;
            IoReg_Write32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, mdomain_cap_frc_cap0_wr_ctrl_reg.regValue);
        }

        mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
        if (TRUE == mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_enable)
        {
            rtd_pr_scaler_memory_info("[Mdomain] Disable cap main dmac and clear db\n");
            IoReg_ClearBits(MDOMAIN_CAP_cap0_reg_doublbuffer_reg, MDOMAIN_CAP_cap0_reg_doublbuffer_cap0_db_en_mask);

            mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
            mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_enable = 0;
            IoReg_Write32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
        }
    }
    else
    {
        mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);

        if (TRUE == mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_enable)
        {
        	//Disable MCap
        	rtd_pr_scaler_memory_info("[Mdomain] Disable cap sub dma and clear db\n");
            IoReg_ClearBits(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask);
            
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);
        	mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_enable = 0;
        	IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg, mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue);
        }

        mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
        if (TRUE == mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_enable)
        {
            rtd_pr_scaler_memory_info("[Mdomain] Disable cap sub dmac and clear db\n");
            IoReg_ClearBits(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_disp1_dma_enable_mask);
            
            mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
            mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_enable = 0;
            IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue);
        }
    }

}

#ifdef BUILD_QUICK_SHOW

static MDOMAIN_MALLOC_STRUCT mdomainBufferInfo[MDOMAIN_MAX] =
{
    {0, 0, 0},
    {0, 0, 0}
};

unsigned long get_vbm_heap_qshow_scaler_address(unsigned long *size);

void scaler_quickshow_memery_init(void)
{
    unsigned int i = 0;
    unsigned char ucStatus = 0;
    unsigned long ulAddr = 0 ;
    unsigned long ulGetSize = 0 ;
    unsigned long allocSize = 0;
    unsigned long main_max_size = 0, i3_max_size = 0, sub_max_size = 0;
	main_max_size = ((MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) > (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K)) ? (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) : (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K);
	sub_max_size = ((MDOMAIN_SUB_TWO_PIXEL_BUFFER_SIZE_PC_MODE) > (MDOMAIN_SUB_BUFFER_SIZE)) ? (MDOMAIN_SUB_TWO_PIXEL_BUFFER_SIZE_PC_MODE) : (MDOMAIN_SUB_BUFFER_SIZE);
	i3_max_size = (_2K_1K_24BIT_SIZE);//from I3DDMA_DolbyVision_HDMI_Init

    for(i = 0; i < MDOMAIN_MAX; i++)
    {
        ucStatus = mdomainBufferInfo[i].status;
        if(ucStatus != 0)
        {
            break;
        }
    }

    if(ucStatus == 0)
    {
        allocSize = main_max_size*3 + sub_max_size*3 + i3_max_size*3;

        ulAddr = get_vbm_heap_qshow_scaler_address(&ulGetSize);

        rtd_pr_scaler_memory_info("[%s %d]ulAddr:0x%08x, sulGetSize:0x%08x\n", __FUNCTION__, __LINE__, ulAddr, ulGetSize);

        if(ulAddr != 0 && (ulGetSize >= allocSize))
        {
            mdomainBufferInfo[MDOMAIN_MAIN].phyAddr = ulAddr;
            mdomainBufferInfo[MDOMAIN_MAIN].size = main_max_size*3 + i3_max_size*3;
            mdomainBufferInfo[MDOMAIN_MAIN].status = 1;

            mdomainBufferInfo[MDOMAIN_SUB].phyAddr = mdomainBufferInfo[MDOMAIN_MAIN].phyAddr + mdomainBufferInfo[MDOMAIN_MAIN].size;
            mdomainBufferInfo[MDOMAIN_SUB].size = sub_max_size*3;
            mdomainBufferInfo[MDOMAIN_SUB].status = 1;
        }
        else
        {
			printf2("[%s %d]qs memory get failed\n", __FUNCTION__, __LINE__);
        }
    }
}

unsigned long qs_get_memory_from_vbm(MDOMAIN_MEMORY_QUERY_ID id, unsigned long allocSize)
{
    unsigned long ulAddr = 0;

    if(id >= MDOMAIN_MAX || (mdomainBufferInfo[id].status == 0))
    {
		printf2("[%s %d]query id or status error id:%d\n", __FUNCTION__, __LINE__, id);
        return 0;
    }

    switch(id)
    {
        case MDOMAIN_MAIN:
            {
                if(judge_scaler_run_i3ddma_vo_path()){
                    //allocSize = i3 + mdomain max size
                    allocSize += (((MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) > (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K)) ? (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K120_CO_BUFFER) : (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K));
                }
                if(allocSize <= mdomainBufferInfo[MDOMAIN_MAIN].size)
                {
                    ulAddr = mdomainBufferInfo[MDOMAIN_MAIN].phyAddr;
                }
                else
                {
                    printf2("[%s %d]error get memory failed\n", __FUNCTION__, __LINE__);
                }
            }
            break;

        case MDOMAIN_SUB:
            {
                if(allocSize <= mdomainBufferInfo[MDOMAIN_SUB].size)
                {
                    ulAddr = mdomainBufferInfo[MDOMAIN_SUB].phyAddr;
                }
                else
                {
                    printf2("[%s %d]error get memory failed\n", __FUNCTION__, __LINE__);
                }
            }
            break;

        default:
            break;
    }

    rtd_pr_scaler_memory_info("[%s %d]ulAddr:0x%08x, id:%d, needSize:0x%08x, mdomainBufferInfo[MDOMAIN_SUB].size:0x%08x\n", __FUNCTION__, __LINE__, ulAddr, id, allocSize, mdomainBufferInfo[id].size);

    return ulAddr;
}
#endif

void memory_frc_decide_capture_mode(unsigned char display)
{
    if((Scaler_InputSrcGetMainChType() == _SRC_VO) && (Scaler_VOFrom(Scaler_Get_CurVoInfo_plane()) == 1))	// yychao+ : use single buffer for JPEG source
    {
        dvrif_memory_set_cap_mode(display, MEMCAPTYPE_FRAME);
    }
    else
    {
        if(dvrif_memory_get_compression_mode(display) == COMPRESSION_LINE_MODE)
        {
            dvrif_memory_set_cap_mode(display, MEMCAPTYPE_LINE);
        }
        else
        {
            dvrif_memory_set_cap_mode(display, MEMCAPTYPE_FRAME);
        }
    }
}

void memory_frc_decide_buffer_number(unsigned char display)
{
#ifdef ENABLE_3_BUFFER_DELAY_MODE
#ifndef BUILD_QUICK_SHOW
    if (get_video_delay_type() == M_DOMAIN_VIDEO_DELAY){
        dvrif_memory_set_buffer_number(display, 3 + get_scaler_video_delay_number());
    } else
#endif
    {
        dvrif_memory_set_buffer_number(display, 3);
    }
#else
    if((Scaler_InputSrcGetMainChType() == _SRC_VO) && (Scaler_VOFrom(Scaler_Get_CurVoInfo_plane()) == 1))
    {
        dvrif_memory_set_buffer_number(display, 1);
    } else {
        dvrif_memory_set_buffer_number(display, 3);
    }
#endif
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(display)){
        dvrif_memory_set_buffer_number(display, 1);
    }
#endif

}

void memory_set_capture_byte_swap(unsigned char display)
{
    mdomain_cap_frc_cap0_wr_ctrl_RBUS mdomain_cap_frc_cap0_wr_ctrl_reg;
    mdom_sub_cap_frc_cap1_wr_ctrl_RBUS mdom_sub_cap_frc_cap1_wr_ctrl_reg;

    //frank@03132013 add below code for GPU access YUV format ++
    if(display){

        mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg);
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_1byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_2byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_4byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_8byte_swap = 1;
        } else 
#endif
        if(Scaler_DispGetStatus(display,SLR_DISP_422CAP))
        {
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_1byte_swap = 0;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_2byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_4byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_8byte_swap = 0;
        }
        else
        {
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_1byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_2byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_4byte_swap = 1;
            mdom_sub_cap_frc_cap1_wr_ctrl_reg.cap1_dma_8byte_swap = 0;
        }
        IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1_WR_Ctrl_reg, mdom_sub_cap_frc_cap1_wr_ctrl_reg.regValue);
    }else{
        mdomain_cap_frc_cap0_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg);
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 1;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 1;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 1;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 1;
        } else 
#endif
        if(Scaler_DispGetStatus(display,SLR_DISP_10BIT))
        {
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 0;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 0;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 0;
            mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
        }
        else
        {
            if(Scaler_DispGetStatus(display,SLR_DISP_422CAP))
            {
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 0;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
            }
            else
            {
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_1byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_2byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_4byte_swap = 1;
                mdomain_cap_frc_cap0_wr_ctrl_reg.cap0_dma_8byte_swap = 0;
            }
        }
        IoReg_Write32(MDOMAIN_CAP_FRC_CAP0_WR_Ctrl_reg, mdomain_cap_frc_cap0_wr_ctrl_reg.regValue);
    }
}

void memory_set_capture_ds_path(unsigned char display)
{
    unsigned int CapWidth = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_WID);
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_CAP_LEN);

    if (display == SLR_MAIN_DISPLAY)
    {
        mdomain_cap_cap0_ds_ctrl_0_RBUS mdomain_cap_cap0_ds_ctrl_0_reg;
        mdomain_cap_cap0_pxl_wrap_ctrl_0_RBUS mdomain_cap_cap0_pxl_wrap_ctrl_0_reg;
        mdomain_cap_frc_cap0_pack_ctrl_RBUS mdomain_cap_frc_cap0_pack_ctrl_reg;

        rtd_pr_scaler_memory_emerg("[Mdomain][Main] check uzd sort_fmt = %d\n", SCALEDOWN_ICH1_UZD_Ctrl0_get_sort_fmt(IoReg_Read32(SCALEDOWN_ICH1_UZD_Ctrl0_reg)));
        
        // ds ctrl
        mdomain_cap_cap0_ds_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap0_DS_Ctrl_0_reg);
        mdomain_cap_cap0_ds_ctrl_0_reg.ds_hor_ds_mode = 0;
        mdomain_cap_cap0_ds_ctrl_0_reg.ds_ver_ds_mode = 0;
        mdomain_cap_cap0_ds_ctrl_0_reg.ds_image_h_size = CapWidth;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE) {
            mdomain_cap_cap0_ds_ctrl_0_reg.ds_ver_en = 3;
            mdomain_cap_cap0_ds_ctrl_0_reg.ds_hor_en = 0;
        } else {
            mdomain_cap_cap0_ds_ctrl_0_reg.ds_ver_en = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdomain_cap_cap0_ds_ctrl_0_reg.ds_hor_en = 0;
            } else {
                mdomain_cap_cap0_ds_ctrl_0_reg.ds_hor_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
            
        }
        IoReg_Write32(MDOMAIN_CAP_cap0_DS_Ctrl_0_reg, mdomain_cap_cap0_ds_ctrl_0_reg.regValue);

        rtd_pr_scaler_memory_emerg("[Mdomain][Main] ds_ver_en = %d, ds_hor_en = %d\n", mdomain_cap_cap0_ds_ctrl_0_reg.ds_ver_en, mdomain_cap_cap0_ds_ctrl_0_reg.ds_hor_en);

        // pxl wrap
        mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg);
        mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.disp0_fs_en = 0;
        mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_data_format = MEM_8_BIT;
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_wrap_mode = 7;
        } else {
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(MDOMAIN_CAP_cap0_Pxl_Wrap_Ctrl_0_reg, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.regValue);

        rtd_pr_scaler_memory_emerg("[Mdomain][Main] pack_wrap_mode = %d, pack_dummy_format = %d, pack_data_format = %d\n",
            mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_wrap_mode,  mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_dummy_format, mdomain_cap_cap0_pxl_wrap_ctrl_0_reg.pack_data_format);

        // pack ctrl
        mdomain_cap_frc_cap0_pack_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_frc_cap0_pack_ctrl_reg);
        mdomain_cap_frc_cap0_pack_ctrl_reg.cap0_frame_acc_mode = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        mdomain_cap_frc_cap0_pack_ctrl_reg.cap0_vact = drvif_memory_get_data_align(CapLength, 2);
        mdomain_cap_frc_cap0_pack_ctrl_reg.cap0_hact = drvif_memory_get_data_align(CapWidth, 2);
        IoReg_Write32(MDOMAIN_CAP_frc_cap0_pack_ctrl_reg, mdomain_cap_frc_cap0_pack_ctrl_reg.regValue);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdomain_cap_frc_cap0c_pack_ctrl_RBUS mdomain_cap_frc_cap0c_pack_ctrl_reg;
            mdomain_cap_frc_cap0c_pack_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_frc_cap0c_pack_ctrl_reg);
            mdomain_cap_frc_cap0c_pack_ctrl_reg.cap0c_vact = drvif_memory_get_data_align(CapLength / 2, 2);
            mdomain_cap_frc_cap0c_pack_ctrl_reg.cap0c_hact = drvif_memory_get_data_align(CapWidth, 2);
            IoReg_Write32(MDOMAIN_CAP_frc_cap0c_pack_ctrl_reg, mdomain_cap_frc_cap0c_pack_ctrl_reg.regValue);
        }
    } else
    {
        mdom_sub_cap_cap1_ds_ctrl_0_RBUS mdom_sub_cap_cap1_ds_ctrl_0_reg;
        mdom_sub_cap_cap1_pxl_wrap_ctrl_0_RBUS mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg;
        mdom_sub_cap_frc_cap1_pack_ctrl_RBUS mdom_sub_cap_frc_cap1_pack_ctrl_reg;
        
        rtd_pr_scaler_memory_emerg("[Mdomain][Sub] check uzd sort_fmt = %d\n", SCALEDOWN_ICH2_UZD_Ctrl0_get_sort_fmt(IoReg_Read32(SCALEDOWN_ICH2_UZD_Ctrl0_reg)));
        
            // ds ctrl
        mdom_sub_cap_cap1_ds_ctrl_0_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_cap1_DS_Ctrl_0_reg);
        mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_hor_ds_mode = 0;
        mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_ver_ds_mode = 0;
        mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_image_h_size = CapWidth;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_ver_en = 3;
            mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_hor_en = 0;
        } else {
            mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_ver_en = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_hor_en = 0;
            } else {
                mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_hor_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
        }
        IoReg_Write32(MDOM_SUB_CAP_cap1_DS_Ctrl_0_reg, mdom_sub_cap_cap1_ds_ctrl_0_reg.regValue);

        rtd_pr_scaler_memory_emerg("[Mdomain][Sub] ds_ver_en = %d, ds_hor_en = %d\n", mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_ver_en, mdom_sub_cap_cap1_ds_ctrl_0_reg.ds_hor_en);

        // pxl wrap
        mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_reg);
        mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.disp1_fs_en = 0;
        mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);

        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_data_format = MEM_8_BIT;
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_wrap_mode = 7;
        } else {
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(MDOM_SUB_CAP_cap1_Pxl_Wrap_Ctrl_0_reg, mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.regValue);
        
        rtd_pr_scaler_memory_emerg("[Mdomain][Sub] pack_wrap_mode = %d, pack_dummy_format = %d, pack_data_format = %d\n",
            mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_wrap_mode,  mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_dummy_format, mdom_sub_cap_cap1_pxl_wrap_ctrl_0_reg.pack_data_format);

        // pack ctrl
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_frc_cap1_pack_ctrl_reg);
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.cap1_frame_acc_mode = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.cap1_vact = drvif_memory_get_data_align(CapLength, 2);
        mdom_sub_cap_frc_cap1_pack_ctrl_reg.cap1_hact = drvif_memory_get_data_align(CapWidth, 2);
        IoReg_Write32(MDOM_SUB_CAP_frc_cap1_pack_ctrl_reg, mdom_sub_cap_frc_cap1_pack_ctrl_reg.regValue);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdom_sub_cap_frc_cap1c_pack_ctrl_RBUS mdom_sub_cap_frc_cap1c_pack_ctrl_reg;
            mdom_sub_cap_frc_cap1c_pack_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_frc_cap1c_pack_ctrl_reg);
            mdom_sub_cap_frc_cap1c_pack_ctrl_reg.cap1c_vact = drvif_memory_get_data_align(CapLength / 2, 2);
            mdom_sub_cap_frc_cap1c_pack_ctrl_reg.cap1c_hact = drvif_memory_get_data_align(CapWidth, 2);
            IoReg_Write32(MDOM_SUB_CAP_frc_cap1c_pack_ctrl_reg, mdom_sub_cap_frc_cap1c_pack_ctrl_reg.regValue);
        }
    }
}

void memory_main_set_wdma(void)
{
	mdomain_cap_frc_cap0_num_bl_wrap_line_step_RBUS mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg;
	mdomain_cap_frc_cap0_num_bl_wrap_ctl_RBUS mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg;
	mdomain_cap_frc_cap0_num_bl_wrap_word_RBUS mdomain_cap_frc_cap0_num_bl_wrap_word_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN);

	if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY) == TRUE)
	{
		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
		{
			TotalSize = memory_get_capture_line_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));//compression test
		}
		else{
			TotalSize = memory_get_capture_frame_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));
		}
	} else
    {
        TotalSize = memory_get_capture_size(SLR_MAIN_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Main] RDMA0 TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

	mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_line_step_reg);
	mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg.cap0_line_step = LineStep;
	IoReg_Write32(MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_line_step_reg, mdomain_cap_frc_cap0_num_bl_wrap_line_step_reg.regValue);


	// cap0_burst_len & cap0_line_num
	mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_ctl_reg);
	mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.cap0_line_num = CapLength;
	mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.cap0_burst_len = _WDMA_BURSTLENGTH;
	IoReg_Write32(MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_ctl_reg, mdomain_cap_frc_cap0_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
	mdomain_cap_frc_cap0_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_word_reg);
	mdomain_cap_frc_cap0_num_bl_wrap_word_reg.cap0_addr_toggle_mode = 0;
	mdomain_cap_frc_cap0_num_bl_wrap_word_reg.cap0_line_burst_num = TotalSize;
	IoReg_Write32(MDOMAIN_CAP_FRC_CAP0_num_bl_wrap_word_reg, mdomain_cap_frc_cap0_num_bl_wrap_word_reg.regValue);

    // byte_swap
    memory_set_capture_byte_swap(SLR_MAIN_DISPLAY);

    // outstanding
    mcap_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);

}

void memory_sub_set_wdma(void)
{
	mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_RBUS mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg;
	mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_RBUS mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg;
	mdom_sub_cap_frc_cap1_num_bl_wrap_word_RBUS mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));//compression test
        }
        else{
            TotalSize = memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));
        }
    } else {
        TotalSize = memory_get_capture_size(SLR_SUB_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] RDMA1 TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

    mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_line_step_reg);
    mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg.cap1_line_step = LineStep;
    IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_line_step_reg, mdom_sub_cap_frc_cap1_num_bl_wrap_line_step_reg.regValue);

	// cap0_burst_len & cap0_line_num
    mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_ctl_reg);
    mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.cap1_line_num = CapLength;
    mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.cap1_burst_len = _WDMA_BURSTLENGTH;
    IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_ctl_reg, mdom_sub_cap_frc_cap1_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
    mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_word_reg);
    mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.cap1_addr_toggle_mode = 0;
    mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.cap1_line_burst_num = TotalSize;
    IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1_num_bl_wrap_word_reg, mdom_sub_cap_frc_cap1_num_bl_wrap_word_reg.regValue);

    // byte_swap
    memory_set_capture_byte_swap(SLR_SUB_DISPLAY);

    // outstanding
    mcap_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);

}

void memory_main_set_wdma_c(void)
{
    mdomain_cap_frc_cap0c_num_bl_wrap_line_step_RBUS mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg;
    mdomain_cap_frc_cap0c_num_bl_wrap_ctl_RBUS mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg;
    mdomain_cap_frc_cap0c_num_bl_wrap_word_RBUS mdomain_cap_frc_cap0c_num_bl_wrap_word_reg;
    mdomain_cap_frc_cap0c_wr_ctrl_RBUS mdomain_cap_frc_cap0c_wr_ctrl_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN);

    if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY) == TRUE)
	{
		if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
		{
			TotalSize = memory_get_capture_line_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));//compression test
		}
		else{
			TotalSize = memory_get_capture_frame_size_compression(SLR_MAIN_DISPLAY, dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));
		}
	} else
    {
        TotalSize = memory_get_capture_size(SLR_MAIN_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Main] RDMA0C TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

    mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_line_step_reg);
    mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg.cap0c_line_step = LineStep;
    IoReg_Write32(MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_line_step_reg, mdomain_cap_frc_cap0c_num_bl_wrap_line_step_reg.regValue);

	// cap0_burst_len & cap0_line_num
    mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_ctl_reg);
    mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.cap0c_line_num = CapLength;
    mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.cap0c_burst_len = _WDMA_BURSTLENGTH;
    IoReg_Write32(MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_ctl_reg, mdomain_cap_frc_cap0c_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
    mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_word_reg);
    mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.cap0c_addr_toggle_mode = 0;
    mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.cap0c_line_burst_num = TotalSize;
    IoReg_Write32(MDOMAIN_CAP_FRC_CAP0C_num_bl_wrap_word_reg, mdomain_cap_frc_cap0c_num_bl_wrap_word_reg.regValue);

    // byte_swap
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
        mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_8byte_swap = 1;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_4byte_swap = 1;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_2byte_swap = 1;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_1byte_swap = 1;
        IoReg_Write32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
    } else
#endif
    {
        mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg);
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_8byte_swap = 0;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_4byte_swap = 0;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_2byte_swap = 0;
        mdomain_cap_frc_cap0c_wr_ctrl_reg.cap0c_dma_1byte_swap = 0;
        IoReg_Write32(MDOMAIN_CAP_FRC_CAP0C_WR_Ctrl_reg, mdomain_cap_frc_cap0c_wr_ctrl_reg.regValue);
    }


    // outstanding
    mcap_dmac_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);

}

void memory_sub_set_wdma_c(void)
{
    mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_RBUS mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg;
    mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_RBUS mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg;
    mdom_sub_cap_frc_cap1c_num_bl_wrap_word_RBUS mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg;
    mdom_sub_cap_frc_cap1c_wr_ctrl_RBUS mdom_sub_cap_frc_cap1c_wr_ctrl_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));//compression test
        }
        else{
            TotalSize = memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));
        }
    } else {
        TotalSize = memory_get_capture_size(SLR_SUB_DISPLAY);
    }

	// cap0_line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] RDMA1C TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

    mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_line_step_reg);
    mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg.cap1c_line_step = LineStep;
    IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_line_step_reg, mdom_sub_cap_frc_cap1c_num_bl_wrap_line_step_reg.regValue);

	// cap0_burst_len & cap0_line_num
    mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_ctl_reg);
    mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.cap1c_line_num = CapLength;
    mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.cap1c_burst_len = _WDMA_BURSTLENGTH;
    IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_ctl_reg, mdom_sub_cap_frc_cap1c_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
    mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_word_reg);
    mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.cap1c_addr_toggle_mode = 0;
    mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.cap1c_line_burst_num = TotalSize;
    IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1C_num_bl_wrap_word_reg, mdom_sub_cap_frc_cap1c_num_bl_wrap_word_reg.regValue);

    // byte_swap
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg);
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_8byte_swap = 0;
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_4byte_swap = 0;
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_2byte_swap = 0;
    mdom_sub_cap_frc_cap1c_wr_ctrl_reg.cap1c_dma_1byte_swap = 0;
    IoReg_Write32(MDOM_SUB_CAP_FRC_CAP1C_WR_Ctrl_reg, mdom_sub_cap_frc_cap1c_wr_ctrl_reg.regValue);

    // outstanding
    mcap_dmac_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);

}

unsigned int memory_frc_main_decide_buffer_size(void)
{
	unsigned int buffer_size = 0;
    unsigned char display = SLR_MAIN_DISPLAY;
	if (is_m_domain_data_frc_need_borrow_memory()) {
		buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K;//using pc mode size
		rtd_pr_scaler_memory_emerg("[4k60VRR] memory buffer_size=%d (MDOMAIN_MAIN_BUFFER_SIZE_16BITS_4K)\n", buffer_size);
	} else if(dvrif_memory_compression_get_enable(display)==_ENABLE) {
		//RGB444 mode use 24bits @Crixus 20160902
		//K6 M-domain =13M, so only can use 16bit for PQC
		if (dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY)==COMPRESSION_16_BITS) {
			//buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_24BITS;
				buffer_size = drv_get_caveout_buffer_size_by_platform(MDOMAIN_16BITS);
		} else {
				buffer_size = drv_get_caveout_buffer_size_by_platform(MDOMAIN_12BITS);
		}

		if(drv_memory_Get_multibuffer_flag()) {
			buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_MULT4K_12BITS;
		}
	} else {/*qiangzhou:gamemode and livezoom not open compress mode now,
					so borrow di&rtnr memory,so video should not enter di&rtnr in this case*/
#ifndef BUILD_QUICK_SHOW
		if((display == SLR_MAIN_DISPLAY) && Get_rotate_function(SLR_MAIN_DISPLAY)) {
			buffer_size = drv_get_caveout_buffer_size_by_platform(MDOMAIN_12BITS);
		} else
#endif
		{
			buffer_size = drv_get_caveout_buffer_size_by_platform(MDOMAIN_16BITS);
		}
	}

	if(vdec_data_frc_need_borrow_cma_buffer(display)){
		buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_12BITS_4K;
	}

#ifdef CONFIG_RTK_KDRV_DV_IDK_DUMP
    buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_30BITS;
    rtd_pr_scaler_memory_info("[%s:%d] force go to 30 bit buffer %d\n", __func__, __LINE__, buffer_size);
#endif

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)){
        buffer_size = Get_Val_rotate_block_size();
    }
#endif

#ifdef CONFIG_MDOMAIN_FORCE_CARVEOUT
    buffer_size = MDOMAIN_CMP_LINE_MODE_BUFFER_SIZE(1920, 1080, 30);
#endif
 
    dvrif_memory_set_buffer_size(SLR_MAIN_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_emerg("[Mdomain][Main] frc buffer size = %x\n", buffer_size);

	return buffer_size;
}

unsigned int memory_frc_sub_decide_buffer_size(void)
{
    unsigned int buffer_size = 0;
    //sub use cap max size @Crixus 20151231

    buffer_size = MDOMAIN_SUB_BUFFER_SIZE;

    dvrif_memory_set_buffer_size(SLR_SUB_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] frc buffer size = %x\n", buffer_size);

    return buffer_size;
}

void memory_frc_main_alloc_buffer(void)
{
    unsigned char display = SLR_MAIN_DISPLAY;
#ifdef CONFIG_ARM64
    unsigned long align_addr = 0, query_addr = 0;
#else
    unsigned int align_addr = 0, query_addr = 0;
#endif
#ifndef CONFIG_ALL_SOURCE_DATA_FS
	unsigned int buffer_size_page_align = 0;
#endif
    void *m_domain_borrow_buffer = NULL;
#ifdef BUILD_QUICK_SHOW
	unsigned long i3ddma_highest_phyaddr = 0;
#endif
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

        //merlin4 memory do 96 bytes align @Crixus 20180321
    query_addr = get_query_start_address(QUERY_IDX_MDOMAIN_MAIN);
    buffer_size = drvif_memory_get_data_align(buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    align_addr = dvr_memory_alignment((unsigned long)query_addr, buffer_size);
#ifdef CONFIG_ARM64
    if (align_addr == (unsigned long)NULL)
            BUG();
#else
    if (align_addr == (unsigned int)NULL)
            BUG();
#endif

#ifdef CONFIG_RTK_KDRV_DV_IDK_DUMP
    MemTag[MEMIDX_MAIN].StartAddr = align_addr;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
#else
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = align_addr;
 //   #ifndef CONFIG_MDOMAIN_FORCE_CARVEOUT
    if (get_platform_model() == PLATFORM_MODEL_5K)
 //   #else
   // if (1)
  //  #endif
    {
    
        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    }
	else if (drv_memory_Get_multibuffer_flag()){
        rtd_pr_scaler_memory_info("[VideoDelay] multibuffer update mem tag\n");
        MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[7].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[6].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_SIXTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[5].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_FIFTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[4].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_FOURTH].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[3].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[2].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[1].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(s_i3ddma_sw_cap_buffer[0].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
    }
    else if(
#ifndef BUILD_QUICK_SHOW
        is_m_domain_data_frc_need_borrow_memory() &&
#else
        judge_scaler_run_i3ddma_vo_path() ||
#endif
        (m_domain_4k_memory_from_type() == BORROW_FROM_VBM))
    {
        if(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr)
        {
#ifdef BUILD_QUICK_SHOW
            if(judge_scaler_run_i3ddma_vo_path()){
                //i3 use vbm first half buffer , m-domain use second half buffer
                i3ddma_highest_phyaddr = rtd_inl(H3DDMA_CAP0_CTI_DMA_WR_Rule_check_up_1_reg);
                MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(i3ddma_highest_phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
                MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(i3ddma_highest_phyaddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(i3ddma_highest_phyaddr + buffer_size*2, DMA_SPEEDUP_ALIGN_VALUE);
            }else
#endif
            {
                MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr, DMA_SPEEDUP_ALIGN_VALUE);
                MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(i3ddmaCtrl[I3DDMA_DEV].cap_buffer[0].phyaddr + buffer_size*2, DMA_SPEEDUP_ALIGN_VALUE);
            }
        }
        else
        {
#ifdef CONFIG_MDOMAIN_FORCE_CARVEOUT
            if (1){
                MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
            }
#else
#ifndef BUILD_QUICK_SHOW
            rtd_pr_scaler_memory_err("##[ERR] 4k no memory cap##\r\n");
#else
            printf2("2 main modomain no memory\n");
#endif
            set_m_domain_setting_err_status(display, MDOMAIN_SETTING_ERR_MAIN_NO_MEM); // set error status and don't enable Mcap_en later
            return;
#endif
        }
    }
#ifdef CONFIG_MDOMAIN_FORCE_CARVEOUT
	else if (1){  
        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    }
#endif
    else
    {
#ifndef BUILD_QUICK_SHOW
        if ((vdec_data_frc_need_borrow_cma_buffer(display)) || (is_m_domain_data_frc_need_borrow_memory() && m_domain_4k_memory_from_type() == BORROW_FROM_CMA)) {//4k frc and need borrow memory from cma
            unsigned long phy_addr_buffer_memory;
            unsigned char borrow_memory = TRUE;//need borrow memory from cma
            unsigned int borrow_size;//borrow size
            if(vdec_data_frc_need_borrow_cma_buffer(display)){
                borrow_size =  buffer_size;// borrow 1 buffer
            }else{
                borrow_size = 2 * buffer_size;
            }
            rtd_pr_scaler_memory_notice("##func:%s main m domain borrow memory from CMA size:0x%x ###\r\n", __FUNCTION__, borrow_size);
            if(get_m_domain_borrow_buffer())
            {//alread has size from cma
                if(borrow_size <= get_m_domain_borrow_size())
                {//no need to borrow again
                    borrow_memory = FALSE;
                    m_domain_borrow_buffer = get_m_domain_borrow_buffer();//get original addr
                    phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                    if(vdec_data_frc_need_borrow_cma_buffer(display)){
                        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }else{
                        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }
                }
                else
                    set_m_domain_borrow_buffer(NULL, 0);//free memory before borrow big size
            }
            if(borrow_memory)
            {//need borrrow memory from cam
                m_domain_borrow_buffer = dvr_malloc(borrow_size);
                if(m_domain_borrow_buffer)
                {
                    phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                    set_m_domain_borrow_buffer(m_domain_borrow_buffer, borrow_size);//save addr and size
                    if(vdec_data_frc_need_borrow_cma_buffer(display)){
                        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }else{
                        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
                    }
                }
                else
                {
                    rtd_pr_scaler_memory_err("###[Bug] func:%s no memory from cma ##\r\n",__FUNCTION__);
                    set_m_domain_setting_err_status(display, MDOMAIN_SETTING_ERR_MAIN_CMA_ALLOC_FAIL); // set error status and don't enable Mcap_en later
                    return;
                }
            }
        } else
#endif
        {
           	MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        }
        if(vdec_data_frc_need_borrow_cma_buffer(display)){
            MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        }else{
            MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        }
    }
#endif

    if(drv_memory_Get_multibuffer_flag())
    {
        MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = MemTag[MEMIDX_MAIN_EIGHTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = MemTag[MEMIDX_MAIN_SEVENTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_SIXTH].StartAddr = MemTag[MEMIDX_MAIN_SIXTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_FIFTH].StartAddr = MemTag[MEMIDX_MAIN_FIFTH].StartAddr & 0xfffffff0;
        MemTag[MEMIDX_MAIN_FOURTH].StartAddr = MemTag[MEMIDX_MAIN_FOURTH].StartAddr & 0xfffffff0;
    }
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr & 0xfffffff0;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr& 0xfffffff0;
    MemTag[MEMIDX_MAIN].StartAddr = MemTag[MEMIDX_MAIN].StartAddr & 0xfffffff0;

    //free virtual memory
    //drvif_memory_free_fixed_virtual_memory();
    //set virtual memory size
    //rtd_pr_scaler_memory_emerg("buffer_size=%x\n", buffer_size);
    buffer_size_page_align = PAGE_ALIGN(buffer_size);
    //buffer_size = buffer_size_page_align;
    rtd_pr_scaler_memory_emerg("memory_set_capture PAGE_ALIGN(buffer_size)=%x\n", PAGE_ALIGN(buffer_size));
    drvif_memory_set_fixed_virtual_memory_size(buffer_size_page_align);
    MemTag[MEMIDX_MAIN_THIRD].Allocate_StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr;
    MemTag[MEMIDX_MAIN_THIRD].Size = buffer_size;
    MemTag[MEMIDX_MAIN_THIRD].Status = ALLOCATED_FROM_PLI;

    MemTag[MEMIDX_MAIN_SEC].Allocate_StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr;
    MemTag[MEMIDX_MAIN_SEC].Size = buffer_size;
    MemTag[MEMIDX_MAIN_SEC].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN_SEC].Allocate_VirAddr = 0;//not use

    MemTag[MEMIDX_MAIN].Allocate_StartAddr = MemTag[MEMIDX_MAIN].StartAddr;
    MemTag[MEMIDX_MAIN].Size = buffer_size;
    MemTag[MEMIDX_MAIN].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN].Allocate_VirAddr = 0;//not use

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY))
    {
        unsigned int c_offset = drvif_memory_get_data_align(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID) * 8, 128) / 8 * Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN);

        MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN].StartAddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_THIRD].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_FOURTH].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size, DMA_SPEEDUP_ALIGN_VALUE);

        MemTag[MEMIDX_MAIN_C].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN].StartAddr + c_offset, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_C_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr + c_offset, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_C_THIRD].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr + c_offset, DMA_SPEEDUP_ALIGN_VALUE);
        MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_FOURTH].StartAddr + c_offset, DMA_SPEEDUP_ALIGN_VALUE);
    }
#endif
}

void memory_frc_sub_alloc_buffer(void)
{
#ifdef ENABLE_3_BUFFER_DELAY_MODE
	void *sub_3rd_buffer;
#endif
	void *sub_2nd_buffer;
	void *sub_1st_buffer;
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);

#ifndef BUILD_QUICK_SHOW
#ifdef ENABLE_3_BUFFER_DELAY_MODE
		//I2rnd mode s-cap use 2-buffer
		sub_3rd_buffer = get_sub_m_domain_3rd_buf_address();
		if (sub_3rd_buffer != NULL)
			dvr_free(sub_3rd_buffer);

		sub_3rd_buffer = dvr_malloc(buffer_size);
		set_sub_m_domain_3rd_buf_address(sub_3rd_buffer);
		MemTag[MEMIDX_SUB_THIRD].StartAddr = dvr_to_phys(sub_3rd_buffer);
		rtd_pr_scaler_memory_info("set M-domain sub 3rd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_3rd_buffer, MemTag[MEMIDX_SUB_THIRD].StartAddr, buffer_size);
#endif

	sub_2nd_buffer = get_sub_m_domain_2nd_buf_address();
	if (sub_2nd_buffer != NULL)
		dvr_free(sub_2nd_buffer);

	sub_2nd_buffer = dvr_malloc(buffer_size);
	set_sub_m_domain_2nd_buf_address(sub_2nd_buffer);
	MemTag[MEMIDX_SUB_SEC].StartAddr = dvr_to_phys(sub_2nd_buffer);
	rtd_pr_scaler_memory_info("set M-domain sub 2nd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_2nd_buffer, MemTag[MEMIDX_SUB_SEC].StartAddr, buffer_size);


	sub_1st_buffer = get_sub_m_domain_1st_buf_address();
	if (sub_1st_buffer != NULL)
		dvr_free(sub_1st_buffer);

	sub_1st_buffer = dvr_malloc(buffer_size);
	set_sub_m_domain_1st_buf_address(sub_1st_buffer);
	MemTag[MEMIDX_SUB].StartAddr = dvr_to_phys(sub_1st_buffer);
	rtd_pr_scaler_memory_info("set M-domain sub 1st buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_1st_buffer, MemTag[MEMIDX_SUB].StartAddr, buffer_size);
#else
	unsigned long startAddr = 0;
	startAddr = qs_get_memory_from_vbm(MDOMAIN_SUB, buffer_size * 3);
	if(startAddr != 0)
	{
		set_sub_m_domain_3rd_buf_address((void *)startAddr);
		MemTag[MEMIDX_SUB_THIRD].StartAddr = startAddr;
		set_sub_m_domain_2nd_buf_address((void *)(startAddr + buffer_size));
		MemTag[MEMIDX_SUB_SEC].StartAddr = startAddr + buffer_size;
		MemTag[MEMIDX_SUB].StartAddr = startAddr + buffer_size * 2;
		rtd_pr_scaler_memory_info("qs mdomain sub 3 buffer 3rd:0x%08x, 2nd:0x%08x, 1st:0x%08x, startAddr:0x%08x, needSize:0x%08x\n", MemTag[MEMIDX_SUB_THIRD].StartAddr, MemTag[MEMIDX_SUB_SEC].StartAddr, MemTag[MEMIDX_SUB].StartAddr, startAddr, buffer_size * 3);
	}
	else
	{
		printf2("qs sub modomain no memory\n");
	}
#endif

}

void memory_main_update_capture_buffer(void)
{
    mdomain_cap_cap0_boundaryaddr_ctrl0_RBUS mdomain_cap_cap0_boundaryaddr_ctrl0_reg;
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

    rtd_pr_scaler_memory_emerg("[Mdomain]update mcap buffer number = %d\n", cap_num);

    //M-domain start address
    if(cap_num > 0){

        IoReg_Write32(MDOMAIN_CAP_cap0_first_buf_addr_reg, MemTag[MEMIDX_MAIN].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN].StartAddr = %lx\n", MemTag[MEMIDX_MAIN].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN].StartAddr = %x\n", MemTag[MEMIDX_MAIN].StartAddr);
    #endif
    }

    if(cap_num > 1){

        IoReg_Write32(MDOMAIN_CAP_cap0_second_buf_addr_reg, MemTag[MEMIDX_MAIN_SEC].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEC].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_SEC].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEC].StartAddr = %x\n", MemTag[MEMIDX_MAIN_SEC].StartAddr);
    #endif
    }

    if(cap_num > 2){

        IoReg_Write32(MDOMAIN_CAP_cap0_third_buf_addr_reg, MemTag[MEMIDX_MAIN_THIRD].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_THIRD].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_THIRD].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_THIRD].StartAddr = %x\n", MemTag[MEMIDX_MAIN_THIRD].StartAddr);
    #endif
    }

    if(cap_num > 3){

        IoReg_Write32(MDOMAIN_CAP_cap0_forth_buf_addr_reg, MemTag[MEMIDX_MAIN_FOURTH].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FOURTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_FOURTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FOURTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_FOURTH].StartAddr);
    #endif
    }

    if(cap_num > 4){

        IoReg_Write32(MDOMAIN_CAP_cap0_fifth_buf_addr_reg, MemTag[MEMIDX_MAIN_FIFTH].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FIFTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_FIFTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_FIFTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_FIFTH].StartAddr);
    #endif
    }

    if(cap_num > 5){

        IoReg_Write32(MDOMAIN_CAP_cap0_sixth_buf_addr_reg, MemTag[MEMIDX_MAIN_SIXTH].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SIXTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_SIXTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SIXTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_SIXTH].StartAddr);
    #endif
    }

    if(cap_num > 6){

        IoReg_Write32(MDOMAIN_CAP_cap0_seventh_buf_addr_reg, MemTag[MEMIDX_MAIN_SEVENTH].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_SEVENTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_SEVENTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_SEVENTH].StartAddr);
    #endif
    }

    if(cap_num > 7){

        IoReg_Write32(MDOMAIN_CAP_cap0_eightth_buf_addr_reg, MemTag[MEMIDX_MAIN_EIGHTH].StartAddr);
        IoReg_Write32(MDOMAIN_CAP_Cap0_downlimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOMAIN_CAP_Cap0_uplimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = %lx\n", MemTag[MEMIDX_MAIN_EIGHTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_MAIN_EIGHTH].StartAddr = %x\n", MemTag[MEMIDX_MAIN_EIGHTH].StartAddr);
    #endif
    }

    //M-domain boundary
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_BoundaryAddr_CTRL0_reg);
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk7_sel = 7;
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk6_sel = 6;
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk5_sel = 5;
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk4_sel = 4;
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk3_sel = 3;
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk2_sel = 2;
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk1_sel = 1;
    mdomain_cap_cap0_boundaryaddr_ctrl0_reg.cap0_blk0_sel = 0;
    IoReg_Write32(MDOMAIN_CAP_Cap0_BoundaryAddr_CTRL0_reg, mdomain_cap_cap0_boundaryaddr_ctrl0_reg.regValue);

}

void memory_sub_update_capture_buffer(void)
{
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_RBUS mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg;
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_SUB_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);

    if(cap_num > 0){

        IoReg_Write32(MDOM_SUB_CAP_cap1_first_buf_addr_reg, MemTag[MEMIDX_SUB].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB].StartAddr = %lx\n", MemTag[MEMIDX_SUB].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB].StartAddr = %x\n", MemTag[MEMIDX_SUB].StartAddr);
    #endif
    }

    if(cap_num > 1){

        IoReg_Write32(MDOM_SUB_CAP_cap1_second_buf_addr_reg, MemTag[MEMIDX_SUB_SEC].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEC].StartAddr = %lx\n", MemTag[MEMIDX_SUB_SEC].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEC].StartAddr = %x\n", MemTag[MEMIDX_SUB_SEC].StartAddr);
    #endif
    }

    if(cap_num > 2){

        IoReg_Write32(MDOM_SUB_CAP_cap1_third_buf_addr_reg, MemTag[MEMIDX_SUB_THIRD].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_THIRD].StartAddr = %lx\n", MemTag[MEMIDX_SUB_THIRD].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_THIRD].StartAddr = %x\n", MemTag[MEMIDX_SUB_THIRD].StartAddr);
    #endif
    }

    if(cap_num > 3){

        IoReg_Write32(MDOM_SUB_CAP_cap1_forth_buf_addr_reg, MemTag[MEMIDX_SUB_FOURTH].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FOURTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_FOURTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FOURTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_FOURTH].StartAddr);
    #endif
    }

    if(cap_num > 4){

        IoReg_Write32(MDOM_SUB_CAP_cap1_fifth_buf_addr_reg, MemTag[MEMIDX_SUB_FIFTH].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FIFTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_FIFTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_FIFTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_FIFTH].StartAddr);
    #endif
    }

    if(cap_num > 5){

        IoReg_Write32(MDOM_SUB_CAP_cap1_sixth_buf_addr_reg, MemTag[MEMIDX_SUB_SIXTH].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SIXTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_SIXTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SIXTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_SIXTH].StartAddr);
    #endif
    }

    if(cap_num > 6){

        IoReg_Write32(MDOM_SUB_CAP_cap1_seventh_buf_addr_reg, MemTag[MEMIDX_SUB_SEVENTH].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEVENTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_SEVENTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_SEVENTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_SEVENTH].StartAddr);
    #endif
    }

    if(cap_num > 7){

        IoReg_Write32(MDOM_SUB_CAP_cap1_eightth_buf_addr_reg, MemTag[MEMIDX_SUB_EIGHTH].StartAddr);
        IoReg_Write32(MDOM_SUB_CAP_Cap1_downlimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr & 0xfffffff0));
        IoReg_Write32(MDOM_SUB_CAP_Cap1_uplimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    #ifdef CONFIG_ARM64
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_EIGHTH].StartAddr = %lx\n", MemTag[MEMIDX_SUB_EIGHTH].StartAddr);
    #else
        rtd_pr_scaler_memory_emerg("[Mdomain]MemTag[MEMIDX_SUB_EIGHTH].StartAddr = %x\n", MemTag[MEMIDX_SUB_EIGHTH].StartAddr);
    #endif
    }

    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_BoundaryAddr_CTRL0_reg);
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk7_sel = 7;
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk6_sel = 6;
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk5_sel = 5;
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk4_sel = 4;
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk3_sel = 3;
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk2_sel = 2;
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk1_sel = 1;
    mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.cap1_blk0_sel = 0;
    IoReg_Write32(MDOM_SUB_CAP_Cap1_BoundaryAddr_CTRL0_reg, mdom_sub_cap_cap1_boundaryaddr_ctrl0_reg.regValue);

}

void memory_main_update_capture_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(MDOMAIN_CAP_cap0c_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(MDOMAIN_CAP_cap0c_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOMAIN_CAP_cap0c_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOMAIN_CAP_cap0c_forth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOMAIN_CAP_cap0c_fifth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOMAIN_CAP_cap0c_sixth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOMAIN_CAP_cap0c_seventh_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOMAIN_CAP_cap0c_eightth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_CAP_Cap0c_downlimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_CAP_Cap0c_uplimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

void memory_sub_update_capture_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(MDOM_SUB_CAP_cap1c_first_buf_addr_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(MDOM_SUB_CAP_cap1c_second_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOM_SUB_CAP_cap1c_third_buf_addr_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOM_SUB_CAP_cap1c_forth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOM_SUB_CAP_cap1c_fifth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOM_SUB_CAP_cap1c_sixth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOM_SUB_CAP_cap1c_seventh_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOM_SUB_CAP_cap1c_eightth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_CAP_Cap1c_downlimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_CAP_Cap1c_uplimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

void memory_main_update_display_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(MDOMAIN_DISP_disp0c_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_0_reg, (MemTag[MEMIDX_MAIN_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(MDOMAIN_DISP_disp0c_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_1_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOMAIN_DISP_disp0c_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_2_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOMAIN_DISP_disp0c_forth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_3_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOMAIN_DISP_disp0c_fifth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_4_reg, (MemTag[MEMIDX_MAIN_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOMAIN_DISP_disp0c_sixth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_5_reg, (MemTag[MEMIDX_MAIN_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOMAIN_DISP_disp0c_seventh_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_6_reg, (MemTag[MEMIDX_MAIN_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOMAIN_DISP_disp0c_eightth_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_7_reg, (MemTag[MEMIDX_MAIN_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

void memory_sub_update_display_C_buffer(unsigned char cap_num, unsigned int buffer_size)
{
    if(cap_num > 0){

    IoReg_Write32(MDOM_SUB_DISP_disp1c_first_buf_addr_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);

    IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_0_reg, (MemTag[MEMIDX_SUB_C].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(MDOM_SUB_DISP_disp1c_second_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_1_reg, (MemTag[MEMIDX_SUB_C_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOM_SUB_DISP_disp1c_third_buf_addr_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_2_reg, (MemTag[MEMIDX_SUB_C_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOM_SUB_DISP_disp1c_forth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_3_reg, (MemTag[MEMIDX_SUB_C_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOM_SUB_DISP_disp1c_fifth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_4_reg, (MemTag[MEMIDX_SUB_C_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOM_SUB_DISP_disp1c_sixth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_5_reg, (MemTag[MEMIDX_SUB_C_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOM_SUB_DISP_disp1c_seventh_buf_addr_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_6_reg, (MemTag[MEMIDX_SUB_C_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOM_SUB_DISP_disp1c_eightth_buf_addr_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1c_downlimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1c_uplimit_7_reg, (MemTag[MEMIDX_SUB_C_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

}

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
void rotate_update_display_buffer_limit(void)
{
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

    IoReg_Write32(MDOMAIN_DISP_disp0_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_0_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_0_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(MDOMAIN_DISP_disp0_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_1_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_1_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(MDOMAIN_DISP_disp0_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_2_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_2_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);//need cut burstLen


    IoReg_Write32(MDOMAIN_DISP_disp0c_first_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_0_reg, (MemTag[MEMIDX_MAIN_C_SEC].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_0_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(MDOMAIN_DISP_disp0c_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_1_reg, (MemTag[MEMIDX_MAIN_C_THIRD].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_1_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    IoReg_Write32(MDOMAIN_DISP_disp0c_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0c_downlimit_2_reg, (MemTag[MEMIDX_MAIN_C_FOURTH].StartAddr) & 0xfffffff0);
    IoReg_Write32(MDOMAIN_DISP_Disp0c_uplimit_2_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

}
#endif

void memory_frc_set_capture_block_control(unsigned char display)
{
    if (display == SLR_MAIN_DISPLAY)
    {
        mdomain_cap_cap0_buf_control_RBUS mdomain_cap_cap0_buf_control_reg;
        mdomain_cap_cap0_buffer_change_line_num_0_RBUS mdomain_cap_cap0_buffer_change_line_num_0_reg;

        mdomain_cap_cap0_buf_control_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Cap0_buf_control_reg);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdomain_cap_cap0_buf_control_reg.cap0_buf_distance = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_force_en = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_force_addr = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_freeze_en = 0;
        mdomain_cap_cap0_buf_control_reg.cap0_buf_change_mode = 0;
        IoReg_Write32(MDOMAIN_CAP_Cap0_buf_control_reg, mdomain_cap_cap0_buf_control_reg.regValue);

        mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg);
        mdomain_cap_cap0_buffer_change_line_num_0_reg.cap0_buf_change_line_num_sw = 0;
        IoReg_Write32(MDOMAIN_CAP_cap0_buffer_change_line_num_0_reg, mdomain_cap_cap0_buffer_change_line_num_0_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Main] cap0_buf_num = %d\n", mdomain_cap_cap0_buf_control_reg.cap0_buf_num);
    } else
    {
        mdom_sub_cap_cap1_buf_control_RBUS mdom_sub_cap_cap1_buf_control_reg;
        mdom_sub_cap_cap1_buf_control_reg.regValue = IoReg_Read32(MDOM_SUB_CAP_Cap1_buf_control_reg);
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_distance = 0;
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_en = 0;
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_force_addr = 0;
        mdom_sub_cap_cap1_buf_control_reg.cap1_buf_freeze_en = 0;
        IoReg_Write32(MDOM_SUB_CAP_Cap1_buf_control_reg, mdom_sub_cap_cap1_buf_control_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Sub] cap1_buf_num = %d\n", mdom_sub_cap_cap1_buf_control_reg.cap1_buf_num);
    }
}

void memory_fs_main_decide_buffer_size(void)
{
    unsigned int buffer_size = 0;

    if(dvrif_memory_compression_get_enable(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL))==_ENABLE) {
        //RGB444 mode use 24bits @Crixus 20160902
        //sync k8?
        if (dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY)==COMPRESSION_16_BITS) {
            //buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_24BITS;
            buffer_size = drv_get_caveout_buffer_size_by_platform(MDOMAIN_16BITS);
        }
        else
        {
            buffer_size = drv_get_caveout_buffer_size_by_platform(MDOMAIN_12BITS);
        }
    } else {

            buffer_size = drv_get_caveout_buffer_size_by_platform(MDOMAIN_30BITS);
    }

    if(vdec_data_frc_need_borrow_cma_buffer(SLR_MAIN_DISPLAY)){
        buffer_size = MDOMAIN_MAIN_BUFFER_SIZE_12BITS_4K;
    }

    dvrif_memory_set_buffer_size(SLR_MAIN_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_info("[Mdomain][Fsync][Main] buffer size = %x, %s\n", buffer_size, __FUNCTION__);
}

void memory_fs_sub_decide_buffer_size(void)
{
    unsigned int buffer_size = 0;
    rtd_pr_scaler_memory_debug("\n Allocate Sub Disp Memory !\n");
    // set capture memory for sub display
    drvif_memory_free_block(MEMIDX_SUB);	// free block befor getting a new one
    drvif_memory_free_block(MEMIDX_SUB_SEC);	// free block befor getting a new one
    drvif_memory_free_block(MEMIDX_SUB_THIRD);	// free block befor getting a new one
    //sub use cap max size @Crixus 20151231
    buffer_size = MDOMAIN_SUB_BUFFER_SIZE;
    dvrif_memory_set_buffer_size(SLR_SUB_DISPLAY, buffer_size);
    rtd_pr_scaler_memory_info("[Mdomain] buffer size = %x, %s\n", buffer_size, __FUNCTION__);
}

void memory_fs_main_alloc_buffer(void)
{
#ifdef CONFIG_ARM64
	unsigned long align_addr = 0, query_addr = 0;
#else
	unsigned int align_addr = 0, query_addr = 0;
#endif
#ifndef CONFIG_ALL_SOURCE_DATA_FS
	unsigned int buffer_size_page_align = 0;
#endif
    unsigned long phy_addr_buffer_memory;
    unsigned char borrow_memory = TRUE;//need borrow memory from cma
    unsigned int borrow_size;//borrow size
    void *m_domain_borrow_buffer = NULL;
    SCALER_DISP_CHANNEL display = (SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);

    //merlin4 memory do 96 bytes align @Crixus 20180321
    query_addr = get_query_start_address(QUERY_IDX_MDOMAIN_MAIN);
    buffer_size = drvif_memory_get_data_align(buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    align_addr = dvr_memory_alignment((unsigned long)query_addr, buffer_size);
    #ifdef CONFIG_ARM64
    if (align_addr == (unsigned long)NULL)
            BUG();
    #else
    if (align_addr == (unsigned int)NULL)
            BUG();
    #endif

        //Just for reserved memory, update to use query driver @Crixus 20170817
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = align_addr;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_THIRD].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(MemTag[MEMIDX_MAIN_SEC].StartAddr+buffer_size, DMA_SPEEDUP_ALIGN_VALUE);
    if(vdec_data_frc_need_borrow_cma_buffer(display))
    {
        borrow_size =  buffer_size;
        rtd_pr_scaler_memory_notice("##func:%s main m domain borrow memory from CMA size:0x%x ###\r\n", __FUNCTION__, borrow_size);
        if(get_m_domain_borrow_buffer())
        {//alread has size from cma
            if(borrow_size <= get_m_domain_borrow_size())
            {//no need to borrow again
                borrow_memory = FALSE;
                m_domain_borrow_buffer = get_m_domain_borrow_buffer();//get original addr
                phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
            }
            else
                set_m_domain_borrow_buffer(NULL, 0);//free memory before borrow big size
        }
        if(borrow_memory)
        {//need borrrow memory from cam
            m_domain_borrow_buffer = dvr_malloc(borrow_size);
            if(m_domain_borrow_buffer)
            {
                phy_addr_buffer_memory = dvr_to_phys(m_domain_borrow_buffer);
                set_m_domain_borrow_buffer(m_domain_borrow_buffer, borrow_size);//save addr and size
                MemTag[MEMIDX_MAIN].StartAddr = drvif_memory_get_data_align(phy_addr_buffer_memory, DMA_SPEEDUP_ALIGN_VALUE);
            }
            else
            {
                rtd_pr_scaler_memory_err("###[Bug] func:%s no memory from cma ##\r\n",__FUNCTION__);
                return;
            }
        }
    }

    //16 bytes alignment
    MemTag[MEMIDX_MAIN_THIRD].StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr& 0xfffffff0;
    MemTag[MEMIDX_MAIN_SEC].StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr & 0xfffffff0;
    MemTag[MEMIDX_MAIN].StartAddr = MemTag[MEMIDX_MAIN].StartAddr & 0xfffffff0;

    //free virtual memory
    //drvif_memory_free_fixed_virtual_memory();
    //set virtual memory size
    //rtd_pr_scaler_memory_emerg("buffer_size=%x\n", buffer_size);
    buffer_size_page_align = PAGE_ALIGN(buffer_size);
    //buffer_size = buffer_size_page_align;
    rtd_pr_scaler_memory_emerg("memory_set_memory_size PAGE_ALIGN(buffer_size)=%x\n", PAGE_ALIGN(buffer_size));
    drvif_memory_set_fixed_virtual_memory_size(buffer_size_page_align);

    MemTag[MEMIDX_MAIN_THIRD].Allocate_StartAddr = MemTag[MEMIDX_MAIN_THIRD].StartAddr;
    MemTag[MEMIDX_MAIN_THIRD].Size = buffer_size;
    MemTag[MEMIDX_MAIN_THIRD].Status = ALLOCATED_FROM_PLI;

    MemTag[MEMIDX_MAIN_SEC].Allocate_StartAddr = MemTag[MEMIDX_MAIN_SEC].StartAddr;
    MemTag[MEMIDX_MAIN_SEC].Size = buffer_size;
    MemTag[MEMIDX_MAIN_SEC].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN_SEC].Allocate_VirAddr = 0;//not use

    MemTag[MEMIDX_MAIN].Allocate_StartAddr = MemTag[MEMIDX_MAIN].StartAddr;
    MemTag[MEMIDX_MAIN].Size = buffer_size;
    MemTag[MEMIDX_MAIN].Status = ALLOCATED_FROM_PLI;
    MemTag[MEMIDX_MAIN].Allocate_VirAddr = 0;//not use
}

void memory_fs_sub_alloc_buffer(void)
{
#ifdef CONFIG_DUAL_CHANNEL
#ifdef ENABLE_3_BUFFER_DELAY_MODE
	void *sub_3rd_buffer;
#endif
	void *sub_2nd_buffer;
	void *sub_1st_buffer;
#endif
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);

#ifdef ENABLE_3_BUFFER_DELAY_MODE
    //I2rnd mode s-cap use 2-buffer
    sub_3rd_buffer = get_sub_m_domain_3rd_buf_address();
    if (sub_3rd_buffer != NULL)
        dvr_free(sub_3rd_buffer);

    sub_3rd_buffer = dvr_malloc(buffer_size);
    set_sub_m_domain_3rd_buf_address(sub_3rd_buffer);
    MemTag[MEMIDX_SUB_THIRD].StartAddr = dvr_to_phys(sub_3rd_buffer);
    rtd_pr_scaler_memory_info("set M-domain sub 3rd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_3rd_buffer, MemTag[MEMIDX_SUB_THIRD].StartAddr, buffer_size);
#endif

    sub_2nd_buffer = get_sub_m_domain_2nd_buf_address();
    if (sub_2nd_buffer != NULL)
        dvr_free(sub_2nd_buffer);

    sub_2nd_buffer = dvr_malloc(buffer_size);
    set_sub_m_domain_2nd_buf_address(sub_2nd_buffer);
    MemTag[MEMIDX_SUB_SEC].StartAddr = dvr_to_phys(sub_2nd_buffer);
    rtd_pr_scaler_memory_info("set M-domain sub 2nd buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_2nd_buffer, MemTag[MEMIDX_SUB_SEC].StartAddr, buffer_size);


    sub_1st_buffer = get_sub_m_domain_1st_buf_address();
    if (sub_1st_buffer != NULL)
        dvr_free(sub_1st_buffer);

    sub_1st_buffer = dvr_malloc(buffer_size);
    set_sub_m_domain_1st_buf_address(sub_1st_buffer);
    MemTag[MEMIDX_SUB].StartAddr = dvr_to_phys(sub_1st_buffer);
    rtd_pr_scaler_memory_info("set M-domain sub 1st buffer 0x%px paddr 0x%08X, size=0x%x\n", sub_1st_buffer, MemTag[MEMIDX_SUB].StartAddr, buffer_size);
}

void memory_set_display_byte_swap(unsigned char display)
{
    mdomain_disp_frc_disp0_rd_ctrl_RBUS mdomain_disp_frc_disp0_rd_ctrl_reg;
    mdom_sub_disp_frc_disp1_rd_ctrl_RBUS mdom_sub_disp_frc_disp1_rd_ctrl_reg;

    mdomain_disp_frc_disp0_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg);
	mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg);//update disp byte swap reg @Crixus 20170515
	if(display){
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_1byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_2byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_4byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_8byte_swap = 1;
        } else
#endif
        if(Scaler_DispGetStatus(display, SLR_DISP_422CAP))
		{
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_1byte_swap = 0;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_2byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_4byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_8byte_swap = 0;
		}
		else
		{
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_1byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_2byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_4byte_swap = 1;
            mdom_sub_disp_frc_disp1_rd_ctrl_reg.disp1_dma_8byte_swap = 0;
		}
	}else{
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(display)) {
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 1;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 1;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 1;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 1;
        } else
#endif
        if(Scaler_DispGetStatus(display, SLR_DISP_10BIT))
        {
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 0;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 0;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 0;
            mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 0;
        }
        else
        {
            if(Scaler_DispGetStatus(display, SLR_DISP_422CAP))
            {
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 0;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 0;
            }
            else
            {
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_1byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_2byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_4byte_swap = 1;
                mdomain_disp_frc_disp0_rd_ctrl_reg.disp0_dma_8byte_swap = 0;
            }
        }
	}

    IoReg_Write32(MDOMAIN_DISP_FRC_DISP0_RD_Ctrl_reg, mdomain_disp_frc_disp0_rd_ctrl_reg.regValue);
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1_RD_Ctrl_reg, mdom_sub_disp_frc_disp1_rd_ctrl_reg.regValue);
}

void memory_set_display_us_path(unsigned char display)
{
    unsigned int DispWidth = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID);
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN);

    if (display == SLR_MAIN_DISPLAY){
        mdomain_disp_disp0_us_ctrl_RBUS mdomain_disp_disp0_us_ctrl_reg;
        mdomain_disp_disp0_unpack_ctrl_RBUS mdomain_disp_disp0_unpack_ctrl_reg;
        mdomain_disp_frc_disp0_size_ctrl0_RBUS mdomain_disp_frc_disp0_size_ctrl0_reg;
            // us ctrl
        mdomain_disp_disp0_us_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_disp0_US_Ctrl_reg);
        mdomain_disp_disp0_us_ctrl_reg.disp0_hor_us_mode = 0;
        mdomain_disp_disp0_us_ctrl_reg.disp0_ver_us_mode = 0;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdomain_disp_disp0_us_ctrl_reg.disp0_ver_en = 3;
            mdomain_disp_disp0_us_ctrl_reg.disp0_hor_en = 0;
        } else {
            mdomain_disp_disp0_us_ctrl_reg.disp0_ver_en = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdomain_disp_disp0_us_ctrl_reg.disp0_hor_en = 0;
            }
            else{
                mdomain_disp_disp0_us_ctrl_reg.disp0_hor_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
        }
        IoReg_Write32(MDOMAIN_DISP_disp0_US_Ctrl_reg, mdomain_disp_disp0_us_ctrl_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Main] US0 disp0_ver_en = %d, disp0_hor_en = %d\n", mdomain_disp_disp0_us_ctrl_reg.disp0_ver_en, mdomain_disp_disp0_us_ctrl_reg.disp0_hor_en);
        
        // unpack_ctrl
        mdomain_disp_disp0_unpack_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_disp0_unpack_ctrl_reg);
        mdomain_disp_disp0_unpack_ctrl_reg.disp0_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_data_format = MEM_8_BIT;
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_wrap_mode = 10;
        } else {
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(MDOMAIN_DISP_disp0_unpack_ctrl_reg, mdomain_disp_disp0_unpack_ctrl_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Main] US0 disp0_wrap_mode = %d, disp0_dummy_format = %d, disp0_data_format = %d\n",
            mdomain_disp_disp0_unpack_ctrl_reg.disp0_wrap_mode, mdomain_disp_disp0_unpack_ctrl_reg.disp0_dummy_format, mdomain_disp_disp0_unpack_ctrl_reg.disp0_data_format);

        // size ctrl
        mdomain_disp_frc_disp0_size_ctrl0_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_disp0_size_ctrl0_reg);
        mdomain_disp_frc_disp0_size_ctrl0_reg.disp0_vact = drvif_memory_get_data_align(DispLength, 2);
        mdomain_disp_frc_disp0_size_ctrl0_reg.disp0_hact = drvif_memory_get_data_align(DispWidth, 2);
        IoReg_Write32(MDOMAIN_DISP_FRC_disp0_size_ctrl0_reg, mdomain_disp_frc_disp0_size_ctrl0_reg.regValue);

        if (dvrif_memory_get_block_mode_enable(display) == TRUE)
        {
            mdomain_disp_frc_disp0c_size_ctrl0_RBUS mdomain_disp_frc_disp0c_size_ctrl0_reg;
            mdomain_disp_frc_disp0c_size_ctrl0_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_disp0c_size_ctrl0_reg);
            mdomain_disp_frc_disp0c_size_ctrl0_reg.disp0c_vact = drvif_memory_get_data_align(DispLength, 2);
            mdomain_disp_frc_disp0c_size_ctrl0_reg.disp0c_hact = drvif_memory_get_data_align(DispWidth, 2);
            IoReg_Write32(MDOMAIN_DISP_FRC_disp0c_size_ctrl0_reg, mdomain_disp_frc_disp0c_size_ctrl0_reg.regValue);
        }
    } else {
        mdom_sub_disp_disp1_us_ctrl_RBUS mdom_sub_disp_disp1_us_ctrl_reg;
        mdom_sub_disp_disp1_unpack_ctrl_RBUS mdom_sub_disp_disp1_unpack_ctrl_reg;
        mdom_sub_disp_frc_disp1_size_ctrl0_RBUS mdom_sub_disp_frc_disp1_size_ctrl0_reg;

        // us ctrl
        mdom_sub_disp_disp1_us_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_disp1_US_Ctrl_reg);
        mdom_sub_disp_disp1_us_ctrl_reg.disp1_hor_us_mode = 0;
        mdom_sub_disp_disp1_us_ctrl_reg.disp1_ver_us_mode = 0;
        if (dvrif_memory_get_block_mode_enable(display) == TRUE){
            mdom_sub_disp_disp1_us_ctrl_reg.disp1_ver_en = 3;
            mdom_sub_disp_disp1_us_ctrl_reg.disp1_hor_en = 0;
        } else {
            mdom_sub_disp_disp1_us_ctrl_reg.disp1_ver_en = 0;
            if (dvrif_memory_compression_get_enable(display)){
                mdom_sub_disp_disp1_us_ctrl_reg.disp1_hor_en = 0;
            } else {
                mdom_sub_disp_disp1_us_ctrl_reg.disp1_hor_en = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
            }
        }
        IoReg_Write32(MDOM_SUB_DISP_disp1_US_Ctrl_reg, mdom_sub_disp_disp1_us_ctrl_reg.regValue);

        rtd_pr_scaler_memory_info("[Mdomain][Sub] US1 disp1_ver_en = %d, disp1_hor_en = %d\n", mdom_sub_disp_disp1_us_ctrl_reg.disp1_ver_en, mdom_sub_disp_disp1_us_ctrl_reg.disp1_hor_en);

        // unpack_ctrl
        mdom_sub_disp_disp1_unpack_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_disp1_unpack_ctrl_reg);
        mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_dummy_format = (dvrif_memory_get_cap_mode(display) == MEMCAPTYPE_FRAME);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE) {
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_data_format = MEM_8_BIT;
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_wrap_mode = 10;
        } else {
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_data_format = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_wrap_mode = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        IoReg_Write32(MDOM_SUB_DISP_disp1_unpack_ctrl_reg, mdom_sub_disp_disp1_unpack_ctrl_reg.regValue);
        
        rtd_pr_scaler_memory_info("[Mdomain][Sub] US1 disp1_wrap_mode = %d, disp1_dummy_format = %d, disp1_data_format = %d\n",
            mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_wrap_mode, mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_dummy_format, mdom_sub_disp_disp1_unpack_ctrl_reg.disp1_data_format);

        // size ctrl
        mdom_sub_disp_frc_disp1_size_ctrl0_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_disp1_size_ctrl0_reg);
        mdom_sub_disp_frc_disp1_size_ctrl0_reg.disp1_vact = drvif_memory_get_data_align(DispLength, 2);
        mdom_sub_disp_frc_disp1_size_ctrl0_reg.disp1_hact = drvif_memory_get_data_align(DispWidth, 2);
        IoReg_Write32(MDOM_SUB_DISP_FRC_disp1_size_ctrl0_reg, mdom_sub_disp_frc_disp1_size_ctrl0_reg.regValue);
        if (dvrif_memory_get_block_mode_enable(display) == TRUE) {
            mdom_sub_disp_frc_disp1c_size_ctrl0_RBUS mdom_sub_disp_frc_disp1c_size_ctrl0_reg;
            mdom_sub_disp_frc_disp1c_size_ctrl0_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_disp1c_size_ctrl0_reg);
            mdom_sub_disp_frc_disp1c_size_ctrl0_reg.disp1c_vact = drvif_memory_get_data_align(DispLength, 2);
            mdom_sub_disp_frc_disp1c_size_ctrl0_reg.disp1c_hact = drvif_memory_get_data_align(DispWidth, 2);
            IoReg_Write32(MDOM_SUB_DISP_FRC_disp1c_size_ctrl0_reg, mdom_sub_disp_frc_disp1c_size_ctrl0_reg.regValue);
        }
    }
}

void memory_main_set_rdma(void)
{
    mdomain_disp_frc_disp0_num_bl_wrap_line_step_RBUS mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg;
    mdomain_disp_frc_disp0_num_bl_wrap_ctl_RBUS mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg;
    mdomain_disp_frc_disp0_num_bl_wrap_word_RBUS mdomain_disp_frc_disp0_num_bl_wrap_word_reg;
    mdomain_disp_disp0_dma_crop_RBUS mdomain_disp_disp0_dma_crop_reg;
    mdomain_disp_disp0_out_crop_RBUS mdomain_disp_disp0_out_crop_reg;
    mdomain_disp_disp0_dma_rmask_RBUS mdomain_disp_disp0_dma_rmask_reg;
    
    unsigned int TotalSize;
	unsigned int LineStep;
    unsigned int PixelSize;
	unsigned int DispWidth = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_WID);
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN);
    unsigned int x_start =  Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_HSTA);

    if(dvrif_memory_compression_get_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        PixelSize = dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY);
        if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = memory_get_capture_line_size_compression(SLR_MAIN_DISPLAY, PixelSize);//compression test
        }
        else{
            TotalSize = memory_get_capture_frame_size_compression(SLR_MAIN_DISPLAY, PixelSize);
        }
    } else{
        PixelSizeInfo pixelSizeInfo;

        pixelSizeInfo.YCMode = dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY);
        pixelSizeInfo.BitNum = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_422CAP);

        PixelSize = memory_get_pixel_size(&pixelSizeInfo);
        TotalSize = memory_get_display_size(SLR_MAIN_DISPLAY);
    }

    // line_step
	if (dvrif_memory_get_cap_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE)
	{
        if(get_mdomain_1p_for_4k60_vrr_flag()){
            LineStep = TotalSize * 2;
        } else {
            LineStep = TotalSize;
        }

        if (get_mdomain_vflip_flag(SLR_MAIN_DISPLAY) == TRUE){
            LineStep = ~LineStep + 1;
        }

	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_info("[Mdomain][Main] RDMA0 TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_line_step_reg);
    mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg.disp0_line_step = LineStep;
    IoReg_Write32(MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_line_step_reg, mdomain_disp_frc_disp0_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_ctl_reg);
    mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.disp0_line_num = DispLength;
    mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.disp0_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_ctl_reg, mdomain_disp_frc_disp0_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdomain_disp_frc_disp0_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_word_reg);
    mdomain_disp_frc_disp0_num_bl_wrap_word_reg.disp0_addr_toggle_mode = 0;
    mdomain_disp_frc_disp0_num_bl_wrap_word_reg.disp0_line_burst_num = TotalSize;
    IoReg_Write32(MDOMAIN_DISP_FRC_DISP0_num_bl_wrap_word_reg, mdomain_disp_frc_disp0_num_bl_wrap_word_reg.regValue);

    
    mdomain_disp_disp0_dma_crop_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_DMA_CROP_reg);
    if (x_start > 0 && ((DispWidth * PixelSize) % 128 == 0)){
        mdomain_disp_disp0_dma_crop_reg.disp0_crop_end_align_128 = 1;
    } else {
        mdomain_disp_disp0_dma_crop_reg.disp0_crop_end_align_128 = 0;
    }
    IoReg_Write32(MDOMAIN_DISP_DISP0_DMA_CROP_reg, mdomain_disp_disp0_dma_crop_reg.regValue);

    mdomain_disp_disp0_out_crop_reg.regValue = IoReg_Read32(MDOMAIN_DISP_DISP0_OUT_CROP_reg);
    mdomain_disp_disp0_out_crop_reg.disp0_crop_sta_pxl = (x_start * PixelSize) % 128;
    IoReg_Write32(MDOMAIN_DISP_DISP0_OUT_CROP_reg, mdomain_disp_disp0_out_crop_reg.regValue);
  
    rtd_pr_scaler_memory_info("[Mdomain][Main] disp0_crop_sta_pxl = %d, disp0_crop_end_align_128 = %d\n", mdomain_disp_disp0_out_crop_reg.disp0_crop_sta_pxl, mdomain_disp_disp0_dma_crop_reg.disp0_crop_end_align_128);
    
    if((DispWidth % 32) != 0){
		DispWidth = DispWidth + (32 - (DispWidth % 32));
	}

    //single buffer aovid tear
    mdomain_disp_disp0_dma_rmask_reg.regValue = IoReg_Read32(MDOMAIN_DISP_disp0_dma_rmask_reg);
    mdomain_disp_disp0_dma_rmask_reg.disp0_read_first_disable = 1;
    mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_distance = (DispWidth*PixelSize/128)*2/_RDMA_BURSTLENGTH;
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
        mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_force_en = 1;
        mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_force_value = 0;
    } else
#endif
    {
        mdomain_disp_disp0_dma_rmask_reg.disp0_read_mask_force_en = 1;
    }
    IoReg_Write32(MDOMAIN_DISP_disp0_dma_rmask_reg, mdomain_disp_disp0_dma_rmask_reg.regValue);

    // byte_swap
    memory_set_display_byte_swap(SLR_MAIN_DISPLAY);

    // outstanding
    mdisp_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);
}

void memory_sub_set_rdma(void)
{
    mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_RBUS mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg;
    mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_RBUS mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg;
    mdom_sub_disp_frc_disp1_num_bl_wrap_word_RBUS mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg;
    mdom_sub_disp_disp1_crop_RBUS mdom_sub_disp_disp1_crop_reg;
    mdom_sub_disp_disp1_out_crop_RBUS mdom_sub_disp_disp1_out_crop_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
    unsigned int PixelSize;
	unsigned int DispWidth = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_WID);
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_LEN);
    unsigned int x_start =  Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_HSTA);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        PixelSize = dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY);
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, PixelSize);//compression test
        }
        else{
            TotalSize = memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, PixelSize);
        }
    } else{
        PixelSizeInfo pixelSizeInfo;

        pixelSizeInfo.YCMode = dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY);
        pixelSizeInfo.BitNum = Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT);
        pixelSizeInfo.Format = Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_422CAP);

        PixelSize = memory_get_pixel_size(&pixelSizeInfo);
        TotalSize = memory_get_display_size(SLR_SUB_DISPLAY);
    }
    // line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
        if(get_mdomain_1p_for_4k60_vrr_flag()){
            LineStep = TotalSize * 2;
        } else {
            LineStep = TotalSize;
        }
		if (get_mdomain_vflip_flag(SLR_SUB_DISPLAY) == TRUE){
            LineStep = ~LineStep + 1;
        }
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_info("[Mdomain][Sub] RDMA1 TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_line_step_reg);
    mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg.disp1_line_step = LineStep;
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_line_step_reg, mdom_sub_disp_frc_disp1_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_ctl_reg);
    mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.disp1_line_num = DispLength;
    mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.disp1_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_ctl_reg, mdom_sub_disp_frc_disp1_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_word_reg);
    mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.disp1_addr_toggle_mode = 0;
    mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.disp1_line_burst_num = TotalSize;
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1_num_bl_wrap_word_reg, mdom_sub_disp_frc_disp1_num_bl_wrap_word_reg.regValue);

    // crop
    mdom_sub_disp_disp1_crop_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_CROP_reg);
    if (x_start > 0 && ((DispWidth * PixelSize) % 128 == 0)){
        mdom_sub_disp_disp1_crop_reg.disp1_crop_end_align_128 = 1;
    } else {
        mdom_sub_disp_disp1_crop_reg.disp1_crop_end_align_128 = 0;
    }
    IoReg_Write32(MDOM_SUB_DISP_DISP1_CROP_reg, mdom_sub_disp_disp1_crop_reg.regValue);

    mdom_sub_disp_disp1_out_crop_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_DISP1_OUT_CROP_reg);
    mdom_sub_disp_disp1_out_crop_reg.disp1_crop_sta_pxl = (x_start * PixelSize) % 128;
    IoReg_Write32(MDOM_SUB_DISP_DISP1_OUT_CROP_reg, mdom_sub_disp_disp1_out_crop_reg.regValue);

    rtd_pr_scaler_memory_info("[Mdomain][Sub] disp1_crop_sta_pxl = %d, disp1_crop_end_align_128 = %d\n", mdom_sub_disp_disp1_out_crop_reg.disp1_crop_sta_pxl, mdom_sub_disp_disp1_out_crop_reg.disp1_crop_sta_pxl);

    // byte_swap
    memory_set_display_byte_swap(SLR_SUB_DISPLAY);

    // outstanding
    mdisp_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);
}

void memory_main_set_rdma_c(void)
{
    mdomain_disp_frc_disp0c_num_bl_wrap_line_step_RBUS mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg;
    mdomain_disp_frc_disp0c_num_bl_wrap_ctl_RBUS mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg;
    mdomain_disp_frc_disp0c_num_bl_wrap_word_RBUS mdomain_disp_frc_disp0c_num_bl_wrap_word_reg;
    mdomain_disp_frc_disp0c_rd_ctrl_RBUS mdomain_disp_frc_disp0c_rd_ctrl_reg;
	
    unsigned int TotalSize;
	unsigned int LineStep;
    unsigned int PixelSize;
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_MEM_ACT_LEN) / 2;

    PixelSizeInfo pixelSizeInfo;
    pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
    pixelSizeInfo.BitNum = MEM_8_BIT;
    pixelSizeInfo.Format = MEM_420;

    PixelSize = memory_get_pixel_size(&pixelSizeInfo);
    TotalSize = memory_get_display_size(SLR_MAIN_DISPLAY);

    // line_step
    if(get_mdomain_1p_for_4k60_vrr_flag()){
        LineStep = TotalSize * 2;
    } else {
        LineStep = TotalSize;
    }

    if (get_mdomain_vflip_flag(SLR_MAIN_DISPLAY) == TRUE){
        LineStep = ~LineStep + 1;
    }
    rtd_pr_scaler_memory_info("[Mdomain][Main] RDMA0C TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_line_step_reg);
    mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg.disp0c_line_step = LineStep;
    IoReg_Write32(MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_line_step_reg, mdomain_disp_frc_disp0c_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_ctl_reg);
    mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.disp0c_line_num = DispLength;
    mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.disp0c_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_ctl_reg, mdomain_disp_frc_disp0c_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_word_reg);
    mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.disp0c_addr_toggle_mode = 1;
    mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.disp0c_line_burst_num = TotalSize;
    IoReg_Write32(MDOMAIN_DISP_FRC_DISP0C_num_bl_wrap_word_reg, mdomain_disp_frc_disp0c_num_bl_wrap_word_reg.regValue);

    // byte_swap
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
        mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_8byte_swap = 1;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_4byte_swap = 1;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_2byte_swap = 1;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_1byte_swap = 1;
        IoReg_Write32(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue);
    } else 
#endif
    {
        mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg);
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_8byte_swap = 0;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_4byte_swap = 0;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_2byte_swap = 0;
        mdomain_disp_frc_disp0c_rd_ctrl_reg.disp0c_dma_1byte_swap = 0;
        IoReg_Write32(MDOMAIN_DISP_FRC_DISP0C_RD_Ctrl_reg, mdomain_disp_frc_disp0c_rd_ctrl_reg.regValue);
    }

    // outstanding
    mdisp_dmac_outstanding_ctrl(SLR_MAIN_DISPLAY, _ENABLE);
}

void memory_sub_set_rdma_c(void)
{
    mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_RBUS mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg;
    mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_RBUS mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg;
    mdom_sub_disp_frc_disp1c_num_bl_wrap_word_RBUS mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg;
    mdom_sub_disp_frc_disp1c_rd_ctrl_RBUS mdom_sub_disp_frc_disp1c_rd_ctrl_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int DispLength = Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_LEN);

    if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
        {
            TotalSize = memory_get_capture_line_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));//compression test
        }
        else{
            TotalSize = memory_get_capture_frame_size_compression(SLR_SUB_DISPLAY, dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));
        }
    } else{
        TotalSize = memory_get_display_size(SLR_SUB_DISPLAY);
    }
    // line_step
	if (dvrif_memory_get_cap_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE)
	{
        if(get_mdomain_1p_for_4k60_vrr_flag()){
            LineStep = TotalSize * 2;
        } else {
            LineStep = TotalSize;
        }
		
	} else {
		LineStep = 0;
	}

    rtd_pr_scaler_memory_info("[Mdomain][Sub] RDMA1C TotalSize = %d, LineStep = %d\n", TotalSize, LineStep);

    mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_line_step_reg);
    mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg.disp1c_line_step = LineStep;
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_line_step_reg, mdom_sub_disp_frc_disp1c_num_bl_wrap_line_step_reg.regValue);

    // line_num & burst_len
    mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_ctl_reg);
    mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.disp1c_line_num = DispLength / 2;
    mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.disp1c_burst_len = _RDMA_BURSTLENGTH;
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_ctl_reg, mdom_sub_disp_frc_disp1c_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_word_reg);
    mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.disp1c_addr_toggle_mode = 1;
    mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.disp1c_line_burst_num = TotalSize;
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1C_num_bl_wrap_word_reg, mdom_sub_disp_frc_disp1c_num_bl_wrap_word_reg.regValue);

    // byte_swap
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg);
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_8byte_swap = 0;
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_4byte_swap = 0;
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_2byte_swap = 0;
    mdom_sub_disp_frc_disp1c_rd_ctrl_reg.disp1c_dma_1byte_swap = 0;
    IoReg_Write32(MDOM_SUB_DISP_FRC_DISP1C_RD_Ctrl_reg, mdom_sub_disp_frc_disp1c_rd_ctrl_reg.regValue);
    
    // outstanding
    mdisp_dmac_outstanding_ctrl(SLR_SUB_DISPLAY, _ENABLE);
}

unsigned int memory_frc_calculate_display_shift_address(unsigned char display)
{
    unsigned int x_start =  Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_HSTA);
    unsigned int y_start =  Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_VSTA);
    unsigned int disp_wid = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_WID);
    unsigned int cap_len = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_MEM_ACT_LEN);
    PixelSizeInfo pixelSizeInfo;
    unsigned int PixelSize;
    unsigned int TotalSize;
    unsigned int shift_addr = 0;

    if(dvrif_memory_compression_get_enable(display) == TRUE)
    {
        PixelSize = dvrif_memory_get_compression_bits(display);
        TotalSize = memory_get_capture_line_size_compression(display, PixelSize);

    } else {
        if (dvrif_memory_get_block_mode_enable(display)){
            pixelSizeInfo.YCMode = MEM_YC_SEPARATE;
            pixelSizeInfo.BitNum = MEM_8_BIT;
            pixelSizeInfo.Format = MEM_420;
        } else {
            pixelSizeInfo.YCMode = MEM_YC_COMBINE;
            pixelSizeInfo.BitNum = Scaler_DispGetStatus(display, SLR_DISP_10BIT);
            pixelSizeInfo.Format = Scaler_DispGetStatus(display, SLR_DISP_422CAP);
        }
        TotalSize = memory_get_line_size(disp_wid, &pixelSizeInfo);
        PixelSize = memory_get_pixel_size(&pixelSizeInfo);
    }

    if (get_mdomain_vflip_flag(display) == FALSE){
        shift_addr = TotalSize * y_start + (PixelSize * x_start) / 128;
    } else {
        shift_addr = TotalSize * (cap_len - y_start  - 1) + (PixelSize * x_start) / 128;
    }
    return shift_addr << 4;
}

void memory_main_update_display_buffer(void)
{
    mdomain_disp_disp0_boundaryaddr_ctrl0_RBUS mdomain_disp_disp0_boundaryaddr_ctrl0_reg;
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY);
    unsigned int MemShiftAddr;

    if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == MEMCAPTYPE_LINE){
        MemShiftAddr = memory_frc_calculate_display_shift_address(SLR_MAIN_DISPLAY);
    } else {
        MemShiftAddr = 0;
    }

    rtd_pr_scaler_memory_emerg("[Mdomain][Main] update mdisp buffer number = %d\n", cap_num);

    if(cap_num > 0){

        IoReg_Write32(MDOMAIN_DISP_disp0_first_buf_addr_reg, (MemTag[MEMIDX_MAIN].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_0_reg, (MemTag[MEMIDX_MAIN].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen
    }

    if(cap_num > 1){

        IoReg_Write32(MDOMAIN_DISP_disp0_second_buf_addr_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_1_reg, (MemTag[MEMIDX_MAIN_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOMAIN_DISP_disp0_third_buf_addr_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_2_reg, (MemTag[MEMIDX_MAIN_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOMAIN_DISP_disp0_forth_buf_addr_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_3_reg, (MemTag[MEMIDX_MAIN_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOMAIN_DISP_disp0_fifth_buf_addr_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_4_reg, (MemTag[MEMIDX_MAIN_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOMAIN_DISP_disp0_sixth_buf_addr_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_5_reg, (MemTag[MEMIDX_MAIN_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOMAIN_DISP_disp0_seventh_buf_addr_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_6_reg, (MemTag[MEMIDX_MAIN_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOMAIN_DISP_disp0_eightth_buf_addr_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOMAIN_DISP_Disp0_downlimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOMAIN_DISP_Disp0_uplimit_7_reg, (MemTag[MEMIDX_MAIN_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.regValue = IoReg_Read32(MDOMAIN_DISP_Disp0_BoundaryAddr_CTRL0_reg);
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk7_sel = 7;
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk6_sel = 6;
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk5_sel = 5;
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk4_sel = 4;
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk3_sel = 3;
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk2_sel = 2;
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk1_sel = 1;
    mdomain_disp_disp0_boundaryaddr_ctrl0_reg.disp0_blk0_sel = 0;
    IoReg_Write32(MDOMAIN_DISP_Disp0_BoundaryAddr_CTRL0_reg, mdomain_disp_disp0_boundaryaddr_ctrl0_reg.regValue);
}

void memory_sub_update_display_buffer(void)
{
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_RBUS mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg;
    unsigned char cap_num = dvrif_memory_get_buffer_number(SLR_SUB_DISPLAY);
    unsigned int buffer_size = dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY);
    unsigned int MemShiftAddr;

    if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == MEMCAPTYPE_LINE){
        MemShiftAddr = memory_frc_calculate_display_shift_address(SLR_SUB_DISPLAY);
    } else {
        MemShiftAddr = 0;
    }
 
    rtd_pr_scaler_memory_emerg("[Mdomain][Sub] update mdisp buffer number = %d\n", cap_num);
    
    if(cap_num > 0){

        IoReg_Write32(MDOM_SUB_DISP_disp1_first_buf_addr_reg, (MemTag[MEMIDX_SUB].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_0_reg, (MemTag[MEMIDX_SUB].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(MDOM_SUB_DISP_disp1_second_buf_addr_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_1_reg, (MemTag[MEMIDX_SUB_SEC].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOM_SUB_DISP_disp1_third_buf_addr_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_2_reg, (MemTag[MEMIDX_SUB_THIRD].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOM_SUB_DISP_disp1_forth_buf_addr_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_3_reg, (MemTag[MEMIDX_SUB_FOURTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOM_SUB_DISP_disp1_fifth_buf_addr_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_4_reg, (MemTag[MEMIDX_SUB_FIFTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOM_SUB_DISP_disp1_sixth_buf_addr_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_5_reg, (MemTag[MEMIDX_SUB_SIXTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOM_SUB_DISP_disp1_seventh_buf_addr_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_6_reg, (MemTag[MEMIDX_SUB_SEVENTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOM_SUB_DISP_disp1_eightth_buf_addr_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr + MemShiftAddr) & 0xfffffff0);

        IoReg_Write32(MDOM_SUB_DISP_Disp1_downlimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr) & 0xfffffff0);
        IoReg_Write32(MDOM_SUB_DISP_Disp1_uplimit_7_reg, (MemTag[MEMIDX_SUB_EIGHTH].StartAddr + buffer_size - FINAL_BURST_LENTH) & 0xfffffff0);//need cut burstLen

    }

    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_Disp1_BoundaryAddr_CTRL0_reg);
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk7_sel = 7;
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk6_sel = 6;
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk5_sel = 5;
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk4_sel = 4;
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk3_sel = 3;
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk2_sel = 2;
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk1_sel = 1;
    mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.disp1_blk0_sel = 0;
    IoReg_Write32(MDOM_SUB_DISP_Disp1_BoundaryAddr_CTRL0_reg, mdom_sub_disp_disp1_boundaryaddr_ctrl0_reg.regValue);
}

void memory_frc_set_display_block_control(unsigned char display)
{
    if (display == SLR_MAIN_DISPLAY)
    {
        mdomain_disp_frc_disp0_buffer_select_control_RBUS mdomain_disp_frc_disp0_buffer_select_control_reg;
        mdomain_disp_frc_disp0_buffer_select_control_reg.regValue = IoReg_Read32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_distance = 0;
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_en = 0;
        mdomain_disp_frc_disp0_buffer_select_control_reg.disp0_buf_force_addr = 0;
        IoReg_Write32(MDOMAIN_DISP_FRC_disp0_buffer_select_control_reg, mdomain_disp_frc_disp0_buffer_select_control_reg.regValue);

    } else
    {
        mdom_sub_disp_frc_disp1_buffer_select_control_RBUS mdom_sub_disp_frc_disp1_buffer_select_control_reg;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue = IoReg_Read32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg);
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_num = (dvrif_memory_get_buffer_number(display) == 0) ? 0 : (dvrif_memory_get_buffer_number(display) - 1);
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_distance = 0;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_en = 0;
        mdom_sub_disp_frc_disp1_buffer_select_control_reg.disp1_buf_force_addr = 0;
        IoReg_Write32(MDOM_SUB_DISP_FRC_disp1_buffer_select_control_reg, mdom_sub_disp_frc_disp1_buffer_select_control_reg.regValue);
    }
}

void dvrif_memory_set_gatting_settings(unsigned char display, unsigned int gating_mask_line){
    if (display == SLR_MAIN_DISPLAY)
    {
        mdomain_cap_frc_cap0_timing_ctrl_RBUS mdomain_cap_frc_cap0_timing_ctrl_reg;
        mdomain_disp_disp0_fsbuf_ctrl_1_RBUS mdomain_disp_disp0_fsbuf_ctrl_1_reg;

        mdomain_cap_frc_cap0_timing_ctrl_reg.regValue = IoReg_Read32(MDOMAIN_CAP_frc_cap0_timing_ctrl_reg);
        mdomain_cap_frc_cap0_timing_ctrl_reg.cap0_htotal = Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_H_LEN);
        IoReg_Write32(MDOMAIN_CAP_frc_cap0_timing_ctrl_reg, mdomain_cap_frc_cap0_timing_ctrl_reg.regValue);

        mdomain_disp_disp0_fsbuf_ctrl_1_reg.regValue = IoReg_Read32(MDOMAIN_DISP_disp0_FSBUF_CTRL_1_reg);
        mdomain_disp_disp0_fsbuf_ctrl_1_reg.disp0_fs_gating_mask_line = gating_mask_line;
        IoReg_Write32(MDOMAIN_DISP_disp0_FSBUF_CTRL_1_reg, mdomain_disp_disp0_fsbuf_ctrl_1_reg.regValue);

        rtd_pr_scaler_memory_emerg("cap0_htotal = %d, fs_gating_mask_line = %d\n", Scaler_DispGetInputInfoByDisp(display, SLR_INPUT_H_LEN), gating_mask_line);
    }
}

void memory_frc_main_set_capture(void)
{
    mdomain_cap_cap0_reg_doublbuffer_RBUS mdomain_cap_cap0_reg_doublbuffer_reg;

	// disable cap db
	if(scaler_vsc_get_vr360_block_mdomain_doublebuffer() == FALSE){
		mdomain_cap_cap0_reg_doublbuffer_reg.regValue = rtd_inl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg);
		mdomain_cap_cap0_reg_doublbuffer_reg.cap0_db_en = 0;
		rtd_outl(MDOMAIN_CAP_cap0_reg_doublbuffer_reg,mdomain_cap_cap0_reg_doublbuffer_reg.regValue);
	}

    memory_frc_main_decide_buffer_size();
    memory_frc_main_alloc_buffer();

    memory_set_capture_ds_path(SLR_MAIN_DISPLAY);
	memory_main_set_wdma();
    memory_main_update_capture_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        memory_main_set_wdma_c();
        memory_main_update_capture_C_buffer(dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY),  dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY));
    }
    memory_frc_set_capture_block_control(SLR_MAIN_DISPLAY);
}

void memory_frc_sub_set_capture(void)
{
    memory_frc_sub_decide_buffer_size();
    // set capture memory for sub display
    drvif_memory_free_block(MEMIDX_SUB);	// free block befor getting a new one
    drvif_memory_free_block(MEMIDX_SUB_SEC);	// free block befor getting a new one
    drvif_memory_free_block(MEMIDX_SUB_THIRD);	// free block befor getting a new one
    memory_frc_sub_alloc_buffer();

    memory_set_capture_ds_path(SLR_SUB_DISPLAY);
    memory_sub_set_wdma();
    memory_sub_update_capture_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        memory_sub_set_wdma_c();
        memory_sub_update_capture_C_buffer(3,  dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY));
    }
    memory_frc_set_capture_block_control(SLR_SUB_DISPLAY);
}

void memory_frc_main_set_display(void)
{
    memory_frc_set_pre_read_v_start(SLR_MAIN_DISPLAY, drv_Calculate_m_pre_read_value());
    drv_memory_set_vo_gating_threshold();

    memory_set_display_us_path(SLR_MAIN_DISPLAY);
    memory_main_set_rdma();
    memory_main_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_MAIN_DISPLAY) == TRUE)
    {
        memory_main_set_rdma_c();
        memory_main_update_display_C_buffer(dvrif_memory_get_buffer_number(SLR_MAIN_DISPLAY), dvrif_memory_get_buffer_size(SLR_MAIN_DISPLAY));
#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
        if (Get_rotate_function(SLR_MAIN_DISPLAY)) {
            rotate_update_display_buffer_limit();
        }
#endif
    }
    memory_frc_set_display_block_control(SLR_MAIN_DISPLAY);	
}

void memory_frc_sub_set_display(void)
{
    memory_frc_set_pre_read_v_start(SLR_SUB_DISPLAY, drv_Calculate_m_pre_read_value());
    drv_memory_set_vo_gating_threshold();

    memory_set_display_us_path(SLR_SUB_DISPLAY);
    memory_sub_set_rdma();
    memory_sub_update_display_buffer();
    if (dvrif_memory_get_block_mode_enable(SLR_SUB_DISPLAY) == TRUE)
    {
        memory_sub_set_rdma_c();
        memory_sub_update_display_C_buffer(dvrif_memory_get_buffer_number(SLR_SUB_DISPLAY), dvrif_memory_get_buffer_size(SLR_SUB_DISPLAY));
    }
    memory_frc_set_display_block_control(SLR_SUB_DISPLAY);
}

void dvrif_memory_frc_handler(unsigned char display)
{
#ifndef UT_flag
	extern unsigned char compare_rerify_vsc_source_num_result(unsigned char display);
#endif // #ifndef UT_flag
	/* should not disable ddomain interrupt DTG_ie.mv_den_sta_event_ie*/
	//if(display == SLR_MAIN_DISPLAY)
	//	drvif_scaler_ddomain_switch_irq(_DISABLE);	// avoid to DSR to switch buffer when double bit not set

	set_m_domain_setting_err_status(display, MDOMAIN_SETTING_NO_ERR); // reset M domain setting error status
	dvrif_memory_set_gatting_settings(display, _M_DATA_FRC_GATTING_MASK_LINE);

#ifdef CONFIG_ENABLE_SE_ROTATE_M_DOMAIN_BLOCK_MODE
    dvrif_memory_set_block_mode_enable(display, Get_rotate_function(display));
#else
    dvrif_memory_set_block_mode_enable(display, FALSE);
#endif

	if (display == SLR_MAIN_DISPLAY) {	// Main
		down(get_main_mdomain_ctrl_semaphore());
#ifndef BUILD_QUICK_SHOW
#ifndef UT_flag
		if(!compare_rerify_vsc_source_num_result(SLR_MAIN_DISPLAY))
		{
			up(get_main_mdomain_ctrl_semaphore());
			rtd_pr_scaler_memory_info("### dvrif_memory_handler vsc source chnage return ###\r\n");
			return;
		}
#endif // #ifndef UT_flag
#endif // #ifndef BUILD_QUICK_SHOW
		//Disable M-disp when reset M-domain=>To Fixed DTV change issue @ Crixus 20141126
		drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_MAIN_DISPLAY, _DISABLE);
		drvif_memory_DisableMCap(SLR_MAIN_DISPLAY); // Disable M-cap before do M-cap related setting

		memory_frc_decide_capture_mode(display);

		memory_frc_decide_buffer_number(display);

        memory_frc_main_set_capture();
		memory_frc_main_set_display();

		if((get_mdomain_1p_for_hdmi_4k120_flag()) && (get_panel_pixel_mode() > PANEL_1_PIXEL)
#ifndef BUILD_QUICK_SHOW
                        && (Get_rotate_function(display) == FALSE)
#endif
       )
		{//run sub mdomain for two pixel mode only for 120 panel
			set_m_domain_setting_err_status(SLR_SUB_DISPLAY, MDOMAIN_SETTING_NO_ERR); // reset M domain setting error status

			if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE)
			{
				if(dvrif_memory_get_compression_mode(display) == COMPRESSION_LINE_MODE)
				{
                    dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_LINE);
				}
				else
				{
                    dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME);
				}
			}
			else{
				dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME);
			}

            memory_frc_sub_set_capture();
            memory_frc_sub_set_display();
            //sub sync main pre read
            memory_frc_set_pre_read_v_start(SLR_SUB_DISPLAY, drv_Calculate_m_pre_read_value());

			dvrif_memory_comp_setting_sub(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) ? TRUE : FALSE, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));

            if(get_m_domain_setting_err_status(SLR_SUB_DISPLAY) == MDOMAIN_SETTING_NO_ERR)
				drvif_memory_EnableMCap(SLR_SUB_DISPLAY);
			else
				rtd_pr_scaler_memory_emerg("[%s] %d ERROR! Don't enable Mcap_en. (display=%d, err_status=%d)\n", __FUNCTION__, __LINE__, SLR_SUB_DISPLAY, get_m_domain_setting_err_status(SLR_SUB_DISPLAY));
			drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_SUB_DISPLAY, _ENABLE);
		} else if (get_mdomain_1p_for_4k60_vrr_flag()) {
			memory_frc_sub_set_display();
            //sub sync main pre read
            memory_frc_set_pre_read_v_start(SLR_SUB_DISPLAY, drv_Calculate_m_pre_read_value());

			dvrif_memory_comp_setting_sub(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) ? TRUE : FALSE, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_WID), Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_MEM_ACT_LEN), dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));

            drvif_memory_EnableMDisp_Or_DisableMDisp(SLR_SUB_DISPLAY, _ENABLE);
		}
         else if(check_sub_is_idle_for_srnn(display)) {
            set_sub_for_srnn(SUB_MDOMAIN_SRNN, 1);
        }
    }
#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
	else {	//Sub enable
		if (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_PIPMLSR_4x3)
		{// Sub PIP

            memory_frc_decide_capture_mode(display);
            memory_frc_decide_buffer_number(display);

			memory_frc_sub_set_capture();
            memory_frc_sub_set_display();
		}
		else if (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_MP1L7B)
		{// Sub MP
			memory_frc_sub_set_capture();
            memory_frc_sub_set_display();
			Scaler_PipSetSubInfo(SLR_PIP_SUB_MPENABLE,1);
		}
		else
		{

            memory_frc_decide_capture_mode(display);
            memory_frc_decide_buffer_number(display);

			memory_frc_sub_set_capture();
            memory_frc_sub_set_display();
		}
	}
#endif //CONFIG_DUAL_CHANNEL  // There are two channels

	//compression & de-compression setting
	if(display== SLR_MAIN_DISPLAY)
	{
		if((Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1) && (dvrif_memory_compression_get_enable(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) == TRUE)){
			dvrif_memory_comp_setting(TRUE, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(display));
			if(scaler_vsc_get_vr360_block_mdomain_doublebuffer()==FALSE)
				IoReg_SetBits(MDOMAIN_DISP_Disp0_db_ctrl_reg, MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
		}
		else{
			dvrif_memory_comp_setting(FALSE, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(display));
			if(scaler_vsc_get_vr360_block_mdomain_doublebuffer()==FALSE)
				IoReg_SetBits(MDOMAIN_DISP_Disp0_db_ctrl_reg, MDOMAIN_DISP_Disp0_db_ctrl_disp0_db_apply_mask);
		}
	}
#ifdef CONFIG_DUAL_CHANNEL
	else
	{
		//enable or disable compress mode for sub path.
		dvrif_memory_comp_setting_sub(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) ? TRUE : FALSE, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(display));
	}
#endif

#ifndef BUILD_QUICK_SHOW
    if(TRUE==Get_vscFilmMode())
	{
		//because film mode need 4 buffers
		FilmModeCachaddr = (unsigned long)dvr_malloc(drvif_memory_get_block_size(MEMIDX_MAIN));
		//FilmModePhyadddr = (unsigned int)virt_to_phys((void*)FilmModeCachaddr);
		#ifdef CONFIG_ARM64
		FilmModePhyadddr = (unsigned long)dvr_to_phys((void*)FilmModeCachaddr);
		#else
		FilmModePhyadddr = (unsigned int)dvr_to_phys((void*)FilmModeCachaddr);
		#endif
		filmmode_videofw_config(TRUE,FilmModePhyadddr);
	}
#endif

	//Game mode setting
	//Update the Game mode condition RGB444 @Crixus 20160222
	if (display == SLR_MAIN_DISPLAY) {
		//Eric@20181016 Vdec Direct Low Latency mode flow
		if(drv_memory_get_vdec_direct_low_latency_mode() && !((Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI) || (Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_LVDS_TO_HDMI)))
			drv_memory_GameMode_Switch_SingleBuffer();

#ifndef BUILD_QUICK_SHOW
		/*qiangzhou:all i3ddma source go vodma timing sync i3ddma,all i3ddma source should setting iv2pv delay*/
		if(((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_ADC)
			||((Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_AVD)  && (get_AVD_Input_Source() != _SRC_TV))
			||((false == is_hdmi_dolby_vision_sink_led()) && (get_vsc_src_is_hdmi_or_dp(display))))//dolby case set false
			&&(!drvif_i3ddma_triplebuf_flag())&&(!drvif_i3ddma_triplebuf_by_timing_protect_panel()) && (display == SLR_MAIN_DISPLAY)
			&& !(Scaler_Get_FreeRun_To_FrameSync_By_Reset_VODMA() && vbe_get_HDMI_run_timing_framesync_condition())
			&& (get_force_i3ddma_enable(display))){
			drv_I3ddmaVodma_GameMode_iv2pv_delay(TRUE);
		}else if (display == SLR_MAIN_DISPLAY){
			drv_I3ddmaVodma_GameMode_iv2pv_delay(FALSE);
		}
#endif
	}

	//limit boundary enable @Crixus 20180320
	check_and_set_mdomain_input_fast(display);

	if(get_m_domain_setting_err_status(display) == MDOMAIN_SETTING_NO_ERR)
		drvif_memory_EnableMCap(display);
	else
		rtd_pr_scaler_memory_emerg("[%s] %d ERROR! Don't enable Mcap_en. (display=%d, err_status=%d)\n", __FUNCTION__, __LINE__, display, get_m_domain_setting_err_status(display));
	drvif_memory_EnableMDisp_Or_DisableMDisp(display, _ENABLE);
	
    if (display == SLR_MAIN_DISPLAY)
		up(get_main_mdomain_ctrl_semaphore());
}

void memory_fs_main_set_capture(void)
{
    memory_fs_main_decide_buffer_size();
    memory_fs_main_alloc_buffer();

    memory_set_capture_ds_path(SLR_MAIN_DISPLAY);
    memory_main_update_capture_buffer();
}

void memory_fs_sub_set_capture(void)
{
    memory_fs_sub_decide_buffer_size();
    memory_fs_sub_alloc_buffer();
    memory_sub_update_capture_buffer();
}

void memory_fs_main_set_display(void)
{
    memory_set_display_us_path(SLR_MAIN_DISPLAY);
    memory_main_update_display_buffer();
}

void memory_fs_sub_set_display(void)
{
    memory_set_display_us_path(SLR_SUB_DISPLAY);
    memory_sub_update_display_buffer();
}

void dvrif_memory_fs_handler(unsigned char display)
{
    unsigned char buf_num = 0;

    drv_memory_set_vo_gating_threshold();
    memory_frc_set_pre_read_v_start(SLR_MAIN_DISPLAY, drv_Calculate_m_pre_read_value());
	dvrif_memory_set_gatting_settings(display, _M_DATA_FS_GATTING_MASK_LINE);

    if (display == SLR_MAIN_DISPLAY)
    {
        if((Scaler_InputSrcGetMainChType() == _SRC_VO) && (Scaler_VOFrom(Scaler_Get_CurVoInfo_plane()) == 1))	// yychao+ : use single buffer for JPEG source
        {
            buf_num = 1;
            dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_FRAME);	//  CSW 0961105 change single buffer to double to slove 1080i moving picture not smooth
        } else
        {
            buf_num = 3;
	#if 0
            if((Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) == _CHANNEL1) && (dvrif_memory_compression_get_enable(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL)) == TRUE)){
                //update M-domain PQC frame mode @Crixus 20170626
                if(dvrif_memory_get_compression_mode(SLR_MAIN_DISPLAY) == COMPRESSION_LINE_MODE){
                    dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_LINE);
                } else {
                    dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_FRAME);
                }
            } else {
                dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_FRAME);	//  CSW 0961105 change single buffer to double to slove 1080i moving picture not smooth
            }
	#endif
	    dvrif_memory_set_cap_mode(SLR_MAIN_DISPLAY, MEMCAPTYPE_LINE);
        }

#ifdef ENABLE_3_BUFFER_DELAY_MODE
        buf_num = 3;
#endif
        dvrif_memory_set_buffer_number(SLR_MAIN_DISPLAY, buf_num);

        memory_fs_main_set_capture();
        memory_fs_main_set_display();
    }
 #ifdef CONFIG_DUAL_CHANNEL
    else {
        if (Scaler_PipGetInfo(SLR_PIP_TYPE) <= SUBTYPE_PIPMLSR_4x3)
        {// Sub PIP
            buf_num = 3;
            dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME);
        }
        else
        {
            if(dvrif_memory_compression_get_enable(SLR_SUB_DISPLAY) == TRUE){
                //update M-domain PQC frame mode @Crixus 20170626
                if(dvrif_memory_get_compression_mode(SLR_SUB_DISPLAY) == COMPRESSION_LINE_MODE){
                    buf_num = 3;
                    dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_LINE);
                } else {
                    buf_num = 3;
                    dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME);
                }
            }else{
                buf_num = 3;
                dvrif_memory_set_cap_mode(SLR_SUB_DISPLAY, MEMCAPTYPE_FRAME); // memory, capture part for other cases
            }
        }

        dvrif_memory_set_buffer_number(SLR_SUB_DISPLAY, buf_num);

        memory_fs_sub_set_capture();
        memory_fs_sub_set_display();
    }
#endif

	//data fs need to disable compression clock @Crixus 20171227
	dvrif_memory_comp_setting(FALSE, Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(SLR_MAIN_DISPLAY));
	if(get_mdomain_1p_for_hdmi_4k120_flag())
		dvrif_memory_comp_setting_sub(FALSE, Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_WID), Scaler_DispGetInputInfoByDisp(SLR_SUB_DISPLAY, SLR_INPUT_CAP_LEN), dvrif_memory_get_compression_bits(SLR_SUB_DISPLAY));
}

/*======================== End of File =======================================*/
/**
 *
 * @}
 */

