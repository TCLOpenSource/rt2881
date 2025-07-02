#ifndef _FLICKER_CTRL_DEF_H
#define _FLICKER_CTRL_DEF_H

typedef struct
{
    unsigned char u1_mc_deflicker_calc_en;
}_PARAM_FLICKER_CTRL;

typedef struct
{
	unsigned char u1_detect;
    unsigned char flicker_cnt1;
    unsigned char flicker_cnt2;
    unsigned char flicker_cnt3;
    unsigned char flicker_intp_cnt;
    unsigned char flicker_panning_cnt;
    unsigned char ipme_lpf_coef;
}_OUTPUT_FLICKER_CTRL;

VOID FlickerCtrl_Init_RTK2885pp(_OUTPUT_FLICKER_CTRL *pOutput);
VOID FlickerCtrl_Proc_RTK2885pp(const _PARAM_FLICKER_CTRL *pParam, _OUTPUT_FLICKER_CTRL *pOutput);
VOID MEMC_MC_Deflicker_Calc(const _PARAM_FLICKER_CTRL *pParam, _OUTPUT_FLICKER_CTRL *pOutput);

#endif