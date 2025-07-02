#ifndef __KILLER_QUEUE_H__
#define __KILLER_QUEUE_H__
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/string.h>
#include <linux/list_sort.h>
#include <linux/slab.h>

enum PURGE_NAME {
    KSWAPD = 0,
    GTS = 27,
    TVTS = 28,
    CTS = 29,
};

unsigned long get_purge_state(void);
bool is_purge(void);
void enable_purge(enum PURGE_NAME name);
void disable_purge(enum PURGE_NAME name);

#define CACHED_APP_MIN_ADJ                      900
#define SERVICE_B_ADJ                           800
#define PREVIOUS_APP_ADJ                        700
#define HOME_APP_ADJ                            600
#define SERVICE_ADJ                             500
#define HEAVY_WEIGHT_APP_ADJ                    400
#define BACKUP_APP_ADJ                          300
#define PERCEPTIBLE_LOW_APP_ADJ                 250
#define PERCEPTIBLE_MEDIUM_APP_ADJ              225
#define PERCEPTIBLE_APP_ADJ                     200
#define VISIBLE_APP_ADJ                         100
#define PERCEPTIBLE_RECENT_FOREGROUND_APP_ADJ    50
#define FOREGROUND_APP_ADJ                        0

#define WARM_APP_NR                               3

/**
 * p_element_t - Linked list element
 * @pid: pid of the task
 * @prev_oom_adj: Record previous oom score adj
 * @list: node of a doubly-linked list
 */
typedef struct {
    pid_t pid;
    int prev_oom_adj;
    struct list_head list;
} p_element_t;

/**
 * p_new() - Create an empty queue whose next and prev pointer point to itself
 *
 * Return: NULL for allocation failed
 */
struct list_head *p_new(void);

/**
 * p_free() - Free all storage used by queue, no effect if header is NULL
 * @head: header of queue
 */
void p_free(struct list_head *head);

/**
 * p_insert_tail() - Insert an element at the tail
 * @head: header of queue
 * @pid: pid of the task would be inserted
 * @prev_adj: the oom score adj of the task before updated
 *
 * Return: true for success, false for allocation failed or queue is NULL
 */
bool p_insert_tail(struct list_head *head, pid_t pid, int prev_adj);

/**
 * p_remove_node() - Remove the element which has pid
 * @head: header of queue
 * @pid: the magic pid for the removed task
 */
void p_remove_node(struct list_head *head, pid_t pid);

/**
 * p_remove_begin() - Remove first element at the begin
 * @head: header of queue
 * @min_oom_score_adj: this value and below oom_score_adj would not be killed
 *
 * Return: the pid of task should be killed. if 0, no task can be killed
 */
pid_t p_remove_begin(struct list_head *head, int min_oom_score_adj);

/**
 * p_exist_element() - Check if element in the queue
 * @head: header of queue
 * @pid: the pid of task want to be found
 * @prev_adj: the oom score adj of task before updated
 *
 * Return: true for the task exists in queue, otherwise is false
 */
bool p_exist_element(struct list_head *head, pid_t pid, int prev_adj);

/**
 * p_release_element() - Release the element
 * @e: element would be released
 *
 * This function is intended for internal use only.
 */
void p_release_element(p_element_t *e);

/**
 * p_oom_cmp() - Compare function for sorting
 */
int oom_cmp(void *priv, const struct list_head *a, const struct list_head *b);


/**
 * p_sort() - Sort elements of queue in ascending order
 * @head: header of queue
 *
 * No effect if queue is NULL or empty. If there has only one element, do
 * nothing.
 */
void p_sort(struct list_head *head);
#endif /* __KILLER_QUEUE_H__ */
