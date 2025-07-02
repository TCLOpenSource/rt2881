#include <linux/module.h>
#include <rbus/mc_secure_reg.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <rtk_kdriver/io.h>
#include "rtk_hw_monitor.h"
#include "rtk_dc_mt.h"
#include "rtk_dc_mt_config.h"
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <asm/system_misc.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/syscore_ops.h>

#define MEMPRISON_SGI_ID (1+7)

int foreach_process_scan_phyaddr(unsigned long phy_start, unsigned long phy_end);

#ifdef CONFIG_RTK_KDRV_DC_MEMORY_TRASH_DETCTER
extern char* DCMT_module_str(unsigned char id);
#endif
/****************************** MEM_PRISON start **************************/
static void tee_err_handler(void)
{
	unsigned int err = rtd_inl(MC_SECURE_MC_secure_dummy_test_1_reg);
	rtd_pr_hw_monitor_debug("tee_err_handler check %x =  %x", MC_SECURE_MC_secure_dummy_test_1_reg, err);

	if(err == 0xa21cdead) {
		panic("[OPTEE]Panic in OPTEE!!\n");
	}
}

#if defined(CONFIG_ARCH_RTK2885P)
#define MC2_OFST 0x1000
#else 
#define MC2_OFST 0x800
#endif

static int system_mem_prison_trap = 0;

void memprison_err_handler(void)
{
	unsigned int errorInfo = rtd_inl(MC_SECURE_MC_secure_error_info_0_reg);
	unsigned int errorInfo1 = rtd_inl(MC_SECURE_MC_secure_error_info_1_reg);
	unsigned int dc_addr = rtd_inl(MC_SECURE_MC_secure_dummy_test_1_reg);
	unsigned char __maybe_unused module_id;
	int old_console_log;
    unsigned int dc_addr_align = (dc_addr/PAGE_SIZE)*PAGE_SIZE;

	if(system_mem_prison_trap)
		return;

	old_console_log = console_loglevel;
	if(console_loglevel < 3)
		console_loglevel = 5;


	if( errorInfo & MC_SECURE_MC_secure_error_info_0_err_latch_mask ) {
		module_id = (unsigned char) ( errorInfo & MC_SECURE_MC_secure_error_info_0_err_id_mask);
		if(system_mem_prison_trap)
			return;
		system_mem_prison_trap = 1;
        foreach_process_scan_phyaddr(dc_addr_align, dc_addr_align+PAGE_SIZE);
#ifdef CONFIG_RTK_KDRV_DC_MEMORY_TRASH_DETCTER
		panic("[MEM_PRISON]MC1:module=%s(%x,%x) %s DC_addr=0x%08x (MC1_addr=0x%08x) len=0x%x\n" ,
				DCMT_module_str(module_id), BRIDGE_ID(module_id),SUB_MODULE_ID(module_id),
				(errorInfo & MC_SECURE_MC_secure_error_info_0_err_w_mask)? "Write" : "Read",
				dc_addr,
				errorInfo1,
				(((errorInfo &  MC_SECURE_MC_secure_error_info_0_err_bl_mask ) >> MC_SECURE_MC_secure_error_info_0_err_bl_shift ) << 3));
#else
		panic("[MEM_PRISON]MC1:module=%s(%x,%x) %s DC_addr=0x%08x (MC1_addr=0x%08x) len=0x%x\n" ,
				"Unknown", BRIDGE_ID(module_id),SUB_MODULE_ID(module_id),
				(errorInfo & MC_SECURE_MC_secure_error_info_0_err_w_mask)? "Write" : "Read",
				dc_addr,
				errorInfo1,
				(((errorInfo &  MC_SECURE_MC_secure_error_info_0_err_bl_mask ) >> MC_SECURE_MC_secure_error_info_0_err_bl_shift ) << 3));
#endif
	}

#if  defined(CONFIG_ARCH_RTK2851F) ||  defined(CONFIG_ARCH_RTK2851C) || defined(CONFIG_ARCH_RTK2851A)
	//2851a 2851c only MC1
	goto handle_end;
#endif

	errorInfo = rtd_inl(MC_SECURE_MC_secure_error_info_0_reg + MC2_OFST);
	errorInfo1 = rtd_inl(MC_SECURE_MC_secure_error_info_1_reg + MC2_OFST);
	dc_addr = rtd_inl(MC_SECURE_MC_secure_dummy_test_1_reg + MC2_OFST);
    dc_addr_align = (dc_addr/PAGE_SIZE)*PAGE_SIZE;

	if( errorInfo & MC_SECURE_MC_secure_error_info_0_err_latch_mask ) {
		module_id = (unsigned char) ( errorInfo & MC_SECURE_MC_secure_error_info_0_err_id_mask);
		if(system_mem_prison_trap)
			return;
		system_mem_prison_trap = 1;
        foreach_process_scan_phyaddr(dc_addr_align, dc_addr_align+PAGE_SIZE);
#ifdef CONFIG_RTK_KDRV_DC_MEMORY_TRASH_DETCTER
		panic("[MEM_PRISON]MC2:module=%s(%x,%x) %s DC_addr=0x%08x (MC2_addr=0x%08x) len=0x%x\n" ,
				DCMT_module_str(module_id), BRIDGE_ID(module_id),SUB_MODULE_ID(module_id),
				(errorInfo & MC_SECURE_MC_secure_error_info_0_err_w_mask)? "Write" : "Read",
				dc_addr,
				errorInfo1,
				(((errorInfo &  MC_SECURE_MC_secure_error_info_0_err_bl_mask ) >> MC_SECURE_MC_secure_error_info_0_err_bl_shift ) << 3));
#else
		panic("[MEM_PRISON]MC2:module=%s(%x,%x) %s DC_addr=0x%08x (MC2_addr=0x%08x) len=0x%x\n" ,
				"Unknown", BRIDGE_ID(module_id),SUB_MODULE_ID(module_id),
				(errorInfo & MC_SECURE_MC_secure_error_info_0_err_w_mask)? "Write" : "Read",
				dc_addr,
				errorInfo1,
				(((errorInfo &  MC_SECURE_MC_secure_error_info_0_err_bl_mask ) >> MC_SECURE_MC_secure_error_info_0_err_bl_shift ) << 3));
#endif
	}

	goto handle_end;

handle_end:
	tee_err_handler();
	console_loglevel = old_console_log;
}

/****************************** MEM_PRISON end ****************************/


#ifdef CONFIG_ARM64
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))
static irqreturn_t ipi_memprison_handler(int irq, void *data)
{
	HWM_ERR("MEM PRISON TRAP\n");
	memprison_err_handler();
    if(system_mem_prison_trap){
        system_mem_prison_trap = 0;
	    return IRQ_HANDLED;
    }
    else 
        return IRQ_NONE;

}
#endif



static int rtk_mem_prison_ipi_isr_init(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))
	int ret; 
    if(nr_ipi_get() != (MEMPRISON_SGI_ID -1 )){
        HWM_ERR("[MEM PRISON] No available ipi irq for mem prison!!!\n");
        return 0;
    }
	ret = request_percpu_irq(MEMPRISON_SGI_ID, ipi_memprison_handler, "MEM PRISON", &cpu_number);
	if(ret < 0){
		HWM_ERR("irq register failed  for mem prison\n");
		return 0;
	}
    enable_percpu_irq(MEMPRISON_SGI_ID, 0);
    HWM_ERR("[MEM PRISON] irq register Success\n");
#endif
	return 0;
}

#else

static int rtk_mem_prison_ipi_isr_init(void)
{
    return 0;
}
#endif



static int rtk_mem_prison_suspend(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))
    disable_percpu_irq(MEMPRISON_SGI_ID);
#endif
    return 0;
}


static void  rtk_mem_prison_resume(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))
    enable_percpu_irq(MEMPRISON_SGI_ID, 0);
#endif
    return ;
}



static struct syscore_ops mem_prison_syscore_ops =
{
    .suspend = rtk_mem_prison_suspend,
    .resume = rtk_mem_prison_resume, 
};

int rtk_mem_prison_ipi_handle_init(void)
{
    register_syscore_ops(&mem_prison_syscore_ops);
    rtk_mem_prison_ipi_isr_init();
    return 0;
}

