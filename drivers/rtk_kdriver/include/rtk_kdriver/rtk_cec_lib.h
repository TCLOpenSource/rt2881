#ifndef __RTK_CEC_LIB_H__
#define __RTK_CEC_LIB_H__

#define CEC_OSD_NAME_MAX_LENGTH 40
#define CEC_DEVICE_TOTAL_NUM 15
typedef struct
{
    int logic_addr;
    char physical_addr[4];
    char logic_addr_name[CEC_OSD_NAME_MAX_LENGTH];
    char osd_name[CEC_OSD_NAME_MAX_LENGTH];
}CEC_DEVICE_INFO;

// for low level API
int rtk_cec_lib_get_device_num(void);
void rtk_cec_lib_get_device_list(CEC_DEVICE_INFO device_list[]);
void rtk_cec_lib_remove_device(unsigned char port);

#endif  // __RTK_CEC_LIB_H__