/* ST2094 header */
#ifndef  __HDR10_vivid_H__
#define  __HDR10_vivid_H__

#ifdef __cplusplus
extern "C" {
#endif
/* some include about scaler*/

#include "scaler/vipCommon.h"
#include "scaler/vipRPCCommon.h"

#define HDR_vivid_DMA_size 1025

typedef struct _VIP_HDR10_vivid_DMA_CTRL {
	unsigned int size;
	unsigned int phy_addr_align;
	unsigned int *pVir_addr_align;
} VIP_HDR10_vivid_DMA_CTRL;

typedef struct _VIP_HDR10_vivid_CTRL {
	/* 0*/ unsigned int Enable;
	/* 1*/ unsigned int update_DMA_Flag;
	/* 2*/ unsigned int CUVA_K_shift_bit;
	/* 3*/ unsigned int Debug_Log;
	/* 4*/ unsigned int Debug_Delay;
	/* 5*/ unsigned int Debug_VGIP_Line_sta;
	/* 6*/ unsigned int Debug_VGIP_Line_end;
	/* 7*/ unsigned int Debug_VGIP_Line_diff;
	/* 8*/ unsigned int Debug_90clk_sta;
	/* 9*/ unsigned int Debug_90clk_end;
	/*10*/ unsigned int Debug_90clk_diff;
	/*11*/ unsigned int Debug_Cal_skip_flag;
	/*12*/ unsigned int MetadataProc_VLC_LineCnt;

	unsigned int k_hdr10_vivid[HDR_vivid_DMA_size];
	unsigned int sca_hdr10_vivid[HDR_vivid_DMA_size];
	unsigned int EOTF_hdr10_vivid[HDR_vivid_DMA_size];
	
	VIP_HDR10_vivid_DMA_CTRL EOTF_DMA;
	VIP_HDR10_vivid_DMA_CTRL OETF_DMA;

} VIP_HDR10_vivid_CTRL;

unsigned char drvif_Get_HDR10_vivid_EFuse_Status(void);

HDR_VIVID_Metadata* drvif_fwif_color_Get_HDR10_vivid_METADATA_Buff(void);
VIP_HDR10_vivid_CTRL* drvif_fwif_color_Get_HDR10_vivid_Ctrl(void);
void drvif_fwif_color_ScalerVIP_set_HDR10_vivid_metadata2reg_frameSync(int display_Brightness, _RPC_system_setting_info *RPC_sys_info, VIP_HDR10_vivid_CTRL* HDR10_vivi_CTRL);

void drvif_DM2_hdr_cuva_k_shift_sel_Set(unsigned char shift_bit);
void drvif_DM2_hdr_cuva_En_Set(unsigned char En);
void drvif_HDR_VIVID_B0106_set(unsigned char enable);

#ifdef __cplusplus
}
#endif

#endif // __TCHDR_H__


