#ifndef __HDCP2_RCP_API_H__
#define __HDCP2_RCP_API_H__
#include <rtk_kdriver/mcp/rtk_rcp.h>
#if IS_ENABLED(CONFIG_RTK_KDRV_MCP)
#define RTK_RCP_SET_CW_HDCP2(id, pCW, len)  RTK_RCP_SET_CW((id+1), (unsigned char*)pCW, len)
#define RTK_RCP_GET_CW_HDCP2(id, pCW, len)  RTK_RCP_GET_CW((id+1), (unsigned char*)pCW, len)
#else
#define RTK_RCP_SET_CW_HDCP2(id, pCW, len) 
#define RTK_RCP_GET_CW_HDCP2(id, pCW, len) 
#endif
/*//////////////////////// Functions for HDCP2 /////////////////////////////*/
//void RCP_HDCP2_EkhKm(int KhCw, int KmCw, unsigned char *Rtx,
//                     unsigned char *pEkhKm);
void RCP_HDCP2_GenDKey(unsigned char *Km, unsigned char *Rtx, unsigned char *Rrx,
                       unsigned char *Rn, unsigned char ctr, unsigned char *DKey,
                       int modeHDCP22);
void RCP_HDCP2_GenKd(unsigned char *Km, unsigned char *Rtx, unsigned char *Rrx, unsigned char *Rn, unsigned char *Kd, int modeHDCP22);
void RCP_HDCP2_GenKs(unsigned char * dKey2CW, unsigned char *EdKeyKs,
                     unsigned char *rRx, unsigned char * KsCW);
void RCP_HDCP2_GenKsXorLc128(const uint8_t *s1, const uint8_t *s2, uint8_t *d, int len);
void RCP_HDCP2_DataDecrypt(int KsXorLs128CW,  long DatadecryptoCW, unsigned char *key, unsigned char *pcounter);
#endif
