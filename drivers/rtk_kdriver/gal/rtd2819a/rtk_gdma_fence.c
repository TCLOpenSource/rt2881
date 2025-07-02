

#include <linux/spinlock.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/slab.h>     /* kmalloc()      */
#include <linux/sync_file.h>

#include "gal/rtk_gdma_driver.h" //gdma_dev
#include <gal/rtk_gdma_export.h>

#include "rtk_gdma_fence.h"

extern gdma_dev *gdma_devices;
extern unsigned int gDebugGDMA_loglevel;

#define GDMA_FENCE_ARRAY_SIZE    4096
struct gdma_fence *gdma_fence_array = NULL;
unsigned int gdma_fence_index = 0;

int gDebugFence_loglevel = 0;

static DEFINE_SPINLOCK(dma_buf_gdma_fence_lock);

int GDMA_SignalFence(struct gdma_fence *gf);

unsigned int gFence_OSD_Context = 0;
/* The dma-buf fence sequence number for this atom. This is
 * increased every time this katom uses dma-buf fence.
 */
atomic_t gFence_OSD_Seqno;

static bool timeline_fence_enable_signaling(struct dma_fence *fence)
{
    return test_and_set_bit(DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT, &fence->flags);
}
static const char *timeline_fence_get_timeline_name(struct dma_fence *fence)
{
    return "T";
}
static const char *timeline_fence_get_driver_name(struct dma_fence *fence)
{
	return "gdma";
}
static bool gdma_fence_signaled(struct dma_fence *fence)
{
    return test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags);
}
static void gdma_fence_fence_value_str(struct dma_fence *fence, char *str, int size)
{
#if (KERNEL_VERSION(5, 1, 0) > LINUX_VERSION_CODE)
	snprintf(str, size, "%u", fence->seqno);
#else
	snprintf(str, size, "%llu", fence->seqno);
#endif
}
static void gdma_fence_timeline_value_str(struct dma_fence *fence, char *str, int size)
{
    snprintf(str, size, "%d", atomic_read(&gFence_OSD_Seqno));
}

void gdma_fence_free(struct dma_fence *fence)
{
    //rtd_pr_gdma_err("gdma fence (%llx) to free \n", fence);
}

static const struct dma_fence_ops timeline_fence_ops = {
	.get_driver_name = timeline_fence_get_driver_name,
	.get_timeline_name = timeline_fence_get_timeline_name,
	.enable_signaling = timeline_fence_enable_signaling,
    .signaled = gdma_fence_signaled,
	.wait = dma_fence_default_wait,
	.release = gdma_fence_free,
	.fence_value_str = gdma_fence_fence_value_str,
    .timeline_value_str = gdma_fence_timeline_value_str,
};


static struct gdma_fence* alloc_gdma_fence(void)
{
    struct gdma_fence *gf = 0;
    int fd = 0;
    struct sync_file *local_sync_file = 0;
	gdma_dev *gdma = &gdma_devices[0];

	down(&gdma->sem_get_fence);    
    //gf = kzalloc(sizeof(*gf), GFP_KERNEL);
    if (gdma_fence_array) {
        gf = gdma_fence_array + gdma_fence_index;
        gdma_fence_index = (gdma_fence_index+1) % GDMA_FENCE_ARRAY_SIZE;
    }

    if (!gf)
    {
        up(&gdma->sem_get_fence);
        return 0;
    }
        
    dma_fence_init(&gf->base,
                   &timeline_fence_ops,
                   &dma_buf_gdma_fence_lock,
                   gFence_OSD_Context,
                   atomic_inc_return(&gFence_OSD_Seqno) );

    fd = get_unused_fd_flags(O_CLOEXEC);
    if (fd < 0){
        kfree(gf);
		up(&gdma->sem_get_fence);
		return 0;
    }
    
    gf->gdma_fence_fd = fd;
    local_sync_file = sync_file_create(&gf->base);

    if (!local_sync_file) {
		put_unused_fd(fd);
		up(&gdma->sem_get_fence);
		return 0;
	}

	fd_install(fd, local_sync_file->file);
	up(&gdma->sem_get_fence);
    
	return gf;
}


int GDMA_Fence_Init( void)
{

    gdma_fence_array = kzalloc(sizeof(struct gdma_fence)*GDMA_FENCE_ARRAY_SIZE, GFP_KERNEL);

    if( gdma_fence_array == NULL ) {
        return -1;
    }


    return 0;
}

int GDMA_GetFenceInfo(GDMA_FENCE_INFO *getInfo)
{
    if(getInfo == 0)
        return GDMA_FAIL;

    {
        struct gdma_fence *new_gdma_fence = alloc_gdma_fence();
        if (new_gdma_fence)
        {
            getInfo->fence_fd = new_gdma_fence->gdma_fence_fd;
            getInfo->fence_handle = (uintptr_t) new_gdma_fence;
           GDMA_PRINT(9, "fence get %llu :%d\n",getInfo->fence_handle, getInfo->fence_fd);
        }
    }
    
    return GDMA_SUCCESS;
}

int GDMA_SignalFence(struct gdma_fence *gf)
{
    int ret = 0;
 
    if (gf)
    {
        ret = dma_fence_signal(&gf->base);
    }

    return ret;
}
