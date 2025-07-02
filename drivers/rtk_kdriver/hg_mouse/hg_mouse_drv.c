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
#include <linux/miscdevice.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/pageremap.h>
#include <linux/compat.h>
#include <linux/input.h>

#include "rtk_otp_region_api.h"
#include <rtd_log/rtd_module_log.h>

#define MOUSE_DEBUG(fmt, args...)  rtd_pr_ai_dbg_notice("[hg_mouse]"fmt, ## args)
#define MOUSE_INFO(fmt, args...)   rtd_pr_ai_dbg_info("[hg_mouse]" fmt, ## args)
#define MOUSE_WARN(fmt, args...)   rtd_pr_ai_dbg_warn("[hg_mouse]" fmt, ## args)
#define MOUSE_ERROR(fmt, args...)  rtd_pr_ai_dbg_err("[hg_mouse]" fmt, ## args)


#define HG_MOUSE_DEVICE_NAME "hg_mouse"
#define IS_FCC(a, b) (*((u32*)(a)) == *((u32*)(b)))

static struct hg_mouse {
	struct input_dev *indev;
	int btn_press;
	int btn_rel;
} mouse;

static DEFINE_MUTEX(mouse_mutex);

static int hg_mouse_init(void)
{
	struct input_dev *indev;

	indev = input_allocate_device();
	if (!indev) {
		MOUSE_ERROR("[%s %d] input_allocate_device fails\n", __func__, __LINE__);
		return -ENODEV;
	}

	indev->name = HG_MOUSE_DEVICE_NAME;

	indev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
	indev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) | BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
	indev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);
	//indev->keybit[BIT_WORD(BTN_MOUSE)] |= BIT_MASK(BTN_SIDE) | BIT_MASK(BTN_EXTRA);
	indev->relbit[0] |= BIT_MASK(REL_WHEEL);

	indev->evbit[0] |= BIT_MASK(EV_MSC);
	indev->mscbit[0] = BIT_MASK(EV_MSC);

	if (input_register_device(indev)) {
		input_free_device(indev);
		MOUSE_ERROR("[%s %d] input_register_device fails\n", __func__, __LINE__);
		return -ENODEV;
	}
	mouse.indev = indev;
	return 0;
}

static int hg_mouse_deinit(void)
{
	if (mouse.indev)
		input_unregister_device(mouse.indev);

	return 0;
}
static int hg_mouse_dummy(struct inode *inode, struct file *filp)
{

        return 0;
}


static ssize_t hg_mouse_exec(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
#define str_size  256
	char data[str_size];
	char *cmd = data;
	char *token;
	s32 value[16] = {0};
	int i = 0;

	if (count > str_size) {
		MOUSE_ERROR("[%s %d] overflow\n", __func__, __LINE__);
		return count;
	}

        if (copy_from_user(data, buf, count)) {
                //AISCHED_ERROR("[%s %d]copy cmd fail\n", __func__, __LINE__);
                return count;
        }

	while ((token = strsep(&cmd, " "))) {
		if (kstrtoint(token, 0, &value[i]))
			break;

		MOUSE_DEBUG("[%d][%d]\n", i, value[i]);
		i++;
	}

	mouse.btn_press = value[2] | value[3] | value[4];
	if (mouse.btn_press || mouse.btn_rel)
		input_event(mouse.indev, EV_MSC, MSC_SCAN, 589825);

	mouse.btn_rel = mouse.btn_press;

	input_report_rel(mouse.indev, REL_X,      value[0]);
	input_report_rel(mouse.indev, REL_Y,      value[1]);
	input_report_key(mouse.indev, BTN_LEFT,   value[2]);
	input_report_key(mouse.indev, BTN_RIGHT,  value[3]);
	input_report_key(mouse.indev, BTN_MIDDLE, value[4]);

	if (!mouse.btn_press && !mouse.btn_rel)
		input_report_rel(mouse.indev, REL_WHEEL,  value[5]);


	input_sync(mouse.indev);
	return count;

}

static ssize_t hg_mouse_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t c;

	mutex_lock(&mouse_mutex);
	c = hg_mouse_exec(filp, buf, count, f_pos);
	mutex_unlock(&mouse_mutex);

	return c;
}

static struct file_operations hg_mouse_fops = {
        .owner = THIS_MODULE,
        .write = hg_mouse_write,
        //.unlocked_ioctl = ai_sched_ioctl,
        .release = hg_mouse_dummy,
        .open = hg_mouse_dummy,
#ifdef CONFIG_COMPAT
        //.compat_ioctl = ai_sched_compat_ioctl,
#endif
};


static struct miscdevice hg_mouse_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = HG_MOUSE_DEVICE_NAME,
	.fops = &hg_mouse_fops,
	.mode = 0666,
};

static int __init hg_mouse_module_init(void)
{
	if (rtk_is_NNIP_function_disable()) {
		MOUSE_ERROR("[%s %d] NO NPU\n", __func__, __LINE__);
		return -ENODEV;
	}

	if (hg_mouse_init()) {
		MOUSE_ERROR("[%s %d] hg_mouse_init fails\n", __func__, __LINE__);
		return -ENODEV;
	}

	if (misc_register(&hg_mouse_miscdev)) {
		MOUSE_ERROR("[%s %d] misc_register fails\n", __func__, __LINE__);
		return -ENODEV;
	}

	MOUSE_DEBUG("[%s %d] done\n", __func__, __LINE__);
	return 0;
}


static void __exit hg_mouse_module_exit(void)
{
	misc_deregister(&hg_mouse_miscdev);
	hg_mouse_deinit();
}

module_init(hg_mouse_module_init);
module_exit(hg_mouse_module_exit);
MODULE_AUTHOR("stream, Realtek Semiconductor");
MODULE_LICENSE("GPL");
