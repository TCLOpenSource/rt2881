/* #include <Application/AppClass/channeldef.h> */
#include <linux/slab.h>
#include "tv_osal.h"
#include "rtk_tv_fe_types.h"
//#include "tv_fe_aux.h"
#include "comm.h"
#include "tuner.h"

void Tuner_Constructors_Ext(TUNER* pTunerKernel)
{
	pTunerKernel->m_addr			= 0;
	pTunerKernel->m_id			= TV_TUNER_NO_TUNER;
	pTunerKernel->m_capability	= TV_SYSTEM_TYPE_DVB | TV_SYSTEM_TYPE_ATSC;
	pTunerKernel->m_pComm 		= NULL;
	pTunerKernel->m_tv_sys		= TV_TUNER_NO_TUNER;
	pTunerKernel->m_freq			= 0;
	pTunerKernel->m_op_mode		= TV_OP_MODE_NORMAL;
	pTunerKernel->DemodParamEn	= 0;

	pTunerKernel->TunerParam.id = TV_TUNER_NO_TUNER;
	pTunerKernel->TunerParam.if_inversion = 0;
	pTunerKernel->TunerParam.if_agc_pola = 0;
	pTunerKernel->TunerParam.if_freq = 5000000;

	pTunerKernel->Init = Tuner_Init_Ext;
	pTunerKernel->Reset = Tuner_Reset_Ext;
	pTunerKernel->SetTuner = Tuner_SetTuner_Ext;
	pTunerKernel->SetTunerEx = Tuner_SetTunerEx_Ext;
	pTunerKernel->GetParam = Tuner_GetParam_Ext;
	pTunerKernel->Destory = Tuner_Destructors_Ext;
}

void Tuner_Destructors_Ext(TUNER* pDemodKernel)
{
}


int Tuner_GetParam_Ext(TUNER* pTunerKernel, TV_SYSTEM sys, TUNER_PARAM *pParam)
{
	pParam->id = pTunerKernel->TunerParam.id;
	pParam->std = pTunerKernel->TunerParam.std;
	pParam->step_freq = pTunerKernel->TunerParam.step_freq;
	pParam->if_freq = pTunerKernel->TunerParam.if_freq;
	pParam->if_inversion = pTunerKernel->TunerParam.if_inversion;
	pParam->if_agc_pola = pTunerKernel->TunerParam.if_agc_pola;
	pParam->min_freq = pTunerKernel->TunerParam.min_freq;
	pParam->max_freq = pTunerKernel->TunerParam.max_freq;
	pParam->lowband_midband_limit_freq = pTunerKernel->TunerParam.lowband_midband_limit_freq;
	pParam->midband_highband_limit_freq = pTunerKernel->TunerParam.midband_highband_limit_freq;

	if (pParam->id == TV_TUNER_NO_TUNER) {
		switch (sys) {
		case TV_SYS_ATSC:
		case TV_SYS_ATSC3P0_6M:
		case TV_SYS_ATSC3P0_7M:
		case TV_SYS_ATSC3P0_8M:
		case TV_SYS_OPENCABLE_64:
		case TV_SYS_OPENCABLE_256:
			pParam->if_freq = 6000000;
			break;

		default:
			pParam->if_freq = 5000000;
			break;
		}
	}

	return TUNER_CTRL_OK;
}

int Tuner_Init_Ext(TUNER* pTunerKernel)
{
	return TUNER_CTRL_OK;
}

int Tuner_Reset_Ext(TUNER* pTunerKernel)
{
	return TUNER_CTRL_OK;
}

int Tuner_SetTuner_Ext(TUNER* pTunerKernel, U32BITS freq, TV_SYSTEM sys)
{
	return TUNER_CTRL_OK;
}

int Tuner_SetTunerEx_Ext(TUNER* pTunerKernel, U32BITS freq, TV_SYSTEM sys, const TV_SIG_INFO *pSigInfo)
{
	return pTunerKernel->SetTuner(pTunerKernel, freq, sys);
}

