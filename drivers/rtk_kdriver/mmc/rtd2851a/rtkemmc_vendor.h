/*
 * Realtek MMC/SD/SDIO driver
 *
 * Authors:
 * Copyright (C) 2008-2009 Realtek Ltd.
 *
 */
#ifndef _RTKEMMC_VENDOR_H_
#define _RTKEMMC_VENDOR_H_

/*
    Note:
        For Some eMMC, The best phase setting for different SOC may be different.
        Therefore, each SOC has its own settings.

    Vendor ID List
    mid 0x11 ==> KIOXIA(Toshiba)
    mid 0x13 ==> Micron
    mid 0x15 ==> Samsung
    mid 0x2C ==> HIKSEMI
    mid 0x32 ==> Phison
    mid 0x90 ==> Hynix
    mid 0x45 ==> Sandisk
*/


/* ===================================================================
* 1) MMC_DEMO_DFT => setting for demo board. If the vendor of mmc is unknow, then use this.
*        Only one set of parameters in the table can set this flag.
* 2) MMC_VEND_DFT => Specify vendor defaults. Only one set of the same provider can set this flag.
* 3) If there are multiple sets of parameters for the same supplier, the PNM field should be set.
* 4) The count of members of vendor_phase_tbl can't be over 32.
 =================================================================== */
#define MMC_DEMO_DFT        (BIT(0))
#define MMC_VEND_DFT        (BIT(1))
#define MMC_PD_ACCESS       (BIT(4))
vender_info_t vendor_phase_tbl[] = {
    { 0x2C,  0,          /* 0, HSEMSEY2S2B32G */
        {"SEY2S2"},    /* PNM[0:5] */
        15, 15,  8,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        15, 15,  6,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        17, 17,  7,  9,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        17,  9,  7,  9 },    /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x2C,  0,          /* 0, HSEMSES2S2B16G */
        {"SES2S2"},    /* PNM[0:5] */
        15, 15,  6,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        15, 15, 15,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        17, 16,  5, 10,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        17,  9,  5, 10 },    /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x2C,  MMC_VEND_DFT,          /* Defalut as HIKSEMI HSEMSBS3S2B08G */
        {"SBS3S2"},    /* PNM[0:5] */
        15, 15,  6,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        15, 15, 21,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        18, 17,  5, 12,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        18,  8,  5, 12 },   /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x32,  MMC_VEND_DFT,           /* Phison, Pre-define access or default */
        {0,  0,  0,  0,  0,  0},     /* PNM[0:5] */
        18, 18,  8,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        18, 18,  6,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        18, 18,  8, 10,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        18, 10,  8, 10 },   /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x11,  MMC_VEND_DFT,          /* KIOXIA, Pre-define access or default */
        "008GB1",    /* PNM[0:5] */
        18, 18,  8,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        18, 18,  8,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        18, 21,  6, 11,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        18,  7,  6, 11 },   /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x15,  MMC_VEND_DFT,           /* Samsung, Pre-define access or default */
        {0,  0,  0,  0,  0,  0},     /* PNM[0:5] */
        18, 18,  8,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        18, 18,  6,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        16, 16,  6, 12,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        16, 10,  6, 12 },   /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x00,  MMC_DEMO_DFT,          /* Defalut as KIOXIA, Pre-define access or default */
        {0,  0,  0,  0,  0,  0},    /* PNM[0:5] */
        18, 18,  8,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        18, 18,  8,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        18, 21,  6, 11,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        18,  7,  6, 11 }    /* hs_400     -> cmd_w,dq_w,dq_r,ds */
};


#endif  //?_RTKEMMC_VENDOR_H_
