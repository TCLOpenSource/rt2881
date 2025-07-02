#ifndef __RTK_OSDEXT_LINUX_H__
#define __RTK_OSDEXT_LINUX_H__


#include "rtk_osdExt_intr.h"

int GDMAExt_init_os_interrupt( struct platform_device* pdev, int idx, GDMA_IRQ_handler_funcp funcp  );

int GDMAExt_init_module(void);
void GDMAExt_cleanup_module(void);
bool GDMAExt_init_debug_proc(void);
void GDMAExt_uninit_debug_proc(void);

#endif//__RTK_OSDEXT_LINUX_H__
