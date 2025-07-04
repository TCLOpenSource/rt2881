#ifndef __FOUNDATION_H
#define __FOUNDATION_H

/**

@file

@brief   Fundamental interface declaration

Fundamental interface contains base function pointers and some mathematics tools.

*/

#include <linux/string.h>
#include <linux/delay.h>
#include "i2c_bridge.h"
#include "math_mpi.h"
#include "tv_osal.h"
#include <rtd_log/rtd_module_log.h>
#include "./../demod_ext_debug.h"


#ifdef __cplusplus
extern "C" {
#endif




// Definitions

// API version
#define REALTEK_NIM_API_VERSION		"Realtek NIM API 2012.12.12"

// Constants
#define INVALID_POINTER_VALUE		0
#define NO_USE						0

#define LEN_1_BYTE					1
#define LEN_2_BYTE					2
#define LEN_3_BYTE					3
#define LEN_4_BYTE					4
#define LEN_5_BYTE					5
#define LEN_6_BYTE					6
#define LEN_11_BYTE					11

#define LEN_1_BIT					1

#define BYTE_MASK					0xff
#define BYTE_SHIFT					8
#define HEX_DIGIT_MASK				0xf
#define BYTE_BIT_NUM				8
#define LONG_BIT_NUM				32

#define BIT_0_MASK					0x1
#define BIT_1_MASK					0x2
#define BIT_2_MASK					0x4
#define BIT_3_MASK					0x8
#define BIT_8_MASK					0x100
#define BIT_7_SHIFT					7
#define BIT_8_SHIFT					8



// I2C buffer length
// Note: I2C_BUFFER_LEN must be greater than I2cReadingByteNumMax and I2cWritingByteNumMax in BASE_INTERFACE_MODULE.
#define I2C_BUFFER_LEN				128





/// Module types
enum MODULE_TYPE {

	// DVB-T demod
	DVBT_DEMOD_TYPE_MERLIN2,			///<   MERLIN2 QAM demod
	DVBT_DEMOD_TYPE_REALTEK_I,				///<   RTD284X QAM demod
	DVBTX_DEMOD_TYPE_REALTEK_A, 

	// QAM demod
	QAM_DEMOD_TYPE_RTL2840,					///<   RTL2840 DVB-C demod
	QAM_DEMOD_TYPE_RTL2810_OC,				///<   RTL2810 OpenCable demod
	QAM_DEMOD_TYPE_RTL2820_OC,				///<   RTL2820 OpenCable demod
	QAM_DEMOD_TYPE_RTD2885_QAM,				///<   RTD2885 QAM demod
	QAM_DEMOD_TYPE_RTD2932_QAM,				///<   RTD2932 QAM demod
	QAM_DEMOD_TYPE_RTL2836B_DVBC,			///<   RTL2836 DVB-C demod
	QAM_DEMOD_TYPE_RTL2810B_QAM,			///<   RTL2810B QAM demod
	QAM_DEMOD_TYPE_RTD2840B_QAM,			///<   RTD2840B QAM demod
	QAM_DEMOD_TYPE_RTD2974_QAM,				///<   RTD2974 QAM demod
	QAM_DEMOD_TYPE_RTD2648_QAM,				///<   RTD2648 QAM demod
	QAM_DEMOD_TYPE_RTD2995_QAM,				///<   RTD2995 QAM dem
	QAM_DEMOD_TYPE_RTD2945_QAM,				///<   RTD2945 QAM demodod
	QAM_DEMOD_TYPE_RTD299S_QAM,				///<   RTD299S QAM demod
	QAM_DEMOD_TYPE_MERLIN2,				///<   MERLIN2 QAM demod
	QAM_DEMOD_TYPE_REALTEK_I,				///<   RTD284X QAM demod
	QAM_DEMOD_TYPE_REALTEK_R_QAM,

	// DVB-C demod
	DVBC_DEMOD_TYPE_MERLIN2,
	DVBC_DEMOD_TYPE_REALTEK_I,

	// DVB-C2 demod
	DVBC2_DEMOD_TYPE_REALTEK_A,

	// OOB demod
	OOB_DEMOD_TYPE_RTL2820_OOB,				///<   RTL2820 OOB demod

	// ATSC demod
	ATSC_DEMOD_TYPE_MERLIN2,			///<   MERLIN2 ATSC demod
	ATSC_DEMOD_TYPE_REALTEK_R,				///<   RTD284X ATSC demod
	ATSC3_DEMOD_TYPE_REALTEK_R,				///<   RTD284X ATSC demod

	// DTMB demod
	DTMB_DEMOD_TYPE_MERLIN2_DTMB,			///<   MERLIN2 DTMB demod
	DTMB_DEMOD_TYPE_REALTEK_H,				///<   RTD284X DTMB demod

	// ISDB-T demod
	ISDBT_DEMOD_TYPE_MERLIN2,			///<   MERLIN2 ISDB-T demod
	ISDBT_DEMOD_TYPE_REALTEK_I,				///<   RTD284X ISDB-T demod
	ISDBT_DEMOD_TYPE_REALTEK_R,

	//DVB-S demod
	DVBS_DEMOD_TYPE_MERLIN2,
	DVBS_DEMOD_TYPE_REALTEK_I,

	//DVB-S2 demod
	DVBS2_DEMOD_TYPE_MERLIN2,
	DVBS2_DEMOD_TYPE_REALTEK_I,
	
	DVBSX_DEMOD_TYPE_REALTEK_A,

	// TUNER
	TUNER_TYPE_TDCGG052D,					///<   TDCG-G052D tuner (QAM)
	TUNER_TYPE_TDCHG001D,					///<   TDCH-G001D tuner (QAM)
	TUNER_TYPE_TDQE3003A,					///<   TDQE3-003A tuner (QAM)
	TUNER_TYPE_DCT7045,						///<   DCT-7045 tuner (QAM)
	TUNER_TYPE_MT2062,						///<   MT2062 tuner (QAM)
	TUNER_TYPE_MXL5005S,					///<   MxL5005S tuner (DVB-T, ATSC)
	TUNER_TYPE_TDVMH715P,					///<   TDVM-H751P tuner (QAM, OOB, ATSC)
	TUNER_TYPE_UBA00AL,						///<   UBA00AL tuner (QAM, ATSC)
	TUNER_TYPE_MT2266,						///<   MT2266 tuner (DVB-T)
	TUNER_TYPE_FC2580,						///<   FC2580 tuner (DVB-T, DTMB)
	TUNER_TYPE_TUA9001,						///<   TUA9001 tuner (DVB-T)
	TUNER_TYPE_DTT75300,					///<   DTT-75300 tuner (DVB-T)
	TUNER_TYPE_MXL5007T,					///<   MxL5007T tuner (DVB-T, ATSC)
	TUNER_TYPE_MXL603,						///<   MxL603 tuner (DVB-C, DTMB)
	TUNER_TYPE_VA1T1ED6093,					///<   VA1T1ED6093 tuner (DTMB)
	TUNER_TYPE_TUA8010,						///<   TUA8010 tuner (DVB-T)
	TUNER_TYPE_E4000,						///<   E4000 tuner (DVB-T)
	TUNER_TYPE_E4005,						///<   E4005 tuner (DVB-T)
	TUNER_TYPE_DCT70704,					///<   DCT-70704 tuner (QAM)
	TUNER_TYPE_MT2063,						///<   MT2063 tuner (DVB-T, QAM)
	TUNER_TYPE_FC0012,						///<   FC0012 tuner (DVB-T, DTMB)
	TUNER_TYPE_TDAG,						///<   TDAG tuner (DTMB)
	TUNER_TYPE_ADMTV804,					///<   ADMTV804 tuner (DVB-T, DTMB)
	TUNER_TYPE_MAX3543,						///<   MAX3543 tuner (DVB-T)
	TUNER_TYPE_TDA18272,					///<   TDA18272 tuner (DVB-T, QAM, DTMB)
	TUNER_TYPE_TDA18250,					///<   TDA18250 tuner (DVB-T, QAM, DTMB)
	TUNER_TYPE_FC0013,						///<   FC0013 tuner (DVB-T, DTMB)
	TUNER_TYPE_FC0013B,						///<   FC0013B tuner (DVB-T, DTMB)
	TUNER_TYPE_FC0013S,						///<   FC0013S tuner (DTMB)
	TUNER_TYPE_VA1E1ED2403,					///<   VA1E1ED2403 tuner (DTMB)
	TUNER_TYPE_AVALON,						///<   AVALON tuner (DTMB)
	TUNER_TYPE_SUTRE201,					///<   SUTRE201 tuner (DTMB)
	TUNER_TYPE_MR1300,						///<   MR1300 tuner (ISDB-T 1-Seg)
	TUNER_TYPE_TDAC7,						///<   TDAC7 tuner (DTMB, QAM)
	TUNER_TYPE_VA1T1ER2094,					///<   VA1T1ER2094 tuner (DTMB)
	TUNER_TYPE_TDAC3,						///<   TDAC3 tuner (DTMB)
	TUNER_TYPE_RT910,						///<   RT910 tuner (DVB-T)
	TUNER_TYPE_DTM4C20,						///<   DTM4C20 tuner (DTMB)
	TUNER_TYPE_GTFD32,						///<   GTFD32 tuner (DTMB)
	TUNER_TYPE_GTLP10,						///<   GTLP10 tuner (DTMB)
	TUNER_TYPE_JSS66T,						///<   JSS66T tuner (DTMB)
	TUNER_TYPE_NONE,						///<   NONE tuner (DTMB)
	TUNER_TYPE_MR1500,						///<   MR1500 tuner (ISDB-T full-Seg)
	TUNER_TYPE_R820T,						///<   R820T tuner (DVB-T)
	TUNER_TYPE_R820C,						///<   R820C tuner (DVB-T, QAM)
	TUNER_TYPE_YTMB04,						///<   YTMB-04 tuner (DTMB)
	TUNER_TYPE_DTI7,						///<   DTI7 tuner (DTMB)
	TUNER_TYPE_FC5100,						///<   FC5100 tuner (DVB-T, DTMB)
	TUNER_TYPE_TDA18273,					///<   TDA18272 tuner (DVB-T, QAM, DTMB)
	TUNER_TYPE_SI2158,					///<   SI2158 tuner (DVB-T, QAM, DTMB)
	TUNER_TYPE_RDA5880P,					///<   RDA5880P (DTMB)
	TUNER_TYPE_NM120,						///<   nm120 tuner (DTMB)	// DVB-T NIM
};


/// Yes/no status
enum YES_NO_STATUS {
	NO,			///<   No
	YES,		///<   Yes
};


/// Lock status
enum LOCK_STATUS {
	NOT_LOCKED,			///<   Not locked
	LOCKED,				///<   Locked
};


/// Loss status
enum LOSS_STATUS {
	NOT_LOST,			///<   Not lost
	LOST,				///<   Lost
};


/// Function return status
enum FUNCTION_RETURN_STATUS {
	FUNCTION_SUCCESS,			///<   Execute function successfully.
	FUNCTION_ERROR,				///<   Execute function unsuccessfully.
};


/// Crystal frequency
enum CRYSTAL_FREQ_HZ {
	CRYSTAL_FREQ_4000000HZ  =  4000000,			///<   Crystal frequency =    4.0 MHz
	CRYSTAL_FREQ_16000000HZ = 16000000,			///<   Crystal frequency =   16.0 MHz
	CRYSTAL_FREQ_16384000HZ = 16384000,			///<   Crystal frequency = 16.384 MHz
	CRYSTAL_FREQ_16457143HZ = 16457143,			///<   Crystal frequency = 16.457 MHz
	CRYSTAL_FREQ_20000000HZ = 20000000,			///<   Crystal frequency =   20.0 MHz
	CRYSTAL_FREQ_20250000HZ = 20250000,			///<   Crystal frequency =  20.25 MHz
	CRYSTAL_FREQ_20480000HZ = 20480000,			///<   Crystal frequency =  20.48 MHz
	CRYSTAL_FREQ_23000000HZ = 23000000,			///<   Crystal frequency =   23.0 MHz
	CRYSTAL_FREQ_24000000HZ = 24000000,			///<   Crystal frequency =   24.0 MHz
	CRYSTAL_FREQ_25000000HZ = 25000000,			///<   Crystal frequency =   25.0 MHz
	CRYSTAL_FREQ_25200000HZ = 25200000,			///<   Crystal frequency =   25.2 MHz
	CRYSTAL_FREQ_26000000HZ = 26000000,			///<   Crystal frequency =   26.0 MHz
	CRYSTAL_FREQ_26690000HZ = 26690000,			///<   Crystal frequency =  26.69 MHz
	CRYSTAL_FREQ_27000000HZ = 27000000,			///<   Crystal frequency =   27.0 MHz
	CRYSTAL_FREQ_28800000HZ = 28800000,			///<   Crystal frequency =   28.8 MHz
	CRYSTAL_FREQ_30600000HZ = 30600000,			///<   Crystal frequency =   30.6 MHz
	CRYSTAL_FREQ_31500000HZ = 31500000,			///<   Crystal frequency =   31.5 MHz
	CRYSTAL_FREQ_32000000HZ = 32000000,			///<   Crystal frequency =   32.0 MHz
	CRYSTAL_FREQ_36000000HZ = 36000000,			///<   Crystal frequency =   36.0 MHz
};


/// IF frequency
enum IF_FREQ_HZ {
	IF_FREQ_0HZ        =        0,			///<   IF frequency =      0 MHz
	IF_FREQ_3250000HZ  =  3250000,			///<   IF frequency =   3.25 MHz
	IF_FREQ_3570000HZ  =  3570000,			///<   IF frequency =   3.57 MHz
	IF_FREQ_4000000HZ  =  4000000,			///<   IF frequency =    4.0 MHz
	IF_FREQ_4063000HZ  =  4063000,			///<   IF frequency =  4.063 MHz
	IF_FREQ_4500000HZ  =  4500000,			///<   IF frequency =    4.5 MHz
	IF_FREQ_4570000HZ  =  4570000,			///<   IF frequency =   4.57 MHz
	IF_FREQ_4571429HZ  =  4571429,			///<   IF frequency =  4.571 MHz
	IF_FREQ_4800000HZ  =  4800000,			///<   IF frequency =    4.8 MHz
	IF_FREQ_5000000HZ  =  5000000,			///<   IF frequency =    5.0 MHz
	IF_FREQ_5070000HZ  =  5070000,			///<   IF frequency =   5.07 MHz
	IF_FREQ_36000000HZ = 36000000,			///<   IF frequency =   36.0 MHz
	IF_FREQ_36125000HZ = 36125000,			///<   IF frequency = 36.125 MHz
	IF_FREQ_36150000HZ = 36150000,			///<   IF frequency =  36.15 MHz
	IF_FREQ_36166667HZ = 36166667,			///<   IF frequency = 36.167 MHz
	IF_FREQ_36170000HZ = 36170000,			///<   IF frequency =  36.17 MHz
	IF_FREQ_43750000HZ = 43750000,			///<   IF frequency =  43.75 MHz
	IF_FREQ_44000000HZ = 44000000,			///<   IF frequency =   44.0 MHz
};


/// Spectrum mode
enum SPECTRUM_MODE {
	SPECTRUM_NORMAL,			///<   Normal spectrum
	SPECTRUM_INVERSE,			///<   Inverse spectrum
};
#define SPECTRUM_MODE_NUM		2


/// TS interface mode
enum TS_INTERFACE_MODE {
	TS_INTERFACE_PARALLEL,			///<   Parallel TS interface
	TS_INTERFACE_SERIAL,			///<   Serial TS interface
};
#define TS_INTERFACE_MODE_NUM		2

/// TS interface CLK POL
enum TS_INTERFACE_CLK_POL {
	TS_INTERFACE_CLK_POL_FALLING,
	TS_INTERFACE_CLK_POL_RISING,
};
#define TS_INTERFACE_CLK_POL_NUM		2


/// Diversity mode
enum DIVERSITY_PIP_MODE {
	DIVERSITY_PIP_OFF,				///<   Diversity disable and PIP disable
	DIVERSITY_ON_MASTER,			///<   Diversity enable for Master Demod
	DIVERSITY_ON_SLAVE,				///<   Diversity enable for Slave Demod
	PIP_ON_MASTER,					///<   PIP enable for Master Demod
	PIP_ON_SLAVE,					///<   PIP enable for Slave Demod
};
#define DIVERSITY_PIP_MODE_NUM		5





/// Base interface module alias
typedef struct BASE_INTERFACE_MODULE_TAG BASE_INTERFACE_MODULE;





/**

@brief   Basic I2C reading function pointer

Upper layer functions will use BASE_FP_I2C_READ() to read ByteNum bytes from I2C device to pReadingBytes buffer.


@param [in]    pBaseInterface   The base interface module pointer
@param [in]    DeviceAddr       I2C device address in 8-bit format
@param [out]   pReadingBytes    Buffer pointer to an allocated memory for storing reading bytes
@param [in]    ByteNum          Reading byte number


@retval   FUNCTION_SUCCESS   Read bytes from I2C device with reading byte number successfully.
@retval   FUNCTION_ERROR     Read bytes from I2C device unsuccessfully.


@note
	The requirements of BASE_FP_I2C_READ() function are described as follows:
	-# Follow the I2C format for BASE_FP_I2C_READ(). \n
	   start_bit + (DeviceAddr | reading_bit) + reading_byte * ByteNum + stop_bit
	-# Don't allocate memory on pReadingBytes.
	-# Upper layer functions should allocate memory on pReadingBytes before using BASE_FP_I2C_READ().
	-# Need to assign I2C reading funtion to BASE_FP_I2C_READ() for upper layer functions.



@par Example:
@code


#include "foundation.h"


// Implement I2C reading funciton for BASE_FP_I2C_READ function pointer.
int
CustomI2cRead(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	unsigned char *pReadingBytes,
	unsigned char ByteNum
	)
{
	...

	return FUNCTION_SUCCESS;

error_status:

	return FUNCTION_ERROR;
}


int main(void)
{
	BASE_INTERFACE_MODULE *pBaseInterface;
	BASE_INTERFACE_MODULE BaseInterfaceModuleMemory;
	unsigned char ReadingBytes[100];


	// Assign implemented I2C reading funciton to BASE_FP_I2C_READ in base interface module.
	BuildBaseInterface(&pBaseInterface, &BaseInterfaceModuleMemory, ..., ..., CustomI2cRead, ..., ...);

	...

	// Use I2cRead() to read 33 bytes from I2C device and store reading bytes to ReadingBytes.
	pBaseInterface->I2cRead(pBaseInterface, 0x20, ReadingBytes, 33);

	...

	return 0;
}


@endcode

*/
typedef int
(*BASE_FP_I2C_READ)(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	unsigned char* pRegisterAddr,
	unsigned char RegisterAddrSize,
	unsigned char *pReadingBytes,
	U32BITS ByteNum
);





/**

@brief   Basic I2C writing function pointer

Upper layer functions will use BASE_FP_I2C_WRITE() to write ByteNum bytes from pWritingBytes buffer to I2C device.


@param [in]   pBaseInterface   The base interface module pointer
@param [in]   DeviceAddr       I2C device address in 8-bit format
@param [in]   pWritingBytes    Buffer pointer to writing bytes
@param [in]   ByteNum          Writing byte number


@retval   FUNCTION_SUCCESS   Write bytes to I2C device with writing bytes successfully.
@retval   FUNCTION_ERROR     Write bytes to I2C device unsuccessfully.


@note
	The requirements of BASE_FP_I2C_WRITE() function are described as follows:
	-# Follow the I2C format for BASE_FP_I2C_WRITE(). \n
	   start_bit + (DeviceAddr | writing_bit) + writing_byte * ByteNum + stop_bit
	-# Need to assign I2C writing funtion to BASE_FP_I2C_WRITE() for upper layer functions.



@par Example:
@code


#include "foundation.h"


// Implement I2C writing funciton for BASE_FP_I2C_WRITE function pointer.
int
CustomI2cWrite(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	const unsigned char *pWritingBytes,
	unsigned char ByteNum
	)
{
	...

	return FUNCTION_SUCCESS;

error_status:

	return FUNCTION_ERROR;
}


int main(void)
{
	BASE_INTERFACE_MODULE *pBaseInterface;
	BASE_INTERFACE_MODULE BaseInterfaceModuleMemory;
	unsigned char WritingBytes[100];


	// Assign implemented I2C writing funciton to BASE_FP_I2C_WRITE in base interface module.
	BuildBaseInterface(&pBaseInterface, &BaseInterfaceModuleMemory, ..., ..., ..., CustomI2cWrite, ...);

	...

	// Use I2cWrite() to write 33 bytes from WritingBytes to I2C device.
	pBaseInterface->I2cWrite(pBaseInterface, 0x20, WritingBytes, 33);

	...

	return 0;
}


@endcode

*/
typedef int
(*BASE_FP_I2C_WRITE)(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	const unsigned char *pWritingBytes,
	U32BITS ByteNum
);





/**

@brief   Basic waiting function pointer

Upper layer functions will use BASE_FP_WAIT_MS() to wait WaitTimeMs milliseconds.


@param [in]   pBaseInterface   The base interface module pointer
@param [in]   WaitTimeMs       Waiting time in millisecond


@note
	The requirements of BASE_FP_WAIT_MS() function are described as follows:
	-# Need to assign a waiting function to BASE_FP_WAIT_MS() for upper layer functions.



@par Example:
@code


#include "foundation.h"


// Implement waiting funciton for BASE_FP_WAIT_MS function pointer.
void
CustomWaitMs(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned long WaitTimeMs
	)
{
	...

	return;
}


int main(void)
{
	BASE_INTERFACE_MODULE *pBaseInterface;
	BASE_INTERFACE_MODULE BaseInterfaceModuleMemory;


	// Assign implemented waiting funciton to BASE_FP_WAIT_MS in base interface module.
	BuildBaseInterface(&pBaseInterface, &BaseInterfaceModuleMemory, ..., ..., ..., ..., CustomWaitMs);

	...

	// Use WaitMs() to wait 30 millisecond.
	pBaseInterface->WaitMs(pBaseInterface, 30);

	...

	return 0;
}


@endcode

*/
typedef void
(*BASE_FP_WAIT_MS)(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned long WaitTimeMs
);





/**

@brief   User defined data pointer setting function pointer

One can use BASE_FP_SET_USER_DEFINED_DATA_POINTER() to set user defined data pointer of base interface structure for
custom basic function implementation.


@param [in]   pBaseInterface     The base interface module pointer
@param [in]   pUserDefinedData   Pointer to user defined data


@note
	One can use BASE_FP_GET_USER_DEFINED_DATA_POINTER() to get user defined data pointer of base interface structure for
	custom basic function implementation.



@par Example:
@code


#include "foundation.h"


// Implement I2C reading funciton for BASE_FP_I2C_READ function pointer.
int
CustomI2cRead(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	unsigned char *pReadingBytes,
	unsigned char ByteNum
	)
{
	CUSTOM_USER_DEFINED_DATA *pUserDefinedData;


	// Get user defined data pointer of base interface structure for custom I2C reading function.
	pBaseInterface->GetUserDefinedDataPointer(pBaseInterface, (void **)&pUserDefinedData);

	...

	return FUNCTION_SUCCESS;

error_status:

	return FUNCTION_ERROR;
}


int main(void)
{
	BASE_INTERFACE_MODULE *pBaseInterface;
	BASE_INTERFACE_MODULE BaseInterfaceModuleMemory;
	unsigned char ReadingBytes[100];

	CUSTOM_USER_DEFINED_DATA UserDefinedData;


	// Assign implemented I2C reading funciton to BASE_FP_I2C_READ in base interface module.
	BuildBaseInterface(&pBaseInterface, &BaseInterfaceModuleMemory, ..., ..., CustomI2cRead, ..., ...);

	...

	// Set user defined data pointer of base interface structure for custom basic functions.
	pBaseInterface->SetUserDefinedDataPointer(pBaseInterface, &UserDefinedData);

	// Use I2cRead() to read 33 bytes from I2C device and store reading bytes to ReadingBytes.
	pBaseInterface->I2cRead(pBaseInterface, 0x20, ReadingBytes, 33);

	...

	return 0;
}


@endcode

*/
typedef void
(*BASE_FP_SET_USER_DEFINED_DATA_POINTER)(
	BASE_INTERFACE_MODULE *pBaseInterface,
	void *pUserDefinedData
);





/**

@brief   User defined data pointer getting function pointer

One can use BASE_FP_GET_USER_DEFINED_DATA_POINTER() to get user defined data pointer of base interface structure for
custom basic function implementation.


@param [in]   pBaseInterface      The base interface module pointer
@param [in]   ppUserDefinedData   Pointer to user defined data pointer


@note
	One can use BASE_FP_SET_USER_DEFINED_DATA_POINTER() to set user defined data pointer of base interface structure for
	custom basic function implementation.



@par Example:
@code


#include "foundation.h"


// Implement I2C reading funciton for BASE_FP_I2C_READ function pointer.
int
CustomI2cRead(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	unsigned char *pReadingBytes,
	unsigned char ByteNum
	)
{
	CUSTOM_USER_DEFINED_DATA *pUserDefinedData;


	// Get user defined data pointer of base interface structure for custom I2C reading function.
	pBaseInterface->GetUserDefinedDataPointer(pBaseInterface, (void **)&pUserDefinedData);

	...

	return FUNCTION_SUCCESS;

error_status:

	return FUNCTION_ERROR;
}


int main(void)
{
	BASE_INTERFACE_MODULE *pBaseInterface;
	BASE_INTERFACE_MODULE BaseInterfaceModuleMemory;
	unsigned char ReadingBytes[100];

	CUSTOM_USER_DEFINED_DATA UserDefinedData;


	// Assign implemented I2C reading funciton to BASE_FP_I2C_READ in base interface module.
	BuildBaseInterface(&pBaseInterface, &BaseInterfaceModuleMemory, ..., ..., CustomI2cRead, ..., ...);

	...

	// Set user defined data pointer of base interface structure for custom basic functions.
	pBaseInterface->SetUserDefinedDataPointer(pBaseInterface, &UserDefinedData);

	// Use I2cRead() to read 33 bytes from I2C device and store reading bytes to ReadingBytes.
	pBaseInterface->I2cRead(pBaseInterface, 0x20, ReadingBytes, 33);

	...

	return 0;
}


@endcode

*/
typedef void
(*BASE_FP_GET_USER_DEFINED_DATA_POINTER)(
	BASE_INTERFACE_MODULE *pBaseInterface,
	void **ppUserDefinedData
);





/// Base interface module structure
struct BASE_INTERFACE_MODULE_TAG {
	// Variables and function pointers
	unsigned long I2cReadingByteNumMax;
	unsigned long I2cWritingByteNumMax;

	BASE_FP_I2C_READ    I2cRead;
	BASE_FP_I2C_WRITE   I2cWrite;
	BASE_FP_WAIT_MS     WaitMs;

	BASE_FP_SET_USER_DEFINED_DATA_POINTER   SetUserDefinedDataPointer;
	BASE_FP_GET_USER_DEFINED_DATA_POINTER   GetUserDefinedDataPointer;


	// User defined data
	void *pUserDefinedData;
	unsigned char useI2C;
};





/**

@brief   Base interface builder

Use BuildBaseInterface() to build base interface for module functions to access basic functions.


@param [in]   ppBaseInterface              Pointer to base interface module pointer
@param [in]   pBaseInterfaceModuleMemory   Pointer to an allocated base interface module memory
@param [in]   I2cReadingByteNumMax         Maximum I2C reading byte number for basic I2C reading function
@param [in]   I2cWritingByteNumMax         Maximum I2C writing byte number for basic I2C writing function
@param [in]   I2cRead                      Basic I2C reading function pointer
@param [in]   I2cWrite                     Basic I2C writing function pointer
@param [in]   WaitMs                       Basic waiting function pointer


@note
	-# One should build base interface before using module functions.
	-# The I2C reading format is described as follows:
	   start_bit + (device_addr | reading_bit) + reading_byte * byte_num + stop_bit
	-# The I2cReadingByteNumMax is the maximum byte_num of the I2C reading format.
	-# The I2C writing format is described as follows:
	   start_bit + (device_addr | writing_bit) + writing_byte * byte_num + stop_bit
	-# The I2cWritingByteNumMax is the maximum byte_num of the I2C writing format.



@par Example:
@code


#include "foundation.h"


// Implement I2C reading funciton for BASE_FP_I2C_READ function pointer.
int
CustomI2cRead(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	unsigned char *pReadingBytes,
	unsigned char ByteNum
	)
{
	...

	return FUNCTION_SUCCESS;

error_status:

	return FUNCTION_ERROR;
}


// Implement I2C writing funciton for BASE_FP_I2C_WRITE function pointer.
int
CustomI2cWrite(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned char DeviceAddr,
	const unsigned char *pWritingBytes,
	unsigned char ByteNum
	)
{
	...

	return FUNCTION_SUCCESS;

error_status:

	return FUNCTION_ERROR;
}


// Implement waiting funciton for BASE_FP_WAIT_MS function pointer.
void
CustomWaitMs(
	BASE_INTERFACE_MODULE *pBaseInterface,
	unsigned long WaitTimeMs
	)
{
	...

	return;
}


int main(void)
{
	BASE_INTERFACE_MODULE *pBaseInterface;
	BASE_INTERFACE_MODULE BaseInterfaceModuleMemory;


	// Build base interface with the following settings.
	//
	// 1. Assign 9 to maximum I2C reading byte number.
	// 2. Assign 8 to maximum I2C writing byte number.
	// 3. Assign CustomI2cRead() to basic I2C reading function pointer.
	// 4. Assign CustomI2cWrite() to basic I2C writing function pointer.
	// 5. Assign CustomWaitMs() to basic waiting function pointer.
	//
	BuildBaseInterface(
		&pBaseInterface,
		&BaseInterfaceModuleMemory,
		9,
		8,
		CustomI2cRead,
		CustomI2cWrite,
		CustomWaitMs
		);

	...

	return 0;
}


@endcode

*/
void
BuildExtInterface(
	BASE_INTERFACE_MODULE **ppBaseInterface,
	BASE_INTERFACE_MODULE *pBaseInterfaceModuleMemory,
	unsigned long I2cReadingByteNumMax,
	unsigned long I2cWritingByteNumMax,
	BASE_FP_I2C_READ I2cRead,
	BASE_FP_I2C_WRITE I2cWrite,
	BASE_FP_WAIT_MS WaitMs
);





// User data pointer of base interface structure setting and getting functions
void
ext_interface_SetUserDefinedDataPointer(
	BASE_INTERFACE_MODULE *pBaseInterface,
	void *pUserDefinedData
);

void
ext_interface_GetUserDefinedDataPointer(
	BASE_INTERFACE_MODULE *pBaseInterface,
	void **ppUserDefinedData
);










/**

@mainpage Realtek demod Source Code Manual

@note
	-# The Realtek demod API source code is designed for demod IC driver porting.
	-# The API source code is written in C language without floating-point arithmetic.
	-# One can use the API to manipulate Realtek demod IC.
	-# The API will call custom underlayer functions through API base interface module.


@par Important:
	-# Please assign API base interface module with custom underlayer functions instead of modifying API source code.
	-# Please see the example code to understand the relation bewteen API and custom system.

*/















#ifdef __cplusplus
}
#endif

#endif
