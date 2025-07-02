#ifndef __HDCP_CAL
#define __HDCP_CAL


extern void Hdcp2AesCalculate(unsigned char port, unsigned char* pucAesDataIn, unsigned char* pucAesKeyIn);
extern void Hdcp2AesCalculate_out(unsigned char port, unsigned char* pucAesDataIn, unsigned char* pucAesKeyIn, unsigned char* pucAesDataOut);
extern unsigned char Hdcp2Sha256Calculate(unsigned char port, unsigned char *pucInputArray, unsigned char ucCalCount);
extern unsigned char Hdcp2AesRandomGen(unsigned char port, unsigned char *AesData, unsigned char len);
extern unsigned char Hdcp2HmacSha256Calculate(unsigned char port, unsigned char *pucHMACSHA256Input, unsigned char *pucHMACSHA256CipherInput, unsigned char ucInputArray1BlockNumber);
#endif
