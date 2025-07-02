#ifndef __MJPEG_ENC_DEC_H_
#define __MJPEG_ENC_DEC_H_


#define _SOI_SOF_ 0xFFD8FFC0
#define SOF_Lf 17 //frame header length
#define SOF_P 8 //pixel precision 8
#define SOF_Nf 3
#define SOF_C1 1
#define SOF_C2 2
#define SOF_C3 3
#define SOF_H1 2
#define SOF_H2 1
#define SOF_H3 1
#define SOF_V1 2
#define SOF_V2 1
#define SOF_V3 1
#define SOF_Tq1 0
#define SOF_Tq2 1
#define SOF_Tq3 1

#define SOS_Ls 12
#define SOS_Ns 3
#define SOS_Cs1 1
#define SOS_Cs2 2
#define SOS_Cs3 3
#define SOS_Td1 0
#define SOS_Td2 1
#define SOS_Td3 1

#define SOS_Ta1 0
#define SOS_Ta2 1
#define SOS_Ta3 1

#define SOS_Ss 0
#define SOS_Se 63
#define SOS_Ah 0
#define SOS_Al 0
#define _SOS_ 0xFFDA
#define APP0_Length 0x7 //insert ?’h10??14 zeros bytes


typedef enum   MJPEG_RTN
{
  MJPEG_RTN_FALSE            = 0,
  MJPEG_RTN_OK               = 1,
  MJPEG_RTN_TRUE             = 1,
} MJPEG_RTN_Err;

typedef enum MSG_MJPEG_EVENT_TYPE
{
    MSG_MJPEG_ENC_DONE_EVENT = 0,
} MSG_MJPEG_EVENT_TYPE_T;


void Set_Val_demo_mode(unsigned char value);
void Set_Val_yc_spearate(unsigned char value);
void Set_Val_nosignal_mode(unsigned char value);
void Set_Val_nosupport_mode(unsigned char value);
void Set_Val_output_fmt(unsigned char value);
unsigned int Get_Val_bitcnt(void);
unsigned int Get_Val_enc_crc(void);
void Set_Val_demo_mode_jpeg_addr(unsigned int addr);

int	mjpeg_enc_done_isr(void);
unsigned char Get_Val_dqt_id(unsigned char dqt_value );

void set_MJPEG_ENC(void);

void IMD_bypass_setting(void);
void frc_2kto1k_uzd_enc_dec_uzu(void);
unsigned int mjpeg_get_YBufSize(void);
unsigned int mjpeg_get_CBufSize(void);


#endif