#ifndef __RTK_USB_UAC_API_H__
#define __RTK_USB_UAC_API_H__

#include "usb_cdc.h"
//****************************************************************************
// UAC USB INTERFACE
//****************************************************************************
//#define UAC1
#define UAC2

#define UAC1_IN_EP_NUMBER  6
#define UAC1_OUT_EP_NUMBER 7
#define UAC2_OUT_EP_NUMBER 13


#define UAC1_OUT_EP_MAX_PACKET_SIZE     200
#define UAC1_DEF_CCHMASK                0x3
#define UAC1_DEF_CSRATE                 48000
#define UAC1_DEF_CSSIZE                 2
#define UAC1_DEF_PCHMASK                0x3
#define UAC1_DEF_PSRATE                 48000
#define UAC1_DEF_PSSIZE                 2
#define UAC1_DEF_REQ_NUM                2

#ifdef UAC1
#define UAC_SET_                        0x00
#define UAC_GET_                        0x80

#define UAC__CUR                        0x1
#define UAC__MIN                        0x2
#define UAC__MAX                        0x3
#define UAC__RES                        0x4
#define UAC__MEM                        0x5

#define UAC_SET_CUR                     (UAC_SET_ | UAC__CUR)
#define UAC_GET_CUR                     (UAC_GET_ | UAC__CUR)
#define UAC_SET_MIN                     (UAC_SET_ | UAC__MIN)
#define UAC_GET_MIN                     (UAC_GET_ | UAC__MIN)
#define UAC_SET_MAX                     (UAC_SET_ | UAC__MAX)
#define UAC_GET_MAX                     (UAC_GET_ | UAC__MAX)
#define UAC_SET_RES                     (UAC_SET_ | UAC__RES)
#define UAC_GET_RES                     (UAC_GET_ | UAC__RES)
#define UAC_SET_MEM                     (UAC_SET_ | UAC__MEM)
#define UAC_GET_MEM                     (UAC_GET_ | UAC__MEM)
#endif

#ifdef UAC2
#define UAC2_CS_CUR             0x01
#define UAC2_CS_RANGE           0x02
#endif

#define USB_UAC_DEBUG_STATUS_MONITOR_TIME_MS 1000
#define USB_UAC_DDR_SIZE 256*1024
#define USB_UAC_TRB_SIZE 16
#define USB_UAC_TRB_LINK_CNT 1

#define USB_TRB_SIZE 0x10
#define UAC_MPS_SIZE                    0xd0
#define UAC_MPS_SIZE_96K16B             0x190
#define UAC_MPS_SIZE_192K16B            0x310

// ===================== FPGA porting ===========================================

#define UAC_OUT1_CONFIG     1
#define UAC_OUT2_CONFIG     1

#define UAC_IN__48K_16BIT     1
#define UAC_IN__96K_16BIT     0
#define UAC_IN__96K_24BIT     0
#define UAC_IN_192K_24BIT     0

#define UAC_OUT__48K_16BIT    1
#define UAC_OUT__96K_16BIT    0
#define UAC_OUT__96K_24BIT    0
#define UAC_OUT_192K_24BIT    0

#if UAC_IN__48K_16BIT

#define UAC_IN__INTERVAL                   4
#define UAC_OUT_INTERVAL                   4
#define UAC_TABLE                       0x80, 0xBB, 0x00
#define UAC_MPS_TABLE                   0xd0, 0x0

#elif UAC_IN__96K_16BIT

#define UAC_MPS_SIZE                   0x180
#define UAC_IN__INTERVAL                   4
#define UAC_OUT_INTERVAL                   4
#define UAC_TABLE                       0x00, 0x77, 0x01
#define UAC_MPS_TABLE                   0x80, 0x01

#elif UAC_IN__96K_24BIT

#define UAC_MPS_SIZE                   0x240
#define UAC_IN__INTERVAL                   4
#define UAC_OUT_INTERVAL                   4
#define UAC_TABLE                       0x00, 0x77, 0x01
#define UAC_MPS_TABLE                   0x40, 0x02

#elif UAC_IN_192K_24BIT

#define UAC_MPS_SIZE                   0x240
#define UAC_IN__INTERVAL                   5
#define UAC_OUT_INTERVAL                   5
#define UAC_TABLE                       0x00, 0xEE, 0x02
#define UAC_MPS_TABLE                   0x40, 0x02

#endif


//#define UAC_AVSYNC_SUPPORT    1


//****************************************************************************
// CODE TABLES
//****************************************************************************
typedef enum UAC_TYPE
{
    _UAC_IN0 = 0,
    _UAC_OUT0,
    _UAC_OUT1,
    _UAC_TYPE_MAC
} UAC_TYPE;

typedef enum UAC_TYPE_SETTING
{
    _UAC_NONE_ = 0,
    _192K_24B,
    _96K_24B,
    _48K_24B,
    _192K_16B,
    _96K_16B,
    _48K_16B
} UAC_TYPE_SETTING;

typedef enum MSG_Q_SUB_UAC {
    MSG_UAC_MONITOR = 0,        // 0
    MSG_AP_UAC_CONNECT,         // 1
    MSG_UAC_AP_CONNECT_DONE,    // 2
    MSG_AP_UAC_DISCONNECT,      // 3
    MSG_UAC_AP_DISCONNECT_DONE, // 4
    MAX_MSG_Q_SUB_UAC_COUNT,    // 5
} _MSG_Q_SUB_UAC;

struct cntrl_cur_lay3 {
        UINT32  dCUR;
};

struct cntrl_range_lay3 {
        UINT16  wNumSubRanges;
        UINT32  dMIN;
        UINT32  dMAX;
        UINT32  dRES;
};

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
UINT8 usb_uac_alloc(UAC_TYPE_SETTING uac_in_freq_type, UAC_TYPE_SETTING uac_out0_freq_type);
UINT8 usb_uac_in_sram_init(UAC_TYPE_SETTING uac_freq_type);
UINT8 usb_uac_out0_sram_init(UAC_TYPE_SETTING uac_freq_type);
UINT8 usb_uac_out1_sram_init(UAC_TYPE_SETTING uac_freq_type);
UINT8 usb_uac_in_connect(UAC_TYPE uac_type);
UINT8 usb_uac_in_disconnect(UAC_TYPE uac_type);
UINT8 usb_uac_out_connect(UAC_TYPE uac_type);
UINT8 usb_uac_out_disconnect(UAC_TYPE uac_type);
void rtk_uac_control_interface(struct usb_ctrlrequest *ctrl_req);
void rtk_uac_stream_interface(struct usb_ctrlrequest *ctrl_req);
void usb_uac_monitor(void);
void usb_uac_init(void);

#endif // __RTK_USB_UAC_API_H__
