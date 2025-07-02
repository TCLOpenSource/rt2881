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
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/pageremap.h>
#include <linux/compat.h>

#include <ioctrl/ai/ai_scheduling_cmd_id.h>
#include "rtk_otp_region_api.h"

#include <ai_scheduling.h>

DEFINE_MUTEX(aisched_ioctl_mutex);

static int ai_sched_open(struct inode *inode, struct file *filp)
{
	AI_SCHED_GKID *gid = (AI_SCHED_GKID*)vzalloc(sizeof(AI_SCHED_GKID));

	if (gid == 0)
		return -ENOMEM;

	gid->pid = task_tgid_vnr(current);
	gid->tid = task_pid_vnr(current);
	filp->private_data = (void*)gid;
	AISCHED_DEBUG("[%s] pid = %d, tid = %d, filp = %p\n",__func__, gid->pid, gid->tid, filp);


	return 0;
}

static int ai_sched_release(struct inode *inode, struct file *filp)
{
	AI_SCHED_GKID *gid = (AI_SCHED_GKID*)filp->private_data;

	if (gid == 0)
		return 0;

	ai_sched_cancel(*gid);
	AISCHED_DEBUG("[%s] pid = %d, tid = %d, filp = %p\n",__func__, gid->pid, gid->tid, filp);
	vfree(gid);
	filp->private_data = 0;

	return 0;
}
static long ai_sched_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret;


	mutex_lock(&aisched_ioctl_mutex);
	ret = ai_sched_ioctl_impl(filp, cmd, arg);
	mutex_unlock(&aisched_ioctl_mutex);

	return ret;
}

#ifdef CONFIG_COMPAT
static long ai_sched_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	void __user *compat_arg = compat_ptr(arg);
	return ai_sched_ioctl(filp, cmd, (unsigned long)compat_arg);
}
#endif


static ssize_t ai_sched_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    mutex_lock(&aisched_ioctl_mutex);
    ai_sched_proc(filp, buf, count, f_pos);
    mutex_unlock(&aisched_ioctl_mutex);
    return count;
}

static struct file_operations ai_sched_fops = {
	.owner = THIS_MODULE,
	.write = ai_sched_write,
	.unlocked_ioctl = ai_sched_ioctl,
	.release = ai_sched_release,
	.open = ai_sched_open,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ai_sched_compat_ioctl,
#endif
};



static struct miscdevice ai_sched_miscdev = {
	MISC_DYNAMIC_MINOR,
	AI_SCHED_DEVICE_NAME,
	&ai_sched_fops
};

static int __init ai_sched_module_init(void)
{
	AISCHED_DEBUG("[%s] start\n",__func__);

	if (rtk_is_NNIP_function_disable()) {
		AISCHED_ERROR("[%s] NNIP is disabled\n",__func__);
		return -ENODEV;
	}

	if(ai_sched_init())
		return -ENODEV;

	if (misc_register(&ai_sched_miscdev))
		return -ENODEV;

	AISCHED_DEBUG("[%s] done\n",__func__);
	return 0;

}

static void __exit ai_sched_module_exit(void)
{
	misc_deregister(&ai_sched_miscdev);
	ai_sched_deinit();
}


module_init(ai_sched_module_init);
module_exit(ai_sched_module_exit);
MODULE_AUTHOR("stream, Realtek Semiconductor");
MODULE_LICENSE("GPL");


