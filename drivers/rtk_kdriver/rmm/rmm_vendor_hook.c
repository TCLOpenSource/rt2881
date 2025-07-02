#include <linux/version.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <linux/syscalls.h>
#include <linux/suspend.h>
#include <linux/mm.h>
#include <linux/page-isolation.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)) && defined(CONFIG_ANDROID_VENDOR_HOOKS)
#include <trace/hooks/dmabuf.h>
#include <trace/hooks/mm.h>
#endif
#include <rtd_log/rtd_module_log.h>
#include <mach/rtk_platform.h>

bool cma_sync_valve = true;
atomic_t cma_allocating;

struct block_device *global_bdev;
struct work_struct cma_sync_work_q;

void cma_sync_work(struct work_struct *work)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 108))
	ksys_sync_helper();
	rtd_pr_rmm_info("%s(): EBUSY Sync complete\n", __func__);
#else
	rtd_pr_rmm_info("%s(): cma_sync_work start\n", __func__);
	cma_sync_valve = false;
	fsync_bdev(global_bdev);
	cma_sync_valve = true;
	rtd_pr_rmm_info("%s(): cma_sync_work finish\n", __func__);
#endif
}

void cma_ebusy_sync_pinned_pages(struct inode *inode)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 108))
	schedule_work(&cma_sync_work_q);
#else
	struct super_block *sb;

	if (!IS_ERR_OR_NULL(inode) && inode->i_sb) {
		sb = inode->i_sb;
		switch (sb->s_magic) {
		case BDEVFS_MAGIC:
			global_bdev = I_BDEV(inode);
			if (!IS_ERR_OR_NULL(global_bdev)) {
#ifdef CONFIG_CMA_DEBUG
				char dev_name[BDEVNAME_SIZE];
				bdevname(global_bdev, dev_name);
				rtd_pr_rmm_debug("%s(): CMA page pinned by block device name:%s\n",
						__func__, dev_name);
#endif
//				cma_sync_valve = false;
				schedule_work(&cma_sync_work_q);
			}
			rtd_pr_rmm_debug("%s(): CMA page pinned by sb->s_magic=0x%lx\n",
					__func__, sb->s_magic);
			break;
		case EXT4_SUPER_MAGIC:
		case F2FS_SUPER_MAGIC:
		default:
			rtd_pr_rmm_debug("%s(): CMA page pinned by unhandled sb->s_magic=0x%lx\n",
					__func__, sb->s_magic);
			break;
		}
	}
#endif
}

void rmm_vh_cma_alloc_start(void)
{
//	cma_sync_valve = true;
	atomic_inc(&cma_allocating);
}

void rmm_vh_cma_alloc_finish(void)
{
//	cma_sync_valve = false;
	atomic_dec(&cma_allocating);
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 108))
void rmm_vh_cma_alloc_busy_info(void)
#else
void rmm_vh_cma_alloc_busy_info(struct acr_info *info)
#endif
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 108))
	cma_ebusy_sync_pinned_pages(NULL);
#else
	struct page *page = NULL;
	bool is_lru;
	struct address_space *mapping;
	struct inode *host;
	const struct address_space_operations *a_ops;
	struct hlist_node *dentry_first;
	unsigned long ino;

	if (info->failed_pfn && cma_sync_valve) {
		page = pfn_to_page(info->failed_pfn);

		/* 0249af9c0e0b ANDROID: mm: page_alloc: skip dump pages for freeable page
		 * Said the page will be freed by putback_movable_pages soon
		 * But code only exists in ACK kernel 5.10 but not 5.15
		 * Maybe we could remove this condition when alloc_contig_dump_pages()
		 * removes this too.
		 * */
		if (page_count(page) == 1) {
			rtd_pr_rmm_debug("%s(): The page will be freed by putback_movable_pages soon\n",
					__func__);
			return;
		}

		is_lru = !__PageMovable(page);
		if (is_lru) {
			mapping = page_mapping(page);
			if (mapping) {
				/*
				 * If mapping is an invalid pointer, we don't want to crash
				 * accessing it, so probe everything depending on it carefully.
				 */
				if (get_kernel_nofault(host, &mapping->host) ||
				    get_kernel_nofault(a_ops, &mapping->a_ops)) {
					rtd_pr_rmm_debug("invalid mapping:%px\n", mapping);
					return;
				}

				if (!host) {
					rtd_pr_rmm_debug("aops:%ps\n", a_ops);
					return;
				}

				if (get_kernel_nofault(dentry_first, &host->i_dentry.first) ||
				    get_kernel_nofault(ino, &host->i_ino)) {
					rtd_pr_rmm_debug("aops:%ps invalid inode:%px\n", a_ops, host);
					return;
				}

				cma_ebusy_sync_pinned_pages(host);
			}
		}
		
	}
#endif
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)) && defined(CONFIG_ANDROID_VENDOR_HOOKS)

#define ALLOC_CMA	0x80 /* allow allocations from CMA areas */
#define RTK_CMA_THRASHING_LIMIT 10

static void rmm_dmabuf_heap_flag_skip(void *p,
	struct dma_heap *heap, size_t len,
	unsigned int fd_flags, unsigned int heap_flags, bool *skip)
{
	if (skip)
		*skip = true;
}

/* Convert GFP flags to their corresponding migrate type */
#define GFP_MOVABLE_MASK (__GFP_RECLAIMABLE|__GFP_MOVABLE)
#define GFP_MOVABLE_SHIFT 3

static inline int rtk_gfp_migratetype(const gfp_t gfp_flags)
{
	return (gfp_flags & GFP_MOVABLE_MASK) >> GFP_MOVABLE_SHIFT;
}
#undef GFP_MOVABLE_MASK
#undef GFP_MOVABLE_SHIFT

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 108)) // for AN14+
static void rtk_cma_selector(void *data, gfp_t gfp_mask,
		unsigned int *alloc_flags, bool *bypass)
{
	static unsigned long last_select_jiffies = 0;
	static unsigned long init_ws_refault = 0, base_file_lru = 0, thrashing = 0;
	static bool select_valve = false;

	static char vold_comm_str[20] = {0};
	static int  vold_pid = 0;

#ifdef CONFIG_ARCH_RTK6748
	if (is_ddr_unbalance)
		return;
#endif
	if (gfp_mask & __GFP_CMA) // original
		return;

	if (strncmp(current->comm, "tvservice", 9) == 0) { 	//FIXME! MMAP1-1668 for tracking
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "tvservice", 9) == 0)) {
		return;
	}

	if (strncmp(current->comm, "rvice.tvmidware", 15) == 0) { 	//FIXME! RT75PGTV-1193 for tracking
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "rvice.tvmidware", 15) == 0)) {
		return;
	}

	if (strncmp(current->comm, "rs.media.module", 15) == 0) { 	//FIXME! RT75PGTV-2830 for tracking
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "rs.media.module", 15) == 0)) {
		return;
	}

	if (strncmp(current->comm, "NvrHungAnalyse_", 15) == 0) { 	//TCL
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "NvrHungAnalyse_", 15) == 0)) {
		return;
	}

	if (strncmp(current->comm, "TclPqTP_", 8) == 0) { 	//TCL
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "TclPqTP_", 8) == 0)) {
		return;
	}

	if (strncmp(current->comm, "heserver", 8) == 0) { 	//TCL
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "heserver", 8) == 0)) {
		return;
	}

	if (strncmp(current->comm, "com.tcl.tvinput", 15) == 0) { 	//TCL
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "com.tcl.tvinput", 15) == 0)) {
		return;
	}

        if (strncmp(current->comm, "DataMgr", 7) == 0) { 	//TCL
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "DataMgr", 7) == 0)) {
		return;
	}

        if (strncmp(current->comm, "binder", 6) == 0) { 	//TCL
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "binder", 6) == 0)) {
		return;
	}

	if (strncmp(current->comm, "tvmgr_main", 10) == 0) { 	//TCL
		return;
	} else if (current->group_leader && (strncmp(current->group_leader->comm, "tvmgr_main", 10) == 0)) {
		return;
	}

	if (strcmp(current->comm, "vold") == 0) { 	//FIXME! MAC8QQC-3839 for tracking
		if (vold_pid != current->tgid) {
			rtd_pr_rmm_err("%s: change vold pid(%d->%d)\n", __func__, vold_pid, current->tgid);
			vold_pid = current->tgid;
			snprintf(vold_comm_str, sizeof(vold_comm_str), "binder:%d", vold_pid);
		}
		return;
	} else if (vold_comm_str[0] != 0 && strncmp(current->comm, vold_comm_str, 10) == 0) {
		return;
	}

	if (rtk_gfp_migratetype(gfp_mask) == MIGRATE_MOVABLE) {
		if(time_after(jiffies, (last_select_jiffies + HZ))) {
			/* Time to calculate page thrashing stat */
			last_select_jiffies = jiffies;
			select_valve = true;
			if (select_valve) {
				select_valve = false;

				if (init_ws_refault) {
					thrashing = ((global_node_page_state(WORKINGSET_REFAULT_FILE) -
							init_ws_refault) * 100) / (base_file_lru + 1);
					rtd_pr_rmm_debug("%s(): thrashing=%lu limit=%u cma_allocating=%d\n",
							__func__, thrashing, RTK_CMA_THRASHING_LIMIT,
							atomic_read(&cma_allocating));
				}
				init_ws_refault = global_node_page_state(WORKINGSET_REFAULT_FILE);
				base_file_lru = global_node_page_state(NR_ACTIVE_FILE) +
						global_node_page_state(NR_INACTIVE_FILE);
			}
		}
		
		*bypass = true;
		if (thrashing > RTK_CMA_THRASHING_LIMIT)
			*alloc_flags |= ALLOC_CMA;
		else if (!atomic_read(&cma_allocating) && thrashing)
			*alloc_flags |= ALLOC_CMA;
		else if (atomic_read(&cma_allocating) && gfp_mask == (GFP_USER|__GFP_RETRY_MAYFAIL|__GFP_MOVABLE))
			*alloc_flags &= ~ALLOC_CMA;
	}
}
#endif

int rmm_vendor_hook_init(void)
{
	int ret = 0;

	ret = register_trace_android_vh_dmabuf_heap_flags_validation(rmm_dmabuf_heap_flag_skip, NULL);
	if (ret)
		goto err_out;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 108)) // for AN14+
	ret = register_trace_android_vh_calc_alloc_flags(rtk_cma_selector, NULL);
	if (ret)
		goto err_out;
#endif

	return 0;

err_out:
	unregister_trace_android_vh_dmabuf_heap_flags_validation(rmm_dmabuf_heap_flag_skip, NULL);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 108)) // for AN14+
	unregister_trace_android_vh_calc_alloc_flags(rtk_cma_selector, NULL);
#endif

	return -EINVAL;
}

void rmm_vendor_hook_exit(void)
{
	unregister_trace_android_vh_dmabuf_heap_flags_validation(rmm_dmabuf_heap_flag_skip, NULL);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 108)) // for AN14+
	unregister_trace_android_vh_calc_alloc_flags(rtk_cma_selector, NULL);
#endif
}
#else
int rmm_vendor_hook_init(void) { return 0; }
void rmm_vendor_hook_exit(void) { }
#endif
