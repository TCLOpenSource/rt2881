#include <rbus/mipi_dphy_reg.h>
#include <tvscalercontrol/mipitx/mipitx.h>
#include <tvscalercontrol/mipitx/mipitx_dphy.h>
#include <tvscalercontrol/io/ioregdrv.h>

void mipitx_dphy_init(void)
{
    mipi_dphy_mipi_dphy0_clock_gen_RBUS mipi_dphy_mipi_dphy0_clock_gen_reg;
    mipi_dphy_mipi_dphy0_lane_pn_swap_1_RBUS mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg;
    mipi_dphy_mipi_dphy0_lane_pn_swap_2_RBUS mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg;
   
    mipi_dphy_mipi_dphy0_clock_gen_reg.regValue = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_CLOCK_GEN_reg);
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_lane0_en =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_lane1_en =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_lane2_en =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_lane3_en =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_clk_lane_en =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_rx_rst_n =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_tx_rst_n =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_clk_deglitch_pll_en =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_pll_lane0_enable =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_pll_lane1_enable =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_pll_lane2_enable =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_pll_lane3_enable =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_pll_ck_lane_enable =1;
    mipi_dphy_mipi_dphy0_clock_gen_reg.reg_mtx_en =1;
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_CLOCK_GEN_reg, mipi_dphy_mipi_dphy0_clock_gen_reg.regValue);

    mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg.regValue = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_1_reg);
    mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg.reg_tx_lane0_sel = 3;
    mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg.reg_tx_lane1_sel = 0;
    mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg.reg_tx_lane2_sel = 1;
    mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg.reg_tx_lane3_sel = 2;
    mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg.reg_tx_lane4_sel = 4;
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_1_reg, mipi_dphy_mipi_dphy0_lane_pn_swap_1_reg.regValue);

    mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg.regValue = IoReg_Read32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_2_reg);
    mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg.reg_rx_lane0_sel = 3;
    mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg.reg_rx_lane1_sel = 0;
    mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg.reg_rx_lane2_sel = 1;
    mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg.reg_rx_lane3_sel = 2;
    mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg.reg_rx_lane4_sel = 4;
    IoReg_Write32(MIPI_DPHY_MIPI_DPHY0_lane_pn_swap_2_reg, mipi_dphy_mipi_dphy0_lane_pn_swap_2_reg.regValue);
}