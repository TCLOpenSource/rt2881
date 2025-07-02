/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2010 by Chien-An Lin <colin@realtek.com.tw>
 *
 * sysfs attributes in /sys/realtek_boards
 */

#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#include <linux/wait.h>
#include <linux/freezer.h>
#include <linux/suspend.h>
#endif

#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ctype.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/pid.h>
#include <linux/slab.h>
#include <linux/firmware.h>
#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>
#include <mach/platform.h>
#include <mach/pcbMgr.h>
#include <linux/kthread.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include <linux/types.h>
#include <linux/pageremap.h>
#ifdef CONFIG_REALTEK_VDEC
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#endif
#ifdef CONFIG_SUPPORT_GDMA
#include <rtk_gdma_export.h>
#endif
#include <base_types.h>
#include <linux/cpumask.h>
#include <asm/smp_plat.h>
#include <asm/cputype.h>
#include <asm/cpu.h>
#include <asm/unistd.h>
#include <asm/fcntl.h>

#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <rtk_kdriver/rtk_thermal_sensor.h>
#if IS_ENABLED(CONFIG_RTK_KDRV_WATCHDOG)
#include <rtk_kdriver/rtk_watchdog.h>
#endif
#ifdef CONFIG_TRUST_ZONE_FOR_NORMAL_WORLD
#include "../../kernel/TZMoniter/tz_syscalls_cmd_def.h"
#endif
#ifdef CONFIG_REALTEK_UART2RBUS_CONTROL
#include <rbus/iso_misc_reg.h>
#endif
#include <rtd_log/rtd_module_log.h>

#if defined(CONFIG_RTK_KDRV_GDMA)
#include <rbus/sys_reg_reg.h>
#include <rbus/gdma_reg.h>
extern unsigned int gdma_dev_addr ;//kernel winfo addr
#endif
//#include <generated/compile.h>
#include <rtk_kdriver/rtd_logger.h>
#include <mach/compile.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#ifdef CONFIG_RTK_LOW_POWER_MODE
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/delay.h>
#define CONFIG_LPM_SUPPORT_ENABLE

//#ifdef CONFIG_ARCH_RTK2885P
//#define CONFIG_LPM_BRINGUP_VERIFY
//#endif

#endif
#endif //LINUX_VERSION_CODE

#define SYNC_STRUCT_ADDR 0x000000cc
//#define UNLZMA_SYNC_ADDR 0x00000070

#define CONFIG_BOOTUP_ANIMATION

/* MPIDR */
#define MPIDR_Cluster0_Core0	0x80000000
#define MPIDR_Cluster0_Core1	0x80000001
#define MPIDR_Cluster1_Core0	0x80000100
#define MPIDR_Cluster1_Core1	0x80000101

#ifdef CONFIG_RTK_KDRV_SYSFS
extern char *saved_command_line;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#ifdef  CONFIG_LPM_SUPPORT_ENABLE
static DEFINE_MUTEX(rtk_lpm_lock);
#endif //CONFIG_LPM_SUPPORT_ENABLE
#endif //LINUX_VERSION_CODE

#ifdef CONFIG_BOOTUP_ANIMATION
//extern char corepath[100];
extern void *pAnimation;
extern void *pAnimation_1;
extern void *pLastImage;
#endif
extern struct dma_buf *pQShowVBM;
void setup_boot_image(void);
void setup_boot_image_mars(void);

#ifdef CONFIG_REALTEK_SHOW_STACK
void show_trace(struct task_struct *task, unsigned long *stack);
#endif

#define REALTEK_BOARDS_ATTR_RO(_name) \
static struct kobj_attribute _name##_attr = __ATTR_RO(_name)

#define REALTEK_BOARDS_ATTR_RW(_name) \
static struct kobj_attribute _name##_attr = \
	__ATTR(_name, 0644, _name##_show, _name##_store)

extern void sched_show_task(struct task_struct *p); 

static ssize_t kernel_source_code_info_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%s\n", platform_info.kernel_source_code_info);
}
REALTEK_BOARDS_ATTR_RO(kernel_source_code_info);

static ssize_t bootloader_version_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%s\n", platform_info.bootloader_version);
}
REALTEK_BOARDS_ATTR_RO(bootloader_version);

static ssize_t board_id_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%x\n", platform_info.board_id);
}
REALTEK_BOARDS_ATTR_RO(board_id);

static ssize_t cpu_id_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%x\n", platform_info.cpu_id);
}
REALTEK_BOARDS_ATTR_RO(cpu_id);

static ssize_t tv_encoding_system_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	unsigned char *name = NULL;
	switch(platform_info.tv_encoding_system) {
	case PAL:
		name = "PAL";
		break;
	case NTSC:
	default:
		name = "NTSC";
		break;
	}
	return snprintf(page, PAGE_SIZE-1, "%x (%s)\n", platform_info.tv_encoding_system, name);
}
REALTEK_BOARDS_ATTR_RO(tv_encoding_system);

#ifdef CONFIG_REALTEK_SHOW_STACK
static void show_task_stack(int pid)
{
	struct task_struct	*task;
	struct pt_regs		*regs;

	rcu_read_lock();
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	if (task == NULL) {
		rtd_pr_mach_rtk_err("Task %d doesn't exist...\n", pid);
		return;
	}
	regs = (struct pt_regs *)(((unsigned char *)task->stack)+THREAD_SIZE-32-sizeof(struct pt_regs));

	rtd_pr_mach_rtk_err("task name: %s, priority: %d state: %lx flags: %lx (epc == %lx, ra == %lx) \n",
			task->comm, task->prio, task->state, task_thread_info(task)->flags,
			(unsigned long)regs->cp0_epc, (unsigned long)regs->regs[31]);
	show_stack(task, NULL);
}

static ssize_t task_stack_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return sprintf(page, "Please add thread id as parameter...\n");
}

static ssize_t task_stack_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	char *p, buffer[20] = {0};
	int i, len, pid;

	p = memchr(page, '\n', count);
	len = p ? p - page : count;
	strncpy(buffer, page, len);
	for (i = 0; i < len; i++)
		if (!isdigit(buffer[i])) {
			rtd_pr_mach_rtk_err("%c is not digit...\n", buffer[i]);
			return count;
		}
	sscanf(buffer, "%d", &pid);
	rtd_pr_mach_rtk_err("pid value: %d \n", pid);
	show_task_stack(pid);

	return count;
}
REALTEK_BOARDS_ATTR_RW(task_stack);
#endif

#ifdef CONFIG_REALTEK_SCHED_LOG
extern void auth_sched_log_init(int size);
extern void auth_sched_log_start(void);
extern void auth_sched_log_free(char *filename);
extern void log_event(int cpu, int event);

static ssize_t sched_log_time_duration_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%d\n", platform_info.sched_log_time_duration);
}


static struct task_struct *rtk_sched_log_task;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
int diff_time_val_ms(struct timespec64 *start, struct timespec64 *end)
{
	return (end->tv_sec - start->tv_sec)*1000 + ((end->tv_nsec - start->tv_nsec)/1000000);
}
#else
int diff_time_val_ms(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec - start->tv_sec)*1000 + ((end->tv_usec - start->tv_usec)/1000);
}
#endif

static int kthread_logging_file(void *arg)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	struct timespec64 t0_start, t0_end = {.tv_sec = 0, .tv_nsec = 0};
#else
	struct timeval t0_start, t0_end = {.tv_sec = 0, .tv_usec = 0};
#endif
	//int *d = (int *) arg;

	unsigned int target_duration=platform_info.sched_log_time_duration;
	if(target_duration>=300)
		target_duration=300;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	ktime_get_real_ts64(&t0_start);
#else
	do_gettimeofday(&t0_start);
#endif
    for(;;) {
        if (kthread_should_stop())
			break;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
		ktime_get_real_ts64(&t0_end);
#else
		do_gettimeofday(&t0_end);
#endif

		if(platform_info.sched_log_file_stop) //user issue stop command
			break;

		if(diff_time_val_ms(&t0_start , &t0_end)> (target_duration*1000))
			break;

		msleep(10);
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	rtd_pr_mach_rtk_err("logging time info:%ld %ld end:%ld %ld diff:%d\n", t0_start.tv_sec, t0_start.tv_nsec,
		t0_end.tv_sec, t0_end.tv_nsec, diff_time_val_ms(&t0_start, &t0_end));
#else
	rtd_pr_mach_rtk_err("logging time info:%ld %ld end:%ld %ld diff:%d\n", t0_start.tv_sec, t0_start.tv_usec,
		t0_end.tv_sec, t0_end.tv_usec, diff_time_val_ms(&t0_start, &t0_end));
#endif

	if(strlen(platform_info.sched_log_name)) {
		auth_sched_log_free(platform_info.sched_log_name);
		rtd_pr_mach_rtk_err("end to logging files:%s\n",platform_info.sched_log_name);
	}
	else
	{
		auth_sched_log_free("/tmp/usb/sda/sda1/Performance.vcd");
		rtd_pr_mach_rtk_err("end to logging files:/tmp/usb/sda/sda1/Performance.vcd\n");
	}
	platform_info.sched_log_file_start=0;
    return 0;
}

static ssize_t sched_log_time_duration_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	int ret, err;
	if(platform_info.sched_log_file_start)
		return 0;

	if(page) {
		err = kstrtoint(page, 10, &platform_info.sched_log_time_duration);
		if(err)
			return err;
	}
	else
		platform_info.sched_log_time_duration=10;
	if(platform_info.sched_log_time_duration<=0)
		platform_info.sched_log_time_duration=10;
	if(platform_info.sched_log_size>0)
		auth_sched_log_init(platform_info.sched_log_size);
	else
		auth_sched_log_init(6*1024*1024);
	auth_sched_log_start();
	platform_info.sched_log_file_start=1;

	rtk_sched_log_task = kthread_create(kthread_logging_file, NULL, "sched_log");
	if (IS_ERR(rtk_sched_log_task)) {
        	ret = PTR_ERR(rtk_sched_log_task);
        	rtk_sched_log_task = NULL;
	}
	else
	    wake_up_process(rtk_sched_log_task);

	return count;
}

REALTEK_BOARDS_ATTR_RW(sched_log_time_duration); //default 10s, use timer to stop log, set it will trigger log start and stop
static ssize_t sched_log_file_start_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1,"%d\n", platform_info.sched_log_file_start);
}

static ssize_t sched_log_file_start_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	int err;
	if(page) {
		err = kstrtoint(page, 10, &platform_info.sched_log_file_start);
		if(err)
			return err;
	}
	else
		platform_info.sched_log_file_start=0;
	return count;
}

REALTEK_BOARDS_ATTR_RW(sched_log_file_start); // cat it will start to log file
static ssize_t sched_log_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	if(platform_info.sched_log_size<=0)
		platform_info.sched_log_size=6*1024*1024;
	return snprintf(page, PAGE_SIZE-1,"%d\n", platform_info.sched_log_size);
}

static ssize_t sched_log_size_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	int err;
	if(page) {
		err = kstrtoint(page, 10, &platform_info.sched_log_size);
		if(err)
			return err;
	}
	else
		platform_info.sched_log_size=6*1024*1024;
	if(platform_info.sched_log_size>(12*1024*1024))
		platform_info.sched_log_size=12*1024*1024;
	return count;
}

REALTEK_BOARDS_ATTR_RW(sched_log_size); //default 6M



static ssize_t sched_log_event_number_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	if(platform_info.sched_log_event_number<=0)
		platform_info.sched_log_event_number=100;
	return snprintf(page, PAGE_SIZE-1,"%d\n", platform_info.sched_log_event_number);
}

static ssize_t sched_log_event_number_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	int err, number_int=0;
	if(page) {
		err = kstrtoint(page, 10, &number_int);
		if(err)
			return err;
		platform_info.sched_log_event_number = (short)number_int;
	}
	else
		platform_info.sched_log_event_number=100;
	if(platform_info.sched_log_event_number> 65536)
		platform_info.sched_log_event_number=100;
	log_event(smp_processor_id(),platform_info.sched_log_event_number);
	log_event(smp_processor_id(),platform_info.sched_log_event_number+1);
	return count;
}

REALTEK_BOARDS_ATTR_RW(sched_log_event_number);

static ssize_t sched_log_file_stop_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%d\n", platform_info.sched_log_file_stop);
}

static ssize_t sched_log_file_stop_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	int err;
	if(page) {
		err = kstrtoint(page, 10, &platform_info.sched_log_file_stop);
		if(err)
			return err;
	}
	else
		platform_info.sched_log_file_stop=0;
	kthread_stop(rtk_sched_log_task);
	platform_info.sched_log_file_start=0;
	return count;
}

REALTEK_BOARDS_ATTR_RW(sched_log_file_stop);

static ssize_t sched_log_name_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%s\n", platform_info.sched_log_name);
}

static ssize_t sched_log_name_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	int name_max_len = 120;

	memset(platform_info.sched_log_name, 0, name_max_len);
	if(page) {
		strncpy(platform_info.sched_log_name, page, min((int)(strlen(page)-1), name_max_len-1));
		platform_info.sched_log_name[name_max_len-1] = 0;
	}
	else
		strncpy(platform_info.sched_log_name,"/tmp/usb/sda/sda1/Performance.vcd", 119);
	return count;
}

REALTEK_BOARDS_ATTR_RW(sched_log_name); //default /tmp/usb/sda/sda1
#endif


//echo "set2,18061500," > /sys/realtek_boards/arm_wrapper
#if IS_ENABLED(CONFIG_REALTEK_ARM_WRAPPER)
__attribute__((weak)) void arm_wrapper_get (void) {}
__attribute__((weak)) void arm_wrapper_dump (void) {}
__attribute__((weak)) void arm_wrapper_set (const char *buffer) {}

__attribute__((weak)) void arm_wrapper_suspend (void) {}
__attribute__((weak)) void arm_wrapper_resume (void) {}
__attribute__((weak)) int arm_wrapper_get_info (char *buf)
{
        return 0;
}

void arm_wrapper_get (void);
int arm_wrapper_get_info (char *buf);
void arm_wrapper_dump (void);
void arm_wrapper_set (const char *buffer);
static ssize_t arm_wrapper_show (struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
    return arm_wrapper_get_info(page);
}

//static volatile int arm_wrapper[0x10] __attribute__((aligned(256))) ;
static volatile int* arm_wrapper=NULL;
#define ARM_WRAPPER_TEST_SIZE 1024*1024
static ssize_t arm_wrapper_store (struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
    char buffer[128];
    int arm_wrapper_ = 0;
    static unsigned int addr_s,addr_e;
    unsigned char old_loglevel=console_loglevel;

    console_loglevel=7;
    
    if (strncmp(page,"dbg_",strlen("dbg_"))==0)
    {
        if (!arm_wrapper)
        {
            arm_wrapper = kmalloc(ARM_WRAPPER_TEST_SIZE,GFP_KERNEL);
            if (!arm_wrapper)
            {
                rtd_pr_mach_rtk_err("%s %d  memory allocate fail\n",__func__,__LINE__);
                return count;
            }

            addr_s=virt_to_phys(arm_wrapper);
            addr_e =virt_to_phys(arm_wrapper)+ARM_WRAPPER_TEST_SIZE-1;
        }
    
        memset(buffer, 0, sizeof(buffer));
        if (strncmp(page,"dbg_show",strlen("dbg_show"))==0)
        {
            rtd_pr_mach_rtk_err("dbg start addr=%x, dbg end addr=%x\n",addr_s,addr_e);

            rtd_pr_mach_rtk_err("You can set ARM wrapper cmd like below:\n");

            snprintf(buffer,sizeof(buffer),"echo \"set%d %x-%x %s\">/sys/realtek_boards/arm_wrapper",2,addr_s,addr_e,"r");
            rtd_pr_mach_rtk_err("%s\n",buffer);
            rtd_pr_mach_rtk_err("echo dbg_read >/sys/realtek_boards/arm_wrapper\n\n");

            snprintf(buffer,sizeof(buffer),"echo \"set%d %x-%x %s\">/sys/realtek_boards/arm_wrapper",2,addr_s,addr_e,"w");
            rtd_pr_mach_rtk_err("%s\n",buffer);
            rtd_pr_mach_rtk_err("echo dbg_write >/sys/realtek_boards/arm_wrapper\n\n");

            rtd_pr_mach_rtk_err("echo dbg_flush_cache >/sys/realtek_boards/arm_wrapper   (flush cache by copy lots of data)\n\n");
        }
        else if (strncmp(page,"dbg_read",strlen("dbg_read"))==0)
        {
            rtd_pr_mach_rtk_err("\n\n\nSCPU triggerr arm wrapper interrupt: invalid read access debug address          \e[1;31m0x%08x  \e[0m\n\n\n",&(arm_wrapper[2]));
            arm_wrapper_=arm_wrapper[2];

            rtd_pr_mach_rtk_err("\n\n\n%s %d. SCPU dbg read finish\n",__func__,__LINE__);
        }
        else if (strncmp(page,"dbg_write",strlen("dbg_write"))==0)
        {
            rtd_pr_mach_rtk_err("\n\n\nSCPU triggerr arm wrapper interrupt: invalid write access debug address          \e[1;31m0x%08x\e[0m\n\n\n",&(arm_wrapper[2]));
            arm_wrapper[2]=38;

            rtd_pr_mach_rtk_err("\n\n\n%s %d. SCPU dbg write finish\n",__func__,__LINE__);
        }
        else if (strncmp(page,"dbg_dump",strlen("dbg_dump"))==0)
        {
            arm_wrapper_dump();
        }
        else if (strncmp(page,"dbg_flush_cache",strlen("dbg_flush_cache"))==0)
        {
            int i;
            rtd_pr_mach_rtk_err("%s %d  drop cache \n",__func__,__LINE__);
            for (i=0;i<ARM_WRAPPER_TEST_SIZE;i++)
            {
                arm_wrapper[i]=i*i;
            }
        }
    }
    else
    {
        arm_wrapper_set(page);
    }
    console_loglevel=old_loglevel;
    return count;
}
REALTEK_BOARDS_ATTR_RW(arm_wrapper);
#endif

#ifdef CONFIG_RTK_KDRV_QOS_CONTROL
extern ssize_t rtk_qos_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
extern ssize_t rtk_qos_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);

REALTEK_BOARDS_ATTR_RW(rtk_qos);
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_SB2)
static ssize_t sb2_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
    int value=1;

	#ifdef CONFIG_TRUST_ZONE_FOR_NORMAL_WORLD
	//TZ_smc_with_cmd(NT_SMC_SWITCH, NT_SMC_SEND_SGI_INT, 0);
	//gic_show_info();
	#endif

	return snprintf(page, PAGE_SIZE-1,"sb2_show: %d\n", value);
}


//echo 1 > /sys/realtek_boards/sb2
extern char _etext[],_edata[];
static ssize_t sb2_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
    unsigned int value;
    char str[2];
    strncpy(str,page,1);
    str[1]=0;
    //rtd_pr_mach_rtk_err("@sb2_store       page=%s   str=%s  count=%d\n",page,str,count);

    //#define VENUS_MIS_TCWTR            0x1BB04     --> 0x1801 bb04
    //outl(0x01, VENUS_MIS_TCWTR);
    //inl(SATURN_DC_EC_ADDCMD_HI);

    //reserved rbus address at 1800_xxxx ~ 1806_xxxx
    if (strncmp(str,"1",1)==0)
    {
        //value=rtd_inl(0xb8063000);
        //value=inl(0x63000);//0x1806_3000 - 0x1806_FFFF  --> Reserved

        //test for disbale vdc
        value=rtd_inl(0xb8000114);
        rtd_outl(0xb8000114,value&0xFFBFFFFF);

        value=0Xb8019000;
        rtd_pr_mach_rtk_err("trigger sb2 interrupt: access reserverd  rbus address          \e[1;31m0x%x\e[0m\n\n\n",value);
        value=rtd_inl(value);
    }
    else if (strncmp(str,"2",1)==0)
    {
        //value=rtd_inl(0xb8063000);
        //value=inl(0x63000);//0x1806_3000 - 0x1806_FFFF  --> Reserved

        //test for disbale vdc
        //value=rtd_inl(0xb8000114);
        //rtd_outl(0xb8000114,value&0xFFBFFFFF);

        value=0Xb8019000;
        rtd_pr_mach_rtk_err("trigger sb2 interrupt: access reserverd  rbus address          \e[1;31m0x%x\e[0m\n\n\n",value);
        value=rtd_inl(value);
    }


    //flash space:0x1dc0_0000 - 0x1fff_ffff, rbus 0x1800_0000 -0x1806_ffff , dbus 0x0000_0000 - 0x1000_0000
    #if 0
    else if (strncmp(page,"2",1)==0)
    {
        rtd_pr_mach_rtk_err("trigger sb2 interrupt: access \e[1;31minvalid address\e[0m      current mac scpu donot have this type interrupt\n\n\n");
        value= *(unsigned long *)0x9db00000;
    }
    else if (strncmp(page,"3",1)==0)
    {
        value=(unsigned int )_etext-0x10;
        rtd_pr_mach_rtk_err("trigger sb2 interrupt: access \e[1;31mdebug specified address[%x,%x]\e[0m\n\n\n",value,_etext);
        //value=*(unsigned long *)(value);
        *(unsigned long *)(value)=0;
    }
    #endif
    else
    {
        rtd_pr_mach_rtk_err("trigger sb2 interrupt:  no interrupt\n");
    }
    rtd_pr_mach_rtk_err("trigger sb2 interrupt:        \e[1;31mfinished!\e[0m\n\n");
    return count;
}
REALTEK_BOARDS_ATTR_RW(sb2);
#endif


#if IS_ENABLED(CONFIG_RTK_KDRV_SB2)
__attribute__((weak)) int sb2_dbg_get_info (char *buf) {return 0;}
__attribute__((weak)) void sb2_dbg_set (const char *buffer) {return ;}
__attribute__((weak)) int sb2_dbg_monitor (unsigned int start,unsigned int end,int cpu_id,int set_id){return 0;}

int sb2_dbg_get_info (char * buf);
void sb2_dbg_set (const char *buffer);
int sb2_dbg_monitor (unsigned int start,unsigned int end,int cpu_id,int set_id);
static ssize_t sb2_dbg_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{

        return sb2_dbg_get_info(page);

}

//echo dbg > /sys/realtek_boards/sb2_dbg
static ssize_t sb2_dbg_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
    unsigned int value;
    if (strncmp(page,"dbg",3)==0)
    {
        value=0xb8061500;//hdmi
        rtd_pr_mach_rtk_err("trigger sb2 dbg interrupt: access debug rbus address          \e[1;31m0x%x\e[0m\n\n\n",value);
        sb2_dbg_monitor(0x18061500,(0x18061500+0x100),3,8);
        value=rtd_inl(value);
    }
    else if (strncmp(page,"1",1)==0)
    {
        value=0xb8061500;//hdmi
        rtd_pr_mach_rtk_err("trigger sb2 dbg interrupt: access debug rbus address          \e[1;31m0x%x\e[0m\n\n\n",value);
        sb2_dbg_monitor(0x18061500,(0x18061500+0x100),4,8);
    }
    else
    {
        sb2_dbg_set(page);
    }
    return count;
}

REALTEK_BOARDS_ATTR_RW(sb2_dbg);
#endif

#if IS_ENABLED(CONFIG_REALTEK_LOGBUF)
__attribute__((weak)) ssize_t rtdlog_get (char *buffer)
{
    return 0;
}

__attribute__((weak)) void rtdlog_set (const char *buffer)
{
    return;
}

static ssize_t rtdlog_show (struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
    return rtdlog_get(page);
}

static ssize_t rtdlog_store (struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
    rtdlog_set(page);
    return count;
}

REALTEK_BOARDS_ATTR_RW(rtdlog);
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_WATCHDOG)
//echo 1 > /sys/realtek_boards/watchdog
//cat /sys/realtek_boards/watchdog

#if defined(CONFIG_RTK_KDRV_RTICE)
extern volatile int watchdog_rtice;
#endif

static ssize_t watchdog_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
#if defined(CONFIG_RTK_KDRV_RTICE)
    return snprintf(page, PAGE_SIZE-1, "/sys/realtek_boards/watchdog: %d  %d.  NMI:%x WOV%x\n",
        rtk_watchdog_get_flag(),watchdog_rtice,rtd_inl(0xB806220C),rtd_inl(0xB8062210));
#else
return snprintf(page, PAGE_SIZE-1, "/sys/realtek_boards/watchdog: %d\n", rtk_watchdog_get_flag());
#endif
}

static ssize_t watchdog_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
    char str[2];
    strncpy(str,page,1);
    str[1]=0;

    if (strncmp(str,"1",1)==0)
    {
        rtd_pr_mach_rtk_err("watchdog test!\n\n\n");
        local_irq_disable();
        while (1) ;
    }
    else
    {
        rtd_pr_mach_rtk_err("@watchdog_store\n");
    }
    return count;
}
REALTEK_BOARDS_ATTR_RW(watchdog);
#endif


#ifdef CONFIG_BOOTUP_ANIMATION
extern void do_dvr_reclaim(int cmd);

#if 1
static ssize_t reclaim_dvr_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1,"Animation: %p\nLastImage: %p\nQShowVBM: %p\n", pAnimation, pLastImage, pQShowVBM);
}

static ssize_t reclaim_dvr_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	char *p, buffer[20] = {0};
	int len, cmd, err;

	p = memchr(page, '\n', count);
	len = p ? p - page : count;
	strncpy(buffer, page, len);
	err = kstrtoint(buffer, 10, &cmd);
	if(err)
		return err;

	do_dvr_reclaim(cmd);

	return count;
}
REALTEK_BOARDS_ATTR_RW(reclaim_dvr);
#else
static ssize_t reclaim_dvr_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "Animation: %p\n\n", pAnimation);
}
REALTEK_BOARDS_ATTR_RO(reclaim_dvr);
#endif
#endif

#ifdef CONFIG_REALTEK_UART2RBUS_CONTROL
void enable_uart2rbus(unsigned int value)
{
#define DDR_DUMMY_REG 0xB8008EB4
#define DDC_CTRL_DDC_U2R_OFF 0
#define DDC_CTRL_DDC_U2R_USE_DDC0 1
#define DDC_CTRL_DDC_U2R_USE_DDC1 2
#define DDC_CTRL_DDC_U2R_USE_DDC2 3
#define DDC_U2R_DISABLE DDC_CTRL_DDC_U2R_OFF
#define DDC_U2R_ENABLE DDC_CTRL_DDC_U2R_USE_DDC1
	if ((rtd_inl(DDR_DUMMY_REG) & 0x01) == 0) //0xB8008EB4[0] = 0:switch U2R, 1: don't switch U2R
	{
		if (value)
		{
			;//rtd_maskl(ISO_MISC_DDC_CTRL_reg, ~ISO_MISC_DDC_CTRL_ddc_u2r_en_mask, ISO_MISC_DDC_CTRL_ddc_u2r_en(DDC_U2R_ENABLE));
		}
		else
		{
#if !defined(CONFIG_ARCH_RTK2875Q) && !defined(CONFIG_ARCH_RTK2819A)
			rtd_maskl(ISO_MISC_DDC_CTRL_reg, ~ISO_MISC_DDC_CTRL_ddc_u2r_en_mask, ISO_MISC_DDC_CTRL_ddc_u2r_en(DDC_U2R_DISABLE));
#else
			rtd_maskl(ISO_MISC_DDC_CTRL_reg, ~ISO_MISC_DDC_CTRL_ddc_u2r_en_st_mask, ISO_MISC_DDC_CTRL_ddc_u2r_en_st(DDC_U2R_DISABLE));
#endif
        }
	}
}

void do_rtk_uart2rbus_control(int cmd)
{
	if (cmd == 0)
	{
		rtd_pr_mach_rtk_err("disable uart2rbus\n");
		enable_uart2rbus(0);
	}
	else if (cmd == 1)
	{
		rtd_pr_mach_rtk_err("enalbe uart2rbus\n");
		enable_uart2rbus(1);
	}
	else
	{
		rtd_pr_mach_rtk_err("%s, wrong command\n", __FUNCTION__);
	}
}
static ssize_t rtk_uart2rbus_control_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
#if !defined(CONFIG_ARCH_RTK2875Q) && !defined(CONFIG_ARCH_RTK2819A)
	return snprintf(page, PAGE_SIZE-1, "%d \n", (rtd_inl(ISO_MISC_DDC_CTRL_reg) & ISO_MISC_DDC_CTRL_ddc_u2r_en_mask) >> ISO_MISC_DDC_CTRL_ddc_u2r_en_shift);
#else
	return snprintf(page, PAGE_SIZE-1, "%d \n", (rtd_inl(ISO_MISC_DDC_CTRL_reg) & ISO_MISC_DDC_CTRL_ddc_u2r_en_st_mask) >> ISO_MISC_DDC_CTRL_ddc_u2r_en_st_shift);
#endif
}

static ssize_t rtk_uart2rbus_control_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	char *p, buffer[20] = {0};
	int len, cmd;
        int ret = 0;

	p = memchr(page, '\n', count);
	len = p ? p - page : count;
	strncpy(buffer, page, len);
	ret = sscanf(buffer, "%d", &cmd);
        if (ret != 1) {
                rtd_pr_mach_rtk_err("Bad line\n");
                return -EINVAL;
        }

	do_rtk_uart2rbus_control(cmd);

	return count;
}
REALTEK_BOARDS_ATTR_RW(rtk_uart2rbus_control);
#endif

static ssize_t update_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%d \n", platform_info.update_mode);
}

static ssize_t update_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	char *p, buffer[20] = {0};
	int i, len, mode, err;

	p = memchr(page, '\n', count);
	len = p ? p - page : count;
	strncpy(buffer, page, len);
	for (i = 0; i < len; i++)
		if (!isdigit(buffer[i])) {
			rtd_pr_mach_rtk_err("%c is not digit...\n", buffer[i]);
			return count;
		}
	err = kstrtoint(buffer, 10, &mode);
	if(err)
		return err;
	platform_info.update_mode = mode;

	return count;
}
REALTEK_BOARDS_ATTR_RW(update);

/*hw monitor API start*/
__attribute__((weak)) void hw_monitor_set_cmd (const char *buffer) {}

static ssize_t hwm_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
        rtd_pr_mach_rtk_err("%s called...\n", __FUNCTION__);
        return 0;
}

static ssize_t hwm_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
        rtd_pr_mach_rtk_err("%s called...\n", __FUNCTION__);
        hw_monitor_set_cmd(page);
        return count;
}

REALTEK_BOARDS_ATTR_RW(hwm);
/****************hw monitor API end****************/

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
// [ML7BU-1643] move  strNotifyAP from kernel/power/main.c to sysfs.c here
/*ML3RTANDN-468 : add STR event for AP notify*/
#define CONFIG_PM_STR_SUPPORT_EVENT
unsigned int pm_str_evnet_freeze_fail = 0;
#ifdef CONFIG_PM_STR_SUPPORT_EVENT
#define	STR_EVENT_SUSPEND_EVENT_READY_TIMEOUT_MS		40000
#define	STR_EVENT_SUSPEND_STR_MAIN_FLOW_WAIT_TIMEOUT_SEC	5
#define	STR_EVENT_INIT_STATE	0
#define	STR_EVENT_WAIT_SUSPEND_EVENT_STATE	1
#define	STR_EVENT_SUSPEND_EVENT_READY_STATE	2
#define	STR_EVENT_EXIT_WAIT_SUSPEND_EVENT_STATE	3

static DEFINE_SEMAPHORE(str_main_flow_sema);
static DECLARE_WAIT_QUEUE_HEAD(pm_str_event_wait_queue);

static volatile unsigned int pm_str_event_wait_flag = 1;
static volatile unsigned int str_event_state =  STR_EVENT_INIT_STATE;
static volatile unsigned int str_event_log =0;
/*When AP echo enable_force_wait at first, then system will enable force waiting flow
   that means the STR suspend main flow will wait until STR_Worker issue next suspend_event_ready event.*/
static unsigned int pm_str_event_force_wait = 0;
#endif
void str_event_before_suspend_peration(suspend_state_t state)
{
#ifdef CONFIG_PM_STR_SUPPORT_EVENT
	unsigned long timeout_check = jiffies + HZ*STR_EVENT_SUSPEND_STR_MAIN_FLOW_WAIT_TIMEOUT_SEC;

	if(state == PM_SUSPEND_MEM)
	{
		unsigned int cnt = 1;
		//wait here for STR_Worker issue next wait_suspend_event
		while( (pm_str_event_force_wait == 1) && (str_event_state != STR_EVENT_WAIT_SUSPEND_EVENT_STATE))
		{
			str_event_log |= 0x1;
			if(time_after(jiffies, timeout_check))
			{
				rtd_pr_mach_rtk_err("STR_NotifyAP(v) : wait timeout (%d sec)(%d)\n", STR_EVENT_SUSPEND_STR_MAIN_FLOW_WAIT_TIMEOUT_SEC * cnt, str_event_state);
				timeout_check = jiffies + HZ*STR_EVENT_SUSPEND_STR_MAIN_FLOW_WAIT_TIMEOUT_SEC;
				cnt++;
				str_event_log |= 0x10;
			}
		}

		pm_str_event_wait_flag = 0;
		wake_up(&pm_str_event_wait_queue);

		//wait here for str event finish, but have timetout value
		if(down_timeout(&str_main_flow_sema, msecs_to_jiffies(STR_EVENT_SUSPEND_EVENT_READY_TIMEOUT_MS)))
		{
			str_event_log |= 0x2;
			str_event_state = STR_EVENT_INIT_STATE;
			rtd_pr_mach_rtk_err("STR_NotifyAP(v) : Timeout for suspend event ready (%d ms)\n", STR_EVENT_SUSPEND_EVENT_READY_TIMEOUT_MS);
		}
	}
#endif
}

void str_event_after_resume_operation(suspend_state_t state)
{
#ifdef CONFIG_PM_STR_SUPPORT_EVENT
	if(state == PM_SUSPEND_MEM)
	{
		up(&str_main_flow_sema);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
		str_event_log |= 0x80;
		pm_str_event_wait_flag = 0;
		wake_up(&pm_str_event_wait_queue);
#else
		pm_str_event_wait_flag = 1;
		str_event_log = 0;
#endif
	}
#endif
}

static int str_notifyAP_pm_event(struct notifier_block *this, unsigned long event, void *ptr)
{
        switch (event) {
	case PM_SUSPEND_PREPARE:
		str_event_before_suspend_peration(PM_SUSPEND_MEM);
		break;
	case PM_POST_SUSPEND:
		str_event_after_resume_operation(PM_SUSPEND_MEM);
		break;
        default:
                break;
        }
	return NOTIFY_DONE;
}

static struct notifier_block str_notifyAP_notifier = {
        .notifier_call = str_notifyAP_pm_event,
};


#ifdef CONFIG_PM_STR_SUPPORT_EVENT
static ssize_t pm_STR_event_show(struct kobject *kobj,
				      struct kobj_attribute *attr, char *buf)
{
	//extern void print_current_irq_records(int);
	//print_current_irq_records(-1);
	rtd_pr_mach_rtk_notice("STR_NotifyAP(v) : Support wait_suspend_event /suspend_event_ready\n");
	rtd_pr_mach_rtk_notice("STR_NotifyAP(v) : Current event state is (%d)\n", str_event_state);
	rtd_pr_mach_rtk_notice("STR_NotifyAP(v) : Current force wait is (%d), str_event_log(%d)\n", pm_str_event_force_wait, str_event_log);
	return 0;
}
#define LPM_WAIT_RETRY_EACH_TIME_NORMAL	(50)
#define LPM_WAIT_RETRY_EACH_TIME_ERROR	(1000)
//extern void set_irq_records_clear(void);
//extern void reset_desc_rtk_interrupt_statistic(void);
/*
echo wait_suspend_event > /sys/realtek_boards/pm_STR_event
echo suspend_event_ready > /sys/realtek_boards/pm_STR_event
echo enable_force_wait > /sys/realtek_boards/pm_STR_event
echo clear > /sys/realtek_boards/pm_STR_event
*/
static ssize_t pm_STR_event_store(struct kobject *kobj,
				       struct kobj_attribute *attr,
				       const char *buf, size_t n)
{
	int error = 0;
#ifdef CONFIG_LPM_SUPPORT_ENABLE
static unsigned int pm_lpm_state = PM_LPM_POWER_MODE_S0;
unsigned int todo;
unsigned int sleep_msecs;
#endif
#ifdef CONFIG_PM_STR_SUPPORT_EVENT
	unsigned int input_state;
	char *p;
	int len;
	p = memchr(buf, '\n', n);
	len = p ? p - buf : n;
	if (len == 18 && !strncmp(buf, "wait_suspend_event", len))
		input_state = STR_EVENT_WAIT_SUSPEND_EVENT_STATE;
	else if(len == 19 && !strncmp(buf, "suspend_event_ready", len))
		input_state = STR_EVENT_SUSPEND_EVENT_READY_STATE;
	else if(len == 23 && !strncmp(buf, "exit_wait_suspend_event", len))
		input_state = STR_EVENT_EXIT_WAIT_SUSPEND_EVENT_STATE;
	else if(len == 5 && !strncmp(buf, "clear", len))
	{
		//set_irq_records_clear();
		return n;
	}
	else if(len == 5 && !strncmp(buf, "reset", len))
	{
		//reset_desc_rtk_interrupt_statistic();
		return n;
	}
	else if(len == 17 && !strncmp(buf, "enable_force_wait", len))
	{
		pm_str_event_force_wait = 1;
		rtd_pr_mach_rtk_err("STR_NotifyAP(v) :  set pm_str_event_force_wait on\n");
		return n;
	}
#ifdef CONFIG_LPM_SUPPORT_ENABLE
	else if(len == 12 && !strncmp(buf, "enter_lpm_S1", len))
	{
		mutex_lock(&rtk_lpm_lock);

		if(pm_lpm_state != PM_LPM_POWER_MODE_S0)
		{
			pr_err("[LPM] Error: enter_lpm_S1(%d)\n", pm_lpm_state);
			mutex_unlock(&rtk_lpm_lock);
			return -EINVAL;
		}

		pm_lpm_state = PM_LPM_POWER_MODE_S1;
		pr_err("[LPM] Flow: prepare to S1 ...\n");
#ifndef CONFIG_LPM_BRINGUP_VERIFY
		lpm_notifier_call_chain(PM_LPM_POWER_MODE_S1);
#endif
		pr_err("[LPM] Flow: Finish to S1\n");
		mutex_unlock(&rtk_lpm_lock);
		return n;
	}
	else if(len == 12 && !strncmp(buf, "enter_lpm_S2", len))
	{
#ifndef CONFIG_LPM_BRINGUP_VERIFY
		struct rtk_lpm_dev_info *info;
#endif
		mutex_lock(&rtk_lpm_lock);
		if(pm_lpm_state != PM_LPM_POWER_MODE_S1)
		{
			pr_err("[LPM] Error: enter_lpm_S2(%d)\n", pm_lpm_state);
			mutex_unlock(&rtk_lpm_lock);
			return -EINVAL;
		}

		pm_lpm_state = PM_LPM_POWER_MODE_S2;
		pr_err("[LPM] Flow: prepare to S2 ...\n");
#ifndef CONFIG_LPM_BRINGUP_VERIFY
		pr_err("[LPM] RPM: start put ...\n");
		list_for_each_entry(info, &rtk_lpm_dev_list, list_head)
		{
			struct device *dev = info->dev;
			pr_err("[LPM] put dev(%s: %x)\n", dev_name(dev), dev);
			pm_runtime_put(dev);
		}

		sleep_msecs = 0;
		while(1)
		{
			todo = 0;
			list_for_each_entry(info, &rtk_lpm_dev_list, list_head)
			{
				struct device *dev = info->dev;
				if( !pm_runtime_suspended(dev))
				{
					pr_err("(%d)[LPM] S2 wait(%s)\n", sleep_msecs, dev_name(dev));
					todo++;
				}
			}

			if(todo == 0)
				break;

			if(sleep_msecs <= LPM_WAIT_RETRY_EACH_TIME_ERROR)
			{
				sleep_msecs += LPM_WAIT_RETRY_EACH_TIME_NORMAL;
				msleep(LPM_WAIT_RETRY_EACH_TIME_NORMAL);
			}
			else
			{
				sleep_msecs += LPM_WAIT_RETRY_EACH_TIME_ERROR;
				msleep(LPM_WAIT_RETRY_EACH_TIME_ERROR);
			}
		}
		pr_err("[LPM] RPM: finish put\n");
		lpm_notifier_call_chain(PM_LPM_POWER_MODE_S2);//later after RPM
#endif//CONFIG_LPM_BRINGUP_VERIFY
		pr_err("[LPM] Flow: Finish to S2\n");
		mutex_unlock(&rtk_lpm_lock);
		return n;
	}
	else if(len == 8 && !strncmp(buf, "exit_lpm", len))
	{
#ifndef CONFIG_LPM_BRINGUP_VERIFY
		struct rtk_lpm_dev_info *info;
#endif
		mutex_lock(&rtk_lpm_lock);
		if(pm_lpm_state == PM_LPM_POWER_MODE_S0)
		{
			pr_err("[LPM] Error: exit_lpm error(%d)\n", pm_lpm_state);
			mutex_unlock(&rtk_lpm_lock);
			return -EINVAL;
		}
		pr_err("[LPM] Flow: prepare to S0 ...\n");

#ifndef CONFIG_LPM_BRINGUP_VERIFY
		lpm_notifier_call_chain(PM_LPM_POWER_MODE_S0);//early before RPM
#endif

		if(pm_lpm_state == PM_LPM_POWER_MODE_S1)
			goto EXIT_LPM;

#ifndef CONFIG_LPM_BRINGUP_VERIFY
		pr_err("[LPM] RPM: start get ...\n");
		list_for_each_entry(info, &rtk_lpm_dev_list, list_head)
		{
			struct device *dev = info->dev;
			pr_err("[LPM] get dev(%s: %x)\n", dev_name(dev), dev);
			pm_runtime_get(dev);
		}
		sleep_msecs = 0;
		while(1)
		{
			todo = 0;
			list_for_each_entry(info, &rtk_lpm_dev_list, list_head)
			{
				struct device *dev = info->dev;
				if( !pm_runtime_active(dev))
				{
					pr_err("(%d)[LPM] S0 wait(%s)\n", sleep_msecs, dev_name(dev));
					todo++;
				}
			}

			if(todo == 0)
				break;

			if(sleep_msecs <= LPM_WAIT_RETRY_EACH_TIME_ERROR)
			{
				sleep_msecs += LPM_WAIT_RETRY_EACH_TIME_NORMAL;
				msleep(LPM_WAIT_RETRY_EACH_TIME_NORMAL);
			}
			else
			{
				sleep_msecs += LPM_WAIT_RETRY_EACH_TIME_ERROR;
				msleep(LPM_WAIT_RETRY_EACH_TIME_ERROR);
			}
		}
		pr_err("[LPM] RPM: finish get\n");
#endif//CONFIG_LPM_BRINGUP_VERIFY
EXIT_LPM:
		pm_lpm_state = PM_LPM_POWER_MODE_S0;
		pr_err("[LPM] Flow: Finish to S0\n");
		mutex_unlock(&rtk_lpm_lock);
		return n;
	}
#endif //CONFIG_LPM_SUPPORT_ENABLE
	else
	{
		str_event_log |= 0x4;
		rtd_pr_mach_rtk_err("STR_NotifyAP(v) : Not support event(%d)\n", str_event_state);
		error = -EINVAL;
		return error;
	}

	// for state init -> wait_suspend_stae or suspend_event_ready state -> wait_suspend_stae
	if(((str_event_state == STR_EVENT_INIT_STATE) && (input_state == STR_EVENT_WAIT_SUSPEND_EVENT_STATE)))
		//||( (str_event_state == STR_EVENT_SUSPEND_EVENT_READY_STATE) && (input_state == STR_EVENT_WAIT_SUSPEND_EVENT_STATE)))
	{
		if(down_trylock(&str_main_flow_sema) != 0)
		{
			if(pm_str_evnet_freeze_fail)
				while(1); //because freeze fail, wait here for emcu_on on active for enter standby mode
			else	//means str main flow already enter suspend flow, AP should issue this before already run str flow
				panic("STR_NotifyAP(v) : pm_STR_event_store state mismatch(%d/%d/%d)\n", input_state, pm_str_event_force_wait, str_event_log);
		}

		str_event_state = STR_EVENT_WAIT_SUSPEND_EVENT_STATE; //means STR_Worker gain sema
		wait_event(pm_str_event_wait_queue, (pm_str_event_wait_flag == 0));//wait here for str main flow start
		str_event_log |= 0x20;
		pm_str_event_wait_flag = 1;
	}
	else if((str_event_state == STR_EVENT_WAIT_SUSPEND_EVENT_STATE) && (input_state == STR_EVENT_SUSPEND_EVENT_READY_STATE))
	{
		str_event_state = 	STR_EVENT_SUSPEND_EVENT_READY_STATE;
		up(&str_main_flow_sema); //wakeup str main flow
		str_event_log |= 0x40;

		//chnage to set PF_FREEZER_SKIP and use wait_event to avoid wait evnet be interrupted.
		freezer_do_not_count();
		wait_event(pm_str_event_wait_queue, (pm_str_event_wait_flag == 0));//wait here for after resume
		str_event_state = 	STR_EVENT_INIT_STATE;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
		pm_str_event_wait_flag = 1;
		str_event_log = 0;
#endif
	}
	else if((str_event_state == STR_EVENT_WAIT_SUSPEND_EVENT_STATE) && (input_state == STR_EVENT_EXIT_WAIT_SUSPEND_EVENT_STATE))
	{
		str_event_state = STR_EVENT_EXIT_WAIT_SUSPEND_EVENT_STATE;
		pm_str_event_wait_flag = 0;
		wake_up(&pm_str_event_wait_queue);
		rtd_pr_mach_rtk_err("STR_NotifyAP(v) : force to exit wait supsend event state!!\n");
		str_event_log |= 0x80;
	}
	else
	{
		str_event_log |= 0x8;
		rtd_pr_mach_rtk_err("STR_NotifyAP(v) : Current state is %d, and input state is %d\n", str_event_state, input_state);
		error = -EINVAL;
	}
#endif
	return error;
}
REALTEK_BOARDS_ATTR_RW(pm_STR_event);
#endif
#endif //#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))

static ssize_t misc_operations_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "0\n");
}

// We use "extern prom_printf" here because when printk is disabled, prom_printf will be defined as null if "prom.h" is included
extern void prom_printf(char *fmt, ...);

static ssize_t misc_operations_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	char *p;
	int len;

	p = memchr(page, '\n', count);
	len = p ? p - page : count;

	if(len>0 && !strncmp(page, "1", 1)) {
		if(len>1 && !strncmp(page+1, " ", 1))
			rtd_pr_mach_rtk_err("This is the current time: %s", page+2);
		else {
		rtd_pr_mach_rtk_err("--------- %d\n", len);
			rtd_pr_mach_rtk_err("This is the current time.\n");
		}
	} else if(len>0 && !strncmp(page, "2", 1)) {
		// set the default location of core dump...
		if(len>1 && !strncmp(page+1, " ", 1)) {
//			strcpy(corepath, page+2);
//			*strchr(corepath, '\n') = 0;
//			rtd_pr_mach_rtk_err("set corepath: %s \n", corepath);
		} else {
			rtd_pr_mach_rtk_err("Error, you must specify the location...\n");
		}
	}
	return count;
}
REALTEK_BOARDS_ATTR_RW(misc_operations);

#if defined(CONFIG_RTK_KDRV_RTICE)
int rtice_enable = 1;
static ssize_t rtice_enable_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
        return snprintf(page, PAGE_SIZE-1, "%x\n", rtice_enable);
}

static ssize_t rtice_enable_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
//        char *p;
//        int len;


 	 rtice_enable = simple_strtoul(page, 0, 0);

        return count;
}
REALTEK_BOARDS_ATTR_RW(rtice_enable);
#endif

/*
static ssize_t vout_interface_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return sprintf(page, "%d \n", (int)platform_info.vout_interface[0]);
}

static ssize_t vout_interface_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
	char *p;
	int len;

	p = memchr(page, '\n', count);
	if (!p)
		return count;
	len = p - page;
	if (!strncmp(page, "0", len)) {
		rtd_pr_mach_rtk_err("[SYS] setting vout to component...\n");
		sscanf(page, "%d", &platform_info.vout_interface[0]);
	} else if (!strncmp(page, "1", len)) {
		rtd_pr_mach_rtk_err("[SYS] setting vout to SCART...\n");
		sscanf(page, "%d", &platform_info.vout_interface[0]);
	} else {
		rtd_pr_mach_rtk_err("[SYS] Unknown vout setting...\n");
	}

	return count;
}
REALTEK_BOARDS_ATTR_RW(vout_interface);
*/

static ssize_t system_parameters_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%s\n", platform_info.system_parameters);
}
REALTEK_BOARDS_ATTR_RO(system_parameters);

static ssize_t AES_CCMP_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	if(platform_info.AES_CCMP_len>0) {
		memcpy(page, platform_info.AES_CCMP, platform_info.AES_CCMP_len);
		return platform_info.AES_CCMP_len;
	} else
		return 0;
}

REALTEK_BOARDS_ATTR_RO(AES_CCMP);

#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)

static ssize_t panel_parameter_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	if(platform_info.panel_parameter_len>0) {
		memcpy(page, platform_info.panel_parameter, platform_info.panel_parameter_len);
		return platform_info.panel_parameter_len;
	} else
		return 0;
}
REALTEK_BOARDS_ATTR_RO(panel_parameter);

// Darwin pcb enumeration for /sys/realtek_boards/pcb_enum

//__setup("pcb_enum=", pcb_enum_add);
#endif
static ssize_t modelconfig_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	if(platform_info.modelconfig_len>0) {
		memcpy(page, platform_info.modelconfig, platform_info.modelconfig_len);
		return platform_info.modelconfig_len;
	} else
		return 0;
}
REALTEK_BOARDS_ATTR_RO(modelconfig);

static ssize_t signature_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%s\n", platform_info.signature);
}
REALTEK_BOARDS_ATTR_RO(signature);

static ssize_t AES_IMG_KEY_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	memcpy(page, platform_info.AES_IMG_KEY, AES_IMG_KEY_LEN);
	return AES_IMG_KEY_LEN;
}
REALTEK_BOARDS_ATTR_RO(AES_IMG_KEY);

static ssize_t RSA_KEY_MODULUS_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	if(platform_info.secure_boot == 1) {
		memcpy(page, platform_info.RSA_KEY_MODULUS, RSA_KEY_MODULUS_LEN);
		return RSA_KEY_MODULUS_LEN;
	} else
		return 0;
}
REALTEK_BOARDS_ATTR_RO(RSA_KEY_MODULUS);

static ssize_t _priv__show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%s\n", platform_info._priv_);
}
REALTEK_BOARDS_ATTR_RO(_priv_);

static ssize_t _custom_param__show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
	return snprintf(page, PAGE_SIZE-1, "%s\n", platform_info._custom_param_);
}
REALTEK_BOARDS_ATTR_RO(_custom_param_);

static inline unsigned long get_MPIDR(void)
{
	unsigned long val;

#ifdef CONFIG_ARM64
	val = read_cpuid_mpidr();
#else
	asm("mrc   p15, 0, %0, c0, c0, 5	@ get MPIDR" : "=r" (val) : : "cc");
#endif
	return val;
}


#ifdef CONFIG_RTK_KDRV_SYSFS
static int cmdline_find_option(const char *option, char *buffer, int bufsize)
{
	extern char *saved_command_line;
	char *start = NULL;
	char *end = NULL;
	int size = 0;


	start = strstr(saved_command_line, option);
	if(start == NULL)
	{
		strncpy(buffer, "bootcode_git_version = unknow", bufsize-1);
                buffer[bufsize-1] = 0;
		return -1;
	}

	if((end = strstr(start, " ")))   // end with 'space'
	{
		size = end-start;
                if(size > bufsize -1)
                        size = bufsize -1;
		strncpy(buffer, start, size);
                buffer[bufsize-1] = 0;
	}
	else if((end = strstr(start, "\0"))) // end with '\0'
	{
		strncpy(buffer, start, bufsize-1);
                buffer[bufsize-1] = 0;
	}
	else
	{
		strncpy(buffer, "bootcode_git_version = unknow", bufsize-1);
                buffer[bufsize-1] = 0;
		return -1;
	}
	return 0;
}
#endif

#if IS_ENABLED(CONFIG_ARCH_RTK6702) && IS_ENABLED(CONFIG_REALTEK_VOLTAGE_CTRL)
extern int rtk_cpu_dvs_enable;
extern int rtk_cpu_voltage_val;
void rtk_set_voltage_directly(int level);
#endif

static ssize_t scpu_voltage_show(struct kobject *kobj, struct kobj_attribute *attr, char *page)
{
#if IS_ENABLED(CONFIG_ARCH_RTK6702) && IS_ENABLED(CONFIG_REALTEK_VOLTAGE_CTRL)
		if(rtk_cpu_dvs_enable == 0)
			return snprintf(page, PAGE_SIZE-1,"%d\n",rtk_cpu_voltage_val);
		else
			return snprintf(page, PAGE_SIZE-1,"%d\n",0);
#else
		return snprintf(page, PAGE_SIZE-1,"%d\n",0);
#endif
}
static ssize_t scpu_voltage_store(struct kobject *kobj, struct kobj_attribute *attr, const char *page, size_t count)
{
#if IS_ENABLED(CONFIG_ARCH_RTK6702) && IS_ENABLED(CONFIG_REALTEK_VOLTAGE_CTRL)
	if(rtk_cpu_dvs_enable == 0)
	{
		rtk_cpu_voltage_val = simple_strtoul(page, NULL, 0);
		if(rtk_cpu_voltage_val != 0)
			rtk_set_voltage_directly(rtk_cpu_voltage_val);
	}
	return count;
#else
	return count;
#endif
}
REALTEK_BOARDS_ATTR_RW(scpu_voltage);

#ifdef CONFIG_RTK_FEATURE_FOR_GKI
#define PLL_DUMP_GPU_APM_INFO
#endif

#define PAGE_INFO_SIZE          1024
static ssize_t pll_info_show(struct kobject * kobj, struct kobj_attribute * attr, char * page)
{
#if 1	//TODO
	//	char   msg[64];
	//char   page_info[512];
	char   msg[128];
	ssize_t ret_size;
	char *page_info;
    //char   page_info[1024];
	uint value_f,value_n,value_m;
#ifdef PLL_DUMP_GPU_APM_INFO
	uint value;
#endif
	uint freq, tmp;
	uint gpu_pre_div;
	int len = 0;

	//UINT32 frame_rate, cnt = 0x1fff, stable_cnt = 0;
        //UINT32 xclk = 270000000;
	//UINT32 memc_dtg_frame_rate = 1;
#if 0
	/* DFS relate */
	struct file *filp0,*filp1,*filp2,*filp3;
	uint dfs_enable_A53 = 0; // if enable,value = 1; if disable ,value = 0;

	/* SCPU PLL relate */
	uint d_divisor = 0;
	mm_segment_t fs;
	struct inode *inode;
	off_t fsize;
	int ret;
	char buf[32];
	// running cpu info
	int i = 0;
	u32 cpuid = 0;
	u32 core_A53 = 0;
#else
#ifdef PLL_DUMP_GPU_APM_INFO
	struct file *filp0;
#endif
#endif

	if ((page_info = kmalloc(PAGE_INFO_SIZE, GFP_KERNEL)) == NULL)
	{
		rtd_pr_mach_rtk_err("OOM for pll info\n");
		return 0; //none
	}

	memset(page_info, 0, PAGE_INFO_SIZE);

#if 0
        while(cnt--){
                frame_rate = xclk / rtd_inl(0xB80282EC);
                if((frame_rate % 10) >4)
                        frame_rate = frame_rate / 10 + 1;
                else
                        frame_rate = frame_rate / 10;
                if(frame_rate == memc_dtg_frame_rate){
                        if(stable_cnt == 5)
                                break;
                        else
                                stable_cnt++;
                }
                else{
                        if(memc_dtg_frame_rate !=0)
                                stable_cnt--;
                        memc_dtg_frame_rate = frame_rate;
                        msleep(5);
                }
        }

	memset(msg, 0, sizeof(msg));
        sprintf( msg, "MEMC DTG output frame_rate = %dHz\n", frame_rate);
        strcat(page_info, msg);

	value_f = 0;
        tmp = rtd_inl(0xb809c038);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c040);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c048);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c050);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c070);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c090);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c098);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c0a0);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c114);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c11C);
        value_f += ( tmp >> 20 ) & 1;

	tmp = rtd_inl(0xb809c138);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c140);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c15C);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c164);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c180);
        value_f += ( tmp >> 20 ) & 1;

        tmp = rtd_inl(0xb809c188);
        value_f += ( tmp >> 20 ) & 1;

        memset(msg, 0, sizeof(msg));
        if ( value_f == 0 )
                sprintf( msg, "MEMC repeated mode  : On \n");
        else
                sprintf( msg, "MEMC repeated mode  : Off \n");
        strcat(page_info, msg);

	// get RTNR on/off (B8024000 B[28]=1) && (0xb80241a0 B[0]=1)
	tmp = rtd_inl(0xB8024000);
	value_f = ((tmp & 0x10000000) >> 28);
	tmp = rtd_inl(0xb80241a0);
	value_n = ((tmp & _BIT0));

	memset(msg, 0, sizeof(msg));
	if ( value_f && value_n )
		sprintf( msg, "RTNR                : On \n");
	else
	      sprintf( msg, "RTNR                : Off \n");
	strcat(page_info, msg);
#endif
	// VE compression on/off B8005100 B[0]=1
	tmp = rtd_inl(0xB8005100) & 1;
	memset(msg, 0, sizeof(msg));
	if ( tmp )
		snprintf( msg, sizeof(msg)-1, "VE compression      : On \n");
	else
		snprintf( msg, sizeof(msg)-1, "VE compression      : Off \n");
    msg[sizeof(msg)-1] = 0;
	len = PAGE_INFO_SIZE-strlen(page_info)-1;
	if (len > 0)
		strncat(page_info, msg, len);

	// get GPU PLL FN value
	tmp = rtd_inl(0xB8000454);
	gpu_pre_div = ((rtd_inl(0xB8000450) & 0x30) >> 4)+1;
	value_f = ((tmp & 0x07FF0000) >> 16);
	value_n = ((tmp & 0x0000FF00) >> 8);
	freq = 27 * (value_n + 4) + ((27 * value_f) >> 11); // PLL Frequency = 27*(NCODE +4+ FCODE/2048)
	freq = freq/gpu_pre_div;

	memset(msg, 0, sizeof(msg));
	snprintf( msg, sizeof(msg)-1, "GPU    PLL       = %4d MHz\n", freq );
        msg[sizeof(msg)-1] = 0;
	strcat(page_info, msg);

		// get DDR_1 PLL FN value
	tmp = rtd_inl(0xb80c9024);
	value_f = ((tmp & 0x07FF0000) >> 16);
	tmp = rtd_inl(0xb80c9028);
	value_n = ((tmp & 0x0000FF00) >> 8);
	freq = 27 * (value_n + 3) + ((27 * value_f) >> 11); //	27*(NCODE + 3 + FCODE/2048)
	memset(msg, 0, sizeof(msg));

	//coverity 557779: Logically dead code (DEADCODE), the value of freq must be between 81 and 6992
	snprintf( msg, sizeof(msg)-1, "DDR    PLL       = %4d MHz (DC1)\n", freq );
        msg[sizeof(msg)-1] = 0;
	strcat(page_info, msg);

#if 0
	// get DDR_1 PLL FN value
	tmp = rtd_inl(0xb8004024);
	value_f = ((tmp & 0x07FF0000) >> 16);
	tmp = rtd_inl(0xb8004028);
	value_n = ((tmp & 0x0000FF00) >> 8);
	freq = 27 * (value_n + 3) + ((27 * value_f) >> 11); //	27*(NCODE + 3 + FCODE/2048)
	memset(msg, 0, sizeof(msg));
	if (freq != 0)
		sprintf( msg, "DDR    PLL       = %4d MHz (DC2)\n", freq );
	else
		sprintf( msg, "DDR    PLL       = --- MHz (DC2)\n");
	strcat(page_info, msg);
#endif

#ifdef CONFIG_RTK_KDRV_SYSFS
	// get bootcode version
	memset(msg, 0, sizeof(msg));
	cmdline_find_option("bootcode_git_version", msg, sizeof(msg));
	strcat(page_info, msg);
	strcat(page_info, "\n");
#endif

	// get IB enable or not
	memset(msg, 0, sizeof(msg));
	if (0x1010 == (rtd_inl(0xb805c084) & 0xffff)) {
		snprintf( msg, sizeof(msg)-1, "IB                  : disable \n");
	}
	else {
		snprintf( msg, sizeof(msg)-1, "IB                  : enable \n");
	}
        msg[sizeof(msg)-1] = 0;
	strcat(page_info, msg);

	// get Arbitration enable or not
	memset(msg, 0, sizeof(msg));
	if (0 == (rtd_inl(0xb8007400) & (_BIT0 | _BIT1))) {
		snprintf( msg, sizeof(msg)-1, "Arbitration         : disable \n");
	}
	else {
		snprintf( msg, sizeof(msg)-1, "Arbitration         : enable \n");
	}
        msg[sizeof(msg)-1] = 0;
	strcat(page_info, msg);

#ifdef PLL_DUMP_GPU_APM_INFO
	// GPU APM
	memset(msg, 0, sizeof(msg));
	filp0 = filp_open("/dev/dri/renderD128",O_RDONLY,0);
	if(IS_ERR(filp0))
	{
		snprintf( msg, sizeof(msg)-1, "GPU APM             : disable \n");
	}
	else
	{
		filp_close(filp0, 0);
		if(0 == (rtd_inl(0xb8000114) & _BIT3))
		{
			snprintf( msg, sizeof(msg)-1, "GPU APM             : enable \n");
		}
		else
		{
			if (0 == (rtd_inl(0xb810e09c) & _BIT4)) {
				snprintf( msg, sizeof(msg)-1, "GPU APM             : enable \n");
			}
			else {
				snprintf( msg, sizeof(msg)-1, "GPU APM             : disable \n");
			}
		}
	}
	strcat(page_info, msg);

	// VB1 status
	memset(msg, 0, sizeof(msg));
	tmp = rtd_inl(0xb802d704);
	value = ((tmp & 0x001f0000) >> 16);
	if(value)
	{
		snprintf(msg, sizeof(msg)-1, "VB1 status          : %d lanes\n", value);
	}
	else
	{
		snprintf(msg, sizeof(msg)-1, "VB1 status          : not use\n");
	}

	strcat(page_info, msg);
#endif //PLL_DUMP_GPU_APM_INFO
	// VP9 CLOCK
	memset(msg, 0, sizeof(msg));
	if (1 == (rtd_inl(0xb8000110) & _BIT13)) {
		snprintf( msg, sizeof(msg)-1, "VP9 clock           : enable \n");
	}
	else {
		snprintf( msg, sizeof(msg)-1, "VP9 clock           : disable\n");
	}
	strcat(page_info, msg);

	// EMMC MODE and PLL
	tmp = rtd_inl(0xb8000720);
	value_f = ((tmp & 0x00ff0000)>>16);

	tmp = rtd_inl(0xb8000700);
	value_n = ((tmp & 0x00000700) >> 8);
	value_m = ((tmp & 0x00020000) >> 17);
	freq = 27 * (value_f + 3)/(value_n+2)/(value_m+1)/2; //  27 *( PLLEMMC_NCODE_T +3)/ (PLLSN+2)/( PLLSDIV2+1)/2
    memset(msg, 0, sizeof(msg));
	if (freq != 0)
		snprintf( msg, sizeof(msg)-1, "EMMC    PLL      = %4d MHz \n", freq );
	else
		snprintf( msg, sizeof(msg)-1, "EMMC    PLL      = --- MHz \n");
	strcat(page_info, msg);

	memset(msg, 0, sizeof(msg));
	tmp = rtd_inl(0xb8010a0c);
	if(tmp & _BIT31)
	{
		snprintf( msg, sizeof(msg)-1, "eMMC MODE           :  HS400 (%u)\n", freq );
	}
	else
	{
		if(freq > 50)
			snprintf( msg, sizeof(msg)-1, "eMMC MODE           :  HS200 (%u)\n", freq );
		else
		{
			snprintf( msg, sizeof(msg)-1, "eMMC MODE           :  SDR50 (%u)\n", freq );
		}
	}
	strcat(page_info, msg);

#if 0 //remove display the DFS and PLL information
	for_each_online_cpu(i) {

#ifdef CONFIG_ARM64
        cpuid = read_cpuid_id();
#else
        cpuid = is_smp() ? per_cpu(cpu_data, i).cpuid : read_cpuid_id();
#endif
		if((cpuid & 0x0000fff0) == 0x0000d030) {
				core_A53++;
		}
	}
	memset(msg, 0, sizeof(msg));
	snprintf( msg, sizeof(msg)-1, "Running CPU core   : A53 x%d\n", core_A53);
	strcat(page_info, msg);

	// SCPU DFS
	filp0 = filp_open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor",O_RDONLY,0);
	filp1 = filp_open("/sys/devices/system/cpu/cpu1/cpufreq/scaling_governor",O_RDONLY,0);
	filp2 = filp_open("/sys/devices/system/cpu/cpu2/cpufreq/scaling_governor",O_RDONLY,0);
	filp3 = filp_open("/sys/devices/system/cpu/cpu3/cpufreq/scaling_governor",O_RDONLY,0);
	memset(msg, 0, sizeof(msg));
	if((IS_ERR(filp0)) && (IS_ERR(filp1)) && (IS_ERR(filp2)) &&(IS_ERR(filp3)))
	{
		snprintf( msg, sizeof(msg)-1, "SCPU    DFS         : disable \n");
		dfs_enable_A53 = 0;
	}
	else
	{
		snprintf( msg, sizeof(msg)-1, "SCPU    DFS         : enable \n");
		if(0==IS_ERR(filp0))
		{
			dfs_enable_A53 = 1;
			filp_close(filp0, 0);
		}
		if(0==IS_ERR(filp1))
			filp_close(filp1, 0);
		if(0==IS_ERR(filp2))
		{
			filp_close(filp2, 0);
		}
		if(0==IS_ERR(filp3))
			filp_close(filp3, 0);
	}
	strcat(page_info, msg);

	// get SCPU:A53 PLL
	memset(msg, 0, sizeof(msg));
	memset(buf, 0, sizeof(buf));
	if(dfs_enable_A53 == 1) // DFS enable
	{
		filp0 = filp_open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq",O_RDONLY,0);
                /* [ML4SONYAND-1836] Coverity fix */
		if(IS_ERR_OR_NULL(filp0))
                {
                        strcat(page_info, "SCPU(A53) PLL: get fail\n");

                        /*
			if(page_info)
			{
				kfree(page_info);
				page_info=NULL;
			}
			return -1;
                        */
		}
                else {

    		        inode = filp0->f_path.dentry->d_inode;
		        fsize = inode->i_size;

		        fs = get_fs();
		        set_fs(KERNEL_DS);
		        ret = filp0->f_op->read(filp0, buf, fsize, &(filp0->f_pos));
		        set_fs(fs);

		        if(ret > 0)
		        {
			        for(i=0; i<sizeof(buf); i++) // use 0 replace unused data
			        {
				        if(buf[i] == 0)
					        break;
				        if(buf[i]<'0' || buf[i]>'9')
				        {
					        buf[i] = 0;
				        }
			        }
			        snprintf(msg, sizeof(msg)-1, "SCPU(A53) PLL   = %s MHz\n", buf);
			        strcat(page_info, msg);
		        }
		        else if(ret == 0)
		        {
			        strcat(page_info, "SCPU(A53) PLL: get fail\n");
		        }
		        else
		        {
			        strcat(page_info, "SCPU(A53) PLL: get fail\n");
		        }

                        if(filp0)
                        {
		                filp_close(filp0, NULL);
                        }
                }/* End of if(IS_ERR(filp0) || (!filp0))*/
	}
	else // DFS disable
	{
		d_divisor = 0;
		tmp = rtd_inl(0xb805b004);
		value_f = ((tmp & 0x07FF0000) >> 16);
		value_n = ((tmp & 0x0000FF00) >> 8);
		freq = 27 * (value_n + 3) + ((27 * value_f) >> 11); // PLL Frequency = 27*(NCODE+3+FCODE/2048)
		d_divisor = (rtd_inl(0xb805b044) & 0xf0) >>4;
		freq = freq * (16-d_divisor)/16;
		memset(msg, 0, sizeof(msg));
		snprintf( msg, sizeof(msg)-1, "SCPU(A53) PLL   = %4d MHz\n", freq );
		strcat(page_info, msg);
	} // end if(dfs_enable_A53 == 1)
#endif//end of remove display the DFS and PLL information
	ret_size = snprintf(page, PAGE_SIZE-1, "%s", page_info);
	kfree(page_info);
	return ret_size;
#else
	return 0;
#endif	//TODO
}
REALTEK_BOARDS_ATTR_RO(pll_info);

#define VIDEO_GIT_VERSION  "video_git_version=not_support"
#define AUDIO_GIT_VERSION  	"audio_git_version=not_support"
extern unsigned char VideoGITHASH[8];
extern char AudioGITHASH[7];

#ifdef CONFIG_RTK_KDRV_SYSFS
extern char optee_git_version[13];
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 108))
extern const char rtk_linux_git_version[];
extern const char rtk_kdriver_git_version[];
#endif
#endif

static ssize_t fw_git_version_show(struct kobject * kobj, struct kobj_attribute * attr, char * page)
{
	char *page_info = NULL;
	char   msg[128];
	ssize_t ret_size;
	
	if ((page_info = kmalloc(1024, GFP_KERNEL)) == NULL)
	{
		rtd_pr_mach_rtk_err("OOM for fw_git_version\n");
		return 0; //none
	}

	memset(page_info, 0, 1024);

#ifdef CONFIG_RTK_KDRV_SYSFS
	// get bootcode version
	memset(msg, 0, sizeof(msg));
	cmdline_find_option("bootcode_git_version", msg, sizeof(msg));
	strcat(page_info, msg);
	strcat(page_info, "\n");
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_SYSFS)
	// get linux git version
	memset(msg, 0, sizeof(msg));
	snprintf(msg, sizeof(msg)-1, RTK_LINUX_GIT_VERSION);
	strcat(page_info, msg);
	strcat(page_info, "\n");

	// get rtk_kdriver git version
	memset(msg, 0, sizeof(msg));
	snprintf(msg, sizeof(msg)-1, RTK_KDRIVER_GIT_VERSION);
	strcat(page_info, msg);
	strcat(page_info, "\n");

	// get common git version
	memset(msg, 0, sizeof(msg));
	snprintf(msg, sizeof(msg)-1, RTK_COMMON_GIT_VERSION);
	strcat(page_info, msg);
	strcat(page_info, "\n");
#endif

#if defined(CONFIG_RTK_KDRV_TEE)
	//get optee git version
	memset(msg, 0, sizeof(msg));
	snprintf(msg, sizeof(msg)-1, "optee_git_version=%s", optee_git_version);
	strcat(page_info, msg);
	strcat(page_info, "\n");
	ret_size = snprintf(page, PAGE_SIZE-1, "%s", page_info);
#endif

	//get video git version
	memset(msg, 0, sizeof(msg));
#if (defined CONFIG_RTK_KDRV_VDEC)
	snprintf(msg, sizeof(msg)-1, "video_git_version=%s", VideoGITHASH);
#else
       snprintf(msg, sizeof(msg)-1, VIDEO_GIT_VERSION);
#endif
	strcat(page_info, msg);
	strcat(page_info, "\n");
	ret_size = snprintf(page, PAGE_SIZE-1, "%s", page_info);

	//get audio git version
	memset(msg, 0, sizeof(msg));
#if (defined CONFIG_RTK_KDRV_AUDIO)
	snprintf(msg, sizeof(msg)-1, "audio_git_version=%s", AudioGITHASH);
#else
        snprintf(msg, sizeof(msg)-1, AUDIO_GIT_VERSION);
#endif
	strcat(page_info, msg);
	strcat(page_info, "\n");
	ret_size = snprintf(page, PAGE_SIZE-1, "%s", page_info);
	
	kfree(page_info);
	return ret_size;
}
REALTEK_BOARDS_ATTR_RO(fw_git_version);

//move to common.c
struct kobject *realtek_boards_kobj;
EXPORT_SYMBOL(realtek_boards_kobj);
//extern struct kobject *realtek_boards_kobj;

static struct attribute * realtek_boards_attrs[] = {
	&kernel_source_code_info_attr.attr,
	&bootloader_version_attr.attr,
	&board_id_attr.attr,
	&cpu_id_attr.attr,
	&tv_encoding_system_attr.attr,
#ifdef CONFIG_REALTEK_SHOW_STACK
	&task_stack_attr.attr,
#endif

//#ifdef CONFIG_BOOTUP_ANIMATION
//(defined(CONFIG_REALTEK_RESERVE_DVR) && !defined(CONFIG_REALTEK_AUTO_RECLAIM)) ||
//     defined(CONFIG_REALTEK_BOOT_UP_ANIMATION_2)
#ifdef CONFIG_BOOTUP_ANIMATION
	&reclaim_dvr_attr.attr,
#endif
//#endif
	&misc_operations_attr.attr,
//	&vout_interface_attr.attr,
	&system_parameters_attr.attr,
	&AES_CCMP_attr.attr,
	&signature_attr.attr,
	&AES_IMG_KEY_attr.attr,
	&RSA_KEY_MODULUS_attr.attr,
	&_priv__attr.attr,
	&_custom_param__attr.attr,
	&modelconfig_attr.attr,
	&update_attr.attr,
	&hwm_attr.attr,

#if IS_ENABLED(CONFIG_RTK_KDRV_WATCHDOG)
	&watchdog_attr.attr,
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_SB2)
	&sb2_attr.attr,
	&sb2_dbg_attr.attr,
#endif

#if IS_ENABLED(CONFIG_REALTEK_ARM_WRAPPER)
	&arm_wrapper_attr.attr,
#endif

#ifdef CONFIG_RTK_KDRV_QOS_CONTROL
	&rtk_qos_attr.attr,
#endif
#ifdef CONFIG_REALTEK_UART2RBUS_CONTROL
	&rtk_uart2rbus_control_attr.attr,
#endif
#if IS_ENABLED(CONFIG_REALTEK_LOGBUF)
    	&rtdlog_attr.attr,
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#ifdef CONFIG_PM_STR_SUPPORT_EVENT
	&pm_STR_event_attr.attr,
#endif
#endif

#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)
	&panel_parameter_attr.attr,
#endif
#if defined(CONFIG_RTK_KDRV_RTICE)
	&rtice_enable_attr.attr,
#endif

	&scpu_voltage_attr.attr,
	&pll_info_attr.attr,

        &fw_git_version_attr.attr,

#ifdef CONFIG_REALTEK_SCHED_LOG
	&sched_log_time_duration_attr.attr,
	&sched_log_file_start_attr.attr, // cat it will start to log file
	&sched_log_size_attr.attr, //default 6M
	&sched_log_file_stop_attr.attr,
	&sched_log_event_number_attr.attr,
	&sched_log_name_attr.attr, //default /tmp/usb/sda/sda1
#endif
	NULL,
	NULL
};

static struct attribute_group realtek_boards_attr_group = {
	.attrs = realtek_boards_attrs,
};

#define __LOGO_ATTR_RO(_name) { .name = __stringify(_name), .mode = 0444/*, .owner = THIS_MODULE */}

#define LOGO_ATTR_RO(_name) \
	static struct attribute _name##_attr = __LOGO_ATTR_RO(_name)


#if IS_ENABLED(CONFIG_RTK_KDRV_THERMAL_SENSOR)

extern ssize_t rtk_monitor_active_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t rtk_monitor_active_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
extern ssize_t rtk_monitor_delay_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t rtk_monitor_delay_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
extern ssize_t rtk_monitor_debug_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t rtk_monitor_debug_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

static struct kobj_attribute rtk_monitor_active_attr =
    __ATTR(active, 0644, rtk_monitor_active_show, rtk_monitor_active_store);

static struct kobj_attribute rtk_monitor_delay_attr =
    __ATTR(delay, 0644, rtk_monitor_delay_show, rtk_monitor_delay_store);

static struct kobj_attribute rtk_monitor_debug_attr = 
       __ATTR(debug, 0644, rtk_monitor_debug_show, rtk_monitor_debug_store);

static struct attribute * rtk_monitor_attrs[] = {
    &rtk_monitor_active_attr.attr,
    &rtk_monitor_delay_attr.attr,
    &rtk_monitor_debug_attr.attr,
    NULL
};

static struct attribute_group rtk_monitor_attr_group = {
    .name = "rtk_monitor",
    .attrs = rtk_monitor_attrs,
};

static int realtek_sysfs_rtk_monitor_init(struct kobject *parent_kobj)
{
    int err = 0;

    err = sysfs_create_group(parent_kobj, &rtk_monitor_attr_group);
    if(err) {
        rtd_pr_mach_rtk_err("Realtek: sysfs init rtk_monitor failed\n");
        return err;
    }

    return err;
}
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_CPU_FREQUENCY) && (IS_ENABLED(CONFIG_ARCH_RTK2851A) || IS_ENABLED(CONFIG_ARCH_RTK6748) || IS_ENABLED(CONFIG_ARCH_RTK6702) || IS_ENABLED(CONFIG_ARCH_RTK2851C) || IS_ENABLED(CONFIG_ARCH_RTK2851F) || IS_ENABLED(CONFIG_ARCH_RTK2885P) || IS_ENABLED(CONFIG_ARCH_RTK2819A))
extern ssize_t rtk_boost_performance_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t rtk_boost_performance_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
extern ssize_t rtk_boost_timeout_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t rtk_boost_timeout_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
extern ssize_t rtk_boost_max_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
//extern ssize_t rtk_boost_max_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
extern ssize_t rtk_boost_default_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
//extern ssize_t rtk_boost_default_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

static struct kobj_attribute rtk_boost_performance_attr =
	__ATTR(performance, 0644, rtk_boost_performance_show, rtk_boost_performance_store);

static struct kobj_attribute rtk_boost_timeout_attr =
	__ATTR(timeout, 0644, rtk_boost_timeout_show, rtk_boost_timeout_store);

static struct kobj_attribute rtk_boost_max_attr =
	__ATTR(max, 0444, rtk_boost_max_show, NULL);

static struct kobj_attribute rtk_boost_default_attr =
	__ATTR(default, 0444, rtk_boost_default_show, NULL);


static struct attribute * rtk_boost_attrs[] = {
	&rtk_boost_timeout_attr.attr,
	&rtk_boost_max_attr.attr,
	&rtk_boost_default_attr.attr,
	&rtk_boost_performance_attr.attr,
	NULL
};

static struct attribute_group rtk_boost_attr_group = {
    .name = "rtk_boost",
    .attrs = rtk_boost_attrs,
};

static int realtek_sysfs_rtk_boost_init(struct kobject *parent_kobj)
{
    int err = 0;

    err = sysfs_create_group(parent_kobj, &rtk_boost_attr_group);
    if(err) {
        rtd_pr_mach_rtk_err("Realtek: sysfs init rtk_boost failed\n");
        return err;
    }

    return err;
}
#endif

#ifdef CONFIG_RTK_KDRV_SYSFS_MODULE
extern int realtek_sysfs_pcb_enum_dir_init(struct kobject *parent_kobj);
#endif
int realtek_boards_sysfs_init(void)
{
	int error;
	realtek_boards_kobj = kobject_create_and_add("realtek_boards", NULL);

	if(!realtek_boards_kobj)
		return -ENOMEM;
	error = sysfs_create_group(realtek_boards_kobj, &realtek_boards_attr_group);
	if(error) {
		kobject_put(realtek_boards_kobj);
		return error;
	}

#ifdef CONFIG_RTK_KDRV_SYSFS_MODULE
	realtek_sysfs_pcb_enum_dir_init(realtek_boards_kobj);
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_THERMAL_SENSOR)
        realtek_sysfs_rtk_monitor_init(realtek_boards_kobj);
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_CPU_FREQUENCY) && (IS_ENABLED(CONFIG_ARCH_RTK2851A) || IS_ENABLED(CONFIG_ARCH_RTK6748) || IS_ENABLED(CONFIG_ARCH_RTK6702) || IS_ENABLED(CONFIG_ARCH_RTK2851C) || IS_ENABLED(CONFIG_ARCH_RTK2851F) || IS_ENABLED(CONFIG_ARCH_RTK2885P) || IS_ENABLED(CONFIG_ARCH_RTK2819A))
	realtek_sysfs_rtk_boost_init(realtek_boards_kobj);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#ifdef CONFIG_PM_STR_SUPPORT_EVENT
		register_pm_notifier(&str_notifyAP_notifier);
#endif
#endif

	rtd_pr_mach_rtk_err("Realtek: sysfs init done\n");

 	return 0;
}

#ifndef CONFIG_RTK_KDRV_SYSFS_MODULE
arch_initcall(realtek_boards_sysfs_init);
#else
int __init rtk_sysfs_module_init(void)
{
	realtek_boards_sysfs_init();
	rtd_pr_mach_rtk_warn("rtk_sysfs_module_init init finish\n");
	return 0;
}

void __exit rtk_sysfs_module_exit(void)
{
	;
}

module_init(rtk_sysfs_module_init);
module_exit(rtk_sysfs_module_exit);
MODULE_LICENSE("GPL");
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
#endif

