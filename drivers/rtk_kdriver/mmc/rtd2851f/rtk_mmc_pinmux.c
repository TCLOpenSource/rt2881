/*************************************************************************
	> File Name: rtk_mmc_pinmux.c
	> Author: bennychen
	> Mail: bennytschen@realtek.com
	> Created Time: Thu, 27 May 2021 16:40:38 +0800
 ************************************************************************/
#include <linux/module.h>
#include <rbus/sb2_reg.h>
#include <rbus/pinmux_reg.h>
#include <rbus/pinmux_main_reg.h>
#include <rbus/emmc_wrap_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rtk_kdriver/io.h>
#include "rtk_mmc.h"

#define cr_readl(offset)        rtd_inl(offset)
#define cr_writel(val, offset)  rtd_outl(offset, val)

/* xxxx_DesignSpec_Pinmux.doc */
void set_emmc_pin_mux(void)
{
    unsigned int reginfo;

    reginfo = cr_readl(EMMC_WRAP_emmc_33v_sel_reg);
    cr_writel(reginfo&0xfffffffe,EMMC_WRAP_emmc_33v_sel_reg);

    // 0x18000870[0]=0x1 mmc sel
    reginfo = cr_readl(PINMUX_MAIN_GPIO_EMMC_CFG_2_reg);
    cr_writel(reginfo|0x00000001,PINMUX_MAIN_GPIO_EMMC_CFG_2_reg);

    //  PAD_EMMC_RST_N      0x1800085C[31:28] = 0x1
    //  PAD_EMMC_CLK        0x1800085C[23:20] = 0x1
    //  PAD_EMMC_CMD        0x1800085C[15:12] = 0x1
    reginfo = cr_readl(PINMUX_MAIN_GPIO_EMMCCLK_CFG_0_reg) & 0x0f0f0fff;
    cr_writel(reginfo|0x10101000,PINMUX_MAIN_GPIO_EMMCCLK_CFG_0_reg);

    //  PAD_EMMC_D6         0x1800086C[31:28] = 0x1
    //  PAD_EMMC_DS         0x1800086C[23:20] = 0x1
    //  PAD_EMMC_D5         0x1800086C[15:12] = 0x1
    //  PAD_EMMC_D3         0x1800086C[ 7: 4] = 0x1
    reginfo = cr_readl(PINMUX_MAIN_GPIO_EMMC_CFG_1_reg) & 0x0f0f0f0f;
    cr_writel(reginfo|0x10101010,PINMUX_MAIN_GPIO_EMMC_CFG_1_reg);

    //  PAD_EMMC_D0         0x18000868[31:28] = 0x1
    //  PAD_EMMC_D1         0x18000868[23:20] = 0x1
    //  PAD_EMMC_D2         0x18000884[15:12] = 0x1
    //  PAD_EMMC_D7         0x18000884[ 7: 4] = 0x1
    reginfo = cr_readl(PINMUX_MAIN_GPIO_EMMC_CFG_0_reg) & 0x0f0f0f0f;
    cr_writel(reginfo|0x10101010,PINMUX_MAIN_GPIO_EMMC_CFG_0_reg);

	//  PAD_EMMC_D4         0x18000870[31:28] = 0x1
	reginfo = cr_readl(PINMUX_MAIN_GPIO_EMMC_CFG_2_reg) & 0x0fffffff;
    cr_writel(reginfo|0x10000000,PINMUX_MAIN_GPIO_EMMC_CFG_2_reg);
}

