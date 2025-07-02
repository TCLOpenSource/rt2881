#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp14_tx.h>
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp2_tx.h>
#include "rbus/hdmitx20_mac0_reg.h"
#include <linux/miscdevice.h>
#include "hdcptx_common.h"
#include <hdcp/hdcptx/hdcptx_export.h>


unsigned char hdcp14tx_func_test = 0;
unsigned char hdcp14tx_sha_log_print = 0;
unsigned char hdcp14tx_cal_log_print = 0;
unsigned char hdcp14tx_sha_test = 0;

extern unsigned char hdmi_hdcptx_timer_eanble;
extern HDMI_HDCPTX_E hdcp_tx_mode;

extern unsigned char g_pucHdmiHdcp14Tx2HdcpM0[8];
extern unsigned char hdcp_14_tx_key[336 + 16];
unsigned char g_fake_ksv_list[_HDCP_TX_DEVICE_COUNT_MAX * 5];
unsigned int g_fake_sha1_value[5];
unsigned char g_Bksv[5] = {0x14, 0xf7, 0x61, 0x03, 0xb7};//{0xb7, 0x03, 0x61, 0xf7, 0x14};

unsigned char g_R0[2] = {0x85, 0x64};
unsigned char g_receive_b_keys[280] = {
    0xbc, 0x13, 0xe0, 0xc7, 0x5b, 0xf0, 0xfd,
    0xae, 0x0d, 0x2c, 0x7f, 0x76, 0x44, 0x3b,
    0x24, 0xbf, 0x21, 0x85, 0xa3, 0x6c, 0x60,
    0xf4, 0xbc, 0x6c, 0xbc, 0xd7, 0xa3, 0x2f,
    0xa7, 0x2e, 0x69, 0xc5, 0xeb, 0x63, 0x88,
    0x7f, 0xa2, 0xd2, 0x7a, 0x37, 0xd9, 0xf8,
    0x32, 0xfd, 0x35, 0x29, 0xde, 0xa3, 0xd1,
    0x48, 0x5f, 0xc2, 0x40, 0xcc, 0x9b, 0xae,
    0x3b, 0x98, 0x57, 0x79, 0x7d, 0x51, 0x03,
    0x0d, 0xd1, 0x70, 0xbe, 0x61, 0x52, 0x50,
    0x1a, 0x74, 0x8b, 0xe4, 0x86, 0x6b, 0xb1,
    0xf9, 0x60, 0x6a, 0x7c, 0x34, 0x8c, 0xca,
    0x4b, 0xbb, 0x03, 0x78, 0x99, 0xee, 0xa1,
    0x19, 0x0e, 0xcf, 0x9c, 0xc0, 0x95, 0xa9,
    0xa8, 0x21, 0xc4, 0x68, 0x97, 0x44, 0x7f,
    0x1a, 0x8a, 0x0b, 0xc4, 0x29, 0x8a, 0x41,
    0xae, 0xfc, 0x08, 0x53, 0xe6, 0x20, 0x82,
    0xf7, 0x5d, 0x4a, 0x0c, 0x49, 0x7b, 0xa4,
    0xad, 0x64, 0x95, 0xfc, 0x8a, 0x06, 0xd8,
    0x67, 0xc2, 0x02, 0x0c, 0x2b, 0x2e, 0x02,
    0x8f, 0x11, 0x6b, 0x18, 0xf4, 0xae, 0x8d,
    0xe3, 0x05, 0x3f, 0xa3, 0xe9, 0xfa, 0x69,
    0x37, 0xd8, 0x00, 0x28, 0x81, 0xc7, 0xd1,
    0xc3, 0xa5, 0xfd, 0x1c, 0x15, 0x66, 0x9c,
    0x9e, 0x93, 0xd4, 0x1e, 0x08, 0x11, 0xf7,
    0x2c, 0x40, 0x74, 0x50, 0x9e, 0xec, 0x6c,
    0x8b, 0x7f, 0xd8, 0x19, 0x27, 0x9b, 0x61,
    0xd7, 0xca, 0xad, 0xa0, 0xa0, 0x6c, 0xe9,
    0x92, 0x97, 0xdc, 0xa1, 0xf8, 0xc1, 0xdb,
    0x5d, 0x1a, 0xaa, 0x99, 0xde, 0xa4, 0x89,
    0x60, 0xcb, 0x56, 0xdd, 0xba, 0xa1, 0xd9,
    0x85, 0xd4, 0xad, 0x5e, 0x5f, 0xf2, 0xe0,
    0x12, 0x80, 0x16, 0x12, 0x21, 0xdf, 0x6d,
    0xca, 0x31, 0xa5, 0xf2, 0x40, 0x65, 0x89,
    0x1d, 0x30, 0xe8, 0xcb, 0x19, 0x8e, 0x6f,
    0xd1, 0xc1, 0x8b, 0xed, 0x07, 0xd3, 0xfa,
    0xce, 0xc7, 0xec, 0x09, 0x24, 0x5b, 0x43,
    0xb0, 0x81, 0x29, 0xef, 0xed, 0xd5, 0x83,
    0x21, 0x34, 0xcf, 0x4c, 0xe2, 0x86, 0xe5,
    0xed, 0xee, 0xf9, 0xd0, 0x99, 0xb7, 0x8c
};

unsigned char g_receive_b_enkeys[320] = {
    0x6d, 0x7a, 0x5d, 0x23, 0x17, 0x0d, 0x16, 0x2f,
    0x43, 0x11, 0x69, 0x4b, 0x5e, 0x74, 0x50, 0x4e,
    0x1a, 0x09, 0x77, 0x75, 0x01, 0x1c, 0x55, 0x20,
    0x55, 0x3e, 0x2f, 0x3c, 0x59, 0x77, 0x55, 0x2b,
    0x11, 0x5e, 0x75, 0x55, 0x07, 0x7f, 0x59, 0x48,
    0x0b, 0x42, 0x4f, 0x68, 0x54, 0x29, 0x1b, 0x45,
    0x68, 0x5e, 0x2b, 0x3f, 0x28, 0x34, 0x45, 0x61,
    0x35, 0x44, 0x22, 0x36, 0x14, 0x0b, 0x46, 0x14,
    0x50, 0x32, 0x6c, 0x59, 0x4c, 0x31, 0x3c, 0x67,
    0x08, 0x14, 0x76, 0x4a, 0x51, 0x35, 0x0c, 0x36,
    0x62, 0x4e, 0x61, 0x14, 0x25, 0x60, 0x67, 0x77,
    0x39, 0x61, 0x28, 0x7a, 0x54, 0x77, 0x09, 0x0d,
    0x72, 0x6f, 0x5c, 0x1e, 0x44, 0x1a, 0x3d, 0x74,
    0x73, 0x53, 0x30, 0x3c, 0x6b, 0x69, 0x58, 0x17,
    0x4d, 0x11, 0x6f, 0x1e, 0x3e, 0x03, 0x11, 0x0e,
    0x78, 0x06, 0x34, 0x72, 0x05, 0x78, 0x32, 0x77,
    0x30, 0x78, 0x01, 0x47, 0x76, 0x70, 0x45, 0x4e,
    0x14, 0x4c, 0x74, 0x3a, 0x12, 0x73, 0x4e, 0x4b,
    0x09, 0x77, 0x13, 0x1c, 0x4f, 0x20, 0x69, 0x2e,
    0x30, 0x0f, 0x15, 0x7a, 0x12, 0x18, 0x0a, 0x42,
    0x77, 0x6f, 0x28, 0x5e, 0x62, 0x75, 0x1e, 0x5e,
    0x7b, 0x0c, 0x54, 0x47, 0x2b, 0x54, 0x78, 0x6a,
    0x68, 0x57, 0x56, 0x1e, 0x30, 0x10, 0x2c, 0x41,
    0x07, 0x7f, 0x6e, 0x1a, 0x68, 0x45, 0x7b, 0x7a,
    0x4c, 0x52, 0x12, 0x0a, 0x78, 0x2b, 0x14, 0x5f,
    0x1d, 0x09, 0x4b, 0x16, 0x7e, 0x37, 0x00, 0x26,
    0x7a, 0x24, 0x27, 0x79, 0x70, 0x4b, 0x4f, 0x7e,
    0x7b, 0x69, 0x70, 0x76, 0x29, 0x6e, 0x2a, 0x5b,
    0x49, 0x50, 0x5a, 0x57, 0x39, 0x4b, 0x74, 0x79,
    0x71, 0x79, 0x2b, 0x3f, 0x73, 0x6e, 0x3e, 0x35,
    0x69, 0x58, 0x3b, 0x7d, 0x7d, 0x3b, 0x22, 0x02,
    0x1a, 0x7c, 0x4d, 0x2a, 0x6e, 0x66, 0x6c, 0x38,
    0x7d, 0x25, 0x56, 0x60, 0x7a, 0x30, 0x12, 0x71,
    0x71, 0x59, 0x70, 0x20, 0x4f, 0x04, 0x17, 0x7c,
    0x5d, 0x07, 0x3c, 0x09, 0x17, 0x67, 0x1f, 0x37,
    0x2b, 0x54, 0x47, 0x1b, 0x25, 0x62, 0x0a, 0x1b,
    0x58, 0x24, 0x60, 0x79, 0x18, 0x6d, 0x62, 0x5c,
    0x50, 0x53, 0x44, 0x4d, 0x27, 0x74, 0x12, 0x09,
    0x7c, 0x77, 0x31, 0x5c, 0x0c, 0x61, 0x7f, 0x00,
    0x17, 0x5d, 0x3c, 0x16, 0x67, 0x4f, 0x4b, 0x2c
};
unsigned char fake_ksv_list1[5] = {
    0x2e, 0x17, 0x6a, 0x79, 0x35
};
unsigned char fake_ksv_list2[10] = {
    0x2e, 0x17, 0x6a, 0x79, 0x35,
    0x0f, 0xe2, 0x71, 0x8e, 0x47
};
unsigned char fake_ksv_list3[15] = {
    0x2e, 0x17, 0x6a, 0x79, 0x35,
    0x0f, 0xe2, 0x71, 0x8e, 0x47,
    0xa6, 0x97, 0x53, 0xe8, 0x74
};
unsigned char fake_ksv_list4[20] = {
    0x2e, 0x17, 0x6a, 0x79, 0x35,
    0x0f, 0xe2, 0x71, 0x8e, 0x47,
    0xa6, 0x97, 0x53, 0xe8, 0x74,
    0xa6, 0x97, 0x53, 0xe8, 0x74
};
unsigned int sha1_result1[5] = {0xf5b0ceb8, 0x79d98004, 0xa6b8d9e8, 0xfdf89572, 0x7be18b4d};
unsigned int sha1_result2[5] = {0x5a578215, 0x6796afc7, 0x0b7e6b2b, 0xdf9c2d9c, 0xd0879f6d};
unsigned int sha1_result3[5] = {0x0fcbd586, 0xefc107ef, 0xccd70a1d, 0xb1186dda, 0x1fb3ff5e};
unsigned int sha1_result4[5] = {0xd3fce232, 0x5ed7d444, 0xccb011ba, 0x9a842863, 0x72437e7d};

void hdcp14Tx_sha_test(unsigned char select)
{
    unsigned short usKsvFifoLength = 0;
    unsigned char port = 0;
    switch (select) {
        case 1:
            HDCPTX_WARIN("test_hdcp14Tx_sha device = 1, depth =2\n");
            usKsvFifoLength = 5;

            // fill Ksv list
            memcpy(g_fake_ksv_list, fake_ksv_list1, usKsvFifoLength);

            memcpy(g_fake_sha1_value, sha1_result1, 20);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port, 1);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port, 2);
            break;

        case 2:
            HDCPTX_WARIN("test_hdcp14Tx_sha device = 2, depth =2\n");
            usKsvFifoLength = 10;
            // fill Ksv list
            memcpy(g_fake_ksv_list, fake_ksv_list2, usKsvFifoLength);

            memcpy(g_fake_sha1_value, sha1_result2, 20);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port, 2);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port, 2);

            break;

        case 3:
            HDCPTX_WARIN("test_hdcp14Tx_sha device = 3, depth =2\n");
            usKsvFifoLength = 15;
            // fill Ksv list
            memcpy(g_fake_ksv_list, fake_ksv_list3, usKsvFifoLength);

            memcpy(g_fake_sha1_value, sha1_result3, 20);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port, 3);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port, 2);

            break;

        case 4:
            HDCPTX_WARIN("test_hdcp14Tx_sha device = 4, depth =2\n");
            usKsvFifoLength = 20;
            // fill Ksv list
            memcpy(g_fake_ksv_list, fake_ksv_list4, usKsvFifoLength);

            memcpy(g_fake_sha1_value, sha1_result4, 20);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port, 4);
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port, 2);

            break;

        default:
            break;
    }

    //fill Mo data
    g_pucHdmiHdcp14Tx2HdcpM0[0] = 0x8f;
    g_pucHdmiHdcp14Tx2HdcpM0[1] = 0xe7;

    g_pucHdmiHdcp14Tx2HdcpM0[2] = 0xbb;
    g_pucHdmiHdcp14Tx2HdcpM0[3] = 0x38;

    g_pucHdmiHdcp14Tx2HdcpM0[4] = 0xce;
    g_pucHdmiHdcp14Tx2HdcpM0[5] = 0x3d;

    g_pucHdmiHdcp14Tx2HdcpM0[6] = 0x2d;
    g_pucHdmiHdcp14Tx2HdcpM0[7] = 0x37;

#ifdef HDCP_HW_SPEED_UP
    ScalerHdmiHdcp14Tx2Auth2WriteShaInput(port);
    ScalerHdmiHdcp14Tx2Auth2CompareV(port);
#else
    ScalerHdmiHdcp14Tx2Auth2Proc(port);
#endif
}

void test_hdcp14tx_handle(void)
{
    unsigned short usKsvFifoLength = 15;

    //memcpy(&hdcp_14_tx_key[HDCPTX14_KEY_START], g_receive_b_keys, 280);
    memcpy(&hdcp_14_tx_key[HDCPTX14_KEY_START], g_receive_b_enkeys, 320);
    // fill Ksv list
    memcpy(g_fake_ksv_list, fake_ksv_list3, usKsvFifoLength);

    memcpy(g_fake_sha1_value, sha1_result3, 20);
    ScalerHdmiHdcp14Tx2Handler(0);
}
void HDCP14_TX_TEST(void)
{
#if 0
    unsigned char uci;
    pm_printk(LOGGER_ERROR, "\nhdcp14Tx_sha_test\n");
    for (uci = 1; uci < 5; uci++) {
        hdcp14Tx_sha_test(uci);
    }
#endif
    test_hdcp14tx_handle();
}
#if 0
void HdmiTxInit(void)
{
    pm_printk(LOGGER_ERROR, "RealTxInit \n");
    rtd_outl(0xb8000114, 0x00000701);//dclken
    rtd_outl(0xb8000110, 0x00400001);//hdmitx_en
    rtd_outl(0xb8000208, 0x00002000);//dclksel
    rtd_outl(0xb8000230, 0x00000011);
    rtd_outl(0xb8008044, 0x0000000f);

    rtd_outl(0xb800780c, 0x00000200);
    rtd_outl(0xb8007800, 0x01000000);// hdmi2.0
    rtd_outl(0xb8007808, 0x10124120);
    rtd_outl(0xb8007814, 0x00000000);
    rtd_outl(0xb8007818, 0x00022210);
    rtd_outl(0xb800781c, 0x00010000);
    rtd_outl(0xb8007820, 0x03302100);
    rtd_outl(0xb8007824, 0x03301010);
    rtd_outl(0xb8007828, 0x10050103);
    rtd_outl(0xb8007830, 0x00001000);
    rtd_outl(0xb8007844, 0x10001111);

    rtd_outl(0xb800785c, 0xccb6c0e4);
    rtd_outl(0xb80078b0, 0x13000000);
    rtd_outl(0xb80078b4, 0x97200037);
    rtd_outl(0xb80078ac, 0x00003ff0);
    rtd_outl(0xb8007804, 0x00000000);
    rtd_outl(0xb8007824, 0x00012010);
    rtd_outl(0xb8007858, 0x80444000);
    rtd_outl(0xb80078ac, 0x00003ff1);
    rtd_outl(0xb80078ac, 0x00003ff0);


    // hmdi clk enable setting
    rtd_outl(0xb8000114, 0x00000401);
    rtd_outl(0xb8002044, 0x00000101);
    rtd_outl(0xb8032410, 0x00000013);
    rtd_outl(0xb8032404, 0x00203855);
    rtd_outl(0xb8032404, 0x00203845);
    rtd_outl(0xb8032404, 0x00203855);
    rtd_outl(0xb8032400, 0x450a005f);//for 4k60
    //rtd_outl(0xb8032400, 0x4a8a015f);//for 4k120
    rtd_outl(0xb8032414, 0x00000001);//for 4k60
    //rtd_outl(0xb8032414, 0x00000000);//for 4k120
    rtd_outl(0xb8032410, 0x00000013);
    rtd_outl(0xb8032404, 0x00203851);
    rtd_outl(0xb8032018, 0x00000008);
    rtd_outl(0xb8032048, 0x00000100);
    rtd_outl(0xb80321a8, 0x0000080e);
    //------------- end hdmi clk setting
}
#endif

/*
 *Device Attribute
 */
ssize_t rtk_hdcp14tx_show_param(struct device *dev,
                                struct device_attribute *attr, char *buf)
{
    int ret = -1;
    int  n,i;
    int  count = PAGE_SIZE;
    char *ptr = buf;

    if (strncmp(attr->attr.name, "hdcp14tx_func_test", 17) == 0) {
        ret = sprintf(buf, "%u\n", hdcp14tx_func_test);
    }
    else if (strncmp(attr->attr.name, "hdcp14tx_sha_log_print", 22) == 0) {
        ret = sprintf(buf, "%u\n", hdcp14tx_sha_log_print);
    }
    else if (strncmp(attr->attr.name, "hdcp14tx_cal_log_print", 21) == 0) {
        ret = sprintf(buf, "%u\n", hdcp14tx_cal_log_print);
    }
    else if (strncmp(attr->attr.name, "hdcp14tx_key", 12) == 0) {
        n = scnprintf(ptr, count, "KSV: \n");
        ptr+=n; count-=n;
        for(i = 0; i<5; i++)
        {
            n = scnprintf(ptr, count, "%x\t", hdcp_14_tx_key[i]);
            ptr+=n; count-=n;
        }
        n = scnprintf(ptr, count, "\n");
        ptr+=n; count-=n;

        n = scnprintf(ptr, count, "Device Key: \n");
        ptr+=n; count-=n;
        for(i = 0; i<320; i++)
        {
            if(i%8 == 0)
            {
                n = scnprintf(ptr, count, "\n");
                ptr+=n; count-=n;
            }

            n = scnprintf(ptr, count, "%x\t", hdcp_14_tx_key[5+i]);
            ptr+=n; count-=n;
        }
        n = scnprintf(ptr, count, "\n");
        ptr+=n; count-=n;
        return ptr - buf;
    }
    else if (strncmp(attr->attr.name, "status", 6) == 0) {
        n = scnprintf(ptr, count, "RX_STATUS:%d",GET_HDMIRX_HDCP_TYPE());
        ptr+=n; count-=n;
        for(i=0; i<HDMI_TX_PORT_MAX_NUM; i++){
            n = scnprintf(ptr, count, "HDCPTX_RUN:%d\t", GET_HDMI_HDCPTX_RUN(i));
            ptr+=n; count-=n;
            ret = (get_hdmitx_connect_status(i) || hdmi_hdcptx_timer_eanble!=0);
            n = scnprintf(ptr, count, "HDCP timer enable:\n", ret);
            ptr+=n; count-=n;
            n = scnprintf(ptr, count, "HDCP auth mode:%d auth2_status:%d auth1_status:%d \n", hdcp_tx_mode, GET_HDMI_HDCP2_TX2_AUTH_STATE(i), GET_HDMI_HDCP14_TX2_AUTH_STATE(i));
            ptr+=n; count-=n;
        }
        return ptr - buf;
    }

    else {
        HDCPTX_WARIN("invalid command - %s\n", buf);
    }
    return ret;
}

ssize_t rtk_hdcp14tx_set_param(struct device *dev,
                               struct device_attribute *attr,
                               const char *buf, size_t count)

{
    unsigned long val;

    if (strncmp(attr->attr.name, "hdcp14tx_func_test", 17) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            hdcp14tx_func_test = val;
            if (val != 0) {
                set_hdmitx_connect_status(0, 1);
                SET_HDMIRX_HDCP_TYPE(1);
                SET_HDMI_HDCPTX_RUN(0, 1);
                // fill Ksv list
                memcpy(g_fake_ksv_list, fake_ksv_list1, 5);
                memcpy(g_fake_sha1_value, sha1_result1, 20);
                SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(0, 1);
                SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(0, 2);
            }
            else {
                SET_HDMIRX_HDCP_TYPE(0);
                SET_HDMI_HDCPTX_RUN(0, 0);
                ScalerHdmiHdcp2TxClearAuthState(0);
                msleep(10); //wait hdcp status clear
                set_hdmitx_connect_status(0, 0);
            }
            HDCPTX_WARIN("hdcp14tx_func_test val=%u", val);
        }
    }
    else if (strncmp(attr->attr.name, "hdcp14tx_sha_log_print", 22) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            hdcp14tx_sha_log_print = val;
            HDCPTX_WARIN("hdcp14tx_sha_log_print val=%u", val);
        }
    }
    else if (strncmp(attr->attr.name, "hdcp14tx_cal_log_print", 21) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            hdcp14tx_cal_log_print = val;
            HDCPTX_WARIN("hdcp14tx_cal_log_print val=%u", val);
        }
    }
    else if (strncmp(attr->attr.name, "hdcp14tx_sha_test", 17) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            hdcp14tx_sha_test = val;
            hdcp14Tx_sha_test(val);
            HDCPTX_WARIN("hdcp14tx_sha_test val=%u", val);
            hdcp14tx_sha_test = 0;
        }
    }
    else if (strncmp(attr->attr.name, "hdcp14tx_run_test", 16) == 0) {
        if (sscanf(buf, "%lx", &val) == 1) {
            if (val != 0) {
                set_hdmitx_connect_status(0, 1);
                SET_HDMIRX_HDCP_TYPE(1);
                SET_HDMI_HDCPTX_RUN(0, 1);
            }
            else {
                SET_HDMIRX_HDCP_TYPE(0);
                SET_HDMI_HDCPTX_RUN(0, 0);
                ScalerHdmiHdcp2TxClearAuthState(0);
                msleep(10); //wait hdcp status clear
                set_hdmitx_connect_status(0, 0);
            }
            HDCPTX_WARIN("hdcp14tx_run_test val=%u", val);
        }
    }
    else if(strncmp(attr->attr.name, "hdcp14tx_load_key", 17) == 0){
        HDCPTX_WARIN("DBG load hdcp14 tx key\n");
        load_hdcp14_tx_key();
    }
    else {
        HDCPTX_WARIN("invalid command - %s\n", buf);
    }
    return count;
}

DEVICE_ATTR(hdcp14tx_key, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp14tx_show_param, rtk_hdcp14tx_set_param);
DEVICE_ATTR(hdcp14tx_load_key, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp14tx_show_param, rtk_hdcp14tx_set_param);
DEVICE_ATTR(hdcp14tx_run_test, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp14tx_show_param, rtk_hdcp14tx_set_param);
DEVICE_ATTR(hdcp14tx_func_test, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp14tx_show_param, rtk_hdcp14tx_set_param);
DEVICE_ATTR(hdcp14tx_sha_log_print, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp14tx_show_param, rtk_hdcp14tx_set_param);
DEVICE_ATTR(hdcp14tx_cal_log_print, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp14tx_show_param, rtk_hdcp14tx_set_param);
DEVICE_ATTR(hdcp14tx_sha_test, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP,
            rtk_hdcp14tx_show_param, rtk_hdcp14tx_set_param);

void creat_hdcp14tx_device_node(struct device *dev)
{
    device_create_file(dev, &dev_attr_hdcp14tx_func_test);
    device_create_file(dev, &dev_attr_hdcp14tx_run_test);
    device_create_file(dev, &dev_attr_hdcp14tx_sha_log_print);
    device_create_file(dev, &dev_attr_hdcp14tx_cal_log_print);
    device_create_file(dev, &dev_attr_hdcp14tx_sha_test);
    device_create_file(dev, &dev_attr_hdcp14tx_load_key);
    device_create_file(dev, &dev_attr_hdcp14tx_key);
    return;
}

void remove_hdcp14tx_device_node(struct device *dev)
{
    device_remove_file(dev, &dev_attr_hdcp14tx_func_test);
    device_remove_file(dev, &dev_attr_hdcp14tx_run_test);
    device_remove_file(dev, &dev_attr_hdcp14tx_sha_log_print);
    device_remove_file(dev, &dev_attr_hdcp14tx_cal_log_print);
    device_remove_file(dev, &dev_attr_hdcp14tx_sha_test);
    device_remove_file(dev, &dev_attr_hdcp14tx_load_key);
    device_remove_file(dev, &dev_attr_hdcp14tx_key);
    return;
}

