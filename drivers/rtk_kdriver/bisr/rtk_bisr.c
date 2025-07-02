#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mod_devicetable.h> // for of_device_id
#include <linux/platform_device.h>

/* RTK headers */
#include <rtk_kdriver/io.h> // for rtd_setbits
#include <mach/rtk_platform.h> // for get_mach_type
#include <rtd_log/rtd_module_log.h>

/* rbus headers */
#include <rbus/M8P_topbist_reg.h>

static void enable_bisr_interrupt(void)
{
	enum MACH_TYPE mach_type;

	/* Lock BISR remap bits & enable interrupts */
	mach_type = get_mach_type();
	if ((mach_type == MACH_ARCH_RTK2885P2) ||
	    (mach_type == MACH_ARCH_RTK2885PP)) {
		// Block all write to bisr remap bits
		rtd_setbits(M8P_TOPBIST_remap_lock_reg,
			    M8P_TOPBIST_remap_lock_remap_lock_mask);
	}
}

// The top-half interrupt handler function
static irqreturn_t rtk_bisr_interrupt_handler(int irq, void *dev_id)
{
	rtd_pr_bisr_info("Interrupt occurred.\n");

	// Handle the interrupt
	panic("BISR remap bits not set.\n");

	// Return IRQ_HANDLED to indicate the interrupt was successfully handled
	return IRQ_HANDLED;
}

static int bisr_probe(struct platform_device *pdev)
{
	int irq;
	int ret;

	rtd_pr_bisr_info("Probing device.\n");

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		rtd_pr_bisr_err("Failed to get IRQ number\n");
		return irq;
	}

	rtd_pr_bisr_info("Obtained IRQ number %d\n", irq);

	// Register the interrupt handler
	ret = request_irq(irq, rtk_bisr_interrupt_handler, IRQF_SHARED, "BISR",
			  (void *)(rtk_bisr_interrupt_handler));

	if (ret) {
		rtd_pr_bisr_err("Cannot register IRQ %d\n", irq);
		return ret;
	}

	rtd_pr_bisr_info("Successfully registered IRQ %d\n", irq);

	enable_bisr_interrupt();

	return 0;
}

static int bisr_remove(struct platform_device *pdev)
{
	int irq = platform_get_irq(pdev, 0);
	rtd_pr_bisr_info("Removing device.\n");
	free_irq(irq, (void *)(rtk_bisr_interrupt_handler));
	return 0;
}

static const struct of_device_id bisr_of_match[] = {
	{ .compatible = "realtek,bisr" },
	{},
};
MODULE_DEVICE_TABLE(of, bisr_of_match);

static struct platform_driver bisr_platform_driver = {
	.probe = bisr_probe,
	.remove = bisr_remove,
	.driver = {
		.name		= "rtk_bisr",
		.owner		= THIS_MODULE,
		.of_match_table = bisr_of_match,
	},
};

// Initialize the module
static int __init rtk_bisr_init(void)
{
	int ret;

	ret = platform_driver_register(&bisr_platform_driver);
	if (ret) {
		rtd_pr_bisr_emerg("Unable to register driver: %d\n", ret);
		return ret;
	}

	rtd_pr_bisr_info("Init\n");

	return 0;
}

// Cleanup the module
static void __exit rtk_bisr_exit(void)
{
	rtd_pr_bisr_info("Exit\n");
}

module_init(rtk_bisr_init);
module_exit(rtk_bisr_exit);

MODULE_LICENSE("GPL");
