/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2011>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdmiHdcp2Tx2Include.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#ifndef  _SCALERHDMIHDCP2TX2INCLUDE_H_
#define  _SCALERHDMIHDCP2TX2INCLUDE_H_

#include <hdcp/hdcp_common.h>

#define _TIME_OUT                  1

#define HDCP_TX_AUTH_MAX_CNT 8
#define HDCP14_TX_AUTH_MAX_CNT 20

#define _HDCP2_TX_DEVICE_COUNT_MAX                           32

extern unsigned char g_bHdmiHdcp2StateVerifySignatureTimeStatus[HDMI_TX_PORT_MAX_NUM];
extern unsigned char g_bHdmiHdcp2StateNoStoredKmTimestatus[HDMI_TX_PORT_MAX_NUM];
extern unsigned char g_bHdmiHdcp2StateStoredKmTimestatus[HDMI_TX_PORT_MAX_NUM];
extern unsigned char g_bHdmiHdcp2StateParingTimestatus[HDMI_TX_PORT_MAX_NUM];
extern unsigned char g_bHdmiHdcp2StateSkeTimestatus[HDMI_TX_PORT_MAX_NUM];
extern unsigned char g_bHdmiHdcp2StateLCTimestatus[HDMI_TX_PORT_MAX_NUM];
extern unsigned char g_bHdmiHdcp2StateEncTimestatus[HDMI_TX_PORT_MAX_NUM];
extern unsigned short hdcp2_auth_cnt[HDMI_TX_PORT_MAX_NUM] ;

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************

#define GET_HDMI_HDCP2_TX2_AUTH_STATE(port)                         (g_stHdmiHdcp2Tx2AuthInfo[port].b4AuthState)
#define SET_HDMI_HDCP2_TX2_AUTH_STATE(port,x)                        (g_stHdmiHdcp2Tx2AuthInfo[port].b4AuthState = (x))

#define GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthStateChange)
#define SET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthStateChange = _TRUE)
#define CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthStateChange = _FALSE)

#define GET_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Version)
#define SET_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Version = _TRUE)
#define CLR_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Version = _FALSE)

#define GET_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Capable)
#define SET_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Capable = _TRUE)
#define CLR_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Capable = _FALSE)

#define GET_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port)                 (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Repeater)
#define SET_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port)                 (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Repeater = _TRUE)
#define CLR_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port)                 (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp2Repeater = _FALSE)

#define GET_HDMI_HDCP2_TX2_AUTH_START(port)                         (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthStart)
#define SET_HDMI_HDCP2_TX2_AUTH_START(port)                         (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthStart = _TRUE)
#define CLR_HDMI_HDCP2_TX2_AUTH_START(port)                         (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthStart = _FALSE)

#define GET_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1CertificateTimeout)
#define SET_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1CertificateTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1CertificateTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_H_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1HprimeTimeout)
#define SET_HDMI_HDCP2_TX2_H_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1HprimeTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_H_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1HprimeTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_POLLIN_HPRIME_ENABLE(port)               (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdmiHdcp2PollinHEnable)
#define SET_HDMI_HDCP2_TX2_POLLIN_HPRIME_ENABLE(port)               (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdmiHdcp2PollinHEnable = _TRUE)
#define CLR_HDMI_HDCP2_TX2_POLLIN_HPRIME_ENABLE(port)               (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdmiHdcp2PollinHEnable = _FALSE)

#define GET_HDMI_HDCP2_TX2_PARING_TIMEOUT(port)                     (g_stHdmiHdcp2Tx2AuthInfo[port].b1ParingTimeout)
#define SET_HDMI_HDCP2_TX2_PARING_TIMEOUT(port)                     (g_stHdmiHdcp2Tx2AuthInfo[port].b1ParingTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_PARING_TIMEOUT(port)                     (g_stHdmiHdcp2Tx2AuthInfo[port].b1ParingTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1LprimeTimeout)
#define SET_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1LprimeTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1LprimeTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_WRITE_M_MSG_TIMEOUT(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1WriteMTimeout)
#define SET_HDMI_HDCP2_TX2_WRITE_M_MSG_TIMEOUT(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1WriteMTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_WRITE_M_MSG_TIMEOUT(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1WriteMTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port)                   (g_stHdmiHdcp2Tx2AuthInfo[port].usLcRetryCounter)
#define ADD_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port)                   (g_stHdmiHdcp2Tx2AuthInfo[port].usLcRetryCounter++)
#define CLR_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port)                   (g_stHdmiHdcp2Tx2AuthInfo[port].usLcRetryCounter = 0)

#define GET_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port)               (g_stHdmiHdcp2Tx2AuthInfo[port].b1EncryptDataTimeout)
#define SET_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port)               (g_stHdmiHdcp2Tx2AuthInfo[port].b1EncryptDataTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port)               (g_stHdmiHdcp2Tx2AuthInfo[port].b1EncryptDataTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_V_READY_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1VReadyTimeout)
#define SET_HDMI_HDCP2_TX2_V_READY_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1VReadyTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_V_READY_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1VReadyTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_POLLING_V_READY(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1PollingVReady)
#define SET_HDMI_HDCP2_TX2_POLLING_V_READY(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1PollingVReady = _TRUE)
#define CLR_HDMI_HDCP2_TX2_POLLING_V_READY(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1PollingVReady = _FALSE)

#define GET_HDMI_HDCP2_TX2_V_READY_BIT(port)                        (g_stHdmiHdcp2Tx2AuthInfo[port].b1VReadyBit)
#define SET_HDMI_HDCP2_TX2_V_READY_BIT(port)                        (g_stHdmiHdcp2Tx2AuthInfo[port].b1VReadyBit = _TRUE)
#define CLR_HDMI_HDCP2_TX2_V_READY_BIT(port)                        (g_stHdmiHdcp2Tx2AuthInfo[port].b1VReadyBit = _FALSE)

#define GET_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1LcAlreadySent)
#define SET_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1LcAlreadySent = _TRUE)
#define CLR_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1LcAlreadySent = _FALSE)

#define GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b5RxInfoDevice)
#define SET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port,x)               (g_stHdmiHdcp2Tx2AuthInfo[port].b5RxInfoDevice = ((x) & 0x1F))

#define GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT_EXCEED(port)         (g_stHdmiHdcp2Tx2AuthInfo[port].b1RxInfoDeviceExceed)
#define SET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT_EXCEED(port)         (g_stHdmiHdcp2Tx2AuthInfo[port].b1RxInfoDeviceExceed = _TRUE)
#define CLR_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT_EXCEED(port)         (g_stHdmiHdcp2Tx2AuthInfo[port].b1RxInfoDeviceExceed = _FALSE)

#define GET_HDMI_HDCP2_TX2_RXINFO_DEPTH(port)                       (g_stHdmiHdcp2Tx2AuthInfo[port].b3RxInfoDepth)
#define SET_HDMI_HDCP2_TX2_RXINFO_DEPTH(port,x)                      (g_stHdmiHdcp2Tx2AuthInfo[port].b3RxInfoDepth = ((x) & 0x07))

#define GET_HDMI_HDCP2_TX2_RXINFO_DEPTH_EXCEED(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1RxInfoDepthExceed)
#define SET_HDMI_HDCP2_TX2_RXINFO_DEPTH_EXCEED(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1RxInfoDepthExceed = _TRUE)
#define CLR_HDMI_HDCP2_TX2_RXINFO_DEPTH_EXCEED(port)                (g_stHdmiHdcp2Tx2AuthInfo[port].b1RxInfoDepthExceed = _FALSE)

#define GET_HDMI_HDCP2_TX2_RXINFO_HDCP20_REPEATER_DOWNSTREAM(port)  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp20RepeaterDownstream)
#define SET_HDMI_HDCP2_TX2_RXINFO_HDCP20_REPEATER_DOWNSTREAM(port)  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp20RepeaterDownstream = _TRUE)
#define CLR_HDMI_HDCP2_TX2_RXINFO_HDCP20_REPEATER_DOWNSTREAM(port)  (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp20RepeaterDownstream = _FALSE)

#define GET_HDMI_HDCP2_TX2_RXINFO_HDCP1_DOWNSTREAM(port)            (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp1DeviceDownstream)
#define SET_HDMI_HDCP2_TX2_RXINFO_HDCP1_DOWNSTREAM(port)            (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp1DeviceDownstream = _TRUE)
#define CLR_HDMI_HDCP2_TX2_RXINFO_HDCP1_DOWNSTREAM(port)            (g_stHdmiHdcp2Tx2AuthInfo[port].b1Hdcp1DeviceDownstream = _FALSE)

#define GET_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(port)                   (g_stHdmiHdcp2Tx2AuthInfo[port].ucAuthDownstreamEvent)
#define SET_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(port,x)                  (g_stHdmiHdcp2Tx2AuthInfo[port].ucAuthDownstreamEvent = (x))
#define CLR_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(port)                   (g_stHdmiHdcp2Tx2AuthInfo[port].ucAuthDownstreamEvent = _HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_NONE)

#define GET_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port)                 (g_stHdmiHdcp2Tx2AuthInfo[port].b1PairingInfoReady)
#define SET_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port)                 (g_stHdmiHdcp2Tx2AuthInfo[port].b1PairingInfoReady = _TRUE)
#define CLR_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port)                 (g_stHdmiHdcp2Tx2AuthInfo[port].b1PairingInfoReady = _FALSE)

#define GET_HDMI_HDCP2_TX2_CONTENT_STREAM_MANAGEMENT(port)          (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthContentStreamManage)
#define SET_HDMI_HDCP2_TX2_CONTENT_STREAM_MANAGEMENT(port)          (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthContentStreamManage = _TRUE)
#define CLR_HDMI_HDCP2_TX2_CONTENT_STREAM_MANAGEMENT(port)          (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthContentStreamManage = _FALSE)

#define GET_HDMI_HDCP2_TX2_SEQUENCE_NUMBER_M(port)                  (g_ulHdmiHdcp2Tx2SeqNumM)
#define ADD_HDMI_HDCP2_TX2_SEQUENCE_NUMBER_M(port)                  (g_ulHdmiHdcp2Tx2SeqNumM++)
#define CLR_HDMI_HDCP2_TX2_SEQUENCE_NUMBER_M(port)                  ((g_ulHdmiHdcp2Tx2SeqNumM) = 0)

#define GET_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1MprimeTimeout)
#define SET_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1MprimeTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port)                    (g_stHdmiHdcp2Tx2AuthInfo[port].b1MprimeTimeout = _FALSE)

#define GET_HDMI_HDCP2_TX2_POLLING_FAIL(port)                       (g_stHdmiHdcp2Tx2AuthInfo[port].b1PollingFail)
#define SET_HDMI_HDCP2_TX2_POLLING_FAIL(port)                       (g_stHdmiHdcp2Tx2AuthInfo[port].b1PollingFail = _TRUE)
#define CLR_HDMI_HDCP2_TX2_POLLING_FAIL(port)                       (g_stHdmiHdcp2Tx2AuthInfo[port].b1PollingFail = _FALSE)

#define GET_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port)                (g_bHdmiHdcp2Tx2StableCheck)
#define SET_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port)                (g_bHdmiHdcp2Tx2StableCheck = _TRUE)
#define CLR_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port)                (g_bHdmiHdcp2Tx2StableCheck = _FALSE)

#define GET_HDMI_HDCP2_TX2_AUTH_GO(port)                            (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthGoTimeout)
#define SET_HDMI_HDCP2_TX2_AUTH_GO(port)                            (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthGoTimeout = _TRUE)
#define CLR_HDMI_HDCP2_TX2_AUTH_GO(port)                            (g_stHdmiHdcp2Tx2AuthInfo[port].b1AuthGoTimeout = _FALSE)

#define GET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdcpReportRidList)
#define SET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdcpReportRidList = _TRUE)
#define CLR_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdcpReportRidList = _FALSE)

#define GET_HDMI_MAC_TX2_HDCP_NOT_BURN_KEY_STATUS(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdcpNotBurnKey)
#define SET_HDMI_MAC_TX2_HDCP_NOT_BURN_KEY_STATUS(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdcpNotBurnKey = _TRUE)
#define CLR_HDMI_MAC_TX2_HDCP_NOT_BURN_KEY_STATUS(port)                  (g_stHdmiHdcp2Tx2AuthInfo[port].b1HdcpNotBurnKey = _FALSE)

#define GET_RX_PHY_NOT_STABLE_CNT(port)                            (rx_phy_not_stable_cnt[port])
#define SET_RX_PHY_NOT_STABLE_CNT(port, value)                     (rx_phy_not_stable_cnt[port] = value)
#define ADD_RX_PHY_NOT_STABLE_CNT(port)                            (rx_phy_not_stable_cnt[port]++)
#define CLR_RX_PHY_NOT_STABLE_CNT(port)                            (rx_phy_not_stable_cnt[port] = 0)

#define GET_HDMI_HDCP2_TX_STATE_VERIFY_SIGNATURE_TIME_STATUS(port)                  (g_bHdmiHdcp2StateVerifySignatureTimeStatus[port])
#define SET_HDMI_HDCP2_TX_STATE_VERIFY_SIGNATURE_TIME_STATUS(port)                  (g_bHdmiHdcp2StateVerifySignatureTimeStatus[port] = _TIME_OUT)
#define CLR_HDMI_HDCP2_TX_STATE_VERIFY_SIGNATURE_TIME_STATUS(port)                  (g_bHdmiHdcp2StateVerifySignatureTimeStatus[port] = 0)

#define GET_HDMI_HDCP2_TX_STATE_NO_STORED_KM_TIME_STATUS(port)                  (g_bHdmiHdcp2StateNoStoredKmTimestatus[port])
#define SET_HDMI_HDCP2_TX_STATE_NO_STORED_KM_TIME_STATUS(port)                  (g_bHdmiHdcp2StateNoStoredKmTimestatus[port] = _TIME_OUT)
#define CLR_HDMI_HDCP2_TX_STATE_NO_STORED_KM_TIME_STATUS(port)                  (g_bHdmiHdcp2StateNoStoredKmTimestatus[port] = 0)

#define GET_HDMI_HDCP2_TX_STATE_STATE_STORED_KM_TIME_STATUS(port)                  (g_bHdmiHdcp2StateStoredKmTimestatus[port])
#define SET_HDMI_HDCP2_TX_STATE_STATE_STORED_KM_TIME_STATUS(port)                  (g_bHdmiHdcp2StateStoredKmTimestatus[port] = _TIME_OUT)
#define CLR_HDMI_HDCP2_TX_STATE_STATE_STORED_KM_TIME_STATUS(port)                  (g_bHdmiHdcp2StateStoredKmTimestatus[port] = 0)

#define GET_HDMI_HDCP2_TX_STATE_STATE_PAIRING_TIME_STATUS(port)                  (g_bHdmiHdcp2StateParingTimestatus[port])
#define SET_HDMI_HDCP2_TX_STATE_STATE_PAIRING_TIME_STATUS(port)                  (g_bHdmiHdcp2StateParingTimestatus[port] = _TIME_OUT)
#define CLR_HDMI_HDCP2_TX_STATE_STATE_PAIRING_TIME_STATUS(port)                  (g_bHdmiHdcp2StateParingTimestatus[port] = 0)

#define GET_HDMI_HDCP2_TX_STATE_SKE_TIME_STATUS(port)                  (g_bHdmiHdcp2StateSkeTimestatus[port])
#define SET_HDMI_HDCP2_TX_STATE_SKE_TIME_STATUS(port)                  (g_bHdmiHdcp2StateSkeTimestatus[port] = _TIME_OUT)
#define CLR_HDMI_HDCP2_TX_STATE_SKE_TIME_STATUS(port)                  (g_bHdmiHdcp2StateSkeTimestatus[port] = 0)

#define GET_HDMI_HDCP2_TX_STATE_LOCALITY_CHECK_TIME_STATUS(port)                  (g_bHdmiHdcp2StateLCTimestatus[port])
#define SET_HDMI_HDCP2_TX_STATE_LOCALITY_CHECK_TIME_STATUS(port)                  (g_bHdmiHdcp2StateLCTimestatus[port] = _TIME_OUT)
#define CLR_HDMI_HDCP2_TX_STATE_LOCALITY_CHECK_TIME_STATUS(port)                  (g_bHdmiHdcp2StateLCTimestatus[port] = 0)

#define GET_HDMI_HDCP2_TX_STATE_ENC_TIME_STATUS(port)                  (g_bHdmiHdcp2StateEncTimestatus[port])
#define SET_HDMI_HDCP2_TX_STATE_ENC_TIME_STATUS(port)                  (g_bHdmiHdcp2StateEncTimestatus[port] = _TIME_OUT)
#define CLR_HDMI_HDCP2_TX_STATE_ENC_TIME_STATUS(port)                  (g_bHdmiHdcp2StateEncTimestatus[port] = 0)

#define GET_HDMI_hdcp2_auth_cnt_STAT(port)                (hdcp2_auth_cnt[port])
#define SET_HDMI_hdcp2_auth_cnt_STAT(port, value)         (hdcp2_auth_cnt[port] = value)
#define ADD_HDMI_hdcp2_auth_cnt_STAT(port)                (hdcp2_auth_cnt[port]++)
#define CLR_HDMI_hdcp2_auth_cnt_STAT(port)                (hdcp2_auth_cnt[port] = 0)

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
#define _HDCP_2_2_TX_STATE_IDLE                             0x00
#define _HDCP_2_2_TX_STATE_AKE_INIT                         0x01
#define _HDCP22_AKE_SEND_CERT                               0x02
#define _HDCP_2_2_TX_STATE_VERIFY_SIGNATURE                 0x03
#define _HDCP_2_2_TX_STATE_NO_STORED_KM                     0x04
#define _HDCP_2_2_TX_STATE_STORED_KM                        0x05
#define _HDCP_2_2_TX_STATE_VERIFY_H_PRIME                   0x07
#define _HDCP_2_2_TX_STATE_PAIRING                          0x08
#define _HDCP_2_2_TX_STATE_LOCALITY_CHECK                   0x09
#define _HDCP_2_2_TX_STATE_SKE                              0x0A
#define _HDCP_2_2_TX_STATE_AUTHEN_DONE                      0x0B
#define _HDCP_2_2_TX_STATE_VERIFY_V_PRIME                   0x0C
#define _HDCP_2_2_TX_STATE_ENC                              0x0E
#define _HDCP_2_2_TX_STATE_POLLING_V_READY_TIME             0x10

#define _HDCP_2_2_DKEY_0 0x00
#define _HDCP_2_2_DKEY_1 0x01
#define _HDCP_2_2_DKEY_2 0x02

#define _HDCP_2_2_TX_NONE   0x00
#define _HDCP_2_2_TX_REAUTH_REQUEST     0x01
#define _HDCP_2_2_TX_V_READY        0x02

#define HDCP2_TX_READ_RX_TIMEOUT                    (0xFF)
#define HDCP_TX_RIV_HMAC_NOT_MATCH                 2

#define _HDMI_TX_HDCP2_LC_RETRY_MAX_NUMBER              (1024)

#define HDCP2_TX2_POLLING_HANDLE_MS                 20

#define VERIFY_SIGNATURE_ON_THE_CERT_TIMEOUT        (100)
#define VERIFY_INTEGRITY_OF_SRM_NO_STORED_TIMEOUT        (1000)
#define VERIFY_INTEGRITY_OF_SRM_STORED_TIMEOUT        (200)
#define PAIRING_TIMEOUT                             (200)
#define VERIFY_LPRIME_POLLING_TIMEOUT               (20)  //hdmi 20ms /IRealone 7ms
#define LPRIME_READ_MSG_TIME                        (2)  // for i2c 100K hz, read L' should take about 2 ms
#define VERIFY_LPRIME_TIMEOUT                   (VERIFY_LPRIME_POLLING_TIMEOUT + LPRIME_READ_MSG_TIME)
#define HDCP2_TX2_ENCRYPT_DATA_TIMEOUT                                                     (200)
#define HDCP2_TX2_POLLING_REAUTH_TIMEOUT                                                     (200)
#define HDCP2_TX2_POLLING_V_READY_TIME              (3000)
#define HDCP2_TX_KEY_SIZE           (800)

typedef enum {
    _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON,
    _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON
} HdmiTxModeStatus;

typedef enum {
    NULL_MSG = 1,
    AKE_INIT = 2,
    AKE_SEND_CERT = 3,
    AKE_NO_STORED_EKM = 4,
    AKE_STORED_EKM = 5,
    AKE_SEND_RRX = 6,
    AKE_SEND_H_PRIME = 7,
    AKE_SEND_PAIRING_INFO = 8,
    LC_INIT = 9,
    LC_SEND_L_PRIME = 10,
    SKE_SEND_EKS = 11,
    REP_AUTH_SEND_RXID_LIST = 12,
    RTT_READY = 13,
    RTT_CHALLENGE = 14,
    REPEATERAUTH_SEND_ACK = 15,
    REPEATERAUTH_STREAM_MANAGE = 16,
    REPEATERAUTH_STREAM_READY = 17,
    RECEIVER_AUTHSTATUS = 18,
    AKE_TRANSMITTER_INFO = 19,
    AKE_RECEIVER_INFO = 20,
} HDCPMsgId_t;

typedef enum {
    HDCP_TX_NO_ERROR = 0x0,
    HDCP_NO_TX_KEY = 0x1,
    HDCP_RX_NOT_READY = 0x2,
    HDCP_AKE_INIT_FAIL = 0x3,
    HDCP_AKE_TRANSITTER_INFO_FAIL = 0x4,
    HDCP_CERT_AND_RECEIVER_INFO_TIMEOUT = 0x5,
    HDCP_SIGNATURE_FAIL = 0x6,
    HDCP_NO_STOREKM_FAIL = 0x7,
    HDCP_STOREKM_FAIL = 0x8,
    HDCP_AKE_STOREKM_VERIFY_H_TIMEOUT = 0x9,
    HDCP_AKE_NO_STOREKM_VERIFY_H_TIMEOUT = 0x10,
    HDCP_HPRIME_FAIL = 0x11,
    HDCP_COMPUTE_H_FAIL = 0x12,
    HDCP_PAIRING_TIMEOUT = 0x13,
    HDCP_PARING_FAIL = 0x14,
    HDCP_VERIFY_L_TIMEOUT = 0x15,
    HDCP_LC_FAIL = 0x16,
    HDCP_SKE_PROC_FAIL = 0x17,
} HDCP_TYPE_ERROR;



typedef struct {
    unsigned char b4AuthState;
    unsigned char b1AuthStateChange;
    unsigned char b1Hdcp2Version;
    unsigned char b1Hdcp2Capable;
    unsigned char b1Hdcp2Repeater;
    unsigned char b1AuthStart;
    unsigned char b1CertificateTimeout;
    unsigned char b1HprimeTimeout;
    unsigned char b1HdmiHdcp2PollinHEnable;
    unsigned char b1ParingTimeout;
    unsigned char b1LprimeTimeout;
    unsigned char b1WriteMTimeout;
    unsigned short usLcRetryCounter;
    unsigned char b1EncryptDataTimeout;
    unsigned char b1VReadyTimeout;
    unsigned char b1PollingVReady;
    unsigned char b1VReadyBit;
    unsigned char b1LcAlreadySent;
    unsigned short b5RxInfoDevice;
    unsigned char b1RxInfoDeviceExceed;
    unsigned char b3RxInfoDepth;
    unsigned char b1RxInfoDepthExceed;
    unsigned char b1Hdcp20RepeaterDownstream;
    unsigned char b1Hdcp1DeviceDownstream;
    unsigned char ucAuthDownstreamEvent;
    unsigned char b1PairingInfoReady;
    unsigned char b1AuthContentStreamManage;
    unsigned char b1MprimeTimeout;
    unsigned char b1PollingFail;
    unsigned char b1AuthGoTimeout;
    unsigned char b1HdcpReportRidList;
    unsigned char b1HdcpNotBurnKey;
} StructHdcp2TxAuthInfo;


typedef enum
{
    SAMPLE_HDCP2_TX_KEY,
    USER_HDCP2_TX_KEY,
    NOT_BURN_TX_KEY,
} HDCP2_TX_KEY_STATUS_T;

//--------------------------------------------------
// Macro of HDCP 2 Error types State
//--------------------------------------------------
#define GET_HDCP2_TX_TYPE_ERROR_STATUS()                      (g_ucHdcp2TxErrorStatus)
#define SET_HDCP2_TX_TYPE_ERROR_STATUS(value)                      (g_ucHdcp2TxErrorStatus = value)
#define CLR_HDCP2_TX_TYPE_ERROR_STATUS()                      (g_ucHdcp2TxErrorStatus = 0)

//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************
extern unsigned char g_bHdmiHdcp2Tx2StableCheck;
extern volatile StructHdcp2TxAuthInfo g_stHdmiHdcp2Tx2AuthInfo[HDMI_TX_PORT_MAX_NUM];
extern unsigned char g_pucHdmiHdcp2Tx2ReceiverId[HDMI_TX_PORT_MAX_NUM][5];
extern volatile unsigned char g_ucHdcp2TxErrorStatus;
extern unsigned short rx_phy_not_stable_cnt[HDMI_TX_PORT_MAX_NUM];

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern void ScalerHdmiHdcp2Tx2ChangeAuthState(unsigned char port, unsigned char enumHdcp2AuthState);
extern unsigned char ScalerHdmiHdcp2Tx2CheckCapability(unsigned char port);

//repeater
extern void ScalerHdmiHdcp2Tx2TimeoutToReadCertificateEvent(unsigned char port);
extern void ScalerHdmiHdcp2Tx2TimeoutToEncyptDataEvent(unsigned char port);
extern void ScalerHdmiHdcp2Tx2TimeoutForVReadyEvent(unsigned char port);
extern void ScalerHdmiHdcp2Tx2PollingVReadyEvent(unsigned char port);
extern void ScalerHdmiHdcp2Tx2TimeoutToReadMPrimeEvent(unsigned char port);
extern void ScalerHdmiHdcp2Tx2ContentStreamManagementHandler(unsigned char port);


extern void ScalerHdmiHdcp2Tx2Handler(unsigned char port);
extern void ScalerHdmiHdcp2TxInit(void);
extern unsigned char ScalerHdmiHdcp2Tx2PollingStatus(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2ReadMessage(unsigned char port, unsigned short usLength, unsigned char *pucReadArray, unsigned short timeout);
extern unsigned char ScalerHdmiHdcp2Tx2WriteMessage(unsigned char port, unsigned short usLength, unsigned char *pucReadArray);
extern unsigned char ScalerHdmiHdcp2Tx2ReadData(unsigned char port, unsigned short usLength, unsigned char *pucReadArray, unsigned int timeout);
extern HdmiTxModeStatus GET_HDMI_MAC_TX_MODE_STATE(unsigned char port);
extern unsigned char ScalerHdmiMacTx2GetHdcpEncStatus(unsigned char port);
extern void ScalerHdmiMacTx2HdcpEncryptSignalProc(unsigned char port, unsigned char onOff);
extern HDCP2_TX_KEY_STATUS_T hdcp2_get_tx_key_status(void);
extern HDCP2_TX_KEY_STATUS_T hdcp2_isr_get_tx_key_status(void);
extern unsigned char is_flash_burned_tx_key(void);
extern void ScalerHdmiHdcp2TxSetAuthTimeOut(unsigned char port);
extern unsigned char ScalerHdmiHdcp2TxGetAuthTimeOut(unsigned char port);
extern void ScalerHdmiHdcp2TxClearAuthState(unsigned char port);
extern void load_hdcp22_tx_key(void);
#endif
