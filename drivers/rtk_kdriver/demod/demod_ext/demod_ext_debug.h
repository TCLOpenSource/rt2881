#ifndef _DEMOD_EXT_DEBUG_H_
#define _DEMOD_EXT_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtd_log/rtd_module_log.h>
#include <linux/proc_fs.h>
#define EXTDEMOD_PROC_DIR                        "extdemod"
#define EXTDEMOD_PROC_ENTRY                      "dbg"
#define EXTDBG_MSG_BUFFER_LENGTH                 65536

typedef enum {
	EXTDEMOD_PROC_SETLOGONOFF,
	EXTDEMOD_PROC_GETLOCK,
	EXTDEMOD_PROC_GETCROFFSET,
	EXTDEMOD_PROC_ATSONOFF,
	EXTDEMOD_PROC_COUNT,                   /* max number */
} EXTDEMOD_PROC_CMD;


typedef struct _EXT_DEMOD_DEBUG_CMD {
	const char *extdemod_cmd_str;
	EXTDEMOD_PROC_CMD proc_cmd;
} SEXT_DEMOD_DEBUG_CMD, *PS_EXT_DEMOD_DEBUG_CMD;


ssize_t extdemod_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
ssize_t extdemod_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);

#if 1
extern unsigned char extdemod_log_onoff;
//extern unsigned char ats_onoff;
#if 1
#define rtd_extdemod_print(fmt, args...)  \
{ \
	if (extdemod_log_onoff) \
	{ \
		rtd_pr_demod_print( fmt, ##args); \
	} \
}

#else
#define rtd_extdemod_print(fmt, args...)  rtd_pr_demod_print(fmt, ##args)
#endif

#define rtd_print_extdemod_force(fmt, args...)  \
{ \
		rtd_pr_demod_print( fmt, ##args); \
}

#endif

#ifdef __cplusplus
}
#endif

#endif
