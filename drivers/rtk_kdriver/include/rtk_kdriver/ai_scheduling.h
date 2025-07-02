#ifndef __AI_SCHEDULING_DRV_H_
#define __AI_SCHEDULING_DRV_H_

#include <rtd_log/rtd_module_log.h>
#include <ioctrl/ai/ai_scheduling_cmd_id.h>

#define AISCHED_DEBUG(fmt, args...)  rtd_pr_ai_dbg_notice("[ai_sched]"fmt, ## args)
#define AISCHED_INFO(fmt, args...)   rtd_pr_ai_dbg_info("[ai_sched]" fmt, ## args)
#define AISCHED_WARN(fmt, args...)   rtd_pr_ai_dbg_warn("[ai_sched]" fmt, ## args)
#define AISCHED_ERROR(fmt, args...)  rtd_pr_ai_dbg_err("[ai_sched]" fmt, ## args)

int ai_sched_init(void);
int ai_sched_cancel(AI_SCHED_GKID gid);
void ai_sched_deinit(void);
long ai_sched_ioctl_impl(struct file *filp, unsigned int cmd, unsigned long arg);
ssize_t ai_sched_proc(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
void ai_sched_show_working_gid(void);
void ai_sched_show_graps(void);


#endif //__AI_SCHEDULING_DRV_H_