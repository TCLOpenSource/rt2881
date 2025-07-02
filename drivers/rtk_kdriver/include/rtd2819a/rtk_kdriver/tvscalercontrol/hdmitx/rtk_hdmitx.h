#ifndef _RTK_HDMITX_H_
#define _RTK_HDMITX_H_


//#include <rtk_kdriver/rtd_types.h>
#ifndef UINT8
typedef unsigned char  __UINT8;
#define UINT8 __UINT8
#endif
#ifndef UINT16
typedef unsigned short __UINT16;
#define UINT16 __UINT16
#endif
#ifndef UINT32
typedef unsigned int   __UINT32;
#define UINT32 __UINT32
#endif
/************************
// Number/Functoin Definition
//
*************************/

/************************
// Enum type define
//
*************************/


/************************
// data struture type define
//
*************************/
typedef struct
{
    UINT8 * pEdidBufAddr;
    UINT16 u16EdidLen;
}StructHDMITxEdidBufInfo;

// HDMITX repeater control event
typedef enum _TX_REPEATER_EVENT_TYPE{
    MSG_HDMITX_AP_TX_STATE_INITIAL=4,
    MSG_AP_HDMITX_TX_DISCONNECT,
    MSG_AP_HDMITX_TX_CONNECT,
    MSG_HDMITX_AP_RX_NO_SIGNAL,
    MSG_HDMITX_AP_TX_STATE_POWER_SAVING,
    MSG_HDMITX_AP_TX_STATE_DC_OFF,
    MSG_HDMITX_AP_TX_STATE_REINITIAL,
    MSG_HDMITX_AP_TX_CTS_MODE,
    //for match scaler index
    MSG_HDMITX_SCALER_TXRUN = 0x20,
    MSG_HDMITX_TO_SCALER_TX_SETTINGDONE,
}TX_REPEATER_EVENT_TYPE;

typedef enum _TX_OUT_MODE_SELECT_TYPE{
    TX_OUT_THROUGH_MODE=0,
    TX_OUT_SOURCE_MODE,
    TX_OUT_TEST_MODE,
    TX_OUT_USER_MODE,
    TX_OUT_MODE_NUM
}TX_OUT_MODE_SELECT_TYPE;

typedef enum _TX_INPUT_SRC_SELECT_TYPE{
    TX_INPUT_SOURCE_HDMIRX=0,
    TX_INPUT_SOURCE_DPRX,
    TX_INPUT_SOURCE_DTG,
    TX_INPUT_SOURCE_NUM
}TX_INPUT_SRC_SELECT_TYPE;


typedef struct
{
        UINT8 SCDC_0x35;
}StructHDMITxSCDCInfo;

// TX repeater mode type for TX support timing query
typedef enum _TX_REPEATER_OUT_MODE_SELECT_TYPE{
   TX_REPEATER_SOURCE_MODE=0,
   TX_REPEATER_THROUGH_MODE,
   TX_REPEATER_TEST_MODE,
   TX_REPEATER_USER_MODE,
}TX_REPEATER_OUT_MODE_SELECT_TYPE;

extern TX_OUT_MODE_SELECT_TYPE repeater_tx_out_mode;
#define ApSetTxHdmiTxOutputMode(x) (repeater_tx_out_mode = x)

//UINT8 APGetHdmiTxOutPutInfo(UINT8 hdmitx_port,StructHDMITxOutputTimingInfo *stTxOutputTimingInfo);
//UINT8 APGetHdmiTxEdid(UINT8 *len, UINT8* pEdidbuf);
#endif // #ifndef _HDMITX_CTRL_H_
