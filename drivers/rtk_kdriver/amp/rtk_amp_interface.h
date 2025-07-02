#ifndef __RTK_AMP_INTERFACE_H__
#define __RTK_AMP_INTERFACE_H__
#include "alc1310.h"
#include "alc1312.h"
#include "ad82010.h"
#include "ad82120.h"
#include "ad82088.h"
#include "tas5751.h"
#include "tas5707.h"
#include "tas5711.h"
#include "tas5805m.h"
#include "wa6819.h"
#include "WA156819.h"
#include "ad82050.h"
#include "ad82088d.h"
#include "fs2105.h"
#include "acm8625.h"
#include "ad82120b.h"
#include "ntp8918.h"
#include "rtk_amp_device_id_table.h"
#include <rtk_kdriver/i2c-rtk-api.h>


#include <rtd_log/rtd_module_log.h>
#ifndef BUILD_QUICK_SHOW
#include <linux/printk.h>

#define AMP_INFO(fmt, args...)  rtd_pr_amp_debug(fmt, ## args)
#define AMP_NOTICE(fmt, args...) rtd_pr_amp_notice(fmt, ## args)
#define AMP_WARN(fmt, args...)  rtd_pr_amp_warn(fmt, ## args)
#define AMP_ERR(fmt, args...)   rtd_pr_amp_err(fmt, ## args)
#else
#include <printf.h>
#include "rtk_kdriver/i2c-rtk-api.h"
#define AMP_INFO(fmt, args...)
#define AMP_NOTICE(fmt, args...) printf2("[AMP]"fmt, ## args)
#define AMP_WARN(fmt, args...)
#define AMP_ERR(fmt, args...)   printf2("[AMP]"fmt, ## args)
#endif


#define AMP_ADDR_NULL 0
#define AMP_SEL_NULL 0

#define AMP_MUTE_ON     (1)
#define AMP_MUTE_OFF    (0)

#define ENABLE_NEW_AMP_INI_FLOW 1

struct amp_controller {
    int amp_i2c_id;
    int sel_index;
    int slave_addr;
    int addr_size;
    int data_size;
    char name[32];
    void (*ops)(int amp_i2c_id, unsigned short slave_addr);
    int (*param_get)(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr);
    int (*param_set)(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr);
    int (*mute_set)(int on_off, int amp_i2c_id, unsigned short slave_addr);
    int (*dump_all)(const char *buf, int amp_i2c_id, unsigned short slave_addr);
    void (*amp_reset)(void);
    void (*amp_device_suspend)(int);
};

#ifdef ENABLE_NEW_AMP_INI_FLOW
#define AMP_INI_SHARE_BUFF_START_ADDR 0x00175000

typedef enum {
        AMP_RESET_CMD = 0,
        AMP_QS_INIT_CMD,
        AMP_INIT_CMD,
        AMP_EQ_NONE_CMD,
        AMP_EQ_DESK_CMD,
        AMP_EQ_WALL_CMD,
        AMP_EQ_BYPASS_CMD,
} AMP_I2C_CMD;

typedef struct {
	unsigned int magic_num;//fixed to 0x475A5A47
	unsigned char support_amp_num;
	unsigned char default_pcb_amp;//0:amp from ini[R+], 1:amp from pcb[demo or amp sel 0]
	unsigned char init_stage; //1:boot,2:kernel,3:ap
	unsigned char eq_mode; //0:no need init eq,1:desk,2:wall,3:bypass
	unsigned int amp_ini_table_addr;
	unsigned int amp_ini_table_size;
	unsigned int amp_ini_real_size;
} amp_basic_info_t;



typedef struct {
	char ampModelName[32];
	unsigned char amp_addr; //8bit address
	
	unsigned int soft_reset_cmd_len;
	unsigned int soft_reset_cmd_offset;
	
	unsigned int init_qs_cmd_len;
	unsigned int init_qs_cmd_offset;
	
	unsigned int init_cmd_len;
	unsigned int init_cmd_offset;
	
	unsigned int eq_desk_cmd_len;
	unsigned int eq_desk_cmd_offset;
	
	unsigned int eq_wall_cmd_len;
	unsigned int eq_wall_cmd_offset;
	
	unsigned int eq_bypass_cmd_len;
	unsigned int eq_bypass_cmd_offset;
} amp_param_layout_t;

int is_new_amp_ini_flow(void);
int is_kernel_init_amp(void);
int get_amp_basic_info(amp_basic_info_t *basic_info);
int get_amp_model_info(unsigned char id,amp_param_layout_t *amp_layout_info);
int set_amp_eq_mode(char *eq_mode);
int do_amp_i2c_cmd(unsigned char amp_i2c_id,AMP_I2C_CMD cmdType,amp_param_layout_t amp_layout_info);
void do_ampIniOps(void);
#endif

int rtk_amp_pin_set(unsigned char *pcbname,unsigned char val);
int rtk_amp_i2c_id(void);
int amp_get_addr(char *addr_name);
#endif
