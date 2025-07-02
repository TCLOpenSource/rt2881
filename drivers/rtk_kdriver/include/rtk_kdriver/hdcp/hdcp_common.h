#ifndef _HDCP_COMMON
#define _HDCP_COMMON

typedef enum HDCP_OPTEE_CERT_STATUS
{
    HDCP_OPTEE_CERT_NO_DEF = 0,
    HDCP_OPTEE_CERT_OK = 1,
    HDCP_OPTEE_CERT_ERR_TEE = 2,
    HDCP_OPTEE_CERT_ERR_GENERAL = 3,
    HDCP_OPTEE_CERT_ERR_NO_KEY = 4,
    HDCP_OPTEE_CERT_ERR_CERTIFICATE_FAIL = 5,
} HDCP_OPTEE_CERT_STATUS;

typedef enum {
    HDMITX_HDCP_MODE,
    DPTX_OPTEE_HDCP_MODE,
} HDMI_DP_TX_HDCP_MODE_T;

#define HDMI_TX_PORT_MAX_NUM 1
#define DP_TX_PORT_MAX_NUM 1


#define hdmi_in(addr)                       rtd_inl(addr)
#define hdmi_out(addr, value)               rtd_outl(addr, value)
#define hdmi_mask(addr, andmask, ormask)    rtd_maskl(addr, andmask, ormask)

#define hdcp_in(addr)                       rtd_inl(addr)
#define hdcp_out(addr, value)               rtd_outl(addr, value)
#define hdcp_mask(addr, andmask, ormask)    rtd_maskl(addr, andmask, ormask)

#define hdcp_in_byte(addr)                       (rtd_inl(addr)&0XFF)
#define hdcp_in_bit(addr, bit)                   (rtd_inl(addr)&bit)
#endif
