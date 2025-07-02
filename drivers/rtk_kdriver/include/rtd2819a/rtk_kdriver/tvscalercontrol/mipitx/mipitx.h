#ifndef _MIPITX_H_
#define _MIPITX_H_

#include <rtd_log/rtd_module_log.h>

#define MIPITX_BRINGUP_SCRIPT


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

#define MIPITX_ERR  -1
#define MIPITX_OK  0

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

#define MIPITX_PRINT_DEBUG(fmt, args...)    pr_debug(fmt, ##args)

typedef enum _MIPITX_PANEL_INDEX{
    MIPITX_HD_720_1440_PANEL = 0,
    MIPITX_TM697_DSC_PANEL = 1,
    MIPITX_PANEL_MAX,
}MIPITX_PANEL_INDEX;

typedef enum _MIPITX_LANE_CNT_CONFIG{
    SINGLE_PANEL_ONE_LANE = 1,
    SINGLE_PANEL_TWO_LANE =2,
    DUAL_PANEL_ONE_LANE = 2,
    SINGLE_PANEL_FOUR_LANE = 4,
    DUAL_PANEL_TWO_LANE = 4,
    SINGLE_PANEL_EIGHT_LANE = 8,
    DUAL_PANEL_FOUR_LANE = 8,
}MIPITX_LANE_CNT_CONFIG;

typedef enum _MIPITX_COLOR_DEPTH_CONFIG{
    RGB_6BIT_MODE = 0,
    RGB_8BIT_MODE,
    RGB_10BIT_MODE,
    DSC_MODE,
}MIPITX_COLOR_DEPTH_CONFIG;

typedef enum _MIPITX_COLOR_BITS_CONFIG{
    RGB_18BITS = 18,
    RGB_24BITS = 24,
    RGB_30BITS = 30,
}MIPITX_COLOR_BITS_CONFIG;

typedef struct _MIPITX_CONFIG_PARAMETER{
    UINT8 is_cmd_panel;
    MIPITX_LANE_CNT_CONFIG lane_cnt;
    MIPITX_COLOR_DEPTH_CONFIG color_depth;
    MIPITX_COLOR_BITS_CONFIG color_bits;
    UINT32 pixelclk;
    UINT16 hwidth;
    UINT16 vlength;
    UINT16 htotal;
    UINT16 vtotal;
    UINT8  hsyncwidth;
    UINT8  vsyncwidth;
    UINT16 hfrontporch;
    UINT16 hbackporch;
    UINT16 vfrontporch;
    UINT16 vbackporch;
    UINT8 hpolarity;
    UINT8 vpolarity;
    UINT8 dsc_en;
    UINT8 *dsc_pps;
}MIPITX_CONFIG_PARAMETER;

extern MIPITX_CONFIG_PARAMETER default_mipitx_parameter;

extern void mipitx_HD_720_1440_panel(void);
extern void mipitx_TM697_DSC_panel(void);
extern int mipitx_state_handler(void);
extern void vbe_disp_mipi_panel_suspend(void);
extern void vbe_disp_mipi_panel_resume(void);
#endif