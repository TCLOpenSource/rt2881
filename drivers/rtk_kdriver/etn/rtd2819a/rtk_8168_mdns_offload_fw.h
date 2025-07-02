#ifndef __RTK_8168_MDNS_OFFLOAD_FW_H__
#define __RTK_8168_MDNS_OFFLOAD_FW_H__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include "rtk_8168_inc.h"

#define OFFLOAD_IPV4_ADDR_ADDR 0xCD08
#define OFFLOAD_IPV6_ADDR_ADDR 0xCD10
#define OFFLOAD_MAC_ADDR_ADDR 0xCD70
#define OFFLOAD_VLAN0_MAC_ADDR_ADDR 0xCC82
#define OFFLOAD_PASSTHROUGH_LIST_CONTENT_ADDR 0x6000
#define OFFLOAD_PASSTHROUGH_COUNT_ADDR 0x7bd0
#define OFFLOAD_PASSTHROUGH_LENGTH_TABLE_ADDR 0x7bd2
#define MAX_OFFLOAD_DATA_ADDR 0x7800
#define OFFLOAD_RAWPACKET_COUNT_ADDR 0x7800
#define OFFLOAD_RAWPACKET_START_ADDR 0x7802
#define OFFLOAD_RAWPACKET_RR_COUNT_ADDR 0x7804
#define OFFLOAD_RAWPACKET_RR_START_ADDR 0x7806
#define OFFLOAD_RAWPACKET_TABLE_ADDR 0x7808
#define OFFLOAD_WAKE_REASON_ADDR 0x7bc0
#define OFFLOAD_PASSBEHAVIOUR_ADDR 0x7bc2
#define OFFLOAD_RESPONSE_COUNTER_ADDR 0x7bc4
#define OFFLOAD_MISS_COUNTER_ADDR 0x7bc6
#define OFFLOAD_WAKE_UP_PACKET_LEN_ADDR 0x7bc8
#define OFFLOAD_WAKE_UP_PACKET_CONTENT_ADDR 0x7000


void rtl8168_mac_hw_mdns_offloading_setting(struct rtl8168_private *tp);
unsigned int rtl8168_get_hw_mdns_offload_passthrough_list_content_addr(struct rtl8168_private *tp);

#endif
