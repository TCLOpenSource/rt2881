#ifndef __DTMB_DEMOD_BASE_H
#define __DTMB_DEMOD_BASE_H


#ifdef __cplusplus
extern "C" {
#endif
#include "foundation.h"

// DTMB demod module pre-definition
typedef struct DTMB_DEMOD_MODULE_TAG DTMB_DEMOD_MODULE;

/**

@brief   DTMB demod type getting function pointer

One can use DTMB_DEMOD_FP_GET_DEMOD_TYPE() to get DTMB demod type.


@param [in]    pDemod       The demod module pointer
@param [out]   pDemodType   Pointer to an allocated memory for storing demod type


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_DEMOD_TYPE() with the corresponding function.


@see   MODULE_TYPE

*/
typedef void
(*DTMB_DEMOD_FP_GET_DEMOD_TYPE)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pDemodType
	);





/**

@brief   DTMB demod I2C device address getting function pointer

One can use DTMB_DEMOD_FP_GET_DEVICE_ADDR() to get DTMB demod I2C device address.


@param [in]    pDemod        The demod module pointer
@param [out]   pDeviceAddr   Pointer to an allocated memory for storing demod I2C device address


@retval   FUNCTION_SUCCESS   Get demod device address successfully.
@retval   FUNCTION_ERROR     Get demod device address unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_DEVICE_ADDR() with the corresponding function.

*/
typedef void
(*DTMB_DEMOD_FP_GET_DEVICE_ADDR)(
	DTMB_DEMOD_MODULE *pDemod,
	unsigned char *pDeviceAddr
	);





/**

@brief   DTMB demod crystal frequency getting function pointer

One can use DTMB_DEMOD_FP_GET_CRYSTAL_FREQ_HZ() to get DTMB demod crystal frequency in Hz.


@param [in]    pDemod           The demod module pointer
@param [out]   pCrystalFreqHz   Pointer to an allocated memory for storing demod crystal frequency in Hz


@retval   FUNCTION_SUCCESS   Get demod crystal frequency successfully.
@retval   FUNCTION_ERROR     Get demod crystal frequency unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_CRYSTAL_FREQ_HZ() with the corresponding function.

*/
typedef void
(*DTMB_DEMOD_FP_GET_CRYSTAL_FREQ_HZ)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pCrystalFreqHz
	);





/**

@brief   DTMB demod software resetting function pointer

One can use DTMB_DEMOD_FP_SOFTWARE_RESET() to reset DTMB demod by software reset.


@param [in]   pDemod   The demod module pointer


@retval   FUNCTION_SUCCESS   Reset demod by software reset successfully.
@retval   FUNCTION_ERROR     Reset demod by software reset unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_SOFTWARE_RESET() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_SOFTWARE_RESET)(
	DTMB_DEMOD_MODULE *pDemod
	);





/**

@brief   DTMB demod initializing function pointer

One can use DTMB_DEMOD_FP_INITIALIZE() to initialie DTMB demod.


@param [in]   pDemod   The demod module pointer


@retval   FUNCTION_SUCCESS   Initialize demod successfully.
@retval   FUNCTION_ERROR     Initialize demod unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_INITIALIZE() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_INITIALIZE)(
	DTMB_DEMOD_MODULE *pDemod
	);


typedef int
(*DTMB_DEMOD_FP_DEINITIALIZE)(
	DTMB_DEMOD_MODULE *pDemod
	);



/**

@brief   DTMB demod IF frequency setting function pointer

One can use DTMB_DEMOD_FP_SET_IF_FREQ_HZ() to set DTMB demod IF frequency in Hz.


@param [in]   pDemod     The demod module pointer
@param [in]   IfFreqHz   IF frequency in Hz for setting


@retval   FUNCTION_SUCCESS   Set demod IF frequency successfully.
@retval   FUNCTION_ERROR     Set demod IF frequency unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_SET_IF_FREQ_HZ() with the corresponding function.


@see   IF_FREQ_HZ

*/
typedef int
(*DTMB_DEMOD_FP_SET_IF_FREQ_HZ)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS IfFreqHz
	);





/**

@brief   DTMB demod spectrum mode setting function pointer

One can use DTMB_DEMOD_FP_SET_SPECTRUM_MODE() to set DTMB demod spectrum mode.


@param [in]   pDemod         The demod module pointer
@param [in]   SpectrumMode   Spectrum mode for setting


@retval   FUNCTION_SUCCESS   Set demod spectrum mode successfully.
@retval   FUNCTION_ERROR     Set demod spectrum mode unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_SET_SPECTRUM_MODE() with the corresponding function.


@see   SPECTRUM_MODE

*/
typedef int
(*DTMB_DEMOD_FP_SET_SPECTRUM_MODE)(
	DTMB_DEMOD_MODULE *pDemod,
	int SpectrumMode
	);





/**

@brief   DTMB demod IF frequency getting function pointer

One can use DTMB_DEMOD_FP_GET_IF_FREQ_HZ() to get DTMB demod IF frequency in Hz.


@param [in]    pDemod      The demod module pointer
@param [out]   pIfFreqHz   Pointer to an allocated memory for storing demod IF frequency in Hz


@retval   FUNCTION_SUCCESS   Get demod IF frequency successfully.
@retval   FUNCTION_ERROR     Get demod IF frequency unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_IF_FREQ_HZ() with the corresponding function.


@see   IF_FREQ_HZ

*/
typedef int
(*DTMB_DEMOD_FP_GET_IF_FREQ_HZ)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pIfFreqHz
	);





/**

@brief   DTMB demod spectrum mode getting function pointer

One can use DTMB_DEMOD_FP_GET_SPECTRUM_MODE() to get DTMB demod spectrum mode.


@param [in]    pDemod          The demod module pointer
@param [out]   pSpectrumMode   Pointer to an allocated memory for storing demod spectrum mode


@retval   FUNCTION_SUCCESS   Get demod spectrum mode successfully.
@retval   FUNCTION_ERROR     Get demod spectrum mode unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_SPECTRUM_MODE() with the corresponding function.


@see   SPECTRUM_MODE

*/
typedef int
(*DTMB_DEMOD_FP_GET_SPECTRUM_MODE)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pSpectrumMode
	);





/**

@brief   DTMB demod signal lock asking function pointer

One can use DTMB_DEMOD_FP_IS_SIGNAL_LOCKED() to ask DTMB demod if it is signal-locked.


@param [in]    pDemod    The demod module pointer
@param [out]   pAnswer   Pointer to an allocated memory for storing answer


@retval   FUNCTION_SUCCESS   Perform signal lock asking to demod successfully.
@retval   FUNCTION_ERROR     Perform signal lock asking to demod unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_IS_SIGNAL_LOCKED() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_IS_SIGNAL_LOCKED)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pAnswer
	);





/**

@brief   DTMB demod signal strength getting function pointer

One can use DTMB_DEMOD_FP_GET_SIGNAL_STRENGTH() to get signal strength.


@param [in]    pDemod            The demod module pointer
@param [out]   pSignalStrength   Pointer to an allocated memory for storing signal strength (value = 0 ~ 100)


@retval   FUNCTION_SUCCESS   Get demod signal strength successfully.
@retval   FUNCTION_ERROR     Get demod signal strength unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_SIGNAL_STRENGTH() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_GET_SIGNAL_STRENGTH)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pSignalStrength
	);





/**

@brief   DTMB demod signal quality getting function pointer

One can use DTMB_DEMOD_FP_GET_SIGNAL_QUALITY() to get signal quality.


@param [in]    pDemod           The demod module pointer
@param [out]   pSignalQuality   Pointer to an allocated memory for storing signal quality (value = 0 ~ 100)


@retval   FUNCTION_SUCCESS   Get demod signal quality successfully.
@retval   FUNCTION_ERROR     Get demod signal quality unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_SIGNAL_QUALITY() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_GET_SIGNAL_QUALITY)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pSignalQuality
	);

typedef int
(*DTMB_DEMOD_FP_GET_DEMOD_STATUS)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pDemodStatus
	);


typedef int
(*DTMB_DEMOD_FP_GET_WRAPPER_STATUS)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pWrapperStatus
	);


/**

@brief   DTMB demod BER getting function pointer

One can use DTMB_DEMOD_FP_GET_BER() to get BER.


@param [in]    pDemod          The demod module pointer
@param [out]   pBerNum         Pointer to an allocated memory for storing BER numerator
@param [out]   pBerDen         Pointer to an allocated memory for storing BER denominator


@retval   FUNCTION_SUCCESS   Get demod error rate value successfully.
@retval   FUNCTION_ERROR     Get demod error rate value unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_BER() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_GET_BER)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pBerNum,
	U32BITS *pBerDen
	);



/**

@brief   DTMB demod ITER getting function pointer

One can use DTMB_DEMOD_FP_GET_ITER() to get BER.


@param [in]    pDemod          The demod module pointer
@param [out]   pIterNum         Pointer to an allocated memory for storing Iter numerator


@retval   FUNCTION_SUCCESS   Get demod error rate value successfully.
@retval   FUNCTION_ERROR     Get demod error rate value unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_ITERR() with the corresponding function.

*/

typedef int
(*DTMB_DEMOD_FP_GET_ITER)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pIterNum
	);




/**

@brief   DTMB demod PER getting function pointer

One can use DTMB_DEMOD_FP_GET_PER() to get PER.


@param [in]    pDemod          The demod module pointer
@param [out]   pPerNum         Pointer to an allocated memory for storing PER numerator
@param [out]   pPerDen         Pointer to an allocated memory for storing PER denominator


@retval   FUNCTION_SUCCESS   Get demod error rate value successfully.
@retval   FUNCTION_ERROR     Get demod error rate value unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_PER() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_GET_PER)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pPerNum,
	U32BITS *pPerDen
	);





/**

@brief   DTMB demod SNR getting function pointer

One can use DTMB_DEMOD_FP_GET_SNR_DB() to get SNR in dB.


@param [in]    pDemod      The demod module pointer
@param [out]   pSnrDbNum   Pointer to an allocated memory for storing SNR dB numerator
@param [out]   pSnrDbDen   Pointer to an allocated memory for storing SNR dB denominator


@retval   FUNCTION_SUCCESS   Get demod SNR successfully.
@retval   FUNCTION_ERROR     Get demod SNR unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_SNR_DB() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_GET_SNR_DB)(
	DTMB_DEMOD_MODULE *pDemod,
	S32BITS *pSnrDbNum,
	S32BITS *pSnrDbDen
	);





/**

@brief   DTMB demod RF AGC getting function pointer

One can use DTMB_DEMOD_FP_GET_RF_AGC() to get DTMB demod RF AGC value.


@param [in]    pDemod   The demod module pointer
@param [out]   pRfAgc   Pointer to an allocated memory for storing RF AGC value


@retval   FUNCTION_SUCCESS   Get demod RF AGC value successfully.
@retval   FUNCTION_ERROR     Get demod RF AGC value unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_RF_AGC() with the corresponding function.
	-# The range of RF AGC value is 0 ~ (pow(2, 14) - 1).

*/
typedef int
(*DTMB_DEMOD_FP_GET_RF_AGC)(
	DTMB_DEMOD_MODULE *pDemod,
	S32BITS *pRfAgc
	);





/**

@brief   DTMB demod IF AGC getting function pointer

One can use DTMB_DEMOD_FP_GET_IF_AGC() to get DTMB demod IF AGC value.


@param [in]    pDemod   The demod module pointer
@param [out]   pIfAgc   Pointer to an allocated memory for storing IF AGC value


@retval   FUNCTION_SUCCESS   Get demod IF AGC value successfully.
@retval   FUNCTION_ERROR     Get demod IF AGC value unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_IF_AGC() with the corresponding function.
	-# The range of IF AGC value is 0 ~ (pow(2, 14) - 1).

*/
typedef int
(*DTMB_DEMOD_FP_GET_IF_AGC)(
	DTMB_DEMOD_MODULE *pDemod,
	S32BITS *pIfAgc
	);





/**

@brief   DTMB demod digital AGC getting function pointer

One can use DTMB_DEMOD_FP_GET_DI_AGC() to get DTMB demod digital AGC value.


@param [in]    pDemod   The demod module pointer
@param [out]   pDiAgc   Pointer to an allocated memory for storing digital AGC value


@retval   FUNCTION_SUCCESS   Get demod digital AGC value successfully.
@retval   FUNCTION_ERROR     Get demod digital AGC value unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_DI_AGC() with the corresponding function.
	-# The range of digital AGC value is 0 ~ (pow(2, 12) - 1).

*/
typedef int
(*DTMB_DEMOD_FP_GET_DI_AGC)(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pDiAgc
	);





/**

@brief   DTMB demod TR offset getting function pointer

One can use DTMB_DEMOD_FP_GET_TR_OFFSET_PPM() to get TR offset in ppm.


@param [in]    pDemod         The demod module pointer
@param [out]   pTrOffsetPpm   Pointer to an allocated memory for storing TR offset in ppm


@retval   FUNCTION_SUCCESS   Get demod TR offset successfully.
@retval   FUNCTION_ERROR     Get demod TR offset unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_TR_OFFSET_PPM() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_GET_TR_OFFSET_PPM)(
	DTMB_DEMOD_MODULE *pDemod,
	S32BITS *pTrOffsetPpm
	);





/**

@brief   DTMB demod CR offset getting function pointer

One can use DTMB_DEMOD_FP_GET_CR_OFFSET_HZ() to get CR offset in Hz.


@param [in]    pDemod        The demod module pointer
@param [out]   pCrOffsetHz   Pointer to an allocated memory for storing CR offset in Hz


@retval   FUNCTION_SUCCESS   Get demod CR offset successfully.
@retval   FUNCTION_ERROR     Get demod CR offset unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_CR_OFFSET_HZ() with the corresponding function.

*/
typedef int
(*DTMB_DEMOD_FP_GET_CR_OFFSET_HZ)(
	DTMB_DEMOD_MODULE *pDemod,
	S32BITS *pCrOffsetHz
	);





/**

@brief   DTMB demod carrier mode getting function pointer

One can use DTMB_DEMOD_FP_GET_CARRIER_MODE() to get DTMB demod carrier mode.


@param [in]    pDemod         The demod module pointer
@param [out]   pCarrierMode   Pointer to an allocated memory for storing demod carrier mode


@retval   FUNCTION_SUCCESS   Get demod carrier mode successfully.
@retval   FUNCTION_ERROR     Get demod carrier mode unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_CARRIER_MODE() with the corresponding function.


@see   DTMB_CARRIER_MODE

*/
typedef int
(*DTMB_DEMOD_FP_GET_CARRIER_MODE)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pCarrierMode
	);





/**

@brief   DTMB demod PN mode getting function pointer

One can use DTMB_DEMOD_FP_GET_PN_MODE() to get DTMB demod PN mode.


@param [in]    pDemod    The demod module pointer
@param [out]   pPnMode   Pointer to an allocated memory for storing demod PN mode


@retval   FUNCTION_SUCCESS   Get demod PN mode successfully.
@retval   FUNCTION_ERROR     Get demod PN mode unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_PN_MODE() with the corresponding function.


@see   DTMB_PN_MODE

*/
typedef int
(*DTMB_DEMOD_FP_GET_PN_MODE)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pPnMode
	);





/**

@brief   DTMB demod QAM mode getting function pointer

One can use DTMB_DEMOD_FP_GET_QAM_MODE() to get DTMB demod QAM mode.


@param [in]    pDemod     The demod module pointer
@param [out]   pQamMode   Pointer to an allocated memory for storing demod QAM mode


@retval   FUNCTION_SUCCESS   Get demod QAM mode successfully.
@retval   FUNCTION_ERROR     Get demod QAM mode unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_QAM_MODE() with the corresponding function.


@see   DTMB_QAM_MODE

*/
typedef int
(*DTMB_DEMOD_FP_GET_QAM_MODE)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pQamMode
	);





/**

@brief   DTMB demod code rate mode getting function pointer

One can use DTMB_DEMOD_FP_GET_CODE_RATE_MODE() to get DTMB demod code rate mode.


@param [in]    pDemod          The demod module pointer
@param [out]   pCodeRateMode   Pointer to an allocated memory for storing demod code rate mode


@retval   FUNCTION_SUCCESS   Get demod code rate mode successfully.
@retval   FUNCTION_ERROR     Get demod code rate mode unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_CODE_RATE_MODE() with the corresponding function.


@see   DTMB_CODE_RATE_MODE

*/
typedef int
(*DTMB_DEMOD_FP_GET_CODE_RATE_MODE)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pCodeRateMode
	);





/**

@brief   DTMB demod time interleaver mode getting function pointer

One can use DTMB_DEMOD_FP_GET_TIME_INTERLEAVER_MODE() to get DTMB demod time interleaver mode.


@param [in]    pDemod                 The demod module pointer
@param [out]   pTimeInterleaverMode   Pointer to an allocated memory for storing demod time interleaver mode


@retval   FUNCTION_SUCCESS   Get demod time interleaver mode successfully.
@retval   FUNCTION_ERROR     Get demod time interleaver mode unsuccessfully.


@note
	-# Demod building function will set DTMB_DEMOD_FP_GET_TIME_INTERLEAVER_MODE() with the corresponding function.


@see   DTMB_TIME_INTERLEAVER_MODE

*/
typedef int
(*DTMB_DEMOD_FP_GET_TIME_INTERLEAVER_MODE)(
	DTMB_DEMOD_MODULE *pDemod,
	int *pTimeInterleaverMode
	);

typedef int
(*DTMB_DEMOD_FP_UPDATE_FUNCTION)(
	DTMB_DEMOD_MODULE *pDemod
	);

typedef int
(*DTMB_DEMOD_FP_RESET_FUNCTION)(
	DTMB_DEMOD_MODULE *pDemod
	);


/// RTL2836 extra module
typedef struct RTL2836_EXTRA_MODULE_TAG RTL2836_EXTRA_MODULE;
struct RTL2836_EXTRA_MODULE_TAG
{
	// RTL2836 update procedure enabling status
	int IsFunc1Enabled;
	int IsFunc2Enabled;

	// RTL2836 update Function 1 variables
	int Func1CntThd;
	int Func1Cnt;

	// RTL2836 update Function 2 variables
	int Func2SignalModePrevious;
};





/// DTMB demod module structure
struct DTMB_DEMOD_MODULE_TAG
{
	// Private variables
	int           DemodType;
	unsigned char DeviceAddr;
	U32BITS CrystalFreqHz;
	int           TsInterfaceMode;
	int           DiversityPipMode;

	U32BITS IfFreqHz;
	int           SpectrumMode;

	int IsIfFreqHzSet;
	int IsSpectrumModeSet;

	U32BITS CurrentPageNo;		// temp, because page register is write-only.

	union											///<   Demod extra module used by driving module
	{
		RTL2836_EXTRA_MODULE Rtl2836;
	}
	Extra;

	BASE_INTERFACE_MODULE *pBaseInterface;
	I2C_BRIDGE_MODULE *pI2cBridge;

	// Demod manipulating function pointers
	DTMB_DEMOD_FP_GET_DEMOD_TYPE              GetDemodType;
	DTMB_DEMOD_FP_GET_DEVICE_ADDR             GetDeviceAddr;
	DTMB_DEMOD_FP_GET_CRYSTAL_FREQ_HZ         GetCrystalFreqHz;

	DTMB_DEMOD_FP_SOFTWARE_RESET              SoftwareReset;

	DTMB_DEMOD_FP_INITIALIZE                  Initialize;
	DTMB_DEMOD_FP_DEINITIALIZE                DeInitialize;
	DTMB_DEMOD_FP_SET_IF_FREQ_HZ              SetIfFreqHz;
	DTMB_DEMOD_FP_SET_SPECTRUM_MODE           SetSpectrumMode;
	DTMB_DEMOD_FP_GET_IF_FREQ_HZ              GetIfFreqHz;
	DTMB_DEMOD_FP_GET_SPECTRUM_MODE           GetSpectrumMode;

	DTMB_DEMOD_FP_IS_SIGNAL_LOCKED            IsSignalLocked;

	DTMB_DEMOD_FP_GET_SIGNAL_STRENGTH         GetSignalStrength;
	DTMB_DEMOD_FP_GET_SIGNAL_QUALITY          GetSignalQuality;

	DTMB_DEMOD_FP_GET_DEMOD_STATUS			  GetDemodStatus;
	DTMB_DEMOD_FP_GET_WRAPPER_STATUS		  GetWrapperStatus;	

	DTMB_DEMOD_FP_GET_ITER					  GetIter;
	DTMB_DEMOD_FP_GET_BER                     GetBer;
	DTMB_DEMOD_FP_GET_PER                     GetPer;
	DTMB_DEMOD_FP_GET_SNR_DB                  GetSnrDb;

	DTMB_DEMOD_FP_GET_RF_AGC                  GetRfAgc;
	DTMB_DEMOD_FP_GET_IF_AGC                  GetIfAgc;
	DTMB_DEMOD_FP_GET_DI_AGC                  GetDiAgc;

	DTMB_DEMOD_FP_GET_TR_OFFSET_PPM           GetTrOffsetPpm;
	DTMB_DEMOD_FP_GET_CR_OFFSET_HZ            GetCrOffsetHz;

	DTMB_DEMOD_FP_GET_CARRIER_MODE            GetCarrierMode;
	DTMB_DEMOD_FP_GET_PN_MODE                 GetPnMode;
	DTMB_DEMOD_FP_GET_QAM_MODE                GetQamMode;
	DTMB_DEMOD_FP_GET_CODE_RATE_MODE          GetCodeRateMode;
	DTMB_DEMOD_FP_GET_TIME_INTERLEAVER_MODE   GetTimeInterleaverMode;

	DTMB_DEMOD_FP_UPDATE_FUNCTION             UpdateFunction;
	DTMB_DEMOD_FP_RESET_FUNCTION              ResetFunction;
};

// DTMB demod default manipulating functions
void
dtmb_demod_default_GetDemodType(
	DTMB_DEMOD_MODULE *pDemod,
	int *pDemodType
	);

void
dtmb_demod_default_GetDeviceAddr(
	DTMB_DEMOD_MODULE *pDemod,
	unsigned char *pDeviceAddr
	);

void
dtmb_demod_default_GetCrystalFreqHz(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pCrystalFreqHz
	);

int
dtmb_demod_default_GetBandwidthMode(
	DTMB_DEMOD_MODULE *pDemod,
	int *pBandwidthMode
	);

int
dtmb_demod_default_GetIfFreqHz(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pIfFreqHz
	);

int
dtmb_demod_default_GetSpectrumMode(
	DTMB_DEMOD_MODULE *pDemod,
	int *pSpectrumMode
	);


typedef enum _REALTEK_H_DTMB_QAM_MODE {
    DTMB_QAM_4NR,               ///<   4QAM-NR                
    DTMB_QAM_4,					///<   4QAM
    DTMB_QAM_16,				///<   16 QAM
    DTMB_QAM_32,				///<   32 QAM
    DTMB_QAM_64,				///<   64 QAM
    DVBC_QAM_AUTO
}REALTEK_H_DTMB_QAM_MODE;

typedef enum _REALTEK_H_DEMOD_DTMB_QAM_MODE {
    DTMB_DEDMO_QAM_4_NR = 0,
    DTMB_DEDMO_QAM_4,				
    DTMB_DEMOD_QAM_16,
    DTMB_DEMOD_QAM_32,
    DTMB_DEMOD_QAM_64,
    DVBC_DEMOD_QAM_OTHER
}_REALTEK_H_DEMOD_DTMB_QAM_MODE;




#define REALTEK_H_DTMB_BASE_PRINT_LVL_DBG      0
#define REALTEK_H_DTMB_BASE_PRINT_LVL_INFO     1
#define REALTEK_H_DTMB_BASE_PRINT_LVL_WARNING  2


#ifndef REALTEK_H_DTMB_BASE_PRINT_LVL
#define REALTEK_H_DTMB_BASE_PRINT_LVL          REALTEK_H_DTMB_BASE_PRINT_LVL_INFO
#endif

#define REALTEK_H_DTMB_BASE_DBG(fmt, args...)         rtd_demod_print(KERN_DEBUG,"Dtmb Debug, " fmt, ##args);
#define REALTEK_H_DTMB_BASE_INFO(fmt, args...)        rtd_demod_print(KERN_INFO,"Dtmb Info, " fmt, ##args);
#define REALTEK_H_DTMB_BASE_WARNING(fmt, args...)     rtd_demod_print(KERN_WARNING,"Dtmb Warning, " fmt, ##args);
#ifdef __cplusplus
}
#endif
#endif
