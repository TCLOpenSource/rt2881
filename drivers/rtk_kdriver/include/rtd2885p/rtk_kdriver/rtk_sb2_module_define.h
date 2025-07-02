#ifndef __RTK_SB2_MODULE_DEFINE_H__
#define __RTK_SB2_MODULE_DEFINE_H__

#ifdef CONFIG_ARCH_RTK2885P
static const t_sb2_timout_moudle module_sb2_info[] =
{
    //1800_000  - 1800_ffff
    {0x18000000, 0x18000fff, "Main Top System"},
    {0x18001000, 0x180017ff, "vde"},
    {0x18001800, 0x18001fff, "rng"},
    //{0x18002000, 0x18002fff, "Audio Engine(AE/ADE)"},
    //{0x18003000, 0x18003fff, "DC2_SYS"},
    //{0x18004000, 0x18004fff, "RTMMU"},
    {0x18005000, 0x18005fff, "VODMA"},
    {0x18006000, 0x18006fff, "AIO"},
    {0x18007000, 0x18007fff, "DC_SYS"},

    {0x18008000, 0x18008fff, "VODMA"},
    {0x18009000, 0x18009fff, "ME"},
    //{0x1800A000, 0x1800Afff, "CBUS/ECBUS"},
    {0x1800B000, 0x1800Bfff, "MD"},
    {0x1800c000, 0x1800cfff, "SE"},
    {0x1800D000, 0x1800D9ff, "HDMI"},
    {0x1800DA00, 0x1800Dfff, "HDMIEX"},
    {0x1800e000, 0x1800ffff, "VE"},

    //1801_000  - 1801_ffff
    {0x18010000, 0x180107ff, "nand flash"},
    {0x18010800, 0x18010bff, "emmc"},
    {0x18010c00, 0x18010fff, "SD"},
    {0x18012000, 0x18012fff, "USBEX"},
    {0x18013000, 0x18013fff, "USB"},

    {0x18014000, 0x18014fff, "TP"},
    {0x18015000, 0x18015fff, "CP"},
    {0x18016000, 0x18016fff, "ETN"},
    //{0x18017000, 0x18017fff, "DP"},

    {0x18018000, 0x18018fff, "Video Decorder"},
    {0x18019000, 0x18019fff, "Video Decorder"},
    {0x1801A000, 0x1801Afff, "SB2"},
    {0x1801B000, 0x1801Bfff, "MISC"},
    {0x1801C000, 0x1801Cfff, "System Bridge"},
    {0x1801D000, 0x1801Efff, "ARM"},
    //{0x1801F000, 0x1801Ffff, "TVE"},

    //1802_000  - 1802_ffff
    {0x18020000, 0x18025fff, "DISPI"},
    {0x18026000, 0x18026fff, "LVDS"},
    {0x18027000, 0x18027FFF, "DISPM"},
    {0x18028000, 0x1802cfff, "DISPD"},
    {0x1802d000, 0x1802dfff, "LVDS"},
    {0x1802e000, 0x1802ffff, "DISPD"},

    //1803_000  - 1803_ffff
    {0x18030000, 0x18030fff, "VBI Slicer"},
    //{0x18031000, 0x18031FFF, "OSD_COMP"},   //1200 ~ 1fff : reserved

    {0x18032000, 0x18032fff, "IF DEMOD(Digital)"},
    {0x18033000, 0x18033fff, "IF DEMOD(Analog)"},
    {0x18034000, 0x18035fff, "VE2"},
    //{0x18036000, 0x18036FFF, "Main Top System"},
    {0x18037000, 0x18037FFF, "VDE2"},

    {0x18038000, 0x18038fff, "efuse"},
    {0x18039000, 0x18039FFF, "MISC"},
    //{0x1803A000, 0x1803AFFF, "Audio Engine(AE2/ADE2)"},
    {0x1803B000, 0x1803B9FF, "HDMI"},
    {0x1803BA00, 0x1803BFFF, "HDMIEX"},
    {0x1803C000, 0x1803CFFF, "HDMI"},
    //{0x1803D000, 0x1803D7FF, "hdmitx"},
    //{0x1803D800, 0x1803DFFF, "disp2tve"},
    {0x1803E000, 0x1800EFFF, "SE"},
    //{0x1803F000, 0x1803FFFF, "ib(inter bridge)"},

    //1804_000  - 1804_ffff
    //reserved

    //1805_000  - 1805_ffff
    {0x18050000, 0x1805AFFF, "USB"},
    {0x1805B000, 0x1805BFFF, "ARM(secure)"},
    {0x1805C000, 0x1805CFFF, "ARM(non-secure)"},
    {0x1805D000, 0x1805DFFF, "USB_BB"},
    {0x1805E000, 0x1805EFFF, "USB_HUB"},
    //{0x1805F000, 0x1805FFFF, "USB"},

    //1806_000  - 1806_ffff
    {0x18060000, 0x18060FFF, "Satndby Top/system"},
    {0x18061000, 0x18065FFF, "ST PERI"},
    {0x18066000, 0x18066FFF, "DISPD"},
    {0x18067000, 0x18067FFF, "GDMA"},
    {0x18068000, 0x18068FFF, "LZMA"},
    {0x18069000, 0x18069FFF, "GDCMP"},
    {0x1806A000, 0x1806AFFF, "LVDS"},
    {0x1806B000, 0x1806BFFF, "DOLBY_V"},
    //{0x1806C000, 0x1806CFFF, "NAG"},
    {0x1806D000, 0x1806DFFF, "DOLBY_V"},
    {0x1806E000, 0x1806EFFF, "RSA(CP)"},
    {0x1806F000, 0x1806FFFF, "DOLBY_V"},

    //1807_000  - 1807_ffff
    {0x18070000, 0x18072FFF, "ST2"},
    {0x18073000, 0x18076FFF, "NNIP"},
    {0x18077000, 0x18077FFF, "ACPU ARM(secure)"},
    {0x18078000, 0x18078FFF, "ACPU ARM(non-secure)"},
    {0x18079000, 0x18079FFF, "SRNN"},
    //{0x1807A000, 0x1807AFFF, "GPNN"},
    {0x1807B000, 0x1807BFFF, "DPRX"},
    {0x1807C000, 0x1807EFFF, "PCIE"},
    {0x1807F000, 0x1807FFFF, "SPI_SYNCHRONIZER"},

    //1808_000  - 1808_ffff
    //{0x18080000, 0x1808FFFF, "GIC"},

    //1809_000  - 1809_ffff
    {0x18090000, 0x18098FFF, "USB_OTG"},
    {0x18099000, 0x1809FFFF, "MEMC"},

    //180A_000  - 180A_ffff
    //{0x180A0000, 0x180AFFFF, "CCI"},

    //180B_000  - 180B_ffff
    //{0x180B0000, 0x180B07FF, "DC_PHY_SECU"},
    //{0x180B0800, 0x180B0FFF, "DC2_PHY_SECU"},
    {0x180B1000, 0x180B1FFF, "PST"},    //IMD scaler
    {0x180B2000, 0x180B2FFF, "HDMI"},
    //{0x180B3000, 0x180B30FF, "DC_PATGEN"},
    //{0x180B3100, 0x180B31FF, "DC2_PATGEN"},
    {0x180B4000, 0x180B4FFF, "HDMIEX"},
    {0x180B5000, 0x180B5FFF, "VODMA"},
    //{0x180B6000, 0x180B64FF, "SB3_BLK_WRP"},
    {0x180B6500, 0x180B69FF, "VE_BLK_WRP"},
    //{0x180B6A00, 0x180B6FFF, "VE2_BLK_WRP"},
    {0x180B7000, 0x180B7FFF, "HDMI"},
    {0x180B8000, 0x180B8FFF, "EARC"},
    {0x180B9000, 0x180BAFFF, "HDMI"},
    //{0x180BB000, 0x180BB7FF, "DSCE"},
    {0x180BB800, 0x180BBFFF, "DSCD"},
    {0x180BC000, 0x180BCFFF, "HDMI"},
    {0x180BD000, 0x180BDFFF, "HDMI_STB"},
    //{0x180BE000, 0x180BEFFF, "DC_PHY_DPI"},
    /* f000-ffff  reserved*/
    
    //180C_000  - 180C_ffff
    {0x180C0000, 0x180C01FF, "DC_PATGEN1"},
    {0x180C0200, 0x180C03FF, "DC_PATGEN2"},
    //{0x180C0400, 0x180C05FF, "DC_PATGEN3"},
    {0x180C1000, 0x180C1FFF, "DC_MC_EXP"},
    {0x180C2000, 0x180C2FFF, "DC_MC1"},
    {0x180C3000, 0x180C3FFF, "DC_MC2"},
    {0x180C4000, 0x180C4FFF, "DC_MC3"},
    //{0x180C5000, 0x180C5FFF, "DC_MC4"},
    //{0x180C6000, 0x180C6FFF, "DC_MC5"},
    //{0x180C7000, 0x180C7FFF, "DC_MC6"},
    /* 0x180C8000-0x180C8FFF  reserved*/
    {0x180C9000, 0x180C9FFF, "DC1_PHY_DPI"},
    {0x180CA000, 0x180CAFFF, "DC2_PHY_DPI"},
    //{0x180CB000, 0x180CBFFF, "DC3_PHY_DPI"},
    /* 0x180CC000-0x180CCFFF  reserved*/
    {0x180CD000, 0x180CD7FF, "DC_MC1_SECU"},
    //{0x180CD800, 0x180CDFFF, "DC_MC2_SECU"},
    {0x180CE000, 0x180CE7FF, "DC_MC3_SECU"},
    //{0x180CE800, 0x180CEFFF, "DC_MC4_SECU"},
    //{0x180CF000, 0x180CF7FF, "DC_MC5_SECU"},
    //{0x180CF800, 0x180CFFFF, "DC_MC6_SECU"},

    //180D_000  - 180D_ffff
    {0x180D0000, 0x180D8FFF, "USBEX"},
    /* 0x180D9000-0x180D9FFF  reserved*/
    {0x180DA000, 0x180DAFFF, "USBEX"},
    /* 0x180DB000-0x180DEFFF  reserved*/
    {0x180DF000, 0x180DFFFF, "USBEX"},

    //180E_000  - 180F_ffff
    //{0x180E0000, 0x180FFFFF, "demod core"},
    
    //1810_000  - 1810_ffff
    //{0x18100000, 0x1810DFFF, "GPU"},
    {0x1810E000, 0x1810FFFF, "GPU"},
    
    //1811_000  - 1811_ffff
    //{0x18110000, 0x1811FFFF, "GPU"},
    
    //1812_000  - 1812_ffff
    //{0x18120000, 0x1812FFFF, "GPU"},
    
    //1813_000  - 1813_ffff
    //{0x18130000, 0x1813FFFF, "GPU"},

    //1814_000  - 1815_ffff
    {0x18140000, 0x1815FFFF, "RTK_DTV_DEMOD"},

    //1816_000  - 1816_ffff
    {0x18160000, 0x1816FFFF, "ATB_DEMOD"},
    
    //1817_000  - 1817_ffff
    {0x18170000, 0x1817FFFF, "HDIC_DEMOD"},
    
    //1818_000  - 181a_ffff
    //{0x18180000, 0x181AFFFF, "SSRS"},
    
    //181b_000  - 181b_ffff
    //{0x181B0000, 0x181BFFFF, "SDEMURA"},
    
    //181c_000  - 181c_ffff
    //{0x181C0000, 0x181C0FFF, "SSRS"},
    //{0x181C1000, 0x181C1FFF, "SDEMURA"},
    {0x181C2000, 0x181C3FFF, "DOLBY"},
    /* 0x181C4000-0x181C5FFF  reserved*/
    {0x181C6000, 0x181C7FFF, "LVDS"},
    /* 0x181C8000-0x181CFFFF  reserved*/
    
    //181d_000  - 181f_ffff
};

char* emcu_rbus_timeout_ip[] = 
{
    "no rbus_timeout_int",
    "pmmsys",
    "pmmalu(no use)",
    "pmmwdog",
    "pmmps",
    "emcu",
    "typecpd",
    "share",
    "iso_misc",
    "cec",
    "pmmwov",
    "dprx_aux",
    "iso_misc_off",
    "hdmi",
};

#define EMCU_RBUS_TIMEOUT_IP_ARRAY_NUM (sizeof(emcu_rbus_timeout_ip)/sizeof(emcu_rbus_timeout_ip[0]))

#else
#error "Invalid configuration!\n"
#endif

#endif /*__RTK_SB2_MODULE_DEFINE_H__ */

