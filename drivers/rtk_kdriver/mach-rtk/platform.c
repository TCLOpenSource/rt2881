/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2010 by Chien-An Lin <colin@realtek.com.tw>
 *
 * Venus series board platform device registration
 */

#ifndef BUILD_QUICK_SHOW
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <asm/cputype.h>
#include <linux/irqchip/arm-gic-v3.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
#include <linux/init_syscalls.h>
#else
#include <linux/syscalls.h>
#endif
#ifndef CONFIG_ARM64
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#else
#include <linux/of_reserved_mem.h>
#endif
#include <mach/common.h>
#include <mach/timex.h>
#ifdef CONFIG_HIBERNATION
#include <linux/suspend.h>
#endif
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include <mach/iomap.h>
#include <linux/mm.h>
#else  /* !BUILD_QUICK_SHOW */
#include <no_os/export.h>
#include <string.h>
#include <vsprintf.h>
#endif /* !BUILD_QUICK_SHOW */

#include <mach/platform.h>
#include <mach/rtk_platform.h>

#include <rbus/sys_reg_reg.h>
#include <rbus/stb_reg.h>
#include <rbus/stb_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll_reg_reg.h>

#if defined(CONFIG_ARCH_RTK2885M) || defined(CONFIG_ARCH_RTK2851A)
#include "rbus/dc1_mc_reg.h"
#else
#include "rbus/dc_mc_reg.h"
#endif
#include "rbus/dc_sys_reg.h"
#include <rtk_kdriver/rtk_qos_export.h>
#include <rtk_kdriver/panelConfigParameter.h>
#include <rtd_log/rtd_module_log.h>

int rtk_init_machine(void)
{

#if 0 //move to kdriver dvr_common_module_init
	/* Even we use arch_timer, but still need init misc timer2 timer for schedule log used.*/
	rtk_timer_init();
#endif

	return 0;
}
EXPORT_SYMBOL(rtk_init_machine);

void rtk_init_late(void)
{
   // add console driver node
#ifdef CONFIG_RTK_KDRV_COMMON
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
	//no need to mk console node.
	// and  __init init_mknod() is cant reference by not __init section API.
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
	init_mknod("/dev/console", S_IFCHR + 0600, new_encode_dev(MKDEV(5, 1)));
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0))
	ksys_mknod("/dev/console", S_IFCHR + 0600, new_encode_dev(MKDEV(5, 1)));
#else
	sys_mknod("/dev/console", S_IFCHR + 0600, new_encode_dev(MKDEV(5, 1)));
#endif
#endif

}


void rtk_init_early(void)
{
#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)
   rtd_pr_mach_rtk_err("pcbMgr: RTK\n");
// rtd_pr_mach_rtk_err("prom_init(), _prom_envp=%x\n", (unsigned int)_prom_envp);
   // must be called after parse_early_param that will assign _prom_envp from boot command line
   prom_init();
#endif
}


/*[MAC8PBU-332] need API for distinguish IC type*/
#define ARCH_RTK6748_ID_CODE	0x67480000
#define ARCH_RTK6702_ID_CODE	0x67020000
#if defined(CONFIG_ARCH_RTK2851C)
#define ARCH_RTK2851C_ID_CODE	0x68660000
#endif
#if defined(CONFIG_ARCH_RTK2851F)
#define ARCH_RTK2851F_ID_CODE	0x68660000
#endif
#define ARCH_RTK2851A_ID_CODE	0x66410000
#define ARCH_RTK2885P_ID_CODE	0x68560000
#define ARCH_RTK2875Q_ID_CODE	0x69820000
#define ARCH_RTK2819A_ID_CODE	0x70020000
#define ARCH_RTK2885PP_ID_CORE  0x70320000
/*read from otp will result in module cycle, hardcode read otp ic type*/
#define OTP_REGION_REMARK_EN_reg        0xb80380AC
#define OTP_REGION_FUNC_CONFIG_REM_reg  0xb80380A4
#define OTP_REGION_FUNC_CONFIG_ORI_reg  0xb8038098

static int read_otp_ic_type(void)
{
    int ret = 0, remark=0;
    remark = (rtd_inl(OTP_REGION_REMARK_EN_reg)>>15)&0x01;
    if(remark == 1){
        ret = (rtd_inl(OTP_REGION_FUNC_CONFIG_REM_reg)>>5)&0x01;
    }
    else
        ret = (rtd_inl(OTP_REGION_FUNC_CONFIG_ORI_reg))&0x01;

    return ret;
}

static enum MACH_TYPE mach_type = MACH_ARCH_OTHER;
enum MACH_TYPE get_mach_type(void)
{
	if(mach_type != MACH_ARCH_OTHER)
		return mach_type;
	else
	{
		unsigned int reg_value = rtd_inl(STB_SC_VerID_reg) & 0xFFFF0000;
		if(reg_value == ARCH_RTK6748_ID_CODE)
			mach_type = MACH_ARCH_RTK6748;
		else if(reg_value == ARCH_RTK6702_ID_CODE)
			mach_type = MACH_ARCH_RTK6702;
#if defined(CONFIG_ARCH_RTK2851C)
		else if(reg_value == ARCH_RTK2851C_ID_CODE)
			mach_type = MACH_ARCH_RTK2851C;
#endif
#if defined(CONFIG_ARCH_RTK2851F)
		else if(reg_value == ARCH_RTK2851F_ID_CODE)
			mach_type = MACH_ARCH_RTK2851F;
#endif
		else if(reg_value == ARCH_RTK2851A_ID_CODE)
			mach_type = MACH_ARCH_RTK2851A;
		else if(reg_value == ARCH_RTK2885P_ID_CODE)
			mach_type = MACH_ARCH_RTK2885P;
		else if(reg_value == ARCH_RTK2875Q_ID_CODE)
			mach_type = MACH_ARCH_RTK2875Q;
		else if(reg_value == ARCH_RTK2819A_ID_CODE)
			mach_type = MACH_ARCH_RTK2819A;
		else if(reg_value == ARCH_RTK2885PP_ID_CORE){
			if(read_otp_ic_type() == 1)
				mach_type = MACH_ARCH_RTK2885P2;
			else
				mach_type = MACH_ARCH_RTK2885PP;
        }
		else
			panic("Error: Machine type mismatch(%x)\n", reg_value);

		return mach_type;
	}
}
EXPORT_SYMBOL(get_mach_type);

static enum PLAFTORM_TYPE platform=PLATFORM_OTHER;
enum PLAFTORM_TYPE get_platform (void)
{
    return platform;
}
EXPORT_SYMBOL(get_platform);

static enum PANEL_RES_TYPE panel_resolution = PANEL_RES_MAX;
enum PANEL_RES_TYPE get_panel_res(void)
{
    if( panel_resolution == PANEL_RES_MAX )
    {
#if IS_ENABLED(CONFIG_REALTEK_PCBMGR)
        if( platform_info.panel_parameter_len > 0 )
        {
            PANEL_CONFIG_PARAMETER *panel_parameter = (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;
            unsigned int disp_win = panel_parameter->iCONFIG_DISP_ACT_END_HPOS - panel_parameter->iCONFIG_DISP_ACT_STA_HPOS;
            unsigned int disp_len = panel_parameter->iCONFIG_DISP_ACT_END_VPOS - panel_parameter->iCONFIG_DISP_ACT_STA_VPOS;

            rtd_pr_mach_rtk_info("*** panel parameter disp_win(%d), disp_len(%d) ***\n", disp_win, disp_len);

            if( ( disp_win == 7680 ) && ( disp_len == 4320 ) )
            {
                panel_resolution = PANEL_RES_8K;
            }
            else if( ( ( disp_win == 3840 ) && ( disp_len == 2160 ) ) || ( ( disp_win == 2560 ) && ( disp_len == 1440 ) ) )
            {
                panel_resolution = PANEL_RES_UHD;
            }
            else if( ( disp_win == 3440 ) && ( disp_len == 1440 ) )
            {
                panel_resolution = PANEL_RES_WQHD;
            }
            else if( ( disp_win == 3840 ) && ( disp_len == 1080 ) )
            {
                panel_resolution = PANEL_RES_4K1K;
            }
            else if( ( ( disp_win == 5120 ) && ( disp_len == 1440 ) ) || ( ( disp_win == 5120 ) && ( disp_len == 1080 ) ) )
            {
                panel_resolution = PANEL_RES_5K1K;
            }
            else if( ( ( disp_win == 5120 ) && ( disp_len == 2880 ) ) || ( ( disp_win == 5120 ) && ( disp_len == 2160 ) ) )
            {
                panel_resolution = PANEL_RES_5K2K;
            }
            else if( ( disp_win == 1920 ) && ( disp_len == 1080 ) )
            {
                panel_resolution = PANEL_RES_FHD;
            }
            else if( ( disp_win == 1366 ) && ( disp_len == 768 ) )
            {
                panel_resolution = PANEL_RES_HD;
            }
            else if( ( disp_win == 2160 ) && ( disp_len == 3840 ) )
            {
                panel_resolution = PANEL_RES_2K4K;
            }
            else if( ( disp_win == 720 ) && ( disp_len == 1440 ) )
            {
                panel_resolution = PANEL_RES_720p1440p;
            }
            else if( ( disp_win == 1080 ) && ( disp_len == 1920 ) )
            {
                panel_resolution = PANEL_RES_720p1440p;
            }
            else
            {
                rtd_pr_mach_rtk_err("*** Error panel parameter disp_win(%d), disp_len(%d) ***\n", disp_win, disp_len);
            }
        }
#endif
        return panel_resolution;
    }
    else
    {
        return panel_resolution;
    }
}
EXPORT_SYMBOL(get_panel_res);

static enum PRODUCT_TYPE product_type = PRODUCT_TYPE_DEFAULT;
enum PRODUCT_TYPE get_product_type(void)
{
    return product_type;
}
EXPORT_SYMBOL(get_product_type);

static enum PLATFORM_MODEL platform_model=PLATFORM_MODEL_DEFAULT;
enum PLATFORM_MODEL get_platform_model (void)
{
    return platform_model;
}
EXPORT_SYMBOL(get_platform_model);

static enum DISPLAY_RESOLUTION display_resolution = DISPLAY_RESOLUTION_DEFAULT;
enum DISPLAY_RESOLUTION get_display_resolution (void)
{
    return display_resolution;
}
EXPORT_SYMBOL(get_display_resolution);

static enum UI_RESOLUTION ui_resolution = UI_RESOLUTION_DEFAULT;
enum UI_RESOLUTION get_ui_resolution (void)
{
    return ui_resolution;
}
EXPORT_SYMBOL(get_ui_resolution);

static enum BOARD_VERSION board_version = BOARD_VERSION_DEFAULT;
enum BOARD_VERSION get_board_version (void)
{
    return board_version;
}
EXPORT_SYMBOL(get_board_version);

static enum MEMC_INIT memc_init = MEMC_INIT_DEFAULT;
enum MEMC_INIT is_memc_init(void)
{
    return memc_init;
}
EXPORT_SYMBOL(is_memc_init);

static int early_parse_platform (char *str)
{
    const char *str_plat = "RTD2875";

    if (strncmp(str_plat, str, strlen(str_plat))==0)
    {
        platform=PLATFORM_RTD2875;
        rtd_pr_mach_rtk_info("RTD2875 platform\n");
    }
    else
    {
        rtd_pr_mach_rtk_err("Wrong platform  ? \e[1;31m%s\e[0m\n",str);
    }

    return 0;
}

int early_parse_product_type (char *para)
{
	const char *string_DIAS = "DIAS";
	const char *string_DEMETER = "DEMETER";

	if (strncmp(string_DIAS, para, strlen(string_DIAS))==0)
	{
		product_type = PRODUCT_TYPE_DIAS;
		rtd_pr_mach_rtk_info("DIAS product\n");
	}
	else if (strncmp(string_DEMETER, para, strlen(string_DEMETER))==0)
	{
		product_type = PRODUCT_TYPE_DEMETER;
		rtd_pr_mach_rtk_info("DEMETER product\n");
	}
	else
	{
		rtd_pr_mach_rtk_err("Wrong product ? \e[1;31m%s\e[0m\n", para);
	}

	return 0;
}

int early_parse_platform_model (char *para)
{
	const char *string_2K = "2K";
	const char *string_4K = "4K";
	const char *string_8K = "8K";
	const char *string_5K = "5K";

	if (strncmp(string_4K, para, strlen(string_4K))==0)
	{
		platform_model = PLATFORM_MODEL_4K;
		rtd_pr_mach_rtk_info("4K model\n");
	}
	else if (strncmp(string_2K, para, strlen(string_2K))==0)
	{
		platform_model = PLATFORM_MODEL_2K;
		rtd_pr_mach_rtk_info("2K model\n");
	}
	else if (strncmp(string_8K, para, strlen(string_8K))==0)
	{
		platform_model = PLATFORM_MODEL_8K;
		rtd_pr_mach_rtk_info("8K model\n");
	}
	else if (strncmp(string_5K, para, strlen(string_5K))==0)
	{
		platform_model = PLATFORM_MODEL_5K;
		rtd_pr_mach_rtk_info("5K model\n");
	}
	else
	{
		rtd_pr_mach_rtk_err("Wrong model ? \e[1;31m%s\e[0m\n", para);
	}

	return 0;
}

static int early_parse_disp_res (char *para)
{
	const char *string_2K = "2K";
	const char *string_4K = "4K";
	const char *string_8K = "8K";

	if (strncmp(string_4K, para, strlen(string_4K))==0)
	{
		display_resolution = DISPLAY_RESOLUTION_4K;
		rtd_pr_mach_rtk_info("4K display\n");
	}
	else if (strncmp(string_2K, para, strlen(string_2K))==0)
	{
		display_resolution = DISPLAY_RESOLUTION_2K;
		rtd_pr_mach_rtk_info("2K display\n");
	}
	else if (strncmp(string_8K, para, strlen(string_8K))==0)
	{
		display_resolution = DISPLAY_RESOLUTION_8K;
		rtd_pr_mach_rtk_info("8K display\n");
	}
	else
	{
		rtd_pr_mach_rtk_err("Wrong display resolution ? \e[1;31m%s\e[0m\n", para);
	}

	return 0;
}

static int early_parse_ui_res (char *para)
{
	const char *string_720P = "720P";
	const char *string_2K = "2K";
	const char *string_4K = "4K";

	if (strncmp(string_4K, para, strlen(string_4K))==0)
	{
		ui_resolution = UI_RESOLUTION_4K;
		rtd_pr_mach_rtk_info("4K UI\n");
	}
	else if (strncmp(string_2K, para, strlen(string_2K))==0)
	{
		ui_resolution = UI_RESOLUTION_2K;
		rtd_pr_mach_rtk_info("2K UI\n");
	}
	else if (strncmp(string_720P, para, strlen(string_720P))==0)
	{
		ui_resolution = UI_RESOLUTION_720P;
		rtd_pr_mach_rtk_info("720p UI\n");
	}
	else
	{
		rtd_pr_mach_rtk_err("Wrong UI resolution ? \e[1;31m%s\e[0m\n", para);
	}

	return 0;
}

int is_platform_dvs_enable(void)
{
    return platform_info.enable_dvs;
}

_Bool mem_type_2mc = true;
_Bool is_ddr_unbalance = false;
EXPORT_SYMBOL(mem_type_2mc);
EXPORT_SYMBOL(is_ddr_unbalance);
static int early_parse_mc_dram_num (char *buf)
{
	unsigned int mc_dram_num = 0;
    char *endp;

	if (!buf)
		return -EINVAL;

	mc_dram_num = memparse(buf, &endp);
	if ((mc_dram_num == 0) || (mc_dram_num > 3)) {
		rtd_pr_mach_rtk_err("err: mc_dram_num(%d/%d)\n", mc_dram_num, mem_type_2mc);
		return -EINVAL;
	}

	if (*endp == ',') {
		if (strncmp(endp + 1, "unbalance", 9) == 0)
			is_ddr_unbalance = true;
	}

	mem_type_2mc = (mc_dram_num != 3) ? true : false;
	rtd_pr_mach_rtk_info("mc_dram_num(%d/%d), unbalance=%d\n", mc_dram_num, mem_type_2mc, is_ddr_unbalance);

	return 0;

}

static int early_parse_board_version (char *str)
{
    unsigned int version = 0;

    version = simple_strtoull(str, NULL, 0);

    if (version > 0)
    {
        board_version = (enum BOARD_VERSION)(version - 1);
    }
    else
    {
        board_version = BOARD_VERSION_DEFAULT;
    }
    
    return 0;
}

static int early_parse_memc_init (char *str)
{
	const char *string_true = "true";
	const char *string_false = "false";

	if (strncmp(string_true, str, strlen(string_true))==0)
	{
		memc_init = MEMC_INIT_TRUE;
		rtd_pr_mach_rtk_info("memc init true\n");
	}
	else if (strncmp(string_false, str, strlen(string_false))==0) 
	{
		memc_init = MEMC_INIT_FALSE;
		rtd_pr_mach_rtk_info("memc init false\n");
	}
	else
	{
		memc_init = MEMC_INIT_UNKNOWN;
		rtd_pr_mach_rtk_info("memc init unknown\n");
	}

	return 0;
}

#ifndef BUILD_QUICK_SHOW
#ifdef CONFIG_RTK_KDRV_COMMON
early_param("chip", early_parse_platform);
early_param("chip_product", early_parse_product_type);
early_param("chip_model", early_parse_platform_model);
early_param("chip_disp", early_parse_disp_res);
early_param("chip_ui", early_parse_ui_res);
early_param("mc", early_parse_mc_dram_num);
early_param("board_version", early_parse_board_version);
early_param("memc_init", early_parse_memc_init);
#else //MODULE
void parse_platform(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("chip", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get chip from bootargs\n");
		return;
	}
	early_parse_platform(str);
}
void parse_product(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("chip_product", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get product from bootargs\n");
		return;
	}
	early_parse_product_type(str);
}
void parse_platform_model(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("chip_model", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get chip_model from bootargs\n");
		return;
	}
	early_parse_platform_model(str);
}
void parse_disp_res(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("chip_disp", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get chip_disp from bootargs\n");
		return;
	}
	early_parse_disp_res(str);
}
void parse_ui_res(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("chip_ui", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get chip_ui from bootargs\n");
		return;
	}
	early_parse_ui_res(str);
}
void parse_mc_dram_num(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("mc", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get mc from bootargs\n");
		return;
	}
	early_parse_mc_dram_num(str);
}
void parse_board_version(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("board_version", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_err("Error : can't get board_version from bootargs\n");
		return;
	}
	early_parse_board_version(str);
}
void parse_memc_init(void)
{
	char strings[16];
	char *str = strings;

	if(rtk_parse_commandline_equal("memc_init", str, sizeof(strings)) == 0)
	{
		rtd_pr_mach_rtk_info("Error : can't get memc_init from bootargs\n");
		return;
	}
	early_parse_memc_init(str);
}
#endif
#endif /* !BUILD_QUICK_SHOW */

unsigned int get_ic_version(void)
{
#define GET_IC_VERSION()	rtd_inl(STB_SC_VerID_reg)
#if IS_ENABLED(CONFIG_ARCH_RTK6748)
#define VERSION_A_REG_CODE 	0x67480000
#define VERSION_B_REG_CODE	0x67480010
#elif IS_ENABLED(CONFIG_ARCH_RTK2851C) || IS_ENABLED(CONFIG_ARCH_RTK2851F)
#define VERSION_A_REG_CODE	0x68660000
#define VERSION_B_REG_CODE	0x68660010
#elif IS_ENABLED(CONFIG_ARCH_RTK2885P)
#define VERSION_A_REG_CODE      0x68560000
#define VERSION_B_REG_CODE      0x68560010
#elif IS_ENABLED(CONFIG_ARCH_RTK2875Q)
#define VERSION_A_REG_CODE      0x69820000
#define VERSION_B_REG_CODE      0x69820010
#elif IS_ENABLED(CONFIG_ARCH_RTK2819A)
#define VERSION_A_REG_CODE      0x70020000
#define VERSION_B_REG_CODE      0x70020010
#else //[RL6702-5640]
#define VERSION_A_REG_CODE	0x67020000
#define VERSION_B_REG_CODE	0x67020010
#endif
	if (GET_IC_VERSION() == VERSION_A_REG_CODE)
	{
		return VERSION_A;
	}
	else if (GET_IC_VERSION() == VERSION_B_REG_CODE)
	{
		return VERSION_B;
	}
	else
	{
		return VERSION_C;
	}
}
EXPORT_SYMBOL(get_ic_version);

unsigned int get_bus_pll(void)
{
	unsigned int NCODE, FCODE, PDIV, DIV, reg_bus1_val, reg_bus2_val;
#if IS_ENABLED(CONFIG_ARCH_RTK6702) || IS_ENABLED(CONFIG_ARCH_RTK2851C) || IS_ENABLED(CONFIG_ARCH_RTK2851F) || IS_ENABLED(CONFIG_ARCH_RTK2885P) || IS_ENABLED(CONFIG_ARCH_RTK2819A)
	unsigned int PLLS_DIVMODE;
#endif
	reg_bus1_val = rtd_inl(PLL_REG_SYS_PLL_BUS1_reg);
	reg_bus2_val = rtd_inl(PLL_REG_SYS_PLL_BUS2_reg);

	FCODE = PLL_REG_SYS_PLL_BUS2_get_pllbus_fcode(reg_bus2_val);
	NCODE = PLL_REG_SYS_PLL_BUS2_get_pllbus_ncode(reg_bus2_val);
	PDIV = PLL_REG_SYS_PLL_BUS1_get_pllbus_prediv(reg_bus1_val);
	DIV = PLL_REG_SYS_PLL_BUS1_get_pllbus_o(reg_bus1_val);

#if IS_ENABLED(CONFIG_ARCH_RTK6702) || IS_ENABLED(CONFIG_ARCH_RTK2851C) || IS_ENABLED(CONFIG_ARCH_RTK2851F) || IS_ENABLED(CONFIG_ARCH_RTK2885P) || IS_ENABLED(CONFIG_ARCH_RTK2819A)
	PLLS_DIVMODE = PLL_REG_SYS_PLL_CPU_get_plls_divmode(rtd_inl(PLL_REG_SYS_PLL_CPU_reg));
	return (unsigned)((27*(NCODE+((PLLS_DIVMODE == 0) ? 4: 3))+ 27*FCODE/2048)/(PDIV+1)/(1<<DIV));
#else
	return (unsigned)((27*(NCODE+3)+ 27*FCODE/2048)/(PDIV+1)/(1<<DIV)/2);
#endif
}
EXPORT_SYMBOL(get_bus_pll);

#ifdef CONFIG_RTK_8KCODEC_INTERFACE
#include "kernel/rtk_codec_interface.h"
struct rtk_codec_interface *rtk_8k;
void rtk_codec_interface_register(struct rtk_codec_interface *iface)
{
    rtd_pr_mach_rtk_info("%s, rtk_8k(%p)\n", __func__, iface);
    rtk_8k = iface;
}
void rtk_codec_interface_unregister(void)
{
    rtd_pr_mach_rtk_info("%s\n", __func__);
    rtk_8k = NULL;
}
EXPORT_SYMBOL(rtk_codec_interface_register);
EXPORT_SYMBOL(rtk_codec_interface_unregister);
#endif
