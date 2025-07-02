#ifndef _PERIODIC_CTRL_DEF_H
#define _PERIODIC_CTRL_DEF_H

typedef struct
{
}_PARAM_PERIODIC_CTRL;

typedef struct
{
	unsigned char u1_detect_mode;

	unsigned char u4_lpf_5x5_coef;

	unsigned char u1_layer0_frq_en_ori;
	unsigned char u1_layer1_frq_en_ori;
	unsigned char u1_layer2_frq_en_ori;
	unsigned char u1_layer3_frq_en_ori;

	unsigned char u1_layer0_frq_en;
	unsigned char u1_layer1_frq_en;
	unsigned char u1_layer2_frq_en;
	unsigned char u1_layer3_frq_en;

	signed short s11_avgbv_mvx[5][32];
	signed short s10_avgbv_mvy[5][32];
	unsigned int u32_avgbv_unconf[32];

	unsigned short u8_gmv_z_stable_pre;
	

}_OUTPUT_PERIODIC_CTRL;

VOID PeriodicCtrl_Init_RTK2885pp(_OUTPUT_PERIODIC_CTRL *pOutput);
VOID PeriodicCtrl_Proc_RTK2885pp(const _PARAM_PERIODIC_CTRL *pParam, _OUTPUT_PERIODIC_CTRL *pOutput);

#endif