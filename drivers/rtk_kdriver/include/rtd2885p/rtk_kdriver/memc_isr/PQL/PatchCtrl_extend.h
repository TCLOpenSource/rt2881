#ifndef _PATCH_CTRL_H
#define _PATCH_CTRL_H

typedef struct
{
	unsigned char u1_ID_001_en;
}_PARAM_PATCH_CTRL;

typedef struct
{
	unsigned char u1_ID_001_apply;
}_OUTPUT_PATCH_CTRL;

VOID PatchCtrl_Init_RTK2885pp(_OUTPUT_PATCH_CTRL *pOutput);
VOID PatchCtrl_Proc_RTK2885pp(const _PARAM_PATCH_CTRL *pParam, _OUTPUT_PATCH_CTRL *pOutput);

#endif
