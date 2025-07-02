#include "hdcptx_common.h"
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp2_tx.h>


extern unsigned short repeater_temp[2];
extern unsigned char sink_only_support_hdcp1[HDMI_TX_PORT_MAX_NUM];

typedef int (*HDCP_TOPOLOGY_CALLBACK)(unsigned short rx_status[2], unsigned char* p_topology_list, unsigned int topology_len);
HDCP_TOPOLOGY_CALLBACK hdcp_repeater_update_downstream_topology_callback;

void SET_HDCP_TOPOLOGY_CALLBACK(HDCP_TOPOLOGY_CALLBACK callback)
{
       hdcp_repeater_update_downstream_topology_callback  = callback;
       return;
}
EXPORT_SYMBOL(SET_HDCP_TOPOLOGY_CALLBACK);


void HdcpTxStoredReceiverId(unsigned char *pucInputArray, unsigned char ucDeviceCount)
{
    HDCPTX_WARIN("[HDCP]repeater_temp0 = %x repeater_temp1 = %x\n", repeater_temp[0], repeater_temp[1]);

    if (sink_only_support_hdcp1[0] == 1) {
        if (BSTATUS_GET_DEPTH(repeater_temp[0]) > 6) {
            repeater_temp[0] = repeater_temp[0] | BSTATUS_MAX_CASCADE_EXCEEDED(1);
            repeater_temp[1] = repeater_temp[1] | RX_INFO_MAX_CASCADE_EXCEDED;
            HDCPTX_WARIN("[HDCP] depth more than 6\n");
        }
        if (BSTATUS_GET_DEVICE_COUNT(repeater_temp[0]) > 6) {
            repeater_temp[0] = repeater_temp[0] | BSTATUS_MAX_DEVS_EXCEEDED(1);
            repeater_temp[1] = repeater_temp[1] | RX_INFO_MAX_DEVS_EXCEEDED;
            HDCPTX_WARIN("[HDCP] count more than 6\n");
        }
    }
    else {
        if (BSTATUS_GET_DEPTH(repeater_temp[0]) > 4) {
            repeater_temp[0] = repeater_temp[0] | BSTATUS_MAX_CASCADE_EXCEEDED(1);
            repeater_temp[1] = repeater_temp[1] | RX_INFO_MAX_CASCADE_EXCEDED;
            HDCPTX_WARIN("[HDCP] depth more than 4\n");
        }
        if (BSTATUS_GET_DEVICE_COUNT(repeater_temp[0]) > 31) {
            repeater_temp[0] = repeater_temp[0] | BSTATUS_MAX_DEVS_EXCEEDED(1);
            repeater_temp[1] = repeater_temp[1] | RX_INFO_MAX_DEVS_EXCEEDED;
            HDCPTX_WARIN("[HDCP] count more than 31\n");
        }
    }
#ifdef CONFIG_RTK_KDRV_HDMI_HDCP_REPEATER_ENABLE
    if(hdcp_repeater_update_downstream_topology_callback!=NULL){
        hdcp_repeater_update_downstream_topology_callback(repeater_temp, pucInputArray, ucDeviceCount * 5);
    }
#endif
}
