#ifndef _DSCE2_H_
#define _DSCE2_H_

#ifndef UINT8
typedef unsigned char  __UINT8;
#define UINT8 __UINT8
#endif
#ifndef UINT16
typedef unsigned short __UINT16;
#define UINT16 __UINT16
#endif
#ifndef UINT32
typedef unsigned int   __UINT32;
#define UINT32 __UINT32
#endif

#ifndef TRUE
#define TRUE     1
#endif

#ifndef FALSE
#define FALSE    0
#endif

//------------------------------------------
// Definitions of Bits
//------------------------------------------
#define _ZERO                       0x00
#define _BIT0                       0x01
#define _BIT1                       0x02
#define _BIT2                       0x04
#define _BIT3                       0x08
#define _BIT4                       0x10
#define _BIT5                       0x20
#define _BIT6                       0x40
#define _BIT7                       0x80
#define _BIT8                       0x0100
#define _BIT9                       0x0200
#define _BIT10                      0x0400
#define _BIT11                      0x0800
#define _BIT12                      0x1000
#define _BIT13                      0x2000
#define _BIT14                      0x4000
#define _BIT15                      0x8000
#define _BIT16                      0x10000
#define _BIT17                      0x20000
#define _BIT18                      0x40000
#define _BIT19                      0x80000
#define _BIT20                      0x100000
#define _BIT21                      0x200000
#define _BIT22                      0x400000
#define _BIT23                      0x800000
#define _BIT24                      0x1000000
#define _BIT25                      0x2000000
#define _BIT26                      0x4000000
#define _BIT27                      0x8000000
#define _BIT28                      0x10000000
#define _BIT29                      0x20000000
#define _BIT30                      0x40000000
#define _BIT31                      0x80000000


#define _DSC2_DPF_SYNCWIDTH                                              3
#define _DSC2_DPF_HFRONT                                                 11
#define _DSC2_HBALNK_TOLERANCE                                           80

// DSC clock info
typedef struct {
    unsigned char dsce2_src_div; // 0: div1, 1: div2, 2: div4, 3: div8
    unsigned char dsce2_pixel_mode; // 0: 1 pixel mode, 1: 2 pixel mode, 2: 4 pixel mode, 3: 8 pixel mode
} dsce2_enc_clock_st;

typedef struct _DSCE2_CONFIG_PARAMETER{
    UINT16 dsce2_hactive;
    UINT16 dsce2_vactive;
    UINT16 dsce2_htotal;
    UINT16 dsce2_vtotal;
    UINT8  dsce2_hsync;
    UINT8  dsce2_vsync;
    UINT16 dsce2_hfront;
    UINT16 dsce2_hback;
    UINT16 dsce2_vfront;
    UINT16 dsce2_vback;
    UINT8 dsce2_en;
    UINT8 *dsce2_pps;
}DSCE2_CONFIG_PARAMETER;

extern void dsce2_driver_Config(DSCE2_CONFIG_PARAMETER *dsce2_config, dsce2_enc_clock_st *dsce2_enc_clock);

#define DSCE2_PRINT_DEBUG(fmt, args...)    pr_debug(fmt, ##args)
#endif