/***************************************************************************************************
  File        : rcp_api.cpp
  Description : Low Level API for RCP
  Author      : Kevin Wang
****************************************************************************************************
    Update List :
----------------------------------------------------------------------------------------------------
    20090605    | Create Phase
***************************************************************************************************/

#include <hdcp/hdcp2_2/hdcp2_hal.h>
#include <hdcp/hdcp2_2/hdcp2_rcp_api.h>
#include <mach/system.h>
#include <rtd_log/rtd_module_log.h>

#ifdef CONFIG_ARM64
#include <tvscalercontrol/io/ioregdrv.h>
#define rtd_outl(x, y)     								IoReg_Write32(x,y)
#define rtd_inl(x)     									IoReg_Read32(x)
#endif

void RCP_HDCP2_GenKd(unsigned char *Km, unsigned char *Rtx, unsigned char *Rrx, unsigned char *Rn, unsigned char *Kd, int modeHDCP22)
{
    RCP_HDCP2_GenDKey(Km, Rtx, Rrx, Rn, 0, Kd, modeHDCP22);
    RCP_HDCP2_GenDKey(Km, Rtx, Rrx, Rn, 1, Kd + 16, modeHDCP22);
}



void RCP_HDCP2_GenDKey(unsigned char *Km, unsigned char *Rtx, unsigned char *Rrx,
                       unsigned char *Rn, unsigned char ctr, unsigned char *DKey,
                       int modeHDCP22)
{
    unsigned char tmp[16];
    unsigned char ctr_rrx_out[8];

    memset(ctr_rrx_out, 0x0, 8);
    rtd_pr_hdcp_debug("compute ctr = %d,dkeycw= %ld\n", ctr, DKey[0]);

    /* Generate Km^Rn in DKeyCW   */
    memset(tmp, 0, 16);
    memcpy(&tmp[8], Rn, 8);
    xor_array(Km, tmp, DKey, 16);
    memset(tmp, 0, 16);
    memcpy(tmp, Rtx, 8);
    if (modeHDCP22 == 0)
        tmp[15] = ctr;

    else {
        unsigned char ctr_array[8];
        memset(ctr_array, 0, 8);
        ctr_array[7] = ctr;
        xor_array(Rrx, ctr_array, ctr_rrx_out, 8);
        memcpy(tmp + 8, ctr_rrx_out, 8);
    }
    /* Generate Kd */
    RTK_RCP_AES_ECB_Encryption(DKey, tmp, DKey, 16);
}

void RCP_HDCP2_DataDecrypt(int KsXorLs128CW,  long DatadecryptoCW, unsigned char *key, unsigned char *pcounter)
{
    RTK_RCP_SET_CW_HDCP2(DatadecryptoCW , key, 16);
    RTK_RCP_AES_ECB_Encryption((unsigned char *)(DatadecryptoCW + 1), pcounter, (unsigned char *)(DatadecryptoCW + 1), 16);
}

void RCP_HDCP2_EkhKm(long KhCw, long KmCw, unsigned char *Rtx,
                     unsigned char *pEkhKm)
{
    memset(pEkhKm, 0, 16);
    memcpy(pEkhKm, Rtx, 8);

    RTK_RCP_AES_ECB_Encryption((unsigned char *)(KhCw + 1), pEkhKm, pEkhKm, 16);

    RTK_RCP_AES_ECB_Encryption((unsigned char *)(KhCw + 1), pEkhKm, pEkhKm, 16);

    RTK_RCP_AES_CBC_Decryption((unsigned char *)(KhCw + 1), (unsigned char *)(KmCw + 1), pEkhKm, pEkhKm, 16);
}

void RCP_HDCP2_GenKs(unsigned char *dKey2, unsigned char *EdKeyKs,
                     unsigned char *rRx, unsigned char * Ks)
{
    unsigned char tmp[16];

    memcpy(tmp, EdKeyKs, 16);
    xor_array(&tmp[8], rRx, &tmp[8], 8);
    memcpy(Ks, tmp, 16);
    xor_array(dKey2, Ks, Ks, 16);
}

void RCP_HDCP2_GenKsXorLc128(const uint8_t *s1, const uint8_t *s2, uint8_t *d, int len)
{
    int ii;

    if (!s1 || !s2 || !d) {
        return;
    }

    /** @todo: Optimize by using 32 bit-xors if needed. */
    for (ii = 0 ; ii < len ; ii++) {
        d[ii] = s1[ii] ^ s2[ii];
    }

    return;
}

