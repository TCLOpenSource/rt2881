#ifndef __MATH_MPI_H
#define __MATH_MPI_H

/**

@file

@brief   Mutliple precision integer (MPI) arithmetic declaration

One can use to mutliple precision arithmetic to manipulate large signed integers.

*/



#include "rtk_tv_fe_types.h"



// Constant
#define MPI_BYTE_BIT_NUM			8
#define MPI_LONG_BYTE_NUM			4
#define MPI_LONG_BIT_NUM			32



// Mask and shift
#define MPI_BYTE_MASK				0xff
#define MPI_BYTE_SHIFT				8

#define MPI_BIT_0_MASK				0x1
#define MPI_BIT_7_SHIFT				7



// Multiple precision integer definition
#define MPI_VALUE_BYTE_NUM_MAX		10												///<   Maximum MPI value byte number
#define MPI_VALUE_BIT_NUM_MAX		(MPI_VALUE_BYTE_NUM_MAX * MPI_BYTE_BIT_NUM)		///<   Maximum MPI value bit number

/// Multiple precision integer structure
typedef struct
{
	U32BITS BitNum;
	unsigned char Value[MPI_VALUE_BYTE_NUM_MAX];
}
MPI;



///  MPI yes/no status
enum MPI_YES_NO_STATUS
{
	MPI_NO,			///<   No
	MPI_YES,		///<   Yes
};



// Logarithm with base 2
#define MPI_LOG_MIDDLE_RESULT_FRAC_BIT_NUM			(MPI_VALUE_BIT_NUM_MAX / 2 - 2)





// MPI access
void
MpiSetValue(
	MPI *pMpiVar,
	S32BITS Value
	);

void
MpiGetValue(
	MPI MpiVar,
	S32BITS *pValue
	);

void
MpiSetBit(
	MPI *pMpiVar,
	U32BITS BitPosition,
	unsigned char BitValue
	);

void
MpiGetBit(
	MPI MpiVar,
	U32BITS BitPosition,
	unsigned char *pBitValue
	);

void
MpiGetSignedBit(
	MPI MpiVar,
	unsigned char *pSignedBit
	);



// MPI operator
void
MpiAssign(
	MPI *pResult,
	MPI Operand
	);

void
MpiUnaryMinus(
	MPI *pResult,
	MPI Operand
	);

void
MpiAdd(
	MPI *pSum,
	MPI Augend,
	MPI Addend
	);

void
MpiSub(
	MPI *pDifference,
	MPI Minuend,
	MPI Subtrahend
	);

void
MpiMul(
	MPI *pProduct,
	MPI Multiplicand,
	MPI Multiplicator
	);

void
MpiDiv(
	MPI *pQuotient,
	MPI *pRemainder,
	MPI Dividend,
	MPI Divisor
	);

void
MpiRightShift(
	MPI *pResult,
	MPI Operand,
	U32BITS ShiftBitNum
	);

void
MpiLeftShift(
	MPI *pResult,
	MPI Operand,
	U32BITS ShiftBitNum
	);

int
MpiEqualTo(
	MPI MpiLeft,
	MPI MpiRight
	);

int
MpiGreaterThan(
	MPI MpiLeft,
	MPI MpiRight
	);

int
MpiLessThan(
	MPI MpiLeft,
	MPI MpiRight
	);

void
MpiMinimizeBitNum(
	MPI *pMpiVar
	);



// MPI special function
void
MpiLog2(
	MPI *pResult,
	MPI MpiVar,
	U32BITS ResultFracBitNum
	);

#define RTK_DEMOD_WARNING(fmt, args...)    rtd_extdemod_print(KERN_WARNING,"Warning, " fmt, ##args);

#endif
