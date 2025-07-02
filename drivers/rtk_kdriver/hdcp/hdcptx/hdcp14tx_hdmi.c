/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohiunsigned chared. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdcp14Tx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#ifdef HDCP_HW_SPEED_UP
    #include <rbus/hdcp14_speedup_reg.h>
#endif
#include "rbus/hdmitx20_mac0_reg.h"
#include "rbus/hdmitx20_mac1_reg.h"
#ifdef CONFIG_ENABLE_HDMITX
    //#include <tvscalercontrol/hdmitx/hdmitx_ctrl.h>
#endif
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
    //#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
#endif
#include <hdcp/hdcptx/hdmi_hdcp14_tx.h>
#include <hdcp/hdcptx/hdmi_hdcp2_tx.h>
#include "hdcptx_common.h"
#include <i2c-rtk-api.h>

//#define HDCP14_TX_DEBUG
#define HDCP14_TX_I2C_HANDLE_MESSAGE


#ifndef HIBYTE   //wait ecpu.h build ok
    #define LOBYTE(w)                               ((unsigned char)(w))
    #define HIBYTE(w)                               ((unsigned char)(((unsigned short)(w) >> 8) & 0x00FF))
#endif
//extern StructHDMITxOutputTimingInfo stTxRepeaterOutputTimingInfo;

#define HDCP14_DEFAULT_TXKEY
unsigned char tx14_key_align_offset = 0;

extern unsigned short repeater_temp[2];
//extern unsigned char g_detect_cts_device;
unsigned char g_detect_cts_device = 0;

extern unsigned char g_fake_ksv_list[_HDCP_TX_DEVICE_COUNT_MAX * 5];
extern unsigned int g_fake_sha1_value[5];
extern unsigned char g_Bksv[5];
unsigned char g_An[8] = {0x34, 0x27, 0x1c, 0x13, 0x0c, 0x07, 0x04, 0x03};
extern unsigned char g_R0[2];
extern unsigned char hdcp14tx_func_test;
extern unsigned char hdcp14tx_sha_log_print;
extern unsigned char hdcp14tx_cal_log_print;
extern unsigned char hdcp14tx_sha_test;

extern unsigned char GET_HDMI_MODE_TYPE(void);
extern unsigned int GET_HDCP_TX_VFreq(unsigned char port);
//****************************************************************************
// CODE TABLES
//****************************************************************************

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
StructHdmiTxHDCPAuthInfo g_stHdmiHdcp14Tx2AuthInfo[HDMI_TX_PORT_MAX_NUM];
unsigned short hdcp14_auth_cnt[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char g_pucHdmiHdcp14Tx2HdcpBksv[5];
unsigned char g_pucHdmiHdcp14Tx2HdcpM0[8];
unsigned short g_usHdmiHdcp14Tx2KsvFifoIndex;
//unsigned short g_usHdmiMacTxHdcp14KsvFifoIndex = 0;
//unsigned short g_usHdmiMacTx2HdcpKsvFifoStartIndex = 0;
unsigned short g_usHdmiMacTx2HdcpKsvFifoLength = 0;

unsigned char g_pucHdmiTxHdcpKsvFifo[_HDCP_TX_DEVICE_COUNT_MAX * 5];
unsigned char pucHdmiTx2HdcpKsvFifoTemp[_HDCP_TX_DEVICE_COUNT_MAX * 5];
//unsigned char pucHdmiTx2HdcpKsvFifo[_HDCP_TX_DEVICE_COUNT_MAX * 5];
//unsigned char *pucHdmiTx2HdcpKsvFifo = g_pucHdmiHdcp2Tx2Npub; //for reduce dmem use hdcp2tx var buffer

unsigned char g_subAddr[16];
unsigned char g_subaddr_len = 0x01;
unsigned char hdcp14_wait_v_status = 0;

unsigned long hdcp14_start_time[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long hdcp14_diff_ms = 0;
unsigned long hdcp14_verify_ri_cnt[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long hdcp14_verify_ri[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long hdcp14_verify_pj[HDMI_TX_PORT_MAX_NUM] = {0};

unsigned char pj_count = 0;

#ifdef HDCP14_DEFAULT_TXKEY
unsigned char hdcp_14_tx_key[325] = {
    0xc6, 0x32, 0x4e, 0xbb, 0x85,

    0x12, 0x6e, 0x52, 0x69, 0x35, 0x31, 0x4d, 0x51,
    0x28, 0x35, 0x20, 0x39, 0x31, 0x54, 0x06, 0x26,
    0x65, 0x1d, 0x66, 0x39, 0x4b, 0x49, 0x01, 0x2b,
    0x29, 0x34, 0x6a, 0x23, 0x7a, 0x78, 0x7d, 0x70,
    0x7a, 0x6e, 0x63, 0x4a, 0x2e, 0x38, 0x40, 0x40,
    0x49, 0x54, 0x17, 0x42, 0x51, 0x70, 0x64, 0x5a,
    0x2c, 0x32, 0x15, 0x11, 0x07, 0x17, 0x00, 0x00,
    0x6c, 0x44, 0x4c, 0x5c, 0x1e, 0x63, 0x45, 0x70,
    0x5a, 0x0e, 0x59, 0x69, 0x7b, 0x45, 0x48, 0x1e,
    0x1f, 0x5c, 0x13, 0x4f, 0x26, 0x5f, 0x38, 0x46,
    0x2d, 0x18, 0x06, 0x1b, 0x78, 0x71, 0x36, 0x25,
    0x56, 0x32, 0x7b, 0x49, 0x1e, 0x49, 0x70, 0x00,
    0x42, 0x7c, 0x77, 0x15, 0x56, 0x31, 0x51, 0x75,
    0x59, 0x11, 0x3b, 0x5c, 0x65, 0x75, 0x72, 0x21,
    0x0b, 0x61, 0x71, 0x17, 0x1f, 0x06, 0x6c, 0x1a,
    0x5f, 0x06, 0x1f, 0x20, 0x30, 0x22, 0x5a, 0x23,
    0x1a, 0x36, 0x68, 0x66, 0x24, 0x39, 0x58, 0x40,
    0x76, 0x59, 0x54, 0x1d, 0x4a, 0x3b, 0x02, 0x4e,
    0x3e, 0x04, 0x40, 0x3a, 0x1e, 0x07, 0x71, 0x08,
    0x55, 0x18, 0x2a, 0x65, 0x13, 0x55, 0x40, 0x5a,
    0x47, 0x25, 0x49, 0x2f, 0x61, 0x12, 0x60, 0x39,
    0x5e, 0x44, 0x7c, 0x73, 0x62, 0x48, 0x00, 0x22,
    0x01, 0x05, 0x6d, 0x2a, 0x19, 0x49, 0x08, 0x63,
    0x26, 0x3c, 0x09, 0x21, 0x6b, 0x03, 0x1b, 0x41,
    0x60, 0x7b, 0x0f, 0x38, 0x20, 0x1d, 0x5f, 0x7e,
    0x0a, 0x04, 0x5c, 0x6f, 0x7a, 0x6c, 0x22, 0x0b,
    0x5f, 0x4e, 0x74, 0x3e, 0x1c, 0x4f, 0x60, 0x75,
    0x0a, 0x63, 0x39, 0x40, 0x3b, 0x55, 0x3a, 0x6b,
    0x0d, 0x3d, 0x47, 0x53, 0x73, 0x23, 0x02, 0x1c,
    0x11, 0x5a, 0x77, 0x4b, 0x29, 0x7f, 0x68, 0x05,
    0x0a, 0x43, 0x65, 0x56, 0x5c, 0x66, 0x31, 0x4a,
    0x37, 0x5d, 0x34, 0x09, 0x28, 0x7c, 0x26, 0x0b,
    0x4e, 0x35, 0x3c, 0x74, 0x0b, 0x3f, 0x12, 0x5b,
    0x27, 0x3b, 0x58, 0x7b, 0x66, 0x1f, 0x6c, 0x2a,
    0x0e, 0x76, 0x37, 0x5d, 0x28, 0x22, 0x01, 0x3b,
    0x7e, 0x13, 0x29, 0x4c, 0x7b, 0x5a, 0x7d, 0x07,
    0x39, 0x25, 0x3c, 0x00, 0x6c, 0x04, 0x49, 0x14,
    0x1c, 0x6e, 0x71, 0x48, 0x49, 0x07, 0x47, 0x2b,
    0x05, 0x0d, 0x39, 0x49, 0x20, 0x21, 0x3f, 0x46,
    0x4a, 0x43, 0x3c, 0x13, 0x7d, 0x3e, 0x07, 0x6b,
};
#else
unsigned char hdcp_14_tx_key[325]={0}; //bank27
#endif

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
unsigned int uchar_array_to_uint(unsigned char  input_data[4])
{
    unsigned int Udata;
    *((unsigned char *)&Udata + 3) = input_data[0];
    *((unsigned char *)&Udata + 2) = input_data[1];
    *((unsigned char *)&Udata + 1) = input_data[2];
    *((unsigned char *)&Udata + 0) = input_data[3];
    return Udata;
}

unsigned char HdcpPollingFlagProc(unsigned char times, unsigned int reg, unsigned char bit_num, unsigned char value)
{
    while (times--) {
        if (hdcp_in_bit(reg, bit_num) == bit_num) {
            return _SUCCESS;
        }

        mdelay(value * 10);
    }
    return _FAIL;
}

#if 1
unsigned char ScalerMcuHwIICHDCPBurstRead(unsigned char addr, unsigned char sub_addr_len, unsigned char *sub_addr,
                                          unsigned short read_len, unsigned char *p_read_buff)
{

    int ucret = -1;
#ifdef HDCP14_TX_I2C_HANDLE_MESSAGE
#if defined(CONFIG_I2C)
    unsigned char bus_id = HDCP_TX_I2C_BUSID;
    ucret = i2c_master_recv_ex(bus_id, addr, sub_addr, sub_addr_len, p_read_buff, read_len);
#endif
#endif
    if (ucret < 0) {
        return _FAIL;
    }
    else {
        return _SUCCESS;
    }
}

unsigned char ScalerMcuHwIICHDCP14BurstWrite(unsigned char addr, unsigned char *sub_addr, unsigned short write_len)
{
    int ucret = -1;
#ifdef HDCP14_TX_I2C_HANDLE_MESSAGE
#if defined(CONFIG_I2C)
    unsigned char bus_id = HDCP_TX_I2C_BUSID;
    ucret = i2c_master_send_ex(bus_id, addr, sub_addr, write_len);
#endif
#endif
    if (ucret < 0) {
        return _FAIL;
    }
    else {
        return _SUCCESS;
    }
}
#else
#define ScalerMcuHwIICHDCPBurstRead(addr, sub_addr_len, sub_addr, read_len, p_read_buff) i2c_master_recv_ex(HDCP_TX_I2C_BUSID, addr, sub_addr, sub_addr_len, p_read_buff, read_len)
#define ScalerMcuHwIICHDCP14BurstWrite(addr, sub_addr, write_len) i2c_master_send_ex(HDCP_TX_I2C_BUSID, addr, sub_addr, write_len)
#endif

#ifdef HDCP_REPEATER
//--------------------------------------------------
// Description  : Hdcp Repeater Auth1Proc
// Input Value  : None
// Output Value : None
//--------------------------------------------------
unsigned char ScalerHdcpTxGetUpstreamEncryptStatus(EnumInterfaceTxPortNo enumInterfaceTxPortNo)
{
    EnumInputPort enumMappingPort = ScalerDpMacRxRxDxMapping(_RX0);

    enumMappingPort = enumMappingPort;

    switch (enumInterfaceTxPortNo) {
#if(_HDMI_MAC_TX0_SUPPORT == _ON)
        case _OUTPUT_PORT_HDMI_TX0:
            return ScalerDpHdcpRxGetUpstreamEncryptStatus(enumMappingPort);
#endif

#if(_HDMI_MAC_TX1_SUPPORT == _ON)
        case _OUTPUT_PORT_HDMI_TX1:
            return ScalerDpHdcpRxGetUpstreamEncryptStatus(enumMappingPort);
#endif

#if(_HDMI_MAC_TX2_SUPPORT == _ON)
        case _OUTPUT_PORT_HDMI_TX2:
            return ScalerDpHdcpRxGetUpstreamEncryptStatus(enumMappingPort);
#endif

        default:
            return 0;
    }
}
#endif
//--------------------------------------------------
// Description  : Hdcp Repeater Auth1Proc
// Input Value  : None
// Output Value : None
//--------------------------------------------------
unsigned char ScalerHdcpGetHdmiMode(void)
{
    return GET_HDMI_MODE_TYPE();
}

unsigned char Scaler_auth_start_check(unsigned char port)
{
    ScalerHdmiHdcp14Tx2CheckDownstreamCapability(port);

    if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_SUPPORTED(port) == _TRUE) {
        CLR_HDMI_HDCP14_TX2_AUTH_START(port);
        ADD_HDMI_hdcp14_auth_cnt_STAT(port);
        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_AUTH_1);
        hdcp_out(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, 0x01);
        SET_HDMI_HDCP14_TX2_ENCRYPT_AUTH_GO(port);
        HDCPTX_WARIN("RealTX:p[%d] HDCP1x Go", (unsigned int)port);
        HDCPTX_WARIN("(round=%d)\n", (unsigned int)GET_HDMI_hdcp14_auth_cnt_STAT(port));
    }
    return 0;
}
void ScalerHDmiHdcp14_send_rid_list(unsigned char port)
{
    if (g_detect_cts_device) {
        HDCPTX_WARIN("RealTX:HDCP14 g_detect_cts_device mode\n");
        repeater_temp[0] = BSTATUS_DEVICE_COUNT(1) | BSTATUS_MAX_DEVS_EXCEEDED(0) | BSTATUS_MAX_CASCADE_EXCEEDED(0) | BSTATUS_DEPTH(1) | BSTATUS_HDMI_MODE(1); //hdmi mode bit12(0x1000)
    }
    else {
        HDCPTX_WARIN("RealTX: normal mode hdcp_hdmi_data=%d\n", (unsigned int)ScalerHdcpGetHdmiMode());
        if (ScalerHdcpGetHdmiMode() != 0) {
            repeater_temp[0] = BSTATUS_DEVICE_COUNT(1) | BSTATUS_MAX_DEVS_EXCEEDED(0) | BSTATUS_MAX_CASCADE_EXCEEDED(0) | BSTATUS_DEPTH(1) | BSTATUS_HDMI_MODE(1); //hdmi mode bit12(0x1000)
        }
        else {
            repeater_temp[0] = BSTATUS_DEVICE_COUNT(1) | BSTATUS_MAX_DEVS_EXCEEDED(0) | BSTATUS_MAX_CASCADE_EXCEEDED(0) | BSTATUS_DEPTH(1); //hdmi mode bit12(0x1000)
        }
    }
    repeater_temp[1] = RX_INFO_DEVICE_COUNT(1) | RX_INFO_DEPTH(1);

    // DownStream is Sink Only , fill-in the sink's ReceiverId to the ReceiverID_List directly.
    HdcpTxStoredReceiverId(g_pucHdmiHdcp14Tx2HdcpBksv, 1);
    SET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port);
}

void ScalerHDmiHdcp14_setting_hdcp_repeater_status(unsigned char port, unsigned char *Bstatus)
{
    repeater_temp[0] = (((Bstatus[1] & (~0x7)) | (GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEPTH(port) + 1)) << 8)
                       | ((Bstatus[0] & 0x8) | ((GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEVICE_COUNT(port) + 1) & 0x7F));
    //hdcp22 bit4~bit8 device count bit9~bit11 depth
    repeater_temp[1] = RX_INFO_DEVICE_COUNT(GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEVICE_COUNT(port) + 1) |
                       RX_INFO_DEPTH(GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEPTH(port) + 1) |
                       (((Bstatus[0] >> 7) & 0x1) << 3) | (((Bstatus[1] >> 3) & 0x1) << 2);
}

//--------------------------------------------------
// Description  : Hdmi Tx HDCP1.4 Auth
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2Handler(unsigned char port)
{
    unsigned char bStartAuth = _FALSE;
    unsigned char ucdata;

    if ((GET_HDMI_HDCP14_TX2_AUTH_STATE(port) == _HDMI_TX_HDCP_STATE_IDLE) && (GET_HDMI_HDCP14_TX2_AUTH_START(port) == 0)) {
        SET_HDMI_HDCP14_TX2_AUTH_START(port);
        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
    }
    if (GET_HDMI_HDCP14_TX2_AUTH_STATE(port) == _HDMI_TX_HDCP_STATE_AUTH_DONE) {
        if (hdcp14tx_cal_log_print) {
            //HDCPTX_WARIN("RealTX: Bypass AuthDone Event from Hdcp14\n");
        }
        SET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port, _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_PASS);
    }
#if 0
    if ((GET_HDMI_HDCP14_TX2_AUTH_STATE(port) >= _HDMI_TX_HDCP_STATE_AUTH_2)
            && (GET_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port) == 1)) {
        if (GET_HDMI_MAC_TX2_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_LINK_ON) {
            if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == 0) {
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_8_reg, 0x40);

                ScalerHdmiMacTx2HdcpEncryptSignalProc(port, _ENABLE);

                HDCPTX_WARIN("RealTX: HDCP14 - Encrypt *******\n");

            }
        }
        else {
            if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == 1) {
                ScalerHdmiMacTx2HdcpEncryptSignalProc(port, _DISABLE);
                HDCPTX_WARIN("RealTX: HDCP14 - Discryption\n");
            }
        }
    }
#endif

    switch (GET_HDMI_HDCP14_TX2_AUTH_STATE(port)) {
        case _HDMI_TX_HDCP_STATE_IDLE:
            if (GET_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
                HDCPTX_WARIN("RealTX: HDCP14 State - Idle\n");
                hdcp14_wait_v_status = 0;
                // FOR Special Case of CTS TE
                if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port) != _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_HDCP_CAP_CHANGE) {
                    CLR_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port);
                }

                CLR_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port);
                CLR_HDMI_HDCP14_TX2_DOWNSTREAM_R0_TIMEOUT(port);
                CLR_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_TIMEOUT(port);
                CLR_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port);
                CLR_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_BIT(port);
                CLR_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port);
                CLR_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port);
                CLR_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port);

                CLR_HDMI_HDCP14_TX2_ENCRYPT_AUTH_GO(port);
                hdcp_out(HDMITX20_MAC0_SCHEDULER_7_reg, 0x0B);
#if 0
                core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port);
                core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_WAITING_FOR_V_READY << 8 | port);
                core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY << 8 | port);
                core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX_HDCP_STATE_AUTH_DONE << 8 | port);
                core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX_HDCP_STATE_AUTH_1_COMPARE_R0 << 8 | port);
#else
                hdcp14_start_time[port] = 0;
                hdcp14_diff_ms = 0;
                hdcp14_verify_ri_cnt[port] = 0;
                hdcp14_verify_ri[port] = 0;
                hdcp14_verify_pj[port] = 0;
#endif

                ScalerHdmiHdcp14Tx2ResetInfo(port);

            }

            if (GET_HDMI_HDCP14_TX2_AUTH_START(port) == _TRUE) {
                HDCPTX_WARIN("Scaler_auth_start_check\n");
                Scaler_auth_start_check(port);
            }
            break;

        case _HDMI_TX_HDCP_STATE_AUTH_1:
            if (GET_HDMI_HDCP14_TX2_ENCRYPT_AUTH_GO(port) == _TRUE) {
                HDCPTX_WARIN("RealTX:HDCP_STATE_AUTH_1\n");
                if (ScalerHdcpGetHdmiMode() != 0) {
                    // HDMI mode
                    g_subAddr[0] = 0x42;
                    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x01, &ucdata) == _SUCCESS) {
                        if ((ucdata & 0x10) == 0x10) {
                            bStartAuth = _TRUE;
                            HDCPTX_WARIN("HDCPTX : r pass ucdata=%d\n", (unsigned int)ucdata);
                        }
                        else {
                            // Waiting for RX
                            bStartAuth = _FALSE;
                            ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
                            HDCPTX_WARIN("HDCPTX : r fail ucdata=%d\n", (unsigned int)ucdata);
                        }
                    }
                    else {
                        // Waiting for RX
                        bStartAuth = _FALSE;
                        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
                        HDCPTX_WARIN("HDCPTX: r fail\n");
                    }
                }
                else {
                    // DVI mode
                    g_subAddr[0] = 0x40;
                    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x01, &ucdata) == _SUCCESS) {
                        bStartAuth = _TRUE;
                        HDCPTX_WARIN("HDCPTX DVI: pass ucdata=%d\n", (unsigned int)ucdata);
                    }
                    else {
                        // Waiting for RX
                        bStartAuth = _FALSE;
                        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
                        HDCPTX_WARIN("HDCPTX DVI: fail ucdata=%d\n", (unsigned int)ucdata);
                    }
                }
            }
            if (hdcp14tx_func_test) {
                bStartAuth = _TRUE;
            }
            if (bStartAuth == _TRUE) {
                CLR_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port);
                if (ScalerHdmiHdcp14Tx2Auth1Proc(port) == _SUCCESS) {
                    HDCPTX_WARIN("RealTX: HDCP1x Auth1 Pass\n");

                    ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_AUTH_1_COMPARE_R0);
                }
                else {
                    //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT(port);
                    SET_HDMI_HDCP14_TX2_AUTH_START(port);

                    HDCPTX_WARIN("RealTX[p%d]: HDCP14 Self-reauth for Auth1 Fail !!!\n", (unsigned int)port);
                    ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
                }
            }

            break;

        case _HDMI_TX_HDCP_STATE_AUTH_1_COMPARE_R0:

            if (GET_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
                CLR_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port);

                // Wait 100ms to read R0'
                //ScalerTimerActiveTimerEvent(SEC(0.11), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_WAITING_TO_READ_R0);
                //core_timer_event_addTimerEvent(110, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX_HDCP_STATE_AUTH_1_COMPARE_R0 << 8 | port, 0);
                mdelay(110);
                SET_HDMI_HDCP14_TX2_DOWNSTREAM_R0_TIMEOUT(port);
            }

            if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_R0_TIMEOUT(port) == _TRUE) {
                if (ScalerHdmiHdcp14Tx2Auth1CompareR0(port) == _SUCCESS) {
                    HDCPTX_WARIN("RealTX: HDCP1x R0 Pass\n");

                    if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port) == _TRUE) {
                        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_AUTH_2);
                        //for cts 1B-03 item, need read bcaps between read r0 and enable encryption
                        ScalerHdmiHdcp14Tx2PollingStatus();
                    }
                    else {
                        if (GET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port) == _FALSE) {
                            ScalerHDmiHdcp14_send_rid_list(port);
                        }

                        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_AUTH_DONE);

                        SET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port, _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_PASS);
                    }
                }
                else {
                    //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT(port);
                    SET_HDMI_HDCP14_TX2_AUTH_START(port);

                    HDCPTX_WARIN("RealTX: HDCP14 Self-reauth for Compare R0 Fail\n");
                    ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
                }
            }

            break;

        case _HDMI_TX_HDCP_STATE_AUTH_2:
            if (GET_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
                CLR_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port);
                hdcp14_wait_v_status = 1;
#if 0 //todo :use timer event replace
                core_timer_event_addTimerEvent(5000, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_WAITING_FOR_V_READY << 8 | port, 0);
                core_timer_event_addTimerEvent(250, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY << 8 | port, 0);
#endif
                hdcp14_start_time[port] = rtk_timer_misc_90k_ms();
                mdelay(250);
                SET_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port);
            }

            if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port) == _TRUE) {
                CLR_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port);
                CLR_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_BIT(port);

                if (ScalerHdmiHdcp14Tx2PollingStatus() == _HDCP_1_4_TX_V_READY) {
                    SET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_BIT(port);
                    HDCPTX_WARIN("HDCP14 v ready\n");
                }
                else {
                    //core_timer_event_addTimerEvent(100, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY << 8 | port, 0);
                    mdelay(100);
                    SET_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port);
                }
            }

            if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_BIT(port) == _TRUE) {
                //ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY);
                //ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX2_HDCP_WAITING_FOR_V_READY);
#if 0
                core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_WAITING_FOR_V_READY << 8 | port);
                core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY << 8 | port);
#endif
                if (ScalerHdmiHdcp14Tx2CheckBinfo(port) == _TRUE) {
                    if (ScalerHdmiHdcp14Tx2Auth2Proc(port) == _SUCCESS) {
                        hdcp14_wait_v_status = 0;
                        HDCPTX_WARIN("RealTX: HDCP1x Auth2 Pass\n");

                        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_AUTH_DONE);

                        SET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port, _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_PASS);
                    }
                    else {
                        hdcp14_wait_v_status = 0;
                        HDCPTX_WARIN("RealTX: HDCP1x Auth2 Fail\n");
                        //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT(port);
                        SET_HDMI_HDCP14_TX2_AUTH_START(port);
                        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
                    }
                }
                else {
                    hdcp14_wait_v_status = 0;
                    ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
                    SET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port, _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_DEVICE_DEPTH_MAX);
                }
            }

            //check verify v time not over 5s
            if (hdcp14_start_time[port] != 0) {
                hdcp14_diff_ms = hdcptx_timer_diff_90k_ms_counter(hdcp14_start_time[port]);
                if (hdcp14_diff_ms > 5000) {
                    SET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_TIMEOUT(port);
                }
            }

            if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_TIMEOUT(port) == _TRUE) {
                HDCPTX_WARIN("RealTX: HDCP1x V Timeout\n");
                hdcp14_wait_v_status = 0;
                //todo
                //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY << 8 | port);

                //ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX2_HDCP_POLLING_V_READY);

                //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT(port);
                SET_HDMI_HDCP14_TX2_AUTH_START(port);

                ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
            }

            break;


        case _HDMI_TX_HDCP_STATE_AUTH_DONE:

            if (GET_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
                HDCPTX_WARIN("RealTX: HDCP1x Done\n");
                hdcp14_wait_v_status = 0;

                CLR_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port);
                CLR_HDMI_hdcp14_auth_cnt_STAT(port);
                //SET_TX_MAIN_MODULE_HDCP_ERR(0);
                //CLR_HDMI_MAC_TX2_HDCP_RETRY_COUNT(port);

                // SET_HDMI_TX2_STREAM_SOURCE(_HDMI_TX2_SOURCE_CLONE);

                // ScalerTimerActiveTimerEvent(SEC(2), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP_VERIFY_RI);

                // hdcp_out(P13_F1_HDMI_HPD_CTRL1, 0x02);

                // Set HDCP1.4 rekey win to 44T (steal case)
                hdcp_out(HDMITX20_MAC0_SCHEDULER_7_reg, 0x2B);

                //delay 10ms set enc ready
                mdelay(_HDMI_TX_HDCP_ENCRYPT_DATA_TIMEOUT);
                SET_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port);

                hdcp14_verify_ri_cnt[port] = _HDMI_TX_HDCP_VERIFY_RI_TIMEOUT;
                hdcp14_verify_pj[port] = hdcp14_verify_ri[port] = rtk_timer_misc_90k_ms();

                //_HDMI_TX_HDCP_ENCRYPT_DATA_TIMEOUT
                // core_timer_event_addTimerEvent(_HDMI_TX_HDCP_ENCRYPT_DATA_TIMEOUT, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX_HDCP_STATE_AUTH_DONE<<8 | port, 0);
            }

            if (hdcp14_verify_ri[port] != 0 && (GET_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port) == 1)) {
                hdcp14_diff_ms = hdcptx_timer_diff_90k_ms_counter(hdcp14_verify_ri[port]);
                if (hdcp14_diff_ms > hdcp14_verify_ri_cnt[port]) {
                    ScalerHdmiHdcp14Tx2VerifyRiEvent(port);
                }
            }

            /*
            if(ScalerMcuIICHdmiBurstRead(0x74, 0x01, 0x0A, 0x01, pData) == 0)
            {
                 //return 1;
            }
            */
            //ever 16 frame to check pj, 1000ms/vfreq*16
            if( GET_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port) == _TRUE){
                if(hdcp14_verify_pj[port]!=0 && (GET_HDMI_HDCP14_TX2_ENCRYPT_DATA_TIMEOUT(port) == 1)){
                    hdcp14_diff_ms = hdcptx_timer_diff_90k_ms_counter(hdcp14_verify_pj[port]);
                    if(hdcp14_diff_ms > (1000/(GET_HDCP_TX_VFreq(port)/10))*16){
                        ScalerHdmiHdcp14Tx2VerifyPj(port);
                    }
                }
            }

            //HDCPTX_WARIN("RealTX: Pj = %d\n", (unsigned int)hdcp_in_byte(P56_A0_HDCP14_ENGINE_38));
            // HDCPTX_WARIN("RealTX: Pj' = %d\n", (unsigned int)pData[0]);

            //core_timer_event_addTimerEvent(_HDMI_TX_HDCP_VERIFY_RI_TIMEOUT, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI<<8 | port, 0);
            if (GET_HDMI_HDCP14_TX2_AUTH_START(port) == _TRUE) {
                HDCPTX_WARIN("RealTX[p%d]: TX2 HDCP1x Done To Reauth\n", (unsigned int)port);
                ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
            }

            break;

        default:

            break;
    }
}


//--------------------------------------------------
// Description  : Check Downstream HDCP Capability
// Input Value  :
// Output Value : 1 (HDCP ACK)
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2CheckDownstreamCapability(unsigned char port)
{
    unsigned char Bcaps;
    CLR_HDMI_HDCP14_TX2_DOWNSTREAM_SUPPORTED(port);
    CLR_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port);
    CLR_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port);

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    Bcaps = _BIT6;
    if (Bcaps != 0)
#else
    // Read HDCP capability from HDCP Slave of DownStream
    g_subAddr[0] = 0x40;
    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x01, &Bcaps) == _SUCCESS)
#endif
    {
        //#ifdef HDCP14_TX_DEBUG
        HDCPTX_WARIN("HDCP14 Capability:Bcaps=%x\n", (unsigned int)Bcaps);
        //#endif
        if ((Bcaps & _BIT1) == _BIT1) {
            HDCPTX_WARIN("HDCP14 sink support 1_1 feature\n");
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port);
        }

        // Check if Rx is repeater
        if ((Bcaps & _BIT6) == _BIT6) {
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port);
        }
        /*
        if((pData[0] & _BIT7) == _BIT7)
        {
            hdcp_mask(P56_08_SCHEDULER_0, ~_BIT6, _BIT6);
        }
        else
        {
            hdcp_mask(P56_08_SCHEDULER_0, ~_BIT6, 0x00);
        }
        */
        SET_HDMI_HDCP14_TX2_DOWNSTREAM_SUPPORTED(port);
        return _TRUE;
    }
    else {
        HDCPTX_WARIN("HDCP14 CheckDownstreamCapability fail\n");
        return _FALSE;
    }
}



//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 1
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2Auth1Proc(unsigned char port)
{
    unsigned char Bcaps;
    // Enable DP Tx HDCP Tx function .
    //hdcp_mask(PB_17_DP_HDCP_CONTROL, ~_BIT7, _BIT7);
    //hdcp_mask(HDMITX21_MAC_HDMI21_TX_CTRL, ~_BIT7, _BIT7);
    HDCPTX_WARIN("RealTX:in Auth1Proc\n");

    // Step1: Read Bcap from DPCD

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    Bcaps = _BIT6;
#else
    // Check if Downstream's HDCP is capable
    g_subAddr[0] = 0x40;
    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x01, &Bcaps) == _SUCCESS)
#endif
    {
        // Check if Rx is repeater
        if ((Bcaps & _BIT6) == _BIT6) {
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port);
            hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, ~(_BIT6), _BIT6);
        }
        else {
            CLR_HDMI_HDCP14_TX2_DOWNSTREAM_REPEATER(port);
            hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, ~(_BIT6), 0x00);
        }

        // Due to HW Bug, set HDCP Cipher mode to "New Mode" [1]
        // Double Check HDCP1.1 feature & Set AC Mode Config. [0]
        if ((Bcaps & _BIT1) == _BIT1) {
            hdcp_mask(HDMITX20_MAC1_HDMI_SPDIF_CONTROL2_reg, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));
            SET_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port);
        }
        else {
            hdcp_mask(HDMITX20_MAC1_HDMI_SPDIF_CONTROL2_reg, ~(_BIT1 | _BIT0), _BIT1);
            CLR_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port);
        }
#if 0
        // Set HW's Hdcp1.1 feature if HDMI MNT or HDCP1.1 Feature Support
        if (/*(GET_HDMI_MAC_TX2_SUPPORT() == 1) ||*/ (GET_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port) == _TRUE)) {
            hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, ~(_BIT7), _BIT7);
        }
        else {
            hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, ~(_BIT7), 0x00);
        }
#endif
        hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, ~(_BIT7), _BIT7);

        HDCPTX_WARIN("RealTX: TX2 Bcaps=%d\n", (unsigned int)Bcaps);
    }
    // Step3: Generate An [3]
    hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_8_reg, ~(_BIT3), _BIT3);

    // Step4: Write Akey and write Aksv
    ScalerHdmiHdcp14Tx2Auth1WriteAnAkey(port);

    // Step2: Check Bksv
    HDCPTX_WARIN("RealTX:Check Bksv\n");
    if (ScalerHdmiHdcp14Tx2Auth1CheckBksv(port) == _FAIL) {
        HDCPTX_WARIN("RealTX: HDCP14 Self-reauth for Bksv illegal !!!\n");

        return _FAIL;
    }

    // Step5: Geneate Km,Ks,M0 and R0
    if (ScalerHdmiHdcp14Tx2Auth1GenKm(port) == _FAIL) {
        return _FAIL;
    }

    return _SUCCESS;
}



//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 1 -Check Bksv
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2Auth1CheckBksv(unsigned char port)
{
    unsigned char bResult = _FAIL;
    unsigned char pucTemp[5] = {0};
    unsigned char i = 0;
    unsigned char countsOfOne = 0;

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    memcpy(pucTemp, g_Bksv, 5);
#else
    g_subAddr[0] = 0x00;
    // Read Bksv
    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 5, pucTemp) != _SUCCESS) {
        return _FAIL;
    }
#endif
    if (hdcp14tx_cal_log_print) {
        for (i = 0; i < 5; i++) {
            HDCPTX_WARIN("bksv[%d] = %d  ", (unsigned int)i, (unsigned int)pucTemp[i]);
        }
        HDCPTX_WARIN("\n");
    }
    for (i = 0; i < 5; i++) {
        if ((pucTemp[i] & _BIT0) == _BIT0) {
            countsOfOne++;
        }
        if ((pucTemp[i] & _BIT1) == _BIT1) {
            countsOfOne++;
        }
        if ((pucTemp[i] & _BIT2) == _BIT2) {
            countsOfOne++;
        }
        if ((pucTemp[i] & _BIT3) == _BIT3) {
            countsOfOne++;
        }
        if ((pucTemp[i] & _BIT4) == _BIT4) {
            countsOfOne++;
        }
        if ((pucTemp[i] & _BIT5) == _BIT5) {
            countsOfOne++;
        }
        if ((pucTemp[i] & _BIT6) == _BIT6) {
            countsOfOne++;
        }
        if ((pucTemp[i] & _BIT7) == _BIT7) {
            countsOfOne++;
        }
    }
    //HDCPTX_WARIN( "countsOfOne = %d \n", (unsigned int)countsOfOne);
    if (countsOfOne == 20) {
        bResult = _SUCCESS;
        HDCPTX_WARIN("RealTX: CheckBksv PASS\n");

        if ((g_pucHdmiHdcp14Tx2HdcpBksv[0] != 0x00) || (g_pucHdmiHdcp14Tx2HdcpBksv[1] != 0x00) || (g_pucHdmiHdcp14Tx2HdcpBksv[2] != 0x00) || (g_pucHdmiHdcp14Tx2HdcpBksv[3] != 0x00) || (g_pucHdmiHdcp14Tx2HdcpBksv[4] != 0x00)) {
            if ((g_pucHdmiHdcp14Tx2HdcpBksv[0] != pucTemp[0]) || (g_pucHdmiHdcp14Tx2HdcpBksv[1] != pucTemp[1]) || (g_pucHdmiHdcp14Tx2HdcpBksv[2] != pucTemp[2]) || (g_pucHdmiHdcp14Tx2HdcpBksv[3] != pucTemp[3]) || (g_pucHdmiHdcp14Tx2HdcpBksv[4] != pucTemp[4])) {
                // Different KSV , re-auth
                HDCPTX_WARIN("RealTX: Hdcp14 Set Re-auth Event due to Bksv Different\n");

                SET_HDMI_HDCP14_TX2_DOWNSTREAM_EVENT(port, _HDMI_TX_HDCP_DOWNSTREAM_AUTH_EVENT_REAUTH);
            }
        }

        for (i = 0; i < 5; i++) {
            g_pucHdmiHdcp14Tx2HdcpBksv[i] = pucTemp[i];
        }
    }
    else {
        bResult = _FAIL;
        HDCPTX_WARIN("RealTX: CheckBksv FAIL\n");
    }

    return bResult;
}


//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 1- Gen Aksv
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2Auth1WriteAnAkey(unsigned char port)
{
    unsigned char ucI = 0;
    unsigned char An_data[8];

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_10_reg, ~(_BIT0), _BIT0);
    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_15_reg, g_An[0]);
    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_16_reg, g_An[1]);
    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_17_reg, g_An[2]);
    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_18_reg, g_An[3]);

    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_11_reg, g_An[4]);
    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_12_reg, g_An[5]);
    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_13_reg, g_An[6]);
    hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_14_reg, g_An[7]);
#else
    if (HdcpPollingFlagProc(5, HDMITX20_MAC0_HDCP14_ENGINE_10_reg, _BIT4, 1) == _FAIL) {
        HDCPTX_WARIN("HdcpPollingFlagProc failed \n");
        return;
    }
#endif
    An_data[0] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_14_reg);
    An_data[1] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_13_reg);
    An_data[2] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_12_reg);
    An_data[3] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_11_reg);

    An_data[4] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_18_reg);
    An_data[5] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_17_reg);
    An_data[6] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_16_reg);
    An_data[7] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_15_reg);

    if (hdcp14tx_cal_log_print) {
        for (ucI = 0; ucI < 8; ucI++) {
            HDCPTX_WARIN("An_data = %d\n", (unsigned int)An_data[ucI]);
        }
        for (ucI = 0; ucI < 5; ucI++) {
            HDCPTX_WARIN("AKSV = %d\n", (unsigned int)hdcp_14_tx_key[AKSV_START + ucI]);
        }
    }
#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    return;
#else

    g_subAddr[0] = 0x18;
    memcpy(&g_subAddr[1], An_data, 8);
    // Write An to Downstream
    ScalerMcuHwIICHDCP14BurstWrite(_HDCP_ADDRESS, g_subAddr, 9);

    // FW_TRACE_LOG_ITEM: [7]
    if (GET_HDMI_HDCP14_TX2_DOWNSTREAM_1_1_SUPPORTED(port) == 1) {
        unsigned char AInfo = 0x02;
        g_subAddr[0] = 0x15;
        g_subAddr[1] = AInfo;
        ScalerMcuHwIICHDCP14BurstWrite(_HDCP_ADDRESS, g_subAddr, 2);
    }

    // Write Aksv to Downstream
    g_subAddr[0] = 0x10;
    memcpy(&g_subAddr[1], &hdcp_14_tx_key[AKSV_START], 5);
    ScalerMcuHwIICHDCP14BurstWrite(_HDCP_ADDRESS, g_subAddr, 6);
#endif
}


//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 1 -Gen Km
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2Auth1GenKm(unsigned char port)
{
    unsigned char bResult = _FAIL;
    unsigned char uci = 0x00;
    unsigned char ucj = 0x00;

    // Key process

    // Reset Km accumulation
    hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, ~(_BIT2), _BIT2);
#ifdef HDCP14_TX_DEBUG
    for (uci = 0; uci < 5; uci++) {
        HDCPTX_WARIN("pucTemp[%d] = %d	", (unsigned int)uci, (unsigned int)g_pucHdmiHdcp14Tx2HdcpBksv[uci]);
    }
    HDCPTX_WARIN("\n");
#endif

    // Download DPK
    for (uci = 0; uci < 5; uci++) {
        for (ucj = 0; ucj < 8; ucj++) {
            unsigned char bit_num = _BIT0 << ucj;
            if ((g_pucHdmiHdcp14Tx2HdcpBksv[uci] & bit_num) == bit_num) {
#if(_HW_HDMI_HDCP14_KEY_TYPE == _HDCP14_KEY_320)
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_29_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 0]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_28_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 1]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_27_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 2]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_39_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 3]);

                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_34_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 4]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_33_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 5]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_32_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 6]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_31_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 8 + 7]);
#else
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_29_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 7 + 0]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_28_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 7 + 1]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_27_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 7 + 2]);

                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_34_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 7 + 3]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_33_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 7 + 4]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_32_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 7 + 5]);
                hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_31_reg, hdcp_14_tx_key[HDCPTX14_KEY_START + (uci * 8 + ucj) * 7 + 6]);
#endif
                hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, ~(_BIT3), _BIT3);
                while (hdcp_in_bit(HDMITX20_MAC0_HDCP14_ENGINE_10_reg, _BIT7) == 0x00) {
                    //HDCPTX_WARIN( "wait HW finish current DPK accumulation.. \n");
                }
            }
        }
    }

    // Turn on computation [4]
    hdcp_mask(HDMITX20_MAC0_HDCP14_ENGINE_8_reg, ~(_BIT4), _BIT4);

    // Km = Akey over Bksv
    // (Ks,M0,R0) = Km,Repeater||An

    if (HdcpPollingFlagProc(5, HDMITX20_MAC0_AUDIO_FIFO_STATUS_reg, _BIT2, 1) == _SUCCESS) {
        bResult = _SUCCESS;
        HDCPTX_WARIN("RealTX: DPK download finish\n");
    }
#ifdef HDCP14_TX_DEBUG

    unsigned char g_pucHdmiHdcp14Tx2HdcpKm[8];

    // Read out Km
    g_pucHdmiHdcp14Tx2HdcpKm[0] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_55_reg) ^ 0x9D;
    g_pucHdmiHdcp14Tx2HdcpKm[1] = ~(hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_54_reg)) ^hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_55_reg);
    g_pucHdmiHdcp14Tx2HdcpKm[2] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_53_reg) ^hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_54_reg);
    g_pucHdmiHdcp14Tx2HdcpKm[3] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_52_reg)^hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_53_reg);

    g_pucHdmiHdcp14Tx2HdcpKm[4] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_59_reg)^hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_52_reg);
    g_pucHdmiHdcp14Tx2HdcpKm[5] = ~(hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_58_reg))^hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_59_reg);
    g_pucHdmiHdcp14Tx2HdcpKm[6] = ~(hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_57_reg))^hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_58_reg);
#ifdef HDCP14_TX_DEBUG
    for (uci = 0; uci < 8; uci++) {
        HDCPTX_WARIN("NK[%d] = %d\n", (unsigned int)uci, (unsigned int)g_pucHdmiHdcp14Tx2HdcpKm[uci]);
    }
    HDCPTX_WARIN("\n");
#endif

#endif
    // Read out M0
    g_pucHdmiHdcp14Tx2HdcpM0[0] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_47_reg) ^ 0x9D;
    g_pucHdmiHdcp14Tx2HdcpM0[1] = ~(hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_46_reg) ^ hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_47_reg));
    g_pucHdmiHdcp14Tx2HdcpM0[2] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_45_reg) ^ hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_46_reg);
    g_pucHdmiHdcp14Tx2HdcpM0[3] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_44_reg) ^ hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_45_reg);
    g_pucHdmiHdcp14Tx2HdcpM0[4] = hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_51_reg) ^ hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_44_reg);
    g_pucHdmiHdcp14Tx2HdcpM0[5] = ~(hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_50_reg) ^ hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_51_reg));
    g_pucHdmiHdcp14Tx2HdcpM0[6] = ~(hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_49_reg) ^ hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_50_reg));
    g_pucHdmiHdcp14Tx2HdcpM0[7] = ~(hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_48_reg) ^ hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_49_reg));
#ifdef HDCP14_TX_DEBUG
    for (uci = 0; uci < 8; uci++) {
        HDCPTX_WARIN("M0[%d] = %d\n", (unsigned int)uci, (unsigned int)g_pucHdmiHdcp14Tx2HdcpM0[uci]);
    }
    HDCPTX_WARIN("\n");
#endif
    return bResult;
}



//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 1 Compare R0 and R0'
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2Auth1CompareR0(unsigned char port)
{
    unsigned char ucCompareCounter = 0;
    unsigned char R0_data[2];


    HDCPTX_WARIN("RealTX: Start Compare R0\n");
    do {
#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
        R0_data[0] = g_R0[0];
        R0_data[1] = g_R0[1];
#else
        // Read R0'
        g_subAddr[0] = 0x08;
        if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x02, R0_data) != _SUCCESS) {
            R0_data[0] = 0x00;
            R0_data[1] = 0x00;

            HDCPTX_WARIN("RealTX: TX2 read fail\n");
            return _FAIL;
        }
#endif

        // Compare R0 and R0'
        if ((hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_37_reg) == R0_data[0]) &&
                (hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_36_reg) == R0_data[1])) {
            HDCPTX_WARIN("RealTX: R0 pass\n");

            return _SUCCESS;
        }
        HDCPTX_WARIN("RealTX: R0[0]= %d\n", (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_37_reg));
        HDCPTX_WARIN("RealTX: R0[1]= %d\n", (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_36_reg));
        HDCPTX_WARIN("RealTX: R0'[0]= %d\n", (unsigned int)R0_data[0]);
        HDCPTX_WARIN("RealTX: R0'[1]= %d\n", (unsigned int)R0_data[1]);

        HDCPTX_WARIN("RealTX: R0 FAIL\n");

        //ScalerTimerDelayXms(2);
        mdelay(2);
        ucCompareCounter++;
    } while (ucCompareCounter < 3);

    return _FAIL;
}

bool hdcp14tx_compute_v(unsigned char is_big_endian, unsigned char *ksv_list, unsigned int list_len,
                        unsigned char *bstatus, unsigned char *m0, unsigned char *v)
{
    bool ret = false;
    unsigned char *msg = NULL;
    unsigned int msg_len = 0;
    int ksv_cnt = list_len / 5;
    int i = 0;
    int j = 0;
    unsigned int index = 0;
   // unsigned char is_big_endian = 0;//big/little endian;

    if (!ksv_list && list_len) {
        return false;
    }
    if ((list_len % 5) != 0) {
        return false;
    }

    if (!bstatus || !m0 || !v) {
        return false;
    }
    msg_len = list_len + 2 + 8;
    msg = (unsigned char *)kmalloc(msg_len, GFP_KERNEL);
    if (!msg) {
        return false;
    }

    if (ksv_list) {
        for (i = 0; i < ksv_cnt; i++) {
            unsigned char *ptr = ksv_list + i * 5;
            if (is_big_endian) {
                for (j = 4; j >= 0; j--) {
                    //rtd_pr_hdcp_info("%02x\n", ptr[j]);
                    msg[index++] = ptr[j];
                }
            }
            else {
                memcpy(msg + index, ptr, 5);
                index += 5;
            }
        }
    }

    if (is_big_endian) {
        msg[index++] = bstatus[1];
        msg[index++] = bstatus[0];
    }
    else {
        memcpy(msg + index, bstatus, 2);
        index += 2;
    }

    if (is_big_endian) {
        for (j = 7; j >= 0; j--) {
            msg[index++] = m0[j];
        }
    }
    else {
        memcpy(msg + index, m0, 8);
        index += 8;
    }

    ret = Sha1(msg, msg_len, v);

#ifdef HDCP14_TX_DEBUG
    HDCPTX_WARIN("\n the sha1 value is: \n");
    for (i = 0; i < 20; i += 4) {
        HDCPTX_WARIN("%d	", msg);
    }
    HDCPTX_WARIN("\n");
#endif


    kfree(msg);
    return ret;
}

//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 2
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2Auth2Proc(unsigned char port)
{
#ifdef HDCP_HW_SPEED_UP
    unsigned char ucCompareCounter = 0;

    do {
        // Load SHA-1 Input Data
        ScalerHdmiHdcp14Tx2Auth2WriteShaInput(port);

        // Compare V and V'
        if (ScalerHdmiHdcp14Tx2Auth2CompareV(port) == _SUCCESS) {
            return _SUCCESS;
        }
        ucCompareCounter++;
    } while (ucCompareCounter < 3);
    return 1;
#else
    unsigned char ucDownstreamDeviceCount = GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEVICE_COUNT(port);
    unsigned short usKsvFifoLength;
    unsigned char hdcp_v[20];
    unsigned char hdcp_v_sink[20];
    unsigned short i;
    unsigned char Bstatus[2];

    Bstatus[0] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port);
    Bstatus[1] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port);

    usKsvFifoLength = ucDownstreamDeviceCount * 5;

    memset(pucHdmiTx2HdcpKsvFifoTemp, 0, _HDCP_TX_DEVICE_COUNT_MAX * 5);

    g_usHdmiHdcp14Tx2KsvFifoIndex = 0;
    g_usHdmiMacTx2HdcpKsvFifoLength = ucDownstreamDeviceCount + 1;
    //g_usHdmiHdcp14Tx2KsvFifoIndex = g_usHdmiMacTxHdcp14KsvFifoIndex;
    //g_usHdmiMacTxHdcp14KsvFifoIndex += usKsvFifoLength;
    if (hdcp14tx_func_test || hdcp14tx_sha_test) {
        memcpy(pucHdmiTx2HdcpKsvFifoTemp, g_fake_ksv_list, usKsvFifoLength);
    }
    else {
        // Read KSV FIFO List from Downstream Device
        g_subAddr[0] = 0x43;
        if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, usKsvFifoLength, pucHdmiTx2HdcpKsvFifoTemp) != _SUCCESS) {
            return 1;
        }
    }
    // Prepare KSV List and Sink's BKSV
    if(usKsvFifoLength < _HDCP_TX_DEVICE_COUNT_MAX * 5){
        memcpy(g_pucHdmiTxHdcpKsvFifo, pucHdmiTx2HdcpKsvFifoTemp, usKsvFifoLength);
        memcpy(&g_pucHdmiTxHdcpKsvFifo[usKsvFifoLength], g_pucHdmiHdcp14Tx2HdcpBksv, 5);
    }else{
        memcpy(g_pucHdmiTxHdcpKsvFifo, pucHdmiTx2HdcpKsvFifoTemp, (_HDCP_TX_DEVICE_COUNT_MAX-1)*5);
        memcpy(&g_pucHdmiTxHdcpKsvFifo[(_HDCP_TX_DEVICE_COUNT_MAX-1)*5], g_pucHdmiHdcp14Tx2HdcpBksv, 5);
    }
    if (hdcp14tx_cal_log_print) {
        HDCPTX_WARIN("usKsvFifoLength is:%d", usKsvFifoLength);
        HDCPTX_WARIN("g_pucHdmiTxHdcpKsvFifo is: \n");
        for (i = 0; i < usKsvFifoLength; i++) {
            HDCPTX_WARIN("%x ", g_pucHdmiTxHdcpKsvFifo[i]);
        }
        HDCPTX_WARIN("\n");
        HDCPTX_WARIN("Bstatus0=%x Bstatus1=%x\n", Bstatus[0], Bstatus[1]);
        for (i = 0; i < 8; i++) {
            HDCPTX_WARIN("g_pucHdmiHdcp14Tx2HdcpM0[%d]:%x  ", i, g_pucHdmiHdcp14Tx2HdcpM0[i]);
        }
    }
    // --------------------------------------------------
    hdcp14tx_compute_v(0, g_pucHdmiTxHdcpKsvFifo, usKsvFifoLength, Bstatus, g_pucHdmiHdcp14Tx2HdcpM0, hdcp_v);
    if (hdcp14tx_func_test || hdcp14tx_sha_test) {
        memcpy(hdcp_v_sink, g_fake_sha1_value, SHA1_OUTPUT_SIZE);
    }
    else {
        g_subAddr[0] = 0x20;
        if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, SHA1_OUTPUT_SIZE, hdcp_v_sink) != _SUCCESS) {
            return 1;
        }
    }
    if (hdcp14tx_cal_log_print || hdcp14tx_sha_log_print) {
        HDCPTX_WARIN("the sha1 value is: \n");
        for (i = 0; i < 20; i += 4) {
            HDCPTX_WARIN("%x %x %x %x ", hdcp_v[i + 3], hdcp_v[i + 2], hdcp_v[i + 1], hdcp_v[i]);
        }
        HDCPTX_WARIN("\n");
        HDCPTX_WARIN("the hdcp_v_sink value is: \n");
        for (i = 0; i < 20; i += 4) {
            HDCPTX_WARIN("%x %x %x %x", hdcp_v_sink[i], hdcp_v_sink[i + 1], hdcp_v_sink[i + 2], hdcp_v_sink[i + 3]);
        }
        HDCPTX_WARIN("\n");
    }

    // Compare V with V'
    for (i = 0; i < 20; i += 4) {
        if ((hdcp_v_sink[i] != hdcp_v[i + 3]) ||
                (hdcp_v_sink[i + 1] != hdcp_v[i + 2]) ||
                (hdcp_v_sink[i + 2] != hdcp_v[i + 1]) ||
                (hdcp_v_sink[i + 3] != hdcp_v[i + 0])) {
            HDCPTX_WARIN("CompareV failed , i = %d\n", (unsigned int)i);
            return 1;
        }
    }
    HDCPTX_WARIN("ScalerHdcp14Tx2Auth2CompareV success!!\n");

    HdcpTxStoredReceiverId(g_pucHdmiTxHdcpKsvFifo, g_usHdmiMacTx2HdcpKsvFifoLength);
    return _SUCCESS;
#endif
}

#ifdef HDCP_HW_SPEED_UP
//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 2-Wrtie Data to SHA input
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2Auth2WriteShaInput(unsigned char port)
{
    unsigned char ucDownstreamDeviceCount = GET_HDMI_HDCP14_TX2_DOWNSTREAM_DEVICE_COUNT(port);
    unsigned short usKsvFifoLength;
#ifdef HDCP14_TX_DEBUG
    unsigned char uci;
#endif
    unsigned short usShaInputBitNumbers = 0;
    volatile unsigned char usKsvLengthTemp = 0;
    unsigned char HdcpKsvFifoTemp[4];
    usKsvFifoLength = ucDownstreamDeviceCount * 5;

    memset(pucHdmiTx2HdcpKsvFifoTemp, 0, _HDCP_TX_DEVICE_COUNT_MAX * 5);

    g_usHdmiHdcp14Tx2KsvFifoIndex = 0;
    g_usHdmiMacTx2HdcpKsvFifoLength = ucDownstreamDeviceCount + 1;
    //g_usHdmiHdcp14Tx2KsvFifoIndex = g_usHdmiMacTxHdcp14KsvFifoIndex;
    //g_usHdmiMacTxHdcp14KsvFifoIndex += usKsvFifoLength;
    if (hdcp14tx_func_test) {
        memcpy(pucHdmiTx2HdcpKsvFifoTemp, g_fake_ksv_list, usKsvFifoLength);
    }
    else {
        // Read KSV FIFO List from Downstream Device
        g_subAddr[0] = 0x43;
        if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, usKsvFifoLength, pucHdmiTx2HdcpKsvFifoTemp) != _SUCCESS) {
            return;
        }
    }
    // Prepare KSV List and Sink's BKSV
    memcpy(g_pucHdmiTxHdcpKsvFifo, pucHdmiTx2HdcpKsvFifoTemp, usKsvFifoLength);
    memcpy(&g_pucHdmiTxHdcpKsvFifo[usKsvFifoLength], g_pucHdmiHdcp14Tx2HdcpBksv, 5);

    // --------------------------------------------------
    // Total unsigned char numbers of SHA-1 input = the length of KSV FIFO + the length of B_info + the length of M0'
    usShaInputBitNumbers = (usKsvFifoLength + 2 + 8) * 8;

    // Reset SHA Block counter
    hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~(_BIT3), _BIT3);

    // Start to Write SHA-1 Transform Input
    hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~(_BIT3), 0x00);

    // Initialize The Index of KSV FIFO List
    usKsvLengthTemp = 0;

    if (usKsvFifoLength == 0) {
        return;
    }
    // Write the KSV List into SHA-1 Transform Input
    do {
        HdcpKsvFifoTemp[0] =  g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp];
        HdcpKsvFifoTemp[1] =  g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp + 1];
        HdcpKsvFifoTemp[2] =  g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp + 2];
        HdcpKsvFifoTemp[3] =  g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp + 3];
#ifdef HDCP14_TX_DEBUG
        for (uci = 0; uci < 4; uci++) {
            HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
        }

#endif

        hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

        // Enable write 32-unsigned char data to SHA-1 block
        hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

        usKsvLengthTemp += 4;

        if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
            ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
        }
    } while ((usKsvFifoLength - usKsvLengthTemp) >= 4);

    // Write the remaining KSV, B info and M0' into SHA-1 Transform Input
    switch (usKsvFifoLength - usKsvLengthTemp) {
        case 1:

            HdcpKsvFifoTemp[0] =  g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp];
            HdcpKsvFifoTemp[1] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port);
            HdcpKsvFifoTemp[2] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port);
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[0];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }
            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[1];
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[2];
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[3];
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[4];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[5];
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[6];
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[7];
            HdcpKsvFifoTemp[3] = 0x80;
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            break;

        case 2:

            HdcpKsvFifoTemp[0] = g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp];
            HdcpKsvFifoTemp[1] = g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp + 1];
            HdcpKsvFifoTemp[2] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port);
            HdcpKsvFifoTemp[3] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port);
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[0];
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[1];
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[2];
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[3];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[4];
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[5];
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[6];
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[7];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = 0x80;
            HdcpKsvFifoTemp[1] = 0x00;
            HdcpKsvFifoTemp[2] = 0x00;
            HdcpKsvFifoTemp[3] = 0x00;
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            break;

        case 3:

            HdcpKsvFifoTemp[0] = g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp];
            HdcpKsvFifoTemp[1] = g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp + 1];
            HdcpKsvFifoTemp[2] = g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usKsvLengthTemp + 2];
            HdcpKsvFifoTemp[3] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port);
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port);
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[0];
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[1];
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[2];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[3];
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[4];
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[5];
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[6];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[7];
            HdcpKsvFifoTemp[1] = 0x80;
            HdcpKsvFifoTemp[2] = 0x00;
            HdcpKsvFifoTemp[3] = 0x00;
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            break;

        default:

            HdcpKsvFifoTemp[0] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port);
            HdcpKsvFifoTemp[1] = GET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port);
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[0];
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[1];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }
            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[2];
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[3];
            HdcpKsvFifoTemp[2] = g_pucHdmiHdcp14Tx2HdcpM0[4];
            HdcpKsvFifoTemp[3] = g_pucHdmiHdcp14Tx2HdcpM0[5];
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));
            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            HdcpKsvFifoTemp[0] = g_pucHdmiHdcp14Tx2HdcpM0[6];
            HdcpKsvFifoTemp[1] = g_pucHdmiHdcp14Tx2HdcpM0[7];
            HdcpKsvFifoTemp[2] = 0x80;
            HdcpKsvFifoTemp[3] = 0x00;
#ifdef HDCP14_TX_DEBUG
            for (uci = 0; uci < 4; uci++) {
                HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
            }
#endif

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));
            // Enable write 32-bit data to SHA-1 block
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;

            if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
                ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
            }

            break;
    }

    if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 60) {
        HdcpKsvFifoTemp[0] = 0x00;
        HdcpKsvFifoTemp[1] = 0x00;
        HdcpKsvFifoTemp[2] = 0x00;
        HdcpKsvFifoTemp[3] = 0x00;
#ifdef HDCP14_TX_DEBUG
        for (uci = 0; uci < 4; uci++) {
            HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
        }
#endif

        hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

        // Enable write 32-bit data to SHA-1 block
        hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

        usKsvLengthTemp += 4;

        if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) == 0) {
            ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
        }
    }

    // Write 0x00 into the remaining SHA-1 Transform Input space
    if ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) <= 56) {
        while ((usKsvLengthTemp % _HDMI_HDCP_SHA1_INPUT_SIZE) < 56) {

            HdcpKsvFifoTemp[0] = 0x00;
            HdcpKsvFifoTemp[1] = 0x00;
            HdcpKsvFifoTemp[2] = 0x00;
            HdcpKsvFifoTemp[3] = 0x00;

            hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

            // Enable write 32-bit data to SHA-1 block
            //hdcp_mask(PB_30_SHA_CONTRL, ~_BIT0, _BIT0);
            hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

            usKsvLengthTemp += 4;
        }
        // The input length is equal to 2^64
        HdcpKsvFifoTemp[0] = 0x00;
        HdcpKsvFifoTemp[1] = 0x00;
        HdcpKsvFifoTemp[2] = 0x00;
        HdcpKsvFifoTemp[3] = 0x00;

        hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));

        // Enable write 32-bit data to SHA-1 block
        hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

        HdcpKsvFifoTemp[0] = 0x00;
        HdcpKsvFifoTemp[1] = 0x00;
        HdcpKsvFifoTemp[2] = HIBYTE(usShaInputBitNumbers);
        HdcpKsvFifoTemp[3] = LOBYTE(usShaInputBitNumbers);
#ifdef HDCP14_TX_DEBUG
        for (uci = 0; uci < 4; uci++) {
            HDCPTX_WARIN("HdcpKsvFifoTemp[%d] = %d\n", (unsigned int)uci, (unsigned int)HdcpKsvFifoTemp[uci]);
        }
#endif

        hdcp_out(HDCP14_SPEEDUP_SHA_DATA_reg, (unsigned int)uchar_array_to_uint(HdcpKsvFifoTemp));
        // Enable write 32-bit data to SHA-1 block
        hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~_BIT0, _BIT0);

        usKsvLengthTemp += 8;

        ScalerHdmiHdcp14Tx2Auth2ShaRun((usKsvLengthTemp == _HDMI_HDCP_SHA1_INPUT_SIZE) ? 1 : 0);
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 2 -SHA circuit Run
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2Auth2ShaRun(BOOLEAN bIsFirst)
{
    // Set First Run For SHA-1 Circuit
    hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~(_BIT1), ((bIsFirst == 1) ? _BIT1 : 0x00));

    // Run SHA-1 Circuit
    hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~(_BIT2), _BIT2);

    if (HdcpPollingFlagProc(5, HDCP14_SPEEDUP_SHA_CONTRL_reg, _BIT4, 1) == _SUCCESS) {
        HDCPTX_WARIN("SHA Calculation Done \n");
    }

    // Disable First Run and Disable SHA-1 Circuit
    hdcp_mask(HDCP14_SPEEDUP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1), 0x00);
    HDCPTX_WARIN("hdcp_in_bit(reg,bit_num) = %d\n", (unsigned int)hdcp_in_bit(HDCP14_SPEEDUP_SHA_CONTRL_reg, _BIT4));

}

//--------------------------------------------------
// Description  : Hdmi Tx HDCP Auth Part 2 -Compare V and V'
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2Auth2CompareV(unsigned char port)
{
    unsigned int sha_data_temp = 0;
    unsigned char sha1_data[SHA1_OUTPUT_SIZE];
    unsigned char i = 0;

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    memcpy(sha1_data, g_fake_sha1_value, SHA1_OUTPUT_SIZE);
#else
    g_subAddr[0] = 0x20;
    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, SHA1_OUTPUT_SIZE, sha1_data) != _SUCCESS) {
        return 1;
    }
#endif
    // hdcp_mask(PBB_5B_DP_HDCP_TX_SHA_CTRL, ~(_BIT7 | _BIT6 | _BIT5 | _BIT0), ((ucIndex - ucSha1VHx) << 5));

    // Compare V with V'
    for (i = 0; i < 20; i += 4) {
        sha_data_temp = ScalerGetData(HDCP14_SPEEDUP_SHA_0 + i);
        if (*((unsigned char *)&sha_data_temp + 0) != sha1_data[i]      ||
                *((unsigned char *)&sha_data_temp + 1) != sha1_data[i + 1] ||
                *((unsigned char *)&sha_data_temp + 2) != sha1_data[i + 2] ||
                *((unsigned char *)&sha_data_temp + 3) != sha1_data[i + 3]) {
            HDCPTX_WARIN("CompareV failed , i = %d\n", (unsigned int)i);
            return 1;
        }
    }
    HDCPTX_WARIN("ScalerHdcp14Tx2Auth2CompareV success!!\n");

    HdcpTxStoredReceiverId(g_pucHdmiTxHdcpKsvFifo, g_usHdmiMacTx2HdcpKsvFifoLength);
    //set hdcp repeater message to RX, temp[0] is hdcp14, temp[2] is hdcp2
    //newbase_hdmi_hdcp_repeater_update_downstream_topology(repeater_temp, &g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex], usKsvFifoLength);
    //newbase_hdmi_hdcp_repeater_enable(1);

#ifdef HDCP14_TX_DEBUG
    HDCPTX_WARIN("\n the sha1 value is: \n");
    for (i = 0; i < 20; i += 4) {
        HDCPTX_WARIN("%d  ", (unsigned int)ScalerGetData(HDCP14_SPEEDUP_SHA_0 + i));
    }
    HDCPTX_WARIN("\n");
#endif

    return 0;
}

#endif
//--------------------------------------------------
// Description  : Check Bstatus of Downstream port
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2CheckBinfo(unsigned char port)
{
    unsigned char Bstatus[2];
#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    Bstatus[0] = 0x03;
    Bstatus[1] = 0x02;
#else

    // Read Bstatus(Device Count and Depth)
    g_subAddr[0] = 0x41;
    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x02, Bstatus) != _SUCCESS) {
        return _FALSE;
    }
#endif

    // Record Bstatus of Downstream
    SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port, Bstatus[0]);
    SET_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port, Bstatus[1]);

    // Check if MAX_DEVS_EXCEEDED or MAX_CASCADE_EXCEEDED is 1 and then abort authentication
    if (((Bstatus[0] & _BIT7) == _BIT7) || ((Bstatus[1] & _BIT3) == _BIT3) || (Bstatus[0] == 0x00)) {
        // Re-auth due to CTS 1B-04b
        if ((Bstatus[0] & 0x7F) == 0x00) {
            //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT(port);
            SET_HDMI_HDCP14_TX2_AUTH_START(port);
        }

        return _FALSE;
    }
    //hdcp14 bit0~bit6 device count   bit8~bit10 depth
    ScalerHDmiHdcp14_setting_hdcp_repeater_status(port, Bstatus);
    HDCPTX_WARIN("[HDCP14]bstatus0 = %d bstatus1 = %d\n", (unsigned int)Bstatus[0], (unsigned int)Bstatus[1]);
    return _TRUE;
}

//--------------------------------------------------
// Description  : Change HDCP Auth State
// Input Value  : Target HDCP Auth State
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2ChangeAuthState(unsigned char port, EnumHdmiTxHDCPAuthState enumHDCPAuthState)
{
    SET_HDMI_HDCP14_TX2_AUTH_STATE(port, enumHDCPAuthState);
    SET_HDMI_HDCP14_TX2_AUTH_STATE_CHANGE(port);
}


//--------------------------------------------------
// Description  : Change HDCP Auth State to Compare R0
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2TimeoutToReadR0Event(unsigned char port)
{
    HDCPTX_WARIN("RealTX: TX2 Timeout To Read R0\n");
    SET_HDMI_HDCP14_TX2_DOWNSTREAM_R0_TIMEOUT(port);
}

//--------------------------------------------------
// Description  : Waiting for V ready of downstream
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2TimeoutForVReadyEvent(unsigned char port)
{
    SET_HDMI_HDCP14_TX2_DOWNSTREAM_V_READY_TIMEOUT(port);
}

//--------------------------------------------------
// Description  : Polling V ready of downstream
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2PollingVReadyEvent(unsigned char port)
{
    SET_HDMI_HDCP14_TX2_DOWNSTREAM_POLLING_V_READY(port);
}

//--------------------------------------------------
// Description  : Polling Rx status of Downstream
// Input Value  :
// Output Value :
//--------------------------------------------------
EnumHdmiHdcpStatus ScalerHdmiHdcp14Tx2PollingStatus(void)
{
    unsigned char Bcaps;

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    Bcaps = _BIT5;
#else
    g_subAddr[0] = 0x40;
    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 1, &Bcaps) != _SUCCESS) {
        return _HDCP_1_4_TX_NONE;
    }
#endif
    HDCPTX_WARIN("HDCP14 v Bcaps=%x\n", Bcaps);

    if ((Bcaps & _BIT5) == _BIT5) {
        return _HDCP_1_4_TX_V_READY;
    }

    return _HDCP_1_4_TX_NONE;
}

//--------------------------------------------------
// Description  : Reset HDCP TX Info
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2ResetInfo(unsigned char port)
{
    CLR_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEVICE(port);
    CLR_HDMI_HDCP14_TX2_DOWNSTREAM_BINFO_DEPTH(port);
}

//--------------------------------------------------
// Description  : Verify Ri
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp14Tx2VerifyRiEvent(unsigned char port)
{
    unsigned char pucRiTemp[2] = {0};
    g_subAddr[0] = 0x08;

    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x02, pucRiTemp) == _SUCCESS) {
        if ((hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_37_reg) == pucRiTemp[0]) && (hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_36_reg) == pucRiTemp[1])) {
            //pm_printk(LOGGER_INFO, "[HDCP] Ri pass\n");
            CLR_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port);
            hdcp14_verify_ri_cnt[port] = 2000;
            hdcp14_verify_ri[port] = rtk_timer_misc_90k_ms();
            //  core_timer_event_addTimerEvent(_HDMI_TX_HDCP_VERIFY_RI_TIMEOUT, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port, 0);
            if (hdcp14tx_cal_log_print) {
                HDCPTX_WARIN("[HDCP] Ri[0] = %x\n", (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_37_reg));
                HDCPTX_WARIN("[HDCP] Ri[1] = %x\n", (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_36_reg));
                HDCPTX_WARIN("[HDCP] Ri'[0] = %x\n", (unsigned int)pucRiTemp[0]);
                HDCPTX_WARIN("[HDCP] Ri'[1] = %x\n", (unsigned int)pucRiTemp[1]);
            }
            return;
        }
        else {
            HDCPTX_WARIN("[HDCP] Ri fail\n");
            HDCPTX_WARIN("[HDCP] Ri[0] = %x\n", (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_37_reg));
            HDCPTX_WARIN("[HDCP] Ri[1] = %x\n", (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_36_reg));
            HDCPTX_WARIN("[HDCP] Ri'[0] = %x\n", (unsigned int)pucRiTemp[0]);
            HDCPTX_WARIN("[HDCP] Ri'[1] = %x\n", (unsigned int)pucRiTemp[1]);

            ADD_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port);
#if 0
            HDCPTX_WARIN("RealTX:[HDCP] vFreq=%d\n", (unsigned int)stTxRepeaterOutputTimingInfo.vFreq);
            core_timer_event_addTimerEvent(((1000 + (stTxRepeaterOutputTimingInfo.vFreq / 10) / 2) / (stTxRepeaterOutputTimingInfo.vFreq / 10) + 1), MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port, 0);
#endif
            HDCPTX_WARIN("RealTX:[HDCP] vFreq=%d\n", GET_HDCP_TX_VFreq(port));
            hdcp14_verify_ri_cnt[port] = ((1000 + (GET_HDCP_TX_VFreq(port) / 10) / 2) / (GET_HDCP_TX_VFreq(port) / 10) + 1);
            hdcp14_verify_ri[port] = rtk_timer_misc_90k_ms();
            //core_timer_event_addTimerEvent(((1000 + GET_HDMI_MAC_TX2_INPUT_FRAME_RATE() / 2) / GET_HDMI_MAC_TX2_INPUT_FRAME_RATE() + 1), MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI<<8 | port, 0);
        }
    }
    else {

        hdcp14_verify_ri_cnt[port] = 50;//wati tx
        hdcp14_verify_ri[port] = rtk_timer_misc_90k_ms();
        //core_timer_event_addTimerEvent(50, MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port, 0);
        HDCPTX_WARIN("[HDCP] Ri read fail\n");

        ADD_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port);
    }

    if (GET_HDMI_HDCP14_TX2_RI_RETRY_COUNT(port) >= _HDMI_HDCP_MAX_RI_RETRY_COUNT) {
        SET_HDMI_HDCP14_TX2_AUTH_START(port);
        HDCPTX_WARIN("[HDCP] HDCP14 Self-reauth for Ri Fail !!!\n");
        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);

        // Set rekey window size 12
        hdcp_out(HDMITX20_MAC0_SCHEDULER_7_reg, 0x0B);
        // core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port);
    }
}

unsigned char readReceiverPj(void)
{
    unsigned char ri;
    g_subAddr[0] = 0x0A;

    if(ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 1, &ri) == _SUCCESS){
        return ri;
    }else{
        return 0;
    }
}
unsigned char ScalerHdmiHdcp14Tx2VerifyPj(unsigned char port)
{
    unsigned char T1, T2, R1, R2;

    T1 = (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_38_reg);
    R1 = readReceiverPj();
    if (hdcp14tx_cal_log_print){
        HDCPTX_WARIN( "RealTX:[HDCP] vFreq=%d\n", (unsigned int)GET_HDCP_TX_VFreq(port));
        HDCPTX_WARIN( "HDCP1 pj:%d\n", (unsigned int)(1000/(GET_HDCP_TX_VFreq(port)/10))*16);
        HDCPTX_WARIN( "[HDCP] T1 = %d\n", (unsigned int)T1);
        HDCPTX_WARIN( "[HDCP] R1 = %d\n", (unsigned int)R1);
    }
    pj_count = 0;
    while (T1 != R1)
    {
        T2 = (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_38_reg);
        R2 = readReceiverPj();
        // Check for stable Pj values
        if (T1 == T2 && R1 == R2)
        {
            pj_count++; // Count mismatches
            if (pj_count >= 3)
                break;
            T1 = T2;
            R1 = R2;
            // Wait for any of the values to change
            do
            {
                T2 = (unsigned int)hdcp_in_byte(HDMITX20_MAC0_HDCP14_ENGINE_38_reg);
                R2 =readReceiverPj();
            } while ((T2 == T1) && (R2 == R1));
        }
        T1 = T2;
        R1 = R2;
    }
    hdcp14_verify_pj[port] = rtk_timer_misc_90k_ms();
    if(pj_count > 3){
        SET_HDMI_HDCP14_TX2_AUTH_START(port);
        HDCPTX_WARIN("HDCP1 Self-reauth for PJ Fail !!!\n");
        ScalerHdmiHdcp14Tx2ChangeAuthState(port, _HDMI_TX_HDCP_STATE_IDLE);
        pj_count = 0;
    }
    return 0;
}

//--------------------------------------------------
// Description  : Check KSV
// Input Value  : None
// Output Value : None
//--------------------------------------------------
unsigned char ScalerHdmiHdcp14Tx2ReCheckKsv(unsigned char port)
{
    unsigned short usKsvFifoLength = 0x00;
    unsigned short usCount = 0x00;
    unsigned char Bksv[5];
    unsigned char Bcaps;
    unsigned char Bstatus[2];
    BOOLEAN bResult = 1;

    HDCPTX_WARIN("RealTX: Re-check\n");
    memset(pucHdmiTx2HdcpKsvFifoTemp, 0x00, _HDCP_TX_DEVICE_COUNT_MAX * 5);
    // Read Bksv
    g_subAddr[0] = 0x00;
    if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 5, Bksv) != _SUCCESS) {
        return 1;
    }

    if ((Bksv[0] == g_pucHdmiHdcp14Tx2HdcpBksv[0]) &&
            (Bksv[1] == g_pucHdmiHdcp14Tx2HdcpBksv[1]) &&
            (Bksv[2] == g_pucHdmiHdcp14Tx2HdcpBksv[2]) &&
            (Bksv[3] == g_pucHdmiHdcp14Tx2HdcpBksv[3]) &&
            (Bksv[4] == g_pucHdmiHdcp14Tx2HdcpBksv[4])) {
        g_subAddr[0] = 0x40;
        if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x01, &Bcaps) == _SUCCESS) {
            // Check if Rx is repeater
            if ((Bcaps & _BIT6) == _BIT6) {
                // Is repeater
                g_subAddr[0] = 0x41;
                if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, 0x02, Bstatus) == _FAIL) {
                    return 1;
                }
                usKsvFifoLength = Bstatus[0] * 5;
                g_subAddr[0] = 0x43;
                if (ScalerMcuHwIICHDCPBurstRead(_HDCP_ADDRESS, g_subaddr_len, g_subAddr, usKsvFifoLength, pucHdmiTx2HdcpKsvFifoTemp) == _FAIL) {
                    return 1;
                }
                for (usCount = 0; usCount < usKsvFifoLength; usCount++) {
                    if (pucHdmiTx2HdcpKsvFifoTemp[usCount] != g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp14Tx2KsvFifoIndex + usCount]) {
                        return 1;
                    }
                }
                bResult = 0;
                HDCPTX_WARIN("RealTX: Same KSV List\n");
            }
            else {
                // Not repeater
                bResult = 0;
                HDCPTX_WARIN("RealTX: Same KSV\n");
            }
        }
        else {
            bResult = 1;
        }
    }
    else {
        bResult = 1;
        HDCPTX_WARIN("RealTX: Diff KSV\n");
    }

    return bResult;
}

#if 0
unsigned char is_flash_burned_hdcp14_tx_key(void)
{
    unsigned char ret = 0;
    unsigned char buf[32];
    unsigned char align_offset = 0;
    if ((unsigned int)buf & 0xF) {
        align_offset = (unsigned int)buf % 0x10;
        align_offset = 0x10 - align_offset;
    }
    ret = rtk_check_hdcp_key(HDCP_1_4_TX, &buf[align_offset], 16);
    return ret;
}
#endif
void load_hdcp14_tx_key(void)
{
#ifdef HDCP14_DEFAULT_TXKEY
    HDCPTX_INFO("%s: hdcp14tx defalt key enable\n", __func__);
#else
#ifdef CONFIG_OPTEE_HDCP14
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char temp_buff[5];
    unsigned char countsOfOne = 0;

    if(optee_hdcp14_load_txkey(hdcp_14_tx_key, 325)<0){
        HDCPTX_ERR("load hdcp14 tx key fail");
        return;
    }

    memcpy(temp_buff, hdcp_14_tx_key, 5);
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            if ((temp_buff[i] & 1) == 1) {
                countsOfOne++;
            }
            temp_buff[i] >>= 1;
        }
    }
    if (countsOfOne == 20) {
        HDCPTX_INFO("%s: load hdcp14tx optee key success, countsOfOne=%d\n", __func__, countsOfOne);
    }else{
        HDCPTX_WARIN("%s: load hdcp14tx optee key fail, countsOfOne=%d\n", __func__, countsOfOne);
    }
#endif
#endif
}
EXPORT_SYMBOL(load_hdcp14_tx_key);

void hdcp14tx_init(void)
{
    unsigned char port;
#if 0
#ifdef HDCP14_DEFAULT_TXKEY
    tx14_key_align_offset = 0;
    //memcpy(hdcp_14_tx_key, hdcp14_default_txkey, 325);
    //CLR_HDMI_MAC_TX2_HDCP_NOT_BURN_KEY_STATUS(0);

    HDCPTX_WARIN("[HDCP]use hdcp14 tx default key\n");
    HDCPTX_WARIN("hdcp_14_tx_key = %x\n", (unsigned int)hdcp_14_tx_key[0]);
#else
    //todo ,load hdcp14 key
    if (memcmp("HDCP", hdcp_14_tx_key, 4) == 0) {
        HDCPTX_WARIN("[HDCP]load hdcp key ok\n");
    }
    else {
        HDCPTX_WARIN("[HDCP]not burn hdcp key\n");
    }
#endif
#endif
    for (port = 0; port < HDMI_TX_PORT_MAX_NUM; port++) {
        SET_HDMI_HDCP14_TX2_AUTH_STATE(port, _HDMI_TX_HDCP_STATE_IDLE);
        CLR_HDMI_HDCP14_TX2_AUTH_START(port);
    }
}
EXPORT_SYMBOL(hdcp14tx_init);

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
extern void HdmiTxInit(void);
extern void HDCP14_TX_TEST(void);

void test_hdcp14tx_fsm(void)
{
    unsigned char cnt = 10;
    unsigned char port = 0;

    HDCPTX_WARIN("test_hdcp14tx_fsm start\n");

#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
    // HdmiTxInit();
#endif
    rtd_maskl(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, ~(_BIT0), _BIT0); // HDCP reset

    SET_HDMI_HDCP14_TX2_AUTH_STATE(port, _HDMI_TX_HDCP_STATE_IDLE);
    CLR_HDMI_HDCP14_TX2_AUTH_START(port);
    while (--cnt) {
#ifdef CONFIG_ENABLE_HDCP14_TX_TEST
        HDCP14_TX_TEST();
#else
        ScalerHdmiHdcp14Tx2Handler(port);
#endif

    }

}
#endif

