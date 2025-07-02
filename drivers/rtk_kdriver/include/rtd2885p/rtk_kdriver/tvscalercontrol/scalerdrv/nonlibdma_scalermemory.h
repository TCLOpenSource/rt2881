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

#ifndef _NONLIBDMA_SCALER_MEMORY_H_
#define _NONLIBDMA_SCALER_MEMORY_H_

#ifdef UT_flag
#include <tvscalercontrol/i3ddma/i3ddma.h>
#endif
/*============================ Module dependency  ===========================*/

//#include "rtd_types.h"
//#include <base_types.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#define CONFIG_DDR_COL_BITS 9
#define CONFIG_DDR_BANK_BITS 2
#define CONFIG_DDR_ROW_BITS 12
#define CONFIG_DDR_DATA_BITS 16
#define CONFIG_MDOMAIN_BURST_SIZE 512

#define GAME_MODE_FRAME_SYNC 0
#define GAME_MODE_NEW_FLL 1

/*===================================  Types ================================*/
// DDR resources, Arbiter customers

typedef enum {
	MEM_CH2_WR = 0,
	MEM_CH1_WR,
	MEM_SUB_RD,
	MEM_MAIN_RD,
	MEM_OSDY_RD,
	MEM_OSDX_RD,
	MEM_MCU_RW,
	MEM_DI_RD4,
	MEM_DI_RD3,
	MEM_DI_RD2,
	MEM_DI_RD1,
	MEM_DI_WR,
}eMemResource;

#ifndef CONFIG_DARWIN_SCALER
typedef enum {
	MEM_BLOCK_USE_NORMAL=0,
	MEM_BLOCK_USE_OPTIMIZE,
	MEM_BLOCK_TOTAL_NUM,
} eMemBlockUsage;
#endif

typedef enum
{
	MEM_SWBUFFER_SWAP_ENABLE = 0,
	MEM_SWBUFFER_SWAP_DISABLE,
	MEM_SWBUFFER_SWAP_UPDATE_INFO,
}eMemCap_SwBufferSwapState;

typedef enum
{
	MEM_3D_SWBUFFER_SWAP_CONDITION_CHECK = 0,
	MEM_3D_SWBUFFER_SWAP_ENABLE,
	MEM_3D_SWBUFFER_SWAP_DISABLE,
	MEM_3D_SWBUFFER_SWAP_UPDATE_INFO,
}eMemCap_3D_Mode_SwBufferSwapState;

typedef enum _ZOOM_INPUT_INFO{
	//ZOOM_INPUT_MEM_ACT_HSTA,
	//ZOOM_INPUT_MEM_ACT_VSTA,
	ZOOM_INPUT_MEM_ACT_LEN,
	ZOOM_INPUT_MEM_ACT_WID,
	ZOOM_INPUT_CAP_LEN,
	ZOOM_INPUT_CAP_WID,
	ZOOM_INPUT_DISP_LEN,
	ZOOM_INPUT_DISP_WID,
	//ZOOM_INPUT_DISP_LEN_PRE,
	//ZOOM_INPUT_DISP_WID_PRE,
}ZOOM_INPUT_INFO;

typedef enum _ZOOM_DISP_STATUS{
	ZOOM_DISP_THRIP 			= _BIT0,
	ZOOM_DISP_422 				= _BIT1,
	ZOOM_DISP_10BIT			= _BIT2,
	ZOOM_DISP_422CAP			= _BIT3,
	ZOOM_DISP_FSYNC		= _BIT4,
	ZOOM_DISP_VUZD 			= _BIT5,
	ZOOM_DISP_FSYNC_VUZD		= _BIT4|_BIT5,
	ZOOM_DISP_COMP				= _BIT6,
	ZOOM_DISP_FREEZE			= _BIT7,
	ZOOM_DISP_INTERLACE	= _BIT8,
	ZOOM_DISP_SHOW				= _BIT9,
	ZOOM_DISP_RTNR				= _BIT10,
	ZOOM_DISP_MA3A				= _BIT11,
	ZOOM_DISP_VFLIP3			= _BIT13,
	ZOOM_DISP_SIZE				= _BIT14,
	ZOOM_DISP_FIRST			= _BIT15,
} ZOOM_DISP_STATUS;

//compression segment
typedef enum {
	SEGMENT_1 = 0,
	SEGMENT_2 = 1,
	SEGMENT_4 = 2,
	SEGMENT_8 = 3,
} MemCompression;

// DI PQC
typedef enum _VIP_DI_PQC_MODE {
	PQC_Disable = 0,
	PQC_Frame_Mode,
	PQC_Line_Mode,
	PQC_Line_Mode_Rolling,
	PQC_Line_Mode_Single_Buffer,

	VIP_DI_PQC_MODE_MAX,
} VIP_DI_PQC_MODE;

/*================================ Definitions ==============================*/
#define _DUMMYADDR					0xFFFFFFFF
#define _DDR_BANK_ADDR_ALIGNMENT	 (1 << (CONFIG_DDR_COL_BITS + CONFIG_DDR_BANK_BITS))		// a ROW start address
#define _DDR_BANK_SIZE					(1<< CONFIG_DDR_COL_BITS)
#define _DDR_BITS						CONFIG_DDR_COL_BITS
#define _DDR_ROW_BITS					CONFIG_DDR_COL_BITS + CONFIG_DDR_BANK_BITS
#define _DDR_END_ADDR					((1 << (CONFIG_DDR_COL_BITS + CONFIG_DDR_BANK_BITS + CONFIG_DDR_ROW_BITS))	- 1)

//96-bytes align define @Crixus 20180418
#if 1
//Update the fifo depth for each IC @Crixus 20170418
#if 1/* merlin */
#define MERLIN_FIFO_DEPTH
#define _DISPLAY_OUTSTANDING                (0xf)    // outstanding value
#define _BURSTLENGTH_MDISPLAY_OUTSTANDING   (512/(_DISPLAY_OUTSTANDING+1))  // 256/outstanding   * 8 bytes align 96 bytes.
#define _CAPTURE_OUTSTANDING                (0x7)    // outstanding value
#define _BURSTLENGTH_MCAP_OUTSTANDING       (512/(_CAPTURE_OUTSTANDING+1))  // 256/outstanding   * 8 bytes align 96 bytes.
#define _BURSTLENGTH   (0x84)   // 0x84 (132) * 8 bytes align 96 bytes.
#define _BURSTLENGTH2  (0x78)   // 0x78 (120) for sub-channel
#define _FIFOLENGTH    (0x200)  // FIFO len= 0x200 in Magellan2 //main path FIFO
#define _FIFOLENGTH2   (0x200)  //sub path FIFO
#else/* mac */
#define MAC_FIFO_DEPTH
#define _BURSTLENGTH   (0x84)  //burst length = 64
#define _BURSTLENGTH2  (0x78)  // 0x78 (120) for sub-channel
#define _FIFOLENGTH    (0x100) //fifo length = 256 for mac5p
#define _FIFOLENGTH2   (0x80)  //sub path FIFO
#endif
#else
//Update the fifo depth for each IC @Crixus 20170418
#if 1/* merlin */
#define MERLIN_FIFO_DEPTH
#define _BURSTLENGTH   (0x80)  // 0xf0 (240) for Magellan2 HDMI 4k2k60
#define _BURSTLENGTH2  (0x78)  // 0x78 (120) for sub-channel
#define _FIFOLENGTH    (0x200) // FIFO len= 0x200 in Magellan2 //main path FIFO
#define _FIFOLENGTH2   (0x80)  //sub path FIFO
#else/* mac */
#define MAC_FIFO_DEPTH
#define _BURSTLENGTH   (0x80)  //burst length = 64
#define _BURSTLENGTH2  (0x78)  // 0x78 (120) for sub-channel
#define _FIFOLENGTH    (0x100) //fifo length = 256 for mac5p
#define _FIFOLENGTH2   (0x80)  //sub path FIFO
#endif
#endif

#if 1//mac3 no in2
#define FrcFreeze()	IoReg_SetBits(MDOMAIN_CAP_DDR_In1Ctrl_reg,_BIT1);
#define FrcDeFreeze()	IoReg_ClearBits(MDOMAIN_CAP_DDR_In1Ctrl_reg,_BIT1);
#else
#define FrcFreeze()	IoReg_SetBits(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : MDOMAIN_CAP_DDR_In1Ctrl_reg,_BIT1);
#define FrcDeFreeze()	IoReg_ClearBits(Scaler_DispGetInputInfo(SLR_INPUT_CHANNEL) ? MDOMAIN_CAP_DDR_In2Ctrl_reg : MDOMAIN_CAP_DDR_In1Ctrl_reg,_BIT1);
#endif
#define Mer2_MEMORY_ALLOC
/*================================== Variables ==============================*/
/*================================== Function ===============================*/
/*============================================================================*/
/**
 * drvif_memory_free_block
 * Free memory of the specific tag and insert it to free block information
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { none }
 *
 */
void drvif_memory_free_block(eMemIdx Idx);

/*============================================================================*/
/**
 * MemGetBlockAddr
 * Get start address of allocated memory
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { none }
 *
 */
unsigned int drvif_memory_get_block_addr(eMemIdx Idx);

/*============================================================================*/
/**
 * drvif_memory_get_block_size
 * Get size of allocated memory
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { block size }
 *
 */
unsigned int drvif_memory_get_block_size(eMemIdx Idx);


/*============================================================================*/
/**
 * drvif_memory_alloc_block
 * allocate memory of the specific tag
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { start address of the allocated memory }
 *
 * @Note:
 *  You must drvif_memory_free_block to free block you allocated before calling this function to allocate memory space.
 *  It possible wrong operatoin if you don't observer the front.
 */
unsigned int drvif_memory_alloc_block(eMemIdx Idx, unsigned int space, unsigned char Align);


/*============================================================================*/
/**
 * MemAlignBank
 * Memory alignment for DDR bank0
 *
 * @param <index> { index of DDR memory space, see eMemIdx for details }
 * @return { none }
 *
 */
unsigned int drvif_memory_alignbank(unsigned int StartAddr, unsigned char AlignBank, unsigned char Frm);


/*============================================================================*/
/**
 * dvrif_memory_handler
 * This function is used to handler memory (page 3).
 *
 * @param <info> { display-info struecture }
 * @return { none }
 *
 */
void dvrif_memory_handler(unsigned char display);

/*============================================================================*/
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
void drvif_memory_init(void);

//Release Scaler have allocated memory
void drvif_memory_release(void);
//get scaler memory base physical address
unsigned int get_scaler_memory_base_addr(void);
//Reallocate Scaler d memory
void drvif_memory_reallocate(void);
unsigned char drvif_memory_switch_mem_block(eMemBlockUsage mem_block_usage);
unsigned char drvif_scaler_getValidMemInfo(SCALERDRV_ALLOCBUFFER_INFO *pAllocBufferInfo);
int drvif_scaler_getValidMemInfo_fromVideo(SCALERDRV_ALLOCBUFFER_INFO *pAllocBufferInfo);
unsigned char drvif_scaler_releaseValidMem(void);
unsigned int get_scaler_memory_map_addr(void);
unsigned int get_scaler_memory_map_uncacheaddr(void);

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
void drv_memory_disable_di_write_req(unsigned char disableDiWr);

/*============================================================================*/
/**
 * drv_memory_set_ip_fifo
 * Set IP (video process) memory FIFO registers according to its FIFO-type
 *
 * @param <info> { display-info struecture }
 * @param <access> { 5A => 1, 3A => 0}
 * @return { none }
 * @note
 *
 */
void drv_memory_set_ip_fifo(unsigned char access);


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
unsigned int drvif_memory_get_data_align(unsigned int Value, unsigned int Unit);


/*============================================================================*/
/**
 * drvif_memory_bandwidth_check
 * Check DDR bandwidth used in the present situation
 *
 * @param <display> {Display for Main or Sub}
 * @return { none }
 *
 */
unsigned int drvif_memory_bandwidth_check(unsigned char display);

void drvif_memory_set_fs_display_fifo(void);

#ifdef CONFIG_DUAL_CHANNEL  // There are two channels
/*============================================================================*/
/**
 * drvif_memory_rotate_mp
 * Set active MP and current address for MP rottion
 *
 * @param <MpIdx> {MP window to be active}
 * @return { none }
 * @note
 * It can be called in sub display
 *
 */
void drvif_memory_rotate_mp(unsigned short MpIdx);

void drvif_memory_initMp(void);
#endif


void drvif_memory_set_dbuffer_write(unsigned char display);
void drvif_memory_wait_dbuffer_write(unsigned char display);
void Memory_SetCapture_AfterFreeze(unsigned char bDouble);
//unsigned char drvif_memory_regAvPresetTable(unsigned short *ptr);
void memory_check_capture_polarity_error(unsigned char channel);
unsigned int memory_get_line_size(unsigned int LineSize, unsigned char InputFormat, unsigned char BitNum);
void memory_division(unsigned int Dividend, unsigned char Divisor, unsigned int* Quotient, unsigned char* Remainder);
void memory_set_capture_fifo(unsigned char channel, unsigned int Quotient, unsigned char Remainder, unsigned char WaterLevel);
void memory_set_input_format(unsigned char display, unsigned char bDouble);
void memory_set_input_format2(unsigned char display, unsigned char bDouble);
#ifdef CONFIG_DUMP_MEMTAG
void drvif_memory_dump_MemTag(void);
#endif

void zoom_memory_set_capture(unsigned char bDouble, eMemCapAccessType Cap_type, unsigned char buffernum);
void zoom_memory_set_main(unsigned char bDouble,unsigned char buffernum);
void zoom_set_input_format(unsigned char display, unsigned char bDouble);
void memory_set_main_activewindow_change(void);
void memory_set_main_displaywindow_change(void);
void memory_set_sub_displaywindow_change(unsigned char changetomain);


void jpeg_memory_alloc_capture2(eMemCapAccessType Cap_type,eMemIdx idx);
void jpeg_memory_free_capture2(void);
/*void drvif_memory_set_osd_disp_mem(StructJPEGDisplayInfo *jpeg_info);*/

void memory_set_adc_capture(unsigned int CapBuf_addr, unsigned int Hight,unsigned int width);
void get_adc_dump_addr_size(unsigned int *PhyAddr, unsigned int *VirStartAddr,unsigned int *Size , unsigned int *Width, unsigned int* Height);
void get_frc_buf_addr_size(unsigned int *PhyAddr, unsigned int *VirStartAddr,unsigned int *Size , unsigned int *Width, unsigned int* Height);
void dump_frc_rawdata_start(void);
void dump_frc_rawdata_release(void);

void get_vdc_dump_addr(unsigned int *PhyAddr, unsigned int *VirStartAddr);
void drv_memory_set_ip_fifo_for_2Dcvt3D(unsigned char access);
void memory_set_capture_for_2Dcvt3D(void);
void memory_set_vflip_capture(unsigned char bVflip, eMemCapAccessType Cap_type);
void zoom_memory_set_vflip_main(unsigned char bVflip,unsigned char buffernum);
unsigned int memory_get_capture_size(unsigned char display, eMemCapAccessType AccessType);

//void memory_3ddma_control_OSD(void);
//void memory_3ddma_control(void);
//void memory_3ddma_disable(void);
//void memory_3ddma_LRSWAP(void);

// IP bit number control (saving bandwidth)
void drvif_memory_set_3D_LR_swap(unsigned char lr_swap, unsigned char type_3d);
#ifdef ENABLE_SG_3D_SUPPORT_ALL_SOURCE_3D_MODE // SG 3D PANEL SUPPORT ALL SOURCE 3D MODE
void drvif_memory_set_3D_LR_swap_for_SG_frc_in_mDomain(unsigned char bInvMode);
#endif
void drvif_memory_set_MVC_FRC_LR_swap(unsigned char bInvMode);
unsigned char drvif_memory_sw_buffer_swap_Enable(SCALER_DISP_CHANNEL ucDisplay);
//void drvif_memory_sw_buffer_swap_action(SCALER_DISP_CHANNEL ucDisplay, eMemCap_SwBufferSwapState state);
//void dvrif_memory_set_addr_check(UINT8 Display, UINT8 uc3Buffer);
void dvrif_memory_set_frc_style(unsigned char Display, unsigned char ucEnable);
UINT32 MemGetDDRLineWidth1(UINT8 display,UINT32 width);
UINT32 memory_get_disp_line_size1(UINT8 display,UINT32 width);
UINT32 memory_get_disp_line_size_compression(UINT8 display,UINT32 width);
void MemGetBlockShiftAddr(UINT8 display,UINT16 HOffset, UINT16 VOffset,UINT32 * StartAddr, UINT16 *DropAddr);

#ifdef CONFIG_MPIXEL_SHIFT
void memory_v_shift(int v);
void memory_black_buf_init(unsigned int framesize,unsigned int  upperPhyAddress);
#endif

unsigned int drvif_memory_get_3d_startaddr(unsigned char index);
void drvif_memory_set_3d_startaddr(unsigned int addr,unsigned char index);
unsigned int drvif_memory_get_memtag_startaddr(unsigned char index);
void drvif_memory_set_memtag_startaddr(unsigned int addr,unsigned char index);
StructMemBlock* drvif_memory_get_MemTag_addr(unsigned char index);

void memory_set_main_SG_3D_setting(unsigned int LineStep, unsigned int Addr1, unsigned int Addr2, unsigned int Addr3);
void memory_set_MainandSub_ShowONPR(void);

//Mac new adding block mode related driver
//Crixus@20140808
void drvif_memory_block_mode_enable(bool enable);
void drvif_memory_block_mode_set_YC_index(unsigned int Y_index, unsigned int C_index);
void drvif_memory_block_set_Y_offset(unsigned int Y_V_offset, unsigned int Y_H_offset, unsigned int Y_H_shift);
void drvif_memory_block_set_C_offset(unsigned int C_V_offset, unsigned int C_H_offset);
unsigned char dvrif_memory_compression_get_enable(unsigned char display);
void dvrif_memory_compression_set_enable(unsigned char display, unsigned char enable);
unsigned char dvrif_memory_get_compression_bits(unsigned char display);
void dvrif_memory_set_compression_bits(unsigned char display, unsigned char comp_bits);
void dvrif_memory_comp_setting(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio);
void dvrif_memory_comp_setting_sub(UINT8 enable_compression, UINT16 comp_wid, UINT16 comp_len, UINT8 comp_ratio);
void mdomain_handler_onlycap_i2run(void);
void mdomain_handler_onlycap(void);
void drvif_memory_compression_rpc(unsigned char display);


unsigned long drvif_memory_get_mainblock_viraddr(void);
unsigned long drvif_memory_get_subblock_viraddr(void);
unsigned int drv_memory_di_data_size_get(unsigned char access,unsigned int source_width);
unsigned int drv_memory_di_alloc_buffer_get(void);
void drv_memory_di_alloc_buffer_set(unsigned char source,unsigned char data_10bit);
unsigned int drv_memory_di_info_size_get(unsigned char access);
unsigned int drv_memory_di_buffer_get(void);
#ifdef Mer2_MEMORY_ALLOC
char drv_memory_di_Set_PQC_init(void);
char drv_memory_di_Set_Compression_Bit_Num(unsigned char Bit_Num, unsigned char index);
unsigned char drv_memory_di_Get_Compression_Bit_Num(unsigned char index);
char drv_memory_di_Set_Compression_Mode(unsigned char Mode, unsigned char index);
unsigned char drv_memory_di_Get_Compression_Mode(unsigned char index);
unsigned char drv_memory_di_Decide_PQC_mode(unsigned char src_idx, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_DMA_Mode, unsigned char index);
unsigned char drv_memory_di_Decide_PQC_Limit_Bit(unsigned char src_idx, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_DMA_Mode, unsigned char index);
char drv_memory_di_Compression_Config(unsigned char src_idx, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_DMA_Mode, unsigned char index);
unsigned int drv_memory_Get_di_data_size_Static(unsigned char src_idx, unsigned char isCompression, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_MEM_IDX);
unsigned int drv_memory_Get_di_alloc_buffer_Static(unsigned char src_idx, unsigned char isCompression, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_MEM_IDX);
unsigned int drv_memory_Get_di_info_size_Static(unsigned char src_idx, unsigned char isCompression, unsigned char access, unsigned int DI_Width, unsigned int DI_Height, unsigned char DI_MEM_IDX);
unsigned int drv_memory_set_ip_fifo_Mer2(unsigned char access);
void drv_memory_set_De_Mura(unsigned int memory_Addr_sta);
unsigned char drv_memory_Set_I_BLK_decontour_Mem_Ready_Flag(unsigned char MEM_IDX, unsigned char Flag);
unsigned char drv_memory_Get_I_BLK_decontour_Mem_Ready_Flag(unsigned char MEM_IDX);
unsigned int  drv_memory_blk_decontour_Config(unsigned int phyAddr);// ml8_add
//unsigned char drv_memory_get_DMA_Aligned_Val(void);// ml8_add
unsigned int  drv_memory_MA_SNR_Config(unsigned char access ,unsigned int phyAddr, unsigned char DI_MEM_IDX);
char  drv_memory_MA_SNR_Enable_Check(unsigned char access ,unsigned int phyAddr, unsigned char DI_MEM_IDX, unsigned int DI_Width, unsigned int DI_Height);
char  drv_memory_MA_SNR_Disable(unsigned char MEM_idx);
unsigned char drv_memory_Set_MASNR_Mem_Ready_Flag(unsigned char MEM_IDX, unsigned char Flag);
unsigned char drv_memory_Get_MASNR_Mem_Ready_Flag(unsigned char MEM_IDX);
unsigned int  drv_memory_Get_MA_SNR_Info_Size_Static(unsigned char access, unsigned int I_Width, unsigned int I_Height, unsigned char DI_MEM_IDX);
#endif
unsigned long drvif_memory_get_memtag_virtual_startaddr(UINT8 index);
void drvif_memory_free_fixed_virtual_memory(void);
void drvif_memory_set_fixed_virtual_memory_size(unsigned int size);
unsigned int drvif_memory_get_fixed_virtual_memory_size(void);
void drvif_memory_set_memtag_virtual_startaddr(unsigned long addr);
void drvif_memory_set_mtag_info(UINT32 buffer_size, UINT8 idx);
void drvif_memory_allocate_fixed_virtual_memory(void);
#if 0 //move to common - sharememory.h
void drv_memory_set_game_mode(unsigned char enable);
unsigned char drv_memory_get_game_mode(void);
unsigned char drv_memory_get_game_mode_flag(void);
unsigned char drv_I3ddmaMemory_get_game_mode(void);
#endif
void drv_memory_set_game_mode_dynamic(unsigned char enable);
unsigned char drv_memory_get_game_mode_dynamic(void);
unsigned char drv_memory_get_game_mode_dynamic_flag(void);
unsigned char drv_memory_get_low_delay_game_mode_dynamic(void);
void drv_memory_SingleBuffer_GameMode(unsigned char enable);
void drv_memory_SingleBuffer_GameMode_Dynamic(unsigned char enable);
void drv_GameMode_iv2dv_delay(unsigned char enable);
unsigned int drv_GameMode_decided_iv2dv_delay_old_mode(void);
unsigned int drv_GameMode_decided_iv2dv_delay_new_mode(void);
unsigned int drv_GameMode_iv2dv_delay_compress_margin(unsigned int iv2dv_ori);
void drv_GameMode_adjust_dtgM2GoldenVsync_delay(unsigned char bIsEnterGameMode);
void drv_memory_capture_multibuffer_control(unsigned char buf_num, unsigned char disp2cap_distance);
void drv_memory_capture_multibuffer_address(unsigned int buf4_addr, unsigned int buf5_addr,unsigned int buf6_addr,unsigned int buf7_addr,unsigned int buf8_addr);
void drv_memory_display_multibuffer_control(unsigned char buf_num, unsigned char cap2disp_distance);
void drv_memory_display_multibuffer_address(unsigned int buf4_addr, unsigned int buf5_addr,unsigned int buf6_addr,unsigned int buf7_addr,unsigned int buf8_addr);
void drv_memory_freeze_fw_mode(unsigned char display, unsigned char enable, unsigned char freeze_block);
void mdomain_handler_i2rnd_mcap_sdisp(void);
void i2rnd_default_allocate_mcap1(void);
void i2rnd_default_allocate_mcap2(void);
void i2rnd_default_allocate_DI1(void);
void i2rnd_default_allocate_DI2(void);
void i2rnd_get_mcap2_address(unsigned int *addr1, unsigned int *addr2, unsigned int *addr3);
#if 1//def CONFIG_REALTEK_2K_MODEL_ENABLED
void drv_memory_Set_multibuffer_flag(UINT8 enable);
UINT8 drv_memory_Get_multibuffer_flag(void);
void drv_memory_Set_multibuffer_number(UINT8 number);
UINT8 drv_memory_Get_multibuffer_number(void);
void drv_memory_Set_capture_multibuffer_flag(UINT8 enable);
UINT8 drv_memory_Get_capture_multibuffer_flag(void);
void drv_memory_Set_multibuffer_size(unsigned int buffer_size);;
void drv_memory_Set_multibuffer_changed(UINT8 enable);
UINT8 drv_memory_Get_multibuffer_changed(void);
unsigned int drv_memory_Get_multibuffer_size(void);
void drv_memory_capture_multibuffer_update_addr(unsigned int buffer_size, unsigned int start_addr);
void drv_memory_capture_multibuffer_update_boundary(unsigned char buf_num, unsigned int buffer_size);
void drv_memory_cap_multibuffer_limit_boundary_setting(unsigned char display, unsigned int buffer_size);
void drv_memory_disp_multibuffer_limit_boundary_setting(unsigned char display, unsigned int buffer_size);
void drv_memory_send_multibuffer_number_to_smoothtoogh(void);
void drv_memory_capture_multibuffer_switch_buffer_number(void);
void drv_memory_display_multibuffer_switch_buffer_number(void);
void drv_memory_Set_multibuffer_number_pre(UINT8 number);
UINT8 drv_memory_Get_multibuffer_number_pre(void);
#endif
void check_and_set_mdomain_input_fast(unsigned char display);
void drv_memory_display_set_input_fast(unsigned char display, unsigned char enable);
bool get_mdomain_input_fast_status(unsigned char display);
void wait_m_cap_last_write_done(unsigned char display);
void drv_memory_wait_disp_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case);
void drv_memory_wait_cap_last_write_done(unsigned char display, unsigned char wait_frame, unsigned char game_mode_case);
unsigned char drv_memory_get_game_mode_iv2dv_slow_enable(void);
void drv_memory_wait_mdom_vi_last_write_done(unsigned char display, unsigned char wait_frame);
unsigned char dvrif_memory_get_compression_mode(unsigned char display);
void dvrif_memory_set_compression_mode(unsigned char display, unsigned char comp_mode);
void drv_memory_GameMode_Switch_SingleBuffer(void);
void drv_memory_GameMode_Switch_TripleBuffer(void);
void drv_game_mode_timing_Dynamic(unsigned char enable);
void drv_game_mode_disp_smooth_variable_setting(UINT8 disp_type);
void memory_set_memory_size(unsigned char bDouble, eMemCapAccessType Cap_type);
void dvrif_memory_setting_for_data_fs(void);
void CMA_release_m_domain_original_buffer(void);
unsigned char CMA_alloc_m_domain_original_buffer(void);
extern void (*CMA_reset_DCMT)(unsigned long start_addr, unsigned int size, eAdditional_CMA_DCMT reset_reason);

/*Merlin4 new driver @Crixus 20180320*/
void drv_memory_set_limit_boundary_mode(unsigned char display, unsigned char enable);
unsigned char drv_memory_get_limit_boundary_mode(unsigned char display);
void drv_memory_limit_boundary_enable(unsigned char display, unsigned char enable);
void drv_memory_cap_limit_boundary_setting(unsigned char display, unsigned int buffer_size);
void drv_memory_disp_limit_boundary_setting(unsigned char display);

void Scaler_memory_setting_through_CMA(void);
//unsigned int drv_memory_set_ip_memory_size(unsigned char access);

eMemCapAccessType dvrif_memory_get_cap_mode(unsigned char display);
void dvrif_memory_set_cap_mode(unsigned char display, eMemCapAccessType Cap_type);

void record_current_m_cap_block(unsigned char display);//call by vgip start. we need record at vgip start and use at vgip end. avoid the status has risk at vgip end
unsigned char get_current_m_cap_block(unsigned char display);//return cur_main_m_cap_block
void clear_m_cap_done_status(unsigned char display);////use to clear capture status at vgip start


void set_mdomain_driver_status(unsigned char status);//set m domain driver ready or not
unsigned char get_mdomain_driver_status(void);//decide m domain driver finish or not

void drv_memory_set_vdec_direct_low_latency_mode(unsigned char enable);
unsigned char drv_memory_get_vdec_direct_low_latency_mode(void);



//VRR driver use
void dvrif_LowDelay_memory_handler(unsigned char b_switchToSingleBuffer);
void drv_memory_debug_print_buf_idx(void);
unsigned char drv_memory_get_mbuffer_setting(void);
void drv_memory_display_set_sub_frc_style(void);
unsigned char pc_mode_borrow_di_memory_case(void);//return false: only use main m domain(12bit shiif is enough)   return true: need to use DI memory(16bit shiif)
unsigned char check_mdomain_di_memory_connection(void);//return false: di and m domain memory is not connected. return true:connect
unsigned char pc_mode_mdomain_addr_must_use_di_start_addr(void);//return TRUE:must use di start address.
unsigned char sub_use_main_buffer_case(void);
void main_mdomain_ctrl_semaphore_init(void);//init Main_Mdomain_ctrl_Semaphore
struct semaphore *get_main_mdomain_ctrl_semaphore(void);//get Main_Mdomain_ctrl_Semaphore
void drv_memory_display_check_sub_frc_style(void);
unsigned int drv_Calculate_m_pre_read_value(void);
void set_main_pre_read_v_start(unsigned int m_pre_read);
unsigned int drv_get_caveout_buffer_size_by_platform(unsigned int pixel_bits);
void mcap_outstanding_ctrl(unsigned char display, unsigned char enable);
bool is_2k_memory_buffer_source(SCALER_DISP_CHANNEL display);
unsigned long get_query_start_address(unsigned char idx);
 void drv_memory_set_sub_gate_vo1(UINT8 a_bEnable);
void drv_memory_set_vo_gating_threshold(void);
#ifdef BUILD_QUICK_SHOW
	void Scaler_disp_setting_QS(unsigned char display);
#endif


//unsigned char m_domain_4k_memory_from_type(void);
//bool is_m_domain_data_frc_need_borrow_memory(void);
//bool vdec_data_frc_need_borrow_cma_buffer(unsigned char display);

#ifdef UT_flag
I3DDMA_3DDMA_CTRL_T* drvif_memory_GetI3ddmaCtrl(void);
void memory_set_mdomain_query_addr_for_UT(unsigned char idx, unsigned int a_ulAddr);
#endif
void set_sub_m_domain_1st_buf_address(void *addr);
void *get_sub_m_domain_1st_buf_address(void);
void set_sub_m_domain_2nd_buf_address(void *addr);
void *get_sub_m_domain_2nd_buf_address(void);
void set_sub_m_domain_3rd_buf_address(void *addr);
void *get_sub_m_domain_3rd_buf_address(void);
void set_m_domain_setting_err_status(unsigned char display, eMDomainSettingErrStatus err_status);
unsigned int get_m_domain_setting_err_status(unsigned char display);
void memory_set_capture(unsigned char bDouble, eMemCapAccessType Cap_type, unsigned char display);
void memory_set_main(unsigned char bDouble, unsigned char display);

void drv_memory_Set_multibuffer_flag(UINT8 enable);
void drv_memory_Set_multibuffer_number(UINT8 number);
UINT8 drv_memory_Get_multibuffer_number(void);
void Set_Val_drvif_memory_MemTag_VirAddr(unsigned int a_ulIndex, unsigned int a_ulAddr);

unsigned int memory_get_capture_line_size_compression(unsigned char display, unsigned char compression_bits);
unsigned int drvif_memory_GetRealBufSize(unsigned char display);
unsigned int drvif_memory_GetMemTag_size(unsigned int a_ulIndex);
unsigned int drvif_memory_GetMemTag_addr(unsigned int a_ulIndex);
unsigned int Get_Val_drvif_memory_MemTag_VirAddr(unsigned int a_ulIndex);
unsigned int drv_GetIv2DvDelayTarget(void);
void drvif_memory_EnableMDisp_Or_DisableMDisp(unsigned char display, bool enable);
void drvif_memory_EnableMCap(unsigned char display);
void drvif_memory_DisableMCap(unsigned char display);
void wait_sub_disp_double_buffer_apply(void);

extern unsigned long (*Get_Val_domain_2k_memory_addr)(void);
unsigned int memory_get_capture_frame_size_compression(unsigned char display, unsigned char compression_bits);
void drvif_sub_mdomain_for_srnn(unsigned char enable);
unsigned int get_m_domain_setting_err_status(unsigned char display);
unsigned long qs_get_memory_from_vbm(MDOMAIN_MEMORY_QUERY_ID id, unsigned long allocSize);
void scaler_quickshow_memery_init(void);

#endif //#ifndef _NONLIBDMA_SCALER_MEMORY_H_

/*======================== End of File =======================================*/
/**
*
* @}
*/

