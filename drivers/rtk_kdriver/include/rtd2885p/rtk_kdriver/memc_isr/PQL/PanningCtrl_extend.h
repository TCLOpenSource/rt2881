#ifndef _PANNING_CTRL_DEF_H
#define _PANNING_CTRL_DEF_H

typedef struct
{
}_PARAM_PANNING_CTRL;

typedef struct
{
	unsigned char u1_detect;
}_OUTPUT_PANNING_CTRL;

VOID PanningCtrl_Init_RTK2885pp(_OUTPUT_PANNING_CTRL *pOutput);
VOID PanningCtrl_Proc_RTK2885pp(const _PARAM_PANNING_CTRL *pParam, _OUTPUT_PANNING_CTRL *pOutput);

#endif