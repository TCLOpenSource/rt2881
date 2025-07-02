/*
 *Copyright (C) 2007-2013 Realtek Semiconductor Corporation.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/moduleparam.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/uaccess.h>        /* copy_*_user */
#include <asm/io.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk_gpio.h>
#ifdef CONFIG_OF
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#endif
#include "rbus/emcu_reg.h"
#include "rtk_kdv_emcu.h"


static int irq;

static irqreturn_t rtk_emcu_isr(int this_irq, void *dev_id)
{
    unsigned int  rbus_address = 0;
	unsigned int  rbus_value   = 0;		
	unsigned int  verify_value = 0;
	unsigned int  checksum     = 0;
	
	EMCU_DEBUG("rtk_emcu_isr\n");
		
	if (rtd_inl(EMCU_CPU_INT2_reg) & EMCU_CPU_INT2_int_ms_mask) {
			
        if ((rtd_inl(0xB80605F4) & 0xFFFF) == 0x2379){
			
			rbus_address = rtd_inl(0xB8060584);
			rbus_value   = rtd_inl(0xB8060588);
			verify_value = rtd_inl(0xB806058c);
			checksum     = ((rbus_address + rbus_value) ^ 0xffffffff);
						
			if (checksum == verify_value) {
				
				EMCU_ALERT("[GPIO]->rtd_outl(%08x, %08x)\n", rbus_address, rbus_value);	
				rtd_outl((rbus_address | 0xb0000000), rbus_value);

			} else {
			    EMCU_ALERT("checksum fail %08x, should be %08x\n", checksum, verify_value); 	
			}												
        }   
		rtd_outl(0xB80605F4, 0);
		rtd_outl(EMCU_CPU_INT2_reg, EMCU_CPU_INT2_int_ms_mask);
        return IRQ_HANDLED;
		
    } else {
		
        return IRQ_NONE;
	
	}
}

static int rtk_emcu_probe(struct platform_device *pdev)
{
    irq = platform_get_irq(pdev, 0);
    if (irq < 0) {
        EMCU_WARNING("rtk_emcu_probe get_irq failed \n");
        return irq;
    }
    EMCU_WARNING("rtk_emcu_probe get_irq %d\n\n", irq);
    if (request_irq(irq, rtk_emcu_isr, IRQF_SHARED, "EMCU_ISR", rtk_emcu_isr) < 0) {
        EMCU_WARNING("rtk emcu request irq failed - \n");
        return -ENODEV;
    }

    return 0;
}

static int rtk_emcu_remove(struct platform_device *pdev)
{
    int irq;

    irq = platform_get_irq(pdev, 0);

    if ((irq >= 0)) {
        free_irq(irq, (void *)pdev);
    }

    return 0;
}

static const struct of_device_id of_rtk_emcu_ids[] = {
    { .compatible = "realtek,emcu" },
    {}
};
MODULE_DEVICE_TABLE(of, of_rtk_emcu_ids);

static struct platform_driver emcu_platform_drv = {
    .probe      = rtk_emcu_probe,
    .remove     = rtk_emcu_remove,
    .driver   = {
        .name = "EMCU_ISR",
        .bus  = &platform_bus_type,
        .of_match_table = of_match_ptr(of_rtk_emcu_ids),
    }
};

int register_emcu_isr_device(void)
{
    int ret;

    EMCU_WARNING("register_emcu_isr_device\n");
    ret = platform_driver_register(&emcu_platform_drv); /* regist mio driver */
    return ret;
}

int unregister_emcu_isr_device(void)
{
    platform_driver_unregister(&emcu_platform_drv); /* regist mio driver */
    return 0;
}
