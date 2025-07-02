#ifndef __DEMOD_EXT_ALL_HANDLE_H__
#define __DEMOD_EXT_ALL_HANDLE_H__

#include "demod_ext.h"
#include "demod_ext_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SONY_ISDBT_EN
#define SONY_ISDBS_EN

typedef struct {
	DEMOD*                      m_pDemod;
	DEMOD*                      m_pATSCDemod;
       	DEMOD*                      m_pATSC3Demod;
	DEMOD*                      m_pQAMDemod;
	DEMOD*                      m_pDVBTDemod;
	DEMOD*                      m_pDVBT2Demod;
	DEMOD*                      m_pDVBSDemod;
	DEMOD*                      m_pDVBS2Demod;
	DEMOD*                      m_pDVBCDemod;
	DEMOD*                      m_pDTMBDemod;
	DEMOD*                      m_pISDBTDemod;
#ifdef SONY_ISDBT_EN
	DEMOD*                      m_pSONYISDBTDemod;
	DEMOD*                      m_pSONYISDBT_CXD2856;
	DEMOD*                      m_pSONYISDBT_CXD2878;
#endif
#ifdef SONY_ISDBS_EN
	DEMOD*                      m_pSONYISDBS_CXD2856;
	DEMOD*                      m_pSONYISDBS_CXD2878;
	DEMOD*                      m_pSONYISDBS3_CXD2878;
#endif
	DEMOD*                      m_pATVDemod;
	RTK_DEMOD_MODE                m_mode;
	unsigned char               m_output_mode;
	U32BITS               m_output_freq;
	struct task_struct *demod_kernelthread;
	unsigned char m_KernelAutoTuneThread_start;
	unsigned char               m_kernel_auto_tune_en;
	unsigned char               m_auto_tune_en;//non-kernel autotune

	int (* SetDTVMode)(struct sDEMOD *demod, RTK_DEMOD_MODE mode);
	void (* Destory)(struct sDEMOD *demod);
} REALTEK_ALL;

void REALTEK_EXT_Constructors(DEMOD* pDemodKernel, unsigned char addr, unsigned char output_mode, unsigned char output_freq, COMM* pComm);
void REALTEK_EXT_Destructors(DEMOD* pDemodKernel);
int REALTEK_EXT_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys);
int REALTEK_EXT_Init(DEMOD* pDemodKernel);
int REALTEK_EXT_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO* pSigInfo);
int REALTEK_EXT_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam);
int REALTEK_EXT_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode);
int REALTEK_EXT_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam);
int REALTEK_EXT_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock);
int REALTEK_EXT_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo);
int REALTEK_EXT_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality);
int REALTEK_EXT_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset);
int REALTEK_EXT_GetChannelInfo(DEMOD* pDemodKernel, unsigned char* Standard, unsigned char* Modulation, unsigned char* CodeRate, unsigned char* PilotOnOff, unsigned char* Inverse);
int REALTEK_EXT_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize);
int REALTEK_EXT_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock);
int REALTEK_EXT_Suspend(DEMOD* pDemodKernel);
int REALTEK_EXT_Resume(DEMOD* pDemodKernel);
int REALTEK_EXT_ScanSignal(DEMOD* pDemodKernel, S32BITS* pOffset, SCAN_RANGE range);
int REALTEK_EXT_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off);
int REALTEK_EXT_AutoTune(DEMOD* pDemodKernel);
int REALTEK_EXT_KernelAutoTune(void* pDemodKernel);
int REALTEK_EXT_InspectSignal(DEMOD* pDemodKernel);
int REALTEK_EXT_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode);
int REALTEK_EXT_GetDemodInfo(DEMOD* pDemodKernel, TV_DEMOD_INFO* pDemodInfo);
int REALTEK_EXT_GetRegCmd(DEMOD* pDemodKernel, unsigned int RegAddr, unsigned int *RegValue);
int REALTEK_EXT_SetRegCmd(DEMOD* pDemodKernel, unsigned int RegAddr, unsigned int RegValue);
int REALTEK_EXT_SetTunerInputMode(DEMOD* pDemodKernel, unsigned char Mode);
int REALTEK_EXT_GetTsClkRate(DEMOD* pDemodKernel, unsigned int *pTsClkRate);

int REALTEK_EXT_KernelThread_EN(DEMOD* pDemodKernel, unsigned int thread_en);
int REALTEK_EXT_ATSGetDummy(RTK_DEMOD_ATS_GET_DUMMY *ats_data);

//for ext demod only
int REALTEK_ALL_SetExtDemodInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pSigInfo);
int REALTEK_ALL_GetSignalQuality_Ext(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality);
int REALTEK_ALL_SetTSMode_Ext(DEMOD* pDemodKernel, TS_PARAM* pParam);
#ifdef __cplusplus
}
#endif

#endif // __DEMOD_EXT_ALL_HANDLE_H__
