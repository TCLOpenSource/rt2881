#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/PQLGlobalDefs.h"
#include "memc_reg_def.h"

VOID PanningCtrl_Init_RTK2885pp(_OUTPUT_PANNING_CTRL *pOutput)
{
	pOutput->u1_detect = 0;
}

VOID PanningCtrl_Proc_RTK2885pp(const _PARAM_PANNING_CTRL *pParam, _OUTPUT_PANNING_CTRL *pOutput)
{
	
}
