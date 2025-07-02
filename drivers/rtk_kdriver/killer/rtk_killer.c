#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/freezer.h>
#include <linux/shrinker.h>
#include <linux/mm.h>
#include <linux/mutex.h>
#include <trace/hooks/vmscan.h>
#include <trace/hooks/mm.h>
#include <trace/hooks/sched.h>

#include <rtk_kdriver/killer/purge.h>
#include <rtk_kdriver/killer/butcher.h>
#include <rtk_kdriver/killer/cts.h>
#include <rtk_kdriver/killer/gts.h>
#include <rtk_kdriver/killer/tvts.h>
#include <rtd_log/rtd_module_log.h>

#define DISABLE_TIME_CTS    (3600000)
#define DISABLE_TIME_GTS    (3600000)
#define DISABLE_TIME_TVTS   (7200000)

extern int kill_something_info(int sig, struct kernel_siginfo *info, pid_t pid);
extern char reserved_no_kill_str[64];
extern int reserved_no_kill_size;
extern bool reserved_disable_killer;

static struct task_struct *killer;
static spinlock_t kill_list_lock;
static struct list_head *kill_task_list;

static struct timer_list tvts_timer;
static struct timer_list cts_timer;
static struct timer_list gts_timer;

static struct mutex pin_list_lock;
static struct list_head *pin_file_list;
static struct task_struct *pinner = NULL;

static int sleep_time_ms = 100;
module_param(sleep_time_ms, int, S_IWUSR|S_IRUGO);

static int min_oom_score_adj = SERVICE_ADJ;
module_param(min_oom_score_adj, int, S_IWUSR|S_IRUGO);

static bool user_enable_butcher = false;
module_param(user_enable_butcher, bool, S_IWUSR|S_IRUGO);

bool no_kill_enable = false;
module_param(no_kill_enable, bool, S_IWUSR|S_IRUGO);
EXPORT_SYMBOL(no_kill_enable);

char **no_kill_list = NULL;
EXPORT_SYMBOL(no_kill_list);

static void disable_purge_tvts(struct timer_list *t)
{
    rtd_pr_killer_notice("Enable killer after 2 hours due to TVTS\n");
    disable_purge(TVTS);
}

static void disable_purge_cts(struct timer_list *t)
{
    rtd_pr_killer_notice("Enable killer after 1 hours due to CTS\n");
    disable_purge(CTS);
}

static void disable_purge_gts(struct timer_list *t)
{
    rtd_pr_killer_notice("Enable killer after 1 hours due to GTS\n");
    disable_purge(GTS);
}

/**
 * Check the task if CTS, GTS, TVTS tasks or not
 *
 * @task: The task would be changed name
 * @buf: This is the task name of the APK
 */
static void rtk_check_task_comm(void *unused, struct task_struct *task, const char *buf)
{
    if (task->flags & PF_KTHREAD)
        return;

    if (!buf || !strlen(buf))
        return;

    if (task->signal->oom_score_adj == FOREGROUND_APP_ADJ &&
                             !(get_purge_state() & BIT(CTS)) &&
                             should_disable_kill_cts_task(buf)) {
        rtd_pr_killer_notice("Disable killer 1 hours due to CTS (%s)\n", buf);
        mod_timer(&cts_timer, jiffies + msecs_to_jiffies(DISABLE_TIME_CTS));
        enable_purge(CTS);
    }
    
    if (task->signal->oom_score_adj == FOREGROUND_APP_ADJ &&
                             !(get_purge_state() & BIT(GTS)) &&
                             should_disable_kill_gts_task(buf)) {
        rtd_pr_killer_notice("Disable killer 1 hours due to GTS (%s)\n", buf);
        mod_timer(&gts_timer, jiffies + msecs_to_jiffies(DISABLE_TIME_GTS));
        enable_purge(GTS);
    }

    if (!(get_purge_state() & BIT(TVTS)) && is_tvts_task(buf)) {
        rtd_pr_killer_notice("Do not kill specific tasks 2 hours due to TVTS (%s)\n", buf);
        mod_timer(&tvts_timer, jiffies + msecs_to_jiffies(DISABLE_TIME_TVTS));
        enable_purge(TVTS);
    }

    return;
}

/**
  * Tasks are added to kill_task_list by this function
  *
  * @task: The oom_score_adj of the task is not updated
  * @oom_adj: The new oom_score_adj for the task
 **/
static void rtk_update_kill_list(void *unused, struct task_struct *task, int oom_adj)
{
    if (!task)
        return;

    if (task->tgid != task->pid) {
        rtd_pr_killer_warn("Only thead group id can enter kill list\n");
        return;
    }

    if (oom_adj < FOREGROUND_APP_ADJ) {
        rtd_pr_killer_warn("Avoid %s with oom_score_adj(%d) to enter kill list\n",
                                                                task->comm, oom_adj);
        return;
    }

    spin_lock(&kill_list_lock);
    if(!p_exist_element(kill_task_list, task->pid, task->signal->oom_score_adj)) {
        if (!p_insert_tail(kill_task_list, task->pid, task->signal->oom_score_adj)) {
            rtd_pr_killer_err("p_insert_tail fail.\n");
            spin_unlock(&kill_list_lock);
            return;
        }
    }

    rtd_pr_killer_notice("%s(%d) update oom_adj (%d -> %d)\n",
            task->comm, task->pid, task->signal->oom_score_adj, oom_adj);

    task->signal->oom_score_adj = oom_adj;
    p_sort(kill_task_list);
    spin_unlock(&kill_list_lock);
}

static void rtk_try_to_kill_task(void)
{
    static char task_name[NAME_MAX];
    pid_t pid = 0;
    int oom_score_adj = 0;
    struct task_struct *task = NULL;
    unsigned long start = 0;
    size_t len = 0;

    if (freezing(current) || kthread_should_stop())
        return;

try_to_kill:
    while (get_purge_state() & BIT(CTS))
        schedule();

    spin_lock(&kill_list_lock);
    pid = p_remove_begin(kill_task_list, min_oom_score_adj);
    spin_unlock(&kill_list_lock);
    if (pid) {
        rcu_read_lock();
        task = find_task_by_vpid(pid);
        if (task) {
            oom_score_adj = task->signal->oom_score_adj;
            /* APK Name is from cmdline in Android System */
            if (!get_cmdline(task, task_name, NAME_MAX)) {
                rcu_read_unlock();
                goto try_to_kill;
            }
        } else {
            rcu_read_unlock();
            goto try_to_kill;
        }
        rcu_read_unlock();

        if (is_cts_task(task_name) ||
                   is_gts_task(task_name) ||
                   ((get_purge_state() & BIT(TVTS)) &&
                   should_not_kill_tvts_task(task_name) &&
                   oom_score_adj <= CACHED_APP_MIN_ADJ + 15)) {
            spin_lock(&kill_list_lock);
            p_remove_node(kill_task_list, pid);
            spin_unlock(&kill_list_lock);
            rtd_pr_killer_notice("Ignore task :%s (%d)\n", task_name, pid);
            goto try_to_kill;
        }

        if (is_butcher()) {
            rtd_pr_killer_notice("[Butcher](0x%08x) Kill %s(%d), oom score adj %d\n",
                                    get_butcher_state(), task_name, pid, oom_score_adj);

            /* unpin files when butcher mode */
            mutex_lock(&pin_list_lock);
            while (!b_traversal_unpin(pin_file_list, pinner, &start, &len))
                rtd_pr_killer_notice("[Butcher](0x%08x) Unpin 0x%lx@0x%lx\n",
                                    get_butcher_state(), start, len);
            mutex_unlock(&pin_list_lock);
        } else {
            rtd_pr_killer_notice("(0x%08x)Kill %s(%d), oom score adj %d\n",
                                    get_purge_state(), task_name, pid, oom_score_adj);

            /* repin files when leaving butcher mode */
            mutex_lock(&pin_list_lock);
            while (!b_traversal_repin(pin_file_list, pinner, &start, &len))
                rtd_pr_killer_notice("(0x%08x)Repin 0x%lx@0x%lx\n", get_purge_state(), start, len);
            mutex_unlock(&pin_list_lock);
        }

        kill_something_info(SIGKILL, SEND_SIG_PRIV, pid);

        goto try_to_kill;
    } else {
        rtd_pr_killer_debug("No killable task in queue\n");
    }
    return;
}

static int rtk_killer(void *arg)
{
    set_freezable();

    while (!kthread_should_stop()) {
        if (!is_purge())
            schedule();

        if (user_enable_butcher)
            enable_butcher(USER);
        else
            disable_butcher(USER);

        rtk_try_to_kill_task();
        try_to_freeze();
        msleep(sleep_time_ms);
    }

    return 0;
}

static void wakeup_rtk_killer(void *_unused, void *unused)
{
    enable_purge(KSWAPD);
    wake_up_process(killer);
}

static void sleep_rtk_killer(void *_unused, void *unused)
{
    disable_purge(KSWAPD);
}

static void parse_no_kill_list(void)
{
    int i = 0, j = 0;

    no_kill_list = kmalloc(reserved_no_kill_size * sizeof(char *), GFP_KERNEL);

    for (i = 0; i < reserved_no_kill_size; i++, j++) {
        char name[16];
        int name_size = 0;

        while (reserved_no_kill_str[j] - 'a' >= 0 && 'z' - 'a' >= reserved_no_kill_str[j] - 'a')
            name[name_size++] = reserved_no_kill_str[j++];

        if (name_size == 0)
            continue;

        no_kill_list[i] = kmalloc((name_size + 1) * sizeof(char), GFP_KERNEL);
        no_kill_list[i][name_size] = '\0';

        while(name_size--)
            no_kill_list[i][name_size] = name[name_size];

        rtd_pr_killer_notice("%s is added to non-kill list\n", no_kill_list[i]);
    }
}

static void rtk_remove_pin_files(void *_unused, unsigned long start, size_t len)
{
    mutex_lock(&pin_list_lock);
    b_remove_node(pin_file_list, start, len);
    mutex_unlock(&pin_list_lock);
    return;
}

static void rtk_add_pin_files(void *_unused, unsigned long start, size_t len)
{
    /* Android Pinner Service uses "android.bg" to mlock files */
    if (strcmp(current->comm, "android.bg"))
        return;

    /* Need to use vma info of this task to unpin */
    if (!pinner)
        pinner = current;

    mutex_lock(&pin_list_lock);
    if(!b_insert_tail(pin_file_list, start, len)) {
        rtd_pr_killer_err("b_insert_tail fail.\n");
    } else {
        rtd_pr_killer_notice("0x%lx@0x%lx add to pin-files list\n", start, len);
    }
    mutex_unlock(&pin_list_lock);

    return;
}

/* TODO - using shrink to design unpin flow */
/* Only unpin files when enter in butcher mode */
// static unsigned long unpin_shrink_count(struct shrinker *s, struct shrink_control *sc)
// {
//     return get_butcher_state();
// }
// 
// static unsigned long unpin_shrink(struct shrinker *s, struct shrink_control *sc)
// {
//     spin_lock(&pin_list_lock);
//     b_traversal_unpin(pin_file_list, pinner);
//     spin_unlock(&pin_list_lock);
//     return SHRINK_STOP;
// }
// 
// static struct shrinker unpin_shrinker = {
//     .count_objects = unpin_shrink_count,
//     .scan_objects = unpin_shrink,
//     .seeks = DEFAULT_SEEKS * 4,
// };

static int __init rtk_killer_init(void)
{
    int ret = 0;

    if (reserved_disable_killer) {
        rtd_pr_killer_notice("Disable RTK Killer via bootcode cmdline\n");
        goto ERR;
    }

    spin_lock_init(&kill_list_lock);
    mutex_init(&pin_list_lock);

    // ret = register_shrinker(&unpin_shrinker);
    // if (ret) {
    //     rtd_pr_killer_err("Fail to register unpin_shrinker (ret %d)\n", ret);
    //     goto ERR;
    // }

    register_trace_android_vh_wakeup_kswapd(wakeup_rtk_killer, NULL);
    register_trace_android_vh_kswapd_sleep(sleep_rtk_killer, NULL);
    register_trace_android_vh_write_oom_score_adj(rtk_update_kill_list, NULL);
    register_trace_android_vh_task_rename(rtk_check_task_comm, NULL);
    register_trace_android_vh_record_mlock_files(rtk_add_pin_files, NULL);
    register_trace_android_vh_record_munlock_files(rtk_remove_pin_files, NULL);

    kill_task_list = p_new();
    if (!kill_task_list) {
        rtd_pr_killer_err("Fail to create task list\n");
        ret = -ENOMEM;
        goto ERR;
    }

    pin_file_list = b_new();
    if (!pin_file_list) {
        rtd_pr_killer_err("Fail to create pin list\n");
        ret = -ENOMEM;
        goto ERR;
    }

    if (reserved_no_kill_size)
        parse_no_kill_list();

    timer_setup(&tvts_timer, disable_purge_tvts, 0);
    timer_setup(&cts_timer, disable_purge_cts, 0);
    timer_setup(&gts_timer, disable_purge_gts, 0);
    tvts_timer.expires = 0;
    cts_timer.expires = 0;
    gts_timer.expires = 0;
    add_timer(&tvts_timer);
    add_timer(&cts_timer);
    add_timer(&gts_timer);

    killer = kthread_create(rtk_killer, NULL, "rtk_killer");
    if (IS_ERR(killer)) {
        rtd_pr_killer_err("Fail to create killer thread\n");
        ret = -ENOMEM;
        goto ERR;
    }
    wake_up_process(killer);

    return ret;

ERR:
    if (kill_task_list)
        p_free(kill_task_list);

    if (pin_file_list)
        b_free(pin_file_list);

    return ret;
}

static void __exit rtk_killer_exit(void)
{
    p_free(kill_task_list);
    b_free(pin_file_list);
    // unregister_shrinker(&unpin_shrinker);
}

module_init(rtk_killer_init);
module_exit(rtk_killer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Realtek Semiconductor Corp., Kenny Cheng");
MODULE_DESCRIPTION("Process Killer");
