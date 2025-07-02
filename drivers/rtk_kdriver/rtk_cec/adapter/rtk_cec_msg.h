#ifndef __RTK_CEC_SYSFS_H__
#define __RTK_CEC_SYSFS_H__

#include "../core/rtk_cec.h"
#include "../core/rtk_cm_buff.h"

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

unsigned char *get_logic_addr_name_by_logic_addr(unsigned char logic_addr);

void cec_msg_decode(cm_buff *msg,  unsigned char *mode, unsigned char *buf, unsigned int buf_len);

void cec_msg_dump(cm_buff *msg, unsigned char *status);


#endif
