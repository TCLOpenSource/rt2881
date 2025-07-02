#include <arch.h>
#include <mach.h>
#include <rtk_io.h>

#include <rtk_kdriver/timer.h>

#include <timer_event.h>

#include "msg_q_function_api.h"

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>

#include <rbus/hdmitx_misc_reg.h>

PRAGMA_CODESEG(BANK8);   // HAMITX Bank: 6/7/8/9/38);
PRAGMA_CONSTSEG(BANK8);  // HAMITX Bank: 6/7/8/9/38);

/************************
// Macro/definition
//
*************************/
#define _DEFAULT_PENDING_STATE _FALSE   // stop TX main loop check state
#define _DEFAULT_PENDING_TXPKT _TRUE    // stop send TX packet

/************************
// data structure define
//
*************************/
typedef enum _EnumScalerTimerEventHdmiTx0Type{
    _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK=0,
    _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_ENABLE,
    _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_DISABLE,
    _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT,
    _SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT,
    _SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT,
    _SCALER_TIMER_EVENT_HDMI_TX0_AUDIO_CTS_CODE_CHECK,
    _SCALER_TIMER_EVENT_HDM21_TX0_FRL_PASS_TIMER,
    _SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER,
    _SCALER_TIMER_EVENT_NUM
}EnumScalerTimerEventHdmiTx0Type;

typedef struct{
    UINT32 addr;
    UINT32 mask;
    UINT8 status;
    UINT32 time_90k;
    UINT8 timecount;
}StructEventPollingInfo;

/************************
// Static/Local variable declaration
//
*************************/
static UINT8 value_u8;
static UINT8 gPendingStatus=_DEFAULT_PENDING_STATE;
UINT8 gPendingTxPkt=_DEFAULT_PENDING_TXPKT;

/************************
// Extern Function declaration
//
*************************/
extern INT8 ScalerTimerActiveTimerEvent(UINT16 time_ms, EnumScalerTimerEventHdmiTx0Type event) __banked;
extern INT8 ScalerTimerCancelTimerEvent(EnumScalerTimerEventHdmiTx0Type event) __banked;
extern INT8 ScalerTimerSearchActiveTimerEvent(EnumScalerTimerEventHdmiTx0Type event) __banked;
extern INT8 ScalerTimerReactiveTimerEvent(UINT16 time_ms, EnumScalerTimerEventHdmiTx0Type event) __banked;
extern INT8 ScalerTimerPollingFlagProc_bankIF(StructEventPollingInfo *eventInfo)__banked;

extern void ScalerHdmiTx0_LanePnSwap_config(UINT8 value);
extern void ScalerHdmiTx0_LaneSrcSel_config(UINT8 value);

/************************
// Function implementation
//
*************************/
void hdmitx_timing_event_test(UINT8 cmd)
{
    NoteMessageHDMITx("[HDMITX][DBG] Get Time Event->%x\n", (UINT32)cmd);
    if(cmd == 1){
        ScalerTimerActiveTimerEvent(100, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);
        ScalerTimerActiveTimerEvent(100, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);
        ScalerTimerActiveTimerEvent(200, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);
        ScalerTimerActiveTimerEvent(100, _SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT);
        ScalerTimerActiveTimerEvent(200, _SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER);
    }else if(cmd == 2){
        ScalerTimerSearchActiveTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);
        ScalerTimerSearchActiveTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);
        ScalerTimerSearchActiveTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);
        ScalerTimerSearchActiveTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT);
        ScalerTimerSearchActiveTimerEvent(_SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER);
    }else if(cmd == 3){
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT);
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER);
    }else if(cmd == 4){
        ScalerTimerReactiveTimerEvent(100, _SCALER_TIMER_EVENT_HDMI_TX0_SCDC_SCRAMBLE_CHECK);
        ScalerTimerReactiveTimerEvent(100, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);
        ScalerTimerReactiveTimerEvent(200, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);
        ScalerTimerReactiveTimerEvent(100, _SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT);
        ScalerTimerReactiveTimerEvent(200, _SCALER_TIMER_EVENT_HDM21_TX0_LTS3_TIMER);
    }else if(cmd == 5){
        StructEventPollingInfo eventInfo;
        eventInfo.timecount = 1;
        eventInfo.addr = DEBUG_TX_DUMMY_ADDR;
        eventInfo.mask = _BIT28;
        ScalerTimerPollingFlagProc_bankIF(&eventInfo);
        eventInfo.mask = _BIT29;
        ScalerTimerPollingFlagProc_bankIF(&eventInfo);
    }

    return;
}


void hdmitx_timing_set_timing_gen(UINT8 mode)
{
    value_u8 = HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_get_patgen_en(rtd_inl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg));
    NoteMessageHDMITx("[HDMITX][DBG] Set pattern gen=%x(/%d)\n", (UINT32)mode, (UINT32)value_u8);
    rtd_part_outl( HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg,31,31, mode);//PATGEN_EN[31]Pattern generator: 0: Pattern generator disable for VBY1 37.125M, 1: Pattern generator enable for 27M
    return;
}

void hdmitx_timing_set_pattern_gen(UINT8 mode)
{
    value_u8 = HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_get_self_tg_en(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg));
    NoteMessageHDMITx("[HDMITX][DBG] Set timing gen=%x(/%d)\n", (UINT32)mode, (UINT32)value_u8);
    rtd_part_outl( HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg,0,0, mode); // 0: HDMITX_IN_TIMING_GEN_CTRL Disable for VBY1 37.125M, 1: HDMITX_IN_TIMING_GEN_CTRL Enable for 27M
    return;
}

UINT8 hdmitx_test(UINT8 *time_mode)
{
    UINT8 test_cmd = DEBUG_TX_Get_test_cmd();
    UINT8 test_param = DEBUG_TX_Get_test_mode();

    if(test_cmd == 0)
        return gPendingStatus;

    if(ScalerHdmiTx_Get_Timing_Status() != TX_TIMING_INIT_STATE)
        delayms(3000);
    NoteMessageHDMITx("[HDMITX][DBG] Test CMD=%x\n", (UINT32)test_cmd);
    NoteMessageHDMITx("[HDMITX][DBG] Test Mode=%x\n", (UINT32)test_cmd);

    if(test_cmd == TX_TEST_CMD_CHANGE_TIME){ // timing change flag
        *time_mode = test_param;
        if(*time_mode == 0xff){
            NoteMessageHDMITx("[HDMITX] TX TIMER EVENT END\n");
            setHDMITX_Timing_Ready(TX_TIMING_INIT_STATE);
            return gPendingStatus;
        }
    } else if(test_cmd == TX_TEST_CMD_CHANGE_LANE_MAPPING){
        if(test_param == 0){
            ScalerHdmiTx0_LanePnSwap_config(0xf);
            ScalerHdmiTx0_LaneSrcSel_config(0xb1);
        }else{
            ScalerHdmiTx0_LanePnSwap_config(0x0);
            ScalerHdmiTx0_LaneSrcSel_config(0xe4);
        }
    } else if(test_cmd == TX_TEST_CMD_SEND_EVENT){ // timer event test enable
        hdmitx_timing_event_test(test_param);
    }else if(test_cmd == TX_TEST_CMD_SET_VBY1_LOCK){
        hdmitx_timing_set_pattern_gen(test_param);
        hdmitx_timing_set_timing_gen(test_param);
    }else if(test_cmd == TX_TEST_CMD_SET_PENDING_STATE){
        gPendingStatus = test_param;
    }else if(test_cmd == TX_TEST_CMD_SET_PENDING_TX_PKT){
        gPendingTxPkt = (test_param & 0x80) >> 7;
        *time_mode = test_param & 0x7f;
    }

    DEBUG_TX_Clr_test_cmd();
    return gPendingStatus;
}

