/***************************************************************************************************
  File        : demod_rtk_s_isdbt_cxd2856.c
  Description : CXD2856_S_ISDBT demod
  Author      : Kevin Wang
****************************************************************************************************
    Update List :
----------------------------------------------------------------------------------------------------
    20120207    | create phase
***************************************************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <kadp_OSAL.h>
#include "demodcomm/foundation.h"
#include "demod_ext_common.h"
#include "demod_rtk_s_isdbt_cxd2856.h"
#include "demod_rtk_s_isdbt_cxd2856_priv.h"
#include <rtk_kdriver/pcbMgr.h>

#include "CXD2856Family_refcode/sony_demod.h"
#include "CXD2856Family_refcode/isdb_terr/sony_isdbt.h"
#include "CXD2856Family_refcode/isdb_terr/sony_demod_isdbt.h"
#include "CXD2856Family_refcode/isdb_terr/sony_demod_isdbt_monitor.h"
#include "CXD2856Family_refcode/isdb_terr/sony_integ_isdbt.h"

static int isCreated = 0;
BASE_INTERFACE_MODULE      BaseInterface;
static sony_demod_t* pDemod = NULL;
static const char *Common_Result[13] = {"OK", "Argument Error", "I2C Error", "SW State Error", "HW State Error", "Timeout", "Unlock", "Out of Range", "No Support", "Cancelled", "Other Error", "Overflow", "OK - Confirm"};
static const char *Common_Bandwidth[11] = {"Unknown", "1.7MHz", "Invalid", "Invalid", "Invalid", "5MHz", "6MHz", "7MHz", "8MHz", "J.83B. 5.06/5.36Msps", "J.83B. 5.6Msps"};
extern unsigned char I2c_SpeedByPCB;
/*----------------------------------------------------------------------
 * Func : CXD2856_S_ISDBT
 *
 * Desc : constructor
 *
 * Parm : device_addr : device address
 *        output_mode : REALTEK_S_ISDBT_OUT_IF_SERIAL / REALTEK_S_ISDBT_OUT_IF_PARALLEL
 *        output_freq : REALTEK_S_ISDBT_OUT_FREQ_6M / REALTEK_S_ISDBT_OUT_FREQ_10M
 *        pComm       : hadle of COMM
 *
 * Retn : handle of demod
 *----------------------------------------------------------------------*/
void CXD2856_S_ISDBT_Constructors(DEMOD* pDemodKernel, unsigned char addr, unsigned char output_mode, unsigned char output_freq, COMM* pComm)
{
	CXD2856_S_ISDBT* pRealtekSISDBT;

	pDemodKernel->private_data = kmalloc(sizeof(CXD2856_S_ISDBT), GFP_KERNEL);
	if(pDemodKernel->private_data == NULL) {
		goto kmalloc_fail;
	}
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	ExtDemod_Constructors(pDemodKernel);

	pDemodKernel->m_id                 = DTV_DEMOD_SONY_ISDBT_2856;
	pDemodKernel->m_addr               = addr;
	pDemodKernel->m_pTuner             = NULL;
	pDemodKernel->m_pComm              = pComm;
	pDemodKernel->m_ScanRange          = SCAN_RANGE_3_6;
	pDemodKernel->m_ScanMode           = CH_UNKNOW;
	pDemodKernel->m_Capability         = TV_SYS_TYPE_ISDBT_CXD2856;
	pDemodKernel->m_update_interval_ms = 300;
	pDemodKernel->m_if.freq            = 36125000;
	pDemodKernel->m_if.inversion       = 1;
	pDemodKernel->m_if.agc_pola        = 0;
	pDemodKernel->m_clock              = CRYSTAL_FREQ_27000000HZ;
	pRealtekSISDBT->m_output_freq        = output_freq;
	pRealtekSISDBT->m_private            = (void*) AllocRealtekSIsdbt2856Driver(
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
	pRealtekSISDBT->m_low_snr_detect     = 0;
	pRealtekSISDBT->m_low_snr_recover    = 0;
	pRealtekSISDBT->m_auto_tune_enable   = 0;
	pDemodKernel->m_pComm->SetTargetName(pDemodKernel->m_pComm, pDemodKernel->m_addr, "CXD2856_S_ISDBT");
	pRealtekSISDBT->m_pTunerOptReg       = NULL;
	pRealtekSISDBT->m_TunerOptRegCnt     = 0;
	pRealtekSISDBT->m_acquire_sig_en = 0;
	pDemodKernel->Init_Ext = CXD2856_S_ISDBT_Init;
	pDemodKernel->Reset = ExtDemod_Reset;
	pDemodKernel->AcquireSignal = CXD2856_S_ISDBT_AcquireSignal;
	pDemodKernel->ScanSignal = CXD2856_S_ISDBT_ScanSignal;
	pDemodKernel->SetIF = CXD2856_S_ISDBT_SetIF;
	pDemodKernel->SetTSMode = CXD2856_S_ISDBT_SetTSMode;
	pDemodKernel->SetTVSysEx = CXD2856_S_ISDBT_SetTVSysEx;
	pDemodKernel->SetMode = CXD2856_S_ISDBT_SetMode;
	pDemodKernel->SetTVSys = CXD2856_S_ISDBT_SetTVSys;
	pDemodKernel->GetLockStatus = CXD2856_S_ISDBT_GetLockStatus;
	pDemodKernel->GetSignalInfo = CXD2856_S_ISDBT_GetSignalInfo;
	pDemodKernel->GetSignalQuality = CXD2856_S_ISDBT_GetSignalQuality;
	pDemodKernel->GetDemodStatus = ExtDemod_GetDemodStatus;
	pDemodKernel->GetCarrierOffset = CXD2856_S_ISDBT_GetCarrierOffset;
	pDemodKernel->ForwardI2C = CXD2856_S_ISDBT_ForwardI2C;
	pDemodKernel->AutoTune = CXD2856_S_ISDBT_AutoTune;
	pDemodKernel->KernelAutoTune = CXD2856_S_ISDBT_KernelAutoTune;
	pDemodKernel->GetCapability = ExtDemod_GetCapability;
	pDemodKernel->Suspend = ExtDemod_Suspend;
	pDemodKernel->Resume = ExtDemod_Resume;
	pDemodKernel->SetTvMode = CXD2856_S_ISDBT_SetTvMode;
	pDemodKernel->SetExtDemodInfo = CXD2856_S_ISDBT_SetExtDemodInfo;
	//pDemodKernel->DebugInfo = CXD2856_S_ISDBT_DebugInfo;	  	  // Add monitor function that used to monitor demod status
	pDemodKernel->InspectSignal = CXD2856_S_ISDBT_InspectSignal; // Add monitor function that used to monitor demod status
	pDemodKernel->AttachTuner = ExtDemod_AttachTuner;
	pDemodKernel->GetTSParam = ExtDemod_GetTSParam;
	pDemodKernel->Lock = CXD2856_S_ISDBT_Lock;
	pDemodKernel->Unlock = CXD2856_S_ISDBT_Unlock;
	pDemodKernel->Destory = CXD2856_S_ISDBT_Destructors;

kmalloc_fail:
	CXD2856_S_ISDBT_INFO("CXD2856_S_ISDBT_Constructors kmalloc Failed\n");
}


/*----------------------------------------------------------------------
 * Func : ~CXD2856_S_ISDBT
 *
 * Desc : Destructor of CXD2856_S_ISDBT
 *
 * Parm : pDemod : handle of Demod
 *
 * Retn : N/A
 *----------------------------------------------------------------------*/
void CXD2856_S_ISDBT_Destructors(DEMOD* pDemodKernel)
{
	CXD2856_S_ISDBT* pRealtekSISDBT;
	sony_result_t result = SONY_RESULT_OK;	
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	result = sony_demod_Sleep(pDemod);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBT_INFO("CXD2856_demod_Sleep fail (%d)\n", result);
	}

	pDemod = NULL;
	isCreated = 0;
	//if (pRealtekSISDBT->m_private != NULL)
	ReleaseRealtekSIsdbt2856Driver((CXD2856_S_ISDBT_DRIVER_DATA*)pRealtekSISDBT->m_private);

	kfree(pRealtekSISDBT);
	//pDemodKernel->Destory(pDemodKernel);
	CXD2856_S_ISDBT_INFO("CXD2856_S_ISDBT_Destructors Complete\n");
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
int CXD2856_S_ISDBT_Init(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode)
{
	CXD2856_S_ISDBT* pRealtekSISDBT;
	sony_demod_create_param_t createParam;
	static sony_i2c_t DemodI2c;
	sony_result_t result = SONY_RESULT_OK;

	//unsigned char dem_slv[4] = {0x00, 0xda, 0xc8, 0xca};
	unsigned char dem_slv[2] = {0xda, 0xc8};
	unsigned long long param = 0;

	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	createParam.xtalFreq       = SONY_DEMOD_XTAL_24000KHz;/* 24MHz Xtal */
	createParam.tunerI2cConfig = SONY_DEMOD_TUNER_I2C_CONFIG_DISABLE;/* I2C repeater is used */

	pDemodKernel->Lock(pDemodKernel);
	pRealtekSISDBT->m_auto_tune_enable = 0;

	if (!isCreated) {
		DemodI2c.Read             = sony_i2c_dummyRead;
		DemodI2c.Write            = sony_i2c_dummyWrite;

		DemodI2c.ReadRegister     = sony_i2c_CommonReadRegister;
		DemodI2c.WriteRegister    = sony_i2c_CommonWriteRegister;
		DemodI2c.WriteOneRegister = sony_i2c_CommonWriteOneRegister;

		if(mode == TV_SYS_TYPE_ISDBT_CXD2856)
			createParam.i2cAddressSLVT = dem_slv[0]; //demod1 i2c is 0xda
		else if(mode == TV_SYS_TYPE_ISDBT_CXD2856_2nd)
			createParam.i2cAddressSLVT = dem_slv[1];
		else
			createParam.i2cAddressSLVT = 0; // do not select the correct mode
		
		result = sony_demod_Create(pDemod, &createParam, &DemodI2c);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBT_WARNING("sony_demod_Create 1st error (%d)\n", result);
			goto init_fail;
		}
		result = sony_demod_Initialize(pDemod);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBT_WARNING("sony_demod_Initialize 1st error (%d)\n", result);
			goto init_fail;
		}
/*
		createParam.i2cAddressSLVT =  dem_slv[1]; //demod2 I2C slave address is 0xC8
		result = sony_demod_Create(pDemod, &createParam, &DemodI2c);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBT_WARNING("sony_demod_Create 2nd error (%d)\n", result);
			return TUNER_CTRL_FAIL;
		}
		result = sony_demod_Initialize(pDemod);
		if (result != SONY_RESULT_OK) {
			CXD2856_S_ISDBT_WARNING("sony_demod_Initialize 2nd error (%d)\n", result);
			goto init_fail;
		}
*/
		isCreated = 1;
		if (pcb_mgr_get_enum_info_byname("EXTDEMOD_I2C_SPEED", &param) != 0) {
			CXD2856_S_ISDBT_WARNING("CXD2856_ISDBT[%d]: Get EXTDEMOD_I2C_SPEED fail use default !!!!!!!!!!!!!!!\n", __LINE__);
			param = 0x0;
		}
		I2c_SpeedByPCB = (unsigned char)param;
		CXD2856_S_ISDBT_INFO("CXD2856_ISDBT[%d]: set EXTDEMOD_I2C speed = %d\n", __LINE__, param);
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
	CXD2856_S_ISDBT_INFO("CXD2856_S_ISDBT_Init Complete\n");
	return TUNER_CTRL_OK;

init_fail:
	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBT_WARNING("CXD2856_S_ISDBT_Init Fail\n");
	//pDemodKernel->Unlock(pDemodKernel);

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
int CXD2856_S_ISDBT_SetTVSys(DEMOD* pDemodKernel, TV_SYSTEM sys)
{
	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);

	CXD2856_S_ISDBT_INFO("\033[1;32;31m" "%s %s %d, system = %d\n" "\033[m", __FILE__, __func__, __LINE__, sys);
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
int CXD2856_S_ISDBT_SetTVSysEx(DEMOD* pDemodKernel, TV_SYSTEM sys, const TV_SIG_INFO*  pSigInfo)
{
	TUNER_PARAM Param;
	CXD2856_S_ISDBT* pRealtekSISDBT;
	unsigned char i2cAddr = 0;

	sony_result_t result = SONY_RESULT_OK;
	sony_isdbt_tune_param_t ISDBTparam;

	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	//sys = TV_SYS_ISDBT_6M;
	if (!IsExtIsdbSys(sys)) {
		CXD2856_S_ISDBT_WARNING("Set TV system failed, unsupported TV system\n");
		return  TUNER_CTRL_FAIL;
	}

	CXD2856_S_ISDBT_INFO("pSigInfo->isdbt.portIdx=%u pSigInfo->isdbs_ext.portIdx=%u  pSigInfo->isdbs3_ext.portIdx=%u\n", pSigInfo->isdbt.portIdx, pSigInfo->isdbs_ext.portIdx, pSigInfo->isdbs3_ext.portIdx);
	switch (pSigInfo->isdbt.portIdx) {
	case I2C_PORT1:
		i2cAddr = 0xda;
		CXD2856_S_ISDBT_INFO("Use demod1 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT2:
		i2cAddr = 0xc8;
		CXD2856_S_ISDBT_INFO("Use demod2 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT3:
		i2cAddr = 0xca;
		CXD2856_S_ISDBT_INFO("Use demod3 i2cAddr = 0x%x\n", i2cAddr);
		break;
	default:
		CXD2856_S_ISDBT_WARNING("unknown Demod port index !!!\n");
		goto SetDemod_fail;
	}
	pDemod->i2cAddressSLVT = i2cAddr;
	pDemod->i2cAddressSLVX = i2cAddr + 4;
    
	pRealtekSISDBT->m_auto_tune_enable = 0;

	ISDBTparam.bandwidth = SONY_DTV_BW_6_MHZ;


	CXD2856_S_ISDBT_INFO("Tune to ISDB-T signal with the following parameters:\n");
	CXD2856_S_ISDBT_INFO(" - Bandwidth            : %s\n", Common_Bandwidth[ISDBTparam.bandwidth]);
	CXD2856_S_ISDBT_INFO("\033[1;32;31m" "%s %s %d, system = %d pDemod->i2cAddressSLVT=0x%x\n" "\033[m", __FILE__, __func__, __LINE__, sys, pDemod->i2cAddressSLVT);

	//TUNER_PARAM Param;
	if (pDemodKernel->m_pTuner->GetParam(pDemodKernel->m_pTuner, sys, &Param) == TUNER_CTRL_OK) {
		if ((pDemodKernel->m_if.freq != Param.if_freq) || (pDemodKernel->m_if.inversion != Param.if_inversion) || (pDemodKernel->m_if.agc_pola  != Param.if_agc_pola)) {
			pDemodKernel->m_if.freq = Param.if_freq;
			pDemodKernel->m_if.inversion = Param.if_inversion;
			pDemodKernel->m_if.agc_pola = Param.if_agc_pola;
			if (pDemodKernel->SetIF(pDemodKernel, &pDemodKernel->m_if) != TUNER_CTRL_OK)  return  TUNER_CTRL_FAIL;

		}
	}
	CXD2856_S_ISDBT_INFO("m_if.freq = "PT_S32BITS", m_if.inversion = %d, m_if.agc_pola = %d\n", pDemodKernel->m_if.freq, pDemodKernel->m_if.inversion, pDemodKernel->m_if.agc_pola);
	
	pDemodKernel->Lock(pDemodKernel);

	result = sony_demod_Initialize(pDemod);

	if (result != SONY_RESULT_OK) {
		printk("sony_demod_Initialize error (%d)\n", result);
		goto SetDemod_fail;
	}

/*
	result = sony_demod_I2cRepeaterEnable(pDemod, 1);
	if (result != SONY_RESULT_OK) {
		printk("sony_demod_I2cRepeaterEnable error (%d)\n", result);
		goto SetDemod_fail;
	}
*/
	/*
	* In default, The setting is optimized for Sony silicon tuners.
	* If non-Sony tuner is used, the user should call following to
	* disable Sony silicon tuner optimized setting.
	*/
#if 0
	result = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_TUNER_OPTIMIZE, SONY_DEMOD_TUNER_OPTIMIZE_NONSONY);
	if (result != SONY_RESULT_OK) {
		CXD2856_S_ISDBT_INFO ("sony_demod_SetConfig (SONY_DEMOD_CONFIG_TUNER_OPTIMIZE) failed. (%s)\n", Common_Result[result]);
		return result;
	}
#endif
/*
	result = sony_demod_SetTSClockModeAndFreq(pDemod, SONY_DTV_SYSTEM_ISDBT);
	if (result != SONY_RESULT_OK) {
		//printf("sony_demod_Shutdown error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_OUTPUT_ATSC3, SONY_DEMOD_OUTPUT_ATSC3_ALP);
	if (result != SONY_RESULT_OK) {
		//printf("sony_demod_Shutdown error (%d)\n", result);
		goto SetDemod_fail;
	}
*/

	result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_PARALLEL_SEL, 0);
	if (result != SONY_RESULT_OK) {
		printk("SONY_DEMOD_CONFIG_PARALLEL_SEL error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
	if (result != SONY_RESULT_OK) {
		printk("SONY_DEMOD_CONFIG_ALP_SER_DATA_ON_MSB error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_OUTPUT_SEL_MSB, 1);
	if (result != SONY_RESULT_OK) {
		printk("SONY_DEMOD_CONFIG_OUTPUT_SEL_MSB error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_LATCH_ON_POSEDGE, 1);
	if (result != SONY_RESULT_OK) {
		printk("SONY_DEMOD_CONFIG_LATCH_ON_POSEDGE error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_TSCLK_CONT, 0);
	if (result != SONY_RESULT_OK) {
		printk("SONY_DEMOD_CONFIG_TSCLK_CONT error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_isdbt_Tune(pDemod, &ISDBTparam);

	if (result != SONY_RESULT_OK) {
		printk("sony_demod_isdbt_Tune error (%d)\n", result);
		goto SetDemod_fail;
	}

	result = sony_demod_TuneEnd(pDemod);

	if (result != SONY_RESULT_OK) {
		printk("sony_demod_TuneEnd error (%d)\n", result);
		goto SetDemod_fail;
	}


	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBT_WARNING("CXD2856_S_ISDBT_SetTVSysEx done\n");
	pRealtekSISDBT->m_acquire_sig_en = 1;
	pRealtekSISDBT->m_status_checking_stime = tv_osal_time_ext() + ISDBT_STATUS_CHECK_INTERVAL_MS;
	pRealtekSISDBT->m_auto_tune_enable = 1;

	return TUNER_CTRL_OK;

SetDemod_fail:
	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBT_WARNING("CXD2856_S_ISDBT_SetTVSysEx Fail\n");
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
int CXD2856_S_ISDBT_SetIF(DEMOD* pDemodKernel, IF_PARAM* pParam)
{
	int ret = TUNER_CTRL_OK;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	sony_demod_iffreq_config_t iffreqConfig={0};
	unsigned char if_freqMHz = 0;

	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);
	
	if_freqMHz = (unsigned char)(pParam->freq)/1000000;

	iffreqConfig.configISDBT_6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(3.55);
	iffreqConfig.configISDBT_7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(4.15);
	iffreqConfig.configISDBT_8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG(4.75);

	ret = sony_demod_SetIFFreqConfig (pDemod, &iffreqConfig);

	if (ret != SONY_RESULT_OK) {
		CXD2856_S_ISDBT_WARNING("sony_demod_SetIFFreqConfig failed. (%s)\n", Common_Result[ret]);
		return ret;
	}
	
	/*     
	* Terrestrial side ¡V Forward AGC (Negative) is default
	* If non-Sony tuner is used, the user should do this setting depend on the IFAGC sense of the tuner.
	* Terrestrial side AGC sense setting 
	* Reverse AGC(Positive): 0
	* Forward AGC(Negative): 1
	*/   

	ret = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_IFAGCNEG, pParam->agc_pola);

	if (ret != SONY_RESULT_OK) {
		CXD2856_S_ISDBT_WARNING ("sony_demod_SetConfig (SONY_DEMOD_CONFIG_IFAGCNEG) failed. (%s)\n", Common_Result[ret]);
		return ret;
	}
	/*     
	* generally the IF spectrum sense is inverted from the RF input signal
	* But some tuners output an IF without spectrum inversion
	* Note that even if this setting is not correct, the tune APIs will not be failed because
	* demodulator hardware automatically handles both inverted and not inverted spectrum.
	* However, the sign of the carrier offset monitor value will become opposite.
	* Spectrum sense setting. (IF spectrum sense is same as input signal.)=> 1
	* (IF spectrum sense is invert as input signal.)=> 0
	* However, the sign of the carrier offset monitor value will become opposite.
	*/  

	ret = sony_demod_SetConfig (pDemod, SONY_DEMOD_CONFIG_SPECTRUM_INV, pParam->inversion);

	if (ret != SONY_RESULT_OK) {
		CXD2856_S_ISDBT_WARNING ("sony_demod_SetConfig (SONY_DEMOD_CONFIG_SPECTRUM_INV) failed. (%s)\n", Common_Result[ret]);
		return ret;
	}
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
int CXD2856_S_ISDBT_SetMode(DEMOD* pDemodKernel, TV_OP_MODE mode)
{
	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

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
		CXD2856_S_ISDBT_WARNING("unknown TV_OP_MODE\n");
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
int CXD2856_S_ISDBT_SetTSMode(DEMOD* pDemodKernel, TS_PARAM* pParam)
{
	int ret = TUNER_CTRL_OK;
	unsigned int on_off = -1;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	if(pParam->portIdx != 0x0)
	{
		pDemod->i2cAddressSLVT = pParam->portIdx;
		pDemod->i2cAddressSLVX = pParam->portIdx + 4;
	}

	pDemodKernel->Lock(pDemodKernel);
	if(pParam->tsOutEn == 1) {
		ret |= sony_demod_Switch_TS_Output(pDemod, 1);
		if (ret != SONY_RESULT_OK)
			CXD2856_S_ISDBT_INFO("sony_demod_SetStreamOutput error (%d)\n", ret);
	}
	else {
		ret |= sony_demod_Switch_TS_Output(pDemod, 0);
		if (ret != SONY_RESULT_OK)
			CXD2856_S_ISDBT_INFO("sony_demod_SetStreamOutput error (%d)\n", ret);
	}

	ret |= sony_demod_Get_TS_Output(pDemod, &on_off);
	CXD2856_S_ISDBT_INFO("Get TSTLV Output = %d\n", on_off);
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
int CXD2856_S_ISDBT_GetLockStatus(DEMOD* pDemodKernel, unsigned char* pLock)
{

	sony_result_t result = SONY_RESULT_OK;
	unsigned char DemodLock[3] = {0}, TsLock[3] = {0}, EarlyUnlock[3] = {0};
	unsigned char i = 0;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;


	pDemodKernel->Lock(pDemodKernel);
	result = sony_demod_isdbt_monitor_SyncStat(pDemod, &DemodLock[0], &TsLock[0], &EarlyUnlock[0]);
	tv_osal_msleep_ext(5);
	result = sony_demod_isdbt_monitor_SyncStat(pDemod, &DemodLock[1], &TsLock[1], &EarlyUnlock[1]);
	tv_osal_msleep_ext(5);
	result = sony_demod_isdbt_monitor_SyncStat(pDemod, &DemodLock[2], &TsLock[2], &EarlyUnlock[2]);
	pDemodKernel->Unlock(pDemodKernel);

	for(i = 0; i < 3 ; i++) {
		CXD2856_S_ISDBT_INFO("DemodLock[i] = %u, TsLock[i] = %u, EarlyUnLock[i] = %u\n", DemodLock[i], TsLock[i], EarlyUnlock[i]);
	}
	*pLock = DemodLock[0] | DemodLock[1] | DemodLock[2] ;

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
int CXD2856_S_ISDBT_GetSignalInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pInfo)
{
	sony_result_t result = SONY_RESULT_OK;
	CXD2856_S_ISDBT*	pRealtekSISDBT;
	sony_isdbt_mode_t mode;
	sony_isdbt_guard_t guard;
	sony_isdbt_tmcc_info_t tmccInfo;
	
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	//need implement
	pDemodKernel->Lock(pDemodKernel);
	result = sony_demod_isdbt_monitor_ModeGuard (pDemod, &mode, &guard);

	if (result == SONY_RESULT_OK) {
		switch ((unsigned char)mode) {
		case SONY_ISDBT_MODE_1 :
			pInfo->isdbt.trans_mode = OFDM_FFT_2K;
			break;
		case SONY_ISDBT_MODE_2 :
			pInfo->isdbt.trans_mode = OFDM_FFT_4K;
			break;
		case SONY_ISDBT_MODE_3 :
			pInfo->isdbt.trans_mode = OFDM_FFT_8K;
			break;
		}
		CXD2856_S_ISDBT_INFO("ISDBT_FFT_Mode = %d\n", pInfo->isdbt.trans_mode);

		switch ((unsigned char)guard) {
		case SONY_ISDBT_GUARD_1_32 :
			pInfo->isdbt.guard_interval = OFDM_GI_1_32;
			break;
		case SONY_ISDBT_GUARD_1_16 :
			pInfo->isdbt.guard_interval = OFDM_GI_1_16;
			break;
		case SONY_ISDBT_GUARD_1_8 :
			pInfo->isdbt.guard_interval = OFDM_GI_1_8;
			break;
		case SONY_ISDBT_GUARD_1_4 :
			pInfo->isdbt.guard_interval = OFDM_GI_1_4;
			break;
		}
		CXD2856_S_ISDBT_INFO("guard_interval = %d\n", pInfo->isdbt.guard_interval);
	}else {
		CXD2856_S_ISDBT_INFO (" Mode Guard              | Error           | %s\n", Common_Result[result]);
	}
	

	result |= sony_demod_isdbt_monitor_TMCCInfo (pDemod, &tmccInfo);

	if (result == SONY_RESULT_OK) {
		//Layer A
		switch ((unsigned short)tmccInfo.currentInfo.layerA.modulation) {
		case SONY_ISDBT_MODULATION_DQPSK :
			pInfo->isdbt.layer[0].constellation = OFDM_CONST_DQPSK;
			break;
		case SONY_ISDBT_MODULATION_QPSK :
			pInfo->isdbt.layer[0].constellation = OFDM_CONST_4;
			break;
		case SONY_ISDBT_MODULATION_16QAM :
			pInfo->isdbt.layer[0].constellation = OFDM_CONST_16;
			break;
		case SONY_ISDBT_MODULATION_64QAM :
			pInfo->isdbt.layer[0].constellation = OFDM_CONST_64;
			break;
		}
		switch ((unsigned char)tmccInfo.currentInfo.layerA.codingRate) {
		case SONY_ISDBT_CODING_RATE_1_2 :
			pInfo->isdbt.layer[0].code_rate = OFDM_CODE_RATE_1_2;
			break;
		case SONY_ISDBT_CODING_RATE_2_3 :
			pInfo->isdbt.layer[0].code_rate = OFDM_CODE_RATE_2_3;
			break;
		case SONY_ISDBT_CODING_RATE_3_4 :
			pInfo->isdbt.layer[0].code_rate = OFDM_CODE_RATE_3_4;
			break;
		case SONY_ISDBT_CODING_RATE_5_6 :
			pInfo->isdbt.layer[0].code_rate = OFDM_CODE_RATE_5_6;
			break;
		case SONY_ISDBT_CODING_RATE_7_8 :
			pInfo->isdbt.layer[0].code_rate = OFDM_CODE_RATE_7_8;
			break;
		}
		pInfo->isdbt.layer[0].segment_num = tmccInfo.currentInfo.layerA.segmentsNum;
		CXD2856_S_ISDBT_INFO("[Layer A]Segment %d, Constellation %d, Code Rate 0x%X\n",
						pInfo->isdbt.layer[0].segment_num,
						pInfo->isdbt.layer[0].constellation,
						pInfo->isdbt.layer[0].code_rate);

		//Layer B
		switch ((unsigned short)tmccInfo.currentInfo.layerB.modulation) {
		case SONY_ISDBT_MODULATION_DQPSK :
			pInfo->isdbt.layer[1].constellation = OFDM_CONST_DQPSK;
			break;
		case SONY_ISDBT_MODULATION_QPSK :
			pInfo->isdbt.layer[1].constellation = OFDM_CONST_4;
			break;
		case SONY_ISDBT_MODULATION_16QAM :
			pInfo->isdbt.layer[1].constellation = OFDM_CONST_16;
			break;
		case SONY_ISDBT_MODULATION_64QAM :
			pInfo->isdbt.layer[1].constellation = OFDM_CONST_64;
			break;
		}
		switch ((unsigned char)tmccInfo.currentInfo.layerB.codingRate) {
		case SONY_ISDBT_CODING_RATE_1_2 :
			pInfo->isdbt.layer[1].code_rate = OFDM_CODE_RATE_1_2;
			break;
		case SONY_ISDBT_CODING_RATE_2_3 :
			pInfo->isdbt.layer[1].code_rate = OFDM_CODE_RATE_2_3;
			break;
		case SONY_ISDBT_CODING_RATE_3_4 :
			pInfo->isdbt.layer[1].code_rate = OFDM_CODE_RATE_3_4;
			break;
		case SONY_ISDBT_CODING_RATE_5_6 :
			pInfo->isdbt.layer[1].code_rate = OFDM_CODE_RATE_5_6;
			break;
		case SONY_ISDBT_CODING_RATE_7_8 :
			pInfo->isdbt.layer[1].code_rate = OFDM_CODE_RATE_7_8;
			break;
		}
		pInfo->isdbt.layer[1].segment_num = tmccInfo.currentInfo.layerB.segmentsNum;
		CXD2856_S_ISDBT_INFO("[Layer B]Segment %d, Constellation %d, Code Rate 0x%X\n",
						pInfo->isdbt.layer[1].segment_num,
						pInfo->isdbt.layer[1].constellation,
						pInfo->isdbt.layer[1].code_rate);

		//Layer C
		switch ((unsigned short)tmccInfo.currentInfo.layerC.modulation) {
		case SONY_ISDBT_MODULATION_DQPSK :
			pInfo->isdbt.layer[2].constellation = OFDM_CONST_DQPSK;
			break;
		case SONY_ISDBT_MODULATION_QPSK :
			pInfo->isdbt.layer[2].constellation = OFDM_CONST_4;
			break;
		case SONY_ISDBT_MODULATION_16QAM :
			pInfo->isdbt.layer[2].constellation = OFDM_CONST_16;
			break;
		case SONY_ISDBT_MODULATION_64QAM :
			pInfo->isdbt.layer[2].constellation = OFDM_CONST_64;
			break;
		}
		switch ((unsigned char)tmccInfo.currentInfo.layerC.codingRate) {
		case SONY_ISDBT_CODING_RATE_1_2 :
			pInfo->isdbt.layer[2].code_rate = OFDM_CODE_RATE_1_2;
			break;
		case SONY_ISDBT_CODING_RATE_2_3 :
			pInfo->isdbt.layer[2].code_rate = OFDM_CODE_RATE_2_3;
			break;
		case SONY_ISDBT_CODING_RATE_3_4 :
			pInfo->isdbt.layer[2].code_rate = OFDM_CODE_RATE_3_4;
			break;
		case SONY_ISDBT_CODING_RATE_5_6 :
			pInfo->isdbt.layer[2].code_rate = OFDM_CODE_RATE_5_6;
			break;
		case SONY_ISDBT_CODING_RATE_7_8 :
			pInfo->isdbt.layer[2].code_rate = OFDM_CODE_RATE_7_8;
			break;
		}
		pInfo->isdbt.layer[2].segment_num = tmccInfo.currentInfo.layerC.segmentsNum;
		CXD2856_S_ISDBT_INFO("[Layer C]Segment %d, Constellation %d, Code Rate 0x%X\n",
						pInfo->isdbt.layer[2].segment_num,
						pInfo->isdbt.layer[2].constellation,
						pInfo->isdbt.layer[2].code_rate);
	}else {
		CXD2856_S_ISDBT_INFO(" TMCC Information        | Error           |  %s\n", Common_Result[result]);
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
int CXD2856_S_ISDBT_GetSignalQuality(DEMOD* pDemodKernel, ENUM_TV_QUAL id, TV_SIG_QUAL* pQuality)
{
	sony_result_t result = SONY_RESULT_OK;

	unsigned char DemodLock = 0, TsLock = 0, EarlyUnlock = 0;
	int SNR = 0, TxRSSIdBm = 0, offset = 0;
	unsigned int BerA = 0, BerB = 0, BerC = 0;
	unsigned int PerA = 0, PerB = 0, PerC = 0;
	unsigned int ifAGCOut = 0;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	if(pQuality->demod_i2cAddr != 0x0)
	{
		pDemod->i2cAddressSLVT = pQuality->demod_i2cAddr;
		pDemod->i2cAddressSLVX = pQuality->demod_i2cAddr + 4;
	}

	pDemodKernel->Lock(pDemodKernel);

	switch (id) {
	case TV_QUAL_LOCKSTATUS:
		result = sony_demod_isdbt_monitor_SyncStat(pDemod, &DemodLock, &TsLock, &EarlyUnlock);
		CXD2856_S_ISDBT_INFO("DemodLock = %u, TsLock = %u, EarlyUnLock = %u [i2c=0x%x]\n", DemodLock, TsLock, EarlyUnlock, pDemod->i2cAddressSLVT);
		pQuality->ExtIsdb_Status.lock[0] = DemodLock;
		pQuality->ExtIsdb_Status.lock[1] = TsLock;
		pQuality->ExtIsdb_Status.lock[2] = EarlyUnlock;
		break;

	case TV_QUAL_SNR:
		result = sony_demod_isdbt_monitor_SNR(pDemod, &SNR);//SNR(dB) * 1000 
		CXD2856_S_ISDBT_INFO (" SNR | %d x 10^-3 dB\n", SNR);
		pQuality->snr = SNR * 10000;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	case TV_QUAL_BEC:
	case TV_QUAL_BER:
		result = sony_demod_isdbt_monitor_PreRSBER (pDemod, &BerA, &BerB, &BerC);
		pQuality->ber = BerA;
		pQuality->layer_ber.ber_num = BerA;
		pQuality->layer_ber.ber_den = 1000000000;
		//CXD2856_S_ISDBT_INFO (" Pre-RS BER              | Layer A         | %u x 10^-7\n", berA);
		break;

	case TV_QUAL_PEC:
	case TV_QUAL_PER:
	case TV_QUAL_ISDB_PER:
		result = sony_demod_isdbt_monitor_PER (pDemod, &PerA, &PerB, &PerC);
		CXD2856_S_ISDBT_INFO(" PER  | LayerA %u LayerB %u LayerC %u x 10^-6\n", PerA, PerB, PerC);
		pQuality->ExtIsdb_Status.isdbtPER[0] = BerA;
		pQuality->ExtIsdb_Status.isdbtPER[1] = BerB;
		pQuality->ExtIsdb_Status.isdbtPER[2] = BerC;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;

	case TV_QUAL_SIGNAL_STRENGTH:
		TxRSSIdBm = (int)pQuality->rflevel;
		if(TxRSSIdBm >= 10)
			pQuality->strength = 100;
		else {
			pQuality->strength = TxRSSIdBm + 90;
		}
		break;
	case TV_QUAL_SIGNAL_QUALITY:
		result = sony_demod_isdbt_monitor_SNR(pDemod, &SNR);//SNR(dB) * 1000

		pQuality->quality = SNR * 2 / 1000;
		break;
	case TV_QUAL_AGC:
		result = sony_demod_isdbt_monitor_IFAGCOut (pDemod, &ifAGCOut);
		pQuality->agc = ifAGCOut;
		CXD2856_S_ISDBT_INFO(" IF AGC | %u\n", ifAGCOut);
		break;
	case TV_QUAL_CARRIER_OFFSET:
		result = sony_demod_isdbt_monitor_CarrierOffset(pDemod, &offset);
		pQuality->ExtIsdb_Status.freq_offset = offset;
		pQuality->ExtIsdb_Status.demod_ret = result;
		CXD2856_S_ISDBT_INFO(" Carrier Offset | %d\n", offset);
		break;
	case TV_QUAL_NOSIG :
		result = sony_demod_isdbt_monitor_SyncStat(pDemod, &DemodLock, &TsLock, &EarlyUnlock);
		pQuality->nosig = (DemodLock) ? 0 : 1;
		break;
	case TV_QUAL_PRERSBER:
		result = sony_demod_isdbt_monitor_PreRSBER (pDemod, &BerA, &BerB, &BerC);
		CXD2856_S_ISDBT_INFO(" Pre-RS BER  | Layer A %u Layer B %u Layer C %u x 10^-7\n", BerA, BerB, BerC);
		pQuality->ExtIsdb_Status.preRSBER[0] = BerA;
		pQuality->ExtIsdb_Status.preRSBER[1] = BerB;
		pQuality->ExtIsdb_Status.preRSBER[2] = BerC;
		pQuality->ExtIsdb_Status.demod_ret = result;
		break;
	default:
		CXD2856_S_ISDBT_WARNING("unknown ENUM_TV_QUAL id = %u\n", (unsigned int) id);
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
int CXD2856_S_ISDBT_GetCarrierOffset(DEMOD* pDemodKernel, S32BITS* pOffset)
{
	sony_result_t result = SONY_RESULT_OK;
	int offset;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;


	pDemodKernel->Lock(pDemodKernel);

	result = sony_demod_isdbt_monitor_CarrierOffset(pDemod, &offset);

	if (result == SONY_RESULT_OK) {
		/*-----------------------------------------------------------------
		  CrOffset = Current Frequency - Target Frequency
		  returned offset = Target Freq - Current Offset = -CrOffset
		  Note: REALTEK_R ATSC Demod will handle IF polarity automatically
		------------------------------------------------------------------*/
		*pOffset = offset;
	} else
		*pOffset = 0;

	pDemodKernel->Unlock(pDemodKernel);
	CXD2856_S_ISDBT_INFO("Carrier Offset = "PT_S32BITS" Hz\n", *pOffset);

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
int CXD2856_S_ISDBT_GetDebugLogBuf(DEMOD* pDemodKernel, unsigned int* LogBufAddr, unsigned int* LogBufSize)
{
	int ret = TUNER_CTRL_OK;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

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
int CXD2856_S_ISDBT_AcquireSignal(DEMOD* pDemodKernel, unsigned char WaitSignalLock)
{
#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	unsigned long cur_time;
#endif

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;


#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	cur_time = tv_osal_time_ext();

	while ((tv_osal_time_ext() - cur_time) <= 15) {
		if (pRealtekSISDBT->m_acquire_sig_en == 0)
			return TUNER_CTRL_OK;

		tv_osal_msleep_ext(5);
	}

	return TUNER_CTRL_FAIL;

#else
	//CXD2856_S_ISDBT_AcquireSignalThread(pDemodKernel);
	//pRealtekSISDBT->m_autotune_stime = tv_osal_time_ext() + AUTODETECT_SIG_UPDATE_INTERVAL_MS;
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
int CXD2856_S_ISDBT_AcquireSignalThread(DEMOD* pDemodKernel)
{
	unsigned long   stime;
	unsigned char DemodLock = 0, TsLock = 0, EarlyUnlock = 0;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	if (pRealtekSISDBT->m_acquire_sig_en == 0)
		return TUNER_CTRL_OK;


	CXD2856_S_ISDBT_INFO("\033[1;32;32m" "%s %s %d\n" "\033[m", __FILE__, __func__, __LINE__);


	pDemodKernel->Lock(pDemodKernel);
	pRealtekSISDBT->m_auto_tune_enable = 0;
	stime = tv_osal_time_ext();

	while ((tv_osal_time_ext() - stime) <= SONY_ISDBT_WAIT_DEMOD_LOCK) {
		sony_demod_isdbt_monitor_SyncStat(pDemod, &DemodLock, &TsLock, &EarlyUnlock);
		if ((DemodLock) || (EarlyUnlock == 1)) break;
		tv_osal_msleep_ext(5);
	}

	CXD2856_S_ISDBT_INFO("DemodLock = %u, TsLock = %u, EarlyUnLock = %u, Time = %lu\n", DemodLock, TsLock, EarlyUnlock, tv_osal_time_ext() - stime);

//acquire_signal_result:
	pDemodKernel->Unlock(pDemodKernel);
	pRealtekSISDBT->m_acquire_sig_en = 0;
	pRealtekSISDBT->m_status_checking_stime = tv_osal_time_ext() + ISDBT_STATUS_CHECK_INTERVAL_MS;
	pRealtekSISDBT->m_auto_tune_enable = 1;
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
int CXD2856_S_ISDBT_ScanSignal(DEMOD* pDemodKernel, S32BITS* pOffset, SCAN_RANGE range)
{
	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

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
int CXD2856_S_ISDBT_ForwardI2C(DEMOD* pDemodKernel, unsigned char on_off)
{
	int ret = TUNER_CTRL_OK;

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

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
int CXD2856_S_ISDBT_AutoTune(DEMOD* pDemodKernel)
{
	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);

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
int CXD2856_S_ISDBT_KernelAutoTune(DEMOD* pDemodKernel)
{
	int result = SONY_RESULT_OK;
#if 0
	unsigned long cur_time;
	CXD2856_S_ISDBT* pRealtekSISDBT;

#ifndef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	unsigned char ewsChange;
	unsigned char LockStatus = 0;
	TV_SIG_QUAL Quality;
#endif
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	if(pRealtekSISDBT->m_auto_tune_enable == 0)
		return result;

	cur_time = tv_osal_time_ext();

#ifndef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
	if ((cur_time > pRealtekSISDBT->m_status_checking_stime) || ((pRealtekSISDBT->m_status_checking_stime - cur_time) > ISDBT_STATUS_CHECK_INTERVAL_MS)) {
		result = sony_demod_isdbt_monitor_EWSChange (pDemod, &ewsChange);

		if (result == SONY_RESULT_OK) {
			CXD2856_S_ISDBT_INFO (" EWSChange               | EWS Change      | %d\n", ewsChange);
			if (ewsChange == 1) {
				sony_demod_isdbt_ClearEWSChange(pDemod);
			}
		}
		CXD2856_S_ISDBT_GetLockStatus(pDemodKernel, &LockStatus);
		if (LockStatus == 1) {
			CXD2856_S_ISDBT_GetSignalQuality(pDemodKernel, TV_QUAL_SNR, &Quality);
			CXD2856_S_ISDBT_GetSignalQuality(pDemodKernel, TV_QUAL_BER, &Quality);
			CXD2856_S_ISDBT_GetSignalQuality(pDemodKernel, TV_QUAL_PER, &Quality);
			CXD2856_S_ISDBT_GetSignalQuality(pDemodKernel, TV_QUAL_SIGNAL_STRENGTH, &Quality);
			CXD2856_S_ISDBT_GetSignalQuality(pDemodKernel, TV_QUAL_SIGNAL_QUALITY, &Quality);
			CXD2856_S_ISDBT_GetSignalQuality(pDemodKernel, TV_QUAL_AGC, &Quality);
		}
	}
#endif

	if (pRealtekSISDBT->m_acquire_sig_en) {
#ifdef CONFIG_RTK_KDRV_DEMOD_SCAN_THREAD_ENABLE
		CXD2856_S_ISDBT_AcquireSignalThread(pDemodKernel);
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
int CXD2856_S_ISDBT_InspectSignal(DEMOD* pDemodKernel)
{

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	return TUNER_CTRL_OK;
}


/*----------------------------------------------------------------------
 * Func : Activate REALTEK_R ISDBT Demod
 *
 * Desc : Run Auto Tuning for CCI
 *
 * Parm : N/A
 *
 * Retn : TUNER_CTRL_OK/TUNER_CTRL_FAIL
 *----------------------------------------------------------------------*/
int CXD2856_S_ISDBT_Activate(DEMOD* pDemodKernel, unsigned char force_rst)
{

	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	return TUNER_CTRL_OK;

}


int CXD2856_S_ISDBT_SetTvMode(DEMOD* pDemodKernel, TV_SYSTEM_TYPE mode)
{
	int ret = TUNER_CTRL_FAIL;
	CXD2856_S_ISDBT* pRealtekSISDBT;

	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemodKernel->Lock(pDemodKernel);

	CXD2856_S_ISDBT_INFO("CXD2856_S_ISDBT_SetTvMode(%d)\n", mode);

	switch (mode) {
	case TV_SYS_TYPE_ISDBT_CXD2856:
	case TV_SYS_TYPE_ISDBT_CXD2856_2nd:
		CXD2856_S_ISDBT_INFO("Set to ISDBT \n");
		ret = pDemodKernel->Init_Ext(pDemodKernel, mode);
		//ret |= pDemodKernel->SetTSMode(pDemodKernel, &pDemodKernel->m_ts);

		if (ret != TUNER_CTRL_OK) {
			CXD2856_S_ISDBT_WARNING("Set CXD2856_S_ISDBT to ISDBT mode failed\n");
			goto set_demod_mode_failed;
		}
		break;

	default:
		CXD2856_S_ISDBT_WARNING("unknown TV_SYSTEM_TYPE\n");
		goto set_demod_mode_failed;
	}

	ret = TUNER_CTRL_OK;
	CXD2856_S_ISDBT_INFO("Set CXD2856_S_ISDBT to appointed DTV mode successed\n");
	pDemodKernel->Unlock(pDemodKernel);
	return ret;

set_demod_mode_failed:
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;
}


int CXD2856_S_ISDBT_SetExtDemodInfo(DEMOD* pDemodKernel, TV_SIG_INFO* pSigInfo)
{
	unsigned char i2cAddr = 0;
	CXD2856_S_ISDBT* pRealtekSISDBT;
	pRealtekSISDBT = (CXD2856_S_ISDBT*)(pDemodKernel->private_data);
	pDemod = ((CXD2856_S_ISDBT_DRIVER_DATA*) pRealtekSISDBT->m_private)->pDemod;

	pDemodKernel->Lock(pDemodKernel);

	switch (pSigInfo->isdbt.portIdx) {
	case I2C_PORT1:
		i2cAddr = 0xda;
		CXD2856_S_ISDBT_INFO("Use demod1 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT2:
		i2cAddr = 0xc8;
		CXD2856_S_ISDBT_INFO("Use demod2 i2cAddr = 0x%x\n", i2cAddr);
		break;
	case I2C_PORT3:
		i2cAddr = 0xca;
		CXD2856_S_ISDBT_INFO("Use demod3 i2cAddr = 0x%x\n", i2cAddr);
		break;
	default:
		CXD2856_S_ISDBT_WARNING("unknown Demod port index !!!\n");
		goto update_demod_info_failed;
	}
	
	pDemod->i2cAddressSLVT = i2cAddr;
    pDemod->i2cAddressSLVX = i2cAddr + 4;
    //pDemod->i2cAddressSLVR = i2cAddr - 0x40;
    //pDemod->i2cAddressSLVM = i2cAddr - 0xA8;
	
	CXD2856_S_ISDBT_INFO("Update ISDBT CXD2856 successed\n");
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_OK;

update_demod_info_failed:
	pDemodKernel->Unlock(pDemodKernel);
	return TUNER_CTRL_FAIL;
}

/*-----------------------------------------------------------------
 *     BASE_INTERFACE for CXD2856_S_ISDBT API
 *----------------------------------------------------------------*/
void CXD2856_S_ISDBT_Lock(DEMOD* pDemodKernel)
{
	//FUNCTION_NAME_PRINT("LOCK 0x%x\n",&pDemodKernel->m_lock);
	//mutex_lock(&pDemodKernel->m_lock);
}


void CXD2856_S_ISDBT_Unlock(DEMOD* pDemodKernel)
{
	//FUNCTION_NAME_PRINT("UN_LOCK 0x%x\n",&pDemodKernel->m_lock);
	//mutex_unlock(&pDemodKernel->m_lock);
}


void ReleaseRealtekSIsdbt2856Driver(CXD2856_S_ISDBT_DRIVER_DATA *pDriver)
{
	kfree(pDriver);
}


CXD2856_S_ISDBT_DRIVER_DATA* AllocRealtekSIsdbt2856Driver(
	COMM*               pComm,
	unsigned char       Addr,
	U32BITS       CrystalFreq
	//,int TSMode
)
{
	CXD2856_S_ISDBT_DRIVER_DATA* pDriver = (CXD2856_S_ISDBT_DRIVER_DATA*) kmalloc(
				sizeof(CXD2856_S_ISDBT_DRIVER_DATA) +
				sizeof(sony_demod_t)   +
				sizeof(I2C_BRIDGE_MODULE)   +
				sizeof(BASE_INTERFACE_MODULE), GFP_KERNEL);

	if (pDriver) {
		BASE_INTERFACE_MODULE* pBIF;
		//sony_demod_t* pDemod;

		memset(pDriver, 0, sizeof(CXD2856_S_ISDBT_DRIVER_DATA));
		pDriver->pDemod                = (sony_demod_t*)(((unsigned char*)pDriver) + sizeof(CXD2856_S_ISDBT_DRIVER_DATA));
		pDriver->pBaseInterface        = (BASE_INTERFACE_MODULE*)(((unsigned char*)pDriver->pDemod) + sizeof(sony_demod_t));
		pDriver->pI2CBridge            = (I2C_BRIDGE_MODULE*)(((unsigned char*)pDriver->pBaseInterface) + sizeof(BASE_INTERFACE_MODULE));
		pDriver->DeviceAddr            = Addr;
		pDriver->CrystalFreqHz         = CrystalFreq;

		// Init Base IF
		pBIF = &BaseInterface;
		pBIF->I2cReadingByteNumMax      = RTK_DEMOD_BIF_RX_FIFO_DEPTH;
		pBIF->I2cWritingByteNumMax      = RTK_DEMOD_BIF_TX_FIFO_DEPTH;
		pBIF->I2cRead                   = __realtek_ext_i2c_read;
		pBIF->I2cWrite                  = __realtek_ext_i2c_write;
		pBIF->WaitMs                    = __realtek_ext_wait_ms;
		pBIF->SetUserDefinedDataPointer = ext_interface_SetUserDefinedDataPointer;
		pBIF->GetUserDefinedDataPointer = ext_interface_GetUserDefinedDataPointer;
		pBIF->SetUserDefinedDataPointer(pBIF, (void*)pComm);
		pBIF->useI2C = 0;


	}

	return pDriver;
}


sony_result_t sony_i2c_dummyRead(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode)
{
	/*
		Implement I2C functions
	*/

	int ret = FUNCTION_SUCCESS;
	unsigned char* pRegisterAddr = NULL;
	unsigned char RegisterAddrSize = '\0';

	ret = __realtek_ext_i2c_read(&BaseInterface, deviceAddress, pRegisterAddr, RegisterAddrSize, pData, (U32BITS) size);

	SONY_TRACE_I2C_RETURN((ret == FUNCTION_SUCCESS) ? SONY_RESULT_OK : SONY_RESULT_ERROR_I2C);

}

sony_result_t sony_i2c_dummyWrite(sony_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t *pData, uint32_t size, uint8_t mode)
{
	/*
		Implement I2C functions
	*/

	//int i;
	//printf("Write : 0x%02X ", deviceAddress);
	//printf("0x%02X ", pData[0]);
	//for(i = 1; i < size; i++) printf("0x%02X ", pData[i]);
	//printf("\n");

	int ret = FUNCTION_SUCCESS;
	ret = __realtek_ext_i2c_write(&BaseInterface, deviceAddress, pData, (U32BITS) size);

	SONY_TRACE_I2C_RETURN((ret == FUNCTION_SUCCESS) ? SONY_RESULT_OK : SONY_RESULT_ERROR_I2C);
}

