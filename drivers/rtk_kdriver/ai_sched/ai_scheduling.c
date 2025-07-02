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
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/pageremap.h>
#include <linux/compat.h>
#include <rbus/timer_reg.h>
#include <rtk_kdriver/io.h>


#include <ioctrl/ai/ai_scheduling_cmd_id.h>
#include <ai_scheduling.h>


#define HIGHEST_PRIORITY 0  //realtime
#define LOWEST_PRIORITY 15
#define TOTAL_PRIORITY_LEVEL	(LOWEST_PRIORITY-HIGHEST_PRIORITY+1)
#define PRIORITY_INDEX(p)   ((p)-HIGHEST_PRIORITY)
#define GETPTS() (((s64)rtd_inl(TIMER_SCPU_CLK90K_HI_reg) << 32) | rtd_inl(TIMER_SCPU_CLK90K_LO_reg))

struct graph_desc_node_t {
	AI_SCHED_DESC desc;
	struct graph_desc_node_t* next;
};

static struct graph_desc_node_t* graph_desc;

struct graph_t {
	struct graph_t *next;
	AI_SCHED_GKID gid;
};

struct graph_node_t {
	struct graph_t* head;
	struct graph_t* tail;
};

static struct graph_node_t graphs[TOTAL_PRIORITY_LEVEL];

struct graph_attr {
	AI_SCHED_GKID gid;
	s64 expirePTS;
};

static struct graph_attr working_graph;
const static struct graph_attr idle_graph;
#define __EQUAL__(g1, g2) (g1.pid == g2.pid && g1.tid == g2.tid)
#define __NOT_EQUAL__(g1, g2) (g1.pid != g2.pid || g1.tid != g2.tid)

static u8 npu_locked;

int ai_sched_init(void)
{

	return 0;
}

void ai_sched_deinit(void)
{
	int i;
	for (i = 0; i < TOTAL_PRIORITY_LEVEL; i++) {
		while (graphs[i].head) {
			struct graph_t *next = graphs[i].head->next;
			vfree(graphs[i].head);
			graphs[i].head = next;
		}
	}

	while (graph_desc) {
		struct graph_desc_node_t* tmp = graph_desc;

		graph_desc = graph_desc->next;
		vfree(tmp);
	}
}

static int ai_sched_read_desc_from_user(unsigned long arg, AI_SCHED_DESC *desc)
{
	if (copy_from_user(desc, (void __user *)arg, sizeof(AI_SCHED_DESC)))
		return -EFAULT;

	return 0;
}

static int ai_sched_set_priority(AI_SCHED_DESC desc)
{
	struct graph_desc_node_t *node;
	u32 idx;


	idx = PRIORITY_INDEX(desc.priority);
	if (idx >= TOTAL_PRIORITY_LEVEL) {
		return -EFAULT;
	}

#if 0
	//FIXME
	if (graph_desc[desc.gid].priority != -1) {
		//remove old
	}
#endif
	node = vzalloc(sizeof(struct graph_desc_node_t));
	if (!node)
		return -ENOMEM;

	node->desc = desc;
	node->next = graph_desc;
	graph_desc = node;


	if (__EQUAL__(working_graph.gid, desc.gid)) {
		working_graph = idle_graph;
		npu_locked = 0;
	}
	//AISCHED_DEBUG("[%lld][%s] gid = (%d,%d) is added. priority=%d\n", GETPTS(), __func__, desc.gid.pid, desc.gid.tid, desc.priority);
	return 0;
}

static int ai_sched_remove_desc(AI_SCHED_GKID gid)
{
	struct graph_desc_node_t* head = graph_desc;
	struct graph_desc_node_t* prev = 0;

	while (head) {
		if (__NOT_EQUAL__(head->desc.gid, gid)) {
			prev = head;
			head = head->next;
			continue;
		}

		if (prev == 0)
			graph_desc = head->next;
		else
			prev->next = head->next;

		vfree(head);
		AISCHED_DEBUG("[%s] gid = (%d,%d) is removed\n", __func__, gid.pid, gid.tid);
		return 0;
	}
	return -EFAULT;
}

static u32 ai_sched_get_priority(AI_SCHED_GKID gid)
{
	struct graph_desc_node_t* head = graph_desc;

	while (head) {
		if (__EQUAL__(head->desc.gid, gid))
			return head->desc.priority;

		head = head->next;
		continue;
	}

	return (LOWEST_PRIORITY+1);
}

static s32 ai_sched_get_timeout(AI_SCHED_GKID gid)
{
	struct graph_desc_node_t* head = graph_desc;

	while (head) {
		if (__EQUAL__(head->desc.gid, gid))
			return head->desc.waitTime;

		head = head->next;
		continue;
	}

	return 0;
}
static int ai_sched_set_ready(AI_SCHED_GKID gid)
{
	struct graph_t *g;
	u32 priority;
	u32 idx;

	priority = ai_sched_get_priority(gid);
	idx = PRIORITY_INDEX(priority);
	if (idx >= TOTAL_PRIORITY_LEVEL)
		return -EFAULT;

	g = (struct graph_t*)vmalloc(sizeof(struct graph_t));
	if (!g)
		return -EFAULT;

	g->next = 0;
	g->gid = gid;
	if (graphs[idx].tail == 0) {
		graphs[idx].tail = graphs[idx].head = g;
	} else {
		graphs[idx].tail->next = g;
		graphs[idx].tail = g;
	}

	//AISCHED_DEBUG("[%lld][%s] ready gid = (%d,%d)\n", GETPTS(), __func__, gid.pid, gid.tid);
	return 0;
}

static int ai_sched_update_working_graph(void)
{
	int i;

	if (__NOT_EQUAL__(working_graph.gid, idle_graph.gid)) {
		s64 pts;

		if (npu_locked)
			return 0;

		pts = ((s64)rtd_inl(TIMER_SCPU_CLK90K_HI_reg) << 32) | rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		if (working_graph.expirePTS > pts)
			return 0;

		working_graph = idle_graph;
	}

	for (i = 0; i < TOTAL_PRIORITY_LEVEL; i++) {
		struct graph_t *g = graphs[i].head;
		if (g == 0)
			continue;

		if (graphs[i].head == graphs[i].tail) {
			graphs[i].head = graphs[i].tail = 0;
		} else {
			graphs[i].head = g->next;
		}
		working_graph.gid = g->gid;

		working_graph.expirePTS = ((s64)rtd_inl(TIMER_SCPU_CLK90K_HI_reg) << 32) | rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		working_graph.expirePTS += 90 * ai_sched_get_timeout(g->gid);

		//AISCHED_DEBUG("[%lld][%s] working gid = (%d,%d)\n", GETPTS(), __func__, g->gid.pid, g->gid.tid);
		vfree(g);
		return 0;
	}

	return -EFAULT;
}

static int ai_sched_peek(AI_SCHED_GKID gid)
{
	struct graph_t *g;
	u32 priority, idx;

	//check if it is the next working graph
	if (__EQUAL__(working_graph.gid, gid))
		return 0;

	//search ready list
	priority = ai_sched_get_priority(gid);
	idx = PRIORITY_INDEX(priority);
	if (idx >= TOTAL_PRIORITY_LEVEL)
		return -EFAULT;

	g = graphs[idx].head;

	while (g) {

		if (__EQUAL__(gid, g->gid))
			return 0;

		g = g->next;
	}
	return -ENOENT;
}

static int ai_sched_lock_npu(AI_SCHED_GKID gid)
{
	int ret;

	ret = ai_sched_peek(gid);
	if (ret == -ENOENT) {
		//AISCHED_DEBUG("[%lld][%s] gid = (%d,%d) not found. set ready again\n", GETPTS(), __func__, gid.pid, gid.tid);
		ret = ai_sched_set_ready(gid);
		if (ret != 0)
			return ret;
	}

	if (npu_locked)
		return -EBUSY;

	ai_sched_update_working_graph();
	if (__NOT_EQUAL__(working_graph.gid, gid))
		return -EBUSY;

	npu_locked = 1;
	//AISCHED_DEBUG("[%lld][%s] gid = (%d,%d) locks\n",GETPTS(), __func__, gid.pid, gid.tid);
	return 0;
}

static int ai_sched_unlock_npu(AI_SCHED_GKID gid)
{
	if (__NOT_EQUAL__(working_graph.gid, gid))
		return -EFAULT;


	working_graph = idle_graph;
	npu_locked = 0;
	//AISCHED_DEBUG("[%lld][%s] gid = (%d,%d) unlocks\n", GETPTS(),__func__, gid.pid, gid.tid);
	return 0;
}


int ai_sched_cancel(AI_SCHED_GKID gid)
{
	struct graph_t *g, *prev_g = 0;
	u32 priority;
	u32 idx;


	priority = ai_sched_get_priority(gid);
	idx = PRIORITY_INDEX(priority);
	if (idx >= TOTAL_PRIORITY_LEVEL)
		return -EFAULT;

	g = graphs[idx].head;

	while (g) {
		if (__NOT_EQUAL__(gid, g->gid)) {
			prev_g = g;
			g = g->next;
			continue;
		}

		if (prev_g) prev_g->next = g->next;
		if (g == graphs[idx].head) graphs[idx].head = g->next;
		if (g == graphs[idx].tail) graphs[idx].tail = prev_g;

		{
		struct graph_t *tmp = g;
		g = g->next;
		vfree(tmp);
		}
	}

	if (__EQUAL__(gid, working_graph.gid)) {
		npu_locked = 0;
		working_graph = idle_graph;
		AISCHED_WARN("[%s] gid = (%d,%d) doesn't unlock npu yet!\n", __func__, gid.pid, gid.tid);
	}

	return ai_sched_remove_desc(gid);
}



static inline long ai_sched_copy_to_user(unsigned long arg, const void* from, unsigned long n)
{
	if (copy_to_user((unsigned int __user*)arg, from, n))
		return -EFAULT;

	return 0;
}

long ai_sched_ioctl_impl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	AI_SCHED_GKID gid;

	if (cmd == AI_SCHED_IOC_GET_SPEC) {
		AI_SCHED_SPEC spec;

		spec.highestPriority = HIGHEST_PRIORITY;
		spec.lowestPriority = LOWEST_PRIORITY;
		return ai_sched_copy_to_user(arg, &spec, sizeof(spec));
	} else if (cmd == AI_SCHED_IOC_SET_PRIORITY) {
		AI_SCHED_DESC desc;

		if (ai_sched_read_desc_from_user(arg, &desc))
			return -EINVAL;

		if (filp->private_data) {
			AI_SCHED_GKID *gid = (AI_SCHED_GKID*)filp->private_data;
			*gid = desc.gid;
		}
		return ai_sched_set_priority(desc);
	}
	
	
	if (copy_from_user(&gid, (void __user *)arg, sizeof(gid)))
		return -EINVAL;

	if (cmd == AI_SCHED_IOC_SET_READY) {

		return ai_sched_set_ready(gid);
	}  else if (cmd == AI_SCHED_IOC_LOCK) {

		return ai_sched_lock_npu(gid);
	} else if (cmd == AI_SCHED_IOC_UNLOCK) {

		return ai_sched_unlock_npu(gid);
	} else if (cmd == AI_SCHED_IOC_CANCEL) {

		return ai_sched_cancel(gid);
	}
	return -EFAULT;
}

void ai_sched_show_working_gid(void)
{

	if (__NOT_EQUAL__(working_graph.gid, idle_graph.gid)) {
		s64 pts = ((s64)rtd_inl(TIMER_SCPU_CLK90K_HI_reg) << 32) | rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

		AISCHED_WARN("working gid: (%d, %d)\n", working_graph.gid.pid, working_graph.gid.tid);
		AISCHED_WARN("npu locked: %d\n", npu_locked);
		AISCHED_WARN("expire pts: %lld, now pts: %lld\n", working_graph.expirePTS, pts);
	} else
		AISCHED_WARN("working gid: n/a\n");
}

void ai_sched_show_graps(void)
{
	int i;

	for (i = 0; i < TOTAL_PRIORITY_LEVEL; i++) {
		struct graph_t *g = graphs[i].head;

		while (g) {
			AISCHED_WARN("priotiry %d : gid=(%d, %d)\n", i, g->gid.pid, g->gid.tid);
			g = g->next;
		}
	}

}


