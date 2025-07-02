/***************************************************************************************************
  File        : demod_ext.h
  Description : Base Class of All external Demod
  Author      :
****************************************************************************************************
    Update List :
----------------------------------------------------------------------------------------------------
    1.0a     |   20230627    | Add all Function
***************************************************************************************************/
#ifndef __DEMOD_EXT_HANDLE_H__
#define __DEMOD_EXT_HANDLE_H__
#include "tv_osal.h"
#include "rtk_tv_fe_types.h"
//#include "tv_fe_aux.h"
//#include "comm.h"
#include "tuner.h"

#define MAX_DEMOD_ADDR_COUNT   4


enum {
	DTV_DEMOD_NO_DEMOD = 0,
	DTV_DEMOD_SONY_ISDBT_2856,
	DTV_DEMOD_SONY_ISDBT_2878,
	DTV_DEMOD_SONY_ISDBS_2856,
	DTV_DEMOD_SONY_ISDBS_2878,
	DTV_DEMOD_SONY_ISDBS3_2878,
	DTV_DEMOD_REALTEK_ALL,
};

typedef struct sDEMOD {
	void *private_data;

	int (* Init)(struct sDEMOD *demod);
	int (* Reset)(struct sDEMOD *demod);
	int (* AcquireSignal)(struct sDEMOD *demod, unsigned char WaitSignalLock);
	int (* ScanSignal)(struct sDEMOD *demod, S32BITS* offset, SCAN_RANGE range);
	int (* SetIF)(struct sDEMOD *demod, IF_PARAM* pParam);
	int (* SetTSMode)(struct sDEMOD *demod, TS_PARAM* pParam);
	int (* SetTVSysEx)(struct sDEMOD *demod, TV_SYSTEM sys, const TV_SIG_INFO* pSigInfo);
	int (* SetMode)(struct sDEMOD *demod, TV_OP_MODE mode);
	int (* SetTVSys)(struct sDEMOD *demod, TV_SYSTEM sys);
	int (* GetLockStatus)(struct sDEMOD *demod, unsigned char* pLock);
	int (* GetSignalInfo)(struct sDEMOD *demod, TV_SIG_INFO* pInfo);
	int (* GetSignalQuality)(struct sDEMOD *demod, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality);
	int (* GetDemodStatus)(struct sDEMOD *demod, U32BITS* pStatus);
	int (* GetCarrierOffset)(struct sDEMOD *demod, S32BITS* pOffset);
	int (* GetChannelInfo)(struct sDEMOD *demod, unsigned char* Standard, unsigned char* Modulation, unsigned char* CodeRate, unsigned char* PilotOnOff, unsigned char* Inverse);
	int (* GetDebugLogBuf)(struct sDEMOD *demod, unsigned int* LogBufAddr, unsigned int* LogBufSize);
	int (* ForwardI2C)(struct sDEMOD *demod, unsigned char on_off);
	int (* AutoTune)(struct sDEMOD *demod);
	int (* KernelAutoTune)(struct sDEMOD *demod);
	U32BITS (* GetCapability)(struct sDEMOD *demod);
	int (* Suspend)(struct sDEMOD *demod);
	int (* Resume)(struct sDEMOD *demod);
	int (* SetTvMode)(struct sDEMOD *demod, TV_SYSTEM_TYPE sys_type);
	int (* GetDemodInfo)(struct sDEMOD *demod, TV_DEMOD_INFO* pDemodInfo);
	int (* DebugInfo)(struct sDEMOD *demod, unsigned char debug_mode);
	int (* InspectSignal)(struct sDEMOD *demod);	  // Add monitor function that used to monitor demod status
	void(*AttachTuner)(struct sDEMOD *demod, TUNER* pTuner);
	TS_PARAM(* GetTSParam)(struct sDEMOD *demod);
	int (* GetRegCmd)(struct sDEMOD *demod, unsigned int RegAddr, unsigned int *RegValue);//Add for outer module to get register
	int (* SetRegCmd)(struct sDEMOD *demod, unsigned int RegAddr, unsigned int RegValue);//Add for outer module to set register
	int (* GetTsClkRate)(struct sDEMOD *demod, unsigned int *pTsClkRate);

	//jp ext demod function
	int (* SetExtDemodInfo)(struct sDEMOD *demod, TV_SIG_INFO* pSigInfo);
	int (* GetSignalQuality_Ext)(struct sDEMOD *demod, TV_SYSTEM_TYPE sys_type, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality);
	int (* Init_Ext)(struct sDEMOD *demod, TV_SYSTEM_TYPE mode);
	int (* SetTSMode_Ext)(struct sDEMOD *demod, TS_PARAM* pParam);

	void (* Lock)(struct sDEMOD *demod);
	void (* Unlock)(struct sDEMOD *demod);

	void (*Destory)(struct sDEMOD *demod);
	int (*ATSgetdummy)(RTK_DEMOD_ATS_GET_DUMMY *ats_data);

	unsigned short m_id;
	unsigned char  m_addr;
	U32BITS  m_update_interval_ms;
	COMM*		   m_pComm;
	TUNER*		   m_pTuner;
	SCAN_RANGE	   m_ScanRange;
	U32BITS  m_Capability;
	CH_MODE	   m_ScanMode;

	U32BITS  m_clock;
	IF_PARAM	   m_if;
	TS_PARAM	   m_ts;
	TV_SYSTEM	   m_tv_sys;
	struct mutex   m_lock;
} DEMOD;

int ExtDemod_Init(DEMOD* pDemodKernel);
int ExtDemod_Reset(DEMOD* pDemodKernel);
int ExtDemod_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock);
int ExtDemod_ScanSignal(DEMOD* pDemodKernel, S32BITS* offset, SCAN_RANGE range);
int ExtDemod_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam);
int ExtDemod_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam);
int ExtDemod_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO* pSigInfo);
int ExtDemod_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode);
int ExtDemod_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys);
int ExtDemod_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock);
int ExtDemod_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo);
int ExtDemod_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality);
int ExtDemod_GetDemodStatus(DEMOD* pDemodKernel, U32BITS* pStatus);
int ExtDemod_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset);
int Demod_GetChannelInfo(DEMOD* pDemodKernel, unsigned char* Standard, unsigned char* Modulation, unsigned char* CodeRate, unsigned char* PilotOnOff, unsigned char* Inverse);
int ExtDemod_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize);
int ExtDemod_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off);
int ExtDemod_AutoTune(DEMOD* pDemodKernel);
int ExtDemod_KernelAutoTune(DEMOD* pDemodKernel);
U32BITS ExtDemod_GetCapability(DEMOD* pDemodKernel);
int ExtDemod_Suspend(DEMOD* pDemodKernel);
int ExtDemod_Resume(DEMOD* pDemodKernel);
int ExtDemod_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE sys_type);
int ExtDemod_InspectSignal(DEMOD* pDemodKernel);      // Add monitor function that used to monitor demod status
void ExtDemod_AttachTuner(DEMOD* pDemodKernel, TUNER* pTuner);
TS_PARAM ExtDemod_GetTSParam(DEMOD* pDemodKernel);
int ExtDemod_GetTsClkRate(DEMOD* pDemodKernel, unsigned int *pTsClkRate);
void ExtDemod_Lock(DEMOD* pDemodKernel);
void ExtDemod_Unlock(DEMOD* pDemodKernel);
void ExtDemod_Destructors(DEMOD* pDemodKernel);
void ExtDemod_Constructors(DEMOD* pDemodKernel);

//=============================================================
#define DEMOD_CALLER(fmt, args...)          rtd_extdemod_print(KERN_DEBUG,"DTV_CALLER, " fmt, ##args);
#define DEMOD_DBG(fmt, args...)          rtd_extdemod_print(KERN_DEBUG,"DTV_DEBUG, " fmt, ##args);
#define DEMOD_INFO(fmt, args...)               rtd_extdemod_print(KERN_INFO,"DTV_INFO, " fmt, ##args);
#define DEMOD_WARNING(fmt, args...)         rtd_extdemod_print(KERN_WARNING,"DTV_WARNING, " fmt, ##args);

#endif  //__DEMOD_EXT_HANDLE_H__
