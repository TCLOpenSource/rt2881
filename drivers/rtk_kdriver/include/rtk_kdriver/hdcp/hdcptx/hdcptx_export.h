#ifndef __HDCPTX_EXPORT_H__
#define __HDCPTX_EXPORT_H__

#if 0
typedef enum {
    HDMITX_HDCP_MODE,
    DPTX_OPTEE_HDCP_MODE,
} HDMI_DP_TX_HDCP_MODE_T;
#endif

typedef int (*HDCP_TOPOLOGY_CALLBACK)(unsigned short rx_status[2], unsigned char* p_topology_list, unsigned int topology_len);

extern void ScalerHdmiHdcp2Tx2Handler(unsigned char port);
extern void ScalerHdmiHdcp2TxSetAuthTimeOut(unsigned char port);
extern unsigned char ScalerHdmiHdcp2TxGetAuthTimeOut(unsigned char port);
extern void ScalerHdmiHdcp2TxClearAuthState(unsigned char port);
extern void ScalerSetHdmiHdcp2Tx2Auth(unsigned char port, unsigned char enable);
extern void ScalerHdmiHdcp2Tx2ChangeAuthState(unsigned char port, unsigned char enumHdcp2AuthState);
extern unsigned char ScalerHdmiHdcp2Tx2CheckCapability(unsigned char port);

//rx control
extern void SET_HDCP_TX_VFreq(unsigned char port, unsigned int vfreq);
extern unsigned int GET_HDCP_TX_VFreq(unsigned char port);
extern void SET_HDMIRX_HDCP_TYPE(unsigned char type);
extern unsigned char GET_HDMIRX_HDCP_TYPE(void);
extern void SET_DPRX_HDCP_TYPE(unsigned char type);
extern unsigned char GET_DPRX_HDCP_TYPE(void);
extern void SET_HDCP_TOPOLOGY_CALLBACK(HDCP_TOPOLOGY_CALLBACK callback);
extern void SET_HDMI_MODE_TYPE(unsigned char type);
extern unsigned char GET_HDMI_MODE_TYPE(void);
//tx control
extern void SET_HDMI_HDCPTX_RUN(unsigned char port, unsigned char value);
extern unsigned char GET_HDMI_HDCPTX_RUN(unsigned char port);
extern unsigned char GET_HDCPTX_ONLY_SUPPORT_HDCP1(unsigned char port);
extern void set_hdmitx_connect_status(unsigned char port, unsigned char enable);
extern unsigned char get_hdmitx_connect_status(unsigned char port);
extern void HDMITX_HDCP_Set_AuthVersion(unsigned char hdmitx_port,unsigned char HdcpVer);
extern void hdmitx_load_hdcptx_key(void);
#endif
