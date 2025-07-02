/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdmiHdcp2Tx2.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <linux/kernel.h>
#include <linux/module.h>
#include <rbus/hdmitx21_mac_reg.h>
#include <rbus/hdmitx20_mac0_reg.h>
#include <rbus/hdmitx_misc_reg.h>
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp14_tx.h>
#ifdef HDCP_HW_SPEED_UP
    #include <rbus/hdcp22_speedup_reg.h>
#endif
#include "hdcp2tx_sys.h"
#include "hdcp2tx_common.h"
#include "hdcp_cal.h"
#include <i2c-rtk-api.h>

#ifdef HDCP_DEFAULT_KEY
#ifdef HDCP_HW_SPEED_UP
unsigned char hdcp_22_tx_key[800 + 16] = {
#ifdef CONFIG_HDMIHDCP2TX_TEST   //sample key just for test
    0x93, 0xCE, 0x5A, 0x56, 0xA0, 0xA1, 0xF4, 0xF7, 0x3C, 0x65, 0x8A, 0x1B, 0xD2, 0xAE, 0xF0, 0xF7,
    0xA2, 0xC7, 0x55, 0x57, 0x54, 0xCB, 0xAA, 0xA7, 0x7A, 0x27, 0x92, 0xC3, 0x1A, 0x6D, 0xC2, 0x31,
    0xCF, 0x12, 0xC2, 0x24, 0xBF, 0x89, 0x72, 0x46, 0xA4, 0x8D, 0x20, 0x83, 0xB2, 0xDD, 0x04, 0xDA,
    0x7E, 0x01, 0xA9, 0x19, 0xEF, 0x7E, 0x8C, 0x47, 0x54, 0xC8, 0x59, 0x72, 0x5C, 0x89, 0x60, 0x62,
    0x9F, 0x39, 0xD0, 0xE4, 0x80, 0xCA, 0xA8, 0xD4, 0x1E, 0x91, 0xE3, 0x0E, 0x2C, 0x77, 0x55, 0x6D,
    0x58, 0xA8, 0x9E, 0x3E, 0xF2, 0xDA, 0x78, 0x3E, 0xBA, 0xD1, 0x05, 0x37, 0x07, 0xF2, 0x88, 0x74,
    0x0C, 0xBC, 0xFB, 0x68, 0xA4, 0x7A, 0x27, 0xAD, 0x63, 0xA5, 0x1F, 0x67, 0xF1, 0x45, 0x85, 0x16,
    0x49, 0x8A, 0xE6, 0x34, 0x1C, 0x6E, 0x80, 0xF5, 0xFF, 0x13, 0x72, 0x85, 0x5D, 0xC1, 0xDE, 0x5F,
    0x01, 0x86, 0x55, 0x86, 0x71, 0xE8, 0x10, 0x33, 0x14, 0x70, 0x2A, 0x5F, 0x15, 0x7B, 0x5C, 0x65,
    0x3C, 0x46, 0x3A, 0x17, 0x79, 0xED, 0x54, 0x6A, 0xA6, 0xC9, 0xDF, 0xEB, 0x2A, 0x81, 0x2A, 0x80,
    0x2A, 0x46, 0xA2, 0x06, 0xDB, 0xFD, 0xD5, 0xF3, 0xCF, 0x74, 0xBB, 0x66, 0x56, 0x48, 0xD7, 0x7C,
    0x6A, 0x03, 0x14, 0x1E, 0x55, 0x56, 0xE4, 0xB6, 0xFA, 0x38, 0x2B, 0x5D, 0xFB, 0x87, 0x9F, 0x9E,
    0x78, 0x21, 0x87, 0xC0, 0x0C, 0x63, 0x3E, 0x8D, 0x0F, 0xE2, 0xA7, 0x19, 0x10, 0x9B, 0x15, 0xE1,
    0x11, 0x87, 0x49, 0x33, 0x49, 0xB8, 0x66, 0x32, 0x28, 0x7C, 0x87, 0xF5, 0xD2, 0x2E, 0xC5, 0xF3,
    0x66, 0x2F, 0x79, 0xEF, 0x40, 0x5A, 0xD4, 0x14, 0x85, 0x74, 0x5F, 0x06, 0x43, 0x50, 0xCD, 0xDE,
    0x84, 0xE7, 0x3C, 0x7D, 0x8E, 0x8A, 0x49, 0xCC, 0x5A, 0xCF, 0x73, 0xA1, 0x8A, 0x13, 0xFF, 0x37,
    0x13, 0x3D, 0xAD, 0x57, 0xD8, 0x51, 0x22, 0xD6, 0x32, 0x1F, 0xC0, 0x68, 0x4C, 0xA0, 0x5B, 0xDD,
    0x5F, 0x78, 0xC8, 0x9F, 0x2D, 0x3A, 0xA2, 0xB8, 0x1E, 0x4A, 0xE4, 0x08, 0x55, 0x64, 0x05, 0xE6,
    0x94, 0xFB, 0xEB, 0x03, 0x6A, 0x0A, 0xBE, 0x83, 0x18, 0x94, 0xD4, 0xB6, 0xC3, 0xF2, 0x58, 0x9C,
    0x7A, 0x24, 0xDD, 0xD1, 0x3A, 0xB7, 0x3A, 0xB0, 0xBB, 0xE5, 0xD1, 0x28, 0xAB, 0xAD, 0x24, 0x54,
    0x72, 0x0E, 0x76, 0xD2, 0x89, 0x32, 0xEA, 0x46, 0xD3, 0x78, 0xD0, 0xA9, 0x67, 0x78, 0xC1, 0x2D,
    0x18, 0xB0, 0x33, 0xDE, 0xDB, 0x27, 0xCC, 0xB0, 0x7C, 0xC9, 0xA4, 0xBD, 0xDF, 0x2B, 0x64, 0x10,
    0x32, 0x44, 0x06, 0x81, 0x21, 0xB3, 0xBA, 0xCF, 0x33, 0x85, 0x49, 0x1E, 0x86, 0x4C, 0xBD, 0xF2,
    0x3D, 0x34, 0xEF, 0xD6, 0x23, 0x7A, 0x9F, 0x2C, 0xDA, 0x84, 0xF0, 0x83, 0x83, 0x71, 0x7D, 0xDA,
    0x6E, 0x44, 0x96, 0xCD, 0x1D, 0x05, 0xDE, 0x30, 0xF6, 0x1E, 0x2F, 0x9C, 0x99, 0x9C, 0x60, 0x07,
    0x95, 0xCC, 0x1B, 0x72, 0xDA, 0x8B, 0xC8, 0xF4, 0xE4, 0x88, 0x48, 0x08, 0x40, 0xBD, 0x48, 0xFE,
    0xF4, 0xE3, 0xDB, 0x41, 0x63, 0x41, 0x66, 0x06, 0xBF, 0xAA, 0xB9, 0xC4, 0x3B, 0x7A, 0x6E, 0xCC,
    0x3B, 0xAF, 0xBB, 0x41, 0x83, 0xBE, 0x1A, 0x6A, 0x5D, 0xAF, 0x48, 0x52, 0x72, 0x11, 0xF8, 0xDC,
    0x7F, 0xF0, 0xBD, 0xA5, 0x72, 0x47, 0x55, 0xEB, 0x43, 0x98, 0xCB, 0x07, 0x3A, 0xDD, 0xE3, 0x3B,
    0x40, 0xFD, 0x4C, 0x65, 0x86, 0x2B, 0x64, 0x97, 0x86, 0xCB, 0xD0, 0x4A, 0x4A, 0x9E, 0x84, 0x73,
    0x85, 0xEB, 0xE2, 0x9C, 0xDD, 0x88, 0xF6, 0xF5, 0xE7, 0xEC, 0x1D, 0xDC, 0xD9, 0x93, 0x04, 0xE2,
    0x74, 0x29, 0x8E, 0x5B, 0xF7, 0x1D, 0x51, 0x6F, 0xEA, 0xC5, 0x47, 0x98, 0x41, 0xE4, 0x24, 0xB7,
    0x9B, 0xB7, 0x0C, 0x5F, 0x2F, 0x86, 0x09, 0x6B, 0x19, 0xC9, 0xBB, 0x60, 0x8E, 0xFC, 0xB3, 0x8F,
    0xAF, 0x23, 0x45, 0xB5, 0x54, 0x06, 0x39, 0x9F, 0x93, 0x35, 0x90, 0x32, 0x55, 0x1B, 0xF7, 0xCF,
    0xC7, 0xDD, 0xBE, 0xB5, 0xF8, 0x97, 0xF4, 0x5C, 0x05, 0x64, 0x81, 0x6F, 0x0F, 0x15, 0x97, 0x23,
    0x5E, 0xD6, 0x6D, 0x22, 0xB3, 0x45, 0x6E, 0xFA, 0x55, 0x5B, 0x4D, 0xBC, 0x48, 0xA9, 0xEE, 0x6B,
    0x93, 0xF4, 0x34, 0x4A, 0x4A, 0xF2, 0x9D, 0x09, 0x90, 0x4B, 0xD8, 0xAC, 0x82, 0x9E, 0x5E, 0xFF,
    0xB1, 0xC8, 0x12, 0x9C, 0x46, 0xAC, 0x9E, 0xED, 0x98, 0x08, 0xFE, 0x11, 0xCF, 0x38, 0x7A, 0xFA,
    0x58, 0x57, 0xED, 0x44, 0xF8, 0x5B, 0xA2, 0x51, 0x69, 0x8F, 0x23, 0x13, 0x71, 0xE8, 0x68, 0xAC,
    0xF0, 0x8B, 0xEF, 0x69, 0xAA, 0x03, 0x82, 0x71, 0x1A, 0xE8, 0x4A, 0xBA, 0x30, 0x81, 0x11, 0x51,
    0x5B, 0x8B, 0x55, 0xFD, 0xE1, 0xB7, 0xB7, 0x8A, 0x5E, 0x2D, 0x22, 0x4D, 0x29, 0xA4, 0x94, 0x96,
    0xFE, 0x40, 0x9B, 0x26, 0x69, 0x69, 0x41, 0x32, 0xC5, 0x64, 0xE5, 0x3F, 0x14, 0x3E, 0x53, 0x80,
    0x9F, 0x5A, 0x6D, 0x6C, 0xEF, 0x5F, 0x15, 0xAF, 0xE9, 0x42, 0x49, 0xB5, 0x39, 0x1B, 0x83, 0xE0,
    0x14, 0x4B, 0xDE, 0xA3, 0xF2, 0xC0, 0xCC, 0x33, 0x1F, 0xE6, 0xE3, 0x1F, 0xF8, 0xF5, 0x4C, 0x87,
    0x7B, 0x12, 0x8B, 0xE9, 0x4C, 0xA6, 0xEF, 0x2A, 0xFD, 0x79, 0xB3, 0x6D, 0x8E, 0xA7, 0xF6, 0x07,
    0x3A, 0x33, 0x9B, 0x4B, 0x1A, 0x11, 0x3F, 0x6B, 0x10, 0x66, 0x09, 0xC8, 0xE2, 0x12, 0x61, 0x9B,
    0x53, 0xB4, 0x3C, 0x1D, 0x8C, 0x16, 0x3E, 0xCB, 0x7D, 0xD9, 0x1A, 0x3A, 0x0A, 0x0E, 0x82, 0xD2,
    0x3C, 0x63, 0xAD, 0x55, 0x8C, 0x24, 0x88, 0x6A, 0x9A, 0x84, 0xFF, 0xBD, 0x35, 0x83, 0x90, 0xF2,
    0x01, 0xB4, 0x73, 0x59, 0x12, 0x26, 0x83, 0xB1, 0xFC, 0x43, 0x86, 0x6E, 0x06, 0xBE, 0x1A, 0x35,
    0x96, 0xCE, 0xF2, 0x49, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#else //default key
    0xB5, 0xD8, 0xE9, 0xAB, 0x5F, 0x8A, 0xFE, 0xCA, 0x38, 0x55, 0xB1, 0xA5, 0x1E, 0xC9, 0xBC, 0x0F,
    0xB0, 0xE9, 0xAA, 0x45, 0xF1, 0x29, 0xBA, 0x0A, 0x1C, 0xBE, 0x17, 0x57, 0x28, 0xEB, 0x2B, 0x4E,
    0x8F, 0xD0, 0xC0, 0x6A, 0xAD, 0x79, 0x98, 0x0F, 0x8D, 0x43, 0x8D, 0x47, 0x04, 0xB8, 0x2B, 0xF4,
    0x15, 0x21, 0x56, 0x19, 0x01, 0x40, 0x01, 0x3B, 0xD0, 0x91, 0x90, 0x62, 0x9E, 0x89, 0xC2, 0x27,
    0x8E, 0xCF, 0xB6, 0xDB, 0xCE, 0x3F, 0x72, 0x10, 0x50, 0x93, 0x8C, 0x23, 0x29, 0x83, 0x7B, 0x80,
    0x64, 0xA7, 0x59, 0xE8, 0x61, 0x67, 0x4C, 0xBC, 0xD8, 0x58, 0xB8, 0xF1, 0xD4, 0xF8, 0x2C, 0x37,
    0x98, 0x16, 0x26, 0x0E, 0x4E, 0xF9, 0x4E, 0xEE, 0x24, 0xDE, 0xCC, 0xD1, 0x4B, 0x4B, 0xC5, 0x06,
    0x7A, 0xFB, 0x49, 0x65, 0xE6, 0xC0, 0x00, 0x83, 0x48, 0x1E, 0x8E, 0x42, 0x2A, 0x53, 0xA0, 0xF5,
    0x37, 0x29, 0x2B, 0x5A, 0xF9, 0x73, 0xC5, 0x9A, 0xA1, 0xB5, 0xB5, 0x74, 0x7C, 0x06, 0xDC, 0x7B,
    0x7C, 0xDC, 0x6C, 0x6E, 0x82, 0x6B, 0x49, 0x88, 0xD4, 0x1B, 0x25, 0xE0, 0xEE, 0xD1, 0x79, 0xBD,
    0x39, 0x85, 0xFA, 0x4F, 0x25, 0xEC, 0x70, 0x19, 0x23, 0xC1, 0xB9, 0xA6, 0xD9, 0x7E, 0x3E, 0xDA,
    0x48, 0xA9, 0x58, 0xE3, 0x18, 0x14, 0x1E, 0x9F, 0x30, 0x7F, 0x4C, 0xA8, 0xAE, 0x53, 0x22, 0x66,
    0x2B, 0xBE, 0x24, 0xCB, 0x47, 0x66, 0xFC, 0x83, 0xCF, 0x5C, 0x2D, 0x1E, 0x3A, 0xAB, 0xAB, 0x06,
    0xBE, 0x05, 0xAA, 0x1A, 0x9B, 0x2D, 0xB7, 0xA6, 0x54, 0xF3, 0x63, 0x2B, 0x97, 0xBF, 0x93, 0xBE,
    0xC1, 0xAF, 0x21, 0x39, 0x49, 0x0C, 0xE9, 0x31, 0x90, 0xCC, 0xC2, 0xBB, 0x3C, 0x02, 0xC4, 0xE2,
    0xBD, 0xBD, 0x2F, 0x84, 0x63, 0x9B, 0xD2, 0xDD, 0x78, 0x3E, 0x90, 0xC6, 0xC5, 0xAC, 0x16, 0x77,
    0x2E, 0x69, 0x6C, 0x77, 0xFD, 0xED, 0x8A, 0x4D, 0x6A, 0x8C, 0xA3, 0xA9, 0x25, 0x6C, 0x21, 0xFD,
    0xB2, 0x94, 0x0C, 0x84, 0xAA, 0x07, 0x29, 0x26, 0x46, 0xF7, 0x9B, 0x3A, 0x19, 0x87, 0xE0, 0x9F,
    0xEB, 0x30, 0xA8, 0xF5, 0x64, 0xEB, 0x07, 0xF1, 0xE9, 0xDB, 0xF9, 0xAF, 0x2C, 0x8B, 0x69, 0x7E,
    0x2E, 0x67, 0x39, 0x3F, 0xF3, 0xA6, 0xE5, 0xCD, 0xDA, 0x24, 0x9B, 0xA2, 0x78, 0x72, 0xF0, 0xA2,
    0x27, 0xC3, 0xE0, 0x25, 0xB4, 0xA1, 0x04, 0x6A, 0x59, 0x80, 0x27, 0xB5, 0xDA, 0xB4, 0xB4, 0x53,
    0x97, 0x3B, 0x28, 0x99, 0xAC, 0xF4, 0x96, 0x27, 0x0F, 0x7F, 0x30, 0x0C, 0x4A, 0xAF, 0xCB, 0x9E,
    0xD8, 0x71, 0x28, 0x24, 0x3E, 0xBC, 0x35, 0x15, 0xBE, 0x13, 0xEB, 0xAF, 0x43, 0x01, 0xBD, 0x61,
    0x24, 0x54, 0x34, 0x9F, 0x73, 0x3E, 0xB5, 0x10, 0x9F, 0xC9, 0xFC, 0x80, 0xE8, 0x4D, 0xE3, 0x32,
    0x96, 0x8F, 0x88, 0x10, 0x23, 0x25, 0xF3, 0xD3, 0x3E, 0x6E, 0x6D, 0xBB, 0xDC, 0x29, 0x66, 0xEB,
    0xAB, 0x75, 0xB5, 0x96, 0x35, 0x88, 0x04, 0xBE, 0xC0, 0x1D, 0x06, 0x8A, 0x8B, 0xFB, 0x2E, 0xC8,
    0x90, 0x7F, 0xA6, 0xE8, 0xF3, 0x90, 0x52, 0x30, 0xCE, 0x32, 0x54, 0x9F, 0x2C, 0xC4, 0xB3, 0x7F,
    0x5E, 0xF1, 0xCD, 0x7D, 0xA1, 0xEC, 0xC8, 0xEB, 0x8B, 0xB8, 0x0B, 0x8D, 0x7D, 0x24, 0xD7, 0xF6,
    0x3E, 0xAD, 0x58, 0x19, 0x7C, 0x47, 0x2A, 0xCD, 0x1E, 0x5B, 0x15, 0x6B, 0xAB, 0xD5, 0xC0, 0x6B,
    0x7A, 0xA1, 0x2A, 0x94, 0xCF, 0x43, 0x52, 0xC3, 0xE0, 0xC3, 0x64, 0x95, 0xC2, 0x90, 0xFF, 0xB5,
    0x27, 0x84, 0x4E, 0xA4, 0xBF, 0x18, 0x63, 0x37, 0xDD, 0x56, 0x32, 0x87, 0x32, 0x7B, 0xA2, 0x56,
    0x95, 0x69, 0x8E, 0xC4, 0xB8, 0x78, 0x51, 0x90, 0xBA, 0xC0, 0xE1, 0xFD, 0x3D, 0x12, 0xDD, 0xDB,
    0x7D, 0x5B, 0x6B, 0xCB, 0x0A, 0x06, 0xD5, 0x2A, 0x4C, 0x42, 0xBB, 0xFC, 0x5D, 0x0E, 0xC4, 0x55,
    0x48, 0xB4, 0xD1, 0xF0, 0x77, 0x8E, 0xBA, 0xC6, 0x1B, 0xE9, 0x2B, 0x9D, 0xD0, 0x4D, 0x2E, 0x64,
    0x7D, 0x32, 0xFE, 0xD9, 0x88, 0x68, 0x32, 0x21, 0x6D, 0x68, 0xFD, 0x8A, 0xE3, 0xCD, 0xCD, 0x0E,
    0xF7, 0xD4, 0xA8, 0x34, 0x68, 0x77, 0x0C, 0xB2, 0x8E, 0x91, 0x74, 0x33, 0x78, 0xF7, 0x70, 0x67,
    0x85, 0xD8, 0x47, 0x26, 0xBE, 0x79, 0xD5, 0x44, 0xFB, 0x1E, 0x25, 0x53, 0xBE, 0x64, 0x9B, 0xAC,
    0xE1, 0xF7, 0x61, 0x8F, 0x4F, 0xA5, 0xF5, 0x8A, 0x9E, 0x35, 0xFE, 0x69, 0x24, 0x8E, 0x65, 0x84,
    0x15, 0x9C, 0x7C, 0x6E, 0xBD, 0xD3, 0x91, 0xE9, 0x58, 0xF1, 0xF3, 0x64, 0x94, 0xB5, 0x0D, 0x5B,
    0x46, 0x21, 0x39, 0x89, 0x22, 0xD5, 0x61, 0xD4, 0xE2, 0xEB, 0x80, 0x2F, 0x80, 0xF8, 0xE7, 0x73,
    0x39, 0xE2, 0x67, 0x5E, 0x28, 0x84, 0x68, 0xC6, 0x02, 0xBD, 0xA8, 0xA6, 0x9D, 0xC1, 0xE1, 0xCF,
    0x69, 0xCF, 0xF9, 0x72, 0x10, 0x23, 0x3A, 0x11, 0xFE, 0xEA, 0x35, 0xB7, 0x58, 0x0A, 0x11, 0x32,
    0x06, 0x30, 0xE0, 0x57, 0xDE, 0xCD, 0x7C, 0xB4, 0xF9, 0xDF, 0x42, 0xB1, 0x06, 0x84, 0x4C, 0xE5,
    0x24, 0xD8, 0x21, 0x52, 0xBB, 0xA1, 0xE7, 0x68, 0xE6, 0x1B, 0xF5, 0xA2, 0x09, 0x67, 0xC5, 0xE3,
    0x75, 0x0F, 0x33, 0x45, 0x09, 0xC4, 0x57, 0xEC, 0x9A, 0x4A, 0xF0, 0x22, 0x95, 0x1E, 0xFA, 0x44,
    0x43, 0x91, 0x9F, 0xF8, 0x70, 0xA6, 0xD8, 0xD2, 0x51, 0x22, 0x68, 0x3E, 0x14, 0x6D, 0xFA, 0x23,
    0x41, 0x04, 0x13, 0x18, 0x77, 0x34, 0x76, 0xD9, 0x24, 0x5D, 0x0C, 0x64, 0x6C, 0xFC, 0xA8, 0x60,
    0x0F, 0xB4, 0xB4, 0xA5, 0xD8, 0xF3, 0x62, 0x78, 0x66, 0x04, 0xEF, 0x6D, 0xBC, 0x34, 0x12, 0x8B,
    0x5F, 0x60, 0x49, 0x12, 0xAD, 0xB6, 0x47, 0xFD, 0xFC, 0xD2, 0xF0, 0x39, 0x25, 0x44, 0x64, 0x64,
    0x30, 0x5C, 0xEE, 0x3D, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#endif
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#else
unsigned char hdcp_22_tx_key[445] = {
    //header 4byte
    0x00, 0x00, 0x00, 0x00,
    //lc128 16byte
    0xB5, 0xD8, 0xE9, 0xAB, 0x5F, 0x8A, 0xFE, 0xCA,
    0x38, 0x55, 0xB1, 0xA5, 0x1E, 0xC9, 0xBC, 0x0F,
    //sha1 20byte
    0x38, 0xED, 0x6E, 0xD4, 0xFE, 0x89, 0x63, 0x93, 0x61, 0x28,
    0x26, 0x58, 0x1F, 0x9D, 0x6E, 0x1A, 0xD5, 0x96, 0xC0, 0x8B,
    //N 384byte
    0xB0, 0xE9, 0xAA, 0x45, 0xF1, 0x29, 0xBA, 0x0A, 0x1C, 0xBE, 0x17, 0x57, 0x28, 0xEB, 0x2B, 0x4E,
    0x8F, 0xD0, 0xC0, 0x6A, 0xAD, 0x79, 0x98, 0x0F, 0x8D, 0x43, 0x8D, 0x47, 0x04, 0xB8, 0x2B, 0xF4,
    0x15, 0x21, 0x56, 0x19, 0x01, 0x40, 0x01, 0x3B, 0xD0, 0x91, 0x90, 0x62, 0x9E, 0x89, 0xC2, 0x27,
    0x8E, 0xCF, 0xB6, 0xDB, 0xCE, 0x3F, 0x72, 0x10, 0x50, 0x93, 0x8C, 0x23, 0x29, 0x83, 0x7B, 0x80,
    0x64, 0xA7, 0x59, 0xE8, 0x61, 0x67, 0x4C, 0xBC, 0xD8, 0x58, 0xB8, 0xF1, 0xD4, 0xF8, 0x2C, 0x37,
    0x98, 0x16, 0x26, 0x0E, 0x4E, 0xF9, 0x4E, 0xEE, 0x24, 0xDE, 0xCC, 0xD1, 0x4B, 0x4B, 0xC5, 0x06,
    0x7A, 0xFB, 0x49, 0x65, 0xE6, 0xC0, 0x00, 0x83, 0x48, 0x1E, 0x8E, 0x42, 0x2A, 0x53, 0xA0, 0xF5,
    0x37, 0x29, 0x2B, 0x5A, 0xF9, 0x73, 0xC5, 0x9A, 0xA1, 0xB5, 0xB5, 0x74, 0x7C, 0x06, 0xDC, 0x7B,
    0x7C, 0xDC, 0x6C, 0x6E, 0x82, 0x6B, 0x49, 0x88, 0xD4, 0x1B, 0x25, 0xE0, 0xEE, 0xD1, 0x79, 0xBD,
    0x39, 0x85, 0xFA, 0x4F, 0x25, 0xEC, 0x70, 0x19, 0x23, 0xC1, 0xB9, 0xA6, 0xD9, 0x7E, 0x3E, 0xDA,
    0x48, 0xA9, 0x58, 0xE3, 0x18, 0x14, 0x1E, 0x9F, 0x30, 0x7F, 0x4C, 0xA8, 0xAE, 0x53, 0x22, 0x66,
    0x2B, 0xBE, 0x24, 0xCB, 0x47, 0x66, 0xFC, 0x83, 0xCF, 0x5C, 0x2D, 0x1E, 0x3A, 0xAB, 0xAB, 0x06,
    0xBE, 0x05, 0xAA, 0x1A, 0x9B, 0x2D, 0xB7, 0xA6, 0x54, 0xF3, 0x63, 0x2B, 0x97, 0xBF, 0x93, 0xBE,
    0xC1, 0xAF, 0x21, 0x39, 0x49, 0x0C, 0xE9, 0x31, 0x90, 0xCC, 0xC2, 0xBB, 0x3C, 0x02, 0xC4, 0xE2,
    0xBD, 0xBD, 0x2F, 0x84, 0x63, 0x9B, 0xD2, 0xDD, 0x78, 0x3E, 0x90, 0xC6, 0xC5, 0xAC, 0x16, 0x77,
    0x2E, 0x69, 0x6C, 0x77, 0xFD, 0xED, 0x8A, 0x4D, 0x6A, 0x8C, 0xA3, 0xA9, 0x25, 0x6C, 0x21, 0xFD,
    0xB2, 0x94, 0x0C, 0x84, 0xAA, 0x07, 0x29, 0x26, 0x46, 0xF7, 0x9B, 0x3A, 0x19, 0x87, 0xE0, 0x9F,
    0xEB, 0x30, 0xA8, 0xF5, 0x64, 0xEB, 0x07, 0xF1, 0xE9, 0xDB, 0xF9, 0xAF, 0x2C, 0x8B, 0x69, 0x7E,
    0x2E, 0x67, 0x39, 0x3F, 0xF3, 0xA6, 0xE5, 0xCD, 0xDA, 0x24, 0x9B, 0xA2, 0x78, 0x72, 0xF0, 0xA2,
    0x27, 0xC3, 0xE0, 0x25, 0xB4, 0xA1, 0x04, 0x6A, 0x59, 0x80, 0x27, 0xB5, 0xDA, 0xB4, 0xB4, 0x53,
    0x97, 0x3B, 0x28, 0x99, 0xAC, 0xF4, 0x96, 0x27, 0x0F, 0x7F, 0x30, 0x0C, 0x4A, 0xAF, 0xCB, 0x9E,
    0xD8, 0x71, 0x28, 0x24, 0x3E, 0xBC, 0x35, 0x15, 0xBE, 0x13, 0xEB, 0xAF, 0x43, 0x01, 0xBD, 0x61,
    0x24, 0x54, 0x34, 0x9F, 0x73, 0x3E, 0xB5, 0x10, 0x9F, 0xC9, 0xFC, 0x80, 0xE8, 0x4D, 0xE3, 0x32,
    0x96, 0x8F, 0x88, 0x10, 0x23, 0x25, 0xF3, 0xD3, 0x3E, 0x6E, 0x6D, 0xBB, 0xDC, 0x29, 0x66, 0xEB,
    //E
    0x03,
    //SHA1 20byte
    0x16, 0x57, 0x3E, 0xD7, 0xD3, 0x37, 0x97, 0xF2, 0x8F, 0x07,
    0xBD, 0xB4, 0xEA, 0x74, 0x7F, 0x94, 0x99, 0x18, 0x9F, 0x20,
};
#endif
#else
//const code __at(0x1DFC00) unsigned char hdcp_22_tx_key[880];  //bank29
unsigned char hdcp_22_tx_key[445]={0};
#endif

#define LOCK_HDCP2_TX()   mutex_lock(&hdcp2_tx_lock)
#define UNLOCK_HDCP2_TX() mutex_unlock(&hdcp2_tx_lock)
static DEFINE_MUTEX(hdcp2_tx_lock);

//****************************************************************************
// CODE TABLES
//****************************************************************************
extern unsigned char hdcp14_wait_v_status; //for hdcp14 cts workaround
//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
unsigned short hdcp2_auth_cnt[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char hdcp_auth_timeout_stat[HDMI_TX_PORT_MAX_NUM] = {0};
volatile StructHdcp2TxAuthInfo g_stHdmiHdcp2Tx2AuthInfo[HDMI_TX_PORT_MAX_NUM];
volatile unsigned char g_ucHdcp2TxErrorStatus;
unsigned char g_enumHdmiHdcp2Tx2Status = 0;//_HDCP_2_2_TX_NONE;
unsigned char tx_key_align_offset = 0;
unsigned char g_bHdmiHdcp2Tx2StableCheck;
unsigned char g_bHdmiHdcp2StateVerifySignatureTimeStatus[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char g_bHdmiHdcp2StateNoStoredKmTimestatus[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char g_bHdmiHdcp2StateStoredKmTimestatus[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char g_bHdmiHdcp2StateParingTimestatus[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char g_bHdmiHdcp2StateSkeTimestatus[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char g_bHdmiHdcp2StateLCTimestatus[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned char g_bHdmiHdcp2StateEncTimestatus[HDMI_TX_PORT_MAX_NUM] = {0};

unsigned char g_ucLogPrint[HDMI_TX_PORT_MAX_NUM] = {0};
extern unsigned char pucMessageTx[HDMI_TX_PORT_MAX_NUM][150];
extern unsigned short repeater_temp[2];
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
unsigned char tx_subAddr[256];
unsigned char g_hdcp_addr[4] = {0x3A, 0x3B, 0x3C, 0x3D};       //0x3A//(0x74 >> 1)
#endif
unsigned short rx_phy_not_stable_cnt[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long  start_time[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long  polling_time[HDMI_TX_PORT_MAX_NUM]  = {0};
unsigned long  hdcptx_current_time[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long  vprime_time_ms[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long  enc_time[HDMI_TX_PORT_MAX_NUM] = {0};
unsigned long  diff_ms = 0;
volatile unsigned char ake_init_failed_count = 0;
volatile unsigned char verify_sig_failed_count = 0;
volatile unsigned char no_storedkm_failed_count = 0;
volatile unsigned char storedkm_failed_count = 0;
volatile unsigned char verify_hprime_failed_count = 0;
volatile unsigned char pairing_failed_count = 0;
volatile unsigned char lc_check_failed_count = 0;
volatile unsigned char rtt_chanllege_failed_count = 0;
volatile unsigned char ske_failed_count = 0;
unsigned char pairing_ret = 0;
unsigned char sink_only_support_hdcp1[HDMI_TX_PORT_MAX_NUM] = {0};
HDMI_HDCPTX_E hdcp_tx_mode = NO_HDCP;
unsigned char hdcp_tx_running = 0;

unsigned char hdcptx_en_test = 0;
unsigned char hdmi_hdcptx_timer_eanble = 0;

void Hdcp2TxIdleProcess(unsigned char port);
void Hdcp2TxAkeInitProcess(unsigned char port);
void Hdcp2TxVerifySignatureProcess(unsigned char port);
void Hdcp2TxNoStoredKmProcess(unsigned char port);
void Hdcp2TxStoredKmProcess(unsigned char port);
void Hdcp2TxVerifyHprimeProcess(unsigned char port);
void Hdcp2TxPairingProcess(unsigned char port);
void Hdcp2TxLocalityCheckProcess(unsigned char port);
void Hdcp2TxSkeProcess(unsigned char port);
void Hdcp2TxVerifyVprimeProcess(unsigned char port);
void Hdcp2TxAuthDoneProcess(unsigned char port);
void Hdcp2TxLcRetryProcess(unsigned char port);


static void hdcp2_get_hdcptx_current_time(unsigned long *p_time)
{
    *p_time = rtk_timer_misc_90k_ms();
}


#ifdef CONFIG_ENABLE_HDCP2_TX_ON_SINGLE_DDC

INT8 hdcp22_ddc_bus_write(unsigned char port, unsigned char *wr_buff, unsigned short wr_len)
{
    if (wr_buff[0] == 0x60) { // write message
        wr_buff[1] |= (port << 6); // add port id in the msb of msg id
    }
    return i2c_master_send_ex(HDCP2_TX_I2C_BUSID, g_hdcp_addr[0], wr_buff, wr_len);
}

INT8 hdcp22_ddc_bus_read(unsigned char port, unsigned char sub_addr, unsigned char *p_read_buff, unsigned short read_len)
{
    INT8 ret;

    switch (sub_addr) {
        case 0x50:
            sub_addr = 0x81 + (port << 2); // direct to EDID SRAM
            return i2c_master_recv_ex(HDCP2_TX_I2C_BUSID, 0x50, &sub_addr, 1, p_read_buff, read_len);
        case 0x70:
            sub_addr = 0x82 + (port << 2); // direct to EDID SRAM
            return i2c_master_recv_ex(HDCP2_TX_I2C_BUSID, 0x50, &sub_addr, 1, p_read_buff, read_len);
        case 0x71:
            sub_addr = 0x83 + (port << 2); // direct to EDID SRAM
            return i2c_master_recv_ex(HDCP2_TX_I2C_BUSID, 0x50, &sub_addr, 1, p_read_buff, read_len);
    }

    ret = i2c_master_recv_ex(HDCP2_TX_I2C_BUSID, g_hdcp_addr[0], &sub_addr, 1, p_read_buff, read_len);

    if (sub_addr == 0x80) {
        p_read_buff[0] &= 0x3F;  // remove chip id from msg id
    }

    return ret;
}
#else

#define hdcp22_ddc_bus_write(port, write_buff, write_len)           i2c_master_send_ex(HDCP_TX_I2C_BUSID, g_hdcp_addr[port], write_buff, write_len)
#define hdcp22_ddc_bus_read(port, sub_addr, p_read_buff, read_len)  i2c_master_recv_ex(HDCP_TX_I2C_BUSID, g_hdcp_addr[port], &sub_addr, 1, p_read_buff, read_len)

#endif

void ScalerHdmiHdcp2TxReset(unsigned char port)
{
    HDCPTX_WARIN("ScalerHdmiHdcp2TxReset\n");

    CLR_HDMI_HDCP2_TX2_AUTH_GO(port);
    CLR_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port);
    CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
    CLR_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_H_PRIME_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_POLLIN_HPRIME_ENABLE(port);
    CLR_HDMI_HDCP2_TX2_PARING_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port);
    CLR_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port);
    CLR_HDMI_HDCP2_TX_STATE_ENC_TIME_STATUS(port);
    CLR_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_V_READY_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_POLLING_V_READY(port);
    CLR_HDMI_HDCP2_TX2_V_READY_BIT(port);
    CLR_HDMI_HDCP2_TX2_SEQUENCE_NUMBER_M(port);
    CLR_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_CONTENT_STREAM_MANAGEMENT(port);
    CLR_HDMI_HDCP2_TX2_POLLING_FAIL(port);
    CLR_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port);
    CLR_HDMI_HDCP2_TX2_RXINFO_HDCP20_REPEATER_DOWNSTREAM(port);
    CLR_HDMI_HDCP2_TX2_WRITE_M_MSG_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX_STATE_VERIFY_SIGNATURE_TIME_STATUS(port);
    CLR_HDMI_HDCP2_TX_STATE_STATE_STORED_KM_TIME_STATUS(port);
    CLR_HDMI_HDCP2_TX_STATE_NO_STORED_KM_TIME_STATUS(port);
    CLR_HDMI_HDCP2_TX_STATE_STATE_PAIRING_TIME_STATUS(port);
    CLR_HDMI_HDCP2_TX_STATE_LOCALITY_CHECK_TIME_STATUS(port);
}

void ScalerHdmiHdcp2TxSetAuthTimeOut(unsigned char port)
{
    hdcp_auth_timeout_stat[port] = 1;
}
EXPORT_SYMBOL(ScalerHdmiHdcp2TxSetAuthTimeOut);

unsigned char ScalerHdmiHdcp2TxGetAuthTimeOut(unsigned char port)
{
    return hdcp_auth_timeout_stat[port];
}
EXPORT_SYMBOL(ScalerHdmiHdcp2TxGetAuthTimeOut);

void ScalerHdmiHdcp2TxSetAuthStateIdle(unsigned char port)
{
    hdcp_auth_timeout_stat[port] = 0;
    //sink_only_support_hdcp1[port] == 0;
    hdcp14_wait_v_status = 0;
    hdcp_tx_mode = NO_HDCP;
    CLR_HDMI_hdcp2_auth_cnt_STAT(port);
    SET_HDMI_HDCP2_TX2_AUTH_STATE(port, _HDCP_2_2_TX_STATE_IDLE);
    CLR_HDMI_hdcp14_auth_cnt_STAT(port);
    SET_HDMI_HDCP14_TX2_AUTH_STATE(port, _HDMI_TX_HDCP_STATE_IDLE);
    //HDCPTX_WARIN( "[HDCPTX]set auth statu to idle\n");
}
EXPORT_SYMBOL(ScalerHdmiHdcp2TxSetAuthStateIdle);

void ScalerHdmiHdcp2TxClearAuthState(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) == _HDCP_2_2_TX_STATE_AUTHEN_DONE) {
        //   core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_AUTHEN_DONE << 8 | port);
    }
#ifdef CONFIG_ENABLE_HDCP14_TX
    // core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _SCALER_TIMER_EVENT_HDMI_TX0_HDCP_VERIFY_RI << 8 | port);
#endif
    if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _TRUE) {
        ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
        HDCPTX_WARIN("TX enc enable, Disable Encrypt\n");
    }

    ScalerHdmiHdcp2TxSetAuthStateIdle(port);
    HDCPTX_WARIN("clear auth status\n");
}
EXPORT_SYMBOL(ScalerHdmiHdcp2TxClearAuthState);

//--------------------------------------------------
// Description  : Change HDCP2 Auth State
// Input Value  : Target HDCP2 Auth State
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp2Tx2ChangeAuthState(unsigned char port, unsigned char enumHdcp2AuthState)
{
    SET_HDMI_HDCP2_TX2_AUTH_STATE(port, enumHdcp2AuthState);
    SET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
}
EXPORT_SYMBOL(ScalerHdmiHdcp2Tx2ChangeAuthState);


//--------------------------------------------------
// Description  : Polling Rx status of Downstream
// Input Value  :
// Output Value :
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2PollingStatus(unsigned char port)
{
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    /*
        if (ScalerHdmiMacTx2GetHpdStatus() == _LOW) {
            return _HDCP_2_2_TX_NONE;
        }
    */
    unsigned char tx_data;
    tx_subAddr[0] = 0x71;

    if (hdcp2tx_func_test) {
        if (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) != _HDCP_2_2_TX_STATE_AUTHEN_DONE) {
            return _HDCP_2_2_TX_V_READY;
        }
        else {
            return _HDCP_2_2_TX_NONE;
        }
    }
#if IS_ENABLED(CONFIG_I2C)
    if (hdcp22_ddc_bus_read(port, tx_subAddr[0], &tx_data, 0x01) < 0) {
        HDCPTX_WARIN("tx read I2C 0x71 fail\n");
        SET_HDMI_HDCP2_TX2_POLLING_FAIL(port);
        return _HDCP_2_2_TX_NONE;
    }
#endif


    if (GET_HDMI_HDCP2_TX2_POLLING_FAIL(port) == _TRUE) {
        CLR_HDMI_HDCP2_TX2_POLLING_FAIL(port);

        return _HDCP_2_2_TX_REAUTH_REQUEST;
    }
    // HDCPTX_WARIN("RealTX: RXStatus %d\n", tx_data);

    if ((tx_data & _BIT2) == _BIT2) {
        return _HDCP_2_2_TX_V_READY;
    }
    else if ((tx_data & _BIT3) == _BIT3) {
        return _HDCP_2_2_TX_REAUTH_REQUEST;
    }
#endif
    return 0;//_HDCP_2_2_TX_NONE;
}

//--------------------------------------------------
// Description  : Authentication Key Exchange Initial(AKE_init)
// Input Value  : None
// Output Value : _TRUE (HDCP ACK)
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2CheckCapability(unsigned char port)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
    unsigned char tx_data = 0;
#endif

    if (hdcp2tx_func_test) {
        CLR_RX_PHY_NOT_STABLE_CNT(port);// rx phy already stable & already enable hdcp22
        SET_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port);
        SET_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port);
        return _TRUE;
    }

    CLR_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port);
    CLR_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port);
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    // Check if Receiver support HDCP capable and version
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
    tx_subAddr[0] = 0x50;
    if (hdcp22_ddc_bus_read(port, tx_subAddr[0], &tx_data, 0x01) >= 0) {
        if (tx_data == 0x04) {
            sink_only_support_hdcp1[port] = 0;
            //HDCPTX_WARIN( "HDCP2TX: support hdcp2\n");
            CLR_RX_PHY_NOT_STABLE_CNT(port);// rx phy already stable & already enable hdcp22
            SET_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port);
            SET_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port);
            return _TRUE;
        }
        sink_only_support_hdcp1[port] = 1;
        ADD_RX_PHY_NOT_STABLE_CNT(port);// rx phy not stable
    }
    else {
        HDCPTX_WARIN("i2c read fail\n");
        ADD_RX_PHY_NOT_STABLE_CNT(port);// rx phy not stable & ddc not work
        return _FALSE;
    }
#endif
#endif
    return _FALSE;
}
EXPORT_SYMBOL(ScalerHdmiHdcp2Tx2CheckCapability);

#if 0
//--------------------------------------------------
// Description  : Set Timeout to Read Certificate
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp2Tx2TimeoutToReadCertificateEvent(unsigned char port)
{
    SET_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port);
}


//--------------------------------------------------
// Description  : Set Timeout to Encrypt Data
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp2Tx2TimeoutToEncyptDataEvent(unsigned char port)
{
    SET_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port);
}

//--------------------------------------------------
// Description  : Waiting for V ready
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp2Tx2TimeoutForVReadyEvent(unsigned char port)
{
    SET_HDMI_HDCP2_TX2_V_READY_TIMEOUT(port);
}

//--------------------------------------------------
// Description  : Polling V Ready bit
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp2Tx2PollingVReadyEvent(unsigned char port)
{
    SET_HDMI_HDCP2_TX2_POLLING_V_READY(port);
}




//--------------------------------------------------
// Description  : Set Timeout to Read M'
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp2Tx2TimeoutToReadMPrimeEvent(unsigned char port)
{
    SET_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port);
}
#endif
HdmiTxModeStatus GET_HDMI_MAC_TX_MODE_STATE(unsigned char port)
{
    if (hdcp2tx_func_test) {
        return _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON;
    }
    if (hdmi_in(HDMITX_MISC_HDMITX_MISC_DBG_CTRL_reg) & HDMITX_MISC_HDMITX_MISC_DBG_CTRL_phy_data_src_sel_mask) {
        return _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON;
    }
    else {
        return _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON;
    }
}

unsigned char ScalerHdmiMacTx2GetHdcpEncStatus(unsigned char port)
{
    if (hdcp2tx_func_test) {
        return hdcptx_en_test;
    }

    if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON) {
        if (hdmi_in(HDMITX20_MAC0_SCHEDULER_2_reg) & HDMITX20_MAC0_SCHEDULER_2_hdcp_enc_en_mask) {
            return _TRUE;
        }
    }
    else if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) {
        if (hdmi_in(HDMITX21_MAC_HDMI21_SCHEDULER_3_reg) & HDMITX21_MAC_HDMI21_SCHEDULER_3_hdcp22_enc_en_mask) {
            return _TRUE;
        }
    }
    return _FALSE;
}

void ScalerHdmiMacTx2HdcpEncryptSignalProc(unsigned char port, unsigned char onOff)
{
    if (hdcp2tx_func_test) {
        hdcptx_en_test = onOff;
        return;
    }

    if (onOff) {
        if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON) {
            hdmi_mask(HDMITX20_MAC0_SCHEDULER_2_reg, ~HDMITX20_MAC0_SCHEDULER_2_hdcp_enc_en_mask, HDMITX20_MAC0_SCHEDULER_2_hdcp_enc_en_mask);
        }
        else if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) {
            hdmi_mask(HDMITX21_MAC_HDMI21_SCHEDULER_3_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_3_hdcp22_enc_en_mask, HDMITX21_MAC_HDMI21_SCHEDULER_3_hdcp22_enc_en_mask);
        }
    }
    else {
        hdmi_mask(HDMITX20_MAC0_SCHEDULER_2_reg, ~HDMITX20_MAC0_SCHEDULER_2_hdcp_enc_en_mask, 0);
        hdmi_mask(HDMITX21_MAC_HDMI21_SCHEDULER_3_reg, ~HDMITX21_MAC_HDMI21_SCHEDULER_3_hdcp22_enc_en_mask, 0);
    }
}

//--------------------------------------------------
// Description  : HDCP 2.2 Initial Process
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdcp2TxInitial(unsigned char port)
{
#ifdef HDCP_HW_SPEED_UP
    hdmi_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), _BIT3);
#endif
}

//--------------------------------------------------
// Description  : Send & Backup Paring Info
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2StorePairingInfo(unsigned char port)
{
    //unsigned short usReadLength;
    unsigned char temp[16];

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    // Read Ekn[km]
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    //usReadLength = 17;

    if (ScalerHdmiHdcp2Tx2ReadMessage(port, 17, pucMessageTx[port], 200) == _FALSE) {
        return HDCP2_TX_READ_RX_TIMEOUT;
    }
#endif

    if (hdcp2tx_func_test) {
        memcpy(pucMessageTx[port], pucMessage_AKE_SEND_PAIRING_INFO, 17);
    }

    //if (pucMessageTx[port][0] != 0x08) {
    if (pucMessageTx[port][0] != 0x08) {   //for reduce dseg
        return _FALSE;
    }

    // Store Ekhkm Km. m, Receiver ID
    memcpy(g_pucHdmiHdcp2Tx2PairingEkhkm[port], &pucMessageTx[port][1], 16);
    memcpy(g_pucHdmiHdcp2Tx2PairingKm[port], g_pucHdmiHdcp2Tx2Km[port], 16);

    // to reduce dseg used
    memcpy(temp, g_pucHdmiHdcp2Tx2Rtx[port], 8);
    memcpy(&temp[8], g_pucHdmiHdcp2Tx2Rrx[port], 8);
    memcpy(g_pucHdmiHdcp2Tx2PairingM[port], temp, 16);
    //memcpy(g_pucHdmiHdcp2Tx2PairingM[port], g_pucHdmiHdcp2Tx2Rtx[port], 8);
    //memcpy(&g_pucHdmiHdcp2Tx2PairingM[port][8], g_pucHdmiHdcp2Tx2Rrx[port], 8);
    memcpy(g_pucHdmiHdcp2Tx2PairingReceiverID[port], g_pucHdmiHdcp2Tx2ReceiverId[port], 5);


    if (hdcp2tx_cal_log_print) {
        unsigned char i;
        HDCPTX_WARIN("EKHKM:\n");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x,", (unsigned int)g_pucHdmiHdcp2Tx2PairingEkhkm[port][i]);
        }
        HDCPTX_WARIN("\nKM:\n");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x,", (unsigned int)g_pucHdmiHdcp2Tx2Km[port][i]);
        }
        HDCPTX_WARIN("\nM:\n");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x,", (unsigned int)g_pucHdmiHdcp2Tx2PairingM[port][i]);
        }
        HDCPTX_WARIN("\nReceiverID:\n");
        for (i = 0; i < 5; i++) {
            HDCPTX_WARIN("%x,", (unsigned int)g_pucHdmiHdcp2Tx2ReceiverId[port][i]);
        }
        HDCPTX_WARIN("\n");
    }

    SET_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port);

    return _TRUE;
}

//--------------------------------------------------
// Description  : H Verification
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2VerifyHprime(unsigned char port)
{
    volatile unsigned char ucI = 0;
    unsigned char pucHprime[33] = {0};  // read from rx
    unsigned char usReadLength = 0;
    unsigned char pucTemp[32] = {0};
    unsigned short timeout = 0;

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    memcpy(pucTemp, g_pucHdmiHdcp2Tx2Hdcp2H[port], 32);

    if (GET_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port) == _TRUE) {
        timeout = 200;
    }
    else {
        timeout = 1000;
    }

    // Read H' and Compare H' with H
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    usReadLength = 33;
    if (ScalerHdmiHdcp2Tx2ReadMessage(port, usReadLength, pucHprime, timeout) == _FALSE) {
        return _FALSE;
    }
#endif
    if (hdcp2tx_func_test) {
        memcpy(pucHprime, pucMessage_AKE_SEND_H_PRIME, 33);
    }

    if (pucHprime[0] != 0x07) {
        HDCPTX_WARIN("H' FAIL 0\n");
        return _FALSE;
    }

    for (ucI = 0; ucI < 32; ucI++) {
        if (pucTemp[ucI] != pucHprime[1 + ucI]) {
            HDCPTX_WARIN("H' FAIL 1\n");
            return _FALSE;
        }
    }

    HDCPTX_WARIN("H' PASS\n");

    return _TRUE;
}

//--------------------------------------------------
// Description  : Check Pairing Info
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2CheckPairingInfo(unsigned char port)
{
    unsigned char ucI = 0;

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    if (GET_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port) == _FALSE) {
        return _FALSE;
    }

    for (ucI = 0; ucI < 5; ucI++) {
        if (g_pucHdmiHdcp2Tx2PairingReceiverID[port][ucI] != g_pucHdmiHdcp2Tx2ReceiverId[port][ucI]) {
            memset(g_pucHdmiHdcp2Tx2PairingKm[port], 0, sizeof(g_pucHdmiHdcp2Tx2PairingKm[port]));
            memset(g_pucHdmiHdcp2Tx2PairingM[port], 0, sizeof(g_pucHdmiHdcp2Tx2PairingM[port]));
            memset(g_pucHdmiHdcp2Tx2PairingReceiverID[port], 0, sizeof(g_pucHdmiHdcp2Tx2PairingReceiverID[port]));

            CLR_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port);

            return _FALSE;
        }
    }

    return _TRUE;
}
//--------------------------------------------------
// Description  : M Verification
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------

unsigned char ScalerHdmiHdcp2Tx2VerifyMprime(unsigned char port)
{
    unsigned char ucI = 0;
    // Read M' and Compare M' with M
    // HDCPTX_WARIN("RealTX: In M'");
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    if (ScalerHdmiHdcp2Tx2ReadMessage(port, 33, pucMessageTx[port], 80) == _FALSE) {
        return _FALSE;
    }
#endif

    if (hdcp2tx_func_test) {
        memcpy(pucMessageTx[port], pucMessage_REPEATERAUTH_STREAM_READY, 33);
    }

    if (pucMessageTx[port][0] != 0x11) {
        return _FALSE;
    }

    for (ucI = 0; ucI < 32; ucI++) {
        if (g_pucHdmiHdcp2Tx2M[ucI] != pucMessageTx[port][1 + ucI]) {
            HDCPTX_WARIN("M != M'\n");
            return _FALSE;
        }
    }

    HDCPTX_WARIN("M Pass\n");

    return _TRUE;
}

#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
unsigned char ScalerHdmiHdcp2Tx2ReadMessage(unsigned char port, unsigned short usLength, unsigned char *pucReadArray, unsigned short timeout)
{
    unsigned short usReadLength  = {0};
    unsigned short usReadPtr  = {0};
    unsigned char pDate_tx[2];
    unsigned short read_timeout;
    //unsigned char timeout_flag  = {0};
    unsigned short usRetryCount = 0;
    unsigned char i2c_retry_count = 0;

    pDate_tx [0] = 0;
    pDate_tx [1] = 0;

    read_timeout = timeout;

    if (hdcp2tx_func_test) {
        HDCPTX_WARIN("functest can't i2c w/r\n");
        return _TRUE;
    }

    while (usLength > usReadPtr) {
        tx_subAddr[0] = 0x70;
#if IS_ENABLED(CONFIG_I2C)
        while (hdcp22_ddc_bus_read(port, tx_subAddr[0], pDate_tx, 0x02) < 0) {
            i2c_retry_count++;
            if (i2c_retry_count > 20) {
                i2c_retry_count = 0;
                HDCPTX_WARIN("read I2C 0x70 fail\n");
                return _FALSE;
            }
        }
#endif
        pDate_tx[1] = pDate_tx[1] & 0x03;
        usReadLength  = (pDate_tx[1] << 8) + pDate_tx[0];

        //delayms(2);
        if (usReadLength  != 0) {
            tx_subAddr[0] = 0x80;

#if IS_ENABLED(CONFIG_I2C)
            if (hdcp22_ddc_bus_read(port, tx_subAddr[0], pucReadArray, usReadLength) < 0) {
                HDCPTX_WARIN("read I2C 0x80 fail\n");
                return _FALSE;
            }
#endif
            usReadPtr  += usReadLength ;
        }

        else {
            usRetryCount++;
        }
        if (usRetryCount >= read_timeout) {
            if (usReadPtr == 0) {
                HDCPTX_WARIN("Read Rx message timeout!\n");
                return _FALSE;
            }
            else {
                HDCPTX_WARIN("length = %d\n", (unsigned int)usReadPtr);
                return _TRUE;
            }
        }
#if 0
        if ((hdmi_hdcp2_misc_ms() - start_time) > timeout) {
            timeout_flag  = 1;
            break;
        }
#endif
    }
#if 0

    if (HDCP22_TX_DBG_EN) {
        unsigned short j;
        for (j = 0; j < usReadLength ; j++) {
            if (j % 8 == 0) {
                HDCPTX_WARIN("\n");
            }
            HDCPTX_WARIN("%d ", (unsigned int)pucReadArray[j]);
        }
        HDCPTX_WARIN("\n");
    }

    if (timeout_flag  == 1) {
        HDCPTX_WARIN("Read Rx message timeout [%d - %d > %d]\n", (unsigned int)hdmi_hdcp2_misc_ms(), (unsigned int)start_time, (unsigned int)timeout);
        return _FALSE;
    }
#endif
    return _TRUE;
}

unsigned char ScalerHdmiHdcp2Tx2ReadData(unsigned char port, unsigned short usLength, unsigned char *pucReadArray, unsigned int timeout)
{
    if (hdcp2tx_func_test) {
        HDCPTX_WARIN("functest can't i2c w/r\n");
        return _TRUE;
    }

#if IS_ENABLED(CONFIG_I2C)
    tx_subAddr[0] = 0x80;
    if (hdcp22_ddc_bus_read(port, tx_subAddr[0], pucReadArray, usLength) < 0) {
        HDCPTX_WARIN("read I2C 0x80 fail\n");
        return _FALSE;
    }
    if (pucReadArray[0] != 0x0A) {
        return _FALSE;
    }
#endif
    return _TRUE;
}

unsigned char ScalerHdmiHdcp2Tx2WriteMessage(unsigned char port, unsigned short usLength, unsigned char *pucReadArray)
{
    if (hdcp2tx_func_test) {
        HDCPTX_WARIN("functest can't i2c w/r\n");
        return _TRUE;
    }
    tx_subAddr[0] = 0x60;
    memcpy(&tx_subAddr[1], pucReadArray, usLength);
#if IS_ENABLED(CONFIG_I2C)
    if (hdcp22_ddc_bus_write(port, tx_subAddr, usLength + 1) < 0) {
        HDCPTX_WARIN("write I2C 0x70 fail\n");
        return _FALSE;
    }
#endif
    //HDCPTX_WARIN( "w msg ok\n");
    return _TRUE;
}
#endif

void Hdcp2TxIdleProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("Idle\n");
    }
    ScalerHdmiHdcp2TxReset(port);
    // Disable Encrypt Data
    ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
    if (g_ucLogPrint[port] == 0) {
        HDCPTX_WARIN("Disable Encrypt0\n");
    }
    ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_AKE_INIT);

    //cancel timer evet
    start_time[port] = 0;
    polling_time[port]  = 0;
    hdcptx_current_time[port] = 0;
    vprime_time_ms[port] = 0;
    enc_time[port] = 0;
}

void Hdcp2TxAkeInitProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        if (ScalerHdmiHdcp2Tx2CheckCapability(port) == _FALSE) {
            HDCPTX_WARIN("sink not ready\n");
            g_ucLogPrint[port]++;
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
            return;
        }
        g_ucLogPrint[port] = 0;
        HDCPTX_WARIN("AKE INIT\n");
        // Read Capability AGAIN for CTS
        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
        SET_HDMI_HDCP2_TX2_AUTH_GO(port);
    }

    if (GET_HDMI_HDCP2_TX2_AUTH_GO(port) == _TRUE) {
        if (ScalerHdmiHdcp2Tx2AkeInitial(port) == _TRUE) {
            ake_init_failed_count = 0;
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_VERIFY_SIGNATURE);
        }
        else {
            SET_HDMI_HDCP2_TX2_AUTH_START(port);

            HDCPTX_WARIN("AKE_Init Fail !\n");
            ake_init_failed_count++;
            if (ake_init_failed_count > 5) {
                ake_init_failed_count = 0;
            }
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
        }
    }
}

void Hdcp2TxVerifySignatureProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("Verify S\n");

        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
        SET_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port);
    }

    if (GET_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port) == _TRUE) {
        CLR_HDMI_HDCP2_TX2_CERTIFICATE_TIMEOUT(port);
    }

    hdcp2_get_hdcptx_current_time(&start_time[port]);
    if (ScalerHdmiHdcp2Tx2VerifySignature(port) == _TRUE) {
        diff_ms = hdcptx_timer_diff_90k_ms_counter(start_time[port]);
#if 1
        if (diff_ms > VERIFY_SIGNATURE_ON_THE_CERT_TIMEOUT) {
            SET_HDMI_HDCP2_TX_STATE_VERIFY_SIGNATURE_TIME_STATUS(port);
        }
#endif
        if (GET_HDMI_HDCP2_TX_STATE_VERIFY_SIGNATURE_TIME_STATUS(port) == _TIME_OUT) {
            CLR_HDMI_HDCP2_TX_STATE_VERIFY_SIGNATURE_TIME_STATUS(port);
            HDCPTX_WARIN("cert and receiver info timeout:%lu\n", diff_ms);
            verify_sig_failed_count++;
            if (verify_sig_failed_count > 5) {
                verify_sig_failed_count = 0;
                //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_CERT_AND_RECEIVER_INFO_TIMEOUT);
            }

            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
            return;
        }
        //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_VERIFY_SIGNATURE << 8 | port);
        // Go to No stored km or Stored km
        if (ScalerHdmiHdcp2Tx2CheckPairingInfo(port) == _TRUE) {
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_STORED_KM);
        }
        else {
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_NO_STORED_KM);
        }
        verify_sig_failed_count = 0;
    }
    else {
        //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
        SET_HDMI_HDCP2_TX2_AUTH_START(port);

        HDCPTX_WARIN("Signature Fail\n");
        verify_sig_failed_count++;
        if (verify_sig_failed_count > 5) {
            verify_sig_failed_count = 0;
            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_SIGNATURE_FAIL);
        }

        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }
}

void Hdcp2TxNoStoredKmProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("No Stored Km\n");

        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
    }

    if (ScalerHdmiHdcp2Tx2NoStoredKmProc(port) == _TRUE) {
        hdcp2_get_hdcptx_current_time(&start_time[port]);
        //core_timer_event_addTimerEvent(VERIFY_INTEGRITY_OF_SRM_NO_STORED_TIMEOUT, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_NO_STORED_KM << 8 | port, 0);
        no_storedkm_failed_count = 0;
        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_VERIFY_H_PRIME);

        //delayms(1000);
        //set 1s timer,need added by zhaodong
        //ScalerTimerActiveTimerEvent(SEC(1.5), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_LIMIT_OF_READ_H_PRIME);
    }
    else {
        //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
        SET_HDMI_HDCP2_TX2_AUTH_START(port);

        HDCPTX_WARIN("NoStoreKM Fail\n");
        no_storedkm_failed_count++;
        if (no_storedkm_failed_count > 5) {
            no_storedkm_failed_count = 0;
            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_NO_STOREKM_FAIL);
        }

        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }
}

void Hdcp2TxStoredKmProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("Stored Km\n");

        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
    }

    if (ScalerHdmiHdcp2Tx2StoredKmProc(port) == _TRUE) {
        hdcp2_get_hdcptx_current_time(&start_time[port]);
        //core_timer_event_addTimerEvent(VERIFY_INTEGRITY_OF_SRM_STORED_TIMEOUT, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_STORED_KM << 8 | port, 0);
        storedkm_failed_count = 0;
        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_VERIFY_H_PRIME);

        //delayms(200);
        //set 200ms timer,need added by zhaodong
        //ScalerTimerActiveTimerEvent(SEC(0.3), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_LIMIT_OF_READ_H_PRIME);
    }
    else {
        SET_HDMI_HDCP2_TX2_AUTH_START(port);

        HDCPTX_WARIN("StoreKM Fail\n");
        storedkm_failed_count++;
        if (storedkm_failed_count > 5) {
            storedkm_failed_count = 0;
            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_STOREKM_FAIL);
        }

        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }
}

void Hdcp2TxVerifyHprimeProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("H Prime\n");

        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
        //delayms(100);
        SET_HDMI_HDCP2_TX2_H_PRIME_TIMEOUT(port);
    }
    if (GET_HDMI_HDCP2_TX2_H_PRIME_TIMEOUT(port) == _TRUE) {
        // Compute H for Stored Km Flow
        if (ScalerHdmiHdcp2Tx2ComputeH(port) == _TRUE) {

            if (ScalerHdmiHdcp2Tx2VerifyHprime(port) == _TRUE) {
                diff_ms = hdcptx_timer_diff_90k_ms_counter(start_time[port]);
                if (GET_HDMI_HDCP2_TX2_PAIRING_INFO_READY(port) == _TRUE) {
#if 1
                    if (diff_ms >  VERIFY_INTEGRITY_OF_SRM_STORED_TIMEOUT) {
                        SET_HDMI_HDCP2_TX_STATE_STATE_STORED_KM_TIME_STATUS(port);
                        HDCPTX_WARIN("[HDC22TX]STORED_KM timeout =%d\n", diff_ms);
                    }
#endif
                    if (GET_HDMI_HDCP2_TX_STATE_STATE_STORED_KM_TIME_STATUS(port) == _TIME_OUT) {
                        CLR_HDMI_HDCP2_TX_STATE_STATE_STORED_KM_TIME_STATUS(port);
                        HDCPTX_WARIN("AKE storm verify h timeout\n");
                        verify_hprime_failed_count++;
                        if (verify_hprime_failed_count > 5) {
                            verify_hprime_failed_count = 0;
                            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_AKE_STOREKM_VERIFY_H_TIMEOUT);
                        }

                        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
                        return;
                    }
                    //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_STORED_KM << 8 | port);
                    // Store KM Case ---> Jump to Locality Check State
                    verify_hprime_failed_count = 0;
                    ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_LOCALITY_CHECK);
                }
                else {
#if 1
                    // No Store KM Case ---> Jump to Pairing State
                    if (diff_ms >  VERIFY_INTEGRITY_OF_SRM_NO_STORED_TIMEOUT) {
                        SET_HDMI_HDCP2_TX_STATE_NO_STORED_KM_TIME_STATUS(port);
                        HDCPTX_WARIN("[HDC22TX]NO_STORED_KM timeout:%lu\n", diff_ms);
                    }
#endif
                    if (GET_HDMI_HDCP2_TX_STATE_NO_STORED_KM_TIME_STATUS(port) == _TIME_OUT) {
                        CLR_HDMI_HDCP2_TX_STATE_NO_STORED_KM_TIME_STATUS(port);
                        HDCPTX_WARIN("AKE no storm verify h timeout\n");
                        verify_hprime_failed_count++;
                        if (verify_hprime_failed_count > 5) {
                            verify_hprime_failed_count = 0;
                            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_AKE_NO_STOREKM_VERIFY_H_TIMEOUT);
                        }

                        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
                        return;
                    }
                    //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_NO_STORED_KM << 8 | port);
                    //core_timer_event_addTimerEvent(PAIRING_TIMEOUT, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_PAIRING << 8 | port, 0);
                    hdcp2_get_hdcptx_current_time(&start_time[port]);
                    verify_hprime_failed_count = 0;
                    ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_PAIRING);
                    //delayms(200);
                    //set 200ms timer,need added by zhaodong
                    //ScalerTimerActiveTimerEvent(SEC(0.3), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_TO_READ_PARING);
                }
            }
            else {
                //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
                SET_HDMI_HDCP2_TX2_AUTH_START(port);
                HDCPTX_WARIN("Hprime() Fail\n");
                verify_hprime_failed_count++;
                if (verify_hprime_failed_count > 5) {
                    verify_hprime_failed_count = 0;
                    //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_HPRIME_FAIL);
                }

                ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
            }
        }
        else {
            SET_HDMI_HDCP2_TX2_AUTH_START(port);
            HDCPTX_WARIN("ComputeH Fail\n");
            verify_hprime_failed_count++;
            if (verify_hprime_failed_count > 5) {
                verify_hprime_failed_count = 0;
                //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_COMPUTE_H_FAIL);
            }

            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
        }
    }

}


void Hdcp2TxPairingProcess(unsigned char port)
{
    pairing_ret = ScalerHdmiHdcp2Tx2StorePairingInfo(port);
    if (pairing_ret == _TRUE) {
        diff_ms = hdcptx_timer_diff_90k_ms_counter(start_time[port]);
#if 1
        if (diff_ms > PAIRING_TIMEOUT) {
            SET_HDMI_HDCP2_TX_STATE_STATE_PAIRING_TIME_STATUS(port);
        }
#endif
        if (GET_HDMI_HDCP2_TX_STATE_STATE_PAIRING_TIME_STATUS(port) == _TIME_OUT) {
            CLR_HDMI_HDCP2_TX_STATE_STATE_PAIRING_TIME_STATUS(port);
            HDCPTX_WARIN("pairing timeout:%lu\n", diff_ms);
            pairing_failed_count++;
            if (pairing_failed_count > 5) {
                pairing_failed_count = 0;
                //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_PAIRING_TIMEOUT);
            }

            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
            return;
        }
        //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_PAIRING << 8 | port);
        pairing_failed_count = 0;
        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_LOCALITY_CHECK);
    }
    else if (pairing_ret == HDCP2_TX_READ_RX_TIMEOUT) {
        HDCPTX_WARIN("pairing timeout2\n");
        //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_PAIRING_TIMEOUT);
        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
        return;
    }
    else {
        SET_HDMI_HDCP2_TX2_AUTH_START(port);
        HDCPTX_WARIN("Paring Fail\n");
        pairing_failed_count++;
        if (pairing_failed_count > 5) {
            pairing_failed_count = 0;
            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_PARING_FAIL);
        }

        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }
}
void Hdcp2TxLocalityCheckProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("LC\n");

        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
        //delayms(20);
        //CLR_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port);
        //ScalerTimerActiveTimerEvent(SEC(0.02), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_TO_READ_L_PRIME);
    }

    if (GET_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port) <  _HDMI_TX_HDCP2_LC_RETRY_MAX_NUMBER) {
        if (ScalerHdmiHdcp2Tx2LocalityCheck(port) == _TRUE) {
            diff_ms = hdcptx_timer_diff_90k_ms_counter(start_time[port]);
#if 1
            if (diff_ms > VERIFY_LPRIME_TIMEOUT) {
                SET_HDMI_HDCP2_TX_STATE_LOCALITY_CHECK_TIME_STATUS(port);
            }
#endif
            if (GET_HDMI_HDCP2_TX_STATE_LOCALITY_CHECK_TIME_STATUS(port) == _TIME_OUT) {
                CLR_HDMI_HDCP2_TX_STATE_LOCALITY_CHECK_TIME_STATUS(port);
                HDCPTX_WARIN("verify L timeout:%lu\n", diff_ms);
                //HDCPTX_WARIN( "verify L timeout (time=%d)\n", (unsigned int)diff_ms);
                lc_check_failed_count++;
                if (lc_check_failed_count > 5) {
                    lc_check_failed_count = 0;
                    //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_VERIFY_L_TIMEOUT);
                    //ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
                }

                Hdcp2TxLcRetryProcess(port);
                return;
            }
            //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_LOCALITY_CHECK << 8 | port);
            //HDCPTX_WARIN( "verify L complete (time=%d)\n", (unsigned int)diff_ms);

            HDCPTX_WARIN("verify L complete\n");
            CLR_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port);
            CLR_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port);
            CLR_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port);
            lc_check_failed_count = 0;
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_SKE);

        }
        else if (GET_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port) == _TRUE) {
            ADD_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port);
            CLR_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port);
            CLR_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port);
            //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_LOCALITY_CHECK << 8 | port);
            //ScalerTimerActiveTimerEvent(SEC(0.02), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_TO_READ_L_PRIME);
        }
    }
    else {
        //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
        SET_HDMI_HDCP2_TX2_AUTH_START(port);

        HDCPTX_WARIN("LC Fail\n");
        lc_check_failed_count++;
        if (lc_check_failed_count > 5) {
            lc_check_failed_count = 0;
            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_LC_FAIL);
        }
        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }
}

void Hdcp2TxSkeProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("SKE\n");

        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
    }

    if (ScalerHdmiHdcp2Tx2SessionKeyExchangeProc(port) == _TRUE) {
        hdcp2_get_hdcptx_current_time(&enc_time[port]);
        //core_timer_event_addTimerEvent(HDCP2_TX2_ENCRYPT_DATA_TIMEOUT, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_ENC << 8 | port, 0);
        if (GET_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port) == _TRUE) {
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_VERIFY_V_PRIME);
        }
        else {
#if 1
            if (GET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port) == _FALSE) {
                // Set Global Ksv list Start Index & Length
                //g_usHdmiMacTx2HdcpKsvFifoStartIndex = g_usDpHdcpRx0ReceiverIdListIndex;
                //g_usHdmiMacTx2HdcpKsvFifoLength = 5;
                repeater_temp[0] = BSTATUS_DEVICE_COUNT(1) | BSTATUS_MAX_DEVS_EXCEEDED(0) | BSTATUS_MAX_CASCADE_EXCEEDED(0) | BSTATUS_DEPTH(1) | BSTATUS_HDMI_MODE(1); //hdmi mode bit12(0x1000)
                repeater_temp[1] = RX_INFO_DEVICE_COUNT(1) | RX_INFO_DEPTH(1);

                // DownStream is Sink Only , fill-in the sink's ReceiverId to the ReceiverID_List directly.
                HdcpTxStoredReceiverId(g_pucHdmiHdcp2Tx2ReceiverId[port], 1);
                SET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port);
            }
#endif

            //SET_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(_HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_PASS);
            ske_failed_count = 0;
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_AUTHEN_DONE);
        }
    }
    else {
        // ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
        SET_HDMI_HDCP2_TX2_AUTH_START(port);

        HDCPTX_WARIN("SkeProc Fail\n");
        ske_failed_count++;
        if (ske_failed_count > 5) {
            ske_failed_count = 0;
            //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_SKE_PROC_FAIL);
        }

        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }

}

void Hdcp2TxVerifyVprimeProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("V Prime\n");

        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
        hdcptx_current_time[port] = rtk_timer_misc_90k_ms();
        start_time[port] = rtk_timer_misc_90k_ms();

        //core_timer_event_addTimerEvent(250, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_VERIFY_V_PRIME << 8 | port, 0);
        //core_timer_event_addTimerEvent(3000, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_POLLING_V_READY_TIME << 8 | port, 0);
        vprime_time_ms[port] = 250;
        //ScalerTimerActiveTimerEvent(SEC(3), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_FOR_V_READY);
        //ScalerTimerActiveTimerEvent(SEC(0.25), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_POLLING_V_READY);
    }
    diff_ms = hdcptx_timer_diff_90k_ms_counter(hdcptx_current_time[port]);
    if (diff_ms > vprime_time_ms[port]) {
        SET_HDMI_HDCP2_TX2_POLLING_V_READY(port);
    }
    if (GET_HDMI_HDCP2_TX2_POLLING_V_READY(port) == _TRUE) {
        CLR_HDMI_HDCP2_TX2_POLLING_V_READY(port);
        CLR_HDMI_HDCP2_TX2_V_READY_BIT(port);

#if 1
        if (ScalerHdmiHdcp2Tx2PollingStatus(port) == _HDCP_2_2_TX_V_READY) {
            HDCPTX_WARIN("V - Polling\n");
            SET_HDMI_HDCP2_TX2_V_READY_BIT(port);
        }
        else {
            HDCPTX_WARIN("wait v ready\n");
            vprime_time_ms[port] = 100;
            hdcptx_current_time[port] = rtk_timer_misc_90k_ms();
            //core_timer_event_addTimerEvent(100, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_VERIFY_V_PRIME << 8 | port, 0);
        }
#endif
    }

    //SET_HDMI_HDCP2_TX2_V_READY_BIT(port);
    if (GET_HDMI_HDCP2_TX2_V_READY_BIT(port) == _TRUE) {
        HDCPTX_WARIN("V - GO\n");

        //ScalerTimerDelayXms(3);
        //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_VERIFY_V_PRIME << 8 | port);
        //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_POLLING_V_READY_TIME << 8 | port);

        //ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_POLLING_V_READY);
        //ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_FOR_V_READY);

        /*
        if(ScalerHdmiTx2HDCP2ChecRxInfo() == _TRUE)
        {
            if(ScalerHdmiTx2HDCP2VerifyVprime() == _TRUE)
            {
                ScalerHdmiTx2HDCP2ChangeAuthState(_HDCP_2_2_TX_STATE_AUTHEN_DONE);

                SET_HDMI_TX2_HDCP2_DOWNSTREAM_EVENT(_HDMI_TX2_HDCP2_DOWNSTREAM_AUTH_EVENT_PASS);
            }
            else
            {
                ScalerHdmiTx2HDCP2ChangeAuthState(_HDCP_2_2_TX_STATE_IDLE);
            }
        }
        else
        {
            ScalerHdmiTx2HDCP2ChangeAuthState(_HDCP_2_2_TX_STATE_IDLE);

            SET_HDMI_TX2_HDCP2_DOWNSTREAM_EVENT(_HDMI_TX2_HDCP2_DOWNSTREAM_AUTH_EVENT_DEVICE_DEPTH_EXCEED);
        }
        */

        if (ScalerHdmiHdcp2Tx2VerifyVprime(port, _TRUE) == _TRUE) {
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_AUTHEN_DONE);
            //T_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(_HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_PASS);
        }
        else {
            //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
            SET_HDMI_HDCP2_TX2_AUTH_START(port);
            HDCPTX_WARIN("Vprime() Fail\n");
            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);

        }

    }

    diff_ms = hdcptx_timer_diff_90k_ms_counter(start_time[port]);
#if 1
    if (diff_ms > 3000) {
        SET_HDMI_HDCP2_TX2_V_READY_TIMEOUT(port);
    }
#endif
    if (GET_HDMI_HDCP2_TX2_V_READY_TIMEOUT(port) == _TRUE) {
        //ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_POLLING_V_READY);
        // core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_VERIFY_V_PRIME << 8 | port);

        //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
        SET_HDMI_HDCP2_TX2_AUTH_START(port);

        HDCPTX_WARIN("V_ready timeout\n");
        ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
    }
}

void Hdcp2TxAuthDoneProcess(unsigned char port)
{
    if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
        HDCPTX_WARIN("Auth Done\n");
        polling_time[port] = rtk_timer_misc_90k_ms();

        //SET_TX_MAIN_MODULE_HDCP_ERR(HDCP_TX_NO_ERROR);
        CLR_HDMI_hdcp2_auth_cnt_STAT(port);
        CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
        //SET_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port);
    }
    diff_ms = hdcptx_timer_diff_90k_ms_counter(polling_time[port]);
    if (diff_ms > HDCP2_TX2_POLLING_REAUTH_TIMEOUT) {
        SET_HDMI_HDCP2_TX_STATE_SKE_TIME_STATUS(port);
    }

    //polling reauth every 200ms
    if (GET_HDMI_HDCP2_TX_STATE_SKE_TIME_STATUS(port) == _TIME_OUT) {
        SET_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port);
        CLR_HDMI_HDCP2_TX_STATE_SKE_TIME_STATUS(port);
        polling_time[port] = rtk_timer_misc_90k_ms();
    }
    //core_timer_event_addTimerEvent(HDCP2_TX2_POLLING_REAUTH_TIMEOUT, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_AUTHEN_DONE << 8 | port, 0);
    if (GET_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port) == _TRUE) {
        CLR_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port);

        //ScalerTimerActiveTimerEvent(200, _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_STABLE_CHECK);
        //delayms(200);
        //SET_HDMI_HDCP2_TX2_STABLE_POLLIN_EVENT(port);
#if 1
        if (ScalerHdmiHdcp2Tx2PollingStatus(port) == _HDCP_2_2_TX_REAUTH_REQUEST) {
            SET_HDMI_HDCP2_TX2_AUTH_START(port);
            //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();

            HDCPTX_WARIN("Reauth bit set\n");

            ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
        }
        if (GET_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port) == _TRUE) {
            if (ScalerHdmiHdcp2Tx2PollingStatus(port) == _HDCP_2_2_TX_V_READY) {
                mdelay(3);

                if (ScalerHdmiHdcp2Tx2VerifyVprime(port, _FALSE) == _TRUE) {
                    ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_AUTHEN_DONE);

#if 0
                    // SET_HDMI_TX2_HDCP2_DOWNSTREAM_EVENT(_HDMI_TX2_HDCP2_DOWNSTREAM_AUTH_EVENT_PASS);
                    if (GET_DP_HDCP2_RX0_AUTH_STATE() == _DP_RX_HDCP2_REPEATER_STATE_AUTH_DONE) {
                        HDCPTX_WARIN("RealTX: Hdcp2 Set Re-auth Event due to DPF update V");
                        SET_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(_HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_REAUTH);
                    }
#endif
                }
                else {
                    //ADD_HDMI_MAC_TX2_HDCP_RETRY_COUNT();
                    SET_HDMI_HDCP2_TX2_AUTH_START(port);
                    HDCPTX_WARIN("verify v prime failed\n");
                    ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
                }
            }
        }
#endif
    }
}

void Hdcp2TxLcRetryProcess(unsigned char port)
{
    ADD_HDMI_HDCP2_TX2_LC_RETRY_COUNTER(port);
    CLR_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port);
    CLR_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port);
    //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_LOCALITY_CHECK << 8 | port);
    ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_LOCALITY_CHECK);
}


//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : HDCP Handler Process for Hdmi Tx
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerHdmiHdcp2Tx2Handler(unsigned char port)
{
    if(port < HDMI_TX_PORT_MAX_NUM)
    {
        switch (GET_HDMI_HDCP2_TX2_AUTH_STATE(port)) {
            case _HDCP_2_2_TX_STATE_IDLE:
                Hdcp2TxIdleProcess(port);
                break;

            case _HDCP_2_2_TX_STATE_AKE_INIT:
                ADD_HDMI_hdcp2_auth_cnt_STAT(port);
                Hdcp2TxAkeInitProcess(port);
                break;
            case _HDCP_2_2_TX_STATE_VERIFY_SIGNATURE:
                Hdcp2TxVerifySignatureProcess(port);
                break;
            case _HDCP_2_2_TX_STATE_NO_STORED_KM:
                LOCK_HDCP2_TX();
                Hdcp2TxNoStoredKmProcess(port);
                UNLOCK_HDCP2_TX();
                break;
            case _HDCP_2_2_TX_STATE_STORED_KM:
                LOCK_HDCP2_TX();
                Hdcp2TxStoredKmProcess(port);
                UNLOCK_HDCP2_TX();
                break;
            case _HDCP_2_2_TX_STATE_VERIFY_H_PRIME:
                LOCK_HDCP2_TX();
                Hdcp2TxVerifyHprimeProcess(port);
                UNLOCK_HDCP2_TX();
                break;

            case _HDCP_2_2_TX_STATE_PAIRING:
                LOCK_HDCP2_TX();
                if (GET_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port) == _TRUE) {
                    HDCPTX_WARIN("Pairing\n");
                    CLR_HDMI_HDCP2_TX2_AUTH_STATE_CHANGE(port);
                    Hdcp2TxPairingProcess(port);
                }
                UNLOCK_HDCP2_TX();
                break;
            case _HDCP_2_2_TX_STATE_LOCALITY_CHECK:
                LOCK_HDCP2_TX();
                Hdcp2TxLocalityCheckProcess(port);
                UNLOCK_HDCP2_TX();
                break;
            case _HDCP_2_2_TX_STATE_SKE:
                LOCK_HDCP2_TX();
                Hdcp2TxSkeProcess(port);
                UNLOCK_HDCP2_TX();
                break;
            case _HDCP_2_2_TX_STATE_VERIFY_V_PRIME:
                Hdcp2TxVerifyVprimeProcess(port);
                break;
            case _HDCP_2_2_TX_STATE_AUTHEN_DONE:
                Hdcp2TxAuthDoneProcess(port);
                break;

            default:

                break;
        }
        if (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) == _HDCP_2_2_TX_STATE_AUTHEN_DONE) {
            ScalerHdmiHdcp2Tx2ContentStreamManagementHandler(port);
            //ScalerHdmiHdcp2Tx2ChangeAuthState(_HDCP_2_2_TX_STATE_IDLE);
        }
        if ((GET_HDMI_HDCP2_TX2_AUTH_STATE(port) > _HDCP_2_2_TX_STATE_SKE) &&
                (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) < _HDCP_2_2_TX_STATE_VERIFY_V_PRIME)) {
            if ((GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI20_LINK_ON) ||
                    (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON)) {
                if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _FALSE) {
                    diff_ms = hdcptx_timer_diff_90k_ms_counter(enc_time[port]);
                    if (diff_ms > HDCP2_TX2_ENCRYPT_DATA_TIMEOUT) {
                        HDCPTX_WARIN("[HDC22TX]ready to set enc\n");
                        SET_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port);
                    }
                    if (GET_HDMI_HDCP2_TX_STATE_ENC_TIME_STATUS(port) == _TIME_OUT) {
                        CLR_HDMI_HDCP2_TX_STATE_ENC_TIME_STATUS(port);
                        SET_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port);
                    }
                    if (GET_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port) == _TRUE) {
                        CLR_HDMI_HDCP2_TX2_ENCRYPT_DATA_TIMEOUT(port);
                        ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 1);
                        hdcp_tx_mode = HDCP22;
                        HDCPTX_WARIN("HDCP2 - Encrypt **********\n");
                    }
                }
            }
            else {
                if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _TRUE) {
                    ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
                    HDCPTX_WARIN("Disable Encrypt2\n");
                }
            }
        }
        else {
            if (GET_HDMI_HDCP14_TX2_AUTH_STATE(port) < _HDMI_TX_HDCP_STATE_AUTH_2) {
                if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _TRUE) {
                    ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
                    HDCPTX_WARIN("Disable Encrypt3\n");
                }
            }
        }
        if (GET_HDMI_hdcp2_auth_cnt_STAT(port) >= HDCP_TX_AUTH_MAX_CNT) {
            if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) {
                ScalerHdmiHdcp2TxSetAuthTimeOut(port);
                HDCPTX_WARIN("HDCP2 AUTH timeout!!!!\n");
            }
        }
    }else{
        HDCPTX_ERR("HDCP2 port not correct:%d!!!!\n", port);
    }
}
EXPORT_SYMBOL(ScalerHdmiHdcp2Tx2Handler);

void ScalerSetHdmiHdcp2Tx2Auth(unsigned char port, unsigned char enable)
{
    if (enable) {
        HDCPTX_WARIN("set auth timer\n");
		hdmi_hdcptx_timer_eanble = 1;
        //core_timer_event_addTimerEvent(HDCP2_TX2_POLLING_HANDLE_MS, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_IDLE << 8 | port, 0);
    }
    else {
        //core_timer_event_cancelTimerEvent(MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_IDLE << 8 | port);
        ScalerHdmiHdcp2TxClearAuthState(port);
        hdmi_hdcptx_timer_eanble = 0;
        hdcp_tx_running = 0;
        HDCPTX_WARIN("cancel auth timer\n");
    }
}
EXPORT_SYMBOL(ScalerSetHdmiHdcp2Tx2Auth);

void load_hdcp22_tx_key(void)
{
#ifdef HDCP_DEFAULT_KEY
    HDCPTX_INFO("%s: hdcp2tx defalt key enable\n", __func__);
#else
#ifdef CONFIG_OPTEE_HDCP2
    optee_hdcp2_main(HDMI_HDCP2_MODE, HDCP2_CMD_LOAD_TX_KEY, 0, NULL, 0,  hdcp_22_tx_key, 445);
    HDCPTX_INFO("%s: load hdcp2tx optee key\n", __func__);
#endif
#endif
}
EXPORT_SYMBOL(load_hdcp22_tx_key);

