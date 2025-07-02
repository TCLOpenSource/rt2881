/**

@file

@brief   DTMB demod default function definition

DTMB demod default functions.

*/

#include "dtmb_demod_base.h"


/**

@see   DTMB_DEMOD_FP_GET_DEMOD_TYPE

*/
void
dtmb_demod_default_GetDemodType(
	DTMB_DEMOD_MODULE *pDemod,
	int *pDemodType
	)
{
	// Get demod type from demod module.
	*pDemodType = pDemod->DemodType;


	return;
}





/**

@see   DTMB_DEMOD_FP_GET_DEVICE_ADDR

*/
void
dtmb_demod_default_GetDeviceAddr(
	DTMB_DEMOD_MODULE *pDemod,
	unsigned char *pDeviceAddr
	)
{
	// Get demod I2C device address from demod module.
	*pDeviceAddr = pDemod->DeviceAddr;


	return;
}





/**

@see   DTMB_DEMOD_FP_GET_CRYSTAL_FREQ_HZ

*/
void
dtmb_demod_default_GetCrystalFreqHz(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pCrystalFreqHz
	)
{
	// Get demod crystal frequency in Hz from demod module.
	*pCrystalFreqHz = pDemod->CrystalFreqHz;


	return;
}





/**

@see   DTMB_DEMOD_FP_GET_IF_FREQ_HZ

*/
int
dtmb_demod_default_GetIfFreqHz(
	DTMB_DEMOD_MODULE *pDemod,
	U32BITS *pIfFreqHz
	)
{
	// Get demod IF frequency in Hz from demod module.
	if(pDemod->IsIfFreqHzSet != YES)
		goto error_status_get_demod_if_frequency;

	*pIfFreqHz = pDemod->IfFreqHz;


	return FUNCTION_SUCCESS;


error_status_get_demod_if_frequency:
	return FUNCTION_ERROR;
}





/**

@see   DTMB_DEMOD_FP_GET_SPECTRUM_MODE

*/
int
dtmb_demod_default_GetSpectrumMode(
	DTMB_DEMOD_MODULE *pDemod,
	int *pSpectrumMode
	)
{
	// Get demod spectrum mode from demod module.
	if(pDemod->IsSpectrumModeSet != YES)
		goto error_status_get_demod_spectrum_mode;

	*pSpectrumMode = pDemod->SpectrumMode;


	return FUNCTION_SUCCESS;


error_status_get_demod_spectrum_mode:
	return FUNCTION_ERROR;
}












