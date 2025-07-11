#ifndef __RTK_SB2_MODULE_DEFINE_H__
#define __RTK_SB2_MODULE_DEFINE_H__

#ifdef CONFIG_ARCH_RTK2851A
static const t_sb2_timout_moudle module_sb2_info[] =
{
    //1800_000  - 1800_ffff
    {0x18000000, 0x18000fff, "Main Top System"},
    {0x18001000, 0x18001fff, "vde"},
    //{0x18001800, 0x18001fff, "rng"},
    {0x18002000, 0x18002fff, "Audio Engine(AE/ADE)"},
    //{0x18003000, 0x18003fff, "DC2_SYS"},
    //{0x18004000, 0x18004fff, "DC2_PHY"},
    {0x18005000, 0x18005fff, "VODMA"},
    {0x18006000, 0x18006fff, "AIO"},
    {0x18007000, 0x18007fff, "DC_SYS"},

    {0x18008000, 0x18008fff, "DC_PHY"},
    {0x18009000, 0x18009fff, "ME"},
    //{0x1800A000, 0x1800Afff, "CBUS/ECBUS"},
    {0x1800B000, 0x1800Bfff, "MD"},
    {0x1800c000, 0x1800cfff, "SE"},
    {0x1800D000, 0x1800Dfff, "HDMI"},
    {0x1800e000, 0x1800ffff, "VE"},

    //1801_000  - 1801_ffff
    {0x18010000, 0x180107ff, "nand flash"},
    {0x18010800, 0x18010bff, "emmc"},
    {0x18010c00, 0x18010fff, "SD"},
    //{0x18012000, 0x18012fff, "USBEX"},
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
    {0x18020000, 0x18025fff, "DISPIM"},
    {0x18026000, 0x18026fff, "LVDS"},
    {0x18027000, 0x18027FFF, "DISPIM"},
    {0x18028000, 0x1802Cfff, "DISPD"},
    {0x1802D000, 0x1802Dfff, "LVDS"},
    {0x1802E000, 0x1802ffff, "DISPD"},

    //1803_000  - 1803_ffff
    {0x18030000, 0x18030fff, "VBI Slicer"},
    //{0x18031000, 0x18031FFF, "OSD_COMP"},   //1200 ~ 1fff : reserved

    {0x18032000, 0x18032fff, "IF DEMOD(Digital)"},
    {0x18033000, 0x18033fff, "IF DEMOD(Analog)"},
    //{0x18034000, 0x180343ff, "VP9"},
    //{0x18035000, 0x18036FFF, "VE2"},
    //{0x18037000, 0x18037FFF, "VDE2"},

    {0x18038000, 0x18038fff, "efuse"},
    {0x18039000, 0x18039FFF, "MISC"},
    //{0x1803A000, 0x1803AFFF, "Audio Engine(AE2/ADE2)"},
    {0x1803B000, 0x1803CFFF, "HDMI"},
    //{0x1803D000, 0x1803D7FF, "hdmitx"},
    //{0x1803D800, 0x1803DFFF, "disp2tve"},
    {0x1803E000, 0x1800EFFF, "SE"},
    //{0x1803F000, 0x1803FFFF, "ib(inter bridge)"},

    //1804_000  - 1804_ffff
    //reserved

    //1805_000  - 1805_ffff
    {0x18050000, 0x18058FFF, "USB"},
    //{0x18059000, 0x18059FFF, "DRAM Control"},
    {0x1805A000, 0x1805AFFF, "USB"},
    {0x1805B000, 0x1805BFFF, "ARM(secure)"},
    {0x1805C000, 0x1805CFFF, "ARM(non-secure)"},
    //{0x1805D000, 0x1805DFFF, "MIPS"},
    /* E000-Efff  reserved*/
    {0x1805F000, 0x1805FFFF, "USB"},

    //1806_000  - 1806_ffff
    {0x18060000, 0x18060FFF, "Satndby Top/system"},
    {0x18061000, 0x18062FFF, "ST PERI"},
    /* 3000-3fff  reserved*/
    //{0x18064000, 0x18067FFF, "DI_DMA"},
    {0x18068000, 0x18068FFF, "LZMA"},
    {0x18069000, 0x18069FFF, "GDCMP"},
    /* A000-Afff  reserved*/
    {0x1806B000, 0x1806BFFF, "DOLBY_V"},
    //{0x1806C000, 0x1806CFFF, "certid"},
    //{0x1806D000, 0x1806DFFF, "KT"},
    {0x1806E000, 0x1806EFFF, "RSA(CP)"},
    //{0x1806F000, 0x1806FFFF, "SCE"},

    //1807_000  - 1807_ffff
    //{0x18070000, 0x18072FFF, "demod wrapper"},
    {0x18073000, 0x18076FFF, "NNIP"},
    /* 7000-Ffff  reserved*/

    //1808_000  - 1808_ffff
    {0x18080000, 0x1808FFFF, "GIC"},

    //1809_000  - 1809_ffff
    //{0x18090000, 0x18098FFF, "USB_OTG"},
    //{0x18099000, 0x1809FFFF, "MEMC"},

    //180A_000  - 180A_ffff
    {0x180A0000, 0x180AFFFF, "CCI"},

    //180B_000  - 180B_ffff
    {0x180B0000, 0x180B07FF, "DC_PHY_SECU"},
    //{0x180B0800, 0x180B0FFF, "DC2_PHY_SECU"},
    //{0x180B1000, 0x180B1FFF, "PST"},    //IMD scaler
    {0x180B2000, 0x180B2FFF, "HDMI"},
    {0x180B3000, 0x180B3FFF, "DC_PATGEN"},
    {0x180B4000, 0x180B4FFF, "HDMIEX"},
    /* 5000-5fff  reserved*/
    //{0x180B6000, 0x180B64FF, "SB3_BLK_WRP"},
    {0x180B6500, 0x180B69FF, "VE_BLK_WRP"},
    //{0x180B6A00, 0x180B6FFF, "VE2_BLK_WRP"},
    {0x180B7000, 0x180B7FFF, "HDMI"},
    {0x180B8000, 0x180B8FFF, "EARC"},
    {0x180B9000, 0x180BAFFF, "HDMI"},
    //{0x180BB000, 0x180BBFFF, "DSCD"},
    {0x180BC000, 0x180BDFFF, "HDMI"},
    {0x180BE000, 0x180BEFFF, "DC_PHY_DPI"},
    /* f000-ffff  reserved*/

    //{0x180C0000, 0x180FFFFF, "demod core"},


    {0x18100000, 0x1813FFFF, "gpu"},

    {0x18140000, 0x1815FFFF, "rtk_dtv_demod"},

    {0x18160000, 0x1816FFFF, "atb_demod"},
    {0x18170000, 0x1817FFFF, "hdic_demod"}
};
#else
#error "Invalid configuration!\n"
#endif

#endif /*__RTK_SB2_MODULE_DEFINE_H__ */

