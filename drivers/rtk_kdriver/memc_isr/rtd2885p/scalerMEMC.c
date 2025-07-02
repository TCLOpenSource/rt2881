
//#include <string.h>

//#include <stdio.h>
//#include <stdlib.h>

//#include <rtio.h>
//#include <VP_def.h>

//#include <auto_conf.h> // Provides: CONFIG_VBI_CC
//#include <rtd_system.h>
//#include <rtd_memmap.h>
#include <memc_isr/scalerMEMC.h>
#include "vgip_isr/scalerVIP.h"
#ifndef BUILD_QUICK_SHOW
#include <linux/mutex.h>
#include <linux/version.h> 
#endif
//#include <mach/io.h>

#if 1//RTK_MEMC_Performance_tunging_from_tv001
#include <tvscalercontrol/panel/panelapi.h>
#include <mach/platform.h>
#include "memc_isr/include/PQLAPI.h"
#include "memc_isr/Platform/memc_change_size.h"
#endif

#include <memc_isr/include/memc_lib.h>
#include "memc_reg_def.h"
#include "memc_isr/Common/kw_debug.h" // for LogPrintf
#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/MID/mid_crtc.h"
#include <mach/platform.h>

#include "tvscalercontrol/vip/vip_reg_def.h"
#include <tvscalercontrol/scalerdrv/scalerdrv.h>
#include <tvscalercontrol/panel/panelapi.h>
#include <tvscalercontrol/io/ioregdrv.h>
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
#include <scaler/scalerDrvCommon.h>
#else
#include <scalercommon/scalerDrvCommon.h>
#endif
#include <tvscalercontrol/scalerdrv/scaler_nonlibdma.h>
#include <mach/rtk_platform.h>

#include <rbus/sb2_reg.h> // for hardware semaphore

//#include "memc_isr/Driver/KI7436_reg.h"
//#include "memc_isr/Driver/KI7576_reg.h"
#include "memc_isr/Common/kw_debug.h" // for LogPrintf
#include "memc_isr/PQL/PQLPlatformDefs.h"
#include <tvscalercontrol/panel/panelapi.h>
#ifndef BUILD_QUICK_SHOW
#include <linux/version.h>
#endif
#include "memc_isr/Platform/memc_change_size.h"
#ifdef BUILD_QUICK_SHOW
#include <rtk_kdriver/rmm/rmm.h>
#endif

//#include "scaler_vpqmemcdev.h"

// for register dump
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

#define MEMC_IRQ_HANDLED 1
#define MEMC_IRQ_NONE       0

static unsigned char scaler_memc_enable = 0;
#ifndef MEMC_INIT_STATUS
static unsigned char g_scaler_memc_acon_init = 0;//for ac off->on: dma enable delay 2 vsync than slavemode set, clear when dc suspend
#else
static SCALER_MEMC_INIT_STATUS g_scaler_memc_init_status = MEMC_INIT_NONE;
#endif

#ifdef CONFIG_MEMC_TASK_QUEUE
#define MEMC_TASK_MAX_CNT 34 //Total element count of memc task queue
static SCALER_MEMC_TASK_T g_memc_tasks_queue[MEMC_TASK_MAX_CNT];
static unsigned int g_memc_tasks_header = 0;
static unsigned int g_memc_tasks_tailer = 0;
#endif
extern bool g_new_sport_mode;
unsigned char g_scaler_memc_cadence[_FILM_MEMC_NUM] = {0};

unsigned int g_ucMemcDelay;
int LowDelay_mode = 0;

unsigned char g_MEMC_powersaving_on = 0;

extern unsigned char MEMC_BRING_UP_InitDone_flag;
extern unsigned int HDMI_PowerSaving_stage;
extern int g_memc_hdmi_switch_state;
extern int g_memc_switch_state;
extern unsigned int DTV_PowerSaving_stage;
extern bool g_KERNEL_AFTER_QS_MEMC_FLG;
extern VOID Mid_MISC_SetInINTEnable(INT_TYPE enIntType, BOOL bEnable);
extern VOID Mid_MISC_SetOutINTEnable(INT_TYPE enIntType, BOOL bEnable);
extern unsigned int Get_DISPLAY_REFRESH_RATE(void);
extern unsigned char MEMC_Lib_get_memc_PowerSaving_Mode(VOID);
extern unsigned char get_MEMC_bypass_status_refer_platform_model(void);
extern bool MEMC_Lib_Get_QS_PCmode_Flg(void);
extern unsigned int HAL_MC_GetRepeatMode(VOID);

#if RTK_MEMC_Performance_tunging_from_tv001
extern unsigned char Scaler_MEMC_GetPanelSizeByDisp(void);
#ifdef BUILD_QUICK_SHOW
extern unsigned int Get_DISP_HORIZONTAL_TOTAL(void);
#endif
#endif

VOID MEMC_ISR_test(void);

//==================================================================================================

unsigned int Get_KMC_TOTAL_SIZE(void)
{
#if CONFIG_MC_8_BUFFER

#if ENABLE_4K1K
	if(Get_DISPLAY_REFRESH_RATE() <= 60 || (Get_DISP_ACT_END_VPOS() <= 1088 && Get_DISPLAY_REFRESH_RATE() >= 120)){
#else
	if(Get_DISPLAY_REFRESH_RATE() <= 60){
#endif
		#ifdef BUILD_QUICK_SHOW
			return KMC_TOTAL_SIZE_8buf_8bit_QS;
		#else
			if(g_KERNEL_AFTER_QS_MEMC_FLG || ((get_product_type() == PRODUCT_TYPE_DIAS)&& (Scaler_MEMC_Get_SW_OPT_Flag() == 1))){
				return KMC_TOTAL_SIZE_8buf_8bit_QS;
			}else{
				return KMC_TOTAL_SIZE_8buf_8bit;
			}
		#endif
	}else{
		return KMC_TOTAL_SIZE_8buf_10bit;
	}
#else
	return KMC_TOTAL_SIZE_6buf_10bit;	
#endif
}

unsigned int Get_KMEMC_TOTAL_SIZE(void)
{
#if CONFIG_MC_8_BUFFER
#if ENABLE_4K1K
		if(Get_DISPLAY_REFRESH_RATE() <= 60 || (Get_DISP_ACT_END_VPOS() <= 1088 && Get_DISPLAY_REFRESH_RATE() >= 120)){
#else
		if(Get_DISPLAY_REFRESH_RATE() <= 60){
#endif

		#ifdef BUILD_QUICK_SHOW
			return KMEMC_TOTAL_SIZE_8buf_8bit_QS;
		#else
			if(g_KERNEL_AFTER_QS_MEMC_FLG || ((get_product_type() == PRODUCT_TYPE_DIAS)&& (Scaler_MEMC_Get_SW_OPT_Flag() == 1))){
				return KMEMC_TOTAL_SIZE_8buf_8bit_QS;
			}else{
				return KMEMC_TOTAL_SIZE_8buf_8bit;
			}
		#endif
	}else{
		return KMEMC_TOTAL_SIZE_8buf_10bit;
	}
#else
	return KMEMC_TOTAL_SIZE_6buf_10bit;
#endif
}

void memc_suspend(void)
{
	Scaler_MEMC_SetInitFlag(0);//clear when dc suspend
	Scaler_MEMC_resetMEMCMode();

}

void Scaler_MEMC_SetInitFlag(unsigned char val)
{
#ifndef MEMC_INIT_STATUS
	if(val>1)
		g_scaler_memc_acon_init = 0;
	else
		g_scaler_memc_acon_init = val;
#else
	g_scaler_memc_init_status = val;
#endif

}

unsigned char Scaler_MEMC_GetInitFlag(void)
{
#ifndef MEMC_INIT_STATUS

	return g_scaler_memc_acon_init;
#else
	return g_scaler_memc_init_status;
#endif
}

void Scaler_MEMC_DMA_DB_Check_RTK2885p(void)
{
	unsigned int u32_RB_val=0;
	
	rtd_pr_memc_notice("[MEMC][DMA_DB_Check_1]=[,%x,%x,%x,%x,%x]\n",rtd_inl(MC_DMA_MC_WDMA_DB_CTRL_reg),rtd_inl(MC_DMA_MC_RDMA_DB_CTRL_reg),rtd_inl(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg),rtd_inl(PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg),rtd_inl(LBMC_LBMC_EC_reg));
	
	u32_RB_val=IoReg_Read32(MC_DMA_MC_WDMA_DB_CTRL_reg);//MC_WDMA
	if( (u32_RB_val&0x1)==1){
		IoReg_ClearBits(MC_DMA_MC_WDMA_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(MC_DMA_MC_WDMA_DB_CTRL_reg,_BIT1);
		rtd_pr_memc_notice("MC_WDMA 0xb80994F0=%x\n", IoReg_Read32(MC_DMA_MC_WDMA_DB_CTRL_reg));
		//MC_LF_W uplimit_addr
		rtd_pr_memc_notice("MC_LF_W up limit 0xb8099458=%x\n", IoReg_Read32(MC_DMA_MC_LF_DMA_WR_Rule_check_up_reg));
		//MC_LF_W downlimit_addr
		rtd_pr_memc_notice("MC_LF_W down limit 0xb809945C=%x\n", IoReg_Read32(MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg));
		//MC_HF_W uplimit_addr
		rtd_pr_memc_notice("MC_HF_W up limit 0xb80994D0=%x\n", IoReg_Read32(MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg));
		//MC_HF_W downlimit_addr
		rtd_pr_memc_notice("MC_HF_W down limit 0xb80994D8=%x\n", IoReg_Read32(MC_DMA_MC_HF_DMA_WR_Rule_check_low_reg));
		
	}
	
	u32_RB_val=IoReg_Read32(MC_DMA_MC_RDMA_DB_CTRL_reg);//MC_RDMA
	if( (u32_RB_val&0x1)==1){
		IoReg_ClearBits(MC_DMA_MC_RDMA_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(MC_DMA_MC_RDMA_DB_CTRL_reg,_BIT1);
		rtd_pr_memc_notice("MC_RDMA_DB 0xb809AEA8=%x\n", IoReg_Read32(MC_DMA_MC_RDMA_DB_CTRL_reg));
		//MC_LF_I_R uplimit_addr
		rtd_pr_memc_notice("MC_LF_I_R up limit 0xb809AE14=%x\n", IoReg_Read32(MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_reg));		
		//MC_LF_I_R downlimit_addr
		rtd_pr_memc_notice("MC_LF_I_R down limit 0xb809AE18=%x\n", IoReg_Read32(MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_reg));
		//MC_HF_I_R uplimit_addr
		rtd_pr_memc_notice("MC_HF_I_R up limit 0xb809AE34=%x\n", IoReg_Read32(MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_reg));
		//MC_HF_I_R downlimit_addr
		rtd_pr_memc_notice("MC_HF_I_R down limit 0xb809AE38=%x\n", IoReg_Read32(MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_reg));
		//MC_LF_P_R uplimit_addr
		rtd_pr_memc_notice("MC_LF_P_R up limit 0xb809AE54=%x\n", IoReg_Read32(MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_reg));
		//MC_LF_P_R downlimit_addr
		rtd_pr_memc_notice("MC_LF_P_R down limit 0xb809AE58=%x\n", IoReg_Read32(MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_reg));
		//MC_HF_P_R uplimit_addr
		rtd_pr_memc_notice("MC_HF_P_R up limit 0xb809AE74=%x\n", IoReg_Read32(MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_reg));
		//MC_HF_P_R downlimit_addr
		rtd_pr_memc_notice("MC_HF_P_R down limit 0xb809AE78=%x\n", IoReg_Read32(MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_reg));
		
	}
	
	u32_RB_val=IoReg_Read32(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg);//DECOMP_DB
	if( (u32_RB_val&0x1)==1){
		rtd_pr_memc_notice("DECOMP_DB 0xb8099850=%x\n", IoReg_Read32(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg));
		IoReg_ClearBits(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg,_BIT1);				
	}

	u32_RB_val=IoReg_Read32(PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg);//COMP_DB
	if( (u32_RB_val&0x1)==1){
		rtd_pr_memc_notice("COMP_DB 0xb8099268=%x\n", IoReg_Read32(PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg));
		IoReg_ClearBits(PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg,_BIT1);				
	}

	u32_RB_val=IoReg_Read32(LBMC_LBMC_EC_reg);//LBMC_DB
	if( (u32_RB_val&0x1)==1){
		rtd_pr_memc_notice("LBMC_DB 0xb80999ec=%x\n", IoReg_Read32(LBMC_LBMC_EC_reg));
		IoReg_ClearBits(LBMC_LBMC_EC_reg,_BIT0);
		IoReg_ClearBits(LBMC_LBMC_EC_reg,_BIT1);
		rtd_pr_memc_notice("PC_mode_check 0xb8099924=%x\n", IoReg_Read32(LBMC_LBMC_24_reg));
		
	}

	IoReg_SetBits(MC_DMA_MC_WDMA_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(MC_DMA_MC_RDMA_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(LBMC_LBMC_EC_reg,_BIT1);
	rtd_pr_memc_notice("[MEMC][DMA_DB_Check_2]=[,%x,%x,%x,%x,%x]\n",rtd_inl(MC_DMA_MC_WDMA_DB_CTRL_reg),rtd_inl(MC_DMA_MC_RDMA_DB_CTRL_reg),rtd_inl(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg),rtd_inl(PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg),rtd_inl(LBMC_LBMC_EC_reg));
	
}

void Scaler_MEMC_DMA_DB_Check_RTK2885pp(void)
{
	unsigned int u32_RB_val=0;
	
	rtd_pr_memc_notice("[MEMC][DMA_DB_Check_1]=[,%x,%x,%x,%x,%x]\n",rtd_inl(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg),rtd_inl(M8P_RTHW_LBMC_LBMC_DB_reg));
	
	u32_RB_val=IoReg_Read32(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg);//MC_WDMA
	if( (u32_RB_val&0x1)==1){
		IoReg_ClearBits(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg,_BIT1);
		rtd_pr_memc_notice("MC_WDMA 0xb80996F0=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg));
		//MC_LF_W uplimit_addr
		rtd_pr_memc_notice("MC_LF_W up limit 0xb8099658=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_LF_DMA_WR_Rule_check_up_reg));
		//MC_LF_W downlimit_addr
		rtd_pr_memc_notice("MC_LF_W down limit 0xb809965C=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg));
		//MC_HF_W uplimit_addr
		rtd_pr_memc_notice("MC_HF_W up limit 0xb80996D0=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg));
		//MC_HF_W downlimit_addr
		rtd_pr_memc_notice("MC_HF_W down limit 0xb80996D8=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_HF_DMA_WR_Rule_check_low_reg));
		
	}
	
	u32_RB_val=IoReg_Read32(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg);//MC_RDMA
	if( (u32_RB_val&0x1)==1){
		IoReg_ClearBits(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg,_BIT1);
		rtd_pr_memc_notice("MC_RDMA_DB 0xb80997A8=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg));
		//MC_LF_I_R uplimit_addr
		rtd_pr_memc_notice("MC_LF_I_R up limit 0xb8099714=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_reg));		
		//MC_LF_I_R downlimit_addr
		rtd_pr_memc_notice("MC_LF_I_R down limit 0xb8099718=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_reg));
		//MC_HF_I_R uplimit_addr
		rtd_pr_memc_notice("MC_HF_I_R up limit 0xb8099734=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_reg));
		//MC_HF_I_R downlimit_addr
		rtd_pr_memc_notice("MC_HF_I_R down limit 0xb8099738=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_reg));
		//MC_LF_P_R uplimit_addr
		rtd_pr_memc_notice("MC_LF_P_R up limit 0xb8099754=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_reg));
		//MC_LF_P_R downlimit_addr
		rtd_pr_memc_notice("MC_LF_P_R down limit 0xb8099758=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_reg));
		//MC_HF_P_R uplimit_addr
		rtd_pr_memc_notice("MC_HF_P_R up limit 0xb8099774=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_reg));
		//MC_HF_P_R downlimit_addr
		rtd_pr_memc_notice("MC_HF_P_R down limit 0xb8099778=%x\n", IoReg_Read32(M8P_RTHW_MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_reg));
		
	}
	
	u32_RB_val=IoReg_Read32(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg);//DECOMP_DB
	if( (u32_RB_val&0x1)==1){
		rtd_pr_memc_notice("DECOMP_DB 0xb8099850=%x\n", IoReg_Read32(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg));
		IoReg_ClearBits(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg,_BIT1);				
	}

	u32_RB_val=IoReg_Read32(M8P_RTHW_MC_PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg);//COMP_DB
	if( (u32_RB_val&0x1)==1){
		rtd_pr_memc_notice("COMP_DB 0xb8099268=%x\n", IoReg_Read32(M8P_RTHW_MC_PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg));
		IoReg_ClearBits(M8P_RTHW_MC_PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg,_BIT0);
		IoReg_ClearBits(M8P_RTHW_MC_PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg,_BIT1);				
	}

	u32_RB_val=IoReg_Read32(M8P_RTHW_LBMC_LBMC_DB_reg);//LBMC_DB
	if( (u32_RB_val&0x1)==1){
		rtd_pr_memc_notice("LBMC_DB 0xb80999ec=%x\n", IoReg_Read32(M8P_RTHW_LBMC_LBMC_DB_reg));
		IoReg_ClearBits(M8P_RTHW_LBMC_LBMC_DB_reg,_BIT0);
		IoReg_ClearBits(M8P_RTHW_LBMC_LBMC_DB_reg,_BIT1);
		rtd_pr_memc_notice("PC_mode_check 0xb809A618=%x\n", IoReg_Read32(M8P_RTMC_LBMC_LBMC_CTRL_reg));
		
	}

	IoReg_SetBits(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(M8P_RTHW_MC_PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg,_BIT1);
	IoReg_SetBits(M8P_RTHW_LBMC_LBMC_DB_reg,_BIT1);
	rtd_pr_memc_notice("[MEMC][DMA_DB_Check_2]=[,%x,%x,%x,%x,%x]\n",rtd_inl(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_PQC_PQDC_MC_PQ_CMP_DB_CTRL_reg),rtd_inl(M8P_RTHW_LBMC_LBMC_DB_reg));
	
}

void Scaler_MEMC_DMA_DB_Check(void)
{
	if(RUN_MERLIN8P()){
		Scaler_MEMC_DMA_DB_Check_RTK2885pp();
	}else if(RUN_MERLIN8()){
		Scaler_MEMC_DMA_DB_Check_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8
}

void Scaler_MEMC_Set_MC_MEMBoundary_RTK2885p(unsigned int  nDownlimit,unsigned int nUplimit)
{

#if RTK_MEMC_Performance_tunging_from_tv001
	unsigned int u32_temp_boundary = 0;
	unsigned int u32_MC_Boundary = MEMC_Lib_Get_MC_Boundary(); // mc dma boundary is generated by memc_change_size

	// check the up and down limit of mc boundary
	if(nDownlimit>=nUplimit){
		rtd_pr_memc_emerg("[%s][%d] MC up/down DMA limit error. DownLimit:%x, UpLimit:%x\n", __func__, __LINE__, nDownlimit, nUplimit);
		nDownlimit = 0;
		nUplimit = 0xffffffff;
	}
	else {
		if(u32_MC_Boundary!=0) { // MEMC_MC_DMA_BOUNDARY is generated by memc_change_size
			if(nUplimit-nDownlimit<u32_MC_Boundary){
				rtd_pr_memc_emerg("[%s][%d] abnormal MC DMA boundary. DownLimit:%x, UpLimit:%x, boundary range:%x\n", __func__, __LINE__, nDownlimit, nUplimit, u32_MC_Boundary);

				// protect boundary
				if((0xffffffff-u32_MC_Boundary)<nDownlimit) {
					nDownlimit = 0;
					nUplimit = 0xffffffff;
				}
				else {
					nUplimit = u32_MC_Boundary+nDownlimit;
				}
			}
		}
		else { // MEMC_MC_DMA_BOUNDARY==0
			rtd_pr_memc_emerg("[%s][%d] abnormal MEMC_MC_DMA_BOUNDARY(0)\n", __func__, __LINE__);

			#if 0
			#if CONFIG_MC_8_BUFFER
			if(Get_DISPLAY_REFRESH_RATE() <= 60){
				u32_temp_boundary = KMC_TOTAL_SIZE_8buf_8bit;
			}else{
				u32_temp_boundary = KMC_TOTAL_SIZE_8buf_10bit;
			}
			#else
			u32_temp_boundary = KMC_TOTAL_SIZE_6buf_10bit;
			#endif
			#else
			u32_temp_boundary = Get_KMC_TOTAL_SIZE();
			#endif

			// protect boundary
			if(u32_temp_boundary==0 || (0xffffffff-u32_temp_boundary)<nDownlimit) {
				nDownlimit = 0;
				nUplimit = 0xffffffff;
			}
			else {
				nUplimit = u32_temp_boundary+nDownlimit;
			}
		}
	}
#endif

	rtd_pr_memc_emerg("[MEMC_BRINGUP_TEST][CCC][9458=,%x,]\n", nUplimit);

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
}

void Scaler_MEMC_Set_MC_MEMBoundary_RTK2885pp(unsigned int  nDownlimit,unsigned int nUplimit)
{
	rtd_pr_memc_emerg("[MEMC_BRINGUP_TEST][CCC][9658=,%x,]\n", nUplimit);

	IoReg_Write32(M8P_RTHW_MC_DMA_MC_LF_DMA_WR_Rule_check_up_reg, nUplimit);
	IoReg_Write32(M8P_RTHW_MC_DMA_MC_LF_DMA_WR_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(M8P_RTHW_MC_DMA_MC_HF_DMA_WR_Rule_check_up_reg, nUplimit);
	IoReg_Write32(M8P_RTHW_MC_DMA_MC_HF_DMA_WR_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(M8P_RTHW_MC_DMA_MC_HF_I_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(M8P_RTHW_MC_DMA_MC_HF_I_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(M8P_RTHW_MC_DMA_MC_LF_P_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(M8P_RTHW_MC_DMA_MC_LF_P_DMA_RD_Rule_check_low_reg, nDownlimit);

	IoReg_Write32(M8P_RTHW_MC_DMA_MC_HF_P_DMA_RD_Rule_check_up_reg, nUplimit);
	IoReg_Write32(M8P_RTHW_MC_DMA_MC_HF_P_DMA_RD_Rule_check_low_reg, nDownlimit);
}

void Scaler_MEMC_Set_MC_MEMBoundary(unsigned int  nDownlimit,unsigned int nUplimit)
{
	if(RUN_MERLIN8P()){
		Scaler_MEMC_Set_MC_MEMBoundary_RTK2885pp(nDownlimit, nUplimit);
	}else if(RUN_MERLIN8()){
		Scaler_MEMC_Set_MC_MEMBoundary_RTK2885p(nDownlimit, nUplimit);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

void Scaler_MEMC_Set_ME_MEMBoundary_RTK2885p(unsigned int  nDownlimit,unsigned int nUplimit)
{
	IoReg_Write32(DBUS_WRAPPER_Me_downlimit_addr_reg, nDownlimit);
	IoReg_Write32(DBUS_WRAPPER_Me_uplimit_addr_reg, nUplimit);
}

void Scaler_MEMC_Set_ME_MEMBoundary_RTK2885pp(unsigned int  nDownlimit,unsigned int nUplimit)
{
//MERLIN8P_MV_RELATE
//DMA_CHECK
	M8P_rthw_me_dma_dm0_wr_rule_check_up_RBUS M8P_rthw_me_dma_dm0_wr_rule_check_up;
	M8P_rthw_me_dma_dm1_wr_rule_check_up_RBUS M8P_rthw_me_dma_dm1_wr_rule_check_up;
	M8P_rthw_me_dma_dm2_wr_rule_check_up_RBUS M8P_rthw_me_dma_dm2_wr_rule_check_up;
	M8P_rthw_me_dma_dm3_rd_rule_check_up_RBUS M8P_rthw_me_dma_dm3_rd_rule_check_up;
	M8P_rthw_me_dma_dm4_rd_rule_check_up_RBUS M8P_rthw_me_dma_dm4_rd_rule_check_up;
	M8P_rthw_me_dma_dm5_rd_rule_check_up_RBUS M8P_rthw_me_dma_dm5_rd_rule_check_up;
	M8P_rthw_me_dma_dm6_rd_rule_check_up_RBUS M8P_rthw_me_dma_dm6_rd_rule_check_up;
	M8P_rthw_me_dma_dm7_rd_rule_check_up_RBUS M8P_rthw_me_dma_dm7_rd_rule_check_up;

	M8P_rthw_me_dma_dm0_wr_rule_check_low_RBUS M8P_rthw_me_dma_dm0_wr_rule_check_low;
	M8P_rthw_me_dma_dm1_wr_rule_check_low_RBUS M8P_rthw_me_dma_dm1_wr_rule_check_low;
	M8P_rthw_me_dma_dm2_wr_rule_check_low_RBUS M8P_rthw_me_dma_dm2_wr_rule_check_low;
	M8P_rthw_me_dma_dm3_rd_rule_check_low_RBUS M8P_rthw_me_dma_dm3_rd_rule_check_low;
	M8P_rthw_me_dma_dm4_rd_rule_check_low_RBUS M8P_rthw_me_dma_dm4_rd_rule_check_low;
	M8P_rthw_me_dma_dm5_rd_rule_check_low_RBUS M8P_rthw_me_dma_dm5_rd_rule_check_low;
	M8P_rthw_me_dma_dm6_rd_rule_check_low_RBUS M8P_rthw_me_dma_dm6_rd_rule_check_low;
	M8P_rthw_me_dma_dm7_rd_rule_check_low_RBUS M8P_rthw_me_dma_dm7_rd_rule_check_low;

	M8P_rthw_me_dma_dm0_wr_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_Rule_check_up_reg);
	M8P_rthw_me_dma_dm1_wr_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_Rule_check_up_reg);
	M8P_rthw_me_dma_dm2_wr_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_Rule_check_up_reg);
	M8P_rthw_me_dma_dm3_rd_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_Rule_check_up_reg);
	M8P_rthw_me_dma_dm4_rd_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_Rule_check_up_reg);
	M8P_rthw_me_dma_dm5_rd_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_Rule_check_up_reg);
	M8P_rthw_me_dma_dm6_rd_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_Rule_check_up_reg);
	M8P_rthw_me_dma_dm7_rd_rule_check_up.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_Rule_check_up_reg);

	M8P_rthw_me_dma_dm0_wr_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_Rule_check_low_reg);
	M8P_rthw_me_dma_dm1_wr_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_Rule_check_low_reg);
	M8P_rthw_me_dma_dm2_wr_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_Rule_check_low_reg);
	M8P_rthw_me_dma_dm3_rd_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_Rule_check_low_reg);
	M8P_rthw_me_dma_dm4_rd_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_Rule_check_low_reg);
	M8P_rthw_me_dma_dm5_rd_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_Rule_check_low_reg);
	M8P_rthw_me_dma_dm6_rd_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_Rule_check_low_reg);
	M8P_rthw_me_dma_dm7_rd_rule_check_low.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_Rule_check_low_reg);

	M8P_rthw_me_dma_dm0_wr_rule_check_up.dma_up_limit = nUplimit;
	M8P_rthw_me_dma_dm1_wr_rule_check_up.dma_up_limit = nUplimit;
	M8P_rthw_me_dma_dm2_wr_rule_check_up.dma_up_limit = nUplimit;
	M8P_rthw_me_dma_dm3_rd_rule_check_up.dma_up_limit = nUplimit;
	M8P_rthw_me_dma_dm4_rd_rule_check_up.dma_up_limit = nUplimit;
	M8P_rthw_me_dma_dm5_rd_rule_check_up.dma_up_limit = nUplimit;
	M8P_rthw_me_dma_dm6_rd_rule_check_up.dma_up_limit = nUplimit;
	M8P_rthw_me_dma_dm7_rd_rule_check_up.dma_up_limit = nUplimit;

	M8P_rthw_me_dma_dm0_wr_rule_check_low.dma_low_limit = nDownlimit;
	M8P_rthw_me_dma_dm1_wr_rule_check_low.dma_low_limit = nDownlimit;
	M8P_rthw_me_dma_dm2_wr_rule_check_low.dma_low_limit = nDownlimit;
	M8P_rthw_me_dma_dm3_rd_rule_check_low.dma_low_limit = nDownlimit;
	M8P_rthw_me_dma_dm4_rd_rule_check_low.dma_low_limit = nDownlimit;
	M8P_rthw_me_dma_dm5_rd_rule_check_low.dma_low_limit = nDownlimit;
	M8P_rthw_me_dma_dm6_rd_rule_check_low.dma_low_limit = nDownlimit;
	M8P_rthw_me_dma_dm7_rd_rule_check_low.dma_low_limit = nDownlimit;

	rtd_outl(M8P_RTHW_ME_DMA_DM0_WR_Rule_check_up_reg, M8P_rthw_me_dma_dm0_wr_rule_check_up.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM1_WR_Rule_check_up_reg, M8P_rthw_me_dma_dm1_wr_rule_check_up.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM2_WR_Rule_check_up_reg, M8P_rthw_me_dma_dm2_wr_rule_check_up.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM3_RD_Rule_check_up_reg, M8P_rthw_me_dma_dm3_rd_rule_check_up.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM4_RD_Rule_check_up_reg, M8P_rthw_me_dma_dm4_rd_rule_check_up.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM5_RD_Rule_check_up_reg, M8P_rthw_me_dma_dm5_rd_rule_check_up.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM6_RD_Rule_check_up_reg, M8P_rthw_me_dma_dm6_rd_rule_check_up.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM7_RD_Rule_check_up_reg, M8P_rthw_me_dma_dm7_rd_rule_check_up.regValue);

	rtd_outl(M8P_RTHW_ME_DMA_DM0_WR_Rule_check_low_reg, M8P_rthw_me_dma_dm0_wr_rule_check_low.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM1_WR_Rule_check_low_reg, M8P_rthw_me_dma_dm1_wr_rule_check_low.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM2_WR_Rule_check_low_reg, M8P_rthw_me_dma_dm2_wr_rule_check_low.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM3_RD_Rule_check_low_reg, M8P_rthw_me_dma_dm3_rd_rule_check_low.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM4_RD_Rule_check_low_reg, M8P_rthw_me_dma_dm4_rd_rule_check_low.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM5_RD_Rule_check_low_reg, M8P_rthw_me_dma_dm5_rd_rule_check_low.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM6_RD_Rule_check_low_reg, M8P_rthw_me_dma_dm6_rd_rule_check_low.regValue);
	rtd_outl(M8P_RTHW_ME_DMA_DM7_RD_Rule_check_low_reg, M8P_rthw_me_dma_dm7_rd_rule_check_low.regValue);

	IoReg_Write32(ME_UP_LIMIT_REG, nDownlimit);
	IoReg_Write32(ME_DOWN_LIMIT_REG, nUplimit);
}

void Scaler_MEMC_Set_ME_MEMBoundary(unsigned int  nDownlimit,unsigned int nUplimit)
{
	if(RUN_MERLIN8P()){
		Scaler_MEMC_Set_ME_MEMBoundary_RTK2885pp(nDownlimit, nUplimit);
	}else if(RUN_MERLIN8()){
		Scaler_MEMC_Set_ME_MEMBoundary_RTK2885p(nDownlimit, nUplimit);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

#if 1//#if RTK_MEMC_Performance_tunging_from_tv001
unsigned int Scaler_MEMC_Get_Size_Addr(unsigned long *memc_addr_aligned, unsigned long *memc_addr)
{
	unsigned int memc_size = 0;
	static unsigned char status = 0;

#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
	memc_size = (unsigned int)carvedout_buf_query_secure(CARVEDOUT_SCALER_MEMC, (void **)memc_addr);
#else
	memc_size = (unsigned int)carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void *)memc_addr);
#endif
	#ifdef CONFIG_BW_96B_ALIGNED
	#if 0
	#if CONFIG_MC_8_BUFFER
	if(Get_DISPLAY_REFRESH_RATE() <= 60){
		*memc_addr_aligned = dvr_memory_alignment((unsigned long)*memc_addr, KMEMC_TOTAL_SIZE/*KMEMC_TOTAL_SIZE_8buf_8bit*/);
	}else{
		*memc_addr_aligned = dvr_memory_alignment((unsigned long)*memc_addr, KMEMC_TOTAL_SIZE_8buf_10bit);
	}
	#else
	*memc_addr_aligned = dvr_memory_alignment((unsigned long)*memc_addr, KMEMC_TOTAL_SIZE_6buf_10bit);
	#endif
	#else
	*memc_addr_aligned = dvr_memory_alignment((unsigned long)*memc_addr, Get_KMEMC_TOTAL_SIZE());
	#endif
	if (*memc_addr == 0 || memc_size == 0  || *memc_addr_aligned==0) {
		if(status == 0){
			rtd_pr_memc_emerg("[%s %d] ERR : %lx %x %lx\n", __func__, __LINE__, *memc_addr, memc_size,*memc_addr_aligned);
			status = 1;
		}
		return 0;
	}
	else{
		if(status == 1){
			rtd_pr_memc_emerg("[%s %d] SUCCESS : %lx %x %lx\n", __func__, __LINE__, *memc_addr, memc_size,*memc_addr_aligned);
			status = 0;
		}
	}
	#else
	if (*memc_addr == 0 || memc_size == 0) {
		if(status == 0){
			rtd_pr_memc_emerg("[%s %d] ERR : %lx %x\n", __func__, __LINE__, *memc_addr, memc_size);
			status = 1;
		}
		return 0;
	}
	else{
		if(status == 1){
			rtd_pr_memc_emerg("[%s %d] SUCCESS : %lx %x\n", __func__, __LINE__, *memc_addr, memc_size);
			status = 0;
		}
	}
	#endif

	return memc_size;
}

#endif

extern bool get_reset_MEMC_flg(void);
extern void set_reset_MEMC_flg(bool enable);
extern void Scaler_MEMC_allocate_memory(void);
#ifdef BUILD_QUICK_SHOW
extern bool carvedout_enable(void);
#endif

void Scaler_MEMC_Initialize(unsigned int ME_start_addr, unsigned int MC00_start_addr, unsigned int MC01_start_addr)
{
#if 0 //def MEMC_BRING_UP
	rtd_pr_memc_notice( "[MEMC_BRING_UP][Scaler_MEMC_Initialize]\n");

//	MEMC_Lib_LoadBringUpScript_HDF();
	rtd_pr_memc_notice( "[MEMC_BRING_UP][MEMC_Lib_LoadBringUpScript_HDF]\n");

//	MEMC_Lib_LoadBringUpScript_PhaseTable();
	rtd_pr_memc_notice( "[MEMC_BRING_UP][MEMC_Lib_LoadBringUpScript_PhaseTable]\n");

//	MEMC_Lib_LoadBringUpScript_MemoryAddress();
	rtd_pr_memc_notice( "[MEMC_BRING_UP][MEMC_Lib_LoadBringUpScript_MemoryAddress]\n");

//	MEMC_Lib_LoadBringUpScript_cs_2x2_all_on();
	rtd_pr_memc_notice( "[MEMC_BRING_UP][MEMC_Lib_LoadBringUpScript_cs_2x2_all_on]\n");

//	MEMC_Lib_LoadBringUpScript_Enable_DMA();
	rtd_pr_memc_notice( "[MEMC_BRING_UP][MEMC_Lib_LoadBringUpScript_Enable_DMA]\n");
#else
	unsigned int Hsync_start = 0, Vsync_start = 0;
	PANEL_PARA_ST stPanelPara;
	unsigned short u16Idx = 0;

#if RTK_MEMC_Performance_tunging_from_tv001
	#if CONFIG_MEMC_BOUNDARY_AUTO
	unsigned int memc_size = 0;
	unsigned long memc_addr = 0;
	unsigned long memc_addr_aligned = 0;
	#endif
#endif
	rtd_pr_memc_emerg("[MEMC][%s]\n",__FUNCTION__);
	rtd_pr_memc_notice( "[MEMC]ME_start_addr = %x\n", ME_start_addr);
	rtd_pr_memc_notice( "[MEMC]MC00_start_addr = %x\n", MC00_start_addr);
	rtd_pr_memc_notice( "[MEMC]MC01_start_addr = %x\n", MC01_start_addr);
	rtd_pr_memc_notice( "[MEMC]KME_TOTAL_SIZE = %x\n", KME_TOTAL_SIZE);
	rtd_pr_memc_notice( "[MEMC]KMC_TOTAL_SIZE = %x\n", KMC_TOTAL_SIZE);
	if(RUN_MERLIN8P()){
		rtd_pr_memc_notice("[MEMC][db]=[,%x,%x,%x]\n",rtd_inl(M8P_RTHW_MC_DMA_MC_WDMA_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_DMA_MC_RDMA_DB_CTRL_reg),rtd_inl(M8P_RTHW_MC_PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg));
		rtd_pr_memc_notice("[MEMC][%x, %x, %x, %x, %x, %x]\n\r", rtd_inl(M8P_RTHW_MC_DMA_MC_LF_DMA_WR_Ctrl_reg), rtd_inl(M8P_RTHW_MC_DMA_MC_HF_DMA_WR_Ctrl_reg), rtd_inl(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg), rtd_inl(M8P_RTHW_MC_DMA_MC_HF_I_DMA_RD_Ctrl_reg), rtd_inl(M8P_RTHW_MC_DMA_MC_LF_P_DMA_RD_Ctrl_reg), rtd_inl(M8P_RTHW_MC_DMA_MC_HF_P_DMA_RD_Ctrl_reg)); 
	}else if(RUN_MERLIN8()){
		rtd_pr_memc_notice("[MEMC][db]=[,%x,%x,%x]\n",rtd_inl(MC_DMA_MC_WDMA_DB_CTRL_reg),rtd_inl(MC_DMA_MC_RDMA_DB_CTRL_reg),rtd_inl(PQC_PQDC_MC_PQ_DECMP_DB_CTRL_reg));
		rtd_pr_memc_notice("[MEMC][%x, %x, %x, %x, %x, %x]\n\r", rtd_inl(MC_DMA_MC_LF_DMA_WR_Ctrl_reg), rtd_inl(MC_DMA_MC_HF_DMA_WR_Ctrl_reg), rtd_inl(MC_DMA_MC_LF_I_DMA_RD_Ctrl_reg), rtd_inl(MC_DMA_MC_HF_I_DMA_RD_Ctrl_reg), rtd_inl(MC_DMA_MC_LF_P_DMA_RD_Ctrl_reg), rtd_inl(MC_DMA_MC_HF_P_DMA_RD_Ctrl_reg));	
	}//RUN_MERLIN8P & RUN_MERLIN8

	if(g_KERNEL_AFTER_QS_MEMC_FLG==false){
		Scaler_MEMC_DMA_DB_Check();
	}

#ifndef MEMC_INIT_STATUS
	Scaler_MEMC_SetInitFlag(1);
	//Scaler_MEMC_SetMEMC_Enable(0);
#else
	Scaler_MEMC_SetInitFlag(MEMC_INIT_ACON_ING);
#endif

	MEMC_LibInputVSInterruptEnable(FALSE);
	MEMC_LibOutputVSInterruptEnable(FALSE);
	MEMC_LibInputHSInterruptEnable(FALSE);
	MEMC_LibOutpuHSInterruptEnable(FALSE);	
	Vsync_start = (rtd_inl(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg) & 0xffff0000)>>16;
	Hsync_start = (rtd_inl(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg) & 0xffff0000)>>16;
	rtd_pr_memc_notice( "[MEMC]Vsync_start = %d\n",Vsync_start);
	rtd_pr_memc_notice( "[MEMC]Hsync_start = %d\n",Hsync_start);
	MEMC_LibPanel_Init();
	#if (IC_K6LP || IC_K8LP || IC_K24)
	stPanelPara.enOutputFrameRate = OUTPUT_60HZ;
	
#if RTK_MEMC_Performance_tunging_from_tv001
	stPanelPara.u32Htotal = Get_DISP_HORIZONTAL_TOTAL();
#else
	stPanelPara.u32Htotal = 4400;
#endif

	stPanelPara.u32HtotalMax = 4800;
	stPanelPara.u32HtotalMin = 4000;
	stPanelPara.u32Hactive = 3840;
	stPanelPara.u32HSWidth = 280;
	stPanelPara.u32HSBPorch = 168; //Hsync_start - stPanelPara.u32HSWidth;
	stPanelPara.u32Vtotal = 2250;
	stPanelPara.u32VtotalMax = 2500;
	stPanelPara.u32VtotalMin = 2200;
	stPanelPara.u32Vactive = 2160;
	stPanelPara.u32VSWidth = 8;
	stPanelPara.u32VSBPorch = 35; //Vsync_start - stPanelPara.u32VSWidth;
	#else
	stPanelPara.enOutputFrameRate = OUTPUT_60HZ;
#if RTK_MEMC_Performance_tunging_from_tv001
	stPanelPara.u32Htotal = Get_DISP_HORIZONTAL_TOTAL();
#else
	stPanelPara.u32Htotal = 4400;
#endif
	stPanelPara.u32HtotalMax = 4800;
	stPanelPara.u32HtotalMin = 4000;
	stPanelPara.u32Hactive = 3840;
	stPanelPara.u32HSWidth = 64;
	stPanelPara.u32HSBPorch = 168;//Hsync_start - stPanelPara.u32HSWidth;
	stPanelPara.u32Vtotal = 2250;
	stPanelPara.u32VtotalMax = 2300;
	stPanelPara.u32VtotalMin = 2200;
	stPanelPara.u32Vactive = 2160;
	stPanelPara.u32VSWidth = 8;
	stPanelPara.u32VSBPorch = Vsync_start - stPanelPara.u32VSWidth;
	#endif
	for(u16Idx = 0; u16Idx < OUTPUT_FRAME_MAX; u16Idx++){
		stPanelPara.enOutputFrameRate = u16Idx;
		MEMC_LibSetPanelParameters(&stPanelPara);
	}
	MEMC_LibDMAllocate(MC00_start_addr, MC01_start_addr, ME_start_addr);

	if(g_KERNEL_AFTER_QS_MEMC_FLG==false){
#if 0//RTK_MEMC_Performance_tunging_from_tv001
#if CONFIG_MEMC_BOUNDARY_AUTO
	//set mc boundary with memc max memory size
	memc_size = Scaler_MEMC_Get_Size_Addr(&memc_addr_aligned, &memc_addr);
	if(memc_size==0){
		rtd_pr_memc_info("Fail to set MC boundary for memc_size = 0.\r\n");
	}
	else {
		Scaler_MEMC_Set_MC_MEMBoundary(memc_addr, memc_addr+memc_size);
	}
#else

	if(Scaler_MEMC_GetPanelSizeByDisp() == MEMC_SIZE_2K){//if(Get_DISPLAY_PANEL_TYPE() == P_LVDS_2K1K){
		Scaler_MEMC_Set_ME_MEMBoundary(ME_start_addr, (ME_start_addr+KME_TOTAL_SIZE/*KME_TOTAL_SIZE_8K*/));
		#if 0
		#if CONFIG_MC_8_BUFFER
		if(Get_DISPLAY_REFRESH_RATE() <= 60){
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_8bit/*KMC_TOTAL_SIZE_8buf_8bit_8K*/));	
		}else{
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_10bit/*KMC_TOTAL_SIZE_8buf_10bit_8K*/));	
		}
		#else
		Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_6buf_10bit/*KMC_TOTAL_SIZE_6buf_10bit_8K*/));
		#endif
		#else
		Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + Get_KMC_TOTAL_SIZE()));
		#endif
	}else{
		Scaler_MEMC_Set_ME_MEMBoundary(ME_start_addr, (ME_start_addr+KME_TOTAL_SIZE));
		#if 0
		#if CONFIG_MC_8_BUFFER
		if(Get_DISPLAY_REFRESH_RATE() <= 60){
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_8bit));	
		}else{
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_10bit));	
		}
		#else
		Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_6buf_10bit));	
		#endif
		#else
		Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + Get_KMC_TOTAL_SIZE()));
		#endif
	}
#endif
#else
	if(carvedout_enable()==FALSE && get_reset_MEMC_flg()==true){
		//Scaler_MEMC_allocate_memory();
	}
	else{
		if(Get_DISPLAY_PANEL_TYPE() == P_LVDS_2K1K)
		{
			Scaler_MEMC_Set_ME_MEMBoundary(ME_start_addr, (ME_start_addr+KME_TOTAL_SIZE/*KME_TOTAL_SIZE_8K*/));
			#if 0
			#if CONFIG_MC_8_BUFFER
			if(Get_DISPLAY_REFRESH_RATE() <= 60){
				Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_8bit/*KMC_TOTAL_SIZE_8buf_8bit_8K*/));	
			}else{
				Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_10bit/*KMC_TOTAL_SIZE_8buf_10bit_8K*/));	
			}
			#else
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_6buf_10bit/*KMC_TOTAL_SIZE_6buf_10bit_8K*/));
			#endif
			#else
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + Get_KMC_TOTAL_SIZE()));
			#endif
		}else{
			Scaler_MEMC_Set_ME_MEMBoundary(ME_start_addr, (ME_start_addr+KME_TOTAL_SIZE));
			#if 0
			#if CONFIG_MC_8_BUFFER
			if(Get_DISPLAY_REFRESH_RATE() <= 60){
				Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_8bit));	
			}else{
				Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_8buf_10bit));	
			}
			#else
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + KMC_TOTAL_SIZE_6buf_10bit));	
			#endif
			#else
			Scaler_MEMC_Set_MC_MEMBoundary(MC00_start_addr,(MC00_start_addr + Get_KMC_TOTAL_SIZE()));
			#endif
		}
	}


#endif	
	}

	MEMC_LibInit();
	//MEMC_LibSetInOutFrameRate(INPUT_60HZ,OUTPUT_60HZ);
	//MEMC_LibSetInOutMode(MEMC_RGB_IN_VIDEO_OUT,TRUE);
	//MEMC_LibSetInOutUseCase(INPUT_3840X2160, OUTPUT_3840X2160, INPUT_2D, OUTPUT_2D);
	/* Enable in/out Vsync interrupt */
	
#ifndef BUILD_QUICK_SHOW
	MEMC_LibInputVSInterruptEnable(TRUE);
	MEMC_LibOutputVSInterruptEnable(TRUE); //YE Test LBMC

	MEMC_LibOutpuHSInterruptEnable(TRUE); //YE Test LBMC
	Scaler_MEMC_SetMEMC_Enable(1);
	rtd_pr_memc_notice( "MEMC ISR Enable .\r\n");
#else
	rtd_pr_memc_notice( "MEMC ISR Disable in QS .\r\n");
#endif


#ifdef MEMC_INIT_STATUS
	Scaler_MEMC_SetInitFlag(MEMC_INIT_ACON_FINISH);
#endif
	rtd_pr_memc_notice( "MEMC initialize done. IOvs Intp Enable.\r\n");
#endif

	if(get_reset_MEMC_flg()==true){
		set_reset_MEMC_flg(false);
	}

}

void Scaler_MEMC_Uninitialize(void){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}

unsigned char Scaler_MEMC_Dejudder_Level_Mapping(unsigned char inLevel)
{
	unsigned char outLevel = 0;

#if RTK_MEMC_Performance_tunging_from_tv001//xj//m7 uiä¸è¨­å®æ¯?é?gVIP_Table ?©é?DejudderLevel,DeblurLevel?ä?å¾®å·®??
	unsigned char dejudder_Num = 0;
	SLR_VIP_TABLE *gVip_Table = fwif_colo_get_AP_vip_table_gVIP_Table();

	if( gVip_Table == NULL ) {
		return 0;
	}

	dejudder_Num = gVip_Table->MEMC_PARAMS.JudderTable[0];
	if(inLevel > (dejudder_Num-1))
		inLevel = dejudder_Num-1;

	outLevel = gVip_Table->MEMC_PARAMS.JudderTable[inLevel+1];
	rtd_pr_memc_info("[MEMC][%s] inLevel = %d, outLevel = %d\n", __FUNCTION__, inLevel, outLevel);
#else
	unsigned char level_map[11] = { 0, 14, 28, 42, 56, 70, 84, 96, 107, 118, 128 }; //updated 20211013 by tv006 Ahn
	//{0, 14, 28, 41, 55, 68, 82, 95, 106, 117, 128};	//confirmed by tv006 Ann


	if(inLevel > 10)
		inLevel = 10;

	outLevel = level_map[inLevel];
#endif

	

	return outLevel;
}

unsigned char Scaler_MEMC_Deblur_Level_Mapping(unsigned char inLevel)
{
	unsigned char outLevel = 0;

#if RTK_MEMC_Performance_tunging_from_tv001//xj//m7 uiä¸è¨­å®æ¯?é?gVIP_Table ?©é?DejudderLevel,DeblurLevel?ä?å¾®å·®??
	unsigned char deblur_Num = 0;
	SLR_VIP_TABLE *gVip_Table = fwif_colo_get_AP_vip_table_gVIP_Table();

	if( gVip_Table == NULL ) {
		return 0;
	}

	deblur_Num = gVip_Table->MEMC_PARAMS.BlurTable[0];
	if(inLevel > (deblur_Num-1))
		inLevel = deblur_Num-1;

	outLevel = gVip_Table->MEMC_PARAMS.BlurTable[inLevel+1];

	rtd_pr_memc_info("[MEMC][%s] inLevel = %d, outLevel = %d\n", __FUNCTION__, inLevel, outLevel);
#else
	unsigned char level_map[11] = {0, 14, 28, 41, 55, 68, 82, 95, 106, 117, 128};	//confirmed by tv006 Ann


	if(inLevel > 10)
		inLevel = 10;

	outLevel = level_map[inLevel];
#endif




	return outLevel;
}

extern unsigned char u8_MEMCMode;
extern unsigned int dejudder;
extern unsigned int deblur;
extern unsigned char g_power_saving_flag;
unsigned char g_motion = 0;
extern bool g_NewLowDelay_en;
extern void Set_SetUp_NewGameMode_Flag(bool enable);
int SetMotionComp_return_by_lowdelay = 0;
extern unsigned char VR360_en;

MEMC_MODE Scaler_MEMC_MEMCTypeMapping(SCALER_MEMC_TYPE_T motion)
{
	MEMC_MODE ret = 0;

	if(motion == 0/*VPQ_MEMC_TYPE_OFF*/){ // off
		//OFF memc
		ret = MEMC_OFF;
	}
	else if(motion == 1/*VPQ_MEMC_TYPE_LOW*/){ // natural
		//Set low memc
		ret = MEMC_CLEAR;
	}
	else if(motion == 2/*VPQ_MEMC_TYPE_HIGH*/){ // smooth
		//Set high memc
		ret = MEMC_SMOOTH;
	}
	else if(motion == 3/*VPQ_MEMC_TYPE_NORMAL*/){
		//Set high memc
		ret = MEMC_NORMAL;
	}
	else if(motion == 4/*VPQ_MEMC_TYPE_USER*/){ // user
		//Set user memc
		ret = MEMC_USER;
	}
	else if(motion == 5/*VPQ_MEMC_TYPE_55_PULLDOWN*/){
		#if 0 // tv003
		ret = MEMC_NORMAL;
		#else
		ret = MEMC_OFF;
		#endif
	}
	else if(motion == 6/*VPQ_MEMC_TYPE_MEDIUM*/){ // cinema clear
		//Set high memc
		ret = MEMC_MEDIUM;
	}
	else{
		//OFF memc
		ret = MEMC_OFF;
	}

	return ret;
}

void Scaler_MEMC_SetMotionComp(unsigned char blurLevel, unsigned char judderLevel, SCALER_MEMC_TYPE_T motion){
	_RPC_clues* RPC_SmartPic_clue = (_RPC_clues *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_SMARTPIC_CLUS);
	unsigned char ME_clock_state = 0, MEMC_clock_state = 0;
	static unsigned char First_time_flag = 1;
	//MEMC_MODE memc_type = MEMC_OFF; 
	unsigned int u8_tmp=0;
#if RTK_MEMC_Performance_tunging_from_tv001 //newgamemode_config
	static unsigned char u8_motion_pre = 0;
#endif

	//unsigned int test=0;

	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	rtd_pr_memc_notice( "[MEMC]blurLevel = %d, judderLevel = %d, motion = %d\n",blurLevel, judderLevel, motion);
	//if(MEMC_LibGetMEMC_PCModeEnable() == TRUE){
	//	return;
	//}
	MEMC_clock_state = (rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>31)&0x1;
	ME_clock_state = (rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>30)&0x1;
	if(MEMC_clock_state == 0){
		rtd_pr_memc_notice("[%s][MEMC_clock_state is off !!! return !!!]\n", __FUNCTION__);
		return;
	}

#if RTK_MEMC_Performance_tunging_from_tv001 //newgamemode_config
	if(drv_memory_get_game_mode_dynamic()==TRUE){
		if((u8_motion_pre==0 && motion!=0) || (u8_motion_pre!=0 && motion==0)) {
			Set_SetUp_NewGameMode_Flag(true);
		}
	}
	u8_motion_pre = motion;
#endif

	if(RPC_SmartPic_clue == NULL || (Scaler_MEMC_GetMEMC_Enable() == 0))
		return;

	g_motion = motion;
	//rtd_pr_memc_notice( "[MEMC]LowDelay_mode = %d, vdec_direct = %d, motion = %d\n",LowDelay_mode, drv_memory_get_vdec_direct_low_latency_mode(SLR_MAIN_DISPLAY), MEMC_LibGetMEMCLowDelayModeEnable());


#if RTK_MEMC_Performance_tunging_from_tv001
	//merlin8/8p mdomain one-bin
	//if(drv_memory_get_vdec_direct_low_latency_mode()== TRUE && drv_memory_get_game_mode_dynamic()==0)/*MEMC_LibGetMEMCLowDelayModeEnable() == TRUE*/ /*&& RPC_SmartPic_clue->MEMC_VCPU_setting_info.m_MiraCast == 1*/
	if((get_mach_type() == MACH_ARCH_RTK2885P && drv_memory_get_vdec_direct_low_latency_mode()== TRUE) && drv_memory_get_game_mode_dynamic()==0)/*MEMC_LibGetMEMCLowDelayModeEnable() == TRUE*/ /*&& RPC_SmartPic_clue->MEMC_VCPU_setting_info.m_MiraCast == 1*/
#else	
	//merlin8/8p mdomain one-bin
	//if((drv_memory_get_vdec_direct_low_latency_mode()== TRUE)/*MEMC_LibGetMEMCLowDelayModeEnable() == TRUE*/ /*&& RPC_SmartPic_clue->MEMC_VCPU_setting_info.m_MiraCast == 1*/)
	if((get_mach_type() == MACH_ARCH_RTK2885P && nonlibdma_drv_memory_get_vdec_direct_low_latency_mode()== TRUE)/*MEMC_LibGetMEMCLowDelayModeEnable() == TRUE*/ /*&& RPC_SmartPic_clue->MEMC_VCPU_setting_info.m_MiraCast == 1*/)
#endif	
	{
		//merlin8/8p mdomain one-bin
		//rtd_pr_memc_notice("[MEMC]LowDelay_mode = %d, vdec_direct = %d, motion = %d\n",LowDelay_mode, drv_memory_get_vdec_direct_low_latency_mode(), MEMC_LibGetMEMCLowDelayModeEnable());
		rtd_pr_memc_notice("[MEMC]LowDelay_mode = %d, vdec_direct = %d, motion = %d\n",LowDelay_mode, nonlibdma_drv_memory_get_vdec_direct_low_latency_mode(), MEMC_LibGetMEMCLowDelayModeEnable());
		SetMotionComp_return_by_lowdelay = 1;		
		rtd_pr_memc_notice( "[MEMC]LowDelay = %d, MiraCast = %d\n", MEMC_LibGetMEMCLowDelayModeEnable(), RPC_SmartPic_clue->MEMC_VCPU_setting_info.m_MiraCast);
		return;
	}


	//re-mapping
	if(motion == 0/*VPQ_MEMC_TYPE_OFF*/){ // off
		motion = MEMC_TYPE_OFF;
	}
	else if(motion == 1/*VPQ_MEMC_TYPE_LOW*/){ // natural
		motion = MEMC_TYPE_LOW;
	}
	else if(motion == 2/*VPQ_MEMC_TYPE_HIGH*/){ // smooth
		motion = MEMC_TYPE_HIGH;
	}
	else if(motion == 4/*VPQ_MEMC_TYPE_USER*/){ // user
		motion = MEMC_TYPE_USER;
	}
	else if(motion == 5/*VPQ_MEMC_TYPE_55_PULLDOWN*/){
		motion = MEMC_TYPE_55_PULLDOWN;
	}
	else if(motion == 6/*VPQ_MEMC_TYPE_MEDIUM*/){ // cinema clear
		motion = MEMC_TYPE_MEDIUM;
	}
	else if(motion == 3/*VPQ_MEMC_TYPE_NORMAL*/){
		motion = MEMC_TYPE_NORMAL;
	}
	else{
		motion = MEMC_TYPE_OFF;
	}


	//eadRegister(0xB809D9E4, 31, 31, &test);

	/*if(test){
		motion = MEMC_TYPE_55_PULLDOWN;
		rtd_pr_memc_emerg( "MEMC_TYPE_55_PULLDOWN\n");
	}*/

#if 0 // def MEMC_BRING_UP
	motion = MEMC_TYPE_HIGH;
#endif

	if(motion>=MEMC_TYPE_MAX)		//fix me
		motion = MEMC_TYPE_OFF;

	u8_MEMCMode = motion;
	if(motion == MEMC_TYPE_OFF){
		//OFF memc
		MEMC_LibSetMEMCMode(MEMC_OFF);

		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(1, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
		}
	}
	else if(motion == MEMC_TYPE_LOW){
		//Set low memc
		MEMC_LibSetMEMCMode(MEMC_CLEAR);

		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(0, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
		}
	}
	else if(motion == MEMC_TYPE_HIGH){
		//Set high memc
		MEMC_LibSetMEMCMode(MEMC_SMOOTH);

		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(0, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
		}
	}
	else if(motion == MEMC_TYPE_NORMAL){
		//Set high memc
		MEMC_LibSetMEMCMode(MEMC_NORMAL);

		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(0, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
		}
	}
	else if(motion == MEMC_TYPE_USER){
		//Set user memc
		MEMC_LibSetMEMCMode(MEMC_USER);
		if(g_new_sport_mode){
			//Set blurLevel for Video
			ReadRegister(SOFTWARE2_SOFTWARE2_40_reg, 0,  7, &u8_tmp);
			blurLevel = (u8_tmp<128) ? u8_tmp : 128;
			deblur = blurLevel;
			//Set JudderLevel for Film
			ReadRegister(SOFTWARE2_SOFTWARE2_40_reg, 8, 15, &u8_tmp);
			judderLevel = (u8_tmp<128) ? u8_tmp : 128;
			dejudder = judderLevel;
		}
		else {
			//Set blurLevel for Video
			blurLevel = Scaler_MEMC_Deblur_Level_Mapping(blurLevel);
			deblur = blurLevel;
			MEMC_LibSetMEMCDeblurLevel(blurLevel);
			//Set JudderLevel for Film
			judderLevel = Scaler_MEMC_Dejudder_Level_Mapping(judderLevel);
			dejudder = judderLevel;	
		}
		MEMC_LibSetMEMCDejudderLevel(judderLevel);
		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(0, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
		}
	}
	else if(motion == MEMC_TYPE_55_PULLDOWN){
 
#if RTK_MEMC_Performance_tunging_from_tv001//xj//m7 uiä¸è¨­å®æ¯?é?gVIP_Table ?©é?DejudderLevel,DeblurLevel?ä?å¾®å·®??
		SLR_VIP_TABLE *gVip_Table = fwif_colo_get_AP_vip_table_gVIP_Table();
		unsigned char ModeInCinema = gVip_Table->MEMC_PARAMS.ModeInCinema;
		MEMC_LibSetMEMCMode(ModeInCinema);
		//MEMC_LibSetMEMCMode(MEMC_MEDIUM);

#endif
		#if Pro_tv030
		MEMC_LibSetMEMCMode(MEMC_MEDIUM);
		//MEMC_LibSetMEMCMode(MEMC_NORMAL);

		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
#if RTK_MEMC_Performance_tunging_from_tv001//xj//m7 uiä¸è¨­å®æ¯?é?gVIP_Table ?©é?DejudderLevel,DeblurLevel?ä?å¾®å·®??
			BOOL bRepeat_en = (ModeInCinema==MEMC_OFF) ? 1 : 0;
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(bRepeat_en, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(bRepeat_en, 0);			
#else
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(0, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
#endif

	
		}
		#else
		MEMC_LibSetMEMCMode(MEMC_OFF);
		
		//On 5:5 Pull-down for 24Hz Film source
		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(1, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
		}
		#endif
	}
	else if(motion == MEMC_TYPE_MEDIUM){
		//Set high memc
		MEMC_LibSetMEMCMode(MEMC_MEDIUM);

		//if(RPC_SmartPic_clue->prePowerSaveStatus == PS_ON)
		if(ME_clock_state == 0)
		{
			rtd_pr_memc_notice( "[RepeatMode] Power saving mode on, return.\n");
		}
		else
		{
			if(First_time_flag || g_power_saving_flag)
				MEMC_LibSetMEMCFrameRepeatEnable(0, 1);
			else
				MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
		}
	}

	if(First_time_flag == 1){
		First_time_flag = 0;
	}
	if(g_power_saving_flag == 1){
		g_power_saving_flag = 0;
	}
}

void Scaler_MEMC_SetBlurLevel(unsigned char blurLevel){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	rtd_pr_memc_notice( "[MEMC]blurLevel = %d\n",blurLevel);
	//Set blurLevel for Video

	if(fwif_VIP_get_Project_ID() == VIP_Project_ID_TV006)//blurLevel = 0~10
		blurLevel = Scaler_MEMC_Deblur_Level_Mapping(blurLevel);

	MEMC_LibSetMEMCDeblurLevel(blurLevel);//driver range : 0~128
}

void Scaler_MEMC_SetJudderLevel(unsigned char judderLevel){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	rtd_pr_memc_notice( "[MEMC]judderLevel = %d\n",judderLevel);
	//Set JudderLevel for Film
	if(fwif_VIP_get_Project_ID() == VIP_Project_ID_TV006)//judderLevel = 0~10
		judderLevel = Scaler_MEMC_Dejudder_Level_Mapping(judderLevel);

	MEMC_LibSetMEMCDejudderLevel(judderLevel);//driver range : 0~128
}

void Scaler_MEMC_MotionCompOnOff(unsigned char bOnOff){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	rtd_pr_memc_notice( "[MEMC]bOnOff = %d\n",bOnOff);

	if(bOnOff == TRUE){
		MEMC_LibSetMEMCMode(MEMC_NORMAL);
		MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
	}
	else{
		MEMC_LibSetMEMCMode(MEMC_OFF);
		MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
	}
}


unsigned char Scaler_get_MEMC_LowDelay_disable_condition(void){
	if(/*MEMC_LibGetMEMC_PCModeEnable() ||*/ VR360_en || (Scaler_MEMC_GetMEMC_Enable() == 0) || (Get_DISPLAY_PANEL_TYPE()== P_VBY1_TO_HDMI_MEMC_FREERUN)){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

void Scaler_MEMC_LowDelayMode(unsigned char type){
	static MEMC_MODE last_mode = MEMC_OFF;
	static unsigned char pre_type = 255;
	unsigned char MEMC_clock_state = 0;
	static unsigned int last_lbmc_neg_vtrig_dly = 0;

	MEMC_clock_state = (rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>31)&0x1;
#if RTK_MEMC_Performance_tunging_from_tv001 //newgamemode_config
	if(MEMC_clock_state == 0 && g_NewLowDelay_en==0){
#else
	if(MEMC_clock_state == 0){
#endif
		rtd_pr_memc_notice("[%s][MEMC_clock_state is off !!! return !!!]\n", __FUNCTION__);
		return;
	}

	rtd_pr_memc_notice( "[MEMC][%s][AA][,%d,][,%d,%d,%d,%d,][g_motion,%d,]\n",__FUNCTION__, type, MEMC_LibGetMEMCLowDelayModeEnable(), MEMC_LibGetMEMC_PCModeEnable(), VR360_en, Scaler_MEMC_GetMEMC_Enable(), g_motion);
//	rtd_pr_memc_notice( "$$$   scalerMEMC_LowDelay_(%d)   $$$\n\r", type);
	LowDelay_mode = type;

#if RTK_MEMC_Performance_tunging_from_tv001 //newgamemode_config
	if(type == MEMC_LibGetMEMCLowDelayModeEnable() && g_NewLowDelay_en==0)
#else
	if(type == MEMC_LibGetMEMCLowDelayModeEnable())
#endif
		return;

#if RTK_MEMC_Performance_tunging_from_tv001 //newgamemode_config
	if(Scaler_get_MEMC_LowDelay_disable_condition() && g_NewLowDelay_en==0){
#else
	if(Scaler_get_MEMC_LowDelay_disable_condition()){
#endif
		LowDelay_mode = 0;
		MEMC_LibSetMEMCLowDelayModeEnable(FALSE);
		return;
	}


	//if(VR360_en)
	//	return;

	//rtd_pr_memc_notice( "[MEMC]vdec_direct = %d\n", drv_memory_get_vdec_direct_low_latency_mode());

#if RTK_MEMC_Performance_tunging_from_tv001 //newgamemode_config
	if(g_NewLowDelay_en && type == 1){
		rtd_pr_memc_notice("[MEMC][%s][GG][%d]\n",__FUNCTION__,u8_MEMCMode);
		if(u8_MEMCMode!=0){ //new game mode
			g_NewLowDelay_en = true;
		}else {
			g_NewLowDelay_en = false;
		}
		MEMC_LibSetMEMCLowDelayModeEnable(TRUE);
	}
	else if(type == 1){
		if(u8_MEMCMode!=0){ //new game mode
			g_NewLowDelay_en = true;
		}else {
			g_NewLowDelay_en = false;
		}
		//Reduce frame buffer for low frame delay
		//if(MEMC_LibGetMEMC_PCModeEnable() == FALSE)	//PC mode first
		//{
		//	MEMC_LibSetMEMCMode(MEMC_OFF);
		//	MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
		//}
		rtd_pr_memc_notice( "[MEMC][%s][CC][%d,%d]\n",__FUNCTION__, pre_type,type);
		if(type != pre_type){
			rtd_pr_memc_notice( "[MEMC][%s][DD][%d,%d,%d]\n",__FUNCTION__, pre_type,last_mode,MEMC_LibGetMEMCMode());
			last_mode = Scaler_MEMC_MEMCTypeMapping(g_motion); // MEMC_LibGetMEMCMode();
			MEMC_LibSetMEMCMode(MEMC_OFF);
			MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
		}
		MEMC_LibSetMEMCLowDelayModeEnable(TRUE);
		Mid_CRTC_Set_LBMC_neg_en(TRUE);
		last_lbmc_neg_vtrig_dly = Mid_CRTC_Get_LBMC_neg_vtrig_dly();
		Mid_CRTC_Set_LBMC_neg_vtrig_dly(6);
		Mid_CRTC_Set_DMA_Raw_PCMode_en(TRUE);
	}
#else
	if(type == 1){
		//Reduce frame buffer for low frame delay
		//if(MEMC_LibGetMEMC_PCModeEnable() == FALSE)	//PC mode first
		//{
		//	MEMC_LibSetMEMCMode(MEMC_OFF);
		//	MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
		//}
		rtd_pr_memc_notice( "[MEMC][%s][CC][%d,%d]\n",__FUNCTION__, pre_type,type);
		if(type != pre_type){
			rtd_pr_memc_notice( "[MEMC][%s][DD][%d,%d,%d]\n",__FUNCTION__, pre_type,last_mode,MEMC_LibGetMEMCMode());
			last_mode = Scaler_MEMC_MEMCTypeMapping(g_motion); // MEMC_LibGetMEMCMode();
			MEMC_LibSetMEMCMode(MEMC_OFF);
			MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
		}
		MEMC_LibSetMEMCLowDelayModeEnable(TRUE);
		Mid_CRTC_Set_LBMC_neg_en(TRUE);
		last_lbmc_neg_vtrig_dly = Mid_CRTC_Get_LBMC_neg_vtrig_dly();
		Mid_CRTC_Set_LBMC_neg_vtrig_dly(6);
		Mid_CRTC_Set_DMA_Raw_PCMode_en(TRUE);
	}
#endif
	else{
		//Normal mode
		//MEMC_LibSetMEMCMode(MEMC_NORMAL);
		//MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
		rtd_pr_memc_notice( "[MEMC][%s][EE][%d,%d,%d]\n",__FUNCTION__,pre_type,last_mode,MEMC_LibGetMEMCMode());
		if(last_mode != Scaler_MEMC_MEMCTypeMapping(g_motion)){ // Q16730
			rtd_pr_memc_notice( "[MEMC][%s][last_mode change !!][last_mode,%d,][current mode,%d,]\n",__FUNCTION__, last_mode, Scaler_MEMC_MEMCTypeMapping(u8_MEMCMode));
			last_mode = Scaler_MEMC_MEMCTypeMapping(g_motion);
		}
		if(last_mode != MEMC_OFF){
			rtd_pr_memc_notice( "[MEMC][%s][FF][%d,%d]\n",__FUNCTION__,pre_type,last_mode);
			if(VR360_en){
				MEMC_LibSetMEMCMode(MEMC_OFF);
				MEMC_LibSetMEMCFrameRepeatEnable(1, 0);
			}else{
				MEMC_LibSetMEMCMode(last_mode);
				MEMC_LibSetMEMCFrameRepeatEnable(0, 0);
			}
		}
		MEMC_LibSetMEMCLowDelayModeEnable(FALSE);
		Mid_CRTC_Set_LBMC_neg_en(FALSE);
		Mid_CRTC_Set_LBMC_neg_vtrig_dly(last_lbmc_neg_vtrig_dly);
		Mid_CRTC_Set_DMA_Raw_PCMode_en(FALSE);
	}
}

void Scaler_MEMC_SetRGBYUVMode(unsigned char mode){
	sys_reg_sys_srst3_RBUS sys_reg_sys_srst3_reg;
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	sys_reg_sys_srst3_reg.regValue = IoReg_Read32(SYS_REG_SYS_SRST3_reg);
	sys_reg_sys_clken3_reg.regValue = IoReg_Read32(SYS_REG_SYS_CLKEN3_reg);
	if((sys_reg_sys_srst3_reg.rstn_memc_mc == 0) || (sys_reg_sys_clken3_reg.clken_memc_mc == 0) || (MEMC_Lib_get_memc_PowerSaving_Mode() == 1)){
		return;
	}	
	if(MEMC_Lib_Get_QS_PCmode_Flg()){ //quick show pc mode
		MEMC_LibSetMEMC_PCModeEnable(TRUE);
		return;
	}
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	rtd_pr_memc_notice( "[MEMC]mode = %d\n",mode);

	switch(mode)
	{
		case 0:
			//MEMC_LibSetInOutMode(MEMC_RGB_IN_VIDEO_OUT, FALSE);
			MEMC_LibSetMEMC_PCModeEnable(FALSE);
			break;
		case 1:
			//MEMC_LibSetInOutMode(MEMC_RGB_IN_PC_OUT, FALSE);
			MEMC_LibSetMEMC_PCModeEnable(TRUE);
			break;
#if 0
		case 2:
			MEMC_LibSetInOutMode(MEMC_YUV_IN_VIDEO_OUT, FALSE);
			break;
		case 3:
			MEMC_LibSetInOutMode(MEMC_YUV_IN_PC_OUT, FALSE);
			break;
#endif
		default:
			if(Get_DISPLAY_PANEL_TYPE() == P_VBY1_TO_HDMI_MEMC_FREERUN){
				MEMC_LibSetInOutMode(MEMC_RGB_IN_VIDEO_FREERUN_OUT, FALSE);
			}else{
				MEMC_LibSetInOutMode(MEMC_RGB_IN_VIDEO_OUT, FALSE);
			}
			MEMC_LibSetMEMC_PCModeEnable(FALSE);
			break;
 	}
}

void Scaler_MEMC_GetFrameDelay (unsigned short *pFrameDelay){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}
void Scaler_MEMC_SetVideoBlock(SCALER_MEMC_MUTE_TYPE_T type, unsigned char bOnOff){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}

void Scaler_MEMC_SetTrueMotionDemo(unsigned char type){
	rtd_pr_memc_notice("[MEMC][%s]\n",__FUNCTION__);
  
	if(type==1){
		MEMC_LibSetMEMCDemoMode(DEMO_MODE_LEFT);
	}
	else if(type==2){
		MEMC_LibSetMEMCDemoMode(DEMO_MODE_RIGHT);
	}
	else {
		MEMC_LibSetMEMCDemoMode(DEMO_MODE_OFF);
	}
	
}

void Scaler_MEMC_GetFirmwareVersion(unsigned short *pVersion){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}
void Scaler_MEMC_SetBypassRegion(unsigned char bOnOff, SCALER_MEMC_BYPASS_REGION_T region, unsigned int x, unsigned int y, unsigned int w, unsigned int h){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);

	rtd_pr_memc_notice( "[MEMC]region = %d\n",region);
	rtd_pr_memc_notice( "[MEMC]rect.x = %d, rect.y = %d, rect.w = %d, rect.h = %d\n",x, y, w, h);

	rtd_pr_memc_notice( "[MEMC]bOnOff = %d\n",bOnOff);

	switch(region){
		case BYPASS_REGION_0:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_0, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_0, bOnOff);
			break;

		case BYPASS_REGION_1:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_1, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_1, bOnOff);
			break;

		case BYPASS_REGION_2:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_2, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_2, bOnOff);
			break;

		case BYPASS_REGION_3:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_3, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_3, bOnOff);
			break;

		case BYPASS_REGION_4:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_4, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_4, bOnOff);
			break;
		
		case BYPASS_REGION_5:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_5, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_5, bOnOff);
			break;
		
		case BYPASS_REGION_6:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_6, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_6, bOnOff);
			break;
		
		case BYPASS_REGION_7:
			MEMC_LibSetDemoWindowSettings(BYPASS_REGION_7, x, w, y, h);
			MEMC_LibSetDemoWindowEnable(BYPASS_REGION_7, bOnOff);
			break;

		default:
			break;
	}
}
void Scaler_MEMC_SetReverseControl(unsigned char u8Mode){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}
void Scaler_MEMC_Freeze(unsigned char type){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}
void Scaler_MEMC_SetDemoBar(unsigned char bOnOff, unsigned char r, unsigned char g, unsigned char b){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}
void Scaler_MEMC_DEBUG(void){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
}

void Scaler_MEMC_SetInOutputUseCase(unsigned char input_re, unsigned char output_re, unsigned char input_for, unsigned char output_for){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	MEMC_LibSetInOutUseCase(input_re, output_re, input_for, output_for);
}

void Scaler_MEMC_SetInputOutputFormat(unsigned char input_for, unsigned char output_for){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	//MEMC_LibSetInputOutputFormat(input_for, output_for);
}

void Scaler_MEMC_SetInputOutputResolution(unsigned char input_re, unsigned char output_re){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	//MEMC_LibSetInputOutputResolution(input_re, output_re);
}

void Scaler_MEMC_SetInputFrameRate(unsigned char frame_rate){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	MEMC_LibSetInputFrameRate(frame_rate);
}

void Scaler_MEMC_SetOutputFrameRate(unsigned char frame_rate){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	MEMC_LibSetOutputFrameRate(frame_rate);
}

void Scaler_MEMC_SetMEMCFrameRepeatEnable(unsigned char enable){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	MEMC_LibSetMEMCFrameRepeatEnable(enable, 0);
}

unsigned char g_input_mode = 0;
void ISR_Scaler_MEMC_SetInOutMode(unsigned char emom){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	MEMC_LibSetInOutMode(emom, FALSE);
	g_input_mode = emom;
}

unsigned char ISR_Scaler_MEMC_GetInOutMode(void){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	return g_input_mode;
}
unsigned char g_JP_DTV_4K_flag = 0;
void MEMC_Lib_Set_JP_DTV_4K_flag(unsigned char num)
{
	g_JP_DTV_4K_flag = num;
}
unsigned char MEMC_Lib_Get_JP_DTV_4K_flag(void)
{
	return g_JP_DTV_4K_flag;
}

void Scaler_MEMC_SetInstantBootInitPhaseTable(void){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);

	//enable MEMC input/output clock before run MEMC isr
	rtd_outl(SYS_REG_SYS_DISPCLKSEL_reg,rtd_inl(SYS_REG_SYS_DISPCLKSEL_reg)|( BIT_5 | BIT_6));

	MEMC_LibForcePhaseTableInit();
}

void Scaler_MEMC_SetMEMC_Enable(unsigned char enable){
	scaler_memc_enable = enable;
}

void Scaler_MEMC_SetCinemaMode(unsigned char enable){
	rtd_pr_memc_notice( "[MEMC][%s][%d]\n",__FUNCTION__, enable);

	MEMC_LibSetCinemaMode(enable);
}

unsigned char Scaler_MEMC_GetMEMC_Enable(void){
	return scaler_memc_enable;
}

unsigned char Scaler_MEMC_GetMEMC_Mode(void){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);
	return MEMC_LibGetMEMCMode();
}

BOOL Scaler_MEMC_SetPowerSaveOn(void){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);

	return MEMC_LibPowerSaveOn();
}

BOOL Scaler_MEMC_SetPowerSaveOff(void){
	rtd_pr_memc_notice( "[MEMC][%s]\n",__FUNCTION__);

	return MEMC_LibPowerSaveOff();
}

extern unsigned int time_1_RTK2885pp, time_2_RTK2885pp, time_3_RTK2885pp, time_4_RTK2885pp, time_5_RTK2885pp, time_6_RTK2885pp;
unsigned int Scaler_MEMC_ISR_InputVSInterrupt(void){	

	if(RUN_MERLIN8P()){
		ReadRegister(TIME_MEASURE_REG, 0, 12, &time_2_RTK2885pp);
		ReadRegister(TIME_MEASURE_REG, 16, 28, &time_1_RTK2885pp);
	}//RUN_MERLIN8P & RUN_MERLIN8

	MEMC_LibInputVSInterrupt();

	if(RUN_MERLIN8P()){
		ReadRegister(TIME_MEASURE_REG, 0, 12, &time_4_RTK2885pp);
		ReadRegister(TIME_MEASURE_REG, 16, 28, &time_3_RTK2885pp);
	}//RUN_MERLIN8P & RUN_MERLIN8
	return 0;
}


unsigned int Scaler_MEMC_ISR_OutputVSInterrupt(void){

	MEMC_LibOutputVSInterrupt();
	return 0;
}

#ifndef BUILD_QUICK_SHOW
#if 1//RTK_MEMC_Performance_tunging_from_tv001
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
void memc_TasksHandler_task( struct tasklet_struct *data )
#else
void memc_TasksHandler_task( unsigned long data )
#endif
{
	if(MEMC_GetInOutISREnable() && ((g_memc_hdmi_switch_state == 0) && (HDMI_PowerSaving_stage == 0) && (g_memc_switch_state == 0) && (DTV_PowerSaving_stage == 0)) && (Scaler_MEMC_GetMEMC_Enable() == 1))
		ScalerMEMC_TasksHandler();
	return;	
}
#else
void memc_TasksHandler_task( unsigned long data )
{
	if(MEMC_GetInOutISREnable() && ((g_memc_hdmi_switch_state == 0) && (HDMI_PowerSaving_stage == 0) && (g_memc_switch_state == 0) && (DTV_PowerSaving_stage == 0)) && (Scaler_MEMC_GetMEMC_Enable() == 1))
		ScalerMEMC_TasksHandler();
	return;	
}
#endif
#endif//BUILD_QUICK_SHOW

#ifndef BUILD_QUICK_SHOW
#if 1//RTK_MEMC_Performance_tunging_from_tv001
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
DECLARE_TASKLET(memc_TasksHandler_tasklet, memc_TasksHandler_task);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 241))
DECLARE_TASKLET_OLD(memc_TasksHandler_tasklet, memc_TasksHandler_task);
#else
DECLARE_TASKLET(memc_TasksHandler_tasklet, memc_TasksHandler_task, 0);
#endif
#else
DECLARE_TASKLET(memc_TasksHandler_tasklet, memc_TasksHandler_task, 0);
#endif
#endif //BUILD_QUICK_SHOW

//#define BIT(x)               (1 <<x)

#if 1
extern unsigned int loadscript_cnt_2;

unsigned int Scaler_MEMC_ISR(void){

#ifdef CONFIG_MEMC_BYPASS
	return MEMC_IRQ_HANDLED;
#else

#ifdef CONFIG_MEMC_TASK_QUEUE
	
#if 0
	/*resolve tasks from rpc*/
	if(MEMC_GetInOutISREnable() && ((g_memc_hdmi_switch_state == 0) && (HDMI_PowerSaving_stage == 0) && (g_memc_switch_state == 0) && (DTV_PowerSaving_stage == 0)) && (Scaler_MEMC_GetMEMC_Enable() == 1))
		ScalerMEMC_TasksHandler();
#else
#ifndef BUILD_QUICK_SHOW
	tasklet_schedule(&memc_TasksHandler_tasklet);
#endif
#endif
	
#endif	

#ifdef MEMC_ISR_Times_debug
		if(MEMC_GetInOutISREnable()){
			static unsigned int sEnterCnt = 0;
			static int64_t sLastPts_1s = 0;
			int64_t currentPts = MEMC_GetPTS();
	
			sEnterCnt++;
			if(currentPts < sLastPts_1s){
				sLastPts_1s = currentPts;
				sEnterCnt = 0;
			}
			if(currentPts - sLastPts_1s > 90090){
				rtd_pr_memc_notice("TasksHandler(cnt:%d,%d)'\n", sEnterCnt,Scaler_MEMC_GetMEMC_Enable() );				
				sLastPts_1s = currentPts;
				sEnterCnt = 0;
			}			 
		}
		
#endif




	return MEMC_IRQ_HANDLED;
#endif
}

#else
unsigned char u1_is_me_dead;
extern unsigned int loadscript_cnt_2;

unsigned int Scaler_MEMC_ISR(void){

/*#ifdef CONFIG_MEMC_BYPASS
        return MEMC_IRQ_HANDLED;
#else*/

	//LogPrintf(DBG_MSG,  "[MEMC][%s]\n",__FUNCTION__);
	static unsigned char u1_is_me_dead_pre = 0;
	unsigned int u32_rb_val = 0;

	_RPC_clues* RPC_SmartPic_clue = (_RPC_clues *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_SMARTPIC_CLUS);

	if(get_MEMC_bypass_status_refer_platform_model() == TRUE)
		return MEMC_IRQ_HANDLED;
	else{
	if(RPC_SmartPic_clue == NULL){
		u1_is_me_dead = 255;
	} else {
		u1_is_me_dead = RPC_SmartPic_clue->MEMC_VCPU_setting_info.is_me_dead;
	}

	u32_rb_val = rtd_inl(SYS_REG_SYS_CLKEN3_reg);
	u32_rb_val = (u32_rb_val>>31);

#if 0//def CONFIG_RTK_KDEV_DEBUG_ISR
	unsigned int reg_value = 0;
	reg_value = rtd_inl(0xb802e4f8);

	unsigned int time1;
	static unsigned int time1s_set=0;
	static unsigned int time1s_start=0;
	static unsigned int time1s_end=0;
	static unsigned int time1s_cnt=0;
	static unsigned int time_cnt1=0;
	static unsigned int time_cnt2=0;
	static unsigned int time4 =0, time4_pre = 0;
	if((reg_value & BIT(31))){
		time_cnt1++;
	}

	time1 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
	if(time1s_set==0)
	{
		time1s_start = time1;
		time1s_set=1;
		time1s_cnt=0;
	}
	time1s_end = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

#endif

//	if (u1_is_me_dead == 0 && u32_rb_val == 1){
	if (u32_rb_val == 1 && (MEMC_Lib_get_memc_PowerSaving_Mode()!=1) && MEMC_BRING_UP_InitDone_flag >= 1){
//	if (u1_is_me_dead == 0) {
		if (u1_is_me_dead_pre == 1) {
			//LogPrintf(DBG_WARNING,  "[Scaler_MEMC_ISR] me dead recover , enable isr now!!\n\r");
		}
#if 0//def CONFIG_RTK_KDEV_DEBUG_ISR
	time1s_cnt++;
	time_cnt2++;
#endif

		#ifdef CONFIG_MEMC_TASK_QUEUE
		/*resolve tasks from rpc*/
		if(MEMC_GetInOutISREnable() && ((g_memc_hdmi_switch_state == 0) && (HDMI_PowerSaving_stage == 0) && (g_memc_switch_state == 0) && (DTV_PowerSaving_stage == 0)) && (Scaler_MEMC_GetMEMC_Enable() == 1))
			ScalerMEMC_TasksHandler();
		#endif		
		MEMC_LibInputVSInterrupt();
		MEMC_LibOutputVSInterrupt();

	} else {
		if (u1_is_me_dead_pre == 0) {
			//LogPrintf(DBG_WARNING,  "[Scaler_MEMC_ISR] me dead happened , disable isr now!!\n\r");
			}
		if (u32_rb_val != 1) {
			//rtd_pr_memc_notice( "[Scaler_MEMC_ISR] memc clock off, disable isr now!!\n\r");
		}
	}
	u1_is_me_dead_pre = u1_is_me_dead;

#if 0//def CONFIG_RTK_KDEV_DEBUG_ISR
//====================================================
	time4=rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
	if((time1s_end-time1s_start)>=90000)
	{
		rtd_pr_memc_debug("[ISR]MEMC enable  %x,%d,%d,%d\n",reg_value, u1_is_me_dead, u32_rb_val, u1_is_me_dead_pre);
		rtd_pr_memc_debug("[ISR]MEMC timestamp %d\n",(time4 - time4_pre));
		rtd_pr_memc_debug("[ISR]MEMC times %d, %d\n",time_cnt1,time_cnt2);
		if((time1s_end-time1s_start)>=90000){
			rtd_pr_memc_debug("[ISR]MEMC isr %d times(%d CLK)\n",time1s_cnt, (time1s_end-time1s_start));
			time1s_set=0;
		}
	}
	time4_pre = time4;
//====================================================
#endif
	return MEMC_IRQ_HANDLED;
	}
//#endif
}
#endif

unsigned int scalerVIP_Get_MEMC_status_check(MEMC_STATUS_CHECK MEMC_case_status)
{
	unsigned int repeat_status = 255;
	unsigned int clk_state = 255;
	unsigned int status = 0;
	static unsigned int pre_clk_state = 0;

	clk_state = rtd_inl(SYS_REG_SYS_CLKEN3_reg);
	clk_state = clk_state>>27;
	
	if ( (clk_state&0x10)==0 || (clk_state&0x4)==0){
		status = 255;
		if(pre_clk_state != clk_state ){
			rtd_pr_memc_notice("[%s][MEMC] CLK_check, b800011c[27:31]=,%d \n", __func__, clk_state);
		}	
		pre_clk_state = clk_state;
		return status;	
	}else{
		if(pre_clk_state != clk_state ){
			rtd_pr_memc_notice("[%s][MEMC] CLK_check, b800011c[27:31]=,%d \n", __func__, clk_state);
		}
		pre_clk_state = clk_state;
	}
	
	switch(MEMC_case_status) {

		case MEMC_REPEAT_STATUS:
			//=== LBMC_LBMC_24_reg (0xb8099924[4:4]) ======
			//0x0:repeat_off, 0x1:repeat_on 
			//=======================
			repeat_status = HAL_MC_GetRepeatMode();
	 		//ReadRegister(LBMC_LBMC_24_reg, 4, 4, &repeat_status);
			status = repeat_status;
	 	break;
		
		case MEMC_CLK_STATUS:
		//=== SYS_REG_SYS_CLKEN3_reg (0xb800011c[27:31]) ======
		//=== CLKEN_MEMC			[31]
		//=== CLKEN_MEMC_ME			[30]
		//=== CLKEN_MEMC_RBUS 		[29]
		//=== CLKEN_LZMA			[28]
		//=== CLKEN_MEMC_MC			[27]
		//===========================
			clk_state = rtd_inl(SYS_REG_SYS_CLKEN3_reg);
			clk_state = clk_state>>27;
	 		//ReadRegister(SYS_REG_SYS_CLKEN3_reg, 27, 31, &clk_state);
			status = clk_state;
	 	break;

		default : 
	 	break;
	}

	return status;
	
}

unsigned int scalerVIP_Get_MEMC_status(void)
{
	unsigned int status_repeat = 255;
	unsigned int state_clk = 255;
	static unsigned int pre_status_repeat= 0, pre_state_clk=0;

	state_clk = scalerVIP_Get_MEMC_status_check(MEMC_CLK_STATUS);
	
	if(state_clk == 255){
		if(pre_state_clk != state_clk){
			rtd_pr_memc_notice("[%s][MEMC] CLK_ALL_OFF, clk=,%d \n", __func__, state_clk);
		}
		pre_state_clk = state_clk;
		return MEMC_CLK_ALLOFF;		
	}else{
		status_repeat = scalerVIP_Get_MEMC_status_check(MEMC_REPEAT_STATUS);
		
		if((pre_state_clk != state_clk) || (pre_status_repeat != status_repeat) ){
			rtd_pr_memc_notice("[%s][MEMC] Check_status_change, clk=,%d, repeat=,%d \n", __func__, state_clk, status_repeat);
		}
		
		pre_state_clk = state_clk;
		pre_status_repeat = status_repeat;
		
		if((state_clk>>3)==0x2 ) {
			return MEMC_CLK_MCONLY_REPEATON;
		}else if((state_clk>>3)==0x3 ) {
			if(status_repeat){
				return  MEMC_CLK_ALLON_REPEATON;
			}else{
				return  MEMC_CLK_ALLON_REPEATOFF;
			}			
		}

		return  MEMC_STATUS_APPLY_CHECK;		
	}


}

void Scaler_MEMC_resetMEMCMode(void)
{
	//(LGE)STR init force repeatmode, then change by WEBOS or others
	MEMC_LibResetMEMCMode();
}

#if 1//RTK_MEMC_Performance_tunging_from_tv001
extern MEMC_INFO_FROM_SCALER *Scaler_Get_Info_from_Scaler(VOID);

typedef enum{
	INOUT_FRAMERATE_RATIO_1to1_OUTPUT_60 = 0,	// in:50 out:50, in:60 out:60
	INOUT_FRAMERATE_RATIO_1to2_OUTPUT_60 = 1,	// in:24 out:48, in:25 out:50, in:30 out:60
	INOUT_FRAMERATE_RATIO_2to5_OUTPUT_60 = 2,	// in:24 out:60
	INOUT_FRAMERATE_RATIO_1to2_OUTPUT_120 = 3,	// in:48 out:96, in:50 out:100, in:60 out:120
	INOUT_FRAMERATE_RATIO_1to4_OUTPUT_120 = 4,	// in:25 out:100, in:30 out:120
	INOUT_FRAMERATE_RATIO_1to5_OUTPUT_120 = 5,	// in:24 out:120
	INOUT_SUPPORT_NUM,	
	INOUT_F = 255,	// no support
}INOUT_SUPPORT_TYPE;

typedef enum{
	MemcDelay_ultralowdelay,	// ultra low delay
	MemcDelay_game,				// game mode
	MemcDelay_repeat_pz,		// repeat mode with pz mode
	MemcDelay_repeat_iz,		// repeat mode with iz mode
	MemcDelay_normal,			// normal mode
	MemcDelay_Num,
}MemcDelay_TYPE;

const unsigned char g_ucMemcDelay_table[INOUT_SUPPORT_NUM][MemcDelay_Num] = 
{
/**** memc delay time(ms) = g_ucMemcDelay_table * 500 / output_frame_rate ****/

// game(ultra low delay)	game	repeat(pz)	repeat(iz)	normal
 	{		0,	 			2,			12,			14, 	14}, // in:50 out:50, in:60 out:60
 	{		4,	 			4,			14,			18, 	16}, // in:24 out:48, in:25 out:50, in:30 out:60
 	{		5,	 			5,			18,			24, 	20}, // in:24 out:60
 	{		2,	 			4,			22,			26, 	24}, // in:48 out:96, in:50 out:100, in:60 out:120
 	{		8,	 			8,			26,			34, 	30}, // in:25 out:100, in:30 out:120
 	{		10,	 			10,			34,			44, 	36}, // in:24 out:120 
};

unsigned int Round(unsigned int input)
{
    return (((2*input)+1)>>1);
}

unsigned char Get_InOutType_By_FrameRate(unsigned int in_frame_rate, unsigned int out_frame_rate)
{
	unsigned char u8_InOut_id = INOUT_F;

	if(out_frame_rate<=60) {
		if(in_frame_rate==out_frame_rate){ // in:50 out:50, in:60 out:60
			u8_InOut_id = INOUT_FRAMERATE_RATIO_1to1_OUTPUT_60;
		}
		else if(in_frame_rate*2==out_frame_rate){ // in:24 out:48, in:25 out:50, in:30 out:60
			u8_InOut_id = INOUT_FRAMERATE_RATIO_1to2_OUTPUT_60;
		}
		else if(in_frame_rate*5==out_frame_rate*2){ // in:24 out:60
			u8_InOut_id = INOUT_FRAMERATE_RATIO_2to5_OUTPUT_60;
		}
	}
	else if(out_frame_rate<=120) {
		if(in_frame_rate*2==out_frame_rate){ // in:48 out:96, in:50 out:100, in:60 out:120
			u8_InOut_id = INOUT_FRAMERATE_RATIO_1to2_OUTPUT_120;
		}
		else if(in_frame_rate*4==out_frame_rate){ // in:25 out:100, in:30 out:120
			u8_InOut_id = INOUT_FRAMERATE_RATIO_1to4_OUTPUT_120;
		}
		else if(in_frame_rate*5==out_frame_rate){ // in:24 out:120 
			u8_InOut_id = INOUT_FRAMERATE_RATIO_1to5_OUTPUT_120;
		}
	}

	return u8_InOut_id;
}

#endif



unsigned char * Scaler_MEMC_GetCadence(void)
{
	return g_scaler_memc_cadence;
}

unsigned int Scaler_MEMC_GetAVSyncDelay(void)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
			return 0;
#else


#ifndef CONFIG_MEMC_BYPASS
	//if(get_MEMC_bypass_status_refer_platform_model() == FALSE || get_platform() == PLATFORM_K8HP )
	if(get_MEMC_bypass_status_refer_platform_model() == FALSE)
	{
	static unsigned char MemcDelay_check =0;

#if 1//RTK_MEMC_Performance_tunging_from_tv001
	MEMC_INFO_FROM_SCALER *info_from_scaler = Scaler_Get_Info_from_Scaler();
	unsigned char u8_InOut_id = INOUT_F;
	static unsigned int in_frame_rate_pre = 0, out_frame_rate_pre = 0;
#else
	MEMC_INFO_FROM_SCALER *info_from_scaler = Get_Info_from_Scaler();
#endif	

	unsigned int log_en = 0;
	unsigned int repeat_mode = 0, pz_sel = 0, MC_8_buffer_en = 0;

	unsigned int log_en2=0;
	
	ReadRegister(SOFTWARE3_SOFTWARE3_10_reg, 13, 13, &log_en2);
	
	ReadRegister(SOFTWARE1_SOFTWARE1_60_reg, 25, 25, &log_en);
	ReadRegister(SOFTWARE1_SOFTWARE1_62_reg, 0, 0, &MC_8_buffer_en);
	//ReadRegister(LBMC_LBMC_24_reg, 4, 4, &repeat_mode);
	//ReadRegister(LBMC_LBMC_24_reg, 13, 13, &pz_sel);

	if(log_en2)
	{	
		rtd_pr_memc_emerg("[%s][%d] in:%d out:%d\n\r", __FUNCTION__, __LINE__,info_from_scaler->in_frame_rate, info_from_scaler->out_frame_rate);
	}

	
	if(MEMC_LibGetMEMCMode() == MEMC_OFF){
		repeat_mode = 1;
		if(MEMC_LibGetCinemaMode() == FALSE && info_from_scaler->in_frame_rate == 24 && info_from_scaler->out_frame_rate == 120){
			if(MC_8_buffer_en == 0)
				pz_sel = 1;
			else
				pz_sel = 0; // 8 buffer no need to use PZ
		}
	}

	// for K-17852 +++
	if(info_from_scaler->in_frame_rate == 24 && info_from_scaler->out_frame_rate == 60){
		if(info_from_scaler->game_mode_flag){
			g_ucMemcDelay = 39; // K-23281 // 3*delay_time_per_frm; // 48
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 183; // 11*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 142; // 8.5*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 163; // 10*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 24 && info_from_scaler->out_frame_rate == 48){
		//if(info_from_scaler->game_mode_flag){
			g_ucMemcDelay = 188; // 3*delay_time_per_frm; // 48
		//}
		//else{
		//	g_ucMemcDelay = 188; // 11*delay_time_per_frm; // 176
		//}
	}
	else if(info_from_scaler->in_frame_rate == 25 && info_from_scaler->out_frame_rate == 50){
		if(info_from_scaler->game_mode_flag){
			g_ucMemcDelay = 40; // 2*delay_time_per_frm; // 40
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 180; // 9*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 140; // 7*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 160; // 8*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 30 && info_from_scaler->out_frame_rate == 60){
		if(info_from_scaler->game_mode_flag){
			g_ucMemcDelay = 33; // 2*delay_time_per_frm; // 32
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 150; // 9*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 117; // 7*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 133; // 8*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 48 && info_from_scaler->out_frame_rate == 48){
		if(info_from_scaler->game_mode_flag){
			//if((info_from_scaler->hdmi_flag)||(info_from_scaler->vdec_flag)){
			//	g_ucMemcDelay = 0; // 0*delay_time_per_frm; // 0
			//}
			//else{
				g_ucMemcDelay = 21; // 1*delay_time_per_frm; // 20
			//}
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 104; // 5*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 83; // 4*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 104; // 5*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 50 && info_from_scaler->out_frame_rate == 50){
		if(info_from_scaler->game_mode_flag){
			if((info_from_scaler->hdmi_flag)||(info_from_scaler->vdec_flag)){
				g_ucMemcDelay = 0; // 0*delay_time_per_frm; // 0
			}
			else{
				g_ucMemcDelay = 20; // 1*delay_time_per_frm; // 20
			}
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 140; // 7*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 120; // 6*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 140; // 7*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 60 && info_from_scaler->out_frame_rate == 60){
		if(info_from_scaler->game_mode_flag){
			if((info_from_scaler->hdmi_flag)||(info_from_scaler->vdec_flag)){
				g_ucMemcDelay = 0; // 0*delay_time_per_frm; // 0
			}
			else{
				g_ucMemcDelay = 17; // 1*delay_time_per_frm; // 16
			}
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 117; // 7*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 100; // 6*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 117; // 7*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 24 && info_from_scaler->out_frame_rate == 120){
		if(info_from_scaler->game_mode_flag){
			g_ucMemcDelay = 42; // 5*delay_time_per_frm; // 40
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 175; // 22*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 133; // 17*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 154; // 18*delay_time_per_frm; // 184
			}
		}
	}
#if 1//#if RTK_MEMC_Performance_tunging_from_tv001 //k23
	else if(info_from_scaler->in_frame_rate == 24 && info_from_scaler->out_frame_rate == 96){
		g_ucMemcDelay = 178; // 17*delay_time_per_frm;
	}
	else if(info_from_scaler->in_frame_rate == 24 && info_from_scaler->out_frame_rate == 144){
		g_ucMemcDelay = 118; // 17*delay_time_per_frm;
	}
#endif	
	else if(info_from_scaler->in_frame_rate == 25 && info_from_scaler->out_frame_rate == 100){
		if(info_from_scaler->game_mode_flag){
			g_ucMemcDelay = 40; // 4*delay_time_per_frm; // 40
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 170; // 17*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 130; // 13*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 150; // 14.5*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 25 && info_from_scaler->out_frame_rate == 125){
		g_ucMemcDelay = 136; // 17*delay_time_per_frm;
	}
	else if(info_from_scaler->in_frame_rate == 30 && info_from_scaler->out_frame_rate == 120){
		if(info_from_scaler->game_mode_flag){
			g_ucMemcDelay = 33; // 4*delay_time_per_frm; // 32
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 142; // 17*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 108; // 13*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 121; // 14.5*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 48 && info_from_scaler->out_frame_rate == 96){
		if(info_from_scaler->game_mode_flag){
			//if((info_from_scaler->hdmi_flag)||(info_from_scaler->vdec_flag)){
			//	g_ucMemcDelay = 0; // 0*delay_time_per_frm; // 0
			//}
			//else{
				g_ucMemcDelay = 21; // 2*delay_time_per_frm; // 20
			//}
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 94; // 9*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 73; // 7*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 83; // 8*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 48 && info_from_scaler->out_frame_rate == 144){
		g_ucMemcDelay = 90; // 13*delay_time_per_frm;
	}
	else if(info_from_scaler->in_frame_rate == 50 && info_from_scaler->out_frame_rate == 100){
		if(info_from_scaler->game_mode_flag){
			if(info_from_scaler->hdmi_flag || (info_from_scaler->adaptive_stream_flag && info_from_scaler->YT_callback_flag)){
				g_ucMemcDelay = 10; // 1*delay_time_per_frm; // 10
			}
			else{
				g_ucMemcDelay = 20; // 2*delay_time_per_frm; // 20
			}
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 130; // 13*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 110; // 11*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 120; // 12*delay_time_per_frm; // 184
			}
		}
	}
	else if(info_from_scaler->in_frame_rate == 60 && info_from_scaler->out_frame_rate == 120){
		if(info_from_scaler->game_mode_flag){
			if(info_from_scaler->hdmi_flag || (info_from_scaler->adaptive_stream_flag && info_from_scaler->YT_callback_flag)){
				g_ucMemcDelay = 8; // 1*delay_time_per_frm; // 8
			}
			else{
				g_ucMemcDelay = 17; // 2*delay_time_per_frm; // 16
			}
		}
		else{
			if(repeat_mode){
				if(pz_sel == 0){
					g_ucMemcDelay = 108; // 13*delay_time_per_frm; // 184
				}
				else{
					g_ucMemcDelay = 92; // 11*delay_time_per_frm; // 184
				}
			}
			else{
				g_ucMemcDelay = 100; // 12*delay_time_per_frm; // 184
			}
		}
	}
    else if(info_from_scaler->in_frame_rate == 288 && info_from_scaler->out_frame_rate == 288){
            g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 288 && info_from_scaler->out_frame_rate == 240){
            g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 288 && info_from_scaler->out_frame_rate == 144){
        g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 240 && info_from_scaler->out_frame_rate == 240){
        g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 240 && info_from_scaler->out_frame_rate == 120){
        g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 144 && info_from_scaler->out_frame_rate == 144){
        g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 120 && info_from_scaler->out_frame_rate == 120){
        g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 100 && info_from_scaler->out_frame_rate == 100){
        g_ucMemcDelay = 0;
    }
    else if(info_from_scaler->in_frame_rate == 96 && info_from_scaler->out_frame_rate == 96){
        g_ucMemcDelay = 0;
    }
	else{
#if 1//RTK_MEMC_Performance_tunging_from_tv001
		u8_InOut_id = Get_InOutType_By_FrameRate(info_from_scaler->in_frame_rate, info_from_scaler->out_frame_rate);
		
		// support in-out frame rate
		// memc delay time(ms) = g_ucMemcDelay_table * 500 / output_frame_rate
		if(u8_InOut_id < INOUT_SUPPORT_NUM){				
			if(info_from_scaler->game_mode_flag){
				if(info_from_scaler->hdmi_flag || 
					(info_from_scaler->adaptive_stream_flag && info_from_scaler->YT_callback_flag && info_from_scaler->in_frame_rate*2==info_from_scaler->out_frame_rate) ||
					(info_from_scaler->vdec_flag && info_from_scaler->in_frame_rate*2!=info_from_scaler->out_frame_rate)){ 
					g_ucMemcDelay = Round(g_ucMemcDelay_table[u8_InOut_id][MemcDelay_ultralowdelay]*500/info_from_scaler->out_frame_rate);
				}
				else{
					g_ucMemcDelay = Round(g_ucMemcDelay_table[u8_InOut_id][MemcDelay_game]*500/info_from_scaler->out_frame_rate);
				}
			}
			else{
				if(repeat_mode){
					if(pz_sel == 0){
						g_ucMemcDelay = Round(g_ucMemcDelay_table[u8_InOut_id][MemcDelay_repeat_iz]*500/info_from_scaler->out_frame_rate);
					}
					else{
						g_ucMemcDelay = Round(g_ucMemcDelay_table[u8_InOut_id][MemcDelay_repeat_pz]*500/info_from_scaler->out_frame_rate);
					}
				}
				else{
					g_ucMemcDelay = Round(g_ucMemcDelay_table[u8_InOut_id][MemcDelay_normal]*500/info_from_scaler->out_frame_rate);
				}
			}

			if(in_frame_rate_pre != info_from_scaler->in_frame_rate || out_frame_rate_pre != info_from_scaler->out_frame_rate) {
				rtd_pr_memc_notice("[%s][%d] memc delay time:%d  delay table:%d  in:%d out:%d\n\r", __FUNCTION__, __LINE__, g_ucMemcDelay, u8_InOut_id, info_from_scaler->in_frame_rate, info_from_scaler->out_frame_rate);
			}
		}
		else {
			// memc delay frames
			// game mode:1 frames, iz:5 frames, pz:4 frames, blending:4.5/5 frames
			if(info_from_scaler->game_mode_flag){
				g_ucMemcDelay = Round(1000/info_from_scaler->in_frame_rate); //delay 1 frame
			}
			else{
				if(repeat_mode){
					if(pz_sel == 0){
						g_ucMemcDelay = Round(5000/info_from_scaler->in_frame_rate); //delay 5 frames
					}
					else{
						g_ucMemcDelay = Round(4000/info_from_scaler->in_frame_rate); //delay 4 frames
					}
				}
				else{
					if(info_from_scaler->out_frame_rate/info_from_scaler->in_frame_rate>=2){
						g_ucMemcDelay = Round(4500/info_from_scaler->in_frame_rate); //delay 4.5 frames
					}
					else {
						g_ucMemcDelay = Round(5000/info_from_scaler->in_frame_rate); //delay 5 frames
					}
				}
			}

			if(in_frame_rate_pre != info_from_scaler->in_frame_rate || out_frame_rate_pre != info_from_scaler->out_frame_rate) {
				rtd_pr_memc_notice("[%s][%d] not match case!!  memc delay time:%d  in:%d out:%d\n\r", __FUNCTION__, __LINE__, g_ucMemcDelay, info_from_scaler->in_frame_rate, info_from_scaler->out_frame_rate);
			}
		}
#else		
		if(in_frame_rate_pre != info_from_scaler->in_frame_rate || out_frame_rate_pre != info_from_scaler->out_frame_rate) {
			rtd_pr_memc_notice("[%s][%d] not match case!!  memc delay time:%d  in:%d out:%d\n\r", __FUNCTION__, __LINE__, g_ucMemcDelay, info_from_scaler->in_frame_rate, info_from_scaler->out_frame_rate);
		}
		g_ucMemcDelay = 0;
#endif
	}
	//else{ keep actual calculation }


	
	if(log_en){
		rtd_pr_memc_notice("[%s][%d][,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__, __LINE__, 
			info_from_scaler->in_frame_rate, info_from_scaler->out_frame_rate, info_from_scaler->game_mode_flag,
			info_from_scaler->adaptive_stream_flag, info_from_scaler->YT_callback_flag, info_from_scaler->hdmi_flag);
	}
	// for K-17852 ---

	if(g_ucMemcDelay!= MemcDelay_check){
		//rtd_pr_memc_notice( "[Scaler_MEMC_ISR] memc clock off, disable isr now!!\n\r");
		MemcDelay_check = g_ucMemcDelay;
	}
#if 1//RTK_MEMC_Performance_tunging_from_tv001
	in_frame_rate_pre = info_from_scaler->in_frame_rate;
	out_frame_rate_pre = info_from_scaler->out_frame_rate;
#endif	
	return g_ucMemcDelay;
	}else{
	return 0;
		}
#else
	return 0;
#endif
#endif
}

void Scaler_MEMC_SetAVSyncDelay(unsigned int a_ucDelay)
{
	g_ucMemcDelay = a_ucDelay;
}

void drvif_clock_set_memc_div(sys_reg_sys_dclk_gate_sel0_RBUS* sys_reg_sys_dclk_gate_sel0_reg, sys_reg_sys_dclk_gate_sel1_RBUS* sys_reg_sys_dclk_gate_sel1_reg)
{
    // Merlin7/8 MEMC clock setting guide ( from CRT spec )
    sys_reg_sys_dclk_gate_sel1_reg->dclk_memc_in_if_gate_sel = sys_reg_sys_dclk_gate_sel0_reg->dclk_s1_f2p_gate_sel;
    sys_reg_sys_dclk_gate_sel1_reg->dclk_mc_ippre_gate_sel = sys_reg_sys_dclk_gate_sel0_reg->dclk_s1_f2p_gate_sel;
    if( sys_reg_sys_dclk_gate_sel0_reg->dclk_s1_f2p_gate_sel > 1 )
        sys_reg_sys_dclk_gate_sel1_reg->dclk_mc_ippre2x_gate_sel = sys_reg_sys_dclk_gate_sel0_reg->dclk_s1_f2p_gate_sel - 1;
    else
        sys_reg_sys_dclk_gate_sel1_reg->dclk_mc_ippre2x_gate_sel = 0;
    sys_reg_sys_dclk_gate_sel1_reg->dclk_mc_gate_sel = sys_reg_sys_dclk_gate_sel0_reg->dclk_s2_f2p_gate_sel;
    sys_reg_sys_dclk_gate_sel1_reg->dclk_memc_out_if_gate_sel = sys_reg_sys_dclk_gate_sel0_reg->dclk_s2_f2p_gate_sel + 1;
}

#ifdef CONFIG_MEMC_TASK_QUEUE
#ifndef BUILD_QUICK_SHOW
 static DEFINE_MUTEX(memc_task_mutex);
#define memc_task_lock()      mutex_lock(&memc_task_mutex)
#define memc_task_unlock()    mutex_unlock(&memc_task_mutex)
static DEFINE_SPINLOCK(memc_task_Spinlock);/*Spin lock no context switch. This is for copy paramter*/
#endif

unsigned char Scaler_MEMC_PushTask(SCALER_MEMC_TASK_T *pTask)
{
#ifdef CONFIG_MEMC_NOTSUPPORT // for code size reduce, when MEMC_BYPASS
		return TRUE;
#else

#ifdef CONFIG_MEMC_BYPASS
	return TRUE;
#else
	unsigned int id = 0;
	unsigned char ret = TRUE;
	unsigned long flags;//for spin_lock_irqsave

	//extern void hw_semaphore_try_lock(void);
	//extern void hw_semaphore_unlock(void);

	//if(get_MEMC_bypass_status_refer_platform_model() == TRUE && get_platform() != PLATFORM_K8HP){
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		return TRUE;
	}else{
	//extern void hw_semaphore_try_lock(void);
	//extern void hw_semaphore_unlock(void);
#if 1
	rtd_pr_memc_notice("[%s][%d][CASE,%d,][,%d,%d,%d,%d,%d,%d,]\n",__FUNCTION__, __LINE__, pTask->type,
		MEMC_GetInOutISREnable(), g_memc_hdmi_switch_state, HDMI_PowerSaving_stage,
		g_memc_switch_state, DTV_PowerSaving_stage, Scaler_MEMC_GetMEMC_Enable());
#else // debug
	if((vbe_disp_get_stop_memc_push_flag() == 1) && ((rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>30)==0)){
		rtd_pr_memc_emerg( "[K20444][start][task.type=%d][push=%d,pop=%d][clk=%d,ISR=%d,en=%d],[90k=%d]\n",pTask->type, g_memc_tasks_header, g_memc_tasks_tailer,\
										(rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>30),MEMC_GetInOutISREnable(),Scaler_MEMC_GetMEMC_Enable(),rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
		
		return ret;
	}else{
		rtd_pr_memc_emerg( "[K20444][start][task.type=%d][push=%d,pop=%d][clk=%d,ISR=%d,en=%d],[90k=%d](%d)(%d,%d,%d,%d)\n",pTask->type, g_memc_tasks_header, g_memc_tasks_tailer,\
										(rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>30),MEMC_GetInOutISREnable(),Scaler_MEMC_GetMEMC_Enable(),rtd_inl(TIMER_SCPU_CLK90K_LO_reg),sizeof(SCALER_MEMC_TASK_T),
										g_memc_hdmi_switch_state,HDMI_PowerSaving_stage,g_memc_switch_state,DTV_PowerSaving_stage);
	}
#endif
#ifndef BUILD_QUICK_SHOW
	/*get HW semaphore*/
	memc_task_lock();
#endif
	/*push task to queue*/
	id = (g_memc_tasks_header + 1) % MEMC_TASK_MAX_CNT;
	if(id == g_memc_tasks_tailer){ //can't print inside the spin_lock/unlock to avorid schedule out!
		rtd_pr_memc_notice( "[%s]Task queue status(header=%d,tailer=%d)\n",\
			__FUNCTION__,g_memc_tasks_header,g_memc_tasks_tailer);
		rtd_pr_memc_notice("[%s][line:%d][,%d,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__, __LINE__,\
			MEMC_GetInOutISREnable(), g_memc_hdmi_switch_state, HDMI_PowerSaving_stage,\
			g_memc_switch_state, DTV_PowerSaving_stage, Scaler_MEMC_GetMEMC_Enable());
	}
#ifndef BUILD_QUICK_SHOW
	spin_lock_irqsave(&memc_task_Spinlock, flags);
#endif
	if(id == g_memc_tasks_tailer){
		g_memc_tasks_tailer = (g_memc_tasks_tailer + 1) % MEMC_TASK_MAX_CNT;
		ret = TRUE;
	}
#ifndef BUILD_QUICK_SHOW
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);
#endif
	if(g_memc_tasks_header >= MEMC_TASK_MAX_CNT){
		rtd_pr_memc_notice( "[%s]Invalid task index(%d,%d).Resume!\n",\
			__FUNCTION__,g_memc_tasks_header,g_memc_tasks_tailer);
		g_memc_tasks_header = 0;
		g_memc_tasks_tailer = 0;
		id = (g_memc_tasks_header + 1) % MEMC_TASK_MAX_CNT;
	}
#ifndef BUILD_QUICK_SHOW
	memcpy(&g_memc_tasks_queue[g_memc_tasks_header],pTask,sizeof(SCALER_MEMC_TASK_T));
#endif
	g_memc_tasks_header = id;
#ifndef BUILD_QUICK_SHOW
	/*put HW semaphore*/
	memc_task_unlock();
#endif
	//rtd_pr_memc_emerg( "[K20444][end][push=%d,pop=%d][clk=%d][90k=%d]\n", g_memc_tasks_header, g_memc_tasks_tailer,(rtd_inl(SYS_REG_SYS_CLKEN3_reg)>>30),rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	
	/*return*/
	return ret;
	}
#endif
#endif	
}


unsigned char ScalerMEMC_PopTask(SCALER_MEMC_TASK_T *pTask)
{
#ifdef BUILD_QUICK_SHOW
	return TRUE;
#else
	unsigned long flags;//for spin_lock_irqsave
	unsigned char result = 0;

	spin_lock_irqsave(&memc_task_Spinlock, flags);

	if(g_memc_tasks_tailer == g_memc_tasks_header){
		result = FALSE;
	}
	else if(g_memc_tasks_tailer >= MEMC_TASK_MAX_CNT){
		rtd_pr_memc_notice( "[%s]Invalid task index(%d,%d).Resume!\n",\
						__FUNCTION__,g_memc_tasks_header,g_memc_tasks_tailer);
		g_memc_tasks_header = 0;
		g_memc_tasks_tailer = 0;
		result = FALSE;
	}else{
		memcpy(pTask,&g_memc_tasks_queue[g_memc_tasks_tailer],sizeof(SCALER_MEMC_TASK_T));
		g_memc_tasks_tailer = (g_memc_tasks_tailer + 1) % MEMC_TASK_MAX_CNT;
		result = TRUE;
	}
	spin_unlock_irqrestore(&memc_task_Spinlock, flags);

	return result;
#endif
}

void ScalerMEMC_TasksHandler(VOID)
{
	SCALER_MEMC_TASK_T task;
	int task_cnt = 0;

	/*resolve all tasks in the queue*/
	while(task_cnt < MEMC_TASK_MAX_CNT && ScalerMEMC_PopTask(&task) == TRUE){
		switch(task.type){
		case SCALERIOC_MEMC_SETMOTIONCOMP:
		{
			rtd_pr_memc_notice( "[SETMOTIONCOMP]%d,%d,%d\n",\
						task.data.memc_set_motion_comp.blurLevel,\
						task.data.memc_set_motion_comp.judderLevel,\
						task.data.memc_set_motion_comp.motion);
			Scaler_MEMC_SetMotionComp(task.data.memc_set_motion_comp.blurLevel,\
						task.data.memc_set_motion_comp.judderLevel,\
						task.data.memc_set_motion_comp.motion);
		}
		break;

		case SCALERIOC_MEMC_SETBLURLEVEL:
		{
			rtd_pr_memc_notice( "[SETBLURLEVEL]%d\n",task.data.value);
			Scaler_MEMC_SetBlurLevel(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_SETJUDDERLEVEL:
		{
			rtd_pr_memc_notice( "[SETJUDDERLEVEL]%d\n",task.data.value);
			Scaler_MEMC_SetJudderLevel(task.data.value);
		
		}
		break;

		case SCALERIOC_MEMC_MOTIONCOMPONOFF:
		{
			rtd_pr_memc_notice( "[MOTIONCOMPONOFF]%d\n",task.data.value);
			Scaler_MEMC_MotionCompOnOff(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_LOWDELAYMODE:
		{
			rtd_pr_memc_notice( "[LOWDELAYMODE]%d\n",task.data.value);
			Scaler_MEMC_LowDelayMode(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_SETRGBYUVMode:
		{
			rtd_pr_memc_notice( "[SETRGBYUVMode]%d\n",task.data.value);
			Scaler_MEMC_SetRGBYUVMode(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_GETFRAMEDELAY:
		{
			rtd_pr_memc_notice( "[GETFRAMEDELAY]\n");
			//Scaler_MEMC_GetFrameDelay ((unsigned short *)arg);
		}
		break;

		case SCALERIOC_MEMC_SETVIDEOBLOCK:
		{
			rtd_pr_memc_notice( "[SETMOTIONCOMP]%d,%d\n",\
							task.data.memc_set_video_block.type,\
							task.data.memc_set_video_block.bOnOff);
			Scaler_MEMC_SetVideoBlock(task.data.memc_set_video_block.type,\
							task.data.memc_set_video_block.bOnOff);
		}
		break;

		case SCALERIOC_MEMC_SETTRUEMOTIONDEMO:
		{
			rtd_pr_memc_notice( "[SETTRUEMOTIONDEMO]%d\n",task.data.value);
			Scaler_MEMC_SetTrueMotionDemo(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_GETFIRMWAREVERSION:
		{
			rtd_pr_memc_notice( "[GETFIRMWAREVERSION]%d\n",task.data.value);
			//Scaler_MEMC_GetFirmwareVersion((unsigned short *)arg);
		}
		break;

		case SCALERIOC_MEMC_SETBYPASSREGION:
		{
			rtd_pr_memc_notice( "[SETBYPASSREGION]%d,%d,%d,%d,%d,%d\n",\
							task.data.memc_set_bypass_region.bOnOff,\
							task.data.memc_set_bypass_region.region,\
							task.data.memc_set_bypass_region.x,\
							task.data.memc_set_bypass_region.y,\
							task.data.memc_set_bypass_region.w,\
							task.data.memc_set_bypass_region.h);
			Scaler_MEMC_SetBypassRegion(task.data.memc_set_bypass_region.bOnOff,\
							task.data.memc_set_bypass_region.region,\
							task.data.memc_set_bypass_region.x,\
							task.data.memc_set_bypass_region.y,\
							task.data.memc_set_bypass_region.w,\
							task.data.memc_set_bypass_region.h);
		}
		break;

		case SCALERIOC_MEMC_SETREVERSECONTROL:
		{
			rtd_pr_memc_notice( "[SETREVERSECONTROL]%d\n",task.data.value);
			Scaler_MEMC_SetReverseControl(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_FREEZE:
		{
			rtd_pr_memc_notice( "[FREEZE]%d\n",task.data.value);
			Scaler_MEMC_Freeze(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_SETDEMOBAR:
		{
			rtd_pr_memc_notice( "[SETDEMOBAR]%d,%d,%d,%d\n",\
						task.data.memc_set_demo_bar.bOnOff,\
						task.data.memc_set_demo_bar.r,\
						task.data.memc_set_demo_bar.g,\
						task.data.memc_set_demo_bar.b);
			Scaler_MEMC_SetDemoBar(task.data.memc_set_demo_bar.bOnOff,\
						task.data.memc_set_demo_bar.r,\
						task.data.memc_set_demo_bar.g,\
						task.data.memc_set_demo_bar.b);
		}
		break;

		case SCALERIOC_MEMC_SETINOUTUSECASE:
		{
			rtd_pr_memc_notice( "[SETINOUTUSECASE]%d,%d,%d,%d\n",\
							task.data.memc_set_inout_usecase.input_re,\
							task.data.memc_set_inout_usecase.output_re,\
							task.data.memc_set_inout_usecase.input_for,\
							task.data.memc_set_inout_usecase.output_for);
			Scaler_MEMC_SetInOutputUseCase(task.data.memc_set_inout_usecase.input_re,\
							task.data.memc_set_inout_usecase.output_re,\
							task.data.memc_set_inout_usecase.input_for,\
							task.data.memc_set_inout_usecase.output_for);
		}
		break;

		case SCALERIOC_MEMC_SETINPUTOUTPUTFORMAT:
		{
			rtd_pr_memc_notice( "[SETINPUTOUTPUTFORMAT]%d,%d\n",\
							task.data.memc_set_inout_format.input_for,\
							task.data.memc_set_inout_format.output_for);
			Scaler_MEMC_SetInputOutputFormat(task.data.memc_set_inout_format.input_for,\
							task.data.memc_set_inout_format.output_for);
		}
		break;

		case SCALERIOC_MEMC_SETINPUTOUTPUTRESOLUTION:
		{
			rtd_pr_memc_notice( "[SETINPUTOUTPUTRESOLUTION]%d,%d\n",\
							task.data.memc_set_inout_resolution.input_re,\
							task.data.memc_set_inout_resolution.output_re);
			Scaler_MEMC_SetInputOutputResolution(task.data.memc_set_inout_resolution.input_re,\
							task.data.memc_set_inout_resolution.output_re);
		}
		break;

		case SCALERIOC_MEMC_SETINOUTPUTFRAMERATE:
		{
			rtd_pr_memc_notice( "[SETINOUTPUTFRAMERATE]%d,%d\n",\
					task.data.memc_inout_framerate.input_frame_rate,\
					task.data.memc_inout_framerate.output_frame_rate);
			Scaler_MEMC_SetInputFrameRate(task.data.memc_inout_framerate.input_frame_rate);
			Scaler_MEMC_SetOutputFrameRate(task.data.memc_inout_framerate.output_frame_rate);
		}
		break;

		case SCALERIOC_MEMC_SETMEMCFRAMEREPEATENABLE:
		{
			rtd_pr_memc_notice( "[SETMEMCFRAMEREPEATENABLE]%d\n",task.data.value);
			Scaler_MEMC_SetMEMCFrameRepeatEnable(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_SETMEMCINOUTMODE:
		{
			rtd_pr_memc_notice( "[SETMEMCINOUTMODE]%d\n",task.data.value);
			ISR_Scaler_MEMC_SetInOutMode(task.data.value);
		}
		break;

		case SCALERIOC_MEMC_SETCINEMAMODE:
		{
			rtd_pr_memc_notice( "[SETCINEMAMODE]%d\n",task.data.value);
			Scaler_MEMC_SetCinemaMode(task.data.value);
		}
		break;

		default:
			rtd_pr_memc_notice( "[%s]Unknown task type=%d(header=%d,tailer=%d)\n",\
				__FUNCTION__,task.type,g_memc_tasks_header,g_memc_tasks_tailer);
			return;
		}//~switch(task.type)
		task_cnt++;
	}//~while(task_cnt < MEMC_TASK_MAX_CNT && ScalerMEMC_PopTask(&task)
	if(task_cnt == MEMC_TASK_MAX_CNT){
		rtd_pr_memc_notice( "[%s]Queue lenght maybe too small(header=%d,tailer=%d,max_tasks_num=%d)\n",\
			__FUNCTION__,g_memc_tasks_header,g_memc_tasks_tailer,MEMC_TASK_MAX_CNT);
	}
}


/**table_iDX
 * Level = 0 => no bypass?? * Level = 1 => I DCTI?D_DCTI?V_DCTI?DLTI : bypass?? * Level = 2 => I DCTI?D_DCTI?V_DCTI?DLTI?RTNR Y?RTNR C?MCNR?DI IP Enabl

 *  -> reg_deg : set(1)
 *  --> reg_deg+10 : set(2)
 *  reg_deg-5 <- : set(saved_level)
**/

extern int register_temperature_callback(int degree,void *fn, void* data, char *module_name);
extern int Scaler_MEMC_MEMC_CLK_OnOff(unsigned char ME_enable, unsigned char MC_enable, unsigned char bMute);
#if IS_ENABLED(CONFIG_RTK_KDRV_THERMAL_SENSOR)
static short int  REG_DEGREE=110;
#if 0 //unused variable
static short int  REG_DEGREE_LV2=120;
static short int  REG_DEGREE_LV3=125;
#endif

#if RTK_MEMC_Performance_tunging_from_tv001
//static short int  REG_DEGREE_LV2=120;
//static short int  REG_DEGREE_LV3=125;
#else
static short int  REG_DEGREE_LV2=120;
static short int  REG_DEGREE_LV3=125;
#endif

extern void Scaler_MEMC_Set_PowerSaving_Status(unsigned char mode);
extern unsigned char Scaler_MEMC_Get_PowerSaving_Status(void);
static void Scaler_MEMC_thermal_callback(void *data, int cur_degree , int reg_degree, char *module_name)
{

	rtd_pr_memc_emerg("[Scaler_MEMC_thermal_callback][cur_deg=%d][Scaler_MEMC_Get_PowerSaving_Status()=%d]\n\r", cur_degree, Scaler_MEMC_Get_PowerSaving_Status());

#if 0
	if (cur_degree >= 125) {
		if (Scaler_MEMC_Get_PowerSaving_Status() != 2){
			rtd_pr_memc_emerg("[Scaler_MEMC_thermal_callback][cur_deg=%d] Disable MEMC!\n\r", cur_degree);
			Scaler_MEMC_Set_PowerSaving_Status(2);
			//handle_memc_powersaving_on((unsigned char)2);
			Scaler_MEMC_MEMC_CLK_OnOff(0,0,1);
		}
	}else if (cur_degree >= 115) {
		if (Scaler_MEMC_Get_PowerSaving_Status() == 0){
			rtd_pr_memc_emerg("[Scaler_MEMC_thermal_callback][cur_deg=%d] Set to MC only mode!\n\r", cur_degree);
			Scaler_MEMC_Set_PowerSaving_Status(1);
			//handle_memc_powersaving_on((unsigned char)1);
			Scaler_MEMC_MEMC_CLK_OnOff(0,1,1);
		}
	}
	else if(cur_degree < 110){
		if(Scaler_MEMC_Get_PowerSaving_Status() != 0){
			Scaler_MEMC_Set_PowerSaving_Status(0);
			//handle_memc_powersaving_on((unsigned char)0);
			Scaler_MEMC_MEMC_CLK_OnOff(1,1,1);
			rtd_pr_memc_emerg("[Scaler_MEMC_thermal_callback][cur_deg=%d] Enable MEMC!\n\r", cur_degree);
		}
	}
#else //120HZ
	if (cur_degree >= 125) {
		if (Scaler_MEMC_Get_PowerSaving_Status() == 0){
			//rtd_pr_memc_emerg("[Scaler_MEMC_thermal_callback][cur_deg=%d] Set to MC only mode!\n\r", cur_degree);
			Scaler_MEMC_Set_PowerSaving_Status(1);
			//handle_memc_powersaving_on((unsigned char)1);
			Scaler_MEMC_MEMC_CLK_OnOff(0,1,1);
		}
	}
	else if(cur_degree <= 115 ){
		if(Scaler_MEMC_Get_PowerSaving_Status() != 0){
			Scaler_MEMC_Set_PowerSaving_Status(0);
		}
	}	
#endif
	return;

}

int Scaler_register_MEMC_thermal_handler(void)
{
	int ret;
	int retdata = 0;

	rtd_pr_memc_info("%s\n",__func__);
	if ((ret = register_temperature_callback(REG_DEGREE, Scaler_MEMC_thermal_callback, (void*)&retdata, "MEMC_PowerSaving")) < 0)
		rtd_pr_memc_info("register MEMC thermal handler fail, ret:%d \n", ret);

	return 0;
}

#if (RTK_MEMC_Performance_tunging_from_tv001==0) //no use
void Scaler_register_MEMC_thermal(int t0, int t1, int t2)
{
	if( (t2<135) && (t2> t1) && (t1 > t0))
	{
		REG_DEGREE=t0;
		REG_DEGREE_LV2=t1;
		REG_DEGREE_LV3=t2;
		Scaler_register_MEMC_thermal_handler();//use resource table;
	}
	else
	{
		Scaler_register_MEMC_thermal_handler();//use default;
	}
}
EXPORT_SYMBOL(Scaler_register_MEMC_thermal);
#endif

#ifndef CONFIG_SUPPORT_SCALER_MODULE
late_initcall(Scaler_register_MEMC_thermal_handler);
#endif

#endif //CONFIG_RTK_KDRV_THERMAL_SENSOR

unsigned char Scaler_MEMC_Get_SW_OPT_Flag(void){
	// TRUE : MC only ; FALSE : MEMC normal mode
	unsigned char Ret = TRUE;

	#if Pro_tv006
	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		Ret = TRUE;
	}
	else{
		if(rtk_otp_field_read_int_by_name("sw_memc_disable") == 1)
		{
			Ret = TRUE;
		}
		else
		{
			if(get_panel_res() == PANEL_RES_UHD || get_panel_res() == PANEL_RES_QHD)
			{
				Ret = FALSE;
			}
			else
			{
				Ret = TRUE;
			}
		}
	}
	#else
		Ret = TRUE;	
	#endif
	return Ret;
}

unsigned char Scaler_MEMC_Get_MEMC_Support_Flag(unsigned int Vporch, unsigned int Htotal, unsigned int Hsize, unsigned int Vtotal, unsigned int Vsize)
{
	// TRUE : can support MEMC ; FALSE :can't support MEMC
	unsigned char Ret = FALSE;
	unsigned int N = 0, val = 0;

	if(get_MEMC_bypass_status_refer_platform_model() == TRUE){
		rtd_pr_memc_info("[%s][Invalid platform !!can't support MEMC !!]", __FUNCTION__);
		Ret = FALSE;
		return Ret;
	}

	if(Hsize == 0 || Vsize == 0){
		rtd_pr_memc_info("[%s][Invalid Size !!can't support MEMC !!]", __FUNCTION__);
		Ret = FALSE;
		return Ret;
	}

	N = Vporch * (Htotal/2) / Hsize;
	val = N * ( 2160/Vsize );

	if(val > 48){
		Ret = TRUE;
	}
	else{
		Ret = FALSE;
	}

	return Ret;
}


#endif


