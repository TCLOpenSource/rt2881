#include <linux/version.h>
#include <linux/compiler.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include <rtd_log/rtd_module_log.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk_qos_export.h>
#include <rtk_kdriver/rtd_log.h>

#include <rbus/dc_sys_reg.h>
#include <rbus/ppoverlay_reg.h>

#define XTAL_CLK 27000000

#if IS_ENABLED(CONFIG_ARCH_RTK6748)

#define FIFO_LINE                   16
#define FIFO_LEN                    16

#define BANK_MODE_reg               0xB8007844
#define DATA_LATCH_reg              0xB80C2C00
#define DATA_CTRL_reg               0xB80C2C04
#define DATA_FIFO_reg               0xB80C2C44
#define DATA_STAT1_reg              0xB80C2C48
#define DATA_STAT2_reg              0xB80C2C4C

static int bwinfo_get(void)
{
	unsigned int line, len;
	int fr, cnt, total = 0;
	unsigned long reg;

	// clear fifo
	rtd_setbits(DATA_FIFO_reg, 0x20000000);
	while (rtd_inl(DATA_FIFO_reg) & 0x20000000) {
	}

	// set latch mode
	rtd_outl(DATA_LATCH_reg, 0x00000001);

	// set fifo channel
	reg = rtd_inl(DATA_FIFO_reg);
	reg |= 0x40000000;
	if (rtd_inl(BANK_MODE_reg) & 0x70707070) {
		reg |= 0x10000000;
	}
	else {
		reg &= ~(0x10000000);
		reg |= 0x0C000000;
	}
	reg &= ~(0x03000000);
	rtd_outl(DATA_FIFO_reg, reg);

	// fifo enable
	rtd_setbits(DATA_FIFO_reg, 0x80000000);
	rtd_clearbits(DATA_FIFO_reg, 0x40000000);

	// wait fifo ready
	while ((rtd_inl(DATA_FIFO_reg) & 0x0F) != 0xF) {
		msleep(1);
	}

	// data prediction & analysis
	reg = rtd_inl(DATA_CTRL_reg);
	reg = (reg | 0x80000000 | 0x00008000) & ~(0x000007FF);
	rtd_outl(DATA_CTRL_reg, reg);
	while (rtd_inl(DATA_CTRL_reg) & 0x00008000) {
	}

	if ((rtd_inl(DATA_FIFO_reg) & 0x01) != 0x1) {
		rtd_pr_qos_err("[UTIL] fifo refreshed\n");
		total = 0;
		goto end;
	}
	for (line = 0; line < FIFO_LINE; line++) {

		for (len = 0; len < FIFO_LEN; len++) {
			while ((rtd_inl(DATA_STAT1_reg) & 0x80000000) == 0) {
			}

			cnt = (rtd_inl(DATA_STAT1_reg) & 0x7FFFFFFF) + (rtd_inl(DATA_STAT2_reg) & 0xFFFFFFFF);
			total += cnt;
		}
	}

end:
	// fifo disable
	rtd_setbits(DATA_FIFO_reg, 0x40000000);
	rtd_clearbits(DATA_FIFO_reg, 0x80000000);

	fr = rtd_inl(PPOVERLAY_memcdtg_DVS_cnt_reg);
	return (total / 1000 * 16 * ( XTAL_CLK / fr ) / 1000);
}


static int bwinfo_max(void)
{
	unsigned int reg_val, mem_num, mem_num_dc2, ddr_bitwidth = 0;

	reg_val = rtd_inl(DC_SYS_DC_SYS_MISC_reg);
	mem_num = DC_SYS_DC_SYS_MISC_get_mem_num(reg_val);
	mem_num_dc2 = DC_SYS_DC_SYS_MISC_get_mem_num_dc2(reg_val);
	ddr_bitwidth += mem_num ? 32 : 16;
	ddr_bitwidth += mem_num_dc2 ? 32 : 16;

	if (ddr_bitwidth == 48) {
		return 11841;
	}
	else { // 32bits
		return 7763;
	}
}
#endif // IS_ENABLED(CONFIG_ARCH_RTK6748)


static ssize_t bwinfo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
#if IS_ENABLED(CONFIG_ARCH_RTK6748)

	if (strcmp(attr->attr.name, "used") == 0)
	{
		int bw_info;
		int len = 0;

		bw_info = bwinfo_get();
		if (bw_info >= 0) {
			len += sprintf(buf+len, "%d\n", bw_info);
			return len;
		}
	}
	else if (strcmp(attr->attr.name, "total") == 0)
	{
		int bw_max = bwinfo_max();
		return sprintf(buf, "%d\n", bw_max);
	}

#endif

	return 0;
}


static ssize_t bwinfo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
#if IS_ENABLED(CONFIG_ARCH_RTK6748)

	if (strcmp(attr->attr.name, "used") == 0) {
		//
	}
	else if (strcmp(attr->attr.name, "total") == 0) {
		//
	}
#endif

	return 0;
}


static struct kobj_attribute used_attr = __ATTR(used, 0644, bwinfo_show, bwinfo_store);
static struct kobj_attribute total_attr = __ATTR(total, 0644, bwinfo_show, bwinfo_store);
static struct attribute *bwinfo_attrs[] = {
	&used_attr.attr,
	&total_attr.attr,
	NULL,
};
static struct attribute_group bwinfo_attr_group = {
	.attrs = bwinfo_attrs,
};

__maybe_unused int __init rtk_util_init(void)
{
	int retval;
	struct kobject *bwinfo_kobj;

	// /sys/<bw>/
	bwinfo_kobj = kobject_create_and_add("ddr_bandwidth", NULL);
	if (!bwinfo_kobj) {
		rtd_pr_qos_err("%s[%d] create sysfs failed\n", __func__, __LINE__);
		return -ENOMEM;
	}
	retval = sysfs_create_group(bwinfo_kobj, &bwinfo_attr_group);
	if (retval) {
		kobject_put(bwinfo_kobj);
		rtd_pr_qos_err("%s[%d] create sysfs node failed, error %d\n", __func__, __LINE__, retval);
	}

	return retval;
}


//late_initcall(rtk_util_init);

MODULE_AUTHOR("Realtek.com");
MODULE_LICENSE("Dual BSD/GPL");

