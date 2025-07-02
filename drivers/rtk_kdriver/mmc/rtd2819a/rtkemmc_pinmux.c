/*************************************************************************
    > File Name: rtk_mmc_pinmux.c
    > Author: bennychen
    > Mail: bennytschen@realtek.com
    > Created Time: Thu, 27 May 2021 16:40:38 +0800
 ************************************************************************/

#include <linux/module.h>
#include <rbus/sb2_reg.h>
#include <rbus/pinmux_reg.h>
//#include <rbus/pinmux_main_reg.h>
#include <rbus/emmc_wrap_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <rtk_kdriver/io.h>
#include "rtk_mmc.h"
#include "rtkemmc_dbg.h"
#include "rtksdio.h"

/* xxxx_DesignSpec_Pinmux.doc */
void set_emmc_pin_mux(void)
{

    unsigned int reginfo;

    /* alexkh: this is mac9q pinmux*/
    // 0x18000814[0]=0x1(emmc_sel)
    //REG32(0x18000814) |= 0x00000001;
    reginfo = em_inl(0xb8000814);
    em_outl(0xb8000814,reginfo|0x00000001);


    //  PAD_EMMC_RST_N eMMC RST     0x18000800[15:12] = 0x1
    //  PAD_EMMC_CLK eMMC CLK       0x18000800[31:28] = 0x1
    //  PAD_EMMC_CMD eMMC CMD       0x18000800[23:20] = 0x1
    //REG32(0x18000800) = (REG32(0x18000800) & 0x0f0f0fff) | 0x10101000;
    reginfo = em_inl(0xb8000800);
    em_outl(0xb8000800,(reginfo & 0x0f0f0fff) | 0x10101000);

    //  PAD_EMMC_DS eMMC DS(V5.0)   0x1800080c[31:28] = 0x1
    //  PAD_EMMC_D5 eMMC D5     0x1800080c[23:20] = 0x1
    //  PAD_EMMC_D3 eMMC D3     0x1800080c[15:12] = 0x1
    //  PAD_EMMC_D4 eMMC D4     0x1800080c[7:4] = 0x1
    //REG32(0x1800080c) = (REG32(0x1800080c) & 0x0f0f0f0f) | 0x10101010;
    reginfo = em_inl(0xb800080c);
    em_outl(0xb800080c,(reginfo & 0x0f0f0f0f) | 0x10101010);

    //  PAD_EMMC_D0 eMMC D0     0x18000810[31:28] = 0x1
    //  PAD_EMMC_D1 eMMC D1     0x18000810[23:20] = 0x1
    //  PAD_EMMC_D2 eMMC D2     0x18000810[15:12] = 0x1
    //  PAD_EMMC_D7 eMMC D7     0x18000810[7:4] = 0x1
    //REG32(0x18000810) = (REG32(0x18000810) & 0x0f0f0f0f) | 0x10101010;
    reginfo = em_inl(0xb8000810);
    em_outl(0xb8000810,(reginfo & 0x0f0f0f0f) | 0x10101010);

    //  PAD_EMMC_D6 eMMC D6     0x18000814[31:28] = 0x1
    //REG32(0x18000814) = (REG32(0x18000814) & 0x0fffffff) | 0x10000000;
    reginfo = em_inl(0xb8000814);
    em_outl(0xb8000814,(reginfo & 0x0fffffff) | 0x10000000);

}


