/*************************************************************************
    > File Name: rtk_mmc_pinmux.c
    > Author: bennychen
    > Mail: bennytschen@realtek.com
    > Created Time: Thu, 27 May 2021 16:40:38 +0800
 ************************************************************************/
#include <linux/module.h>
#include <rbus/sb2_reg.h>
//#include <rbus/pinmux_reg.h>
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
#if 1
    unsigned int reginfo;

    // 0x18000840[0]=0x1(emmc_sel)
    reginfo = cr_readl(0xb8000840);
    cr_writel(reginfo|0x00000001,0xb8000840);

    // eMMC    RST     pad_emmc_rst_n_io       0x18000834[31:28]=1
    // eMMC    CLK     pad_emmc_clk_io         0x18000834[23:20]=1
    // eMMC    CMD     pad_emmc_cmd_io         0x18000834[15:12]=1
	reginfo = cr_readl(0xb8000834) & 0x0f0f0fff;
	cr_writel(reginfo|0x10101000,0xb8000834);

    // eMMC    DS      pad_emmc_ds_io          0x18000838[31:28]=1
    // eMMC    DATA5   pad_emmc_d5_io          0x18000838[23:20]=1
    // eMMC    DATA3   pad_emmc_d3_io          0x18000838[15:12]=1
    // eMMC    DATA4   pad_emmc_d4_io          0x18000838[7:4]=1
	reginfo = cr_readl(0xb8000838) & 0x0f0f0f0f;
	cr_writel(reginfo|0x10101010,0xb8000838);

    // eMMC    DATA0   pad_emmc_d0_io          0x1800083c[31:28]=1
    // eMMC    DATA1   pad_emmc_d1_io          0x1800083c[23:20]=1
    // eMMC    DATA2   pad_emmc_d2_io          0x1800083c[15:12]=1
    // eMMC    DATA7   pad_emmc_d7_io          0x1800083c[7:4]=1
	reginfo = cr_readl(0xb800083c) & 0x0f0f0f0f;
	cr_writel(reginfo|0x10101010,0xb800083c);

    // eMMC    DATA6   pad_emmc_d6_io          0x18000840[31:28]=1
	reginfo = cr_readl(0xb8000840) & 0x0fffffff;
	cr_writel(reginfo|0x10000000,0xb8000840);
#else
    unsigned int reginfo;

    reginfo = cr_readl(EMMC_WRAP_emmc_33v_sel_reg);
    cr_writel(reginfo&0xfffffffe,EMMC_WRAP_emmc_33v_sel_reg);

    // 0x18000874[0]=0x1(emmc_sel)
    reginfo = cr_readl(0xb8000874);
    cr_writel(reginfo|0x00000001,0xb8000874);

    //  PAD_EMMC_RST_N eMMC RST     0x18000860[31:28] = 0x1
    //  PAD_EMMC_CLK eMMC CLK       0x18000860[23:20] = 0x1
    //  PAD_EMMC_CMD eMMC CMD       0x18000860[15:12] = 0x1
    reginfo = cr_readl(0xb8000860);
    cr_writel((reginfo & 0x0f0f0fff)|0x10101000,0xb8000860);

    //  PAD_EMMC_DS eMMC DS(V5.0) 0x1800086c[31:28] = 0x1
    //  PAD_EMMC_D5 eMMC D5 0x1800086c[23:20] = 0x1
    //  PAD_EMMC_D3 eMMC D3 0x1800086c[15:12] = 0x1
    //  PAD_EMMC_D4 eMMC D4 0x1800086c[7:4] = 0x1
    reginfo = cr_readl(0xb800086c);
    cr_writel((reginfo & 0x0f0f0f0f)|0x10101010,0xb800086c);

    //  PAD_EMMC_D0 eMMC D0 0x18000870[31:28] = 0x1
    //  PAD_EMMC_D1 eMMC D1 0x18000870[23:20] = 0x1
    //  PAD_EMMC_D2 eMMC D2 0x18000870[15:12] = 0x1
    //  PAD_EMMC_D7 eMMC D7 0x18000870[7:4] = 0x1
    reginfo = cr_readl(0xb8000870);
    cr_writel((reginfo & 0x0f0f0f0f)|0x10101010,0xb8000870);

    //  PAD_EMMC_D6 eMMC D6 0x18000874[31:28] = 0x1
    reginfo = cr_readl(0xb8000874);
    cr_writel((reginfo & 0x0fffffff)|0x10000000,0xb8000874);
#endif
}


