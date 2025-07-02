/***************************************************************************************************
  File        : demod_ext.c
  Description : Base Class of All external Demod
  Author      :
****************************************************************************************************
    Update List :
----------------------------------------------------------------------------------------------------
    1.0a     |   20230627    | Add all Function
***************************************************************************************************/
#include <linux/mutex.h>
#include "tv_osal.h"
#include "rtk_tv_fe_types.h"
//#include "tv_fe_aux.h"
#include "comm.h"
#include "demod_ext.h"
#include "tuner.h"

void ExtDemod_Constructors(DEMOD* pDemodKernel)
{
	pDemodKernel->m_addr			 = 0;
	pDemodKernel->m_id				 = DTV_DEMOD_NO_DEMOD;
	pDemodKernel->m_pComm 			 = NULL;
	pDemodKernel->m_pTuner			 = NULL;
	pDemodKernel->m_ScanRange 		 = SCAN_RANGE_NONE;
	pDemodKernel->m_Capability		 = TV_SYS_TYPE_ATSC |
									   TV_SYS_TYPE_OPENCABLE |
									   TV_SYS_TYPE_ISDBT |
									   TV_SYS_TYPE_DVBT |
									   TV_SYS_TYPE_DVBC |
									   TV_SYS_TYPE_DTMB;

	// HW Parameters
	pDemodKernel->m_clock 			 = 0;

	// IF IN MODE
	pDemodKernel->m_if.freq			 = 0;
	pDemodKernel->m_if.inversion		 = 0;
	pDemodKernel->m_if.agc_pola		 = 1;
	pDemodKernel->m_tv_sys			 = TV_SYS_UNKNOWN;

	// TSOUT MODE
	//		  m_ts.mode 		   = SERIAL_MODE;
	pDemodKernel->m_ts.mode			 = PARALLEL_MODE;
	pDemodKernel->m_ts.data_order 	 = MSB_D7;
	pDemodKernel->m_ts.datapin		 = MSB_FIRST;
	pDemodKernel->m_ts.err_pol		 = ACTIVE_HIGH;
	pDemodKernel->m_ts.sync_pol		 = ACTIVE_HIGH;
	pDemodKernel->m_ts.val_pol		 = ACTIVE_HIGH;
	pDemodKernel->m_ts.clk_pol		 = FALLING_EDGE;
	pDemodKernel->m_ts.internal_demod_input = 0;

	pDemodKernel->m_update_interval_ms = TV_UPDATE_INTERVAL_INFINITY;

	pDemodKernel->Init = ExtDemod_Init;
	pDemodKernel->Reset = ExtDemod_Reset;
	pDemodKernel->AcquireSignal = ExtDemod_AcquireSignal;
	pDemodKernel->ScanSignal = ExtDemod_ScanSignal;
	pDemodKernel->SetIF = ExtDemod_SetIF;
	pDemodKernel->SetTSMode = ExtDemod_SetTSMode;
	pDemodKernel->SetTVSysEx = ExtDemod_SetTVSysEx;
	pDemodKernel->SetMode = ExtDemod_SetMode;
	pDemodKernel->SetTVSys = ExtDemod_SetTVSys;
	pDemodKernel->GetLockStatus = ExtDemod_GetLockStatus;
	pDemodKernel->GetSignalInfo = ExtDemod_GetSignalInfo;
	pDemodKernel->GetSignalQuality = ExtDemod_GetSignalQuality;

	pDemodKernel->GetDemodStatus = ExtDemod_GetDemodStatus;
	pDemodKernel->GetCarrierOffset = ExtDemod_GetCarrierOffset;
	pDemodKernel->GetDebugLogBuf = ExtDemod_GetDebugLogBuf;
	pDemodKernel->ForwardI2C = ExtDemod_ForwardI2C;
	pDemodKernel->AutoTune = ExtDemod_AutoTune;
	pDemodKernel->KernelAutoTune = ExtDemod_KernelAutoTune;
	pDemodKernel->GetCapability = ExtDemod_GetCapability;
	pDemodKernel->Suspend = ExtDemod_Suspend;
	pDemodKernel->Resume = ExtDemod_Resume;
	pDemodKernel->SetTvMode = ExtDemod_SetTvMode;
	pDemodKernel->InspectSignal = ExtDemod_InspectSignal;   // Add monitor function that used to monitor demod status
	pDemodKernel->AttachTuner = ExtDemod_AttachTuner;
	pDemodKernel->GetTSParam = ExtDemod_GetTSParam;
    pDemodKernel->GetTsClkRate = ExtDemod_GetTsClkRate; 
	pDemodKernel->Lock = ExtDemod_Lock;
	pDemodKernel->Unlock = ExtDemod_Unlock;
	pDemodKernel->Destory = ExtDemod_Destructors;

	mutex_init(&pDemodKernel->m_lock);
}

void ExtDemod_Destructors(DEMOD* pDemodKernel)
{
	mutex_destroy(&pDemodKernel->m_lock);
}

int ExtDemod_Init(DEMOD* pDemodKernel)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_Reset(DEMOD* pDemodKernel)
{
	return TUNER_CTRL_OK;
}

//int ExtDemod_AcquireSignal(PDemod pDemodKernel, unsigned char WaitSignalLock = 1)
int ExtDemod_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_ScanSignal(DEMOD* pDemodKernel, S32BITS* offset, SCAN_RANGE range)
{
	int ret;
	*offset = 0;
	if ((ret = pDemodKernel->AcquireSignal(pDemodKernel, 1)) == TUNER_CTRL_OK)
		ret = pDemodKernel->GetCarrierOffset(pDemodKernel, offset);
	return ret;
}

int ExtDemod_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam)
{
	return TUNER_CTRL_OK;
}

//int ExtDemod_SetTVSysEx(TV_SYSTEM sys, const TV_SIG_INFO* pSigInfo = NULL)
int ExtDemod_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO* pSigInfo)
{
	return pDemodKernel->SetTVSys(pDemodKernel, sys);
}

int ExtDemod_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_GetDemodStatus(DEMOD* pDemodKernel, U32BITS* pStatus)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset)
{
	*pOffset = 0;
	return TUNER_CTRL_OK;
}
int ExtDemod_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize)
{
	* LogBufAddr = 0;
	* LogBufSize = 0;
	return TUNER_CTRL_OK;
}

int ExtDemod_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_AutoTune(DEMOD* pDemodKernel)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_KernelAutoTune(DEMOD* pDemodKernel)
{
	return TUNER_CTRL_OK;
}

U32BITS ExtDemod_GetCapability(DEMOD* pDemodKernel)
{
	return pDemodKernel->m_Capability;
}

int ExtDemod_Suspend(DEMOD* pDemodKernel)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_Resume(DEMOD* pDemodKernel)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE sys_type)
{
	return TUNER_CTRL_OK;
}

int ExtDemod_InspectSignal(DEMOD* pDemodKernel)      // Add monitor function that used to monitor demod status
{
	unsigned char  lock;
	//float          snr;
	//float          ber;
	TV_SIG_QUAL    quality;

	pDemodKernel->GetLockStatus(pDemodKernel, &lock);
	pDemodKernel->GetSignalQuality(pDemodKernel, TV_QUAL_SNR, &quality);
	//snr = quality.snr;
	pDemodKernel->GetSignalQuality(pDemodKernel, TV_QUAL_BER, &quality);
	//ber = quality.ber;
	//return printf(pBuff, BufferSize, "lock=%d, snr=%f, ber=%f", lock, snr, ber);
	return 1;
}

void ExtDemod_AttachTuner(DEMOD* pDemodKernel, TUNER* pTuner)
{
	pDemodKernel->m_pTuner = pTuner;
}

TS_PARAM ExtDemod_GetTSParam(DEMOD* pDemodKernel)
{
	return pDemodKernel->m_ts;
}


int ExtDemod_GetTsClkRate(DEMOD* pDemodKernel, unsigned int *pTsClkRate)
{
        *pTsClkRate = 0; 
	return TUNER_CTRL_OK;
}

void ExtDemod_Lock(DEMOD* pDemodKernel)
{
	mutex_lock(&pDemodKernel->m_lock);
}

void ExtDemod_Unlock(DEMOD* pDemodKernel)
{
	mutex_unlock(&pDemodKernel->m_lock);
}

