#include <linux/init.h>
#include <io.h>
#include <mach/platform.h>
#include <mach/rtk_platform.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <rbus/sfg_reg.h>
#include <rbus/mipi_dphy_reg.h>
#include <rbus/mipi_dphy1_reg.h>
#include <rbus/mipi_dsi_reg.h>

#include <tvscalercontrol/mipitx/mipitx.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <ioctrl/scaler/vbe_cmd_id.h>

#define TAG_NAME "MIPITX"
VBE_DISP_PANEL_MIPI_SUSPEND_RESUME_T  vbe_disp_panel_mipi_suspend_resume;

MIPITX_PANEL_INDEX mipitx_panel = MIPITX_PANEL_MAX;

void mipitx_sys_clock_reset(void)
{
	sfg_sfg_clken_ctrl_RBUS sfg_sfg_clken_ctrl_reg;
	sfg_sfg_clken_ctrl_reg.regValue = IoReg_Read32(SFG_SFG_CLKEN_CTRL_reg);
	sfg_sfg_clken_ctrl_reg.clken_sfg = 1;
	sfg_sfg_clken_ctrl_reg.rbus_clken_mipi = 1;
	IoReg_Write32(SFG_SFG_CLKEN_CTRL_reg, sfg_sfg_clken_ctrl_reg.regValue);
    rtd_printk(KERN_EMERG, TAG_NAME,"[MIPITX] %s Done\n", __FUNCTION__);

    return;
}


void mipitx_sys_clock_disable(void)
{
	sfg_sfg_clken_ctrl_RBUS sfg_sfg_clken_ctrl_reg;
	sfg_sfg_clken_ctrl_reg.regValue = IoReg_Read32(SFG_SFG_CLKEN_CTRL_reg);
	sfg_sfg_clken_ctrl_reg.clken_sfg = 0;
	sfg_sfg_clken_ctrl_reg.rbus_clken_mipi = 0;
	IoReg_Write32(SFG_SFG_CLKEN_CTRL_reg, sfg_sfg_clken_ctrl_reg.regValue);
    rtd_printk(KERN_EMERG, TAG_NAME, "[MIPITX] %s Done\n", __FUNCTION__);

    return;
}


int mipitx_state_handler(void)
{
    if(mipitx_panel == MIPITX_HD_720_1440_PANEL) {
        mipitx_HD_720_1440_panel();
    } else if(mipitx_panel == MIPITX_TM697_DSC_PANEL) {
        mipitx_TM697_DSC_panel();
    }
    return 0;
	
}

void vbe_disp_mipi_panel_suspend(void)
{
    /**********MIPI DPHY0*********/
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_CLOCK_GEN_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_CLOCK_GEN_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_CLK_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_CLK_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_2_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_3_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_3_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_4_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_4_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_5_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_5_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_6_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_6_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_RX_DATA_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_RX_DATA_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LANE_SWAP_CTRL_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_LANE_SWAP_CTRL_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LANE_LP_BIST_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_LANE_LP_BIST_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LANE_HS_BIST_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_LANE_HS_BIST_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_contention_detector_and_stopstate_dt_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_contention_detector_and_stopstate_dt_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_DEBUG_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_DEBUG_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_DEBUG_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_DEBUG_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_SSC1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_SSC1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_SSC2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_SSC2_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_SSC3_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_SSC3_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_watchdog_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_watchdog_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_Deglitch_lp_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_Deglitch_lp_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LP_BIST_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_LP_BIST_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LP_BIST_2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_LP_BIST_2_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_2_reg);

    /**********MIPI DPHY1*********/
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_CLOCK_GEN_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_CLOCK_GEN_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_CLK_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_CLK_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_2_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_3_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_3_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_4_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_4_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_5_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_5_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_6_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_6_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_RX_DATA_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_RX_DATA_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LANE_SWAP_CTRL_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_LANE_SWAP_CTRL_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LANE_LP_BIST_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_LANE_LP_BIST_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LANE_HS_BIST_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_LANE_HS_BIST_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_contention_detector_and_stopstate_dt_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_contention_detector_and_stopstate_dt_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_DEBUG_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_DEBUG_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_DEBUG_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_DEBUG_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_SSC1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_SSC1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_SSC2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_SSC2_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_SSC3_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_SSC3_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_watchdog_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_watchdog_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_0_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_Deglitch_lp_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_Deglitch_lp_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LP_BIST_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_LP_BIST_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LP_BIST_2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_LP_BIST_2_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_1_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_2_S_R = IoReg_Read32(MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_2_reg);

    /**********MIPI DSI*********/
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_0_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_1_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_1_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_2_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_2_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_3_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_3_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_4_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_4_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_5_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_5_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_7_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_7_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_8_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_8_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_9_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_9_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_10_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_10_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_11_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_11_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_12_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_12_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_13_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_13_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_14_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_14_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_15_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_15_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_16_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_16_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_17_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_17_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_18_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_18_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_19_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_19_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_20_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_20_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_21_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_21_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_22_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_22_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_23_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_23_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_24_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_24_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_25_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_25_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_26_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_26_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_27_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_27_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_28_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_28_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_29_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_29_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_30_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_30_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_31_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_31_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_32_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_32_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_33_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_33_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_34_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_34_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_35_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_35_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_36_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_36_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_37_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_37_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_38_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_38_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_39_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_39_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_40_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_40_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_41_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_41_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_42_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_42_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_43_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_43_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_44_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_44_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_45_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_45_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_46_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_46_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_47_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_47_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_48_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_48_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_49_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_49_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_50_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_50_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_51_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_51_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_52_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_52_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_53_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_53_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_54_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_54_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_55_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_55_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_56_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_56_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_57_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_57_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_58_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_58_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_59_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_59_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_60_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_60_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_61_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_61_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_62_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_62_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_63_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_63_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_64_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_68_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_68_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_69_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_69_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_70_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_70_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_71_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_71_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_72_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_72_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_73_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_73_reg);
    vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_74_S_R = IoReg_Read32(MIPI_DSI_MIPI_CTRL_74_reg);
}

void vbe_disp_mipi_panel_resume(void)
{
    /**********MIPI DPHY0*********/
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_CLOCK_GEN_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_CLOCK_GEN_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_CLK_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_CLK_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_2_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_3_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_3_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_4_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_4_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_5_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_5_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_6_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_TX_DATA_6_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_ESCAPE_RX_DATA_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_ESCAPE_RX_DATA_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_LANE_SWAP_CTRL_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LANE_SWAP_CTRL_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_LANE_LP_BIST_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LANE_LP_BIST_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_LANE_HS_BIST_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LANE_HS_BIST_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_contention_detector_and_stopstate_dt_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_contention_detector_and_stopstate_dt_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_DEBUG_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_DEBUG_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_DEBUG_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_DEBUG_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_SSC1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_SSC1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_SSC2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_SSC2_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_SSC3_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_SSC3_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_watchdog_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_watchdog_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_RX_ESCAPE_READ_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_Deglitch_lp_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_Deglitch_lp_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_LP_BIST_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LP_BIST_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_LP_BIST_2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_LP_BIST_2_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_2_S_R);

    /**********MIPI DPHY1*********/
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_CLOCK_GEN_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_CLOCK_GEN_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_CLK_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_CLK_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_2_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_3_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_3_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_4_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_4_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_5_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_5_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_6_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_TX_DATA_6_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_ESCAPE_RX_DATA_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_ESCAPE_RX_DATA_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_LANE_SWAP_CTRL_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LANE_SWAP_CTRL_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_LANE_LP_BIST_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LANE_LP_BIST_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_LANE_HS_BIST_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LANE_HS_BIST_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_contention_detector_and_stopstate_dt_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_contention_detector_and_stopstate_dt_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_DEBUG_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_DEBUG_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_DEBUG_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_DEBUG_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_SSC1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_SSC1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_SSC2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_SSC2_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_SSC3_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_SSC3_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_watchdog_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_watchdog_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_0_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_RX_ESCAPE_READ_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_Deglitch_lp_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_Deglitch_lp_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_LP_BIST_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LP_BIST_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_LP_BIST_2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_LP_BIST_2_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_1_S_R);
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DPHY_MIPI_DPHY1_lane_pn_swap_2_S_R);

    /**********MIPI DSI*********/
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_0_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_1_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_1_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_2_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_2_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_3_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_3_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_4_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_4_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_5_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_5_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_7_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_7_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_8_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_8_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_9_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_9_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_10_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_10_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_11_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_11_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_12_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_12_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_13_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_13_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_14_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_14_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_15_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_15_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_16_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_16_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_17_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_17_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_18_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_18_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_19_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_19_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_20_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_20_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_21_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_21_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_22_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_22_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_23_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_23_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_24_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_24_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_25_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_25_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_26_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_26_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_27_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_27_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_28_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_28_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_29_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_29_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_30_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_30_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_31_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_31_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_32_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_32_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_33_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_33_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_34_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_34_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_35_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_35_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_36_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_36_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_37_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_37_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_38_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_38_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_39_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_39_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_40_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_40_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_41_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_41_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_42_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_42_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_43_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_43_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_44_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_44_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_45_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_45_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_46_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_46_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_47_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_47_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_48_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_48_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_49_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_49_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_50_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_50_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_51_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_51_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_52_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_52_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_53_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_53_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_54_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_54_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_55_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_55_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_56_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_56_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_57_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_57_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_58_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_58_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_59_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_59_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_60_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_60_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_61_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_61_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_62_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_62_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_63_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_63_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_64_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_64_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_68_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_68_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_69_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_69_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_70_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_70_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_71_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_71_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_72_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_72_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_73_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_73_S_R);
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_74_reg, vbe_disp_panel_mipi_suspend_resume.MIPI_DSI_MIPI_CTRL_74_S_R);
}