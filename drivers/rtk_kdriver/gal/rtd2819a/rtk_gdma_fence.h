#ifndef __RTK_GDMA_FENCE_H__
#define __RTK_GDMA_FENCE_H__

#include <linux/version.h>
#include <linux/dma-fence.h>
#include <gal/rtk_gdma_export_user.h>



struct gdma_fence {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	struct fence base;
#else
	struct dma_fence base;
#endif
    int gdma_fence_fd;
};


extern struct gdma_fence *gdma_fence_array;
extern int gDebugFence_loglevel;

int GDMA_Fence_Init( void);
int GDMA_GetFenceInfo(GDMA_FENCE_INFO *getInfo);


int GDMA_SignalFence(struct gdma_fence *gf);


#endif//__RTK_GDMA_FENCE_H__
