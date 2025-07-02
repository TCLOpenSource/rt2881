#include <linux/module.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/reboot.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sched/signal.h>
#include <linux/gpio.h>
#include <rtk_kdriver/rtk_crt.h>
#include <rtk_kdriver/rtk_gpio.h>
#include "../gpio/rtk_gpio-reg.h"
#include "panelapi.h"
#include <asm/irq_regs.h>
#include "rtk_emcu_export.h"
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

#define KEY_NETFLIX  0x236

#define RTK_BACKLIGHT_CONTROL "rtk_bl"
static int __initdata quiescent_backlight_flow = 0; /* 0: normal state, 1: enter quiescent_backlight */
static spinlock_t bl_spinlock;
static int quiescent_status = 0; /* 0: quiescent exit, 1: quiescent enter */
static int bl_handler_ref = 0;
static struct work_struct bl_work;
extern int ir_protocol;
extern int IO_Direct_Set(char *enum_name, int value);

#define quiescent_status_PROC_DIR                        "tcl_standby_qhb"
#define quiescent_status_PROC_ENTRY                      "status"

struct proc_dir_entry *quiescent_status_proc_dir;
struct proc_dir_entry *quiescent_status_proc_entry;

static int status_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%d\n", quiescent_status);
    pr_info("rtk_bl.c status_read quiescent_status=%d \n",quiescent_status);
    return 0;
}

static int status_open(struct inode *inode, struct file *file)
{
    return single_open(file, status_show, NULL);
}

static ssize_t status_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char buffer[32];
    unsigned long val;
    int ret;

    if (count >= sizeof(buffer))
        return -EINVAL;

    memset(buffer, 0, sizeof(buffer));
    if (copy_from_user(buffer, buf, count))
        return -EFAULT;

    ret = kstrtoul(buffer, 0, &val);
    if (ret)
        return ret;

    quiescent_status = val;
    pr_info("rtk_bl.c status_write quiescent_status=%d \n",quiescent_status);

    return count;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static struct proc_ops  quiescent_status_fops = {
    .proc_open = status_open,
    .proc_write  = status_write,
    .proc_read  = seq_read,
};
#else
static const struct file_operations quiescent_status_fops = {
    .owner = THIS_MODULE,
    .open = status_open,
    .read = seq_read,
    .write = status_write,
};
#endif


bool quiescent_status_init_proc(void)
{
    if (quiescent_status_proc_dir == NULL && quiescent_status_proc_entry == NULL) {
        quiescent_status_proc_dir = proc_mkdir(quiescent_status_PROC_DIR , NULL);

        if (quiescent_status_proc_dir != NULL) {
            quiescent_status_proc_entry =
                proc_create(quiescent_status_PROC_ENTRY, 0666,
                    quiescent_status_proc_dir, &quiescent_status_fops);

            if (quiescent_status_proc_entry == NULL) {
                proc_remove(quiescent_status_proc_dir);
                quiescent_status_proc_dir = NULL;
                return false;
            }

        } else {
            return false;
        }
    }

    return true;
}

void quiescent_status_uninit_proc(void)
{
    if (quiescent_status_proc_entry) {
        proc_remove(quiescent_status_proc_entry);
        quiescent_status_proc_entry = NULL;
    }

    if (quiescent_status_proc_dir) {
        proc_remove(quiescent_status_proc_dir);
        quiescent_status_proc_dir = NULL;
    }
}


static int bl_flag_set(const char *val, const struct kernel_param *kp)
{
	int ret;
	unsigned long flags;

	ret = param_set_int(val, kp);
	if (ret < 0) {
		pr_err("bl flag set failed\n");
		return ret;
	}

	if (0 == quiescent_status) {
		spin_lock_irqsave(&bl_spinlock, flags);
		if (bl_handler_ref) {
			pr_info("scheduel bl exit work\n");
			bl_handler_ref = 0;
			schedule_work(&bl_work);
		}
		spin_unlock_irqrestore(&bl_spinlock, flags);
	}  else {
		pr_err("bl flag set invalid val\n");
	}

	return 0;
}

static int bl_flag_get(char *buf, const struct kernel_param *kp)
{
	return sprintf(buf, "%d\n", quiescent_status);
}

static const struct kernel_param_ops bl_flow_ops = {
	.set = bl_flag_set,
	.get = bl_flag_get,
};
module_param_cb(BLFlow, &bl_flow_ops, &quiescent_status, 0644);
/*
static int __init setup_bl_flow(char *str)
{
	int ret = 0;
	if (!str)
		goto out;

	if (!strcmp(str, "yes")) {
		quiescent_backlight_flow = 1;
		ret = 1;
	}

out:
	if (!ret)
		pr_info("Unable to prase BLFlow=\n");
	return ret;
}
__setup("BLFlow=", setup_bl_flow);*/


static void quiescent_backlight_event(struct input_handle *handle, unsigned int type,
			unsigned int code, int value)
{
	unsigned long flags;
	unsigned int scancode = 0;
	spin_lock_irqsave(&bl_spinlock, flags);

	/* When the power key is pressed in special quiescent mode,
	 * we need to turn on the backlight */
	if (quiescent_status && (type == EV_KEY) && (!value) && ((code == KEY_POWER)||(code == KEY_NETFLIX))){
		quiescent_status = 0;
		//pr_info("set backlight on in rtk_bl.c \n");
		//Panel_SetPanelOnOff(1);
		//Panel_SetBackLightMode(1);
		pr_info("before set backlight on in rtk_bl.c backlight_reg=%x \n",rtd_inl(0xb8061120));
		IO_Direct_Set("PIN_BL_ON_OFF", 1);
		pr_info("after set backlight on in rtk_bl.c backlight_reg=%x \n",rtd_inl(0xb8061120));

		IO_Direct_Set("PIN_USB_5V_EN", 1);
		if (bl_handler_ref) {
			pr_info("scheduel bl exit work\n");
			bl_handler_ref = 0;
			schedule_work(&bl_work);
		}
		if(strcmp(handle->dev->name, "venus_IR_input") == 0) {
			if(ir_protocol == 8) //RCA: power
			{
				if(code == KEY_POWER)
					scancode = 0xab054f00;
				else if(code == KEY_NETFLIX)
					scancode = 0x080f7f00;
			}
			else if(ir_protocol == 18) //NEC: power
			{
				if(code == KEY_POWER)
					scancode = 0x2ad59966;
				else if(code == KEY_NETFLIX)
					scancode = 0xef10b916;
			}

			powerMgr_update_wakeup_reason(3, scancode, 1); //parameter 0: type, parameter 1: scancode, parameter 2: quiescent mode flag
		}
		else if(strcmp(handle->dev->name, "rtk_keypad") == 0)
		{
			powerMgr_update_wakeup_reason(1, scancode, 1); //parameter 0: type, parameter 1: scancode, parameter 2: quiescent mode flag
		}
	}
	spin_unlock_irqrestore(&bl_spinlock, flags);
	return;
}

static int quiescent_backlight_connect(struct input_handler *handler, struct input_dev *dev,
			const struct input_device_id *id)
{
	struct input_handle *handle;
	int err;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	spin_lock_init(&bl_spinlock);

	handle->dev = dev;
	handle->handler = handler;
	handle->name = RTK_BACKLIGHT_CONTROL;

	err = input_register_handle(handle);
	if (err)
		goto err_free_handle;

	err = input_open_device(handle);
	if (err)
		goto err_unregister_handle;

	pr_info("Connected device: %s (%s at %s)\n",
			dev_name(&dev->dev),
			dev->name ?: "unknown",
			dev->phys ?: "unknown");

	return 0;

err_unregister_handle:
	input_unregister_handle(handle);
err_free_handle:
	kfree(handle);
	return err;
}

static void quiescent_backlight_disconnect(struct input_handle *handle)
{
	pr_info("Disconnected device: %s\n", dev_name(&handle->dev->dev));

	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);

}

static const struct input_device_id input_bl_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT,
		.evbit = { BIT_MASK(EV_KEY) },
	},
	{ },
};
MODULE_DEVICE_TABLE(input, input_bl_ids);

static struct input_handler input_bl_handler = {
	.event = quiescent_backlight_event,
	.connect = quiescent_backlight_connect,
	.disconnect = quiescent_backlight_disconnect,
	.name = RTK_BACKLIGHT_CONTROL,
	.id_table = input_bl_ids,
};

static void bl_exit_work(struct work_struct *wk)
{
	pr_info("bl exit unregister input handler\n");
	input_unregister_handler(&input_bl_handler);
}
extern bool rtk_parse_commandline_equal(const char *string, char *output_string, int string_size);
static int __init quiescent_backlight_init(void)
{
	char str[30] = {0};
	if (rtk_parse_commandline_equal("quiescent_mode", str, sizeof(str)))
	{
		if(strcmp(str,"quiescent_speedup_ac_on")==0)
			quiescent_backlight_flow = 1;
	}
	pr_info("quiescent_backlight_flow = %d \n",quiescent_backlight_flow);
	if (quiescent_backlight_flow) {
		pr_info("backlight input init\n");
		quiescent_status_init_proc();
		quiescent_status = 1;
		bl_handler_ref = 1;
		INIT_WORK(&bl_work, bl_exit_work);
		return input_register_handler(&input_bl_handler);
	}
	else
		return 0;
}

static void __exit quiescent_backlight_exit(void)
{
	input_unregister_handler(&input_bl_handler);
	quiescent_status_uninit_proc();
}

module_init(quiescent_backlight_init);
module_exit(quiescent_backlight_exit);

MODULE_LICENSE("GPL");

