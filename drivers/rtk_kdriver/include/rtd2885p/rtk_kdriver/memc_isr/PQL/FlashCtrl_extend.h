#ifndef _FLASH_CTRL_DEF_H
#define _FLASH_CTRL_DEF_H

typedef struct
{
}_PARAM_FLASH_CTRL;

typedef struct
{
	unsigned char u1_detect;
}_OUTPUT_FLASH_CTRL;

VOID FlashCtrl_Init_RTK2885pp(_OUTPUT_FLASH_CTRL *pOutput);
VOID FlashCtrl_Proc_RTK2885pp(const _PARAM_FLASH_CTRL *pParam, _OUTPUT_FLASH_CTRL *pOutput);

#endif