/**
 * created by Realtek liangliang_song@apowertec.com
 *
 *
 * Author: liangliang_song<liangliang_song@apowertec.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include <rbus/sys_reg_reg.h>
#include <rtk_kdriver/io.h>
#include <rbus/usb2_otg_reg.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include <mach/rtk_platform.h>
#include "rtk-otg-plat.h"
#include "rtk-otg-plat-wrapper.h"
#include <dwc2/core.h>

typedef struct {
	struct platform_device *parent_dev;
	struct platform_device *dev;
}OTG_DEVICE_INFO;

static int check_platform_device_by_info(struct platform_device *dev, void *arg)
{
	OTG_DEVICE_INFO *query_info = (OTG_DEVICE_INFO *)arg;
	char *dev_name = rtk_otg_plat_get_platform_device_name(query_info->parent_dev);
	if(!dev_name || !dev->name || strcmp(dev->name, dev_name) != 0)
		return 0;
	query_info->dev =  dev;
	return 1;
}

struct each_dev_arg {
	void *data;
	int (*fn)(struct platform_device *, void *);
};

static int __each_dev(struct device *dev, void *data)
{
	struct each_dev_arg *arg = (struct each_dev_arg *)data;
	if(!dev_is_platform(dev))
		return 0;

	return arg->fn(to_platform_device(dev), arg->data);
}

static int lookup_platform_for_each_dev(void *data, int (*fn)(struct platform_device *, void *))
{
	struct each_dev_arg arg = {data, fn};

	return bus_for_each_dev(&platform_bus_type, NULL, &arg, __each_dev);
}

static struct platform_device * find_otg_device(struct platform_device *parent_dev)
{
	OTG_DEVICE_INFO query_info;
	memset(&query_info, 0, sizeof(query_info));
	query_info.parent_dev = parent_dev;
	lookup_platform_for_each_dev(&query_info, check_platform_device_by_info);
	return query_info.dev;
}

static void check_and_set_standby_flag(struct platform_device *parent_dev)
{
	struct platform_device *dev = find_otg_device(parent_dev);
	if(dev) {
		struct dwc2_hsotg *dwc2 = dev_get_drvdata(&dev->dev);
		if(dwc2 && dwc2->lx_state == DWC2_L2)
			dwc2->lx_state = DWC2_L3;
	}
}


static bool usbotg = 1;
#ifdef CONFIG_RTK_KDRV_USB_OTG_PLATFORM
static int rtk_otg_switch_handler(char *buf)
{
        char *p = NULL;
        if ((p = strstr(buf, "on")) != NULL || (p = strstr(buf, "1")) != NULL)
                usbotg = 1;
        return 0;
}
__setup("usbotg", rtk_otg_switch_handler);
#else

static int parse_otg_switch_handler(void)
{
    char strings[16] = {0};

    if(rtk_parse_commandline_equal("usbotg", strings,sizeof(strings)) == 0)
    {
        rtk_otg_plat_err("Error : can't get usbotg from bootargs\n");
        return -1;
    }

    if(strcmp(strings, "on") == 0 || strcmp(strings, "1") == 0) {
        usbotg = 1;
    }

    return 0;
}

#endif

bool rtk_otg_is_enable(void)
{
        return usbotg;
}
EXPORT_SYMBOL(rtk_otg_is_enable);

/*Otg can exported via hub or via typea directly in some platform, if so, default via hub*/
static bool g_otg_via_hub_port = true;

#ifdef CONFIG_RTK_KDRV_USB_OTG_PLATFORM
static int rtk_otg_port_handler(char *buf)
{
        char *p = NULL;
        if ((p = strstr(buf, "TYPEA")) != NULL)
                g_otg_via_hub_port = false;
	 else
                g_otg_via_hub_port = true;
        return 0;
}
__setup("usbotg_port", rtk_otg_port_handler);
#else

static int parse_otg_port_handler(void)
{
    char strings[16] = {0};

    if(rtk_parse_commandline_equal("usbotg_port", strings,sizeof(strings)) == 0)
    {
        rtk_otg_plat_err("Error : can't get usbotg_port from bootargs\n");
        return -1;
    }

    if(strcmp(strings, "TYPEA") == 0) {
        g_otg_via_hub_port = false;
    }
    else {
        g_otg_via_hub_port = true;
    }

    return 0;
}

#endif

bool rtk_otg_is_via_hub_port(void)
{
        return g_otg_via_hub_port;
}
EXPORT_SYMBOL(rtk_otg_is_via_hub_port);


/******************************Sysfs*********************************/
struct attribute rtk_otg_otg_enable_attr;
struct attribute rtk_otg_otg_remote_wake_attr;

static struct attribute *rtk_otg_default_attrs[] = {
	&rtk_otg_otg_enable_attr,
	&rtk_otg_otg_remote_wake_attr,
	NULL,
};

static void do_remote_wakeup(struct platform_device *parent_dev)
{
	struct platform_device *dev = find_otg_device(parent_dev);
	if(dev) {
		struct dwc2_hsotg *dwc2 = dev_get_drvdata(&dev->dev);
		if(dwc2 && dwc2->lx_state == DWC2_L2) {
			u32 dctl;
			/* Set Remote Wakeup Signaling */
			dctl = dwc2_readl(dwc2, DCTL);
			dctl |= DCTL_RMTWKUPSIG;
			dwc2_writel(dwc2, dctl, DCTL);
			mdelay(10);
			dctl = dwc2_readl(dwc2, DCTL);
			dctl &= (~DCTL_RMTWKUPSIG);
			dwc2_writel(dwc2, dctl, DCTL);
		}
	}
}

static ssize_t rtk_otg_attrs_show(struct kobject *kobj,
			      struct attribute *attr, char *buf)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = 
	 	(struct RTK_OTG_PRIVATE_DATA *)container_of(kobj, struct RTK_OTG_PRIVATE_DATA, kobj);
	if(attr == &rtk_otg_otg_enable_attr) {
		return snprintf(buf, PAGE_SIZE, "%u\n", private_data->is_enable);
	}
	return -1;
}

static ssize_t rtk_otg_attrs_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t count)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = 
	 	(struct RTK_OTG_PRIVATE_DATA *)container_of(kobj, struct RTK_OTG_PRIVATE_DATA, kobj);

	if (attr == &rtk_otg_otg_enable_attr) {
		unsigned int val = 0;	
		if (kstrtouint(buf, 10, &val) != 0)
			return -EINVAL;
		rtk_otg_plat_enable_otg(private_data->pdev, val ?  true : false);
	} else if (attr == &rtk_otg_otg_remote_wake_attr) {
		unsigned int val = 0;	
		if (kstrtouint(buf, 10, &val) != 0)
			return -EINVAL;
		if(val)
			do_remote_wakeup(private_data->pdev);
	}
	return count;
}


static const struct sysfs_ops rtk_otg_sysfs_ops = {
	.show   = rtk_otg_attrs_show,
	.store  = rtk_otg_attrs_store,
};

static struct kobj_type ktype_rtk_otg = {
	.release        = NULL,
	.sysfs_ops      = &rtk_otg_sysfs_ops,
	.default_attrs = rtk_otg_default_attrs,
};

#define RTK_OTG_ATTR(_name, _mode)				\
	struct attribute rtk_otg_##_name##_attr = {	\
	.name = __stringify(_name), .mode = _mode,	\
	};

RTK_OTG_ATTR(otg_enable, 0644);
RTK_OTG_ATTR(otg_remote_wake, 0644);

static int rtk_otg_plat_remove_child(struct device *dev, void *unused)
{
        struct platform_device *pdev = to_platform_device(dev);

        platform_device_unregister(pdev);

        return 0;
}

static int rtk_otg_plat_get_id_from_dts(struct platform_device *pdev, 
								unsigned int *p_id)
{
        struct device_node *p_node;

        if(!pdev || !p_id)
                return -EINVAL;

        p_node =  pdev->dev.of_node;
        if (!p_node)
                return -ENODEV;

        if(of_property_read_u32_index(p_node, "id", 0, p_id) != 0)
                return -EIO;
        return 0;
}

static int rtk_otg_plat_probe(struct platform_device *pdev)
{
	struct device           *dev = &pdev->dev;
	struct device_node      *node = dev->of_node;
	unsigned int id = 0;
	int    ret;
	struct resource *res_mem = NULL;
	struct RTK_OTG_PRIVATE_DATA *private_data = NULL;

	if(rtk_otg_plat_get_id_from_dts(pdev, &id) != 0)
		id = 0;
	
	private_data = (struct RTK_OTG_PRIVATE_DATA *)kzalloc(
			sizeof(struct RTK_OTG_PRIVATE_DATA), GFP_KERNEL);
	if(!private_data)
		return -ENOMEM;
	if(get_product_type() == PRODUCT_TYPE_DIAS) 
		private_data->is_enable = 1;
	else
		private_data->is_enable = 0;
	private_data->id = id;
	private_data->pdev = pdev;
	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(res_mem)
		private_data->io_base = res_mem->start + 0xA0000000;
	platform_set_drvdata(pdev, private_data);	

	ret = rtk_otg_initial(pdev);
	if (ret) {
		rtk_otg_plat_err("failed to rtk_otg_initial\n");
		kfree(private_data);
		return ret;
	}

	ret = kobject_init_and_add(&private_data->kobj, &ktype_rtk_otg, 
				NULL, "rtk_otg%d", private_data->id);
	if(ret != 0) {
		rtk_otg_plat_err("failed to create kobject\n");
		kfree(private_data);
		return -ENOMEM;
	}

	if (node) {
		ret = of_platform_populate(node, NULL, NULL, dev);
		if (ret) {
			rtk_otg_plat_err("failed to add drd core\n");
			kobject_put(&private_data->kobj);
			kfree(private_data);
			return ret;
		}
	} else {
		rtk_otg_plat_err("no device node, failed to add drd core\n");
		kobject_put(&private_data->kobj);
		kfree(private_data);
		ret = -ENODEV;
		return ret;
	}
	rtk_otg_plat_set_local_interrupts(pdev, true);
	rtk_otg_plat_set_global_interrupts(pdev, true);
	rtk_otg_plat_info("otg [%u] probe, io_base:%x\n", private_data->id, private_data->io_base);
	return 0;
}

static int rtk_otg_plat_remove(struct platform_device *pdev)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = NULL;	
	private_data = (struct RTK_OTG_PRIVATE_DATA *)platform_get_drvdata(pdev);

	device_for_each_child(&pdev->dev, NULL, rtk_otg_plat_remove_child);
	kobject_put(&private_data->kobj);
	rtk_otg_plat_set_local_interrupts(pdev, false);
	rtk_otg_plat_set_global_interrupts(pdev, false);
	kfree(private_data);
	return 0;
}



static const struct of_device_id rtk_otg_plat_match[] =
{
        {
                .compatible = "rtk_dwc2"
        }, {
        }
};
MODULE_DEVICE_TABLE(of, rtk_otg_plat_match);

#ifdef CONFIG_PM_SLEEP
static int rtk_otg_plat_suspend(struct device *dev)
{
        struct platform_device *pdev = container_of(dev, struct platform_device, dev);

        rtk_otg_plat_set_local_interrupts(pdev, false);
        //rtk_otg_plat_set_global_interrupts(pdev, false);
        check_and_set_standby_flag(pdev);

        return 0;
}

static int rtk_otg_plat_resume(struct device *dev)
{
        struct platform_device *pdev = container_of(dev, struct platform_device, dev);
        int    ret;

        ret = rtk_otg_initial(pdev);
        if (ret)
                return ret;
        rtk_otg_plat_set_local_interrupts(pdev, true);
        rtk_otg_plat_set_global_interrupts(pdev, true);

        return 0;
}

static const struct dev_pm_ops rtk_otg_plat_dev_pm_ops =
{
        SET_SYSTEM_SLEEP_PM_OPS(rtk_otg_plat_suspend, rtk_otg_plat_resume)
};

#define DEV_PM_OPS      (&rtk_otg_plat_dev_pm_ops)
#else
#define DEV_PM_OPS      NULL
#endif /* CONFIG_PM_SLEEP */

static struct platform_driver rtk_otg_plat_driver =
{
        .probe          = rtk_otg_plat_probe,
        .remove         = rtk_otg_plat_remove,
        .driver         = {
                .name   = "rtk_otg_plat",
                .of_match_table = rtk_otg_plat_match,
                .pm     = DEV_PM_OPS,
        },
};
MODULE_ALIAS("platform:rtk_otg_plat");

static int __init rtk_otg_plat_init(void)
{

#ifdef CONFIG_RTK_KDRV_USB_OTG_PLATFORM_MODULE
        parse_otg_switch_handler();
        parse_otg_port_handler();
#endif
        if (!usbotg)
        {
                rtk_otg_plat_err("OTG is not enable. Use command: \"usbotg on\" in bootcode to enable it! \n");
                return 0;
        }
        return platform_driver_register(&rtk_otg_plat_driver);
}

late_initcall(rtk_otg_plat_init);


static void __exit rtk_otg_plat_exit(void)
{
        if (usbotg)
                platform_driver_unregister(&rtk_otg_plat_driver);
}
module_exit(rtk_otg_plat_exit);

MODULE_DESCRIPTION("Realtek DWC2 OTG Platform Driver");
MODULE_LICENSE("GPL");

