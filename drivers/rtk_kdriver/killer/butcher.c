#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/export.h>
#include <linux/mmap_lock.h>

#include <rtk_kdriver/killer/butcher.h>
#include <rtd_log/rtd_module_log.h>

extern int do_pin_mlock(unsigned long start, size_t len, vm_flags_t flags,
                               struct task_struct *pinner);
extern int apply_pin_vma_lock_flags(unsigned long start, size_t len,
                               vm_flags_t flags, struct task_struct *pinner);

static unsigned long butcher = 0;

unsigned long get_butcher_state(void)
{
    return butcher;
}
EXPORT_SYMBOL(get_butcher_state);

bool is_butcher(void)
{
    return !!butcher;
}
EXPORT_SYMBOL(is_butcher);

void enable_butcher(enum BUTCHER_NAME name)
{
    set_bit(name, &butcher);
}
EXPORT_SYMBOL(enable_butcher);

void disable_butcher(enum BUTCHER_NAME name)
{
    clear_bit(name, &butcher);
}
EXPORT_SYMBOL(disable_butcher);

/* Create an empty queue */
struct list_head *b_new(void)
{
    struct list_head *head = kmalloc(sizeof(struct list_head), GFP_KERNEL);
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void b_free(struct list_head *l)
{
    if (!l)
        return;

    while (!list_empty(l)) {
        b_element_t *target = list_first_entry(l, b_element_t, list);
        list_del(l->next);
        b_release_element(target);
    }

    kfree(l);
}

/* Insert an element at tail of queue */
bool b_insert_tail(struct list_head *head, unsigned long start, size_t len)
{
    b_element_t *newh = NULL;

    if (!head)
        return false;

    newh = kmalloc(sizeof(b_element_t), GFP_KERNEL);
    if (!newh)
        return false;

    newh->start = start;
    newh->len = len;
    newh->is_mlock = true;

    list_add_tail(&newh->list, head);
    return true;
}

/* Free the node which has pid */
void b_remove_node(struct list_head *l, unsigned long start, size_t len)
{
    b_element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe(entry, safe, l, list) {
        if (entry->start == start && entry->len == len) {
            list_del(&entry->list);
            b_release_element(entry);
            break;
        }
    }
    return;
}

int b_traversal_unpin(struct list_head *l, struct task_struct *pinner,
                            unsigned long *addr, size_t *size)
{
    int ret = -1;
    b_element_t *entry = NULL;
    unsigned long start = 0;
    size_t len = 0;

    list_for_each_entry(entry, l, list) {
        if (!entry->is_mlock)
            continue;

        start = entry->start;
        len = entry->len;

        start = untagged_addr(start);

        len = PAGE_ALIGN(len + (offset_in_page(start)));
        start &= PAGE_MASK;

        if (!mmap_write_lock_killable(pinner->mm)) {
            ret = apply_pin_vma_lock_flags(start, len, 0, pinner);
            mmap_write_unlock(pinner->mm);

            if (!ret) {
                entry->is_mlock = false;
                *addr = entry->start;
                *size = entry->len;
                break;
            }
        }
    }

    return ret;
}

int b_traversal_repin(struct list_head *l, struct task_struct *pinner,
                            unsigned long *addr, size_t *size)
{
    int ret = -1;
    b_element_t *entry = NULL;
    unsigned long start = 0;
    size_t len = 0;

    list_for_each_entry(entry, l, list) {
        if (entry->is_mlock)
            continue;

        start = entry->start;
        len = entry->len;

        ret = do_pin_mlock(start, len, VM_LOCKED, pinner);
        if(!ret) {
            entry->is_mlock = true;
            *addr = entry->start;
            *size = entry->len;
            break;
        }
    }

    return ret;
}

/* Free element in queue */
void b_release_element(b_element_t *e)
{
    kfree(e);
}

MODULE_LICENSE("GPL");
