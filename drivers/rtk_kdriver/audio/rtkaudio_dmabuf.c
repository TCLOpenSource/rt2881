#include <linux/version.h>
#include <linux/slab.h>
#include <linux/dma-buf.h>
#include <rtk_kdriver/rmm/rmm.h>
#include <rtk_kdriver/rmm/pageremap.h>
#include <rtd_log/rtd_module_log.h>

static DEFINE_SEMAPHORE(rtkaudio_dmabuf_lock);

struct dmabuf_mem {
	void *vir_addr;
	int size;
};

#define PREALLOCATE_SIZE (4096 * 2 * 6)
struct dmabuf_mem prealloc_mem[2];

static void rtkaudio_dmabuf_release(struct dma_buf *dmabuf)
{
	down(&rtkaudio_dmabuf_lock);
	if (dmabuf->priv) {
		rtd_pr_adsp_info("%s: free vaddr %lx\n", __func__, (unsigned long) dmabuf->priv);
#if 0
		dvr_free(dmabuf->priv);
#else
		rtd_pr_adsp_info("%s: don't free preallocate mem %lx\n", __func__, (unsigned long) dmabuf->priv);
#endif
		dmabuf->priv = NULL;
	}
	up(&rtkaudio_dmabuf_lock);
}

static struct sg_table *
rtkaudio_map_dmabuf(struct dma_buf_attachment *attachment,
		    enum dma_data_direction direction)
{
	return NULL;
}

static void rtkaudio_unmap_dmabuf(struct dma_buf_attachment *attachment,
				  struct sg_table *table,
				  enum dma_data_direction direction)
{
}

static int rtkaudio_dmabuf_mmap(struct dma_buf *dmabuf,	struct vm_area_struct *vma)
{
	void *vaddr;
	uint32_t size;
	vaddr = dmabuf->priv;
	size = PAGE_ALIGN(dmabuf->size);

	return remap_pfn_range(vma, vma->vm_start, (dvr_to_phys(vaddr) >> PAGE_SHIFT), size, vma->vm_page_prot);
}

static const struct dma_buf_ops rtkaudio_dma_buf_ops = {
	.map_dma_buf = rtkaudio_map_dmabuf,
	.unmap_dma_buf = rtkaudio_unmap_dmabuf,
	.release = rtkaudio_dmabuf_release,
	.mmap = rtkaudio_dmabuf_mmap,
};

void rtkaudio_dmabuf_preallocate_init(void) {
	prealloc_mem[0].vir_addr = NULL;
	prealloc_mem[0].size = PREALLOCATE_SIZE;
	prealloc_mem[0].vir_addr = dvr_malloc_specific(PREALLOCATE_SIZE, GFP_DCU1);

	prealloc_mem[1].vir_addr = NULL;
	prealloc_mem[1].size = PREALLOCATE_SIZE;
	prealloc_mem[1].vir_addr = dvr_malloc_specific(PREALLOCATE_SIZE, GFP_DCU1);

	rtd_pr_adsp_info("%s: malloc carved mem[0] %lx, mem[1] %lx, size %d\n",
		__func__, (unsigned long) prealloc_mem[0].vir_addr, (unsigned long) prealloc_mem[1].vir_addr, PREALLOCATE_SIZE);
}

struct dma_buf *rtkaudio_dmabuf_allocate(int size, int dev)
{
	struct dma_buf *buf = NULL;
	DEFINE_DMA_BUF_EXPORT_INFO(exp_info);
	void *vaddr = NULL;
	int i = dev - 3;

	size = PAGE_ALIGN(size);

	down(&rtkaudio_dmabuf_lock);
	if ((i > 1) || (i < 0)) {
		rtd_pr_adsp_err("%s: allocate memory failed due to device number is wrong %d\n", __func__, dev);
		BUG_ON(1);
	}

	if (size > prealloc_mem[i].size) {
		dvr_free(prealloc_mem[i].vir_addr);
		prealloc_mem[i].vir_addr = dvr_malloc_specific(PREALLOCATE_SIZE, GFP_DCU1);
		rtd_pr_adsp_info("%s: reallocate memory[%d].vaddr %lx, size %d > %d\n",
				__func__, i, (unsigned long) prealloc_mem[i].vir_addr, prealloc_mem[i].size, size);
		prealloc_mem[i].size = size;
	}

	vaddr = prealloc_mem[i].vir_addr;
	up(&rtkaudio_dmabuf_lock);

	rtd_pr_adsp_info("%s: vaddr %lx, use preallocate[%d] for dev %d\n", __func__, (unsigned long) vaddr, i, dev);
	memset(vaddr, 0, size);

	exp_info.ops = &rtkaudio_dma_buf_ops;
	exp_info.size = size;
	exp_info.flags = O_RDWR;
	exp_info.priv = vaddr;
	down(&rtkaudio_dmabuf_lock);
	buf = dma_buf_export(&exp_info);
	if (IS_ERR_OR_NULL(buf)) {
		rtd_pr_adsp_err("%s: dmabuf export err %d\n", __func__, buf);
		up(&rtkaudio_dmabuf_lock);
		goto err;
	}
	up(&rtkaudio_dmabuf_lock);

	return buf;

err:
#if 0
	if (vaddr) {
		dvr_free(vaddr);
	}
#endif
	return NULL;
}


int rtkaudio_dmabuf_fd_allocate(int size, struct dma_buf **audio_buf, int dev)
{
	int fd = -1;
	struct dma_buf *buf = NULL;
	struct dma_buf *buf2 = NULL;

	/* buf count 1 here */
	buf = rtkaudio_dmabuf_allocate(size, dev);

	if (!buf) {
		rtd_pr_adsp_err("%s: malloc dmabuf failed\n", __func__);
		return -1;
	}

	down(&rtkaudio_dmabuf_lock);
	fd = dma_buf_fd(buf, O_RDWR);

	if (fd < 0) {
		dma_buf_put(buf);
		rtd_pr_adsp_err("%s: get dmabuf fd failed\n", __func__);
		up(&rtkaudio_dmabuf_lock);
		return -EFAULT;
	}

	/* get one more time prevent release by user space (audio hal): count is 2 */
	buf2 = dma_buf_get(fd);

	if (IS_ERR_OR_NULL(buf2)) {
		rtd_pr_adsp_err("%s: get dmabuf fd failed, err %d\n", __func__, buf2);
		up(&rtkaudio_dmabuf_lock);
		return -1;
	}
	up(&rtkaudio_dmabuf_lock);

	*audio_buf = buf;

	return fd;
}
EXPORT_SYMBOL_GPL(rtkaudio_dmabuf_fd_allocate);

int rtkaudio_dmabuf_free(struct dma_buf *buf)
{
	down(&rtkaudio_dmabuf_lock);
	if (IS_ERR_OR_NULL(buf)) {
		rtd_pr_adsp_err("%s: get dmabuf fd failed, err %d\n", __func__, buf);
		up(&rtkaudio_dmabuf_lock);
		return -1;
	}

	/* user space close fd, so only need to put buf 1 time to free buffer */
	dma_buf_put(buf);
	up(&rtkaudio_dmabuf_lock);

	return 0;
}
EXPORT_SYMBOL_GPL(rtkaudio_dmabuf_free);

void* rtkaudio_dmabuf_get_vaddr(int fd)
{
	void *vaddr = NULL;
	struct dma_buf *buf = NULL;

	down(&rtkaudio_dmabuf_lock);
	buf = dma_buf_get(fd);
	if (IS_ERR_OR_NULL(buf)) {
		rtd_pr_adsp_err("%s: get dmabuf fd failed err %d\n", __func__, buf);
		up(&rtkaudio_dmabuf_lock);
		return NULL;
	}

	vaddr = buf->priv;
	dma_buf_put(buf);
	up(&rtkaudio_dmabuf_lock);

	return vaddr;
}
EXPORT_SYMBOL_GPL(rtkaudio_dmabuf_get_vaddr);

int rtkaudio_ioctl_dmabuf_mmap(int fd, struct vm_area_struct *area)
{
	int ret = 0;
	struct dma_buf *buf = NULL;

	down(&rtkaudio_dmabuf_lock);
	buf = dma_buf_get(fd);
	if (IS_ERR_OR_NULL(buf)) {
		rtd_pr_adsp_err("%s: get dmabuf fd failed err %d\n", __func__, buf);
		up(&rtkaudio_dmabuf_lock);
		return -1;
	}

	ret = buf->ops->mmap(buf, area);
	dma_buf_put(buf);
	up(&rtkaudio_dmabuf_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(rtkaudio_ioctl_dmabuf_mmap);
