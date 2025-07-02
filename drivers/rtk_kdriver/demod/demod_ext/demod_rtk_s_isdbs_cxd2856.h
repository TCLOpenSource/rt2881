#ifndef __DEMOD_CXD2856_S_ISDBS_H__
#define __DEMOD_CXD2856_S_ISDBS_H__

#include "demod_ext.h"
#include "demod_ext_def.h"

#include "CXD2856Family_refcode/sony_demod.h"

#define ISDBS_STATUS_CHECK_INTERVAL_MS 1000

/*** ISDBS struct ***/
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
} CXD2856_S_ISDBS;


//ISDBS 2nd
void CXD2856_S_ISDBS_Constructors(DEMOD* pDemodKernel, unsigned char addr, 	unsigned char output_mode, unsigned char output_freq, COMM* pComm);
void CXD2856_S_ISDBS_Destructors(DEMOD* pDemodKernel);
int CXD2856_S_ISDBS_Init(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode);
int CXD2856_S_ISDBS_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys);
int CXD2856_S_ISDBS_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO*  pSigInfo);
int CXD2856_S_ISDBS_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam);
int CXD2856_S_ISDBS_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode);
int CXD2856_S_ISDBS_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam);
int CXD2856_S_ISDBS_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock);
int CXD2856_S_ISDBS_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo);
int CXD2856_S_ISDBS_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id,	TV_SIG_QUAL* pQuality);
int CXD2856_S_ISDBS_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset);
int CXD2856_S_ISDBS_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize);
int CXD2856_S_ISDBS_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock);
int CXD2856_S_ISDBS_AcquireSignalThread(DEMOD* pDemodKernel);
int CXD2856_S_ISDBS_ScanSignal(DEMOD* pDemodKernel, S32BITS* pOffset, SCAN_RANGE range);
int CXD2856_S_ISDBS_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off);
int CXD2856_S_ISDBS_AutoTune(DEMOD* pDemodKernel);
int CXD2856_S_ISDBS_KernelAutoTune(DEMOD* pDemodKernel);
int CXD2856_S_ISDBS_InspectSignal(DEMOD* pDemodKernel);
int CXD2856_S_ISDBS_Activate(DEMOD* pDemodKernel, unsigned char force_rst);
int CXD2856_S_ISDBS_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode);
int CXD2856_S_ISDBS_SetExtDemodInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pSigInfo);
void CXD2856_S_ISDBS_Lock(DEMOD* pDemodKernel);
void CXD2856_S_ISDBS_Unlock(DEMOD* pDemodKernel);
sony_result_t CXD2856_i2c_IsdbsRead(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode);
sony_result_t CXD2856_i2c_IsdbsWrite(sony_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t *pData, uint32_t size, uint8_t mode);

#define CXD2856_S_ISDBS_DBG(fmt, args...)        rtd_extdemod_print(KERN_DEBUG, "_CXD2856_ISDBS_DEBUG, " fmt, ##args);
#define CXD2856_S_ISDBS_INFO(fmt, args...)       rtd_extdemod_print(KERN_INFO, "CXD2856_ISDBS_INFO, " fmt, ##args);
#define CXD2856_S_ISDBS_WARNING(fmt, args...)    rtd_extdemod_print(KERN_WARNING, "CXD2856_ISDBS_WARNING, " fmt, ##args);

#endif // __DEMOD_CXD2856_S_ISDBS_H__
