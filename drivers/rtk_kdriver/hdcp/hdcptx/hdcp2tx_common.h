/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2011>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdcp2TxInterface.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#ifndef  _SCALERHDCP2TXINCLUDE_H_
#define  _SCALERHDCP2TXINCLUDE_H_
#include <rtk_kdriver/hdcp/hdcptx/hdmi_hdcp2_tx.h>
#include "hdcptx_common.h"


#define HDCP2_TX_I2C_HANDLE_MESSAGE
#define HDCP_DEFAULT_KEY
//#define HDCP_HW_SPEED_UP //only dante/bridge need

#define _HDMI_TX2_HDCP2_AUTH_START_TIME                  0
#define HDCP22_TX_DBG_EN            0


//#define LOAD_TX_KEY_FROM_FLASH
#ifdef HDCP_HW_SPEED_UP
    #ifdef HDCP_DEFAULT_KEY
        extern unsigned char code hdcp_22_tx_key[800 + 16];
    #else
        //extern const code __at (0x1DFC00) unsigned char hdcp_22_tx_key[880];
        //extern SPI_DEV_DATA hdcptx_pri_spi_data;
        extern unsigned char hdcp_22_tx_key[880];
    #endif
#else
    extern unsigned char hdcp_22_tx_key[445];
#endif


extern unsigned char tx_key_align_offset;
#define LC128_SIZE          (16)
#ifdef HDCP_HW_SPEED_UP
    #define LC128_KEY_START     (tx_key_align_offset)
#else
    #define LC128_KEY_START     (4) //shift header
#endif
#define N_SIZE          (384)
#define N_KEY_START     (tx_key_align_offset +16) // (key_alin_offset  + LC128_SIZE)
#define RRMODN_SIZE          (384)
#define RRMODN_KEY_START     (tx_key_align_offset +400)
#define NPINV_SIZE          (4)
#define NPINV_KEY_START     (tx_key_align_offset + 784)
#define E_SIZE          (1)
#define E_KEY_START     (tx_key_align_offset + 788)

#define hdcp2_tx_get_lc128(tx_lc128)           memcpy(tx_lc128, &hdcp_22_tx_key[LC128_KEY_START], LC128_SIZE)

#ifdef HDCP_HW_SPEED_UP
    #define hdcp2_tx_get_n(tx_modulus_n)         memcpy(tx_modulus_n, &hdcp_22_tx_key[N_KEY_START], N_SIZE)
    #define hdcp2_tx_get_RRmodN(tx_modulus_rrmodn)                 memcpy(tx_modulus_rrmodn, &hdcp_22_tx_key[RRMODN_KEY_START], RRMODN_SIZE)
    #define hdcp2_tx_get_Npinv(tx_Npinv)         memcpy(tx_Npinv, &hdcp_22_tx_key[NPINV_KEY_START], NPINV_SIZE)
    #define hdcp2_tx_get_e(tx_modulus_e)         memcpy(tx_modulus_e, &hdcp_22_tx_key[E_KEY_START], E_SIZE)
#endif
//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
//--------------------------------------------------
// Definitions of HDCP 2.2 Load Key Prpcess
//--------------------------------------------------
#define _HDCP2_TX_KEY_SIGNATURE                 1
#define _HDCP2_TX_KEY_RRMODN                    2
#define _HDCP2_TX_KEY_DCP_LLC_N                 3
#define _HDCP2_TX_KEY_EKEY1                     4
#define _HDCP2_TX_KEY_EKEY2                     5
#define _HDCP2_TX_KEY_NPINV                     6

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// VARIABLE EXTERN
//****************************************************************************

extern unsigned char g_pucHdmiHdcp2Tx2TxCaps[HDMI_TX_PORT_MAX_NUM][3];
extern unsigned char g_pucHdmiHdcp2Tx2ReceiverId[HDMI_TX_PORT_MAX_NUM][5];
extern unsigned char g_pucHdmiHdcp2Tx2RxCaps[HDMI_TX_PORT_MAX_NUM][3];
extern unsigned char g_pucHdmiHdcp2Tx2Rtx[HDMI_TX_PORT_MAX_NUM][8];
extern unsigned char g_pucHdmiHdcp2Tx2Rrx[HDMI_TX_PORT_MAX_NUM][8];
extern unsigned char g_pucHdmiHdcp2Tx2Npub[128];
extern unsigned char g_pucHdmiHdcp2Tx2Epub[3];
extern unsigned char g_pucHdmiHdcp2Tx2Km[HDMI_TX_PORT_MAX_NUM][16];
extern unsigned char g_pucHdmiHdcp2Tx2Hdcp2H[HDMI_TX_PORT_MAX_NUM][32];
extern unsigned char g_pucHdmiHdcp2Tx2Rn[HDMI_TX_PORT_MAX_NUM][8];
extern unsigned char g_pucHdmiHdcp2Tx2dKey0[HDMI_TX_PORT_MAX_NUM][16];
extern unsigned char g_pucHdmiHdcp2Tx2dKey1[HDMI_TX_PORT_MAX_NUM][16];

extern unsigned char g_pucHdmiHdcp2Tx2PairingEkhkm[HDMI_TX_PORT_MAX_NUM][16];
extern unsigned char g_pucHdmiHdcp2Tx2PairingM[HDMI_TX_PORT_MAX_NUM][16];
extern unsigned char g_pucHdmiHdcp2Tx2PairingKm[HDMI_TX_PORT_MAX_NUM][16];
extern unsigned char g_pucHdmiHdcp2Tx2PairingReceiverID[HDMI_TX_PORT_MAX_NUM][5];

extern unsigned char g_pucHdmiHdcp2Tx2SeqNumV[3];
extern unsigned int g_ulHdmiHdcp2Tx2SeqNumM;
extern unsigned char g_pucHdmiHdcp2Tx2M[32];
extern unsigned short g_usHdmiHdcp2Tx2KsvFifoIndex;

extern unsigned short g_usHdmiMacTxHdcpKsvFifoIndex;
extern unsigned char pucMessageTx[HDMI_TX_PORT_MAX_NUM][150];

//****************************************************************************
// FUNCTION EXTERN
//****************************************************************************
extern unsigned char ScalerHdmiHdcp2Tx2ComputeH(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2AkeInitial(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2AkeTransInfo(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2VerifySignature(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2NoStoredKmProc(unsigned char port);
extern unsigned short ScalerHdmiHdcp2Tx2GetNShiftCounter(unsigned char *pucPubN);
extern unsigned int ScalerHdmiHdcp2Tx2NpinvCalculate(unsigned int ulA);
extern unsigned char ScalerHdmiHdcp2Tx2StoredKmProc(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2StorePairingInfo(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2VerifyHprime(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2CheckPairingInfo(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2LocalityCheck(unsigned char port);
extern unsigned char ScalerHdcp2txLprimeCheck(unsigned char port, unsigned int *p_start_time);
extern unsigned char ScalerHdmiHdcp2Tx2SessionKeyExchangeProc(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2VerifyVprime(unsigned char port, unsigned char bIsFirst);
extern unsigned char ScalerHdmiHdcp2Tx2ContentStreamManagement(unsigned char port);
extern unsigned char ScalerHdmiHdcp2Tx2VerifyMprime(unsigned char port);
extern void HDCP2TxCipherEncrypt(unsigned char port, unsigned char *ksArray, unsigned char *lc128Array, unsigned char *rivArray);

#endif
