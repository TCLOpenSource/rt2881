/*
 * rtk_composite.c -- RTK Composite driver
 *
 * Based on hid.c
 *
 * Copyright (C) 2021 liangliang_song <liangliang_song@apowerte.com>
 */


#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb/composite.h>
#include <linux/usb/g_hid.h>
#include <mach/rtk_platform.h>
#include "rtk_composite_platform.h"
#include "rtk_hidg_dev_info.h"


struct hidg_func_node {
	struct list_head node;
	struct rtk_hidg_report_desc_data *func_data;
};

static LIST_HEAD(hidg_func_list);
extern bool g_hid_fake_standby_usb_filter;
extern bool g_hid_fake_standby_filter_function_enable;

struct rtk_hidg_report_desc_data * rtk_hidg_get_hid_func_node_by_name(const char *name)
{
	struct hidg_func_node *n = NULL;
	
	if(!name)
		return NULL;
	
	list_for_each_entry(n, &hidg_func_list, node) {
		if(n->func_data && strcmp(n->func_data->nid, name) == 0)
			return n->func_data;
	}
	return NULL;
}

static int rtk_hidg_plat_driver_probe(struct platform_device *pdev)
{
	struct rtk_hidg_report_desc_data *func_data = dev_get_platdata(&pdev->dev);
	struct hidg_func_node *entry;

	if (!func_data || !func_data->func_desc) {
		RTK_COMPOSITE_ERR("Platform data missing\n");
		return -ENODEV;
	}

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;
	
	entry->func_data= func_data;
	list_add_tail(&entry->node, &hidg_func_list);

	return 0;
}

static int rtk_hidg_plat_driver_remove(struct platform_device *pdev)
{
	struct hidg_func_node *e, *n;

	list_for_each_entry_safe(e, n, &hidg_func_list, node) {
		list_del(&e->node);
		kfree(e);
	}

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int rtk_hidg_platform_suspend_late(struct device *dev)
{
	return 0;
}

static int rtk_hidg_platform_resume_noirq(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	if(g_hid_fake_standby_filter_function_enable) {
		if((get_product_type() == PRODUCT_TYPE_DIAS) && (pdev->id == 0))
			g_hid_fake_standby_usb_filter = true;
	}
	return 0;
}
#endif

static const struct dev_pm_ops rtk_hidg_pm_ops = {
    .resume_early   = rtk_hidg_platform_resume_noirq,
    .suspend_late     = rtk_hidg_platform_suspend_late,
};


static struct platform_driver rtk_hidg_plat_driver = {
	.remove		= rtk_hidg_plat_driver_remove,
	.probe		= rtk_hidg_plat_driver_probe,
	.driver		= {
		.name	= PLATFORM_HIDG_NAME,
		.pm = &rtk_hidg_pm_ops,
	},
};


int __init rtk_composite_init(void)
{
	return platform_driver_register(&rtk_hidg_plat_driver);
}

void  rtk_composite_cleanup(void)
{
	platform_driver_unregister(&rtk_hidg_plat_driver);
}

