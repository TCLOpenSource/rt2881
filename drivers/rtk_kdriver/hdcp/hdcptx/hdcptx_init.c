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
// ID Code      : hdcptx_init.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/freezer.h>
#include "hdcptx_common.h"
#include "rbus/hdmitx20_mac0_reg.h"
#include "rbus/hdmitx20_mac1_reg.h"
#ifdef HDCP_HW_SPEED_UP
    #include <rbus/hdcp22_speedup_reg.h>
#endif
#include "hdcp2tx_common.h"
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp2_tx.h>
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp14_tx.h>

static struct task_struct *hdcp_tsk = NULL;

unsigned char g_hdmirx_hdcp_type = 0;
unsigned char g_dprx_hdcp_type = 0;
unsigned char hdmi_hdcptx_run[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char hdmitx_connect[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char hdcptx_hdmi_dvi = 1;//0:DVI, 1:HDMI
unsigned int  hdcptx_hdmi_vfreq[HDMI_TX_PORT_MAX_NUM] = {600};//default 60HZ
unsigned char hdcptx_type_global = 0;

extern unsigned char sink_only_support_hdcp1[HDMI_TX_PORT_MAX_NUM];
extern unsigned char hdcp_tx_running;
extern HDMI_HDCPTX_E hdcp_tx_mode;
extern unsigned char hdmi_hdcptx_timer_eanble;
extern void ScalerHdmiHdcp2TxSetAuthStateIdle(unsigned char port);
extern void creat_hdcp2tx_device_node(struct device *dev);
extern void remove_hdcp2tx_device_node(struct device *dev);
extern void creat_hdcp14tx_device_node(struct device *dev);
extern void remove_hdcp14tx_device_node(struct device *dev);

/*********************************************************************************/
/*  hdmi/dp rx setting  */
void SET_HDMI_MODE_TYPE(unsigned char type)
{
    HDCPTX_INFO("%s:%d\n", __func__, type);
    hdcptx_hdmi_dvi = type;
}
EXPORT_SYMBOL(SET_HDMI_MODE_TYPE);

unsigned char GET_HDMI_MODE_TYPE(void)
{
    return hdcptx_hdmi_dvi;
}
EXPORT_SYMBOL(GET_HDMI_MODE_TYPE);

void SET_HDMIRX_HDCP_TYPE(unsigned char type)
{
    HDCPTX_INFO("%s:%d\n", __func__, type);
    g_hdmirx_hdcp_type = type;
}
EXPORT_SYMBOL(SET_HDMIRX_HDCP_TYPE);

unsigned char GET_HDMIRX_HDCP_TYPE(void)
{
    return g_hdmirx_hdcp_type;
}
EXPORT_SYMBOL(GET_HDMIRX_HDCP_TYPE);

void SET_DPRX_HDCP_TYPE(unsigned char type)
{
    HDCPTX_INFO("%s:%d\n", __func__, type);
    g_dprx_hdcp_type = type;
}
EXPORT_SYMBOL(SET_DPRX_HDCP_TYPE);

unsigned char GET_DPRX_HDCP_TYPE(void)
{
    return g_dprx_hdcp_type;
}
EXPORT_SYMBOL(GET_DPRX_HDCP_TYPE);
/*********************************************************************************/

/*********************************************************************************/
/*hdmi/dp tx setting*/
void SET_HDMI_HDCPTX_RUN(unsigned char port, unsigned int value)
{
    if (port < HDMI_TX_PORT_MAX_NUM) {
        hdmi_hdcptx_run[port] = value;
        HDCPTX_WARIN("HDMITX[%d] hdcprun:%d\n", port, value);
    }
    else {
        HDCPTX_WARIN("HDMITX hdcprun port not correct:%d, MAX port:%d\n", port, HDMI_TX_PORT_MAX_NUM);
    }
}
EXPORT_SYMBOL(SET_HDMI_HDCPTX_RUN);

unsigned char GET_HDMI_HDCPTX_RUN(unsigned char port)
{
    return hdmi_hdcptx_run[port];
}
EXPORT_SYMBOL(GET_HDMI_HDCPTX_RUN);

unsigned char GET_HDCPTX_ONLY_SUPPORT_HDCP1(unsigned char port)
{
    return sink_only_support_hdcp1[port];
}
EXPORT_SYMBOL(GET_HDCPTX_ONLY_SUPPORT_HDCP1);

void set_hdmitx_connect_status(unsigned char port, unsigned char enable)
{
    if (port < HDMI_TX_PORT_MAX_NUM) {
        hdmitx_connect[port] = enable;
        HDCPTX_WARIN("HDMITX[%d] connect enable:%d\n", port, enable);
    }
    else {
        HDCPTX_WARIN("HDMITX port not correct:%d, MAX port:%d\n", port, HDMI_TX_PORT_MAX_NUM);
    }
}
EXPORT_SYMBOL(set_hdmitx_connect_status);

unsigned char get_hdmitx_connect_status(unsigned char port)
{
    return hdmitx_connect[port];
}
EXPORT_SYMBOL(get_hdmitx_connect_status);

void SET_HDCP_TX_VFreq(unsigned char port, unsigned int vfreq)
{
    HDCPTX_INFO("%s:%d\n", __func__, vfreq);
    hdcptx_hdmi_vfreq[port] = vfreq;
}
EXPORT_SYMBOL(SET_HDCP_TX_VFreq);

unsigned int GET_HDCP_TX_VFreq(unsigned char port)
{
    return hdcptx_hdmi_vfreq[port];
}
EXPORT_SYMBOL(GET_HDCP_TX_VFreq);

void hdmitx_load_hdcptx_key(void)
{
    //hdcp22 tx
    load_hdcp22_tx_key();
    //hdcp14 tx key
    load_hdcp14_tx_key();
    return;
}
EXPORT_SYMBOL(hdmitx_load_hdcptx_key);


void HDMITX_HDCP_Set_AuthVersion(unsigned char hdmitx_port,unsigned char HdcpVer)
{
    hdcptx_type_global = HdcpVer;
    HDCPTX_INFO("HDMITX_HDCP_Set_AuthVersion:%d\n", HdcpVer);
}
EXPORT_SYMBOL(HDMITX_HDCP_Set_AuthVersion);
/*********************************************************************************/


void hdcptx_handle(unsigned char port)
{
#if 0 //workaround for dante cts
    if ((sink_only_support_hdcp1 == 1) && (GetSpecialCtsDevice() == 1)) {
        if (hdmi_hdcptx_run == 0) {
            ScalerHdmiHdcp2TxSetAuthStateIdle(port);
            return;
        }
    }
    else
#endif
    {
        if ((((GET_HDMIRX_HDCP_TYPE() == HDCP_OFF) || (GET_HDMIRX_HDCP_TYPE() == NO_HDCP)) && ((GET_DPRX_HDCP_TYPE() == HDCP_OFF) || (GET_DPRX_HDCP_TYPE() == NO_HDCP)) && (hdcptx_type_global == 0)) || (GET_HDMI_HDCPTX_RUN(port) == 0)) {
            if (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) != _HDCP_2_2_TX_STATE_IDLE) {
                if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _TRUE) {
                    ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
                    HDCPTX_WARIN("RX not hdcp mode, Disable Encrypt\n");
                }
            }
            ScalerHdmiHdcp2TxSetAuthStateIdle(port);
            return;
        }
    }
    hdcp_tx_running = 1;
    if ((/*(sink_only_support_hdcp1 == 1) && (GetSpecialCtsDevice() == 1) ||*/ (hdcptx_type_global == 1) ||
          ((GET_HDMIRX_HDCP_TYPE() == HDCP14) || (GET_HDMIRX_HDCP_TYPE() == HDCP14))  || (GET_HDMI_hdcp2_auth_cnt_STAT(port) >= HDCP14_TX_AUTH_MAX_CNT))
            && (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON)) {
        ScalerHdmiHdcp14Tx2Handler(port);
        if ((GET_HDMI_HDCP14_TX2_AUTH_STATE(port) >= _HDMI_TX_HDCP_STATE_AUTH_2)
                && (GET_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port) == 1)) {
            if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON) {
                if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == 0) {
                    rtd_outl(HDMITX20_MAC0_HDCP14_ENGINE_8_reg, 0x40);
                    ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 1);
                    hdcp_tx_mode = HDCP14;
                    HDCPTX_WARIN("HDCP14 - Encrypt *******\n");
                    //todo core_timer_event_addTimerEvent(_HDMI_TX_HDCP_VERIFY_RI_TIMEOUT, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port, 0);
                }
            }
            else {
                if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == 1) {
                    ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
                    HDCPTX_WARIN("HDCP14 - Discryption\n");
                    //todo core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port);
                }
            }
        }
        else {
            if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _TRUE) {
                ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
                HDCPTX_WARIN("Disable Encrypt1\n");
            }
        }
        if (GET_HDMI_hdcp14_auth_cnt_STAT(port) >= HDCP14_TX_AUTH_MAX_CNT) {
            if (((GET_HDMIRX_HDCP_TYPE() == HDCP_OFF) || (GET_HDMIRX_HDCP_TYPE() == NO_HDCP)) && ((GET_DPRX_HDCP_TYPE() == HDCP_OFF) || (GET_DPRX_HDCP_TYPE() == NO_HDCP))) {
                HDCPTX_WARIN("HDCP RX not run, disable hdcptx run!!!\n");
                hdmi_hdcptx_run[port] = 0;
            }
            else {
                ScalerHdmiHdcp2TxSetAuthTimeOut(port);
                HDCPTX_WARIN("AUTH timeout!!!!\n");
            }
        }
    }
    else
    {
        ScalerHdmiHdcp2Tx2Handler(0);
    }

}

int hdcptx_thread(void *data)
{
    unsigned char i;
    set_freezable();
    while (!kthread_should_stop()) {
        for (i = 0; i < HDMI_TX_PORT_MAX_NUM; i++) {
            if (get_hdmitx_connect_status(i) || (hdmi_hdcptx_timer_eanble!=0)) {
                hdcptx_handle(i);
                if((GET_HDMI_HDCP2_TX2_AUTH_STATE(i) == _HDCP_2_2_TX_STATE_AUTHEN_DONE) || (GET_HDMI_HDCP2_TX2_AUTH_STATE(i) == _HDCP_2_2_TX_STATE_IDLE)){
                    msleep(10);
                }else{
                    msleep(1);
                }
            }
            else {
                msleep(20);
            }
        }
        if (freezing(current)) {
            try_to_freeze();
        }
    }
    return 0;
}

int hdcptx_open(struct inode *inode, struct file *file)
{
    return 0;
}

int hdcptx_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations rtk_hdcptx_fops = {
    .owner      = THIS_MODULE,
    .open       = hdcptx_open,
    .release    = hdcptx_release,
};

static struct miscdevice rtk_hdcptx_miscdev = {
    MISC_DYNAMIC_MINOR,
    "hdcptx_kdv",
    &rtk_hdcptx_fops
};

void hdcp_tx_init(void)
{
    unsigned char port = 0;

    if (misc_register(&rtk_hdcptx_miscdev)) {
        rtd_pr_hdcp_emerg("hdcp_tx_init failed - register misc device failed\n");
        return;
    }
    hdcp14tx_init();

    for (port = 0; port < HDMI_TX_PORT_MAX_NUM; port++) {
#ifdef CONFIG_HDMIHDCP2TX_TEST
        //hdcp_mask(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, ~(_BIT0 | _BIT1), _BIT0 | _BIT1); // HDCP reset
#endif
#ifdef HDCP_HW_SPEED_UP
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), _BIT3); //mac / merlin no speed up
#endif
        memset(g_pucHdmiHdcp2Tx2ReceiverId[port], 0x00, 5);
        memset(g_pucHdmiHdcp2Tx2PairingReceiverID[port], 0, 5);

        SET_HDMI_HDCP2_TX2_AUTH_STATE(port, _HDCP_2_2_TX_STATE_IDLE);
    }
    hdcp_tsk = kthread_run(hdcptx_thread, NULL, "HDCPTX");
    if (IS_ERR(hdcp_tsk)) {
        rtd_pr_hdcp_emerg("hdcp: creat hdcptx thread fail\n\n");
        return;
    }
    creat_hdcp2tx_device_node(rtk_hdcptx_miscdev.this_device);
    creat_hdcp14tx_device_node(rtk_hdcptx_miscdev.this_device);

    return;
}

void hdcp_tx_uninit(void)
{
    unsigned char port = 0;

    remove_hdcp2tx_device_node(rtk_hdcptx_miscdev.this_device);
    remove_hdcp14tx_device_node(rtk_hdcptx_miscdev.this_device);

    if (!IS_ERR(hdcp_tsk)) {
        kthread_stop(hdcp_tsk);
    }
    for (port = 0; port < HDMI_TX_PORT_MAX_NUM; port++) {
        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }
    return;
}

