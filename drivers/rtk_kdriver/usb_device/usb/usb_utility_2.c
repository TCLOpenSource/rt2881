#include <linux/string.h>
#include "msg_q_function_api.h"
#include "rtk_io.h"
#include "usb_mac.h"
#include "usb_utility.h"
#include "usb_utility_config_bank37.h"
#include "usb_buf_mgr.h"
#include <linux/timer.h>
#include "msg_queue_ap_def.h"
#include "rtk_ice-at_common.h"
#include "rtk_ice-at.h"
#include "rtk_ice-at_customer.h"
//#define CONFIG_USB_USE_DEFAULT_DESCRIPTION

#ifdef CONFIG_RTK_KDRV_RTICE_AT_COMMAND
#endif
//#include "sys_dmem.h"

#include "user_data.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK43);
//PRAGMA_CONSTSEG(BANK43);

extern UINT8 uvc_timing_table_index;
extern AT_USB_USER_DATA_T *p_usb_user_data;
extern struct dwc3 g_rtk_dwc3;

UINT32 AT_Set_USB_decription_Index(void)
{
    // TODO Write Serial number to flash
    if(p_usb_user_data->length != 1)
        return 0;

    if(uvc_timing_table_index != p_usb_user_data->data[0]) {
        uvc_timing_table_index = p_usb_user_data->data[0];
        g_rtk_dwc3.dwc_need_reconnect = 1;
    }

    //pm_printk(LOGGER_ERROR, "AT_USB_Set_ForceSpeed:%x\n", (UINT32)p_usb_user_data->data[0]);

    return 0;
}

void AT_Set_USB_decription_Index_cb(void) __banked
{
    //__xdata UINT8 *const cmd = AT_ACQUIRE_R_TPDU();

    // 0. the cmd[0] is 0xa0, means the 0xA0 AT command TPDU.
    /* here to handle the input argument from TPDU cmd.
     * a) please keep cmd[0] constant,
     * b) cmd_index is A0_TPDU_CMD_INDEX(cmd), argument data is A0_TPDU_DATA(cmd)
     */
    //pm_printk(LOGGER_ERROR, "%s size: %d \n", __func__, (UINT32)sizeof(AT_USB_USER_DATA_T));

    // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
    p_usb_user_data = (AT_USB_USER_DATA_T *) AT_TPDU_DATA(AT_ACQUIRE_TPDU());

    AT_Set_USB_decription_Index();
    rtice_pure_ack(AT_ACQUIRE_R_TPDU());
    return;
}

void AT_Get_USB_decription_Index(void)
{
    // TODO Read Serial number from flash
    memset(p_usb_user_data->data, 0, 128);
    p_usb_user_data->length = 1;
    p_usb_user_data->data[0] = uvc_timing_table_index;
    return ;
}

void AT_Get_USB_decription_Index_cb(void) __banked
{
    //__xdata UINT8 *const cmd = AT_ACQUIRE_R_TPDU();

    // 0. the cmd[0] is 0xa0, means the 0xA0 AT command TPDU.
    /* here to handle the input argument from TPDU cmd.
     * a) please keep cmd[0] constant,
     * b) cmd_index is A0_TPDU_CMD_INDEX(cmd), argument data is A0_TPDU_DATA(cmd)
    */

    // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
    p_usb_user_data = (AT_USB_USER_DATA_T *) AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
    AT_Get_USB_decription_Index();

    // 2. response the CMD with data SIZE
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(AT_USB_USER_DATA_T)/*data_size*/);

    return;
}
