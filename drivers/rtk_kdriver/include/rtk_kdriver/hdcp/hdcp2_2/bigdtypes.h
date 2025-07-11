/* $Id: bigdtypes.h $ */

/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.2 originally written by David Ireland,
copyright (c) 2001-8 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
    Last updated:
    $Date: 2008-05-04 13:05:00 $
    $Revision: 2.2.0 $
    $Author: dai $
*/

#ifndef BIGDTYPES_H_
#define BIGDTYPES_H_ 1

/*
The following PP instructions assume that all Linux systems have a C99-conforming
<stdint.h>; that other Unix systems have the uint32_t definitions in <sys/types.h>;
and that MS et al don't have them at all. This version assumes that a long is 32 bits.
Adjust if necessary to suit your system.
You can override by defining HAVE_C99INCLUDES or HAVE_SYS_TYPES.
*/
#if 0
    #ifndef EXACT_INTS_DEFINED_
        #define EXACT_INTS_DEFINED_ 1
        #ifndef HAVE_C99INCLUDES
            #if (__STDC_VERSION >= 199901L) || defined(linux) || defined(__linux__)
                #define HAVE_C99INCLUDES
            #endif  /*
            */
        #endif  /*
        */
        #ifndef HAVE_SYS_TYPES
            #if defined(unix) || defined(__unix__)
                #define HAVE_SYS_TYPES
            #endif  /*
            */
        #endif  /*
        */
        #ifdef HAVE_C99INCLUDES
            #include <stdint.h>
        #elif defined(HAVE_SYS_TYPES)
            #include <sys/types.h>
        #else   /*
            */
            #define uint32_t unsigned long
            #define uint16_t unsigned short
        #endif  /* HAVE_C99INCLUDES */
    #endif  /* EXACT_INTS_DEFINED_ */
#endif  /*
*/
#define uint32_t unsigned int
#define uint16_t unsigned short
/* Macros for format specifiers
-- change to "u", "x" and "X" if necessary */
#if 0
    #ifdef HAVE_C99INCLUDES
        #include <inttypes.h>
    #else   /*
        */
        #define PRIu32 "lu"
        #define PRIx32 "lx"
        #define PRIX32 "lX"
    #endif  /*
    */
#endif  /*
*/
#define PRIu32 "lu"
#define PRIx32 "lx"
#define PRIX32 "lX"
/* We define our own */
#define PRIuBIGD PRIu32
#define PRIxBIGD PRIx32
#define PRIXBIGD PRIX32

#endif  /* BIGDTYPES_H_ */
