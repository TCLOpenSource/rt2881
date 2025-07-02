#ifndef __I3DDMA_DRV_H__
#define __I3DDMA_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include <rbus/rbusTypes.h>
//#include <rtd_types.h>
//#include <rbus/scaler/rbusHDMIReg.h>
//#include <rbusHDMIReg.h>
//#include <OSAL/PLI.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/scaler/scalerlib.h>
//#include "tvscalercontrol/tve/tve.h"
#include "tvscalercontrol/vdac/vdac.h"

#define I3DDMA_PRINTF printf


typedef enum I3DDMA_INFO_LIST{
	I3DDMA_INFO_HW_I3DDMA_DMA=	0x00,
	I3DDMA_INFO_HW_DITHER12X10_ENABLE,
	I3DDMA_INFO_HW_DITHER10x8_ENABLE,
	I3DDMA_INFO_HW_4XXTO4XX_ENABLE,
	I3DDMA_INFO_HW_RGBTOYUV_ENABLE,
	I3DDMA_INFO_TARGET_DEPTH,
	I3DDMA_INFO_TARGET_COLOR,

	I3DDMA_INFO_HW_HSD_ENABLE,
	I3DDMA_INFO_INPUT_H_SIZE,
	I3DDMA_INFO_OUTPUT_H_SIZE,

	I3DDMA_INFO_INPUT_TIMING,
	I3DDMA_INFO_GEN_TIMING,
	I3DDMA_INFO_FRAMERATE_X2,
	I3DDMA_INFO_ENABLE_4XXTO4XX,
	I3DDMA_INFO_PIXEL_BITS,
	I3DDMA_INFO_ENABLE_3DDMA,
	I3DDMA_INFO_INPUT_SRC_IDX,
	I3DDMA_INFO_INPUT_SRC_TYPE,
	I3DDMA_INFO_INPUT_SRC_FROM,
	I3DDMA_INFO_INPUT_MODE_CURR,
	I3DDMA_INFO_INPUT_H_COUNT

}I3DDMA_INFO_LIST;

//#ifdef DTV_SCART_OUT_ENABLE
typedef struct _IDMA_VGIP2_CTRL_INFO {
	//DOUBLE_CLOCK_CTRL_STATE 	curState;
	SRC_TYPE					srcType;
	unsigned char						srcFrom;
	unsigned char						srcIndex;
	SRC_INFO					srcInfo;
	unsigned char						srcChanged;	// Input source is changed before VGIP2 be released
	unsigned char						srcInHand;	// VGIP2 Input source not be released
}IDMA_VGIP2_CTRL_INFO;
//#endif

void drvif_I3DDMA_Init(void);
void drvif_I3DDMA_DeInit(void);
//void drvif_I3DDMA_Set_Path_Policy(I3DDMA_PATH_POLICY_T i3dPathPolicy);
//void drvif_I3DDMA_Set_Input_Timing(I3DDMA_TIMING_T* pInputTiming);

void drvif_I3DDMA_Set_Input_Info_Vaule(I3DDMA_INFO_LIST input_type, unsigned long value);
unsigned long drvif_I3DDMA_Get_Input_Info_Vaule(I3DDMA_INFO_LIST input_type);

char drvif_I3DDMA_3D_FeatureEnable(char enable) ;
void drv_I3DDMA_ApplySetting(unsigned char display);
void drvif_I3DDMA_dispD_to_IdmaVI_idmaConfig(IDMA_DISPD_CAPTURE_INFO capInfo);
void drvif_I3DDMA_dispDtoI3ddma_TVE(IDMA_DISPD_CAPTURE_INFO outSize);
void drvif_I3DDMA_Enable_Vgip(void);
void drvif_I3DDMA_Disable_Vgip(void);
void drvif_I3DDMA_freeze_enable(unsigned char enable);
void drvif_I3DDMA_Disable_uzd(void);
//void scaler_vodmatoI3ddma_prog(TVE_VIDEO_MODE_SEL tve_mode, UINT8 tve_vflip_en);
unsigned char vo_se_scaling_get_enable(void);
void vo_se_scaling_set_enable(unsigned char enable);
void VO_SE_Scaling(bool enable);
void vo_se_scaling_ctrl(int enable);
unsigned char scaler_get_tve_vflip_enable(void);
void scaler_set_tve_vflip_enable(unsigned char enable);
void disable_I3DDMA_dispDtoSub(void);
unsigned char scaler_dispDtoI3ddma_get_enable(void);
void scaler_dispDtoI3ddma_set_enable(unsigned char enable);
unsigned char scaler_scart_out_isr_get_enable(void);
void scaler_scart_out_isr_set_enable(unsigned char enable);
void drvif_I3DDMA_set_reorder_mode(unsigned char enable);
#ifdef SPEEDUP_NEW_SCALER_FLOW
void avd_start_compensation(void);
#endif
#ifdef CONFIG_DTV_SCART_OUT_ENABLE_444_FORMAT
long Scaler_VO_Set_VoForce422_enable(unsigned char bEnable);
#endif
void fw_scaler_ctrl_dma_vgip_regen_vsync(unsigned char enable);
void drvif_I3DDMA_Reset_Vgip_Source_Sel(void);

#ifdef __cplusplus
}
#endif


#endif
