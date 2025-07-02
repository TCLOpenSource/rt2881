#include <linux/kernel.h>
#include <hdcp/hdcp2_2/hdcp2_hal.h>
#include <hdcp/hdcp2_2/crypto.h>
#include <hdcp/hdcp2_2/bigdigits.h>
#include <hdcp/hdcp2_2/hmac.h>
#ifdef HDCP_HW_SPEED_UP
    #include <rbus/hdmitx21_mac_reg.h>
    #include <rbus/hdcp22_speedup_reg.h>
#endif
#include "hdcptx_common.h"

volatile unsigned int temp_data;
unsigned char hdcp_cal_temp_buf[4];
unsigned char hdcp_cal_tmp_data[32];

//unsigned char temp1[64];
//unsigned char temp2[64];
//unsigned char temp[64];

// Description  : HDCP 2.2 AES caculate
void Hdcp2AesCalculate(unsigned char port, unsigned char *pucAesDataIn, unsigned char *pucAesKeyIn)
{
#ifdef HDCP_HW_SPEED_UP
    volatile unsigned char i = 0;

    // AES Calculate
    // HDCP 2.2 Reset
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), 0x00);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), _BIT3);

    // AES Random Disable
    hdcp_mask(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg, ~(_BIT6 | _BIT5), 0x00);

    // Load Input Value
    for (i = 0; i < 16; i += 4) {
        hdcp_cal_temp_buf[3] = pucAesDataIn[i + 0];
        hdcp_cal_temp_buf[2] = pucAesDataIn[i + 1];
        hdcp_cal_temp_buf[1] = pucAesDataIn[i + 2];
        hdcp_cal_temp_buf[0] = pucAesDataIn[i + 3];
        hdcp_out((HDCP22_SPEEDUP_HDCP_AES_DATA_IN_3_reg + i), *((unsigned int *)hdcp_cal_temp_buf));
    }

    for (i = 0; i < 16; i += 4) {
        hdcp_cal_temp_buf[3] = pucAesKeyIn[i + 0];
        hdcp_cal_temp_buf[2] = pucAesKeyIn[i + 1];
        hdcp_cal_temp_buf[1] = pucAesKeyIn[i + 2];
        hdcp_cal_temp_buf[0] = pucAesKeyIn[i + 3];
        hdcp_out((HDCP22_SPEEDUP_HDCP_AES_KEY_3_reg + i), *((unsigned int *)hdcp_cal_temp_buf));
    }

    // Start AES Calculation
    hdcp_mask(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg, ~_BIT6, _BIT6);
    // Wait Finish for AES
    do {
    } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg) & _BIT6) == _BIT6);

    for (i = 0; i < 16; i += 4) {
        hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_3_reg + i,  0);
        hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_3_reg + i,  0);
    }
#endif
}

void Hdcp2AesCalculate_out(unsigned char port, unsigned char *pucAesDataIn, unsigned char *pucAesKeyIn, unsigned char *pucAesDataOut)
{
#ifdef HDCP_HW_SPEED_UP
    unsigned char i = 0;
    //unsigned char aes_data[16];

    memset(hdcp_cal_tmp_data, 0, 16);
    Hdcp2AesCalculate(port, pucAesDataIn, pucAesKeyIn);
    for (i = 0; i < 16; i += 4) {
        temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_AES_OUT_3_reg +  i);
        hdcp_cal_tmp_data[i + 0] = (temp_data >> 24) & 0xff;
        hdcp_cal_tmp_data[i + 1] = (temp_data >> 16) & 0xff;
        hdcp_cal_tmp_data[i + 2] = (temp_data >> 8) & 0xff;
        hdcp_cal_tmp_data[i + 3] = (temp_data) & 0xff;
    }

    memcpy(pucAesDataOut, hdcp_cal_tmp_data, 16);
#endif
}

// Description  : HDCP 2.2 SHA 256 Calculate
unsigned char Hdcp2Sha256Calculate(unsigned char port, unsigned char *pucInputArray, unsigned char ucCalCount)
{
#ifdef HDCP_HW_SPEED_UP
    volatile unsigned char i;

    //memcpy(temp, pucInputArray, 64);
    // Set 1, Then Set 0 for Reset SHA256 Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), _BIT3);

    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), 0x00);

    // Write SHA256 Input Data, Total 512 bits
    for (i = 0; i < 64; i += 4) {
        //rtd_outl(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, ((unsigned int)pucInputArray[i * 4]<<24) | ((unsigned int)pucInputArray[1 + i * 4]<<16) | ((unsigned int)pucInputArray[2 + i * 4]<<8)| ((unsigned int)pucInputArray[3 + i * 4]));
        hdcp_cal_temp_buf[3] = pucInputArray[i];
        hdcp_cal_temp_buf[2] = pucInputArray[i + 1];
        hdcp_cal_temp_buf[1] = pucInputArray[i + 2];
        hdcp_cal_temp_buf[0] = pucInputArray[i + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, *((unsigned int *)hdcp_cal_temp_buf));
        // Inform HW SHA data Has been set first 32 bit
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~_BIT0, _BIT0);
    }

    if (ucCalCount == 0) {
        // Set First Calculate
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

        // Enable SHA256 Calculate
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT0), _BIT2);
    }
    else {
        // Disable First Calculate
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

        // Enable SHA256 Calculate
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT0), _BIT2);
    }

    // Polling 10ms for HDCP Sha Done
    while ((hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg) & _BIT4) != _BIT4) {}
    hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, 0);
#endif
    return _TRUE;
}

//AES Random Gen
unsigned char Hdcp2AesRandomGen(unsigned char port, unsigned char *AesData, unsigned char len)
{
#ifndef HDCP_HW_SPEED_UP
    int i;
    int size_int = len / 4;
    int *tmp = NULL;
    if (len % 4 != 0) {
        HDCPTX_INFO("generate random number size not word aligned\n");
    }
    tmp = (int *)kmalloc(size_int * 4,  GFP_KERNEL);
    if (tmp == NULL) {
        HDCPTX_ERR("ERROR - kmalloc failed, tmp is NULL\n");
        return -1;
    }
    for (i = 0; i < size_int; i++) {
        tmp[i] = crypto_random32();
    }
    memcpy(AesData, tmp, size_int * 4);

    kfree(tmp);
    tmp = NULL;
    return _TRUE;
#else
    volatile unsigned char i;
    volatile unsigned int getVal;
    volatile unsigned int Addr;
    unsigned char hdcp_cal_tmp_data[32];

    if (len % 8 != 0) {
        return _FALSE;
    }
    // Generate 16 bytes for Msb part of Random Stream Seed
    // Enable AES Random Gen
    hdcp_mask(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg, ~(_BIT6 | _BIT5), (_BIT5));
    // AES Trigger
    hdcp_mask(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg, ~_BIT6, _BIT6);

    // Wait Finish for AES Random Gen
    do {

    } while (((hdcp_in(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg) & _BIT6) == _BIT6));
    if ((hdcp_in(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg) & _BIT5) == 0x00) {
        HDCPTX_WARIN("[p%d]random seed1 false\n", (unsigned int)port);
        return _FALSE;
    }
    // Disable AES Random Gen
    hdcp_mask(HDCP22_SPEEDUP_HDCP_AES_CTRL_reg, ~(_BIT6 | _BIT5), 0x00);

    for (i = 0; i < len; i += 4) {
        Addr = HDCP22_SPEEDUP_HDCP_AES_OUT_0_reg - i;
        getVal = hdcp_in(Addr) + rtk_timer_get_us_counter() + 1;
        hdcp_cal_tmp_data[i + 0] = *((unsigned char *)&getVal);
        hdcp_cal_tmp_data[i + 1] = *((unsigned char *)&getVal + 1);
        hdcp_cal_tmp_data[i + 2] = *((unsigned char *)&getVal + 2);
        hdcp_cal_tmp_data[i + 3] = *((unsigned char *)&getVal + 3);
    }
    memcpy(AesData, hdcp_cal_tmp_data, len);
    return _TRUE;
#endif
}


// Description  : HDCP 2.2 HMACSHA 256 Calculate
unsigned char Hdcp2HmacSha256Calculate(unsigned char port, unsigned char *pucHMACSHA256Input, unsigned char *pucHMACSHA256CipherInput, unsigned char ucInputArray1BlockNumber)
{
#ifdef HDCP_HW_SPEED_UP
    unsigned char i;
    //volatile unsigned int temp_data;
    unsigned char *p_cul_t;

    //memcpy(temp1, pucHMACSHA256Input, 64);
    //memcpy(temp2, pucHMACSHA256CipherInput, 64);
    p_cul_t = pucHMACSHA256Input;
    for (i = 0; i < 64; i++) {
        // K0 XOR ipad = 512 bit
        *(pucHMACSHA256CipherInput + i) = *(pucHMACSHA256CipherInput + i) ^ 0x36;
    }

    // Calculate SHA256((K0 XOR ipad) || text)
    if (Hdcp2Sha256Calculate(port, pucHMACSHA256CipherInput, 0) == _TRUE) {
        i = 0;
        do {
            if (Hdcp2Sha256Calculate(port, (pucHMACSHA256Input + (64 * i)), (i + 1)) == _FALSE) {
                return _FALSE;
            }
            i++;
        } while (i < ucInputArray1BlockNumber);
        // KO XOR opad
        for (i = 0; i < 64; i++) {
            *(pucHMACSHA256CipherInput + i) = (*(pucHMACSHA256CipherInput + i) ^ 0x36) ^ 0x5C;
        }
        memset(pucHMACSHA256Input, 0x00, 64);
        for (i = 0; i < 32; i += 4) {
            temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_7_reg +  i);
            *p_cul_t++ = (temp_data >> 24) & 0xff;
            *p_cul_t++ = (temp_data >> 16) & 0xff;
            *p_cul_t++ = (temp_data >> 8) & 0xff;
            *p_cul_t++ = (temp_data) & 0xff;
        }

        *(pucHMACSHA256Input + 32) = 0x80;
        *(pucHMACSHA256Input + 62) = 0x03;

        // SHA256((KO XOR opad) || SHA256(KO XOR ipad) || text)
        if (Hdcp2Sha256Calculate(port, pucHMACSHA256CipherInput, 0) == _TRUE) {
            if (Hdcp2Sha256Calculate(port, pucHMACSHA256Input, 1) == _TRUE) {
                return _TRUE;
            }
        }
    }
#endif
    return _FALSE;
}

