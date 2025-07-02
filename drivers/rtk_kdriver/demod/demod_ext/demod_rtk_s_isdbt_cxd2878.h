#ifndef __DEMOD_CXD2878_S_ISDBT_H__
#define __DEMOD_CXD2878_S_ISDBT_H__

#include "demod_ext.h"
#include "demod_ext_def.h"
#include "CXD2878Family_refcode/sony_demod.h"

#define ISDBT_STATUS_CHECK_INTERVAL_MS 1000

typedef struct {
	void*                       m_private;
	unsigned char               m_output_freq;
	REG_VALUE_ENTRY*            m_pTunerOptReg;
	unsigned int                m_TunerOptRegCnt;
	unsigned char               m_low_snr_detect;
	unsigned char               m_low_snr_recover;
	unsigned char               m_auto_tune_enable;
	unsigned char               m_patch_cnt;
	unsigned char               m_acquire_sig_en;
	unsigned long				m_status_checking_stime;

	//int (*Activate)(struct sDEMOD *demod, unsigned char force_rst);
	//void (*Destory)(struct sDEMOD *demod);
} CXD2878_S_ISDBT;

void CXD2878_S_ISDBT_Constructors(DEMOD* pDemodKernel, unsigned char addr, 	unsigned char output_mode, unsigned char output_freq, COMM* pComm);
void CXD2878_S_ISDBT_Destructors(DEMOD* pDemodKernel);
int CXD2878_S_ISDBT_Init(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode);
int CXD2878_S_ISDBT_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys);
int CXD2878_S_ISDBT_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO*  pSigInfo);
int CXD2878_S_ISDBT_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam);
int CXD2878_S_ISDBT_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode);
int CXD2878_S_ISDBT_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam);
int CXD2878_S_ISDBT_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock);
int CXD2878_S_ISDBT_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo);
int CXD2878_S_ISDBT_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id,	TV_SIG_QUAL* pQuality);
int CXD2878_S_ISDBT_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset);
int CXD2878_S_ISDBT_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize);
int CXD2878_S_ISDBT_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock);
int CXD2878_S_ISDBT_AcquireSignalThread(DEMOD* pDemodKernel);
int CXD2878_S_ISDBT_ScanSignal(DEMOD* pDemodKernel, S32BITS* pOffset, SCAN_RANGE range);
int CXD2878_S_ISDBT_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off);
int CXD2878_S_ISDBT_AutoTune(DEMOD* pDemodKernel);
int CXD2878_S_ISDBT_KernelAutoTune(DEMOD* pDemodKernel);
int CXD2878_S_ISDBT_InspectSignal(DEMOD* pDemodKernel);
int CXD2878_S_ISDBT_Activate(DEMOD* pDemodKernel, unsigned char force_rst);
int CXD2878_S_ISDBT_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode);
int CXD2878_S_ISDBT_SetExtDemodInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pSigInfo);
void CXD2878_S_ISDBT_Lock(DEMOD* pDemodKernel);
void CXD2878_S_ISDBT_Unlock(DEMOD* pDemodKernel);
sony_result_t CXD2878_i2c_dummyRead(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode);
sony_result_t CXD2878_i2c_dummyWrite(sony_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t *pData, uint32_t size, uint8_t mode);


#define CXD2878_S_ISDBT_DBG(fmt, args...)        rtd_extdemod_print(KERN_DEBUG, "CXD2878_ISDBT_DEBUG, " fmt, ##args);
#define CXD2878_S_ISDBT_INFO(fmt, args...)       rtd_extdemod_print(KERN_INFO, "CXD2878_ISDBT_INFO, " fmt, ##args);
#define CXD2878_S_ISDBT_WARNING(fmt, args...)    rtd_extdemod_print(KERN_WARNING, "CXD2878_ISDBT_WARNING, " fmt, ##args);

#endif // __DEMOD_CXD2878_S_ISDBT_H__
