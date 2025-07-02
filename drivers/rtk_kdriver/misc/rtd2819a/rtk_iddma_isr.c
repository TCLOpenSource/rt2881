#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/platform.h>

#include <rbus/sys_reg_reg.h>
#include <rbus/h3ddma_reg.h>
#include <rbus/h4ddma_reg.h>
#include <rbus/h5ddma_reg.h>
#include <rbus/h6ddma_reg.h>

#include <rtd_log/rtd_module_log.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/i3ddma/i3ddma_reg_array.h>
#include <tvscalercontrol/io/ioregdrv.h>

#ifdef CONFIG_OF
#include <linux/of_irq.h>
#include <linux/of_address.h>
#endif

extern unsigned char IDDMA_Set_CapStartAddr(unsigned char dev);
static int iddma_isr_suspend(struct platform_device *dev, pm_message_t state);
static int iddma_isr_resume(struct platform_device *dev);

extern IDDMA_DEV_ATTR_T iddma_attr[];

#ifdef CONFIG_OF
static const struct of_device_id of_iddma_isr_ids[] = {
	{ .compatible = "realtek,iddma_isr" },
	{}
};

MODULE_DEVICE_TABLE(of, of_iddma_isr_ids);
#endif

static struct platform_device *iddma_isr_platform_devs = NULL;
static struct platform_driver iddma_isr_platform_driver = {
#ifdef CONFIG_PM
	.suspend		= iddma_isr_suspend,
	.resume			= iddma_isr_resume,
#endif
	.driver = {
		.name		= "iddma_isr",
		.bus		= &platform_bus_type,
#ifdef CONFIG_OF
		.of_match_table = of_iddma_isr_ids,
#endif
	},
};

#ifdef CONFIG_PM
static int iddma_isr_suspend(struct platform_device *dev, pm_message_t state)
{
	return 0;
}

static int iddma_isr_resume(struct platform_device *dev)
{
    IoReg_Write32(SYS_REG_INT_CTRL_SCPU_2_reg, SYS_REG_INT_CTRL_SCPU_2_i4dma_int_scpu_routing_en_mask |
						SYS_REG_INT_CTRL_SCPU_2_i5dma_int_scpu_routing_en_mask | 
						SYS_REG_INT_CTRL_SCPU_2_i6dma_int_scpu_routing_en_mask | 
						SYS_REG_INT_CTRL_SCPU_2_write_data_mask);

	IoReg_Write32(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_dispipre4_mask | SYS_REG_SYS_SRST6_rstn_dispipre5_mask | 
						SYS_REG_SYS_SRST6_rstn_dispipre6_mask | SYS_REG_SYS_SRST6_write_data_mask);

	rtd_pr_vsc_notice("[IDDMA_ISR] resume done\n");

	return 0;
}

#endif

unsigned char handle_I4_cap1_last_write_flag = 0;
unsigned char handle_I5_cap1_last_write_flag = 0;
unsigned char handle_I6_cap1_last_write_flag = 0;

//unsigned char handle_I4_vs_rising_flag = 0;
//unsigned char handle_I5_vs_rising_flag = 0;
//unsigned char handle_I6_vs_rising_flag = 0;

irqreturn_t iddma_isr(int irq, void *dev_id)
{
	unsigned char ret = IRQ_NONE;
	h4ddma_i4dma_interrupt_enable_RBUS h4ddma_i4dma_interrupt_enable_reg;
	h4ddma_i4dma_cap1_cap_status_RBUS h4ddma_i4dma_cap1_cap_status_reg;
	h5ddma_i5dma_interrupt_enable_RBUS h5ddma_i5dma_interrupt_enable_reg;
	h5ddma_i5dma_cap1_cap_status_RBUS h5ddma_i5dma_cap1_cap_status_reg;
	h6ddma_i6dma_interrupt_enable_RBUS h6ddma_i6dma_interrupt_enable_reg;
	h6ddma_i6dma_cap1_cap_status_RBUS h6ddma_i6dma_cap1_cap_status_reg;

	h4ddma_i4dma_cap2_cap_status_RBUS h4ddma_i4dma_cap2_cap_status_reg;
	h5ddma_i5dma_cap2_cap_status_RBUS h5ddma_i5dma_cap2_cap_status_reg;
	h6ddma_i6dma_cap2_cap_status_RBUS h6ddma_i6dma_cap2_cap_status_reg;
	
	h4ddma_i4dma_interrupt_enable_reg.regValue = IoReg_Read32(H4DDMA_I4DMA_Interrupt_Enable_reg);
	h4ddma_i4dma_cap1_cap_status_reg.regValue = IoReg_Read32(H4DDMA_I4DMA_CAP1_Cap_Status_reg);
	h4ddma_i4dma_cap2_cap_status_reg.regValue = IoReg_Read32(H4DDMA_I4DMA_CAP2_Cap_Status_reg);
	handle_I4_cap1_last_write_flag = h4ddma_i4dma_cap1_cap_status_reg.i4_cap1_cap_last_wr_flag || h4ddma_i4dma_cap2_cap_status_reg.i4_cap2_cap_last_wr_flag;

	h5ddma_i5dma_interrupt_enable_reg.regValue = IoReg_Read32(H5DDMA_I5DMA_Interrupt_Enable_reg);
	h5ddma_i5dma_cap1_cap_status_reg.regValue = IoReg_Read32(H5DDMA_I5DMA_CAP1_Cap_Status_reg);
	h5ddma_i5dma_cap2_cap_status_reg.regValue = IoReg_Read32(H5DDMA_I5DMA_CAP2_Cap_Status_reg);
	handle_I5_cap1_last_write_flag = h5ddma_i5dma_cap1_cap_status_reg.i5_cap1_cap_last_wr_flag || h5ddma_i5dma_cap2_cap_status_reg.i5_cap2_cap_last_wr_flag;

	h6ddma_i6dma_interrupt_enable_reg.regValue = IoReg_Read32(H6DDMA_I6DMA_Interrupt_Enable_reg);
	h6ddma_i6dma_cap1_cap_status_reg.regValue = IoReg_Read32(H6DDMA_I6DMA_CAP1_Cap_Status_reg);
	h6ddma_i6dma_cap2_cap_status_reg.regValue = IoReg_Read32(H6DDMA_I6DMA_CAP2_Cap_Status_reg);
	handle_I6_cap1_last_write_flag = h6ddma_i6dma_cap1_cap_status_reg.i6_cap1_cap_last_wr_flag || h6ddma_i6dma_cap2_cap_status_reg.i6_cap2_cap_last_wr_flag;

	if(h4ddma_i4dma_interrupt_enable_reg.i4_cap1_last_wr_ie && handle_I4_cap1_last_write_flag)
	{
		IDDMA_Set_CapStartAddr(I4DDMA_DEV);
		handle_I4_cap1_last_write_flag = 0;
		IoReg_SetBits(H4DDMA_I4DMA_CAP1_Cap_Status_reg, H4DDMA_I4DMA_CAP1_Cap_Status_i4_cap1_cap_last_wr_flag_mask);
		IoReg_SetBits(H4DDMA_I4DMA_CAP2_Cap_Status_reg, H4DDMA_I4DMA_CAP2_Cap_Status_i4_cap2_cap_last_wr_flag_mask);
		ret = IRQ_HANDLED;
	}

	if(h5ddma_i5dma_interrupt_enable_reg.i5_cap1_last_wr_ie && handle_I5_cap1_last_write_flag)
	{
		IDDMA_Set_CapStartAddr(I5DDMA_DEV);
		handle_I5_cap1_last_write_flag = 0;
		IoReg_SetBits(H5DDMA_I5DMA_CAP1_Cap_Status_reg, H5DDMA_I5DMA_CAP1_Cap_Status_i5_cap1_cap_last_wr_flag_mask);
		IoReg_SetBits(H5DDMA_I5DMA_CAP2_Cap_Status_reg, H5DDMA_I5DMA_CAP2_Cap_Status_i5_cap2_cap_last_wr_flag_mask);
		ret = IRQ_HANDLED;
	}

	if(h6ddma_i6dma_interrupt_enable_reg.i6_cap1_last_wr_ie && handle_I6_cap1_last_write_flag)
	{
		IDDMA_Set_CapStartAddr(I6DDMA_DEV);
		handle_I6_cap1_last_write_flag = 0;
		IoReg_SetBits(H6DDMA_I6DMA_CAP1_Cap_Status_reg, H6DDMA_I6DMA_CAP1_Cap_Status_i6_cap1_cap_last_wr_flag_mask);
		IoReg_SetBits(H6DDMA_I6DMA_CAP2_Cap_Status_reg, H6DDMA_I6DMA_CAP2_Cap_Status_i6_cap2_cap_last_wr_flag_mask);
		ret = IRQ_HANDLED;
	}

	return ret;
}

static int irq_iddma_isr = -1;
static int __init iddma_isr_init_irq(struct platform_device *pdev)
{
	int irq;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
	{
		rtd_pr_vsc_err("iddma_isr: cannot get irq %d\n", irq);
		return irq;
	}

	/* Request IRQ */
	if(request_irq(irq,
                   iddma_isr,
                   IRQF_SHARED,
                   "IDDMA ISR",
                   (void *)pdev))
	{
		rtd_pr_vsc_err("iddma_isr: cannot register IRQ %d\n", irq);
		return -ENXIO;
	}

	irq_iddma_isr = irq;

	// enable route to SCPU
	IoReg_Write32(SYS_REG_INT_CTRL_SCPU_2_reg, SYS_REG_INT_CTRL_SCPU_2_i4dma_int_scpu_routing_en_mask |
							SYS_REG_INT_CTRL_SCPU_2_i5dma_int_scpu_routing_en_mask | 
							SYS_REG_INT_CTRL_SCPU_2_i6dma_int_scpu_routing_en_mask | 
							SYS_REG_INT_CTRL_SCPU_2_write_data_mask);

	IoReg_Write32(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_dispipre4_mask | SYS_REG_SYS_SRST6_rstn_dispipre5_mask | 
						SYS_REG_SYS_SRST6_rstn_dispipre6_mask | SYS_REG_SYS_SRST6_write_data_mask);

	rtd_pr_vsc_info("iddma_isr: register IRQ virq:%d hwirq:%lu\n",
			irq, irqd_to_hwirq(irq_get_irq_data(irq)));

	return 0;
}

int __init iddma_isr_probe(struct platform_device *pdev)
{
	int result;

	result = iddma_isr_init_irq(pdev);
	if (result < 0) {
		rtd_pr_vsc_err("iddma_isr: can not register irq...\n");
		return result;
	}

	iddma_isr_platform_devs = pdev;

	return 0;
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
int __init iddma_isr_init_module(void)
#else
static int __init iddma_isr_init_module(void)
#endif
{

	int result;

	if (platform_driver_probe(&iddma_isr_platform_driver, iddma_isr_probe) != 0) {
                rtd_pr_vsc_info("iddma_isr: can not register platform driver...\n");
                result = -EINVAL;
                return result;
        }

	return 0;

}

void __exit iddma_isr_exit_module(void)
{
	if (irq_iddma_isr > -1) {
		free_irq(irq_iddma_isr, iddma_isr_platform_devs);
		irq_iddma_isr = -1;
	}
	platform_driver_unregister(&iddma_isr_platform_driver);
	iddma_isr_platform_devs = NULL;
}


#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(iddma_isr_init_module);
module_exit(iddma_isr_exit_module);
#endif