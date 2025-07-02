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
// ID      : ScalerHdcp2TxTest.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------
#ifndef  _SCALERHDCP2TXTEST_H_
#define  _SCALERHDCP2TXTEST_H_

extern unsigned char tx_rtx_test[8];
extern unsigned char pucMessage_AKE_SEND_CERT[534];
extern unsigned char tx_modulus_n_test[384];
extern unsigned char tx_modulus_e_test[1];
extern unsigned char tx_Npinv_test[4];
extern unsigned char tx_modules_lc128[16];
extern unsigned char tx_seed_test[32];
extern unsigned char tx_km_test[16];
extern unsigned char pucMessage_AKE_SEND_H_PRIME[33];
extern unsigned char pucMessage_AKE_SEND_PAIRING_INFO[17];
extern unsigned char tx_rn_test[8];
extern unsigned char pucMessage_LC_SEND_L_PRIME[33];
extern unsigned char pucMessage_REPEATERAUTH_SEND_RECEIVERID_LIST[37];
extern unsigned char pucMessage_REPEATERAUTH_STREAM_READY[33];
extern unsigned char tx_modulus_rrmodn_test[384];
extern unsigned char tx_ks_test[16];
extern unsigned char tx_riv_test[8];
extern unsigned char  rx_rrx_test[8];
extern unsigned char rx_rxcaps_test[3];
extern unsigned char hdcp_22_rx_key_test[816+16];

extern unsigned char rx_cert_key_test[522];
extern unsigned char  hdcp_22_tx_key_lc128_test[16];
extern unsigned char hdcp_22_tx_key_e_test[1];
extern unsigned char hdcp_22_tx_key_n_test[384];

extern unsigned char hdcp2tx_func_test;
extern unsigned char hdcp2tx_read_log_print;
extern unsigned char hdcp2tx_cal_log_print;
#endif
