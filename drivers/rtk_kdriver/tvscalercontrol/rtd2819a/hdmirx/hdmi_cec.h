#ifndef __HDMI_CEC_H__
#define __HDMI_CEC_H__
#include <rtk_kdriver/rtk_cec_lib.h>

extern int lib_hdmicec_get_device_num(void);
extern void lib_hdmicec_get_device_list(CEC_DEVICE_INFO device_list[]);
extern void lib_hdmicec_remove_device(unsigned char port);

#endif //__HDMI_CEC_H__