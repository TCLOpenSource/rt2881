#ifndef _GDMA_SPLIT_H_
#define _GDMA_SPLIT_H_
#include <gal/rtk_gdma_export_user.h>
#ifndef BUILD_QUICK_SHOW

#include <linux/types.h>
#endif
typedef enum
{
	GDMA_SPLIT_MODE_0  =	0,
	GDMA_SPLIT_MODE_1  =	1,
	GDMA_SPLIT_MODE_2  =	2,
} _GDMA_SPLIT_T;

int GDMA_split_mode(int mode);

#endif
