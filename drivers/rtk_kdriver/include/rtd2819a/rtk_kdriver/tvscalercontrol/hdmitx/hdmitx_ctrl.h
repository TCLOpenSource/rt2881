#ifndef _HDMITX_CTRL_H_
#define _HDMITX_CTRL_H_
#include <rtk_kdriver/tvscalercontrol/hdmitx/rtk_hdmitx.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>
#include <ioctrl/scaler/hdmitx_cmd_id.h>

//#include "msg_q_function_api.h"

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



typedef enum _HDMITX_AP_STATE_MACHINE{
   HDMITX_AP_INIT=0,
   HDMITX_AP_CONNECT,
   HDMITX_AP_TIMING_MATCH,
   HDMITX_AP_DRIVER_RUN,
   HDMITX_AP_DISCONNECT,
}HDMITX_AP_STATE_MACHINE;
/************************
// Debug Functoin/Macro Definition
//
*************************/
#define hdmitx_ctrl_get_timing_type() (timing_type)
#define hdmitx_ctrl_get_default_timing_type() (TX_OUTPUT_TIMING_DEFAULT)


#define hdmitx_ctrl_get_tx_hpd_init() (txHpdInited)
#define hdmitx_ctrl_clr_tx_hpd_init() (txHpdInited=0)
#define hdmitx_ctrl_set_tx_hpd_init() (txHpdInited=1)

#define hdmitx_get_ap_state() (hdmitx_ap_state)
#define hdmitx_set_ap_state(x) (hdmitx_ap_state = x)
/************************
// Implementation Function define
//
*************************/
void setHDMITX_Timing_Ready(TX_STATUS status);
void hdmitx_timer_event_init(void);
void Scaler_reset_hdmitx_state(void);
void HdmiTxHpdConfig(void);
void hdmitx_check_disconect_handler (void);
UINT8 APGetHdmiTxOutPutInfo(UINT8 hdmitx_port,StructHDMITxOutputTimingInfo *stTxOutputTimingInfo);
UINT8 APGetHdmiTxEdid(UINT16 *len, UINT8* pEdidbuf);

/************************
// Extern function define
//
*************************/
extern UINT8 timing_type;
extern UINT8 txHpdInited;
#endif // #ifndef _HDMITX_CTRL_H_
