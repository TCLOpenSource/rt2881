#include <rtk_kdriver/killer/purge.h>
#include <rtk_kdriver/killer/nts.h>
#include <rtk_kdriver/killer/butcher.h>

extern char **no_kill_list;
extern int reserved_no_kill_size;
extern pid_t warm_apps[WARM_APP_NR];
extern bool no_kill_enable;

static unsigned long purge = 0;

unsigned long get_purge_state(void)
{
    return purge;
}
EXPORT_SYMBOL(get_purge_state);

bool is_purge(void)
{
    return !!purge;
}
EXPORT_SYMBOL(is_purge);

void enable_purge(enum PURGE_NAME name)
{
    set_bit(name, &purge);
}
EXPORT_SYMBOL(enable_purge);

void disable_purge(enum PURGE_NAME name)
{
    clear_bit(name, &purge);
}
EXPORT_SYMBOL(disable_purge);

/* no kill list is created by cmdline */
static bool is_task_in_no_kill_list(char *task_name, int task_oom_score_adj, p_element_t *node)
{
    int i = 0;

    /* This no kill list is from cmdline */
    for (i = 0; i < reserved_no_kill_size; i++) {
        if (strstr(task_name, no_kill_list[i]) &&
                task_oom_score_adj <= CACHED_APP_MIN_ADJ + 15)
            return true;
    }

    if (no_kill_enable) {
        /* no kill enable for panel off (NTS) */
        for (i = 0; i < sizeof(nts_task_list) / sizeof(nts_task_list[0]); i++) {
            if (strstr(task_name, nts_task_list[i])) {
                if (CACHED_APP_MIN_ADJ <= task_oom_score_adj &&
                        task_oom_score_adj <= CACHED_APP_MIN_ADJ + 5)
                    node->prev_oom_adj = PREVIOUS_APP_ADJ;
                return true;
            }
        }
    }

    return false;
}

/* Create an empty queue */
struct list_head *p_new(void)
{
    struct list_head *head = kmalloc(sizeof(struct list_head), GFP_KERNEL);
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free the node which has pid */
void p_remove_node(struct list_head *l, pid_t pid)
{
    p_element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe(entry, safe, l, list) {
        if (entry->pid == pid) {
            list_del(&entry->list);
            p_release_element(entry);
            break;
        }
    }
    return;
}

/* Free first element which is not oom 700 used by queue */
pid_t p_remove_begin(struct list_head *l, int min_oom_score_adj)
{
    static char task_name[TASK_COMM_LEN];
    pid_t pid = 0;
    struct task_struct *task = NULL;
    p_element_t *target = NULL, *entry = NULL, *safe = NULL;
    int task_oom_score_adj = 0, task_state = 0;

    if (!l || list_empty(l))
        goto EXIT;

    if (get_butcher_state() & BIT(GPU))
        min_oom_score_adj = PERCEPTIBLE_APP_ADJ;

    list_for_each_entry_safe(entry, safe, l, list) {
        rcu_read_lock();
        task = find_task_by_vpid(entry->pid);
        if (!task) {
            rcu_read_unlock();
            list_del(&entry->list);
            p_release_element(entry);
            continue;
        } else {
            task_oom_score_adj = task->signal->oom_score_adj;
            task_state = task->__state;
            memcpy(task_name, task->comm, TASK_COMM_LEN);
            rcu_read_unlock();
        }

        if (task_state == TASK_DEAD ||
                task_state == TASK_STOPPED) {
            list_del(&entry->list);
            p_release_element(entry);
            continue;
        }

        if (!is_butcher()) {
            /* no kill home, previous, and B service tasks */
            if (task_oom_score_adj == PREVIOUS_APP_ADJ ||
                    task_oom_score_adj == HOME_APP_ADJ ||
                    task_oom_score_adj == SERVICE_B_ADJ) {
                continue;
            }

            /* no kill antepenultimate tasks */
            if ((entry->prev_oom_adj == PREVIOUS_APP_ADJ ||
                    entry->prev_oom_adj == SERVICE_B_ADJ) &&
                    task_oom_score_adj <= CACHED_APP_MIN_ADJ + 5) {
                continue;
            }

            /* no kill spicial tasks, such as NTS, CTS, etc. */
            if (is_task_in_no_kill_list(task_name, task_oom_score_adj, entry)) {
                continue;
            }
        }

        if (task_oom_score_adj > min_oom_score_adj) {
            target = entry;
            pid = task->pid;
            break;
        }
    }

    if (!target)
        goto EXIT;

    list_del(&target->list);
    p_release_element(target);
EXIT:
    return pid;
}

/* Check if the task is in queue and update previous oom_adj */
bool p_exist_element(struct list_head *l, pid_t pid, int prev_adj)
{
    p_element_t *entry = NULL;

    if (!l)
        return false;

    list_for_each_entry(entry, l, list) {
        if (pid == entry->pid) {
            entry->prev_oom_adj = prev_adj;
            return true;
        }
    }

    return false;
}

/* Free element in queue */
void p_release_element(p_element_t *e)
{
    kfree(e);
}

/* Free all storage used by queue */
void p_free(struct list_head *l)
{
    if (!l)
        return;

    while (!list_empty(l)) {
        p_element_t *target = list_first_entry(l, p_element_t, list);
        list_del(l->next);
        p_release_element(target);
    }

    kfree(l);
}

/* Insert an element at tail of queue */
bool p_insert_tail(struct list_head *head, pid_t pid, int prev_adj)
{
    p_element_t *newh = NULL;

    if (!head)
        return false;

    newh = kmalloc(sizeof(p_element_t), GFP_KERNEL);
    if (!newh)
        return false;

    newh->pid = pid;
    newh->prev_oom_adj = prev_adj;

    list_add_tail(&newh->list, head);
    return true;
}

/* Compare function for sorting */
int oom_cmp(void *priv, const struct list_head *a, const struct list_head *b)
{
    int ret = 0, oom_adj_a = 0, oom_adj_b = 0;
    struct task_struct *task_a = NULL, *task_b = NULL;
    p_element_t *node_a = NULL, *node_b = NULL;
    node_a = list_entry(a, p_element_t, list);
    node_b = list_entry(b, p_element_t, list);

    rcu_read_lock();
    task_a = find_task_by_vpid(node_a->pid);
    if (!task_a) {
        rcu_read_unlock();
        goto EXIT;
    }
    oom_adj_a = task_a->signal->oom_score_adj;
    rcu_read_unlock();

    rcu_read_lock();
    task_b = find_task_by_vpid(node_b->pid);
    if (!task_b) {
        rcu_read_unlock();
        goto EXIT;
    }
    oom_adj_b = task_b->signal->oom_score_adj;
    rcu_read_unlock();

    ret = oom_adj_a >= oom_adj_b;

EXIT:
    return ret;
}

/* Sort elements of queue in ascending order */
void p_sort(struct list_head *head)
{
    list_sort(NULL, head, oom_cmp);
}
