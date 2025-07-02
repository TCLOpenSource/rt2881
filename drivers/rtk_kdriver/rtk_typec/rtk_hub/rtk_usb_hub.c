#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of_platform.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/freezer.h>
#include <mach/pcbMgr.h>
#include <linux/uaccess.h>
#include <rtk_kdriver/io.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/usb2_hub_reg.h>
#include <linux/suspend.h>
#include <linux/notifier.h>
#include <mach/rtk_platform.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include "rtk_usb_hub.h"
#include "rtk_usb_hub_defines.h"
#include "rtk_usb_hub_wrapper.h"
#include "rtk_usb_hub_fw.h"
#include "rtk_usb_hub_smbus.h"
#include "rtk_usb_hub_debug.h"
#include "rtk_usb_hub_camera.h"
#include "rtk_usb_hub_device_monitor.h"
#include <rtk_kdriver/i2c-rtk-api.h>

#define ENABLE_SMBUF_POLL_STATUS
#define DRIVER_DESC "RTK USB hub generic platform driver"

#ifdef ENABLE_SMBUF_POLL_STATUS
static bool is_in_suspend_flow = false;
#endif
struct device *g_usb_hub_devices[MAX_USB_HUB_DEVICE_NUM] = {NULL};
unsigned int usb_typec_value = 0;
extern unsigned int rtk_typec_get_power_watt(void);

static bool download_usb_hub_fw(struct platform_device *pdev, unsigned long dst_addr, unsigned int size)
{
    unsigned char *ubuf;
    unsigned int i;
    unsigned int count;
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    
    rtd_outl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_DATA_SEL_reg, 0x1);    // Enable SW control flow

    ubuf = (unsigned char *)dst_addr;

    // Store USB Hub to IMEM
    for (i = 0; i < size; i++) {
	rtd_outl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_WR_ADDR_reg, i);
	rtd_outl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_WR_DATA_reg, ubuf[i]);
    }

    RTK_USB_HUB_WARNING("EMCU compare data\n");

   rtd_outl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_ADDR_reg, 0x1);
   count = 0;
   while(count++ < 10000) {
	if(rtd_inl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_ADDR_reg) == 0x1)
		break;
   }

   rtd_outl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_ADDR_reg, 0x0);
   count = 0;
   while(count++ < 10000) {
	if(rtd_inl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_ADDR_reg) == 0x0)
		break;
   }

    // Clear IMEM address
    for (i = 0; i < size; i++) {
	unsigned char tmp = 0;
	tmp = rtd_inl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_DATA_reg);
        if(tmp != ubuf[i]) {
            RTK_USB_HUB_WARNING("[ERROR] Compare fail for USB hub bin:%u %02x %02x\n", i, tmp , ubuf[i]);
            goto ERR_GET_EMCU_FW_FAILED;
        }
	count = 0;
	while(count++ < 10000) {
		if(rtd_inl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_ADDR_reg) == (i + 1))
			break;

	}
    }
    rtd_outl(private_data->wrapper_regs.USB_HUB_HUB_RAM_I_DATA_SEL_reg, 0x0);    // Disable SW control flow
    return true;

ERR_GET_EMCU_FW_FAILED:
    return false;
}


void rtk_usb_hub_set_default_dphy_mux(struct device *dev, bool to_tv)
{
	if(to_tv) {
		rtk_usb_hub_dsp_enable(dev, false);
		rtk_usb_hub_usp_enable(dev, false);
	} else {
		rtk_usb_hub_dsp_enable(dev, true);
		rtk_usb_hub_usp_enable(dev, true);
	}
}

bool usb_hub_is_in_suspend(void)
{
#ifdef ENABLE_SMBUF_POLL_STATUS
	return is_in_suspend_flow;
#else
	return false;
#endif
}
extern void rtk_usb_hub_check_and_set_hub_downstream_port_state(struct device *dev);
#ifdef ENABLE_SMBUF_POLL_STATUS
int _usb_hub_status_process(void *arg)
{
	while(!kthread_should_stop()) {
		rtk_usb_hub_check_port((struct device *)arg);
		rtk_usb_hub_check_and_set_hub_downstream_port_state((struct device *)arg);
		msleep(500);
	}
	return 0;
}

static int usb_hub_pm_event (struct notifier_block *this, unsigned long event, void *ptr)
{
	switch (event) {
	case PM_SUSPEND_PREPARE:
		is_in_suspend_flow = true;
		break;
	case PM_POST_SUSPEND:
		is_in_suspend_flow = false;
		break;
	default:
		break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block usb_hub_notifier = {
  .notifier_call = usb_hub_pm_event,
};
#endif

static void rtk_usb_do_hub_mode_check(struct RTK_USB_HUB_PRIVATE *private_data)
{
	unsigned char mode = rtk_usb_check_mode(private_data);
	if(private_data->cur_mode != mode) {
		private_data->cur_mode = mode;
		RTK_USB_HUB_INFO("HUB status change:%s\n", mode ? "device" : "host");		
		if(private_data->cur_mode) {
			rtk_usb_hub_usp_enable(&private_data->pdev->dev, true); 
		} else {
			rtk_usb_hub_usp_enable(&private_data->pdev->dev, false); 
		}
	}
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
static void rtk_usb_hub_mode_check_func(struct timer_list* param_timer)
{
	struct RTK_USB_HUB_PRIVATE *private_data = from_timer(private_data,param_timer,mode_check_timer);
	rtk_usb_do_hub_mode_check(private_data);
	mod_timer(&private_data->mode_check_timer, jiffies + USB_HUB_MODE_CHECK_TIMEOUT);
}
#else
static void rtk_usb_hub_mode_check_func(unsigned long __opaque)
{
	struct RTK_USB_HUB_PRIVATE *private_data = (struct RTK_USB_HUB_PRIVATE *)__opaque;
	rtk_usb_do_hub_mode_check(private_data);
	mod_timer(&private_data->mode_check_timer, jiffies + USB_HUB_MODE_CHECK_TIMEOUT);
}
#endif



static bool rtk_usb_hub_init(struct platform_device *pdev)
{
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    bool ret = false;
    unsigned long usb_hub_addr;
    unsigned int usb_hub_size;
	int i = 0;

    // 0. Set U2 hub wrapper reset
    rtk_usb_hub_wrapper_clock_on(pdev);

    rtd_outl(private_data->wrapper_regs.USB_HUB_HUB_CTRL0_reg, 0x01b4310);//div_1m_cnt div_400k_cnt  ups_vbus_det reg_force_pll on 0 reg_sw_clk_sel reg_sw_mode
    udelay(100);

    // 1. Load 8051 bin
    usb_hub_addr = (unsigned long)get_usb2_hub_bin(&usb_hub_size);
    if(!download_usb_hub_fw(pdev, usb_hub_addr, usb_hub_size)) {
        goto FAIL_GET_USB_HUB;
    }

    // 2. Load APHY
    rtk_usb_hub_load_phy_settings(pdev);

    // 3. Set CLK
    rtk_usb_hub_clk_on(pdev);

	//3.1 Do post init
	rtk_usb_hub_post_init(pdev);

    // 4. Polling SMBUS (create thread)
	for(i = 0; i < MAX_HUB_TYPEA_PORT_NUM; i++) {
		atomic_set(&private_data->dwonstreamport_should_state[i], 0);
		atomic_set(&private_data->dwonstreamport_current_state[i], 0);
	}
#ifdef ENABLE_SMBUF_POLL_STATUS
    private_data->usb_hub_task = kthread_create(_usb_hub_status_process, &pdev->dev, "_usb_hub_status_process");
    if(IS_ERR(private_data->usb_hub_task)) {
        RTK_USB_HUB_WARNING("Create USB Hub Thread fail \n");
	 private_data->usb_hub_task = NULL;
    }
    if(private_data->usb_hub_task)
    	wake_up_process(private_data->usb_hub_task);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	timer_setup(&private_data->mode_check_timer, rtk_usb_hub_mode_check_func,0);
#else
	init_timer(&private_data->mode_check_timer);
	private_data->mode_check_timer.data = (unsigned long) private_data;
	private_data->mode_check_timer.function = rtk_usb_hub_mode_check_func;
#endif

    // 5. Initial USB hub
    if(get_product_type() == PRODUCT_TYPE_DIAS) {
		if(private_data->id == 0) {
        	rtk_usb_hub_dsp_enable(&pdev->dev, false);
		}
        rtk_usb_hub_usp_enable(&pdev->dev, false);  
		private_data->cur_mode = 0;
    } else {
        if(private_data->id == 0) {
    	 	rtk_usb_hub_set_default_dphy_mux(&pdev->dev, true);
        }
		private_data->cur_mode = 0;
    }
	rtk_usb_hub_select_cur_hub(&pdev->dev, 0);
	if(get_product_type() == PRODUCT_TYPE_DIAS)
		mod_timer(&private_data->mode_check_timer, jiffies + USB_HUB_MODE_CHECK_TIMEOUT);
    ret = true;

FAIL_GET_USB_HUB:
    return ret;
}

/******************************Sysfs*********************************/
struct attribute usb_hub_dphy_mux_attr;
struct attribute usb_hub_switch_downstream_port_attr;
struct attribute usb_hub_switch_upstream_port_attr;
struct attribute usb_hub_select_hub_attr;
struct attribute usb_hub_typea_switch_mode_attr;
struct attribute usb_hub_typea_port_switch_mode_attr;
struct attribute usb_hub_monitor_usb_device_attr;
struct attribute usb_hub_power_monitor_value_attr;
struct attribute usb_hub_switch_upstream_port_only_attr;

static struct attribute *usb_hub_default_attrs[] = {
	&usb_hub_dphy_mux_attr,
	&usb_hub_switch_upstream_port_attr,
	&usb_hub_switch_downstream_port_attr,
	&usb_hub_select_hub_attr,
	&usb_hub_typea_switch_mode_attr,
	&usb_hub_typea_port_switch_mode_attr,
	&usb_hub_monitor_usb_device_attr,
	&usb_hub_power_monitor_value_attr,
	&usb_hub_switch_upstream_port_only_attr,
	NULL,
};

unsigned int rtk_typec_get_power_watt(void)
{
    unsigned int total_power = 0;
    unsigned int total_valtage = 0;
    unsigned int total_current = 0;
    unsigned char pWritingBytes[3] = {0x05, 0x20, 0x00};
    unsigned char data[2] = {0};
    unsigned char wdata[1] = {0x3};
    int ret = 0;

    // IC calibration
    ret = i2c_master_send_ex(0, 0x40, &pWritingBytes[0], 3);

    // Voltage
    wdata[0] = 0x2;
    ret = i2c_master_recv_ex(0, 0x40, wdata, 1, data, 2);
    total_valtage = (data[0] << 8);
    total_valtage |= data[1];
    RTK_USB_HUB_WARNING("Typec total voltage = %d mV\n", (total_valtage >> 1));

    // Current
    wdata[0] = 0x4;
    ret = i2c_master_recv_ex(0, 0x40, wdata, 1, data, 2);
    total_current = (data[0] << 8);
    total_current |= data[1];
    RTK_USB_HUB_WARNING("Typec total current = %d mA\n", total_current);

    // Power Watt
    wdata[0] = 0x3;
    ret = i2c_master_recv_ex(0, 0x40, wdata, 1, data, 2);
    total_power = (data[0] << 8);
    total_power |= data[1];

    total_power = (unsigned int)(total_power * 20);
    RTK_USB_HUB_WARNING("Typec total power = %d mW\n", total_power);

    return total_power;
}

static ssize_t usb_hub_attrs_show(struct kobject *kobj,
			      struct attribute *attr, char *buf)
{
	struct RTK_USB_HUB_PRIVATE *private_data = 
	 	(struct RTK_USB_HUB_PRIVATE *)container_of(kobj, struct RTK_USB_HUB_PRIVATE, kobj);
	if(attr == &usb_hub_select_hub_attr) {
		return snprintf(buf, PAGE_SIZE, "current selected hub:%u\n", 
				rtk_usb_hub_get_selected_hub());
	} else if(attr == &usb_hub_typea_switch_mode_attr) {
		return snprintf(buf, PAGE_SIZE, "switch_mode:%u\n", 
				private_data->typea_port_switch_mode[0]);
	} else if(attr == &usb_hub_typea_port_switch_mode_attr) {
		int i = 0;
		snprintf(buf, PAGE_SIZE, "switch_mode(0:auto;1:switch to host, 2:always to PC):");
		
		for(i = 0; i < MAX_HUB_TYPEA_PORT_NUM; i++)
			snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "port%d=%u;", i,  private_data->typea_port_switch_mode[i]);
		snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "\n");
		return strlen(buf);
	} else  if(attr == &usb_hub_monitor_usb_device_attr) {
		rtk_usb_hub_dump_monitor_usb_dev_path(buf, PAGE_SIZE);
		return strlen(buf);
	} else if(attr == &usb_hub_power_monitor_value_attr) {
		usb_typec_value = rtk_typec_get_power_watt();
		return snprintf(buf, PAGE_SIZE, "power_monitor_value:%u\n", usb_typec_value);
	}
	return -1;
}

static ssize_t usb_hub_attrs_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t count)
{

	struct device *dev = NULL;
	struct RTK_USB_HUB_PRIVATE *private_data = 
	 	(struct RTK_USB_HUB_PRIVATE *)container_of(kobj, struct RTK_USB_HUB_PRIVATE, kobj);
	dev = &private_data->pdev->dev;

	if (attr == &usb_hub_dphy_mux_attr) {
		unsigned int val = 0;	
		if (kstrtouint(buf, 10, &val) != 0)
			return -EINVAL;
		rtk_usb_hub_set_default_dphy_mux(dev, !!val);
	} else if (attr == &usb_hub_switch_upstream_port_attr) {	
		unsigned int val = 0;	
		if (kstrtouint(buf, 10, &val) != 0)
			return -EINVAL;
		if((!!val)) {//device mode
			//rtk_usb_hub_usp_enable(dev, true);
			do_check_usb_camera_on_enter_typec(dev);
		} else {
			//rtk_usb_hub_usp_enable(dev, false);
			do_check_usb_camera_on_exit_typec(dev);
		}	
	} else if (attr == &usb_hub_switch_downstream_port_attr) {
		unsigned int val = 0;	
		if (kstrtouint(buf, 10, &val) != 0)
			return -EINVAL;
		if((!!val)) { //device mode
			rtk_usb_hub_dsp_enable(dev, true);
		} else {
			rtk_usb_hub_dsp_enable(dev, false);
		}
	} else if(attr == &usb_hub_select_hub_attr) {
		unsigned int val = 0;	
		if (kstrtouint(buf, 10, &val) != 0)
			return -EINVAL;
		if(val >= MAX_USB_HUB_DEVICE_NUM)
			return -EINVAL;
		rtk_usb_hub_select_cur_hub(dev, val);
	} else if(attr == &usb_hub_typea_switch_mode_attr) {
		unsigned int val = 0;	
		if (kstrtouint(buf, 10, &val) != 0)
			return -EINVAL;
		if(private_data->typea_port_switch_mode[0] != val) {
			int i = 0;
			for(i = 0; i < MAX_HUB_TYPEA_PORT_NUM; i++) {
				private_data->typea_port_switch_mode[i] = val;
			}
			if(private_data->typea_port_switch_mode[0] == 1) //soc
				rtk_usb_hub_dsp_enable(dev, false);
			else if(private_data->typea_port_switch_mode[0] == 2) //PC
				rtk_usb_hub_dsp_enable(dev, true);	
			else
				rtk_usb_hub_dsp_enable(dev, false);//soc
		}
		
	} else if(attr == &usb_hub_typea_port_switch_mode_attr) {
		unsigned int val = 0;	
		unsigned int port_num = 0;
		if(sscanf(buf, "%u,%u", &port_num, &val) != 2)
			return -EINVAL;
		if(port_num >= MAX_HUB_TYPEA_PORT_NUM || val >= 3)
			return -EINVAL;
		
		if(private_data->typea_port_switch_mode[port_num] != val) {
			private_data->typea_port_switch_mode[port_num] = val;
			if(private_data->typea_port_switch_mode[port_num] == 1) //soc
				rtk_usb_hub_specail_dsp_enable(dev, port_num, false);
			else if(private_data->typea_port_switch_mode[port_num] == 2) //PC
				rtk_usb_hub_specail_dsp_enable(dev, port_num, true);
			else
				rtk_usb_hub_specail_dsp_enable(dev, port_num, false);  //soc
		}
		
	} else if(attr == &usb_hub_monitor_usb_device_attr) {
		if(memcmp(buf, "add ", 4) == 0) {
			if(rtk_usb_hub_add_monitor_usb_dev_path(buf + 4) != true)
				return -EINVAL; 
		} else if(memcmp(buf, "del ", 4) == 0) {
			if(rtk_usb_hub_remove_monitor_usb_dev_path(buf + 4) != true)
				return -EINVAL; 
		} else {
			return -EINVAL; 
		}
	}
    else if(attr == &usb_hub_power_monitor_value_attr) {
        unsigned int val = 0;
        if (kstrtouint(buf, 10, &val) != 0)
            return -EINVAL;
        usb_typec_value = val;
    }else if (attr == &usb_hub_switch_upstream_port_only_attr) {
		if(get_product_type() != PRODUCT_TYPE_DIAS) {
			unsigned int val = 0;	
			if (kstrtouint(buf, 10, &val) != 0)
				return -EINVAL;
			if((!!val)) {//device mode
				rtk_usb_hub_usp_enable(dev, true);
			} else {
				rtk_usb_hub_usp_enable(dev, false);
			}
		}
	}
	return count;
}


static const struct sysfs_ops usb_hub_sysfs_ops = {
	.show   = usb_hub_attrs_show,
	.store  = usb_hub_attrs_store,
};

static struct kobj_type ktype_usb_hub = {
	.release        = NULL,
	.sysfs_ops      = &usb_hub_sysfs_ops,
	.default_attrs = usb_hub_default_attrs,
};

#define USB_HUB_ATTR(_name, _mode)				\
	struct attribute usb_hub_##_name##_attr = {	\
	.name = __stringify(_name), .mode = _mode,	\
	};

USB_HUB_ATTR(dphy_mux, 0644);
USB_HUB_ATTR(switch_upstream_port, 0644);
USB_HUB_ATTR(switch_downstream_port, 0644);
USB_HUB_ATTR(select_hub, 0644);
USB_HUB_ATTR(monitor_usb_device, 0644);
USB_HUB_ATTR(typea_switch_mode, 0644);
USB_HUB_ATTR(typea_port_switch_mode, 0644);
USB_HUB_ATTR(power_monitor_value, 0644);
USB_HUB_ATTR(switch_upstream_port_only, 0644);

void rtk_usb_hub_init_wrapper_regs(struct platform_device *pdev)
{
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	private_data->wrapper_regs.USB_HUB_HUB_RAM_I_WR_ADDR_reg = 
					private_data->io_addr + 0xA0000000 + 0x100;
	private_data->wrapper_regs.USB_HUB_HUB_RAM_I_WR_DATA_reg = 
					private_data->io_addr + 0xA0000000 + 0x104;
	private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_ADDR_reg = 
					private_data->io_addr + 0xA0000000 + 0x108; 
	private_data->wrapper_regs.USB_HUB_HUB_RAM_I_RD_DATA_reg = 
					private_data->io_addr + 0xA0000000 + 0x10C; 
	private_data->wrapper_regs.USB_HUB_HUB_RAM_I_DATA_SEL_reg = 
					private_data->io_addr + 0xA0000000 + 0x110;	  
	private_data->wrapper_regs.USB_HUB_HUB_CTRL0_reg = 
					private_data->io_addr + 0xA0000000 + 0x130;
	private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg = 
					private_data->io_addr + 0xA0000000 + 0x13C;
	private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg = 
					private_data->io_addr + 0xA0000000 + 0x140;
	private_data->wrapper_regs.USB_HUB_SMBUS_DATA_1_reg = 
					private_data->io_addr + 0xA0000000 + 0x150;
}

static int rtk_usb_hub_get_id_from_dts(struct platform_device *pdev, unsigned int *p_id)
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

static int rtk_usb_hub_platform_probe(struct platform_device *pdev)
{
	int ret = 0;
	unsigned int id = 0;
	struct RTK_USB_HUB_PRIVATE *private_data = NULL;
	struct resource *res_mem = NULL;
    
	if(rtk_usb_hub_get_id_from_dts(pdev, &id) != 0)
		id = 0;
	if(id >= MAX_USB_HUB_DEVICE_NUM) {
		RTK_USB_HUB_ERR("Incorrect hub id:%d\n", id);
		return -1;
	}

	private_data = (struct RTK_USB_HUB_PRIVATE *)kzalloc(
			sizeof(struct RTK_USB_HUB_PRIVATE), GFP_KERNEL);
	if(!private_data) {
		ret = -ENOMEM;
		goto FAIL_ALLOC_PRIVATE_DATA;
	}
	private_data->id = id;
	private_data->pdev = pdev;
	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	private_data->io_addr = res_mem->start;	
	
	platform_set_drvdata(pdev, private_data);

	ret = kobject_init_and_add(&private_data->kobj, &ktype_usb_hub, 
				NULL, "usb_hub%d", private_data->id);
	if(ret != 0)
		goto FAIL_CREATE_KOBJECT;

	rtk_usb_hub_init_wrapper_regs(pdev);
	spin_lock_init(&private_data->downstreamport_switch_lock);
	if(rtk_usb_hub_init(pdev) != true) {
		ret = -1;
		goto FAIL_INIT_HUB;
	}

	g_usb_hub_devices[id] = &pdev->dev;
	
	rtk_usb_hub_get_monitor_usb_dev_info();
	
	RTK_USB_HUB_WARNING("SLL usb hub[%d] start, io addr:%x\n", private_data->id, private_data->io_addr);
	return 0;
FAIL_INIT_HUB:
	kobject_put(&private_data->kobj);
FAIL_CREATE_KOBJECT:
	kfree(private_data);
	platform_set_drvdata(pdev, NULL);
FAIL_ALLOC_PRIVATE_DATA:
	return ret;
}

static int rtk_usb_hub_platform_remove(struct platform_device *pdev)
{
    struct RTK_USB_HUB_PRIVATE *private_data = NULL;

    private_data = (struct RTK_USB_HUB_PRIVATE *)platform_get_drvdata(pdev);
	del_timer_sync(&private_data->mode_check_timer);
    kobject_put(&private_data->kobj);
#ifdef ENABLE_SMBUF_POLL_STATUS
    if(private_data->usb_hub_task) {
	    kthread_stop(private_data->usb_hub_task);
	    private_data->usb_hub_task = NULL;
    }
#endif    
    
    rtk_usb_hub_set_default_dphy_mux(&pdev->dev, true);
    rtk_usb_hub_wrapper_clock_off(pdev);
    g_usb_hub_devices[private_data->id] = NULL;
    platform_set_drvdata(pdev, NULL);
    kfree(private_data);
    return 0;
}

static int rtk_usb_hub_platform_resume(struct device *dev)
{
    struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    if(rtk_usb_hub_init(pdev) == true) {
		if(get_product_type() != PRODUCT_TYPE_DIAS) {
			struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
			rtk_usb_hub_usp_enable(dev, private_data->usp_suspend_state);
		}
		return 0;
    }
    return -1;
}


static int rtk_usb_hub_platform_suspend(struct device *dev)
{
#ifdef ENABLE_SMBUF_POLL_STATUS
    struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    del_timer_sync(&private_data->mode_check_timer);
    private_data->usp_suspend_state = rtk_usb_hub_usp_get_state(dev);
    if(private_data->usb_hub_task) {
	    kthread_stop(private_data->usb_hub_task);
	    private_data->usb_hub_task = NULL;
    }
#endif
    return 0;
}

static RAW_NOTIFIER_HEAD(g_typec_source_switch_chain);
int register_typec_source_switch_notifier(struct notifier_block *nb)
{
        return raw_notifier_chain_register(&g_typec_source_switch_chain, nb);
}

int unregister_typec_source_switch_notifier(struct notifier_block *nb)
{
        return raw_notifier_chain_unregister(&g_typec_source_switch_chain, nb);
}


int typec_source_switch_notifier_call_chain(unsigned long val, void *v)
{
        return raw_notifier_call_chain(&g_typec_source_switch_chain, val, v);
}
EXPORT_SYMBOL(typec_source_switch_notifier_call_chain);


static const struct dev_pm_ops rtk_usb_hub_platform_pm_ops = {
    .resume = rtk_usb_hub_platform_resume,
    .suspend = rtk_usb_hub_platform_suspend,
};


static const struct of_device_id rtk_usb_hub_of_match[] = {
    { .compatible = "rtk,usb-hub-platform", },
    {},
};
MODULE_DEVICE_TABLE(of, rtk_usb_hub_of_match);


static struct platform_driver rtk_usb_hub_platform_driver = {
    .probe      = rtk_usb_hub_platform_probe,
    .remove     = rtk_usb_hub_platform_remove,
    .driver     = {
        .name = "rtk-usb-hub-platform",
        .pm = &rtk_usb_hub_platform_pm_ops,
        .of_match_table = rtk_usb_hub_of_match,
    }
};


static int __init rtk_usb_hub_platform_init(void)
{
    int ret = 0;
    ret = rtk_usb_hub_register_notifier();
    if(ret != 0) {
	  RTK_USB_HUB_ERR("rtk_usb_hub_register_notifier failed\n");
	  return ret;
    }
    ret = platform_driver_register(&rtk_usb_hub_platform_driver);
    if(ret != 0) {
        rtk_usb_hub_unregister_notifier();
        RTK_USB_HUB_ERR("Realtek USB HUB Platorm driver init failed\n");
    } else {
#ifdef ENABLE_SMBUF_POLL_STATUS    
	register_pm_notifier(&usb_hub_notifier);
#endif
    }
    return ret;
}
#ifdef CONFIG_RTK_KDRV_USB_HUB
rootfs_initcall(rtk_usb_hub_platform_init);
#endif
#ifdef CONFIG_RTK_KDRV_USB_HUB_MODULE
module_init(rtk_usb_hub_platform_init);
#endif


static void __exit rtk_usb_hub_platform_cleanup(void)
{
    rtk_usb_hub_unregister_notifier();
    platform_driver_unregister(&rtk_usb_hub_platform_driver);
#ifdef ENABLE_SMBUF_POLL_STATUS    
    unregister_pm_notifier(&usb_hub_notifier);
#endif
}
module_exit(rtk_usb_hub_platform_cleanup);


MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
