#include "hdmi_common.h"
#include "hdmi_cec.h"

int lib_hdmicec_get_device_num(void)
{
    int device_num = 0;
    device_num = rtk_cec_lib_get_device_num();
    return device_num;
}

void lib_hdmicec_get_device_list(CEC_DEVICE_INFO device_list[])
{
    rtk_cec_lib_get_device_list(device_list);
}

void lib_hdmicec_remove_device(unsigned char port)
{
    rtk_cec_lib_remove_device(port);
}