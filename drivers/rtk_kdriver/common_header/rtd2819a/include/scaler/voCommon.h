/***************************************************************************
                          vdcCommon.h  -  description
                             -------------------
    begin                : 2011/2/17
    copyright           : (C) 2011 by olivershih
    email                : olivershih@realtek.com
 ***************************************************************************/

#ifndef _COMMON_VO_H__
#define _COMMON_VO_H__

// Default vodmapll settings without panel info
#define XTAL_TICK                   27000000UL
#define VODMA_PLL_FCODE_BASIC       2048
#define VODMA_PLL_MCODE_BASIC       4
#define PLLVODMA_PREDIV             1
#define PLLVODMA_O                  0
#define PLLVODMA_CP                 0
#define PLLVODMA_ICP                0
#define PLLVODMA_RS                 9
#define PLLVODMA_WDSET              0
#define PLLVODMA_WDRST              1
#ifdef CONFIG_REALTEK_2K_MODEL_ENABLED
#define PLLVODMA_F                  0   /* 2K Model 297M */
#define PLLVODMA_N                  18
#else
#define PLLVODMA_F                  911 /* Default 735M */
#define PLLVODMA_N                  50
#endif

// Set vodmapll clock by panel info
#define BOOT_PANEL_INFO_ADDR        (0x00001300)
#define BOOT_PANEL_INFO_SIZE        (0x40)
#define VO_MCODE_BASE               (XTAL_TICK >> 1) // Value = (XTAL_TICK / 2) = 13500000
#define VO_DEFINED_2K1K_WIDTH       1920
#define VO_DEFINED_2K1K_HEIGHT      1080
#define VO_DEFINED_4K2K_WIDTH       3840
#define VO_DEFINED_4K2K_HEIGHT      2160
#define VO_DEFINED_2K1K_SAFE_HTOTAL 2881
#define VO_DEFINED_2K1K_HTOTAL      2200 //VO_FIXED_2K1K_HTOTAL:2480
#define VO_DEFINED_4K2K_HTOTAL      4400 //VO_FIXED_4K2K_HTOTAL:4350
#ifdef CONFIG_REALTEK_2K_MODEL_ENABLED
#define VO_DEFINED_PLL              297000000
#define VO_DEFINED_HTOTAL           VO_DEFINED_2K1K_HTOTAL
#define VO_DEFINED_VACT             VO_DEFINED_2K1K_HEIGHT
#define VO_DEFINED_PANEL_HMIN       VO_DEFINED_2K1K_WIDTH
#else
#define VO_DEFINED_PLL              594000000
#define VO_DEFINED_HTOTAL           VO_DEFINED_4K2K_HTOTAL
#define VO_DEFINED_VACT             VO_DEFINED_4K2K_HEIGHT
#define VO_DEFINED_PANEL_HMIN       VO_DEFINED_4K2K_WIDTH
#endif
#define VO_DEFINED_HPORCH_MIN       80 //[ML8BU-1004] dispi_ccdinr_top min
#define VO_DEFINED_VBACKPORCH_MIN   18
#define VO_DEFINED_HSTA_MIN         (VO_DEFINED_HPORCH_MIN >> 1)
#define VO_DEFINED_VFRONTPORCH_MIN  8 //Edge smooth Linebuf 17line mode: 8 line
#define VO_DEFINED_VPORCH_MIN       (VO_DEFINED_VFRONTPORCH_MIN + VO_DEFINED_VBACKPORCH_MIN)
#define VO_DEFINED_VSTA_MIN         VO_DEFINED_VBACKPORCH_MIN
#define VO_SUPPORT_SAFE_MAXPLL      829000000 // Max 829M
#define VO_SUPPORT_LIMIT_MAXPLL     833000000 // Force HW
#endif

