/*************************************************************************
    > File Name: rtk_mmc_pinmux.c
    > Author: bennychen
    > Mail: bennytschen@realtek.com
    > Created Time: Thu, 27 May 2021 16:40:38 +0800
 ************************************************************************/
#include <linux/module.h>
#include <rbus/sb2_reg.h>
#include <rbus/pinmux_reg.h>
#include <rbus/emmc_wrap_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rtk_kdriver/io.h>
#include "rtk_mmc.h"

#define cr_readl(offset)        rtd_inl(offset)
#define cr_writel(val, offset)  rtd_outl(offset, val)

/* xxxx_DesignSpec_Pinmux.doc; sync with romcode */
void set_emmc_pin_mux(void)
{
    unsigned int reginfo;

    // 0x180008e0[0]=0x1(emmc_sel)
    //  PAD_EMMC_D6 eMMC D6     0x180008e0[31:28] = 0x1
    //REG32(0x180008e0) = (REG32(0x180008e0) & 0x0fffffff) | 0x10000001;
    reginfo = cr_readl(0xb80008e0);
    cr_writel((reginfo & 0x0fffffff)|0x10000001,0xb80008e0);

    //  PAD_EMMC_RST_N eMMC RST     0x180008d4[31:28] = 0x1
    //  PAD_EMMC_CLK eMMC CLK       0x180008d4[23:20] = 0x1
    //  PAD_EMMC_CMD eMMC CMD       0x180008d4[15:12] = 0x1
    //REG32(0x180008d4) = (REG32(0x180008d4) & 0x0f0f0fff) | 0x10101000;
    reginfo = cr_readl(0xb80008d4);
    cr_writel((reginfo & 0x0f0f0fff)|0x10101000,0xb80008d4);

    //  PAD_EMMC_DS eMMC DS(V5.0)   0x180008d8[31:28] = 0x1
    //  PAD_EMMC_D5 eMMC D5     0x180008d8[23:20] = 0x1
    //  PAD_EMMC_D3 eMMC D3     0x180008d8[15:12] = 0x1
    //  PAD_EMMC_D4 eMMC D4     0x180008d8[7:4] = 0x1
    //REG32(0x180008d8) = (REG32(0x180008d8) & 0x0f0f0f0f) | 0x10101010;
    reginfo = cr_readl(0xb80008d8);
    cr_writel((reginfo & 0x0f0f0f0f)|0x10101010,0xb80008d8);

    //  PAD_EMMC_D0 eMMC D0     0x180008dc[31:28] = 0x1
    //  PAD_EMMC_D1 eMMC D1     0x180008dc[23:20] = 0x1
    //  PAD_EMMC_D2 eMMC D2     0x180008dc[15:12] = 0x1
    //  PAD_EMMC_D7 eMMC D7     0x180008dc[7:4] = 0x1
    //REG32(0x180008dc) = (REG32(0x180008dc) & 0x0f0f0f0f) | 0x10101010;
    reginfo = cr_readl(0xb80008dc);
    cr_writel((reginfo & 0x0f0f0f0f)|0x10101010,0xb80008dc);

}


