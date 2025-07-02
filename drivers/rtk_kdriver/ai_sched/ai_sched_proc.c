#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/version.h>
#include <ai_scheduling.h>


enum {
	AI_SCHED_CMD_SHOW_ALL = 0,

	AI_SCHED_CMD_NUM
} ;

static const char* cmd_str[] = {
	"show_all", 		 /*AI_SCHED_CMD_SHOW_ALL*/
};


static void ai_sched_exec_cmd(int cmd_id, char* value_ptr, int value_size)
{

	AISCHED_WARN("[%s %d]cmd_id = %d, value = %s\n", __func__, __LINE__, cmd_id, value_ptr);

	switch(cmd_id) {
	case AI_SCHED_CMD_SHOW_ALL:
		ai_sched_show_working_gid();
		ai_sched_show_graps();
		return;

	default:
		break;
	}

}

static int ai_sched_read_cmd(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos, char cmd_value[], size_t cmd_value_size)
{
#define str_size 256
	char str[str_size];
	int cmd_id;

	if (buf == 0) {
		AISCHED_ERROR("[%s %d]invalid argument\n", __func__, __LINE__);
		return AI_SCHED_CMD_NUM;
	}

	if (count > str_size-1)
		count = str_size-1;

	if (copy_from_user(str, buf, count)) {
		AISCHED_ERROR("[%s %d]copy cmd fail\n", __func__, __LINE__);
		return AI_SCHED_CMD_NUM;
	}
	if (count > 0)
		str[count-1] = '\0';

	for (cmd_id = 0; cmd_id < AI_SCHED_CMD_NUM; cmd_id++) {
		if (strncmp(str, cmd_str[cmd_id], strlen(cmd_str[cmd_id])) == 0)
			break;
	}

	if (cmd_id == AI_SCHED_CMD_NUM)
		return AI_SCHED_CMD_NUM;

	if ((count-strlen(cmd_str[cmd_id])) >= cmd_value_size)
		return AI_SCHED_CMD_NUM;

	memcpy((void *)cmd_value,(void *)&str[strlen(cmd_str[cmd_id])],count-strlen(cmd_str[cmd_id]));


	return cmd_id;
}
ssize_t ai_sched_proc(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
#define cmd_value_size 256
	char cmd_value[cmd_value_size] = {};
	int cmd_id;

	cmd_id = ai_sched_read_cmd(filp, buf, count, f_pos, cmd_value, cmd_value_size-1);
	ai_sched_exec_cmd(cmd_id, cmd_value, cmd_value_size);

	return count;
}
