#ifndef _SRNN_H_
#define _SRNN_H_

#include <tvscalercontrol/vip/srnn_table.h>
/*******************************************************************************
*Definitions*
******************************************************************************/
//typedef unsigned char UINT8;
//typedef unsigned short UINT16;
//typedef unsigned int UINT32;
#if 0	// move to vipcommon
#define SRNN_Ctrl_Table_MAX 6

/*******************************************************************************
 * Structure
 ******************************************************************************/

typedef struct VIP_SRNN_ARCH_INFO {
	unsigned char ModeAISR;
	unsigned char NumBlock;
	unsigned char ChIN;
	unsigned char ChF;
	unsigned char ChB;
	unsigned char ChPSNRTail;
	unsigned char ModeScale;
} VIP_SRNN_ARCH_INFO;

typedef struct VIP_SRNN_TOP_INFO {
	unsigned char ClkEn;
	unsigned short SrnnIn_Width;
	unsigned short SrnnIn_Height;
	unsigned char ModeAISR;
	unsigned char ModeScale;
	unsigned char Hx2En;
	unsigned char SubPathEn;
	unsigned short SrnnOut_Width;
	unsigned short SrnnOut_Height;
	unsigned int FrameRate;
} VIP_SRNN_TOP_INFO;
#endif
/* ----- ----- ----- ----- ----- GLOBAL PARMETER ----- ----- ----- ----- ----- */
extern VIP_SRNN_ARCH_INFO gSrnnArchInfo;
extern VIP_SRNN_TOP_INFO gSrnnTopInfo;
extern unsigned char gSrnnblockallDbApply_flag;

/*******************************************************************************
*functions*
******************************************************************************/
void SrnnCtrl_SetSRNNAISRMode(unsigned char mode, unsigned char isSkipDB);
unsigned char SrnnCtrl_GetSRNNAISRMode(void);
void SrnnCtrl_SetSRNNScaleMode(unsigned char SclaeMode, unsigned char isSkipDB);
unsigned char SrnnCtrl_GetSRNNScaleMode(void);
void SrnnCtrl_SetSRNNClkFract(unsigned char En, unsigned char Fract_a, unsigned char Fract_b);
void drvif_set_srnn_init(unsigned char isSkipDB);
void drvif_srnn_gSrnnTopInfo_reset(void);
void drvif_srnn_clk_enable(unsigned char clk_srnn_en);
void drvif_srnn_clk_enable_pqmasktask(unsigned char clk_srnn_en);
void drvif_srnn_pq_enable_set(unsigned char srnn_en);
void drvif_color_Set_NNSR_model_By_DMA(unsigned char ai_sr_mode, unsigned char su_mode, unsigned short* param, unsigned int phy_addr, unsigned int* vir_addr, unsigned char isSkipDB);
void drvif_srnn_weight_blend_1d(unsigned char aisrmode, unsigned short* param1, unsigned short* param2, unsigned short* param_out, int alpha);
void drvif_srnn_set_table(DRV_srnn_table *ptr, unsigned char isSkipDB);
void drvif_srnn_set_vipsrnn_ctrl_table(DRV_SRNN_CTRL_TABLE *ptr, unsigned char isSkipDB);
char fwif_color_reset_NNSR_DMA_Error_Status(unsigned char tbl_idx, unsigned char showMSG);
char fwif_color_check_NNSR_DMA_Apply_Done(unsigned char tbl_idx, unsigned char showMSG);
unsigned char drvif_srnn_get_y_delta_gain(void);
void drvif_srnn_set_y_delta_gain(unsigned char bGain);
unsigned int drvif_srnn_get_weight_table_size(unsigned char nnsr_mode, unsigned char su_mode);
void drvif_srnn_model_rbus_read(unsigned char ai_sr_mode, unsigned short *param);
void drvif_srnn_reg_init(void);
void drvif_srnn_set_Hx2(unsigned char En);
void drvif_srnn_set_clk_fract_by_linerate_ratio(void);
void drvif_srnn_set_aisr_line_delay(unsigned char aisrmode, unsigned int In_Length, unsigned int Out_Length);
void drvif_srnn_set_pre_rd_start(void);
void drvif_srnn_demo_window(unsigned int h_sta, unsigned int h_end, unsigned int v_sta, unsigned int v_end);
void drvif_srnn_demo_window_mode(unsigned char demo_sr_mode);
unsigned char SrnnCtrl_Getdummy(void);
#endif /* _SRNN_H_ */

