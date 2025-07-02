/***************************************************************************************************
  File        : demod_rtk_s_isdbs3_cxd2878.cpp
  Description : CXD2878_S_ISDBS3 demod
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
#include "demod_rtk_s_isdbs3_cxd2878.h"
#include "demod_rtk_s_isdbs3_cxd2878_priv.h"
#include <rtk_kdriver/pcbMgr.h>

#include "CXD2878Family_refcode/sony_demod.h"
#include "CXD2878Family_refcode/isdb_sat/sony_isdbs3.h"
#include "CXD2878Family_refcode/isdb_sat/sony_demod_isdbs3.h"
#include "CXD2878Family_refcode/isdb_sat/sony_demod_isdbs3_monitor.h"
#include "CXD2878Family_refcode/isdb_sat/sony_integ_isdbs3.h"

static int isCreated = 0;
static sony_demod_t* pDemod = NULL;
//sony_demod_create_param_t gCreateParam;

static BASE_INTERFACE_MODULE*	pBaseIfIsdbs_2878;
static const char *Isdbs3_Result[13] = {"OK", "Argument Error", "I2C Error", "SW State Error", "HW State Error", "Timeout", "Unlock", "Out of Range", "No Support", "Cancelled", "Other Error", "Overflow", "OK - Confirm"};
extern unsigned char I2c_SpeedByPCB;
/*----------------------------------------------------------------------
 * Func : CXD2878_S_ISDBS3
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
void CXD2878_S_ISDBS3_Constructors(DEMOD* pDemodKernel, unsigned char addr, unsigned char output_mode, unsigned char output_freq, COMM* pComm)
{
	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pDemodKernel->private_data = kmalloc(sizeof(CXD2878_S_ISDBS3), GFP_KERNEL);
	if(pDemodKernel->private_data == NULL) {
		goto kmalloc_fail;
	}
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	ExtDemod_Constructors(pDemodKernel);

	pDemodKernel->m_id                 = DTV_DEMOD_SONY_ISDBS3_2878;
	pDemodKernel->m_addr               = addr;
	pDemodKernel->m_pTuner             = NULL;
	pDemodKernel->m_pComm              = pComm;
	pDemodKernel->m_ScanRange          = SCAN_RANGE_3_6;
	pDemodKernel->m_ScanMode           = CH_UNKNOW;
	pDemodKernel->m_Capability         = TV_SYS_TYPE_ISDBS3_CXD2878;
	pDemodKernel->m_update_interval_ms = 300;
	pDemodKernel->m_if.freq            = 36125000;
	pDemodKernel->m_if.inversion       = 1;
	pDemodKernel->m_if.agc_pola        = 0;
	pDemodKernel->m_clock              = CRYSTAL_FREQ_27000000HZ;
	pRealtekSISDBS->m_output_freq        = output_freq;
	pRealtekSISDBS->m_private            = (void*) AllocRealtekSIsdbs3Driver(
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
	pDemodKernel->m_pComm->SetTargetName(pDemodKernel->m_pComm, pDemodKernel->m_addr, "CXD2878_S_ISDBS3");
	pRealtekSISDBS->m_pTunerOptReg       = NULL;
	pRealtekSISDBS->m_TunerOptRegCnt     = 0;
	pRealtekSISDBS->m_acquire_sig_en = 0;
	pDemodKernel->Init_Ext = CXD2878_S_ISDBS3_Init;
	pDemodKernel->Reset = ExtDemod_Reset;
	pDemodKernel->AcquireSignal = CXD2878_S_ISDBS3_AcquireSignal;
	pDemodKernel->ScanSignal = CXD2878_S_ISDBS3_ScanSignal;
	pDemodKernel->SetIF = CXD2878_S_ISDBS3_SetIF;
	pDemodKernel->SetTSMode = CXD2878_S_ISDBS3_SetTSMode;
	pDemodKernel->SetTVSysEx = CXD2878_S_ISDBS3_SetTVSysEx;
	pDemodKernel->SetMode = CXD2878_S_ISDBS3_SetMode;
	pDemodKernel->SetTVSys = CXD2878_S_ISDBS3_SetTVSys;
	pDemodKernel->GetLockStatus = CXD2878_S_ISDBS3_GetLockStatus;
	pDemodKernel->GetSignalInfo = CXD2878_S_ISDBS3_GetSignalInfo;
	pDemodKernel->GetSignalQuality = CXD2878_S_ISDBS3_GetSignalQuality;
	pDemodKernel->GetDemodStatus = ExtDemod_GetDemodStatus;
	pDemodKernel->GetCarrierOffset = CXD2878_S_ISDBS3_GetCarrierOffset;
	pDemodKernel->ForwardI2C = CXD2878_S_ISDBS3_ForwardI2C;
	pDemodKernel->AutoTune = CXD2878_S_ISDBS3_AutoTune;
	pDemodKernel->KernelAutoTune = CXD2878_S_ISDBS3_KernelAutoTune;
	pDemodKernel->GetCapability = ExtDemod_GetCapability;
	pDemodKernel->Suspend = ExtDemod_Suspend;
	pDemodKernel->Resume = ExtDemod_Resume;
	pDemodKernel->SetTvMode = CXD2878_S_ISDBS3_SetTvMode;
	pDemodKernel->SetExtDemodInfo = CXD2878_S_ISDBS3_SetExtDemodInfo;
	//pDemodKernel->DebugInfo = CXD2878_S_ISDBS3_DebugInfo;	  	  // Add monitor function that used to monitor demod status
	pDemodKernel->InspectSignal = CXD2878_S_ISDBS3_InspectSignal; // Add monitor function that used to monitor demod status
	pDemodKernel->AttachTuner = ExtDemod_AttachTuner;
	pDemodKernel->GetTSParam = ExtDemod_GetTSParam;
	pDemodKernel->Lock = CXD2878_S_ISDBS3_Lock;
	pDemodKernel->Unlock = CXD2878_S_ISDBS3_Unlock;
	pDemodKernel->Destory = CXD2878_S_ISDBS3_Destructors;

kmalloc_fail:
	CXD2878_S_ISDBS3_INFO("CXD2878_S_ISDBS3_Constructors kmalloc Failed\n");
}


/*----------------------------------------------------------------------
 * Func : ~CXD2878_S_ISDBS3
 *
 * Desc : Destructor of CXD2878_S_ISDBS
 *
 * Parm : pDemod : handle of Demod
 *
 * Retn : N/A
 *----------------------------------------------------------------------*/
void CXD2878_S_ISDBS3_Destructors(DEMOD* pDemodKernel)
{
	CXD2878_S_ISDBS3* pRealtekSISDBS;
	sony_result_t result = SONY_RESULT_OK;
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	result = CXD2878_demod_Sleep(pDemod);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("CXD2878_demod_Sleep fail (%d)\n", result);
	}

	pDemod = NULL;
	isCreated = 0;
	//if (pRealtekSISDBS->m_private != NULL)
	ReleaseRealtekSIsdbs3Driver((CXD2878_S_ISDBS3_DRIVER_DATA*)pRealtekSISDBS->m_private);

	kfree(pRealtekSISDBS);
	//pDemodKernel->Destory(pDemodKernel);
	CXD2878_S_ISDBS3_INFO("CXD2878_S_ISDBS3_Destructors Complete\n");
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
int CXD2878_S_ISDBS3_Init(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode)
{
	CXD2878_S_ISDBS3* pRealtekSISDBS;
	sony_demod_create_param_t createParam;
	static sony_i2c_t DemodI2c;
	sony_result_t result = SONY_RESULT_OK;

	unsigned char dem_slv[2] = {0xda, 0xc8};
	unsigned long long param = 0;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	createParam.xtalFreq       = SONY_DEMOD_XTAL_24000KHz;/* 24MHz Xtal */
	createParam.tunerI2cConfig = SONY_DEMOD_TUNER_I2C_CONFIG_DISABLE;/* I2C repeater is used */

	pDemodKernel->Lock(pDemodKernel);
	pRealtekSISDBS->m_auto_tune_enable = 0;

	if (!isCreated) {
		DemodI2c.Read             = CXD2878_i2c_Isdbs3Read;
		DemodI2c.Write            = CXD2878_i2c_Isdbs3Write;
		DemodI2c.ReadRegister     = CXD2878_i2c_CommonReadRegister;
		DemodI2c.WriteRegister    = CXD2878_i2c_CommonWriteRegister;
		DemodI2c.WriteOneRegister = CXD2878_i2c_CommonWriteOneRegister;

		if(mode == TV_SYS_TYPE_ISDBS3_CXD2878)
			createParam.i2cAddressSLVT = dem_slv[0];
		else if(mode == TV_SYS_TYPE_ISDBS3_CXD2878_2nd)
			createParam.i2cAddressSLVT = dem_slv[1];
		else
			createParam.i2cAddressSLVT = 0; // do not select the correct mode

		result = CXD2878_demod_Create(pDemod, &createParam, &DemodI2c);
		if (result != SONY_RESULT_OK) {
			CXD2878_S_ISDBS3_INFO("sony_demod_Create 1st error (%d)\n", result);
			goto init_fail;
		}
		result = CXD2878_demod_Initialize(pDemod);
		if (result != SONY_RESULT_OK) {
			CXD2878_S_ISDBS3_INFO("sony_demod_Initialize 1st error (%d)\n", result);
			goto init_fail;
		}	
/*		
		createParam.i2cAddressSLVT =  dem_slv[1];
		result |= CXD2878_demod_Create(pDemod, &createParam, &DemodI2c);
		if (result != SONY_RESULT_OK) {
			CXD2878_S_ISDBS3_INFO("sony_demod_Create 2nd error (%d)\n", result);
			return TUNER_CTRL_FAIL;
		}
		
		result = CXD2878_demod_Initialize(pDemod);
		if (result != SONY_RESULT_OK) {
			CXD2878_S_ISDBS3_INFO("sony_demod_Initialize 2nd error (%d)\n", result);
			goto init_fail;
		}
*/
		isCreated = 1;
		if (pcb_mgr_get_enum_info_byname("EXTDEMOD_I2C_SPEED", &param) != 0) {
			CXD2878_S_ISDBS3_WARNING("CXD2878_ISDBS3[%d]: Get EXTDEMOD_I2C_SPEED fail use default !!!!!!!!!!!!!!!\n", __LINE__);
			param = 0x0;
		}
		I2c_SpeedByPCB = (unsigned char)param;
		CXD2878_S_ISDBS3_INFO("CXD2878_ISDBS3[%d]: set EXTDEMOD_I2C speed = %d\n", __LINE__, param);
	}
/*
	result = CXD2878_demod_I2cRepeaterEnable(pDemod, 1);
	if (result != SONY_RESULT_OK) {
		//printf("sony_demod_I2cRepeaterEnable error (%d)\n", result);
		goto init_fail;
	}

	result = CXD2878_demod_Shutdown(pDemod);
	if (result != SONY_RESULT_OK) {
		//printf("sony_demod_Shutdown error (%d)\n", result);
		goto init_fail;
	}
*/
	pDemodKernel->Unlock(pDemodKernel);
	CXD2878_S_ISDBS3_INFO("CXD2878_S_ISDBS3_Init Complete\n");
	return TUNER_CTRL_OK;

init_fail:
	pDemodKernel->Unlock(pDemodKernel);
	CXD2878_S_ISDBS3_WARNING("CXD2878_S_ISDBS3_Init Fail\n");
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
int CXD2878_S_ISDBS3_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys)
{
	CXD2878_S_ISDBS3* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);

	CXD2878_S_ISDBS3_INFO("\033[1;32;31m" "%s %s %d, system = %d\n" "\033[m", __FILE__, __func__, __LINE__, sys);
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
int CXD2878_S_ISDBS3_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO*  pSigInfo)
{
	TUNER_PARAM Param;
	CXD2878_S_ISDBS3* pRealtekSISDBS;
	unsigned char i2cAddr = 0;
	const char *streamidtype_str[] = { "Stream ID", "Relative Stream Number" };


	sony_result_t result = SONY_RESULT_OK;
	sony_isdbs3_tune_param_t Isdbs3Param;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if (!IsExtIsdbSys(sys)) {
		CXD2878_S_ISDBS3_WARNING("Set TV system failed, unsupported TV system\n");
		return  TUNER_CTRL_FAIL;
	}

	CXD2878_S_ISDBS3_INFO("pSigInfo->isdbt.portIdx=%u pSigInfo->isdbs_ext.portIdx=%u  pSigInfo->isdbs3_ext.portIdx=%u\n", pSigInfo->isdbt.portIdx, pSigInfo->isdbs_ext.portIdx, pSigInfo->isdbs3_ext.portIdx);

	//get current demod i2c
	switch (pSigInfo->isdbs3_ext.portIdx) {
	case I2C_PORT1:
		i2cAddr = 0xda;
		CXD2878_S_ISDBS3_INFO("Use demod1 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT2:
		i2cAddr = 0xc8;
		CXD2878_S_ISDBS3_INFO("Use demod2 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT3:
		i2cAddr = 0xca;
		CXD2878_S_ISDBS3_INFO("Use demod3 i2cAddr = 0x%x\n", i2cAddr);
		break;
	default:
		CXD2878_S_ISDBS3_WARNING("unknown Demod port index !!!\n");
		goto SetDemod_fail;
	}

	pDemod->i2cAddressSLVT = i2cAddr;
	pDemod->i2cAddressSLVX = i2cAddr + 4;
	pDemod->i2cAddressSLVR = i2cAddr - 0x40;
	pDemod->i2cAddressSLVM = i2cAddr - 0xA8;	

	pRealtekSISDBS->m_auto_tune_enable = 0;

	/* Configure the ISDBS tune parameters based on the channel requirements */
	Isdbs3Param.centerFreqKHz = pSigInfo->isdbs3_ext.freq_khz;              /* Channel center frequency in KHz */
	Isdbs3Param.streamid = pSigInfo->isdbs3_ext.ts_id;                        /* TS ID */
	//Isdbs3Param.streamidType = SONY_ISDBS3_STREAMID_TYPE_RELATIVE_STREAM_NUMBER; /* TS ID type (absolute TSID or relative TS number) */
	Isdbs3Param.streamidType = SONY_ISDBS3_STREAMID_TYPE_STREAMID; /* TS ID type (absolute TSID or relative TS number) */
	
	CXD2878_S_ISDBS3_INFO("Tune to ISDB-S3 signal with the following parameters:\n");
	CXD2878_S_ISDBS3_INFO(" - Center Freq    : %u kHz\n", Isdbs3Param.centerFreqKHz);
	CXD2878_S_ISDBS3_INFO(" - TS ID          : 0x%04X\n", Isdbs3Param.streamid);
	CXD2878_S_ISDBS3_INFO(" - TS ID Type     : %s\n", streamidtype_str[pSigInfo->isdbs3_ext.ts_type]);

	
	CXD2878_S_ISDBS3_INFO("\033[1;32;31m" "%s %s %d, system = %d\n" "\033[m", __FILE__, __func__, __LINE__, sys);

	//TUNER_PARAM Param;
	if (pDemodKernel->m_pTuner->GetParam(pDemodKernel->m_pTuner, sys, &Param) == TUNER_CTRL_OK) {
		if ((pDemodKernel->m_if.freq != Param.if_freq) || (pDemodKernel->m_if.inversion != Param.if_inversion) || (pDemodKernel->m_if.agc_pola  != Param.if_agc_pola)) {
			pDemodKernel->m_if.freq = Param.if_freq;
			pDemodKernel->m_if.inversion = Param.if_inversion;
			pDemodKernel->m_if.agc_pola = Param.if_agc_pola;
			if (pDemodKernel->SetIF(pDemodKernel, &pDemodKernel->m_if) != TUNER_CTRL_OK)  return  TUNER_CTRL_FAIL;

		}
	}
	CXD2878_S_ISDBS3_INFO("m_if.freq = "PT_S32BITS", m_if.inversion = %d, m_if.agc_pola = %d\n", pDemodKernel->m_if.freq, pDemodKernel->m_if.inversion, pDemodKernel->m_if.agc_pola);

	pDemodKernel->Lock(pDemodKernel);

	result = CXD2878_demod_Initialize(pDemod);

	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("CXD2878_demod_Initialize error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = CXD2878_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_SAT_IFAGCNEG, 1);
    if (result != SONY_RESULT_OK) {
        CXD2878_S_ISDBS3_INFO("sony_demod_SetConfig (SONY_DEMOD_CONFIG_SAT_IFAGCNEG) failed. (%s)\n", result);
        return result;
    }

	/*
	* In default, The setting is optimized for Sony silicon tuners.
	* If non-Sony tuner is used, the user should call following to
	* disable Sony silicon tuner optimized setting.
	*/
	//result = CXD2878_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_TUNER_OPTIMIZE, SONY_DEMOD_TUNER_OPTIMIZE_NONSONY);
	//if (result != SONY_RESULT_OK) {
	//	CXD2878_S_ISDBS3_INFO ("sony_demod_SetConfig (SONY_DEMOD_CONFIG_TUNER_OPTIMIZE) failed. (%s)\n", Isdbs_Result[result]);
	//	return result;
	//}

	result = CXD2878_demod_SetTSClockModeAndFreq(pDemod, SONY_DTV_SYSTEM_ISDBS3);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("CXD2878_demod_SetTSClockModeAndFreq error (%d)\n", result);
		goto SetDemod_fail;
	}

	if(pSigInfo->isdbs3_ext.portIdx == I2C_PORT1) {
		result = CXD2878_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_TLV_PARALLEL_SEL, 1);
	}
	else {
		result = CXD2878_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_TLV_PARALLEL_SEL, 0);
	}
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("Set SONY_DEMOD_CONFIG_TLV_PARALLEL_SEL error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = CXD2878_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_TLV_SER_DATA_ON_MSB, 0);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("Set SONY_DEMOD_CONFIG_TLV_SER_DATA_ON_MSB error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = CXD2878_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_TLV_OUTPUT_SEL_MSB, 1);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("Set SONY_DEMOD_CONFIG_TLV_OUTPUT_SEL_MSB error (%d)\n", result);
		goto SetDemod_fail;
	}

	//detault set rising edge
	result = CXD2878_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_TLV_LATCH_ON_POSEDGE, 1);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("Set SONY_DEMOD_CONFIG_TLV_LATCH_ON_POSEDGE error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = CXD2878_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_SAT_TLV_SERIAL_CLK_FREQ, 0);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("Set SONY_DEMOD_CONFIG_SAT_TLV_SERIAL_CLK_FREQ error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = CXD2878_demod_isdbs3_Tune(pDemod, &Isdbs3Param);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("CXD2878_demod_isdbs_Tune error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = CXD2878_demod_TuneEnd(pDemod);
	if (result != SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("sony_demod_Shutdown error (%d)\n", result);
		goto SetDemod_fail;
	}

	pDemodKernel->Unlock(pDemodKernel);
	CXD2878_S_ISDBS3_WARNING("CXD2878_S_ISDBS3_SetTVSysEx done\n");
	pRealtekSISDBS->m_acquire_sig_en = 1;
	pRealtekSISDBS->m_status_checking_stime = tv_osal_time_ext() + ISDBS3_STATUS_CHECK_INTERVAL_MS;
	pRealtekSISDBS->m_auto_tune_enable = 1;

	return TUNER_CTRL_OK;

SetDemod_fail:
	pDemodKernel->Unlock(pDemodKernel);
	CXD2878_S_ISDBS3_WARNING("CXD2878_S_ISDBS3_SetTVSysEx Fail\n");
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
int CXD2878_S_ISDBS3_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam)
{
	int ret = TUNER_CTRL_OK;

	CXD2878_S_ISDBS3* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);

	CXD2878_S_ISDBS3_INFO("SetIF, IF_freq="PT_U32BITS" spectrumn inv=%d, agc_pola=%d\n", pParam->freq, pParam->inversion, pParam->agc_pola);
	
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
int CXD2878_S_ISDBS3_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode)
{
	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

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
		CXD2878_S_ISDBS3_WARNING("unknown TV_OP_MODE\n");
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
int CXD2878_S_ISDBS3_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam)
{
	int ret = TUNER_CTRL_OK;
	unsigned int on_off = -1;

	CXD2878_S_ISDBS3* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if(pParam->portIdx != 0x0)
	{
		pDemod->i2cAddressSLVT = pParam->portIdx;
		pDemod->i2cAddressSLVX = pParam->portIdx + 4;
		pDemod->i2cAddressSLVR = pParam->portIdx - 0x40;
		pDemod->i2cAddressSLVM = pParam->portIdx - 0xA8;
	}

	pDemodKernel->Lock(pDemodKernel);
	if(pParam->tsOutEn == 1) {
		ret |= sony_demod_SetStreamOutput(pDemod, 1);
		if (ret != SONY_RESULT_OK)
			CXD2878_S_ISDBS3_INFO("sony_demod_SetStreamOutput error (%d)\n", ret);
	}
	else {
		ret |= sony_demod_SetStreamOutput(pDemod, 0);
		if (ret != SONY_RESULT_OK)
			CXD2878_S_ISDBS3_INFO("sony_demod_SetStreamOutput error (%d)\n", ret);
	}

	ret |= sony_demod_Get_TSTLV_Output(pDemod, &on_off);
	CXD2878_S_ISDBS3_INFO("Get TSTLV Output = %d\n", on_off);
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
int CXD2878_S_ISDBS3_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock)
{
	sony_result_t result = SONY_RESULT_OK;
	unsigned char AgcLock = 0, TsLock = 0, TmccLock = 0;

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);

	result = CXD2878_demod_isdbs3_monitor_SyncStat(pDemod, &AgcLock, &TsLock, &TmccLock);
	CXD2878_S_ISDBS3_INFO("\033[1;32;36m" "AGC Lock = %u, TS Lock = %u TMCC Lock = %u\n" "\033[m", AgcLock, TsLock, TmccLock);

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
int CXD2878_S_ISDBS3_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo)
{
	sony_result_t result = SONY_RESULT_OK;
	CXD2878_S_ISDBS3*	pRealtekSISDBS;
	sony_isdbs3_mod_t modH, modL;//isdbs3 modcod
	sony_isdbs3_cod_t codH, codL;
	unsigned char Cnt = 0, siteDiversityInfo = 0;
	sony_isdbs3_tmcc_info_t tmccInfo;
	sony_demod_sat_iq_sense_t iqSense;

	const char *mod_str[] = {
        "Reserved 0", "BPSK", "QPSK", "8PSK",
        "16PSK", "32PSK", "Reserved 6", "Reserved 7",
        "Reserved 8", "Reserved 9", "Reserved 10", "Reserved 11",
        "Reserved 12", "Reserved 13", "Reserved 14", "Unused"
    };
    const char *cod_str[] = {
        "Reserved 0", "41/120(1/3)", "49/120(2/5)", "61/120(1/2)",
        "73/120(3/5)", "81/120(2/3)", "89/120(3/4)", "93/120(7/9)",
        "97/120(4/5)", "101/120(5/6)", "105/120(7/8)", "109/120(9/10)",
        "Reserved 12", "Reserved 13", "Reserved 14", "Unused"
    };
    //const char *type_str[] = {
    //    "Reserved 0", "MPEG-2 TS", "TLV"
    //};
	const char *iqsense_str[] = { "Normal IQ", "Inverted IQ" };
	
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	//need implement
	pDemodKernel->Lock(pDemodKernel);

	result = CXD2878_demod_isdbs3_monitor_ModCod(pDemod, &modH, &modL, &codH, &codL);
	if (result == SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("ModCod (High)   | %s, %s\n", mod_str[modH], cod_str[codH]);
        CXD2878_S_ISDBS3_INFO("ModCod (Low)    | %s, %s\n", mod_str[modL], cod_str[codL]);
		pInfo->isdbs3_ext.modH = (isdbs3_mod_t) modH;
		pInfo->isdbs3_ext.modL = (isdbs3_mod_t) modL;
		pInfo->isdbs3_ext.codH = (isdbs3_cod_t) codH;
		pInfo->isdbs3_ext.codL = (isdbs3_cod_t) codL;
	}else {
		CXD2878_S_ISDBS3_INFO ("ModCod  | Error  | %s\n", Isdbs3_Result[result]);
	}
	
	//tmcc info
	result |= CXD2878_demod_isdbs3_monitor_TMCCInfo (pDemod, &tmccInfo);
	if (result == SONY_RESULT_OK) {
		CXD2878_S_ISDBS3_INFO("TMCCInfo | Change Order    | %d\n", tmccInfo.changeOrder);
		CXD2878_S_ISDBS3_INFO("         | EWS Flag        | %d\n", tmccInfo.ewsFlag);
		CXD2878_S_ISDBS3_INFO("         | Uplink Info     | %d\n", tmccInfo.uplinkInfo);
		pInfo->isdbs3_ext.tmccInfo.changeOrder = tmccInfo.changeOrder;
		pInfo->isdbs3_ext.tmccInfo.ewsFlag = tmccInfo.ewsFlag;
		pInfo->isdbs3_ext.tmccInfo.uplinkInfo = tmccInfo.uplinkInfo;
		
		for (Cnt = 0; Cnt < 8; Cnt++) {
			//CXD2878_S_ISDBS3_INFO("         | Mod Cod #%d      | %s, %s\n", Cnt, mod_str[tmccInfo.modcodSlotInfo[Cnt].mod], cod_str[tmccInfo.modcodSlotInfo[Cnt].cod]);
            //CXD2878_S_ISDBS3_INFO("         | Slot Num of #%d  | %d\n", Cnt, tmccInfo.modcodSlotInfo[Cnt].slotNum);
            //CXD2878_S_ISDBS3_INFO("         | Backoff #%d      | %d\n", Cnt, tmccInfo.modcodSlotInfo[Cnt].backoff);
			pInfo->isdbs3_ext.tmccInfo.modcodSlotInfo[Cnt].mod = (isdbs3_mod_t) tmccInfo.modcodSlotInfo[Cnt].mod;
			pInfo->isdbs3_ext.tmccInfo.modcodSlotInfo[Cnt].cod = (isdbs3_cod_t) tmccInfo.modcodSlotInfo[Cnt].cod;
			pInfo->isdbs3_ext.tmccInfo.modcodSlotInfo[Cnt].slotNum = tmccInfo.modcodSlotInfo[Cnt].slotNum;
			pInfo->isdbs3_ext.tmccInfo.modcodSlotInfo[Cnt].backoff = tmccInfo.modcodSlotInfo[Cnt].backoff;
		}
        for (Cnt = 0; Cnt < 16; Cnt++) {
			if (tmccInfo.streamTypeForEachRelativeStream[Cnt] <= SONY_ISDBS3_STREAM_TYPE_TLV) {
			    //CXD2878_S_ISDBS3_INFO("         | Stream Type #%-2d | %s\n", Cnt, type_str[(uint8_t)tmccInfo.streamTypeForEachRelativeStream[Cnt]]);
				pInfo->isdbs3_ext.tmccInfo.streamTypeForEachRelativeStream[Cnt] = (isdbs3_stream_type_t) tmccInfo.streamTypeForEachRelativeStream[Cnt];
            } else if (tmccInfo.streamTypeForEachRelativeStream[Cnt] == SONY_ISDBS3_STREAM_TYPE_NO_TYPE_ALLOCATED) {
			    CXD2878_S_ISDBS3_INFO("         | Stream Type #%-2d | Unused\n", Cnt);
            } else {
                //CXD2878_S_ISDBS3_INFO("         | Stream Type #%-2d | Reserved %d\n", Cnt, (uint8_t)tmccInfo.streamTypeForEachRelativeStream[Cnt]);
				pInfo->isdbs3_ext.tmccInfo.streamTypeForEachRelativeStream[Cnt] = (isdbs3_stream_type_t) tmccInfo.streamTypeForEachRelativeStream[Cnt];
            }
                //CXD2878_S_ISDBS3_INFO("         | Stream ID #%-2d   | 0x%04X\n", Cnt, tmccInfo.streamidForEachRelativeStream[Cnt]);
				pInfo->isdbs3_ext.tmccInfo.streamidForEachRelativeStream[Cnt] = tmccInfo.streamidForEachRelativeStream[Cnt];
		}
	}else {
		CXD2878_S_ISDBS3_INFO(" TMCC Information        | Error           |  %s\n", Isdbs3_Result[result]);
	}
	
	result |= CXD2878_demod_isdbs3_monitor_SiteDiversityInfo (pDemod, &siteDiversityInfo);
	if (result == SONY_RESULT_OK) {
		//CXD2878_S_ISDBS3_INFO(" SiteDiversityInfo | Site Diversity  | %u\n", siteDiversityInfo);
		pInfo->isdbs3_ext.siteDiversityInfo = siteDiversityInfo;
	}
	else {
		CXD2878_S_ISDBS3_INFO(" SiteDiversityInfo | Error           | %s\n", Isdbs3_Result[result]);
	}
	
	result = CXD2878_demod_isdbs3_monitor_IQSense (pDemod, &iqSense);
	if (result == SONY_RESULT_OK) {
        CXD2878_S_ISDBS3_INFO (" IQSense        | IQ Sense  | %s\n", iqsense_str[iqSense]);
    }
    else {
        CXD2878_S_ISDBS3_INFO (" IQSense        | Error     | %s\n", Isdbs3_Result[result]);
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
int CXD2878_S_ISDBS3_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality)
{
	sony_result_t result = SONY_RESULT_OK;

	unsigned char AgcLock = 0, TsLock = 0, TmccLock = 0, TSIDError = 0;
	int CNR = 0, TxRSSIdBm = 0, offset = 0;
	unsigned int BerH = 0, BerL = 0;
	unsigned int ldpcBerH = 0, ldpcBerL = 0;
	unsigned int FerH = 0, FerL = 0;
	unsigned int ifAGCOut = 0;

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if(pQuality->demod_i2cAddr != 0x0)
	{
		pDemod->i2cAddressSLVT = pQuality->demod_i2cAddr;
		pDemod->i2cAddressSLVX = pQuality->demod_i2cAddr + 4;
		pDemod->i2cAddressSLVR = pQuality->demod_i2cAddr - 0x40;
		pDemod->i2cAddressSLVM = pQuality->demod_i2cAddr - 0xA8;
	}

	pDemodKernel->Lock(pDemodKernel);

	switch (id) {
	case TV_QUAL_LOCKSTATUS:
		result = CXD2878_demod_isdbs3_monitor_SyncStat(pDemod, &AgcLock, &TsLock, &TmccLock);
		CXD2878_S_ISDBS3_INFO("\033[1;32;36m" "AGC Lock = %u, TS Lock = %u TMCC Lock = %u [i2c=0x%x]\n" "\033[m", AgcLock, TsLock, TmccLock, pDemod->i2cAddressSLVT);
		pQuality->ExtIsdb_Status.lock[0] = AgcLock;
		pQuality->ExtIsdb_Status.lock[1] = TsLock;
		pQuality->ExtIsdb_Status.lock[2] = TmccLock;
		break;

	case TV_QUAL_NOSIG :
	case TV_QUAL_INNERLOCK :
		result = CXD2878_demod_isdbs3_monitor_SyncStat(pDemod, &AgcLock, &TsLock, &TmccLock);
		pQuality->nosig = (TsLock) ? 0 : 1;
		pQuality->inner = (TmccLock) ? 1 : 0;
		break;

	case TV_QUAL_SNR:
		result = CXD2878_demod_isdbs3_monitor_CNR(pDemod, &CNR);//SNR(dB) * 1000
		CXD2878_S_ISDBS3_INFO (" CNR | %d x 10^-3 dB result = %u\n", CNR, result);
		pQuality->snr = CNR * 10000;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	case TV_QUAL_BEC:
	case TV_QUAL_BER:
	case TV_QUAL_PRELDPCBER:
		result = CXD2878_demod_isdbs3_monitor_PreLDPCBER(pDemod, &ldpcBerH, &ldpcBerL);
		CXD2878_S_ISDBS3_INFO (" Pre-LDPB BER | (High) %u (Low) %u  | x 10^-7\n", ldpcBerH, ldpcBerL);
		pQuality->ExtIsdbs3_Status.preLDPCBer[0] = ldpcBerH;
		pQuality->ExtIsdbs3_Status.preLDPCBer[1] = ldpcBerL;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;
	case TV_QUAL_PEC:
	case TV_QUAL_PER:
	case TV_QUAL_POSTBCHFER:
 		result = CXD2878_demod_isdbs3_monitor_PostBCHFER(pDemod, &FerH, &FerL);
		CXD2878_S_ISDBS3_INFO (" Post-BCH FER | (High) %u (Low) %u | x 10^-6\n", FerH, FerL);
		pQuality->ExtIsdbs3_Status.postBCHFer[0] = FerH;
		pQuality->ExtIsdbs3_Status.postBCHFer[1] = FerL;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;
	case TV_QUAL_UCBLOCKS:
		pQuality->ucblocks = 0;
		result = SONY_RESULT_OK;

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
		//result = CXD2878_demod_isdbs3_monitor_CNR(pDemod, &CNR1);//CNR(dB) * 1000
		pQuality->quality = 100;
		break;
	case TV_QUAL_AGC:
		result = CXD2878_demod_isdbs3_monitor_IFAGCOut(pDemod, &ifAGCOut);
		pQuality->agc = ifAGCOut;
		CXD2878_S_ISDBS3_INFO (" IF AGC | %u\n", ifAGCOut);
		break;
	case TV_QUAL_CARRIER_OFFSET:
		result = CXD2878_demod_isdbs3_monitor_CarrierOffset(pDemod, &offset);
		pQuality->ExtIsdb_Status.freq_offset = offset;
		pQuality->ExtIsdb_Status.demod_ret = result;
		CXD2878_S_ISDBS3_INFO (" Carrier Offset | %d\n", offset);
		break;
	case TV_QUAL_PREBCHBER:
		result = CXD2878_demod_isdbs3_monitor_PreBCHBER(pDemod, &BerH, &BerL);
		CXD2878_S_ISDBS3_INFO ("PreBCHBER | BER (High)      | %u x 10^-7\n", BerH);
		CXD2878_S_ISDBS3_INFO ("PreBCHBER | BER (Low)       | %u x 10^-7\n", BerL);

		if(BerH == BerL)
		{
			pQuality->ExtIsdbs3_Status.preBCHBerH = 0;
			pQuality->ExtIsdbs3_Status.preBCHBerL = BerL;
		}
		else
		{
			pQuality->ExtIsdbs3_Status.preBCHBerH = BerH;
			pQuality->ExtIsdbs3_Status.preBCHBerL = BerL;
		}
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	case TV_QUAL_TSIDERROR:
		result = CXD2878_demod_isdbs3_monitor_StreamIDError(pDemod, &TSIDError);
		CXD2878_S_ISDBS3_INFO (" TSIDError = %u\n", TSIDError);
		pQuality->ExtIsdb_Status.TSIDError = TSIDError;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	default:
		CXD2878_S_ISDBS3_WARNING("unknown ENUM_TV_QUAL id = %u\n", (unsigned int) id);
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
int CXD2878_S_ISDBS3_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset)
{
	sony_result_t result = SONY_RESULT_OK;
	int offset;

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;


	pDemodKernel->Lock(pDemodKernel);


	result = CXD2878_demod_isdbs3_monitor_CarrierOffset(pDemod, &offset);


	if (result == SONY_RESULT_OK) {
		*pOffset = offset;
	} else
		*pOffset = 0;

	pDemodKernel->Unlock(pDemodKernel);
	CXD2878_S_ISDBS3_INFO("Carrier Offset = "PT_S32BITS" KHz\n", *pOffset);

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
int CXD2878_S_ISDBS3_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize)
{
	int ret = TUNER_CTRL_OK;

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

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
int CXD2878_S_ISDBS3_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock)
{
#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	unsigned long cur_time;
#endif

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;


#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	cur_time = tv_osal_time_ext();

	while ((tv_osal_time_ext() - cur_time) <= 15) {
		if (pRealtekSISDBS->m_acquire_sig_en == 0)
			return TUNER_CTRL_OK;

		tv_osal_msleep_ext(5);
	}

	return TUNER_CTRL_FAIL;

#else
	//CXD2878_S_ISDBS3_AcquireSignalThread(pDemodKernel);
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
int CXD2878_S_ISDBS3_AcquireSignalThread(DEMOD* pDemodKernel)
{
	unsigned long   stime;
	unsigned char AgcLock = 0, TsLock = 0, TmccLock = 0;
	sony_result_t result = SONY_RESULT_OK;

	CXD2878_S_ISDBS3* pRealtekSISDBS;
	//sony_demod_t* pDemod;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if (pRealtekSISDBS->m_acquire_sig_en == 0)
		return TUNER_CTRL_OK;


	CXD2878_S_ISDBS3_INFO("\033[1;32;32m" "%s %s %d\n" "\033[m", __FILE__, __func__, __LINE__);


	pDemodKernel->Lock(pDemodKernel);
	pRealtekSISDBS->m_auto_tune_enable = 0;
	stime = tv_osal_time_ext();

	while ((tv_osal_time_ext() - stime) <= 2000) {
		result = CXD2878_demod_isdbs3_monitor_SyncStat(pDemod, &AgcLock, &TsLock, &TmccLock);
		if (TsLock | TmccLock) break;
		tv_osal_msleep_ext(5);
	}

	if((TsLock |TmccLock))
	{
		CXD2878_S_ISDBS3_INFO("ISDBS3 LOCK AgcLock= %u, TsLock = %u, TmccLock = %u Time = %lu\n", AgcLock, TsLock, TmccLock, tv_osal_time_ext() - stime);	
	}
	else
	{
		CXD2878_S_ISDBS3_INFO("ISDBS3 Unlock AgcLock= %u, TsLock = %u, TmccLock = %u Time = %lu\n", AgcLock, TsLock, TmccLock, tv_osal_time_ext() - stime);
	}
	

//acquire_signal_result:
	pDemodKernel->Unlock(pDemodKernel);
	pRealtekSISDBS->m_acquire_sig_en = 0;
	pRealtekSISDBS->m_status_checking_stime = tv_osal_time_ext() + ISDBS3_STATUS_CHECK_INTERVAL_MS;
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
int CXD2878_S_ISDBS3_ScanSignal(DEMOD* pDemodKernel, S32BITS* pOffset, SCAN_RANGE range)
{
	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

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
int CXD2878_S_ISDBS3_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off)
{
	int ret = TUNER_CTRL_OK;

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

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
int CXD2878_S_ISDBS3_AutoTune(DEMOD* pDemodKernel)
{
	CXD2878_S_ISDBS3* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);

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
int CXD2878_S_ISDBS3_KernelAutoTune(DEMOD* pDemodKernel)
{
	int result = SONY_RESULT_OK;
#if 0
	unsigned long cur_time;
	CXD2878_S_ISDBS3* pRealtekSISDBS;

#ifndef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	unsigned char ewsChange;
	unsigned char LockStatus = 0;
	TV_SIG_QUAL Quality;
#endif
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	if(pRealtekSISDBS->m_auto_tune_enable == 0)
		return result;

	cur_time = tv_osal_time_ext();

#ifndef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	if ((cur_time > pRealtekSISDBS->m_status_checking_stime) || ((pRealtekSISDBS->m_status_checking_stime - cur_time) > ISDBS3_STATUS_CHECK_INTERVAL_MS)) {
		result = CXD2878_demod_isdbs3_monitor_EWSChange (pDemod, &ewsChange);
		if (result == SONY_RESULT_OK) {
			CXD2878_S_ISDBS3_INFO (" EWSChange               | EWS Change      | %d\n", ewsChange);
			if (ewsChange == 1) {
				CXD2878_demod_isdbs3_ClearEWSChange(pDemod);
			}
		}
		CXD2878_S_ISDBS3_GetLockStatus(pDemodKernel, &LockStatus);
		if (LockStatus == 1) {
			CXD2878_S_ISDBS3_GetSignalQuality(pDemodKernel, TV_QUAL_SNR, &Quality);
			CXD2878_S_ISDBS3_GetSignalQuality(pDemodKernel, TV_QUAL_BER, &Quality);
			CXD2878_S_ISDBS3_GetSignalQuality(pDemodKernel, TV_QUAL_PER, &Quality);
			CXD2878_S_ISDBS3_GetSignalQuality(pDemodKernel, TV_QUAL_SIGNAL_STRENGTH, &Quality);
			CXD2878_S_ISDBS3_GetSignalQuality(pDemodKernel, TV_QUAL_SIGNAL_QUALITY, &Quality);
			CXD2878_S_ISDBS3_GetSignalQuality(pDemodKernel, TV_QUAL_AGC, &Quality);
		}
	}
#endif

	if (pRealtekSISDBS->m_acquire_sig_en) {
#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
		CXD2878_S_ISDBS3_AcquireSignalThread(pDemodKernel);
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
int CXD2878_S_ISDBS3_InspectSignal(DEMOD* pDemodKernel)
{

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

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
int CXD2878_S_ISDBS3_Activate(DEMOD* pDemodKernel, unsigned char force_rst)
{

	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	return TUNER_CTRL_OK;

}


int CXD2878_S_ISDBS3_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode)
{
	int ret = TUNER_CTRL_FAIL;
	CXD2878_S_ISDBS3* pRealtekSISDBS;

	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemodKernel->Lock(pDemodKernel);

	CXD2878_S_ISDBS3_INFO("CXD2878_S_ISDBS3_SetTvMode(%d)\n", mode);

	switch (mode) {
	case TV_SYS_TYPE_ISDBS3_CXD2878:
	case TV_SYS_TYPE_ISDBS3_CXD2878_2nd:
		CXD2878_S_ISDBS3_INFO("Set to ISDBS3 \n");
		ret = pDemodKernel->Init_Ext(pDemodKernel, mode);
		//ret |= pDemodKernel->SetTSMode(pDemodKernel, &pDemodKernel->m_ts);

		if (ret != TUNER_CTRL_OK) {
			CXD2878_S_ISDBS3_WARNING("Set CXD2878_S_ISDBS3ND to ISDBS mode failed\n");
			goto set_demod_mode_failed;
		}
		break;

	default:
		CXD2878_S_ISDBS3_WARNING("unknown TV_SYSTEM_TYPE\n");
		goto set_demod_mode_failed;
	}

	ret = TUNER_CTRL_OK;
	CXD2878_S_ISDBS3_INFO("Set CXD2878_S_ISDBS3ND to appointed DTV mode successed\n");
	pDemodKernel->Unlock(pDemodKernel);
	return ret;

set_demod_mode_failed:
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;
}
int CXD2878_S_ISDBS3_SetExtDemodInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pSigInfo)
{
	unsigned char i2cAddr = 0;
	CXD2878_S_ISDBS3* pRealtekSISDBS;
	pRealtekSISDBS = (CXD2878_S_ISDBS3*)(pDemodKernel->private_data);
	pDemod = ((CXD2878_S_ISDBS3_DRIVER_DATA*) pRealtekSISDBS->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);
	switch (pSigInfo->isdbs3_ext.portIdx) {
	case I2C_PORT1:
		i2cAddr = 0xda;
		CXD2878_S_ISDBS3_INFO("Use demod1 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT2:
		i2cAddr = 0xc8;
		CXD2878_S_ISDBS3_INFO("Use demod2 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT3:
		i2cAddr = 0xca;
		CXD2878_S_ISDBS3_INFO("Use demod3 i2cAddr = 0x%x\n", i2cAddr);
		break;
	default:
		CXD2878_S_ISDBS3_WARNING("unknown Demod port index !!!\n");
		goto update_demod_info_failed;
	}
	
	pDemod->i2cAddressSLVT = i2cAddr;
    pDemod->i2cAddressSLVX = i2cAddr + 4;
    pDemod->i2cAddressSLVR = i2cAddr - 0x40;
    pDemod->i2cAddressSLVM = i2cAddr - 0xA8;
	
	CXD2878_S_ISDBS3_INFO("Update ISDBS CXD2878 successed\n");
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_OK;

update_demod_info_failed:
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;
}

/*-----------------------------------------------------------------
 *     BASE_INTERFACE for CXD2878_S_ISDBS3ND API
 *----------------------------------------------------------------*/
void CXD2878_S_ISDBS3_Lock(DEMOD* pDemodKernel)
{
	//FUNCTION_NAME_PRINT("LOCK 0x%x\n",&pDemodKernel->m_lock);
	//mutex_lock(&pDemodKernel->m_lock);
}


void CXD2878_S_ISDBS3_Unlock(DEMOD* pDemodKernel)
{
	//FUNCTION_NAME_PRINT("UN_LOCK 0x%x\n",&pDemodKernel->m_lock);
	//mutex_unlock(&pDemodKernel->m_lock);
}


void ReleaseRealtekSIsdbs3Driver(CXD2878_S_ISDBS3_DRIVER_DATA *pDriver)
{
	kfree(pDriver);
}


CXD2878_S_ISDBS3_DRIVER_DATA* AllocRealtekSIsdbs3Driver(
	COMM*               pComm,
	unsigned char       Addr,
	U32BITS       CrystalFreq
	//,int TSMode
)
{
	CXD2878_S_ISDBS3_DRIVER_DATA* pDriver = (CXD2878_S_ISDBS3_DRIVER_DATA*) kmalloc(
				sizeof(CXD2878_S_ISDBS3_DRIVER_DATA) +
				sizeof(sony_demod_t)   +
				sizeof(I2C_BRIDGE_MODULE)   +
				sizeof(BASE_INTERFACE_MODULE), GFP_KERNEL);

	if (pDriver) {
		//BASE_INTERFACE_MODULE* pBIF;
		//sony_demod_t* pDemod;

		memset(pDriver, 0, sizeof(CXD2878_S_ISDBS3_DRIVER_DATA));
		pDriver->pDemod                = (sony_demod_t*)(((unsigned char*)pDriver) + sizeof(CXD2878_S_ISDBS3_DRIVER_DATA));
		pDriver->pBaseInterface        = (BASE_INTERFACE_MODULE*)(((unsigned char*)pDriver->pDemod) + sizeof(sony_demod_t));
		pDriver->pI2CBridge            = (I2C_BRIDGE_MODULE*)(((unsigned char*)pDriver->pBaseInterface) + sizeof(BASE_INTERFACE_MODULE));
		pDriver->DeviceAddr            = Addr;
		pDriver->CrystalFreqHz         = CrystalFreq;

		// Init Base IF
		pBaseIfIsdbs_2878 = pDriver->pBaseInterface;

		pBaseIfIsdbs_2878->I2cReadingByteNumMax      = RTK_DEMOD_BIF_RX_FIFO_DEPTH;
		pBaseIfIsdbs_2878->I2cWritingByteNumMax      = RTK_DEMOD_BIF_TX_FIFO_DEPTH;
		pBaseIfIsdbs_2878->I2cRead                   = __realtek_ext_i2c_read;
		pBaseIfIsdbs_2878->I2cWrite                  = __realtek_ext_i2c_write;
		pBaseIfIsdbs_2878->WaitMs                    = __realtek_ext_wait_ms;
		pBaseIfIsdbs_2878->SetUserDefinedDataPointer = ext_interface_SetUserDefinedDataPointer;
		pBaseIfIsdbs_2878->GetUserDefinedDataPointer = ext_interface_GetUserDefinedDataPointer;
		pBaseIfIsdbs_2878->SetUserDefinedDataPointer(pBaseIfIsdbs_2878, (void*)pComm);
		pBaseIfIsdbs_2878->useI2C = 0;


	}

	return pDriver;
}

sony_result_t CXD2878_i2c_Isdbs3Read(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode)
{
	/*
		Implement I2C functions
	*/

	int ret = FUNCTION_SUCCESS;
	unsigned char* pRegisterAddr = NULL;
        unsigned char RegisterAddrSize = '\0';

        ret = __realtek_ext_i2c_read(pBaseIfIsdbs_2878, deviceAddress, pRegisterAddr, RegisterAddrSize, pData, (U32BITS) size);

	SONY_TRACE_I2C_RETURN((ret == FUNCTION_SUCCESS) ? SONY_RESULT_OK : SONY_RESULT_ERROR_I2C);

}

sony_result_t CXD2878_i2c_Isdbs3Write(sony_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t *pData, uint32_t size, uint8_t mode)
{
	/*
		Implement I2C functions
	*/
	int ret = FUNCTION_SUCCESS;
	//int i;
    //
	//CXD2878_S_ISDBS3_INFO("Write : 0x%02X ", deviceAddress);
	//CXD2878_S_ISDBS3_INFO("0x%02X ", pData[0]);
	//for(i = 1; i < size; i++) CXD2878_S_ISDBS3_INFO("0x%02X ", pData[i]);
	//CXD2878_S_ISDBS3_INFO("\n");

	
	ret = __realtek_ext_i2c_write(pBaseIfIsdbs_2878, deviceAddress, pData, (U32BITS) size);

	SONY_TRACE_I2C_RETURN((ret == FUNCTION_SUCCESS) ? SONY_RESULT_OK : SONY_RESULT_ERROR_I2C);
}

