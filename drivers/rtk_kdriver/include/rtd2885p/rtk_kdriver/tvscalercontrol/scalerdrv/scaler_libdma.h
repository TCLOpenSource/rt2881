/*==========================================================================
  * Copyright (c)      Realtek Semiconductor Corporation, 2024
  * All rights reserved.
  * ========================================================================*/

/*================= File Description =======================================*/
/**
 * @file
 * 	This file is for scaler libdma register control.
 *
 * @author 	$Author$
 * @date		$Date$
 * @version	$Revision$
 * @ingroup	clock
 */

/**
 * @addtogroup clock
 * @{
 */

#ifndef _SCALER_LIBDMA_H_
#define _SCALER_LIBDMA_H_

/*============================ Module dependency  ===========================*/
#include <tvscalercontrol/scalerdrv/scalermemory.h>
/*===================================  Types ================================*/
/*================================== Variables ==============================*/
/*================================ Definitions ==============================*/

void libdma_set_disp_reg_doublebuffer_enable(unsigned char display, unsigned int enable);
void libdma_set_disp_reg_doublebuffer_apply(unsigned char display);
void libdma_dvrif_memory_fs_handler(unsigned char display);
void libdma_dvrif_memory_frc_handler(unsigned char display);
void libdma_drvif_memory_EnableMDisp_Or_DisableMDisp(unsigned char display, unsigned char enable);
void libdma_mdomain_handler_onlycap(void);
void libdma_wait_m_cap_last_write_done(unsigned char display);
void libdma_memory_set_main_displaywindow_change(void);
void libdma_memory_set_sub_displaywindow_change(unsigned char changetomain);
void libdma_set_mdomain_single_buffer_enable(unsigned char display, unsigned char enable, unsigned char single_buffer_num);
void libdma_qs_sync_mdomain_buffer_adder(unsigned char display);
#ifdef BUILD_QUICK_SHOW
void libdma_scaler_quickshow_memery_init(void);
unsigned long libdma_qs_get_memory_from_vbm(MDOMAIN_MEMORY_QUERY_ID id, unsigned long allocSize);
#endif 
#endif // _SCALER_LIBDMA_H_
/*======================== End of File =======================================*/
/**
*
* @}
*/
