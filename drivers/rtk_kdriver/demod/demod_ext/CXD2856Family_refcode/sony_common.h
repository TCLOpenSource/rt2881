/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/11/30
  Modification ID : 253a4918e2da2cf28a9393596fa16f25024e504d
------------------------------------------------------------------------------*/
/**
 @file  sony_common.h

 @brief Common definitions and functions used in all reference code.

        The user should modify several points depend on their software system.
        Please check "<PORTING>" comments.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_COMMON_H
#define SONY_COMMON_H

/* Type definitions. */
/* <PORTING> Please comment out if conflicted */
//#ifdef __linux__
//#include <stdint.h>
//#else
//typedef unsigned char uint8_t;      /**< Unsigned  8 bit integer. */
//typedef unsigned short uint16_t;    /**< Unsigned 16 bit integer. */
//typedef unsigned int uint32_t;      /**< Unsigned 32 bit integer. */
//typedef signed char int8_t;         /**< Signed    8 bit integer. */
//typedef signed short int16_t;       /**< Signed   16 bit integer. */
//typedef signed int int32_t;         /**< Signed   32 bit integer. */
//#endif
#include <linux/types.h>
#include <tv_osal.h>
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

/* <PORTING> Sleep function define (Please modify as you like) */
#if defined(_WINDOWS)
#include "sony_windows_sleep.h"
#define SONY_SLEEP(n) sony_windows_Sleep(n)
#elif defined(__linux__)
//#include <unistd.h>
//#define SONY_SLEEP(n) usleep(n * 1000)
#define SONY_SLEEP(n) tv_osal_msleep_ext(n)
#endif

#ifndef SONY_SLEEP
#error SONY_SLEEP(n) is not defined. This macro must be ported to your platform.
#endif

/* <PORTING> Trace function enable */
/* #define SONY_TRACE_ENABLE */
/* <PORTING> Enable if I2C related function trace is necessary */
/* #define SONY_TRACE_I2C_ENABLE */

/**
 @brief Macro to specify unused argument and suppress compiler warnings.
*/
#define SONY_ARG_UNUSED(arg) ((void)(arg))

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Return codes
*/
typedef enum {
    SONY_RESULT_OK,              /**< Function was successfully actioned */
    SONY_RESULT_ERROR_ARG,       /**< Invalid argument (maybe software bug) */
    SONY_RESULT_ERROR_I2C,       /**< I2C communication error */
    SONY_RESULT_ERROR_SW_STATE,  /**< Invalid software state */
    SONY_RESULT_ERROR_HW_STATE,  /**< Invalid hardware state */
    SONY_RESULT_ERROR_TIMEOUT,   /**< Timeout occurred */
    SONY_RESULT_ERROR_UNLOCK,    /**< Failed to lock */
    SONY_RESULT_ERROR_RANGE,     /**< Out of range */
    SONY_RESULT_ERROR_NOSUPPORT, /**< Not supported for current device */
    SONY_RESULT_ERROR_CANCEL,    /**< The operation is cancelled */
    SONY_RESULT_ERROR_OTHER,     /**< Unspecified error */
    SONY_RESULT_ERROR_OVERFLOW,  /**< Memory overflow */
    SONY_RESULT_OK_CONFIRM       /**< Tune was successful, but confirm parameters */
} sony_result_t;

/*------------------------------------------------------------------------------
  Common functions
------------------------------------------------------------------------------*/
/**
 @brief Convert unsigned integer to signed integer.

 @param value Unsigned integer value.
 @param bitlen The bit width of "value".

 @return The signed integer value.
*/
int32_t sony_Convert2SComplement(uint32_t value, uint32_t bitlen);

/**
 @brief Split bit from byte array data.

 @param pArray The byte array data.
 @param startBit The start bit position.
 @param bitNum The bit length.

 @return The data split from byte array data.
*/
uint32_t sony_BitSplitFromByteArray(uint8_t *pArray, uint32_t startBit, uint32_t bitNum);
/*------------------------------------------------------------------------------
  Trace
------------------------------------------------------------------------------*/
/*
 Disables MS compiler warning (__pragma(warning(disable:4127))
 with do { } while (0);
*/
/** Macro for beginning multiline macro. */
#define SONY_MACRO_MULTILINE_BEGIN  do {
#if ((defined _MSC_VER) && (_MSC_VER >= 1300))
#define SONY_MACRO_MULTILINE_END \
        __pragma(warning(push)) \
        __pragma(warning(disable:4127)) \
        } while(0) \
        __pragma(warning(pop))
#else
/** Macro for ending multiline macro. */
#define SONY_MACRO_MULTILINE_END } while(0)
#endif


#ifdef SONY_TRACE_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void sony_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sony_trace_log_return(sony_result_t result, const char* filename, unsigned int linenum);
#define SONY_TRACE_ENTER(func) sony_trace_log_enter((func), __FILE__, __LINE__)
#define SONY_TRACE_RETURN(result) \
    SONY_MACRO_MULTILINE_BEGIN \
        sony_trace_log_return((result), __FILE__, __LINE__); \
        return (result); \
    SONY_MACRO_MULTILINE_END
#else /* SONY_TRACE_ENABLE */
/** Trace enter */
#define SONY_TRACE_ENTER(func)
/** Trace return */
#define SONY_TRACE_RETURN(result) return(result)
/** I/O trace enter */
#define SONY_TRACE_I2C_ENTER(func)
/** I/O trace return */
#define SONY_TRACE_I2C_RETURN(result) return(result)
#endif /* SONY_TRACE_ENABLE */


#ifdef SONY_TRACE_I2C_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void sony_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sony_trace_i2c_log_return(sony_result_t result, const char* filename, unsigned int linenum);
#define SONY_TRACE_I2C_ENTER(func) sony_trace_i2c_log_enter((func), __FILE__, __LINE__)
#define SONY_TRACE_I2C_RETURN(result) \
    SONY_MACRO_MULTILINE_BEGIN \
        sony_trace_i2c_log_return((result), __FILE__, __LINE__); \
        return (result); \
    SONY_MACRO_MULTILINE_END
#else /* SONY_TRACE_I2C_ENABLE */
#define SONY_TRACE_I2C_ENTER(func)
#define SONY_TRACE_I2C_RETURN(result) return(result)
#endif /* SONY_TRACE_I2C_ENABLE */


/*------------------------------------------------------------------------------
  Multi-threaded defines
 ------------------------------------------------------------------------------*/
/**
 @brief "<PORTING>" Defines for basic atomic operations for cancellation.
*/
typedef struct sony_atomic_t {
    /**
     @brief Underlying counter.
    */
    volatile int counter;
} sony_atomic_t;
#define sony_atomic_set(a,i) ((a)->counter = i)                 /**< Set counter atomically. */
#define sony_atomic_read(a) ((a)->counter)                      /**< Get counter atomically. */


/*------------------------------------------------------------------------------
  Stopwatch struct and functions definitions
------------------------------------------------------------------------------*/
/**
 @brief "<PORTING>" Stopwatch structure to measure accurate time.
*/
typedef struct sony_stopwatch_t {
    /**
     @brief Underlying start time.
    */
    uint32_t startTime;

} sony_stopwatch_t;

/**
 @brief Start the stopwatch.

 @param pStopwatch The stopwatch instance.

 @return SONY_RESULT_OK is successful.
*/
sony_result_t sony_stopwatch_start (sony_stopwatch_t * pStopwatch);

/**
 @brief Pause for a specified period of time.

 @param pStopwatch The stopwatch instance.

 @param ms  The time in milliseconds to sleep.

 @return SONY_RESULT_OK is successful.
*/
sony_result_t sony_stopwatch_sleep (sony_stopwatch_t * pStopwatch, uint32_t ms);

/**
 @brief Returns the elapsed time (ms) since the stopwatch was started.

 @param pStopwatch The stopwatch instance.

 @param pElapsed The elapsed time in milliseconds.

 @return SONY_RESULT_OK is successful.

*/
sony_result_t sony_stopwatch_elapsed (sony_stopwatch_t * pStopwatch, uint32_t* pElapsed);

#endif /* SONY_COMMON_H */
