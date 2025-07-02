#ifndef _HDCPTX_COMMON_H
#define _HDCPTX_COMMON_H
#include <rtd_log/rtd_module_log.h>
#include <hdcp/hdcp2_2/hdcp2_hal.h>
#include <hdcp/hdcp2_2/crypto.h>
#include <hdcp/hdcp2_2/bigdigits.h>
#include <hdcp/hdcp2_2/hmac.h>
#include <hdcp/hdcp2_2/hdcp2_rcp_api.h>
#include <hdcp/hdcp2_2/HDCP_KEY_SIZE.h>
#include <mach/rtk_timer.h>
#include <hdcp/hdcp_common.h>

#define HDCPTX_ABS(x, y)             ((x > y) ? (x-y) : (y-x))
#define hdcptx_timer_diff_90k_ms_counter(timer)   HDCPTX_ABS(timer, rtk_timer_misc_90k_ms())


//------------------------------------------
// Definitions of Bits
//------------------------------------------
#define _ZERO   						0x00
#define _BIT0   						0x01UL
#define _BIT1   						0x02UL
#define _BIT2   						0x04UL
#define _BIT3   						0x08UL
#define _BIT4   						0x10UL
#define _BIT5   						0x20UL
#define _BIT6   						0x40UL
#define _BIT7   						0x80UL
#define _BIT8   						0x0100UL
#define _BIT9   						0x0200UL
#define _BIT10  						0x0400UL
#define _BIT11  						0x0800UL
#define _BIT12  						0x1000UL
#define _BIT13  						0x2000UL
#define _BIT14  						0x4000UL
#define _BIT15  						0x8000UL
#define _BIT16   						0x00010000UL
#define _BIT17  						0x00020000UL
#define _BIT18   						0x00040000UL
#define _BIT19   						0x00080000UL
#define _BIT20   						0x00100000UL
#define _BIT21   						0x00200000UL
#define _BIT22   						0x00400000UL
#define _BIT23   						0x00800000UL
#define _BIT24   						0x01000000UL
#define _BIT25   						0x02000000UL
#define _BIT26  						0x04000000UL
#define _BIT27  						0x08000000UL
#define _BIT28  						0x10000000UL
#define _BIT29  						0x20000000UL
#define _BIT30  						0x40000000UL
#define _BIT31  						0x80000000UL

#define _TRUE			1
#define _FALSE			0

#define HDCPTX_ERR(fmt, args...)         rtd_pr_hdcp_emerg("[TX] " fmt, ## args)
#define HDCPTX_WARIN(fmt, args...)       rtd_pr_hdcp_warn("[TX] " fmt, ## args)
#define HDCPTX_INFO(fmt, args...)        rtd_pr_hdcp_info("[TX] " fmt, ## args)
#define HDCPTX_DBG(fmt, args...)         rtd_pr_hdcp_debug("[TX] " fmt, ## args)

#ifdef CONFIG_ARCH_RTK2819A
#define HDCP_TX_I2C_BUSID 3
#else
#define HDCP_TX_I2C_BUSID 5
#endif
//-----------------------------------------------------------------
// HDCP1 B_STATUS
//-----------------------------------------------------------------
#define BSTATUS_HDMI_MODE(x)                ((x & 0x1)<<12)
#define BSTATUS_MAX_CASCADE_EXCEEDED(x)     ((x & 0x1)<<11)
#define BSTATUS_DEPTH(x)                    ((x & 0x7)<<8)
#define BSTATUS_MAX_DEVS_EXCEEDED(x)        ((x & 0x1)<<7)
#define BSTATUS_DEVICE_COUNT(x)             ((x & 0x7F))

#define BSTATUS_GET_MAX_CASCADE_EXCEEDED(x) ((x >>11) & 0x1)
#define BSTATUS_GET_DEPTH(x)                ((x >>8) & 0x7)
#define BSTATUS_GET_MAX_DEVS_EXCEEDED(x)    ((x >>7) & 0x1)
#define BSTATUS_GET_DEVICE_COUNT(x)         ((x & 0x7F))

//-----------------------------------------------------------------
// HDCP2 RX INFO
//-----------------------------------------------------------------
#define RX_INFO_DEPTH(x)                    ((x & 0x7)<<9)
#define RX_INFO_DEVICE_COUNT(x)             ((x & 0x1F)<<4)
#define RX_INFO_MAX_DEVS_EXCEEDED           (1<<3)
#define RX_INFO_MAX_CASCADE_EXCEDED         (1<<2)
#define RX_INFO_HDCP2_0_REPEATER_DOWNSTREAM (1<<1)
#define RX_INFO_HDCP1_DEVICE_DOWNSTREAM     (1)

#define RX_INFO_GET_DEPTH(x)                       ((x >>9 )& 0x7)
#define RX_INFO_GET_DEVICE_COUNT(x)                ((x >>4) & 0x1F)
#define RX_INFO_GET_MAX_DEVS_EXCEEDED(x)           ((x>>3) & 0x1)
#define RX_INFO_GET_MAX_CASCADE_EXCEDED(x)         ((x>>2) & 0x1)
#define RX_INFO_GET_HDCP2_0_REPEATER_DOWNSTREAM(x) ((x>>1) & 0x1)
#define RX_INFO_GET_HDCP1_DEVICE_DOWNSTREAM(x)     (x & 0x1)


typedef enum _HDMI_HDCPTX_{
    NO_HDCP = 0,
    HDCP14,
    HDCP22,
    HDCP_OFF
}HDMI_HDCPTX_E;

extern void HdcpTxStoredReceiverId(unsigned char *pucInputArray, unsigned char ucDeviceCount);
#endif
