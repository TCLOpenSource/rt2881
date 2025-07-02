#ifndef _SOBJ_CTRL_DEF_H
#define _SOBJ_CTRL_DEF_H

typedef struct
{
    bool u1_Sobj_en;
    bool u1_Sobj_color_en;
}_PARAM_SOBJ_CTRL;

typedef struct
{
    unsigned int u32_Sobj_rgn_i_clr;
    unsigned int u32_Sobj_rgn_p_clr;
    unsigned int u32_Sobj_med_lvl[32];
    unsigned char u8_med_curve[8];    
}_OUTPUT_SOBJ_CTRL;

VOID SObjCtrl_Init_RTK2885pp(_OUTPUT_SOBJ_CTRL *pOutput);
VOID SObjCtrl_Proc_RTK2885pp(const _PARAM_SOBJ_CTRL *pParam, _OUTPUT_SOBJ_CTRL *pOutput);

#endif