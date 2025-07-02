#ifndef _RMM_COMMON_H
#define _RMM_COMMON_H

#ifdef BUILD_QUICK_SHOW
#include <sysdefs.h>
#endif

typedef enum {
	RTK_DC_COMMON_RET_SUCCESS = 0,
	RTK_DC_COMMON_RET_FAIL = 1
} RTK_DC_COMMON_RET_t;

typedef enum {
	RTK_DC_COMMON_BITWIDTH_16 = 0,
	RTK_DC_COMMON_BITWIDTH_32 = 1, 
	RTK_DC_COMMON_BITWIDTH_48 = 2,
	RTK_DC_COMMON_BITWIDTH_64 = 3
} RTK_DC_COMMON_BITWIDTH_t;

bool carvedout_enable(void); 
RTK_DC_COMMON_RET_t rtk_dc_get_dram_bitwidth(RTK_DC_COMMON_BITWIDTH_t *p_dram_bitwidth);

#endif /* _RMM_COMMON_H */
