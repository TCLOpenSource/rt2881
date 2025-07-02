#ifndef _TX_PRE_DISPLAY_H_
#define _TX_PRE_DISPLAY_H_

/*============================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* ===========================================================================*/

//----------------------------------------------------------------------------
// ID Code      : txpre_dtg.h
// Update Note  :
//----------------------------------------------------------------------------
#include <scaler/scalerCommon.h>

#define FLL_TRACKING_LINE_NUM 2
#define MAX_MULTIPLE_SYNTHESISN 9
#define SYNTHESISN 4

#define DPLL_CLK_4K 5940
#define FLL_MULTIPLIER_2K 0x1400
#define FLL_MULTIPLIER_4K 0x7000


void Txpretg_set_disp_setting(StructTxPreInfo *txpre_info);
void Txpretg_reset_to_free_run(void);
void Txpretg_set_display_timing_gen(StructTxPreInfo *txpre_info);
void Txpretg_set_VRR_timing_framesync(StructTxPreInfo *txpre_info);
void Txpretg_set_line_tracking_timing_freerun(StructTxPreInfo *txpre_info);
void Txpretg_set_iv2dv_delay(StructTxPreInfo *txpre_info);
unsigned int Txpretg_get_tx_dpll_clk(StructTxPreInfo *txpre_info);
void Txpretg_set_fix_last_line_tracking(StructTxPreInfo *txpre_info);


#endif
