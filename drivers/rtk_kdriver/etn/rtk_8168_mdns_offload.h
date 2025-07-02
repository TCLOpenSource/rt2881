#ifndef __RTK_8168_MDNS_OFFLOAD_H__
#define __RTK_8168_MDNS_OFFLOAD_H__
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
#include "rtk_8168_def.h"


#define MDNS_MAX_PACKET_SIZE 512
#define MDNS_PACKET_MAX 16
#define MDNS_TYPE_MAX 8
#define MDNS_PASSTHROUGH_MAX 32
#define MAX_PORT_WAKE_SOURCE_CNT	8
typedef enum
{
		// All the queries are forwarded to the system without anymodification
		FORWARD_ALL,
		// All the queries are dropped.
		DROP_ALL,
		// Only the queries present in the passthrough list are forwarded
		// to the system without any modification.
		PASSTHROUGH_LIST
}PASSTHROUGH_BEHAVIOUR_TYPE;

typedef enum
{
		MdnsOffloadState_DISSABLE = 0,
		MdnsOffloadState_ENABLE
}MdnsOffloadState;

/*struct*/
struct MatchCriteria{
		unsigned short qtype;
		unsigned short nameoffset;
};

struct MdnsProtocolData{
	unsigned char packet[MDNS_MAX_PACKET_SIZE];
	unsigned int packet_size;
	unsigned int type_size;
	struct MatchCriteria type[MDNS_TYPE_MAX];
};

struct MdnsProtocolData_list{
	unsigned int list_size;
	struct MdnsProtocolData list[MDNS_PACKET_MAX];
};

struct MdnsOffload_passthrough{
		unsigned int qname_len;
		unsigned char qname[256];
};

struct MdnsOffload_passthrough_list{
		unsigned int list_size;
		struct MdnsOffload_passthrough list[MDNS_PASSTHROUGH_MAX];
};

struct PORT_WAKE_SOURCE
{
	unsigned short flags;
	/* BIT0: 0:wake up by tcp, 1: wake up by udp
	*	BIT1: 0:wake up by src port, 1:wake up by dst port
	*/
	unsigned short port;
#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
	unsigned short packet_start_code;
	unsigned short crc_value;
	unsigned char wol_mask[32];
		unsigned char mask_selection_code;
#else
	unsigned char wol_mask[16];
	unsigned short crc_value;
#endif
};

struct PORT_WAKE_SOURCE_list{
		unsigned int list_size;
		struct PORT_WAKE_SOURCE list[MAX_PORT_WAKE_SOURCE_CNT];
};

struct MDNS_PACKET_UPLOAD_WORKER
{
	struct net_device *dev;
	struct work_struct work;
};

extern ssize_t rtl8168_wol_mdns_offload_passthrough_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf);
extern ssize_t rtl8168_wol_mdns_offload_passthrough_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count);
extern ssize_t rtl8168_wol_mdns_offload_protocol_data_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf);
extern ssize_t rtl8168_wol_mdns_offload_protocol_data_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count);
extern ssize_t rtl8168_wol_mdns_offload_passthrough_behaviour_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf);
extern ssize_t rtl8168_wol_mdns_offload_passthrough_behaviour_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count);
extern ssize_t rtl8168_wol_mdns_offload_wake_port_show(
			struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t rtl8168_wol_mdns_offload_wake_port_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count);
extern ssize_t rtl8168_etn_mdns_offfload_state_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf);
extern ssize_t rtl8168_etn_mdns_offfload_state_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count);
extern ssize_t rtl8168_wol_ipv4_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf);
extern ssize_t rtl8168_wol_ipv4_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count);
extern ssize_t rtl8168_wol_ipv6_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t rtl8168_wol_ipv6_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);
extern void rtl8168_init_mdns_packet_upload_work(struct net_device *dev);
extern void rtl8168_set_vcpu_mdns_offload_params(struct net_device *ndev);
extern void rtl8168_check_mdns_packet_upload_flag(struct net_device *dev);
extern void rtl8168_schedule_mdns_packet_upload_work(struct net_device *dev);
extern void rtl8168_cancel_mdns_packet_upload_work(struct net_device *dev);
extern void rtl8168_set_hw_mdns_offload_params(struct rtl8168_private *tp);
extern void rtk8168_hw_mdns_standby_settings(struct rtl8168_private *tp);
extern void rtl8168_get_hw_mdns_wakeup_information(struct rtl8168_private *tp, unsigned char *buf, unsigned int buf_len);
extern void rtk8168_get_pkt_crc16(unsigned char *pkt, unsigned int pkt_len,
			unsigned char *mask_array,	unsigned short *crc_value);
extern void rtk8168_set_wake_port_pattern(struct rtl8168_private *tp);
extern void rtk8168_hw_mdns_standby_pre_oob_setting(struct rtl8168_private *tp);
extern void rtk8168_hw_mdns_standby_post_oob_setting(struct rtl8168_private *tp);
extern void rtl8168_airplay_to_crc_pattern(struct rtl8168_private *tp);
extern void printf_domain_debug_log(void);
#endif
