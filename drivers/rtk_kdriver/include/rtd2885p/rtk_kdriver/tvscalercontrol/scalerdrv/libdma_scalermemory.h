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

#ifndef _LIBDMA_SCALER_MEMORY_H_
#define _LIBDMA_SCALER_MEMORY_H_

/*============================ Module dependency  ===========================*/
//#include <base_types.h>
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
	#include <rtk_kdriver/scalercommon/scalerCommon.h>
#endif
#include <tvscalercontrol/scalerdrv/scalermemory.h>

#define _WDMA_MAX_OUTSTANDING   (0x3f)    // outstanding value
#define _RDMA_MAX_OUTSTANDING   (0x3f)    // outstanding value
#define _WDMA_BURSTLENGTH   (0x10) //32*128 bits
#define _RDMA_BURSTLENGTH   (0x10) //16*128 bits 

/*===================================  Types ================================*/
typedef enum _eMemFormat{
    MEM_444 = 0,
    MEM_422 = 1,
    MEM_420 = 2
} eMemFormat;

typedef enum _eMemBitNum{
    MEM_8_BIT = 0,
    MEM_10_BIT = 1
} eMemBitNum;

typedef enum
{
	MEM_YC_COMBINE = 0x00,
	MEM_YC_SEPARATE = 0x01
} eMemYCMode;

typedef struct _PixelSizeInfo
{
    eMemYCMode YCMode;
	eMemFormat Format;
	eMemBitNum BitNum;
} PixelSizeInfo;

/*================================ Definitions ==============================*/
/*================================== Variables ==============================*/
/*================================== Function ===============================*/
unsigned int calc_comp_line_sum_bit(unsigned int comp_wid, unsigned int comp_ratio);
void memory_frc_main_set_capture(void);
void libdma_drv_memory_display_set_input_fast(unsigned char display, unsigned char enable);
void libdma_drvif_memory_EnableMCap(unsigned char display);
void libdma_drvif_memory_DisableMCap(unsigned char display);
//void libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(unsigned char display, bool enable);
void libdma_drv_memory_display_check_sub_frc_style(void);
void dvrif_memory_fs_handler(unsigned char display);
void dvrif_memory_frc_handler(unsigned char display);
/*============================================================================*/

#endif //#ifndef _LIBDMA_SCALER_MEMORY_H_
/*======================== End of File =======================================*/
/**
*
* @}
*/
