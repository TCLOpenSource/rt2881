/***************************************************************************************************
  File        : demod_rtk_s_isdbs_cxd2856.cpp
  Description : CXD2856_S_ISDBS demod
  Author      : cs_xiao
****************************************************************************************************
    Update List :
----------------------------------------------------------------------------------------------------
    20201202    | create phase
***************************************************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <kadp_OSAL.h>
#include "demodcomm/foundation.h"
#include "demod_ext_common.h"
#include "demod_rtk_s_isdbs_cxd2856.h"
#include "demod_rtk_s_isdbs_cxd2856_priv.h"
#include <rtk_kdriver/pcbMgr.h>

#include "CXD2856Family_refcode/sony_demod.h"
#include "CXD2856Family_refcode/isdb_sat/sony_isdbs.h"
#include "CXD2856Family_refcode/isdb_sat/sony_demod_isdbs.h"
#include "CXD2856Family_refcode/isdb_sat/sony_demod_isdbs_monitor.h"
#include "CXD2856Family_refcode/isdb_sat/sony_integ_isdbs.h"

static int isCreated = 0;
static sony_demod_t* pDemod = NULL;
//sony_demod_create_param_t gCreateParam;

static BASE_INTERFACE_MODULE*	pBaseIfIsdbs;
static const char *Isdbs_Result[13] = {"OK", "Argument Error", "I2C Error", "SW State Error", "HW State Error", "Timeout", "Unlock", "Out of Range", "No Support", "Cancelled", "Other Error", "Overflow", "OK - Confirm"};
extern unsigned char I2c_SpeedByPCB;
/*----------------------------------------------------------------------
 * Func : CXD2856_S_ISDBS
 *
 * Desc : constructor
 *
 * Parm : device_addr : device address
 *        output_mode : REALTEK_S_ISDBS_OUT_IF_SERIAL / REALTEK_S_ISDBS_OUT_IF_PARALLEL
 *        output_freq : REALTEK_S_ISDBS_OUT_FREQ_6M / REALTEK_S_ISDBS_OUT_FREQ_10M
 *        pComm       : hadle of COMM
 *
 * Retn : handle of demod
 *----------------------------------------------------------------------*/
void CXD2856_S_ISDBS_Constructors(DEMOD* pDemodKernel, unsigned char addr, unsigned char output_mode, unsigned char output_freq, COMM* pComm)
{
	CXD2856_S_ISDBS* pRealtekSISDBS;

	pDemodKernel->private_data = kmalloc(sizeof(CXD2856_S_ISDBS), GFP_KERNEL);
	if(pDemodKernel->private_data == NULL) {
		goto kmalloc_fail;
	}
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	ExtDemod_Constructors(pDemodKernel);

	pDemodKernel->m_id                 = DTV_DEMOD_SONY_ISDBS_2878;
	pDemodKernel->m_addr               = addr;
	pDemodKernel->m_pTuner             = NULL;
	pDemodKernel->m_pComm              = pComm;
	pDemodKernel->m_ScanRange          = SCAN_RANGE_3_6;
	pDemodKernel->m_ScanMode           = CH_UNKNOW;
	pDemodKernel->m_Capability         = TV_SYS_TYPE_ISDBS_CXD2878;
	pDemodKernel->m_update_interval_ms = 300;
	pDemodKernel->m_if.freq            = 36125000;
	pDemodKernel->m_if.inversion       = 1;
	pDemodKernel->m_if.agc_pola        = 0;
	pDemodKernel->m_clock              = CRYSTAL_FREQ_27000000HZ;
	pRealtekSISDBS->m_output_freq        = output_freq;
	pRealtekSISDBS->m_private            = (void*) AllocRealtekSIsdbs2856Driver(
				pDemodKernel->m_pComm,
				pDemodKernel->m_addr,
				pDemodKernel->m_clock);
	pDemodKernel->m_ts.mode            = (output_mode == RTK_DEMOD_OUT_IF_PARALLEL) ? PARALLEL_MODE : SERIAL_MODE;
	pDemodKernel->m_ts.data_order      = MSB_D7;
	pDemodKernel->m_ts.datapin         = MSB_FIRST;
	pDemodKernel->m_ts.err_pol         = ACTIVE_HIGH;
	pDemodKernel->m_ts.sync_pol        = ACTIVE_HIGH;
	pDemodKernel->m_ts.val_pol         = ACTIVE_HIGH;
	pDemodKernel->m_ts.clk_pol         = FALLING_EDGE;
	pDemodKernel->m_ts.internal_demod_input = 1;
	pDemodKernel->m_ts.tsOutEn 		   = 1;
	pRealtekSISDBS->m_low_snr_detect     = 0;
	pRealtekSISDBS->m_low_snr_recover    = 0;
	pRealtekSISDBS->m_auto_tune_enable   = 0;
	pDemodKernel->m_pComm->SetTargetName(pDemodKernel->m_pComm, pDemodKernel->m_addr, "CXD2856_S_ISDBS");
	pRealtekSISDBS->m_pTunerOptReg       = NULL;
	pRealtekSISDBS->m_TunerOptRegCnt     = 0;
	pRealtekSISDBS->m_acquire_sig_en = 0;
	pDemodKernel->Init_Ext = CXD2856_S_ISDBS_Init;
	pDemodKernel->Reset = ExtDemod_Reset;
	pDemodKernel->AcquireSignal = CXD2856_S_ISDBS_AcquireSignal;
	pDemodKernel->ScanSignal = CXD2856_S_ISDBS_ScanSignal;
	pDemodKernel->SetIF = CXD2856_S_ISDBS_SetIF;
	pDemodKernel->SetTSMode = CXD2856_S_ISDBS_SetTSMode;
	pDemodKernel->SetTVSysEx = CXD2856_S_ISDBS_SetTVSysEx;
	pDemodKernel->SetMode = CXD2856_S_ISDBS_SetMode;
	pDemodKernel->SetTVSys = CXD2856_S_ISDBS_SetTVSys;
	pDemodKernel->GetLockStatus = CXD2856_S_ISDBS_GetLockStatus;
	pDemodKernel->GetSignalInfo = CXD2856_S_ISDBS_GetSignalInfo;
	pDemodKernel->GetSignalQuality = CXD2856_S_ISDBS_GetSignalQuality;
	pDemodKernel->GetDemodStatus = ExtDemod_GetDemodStatus;
	pDemodKernel->GetCarrierOffset = CXD2856_S_ISDBS_GetCarrierOffset;
	pDemodKernel->ForwardI2C = CXD2856_S_ISDBS_ForwardI2C;
	pDemodKernel->AutoTune = CXD2856_S_ISDBS_AutoTune;
	pDemodKernel->KernelAutoTune = CXD2856_S_ISDBS_KernelAutoTune;
	pDemodKernel->GetCapability = ExtDemod_GetCapability;
	pDemodKernel->Suspend = ExtDemod_Suspend;
	pDemodKernel->Resume = ExtDemod_Resume;
	pDemodKernel->SetTvMode = CXD2856_S_ISDBS_SetTvMode;
	pDemodKernel->SetExtDemodInfo = CXD2856_S_ISDBS_SetExtDemodInfo;
	//pDemodKernel->DebugInfo = CXD2856_S_ISDBS_DebugInfo;	  	  // Add monitor function that used to monitor demod status
	pDemodKernel->InspectSignal = CXD2856_S_ISDBS_InspectSignal; // Add monitor function that used to monitor demod status
	pDemodKernel->AttachTuner = ExtDemod_AttachTuner;
	pDemodKernel->GetTSParam = ExtDemod_GetTSParam;
	pDemodKernel->Lock = CXD2856_S_ISDBS_Lock;
	pDemodKernel->Unlock = CXD2856_S_ISDBS_Unlock;
	pDemodKernel->Destory = CXD2856_S_ISDBS_Destructors;

kmalloc_fail:
	CXD2856_S_ISDBS_INFO("CXD2856_S_ISDBS_Constructors kmalloc Failed\n");
}


/*----------------------------------------------------------------------
 * Func : ~CXD2856_S_ISDBSND
 *
 * Desc : Destructor of CXD2856_S_ISDBSND
 *
 * Parm : pDemod : handle of Demod
 *
 * Retn : N/A
 *----------------------------------------------------------------------*/
void CXD2856_S_ISDBS_Destructors(DEMOD* pDemodKernel)
{
	CXD2856_S_ISDBS* pRealtekSISDBS;
	sony_result_t result = SONY_RESULT_OK;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	result = sony_demod_Sleep(pDemod);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("CXD2856_demod_Sleep fail (%d)\n", result);
	}

	pDemod = NULL;
	isCreated = 0;
	//if (pRealtekSISDBS->m_private != NULL)
	ReleaseRealtekSIsdbs2856Driver((CXD2856_S_ISDBS_DRIVER_DATA*)pRealtekSISDBS->m_private);

	kfree(pRealtekSISDBS);
	//pDemodKernel->Destory(pDemodKernel);
	CXD2856_S_ISDBS_INFO("CXD2856_S_ISDBS_Destructors Complete\n");
}


/*----------------------------------------------------------------------
 * Func : Init
 *
 * Desc : Initial realtek_r
 *
 * Parm : N/A
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int CXD2856_S_ISDBS_Init(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode)
{
	CXD2856_S_ISDBS* pRealtekSISDBS;
	sony_demod_create_param_t createParam;
	static sony_i2c_t DemodI2c;
	sony_result_t result = SONY_RESULT_OK;

	//unsigned char dem_slv[2] = {0xda, 0xca};
	unsigned char dem_slv[2] = {0xda, 0xc8};
	unsigned long long param = 0;

	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	createParam.xtalFreq       = SONY_DEMOD_XTAL_24000KHz;/* 24MHz Xtal */
	createParam.tunerI2cConfig = SONY_DEMOD_TUNER_I2C_CONFIG_DISABLE;/* I2C repeater is used */

	pDemodKernel->Lock(pDemodKernel);
	pRealtekSISDBS->m_auto_tune_enable = 0;


	if (!isCreated) {
		DemodI2c.Read             = CXD2856_i2c_IsdbsRead;
		DemodI2c.Write            = CXD2856_i2c_IsdbsWrite;
		DemodI2c.ReadRegister     = sony_i2c_CommonReadRegister;
		DemodI2c.WriteRegister    = sony_i2c_CommonWriteRegister;
		DemodI2c.WriteOneRegister = sony_i2c_CommonWriteOneRegister;

		if(mode == TV_SYS_TYPE_ISDBS_CXD2856)
			createParam.i2cAddressSLVT = dem_slv[0]; //demod1 i2c is 0xda
		else if(mode == TV_SYS_TYPE_ISDBS_CXD2856_2nd)
			createParam.i2cAddressSLVT = dem_slv[1];
		else
			createParam.i2cAddressSLVT = 0; // do not select the correct mode
		
		result = sony_demod_Create(pDemod, &createParam, &DemodI2c);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBS_INFO("sony_demod_Create 1st error (%d)\n", result);
			goto init_fail;
		}
		result = sony_demod_Initialize(pDemod);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBS_INFO("sony_demod_Initialize error (%d)\n", result);
			goto init_fail;
		}
/*
		createParam.i2cAddressSLVT = dem_slv[1];
		result = sony_demod_Create(pDemod, &createParam, &DemodI2c);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBS_INFO("sony_demod_Create 2st error (%d)\n", result);
			return TUNER_CTRL_FAIL;
		}
		result = sony_demod_Initialize(pDemod);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBS_INFO("sony_demod_Initialize 2nd error (%d)\n", result);
			goto init_fail;
		}		
*/		
		isCreated = 1;
		if (pcb_mgr_get_enum_info_byname("EXTDEMOD_I2C_SPEED", &param) != 0) {
			CXD2856_S_ISDBS_WARNING("CXD2856_ISDBS[%d]: Get EXTDEMOD_I2C_SPEED fail use default !!!!!!!!!!!!!!!\n", __LINE__);
			param = 0x0;
		}
		I2c_SpeedByPCB = (unsigned char)param;
		CXD2856_S_ISDBS_INFO("CXD2856_ISDBS[%d]: set EXTDEMOD_I2C speed = %d\n", __LINE__, param);
	}
/*
	result = sony_demod_I2cRepeaterEnable(pDemod, 1);
	if (result != SONY_RESULT_OK) {
		//printf("sony_demod_I2cRepeaterEnable error (%d)\n", result);
		goto init_fail;
	}

	result = sony_demod_Shutdown(pDemod);
	if (result != SONY_RESULT_OK) {
		//printf("sony_demod_Shutdown error (%d)\n", result);
		goto init_fail;
	}
*/
	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBS_INFO("CXD2856_S_ISDBS_Init Complete\n");
	return TUNER_CTRL_OK;

init_fail:
	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBS_WARNING("CXD2856_S_ISDBS_Init Fail\n");
	return TUNER_CTRL_FAIL;
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
int CXD2856_S_ISDBS_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys)
{
	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);

	CXD2856_S_ISDBS_INFO("\033[1;32;31m" "%s %s %d, system = %d\n" "\033[m", __FILE__, __func__, __LINE__, sys);
	return pDemodKernel->SetTVSysEx(pDemodKernel, sys, NULL);
}


/*----------------------------------------------------------------------
 * Func : SetTVSysEx
 *
 * Desc : Set TV System
 *
 * Parm : sys : TV System
 *        pSigInfo : signal information
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int CXD2856_S_ISDBS_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO*  pSigInfo)
{
	TUNER_PARAM Param;
	CXD2856_S_ISDBS* pRealtekSISDBS;
	unsigned char i2cAddr = 0;
	const char *tsidtype_str[] = { "TS ID", "Relative TS Number" };


	sony_result_t result = SONY_RESULT_OK;
	sony_isdbs_tune_param_t IsdbsParam;

	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if (!IsExtIsdbSys(sys)) {
		CXD2856_S_ISDBS_WARNING("Set TV system failed, unsupported TV system\n");
		return  TUNER_CTRL_FAIL;
	}

	CXD2856_S_ISDBS_INFO("pSigInfo->isdbt.portIdx=%u pSigInfo->isdbs_ext.portIdx=%u  pSigInfo->isdbs3_ext.portIdx=%u\n", pSigInfo->isdbt.portIdx, pSigInfo->isdbs_ext.portIdx, pSigInfo->isdbs3_ext.portIdx);
	switch (pSigInfo->isdbs_ext.portIdx) {
	case I2C_PORT1:
		i2cAddr = 0xda;
		CXD2856_S_ISDBS_INFO("Use demod1 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT2:
		i2cAddr = 0xc8;
		CXD2856_S_ISDBS_INFO("Use demod2 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT3:
		i2cAddr = 0xca;
		CXD2856_S_ISDBS_INFO("Use demod3 i2cAddr = 0x%x\n", i2cAddr);
		break;
	default:
		CXD2856_S_ISDBS_INFO("unknown Demod port index !!!\n");
		goto SetDemod_fail;
	}
	
	pDemod->i2cAddressSLVT = i2cAddr;
    pDemod->i2cAddressSLVX = i2cAddr + 4;
    //pDemod->i2cAddressSLVR = i2cAddr - 0x40;
    //pDemod->i2cAddressSLVM = i2cAddr - 0xA8;
	
	pRealtekSISDBS->m_auto_tune_enable = 0;

	/* Configure the ISDBS tune parameters based on the channel requirements */
	IsdbsParam.centerFreqKHz = pSigInfo->isdbs_ext.freq_khz;              /* Channel center frequency in KHz */
	IsdbsParam.tsid = pSigInfo->isdbs_ext.ts_id;                        /* TS ID */
	IsdbsParam.tsidType = SONY_ISDBS_TSID_TYPE_TSID; /* TS ID type (absolute TSID or relative TS number) */
	
	CXD2856_S_ISDBS_INFO("Tune to ISDB-S signal with the following parameters:\n");
	CXD2856_S_ISDBS_INFO(" - Center Freq    : %u kHz\n", IsdbsParam.centerFreqKHz);
	CXD2856_S_ISDBS_INFO(" - TS ID          : 0x%04X\n", IsdbsParam.tsid);
	CXD2856_S_ISDBS_INFO(" - TS ID Type     : %s\n", tsidtype_str[pSigInfo->isdbs_ext.ts_type]);

	
	CXD2856_S_ISDBS_INFO("\033[1;32;31m" "%s %s %d, system = %d\n" "\033[m", __FILE__, __func__, __LINE__, sys);

	//TUNER_PARAM Param;
	if (pDemodKernel->m_pTuner->GetParam(pDemodKernel->m_pTuner, sys, &Param) == TUNER_CTRL_OK) {
		if ((pDemodKernel->m_if.freq != Param.if_freq) || (pDemodKernel->m_if.inversion != Param.if_inversion) || (pDemodKernel->m_if.agc_pola  != Param.if_agc_pola)) {
			pDemodKernel->m_if.freq = Param.if_freq;
			pDemodKernel->m_if.inversion = Param.if_inversion;
			pDemodKernel->m_if.agc_pola = Param.if_agc_pola;
			if (pDemodKernel->SetIF(pDemodKernel, &pDemodKernel->m_if) != TUNER_CTRL_OK)  return  TUNER_CTRL_FAIL;

		}
	}
	CXD2856_S_ISDBS_INFO("m_if.freq = "PT_S32BITS", m_if.inversion = %d, m_if.agc_pola = %d\n", pDemodKernel->m_if.freq, pDemodKernel->m_if.inversion, pDemodKernel->m_if.agc_pola);
	
	pDemodKernel->Lock(pDemodKernel);

	result = sony_demod_Initialize(pDemod);

	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("sony_demod_Initialize error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_SAT_IFAGCNEG, 1);
    if (result != SONY_RESULT_OK) {
        CXD2856_S_ISDBS_INFO("sony_demod_SetConfig (SONY_DEMOD_CONFIG_SAT_IFAGCNEG) failed. (%s)\n", result);
        return result;
    }

	/*
	* In default, The setting is optimized for Sony silicon tuners.
	* If non-Sony tuner is used, the user should call following to
	* disable Sony silicon tuner optimized setting.
	*/
	//result = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_TUNER_OPTIMIZE, SONY_DEMOD_TUNER_OPTIMIZE_NONSONY);
	//if (result != SONY_RESULT_OK) {
	//	CXD2856_S_ISDBS_INFO ("sony_demod_SetConfig (SONY_DEMOD_CONFIG_TUNER_OPTIMIZE) failed. (%s)\n", Isdbs_Result[result]);
	//	return result;
	//}

	result = sony_demod_SetTSClockModeAndFreq(pDemod, SONY_DTV_SYSTEM_ISDBS);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("sony_demod_SetTSClockModeAndFreq error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_PARALLEL_SEL, 0);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("Set SONY_DEMOD_CONFIG_PARALLEL_SEL error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("Set SONY_DEMOD_CONFIG_SER_DATA_ON_MSB error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_OUTPUT_SEL_MSB, 1);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("Set SONY_DEMOD_CONFIG_OUTPUT_SEL_MSB error (%d)\n", result);
		goto SetDemod_fail;
	}

	//detault set rising edge
	result = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_LATCH_ON_POSEDGE, 1);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("Set SONY_DEMOD_CONFIG_LATCH_ON_POSEDGE error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_TSCLK_CONT, 0);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("Set SONY_DEMOD_CONFIG_TSCLK_CONT error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_isdbs_Tune(pDemod, &IsdbsParam);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("sony_demod_isdbs_Tune error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_TuneEnd(pDemod);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("sony_demod_Shutdown error (%d)\n", result);
		goto SetDemod_fail;
	}

	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBS_WARNING("CXD2856_S_ISDBS_SetTVSysEx done\n");
	pRealtekSISDBS->m_acquire_sig_en = 1;
	pRealtekSISDBS->m_status_checking_stime = tv_osal_time_ext() + ISDBS_STATUS_CHECK_INTERVAL_MS;
	pRealtekSISDBS->m_auto_tune_enable = 1;

	return TUNER_CTRL_OK;

SetDemod_fail:
	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBS_WARNING("CXD2856_S_ISDBS_SetTVSysEx Fail\n");
	//pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;



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
int CXD2856_S_ISDBS_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam)
{
	int ret = TUNER_CTRL_OK;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);

	CXD2856_S_ISDBS_INFO("SetIF, IF_freq="PT_U32BITS" spectrumn inv=%d, agc_pola=%d\n", pParam->freq, pParam->inversion, pParam->agc_pola);
	
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
int CXD2856_S_ISDBS_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode)
{
	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	switch (mode) {
	case TV_OP_MODE_NORMAL:
	case TV_OP_MODE_SCANING:
		break;
	case TV_OP_MODE_STANDBY:
		break;
	case TV_OP_MODE_ATV_NORMAL:
	case TV_OP_MODE_TP_ONLY:
		break;
	default:
		CXD2856_S_ISDBS_WARNING("unknown TV_OP_MODE\n");
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
int CXD2856_S_ISDBS_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam)
{
	int ret = TUNER_CTRL_OK;
	unsigned int on_off = -1;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if(pParam->portIdx != 0x0)
	{
		pDemod->i2cAddressSLVT = pParam->portIdx;
		pDemod->i2cAddressSLVX = pParam->portIdx + 4;
	}

	pDemodKernel->Lock(pDemodKernel);
	if(pParam->tsOutEn == 1) {
		ret |= sony_demod_Switch_TS_Output(pDemod, 1);
		if (ret != SONY_RESULT_OK)
			CXD2856_S_ISDBS_INFO("sony_demod_SetStreamOutput error (%d)\n", ret);
	}
	else {
		ret |= sony_demod_Switch_TS_Output(pDemod, 0);
		if (ret != SONY_RESULT_OK)
			CXD2856_S_ISDBS_INFO("sony_demod_SetStreamOutput error (%d)\n", ret);
	}

	ret |= sony_demod_Get_TS_Output(pDemod, &on_off);
	CXD2856_S_ISDBS_INFO("Get TSTLV Output = %d\n", on_off);
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
int CXD2856_S_ISDBS_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock)
{

	sony_result_t result = SONY_RESULT_OK;
	unsigned char AgcLock = 0, TsLock = 0, TmccLock = 0;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);

	result = sony_demod_isdbs_monitor_SyncStat(pDemod, &AgcLock, &TsLock, &TmccLock);
	CXD2856_S_ISDBS_INFO("\033[1;32;36m" "AGC Lock = %u, TS Lock = %u TMCC Lock = %u\n" "\033[m", AgcLock, TsLock, TmccLock);
	pDemodKernel->Unlock(pDemodKernel);

	*pLock = TsLock ;

	return (result == SONY_RESULT_OK) ? TUNER_CTRL_OK : TUNER_CTRL_FAIL;
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
int CXD2856_S_ISDBS_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo)
{
	sony_result_t result = SONY_RESULT_OK;
	CXD2856_S_ISDBS*	pRealtekSISDBS;
	sony_isdbs_modcod_t modCodH, modCodL;//isdbs modcod
	unsigned char slotNumH = 0, slotNumL = 0, Cnt = 0, siteDiversityInfo = 0;
	sony_isdbs_tmcc_info_t tmccInfo;
	const char *modcod_str[] = {
                "Reserved 0", "BPSK(1/2)", "QPSK(1/2)", "QPSK(2/3)",
                "QPSK(3/4)", "QPSK(5/6)", "QPSK(7/8)", "TC8PSK(2/3)",
                "Reserved 8", "Reserved 9", "Reserved 10", "Reserved 11",
                "Reserved 12", "Reserved 13", "Reserved 14", "Unused"
	};
	
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	//need implement
	pDemodKernel->Lock(pDemodKernel);

	result = sony_demod_isdbs_monitor_ModCod(pDemod, &modCodH, &modCodL);

	if (result == SONY_RESULT_OK) {
		CXD2856_S_ISDBS_INFO("ModCod (High) | %s\n", modcod_str[modCodH]);
		CXD2856_S_ISDBS_INFO("ModCod (Low) | %s\n", modcod_str[modCodL]);
		pInfo->isdbs_ext.modcodH = (isdbs_modcod_t)modCodH;
		pInfo->isdbs_ext.modcodL = (isdbs_modcod_t)modCodL;
	}else {
		CXD2856_S_ISDBS_INFO ("ModCod  | Error  | %s\n", Isdbs_Result[result]);
	}
	
	//slot 
	result |= sony_demod_isdbs_monitor_SlotNum(pDemod, &slotNumH, &slotNumL);
	if (result == SONY_RESULT_OK) {
		//CXD2856_S_ISDBS_INFO("SlotNum (High) = %u\n", slotNumH);
		//CXD2856_S_ISDBS_INFO("SlotNum (Low) = %u\n", slotNumL);
		pInfo->isdbs_ext.slotNumH = slotNumH;
		pInfo->isdbs_ext.slotNumL = slotNumL;
	}else {
		CXD2856_S_ISDBS_INFO ("SlotNum  | Error  | %s\n", Isdbs_Result[result]);
	}

	//tmcc info
	result |= sony_demod_isdbs_monitor_TMCCInfo (pDemod, &tmccInfo);
	if (result == SONY_RESULT_OK) {
		//CXD2856_S_ISDBS_INFO("TMCCInfo | Change Order    | %d\n", tmccInfo.changeOrder);
		//CXD2856_S_ISDBS_INFO("         | EWS Flag        | %d\n", tmccInfo.ewsFlag);
		//CXD2856_S_ISDBS_INFO("         | Uplink Info     | %d\n", tmccInfo.uplinkInfo);
		pInfo->isdbs_ext.tmccInfo.changeOrder = tmccInfo.changeOrder;
		pInfo->isdbs_ext.tmccInfo.ewsFlag = tmccInfo.ewsFlag;
		pInfo->isdbs_ext.tmccInfo.uplinkInfo = tmccInfo.uplinkInfo;
		
		for (Cnt = 0; Cnt < 4; Cnt++) {
			//CXD2856_S_ISDBS_INFO("         | Mod Cod #%d      | %s\n", Cnt, modcod_str[tmccInfo.modcodSlotInfo[Cnt].modCod]);
			//CXD2856_S_ISDBS_INFO("         | Slot Num of #%d  | %d\n", Cnt, tmccInfo.modcodSlotInfo[Cnt].slotNum);
			pInfo->isdbs_ext.tmccInfo.modcodSlotInfo[Cnt].modCod = (isdbs_modcod_t) tmccInfo.modcodSlotInfo[Cnt].modCod;
			pInfo->isdbs_ext.tmccInfo.modcodSlotInfo[Cnt].slotNum = tmccInfo.modcodSlotInfo[Cnt].slotNum;
		}
        for (Cnt = 0; Cnt < 8; Cnt++) {
			//CXD2856_S_ISDBS_INFO("         | TS ID #%d        | 0x%04X\n", Cnt, tmccInfo.tsidForEachRelativeTS[Cnt]);
			pInfo->isdbs_ext.tmccInfo.tsidForEachRelativeTS[Cnt] = tmccInfo.tsidForEachRelativeTS[Cnt];
		}
	}else {
		CXD2856_S_ISDBS_INFO(" TMCC Information        | Error           |  %s\n", Isdbs_Result[result]);
	}
	result |= sony_demod_isdbs_monitor_SiteDiversityInfo (pDemod, &siteDiversityInfo);
	if (result == SONY_RESULT_OK) {
		//CXD2856_S_ISDBS_INFO(" SiteDiversityInfo | Site Diversity  | %u\n", siteDiversityInfo);
		pInfo->isdbs_ext.siteDiversityInfo = siteDiversityInfo;
	}
	else {
		CXD2856_S_ISDBS_INFO(" SiteDiversityInfo | Error           | %s\n", Isdbs_Result[result]);
	}

	pDemodKernel->Unlock(pDemodKernel);

	return result;
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
int CXD2856_S_ISDBS_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality)
{
	sony_result_t result = SONY_RESULT_OK;

	int CNR = 0, TxRSSIdBm = 0;
	unsigned int BerH = 0, BerL = 0, BerTMCC = 0;
	unsigned int PerH = 0, PerL = 0;
	unsigned int ifAGCOut = 0, offset = 0;
	unsigned char AgcLock = 0, TsLock = 0, TmccLock = 0, TSIDError = 0;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if(pQuality->demod_i2cAddr != 0x0)
	{
		pDemod->i2cAddressSLVT = pQuality->demod_i2cAddr;
		pDemod->i2cAddressSLVX = pQuality->demod_i2cAddr + 4;
	}

	pDemodKernel->Lock(pDemodKernel);

	switch (id) {
	case TV_QUAL_LOCKSTATUS:
		result = sony_demod_isdbs_monitor_SyncStat(pDemod, &AgcLock, &TsLock, &TmccLock);
		CXD2856_S_ISDBS_INFO("\033[1;32;36m" "AGC Lock = %u, TS Lock = %u TMCC Lock = %u [i2c=0x%x]\n" "\033[m", AgcLock, TsLock, TmccLock, pDemod->i2cAddressSLVT);
		pQuality->ExtIsdb_Status.lock[0] = AgcLock;
		pQuality->ExtIsdb_Status.lock[1] = TsLock;
		pQuality->ExtIsdb_Status.lock[2] = TmccLock;
		break;

	case TV_QUAL_SNR:
		result = sony_demod_isdbs_monitor_CNR(pDemod, &CNR);//SNR(dB) * 1000 
		CXD2856_S_ISDBS_INFO (" CNR | %d x 10^-3 dB\n", CNR);
		pQuality->snr = CNR * 10000;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	case TV_QUAL_BEC:
	case TV_QUAL_BER:
		//result = sony_demod_isdbs_monitor_PreRSBER(pDemod, &BerH, &BerL, &BerTMCC);
		
		pQuality->ber = BerH;
		pQuality->layer_ber.ber_num = BerL;
		pQuality->layer_ber.ber_den = 1000000000;
		break;

	case TV_QUAL_PEC:
	case TV_QUAL_PER:
	case TV_QUAL_ISDB_PER:
 		result = sony_demod_isdbs_monitor_PER(pDemod, &PerH, &PerL);
		CXD2856_S_ISDBS_INFO (" Pre-RS PER | PER (High)      | %u x 10^-6\n", PerH);
		CXD2856_S_ISDBS_INFO (" Pre-RS PER | PER (Low)      | %u x 10^-6\n", PerL);
		pQuality->ExtIsdb_Status.isdbsPER[0] = PerH;
		pQuality->ExtIsdb_Status.isdbsPER[1] = PerL;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	case TV_QUAL_SIGNAL_STRENGTH:
		//- Signal strength = 100 when RSSI = +10 dBm			
		//- Signal strength decreases 1 unit for every 1dB decreases for RSSI
		TxRSSIdBm = (int)pQuality->rflevel;
		if(TxRSSIdBm >= 10)
			pQuality->strength = 100;
		else {
			pQuality->strength = TxRSSIdBm + 90;
		}
		break;
	case TV_QUAL_SIGNAL_QUALITY:
		result = sony_demod_isdbs_monitor_CNR(pDemod, &CNR);//CNR(dB) * 1000
		pQuality->quality = CNR;
		break;
	case TV_QUAL_AGC:
		result = sony_demod_isdbs_monitor_IFAGCOut(pDemod, &ifAGCOut);
		pQuality->agc = ifAGCOut;
		CXD2856_S_ISDBS_INFO (" IF AGC | %u\n", ifAGCOut);
		break;
	case TV_QUAL_CARRIER_OFFSET:
		result = sony_demod_isdbs_monitor_CarrierOffset(pDemod, &offset);
		pQuality->ExtIsdb_Status.freq_offset = offset;
		pQuality->ExtIsdb_Status.demod_ret = result;
		CXD2856_S_ISDBS_INFO (" Carrier Offset | %d\n", offset);
		break;
	case TV_QUAL_PRERSBER:
		result = sony_demod_isdbs_monitor_PreRSBER(pDemod, &BerH, &BerL, &BerTMCC);
		CXD2856_S_ISDBS_INFO (" Pre-RS BER | (High) %u (Low) %u (TMCC) %u| x 10^-7  result=%u\n", BerH, BerL, BerTMCC, result);
		pQuality->ExtIsdb_Status.preRSBER[0] = BerH;
		pQuality->ExtIsdb_Status.preRSBER[1] = BerL;
		pQuality->ExtIsdb_Status.preRSBER[2] = BerTMCC;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;
	case TV_QUAL_TSIDERROR:
		result = sony_demod_isdbs_monitor_TSIDError(pDemod, &TSIDError);
		CXD2856_S_ISDBS_INFO (" TSIDError = %u\n", TSIDError);
		pQuality->ExtIsdb_Status.TSIDError = TSIDError;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	default:
		CXD2856_S_ISDBS_WARNING("unknown ENUM_TV_QUAL id = %u\n", (unsigned int) id);
		result = SONY_RESULT_OK;
		break;
	}

	pDemodKernel->Unlock(pDemodKernel);
	return (result == SONY_RESULT_OK) ? TUNER_CTRL_OK : TUNER_CTRL_FAIL;
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
int CXD2856_S_ISDBS_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset)
{
	sony_result_t result = SONY_RESULT_OK;
	int offset;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;


	pDemodKernel->Lock(pDemodKernel);

	result = sony_demod_isdbs_monitor_CarrierOffset(pDemod, &offset);

	if (result == SONY_RESULT_OK) {
		*pOffset = offset;
	} else
		*pOffset = 0;

	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBS_INFO("Carrier Offset = "PT_S32BITS" KHz\n", *pOffset);

	return (result == SONY_RESULT_OK) ? TUNER_CTRL_OK : TUNER_CTRL_FAIL;
}

/*----------------------------------------------------------------------
 * Func : GetDebugLogBuf
 *
 * Desc : Get the debug log Buf.
 *
 * Parm :
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int CXD2856_S_ISDBS_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize)
{
	int ret = TUNER_CTRL_OK;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	return ret;
}
/*----------------------------------------------------------------------
 * Func : AcquireSignal
 *
 * Desc : Reset Demodulator and Acquire Signal.
 *        This function should be called when channel changed.
 *
 * Parm : WaitSignalLock : wait until signal lock
 *        0 : don't wait
 *          others: wait for signal lock
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int CXD2856_S_ISDBS_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock)
{
#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	unsigned long cur_time;
#endif

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;


#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	cur_time = tv_osal_time_ext();

	while ((tv_osal_time_ext() - cur_time) <= 15) {
		if (pRealtekSISDBS->m_acquire_sig_en == 0)
			return TUNER_CTRL_OK;

		tv_osal_msleep_ext(5);
	}

	return TUNER_CTRL_FAIL;

#else
	//CXD2856_S_ISDBS_AcquireSignalThread(pDemodKernel);
	//pRealtekSISDBS->m_autotune_stime = tv_osal_time_ext() + AUTODETECT_SIG_UPDATE_INTERVAL_MS;
	return TUNER_CTRL_OK;
#endif
}

/*----------------------------------------------------------------------
 * Func : AcquireSignalThread
 *
 * Desc : Reset Demodulator and Acquire Signal.
 *        This function should be called when channel changed.
 *
 * Parm :
 *
 * Retn : TUNER_CTRL_OK / TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int CXD2856_S_ISDBS_AcquireSignalThread(DEMOD* pDemodKernel)
{
	unsigned long   stime;
	unsigned char AgcLock = 0, TsLock = 0, TmccLock = 0;
	sony_result_t result = SONY_RESULT_OK;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if (pRealtekSISDBS->m_acquire_sig_en == 0)
		return TUNER_CTRL_OK;


	CXD2856_S_ISDBS_INFO("\033[1;32;32m" "%s %s %d\n" "\033[m", __FILE__, __func__, __LINE__);


	pDemodKernel->Lock(pDemodKernel);
	pRealtekSISDBS->m_auto_tune_enable = 0;
	stime = tv_osal_time_ext();

	while ((tv_osal_time_ext() - stime) <= 3500) {
		result = sony_demod_isdbs_monitor_SyncStat(pDemod, &AgcLock, &TsLock, &TmccLock);
		if (TsLock) break;
		tv_osal_msleep_ext(5);
	}

	if((TsLock |TmccLock))
	{
		CXD2856_S_ISDBS_INFO("ISDBS LOCK AgcLock= %u, TsLock = %u, TmccLock = %u Time = %lu\n", AgcLock, TsLock, TmccLock, tv_osal_time_ext() - stime);	
	}
	else
	{
		CXD2856_S_ISDBS_INFO("ISDBS Unlock AgcLock= %u, TsLock = %u, TmccLock = %u Time = %lu\n", AgcLock, TsLock, TmccLock, tv_osal_time_ext() - stime);
	}
	

//acquire_signal_result:
	pDemodKernel->Unlock(pDemodKernel);
	pRealtekSISDBS->m_acquire_sig_en = 0;
	pRealtekSISDBS->m_status_checking_stime = tv_osal_time_ext() + ISDBS_STATUS_CHECK_INTERVAL_MS;
	pRealtekSISDBS->m_auto_tune_enable = 1;
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
int CXD2856_S_ISDBS_ScanSignal(DEMOD* pDemodKernel, S32BITS* pOffset, SCAN_RANGE range)
{
	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	return TUNER_CTRL_OK;
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
int CXD2856_S_ISDBS_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off)
{
	int ret = TUNER_CTRL_OK;

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

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
int CXD2856_S_ISDBS_AutoTune(DEMOD* pDemodKernel)
{
	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);

	pDemodKernel->Lock(pDemodKernel);
	pDemodKernel->Unlock(pDemodKernel);

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
int CXD2856_S_ISDBS_KernelAutoTune(DEMOD* pDemodKernel)
{
	int result = SONY_RESULT_OK;
#if 0
	unsigned long cur_time;
	CXD2856_S_ISDBS* pRealtekSISDBS;

#ifndef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	unsigned char ewsChange;
	unsigned char LockStatus = 0;
	TV_SIG_QUAL Quality;
#endif
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if(pRealtekSISDBS->m_auto_tune_enable == 0)
		return result;

	cur_time = tv_osal_time_ext();

#ifndef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	if ((cur_time > pRealtekSISDBS->m_status_checking_stime) || ((pRealtekSISDBS->m_status_checking_stime - cur_time) > ISDBS_STATUS_CHECK_INTERVAL_MS)) {
		result = sony_demod_isdbs_monitor_EWSChange (pDemod, &ewsChange);
		if (result == SONY_RESULT_OK) {
			CXD2856_S_ISDBS_INFO (" EWSChange               | EWS Change      | %d\n", ewsChange);
			if (ewsChange == 1) {
				sony_demod_isdbs_ClearEWSChange(pDemod);
			}
		}
		CXD2856_S_ISDBS_GetLockStatus(pDemodKernel, &LockStatus);
		if (LockStatus == 1) {
			CXD2856_S_ISDBS_GetSignalQuality(pDemodKernel, TV_QUAL_SNR, &Quality);
			CXD2856_S_ISDBS_GetSignalQuality(pDemodKernel, TV_QUAL_BER, &Quality);
			CXD2856_S_ISDBS_GetSignalQuality(pDemodKernel, TV_QUAL_PER, &Quality);
			CXD2856_S_ISDBS_GetSignalQuality(pDemodKernel, TV_QUAL_SIGNAL_STRENGTH, &Quality);
			CXD2856_S_ISDBS_GetSignalQuality(pDemodKernel, TV_QUAL_SIGNAL_QUALITY, &Quality);
			CXD2856_S_ISDBS_GetSignalQuality(pDemodKernel, TV_QUAL_AGC, &Quality);
		}
	}
#endif

	if (pRealtekSISDBS->m_acquire_sig_en) {
#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
		CXD2856_S_ISDBS_AcquireSignalThread(pDemodKernel);
		//pRealtekADVBTX->m_autotune_stime = tv_osal_time_ext() + AUTODETECT_SIG_UPDATE_INTERVAL_MS;
#endif
	}
#endif
	return result;
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
int CXD2856_S_ISDBS_InspectSignal(DEMOD* pDemodKernel)
{

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	return TUNER_CTRL_OK;
}


/*----------------------------------------------------------------------
 * Func : Activate REALTEK_R ISDBS Demod
 *
 * Desc : Run Auto Tuning for CCI
 *
 * Parm : N/A
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int CXD2856_S_ISDBS_Activate(DEMOD* pDemodKernel, unsigned char force_rst)
{

	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	return TUNER_CTRL_OK;

}


int CXD2856_S_ISDBS_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode)
{
	int ret = TUNER_CTRL_FAIL;
	CXD2856_S_ISDBS* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemodKernel->Lock(pDemodKernel);

	CXD2856_S_ISDBS_INFO("CXD2856_S_ISDBS_SetTvMode(%d)\n", mode);

	switch (mode) {
	case TV_SYS_TYPE_ISDBS_CXD2856:
	case TV_SYS_TYPE_ISDBS_CXD2856_2nd:
		CXD2856_S_ISDBS_INFO("Set to ISDBS \n");
		ret = pDemodKernel->Init_Ext(pDemodKernel, mode);
		//ret |= pDemodKernel->SetTSMode(pDemodKernel, &pDemodKernel->m_ts);

		if (ret != TUNER_CTRL_OK) {
			CXD2856_S_ISDBS_WARNING("Set CXD2856_S_ISDBS to ISDBS mode failed\n");
			goto set_demod_mode_failed;
		}
		break;

	default:
		CXD2856_S_ISDBS_WARNING("unknown TV_SYSTEM_TYPE\n");
		goto set_demod_mode_failed;
	}

	ret = TUNER_CTRL_OK;
	CXD2856_S_ISDBS_INFO("Set CXD2856_S_ISDBS to appointed DTV mode successed\n");
	pDemodKernel->Unlock(pDemodKernel);
	return ret;

set_demod_mode_failed:
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;
}
int CXD2856_S_ISDBS_SetExtDemodInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pSigInfo)
{
	unsigned char i2cAddr = 0;
	CXD2856_S_ISDBS* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2856_S_ISDBS*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBS_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);
	switch (pSigInfo->isdbs_ext.portIdx) {
	case I2C_PORT1:
		i2cAddr = 0xda;
		CXD2856_S_ISDBS_INFO("Use demod1 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT2:
		i2cAddr = 0xc8;
		CXD2856_S_ISDBS_INFO("Use demod2 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT3:
		i2cAddr = 0xca;
		CXD2856_S_ISDBS_INFO("Use demod3 i2cAddr = 0x%x\n", i2cAddr);
		break;
	default:
		CXD2856_S_ISDBS_WARNING("unknown Demod port index !!!\n");
		goto update_demod_info_failed;
	}
	
	pDemod->i2cAddressSLVT = i2cAddr;
    pDemod->i2cAddressSLVX = i2cAddr + 4;
    //pDemod->i2cAddressSLVR = i2cAddr - 0x40;
    //pDemod->i2cAddressSLVM = i2cAddr - 0xA8;
	
	CXD2856_S_ISDBS_INFO("Update ISDBS CXD2878 successed\n");
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_OK;

update_demod_info_failed:
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;
}

/*-----------------------------------------------------------------
 *     BASE_INTERFACE for CXD2856_S_ISDBSND API
 *----------------------------------------------------------------*/
void CXD2856_S_ISDBS_Lock(DEMOD* pDemodKernel)
{
	//FUNCTION_NAME_PRINT("LOCK 0x%x\n",&pDemodKernel->m_lock);
	//mutex_lock(&pDemodKernel->m_lock);
}


void CXD2856_S_ISDBS_Unlock(DEMOD* pDemodKernel)
{
	//FUNCTION_NAME_PRINT("UN_LOCK 0x%x\n",&pDemodKernel->m_lock);
	//mutex_unlock(&pDemodKernel->m_lock);
}


void ReleaseRealtekSIsdbs2856Driver(CXD2856_S_ISDBS_DRIVER_DATA *pDriver)
{
	kfree(pDriver);
}


CXD2856_S_ISDBS_DRIVER_DATA* AllocRealtekSIsdbs2856Driver(
	COMM*               pComm,
	unsigned char       Addr,
	U32BITS       CrystalFreq
	//,int TSMode
)
{
	CXD2856_S_ISDBS_DRIVER_DATA* pDriver = (CXD2856_S_ISDBS_DRIVER_DATA*) kmalloc(
				sizeof(CXD2856_S_ISDBS_DRIVER_DATA) +
				sizeof(sony_demod_t)   +
				sizeof(I2C_BRIDGE_MODULE)   +
				sizeof(BASE_INTERFACE_MODULE), GFP_KERNEL);

	if (pDriver) {
		//BASE_INTERFACE_MODULE* pBIF;
		//sony_demod_t* pDemod;

		memset(pDriver, 0, sizeof(CXD2856_S_ISDBS_DRIVER_DATA));
		pDriver->pDemod                = (sony_demod_t*)(((unsigned char*)pDriver) + sizeof(CXD2856_S_ISDBS_DRIVER_DATA));
		pDriver->pBaseInterface        = (BASE_INTERFACE_MODULE*)(((unsigned char*)pDriver->pDemod) + sizeof(sony_demod_t));
		pDriver->pI2CBridge            = (I2C_BRIDGE_MODULE*)(((unsigned char*)pDriver->pBaseInterface) + sizeof(BASE_INTERFACE_MODULE));
		pDriver->DeviceAddr            = Addr;
		pDriver->CrystalFreqHz         = CrystalFreq;

		// Init Base IF
		pBaseIfIsdbs = pDriver->pBaseInterface;

		pBaseIfIsdbs->I2cReadingByteNumMax      = RTK_DEMOD_BIF_RX_FIFO_DEPTH;
		pBaseIfIsdbs->I2cWritingByteNumMax      = RTK_DEMOD_BIF_TX_FIFO_DEPTH;
		pBaseIfIsdbs->I2cRead                   = __realtek_ext_i2c_read;
		pBaseIfIsdbs->I2cWrite                  = __realtek_ext_i2c_write;
		pBaseIfIsdbs->WaitMs                    = __realtek_ext_wait_ms;
		pBaseIfIsdbs->SetUserDefinedDataPointer = ext_interface_SetUserDefinedDataPointer;
		pBaseIfIsdbs->GetUserDefinedDataPointer = ext_interface_GetUserDefinedDataPointer;
		pBaseIfIsdbs->SetUserDefinedDataPointer(pBaseIfIsdbs, (void*)pComm);
		pBaseIfIsdbs->useI2C = 0;


	}

	return pDriver;
}

sony_result_t CXD2856_i2c_IsdbsRead(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode)
{
	/*
		Implement I2C functions
	*/

	int ret = FUNCTION_SUCCESS;
	unsigned char* pRegisterAddr = NULL;
        unsigned char RegisterAddrSize = '\0';

        ret = __realtek_ext_i2c_read(pBaseIfIsdbs, deviceAddress, pRegisterAddr, RegisterAddrSize, pData, (U32BITS) size);

	SONY_TRACE_I2C_RETURN((ret == FUNCTION_SUCCESS) ? SONY_RESULT_OK : SONY_RESULT_ERROR_I2C);

}

sony_result_t CXD2856_i2c_IsdbsWrite(sony_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t *pData, uint32_t size, uint8_t mode)
{
	/*
		Implement I2C functions
	*/
	int ret = FUNCTION_SUCCESS;
	//int i;
    //
	//CXD2856_S_ISDBS_INFO("Write : 0x%02X ", deviceAddress);
	//CXD2856_S_ISDBS_INFO("0x%02X ", pData[0]);
	//for(i = 1; i < size; i++) CXD2856_S_ISDBS_INFO("0x%02X ", pData[i]);
	//CXD2856_S_ISDBS_INFO("\n");

	
	ret = __realtek_ext_i2c_write(pBaseIfIsdbs, deviceAddress, pData, (U32BITS) size);

	SONY_TRACE_I2C_RETURN((ret == FUNCTION_SUCCESS) ? SONY_RESULT_OK : SONY_RESULT_ERROR_I2C);
}

