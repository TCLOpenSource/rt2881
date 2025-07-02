
//#include <rtk_kdriver/pcbMgr.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_ctrl.h>
//#include <rtk_kdriver/panel/panel_config_parameter.h>
//#include <rtk_kdriver/panel/panel.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_lib.h>
#ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_script.h>
#endif
//#include <rtk_kdriver/tvscalercontrol/rtk_scaler.h>

//#include <rtk_kdriver/measure/measure.h>
//#include "scaler/scalerCommon.h"
//#include "rtk_kdriver/at_header.h"
//#include "rtk_kdriver/at_hdmitx_header.h"
//#include <rtk_platform.h>
//#include <rtk_mem_layout.h>
//#include "sys_share_dmem.h"

/************************
// Static/Local variable declaration
//
*************************/
// please check rtk_mem_layout.h HDMITX_EDID_SIZE
#define HDMITX_INNER_EDID_SIZE          1024

//PCB_ENUM_VAL HDMI_TX_I2C_BUSID;

//static UINT32 value_u32;
//extern UINT32 dma_hdmitx_edid_phys_addr;
extern UINT32 sys_share_dmem_virt_addr;
/************************
// Extern variable declaration
//
*************************/
extern void ScalerHdmiTxSetBypassLinkTrainingEn(UINT8 enable);
extern void ScalerTxHdmiTx0EmpHdOuiInit(void);

/************************
// Local Function declaration
//
*************************/


/************************
// Extern Function declaration
//
*************************/
#if defined(CONFIG_ENABLE_HDCP2_TX)
extern void ScalerHdmiHdcp2TxInit(void);
#endif
#if defined(CONFIG_ENABLE_HDCP14_TX)
extern void hdcp14tx_init();
#endif
#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
extern void ScalerHdmiMacTx0RepeaterEdidSupportConfig(void);
extern INT8 ScalerHdmiMacTx0RepeaterSetEdidSupport(EnumEdidFeatureType enumEdidFeature, UINT8 value);
#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
extern char ScalerHdmiTxRepeaterSetSwPktEn(EnumHdmiRepeaterSwPktType swPktType, UINT8 enable);
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT

#ifdef CONFIG_ENABLE_HDMITX_PHY
extern UINT8 ScalerHdmiTxPhy_CheckFfeSwingTableCodeCorrect(void);
#else
#define ScalerHdmiTxPhy_CheckFfeSwingTableCodeCorrect()   (0)
#endif
// check tx source & external module data structure size
extern UINT16 ScalerHdmiMacTx0GetDsSize_HDMI_DP_TIMING_T(void);
extern UINT16 ScalerHdmiMacTx0GetDsSize_StructDisplayInfo(void);
extern UINT16 ScalerHdmiMacTx0GetDsSize_StructHDMITxOutputTimingInfo(void);

// TX audio
extern void ScalerHdmiMacTx0AudioInputTypeInit(void);
/************************
// Function implementation
//
*************************/
void hdmitx_drv_sys_clock_reset(void)
{
#if 0//fix by zhaodong
#ifdef CONFIG_MACH_RTD2801_ZEBU
   FatalMessageHDMITx("[HDMITX][ZEBU] sys_clock_reset\n");

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

#else
   FatalMessageHDMITx("[HDMITX] sys_clock_reset\n");

    //rtd_outl(0xb8000120, 0x00700001);
    value_u32 = SYS_REG_SYS_SRST4_rstn_hdcp14_mask|SYS_REG_SYS_SRST4_rstn_hdmitx_mask|SYS_REG_SYS_SRST4_rstn_hdmitx_phy_mask|SYS_REG_SYS_SRST4_rstn_hdmitx_off_mask|SYS_REG_SYS_SRST4_write_data_mask;
    rtd_outl(SYS_REG_SYS_SRST4_reg, value_u32);
    delayus(5);

    // rtd_outl(0xb8000130, 0x0040007d);
    value_u32 = (SYS_REG_SYS_CLKEN4_clken_hdcp14_mask|SYS_REG_SYS_CLKEN4_clken_hdmitx_mask|SYS_REG_SYS_CLKEN4_write_data_mask);
    rtd_outl(SYS_REG_SYS_CLKEN4_reg, value_u32);
    delayus(15);

#if 0 // move to fw_scaler_set_main_gate_sel()
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_2p_gate_sel(1));
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_hdmitx_420_2p_gate_sel(1));
    rtd_maskl(SYS_REG_SYS_DCLK_GATE_SEL1_reg, ~SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel_mask, SYS_REG_SYS_DCLK_GATE_SEL1_vgip_2p_gate_sel(1));
#endif
#endif
#endif
    return;
}


UINT8 hdmitx_check_txSrc_extModule_ds_size(void)
{

    UINT8 notMatchFlag=0;
#if 0//fix by zhaodong
    // HDMI_DP_TIMING_T
    value_u32 = ScalerHdmiMacTx0GetDsSize_HDMI_DP_TIMING_T();
    if(value_u32 != sizeof(HDMI_DP_TIMING_T)){
        FatalMessageHDMITx("[HDMITX] HDMI_DP_TIMING_T SIZE NOT MATCH!!\n");
        FatalMessageHDMITx("[HDMITX] HDMI_DP_TIMING_T=%x/", (UINT32)value_u32);
        FatalMessageHDMITx("%x\n", (UINT32)sizeof(HDMI_DP_TIMING_T));
        notMatchFlag = 1;
    }

    // StructDisplayInfo
    value_u32 = ScalerHdmiMacTx0GetDsSize_StructDisplayInfo();
    if(value_u32 != sizeof(StructHdmitxDisplayInfo)){
        FatalMessageHDMITx("[HDMITX] StructDisplayInfo SIZE NOT MATCH!!\n");
        FatalMessageHDMITx("[HDMITX] StructDisplayInfo=%x/", (UINT32)value_u32);
        FatalMessageHDMITx("%x\n", (UINT32)sizeof(StructHdmitxDisplayInfo));
        notMatchFlag = 1;
    }

    // StructHDMITxOutputTimingInfo/AT_HDMI_TX_TIMING_INFO_T
    value_u32 = ScalerHdmiMacTx0GetDsSize_StructHDMITxOutputTimingInfo();
    if(value_u32 != sizeof(AT_HDMI_TX_TIMING_INFO_T)){
        FatalMessageHDMITx("[HDMITX] AT_HDMI_TX_TIMING_INFO_T SIZE NOT MATCH!!\n");
        FatalMessageHDMITx("[HDMITX] AT_HDMI_TX_TIMING_INFO_T=%x/", (UINT32)value_u32);
        FatalMessageHDMITx("%x\n", (UINT32)sizeof(AT_HDMI_TX_TIMING_INFO_T));
        notMatchFlag = 1;
    }
#endif
    return notMatchFlag;
}


INT32 hdmitx_deinit(void)
{
    InfoMessageHDMITx("[HDMITX] hdmitx_deinit@State[%d]\n", (UINT32)ScalerHdmiTx_Get_Timing_Status());

    if(ScalerHdmiTx_Get_Timing_Status() != TX_TIMING_INIT_STATE){
		//fix by zhaodong
        //core_timer_event_addTimerEvent(1, MSG_HDMITX_ID, TX_EVENT_DEINIT_EVENT, (UINT32)hdmitx_deinit);
        //core_fw_msg_queue_addEvent(MSG_HDMITX_ID, TX_EVENT_DEINIT_EVENT, (UINT32)hdmitx_deinit);
    }
    set_hdmitxtiminginfo_addr(NULL);
    set_hdmitxOutputMode(TX_OUT_MODE_NUM);
    //InfoMessageHDMITx("[HDMITX] hdmitx_deinit Done!\n");
    return 0;
}

INT32 hdmitx_init(void)
{
    FatalMessageHDMITx("[HDMITX] Handler start...@hdmitx_init\n");
    // check TX source & external module DS size
    if(hdmitx_check_txSrc_extModule_ds_size() != 0){
        FatalMessageHDMITx("\n\n\n[HDMITX] FORCE BREAK DUE TO TX SOURCE DS NOT MATCH!!\n\n\n");
        return -1;
    }

#ifdef CONFIG_HDMITX_TX_SCRIPT_SUPPORT // show HDMITX test info
    HDMITX_DV_Script_test_info();
#endif

    // check GPIO Pinmux
    #if 0 //fix by zhaodong
    value_u32 = 0;
    if((Get_PIN_ST_GPIO06_Mode() == _PIN_ST_GPIO06_HDMI_TX_SCL) && (Get_PIN_ST_GPIO07_Mode() == _PIN_ST_GPIO07_HDMI_TX_SDA))
        value_u32 |= _BIT0;
     if((Get_PIN_GPIO27_Mode() == _PIN_GPIO27_HDMI_TX_SCL) && (Get_PIN_GPIO28_Mode() == _PIN_GPIO28_HDMI_TX_SDA))
        value_u32 |= _BIT1;
    FatalMessageHDMITx("\n*** [HDMITX] PINMUX CHECK Match=%x ***\n", (UINT32)value_u32);
    if(value_u32 == 0){
        FatalMessageHDMITx("GPIO06=%x/%x,", (UINT32)Get_PIN_ST_GPIO06_Mode(), (UINT32)_PIN_ST_GPIO06_HDMI_TX_SCL);
        FatalMessageHDMITx("GPIO07=%x/%x\n", (UINT32)Get_PIN_ST_GPIO07_Mode(), (UINT32)_PIN_ST_GPIO07_HDMI_TX_SDA);
        FatalMessageHDMITx("GPIO27=%x/%x,", (UINT32)Get_PIN_GPIO27_Mode(), (UINT32)_PIN_GPIO27_HDMI_TX_SCL);
        FatalMessageHDMITx("GPIO28=%x/%x\n", (UINT32)Get_PIN_GPIO28_Mode(), (UINT32)_PIN_GPIO28_HDMI_TX_SDA);
    }

#if !defined(CONFIG_MACH_RTD2801_ZEBU)
    // Get HDMITX I2C BUSID
    //pcb_mgr_get_enum_info_byname("HDMI_TX_I2C", &HDMI_TX_I2C_BUSID);
#endif // #if !defined(CONFIG_MACH_RTD2801_ZEBU)
    HDMI_TX_I2C_BUSID = 1;
    InfoMessageHDMITx("[HDMITX] HDMI_TX_I2C_BUSID=%x\n", (UINT32)HDMI_TX_I2C_BUSID);
    if (dma_buf_request(DMA_HDMITX_EDID, HDMITX_INNER_EDID_SIZE, &dma_hdmitx_edid_phys_addr, &sys_share_dmem_virt_addr) != HDMITX_INNER_EDID_SIZE) {
        FatalMessageHDMITx("\n\n[HDMITX] *** dma_buf_request FAIL!!@hdmitx_init ***\n\n\n");
        //return 1;
    }
#endif
    // check FFE/SWING table size
    if(ScalerHdmiTxPhy_CheckFfeSwingTableCodeCorrect() != 1){
        FatalMessageHDMITx("\n\n[HDMITX] *** CHECK FFE/SWING TABLE FAIL!!@hdmitx_init ***\n\n\n");
        return 0;
    }

#if defined(CONFIG_MACH_RTD2801_ZEBU)
    // force bypass HPD/EDID/LinkTraining control flow
    ScalerHdmiTxSetBypassLinkTrainingEn(1);
#endif // #if defined(CONFIG_MACH_RTD2801_ZEBU)

    // HDMITX state init
    setHDMITX_Timing_Ready(TX_TIMING_INIT_STATE);

#ifdef CONFIG_HDMITX_REPEATER_SUPPORT
    // HDMI repeater support EDID feature config
    ScalerHdmiMacTx0RepeaterEdidSupportConfig();

#endif // #ifdef CONFIG_HDMITX_REPEATER_SUPPORT

#ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    // TX enable send infoframe packet
    ScalerHdmiTxRepeaterSetSwPktEn((UINT32)TX_SW_PKT_EMP_VTEM_VRR|TX_SW_PKT_DRM_HDR | TX_SW_PKT_VSIF_DV_VSIF| TX_SW_PKT_VSIF_HDMI_ALLM|TX_SW_PKT_AVI_INFO, _ENABLE);
#endif // #ifdef CONFIG_HDMITX_SW_BYPASS_INFO_PACKET_SUPPORT
    lib_hdmitx_packet_config_init();

    // TX HW reset
    hdmitx_drv_sys_clock_reset();

    //[ECN] TX EMP header OUI init
    ScalerTxHdmiTx0EmpHdOuiInit();

    // TX audio inout type config
    ScalerHdmiMacTx0AudioInputTypeInit();

    // create hdmitx period timer event handler
    hdmitx_timer_event_init();
    set_hdmitxtiminginfo_addr(NULL);
    set_hdmitxOutputMode(TX_OUT_THROUGH_MODE);
#if defined(CONFIG_ENABLE_HDCP2_TX)
    ScalerHdmiHdcp2TxInit();
#endif
#if defined(CONFIG_ENABLE_HDCP14_TX)
    hdcp14tx_init();
#endif

    InfoMessageHDMITx("[HDMITX] hdmitx_init Done!\n");
    return 0;
}

