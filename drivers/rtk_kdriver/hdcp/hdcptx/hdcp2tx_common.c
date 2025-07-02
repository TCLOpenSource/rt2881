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
// ID Code      : ScalerHdcp2Tx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#include <rbus/hdmitx21_mac_reg.h>
#include <rbus/hdmitx20_mac0_reg.h>
#include <rbus/hdmitx20_mac1_reg.h>
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp2_tx.h>
#include "hdcptx_common.h"
#include "hdcp2tx_common.h"
#include "hdcp2tx_sys.h"
#include "hdcp_cal.h"
#ifdef HDCP_HW_SPEED_UP
    #include <rbus/hdcp22_speedup_reg.h>
#endif

unsigned char g_pucHdmiHdcp2Tx2TxCaps[HDMI_TX_PORT_MAX_NUM][3];
unsigned char g_pucHdmiHdcp2Tx2ReceiverId[HDMI_TX_PORT_MAX_NUM][5];// = {0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char g_pucHdmiHdcp2Tx2RxCaps[HDMI_TX_PORT_MAX_NUM][3];
unsigned char g_pucHdmiHdcp2Tx2Rtx[HDMI_TX_PORT_MAX_NUM][8];
unsigned char g_pucHdmiHdcp2Tx2Rrx[HDMI_TX_PORT_MAX_NUM][8];
unsigned char g_pucHdmiHdcp2Tx2Npub[128];
unsigned char g_pucHdmiHdcp2Tx2Epub[3];
unsigned char g_pucHdmiHdcp2Tx2Protocol = 0;
unsigned char g_pucHdmiHdcp2Tx2Km[HDMI_TX_PORT_MAX_NUM][16];
unsigned char g_pucHdmiHdcp2Tx2Hdcp2H[HDMI_TX_PORT_MAX_NUM][32];
unsigned char g_pucHdmiHdcp2Tx2Rn[HDMI_TX_PORT_MAX_NUM][8];
unsigned char g_pucHdmiHdcp2Tx2dKey0[HDMI_TX_PORT_MAX_NUM][16];
unsigned char g_pucHdmiHdcp2Tx2dKey1[HDMI_TX_PORT_MAX_NUM][16];
unsigned char g_pucHdmiHdcp2Tx2PairingEkhkm[HDMI_TX_PORT_MAX_NUM][16];
unsigned char g_pucHdmiHdcp2Tx2PairingM[HDMI_TX_PORT_MAX_NUM][16];
unsigned char g_pucHdmiHdcp2Tx2PairingKm[HDMI_TX_PORT_MAX_NUM][16];
unsigned char g_pucHdmiHdcp2Tx2PairingReceiverID[HDMI_TX_PORT_MAX_NUM][5];
unsigned char g_pucHdmiHdcp2Tx2SeqNumV[3];
unsigned int g_ulHdmiHdcp2Tx2SeqNumM = 0;
unsigned char g_pucHdmiHdcp2Tx2M[32];
unsigned short g_usHdmiHdcp2Tx2KsvFifoIndex = 0;
unsigned short g_usHdmiTxHdcpKsvFifoIndex = 0;
unsigned char pucMessageTx[HDMI_TX_PORT_MAX_NUM][150];
unsigned char pucHdmiTx2Hdcp2ReceiverIdList[_HDCP2_TX_DEVICE_COUNT_MAX * 5];
unsigned char g_rx_msg_buf[HDMI_TX_PORT_MAX_NUM][33];
unsigned char g_ake_tx_info[3] = {   //txcaps
    0x02,
    0x00,
    0x00
};
unsigned char g_repeater = 0;

static DIGIT_T hdcp_s[3072 / 32] = {0}; // k = length
static DIGIT_T hdcp_n[(3072 / 32)] = {0};
static DIGIT_T hdcp_e[(3072 / 32)] = {0};
static DIGIT_T hdcp_m[(3072 / 32)] = {0};
//static unsigned char hdcp_EM_prime[(3072 / 8)] = {0};

extern unsigned long  start_time[HDMI_TX_PORT_MAX_NUM];

unsigned short repeater_temp[2];
//****************************************************************************
// CODE TABLES
//****************************************************************************
unsigned char g_pucL[HDMI_TX_PORT_MAX_NUM][32];

unsigned char input_E_golden[128] = {
    0x07, 0x0b, 0x6b, 0x8a, 0x18, 0x0a, 0xee, 0x41, 0xde, 0x41, 0x83, 0x25, 0x23, 0x8a, 0x12, 0x3f,
    0x35, 0x65, 0xb3, 0x07, 0x89, 0x48, 0x28, 0x5e, 0x6c, 0x49, 0xa6, 0x48, 0x0e, 0x2a, 0x52, 0x1f,
    0x32, 0x9a, 0x7b, 0x80, 0x51, 0x08, 0x27, 0x81, 0x0d, 0x7e, 0xfd, 0x1e, 0xa5, 0x1e, 0xfa, 0xc0,
    0xc3, 0x5f, 0xb6, 0xba, 0xb1, 0xe6, 0xa9, 0x98, 0x7d, 0x4b, 0xb4, 0xf2, 0x8a, 0x07, 0xd2, 0xc1,
    0x7d, 0x8e, 0xa5, 0x59, 0xa2, 0x56, 0xaf, 0x80, 0xa7, 0x5f, 0x20, 0x23, 0x3f, 0x96, 0xde, 0x8f,
    0x5a, 0xbf, 0x9c, 0x7b, 0x3a, 0x22, 0x3a, 0x22, 0xd9, 0x42, 0x18, 0xf6, 0xdb, 0x65, 0x33, 0x76,
    0x1f, 0xc2, 0x5f, 0xbc, 0xbf, 0x56, 0x24, 0x16, 0x7e, 0xdb, 0x27, 0x78, 0x8c, 0xfc, 0x64, 0x17,
    0xcd, 0x00, 0xd6, 0xb1, 0xf6, 0x71, 0x7a, 0xdc, 0x30, 0xcb, 0xaa, 0xb8, 0x3e, 0x39, 0x64, 0xea,
};

unsigned char tucDigestInfo[20] = {
    0x00, 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48,
    0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20,
};
unsigned char pucLHash[32] = {
    0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14,
    0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
    0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C,
    0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55,
};

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : Compute H
// Input Value  : pucHdmiTxHdcp2Rtx, pucHdmiTxHdcp2Rrx, pucHdmiTxHdcp2Km
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2ComputeH(unsigned char port)
{
    unsigned char i;
    unsigned char pucTemp1[16];
    unsigned char pucTemp2[16];
    unsigned char pucTemp3[16];
    unsigned char pucTemp4[32];
    unsigned char pucHMACSHA256Input[64];
    unsigned char pucHMACSHA256CipherInput[64];
    unsigned char pucKm[16];
    unsigned char rn_xor_buf[16] = {0};
#ifdef HDCP_HW_SPEED_UP
    unsigned int temp_data;
#endif

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }
    //ScalerHdcp2Proc2(pucTemp1, pucHdmiTxHdcp2Km);
    //memcpy(pucTemp1, pucHdmiTxHdcp2Km,16);

    // Get Kd = dkey0 || dkey1
    //ScalerHdcp2DkeyCalculate(_HDCP_2_2_DKEY_0, pucHdmiTxHdcp2Rtx, pucHdmiTxHdcp2Rrx, pucTemp1, g_pucHdmiHdcp2Tx2Rn);
    // Initial AES Input Value
    memcpy(pucTemp1, g_pucHdmiHdcp2Tx2Rtx[port], 8);
    memcpy(&pucTemp1[8], g_pucHdmiHdcp2Tx2Rrx[port], 8);
    memcpy(pucKm, g_pucHdmiHdcp2Tx2Km[port], 16);

#ifndef HDCP_HW_SPEED_UP
    //gen dkey 0
    RCP_HDCP2_GenDKey(pucKm, pucTemp1, &pucTemp1[8], rn_xor_buf, 0, pucTemp2, 1);
    //gen dkey 1
    RCP_HDCP2_GenDKey(pucKm, pucTemp1, &pucTemp1[8], rn_xor_buf, 1, pucTemp3, 1);

    // Compute H
    memset(pucHMACSHA256Input, 0, sizeof(pucHMACSHA256Input));
    memset(pucHMACSHA256CipherInput, 0, sizeof(pucHMACSHA256CipherInput));

    // H = HMAC-SHA256(rtx || RxCaps || TxCaps, kd)
    // HMAC(text) = HMAC(K, text) = SHA256((K0 XOR opad) || SHA256((KO XOR ipad) || text))
    // Calculate SHA256((KO XOR ipad) || text)
    memcpy(pucHMACSHA256CipherInput, pucTemp2, 16);
    memcpy(&pucHMACSHA256CipherInput[16], pucTemp3, 16);
    memcpy(pucHMACSHA256Input, g_pucHdmiHdcp2Tx2Rtx[port], 8);


    for (i = 0; i < 8; i++) {
        pucHMACSHA256Input[i] = pucHMACSHA256Input[i] ^ g_repeater;
    }

    memcpy(&pucHMACSHA256Input[8], g_pucHdmiHdcp2Tx2RxCaps[port], 3);
    memcpy(&pucHMACSHA256Input[11], g_ake_tx_info, 3);
    memset(&pucHMACSHA256CipherInput[32], 0, 32);
    //comput Hprime
    hmacsha256(pucHMACSHA256CipherInput, 32, pucHMACSHA256Input, 14, pucTemp4);
#else
    Hdcp2AesCalculate_out(port, pucTemp1, pucKm, pucTemp2);  //dk0
    //ScalerHdcp2DkeyCalculate(_HDCP_2_2_DKEY_1, pucHdmiTxHdcp2Rtx, pucHdmiTxHdcp2Rrx, pucTemp1, g_pucHdmiHdcp2Tx2Rn);
    memcpy(pucTemp1, g_pucHdmiHdcp2Tx2Rtx[port], 8);
    memcpy(&pucTemp1[8], g_pucHdmiHdcp2Tx2Rrx[port], 8);
    pucTemp1[15] = g_pucHdmiHdcp2Tx2Rrx[port][7] ^ 0x01;

    Hdcp2AesCalculate_out(port, pucTemp1, pucKm, pucTemp3); //dk1
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_3_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_2_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_1_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_0_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_3_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_2_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_1_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_0_reg  0);

    // Compute H

    memset(pucHMACSHA256Input, 0, sizeof(pucHMACSHA256Input));
    memset(pucHMACSHA256CipherInput, 0, sizeof(pucHMACSHA256CipherInput));


    // H = HMAC-SHA256(rtx || RxCaps || TxCaps, kd)
    // HMAC(text) = HMAC(K, text) = SHA256((K0 XOR opad) || SHA256((KO XOR ipad) || text))
    // Calculate SHA256((KO XOR ipad) || text)
    memcpy(pucHMACSHA256CipherInput, pucTemp2, 16);
    memcpy(&pucHMACSHA256CipherInput[16], pucTemp3, 16);
    memcpy(pucHMACSHA256Input, g_pucHdmiHdcp2Tx2Rtx[port], 8);

    for (i = 0; i < 8; i++) {
        pucHMACSHA256Input[i] = pucHMACSHA256Input[i] ^ g_repeater;
    }

    memcpy(&pucHMACSHA256Input[8], g_pucHdmiHdcp2Tx2RxCaps[port], 3);
    memcpy(&pucHMACSHA256Input[11], g_ake_tx_info, 3);
    puc[14] = 0x80;
    puc[62] = 0x02;
    puc[63] = 0x70;
    memset(&pucHMACSHA256CipherInput[32], 0, 32);

    Hdcp2HmacSha256Calculate(port, pucHMACSHA256Input, pucHMACSHA256CipherInput);

    // Get H
    for (i = 0; i < 32; i += 4) {
        temp_data = hdcp_in(HDCP22_SPEECUP_HDCP_SHA_7 +  i);
        pucTemp4[i + 0] = (temp_data >> 24) & 0xff;
        pucTemp4[i + 1] = (temp_data >> 16) & 0xff;
        pucTemp4[i + 2] = (temp_data >> 8) & 0xff;
        pucTemp4[i + 3] = (temp_data) & 0xff;
    }
#endif
    /************HDCP_HW_SPEED_UP END************/

    memcpy(g_pucHdmiHdcp2Tx2dKey0[port], pucTemp2, 16);

    memcpy(g_pucHdmiHdcp2Tx2dKey1[port], pucTemp3, 16);

    memcpy(g_pucHdmiHdcp2Tx2Hdcp2H[port], pucTemp4, 32);


    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("[HDCP2TX]H:\n");
        for (i = 0; i < 32; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)g_pucHdmiHdcp2Tx2Hdcp2H[port][i]);
        }
        HDCPTX_WARIN("[HDCP2TX]\n");

        HDCPTX_WARIN("[HDCP2TX]DK0\n");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)g_pucHdmiHdcp2Tx2dKey0[port][i]);
        }
        HDCPTX_WARIN("[HDCP2TX]\n");
        HDCPTX_WARIN("[HDCP2TX]DK1\n");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)g_pucHdmiHdcp2Tx2dKey1[port][i]);
        }
        HDCPTX_WARIN("[HDCP2TX]\n");
    }

    return _TRUE;
}


//--------------------------------------------------
// Description  : Authentication Key Exchange Initial(AKE_init)
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2AkeInitial(unsigned char port)
{
    //unsigned char pucMessageTemp[12];
    unsigned char rtx[8];

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    // Generate Rtx
    if (Hdcp2AesRandomGen(port, rtx, 8) == _FALSE) {
        return _FALSE;
    }
    memcpy(&g_pucHdmiHdcp2Tx2Rtx[port], rtx, 8);
    if (hdcp2tx_func_test) {
        memcpy(g_pucHdmiHdcp2Tx2Rtx[port], tx_rtx_test, 8);
        HDCPTX_WARIN("[HDCP2TX] copy tx_rtx_test\n");
    }

    //send I2C AKE_INIT, need add by zhaodong
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    // Enable AES INT
    pucMessageTx[port][0] = AKE_INIT;

    memcpy(&pucMessageTx[port][1], rtx, 8);
    memcpy(&pucMessageTx[port][1 + 8], g_ake_tx_info, 3);

    // Write Rtx and TxCaps to HDCP Slave
    if (ScalerHdmiHdcp2Tx2WriteMessage(port, 12, pucMessageTx[port]) == _FALSE) {
        return _FALSE;
    }
#endif
    if (GET_HDCP2_TX_TYPE_ERROR_STATUS() == HDCP_AKE_INIT_FAIL) {
        return _FALSE;
    }
    return _TRUE;
}

//--------------------------------------------------
// Description  : Signature Verification
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2VerifySignature(unsigned char port)
{
    volatile unsigned short j;
    unsigned short i;
    unsigned char ret = _TRUE;
#ifdef HDCP_HW_SPEED_UP
    unsigned char temp_buf[4];
    volatile unsigned short k;
    volatile unsigned char rsaDataAddr = 0;
    unsigned char tx_EkeyX0R[128];
    unsigned char tx_Ekey[1];   //get from flash
    const unsigned char *tx_RRmodN;//[384];//get from flash
    unsigned char tx_npinv[4]; //get from flash
    unsigned int temp_data;
    const unsigned char *tx_Nkey;//[384]; //get from flash
#endif
    unsigned char pucSha256Input[64];
    unsigned char *pucMessageTemp = NULL; //pucMessageTemp[534];
    unsigned char pucKsvTemp[5] = {0};
    unsigned short usReadLength;

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    pucMessageTemp = kmalloc(534, GFP_KERNEL);//kernal space
    if (!pucMessageTemp) {
        goto verfy_signature_final;
    }

    // Read Certificate: Receiver ID(5 bytes) + Public Key(131 bytes) + Reserved(2 bytes)
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    usReadLength = 534;  //msgid 1 + cert 522 + RRX 8 + RXcaps 3
    if (ScalerHdmiHdcp2Tx2ReadMessage(port, usReadLength, pucMessageTemp, 100) == _FALSE) {
        ret = _FALSE;
        goto verfy_signature_final;
    }
#endif
    if (hdcp2tx_func_test) {
        pucMessageTemp[0] = 0x3;
        memcpy(pucMessageTemp + 1, rx_cert_key_test, 522); //cert
        memcpy(pucMessageTemp + 1 + 522, rx_rrx_test, 8); //rrx
        memcpy(pucMessageTemp + 1 + 522 + 8, rx_rxcaps_test, 3); //rxcaps
    }

    if (hdcp2tx_read_log_print) {
        HDCPTX_WARIN("VerifySignature read rx cert keyid:\n");
        for (i = 0; i < 5; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucMessageTemp[1 + i]);  //reduce dmem
            if ((i + 1) % 16 == 0) {
                HDCPTX_WARIN("\n");
            }
        }
        HDCPTX_WARIN("\n");
    }

    if (pucMessageTemp[0] != 0x03) {
        ret = _FALSE;
        goto verfy_signature_final;
    }

    // Receiver ID(5 Bytes)
    memcpy(pucKsvTemp, &pucMessageTemp[1], 5);

    // Public n(128 Bytes)
    memcpy(g_pucHdmiHdcp2Tx2Npub, &pucMessageTemp[6], 128);//1 + 5

    // Public e(3 Bytes) and Reserved(2 Bytes)
    memcpy(g_pucHdmiHdcp2Tx2Epub, &pucMessageTemp[134], 3);//1 + 5 + 128

    //g_pucHdmiHdcp2Tx2Protocol = hdcp2_temp[138] & 0x0F;
    // Signature
    // memcpy(pucHdmiTxHdcp2Signature, &pucMessageTemp_l[139], 384);//1 + 5 + 128 + 3 + 2

    // Read Rrx and RxCaps
    memcpy(g_pucHdmiHdcp2Tx2Rrx[port], &pucMessageTemp[523], 8);//1 + 5 + 128 + 3 + 2 + 384

    memcpy(g_pucHdmiHdcp2Tx2RxCaps[port], &pucMessageTemp[531], 3);//1 + 5 + 128 + 3 + 2 + 384 + 8

    if (g_pucHdmiHdcp2Tx2RxCaps[port][2] == 0x01) {
        SET_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port);
    }

#ifndef HDCP_HW_SPEED_UP
    // follow RSASSA-PKCS-v1_5-Verify
    // compare public cert and other fields.
    memset(pucSha256Input, 0x00, SHA256_DIGEST_HDCP2_SIZE);
    Sha256(&pucMessageTemp[1], 138, pucSha256Input);

    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("VerifySignature sha256(M):\n");
        for (i = 0; i < 32; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucSha256Input[i]);  //reduce dmem
            if ((i + 1) % 16 == 0) {
                HDCPTX_WARIN("\n");
            }
        }
        HDCPTX_WARIN("\n");
    }


    /************* m = RSAVP1((n,e),s) -> m = s^e mod n ****************/
    // s = OS2IP(S), S is signature to be verified.
    mpConvFromOctets(hdcp_s, (3072 / 32), &pucMessageTemp[139], HDCP_RECEIVER_LLC_SIGNATURE_SIZE);
    // - prepare n and e [test v.s production]
    if (hdcp2tx_func_test) {
        mpConvFromOctets(hdcp_n, (3072 / 32), hdcp_22_tx_key_n_test, HDCP_TX_MODULUS_N_SIZE);
        mpConvFromOctets(hdcp_e, (3072 / 32), hdcp_22_tx_key_e_test, HDCP_TX_PUBLIC_EXPONENT_E_SIZE);
    }
    else {
        mpConvFromOctets(hdcp_n, (3072 / 32), &hdcp_22_tx_key[40], HDCP_TX_MODULUS_N_SIZE);
        mpConvFromOctets(hdcp_e, (3072 / 32), &hdcp_22_tx_key[424], HDCP_TX_PUBLIC_EXPONENT_E_SIZE);
        //mpConvFromOctets(hdcp_n,(3072/32),tx_modulus_n_production,HDCP_TX_MODULUS_N_SIZE);
        //mpConvFromOctets(hdcp_e,(3072/32),tx_public_exponent_e_production,HDCP_TX_PUBLIC_EXPONENT_E_SIZE);
    }
    mpModExp(hdcp_m, hdcp_s, hdcp_e, hdcp_n, (3072 / 32));
    /************* m = RSAVP1((n,e),s) -> m = s^e mod n ****************/

    // EM' = I2OSP(m,k)
    memset(pucMessageTemp, 0, 384);
    mpConvToOctets(hdcp_m, (3072 / 32), pucMessageTemp, (3072 / 8));

#else
    /************HDCP_HW_SPEED_UP START************/
    //hdcp2_tx_get_n(tx_Nkey);
    tx_Nkey = &hdcp_22_tx_key[N_KEY_START];
    hdcp2_tx_get_e(tx_Ekey);
    hdcp2_tx_get_Npinv(tx_npinv);
    //hdcp2_tx_get_RRmodN(tx_RRmodN);
    tx_RRmodN = &hdcp_22_tx_key[RRMODN_KEY_START];

    // Compute Encode Message(EM)
    for (i = 0; i < 2; i++) {
        memcpy(pucSha256Input, &pucMessageTemp[1 + i * 64], 64);
        if (Hdcp2Sha256Calculate(port, pucSha256Input, i) == _FALSE) {
            ret = _FALSE;
            goto verfy_signature_final;
        }
    }

    memcpy(pucSha256Input, &pucMessageTemp[1 + 128], 10);
    memset(&pucSha256Input[10], 0, 54);
    pucSha256Input[10] = 0x80;
    pucSha256Input[62] = 0x04;
    pucSha256Input[63] = 0x50;

    if (Hdcp2Sha256Calculate(port, pucSha256Input, 2) == _FALSE) {
        ret = _FALSE;
        goto verfy_signature_final;
    }

    // Get SHA-256(M) of EM
    //memcpy(pucSha256Input, (volatile BYTE xdata *)P63_35_HDCP_SHA_0, 32);
    for (i = 0; i < 32; i += 4) {
        temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_7_reg + i);
        pucSha256Input[i + 0] = *((unsigned char *)&temp_data + 3);
        pucSha256Input[i + 1] = *((unsigned char *)&temp_data + 2);
        pucSha256Input[i + 2] = *((unsigned char *)&temp_data + 1);
        pucSha256Input[i + 3] = *((unsigned char *)&temp_data + 0);
    }
    // Enable HDCP 2.2 RSA Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT0), (_BIT7 | _BIT5));
    // RSA Clock Select
    hdcp_mask(HDCP22_SPEEDUP_HDCP_DUMMY1_reg, ~_BIT7, _BIT7);
    // Reset HDCP 2.2 RSA module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), 0x00);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), _BIT3);
    // Select RSA Key Download Data Length
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL1_reg, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

    //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_NP_INV_reg, ((unsigned int)tx_npinv[0]<<24) | ((unsigned int)tx_npinv[1]<<16)| ((unsigned int)tx_npinv[2]<<8) | ((unsigned int)tx_npinv[3]));
    temp_buf[3] = tx_npinv[0];
    temp_buf[2] = tx_npinv[1];
    temp_buf[1] = tx_npinv[2];
    temp_buf[0] = tx_npinv[3];
    hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_NP_INV_reg, *((unsigned int *)temp_buf));
    for (j = 0; j < 48; j++) {
        k = j << 3;
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load DCP LLC Public N Key
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)tx_Nkey[(i << 3)]<<24) | ((unsigned int)tx_Nkey[(i << 3)+1]<<16) | ((unsigned int)tx_Nkey[(i << 3)+2]<<8)| ((unsigned int)tx_Nkey[(i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)tx_Nkey[(i << 3)+4]<<24) | ((unsigned int)tx_Nkey[(i << 3)+5]<<16) | ((unsigned int)tx_Nkey[(i << 3)+6]<<8)| ((unsigned int)tx_Nkey[(i << 3)+7]));
        temp_buf[3] = tx_Nkey[k];
        temp_buf[2] = tx_Nkey[k + 1];
        temp_buf[1] = tx_Nkey[k + 2];
        temp_buf[0] = tx_Nkey[k + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = tx_Nkey[k + 4];
        temp_buf[2] = tx_Nkey[k + 5];
        temp_buf[1] = tx_Nkey[k + 6];
        temp_buf[0] = tx_Nkey[k + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
        rsaDataAddr = 0x90 + (47 - j);
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, rsaDataAddr);
    }

    for (j = 0; j < 32; j++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load e Key(0x00)
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, 0x00);
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, 0x00);
        rsaDataAddr = 0xC0 + (47 - j);
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, rsaDataAddr);
    }
    memcpy(tx_EkeyX0R, input_E_golden, 128);
    tx_EkeyX0R[127] = tx_Ekey[0] ^ tx_EkeyX0R[127];

    for (j = 32; j < 48; j++) {
        k = (j - 32) << 3;
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load e Key
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)tx_EkeyX0R[((i - 32) << 3)]<<24) | ((unsigned int)tx_EkeyX0R[((i - 32) << 3)+1]<<16) | ((unsigned int)tx_EkeyX0R[((i - 32) << 3)+2]<<8)| ((unsigned int)tx_EkeyX0R[((i - 32) << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)tx_EkeyX0R[((i - 32) << 3)+4]<<24) | ((unsigned int)tx_EkeyX0R[((i - 32) << 3)+5]<<16) | ((unsigned int)tx_EkeyX0R[((i - 32) << 3)+6]<<8)| ((unsigned int)tx_EkeyX0R[((i - 32) << 3)+7]));
        temp_buf[3] = tx_EkeyX0R[k];
        temp_buf[2] = tx_EkeyX0R[k + 1];
        temp_buf[1] = tx_EkeyX0R[k + 2];
        temp_buf[0] = tx_EkeyX0R[k + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = tx_EkeyX0R[k + 4];
        temp_buf[2] = tx_EkeyX0R[k + 5];
        temp_buf[1] = tx_EkeyX0R[k + 6];
        temp_buf[0] = tx_EkeyX0R[k + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
        rsaDataAddr = 0xC0 + (47 - j);
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, rsaDataAddr);
    }

    for (j = 0; j < 48; j++) {
        k = j << 3;
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Enable write cipher text to RSA Module
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)tx_RRmodN[(i << 3)]<<24) | ((unsigned int)tx_RRmodN[(i << 3)+1]<<16) | ((unsigned int)tx_RRmodN[(i << 3)+2]<<8)| ((unsigned int)tx_RRmodN[(i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)tx_RRmodN[(i << 3)+4]<<24) | ((unsigned int)tx_RRmodN[(i << 3)+5]<<16) | ((unsigned int)tx_RRmodN[(i << 3)+6]<<8)| ((unsigned int)tx_RRmodN[(i << 3)+7]));
        temp_buf[3] = tx_RRmodN[k];
        temp_buf[2] = tx_RRmodN[k + 1];
        temp_buf[1] = tx_RRmodN[k + 2];
        temp_buf[0] = tx_RRmodN[k + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = tx_RRmodN[k + 4];
        temp_buf[2] = tx_RRmodN[k + 5];
        temp_buf[1] = tx_RRmodN[k + 6];
        temp_buf[0] = tx_RRmodN[k + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
        rsaDataAddr = 0x30 + (47 - j);
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, rsaDataAddr);
    }


    for (j = 0; j < 48; j++) {
        k = j << 3;
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Enable write cipher text to RSA Module
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)pucMessageTemp_l[139 + (i << 3)]<<24) | ((unsigned int)pucMessageTemp_l[139 + (i << 3)+1]<<16) | ((unsigned int)pucMessageTemp_l[139 + (i << 3)+2]<<8)| ((unsigned int)pucMessageTemp_l[139 + (i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)pucMessageTemp_l[139 + (i << 3)+4]<<24) | ((unsigned int)pucMessageTemp_l[139 + (i << 3)+5]<<16) | ((unsigned int)pucMessageTemp_l[139 + (i << 3)+6]<<8)| ((unsigned int)pucMessageTemp_l[139 + (i << 3)+7]));
        temp_buf[3] = pucMessageTemp[k + 139];
        temp_buf[2] = pucMessageTemp[k + 140];
        temp_buf[1] = pucMessageTemp[k + 141];
        temp_buf[0] = pucMessageTemp[k + 142];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = pucMessageTemp[k + 143];
        temp_buf[2] = pucMessageTemp[k + 144];
        temp_buf[1] = pucMessageTemp[k + 145];
        temp_buf[0] = pucMessageTemp[k + 146];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
        rsaDataAddr = 47 - j;
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, rsaDataAddr);

    }

    // Compute Encode Message Prime(EM')
    // ScalerHdmiTx2HDCP2DcpLlcRsaCalculate(&pucMessageTemp_l[139]);
    // Set RSA Write key done
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);
    // Enable RSA Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT6 | _BIT5 | _BIT4 | _BIT0), (_BIT6 | _BIT5 | _BIT4));
    // Reset RSA Calcualte Enable to Start Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT4 | _BIT0), 0x00);
    // Polling 100ms for RSA Module Done
    while ((hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg) & _BIT2) != _BIT2) {}
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT7 | _BIT0), 0x00);
    // Get EM'
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT6 | _BIT5 | _BIT0), _BIT6);

    memset(pucMessageTemp, 0, 384);
    // Read Message From One of 2 Address
    if (((unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg) & _BIT1) == 0x00) {
        for (j = 0; j < 48; j++) {
            hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (47 - j));
            *((unsigned int *)temp_buf) = (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg);
            pucMessageTemp[j << 3] = temp_buf[3];
            pucMessageTemp[(j << 3) + 1] = temp_buf[2];
            pucMessageTemp[(j << 3) + 2] = temp_buf[1];
            pucMessageTemp[(j << 3) + 3] = temp_buf[0];
            *((unsigned int *)temp_buf) = (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg);
            pucMessageTemp[(j << 3) + 4] = temp_buf[3];
            pucMessageTemp[(j << 3) + 5] = temp_buf[2];
            pucMessageTemp[(j << 3) + 6] = temp_buf[1];
            pucMessageTemp[(j << 3) + 7] = temp_buf[0];
            //pm_printk(LOGGER_ERROR,"11RSA3072:%x,%x\n", (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg),(unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg));
        }
    }
    else {
        for (j = 0; j < 48; j++) {
            rsaDataAddr = 0x30 + (47 - j);
            hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, rsaDataAddr);
            *((unsigned int *)temp_buf) = (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg);
            pucMessageTemp[j << 3] = temp_buf[3];
            pucMessageTemp[(j << 3) + 1] = temp_buf[2];
            pucMessageTemp[(j << 3) + 2] = temp_buf[1];
            pucMessageTemp[(j << 3) + 3] = temp_buf[0];
            *((unsigned int *)temp_buf) = (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg);
            pucMessageTemp[(j << 3) + 4] = temp_buf[3];
            pucMessageTemp[(j << 3) + 5] = temp_buf[2];
            pucMessageTemp[(j << 3) + 6] = temp_buf[1];
            pucMessageTemp[(j << 3) + 7] = temp_buf[0];
            //pm_printk(LOGGER_ERROR,"22RSA3072:%x,%x\n", (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg),(unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg));

        }
    }
#endif
    /************HDCP_HW_SPEED_UP END************/

    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("VerifySignature EM:\n");
        for (i = 0; i < 384; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucMessageTemp[i]);  //reduce dmem
            if ((i + 1) % 16 == 0) {
                HDCPTX_WARIN("\n");
            }
        }
        HDCPTX_WARIN("\n");
    }

    // Compare EM with EM'
    // EM[3071:0] = 0x00 || 0x01 || PS || 0x00 || T(SHA256) || SHA-256(M), length = 3072 bits

    // Check 0x00 || 0x01
    if ((pucMessageTemp[0] != 0x00) && (pucMessageTemp[1] != 0x01)) {
        HDCPTX_WARIN("[HDCP2TX]RealTX: Signature - Fail 0\n");
        ret = _FALSE;
        goto verfy_signature_final;
    }

    // Check PS(330 bytes)
    for (j = 2; j < 332; j++) {
        if (pucMessageTemp[j] != 0xFF) {
            HDCPTX_WARIN("RealTX: Signature - Fail 1 %d\n", (unsigned int)j);
            ret = _FALSE;
            goto verfy_signature_final;
        }
    }

    // Check T(SHA256)
    for (j = 332; j < 352; j++) {
        if (pucMessageTemp[j] != tucDigestInfo[j - 332]) {
            HDCPTX_WARIN("RealTX: Signature - Fail 2 %d\n", (unsigned int)j);
            ret = _FALSE;
            goto verfy_signature_final;
        }
    }

    // Check SHA-256(M)
    for (j = 352; j < 384; j++) {
        if (pucMessageTemp[j] != pucSha256Input[j - 352]) {
            HDCPTX_WARIN("RealTX: Signature - Fail 3 %d\n", (unsigned int)j);
            ret = _FALSE;
            goto verfy_signature_final;
        }
    }
    if ((g_pucHdmiHdcp2Tx2ReceiverId[port][0] != 0x00) || (g_pucHdmiHdcp2Tx2ReceiverId[port][1] != 0x00) || (g_pucHdmiHdcp2Tx2ReceiverId[port][2] != 0x00) ||
            (g_pucHdmiHdcp2Tx2ReceiverId[port][3] != 0x00) || (g_pucHdmiHdcp2Tx2ReceiverId[port][4] != 0x00)) {
        if ((g_pucHdmiHdcp2Tx2ReceiverId[port][0] != pucKsvTemp[0]) || (g_pucHdmiHdcp2Tx2ReceiverId[port][1] != pucKsvTemp[1]) || (g_pucHdmiHdcp2Tx2ReceiverId[port][2] != pucKsvTemp[2]) ||
                (g_pucHdmiHdcp2Tx2ReceiverId[port][3] != pucKsvTemp[3]) || (g_pucHdmiHdcp2Tx2ReceiverId[port][4] != pucKsvTemp[4])) {
            // Different KSV , re-auth
            HDCPTX_WARIN("RealTX: Hdcp2 Set Re-auth Event due to RID Different\n");
            //SET_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(_HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_REAUTH);
        }
    }
    memcpy(g_pucHdmiHdcp2Tx2ReceiverId[port], pucKsvTemp, 5);

    HDCPTX_WARIN("[HDCP2TX]RealTX: Signature - Pass\n");

verfy_signature_final:
    if (pucMessageTemp) {
        kfree(pucMessageTemp);
    }
    return ret;
}


//--------------------------------------------------
// Description  : No Stored Km Procss
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2NoStoredKmProc(unsigned char port)
{
    unsigned char pucSHA256Input[64];
    unsigned char *pucEkpubKm = NULL;//pucEkpubKm[129];
    unsigned char *pucDB = NULL; //pucDB[95];
    unsigned char *pucRrmodn = NULL; //pucRrmodn[128];
    unsigned char pucSeed[32];
    unsigned char i;
    unsigned char ret = _TRUE;
#ifdef HDCP_HW_SPEED_UP
    unsigned char temp_buf[4];
    unsigned int getVal;
    unsigned char *pBuf;
    unsigned char j;
    unsigned char pucEpubXorCode[128];
    unsigned char k;
    unsigned short nshiftcnt;
    volatile unsigned int regAddr;
    unsigned int npinv;
#endif
    unsigned char pucKm[16];
    unsigned char *pMaskedDB = NULL; //pMaskedDB[95];
    unsigned char *pucSeedMask = NULL; //pucSeedMask[95];
    unsigned char seedMask[32] = {0};
    unsigned char maskedSeed[32] = {0};
    DIGIT_T m[(1024 / 32)] = {0};
    DIGIT_T c[(1024 / 32)] = {0};
    // prepare e and n, convert from octet string to digits
    DIGIT_T n[(1024 / 32)] = {0};
    DIGIT_T e[(1024 / 32)] = {0};
    unsigned char *hdcptx_no_storedkm_tmp = NULL;

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    hdcptx_no_storedkm_tmp = kmalloc(129 + 95 + 128 + 95 + 95, GFP_KERNEL); //kernal space
    if (!hdcptx_no_storedkm_tmp) {
        ret = _FALSE;
        goto no_storedkm_final;
    }
    pucEkpubKm = hdcptx_no_storedkm_tmp;
    pucDB = &hdcptx_no_storedkm_tmp[129];
    pucRrmodn = &hdcptx_no_storedkm_tmp[129 + 95];
    pMaskedDB = &hdcptx_no_storedkm_tmp[129 + 95 + 95];
    pucSeedMask  = &hdcptx_no_storedkm_tmp[129 + 95 + 95];

    memset(pucSHA256Input, 0x00, 64);
    memset(pucEkpubKm, 0x00, 129);
    memset(pucDB, 0x00, 95);
    memset(pucRrmodn, 0x00, 128);

    // Encrypt Km
    // Step1: Hash stream  L = 32byte 0x00 stream, lHash = SHA256(L) = 0x e3 b0 04 42 ??== > hLen = 32byte
    // Step2: PS stream = 0x00 stream == > PS Length = length(n) ??length(km)-2xhLen-2 = 128 ??16 ??2x32 -2 = 46bytes
    // Step3: M = km stream == > km length = 16bytes

    // Generate Km
    if (Hdcp2AesRandomGen(port, pucKm, sizeof(pucKm)) == _FALSE) {
        ret = _FALSE;
        goto no_storedkm_final;
    }
    memcpy(g_pucHdmiHdcp2Tx2Km[port], pucKm, 16); //16
    if (hdcp2tx_func_test) {
        memcpy(g_pucHdmiHdcp2Tx2Km[port], tx_km_test, 16);
    }

    // Step4: DB = lHash || PS || 0x01 || M == > DB Length = 95bytes
    memcpy(pucDB, pucLHash, 32);
    memset(&pucDB[32], 0, 46);
    pucDB[78] = 0x01;
    memcpy(&pucDB[79], g_pucHdmiHdcp2Tx2Km[port], 16); //79+16=95

    // Step5: Generate random stream seed == > Seed Length = hLen = 32bytes

    // Generate 16 bytes for Msb part of Random Stream Seed
    if (Hdcp2AesRandomGen(port, pucSeed, 16) == _FALSE) {
        ret = _FALSE;
        goto no_storedkm_final;
    }

    // Generate 16 bytes for Lsb part of Random Stream Seed
    if (Hdcp2AesRandomGen(port, &pucSeed[16], 16) == _FALSE) {
        ret = _FALSE;
        goto no_storedkm_final;
    }
    if (hdcp2tx_func_test) {
        memcpy(pucSeed, tx_seed_test, 32);
    }

#ifndef HDCP_HW_SPEED_UP
    // Step6: dbMask = MGF (seed, k-hLen-1) == > dbMask Length = 95bytes
    memcpy(pucSHA256Input, pucSeed, 32);
    memset(&pucSHA256Input[32], 0, 32);
    // dbMask = MGF(seed, k-hLen-1);
    // DB length = k - hLen - 1; (128 - 32 - 1) = 95
    MGF1_SHA256(pucSeedMask, 95, pucSHA256Input, 32);
    // maskedDB = DB xor dbMask
    crypt_xor(pucDB, pucSeedMask, pMaskedDB, 95);

    // seedMask = MGF(maskedDB,hLen)
    MGF1_SHA256(seedMask, HDCP_SEED_SIZE, pMaskedDB, 95);
    // maskedSeed = seed xor seedMask
    crypt_xor(pucSeed, seedMask, maskedSeed, 32);

    // EM = 0x0 || maskedSeed || maskedDB
    // EM length = k
    pucEkpubKm[0] = 0;
    memcpy(&pucEkpubKm[1], maskedSeed, 32);
    memcpy(&pucEkpubKm[33], pMaskedDB, 95);

    // m = OS2IP(EM)
    mpConvFromOctets(m, (1024 / 32), pucEkpubKm, (1024 / 8));

    // c = RSAEP((n,e),m)
    // where (n,e) is the RSA public key
    // refer to PKCS #1 v2.1 section 5.1.1
    // c = m^e mod n
    mpConvFromOctets(n, (1024 / 32), g_pucHdmiHdcp2Tx2Npub, (1024 / 8));
    mpConvFromOctets(e, (1024 / 32), g_pucHdmiHdcp2Tx2Epub, 3);
    mpModExp(c, m, e, n, (1024 / 32));

    memset(pucEkpubKm, 0, 129);
    // convert c into octet string
    mpConvToOctets(c, (1024 / 32), &pucEkpubKm[1], (1024 / 8));

#else
    /************HDCP_HW_SPEED_UP START************/
    // Step6: dbMask = MGF (seed, k-hLen-1) == > dbMask Length = 95bytes
    memcpy(pucSHA256Input, pucSeed, 32);
    memset(&pucSHA256Input[32], 0, 32);
    pucSHA256Input[36] = 0x80;
    pucSHA256Input[62] = 0x01;
    pucSHA256Input[63] = 0x20;

    pBuf = &pucEkpubKm[1];
    for (i = 0; i < 3; i++) {
        pucSHA256Input[35] = i;
        if (Hdcp2Sha256Calculate(port, pucSHA256Input, 0) == _FALSE) {
            ret = _FALSE;
            goto no_storedkm_final;
        }
        regAddr = HDCP22_SPEEDUP_HDCP_SHA_7_reg;
        for (k = 0; k < 8; k++) {
            getVal = hdcp_in(regAddr);
            *pBuf = *((unsigned char *)&getVal + 3);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 2);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 1);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal);
            pBuf++;
            regAddr += 4;
        }
    }


    // Step7: maskedDB = DB XOR dbMask; == > maskedDB = 95 bytes
    for (i = 0; i < 95; i++) {
        pucDB[i] = pucDB[i] ^ pucEkpubKm[1 + i];
    }

    // Step8: seedMask = MGF(maskedDB, hLen) == > seedMask Length = 32bytes
    memcpy(pucSHA256Input, pucDB, 64);

    if (Hdcp2Sha256Calculate(port, pucSHA256Input, 0) == _FALSE) {
        ret = _FALSE;
        goto no_storedkm_final;
    }

    memcpy(pucSHA256Input, &pucDB[64], 31);
    memset(&pucSHA256Input[31], 0, 33);
    pucSHA256Input[35] = 0x80;
    pucSHA256Input[62] = 0x03;
    pucSHA256Input[63] = 0x18;

    if (Hdcp2Sha256Calculate(port, pucSHA256Input, 1) == _FALSE) {
        ret = _FALSE;
        goto no_storedkm_final;
    }
    // Step9: maskedSeed = seed XOR seedMask == > maskedSeed Length = 32bytes

    pBuf = &pucEkpubKm[2];
    regAddr = HDCP22_SPEEDUP_HDCP_SHA_7_reg;
    for (k = 0; k < 8; k++) {
        getVal = hdcp_in(regAddr);
        *pBuf = *((unsigned char *)&getVal + 3);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal + 2);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal + 1);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal);
        pBuf++;
        regAddr += 4;
    }

    for (i = 0; i < 32; i++) {
        pucEkpubKm[2 + i] = pucSeed[i] ^ pucEkpubKm[2 + i];
    }

    // Step10: EM = 0x00 || maskedSeed || maskedDB ==> EM Length = 1+ 32 + 95 = 128bytes
    pucEkpubKm[1] = 0x00;
    memcpy(&pucEkpubKm[34], pucDB, 95);


    // PDATA_DWORD(0) = 0x593CC783;
    // PDATA_DWORD(1) = ScalerHdmiTx2HDCP2NpinvCalculate(PDATA_DWORD(0));
    //HDCPTX_WARIN("RealTX: Npinv %d", PDATA_DWORD(1));

    // PDATA_DWORD(0) = 0x593CC783;
    // PDATA_DWORD(0) = ScalerHdmiTx2HDCP2NpinvCalculate(PDATA_DWORD(0));
    //HDCPTX_WARIN("RealTX: Npinv %d", PDATA_DWORD(1));

    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[0] = ((PDATA_DWORD(1) & 0xFF000000) >> 6);
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[1] = ((PDATA_DWORD(1) & 0x00FF0000) >> 4);
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[2] = ((PDATA_DWORD(1) & 0x0000FF00) >> 2);
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[3] = ((PDATA_DWORD(1) & 0x000000FF));

    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[0] = 0xE4;
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[1] = 0x7A;
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[2] = 0x00;
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[3] = 0xD5;

    // Get Npub Mod 2^32
    // memcpy(pData, &g_pucHdmiTx2Hdcp2Npub[124], 4);
    //
    // // Get Npinv
    // PDATA_DWORD(0) = ScalerHdmiTx2HDCP2NpinvCalculate(PDATA_DWORD(0));
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[0] = i;
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[1] = j;
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[2] = pData[2];
    // g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv[3] = pData[3];

    //ScalerHdmiHdcp2Tx2RrmodnCalculate(g_pucHdmiHdcp2Tx2Npub);

    memcpy(pucEpubXorCode, input_E_golden, 128);

    //ScalerHdcp2Proc3(pucEpubXorCode);

    // Enable HDCP 2.2 RSA Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT0), (_BIT7 | _BIT5));

    // RSA Clock Select
    hdcp_mask(HDCP22_SPEEDUP_HDCP_DUMMY1_reg, ~_BIT7, _BIT7);

    // Reset HDCP 2.2 RSA module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), 0x00);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), _BIT3);

    // Select RSA Key Download Data Length and RRMODN mode
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL1_reg, ~(_BIT7 | _BIT6 | _BIT5), (_BIT7 | _BIT5));

    for (i = 0; i < 16; i++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Enable write cipher text to RSA Module
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (unsigned char)(0x30 + (15 - i)));

        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, 0);
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, 0);

        if ((0x30 + (15 - i)) == 0x3F) {
            hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, 0x80000000);
        }
    }

    for (i = 0; i < 16; i++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load N Key
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (unsigned char)(0x90 + (15 - i)));

        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)]<<24) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+1]<<16) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+2]<<8)| ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+4]<<24) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+5]<<16) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+6]<<8)| ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+7]));
        temp_buf[3] = g_pucHdmiHdcp2Tx2Npub[(i << 3)];
        temp_buf[2] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 1];
        temp_buf[1] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 2];
        temp_buf[0] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 4];
        temp_buf[2] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 5];
        temp_buf[1] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 6];
        temp_buf[0] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
    }

    // 1024 bits + 66 bits
#if 0 //reduce dseg
    pucEpubXorCode[126] = ((1088 >> 8) & 0xff) ^ pucEpubXorCode[126];
    pucEpubXorCode[127] = ((1088) & 0xff) ^ pucEpubXorCode[127];
#endif
    pucEpubXorCode[126] = 4 ^ pucEpubXorCode[126];
    pucEpubXorCode[127] = 0x40 ^ pucEpubXorCode[127];

    for (i = 0; i < 16; i++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load e Key
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (unsigned char)(0xC0 + (15 - i)));
        k = i << 3;
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)pucEpubXorCode[(i << 3)]<<24) | ((unsigned int)pucEpubXorCode[(i << 3)+1]<<16) | ((unsigned int)pucEpubXorCode[(i << 3)+2]<<8)| ((unsigned int)pucEpubXorCode[(i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)pucEpubXorCode[(i << 3)+4]<<24) | ((unsigned int)pucEpubXorCode[(i << 3)+5]<<16) | ((unsigned int)pucEpubXorCode[(i << 3)+6]<<8)| ((unsigned int)pucEpubXorCode[(i << 3)+7]));
        temp_buf[3] = pucEpubXorCode[(k)];
        temp_buf[2] = pucEpubXorCode[k + 1];
        temp_buf[1] = pucEpubXorCode[k + 2];
        temp_buf[0] = pucEpubXorCode[k + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = pucEpubXorCode[k + 4];
        temp_buf[2] = pucEpubXorCode[k + 5];
        temp_buf[1] = pucEpubXorCode[k + 6];
        temp_buf[0] = pucEpubXorCode[k + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
    }

    //temp = (unsigned int)(((unsigned int)g_pucHdmiHdcp2Tx2Npub[124] <<24) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[125] <<16) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[126] <<8) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[127]));
    temp_buf[3] = g_pucHdmiHdcp2Tx2Npub[124];
    temp_buf[2] = g_pucHdmiHdcp2Tx2Npub[125];
    temp_buf[1] = g_pucHdmiHdcp2Tx2Npub[126];
    temp_buf[0] = g_pucHdmiHdcp2Tx2Npub[127];
    getVal = ScalerHdmiHdcp2Tx2NpinvCalculate(*((unsigned int *)temp_buf));
    // Load Npinv to RSA Module
    hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_NP_INV_reg, (unsigned int)getVal);
    if (HDCP22_TX_DBG_EN) {
        HDCPTX_WARIN("npinv=%x\n", hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_NP_INV_reg));
    }
    // Set RSA Write key done
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

    // Get Mod N Shift Counter
    nshiftcnt = ScalerHdmiHdcp2Tx2GetNShiftCounter(g_pucHdmiHdcp2Tx2Npub);
    hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_TX_FUNTION_reg, (unsigned int)nshiftcnt << 4);
    if (HDCP22_TX_DBG_EN) {
        HDCPTX_WARIN("nshift=%x\n", hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_TX_FUNTION_reg));
    }
    // Enable RSA Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT6 | _BIT5 | _BIT4 | _BIT0), (_BIT6 | _BIT5 | _BIT4));
    // Reset RSA Calcualte Enable to Start Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT4 | _BIT0), 0x00);

    do {

    } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg) & _BIT2) != _BIT2);
    // Disable RSA Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT7 | _BIT0), 0x00);
    // Disable RRMODN mode
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL1_reg, ~_BIT5, 0x00);
    // Read Out RSA Message
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT6 | _BIT5 | _BIT0), _BIT6);

    // Read RRmodN
    pBuf = &pucRrmodn[0];
    for (i = 0; i < 16; i++) {
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (0x30 + (15 - i)));

        getVal = hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg);
        *pBuf = *((unsigned char *)&getVal + 3);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal + 2);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal + 1);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal);
        pBuf++;

        getVal = hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg);
        *pBuf = *((unsigned char *)&getVal + 3);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal + 2);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal + 1);
        pBuf++;
        *pBuf = *((unsigned char *)&getVal);
        pBuf++;

        //pm_printk(LOGGER_ERROR,"rrmodn1024:%x,%x\n", (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg),(unsigned int)hdcp_in((HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg)));
    }

    // Generate Ekpub[km]

    memcpy(pucEpubXorCode, input_E_golden, 128);
    //ScalerHdcp2Proc3(pucEpubXorCode);
    // Enable HDCP 2.2 RSA Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT7 | _BIT6 | _BIT5 | _BIT0), (_BIT7 | _BIT5));
    // RSA Clock Select
    hdcp_mask(HDCP22_SPEEDUP_HDCP_DUMMY1_reg, ~_BIT7, _BIT7);
    // Reset HDCP 2.2 RSA module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), 0x00);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT3 | _BIT0), _BIT3);
    // Select RSA Key Download Data Length
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL1_reg, ~(_BIT7 | _BIT6 | _BIT5), _BIT7);

    // Load EM_km to RSA Module as Plain text
    for (i = 0; i < 16; i++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Enable write cipher text to RSA Module
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (unsigned char)(15 - i));
        k = i << 3;
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)pucEkpubKm[(i << 3)+1]<<24) | ((unsigned int)pucEkpubKm[(i << 3)+2]<<16) | ((unsigned int)pucEkpubKm[(i << 3)+3]<<8)| ((unsigned int)pucEkpubKm[(i << 3)+4]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)pucEkpubKm[(i << 3)+5]<<24) | ((unsigned int)pucEkpubKm[(i << 3)+6]<<16) | ((unsigned int)pucEkpubKm[(i << 3)+7]<<8)| ((unsigned int)pucEkpubKm[(i << 3)+8]));
        temp_buf[3] = pucEkpubKm[k + 1];
        temp_buf[2] = pucEkpubKm[k + 2];
        temp_buf[1] = pucEkpubKm[k + 3];
        temp_buf[0] = pucEkpubKm[k + 4];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = pucEkpubKm[k + 5];
        temp_buf[2] = pucEkpubKm[k + 6];
        temp_buf[1] = pucEkpubKm[k + 7];
        temp_buf[0] = pucEkpubKm[k + 8];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
    }

    for (i = 0; i < 16; i++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load RRmodN
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (unsigned char)(0x30 + (15 - i)));

        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)pucRrmodn[(i << 3)]<<24) | ((unsigned int)pucRrmodn[(i << 3)+1]<<16) | ((unsigned int)pucRrmodn[(i << 3)+2]<<8)| ((unsigned int)pucRrmodn[(i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)pucRrmodn[(i << 3)+4]<<24) | ((unsigned int)pucRrmodn[(i << 3)+5]<<16) | ((unsigned int)pucRrmodn[(i << 3)+6]<<8)| ((unsigned int)pucRrmodn[(i << 3)+7]));
        temp_buf[3] = pucRrmodn[(i << 3)];
        temp_buf[2] = pucRrmodn[(i << 3) + 1];
        temp_buf[1] = pucRrmodn[(i << 3) + 2];
        temp_buf[0] = pucRrmodn[(i << 3) + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = pucRrmodn[(i << 3) + 4];
        temp_buf[2] = pucRrmodn[(i << 3) + 5];
        temp_buf[1] = pucRrmodn[(i << 3) + 6];
        temp_buf[0] = pucRrmodn[(i << 3) + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
    }

    for (i = 0; i < 16; i++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load N Key
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (unsigned char)(0x90 + (15 - i)));

        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)]<<24) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+1]<<16) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+2]<<8)| ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+4]<<24) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+5]<<16) | ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+6]<<8)| ((unsigned int)g_pucHdmiHdcp2Tx2Npub[(i << 3)+7]));
        temp_buf[3] = g_pucHdmiHdcp2Tx2Npub[(i << 3)];
        temp_buf[2] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 1];
        temp_buf[1] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 2];
        temp_buf[0] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 4];
        temp_buf[2] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 5];
        temp_buf[1] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 6];
        temp_buf[0] = g_pucHdmiHdcp2Tx2Npub[(i << 3) + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
    }


    // Encrypted Epub Key
    for (i = 0; i < 3; i++) {
        pucEpubXorCode[125 + i] = pucEpubXorCode[125 + i] ^ g_pucHdmiHdcp2Tx2Epub[i];
    }

    for (i = 0; i < 16; i++) {
        hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

        // Load e Key
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (unsigned char)(0xC0 + (15 - i)));

        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, ((unsigned int)pucEpubXorCode[(i << 3)]<<24) | ((unsigned int)pucEpubXorCode[(i << 3)+1]<<16) | ((unsigned int)pucEpubXorCode[(i << 3)+2]<<8)| ((unsigned int)pucEpubXorCode[(i << 3)+3]));
        //hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, ((unsigned int)pucEpubXorCode[(i << 3)+4]<<24) | ((unsigned int)pucEpubXorCode[(i << 3)+5]<<16) | ((unsigned int)pucEpubXorCode[(i << 3)+6]<<8)| ((unsigned int)pucEpubXorCode[(i << 3)+7]));
        temp_buf[3] = pucEpubXorCode[(i << 3)];
        temp_buf[2] = pucEpubXorCode[(i << 3) + 1];
        temp_buf[1] = pucEpubXorCode[(i << 3) + 2];
        temp_buf[0] = pucEpubXorCode[(i << 3) + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN1_reg, *((unsigned int *)temp_buf));
        temp_buf[3] = pucEpubXorCode[(i << 3) + 4];
        temp_buf[2] = pucEpubXorCode[(i << 3) + 5];
        temp_buf[1] = pucEpubXorCode[(i << 3) + 6];
        temp_buf[0] = pucEpubXorCode[(i << 3) + 7];
        hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_IN0_reg, *((unsigned int *)temp_buf));
    }

    //// Get Npub Mod 2^32
    // memcpy(pData, &g_pucHdmiTx2Hdcp2Npub[124], 4);
    //
    //// Get Npinv
    // PDATA_DWORD(0) = ScalerHdmiTx2HDCP2NpinvCalculate(PDATA_DWORD(0));
    //
    //// Load Npinv to RSA Module
    ////memcpy((volatile BYTE xdata *)P63_12_HDCP_RSA_NP_INV_3, &g_stRxHdcp2DownLoadKeyType.pucHdcp2Npinv, 4);
    // memcpy((volatile BYTE xdata *)P63_12_HDCP_RSA_NP_INV_3, pData, 4);

    // Set RSA Write key done
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~_BIT0, _BIT0);

    // Enable RSA Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT6 | _BIT5 | _BIT4 | _BIT0), (_BIT6 | _BIT5 | _BIT4));

    // Reset RSA Calcualte Enable to Start Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT4 | _BIT0), 0x00);

    // Polling 100ms for RSA Module Done
    do {

    } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg) & _BIT2) != _BIT2);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT7 | _BIT0), 0x00);

    // Read Out RSA Message
    hdcp_mask(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg, ~(_BIT6 | _BIT5 | _BIT0), _BIT6);

    // Read Message From One of 2 Address

    if ((hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_CTRL0_reg) & _BIT1) == 0x00) {
        pBuf = &pucEkpubKm[1];
        for (i = 0; i < 16; i++) {
            hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (15 - i));

            getVal = hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg);
            *pBuf = *((unsigned char *)&getVal + 3);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 2);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 1);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal);
            pBuf++;

            getVal = hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg);
            *pBuf = *((unsigned char *)&getVal + 3);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 2);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 1);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal);
            pBuf++;

            //pm_printk(LOGGER_ERROR,"111RSA1024:%x,%x\n", (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg),(unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg));

        }
    }
    else {
        for (i = 0; i < 16; i++) {
            hdcp_out(HDCP22_SPEEDUP_HDCP_RSA_DATA_ADDR_reg, (0x30 + (15 - i)));

            getVal = hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg);
            *pBuf = *((unsigned char *)&getVal + 3);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 2);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 1);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal);
            pBuf++;

            getVal = hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg);
            *pBuf = *((unsigned char *)&getVal + 3);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 2);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal + 1);
            pBuf++;
            *pBuf = *((unsigned char *)&getVal);
            pBuf++;
            //pm_printk(LOGGER_ERROR,"111RSA1024:%x,%x\n", (unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT1_reg),(unsigned int)hdcp_in(HDCP22_SPEEDUP_HDCP_RSA_DATA_OUT0_reg));
        }
    }
#endif
    /************HDCP_HW_SPEED_UP END************/

    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("RealTX: Epub[km]:\n");
        for (i = 1; i < 129; i++) {
            HDCPTX_WARIN("%x,", (unsigned int)pucEkpubKm[i]);
        }
        HDCPTX_WARIN("\n");
    }

    // Write Ekpub[km]
    pucEkpubKm[0] = 0x04;

    //ScalerHdcp2Proc2(g_pucHdmiHdcp2Tx2Km, pucTemp);
    memcpy(pucMessageTx[port], pucEkpubKm, 129);
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    if (ScalerHdmiHdcp2Tx2WriteMessage(port, 129, pucMessageTx[port]) == _FALSE) {
        ret = _FALSE;
        goto no_storedkm_final;
    }
#endif
    if (GET_HDCP2_TX_TYPE_ERROR_STATUS() == HDCP_NO_STOREKM_FAIL) {
        ret = _FALSE;
        goto no_storedkm_final;
    }
no_storedkm_final:
    if (hdcptx_no_storedkm_tmp) {
        kfree(hdcptx_no_storedkm_tmp);
    }

    return ret;
}

//--------------------------------------------------
// Description  : Calculate Mod N Shift Counter
// Input Value  : None
// Output Value : 66 ~ 1089 => (1023 - (MSB No. of Kpubdcp-key) + 66)
//--------------------------------------------------

unsigned short ScalerHdmiHdcp2Tx2GetNShiftCounter(unsigned char *pucPubN)
{
    unsigned char i, j;
    unsigned char shift_off;
    unsigned short getVal;

    for (i = 0; i < 128; i++)  {
        shift_off = 0x80;
        for (j = 0; j < 8; j++) {
            if (pucPubN[i] & shift_off) {
                // return ((unsigned short)(i * 8) + j + 66);
                getVal = i * 8;
                getVal += (j + 66);
                return getVal;
            }
            shift_off = shift_off >> 1;
        }
    }
    return 0x0441;
}

//--------------------------------------------------
// Description  : Npinv Calculate by Euclidean Algorithm
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned int ScalerHdmiHdcp2Tx2NpinvCalculate(unsigned int ulA)
{
    volatile unsigned int ulM = 0xFFFFFFFF; // 2^32 - 1
    volatile unsigned int ulE = ulM - ulA + 1;
    volatile unsigned int ulX = 0;
    volatile unsigned int ulY = 1;
    volatile unsigned int ulx = 1;
    volatile unsigned int uly = 1;
    volatile unsigned int temp0;
    volatile unsigned int temp1;

    while (ulE != 0) {
        //HDCPTX_WARIN("RealTX: ulM=%d", ulM);
        //HDCPTX_WARIN("RealTX: ulE=%d", ulE);

        temp0 = ulM / ulE;
        temp1 = ulM % ulE;

        if (ulM == 0xFFFFFFFF) {
            temp1++;
        }

        ulM = ulE;
        ulE = temp1;

        temp1 = ulY;

        ulY *= temp0;

        if (ulx == uly) {
            if (ulX >= ulY) {
                ulY = ulX - ulY;
            }
            else {
                ulY -= ulX;
                uly = 0;
            }
        }
        else {
            ulY += ulX;
            ulx = 1 - ulx;
            uly = 1 - uly;
        }

        ulX = temp1;
    }

    if (ulx == 0) {
        ulX = 0xFFFFFFFF - ulX + 1;
    }
    return ulX;
}

//--------------------------------------------------
// Description  : Stored Km Procss
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2StoredKmProc(unsigned char port)
{

    unsigned char temp_buff[33];

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    // msg_id
    temp_buff[0] = 0x05;

    // Ekh[km]
    memcpy(&temp_buff[1], &g_pucHdmiHdcp2Tx2PairingEkhkm[port][0], 16);

    // M
    memcpy(&temp_buff[17], &g_pucHdmiHdcp2Tx2PairingM[port][0], 16);

#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    if (ScalerHdmiHdcp2Tx2WriteMessage(port, 33, temp_buff) == _FALSE) {
        return _FALSE;
    }
#endif
    if (GET_HDCP2_TX_TYPE_ERROR_STATUS() == HDCP_STOREKM_FAIL) {
        return _FALSE;
    }
    return _TRUE;
}

//--------------------------------------------------
// Description  : Locality Check
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2LocalityCheck(unsigned char port)
{
    unsigned char i = 0;
    unsigned char pucL[32] = {0};
    unsigned char pucTemp1[16];
    unsigned char pucTemp2[16];
    unsigned char pucHMACSHA256Input[64];
    unsigned char pucHMACSHA256CipherInput[64];
#ifdef HDCP_HW_SPEED_UP
    unsigned int temp_data;
    unsigned char temp_buf[4];
#endif
    unsigned short usReadLength;
    unsigned char pucRn[8];

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }
    //ScalerHdcp2Proc2(pucTemp1, g_pucHdmiHdcp2Tx2dKey0);

    //ScalerHdcp2Proc2(pucTemp2, g_pucHdmiHdcp2Tx2dKey1);
    memcpy(pucTemp1, g_pucHdmiHdcp2Tx2dKey0[port], 16);
    memcpy(pucTemp2, g_pucHdmiHdcp2Tx2dKey1[port], 16);

    if ((GET_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port) == _FALSE) && (GET_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port) == _FALSE)) {
        SET_HDMI_HDCP2_TX2_LC_ALREADY_SENT(port);
        SET_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port);

        // Generate Rn
        if (Hdcp2AesRandomGen(port, pucRn, 8) == _FALSE) {
            return _FALSE;
        }
        if (hdcp2tx_func_test) {
            memcpy(pucRn, tx_rn_test, 8);
        }
        memcpy(g_pucHdmiHdcp2Tx2Rn[port], pucRn, 8);
        // Write Rn
        pucMessageTx[port][0] = 0x09;
        memcpy(&pucMessageTx[port][1], g_pucHdmiHdcp2Tx2Rn[port], 8);

#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
        if (ScalerHdmiHdcp2Tx2WriteMessage(port, 9, pucMessageTx[port]) == _FALSE) {
            return _FALSE;
        }
#endif
        start_time[port] = rtk_timer_misc_90k_ms();
        //core_timer_event_addTimerEvent(HDCP_VERIFY_L_TIMEOUT, MSG_HDCPTX_ID, _HDCP_2_2_TX_STATE_LOCALITY_CHECK << 8 | port, 0);
    }

    if (GET_HDMI_HDCP2_TX2_L_PRIME_TIMEOUT(port) == _TRUE) {
        // Compute L
        memset(pucHMACSHA256Input, 0, sizeof(pucHMACSHA256Input));
        memset(pucHMACSHA256CipherInput, 0, sizeof(pucHMACSHA256CipherInput));
        memcpy(pucHMACSHA256CipherInput, pucTemp1, 16);
        for (i = 16; i < 32; i++) {
            //pucHMACSHA256CipherInput[i] = *(pucTemp2 + i - 16);
            pucHMACSHA256CipherInput[i] = pucTemp2[i - 16];
            if (i > 23) {
                pucHMACSHA256CipherInput[i] = pucHMACSHA256CipherInput[i] ^ (g_pucHdmiHdcp2Tx2Rrx[port][i - 24]);
            }
        }

        if (hdcp2tx_cal_log_print) {
            for (i = 0; i < 8; i++) {
                HDCPTX_WARIN("rn:%x,", (unsigned int)pucRn[i]);
            }
            HDCPTX_WARIN("\n");
            for (i = 0; i < 8; i++) {
                HDCPTX_WARIN("g_pucHdmiHdcp2Tx2Rrx:%x,", (unsigned int)g_pucHdmiHdcp2Tx2Rrx[0][i]);
            }
            HDCPTX_WARIN("\n");
            for (i = 0; i < 16; i++) {
                HDCPTX_WARIN("g_pucHdmiHdcp2Tx2dKey0:%x,", (unsigned int)pucTemp1[i]);
            }
            HDCPTX_WARIN("\n");
            for (i = 0; i < 16; i++) {
                HDCPTX_WARIN("g_pucHdmiHdcp2Tx2dKey1:%x,", (unsigned int)pucTemp2[i]);
            }
            HDCPTX_WARIN("\n");
            for (i = 0; i < 32; i++) {
                HDCPTX_WARIN("pucHMACSHA256CipherInput:%x,", (unsigned int)pucHMACSHA256CipherInput[i]);
            }
            HDCPTX_WARIN("\n");
        }

#ifndef HDCP_HW_SPEED_UP
        // do HMAC-SHA256(rn, Kd XOR rrx)
        hmacsha256(pucHMACSHA256CipherInput, 32, pucRn, 8, pucL);
#else
        memset(&pucHMACSHA256CipherInput[32], 0, 32);
        memcpy(pucHMACSHA256Input, g_pucHdmiHdcp2Tx2Rn[port], 8);

        pucHMACSHA256Input[8] = 0x80;
        memset(&pucHMACSHA256Input[9], 0, 53);
        pucHMACSHA256Input[62] = 0x02;
        pucHMACSHA256Input[63] = 0x40;
        Hdcp2HmacSha256Calculate(port, pucHMACSHA256Input, pucHMACSHA256CipherInput, 1);

        // Get L
        for (i = 0; i < 32; i += 4) {
            temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_7_reg + i);
            pucL[0 + i] = *((unsigned char *)&temp_data + 3);
            pucL[1 + i] = *((unsigned char *)&temp_data + 2);
            pucL[2 + i] = *((unsigned char *)&temp_data + 1);
            pucL[3 + i] = *((unsigned char *)&temp_data + 0);
        }
#endif
    }

    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("RealTX prime:\n");
        for (i = 0; i < 32; i++) {
            HDCPTX_WARIN("%x,", (unsigned int)pucL[i]);
        }
        HDCPTX_WARIN("\n");
    }
    //  memcpy(g_pucL[port], pucL, 32);
    // Read L' and Compare L' with L
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    usReadLength = 33;
    if (ScalerHdmiHdcp2Tx2ReadMessage(port, usReadLength, g_rx_msg_buf[port], VERIFY_LPRIME_TIMEOUT) == _FALSE) {
        HDCPTX_WARIN("[HDCP2TX]read failed\n");
        return _FALSE;
    }
#endif

    if (hdcp2tx_func_test) {
        memcpy(g_rx_msg_buf[port], pucMessage_LC_SEND_L_PRIME, 33);
    }
    if (g_rx_msg_buf[port][0] != LC_SEND_L_PRIME) {
        HDCPTX_WARIN("RealTX: LC FAIL 0\n");
        return _FALSE;
    }

    for (i = 0; i < 32; i++) {
        if (pucL[i] != g_rx_msg_buf[port][1 + i]) {
            HDCPTX_WARIN("RealTX: LC FAIL 1\n");
            return _FALSE;
        }
    }

    HDCPTX_WARIN("[HDCP2TX]RealTX: LC PASS\n");
    return _TRUE;
}

void HDCP2TxCipherEncrypt(unsigned char port, unsigned char *ksArray, unsigned char *lc128Array, unsigned char *rivArray)
{
    unsigned char i, ucI;
    //hdcp2 temp has clean in previous api
    unsigned char pucTemp[6] = {0};
    unsigned char tmpks[16] = {0};
    unsigned char  tmplc128[16] = {0};
    unsigned char tucEncData[16] = {
        0x52, 0x45, 0x41, 0x4C, 0x54, 0x45, 0x4B, 0x53,
        0x45, 0x4D, 0x49, 0x43, 0x4f, 0x4E, 0x44, 0x55,
    };

    for (ucI = 0; ucI < 16; ucI++) {
        pucTemp[0] = (ksArray[ucI] & 0x80) +
                     ((ksArray[ucI] & 0x01) << 6) +
                     (((ksArray[ucI] & 0x01) << 5) ^ ((ksArray[ucI] & 0x02) << 4)) +
                     ((ksArray[ucI] & 0x10) ^ 0x10) +
                     ((ksArray[ucI] & 0x40) >> 3) +
                     (ksArray[ucI] & 0x04) +
                     (((ksArray[ucI] & 0x20) >> 4) ^ ((ksArray[ucI] & 0x04) >> 1)) +
                     ((ksArray[ucI] & 0x08) >> 3);

        if ((ucI % 4) == 0) {
            pucTemp[1] = ((pucTemp[0] & 0x01) << 7) + ((pucTemp[0] & 0xFE) >> 1);
        }
        else if ((ucI % 4) == 1) {
            pucTemp[1] = ((pucTemp[0] & 0x07) << 5) + ((pucTemp[0] & 0xF8) >> 3);
        }
        else if ((ucI % 4) == 2) {
            pucTemp[1] = ((pucTemp[0] & 0x1F) << 3) + ((pucTemp[0] & 0xE0) >> 5);
        }
        else if ((ucI % 4) == 3) {
            pucTemp[1] = ((pucTemp[0] & 0x7F) << 1) + ((pucTemp[0] & 0x80) >> 7);
        }

        pucTemp[2] = pucTemp[1] ^ tucEncData[ucI];

        // memcpy(g_stRxHdcp2DownLoadKeyType.pucHdcp2Lc, g_stucHDCP2_LCKey_test, 16);

        pucTemp[3] = (lc128Array[ucI] & 0x80) +
                     ((lc128Array[ucI] & 0x01) << 6) +
                     (((lc128Array[ucI] & 0x01) << 5) ^ ((lc128Array[ucI] & 0x02) << 4)) +
                     ((lc128Array[ucI] & 0x10) ^ 0x10) +
                     ((lc128Array[ucI] & 0x40) >> 3) +
                     (lc128Array[ucI] & 0x04) +
                     (((lc128Array[ucI] & 0x20) >> 4) ^ ((lc128Array[ucI] & 0x04) >> 1)) +
                     ((lc128Array[ucI] & 0x08) >> 3);

        if ((ucI % 4) == 0) {
            pucTemp[4] = ((pucTemp[3] & 0x01) << 7) + ((pucTemp[3] & 0xFE) >> 1);
        }
        else if ((ucI % 4) == 1) {
            pucTemp[4] = ((pucTemp[3] & 0x07) << 5) + ((pucTemp[3] & 0xF8) >> 3);
        }
        else if ((ucI % 4) == 2) {
            pucTemp[4] = ((pucTemp[3] & 0x1F) << 3) + ((pucTemp[3] & 0xE0) >> 5);
        }
        else if ((ucI % 4) == 3) {
            pucTemp[4] = ((pucTemp[3] & 0x7F) << 1) + ((pucTemp[3] & 0x80) >> 7);
        }

        pucTemp[5] = pucTemp[4] ^ tucEncData[ucI];

        // Set Ks to Cipher
        //ScalerHdmiHdcp2Tx0SetKsValue(ucI, pucTemp[2]);
        tmpks[ucI] = pucTemp[2];
        tmplc128[ucI] = pucTemp[5];

        // Set LC to Cipher
        //ScalerHdmiHdcp2Tx0SetLcValue(ucI, pucTemp[5]);
    }

    for (i = 0; i < 16; i++) {
        if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) {
            hdcp_out((HDMITX21_MAC_HDCP22_ENGINE_15_reg - i * 4), tmpks[i]);
        }
        else {
            if (i < 9) {
                hdcp_out((HDMITX20_MAC0_HDCP22_ENGINE_15_reg - i * 4), tmpks[i]);
            }
            else {
                hdcp_out((HDMITX20_MAC0_HDCP22_ENGINE_15_reg - i * 4 - 4), tmpks[i]);
            }
        }
    }
    for (i = 0; i < 8; i++) {
        if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) {
            hdcp_out((HDMITX21_MAC_HDCP22_ENGINE_39_reg - i * 4), rivArray[i]);
        }
        else {
            hdcp_out((HDMITX20_MAC0_HDCP22_ENGINE_39_reg - i * 4), rivArray[i]);
        }
    }


    for (i = 0; i < 16; i++) {

        if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) {
            if (i < 2) {
                hdcp_out((HDMITX21_MAC_HDCP22_ENGINE_31_reg - i * 4), tmplc128[i]);
            }
            else {
                hdcp_out((HDMITX21_MAC_HDCP22_ENGINE_31_reg - i * 4 - 8), tmplc128[i]);
            }

        }
        else {
            hdcp_out((HDMITX20_MAC0_HDCP22_ENGINE_31_reg - i * 4), tmplc128[i]);
        }
    }
}


//--------------------------------------------------
// Description  : Session Key Exchange Proc
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2SessionKeyExchangeProc(unsigned char port)
{
    volatile unsigned char ucI = 0;
    unsigned char i = 0;
    unsigned char pucdKey2[16];
    unsigned char pucdRiv[8];
    //unsigned char pucMessageTemp[25];

#ifdef HDCP_HW_SPEED_UP
    unsigned char pucTemp1[16];
#endif
    unsigned char pucTemp2[16];
    unsigned char pucKs[16];
    unsigned char ksArray[16];
    unsigned char lc128Array[16];

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    //ScalerHdcp2Proc1(pucTemp1, g_stHdcp2RxDownLoadKeyType.pucHdcp2Lc);

    //ScalerHdcp2Proc2(pucTemp2, g_pucHdmiHdcp2Tx2Km);
    memcpy(pucTemp2, g_pucHdmiHdcp2Tx2Km[port], 16);

    // Generate Ks(128 bits)
    if (Hdcp2AesRandomGen(port, pucKs, 16) == _FALSE) {
        return _FALSE;
    }

    hdcp2_tx_get_lc128(lc128Array);

    if (hdcp2tx_func_test) {
        memcpy(pucKs, tx_ks_test, 16);
        memcpy(lc128Array, hdcp_22_tx_key_lc128_test, 16);
    }
    memcpy(ksArray, pucKs, 16);

#ifndef HDCP_HW_SPEED_UP
    RCP_HDCP2_GenDKey(g_pucHdmiHdcp2Tx2Km[port], g_pucHdmiHdcp2Tx2Rtx[port], g_pucHdmiHdcp2Tx2Rrx[port], g_pucHdmiHdcp2Tx2Rn[port], 2, pucdKey2, 1);
#else
    // Generate dkey2
    //ScalerHdcp2DkeyCalculate(_HDCP_2_2_DKEY_2, g_pucHdmiHdcp2Tx2Rtx, g_pucHdmiHdcp2Tx2Rrx, pucTemp2, g_pucHdmiHdcp2Tx2Rn);

    memcpy(pucTemp1, g_pucHdmiHdcp2Tx2Rtx[port], 8);
    memcpy(&pucTemp1[8], g_pucHdmiHdcp2Tx2Rrx[port], 7);
    pucTemp1[15] = g_pucHdmiHdcp2Tx2Rrx[port][7] ^ 0x02;
    memcpy(pucTemp2, g_pucHdmiHdcp2Tx2Km[port], 8);
    for (ucI = 8; ucI < 16; ucI++) {
        pucTemp2[ucI] = (g_pucHdmiHdcp2Tx2Km[port][ucI]) ^ (g_pucHdmiHdcp2Tx2Rn[port][ucI - 8]);
    }

    Hdcp2AesCalculate_out(port, pucTemp1, pucTemp2, pucdKey2);  //reduce imem, use hdcp2temp[80] replace pucdKey2

    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_3_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_2_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_1_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_DATA_IN_0_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_3_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_2_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_1_reg,  0);
    hdcp_out(HDCP22_SPEEDUP_HDCP_AES_KEY_0_reg,  0);
#endif

    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("RealTX: dkey2:\n");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucdKey2[i]);
        }
        HDCPTX_WARIN("\n");
    }
    // Compute Edkey(ks)
    // LSB 64bits of dkey2 ^ Rrx
    for (ucI = 0; ucI < 8; ucI++) {
        pucdKey2[ucI + 8] = g_pucHdmiHdcp2Tx2Rrx[port][ucI] ^ pucdKey2[ucI + 8];
    }

    // Edkey(ks) = ks ^ (dkey2 ^ Rrx)
    for (ucI = 0; ucI < 16; ucI++) {
        pucKs[ucI] = pucKs[ucI] ^ pucdKey2[ucI];
        if (hdcp2tx_cal_log_print) {
            HDCPTX_WARIN("%x ", (unsigned int)pucKs[ucI]);
        }
    }
    HDCPTX_WARIN("\n");

    // Write Edkey(ks)
    memcpy(&pucMessageTx[port][1], pucKs, 16);

    //pucdRiv = &hdcp2_temp[96]; //16+16+16+16+16+16
    // Generate riv(64 bits)
    if (Hdcp2AesRandomGen(port, pucdRiv, 8) == _FALSE) {
        return _FALSE;
    }
    if (hdcp2tx_func_test) {
        memcpy(pucdRiv, tx_riv_test, 8);
    }
    else {
        HDCP2TxCipherEncrypt(port, ksArray, lc128Array, pucdRiv);
    }
    // Write riv
    memcpy(&pucMessageTx[port][17], pucdRiv, 8);

    // Write SKE_Send_Eks
    pucMessageTx[port][0] = 0x0B;
    // MCH 20170104 modify
#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    if (ScalerHdmiHdcp2Tx2WriteMessage(port, 25, pucMessageTx[port]) == _FALSE) {
        return _FALSE;
    }
#endif

    if (hdcp2tx_func_test) {
        HDCPTX_WARIN("func test can't set real enc reg\n");
    }
    else {
        if (GET_HDMI_MAC_TX_MODE_STATE(port) == _HDMI_TX_MODE_STATUS_HDMI21_LINK_ON) {
            hdcp_mask(HDMITX21_MAC_HDCP22_ENGINE_40_reg, ~(_BIT6 | _BIT1 | _BIT0), _BIT1 | _BIT0);
            hdcp_mask(HDMITX21_MAC_HDCP22_ENGINE_40_reg, ~(_BIT6 | _BIT1 | _BIT0), (_BIT6 | _BIT1 | _BIT0));
        }
        else {
            hdcp_mask(HDMITX20_MAC1_HDCP_AES_CTRL_reg, ~_BIT3, _BIT3);
            hdcp_out(HDMITX20_MAC0_HDCP14_ENGINE_7_reg, 0x80);
            hdcp_mask(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, ~(_BIT6 | _BIT1 | _BIT0), _BIT1 | _BIT0);
            hdcp_mask(HDMITX20_MAC0_HDCP22_ENGINE_40_reg, ~(_BIT6 | _BIT1 | _BIT0), (_BIT6 | _BIT1 | _BIT0));
        }
    }
    // Reset AES Cipher & release Hdcp Reset to Normal
    //ScalerHdmiHdcp2Tx2ResetAesCipherFrameCnt();
    if (GET_HDCP2_TX_TYPE_ERROR_STATUS() == HDCP_SKE_PROC_FAIL) {
        return _FALSE;
    }
    return _TRUE;
}

//--------------------------------------------------
// Description  : V Verification
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2VerifyVprime(unsigned char port, unsigned char bIsFirst)
{
    unsigned char i = 0;
    unsigned char ret = _TRUE;
    unsigned char pucRxInfo[2];
    unsigned char pucSeqNumV[3];
    unsigned char pucV[32];
    unsigned char pucTemp1[16];
    unsigned char pucTemp2[16];
    unsigned char pucSHA256Input[64];
    unsigned char *pucHMACSHA256Input = NULL; //pucHMACSHA256Input[192];
    //unsigned char pucHdmiTx2Hdcp2ReceiverIdList[160];
    unsigned char pucHMACSHA256CipherInput[64];
#ifdef HDCP_HW_SPEED_UP
    unsigned short temp;
    unsigned char  temp_buf[4];
    unsigned char j = 0;
    unsigned int temp_data;
#endif
    unsigned char uctemp;
    unsigned char *pucMessageTemp = NULL; //pucMessageTemp[177];
    unsigned char *verify_vprime_buffer = NULL;
    //unsigned short usReadLength = 0;

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    verify_vprime_buffer = kmalloc(192 + 177, GFP_KERNEL); //kernal space
    if (!verify_vprime_buffer) {
        ret = _FALSE;
        goto verify_vprime_final;
    }
    pucHMACSHA256Input = verify_vprime_buffer;
    pucMessageTemp = &verify_vprime_buffer[192];

    //memset(pucHdmiTx2Hdcp2ReceiverIdList, 0, sizeof(pucHdmiTx2Hdcp2ReceiverIdList));
    memset(pucMessageTemp, 0x00, 177);
    memset(pucHMACSHA256Input, 0x00, 192);
    memset(pucSHA256Input, 0x00, 64);
    memset(pucHMACSHA256CipherInput, 0x00, 64);

    HDCPTX_WARIN("RealTX: In check V\n");
    memcpy(pucTemp1, g_pucHdmiHdcp2Tx2dKey0[port], 16);
    memcpy(pucTemp2, g_pucHdmiHdcp2Tx2dKey1[port], 16);

    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("verify v dkey0:");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucTemp1[i]);
        }

        HDCPTX_WARIN("\n");
        HDCPTX_WARIN("verify v dkey1:");

        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucTemp2[i]);
        }
        HDCPTX_WARIN("\n");
    }


#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    //usReadLength = 37;  //teamp_value, follow 0x70 ReadLength
    //mark retry 3000 ,because use tiemevent to control the time, retry set 250
    if (ScalerHdmiHdcp2Tx2ReadMessage(port, 27, &pucMessageTemp[0], 250) == _FALSE) {
        ret = _FALSE;
        goto verify_vprime_final;
    }
#endif


    if (hdcp2tx_func_test) {
        //usReadLength = 37;
        memcpy(pucMessageTemp, pucMessage_REPEATERAUTH_SEND_RECEIVERID_LIST, 37);
    }

    if (pucMessageTemp[0] != 0x0C) {
        HDCPTX_WARIN("[HDCP2TX] message id erro\n");
        ret = _FALSE;
        goto verify_vprime_final;
    }
    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("verify v ksv list\n");
        for (i = 0; i < 37; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucMessageTemp[i]);
            if ((i + 1) % 16 == 0) {
                HDCPTX_WARIN("\n");
            }
        }
    }
    if ((pucMessageTemp[2] & _BIT3) == _BIT3) {
        SET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT_EXCEED(port);
    }
    else {
        CLR_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT_EXCEED(port);
    }

    if ((pucMessageTemp[2] & _BIT2) == _BIT2) {
        SET_HDMI_HDCP2_TX2_RXINFO_DEPTH_EXCEED(port);
    }
    else {
        CLR_HDMI_HDCP2_TX2_RXINFO_DEPTH_EXCEED(port);
    }
    uctemp = ((pucMessageTemp[1] & _BIT0) << 4) | (pucMessageTemp[2] & 0xF0) >> 4;
    // Set Rx Info
    SET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port, uctemp);
    SET_HDMI_HDCP2_TX2_RXINFO_DEPTH(port, (pucMessageTemp[1] & 0x0E) >> 1);

    if ((GET_HDMI_HDCP2_TX2_RXINFO_HDCP20_REPEATER_DOWNSTREAM(port) == _FALSE) && ((pucMessageTemp[2] & _BIT1) == _BIT1)) {
        SET_HDMI_HDCP2_TX2_RXINFO_HDCP20_REPEATER_DOWNSTREAM(port);
    }

    if ((pucMessageTemp[2] & _BIT0) == _BIT0) {
        SET_HDMI_HDCP2_TX2_RXINFO_HDCP1_DOWNSTREAM(port);
    }
    else {
        CLR_HDMI_HDCP2_TX2_RXINFO_HDCP1_DOWNSTREAM(port);
    }


    //g_usHdmiHdcp2Tx2KsvFifoIndex = g_usHdmiTxHdcpKsvFifoIndex;
    //g_usHdmiTxHdcpKsvFifoIndex += GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) * 5;

    // Prepare ReceiverID List and Sink's ReceiverID
    if(GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) * 5 <= 31*5){
        memcpy(pucHdmiTx2Hdcp2ReceiverIdList, &pucMessageTemp[22], GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) * 5);
        memcpy(&pucHdmiTx2Hdcp2ReceiverIdList[GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) * 5],  g_pucHdmiHdcp2Tx2ReceiverId[port], 5);
    }else{
	    HDCPTX_WARIN("DEVICE COUNT is EXCEED:%d\n", GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port));
        memcpy(pucHdmiTx2Hdcp2ReceiverIdList, &pucMessageTemp[22], 31*5);
        memcpy(&pucHdmiTx2Hdcp2ReceiverIdList[155],  g_pucHdmiHdcp2Tx2ReceiverId[port], 5);
    }

    if (GET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port) == _FALSE) {
        // Set Global Ksv list Start Index & Length
        //g_usHdmiMacTx2HdcpKsvFifoStartIndex = g_usDpHdcpRx0ReceiverIdListIndex;
        //g_usHdmiMacTx2HdcpKsvFifoLength = (GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT() + 1) * 5;
        repeater_temp[0] = BSTATUS_DEVICE_COUNT(GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) + 1) | BSTATUS_MAX_DEVS_EXCEEDED(GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT_EXCEED(port)) | BSTATUS_MAX_CASCADE_EXCEEDED(GET_HDMI_HDCP2_TX2_RXINFO_DEPTH_EXCEED(port)) | BSTATUS_DEPTH(GET_HDMI_HDCP2_TX2_RXINFO_DEPTH(port) + 1) | BSTATUS_HDMI_MODE(1); //hdmi mode bit12(0x1000)
        repeater_temp[1] = ((pucMessageTemp[1] << 8 | pucMessageTemp[2]) & ~(0xFF0)) | RX_INFO_DEPTH(GET_HDMI_HDCP2_TX2_RXINFO_DEPTH(port) + 1) | RX_INFO_DEVICE_COUNT(GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) + 1);
        if ((GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) + 1) == 32) {
            HDCPTX_WARIN("[HDCP2TX]begin repeater_temp[1] %d\n", (unsigned int)repeater_temp[1]);
            repeater_temp[1] |= 0x100;
        }
        HdcpTxStoredReceiverId(pucHdmiTx2Hdcp2ReceiverIdList, GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT(port) + 1);
        SET_HDMI_MAC_TX2_HDCP_REPORT_RID_LIST(port);
    }

    if ((GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT_EXCEED(port) == _TRUE) ||
            (GET_HDMI_HDCP2_TX2_RXINFO_DEPTH_EXCEED(port) == _TRUE)) {
        //SET_HDMI_HDCP2_TX2_DOWNSTREAM_EVENT(_HDMI_TX_HDCP2_DOWNSTREAM_AUTH_EVENT_DEVICE_DEPTH_EXCEED);
        HDCPTX_WARIN("[HDCP2TX]DEVICE COUNT/DEPTH EXCEED\n");
        ret = _FALSE;
        goto verify_vprime_final;
    }


    // Read Rxinfo
    memcpy(pucRxInfo, &pucMessageTemp[1], 2);

    // Read seq_bum_V
    memcpy(pucSeqNumV, &pucMessageTemp[3], 3);
    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("verify v rxinfo\n");
        for (i = 0; i < 2; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucRxInfo[i]);
        }

        HDCPTX_WARIN("\n");
        HDCPTX_WARIN("verify v Seq\n");
        for (i = 0; i < 3; i++) {
            HDCPTX_WARIN("%x ", (unsigned int)pucSeqNumV[i]);
        }

        HDCPTX_WARIN("\n");
    }
    if (((pucSeqNumV[0] != 0x00) || (pucSeqNumV[1] != 0x00) || (pucSeqNumV[2] != 0x00)) && (bIsFirst == _TRUE)) {
        ret = _FALSE;
        goto verify_vprime_final;
    }

    if (((pucSeqNumV[0] == 0x00) || (pucSeqNumV[1] == 0x00) || (pucSeqNumV[2] == 0x00)) && (bIsFirst == _FALSE)) {
        ret = _FALSE;
        goto verify_vprime_final;
    }

    if ((g_pucHdmiHdcp2Tx2SeqNumV[0] > pucSeqNumV[0]) && (g_pucHdmiHdcp2Tx2SeqNumV[1] > pucSeqNumV[1]) && (g_pucHdmiHdcp2Tx2SeqNumV[2] > pucSeqNumV[2]) && (bIsFirst == _FALSE)) {
        ret = _FALSE;
        goto verify_vprime_final;
    }

    //memcpy(&g_pucHdmiTxHdcpKsvFifo[g_usHdmiHdcp2Tx2KsvFifoIndex], &pucMessageTemp[22], GET_HDMI_HDCP2_TX2_RXINFO_DEVICE_COUNT() * 5);

#ifndef HDCP_HW_SPEED_UP
    memcpy(pucHMACSHA256CipherInput, pucTemp1, 16);
    memcpy(&pucHMACSHA256CipherInput[16], pucTemp2, 16);

    i = ((*pucRxInfo & _BIT0) << 4 | (*(pucRxInfo + 1) >> 4)) * 5;
    memcpy(pucHMACSHA256Input, &pucMessageTemp[22], i);  //for reduce desg(pucHMACSHA256Input, &hdcp2_temp[177])
    memcpy(&pucHMACSHA256Input[i], pucRxInfo, 2);
    memcpy(&pucHMACSHA256Input[i + 2], pucSeqNumV, 3);

    hmacsha256(pucHMACSHA256CipherInput, 32, pucHMACSHA256Input, i + 2 + 3, pucV);
#else
    // Compute V = HMAC-SHA256(Receiver ID List || RxInfo || seq_num_V, kd)
    // V = HMAC-SHA256(Receiver ID List || RxInfo || seq_num_V, kd)
    // HMAC(text) = HMAC(K, text) = SHA256((K0 XOR opad) || SHA256((KO XOR ipad) || text))
    // Calculate SHA256((KO XOR ipad) || text)
    memcpy(pucHMACSHA256CipherInput, pucTemp1, 16);
    memcpy(&pucHMACSHA256CipherInput[16], pucTemp2, 16);
    memset(&pucHMACSHA256CipherInput[32], 0, 32);

    i = ((*pucRxInfo & _BIT0) << 4 | (*(pucRxInfo + 1) >> 4)) * 5;
    j = i + 14;
    j = ((j <= 64) ? 1 : ((j <= 128) ? 2 : 3));
    repeater_temp[0] = (69 + i) * 8;
#if 0
    memcpy(&hdcp2_temp[177], &pucMessageTemp[22], i);  //for reduce desg(pucHMACSHA256Input, &hdcp2_temp[177])
    memcpy(&hdcp2_temp[177 + i], pucRxInfo, 2);
    memcpy(&hdcp2_temp[177 + i + 2], pucSeqNumV, 3);
    hdcp2_temp[177 + i + 5] = 0x80;
    hdcp2_temp[177 + (j * 64) - 2] = *((unsigned char *)&repeater_temp[0] + 1);
    hdcp2_temp[177 + (j * 64) - 1] = *((unsigned char *)&repeater_temp[0]);
#endif
    memcpy(pucHMACSHA256Input, &pucMessageTemp[22], i);  //for reduce desg(pucHMACSHA256Input, &hdcp2_temp[177])
    memcpy(&pucHMACSHA256Input[i], pucRxInfo, 2);
    memcpy(&pucHMACSHA256Input[i + 2], pucSeqNumV, 3);
    pucHMACSHA256Input[i + 5] = 0x80;
    pucHMACSHA256Input[(j * 64) - 2] = *((unsigned char *)&repeater_temp[0] + 1);
    pucHMACSHA256Input[(j * 64) - 1] = *((unsigned char *)&repeater_temp[0]);

    // Read MSB of V'
    //memcpy(pData, &pucMessageTemp[6], 16);
    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("\nSHA256Input");
        for (i = 0; i < 64; i++) {
            HDCPTX_WARIN(" %x", (unsigned int)pucHMACSHA256Input[i]);
        }
        HDCPTX_WARIN("\n");
        HDCPTX_WARIN("\nSHAchiper256Input");
        for (i = 0; i < 64; i++) {
            HDCPTX_WARIN(" %x", (unsigned int)pucHMACSHA256CipherInput[i]);
        }
        HDCPTX_WARIN("\n");
    }
    if (Hdcp2HmacSha256Calculate(port, pucHMACSHA256Input, pucHMACSHA256CipherInput, j) == _TRUE) {
        // Get V
        for (i = 0; i < 32; i += 4) {
            temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_7_reg + i);
            pucV[i + 0] = *((unsigned char *)&temp_data + 3);
            pucV[i + 1] = *((unsigned char *)&temp_data + 2);
            pucV[i + 2] = *((unsigned char *)&temp_data + 1);
            pucV[i + 3] = *((unsigned char *)&temp_data);
        }
    }
#endif
    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("\nRealTX: V");
        for (i = 0; i < 32; i++) {
            HDCPTX_WARIN(" %x", (unsigned int)pucV[i]);
        }
        HDCPTX_WARIN("\n");
        HDCPTX_WARIN("\nRead: V");
        for (i = 0; i < 16; i++) {
            HDCPTX_WARIN(" %x", (unsigned int)pucMessageTemp[6 + i]);
        }
        HDCPTX_WARIN("\n");
    }
    for (i = 0; i < 16; i++) {
        if (pucV[i] != pucMessageTemp[6 + i]) {
            HDCPTX_WARIN("RealTX: V' FAIL\n");
            ret = _FALSE;
            goto verify_vprime_final;
        }
    }

    // Write LSB of V
    pucMessageTx[port][0] = 0x0F;
    memcpy(&pucMessageTx[port][1], &pucV[16], 16);

#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    if (ScalerHdmiHdcp2Tx2WriteMessage(port, 17, pucMessageTx[port]) == _FALSE) {
        ret = _FALSE;
        goto verify_vprime_final;
    }
#endif

    g_pucHdmiHdcp2Tx2SeqNumV[0] = pucSeqNumV[0];
    g_pucHdmiHdcp2Tx2SeqNumV[1] = pucSeqNumV[1];
    g_pucHdmiHdcp2Tx2SeqNumV[2] = pucSeqNumV[2];

    HDCPTX_WARIN("RealTX: V' PASS\n");

verify_vprime_final:
    if (verify_vprime_buffer) {
        kfree(verify_vprime_buffer);
    }
    return ret;
}



//--------------------------------------------------
// Description  : Content Stream Management
// Input Value  : None
// Output Value : None
//--------------------------------------------------
unsigned char ScalerHdmiHdcp2Tx2ContentStreamManagement(unsigned char port)
{
    unsigned char i = 0;
#ifdef HDCP_HW_SPEED_UP
    unsigned char j = 0;
    unsigned short temp;
    unsigned char  uctemp;
    unsigned char temp_buf[4];
    unsigned int temp_data;
#endif
    unsigned char pucK[2] = {0x00, 0x01};
    unsigned char pucStreamIDType[2] = {0x00, 0x01};
    //unsigned char pucMessageTemp[8];
    unsigned char pucHdmiTx2Hdcp2SeqNumM[3];
    unsigned char pucTemp1[16];
    unsigned char pucTemp2[16];
    unsigned char pucTemp3[32];
    unsigned char *pucHMACSHA256Input = NULL; //pucHMACSHA256Input[192];
    unsigned char *pucHMACSHA256CipherInput = NULL; //pucHMACSHA256CipherInput[64];
    unsigned char *pucSHA256Input = NULL; //pucSHA256Input[64];
    unsigned char *content_stream_buffer = NULL;
    unsigned char ret = _TRUE;

    if(port >= HDMI_TX_PORT_MAX_NUM){
        HDCPTX_ERR("%s: port not correct:%u", __func__, port);
        return _FALSE;
    }

    content_stream_buffer = kmalloc(192 + 64 + 64, GFP_KERNEL); //kernal space
    if (!content_stream_buffer) {
        ret = _FALSE;
        goto content_stream_final;
    }
    pucHMACSHA256Input = content_stream_buffer;
    pucHMACSHA256CipherInput = &content_stream_buffer[192];
    pucSHA256Input = &content_stream_buffer[192 + 64];

    pucMessageTx[port][0] = 0x10;

    memset(pucHMACSHA256CipherInput, 0x00, SHA256_DIGEST_HDCP2_SIZE);
    if (hdcp2tx_func_test) {
        g_ulHdmiHdcp2Tx2SeqNumM = 0x000000;
    }
    pucMessageTx[port][1] = *((unsigned char *)&g_ulHdmiHdcp2Tx2SeqNumM + 2);
    pucMessageTx[port][2] = *((unsigned char *)&g_ulHdmiHdcp2Tx2SeqNumM + 1);
    pucMessageTx[port][3] = *((unsigned char *)&g_ulHdmiHdcp2Tx2SeqNumM);
#if 0 //for reduce dseg
    pucMessageTx[port][1] = LOBYTE((unsigned short)(((unsigned int)g_ulHdmiHdcp2Tx2SeqNumM >> 16) & 0xffff));
    pucMessageTx[port][2] = HIBYTE((unsigned short)((unsigned int)(g_ulHdmiHdcp2Tx2SeqNumM) & 0xffff));
    pucMessageTx[port][3] = LOBYTE((unsigned short)((unsigned int)(g_ulHdmiHdcp2Tx2SeqNumM) & 0xffff));
#endif
    memcpy(&pucMessageTx[port][4], pucK, 2);
    memcpy(&pucMessageTx[port][6], pucStreamIDType, 2);

    //ScalerHdcp2Proc2(pucTemp1, g_pucHdmiHdcp2Tx2dKey0);

    //ScalerHdcp2Proc2(pucTemp2, g_pucHdmiHdcp2Tx2dKey1);
    memcpy(pucTemp1, g_pucHdmiHdcp2Tx2dKey0[port], 16); //for reduce dseg(&hdcp2_temp[352], pucTemp1)
    memcpy(pucTemp2, g_pucHdmiHdcp2Tx2dKey1[port], 16); //for reduce dseg(&hdcp2_temp[352], pucTemp2)

#ifdef HDCP2_TX_I2C_HANDLE_MESSAGE
    if (ScalerHdmiHdcp2Tx2WriteMessage(port, 8, pucMessageTx[port]) == _FALSE) {
        ret = _FALSE;
        goto content_stream_final;
    }
#endif


    pucHdmiTx2Hdcp2SeqNumM[0] = g_ulHdmiHdcp2Tx2SeqNumM >> 16;
    pucHdmiTx2Hdcp2SeqNumM[1] = g_ulHdmiHdcp2Tx2SeqNumM >> 8;
    pucHdmiTx2Hdcp2SeqNumM[2] = g_ulHdmiHdcp2Tx2SeqNumM;


#ifndef HDCP_HW_SPEED_UP
    memcpy(pucHMACSHA256CipherInput, pucTemp1, 16);
    memcpy(&pucHMACSHA256CipherInput[16], pucTemp2, 16);

    Sha256(pucHMACSHA256CipherInput, 32, pucSHA256Input);

    // Number of StreamID_Type
    i = *(pucK + 1) * 2;
    memcpy(pucHMACSHA256Input, pucStreamIDType, i);
    memcpy(&pucHMACSHA256Input[i], pucHdmiTx2Hdcp2SeqNumM, 3);

    hmacsha256(pucSHA256Input, 32, pucHMACSHA256Input, i + 3, pucTemp3);

#else
    // M = HMAC-SHA256(StreamID_Type || seq_num_M, SHA256(kd))
    // HMAC(text) = HMAC(K, text) = SHA256((K0 XOR opad) || SHA256((KO XOR ipad) || text))
    // Calculate SHA256((KO XOR ipad) || text)

    memcpy(pucHMACSHA256CipherInput, pucTemp1, 16);
    memcpy(&pucHMACSHA256CipherInput[16], pucTemp2, 16);
    memset(&pucHMACSHA256CipherInput[32], 0, 32);
    pucHMACSHA256CipherInput[32] = 0x80;
    pucHMACSHA256CipherInput[62] = 0x01;
    pucHMACSHA256CipherInput[63] = 0x00;

    // Set 1, Then Setj 0 for Reset SHA256 Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), _BIT3);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), 0x00);
    // Write SHA256 Input Data, Total 512 bits
    for (i = 0; i < 64; i += 4) {
        //hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, ((unsigned int)pucHMACSHA256CipherInput[i * 4]<<24) | ((unsigned int)pucHMACSHA256CipherInput[1 + i * 4]<<16) | ((unsigned int)pucHMACSHA256CipherInput[2 + i * 4]<<8)| ((unsigned int)pucHMACSHA256CipherInput[3 + i * 4]));
        temp_buf[3] = pucHMACSHA256CipherInput[i ];
        temp_buf[2] = pucHMACSHA256CipherInput[i + 1];
        temp_buf[1] = pucHMACSHA256CipherInput[i + 2];
        temp_buf[0] = pucHMACSHA256CipherInput[i + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, *((unsigned int *)temp_buf));
        // Inform HW SHA data Has been set first 32 bit
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~_BIT0, _BIT0);
    }
    // Set First Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);
    // Enable SHA256 Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT0), _BIT2);
    // Polling 10ms for HDCP Sha Done
    do {

    } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg) &_BIT4) != _BIT4);
    hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, 0);

    // Get SHA256(kd)
    for (i = 0; i < 32; i += 4) {
        temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_7_reg + i);
        pucHMACSHA256CipherInput[i + 0] = *((unsigned char *)&temp_data + 3);
        pucHMACSHA256CipherInput[i + 1] = *((unsigned char *)&temp_data + 2);
        pucHMACSHA256CipherInput[i + 2] = *((unsigned char *)&temp_data + 1);
        pucHMACSHA256CipherInput[i + 3] = *((unsigned char *)&temp_data);

    }
    memset(&pucHMACSHA256CipherInput[32], 0, 32);

    // Number of StreamID_Type
    i = *(pucK + 1) * 2;

    // Number of (StreamID_Type + seq_num_M + 0x80 + Bit Length of SHA256)
    j = i + 12;

    // Amount of SHA256 block
    j = ((j <= 64) ? 1 : ((j <= 128) ? 2 : 3));

    // Bit length of SHA256
    repeater_temp[0] = (67 + i) * 8;

    //for reduce dseg(hdcp2_temp,pucHMACSHA256Input)
    memcpy(pucHMACSHA256Input, pucStreamIDType, i);
    memcpy(&pucHMACSHA256Input[i], pucHdmiTx2Hdcp2SeqNumM, 3);
    pucHMACSHA256Input[i + 3] = 0x80;
    pucHMACSHA256Input[(j * 64) - 2] = *((unsigned char *)&repeater_temp[0] + 1);
    pucHMACSHA256Input[(j * 64) - 1] = *((unsigned char *)&repeater_temp[0] + 0);

    for (i = 0; i < 64; i++) {
        // K0 XOR ipad = 512 bit
        *(pucHMACSHA256CipherInput + i) = *(pucHMACSHA256CipherInput + i) ^ 0x36;
    }

    // Calculate SHA256((K0 XOR ipad) || text)
    // Set 1, Then Setj 0 for Reset SHA256 Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), _BIT3);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), 0x00);
    // Write SHA256 Input Data, Total 512 bits
    for (i = 0; i < 64; i += 4) {
        //hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, ((unsigned int)pucHMACSHA256CipherInput[i * 4]<<24) | ((unsigned int)pucHMACSHA256CipherInput[1 + i * 4]<<16) | ((unsigned int)pucHMACSHA256CipherInput[2 + i * 4]<<8)| ((unsigned int)pucHMACSHA256CipherInput[3 + i * 4]));
        temp_buf[3] = pucHMACSHA256CipherInput[i ];
        temp_buf[2] = pucHMACSHA256CipherInput[i + 1];
        temp_buf[1] = pucHMACSHA256CipherInput[i + 2];
        temp_buf[0] = pucHMACSHA256CipherInput[i + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, *((unsigned int *)temp_buf));
        // Inform HW SHA data Has been set first 32 bit
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~_BIT0, _BIT0);
    }
    // Set First Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);
    // Enable SHA256 Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT0), _BIT2);
    // Polling 10ms for HDCP Sha Done
    do {

    } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg) &_BIT4) != _BIT4);
    hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, 0);

    uctemp = 0;

    do {
        memcpy(pucSHA256Input, (pucHMACSHA256Input + (64 * uctemp)), 64);

        // Set 1, Then Setj 0 for Reset SHA256 Module
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), _BIT3);
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), 0x00);
        // Write SHA256 Input Data, Total 512 bits
        for (i = 0; i < 64; i += 4) {
            //hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, ((unsigned int)pucSHA256Input[i * 4]<<24) | ((unsigned int)pucSHA256Input[1 + i * 4]<<16) | ((unsigned int)pucSHA256Input[2 + i * 4]<<8)| ((unsigned int)pucSHA256Input[3 + i * 4]));
            temp_buf[3] = pucSHA256Input[i ];
            temp_buf[2] = pucSHA256Input[i + 1];
            temp_buf[1] = pucSHA256Input[i + 2];
            temp_buf[0] = pucSHA256Input[i + 3];
            hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, *((unsigned int *)temp_buf));
            // Inform HW SHA data Has been set first 32 bit
            hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~_BIT0, _BIT0);
        }
        // Disable First Calculate
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
        // Enable SHA256 Calculate
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT0), _BIT2);
        // Polling 10ms for HDCP Sha Done
        do {

        } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg) &_BIT4) != _BIT4);
        hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, 0);

        uctemp++;
    } while (uctemp < j);


    // KO XOR opad
    for (i = 0; i < 64; i++) {
        *(pucHMACSHA256CipherInput + i) = (*(pucHMACSHA256CipherInput + i) ^ 0x36) ^ 0x5C;
    }

    for (i = 0; i < 32; i += 4) {
        temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_7_reg + i);
        pucHMACSHA256Input[i + 0] = *((unsigned char *)&temp_data + 3);
        pucHMACSHA256Input[i + 1] = *((unsigned char *)&temp_data + 2);
        pucHMACSHA256Input[i + 2] = *((unsigned char *)&temp_data + 1);
        pucHMACSHA256Input[i + 3] = *((unsigned char *)&temp_data);
    }
    memset(pucHMACSHA256Input + 32, 0, 32);
    *(pucHMACSHA256Input + 32) = 0x80;
    *(pucHMACSHA256Input + 62) = 0x03;


    // SHA256((KO XOR opad) || SHA256(KO XOR ipad) || text)
    // Set 1, Then Setj 0 for Reset SHA256 Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), _BIT3);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), 0x00);
    // Write SHA256 Input Data, Total 512 bits
    for (i = 0; i < 64; i += 4) {
        //hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, ((unsigned int)pucHMACSHA256CipherInput[i * 4]<<24) | ((unsigned int)pucHMACSHA256CipherInput[1 + i * 4]<<16) | ((unsigned int)pucHMACSHA256CipherInput[2 + i * 4]<<8)| ((unsigned int)pucHMACSHA256CipherInput[3 + i * 4]));
        temp_buf[3] = pucHMACSHA256CipherInput[i ];
        temp_buf[2] = pucHMACSHA256CipherInput[i + 1];
        temp_buf[1] = pucHMACSHA256CipherInput[i + 2];
        temp_buf[0] = pucHMACSHA256CipherInput[i + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, *((unsigned int *)temp_buf));
        // Inform HW SHA data Has been set first 32 bit
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~_BIT0, _BIT0);
    }
    // Set First Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);
    // Enable SHA256 Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT0), _BIT2);
    // Polling 10ms for HDCP Sha Done
    do {

    } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg) &_BIT4) != _BIT4);
    hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, 0);


    // Set 1, Then Setj 0 for Reset SHA256 Module
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), _BIT3);
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT3 | _BIT0), 0x00);

    // Write SHA256 Input Data, Total 512 bits
    for (i = 0; i < 64; i += 4) {
        //hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, ((unsigned int)pucHMACSHA256Input[i * 4]<<24) | ((unsigned int)pucHMACSHA256Input[1 + i * 4]<<16) | ((unsigned int)pucHMACSHA256Input[2 + i * 4]<<8)| ((unsigned int)pucHMACSHA256Input[3 + i * 4]));
        temp_buf[3] = pucHMACSHA256Input[i ];
        temp_buf[2] = pucHMACSHA256Input[i + 1];
        temp_buf[1] = pucHMACSHA256Input[i + 2];
        temp_buf[0] = pucHMACSHA256Input[i + 3];
        hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, *((unsigned int *)temp_buf));

        // Inform HW SHA data Has been set first 32 bit
        hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~_BIT0, _BIT0);
    }
    // Disable First Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
    // Enable SHA256 Calculate
    hdcp_mask(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg, ~(_BIT2 | _BIT0), _BIT2);
    // Polling 10ms for HDCP Sha Done
    do {

    } while ((hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_CONTRL_reg) &_BIT4) != _BIT4);
    hdcp_out(HDCP22_SPEEDUP_HDCP_SHA_DATA_reg, 0);

    // Get M
    for (i = 0; i < 32; i += 4) {
        temp_data = hdcp_in(HDCP22_SPEEDUP_HDCP_SHA_7_reg + i);
        pucTemp3[i + 0] = *((unsigned char *)&temp_data + 3);
        pucTemp3[i + 1] = *((unsigned char *)&temp_data + 2);
        pucTemp3[i + 2] = *((unsigned char *)&temp_data + 1);
        pucTemp3[i + 3] = *((unsigned char *)&temp_data);
    }
#endif
    if (hdcp2tx_cal_log_print) {
        HDCPTX_WARIN("RealTX: M \n");
        for (i = 0; i < 32; i++) {
            HDCPTX_WARIN(" %x", (unsigned int)pucTemp3[i]);
        }
        HDCPTX_WARIN("\n");
    }
    //ScalerHdcp2Proc2(g_pucHdmiHdcp2Tx2M, pucTemp3);

    //ScalerHdcp2Proc2(g_pucHdmiHdcp2Tx2M + 16, pucTemp3 + 16);
    memcpy(g_pucHdmiHdcp2Tx2M, pucTemp3, 32);

content_stream_final:
    if (content_stream_buffer) {
        kfree(content_stream_buffer);
    }
    return ret;
}


//--------------------------------------------------
// Description  : Content Stream Management Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------

void ScalerHdmiHdcp2Tx2ContentStreamManagementHandler(unsigned char port)
{
    unsigned char b1Managed = 0;

    // Content Stream Management
    if (GET_HDMI_HDCP2_TX2_CONTENT_STREAM_MANAGEMENT(port) == _FALSE) {
        if ((GET_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port) == _TRUE) &&
                (GET_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port) == _TRUE) &&
                (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) > _HDCP_2_2_TX_STATE_SKE)) {
            if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _TRUE) {
                ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
                HDCPTX_WARIN("[HDCP2TX]StreamManagemen - Disable Encrypt\n");
            }
            if (GET_HDMI_HDCP2_TX2_REPEATER_SUPPORTED(port) == _TRUE) {
                //delayms(700);
                //ScalerTimerActiveTimerEvent(SEC(0.7), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_TO_SEND_M_MSG);
                SET_HDMI_HDCP2_TX2_WRITE_M_MSG_TIMEOUT(port);
                if (GET_HDMI_HDCP2_TX2_WRITE_M_MSG_TIMEOUT(port) == _TRUE) {
                    b1Managed = 1;
                    HDCPTX_WARIN("[HDCP2TX]RealTX: CSM - HDCP 22\n");

                    CLR_HDMI_HDCP2_TX2_WRITE_M_MSG_TIMEOUT(port);

                    if (ScalerHdmiHdcp2Tx2ContentStreamManagement(port) == _TRUE) {
                        //ScalerTimerActiveTimerEvent(SEC(0.1), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_TO_READ_M_PRIME);
                        SET_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port);
                    }
                    else {
                        HDCPTX_WARIN("[HDCP2TX]RealTX: M Preset fail\n");
                    }
                }
            }
            else {
#if 0//(_HDMI_MAC_TX2_SUPPORT_HDMI21 == _ON)
                if ((GET_HDMI_MAC_TX2_STREAM_SOURCE() == _HDMI_TX_SOURCE_HDMI) ||
                        ((GET_HDMI_MAC_TX2_STREAM_SOURCE() == _HDMI_TX_SOURCE_HDMI21) && (ScalerHdmi21MacTxGetVideoStatus(ScalerHdmiMacTxPxMapping(_TX2)) == _TRUE)))
#endif
                {
                    // Set up 200ms Timer Event
                    //ScalerTimerActiveTimerEvent(SEC(0.2), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_TO_ENCRYPT_DATA);
                    b1Managed = 1;
                }
            }
        }
        else if ((GET_HDMI_HDCP2_TX2_CAPABLE_SUPPORTED(port) == _TRUE) &&
                 (GET_HDMI_HDCP2_TX2_VERSION_SUPPORTED(port) == _FALSE) &&
                 (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) > _HDCP_2_2_TX_STATE_SKE)) {
            b1Managed = 1;
            HDCPTX_WARIN("[HDCP2TX]RealTX: CSM - HDCP 20\n");

            // HDCP 2.0-compliant can't support content stream management
            if (ScalerHdmiMacTx2GetHdcpEncStatus(port) == _TRUE) {
                ScalerHdmiMacTx2HdcpEncryptSignalProc(port, 0);
                HDCPTX_WARIN("[HDCP2TX]RealTX: HDCP2 - Disable Encrypt\n");
            }

            /*
            // Disable AES-Ctrl Cipher
            hdcp_mask(PBB_90_HDCP22_CTRL, ~_BIT1, 0x00);

            // Reset type value of Tx_STx(x = 1~4)
            hdcp_mask(PBB_90_HDCP22_CTRL, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), 0x00);

            if(GET_REG_DP_RX_MST_ENABLE() == _TRUE)
            {
                i = 0;

                switch(GET_DP_TX_HDCP2_DPCD_INFO(0x06, 0x93, 0xF4) - 1)
                {
                    case 3:
                        // RX in MST Mode
                        i += (BYTE)GET_DP_TX_HDCP2_RX_STX_TYPE(GET_REG_DP_RX_MST_ST5_SOURCE_STATUS());
                    case 2:
                        // RX in MST Mode
                        i += (BYTE)GET_DP_TX_HDCP2_RX_STX_TYPE(GET_REG_DP_RX_MST_ST4_SOURCE_STATUS());
                    case 1:
                        // RX in MST Mode
                        i += (BYTE)GET_DP_TX_HDCP2_RX_STX_TYPE(GET_REG_DP_RX_MST_ST3_SOURCE_STATUS());
                        break;

                    default:
                        break;
                }
            }
            else
            {
                // RX in SST Mode
                i = GET_DP_TX_HDCP2_DPCD_INFO(0x06, 0x93, 0xF6);
            }

            if(i == 0x00)
            {
                // Enable AES-Ctrl Cipher
                hdcp_mask(PBB_90_HDCP22_CTRL, ~_BIT1, _BIT1);

                // Set up 200ms Timer Event
                ScalerMiscTimerActiveTimerEvent(SEC(0.2), _SCALER_TIMER_EVENT_DP_TX_HDCP2_WAITING_TO_ENCRYPT_DATA);
            }
            else
            {
               HDCPTX_WARIN("RealTX: Type = 1");

                SET_DP_TX_HDCP2_STOP_CONTENT_STREAM();
            }
            */

            HDCPTX_WARIN("RealTX: Type = 1\n");
        }

        if (b1Managed == 1) {
            SET_HDMI_HDCP2_TX2_CONTENT_STREAM_MANAGEMENT(port);
        }
    }

    if (GET_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port) == _TRUE) {
        CLR_HDMI_HDCP2_TX2_M_PRIME_TIMEOUT(port);

        if (GET_HDMI_HDCP2_TX2_AUTH_STATE(port) > _HDCP_2_2_TX_STATE_SKE) {
            if (ScalerHdmiHdcp2Tx2VerifyMprime(port) == _TRUE) {
                // Set up 200ms Timer Event
                //ScalerTimerActiveTimerEvent(SEC(0.2), _SCALER_TIMER_EVENT_HDMI_TX2_HDCP2_WAITING_TO_ENCRYPT_DATA);
            }
            else {
                HDCPTX_WARIN("RealTX: M Verify fail\n");

                // seq_num_M roll-over detected
                if (GET_HDMI_HDCP2_TX2_SEQUENCE_NUMBER_M(port) == 0xFFFFFF) {
                    ScalerHdmiHdcp2Tx2ChangeAuthState(port, _HDCP_2_2_TX_STATE_IDLE);
                }
                else {
                    ADD_HDMI_HDCP2_TX2_SEQUENCE_NUMBER_M(port);

                    CLR_HDMI_HDCP2_TX2_CONTENT_STREAM_MANAGEMENT(port);
                }
            }
        }
    }

}
//--------------------------------------------------
// Description  : HDCP2 Read Message
// Input Value  : None
// Output Value : _TRUE or _FALSE
//--------------------------------------------------


