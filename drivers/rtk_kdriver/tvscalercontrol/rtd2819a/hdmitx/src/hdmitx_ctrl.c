
//#include "msg_q_function_api.h"
//#include "msg_queue_ap_def.h"
#include <rbus/hdmitx_misc_reg.h>
#include <rbus/hdmitx_vgip_reg.h>

#include <rbus/timer_reg.h>
#include <rbus/emcu_reg.h>
//#include <rbus/ppoverlay_outtg_reg.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h> 
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_init.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_ctrl.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_lib.h>
#include <tvscalercontrol/hdmitx/hdmitx.h>
//#include <scaler/scalerDrvCommon.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_share.h>
#ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_script.h>
#endif
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>

#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
//#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
#endif
#ifdef CONFIG_ENABLE_DPRX
#include <rtk_kdriver/dprx/drvif_dprx.h>
#endif
#if IS_ENABLED(CONFIG_HDCPTX)
#include <hdcp/hdcptx/hdcptx_export.h>
#endif
/************************
// Extern variable declaration
//
*************************/
#include <scaler/scalerCommon.h>


/************************
// Type/Enum define
//
*************************/

//****************************************************************************
// global variable declaration
//****************************************************************************
UINT8 bWaitLinkTrainingReady=0;


/************************
// Static/Local variable declaration
//
*************************/
static TX_STATUS preTxState[2]={TX_TIMING_INIT_STATE,TX_TIMING_INIT_STATE};
static TX_STATUS curTxState=0;
UINT8 timing_type=TX_TIMING_NUM;
UINT8 HDMITX_TIMING_INDEX = TX_TIMING_NUM;
TX_STATUS tx_status;
//static UINT8 value_u8;

static UINT16 ucCheckCnt=0;
//static UINT16 usFrameCnt_pre=0, usFrameCnt=0;
static UINT16  usFrameCnt=0;
static UINT16 usFrameCnt_lastDump=0;


//#define ENABLE_PROFILE_TX_STATE_RUN_TIME


//static UINT16 dtgIvsCnt, dtgDvsCnt;
StructHDMITxOutputTimingInfo stTxRepeaterOutputTimingInfo = {0};

#ifdef ENABLE_HDMITX_PTG_AUTO_RUN
UINT8 ApSetTxInit = 1;
UINT8 scalerSetTxRun = 1;
UINT8 sendApHdmiConnect=1;
#else
UINT8 ApSetTxInit = 0;
UINT8 scalerSetTxRun = 0;
UINT8 sendApHdmiConnect=0;
#endif
UINT8 txInBypassMode=0;
UINT8 TxRunDone = 0;
UINT8 txHpdInited=0;
#ifdef _DEBUG_HALT_TX_WHEN_FIFO_ERROR
UINT8 TxHaltFlag = 0;
#endif
UINT8 u8HdcpDisable = 0;
UINT8 hdmitx_ap_state = HDMITX_AP_INIT;
UINT8 u8hdmitx_Hdcp_state = 0;//0: normal ;1 hdcp ptg
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
extern UINT8 current_port;
#if(_AUDIO_SUPPORT == _ON)
extern UINT8 ScalerStreamAudioGetChannelCount(EnumOutputPort ucOutputPort);
extern UINT8 ScalerStreamAudioGetCodingType(EnumOutputPort ucOutputPort);
extern UINT8 ScalerStreamAudioGetSamplingFrequencyType(EnumOutputPort ucOutputPort);
extern UINT8 ScalerStreamGetAudioMuteFlag(EnumOutputPort ucOutputPort);
#endif
#endif

extern UINT8 ScalerHdmiJudgeTxConnect(void);
extern void ScalerHdmiTxBypassAVMuteAndPacketCtrl(UINT8 enable);
extern void ScalerHdmiMacTx0SCDCReset(void);
extern void ScalerHdmiJudgeReloadEdid(void);
extern void HDMITX_drv_menuCmd_setTxPtgTimingMode(UINT8 timeMode);
extern UINT8 getVscInitdoneFlag(void);
extern UINT16 Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_INFO infoList);
/************************
// Macro/define
//
*************************/
#define ABS(x,y)						((x > y) ? (x-y) : (y-x))

#define ScalerHdmiTxGetTargetFrlRate() (HDMITX_OUT_FRL_RATE)
/************************
// Extern variable
//
*************************/
extern EnumHdmi21FrlType HDMITX_OUT_FRL_RATE;
extern UINT8 u8hdcprun;

//extern UINT8 g_detect_cts_device;
/************************
// Extern Function declaration
//
*************************/
extern void HDMITX_Input_timing_config(void);
extern void HDMITX_phy_init(void);
extern UINT8 Scaler_TxStateHandler(void);
extern INT8 ScalerHdmiPhyTx0PowerOff(void);
extern UINT8 ScalerHdmiTxGetStreamStateReady(void);
#ifdef ENABLE_TIMER_EVENT_TEST // [CMD]: change TX output timing event
extern UINT32 hdmitx_test(UINT8 *time_mode);
#endif //     #ifdef ENABLE_TIMER_EVENT_TEST // [CMD]: change TX output timing event

extern void ScalerHdmiTxMuteEnable(UINT8 enable);
extern void CLR_EDID_READ_FLG(void);
extern void ScalerHdmiMacTx0RepeaterOutputTimingInfo(void);
extern StructHDMITxEdidBufInfo* ApGetScalerHdmiMacTx0RepeaterEdid(void);
//extern UINT8 ScalerHdmiHdcp2Tx2CheckCapability(UINT8 port);
extern unsigned char txpre_config(StructTxPreInfo * txpre_info);
extern StructTxPreInfo txpre_info;

/************************
// Function implementation
//
*************************/
void setHDMITX_Timing_Ready(TX_STATUS status)
{
    if(tx_status != status){
        FatalMessageHDMITx("[HDMITX] tx_status=%d\n", (UINT32) status);
        tx_status = status;
    }
    return;
}

void hdmitx_check_hdmitx_vsyncPeriod(void)
{
#if 0
    UINT32 uStcCnt_lastDump=0;
    UINT32 uStcCnt=0;
    UINT32 stcDiff;
    UINT32 value_u32;
    //UINT8 logLevel=0;

    //DSCE Timing gen
    if((rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg) & HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask) == 0){
        rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask);
        InfoMessageHDMI21Tx("[HDMITX] FrameRate Measure Enable!\n");
    }else{
    //fix by zhaodong
        //usFrameCnt = HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_get_frame_cnt(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_reg));
        if(usFrameCnt_pre != usFrameCnt){
            uStcCnt = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
            stcDiff = (uStcCnt - uStcCnt_lastDump)/90;
            value_u32 = (UINT32)250000000UL / HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_get_vs2vs_cycle(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg));

            //logLevel = (usFrameCnt < 5? LOGGER_FATAL: LOGGER_INFO);
            if(ABS(usFrameCnt, usFrameCnt_pre) > 100){
                //logLevel = 0;
                usFrameCnt_pre = usFrameCnt;
                uStcCnt_lastDump = uStcCnt;
            }
            NoteMessageHDMITx("VSync=%d(Hz)@StcDiff=%d(ms)\n", (UINT32)value_u32, (UINT32)stcDiff);
            NoteMessageHDMITx("VSync=%d(Hz)", (UINT32)value_u32);
            if(g_scaler_display_info.input_src == 0){

#ifdef CONFIG_RTK_HDMI_RX
            if(GET_H_AUDIO_FSM(0) == AUDIO_FSM_AUDIO_CHECK){
                value_u32 = ScalerStreamAudioGetChannelCount(0);
                NoteMessageHDMITx("Aud Ch/Freq/Type=%d/", (UINT32)value_u32);
                value_u32 = ScalerStreamAudioGetSamplingFrequencyType(0);
                NoteMessageHDMITx("%d/", (UINT32)value_u32);
                value_u32 = ScalerStreamAudioGetCodingType(0);
                NoteMessageHDMITx("%d", (UINT32)value_u32);
            }
#endif // #ifdef CONFIG_RTK_HDMI_RX
    }else{
            #if defined(CONFIG_ENABLE_DPRX)
            value_u32  = DRVIF_DPRX_GET_AUDIO_SAMPLE_FREQUENCY()/1000;
            NoteMessageHDMITx( "Audio=%d(KHz)", (UINT32)value_u32);
            #endif
        }
            //NoteMessageHDMITx(HDMITX_BANK6, logLevel, "\n");

        }
    }
#endif
    return;
}

void hdmitx_deinit_event_handler(UINT32 param)
{
    NoteMessageHDMITx("[HDMITX] RCV TX DEINIT[%x]@State[%d]!\n", (UINT32)param, (UINT32)ScalerHdmiTx_Get_Timing_Status());
    setHDMITX_Timing_Ready(TX_TIMING_INIT_STATE);

    return;
}

void hdmitx_check_event_handler(UINT32 param)
{
    if(ApSetTxInit && scalerSetTxRun)
    {
        if((timing_type == TX_TIMING_NUM)|| ((HDMITX_TIMING_INDEX != TX_TIMING_NUM) && (timing_type != HDMITX_TIMING_INDEX)))
        {
            timing_type = (HDMITX_TIMING_INDEX < TX_TIMING_NUM? HDMITX_TIMING_INDEX: hdmitx_ctrl_get_default_timing_type());
            setHDMITX_Timing_Ready(TX_TIMING_NEED_CHANGE);
            NoteMessageHDMITx("[HDMITX] New Time Mode=%d@%d\n", (UINT32)timing_type, (UINT32)ScalerHdmiTx_Get_Timing_Status());
        } // if((timing_type == TX_TIMING_NUM)|| ((HDMITX_TIMING_INDEX != TX_TIMING_NUM) && (timing_type != HDMITX_TIMING_INDEX))){

        if(preTxState[0] != ScalerHdmiTx_Get_Timing_Status())
            NoteMessageHDMITx("[HDMITX] check state: %x->%x\n", (UINT32)preTxState[0], (UINT32)ScalerHdmiTx_Get_Timing_Status());

        curTxState = ScalerHdmiTx_Get_Timing_Status();
        if(preTxState[0] != curTxState)
            NoteMessageHDMITx("[HDMITX][%x] init state from %x\n", (UINT32)param, (UINT32)preTxState[0]);
    }
	#if 0
    if(ApSetTxInit){
        core_timer_event_addTimerEvent(TIMER_HDMITX_STATE_CHECK, MSG_HDMITX_ID, TX_EVENT_STATE_EVENT, (UINT32)hdmitx_check_event_handler);
    }else{
        core_timer_event_addTimerEvent(TIMER_HDMITX_STATE_CHECK, MSG_HDMITX_ID, TX_EVENT_CHECK_EVENT, (UINT32)hdmitx_check_event_handler);
    }
	#endif
    preTxState[0] = curTxState;
    return;
}

extern void ScalerHdmiTx_Set_DtgClkSource(int index, DCLK_SRC_TYPE type);
INT32 hdmitx_state_event_handler(void *param) // [BANK1][TIMER] don't need declare __bank due to timer event handler call by bank1 function (same bank function)
{
    UINT8 bVgipEn; 

    UINT16 stateEventLoopCnt=0;
#ifdef ENABLE_PROFILE_TX_STATE_RUN_TIME
    UINT32 stcDiff;
    UINT32 value_u32;
    UINT32 txEnterStc, txBankSwitchCnt, txEnterLtStc;
#endif
#ifdef ENABLE_PROFILE_TX_STATE_RUN_TIME
    txEnterStc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    txBankSwitchCnt = rtd_inl(EMCU_dummy_3_reg);
#endif

    // [CTS][HFR1-68] reload EDID and link training flow when HPD detected
    if(ScalerHdmiTxGetForceLinkTrainingEn(_TX0)){
        if(ScalerHdmiTx_Get_Timing_Status() != TX_TIMING_INIT_STATE){
            Scaler_reset_hdmitx_state();
            CLR_EDID_READ_FLG();
            ApSetTxInit = 1;
            scalerSetTxRun = 1;
        }
        ScalerHdmiTxSetForceLinkTrainingEn(_DISABLE);
    }

    if(preTxState[1] != ScalerHdmiTx_Get_Timing_Status()){
        DebugMessageHDMITx("[HDMITX] event state: %x->%x\n", (UINT32)preTxState[1], (UINT32)ScalerHdmiTx_Get_Timing_Status());
        stateEventLoopCnt=0;
    }

    if((++stateEventLoopCnt % 100) == 0){
        if((ScalerHdmiTx_Get_Timing_Status() != TX_TIMING_SETTING_DONE)){
            DebugMessageHDMITx("[HDMITX][%d] event state: %x\n", (UINT32)stateEventLoopCnt, (UINT32)ScalerHdmiTx_Get_Timing_Status());
         }
         else{
            DebugMessageHDMITx("[HDMITX] current timing: %x\n", (UINT32)timing_type);
         }
     }

    DebugMessageHDMITx("[HDMITX] current timing: %x\n", (UINT32)timing_type);
#if 0 //fix by zhaodong
    if(g_detect_cts_device){
        if(ScalerHdmiJudgeTxConnect()){
            if((!ScalerHdmiHdcp2Tx2CheckCapability(0)) && (!u8hdmitx_Hdcp_state)){
                u8hdmitx_Hdcp_state = 1;
                FatalMessageHDMITx("[HDMITX]  able hdcp ptg: %d\n", (UINT32)g_detect_cts_device);
                HDMITX_drv_menuCmd_setTxPtgTimingMode(CTS_TIMING_INDEX);
                }
            }
    }else{
        if(u8hdmitx_Hdcp_state == 1){
            u8hdmitx_Hdcp_state = 0;
            FatalMessageHDMITx("[HDMITX]  disable hdcp ptg: %d\n", (UINT32)g_detect_cts_device);
            ScalerHdmiPhyTx0PowerOff();
        }
    }
#endif

    {
		DebugMessageHDMITx("[HDMITX]ScalerHdmiTx_Get_Timing_Status(): %d\n", ScalerHdmiTx_Get_Timing_Status());
		hdmitx_check_event_handler(0);
        switch(ScalerHdmiTx_Get_Timing_Status()){
          case TX_TIMING_INIT_STATE:
            if(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_get_dummy_8(rtd_inl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg)))
            {
                u8HdcpDisable = 1;
                NoteMessageHDMITx("[HDMITX]  disable hdcp : %d\n", (UINT32)u8HdcpDisable);
                rtd_maskl(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg, ~HDMITX_MISC_HDMITX_MISC_DBG_CTRL_dummy_8_mask, HDMITX_MISC_HDMITX_MISC_DBG_CTRL_dummy_8(0));
            }

            if(ApSetTxInit)
                Scaler_TxStateHandler();
            break;
          case TX_TIMING_NEED_CHANGE:
#ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT
            HDMITX_DV_Script_test_run();
            if(SCRIPT_TX_SETING_DONE() == FALSE)
#endif
                HDMITX_Input_timing_config();


//#ifdef CONFIG_ENABLE_TXSOC_PATH     
            setHDMITX_Timing_Ready(TX_TIMING_FREERUN_CHANGE_DONE);

            break;
          case TX_TIMING_FREERUN_CHANGE_DONE:
#ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT
            if(SCRIPT_TX_SETING_DONE())
                TxRunDone = 1;
            else
#endif // #else // #ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT
                TxRunDone = Scaler_TxStateHandler();
            if(TxRunDone) // stream ready
            {
                setHDMITX_Timing_Ready(TX_TIMING_WAIT_SETTING_DONE);
                bVgipEn= HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));
                if(bVgipEn)
                    txpre_config(&txpre_info);
				//fix by zhaodong
                //core_fw_msg_queue_addEvent(MSG_SCALER_ID, MSG_HDMITX_TO_SCALER_TX_SETTINGDONE, 0);//return tx run ok to scaler
            }
            break;
          case TX_TIMING_WAIT_SETTING_DONE:
            setHDMITX_Timing_Ready(TX_TIMING_SETTING_DONE);
            FatalMessageHDMITx("TX_TIMING_WAIT_SETTING_DONE\n");
              break;
          case TX_TIMING_SETTING_DONE:
#ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT
            HDMITX_DV_Script_test_check();
#endif
            if(SCRIPT_TX_SETING_DONE() == FALSE)
                Scaler_TxStateHandler();
            break;
          default:
            break;
        }

    }

#ifndef CONFIG_HDMITX_TX_MENU_TEST_SUPPORT // [DUMP INFO]
    if(ScalerHdmiTx_Get_Timing_Status() == TX_TIMING_SETTING_DONE){
        // dump TX input vsync period
        hdmitx_check_hdmitx_vsyncPeriod();

        if(usFrameCnt != usFrameCnt_lastDump){
    #if 0//ndef ENABLE_PROFILE_TX_STATE_RUN_TIME //fix by zhaodong
            // DTG fsync mode
            value_u8 = PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_get_disp_fsync_en(rtd_inl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg));
            value_u32 = rtd_inl(CLK_DIV_REG);
            logLevel = (usFrameCnt < 5? LOGGER_INFO: LOGGER_INFO);
            InfoMessageHDMITx(HDMITX_BANK6, logLevel, "[HDMITX] FrameCnt=%d\n", (UINT32)usFrameCnt);
            InfoMessageHDMITx(HDMITX_BANK6, logLevel, "[HDMITX] DTG Sync=%d, txClkRatio=%x\n", (UINT32)value_u8, (UINT32)value_u32);
    

            // ivs/dvs
            dtgIvsCnt = PPOVERLAY_OUTTG_OUTTG_IVS_cnt_get_ivs_cnt(rtd_inl(PPOVERLAY_OUTTG_OUTTG_IVS_cnt_reg));
            dtgDvsCnt = PPOVERLAY_OUTTG_OUTTG_DVS_cnt_get_dvs_cnt(rtd_inl(PPOVERLAY_OUTTG_OUTTG_DVS_cnt_reg));
#endif
        #if 0
            if(ABS(usFrameCnt - usFrameCnt_lastDump) > 100){
                logLevel = 0;
                usFrameCnt_lastDump = usFrameCnt;
            }
        #endif
            //InfoMessageHDMITx(HDMITX_BANK6, logLevel, "VS=%x/%x\n", (UINT32)dtgIvsCnt, (UINT32)dtgDvsCnt);
        }else{
            if(ucCheckCnt % 100 == 0)
                DebugMessageHDMITx("[HDMI_TX] State=%d, loopCnt=%d\n", (UINT32)ScalerHdmiTx_Get_Timing_Status() , (UINT32)ucCheckCnt);
        }

        ++ucCheckCnt;
    }
#endif

#ifdef _DEBUG_HALT_TX_WHEN_FIFO_ERROR // HALT TX when TX FIFO error
    if(TxHaltFlag){
        ErrorMessageHDMI21Tx("[HDMI21_TX] FORCE TX STATE HALT-2!!\n");
        return 0;
    }
#endif

    // trigger next hdmitx timer event
    #if 0
    if((ScalerHdmiTx_Get_Timing_Status() == TX_TIMING_INIT_STATE)){
        core_timer_event_addTimerEvent(TIMER_HDMITX_STATE_CHECK, MSG_HDMITX_ID, TX_EVENT_CHECK_EVENT, (UINT32)hdmitx_state_event_handler);
    }else if(ScalerHdmiTx_Get_Timing_Status() == TX_TIMING_SETTING_DONE){
        if(ScalerHdmiTxGetTargetFrlRate() != 0) // [HFR1-68] hdmi21 need keep check FLT_Upate in 10ms
            core_timer_event_addTimerEvent(TIMER_HDMITX_STATE_CHECK, MSG_HDMITX_ID, TX_EVENT_CHECK_EVENT, (UINT32)hdmitx_state_event_handler);
        else
            core_timer_event_addTimerEvent(TIMER_HDMITX_SETTING_DONE_STATE_CHECK, MSG_HDMITX_ID, TX_EVENT_CHECK_EVENT, (UINT32)hdmitx_state_event_handler);
    }else{
        core_timer_event_addTimerEvent(TIMER_HDMITX_STATE_CHECK, MSG_HDMITX_ID, TX_EVENT_STATE_EVENT, (UINT32)hdmitx_state_event_handler);
    }
    preTxState[1] = ScalerHdmiTx_Get_Timing_Status();
	#endif

#ifdef ENABLE_PROFILE_TX_STATE_RUN_TIME
    value_u32 = rtd_inl(EMCU_dummy_3_reg)-txBankSwitchCnt;
    FatalMessageHDMITx("[Prof][%d] TX BankSw/Time=%x/", (UINT32)ScalerHdmiTx_Get_Timing_Status(), (UINT32)value_u32);
    stcDiff = (rtd_inl(TIMER_SCPU_CLK90K_LO_reg) - txEnterStc)/90;
    FatalMessageHDMITx("%d\n", (UINT32)stcDiff);
#endif
    return 0;
}

void hdmitx_timer_event_init(void)
{
    DebugMessageHDMITx("[HDMITX] hdmitx_timer_event_init\n");

#ifdef ENABLE_PROFILE_TX_STATE_RUN_TIME
    prid_set_ll(HDMITX_BANK6, LOGGER_FATAL);
    prid_set_ll(HDMITX_BANK7, LOGGER_FATAL);
    prid_set_ll(HDMITX_BANK8, LOGGER_FATAL);
    prid_set_ll(HDMITX_BANK9, LOGGER_FATAL);
    prid_set_ll(HDMITX_BANK38, LOGGER_FATAL);
#endif

    hdmitx_check_event_handler(0);
    return;
}
void hdmitx_AP_Init_event_handler(UINT32 param) // [BANK1][TIMER] don't need declare __bank due to timer event handler call by bank1 function (same bank function)
{
    FatalMessageHDMITx("[HDMITX] hdmitx_AP_Init\n");
    if(hdmitx_get_otp_status() != 0) {
        FatalMessageHDMITx("[HDMITX] hdmitx disable\n");
        ScalerHdmiPhyTx0PowerOff();
        return;
    }
    hdmitx_set_ap_state(HDMITX_AP_INIT);
    ScalerHdmiMacTx0SCDCReset();
    ApSetTxInit = 1;
    // HDMITX HW init: TX PHY power on, TX Mac reset, HPD enable, TX interrupt enable(on SOC)

    if(hdmitx_ctrl_get_tx_hpd_init() == 0)
        HDMITX_phy_init();

    return;
}
void hdmitx_Scaler_Txrun_event_handler(UINT32 param) // [BANK1][TIMER] don't need declare __bank due to timer event handler call by bank1 function (same bank function)
{
    FatalMessageHDMITx("[HDMITX]Scaler set Txrun, timing = %d,state:[%d]\n",(UINT32)param, (UINT32)hdmitx_get_ap_state());
    if(hdmitx_get_ap_state() != HDMITX_AP_TIMING_MATCH)
        return;
    hdmitx_set_ap_state(HDMITX_AP_DRIVER_RUN);
    if(param >= TX_TIMING_NUM){
     FatalMessageHDMITx("[HDMITX]Scaler set timing  = %d,over max\n",(UINT32)param);
     return;
    }
    HDMITX_TIMING_INDEX = param;
    scalerSetTxRun = 1;
    return;
}
void Scaler_reset_hdmitx_state(void)
{
    FatalMessageHDMITx("[HDMITX] Scaler_reset_hdmitx_state\n");
    ScalerHdmiTxMuteEnable(_ENABLE);
    ScalerHdmiTxBypassAVMuteAndPacketCtrl(_DISABLE);
    ScalerHdmiPhyTx0PowerOff();
    ScalerHdmiMacTx0SCDCReset();
    ApSetTxInit = 0;
    scalerSetTxRun = 0;
    TxRunDone = 0;
    timing_type = TX_TIMING_NUM;
    setHDMITX_Timing_Ready(TX_TIMING_INIT_STATE);
    //CLR_EDID_READ_FLG();
}

void hdmitx_ptg_reset_state(void)
{
    //hdmitx_set_ap_state(HDMITX_AP_INIT);
    Scaler_reset_hdmitx_state();
}

void hdmitx_AP_NoSignal_event_handler(UINT32 param)
{
    FatalMessageHDMITx("[HDMITX] hdmitx_AP_NoSignal_event\n");
    hdmitx_set_ap_state(HDMITX_AP_DISCONNECT);
    scalerSetTxRun = 0;
    TxRunDone = 0;
    timing_type = TX_TIMING_NUM;
    setHDMITX_Timing_Ready(TX_TIMING_INIT_STATE);
    ScalerHdmiPhyTx0PowerOff();
    return;
}
void hdmitx_status_reset(void)
{
        hdmitx_set_ap_state(HDMITX_AP_INIT);
        Scaler_reset_hdmitx_state();
        ApSetTxInit = 1;
        u8hdcprun = 0;
#if IS_ENABLED(CONFIG_HDCPTX)
        SET_HDMI_HDCPTX_RUN(0, 0);
#endif

}
void hdmitx_AP_ReInit_event_handler(UINT32 param)
{
    if(hdmitx_get_ap_state() == HDMITX_AP_INIT || ((hdmitx_get_ap_state() == HDMITX_AP_DISCONNECT) && (!ScalerHdmiJudgeTxConnect()))){
        hdmitx_set_ap_state(HDMITX_AP_INIT);
        FatalMessageHDMITx("[HDMITX]already in init state\n");
        return;
    }
    FatalMessageHDMITx("[HDMITX] hdmitx_AP_ReInit_event\n");
    hdmitx_status_reset();
    //default param=0 timing change no reload EDID, param=1 for ap force reload EDID case
    if(param == 0)
        ScalerHdmiJudgeReloadEdid();
    else
        CLR_EDID_READ_FLG();
    return;
}

UINT8 APGetHdmiTxEdid(UINT16 *len, UINT8* pEdidbuf)
{
    StructHDMITxEdidBufInfo * pstHdmiTxEdidInfo;

    pstHdmiTxEdidInfo = ApGetScalerHdmiMacTx0RepeaterEdid();
    *len = pstHdmiTxEdidInfo->u16EdidLen;
    memcpy(pEdidbuf, pstHdmiTxEdidInfo->pEdidBufAddr, EDID_BUFFER_SIZE);
    return TRUE;
}

UINT8 hdmitx_AP_SetTxConnect_handler(UINT8 hdmitx_port,UINT8 mode, UINT8 input_src, UINT8 input_src_port){
    FatalMessageHDMITx("[HDMITX] hdmitx_AP_SetTxConnect_handler mode[%d],src[%d]\n", (UINT32)mode, (UINT32)input_src);
    if((hdmitx_get_ap_state() == HDMITX_AP_DISCONNECT) && (ScalerHdmiJudgeTxConnect())) {
        FatalMessageHDMITx("[HDMITX]already in connect state\n");
    }
    else if(hdmitx_get_ap_state() != HDMITX_AP_CONNECT) {
        FatalMessageHDMITx("[HDMITX] match state fail[%d]\n", (UINT32)hdmitx_get_ap_state());
        return FALSE;
    }
    hdmitx_set_ap_state(HDMITX_AP_TIMING_MATCH);
    set_hdmitxtiminginfo_addr(&stTxRepeaterOutputTimingInfo); 
    set_hdmitxOutputMode((TX_OUT_MODE_SELECT_TYPE)mode);
    set_hdmitxInputSrc((TX_INPUT_SRC_SELECT_TYPE)input_src);
    set_hdmitxPort(hdmitx_port);
    set_hdmitxInputSrcPort(input_src_port);
    ScalerHdmiMacTx0RepeaterOutputTimingInfo();
    return TRUE;
}
UINT8 APGetHdmiTxOutPutInfo(UINT8 hdmitx_port,StructHDMITxOutputTimingInfo *stTxOutputTimingInfo)
{
    FatalMessageHDMITx("[HDMITX] APGetHdmiTxOutPutInfo port[%d]\n", (UINT32)hdmitx_port);

    set_hdmitxtiminginfo_addr(stTxOutputTimingInfo); 
    ScalerHdmiMacTx0RepeaterOutputTimingInfo();
    return TRUE;
}

void hdmitx_check_disconect_handler (void)
{
#ifdef CONFIG_HDMITX_AP_TEST
            FatalMessageHDMITx("[HDMITX] send MSG_AP_HDMITX_DISCONNECT() !\n");

            core_fw_msg_queue_addEvent(MSG_AP_ID, 102, 0);//return connect to ap
            //core_fw_msg_queue_addEvent(MSG_HDMITX_ID, 9, 0);
#endif
}

void hdmitx_AP_CTS_Mode_event_handler(UINT32 param)
{
    HDMITX_drv_menuCmd_setTxPtgTimingMode(CTS_TIMING_INDEX);
}
