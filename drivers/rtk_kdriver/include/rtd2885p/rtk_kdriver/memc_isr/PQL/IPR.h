#ifndef _IPR_H_
#define _IPR_H_


VOID Identification_Pattern_preProcess_TV001_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_preProcess_TV002_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_preProcess_TV006_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_preProcess_TV011_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_preProcess_TV030_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);	
VOID Identification_Pattern_preProcess_TV043_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);

VOID Identification_Pattern_wrtAction_TV001_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_wrtAction_TV002_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_wrtAction_TV006_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_wrtAction_TV011_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_wrtAction_TV030_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);	
VOID Identification_Pattern_wrtAction_TV043_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);

#if 1//#if RTK_MEMC_Performance_tunging_from_tv001
VOID Identification_Pattern_preProcess_TV010_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identification_Pattern_wrtAction_TV010_RTK2885p(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);

#endif

VOID Identificaton_other_Pre(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);
VOID Identificaton_other(const _PARAM_WRT_COM_REG *pParam, _OUTPUT_WRT_COM_REG *pOutput);

#endif
