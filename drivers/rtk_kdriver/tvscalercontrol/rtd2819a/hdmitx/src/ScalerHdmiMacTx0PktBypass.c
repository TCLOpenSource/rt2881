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

//#include <rbus/ppoverlay_reg.h>
//#include <rbus/ppoverlay_outtg_reg.h>
//#include <rbus/scaleup_reg.h>
//#include <rbus/vgip_reg.h>
//#include <rbus/hdmitx_misc_reg.h>
//#include <rbus/timer_reg.h>

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> 
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_init.h>
//#include <rtk_kdriver/measure/measure.h>
#include "../include/ScalerTx.h"
#include "../include/HdmiEdid/hdmitx_edid.h"
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
#include <rtk_kdriver/tvscalercontrol/hdmirx/hdmifun.h>
#endif

//****************************************************************************
// Macro/Define
//****************************************************************************
#define HDMI20_TX_MUTE_BYPASS_CTRL_MASK (HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_sel_in_mask|HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_sel_in_mask|HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_bypass_en_mask|HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_bypass_en_mask)
#define HDMI21_TX_MUTE_BYPASS_CTRL_MASK (HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_sel_in_mask|HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_sel_in_mask|HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_bypass_en_mask|HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_bypass_en_mask)

//****************************************************************************
// CODE TABLES
//****************************************************************************



//****************************************************************************
// local variable declaration
//****************************************************************************
//#include <rbus/dsce_misc_reg.h>
//#include <rbus/ppoverlay_reg.h>


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

//#include <scaler/scalerstruct.h>
//#include <scaler/scalerdrv.h>
#include <rbus/hdmi21_p0_reg.h>
#include <rbus/hdmi_p0_reg.h>


//****************************************************************************
// Macro/Define
//****************************************************************************
#define WAIT_TIMEOUT (90*100)


//****************************************************************************
// Local/Static VARIABLE DECLARATIONS
//****************************************************************************
// EMP packet status


static UINT8 empPktBypassEnableMask=0x1; // HDMITX_MISC_HD_EMP_CTL_EN_reg
static UINT8 empPkt_usr_oid1=0x2, empPkt_usr_oid0=0x3; // 0: vendor, 1: HDMI Forum, 2: CTA-861, 3: VESA, 4-255: reserved
#define EMPPKTOID0_OUI0 (0x0046d000UL)   // OUI: Dolby Vision HDR
#define EMPPKTOID0_OUI1 (0x008b8490UL)   // OUI: HDR10p (SSTM)
#define EMPPKTOID0_OUI2 (0x0046d000UL)   // OUI: Dolby Vision HDR

#define VSIF_OUI_HF           (0xC45DD8UL)    // OUI: HF
#define VSIF_OUI_H14B      (0x000C03UL)    // OUI: H14B
#define VSIF_OUI_HDR10P (0x90848BUL)    // OUI: HDR10P
#define VSIF_OUI_DOLBY   (0x00D046UL)    // OUI: Dolby
#define VSIF_OUI_CUVA      (0x047503UL)   // OUI: CUVA

/************************
// Extern variable declaration
//
*************************/
extern UINT8 AudioFreqType;

extern UINT32  hdmiRepeaterBypassPktHdrEnMask;
extern StructHDMITxAudioInfo g_stHdmiMacTx0AudioInfo;
extern UINT16 lineCnt, start, end;
extern UINT32 lineCntAddr;
extern UINT32 stc;

extern StructHdmitxDisplayInfo g_scaler_display_info;
/************************
// Extern Function declaration
//
*************************/
extern UINT8 need_sw_avi_packet;
/************************
// Function implementation
//
*************************/

// [ECN] TX EMP header OUI init
void ScalerTxHdmiTx0EmpHdOuiInit(void)
{
    //[ECN] TX EMP header OUI init
    if(lib_hdmitx_is_HDR10_PLUS_Function_support()) {
        rtd_outl(HDMITX_MISC_HD_EMP_OUI1_reg, EMPPKTOID0_OUI1);
    } else {
        rtd_outl(HDMITX_MISC_HD_EMP_OUI1_reg, 0x0);
    }

    rtd_outl(HDMITX_MISC_HD_EMP_OUI0_reg, EMPPKTOID0_OUI0);
    rtd_outl(HDMITX_MISC_HD_EMP_OUI2_reg, EMPPKTOID0_OUI2);

    return;
}


void ScalerHdmiMacTx0RepeaterSetPktHdrBypass_EmpPacketBypass(UINT8 enable)
{
    UINT32 value_u32;
    if(enable){
        //B[0]: hd_emp_all_en
        if(empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en_mask){
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en_mask);
        }else{
            // EMP packet bypass config
            //HD_EMP_CTL_EN 0x18019744
            //B[0]: hd_emp_all_en
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en_mask, 0);

            //B[7]: hd_emp_usr_oid1_en
            //HD_EMP_USR_OID 0x18019740 B[15:8] hd_emp_usr_oid1
            if(empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid1_en_mask){
                value_u32 = (UINT32)HDMITX_MISC_HD_EMP_USR_OID_hd_emp_usr_oid1(empPkt_usr_oid1);
                rtd_maskl(HDMITX_MISC_HD_EMP_USR_OID_reg, ~HDMITX_MISC_HD_EMP_USR_OID_hd_emp_usr_oid1_mask, value_u32);
            }
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid1_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid1_en_mask));

            //B[6]: hd_emp_usr_oid0_en
            //HD_EMP_USR_OID 0x18019740 B[ 7:0] hd_emp_usr_oid0
            if(empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en_mask){
                value_u32 = (UINT32)HDMITX_MISC_HD_EMP_USR_OID_hd_emp_usr_oid0(empPkt_usr_oid0);
                rtd_maskl(HDMITX_MISC_HD_EMP_USR_OID_reg, ~HDMITX_MISC_HD_EMP_USR_OID_hd_emp_usr_oid0_mask, value_u32);
            }
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en_mask));

            //B[5]: hd_emp_vsemds2_en
            //HD_EMP_OUI2	0x1801973c B[23:16]: hd_emp_oui2_3rd, B[15: 8]: hd_emp_oui2_2nd, B[ 7: 0]: hd_emp_oui2_1st
            //B[23:16]: OUI Byte[0], B[15:8]: OUI Byte[1], B[7:0]: OUI Byte[2]
            if(empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en_mask){
                value_u32 = (UINT32)(EMPPKTOID0_OUI2 & 0xffffff);
                rtd_outl(HDMITX_MISC_HD_EMP_OUI2_reg, value_u32);
            }
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en_mask));

            //B[4]: hd_emp_vsemds1_en
            //HD_EMP_OUI1	0x18019738 B[23:16]: hd_emp_oui1_3rd, B[15: 8]: hd_emp_oui1_2nd, B[ 7: 0]: hd_emp_oui1_1st
            //B[23:16]: OUI Byte[0], B[15:8]: OUI Byte[1], B[7:0]: OUI Byte[2]
            if(empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en_mask){
                value_u32 = (UINT32)(EMPPKTOID0_OUI1 & 0xffffff);
                rtd_outl(HDMITX_MISC_HD_EMP_OUI1_reg, value_u32);
            }
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en_mask));

            //B[3]: hd_emp_vsemds0_en
            //HD_EMP_OUI0	0x18019734 B[23:16]: hd_emp_oui0_3rd, B[15: 8]: hd_emp_oui0_2nd, B[ 7: 0]: hd_emp_oui0_1st
            //B[23:16]: OUI Byte[0], B[15:8]: OUI Byte[1], B[7:0]: OUI Byte[2]
            if(empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en_mask){
                value_u32 = (UINT32)(EMPPKTOID0_OUI0 & 0xffffff);
                rtd_outl(HDMITX_MISC_HD_EMP_OUI0_reg, value_u32);
            }
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en_mask));

            //B[2]: hd_emp_sbtm_en
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_sbtm_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_sbtm_en_mask));

            //B[1]: hd_emp_vtem_en
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vtem_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vtem_en_mask));
            rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_cvtem_en_mask, (empPktBypassEnableMask & HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_cvtem_en_mask));
        }

    }else{
        //rtd_outl(0xb8019744, 0x00000001);   // HDMITX_MISC_HD_EMP_CTL_EN_reg
        rtd_outl(HDMITX_MISC_HD_EMP_CTL_EN_reg, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid1_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_sbtm_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_sbtm_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vtem_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vtem_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_cvtem_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_cvtem_en(0));
        rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en(0));   // HDMITX_MISC_HD_EMP_CTL_EN_reg
    }
    return;
}
void ScalerHdmiTxNCtsRegenFifoCheckSetting(void)
{
    UINT32 audio_clk_ini,audio_clk_max,audio_clk_min,rbus_clk_init;
    UINT16 usAudioSampleFreq = 480; // Unit:0.1KHz

    switch(GET_HDMI_MAC_TX0_AUDIO_FREQ()){
        case _AUDIO_FREQ_32K:
            usAudioSampleFreq = 320;
            break;
        case _AUDIO_FREQ_64K:
            usAudioSampleFreq = 640;
            break;
        case _AUDIO_FREQ_128K:
            usAudioSampleFreq = 1280;
            break;
        case _AUDIO_FREQ_44_1K:
            usAudioSampleFreq = 441;
            break;
        case _AUDIO_FREQ_88_2K:
            usAudioSampleFreq = 882;
            break;
        case _AUDIO_FREQ_176_4K:
            usAudioSampleFreq = 1764;
            break;
        case _AUDIO_FREQ_48K:
            usAudioSampleFreq = 480;
            break;
        case _AUDIO_FREQ_96K:
            usAudioSampleFreq = 960;
            break;
        case _AUDIO_FREQ_192K:
            usAudioSampleFreq = 1920;
            break;
        case _AUDIO_FREQ_256K:
            usAudioSampleFreq = 2560;
            break;
         case _AUDIO_FREQ_352_8K:
            usAudioSampleFreq = 3528;
            break;
        case _AUDIO_FREQ_384K:
            usAudioSampleFreq = 3840;
            break;
        case _AUDIO_FREQ_512K:
            usAudioSampleFreq = 5120;
            break;
        case _AUDIO_FREQ_705_6K:
            usAudioSampleFreq = 7056;
            break;
        case _AUDIO_FREQ_768K:
            usAudioSampleFreq = 7680;
            break;
    }
    audio_clk_ini = AUDIO_CLK_17TH_POWER_OF_2 * 250;
    audio_clk_ini = audio_clk_ini / RBUS_CLK_250M;
    audio_clk_ini = audio_clk_ini * usAudioSampleFreq;

    rbus_clk_init = RBUS_CLK_250M;
    rbus_clk_init = rbus_clk_init * 100;

    audio_clk_max = audio_clk_ini + AUDIO_CLK_FIFO_RANGE;
    audio_clk_min = audio_clk_ini - AUDIO_CLK_FIFO_RANGE;

    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_5_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_5_audio_clk_init_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_5_audio_clk_init(audio_clk_ini));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_4_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_4_rbus_clk_init_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_4_rbus_clk_init(rbus_clk_init));

    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_B_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_B_audio_clk_max_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_B_audio_clk_max(audio_clk_max));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_C_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_C_audio_clk_min_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_C_audio_clk_min(audio_clk_min));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_fifo_check_period_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_fifo_check_period(0x7));

}

void ScalerHdmiMacTx0NCtsRegenConfig(void)
{

    UINT32 ulHDMITxACRNcode = 0;
    UINT16 usAudioSampleFreq = 480; // Unit:0.1KHz
    UINT8 value_u8;

    SET_HDMI_MAC_TX0_AUDIO_FREQ(AudioFreqType);

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
    FatalMessageHDMITx("[HDMITX] AudioBypassEn=%d,", (UINT32)!SCRIPT_TX_SKIP_AUDIO_BYPASS());
    FatalMessageHDMITx("Type[%d]/,", (UINT32)AudioFreqType);
    FatalMessageHDMITx("Sample=%d,", (UINT32)usAudioSampleFreq);
    FatalMessageHDMITx("Ncode=%d\n", (UINT32)ulHDMITxACRNcode);

        //rtd_outl(0xb801952c, 0x05000001); // HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg
    rtd_outl(HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg, HDMITX_MISC_HDMITX_NCTS_REGEN_3_tx_n_value(ulHDMITxACRNcode));
    value_u8 = (SCRIPT_TX_SKIP_AUDIO_BYPASS()? 0: 1);
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg, ~ HDMITX_MISC_HDMITX_NCTS_REGEN_3_ncts_dbg_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_3_ncts_dbg_en(value_u8));

        // [V] HDMITX MISC NCTS Regen
    //rtd_outl(0xb8019524, 0x00ff0820); // HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_1_fifo_threshold_hi_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_1_fifo_threshold_hi(0x820));

    //rtd_outl(0xb8019528, 0x080007e0); // HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_mid_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_mid(0x800));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_low_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_low(0x7e0));

    //rtd_outl(0xb8019520, 0x71000fa0); // HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg
    value_u8 = (SCRIPT_TX_SKIP_AUDIO_BYPASS()? 0: 1);
    rtd_outl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_mux(value_u8)); // N/CTS mux: 0x0 : N/CTS from spdif. Original path, 0x1 : N/CTS from regen. The new method.
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_rbus_period_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_rbus_period(0xfa));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_measure_period_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_measure_period(0x7));
#ifdef CONFIG_FORCE_AUDIO_PACKET_BYPASS 
    if(lib_hdmitx_is_hdmi_bypass_support() == _TRUE)
        value_u8 = 0;// always ncts bypass rx
    else
#endif
        value_u8 = (SCRIPT_TX_SKIP_AUDIO_BYPASS()? 0: 1);
    if(value_u8){
        ScalerHdmiTxNCtsRegenFifoCheckSetting();
    }
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en(value_u8));

    return;
}

void ScalerHdmiTxBypassAVMuteCtrl(UINT8 enable)
{
    //CLR_AVMUTE always use sw mode for h5x tv
    // HDMMI20 AVMUTE bypass control
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_sel_in(0));
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_sel_in(enable));
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_clr_bypass_en(0));
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute20_set_bypass_en(enable));

    // HDMMI21 AVMUTE bypass control
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_sel_in(0));
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_sel_in_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_sel_in(enable));
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_clr_bypass_en(0));
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_bypass_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_avmute21_set_bypass_en(enable));

    // HDMI20/21 dispD2tx forceBg
    rtd_maskl(HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_reg, ~HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_dispd2tx_force_bg_en_mask, HDMITX_MISC_MUTE_ENC_BYPASS_CTRL00_dispd2tx_force_bg_en(enable));

}

void ScalerHdmiTxBypassPacketCtrl(UINT8 enable)
{
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL00_reg, ~HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en(enable));
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_21_en_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_21_en(enable));
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en(enable));
}

void ScalerHdmiTxBypassAVMuteAndPacketCtrl(UINT8 enable)
{
    ScalerHdmiTxBypassAVMuteCtrl(enable);
    ScalerHdmiTxBypassPacketCtrl(enable);
}

void ScalerHdmiMacTx0RepeaterSetPktHdrBypass(UINT8 enable)
{
    UINT8 value_u8;

    //rtd_outl(0xb8019700, 0x00000001);   // HDMITX_MISC_HD_BYPASS_CTRL00_reg
    // update avi/vsif/dram/spd in FAPA region location 0 or v-blanking: 0x1:FAPA region region location 0 , 0x0: v-blanking
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL00_reg, HDMITX_MISC_HD_BYPASS_CTRL00_hd_packet_trans_mode(0));
    // metabuf_en: packet transfer main switch (2dff): 0x0:Disable , 0x1: Enable
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL00_reg, ~HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en(enable));

    FatalMessageHDMITx("[HDMI_TX] BYPASS_PKT_HDR Cfg=%x\n", (UINT32)hdmiRepeaterBypassPktHdrEnMask);

    // Info frame packet bypass
    //rtd_outl(0xb8019704, 0x000003ff);   // HDMITX_MISC_HD_BYPASS_CTRL01_reg
    // [DANBU-718] set VSIF packet enable config: transfer vsif oui info frame packet(s) which is (are) set to enable
    rtd_outl(HDMITX_MISC_HD_OUI_COS_reg, VSIF_OUI_CUVA); // OUI: CUVA
    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_OUI_CUNTOMER);
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_cuntomer_en(value_u8));

    rtd_outl(HDMITX_MISC_HD_OUI_DB_reg, VSIF_OUI_DOLBY); // OUI: Dolby
    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_OUI_DOLBY);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_dolby_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_dolby_en(value_u8));

    
    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_OUI_HDR10P);
    if(value_u8)
        rtd_outl(HDMITX_MISC_HD_OUI_HDR10P_reg, VSIF_OUI_HDR10P); // OUI: HDR10P
    else
        rtd_outl(HDMITX_MISC_HD_OUI_HDR10P_reg, 0);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hdr10p_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hdr10p_en(value_u8));

    rtd_outl(HDMITX_MISC_HD_OUI_H14B_reg, VSIF_OUI_H14B); // OUI: H14B
    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_OUI_H14B);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_h14b_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_h14b_en(value_u8));

    rtd_outl(HDMITX_MISC_HD_OUI_HF_reg, VSIF_OUI_HF); // OUI: HF
    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_OUI_HF);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hf_en(value_u8));

    // audio info frame packet bypass control
    value_u8 = (!SCRIPT_TX_SKIP_AUDIO_BYPASS() && ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_AUDIO_INF)? 1: 0);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_inf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_inf_en(value_u8));

    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_GMP);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_gmp_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_gmp_en(value_u8));
    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_RES);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_res_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_res_en(value_u8));
    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_SPD);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_spd_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_spd_en(value_u8));

    // audio sample packet bypass control
    value_u8 = (!SCRIPT_TX_SKIP_AUDIO_BYPASS() && ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_AUDIO_HBR)? 1: 0);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_hbr_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_hbr_en(value_u8));
    value_u8 = (!SCRIPT_TX_SKIP_AUDIO_BYPASS() && ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_AUDIO)? 1: 0);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_en(value_u8));

    lib_hdmitx_Vrrpacket_bypass_setting();

    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_VSIF);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_vsif_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_vsif_en(value_u8));

    value_u8 = (((get_hdmitxOutputMode() != TX_OUT_THROUGH_MODE) || (SCRIPT_TX_ENABLE_PACKET_BYPASS() == _FALSE))? 0: 1);
    value_u8 = ((need_sw_avi_packet || g_scaler_display_info.input_src  != 0) ? 0 : value_u8);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_avi_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_avi_en(value_u8));

    value_u8 = ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_DRAM);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_dram_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_dram_en(value_u8));   // HDMITX_MISC_HD_BYPASS_CTRL01_reg
    value_u8 = (((ScalerHdmiTxRepeaterGetBypassPktHdrEn(TX_BYPASS_PKT_HD_DRAM)) && (g_scaler_display_info.input_src == 1))? 1: 0);
    rtd_maskl(HDMITX_MISC_DP_BYPASS_CTRL00_reg, ~HDMITX_MISC_DP_BYPASS_CTRL00_dp_dram_bypass_en_mask, HDMITX_MISC_DP_BYPASS_CTRL00_dp_dram_bypass_en(value_u8));   // HDMITX_MISC_HD_BYPASS_CTRL01_reg

    InfoMessageHDMITx("[HDMI_TX] BYPASS_PKT_HDR Setting=%x\n", (UINT32)rtd_inl(HDMITX_MISC_HD_BYPASS_CTRL01_reg));
    InfoMessageHDMITx("[HDMI_TX] DP_BYPASS_PKT_HDR Setting=%x\n", (UINT32)rtd_inl(HDMITX_MISC_DP_BYPASS_CTRL00_reg));
    //rtd_outl(0xb801970c, 0x83090285);   // HDMITX_MISC_HD_BYPASS_CTRL03_reg
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, HDMITX_MISC_HD_BYPASS_CTRL03_hd_spd_header(0x83));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, ~HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_hbr_header_mask, HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_hbr_header(0x9));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, ~HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_header_mask, HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_header(0x2));
#ifdef CONFIG_FORCE_AUDIO_PACKET_BYPASS
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, ~HDMITX_MISC_HD_BYPASS_CTRL03_hd_res_header_mask, HDMITX_MISC_HD_BYPASS_CTRL03_hd_res_header(0x01));   // HDMITX_MISC_HD_BYPASS_CTRL03_reg
#else
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, ~HDMITX_MISC_HD_BYPASS_CTRL03_hd_res_header_mask, HDMITX_MISC_HD_BYPASS_CTRL03_hd_res_header(0x85));   // HDMITX_MISC_HD_BYPASS_CTRL03_reg
#endif
    // EMP packet bypass control
    value_u8 = (SCRIPT_TX_SKIP_EMP_BYPASS()? _DISABLE: _ENABLE);
    ScalerHdmiMacTx0RepeaterSetPktHdrBypass_EmpPacketBypass(value_u8);

    //rtd_outl(0xb8019550, 0x00000122);   // HDMITX_MISC_PACKET_PATH_CTRL_00_reg
    // B[9:8]: fapa_end_sel: 0x0: FW setting 1/2 v-blanking, 0x1: Vsync falling
    rtd_outl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, HDMITX_MISC_PACKET_PATH_CTRL_00_fapa_end_sel(1));
    // B[3:2]: pkt_bypass_src_sel: bypass for HDMI: 0x0:HDMIRX , 0x1: DPRX
    value_u8 = (g_scaler_display_info.input_src == 0? 0: 1); // HDMI20 mode
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_src_sel(value_u8));

    // bypass for HDMI21: HDMI 21 audio input path select: 0x0: Original path(spdif in), 0x1: audio sample packet
    value_u8 = ((enable && !SCRIPT_TX_SKIP_AUDIO_BYPASS() && (ScalerHdmiTxGetTargetFrlRate() > 0))? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_aud21_pkt_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_aud21_pkt_src_sel(value_u8));

    // bypass for HDMI20: HDMI 20 audio input path select: 0x0: Original path(spdif in) , 0x1: audio sample packet
    value_u8 = ((enable && !SCRIPT_TX_SKIP_AUDIO_BYPASS() && (ScalerHdmiTxGetTargetFrlRate() == 0))? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_aud20_pkt_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_aud20_pkt_src_sel(value_u8));

    // bypass for HDMI21: HDMI 21 infoframe/EMP packet: 0x0:Disable BYPASS packet , 0x1: Enable BYPASS packet
    value_u8 = ((enable && (ScalerHdmiTxGetTargetFrlRate() > 0))? 1: 0); // for HDMI21 mode
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_21_en_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_21_en(value_u8));

    // bypass for HDMI20: HDMI20 infoframe/EMP packet: 0x0:Disable BYPASS packet , 0x1: Enable BYPASS packet
    value_u8 = ((enable && (ScalerHdmiTxGetTargetFrlRate() == 0))? 1: 0); // for HDMI20 mode
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en(value_u8));

    value_u8 = (enable && (SCRIPT_TX_SKIP_AVMUTE_BYPASS() == 0) ? 1: 0);
    // HDMMI20/21 AVMUTE bypass control
    ScalerHdmiTxBypassAVMuteCtrl(value_u8);
    InfoMessageHDMITx("[HDMI_TX] HDMITX_MISC_PACKET_PATH_CTRL_00_reg=%x\n", (UINT32)rtd_inl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg));
    return;
}


void ScalerHdmiMacTx0PacketBypassConfig(void)
{
    UINT8 value_u8;

#if 1  //new HW setting //#ifdef FIX_ME_HDMITX_BRING_UP
    // check HDMI mode status
    //value_u8 = (SCRIPT_TX_DTG_FSYNC_EN() && SCRIPT_TX_ENABLE_PACKET_BYPASS()? _TRUE: _FALSE);
    UINT8 timeout = 3;//(value_u8? 10: 1);
    while(--timeout){
        value_u8 = HDMITX20_MAC0_SCHEDULER_0_get_hdmi_dvi_mode_sel(rtd_inl(HDMITX20_MAC0_SCHEDULER_0_reg));
        if(value_u8)
            break;
        HDMITX_DTG_Wait_Den_Stop_Done();
        HDMITX_DTG_Wait_vsync_start();
    }
    if(timeout == 0)
        InfoMessageHDMITx("[HDMITX] wait HDMI mode timeout(10)\n");

    // [FIX-ME] to avoid RX received broken info frame packet
    // [Note] enable packet bypass when DTG fsync ready or clear avmute after fsync ready
    value_u8 = (get_hdmitxOutputMode() == TX_OUT_TEST_MODE? 0: SCRIPT_TX_ENABLE_PACKET_BYPASS());
    ScalerHdmiMacTx0RepeaterSetPktHdrBypass(value_u8);

    if(SCRIPT_TX_ENABLE_PACKET_BYPASS()){
        HDMITX_DTG_Wait_Den_Stop_Done();
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
        drvif_hdmi_pkt_bypass_to_hdmitx_en(0xF, _ENABLE);
#endif

        InfoMessageHDMITx("[HDMITX] RX Bypass to TX Done!\n");
    }
#endif // #if 1  // new HW setting //#ifdef FIX_ME_HDMITX_BRING_UP

    ScalerHdmiMacTx0NCtsRegenConfig();//hdmi20 N/CTS regin setting

    return;
}
