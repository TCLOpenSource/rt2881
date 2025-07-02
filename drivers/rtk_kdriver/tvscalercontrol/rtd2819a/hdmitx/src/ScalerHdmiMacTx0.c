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
// ID Code      : ScalerHdmiMacTx0.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

//#include <rbus/ppoverlay_outtg_reg.h>
#include <rbus/hdmitx_vgip_reg.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> 
//#include <rtk_kdriver/tvscalercontrol/scaler_vscdev.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_init.h>
//#include <rtk_kdriver/measure/measure.h>
#include "../include/ScalerTx.h"
#include "../include/HdmiEdid/hdmitx_edid.h"
#if IS_ENABLED(CONFIG_HDCPTX)
#include <hdcp/hdcptx/hdcptx_export.h>
#endif

//****************************************************************************
// local variable declaration
//****************************************************************************
//#include <rbus/dsce_misc_reg.h>
//#include <rbus/ppoverlay_reg.h>
#include <rbus/hdmitx_phy_reg.h>

#include "../include/ScalerFunctionInclude.h"
#include "../include/HdmiMacTx0/ScalerHdmiMacTx0.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTxInclude.h"
#include "../include/Hdmi21MacTx/ScalerHdmi21MacTxInclude.h"
#include "../include/Hdmi21MacTx0/ScalerHdmi21MacTx0.h"
#include "../include/HdmiPhyTx/ScalerHdmiPhyTxInclude.h"
//#include "../include/HdmiHdcp14Tx0/ScalerHdmiHdcp14Tx0Include.h"
//#include "../include/HdmiHdcp2Tx0/ScalerHdmiHdcp2Tx0Include.h"
//#include "../include/hdmitx_phy_struct_def.h"
#include "../include/ScalerTx.h"
#include "../include/HdmiMacTx/ScalerHdmiMacTx.h"
//#include "../include/hdmitx_dsc_pps_struct_def.h"

//#include <mach/platform.h>

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
//#include <tvscalercontrol/hdmirx/hdmi_vfe.h>
//#include <tvscalercontrol/hdmirx/hdmifun.h>
#endif
#include <rtk_kdriver/measure/rtk_measure.h>


//****************************************************************************
// Macro/Define
//****************************************************************************
#define _HDMITX_AUDIO_MIN_BLANK_AREA 121
#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
#define ScalerHdmi21MacTxPxTxMapping(x) (_HW_P0_HDMI21_MAC_TX_MAPPING)
#define ScalerHdmi21MacTxPxMapping(x) (_P0_OUTPUT_PORT)
#define ScalerHdmi21MacTxSetLinkTrainingState(x, y) SET_HDMI21_MAC_TX_LINK_TRAINING_STATE(_TX0, y)
#endif

//****************************************************************************
// enum/structure typedef
//****************************************************************************
#define ScalerColorStream422To420(x, y)
#define ScalerColorStreamYuv2RgbClamp(x, y)
#define ScalerColorStream444To420(x, y)
#define ScalerColorStreamRgb2Yuv(x, y, z)
//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// Local/Static VARIABLE DECLARATIONS
//****************************************************************************

#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
static UINT32 lastDumpStc_HDRSet = 0;
#endif


#if 0 // TEST
static UINT32 lastStc_TxHpdCheck=0;
#endif
//static UINT8  last_hdrType=0;

static BOOLEAN bHdcpRestart = _FALSE;
static UINT16 frlFifoErrCnt=0;


//static UINT32 hdmitx20_mac0_hdmi_control_2_reg_regValue;
#ifndef CONFIG_MACH_RTD2801_ZEBU//Don't need check PFIFO status in Zebu platform (just for TX PHY status check)
//static UINT32 hdmitx20_on_region_pixel_conv_p1_reg_regValue;
#endif
//static UINT32 hdmitx20_mac1_hdmi_input_ctsfifo_reg_regValue;

// EMP packet status
//static UINT32 regValue, value_u32;
UINT8 runontime = 0;
EnumHDMITxGcpToHwState gcpswtohw = _HDMI_TX_GCP_TOHW_INIT;
//static UINT16 value_u16;


//****************************************************************************
// Global VARIABLE DECLARATIONS
//****************************************************************************
UINT8 g_tx_check_tx_status = 0;

StructTimingInfo *g_stHdmiMacTx0InputTimingInfo = NULL;

StructHDMITxAudioInfo g_stHdmiMacTx0AudioInfo = {0};
StructHDMITxInputInfo g_stHdmiMacTx0InputInfo = {0};
StructHDMITxModeInfoType g_stHdmiMacTx0ModeInfo = {0};
BOOLEAN g_bHdmiMacTx0FindCtsTestEdid = _FALSE;
//#define TX0_GET_HDMI_MAC_TX0_FIND_CTS_TEST_EDID()                   (g_bHdmiMacTx0FindCtsTestEdid)

BOOLEAN g_bHdmiMacTx0HPDLowDeglitch;
BOOLEAN g_bHdmiMacTx2HPDToggleDetectedDFromIrq;
#if(_HDMI_MAC_TX0_PURE == _ON)
BOOLEAN g_bHdmiMacTx0HPDZ0ToggleTimeout;
BOOLEAN g_bHdmiMacTx0Z0ToggleTimeout;
#endif
StructHDMITxHPDTimerStatus g_stHdmiMacTx0HPDTimerStatus = {0};
BOOLEAN g_bHdmiMacTx0HPDToggleTimeout = _FALSE;
BOOLEAN g_bHdmiMacTx0HPDToggleDetectedDFromIrq = _FALSE;
BOOLEAN g_bHdmiMacTx0DownstreamPortStatusChange = _FALSE;
BOOLEAN g_bHdmiMacTx04KVICSendByVSIF = _FALSE;

#if(_AUDIO_SUPPORT == _ON)
EnumHdmiTxAudioInputType g_ucHdmiMacTx0AudioInputType=_TX_AUDIO_IN_RX_PACKET;

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
BOOLEAN g_bHdmiMacTx0CtsCodeCheckEvent;
BOOLEAN g_bHdmiMacTx0CtsCodeFwMode;
UINT32 g_ulHdmiMacTx0FwCTSVaule;
#endif

#endif

//*****bridge for different bank
extern BOOLEAN g_bHdmiMacTx0ModifyEdidEnable;
#define TX0_CLR_HDMI_MAC_TX0_MODIFY_EDID_ENABLE()                   (g_bHdmiMacTx0ModifyEdidEnable = _FALSE)

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
BOOLEAN g_bHdmiMacTx0SWPktDFF0Used;
BOOLEAN g_bHdmiMacTx0SWPktDFF1Used;
#endif

UINT8 g_usDpHdcpRx0ReceiverIdListIndex=0;

StructHdmiOutputStatus g_stHdmiOutputStatus = {0};

UINT8  bTxSupportHdmiTxOutput=0xff;
UINT8  bCheckEdidForHdmitxSupport=0;

UINT8  bOutoutDfpDeviceCapChange=0;
UINT8  bOutputDfpDeviceChange=0;

BOOLEAN g_bHDMITx0FirstHPD;

UINT8 u8hdcprun = 0;
StructHDMITxSCDCInfo stTxRepeaterOutputSCDCInfo = {0};
UINT8 hpd_low_high = 0;
UINT8 HdmiIsDisConnectAtAp=1;
//****************************************************************************
// External VARIABLE DECLARATIONS
//****************************************************************************
extern void _SCDC_check_RSED_CED_Status(void);
extern UINT8  ScalerStreamGetAudioMuteFlag(EnumOutputPort ucOutputPort);
extern void ScalerHdmiMacTx0PacketBypassConfig(void);
extern void ScalerHdmiMacTx0RepeaterSetPktHdrBypass_EmpPacketBypass(UINT8 enable);
UINT8 ScalerHdmiMacTx0SCDCRead(UINT8 scdc_offset);
extern void ScalerHdmiMacTx0CheckAviUpdate(UINT8 HdmiType);
extern void hdmitx_status_reset(void);

extern unsigned long long HDMI_TX_I2C_BUSID;
//extern SLR_VOINFO *pHDMITX_VOInfo;
extern UINT8 scalerSetTxRun;
extern UINT8 sendApHdmiConnect;
extern UINT32 stc;
extern UINT8 AudioFreqType;
extern BOOLEAN g_bHdmiMacTx0FindSpecialTv;
extern StructHdmitxDisplayInfo g_scaler_display_info;
extern MEASURE_TIMING_T timing_info;
extern BOOLEAN bEdidHdmiFormat;
extern UINT8 hdmitx_ap_state;
extern UINT8 need_sw_avi_packet;
//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************

void ScalerHdmiMacTx0HdcpReset(UINT8 * func, UINT32 line);

extern HDMITX_INIT_SETTING_TYPE hdmitx_init_setting;
#if(_AUDIO_SUPPORT == _ON)
void ScalerHdmiMacTx0AudioInputTypeSet(EnumHdmiTxAudioInputType type);
#endif

void ScalerHdmiMacTx0AudioInputTypeInit(void)
{
#if(_AUDIO_SUPPORT == _ON)
EnumHdmiTxAudioInputType AUDIO_SUPPORT_DEFAULT_IN_TYPE  = _TX_AUDIO_IN_RX_SPDIF; //_TX_AUDIO_IN_RX_PACKET //
#if 0
static UINT32 lastDumpStc_ACRPacket = 0;
static UINT32 lastDumpStc_AudioSet = 0;
#endif
#endif

#ifdef CONFIG_FORCE_AUDIO_PACKET_BYPASS
    if(lib_hdmitx_is_hdmi_bypass_support() == _TRUE)
        AUDIO_SUPPORT_DEFAULT_IN_TYPE = _TX_AUDIO_IN_RX_PACKET;
    else
        AUDIO_SUPPORT_DEFAULT_IN_TYPE = _TX_AUDIO_IN_RX_SPDIF;
#endif
#if(_AUDIO_SUPPORT == _ON)
    ScalerHdmiMacTx0AudioInputTypeSet(AUDIO_SUPPORT_DEFAULT_IN_TYPE);
    hdmitx_init_setting.init_setting.bSkipAudioPktBypassFlag = (AUDIO_SUPPORT_DEFAULT_IN_TYPE == _TX_AUDIO_IN_RX_SPDIF? _TRUE: _FALSE);
#else
    hdmitx_init_setting.init_setting.bSkipAudioPktBypassFlag = _TRUE;
#endif

    FatalMessageHDMITx("[HDMITX] Audio InputType=%d,", (UINT32)GET_HDMITX_AUDIO_INPUT_TYPE());
    FatalMessageHDMITx("SkipPktBypass=%d\n", (UINT32)hdmitx_init_setting.init_setting.bSkipAudioPktBypassFlag);

    return;
}
void ScalerHdmiMacTx0SetGcpToHwMode(void){
        if(gcpswtohw != _HDMI_TX_GCP_TOHW_JUMP)
            return;
        HDMITX_DTG_Wait_vsync_start(); // Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);
        HDMITX_DTG_Wait_Den_Stop_Done(); // Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);
        HDMITX_DTG_Wait_vsync_start(); // Scaler_Wait_for_event(TXSOCTG_TXSOC_pending_status_reg, TXSOCTG_TXSOC_pending_status_mv_den_end_event_mask,1);

        DebugMessageHDMITx("[HDMITX] sw to hw1\n");
        //runontime = 1;
        gcpswtohw = _HDMI_TX_GCP_TOHW_DONE;
        // HDMMI20 AVMUTE bypass control
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_sel_in(1));
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_sel_in(1));
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_bypass_en(1));
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_bypass_en(1));

        // HDMMI21 AVMUTE bypass control
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_sel_in(1));
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_sel_in(1));
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_bypass_en(1));
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_bypass_en(1));

        // HDMI20/21 dispD2tx forceBg
        rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_dispd2tx_force_bg_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_dispd2tx_force_bg_en(1));
}
// check EMP bypass buffer status
void ScalerHdmiMacTx0RepeaterCheckPktBypass_bufferStatus(void)
{
    UINT32 regValue;
    #define ALL_EMP_EN_MASK (HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid1_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_sbtm_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vtem_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_cvtem_en_mask\
                                                    |HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en_mask)

    if(rtd_inl(HDMITX_MISC_HD_EMP_CTL_EN_reg) & ALL_EMP_EN_MASK){
        // 1. info frame packet buffer error status
        regValue = rtd_inl(HDMITX_MISC_HD_BYPASS_CTRL04_reg);
        //HDMITX_MISC_HD_BYPASS_CTRL04_reg 0xB8019710 B[1]: aud_buf_ovf, B[0]: emp_buf_ovf
        if(regValue & HDMITX_MISC_HD_BYPASS_CTRL04_aud_buf_ovf_mask){
            FatalMessageHDMITx("[HDMI_TX] Audio bypass buffer Overflow!\n");
            rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL04_reg, ~HDMITX_MISC_HD_BYPASS_CTRL04_aud_buf_ovf_mask, HDMITX_MISC_HD_BYPASS_CTRL04_aud_buf_ovf_mask);
        }

        if(regValue & HDMITX_MISC_HD_BYPASS_CTRL04_emp_buf_ovf_mask){
            FatalMessageHDMITx("[HDMI_TX] EMP bypass buffer Overflow!\n");
            rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL04_reg, ~HDMITX_MISC_HD_BYPASS_CTRL04_emp_buf_ovf_mask, HDMITX_MISC_HD_BYPASS_CTRL04_emp_buf_ovf_mask);
        }

        // check RX packet bypass buffer WL
#ifdef _ENABLE_DUMP_BYPASS_BUFFER_WATER_LEVEL
  #ifndef CONFIG_MACH_RTD2801_ZEBU //keep dump in Zebu platform
        if(((UINT32)(rtd_inl(TIMER_SCPU_CLK90K_LO_reg) - lastDumpStc_MonitorFIFOStatus)) > 90000)
  #endif
        {
    #if 0// pending loop
            if(0xb80194C0 & _BIT16)
                HDMITX_DTG_Wait_vsync_start();
            else
                HDMITX_DTG_Wait_Den_Stop_Done();
    #endif

            value_u32 = rtd_inl(PPOVERLAY_OUTTG_OUTTG_new_meas0_linecnt_real_reg);

            //HDMITX_MISC_HD_BYPASS_CTRL05_reg 0xB8019714 B[7:0]: aud_wl, B[31]: aud_wl_pop
            rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL05_reg, ~HDMITX_MISC_HD_BYPASS_CTRL05_aud_wl_pop_mask, HDMITX_MISC_HD_BYPASS_CTRL05_aud_wl_pop_mask);
            rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL06_reg, ~HDMITX_MISC_HD_BYPASS_CTRL06_emp_wl_pop_mask, HDMITX_MISC_HD_BYPASS_CTRL06_emp_wl_pop_mask);
            udelay(1);
            value_u8 = HDMITX_MISC_HD_BYPASS_CTRL05_get_aud_wl(rtd_inl(HDMITX_MISC_HD_BYPASS_CTRL05_reg));
            InfoMessageHDMITx("[HDMI_TX] AUDIO/EMP FIFO WL=%x/", (UINT32)value_u8);

            //HDMITX_MISC_HD_BYPASS_CTRL06_reg 0xB8019718 B[9:0]: emp_wl, B[31]: emp_wl_pop
            //rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL06_reg, ~HDMITX_MISC_HD_BYPASS_CTRL06_emp_wl_pop_mask, HDMITX_MISC_HD_BYPASS_CTRL06_emp_wl_pop_mask);
            //udelay(1);
            value_u8 = HDMITX_MISC_HD_BYPASS_CTRL06_get_emp_wl(rtd_inl(HDMITX_MISC_HD_BYPASS_CTRL06_reg));
            InfoMessageHDMITx("%x@LC=%x\n", (UINT32)value_u8, (UINT32) value_u32);

            lastDumpStc_MonitorFIFOStatus = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
        }
#endif // #ifdef _ENABLE_DUMP_BYPASS_BUFFER_WATER_LEVEL
    }
    return;
}


void ScalerHdmiMacTx0EnablePtg(UINT8 ptgmode,UINT8 enable)
{
    //select ptg mode
   rtd_maskl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_rand_mode_mask,
    	HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_rand_mode(ptgmode));
    //enable hdmi  pattern gen
    rtd_maskl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_en_mask,
    	HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_en(enable));
}

UINT8 waitOutTgStable(void)
{

    UINT8 vgipen = HDMITX_VGIP_HDMITX_VGIP_CTRL_get_hdmitx_ivrun(rtd_inl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg));
    return TRUE;

    if(vgipen && (g_tx_check_tx_status ==  6)) // normal flow need wait outtg done
        return TRUE;
    else if(!vgipen)  // ptg mode no need wait outtg done
        return TRUE;
    else
        return FALSE;

}

#if(_HDCP2_SUPPORT == _ON)
extern void ScalerHdmiHdcp2Tx2Handler(UINT8 port);
extern void ScalerHdmiHdcp2TxSetAuthTimeOut(UINT8 port);
extern UINT8 ScalerHdmiHdcp2TxGetAuthTimeOut(UINT8 port);
extern void ScalerHdmiHdcp2TxClearAuthState(UINT8 port);
extern void ScalerSetHdmiHdcp2Tx2Auth(UINT8 port, UINT8 enable);
static EnumHDMITxHdcpState  HdmiTxHdcpState=_HDMI_TX_NOT_HDCP_AUTH;// 2:need clear auth state 1:auth handle 0 :not auth
extern UINT8 u8HdcpDisable;
extern void ScalerHdmiHdcp2Tx2ChangeAuthState(UINT8 port, UINT8 enumHdcp2AuthState);
//--------------------------------------------------
// Description  : HDMI Tx HDCP Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0HDCPHandler(void)
{
    if(ScalerHdmiTxGetBypassLinkTrainingEn() || u8HdcpDisable){
        return;
    }
    //if(!HDCP2_2_EN)
        //return;

        if(HdmiTxHdcpState == _HDMI_TX_HDCP_AUTH_AND_CLR_AUTH_STATE){
                ScalerHdmiHdcp2TxClearAuthState(0);
                HdmiTxHdcpState=_HDMI_TX_HDCP_AUTH_HANDLE;
                FatalMessageHDMITx("[HDMI Tx]open Hdcp2Tx2Auth timer\n");
                ScalerSetHdmiHdcp2Tx2Auth(0, 1);
        }

        if(HdmiTxHdcpState==_HDMI_TX_HDCP_AUTH_HANDLE){
            if(ScalerHdmiHdcp2TxGetAuthTimeOut(0)){
              ScalerHdmiMacTx0EnablePtg(1,1);
              rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL_reg, ~_BIT4, 0x00);
              rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_reg, ~_BIT4, 0x00);
               HdmiTxHdcpState = _HDMI_TX_HDCP_AUTH_DISCONNECT;
              FatalMessageHDMITx("[HDMI Tx]Hdcp2Tx Auth time out  \n");
              ScalerSetHdmiHdcp2Tx2Auth(0, 0);

            }

            //ScalerHdmiHdcp2Tx2Handler(0);
        }

        if(_HDMI_TX_HDCP_AUTH_DISCONNECT == HdmiTxHdcpState){
            HdmiTxHdcpState = _HDMI_TX_NOT_HDCP_AUTH;
            ScalerHdmiHdcp2Tx2ChangeAuthState(0, _HDCP_2_2_TX_STATE_IDLE);
            FatalMessageHDMITx("[HDMI Tx]close Hdcp2Tx2Auth timer\n");
            ScalerSetHdmiHdcp2Tx2Auth(0, 0);
        }
    return;
}
#endif

//--------------------------------------------------
// Description  : HDMI Tx0 Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0Handler(void)
{
    if(GET_HDMI_TX0_HPD_FIRST_PLUG())
        DebugMessageIgnore("[HDMI Tx] ScalerHdmiMacTx0Handler(), PowerState=%d ...\n", (UINT32)GET_OUTPUT_POWER_STATUS());

    if(GET_OUTPUT_POWER_STATUS() == _POWER_STATUS_NORMAL)
    {
        // HPD Detection Function
        ScalerHdmiMacTx0HPDProc();

        ScalerHdmiMacTx0ModifyEdid();

        if(scalerSetTxRun && (ScalerHdmiTx_Get_Timing_Status() != TX_TIMING_INIT_STATE))
        {
        	ScalerHdmiMacTx0JudgeHandler();

        	ScalerHdmiMacTx0ModeHandler();
        }

#if(_HDCP2_SUPPORT == _ON)
        ScalerHdmiMacTx0HDCPHandler();
#endif

    }

    if(GET_HDMI_TX0_HPD_FIRST_PLUG())
        DebugMessageIgnore("[HDMI Tx] ScalerHdmiMacTx0Handler() Done!!\n");
    return;
}
UINT8 ScalerHdmiJudgeTxConnect(void){
    FatalMessageHDMITx("[HDMITX] ScalerHdmiJudgeTxConnect DS[%d],EDID[%d]\n", (UINT32)GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS(), (UINT32)GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG());
     if( GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() && GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG()) 
        return _TRUE;

     return _FALSE;
}
void ScalerHdmiJudgeReloadEdid(void)
{
    if(GET_HDMI_MAC_TX0_HPD_STATUS() && GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() && GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG()) {
	// fix by zhaodong	
	//core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HDMITX_CONNECT, 0);//return connect to ap
        sendApHdmiConnect = 1;
        HdmiIsDisConnectAtAp = 0;
        if(hdmitx_get_ap_state() == HDMITX_AP_INIT)
            hdmitx_set_ap_state(HDMITX_AP_CONNECT);
        FatalMessageHDMITx("[HDMITX][%d] no reload EDID !\n", (UINT32)hdmitx_get_ap_state());
    }
}

//--------------------------------------------------
// Description  : HDMI Tx Initial
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0Initial(void)
{
    DebugMessageIgnore("[HDMI Tx] ScalerHdmiMacTx0Initial() ...\n");

    SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_NO_HPD_EVENT);
    CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();
    SET_HDMI_MAC_TX0_MODE_STATE(_HDMI_TX_MODE_STATUS_INITIAL);
    CLR_HDMI_MAC_TX0_MODE_STATE_CHANGE();
    SET_HDMI_MAC_TX0_STREAM_SOURCE(_HDMI_TX_SOURCE_NONE);

    SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(8);
    SET_HDMI_MAC_TX0_DEEP_COLOR_CLK(0);
    SET_HDMI_MAC_TX0_INPUT_REPEAT_NUM(0);
    SET_HDMI_MAC_TX0_INPUT_FRAME_RATE(60);

    CLR_HDMI_MAC_TX0_DOWNSTREAM_STATUS_CHANGE();

    SET_HDMI_MAC_TX0_AUDIO_CH_NUM(0);
    SET_HDMI_MAC_TX0_AUDIO_FREQ(0);
    CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();

    SET_HDMI_TX0_HPD_FIRST_PLUG();

    // disable packet bypass enable
    ScalerHdmiMacTx0RepeaterSetPktHdrBypass_EmpPacketBypass(_DISABLE);

    // Disable HDMI20 Tx module
    rtd_outl(HDMITX20_MAC0_SCHEDULER_0_reg, 0x00);

    // Set Sp_pst based on sp_pst_blank and sp_pst_active Reg
    rtd_maskl(HDMITX20_MAC1_HDMI_SPDIF_CONTROL2_reg, ~_BIT5, 0x00);
    //Set scheduler_gcp_to_di_en to 1
    rtd_maskl(HDMITX20_MAC1_HDMI_SPDIF_CONTROL2_reg, ~_BIT6, _BIT6);
    // Disable ACR Packet
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT4, _BIT4);

    //Reset N/CTS regen
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_mux_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_mux(0)); // N/CTS mux: 0x0 : N/CTS from spdif. Original path, 0x1 : N/CTS from regen. The new method.
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en(0));

#if(_HDMI_MAC_TX0_DP_PLUS == _ON)
    rtd_maskl(PBB_70_HPD_CTRL, ~(_BIT6 | _BIT5), 0x00);
#endif
    //disable pfifo_en
    rtd_maskl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg, ~HDMITX20_ON_REGION_PIXEL_CONV_P1_pfifo_en_mask, HDMITX20_ON_REGION_PIXEL_CONV_P1_pfifo_en(0x0));

    // Set Rekey window size = 12
    //rtd_outl(HDMITX20_MAC0_SCHEDULER_7_reg, 0x0B);
    //SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(0x0B + 0x01);
    rtd_outl(HDMITX20_MAC0_SCHEDULER_7_reg, 0x39);
    SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(0x39 + 0x01);
    // HDCP Bug
    rtd_maskl(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, ~_BIT0, _BIT0);

    // HDCP compatibility, (default 0xCA) - 0x20
    rtd_outl(HDMITX20_MAC0_SCHEDULER_5_reg, 0xAA);

    // Enable HPD Detection
#if(_HDMI_MAC_TX0_PURE == _ON)
    rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, ~_BIT7, _BIT7);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
    rtd_maskl(PBB_70_HPD_CTRL, ~_BIT7, _BIT7);
#endif

#if(_HDMI_MAC_TX0_DP_PLUS == _ON)
    // Tx Small FIFO User Mode
    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT7, _BIT7);

    // Lane Swap
    rtd_outl(HDMITX20_ON_REGION_TXFIFO_CTRL2_reg, 0x41);
    rtd_outl(HDMITX20_ON_REGION_TXFIFO_CTRL3_reg, 0x03);
#endif
    // Phy Initial
    ScalerHdmiPhyTxInitial(ScalerHdmiMacTxPxMapping(_TX0));

    ScalerHdmiTxHPDInitial();

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    // Hdmi21 Mac Initial
    ScalerHdmi21MacTxInitial(ScalerHdmiMacTxPxMapping(_TX0));
#endif

    // [RL6583-3452] fix HDMITX 2.0 6G Guard Bang Disparity
//#ifndef FIX_ME_HDMITX_BRING_UP // [TBD]
    rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~HDMITX20_MAC1_VFIFO_8_tmds_disp_cnt_sel_mask, HDMITX20_MAC1_VFIFO_8_tmds_disp_cnt_sel_mask);
//#endif

    CLR_HDMI21_MAC_TX_DSCE_EN(_TX0);

    DebugMessageHDMITx("[HDMI Tx] ScalerHdmiMacTx0Initial() Done!!\n");
    return;
}

//--------------------------------------------------
// Description  : HDMI Tx0 Ac On Initial
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0AcOnInitial(void)
{
}

void ScalerHdmiMacTx0AdjustHdmiMode(void)
{
    UINT8 value;

    value = GET_HDMI_MAC_TX0_SUPPORT();
    // [DANBU-844] Any device with an E-EDID that does not contain a CEA Extension or does not contain an HDMI VSDB of any valid length shall be treated by the Source as a DVI device.
    // if EDID no suppport hdmi mode, always set false, else if bypass mode, sync with rx status,  else sync with edid
    if(GET_HDMI_EDID_HDMI_MODE() == FALSE){
        // don't change HDMI mode when sink device is DVI device
        SET_HDMI_MAC_TX0_SUPPORT(FALSE);
    }else if((g_scaler_display_info.VSC_isConnect) && (get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE)){
        SET_HDMI_MAC_TX0_SUPPORT(timing_info.mode != HDMITX_MODE_DVI);
    }else{
        SET_HDMI_MAC_TX0_SUPPORT(GET_HDMI_EDID_HDMI_MODE());
    }

    FatalMessageHDMITx("[HDMITX] HDMI Mode=%d->%d\n", (UINT32) value, (UINT32)GET_HDMI_MAC_TX0_SUPPORT());
    return;
}
//--------------------------------------------------
// Description  : Hdmi Tx PowerProc
// Input Value  : EnumPowerAction
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0PowerProc(EnumPowerAction enumPowerAction)
{
    switch(enumPowerAction)
    {
        case _POWER_ACTION_AC_ON_TO_NORMAL:
        case _POWER_ACTION_PS_TO_NORMAL:
        case _POWER_ACTION_OFF_TO_NORMAL:
        case _POWER_ACTION_OFF_TO_FAKE_OFF:

            DebugMessageHDMITx("[HDMI Tx] Power Status to Normal@ScalerHdmiMacTx0PowerProc()\n");

            ScalerHdmiMacTx0PowerGroupPSMode(_OFF);

            // set AV Mute before HDMITX init
            ScalerHdmiTxMuteEnable(_ENABLE);
#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON) // [TEST]support fast timing change flow
            if(ScalerHdmiTxGetFastTimingSwitchEn()){
                // enable gap
                rtd_maskl(HDMITX21_MAC_HDMI21_FRL_19_reg, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
                HDMITX_DTG_Wait_Den_Stop_Done();
                SET_HDMI_MAC_TX0_MODE_STATE(_HDMI_TX_MODE_STATUS_INITIAL);
            }else
#endif
                ScalerHdmiMacTx0Initial();

//#ifndef H5X_HDMITX_FIXME // enable HDMITX interrupt in SOC platform
//            ScalerHdmiTx0_H5xMac_interruptEnable(_ENABLE);
//#endif

            SET_OUTPUT_POWER_STATUS(_POWER_STATUS_NORMAL);
            break;

        case _POWER_ACTION_NORMAL_TO_PS:
        case _POWER_ACTION_NORMAL_TO_OFF:
        case _POWER_ACTION_AC_ON_TO_OFF:

            DebugMessageHDMITx("[HDMI Tx] Power Status to Saving\n");

            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_INITIAL);

            ScalerHdmiMacTx0Reset();

            ScalerHdmiMacTx0PowerGroupPSMode(_ON);

            SET_OUTPUT_POWER_STATUS(_POWER_STATUS_OFF);
            break;

        case _POWER_ACTION_PS_TO_OFF:

            DebugMessageHDMITx("[HDMI Tx] Power Status to OFF\n");
            SET_OUTPUT_POWER_STATUS(_POWER_STATUS_OFF);
            break;

        default:

            break;
    }
}
//--------------------------------------------------
// Description  : Hdmi Tx Clr Config
// Input Value  : VDB TYPE
// Output Value : VIC TABLE
//--------------------------------------------------
void ScalerHdmiMacTx0UnplugNotify(void)
{
    SET_OUTPUT_DFP_DEVICE_CURRENT_TYPE(ScalerHdmiMacTxPxMapping(_TX0), _DFP_TYPE_NONE);

    SET_OUTPUT_DFP_DEVICE_VERSION(ScalerHdmiMacTxPxMapping(_TX0), _DFP_VERSIOIN_NONE);

    CLR_OUTPUT_DFP_DEVICE_VALID(ScalerHdmiMacTxPxMapping(_TX0));

    SET_OUTPUT_DFP_DEVICE_CURRENT_SINK_COUNT(ScalerHdmiMacTxPxMapping(_TX0), 0);

    // Set DownStream Device Change
    SET_OUTPUT_DFP_DEVICE_CHANGE(ScalerHdmiMacTxPxMapping(_TX0));

    // Load Default Edid
    ScalerMcuLoadDefaultEdidByTxPort(ScalerHdmiMacTxPxMapping(_TX0));
}
//--------------------------------------------------
// Description  : HDMI HPD Detection
// Input Value  : None
// Output Value : None
//--------------------------------------------------
#define MONITOR_HPD_HL_DELAY
void ScalerHdmiMacTx0HPDProc(void)
{
    UINT8 curHpdStatus=0;
#if(_HDMI_MAC_TX0_PURE == _ON)
    UINT8 ucIBHNTRIMCurrent = 0x00;
#endif
    EnumHDMITxZ0Detect z0Detect_state = 0;
    I2C_HDMITX_PARA_T i2c_para_info;
    hdmitx20_hpd_detection_hdmi_hpd_ctrl0_RBUS hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg;
    hdmitx20_hpd_detection_hdmi_hpd_ctrl1_RBUS hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg;

    i2c_para_info.addr = _EDID_ADDRESS;
    i2c_para_info.sub_addr_len = 1;
    i2c_para_info.read_len = 4;
    i2c_para_info.wr_len = 0x0;
    if(GET_HDMI_TX0_HPD_FIRST_PLUG())
        DebugMessageIgnore("[HDMITX] ScalerHdmiMacTx0HPDProc()...\n");

    SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_NO_HPD_EVENT);

    curHpdStatus = ScalerHdmiMacTx0GetHpdStatus();
    hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.regValue = rtd_inl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg);
    hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.regValue = rtd_inl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg);
    // [H5X_HDMITX_BYPASS_HPD_EDID]

#if 0 // TEST
    value_u16 = (UINT16) rtd_inl(TIMER_SCPU_CLK90K_LO_reg) - lastStc_TxHpdCheck;
    FatalMessageHDMITx("[HDMITX] HPD Period=%x(ms)\n", (UINT32)value_u16);
    lastStc_TxHpdCheck = stc;
#endif

    if(ScalerHdmiTxGetBypassLinkTrainingEn()){
#if (_HDMI21_TX_SUPPORT == _ON)
        if(ScalerHdmiTxGetTargetFrlRate() != 0){ // HDMI2.1 (FRL > 0)
            if(GET_HDMI_MAC_TX0_MODE_STATE() != _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON){
                // force TX mode state = HDMI21_LINK_ON
                SET_HDMI_MAC_TX0_MODE_STATE(_HDMI_TX_MODE_STATUS_HDMI21_LINK_ON);
                SET_HDMI21_MAC_VIDEO_START(_TX0);
                SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0, _LINK_TRAING_PASS);
                SET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(_TX0, _TX_STREAM_READY);
                CLR_HDMI21_MAC_TX_LTS_PASS_POLLING(_TX0);

                SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_NO_HPD_EVENT);
                SET_HDMI_MAC_TX0_STREAM_SOURCE(_HDMI_TX_SOURCE_NONE);

                // DSC enable control
#if(_DSC_SUPPORT == _ON)
                SET_HDMI21_MAC_TX_DSCE_EN(_TX0, ScalerHdmiTxGetDSCEn());
#endif
                SET_HDMI21_MAC_TX_INPUT_FROM(_TX0, _INPUT_FROM_DP);
                SET_HDMI21_MAC_TX_DOWNSTREAM_TYPE(_TX0, _DOWN_STREAM_VESA);
            }
        }else
#endif // #if (_HDMI21_TX_SUPPORT == _ON)
        {

            if(GET_HDMI_MAC_TX0_MODE_STATE() != _HDMI_TX_MODE_STATUS_LINK_ON){
                // force TX mode state = LINK_ON
                SET_HDMI_MAC_TX0_MODE_STATE(_HDMI_TX_MODE_STATUS_LINK_ON);
                SET_HDMI_MAC_TX0_STREAM_SOURCE(_HDMI_TX_SOURCE_NONE);

#if (_HDMI21_TX_SUPPORT == _ON)
                // reset link training result in HDMI20 mode
                SET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(_TX0, _LINK_TRAING_NONE);

                //since HDCP14 and HDCP20 not enable
                SET_HDMI21_MAC_FORCE_LEGACY(_TX0);
#endif // #if (_HDMI21_TX_SUPPORT == _ON)
            }
        }

        SET_HDMI_MAC_TX0_HPD_STATUS(curHpdStatus);
        // force EDID to HDMI mode
        SET_HDMI_MAC_TX0_SUPPORT(TRUE);

        // set stream type to SST
        SET_OUTPUT_STREAM_TYPE(_STREAM_TYPE_SST);

        // force downstream status = ready
        SET_HDMI_MAC_TX0_DOWNSTREAM_STATUS();

        CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();
        TX0_CLR_HDMI_MAC_TX0_MODIFY_EDID_ENABLE();
        SET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(_TRUE);
        if(hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.fhpd_int
            || hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int)
        {
            NoteMessageHDMITx("[HDMITX] ScalerHdmiMacTx0HPDProc()\n");
            NoteMessageHDMITx("[HDMITX] HPD/DEB=%d/", (UINT32)GET_HDMI_MAC_TX0_HPD_STATUS());
            NoteMessageHDMITx("%d, ",(UINT32)GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS());
            NoteMessageHDMITx("LH/HL/DS=%d/", (UINT32)hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.fhpd_int);
            NoteMessageHDMITx("%d/", (UINT32)hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int);
            NoteMessageHDMITx("%d\n", (UINT32)GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS());
            if(hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.fhpd_int)
                rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, ~_BIT1, _BIT1);
            if(hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int)
                rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~_BIT1, _BIT1);
        }

        return;
    }

    if((GET_HDMI_MAC_TX0_HPD_STATUS() != curHpdStatus)
        || hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.fhpd_int
        ||  hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int)
    {
            FatalMessageHDMITx("[HDMITX] ScalerHdmiMacTx0HPDProc()-11\n");
            FatalMessageHDMITx("[HDMITX] HPD/DEB=%d->%d/%d,LH/HL/DS=%d/%d/%d\n", (UINT32)GET_HDMI_MAC_TX0_HPD_STATUS(), (UINT32)curHpdStatus,
                 (UINT32)GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS(), (UINT32)hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.fhpd_int, (UINT32)hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int,
                  (UINT32)GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS());

        SET_HDMI_MAC_TX0_HPD_STATUS_CHANGE();

        if(curHpdStatus == HIGH){
            // force re-start link training flow after detect HDMI cable connect
            if(hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.fhpd_int){
                if(ScalerHdmiTx_Get_Timing_Status() != TX_TIMING_INIT_STATE){
                    ScalerHdmiTxSetForceLinkTrainingEn(_ENABLE);
                 } else {
                     if((!HdmiIsDisConnectAtAp)&&(hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg.fhpd_int) &&
                        (hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int))
                            hpd_low_high = 1;
                    CLR_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0));
                 }
            }
        }else{
            // set av mute when detect unplug
            if(hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int)
                ScalerHdmiTxMuteEnable(_ENABLE);

            if(hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg.hpd_fal_int){
			//fix by zhaodong	
			 //core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HDMITX_DISCONNECT, 0);//send disconnect msg to ap
#if(_HDCP2_SUPPORT == _ON)
                HdmiTxHdcpState = _HDMI_TX_HDCP_AUTH_DISCONNECT;
#endif
#ifdef MONITOR_HPD_HL_DELAY
                if(g_bHdmiMacTx0FindSpecialTv)
                    mdelay(1000);//for i2c reconnect
#endif

                CLR_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0));
                CLR_HDMI_MAC_TX0_DOWNSTREAM_STATUS();
                HdmiIsDisConnectAtAp = 1;
                hdmitx_status_reset();
                sendApHdmiConnect = 0;
                SET_HDMI_MAC_TX0_MODE_STATE(_HDMI_TX_MODE_STATUS_INITIAL);

                 FatalMessageHDMITx("[HDMITX] send AP_HDMITX_DISCONNECT() !\n");
            }
        }

#if(_HDMI_MAC_TX0_PURE == _ON)
        rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, ~_BIT1, _BIT1);
        rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~_BIT1, _BIT1);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
        rtd_maskl(PBB_71_HPD_IRQ, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT4 | _BIT1));
#endif

        SET_HDMI_MAC_TX0_HPD_STATUS(curHpdStatus);

        if(GET_HDMI_MAC_TX0_HPD_LOW_DEGLITCH() == _TRUE)
        {
            if(GET_HDMI_MAC_TX0_HPD_STATUS() == _HIGH)
            {
                HDMITX_MDELAY(2);

                if(curHpdStatus == _HIGH)
                {
                    CLR_HDMI_MAC_TX0_HPD_LOW_DEGLITCH();

                    SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_RISING_EVENT);
                }
            }
        }
        else
        {
            if(GET_HDMI_MAC_TX0_HPD_STATUS() == _LOW)
            {
                HDMITX_MDELAY(2);

                if(curHpdStatus == _LOW)
                {
                    SET_HDMI_MAC_TX0_HPD_LOW_DEGLITCH();
                }
            }
        }
    }
            //NoteMessageHDMITx("[HDMITX] ScalerHdmiMacTx0HPDProc()-2\n");
            //NoteMessageHDMITx("[HDMITX] HPD/DEB=%d/%d", (UINT32)GET_HDMI_MAC_TX0_HPD_STATUS(), (UINT32)GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS());
            //NoteMessageHDMITx("[HDMITX] LH=%d", (UINT32)HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_get_fhpd_int(hdmitx20_hpd_detection_hdmi_hpd_ctrl0_reg_regValue));
            //NoteMessageHDMITx("[HDMITX] HL=%d", (UINT32)HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_get_hpd_fal_int(hdmitx20_hpd_detection_hdmi_hpd_ctrl1_reg_regValue));
            //NoteMessageHDMITx("[HDMITX] DS=%d\n", (UINT32)GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS());

#if(_HDMI_MAC_TX0_PURE == _ON)

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    if(GET_HDMI21_MAC_TX_FRL_SETPHY(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _FALSE)
#endif
    {
        if(GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)
        {
            // Enable HDMI TX Power and CMU Signal
            ucIBHNTRIMCurrent = ScalerHdmiPhyTxGetIBHNTrim(ScalerHdmiMacTxPxMapping(_TX0));
            ScalerHdmiPhyTxSetIBHNTrim(ScalerHdmiMacTxPxMapping(_TX0), 0x02);
            ScalerHdmiPhyTxPowerOn(ScalerHdmiMacTxPxMapping(_TX0));
            ScalerHdmiPhyTxSetCMUSignal(ScalerHdmiMacTxPxMapping(_TX0), _ENABLE);
        }
    }

    // Enable HDMI termination detect
    ScalerHdmiPhyTxSetZ0Detect(ScalerHdmiMacTxPxMapping(_TX0), _ENABLE);
    // avoid if and else if read different value
    z0Detect_state = ScalerHdmiPhyTxZ0Detect(ScalerHdmiMacTxPxMapping(_TX0));
    DebugMessageHDMITx("[HDMITX] z0=%d,ds=%d,deb=%d,hpd_chg=%d\n",(UINT32)z0Detect_state,(UINT32)GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS(),
        (UINT32)GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS(),(UINT32)GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE());
    // Z0 Detection = Low
    if((z0Detect_state == _HDMI_TX_Z0_CLK_LOW) && (GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() == _HIGH))
    {
        // Z0 Falling Edge Active Z0_100ms_Toggle Timer
        if((GET_HDMI_MAC_TX0_Z0_TOGGLE_TIMER_ACTIVE() == _FALSE) && (GET_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT() == _FALSE))
        {
            ScalerTimerActiveTimerEvent(_TOGGLE_DEBOUNCE_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT);

            SET_HDMI_MAC_TX0_Z0_TOGGLE_TIMER_ACTIVE();

            CLR_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT();
        }
    }
    else if(z0Detect_state == _HDMI_TX_Z0_ALL_LOW)
#endif
    {
        if(GET_HDMI_MAC_TX0_HPD_STATUS() == _LOW)
        {
#if(_HDMI_MAC_TX0_PURE == _ON)
            // HPD Falling Edge Active HPD_100ms_Toggle Timer
            if(GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() == _HIGH)
            {
                if((GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE() == _FALSE) && (GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT() == _FALSE))
                {
                    ScalerTimerActiveTimerEvent(_TOGGLE_DEBOUNCE_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);

                    SET_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE();

                    CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT();
                }
            }
#endif

            // pure HDMI : Status Change Flag been Used When Z0=HPD After HPD Rising/Falling, (One Time)
            if(GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE() == _TRUE)
            {

                CLR_HDMI_MAC_TX0_HPD_STATUS_CHANGE();

                // fix this when call back function done
                // Cancel & Active HPD_UNPLUG_DEBOUNCE Timer, (Use Reactive Fx Here)
                ScalerTimerReactiveTimerEvent(_TOGGLE_DEBOUNCE_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);

                SET_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE();

#if(_HDMI_MAC_TX0_PURE == _ON)
                CLR_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT();
#endif

                CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();



                // Clear HPD Edge Detect Flag for Debounce Period Glitch Check.
#if(_HDMI_MAC_TX0_PURE == _ON)
                rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, ~_BIT1, _BIT1);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
                rtd_maskl(PBB_71_HPD_IRQ, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT4 | _BIT1));
#endif
            }
            else
            {
                // Unplug 300ms Timer Debounce Done and Success
                if(GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS() == _TRUE)
                {
                    CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();

                    // Check Down Stream Sink Device EDID Still Exist
                    subEdidAddr[0] = 0x00;
                    if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x04,0x01, _HDMI21_TX0_IIC) == _TRUE)
                    {
                        // Cancel & Active HPD_UNPLUG_DEBOUNCE Timer, (Use Reactive Fx Here)
                        ScalerTimerReactiveTimerEvent(_TOGGLE_DEBOUNCE_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);

                        SET_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE();

                        DebugMessageHDMITx("[HDMI_TX] HPD low but read EDID OK@ScalerHdmiMacTx0HPDProc()\n");
                    }
                    else
                    {
                        SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_UNPLUG_EVENT);
                    }
                }
            }
        }
#if(_HDMI_MAC_TX0_PURE == _ON)
        else  // GET_HDMI_MAC_TX0_HPD_STATUS() == _HIGH
        {
            // hpd status=high, z0=all_low,debounce=true, rerun hpd change flow
            if(GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS())
                SET_HDMI_MAC_TX0_HPD_STATUS_CHANGE();

            // HPD Rising Edge Cancel HPD_100ms_Toggle Timer
            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);

            CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE();

            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);

            CLR_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE();

            CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();
        }
    }
    else // Z0 Detection = High
    {
        // Z0 Rising Edge Cancel Z0_100ms_Toggle Timer
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_Z0_TOGGLE_EVENT);

        CLR_HDMI_MAC_TX0_Z0_TOGGLE_TIMER_ACTIVE();

        if(GET_HDMI_MAC_TX0_HPD_STATUS() == _LOW)
        {

            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);

            CLR_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE();

            CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();

            if(GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() == _HIGH)
            {
                if((GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE() == _FALSE) && (GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT() == _FALSE))
                {
                    ScalerTimerActiveTimerEvent(_TOGGLE_DEBOUNCE_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);

                    SET_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE();

                    CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT();
                }
            }
        }
#endif
        else // GET_HDMI_MAC_TX0_HPD_STATUS() == _HIGH
        {

#if(_HDMI_MAC_TX0_PURE == _ON)
            // HPD Rising Edge Cancel HPD_100ms_Toggle Timer
            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_HPD_TOGGLE_EVENT);

            CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE();

            // Check Z0 Toggle Timeout Flag
            if(GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() == _HIGH)
            {
                if(GET_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT() == _TRUE)
                {
                    CLR_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT();

                    SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_Z0_TOGGLE_EVENT);
                }
            }
#endif

            if(GET_HDMI_MAC_TX0_HPD_EVENT()){
                NoteMessageHDMITx("[HDMI_TX] HPD Event=%d,", (UINT32)GET_HDMI_MAC_TX0_HPD_EVENT());
                NoteMessageHDMITx("HPD_STATUS_CHANGE()=%d\n", (UINT32)GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE());
            }
            // pure HDMI : Status Change Flag been Used When Z0=HPD After HPD Rising/Falling, (One Time)
            if(GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE() == _TRUE)
            {

                CLR_HDMI_MAC_TX0_HPD_STATUS_CHANGE();
                // fix this when call back function done
                // Cancel & Active HPD_PLUG_DEBOUNCE Timer, (Use Reactive Fx Here)
                ScalerTimerReactiveTimerEvent(_PLUG_DEBOUNCE_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);

                SET_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE();

                CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();


                // Clear HPD Edge Detect Flag for Debounce Period Glitch Check.
#if(_HDMI_MAC_TX0_PURE == _ON)
                rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~_BIT1, _BIT1);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
                rtd_maskl(PBB_71_HPD_IRQ, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT4 | _BIT1));
#endif

                // Check when Down Stream Sink Device Steady After Plug Event
                if(GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() == _HIGH)
                {
                    // Check if EDID Diff After HPD 0->1 Rising Edge (Must under Link Off/On)
                    if((GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE) && (ScalerHdmiMacTx0CheckEdidDifference() == _TRUE))
                    {
                        SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_EDID_CHANGE_EVENT);
                    }
#if(_HDMI_MAC_TX0_PURE == _ON)
                    else if(GET_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT() == _TRUE)
                    {
                        CLR_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT();
                        CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT();
                        SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_Z0_TOGGLE_EVENT);
                    }
#endif
                    else if(GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT() == _TRUE)
                    {
#if(_HDMI_MAC_TX0_PURE == _ON)
                        CLR_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT();
#endif

                        CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT();
                        SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_TOGGLE_EVENT);
                    }
                }
            }
            else
            {

                if(GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS())
                    NoteMessageHDMITx("[HDTX]HPD_XMS_DEB=%d\n", (UINT32)GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS());
                // Plug 30ms Timer Debounce Done and Success
                if(GET_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS() == _TRUE)
                {
                    CLR_HDMI_MAC_TX0_HPD_XMS_DEBOUNCE_STATUS();

#if(_HDMI_PLUG_EVENT_CHECK_EDID == _ON)
                    // Check Down Stream Sink Device EDID Exist

                    subEdidAddr[0] = 0x00;
                    i2c_para_info.read_len = 16;
                    if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_HDMI21_SCDC_SLAVE_ADDR, subEdidAddr, 0x01, pEdidData,0x10,0x01, _HDMI21_TX0_IIC) == _FALSE)
                    {
                        // Cancel & Active HPD_PLUG_DEBOUNCE Timer, (Use Reactive Fx Here)
                        ScalerTimerReactiveTimerEvent(_PLUG_DEBOUNCE_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_HPD_DEBOUNCE_EVENT);

                        SET_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE();

                        // Clear HPD Edge Detect Flag for Debounce Period Glitch Check.
#if(_HDMI_MAC_TX0_PURE == _ON)
                        rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~_BIT1, _BIT1);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
                        rtd_maskl(PBB_71_HPD_IRQ, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT4 | _BIT1));
#endif

                        DebugMessageHDMITx("[HDMI_TX] HPD High but Read EDID Fail@ScalerHdmiMacTx0HPDProc()_01()\n");
                    }
                    else
#endif
                    {
                        SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_PLUG_EVENT);
                    }
                }
            }
        }
    }

#if(_HDMI_MAC_TX0_PURE == _ON)

    // Disable HDMI termination detect
    ScalerHdmiPhyTxSetZ0Detect(ScalerHdmiMacTxPxMapping(_TX0), _DISABLE);
#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    if(GET_HDMI21_MAC_TX_FRL_SETPHY(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _FALSE)
#endif
    {
        if(GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)
        {
            // Disable HDMI TX Power and CMU Signal
            ScalerHdmiPhyTxSetCMUSignal(ScalerHdmiMacTxPxMapping(_TX0), _DISABLE);
            //turn off phy power will cause fifo unstable
            //ScalerHdmiPhyTxPowerOff(ScalerHdmiMacTxPxMapping(_TX0));
            ScalerHdmiPhyTxSetIBHNTrim(ScalerHdmiMacTxPxMapping(_TX0), ucIBHNTRIMCurrent);
        }
    }
#endif
    if((GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_PLUG_EVENT) || (GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_UNPLUG_EVENT))
    {
        if(GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() == GET_HDMI_MAC_TX0_HPD_STATUS())
        {
            SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_NO_HPD_EVENT);
        }
    }

    if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_EDID_CHANGE_EVENT)
    {
        SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_UNPLUG_EVENT);
    }
    if((GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_PLUG_EVENT) || (GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_UNPLUG_EVENT))
    {
        CLR_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0));

        if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_UNPLUG_EVENT)
        {
            ScalerHdmiMacTx0UnplugNotify();

            CLR_HDMI_MAC_TX0_DOWNSTREAM_STATUS();

#if(_HDMI_MAC_TX0_PURE == _ON)
            CLR_HDMI_MAC_TX0_HPD_Z0_TOGGLE_TIMEOUT();

            CLR_HDMI_MAC_TX0_Z0_TOGGLE_TIMEOUT();
#endif

            CLR_HDMI_MAC_TX0_HPD_TOGGLE_TIMEOUT();

#if(_HDMI_MAC_TX0_PURE == _ON)
            // Disable Rising/Falling IRQ
            rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, ~(_BIT1 | _BIT0), 0x00);
            rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~(_BIT1 | _BIT0), 0x00);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
            // Disable Rising/Falling IRQ
            rtd_maskl(PBB_72_HPD_IRQ_EN, ~_BIT1, 0x00);
            rtd_maskl(PBB_72_HPD_IRQ_EN, ~_BIT4, 0x00);
#endif
        }
        else if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_PLUG_EVENT)
        {
            SET_HDMI_MAC_TX0_DOWNSTREAM_STATUS();

#if(_HDMI_MAC_TX0_PURE == _ON)
            // Enable Falling IRQ
            rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~(_BIT1 | _BIT0), _BIT0);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
            // Enable Falling IRQ
            rtd_maskl(PBB_72_HPD_IRQ_EN, ~_BIT4, _BIT4);
#endif
        }

        SET_HDMI_MAC_TX0_DOWNSTREAM_STATUS_CHANGE();

        FatalMessageHDMITx("[TX]DS chg=%d\n", (UINT32)GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS());
    }

    if(GET_HDMI_MAC_TX0_HPD_DEBOUNCE_TIMER_ACTIVE() == _FALSE)
    {
#if(_HDMI_MAC_TX0_PURE == _ON)
        if((GET_HDMI_MAC_TX0_Z0_TOGGLE_TIMER_ACTIVE() == _FALSE) && (GET_HDMI_MAC_TX0_HPD_TOGGLE_TIMER_ACTIVE() == _FALSE))
#endif
        {
            if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_NO_HPD_EVENT)
            {
                if(GET_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ() == _TRUE)
                {
                    CLR_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ();

                    SET_HDMI_MAC_TX0_HPD_EVENT(_HDMI_TX_HPD_TOGGLE_EVENT);

                    DebugMessageHDMITx("HDMI_TX0: HPD Toggle Detected From INT@ScalerHdmiMacTx0HPDProc()\n");
                }
            }
            else
            {
                CLR_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ();
            }
        }
    }

    if((GET_HDMI_MAC_TX0_HPD_EVENT() != _HDMI_TX_NO_HPD_EVENT) && (GET_HDMI_MAC_TX0_HPD_EVENT() != _HDMI_TX_HPD_UNPLUG_EVENT))
    {
#if(_HDMI_MAC_TX0_PURE == _ON)
        // Enable Falling IRQ
        rtd_maskl(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL1_reg, ~(_BIT1 | _BIT0), _BIT0);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
        // Enable Falling IRQ
        rtd_maskl(PBB_72_HPD_IRQ_EN, ~_BIT4, _BIT4);
#endif

        CLR_HDMI_MAC_TX0_HPD_TOGGLE_DETECTED_FROM_IRQ();
    }


    if(GET_HDMI_MAC_TX0_HPD_EVENT() != _HDMI_TX_NO_HPD_EVENT)
    {
        DebugMessageHDMITx("[HDMITX] HPD Event=%d\n", (UINT32)GET_HDMI_MAC_TX0_HPD_EVENT());
    }
    return;
}

extern UINT8 bHD21_480P_576P;
//--------------------------------------------------
// Description  : HDMI Tx Judge Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0JudgeHandler(void)
{

    if((GET_HDMI_TX0_HPD_FIRST_PLUG())&& (GET_HDMI_MAC_TX0_HPD_STATUS() == _HIGH)){
            DebugMessageHDMITx("[HDMITX] Tx0JudgeHandler()\n");
            //DebugMessageHDMITx("[HDMITX] TX/HPD state=%d/%d\n", (UINT32)GET_HDMI_MAC_TX0_MODE_STATE(), (UINT32)GET_HDMI_MAC_TX0_HPD_EVENT());
    }
    CLR_HDMI_MAC_TX0_MODE_STATE_CHANGE();

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    if(GET_HDMI_MAC_TX0_MODE_STATE() > _HDMI_TX_MODE_STATUS_INITIAL) {
        if((GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_TOGGLE_EVENT) || (GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_Z0_TOGGLE_EVENT))
        {
            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_TRAINING);

            FatalMessageHDMITx("[HDMI_TX] HPD Toggle --> Link Training\n");

            DebugMessageWuzanne("[HDMI_TX] wuzanne test : HDMI Tx0 Judge to Link Training (HPD Toggle)\n");

            ScalerHdmi21MacTxLinktrainingResetToLTS1(ScalerHdmiMacTxPxMapping(_TX0));
        }
        else if(!ScalerHdmiTxGetBypassLinkTrainingEn() && (ScalerHdmi21MacTxLinktrainingPassCheck(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE))
        {
            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_TRAINING);

            FatalMessageHDMITx("[HDMI_TX] LinktrainingPassCheck False --> Link Training\n");

            DebugMessageWuzanne("[HDMI_TX] wuzanne test : HDMI Tx0 Judge to Link Training (LinktrainingPassCheck False)\n");

            //ScalerHdmi21MacTxLinktrainingReset(ScalerHdmiMacTxPxMapping(_TX0));
        }
    }
#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON) // [TEST]support fast timing change flow
    if(ScalerHdmiTxGetFastTimingSwitchEn() && (GET_HDMI_MAC_TX0_MODE_STATE() == _HDMI_TX_MODE_STATUS_INITIAL)){
            // disable HDCP before change timing
            ScalerHdmiMacTx0HdcpReset((UINT8 *)__FUNCTION__, __LINE__);

            // PHY setting & change state into _HDMI_TX_MODE_STATUS_LINK_OFF
            ScalerHdmiPhyTxSet(ScalerHdmiMacTxPxMapping(_TX0));
            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);
            SET_HDMI_MAC_TX0_STREAM_SOURCE(_HDMI_TX_SOURCE_NONE);
    }
#endif // #if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON) // [TEST]support fast timing change flow
#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)

    switch(GET_HDMI_MAC_TX0_MODE_STATE())
    {
        case _HDMI_TX_MODE_STATUS_INITIAL:

            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_UNPLUG);

            FatalMessageHDMITx("[HDMI_TX]Initial --> Unplug\n");

            DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Un-Plug State (From Initial)\n");

            break;

        case _HDMI_TX_MODE_STATUS_UNPLUG:

            if(GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
            {
#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_TRAINING);

                FatalMessageHDMITx("[HDMI_TX]Unplug --> Link Training\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link Training (From Un-Plug)\n");
#else
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                FatalMessageHDMITx("[HDMI_TX]Unplug --> Link Off\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link OFF State (From Un-Plug)\n");
#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
            }

            break;

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)

        case _HDMI_TX_MODE_STATUS_LINK_TRAINING:

            if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_UNPLUG_EVENT)
            {
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_UNPLUG);

                FatalMessageHDMITx("[HDMI_TX] Link Training --> Unplug\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Un-Plug State (From Link Training)\n");
            }

            if((GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _LINK_TRAING_PASS) || (GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _LINK_TRAING_LEGACY))
            {
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                FatalMessageHDMITx("[HDMI_TX] Link Training --> Link Off\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link OFF State (From Link Training)\n");
            }

            break;

#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
        case _HDMI_TX_MODE_STATUS_LINK_OFF:

            if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_UNPLUG_EVENT)
            {
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_UNPLUG);

                FatalMessageHDMITx("[HDMI_TX]Link Off --> Unplug\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Un-Plug State (From Link OFF)\n");
            }
            if((GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE) && (GET_OUTPUT_STREAM_TYPE(ScalerHdmiMacTxPxMapping(_TX0)) == _STREAM_TYPE_SST))
            {
#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)

                if(GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _LINK_TRAING_PASS)
                {
                    if(GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _TX_STREAM_READY)
                    {
                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_BANDWIDTH_DECISION);

                        FatalMessageHDMITx("[HDMI_TX]Link Off --> Bandwidth Decision\n");

                        DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Bandwidth Decision State (From Link OFF)\n");
                    }
                }
                else
#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
                {
                    ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_ON);

                    FatalMessageHDMITx("[HDMI_TX] Link Off --> Link On\n");

                    DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link ON State (From Link OFF)\n");
                }
            }
            break;

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)

        case _HDMI_TX_MODE_STATUS_BANDWIDTH_DECISION:

            if(GET_HDMI21_MAC_TX_BW_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) == _BW_PASS)
            {
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_HDMI21_LINK_ON);
                FatalMessageHDMITx("[HDMI_TX]Bandwidth Decision --> 2.1 Link On\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to 2.1 Link ON State (From Bandwidth Decision)\n");
            }
            else
            {
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                FatalMessageHDMITx("[HDMI_TX] Bandwidth Decision --> Link Off\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link OFF State (From Bandwidth Decision)\n");
            }

            break;

        case _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON:

            if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_UNPLUG_EVENT)
            {
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_UNPLUG);

                FatalMessageHDMITx("[HDMI21_TX] UNPLUG_EVENT, 2.1 Link On --> Unplug\n");

                DebugMessageWuzanne("[HDMI21_TX] 9. wuzanne test : HDMI Tx0 Judge to Un-Plug State (From 2.1 Link ON)\n");
            }
            else
            {

                if(GET_HDMI21_MAC_TX_STREAM_PROCESS_STATE(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) != _TX_STREAM_READY)
                {
                    DebugMessageHDMI21Tx("[HDMI21_TX]: Stream Fail\n");

                    bHdcpRestart = _TRUE;

#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON)
                    if(ScalerHdmiTxGetFrlNewModeEnable()  && ScalerHdmiTxGetFastTimingSwitchEn()){
                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                        FatalMessageHDMITx("[HDMI21_TX] 2.1 Link On --> Link Off\n");
                        FatalMessageHDMITx("[HDMI21_TX] wuzanne test 1: HDMI Tx0 Judge to Off State (From 2.1 Link ON)\n");
                    } else
#else
                    {
                        // when dp rx stream switch, need go back link training state
                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_TRAINING);
                        ScalerHdmi21MacTxLinktrainingResetToLTS1(ScalerHdmiMacTxPxMapping(_TX0));

                        FatalMessageHDMITx("[HDMI21_TX] 2.1 Link On --> Link training state\n");
                        DebugMessageHDMITx("[HDMI21_TX] wuzanne test : HDMI Tx0 Judge to Un-Plug State (From 2.1 Link ON)\n");
                    }
#endif
                }

                if(GET_HDMI_MAC_TX0_STREAM_SOURCE() != _HDMI_TX_SOURCE_NONE)
                {
                    #define FRL_FIFO_ERR_THRESH 3
                    UINT8 timeout=3; // wait input clock stable
                    //UINT8 bDispDInput = _TRUE;//(Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_HDMITX_DSC_SRC) == DSC_SRC_DISPD);

                    // check VESA CTS FIFO && FRL SRAM status
                    if(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_get_vesa_fifo_unstable(rtd_inl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg))|| HDMITX21_MAC_HDMI21_FRL_23_get_sram_ovf(rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg))){
                        if(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_get_vesa_fifo_unstable(rtd_inl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg))){
                            static UINT32 lastDumpStc = 0, dumpCnt=0;
                            UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
#if 1// [IC] #ifndef CONFIG_ENABLE_ZEBU_BRING_UP
                            if(((UINT32)(stc - lastDumpStc)) > 3*90000)
#endif
                            { // dump period=1sec
                                WarnMessageHDMI21Tx("[HDMI21_TX][%d] VESA CTS FIFO UnStable=%x!!\n",
                                    (UINT32)dumpCnt,
                                    (UINT32) rtd_inl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg));
                                lastDumpStc = stc;
                                dumpCnt = 0;
                            }
                            dumpCnt++;
                            rtd_maskl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg, ~HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_vesa_fifo_unstable_mask, HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_vesa_fifo_unstable_mask);
                            udelay(150);
                         }
                        if(rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg) & HDMITX21_MAC_HDMI21_FRL_ERR){
                            static UINT32 lastDumpStc = 0, dumpCnt=0;
                            UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
#if 1// [IC] #ifndef CONFIG_ENABLE_ZEBU_BRING_UP
                            if(((UINT32)(stc - lastDumpStc)) > 3*90000)
#endif
                            { // dump period // fake report error in Version A/B chip
                                DebugMessageHDMI21Tx("[HDMI21_TX][%d] FRL FIFO Err=%x!!\n",
                                        (UINT32)dumpCnt, (UINT32)rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg));
                                lastDumpStc = stc;
                                dumpCnt = 0;
                            }
                            dumpCnt++;
                            rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~HDMITX21_MAC_HDMI21_FRL_ERR, rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg));
                            udelay(150);
                        }
                    }
#if(_DSC_SUPPORT == _ON)
                    if(ScalerHdmiTxGetDSCEn() == FALSE)
#endif
			{ // DSC will not through DPC FIFO
                        while((HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_ovf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg)) || HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_udf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg))) && --timeout){
                            // reset dpc small fifo error status
                            NoteMessageHDMI21Tx("[HDMI21_TX] DPC FIFO Err timeout=%d,", (UINT32)timeout);
                            NoteMessageHDMI21Tx("dpc_sm_ovf=%d,", (UINT32)HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_ovf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg)));
                            NoteMessageHDMI21Tx("dpc_sm_udf=%d\n", (UINT32)HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_udf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg)));
                            rtd_outl(HDMITX21_MAC_HDMI21_DPC_1_reg, rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg));
                            udelay(150);
                        }
                    }

#if 0 // Ignore check DispD FIFO status when TX configure flow
                    if(bDispDInput && rtd_getbits(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, (_BIT1|_BIT0))){
                        static UINT32 lastDumpStc = 0, dumpCnt=0;
                        stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                    #if 1// [IC] #ifndef CONFIG_ENABLE_ZEBU_BRING_UP
                        if(((UINT32)(stc - lastDumpStc)) > 3*90000)
                    #endif
                        { // dump period // fake report error in Version A/B chip
                            ErrorMessageHDMI21Tx("[HDMI21_TX] DispD FIFO ERR[Ud=%d| Ov=%d]\n",
                                (UINT32)(rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg) & _BIT1) >> 1,
                                (UINT32)rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg) & _BIT0);
                            lastDumpStc = stc;
                            dumpCnt = 0;
                        }
                        dumpCnt++;
                        rtd_maskl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, ~(_BIT1|_BIT0), (_BIT1|_BIT0)); // clear dispD FIFO
                    }
#endif

                    // [WOSQRTK-14080] check periodic SRAM read/write request status
                    if(rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg) & _BIT6){
                        static UINT32 lastDumpStc = 0, errCnt=0;
                        UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                        // SRAM read/write request conflict
                        if(rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg) & HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_pr_rw_conflict_mask){
#if 1// [IC] #ifndef CONFIG_ENABLE_ZEBU_BRING_UP
                            if(((UINT32)(stc - lastDumpStc)) > 90000)
#endif
                            { // dump period=1sec
                                FatalMessageHDMITx("[HDMI21_TX][%d] pr_rw_conflict ERR(%x)\n", (UINT32)errCnt, (UINT32)rtd_inl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg));
                                lastDumpStc = stc;
                                errCnt = 0;
                            }
                            rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg, ~HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_pr_rw_conflict_mask, HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_pr_rw_conflict_mask);
                            errCnt++;
                        }else{
                            errCnt=0;
                        }
                    }

                    // Check VESA/FRL/Deep Color SRAM
                    if(((rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg) & HDMITX21_MAC_HDMI21_FRL_ERR) && (++frlFifoErrCnt > FRL_FIFO_ERR_THRESH))
                        ||((bHD21_480P_576P) && ((HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_get_txfifo21_ovflow_flag(rtd_inl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg))) || (HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_get_txfifo21_udflow_flag(rtd_inl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg)))))
                        || ((GET_HDMI21_MAC_TX_INPUT_REPEAT_NUM(_TX0) == FALSE) && (HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_ovf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg)) || HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_udf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg)))))
                    {
                        #if 0 // [FOR TEST] pending when DPC FIFO error twice in 1 sec
                        static UINT32 last_fifoErrStc=0;
                        stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                        #endif
                        FatalMessageHDMITx("[HDMI21_TX] VESA FIFO ERR=%d,", (UINT32)HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_get_vesa_fifo_unstable(rtd_inl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg)));
                        FatalMessageHDMITx("FRL FIFO ERR=%x(%d),", (UINT32)(rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg) & HDMITX21_MAC_HDMI21_FRL_ERR),(UINT32)frlFifoErrCnt);
                        FatalMessageHDMITx("DPC SM OVF ERR=%d,", (UINT32)HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_ovf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg)));
                        FatalMessageHDMITx("DPC SM UDF ERR=%d\n", (UINT32)HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_udf(rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg)));
                        FatalMessageHDMITx("txfifo21_ovflow=%d,", (UINT32)HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_get_txfifo21_ovflow_flag(rtd_inl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg)));
                        FatalMessageHDMITx("txfifo21_underflow=%d\n", (UINT32)HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_get_txfifo21_udflow_flag(rtd_inl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg)));
                        #if 0 // [FOR TEST] pending when DPC FIFO error twice in 1 sec
                        if((UINT32)(stc - last_fifoErrStc) < 90000)
                        {
                            //ErrorMessageHDMI21Tx("[HDMI21_TX] ABNORMAL FIFO ERR, Enter Pending!! diff=%d ms\n", (stc - last_fifoErrStc)/90);
                            while((rtd_inl(0xB8028304) & _BIT20) == 0){ // 0x00100000
                                ErrorMessageHDMI21Tx("[HDMI21_TX] Error Pending!, dp_vch_num=%d\n", HDMITX21_MAC_HDMI21_DPC_3_get_dp_vch_num(rtd_inl(HDMITX21_MAC_HDMI21_DPC_3_reg)));
                                msleep(3000);
                            }
                        }
                        //last_fifoErrStc = stc;
                        #endif
                        bHdcpRestart = _TRUE;
                        if(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_get_txfifo21_ovflow_flag(rtd_inl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg))) {
                            rtd_maskl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg, ~HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_txfifo21_ovflow_flag_mask, HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_txfifo21_ovflow_flag(0x1));
                        }
                        if(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_get_txfifo21_udflow_flag(rtd_inl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg))){
                            rtd_maskl(HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_reg, ~HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_txfifo21_udflow_flag_mask, HDMITX21_ON_REGION_HDMI21_BIT_CONV_1_txfifo21_udflow_flag(0x1));
                        }
                        // force re-start link training flow after detect TX FIFO error
                        if(rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg) & HDMITX21_MAC_HDMI21_FRL_ERR){
                            //ScalerHdmiTxSetForceLinkTrainingEn(_ENABLE);
                            FatalMessageHDMITx("[HDMITX] frlFifoErrCnt clr\n");
                            frlFifoErrCnt = 0;
                        }
#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _ON)
                        if(ScalerHdmiTxGetFrlNewModeEnable()  && ScalerHdmiTxGetFastTimingSwitchEn()){
                            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                            FatalMessageHDMITx("[HDMI21_TX] 2.1 Link On --> Link Off\n");
                            DebugMessageHDMITx("[HDMI21_TX] wuzanne test : HDMI Tx0 Judge to Link OFF State : 3 (From 2.1 Link ON)\n");
                        } else
#else
                        {
                            // when dp rx stream switch, need go back link training state
                            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_TRAINING);
                            ScalerHdmi21MacTxLinktrainingResetToLTS1(ScalerHdmiMacTxPxMapping(_TX0));

                            FatalMessageHDMITx("[HDMI21_TX] 2.1 Link On --> Link training state\n");
                            DebugMessageHDMITx("[HDMI21_TX] wuzanne test : HDMI Tx0 Judge to link tarining State (From 2.1 Link ON)\n");
                        }
#endif
                    }else if(rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg) & HDMITX21_MAC_HDMI21_FRL_ERR){
                        WarnMessageHDMI21Tx("[HDMI21_TX] FRL FIFO ERR=%x(%d)\n", (UINT32)(rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg) & HDMITX21_MAC_HDMI21_FRL_ERR), (UINT32)frlFifoErrCnt);
                    }else{
                        frlFifoErrCnt = 0;
                    }

                    // check EMP packet bypass FIFO status
                    ScalerHdmiMacTx0RepeaterCheckPktBypass_bufferStatus();

                    // Check DP Rx PHY (8b/10b Error)
                    if(GET_HDMI_MAC_TX0_AUDIO_BW_ERROR() == _TRUE)
                    {
                        FatalMessageHDMITx("[HDMI21_TX] Audio BW Fail\n");

                        bHdcpRestart = _TRUE;

                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                        FatalMessageHDMITx("[HDMI21_TX] 2.1 Link On --> Link Off\n");

                        DebugMessageWuzanne("[HDMI21_TX] wuzanne test : HDMI Tx0 Judge to Link OFF State : 4 (From 2.1 Link ON)\n");
                    }
                }
            }

            break;
#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)

        case _HDMI_TX_MODE_STATUS_LINK_ON:

            if(GET_HDMI_MAC_TX0_HPD_EVENT() == _HDMI_TX_HPD_UNPLUG_EVENT)
            {
                ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_UNPLUG);

                FatalMessageHDMITx("[HDMI_TX] Link On --> Unplug\n");

                DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Un-Plug State (From Link ON)\n");
            }
            else
            {

                if(GET_OUTPUT_STREAM_TYPE(ScalerHdmiMacTxPxMapping(_TX0)) == _STREAM_TYPE_NONE)
                {
                    FatalMessageHDMITx("[HDMI_TX] DP Rx Init\n");

                    bHdcpRestart = _TRUE;

                    ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                    FatalMessageHDMITx("[HDMI_TX] Link On --> Link Off\n");

                    DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link OFF State : 1 (From Link ON)\n");
                }

                if(GET_HDMI_MAC_TX0_STREAM_SOURCE() != _HDMI_TX_SOURCE_NONE)
                {
                    //UINT8 timeout=3; // wait input clock stable
                   // UINT8 bDispDInput = _TRUE;//(Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_HDMITX_DSC_SRC) == DSC_SRC_DISPD);

#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)

                    if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() > 3400)
                    {
                        if((GET_HDMI_MAC_TX0_SINK_SCRAMBLE_STATUS() == _HDMI_TX_SINK_SCRAMBLE_OFF) && (GET_HDMI_MAC_TX0_SCRAMBLE_RETRY() == _FALSE))
                        {
                            SET_HDMI_MAC_TX0_SCRAMBLE_RETRY();

                            FatalMessageHDMITx("[HDMI_TX] SCDC Scramble Fail\n");

                            bHdcpRestart = _TRUE;

                            ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                            FatalMessageHDMITx("[HDMI_TX] Link On --> Link Off\n");

                            DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link OFF State : 2 (From Link ON)\n");
                        }
                    }
#endif

#if 0 //Ignore check DispD FIFO status when TX configure flow
                    if(bDispDInput && rtd_getbits(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, (_BIT1|_BIT0))){
                        static UINT32 lastDumpStc = 0, dumpCnt=0;
                        stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
                    #ifndef CONFIG_MACH_RTD2801_ZEBU
                        if(((UINT32)(stc - lastDumpStc)) > 3*90000)
                    #endif
                        { // dump period // fake report error in Version A/B chip
                            ErrorMessageHDMI21Tx("[HDMI_TX] DispD FIFO ERR[Ud=%d| Ov=%d]\n", (UINT32)(rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg) & _BIT1) >> 1, (UINT32)rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg) & _BIT0);
                            lastDumpStc = stc;
                            dumpCnt = 0;
                        }
                        dumpCnt++;
                        rtd_maskl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, ~(_BIT1|_BIT0), (_BIT1|_BIT0));
 // clear dispD FIFO
                    }
#endif 
#if 0

                    hdmitx20_mac0_hdmi_control_2_reg_regValue = rtd_inl(HDMITX20_MAC0_HDMI_CONTROL_2_reg);
                    while(((hdmitx20_mac0_hdmi_control_2_reg_regValue & HDMITX20_MAC0_HDMI_CONTROL_2_sram_ovflow_flag_mask)
                                    || (hdmitx20_mac0_hdmi_control_2_reg_regValue & HDMITX20_MAC0_HDMI_CONTROL_2_sram_udflow_flag_mask))
                                && --timeout)
                    {
                        NoteMessageHDMI21Tx("[HDMI_TX] DPC FIFO ErrCnt=[%d]@LINK_ON,", (UINT32)timeout);
                        NoteMessageHDMI21Tx("OVF=%d,", (UINT32)HDMITX20_MAC0_HDMI_CONTROL_2_get_sram_ovflow_flag(hdmitx20_mac0_hdmi_control_2_reg_regValue));
                        NoteMessageHDMI21Tx("UDF=%d\n", (UINT32)HDMITX20_MAC0_HDMI_CONTROL_2_get_sram_udflow_flag(hdmitx20_mac0_hdmi_control_2_reg_regValue));
                        rtd_outl(HDMITX20_MAC0_HDMI_CONTROL_2_reg, hdmitx20_mac0_hdmi_control_2_reg_regValue);
                        udelay(150);
                        hdmitx20_mac0_hdmi_control_2_reg_regValue = rtd_inl(HDMITX20_MAC0_HDMI_CONTROL_2_reg);
                    }

                    // Check Deep Color SRAM
                    if(rtd_getbits(HDMITX20_MAC0_HDMI_CONTROL_2_reg, (_BIT7 | _BIT6)) != 0x00)
                    {
                        FatalMessageHDMITx("[HDMI_TX] DEEP COLOR SRAM FIFO Fail\n");

                        bHdcpRestart = _TRUE;

                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                        FatalMessageHDMITx("[HDMI_TX] Link On --> Link Off\n");

                        DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link OFF State : 3 (From Link ON)\n");
                    }


#ifndef CONFIG_MACH_RTD2801_ZEBU //  Don't need check PFIFO status in Zebu platform (just for TX PHY status check)
                    // [PTG] check pfifo_ovflow_flag
                    hdmitx20_on_region_pixel_conv_p1_reg_regValue = rtd_inl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg);
                    if((hdmitx20_on_region_pixel_conv_p1_reg_regValue & HDMITX20_ON_REGION_PIXEL_CONV_P1_pfifo_ovflow_flag_mask)
                        || (hdmitx20_on_region_pixel_conv_p1_reg_regValue & HDMITX20_ON_REGION_PIXEL_CONV_P1_pfifo_udflow_flag_mask))
                    {
                        NoteMessageHDMITx("[ERR][HDMI_TX] PFIFO Fail, OVF=%d,", (UINT32)HDMITX20_ON_REGION_PIXEL_CONV_P1_get_pfifo_ovflow_flag(hdmitx20_on_region_pixel_conv_p1_reg_regValue));
                        NoteMessageHDMITx("UDF=%d\n", (UINT32)HDMITX20_ON_REGION_PIXEL_CONV_P1_get_pfifo_udflow_flag(hdmitx20_on_region_pixel_conv_p1_reg_regValue));
                        bHdcpRestart = _TRUE;
                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);
                    }
#endif // #ifndef CONFIG_MACH_RTD2801_ZEBU 

                    // check CTS FIFO unstable
                    hdmitx20_mac1_hdmi_input_ctsfifo_reg_regValue = rtd_inl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg);
                    if(hdmitx20_mac1_hdmi_input_ctsfifo_reg_regValue & HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_fifo_unstable_mask)
                    {
                        NoteMessageHDMITx("[ERR][HDMI_TX] CTS FIFO UNSTABLE!!@%x\n", (UINT32)hdmitx20_mac1_hdmi_input_ctsfifo_reg_regValue);
                        bHdcpRestart = _TRUE;
                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);
                    }

                    // check EMP packet bypass FIFO status
                    ScalerHdmiMacTx0RepeaterCheckPktBypass_bufferStatus();

                    // Check DP Rx PHY (8b/10b Error)
                    if(GET_HDMI_MAC_TX0_AUDIO_BW_ERROR() == _TRUE)
                    {
                        FatalMessageHDMITx("[HDMI_TX] Audio BW Fail\n");

                        bHdcpRestart = _TRUE;

                        ScalerHdmiMacTx0StateChangeEvent(_HDMI_TX_MODE_STATUS_LINK_OFF);

                        FatalMessageHDMITx("[HDMI_TX] Link On --> Link Off\n");

                        DebugMessageWuzanne("[HDMI_TX] 9. wuzanne test : HDMI Tx0 Judge to Link OFF State : 4 (From Link ON)\n");
                    }
#endif
                }

            }

            break;

        default:
            break;
    }

    if(GET_HDMI_TX0_HPD_FIRST_PLUG())
        DebugMessageHDMITx("[HDMITX] Tx0JudgeHandler() Done!\n");

    return;
}
extern UINT8 scalerSetTxRun;
extern UINT8 g_edid_policy;
void ScalerHdmiMacTx0ModifyEdid(void)
{
    static UINT16 waitEdidReadyLoopCnt=0;

    waitEdidReadyLoopCnt++;

    if(((waitEdidReadyLoopCnt % 100) == 0) )
        DebugMessageHDMITx("[HDMITX]ModifyEdid() %d %d\n",(UINT32)GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS(),(UINT32)GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0)));

    if((GET_HDMI_MAC_TX0_DOWNSTREAM_STATUS() == _HIGH) && (GET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE))
    {
#ifdef HDMITX_SW_CONFIG    
        if(g_edid_policy != _EDID_RX_EDID_Policy_RX_from_flash){
#else
		if(1) {
#endif
            // Get HDMI Downstream EDID
            if(ScalerHdmiMacTx0GetDSEDID() == _TRUE)
            {
                #ifdef NOT_USED_NOW
                // Send EDID info to Host
                ScalerHdmiTxSendTxConnectStatus(_FALSE);
                #endif
            }
        }else {
            // ap read EDID_buffer data from flash
            SET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(EdidForI2cMacTxPxMapping(_TX0));
            ScalerHdmiMacTx0EdidAnalysis();
            SET_HDMI_MAC_TX0_MODIFY_EDID_ENABLE();
        }

        // show EDID AI support status
        //NoteMessageHDMITx("[HDMITX][EDID] Support AI=%d, SCDC=%d\n", ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT, _AI_SUPPORT), ScalerHdmiMacTxEdidGetFeature(_P0_OUTPUT_PORT, _SCDC_PRESENT));
    }

     if((GET_HDMI_MAC_TX0_MODIFY_EDID_ENABLE() == _ENABLE)|| (ScalerHdmiTxGetBypassLinkTrainingEn() && (sendApHdmiConnect==0))){
#ifdef HDMITX_SW_CONFIG  
            if((g_edid_policy  != _EDID_RX_EDID_Policy_RX_from_flash) && (g_edid_policy  != _EDID_RX_EDID_Policy_RX_mixed_default_and_TX_by_RTD2801))
#else
		if(1)
#endif
                ModifyEdidProcess();
            else
                CLR_HDMI_MAC_TX0_MODIFY_EDID_ENABLE();

            if(hdmitx_get_ap_state() == HDMITX_AP_INIT)
                hdmitx_set_ap_state(HDMITX_AP_CONNECT);
            stTxRepeaterOutputSCDCInfo.SCDC_0x35 = ScalerHdmiMacTx0SCDCRead((UINT8)_HDMI21_SCDC_OFFSET_35);
            FatalMessageHDMITx("[HDMITX][%d]AP_HDMITX_CONNECT\n", (UINT32)hdmitx_get_ap_state());
            if(hpd_low_high) {
                FatalMessageHDMITx("[HDMITX]MSG_AP_HDMITX_EDID_UPDATE\n");
                hpd_low_high = 0;
                hdmitx_set_ap_state(HDMITX_AP_CONNECT);
                //core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HDMITX_EDID_UPDATE, 0); //fix by zhaodong
            } else{
                //core_fw_msg_queue_addEvent(MSG_AP_ID, MSG_AP_HDMITX_CONNECT, 1);//return connect to ap //fix by zhaodong
                HdmiIsDisConnectAtAp = 0;
            }
            sendApHdmiConnect = 1;

    }

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    // update new EDID to source device
    if(GET_OUTPUT_DFP_DEVICE_CAP_CHANGE(ScalerHdmiMacTxPxMapping(_TX0))){
        // trigger update source EDID
        //ScalerHdmiMacTx0RepeaterEdidSendToRx(_ENABLE);

        ScalerHdmiMacTx0EdidChangeReset();
    }
#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT

}

//--------------------------------------------------
// Description  : HDMI Tx Mode Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0ModeHandler(void)
{


    if(GET_HDMI_TX0_HPD_FIRST_PLUG())
        InfoMessageHDMITx("[HDMITX] Tx0ModeHandler(), State/Src=%d/%d\n", (UINT32)GET_HDMI_MAC_TX0_MODE_STATE(), (UINT32)GET_HDMI_MAC_TX0_STREAM_SOURCE());



    /****************************************/
    /****** HDMI Tx Mode State Machine ******/
    /****************************************/
    switch (GET_HDMI_MAC_TX0_MODE_STATE())
    {
        case _HDMI_TX_MODE_STATUS_INITIAL:

            break;

        case _HDMI_TX_MODE_STATUS_UNPLUG:
            if(GET_HDMI_MAC_TX0_MODE_STATE_CHANGE() == _TRUE)
            {
#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
                CLR_HDMI_MAC_TX0_SCRAMBLE_RETRY();
#endif
                ScalerHdmiMacTx0Reset();

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
                ScalerHdmi21MacTxLinktrainingResetToLTS1((EnumOutputPort)_TX0);
#endif
            }
            break;

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
        case _HDMI_TX_MODE_STATUS_LINK_TRAINING:
//#if(_HDMI21_STREAM_OFF_WITHOUT_LINKTRAINING == _OFF)
#ifdef NOT_USED_NOW
            if(ScalerHdmiTxGetChipVersion() < VERSION_C){
                if(GET_HDMI_MAC_TX0_MODE_STATE_CHANGE() == _TRUE)
                {
                    ScalerHdmiMacTx0Reset();
                    ScalerHdmiMacTx0TopPowerReset();
                    ScalerHdmiMacTx0RegRecovery();
                }
            }
#endif
//#endif
            ScalerHdmi21MacTxGetStreamTimingInfo(ScalerHdmiMacTxPxMapping(_TX0));
            ScalerHdmi21MacTxLinktraining(ScalerHdmiMacTxPxMapping(_TX0));
            break;
#endif
        case _HDMI_TX_MODE_STATUS_LINK_OFF:

            if(GET_HDMI_MAC_TX0_MODE_STATE_CHANGE() == _TRUE)
            {
                ScalerHdmiMacTx0Reset();
            }
            runontime = 0;
            gcpswtohw = _HDMI_TX_GCP_TOHW_INIT;
            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_GCPTOHW_TIMER);
            break;

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
        case _HDMI_TX_MODE_STATUS_BANDWIDTH_DECISION:

            ScalerHdmi21MacTxGetStreamTimingInfo(ScalerHdmiMacTxPxMapping(_TX0));
            SET_HDMI21_MAC_TX_BW_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0)), ScalerHdmi21MacTxBwDecision(ScalerHdmiMacTxPxMapping(_TX0)));

            break;

        case _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON:
            if(GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)
            {
                // [H5X_HDMITX_BYPASS_HPD_EDID]
                if(ScalerHdmiTxGetBypassLinkTrainingEn()){
                    // reset TX Max before HDMITX HW setting
                    ScalerHdmiMacTx0Reset();

                    // [H5X] FORCE update timing info
                    ScalerHdmi21MacTxGetStreamTimingInfo(ScalerHdmiMacTxPxMapping(_TX0));
                    SET_HDMI21_MAC_TX_BW_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0)), ScalerHdmi21MacTxBwDecision(ScalerHdmiMacTxPxMapping(_TX0)));

  #ifdef FIX_ME_HDMITX_BRING_UP
            #if 1 // enable hdmi_21_tx_en in v-porch
                    // set hdmi_21_tx_en=1 in v-porch
                    HDMITX_DTG_Wait_Den_Stop_Done();
                     rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~_BIT7, _BIT7); // hdmi_21_tx_en=1
            #else // enable hdmi_21_tx_en before enable frl_en
                    // set hdmi_21_tx_en=1 before enable frl_en
                    rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~_BIT7, 0);
            #endif
  #else
                    // Enable HDMI2.1 Module
                    rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~_BIT7, _BIT7);
  #endif

                    // update HDMITX PHY HW setting
                    ScalerHdmiPhyTxSet(ScalerHdmiMacTxPxMapping(_TX0));

                    // [H5X] HDMITX MAC 2.0 setting
                    ScalerHdmiTx0_MacConfig(TC_HDMI21);

                    // [H5X] LANE PN Swap
                    ScalerHdmiTx0_LanePNSwap();

                    // [H5X] LANE src sel
                    ScalerHdmiTx0_LaneSrcSel();

                    ScalerHdmiPhyTxPowerOn(ScalerHdmi21MacTxPxMapping(_TX0));

                    SET_HDMI21_MAC_TX_FRL_SETPHY(_TX0);

                    // Enable FRL after TX MAC setting done
                    // LT FRL setting
                    ScalerTxHdmiTx0LtspGap();
                }

                // HDMI Tx Stream Handler(PHY and MAC)
                if(ScalerHdmi21MacTxStreamHandler(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE)
                {
  #if 0 // TEST
                    static UINT8  bDumpReg=0;
                    //extern void hdmi21_DumpReg_DSCE_VIC97_4kp60_8bit_yuv444_dispd_6G4L(void);
                    extern void hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L(void);
                    ROSTimeDly(100);
                    if(!bDumpReg){
                        NoteMessageHDMITx("\n\n\n[TEST] DUMP REG!!\n");
                        //hdmi21_DumpReg_DSCE_VIC97_4kp60_8bit_yuv444_dispd_6G4L();
                        hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L();
                        NoteMessageHDMITx("[TEST] END\n\n\n\n");
                    #if 0 // TEST
                        NoteMessageHDMITx("[TEST] PENDING@Fail Case...\n");
                        while((rtd_inl(0xb8005040) & _BIT16) == 0){
                            ROSTimeDly(1000);
                        }
                    #endif
                    }
                    bDumpReg = 1;
  #endif
#ifdef _DEBUG_HALT_TX_WHEN_FIFO_ERROR // HALT TX when TX FIFO error
extern UINT8 TxHaltFlag;
                    if(TxHaltFlag){
                        InfoMessageHDMITx("[HDMI21_TX] SKIP MAC RESET!!\n");; //
                    }else
#endif
                    ScalerHdmiMacTx0Reset();
                }

        #if 0 // [TEST] dump register setting
                if(1){
                    static UINT8  bDumpReg=0;
                    extern void hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L(void);
                    //extern void hdmi21_DumpReg_DSCE_VIC97_4kp60_8bit_yuv444_dispd_6G4L(void);
                    ROSTimeDly(100);
                    if(!bDumpReg){
                        NoteMessageHDMITx("\n\n\n[TEST] DUMP REG!!\n");
                        //hdmi21_DumpReg_DSCE_VIC97_4kp60_8bit_yuv444_dispd_6G4L();
                        hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L();
                        NoteMessageHDMITx("[TEST] END\n\n\n\n");
                    #if 0 // TEST
                        NoteMessageHDMITx("[TEST] PENDING@Finish Case...\n");
                        while((rtd_inl(0xb8005040) & _BIT16) == 0){
                            ROSTimeDly(1000);
                        }
                    #endif
                    }
                    bDumpReg = 1;
                }
        #elif 0 // [TEST] dump register setting
                static UINT8  bDumpReg=0;
                extern void hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L(void);
                //extern void hdmi21_DumpReg_LTP_Pattern_5678(void);
                ROSTimeDly(100);
                if(!bDumpReg){
                    NoteMessageHDMITx("\n\n\n[TEST] DUMP REG!!\n");
                    hdmi21_DumpReg_VIC196_8kp30_12bit_12G4L();
                    //hdmi21_DumpReg_LTP_Pattern_5678();
                    NoteMessageHDMITx("[TEST] END\n\n\n\n");
                }
                bDumpReg = 1;
        #endif
                
            }
            else
            {
                if(waitOutTgStable() && !u8hdcprun) {
                    u8hdcprun =1;
#if IS_ENABLED(CONFIG_HDCPTX)
                    SET_HDMI_HDCPTX_RUN(0, 1);
#endif
                    #if(_HDCP2_SUPPORT == _ON)
                    HdmiTxHdcpState = _HDMI_TX_HDCP_AUTH_AND_CLR_AUTH_STATE;
                    #endif
                }

                ScalerHdmi21MacTxPktFineTune(ScalerHdmiMacTxPxMapping(_TX0));

                //mute enable control
                if(ScalerHdmiTxGetMute() || (GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE() == FALSE)){
                    if(waitOutTgStable()) {
                        ScalerHdmiTxMuteEnable(ScalerHdmiTxGetMute());
                        if(!runontime){
                             runontime = 1;
                            ScalerTimerReactiveTimerEvent(_GCP_TO_HW_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_GCPTOHW_TIMER);
                            }
                           // ScalerHdmiMacTx0SetGcpToHwMode();
                    }
                }
                ScalerHdmiMacTx0SetGcpToHwMode();
            }
        break;

#endif // #if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)

        case _HDMI_TX_MODE_STATUS_LINK_ON:

            if(GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)
            {
                // reset TX Max before HDMITX HW setting
                ScalerHdmiMacTx0Reset();

                ScalerHdmiMacTx0GetStreamTimingInfo(ScalerHdmiMacTxPxMapping(_TX0));

                // HDMI Tx Stream Handler(PHY and MAC)
                if(ScalerHdmiMacTx0StreamHandler() == _TRUE)
                {
                    ScalerHdmiPhyTxEnable3rdTracking(ScalerHdmiMacTxPxMapping(_TX0));
                }
                else
                {
                    DebugMessageHDMITx("[HDMITX] SIGNAL UNSTABLE ON LINK_ON!!\n");
                    ScalerHdmiMacTx0Reset();
                }
                
            }
            else
            {
                if(waitOutTgStable() &&!u8hdcprun) {
                    u8hdcprun =1;
#if IS_ENABLED(CONFIG_HDCPTX)
                    SET_HDMI_HDCPTX_RUN(0, 1);
#endif
                    #if(_HDCP2_SUPPORT == _ON)
                    HdmiTxHdcpState = _HDMI_TX_HDCP_AUTH_AND_CLR_AUTH_STATE;
                    #endif
                }

                if ((rtd_getbits(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, _BIT7) == 0)&&(waitOutTgStable())) {
                    // Enable HDMI Tx Small FIFO
                    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, ~_BIT7, _BIT7);
                    FatalMessageHDMITx("[HDMITX] 2.0 stream en\n");
                }

                ScalerHdmiPhyTxThirdSetPICode(ScalerHdmiMacTxPxMapping(_TX0));

                ScalerHdmiMacTx0PktFineTune();

#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
                if(waitOutTgStable())
                    ScalerHdmiMacTx0SCDCProcHandler();
#endif

                //mute enable control
                if(ScalerHdmiTxGetMute() || (GET_HDMI_MAC_TX0_HPD_STATUS_CHANGE() == FALSE)){
                     if(waitOutTgStable()) {
                        ScalerHdmiTxMuteEnable(ScalerHdmiTxGetMute());
                        if(!runontime){
                             runontime = 1;
                            ScalerTimerReactiveTimerEvent(_GCP_TO_HW_TIME, _SCALER_TIMER_EVENT_HDMI_TX0_GCPTOHW_TIMER);
                            }
                            //ScalerHdmiMacTx0SetGcpToHwMode();
                     }
                }
                ScalerHdmiMacTx0SetGcpToHwMode();
                _SCDC_check_RSED_CED_Status();
            }
            break;

        default:

            break;
    }

    if(GET_HDMI_TX0_HPD_FIRST_PLUG())
        DebugMessageHDMITx("[HDMITX] ScalerHdmiMacTx0ModeHandler() Done!\n");

    return;
}


//--------------------------------------------------
// Description  : HDMI Tx Mode Switch
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0StateChangeEvent(EnumHDMITxModeStatus enumModeState)
{
    InfoMessageHDMITx("[HDMITX] State Change=%d", (UINT32)GET_HDMI_MAC_TX0_MODE_STATE());
    InfoMessageHDMITx("->%d\n", (UINT32)enumModeState);
    SET_HDMI_MAC_TX0_MODE_STATE(enumModeState);
    SET_HDMI_MAC_TX0_MODE_STATE_CHANGE();
}
UINT8 hdmitx_check_underMinBlankArea(void)
{
    static UINT16 tmpWidth,tmpHeight,tmpVfreq;// = 0x00;
    UINT16 usHactive,usHtotal;// = 0x00;


    tmpWidth = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_ACT_WID);
    tmpHeight = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_V_ACT_LEN);
    tmpVfreq = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_VFREQ);
    DebugMessageHDMITx("[HDMITX] check_blank timing=%d %d %d", (UINT32)tmpWidth, (UINT32)tmpHeight, (UINT32)tmpVfreq);

    usHtotal = Scaler_HDMITX_DispGetInputInfo(SCALER_INFO_TX_OUTPUT_H_TOTAL);
    usHtotal = usHtotal * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8;
    usHactive = tmpWidth * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8;
#ifdef SCALER_SUPPORT_420_CONFIG
    if((ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420) && (GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420))
#else
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
#endif
    {
        usHtotal = (usHtotal / 2);
        usHactive = (usHactive / 2);
    }
    if(((usHtotal - usHactive) * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1)) < _HDMITX_AUDIO_MIN_BLANK_AREA){
        DebugMessageHDMITx("[HDMITX] need reset rekey window size\n");
        return 1;
    }
    return 0;
}

//--------------------------------------------------
// Description  : HDMI Tx Stream Handler
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0StreamHandler(void)
{
    // Convert to YCbCr420 or reduce color depth if TMDS Clk is not enough
    ScalerHdmiMacTx0ReduceTMDSClk();

    // Convert to RGB444 if Sink not Support YCbCr444/YCbCr422 For Clone mode
    ScalerHdmiMacTx0ColorSpaceConvert();

    // Convert to 24bit if Sink not Support 36bit/30bit For Clone mode
    ScalerHdmiMacTx0ColorDepthDecision();

    // Pixel repeat if Deep color Clk < 25MHz or Audio BW not enough
    ScalerHdmiMacTx0RepetitionDecision();

    // Calcaute Deep Color Clk
    ScalerHdmiMacTx0DeepColorClkCal();
    // for 2k240 Audio BW error
    if(hdmitx_check_underMinBlankArea()){
        rtd_outl(HDMITX20_MAC0_SCHEDULER_7_reg, 0x0);
        SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(0x0 + 0x01);
    }
    else{
        if (g_scaler_display_info.HDCP == HDCP14 ||g_scaler_display_info.HDCP ==HDCP22 ) {
            rtd_outl(HDMITX20_MAC0_SCHEDULER_7_reg, 0x39);
            SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(0x39 + 0x01);
        } else {
            rtd_outl(HDMITX20_MAC0_SCHEDULER_7_reg, 0x04);
            SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(0x04 + 0x01);
        }
    }
    // Power On CMU and PHY Parameter Set
    if(ScalerHdmiPhyTxSet(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
    {
        // Power On MAC
        if(ScalerHdmiMacTx0OutputConfiguration() == _FALSE)
        {
            return _FALSE;
        }

        if(ScalerHdmiMacTx0MonitorFIFOStatus() == _STABLE)
        {
            // Power On HDMI Tx Driver
#if(_HDMI_MAC_TX0_PURE == _ON)
            ScalerHdmiPhyTxPowerOn(ScalerHdmiMacTxPxMapping(_TX0));
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
            ScalerHdmiPhyTxLanesSet(ScalerHdmiMacTxPxMapping(_TX0), _ENABLE);
#endif
            // Enable HDMI Tx Small FIFO set move to wait dtg done
            //rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, ~_BIT7, _BIT7);

#ifdef NOT_USED_NOW
            drvif_scaler_set_display_mode(DISPLAY_MODE_FRC, scaler_dtg_get_app_type());//Scaler_TxsocSetDispSync(_FALSE); // TXSOC DTG free run with VODMA
#endif

#if(_HDMI_MAC_TX0_DP_PLUS == _ON)
            // Small FIFO Lane3 Gen TMDS Clk(Data 1111100000)
            rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT2, _BIT2);
#endif

#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
            ScalerHdmiMacTx0SCDCSetTMDSConfiguration();
            if(waitOutTgStable())
                ScalerHdmiMacTx0SCDCProcHandler();
#endif

            if(GET_HDMI_MAC_TX0_SUPPORT() == _TRUE)
            {
                SET_HDMI_MAC_TX0_STREAM_SOURCE(_HDMI_TX_SOURCE_HDMI);
            }
            else
            {
                SET_HDMI_MAC_TX0_STREAM_SOURCE(_HDMI_TX_SOURCE_DVI);
            }

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
            ScalerTimerActiveTimerEvent(ScalerHdmiMacTx0CtsCodeCheckEventPeriod(), _SCALER_TIMER_EVENT_HDMI_TX0_AUDIO_CTS_CODE_CHECK);
#endif

            FatalMessageHDMITx("[HDMI_TX] Stream Set Finish\n");

//#ifdef H5X_HDMITX_FIXME // [FIX-ME] VO clock is fast than TX(600 vs 150MHz), change vo freerun period to sync with TX
#ifdef NOT_USED_NOW
            drvif_scaler_set_display_mode(DISPLAY_MODE_FRC, scaler_dtg_get_app_type());//Scaler_TxsocSetDispSync(_FALSE); // TXSOC DTG free run with VODMA
#endif
            if(GET_HDMI_TX0_HPD_FIRST_PLUG())
                CLR_HDMI_TX0_HPD_FIRST_PLUG();

            // frame measure enable
            rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask);

    #if 0 // [FIX-ME][WORKAROUND] -- toggle hdmi_en after TX setting done
            // toggle pll_reset
            rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL10_reg, ~HDMITX_PHY_HDMITXPHY_CTRL10_reg_pll_rstb_mask, HDMITX_PHY_HDMITXPHY_CTRL10_reg_pll_rstb(0x0));
            msleep(10);
            rtd_maskl(HDMITX_PHY_HDMITXPHY_CTRL10_reg, ~HDMITX_PHY_HDMITXPHY_CTRL10_reg_pll_rstb_mask, HDMITX_PHY_HDMITXPHY_CTRL10_reg_pll_rstb(0x1));
            msleep(10);
            NoteMessageHDMITx("[HDMITX] PLL RESET3!!\n");

            Wait_TXSOC_Den_Stop_Done();
            Wait_TXSOC_Den_Start();
            Wait_TXSOC_Den_Stop_Done();

            // toggle hdmi_en
            DebugMessageHDMITx("[HDMITX] Toggle hdmi_en!\n");
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~_BIT7, 0);
            udelay(150);
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~_BIT7, _BIT7);
    #endif

//#endif
            return _TRUE;
        }
    }
    return _FALSE;
}

//--------------------------------------------------
// Description  : Hdmi Tx Reduce TMDS Clk
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0ReduceTMDSClk(void)
{
    DebugMessageHDMITx("[HDMI_TX] Color Depth=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());

#if(_HDMI20_MAC_TX0_SUPPORT == _ON)
    if(ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420)
    {
        // For 4K2K@60Hz bpc>8
        // Reduce TMDS Clk based on Format Convert
        if((GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5200) && (ScalerHdmiMacTx0EdidGetFeature(_HDMI_MAX_TMDS_CLK) <= 68))
        {
            if(ScalerHdmiMacTx0EdidGetFeature(_YCC420_SUPPORT) == _TRUE)
            {
                DebugMessageHDMITx("[HDMI_TX] 1.ReduceTMDSClk@%d to YUV420\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK());

                ScalerHdmiMacTx0ConvertTo420();
            }
        }
#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
        else if(((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() != _DP_STREAM_COLOR_DEPTH_8_BITS) && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5200) && (GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR422)) ||
                (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 6000))
        {
            if(((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS) && (ScalerHdmiMacTx0EdidGetFeature(_YCC420_SUPPORT) == _TRUE)) ||
               ((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_10_BITS) && (ScalerHdmiMacTx0EdidGetFeature(_DC30_420_SUPPORT) == _TRUE)) ||
               ((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_12_BITS) && (ScalerHdmiMacTx0EdidGetFeature(_DC36_420_SUPPORT) == _TRUE)))
            {
                DebugMessageHDMITx("[HDMI_TX] 2.ReduceTMDSClk@%d to YUV420\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK());

                ScalerHdmiMacTx0ConvertTo420();
            }
        }
#else
        else if(GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5200)
        {
            if(ScalerHdmiMacTx0EdidGetFeature(_YCC420_SUPPORT) == _TRUE)
            {
                DebugMessageHDMITx("[HDMI_TX] 2.ReduceTMDSClk@%d to YUV420\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK());

                ScalerHdmiMacTx0ConvertTo420();
            }
        }
#endif
    }
#endif

    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR422)
    {
        if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR420)
        {
#if(_HDMI_MAC_6G_TX0_SUPPORT == _ON)
            // 6000*8/12 = 4000 or 3400*8/10 = 4800
            if(((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() >= _DP_STREAM_COLOR_DEPTH_12_BITS) && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 4000)) ||
               ((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() >= _DP_STREAM_COLOR_DEPTH_10_BITS) && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 4800)))
#else
            // 3400*8/12 = 2266 or 3400*8/10 = 2720
            if(((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() >= _DP_STREAM_COLOR_DEPTH_12_BITS) && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 2266)) ||
               ((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() >= _DP_STREAM_COLOR_DEPTH_10_BITS) && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 2720)))
#endif
            {
                if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() != _COLOR_SPACE_RGB)
                {
                    ScalerColorStreamYuv2Rgb(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);
                }

                DebugMessageHDMITx("[HDMI_TX] Color Space not 420, Reduce Color Depth %d->8\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());

                SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_RGB);

                SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
            }
        }
#if(_HDMI_MAC_6G_TX0_SUPPORT == _OFF)
        else
        {
            if(((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() >= _DP_STREAM_COLOR_DEPTH_12_BITS) && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 4532)) ||
               ((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() >= _DP_STREAM_COLOR_DEPTH_10_BITS) && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5440)))
            {
                DebugMessageHDMITx("[HDMI_TX] Color Space 420, Reduce Color Depth %d->8\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());
                SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
            }
        }
#endif
    }
}
//--------------------------------------------------
// Description  : Hdmi Tx Color Space Convert
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0ColorSpaceConvert(void)
{
#ifndef FIX_ME_HDMITX_BRING_UP // don't check RX EDID in IC bring up stage
    if((GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR444)
        && ((ScalerHdmiMacTx0EdidGetFeature(_YCC444_SUPPORT) == _FALSE) && (ScalerHdmiMacTx0EdidGetFeature(_DC_Y444_SUPPORT) == FALSE)))
    {
        FatalMessageHDMITx("[HDMI_TX] YUV444@%d to RGB444\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE());

        // Enable YUV 444 to RGB 444 Function
        ScalerColorStreamYuv2Rgb(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

        ScalerColorStreamYuv2RgbClamp(ScalerHdmiMacTxPxMapping(_TX0), _ENABLE);

        SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_RGB);
    }
#endif // #ifndef FIX_ME_HDMITX_BRING_UP

    if(ScalerColorStream422to444Support(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
    {
        if((GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR422)
            && ((ScalerHdmiMacTx0EdidGetFeature(_YCC444_SUPPORT) == _FALSE) && (ScalerHdmiMacTx0EdidGetFeature(_DC_Y444_SUPPORT) == FALSE)))
        {
            DebugMessageHDMITx("[HDMI_TX] YUV422@%d to RGB444\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE());

            // Enable YUV 422 to YUV 444 Function
            ScalerColorStream422To444(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

            // Enable YUV 444 to RGB 444 Function
            ScalerColorStreamYuv2Rgb(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

            // Must Set
            ScalerColorStreamYuv2RgbClamp(ScalerHdmiMacTxPxMapping(_TX0), _ENABLE);

            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_RGB);
        }
    }
}

#if(_HDMI20_MAC_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Color Space Convert to 420
// Input Value  :
// Output Value :
//--------------------------------------------------

void ScalerHdmiMacTx0ConvertTo420(void)
{
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR422)
    {
        if(ScalerColorStream422to420Support(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
        {
            ScalerColorStream422To420(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_YCBCR420);
        }
        else if((ScalerColorStream444to420Support(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE) && (ScalerColorStream422to444Support(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE))
        {
            ScalerColorStream422To444(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

            ScalerColorStream444To420(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_YCBCR420);
        }
        else
        {
            DebugMessageHDMITx("[HDMI_TX] HW can't support 420 convert\n");

            return;
        }
    }
    else if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_RGB)
    {
        if(ScalerColorStream444to420Support(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
        {
            if(GET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE() == _RGB_QUANTIZATION_LIMIT_RANGE)
            {
                ScalerColorStreamRgb2Yuv(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON, _RGB2YUV_ITU709_LIMIT);
            }
            else
            {
                ScalerColorStreamRgb2Yuv(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON, _RGB2YUV_ITU709_FULL);
            }

            ScalerColorStream444To420(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_YCBCR420);
        }
        else
        {
            DebugMessageHDMITx("[HDMI_TX] HW can't support 420 convert\n");

            return;
        }
    }
    else if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR444)
    {
        if(ScalerColorStream444to420Support(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
        {
            ScalerColorStream444To420(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_ON);

            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_YCBCR420);
        }
        else
        {
            DebugMessageHDMITx("[HDMI_TX] HW can't support 420 convert\n");

            return;
        }
    }
}
#endif // #if(_HDMI20_MAC_TX0_SUPPORT == _ON)


//--------------------------------------------------
// Description  : HDMI Tx Get timing info from ST
// Input Value  : OutputPort
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0GetStreamTimingInfo(UINT8 ucOutputPort)
{
    UINT16 ret;

    DebugMessageHDMITx("[HDMI_TX] Get Timing Info[%d]\n", (UINT32)ucOutputPort);

    // Get Pixel Clk
    SET_HDMI_MAC_TX0_INPUT_PIXEL_CLK(ScalerDpStreamGetElement(ucOutputPort, _PIXEL_CLOCK));

    DebugMessageHDMITx("[HDMI_TX] input TMDS clk=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK());

    // Get Frame Rate
    SET_HDMI_MAC_TX0_INPUT_FRAME_RATE(ScalerDpStreamGetElement(ucOutputPort, _FRAME_RATE));

    // GET Interlace Mode
    if(ScalerDpStreamGetElement(ucOutputPort, _INTERLACE_INFO) == _TRUE)
    {
        SET_HDMI_MAC_TX0_INPUT_INTERLACE();
        SET_HDMI_MAC_TX0_INPUT_FRAME_RATE(GET_HDMI_MAC_TX0_INPUT_FRAME_RATE() / 2);
    }
    else
    {
        CLR_HDMI_MAC_TX0_INPUT_INTERLACE();
    }

    DebugMessageHDMITx("[HDMI_TX] Interlaced=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_INTERLACE());


    // Get Tx Input Timing
    ScalerHdmiMacTx0MeasureInputInfo(ucOutputPort);

    // Set Color Space
    switch(ret = ScalerDpStreamGetElement(ucOutputPort, _COLOR_SPACE))
    {
        case _COLOR_SPACE_RGB:
            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_RGB);
            break;

        case _COLOR_SPACE_YCBCR444:
            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_YCBCR444);
            break;

        case _COLOR_SPACE_YCBCR422:
            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_YCBCR422);
            break;

        case _COLOR_SPACE_YCBCR420:
            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_YCBCR420);
            break;

        default:
            SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(_COLOR_SPACE_RGB);
            break;
    }
    DebugMessageHDMITx("[HDMI_TX] Timing Info.color=[%d]\n", (UINT32)ret);

    // Set Color Depth
    switch(ret = ScalerDpStreamGetElement(ucOutputPort, _COLOR_DEPTH))
    {
        case _DP_STREAM_COLOR_DEPTH_6_BITS:
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
            break;

        case _DP_STREAM_COLOR_DEPTH_8_BITS:
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
            break;

        case _DP_STREAM_COLOR_DEPTH_10_BITS:
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_10_BITS);
            break;

        case _DP_STREAM_COLOR_DEPTH_12_BITS:
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_12_BITS);
            break;

        case _DP_STREAM_COLOR_DEPTH_16_BITS:
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_16_BITS);
            break;

        default:
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
            break;
    }
    DebugMessageHDMITx("[HDMI_TX] Timing Info.depth=[%d]\n", (UINT32)ret);

    // Get Colorimetry
    SET_HDMI_MAC_TX0_INPUT_COLORIMETRY(ScalerDpStreamGetElement(ucOutputPort, _COLORIMETRY));

    // Get EXT_Colorimetry
    SET_HDMI_MAC_TX0_INPUT_EXT_COLORIMETRY(ScalerDpStreamGetElement(ucOutputPort, _EXT_COLORIMETRY));

    // Get RGB QUANTIZATION RANGE
    SET_HDMI_MAC_TX0_INPUT_COLOR_RGB_QUANTIZATION_RANGE(ScalerDpStreamGetElement(ucOutputPort, _COLOR_RGB_QUANTIZATION_RANGE));

    // Get YCC QUANTIZATION RANGE
    SET_HDMI_MAC_TX0_INPUT_COLOR_YCC_QUANTIZATION_RANGE(ScalerDpStreamGetElement(ucOutputPort, _COLOR_YCC_QUANTIZATION_RANGE));

    return;
}

//--------------------------------------------------
// Description  : Measure function for HDMI TX
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0MeasureInputInfo(UINT8 ucOutputPort)
{
    g_stHdmiMacTx0InputTimingInfo = ScalerDpStreamGetDisplayTimingInfo(ucOutputPort);

    g_stHdmiMacTx0InputTimingInfo->b1Interlace = GET_HDMI_MAC_TX0_INPUT_INTERLACE();

    g_stHdmiMacTx0InputTimingInfo->usVFreq = (UINT16)(GET_HDMI_MAC_TX0_INPUT_FRAME_RATE()*10);

    DebugMessageHDMITx("[HDMI_TX] Input Timing H Polarity=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->b1HSP);
    DebugMessageHDMITx("[HDMI_TX] Input Timing V Polarity=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->b1VSP);
    DebugMessageHDMITx("[HDMI_TX] Input Timing HFreq=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHFreq);
    DebugMessageHDMITx("[HDMI_TX] Input Timing HStart=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHStart);
    DebugMessageHDMITx("[HDMI_TX] Input Timing HSW=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHSWidth);
    DebugMessageHDMITx("[HDMI_TX] Input Timing HWidth=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHWidth);
    DebugMessageHDMITx("[HDMI_TX] Input Timing HTotal=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHTotal);
    DebugMessageHDMITx("[HDMI_TX] Input Timing VFreq=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usVFreq);
    DebugMessageHDMITx("[HDMI_TX] Input Timing Interlaced=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->b1Interlace);
    DebugMessageHDMITx("[HDMI_TX] Input Timing VStart=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usVStart);
    DebugMessageHDMITx("[HDMI_TX] Input Timing VSW=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usVSWidth);
    DebugMessageHDMITx("[HDMI_TX] Input Timing VHeight=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usVHeight);
    DebugMessageHDMITx("[HDMI_TX] Input Timing VTotal=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usVTotal);
}

//--------------------------------------------------
// Description  : Get HPD Status
// Input Value  : None
// Output Value : _TRUE / _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0GetHpdStatus(void)
{
#if(_HDMI_MAC_TX0_PURE == _ON)
    return (rtd_getbits(HDMITX20_HPD_DETECTION_HDMI_HPD_CTRL0_reg, _BIT2) >> 2);
#elif(_HDMI_MAC_TX0_DP_PLUS == _ON)
    return (rtd_getbits(PBB_70_HPD_CTRL, _BIT2) >> 2);
#else
    return _FALSE;
#endif
}
#ifdef NOT_USED_NOW
//--------------------------------------------------
// Description  : Get HPD Status
// Input Value  : None
// Output Value : _TRUE / _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0CableDetect(void)
{
    if(ScalerHdmiMacTx0GetHpdStatus() == _TRUE)
    {
        return _TRUE;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : HDMI Tx POWER Reset
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0TopPowerReset(void)
{
    ScalerPowerGroupHDMITxPowerCutReset(_TX0, _ON);
    ScalerPowerGroupHDMITxPowerCutReset(_TX0, _OFF);
}


#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
//--------------------------------------------------
// Description  : HDMI Tx OFF region Reg setting Recovery
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0RegRecovery(void)
{

    DebugMessageHDMI21TxLinkTraining("[HDMI21_TX] RegRecovery...\n");

    // Set Sp_pst based on sp_pst_blank and sp_pst_active Reg
    rtd_maskl(HDMITX20_MAC1_HDMI_SPDIF_CONTROL2_reg, ~_BIT5, 0x00);

    // Disable ACR Packet
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT4, _BIT4);

    // Set Rekey window size = 12
    rtd_outl(HDMITX20_MAC0_SCHEDULER_7_reg, 0x39);
    SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(0x39 + 0x01);

    // HDCP Bug
    rtd_maskl(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, ~_BIT0, _BIT0);

    // HDCP compatibility, (default 0xCA) - 0x20
    rtd_outl(HDMITX20_MAC0_SCHEDULER_5_reg, 0xAA);

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    // Hdmi21 Mac Initial
    ScalerHdmi21MacTxInitial(ScalerHdmiMacTxPxMapping(_TX2));
#endif
}
#endif //#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
#endif

//--------------------------------------------------
// Description  : HDMI Tx Reset
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0Reset(void)
{
    static UINT32 lastDumpStc_Tx0Reset = 0, dumpCnt_Tx0Reset=0;
//****************************************************************************
// Global VARIABLE DECLARATIONS
//****************************************************************************
    UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
    if((lastDumpStc_Tx0Reset == 0)|| (((UINT32)(stc - lastDumpStc_Tx0Reset)) > 90000)){ // dump period=1sec
        NoteMessageHDMITx("[HDMITX][%x] MacTx0Reset()@%x\n", (UINT32)dumpCnt_Tx0Reset, (UINT32)lastDumpStc_Tx0Reset);
        lastDumpStc_Tx0Reset = stc;
        dumpCnt_Tx0Reset = 0;
    }
    dumpCnt_Tx0Reset++;

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    // if LT PASS, can't stop send GAP pattern
    if(GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) != _LINK_TRAING_PASS)
#endif
    {
#if(_HDMI_MAC_TX0_PURE == _ON)
        // Power off Driver
        // turn off phy power will cause fifo unstable
        //ScalerHdmiPhyTxPowerOff(ScalerHdmiMacTxPxMapping(_TX0));

#endif

        // Power off Hdmi20 MAC
        ScalerHdmiMacTx0PowerOff();
    }

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    // Hdmi21 Mac Reset
    ScalerHdmi21MacTxReset(ScalerHdmiMacTxPxMapping(_TX0));
#endif
    rtd_maskl(HDMITX21_MAC_HDMI21_MIDDLE_VS_SHIFT_reg, ~HDMITX21_MAC_HDMI21_MIDDLE_VS_SHIFT_middle_vs_shift_mask, HDMITX21_MAC_HDMI21_MIDDLE_VS_SHIFT_middle_vs_shift(0));

    // Disable Color Convert function
    ScalerColorStream422To444(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_OFF);
    ScalerColorStreamRgb2Yuv(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_OFF, _RGB2YUV_ITU709_LIMIT);
    ScalerColorStream444To420(ScalerHdmiMacTxPxMapping(_TX0), _FUNCTION_OFF);

#if(_AUDIO_SUPPORT == _ON)
    ScalerHdmiMacTx0AudioPowerOff();

#if(_HW_HDMI_AUDIO_STRUCT == _HDMI_AUDIO_GEN_2)
    // Reset Audio Top Module
    rtd_maskl(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, ~_BIT3, _BIT3);
    rtd_maskl(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, ~_BIT3, 0x00);
#endif
    //Reset N/CTS regen
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en(0));

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
    CLR_HDMI_MAC_TX0_CTS_CODE_CHECK_EVENT();

    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX0_AUDIO_CTS_CODE_CHECK);

    CLR_HDMI_MAC_TX0_CTS_CODE_FW_MODE();
#endif
#endif

#if(_HDMI_MAC_TX0_SUPPORT_HDMI21 == _ON)
    // if LT PASS, can't stop send GAP pattern
    if(GET_HDMI21_MAC_TX_LINK_TRAINING_RESULT(ScalerHdmi21MacTxPxTxMapping(ScalerHdmiMacTxPxMapping(_TX0))) != _LINK_TRAING_PASS)
#endif
    {
#if(_HDMI_MAC_TX0_PURE == _ON)
        // Power off CMU
        ScalerHdmiPhyTxCMUPowerOff(ScalerHdmiMacTxPxMapping(_TX0));
#endif
    }

    SET_HDMI_MAC_TX0_STREAM_SOURCE(_HDMI_TX_SOURCE_NONE);

#if(_HW_HDMI_SW_PKT_TYPE == _HW_HDMI_SW_PKT_GEN_2)
    CLR_HDMI_MAC_TX0_SW_PKT_DFF0_USED();
    CLR_HDMI_MAC_TX0_SW_PKT_DFF1_USED();
#endif

    ScalerHdmiPhyTxTimerReset(ScalerHdmiMacTxPxMapping(_TX0));

#if(_HDMI_MAC_TX0_DP_PLUS == _ON)
    // Reset DP++
    ScalerHdmiPhyTxDpppReset(ScalerHdmiMacTxPxMapping(_TX0));
#endif
    DebugMessageHDMITx("[HDMITX] ScalerHdmiMacTx0Reset() Done\n");
    return;
}


//--------------------------------------------------
// Description  : HDMI Tx Monitor Deep color FIFO Status
// Input Value  : None
// Output Value : _UNSTABLE or _STABLE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0MonitorFIFOStatus(void)
{
    //UINT8 bDispDInput = _TRUE;//(Scaler_HDMITX_DispGetInputInfo(SLR_INPUT_HDMITX_DSC_SRC) == DSC_SRC_DISPD);

    DebugMessageHDMITx("[HDMITX] Tx0MonitorFIFOStatus... \n");

    // Clear FIFO overflow/underflow flag
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_2_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT7 | _BIT6));
    rtd_maskl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg, ~(_BIT6|_BIT5), (_BIT6|_BIT5));
    rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~_BIT6, _BIT6);

    //if(bDispDInput) // clear dispD FIFO
    //    rtd_maskl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, ~(_BIT1|_BIT0), (_BIT1|_BIT0));

    HDMITX_MDELAY(2);

    // Underflow or Overflow
    if((rtd_getbits(HDMITX20_MAC0_HDMI_CONTROL_2_reg, (_BIT7 | _BIT6)) != 0x00)
#ifndef CONFIG_MACH_RTD2801_ZEBU //Don't need check PFIFO status in Zebu platform (just for TX PHY status check)
        || (rtd_getbits(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg, (_BIT6 | _BIT5)) != 0x00)
#endif
        || (rtd_getbits(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, _BIT6) != 0x00)
#if 0 //  Ignore check DispD FIFO status when TX configure flow
        || (bDispDInput && (rtd_getbits(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, (_BIT1|_BIT0)) != 0))
#endif
        )
    {
        static UINT32 lastDumpStc_MonitorFIFOStatus=0, dumpCnt_MonitorFIFOStatus=0;
#ifndef CONFIG_MACH_RTD2801_ZEBU
        if(((UINT32)(rtd_inl(TIMER_SCPU_CLK90K_LO_reg) - lastDumpStc_MonitorFIFOStatus)) > 90000)
#endif
        { // dump period=1sec
            if(rtd_getbits(HDMITX20_MAC0_HDMI_CONTROL_2_reg, (_BIT7 | _BIT6))){
                FatalMessageHDMITx("[HDMITX] 2.0 SRAM FIFO ERR Cnt=%d,", (UINT32)dumpCnt_MonitorFIFOStatus);
                FatalMessageHDMITx("[Ud=%d| Ov=%d]\n", (UINT32)(rtd_inl(HDMITX20_MAC0_HDMI_CONTROL_2_reg) & _BIT7) >> 7, (UINT32)(rtd_inl(HDMITX20_MAC0_HDMI_CONTROL_2_reg) & _BIT6) >> 6);
            }

#ifndef CONFIG_MACH_RTD2801_ZEBU // Don't need check PFIFO status in Zebu platform (just for TX PHY status check)
            if(rtd_getbits(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg, (_BIT6 | _BIT5)))
                FatalMessageHDMITx("[HDMITX] 2.0 PFIFO ERR[Ud=%d| Ov=%d]\n", (UINT32)(rtd_inl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg) & _BIT6) >> 6, (UINT32)(rtd_inl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg) & _BIT5) >> 5);
#endif

            if(rtd_getbits(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, _BIT6))
                FatalMessageHDMITx("[HDMITX] 2.0 CTS FIFO UNSTABLE@%x\n", (UINT32)rtd_inl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg));

            //if(bDispDInput && rtd_getbits(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, (_BIT1|_BIT0))) // check dispD FIFO
            //    NoteMessageHDMI21Tx("[HDMITX] 2.0 DispD FIFO ERR[Ud=%d| Ov=%d]\n", (UINT32)(rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg) & _BIT1) >> 1, (UINT32)rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg) & _BIT0);

            lastDumpStc_MonitorFIFOStatus = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
            dumpCnt_MonitorFIFOStatus = 0;
        }
        dumpCnt_MonitorFIFOStatus++;

        // Clear FIFO overflow/underflow flag
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_2_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT7 | _BIT6));
        FatalMessageHDMITx("[HDMITX] FIFO UNSTABLE!\n");
        return _STABLE;//_UNSTABLE;
    }
    else
    {
        return _STABLE;
    }
}

// mode: 1: bypass mode(for HDMI20), 0: 2pto4p mode (for HDMI21)
void OutputConfigurationDscSrcSel(UINT8 mode)
{
    rtd_inl(HDMITX_PHY_DSC_SRC_SEL_reg); //0x00000000);
    //rtd_outl(0xb80178f8, 0x00000006);   // DSC_SRC_SEL_REG
    // Decide HDMITX_Insel function_mode: 0 => 2pto4p mode (4p valid, ch0/ch1/ch2/ch3), 1 => bypass mode (2p valid, ch0/ch1)
    rtd_outl(HDMITX_PHY_DSC_SRC_SEL_reg, HDMITX_PHY_DSC_SRC_SEL_reg_insel_mode(mode));
    rtd_maskl(HDMITX_PHY_DSC_SRC_SEL_reg, ~HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset_mask, HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset(1));

    mdelay(10);

    rtd_maskl(HDMITX_PHY_DSC_SRC_SEL_reg, ~HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_en_mask, HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_en(1));
    return;
}

//--------------------------------------------------
// Description  : HDMI Tx MAC Set
// Input Value  :
// Output Value :
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0OutputConfiguration(void)
{

    DebugMessageHDMITx("[HDMITX] Tx0OutputConfiguration()... \n");

    // [H5X] HDMITX MAC 2.0 setting
    ScalerHdmiTx0_MacConfig(ScalerHdmiTxGetTargetFrlRate() != 0? TC_HDMI21: TC_HDMI20);

    // [H5X] LANE PN Swap
    ScalerHdmiTx0_LanePNSwap();

    // [H5X] LANE src sel
    ScalerHdmiTx0_LaneSrcSel();

    // HDCP Bug
    rtd_maskl(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, ~_BIT0, _BIT0);

    // Hdmi Tx InFIFO
    ScalerHdmiMacTx0InFIFO();

    // Hdmi Tx clock >3.4G open div4
    if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() > 3400)
    {
        ScalerHdmiMacTxSetClkDiv4(ScalerHdmiMacTxPxMapping(_TX0),_ENABLE);
    }
    else
    {
        ScalerHdmiMacTxSetClkDiv4(ScalerHdmiMacTxPxMapping(_TX0),_DISABLE);
    }

    // Hdmi Tx SRAM(Deep color FIFO)
    ScalerHdmiMacTx0DPCSRAM();

    // Hdmi Tx Scheduler
    ScalerHdmiMacTx0Scheduler();

    // Hdmi Tx Encoder
    rtd_maskl(HDMITX20_MAC0_TMDS_ENCODER_3_reg, ~_BIT0, _BIT0);

    // Hdmi Tx pfifo enable
    rtd_maskl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg, ~HDMITX20_ON_REGION_PIXEL_CONV_P1_pfifo_en_mask,HDMITX20_ON_REGION_PIXEL_CONV_P1_pfifo_en(0x1));

    HDMITX_DTG_Wait_Den_Stop_Done(); //Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);

    // TX DSC SRC SEL & FIFO reset/enable
    // mode: 1: bypass mode, 0: 2pto4p mode
    // 0 => 2pto4p mode for HDMI21, 1 => bypass mode for HDMI20
    OutputConfigurationDscSrcSel(1);

    // Enable HDMI Tx module(Data Path:InFIFO-SRAM-Scheduler-Data Island Packet-Scramble-Encoder-Small FIFO)
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT7 | _BIT6), _BIT7);

    // Select HDMI Mode or DVI Mode
    if(GET_HDMI_MAC_TX0_SUPPORT() == _TRUE)
    {
        // HDMI Mode
        rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~_BIT6, _BIT6);

        // delay 1frame for P56_08[6] double buffer
        // [FIXME] wait den_end event
        //ScalerTimerDelayXms((1000 / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE()));
        // Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);

        // Hdmi Tx Data Island Packing
        ScalerHdmiMacTx0DataIslandPacking();

#if(_AUDIO_SUPPORT == _ON)
        // First Set Audio Before Power on Driver
        if(GET_HDMITX_AUDIO_INPUT_TYPE() == _TX_AUDIO_IN_RX_SPDIF){
            if(ScalerHdmiMacTx0AudioSet() == _FALSE)
            {
                DebugMessageHDMITx("[HDMI_TX] Audio Set Fail\n");
                return _FALSE;
            }
        }
#endif
    }
    else
    {
        // DVI Mode
        rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~_BIT6, 0x00);
    }

    // Small FIFO Lane3 Gen TMDS Clk(Data 1111100000)
    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL3_reg, ~_BIT3, _BIT3);
    return _TRUE;
}


void ScalerHdmiMacTx0SetSwGcpPktMuteEn(void)
{
    ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT4_ADDRESS, _HDMI_TX_GCP_MUTE_CTRL, _HDMI_TX_SW_PKT_DFF0);
    return;
}



//--------------------------------------------------
// Description  : HDMI Tx MAC Off
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0PowerOff(void)
{
    DebugMessageHDMITx("[HDMI_TX] ScalerHdmiMacTx0PowerOff()...\n");

    // Disable the transmission of Periodic RAM0-5
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), 0x00);
    // Disable the transmission of Periodic RAM6-9
    rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), 0x00);
    if(GET_HDMI_MAC_TX0_SUPPORT() == _TRUE)
    {
        // Disable HDMI mode
        rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~_BIT6, 0x00);

        // Delay 1 Frame + 2ms
        // [FIXME] wait den_end event
        //ScalerTimerDelayXms((1000 / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE() + 2));
        HDMITX_DTG_Wait_Den_Stop_Done(); //Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);
        HDMITX_MDELAY(2);
    }

    // Disable HDMI Tx module
    rtd_outl(HDMITX20_MAC0_SCHEDULER_0_reg, 0x00);

    // Disable SRAM
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_4_reg, ~_BIT7, 0x00);

    // Disable Small FIFO
    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, ~_BIT7, 0x00);

    // Reset Global
    SET_HDMI_MAC_TX0_INPUT_REPEAT_NUM(0);
    SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(8);

    SET_OUTPUT_POWER_STATUS(_POWER_STATUS_OFF);
    ScalerHdmiMacTxSetClkDiv4(ScalerHdmiMacTxPxMapping(_TX0),_DISABLE);

    //set HDR flag to false
    ScalerHdmiTxSetHDRFlag(FALSE);

    DebugMessageHDMITx("[HDMI_TX] ScalerHdmiMacTx0PowerOff() Done\n");
    return;
}


//--------------------------------------------------
// Description  : Hdmi Tx InFIFO
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0InFIFO(void)
{
    UINT16 usVBackPorch = 0;

    // [HDMITX 2.0] input FIFO channel connet: color verify on ZEBU platform
    #define HDMI_CONTROL_0_INFIFO_MASK (HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chb_swap_mask|HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chg_swap_mask|HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chr_swap_mask)
    #define HDMI_CONTROL_0_INFIFO_SETTING (\
                            HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chb_swap(0)\
                            |HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chg_swap(1)\
                            |HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chr_swap(2))
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_0_reg, ~HDMI_CONTROL_0_INFIFO_MASK, HDMI_CONTROL_0_INFIFO_SETTING);

    // Set Reg Line Count based on Vactive
    rtd_outl(HDMITX20_MAC1_TIMING_1_reg, (UINT8)(g_stHdmiMacTx0InputTimingInfo->usVHeight >> 4)); // [11:4] -> [7:0]
    rtd_maskl(HDMITX20_MAC1_TIMING_2_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), ((UINT8)(g_stHdmiMacTx0InputTimingInfo->usVHeight << 4) & 0xF0)); // usVactive[3:0] | usVBackPorch[11:8]   [3:0]->[7:4]

    // Get V Back porch = usVstart - usVsync
    usVBackPorch = (UINT16)(g_stHdmiMacTx0InputTimingInfo->usVStart - g_stHdmiMacTx0InputTimingInfo->usVSWidth);

#if 0 
    // For Dell MNT and 8CH Audio
    if(GET_HDMI_MAC_TX0_INPUT_INTERLACE() == _TRUE)
    {
        usVBackPorch = (usVBackPorch - 1);
    }
#endif

    // Set Reg Line Porch based on Back Porch
    rtd_maskl(HDMITX20_MAC1_TIMING_2_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((UINT8)(usVBackPorch >> 8) & 0x0F)); // usVactive[3:0] | usVBackPorch[11:8]   [11:8]->[3:0]
    rtd_outl(HDMITX20_MAC1_TIMING_3_reg, (UINT8)(usVBackPorch));

    DebugMessageHDMITx("[HDMI_TX] Color Space=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE());

    // Select pixelencfmt based on Pixel Color Format
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_RGB)
    {
        rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~(_BIT7 | _BIT6), 0x00);
    }
    else if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
    {
        rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~(_BIT7 | _BIT6), _BIT6);
    }
    else if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR444)
    {
        rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~(_BIT7 | _BIT6), _BIT7);
    }
    else if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR422)
    {
#if 0 
        if((!lib_hdmitx_is_hdmi_422_support()) && (get_hdmitxOutputMode() == TX_OUT_THROUGH_MODE)){ //bypass 444 to 422 in through mode (RX bypass packet)
            rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~(_BIT7 | _BIT6), 0);
            //rtd_maskl(PPOVERLAY_OUTTG_OUTTG_dispd_ds444_reg, ~PPOVERLAY_OUTTG_OUTTG_dispd_ds444_ds444_mode_mask, 0); // 0: bypass
        }
        else
#endif
        rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
    }

    // Set pixelencycbcr422 = 1 when need to output Ycbcr422(Double Buffer Register)
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR422)
    {
        rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~_BIT3, _BIT3);

        // Set ycbcr422 10/12 BOOLEAN
        rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~_BIT5, 0x00);
    }

    // Select VSYNC/HSYNC pol For CTS
    ScalerHdmiMacTx0HVPol();

    rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT3), _BIT3);
    rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT3), 0x00);
}

//--------------------------------------------------
// Description  : Hdmi Tx Hsync/Vsync Pol
// Input Value  :
// Output Value :
//--------------------------------------------------
#if 1
void ScalerHdmiMacTx0HVPol(void)
{
    UINT16 usHFrontPorch = 0;

    // Calculate HfrontPorch = Hblank - Hstart = usHtotal - usHactive - Hstart
    usHFrontPorch = (g_stHdmiMacTx0InputTimingInfo->usHTotal - g_stHdmiMacTx0InputTimingInfo->usHWidth - g_stHdmiMacTx0InputTimingInfo->usHStart);

    // HfrontPorch >= Hsync
    if(usHFrontPorch >= g_stHdmiMacTx0InputTimingInfo->usHSWidth)
    {
        // Hpol/Vpol : P/P
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT1), _BIT1);
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT2), _BIT2);
    }
    else
    {
        // VIC = 39
        if((usHFrontPorch == 32) && (g_stHdmiMacTx0InputTimingInfo->usHSWidth == 168))
        {
            // Hpol/Vpol : P/N
            rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT1), _BIT1);
            rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT2), 0x00);
        }
        else
        {
            // Hpol/Vpol : N/N
            rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT1), 0x00);
            rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT2), 0x00);
        }
    }

    // dump H/V polarity status
    DebugMessageHDMITx("[HDMITX] POL H=%d\n", (UINT32)HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_get_hs_out_inv(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg));
    DebugMessageHDMITx("[HDMITX] POL V=%d\n", (UINT32)HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_get_vs_out_inv(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg));
    DebugMessageHDMITx("[HDMITX] hs/start=%d/%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHSWidth, (UINT32)g_stHdmiMacTx0InputTimingInfo->usHStart);
    DebugMessageHDMITx("[HDMITX] Width/front=%d/%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHWidth, (UINT32)usHFrontPorch);
    DebugMessageHDMITx("[HDMITX] H-Total=%d\n", (UINT32)g_stHdmiMacTx0InputTimingInfo->usHTotal);

    return;
}
#endif
#if 0
void ScalerHdmiMacTx0HVPol(void)
{
    UINT8 hdmirx_pol = 0x3;
#ifdef CONFIG_RTK_HDMI_RX
    if(g_scaler_display_info.input_src == 0)
        hdmirx_pol = GET_HDMIRX_RAW_TIMING_POLARITY();
#endif
    FatalMessageHDMITx("[HDMI20_TX]polarity=%d\n", (UINT32)hdmirx_pol);
    // 0 : hpol=N,vpol=N, 1 : hpol=P,vpol=N, 2 : hpol=N,vpol=P, 3 : hpol=P,vpol=P
    if(hdmirx_pol == 0) {
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT1), 0x00);
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT2), 0x00);
    } else if(hdmirx_pol == 1) {
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT1), _BIT1);
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT2), 0x00);
    } else if(hdmirx_pol == 2) {
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT1), 0x00);
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT2), _BIT2);
    } else {
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT1), _BIT1);
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(_BIT6 | _BIT2), _BIT2);
    }
}
#endif

//--------------------------------------------------
// Description  : Hdmi Tx SRAM(Deep color FIFO)
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0DPCSRAM(void)
{
    // Select repeat mode
    if(GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() > 0)
    {
        // Set repeat_mode
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_5_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() << 4));

        // Set repetition_en = 1
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_5_reg, ~_BIT3, _BIT3);
    }
    else
    {
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_5_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), 0x00);
    }

    // Select Deep color mode
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() != _DP_STREAM_COLOR_DEPTH_8_BITS)
    {
#if(_HDMI_TX_CTS_TEST == _ON)
        // for QD980 Compatibility : if get CTS EDID, Set DPC initial phase 0 of VS rising
        if(GET_HDMI_MAC_TX0_FIND_CTS_TEST_EDID() == _TRUE)
        {
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_2_reg, ~(_BIT7 | _BIT3), _BIT3);
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_2_reg, ~(_BIT7 | _BIT2 | _BIT1 | _BIT0), 0x00);
        }
#endif
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        if((g_scaler_display_info.input_src == 0) && GET_HDMIRX_DPC_DEFAULT_PH())
        {
            //dp_phase_default = 1
            rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~ _BIT2, _BIT2);
            //dp_phase_default_en = 1
            rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~_BIT3, _BIT3);
        }
        else
#endif
        {
            //dp_phase_default = 0 dp_phase_default_en = 0
            rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~(_BIT3 | _BIT2), 0x00);
        }

        // Set dp_enable = 1
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~(_BIT1 | _BIT0), _BIT1);

        // Set dp_color_depth
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_7_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), ((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 2) << 4));

        // Set dp_vch_mode=1 Double Buffer for Deep color Register
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~_BIT0, _BIT0);
    }
    else
    {
        // Set dp_color_depth
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_7_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), ((_DP_STREAM_COLOR_DEPTH_8_BITS / 2) << 4));

#if 0//#ifdef FIX_ME_HDMITX_BRING_UP //force dp enable
        // Set dp_enable = 1
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~(_BIT1 | _BIT0), _BIT1);
#else
        //dp_phase_default = 0 dp_phase_default_en = 0
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~(_BIT3 | _BIT2), 0x00);
        // Set dp_enable = 0 ,dp_phase_default=0 , dp_phase_default_en=0
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, ~(_BIT1 | _BIT0), 0x00);
#endif // #ifdef FIX_ME_HDMITX_BRING_UP //force dp enable
    }

    // sram_en = 1
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_4_reg, ~_BIT7, _BIT7);
}

//--------------------------------------------------
// Description  : Hdmi Tx Scheduler
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0Scheduler(void)
{
    UINT16 usHstart = g_stHdmiMacTx0InputTimingInfo->usHStart;
    UINT16 usPre_Keepout_Start = 0x00;


    // if HDMI convert color space to 420, Hstart should be /2
#ifdef SCALER_SUPPORT_420_CONFIG
    if((ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420) && (GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420))
#else
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
#endif
    {
        usHstart = usHstart / 2;
    }

    usHstart = usHstart * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8 * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1);

    if(usHstart > _HDMI_MAC_TX0_PRE_KEEP_OUT_SIZE)
    {
        usPre_Keepout_Start = usHstart - _HDMI_MAC_TX0_PRE_KEEP_OUT_SIZE;
    }
    else
    {
        // if set 0, HW will not send any ASP
        usPre_Keepout_Start = 0;

        // disable Audio
        SET_HDMI_MAC_TX0_BW_AUDIO_MUTE();
    }

    // Select maximum number of packets in Vblanking: 1 packet
#ifdef FIX_ME_HDMITX_BRING_UP
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
#else
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
#endif

#ifdef CONFIG_MACH_RTD2801_ZEBU
    rtd_outl(HDMITX20_MAC0_SCHEDULER_1_reg, HDMITX20_MAC0_SCHEDULER_1_vactive_island_max_pkt_num(1));
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~HDMITX20_MAC0_SCHEDULER_1_gcp_en_mask, HDMITX20_MAC0_SCHEDULER_1_gcp_en(1));
#else
    // Select maximum number of packets in Hblanking: 2 packet
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), HDMITX20_MAC0_SCHEDULER_1_vactive_island_max_pkt_num(1));
#endif // #ifdef CONFIG_MACH_RTD2801_ZEBU


    rtd_outl(HDMITX20_MAC0_SCHEDULER_3_reg, (UINT8)usPre_Keepout_Start & 0xff);
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_4_reg, ~(HDMITX20_MAC0_SCHEDULER_4_video_pre_keepout_start_14_8_mask), ((UINT8)((usPre_Keepout_Start >> 8) << 1) & HDMITX20_MAC0_SCHEDULER_4_video_pre_keepout_start_14_8_mask));
    //turn off HDCP enable
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_2_reg, ~(_BIT7), 0x00);
}

//--------------------------------------------------
// Description  : Hdmi Tx DataIsland Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0DataIslandPacking(void)
{
    DebugMessageHDMITx("[HDMITX] ScalerHdmiMacTx0DataIslandPacking() ...\n");

    // HW Set Packet
    // When High, no clock recovery packets will be sent.
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT4, _BIT4);

    // Periodic SRAM Packet
    ScalerHdmiMacTx0PeriodicSRAMPacking();

    // HDMITX packet bypass setting
    ScalerHdmiMacTx0PacketBypassConfig();

    // GCP Packet
#if(_HDMI_TX_CTS_TEST == _ON)
    // solution for Goodway HDMI2.0 Tx 4K60 CTS Fail
    if(SCRIPT_TX_SKIP_AVMUTE_BYPASS() && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5000) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS))
    {
        // gcp_en = 0, send GCP Packet by SW Packet
        rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~_BIT2, 0x00);

        // Delay 1 Frame + 2ms
        //ScalerTimerDelayXms((1000 / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE() + 2));
        HDMITX_DTG_Wait_Den_Stop_Done(); //Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);

        DebugMessageHDMITx("[HDMITX] GCP Packet is Disabled(SW packet Mode)!\n");
        HDMITX_MDELAY(2);
    }
    else
#endif // #if(_HDMI_TX_CTS_TEST == _ON)
    {
        ScalerHdmiMacTx0GCPPacket();
    }
}





#if(_AUDIO_SUPPORT == _ON)
void ScalerHdmiMacTx0AudioInputTypeSet(EnumHdmiTxAudioInputType type)
{
    if(type < _TX_AUDIO_IN_NUM){
        if(GET_HDMITX_AUDIO_INPUT_TYPE() != type){
            DebugMessageHDMITx("[HDMI_TX] Set Audio Input Type=%d\n", (UINT32) type);
            SET_HDMITX_AUDIO_INPUT_TYPE(type);
        }
    }
    return;
}

//--------------------------------------------------
// Description  : Hdmi Tx Audio Set
// Input Value  :
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0AudioSet(void)
{
    UINT8 value_u8 = 0;
    UINT8 audFmtChange=0;
    UINT8 ucAudioSampleNuminOnePacket = 0;
    //UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

    // Audio Exist(Audio Frequency Range Detect Result and VB-ID amute flag = 0)
    if((ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE) != _AUDIO_FREQ_NO_AUDIO)
            && (ScalerStreamGetAudioMuteFlag(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE))
    {
        // Audio Firstly deal or Audio Format/ Frequency/Audio CH Num change
        if((GET_HDMI_MAC_TX0_AUDIO_FREQ() != ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE)) ||
           (GET_HDMI_MAC_TX0_AUDIO_CH_NUM() != (ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT) + 1)) ||
           (GET_HDMI_MAC_TX0_AUDIO_FORMAT() != (ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CODING_TYPE))) ||
           (ScalerHdmiMacTx0DetectHdcpRekeyWindowChg() == _TRUE))
        {
            // Save Audio Ch Num and Frequency infomation
            audFmtChange = 0; // check audio format change
            value_u8 = ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE);
            if(GET_HDMI_MAC_TX0_AUDIO_FREQ() != value_u8){
                FatalMessageHDMITx("[HDMI_TX]  Audio Freq=%d->", (UINT32)value_u8);
                FatalMessageHDMITx("%d\n", (UINT32)GET_HDMI_MAC_TX0_AUDIO_FREQ());
                SET_HDMI_MAC_TX0_AUDIO_FREQ(value_u8);
                audFmtChange=1;
            }
            value_u8 = ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT);
            if(GET_HDMI_MAC_TX0_AUDIO_CH_NUM() != (value_u8 + 1)){
                FatalMessageHDMITx("[HDMI_TX]  Audio CH=%d->", (UINT32)value_u8+1);
                FatalMessageHDMITx("%d\n", (UINT32)GET_HDMI_MAC_TX0_AUDIO_CH_NUM());
                SET_HDMI_MAC_TX0_AUDIO_CH_NUM(value_u8 + 1);
                audFmtChange=1;
            }
            value_u8 = ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CODING_TYPE);
            if(GET_HDMI_MAC_TX0_AUDIO_FORMAT() != value_u8){
                FatalMessageHDMITx("[HDMI_TX]  Audio Format=%d->", (UINT32)value_u8);
                FatalMessageHDMITx("%d\n", (UINT32)GET_HDMI_MAC_TX0_AUDIO_FORMAT());
                SET_HDMI_MAC_TX0_AUDIO_FORMAT(value_u8);
                audFmtChange=1;
            }
#if(AUDIO_BYPASS)
            rtd_maskl(HDMITX_MISC_HDMITX_AUDIO_BYPASS_reg,~(_BIT29 | _BIT28| _BIT4| _BIT0),_BIT29 | _BIT28| _BIT4| _BIT0);
#endif
#if 0
            //if(((UINT32)(stc - lastDumpStc_AudioSet)) > 90000){ // dump period=1sec
            if(audFmtChange|| (GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)){
                WarnMessageHDMITx("[HDMI_TX] Audio Freq/CH/Fmt=%d/", (UINT32)GET_HDMI_MAC_TX0_AUDIO_FREQ());
                WarnMessageHDMITx("%d/", (UINT32)GET_HDMI_MAC_TX0_AUDIO_CH_NUM());
                WarnMessageHDMITx("%d@", (UINT32)GET_HDMI_MAC_TX0_AUDIO_FORMAT());
                //lastDumpStc_AudioSet = stc;
            }
#endif


#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
            /*if(ScalerDpStreamGetAudioFwTrackingStableStatus(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE)
            {
                // Set afifo_en = 0(Disable Audio Data into HDMI Module)
                CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();
            }
            else*/
            {
                // Set afifo_en = 1(Enable Audio Data into HDMI Module)
                SET_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();
            }
#else
            SET_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();
#endif

            // -----------------------------------------------------------
            // FW Precheck Audio BW Avaliable
            // -----------------------------------------------------------
            ucAudioSampleNuminOnePacket = ScalerHdmiMacTx0CalAudioSampleNuminOnePacket();

            if(GET_HDMI_MAC_TX0_AUDIO_BW_ERROR() == _TRUE)
            {
                if((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == 8) && (GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() <= 3000))
                {
                    FatalMessageHDMITx("[HDMI_TX] AUDIO BW ERR@ 8bit clock@%d\n", (UINT32)GET_HDMI_MAC_TX0_DEEP_COLOR_CLK());
                    return _FALSE;
                }
                else
                {
                    CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();

                    // Disable Audio FIFO
                    CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();

                    SET_HDMI_MAC_TX0_AUDIO_FREQ(0x00);
                    SET_HDMI_MAC_TX0_AUDIO_CH_NUM(0x00);

                    // Disable ACR Packet
                    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT4, _BIT4);

                    FatalMessageHDMITx("[HDMI_TX] AUDIO BW ERR@ color depth=%d/", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());
                    FatalMessageHDMITx("clock=%d\n", (UINT32)GET_HDMI_MAC_TX0_DEEP_COLOR_CLK());
                    return _TRUE;
                }
            }

            // -----------------------------------------------------------
            // Set Audio Sample Packet Relatived Setting
            // -----------------------------------------------------------
            ScalerHdmiMacTx0AudioSamplePktProc(ucAudioSampleNuminOnePacket);

            // -----------------------------------------------------------
            // Set Audio Auxiliary Info Pkt
            // -----------------------------------------------------------
#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
            SET_HDMI_MAC_TX0_CTS_CODE_FW_MODE();
#endif
#if(AUDIO_BYPASS)
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT2, _BIT2);

            ScalerHdmiMacTx0ACRPacket(audFmtChange);
            // ACR Packet will be sent
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT4, 0x00);

#else
            ScalerHdmiMacTx0ACRPacket(audFmtChange);

            // ACR Packet will be sent
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT4, 0x00);

            // Packet1: Audio infoframe
            // disable pkt transmission

            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT3, 0x00);

            ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT1_ADDRESS, _HDMI_TX_AUDIO_INFOFRAME, _HDMI_TX_SW_PKT_DFF0);

            // enable pkt transmission
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT3, _BIT3);

#endif
#if(_HDMI_3D_AUDIO_TX0_SUPPORT == _ON)
            if(ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CODING_TYPE) == _3D_LPCM_ASP)
            {
                // Packet7: Audio Metadata Packet
                // disable pkt transmission
                rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~_BIT5, 0x00);
                ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT7_ADDRESS, _HDMI_TX_AUDIO_METADATA, _HDMI_TX_SW_PKT_DFF1);

                // enable pkt transmission
                rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~_BIT5, _BIT5);
            }
#endif

            // -----------------------------------------------------------
            // Set hdmi_tx_spdif_en = 1, SPDIF Audio Format to HDMI Audio Packet
            // -----------------------------------------------------------
            rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL_reg, ~_BIT4, _BIT4);

            HDMITX_MDELAY(10);
        }

        // VG870 audio 2ch >> 8ch 0x137A[4]=1(audio SRAM full)
        // no audio output in this case, FW need to reset audio FIFO(0x126C[0])
        // Chech audio sram status
        if(rtd_getbits(HDMITX20_MAC1_HDMI_AUDIO_SRAM_0_reg, _BIT4) == _BIT4)
        {
            // Reset Audio FIFO
            CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();
            SET_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();

            HDMITX_MDELAY(10);

            rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_SRAM_0_reg, ~(_BIT5 | _BIT4), _BIT4);
        }

    }
    else
    {
        if((ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE) == _AUDIO_FREQ_NO_AUDIO)
                && (ScalerStreamGetAudioMuteFlag(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE))
        {

#if(AUDIO_BYPASS)
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT2, 0x00);
            rtd_maskl(HDMITX_MISC_HDMITX_AUDIO_BYPASS_reg,~(_BIT29 | _BIT28| _BIT4| _BIT0), 0x00);

#endif
            if(rtd_getbits(HDMITX20_MAC1_HDMI_AUDIO_CTRL_reg, _BIT4) == _BIT4)
            {
                // ACR Packet will be not sent
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT4, _BIT4);

                // Set hdmi_tx_spdif_en = 0, No SPDIF Audio Format to HDMI Audio Packet
                rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL_reg, ~_BIT4, 0x00);

                // Set afifo_en = 0 Disable Audio FIFO
                CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();

                // Packet1: Set Null Packet
                // disable pkt transmission
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT3, 0x00);
                ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT1_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF0);

                // enable pkt transmission
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~_BIT3, _BIT3);

#if(_HDMI_3D_AUDIO_TX0_SUPPORT == _ON)
                // Packet7: Set Null Packet
                // disable pkt transmission
                rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~_BIT5, 0x00);
                ScalerHdmiMacTx0FillPktData(_HDMI_TX_SW_PKT7_ADDRESS, _HDMI_TX_NULL, _HDMI_TX_SW_PKT_DFF1);

                // enable pkt transmission
                rtd_maskl(HDMITX20_MAC1_HDMI21_SW_PKT_42_reg, ~_BIT5, _BIT5);
#endif
                WarnMessageHDMITx("[HDMI_TX]  Audio OFF/Mute\n");
            }
        }
        SET_HDMI_MAC_TX0_AUDIO_FREQ(0);
        SET_HDMI_MAC_TX0_AUDIO_CH_NUM(0);
    }
    return _TRUE;
}
#endif // ScalerHdmiMacTx0AudioSet

//--------------------------------------------------
// Description  : Hdmi Tx SW Packet Fine Tune, Handle For DP Rx Pkt change
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0PktFineTune(void)
{

#if(_AUDIO_SUPPORT == _ON)
    // Check Audio Setting only when HDMI Mode
    if(GET_HDMI_MAC_TX0_SUPPORT() == _TRUE)
    {
        if(GET_HDMITX_AUDIO_INPUT_TYPE() == _TX_AUDIO_IN_RX_SPDIF)
            ScalerHdmiMacTx0AudioSet();
    }

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
    ScalerHdmiMacTx0CtsCodeHandler();
#endif
#endif // #if(_AUDIO_SUPPORT == _ON)

#if(_HDMI_HDR10_TX0_SUPPORT == _ON)
    ScalerHdmiMacTx0HDRSet();
#endif

#if(_HDMI_FREESYNC_TX0_SUPPORT == _ON)
    ScalerHdmiMacTx0FreesyncSet();
#endif

    if(need_sw_avi_packet){
        ScalerHdmiMacTx0CheckAviUpdate(TC_HDMI20);
    }

#ifdef CONFIG_ENABLE_ZEBU_BRING_UP 
    if(rtd_inl(0xB8028304) & _BIT16){
        EnumColorSpace colorFmt = rtd_inl(0xB8028304) & 0x3;

        msleep(3000);

        // 0: RGB, 2: YUB444
        SET_HDMI_MAC_TX0_INPUT_COLOR_SPACE(colorFmt);
        Scaler_HDMITX_DispSetInputInfo(SCALER_INFO_COLOR_SPACE, colorFmt);
        NoteMessageHDMI21Tx("[HDMI_TX] Set AVI COLOR SPACE=%d\n", (UINT32)colorFmt);
        ScalerHdmiMacTx0FillAVIInfoPacket();
        rtd_clearbits(0xB8028304, _BIT16);
    }
#endif // #ifdef CONFIG_ENABLE_ZEBU_BRING_UP

}


//--------------------------------------------------
// Description  : Hdmi Tx GCP Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0GCPPacket(void)
{
    UINT8 timeout = 3;//(value_u8? 10: 1);

    // gcp_setavmute = 0
    // cp_clravmute = 1
#ifndef CONFIG_MACH_RTD2801_ZEBU
//if((rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg) & HDMITX20_MAC0_SCHEDULER_1_gcp_en_mask) == 0)
#endif
{
    DebugMessageHDMITx("[HDMI_TX] Enable GCP...\n");

    rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT0);

    // Delay 1 Frame + 2ms
    // [FIXME] wait den_end event
    //ScalerTimerDelayXms((1000 / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE() + 2));
    // wait gcp_clravmute done
    if(rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg) & HDMITX20_MAC0_SCHEDULER_1_gcp_en_mask)
        HDMITX_DTG_Wait_Den_Stop_Done();

    HDMITX_MDELAY(2);

    // gcp_en = 1
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT0));
    // Delay 1 Frame + 2ms
    //ScalerTimerDelayXms((1000 / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE() + 2));
}

//#ifdef FIX_ME_HDMITX_BRING_UP
    //value_u8 = (SCRIPT_TX_DTG_FSYNC_EN() && SCRIPT_TX_ENABLE_PACKET_BYPASS()? _TRUE: _FALSE);
    while(timeout--){
        if(rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg) & HDMITX20_MAC0_SCHEDULER_1_gcp_en_mask)
            break;
        if(timeout == 0){
            DebugMessageHDMITx("[HDMITX][ERROR] Wait GCP Enable Timeout!!\n");
            break;
        }
        HDMITX_DTG_Wait_Den_Stop_Done(); //Scaler_Wait_for_event(PPOVERLAY_DTG_pending_status_reg, PPOVERLAY_DTG_pending_status_mv_den_end_event_mask,1);
        HDMITX_MDELAY(2);
    }
//#endif
    DebugMessageHDMITx("[HDMI_TX] GCP=%d, timeout=%d\n", (UINT32)rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg), (UINT32)timeout);
    return;
}



//--------------------------------------------------
// Description  : Hdmi Tx ACR Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
//static UINT32 ulHDMITxACRNcode;// = 0;


#if(_AUDIO_SUPPORT == _ON)
void ScalerHdmiMacTx0ACRPacket(UINT8 audFmtChange)
{
    UINT32 ulHDMITxACRNcode = 0;
    UINT16 usAudioSampleFreq = 480; // Unit:0.1KHz
#if(_HDMI_TX_ACR_CLAMP_SUPPORT == _ON)
    UINT32 ulHDMIRefCTScode;// = 0;
    UINT32 ulHDMIHiBondCTScode;// = 0;
    UINT32 ulHDMILowBondCTScode;// = 0;
#endif
    //UINT32 stc = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);

// Set N based on TMDS clk and Audio Sample clk
    switch(GET_HDMI_MAC_TX0_AUDIO_FREQ())
    {
        case _AUDIO_FREQ_32K:

            if((GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970) || (GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 5940))
            {
                ulHDMITxACRNcode = 3072;
            }
            else
            {
                ulHDMITxACRNcode = 4096;
            }

            usAudioSampleFreq = 320;

            break;

        case _AUDIO_FREQ_64K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 6144;
            }
            else
            {
                ulHDMITxACRNcode = 8192;
            }

            usAudioSampleFreq = 640;

            break;

        case _AUDIO_FREQ_128K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 12288;
            }
            else
            {
                ulHDMITxACRNcode = 16384;
            }

            usAudioSampleFreq = 1280;

            break;

        case _AUDIO_FREQ_44_1K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 4704;
            }
            else if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 5940)
            {
                ulHDMITxACRNcode = 9408;
            }
            else
            {
                ulHDMITxACRNcode = 6272;
            }

            usAudioSampleFreq = 441;

            break;

        case _AUDIO_FREQ_88_2K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 9408;
            }
            else if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 5940)
            {
                ulHDMITxACRNcode = 18816;
            }
            else
            {
                ulHDMITxACRNcode = 12544;
            }

            usAudioSampleFreq = 882;

            break;

        case _AUDIO_FREQ_176_4K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 18816;
            }
            else if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 5940)
            {
                ulHDMITxACRNcode = 37632;
            }
            else
            {
                ulHDMITxACRNcode = 25088;
            }

            usAudioSampleFreq = 1764;

            break;

        case _AUDIO_FREQ_48K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 5120;
            }
            else
            {
                ulHDMITxACRNcode = 6144;
            }

            usAudioSampleFreq = 480;

            break;

        case _AUDIO_FREQ_96K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 10240;
            }
            else
            {
                ulHDMITxACRNcode = 12288;
            }

            usAudioSampleFreq = 960;

            break;

        case _AUDIO_FREQ_192K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 20480;
            }
            else
            {
                ulHDMITxACRNcode = 24576;
            }

            usAudioSampleFreq = 1920;

            break;
        case _AUDIO_FREQ_256K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 24576;
            }
            else
            {
                ulHDMITxACRNcode = 32768;
            }
            usAudioSampleFreq = 2560;
            break;

        case _AUDIO_FREQ_352_8K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 37632;
            }
            else if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 5940)
            {
                ulHDMITxACRNcode = 75264;
            }
            else
            {
                ulHDMITxACRNcode = 50176;
            }

            usAudioSampleFreq = 3528;
            break;

        case _AUDIO_FREQ_384K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 40960;
            }
            else
            {
                ulHDMITxACRNcode = 49152;
            }

            usAudioSampleFreq = 3840;
            break;

        case _AUDIO_FREQ_512K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 49152;
            }
            else
            {
                ulHDMITxACRNcode = 65536;
            }
            usAudioSampleFreq = 5120;
            break;
        case _AUDIO_FREQ_705_6K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 75264;
            }
            else if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 5940)
            {
                ulHDMITxACRNcode = 150528;
            }
            else
            {
                ulHDMITxACRNcode = 100352;
            }
            usAudioSampleFreq = 7056;
            break;
        case _AUDIO_FREQ_768K:

            if(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() == 2970)
            {
                ulHDMITxACRNcode = 81920;
            }
            else
            {
                ulHDMITxACRNcode = 98304;
            }
            usAudioSampleFreq = 7680;
            break;

        default:

            break;
    }

    //when N is ODD, need correct to EVEN
    if(ulHDMITxACRNcode % 2) {
        ulHDMITxACRNcode = ulHDMITxACRNcode << 1;
    }

    if(audFmtChange|| (GET_HDMI_MAC_TX0_STREAM_SOURCE() == _HDMI_TX_SOURCE_NONE)){
        FatalMessageHDMITx("[HDMITX] Audio FreqType/Freq=%d/", (UINT32)AudioFreqType);
        FatalMessageHDMITx("%d/,", (UINT32)GET_HDMI_MAC_TX0_AUDIO_FREQ());
        FatalMessageHDMITx("Sample=%d,", (UINT32)usAudioSampleFreq);
        FatalMessageHDMITx("Ncode=%d\n", (UINT32)ulHDMITxACRNcode);
    }

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_6_reg, (UINT8)(ulHDMITxACRNcode)); // [7:0]
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_7_reg, (UINT8)(ulHDMITxACRNcode >> 8)); // [15:8] -> [7:0]
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_8_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), ((UINT8)(ulHDMITxACRNcode >> 12) & 0xF0)); // [19:16] -> [7:4]

    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT5, 0x00);

#if(_HDMI_TX_ACR_CLAMP_SUPPORT == _ON)

    // Cal the Ref CTS Value
    // CTS = TMDS_Clk * N / (128 * fs)
    ulHDMIRefCTScode = (UINT32)(ulHDMITxACRNcode * GET_HDMI_MAC_TX0_DEEP_COLOR_CLK());
    // / 128 * 1000 = / 16 * 125
    ulHDMIRefCTScode = (UINT32)(ulHDMIRefCTScode / 16);
    ulHDMIRefCTScode = (UINT32)(ulHDMIRefCTScode  * 125);
    ulHDMIRefCTScode = (UINT32)(ulHDMIRefCTScode /usAudioSampleFreq);

#if 0
    if(((UINT32)(stc - lastDumpStc_ACRPacket)) > 90000){ // dump period=1sec
        NoteMessageHDMITx("[HDMI_TX] ulHDMITxACRNcode=%x\n", (UINT32)ulHDMITxACRNcode);

        NoteMessageHDMITx("[HDMI_TX] ulHDMIRefCTScode=%x\n", (UINT32)ulHDMIRefCTScode);
        lastDumpStc_ACRPacket = stc;
    }
#endif

    // Cal Hi/ Low boundary value ( 10%)

    // Set the Upper/ Lower CTS Value of ACR
    ulHDMIHiBondCTScode = ulHDMIRefCTScode + (ulHDMIRefCTScode / 10);
    rtd_outl(HDMITX20_MAC0_HDMI_AUD_CTS_CLAMP1_reg, LOBYTE(LOWORD(ulHDMIHiBondCTScode)));
    rtd_outl(HDMITX20_MAC0_HDMI_AUD_CTS_CLAMP2_reg, HIBYTE(LOWORD(ulHDMIHiBondCTScode)));
    rtd_outl(HDMITX20_MAC0_HDMI_AUD_CTS_CLAMP3_reg, (LOBYTE(HIWORD(ulHDMIHiBondCTScode)) & 0x0F) << 4);

    ulHDMILowBondCTScode = ulHDMIRefCTScode - (ulHDMIRefCTScode / 10);
    rtd_outl(HDMITX20_MAC0_HDMI_AUD_CTS_CLAMP4_reg, LOBYTE(LOWORD(ulHDMILowBondCTScode)));
    rtd_outl(HDMITX20_MAC0_HDMI_AUD_CTS_CLAMP5_reg, HIBYTE(LOWORD(ulHDMILowBondCTScode)));
    rtd_outl(HDMITX20_MAC0_HDMI_AUD_CTS_CLAMP6_reg, (LOBYTE(HIWORD(ulHDMILowBondCTScode)) & 0x0F) << 4);

    // Enable Clamp Function
    rtd_maskl(HDMITX20_MAC0_HDMI_AUD_CTS_CLAMP0_reg, ~_BIT7, _BIT7);

#endif

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
    g_ulHdmiMacTx0FwCTSVaule = 0;

    if(GET_HDMI_MAC_TX0_CTS_CODE_FW_MODE() == _TRUE)
    {
        // FW calucate CTS value
        rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT3, _BIT3);

        // CTS = TMDS_Clk * N / (128 * fs)
        g_ulHdmiMacTx0FwCTSVaule = (UINT32)(ulHDMITxACRNcode * GET_HDMI_MAC_TX0_DEEP_COLOR_CLK());
        DebugMessageHDMITx("[HDMI_TX] DEEP_COLOR_CLK=%x\n", (UINT32)GET_HDMI_MAC_TX0_DEEP_COLOR_CLK());
        // / 128 * 1000 = / 16 * 125
        g_ulHdmiMacTx0FwCTSVaule = (UINT32)(g_ulHdmiMacTx0FwCTSVaule / 16);
        g_ulHdmiMacTx0FwCTSVaule = (UINT32)(g_ulHdmiMacTx0FwCTSVaule  * 125);
        g_ulHdmiMacTx0FwCTSVaule = (UINT32)(g_ulHdmiMacTx0FwCTSVaule / usAudioSampleFreq);

        rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_10_reg, (UINT8)(g_ulHdmiMacTx0FwCTSVaule >> 12)); // [19:12] -> [7:0]
        rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_9_reg, (UINT8)(g_ulHdmiMacTx0FwCTSVaule >> 4)); // [11:4] -> [7:0]
        rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_8_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((UINT8)(g_ulHdmiMacTx0FwCTSVaule) & 0x0F)); // [3:0]

        DebugMessageHDMITx("[HDMI_TX] CTScode=%x\n", (UINT32)g_ulHdmiMacTx0FwCTSVaule);
        // N Value = 6144 = 0x1800    CTS Vaule=148500 = 0x24414  1080p 48KHz

    }
    else
    {
        // acrp_cts_source = 0  HW auto calucate CTS value
        rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT3, 0x00);
    }
#else
    // acrp_cts_source = 0  HW auto calucate CTS value
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT3, 0x00);
#endif
}


#if(_AUDIO_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Audio Sample Packet Proc
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0AudioSamplePktProc(UINT8 ucAudioSampleNuminOnePacket)
{
#if(_HW_HDMI_AUDIO_STRUCT == _HDMI_AUDIO_GEN_2)
    // Audio SRAM Input Fix to 1 sp & Clr sp_pst_fw_md
    rtd_maskl(HDMITX20_MAC1_HDMI_SPDIF_CONTROL_reg, ~(_BIT5 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
#endif

    switch(ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CODING_TYPE))
    {
        case _LPCM_ASP:
            // Audio Packet Header to ASP
            rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_1_reg, 0x02);

            // Set Max Continue Pkt Size to 2, vblank_island_max_pkt_num(bit0~bit4) = 1
            if(GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == 8){
                // CTS 1.4 7-29, CTS Interval, VBanking 1 = 2 packet
                // CTS 1.4 7-29, CTS Interval, VActive 1 = 2 packet, vactive_island_max_pkt_num(bit3~bit7) = 1
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), _BIT3);
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT6));
            } else{
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), 0x00);
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT6));
            }
            if(SCRIPT_TX_SKIP_AVMUTE_BYPASS() && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5000) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS))
            {
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
            }
            else
            {
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~( _BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT0));
            }



            // 8 ch
            if(GET_HDMI_MAC_TX0_AUDIO_CH_NUM() > 2)
            {
                // Layout = 1
                // bit3: asp_audiolayout=1(8ch, Layout 1), bit4: asp_force_samplepresent = 0, bit5: asp_force_samplepresent = 0
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_0_reg, ~(_BIT5 | _BIT4 | _BIT3), _BIT3);

                // Audio Pkt: Fix mode
                rtd_maskl(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, ~_BIT2, 0x00);

                // Set 8ch Output
                rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL2_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT0);
            }
            else // 2 ch
            {
                // Layout = 0
                // Set sp & b frame HW mode
                rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_0_reg, ~(_BIT5 | _BIT4 | _BIT3), 0x00);
                rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_2_reg, 0x00);

                // Set Aud Pkt Mode
#if(_DEFAULT_AUDIO_PKT_MODE == _AUD_FIX_MODE)
                rtd_maskl(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, ~_BIT2, 0x00);
#endif
                // Set 2ch Output
                rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL2_reg, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
            }

            rtd_maskl(HDMITX20_MAC1_HDMI_SPDIF_CONTROL_reg, ~(_BIT5), HDMITX20_MAC1_HDMI_SPDIF_CONTROL_sp_pst_fw_md(0x1));

            // Set sp# for an ASP
#if(_HW_HDMI_AUDIO_STRUCT == _HDMI_AUDIO_GEN_1)
            rtd_maskl(HDMITX20_MAC1_HDMI_SPDIF_CONTROL_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (((ucAudioSampleNuminOnePacket - 1) << 2) | (ucAudioSampleNuminOnePacket - 1)));
#else
            if(rtd_getbits(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, _BIT2) == 0x00)
            {
                rtd_maskl(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, ~(_BIT1 | _BIT0), (ucAudioSampleNuminOnePacket - 1));
            }
#endif
            break;

#if(_HDMI_3D_AUDIO_TX0_SUPPORT == _ON)
        case _3D_LPCM_ASP:

            // Audio Packet Header to 3D ASP
            rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_1_reg, 0x0B);

            // Audio Pkt: Fix mode
            rtd_maskl(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, ~_BIT2, 0x00);

            if(GET_HDMI_MAC_TX0_AUDIO_CH_NUM() <= 12)
            {
                // Set Max Continue Pkt Size to 2
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT6));

                if(SCRIPT_TX_SKIP_AVMUTE_BYPASS() && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5000) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS))
                {
                    rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT3);
                }
                else
                {
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT0));
                }

                // DP Audio Data Format: 12 ch
                rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL2_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);
            }
            else if(GET_HDMI_MAC_TX0_AUDIO_CH_NUM() <= 16)
            {
                // Set Max Continue Pkt Size to 2
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT6));

                if(SCRIPT_TX_SKIP_AVMUTE_BYPASS() && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5000) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS))
                {
                    rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT3);
                }
                else
                {
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT0);
                }

                // DP Audio Data Format: 16 ch
                rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL2_reg, ~(_BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));
            }

            break;
#endif

#if(_HDMI_HBR_AUDIO_TX0_SUPPORT == _ON)
        case _HBR_ASP:

            // Audio Packet Header to HBR ASP
            rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_1_reg, 0x09);

            // Set Max Continue Pkt Size to 1
            rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~(_BIT6 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT6);

            if(SCRIPT_TX_SKIP_AVMUTE_BYPASS() && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() > 5000) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS))
            {
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
            }
            else
            {
                rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT0));
            }
            // DP Audio Data Format: 8 ch (Current DP Rx use 4 spdif to TX Mac for HBR Audio)
            //HBR in SPDIF 2ch mode
            rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL2_reg, ~(_BIT2 | _BIT1 | _BIT0), 0);

            // Audio Pkt: Fix mode & 4 sp# out of Aud SRAM
            rtd_maskl(HDMITX20_MAC0_HDMI_AUD_NEW_ADD0_reg, ~(_BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));

            //enable b frame//according spec HBR mode should set b frame 0
            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_0_reg, ~_BIT4, 0);

            rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_2_reg, ~(_BIT3 |_BIT2 | _BIT1 | _BIT0), 0);
            break;
#endif

        default:
            break;
    }
}

//--------------------------------------------------
// Description  : HDMI Tx Detect Hdcp rekey window change
// Input Value  : None
// Output Value : TRUE: Change; FALSE: No Change
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0DetectHdcpRekeyWindowChg(void)
{
    UINT8 ucReKeyWinSize = rtd_inl(HDMITX20_MAC0_SCHEDULER_7_reg) + 1;

    if(GET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE() != ucReKeyWinSize)
    {
        SET_HDMI_MAC_TX0_HDCP_REKEY_WIN_SIZE(ucReKeyWinSize);

        return _TRUE;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : HDMI Tx Audio Off
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0AudioPowerOff(void)
{
    // Set hdmi_tx_spdif_en = 0, No SPDIF Audio Format to HDMI Audio Packet
    rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_CTRL_reg, ~_BIT4, 0x00);
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, ~_BIT2, 0x00);
    rtd_maskl(HDMITX_MISC_HDMITX_AUDIO_BYPASS_reg,~(_BIT29 | _BIT28| _BIT4| _BIT0), 0x00);
    rtd_maskl(HDMITX_MISC_HDMITX_AUDIO_BYPASS_reg,~(_BIT25 | _BIT24| _BIT4| _BIT0),0x00);
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~_BIT2, 0x00);

    // Diable Audio FIFO
    CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();

    // Set Global to Initial
    SET_HDMI_MAC_TX0_AUDIO_FREQ(0);
    SET_HDMI_MAC_TX0_AUDIO_FORMAT(0);
    SET_HDMI_MAC_TX0_AUDIO_CH_NUM(0);
    CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();

    DebugMessageHDMITx("[HDMI_TX] Audio Reset\n");
}

//--------------------------------------------------
// Description  : HDMI Tx Calculate Audio Sample Number in One HDMI Audio Sample Packet
// Input Value  : None
// Output Value : None
//--------------------------------------------------

UINT8 ScalerHdmiMacTx0CalAudioSampleNuminOnePacket(void)
{
    UINT8 ucReKeyWinSize;// = 0;
    UINT16 usAudioSampleFreq = 480;
    UINT32 ulAudioSampleNumPerLine = 0;
    UINT16 usMaxPktCanTransmitPerLine = 0;
    UINT8 usACRFreq = 10; // Unit:0.1KHz
    UINT16 usACRfactor = 0; // Unit:0.01
    UINT16 usHactive= 0x00;
    UINT16 usHtotal = 0x00;
    UINT16 usVtotal= 0x00;
    UINT8 ucAudioSampleNuminOnePacket = 4;

    UINT8 ucAudioCHnum = GET_HDMI_MAC_TX0_AUDIO_CH_NUM();

    // Rekey reg + HW dummy cycle

    UINT8 bLpcmAudio = ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CODING_TYPE);
    ucReKeyWinSize = rtd_inl(HDMITX20_MAC0_SCHEDULER_7_reg) + 1;

    switch(ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE))
    {
        case _AUDIO_FREQ_32K:

            usAudioSampleFreq = 320;
            break;

        case _AUDIO_FREQ_44_1K:

            usAudioSampleFreq = 441;
            break;

        case _AUDIO_FREQ_48K:

            usAudioSampleFreq = 480;
            break;

        case _AUDIO_FREQ_64K:

            usAudioSampleFreq = 640;
            break;

        case _AUDIO_FREQ_88_2K:

            usAudioSampleFreq = 882;
            break;

        case _AUDIO_FREQ_96K:

            usAudioSampleFreq = 960;
            break;

        case _AUDIO_FREQ_128K:

            usAudioSampleFreq = 1280;
            break;

        case _AUDIO_FREQ_176_4K:

            usAudioSampleFreq = 1764;
            break;

        case _AUDIO_FREQ_192K:

            usAudioSampleFreq = 1920;
            break;

        case  _AUDIO_FREQ_256K:

            usAudioSampleFreq = 2560;
            break;

        case  _AUDIO_FREQ_352_8K:

            usAudioSampleFreq = 3528;
            break;

        case  _AUDIO_FREQ_384K:

            usAudioSampleFreq = 3840;
            break;

        case  _AUDIO_FREQ_512K:

            usAudioSampleFreq = 5120;
            break;

        case  _AUDIO_FREQ_705_6K:

            usAudioSampleFreq = 7056;
            break;

        case  _AUDIO_FREQ_768K:

            usAudioSampleFreq = 7680;
            break;

        default:
            usAudioSampleFreq = 0;

            break;
    }

    // Decide ACR factor equivalent to audio sample unit
    if(bLpcmAudio == _LPCM_ASP)
    {
        if((ucAudioCHnum > 12) && (ucAudioCHnum <= 16))
        {
            usACRfactor = 33;
        }
        else if((ucAudioCHnum > 8) && (ucAudioCHnum <= 12))
        {
            usACRfactor = 50;
        }
        else if((ucAudioCHnum > 2) && (ucAudioCHnum <= 8))
        {
            // 8ch LPCM
            usACRfactor = 100;
        }
        else
        {
            // 2ch LPCM
            usACRfactor = 400;
        }
    }
    else
    {
        // N-LPCM (case as 2ch LPCM)
        usACRfactor = 400;
    }

    // ---------------------------------------------------
    // Get Real Output Timing (unit: Tri-Byte)
    // ---------------------------------------------------
    usHtotal = (UINT16)g_stHdmiMacTx0InputTimingInfo->usHTotal * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8;
    usHactive = (UINT16)g_stHdmiMacTx0InputTimingInfo->usHWidth * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8;
    usVtotal = g_stHdmiMacTx0InputTimingInfo->usVTotal;

#ifdef SCALER_SUPPORT_420_CONFIG
    if((ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420) && (GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420))
#else
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
#endif
    {
        usHtotal = (usHtotal / 2);
        usHactive = (usHactive / 2);
    }

    if(GET_HDMI_MAC_TX0_INPUT_INTERLACE() == _TRUE)
    {
        if(ScalerDpStreamGetInterlaceVttEven(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE) // Even
        {
            usVtotal = (usVtotal * 2);
        }
        else
        {
            usVtotal = (usVtotal * 2 + 1);
        }
    }

    ulAudioSampleNumPerLine = ((((UINT16)(usAudioSampleFreq + (usACRFreq * usACRfactor / 100)) * 100) / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE()) / usVtotal + 1); // Unit: sample

    if(((rtd_inl(HDMITX20_MAC0_SCHEDULER_1_reg) & HDMITX20_MAC0_SCHEDULER_1_vactive_island_max_pkt_num_mask) == 0x00) ||
       ((ucAudioCHnum > 2) && (ucAudioCHnum <= 8) && (ulAudioSampleNumPerLine == 1)) || ((ucAudioCHnum <= 2) && (ulAudioSampleNumPerLine <= 4)))
    {
        // (1pkt per DI) or (only need 1pkt per line)
        if(((usHtotal - usHactive) * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1)) > (ucReKeyWinSize + 14))
        {
            // CTRL + Preamble + VGB = 4 + 8 + 2 = 14
            // 1Dummy + CTRL + Preamble + DI GB + Packet Body = 1 + 4 + 8 + 4 + 32 = 49
            usMaxPktCanTransmitPerLine = ((usHtotal - usHactive) * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1) - ucReKeyWinSize - 14) / 49;
        }
        else
        {
            usMaxPktCanTransmitPerLine = 0;
        }
    }
    else
    {
        if(((usHtotal - usHactive) * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1)) > (ucReKeyWinSize + 14 + 13 + 16))
        {
            // CTRL + Preamble + DI Leading GB = 4 + 8 + 2 = 14
            // DI Trailing GB + CTRL + Preamble + VGB = 2 + 4 + 8 + 2 = 16
            // 13T Dummy only appear when a DI has more than 1 packet.
            usMaxPktCanTransmitPerLine = ((usHtotal - usHactive) * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1) - ucReKeyWinSize - 14 - 16 - 13) / 32;
        }
        else
        {
            usMaxPktCanTransmitPerLine = 0;
        }
    }

    // Check usMaxPktCanTransmitPerLine = 0?
    if((usMaxPktCanTransmitPerLine == 0) && (ulAudioSampleNumPerLine > 0))
    {
        SET_HDMI_MAC_TX0_AUDIO_BW_ERROR();
        DebugMessageHDMITx("HDMI_TX0: Audio BW Error, usMaxPktCanTransmitPerLine = %d\n", (UINT32)usMaxPktCanTransmitPerLine);

        return ucAudioSampleNuminOnePacket;
    }
#if(_HDMI_3D_AUDIO_TX0_SUPPORT == _ON)
    // Set sp# & Audio BW re-check
    if(GET_HDMI_MAC_TX0_AUDIO_FORMAT() == _3D_LPCM_ASP)
    {
        if((ucAudioCHnum > 12) && (ucAudioCHnum <= 16))
        {
            ucAudioSampleNuminOnePacket = 4;

            if(ulAudioSampleNumPerLine > (usMaxPktCanTransmitPerLine / 3))
            {
                // Audio Error
                SET_HDMI_MAC_TX0_AUDIO_BW_ERROR();
                        FatalMessageHDMITx("HDMI_TX0: Audio BW ERROR!!!, 13~16ch\n");
            }
            else
            {
                CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();
            }
        }
        else if((ucAudioCHnum > 8) && (ucAudioCHnum <= 12))
        {
            ucAudioSampleNuminOnePacket = 4;

            if(ulAudioSampleNumPerLine > (usMaxPktCanTransmitPerLine / 2))
            {
                // Audio Error
                SET_HDMI_MAC_TX0_AUDIO_BW_ERROR();
                DebugMessageHDMITx("[HDMI_TX] Audio BW ERROR!!!, 9~12ch\n");
            }
            else
            {
                CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();
            }
        }
    }
    else
#endif
    {
        if((bLpcmAudio == _LPCM_ASP) && (ucAudioCHnum > 2) && (ucAudioCHnum <= 8))
        {
            // LPCM 8ch layout
            ucAudioSampleNuminOnePacket = 4;

            if(ulAudioSampleNumPerLine > usMaxPktCanTransmitPerLine)
            {
                // Audio Error
                SET_HDMI_MAC_TX0_AUDIO_BW_ERROR();
                FatalMessageHDMITx("[HDMI_TX] Audio BW ERROR!!!, 8ch\n");
            }
            else
            {
                ucAudioSampleNuminOnePacket = 4;

                CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();
            }
        }
        else
        {
            // N-LPCM or LPCM 2ch layout
            if(usMaxPktCanTransmitPerLine != 0)
            {
                if(ulAudioSampleNumPerLine % usMaxPktCanTransmitPerLine != 0)
                {
                    ucAudioSampleNuminOnePacket = (ulAudioSampleNumPerLine / usMaxPktCanTransmitPerLine) + 1;
                }
                else
                {
                    ucAudioSampleNuminOnePacket = ulAudioSampleNumPerLine / usMaxPktCanTransmitPerLine;
                }
            }
            else
            {
                // if no Packet can be transmit --> sp_pst of an ASP is Unlimited!! (set 4 sp in an ASP)
                ucAudioSampleNuminOnePacket = 4;
            }

            if(ucAudioSampleNuminOnePacket > 4)
            {
                ucAudioSampleNuminOnePacket = 4;

                // Audio Error
                SET_HDMI_MAC_TX0_AUDIO_BW_ERROR();
                DebugMessageHDMITx("[HDMI_TX] Audio BW ERROR!!!, 2ch\n");
            }
            else
            {
                CLR_HDMI_MAC_TX0_AUDIO_BW_ERROR();
            }
        }
    }

    FatalMessageHDMITx("[HDMI_TX] AudSp NumPerLine=%d\n", (UINT32)ulAudioSampleNumPerLine);
    FatalMessageHDMITx("[HDMI_TX] AudSp ForActual=%d\n", (UINT32)usMaxPktCanTransmitPerLine);
    FatalMessageHDMITx("[HDMI_TX] AudSp OnePacket=%d\n", (UINT32)ucAudioSampleNuminOnePacket);

    return ucAudioSampleNuminOnePacket;
}

#if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Audio NCTS Timer Event Set
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0CtsCodeCheckEventSet(void)
{
    SET_HDMI_MAC_TX0_CTS_CODE_CHECK_EVENT();
}

//--------------------------------------------------
// Description  : Hdmi Tx Audio NCTS Timer Event Period
// Input Value  :
// Output Value : Timer Event Period (ms)
//--------------------------------------------------
UINT16 ScalerHdmiMacTx0CtsCodeCheckEventPeriod(void)
{
    return ((UINT16)ScalerDpStreamGetOneFrameDelay((ScalerHdmiMacTxPxMapping(_TX0))) * _HDMI_MAC_TX0_CTS_TIMER_EVENT_FRAME_CNT);
}

//--------------------------------------------------
// Description  : Hdmi Tx Audio NCTS FW/HW Mode Change
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0CtsCodeHandler(void)
{
    // event occured and video active region
    /*
    if((GET_HDMI_MAC_TX0_CTS_CODE_CHECK_EVENT() == _TRUE) && (ScalerDpStreamAudioVerticalBlanking(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE))
    {
        CLR_HDMI_MAC_TX0_CTS_CODE_CHECK_EVENT();

        ScalerTimerActiveTimerEvent(ScalerHdmiMacTx0CtsCodeCheckEventPeriod(), _SCALER_TIMER_EVENT_HDMI_TX0_AUDIO_CTS_CODE_CHECK);
    }
    else
    {
        return;
    }
*/
    // check Apll FW tracking stable
    // for TV it should always be FW mode
    if(ScalerDpStreamGetAudioFwTrackingStableStatus(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE)
    {
        // Unstable: HW -> FW mode
        if((rtd_getbits(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, _BIT3) == 0x00) && (g_ulHdmiMacTx0FwCTSVaule != 0x00))
        {
            DebugMessageHDMITx("[HDMI Tx0 CTS] Unstable, use FW CTS\n");

            // Set FW NCTS
            SET_HDMI_MAC_TX0_CTS_CODE_FW_MODE();

            ScalerHdmiMacTx0ACRPacket(FALSE);
        }

    }
    else
    {
        // Stable: FW -> HW mode
        if(rtd_getbits(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_5_reg, _BIT3) == _BIT3)
        {
            DebugMessageHDMITx("[HDMI Tx0 CTS] Stable, use HW CTS\n");

            ScalerHdmiMacTx0UpdateFixedCTS();

            // Set HW NCTS
            CLR_HDMI_MAC_TX0_CTS_CODE_FW_MODE();

            ScalerHdmiMacTx0ACRPacket(FALSE);
        }
    }
}

//--------------------------------------------------
// Description  : HDMI Tx update FW CTS code
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0UpdateFixedCTS(void)
{
    UINT8 pucRegRead[3] = {0x00, 0x00, 0x00};

    pucRegRead[0]=rtd_inl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_11_reg);
    pucRegRead[1]=rtd_inl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_12_reg);
    pucRegRead[2]=rtd_inl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_13_reg);
    g_ulHdmiMacTx0FwCTSVaule = ((UINT32)(pucRegRead[2] & 0xF0) << 12) | ((UINT16)pucRegRead[1] << 8) | (pucRegRead[0]);

    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_10_reg, (UINT8)(g_ulHdmiMacTx0FwCTSVaule >> 12)); // [19:12] -> [7:0]
    rtd_outl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_9_reg, (UINT8)(g_ulHdmiMacTx0FwCTSVaule >> 4)); // [11:4] -> [7:0]
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_8_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((UINT8)(g_ulHdmiMacTx0FwCTSVaule) & 0x0F)); // [3:0]

    // Reset Audio FIFO
    CLR_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();
    SET_HDMI_MAC_TX0_AUDIO_FIFO_ENABLE();

    HDMITX_MDELAY(10);

    rtd_maskl(HDMITX20_MAC1_HDMI_AUDIO_SRAM_0_reg, ~(_BIT5 | _BIT4), _BIT4);
}
#endif // #if(_HDMI_AUDIO_FW_TRACKING_SUPPORT == _ON)

#if(_HDMI_3D_AUDIO_TX0_SUPPORT == _ON)
//--------------------------------------------------
// Description  : HDMI Tx Set Audio Metadata Pkt
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiMacTx0AudioMetadataPacket(void)
{
// TODO: 3D Audio Issue
}

#endif // #if(_HDMI_3D_AUDIO_TX0_SUPPORT == _ON)
#endif // #if(_AUDIO_SUPPORT == _ON)
#endif





//--------------------------------------------------
// Description  : Hdmi Tx Color Depth Decision
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0ColorDepthDecision(void)
{
    UINT16 ustempTMDSClk = 0x00;
    UINT16 usMaxTMDSClk = 0x00; // Unit:0.1MHz

    // YCbCr 422 only have 12bit mode, but to output 8 BOOLEAN mode
    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR422)
    {
        SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
    }

    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
    {
        if((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == 12) && (ScalerHdmiMacTx0EdidGetFeature(_DC36_420_SUPPORT) == _FALSE))
        {
            if(ScalerHdmiMacTx0EdidGetFeature(_DC30_420_SUPPORT) == _TRUE)
            {
                SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_10_BITS);
            }
            else
            {
                SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
            }

            DebugMessageHDMITx("[HDMI_TX] Reduce 420 Color Depth 12-> %d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());
        }
        else if((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == 10) && (ScalerHdmiMacTx0EdidGetFeature(_DC30_420_SUPPORT) == _FALSE))
        {
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);

            DebugMessageHDMITx("[HDMI_TX] Reduce 420 Color Depth 10-> %d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());
        }
    }
    else
    {
        if((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == 12) && (ScalerHdmiMacTx0EdidGetFeature(_DC36_SUPPORT) == _FALSE))
        {
            if(ScalerHdmiMacTx0EdidGetFeature(_DC30_SUPPORT) == _TRUE)
            {
                SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_10_BITS);
            }
            else
            {
                SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
            }

            DebugMessageHDMITx("[HDMI_TX] Reduce Color Depth 12-> %d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());
        }
        else if((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == 10) && (ScalerHdmiMacTx0EdidGetFeature(_DC30_SUPPORT) == _FALSE))
        {
            SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);

            DebugMessageHDMITx("[HDMI_TX] Reduce Color Depth 10-> %d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());
        }
    }

    ustempTMDSClk = (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8);

    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
    {
        ustempTMDSClk = ustempTMDSClk / 2;
    }

    usMaxTMDSClk = ((UINT16)ScalerHdmiMacTx0EdidGetFeature(_HDMI_MAX_TMDS_CLK) * 50);

    DebugMessageHDMITx("[HDMI_TX] ustempTMDSClk=%d\n", (UINT32)ustempTMDSClk);
    DebugMessageHDMITx("[HDMI_TX] usMaxTMDSClk=%d\n", (UINT32)usMaxTMDSClk);

    if((GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() > 8) && (ustempTMDSClk > usMaxTMDSClk) && (ScalerHdmiMacTx0EdidGetFeature(_HDMI_MAX_TMDS_CLK) != 0))
    {
        DebugMessageHDMITx("[HDMI_TX] MAX TMDS clk not enough@Depth=%d\n", (UINT32)GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH());

        SET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH(_DP_STREAM_COLOR_DEPTH_8_BITS);
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx Pixel Repetition Decision
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0RepetitionDecision(void)
{
#if(_AUDIO_SUPPORT == _ON)

    UINT16 usAudioSampleFreq = 480;
    UINT8 ucAudioCHnum = 2;
    UINT16 usMaxPacketSizeInIdle = 0;
    UINT32 ulAudioSampleNumPerLine = 1;

    UINT8 ucReKeyWinSize = 0;
    UINT16 usHactive= 0x00;
    UINT16 usHtotal = 0x00;
    UINT16 usVtotal= 0x00;
    UINT32 ulAudioSamplePacketNumPerLineForActual = 0;
    UINT32 ulAudioSampleNumPerLineForActual = 0;

    UINT16 usTemp = 0;
    UINT8 ucN = 0;
#endif

    if(((GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() < 166) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_12_BITS)) ||
       ((GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() < 200) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_10_BITS)) ||
       ((GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() < 250) && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS)))   // (Unit in 0.1MHz)
    {
        SET_HDMI_MAC_TX0_INPUT_REPEAT_NUM((250 / GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK()));
    }
    else
    {
        SET_HDMI_MAC_TX0_INPUT_REPEAT_NUM(0);
    }

#if(_AUDIO_SUPPORT == _ON)

    //ucReKeyWinSize = rtd_inl(HDMITX20_MAC0_SCHEDULER_7_reg) + 1;

    if(GET_HDMI_MAC_TX0_SUPPORT() == _TRUE)
    {
        // when Audio Exist and AV_Mute Flag=0
        // And when color Depth = 8 and Pixel Clk <= 300MHz
        // Decide if HDMI Tx Need Repetition For Audio Bandwidth
        if((ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE) != _AUDIO_FREQ_NO_AUDIO)
                && (GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() == _DP_STREAM_COLOR_DEPTH_8_BITS)
                && (ScalerStreamGetAudioMuteFlag(ScalerHdmiMacTxPxMapping(_TX0)) == _FALSE)
                && (GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() <= 3000))
        {
            UINT8 ucAudioChfromDpRx = ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_CHANNEL_COUNT); // ch num = ucAudioChfromDpRx + 1

            ucN = GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM();

            if((ucAudioChfromDpRx < 16) && (ucAudioChfromDpRx >= 12))
            {
                ucAudioCHnum = 16;
            }
            else if((ucAudioChfromDpRx < 12) && (ucAudioChfromDpRx >= 8))
            {
                ucAudioCHnum = 12;
            }
            else if((ucAudioChfromDpRx < 8) && (ucAudioChfromDpRx >= 2))
            {
                ucAudioCHnum = 8;
            }
            else
            {
                ucAudioCHnum = 2;
            }

            switch(ScalerHdmiMacTxGetAudioFormat(ScalerHdmiMacTxPxMapping(_TX0), _AUDIO_SAMPLING_FREQUENCY_TYPE))
            {
                case _AUDIO_FREQ_32K:

                    usAudioSampleFreq = 320;
                    break;

                case  _AUDIO_FREQ_44_1K:

                    usAudioSampleFreq = 441;
                    break;

                case  _AUDIO_FREQ_48K:

                    usAudioSampleFreq = 480;
                    break;

                case  _AUDIO_FREQ_64K:

                    usAudioSampleFreq = 640;
                    break;

                case  _AUDIO_FREQ_88_2K:

                    usAudioSampleFreq = 882;
                    break;

                case  _AUDIO_FREQ_96K:

                    usAudioSampleFreq = 960;
                    break;

                case _AUDIO_FREQ_128K:

                    usAudioSampleFreq = 1280;
                    break;

                case  _AUDIO_FREQ_176_4K:

                    usAudioSampleFreq = 1764;
                    break;

                case  _AUDIO_FREQ_192K:

                    usAudioSampleFreq = 1920;
                    break;

                case  _AUDIO_FREQ_256K:

                    usAudioSampleFreq = 2560;
                    break;

                case  _AUDIO_FREQ_352_8K:

                    usAudioSampleFreq = 3528;
                    break;

                case  _AUDIO_FREQ_384K:

                    usAudioSampleFreq = 3840;
                    break;

                case  _AUDIO_FREQ_512K:

                    usAudioSampleFreq = 5120;
                    break;

                case  _AUDIO_FREQ_705_6K:

                    usAudioSampleFreq = 7056;
                    break;

                case  _AUDIO_FREQ_768K:

                    usAudioSampleFreq = 7680;
                    break;

                default:
                    usAudioSampleFreq = 0;
                    break;
            }

            // Modity Timing Info due to DC/420/interlace effect
            usHtotal = g_stHdmiMacTx0InputTimingInfo->usHTotal * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8;
            usHactive = g_stHdmiMacTx0InputTimingInfo->usHWidth * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8;
            usVtotal = g_stHdmiMacTx0InputTimingInfo->usVTotal;

#ifdef SCALER_SUPPORT_420_CONFIG
            if((ScalerDpStreamGetElement(ScalerHdmiMacTxPxMapping(_TX0), _COLOR_SPACE) != _COLOR_SPACE_YCBCR420) && (GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420))
#else
            if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
#endif
            {
                usHtotal = (usHtotal / 2);
                usHactive = (usHactive / 2);
            }

            if(GET_HDMI_MAC_TX0_INPUT_INTERLACE() == _TRUE)
            {
                if(ScalerDpStreamGetInterlaceVttEven(ScalerHdmiMacTxPxMapping(_TX0)) == _TRUE)
                {
                    usVtotal = (usVtotal * 2);
                }
                else
                {
                    usVtotal = (usVtotal * 2 + 1);
                }
            }

            usTemp = 100;

            while((ulAudioSampleNumPerLine > ulAudioSampleNumPerLineForActual) && (--usTemp != 0))
            {
                ucN = (ucN + 1); // ucN = Pixel Repetition + 1
                usMaxPacketSizeInIdle = (((usHtotal - usHactive) * ucN - 16 - 8 - 2) / 32); // K value
                ulAudioSampleNumPerLine = ((((UINT32)(usAudioSampleFreq) * 100) / GET_HDMI_MAC_TX0_INPUT_FRAME_RATE()) / usVtotal + 1); // per sample
                ucReKeyWinSize = rtd_inl(HDMITX20_MAC0_SCHEDULER_7_reg) + 1;
                ulAudioSamplePacketNumPerLineForActual = (((usHtotal - usHactive) * ucN - ucReKeyWinSize - 14 - (usMaxPacketSizeInIdle * 2)) / 48); // per pkt
                // TODO: PKT Continue Issue
                /*
                ulAudioSamplePacketNumPerLineForActual = ((((usHtotal - usHactive) * ucN - ucReKeyWinSize - 14 - (usMaxPacketSizeInIdle * 2)) / (16 + 32 * ucPktCountNum)) * ucPktCountNum)
                                                         + (((usHtotal - usHactive) * ucN - ucReKeyWinSize - 14 - (usMaxPacketSizeInIdle * 2)) % (16 + 32ucI)) / 48; // per pkt
                */

                if((ucAudioCHnum > 8) && (ucAudioCHnum <= 16))
                {
                    ulAudioSampleNumPerLineForActual = (ulAudioSamplePacketNumPerLineForActual / 2);
                }
                else if(ucAudioCHnum == 8)
                {
                    ulAudioSampleNumPerLineForActual = ulAudioSamplePacketNumPerLineForActual;
                }
                else if(ucAudioCHnum == 2)
                {
                    ulAudioSampleNumPerLineForActual = (ulAudioSamplePacketNumPerLineForActual * 4);
                }
            }

            if(lib_hdmitx_is_hdmi_bypass_support() == _TRUE) {
               FatalMessageHDMITx("[HDMI_TX] Audio Decide Repeat ucN=%d\n", (UINT32)ucN);
            } else {
              SET_HDMI_MAC_TX0_INPUT_REPEAT_NUM((ucN - 1));
            }
        }
    }
#endif // End of #if((_AUDIO_SUPPORT == _ON) && (_HDMI_TX_CTS_TEST == _OFF))
}

//--------------------------------------------------
// Description  : Hdmi Tx Deep Color Clk Calculate
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0DeepColorClkCal(void)
{
    SET_HDMI_MAC_TX0_DEEP_COLOR_CLK(((UINT32)GET_HDMI_MAC_TX0_INPUT_PIXEL_CLK() * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1) / 8));  // (Unit in 0.1MHz)

    if(GET_HDMI_MAC_TX0_INPUT_COLOR_SPACE() == _COLOR_SPACE_YCBCR420)
    {
        SET_HDMI_MAC_TX0_DEEP_COLOR_CLK(GET_HDMI_MAC_TX0_DEEP_COLOR_CLK() / 2);
    }

    DebugMessageHDMITx("[HDMI_TX] Deep Color Clk=%d\n", (UINT32)GET_HDMI_MAC_TX0_DEEP_COLOR_CLK());
}


//--------------------------------------------------
// Description  : Switch Power Group State of HDMI Tx
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0PowerGroupPSMode(BOOLEAN bOn)
{
    if(bOn == _ON)  // Power down
    {
        ScalerPowerGroupHDMITxIsolation(_TX0, _ON);
        ScalerPowerGroupHDMITxPowerCut(_TX0, _ON);
    }
    else    // Power On
    {

        // Group VDAC Power Cut block Reset
        ScalerPowerGroupHDMITxPowerCutReset(_TX0, _ON);

        ScalerPowerGroupHDMITxPowerCut(_TX0, _OFF);
        ScalerPowerGroupHDMITxIsolation(_TX0, _OFF);

        ScalerPowerGroupHDMITxPowerCutReset(_TX0, _OFF);
    }
}



#if(_HDMI_MAC_TX0_PURE == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Set Clk lane(CH3) transmit by data format
// Input Value  : _ENABLE or _DISABLE
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SetClkDataTransmit(BOOLEAN bEnable)
{
    if(bEnable == _ENABLE)
    {
        rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT2, _BIT2);
    }
    else
    {
        rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT2, 0x00);
    }
}

//--------------------------------------------------
// Description  : Hdmi Tx Set Clk lane div4
// Input Value  : _ENABLE or _DISABLE
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SetClkDiv4(BOOLEAN bEnable)
{
    if(bEnable == _ENABLE)
    {
        rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT1, _BIT1);
    }
    else
    {
        rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT1, 0x00);
    }
}
#endif

#ifdef NOT_USED_NOW
#if(_HDMI_MAC_TX0_PURE == _ON)
//--------------------------------------------------
// Description  : Hdmi Tx Set Clk lane(CH3) transmit by data format
// Input Value  : _ENABLE or _DISABLE
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SetClkDataTransmit(BOOLEAN bEnable)
{
    if(bEnable == _ENABLE)
    {
        rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT2, _BIT2);
    }
    else
    {
        rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL4_reg, ~_BIT2, 0x00);
    }
}
#endif
//--------------------------------------------------
// Description  : Hdmi Tx Set Small FIFO -> Double Rate
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SetSmallFIFODoubleRate(void)
{
    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, ~(_BIT6 | _BIT5), _BIT6);
}

//--------------------------------------------------
// Description  : Hdmi Tx Set Small FIFO -> Full Rate
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SetSmallFIFOFullRate(void)
{
    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, ~(_BIT6 | _BIT5), _BIT5);
}

//--------------------------------------------------
// Description  : Hdmi Tx Set Small FIFO -> Half Rate
// Input Value  :
// Output Value :
//--------------------------------------------------
void ScalerHdmiMacTx0SetSmallFIFOHalfRate(void)
{
    rtd_maskl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, ~(_BIT6 | _BIT5), 0x00);
}

//--------------------------------------------------
// Description  : GET Hdcp Enc Status
// Input Value  :
// Output Value : _TRUE: ENC_EN
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0GetHdcpEncStatus(void)
{
    {
        return (rtd_getbits(HDMITX20_MAC0_SCHEDULER_2_reg, _BIT7) == _BIT7);
    }
}

#endif // #ifdef NOT_USED_NOW

