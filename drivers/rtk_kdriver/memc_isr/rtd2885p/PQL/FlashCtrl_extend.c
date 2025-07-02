#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

VOID FlashCtrl_Init_RTK2885pp(_OUTPUT_FLASH_CTRL *pOutput)
{
	pOutput->u1_detect = 0;
}

VOID FlashCtrl_Proc_RTK2885pp(const _PARAM_FLASH_CTRL *pParam, _OUTPUT_FLASH_CTRL *pOutput)
{
	
}
