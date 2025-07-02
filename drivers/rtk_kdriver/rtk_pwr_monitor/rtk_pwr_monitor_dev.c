#include <linux/uaccess.h>
#include <linux/bitops.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/input.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kfifo.h>
#include <linux/kthread.h>
#include <linux/of.h>
#include <linux/pci.h>
#include <linux/poll.h>
#include <linux/platform_device.h>
#include <linux/random.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <mach/pcbMgr.h>
#include <linux/kernel.h>
#include <rtd_log/rtd_module_log.h>
#include <rtk_kdriver/i2c-rtk-api.h>
#include <rtk_kdriver/rtk-kdrv-common.h>

#define DRIVER_NAME "pwr_monitor"

int rtk_pwr_monitor_getValue_VI(unsigned char *pwr_value);
int rtk_pwr_monitor_getValue_All(unsigned char *pwr_value);

static struct timer_list pwr_monitor_tm;
static int pwr_monitor_period_count = 0;
static int pwr_monitor_init_enable = 0;

static
ssize_t pwr_monitor_read_raw_vi_dev_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int i;
	unsigned char pwr_data[16] = {0};
	if(rtk_pwr_monitor_getValue_VI(pwr_data) == 0) {
		for(i = 0; i < 12; i++) {
			rtd_pr_pwr_monitor_warn("[%d] = %x\n", i, pwr_data[i]);
		}
	}

	return 0;
}

static
ssize_t pwr_monitor_read_raw_vi_dev_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	rtd_pr_pwr_monitor_warn("\n(%s)no operation!\n",__func__);

	return 0;
}

DEVICE_ATTR(pwr_monitor_read_raw_vi, S_IRUGO | S_IWUSR,
		pwr_monitor_read_raw_vi_dev_show, pwr_monitor_read_raw_vi_dev_store);

static
ssize_t pwr_monitor_read_vi_dev_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int i;
	unsigned char pwr_data[16] = {0};
	int voltage_val, current_val;
	
	if(rtk_pwr_monitor_getValue_VI(pwr_data) == 0) {
		for(i = 0; i < 3; i++) {
			voltage_val = pwr_data[i * 4 + 0];
			voltage_val = (voltage_val << 8) | pwr_data[i * 4 + 1];
			voltage_val = (voltage_val * 4) / 8;
			current_val = pwr_data[i * 4 + 2];
			current_val = (current_val << 8) | pwr_data[i * 4 + 3];
			if(i == 0) {
				rtd_pr_pwr_monitor_warn("core power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (voltage_val * current_val) / 1000000, (voltage_val * current_val) % 1000000);
			}
			else if(i == 1) {
				rtd_pr_pwr_monitor_warn("SCPU power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (voltage_val * current_val) / 1000000, (voltage_val * current_val) % 1000000);
			}
			else {
				rtd_pr_pwr_monitor_warn("DDR power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (voltage_val * current_val) / 1000000, (voltage_val * current_val) % 1000000);
			}
		}
	}

	return 0;
}

static
ssize_t pwr_monitor_read_vi_dev_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	rtd_pr_pwr_monitor_warn("\n(%s)no operation!\n",__func__);

	return count;
}

DEVICE_ATTR(pwr_monitor_read_vi, S_IRUGO | S_IWUSR,
		pwr_monitor_read_vi_dev_show, pwr_monitor_read_vi_dev_store);

static
ssize_t pwr_monitor_read_raw_dev_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int i;
	unsigned char pwr_data[20] = {0};
	if(rtk_pwr_monitor_getValue_All(pwr_data) == 0) {
		for(i = 0; i < 18; i++) {
			rtd_pr_pwr_monitor_warn("[%d] = %x\n", i, pwr_data[i]);
		}
	}

	return 0;
}

static
ssize_t pwr_monitor_read_raw_dev_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	rtd_pr_pwr_monitor_warn("\n(%s)no operation!\n",__func__);

	return count;
}

DEVICE_ATTR(pwr_monitor_read_raw, S_IRUGO | S_IWUSR,
		pwr_monitor_read_raw_dev_show, pwr_monitor_read_raw_dev_store);

static
ssize_t pwr_monitor_read_dev_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int i;
	unsigned char pwr_data[20] = {0};
	int voltage_val, current_val, pwr_val;
	
	if(rtk_pwr_monitor_getValue_All(pwr_data) == 0) {
		for(i = 0; i < 3; i++) {
			voltage_val = pwr_data[i * 6 + 0];
			voltage_val = (voltage_val << 8) | pwr_data[i * 6 + 1];
			voltage_val = (voltage_val * 4) / 8;
			current_val = pwr_data[i * 6 + 2];
			current_val = (current_val << 8) | pwr_data[i * 6 + 3];
			pwr_val = pwr_data[i * 6 + 4];
			pwr_val = (pwr_val << 8) | pwr_data[i * 6 + 5];
			if(i == 0) {
				rtd_pr_pwr_monitor_warn("core power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (pwr_val * 2)/ 100, (pwr_val * 2) % 100);
			}
			else if(i == 1) {
				rtd_pr_pwr_monitor_warn("SCPU power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (pwr_val * 2)/ 100, (pwr_val * 2) % 100);
			}
			else {
				rtd_pr_pwr_monitor_warn("DDR power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (pwr_val * 2)/ 100, (pwr_val * 2) % 100);
			}
		}
	}

	rtd_pr_pwr_monitor_warn("[pwr_monitor_read_dev_show] run\n");
	return 0;
}

static
ssize_t pwr_monitor_read_dev_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	rtd_pr_pwr_monitor_warn("\n(%s)no operation!\n",__func__);

	return count;
}

DEVICE_ATTR(pwr_monitor_read, S_IRUGO | S_IWUSR,
		pwr_monitor_read_dev_show, pwr_monitor_read_dev_store);

//---
static int fTrigger_pwr_monitoir = 0;
static int fPwr_monitor_thread_running = 0;
static struct task_struct *Pwr_monitor_thread = NULL;

static int thread_pwr_monitor(void *arg)
{
	int i;
	unsigned char pwr_data[20] = {0};
	int voltage_val, current_val, pwr_val;
	
	rtd_pr_pwr_monitor_warn("Start pwr monitor thread\n");
	while (!kthread_should_stop()) {
		ssleep(1); // Sleep for 1 second
		
		if(fTrigger_pwr_monitoir) {
			if(rtk_pwr_monitor_getValue_All(pwr_data) == 0) {
				fTrigger_pwr_monitoir = 0;
				for(i = 0; i < 3; i++) {
					voltage_val = pwr_data[i * 6 + 0];
					voltage_val = (voltage_val << 8) | pwr_data[i * 6 + 1];
					voltage_val = (voltage_val * 4) / 8;
					current_val = pwr_data[i * 6 + 2];
					current_val = (current_val << 8) | pwr_data[i * 6 + 3];
					pwr_val = pwr_data[i * 6 + 4];
					pwr_val = (pwr_val << 8) | pwr_data[i * 6 + 5];
					if(i == 0) {
						rtd_pr_pwr_monitor_warn("core power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (pwr_val * 2)/ 100, (pwr_val * 2) % 100);
					}
					else if(i == 1) {
						rtd_pr_pwr_monitor_warn("SCPU power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (pwr_val * 2)/ 100, (pwr_val * 2) % 100);
					}
					else {
						rtd_pr_pwr_monitor_warn("DDR power: voltage = %d.%d V, current = %d.%d A, power = %d.%d W\n", voltage_val/1000, voltage_val%1000, current_val/1000, current_val%1000, (pwr_val * 2)/ 100, (pwr_val * 2) % 100);
					}
				}
				//rtd_pr_pwr_monitor_warn("trigger pwr monitor thread\n");
			}
		}
	}
	rtd_pr_pwr_monitor_warn("Stop pwr monitor thread\n");
	return 0;
}

static
ssize_t pwr_monitor_period_dev_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	//unsigned char pwr_data[16] = {0};
	//if(rtk_pwr_monitor_getValue_VI(pwr_data) == 0) {
	//	for(i = 0; i < 12; i++) {
	rtd_pr_pwr_monitor_warn("pwr measure period = %d\n", pwr_monitor_period_count);

	return 0;
}

static
ssize_t pwr_monitor_period_dev_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	//char command[256];
	unsigned long long pwr_monitor_get_period = 0;	

	if (kstrtoull(buf, 10, &pwr_monitor_get_period)){
		rtd_pr_pwr_monitor_err("get param fail\n");
		return count;
	}

	if(pwr_monitor_period_count) {
		if(pwr_monitor_get_period == 0) {
			del_timer_sync(&pwr_monitor_tm);
			pwr_monitor_period_count = pwr_monitor_get_period;
			kthread_stop(Pwr_monitor_thread);
			fPwr_monitor_thread_running = 0;
			rtd_pr_pwr_monitor_warn("pwr monitor thread stopped\n");			
		}
		else {
			if(pwr_monitor_period_count != pwr_monitor_get_period) {
				del_timer_sync(&pwr_monitor_tm);
			}
			pwr_monitor_period_count = pwr_monitor_get_period;
			mod_timer(&pwr_monitor_tm, (jiffies + HZ * pwr_monitor_period_count) );
			if (!fPwr_monitor_thread_running) {
				Pwr_monitor_thread = kthread_run(thread_pwr_monitor, NULL, "pwr_monitor_thread");
				if (IS_ERR(Pwr_monitor_thread)) {
					rtd_pr_pwr_monitor_err("Failed to create pwr monitor thread\n");
					return count;
				}
				fPwr_monitor_thread_running = 1;
			}
		}
	}
	else {
		if(pwr_monitor_get_period) {
			pwr_monitor_period_count = pwr_monitor_get_period;
			mod_timer(&pwr_monitor_tm, (jiffies + HZ * pwr_monitor_period_count) );
			if (!fPwr_monitor_thread_running) {
				Pwr_monitor_thread = kthread_run(thread_pwr_monitor, NULL, "pwr_monitor_thread");
				if (IS_ERR(Pwr_monitor_thread)) {
					rtd_pr_pwr_monitor_err("Failed to create pwr monitor thread\n");
					return count;
				}
				fPwr_monitor_thread_running = 1;
			}
		}
	}
	
	rtd_pr_pwr_monitor_warn("get pwr measure period = %d\n", pwr_monitor_period_count);

	return count;
}

DEVICE_ATTR(pwr_monitor_period, S_IRUGO | S_IWUSR,
		pwr_monitor_period_dev_show, pwr_monitor_period_dev_store);

void pwr_monitor_period_callback(struct timer_list *arg)
{
	rtd_pr_pwr_monitor_warn("\n[%s] : %lu--\n", __func__, jiffies);
	fTrigger_pwr_monitoir = 1;
	
	if(pwr_monitor_period_count) {	
		mod_timer(&pwr_monitor_tm, (jiffies + HZ * pwr_monitor_period_count) );
	}
}

// ===========
#if 0
static int emcu_drv_suspend(struct device *dev)
{
    rtd_pr_pwr_monitor_info("%s(%d)\n",__func__,__LINE__);
	return 0;
}


static int emcu_drv_resume(struct device *dev)
{
    rtd_pr_pwr_monitor_info("%s(%d)\n",__func__,__LINE__);
	return 0;		
}
#endif
// ===========

static int pwr_monitor_dev_open(struct inode *inode, struct file *file)
{
    rtd_pr_pwr_monitor_info("Power monitor open\n");

    return 0;     /* success */
}

static long rtk_pwr_monitor_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	rtd_pr_pwr_monitor_info("%s(%d) \n",__func__,__LINE__);
	return 0;
}

static int pwr_monitor_dev_close(struct inode *inode, struct file *file)
{
    rtd_pr_pwr_monitor_info("Power monitor close\n");

    return 0;     /* success */
}

struct file_operations pwr_monitor_dev_fops = {
	.owner = THIS_MODULE,
    .open = pwr_monitor_dev_open,
	.unlocked_ioctl = rtk_pwr_monitor_ioctl,
    .release = pwr_monitor_dev_close,
};

static struct miscdevice rtk_pwr_monitor_miscdev = {
	MISC_DYNAMIC_MINOR,
	"pwr_monitor",
	&pwr_monitor_dev_fops
};

static struct platform_device pwr_monitor_drv_pm_device = {
    .name          = DRIVER_NAME,
    .id            = -1,
};

static struct platform_driver pwr_monitor_drv_pm_driver = {
    .driver    = {
        .name  = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
};

static int pwr_monitor_init(void)
{
	int ret = 0;
	char strings[20] = {0};	
	unsigned long long pmic_power_info = 0;

	if(rtk_parse_commandline_equal("pwr_monitor", strings, sizeof(strings))) {
		pwr_monitor_init_enable = simple_strtoul(strings, NULL, 10);
		rtd_pr_pwr_monitor_warn("Init flag pass from bootcode  %x\n", pwr_monitor_init_enable);	
		if(pwr_monitor_init_enable == 1) {
		
			// Get PCB info passed by bootcode
			ret = pcb_mgr_get_enum_info_byname("PMIC_POWER_INFO", &pmic_power_info);
			if (ret != 0) {
				rtd_pr_pwr_monitor_err("Not exist PMIC_POWER_INFO info\n");
				ret = -ENODEV;
				goto error;
			}
			else {
				rtd_pr_pwr_monitor_info("Get PMIC_POWER_INFO info = %lx\n", pmic_power_info);
				
				if (misc_register(&rtk_pwr_monitor_miscdev)) {
					rtd_pr_pwr_monitor_err("rtk_pwr_monitor_module_init failed - register misc device failed\n");
					ret = -ENODEV;
					goto error;
				}
				// mount system file
				device_create_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read_raw);
				device_create_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read);
				device_create_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read_raw_vi);
				device_create_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read_vi);
				device_create_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_period);

				ret = platform_device_register(&pwr_monitor_drv_pm_device);
				if (ret) {
					rtd_pr_pwr_monitor_err("%s register driver error.\n", DRIVER_NAME);
					goto error;
				}

				ret = platform_driver_probe(&pwr_monitor_drv_pm_driver, NULL);
				if (ret) {
					rtd_pr_pwr_monitor_err("%s probe driver error.\n", DRIVER_NAME);
					goto error;
				}
				
				timer_setup(&pwr_monitor_tm, pwr_monitor_period_callback,0);

				rtd_pr_pwr_monitor_info("%s driver installed.\n", DRIVER_NAME);

			}
		}
	}

error:
	return (ret);
}

static void pwr_monitor_exit(void)
{
	if(pwr_monitor_init_enable) {
		del_timer(&pwr_monitor_tm);
		pwr_monitor_period_count = 0;

		platform_driver_unregister(&pwr_monitor_drv_pm_driver);
		device_remove_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read_raw);
		device_remove_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read);
		device_remove_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read_raw_vi);
		device_remove_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_read_vi);
		device_remove_file(rtk_pwr_monitor_miscdev.this_device, &dev_attr_pwr_monitor_period);

		rtd_pr_pwr_monitor_info("%s driver removed.\n", DRIVER_NAME);
		pwr_monitor_init_enable = 0;
	}
}

module_init(pwr_monitor_init);
module_exit(pwr_monitor_exit);
MODULE_AUTHOR("Weihao Lo <weihao@realtek.com.tw>");
MODULE_DESCRIPTION("Realtek series power monitor");
MODULE_LICENSE("GPL v2");
