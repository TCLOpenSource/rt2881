#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include "rtk_scd.h"
#include <linux/platform_device.h>
#include <rtk_otp_region_api.h>
#include <mach/rtk_platform.h>

#include "rtk_scd_dev.h"
#include "rtk_scd_sysfs.h"

extern int scd_module_init(void);
extern void scd_module_exit(void);

static scd_device *scd_device_list[MAX_SCD_DEVICE] = { 0 };
#ifdef CONFIG_RTK_KDRV_MIO
extern int get_mio_enable_status(void);
#endif


/*------------------------------------------------------------------
 * Func : scd_bus_match
 *
 * Desc : scd dev exit function
 *
 * Parm : dev    : device
 *        driver : driver
 *
 * Retn : 1 : device & driver matched, 0 : device & driver not matched
 *------------------------------------------------------------------*/
int scd_bus_match(struct device *dev, struct device_driver *drv)
{
	/* scd bus have no idea to match device & driver, return 1 to pass all*/
	return 1;
}

struct bus_type scd_bus_type = {
	.name = "smartcard",
	.match = scd_bus_match,
};

#ifdef CONFIG_PM
static struct platform_device *smc_platform_dev[MAX_SCD_DEVICE];


/*------------------------------------------------------------------
 * Func : smc_pm_suspend
 *
 * Desc : suspend scd dev
 *
 * Parm : dev    : device to be suspend 
 *        state  : event : power state
 *               PM_EVENT_ON      0
 *               PM_EVENT_FREEZE  1
 *               PM_EVENT_SUSPEND 2
 *         
 * Retn : 0
 *------------------------------------------------------------------*/
static int smc_pm_suspend(struct platform_device *dev, pm_message_t state)
{
	scd_device* p_dev = scd_device_list[dev->id];
	scd_driver* p_drv = to_scd_driver(p_dev->dev.driver);
	SC_INFO(" smart card device suspend!'\n");
	return (p_drv->suspend) ? p_drv->suspend(p_dev) : 0;
}


/*------------------------------------------------------------------
 * Func : smc_pm_resume
 *
 * Desc : resume scd bus
 *
 * Parm : dev    : device to be resumed 
 *         
 * Retn : 0
 *------------------------------------------------------------------*/
static int smc_pm_resume(struct platform_device *dev)
{
	scd_device* p_dev = scd_device_list[dev->id];
	scd_driver* p_drv = to_scd_driver(p_dev->dev.driver);
	SC_INFO("smart card device resume!\n");
	return (p_drv->resume) ? p_drv->resume(p_dev) : 0;
}
#endif

static struct platform_driver smc_platform_drv = {
#ifdef CONFIG_PM
	.suspend = smc_pm_suspend,
	.resume = smc_pm_resume,
#endif
	.driver = {
		   .name = "SMARTCARD",
		   .bus = &platform_bus_type,
		   }
};


/*------------------------------------------------------------------
 * Func : register_scd_device
 *
 * Desc : register scd device
 *
 * Parm : device    : scd device to be registered
 *
 * Retn : N/A
 *------------------------------------------------------------------*/
void scd_device_release(struct device *dev)
{
	scd_device *p_dev = to_scd_device(dev);

	SC_INFO("scd dev %s released\n", p_dev->name);
}

/*******************************************************************/
/*                                                                          */
/*                           scd_device                                */
/*                                                                          */
/*******************************************************************/


/*------------------------------------------------------------------
 * Func : register_scd_device
 *
 * Desc : register scd device
 *
 * Parm : device    : scd device to be registered
 *
 * Retn : 0
 *------------------------------------------------------------------*/
int register_scd_device(scd_device *device)
{
	struct device *dev = &device->dev;
	int i;

	for (i = 0; i < MAX_SCD_DEVICE; i++) {
		if (scd_device_list[i] == NULL) {
			SC_INFO("register scd device '%s' (%p) to scd%d\n",
				device->name, dev, i);

			dev_set_name(dev, "scd%d", i);

			dev->bus = &scd_bus_type;

			dev->release = scd_device_release;

			if (device_register(dev) < 0) {
				SC_INFO
				    ("register scd device '%s' (%p) to scd%d failed\n",
				     device->name, dev, i);
				return -1;
			}

			scd_device_list[i] = device;

#ifdef CONFIG_PM
			smc_platform_dev[i] =
			platform_device_register_simple("SMARTCARD", i, NULL, 0);
#endif /*  */
			return 0;
		}
	}

	SC_INFO("register scd device '%s' (%p) failed, no more free entry\n",
		device->name, dev);

	return -1;
}

/*------------------------------------------------------------------
 * Func : unregister_scd_device
 *
 * Desc : unregister scd device
 *
 * Parm : device : scd device to be unregistered
 *
 * Retn : N/A
 *------------------------------------------------------------------*/
void unregister_scd_device(scd_device *device)
{
	int i;

	for (i = 0; i < MAX_SCD_DEVICE; i++) {
		if (scd_device_list[i] == device) {
			device_unregister(&device->dev);
			scd_device_list[i] = NULL;
#ifdef CONFIG_PM
			platform_device_unregister(smc_platform_dev[i]);
#endif
		}
	}
}

/*******************************************************************/
/*                                                                          */
/*                           scd_driver                                */
/*                                                                          */
/*******************************************************************/
/*------------------------------------------------------------------
 * Func : scd_drv_probe
 *
 * Desc : probe scd device
 *
 * Parm : dev : scd device to be probed
 *
 * Retn : 0 : if supportted, others : failed
 *------------------------------------------------------------------*/
int scd_drv_probe(struct device *dev)
{
	scd_device *p_dev = to_scd_device(dev);
	scd_driver *p_drv = to_scd_driver(dev->driver);
	SC_INFO("probe : scd_dev '%s' (%p), scd_drv '%s' (%p)\n", p_dev->name,
		dev, p_drv->name, dev->driver);

	if (!p_drv->probe)
		return -ENODEV;

	if (p_drv->probe(p_dev) == 0) {
		create_scd_dev_node(p_dev);
		rtk_scd_sysfs_create(dev);
		return 0;
	}

	return -ENODEV;
}

/*------------------------------------------------------------------
 * Func : scd_drv_remove
 *
 * Desc : this function was used to inform the scd driver that a scd
 *        device has been removed
 *
 * Parm : dev : scd device to be removed
 *
 * Retn : 0 for success, others failed
 *------------------------------------------------------------------*/
int scd_drv_remove(struct device *dev)
{
	scd_device *p_dev = to_scd_device(dev);
	scd_driver *p_drv = to_scd_driver(dev->driver);

	SC_INFO("remove smart card device '%s'\n", p_dev->name);
	rtk_scd_sysfs_destroy(dev);

	if (p_drv->remove)
		p_drv->remove(p_dev);

	remove_scd_dev_node(p_dev);

	return 0;
}

/*------------------------------------------------------------------
 * Func : scd_drv_shutdown
 *
 * Desc : this function was used to short down a scd device
 *
 * Parm : dev : scd device to be shut down
 *
 * Retn : 0 for success, others failed
 *------------------------------------------------------------------*/
void scd_drv_shutdown(struct device *dev)
{
	scd_device *p_dev = to_scd_device(dev);
	scd_driver *p_drv = to_scd_driver(dev->driver);

	SC_INFO("shotdown smart card device '%s'\n", p_dev->name);

	if (p_drv->enable)
		p_drv->enable(p_dev, 0);
}

/*------------------------------------------------------------------
 * Func : scd_drv_suspend
 *
 * Desc : this function was used to suspend a scd device
 *
 * Parm : dev : scd device to be suspend
 *
 * Retn : 0 for success, others failed
 *------------------------------------------------------------------*/
int scd_drv_suspend(struct device *dev, pm_message_t state)
{
	scd_device *p_dev = to_scd_device(dev);
	scd_driver *p_drv = to_scd_driver(dev->driver);

	SC_INFO("suspend scd_dev '%s'\n", p_dev->name);
	return (p_drv->suspend) ? p_drv->suspend(p_dev) : 0;
}

/*------------------------------------------------------------------
 * Func : scd_drv_resume
 *
 * Desc : this function was used to resume a scd device
 *
 * Parm : dev : scd device to be suspend
 *
 * Retn : 0 for success, others failed
 *------------------------------------------------------------------*/
int scd_drv_resume(struct device *dev)
{
	scd_device *p_dev = to_scd_device(dev);
	scd_driver *p_drv = to_scd_driver(dev->driver);

	SC_INFO("resume scd_dev '%s'\n", p_dev->name);

	return (p_drv->resume) ? p_drv->resume(p_dev) : 0;
}

/*------------------------------------------------------------------
 * Func : register_scd_driver
 *
 * Desc : register scd device driver
 *
 * Parm : driver    : scd device driver to be registered
 *
 * Retn : 0
 *------------------------------------------------------------------*/
int register_scd_driver(scd_driver *driver)
{
	struct device_driver *drv = &driver->drv;

	drv->name = driver->name;
	drv->bus = &scd_bus_type;
	drv->probe = scd_drv_probe;
	drv->remove = scd_drv_remove;
	drv->shutdown = scd_drv_shutdown;
	drv->suspend = scd_drv_suspend;
	drv->resume = scd_drv_resume;

	SC_INFO("register scd driver '%s' (%p)\n", drv->name, drv);

	return driver_register(drv);
}

/*------------------------------------------------------------------
 * Func : unregister_scd_driver
 *
 * Desc : unregister scd driver
 *
 * Parm : driver : scd driver to be unregistered
 *
 * Retn : N/A
 *------------------------------------------------------------------*/
void unregister_scd_driver(scd_driver *driver)
{
	struct device_driver *drv = &driver->drv;
	SC_INFO("unregister scd driver '%s' (%p)\n", drv->name, &driver->drv);
	driver_unregister(&driver->drv);
}

/*------------------------------------------------------------------
 * Func : scd_dev_module_init
 *
 * Desc : scd dev init function
 *
 * Parm : N/A
 *
 * Retn : 0 : success, others fail
 *------------------------------------------------------------------*/
static int __init scd_core_init(void)
{
	int ret;
	SC_NUM index = SC_INTERAL;
	SC_INFO("%s, register scd_bus %p\n", __FUNCTION__, &scd_bus_type);
	if (get_product_type() == PRODUCT_TYPE_DIAS) {
		SC_ERR("smartcard not support on dias platform\n");
		return 0;
	}

#ifdef CONFIG_RTK_KDRV_MIO_SMARTCARD
#ifdef CONFIG_ARCH_RTK6702
#else
	index = SC_MIO;
#endif
#endif
	if ( rtk_is_sc_disable(index) ) {
		SC_INFO("module_init :  sc%d otp disabled\n", index);
		return 0;
	}

#ifdef CONFIG_RTK_KDRV_MIO_SMARTCARD
	if(!get_mio_enable_status()){
		SC_ERR("no connect mio , smartcard driver will skip \n");
		SC_ERR("no connect mio , smartcard driver will skip \n");
		SC_ERR("no connect mio , smartcard driver will skip \n");
		return 0;
	}
#endif 

	scd_dev_module_init();

#ifdef CONFIG_PM
	platform_driver_register(&smc_platform_drv);	/* regist mcp driver */
#endif /* */
	ret = bus_register(&scd_bus_type);/*register scd bus type*/
	if(ret != 0){
		SC_ERR("bus_register failed\n");
		return -1;
	}
	return scd_module_init(); /*register scd interface*/
}

/*------------------------------------------------------------------
 * Func : scd_dev_module_exit
 *
 * Desc : scd dev module exit function
 *
 * Parm : N/A
 *
 * Retn : 0 : success, others fail
 *------------------------------------------------------------------*/
static void __exit scd_core_exit(void)
{
	scd_dev_module_exit();

	bus_unregister(&scd_bus_type);	/* unregister scd bus*/

	scd_module_exit(); /* unregister scd interface*/
#ifdef CONFIG_PM
	platform_driver_unregister(&smc_platform_drv);
#endif /*  */
}

module_init(scd_core_init);
module_exit(scd_core_exit);

EXPORT_SYMBOL(register_scd_device);
EXPORT_SYMBOL(unregister_scd_device);

EXPORT_SYMBOL(register_scd_driver);
EXPORT_SYMBOL(unregister_scd_driver);
