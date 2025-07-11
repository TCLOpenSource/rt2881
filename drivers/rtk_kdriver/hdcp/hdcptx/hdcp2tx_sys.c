/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdcp2TxTest.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp2_tx.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <hdcp/hdcptx/hdcptx_export.h>
#include "hdcptx_common.h"

unsigned char hdcp2tx_func_test = 0;
unsigned char hdcp2tx_read_log_print = 0;
unsigned char hdcp2tx_cal_log_print = 0;

//****************************************************************************
// CODE TABLES
//****************************************************************************
unsigned char tx_rtx_test[8] = {0x18, 0xfa, 0xe4, 0x20, 0x6a, 0xfb, 0x51, 0x49};
unsigned char tx_seed_test[32] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
    0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

unsigned char tx_km_test[16] = {
    0x68, 0xbc, 0xc5, 0x1b, 0xa9, 0xdb, 0x1b, 0xd0,
    0xfa, 0xf1, 0x5e, 0x9a, 0xd8, 0xa5, 0xaf, 0xb9
};


unsigned char pucMessage_AKE_SEND_H_PRIME[33] = {
    0x07,
    0x69, 0xe0, 0xab, 0x21, 0x2f, 0xdb, 0x57, 0xe6, 0x7e, 0xfc, 0x43, 0x76, 0x1a, 0x2c, 0x5c, 0xce,
    0x76, 0xc3, 0x65, 0xf1, 0x9b, 0x75, 0xc3, 0xea, 0xc2, 0xd2, 0x77, 0xdd, 0x5c, 0x7e, 0x4a, 0xc4,
};

unsigned char pucMessage_AKE_SEND_PAIRING_INFO[17] = {
    0x08,
    0xb8, 0x9f, 0xf9, 0x72, 0x6a, 0x6f, 0x2c, 0x1e, 0x29, 0xb6, 0x44, 0x8d, 0xdc, 0xa3, 0x10, 0xbd,
};

unsigned char tx_rn_test[8] =
{0x32, 0x75, 0x3e, 0xa8, 0x78, 0xa6, 0x38, 0x1c};

unsigned char pucMessage_LC_SEND_L_PRIME[33] = {
    0x0a,
    0xbc, 0x20, 0x92, 0x33, 0x54, 0x91, 0xc1, 0x9e, 0xa4, 0xde, 0x8b, 0x30, 0x49, 0xc2, 0x06, 0x6a,
    0xd8, 0x11, 0xa2, 0x2a, 0xb1, 0x46, 0xdf, 0x74, 0x58, 0x47, 0x05, 0xa8, 0xb7, 0x67, 0xfb, 0xdd,
};

unsigned char pucMessage_REPEATERAUTH_SEND_RECEIVERID_LIST[37] = {
    0x0c, 0x02, 0x31, 0x00, 0x00, 0x00, 0xbc, 0xcc,
    0x7d, 0x16, 0xe6, 0xbc, 0xb9, 0x02, 0x60, 0x08,
    0x1d, 0xf7, 0x4a, 0xb4, 0x5c, 0x8a, 0x47, 0x8e,
    0x71, 0xe2, 0x0f, 0x35, 0x79, 0x6a, 0x17, 0x0e,
    0x74, 0xe8, 0x53, 0x97, 0xa2,
};

unsigned char pucMessage_REPEATERAUTH_STREAM_READY[33] = {
    0x11, 0xdd, 0x26, 0xe9, 0x52, 0x6e, 0x0e, 0x1d, 0x69, 0xc8,
    0x84, 0xe4, 0xcc, 0xc8, 0x09, 0xaa, 0xc7, 0x71, 0xe9, 0x97,
    0xb5, 0x61, 0x89, 0x09, 0x6e, 0x4d, 0x94, 0x24, 0xc2, 0x1b,
    0x64, 0x58, 0xc6,
};

unsigned char tx_ks_test[16] = {
    0xf3, 0xdf, 0x1d, 0xd9, 0x57, 0x96, 0x12, 0x3f,
    0x98, 0x97, 0x89, 0xb4, 0x21, 0xe1, 0x2d, 0xe1,
};

unsigned char tx_riv_test[8] = {
    0x40, 0x2b, 0x6b, 0x43, 0xc5, 0xe8, 0x86, 0xd8
};

unsigned char  rx_rrx_test[8] = {0x3b, 0xa0, 0xbe, 0xde, 0x0c, 0x46, 0xa9, 0x91};
unsigned char rx_rxcaps_test[3] = {0x02, 0x00, 0x01};
unsigned char rx_cert_key_test[522] = {
    0x74, 0x5b, 0xb8, 0xbd, 0x04, 0xaf,
    0xb5, 0xc5, 0xc6, 0x7b, 0xc5, 0x3a, 0x34, 0x90,
    0xa9, 0x54, 0xc0, 0x8f, 0xb7, 0xeb, 0xa1, 0x54, 0xd2, 0x4f, 0x22, 0xde, 0x83, 0xf5,
    0x03, 0xa6, 0xc6, 0x68, 0x46, 0x9b, 0xc0, 0xb8, 0xc8, 0x6c, 0xdb, 0x26, 0xf9, 0x3c,
    0x49, 0x2f, 0x02, 0xe1, 0x71, 0xdf, 0x4e, 0xf3, 0x0e, 0xc8, 0xbf, 0x22, 0x9d, 0x04,
    0xcf, 0xbf, 0xa9, 0x0d, 0xff, 0x68, 0xab, 0x05, 0x6f, 0x1f, 0x12, 0x8a, 0x68, 0x62,
    0xeb, 0xfe, 0xc9, 0xea, 0x9f, 0xa7, 0xfb, 0x8c, 0xba, 0xb1, 0xbd, 0x65, 0xac, 0x35,
    0x9c, 0xa0, 0x33, 0xb1, 0xdd, 0xa6, 0x05, 0x36, 0xaf, 0x00, 0xa2, 0x7f, 0xbc, 0x07,
    0xb2, 0xdd, 0xb5, 0xcc, 0x57, 0x5c, 0xdc, 0xc0, 0x95, 0x50, 0xe5, 0xff, 0x1f, 0x20,
    0xdb, 0x59, 0x46, 0xfa, 0x47, 0xc4, 0xed, 0x12, 0x2e, 0x9e, 0x22, 0xbd, 0x95, 0xa9,
    0x85, 0x59, 0xa1, 0x59, 0x3c, 0xc7, 0x83,  //n
    0x01, 0x00, 0x01,   //e
    0x10, 0x00, 0x0b, 0xa3,
    0x73, 0x77, 0xdd, 0x03, 0x18, 0x03, 0x8a, 0x91, 0x63, 0x29, 0x1e, 0xa2, 0x95, 0x74,
    0x42, 0x90, 0x78, 0xd0, 0x67, 0x25, 0xb6, 0x32, 0x2f, 0xcc, 0x23, 0x2b, 0xad, 0x21,
    0x39, 0x3d, 0x14, 0xba, 0x37, 0xa3, 0x65, 0x14, 0x6b, 0x9c, 0xcf, 0x61, 0x20, 0x44,
    0xa1, 0x07, 0xbb, 0xcf, 0xc3, 0x4e, 0x95, 0x5b, 0x10, 0xcf, 0xc7, 0x6f, 0xf1, 0xc3,
    0x53, 0x7c, 0x63, 0xa1, 0x8c, 0xb2, 0xe8, 0xab, 0x2e, 0x96, 0x97, 0xc3, 0x83, 0x99,
    0x70, 0xd3, 0xdc, 0x21, 0x41, 0xf6, 0x0a, 0xd1, 0x1a, 0xee, 0xf4, 0xcc, 0xeb, 0xfb,
    0xa6, 0xaa, 0xb6, 0x9a, 0xaf, 0x1d, 0x16, 0x5e, 0xe2, 0x83, 0xa0, 0x4a, 0x41, 0xf6,
    0x7b, 0x07, 0xbf, 0x47, 0x85, 0x28, 0x6c, 0xa0, 0x77, 0xa6, 0xa3, 0xd7, 0x85, 0xa5,
    0xc4, 0xa7, 0xe7, 0x6e, 0xb5, 0x1f, 0x40, 0x72, 0x97, 0xfe, 0xc4, 0x81, 0x23, 0xa0,
    0xc2, 0x90, 0xb3, 0x49, 0x24, 0xf5, 0xb7, 0x90, 0x2c, 0xbf, 0xfe, 0x04, 0x2e, 0x00,
    0xa9, 0x5f, 0x86, 0x04, 0xca, 0xc5, 0x3a, 0xcc, 0x26, 0xd9, 0x39, 0x7e, 0xa9, 0x2d,
    0x28, 0x6d, 0xc0, 0xcc, 0x6e, 0x81, 0x9f, 0xb9, 0xb7, 0x11, 0x33, 0x32, 0x23, 0x47,
    0x98, 0x43, 0x0d, 0xa5, 0x1c, 0x59, 0xf3, 0xcd, 0xd2, 0x4a, 0xb7, 0x3e, 0x69, 0xd9,
    0x21, 0x53, 0x9a, 0xf2, 0x6e, 0x77, 0x62, 0xae, 0x50, 0xda, 0x85, 0xc6, 0xaa, 0xc4,
    0xb5, 0x1c, 0xcd, 0xa8, 0xa5, 0xdd, 0x6e, 0x62, 0x73, 0xff, 0x5f, 0x7b, 0xd7, 0x3c,
    0x17, 0xba, 0x47, 0x0c, 0x89, 0x0e, 0x62, 0x79, 0x43, 0x94, 0xaa, 0xa8, 0x47, 0xf4,
    0x4c, 0x38, 0x89, 0xa8, 0x81, 0xad, 0x23, 0x13, 0x27, 0x0c, 0x17, 0xcf, 0x3d, 0x83,
    0x84, 0x57, 0x36, 0xe7, 0x22, 0x26, 0x2e, 0x76, 0xfd, 0x56, 0x80, 0x83, 0xf6, 0x70,
    0xd4, 0x5c, 0x91, 0x48, 0x84, 0x7b, 0x18, 0xdb, 0x0e, 0x15, 0x3b, 0x49, 0x26, 0x23,
    0xe6, 0xa3, 0xe2, 0xc6, 0x3a, 0x23, 0x57, 0x66, 0xb0, 0x72, 0xb8, 0x12, 0x17, 0x4f,
    0x86, 0xfe, 0x48, 0x0d, 0x53, 0xea, 0xfe, 0x31, 0x48, 0x7d, 0x86, 0xde, 0xeb, 0x82,
    0x86, 0x1e, 0x62, 0x03, 0x98, 0x59, 0x00, 0x37, 0xeb, 0x61, 0xe9, 0xf9, 0x7a, 0x40,
    0x78, 0x1c, 0xba, 0xbc, 0x0b, 0x88, 0xfb, 0xfd, 0x9d, 0xd5, 0x01, 0x11, 0x94, 0xe0,
    0x35, 0xbe, 0x33, 0xe8, 0xe5, 0x36, 0xfb, 0x9c, 0x45, 0xcb, 0x75, 0xaf, 0xd6, 0x35,
    0xff, 0x78, 0x92, 0x7f, 0xa1, 0x7c, 0xa8, 0xfc, 0xb7, 0xf7, 0xa8, 0x52, 0xa9, 0xc6,
    0x84, 0x72, 0x3d, 0x1c, 0xc9, 0xdf, 0x35, 0xc6, 0xe6, 0x00, 0xe1, 0x48, 0x72, 0xce,
    0x83, 0x1b, 0xcc, 0xf8, 0x33, 0x2d, 0x4f, 0x98, 0x75, 0x00, 0x3c, 0x41, 0xdf, 0x7a,
    0xed, 0x38, 0x53, 0xb1
};

unsigned char  hdcp_22_tx_key_lc128_test[16] = {
    0x93, 0xce, 0x5a, 0x56, 0xa0, 0xa1, 0xf4, 0xf7,
    0x3c, 0x65, 0x8a, 0x1b, 0xd2, 0xae, 0xf0, 0xf7
};

unsigned char   hdcp_22_tx_key_e_test[1] = {0x03};

unsigned char   hdcp_22_tx_key_n_test[384] = {
    0xA2, 0xC7, 0x55, 0x57, 0x54, 0xCB, 0xAA, 0xA7, 0x7A,
    0x27, 0x92, 0xC3, 0x1A, 0x6D, 0xC2, 0x31, 0xCF, 0x12,
    0xC2, 0x24, 0xBF, 0x89, 0x72, 0x46, 0xA4, 0x8D, 0x20,
    0x83, 0xB2, 0xDD, 0x04, 0xDA, 0x7E, 0x01, 0xA9, 0x19,
    0xEF, 0x7E, 0x8C, 0x47, 0x54, 0xC8, 0x59, 0x72, 0x5C,
    0x89, 0x60, 0x62, 0x9F, 0x39, 0xD0, 0xE4, 0x80, 0xCA,
    0xA8, 0xD4, 0x1E, 0x91, 0xE3, 0x0E, 0x2C, 0x77, 0x55,
    0x6D, 0x58, 0xA8, 0x9E, 0x3E, 0xF2, 0xDA, 0x78, 0x3E,
    0xBA, 0xD1, 0x05, 0x37, 0x07, 0xF2, 0x88, 0x74, 0x0C,
    0xBC, 0xFB, 0x68, 0xA4, 0x7A, 0x27, 0xAD, 0x63, 0xA5,
    0x1F, 0x67, 0xF1, 0x45, 0x85, 0x16, 0x49, 0x8A, 0xE6,
    0x34, 0x1C, 0x6E, 0x80, 0xF5, 0xFF, 0x13, 0x72, 0x85,
    0x5D, 0xC1, 0xDE, 0x5F, 0x01, 0x86, 0x55, 0x86, 0x71,
    0xE8, 0x10, 0x33, 0x14, 0x70, 0x2A, 0x5F, 0x15, 0x7B,
    0x5C, 0x65, 0x3C, 0x46, 0x3A, 0x17, 0x79, 0xED, 0x54,
    0x6A, 0xA6, 0xC9, 0xDF, 0xEB, 0x2A, 0x81, 0x2A, 0x80,
    0x2A, 0x46, 0xA2, 0x06, 0xDB, 0xFD, 0xD5, 0xF3, 0xCF,
    0x74, 0xBB, 0x66, 0x56, 0x48, 0xD7, 0x7C, 0x6A, 0x03,
    0x14, 0x1E, 0x55, 0x56, 0xE4, 0xB6, 0xFA, 0x38, 0x2B,
    0x5D, 0xFB, 0x87, 0x9F, 0x9E, 0x78, 0x21, 0x87, 0xC0,
    0x0C, 0x63, 0x3E, 0x8D, 0x0F, 0xE2, 0xA7, 0x19, 0x10,
    0x9B, 0x15, 0xE1, 0x11, 0x87, 0x49, 0x33, 0x49, 0xB8,
    0x66, 0x32, 0x28, 0x7C, 0x87, 0xF5, 0xD2, 0x2E, 0xC5,
    0xF3, 0x66, 0x2F, 0x79, 0xEF, 0x40, 0x5A, 0xD4, 0x14,
    0x85, 0x74, 0x5F, 0x06, 0x43, 0x50, 0xCD, 0xDE, 0x84,
    0xE7, 0x3C, 0x7D, 0x8E, 0x8A, 0x49, 0xCC, 0x5A, 0xCF,
    0x73, 0xA1, 0x8A, 0x13, 0xFF, 0x37, 0x13, 0x3D, 0xAD,
    0x57, 0xD8, 0x51, 0x22, 0xD6, 0x32, 0x1F, 0xC0, 0x68,
    0x4C, 0xA0, 0x5B, 0xDD, 0x5F, 0x78, 0xC8, 0x9F, 0x2D,
    0x3A, 0xA2, 0xB8, 0x1E, 0x4A, 0xE4, 0x08, 0x55, 0x64,
    0x05, 0xE6, 0x94, 0xFB, 0xEB, 0x03, 0x6A, 0x0A, 0xBE,
    0x83, 0x18, 0x94, 0xD4, 0xB6, 0xC3, 0xF2, 0x58, 0x9C,
    0x7A, 0x24, 0xDD, 0xD1, 0x3A, 0xB7, 0x3A, 0xB0, 0xBB,
    0xE5, 0xD1, 0x28, 0xAB, 0xAD, 0x24, 0x54, 0x72, 0x0E,
    0x76, 0xD2, 0x89, 0x32, 0xEA, 0x46, 0xD3, 0x78, 0xD0,
    0xA9, 0x67, 0x78, 0xC1, 0x2D, 0x18, 0xB0, 0x33, 0xDE,
    0xDB, 0x27, 0xCC, 0xB0, 0x7C, 0xC9, 0xA4, 0xBD, 0xDF,
    0x2B, 0x64, 0x10, 0x32, 0x44, 0x06, 0x81, 0x21, 0xB3,
    0xBA, 0xCF, 0x33, 0x85, 0x49, 0x1E, 0x86, 0x4C, 0xBD,
    0xF2, 0x3D, 0x34, 0xEF, 0xD6, 0x23, 0x7A, 0x9F, 0x2C,
    0xDA, 0x84, 0xF0, 0x83, 0x83, 0x71, 0x7D, 0xDA, 0x6E,
    0x44, 0x96, 0xCD, 0x1D, 0x05, 0xDE, 0x30, 0xF6, 0x1E,
    0x2F, 0x9C, 0x99, 0x9C, 0x60, 0x07
};

void test_hdcp2tx_fsm(void)
{
#ifdef CONFIG_HDMIHDCP2TX_TEST
    unsigned short cnt = 1000;
    HDCPTX_WARIN("test_hdcp2tx_fsm start\n");
    ScalerHdmiHdcp2TxInit();
    ScalerHdmiHdcp2Tx2ChangeAuthState(0, _HDCP_2_2_TX_STATE_IDLE);
    while (--cnt) {
        ScalerHdmiHdcp2Tx2Handler(0);
        mdelay(10);
    }
#endif
}


/*
 *Device Attribute
 */
ssize_t rtk_hdcp2tx_show_param(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    int ret = -1;
    if (strncmp(attr->attr.name, "hdcp2tx_func_test", 17) == 0) {
        ret = sprintf(buf, "%u\n", hdcp2tx_func_test);
    }
    else if (strncmp(attr->attr.name, "hdcp2tx_read_log_print", 22) == 0) {
        ret = sprintf(buf, "%u\n", hdcp2tx_read_log_print);
    }
    else if (strncmp(attr->attr.name, "hdcp2tx_cal_log_print", 21) == 0) {
        ret = sprintf(buf, "%u\n", hdcp2tx_cal_log_print);
    }
    else if (strncmp(attr->attr.name, "hdcprx_type", 11) == 0) {
        ret = sprintf(buf, "%u\n", GET_HDMIRX_HDCP_TYPE());
    }
    else {
        HDCPTX_WARIN("invalid command - %s\n", buf);
    }
    return ret;
}

ssize_t rtk_hdcp2tx_set_param(struct device *dev,
                              struct device_attribute *attr,
                              const char *buf, size_t count)

{
    unsigned long val;

    if (strncmp(attr->attr.name, "hdcp2tx_func_test", 17) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            hdcp2tx_func_test = val;
            if (val != 0) {
                set_hdmitx_connect_status(0, 1);
                SET_HDMIRX_HDCP_TYPE(2);
                SET_HDMI_HDCPTX_RUN(0, 1);
            }
            else {
                SET_HDMIRX_HDCP_TYPE(0);
                SET_HDMI_HDCPTX_RUN(0, 0);
                ScalerHdmiHdcp2TxClearAuthState(0);
                msleep(10); //wait hdcp status clear
                set_hdmitx_connect_status(0, 0);
            }
            HDCPTX_WARIN("hdcp2tx_func_test val=%u", val);
        }
    }
    else if (strncmp(attr->attr.name, "hdcp2tx_read_log_print", 22) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            hdcp2tx_read_log_print = val;
            HDCPTX_WARIN("hdcp2tx_read_log_print val=%u", val);
        }
    }
    else if (strncmp(attr->attr.name, "hdcp2tx_cal_log_print", 21) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            hdcp2tx_cal_log_print = val;
            HDCPTX_WARIN("hdcp2tx_cal_log_print val=%u", val);
        }
    }
    else if (strncmp(attr->attr.name, "hdcp2tx_run_test", 16) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            if (val != 0) {
                set_hdmitx_connect_status(0, 1);
                SET_HDMIRX_HDCP_TYPE(2);
                SET_HDMI_HDCPTX_RUN(0, 1);
            }
            else {
                SET_HDMIRX_HDCP_TYPE(0);
                SET_HDMI_HDCPTX_RUN(0, 0);
                ScalerHdmiHdcp2TxClearAuthState(0);
                msleep(10); //wait hdcp status clear
                set_hdmitx_connect_status(0, 0);
            }
            HDCPTX_WARIN("hdcp2tx_run_test val=%u", val);
        }
    }
    else if (strncmp(attr->attr.name, "hdcprx_type", 11) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            SET_HDMIRX_HDCP_TYPE(val);
            ScalerHdmiHdcp2TxClearAuthState(0);
            HDCPTX_WARIN("SET_HDMIRX_HDCP_TYPE=%u", val);
        }
    }
    else if(strncmp(attr->attr.name, "hdcp2tx_load_key", 16) == 0){
        HDCPTX_WARIN("DBG load hdcp22 tx key\n");
        load_hdcp22_tx_key();
    }
    else {
        HDCPTX_WARIN("invalid command - %s\n", buf);
    }
    return count;
}

DEVICE_ATTR(hdcp2tx_load_key, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp2tx_show_param, rtk_hdcp2tx_set_param);
DEVICE_ATTR(hdcp2tx_run_test, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp2tx_show_param, rtk_hdcp2tx_set_param);
DEVICE_ATTR(hdcp2tx_func_test, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp2tx_show_param, rtk_hdcp2tx_set_param);
DEVICE_ATTR(hdcp2tx_read_log_print, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp2tx_show_param, rtk_hdcp2tx_set_param);
DEVICE_ATTR(hdcp2tx_cal_log_print, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp2tx_show_param, rtk_hdcp2tx_set_param);
DEVICE_ATTR(hdcprx_type, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp2tx_show_param, rtk_hdcp2tx_set_param);

void creat_hdcp2tx_device_node(struct device *dev)
{
    device_create_file(dev, &dev_attr_hdcp2tx_func_test);
    device_create_file(dev, &dev_attr_hdcp2tx_read_log_print);
    device_create_file(dev, &dev_attr_hdcp2tx_cal_log_print);
    device_create_file(dev, &dev_attr_hdcp2tx_run_test);
    device_create_file(dev, &dev_attr_hdcprx_type);
    device_create_file(dev, &dev_attr_hdcp2tx_load_key);
    return;
}

void remove_hdcp2tx_device_node(struct device *dev)
{
    device_remove_file(dev, &dev_attr_hdcp2tx_func_test);
    device_remove_file(dev, &dev_attr_hdcp2tx_read_log_print);
    device_remove_file(dev, &dev_attr_hdcp2tx_cal_log_print);
    device_remove_file(dev, &dev_attr_hdcp2tx_run_test);
    device_remove_file(dev, &dev_attr_hdcprx_type);
    device_remove_file(dev, &dev_attr_hdcp2tx_load_key);
    return;
}

