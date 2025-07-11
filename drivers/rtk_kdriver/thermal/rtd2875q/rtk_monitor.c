/* rtk hw monitor
 *
 * Created by: davidwang@realtek.com
 * Copyright: (C) 2015 Realtek Semiconductor Corp.
 *
 *
 */

#include <rtd_log/rtd_module_log.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/time.h>
#include <linux/kobject.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 12, 1))
#include <linux/sched/types.h>
#endif
#include <uapi/linux/sched/types.h>
#include <linux/sched/clock.h>
#include <linux/delay.h>
#include <linux/err.h> /* IS_ERR() PTR_ERR() */
#include <asm/io.h>    /* rtd_inl() rtd_outl() ... */
#include <rtk_kdriver/io.h>
#if (defined CONFIG_RTK_KDRIVER) || (defined CONFIG_RTK_KDRIVER_SUPPORT)
#include <rtk_kdriver/rtk_thermal_sensor.h>
#include <rtk_kdriver/rtk_gpio.h>
#else
#include <mach/rtk_thermal_sensor.h>
#include <mach/rtk_gpio.h>
#endif
#include <linux/freezer.h>
#include <rbus/stb_reg.h>
static DEFINE_MUTEX(rtk_monitor_activation_lock);
static DEFINE_MUTEX(rtk_monitor_delay_lock);
static unsigned int rtk_monitor_active = 0;
static unsigned int rtk_monitor_delay = 1000; // ms
unsigned int rtk_monitor_debug = 0;
#define RTK_MONITOR_THERMAL_TEMP_PRINT_THRESHOLD 120
#define RTK_MONITOR_THERMAL_TEMP_ZQ_THRESHOLD    10  // Celsius

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>

//disable for bringup
#define CONFIG_RTK_ZQ_ENABLE

static unsigned int	ZQ_init_flag = 0;
static struct task_struct *rtk_monitor_task;
static unsigned long pre_jiffies;

#define DPI1_PAD_CTRL_PROG 0xb80c9130
#define DPI1_PAD_ZCTRL_STATUS 0xb80c9140
#define DPI1_PAD_BUS_1 0xb80c91f8

//dummy reg.
#define ZQFW_CTRL1 0xb80c2750
#define ZQFW_CTRL2 0xb80c2754

//2MC ref sync
#define MC1_SYS_REF_1 0xb80c2064
#define MC2_SYS_REF_1 0xb80c3064
unsigned int test_mode=0, ZQ_Config=0;
unsigned long long now, start;

#ifdef CONFIG_RTK_ZQ_ENABLE

static void ZQ_VREF_SET(unsigned int set_num)
{

	if(set_num<=3) //SET0~3
	{
		//0.5VDD cal. for CK/CA
		rtd_maskl(0xb80c9238,0xfffffffc,0x00000002);    //fw_set delay[1:0]=2 immediately update
		rtd_outl(0xb80c913c,0x00000000);    //zq_ena_nocd2[0]=0 disable NOCD2 for 0.5VDD
		rtd_maskl(0xb80c9000,0xffffefff,0x00000000);    //zq_ddr3_mode[12]=0 for 0.5VDD
		rtd_maskl(0xb80c91f4,0x0003ffff,0x82b80000);    //Vref_range[27]=0:28%~92%,ZQ_Vref[26:21]=0x15->50%
		rtd_outl(0xb80c9130,0x04080c99);    //[27]=0:disable zq auto update,[26:25]=2'b10:zclk/32,[19]=1:R240 external,zq_cal_cmd[7]=1
		rtd_maskl(0xb80c9130,0xffff80ff,0x00007f00);
		rtd_outl(0xb80c923c,0x00000003);    //update write delay tap
		udelay(10);
		rtd_maskl(0xb80c9238,0xfffffffc,0x00000000);	//fw_set[1:0]=0, read:during no read,write:during refresh
	}
	else  //SET4~7
	{
		//0.8VDD cal. for DQ/DQS	
		rtd_maskl(0xb80c9238,0xfffffffc,0x00000002);    //fw_set delay[1:0]=2 immediately update
		rtd_outl(0xb80c913c,0x00000001);    //zq_ena_nocd2[0]=1 enable NOCD2 for 0.8VDD
		rtd_maskl(0xb80c9000,0xffffefff,0x00000000);    //zq_ddr3_mode[12]=0 for 0.8VDD
		rtd_maskl(0xb80c91f4,0x0003ffff,0x86780000);    //Vref_range[27]=0:28%~92%,ZQ_Vref[26:21]=0x33->80.5%
		rtd_outl(0xb80c9130,0x04080c19);    //[27]=0:disable zq auto update,[26:25]=2'b10:zclk/32,[19]=1:R240 external,zq_cal_cmd[7]=0
		rtd_maskl(0xb80c9130,0xffff80ff,0x00007f00);
		rtd_outl(0xb80c923c,0x00000003);    //update write delay tap
		udelay(10);
		rtd_maskl(0xb80c9238,0xfffffffc,0x00000000);	//fw_set[1:0]=0, read:during no read,write:during refresh	
	}	

}

static void MC1_KGD_ZQ_K(void)
{
	//ZQCS
	// rtd_outl(0xb80c2110,0xd0000000);
	// rtd_outl(0xb80c2130,0x00a00008); //tZQCS>128nCK
	// rtd_outl(0xb80c2100,0x80000000);
	// udelay(1);
	//ZQCL
	rtd_outl(0xb80c2110,0xc0000400);
	rtd_outl(0xb80c2130,0x02500008); //tZQoper>512nCK
	rtd_outl(0xb80c2100,0x80000000);
	udelay(1);
	rtd_pr_zq_info("MC1 KGD ZQ Calibration done\n");
}


static void MC2_KGD_ZQ_K(void)
{
	//ZQCS
	// rtd_outl(0xb80c2110,0xd0000000);
	// rtd_outl(0xb80c2130,0x00a00008); //tZQCS>128nCK
	// rtd_outl(0xb80c2100,0x80000000);
	// udelay(1);
	//ZQCL
	rtd_outl(0xb80c3110,0xc0000400);
	rtd_outl(0xb80c3130,0x02500008); //tZQoper>512nCK
	rtd_outl(0xb80c3100,0x80000000);
	udelay(1);
	rtd_pr_zq_info("MC2 KGD ZQ Calibration done\n");
}


static void SOC_ZQ_K_UPDATE(void)
{    
	rtd_pr_zq_info("before zq update\n");
	rtd_pr_zq_info("[ZqCalInfo](180~18c):0x%08x, 0x%08x, 0x%08x, 0x%08x\n",rtd_inl(0xb80c9180),rtd_inl(0xb80c9184),rtd_inl(0xb80c9188),rtd_inl(0xb80c918c));
	rtd_pr_zq_info("[ZqCalInfo](190~19c):0x%08x, 0x%08x, 0x%08x, 0x%08x\n",rtd_inl(0xb80c9190),rtd_inl(0xb80c9194),rtd_inl(0xb80c9198),rtd_inl(0xb80c919c));	
	//Dual MC mode
	rtd_maskl(DPI1_PAD_CTRL_PROG, 0xFFFBFFFF, 0x00040000);	// [18]=1,Enable Update during Refresh
	udelay(1);
	start = local_clock();
	rtd_maskl(MC1_SYS_REF_1, 0xffefffff, 0x00100000); //bit20, auto clear
	rtd_maskl(MC2_SYS_REF_1, 0xffefffff, 0x00100000); //bit20, auto clear
	udelay(10);
	while ((rtd_inl(MC1_SYS_REF_1) & 0x00100000) && (rtd_inl(MC2_SYS_REF_1) & 0x00100000) )
	{
		now = local_clock();
		if (div_u64((now - start), 1000UL) > 64 * 1000ULL /*64ms*/)
		{
		  printk_once(KERN_EMERG "after soc update, MC1_SYS_REF_1 (0x%x) still hold REF_EN high in 64ms.\n", rtd_inl(MC1_SYS_REF_1));
		  printk_once(KERN_EMERG "after soc update, MC2_SYS_REF_1 (0x%x) still hold REF_EN high in 64ms.\n", rtd_inl(MC2_SYS_REF_1));
		  start = now;
		  //break;  // 64ms expiration
		  /* sleep 50ms, and polling infinitely */
		  //schedule_timeout_interruptible(HZ/20);
		}
	}
	rtd_maskl(DPI1_PAD_CTRL_PROG, 0xFFFBFFFF, 0x00000000);	// [18]=1,Disable Update during Refresh
	rtd_pr_zq_info("after zq update\n");
	rtd_pr_zq_info("[ZqCalInfo](1a0~1ac):0x%08x, 0x%08x, 0x%08x, 0x%08x\n",rtd_inl(0xb80c91a0),rtd_inl(0xb80c91a4),rtd_inl(0xb80c91a8),rtd_inl(0xb80c91ac));
	rtd_pr_zq_info("[ZqCalInfo](1b0~1bc):0x%08x, 0x%08x, 0x%08x, 0x%08x\n",rtd_inl(0xb80c91b0),rtd_inl(0xb80c91b4),rtd_inl(0xb80c91b8),rtd_inl(0xb80c91bc));	
}


static void Kernal_ZqCalibration(void)
{
	unsigned int total_SET = 7;
	unsigned int i;
	int retry_timeout;

	rtd_pr_zq_info("before zqk\n");
	rtd_pr_zq_info("[ZqCalInfo](1a0~1ac):0x%08x, 0x%08x, 0x%08x, 0x%08x\n",rtd_inl(0xb80c91a0),rtd_inl(0xb80c91a4),rtd_inl(0xb80c91a8),rtd_inl(0xb80c91ac));
	rtd_pr_zq_info("[ZqCalInfo](1b0~1bc):0x%08x, 0x%08x, 0x%08x, 0x%08x\n",rtd_inl(0xb80c91b0),rtd_inl(0xb80c91b4),rtd_inl(0xb80c91b8),rtd_inl(0xb80c91bc));	

    rtd_maskl(ZQFW_CTRL1, 0xffffefff, 0x1000); //[12]=1 zq fw start
               
	//**DCU1 ZQ Calibration for SET0~7**//
	rtd_maskl(DPI1_PAD_CTRL_PROG, 0xF7FFFFFF, 0x00000000);	//[27]=0,Disable ZQ update immediately
	rtd_maskl(DPI1_PAD_CTRL_PROG, 0xFFFBFFFF, 0x00000000);	//[18]=0,Disable ZQ update during refresh
	rtd_maskl(DPI1_PAD_BUS_1,0xffffffbf,0x00000000);	//zq pad pd[6]:0(default) -> power on
	udelay(10);
	rtd_maskl(DPI1_PAD_CTRL_PROG, 0x7FFFFFFF, 0x80000000); // [31]=1,ZQ Calibration filter enable
	
	if((rtd_inl(DPI1_PAD_ZCTRL_STATUS) & 0x08000000) == 0x0) //[27]=0,zq auto update disable
		rtd_pr_zq_info("zq auto update disable\n");
	
	for(i = 0; i < total_SET; i++)   //SET0~7
	{
			ZQ_VREF_SET(i);		//set ZQ VREF
			
			rtd_maskl(DPI1_PAD_CTRL_PROG, 0x8FFFFFFF, (i << 28)); // Enable auto update OCD / ODT set0~set7 [30:28]=i				
			rtd_maskl(DPI1_PAD_CTRL_PROG, 0xFEFFFFFF, 0x01000000); // [24]=1,Calibration Start					

			retry_timeout = (250 * 4); //(250*4) us
			while (retry_timeout > 0)
			{
					retry_timeout -= 1;
					udelay(10);
					// smp_mb();

					if((rtd_inl(DPI1_PAD_ZCTRL_STATUS) & 0x80000000) == 0x80000000) //done[31]
							break;
			}
			
			rtd_maskl(DPI1_PAD_CTRL_PROG, 0xFEFFFFFF, 0x00000000); // [24]=1,Calibration Stop					
	}
	rtd_maskl(DPI1_PAD_CTRL_PROG, 0x7FFFFFFF, 0x00000000); // [31]=1,ZQ Calibration filter disable

	//** update soc zq calibration data **//
	if((test_mode & 0x00000002) == 0x00000002)   //[1]=1, SOC ZQ update
		SOC_ZQ_K_UPDATE();

	//**KGD ZQCS/ZQCL Cal.**//
	if((test_mode & 0x00000004) == 0x00000004)   //[2]=1, KGD ZQ K
	{
		MC1_KGD_ZQ_K();
		MC2_KGD_ZQ_K();
	}		
	
	rtd_maskl(ZQFW_CTRL1, 0xffffdfff, 0x2000); //[13]=1 zq fw finish    

} //end of Main enable

#endif //CONFIG_RTK_ZQ_ENABLE


static int rtk_monitor_thread(void *arg)
{
	struct sched_param param = { .sched_priority = 1 };

	int reg_value;
	int monitor_value=0;

#ifdef CONFIG_RTK_ZQ_ENABLE
	int monitor_tmp=0;
	int monitor_diff, flag = 0;
	//int onetime_flag = 0;
#endif //CONFIG_RTK_ZQ_ENABLE

	set_freezable();
	sched_setscheduler_nocheck(current, SCHED_FIFO, &param);

	pre_jiffies = jiffies; // initial
	do
	{
		try_to_freeze();
		msleep(rtk_monitor_delay >> 1);
#if 0 //test
		diff = jiffies_to_msecs(jiffies - pre_jiffies);
		pre_jiffies = jiffies;

		rtd_pr_zq_info("diff=%d\n", diff);
#endif

		reg_value=rtk_get_thermal_reg();
		monitor_value = reg_value & 0x7FFFF;
        if((monitor_value & 0x40000) == 0x40000) { //if [18]=1,negative value
        	monitor_value = monitor_value - 1;
	    	monitor_value = (~monitor_value) & 0x0003FFFF;
        	monitor_value = -monitor_value;
   		}
		monitor_value = (1000*monitor_value)/1024;
/*adding the highest temperatue value*/
		if((monitor_value%1000) < 0)
		{
			rtd_pr_thermal_debug("[Rtk Monitor] =%3d.%03d Highest:%d reg:%x\n",monitor_value/1000, -(monitor_value%1000),(rtd_inl(STB_WDOG_DATA14_reg)&0x00FF0000)>>16,reg_value);
                       if(rtk_monitor_debug == 1) //message out with low level by debug flag
                               rtd_pr_thermal_crit("[Rtk Monitor] =%3d.%03d Highest:%d reg:%x\n",monitor_value/1000, -(monitor_value%1000),(rtd_inl(STB_WDOG_DATA14_reg)&0x00FF0000)>>16,reg_value);
		}
		else
		{
			rtd_pr_thermal_debug("[Rtk Monitor] =%3d.%03d Highest:%d reg:%x\n",monitor_value/1000, monitor_value%1000,(rtd_inl(STB_WDOG_DATA14_reg)&0x00FF0000)>>16,reg_value);
                       if(rtk_monitor_debug == 1) //message out with low level by debug flag
                               rtd_pr_thermal_crit("[Rtk Monitor] =%3d.%03d Highest:%d reg:%x\n",monitor_value/1000, (monitor_value%1000),(rtd_inl(STB_WDOG_DATA14_reg)&0x00FF0000)>>16,reg_value);
		}

		if(monitor_value/1000 >= RTK_MONITOR_THERMAL_TEMP_PRINT_THRESHOLD)
			rtd_pr_thermal_crit("[Rtk Monitor]:%d reg:%x\n",monitor_value/1000,reg_value);

		{
			int previous_value = (rtd_inl(STB_WDOG_DATA14_reg) & (0x00ff0000)) >> 16;
			int val = monitor_value/1000;
            if((val > 0) && (val < 255)) {
				//keep the highest monitor value
				if(val > previous_value)
					rtd_maskl(STB_WDOG_DATA14_reg,0xFF00FFFF, (val)<<16);
            } else { //invalid value
				rtd_maskl(STB_WDOG_DATA14_reg,0xFF00FFFF, (0xff)<<16);
			}
		}
		
#ifdef CONFIG_RTK_ZQ_ENABLE
		test_mode = rtd_inl(ZQFW_CTRL1); //[1]:ZQ per sec;[0]:Main Enable
		ZQ_Config = rtd_inl(ZQFW_CTRL2);
		
		if((test_mode & 0x1) == 0x1)   //[0]=1:Main Enable
		{
			//test_mode |= 0x00000008 ;; // [3]=1:forece test mode for debug purpose, once K per second
			if((test_mode & 0x00000008) == 0x00000000)   //[3]=0 update by monitor_diff > 10 or <-10
			{
				if(flag == 0)
					monitor_tmp = monitor_value;

				monitor_diff = (monitor_tmp - monitor_value)/1000;

				if((flag == 0) || (monitor_diff > RTK_MONITOR_THERMAL_TEMP_ZQ_THRESHOLD) || (monitor_diff < -RTK_MONITOR_THERMAL_TEMP_ZQ_THRESHOLD))
				{
					rtd_pr_zq_info("SOC_Tj = %d degree\n",monitor_value/1000);
					Kernal_ZqCalibration(); //DDR zq cal.
					monitor_tmp = monitor_value;
					flag = 1;
				}
			}
			else   //[3]=1, update zq by second
			{
				rtd_pr_zq_info("SOC_Tj = %d degree\n",monitor_value/1000);
				Kernal_ZqCalibration();
			}
		}
	
#endif //CONFIG_RTK_ZQ_ENABLE

		try_to_freeze();
		msleep(rtk_monitor_delay >> 1);

	} while (!kthread_should_stop());

	return 0;
}

static struct task_struct * rtk_monitor_thread_create(void)
{
	struct task_struct *task = NULL;
	int err;

	task = kthread_create(rtk_monitor_thread, NULL, "rtk_monitor_thread");
	if (IS_ERR(task)) {
		err = PTR_ERR(task);
		rtd_pr_zq_err("%s failed (%d)\n", __func__, err);
		task = NULL;
		return task;
	}

	wake_up_process(task);
	return task;
}

static int rtk_monitor_enable(void)
{
	int ret = 0;

	mutex_lock(&rtk_monitor_activation_lock);

	if (rtk_monitor_active) {
		mutex_unlock(&rtk_monitor_activation_lock);
		return 0;
	}

	rtk_monitor_task = rtk_monitor_thread_create();

	if (rtk_monitor_task == NULL) {
		ret = -1;
		goto out;
	}

	rtk_monitor_active = 1;

out:
	mutex_unlock(&rtk_monitor_activation_lock);

	if (rtk_monitor_task != NULL)
		rtd_pr_zq_info("rtk monitor thread created\n");
	else
		rtd_pr_zq_err("rtk monitor thread NOT created\n");

	return ret;
}

void rtk_monitor_disable(void)
{
	mutex_lock(&rtk_monitor_activation_lock);

	if (!rtk_monitor_active)
		goto out;

	kthread_stop(rtk_monitor_task);

	rtk_monitor_active = 0;

out:
	mutex_unlock(&rtk_monitor_activation_lock);
}

ssize_t rtk_monitor_active_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", rtk_monitor_active);
}

ssize_t rtk_monitor_active_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int ret;

	switch (buf[0]) {
	case '0':
		rtk_monitor_disable();
		ret = 0;
		break;
	case '1':
		ret = rtk_monitor_enable();
		break;
	default:
		ret = -EINVAL;
	}

	return (ret >= 0) ? count : ret;
}

ssize_t rtk_monitor_delay_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", rtk_monitor_delay);
}

ssize_t rtk_monitor_delay_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int ret;

	mutex_lock(&rtk_monitor_delay_lock);
	ret = sscanf(buf, "%u", &rtk_monitor_delay);
	if (ret != 1) {
		mutex_unlock(&rtk_monitor_delay_lock);
		return -EINVAL;
	}

//out:
	mutex_unlock(&rtk_monitor_delay_lock);
	return count;
}

ssize_t rtk_monitor_debug_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
       return sprintf(buf, "%u\n", rtk_monitor_debug);
}

ssize_t rtk_monitor_debug_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
       int ret;

       mutex_lock(&rtk_monitor_delay_lock);
       ret = sscanf(buf, "%u", &rtk_monitor_debug);
       if (ret != 1) {
               mutex_unlock(&rtk_monitor_delay_lock);
               return -EINVAL;
       }

//out:
       mutex_unlock(&rtk_monitor_delay_lock);
      return count;
}

EXPORT_SYMBOL(rtk_monitor_active_show);
EXPORT_SYMBOL(rtk_monitor_active_store);
EXPORT_SYMBOL(rtk_monitor_delay_show);
EXPORT_SYMBOL(rtk_monitor_delay_store);
EXPORT_SYMBOL(rtk_monitor_debug_show);
EXPORT_SYMBOL(rtk_monitor_debug_store);

#if 0 // move to sysfs.c
static struct kobj_attribute rtk_monitor_active_attr =
	__ATTR(active, 0644, rtk_monitor_active_show, rtk_monitor_active_store);

static struct kobj_attribute rtk_monitor_delay_attr =
	__ATTR(delay, 0644, rtk_monitor_delay_show, rtk_monitor_delay_store);

//static struct attribute *rtk_monitor_attrs[] = {
//	&rtk_monitor_active_attr.attr,
//	&rtk_monitor_delay_attr.attr,
//	NULL,
//};

//static struct attribute_group rtk_monitor_attr_group = {
//	.attrs = rtk_monitor_attrs,
//};

static struct kobject *rtk_monitor_kobj;
extern struct kobject *realtek_boards_kobj;

static int __init rtk_monitor_sysfs_init(void)
{
	int ret;

//	rtk_monitor_kobj = kobject_create_and_add("rtk_monitor", kernel_kobj);
//	if (!rtk_monitor_kobj)
//		return -ENOMEM;
//	ret = sysfs_create_group(rtk_monitor_kobj, &rtk_monitor_attr_group);
//	if (ret)
//		kobject_put(rtk_monitor_kobj);

	if (!realtek_boards_kobj)
		return -ENOMEM;

	rtk_monitor_kobj = kobject_create();
	ret = kobject_set_name(rtk_monitor_kobj, "rtk_monitor");
	if (ret)
		goto out;
	ret = kobject_add(rtk_monitor_kobj, realtek_boards_kobj, "rtk_monitor");
	if (ret)
		goto out;

	ret = sysfs_create_file(rtk_monitor_kobj, &rtk_monitor_active_attr.attr);
	ret |= sysfs_create_file(rtk_monitor_kobj, &rtk_monitor_delay_attr.attr);
	if (ret)
		kobject_put(rtk_monitor_kobj);

out:
	return ret;
}
#endif

int rtk_monitor_init(void)
{
	int ret;

	init_rtk_thermal_sensor();

	ret = rtk_monitor_enable();
	if (ret)
		return ret;

#if 0 // move to sysfs.c
	ret = rtk_monitor_sysfs_init();
	if (ret)
		rtd_pr_zq_err("%s: unable to create sysfs entry\n", __func__);
#endif

	return 0;
}


#if 0 // move to rtk_tmu_probe
static int rtk_monitor_probe(struct platform_device *pdev)
{
	rtk_monitor_init();
	return 0;
}

static int rtk_monitor_remove(struct platform_device *pdev)
{
	return 0;
}

static int rtk_monitor_suspend(struct device *dev)
{

	return 0;
}

static int rtk_monitor_resume(struct device *dev)
{
	ZQ_init_flag = 0;
	
	return 0;
}

static const struct dev_pm_ops rtk_monitor_ops = {
	.suspend    = rtk_monitor_suspend,
	.resume     = rtk_monitor_resume,
#ifdef CONFIG_HIBERNATION
	.freeze     = rtk_monitor_suspend,
	.thaw	    = rtk_monitor_resume,
	.poweroff   = rtk_monitor_suspend,
	.restore    = rtk_monitor_resume,
#endif
};

static const struct of_device_id rtk_monitor_match[] = {
	{
	 .compatible = "rtk_monitor",
	 .data = (void *)&rtk_monitor_ops,
	 },
	{},
};

static struct platform_device_id rtk_monitor_ids[] = {
#if defined(CONFIG_ARCH_RTK289X)
	{
		 .name = "rtk-monitor",
		 .driver_data = (kernel_ulong_t) NULL,
	 },
#endif 
	{},
};

MODULE_DEVICE_TABLE(of, rtk_monitor_match);

static struct platform_driver rtk_monitor_driver = {
	.driver = {
		   .name = "rtk-monitor",
		   .owner = THIS_MODULE,
#ifdef CONFIG_PM
		   .pm = &rtk_monitor_ops,
#endif
		   .of_match_table = of_match_ptr(rtk_monitor_match),
		   },
	.probe = rtk_monitor_probe,
	.remove = rtk_monitor_remove,
	.id_table = rtk_monitor_ids,
};

module_platform_driver(rtk_monitor_driver);

#else
//late_initcall(rtk_monitor_init); //move to rtk_tmu_probe
int rtk_monitor_resume(void)
{
        ZQ_init_flag = 0;
        return 0;
}
#endif
