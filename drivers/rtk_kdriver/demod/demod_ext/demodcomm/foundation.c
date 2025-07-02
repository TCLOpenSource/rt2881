/**

@file

@brief   Fundamental interface definition

Fundamental interface contains base function pointers and some mathematics tools.

*/


#include "foundation.h"





// Base interface builder
void
BuildExtInterface(
	BASE_INTERFACE_MODULE **ppBaseInterface,
	BASE_INTERFACE_MODULE *pBaseInterfaceModuleMemory,
	unsigned long I2cReadingByteNumMax,
	unsigned long I2cWritingByteNumMax,
	BASE_FP_I2C_READ I2cRead,
	BASE_FP_I2C_WRITE I2cWrite,
	BASE_FP_WAIT_MS WaitMs
	)
{
	// Set base interface module pointer.
	*ppBaseInterface = pBaseInterfaceModuleMemory;


	// Set all base interface function pointers and arguments.
	(*ppBaseInterface)->I2cReadingByteNumMax      = I2cReadingByteNumMax;
	(*ppBaseInterface)->I2cWritingByteNumMax      = I2cWritingByteNumMax;
	(*ppBaseInterface)->I2cRead                   = I2cRead;
	(*ppBaseInterface)->I2cWrite                  = I2cWrite;
	(*ppBaseInterface)->WaitMs                    = WaitMs;
	(*ppBaseInterface)->SetUserDefinedDataPointer = ext_interface_SetUserDefinedDataPointer;
	(*ppBaseInterface)->GetUserDefinedDataPointer = ext_interface_GetUserDefinedDataPointer;


	return;
}





/**

@brief   Set user defined data pointer of base interface structure for custom basic function implementation.

@note
	-# Base interface builder will set BASE_FP_SET_USER_DEFINED_DATA_POINTER() function pointer with
	   base_interface_SetUserDefinedDataPointer().

@see   BASE_FP_SET_USER_DEFINED_DATA_POINTER

*/
void
ext_interface_SetUserDefinedDataPointer(
	BASE_INTERFACE_MODULE *pBaseInterface,
	void *pUserDefinedData
	)
{
	// Set user defined data pointer of base interface structure with user defined data pointer argument.
	pBaseInterface->pUserDefinedData = pUserDefinedData;


	return;
}





/**

@brief   Get user defined data pointer of base interface structure for custom basic function implementation.

@note
	-# Base interface builder will set BASE_FP_GET_USER_DEFINED_DATA_POINTER() function pointer with
	   base_interface_GetUserDefinedDataPointer().

@see   BASE_FP_GET_USER_DEFINED_DATA_POINTER

*/
void
ext_interface_GetUserDefinedDataPointer(
	BASE_INTERFACE_MODULE *pBaseInterface,
	void **ppUserDefinedData
	)
{
	// Get user defined data pointer from base interface structure to the caller user defined data pointer.
	*ppUserDefinedData = pBaseInterface->pUserDefinedData;


	return;
}





/**

@brief   Convert signed integer to binary.

Convert 2's complement signed integer to binary with bit number.


@param [in]   Value    the converting value in 2's complement format
@param [in]   BitNum   the bit number of the converting value


@return   Converted binary


@note
	The converting value must be -pow(2, BitNum - 1) ~ (pow(2, BitNum - 1) -1).



@par Example:
@code


#include "foundation.h"


int main(void)
{
	long Value = -345;
	U32BITS Binary;


	// Convert 2's complement integer to binary with 10 bit number.
	Binary = SignedIntToBin(Value, 10);


	// Result in base 2:
	// Value  = 1111 1111 1111 1111 1111 1110 1010 0111 b = -345  (in 32-bit 2's complement format)
	// Binary = 0000 0000 0000 0000 0000 0010 1010 0111 b =  679  (in 10-bit binary format)

	...

	return 0;
}


@endcode

*/











