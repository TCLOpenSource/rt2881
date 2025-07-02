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

#ifndef _SCALER_MEMORY_H_
#define _SCALER_MEMORY_H_

/*============================ Module dependency  ===========================*/
//#include <base_types.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#ifdef CONFIG_BW_96B_ALIGNED
#define DMA_SPEEDUP_ALIGN_VALUE 96
#else
#define DMA_SPEEDUP_ALIGN_VALUE 16
#endif

/*===================================  Types ================================*/
// Structure of memory tag
typedef enum {
	UNDEFINED = 0,
	ALLOCATED_FROM_LOW = 1,
	ALLOCATED_FROM_HIGH = 2,
	ALLOCATED_FROM_PLI = 3,
} MemBlockStatus;

typedef struct _MemoryBlock
{
#ifdef CONFIG_DYNAMIC_ALLOCATION_SCALER_MEMORY
	unsigned int Allocate_StartAddr;
	unsigned long Allocate_VirAddr;
#endif
	MemBlockStatus Status;		// 0: undefined, 1: allocated from low, 2: allocated from high
#ifdef CONFIG_ARM64
	unsigned long StartAddr;
#else
	unsigned int StartAddr;
#endif
	unsigned int Size;
}StructMemBlock;

// Memory Index
typedef enum {
    MEMIDX_VDC=0x0,
    MEMIDX_TT,
    MEMIDX_JPEG,
    MEMIDX_DI,
    MEMIDX_MAIN,
    MEMIDX_MAIN_SEC,
    MEMIDX_MAIN_THIRD,
    MEMIDX_MAIN_FOURTH,
    MEMIDX_MAIN_FIFTH,
    MEMIDX_MAIN_SIXTH,
    MEMIDX_MAIN_SEVENTH,
    MEMIDX_MAIN_EIGHTH,
    MEMIDX_SUB,
    MEMIDX_SUB_SEC,
    MEMIDX_SUB_THIRD,
    MEMIDX_SUB_FOURTH,
    MEMIDX_SUB_FIFTH,
    MEMIDX_SUB_SIXTH,
    MEMIDX_SUB_SEVENTH,
    MEMIDX_SUB_EIGHTH,
    MEMIDX_MAIN_C,
    MEMIDX_MAIN_C_SEC,
    MEMIDX_MAIN_C_THIRD,
    MEMIDX_MAIN_C_FOURTH,
    MEMIDX_MAIN_C_FIFTH,
    MEMIDX_MAIN_C_SIXTH,
    MEMIDX_MAIN_C_SEVENTH,
    MEMIDX_MAIN_C_EIGHTH,
    MEMIDX_SUB_C,
    MEMIDX_SUB_C_SEC,
    MEMIDX_SUB_C_THIRD,
    MEMIDX_SUB_C_FOURTH,
    MEMIDX_SUB_C_FIFTH,
    MEMIDX_SUB_C_SIXTH,
    MEMIDX_SUB_C_SEVENTH,
    MEMIDX_SUB_C_EIGHTH,
    MEMIDX_MAIN_2,
    MEMIDX_MAIN_2_SEC,
    MEMIDX_MAIN_2_THIRD,
    MEMIDX_DI_2,
    MEMIDX_ALL,
    MEMIDX_UNKNOW
}eMemIdx;

typedef struct{
	UINT32 display;
	UINT32 bOnOff;
	UINT32 compbit;
	UINT32 GameMode;
	UINT32 RGB444Mode;
	UINT32 LivezoomMode;
	UINT32 compmode;
}M_DOMAIN_COMPRESSION_T;

//compression bits
typedef enum {
	COMPRESSION_10_BITS = 10,
	COMPRESSION_11_BITS = 11,
	COMPRESSION_12_BITS = 12,
	COMPRESSION_13_BITS = 13,
	COMPRESSION_14_BITS = 14,
	COMPRESSION_15_BITS = 15,
	COMPRESSION_16_BITS = 16,
	COMPRESSION_17_BITS = 17,
	COMPRESSION_18_BITS = 18,
	COMPRESSION_19_BITS = 19,
	COMPRESSION_20_BITS = 20,
	COMPRESSION_21_BITS = 21,
	COMPRESSION_22_BITS = 22,
	COMPRESSION_23_BITS = 23,
	COMPRESSION_24_BITS = 24,
	COMPRESSION_25_BITS = 25,
	COMPRESSION_26_BITS = 26,
	COMPRESSION_27_BITS = 27,
	COMPRESSION_28_BITS = 28,
	COMPRESSION_29_BITS = 29,
	COMPRESSION_NONE_BITS = 30,
}MemCompression_Bits;

//compression mode
typedef enum {
	COMPRESSION_LINE_MODE = 0,
	COMPRESSION_FRAME_MODE = 1,
	COMPRESSION_NONE_MODE = 2,
}MemCompression_Mode;

typedef enum
{
	MEMCAPTYPE_LINE = 0x00,
	MEMCAPTYPE_FRAME = 0x01
}eMemCapAccessType;

typedef enum _SLR_DMA_mode{
	SLR_DMA_422_mode 			= 0,
	SLR_DMA_420_mode 			= 1,
	SLR_DMA_400_mode 			= 2,
} SLR_DMA_mode;

// Check for error during M domain setting flow
typedef enum
{
	MDOMAIN_SETTING_NO_ERR = 0,
	MDOMAIN_SETTING_ERR_MAIN_4K120_NO_MEM,
	MDOMAIN_SETTING_ERR_MAIN_NO_MEM,
	MDOMAIN_SETTING_ERR_MAIN_CMA_ALLOC_FAIL,
	MDOMAIN_SETTING_ERR_MAIN_CMA_BORROW_FROM_MEMC_FAIL,
	MDOMAIN_SETTING_ERR_SUB_4K120_NO_MEM,
}eMDomainSettingErrStatus;

typedef enum {
	MEMALIGN_BANK0 = 0,
	MEMALIGN_BANK1,
	MEMALIGN_BANK2,
	MEMALIGN_BANK3,
	MEMALIGN_ANY,
} eMemAlignment;

//mac5p start to use query driver to get memory and address @Crixus 20170504
typedef enum {
	QUERY_IDX_MDOMAIN_MAIN = 0,
	QUERY_IDX_MDOMAIN_MAIN_I2R,
	QUERY_IDX_DI,
	QUERY_IDX_DI_2,
	QUERY_IDX_VIP_DMAto3DTABLE,
	QUERY_IDX_VIP_DeMura,
	QUERY_IDX_VIP_NN,
    QUERY_IDX_I3DDMA,
	QUERY_IDX_MAX,
} MEMORY_QUERY_IDX;

typedef enum _M_DOMAIN_BORROW_MEMORY_TYPE {
	BORROW_FROM_VBM = 0,
	BORROW_FROM_MEMC,
	BORROW_FROM_CMA,
}_M_DOMAIN_BORROW_MEMORY_TYPE;

// Memory Index for CMA malloc
typedef enum {
	MEMIDX_CMA_FIRST=0x0,
	MEMIDX_CMA_UNKNOWN
}eMemIdx_CMA;

// Memory Index for Mdomain borrow memory from MEMC in CMA case
typedef enum {
	MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_0=0x0,
	MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_1,
	MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_2,
	MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_3,
	MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_4,
	MEMIDX_MDOMAIN_BORROW_MEMC_BUFFER_INDEX_UNKNOWN
}eMemIdx_CMA_MDOMAIN_BORROW_FROM_MEMC;

// Where does the additional CMA buffer comes from.
typedef enum {
	ADDITIONAL_CMA_DCMT_I3DDMA=0x0,
	ADDITIONAL_CMA_DCMT_VDEC_DATA_FRC,
	ADDITIONAL_CMA_DCMT_VDEC_DATA_FRC_OR_MDOMAIN_CMA,
	ADDITIONAL_CMA_DCMT_MDOMAIN_BORROW_MEMC_BUFFER,
	ADDITIONAL_CMA_DCMT_UNKNOWN
}eAdditional_CMA_DCMT;

typedef enum _MDOMAIN_MEMORY_QUERY_ID{
    MDOMAIN_MAIN = 0,
    MDOMAIN_SUB,
    MDOMAIN_MAX,
}MDOMAIN_MEMORY_QUERY_ID;

typedef struct MDOMAIN_MALLOC_STRUCT {
    unsigned char status;
    unsigned long phyAddr;
    unsigned long size;
} MDOMAIN_MALLOC_STRUCT;

// DI PQC
typedef enum _VIP_DeXCXL_Data_MODE {
	DeXCXL_Disable = 0,
	DeXCXL_Y10C6,
	DeXCXL_Y8C8,
	DeXCXL_Y10C6_CMP,
	DeXCXL_Y8C8_CMP,

} VIP_DeXCXL_Data_MODE;

/*================================ Definitions ==============================*/
/*================================== Variables ==============================*/
extern StructMemBlock MemTag[MEMIDX_UNKNOW];
/*================================== Function ===============================*/
// IP bit number control (saving bandwidth)
unsigned int drv_memory_get_ip_Btr_mode(void);
unsigned int drv_memory_get_ip_DMA420_mode(void);
void drv_memory_set_ip_Btr_mode(unsigned int mode);
void drv_memory_set_ip_DMA420_mode(unsigned int mode);
unsigned char drv_I3ddmaMemory_get_game_mode(void);
unsigned int drvif_memory_get_data_align(unsigned int Value, unsigned int unit);
eMemCapAccessType dvrif_memory_get_cap_mode(unsigned char display);
void dvrif_memory_set_cap_mode(unsigned char display, eMemCapAccessType Cap_type);
unsigned char dvrif_memory_compression_get_enable(unsigned char display);
void dvrif_memory_compression_set_enable(unsigned char display, unsigned char enable);
unsigned char dvrif_memory_get_compression_bits(unsigned char display);
void dvrif_memory_set_compression_bits(unsigned char display, unsigned char comp_bits);
unsigned char dvrif_memory_get_compression_mode(unsigned char display);
void dvrif_memory_set_compression_mode(unsigned char display, unsigned char comp_mode);
void drvif_memory_compression_rpc(unsigned char display);
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
void drv_memory_Set_multibuffer_changed(UINT8 enable);
UINT8 drv_memory_Get_multibuffer_changed(void);
void drv_memory_send_multibuffer_number_to_smoothtoogh(void);

unsigned char drv_memory_get_DMA_Aligned_Val(void);
unsigned char drv_memory_get_game_mode_flag(void);
unsigned char drv_memory_get_game_mode(void);
unsigned long get_query_start_address(unsigned char idx);
unsigned int drvif_memory_alloc_block(eMemIdx Idx, unsigned int space, unsigned char Align);
unsigned char get_mdomain_driver_status(void);
void set_m_domain_borrow_buffer(void *ptr, unsigned int size);
void *get_m_domain_borrow_buffer(void);
unsigned int get_m_domain_borrow_size(void);
unsigned char m_domain_4k_memory_from_type(void);
bool is_m_domain_data_frc_need_borrow_memory(void);
bool vdec_data_frc_need_borrow_cma_buffer(unsigned char display);
void CMA_set_m_domain_original_buffer(unsigned long addr, unsigned int size, unsigned char CMA_buffer_index);
unsigned long CMA_get_m_domain_original_buffer(unsigned char CMA_buffer_index);
unsigned int CMA_get_m_domain_original_size(unsigned char CMA_buffer_index);
void CMA_release_m_domain_original_buffer(void);
unsigned char CMA_alloc_m_domain_original_buffer(void);
void CMA_reset_DCMT_impl(unsigned long start_addr, unsigned int size, eAdditional_CMA_DCMT reset_reason);
extern void (*CMA_reset_DCMT)(unsigned long start_addr, unsigned int size, eAdditional_CMA_DCMT reset_reason);
unsigned char CMA_borrow_m_domain_buffer_from_MEMC(eMemIdx_CMA_MDOMAIN_BORROW_FROM_MEMC borrow_index, unsigned long *borrow_addr, unsigned int *borrow_size);
void set_sub_m_domain_1st_buf_address(void *addr);
void *get_sub_m_domain_1st_buf_address(void);
void set_sub_m_domain_2nd_buf_address(void *addr);
void *get_sub_m_domain_2nd_buf_address(void);
void set_sub_m_domain_3rd_buf_address(void *addr);
void *get_sub_m_domain_3rd_buf_address(void);
void set_m_domain_setting_err_status(unsigned char display, eMDomainSettingErrStatus err_status);
unsigned int get_m_domain_setting_err_status(unsigned char display);

unsigned int drvif_memory_get_memtag_startaddr(UINT8 index);
void drvif_memory_set_memtag_startaddr(unsigned int addr,UINT8 index);
unsigned long drvif_memory_get_memtag_virtual_startaddr(UINT8 index);
StructMemBlock* drvif_memory_get_MemTag_addr(UINT8 index);
void drvif_memory_set_memtag_virtual_startaddr(unsigned long addr);
void drvif_memory_free_fixed_virtual_memory(void);
void drvif_memory_allocate_fixed_virtual_memory(void);
void drvif_memory_set_mtag_info(UINT32 buffer_size, UINT8 idx);
unsigned int drvif_memory_get_fixed_virtual_memory_size(void);
void drvif_memory_set_fixed_virtual_memory_size(unsigned int size);
unsigned int drvif_memory_get_block_addr(eMemIdx Idx);
unsigned int drvif_memory_get_block_size(eMemIdx Idx);

void main_mdomain_ctrl_semaphore_init(void);
struct semaphore *get_main_mdomain_ctrl_semaphore(void);
void drv_memory_set_vdec_direct_low_latency_mode(unsigned char enable);
unsigned char drv_memory_get_vdec_direct_low_latency_mode(void);
bool is_hdmi_dolby_vision_sink_led(void);
unsigned char drv_memory_get_low_delay_game_mode_dynamic(void);
void drv_memory_set_game_mode(unsigned char enable);
void drv_memory_set_game_mode_dynamic(unsigned char enable);
void set_mdomain_driver_status(unsigned char status);

char  drv_memory_I_De_XC_ON_OFF_Wait(unsigned char En);
unsigned int  drv_memory_I_De_XC_DMA_Memory_Get_From_VBM(unsigned char DI_MEM_IDX, unsigned int DI_Width, unsigned int DI_Height, unsigned char Force_Clr);
unsigned int  drv_memory_I_De_XC_DMA_Config(unsigned int DI_phyAddr, unsigned char deXC_mode);
unsigned int drv_memory_Set_I_De_XC_DMA(void *dma_data, unsigned char DI_MEM_IDX);
unsigned char drv_memory_Set_I_De_XC_Mem_Ready_Flag(unsigned char MEM_IDX, unsigned char Flag);
unsigned char drv_memory_Get_I_De_XC_Mem_Ready_Flag(unsigned char MEM_IDX);
unsigned char drv_memory_DeXCXL_Memory_Alloc_Condition_Check(void);

/*============================================================================*/

#endif //#ifndef _SCALER_MEMORY_H_
/*======================== End of File =======================================*/
/**
*
* @}
*/
