#ifndef __KILLER_BUTCHER_H__
#define __KILLER_BUTCHER_H__
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm.h>

enum BUTCHER_NAME {
    GPU = 0,
    KARAOKE = 1,
    USER = 30,
};

unsigned long get_butcher_state(void);
bool is_butcher(void);
void enable_butcher(enum BUTCHER_NAME name);
void disable_butcher(enum BUTCHER_NAME name);

/**
 * b_element_t - Linked list element
 * @start: address to mlock
 * @len: the size of mlock
 * @list: node of a doubly-linked list
 */
typedef struct {
    unsigned long start;
    size_t len;
    bool is_mlock;
    struct list_head list;
} b_element_t;

/**
 * b_new() - Create an empty queue whose next and prev pointer point to itself
 *
 * Return: NULL for allocation failed
 */
struct list_head *b_new(void);

/**
 * b_free() - Free all storage used by queue, no effect if header is NULL
 * @head: header of queue
 */
void b_free(struct list_head *head);

/**
 * b_insert_tail() - Insert an element at the tail
 * @head: header of queue
 * @start: the mlocked address would be inserted
 * @len: the size of mlock
 *
 * Return: true for success, false for allocation failed or queue is NULL
 */
bool b_insert_tail(struct list_head *head, unsigned long start, size_t len);

/**
 * b_remove_node() - Remove the element which has pid
 * @head: header of queue
 * @start: the magic pid for the removed
 * @len: the size of munlock
 */
void b_remove_node(struct list_head *head, unsigned long start, size_t len);

/**
 * b_traversal_unpin() - Traversal all element
 * @head: header of queue
 * @pinner: the task of pinner
 * @addr: the unpin address
 * @size: the unpin size from addr
 *
 * @return: 0 is successed to unpin files, otherwise fail
 */
int b_traversal_unpin(struct list_head *l, struct task_struct *pinner,
                            unsigned long *addr, size_t *size);

/**
 * b_traversal_repin() - Traversal all element
 * @head: header of queue
 * @pinner: the task of pinner
 * @addr: the repin address
 * @size: the repin size from addr
 *
 * @return: 0 is successed to repin files, otherwise fail
 */
int b_traversal_repin(struct list_head *l, struct task_struct *pinner,
                            unsigned long *addr, size_t *size);

/**
 * b_release_element() - Release the element
 * @e: element would be released
 *
 * This function is intended for internal use only.
 */
void b_release_element(b_element_t *e);

#endif /* __KILLER_BUTCHER_H__ */
