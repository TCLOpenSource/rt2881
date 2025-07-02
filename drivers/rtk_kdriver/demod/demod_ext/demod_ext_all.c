/***************************************************************************************************
  File        : demod_rtk_all.cpp
  Description : REALTEK_ALL demod
  Author      : Kevin Wang
****************************************************************************************************
    Update List :
----------------------------------------------------------------------------------------------------
    20120312   | create phase
***************************************************************************************************/
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <linux/slab.h>
#include <rtk_kdriver/io.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include "demod_ext.h"
#include "demod_ext_all.h"
//#include "demod_rtk_r_atsc.h"
//#include "demod_rtk_r_atsc3.h"
//#include "demod_rtk_r_qam.h"
//#include "demod_rtk_a_dvbtx.h"
//#include "demod_rtk_a_dvbsx.h"
//#include "demod_rtk_r_dvbc.h"
//#include "demod_rtk_h_dtmb.h"
//#include "demod_rtk_r_dtmb.h"
//#include "demod_rtk_r_isdbt.h"
//#include "demod_rtk_r_atv.h"
#include "demod_ext_common.h"
//#include "diseqc_rtk.h"
//#include <rtk_kdriver/rtk_diseqc.h>
#include "rbus/dtv_frontend_reg.h"
#include "rbus/pll27x_reg_reg.h"

//#define SUPPORT_WINTOOL
#define AUTO_TEST_POLLING_INTERVAL (1000)

static int g_KernelAutoTuneThreadRunning = false;
//extern unsigned char TunerInputMode;

#ifdef SONY_ISDBT_EN
#include "demod_rtk_s_isdbt_cxd2856.h"
#include "demod_rtk_s_isdbt_cxd2878.h"
#endif

#ifdef SONY_ISDBS_EN
#include "demod_rtk_s_isdbs_cxd2856.h"
#include "demod_rtk_s_isdbs_cxd2878.h"
#include "demod_rtk_s_isdbs3_cxd2878.h"
#endif

#define ATSC3_I2C_ADDR 0x70

/*----------------------------------------------------------------------
 * Func : REALTEK_EXT_Constructors
 *
 * Desc : constructor
 *
 * Parm : device_addr : device address
 *        output_mode : REALTEK_ALL_OUT_IF_SERIAL / REALTEK_ALL_OUT_IF_PARALLEL
 *        output_freq : REALTEK_ALL_OUT_FREQ_6M / REALTEK_ALL_OUT_FREQ_10M
 *        pComm       : hadle of COMM
 *
 * Retn : handle of demod
 *----------------------------------------------------------------------*/

void REALTEK_EXT_Constructors(DEMOD* pDemodKernel, unsigned char addr, unsigned char output_mode, unsigned char output_freq, COMM* pComm)
{
	REALTEK_ALL* pRealtekAll;

	DEMOD_CALLER(" REALTEK_EXT_Constructors\n");
	pDemodKernel->private_data = kmalloc(sizeof(REALTEK_ALL), GFP_KERNEL);
	if (pDemodKernel->private_data == NULL) {
		goto kmalloc_fail;
	}

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	ExtDemod_Constructors(pDemodKernel);

	pDemodKernel->m_id                 = DTV_DEMOD_REALTEK_ALL;
	pDemodKernel->m_addr               = addr;
	pDemodKernel->m_pTuner             = NULL;
	pDemodKernel->m_pComm              = pComm;
	pRealtekAll->m_pDemod             = NULL;
#ifdef SONY_ISDBT_EN
	pRealtekAll->m_pSONYISDBTDemod     = NULL;
	pRealtekAll->m_pSONYISDBT_CXD2856     = NULL;
	pRealtekAll->m_pSONYISDBT_CXD2878  = NULL;
#endif
#ifdef SONY_ISDBS_EN
	pRealtekAll->m_pSONYISDBS_CXD2856     = NULL;
	pRealtekAll->m_pSONYISDBS_CXD2878  = NULL;
	pRealtekAll->m_pSONYISDBS3_CXD2878  = NULL;
#endif
	pRealtekAll->m_pATVDemod         	= NULL;
	pDemodKernel->m_ScanRange          = SCAN_RANGE_3_6;
	pDemodKernel->m_Capability         = TV_SYS_TYPE_ATSC | TV_SYS_TYPE_OPENCABLE | TV_SYS_TYPE_DVBT | TV_SYS_TYPE_DVBC | TV_SYS_TYPE_DTMB | TV_SYS_TYPE_ISDBT;
	pDemodKernel->m_update_interval_ms = 300;
	pDemodKernel->m_if.freq            = 36125000;
	pDemodKernel->m_if.inversion       = 0;
	pDemodKernel->m_if.agc_pola        = 0;

	pRealtekAll->m_output_mode        = output_mode;
	pRealtekAll->m_mode               = RTK_DEMOD_MODE_NONE;
	pRealtekAll->m_output_freq        = output_freq;
	pDemodKernel->m_ts.mode            = (pRealtekAll->m_output_mode == RTK_DEMOD_OUT_IF_PARALLEL) ? PARALLEL_MODE : SERIAL_MODE;
	pDemodKernel->m_ts.data_order      = MSB_D7;
	pDemodKernel->m_ts.datapin         = MSB_FIRST;
	pDemodKernel->m_ts.err_pol         = ACTIVE_HIGH;
	pDemodKernel->m_ts.sync_pol        = ACTIVE_HIGH;
	pDemodKernel->m_ts.val_pol         = ACTIVE_HIGH;
	pDemodKernel->m_ts.clk_pol         = FALLING_EDGE;
	pDemodKernel->m_ts.internal_demod_input = 1;
	pDemodKernel->m_ts.tsOutEn = 1;
	pDemodKernel->m_tv_sys = TV_SYS_UNKNOWN;

	pDemodKernel->Init = REALTEK_EXT_Init;
	pDemodKernel->Reset = ExtDemod_Reset;
	pDemodKernel->AcquireSignal = REALTEK_EXT_AcquireSignal;
	pDemodKernel->ScanSignal = REALTEK_EXT_ScanSignal;
	pDemodKernel->SetIF = REALTEK_EXT_SetIF;
	pDemodKernel->SetTSMode = REALTEK_EXT_SetTSMode;
	pDemodKernel->SetTVSysEx = REALTEK_EXT_SetTVSysEx;
	pDemodKernel->SetMode = REALTEK_EXT_SetMode;
	pDemodKernel->SetTVSys = REALTEK_EXT_SetTVSys;
	pDemodKernel->GetLockStatus = REALTEK_EXT_GetLockStatus;
	pDemodKernel->GetSignalInfo = REALTEK_EXT_GetSignalInfo;
	pDemodKernel->GetSignalQuality = REALTEK_EXT_GetSignalQuality;

	pDemodKernel->GetDemodStatus = ExtDemod_GetDemodStatus;
	pDemodKernel->GetCarrierOffset = REALTEK_EXT_GetCarrierOffset;
	pDemodKernel->GetChannelInfo = REALTEK_EXT_GetChannelInfo;
	pDemodKernel->GetDebugLogBuf = REALTEK_EXT_GetDebugLogBuf;
	pDemodKernel->ForwardI2C = REALTEK_EXT_ForwardI2C;
	pDemodKernel->AutoTune = REALTEK_EXT_AutoTune;
	pDemodKernel->ATSgetdummy = REALTEK_EXT_ATSGetDummy;
	//pDemodKernel->KernelAutoTune = REALTEK_EXT_KernelAutoTune;
	pDemodKernel->GetCapability = ExtDemod_GetCapability;
	pDemodKernel->Suspend = REALTEK_EXT_Suspend;
	pDemodKernel->Resume = REALTEK_EXT_Resume;
	pDemodKernel->SetTvMode = REALTEK_EXT_SetTvMode;
	pDemodKernel->InspectSignal = REALTEK_EXT_InspectSignal;	  // Add monitor function that used to monitor demod status
	pDemodKernel->AttachTuner = ExtDemod_AttachTuner;
	pDemodKernel->GetTSParam = ExtDemod_GetTSParam;
	pDemodKernel->GetDemodInfo = REALTEK_EXT_GetDemodInfo;
	pDemodKernel->GetRegCmd = REALTEK_EXT_GetRegCmd;
	pDemodKernel->SetRegCmd = REALTEK_EXT_SetRegCmd;
	pDemodKernel->Lock = ExtDemod_Lock;
	pDemodKernel->Unlock = ExtDemod_Unlock;
	pDemodKernel->GetTsClkRate = REALTEK_EXT_GetTsClkRate;

	pDemodKernel->SetExtDemodInfo = REALTEK_ALL_SetExtDemodInfo;
	pDemodKernel->GetSignalQuality_Ext = REALTEK_ALL_GetSignalQuality_Ext;
	pDemodKernel->SetTSMode_Ext = REALTEK_ALL_SetTSMode_Ext;

	pRealtekAll->Destory = REALTEK_EXT_Destructors;
	pRealtekAll->m_KernelAutoTuneThread_start = 0;
	pRealtekAll->demod_kernelthread = NULL;
	pRealtekAll->m_kernel_auto_tune_en = 0;
	pRealtekAll->m_auto_tune_en = 0;
	DEMOD_INFO("\033[0;32;31m"" REALTEK_EXT_Constructors done!\n""\033[m" );
//#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	//REALTEK_EXT_KernelThread_EN(pDemodKernel, 1);
//#endif

kmalloc_fail:
	DEMOD_WARNING("REALTEK_EXT_Constructors kmalloc Failed\n");
}



/*----------------------------------------------------------------------
 * Func :REALTEK_EXT_Destructors
 *
 * Desc : Destructor of REALTEK_ALL
 *
 * Parm : pDemod : handle of Demod
 *
 * Retn : N/A
 *----------------------------------------------------------------------*/
void REALTEK_EXT_Destructors(DEMOD* pDemodKernel)
{
	REALTEK_ALL* pRealtekAll;

	DEMOD_CALLER(" REALTEK_EXT_Destructors\n");

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	pDemodKernel->Lock(pDemodKernel);
	pRealtekAll->m_mode               = RTK_DEMOD_MODE_NONE;

#ifdef SONY_ISDBT_EN
	if (pRealtekAll->m_pSONYISDBT_CXD2856 != NULL)
		pRealtekAll->m_pSONYISDBT_CXD2856->Destory(pRealtekAll->m_pSONYISDBT_CXD2856);
	if (pRealtekAll->m_pSONYISDBT_CXD2878 != NULL)
		pRealtekAll->m_pSONYISDBT_CXD2878->Destory(pRealtekAll->m_pSONYISDBT_CXD2878);
#endif
#ifdef SONY_ISDBS_EN
	if (pRealtekAll->m_pSONYISDBS_CXD2856 != NULL)
		pRealtekAll->m_pSONYISDBS_CXD2856->Destory(pRealtekAll->m_pSONYISDBS_CXD2856);
	if (pRealtekAll->m_pSONYISDBS_CXD2878 != NULL)
		pRealtekAll->m_pSONYISDBS_CXD2878->Destory(pRealtekAll->m_pSONYISDBS_CXD2878);
	if (pRealtekAll->m_pSONYISDBS3_CXD2878 != NULL)
		pRealtekAll->m_pSONYISDBS3_CXD2878->Destory(pRealtekAll->m_pSONYISDBS3_CXD2878);
#endif

	if (pRealtekAll->m_pATVDemod != NULL)
		pRealtekAll->m_pATVDemod->Destory(pRealtekAll->m_pATVDemod);

	DEMOD_INFO("\033[0;32;31m"" REALTEK_EXT_Destructors done!\n""\033[m" );
	pDemodKernel->Unlock(pDemodKernel);

	kfree(pRealtekAll);

	//pDemodKernel->Destory(pDemodKernel);
}


int REALTEK_EXT_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys)
{
	DEMOD_CALLER("REALTEK_EXT_SetTVSys");
	return TUNER_CTRL_OK;
}


/*----------------------------------------------------------------------
 * Func : Init
 *
 * Desc : Initial REAKTEK_I
 *
 * Parm : N/A
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_Init(DEMOD* pDemodKernel)
{
	DEMOD_CALLER(" REALTEK_EXT_Init\n");
	return TUNER_CTRL_OK;
}



/*----------------------------------------------------------------------
 * Func : SetTVSys
 *
 * Desc : Set TV System
 *
 * Parm : sys : TV System
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO* pSigInfo)
{

	REALTEK_ALL* pRealtekAll;
	//TUNER_PARAM TunerParm;
	int ret = TUNER_CTRL_OK;
	//TV_SYSTEM_TYPE TvSysMode;

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);


	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m" , pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	//int pri_mode = pRealtekAll->m_mode;

	pRealtekAll->m_kernel_auto_tune_en = 0;
	pRealtekAll->m_auto_tune_en = 0;

	DEMOD_INFO("REALTEK_EXT_SetTVSysEx: system type=%d\n", sys);

	pDemodKernel->Lock(pDemodKernel);
	switch(sys)
	{
		case TV_SYS_ISDBT_CXD2856:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2856;
			pRealtekAll->m_mode   = RTK_DEMOD_MODE_ISDBT_EXT;
			break;
		case TV_SYS_ISDBT_CXD2878:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2878;
			pRealtekAll->m_mode   = RTK_DEMOD_MODE_ISDBT_EXT;
			break;
		case TV_SYS_ISDBS_CXD2856:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2856;
			pRealtekAll->m_mode   = RTK_DEMOD_MODE_ISDBS_EXT;
			break;
		case TV_SYS_ISDBS_CXD2878:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2878;
			pRealtekAll->m_mode   = RTK_DEMOD_MODE_ISDBS_EXT;
			break;
		case TV_SYS_ISDBS3_CXD2878:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS3_CXD2878;
			pRealtekAll->m_mode   = RTK_DEMOD_MODE_ISDBS3_EXT;
			break;
		default:
			DEMOD_INFO("\033[0;32;31m""Not JP system\n""\033[m");
	}

	ret = pRealtekAll->m_pDemod->SetTVSysEx(pRealtekAll->m_pDemod, sys, pSigInfo);

	pDemodKernel->Unlock(pDemodKernel);

	if (ret == TUNER_CTRL_OK)
		pDemodKernel->m_tv_sys = sys;


	pRealtekAll->m_kernel_auto_tune_en = 1;
	pRealtekAll->m_auto_tune_en = 1;

	return ret;
}



/*----------------------------------------------------------------------
 * Func : SetIF
 *
 * Desc : Set IF Parameter
 *
 * Parm : pParam : IF Param
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_SetIF\n");

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);

	pDemodKernel->m_if = *pParam;

	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->SetIF(pRealtekAll->m_pDemod, pParam);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}



/*----------------------------------------------------------------------
 * Func : SetMode
 *
 * Desc : Set operation mode of demod
 *
 * Parm : mode : Demod opreation mode
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode)
{
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_SetMode\n");

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	switch (mode) {
	case TV_OP_MODE_NORMAL:
	case TV_OP_MODE_SCANING:
		DEMOD_INFO("REALTEK_EXT_SetMode:TV_OP_MODE_NORMAL\n");
		//[todd] need implement
		//if (DtvDemodClockEnable(1))       // reenabler demod clock
		//	pDemodKernel->AcquireSignal(pDemodKernel, 0);        // reset demod....
		break;

	case TV_OP_MODE_STANDBY:
		//[todd] need implement

		DEMOD_INFO("REALTEK_EXT_SetMode:TV_OP_MODE_STANDBY\n");
		//DtvDemodClockEnable(0);          // suspend demod clock
		break;

	case TV_OP_MODE_ATV_NORMAL:
	case TV_OP_MODE_TP_ONLY:
		DEMOD_WARNING("REALTEK_EXT_SetMode: NOT DTV MODE; %s(line%d)\n", __FILE__, __LINE__);
		break;
	}
	return TUNER_CTRL_OK;
}



/*----------------------------------------------------------------------
 * Func : SetTSMode
 *
 * Desc : Set mode of transport stream output
 *
 * Parm : pParam : TS Param
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam)
{
	REALTEK_ALL* pRealtekAll;
	int ret = TUNER_CTRL_FAIL;
	DEMOD_CALLER(" REALTEK_EXT_SetTSMode\n");

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}


	pDemodKernel->Lock(pDemodKernel);

	if (pRealtekAll->m_pDemod && pRealtekAll->m_pDemod->SetTSMode(pRealtekAll->m_pDemod, pParam) == TUNER_CTRL_OK) {
		pDemodKernel->m_ts = *pParam;
		ret = TUNER_CTRL_OK;
	}
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}



/*----------------------------------------------------------------------
 * Func : GetLockStatus
 *
 * Desc : Get Demod Lock Status
 *
 * Parm : pLock : Lock Status
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock)
{
	REALTEK_ALL* pRealtekAll;
	int ret = TUNER_CTRL_FAIL;
	DEMOD_CALLER(" REALTEK_EXT_GetLockStatus\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m" , pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetLockStatus(pRealtekAll->m_pDemod, pLock);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}



/*----------------------------------------------------------------------
 * Func : GetSignalInfo
 *
 * Desc : Get Signal Information
 *
 * Parm : pInfo : Siganl Information
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_GetSignalInfo\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m" , pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetSignalInfo(pRealtekAll->m_pDemod, pInfo);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}


/*----------------------------------------------------------------------
 * Func : GetSignalQuality
 *
 * Desc : Get Signal Information
 *
 * Parm : id : Type of Quality Information
 *        pQuality : Siganl Quality
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_GetSignalQuality\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}


#ifdef SUPPORT_DEMOD_AUTOTEST
	if (PLL27X_REG_IFADC_1_get_pll27x_ifadc_dtv_cko_en(rtd_inl(PLL27X_REG_IFADC_1_reg)) == 0x1) {

		if (((rtd_inl(DTV_FRONTEND_DTV_FRONTEND_DUMMY5_reg) >> 31) & 0x1) == 0x1)
			return TUNER_CTRL_OK;

	}
#endif

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetSignalQuality(pRealtekAll->m_pDemod, id, pQuality);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}


/*----------------------------------------------------------------------
 * Func : GetChannelInfo
 *
 * Desc : Get the channel info.
 *
 * Parm :
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_GetChannelInfo(DEMOD* pDemodKernel, unsigned char* Standard, unsigned char* Modulation, unsigned char* CodeRate, unsigned char* PilotOnOff, unsigned char* Inverse)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_GetChannelInfo\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetChannelInfo(pRealtekAll->m_pDemod, Standard, Modulation, CodeRate, PilotOnOff, Inverse);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}


/*----------------------------------------------------------------------
 * Func : GetCarrierOffset
 *
 * Desc : Get the carrier offset.
 *
 * Parm : pOffset : carrier frequency offset output (unit : Hz)
 *
 *          Note: the offset value is only valid as demod is on lock stage.
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_GetCarrierOffset\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m" , pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetCarrierOffset(pRealtekAll->m_pDemod, pOffset);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}
/*----------------------------------------------------------------------
 * Func : GetDebugLogBuf
 *
 * Desc : Get the debug log Buf.
 *
 * Parm :
 *
 *
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetDebugLogBuf(pRealtekAll->m_pDemod, LogBufAddr, LogBufSize);
	pDemodKernel->Unlock(pDemodKernel);

	return ret;
}


/*----------------------------------------------------------------------
 * Func : AcquireSignal
 *
 * Desc : Reset Demodulator and Acquire Signal.
 *        This function should be called when channel changed.
 *
 * Parm : WaitSignalLock : wait until signal lock
 *          0 : don't wait,
 *          others: wait for signal lock
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_AcquireSignal\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	//pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->AcquireSignal(pRealtekAll->m_pDemod, WaitSignalLock);
	//pDemodKernel->Unlock(pDemodKernel);
	return ret;
}

/*----------------------------------------------------------------------
 * Func : Suspend
 *
 * Desc : Set Mode of REALTEK_ALL
 *
 * Parm : mode :    RTK_DEMOD_MODE_DVBT : DVBT mode
 *                  RTK_DEMOD_MODE_DVBC  : DVBC mode
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_Suspend(DEMOD* pDemodKernel)
{
	REALTEK_ALL* pRealtekAll;

	DEMOD_CALLER(" REALTEK_EXT_Suspend\n");

	pDemodKernel->Lock(pDemodKernel);
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);
	pRealtekAll->m_kernel_auto_tune_en = 0;
	pRealtekAll->m_auto_tune_en = 0;
	pRealtekAll->m_mode = RTK_DEMOD_MODE_NONE;
	//if (pRealtekAll->m_pDemod)
	//	ret = pRealtekAll->m_pDemod->Suspend(pRealtekAll->m_pDemod);
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_OK;
}


/*----------------------------------------------------------------------
 * Func : Resume
 *
 * Desc : Set Mode of REALTEK_ALL
 *
 * Parm : mode :    RTK_DEMOD_MODE_DVBT : DVBT mode
 *                  RTK_DEMOD_MODE_DVBC  : DVBC mode
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_Resume(DEMOD* pDemodKernel)
{
	int ret = TUNER_CTRL_OK;
	REALTEK_ALL* pRealtekAll;

	DEMOD_CALLER(" REALTEK_EXT_Resume\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	pDemodKernel->Lock(pDemodKernel);
	pRealtekAll->m_kernel_auto_tune_en = 0;
	pRealtekAll->m_auto_tune_en = 0;
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->Resume(pRealtekAll->m_pDemod);
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_OK;
}


/*----------------------------------------------------------------------
 * Func : ScanSignal
 *
 * Desc : Ask demodulator to Search signal within specified range
 *        This function should be called every time the channel changed.
 *
 * Parm : pOffset : Offset Output
 *        range   : Scan Range
 *          SCAN_RANGE_NONE : center only
 *          SCAN_RANGE_1_6  : (-)1/6 MHz
 *          SCAN_RANGE_2_6  : (-)2/6 MHz
 *          SCAN_RANGE_3_6  : (-)3/6 MHz
 *
 * Retn : TUNER_CTRL_OK        : signal found and locked
 *        TUNER_CTRL_FAIL      : error occured during scan channel
 *        TUNER_CTRL_NO_SIGNAL : no signal found in specified range
 *----------------------------------------------------------------------*/
int REALTEK_EXT_ScanSignal(DEMOD* pDemodKernel, S32BITS* pOffset, SCAN_RANGE range)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_ScanSignal\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->ScanSignal(pRealtekAll->m_pDemod, pOffset, range);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}


/*----------------------------------------------------------------------
 * Func : ForwardI2C
 *
 * Desc :
 *
 * Parm :
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off)
{
	DEMOD_CALLER(" REALTEK_EXT_ForwardI2C\n");
	return TUNER_CTRL_OK;
}




/*----------------------------------------------------------------------
 * Func : AutoTune
 *
 * Desc :
 *
 * Parm :
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_AutoTune(DEMOD* pDemodKernel)
{
	int ret = TUNER_CTRL_OK;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_AutoTune\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll != NULL) {
		if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
			DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
			return TUNER_CTRL_FAIL;
		}
		if (pRealtekAll->m_auto_tune_en == 1) {
			pDemodKernel->Lock(pDemodKernel);
			if (pRealtekAll->m_pDemod) {
				ret = pRealtekAll->m_pDemod->AutoTune(pRealtekAll->m_pDemod);
			}
			pDemodKernel->Unlock(pDemodKernel);
		}
	}

	return ret;
}

/*----------------------------------------------------------------------
 * Func : AutoTune
 *
 * Desc :
 *
 * Parm :
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_ATSGetDummy(RTK_DEMOD_ATS_GET_DUMMY *ats_data)
{
	#if 0
	TV_SIG_INFO SigInfo;
	TV_SYSTEM OSD_TV_System = TV_SYS_DVBC_8M;
	int result, result2;
	int OSD_autotest_mode;
	int OSD_load_setting;
	int OSD_BW_setting;
	unsigned long int OSD_freq_setting;
	int OSD_DTV_flag;
	int OSD_TV_mode;
	if (ats_onoff == 1) {

		result = rtd_inl(DTV_FRONTEND_DTV_FRONTEND_DUMMY5_reg);
		OSD_autotest_mode = (result >> 31) & 0x1;
		OSD_load_setting = (result >> 30) & 0x1;
		OSD_DTV_flag = (result >> 29) & 0x1;
		OSD_TV_mode = (result >> 25) & 0xf;
		OSD_BW_setting = (result >> 23) & 0x3;
		OSD_freq_setting = (result & 0x7FFFFF) * 1000; //hz

		SigInfo.dvbt2_ofdm.plp_select = 0;
		SigInfo.ofdm.hierarchy = 0;
		/*
			TV_mode :
			0 : ATSC
			1 : DVBT
			2 : DVBT2
			3 : DVBC
			4 : DVBS
			5 : DVBS2
			6 : OpenCable : 64qam
			7 : OpenCable : 256qam
			8 : DTMB
			9 : ISDBT
			*/
		switch (OSD_TV_mode) {
		case 0:
			OSD_TV_System = TV_SYS_ATSC;
			break;
		case 1:
			switch (OSD_BW_setting) {
			case 1:
				OSD_TV_System = TV_SYS_DVBT_6M;
				SigInfo.mod = TV_MODULATION_OFDM;
				break;
			case 2:
				OSD_TV_System = TV_SYS_DVBT_7M;
				SigInfo.mod = TV_MODULATION_OFDM;
				break;
			case 3:
				OSD_TV_System = TV_SYS_DVBT_8M;
				SigInfo.mod = TV_MODULATION_OFDM;
				break;
			}
			break;
		case 2:
			switch (OSD_BW_setting) {
			case 0:
				OSD_TV_System = TV_SYS_DVBT2_1D7M;
				SigInfo.mod = TV_MODULATION_DVBT2_OFDM;
				break;
			case 1:
				OSD_TV_System = TV_SYS_DVBT2_6M;
				SigInfo.mod = TV_MODULATION_DVBT2_OFDM;
				break;
			case 2:
				OSD_TV_System = TV_SYS_DVBT2_7M;
				SigInfo.mod = TV_MODULATION_DVBT2_OFDM;
				break;
			case 3:
				OSD_TV_System = TV_SYS_DVBT2_8M;
				SigInfo.mod = TV_MODULATION_DVBT2_OFDM;
				break;
			}
			break;
		case 3:
			switch (OSD_BW_setting) {
			case 1:
				OSD_TV_System = TV_SYS_DVBC_6M;
				SigInfo.mod = TV_MODULATION_DVBC_QAM;
				break;
			case 2:
				OSD_TV_System = TV_SYS_DVBC_7M;
				SigInfo.mod = TV_MODULATION_DVBC_QAM;
				break;
			case 3:
				OSD_TV_System = TV_SYS_DVBC_8M;
				SigInfo.mod = TV_MODULATION_DVBC_QAM;
				break;
			}
			break;
		case 4:
			OSD_TV_System = TV_SYS_DVBS;
			result2 = rtd_inl(DTV_FRONTEND_DTV_FRONTEND_DUMMY4_reg);
			SigInfo.dvbs.symbol_rate = (result2 & 0x1ffff) * 1000;
			break;
		case 5:
			OSD_TV_System = TV_SYS_DVBS2;
			result2 = rtd_inl(DTV_FRONTEND_DTV_FRONTEND_DUMMY4_reg);
			SigInfo.dvbs.symbol_rate = (result2 & 0x1ffff) * 1000;
			break;
		case 6:
			OSD_TV_System = TV_SYS_OPENCABLE_64;
			break;
		case 7:
			OSD_TV_System = TV_SYS_OPENCABLE_64;
			break;
		case 8:
			OSD_TV_System = TV_SYS_DTMB;
			break;
		case 9:
			switch (OSD_BW_setting) {
			case 1:
				OSD_TV_System = TV_SYS_ISDBT_6M;
				break;
			case 2:
				OSD_TV_System = TV_SYS_ISDBT_7M;
				break;
			case 3:
				OSD_TV_System = TV_SYS_ISDBT_8M;
				break;
			}
			break;
		case 10:
			OSD_TV_System = TV_SYS_ABS_S;
			break;

		}

		ats_data->freq = OSD_freq_setting;
		ats_data->TVSys = OSD_TV_System;
		ats_data->SigInfo = SigInfo;

		if (OSD_autotest_mode && OSD_load_setting && OSD_DTV_flag) {
			rtd_maskl(DTV_FRONTEND_DTV_FRONTEND_DUMMY5_reg, ~(0x1 << 30), 0x0); //ACK
			return 1;
		} else
			return 0;

	} else
	#endif
		return 0;

}
int REALTEK_EXT_KernelAutoTune(void* pDemodKernel)
{
	int ret = TUNER_CTRL_OK;
	REALTEK_ALL* pRealtekAll;
	//pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	DEMOD_CALLER(" REALTEK_EXT_KernelAutoTune\n");


	while (!kthread_should_stop())

	{

		g_KernelAutoTuneThreadRunning = true;
		pRealtekAll = (REALTEK_ALL*)(((DEMOD*)pDemodKernel)->private_data);


		if (pRealtekAll != NULL && pRealtekAll->m_kernel_auto_tune_en == 1) {
			((DEMOD*)pDemodKernel)->Lock(pDemodKernel);
			if (pRealtekAll->m_pDemod && PLL27X_REG_IFADC_1_get_pll27x_ifadc_dtv_cko_en(rtd_inl(PLL27X_REG_IFADC_1_reg)) == 0x1) {
				ret = pRealtekAll->m_pDemod->KernelAutoTune(pRealtekAll->m_pDemod);

				#if 0
				if (ats_onoff == 1) {
					TV_SIG_QUAL SigQuality;
					int result;
					int OSD_autotest_mode;
					int SignalInfo_Load;
					int SignalInfo_Type;
					unsigned char SignalInfo_Lock;

					result = rtd_inl(DTV_FRONTEND_DTV_FRONTEND_DUMMY5_reg);
					OSD_autotest_mode = (result >> 31) & 0x1;
					result = rtd_inl(DTV_FRONTEND_DTV_FRONTEND_DUMMY4_reg);
					SignalInfo_Load = (result >> 31) & 0x1;
					SignalInfo_Type = (result >> 17) & 0x1fff;
					//DEMOD_INFO("[OSD][%d] SignalInfo_Load=%d, SignalInfo_Type=%d\n", osd_i,SignalInfo_Load, SignalInfo_Type);

					/*
						SignalInfo_Type :
						0 : Lock Status
						1 : Ber
						2 : Per
						3 : SNR
					*/

					if (OSD_autotest_mode) {
						if (SignalInfo_Load == 1) {
							//DTVFE_CTRL_INFO("[OSD][%d] Get Per.... \n",osd_i);
							switch (SignalInfo_Type) {
							case 0:
								ret = pRealtekAll->m_pDemod->GetLockStatus(pRealtekAll->m_pDemod, &SignalInfo_Lock);
								rtd_outl(DTV_FRONTEND_DTV_FRONTEND_DUMMY3_reg, SignalInfo_Lock);
								break;
							case 1:
								ret = pRealtekAll->m_pDemod->GetSignalQuality(pRealtekAll->m_pDemod, TV_QUAL_BEC, &SigQuality);
								rtd_outl(DTV_FRONTEND_DTV_FRONTEND_DUMMY2_reg, SigQuality.layer_ber.ber_num);
								rtd_outl(DTV_FRONTEND_DTV_FRONTEND_DUMMY3_reg, SigQuality.layer_ber.ber_den);
								break;
							case 2:
								ret = pRealtekAll->m_pDemod->GetSignalQuality(pRealtekAll->m_pDemod, TV_QUAL_PEC, &SigQuality);
								rtd_outl(DTV_FRONTEND_DTV_FRONTEND_DUMMY_reg, SigQuality.layer_per.per_num);
								rtd_outl(DTV_FRONTEND_DTV_FRONTEND_DUMMY3_reg, SigQuality.layer_per.per_den);
								break;
							case 3:
								ret = pRealtekAll->m_pDemod->GetSignalQuality(pRealtekAll->m_pDemod, TV_QUAL_SNR, &SigQuality);
								rtd_outl(DTV_FRONTEND_DTV_FRONTEND_DUMMY3_reg, SigQuality.snr);
								break;
							default:
								break;
							}
							rtd_maskl(DTV_FRONTEND_DTV_FRONTEND_DUMMY4_reg, ~(0x1 << 31), 0x0); //clear SignalInfo_Load
						}
					}
				}
				#endif
			}
			((DEMOD*)pDemodKernel)->Unlock((DEMOD*)pDemodKernel);
		}
		tv_osal_msleep_ext(AUTO_TEST_POLLING_INTERVAL);
	}
	g_KernelAutoTuneThreadRunning = false;
	return ret;
}

/*----------------------------------------------------------------------
 * Func : InspectSignal
 *
 * Desc : Inspect Signal Info (for Debug Purpose)
 *
 * Parm : pBuff : message buffer
 *        BufferSize : size of message buffer
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_InspectSignal(DEMOD* pDemodKernel)
{
	int ret = TUNER_CTRL_OK;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_EXT_InspectSignal\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m", pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);

	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->InspectSignal(pRealtekAll->m_pDemod);

	pDemodKernel->Unlock(pDemodKernel);
	return ret;
}

/*----------------------------------------------------------------------
 * Func : SetTvMode
 *
 * Desc : Set Mode of REALTEK_ALL
 *
 * Parm : mode :    RTK_DEMOD_MODE_DVBT : DVBT mode
 *                  RTK_DEMOD_MODE_DVBC  : DVBC mode
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	RTK_DEMOD_INFO("REALTEK_EXT_SetTvMode\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);
	pRealtekAll->m_kernel_auto_tune_en = 0;
	pRealtekAll->m_auto_tune_en = 0;
	pRealtekAll->m_mode               = RTK_DEMOD_MODE_NONE;
	//REALTEK_EXT_KernelThread_EN(pDemodKernel, 0);
	//RTK_DEMOD_INFO("REALTEK_EXT_SetTvMode REALTEK_EXT_KernelThread_EN end\n");
	pDemodKernel->Lock(pDemodKernel);

	switch (mode) {
#ifdef SONY_ISDBT_EN
	case TV_SYS_TYPE_ISDBT_CXD2856:
	case TV_SYS_TYPE_ISDBT_CXD2856_2nd:
		if (pRealtekAll->m_pSONYISDBT_CXD2856 == NULL) {
			RTK_DEMOD_INFO("Open New External ISDBT lib and Set to ISDBT CXD2856\n");
			pRealtekAll->m_pSONYISDBT_CXD2856 = kmalloc(sizeof(DEMOD), GFP_KERNEL);
			if (pRealtekAll->m_pSONYISDBT_CXD2856 == NULL) {
				DEMOD_WARNING("REALTEK_EXT_SetTvMode kmalloc failed\n");
				kfree(pRealtekAll->m_pSONYISDBT_CXD2856);
				goto set_demod_mode_failed;
			}
			CXD2856_S_ISDBT_Constructors(pRealtekAll->m_pSONYISDBT_CXD2856, pDemodKernel->m_addr, pRealtekAll->m_output_mode, pRealtekAll->m_output_freq, pDemodKernel->m_pComm);
			pRealtekAll->m_pSONYISDBT_CXD2856->AttachTuner(pRealtekAll->m_pSONYISDBT_CXD2856, pDemodKernel->m_pTuner);
			ret  = pRealtekAll->m_pSONYISDBT_CXD2856->SetTvMode(pRealtekAll->m_pSONYISDBT_CXD2856, mode);
			//ret |= pRealtekAll->m_pSONYISDBT_CXD2856->SetTSMode(pRealtekAll->m_pSONYISDBT_CXD2856, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBT CXD2856 mode failed\n");
				pRealtekAll->m_pSONYISDBT_CXD2856->Destory(pRealtekAll->m_pSONYISDBT_CXD2856);
				kfree(pRealtekAll->m_pSONYISDBT_CXD2856);
				pRealtekAll->m_pSONYISDBT_CXD2856 = NULL;
				goto set_demod_mode_failed;
			}
		} else {
			RTK_DEMOD_INFO("Set to ISDBT CXD2856\n");
			ret  = pRealtekAll->m_pSONYISDBT_CXD2856->SetTvMode(pRealtekAll->m_pSONYISDBT_CXD2856, mode);
			//ret |= pRealtekAll->m_pSONYISDBT_CXD2856->SetTSMode(pRealtekAll->m_pSONYISDBT_CXD2856, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBT CXD2856 failed\n");
				pRealtekAll->m_pSONYISDBT_CXD2856->Destory(pRealtekAll->m_pSONYISDBT_CXD2856);
				kfree(pRealtekAll->m_pSONYISDBT_CXD2856);
				pRealtekAll->m_pSONYISDBT_CXD2856 = NULL;
				goto set_demod_mode_failed;
			}
		}

		pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2856;
		pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBT_EXT;
		break;
	case TV_SYS_TYPE_ISDBT_CXD2878:
	case TV_SYS_TYPE_ISDBT_CXD2878_2nd:
		if (pRealtekAll->m_pSONYISDBT_CXD2878 == NULL) {
			RTK_DEMOD_INFO("Open New External ISDBT lib and Set to ISDBT CXD2878\n");
			pRealtekAll->m_pSONYISDBT_CXD2878 = kmalloc(sizeof(DEMOD), GFP_KERNEL);
			if (pRealtekAll->m_pSONYISDBT_CXD2878 == NULL) {
				DEMOD_WARNING("REALTEK_EXT_SetTvMode kmalloc failed\n");
				kfree(pRealtekAll->m_pSONYISDBT_CXD2878);
				goto set_demod_mode_failed;
			}
			CXD2878_S_ISDBT_Constructors(pRealtekAll->m_pSONYISDBT_CXD2878, pDemodKernel->m_addr, pRealtekAll->m_output_mode, pRealtekAll->m_output_freq, pDemodKernel->m_pComm);
			pRealtekAll->m_pSONYISDBT_CXD2878->AttachTuner(pRealtekAll->m_pSONYISDBT_CXD2878, pDemodKernel->m_pTuner);
			ret  = pRealtekAll->m_pSONYISDBT_CXD2878->SetTvMode(pRealtekAll->m_pSONYISDBT_CXD2878, mode);
			//ret |= pRealtekAll->m_pSONYISDBT_CXD2878->SetTSMode(pRealtekAll->m_pSONYISDBT_CXD2878, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBT CXD2878 mode failed\n");
				pRealtekAll->m_pSONYISDBT_CXD2878->Destory(pRealtekAll->m_pSONYISDBT_CXD2878);
				kfree(pRealtekAll->m_pSONYISDBT_CXD2878);
				pRealtekAll->m_pSONYISDBT_CXD2878 = NULL;
				goto set_demod_mode_failed;
			}
		} else {
			RTK_DEMOD_INFO("Set to ISDBT CXD2878\n");
			ret  = pRealtekAll->m_pSONYISDBT_CXD2878->SetTvMode(pRealtekAll->m_pSONYISDBT_CXD2878, mode);
			//ret |= pRealtekAll->m_pSONYISDBT_CXD2878->SetTSMode(pRealtekAll->m_pSONYISDBT_CXD2878, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBT CXD2878 failed\n");
				pRealtekAll->m_pSONYISDBT_CXD2878->Destory(pRealtekAll->m_pSONYISDBT_CXD2878);
				kfree(pRealtekAll->m_pSONYISDBT_CXD2878);
				pRealtekAll->m_pSONYISDBT_CXD2878 = NULL;
				goto set_demod_mode_failed;
			}
		}

		pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2878;
		pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBT_EXT;
		break;
#endif
#ifdef SONY_ISDBS_EN
	case TV_SYS_TYPE_ISDBS_CXD2856:
	case TV_SYS_TYPE_ISDBS_CXD2856_2nd:
		if (pRealtekAll->m_pSONYISDBS_CXD2856 == NULL) {
			RTK_DEMOD_INFO("Open New External ISDBS lib and Set to ISDBS CXD2856\n");
			pRealtekAll->m_pSONYISDBS_CXD2856 = kmalloc(sizeof(DEMOD), GFP_KERNEL);
			if (pRealtekAll->m_pSONYISDBS_CXD2856 == NULL) {
				DEMOD_WARNING("REALTEK_EXT_SetTvMode kmalloc failed\n");
				kfree(pRealtekAll->m_pSONYISDBS_CXD2856);
				goto set_demod_mode_failed;
			}
			CXD2856_S_ISDBS_Constructors(pRealtekAll->m_pSONYISDBS_CXD2856, pDemodKernel->m_addr, pRealtekAll->m_output_mode, pRealtekAll->m_output_freq, pDemodKernel->m_pComm);
			pRealtekAll->m_pSONYISDBS_CXD2856->AttachTuner(pRealtekAll->m_pSONYISDBS_CXD2856, pDemodKernel->m_pTuner);
			ret  = pRealtekAll->m_pSONYISDBS_CXD2856->SetTvMode(pRealtekAll->m_pSONYISDBS_CXD2856, mode);
			//ret |= pRealtekAll->m_pSONYISDBS_CXD2856->SetTSMode(pRealtekAll->m_pSONYISDBS_CXD2856, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBS CXD2856 mode failed\n");
				pRealtekAll->m_pSONYISDBS_CXD2856->Destory(pRealtekAll->m_pSONYISDBS_CXD2856);
				kfree(pRealtekAll->m_pSONYISDBS_CXD2856);
				pRealtekAll->m_pSONYISDBS_CXD2856 = NULL;
				goto set_demod_mode_failed;
			}
		} else {
			RTK_DEMOD_INFO("Set to ISDBS CXD2856\n");
			ret  = pRealtekAll->m_pSONYISDBS_CXD2856->SetTvMode(pRealtekAll->m_pSONYISDBS_CXD2856, mode);
			//ret |= pRealtekAll->m_pSONYISDBS_CXD2856->SetTSMode(pRealtekAll->m_pSONYISDBS_CXD2856, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBS CXD2856 failed\n");
				pRealtekAll->m_pSONYISDBS_CXD2856->Destory(pRealtekAll->m_pSONYISDBS_CXD2856);
				kfree(pRealtekAll->m_pSONYISDBS_CXD2856);
				pRealtekAll->m_pSONYISDBS_CXD2856 = NULL;
				goto set_demod_mode_failed;
			}
		}

		pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2856;
		pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS_EXT;
		break;
	case TV_SYS_TYPE_ISDBS_CXD2878:
	case TV_SYS_TYPE_ISDBS_CXD2878_2nd:
		if (pRealtekAll->m_pSONYISDBS_CXD2878 == NULL) {
			RTK_DEMOD_INFO("Open New External ISDBS lib and Set to ISDBS CXD2878\n");
			pRealtekAll->m_pSONYISDBS_CXD2878 = kmalloc(sizeof(DEMOD), GFP_KERNEL);
			if (pRealtekAll->m_pSONYISDBS_CXD2878 == NULL) {
				DEMOD_WARNING("REALTEK_EXT_SetTvMode kmalloc failed\n");
				kfree(pRealtekAll->m_pSONYISDBS_CXD2878);
				goto set_demod_mode_failed;
			}
			CXD2878_S_ISDBS_Constructors(pRealtekAll->m_pSONYISDBS_CXD2878, pDemodKernel->m_addr, pRealtekAll->m_output_mode, pRealtekAll->m_output_freq, pDemodKernel->m_pComm);
			pRealtekAll->m_pSONYISDBS_CXD2878->AttachTuner(pRealtekAll->m_pSONYISDBS_CXD2878, pDemodKernel->m_pTuner);
			ret  = pRealtekAll->m_pSONYISDBS_CXD2878->SetTvMode(pRealtekAll->m_pSONYISDBS_CXD2878, mode);
			//ret |= pRealtekAll->m_pSONYISDBS_CXD2878->SetTSMode(pRealtekAll->m_pSONYISDBS_CXD2878, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBS CXD2878 mode failed\n");
				pRealtekAll->m_pSONYISDBS_CXD2878->Destory(pRealtekAll->m_pSONYISDBS_CXD2878);
				kfree(pRealtekAll->m_pSONYISDBS_CXD2878);
				pRealtekAll->m_pSONYISDBS_CXD2878 = NULL;
				goto set_demod_mode_failed;
			}
		} else {
			RTK_DEMOD_INFO("Set to ISDBS CXD2878\n");
			ret  = pRealtekAll->m_pSONYISDBS_CXD2878->SetTvMode(pRealtekAll->m_pSONYISDBS_CXD2878, mode);
			//ret |= pRealtekAll->m_pSONYISDBS_CXD2878->SetTSMode(pRealtekAll->m_pSONYISDBS_CXD2878, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBS CXD2878 failed\n");
				pRealtekAll->m_pSONYISDBS_CXD2878->Destory(pRealtekAll->m_pSONYISDBS_CXD2878);
				kfree(pRealtekAll->m_pSONYISDBS_CXD2878);
				pRealtekAll->m_pSONYISDBS_CXD2878 = NULL;
				goto set_demod_mode_failed;
			}
		}

		pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2878;
		pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS_EXT;
		break;
	case TV_SYS_TYPE_ISDBS3_CXD2878:
	case TV_SYS_TYPE_ISDBS3_CXD2878_2nd:
		if (pRealtekAll->m_pSONYISDBS3_CXD2878 == NULL) {
			RTK_DEMOD_INFO("Open New External ISDBS3 lib and Set to ISDBS3 CXD2878\n");
			pRealtekAll->m_pSONYISDBS3_CXD2878 = kmalloc(sizeof(DEMOD), GFP_KERNEL);
			if (pRealtekAll->m_pSONYISDBS3_CXD2878 == NULL) {
				DEMOD_WARNING("REALTEK_EXT_SetTvMode kmalloc failed\n");
				kfree(pRealtekAll->m_pSONYISDBS3_CXD2878);
				goto set_demod_mode_failed;
			}
			CXD2878_S_ISDBS3_Constructors(pRealtekAll->m_pSONYISDBS3_CXD2878, pDemodKernel->m_addr, pRealtekAll->m_output_mode, pRealtekAll->m_output_freq, pDemodKernel->m_pComm);
			pRealtekAll->m_pSONYISDBS3_CXD2878->AttachTuner(pRealtekAll->m_pSONYISDBS3_CXD2878, pDemodKernel->m_pTuner);
			ret  = pRealtekAll->m_pSONYISDBS3_CXD2878->SetTvMode(pRealtekAll->m_pSONYISDBS3_CXD2878, mode);
			//ret |= pRealtekAll->m_pSONYISDBS3_CXD2878->SetTSMode(pRealtekAll->m_pSONYISDBS3_CXD2878, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBS3 CXD2878 mode failed\n");
				pRealtekAll->m_pSONYISDBS3_CXD2878->Destory(pRealtekAll->m_pSONYISDBS3_CXD2878);
				kfree(pRealtekAll->m_pSONYISDBS3_CXD2878);
				pRealtekAll->m_pSONYISDBS3_CXD2878 = NULL;
				goto set_demod_mode_failed;
			}
		} else {
			RTK_DEMOD_INFO("Set to ISDBS3 CXD2878\n");
			ret  = pRealtekAll->m_pSONYISDBS3_CXD2878->SetTvMode(pRealtekAll->m_pSONYISDBS3_CXD2878, mode);
			//ret |= pRealtekAll->m_pSONYISDBS3_CXD2878->SetTSMode(pRealtekAll->m_pSONYISDBS3_CXD2878, &pDemodKernel->m_ts);

			if (ret != TUNER_CTRL_OK) {
				DEMOD_WARNING("Set REALTEK_ALL to ISDBS3 CXD2878 failed\n");
				pRealtekAll->m_pSONYISDBS3_CXD2878->Destory(pRealtekAll->m_pSONYISDBS3_CXD2878);
				kfree(pRealtekAll->m_pSONYISDBS3_CXD2878);
				pRealtekAll->m_pSONYISDBS3_CXD2878 = NULL;
				goto set_demod_mode_failed;
			}
		}

		pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS3_CXD2878;
		pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS3_EXT;
		break;
#endif
	case TV_SYS_TYPE_UNKNOWN:

		DEMOD_WARNING("UNKNOW TV SYSTEM !!! ExtDemod is in sleep mode~~ \n");
		rtd_pr_demod_warn("LowPower::info::demod::enter sleep mode !!!");

		ret = TUNER_CTRL_OK;
		goto set_demod_mode_unknow_sys_end;
		break;

	default:
		goto set_demod_mode_failed;
	}

	pDemodKernel->m_update_interval_ms = pRealtekAll->m_pDemod->m_update_interval_ms;
	pDemodKernel->m_ScanRange = pRealtekAll->m_pDemod->m_ScanRange;
	ret = TUNER_CTRL_OK;
	RTK_DEMOD_INFO("Set REALTEK_ALL to appointed DTV mode successed\n");


	pDemodKernel->Unlock(pDemodKernel);
	//REALTEK_EXT_KernelThread_EN(pDemodKernel, 1);
	//RTK_DEMOD_INFO("REALTEK_EXT_SetTvMode REALTEK_EXT_KernelThread_EN start\n");
	return ret;

set_demod_mode_failed:
	pRealtekAll->m_pDemod = NULL;
	pRealtekAll->m_mode               = RTK_DEMOD_MODE_NONE;
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;

set_demod_mode_unknow_sys_end:
	pRealtekAll->m_pDemod = NULL;
	pRealtekAll->m_mode               = RTK_DEMOD_MODE_NONE;
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_OK;


}


int REALTEK_EXT_GetDemodInfo(DEMOD* pDemodKernel, TV_DEMOD_INFO* pDemodInfo)
{
	RTK_DEMOD_CALLER("REALTEK_EXT_GetDemodInfo\n");

	pDemodInfo -> IcName = REALTEKALL;
	pDemodInfo -> IcVersion = 0;
	pDemodInfo -> PGAMode = 0;
	pDemodInfo -> PGAGain = 0;
	pDemodInfo -> IfPathType = 0;

	return TUNER_CTRL_OK;
}


int REALTEK_EXT_KernelThread_EN(DEMOD* pDemodKernel, unsigned int thread_en)
{

	REALTEK_ALL* pRealtekAll;
	struct task_struct *demod_kernelthread;
	int ret = TUNER_CTRL_OK;
	int waitThreadStopCount = 20;
	RTK_DEMOD_INFO("REALTEK_EXT_KernelThread_EN = %u\n", thread_en);

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	pDemodKernel->Lock(pDemodKernel);
	
	if (thread_en) {
		if (pRealtekAll->demod_kernelthread != NULL) {
			RTK_DEMOD_INFO("Demod kernel thread is already existed\n");
		} else {
			demod_kernelthread = kthread_run(REALTEK_EXT_KernelAutoTune, pDemodKernel, "KernelAutoTune");    /* no need to pass parameter into thread function */
			if (!IS_ERR(demod_kernelthread)) {
				pRealtekAll->demod_kernelthread = demod_kernelthread;
			} else {
				pDemodKernel->Unlock(pDemodKernel);
				RTK_DEMOD_INFO("create KernelAutoTune thread failed\n");
				return TUNER_CTRL_FAIL;
			}
		}
		pRealtekAll->m_KernelAutoTuneThread_start = 1;
	} else {
		if (pRealtekAll->demod_kernelthread == NULL) {
			RTK_DEMOD_INFO("KernelAutoTune thread does not exist, just exit\n");
		} else {
			pRealtekAll->m_KernelAutoTuneThread_start = 0;
			pDemodKernel->Unlock(pDemodKernel);
			/* it will wait until thread function exit */
			ret = kthread_stop(pRealtekAll->demod_kernelthread);
			pDemodKernel->Lock(pDemodKernel);
			while(g_KernelAutoTuneThreadRunning && waitThreadStopCount > 0){
				waitThreadStopCount--;
				RTK_DEMOD_INFO("waitting kernelautotune thread stop waitThreadStopCount=%d !! %d", waitThreadStopCount, ret);
				tv_osal_msleep_ext(20);
			}
			if (ret < 0) {
				RTK_DEMOD_INFO("call kthread_stop return error = %d", ret);
				pDemodKernel->Unlock(pDemodKernel);
				return TUNER_CTRL_FAIL;
			}
			pRealtekAll->demod_kernelthread = NULL;
		}

	}

	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_OK;

}

/*----------------------------------------------------------------------
 * Func : GetRegCmd
 *
 * Desc : Get register value from out kernel caller
 *
 * Parm : RegAddr : reg addr
 *        RegValue : value of reg
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int REALTEK_EXT_GetRegCmd(DEMOD* pDemodKernel,unsigned int RegAddr, unsigned int *RegValue)
{
	int ret = TUNER_CTRL_OK;
	DEMOD_CALLER("REALTEK_EXT_GetRegCmd\n");
	if (((RegAddr&0xFFFFFF00) == 0xb8000800)||((RegAddr&0xFFFFFF00) == 0x18000800))	{
	pDemodKernel->Lock(pDemodKernel);
	*RegValue = rtd_inl(RegAddr);
	pDemodKernel->Unlock(pDemodKernel);

	RTK_DEMOD_INFO("GetRegdata RegAddr=0x%x RegValue=0x%x \n",RegAddr, *RegValue);
	} else {
		*RegValue = 0;
		RTK_DEMOD_INFO("out of supported address!!\n");

	}

	return ret;
}

/*----------------------------------------------------------------------
 * Func : SetRegCmd
 *
 * Desc : Set register value from out kernel caller
 *
 * Parm : RegAddr : reg addr
 *        RegValue : value of reg
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/

int REALTEK_EXT_SetRegCmd(DEMOD* pDemodKernel,unsigned int RegAddr, unsigned int RegValue)
{
	int ret = TUNER_CTRL_OK;
	DEMOD_CALLER("REALTEK_EXT_SetRegCmd\n");

	if (((RegAddr&0xFFFFFF00) == 0xb8000800)||((RegAddr&0xFFFFFF00) == 0x18000800))	{
	RTK_DEMOD_INFO("SetRegdata addr=0x%x value=0x%x \n", RegAddr, RegValue);
	pDemodKernel->Lock(pDemodKernel);
	rtd_outl(RegAddr,RegValue);
	pDemodKernel->Unlock(pDemodKernel);
	} else {
		RTK_DEMOD_INFO("out of  supported address!!\n");
	}

	return ret;
}


int REALTEK_EXT_GetTsClkRate(DEMOD* pDemodKernel, unsigned int *pTsClkRate)
{
	REALTEK_ALL* pRealtekAll;
	int ret = TUNER_CTRL_OK;
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	DEMOD_CALLER("REALTEK_EXT_GetTsClkRate\n");

	pDemodKernel->Lock(pDemodKernel);
	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetTsClkRate(pRealtekAll->m_pDemod, pTsClkRate);
	RTK_DEMOD_DBG("\033[1;32;33m" "[REALTEK_EXT_GetTsClkRate] TsClkKHz = %u\n" "\033[m", *pTsClkRate);
	pDemodKernel->Unlock(pDemodKernel);

	return ret;
}

int REALTEK_ALL_SetExtDemodInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pSigInfo)
{
	int ret = TUNER_CTRL_OK;
	REALTEK_ALL* pRealtekAll;
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);
	DEMOD_CALLER("REALTEK_ALL_SetExtDemodInfo \n");

	pDemodKernel->Lock(pDemodKernel);
	switch(pSigInfo->mod)
	{
		case TV_MODULATION_ISDBT:
			RTK_DEMOD_INFO("[ALL]TV_MODULATION_ISDBT\n");
			if(pSigInfo->isdbt.portIdx == 0xc8) {
				if (pRealtekAll->m_pSONYISDBT_CXD2856)  ret = pRealtekAll->m_pSONYISDBT_CXD2856->SetExtDemodInfo(pRealtekAll->m_pDemod, pSigInfo);
			}
			else {
				if (pRealtekAll->m_pSONYISDBT_CXD2878)  ret = pRealtekAll->m_pSONYISDBT_CXD2878->SetExtDemodInfo(pRealtekAll->m_pDemod, pSigInfo);
			}
			break;
		case TV_MODULATION_ISDBS:
			RTK_DEMOD_INFO("[ALL]TV_MODULATION_ISDBS\n");
			if(pSigInfo->isdbs_ext.portIdx == 0xc8) {
				if (pRealtekAll->m_pSONYISDBS_CXD2856)  ret = pRealtekAll->m_pSONYISDBS_CXD2856->SetExtDemodInfo(pRealtekAll->m_pDemod, pSigInfo);
			}
			else {
				if (pRealtekAll->m_pSONYISDBS_CXD2878)  ret = pRealtekAll->m_pSONYISDBS_CXD2878->SetExtDemodInfo(pRealtekAll->m_pDemod, pSigInfo);
			}
			break;
		case TV_MODULATION_ISDBS3:
			RTK_DEMOD_INFO("[ALL]TV_MODULATION_ISDBS3\n");
			if (pRealtekAll->m_pSONYISDBS3_CXD2878)  ret = pRealtekAll->m_pSONYISDBS3_CXD2878->SetExtDemodInfo(pRealtekAll->m_pDemod, pSigInfo);
			break;
		default:
			RTK_DEMOD_INFO("Unknow TV modulation!!! mod=%u\n", pSigInfo->mod);
	}
	pDemodKernel->Unlock(pDemodKernel);

	return ret;
}

int REALTEK_ALL_GetSignalQuality_Ext(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality)
{
	int ret = TUNER_CTRL_FAIL;
	REALTEK_ALL* pRealtekAll;
	DEMOD_CALLER(" REALTEK_ALL_GetSignalQuality_Ext\n");
	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m" , pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	//DEMOD_INFO("\033[0;32;31m""TV mode = 0x%x id = %u i2c addr = 0x%x\n""\033[m" , mode, id, pQuality->demod_i2cAddr);

	pDemodKernel->Lock(pDemodKernel);
	switch(mode)
	{
		case TV_SYS_TYPE_ISDBT_CXD2856:
		case TV_SYS_TYPE_ISDBT_CXD2856_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2856;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBT_EXT;
			break;
		case TV_SYS_TYPE_ISDBT_CXD2878:
		case TV_SYS_TYPE_ISDBT_CXD2878_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2878;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBT_EXT;
			break;
		case TV_SYS_TYPE_ISDBS_CXD2856:
		case TV_SYS_TYPE_ISDBS_CXD2856_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2856;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS_EXT;
			break;
		case TV_SYS_TYPE_ISDBS_CXD2878:
		case TV_SYS_TYPE_ISDBS_CXD2878_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2878;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS_EXT;
			break;
		case TV_SYS_TYPE_ISDBS3_CXD2878:
		case TV_SYS_TYPE_ISDBS3_CXD2878_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS3_CXD2878;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS3_EXT;
			break;
		default:
			goto get_signalquality_failed;
	}

	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->GetSignalQuality(pRealtekAll->m_pDemod, id, pQuality);
	pDemodKernel->Unlock(pDemodKernel);
	return ret;

get_signalquality_failed:
	return TUNER_CTRL_FAIL;
}

int REALTEK_ALL_SetTSMode_Ext(DEMOD* pDemodKernel, TS_PARAM* pParam)
{
	REALTEK_ALL* pRealtekAll;
	int ret = TUNER_CTRL_FAIL;
	DEMOD_CALLER(" REALTEK_ALL_SetTSMode_Ext\n");

	pRealtekAll = (REALTEK_ALL*)(pDemodKernel->private_data);

	if (pRealtekAll->m_mode == RTK_DEMOD_MODE_NONE) {
		DEMOD_INFO("\033[0;32;31m""ExtDEMOD need init first !!!!! demod mode=%d\n""\033[m" , pRealtekAll->m_mode);
		return TUNER_CTRL_FAIL;
	}

	pDemodKernel->Lock(pDemodKernel);
	DEMOD_INFO("\033[0;32;31m""TV mode = 0x%x i2c addr = 0x%x\n""\033[m" , pParam->mode, pParam->portIdx);

	switch(pParam->mode)
	{
		case TV_SYS_TYPE_ISDBT_CXD2856:
		case TV_SYS_TYPE_ISDBT_CXD2856_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2856;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBT_EXT;
			break;
		case TV_SYS_TYPE_ISDBT_CXD2878:
		case TV_SYS_TYPE_ISDBT_CXD2878_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBT_CXD2878;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBT_EXT;
			break;
		case TV_SYS_TYPE_ISDBS_CXD2856:
		case TV_SYS_TYPE_ISDBS_CXD2856_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2856;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS_EXT;
			break;
		case TV_SYS_TYPE_ISDBS_CXD2878:
		case TV_SYS_TYPE_ISDBS_CXD2878_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS_CXD2878;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS_EXT;
			break;
		case TV_SYS_TYPE_ISDBS3_CXD2878:
		case TV_SYS_TYPE_ISDBS3_CXD2878_2nd:
			pRealtekAll->m_pDemod = pRealtekAll->m_pSONYISDBS3_CXD2878;
			pRealtekAll->m_mode               = RTK_DEMOD_MODE_ISDBS3_EXT;
			break;
		default:
			goto set_ts_mode_failed;
	}

	if (pRealtekAll->m_pDemod)
		ret = pRealtekAll->m_pDemod->SetTSMode(pRealtekAll->m_pDemod, pParam);

	pDemodKernel->Unlock(pDemodKernel);
	return ret;

set_ts_mode_failed:
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;
}

