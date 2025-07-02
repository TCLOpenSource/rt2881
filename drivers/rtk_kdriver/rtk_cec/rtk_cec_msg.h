#ifndef __RTK_CEC_MSG_H__
#define __RTK_CEC_MSG_H__

#include <uapi/linux/cec.h>

#define CEC_OSD_NAME_MAX_LENGTH 40
#define CEC_DEVICE_TOTAL_NUM 15

typedef struct
{
    int logic_addr;
    char physical_addr[4];
    char logic_addr_name[CEC_OSD_NAME_MAX_LENGTH];
    char osd_name[CEC_OSD_NAME_MAX_LENGTH];
    int connect_st;
}cec_device_st;

void rtk_cec_get_device(cec_device_st device_list[]);

int rtk_cec_get_device_num(void);

void rtk_cec_remove_device(unsigned char port);

void rtk_cec_device_list_init(void);

void cec_msg_decode(struct cec_msg *msg,  unsigned char *mode, unsigned char *buf, unsigned int buf_len);

void cec_msg_dump(struct cec_msg *msg, unsigned char *status);

void cec_msg_set_print_enable(int opcode, unsigned char enable);

int cec_msg_get_print_enable(int opcode);

void cec_msg_clear_msg_count(int opcode);

int cec_msg_get_msg_count(int opcode);

#endif
