#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/rcupdate.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/freezer.h>
#include <linux/shrinker.h>
#include <linux/mm.h>
#include <linux/pageremap.h>

#include <trace/hooks/vmscan.h>
#include <trace/hooks/mm.h>
#include <rtd_log/rtd_module_log.h>

#include <uapi/linux/sched/types.h>


#define FOREGROUND_APP_ADJ  0
#define TARGET_NICE_VALUE -20

static char *target_thread_name = "animation";
module_param(target_thread_name, charp, 0644);
MODULE_PARM_DESC(target_thread_name, "Target thread name to adjust priority.");

static struct task_struct *adjuster;
static spinlock_t checklist_lock;
static atomic_t rtk_adjuster_should_sleep = ATOMIC_INIT(0);
static int checklist[32]={0} , index = 0;
static bool detect_thread = false;

static void rtk_adjust_nice(struct task_struct *task) {

    rtd_pr_adjuster_info("Successfully Enter rtk_adjust_priority().");
    rtd_pr_adjuster_info("Original nice value: %d, Target nice value: %d", task_nice(task), TARGET_NICE_VALUE);

    set_user_nice(task, TARGET_NICE_VALUE);
}

static void check_foreground_process(void *unused, struct task_struct *task)
{
    if (!task)
        return;

    if (detect_thread)
    {
        rtd_pr_adjuster_warn("Detect thread\n");
        return;

    }

    /*Check if the current task is subtask*/
    if (task->tgid != task->pid) {
        rtd_pr_adjuster_warn("Only Choose Thread Group\n");
        return;
    }

    if (task->signal->oom_score_adj < FOREGROUND_APP_ADJ) {
        rtd_pr_adjuster_warn("Get task( %s ) with oom_score_adj ( %d )t\n",
                task->comm, task->signal->oom_score_adj);
        return;
    }

    /*
    ** oom_adj: be set 0 when cold start 
    ** oom_score_adj: be set 0 when warm start
    */

    if (task->signal->oom_score_adj == FOREGROUND_APP_ADJ && index < 30) {
	spin_lock(&checklist_lock);    
        checklist[index] = task->pid;
	index++;
	spin_unlock(&checklist_lock);
	atomic_set(&rtk_adjuster_should_sleep, 0);
    }
}

static void rtk_try_setnice(void)
{
    struct task_struct *thread;
    struct task_struct *task = NULL;
    int i = 0, timeout = 0;
    pid_t pid = 0;
    while (index > 0 ) {
        spin_lock(&checklist_lock);
        rcu_read_lock();
        task = find_task_by_vpid(checklist[0]);

        for (i=0 ; i < index ; i++){
            checklist[i] = checklist[i+1];
            index--;
        }

        spin_unlock(&checklist_lock);

        if (task)
        {
            if (strstr(task->comm, "e-initial") != NULL){
                rtd_pr_adjuster_warn("sleep...\n");
                msleep(50);
            }
            
            if (task->policy == SCHED_FIFO && strstr(task->comm, "netflix")){
                pid = task->pid;
                detect_thread = true;
                task = NULL;
                rcu_read_unlock();

                spin_lock(&checklist_lock);
                for (i=0 ; i < index ; i++)
                            checklist[i] = 0;
                index = 0;
                spin_unlock(&checklist_lock);

                goto TRYNICE;
            }
        }
        rcu_read_unlock();
    }

    atomic_set(&rtk_adjuster_should_sleep, 1);
    return;
  
TRYNICE:
    timeout = 0 ;
    while (detect_thread && timeout < 10) {
        rcu_read_lock();
        task = find_task_by_vpid(pid);    
        if (task == NULL || task->policy != SCHED_FIFO) {
            rtd_pr_adjuster_warn("MIKETAG: process is not forground or exit\n");
            rcu_read_unlock();
            atomic_set(&rtk_adjuster_should_sleep, 1);
            return;
        }

        for_each_thread(task, thread) {
            if (strcmp(thread->comm, target_thread_name) == 0) {
               rtd_pr_adjuster_warn("Found target thread: %s [TID: %d]\n", thread->comm, thread->pid);
               rtk_adjust_nice(thread);
               detect_thread = false;
            }
        }

        task = NULL;
        rcu_read_unlock();
        if (detect_thread) {
            rtd_pr_adjuster_warn("No found animation, sleep 1 sec\n");
            timeout++;
            msleep(1000);
        }
    }
    atomic_set(&rtk_adjuster_should_sleep, 1);
} 

static int rtk_adjuster(void *arg)
{
    set_freezable();

    while (!kthread_should_stop()) {
        if (atomic_read(&rtk_adjuster_should_sleep)) {
            try_to_freeze();
            msleep(500);
            continue;
        }

        rtk_try_setnice();
    }

    return 0;
}


static int __init rtk_adjuster_init(void) {

    int ret = 0;

/*Only for merlin7 subatomic device*/
#ifdef CONFIG_ARCH_RTK6748

    
	if (get_memory_size_total() != 0x60000000) {
		/*If the memory size is not 1.5GB .*/
        return ret;     
	}

    atomic_set(&rtk_adjuster_should_sleep, 1);

    spin_lock_init(&checklist_lock);

    register_trace_android_vh_update_oom_score_adj(check_foreground_process, NULL);

    adjuster = kthread_create(rtk_adjuster, NULL, "rtk_adjuster");

    if (IS_ERR(adjuster)) {
        rtd_pr_adjuster_err("Fail to create adjuster thread\n");
        ret = -1;
        goto EXIT;
    }

    rtd_pr_adjuster_info("Adjuster module loaded.\n");

    wake_up_process(adjuster);

EXIT:
    rtd_pr_adjuster_info("Enter-return");
    return ret;

#endif

    return ret;
}


static void __exit rtk_adjuster_exit(void) {
    // cancel_delayed_work_sync(&check_work);
    // destroy_workqueue(check_wq);

    rtd_pr_adjuster_warn("Adjuster module unloaded.\n");
}

module_init(rtk_adjuster_init);
module_exit(rtk_adjuster_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Realtek Semiconductor Corp., Mike Chen");
MODULE_DESCRIPTION("Process Priority Adjuster");
