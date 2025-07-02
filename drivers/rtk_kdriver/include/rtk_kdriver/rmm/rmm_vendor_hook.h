#ifndef _RMM_VENDOR_HOOK_H
#define _RMM_VENDOR_HOOK_H

int rmm_vendor_hook_init(void);
void rmm_vendor_hook_exit(void);

void rmm_vh_cma_alloc_start(void);
void rmm_vh_cma_alloc_finish(void);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 108))
void rmm_vh_cma_alloc_busy_info(void);
#else
struct acr_info;
void rmm_vh_cma_alloc_busy_info(struct acr_info *info);
#endif

#endif /* _RMM_VENDOR_HOOK_H */
