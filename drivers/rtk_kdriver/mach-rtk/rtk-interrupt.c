#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/irq.h>
#include <linux/version.h>
#include <linux/irqchip/arm-gic-v3.h>

#include <mach/pcbMgr.h>
#include <rtk_kdriver/io.h>
#include <rtd_log/rtd_module_log.h>


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)) && defined(CONFIG_TRACEPOINTS) && defined(CONFIG_ANDROID_VENDOR_HOOKS)
#include<trace/hooks/gic_v3.h>

#if 0
static void rtk_gic_v3_affinity_init(void *p, int irq, u32 offset, u64 *affinity)
{

	return;
}
#endif
static void rtk_gic_v3_set_affinity(void *p, struct irq_data *d, const struct cpumask *mask_val,
                 u64 *affinity, bool force, void __iomem *base,
                 void __iomem *rbase, u64 redist_stride)
{
	/*	force to set GICD_IROUTER bit[31] Interrupt_Routing_Mode to 0x1.
		to support 1 of N distribution.
	*/
	if(d->irq != 32) {
		rtd_pr_mach_rtk_notice("[MIKETAG][MACH] enter rtk interrupt!!");
		*affinity = GICD_IROUTER_SPI_MODE_ANY;
	}
	return;
}

#if 0
static int rtk_gic_cpu_pm_notifier(struct notifier_block *self, unsigned long cmd, void *v)
{
	rtd_pr_mach_rtk_err("rtk_gic_cpu_pm_notifier(%x)\n", cmd);

	if (cmd == CPU_PM_EXIT)
	{
		
	}
	else if (cmd == CPU_PM_ENTER)
	{
		
	}
	else if (cmd == CPU_CLUSTER_PM_EXIT)
	{
	
	}
	else if (cmd == CPU_CLUSTER_PM_ENTER)
	{
	}
	return NOTIFY_OK;
}

static struct notifier_block rtk_gic_cpu_pm_notifier_block = {
	.notifier_call = rtk_gic_cpu_pm_notifier,
};
#endif

void interrupt_vendor_hook_init(void)
{
	int ret;
#if 0
	ret = register_trace_android_vh_gic_v3_affinity_init(rtk_gic_v3_affinity_init, NULL);
	if (ret)
		rtd_pr_mach_rtk_err("[GKI] register rtk_gic_v3_affinity_init fail\n");
#endif

	ret = register_trace_android_rvh_gic_v3_set_affinity(rtk_gic_v3_set_affinity, NULL);
	if (ret)
		 rtd_pr_mach_rtk_err("[GKI] register rtk_gic_v3_affinity_init fail\n");

	//cpu_pm_register_notifier(&rtk_gic_cpu_pm_notifier_block);
	rtd_pr_mach_rtk_warn("interrupt_vendor_hook_init finish\n");
	return;
}

#if 0
void interrupt_vendor_hook_exit(void)
{
	unregister_trace_android_vh_gic_v3_affinity_init(rtk_gic_v3_affinity_init, NULL);
	unregister_trace_android_rvh_gic_v3_set_affinity(rtk_gic_v3_set_affinity, NULL);
}
#endif

#else

void interrupt_vendor_hook_init(void)
{
	return;
}

#endif

