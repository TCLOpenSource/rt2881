#ifndef BUILD_QUICK_SHOW
#include <linux/version.h>
#include <rtd_log/rtd_module_log.h>
#include <rtk_kdriver/io.h>

inline bool __weak carvedout_enable(void)
{
	rtd_pr_rmm_err("weak carvedout_enable");
	return true;
}
EXPORT_SYMBOL(carvedout_enable);

#endif /* BUILD_QUICK_SHOW */
