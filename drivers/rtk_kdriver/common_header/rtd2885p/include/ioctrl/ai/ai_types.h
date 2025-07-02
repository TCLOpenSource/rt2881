#ifndef _AI_TYPE_H_
#define _AI_TYPE_H_

/************************************************************************
 *  Include files
 ************************************************************************/
//#include <linux/types.h>

 /************************************************************************
 *  type
 ************************************************************************/
#if ( !defined(CONFIG_RTK_AI_DRV) && !defined(CONFIG_RTK_AI_DRV_MODULE) )

 // use s64 for 64/32bit compatibility
#ifndef s64
typedef signed long long _s64;
#define s64 _s64
#endif

#ifndef u8
typedef uint8_t _u8;
#define u8 _u8
#endif

#ifndef s8
typedef int8_t _s8;
#define s8 _s8
#endif

#ifndef u16
typedef uint16_t _u16;
#define u16 _u16
#endif

#ifndef s16
typedef int16_t _s16;
#define s16 _s16
#endif

#ifndef u32
typedef uint32_t _u32;
#define u32 _u32
#endif

#ifndef s32
typedef int32_t _s32;
#define s32 _s32
#endif
#endif

struct timespec_rtk{
	s64	tv_sec;                 /* seconds */
	s64	tv_nsec;                /* nanoseconds */
};

#endif	/* _AI_TYPE_H_ */
