/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/
/**
 @file  sony_math.h

 @brief The mathmatics functions used in this reference code.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_MATH_H_
#define SONY_MATH_H_

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "demod/demod_ext/CXD2856Family_refcode/sony_common.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/

/**
 @brief Get an integer based log2 of x and return as (log2(x) * 100)
        No checking of 0 parameter. Passing in 0
        will return 0.

 @param x The value to get the log2() of.

 @return log2(x) * 100.
*/
uint32_t sony_math_log2 (uint32_t x);

/**
 @brief Get and integer based log10 of x and return as (log10(x) * 100)
        No checking of 0 parameter. Passing in 0
        will return 0.

 @param x The value to get the log10() of.

 @return log10(x) * 100.
*/
uint32_t sony_math_log10 (uint32_t x);

/**
 @brief Get an integer based ln (log) of x and return as (ln(x) * 100)
        No checking of 0 parameter. Passing in 0
        will return 0.

 @param x The value to get the ln() of.

 @return ln(x) * 100.
*/
uint32_t sony_math_log (uint32_t x);

/**
 @brief minimum macro if not already defined.  Returns the minimum value between two numbers

 @param a First number to compare
 @param b Second number to compare
*/
//ifndef min
//   define min(a,b)            (((a) < (b)) ? (a) : (b))
//endif

#endif /* SONY_MATH_H_ */
