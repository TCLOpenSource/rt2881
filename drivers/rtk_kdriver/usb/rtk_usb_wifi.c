#include <linux/kernel.h>
#include <linux/usb/ch11.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/uaccess.h>
#include <rtk_kdriver/io.h>
#include <mach/system.h>
#include <linux/usb/hcd.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include "rtk_usb_core.h"
#include "rtk_usb_wifi.h"

#define RTK_USB_WIFI_INFO(fmt, args...)       rtd_pr_rtk_usb_info(fmt, ## args)
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)	
struct usb_hcd * ehci_find_ehci_hcd_by_reg_start(unsigned int reg_start, unsigned int reg_mask);
void ehci_hcd_switch_port_owner(struct usb_hcd *hcd, unsigned int port_num, bool switch_to_ehci);
int ehci_hcd_set_phy_reg(
        struct usb_hcd     *hcd,
        unsigned char       port1,
        unsigned char       page,
        unsigned char       addr,
        unsigned char       val);
int ehci_hcd_get_phy_reg(
        struct usb_hcd     *hcd,
        unsigned char       port1,
        unsigned char       page,
        unsigned char       addr);
void ehci_usb2_save_z0_tuning_value(struct usb_hcd *hcd, unsigned int  port, unsigned int z0_value);
#endif
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)	
void xhci_hcd_disconnect_usb2_port(struct usb_hcd *hcd, unsigned int port_num);
int xhci_hcd_set_phy_reg(struct usb_hcd *hcd, unsigned char phy, unsigned char page, unsigned char addr, unsigned char val);
int xhci_hcd_get_phy_reg(struct usb_hcd *hcd, unsigned char phy, unsigned char page, unsigned char addr);
void xhci_usb2_save_z0_tuning_value(struct usb_hcd *hcd, unsigned int  port, unsigned int z0_value);
#endif

unsigned int g_ehci_port_z0_tuning_array[] = EHCI_PORT_Z0_TUNING_ARRAY;
#define EHCI_PORT_Z0_TUNING_ARRAY_SIZE (sizeof(g_ehci_port_z0_tuning_array) / sizeof(g_ehci_port_z0_tuning_array[0]))
unsigned int g_cur_ehci_tuning_index = 0;
unsigned int g_default_ehci_port_z0_val = 0xffffffff;



unsigned int g_xhci_port_z0_tuning_array[] = XHCI_PORT_Z0_TUNING_ARRAY;
#define XHCI_PORT_Z0_TUNING_ARRAY_SIZE (sizeof(g_xhci_port_z0_tuning_array) / sizeof(g_xhci_port_z0_tuning_array[0]))
unsigned int g_cur_xhci_tuning_index = 0;
unsigned int g_default_xhci_port_z0_val = 0xffffffff;
bool g_z0_tuning_done = false;


void reset_usb_wifi_tuning_status(void)
{
	g_cur_ehci_tuning_index = 0;
	g_cur_xhci_tuning_index = 0;
	g_z0_tuning_done = false;
}

static bool is_a_usb_device(struct device *dev)
{
	if (dev->type && dev->type->name 
		&& (strcmp(dev->type->name, "usb_device") == 0))
			return true;
	return false;
}

void rtk_usb_disconnect_usb_port(struct wifi_port_info *port_info)
{
	if(port_info->is_ehci_or_xhci_port) {
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)	
		ehci_hcd_switch_port_owner(port_info->hcd_priv, port_info->port_num, true);
		RTK_USB_WIFI_INFO("[WIFI] try to disconnect port  %d\n", port_info->port_num);
#endif
	} else{
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)	
		xhci_hcd_disconnect_usb2_port(port_info->hcd_priv, port_info->port_num);
		RTK_USB_WIFI_INFO("[WIFI] try to disconnect port  %d\n", port_info->port_num);
#endif
	}
}

#ifdef EHCI_PORT_Z0_SPECAIL_SETTING
static void specail_z0_set_flow(struct wifi_port_info *port_info, unsigned char z0_value)
{
	if(port_info->port_num == 1) {
		unsigned int mask = ((1 << EHCI_PORT_Z0_BIT_NUM) - 1);
		unsigned int val = ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG);
		val = (val & ~(mask << EHCI_PORT_Z0_BIT_OFFSET)) 
				  			| ((z0_value & mask) << EHCI_PORT_Z0_BIT_OFFSET);
		ehci_hcd_set_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG, val);
	} else if(port_info->port_num == 2) {
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE3, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE3) & (~(1 << 6))) | ((z0_value & 0x1) << 6));
		z0_value = z0_value >> 1;
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE1, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE1) & (~(1 << 3))) | ((z0_value & 0x1) << 3));
		z0_value = z0_value >> 1;
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE3, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE3) & (~(1 << 7))) | ((z0_value & 0x1) << 7));
		z0_value = z0_value >> 1;
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE2, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE2) & (~(1 << 0))) | ((z0_value & 0x1) << 0));
	} else{
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE2, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE2) & (~(1 << 1))) | ((z0_value & 0x1) << 1));
		z0_value = z0_value >> 1;
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE1, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE1) & (~(1 << 4))) | ((z0_value & 0x1) << 4));
		z0_value = z0_value >> 1;
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE2, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE2) & (~(1 << 2))) | ((z0_value & 0x1) << 2));
		z0_value = z0_value >> 1;
		ehci_hcd_set_phy_reg(port_info->hcd_priv, 2, 2,  0xE2, 
			(ehci_hcd_get_phy_reg(port_info->hcd_priv,  2, 2, 0xE2) & (~(1 << 3))) | ((z0_value & 0x1) << 3));		
	}
}

static unsigned char specail_z0_get_flow(struct wifi_port_info *port_info)
{
	if(port_info->port_num == 1) {
		unsigned int mask = ((1 << EHCI_PORT_Z0_BIT_NUM) - 1);
		unsigned int val = ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG);
		return ((val >> EHCI_PORT_Z0_BIT_OFFSET) & mask);
	} else if(port_info->port_num == 2) {
		unsigned char z0_val = 0x0;
		z0_val = ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE2) & 0x1;
		z0_val = (z0_val << 1) | ((ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE3) >> 7) & 0x1);
		z0_val = (z0_val << 1) | ((ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE1) >> 3) & 0x1);
		z0_val = (z0_val << 1) | ((ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE3) >> 6) & 0x1);
		return z0_val;
	} else{
		unsigned char z0_val = 0x0;
		z0_val = ((ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE2) >> 3) & 0x1);
		z0_val = (z0_val << 1) | ((ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE2) >> 2) & 0x1);
		z0_val = (z0_val << 1) | ((ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE1) >> 4) & 0x1);
		z0_val = (z0_val << 1) | ((ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								2, 2, 0xE2) >> 1) & 0x1);
		return z0_val;
	}	
}
#endif

static void get_default_port_z0_value(struct wifi_port_info *port_info)
{
	if(port_info->is_ehci_or_xhci_port) {
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)		
		if((g_cur_ehci_tuning_index == 0) && (g_default_ehci_port_z0_val == 0xffffffff)) {
#ifdef EHCI_PORT_Z0_SPECAIL_SETTING
			g_default_ehci_port_z0_val = specail_z0_get_flow(port_info);
#else			
			unsigned int mask = ((1 << EHCI_PORT_Z0_BIT_NUM) - 1);
			unsigned int val = ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG);
			g_default_ehci_port_z0_val = (val >> EHCI_PORT_Z0_BIT_OFFSET) & mask;
#endif			
			RTK_USB_WIFI_INFO("[WIFI] get port[%d] default z0:%x\n", port_info->port_num, g_default_ehci_port_z0_val);
		}
#endif
	} else {
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)	
		 if((g_cur_xhci_tuning_index == 0) && (g_default_xhci_port_z0_val == 0xffffffff)) {
			unsigned int mask = ((1 << XHCI_PORT_Z0_BIT_NUM) - 1);
			unsigned int val = xhci_hcd_get_phy_reg(port_info->hcd_priv,  port_info->port_num, 
				XHCI_PORT_Z0_PAGE, XHCI_PORT_Z0_REG);
	              g_default_xhci_port_z0_val = (val >> XHCI_PORT_Z0_BIT_OFFSET) & mask;
		       RTK_USB_WIFI_INFO("[WIFI] get port[%d] default z0:%x\n", port_info->port_num, g_default_xhci_port_z0_val);
		 }
#endif
	}
}

static void set_target_port_default_z0_value(struct wifi_port_info *port_info)
{
	if(port_info->is_ehci_or_xhci_port) {
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)		
		if(g_default_ehci_port_z0_val != 0xffffffff) {
#ifdef EHCI_PORT_Z0_SPECAIL_SETTING
			specail_z0_set_flow(port_info, g_default_ehci_port_z0_val);
#else
			unsigned int mask = ((1 << EHCI_PORT_Z0_BIT_NUM) - 1);
			unsigned int val = ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG);
	              val = (val & ~(mask << EHCI_PORT_Z0_BIT_OFFSET)) 
				  			| ((g_default_ehci_port_z0_val & mask) << EHCI_PORT_Z0_BIT_OFFSET);
			ehci_hcd_set_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG, val);
#ifdef EHCI_PORT_Z0_MASTER_PORT
			ehci_hcd_set_phy_reg(port_info->hcd_priv,  
								EHCI_PORT_Z0_MASTER_PORT, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG, val);
#endif
#endif
			RTK_USB_WIFI_INFO("[WIFI] set port[%d] default z0:%x\n", port_info->port_num, g_default_ehci_port_z0_val);
		}
			
#endif
	} else {
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)	
		if(g_default_xhci_port_z0_val != 0xffffffff) {
			unsigned int mask = ((1 << XHCI_PORT_Z0_BIT_NUM) - 1);
			unsigned int val = xhci_hcd_get_phy_reg(port_info->hcd_priv,  port_info->port_num, 
				XHCI_PORT_Z0_PAGE, XHCI_PORT_Z0_REG);
	              val = (val & ~(mask << XHCI_PORT_Z0_BIT_OFFSET)) 
				  	| ((g_default_xhci_port_z0_val & mask) << XHCI_PORT_Z0_BIT_OFFSET);
			xhci_hcd_set_phy_reg(port_info->hcd_priv,  port_info->port_num, 
					XHCI_PORT_Z0_PAGE, XHCI_PORT_Z0_REG, val);
			RTK_USB_WIFI_INFO("[WIFI] set port[%d] default z0:%x\n", port_info->port_num, g_default_xhci_port_z0_val);
		}
#endif
	}
}


static void set_target_port_tuning_z0_value(struct wifi_port_info *port_info)
{
	get_default_port_z0_value(port_info);
	if(port_info->is_ehci_or_xhci_port) {
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)
		if(g_cur_ehci_tuning_index < EHCI_PORT_Z0_TUNING_ARRAY_SIZE) {
#ifdef EHCI_PORT_Z0_SPECAIL_SETTING	
                    specail_z0_set_flow(port_info, g_ehci_port_z0_tuning_array[g_cur_ehci_tuning_index]);
#else
			unsigned int mask = ((1 << EHCI_PORT_Z0_BIT_NUM) - 1);
			unsigned int val = ehci_hcd_get_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG);
	              val = (val & ~(mask << EHCI_PORT_Z0_BIT_OFFSET)) 
				  			| (g_ehci_port_z0_tuning_array[g_cur_ehci_tuning_index] << EHCI_PORT_Z0_BIT_OFFSET);
			ehci_hcd_set_phy_reg(port_info->hcd_priv,  
								port_info->port_num, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG, val);
#ifdef EHCI_PORT_Z0_MASTER_PORT
			ehci_hcd_set_phy_reg(port_info->hcd_priv,  
								EHCI_PORT_Z0_MASTER_PORT, EHCI_PORT_Z0_PAGE, EHCI_PORT_Z0_REG, val);
#endif	
#endif
			ehci_usb2_save_z0_tuning_value(port_info->hcd_priv, port_info->port_num, g_ehci_port_z0_tuning_array[g_cur_ehci_tuning_index]);
			RTK_USB_WIFI_INFO("[WIFI] set port[%d] tuning z0:%x\n", port_info->port_num, g_ehci_port_z0_tuning_array[g_cur_ehci_tuning_index]);
			g_cur_ehci_tuning_index++;
			rtk_usb_disconnect_usb_port(port_info);
		} else {
			set_target_port_default_z0_value(port_info);
			g_z0_tuning_done = true;
		}
#endif
	} else {
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)	
		if(g_cur_xhci_tuning_index < XHCI_PORT_Z0_TUNING_ARRAY_SIZE) {
			unsigned int mask = ((1 << XHCI_PORT_Z0_BIT_NUM) - 1);
			unsigned int val = xhci_hcd_get_phy_reg(port_info->hcd_priv,  port_info->port_num, 
				XHCI_PORT_Z0_PAGE, XHCI_PORT_Z0_REG);
	              val = (val & ~(mask << XHCI_PORT_Z0_BIT_OFFSET)) 
				  	| (g_xhci_port_z0_tuning_array[g_cur_xhci_tuning_index] << XHCI_PORT_Z0_BIT_OFFSET);
			xhci_hcd_set_phy_reg(port_info->hcd_priv,  port_info->port_num, 
					XHCI_PORT_Z0_PAGE, XHCI_PORT_Z0_REG, val);
			xhci_usb2_save_z0_tuning_value(port_info->hcd_priv, port_info->port_num, g_xhci_port_z0_tuning_array[g_cur_xhci_tuning_index]);
			RTK_USB_WIFI_INFO("[WIFI] set port[%d] tuning z0:%x\n", port_info->port_num, g_xhci_port_z0_tuning_array[g_cur_xhci_tuning_index]);
			g_cur_xhci_tuning_index++;
			rtk_usb_disconnect_usb_port(port_info);
		} else {
			set_target_port_default_z0_value(port_info);
			g_z0_tuning_done = true;
		}
#endif
	}
}


bool get_usb_device_controller(struct usb_device *udev, struct wifi_port_info *port_info)
{
	struct usb_bus * bus = udev->parent->bus;
	struct device *controller_dev = NULL;
       struct platform_device *pdev = NULL;
	struct usb_hcd *hcd = NULL;
	struct resource * res_mem = NULL;
	if(!bus)
		return false;
	controller_dev= bus->controller;
	if(!controller_dev)
		return false;
	
	pdev = container_of(controller_dev, struct platform_device, dev);
	hcd = (struct usb_hcd *)dev_get_drvdata(controller_dev);
	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!hcd || !hcd->product_desc || !res_mem)
		return false;
	if(strstr(hcd->product_desc, "xHCI Host Controller")) {
#if IS_ENABLED(CONFIG_RTK_KDRV_XHCI_HCD_PLATFORM)
#ifdef XHCI_PORT_Z0_TUNING_DATA_EFFECTED
		port_info->is_ehci_or_xhci_port = false;
		port_info->port_num = udev->portnum;
		port_info->hcd_priv = hcd;
#endif
#endif
		
	}
	else if(strstr(hcd->product_desc, "OHCI controller")
		|| strstr(hcd->product_desc, "EHCI Host Controller")) {
#if IS_ENABLED(CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM)		
#ifdef EHCI_PORT_Z0_TUNING_DATA_EFFECTED
		port_info->is_ehci_or_xhci_port = true;
		port_info->port_num = udev->portnum;
		port_info->hcd_priv = ehci_find_ehci_hcd_by_reg_start(res_mem->start, 0xFFFFF000);
#endif
#endif
	} else {
		return false;
	}
	if(!port_info->hcd_priv)
		return false;
	
	RTK_USB_WIFI_INFO("[WIFI] got hcd information: %s %x %x %lx\n", hcd->product_desc, 
				port_info->is_ehci_or_xhci_port, port_info->port_num, (unsigned long)port_info->hcd_priv);
	return true;
	
	
}

bool rtk_is_usb_device_under_root_port(struct device *dev)
{
	struct usb_device *udev = NULL;
	if(!is_a_usb_device(dev))
		return false;
	udev = to_usb_device(dev);
	if(udev->parent && !udev->parent->parent)
		return true;
	return false;
}

static bool is_target_wifi_device(struct usb_device *udev)
{
#if 0
	if((udev->descriptor.idVendor == 0x0BDA
			&& udev->descriptor.idProduct == 0xC82C)
			|| (udev->descriptor.idVendor == 0x043e
			&& udev->descriptor.idProduct == 0x3113))
			return true;
	return false;
#else
	bool ret = false;
	
	if(udev->speed != USB_SPEED_FULL)
		return ret;

	if(udev->product && strstr(udev->product, "802.11")
			&& strstr(udev->product, "NIC"))
			ret =true;

	if(!ret) {
		if(udev->actconfig) {
			int i = 0;
			for(i = 0; i < USB_MAXIADS; i++) {
				struct usb_interface_assoc_descriptor *intf_assoc = udev->actconfig->intf_assoc[i];
				if(!intf_assoc)
					break;
				if(intf_assoc->bFunctionClass == 0xE0) {
					ret = true;
					break;
				}
			}
		}

	}
	return ret;
#endif
}


static int rtk_usb_wifi_bus_notify(struct notifier_block *nb, unsigned long action,
		void *data)
{
	struct device *dev = data;
	struct usb_device *udev = NULL;
	struct wifi_port_info port_info;
	bool ret;
	switch (action) {
	case BUS_NOTIFY_ADD_DEVICE:
		if(g_z0_tuning_done)
			break;
		if(!rtk_is_usb_device_under_root_port(dev))
			break;
		udev = to_usb_device(dev);	
		if(!is_target_wifi_device(udev))
			break;
		memset(&port_info, 0, sizeof(port_info));
		ret = get_usb_device_controller(udev, &port_info);
		if(!ret)
			break;		
		if(udev->speed >= USB_SPEED_HIGH) {
			set_target_port_default_z0_value(&port_info);
			break;
		}
		
		RTK_USB_WIFI_INFO("[WIFI] vid:%04x, pid:%04x, spedd:%d\n", 
				udev->descriptor.idVendor, udev->descriptor.idProduct, udev->speed);
		set_target_port_tuning_z0_value(&port_info);
		break;
	case BUS_NOTIFY_DEL_DEVICE:
		if(!rtk_is_usb_device_under_root_port(dev))
			break;
		udev = to_usb_device(dev);	
		if(!is_target_wifi_device(udev))
			break;
		if(udev->speed >= USB_SPEED_HIGH) {
			reset_usb_wifi_tuning_status();
		}
		break;
	}
	
		
	return 0;
}

static struct notifier_block rtk_usb_wifi_bus_nb = {
	.notifier_call = rtk_usb_wifi_bus_notify,
};

static struct bus_type *s_usb_bus_type = NULL;

static struct usb_device_id dummy_usb_wifi_ids[] = {
	{ }
};

static int dummy_usb_wifi_probe(struct usb_interface *intf,
			 const struct usb_device_id *id)
{
	return -1;
}

static void dummy_usb_wifi_disconnect(struct usb_interface *intf)
{
	
}

static struct usb_driver dummy_usb_wifi_driver = {
	.name =		"dummy_usb_wifi_driver",
	.probe =	dummy_usb_wifi_probe,
	.disconnect =	dummy_usb_wifi_disconnect,
	.id_table =	dummy_usb_wifi_ids,
	.supports_autosuspend = 0,
	.soft_unbind =	0,
};

int rtk_usb_wifi_register_notifier(void)
{
	int ret;
	ret = usb_register(&dummy_usb_wifi_driver);
	if(ret)
		goto EXIT;
	s_usb_bus_type = dummy_usb_wifi_driver.drvwrap.driver.bus;
	usb_deregister(&dummy_usb_wifi_driver);
	if(!s_usb_bus_type)
		goto EXIT;

	ret = bus_register_notifier(s_usb_bus_type, &rtk_usb_wifi_bus_nb);
	if(ret)
		s_usb_bus_type = NULL;
EXIT:
	return 0;
}

void rtk_usb_wifi_unregister_notifier(void)
{
	if(s_usb_bus_type) {
		bus_unregister_notifier(s_usb_bus_type, &rtk_usb_wifi_bus_nb);
		s_usb_bus_type = NULL;
	}
}

