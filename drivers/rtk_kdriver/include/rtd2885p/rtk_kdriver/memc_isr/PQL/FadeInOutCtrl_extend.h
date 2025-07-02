#ifndef _FADEINOUT_CTRL_DEF_H
#define _FADEINOUT_CTRL_DEF_H

typedef struct
{
        unsigned char u1_fadeinout_calc_en;
}_PARAM_FADEINOUT_CTRL;

typedef struct
{
	unsigned char u1_detect;
}_OUTPUT_FADEINOUT_CTRL;

VOID FadeInOutCtrl_Init_RTK2885pp(_OUTPUT_FADEINOUT_CTRL *pOutput);
VOID FadeInOutCtrl_Proc_RTK2885pp(const _PARAM_FADEINOUT_CTRL *pParam, _OUTPUT_FADEINOUT_CTRL *pOutput);

#endif