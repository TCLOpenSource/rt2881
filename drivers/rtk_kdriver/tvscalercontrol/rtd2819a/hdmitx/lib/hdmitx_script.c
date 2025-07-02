#include <arch.h>
#include <mach.h>
#include <rtk_io.h>

#include <rtk_kdriver/timer.h>
#include <rtk_kdriver/pm_printk.h>
#include <timer_event.h>

#include "msg_q_function_api.h"

//#include <rbus/sys_reg_reg.h>
//#include <rbus/ppoverlay_reg.h>
#include <rbus/ppoverlay_outtg_reg.h>
#include <rbus/two_step_uzu_reg.h>
//#include <rbus/vgip_reg.h>
#include <rbus/input_3x3_gamma_reg.h>
#include <rbus/srnn_pre_post_reg.h>
#include <rbus/hdmitx_phy_reg.h>
#include <rbus/hdmitx20_mac0_reg.h>
#include <rbus/hdmitx20_mac1_reg.h>
#include <rbus/hdmitx20_on_region_reg.h>
#include <rbus/hdmitx21_mac_reg.h>
#include <rbus/hdmitx21_on_region_reg.h>
#include <rbus/hdmitx21_packet_control_reg.h>
#include <rbus/hdmitx_misc_reg.h>
#include <rbus/hdmi_p0_reg.h>
#include <rbus/hdmi21_p0_reg.h>

#include <rbus/timer_reg.h>

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_init.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_driver.h>

#include <rbus-dante/hdmi_p0_reg.h>
#include <rtk_kdriver/tvscalercontrol/hdmirx/drvif_hdmirx.h>
//#include "../../../rtk_kdriver/tvscalercontrol/hdmirx/hdmi_info_packet.h"
//#include "../../../rtk_kdriver/tvscalercontrol/hdmirx/hdmi_video.h"

PRAGMA_CODESEG(BANK6);   // HAMITX Bank: 6/7/8/9/38);
PRAGMA_CONSTSEG(BANK6);  // HAMITX Bank: 6/7/8/9/38);

/************************
// Type/Enum define
//
*************************/
typedef enum{
    HDMITX_INPUT_TIMING_HDMI21_3G3L_640X480P60_8BIT_RGB=0,
    HDMITX_INPUT_TIMING_HDMI21_3G3L_1080P60_8BIT_RGB,
    HDMITX_INPUT_TIMING_HDMI21_6G4L_1080P60_8BIT_RGB,
    HDMITX_INPUT_TIMING_HDMI21_6G4L_4k2kP60_8BIT_RGB,
    HDMITX_INPUT_TIMING_HDMI21_8G_4k2kP120_8BIT_RGB,
    HDMITX_INPUT_TIMING_HDMI20_2K1KP60_8BIT_RGB,
    HDMITX_INPUT_TIMING_HDMI20_4K2KP30_8BIT_RGB,
    HDMITX_INPUT_TIMING_HDMI20_4K2KP60_8BIT_RGB,
    HDMITX_INPUT_TIMING_NUM,
}EnumHdmitxInputTimingType;


typedef struct{
    UINT16 DTG_H_DEN_STA ;
    UINT16 DTG_V_DEN_STA ;
    UINT16 DTG_H_PORCH ;
    UINT16 DTG_V_PORCH ;
    UINT16 DTG_DH_ACT;
    UINT16 DTG_DV_ACT;
    UINT16 DTG_IV2DV_HSYNC_WIDTH;
    UINT16 DTG_DHS_WIDTH ;
    UINT16 DTG_DVS_LENGTH ;
}scaler_hdmi_timing_info;


typedef struct{
    UINT8 frl_mode;
    UINT8 vic;
    scaler_hdmi_timing_info scalerHdmiTimingInfo;
}hdmitx_output_timing_table;


#ifdef CONFIG_MACH_RTD2801_ZEBU
  #undef msleep
  #define msleep(x) delayus(10)
#else
  #ifndef msleep
  #define msleep delayms
  #endif
#endif

/************************
// Timing table definition
//
*************************/
//{set1}  : {blank, active, sync, front, back} for h
//{set2}  : {space, blank, act_video, active, sync, front, back} for v
//     {1, {160, 640, 96, 16, 48}, {1, 45, 480, 480, 2, 10, 33}, 251},                      // TX_TIMING_HDMI21_640x480P59_RGB_8BIT_3G (HDMI21_TX_640_480P_60HZ)
//    {16, {280, 1920, 44, 88, 148}, {1, 45, 1080, 1080, 5, 4, 36}, 1485},             //TX_TIMING_HDMI20_1080P60_RGB_8BIT (HDMI_TX_1920_1080P_60HZ) (HDMI2.0)
//    {95, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 2970},	  // TX_TIMING_HDMI20_4K2KP30_RGB_8BIT (HDMI_TX_3840_2160P_444_30HZ)
//    {97, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 5940},         // TX_TIMING_HDMI20_4K2KP60_RGB_8BIT (HDMI_TX_3840_2160P_444_60HZ)
//   {118, {560, 3840, 88, 176, 296}, {1, 90, 2160, 2160, 10, 8, 72}, 11880},	  // TX_TIMING_HDMI21_4K2KP120_YUV444_8BIT (HDMI21_TX_3840_2160P_444_120HZ_8G)

// set1: {frl_mode, vic}
// set2: {h-start, v-start, h-porch, v-porch, h-act, v-act, iv2dv_hsw, hs_wid, vs_len}
hdmitx_output_timing_table code hdmitxOutputTimingTable[HDMITX_INPUT_TIMING_NUM]={
                                    {1, 2, {144, 35, 160, 45, 640  , 480 , 0x320, 96, 2}},        /* HDMITX_INPUT_TIMING_HDMI21_3G3L_640X480P60_8BIT_RGB */
                                    {1, 16, {192, 41, 280, 45, 1920, 1080, 0x320, 44, 5}},      /* HDMITX_INPUT_TIMING_HDMI21_3G3L_1080P60_8BIT_RGB */
                                    {3, 16, {192, 41, 280, 45, 1920, 1080, 0x320, 44, 5}},      /* HDMITX_INPUT_TIMING_HDMI21_6G4L_1080P60_8BIT_RGB */
                                    {3, 97, {384, 85, 560, 90, 3840, 2160, 0x320, 88, 10}},  /* HDMITX_INPUT_TIMING_HDMI21_6G4L_4K2KP60_8BIT_RGB */
                                    {4, 118, {384, 85, 560, 90, 3840, 2160, 0x320, 88, 10}},  /* HDMITX_INPUT_TIMING_HDMI21_8G_4K2KP120_8BIT_RGB */
                                    {0, 16, {192, 41, 280, 45, 1920, 1080, 0x320, 44, 5}},      /* HDMITX_INPUT_TIMING_HDMI20_2K1KP60_8BIT_RGB */
                                    {0, 95, {384, 85, 560, 90, 3840, 2160, 0x320, 88, 10}},  /* HDMITX_INPUT_TIMING_HDMI20_4K2KP30_8BIT_RGB */
                                    {0, 97, {384, 85, 560, 90, 3840, 2160, 0x320, 88, 10}}};  /* HDMITX_INPUT_TIMING_HDMI20_4K2KP60_8BIT_RGB */

UINT8 code timingTableStr[HDMITX_INPUT_TIMING_NUM][40]={
    {"HDMI21_3G3L_640X480P60_8BIT_RGB"},// HDMITX_INPUT_TIMING_HDMI21_3G3L_640X480P60_8BIT_RGB=0,
    {"HDMI21_3G3L_1080P60_8BIT_RGB"},   // HDMITX_INPUT_TIMING_HDMI21_3G3L_1080P60_8BIT_RGB,
    {"HDMI21_6G4L_1080P60_8BIT_RGB"},   // HDMITX_INPUT_TIMING_HDMI21_6G4L_1080P60_8BIT_RGB,
    {"HDMI21_6G4L_4k2kP60_8BIT_RGB"},   // HDMITX_INPUT_TIMING_HDMI21_6G4L_4k2kP60_8BIT_RGB,
    {"HDMI21_8G_4k2kP120_8BIT_RGB"},    // HDMITX_INPUT_TIMING_HDMI21_8G_4k2kP120_8BIT_RGB,
    {"HDMI20_2K1KP60_8BIT_RGB"},        // HDMITX_INPUT_TIMING_HDMI20_2K1KP60_8BIT_RGB,
    {"HDMI20_4K2KP30_8BIT_RGB"},        // HDMITX_INPUT_TIMING_HDMI20_4K2KP30_8BIT_RGB,
    {"HDMI20_4K2KP60_8BIT_RGB"},        // HDMITX_INPUT_TIMING_HDMI20_4K2KP60_8BIT_RGB,
};

/************************
// Macro/define declaration
//
*************************/
#define HDMITX21_MAC_HDMI21_FRL_ERR 0xf8 // sram_ovf(_BIT7)| borrow_fail(_BIT6)| blank_video_err(_BIT5)| sup_blk_err(_BIT4)| input_tb_ovf (_BIT3)
#define _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE 70

#define HDMITX_INPUT_TIMING_MODE HDMITX_INPUT_TIMING_HDMI20_4K2KP60_8BIT_RGB

#define ENABLE_HDMI_REPEATER_MODE 1 // 1: repeater rx bypass to TX, 0: TX PTG (loopback)
#define ENABLE_LOAD_HDMIRX_SCRIPT 1 // 1: Load RX script, 0: RX driver
#define ENABLE_LOAD_HDMITX_SCRIPT 0 // 1: Load TX script, 0: TX driver flow

#define ENABLE_DTG_FSYNC 0
#define ENABLE_VRR_SUPPORT 0

#define _SKIP_AUDIO_BYPASS 1
#define _SKIP_EMP_BYPASS 1

#define _SKIP_EDID_MODIFY 0

#define ENABLE_SHOW_TX_PTG 0
#define ENABLE_SHOW_RX_PS 1

// VRR verify
#define VRR_EN 0

/************************
// Static/Local variable declaration
//
*************************/
static UINT32 value_u32, regValue;
static UINT16 value_u16;
static UINT8 value_u8;
static UINT8 ucHdmiMode; // 0: HDMI20, 1: HDMI21
static UINT8 ucLaneNum;
static UINT16 total_h, total_v;
static UINT8 div_u8;
static hdmitx_output_timing_table code *pHdmitxOutputTimingTable=NULL;
static scaler_hdmi_timing_info code *pScalerHdmiTimingInfo=NULL;

static UINT32 hdmitx21_mac_hdmi21_dpc_1_regValue;
static UINT32 hdmitx21_mac_hdmi21_vesa_cts_fifo_regValue;
static UINT32 hdmitx21_mac_hdmi21_frl_23_regValue;
static UINT32 hdmitx20_mac0_hdmi_control_2_regValue;
static UINT32 hdmitx20_mac1_hdmi_input_ctsfifo_regValue;
static UINT32 hdmitx_phy_dsc_insel_fifo_status_regValue;
static UINT8 bErrFlag=0;
static UINT8 bFifoUdf, bFifoOvf;
static UINT32 ucCheckCnt=0;
static UINT16 usFrameCnt_pre=0, usFrameCnt=0;
static UINT8 ucPtgToggleState=0;
static UINT16 usFrameCnt_lastDump=0;
static UINT32 uStcCnt_lastDump=0, uStcCnt;
static UINT16 stcDiff;

static UINT16 dtgIvsCnt, dtgDvsCnt;

// avi info frame
static UINT8 ucSBNum = 0x00;
static UINT8 ucChecksum = 0x00;
static UINT8 pucAVIInfoPacketData[5] = {0};

// RX packet status check
static UINT32 rxPktVideoStatus, rxPktVideoStatus1, rxEmPktVideoStatus;
/************************
// Extern variable declaration
//
*************************/
extern HDMITX_INIT_SETTING_TYPE hdmitx_init_setting;

/************************
// Local Function declaration
//
*************************/
void HDMITX_DV_Script_hdmitx_phy_clr_clkratio(void);
void HDMITX_DV_Script_hdmitx21_hdmitx_phy_pll(void);
void HDMITX_DV_Script_hdmitx21_mac_config(void);
void HDMITX_DV_Script_hdmitx21_mac_enable(UINT8 enable);
void HDMITX_DV_Script_hdmitx21_on_region_source_sel(UINT16 value);
void HDMITX_DV_Script_hdmitx21_reset_fifoErr(void);
void HDMITX_DV_Script_hdmitx20_reset_fifoErr(void);
void HDMITX_DV_Script_hdmitx20_check_fifoErr(void);

/************************
// Extern Function declaration
//
*************************/

//****************************************************************************
// Macro/Definition
//****************************************************************************
#define memcpy tx_script_memcpy
#define memset tx_script_memset


/************************
// Function implementation
//
*************************/
//static UINT16 i;
void tx_script_memset(UINT8 *buf, UINT8 value, UINT16 len)
{
    UINT16 i = 0;

    if(buf == NULL){
        //FatalMessageHDMITx("[HDMITX] NULL BUF@tx_script_memset\n");
        return;
    }

    for (i = 0; i < len; i ++) {
        buf[i] = value;
    }
}


#pragma save
#pragma nogcse /* turnoff global subexpression elimination */
void tx_script_memcpy(UINT8 *dst, UINT8 *src, UINT16 len)
{
    UINT16 i = 0;

    if((dst == NULL)|| (src == NULL)){
        //FatalMessageHDMITx("[HDMITX] NULL DST@tx_script_memcpy\n");
        return;
    }

    for (i = 0; i < len; i ++) {
        dst[i] = src[i];
    }
}
#pragma restore /* turn the optimizations back on */



void HDMITX_DV_Script_crt_init(void)
{

    NoteMessageHDMITx("[HDMITX] HDMITX_DV_Script_crt_init...\n");

    // rtd_outl(0xb8000234, 0x00001010);
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel(1));
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel(1));

    //rtd_outl(0xb8000104, 0x00000601);
    value_u32 = (SYS_REG_SYS_SRST1_rstn_hdmi_mask|SYS_REG_SYS_SRST1_rstn_disp_mask|SYS_REG_SYS_SRST1_write_data_mask);
    rtd_outl(SYS_REG_SYS_SRST1_reg,  value_u32);
    delayus(5);

    // rtd_outl(0xb8000114, 0x00000601);
    value_u32 = (SYS_REG_SYS_CLKEN1_clken_hdmi_mask|SYS_REG_SYS_CLKEN1_clken_disp_d_mask|SYS_REG_SYS_CLKEN1_write_data_mask);
    rtd_outl(SYS_REG_SYS_CLKEN1_reg, value_u32);
    delayus(5);

    //rtd_outl(0xb8000120, 0x00700001);
    value_u32 = SYS_REG_SYS_SRST4_rstn_hdmitx_mask|SYS_REG_SYS_SRST4_rstn_hdmitx_phy_mask|SYS_REG_SYS_SRST4_rstn_hdmitx_off_mask|SYS_REG_SYS_SRST4_write_data_mask;
    rtd_outl(SYS_REG_SYS_SRST4_reg, value_u32);
    delayus(5);

    // rtd_outl(0xb8000130, 0x0040007d);
    value_u32 = (SYS_REG_SYS_CLKEN4_clken_hdmitx_mask
                            |SYS_REG_SYS_CLKEN4_clken_disp_im_mask|SYS_REG_SYS_CLKEN4_clken_disp_gdma_mask|SYS_REG_SYS_CLKEN4_clken_dc2hlike_m_mask
                            |SYS_REG_SYS_CLKEN4_clken_dc2hlike_i_mask|SYS_REG_SYS_CLKEN4_clken_dc2hlike_d_mask|SYS_REG_SYS_CLKEN4_write_data_mask);
    rtd_outl(SYS_REG_SYS_CLKEN4_reg, value_u32);
    delayus(15);

    // rtd_outl(0xB8000138, 0x00020001);
    value_u32 = SYS_REG_SYS_CLKEN6_clken_disp_crt_mask|SYS_REG_SYS_CLKEN6_write_data_mask;
    rtd_outl(SYS_REG_SYS_CLKEN6_reg, value_u32);
    delayus(15);


    // rtd_outl(0xb8000234, 0x00001010);
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel(1));
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel(1));

    //rtd_outl(0xb8000204, 0x10202400);
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_capclk_src_sel_mask, SYS_REG_SYS_CLKSEL_capclk_src_sel(1)); // (0x10000000UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_hdmi_dp_sel_mask, SYS_REG_SYS_CLKSEL_hdmi_dp_sel(0)); // (0x02000000UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_d1d2clk_src_sel_mask, SYS_REG_SYS_CLKSEL_d1d2clk_src_sel(1)); // (0x00600000UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_dispd_gdma_clk_sel_mask, SYS_REG_SYS_CLKSEL_dispd_gdma_clk_sel(0)); // (0x00008000UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_usb3_lane_sel_mask, SYS_REG_SYS_CLKSEL_usb3_lane_sel(0)); // (0x00004000UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_bb_src_usb2phy_sel_mask, SYS_REG_SYS_CLKSEL_bb_src_usb2phy_sel(1)); // (0x00002000UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_usb3_src_usb2phy_sel_mask, SYS_REG_SYS_CLKSEL_usb3_src_usb2phy_sel(0)); // (0x00001000UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_p0_usb2phy_src_mac_sel_mask, SYS_REG_SYS_CLKSEL_p0_usb2phy_src_mac_sel(0)); // (0x00000800UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_p1_usb2phy_src_mac_sel_mask, SYS_REG_SYS_CLKSEL_p1_usb2phy_src_mac_sel(1)); // (0x00000400UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_i2c2_clksel_mask, SYS_REG_SYS_CLKSEL_i2c2_clksel(0)); // (0x00000008UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_i2c1_clksel_mask, SYS_REG_SYS_CLKSEL_i2c1_clksel(0)); // (0x00000004UL)
    rtd_maskl(SYS_REG_SYS_CLKSEL_reg, ~SYS_REG_SYS_CLKSEL_dpi_bypass_pll_ref_test_mode_mask, SYS_REG_SYS_CLKSEL_dpi_bypass_pll_ref_test_mode(0)); // (0x00000001UL)

    //rtd_outl(0xb8000234, 0x00011010);
    //rtd_outl(0xb8000234, 0x00011010);
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_2p_gate_sel(1));
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel(1));
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel(1));

    return;
}


//--------------------------------------------------
// Description  : Hdmi Tx AVI Infoframe Packet
// Input Value  :
// Output Value :
//--------------------------------------------------
void HDMITX_DV_Script_ScalerHdmiMacTx0FillAVIInfoPacket(void)
{
    pucAVIInfoPacketData[0] = 0x10;
    pucAVIInfoPacketData[1] = 0xd9;
    pucAVIInfoPacketData[2] = 0xc0;
    pucAVIInfoPacketData[3] = pHdmitxOutputTimingTable->vic;//0x5f; // VIC
    pucAVIInfoPacketData[4] = 0x20;

    ucChecksum -= 0x82;
    ucChecksum -= 0x02;
    ucChecksum -= 0x0D;

    ucChecksum -= pucAVIInfoPacketData[0];
    ucChecksum -= pucAVIInfoPacketData[1];
    ucChecksum -= pucAVIInfoPacketData[2];
    ucChecksum -= pucAVIInfoPacketData[3];
    ucChecksum -= pucAVIInfoPacketData[4];

    // Pacekt Header
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_3_reg, 0x82); // HB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_2_reg, 0x02); // HB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_1_reg, 0x0D); // HB2

    // Clear PB Data
    for(ucSBNum = 0; ucSBNum < 32; ucSBNum++)  // PB0-27
    {
        rtd_outl((HDMITX20_MAC1_HDMI21_SW_PKT_4_reg + ucSBNum*4), 0x00);
    }

    // Fill data in Packet Body, HW will Auto Calculate BCH
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_7_reg, ucChecksum); // WORD0: PB0
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_6_reg, pucAVIInfoPacketData[0]); // WORD0: PB1
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_5_reg, pucAVIInfoPacketData[1]); // WORD0: PB2
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_4_reg, pucAVIInfoPacketData[2]); // WORD0: PB3
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_11_reg, pucAVIInfoPacketData[3]); // WORD1: PB4
    rtd_outl(HDMITX20_MAC1_HDMI21_SW_PKT_10_reg, pucAVIInfoPacketData[4]); // WORD1: PB5

    return;
}


//--------------------------------------------------
// Description  : Hdmi Tx PSRAM Packet Write Process
// Input Value  :
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
bit HDMITX_DV_Script_ScalerHdmiMacTx0PktDataApplyToPSRAM(void)
{
    // Enable Write
    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_0_reg, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT3);

    return _TRUE;
}



void HDMITX_DV_Script_hdmitx_timing_gen_cfg(void)
{
    rtd_inl(0xb80194c4); //0x00000000);
    //rtd_outl(0xb80194c4, 0x00300025);   // HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA; // 0x30(48) -> 192
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_x_sta_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_x_sta(value_u16)); // h-start(48)
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA ; // 0x25(37) -> 41
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_y_sta_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_STA_self_tg_y_sta(value_u16)); // v-start(37)

    rtd_inl(0xb80194c8); //0x00000000);
    //rtd_outl(0xb80194c8, 0x01e00438);   // HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg
    div_u8 = (pHdmitxOutputTimingTable->frl_mode > 0? 4: 1); // timing gen /4 for HDMI21 mode
    value_u16 = pScalerHdmiTimingInfo->DTG_DH_ACT /div_u8; // 0x1e0(480) -> 1920/4=480
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_x_size_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_x_size(value_u16)); // pattern width(1920/4)
    value_u16 = pScalerHdmiTimingInfo->DTG_DV_ACT; // 0x438(1080) -> 1080
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_y_size_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_SIZE_self_tg_y_size(value_u16)); // pattern len(1080)

    rtd_inl(0xb80194cc); //0x00000000);
    //rtd_outl(0xb80194cc, 0x00160004);   // HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg
    // h-front = h-blank - h-start = 160-144=16
    value_u16 = pScalerHdmiTimingInfo->DTG_H_PORCH - pScalerHdmiTimingInfo->DTG_H_DEN_STA; // 0x16(22) -> 16
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_x_end_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_x_end(value_u16)); // h-front(22)
    // v-front = v-blank - v-start = 45 - 35 = 10
    value_u16 = pScalerHdmiTimingInfo->DTG_V_PORCH - pScalerHdmiTimingInfo->DTG_V_DEN_STA; // 0x4 -> 10
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_y_end_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_END_self_tg_y_end(value_u16)); // v-front(4)

    rtd_inl(0xb80194d4); //0x00000000);
    //rtd_outl(0xb80194d4, 0x000b0000);   // HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DHS_WIDTH/2 ; // 0xb(11) -> 44/2
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_end_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_end(value_u16)); // h-sync end(0xb)
    rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_sta_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_HSYNC_self_tg_hs_sta(0)); // h-sync start(0)

}



void HDMITX_DV_Script_hdmitx_pattern_gen_cfg(UINT8 enable)
{
    // PTG color setting
    rtd_inl(0xb80194dc); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg, 0x00001002);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg

    rtd_inl(0xb80194e0); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_reg, 0x00000000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_reg

    rtd_inl(0xb80194e4); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg, 0x004628b9);   //HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg

    rtd_inl(0xb80194e8); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_reg, 0x000000df);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_reg

    rtd_inl(0xb80194ec); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg, 0x00005038);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg

    rtd_inl(0xb80194f0); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg, 0x00000064);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg

    rtd_inl(0xb80194f4); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg, 0x0005a004);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg

    rtd_inl(0xb80194f8); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_reg, 0x0000004e);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_reg

    rtd_inl(0xb80194fc); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_reg, 0x00006401);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_reg

    rtd_inl(0xb8019500); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_reg, 0x00000005);   //HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_reg

    rtd_inl(0xb8019504); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_reg, 0x004007e9);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_reg

    rtd_inl(0xb8019508); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_reg, 0x0000002f);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_reg

    rtd_inl(0xb801950c); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_reg, 0x003151c9);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_reg

    rtd_inl(0xb8019510); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_reg, 0x000000b0);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_reg

    rtd_inl(0xb8019514); //0x0000f00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_reg, 0x0000a00f);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_reg

    rtd_inl(0xb8019518); //0x0000000f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_reg, 0x0000000c);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_reg

    // PTG control setting
    rtd_inl(0xb80194d8); //0x0000000f);
    //rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, 0xc000077f);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_width(0x77f)); //

    rtd_inl(0xb80194dc); //0x00001002);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg, 0x00ffffff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_RG_reg

    rtd_inl(0xb80194e0); //0x00000000);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_reg, 0x00000fff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C0_B_reg

    rtd_inl(0xb80194e4); //0x004628b9);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg, 0x00ffffff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_RG_reg

    rtd_inl(0xb80194e8); //0x000000df);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_reg, 0x00000000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C1_B_reg

    rtd_inl(0xb80194ec); //0x00005038);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg, 0x00fff000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_RG_reg

    rtd_inl(0xb80194f0); //0x00000064);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg, 0x00000fff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C2_B_reg

    rtd_inl(0xb80194f4); //0x0005a004);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg, 0x00fff000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_RG_reg

    rtd_inl(0xb80194f8); //0x0000004e);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_reg, 0x00000000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C3_B_reg

    rtd_inl(0xb80194fc); //0x00006401);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_reg, 0x00000fff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_RG_reg

    rtd_inl(0xb8019500); //0x00000005);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_reg, 0x00000fff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C4_B_reg

    rtd_inl(0xb8019504); //0x004007e9);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_reg, 0x00000fff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_RG_reg

    rtd_inl(0xb8019508); //0x0000002f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_reg, 0x00000000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C5_B_reg

    rtd_inl(0xb801950c); //0x003151c9);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_reg, 0x00000000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_RG_reg

    rtd_inl(0xb8019510); //0x000000b0);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_reg, 0x00000fff);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C6_B_reg

    rtd_inl(0xb8019514); //0x0000a00f);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_reg, 0x00000000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_RG_reg

    rtd_inl(0xb8019518); //0x0000000c);
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_reg, 0x00000000);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_C7_B_reg

    rtd_inl(0xb80194d8); //0xc000077f);
    //rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, 0x800000ef);   // HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg
    value_u16 = (pScalerHdmiTimingInfo->DTG_DH_ACT /8)-1; // 0xef(240-1) -> 240-1
    rtd_outl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_width(0xef)); //
    rtd_maskl(HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_reg, ~HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_en_mask, HDMITX_MISC_HDMITX_IN_PATTERN_GEN_CTRL_0_patgen_en(enable));

    return;
}


void HDMITX_DV_Script_scaler_IN3X3_pattern_gen_cfg(UINT8 enable, UINT8 dynamic)
{
    if(enable){
        // in boader : 3x3
        // 0x18024A00 = 0x00000041

        // in dynamic: 3x3
        // 0x18024A00 = 0x00000051

        // Pattern generator mode:
        // 'b000 : random pattern with Seed = 30'h1,
        // 'b001 : random pattern with Seed={14'h0,PATGEN_WIDTH}
        // 'b010 : self-defined color bar (4-colors)
        // 'b011 : self-defined color bar (8-colors)
        // 'b100 : 1 pixel border with cross
        // 'b101 : Moving block
        // 'b110 : self-defined checker board
        value_u8 = (dynamic? 5: 4);
        rtd_outl(INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl0_reg, INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl0_patgen_mode(value_u8));


        // dynamic pattern
        if(dynamic){
            // block:
            // 0xb8024a08= 0x40,
            rtd_outl(INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_FRAME_TOGGLE_reg,
                                                        INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_FRAME_TOGGLE_patgen_tog_num(0)|
                                                        INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_FRAME_TOGGLE_patgen_tog_sel(0)|
                                                        INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_FRAME_TOGGLE_patgen_width(0x40));

            // 0x18024A3C=00200020
            rtd_outl(INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_MOVING_CTRL1_reg,
                                                        INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_MOVING_CTRL1_y_speed(0x20)|
                                                        INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_MOVING_CTRL1_x_speed(0x20));
        }

        // size
        // 0xb8024a04= 7800438
        rtd_outl(INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl1_reg,
                                                        INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl1_patgen_hact_width(pScalerHdmiTimingInfo->DTG_DH_ACT)|
                                                        INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl1_patgen_vact_width(pScalerHdmiTimingInfo->DTG_DV_ACT));
    }

    // Pattern generator selector: 00: Pattern generator disabled, 01: Pattern generator connect to MAIN channel
    rtd_maskl(INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl0_reg, ~INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl0_patgen_sel_mask,
                                                                                            INPUT_3X3_GAMMA_IN_3X3_GAMMA_PATGEN_Globle_Ctrl0_patgen_sel(enable));
    return;
}


void HDMITX_DV_Script_scaler_2stepUZU_pattern_gen_cfg(UINT8 enable, UINT8 dynamic)
{

    if(enable){

        // uzu boader:
        // 0x18029600 = 0x00000041
        // uzu dynamic:3x3
        // 0x18029600 = 0x00000051
        value_u8 = (dynamic? 5: 4);
        rtd_outl(TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl0_reg, TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl0_patgen_mode(value_u8));

        // dynamic
        if(dynamic){
            // block:
            // 0xb8029608= 0x40
            rtd_outl(TWO_STEP_UZU_D_TWO_STEP_PATGEN_FRAME_TOGGLE_reg,
                                                        TWO_STEP_UZU_D_TWO_STEP_PATGEN_FRAME_TOGGLE_patgen_tog_num(0)|
                                                        TWO_STEP_UZU_D_TWO_STEP_PATGEN_FRAME_TOGGLE_patgen_tog_sel(0)|
                                                        TWO_STEP_UZU_D_TWO_STEP_PATGEN_FRAME_TOGGLE_patgen_width(4));
            // 0x1802963C=200020
            rtd_outl(TWO_STEP_UZU_D_TWO_STEP_PATGEN_MOVING_CTRL1_reg,
                                                        TWO_STEP_UZU_D_TWO_STEP_PATGEN_MOVING_CTRL1_y_speed(0x20)|
                                                        TWO_STEP_UZU_D_TWO_STEP_PATGEN_MOVING_CTRL1_x_speed(0x20));
        }

        // size
        // 0xb8029604= 7800438
        rtd_outl(TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg,
                                                       TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_patgen_hact_width(pScalerHdmiTimingInfo->DTG_DH_ACT)|
                                                       TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_patgen_vact_width(pScalerHdmiTimingInfo->DTG_DV_ACT));
    }

    // uzu boader:
    // 0x18029600 = 0x00000041
    // uzu dynamic:3x3
    // 0x18029600 = 0x00000051
    rtd_maskl(TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl0_reg, ~TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl0_patgen_sel_mask,
                                                                                            TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl0_get_patgen_den_mode(enable));

    return;
}



void HDMITX_DV_Script_hdmitx_misc_crcEn(void)
{
    // CRC enable
    rtd_inl(0xb80194a8); //0x00000000);
    //rtd_outl(0xb80194a8, 0x00000003);   // HDMITX_MISC_CRC_CTRL_reg
    rtd_outl(HDMITX_MISC_CRC_CTRL_reg, HDMITX_MISC_CRC_CTRL_crc_conti_mask|HDMITX_MISC_CRC_CTRL_crc_start_mask);
}


// mode: 1: bypass mode, 0: 2pto4p mode
void HDMITX_DV_Script_hdmitx_dsc_src_sel(UINT8 mode)
{
    rtd_inl(0xb80178f8); //0x00000000);
    //rtd_outl(0xb80178f8, 0x00000006);   // HDMITX_PHY_DSC_SRC_SEL_reg
    // Decide HDMITX_Insel function_mode: 0 => 2pto4p mode (4p valid, ch0/ch1/ch2/ch3), 1 => bypass mode (2p valid, ch0/ch1)
    rtd_outl(HDMITX_PHY_DSC_SRC_SEL_reg, HDMITX_PHY_DSC_SRC_SEL_reg_insel_mode(mode));
    rtd_maskl(HDMITX_PHY_DSC_SRC_SEL_reg, ~HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset_mask, HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset(1));
    rtd_maskl(HDMITX_PHY_DSC_SRC_SEL_reg, ~HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_en_mask, HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_en(1));

}


void HDMITX_DV_Script_hdmitx_pktHdrBypass_config(void)
{
    //rtd_outl(0xb8019700, 0x00000001);   // HDMITX_MISC_HD_BYPASS_CTRL00_reg
    // update avi/vsif/dram/spd in FAPA region location 0 or v-blanking: 0x1:FAPA region region location 0 , 0x0: v-blanking
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL00_reg, HDMITX_MISC_HD_BYPASS_CTRL00_hd_packet_trans_mode(0));
    // metabuf_en: packet transfer main switch (2dff): 0x0:Disable , 0x1: Enable
    value_u8 = (ENABLE_HDMI_REPEATER_MODE? 1: 0);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL00_reg, ~HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en(value_u8));

    //rtd_outl(0xb8019704, 0x000003ff);   // HDMITX_MISC_HD_BYPASS_CTRL01_reg
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_cuntomer_en(0));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_dolby_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_dolby_en(0));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hdr10p_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hdr10p_en(0));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_h14b_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_h14b_en(0));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_oui_hf_en(0));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_inf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_inf_en(1));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_gmp_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_gmp_en(1));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_res_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_res_en(1));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_spd_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_spd_en(1));

    value_u8 = (_SKIP_AUDIO_BYPASS? 0: 1); // bypass audio sample?
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_hbr_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_hbr_en(value_u8));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_en(value_u8));

    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_emp_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_emp_en(1));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_vsif_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_vsif_en(1));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_avi_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_avi_en(1));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, ~HDMITX_MISC_HD_BYPASS_CTRL01_hd_dram_en_mask, HDMITX_MISC_HD_BYPASS_CTRL01_hd_dram_en(1));   // HDMITX_MISC_HD_BYPASS_CTRL01_reg

    //rtd_outl(0xb801970c, 0x83090285);   // HDMITX_MISC_HD_BYPASS_CTRL03_reg
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, HDMITX_MISC_HD_BYPASS_CTRL03_hd_spd_header(0x83));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, ~HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_hbr_header_mask, HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_hbr_header(0x9));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, ~HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_header_mask, HDMITX_MISC_HD_BYPASS_CTRL03_hd_audio_header(0x2));
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, ~HDMITX_MISC_HD_BYPASS_CTRL03_hd_res_header_mask, HDMITX_MISC_HD_BYPASS_CTRL03_hd_res_header(0x85));   // HDMITX_MISC_HD_BYPASS_CTRL03_reg

    //rtd_outl(0xb8019744, 0x00000001);   // HDMITX_MISC_HD_EMP_CTL_EN_reg
    rtd_outl(HDMITX_MISC_HD_EMP_CTL_EN_reg, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid1_en(0));
    rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_usr_oid0_en(0));
    rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds2_en(0));
    rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds1_en(0));
    rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vsemds0_en(0));
    rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_sbtm_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_sbtm_en(0));
    rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vtem_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_vtem_en(0));

    // enable all EMP packet
    rtd_maskl(HDMITX_MISC_HD_EMP_CTL_EN_reg, ~HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en_mask, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en(1));   // HDMITX_MISC_HD_EMP_CTL_EN_reg

    //rtd_outl(0xb8019550, 0x00000122);   // HDMITX_MISC_PACKET_PATH_CTRL_00_reg
    rtd_outl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, HDMITX_MISC_PACKET_PATH_CTRL_00_fapa_end_sel(1));
    // bypass for HDMI: 0x0:HDMIRX , 0x1: DPRX
    value_u8 = 0;
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_src_sel(value_u8));

    // bypass for HDMI21: HDMI 21 audio input path select: 0x0: Original path(spdif in), 0x1: audio sample packet
    value_u8 = (!_SKIP_AUDIO_BYPASS && (pHdmitxOutputTimingTable->frl_mode > 0)? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_aud21_pkt_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_aud21_pkt_src_sel(value_u8));
    // bypass for HDMI21: HDMI 21 infoframe/EMP packet: 0x0:Disable BYPASS packet , 0x1: Enable BYPASS packet
    value_u8 = (pHdmitxOutputTimingTable->frl_mode > 0? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_21_en_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_21_en(value_u8));

    // bypass for HDMI20: HDMI 20 audio input path select: 0x0: Original path(spdif in) , 0x1: audio sample packet
    value_u8 = (!_SKIP_AUDIO_BYPASS && (pHdmitxOutputTimingTable->frl_mode == 0)? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_aud20_pkt_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_aud20_pkt_src_sel(value_u8));
    // bypass for HDMI20: HDMI20 infoframe/EMP packet: 0x0:Disable BYPASS packet , 0x1: Enable BYPASS packet
    value_u8 = (pHdmitxOutputTimingTable->frl_mode == 0? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en(value_u8));   // HDMITX_MISC_PACKET_PATH_CTRL_00_reg

    return;
}


void HDMITX_DV_Script_hdmitx_timing_gen_enable(UINT8 enable)
{
    rtd_inl(0xb80194c0); //0x00000000);
    //rtd_outl(0xb80194c0, 0x00000001);   // HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg
    rtd_outl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_reg, HDMITX_MISC_HDMITX_IN_TIMING_GEN_CTRL_self_tg_en(enable));

    return;
}


void HDMITX_DV_Script_hdmitx_frame_rate_measure_en(UINT8 enable)
{

    // measure frame rate
    rtd_inl(0xb80194d0); //0x00000000);
    //rtd_outl(0xb80194d0, 0x80000000); // HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg
    rtd_outl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg, HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en(enable));

    return;
}



void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_phy(void)
{
    NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmirx_phy ...\n");

    // HDMIRX IP setting -- keep no  change
    rtd_outl(0xb8040508, 0x00000001);   // HDMI_P0_FW_FUNC_reg
    rtd_outl(0xb8000204, 0x10202400);   // SYS_REG_SYS_CLKSEL_reg
    rtd_outl(0xb8040628, 0x02200000);   // HDMI_P0_hdmi_clken0_reg
    rtd_outl(0xb8040020, 0x00000001);   // HDMI_P0_TMDS_Z0CC_reg
    rtd_outl(0xb804001c, 0x00000010);   // HDMI_P0_TMDS_PWDCTL_reg
    rtd_outl(0xb8040018, 0x00000200);   // HDMI_P0_TMDS_OUTCTL_reg
    rtd_outl(0xb80401ac, 0x01c0d070);   // HDMI_P0_CH_CR_reg
    rtd_outl(0xb8040198, 0x02a54a95);   // HDMI_P0_PHY_FIFO_CR0_reg

    return;
}


void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_comboex(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmirx_comboex...\n");

    // HDMIRX IP setting -- keep no  change
    rtd_outl(0xb8037004, 0x00000007);   // HDMI_COMBOEX_EX_PHY_FIFO_CR1_reg
    rtd_outl(0xb8037018, 0x00000008);   // HDMI_COMBOEX_EX_PHY_FIFO_CR3_reg
    rtd_outl(0xb8037000, 0x00000015);   // HDMI_COMBOEX_EX_PHY_FIFO_CR0_reg
    rtd_outl(0xb8037014, 0x00000000);   // HDMI_COMBOEX_EX_PHY_FIFO_CR2_reg

    return;
}


void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_mac(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmirx_comboex...\n");

    // HDMIRX IP setting -- keep no  change
    rtd_outl(0xb8040024, 0x000a6c00);   // HDMI_P0_TMDS_CPS_reg
    rtd_outl(0xb8040090, 0x0000013c);   // HDMI_P0_HDMI_SCR_reg

    // B[0]: hdmi_2p0_en, 1: Enable HDMI 2.0 data path (including error detection)
#ifdef CONFIG_MACH_RTD2801_ZEBU
    // [ZEBU] B[0]=0, disable scramble for ZEBU
    rtd_outl(0xb80401b4, 0xffff00fe);   // HDMI_P0_HDMI_2p0_CR_reg
#else
    rtd_outl(0xb80401b4, 0xffff00ff);   // HDMI_P0_HDMI_2p0_CR_reg
#endif

    rtd_outl(0xb80401bc, 0x00000002);   // HDMI_P0_SCR_CR_reg
    rtd_outl(0xb80400bc, 0x22000000);   // HDMI_P0_HDMI_VCR_reg
    rtd_outl(0xb8040044, 0x00000400);   // HDMI_P0_TMDS_DPC1_reg
    rtd_outl(0xb8040048, 0x00000000);   // HDMI_P0_TMDS_DPC_SET0_reg
    rtd_outl(0xb8040098, 0x00000008);   // HDMI_P0_HDMI_AVMCR_reg
    rtd_outl(0xb8040628, 0x22200000);   // HDMI_P0_hdmi_clken0_reg
    rtd_outl(0xb8040524, 0x00000009);   // HDMI_P0_hdmi_20_21_ctrl_reg
    rtd_outl(0xb8040628, 0x02200000);   // HDMI_P0_hdmi_clken0_reg
    rtd_outl(0xb8040620, 0x1fe40000);   // HDMI_P0_hdmi_rst0_reg
    rtd_outl(0xb8040620, 0x3fe40000);   // HDMI_P0_hdmi_rst0_reg
    rtd_outl(0xb804050c, 0x00000000);   // HDMI_P0_PORT_SWITCH_reg
    rtd_outl(0xb8040628, 0x22200000);   // HDMI_P0_hdmi_clken0_reg
    rtd_outl(0xb8040048, 0x00000080);   // HDMI_P0_TMDS_DPC_SET0_reg

    // enable all EMP packet detect
    rtd_maskl(HDMI_P0_fapa_ct_tmp_reg, ~HDMI_P0_fapa_ct_tmp_all_mode_mask, HDMI_P0_fapa_ct_tmp_all_mode(1));
    rtd_maskl(HDMI_P0_em_ct_reg, ~HDMI_P0_em_ct_hdr_hd20_hd21_sel_mask, HDMI_P0_em_ct_hdr_hd20_hd21_sel(0));

    // clear vrr memory
    rtd_maskl(HDMI_P0_HDMI_VRR_EMC_reg, ~HDMI_P0_HDMI_VRR_EMC_vrr_clr_mask, HDMI_P0_HDMI_VRR_EMC_vrr_clr(1));
    rtd_maskl(HDMI_P0_HDMI_VRR_EMC_reg, ~HDMI_P0_HDMI_VRR_EMC_vrr_clr_mask, HDMI_P0_HDMI_VRR_EMC_vrr_clr(0));

    return;
}
void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_aud(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmirx_aud...\n");

    // HDMIRX IP setting -- keep no  change
    rtd_outl(0xb8040014, 0x00000008);   // HDMI_P0_TMDS_CTRL_reg
    rtd_outl(0xb804011c, 0x00000001);   // HDMI_P0_HDMI_AUDCR_reg
    rtd_outl(0xb8040014, 0x00000008);   // HDMI_P0_TMDS_CTRL_reg
    rtd_outl(0xb8040014, 0x00000008);   // HDMI_P0_TMDS_CTRL_reg
}


void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_clr_vsync(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmirx_clr_vsync...\n");

    // HDMIRX IP setting -- keep no  change
    rtd_outl(0xb8040014, 0x00000008);   // HDMI_P0_TMDS_CTRL_reg
    rtd_outl(0xb8040014, 0x00000008);   // HDMI_P0_TMDS_CTRL_reg
    rtd_outl(0xb8040014, 0x00000008);   // HDMI_P0_TMDS_CTRL_reg
}

void imd_1080p60_bypass_no_srnnprepost_new(void)
{
    //vby1 display 4K default);
    //dw 0xb802d904 , 01b008c9 (iv2dv dvtotal);
    //dw 0xb802d908 , 112f112f (dhtotal dh last line);
    //dw 0xb802d90c , 00001f03 (hswidth 15~8 vswidth 7~0);
    //dw 0xb802d910 , 01181018 (dh_den_st dh_den_end);
    //dw 0xb802d914 , 002d089d (dv_den_st dv_den_end);

    //dh total , 897);
    //dh denst , 8c end+780);
    //dv debst , 16 end+438);
    #if 0 // VBY1 RX
    // 2kto4k timing);
    rtd_outl(0xb808d970,0x0000031f);
    rtd_outl(0xb808d974,0x08c91130);
    rtd_outl(0xb808d978,0x002d089d);
    rtd_outl(0xb808d97c,0x01181018);

    // vby1 timing adjust);
    rtd_outl(0xb808d914,0x002d089d);

    //vby1 pq bypass);
    rtd_outl(0xb808d900,0x003b0001);
    #endif
    //========================================
    // IMD START
    //========================================
    // vgip
    rtd_outl(0xb8020210, 0x80000003);   // VGIP_VGIP_CHN1_CTRL_reg

    //rtd_outl(0xb8020218, 0x00000780);   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg
    rtd_outl(VGIP_VGIP_CHN1_ACT_HSTA_Width_reg,
                    VGIP_VGIP_CHN1_ACT_HSTA_Width_ch1_ih_act_sta(0)|
                    VGIP_VGIP_CHN1_ACT_HSTA_Width_ch1_ih_act_wid(pScalerHdmiTimingInfo->DTG_DH_ACT));   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg

    //rtd_outl(0xb802021c, 0x00000438);   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg
    rtd_outl(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg,
                    VGIP_VGIP_CHN1_ACT_VSTA_Length_ch1_iv_act_sta(0)|
                    VGIP_VGIP_CHN1_ACT_VSTA_Length_ch1_iv_act_len(pScalerHdmiTimingInfo->DTG_DV_ACT));   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg

    rtd_outl(0xb8020224, 0x20002900);   // VGIP_VGIP_CHN1_MISC_reg
    //rtd_outl(0xb8020224, 0x20002904); // add channel swap

    // ds444
    rtd_outl(0xb8023b1c, 0x00000003);   // DISPD_DS444_DISPD_DS444_ACC_CRC_CRC_Ctrl_reg

    // multi band
    rtd_outl(0xb8023ce8, 0x00000001);   // COLOR_MB_PEAKING_SDNR_4K_120_MODE_reg

    //prepost
    rtd_outl(0xb8025600, 0x00000002);

    //rtd_outl(0xb8025604, 0x07800438);
    rtd_outl(SRNN_PRE_POST_SRNN_PRE_POS_UZU_Input_Size_reg,
                    SRNN_PRE_POST_SRNN_PRE_POS_UZU_Input_Size_srnn_uzu_hor_input_size(pScalerHdmiTimingInfo->DTG_DH_ACT)|
                    SRNN_PRE_POST_SRNN_PRE_POS_UZU_Input_Size_srnn_uzu_ver_input_size(pScalerHdmiTimingInfo->DTG_DV_ACT));

    //rtd_outl(0xb8025804, 0x07800438);
    rtd_outl(SRNN_PRE_POST_D_SRNN_PATGEN_Globle_Ctrl1_reg,
                    SRNN_PRE_POST_D_SRNN_PATGEN_Globle_Ctrl1_patgen_hact_width(pScalerHdmiTimingInfo->DTG_DH_ACT)|
                    SRNN_PRE_POST_D_SRNN_PATGEN_Globle_Ctrl1_patgen_vact_width(pScalerHdmiTimingInfo->DTG_DV_ACT));

    // two step uzu
    //rtd_outl(0xb8029604, 0x07800438);   // TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg
    rtd_outl(TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg,
                    TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_patgen_hact_width(pScalerHdmiTimingInfo->DTG_DH_ACT)|
                    TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_patgen_vact_width(pScalerHdmiTimingInfo->DTG_DV_ACT));   // TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg

    //rtd_outl(0xb8029b2c, 0x07800438);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg
    rtd_outl(TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg,
                    TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_hor_input_size(pScalerHdmiTimingInfo->DTG_DH_ACT)|
                    TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_ver_input_size(pScalerHdmiTimingInfo->DTG_DV_ACT));   // TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg

    // dctl
    //rtd_outl(0xb8027004, 0x0000001f);   // PPOVERLAY_DH_Width_reg
    // Display h sync width : Setting value = (real value - 1)
    rtd_outl(PPOVERLAY_DH_Width_reg, PPOVERLAY_DH_Width_dh_width(pScalerHdmiTimingInfo->DTG_DHS_WIDTH-1));   // PPOVERLAY_DH_Width_reg

    //rtd_outl(0xb8027008, 0x08970000);   // PPOVERLAY_DH_Total_Length_reg
    // Display horizontal total pixels for d domain: Setting value = (real value - 1)
    total_h = pScalerHdmiTimingInfo->DTG_DH_ACT + pScalerHdmiTimingInfo->DTG_H_PORCH - 1;
    rtd_outl(PPOVERLAY_DH_Total_Length_reg, PPOVERLAY_DH_Total_Length_dh_total(total_h));

    // rtd_outl(0xb802700c, 0x00000002);   // PPOVERLAY_DV_Length_reg
    // Display vertical sync width: Setting value = (real value - 1)
    rtd_outl(PPOVERLAY_DV_Length_reg, PPOVERLAY_DV_Length_dv_length(pScalerHdmiTimingInfo->DTG_DVS_LENGTH-1));   // PPOVERLAY_DV_Length_reg

    //rtd_outl(0xb8027010, 0x00000464);   // PPOVERLAY_DV_total_reg
    // Display vertical total lines: Note: set 1N-1
    total_v = pScalerHdmiTimingInfo->DTG_DV_ACT + pScalerHdmiTimingInfo->DTG_V_PORCH - 1;
    rtd_outl(PPOVERLAY_DV_total_reg, PPOVERLAY_DV_total_dv_total(total_v));   // PPOVERLAY_DV_total_reg

    //rtd_outl(0xb8027014, 0x008c080c);   // PPOVERLAY_DH_DEN_Start_End_reg
    // Display v data enable start: Note: set 1N
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA + pScalerHdmiTimingInfo->DTG_DH_ACT;
    rtd_outl(PPOVERLAY_DH_DEN_Start_End_reg,
                    PPOVERLAY_DH_DEN_Start_End_dh_den_sta(pScalerHdmiTimingInfo->DTG_H_DEN_STA)|
                    PPOVERLAY_DH_DEN_Start_End_dh_den_end(value_u16));   // PPOVERLAY_DH_DEN_Start_End_reg

    //rtd_outl(0xb8027018, 0x0016044e);   // PPOVERLAY_DV_DEN_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA + pScalerHdmiTimingInfo->DTG_DV_ACT;
    rtd_outl(PPOVERLAY_DV_DEN_Start_End_reg,
                    PPOVERLAY_DV_DEN_Start_End_dv_den_sta(pScalerHdmiTimingInfo->DTG_V_DEN_STA)|
                    PPOVERLAY_DV_DEN_Start_End_dv_den_end(value_u16));   // PPOVERLAY_DV_DEN_Start_End_reg


    //rtd_outl(0xb8027400, 0x00000006);   // PPOVERLAY_DTG_sw_delay_timing_reg
    rtd_outl(PPOVERLAY_DTG_sw_delay_timing_reg,
                    PPOVERLAY_DTG_sw_delay_timing_osd_loc1_output_sw_delay_en(1)|
                    PPOVERLAY_DTG_sw_delay_timing_two_step_uzu_sw_delay_en(1)|
                    PPOVERLAY_DTG_sw_delay_timing_srnn_sw_delay_en(0));   // PPOVERLAY_DTG_sw_delay_timing_reg

    //rtd_outl(0xb8027420, 0x00580000);   // PPOVERLAY_DTG_two_step_uzu_timing_reg
    rtd_outl(PPOVERLAY_DTG_two_step_uzu_timing_reg,
                    PPOVERLAY_DTG_two_step_uzu_timing_two_step_uzu_pixel_delay(0x58)|
                    PPOVERLAY_DTG_two_step_uzu_timing_two_step_uzu_line_delay(0));   // PPOVERLAY_DTG_two_step_uzu_timing_reg

    //rtd_outl(0xb8027440, 0x00d30000);   // PPOVERLAY_DTG_osd_loc1_output_timing_reg
    rtd_outl(PPOVERLAY_DTG_osd_loc1_output_timing_reg,
                    PPOVERLAY_DTG_osd_loc1_output_timing_osd_loc1_output_pixel_delay(0xd3)|
                    PPOVERLAY_DTG_osd_loc1_output_timing_osd_loc1_output_line_delay(0));   // PPOVERLAY_DTG_osd_loc1_output_timing_reg

    //rtd_outl(0xb8027000, 0x80000010);   // PPOVERLAY_Display_Timing_CTRL1_reg
    rtd_outl(PPOVERLAY_Display_Timing_CTRL1_reg,
                    PPOVERLAY_Display_Timing_CTRL1_disp_frc_fsync(1)|
                    PPOVERLAY_Display_Timing_CTRL1_disp_fsync_en(1)|
                    PPOVERLAY_Display_Timing_CTRL1_disp_en(0));   // PPOVERLAY_Display_Timing_CTRL1_reg

    //rtd_outl(0xb8027000, 0x80000011);   // PPOVERLAY_Display_Timing_CTRL1_reg
    rtd_maskl(PPOVERLAY_Display_Timing_CTRL1_reg, ~PPOVERLAY_Display_Timing_CTRL1_disp_en_mask,
                    PPOVERLAY_Display_Timing_CTRL1_disp_en(1));


    // dctl bypass
    //rtd_outl(0xb8027804, 0x0000001f);   // PPOVERLAY_OUTTG_OUTTG_DH_Width_reg
    // Display h sync width: Setting value = (real value - 1)
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DH_Width_reg, PPOVERLAY_OUTTG_OUTTG_DH_Width_dh_width(pScalerHdmiTimingInfo->DTG_DHS_WIDTH-1));   // PPOVERLAY_OUTTG_OUTTG_DH_Width_reg

    //rtd_outl(0xb8027808, 0x08970000);   // PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg
    total_h = pScalerHdmiTimingInfo->DTG_DH_ACT + pScalerHdmiTimingInfo->DTG_H_PORCH - 1;
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg, PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_dh_total(total_h));   // PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg

    //rtd_outl(0xb802780c, 0x00000002);   // PPOVERLAY_OUTTG_OUTTG_DV_Length_reg
    // Display vertical sync width: Setting value = (real value - 1)
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DV_Length_reg, PPOVERLAY_OUTTG_OUTTG_DV_Length_dv_length(pScalerHdmiTimingInfo->DTG_DVS_LENGTH-1));   // PPOVERLAY_OUTTG_OUTTG_DV_Length_reg


    //rtd_outl(0xb8027810, 0x00000464);   // PPOVERLAY_OUTTG_OUTTG_DV_total_reg
    total_v = pScalerHdmiTimingInfo->DTG_DV_ACT + pScalerHdmiTimingInfo->DTG_V_PORCH - 1;
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DV_total_reg, PPOVERLAY_OUTTG_OUTTG_DV_total_dv_total(total_v));   // PPOVERLAY_OUTTG_OUTTG_DV_total_reg

    //rtd_outl(0xb8027814, 0x008c080c);   // PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA + pScalerHdmiTimingInfo->DTG_DH_ACT;
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg,
                        PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_sta(pScalerHdmiTimingInfo->DTG_H_DEN_STA)|
                        PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_end(value_u16));   // PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg

    //rtd_outl(0xb8027818, 0x0016044e);   // PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA + pScalerHdmiTimingInfo->DTG_DV_ACT;
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg,
                            PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_sta(pScalerHdmiTimingInfo->DTG_V_DEN_STA)|
                            PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_end(value_u16));   // PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg


    //rtd_outl(0xb8027840, 0x008c080c);   // PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA + pScalerHdmiTimingInfo->DTG_DH_ACT;
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg,
                            PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_sta(pScalerHdmiTimingInfo->DTG_H_DEN_STA)|
                            PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_end(value_u16));   // PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg

    //rtd_outl(0xb8027844, 0x0016044e);   // PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA + pScalerHdmiTimingInfo->DTG_DV_ACT;
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg,
                        PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_sta(pScalerHdmiTimingInfo->DTG_V_DEN_STA )|
                        PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_end(value_u16));   // PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg

    //rtd_outl(0xb8027800, 0x80000010);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg // no need fix_last_line
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg,
                        PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_frc_fsync(1)|
                        PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_fsync_en(1)|
                        PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en(0));   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg // no need fix_last_line

    //rtd_outl(0xb8027800, 0x80000011);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, ~PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en_mask,
                        PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en(1));   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg

    return;
}


void HDMITX_DV_Script_HDMIRX_pktBypassToTx_enable(UINT8 enable)
{
    // [v] HDMIRX bypass to TX output disable
    //rtd_outl(0xb8040210, 0x00000006);   // HDMI_P0_HDMI_PKT_reg
    value_u8 = (enable && (pHdmitxOutputTimingTable->frl_mode == 0)? 1: 0);
    rtd_outl(HDMI_P0_HDMI_PKT_reg, HDMI_P0_HDMI_PKT_pkt_non_aud_data_drop_en_mask|HDMI_P0_HDMI_PKT_pkt_head_drop_en_mask|HDMI_P0_HDMI_PKT_pkt_2hdmitx_output_en(value_u8));

    //rtd_outl(0xb8041360, 0x00000006);   // HDMI21_P0_HD21_HDMI_PKT_reg
    value_u8 = (enable && (pHdmitxOutputTimingTable->frl_mode > 0)? 1: 0);
    rtd_outl(HDMI21_P0_HD21_HDMI_PKT_reg, HDMI21_P0_HD21_HDMI_PKT_pkt_non_aud_data_drop_en_mask|HDMI21_P0_HD21_HDMI_PKT_pkt_head_drop_en_mask|HDMI21_P0_HD21_HDMI_PKT_pkt_2hdmitx_output_en(value_u8));

    return;
}

void HDMITX_DV_Script_HDMIRX_colorControlRegister_config(void)
{
    rtd_outl(0xb804127c, 0x21108060);   // HDMI21_P0_HD21_HDMI_VCR_reg
}


void HDMITX_DV_Script_hdmitx_NCTS_REGEN_config(void)
{
    // [V] HDMITX MISC NCTS Regen
    rtd_outl(0xb8019524, 0x00ff0820);   // HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg
    rtd_outl(0xb8019528, 0x080007e0);   // HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg
    // from hdmi21 1080p60 sscaling to 4k2k60
    rtd_outl(0xb801952c, 0x01000001);   // HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg
    rtd_outl(0xb8019520, 0x71000fa0);   // HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg

    return;
}


void HDMITX_DV_Script_Disp_DTG_outputMuxSel(UINT8 mode)
{
    // [V] Scaler DTG
    // d1_output_mux_sel (mux before UZD): 0: through d1 PQ IP, 1: bypass path
    //rtd_outl(0xb8027580, 0x00000001);   // PPOVERLAY_DTG_d1_output_mux_reg
    rtd_outl(PPOVERLAY_DTG_d1_output_mux_reg, PPOVERLAY_DTG_d1_output_mux_d1_output_mux_sel(mode));   // PPOVERLAY_DTG_d1_output_mux_reg
    return;
}


void HDMITX_DV_Script_Disp_OutTG_dispEn(void)
{
    // from hdmi21 1080p60 bypass mode IMD script
    //rtd_outl(0xb8027800, 0x80000010);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg // no need fix_last_line
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_frc_fsync(1)
                                                        | PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_fix_last_line(ENABLE_VRR_SUPPORT)
                                                        | PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_fsync_en(ENABLE_DTG_FSYNC));

    //rtd_outl(0xb8027800, 0x80000011);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, ~PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en_mask,
                                                            PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en(1));

    return;
}


void HDMITX_DV_Script_Disp_OutTG_disp_fsync_en(BOOLEAN enable)
{
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, ~PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_fsync_en_mask,
                                                            PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_fsync_en(enable));
    return;
}



void HDMITX_DV_Script_Disp_VGIP_cfg(void)
{
    // [X] Scaler VGIP
    rtd_outl(0xb8020284, 0x00002004);   // VGIP_Data_Path_Select_reg

    // from 1080p60 scaling to 4k2kp60
    rtd_outl(0xb8020210, 0x00000003);   // VGIP_VGIP_CHN1_CTRL_reg

    //rtd_outl(0xb8020218, 0x00000780);   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DH_ACT;
    rtd_outl(VGIP_VGIP_CHN1_ACT_HSTA_Width_reg,
                    VGIP_VGIP_CHN1_ACT_HSTA_Width_ch1_ih_act_sta(0)|
                    VGIP_VGIP_CHN1_ACT_HSTA_Width_ch1_ih_act_wid(value_u16));   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg

    //rtd_outl(0xb802021c, 0x00000014);   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DV_ACT;
    rtd_outl(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg,
                    VGIP_VGIP_CHN1_ACT_VSTA_Length_ch1_iv_act_sta(0)|
                    VGIP_VGIP_CHN1_ACT_VSTA_Length_ch1_iv_act_len(value_u16));   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg

    rtd_outl(0xb8020224, 0x20007900);   // VGIP_VGIP_CHN1_MISC_reg
    rtd_outl(0xb8020210, 0x80000003);   // VGIP_VGIP_CHN1_CTRL_reg

    // [ADD]
    //rtd_outl(0xb8020254, 0x00000897);   // VGIP_ICH1_VGIP_HTOTAL_reg
    // (Channel 1) Video_Processor_H_total: Register value = real value-1
    total_h = pScalerHdmiTimingInfo->DTG_DH_ACT + pScalerHdmiTimingInfo->DTG_H_PORCH -1;
    rtd_outl(VGIP_ICH1_VGIP_HTOTAL_reg, VGIP_ICH1_VGIP_HTOTAL_ch1_htotal_num(total_h));
}



void HDMITX_DV_Script_hdmirx_hdmi21_1080p60_cfg(void)
{

    // from hdmi21 1080p60 scaling to 4k2k60
    // HDMIRX 20;
    rtd_outl(0xb8040508, 0x00000001);   // HDMI_P0_FW_FUNC_reg
    rtd_outl(0xb804062c, 0x02200000);   // HDMI_P0_hdmi_clken3_reg

    // HDMIRX 21;
    rtd_outl(0xb8041000, 0x000000e1);   // HDMI21_P0_HD21_CR_reg
    //if(ucLaneNum > 3) // lane_mode_fw: 0: 3 lane mode, 1: 4 lane mode
    //    rtd_maskl(HDMI21_P0_HD21_CR_reg, ~HDMI21_P0_HD21_CR_lane_mode_fw_mask, HDMI21_P0_HD21_CR_lane_mode_fw(1));

    rtd_outl(0xb804127c, 0x01008000);   // HDMI21_P0_HD21_HDMI_VCR_reg
    rtd_outl(0xb8041020, 0x19232000);   // HDMI21_P0_hd21_channel_align_b_reg
    rtd_outl(0xb804101c, 0x19232000);   // HDMI21_P0_hd21_channel_align_g_reg
    rtd_outl(0xb8041018, 0x19232000);   // HDMI21_P0_hd21_channel_align_r_reg
    rtd_outl(0xb8041014, 0x19232000);   // HDMI21_P0_hd21_channel_align_c_reg

    // HDMIRX 20;
    rtd_outl(0xb80401a8, 0x00000817);   // HDMI_P0_HDMI_CTS_FIFO_CTL_reg
    rtd_outl(0xb8040198, 0x02a54a95);   // HDMI_P0_PHY_FIFO_CR0_reg

    // HDMI_COMBOEX
    rtd_outl(0xb8037004, 0x00000007);   // HDMI_COMBOEX_EX_PHY_FIFO_CR1_reg
    rtd_outl(0xb8037018, 0x00000008);   // HDMI_COMBOEX_EX_PHY_FIFO_CR3_reg
    rtd_outl(0xb8037000, 0x00000015);   // HDMI_COMBOEX_EX_PHY_FIFO_CR0_reg
    rtd_outl(0xb8037014, 0x00000000);   // HDMI_COMBOEX_EX_PHY_FIFO_CR2_reg

    // HDMIRX 21/20;
    rtd_outl(0xb8041034, 0x00000001);   // HDMI21_P0_hd21_ch_sync_b_reg
    rtd_outl(0xb8041030, 0x00000001);   // HDMI21_P0_hd21_ch_sync_g_reg
    rtd_outl(0xb804102c, 0x00000001);   // HDMI21_P0_hd21_ch_sync_r_reg
    rtd_outl(0xb804062c, 0x22200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb8040404, 0x00202855);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202845);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202855);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202805);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040420, 0x0000e5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040410, 0x00000003);   // HDMI_P0_MN_SCLKG_CTRL_reg
    rtd_outl(0xb8040420, 0x0000c5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040424, 0x00340000);   // HDMI_P0_HDMI_FDPLL_CTR1_reg
    rtd_outl(0xb8040428, 0x19804403);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040414, 0x00000000);   // HDMI_P0_MN_SCLKG_DIVS_reg
    rtd_outl(0xb8040410, 0x00000013);   // HDMI_P0_MN_SCLKG_CTRL_reg
    rtd_outl(0xb8040428, 0x1980440b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040420, 0x000085ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040428, 0x0980440b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040428, 0x0980441b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040420, 0x0000c5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040420, 0x0000c5e2);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040420, 0x0000c5e2);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040524, 0x00000009);   // HDMI_P0_hdmi_20_21_ctrl_reg
    rtd_outl(0xb804062c, 0x02200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb8040624, 0x1fe40000);   // HDMI_P0_hdmi_rst3_reg
    rtd_outl(0xb8040624, 0x3fe40000);   // HDMI_P0_hdmi_rst3_reg
    rtd_outl(0xb804050c, 0x00000000);   // HDMI_P0_PORT_SWITCH_reg
    rtd_outl(0xb804062c, 0x22200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb804114c, 0x00009601);   // HDMI21_P0_HD21_TIMING_GEN_CR_reg
    rtd_outl(0xb804127c, 0x21008060);   // HDMI21_P0_HD21_HDMI_VCR_reg
    rtd_outl(0xb80412b8, 0x00000001);   // HDMI21_P0_HD21_HDMI_AUDCR_reg

    // enable all EMP packet detect
    rtd_maskl(HDMI21_P0_HD21_fapa_ct_tmp_reg, ~HDMI21_P0_HD21_fapa_ct_tmp_all_mode_mask, HDMI21_P0_HD21_fapa_ct_tmp_all_mode(1));
    rtd_maskl(HDMI_P0_em_ct_reg, ~HDMI_P0_em_ct_hdr_hd20_hd21_sel_mask, HDMI_P0_em_ct_hdr_hd20_hd21_sel(1));

    // clear vrr memory
    rtd_maskl(HDMI21_P0_HD21_HDMI_VRR_EMC_reg, ~HDMI21_P0_HD21_HDMI_VRR_EMC_vrr_clr_mask, HDMI21_P0_HD21_HDMI_VRR_EMC_vrr_clr(1));
    rtd_maskl(HDMI21_P0_HD21_HDMI_VRR_EMC_reg, ~HDMI21_P0_HD21_HDMI_VRR_EMC_vrr_clr_mask, HDMI21_P0_HD21_HDMI_VRR_EMC_vrr_clr(0));


    return;
}



void HDMITX_DV_Script_TC_Repeater_hdmi21_rxBypassToTx(void)
{
    NoteMessageHDMITx("[HDMITX] hdmi21_rxBypassToTx...\n");

    // [CRT] init
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_crt_init();

    // [RX] HDMIRX bypass to TX output disable
    if(ENABLE_LOAD_HDMIRX_SCRIPT == 1)
        HDMITX_DV_Script_HDMIRX_pktBypassToTx_enable(_DISABLE);

    // [HDMITX] PHY -- disable apply
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_hdmitx_phy_clr_clkratio();

    // [Scaler] DTG output mode
    HDMITX_DV_Script_Disp_DTG_outputMuxSel(1);

    // [Scaler] VGIP
    HDMITX_DV_Script_Disp_VGIP_cfg();

    if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
        // [HDMITX] On-Region Lane source sel
        HDMITX_DV_Script_hdmitx21_on_region_source_sel(0xf);

        // [HDMITX] from hdmi21 1080p60 ptg
        HDMITX_DV_Script_hdmitx21_hdmitx_phy_pll();
#if 1 // [disp] dtg
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_disp_dtg();
#else // [Scaler] from 1080p60 bypass mode IMD script
        imd_1080p60_bypass_no_srnnprepost_new();
#endif
    }

    if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
        // [HDMITX] On-Region Lane source sel
        HDMITX_DV_Script_hdmitx21_on_region_source_sel(0x0);

        // [HDMITX] from hdmi21 tx 1080p60 ouput mode
        HDMITX_DV_Script_hdmitx21_mac_config();
    }

    // [HDMIRX] hdmi21 1080p60 config
    if(ENABLE_LOAD_HDMIRX_SCRIPT == 1)
        HDMITX_DV_Script_hdmirx_hdmi21_1080p60_cfg();

    // [HDMITX] HDMI21 TX MAC enable
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
        HDMITX_DV_Script_hdmitx21_mac_enable(_ENABLE);

        // [Scaler] OutTG enable
        HDMITX_DV_Script_Disp_OutTG_dispEn();
        msleep(30);

        // [TX] FIFO reset
        HDMITX_DV_Script_hdmitx21_reset_fifoErr();

        // [HDMITX] MISC HD/EMP/Packet bypass
        HDMITX_DV_Script_hdmitx_pktHdrBypass_config();
    }

    // [HDMIRX] packet bypass to TX output enable
    if(ENABLE_LOAD_HDMIRX_SCRIPT == 1)
        HDMITX_DV_Script_HDMIRX_pktBypassToTx_enable(_ENABLE);

    // [HDMITX] NCTS regen config
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_hdmitx_NCTS_REGEN_config();

    // [HDMIRX] 21 color control register config
    if(ENABLE_LOAD_HDMIRX_SCRIPT == 1)
        HDMITX_DV_Script_HDMIRX_colorControlRegister_config();

    // [TX] DSC SRC SEL
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
        // [HDMITX] PHY DSC INSEL/DispD FIFO Enable
        // mode: 0 => 2pto4p mode for HDMI21, 1 => bypass mode for HDMI20
        HDMITX_DV_Script_hdmitx_dsc_src_sel(0); // 1: bypass mode
    }

    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_hdmitx_frame_rate_measure_en(_ENABLE);

#if 1 // TEST
    NoteMessageHDMITx("[HDMITX] dpc_sm_rd_start_pos=%d\n", (UINT32)HDMITX21_MAC_HDMI21_DPC_2_dpc_sm_rd_start_pos(rtd_inl(HDMITX21_MAC_HDMI21_DPC_2_reg)));
#endif
    NoteMessageHDMITx("[HDMITX] pkt_bypass=%x\n", (UINT32)rtd_inl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg));
    NoteMessageHDMITx("[HDMITX] hdmi21_rxBypassToTx Done!\n");
    return;
}



#ifdef NOT_USED
void HDMITX_DV_Script_TC_Repeater_hdmi21_1080p60_ScalingTo_4k2kp60(void)
{
    NoteMessageHDMITx("[HDMITX] hdmi21_1080p60_ScalingTo_4k2kp60...\n");

    ///////////////////////
    // CRT
    rtd_outl(0xb8000120, 0x00200201);   // SYS_REG_SYS_SRST4_reg
    rtd_outl(0xb8000114, 0x00000601);   // SYS_REG_SYS_CLKEN1_reg
    rtd_outl(0xb8000130, 0x0040007d);   // SYS_REG_SYS_CLKEN4_reg
    rtd_outl(0xb8000138, 0x00004001);   // SYS_REG_SYS_CLKEN6_reg
    rtd_outl(0xb8000234, 0x00000000);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg
    rtd_outl(0xb8000204, 0x10002400);   // SYS_REG_SYS_CLKSEL_reg

    // SRNN PLL SEL
    // [X] Srnn_ip_clk pll mux sel: 0: use d_clk source, 1: use SRNN_PLL
    rtd_outl(0xb8077420, 0x00000001);   // SRNN_SRNN_PLL_SEL_reg

    // SYS/PLL REG
    rtd_outl(0xb8000120, 0x00000005);   // SYS_REG_SYS_SRST4_reg
    rtd_outl(0xb8000464, 0x00000004);   // PLL_REG_SYS_PLL_SRNN2_reg
    rtd_outl(0xb8000460, 0x00014004);   // PLL_REG_SYS_PLL_SRNN1_reg
    rtd_outl(0xb8000464, 0x00000005);   // PLL_REG_SYS_PLL_SRNN2_reg
    rtd_outl(0xb8000464, 0x00000007);   // PLL_REG_SYS_PLL_SRNN2_reg
    rtd_outl(0xb8000464, 0x00000003);   // PLL_REG_SYS_PLL_SRNN2_reg
    rtd_outl(0xb8000120, 0x00700001);   // SYS_REG_SYS_SRST4_reg
    rtd_outl(0xb8000114, 0x00000601);   // SYS_REG_SYS_CLKEN1_reg
    rtd_outl(0xb8000130, 0x00400041);   // SYS_REG_SYS_CLKEN4_reg

    // CRT DCLK Gate Sel
    rtd_outl(0xb8000234, 0x00000000);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg
    rtd_outl(0xb8000204, 0x10002400);   // SYS_REG_SYS_CLKSEL_reg
    rtd_outl(0xb8000234, 0x00010000);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg
    rtd_outl(0xb8000234, 0x00011000);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg

    // HDMITX PHY
    rtd_outl(0xb8017858, 0x00004000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg

    // HDMIRX output disable
    rtd_outl(0xb8040210, 0x00000006);   // HDMI_P0_HDMI_PKT_reg
    rtd_outl(0xb8041360, 0x00000006);   // HDMI21_P0_HD21_HDMI_PKT_reg

    // HDMITX On-Region Lane source sel
    rtd_outl(0xb8018044, 0x0000000f);   // HDMITX21_ON_REGION_HDMI21_PRBS_CTRL_0_reg

    // HDMITX PHY
    rtd_outl(0xb8017800, 0x01001000);   // HDMITX_PHY_HDMITXPHY_CTRL00_reg
    rtd_outl(0xb8017808, 0x10124120);   // HDMITX_PHY_HDMITXPHY_CTRL02_reg
    rtd_outl(0xb801780c, 0x00000200);   // HDMITX_PHY_HDMITXPHY_CTRL03_reg
    rtd_outl(0xb8017814, 0x00000000);   // HDMITX_PHY_HDMITXPHY_CTRL05_reg
    rtd_outl(0xb8017818, 0x00022210);   // HDMITX_PHY_HDMITXPHY_CTRL06_reg
    rtd_outl(0xb801781c, 0x00010001);   // HDMITX_PHY_HDMITXPHY_CTRL07_reg
    rtd_outl(0xb8017820, 0x03312100);   // HDMITX_PHY_HDMITXPHY_CTRL08_reg
    rtd_outl(0xb8017828, 0x10050003);   // HDMITX_PHY_HDMITXPHY_CTRL10_reg
    rtd_outl(0xb8017830, 0x00004000);   // HDMITX_PHY_HDMITXPHY_CTRL12_reg
    rtd_outl(0xb8017844, 0x10001111);   // HDMITX_PHY_HDMITXPHY_CTRL17_reg
    rtd_outl(0xb801785c, 0xccb34471);   // HDMITX_PHY_HDMITX_TMDS_CTRL0_reg
    rtd_outl(0xb80178b0, 0x13000000);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg
    rtd_outl(0xb80178b4, 0x97200037);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg
    rtd_outl(0xb80178ac, 0x00003ff0);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg
    rtd_outl(0xb8017804, 0x00000000);   // HDMITX_PHY_HDMITXPHY_CTRL01_reg
    rtd_outl(0xb8017824, 0x00012010);   // HDMITX_PHY_HDMITXPHY_CTRL09_reg
    rtd_outl(0xb8017858, 0x80604000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg
    rtd_outl(0xb8017858, 0x80664000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg
    rtd_outl(0xb80178ac, 0x00003ff1);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg
    rtd_outl(0xb80178ac, 0x00003ff0);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg

    // HDMITX HDMI21 MAc
    rtd_outl(0xb8018410, 0x00000080);   // HDMITX21_MAC_HDMI21_IN_CONV_0_reg,
    rtd_outl(0xb80187c0, 0x000000f8);   // HDMITX21_MAC_HDMI21_ENC_0_reg

    // HDMITX On-Region Lane source sel
    rtd_outl(0xb8018044, 0x00000000);   // HDMITX21_ON_REGION_HDMI21_PRBS_CTRL_0_reg

    // HDMITX HDMI21 MAc
    // [@] video size need change
    rtd_outl(0xb8018418, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_2_reg
    rtd_outl(0xb801841c, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_3_reg
    rtd_outl(0xb8018420, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_4_reg
    rtd_outl(0xb8018424, 0x00000028);   // HDMITX21_MAC_HDMI21_IN_CONV_5_reg
    rtd_outl(0xb8018428, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_6_reg
    rtd_outl(0xb801842c, 0x00000011);   // HDMITX21_MAC_HDMI21_IN_CONV_7_reg
    rtd_outl(0xb8018430, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_8_reg
    rtd_outl(0xb8018434, 0x00000014);   // HDMITX21_MAC_HDMI21_IN_CONV_9_reg
    rtd_outl(0xb8018590, 0x000000aa);   // HDMITX21_MAC_HDMI21_SCHEDULER_4_reg
    rtd_outl(0xb801858c, 0x00000000);   // HDMITX21_MAC_HDMI21_SCHEDULER_3_reg
    rtd_outl(0xb8018594, 0x00000001);   // HDMITX21_MAC_HDMI21_SCHEDULER_5_reg
    rtd_outl(0xb8018598, 0x000000ea);   // HDMITX21_MAC_HDMI21_SCHEDULER_6_reg
    rtd_outl(0xb80185ac, 0x000000f8);   // HDMITX21_MAC_HDMI21_SCHEDULER_10_reg
    rtd_outl(0xb80185b4, 0x0000008c);   // HDMITX21_MAC_HDMI21_SCHEDULER_13_reg
    rtd_outl(0xb801859c, 0x0000000b);   // HDMITX21_MAC_HDMI21_SCHEDULER_15_reg
    rtd_outl(0xb8018490, 0x00000020);   // HDMITX21_MAC_HDMI21_422_CONV_reg
    rtd_outl(0xb80184c4, 0x000000a0);   // HDMITX21_MAC_HDMI21_DPC_0_reg
    rtd_outl(0xb80184cc, 0x00000008);   // HDMITX21_MAC_HDMI21_DPC_2_reg
    rtd_outl(0xb80184d0, 0x00000020);   // HDMITX21_MAC_HDMI21_DPC_3_reg
    rtd_outl(0xb80184e4, 0x000000c0);   // HDMITX21_MAC_HDMI21_FORMAT_TOP_reg
    rtd_outl(0xb8018588, 0x000000a0);   // HDMITX21_MAC_HDMI21_SCHEDULER_2_reg
    rtd_outl(0xb8018580, 0x00000091);   // HDMITX21_MAC_HDMI21_SCHEDULER_0_reg
    rtd_outl(0xb80186a8, 0x00000001);   // HDMITX21_MAC_HDCP22_ENGINE_40_reg
    rtd_outl(0xb80186c0, 0x000000f0);   // HDMITX21_MAC_HDMI21_RC_reg
    rtd_outl(0xb80186cc, 0x00000020);   // HDMITX21_MAC_HDMI21_FRL_0_reg
    rtd_outl(0xb8018780, 0x00000080);   // HDMITX21_MAC_HDMI21_RS_0_reg
    rtd_outl(0xb8018790, 0x000000f0);   // HDMITX21_MAC_HDMI21_SCR_0_reg
    rtd_outl(0xb80187c0, 0x000000f8);   // HDMITX21_MAC_HDMI21_ENC_0_reg

    // HDMITX PHY DSC INSEL/DispD FIFO Enable);
    rtd_outl(0xb80178f8, 0x00000006);   // HDMITX_PHY_DSC_SRC_SEL_reg

    // [X] HDMITX MISC NCTS Regen
    rtd_outl(0xb8019524, 0x00ff0820);   // HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg
    rtd_outl(0xb8019528, 0x080007e0);   // HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg
    rtd_outl(0xb801952c, 0x01000001);   // HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg
    rtd_outl(0xb8019520, 0x71000fa0);   // HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg

    // HDMITX HDMI21 MAC enable);
    rtd_outl(0xb801840c, 0x00000080);   // HDMITX21_MAC_HDMI21_TX_CTRL_reg
    rtd_outl(0xb80186cc, 0x000000a0);   // HDMITX21_MAC_HDMI21_FRL_0_reg

    // Scaler Peaking SDNRs
    rtd_outl(0xb8023ce8, 0x00000001);   // COLOR_MB_PEAKING_SDNR_4K_120_MODE_reg

    // Scaler DTG VLC
    rtd_outl(0xb8027208, 0xc0000006);   // PPOVERLAY_DTG_LC_reg

    // CRT DCLK Gate Sel
    rtd_outl(0xb8000230, 0x20011011);   // SYS_REG_SYS_DCLK_GATE_SEL0_reg
    rtd_outl(0xb8000234, 0x00011011);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg

    // Scaler DTG Gate Sel
    rtd_outl(0xb8027300, 0x80000003);   // PPOVERLAY_d_clk_porch_gated_reg

    // Scaler SRNN_PATGEN, SRNN UZU
    rtd_outl(0xb8025804, 0x07800870);   // SRNN_PRE_POST_D_SRNN_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb8025604, 0x07800000);   // SRNN_PRE_POST_SRNN_PRE_POS_UZU_Input_Size_reg
    rtd_outl(0xb8025804, 0x07800014);   // SRNN_PRE_POST_D_SRNN_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb8025604, 0x07800014);   // SRNN_PRE_POST_SRNN_PRE_POS_UZU_Input_Size_reg

    // [X] Scaler two step UZU
    rtd_outl(0xb8029b2c, 0x07800000);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg
    rtd_outl(0xb8029b2c, 0x07800014);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg
    rtd_outl(0xb8029604, 0x07800870);   // TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb8029604, 0x07800014);   // TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg

    // [X] Scaler DTG/SRNN
    rtd_outl(0xb8025600, 0x00000002);   // SRNN_PRE_POST_SRNN_PRE_POS_UZU_SEL_reg
    rtd_outl(0xb8027420, 0x00580002);   // PPOVERLAY_DTG_two_step_uzu_timing_reg
    rtd_outl(0xb8029b2c, 0x07800014);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg
    rtd_outl(0xb8029b2c, 0x07800014);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Input_Size_reg
    rtd_outl(0xb8029604, 0x07800014);   // TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb8029604, 0x07800014);   // TWO_STEP_UZU_D_TWO_STEP_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb8029b24, 0x00080000);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Scale_Hor_Factor_reg
    rtd_outl(0xb8029b28, 0x00080000);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Scale_Ver_Factor_reg
    rtd_outl(0xb8029b20, 0x002a8031);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_reg
    rtd_outl(0xb8029b20, 0x002a8033);   // TWO_STEP_UZU_DM_TWO_STEP_SR_Ctrl_reg

    // [X] Scaler DISP DS444 CRC
    rtd_outl(0xb8023b1c, 0x00000003);   // DISPD_DS444_DISPD_DS444_ACC_CRC_CRC_Ctrl_reg

    // [X] Scaler D1 UZU
    rtd_outl(0xb8029240, 0x00280f00);   // D1_UZD_D1_IBUFF_CTRLI_BUFF_CTRL_reg
    rtd_outl(0xb802920c, 0x00000000);   // D1_UZD_D1_UZD_Scale_Hor_Factor_reg
    rtd_outl(0xb8029210, 0x00000000);   // D1_UZD_D1_UZD_Scale_Ver_Factor_reg
    rtd_outl(0xb8029204, 0x01000300);   // D1_UZD_D1_UZD_Ctrl0_reg

    // Scalr FSBUf
    rtd_outl(0xb8023420, 0x00000011);
    rtd_outl(0xb8023424, 0x00000000);

    // [X] HDR to SDR
    rtd_outl(0xb8082c00, 0x80000000);   // HDR2SDR_HDR2SDR_CTRL_reg

    // [X] Scaler DTG
    rtd_outl(0xb8027004, 0x0000001f);   // PPOVERLAY_DH_Width_reg
    rtd_outl(0xb8027008, 0x112f0000);   // PPOVERLAY_DH_Total_Length_reg
    rtd_outl(0xb802700c, 0x00000002);   // PPOVERLAY_DV_Length_reg
    rtd_outl(0xb8027010, 0x0000004f);   // PPOVERLAY_DV_total_reg
    rtd_outl(0xb8027014, 0x00f00ff0);   // PPOVERLAY_DH_DEN_Start_End_reg
    rtd_outl(0xb8027018, 0x0014003c);   // PPOVERLAY_DV_DEN_Start_End_reg
    rtd_outl(0xb8027000, 0x00000010);   // PPOVERLAY_Display_Timing_CTRL1_reg
    rtd_outl(0xb8027000, 0x80000011);   // PPOVERLAY_Display_Timing_CTRL1_reg

    // Scaler OutTG
    rtd_outl(0xb8027804, 0x0000001f);   // PPOVERLAY_OUTTG_OUTTG_DH_Width_reg
    rtd_outl(0xb8027808, 0x112f0000);   // PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg
    rtd_outl(0xb802780c, 0x00000002);   // PPOVERLAY_OUTTG_OUTTG_DV_Length_reg
    rtd_outl(0xb8027810, 0x0000004f);   // PPOVERLAY_OUTTG_OUTTG_DV_total_reg
    rtd_outl(0xb8027814, 0x00f00ff0);   // PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg
    rtd_outl(0xb8027818, 0x0014003c);   // PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg
    rtd_outl(0xb8027840, 0x00f00ff0);   // PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg
    rtd_outl(0xb8027844, 0x0014003c);   // PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg
    rtd_outl(0xb8027800, 0x90000010);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
    rtd_outl(0xb8027840, 0x00f00ff0);   // PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg
    rtd_outl(0xb8027844, 0x0014003c);   // PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg

    // Scaler OutTG enable
    rtd_outl(0xb8027800, 0x90000011);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg

    // [X] Scaler Scaling down
    rtd_outl(0xb8085240, 0x00280780);   // SCALEDOWN_ICH1_IBUFF_CTRLI_BUFF_CTRL_reg
    rtd_outl(0xb8085204, 0x0100030b);   // SCALEDOWN_ICH1_UZD_Ctrl0_reg
    rtd_outl(0xb808520c, 0x00200444);   // SCALEDOWN_ICH1_UZD_Scale_Hor_Factor_reg
    rtd_outl(0xb8085210, 0x0021af28);   // SCALEDOWN_ICH1_UZD_Scale_Ver_Factor_reg

    // [X] Scaler UZU
    rtd_outl(0xb8089028, 0x07800000);   // SCALEUP_DM_UZU_Input_Size_reg
    rtd_outl(0xb8089028, 0x07800014);   // SCALEUP_DM_UZU_Input_Size_reg
    rtd_outl(0xb80890c8, 0x07800870);   // SCALEUP_DM_UZU_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb80890c8, 0x07800014);   // SCALEUP_DM_UZU_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb8089128, 0x07800000);   // SCALEUP_CH_UZU_Input_Size_reg
    rtd_outl(0xb8089128, 0x0780000a);   // SCALEUP_CH_UZU_Input_Size_reg
    rtd_outl(0xb80891c8, 0x07800870);   // SCALEUP_CH_UZU_PATGEN_Globle_Ctrl1_reg
    rtd_outl(0xb80891c8, 0x0780000a);   // SCALEUP_CH_UZU_PATGEN_Globle_Ctrl1_reg

    // [X] Scaler DC2H DTG
    rtd_outl(0xb8088000, 0x70000310);   // PPOVERLAY_DC2HLK_DC2HLK_Display_Timing_CTRL_reg
    rtd_outl(0xb8088108, 0x007807f8);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE1_DH_DEN_Start_End_reg
    rtd_outl(0xb8088100, 0x00000897);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE1_DH_Total_Last_Line_Length_reg
    rtd_outl(0xb808810c, 0x000a001e);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE1_DV_DEN_Start_End_reg
    rtd_outl(0xb8088104, 0x00000026);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE1_DV_total_reg
    rtd_outl(0xb8088008, 0xc0100000);   // PPOVERLAY_DC2HLK_DC2HLK_XTAL_VSYNC_reg
    rtd_outl(0xb8088008, 0xc010000a);   // PPOVERLAY_DC2HLK_DC2HLK_XTAL_VSYNC_reg
    rtd_outl(0xb808800c, 0x00000009);   // PPOVERLAY_DC2HLK_DC2HLK_DATA_FINISH_reg
    rtd_outl(0xb8088208, 0x007807f8);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE2_DH_DEN_Start_End_reg
    rtd_outl(0xb8088200, 0x00000897);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE2_DH_Total_Last_Line_Length_reg
    rtd_outl(0xb808820c, 0x0005000f);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE2_DV_DEN_Start_End_reg
    rtd_outl(0xb8088204, 0x00000012);   // PPOVERLAY_DC2HLK_DC2HLK_PLANE2_DV_total_reg
    rtd_outl(0xb808800c, 0x00000004);   // PPOVERLAY_DC2HLK_DC2HLK_DATA_FINISH_reg
    rtd_outl(0xb8088008, 0xc0102d71);   // PPOVERLAY_DC2HLK_DC2HLK_XTAL_VSYNC_reg
    rtd_outl(0xb808800c, 0x00000009);   // PPOVERLAY_DC2HLK_DC2HLK_DATA_FINISH_reg
    rtd_outl(0xb8088008, 0xc0103eb6);   // PPOVERLAY_DC2HLK_DC2HLK_XTAL_VSYNC_reg
    rtd_outl(0xb8088008, 0xc0003eb6);   // PPOVERLAY_DC2HLK_DC2HLK_XTAL_VSYNC_reg
    rtd_outl(0xb8088000, 0x71000310);   // PPOVERLAY_DC2HLK_DC2HLK_Display_Timing_CTRL_reg

    // Scaler VGIP
    rtd_outl(0xb8020210, 0x00000003);   // VGIP_VGIP_CHN1_CTRL_reg
    rtd_outl(0xb8020218, 0x00000780);   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg
    rtd_outl(0xb802021c, 0x00000014);   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg
    rtd_outl(0xb8020224, 0x20007900);   // VGIP_VGIP_CHN1_MISC_reg
    rtd_outl(0xb8020210, 0x80000003);   // VGIP_VGIP_CHN1_CTRL_reg

    // CRT
    rtd_outl(0xb8000104, 0x00000401);   // SYS_REG_SYS_SRST1_reg
    rtd_outl(0xb8000114, 0x00000401);   // SYS_REG_SYS_CLKEN1_reg
    rtd_outl(0xb8000114, 0x00000601);   // SYS_REG_SYS_CLKEN1_reg
    rtd_outl(0xb8000130, 0x0040007d);   // SYS_REG_SYS_CLKEN4_reg

    // DCLK_GATE_SEL
    rtd_outl(0xb8000234, 0x00011011);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg

    // PinMux
    rtd_outl(0xb8060214, 0x3030f0f0);   // PINMUX_MAIN_ST_GPIO_ST_CFG_2_reg

    // HDMIRX 20;
    rtd_outl(0xb8040508, 0x00000001);   // HDMI_P0_FW_FUNC_reg
    rtd_outl(0xb804062c, 0x02200000);   // HDMI_P0_hdmi_clken3_reg

    // HDMIRX 21;
    rtd_outl(0xb8041000, 0x000000e1);   // HDMI21_P0_HD21_CR_reg
    rtd_outl(0xb804127c, 0x01008000);   // HDMI21_P0_HD21_HDMI_VCR_reg
    rtd_outl(0xb8041020, 0x19232000);   // HDMI21_P0_hd21_channel_align_b_reg
    rtd_outl(0xb804101c, 0x19232000);   // HDMI21_P0_hd21_channel_align_g_reg
    rtd_outl(0xb8041018, 0x19232000);   // HDMI21_P0_hd21_channel_align_r_reg
    rtd_outl(0xb8041014, 0x19232000);   // HDMI21_P0_hd21_channel_align_c_reg

    // HDMIRX 20;
    rtd_outl(0xb80401a8, 0x00000817);   // HDMI_P0_HDMI_CTS_FIFO_CTL_reg
    rtd_outl(0xb8040198, 0x02a54a95);   // HDMI_P0_PHY_FIFO_CR0_reg

    // HDMI_COMBOEX
    rtd_outl(0xb8037004, 0x00000007);   // HDMI_COMBOEX_EX_PHY_FIFO_CR1_reg
    rtd_outl(0xb8037018, 0x00000008);   // HDMI_COMBOEX_EX_PHY_FIFO_CR3_reg
    rtd_outl(0xb8037000, 0x00000015);   // HDMI_COMBOEX_EX_PHY_FIFO_CR0_reg
    rtd_outl(0xb8037014, 0x00000000);   // HDMI_COMBOEX_EX_PHY_FIFO_CR2_reg

    // HDMIRX 21/20;
    rtd_outl(0xb8041034, 0x00000001);   // HDMI21_P0_hd21_ch_sync_b_reg
    rtd_outl(0xb8041030, 0x00000001);   // HDMI21_P0_hd21_ch_sync_g_reg
    rtd_outl(0xb804102c, 0x00000001);   // HDMI21_P0_hd21_ch_sync_r_reg
    rtd_outl(0xb804062c, 0x22200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb8040404, 0x00202855);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202845);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202855);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202805);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040420, 0x0000e5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040410, 0x00000003);   // HDMI_P0_MN_SCLKG_CTRL_reg
    rtd_outl(0xb8040420, 0x0000c5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040424, 0x00340000);   // HDMI_P0_HDMI_FDPLL_CTR1_reg
    rtd_outl(0xb8040428, 0x19804403);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040414, 0x00000000);   // HDMI_P0_MN_SCLKG_DIVS_reg
    rtd_outl(0xb8040410, 0x00000013);   // HDMI_P0_MN_SCLKG_CTRL_reg
    rtd_outl(0xb8040428, 0x1980440b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040420, 0x000085ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040428, 0x0980440b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040428, 0x0980441b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040420, 0x0000c5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040420, 0x0000c5e2);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040420, 0x0000c5e2);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040524, 0x00000009);   // HDMI_P0_hdmi_20_21_ctrl_reg
    rtd_outl(0xb804062c, 0x02200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb8040624, 0x1fe40000);   // HDMI_P0_hdmi_rst3_reg
    rtd_outl(0xb8040624, 0x3fe40000);   // HDMI_P0_hdmi_rst3_reg
    rtd_outl(0xb804050c, 0x00000000);   // HDMI_P0_PORT_SWITCH_reg
    rtd_outl(0xb804062c, 0x22200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb804114c, 0x00009601);   // HDMI21_P0_HD21_TIMING_GEN_CR_reg
    rtd_outl(0xb804127c, 0x21008060);   // HDMI21_P0_HD21_HDMI_VCR_reg
    rtd_outl(0xb80412b8, 0x00000001);   // HDMI21_P0_HD21_HDMI_AUDCR_reg

    // Scaler DTG VLC
    rtd_outl(0xb8027104, 0x00000004);   // PPOVERLAY_DTG_pending_status_reg
    rtd_outl(0xb8027208, 0xc000000a);   // PPOVERLAY_DTG_LC_reg
    rtd_outl(0xb8027104, 0x00000004);   // PPOVERLAY_DTG_pending_status_reg
    rtd_outl(0xb8027104, 0x00000004);   // PPOVERLAY_DTG_pending_status_reg
    rtd_outl(0xb8027208, 0xc000000a);   // PPOVERLAY_DTG_LC_reg
    rtd_outl(0xb8027104, 0x00000004);   // PPOVERLAY_DTG_pending_status_reg

    // HDMITX MISC HD/EMP/Packet bypass
    rtd_outl(0xb8019700, 0x00000001);   // HDMITX_MISC_HD_BYPASS_CTRL00_reg
    rtd_outl(0xb8019704, 0x000003ff);   // HDMITX_MISC_HD_BYPASS_CTRL01_reg
    rtd_outl(0xb801970c, 0x83090285);   // HDMITX_MISC_HD_BYPASS_CTRL03_reg
    rtd_outl(0xb8019744, 0x00000001);   // HDMITX_MISC_HD_EMP_CTL_EN_reg
    rtd_outl(0xb8019550, 0x00000122);   // HDMITX_MISC_PACKET_PATH_CTRL_00_reg

    // HDMIRX output enable);
    rtd_outl(0xb8040210, 0x00000007);   // HDMI_P0_HDMI_PKT_reg
    rtd_outl(0xb8041360, 0x00000007);   // HDMI21_P0_HD21_HDMI_PKT_reg


    NoteMessageHDMITx("[HDMITX] hdmi21_1080p60_ScalingTo_4k2kp60 Done\n");
    return;
}
#endif // #ifdef NOT_USED


#ifdef NOT_USED
void HDMITX_DV_Script_TC_Repeater_hdmi21_4kp30_rxBypassToTx(void)
{
    NoteMessageHDMITx("[HDMITX] hdmi21_4kp30_rxBypassToTx...\n");

    ///////////////////////
    // [v] CRT
    rtd_outl(0xb8000104, 0x00000401);   // SYS_REG_SYS_SRST1_reg
    rtd_outl(0xb8000114, 0x00000401);   // SYS_REG_SYS_CLKEN1_reg
    rtd_outl(0xb8000114, 0x00000601);   // SYS_REG_SYS_CLKEN1_reg
    rtd_outl(0xb8000130, 0x0040007d);   // SYS_REG_SYS_CLKEN4_reg
    rtd_outl(0xb8000234, 0x00001010);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg

    // [v] HDMIRX output disable
    rtd_outl(0xb8040210, 0x00000006);   // HDMI_P0_HDMI_PKT_reg
    rtd_outl(0xb8041360, 0x00000006);   // HDMI21_P0_HD21_HDMI_PKT_reg

    // [v] CRT
    rtd_outl(0xb8000120, 0x00700001);   // SYS_REG_SYS_SRST4_reg
    rtd_outl(0xb8000114, 0x00000601);   // SYS_REG_SYS_CLKEN1_reg
    rtd_outl(0xb8000130, 0x00400041);   // SYS_REG_SYS_CLKEN4_reg

    // [X] DCLK_GATE_SEL
    rtd_outl(0xb8000234, 0x00001010);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg
    rtd_outl(0xb8000204, 0x10202400);   // SYS_REG_SYS_CLKSEL_reg
    rtd_outl(0xb8000234, 0x00011010);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg
    rtd_outl(0xb8000234, 0x00011010);   // SYS_REG_SYS_DCLK_GATE_SEL1_reg

    // [X] HDMITX PHY
    rtd_outl(0xb8017858, 0x00004000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg

    // Scaler DTG
    // d1_output_mux_sel (mux before UZD): 0: through d1 PQ IP, 1: bypass path
    rtd_outl(0xb8027580, 0x00000001);   // PPOVERLAY_DTG_d1_output_mux_reg

    // PinMux
    rtd_outl(0xb8060208, 0xf0f0f030);   // PINMUX_MAIN_ST_ST2_SMT_0_reg
    rtd_outl(0xb806020c, 0x30303030);   // PINMUX_MAIN_ST_GPIO_ST_CFG_0_reg
    rtd_outl(0xb8060210, 0x30f0f0f0);   // PINMUX_MAIN_ST_GPIO_ST_CFG_1_reg

    // [X] Scaler VGIP
    rtd_outl(0xb8020284, 0x00002004);   // VGIP_Data_Path_Select_reg
    rtd_outl(0xb8020210, 0x00000003);   // VGIP_VGIP_CHN1_CTRL_reg
    rtd_outl(0xb8020218, 0x00000f00);   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg
    rtd_outl(0xb802021c, 0x00000006);   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg
    rtd_outl(0xb8020224, 0x20018000);   // VGIP_VGIP_CHN1_MISC_reg
    rtd_outl(0xb8020210, 0x80000003);   // VGIP_VGIP_CHN1_CTRL_reg
    rtd_outl(0xb8020254, 0x0000157b);   // VGIP_ICH1_VGIP_HTOTAL_reg

    // [V] HDMITX On-Region Lane source sel
    rtd_outl(0xb8018044, 0x0000000f);   // HDMITX21_ON_REGION_HDMI21_PRBS_CTRL_0_reg

    // [X] HDMITX PHY
    rtd_outl(0xb8017800, 0x01001000);   // HDMITX_PHY_HDMITXPHY_CTRL00_reg
    rtd_outl(0xb8017808, 0x10124120);   // HDMITX_PHY_HDMITXPHY_CTRL02_reg
    rtd_outl(0xb801780c, 0x00000210);   // HDMITX_PHY_HDMITXPHY_CTRL03_reg
    rtd_outl(0xb8017814, 0x00000000);   // HDMITX_PHY_HDMITXPHY_CTRL05_reg
    rtd_outl(0xb8017818, 0x00022210);   // HDMITX_PHY_HDMITXPHY_CTRL06_reg
    rtd_outl(0xb801781c, 0x00010001);   // HDMITX_PHY_HDMITXPHY_CTRL07_reg
    rtd_outl(0xb8017820, 0x03312100);   // HDMITX_PHY_HDMITXPHY_CTRL08_reg
    rtd_outl(0xb8017828, 0x10050003);   // HDMITX_PHY_HDMITXPHY_CTRL10_reg
    rtd_outl(0xb8017830, 0x00006000);   // HDMITX_PHY_HDMITXPHY_CTRL12_reg
    rtd_outl(0xb8017844, 0x10001111);   // HDMITX_PHY_HDMITXPHY_CTRL17_reg
    rtd_outl(0xb801785c, 0xccb18639);   // HDMITX_PHY_HDMITX_TMDS_CTRL0_reg
    rtd_outl(0xb80178b0, 0x13000000);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg
    rtd_outl(0xb80178b4, 0x97200037);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg
    rtd_outl(0xb80178ac, 0x00003ff0);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg
    rtd_outl(0xb8017804, 0x00000000);   // HDMITX_PHY_HDMITXPHY_CTRL01_reg
    rtd_outl(0xb8017824, 0x00012010);   // HDMITX_PHY_HDMITXPHY_CTRL09_reg
    rtd_outl(0xb8017858, 0x80604000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg
    rtd_outl(0xb8017858, 0x80664000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg
    rtd_outl(0xb80178ac, 0x00003ff1);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg
    rtd_outl(0xb80178ac, 0x00003ff0);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg

    // [X] Scaler OutTG
    rtd_outl(0xb8027804, 0x0000001f);   // PPOVERLAY_OUTTG_OUTTG_DH_Width_reg
    rtd_outl(0xb8027808, 0x157b0000);   // PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg
    rtd_outl(0xb802780c, 0x00000002);   // PPOVERLAY_OUTTG_OUTTG_DV_Length_reg
    rtd_outl(0xb8027810, 0x00000011);   // PPOVERLAY_OUTTG_OUTTG_DV_total_reg
    rtd_outl(0xb8027814, 0x01801080);   // PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg
    rtd_outl(0xb8027818, 0x0008000e);   // PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg
    rtd_outl(0xb8027840, 0x01801080);   // PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg
    rtd_outl(0xb8027844, 0x0008000e);   // PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg
    rtd_outl(0xb8027800, 0x90000000);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
    rtd_outl(0xb8027804, 0x00000057);   // PPOVERLAY_OUTTG_OUTTG_DH_Width_reg
    rtd_outl(0xb802780c, 0x00000001);   // PPOVERLAY_OUTTG_OUTTG_DV_Length_reg
    rtd_outl(0xb8027800, 0x10000000);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg

    // [X] HDMITX HDMI21 MAC
    rtd_outl(0xb8018410, 0x00000080);   // HDMITX21_MAC_HDMI21_IN_CONV_0_reg,
    rtd_outl(0xb80187c0, 0x000000f8);   // HDMITX21_MAC_HDMI21_ENC_0_reg

    // [V] HDMITX On-Region Lane source sel
    rtd_outl(0xb8018044, 0x00000000);   // HDMITX21_ON_REGION_HDMI21_PRBS_CTRL_0_reg

    // [X] HDMITX HDMI21 MAC
    rtd_outl(0xb8018418, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_2_reg
    rtd_outl(0xb801841c, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_3_reg
    rtd_outl(0xb8018420, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_4_reg
    rtd_outl(0xb8018424, 0x00000006);   // HDMITX21_MAC_HDMI21_IN_CONV_5_reg
    rtd_outl(0xb8018428, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_6_reg
    rtd_outl(0xb801842c, 0x00000006);   // HDMITX21_MAC_HDMI21_IN_CONV_7_reg
    rtd_outl(0xb8018430, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_8_reg
    rtd_outl(0xb8018434, 0x00000006);   // HDMITX21_MAC_HDMI21_IN_CONV_9_reg
    rtd_outl(0xb8018590, 0x0000003a);   // HDMITX21_MAC_HDMI21_SCHEDULER_4_reg
    rtd_outl(0xb801858c, 0x00000001);   // HDMITX21_MAC_HDMI21_SCHEDULER_3_reg
    rtd_outl(0xb8018594, 0x00000006);   // HDMITX21_MAC_HDMI21_SCHEDULER_5_reg
    rtd_outl(0xb8018598, 0x00000036);   // HDMITX21_MAC_HDMI21_SCHEDULER_6_reg
    rtd_outl(0xb80185ac, 0x000000f8);   // HDMITX21_MAC_HDMI21_SCHEDULER_10_reg
    rtd_outl(0xb80185b4, 0x0000008c);   // HDMITX21_MAC_HDMI21_SCHEDULER_13_reg
    rtd_outl(0xb801859c, 0x0000000b);   // HDMITX21_MAC_HDMI21_SCHEDULER_15_reg
    rtd_outl(0xb8018490, 0x00000020);   // HDMITX21_MAC_HDMI21_422_CONV_reg
    rtd_outl(0xb80184c4, 0x000000a0);   // HDMITX21_MAC_HDMI21_DPC_0_reg
    rtd_outl(0xb80184cc, 0x00000008);   // HDMITX21_MAC_HDMI21_DPC_2_reg
    rtd_outl(0xb80184d0, 0x00000020);   // HDMITX21_MAC_HDMI21_DPC_3_reg
    rtd_outl(0xb80184e4, 0x000000c0);   // HDMITX21_MAC_HDMI21_FORMAT_TOP_reg
    rtd_outl(0xb8018588, 0x000000a0);   // HDMITX21_MAC_HDMI21_SCHEDULER_2_reg
    rtd_outl(0xb8018580, 0x00000091);   // HDMITX21_MAC_HDMI21_SCHEDULER_0_reg
    rtd_outl(0xb80186a8, 0x00000001);   // HDMITX21_MAC_HDCP22_ENGINE_40_reg
    rtd_outl(0xb80186c0, 0x000000f0);   // HDMITX21_MAC_HDMI21_RC_reg
    rtd_outl(0xb80186cc, 0x00000000);   // HDMITX21_MAC_HDMI21_FRL_0_reg
    rtd_outl(0xb8018780, 0x00000080);   // HDMITX21_MAC_HDMI21_RS_0_reg
    rtd_outl(0xb8018790, 0x000000f0);   // HDMITX21_MAC_HDMI21_SCR_0_reg
    rtd_outl(0xb80187c0, 0x000000f8);   // HDMITX21_MAC_HDMI21_ENC_0_reg

    // [V] HDMITX PHY DSC INSEL/DispD FIFO Enable
    rtd_outl(0xb80178f8, 0x00000006);   // HDMITX_PHY_DSC_SRC_SEL_reg

    // [X] HDMIRX 20
    rtd_outl(0xb8040508, 0x00000001);   // HDMI_P0_FW_FUNC_reg
    rtd_outl(0xb804062c, 0x02200000);   // HDMI_P0_hdmi_clken3_reg

    // [X] HDMIRX 21
    rtd_outl(0xb8041000, 0x000000e1);   // HDMI21_P0_HD21_CR_reg
    rtd_outl(0xb804127c, 0x01008000);   // HDMI21_P0_HD21_HDMI_VCR_reg
    rtd_outl(0xb8041020, 0x19232000);   // HDMI21_P0_hd21_channel_align_b_reg
    rtd_outl(0xb804101c, 0x19232000);   // HDMI21_P0_hd21_channel_align_g_reg
    rtd_outl(0xb8041018, 0x19232000);   // HDMI21_P0_hd21_channel_align_r_reg
    rtd_outl(0xb8041014, 0x19232000);   // HDMI21_P0_hd21_channel_align_c_reg

    // [X] HDMIRX 20
    rtd_outl(0xb80401a8, 0x00000817);   // HDMI_P0_HDMI_CTS_FIFO_CTL_reg
    rtd_outl(0xb8040198, 0x02a54a95);   // HDMI_P0_PHY_FIFO_CR0_reg

    // [X] HDMI_COMBOEX
    rtd_outl(0xb8037004, 0x00000007);   // HDMI_COMBOEX_EX_PHY_FIFO_CR1_reg
    rtd_outl(0xb8037018, 0x00000008);   // HDMI_COMBOEX_EX_PHY_FIFO_CR3_reg
    rtd_outl(0xb8037000, 0x00000015);   // HDMI_COMBOEX_EX_PHY_FIFO_CR0_reg
    rtd_outl(0xb8037014, 0x00000000);   // HDMI_COMBOEX_EX_PHY_FIFO_CR2_reg

    // [X] HDMIRX 21/20
    rtd_outl(0xb8041034, 0x00000001);   // HDMI21_P0_hd21_ch_sync_b_reg
    rtd_outl(0xb8041030, 0x00000001);   // HDMI21_P0_hd21_ch_sync_g_reg
    rtd_outl(0xb804102c, 0x00000001);   // HDMI21_P0_hd21_ch_sync_r_reg
    rtd_outl(0xb804062c, 0x22200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb8040404, 0x00202855);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202845);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202855);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202805);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040404, 0x00202815);   // HDMI_P0_HDMI_VPLLCR1_reg
    rtd_outl(0xb8040420, 0x0000e5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040410, 0x00000003);   // HDMI_P0_MN_SCLKG_CTRL_reg
    rtd_outl(0xb8040420, 0x0000c5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040424, 0x00340000);   // HDMI_P0_HDMI_FDPLL_CTR1_reg
    rtd_outl(0xb8040428, 0x19804403);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040414, 0x00000000);   // HDMI_P0_MN_SCLKG_DIVS_reg
    rtd_outl(0xb8040410, 0x00000013);   // HDMI_P0_MN_SCLKG_CTRL_reg
    rtd_outl(0xb8040428, 0x1980440b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040420, 0x000085ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040428, 0x0980440b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040428, 0x0980441b);   // HDMI_P0_HDMI_FDPLL_CTR2_reg
    rtd_outl(0xb8040420, 0x0000c5ea);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040420, 0x0000c5e2);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040420, 0x0000c5e2);   // HDMI_P0_HDMI_FDPLL_CTR0_reg
    rtd_outl(0xb8040524, 0x00000009);   // HDMI_P0_hdmi_20_21_ctrl_reg
    rtd_outl(0xb804062c, 0x02200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb8040624, 0x1fe40000);   // HDMI_P0_hdmi_rst3_reg
    rtd_outl(0xb8040624, 0x3fe40000);   // HDMI_P0_hdmi_rst3_reg
    rtd_outl(0xb804050c, 0x00000000);   // HDMI_P0_PORT_SWITCH_reg
    rtd_outl(0xb804062c, 0x22200000);   // HDMI_P0_hdmi_clken3_reg
    rtd_outl(0xb804114c, 0x00009601);   // HDMI21_P0_HD21_TIMING_GEN_CR_reg

    // [X] HDMITX HDMI21 MAC enable
    rtd_outl(0xb801840c, 0x00000080);   // HDMITX21_MAC_HDMI21_TX_CTRL_reg
    rtd_outl(0xb80186cc, 0x00000080);   // HDMITX21_MAC_HDMI21_FRL_0_reg

    // [X] Scaler OutTG enable
    rtd_outl(0xb8027800, 0x10000001);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg

    // [X] HDMIRX 21
    rtd_outl(0xb804127c, 0x21508060);   // HDMI21_P0_HD21_HDMI_VCR_reg
    rtd_outl(0xb80412b8, 0x00000001);   // HDMI21_P0_HD21_HDMI_AUDCR_reg
    rtd_outl(0xb804127c, 0x21108060);   // HDMI21_P0_HD21_HDMI_VCR_reg
    rtd_outl(0xb804127c, 0x21108060);   // HDMI21_P0_HD21_HDMI_VCR_reg

    // [V] HDMITX MISC HD/EMP/Packet bypass
    rtd_outl(0xb8019700, 0x00000001);   // HDMITX_MISC_HD_BYPASS_CTRL00_reg
    rtd_outl(0xb8019704, 0x000003ff);   // HDMITX_MISC_HD_BYPASS_CTRL01_reg
    rtd_outl(0xb801970c, 0x83090285);   // HDMITX_MISC_HD_BYPASS_CTRL03_reg
    rtd_outl(0xb8019744, 0x00000001);   // HDMITX_MISC_HD_EMP_CTL_EN_reg
    rtd_outl(0xb8019550, 0x00000122);   // HDMITX_MISC_PACKET_PATH_CTRL_00_reg

    // [V] HDMIRX output enable
    rtd_outl(0xb8040210, 0x00000007);   // HDMI_P0_HDMI_PKT_reg
    rtd_outl(0xb8041360, 0x00000007);   // HDMI21_P0_HD21_HDMI_PKT_reg

    // [V] HDMITX MISC NCTS Regen
    rtd_outl(0xb8019524, 0x00ff0820);   // HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg
    rtd_outl(0xb8019528, 0x080007e0);   // HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg
    rtd_outl(0xb801952c, 0x05a00001);   // HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg
    rtd_outl(0xb8019520, 0x71000fa0);   // HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg

    // HDMIRX 21
    rtd_outl(0xb804127c, 0x21108060);   // HDMI21_P0_HD21_HDMI_VCR_reg

    NoteMessageHDMITx("[HDMITX] hdmi21_4kp30_rxBypassToTx Done!!\n");

    return;
}
#endif // #ifdef NOT_USED



void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmitx_phy_clr_clkratio(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmitx_phy_clr_clkratio...\n");

    //rtd_outl(0xb8017858, 0x00004000);
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_active_mask    , HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_active(0)); // (0x80000000UL)
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_hdmitxpll_divs2_sel_mask, HDMITX_PHY_HDMITX_CLK_DIV_reg_hdmitxpll_divs2_sel(0)); // (0x00800000UL)
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_pixel_mac_mask , HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_pixel_mac(0)); // (0x00700000UL)
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_txsoc_420_mask , HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_txsoc_420(0)); // (0x00070000UL)
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_txsoc_mask     , HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_txsoc(4)); // (0x00007000UL)
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_sclk_mask , HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_sclk(0)); // (0x00000700UL)
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_cclk_mask , HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_cclk(0)); // (0x00000070UL)
    rtd_maskl(HDMITX_PHY_HDMITX_CLK_DIV_reg, ~HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_pclk_mask , HDMITX_PHY_HDMITX_CLK_DIV_reg_clkratio_dsce_pclk(0)); // (0x00000007UL)
    return;
}


void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmitx_phy_pll(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmitx_phy_pll...\n");

    // HDMITX PHY PLL -- keep no  change
    rtd_outl(0xb8017800, 0x01000000);   // HDMITX_PHY_HDMITXPHY_CTRL00_reg
    rtd_outl(0xb8017808, 0x10124120);   // HDMITX_PHY_HDMITXPHY_CTRL02_reg

    // B[11:4]: pixel_div: HDMI20 4k2kp30=0x21, HDMI20 2k1kp60=0x22
    if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_2K1KP60_8BIT_RGB)
        value_u32 = 0x00000220;// pixel_div=0x22
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_4K2KP30_8BIT_RGB)
        value_u32 = 0x00000210; // pixel_div=0x21
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_4K2KP60_8BIT_RGB)
        value_u32 = 0x00000200;// pixel_div=0x20
    rtd_outl(0xb801780c, value_u32);   // HDMITX_PHY_HDMITXPHY_CTRL03_reg

    rtd_outl(0xb8017814, 0x00000000);   // HDMITX_PHY_HDMITXPHY_CTRL05_reg
    rtd_outl(0xb8017818, 0x00022210);   // HDMITX_PHY_HDMITXPHY_CTRL06_reg
    rtd_outl(0xb801781c, 0x00010000);   // HDMITX_PHY_HDMITXPHY_CTRL07_reg
    rtd_outl(0xb8017820, 0x03302100);   // HDMITX_PHY_HDMITXPHY_CTRL08_reg
    rtd_outl(0xb8017828, 0x10050003);   // HDMITX_PHY_HDMITXPHY_CTRL10_reg

    // B[15:12]: tmds_div: HDMI20 4k2kp60=0x1, HDMI20 4k2kp30=0x2, HDMI20 2k1kp60=0x4
    //value_u32 = (HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_4K2KP30_8BIT_RGB? 0x00002000: 0x00004000);
    if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_2K1KP60_8BIT_RGB)
        value_u32 = 0x00004000;// tmds_div=0x4
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_4K2KP30_8BIT_RGB)
        value_u32 = 0x00002000; // tmds_div=0x2
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_4K2KP60_8BIT_RGB)
        value_u32 = 0x00001000;// tmds_div=0x1
    rtd_outl(0xb8017830, value_u32);   // HDMITX_PHY_HDMITXPHY_CTRL12_reg

    rtd_outl(0xb8017844, 0x10001111);   // HDMITX_PHY_HDMITXPHY_CTRL17_reg
    rtd_outl(0xb801785c, 0xccb00000);   // HDMITX_PHY_HDMITX_TMDS_CTRL0_reg
    rtd_outl(0xb80178b0, 0x13000000);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg
    rtd_outl(0xb80178b4, 0x97200037);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg
    rtd_outl(0xb80178ac, 0x00003ff0);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg
    rtd_outl(0xb8017804, 0x00000000);   // HDMITX_PHY_HDMITXPHY_CTRL01_reg
    rtd_outl(0xb8017824, 0x00000010);   // HDMITX_PHY_HDMITXPHY_CTRL09_reg

    //rtd_outl(0xb8017858, 0x80004000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg
    if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_2K1KP60_8BIT_RGB)
        value_u32 = 0x80006000;// clk_ratio(PixelMac/Txsoc/420)=0x0 6 0
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_4K2KP30_8BIT_RGB)
        value_u32 = 0x80006000; // clk_ratio(PixelMac/Txsoc/420)=0x0 6 0
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI20_4K2KP60_8BIT_RGB)
        value_u32 = 0x80006000;// clk_ratio(PixelMac/Txsoc/420)=0x0 6 0
    rtd_outl(0xb8017858, value_u32);   // HDMITX_PHY_HDMITX_CLK_DIV_reg
    //rtd_outl(0xb8017858, 0x80044000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg

    return;
}

void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_disp_vgip(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_disp_vgip...\n");

    // disp VGIP
    // rtd_outl(0xb8027580, 0x00000001);
    // d1_output_mux_sel (mux before UZD): 0: through d1 PQ IP, 1: bypass path
    rtd_maskl(PPOVERLAY_DTG_d1_output_mux_reg, ~PPOVERLAY_DTG_d1_output_mux_d1_output_mux_sel_mask, PPOVERLAY_DTG_d1_output_mux_d1_output_mux_sel(1));
    //NoteMessageHDMITx("[HDMITX] 0xb8027580=%x\n", (UINT32)rtd_inl(PPOVERLAY_DTG_d1_output_mux_reg));

    if(ENABLE_HDMI_REPEATER_MODE == 1){// access scaler i-domain register will cause rbus timeout if no input source clock is connected
        rtd_outl(0xb8020284, 0x00002004);   // VGIP_Data_Path_Select_reg
        rtd_outl(0xb8020210, 0x00000003);   // VGIP_VGIP_CHN1_CTRL_reg

        //rtd_outl(0xb8020218, 0x00000f00);   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg
        rtd_outl(VGIP_VGIP_CHN1_ACT_HSTA_Width_reg,
                            VGIP_VGIP_CHN1_ACT_HSTA_Width_ch1_ih_act_sta(0)|
                            VGIP_VGIP_CHN1_ACT_HSTA_Width_ch1_ih_act_wid(pScalerHdmiTimingInfo->DTG_DH_ACT));   // VGIP_VGIP_CHN1_ACT_HSTA_Width_reg

        //rtd_outl(0xb802021c, 0x00000008);   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg
        rtd_outl(VGIP_VGIP_CHN1_ACT_VSTA_Length_reg,
                            VGIP_VGIP_CHN1_ACT_VSTA_Length_ch1_iv_act_sta(0)|
                            VGIP_VGIP_CHN1_ACT_VSTA_Length_ch1_iv_act_len(pScalerHdmiTimingInfo->DTG_DV_ACT));   // VGIP_VGIP_CHN1_ACT_VSTA_Length_reg

        rtd_outl(0xb8020224, 0x20018000);   // VGIP_VGIP_CHN1_MISC_reg
        rtd_outl(0xb8020210, 0x80000003);   // VGIP_VGIP_CHN1_CTRL_reg

        //rtd_outl(0xb8020254, 0x0000157b);   // VGIP_ICH1_VGIP_HTOTAL_reg
        total_h = pScalerHdmiTimingInfo->DTG_DH_ACT + pScalerHdmiTimingInfo->DTG_H_PORCH - 1;
        rtd_outl(VGIP_ICH1_VGIP_HTOTAL_reg, VGIP_ICH1_VGIP_HTOTAL_ch1_htotal_num(total_h));   // VGIP_ICH1_VGIP_HTOTAL_reg


    }

    return;
}

void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_disp_dtg(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_disp_dtg...\n");

    // disp DTG
    //rtd_outl(0xb8027804, 0x0000001f); // PPOVERLAY_OUTTG_OUTTG_DH_Width_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DHS_WIDTH - 1; // Display h sync width, set 2N-1
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DH_Width_reg, PPOVERLAY_OUTTG_OUTTG_DH_Width_dh_width(value_u16));
    //rtd_outl(0xb8027808, 0x157b0000); // PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DH_ACT + pScalerHdmiTimingInfo->DTG_H_PORCH - 1; // Display horizontal total pixels, set 2N-1
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_reg, PPOVERLAY_OUTTG_OUTTG_DH_Total_Length_dh_total(value_u16));

    //rtd_outl(0xb802780c, 0x00000002); // PPOVERLAY_OUTTG_OUTTG_DV_Length_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DVS_LENGTH - 1; // Display vertical sync width, set 1N-1
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DV_Length_reg, PPOVERLAY_OUTTG_OUTTG_DV_Length_dv_length(value_u16));

    //rtd_outl(0xb8027810, 0x00000029); // PPOVERLAY_OUTTG_OUTTG_DV_total_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DV_ACT + pScalerHdmiTimingInfo->DTG_V_PORCH - 1; // Display vertical total lines, set 1N-1
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DV_total_reg, PPOVERLAY_OUTTG_OUTTG_DV_total_dv_total(value_u16));

    //rtd_outl(0xb8027814, 0x01801080); // PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA; // Display h data enable start, set 2N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_sta_mask, PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_sta(value_u16));
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA + pScalerHdmiTimingInfo->DTG_DH_ACT; // Display h data enable end , set 2N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_end_mask, PPOVERLAY_OUTTG_OUTTG_DH_DEN_Start_End_dh_den_end(value_u16));

    //rtd_outl(0xb8027818, 0x001a0022); // PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA; // Display v data enable start, set 1N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_sta_mask, PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_sta(value_u16));
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA + pScalerHdmiTimingInfo->DTG_DV_ACT; // Display v data enable end, set 1N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_end_mask, PPOVERLAY_OUTTG_OUTTG_DV_DEN_Start_End_dv_den_end(value_u16));

    //rtd_outl(0xb8027840, 0x01801080); // PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA; // Display h data enable start, set 2N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_sta_mask, PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_sta(value_u16));
    value_u16 = pScalerHdmiTimingInfo->DTG_H_DEN_STA + pScalerHdmiTimingInfo->DTG_DH_ACT; // Display h data enable end , set 2N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_end_mask, PPOVERLAY_OUTTG_OUTTG_DH_ACT_Start_End_dh_act_end(value_u16));

    //rtd_outl(0xb8027844, 0x001a0022); // PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA; // Display v data enable start, set 1N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_sta_mask, PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_sta(value_u16));
    value_u16 = pScalerHdmiTimingInfo->DTG_V_DEN_STA + pScalerHdmiTimingInfo->DTG_DV_ACT; // Display v data enable end, set 1N
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_reg, ~PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_end_mask, PPOVERLAY_OUTTG_OUTTG_DV_ACT_Start_End_dv_act_end(value_u16));

    //rtd_outl(0xb8027800, 0x90000000); // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
    value_u32 = PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_frc_fsync_mask|PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_fix_last_line_mask;
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, value_u32);

    // redundant setting
    //rtd_outl(0xb8027804, 0x00000057); // PPOVERLAY_OUTTG_OUTTG_DH_Width_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DHS_WIDTH - 1; // Display h sync width, set 2N-1
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DH_Width_reg, PPOVERLAY_OUTTG_OUTTG_DH_Width_dh_width(value_u16));

    // redundant setting
    //rtd_outl(0xb802780c, 0x00000009); // PPOVERLAY_OUTTG_OUTTG_DV_Length_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DVS_LENGTH - 1; // Display vertical sync width, set 1N-1
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_DV_Length_reg, PPOVERLAY_OUTTG_OUTTG_DV_Length_dv_length(value_u16));

    // redundant setting
    //rtd_outl(0xb8027800, 0x10000000); // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
    // TX input timing gen (free run)
    value_u8 = (ENABLE_VRR_SUPPORT==1? 1: 0);
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_fix_last_line(value_u8));

    return;
}

void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmi20_mac(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_hdmitx_hdmi20_mac...\n");

    //rtd_outl(0xb801940c, 0x00100000); // HDMITX_MISC_HDMITX_4PTO1P_reg
    rtd_outl(HDMITX_MISC_HDMITX_4PTO1P_reg, HDMITX_MISC_HDMITX_4PTO1P_hdmitx_2pto1p_en(1));

    //rtd_outl(0xb8018c00, 0x00000006); // HDMITX20_MAC0_HDMI_CONTROL_0_reg
    rtd_outl(HDMITX20_MAC0_HDMI_CONTROL_0_reg, 0);
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_0_reg, ~HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chb_swap_mask, HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chb_swap(0));
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_0_reg, ~HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chg_swap_mask, HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chg_swap(1));
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_0_reg, ~HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chr_swap_mask, HDMITX20_MAC0_HDMI_CONTROL_0_infifo_chr_swap(2));

    //rtd_outl(0xb8018c14, 0x00000002); // HDMITX20_MAC0_HDMI_CONTROL_5_reg
    rtd_outl(HDMITX20_MAC0_HDMI_CONTROL_5_reg, HDMITX20_MAC0_HDMI_CONTROL_5_dp_vch_num(1));

    //rtd_outl(0xb8018c18, 0x00000000); // HDMITX20_MAC0_HDMI_CONTROL_6_reg
    rtd_outl(HDMITX20_MAC0_HDMI_CONTROL_6_reg, HDMITX20_MAC0_HDMI_CONTROL_6_dp_enable(0));

    //rtd_outl(0xb8018c1c, 0x00000040); // HDMITX20_MAC0_HDMI_CONTROL_7_reg
    rtd_outl(HDMITX20_MAC0_HDMI_CONTROL_7_reg, HDMITX20_MAC0_HDMI_CONTROL_7_dp_color_depth(4)); // 4: 24bit, 5: 30bit, 6: 36bit

    //rtd_outl(0xb8018c10, 0x000000df); // HDMITX20_MAC0_HDMI_CONTROL_4_reg
    rtd_outl(HDMITX20_MAC0_HDMI_CONTROL_4_reg, 0);
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_4_reg, ~HDMITX20_MAC0_HDMI_CONTROL_4_sram_rd_start_pos_mask, HDMITX20_MAC0_HDMI_CONTROL_4_sram_rd_start_pos(0x1f));
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_4_reg, ~HDMITX20_MAC0_HDMI_CONTROL_4_vch_mode_bypass_mask, HDMITX20_MAC0_HDMI_CONTROL_4_vch_mode_bypass(1));
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_4_reg, ~HDMITX20_MAC0_HDMI_CONTROL_4_sram_en_mask, HDMITX20_MAC0_HDMI_CONTROL_4_sram_en(1));

    //rtd_outl(0xb8018c24, 0x0000000c); // HDMITX20_MAC0_SCHEDULER_1_reg
    rtd_outl(HDMITX20_MAC0_SCHEDULER_1_reg, HDMITX20_MAC0_SCHEDULER_1_vactive_island_max_pkt_num(1));
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_1_reg, ~HDMITX20_MAC0_SCHEDULER_1_gcp_en_mask, HDMITX20_MAC0_SCHEDULER_1_gcp_en(1));

    //rtd_outl(0xb8018c28, 0x00000000); // HDMITX20_MAC0_SCHEDULER_2_reg
    rtd_outl(HDMITX20_MAC0_SCHEDULER_2_reg, HDMITX20_MAC0_SCHEDULER_2_hdcp_enc_en(0));
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_2_reg, ~HDMITX20_MAC0_SCHEDULER_2_vs_phase_en_eco_mask, HDMITX20_MAC0_SCHEDULER_2_vs_phase_en_eco(0));
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_2_reg, ~HDMITX20_MAC0_SCHEDULER_2_vs_phase_eco_mask, HDMITX20_MAC0_SCHEDULER_2_vs_phase_eco(0));


    #define _HDMI_MAC_TX0_PRE_KEEP_OUT_SIZE 50
    //usPre_Keepout_Start = (usHstart - _HDMI_MAC_TX0_PRE_KEEP_OUT_SIZE) * GET_HDMI_MAC_TX0_INPUT_COLOR_DEPTH() / 8 * (GET_HDMI_MAC_TX0_INPUT_REPEAT_NUM() + 1);
    value_u32 = pScalerHdmiTimingInfo->DTG_H_DEN_STA - _HDMI_MAC_TX0_PRE_KEEP_OUT_SIZE; // 4kp30=384 - 50 = 334 (x14e)

    //rtd_outl(0xb8018c2c, 0x00000036); // HDMITX20_MAC0_SCHEDULER_3_reg
    value_u16 = value_u32 & HDMITX20_MAC0_SCHEDULER_3_video_pre_keepout_start_7_0_mask;
    rtd_outl(HDMITX20_MAC0_SCHEDULER_3_reg, HDMITX20_MAC0_SCHEDULER_3_video_pre_keepout_start_7_0(value_u16));

    //rtd_outl(0xb8018c30, 0x0000000c); // HDMITX20_MAC0_SCHEDULER_4_reg
    value_u16 = (value_u32 >> 8) & HDMITX20_MAC0_SCHEDULER_4_video_pre_keepout_start_14_8_mask;
    rtd_outl(HDMITX20_MAC0_SCHEDULER_4_reg, HDMITX20_MAC0_SCHEDULER_4_video_pre_keepout_start_14_8(value_u16));

    //rtd_outl(0xb8018c6c, 0x00000001); // HDMITX20_MAC0_TMDS_ENCODER_3_reg
    rtd_outl(HDMITX20_MAC0_TMDS_ENCODER_3_reg, HDMITX20_MAC0_TMDS_ENCODER_3_tmds_en(1));

    //rtd_outl(0xb8018cb8, 0x00000080); // HDMITX20_ON_REGION_TXFIFO_CTRL0_reg
    rtd_outl(HDMITX20_ON_REGION_TXFIFO_CTRL0_reg, HDMITX20_ON_REGION_TXFIFO_CTRL0_tx_en(1));

    //rtd_outl(0xb8018fc4, 0x00000080); // HDMITX20_ON_REGION_PIXEL_CONV_P1_reg
    rtd_outl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg, HDMITX20_ON_REGION_PIXEL_CONV_P1_pfifo_en(1));

    //rtd_outl(0xb8018cc4, 0x0000004b); // HDMITX20_ON_REGION_TXFIFO_CTRL3_reg
    rtd_outl(HDMITX20_ON_REGION_TXFIFO_CTRL3_reg, 0x4b);

    //rtd_outl(0xb8018f6c, 0x00000001); // HDMITX20_MAC0_HDCP22_ENGINE_40_reg
    rtd_outl(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, HDMITX20_MAC0_HDCP22_ENGINE_40_hdcp_rst_n(1));

    //rtd_outl(0xb80191c0, 0x00000046); // HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg
    value_u16 = HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_vs_out_inv_mask|HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_hs_out_inv_mask;
    rtd_outl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, value_u16);
    rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_fifo_unstable_mask, HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_fifo_unstable(1));

    //rtd_outl(0xb8019140, 0x00000000); // HDMITX20_MAC1_TIMING_1_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DV_ACT >> 4;
    rtd_outl(HDMITX20_MAC1_TIMING_1_reg, HDMITX20_MAC1_TIMING_1_linecount_11_4(value_u16));

    //rtd_outl(0xb8019144, 0x00000080); // HDMITX20_MAC1_TIMING_2_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DV_ACT & HDMITX20_MAC1_TIMING_2_linecount_3_0_mask;
    rtd_outl(HDMITX20_MAC1_TIMING_2_reg, HDMITX20_MAC1_TIMING_2_linecount_3_0(value_u16));

    // Get V Back porch = usVstart - usVsync
    value_u32 = (pScalerHdmiTimingInfo->DTG_V_DEN_STA - pScalerHdmiTimingInfo->DTG_DVS_LENGTH );
    value_u16 = value_u32 >> 8;
    rtd_maskl(HDMITX20_MAC1_TIMING_2_reg, ~HDMITX20_MAC1_TIMING_2_lineporch_11_8_mask, HDMITX20_MAC1_TIMING_2_lineporch_11_8(value_u16));

    //rtd_outl(0xb8019148, 0x00000010); // HDMITX20_MAC1_TIMING_3_reg
    value_u16 = value_u32 & HDMITX20_MAC1_TIMING_3_lineporch_7_0_mask;
    rtd_outl(HDMITX20_MAC1_TIMING_3_reg, HDMITX20_MAC1_TIMING_3_lineporch_7_0(value_u16));

    //rtd_outl(0xb8019168, 0x00000020); // HDMITX20_MAC1_VFIFO_8_reg
    rtd_outl(HDMITX20_MAC1_VFIFO_8_reg, HDMITX20_MAC1_VFIFO_8_ycbcr422_8bits(1));
    rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~HDMITX20_MAC1_VFIFO_8_pixelencfmt_mask, HDMITX20_MAC1_VFIFO_8_pixelencfmt(0));

    //rtd_outl(0xb8018c20, 0x000000c0); // HDMITX20_MAC0_SCHEDULER_0_reg
    // hdmi_dvi_mode_sel is double buffer, read value effective after double buffer triggered
    rtd_outl(HDMITX20_MAC0_SCHEDULER_0_reg, HDMITX20_MAC0_SCHEDULER_0_hdmi_en(1));
    rtd_maskl(HDMITX20_MAC0_SCHEDULER_0_reg, ~HDMITX20_MAC0_SCHEDULER_0_hdmi_dvi_mode_sel_mask, HDMITX20_MAC0_SCHEDULER_0_hdmi_dvi_mode_sel(1));

    //rtd_outl(0xb80178f8, 0x00010006); // HDMITX_PHY_DSC_SRC_SEL_reg
    // Decide HDMITX_Insel function_mode: 0 => 2pto4p mode (4p valid, ch0/ch1/ch2/ch3) for HDMI21, 1 => bypass mode (2p valid, ch0/ch1) for HDMI20
    rtd_outl(HDMITX_PHY_DSC_SRC_SEL_reg, HDMITX_PHY_DSC_SRC_SEL_reg_insel_mode(1));
    rtd_maskl(HDMITX_PHY_DSC_SRC_SEL_reg, ~HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset_mask, HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset(1));

    // delay
#if 1
    //delayms(1);
    delayus(10);
#else
    value_u8 = 50;
    while(rtd_inl(HDMITX_PHY_DSC_SRC_SEL_reg) & HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_reset_mask){
        delayms(1);
        if(--value_u8 == 0)
            break;
    }
    NoteMessageHDMITx("[HDMITX] dispd_fifo_reset cnt=%d\n", (UINT32)value_u8);
#endif
    rtd_maskl(HDMITX_PHY_DSC_SRC_SEL_reg, ~HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_en_mask, HDMITX_PHY_DSC_SRC_SEL_reg_dispd_fifo_en(1));


    //rtd_outl(0xb8019524, 0x00ff0820); // HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_1_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_1_fifo_threshold_hi_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_1_fifo_threshold_hi(0x820));

    //rtd_outl(0xb8019528, 0x080007e0); // HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_mid_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_mid(0x800));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_2_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_low_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_2_fifo_threshold_low(0x7e0));


    //rtd_outl(0xb801952c, 0x05000001); // HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg
    rtd_outl(HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg, HDMITX_MISC_HDMITX_NCTS_REGEN_3_tx_n_value(0x5000));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_3_reg, ~ HDMITX_MISC_HDMITX_NCTS_REGEN_3_ncts_dbg_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_3_ncts_dbg_en(1));

    //rtd_outl(0xb8019520, 0x71000fa0); // HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg
    rtd_outl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_mux(1)); // N/CTS mux: 0x0 : N/CTS from spdif. Original path, 0x1 : N/CTS from regen. The new method.
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_rbus_period_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_rbus_period(0xfa));
    rtd_maskl(HDMITX_MISC_HDMITX_NCTS_REGEN_0_reg, ~HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en_mask, HDMITX_MISC_HDMITX_NCTS_REGEN_0_ncts_regen_en(1));

    return;
}



void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmi21_mac_enable(UINT8 enable)
{
    //NoteMessageHDMITx("[HDMITX] HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmi21_mac_enable...\n");

    //rtd_outl(0xb801840c, 0x00000080);   // [?] HDMITX21_MAC_HDMI21_TX_CTRL_reg
    rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~HDMITX21_MAC_HDMI21_TX_CTRL_hdmi_21_tx_en_mask, HDMITX21_MAC_HDMI21_TX_CTRL_hdmi_21_tx_en(enable));

    //rtd_outl(0xb80186cc, 0x00000080);   // [?] HDMITX21_MAC_HDMI21_FRL_0_reg
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~HDMITX21_MAC_HDMI21_FRL_0_frl_en_mask, HDMITX21_MAC_HDMI21_FRL_0_frl_en(enable));

    return;
}


void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_disp_dtg_dispEn(void)
{
    //NoteMessageHDMITx("[HDMITX] hdmi20_4k30_bypass_cfg_disp_dtg_dispEn...\n");
    // rtd_outl(0xb8027800, 0x10000001);   // PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg
    value_u8 = (ENABLE_VRR_SUPPORT==1? 1: 0);
    rtd_outl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_fix_last_line(value_u8)); // free run for ptg
    rtd_maskl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg, ~PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en_mask, PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_disp_en(1));

    return;
}


void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmitx_misc_bypass(void)
{
    //NoteMessageHDMITx("[HDMITX] HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmitx_misc_bypass...\n");

    //rtd_outl(0xb8019700, 0x00000001);   // HDMITX_MISC_HD_BYPASS_CTRL00_reg
    // update avi/vsif/dram/spd in FAPA region location 0 or v-blanking: 0x1:FAPA region region location 0 , 0x0: v-blanking
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL00_reg, HDMITX_MISC_HD_BYPASS_CTRL00_hd_packet_trans_mode(0));
    // packet transfer main switch (2dff): 0x0:Disable , 0x1: Enable
    value_u8 = (ENABLE_HDMI_REPEATER_MODE==1? 1: 0);
    rtd_maskl(HDMITX_MISC_HD_BYPASS_CTRL00_reg, ~HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en_mask, HDMITX_MISC_HD_BYPASS_CTRL00_metabuf_en(value_u8));

    //rtd_outl(0xb8019704, 0x000003ff);   // HDMITX_MISC_HD_BYPASS_CTRL01_reg
    value_u32 = (UINT32)((HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_inf_en_mask|HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_inf_en_mask)|
                            (HDMITX_MISC_HD_BYPASS_CTRL01_hd_gmp_en_mask|HDMITX_MISC_HD_BYPASS_CTRL01_hd_res_en_mask|HDMITX_MISC_HD_BYPASS_CTRL01_hd_spd_en_mask|HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_hbr_en_mask| HDMITX_MISC_HD_BYPASS_CTRL01_hd_audio_en_mask)|
                            (HDMITX_MISC_HD_BYPASS_CTRL01_hd_emp_en_mask|HDMITX_MISC_HD_BYPASS_CTRL01_hd_vsif_en_mask|HDMITX_MISC_HD_BYPASS_CTRL01_hd_avi_en_mask|HDMITX_MISC_HD_BYPASS_CTRL01_hd_dram_en_mask));
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL01_reg, (UINT32)value_u32 );

    //rtd_outl(0xb801970c, 0x83090285);   // HDMITX_MISC_HD_BYPASS_CTRL03_reg
    rtd_outl(HDMITX_MISC_HD_BYPASS_CTRL03_reg, 0x83090285);


    //rtd_outl(0xb8019744, 0x00000001);   // HDMITX_MISC_HD_EMP_CTL_EN_reg
    rtd_outl(HDMITX_MISC_HD_EMP_CTL_EN_reg, HDMITX_MISC_HD_EMP_CTL_EN_hd_emp_all_en(1));

    //rtd_outl(0xb8019550, 0x00000111);   // HDMITX_MISC_PACKET_PATH_CTRL_00_reg
    rtd_outl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, 0);
    // fapa_end_sel: 0x0: FW setting 1/2 v-blanking, 0x1: Vsync falling
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_fapa_end_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_fapa_end_sel(1));
    // aud20_pkt_src_sel: HDMI 20 audio input path select : 0x0: Original path(spdif in) , 0x1: audio sample packet
    value_u8 = (!_SKIP_AUDIO_BYPASS && (ENABLE_HDMI_REPEATER_MODE==1)? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_aud20_pkt_src_sel_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_aud20_pkt_src_sel(value_u8));
    // pkt_bypass_20_en: HDMI20 infoframe/EMP packet: 0x0:Disable BYPASS packet , 0x1: Enable BYPASS packet
    value_u8 = (ENABLE_HDMI_REPEATER_MODE == 1? 1: 0);
    rtd_maskl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg, ~HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en_mask, HDMITX_MISC_PACKET_PATH_CTRL_00_pkt_bypass_20_en(value_u8));

    return;
}


void HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_aviInfoFramePkt(void)
{
    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, 0);

    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(_BIT2),0x00);

    // Set Periodic SRAM Address
    rtd_outl(HDMITX20_MAC1_HDMI21_PKT_CTRL_1_reg, 0x0);

    // if update pkt by align_vde mode, select move REG table to SW DFF0 or DFF1
    if(rtd_inl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg) & _BIT6)
        rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT7, 0x00);

    // HDMI20 TX packet InfoFrame control
    HDMITX_DV_Script_ScalerHdmiMacTx0FillAVIInfoPacket();

    HDMITX_DV_Script_ScalerHdmiMacTx0PktDataApplyToPSRAM();

    // enable avi pkt
    rtd_maskl(HDMITX20_MAC0_DATA_ISLAND_PACKETIZER_54_reg, ~(_BIT2),_BIT2);

    rtd_maskl(HDMITX20_MAC1_HDMI21_PKT_CTRL_2_reg, ~_BIT6, _BIT6);

}


void HDMITX_DV_Script_Repeater_PTG_hdmi20(void)
{
    FatalMessageHDMITx("[HDMITX] HDMITX_DV_Script_Repeater_PTG_hdmi20...\n");

    // [CRT] init
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_crt_init();

    // [RX] HDMIRX bypass to TX output disable
    if(ENABLE_LOAD_HDMIRX_SCRIPT == 1)
        HDMITX_DV_Script_HDMIRX_pktBypassToTx_enable(_DISABLE);

    // [TX] PHY - clkratio_active=0
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmitx_phy_clr_clkratio();

    // [disp] vgip
    HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_disp_vgip();

    // [TX] PHY pll
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmitx_phy_pll();
        msleep(30);

        // [disp] dtg
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_disp_dtg();
    }

    if((ENABLE_HDMI_REPEATER_MODE == 0) && ENABLE_LOAD_HDMITX_SCRIPT){ // TX input timing gen (free run)
        // [TX] timing gen
        HDMITX_DV_Script_hdmitx_timing_gen_cfg();

        // [TX] pattern gen
        HDMITX_DV_Script_hdmitx_pattern_gen_cfg(_ENABLE);
    }

    // [TX] HDMITX HDMI20 MAC
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmi20_mac();

    if(/*(ENABLE_HDMI_REPEATER_MODE == 1) &&*/ (ENABLE_LOAD_HDMIRX_SCRIPT == 1)){
        // [RX] HDMIRX PHY
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_phy();

        // [RX] HDMIRX comboex
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_comboex();

        // [RX] HDMI RX MAC
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_mac();
    }

    // [disp] dtg disp_en
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
#if 1 // frame sync enable
        HDMITX_DV_Script_Disp_OutTG_dispEn();
#else // [VRR] need enable both fix_last_line=1 and disp_fsync_en=1 in VRR mode
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_disp_dtg_dispEn();
#endif
        msleep(30);
    }

    // [TX] FIFO reset
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_hdmitx20_reset_fifoErr();

    if(ENABLE_HDMI_REPEATER_MODE == 1){
        // [RX] HDMIRX aud_en
        if(ENABLE_LOAD_HDMIRX_SCRIPT == 1)
            HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_aud();

        // [TX] HDMITX BYPASS
        if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
#if 1 //[HDMITX] MISC HD/EMP/Packet bypass
            HDMITX_DV_Script_hdmitx_pktHdrBypass_config();
#else
            HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmitx_misc_bypass();
#endif
        }

        if(ENABLE_LOAD_HDMIRX_SCRIPT == 1){
            // [RX] HDMIRX clean Vsync Occur
            HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_hdmirx_clr_vsync();

            // HDMIRX packet bypass to TX output enable
            if((ENABLE_HDMI_REPEATER_MODE == 1) && (ENABLE_LOAD_HDMITX_SCRIPT == 1)) // enable RX bypass when TX bypass setting ready
                HDMITX_DV_Script_HDMIRX_pktBypassToTx_enable(_ENABLE);
        }
    }

    // [TX] DSC SRC SEL
    if(ENABLE_LOAD_HDMITX_SCRIPT == 1){
        // 0 => 2pto4p mode for HDMI21, 1 => bypass mode for HDMI20
        HDMITX_DV_Script_hdmitx_dsc_src_sel(1);
    }

    if((ENABLE_HDMI_REPEATER_MODE == 0) && ENABLE_LOAD_HDMITX_SCRIPT){ // TX input timing gen (free run)
        // [TX] set avi info frame packet
        HDMITX_DV_Script_Repeater_PTG_hdmi20_cfg_aviInfoFramePkt();
        // [TX] timing gen
        HDMITX_DV_Script_hdmitx_timing_gen_enable(_ENABLE);
    }

    if(ENABLE_LOAD_HDMITX_SCRIPT == 1)
        HDMITX_DV_Script_hdmitx_frame_rate_measure_en(_ENABLE);

    NoteMessageHDMITx("[HDMITX] pkt_bypass=%x\n", (UINT32)rtd_inl(HDMITX_MISC_PACKET_PATH_CTRL_00_reg));
    FatalMessageHDMITx("[HDMITX] HDMITX_DV_Script_Repeater_PTG_hdmi20 Done!\n");

    return;
}


void HDMITX_DV_Script_hdmitx21_on_region_source_sel(UINT16 value)
{
    rtd_inl(0xb8018044); //0x00000000);
    //rtd_outl(0xb8018044, 0x0000000f);
    rtd_outl(HDMITX21_ON_REGION_HDMI21_PRBS_CTRL_0_reg, value); // Lane x mux for source select: 0:data, 1: PRBS
}

void HDMITX_DV_Script_hdmitx_phy_clr_clkratio(void)
{
    rtd_outl(0xb8017858, 0x00004000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg

    return;
}


void HDMITX_DV_Script_hdmitx21_hdmitx_phy_pll(void)
{
    //NoteMessageHDMITx("[HDMITX] HDMITX_DV_Script_hdmitx21_hdmitx_phy_pll...\n");

    // HDMITX PHY PLL -- keep no  change
    rtd_inl(0xb8017800); //0x01000000);
    rtd_outl(0xb8017800, 0x01001000);   // HDMITX_PHY_HDMITXPHY_CTRL00_reg

    rtd_inl(0xb8017808); //0x00124120);
    rtd_outl(0xb8017808, 0x10124120);   // HDMITX_PHY_HDMITXPHY_CTRL02_reg

    rtd_inl(0xb801780c); //0x00000020);
    if((HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_6G4L_1080P60_8BIT_RGB)
        || (HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_3G3L_1080P60_8BIT_RGB))
        value_u32 = 0x00000220;// pixel_div=0x22
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_6G4L_4k2kP60_8BIT_RGB)
        value_u32 = 0x00000200;// pixel_div=0x20
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_8G_4k2kP120_8BIT_RGB)
        value_u32 = 0x00000200;// pixel_div=0x20
    rtd_outl(0xb801780c, value_u32);   // HDMITX_PHY_HDMITXPHY_CTRL03_reg

    rtd_inl(0xb8017814); //0x00000002);
    rtd_outl(0xb8017814, 0x00000000);   // HDMITX_PHY_HDMITXPHY_CTRL05_reg

    rtd_inl(0xb8017818); //0x00022200);
    rtd_outl(0xb8017818, 0x00022210);   // HDMITX_PHY_HDMITXPHY_CTRL06_reg

    rtd_inl(0xb801781c); //0x00000000);
    rtd_outl(0xb801781c, 0x00010001);   // HDMITX_PHY_HDMITXPHY_CTRL07_reg

    rtd_inl(0xb8017820); //0x03302102);
    rtd_outl(0xb8017820, 0x03312100);   // HDMITX_PHY_HDMITXPHY_CTRL08_reg

    rtd_inl(0xb8017828); //0x00050003);
    rtd_outl(0xb8017828, 0x10050003);   // HDMITX_PHY_HDMITXPHY_CTRL10_reg

    rtd_inl(0xb8017830); //0x00004000);
    // B[15:12]: tmds_div: HDMI20 4k2kp60=0x1, HDMI20 4k2kp30=0x2, HDMI20 2k1kp60=0x4
    if((HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_6G4L_1080P60_8BIT_RGB)
        || (HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_3G3L_1080P60_8BIT_RGB))
        value_u32 = 0x00008000;// tmds_div=0x8
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_6G4L_4k2kP60_8BIT_RGB)
        value_u32 = 0x00004000;// tmds_div=0x4
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_8G_4k2kP120_8BIT_RGB)
        value_u32 = 0x00002000;// tmds_div=0x4
    rtd_outl(0xb8017830, value_u32);   // HDMITX_PHY_HDMITXPHY_CTRL12_reg

    rtd_inl(0xb8017844); //0x00000000);
    rtd_outl(0xb8017844, 0x10001111);   // HDMITX_PHY_HDMITXPHY_CTRL17_reg

    rtd_inl(0xb801785c); //0x88b13000);
    rtd_outl(0xb801785c, 0xccb6c0e4);   // HDMITX_PHY_HDMITX_TMDS_CTRL0_reg

    rtd_inl(0xb80178b0); //0x00000000);
    rtd_outl(0xb80178b0, 0x13000000);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC1_reg

    rtd_inl(0xb80178b4); //0x97200032);
    rtd_outl(0xb80178b4, 0x97200037);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC2_reg

    rtd_inl(0xb80178ac); //0x00003ff0);
    rtd_outl(0xb80178ac, 0x00003ff0);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg

    rtd_inl(0xb8017804); //0x00000000);
    rtd_outl(0xb8017804, 0x00010000);   // HDMITX_PHY_HDMITXPHY_CTRL01_reg

    rtd_inl(0xb8017824); //0x00022000);
    rtd_outl(0xb8017824, 0x00012010);   // HDMITX_PHY_HDMITXPHY_CTRL09_reg

    // 0x18017858 = 0X80044000 // from WunLin
    // B[22:20]: clkratio_pixel_mac
    // B[18:16]: clkratio_txsoc_420
    // B[14:12]: clkratio_txsoc
    // rtd_outl(0xb8017858, 0x80044000);
    rtd_inl(0xb8017858); //0x00000000);

    if((HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_6G4L_1080P60_8BIT_RGB)
        || (HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_3G3L_1080P60_8BIT_RGB))
        value_u32 = 0x80664000;// clk_ratio(pixelMac/Txsoc/420)=0x6 4 6
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_6G4L_4k2kP60_8BIT_RGB)
        value_u32 = 0x80666000;// clk_ratio(pixelMac/Txsoc/420)=0x6 6 6
    else if(HDMITX_INPUT_TIMING_MODE == HDMITX_INPUT_TIMING_HDMI21_8G_4k2kP120_8BIT_RGB)
        value_u32 = 0x80404000;// clk_ratio=0x4 4 0
    rtd_outl(0xb8017858, value_u32);   // HDMITX_PHY_HDMITX_CLK_DIV_reg
    //rtd_outl(0xb8017858, 0x80664000);   // HDMITX_PHY_HDMITX_CLK_DIV_reg

    rtd_inl(0xb80178ac); //0x00003ff0);
    rtd_outl(0xb80178ac, 0x00003ff1);   // HDMITX_PHY_HDMITX_DISP_PLL_SSC0_reg
    rtd_outl(0xb80178ac, 0x00003ff0);

    return;
}



void HDMITX_DV_Script_hdmitx21_packet_audio_control(UINT8 enable)
{
    // HDMITX21 Audio sample, ACR/CTS packet, SPDIF input control  -- keep no change
    rtd_inl(0xb8018b04); //0x00000002);
    rtd_outl(0xb8018b04, 0x00000002);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_1_reg
    rtd_inl(0xb8018b0c); //0x00000000);
    rtd_outl(0xb8018b0c, 0x00000003);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_3_reg
    rtd_inl(0xb8018b1c); //0x00000000);
    rtd_outl(0xb8018b1c, 0x00000080);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_7_reg
    rtd_inl(0xb8018b20); //0x00000000);
    rtd_outl(0xb8018b20, 0x00000010);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_8_reg
    rtd_inl(0xb8018b24); //0x00000000);
    rtd_outl(0xb8018b24, 0x00000000);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_9_reg
    rtd_inl(0xb8018b3c); //0x00000000);
    rtd_outl(0xb8018b3c, 0x00000080);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_15_reg
    rtd_inl(0xb8018b4c); //0x00000000);
    rtd_outl(0xb8018b4c, 0x00000000);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_19_reg
    rtd_inl(0xb8018b50); //0x00000000);
    rtd_outl(0xb8018b50, 0x000000a0);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_20_reg
    rtd_inl(0xb8018b54); //0x00000000);
    rtd_outl(0xb8018b54, 0x00000000);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_21_reg
    rtd_inl(0xb8018b5c); //0x00000000);
    //rtd_outl(0xb8018b5c, 0x00000001);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_23_reg
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_23_reg, HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_23_aud_spdif_in_en(enable));

    rtd_inl(0xb8018b88); //0x00000000);
    rtd_inl(0xb8018b00); //0x00000000);
    rtd_outl(0xb8018b88, 0x00000001);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_1_reg
    rtd_outl(0xb8018b00, 0x00000008);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_0_reg
    rtd_inl(0xb8018b10); //0x00000070);
    rtd_outl(0xb8018b10, 0x00000050);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_4_reg
    rtd_inl(0xb8018b84); //0x00000000);

    // ACR Packet will be not sent when audio disable
    rtd_maskl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_reg, ~HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_acrp_disablecrp_mask, HDMITX21_PACKET_CONTROL_HDMI21_AUD_TOP_6_acrp_disablecrp(!enable));

    //rtd_outl(0xb8018b84, 0x00000010);   // HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_reg
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_reg, HDMITX21_PACKET_CONTROL_HDMI21_AUD_CONV_TOP_0_hdmi_tx_spdif_en(enable));
}

void HDMITX_DV_Script_hdmitx21_mac_config(void)
{
    rtd_inl(0xb8018410); //0x00000000);
    //rtd_outl(0xb8018410, 0x00000080);   // HDMITX21_MAC_HDMI21_IN_CONV_0_reg,
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_0_reg, HDMITX21_MAC_HDMI21_IN_CONV_0_vesa_in_pixel_mode(2)); // 4 pixel mode

    rtd_inl(0xb80187c0); //0x00000000);
    //rtd_outl(0xb80187c0, 0x000000f8);   // HDMITX21_MAC_HDMI21_ENC_0_reg
    rtd_outl(HDMITX21_MAC_HDMI21_ENC_0_reg, 0x000000f0);    // enc_en_l0~3 enable
    rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~HDMITX21_MAC_HDMI21_ENC_0_data_mux_mask, HDMITX21_MAC_HDMI21_ENC_0_data_mux(1)); // 0: data from LT, 1: data from FRL

    rtd_inl(0xb8018418); //0x00000000);
    //rtd_outl(0xb8018418, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_2_reg
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_2_reg, 0); // unvalid phase number

    rtd_inl(0xb801841c); //0x00000000);
    //rtd_outl(0xb801841c, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_3_reg
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_3_reg, 0); // unvalid phase number

    rtd_inl(0xb8018420); //0x00000000);
    //rtd_outl(0xb8018420, 0x00000004);   // HDMITX21_MAC_HDMI21_IN_CONV_4_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DV_ACT >> 8; // 0x4
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_4_reg, HDMITX21_MAC_HDMI21_IN_CONV_4_v_height_13_8(value_u16)); // v-len B[13:8]

    rtd_inl(0xb8018424); //0x00000000);
    //rtd_outl(0xb8018424, 0x00000038);   // HDMITX21_MAC_HDMI21_IN_CONV_5_reg
    value_u16 = pScalerHdmiTimingInfo->DTG_DV_ACT & 0xff;   // 0x38
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_5_reg, HDMITX21_MAC_HDMI21_IN_CONV_5_v_height_7_0(value_u16));  // v-len B[7:0]

    rtd_inl(0xb8018428); //0x00000000);
    //rtd_outl(0xb8018428, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_6_reg
    value_u16 = (pScalerHdmiTimingInfo->DTG_V_DEN_STA - pScalerHdmiTimingInfo->DTG_DVS_LENGTH - 1) >> 8; // 0
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_6_reg, HDMITX21_MAC_HDMI21_IN_CONV_6_v_bporch_15_8(value_u16)); // v-bporch B[15:8]

    rtd_inl(0xb801842c); //0x00000000);
    //rtd_outl(0xb801842c, 0x00000024);   // HDMITX21_MAC_HDMI21_IN_CONV_7_reg
    // v_bporch = v-start - v-sync = 41 - 5 = 38
    value_u16 = (pScalerHdmiTimingInfo->DTG_V_DEN_STA - pScalerHdmiTimingInfo->DTG_DVS_LENGTH - 1)  & 0xff; // 0x24(37-1) ->38-1
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_7_reg, HDMITX21_MAC_HDMI21_IN_CONV_7_v_bporch_7_0(value_u16)); // v-bporch B[7:0] -1

    rtd_inl(0xb8018430); //0x00000000);
    //rtd_outl(0xb8018430, 0x00000000);   // HDMITX21_MAC_HDMI21_IN_CONV_8_reg
    value_u16 = (pScalerHdmiTimingInfo->DTG_V_PORCH / 2) >> 8; // 0
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_8_reg, HDMITX21_MAC_HDMI21_IN_CONV_8_v_blank_half_15_8(value_u16));

    rtd_inl(0xb8018434); //0x00000000);
    // rtd_outl(0xb8018434, 0x00000016);   // HDMITX21_MAC_HDMI21_IN_CONV_9_reg
    value_u16 = (pScalerHdmiTimingInfo->DTG_V_PORCH / 2) & 0xff; // 0x16 (22) -> 45/2=22
    rtd_outl(HDMITX21_MAC_HDMI21_IN_CONV_9_reg, HDMITX21_MAC_HDMI21_IN_CONV_9_v_blank_half_7_0(value_u16));

    rtd_inl(0xb8018590); //0x000000ff);
    //rtd_outl(0xb8018590, 0x0000007a);   // HDMITX21_MAC_HDMI21_SCHEDULER_4_reg
    // usKeepoutStart1st = usHStart - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE(70); when usHStart > _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE(70)
    value_u16 = (pScalerHdmiTimingInfo->DTG_H_DEN_STA - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE) & 0xff; // 0x7a(122) -> 192 - 70 = 122
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_4_reg, HDMITX21_MAC_HDMI21_SCHEDULER_4_video_pre_keepout_start_1st_7_0(value_u16));

    rtd_inl(0xb801858c); //0x0000007f);
    //rtd_outl(0xb801858c, 0x00000000);   // HDMITX21_MAC_HDMI21_SCHEDULER_3_reg
    value_u16 = (pScalerHdmiTimingInfo->DTG_H_DEN_STA - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE) >> 8; // 0x7a(122) -> 192 - 70 = 122
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_3_reg, HDMITX21_MAC_HDMI21_SCHEDULER_3_video_pre_keepout_start_1st_14_8(value_u16));
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_3_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_3_hdcp22_enc_en_mask, HDMITX21_MAC_HDMI21_SCHEDULER_3_hdcp22_enc_en(0));

    rtd_inl(0xb8018594); //0x0000001f);
    //rtd_outl(0xb8018594, 0x00000000);   // HDMITX21_MAC_HDMI21_SCHEDULER_5_reg
    // usKeepoutStart = usHTotal - usHWidth - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE(70);
    value_u16 = (pScalerHdmiTimingInfo->DTG_H_PORCH - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE)>>8; // 0xd2(210) -> 280-70=210
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_5_reg, HDMITX21_MAC_HDMI21_SCHEDULER_5_video_pre_keepout_start_12_8(value_u16));

    rtd_inl(0xb8018598); //0x000000ff);
    //rtd_outl(0xb8018598, 0x000000d2);   // HDMITX21_MAC_HDMI21_SCHEDULER_6_reg
    value_u16 = (pScalerHdmiTimingInfo->DTG_H_PORCH - _HDMI21_MAC_TX0_PRE_KEEP_OUT_SIZE)& 0xff; // 0xd2(210) -> 280-70=210
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_6_reg, HDMITX21_MAC_HDMI21_SCHEDULER_6_video_pre_keepout_start_7_0(value_u16));

    rtd_inl(0xb80185ac); //0x000000fc);
    //rtd_outl(0xb80185ac, 0x000000f8);   // HDMITX21_MAC_HDMI21_SCHEDULER_10_reg
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_10_reg, HDMITX21_MAC_HDMI21_SCHEDULER_10_hdcp_keepout_win_start_7_0(0xf8));

    rtd_inl(0xb80185b4); //0x0000008a);
    //rtd_outl(0xb80185b4, 0x0000008c);   // HDMITX21_MAC_HDMI21_SCHEDULER_13_reg
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_13_reg, HDMITX21_MAC_HDMI21_SCHEDULER_13_hdcp_keepout_win_end_7_0(0x8c));

    rtd_inl(0xb801859c); //0x0000003a);
    //rtd_outl(0xb801859c, 0x00000000);   // HDMITX21_MAC_HDMI21_SCHEDULER_15_reg
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_15_reg, HDMITX21_MAC_HDMI21_SCHEDULER_15_hdcp_opp_win_start_7_0(0));

    rtd_inl(0xb8018490); //0x00000020);
    //rtd_outl(0xb8018490, 0x00000020);   // HDMITX21_MAC_HDMI21_422_CONV_reg
    rtd_outl(HDMITX21_MAC_HDMI21_422_CONV_reg, HDMITX21_MAC_HDMI21_422_CONV_pixelencfmt(0));
    rtd_maskl(HDMITX21_MAC_HDMI21_422_CONV_reg, ~HDMITX21_MAC_HDMI21_422_CONV_ycbcr422_8bits_mask, HDMITX21_MAC_HDMI21_422_CONV_ycbcr422_8bits(1));

    rtd_inl(0xb80184c4); //0x00000000);
    rtd_inl(0xb80184cc); //0x00000008);
    rtd_inl(0xb80184d0); //0x00000020);
    //rtd_outl(0xb80184c4, 0x000000a0);   // HDMITX21_MAC_HDMI21_DPC_0_reg
    rtd_outl(HDMITX21_MAC_HDMI21_DPC_0_reg, HDMITX21_MAC_HDMI21_DPC_0_dp_color_depth(4));
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_0_reg, ~HDMITX21_MAC_HDMI21_DPC_0_dp_enable_mask, HDMITX21_MAC_HDMI21_DPC_0_dp_enable(1));

    //rtd_outl(0xb80184cc, 0x00000008);   // HDMITX21_MAC_HDMI21_DPC_2_reg
    value_u8 = 1; // 8 ->1: tx driver setting
    rtd_outl(HDMITX21_MAC_HDMI21_DPC_2_reg, HDMITX21_MAC_HDMI21_DPC_2_dpc_sm_rd_start_pos(value_u8)); // default value

    //rtd_outl(0xb80184d0, 0x00000020);   // HDMITX21_MAC_HDMI21_DPC_3_reg
    rtd_outl(HDMITX21_MAC_HDMI21_DPC_3_reg, HDMITX21_MAC_HDMI21_DPC_3_dp_vch_num(1)); // default value

    rtd_inl(0xb80184e4); //0x00000000);
    //rtd_outl(0xb80184e4, 0x000000c0);   // HDMITX21_MAC_HDMI21_FORMAT_TOP_reg
    rtd_outl(HDMITX21_MAC_HDMI21_FORMAT_TOP_reg, HDMITX21_MAC_HDMI21_FORMAT_TOP_format_top_mux(3)); // 1: dp, 2: dsc, 3: vesa

    rtd_inl(0xb8018588); //0x00000000);
    //rtd_outl(0xb8018588, 0x000000a0);   // HDMITX21_MAC_HDMI21_SCHEDULER_2_reg
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_en(1)|HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute(1));
    //rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_2_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute_mask, HDMITX21_MAC_HDMI21_SCHEDULER_2_gcp_clravmute(1));

    rtd_inl(0xb8018580); //0x00000011);
    //rtd_outl(0xb8018580, 0x00000091);   // HDMITX21_MAC_HDMI21_SCHEDULER_0_reg
    rtd_outl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, HDMITX21_MAC_HDMI21_SCHEDULER_0_vblank_island_max_pkt_num(0x11)); // default value
    rtd_maskl(HDMITX21_MAC_HDMI21_SCHEDULER_0_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_0_scheduler_en_mask, HDMITX21_MAC_HDMI21_SCHEDULER_0_scheduler_en(1));

    rtd_inl(0xb80186a8); //0x00000000);
    //rtd_outl(0xb80186a8, 0x00000001);   // HDMITX21_MAC_HDCP22_ENGINE_40_reg
    rtd_outl(HDMITX21_MAC_HDCP22_ENGINE_40_reg, HDMITX21_MAC_HDCP22_ENGINE_40_hdcp_rst_n(1));

    rtd_inl(0xb80186c0); //0x00000070);
    //rtd_outl(0xb80186c0, 0x000000f0);   // HDMITX21_MAC_HDMI21_RC_reg
    rtd_outl(HDMITX21_MAC_HDMI21_RC_reg, HDMITX21_MAC_HDMI21_RC_rc_compress_max(0x7)); // default value
    rtd_maskl(HDMITX21_MAC_HDMI21_RC_reg, ~HDMITX21_MAC_HDMI21_RC_rc_en_mask, HDMITX21_MAC_HDMI21_RC_rc_en(1));

    rtd_inl(0xb80186cc); //0x00000000);
    //rtd_outl(0xb80186cc, 0x00000000);   // HDMITX21_MAC_HDMI21_FRL_0_reg
    rtd_outl(HDMITX21_MAC_HDMI21_FRL_0_reg, HDMITX21_MAC_HDMI21_FRL_0_frl_en(0));
    if(ucLaneNum > 3) // 4Lane
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~HDMITX21_MAC_HDMI21_FRL_0_frl_lane_num_mask, HDMITX21_MAC_HDMI21_FRL_0_frl_lane_num(1));

    rtd_inl(0xb8018780); //0x00000000);
    //rtd_outl(0xb8018780, 0x00000080);   // HDMITX21_MAC_HDMI21_RS_0_reg
    rtd_outl(HDMITX21_MAC_HDMI21_RS_0_reg, HDMITX21_MAC_HDMI21_RS_0_rs_en(1));

    rtd_inl(0xb8018790); //0x00000000);
    //rtd_outl(0xb8018790, 0x000000f0);   // HDMITX21_MAC_HDMI21_SCR_0_reg
    rtd_outl(HDMITX21_MAC_HDMI21_SCR_0_reg, 0x000000f0);    // scr_en_l 0~3 scramble function=1

    rtd_inl(0xb80187c0); //0x000000f8);
    //rtd_outl(0xb80187c0, 0x000000f8);   // HDMITX21_MAC_HDMI21_ENC_0_reg
    value_u16 = (HDMITX21_MAC_HDMI21_ENC_0_enc_en_l3_mask|HDMITX21_MAC_HDMI21_ENC_0_enc_en_l2_mask|HDMITX21_MAC_HDMI21_ENC_0_enc_en_l1_mask|HDMITX21_MAC_HDMI21_ENC_0_enc_en_l0_mask);
    rtd_outl(HDMITX21_MAC_HDMI21_ENC_0_reg, value_u16); // lane 0~3 16b8b encode=1
    rtd_maskl(HDMITX21_MAC_HDMI21_ENC_0_reg, ~HDMITX21_MAC_HDMI21_ENC_0_data_mux_mask, HDMITX21_MAC_HDMI21_ENC_0_data_mux(1)); // 0: data fraom LT, 1: data from frl

}


void HDMITX_DV_Script_hdmitx21_packet_infoframe_control(UINT8 enable)
{
    rtd_outl(0xb8018894, 0x0000000d);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_1_reg
    rtd_outl(0xb8018898, 0x00000002);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_2_reg
    rtd_outl(0xb801889c, 0x00000082);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_3_reg
    rtd_outl(0xb80188a0, 0x000000c0);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_4_reg
    rtd_outl(0xb80188a4, 0x000000d9);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_5_reg
    rtd_outl(0xb80188a8, 0x00000010);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_6_reg
    rtd_outl(0xb80188ac, 0x000000a5);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_7_reg
    rtd_outl(0xb80188b4, 0x00000000);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_9_reg
    rtd_outl(0xb80188b8, 0x00000020);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_10_reg
    // VIC
    //rtd_outl(0xb80188bc, 0x00000010);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg
    value_u16 = pHdmitxOutputTimingTable->vic;
    rtd_outl(0xb80188bc, value_u16);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_11_reg

    rtd_outl(0xb80188c0, 0x00000000);
    rtd_outl(0xb80188c4, 0x00000000);
    rtd_outl(0xb80188c8, 0x00000000);
    rtd_outl(0xb80188cc, 0x00000000);
    rtd_outl(0xb80188d4, 0x00000000);
    rtd_outl(0xb80188d8, 0x00000000);
    rtd_outl(0xb80188dc, 0x00000000);
    rtd_outl(0xb80188e0, 0x00000000);
    rtd_outl(0xb80188e4, 0x00000000);
    rtd_outl(0xb80188e8, 0x00000000);
    rtd_outl(0xb80188ec, 0x00000000);
    rtd_outl(0xb80188f4, 0x00000000);
    rtd_outl(0xb80188f8, 0x00000000);
    rtd_outl(0xb80188fc, 0x00000000);
    rtd_outl(0xb8018900, 0x00000000);
    rtd_outl(0xb8018904, 0x00000000);
    rtd_outl(0xb8018908, 0x00000000);
    rtd_outl(0xb801890c, 0x00000000);
    rtd_outl(0xb8018914, 0x00000000);
    rtd_outl(0xb8018918, 0x00000000);
    rtd_outl(0xb801891c, 0x00000000);   // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_35_reg

    rtd_outl(0xb8018944, 0x00000000);   // HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_1_reg
    rtd_outl(0xb8018948, 0x00000004);   // HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_2_reg
    rtd_inl(0xb8018940); //0x00000000);
    //rtd_outl(0xb8018940, 0x00000008);   // HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg, HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_sw_pkt_write_en(1));

    rtd_inl(0xb8018940); //0x00000000);
    rtd_inl(0xb8018940); //0x00000000);
    rtd_inl(0xb8018940); //0x00000001);
    //rtd_outl(0xb8018940, 0x00000001);   // HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_reg, HDMITX21_PACKET_CONTROL_HDMI21_PKT_CTRL_0_sw_pkt_bypass_done(1));

    rtd_inl(0xb8018938); //0x00000000);
    //rtd_outl(0xb8018938, 0x00000001); // HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg
    rtd_outl(HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_reg, HDMITX21_PACKET_CONTROL_HDMI21_SW_PKT_42_pr_spkt0_en(enable)); // enable SW PKT0 for AVI info frame

    return;
}


void HDMITX_DV_Script_hdmitx21_mac_enable(UINT8 enable)
{
    //NoteMessageHDMITx("[HDMITX] HDMITX_DV_Script_hdmitx21_mac_enable=%d\n", (UINT32) enable);

    //rtd_outl(0xb801840c, 0x00000080);   // [?] HDMITX21_MAC_HDMI21_TX_CTRL_reg
    rtd_maskl(HDMITX21_MAC_HDMI21_TX_CTRL_reg, ~HDMITX21_MAC_HDMI21_TX_CTRL_hdmi_21_tx_en_mask, HDMITX21_MAC_HDMI21_TX_CTRL_hdmi_21_tx_en(enable));

    //rtd_outl(0xb80186cc, 0x00000080);   // [?] HDMITX21_MAC_HDMI21_FRL_0_reg
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_0_reg, ~HDMITX21_MAC_HDMI21_FRL_0_frl_en_mask, HDMITX21_MAC_HDMI21_FRL_0_frl_en(enable));

    return;
}



void HDMITX_DV_Script_TC_PTG_hdmi21_ptg(void)
{
    NoteMessageHDMITx("[HDMITX] hdmi21_ptg...\n");

    // CRT
    HDMITX_DV_Script_crt_init();

    // source_sel
    HDMITX_DV_Script_hdmitx21_on_region_source_sel(0xf);

    // TX PLL
    HDMITX_DV_Script_hdmitx21_hdmitx_phy_pll();

    //
    //rtd_inl(0xb8018580); //0x00000011); // HDMITX21_MAC_HDMI21_SCHEDULER_0_reg

    // tx timing gen
    HDMITX_DV_Script_hdmitx_timing_gen_cfg();

    // tx pattern gen
    HDMITX_DV_Script_hdmitx_pattern_gen_cfg(_ENABLE);

    // CRC en
    HDMITX_DV_Script_hdmitx_misc_crcEn();

    // HDMI21 TX packet audio control
    HDMITX_DV_Script_hdmitx21_packet_audio_control(_DISABLE);

    // source_sel
    HDMITX_DV_Script_hdmitx21_on_region_source_sel(0x0);

    // HDMI21 TX MAC
    HDMITX_DV_Script_hdmitx21_mac_config();

    // DSC SRC SEL
    // 0 => 2pto4p mode for HDMI21, 1 => bypass mode for HDMI20
    HDMITX_DV_Script_hdmitx_dsc_src_sel(0);


    // HDMITX timing gen
    HDMITX_DV_Script_hdmitx_timing_gen_enable(_ENABLE);
    HDMITX_DV_Script_hdmitx_frame_rate_measure_en(_ENABLE);
    delayus(10);

    HDMITX_DV_Script_hdmitx21_mac_enable(_ENABLE);
    delayus(100);

    // FIFO reset
    HDMITX_DV_Script_hdmitx21_reset_fifoErr();
    ucCheckCnt=0;

    // HDMI21 TX packet InfoFrame control
    HDMITX_DV_Script_hdmitx21_packet_infoframe_control(_ENABLE);

    NoteMessageHDMITx("[HDMITX] hdmi21_ptg Done!\n");
    return;
}


void HDMITX_DV_Script_hdmitx20_reset_fifoErr(void)
{
    rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_2_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT7 | _BIT6));
    rtd_maskl(HDMITX20_ON_REGION_PIXEL_CONV_P1_reg, ~(_BIT6|_BIT5), (_BIT6|_BIT5));
    rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~_BIT6, _BIT6);
    rtd_maskl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, ~(_BIT1|_BIT0), (_BIT1|_BIT0));
}

void HDMITX_DV_Script_hdmitx20_check_fifoErr(void)
{

    hdmitx20_mac0_hdmi_control_2_regValue = rtd_inl(HDMITX20_MAC0_HDMI_CONTROL_2_reg);
    hdmitx20_mac1_hdmi_input_ctsfifo_regValue= rtd_inl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg);
    hdmitx_phy_dsc_insel_fifo_status_regValue = rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg);
    usFrameCnt = HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_get_frame_cnt(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_reg));
    bErrFlag = 0;

    bFifoOvf = HDMITX20_MAC0_HDMI_CONTROL_2_get_sram_ovflow_flag(hdmitx20_mac0_hdmi_control_2_regValue);
    bFifoUdf = HDMITX20_MAC0_HDMI_CONTROL_2_get_sram_udflow_flag(hdmitx20_mac0_hdmi_control_2_regValue);
    if(hdmitx20_mac0_hdmi_control_2_regValue & (HDMITX20_MAC0_HDMI_CONTROL_2_sram_udflow_flag_mask|HDMITX20_MAC0_HDMI_CONTROL_2_sram_ovflow_flag_mask)){
        NoteMessageHDMI21Tx("[HDMITX][%d] 2.0 SRAM FIFO ERR=%d|%d\n", (UINT32)bFifoOvf, (UINT32)bFifoUdf);
        rtd_maskl(HDMITX20_MAC0_HDMI_CONTROL_2_reg, ~(HDMITX20_MAC0_HDMI_CONTROL_2_sram_udflow_flag_mask|HDMITX20_MAC0_HDMI_CONTROL_2_sram_ovflow_flag_mask),
                                                                                                hdmitx20_mac0_hdmi_control_2_regValue);
    }
    if(hdmitx20_mac1_hdmi_input_ctsfifo_regValue & HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_fifo_unstable_mask){
        NoteMessageHDMI21Tx("[HDMITX] 2.0 CTS FIFO UNSTABLE@%x\n", (UINT32)hdmitx20_mac1_hdmi_input_ctsfifo_regValue);
        rtd_maskl(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_reg, ~(HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_fifo_unstable_mask), HDMITX20_MAC1_HDMI_INPUT_CTSFIFO_fifo_unstable_mask);
    }

    if(ENABLE_HDMI_REPEATER_MODE == 1){ // TX input timing gen (free run)
        bFifoOvf = HDMITX_PHY_DSC_INSEL_FIFO_STATUS_get_dispd_fifo_of(hdmitx_phy_dsc_insel_fifo_status_regValue);
        bFifoUdf = HDMITX_PHY_DSC_INSEL_FIFO_STATUS_get_dispd_fifo_uf(hdmitx_phy_dsc_insel_fifo_status_regValue);
        if(bFifoOvf|| bFifoUdf){
            NoteMessageHDMI21Tx("[HDMITX] 2.0 DSCE DISP FIFO Err=%d|%d\n", (UINT32)bFifoOvf, (UINT32)bFifoUdf);
            rtd_outl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, hdmitx_phy_dsc_insel_fifo_status_regValue);
            bErrFlag = 1;
        }
    }

    if(bErrFlag==0)
        NoteMessageHDMI21Tx("[HDMI20_TX][%d] No Err Detect@Frame[%d]!\n", (UINT32)ucCheckCnt, (UINT32)usFrameCnt);

    return;
}


void HDMITX_DV_Script_hdmitx21_reset_fifoErr(void)
{
    rtd_maskl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg, ~HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_vesa_fifo_unstable_mask, HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_vesa_fifo_unstable_mask);
    rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~HDMITX21_MAC_HDMI21_FRL_ERR, HDMITX21_MAC_HDMI21_FRL_ERR);
    rtd_maskl(HDMITX21_MAC_HDMI21_DPC_1_reg, ~(HDMITX21_MAC_HDMI21_DPC_1_dpc_sm_udf_mask|HDMITX21_MAC_HDMI21_DPC_1_dpc_sm_ovf_mask), HDMITX21_MAC_HDMI21_DPC_1_dpc_sm_udf_mask|HDMITX21_MAC_HDMI21_DPC_1_dpc_sm_ovf_mask);
    rtd_maskl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, ~(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_dispd_fifo_uf_mask|HDMITX_PHY_DSC_INSEL_FIFO_STATUS_dispd_fifo_of_mask), HDMITX_PHY_DSC_INSEL_FIFO_STATUS_dispd_fifo_uf_mask|HDMITX_PHY_DSC_INSEL_FIFO_STATUS_dispd_fifo_of_mask);

}


void HDMITX_DV_Script_hdmitx21_check_fifoErr(void)
{

    // check VESA CTS FIFO && FRL SRAM status
    hdmitx21_mac_hdmi21_vesa_cts_fifo_regValue = rtd_inl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg);
    hdmitx21_mac_hdmi21_frl_23_regValue = rtd_inl(HDMITX21_MAC_HDMI21_FRL_23_reg);
    hdmitx21_mac_hdmi21_dpc_1_regValue = rtd_inl(HDMITX21_MAC_HDMI21_DPC_1_reg);
    hdmitx_phy_dsc_insel_fifo_status_regValue = rtd_inl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg);
    usFrameCnt = HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_get_frame_cnt(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_reg));

    bErrFlag = 0;

    if(hdmitx21_mac_hdmi21_vesa_cts_fifo_regValue & HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_vesa_fifo_unstable_mask){
        NoteMessageHDMI21Tx("[HDMI21_TX] VESA CTS FIFO UnStable=%x!!\n", (UINT32)hdmitx21_mac_hdmi21_vesa_cts_fifo_regValue);
        rtd_maskl(HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_reg, ~HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_vesa_fifo_unstable_mask, HDMITX21_MAC_HDMI21_VESA_CTS_FIFO_vesa_fifo_unstable_mask);
        bErrFlag=1;
    }

    if(hdmitx21_mac_hdmi21_frl_23_regValue & HDMITX21_MAC_HDMI21_FRL_ERR){
        NoteMessageHDMI21Tx("[HDMI21_TX] FRL FIFO Err=%x!!\n", (UINT32)hdmitx21_mac_hdmi21_frl_23_regValue);
        rtd_maskl(HDMITX21_MAC_HDMI21_FRL_23_reg, ~HDMITX21_MAC_HDMI21_FRL_ERR, hdmitx21_mac_hdmi21_frl_23_regValue);
        bErrFlag=1;
    }

    bFifoOvf = HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_ovf(hdmitx21_mac_hdmi21_dpc_1_regValue);
    bFifoUdf = HDMITX21_MAC_HDMI21_DPC_1_get_dpc_sm_udf(hdmitx21_mac_hdmi21_dpc_1_regValue);
    if(bFifoOvf|| bFifoUdf){
        NoteMessageHDMI21Tx("[HDMI21_TX] DPC FIFO Err=%d|%d\n", (UINT32)bFifoOvf, (UINT32)bFifoUdf);
        rtd_outl(HDMITX21_MAC_HDMI21_DPC_1_reg, hdmitx21_mac_hdmi21_dpc_1_regValue);
        bErrFlag=1;
    }

    if(ENABLE_HDMI_REPEATER_MODE == 1){ // TX input timing gen (free run)
        bFifoOvf = HDMITX_PHY_DSC_INSEL_FIFO_STATUS_get_dispd_fifo_of(hdmitx_phy_dsc_insel_fifo_status_regValue);
        bFifoUdf = HDMITX_PHY_DSC_INSEL_FIFO_STATUS_get_dispd_fifo_uf(hdmitx_phy_dsc_insel_fifo_status_regValue);
        if(bFifoOvf|| bFifoUdf){
            NoteMessageHDMI21Tx("[HDMI21_TX] DSCE DISP FIFO Err=%d|%d\n", (UINT32)bFifoOvf, (UINT32)bFifoUdf);
            rtd_outl(HDMITX_PHY_DSC_INSEL_FIFO_STATUS_reg, hdmitx_phy_dsc_insel_fifo_status_regValue);
            bErrFlag = 1;
        }
    }

    if(bErrFlag==0)
        NoteMessageHDMI21Tx("[HDMI21_TX][%d] No Err Detect@Frame[%d]!\n", (UINT32)ucCheckCnt, (UINT32)usFrameCnt);
    //else if(usFrameCnt_pre != usFrameCnt)
    //    NoteMessageHDMI21Tx("[HDMI21_TX][%d] Frame[%d]!\n", (UINT32)ucCheckCnt, (UINT32)usFrameCnt);
    //usFrameCnt_pre = usFrameCnt;

    return;
}



void HDMITX_DV_Script_test_info(void) __banked
{
    FatalMessageHDMITx("[HDMITX][BANK][6] HDMITX_DV_Script_test_info, REPEATER MODE=%d\n", (UINT32)ENABLE_HDMI_REPEATER_MODE);
    FatalMessageHDMITx("[HDMITX] Load Script for TX/RX=%d/%d!!\n", (UINT32) ENABLE_LOAD_HDMITX_SCRIPT, (UINT32) ENABLE_LOAD_HDMIRX_SCRIPT);

    if(HDMITX_INPUT_TIMING_MODE < HDMITX_INPUT_TIMING_NUM){
        FatalMessageHDMITx("[HDMITX] TX Output[%d]: %s\n", (UINT32)HDMITX_INPUT_TIMING_MODE, timingTableStr[HDMITX_INPUT_TIMING_MODE]);
    }else{
        FatalMessageHDMITx("[HDMITX] TX Output: UNKNOW[%d]!", (UINT32)HDMITX_INPUT_TIMING_MODE);
    }

    return;
}



void HDMITX_DV_Script_check_hdmitx_vsyncPeriod(void)
{
    // DSCE Timing gen
    if((rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg) & HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask) == 0){
        rtd_maskl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg, ~HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask, HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_frame_rate_measure_en_mask);
        NoteMessageHDMI21Tx("[HDMI21_TX] FrameRate Measure Enable!\n");
    }else{
        usFrameCnt = HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_get_frame_cnt(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_reg));
        ErrorMessageHDMI21Tx("[HDMI21_TX][%d] FrameCnt=%d\n", (UINT32)ucCheckCnt, (UINT32)usFrameCnt);
        if(usFrameCnt_pre != usFrameCnt){
            uStcCnt = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
            stcDiff = (UINT16)((uStcCnt - uStcCnt_lastDump)/90);
            value_u32 = (UINT32)250000000UL / HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_get_vs2vs_cycle(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE_reg));
            ErrorMessageHDMI21Tx("VSync=%d(Hz)@StcDiff=%x(ms)\n", (UINT32)value_u32, (UINT32)stcDiff);
            usFrameCnt_pre = usFrameCnt;
            uStcCnt_lastDump = uStcCnt;
        }
    }

    return;
}


#define AVI_INFO_OFST       0
#define SPD_INFO_OFST       294
#define FVS_INFO_OFST       252
#define DVS_INFO_OFST       349
#define DRM_INFO_OFST       264
#define EM_VTEM_INFO_OFST  192
#define EM_VSEM_INFO_OFST  224

#define AVI_INFO_LEN        17 // 14+3
#define SPD_INFO_LEN       28 // 25+3
#define FVS_INFO_LEN        12
#define DRM_INFO_LEN        30
#define DVS_INFO_LEN        31
#define EM_VTEM_INFO_LEN  32
#define EM_VSEM_INFO_LEN  32
#define MAX_INFO_LEN    32
#define INFO_HDR_LEN    3

#define TYPE_CODE_VS_PACKET       0x81
#define TYPE_CODE_AVI_PACKET      0x82
#define TYPE_CODE_SPD_PACKET     0x83
#define TYPE_CODE_DRM_PACKET     0x87
#define TYPE_CODE_EMP_PACKET      0x7F

typedef enum {
    INFOFRAME_TYPE_AVI=0,
    INFOFRAME_TYPE_FVSI,
    INFOFRAME_TYPE_DVSI,
    INFOFRAME_TYPE_SPD,
    INFOFRAME_TYPE_DRM,
    INFOFRAME_TYPE_VTEM,
    INFOFRAME_TYPE_VSEM,
    INFOFRAME_NUM,
}ENUM_INFOFRAME_TYPE;


typedef struct{
    UINT8 type_code;
    UINT8 pkt_len;
    UINT16 info_offset;
    UINT32 ps_mask;
    UINT8 bBchErr_flag; // 1st data of Packet Storage SRAM for bch error status
    UINT8 code_str[4];
} stInfoFramePkt_info;

stInfoFramePkt_info code info_packet_table[INFOFRAME_NUM] = {
    {TYPE_CODE_AVI_PACKET, AVI_INFO_LEN, AVI_INFO_OFST, HDMI_P0_HDMI_GPVS_avips_mask, _TRUE, "AVI"}, /* INFOFRAME_TYPE_AVI*/
    {TYPE_CODE_VS_PACKET, FVS_INFO_LEN, FVS_INFO_OFST, HDMI_P0_HDMI_GPVS_fvsps_mask, _FALSE, "FVSI"}, /* INFOFRAME_TYPE_FVSI */
    {TYPE_CODE_VS_PACKET, DVS_INFO_LEN, DVS_INFO_OFST, HDMI_P0_HDMI_GPVS_dvsps_mask, _FALSE, "DVSI"}, /* INFOFRAME_TYPE_DVSI */
    {TYPE_CODE_SPD_PACKET, SPD_INFO_LEN, SPD_INFO_OFST, HDMI_P0_HDMI_GPVS_spdps_mask, _TRUE, "SPD"}, /* INFOFRAME_TYPE_SPD */
    {TYPE_CODE_DRM_PACKET, DRM_INFO_LEN, DRM_INFO_OFST, HDMI_P0_HDMI_GPVS_drmps_mask, _TRUE, "DRM"}, /* INFOFRAME_TYPE_DRM */
    {TYPE_CODE_EMP_PACKET, EM_VTEM_INFO_LEN, EM_VTEM_INFO_OFST, HDMI_P0_VRREM_ST_em_vrr_no_mask, _FALSE, "VTEM"}, /* INFOFRAME_TYPE_VTEM */
    {TYPE_CODE_EMP_PACKET, EM_VSEM_INFO_LEN, EM_VSEM_INFO_OFST, 0, _FALSE, "VSEM"}, /* INFOFRAME_TYPE_VSEM */
};

static stInfoFramePkt_info code *pInfo_packet_table=NULL;
static UINT8 p_bch=0;
static UINT8 pkt_hdr[INFO_HDR_LEN];
static UINT8 pkt_buf[MAX_INFO_LEN];

#pragma save
#pragma nogcse /* turnoff global subexpression elimination */
void  HDMITX_DV_Script_check_HDMI_RX_PS(ENUM_INFOFRAME_TYPE pkt_type)
{
    /* imem 3 */
    //UINT8 len_extend;
    UINT8 i = 0, empPs=0;

    if(pkt_type >= INFOFRAME_NUM){
        NoteMessageHDMITx("[HDMITX] Not Supported Packet Type[%d]\n", (UINT32)pkt_type);
        return;
    }

    pInfo_packet_table = (&info_packet_table[0])+pkt_type;

    // clear info status
    if(pkt_type == INFOFRAME_TYPE_VTEM){
        // write 1 clear no/first/last flag of emp
        regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_VRREM_ST_reg: HDMI_P0_VRREM_ST_reg);
        empPs = rtd_inl(regValue);
        pm_printk(LOGGER_ERROR, "VTEM status=%x\n", (UINT32)empPs);
        rtd_maskl(regValue, ~(HDMI_P0_VRREM_ST_em_vrr_no_mask|HDMI_P0_VRREM_ST_em_vrr_end_mask|HDMI_P0_VRREM_ST_em_vrr_new_mask|HDMI_P0_VRREM_ST_em_vrr_first_mask| HDMI_P0_VRREM_ST_em_vrr_last_mask), empPs);
    }else{
        regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_HDMI_GPVS_reg: HDMI_P0_HDMI_GPVS_reg);
        rtd_outl(regValue, pInfo_packet_table->ps_mask);
    }

    // check bch err status
    if(pInfo_packet_table->bBchErr_flag){
        regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_HDMI_PSAP_reg: HDMI_P0_HDMI_PSAP_reg);
        rtd_outl(regValue, pInfo_packet_table->info_offset);
        regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_HDMI_PSDP_reg: HDMI_P0_HDMI_PSDP_reg);
        p_bch = rtd_inl(regValue);
        if (p_bch != 0)
            pm_printk(LOGGER_ERROR, "%s BCH Error= %d\n", pInfo_packet_table->code_str, (UINT32)p_bch);
    }

    // check infoframe packet:
    if(pkt_type == INFOFRAME_TYPE_VTEM){
        //lib_hdmi_read_emp_sram(nport, EM_VTEM_INFO_OFST + EM_VTEM_INFO_LEN - 4, 4, pkt_buf); // HB0~2 + BCH
        for (i = 0; i < 4; i++) {
            regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_HDMI_EMAP_PP_reg: HDMI_P0_HDMI_EMAP_PP_reg);
            rtd_outl(regValue, (pInfo_packet_table->info_offset + pInfo_packet_table->pkt_len-4)+i);

            regValue = (ucHdmiMode == 1? HDMI21_P0_hd21_HDMI_EMDP_PP_reg: HDMI_P0_HDMI_EMDP_PP_reg);
            pkt_buf[i] = rtd_inl(regValue);
        }

        if (pkt_buf[3] != 0)
            pm_printk(LOGGER_ERROR, "VTEM BCH Error= %d\n", pkt_buf[3]);

        //lib_hdmi_read_emp_sram(nport, EM_VTEM_INFO_OFST, EM_VTEM_INFO_LEN - 4, pkt_buf + 3); // PB0~PB27
        for (i = 3; i < pInfo_packet_table->pkt_len-4; i++) {
            regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_HDMI_EMAP_PP_reg: HDMI_P0_HDMI_EMAP_PP_reg);
            rtd_outl(regValue, pInfo_packet_table->info_offset + i);

            regValue = (ucHdmiMode == 1? HDMI21_P0_hd21_HDMI_EMDP_PP_reg: HDMI_P0_HDMI_EMDP_PP_reg);
            pkt_buf[i] = rtd_inl(regValue);
        }

    }else{
        // newbase_hdmi_update_drm_info()
        for (i = 0; i < (pInfo_packet_table->pkt_len - pInfo_packet_table->bBchErr_flag); i++) {
            regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_HDMI_PSAP_reg: HDMI_P0_HDMI_PSAP_reg);
            rtd_outl(regValue, pInfo_packet_table->info_offset + pInfo_packet_table->bBchErr_flag + i);

            regValue = (ucHdmiMode == 1? HDMI21_P0_HD21_HDMI_PSDP_reg: HDMI_P0_HDMI_PSDP_reg);
            pkt_buf[i] = rtd_inl(regValue);
        }

        // skip checksum
        if(pInfo_packet_table->bBchErr_flag){
            pkt_hdr[0] = pInfo_packet_table->type_code;// Info Frame Type
            pkt_hdr[1] = pkt_buf[0];//version
            pkt_hdr[2] = pkt_buf[1];//len
            memcpy(pkt_buf, pkt_hdr,INFO_HDR_LEN);
        }else{
            pkt_buf[0] = pInfo_packet_table->type_code;// Info Frame Type
        }
    }

    pm_printk(LOGGER_ERROR, "[PKT] Pkt[%d][%s] detected ", (UINT32)pkt_type, pInfo_packet_table->code_str);
    pm_printk(LOGGER_ERROR, "Len=%x/%x:\n", (UINT32)i, (UINT32)pInfo_packet_table->pkt_len);
    for(i = 0; i < pInfo_packet_table->pkt_len; i++){
        pm_printk(LOGGER_ERROR, "%d ", (UINT32)pkt_buf[i]);
        if(i == 16)
            pm_printk(LOGGER_ERROR, "\n");
    }
    pm_printk(LOGGER_ERROR, "\n");

    return;
}
#pragma restore /* turn the optimizations back on */



void HDMITX_DV_Script_test_check(void) __banked
{

    NoteMessageHDMITx("[HDMITX][BANK][6] HDMITX_DV_Script_test_check@HDMI2.%d\n", (UINT32)ucHdmiMode);

    if(ENABLE_LOAD_HDMITX_SCRIPT){
        if(ucHdmiMode == 1){ // HDMI21
            HDMITX_DV_Script_hdmitx21_check_fifoErr();
        }else{
            HDMITX_DV_Script_hdmitx20_check_fifoErr();
        }

        // check timing gen vsync peroid
        HDMITX_DV_Script_check_hdmitx_vsyncPeriod();
    }else{
        usFrameCnt = HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_get_frame_cnt(rtd_inl(HDMITX_MISC_HDMITX_IN_TIMING_GEN_FRAME_RATE2_reg));
    }

    if(usFrameCnt != usFrameCnt_lastDump){
        if(ENABLE_HDMI_REPEATER_MODE){
            // DTG fsync mode
            value_u8 = PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_get_disp_fsync_en(rtd_inl(PPOVERLAY_OUTTG_OUTTG_Display_Timing_CTRL1_reg));
            value_u32 = rtd_inl(HDMITX_PHY_HDMITX_CLK_DIV_reg);
            NoteMessageHDMITx("[HDMITX] DTG Sync=%d, txClkRatio=%x\n", (UINT32)value_u8, (UINT32)value_u32);

            // ivs/dvs
            dtgIvsCnt = PPOVERLAY_OUTTG_OUTTG_IVS_cnt_get_ivs_cnt(rtd_inl(PPOVERLAY_OUTTG_OUTTG_IVS_cnt_reg));
            dtgDvsCnt = PPOVERLAY_OUTTG_OUTTG_DVS_cnt_get_dvs_cnt(rtd_inl(PPOVERLAY_OUTTG_OUTTG_DVS_cnt_reg));
            FatalMessageHDMITx("[HDMITX] IVS/DVS=%x/%x\n", (UINT32)dtgIvsCnt, (UINT32)dtgDvsCnt);
        }

#if 1 // VRR TEST
        if(VRR_EN == 1){
            // do nothing
            uStcCnt = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
            if(uStcCnt_lastDump){
                stcDiff = (UINT16)((uStcCnt - uStcCnt_lastDump)/90);
                FatalMessageHDMITx("[VRR] Frame[%x], stcDiff=%x\n", (UINT32)usFrameCnt, (UINT32)stcDiff);
            }
            uStcCnt_lastDump = uStcCnt;
        }
        else
#endif
        // show RX packet status
        if(ENABLE_SHOW_RX_PS){
            // HDMI20
            if(ucHdmiMode == 0){ // HDMI20
                rxPktVideoStatus = rtd_inl(HDMI_P0_HDMI_GPVS_reg);
                rxPktVideoStatus1 = rtd_inl(HDMI_P0_HDMI_GPVS1_reg);
                rxEmPktVideoStatus = rtd_inl(HDMI_P0_VRREM_ST_reg);
            }else { // HDMI21
                rxPktVideoStatus = rtd_inl(HDMI21_P0_HD21_HDMI_GPVS_reg);
                rxPktVideoStatus1 = rtd_inl(HDMI21_P0_HD21_HDMI_GPVS1_reg);
                rxEmPktVideoStatus = rtd_inl(HDMI21_P0_HD21_VRREM_ST_reg);
            }

            NoteMessageHDMITx("GPVS=%x@frameCnt=%x\n", (UINT32)rxPktVideoStatus, (UINT32)usFrameCnt);
            NoteMessageHDMITx("GPVS1/VRREM=%x/%x\n", (UINT32)rxPktVideoStatus1, (UINT32)rxEmPktVideoStatus);

            // check AVI info frame
            value_u32 = (ucHdmiMode==1? HDMI21_P0_HD21_HDMI_GPVS_avips_mask: HDMI_P0_HDMI_GPVS_avips_mask);
            if(rxPktVideoStatus & value_u32){
                // check AVI info frame info
                //HDMITX_DV_Script_check_HDMI20_RX_AVI_PS();
                HDMITX_DV_Script_check_HDMI_RX_PS(INFOFRAME_TYPE_AVI);
             }

            // check SPD info frame
            value_u32 = (ucHdmiMode==1? HDMI21_P0_HD21_HDMI_GPVS_spdps_mask: HDMI_P0_HDMI_GPVS_spdps_mask);
            if(rxPktVideoStatus & value_u32){
                // check SPD info frame info
                //HDMITX_DV_Script_check_HDMI20_RX_SPD_PS();
                HDMITX_DV_Script_check_HDMI_RX_PS(INFOFRAME_TYPE_SPD);
             }

            // check FVS info frame
            value_u32 = (ucHdmiMode==1? HDMI21_P0_HD21_HDMI_GPVS_fvsps_mask: HDMI_P0_HDMI_GPVS_fvsps_mask);
            if(rxPktVideoStatus & value_u32){
                // check FVS info frame info
                //HDMITX_DV_Script_check_HDMI20_RX_FVS_PS();
                HDMITX_DV_Script_check_HDMI_RX_PS(INFOFRAME_TYPE_FVSI);
             }

            // check DVS info frame
            value_u32 = (ucHdmiMode==1? HDMI21_P0_HD21_HDMI_GPVS_dvsps_mask: HDMI_P0_HDMI_GPVS_dvsps_mask);
            if(rxPktVideoStatus & value_u32){
                // check DVS info frame info
                //HDMITX_DV_Script_check_HDMI20_RX_DVS_PS();
                HDMITX_DV_Script_check_HDMI_RX_PS(INFOFRAME_TYPE_DVSI);
             }

            // check DRM (HDR10/HLG)
            value_u32 = (ucHdmiMode==1? HDMI21_P0_HD21_HDMI_GPVS_drmps_mask: HDMI_P0_HDMI_GPVS_drmps_mask);
            if(rxPktVideoStatus & value_u32){
                // check DRM info frame info
                HDMITX_DV_Script_check_HDMI_RX_PS(INFOFRAME_TYPE_DRM);
             }

            // check EMP (VTEM/VSEM)
            if(ucHdmiMode==1) // HDMI21
                value_u32 = (HDMI21_P0_HD21_VRREM_ST_em_vrr_end_mask|HDMI21_P0_HD21_VRREM_ST_em_vrr_new_mask|HDMI21_P0_HD21_VRREM_ST_em_vrr_first_mask|HDMI21_P0_HD21_VRREM_ST_em_vrr_last_mask);
            else // HDMI20
                value_u32 = (HDMI_P0_VRREM_ST_em_vrr_end_mask|HDMI_P0_VRREM_ST_em_vrr_new_mask|HDMI_P0_VRREM_ST_em_vrr_first_mask|HDMI_P0_VRREM_ST_em_vrr_last_mask);
            if(rxEmPktVideoStatus & value_u32){
                // check EMP info frame info
                HDMITX_DV_Script_check_HDMI_RX_PS(INFOFRAME_TYPE_VTEM);
             }
        }

        if (ENABLE_SHOW_TX_PTG == 1){ // TEST
            if((ucPtgToggleState == 0) && (usFrameCnt >= 5)){
                NoteMessageHDMITx("[HDMITX] PTG ON\n");
                HDMITX_DV_Script_hdmitx_pattern_gen_cfg(_ENABLE);
                ucPtgToggleState = 1;
            }else  if((ucPtgToggleState == 1) && (usFrameCnt >= 8)){
                NoteMessageHDMITx("[HDMITX] PTG OFF\n");
                HDMITX_DV_Script_hdmitx_pattern_gen_cfg(_DISABLE);
                ucPtgToggleState = 0;
            }
        }

        usFrameCnt_lastDump = usFrameCnt;
    }

    ++ucCheckCnt;
}


void HDMITX_DV_Script_test_run(void) __banked
{

    if((ENABLE_HDMI_REPEATER_MODE == 0) && (ENABLE_LOAD_HDMITX_SCRIPT == 0) && (ENABLE_LOAD_HDMIRX_SCRIPT == 0))
        return;

    if(HDMITX_INPUT_TIMING_MODE > HDMITX_INPUT_TIMING_NUM){
        FatalMessageHDMITx("[HDMITX] Timig Mode[%x] Over Range!!\n", (UINT32)HDMITX_INPUT_TIMING_MODE);
        return;
    }

    NoteMessageHDMITx("[HDMITX][BANK][6] HDMITX_DV_Script_test_run\n");

    hdmitx_init_setting.init_setting.bHdmitxSettingDoneFlag = ENABLE_LOAD_HDMITX_SCRIPT;
    hdmitx_init_setting.init_setting.bHdmirxSettingDoneFlag = ENABLE_LOAD_HDMIRX_SCRIPT;
    hdmitx_init_setting.init_setting.bPktBypassEnableFlag = ENABLE_HDMI_REPEATER_MODE;
    hdmitx_init_setting.init_setting.bSkipAudioPktBypassFlag = _SKIP_AUDIO_BYPASS;
    hdmitx_init_setting.init_setting.bSkipEMPPktBypassFlag = _SKIP_EMP_BYPASS;
    hdmitx_init_setting.init_setting.bDtgFSyncEnable = ENABLE_DTG_FSYNC;
    hdmitx_init_setting.init_setting.bVrrEnable = ENABLE_VRR_SUPPORT;
    hdmitx_init_setting.init_setting.bSkipEdidModify = _SKIP_EDID_MODIFY;

    pHdmitxOutputTimingTable = &hdmitxOutputTimingTable[0]+HDMITX_INPUT_TIMING_MODE;
    pScalerHdmiTimingInfo = &pHdmitxOutputTimingTable->scalerHdmiTimingInfo;
    total_h = pScalerHdmiTimingInfo->DTG_DH_ACT + pScalerHdmiTimingInfo->DTG_H_PORCH;
    total_v = pScalerHdmiTimingInfo->DTG_DV_ACT + pScalerHdmiTimingInfo->DTG_V_PORCH;

    // decide HDMI mode: 0: HDMI20, 1: HDMI21
    ucHdmiMode = (pHdmitxOutputTimingTable->frl_mode? 1: 0);
    ucLaneNum = (pHdmitxOutputTimingTable->frl_mode > 2? 4: 3);

    FatalMessageHDMITx("[HDMITX] TX Out: %d\n", (UINT32)HDMITX_INPUT_TIMING_MODE);
    FatalMessageHDMITx("Active: %dx%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_DH_ACT, (UINT32)pScalerHdmiTimingInfo->DTG_DV_ACT);
    FatalMessageHDMITx("VIC:%d@FRL:%d\n", (UINT32)pHdmitxOutputTimingTable->vic, (UINT32)pHdmitxOutputTimingTable->frl_mode);
    NoteMessageHDMITx("Total: %dx%d\n", (UINT32)total_h, (UINT32)total_v);
    NoteMessageHDMITx("Start: H=%d, V=%d\n", (UINT32)pScalerHdmiTimingInfo->DTG_H_DEN_STA , (UINT32)pScalerHdmiTimingInfo->DTG_V_DEN_STA);

    // rbus dump start
    rtd_outl(0xb801460c, 0x12345678);

    if(ENABLE_HDMI_REPEATER_MODE == 1){
        if(ucHdmiMode == 1){
            HDMITX_DV_Script_TC_Repeater_hdmi21_rxBypassToTx(); // HDMI21 RX 1080p60 bypass to HDMI21 TX 1080p60: Pass
        }else { // HDMI20
            HDMITX_DV_Script_Repeater_PTG_hdmi20(); // HDMI20 RX 4k2k30 bypass to HDMI21 TX 4k30/2k1k60: Pass
        }
    }else{
        if(ucHdmiMode == 1){ // HDMI21
            HDMITX_DV_Script_TC_PTG_hdmi21_ptg();   // HDMI21 TX 1080p60 output: Pass
        }else{ // HDMI20
            HDMITX_DV_Script_Repeater_PTG_hdmi20();    // HDMI20 4k2kp30 output: Pass
        }
    }

    // rbus dump end
    rtd_outl(0xb801460c, 0x87654321);

    return;
}

