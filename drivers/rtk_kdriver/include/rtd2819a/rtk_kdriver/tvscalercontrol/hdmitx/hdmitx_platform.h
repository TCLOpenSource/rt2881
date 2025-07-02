#ifndef _HDMITX_PLATFORM_H_
#define _HDMITX_PLATFORM_H_

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <mach/rtk_log.h>
#include <mach/rtk_timer.h>
#include <rtd_types.h>

/**********************************************************************************************
*
*   Marco or Definitions
*
**********************************************************************************************/
#if 0
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
#ifndef INT8
typedef char  __INT8;
#define INT8 __INT8
#endif
#ifndef INT16
typedef short __INT16;
#define INT16 __INT16
#endif
#ifndef INT32
typedef int   __INT32;
#define INT32 __INT32
#endif
#endif
#ifndef BOOLEAN
#ifndef _EMUL_WIN
typedef	unsigned int    __BOOLEAN;
#define BOOLEAN __BOOLEAN
#else
typedef	unsigned char   __BOOLEAN;
#define BOOLEAN __BOOLEAN
#endif
#endif

#define _OFF 0
#define _ON 1
#define _FALSE 0
#define _TRUE 1
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef HIGH
#define HIGH 1
#endif
#define _LOW 0
#define _HIGH 1
#define _DISABLE 0
#define _ENABLE 1
#define _UNSTABLE 0
#define _STABLE 1
#define _FAIL 0
#define _SUCCESS 1
#define _FUNCTION_OFF 0
#define _FUNCTION_ON 1

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

//------------------------------------------
// Definitions of compiler option
//------------------------------------------
#define HDMITX_LINUX_PLATFORM _ON
#define _HDMI_TX_SUPPORT _ON
#define _HDMI_TX0_EXIST _ON
#define _HDMI_PURE_SUPPORT _ON

#define _HDMI_PHY_TX0_SUPPORT _ON
#define _HDMI_PHY_TX1_SUPPORT _OFF
#define _HDMI_PHY_TX2_SUPPORT _OFF

#define _HDMI_MAC_TX0_PURE _ON
#define _HDMI_MAC_6G_TX0_SUPPORT _ON
#define _HDMI_MAC_TX0_SUPPORT _ON
#define _HDMI_MAC_TX1_SUPPORT _OFF
#define _HDMI_MAC_TX2_SUPPORT _OFF

#define _HDMI20_MAC_TX0_SUPPORT _ON
#define _P1_HDMI_SUPPORT _ON
#define _P2_HDMI_SUPPORT _OFF
#define _P3_HDMI_SUPPORT _OFF
#define _P4_HDMI_SUPPORT _OFF

#ifndef CONFIG_OPTEE_HDCP2
#ifndef CONFIG_HDCPTX
  #define _HDCP2_SUPPORT _OFF
#else
  #define _HDCP2_SUPPORT _ON
#endif
  #define _HDCP2_TX_SUPPORT _OFF

  #define _HDMI_HDCP2_TX_SUPPORT _OFF
  #define _HDMI_HDCP2_TX0_SUPPORT _OFF
  #define _HDMI_HDCP14_TX_SUPPORT _OFF
  #define _HDMI_HDCP14_TX0_SUPPORT _OFF
#else
#ifndef CONFIG_HDCPTX
  #define _HDCP2_SUPPORT _OFF
#else
  #define _HDCP2_SUPPORT _ON
#endif
  #define _HDCP2_TX_SUPPORT _OFF

  #define _HDMI_HDCP2_TX_SUPPORT _OFF
  #define _HDMI_HDCP2_TX0_SUPPORT _OFF
  #define _HDMI_HDCP14_TX_SUPPORT _OFF
  #define _HDMI_HDCP14_TX0_SUPPORT _OFF
#endif

#define _HDMI_HDCP14_TX1_SUPPORT _OFF
#define _HDMI_HDCP14_TX2_SUPPORT _OFF
#define _HDMI_HDCP2_TX1_SUPPORT _OFF
#define _HDMI_HDCP2_TX2_SUPPORT _OFF

#define _HDMI21_TX_SUPPORT _ON
#define _HDMI21_MAC_TX0_SUPPORT _ON
#define _HDMI_MAC_TX0_SUPPORT_HDMI21 _ON
#define _HDMI21_HDCP2_TX_SUPPORT _OFF
#define _HDMI21_MAC_TX1_SUPPORT _OFF
#define _HDMI21_MAC_TX2_SUPPORT _OFF
#define _P0_HDMI21_SUPPORT _ON
#define _P1_HDMI21_SUPPORT _OFF
#define _P2_HDMI21_SUPPORT _OFF
#define _P3_HDMI21_SUPPORT _OFF

#define _DSC_SUPPORT _ON
#define _DSC_ENCODER_SUPPORT _ON
#define _DSC_DECODER_SUPPORT _OFF
#define _HDMI_TX_DSC_ENCODER_SUPPORT _ON//_ON

#define _HDMI_DP_PLUS_SUPPORT _OFF
#define _HDMI_MAC_TX0_DP_PLUS _OFF
#define _HDMI_DPPP_PLUG_EVENT_CHECK_EDID _ON

#define _DP_MST_SUPPORT _OFF
#define _DP_MST_MULTI_FUNCTION_SUPPORT _OFF
#define _HW_DP_STREAM_DSC_BYPASS_SUPPORT _OFF
#define _DP_DUAL_MODE_TX_SUPPORT _OFF

#define _AUDIO_SUPPORT _ON //
#define _HDMI_3D_AUDIO_TX0_SUPPORT _OFF
#define _HDMI21_3D_AUDIO_TX0_SUPPORT _OFF
#define _HDMI_AUDIO_FW_TRACKING_SUPPORT _OFF
#define _HDMI21_HBR_AUDIO_TX0_SUPPORT _ON
#define _HDMI_HBR_AUDIO_TX0_SUPPORT _ON

#define _4K2K_EDID_MODIFY_SUPPORT _OFF//_ON
#define _HDMI_HDR10_TX0_SUPPORT _OFF//_ON
#define _ST4_EXIST _OFF
#define _DP_TX_SUPPORT _OFF
#define _HDMI_FREESYNC_TX0_SUPPORT _OFF
#define _VGA_TX_SUPPORT _OFF

#define HDMI_PHY_TX_ENABLE_3RD_TRACKING _OFF

#define _HW_HDMI_SW_PKT_GEN_1 0
#define _HW_HDMI_SW_PKT_GEN_2 1
#define _HW_HDMI_SW_PKT_TYPE _HW_HDMI_SW_PKT_GEN_2

#define _HDMI_AUDIO_GEN_1 0
#define _HDMI_AUDIO_GEN_2 1
#define _HW_HDMI_AUDIO_STRUCT _HDMI_AUDIO_GEN_2

#define _HDCP14_KEY_280 0 // original key size
#define _HDCP14_KEY_320 1 // key size after encryption
#define _HW_HDMI_HDCP14_KEY_TYPE _HDCP14_KEY_320

#define _HW_PHY_TX_GEN_1 0
#define _HW_PHY_TX_GEN_2 1
#define _HW_PHY_TX_TYPE _HW_PHY_TX_GEN_2

#define CONFIG_HDMITX_REPEATER_SUPPORT 1

//#define  CONFIG_FORCE_AUDIO_PACKET_BYPASS _ON

//#define HDMI_INPUT_SOURCE_CONFIG
//#define HDMITX_SW_CONFIG
#endif
