#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <rtd_log/rtd_module_log.h>
#include <rtk_kdriver/io.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include "rtk_8168_def.h"
#include "rtk_8168_mdns_offload.h"
#include "rtk_8168_inc.h"
#include "rtk_8168_mdns_offload_fw.h"

#define ETN_ROUND_UP(n, d)	(DIV_ROUND_UP(n, d) * (d))
void packet_data_log(unsigned char *packet, unsigned int size);
static struct MdnsProtocolData_list mdns_data_list = {0};
static struct MdnsOffload_passthrough_list passthrough_list = {0};
static MdnsOffloadState mdns_offfload_state = MdnsOffloadState_DISSABLE;
static PASSTHROUGH_BEHAVIOUR_TYPE passthrough_behaviour = FORWARD_ALL;
static struct PORT_WAKE_SOURCE_list wakeup_source_list = { 
	.list_size = 1,
	.list = {
		{
			.flags = 0x4,
			.port = 65535,
		},
	},	
};
static struct MDNS_PACKET_UPLOAD_WORKER g_upload_worker;
static volatile bool g_need_upload_mdns_packet = false;
static unsigned char g_wol_ipv4_addr[4] = {0};
static unsigned short g_wol_ipv6_addr[8] = {0};


static int mdns_parse_field(const unsigned char *rawOffloadPacket, 
							const unsigned int rawOffloadPacket_len, 
								unsigned int cur_ofst, unsigned char *output, int *is_end_normal)
{
	int output_len = 0;
	
	while (1) {
		if (cur_ofst >= rawOffloadPacket_len)
			return -1;
		else if (rawOffloadPacket[cur_ofst] == 0) {
			*is_end_normal = 1;
			break;
		}
		else if((cur_ofst + 2) > rawOffloadPacket_len)
			return -1;
		else if (rawOffloadPacket[cur_ofst] == 0xc0) {
			int ret = 0;
			if (rawOffloadPacket[cur_ofst + 1] >= rawOffloadPacket_len)
				return -1;
			ret = mdns_parse_field(rawOffloadPacket, rawOffloadPacket_len, 
					rawOffloadPacket[cur_ofst + 1], output + output_len, is_end_normal);
			if (ret == -1)
				return -1;
			output_len += ret;
			cur_ofst += 2;
			if (*is_end_normal)
				break;
		}
		else {
			if ((cur_ofst + rawOffloadPacket[cur_ofst] + 1) > rawOffloadPacket_len)
				return -1;
			if ((output_len + (rawOffloadPacket[cur_ofst] + 1)) > 255)
				return -1;
			memcpy(output + output_len, rawOffloadPacket + cur_ofst, rawOffloadPacket[cur_ofst] + 1);
			output_len += (rawOffloadPacket[cur_ofst] + 1);
			cur_ofst += (rawOffloadPacket[cur_ofst] + 1);
		}
	}
	return output_len;
}

int rtk_8168_mdns_parse_field(const unsigned char *rawOffloadPacket, 
								const unsigned int rawOffloadPacket_len, 
								unsigned int cur_ofst, unsigned char *output)
{
	int is_end_normal = 0;
	int len = 0;
	if(!rawOffloadPacket || rawOffloadPacket_len <= 12 || !output)
		return -1;	
	len = mdns_parse_field(rawOffloadPacket, rawOffloadPacket_len, cur_ofst, output, &is_end_normal);
	if(!is_end_normal)
		return -1;
	if(len <= 6)
		return -1;
	if(output[len - 6] != 5 || memcmp(output + len - 5, "local", 5) != 0)
		return -1;	
	return len;
}


void print_passthrough_name(unsigned char *packet, unsigned int size)
{
	int i = 0;
	unsigned char tmp[8] = {0}, buf[1024] = {0};

	memset(buf,0,sizeof(buf));

	while( i < size)
	{

		if(packet[i] > 32 && packet[i]<127)
		{
			snprintf(tmp, 2,"%c", packet[i]);
		}
		else
		{
			snprintf(tmp, 2," ");
		}
		strcat(buf,tmp);
		memset(tmp,0,sizeof(tmp));
		i++;
	}

	eth_err("passthrough name string: %s \n",buf);
}


static void eth_parse_mdns_domain_name(u8 *buf, u16 buf_len, u16 cur_offset)
{
	u8 name[255] = {0};
	u8 lable_remain_len = 0;
	u16 name_ptr = 0;
	u16 len = 0;

	while (1) {
		if (cur_offset >= buf_len || len >= 255) {
			len = 0;
			goto exit;
		}

		if (!lable_remain_len) {
			/* compression label */
			if ((buf[cur_offset] & 0xc0) == 0xc0) {
				if (cur_offset + 1 >= buf_len) {
					len = 0;
					goto exit;
				}

				name_ptr = ((buf[cur_offset] & 0x3f) << 8) +
					   buf[cur_offset + 1];
				if (name_ptr >= cur_offset) {
					len = 0;
					goto exit;
				}
				cur_offset = name_ptr;
			}

			/* label length */
			if (buf[cur_offset] > 63) {
				len = 0;
				goto exit;
			}

			if (buf[cur_offset] == 0) {
				name[len] = '\0';
				len += 1;
				goto exit;
			} else {
				name[len] = '.';
				len += 1;
				lable_remain_len = buf[cur_offset];
				cur_offset++;
			}
		} else {
			name[len] = buf[cur_offset];
			len += 1;
			lable_remain_len = lable_remain_len - 1;
			cur_offset++;
		}
	}

exit:


	if (len)
		eth_err("domain name=%s\n", name + 1);
	else
		eth_err("domain name parsing failed\n");

}

// void print_mdns_offload_protocol_name(unsigned char *packet, unsigned int nameoffset, unsigned int size)
// {
//     int i = nameoffset;
//     unsigned char tmp[8] = {0}, buf[1024] = {0};

//     memset(buf,0,sizeof(buf));

//     while( i < size)
//     {

//         if(packet[i] > 32 && packet[i]<127)
//         {
//             snprintf(tmp, 2,"%c", packet[i]);
//         }
//         else if(packet[i] == 0 )
//         {
//             break;
//         }
//         else
//         {
//             snprintf(tmp, 2," ");
//         }

//         strcat(buf,tmp);
//         memset(tmp,0,sizeof(tmp));
//         i++;
//     }
//     eth_err("mdns offload protocol data: %s",buf);
// 	eth_parse_mdns_domain_name(packet,size,nameoffset);
// }

ssize_t rtl8168_wol_mdns_offload_passthrough_show(
			struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int i=0;
	for(i=0; i < passthrough_list.list_size;i++)
	{
		eth_err("passthrough_list: %d \n",i);
		packet_data_log(passthrough_list.list[i].qname,passthrough_list.list[i].qname_len);
		print_passthrough_name(passthrough_list.list[i].qname,passthrough_list.list[i].qname_len);
	}

	return 0;
}


ssize_t rtl8168_wol_mdns_offload_wake_port_show(
			struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int i = 0;
	buf[0] = 0;
	for(i = 1; i < wakeup_source_list.list_size; i++) {
		snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "%d:%u-%u\n", 
			(i + 7), wakeup_source_list.list[i].flags, wakeup_source_list.list[i].port);
	}
	return strlen(buf);
}

#define TCP_SOURCE_PORT_PACKET_PARTTEN { \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
}

#define TCP_SOURCE_PORT_MASK_PARTTEN { \
	0x00,0x70,0x80,0x00,	\
	0x0c,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
}

#define TCP_DEST_PORT_PACKET_PARTTEN { \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
}

#define TCP_DEST_PORT_MASK_PARTTEN { \
	0x00,0x70,0x80,0x00,	\
	0x30,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
}

#define UCP_SOURCE_PORT_PACKET_PARTTEN { \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
}

#define UDP_SOURCE_PORT_MASK_PARTTEN { \
	0x00,0x70,0x80,0x00,	\
	0x0c,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
}

#define UCP_DEST_PORT_PACKET_PARTTEN { \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
}

#define UDP_DEST_PORT_MASK_PARTTEN { \
	0x00,0x70,0x80,0x00,	\
	0x30,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
	0x00,0x00,0x00,0x00,	\
}
#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
static void rtl8168_wake_port_to_crc_pattern(unsigned int index, unsigned short flags, unsigned short port)
{
	wakeup_source_list.list[index].mask_selection_code = 0;
	wakeup_source_list.list[index].packet_start_code = 0;
	if(flags & 0x1) { //udp wake
		if(flags & 0x2) { //dst port
			unsigned char packet_content[128] = UCP_DEST_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = UDP_DEST_PORT_MASK_PARTTEN;
			memset(wakeup_source_list.list[index].wol_mask, 0,	32);
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[36] = ((port >> 8) & 0xFF);
			packet_content[37] = ((port >> 0) & 0xFF);
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		} else {
			unsigned char packet_content[128] = UCP_SOURCE_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = UDP_SOURCE_PORT_MASK_PARTTEN;
			memset(wakeup_source_list.list[index].wol_mask, 0,	32);
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[34] = ((port >> 8) & 0xFF);
			packet_content[35] = ((port >> 0) & 0xFF);	
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		}
	} else {
		if(flags & 0x2) { //dst port
			unsigned char packet_content[128] = TCP_DEST_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = TCP_DEST_PORT_MASK_PARTTEN;
			memset(wakeup_source_list.list[index].wol_mask, 0,	32);
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[36] = ((port >> 8) & 0xFF);
			packet_content[37] = ((port >> 0) & 0xFF);	
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		} else {
			unsigned char packet_content[128] = TCP_SOURCE_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = TCP_SOURCE_PORT_MASK_PARTTEN;
			memset(wakeup_source_list.list[index].wol_mask, 0,	32);
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[34] = ((port >> 8) & 0xFF);
			packet_content[35] = ((port >> 0) & 0xFF);
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		}
	}
}
#else
static void rtl8168_wake_port_to_crc_pattern(unsigned int index, unsigned short flags, unsigned short port)
{
	if(flags & 0x1) { //udp wake
		if(flags & 0x2) { //dst port
			unsigned char packet_content[128] = UCP_DEST_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = UDP_DEST_PORT_MASK_PARTTEN;
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[36] = ((port >> 8) & 0xFF);
			packet_content[37] = ((port >> 0) & 0xFF);
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		} else {
			unsigned char packet_content[128] = UCP_SOURCE_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = UDP_SOURCE_PORT_MASK_PARTTEN;
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[34] = ((port >> 8) & 0xFF);
			packet_content[35] = ((port >> 0) & 0xFF);	
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		}
	} else {
		if(flags & 0x2) { //dst port
			unsigned char packet_content[128] = TCP_DEST_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = TCP_DEST_PORT_MASK_PARTTEN;
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[36] = ((port >> 8) & 0xFF);
			packet_content[37] = ((port >> 0) & 0xFF);	
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		} else {
			unsigned char packet_content[128] = TCP_SOURCE_PORT_PACKET_PARTTEN;
			unsigned char tmp_mask[16] = TCP_SOURCE_PORT_MASK_PARTTEN;
			memcpy(wakeup_source_list.list[index].wol_mask, tmp_mask, 16);
			packet_content[34] = ((port >> 8) & 0xFF);
			packet_content[35] = ((port >> 0) & 0xFF);
			rtk8168_get_pkt_crc16(packet_content, sizeof(packet_content), 
					wakeup_source_list.list[index].wol_mask, &wakeup_source_list.list[index].crc_value);
		}
	}
}
#endif

#define AIRPLAY_MAIGIC_PACKET { \
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x01, 0x02, 0x03, 0x04, \
	0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, \
	0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, \
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, \
	0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, \
}
#define AIRPLAY_MAIGIC_MASK_PARTTEN { \
	0x3f,0x70,0x80,0x00,	\
	0x00,0xfc,0xff,0xff,	\
	0xff,0xff,0xff,0xff,	\
	0xff,0xff,0xff,0x3f,	\
}

void rtl8168_airplay_to_crc_pattern(struct rtl8168_private *tp)
{
	struct net_device *ndev = tp->dev;
	unsigned char packet[128] = AIRPLAY_MAIGIC_PACKET;
	unsigned char tmp_mask[16] = AIRPLAY_MAIGIC_MASK_PARTTEN;
	int i	= 0;
#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
	wakeup_source_list.list[0].mask_selection_code = 0;
	wakeup_source_list.list[0].packet_start_code = 0;
	memset(wakeup_source_list.list[0].wol_mask, 0,	32);
#else
	memset(wakeup_source_list.list[0].wol_mask, 0,	16);
#endif
	memcpy(wakeup_source_list.list[0].wol_mask, tmp_mask, 16);

	for(i = 0; i < 11; i++) {
		u8	*buf = packet + 60 + i * 6;
		memcpy(buf, ndev->dev_addr, 6);
	}
	
	rtk8168_get_pkt_crc16(packet, sizeof(packet), 
					wakeup_source_list.list[0].wol_mask, &wakeup_source_list.list[0].crc_value);

}

ssize_t rtl8168_wol_mdns_offload_wake_port_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	ssize_t ret = -1;
	
	if((count >= 5) && (memcmp(buf, "clean", 5) == 0)) {
		wakeup_source_list.list_size = 1;
		ret = count;
	} else {
		unsigned int flags = 0;
		unsigned int port = 0;
		unsigned int i = 0;
		if (sscanf(buf, "add,%u,%u", &flags, &port) != 2)
			goto EXIT;
		for(i = 0; i < wakeup_source_list.list_size; i++) {
			if(wakeup_source_list.list[i].flags == flags
				&& wakeup_source_list.list[i].port == port) {
				ret = count;
				goto EXIT;
			}
		}

		if(wakeup_source_list.list_size >= MAX_PORT_WAKE_SOURCE_CNT)
			goto EXIT;
		if (sscanf(buf, "add,%u,%u", &flags, &port) == 2) {
			i = wakeup_source_list.list_size;
			wakeup_source_list.list[i].flags = flags;
			wakeup_source_list.list[i].port = port;
			rtl8168_wake_port_to_crc_pattern(i, flags, port);
			wakeup_source_list.list_size++;		
			ret = count;
		}
	}

EXIT:
	return ret;
}


static void rtl8168_free_split_string(unsigned char **tokens,int maxTokens) {

	int i = 0;

	for (i = 0; i < maxTokens; i++)
	{
		if(tokens[i] != NULL)
		{
			kfree(tokens[i]);
		}
	}
}

// Function to split a string
static int rtl8168_split_string(char *str, unsigned char *delimiter, unsigned char **tokens, int maxTokens, unsigned char **str_point) {
		unsigned char *token;
		int tokenCount = 0;
		// The first call to strsep requires the string pointer
		// subsequent calls should use NULL
		token = strsep(&str, delimiter);
		while (token != NULL && tokenCount < maxTokens) {
				tokens[tokenCount] = kstrdup(token,GFP_KERNEL);	// Dynamically allocate memory
				tokenCount++;
				if(tokenCount >= maxTokens)
						break;
				// Subsequent calls to strsep with NULL
				token = strsep(&str, delimiter);
		}

		*str_point = str;
		return tokenCount;	// Return the number of tokens stored
}

static int rtl8168_verify_passthrough(unsigned char* qname, int qname_size)
{
		int i = 0;
		if ( passthrough_list.list_size >= MDNS_PASSTHROUGH_MAX)
		{
				eth_err(" passthrough_list.list_size >= MDNS_PASSTHROUGH_MAX \n");
				return -1;
		}

		for (i = 0; i < passthrough_list.list_size; i++) {
				if((passthrough_list.list[i].qname_len == qname_size) && (0 == memcmp(passthrough_list.list[i].qname, qname, qname_size))) {
						eth_err(" Already set %s \n", passthrough_list.list[i].qname);
						return -1;
				}
		}

		return 0;

}

static void rtl8168_set_passthrough(unsigned char* qname, int qname_size)
{
	packet_data_log(qname,qname_size);
	if(0 == rtl8168_verify_passthrough(qname,qname_size))
	{
		passthrough_list.list[passthrough_list.list_size].qname_len = qname_size;
		memcpy(passthrough_list.list[passthrough_list.list_size].qname, qname, qname_size);
		passthrough_list.list_size++;
	}
	else
	{
		eth_err(" rtl8168_verify_passthrough fail\n");
	}
}

static int rtl8168_set_passthrough_s(unsigned char* qname, int qname_size)
{
	/**/
	unsigned char tmp_qname[257] = {0};
	int i = 0;
	unsigned char tmp_qname_size = 0;
	int size_p = 0;


	if(qname_size > 256 || qname_size < 1)
	{
		eth_err(" rtl8168_set_passthrough_s qname_size = %d fail\n",qname_size);
		return -1;
	}
	


	for(i = 0; i < qname_size; i++)
	{

		if(qname[i] != '.')
		{
			tmp_qname[i+1] = qname[i];
			tmp_qname_size++;
		}
		else
		{
			tmp_qname[size_p] = tmp_qname_size;
			size_p = i+1;
			tmp_qname_size = 0;
		}
	}
	
	tmp_qname[size_p] = tmp_qname_size;

	packet_data_log(tmp_qname,qname_size+1);

	eth_info("rtl8168_set_passthrough_s: %s \n",tmp_qname);

	rtl8168_set_passthrough(tmp_qname,qname_size+1);
	return 0;
}

ssize_t rtl8168_wol_mdns_offload_passthrough_store(
			struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	int qname_len = 0,in_val = 0;
	unsigned char *buforg = NULL;
	unsigned char *tokens[5] = {0};	// Assuming you have a maximum of 10 tokens
	unsigned char *delimiter = ",";	// The delimiter is a comma ","
	unsigned char * str_pointer = NULL;
	int tokenCount = 0;
	int ret = 0;

	buforg = kmalloc(count + 1, GFP_KERNEL);

	if (!buforg) {
		goto error;
	}

	memcpy(buforg, buf, count);

	tokenCount = rtl8168_split_string(buforg, delimiter, tokens, 3, &str_pointer);
	// echo add,qname,qname size > /sys/etn/wol_mdns_offload_passthrough
	if (0 == strncmp(tokens[0], "add", strlen("add"))) {
		if(tokenCount != 3)
		{
			eth_err("	tokenCount != 3 \n");
			goto error;
		}

		if (sscanf(tokens[1], "%d", &in_val) != 1) {
			eth_err(" qname size = %s\n", tokens[1]);
			goto error;
		}

		eth_info("rtl8168_wol_mdns_offload_passthrough_store add \n");
		qname_len = in_val;
		rtl8168_set_passthrough(tokens[2],qname_len);
	}// echo clean > /sys/etn/wol_mdns_offload_passthrough
	else if((0 == strncmp(tokens[0], "s_add", strlen("s_add")))) {
		
		if(tokenCount != 3)
		{
			eth_err("	tokenCount != 3 \n");
			goto error;
		}

		if (sscanf(tokens[1], "%d", &in_val) != 1) {
			eth_err(" qname size = %s\n", tokens[1]);
			goto error;
		}
		eth_info("rtl8168_wol_mdns_offload_passthrough_store s_add \n");
		qname_len = in_val;
		ret = rtl8168_set_passthrough_s(tokens[2],qname_len);
		if(ret == -1)
		{
			eth_err("rtl8168_set_passthrough_s fail \n");
			goto error;
		}

	}
	else if((0 == strncmp(tokens[0], "clean", strlen("clean")))) {
		if(tokenCount != 1)
		{
			eth_err("	tokenCount != 1 \n");
			goto error;
		}
		memset(&passthrough_list, 0, sizeof(struct MdnsOffload_passthrough_list));
	}

	rtl8168_free_split_string(tokens,tokenCount);
	if (buforg) {
		kfree(buforg);
	}

	return count;

error:
	rtl8168_free_split_string(tokens,tokenCount);
	if (buforg) {
		kfree(buforg);
	}

	return -1;
}


void packet_data_log(unsigned char *packet, unsigned int size)
{
	unsigned char *buf = NULL;
	unsigned int i =0;
	unsigned char tmp[8] = {0};

	buf = vmalloc(8192 * sizeof(unsigned char));
	memset(buf,0,8192 * sizeof(unsigned char));
	for (i = 0; i < size; i++)
	{
		snprintf(tmp, 3,"%02x", packet[i]);
		strcat(buf,tmp);
		memset(tmp,0,sizeof(tmp));
	}
	eth_err("HEX: %s \n",buf);
	vfree(buf);
}

void printf_domain_debug_log(void)
{
	int i=0, j=0;
	for(i=0; i < mdns_data_list.list_size;i++)
	{

			for(j=0; j < mdns_data_list.list[i].type_size; j++)
			{
				eth_err("list: %d type list: %d nameoffset = %x type = %x\n", i, j ,mdns_data_list.list[i].type[j].nameoffset, mdns_data_list.list[i].type[j].qtype);

				eth_parse_mdns_domain_name(mdns_data_list.list[i].packet,  mdns_data_list.list[i].packet_size, mdns_data_list.list[i].type[j].nameoffset);
			}
			packet_data_log(mdns_data_list.list[i].packet,mdns_data_list.list[i].packet_size);
	}
}

ssize_t rtl8168_wol_mdns_offload_protocol_data_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
    printf_domain_debug_log();

    return 0;
}

static int rtl8168_verify_protocol_data_type(int packet_index,int type,int nameoffset)
{
		int i = 0;

		for (i = 0; i < mdns_data_list.list[packet_index].type_size; i++) {
				if (type == mdns_data_list.list[packet_index].type[i].qtype && 
					nameoffset == mdns_data_list.list[packet_index].type[i].nameoffset) {
						eth_err(" Already set qtype: %x nameoffset: %x\n", type, nameoffset);
						return 1;
				}
		}

		if ( mdns_data_list.list[packet_index].type_size >= MDNS_TYPE_MAX)
		{
				eth_err(" mdns_data_list.list[packet_index].type_size >= MDNS_TYPE_MAX \n");
				return -1;
		}

		return 0;
}


static void rtl8168_set_protocol_data_type(int packet_index, int type, int nameoffset)
{
	if(0 == rtl8168_verify_protocol_data_type(packet_index, type, nameoffset))
	{
		mdns_data_list.list[packet_index].type[mdns_data_list.list[packet_index].type_size].nameoffset = nameoffset;
		mdns_data_list.list[packet_index].type[mdns_data_list.list[packet_index].type_size].qtype = type;
		mdns_data_list.list[packet_index].type_size++;
	}
	else
	{
		eth_err(" rtl8168_set_protocol_data_type fail\n");
	}
}

static int rtl8168_verify_protocol_data_packet(unsigned char* packet, int packet_size)
{
		int i = 0;

		for (i = 0; i < mdns_data_list.list_size; i++) {
				eth_err(" mdns_data_list.list[%d].packet: %s \n", i, mdns_data_list.list[i].packet);
				if ( mdns_data_list.list[i].packet_size == packet_size && (0 == memcmp(mdns_data_list.list[i].packet, packet, packet_size))) {
						return i;
				}
		}

		if ( mdns_data_list.list_size >= MDNS_PACKET_MAX)
		{
				eth_err(" mdns_data_list.list_size >= MDNS_PACKET_MAX \n");
				return -1;
		}

		return -2;
}

static int rtl8168_set_protocol_data_packet(unsigned char* packet_data, int type, int nameoffset, int packet_size)
{
		int ret = rtl8168_verify_protocol_data_packet(packet_data,packet_size);
		if(-2 == ret)
		{
				memcpy(mdns_data_list.list[mdns_data_list.list_size].packet, packet_data, packet_size);
				mdns_data_list.list[mdns_data_list.list_size].type[0].nameoffset = nameoffset;
				mdns_data_list.list[mdns_data_list.list_size].type[0].qtype = type;
				mdns_data_list.list[mdns_data_list.list_size].packet_size = packet_size;
				mdns_data_list.list[mdns_data_list.list_size].type_size = 1;
				mdns_data_list.list_size++;
		}
		else if( -1 == ret)
		{
				eth_err(" rtl8168_set_protocol_data_packet fail\n");
		}

		return ret;
}

ssize_t rtl8168_wol_mdns_offload_protocol_data_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{

	int type = 0, nameoffset = 0, packet_size = 0, in_val = 0, packet_index = 0;
	unsigned char *buforg = NULL;
	unsigned char *tokens[5] = {0};	// Assuming you have a maximum of 10 tokens
	unsigned char *delimiter = ",";	// The delimiter is a comma ","
	unsigned char * str_pointer = NULL;
	int tokenCount = 0;

	buforg = kmalloc(count + 1, GFP_KERNEL);

	if (!buforg) {
		goto error;
	}

	memcpy(buforg, buf, count);

	tokenCount = rtl8168_split_string(buforg, delimiter, tokens, 4, &str_pointer);

	// echo add,qtype,offset,packet size,packet > /sys/etn/wol_mdns_offload_protocol_data
	if (0 == strncmp(tokens[0], "add", strlen("add"))) {
		if(tokenCount != 4)
			goto error;
		if (sscanf(tokens[1], "%d", &in_val) != 1) {
			eth_err(" type = %s\n", tokens[1]);
			goto error;
		}
		type = in_val;
		if (sscanf(tokens[2], "%d", &in_val) != 1) {
			eth_err(" nameoffset = %s\n", tokens[2]);
			goto error;
		}
		nameoffset = in_val;
		if (sscanf(tokens[3], "%d", &in_val) != 1) {
			eth_err(" packet_size = %s\n", tokens[3]);
			goto error;
		}
		packet_size = in_val;
		packet_index = rtl8168_set_protocol_data_packet(str_pointer, type, nameoffset, packet_size);

		if(packet_index >=0)
		{
				rtl8168_set_protocol_data_type(packet_index, type, nameoffset);
		}
	}// echo clean > /sys/etn/wol_mdns_offload_protocol_data
	else if((0 == strncmp(tokens[0], "clean", strlen("clean")))) {
		if(tokenCount != 1)
		{
			eth_err("	tokenCount != 1 \n");
			goto error;
		}
		memset(&mdns_data_list, 0, sizeof(struct MdnsProtocolData_list));
	}

	rtl8168_free_split_string(tokens,tokenCount);
	if (buforg) {
		kfree(buforg);
	}

	return count;

error:
	rtl8168_free_split_string(tokens,tokenCount);
	if (buforg) {
		kfree(buforg);
	}

	return -1;
}
ssize_t rtl8168_wol_mdns_offload_passthrough_behaviour_show(
			struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	if (passthrough_behaviour == FORWARD_ALL) {
		return sprintf(buf, "passthrough_behaviour: FORWARD_ALL\n");
	}
	else if (passthrough_behaviour == DROP_ALL) {
		return sprintf(buf, "passthrough_behaviour: DROP_ALL\n");
	}
	else if (passthrough_behaviour == PASSTHROUGH_LIST) {
		return sprintf(buf, "passthrough_behaviour: PASSTHROUGH_LIST\n");
	}
	return 0;
}
ssize_t rtl8168_wol_mdns_offload_passthrough_behaviour_store(
			struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	int in_val;

	if (sscanf(buf, "%d", &in_val) != 1) {
		goto error;
	}
	if (in_val == FORWARD_ALL || in_val == DROP_ALL || in_val == PASSTHROUGH_LIST) {
		passthrough_behaviour = in_val;
	}
	else {
		goto error;
	}
	return count;
error:
	return -1;

}

ssize_t rtl8168_wol_ipv4_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "ipv4: %d.%d.%d.%d \n", g_wol_ipv4_addr[0], g_wol_ipv4_addr[1], g_wol_ipv4_addr[2],g_wol_ipv4_addr[3]);
}

ssize_t rtl8168_wol_ipv4_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
	int i = 0;
	unsigned int ipv4[4] = {0};
	if (sscanf(buf, "%u.%u.%u.%u", &ipv4[0], &ipv4[1], &ipv4[2], &ipv4[3]) != 4)
		goto error;
	
	for(i = 0; i < 4; i++) {
		if(ipv4[i] > 255)
		goto error;
	g_wol_ipv4_addr[i] = ipv4[i] & 0xFF;
	}
	return count;
error:
	return -1;
}

ssize_t rtl8168_wol_ipv6_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "ipv6: %hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx\n", ntohs(g_wol_ipv6_addr[0]), ntohs(g_wol_ipv6_addr[1]), ntohs(g_wol_ipv6_addr[2]),ntohs(g_wol_ipv6_addr[3]),
																	ntohs(g_wol_ipv6_addr[4]), ntohs(g_wol_ipv6_addr[5]), ntohs(g_wol_ipv6_addr[6]),ntohs(g_wol_ipv6_addr[7]));
}

ssize_t rtl8168_wol_ipv6_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
	int i = 0;
	unsigned short ipv6[8];
	unsigned short big_endian_val[8];
	if (sscanf(buf, "%hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx", &ipv6[0], &ipv6[1], &ipv6[2], &ipv6[3], &ipv6[4], &ipv6[5], &ipv6[6], &ipv6[7]) != 8)
		goto error;
	for (i=0;i<8;i++) {
		big_endian_val[i] = htons(ipv6[i]);
	}
	memcpy(g_wol_ipv6_addr, big_endian_val, sizeof(big_endian_val));
	return count;
error:
	return -1;
}


ssize_t rtl8168_etn_mdns_offfload_state_store(
			struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	int in_val;

	if (sscanf(buf, "%d", &in_val) != 1) {
		goto error;
	}
	if (in_val == 1 || in_val == 0) {
		mdns_offfload_state = in_val;
	}
	else {
		goto error;
	}
	return count;
error:
	return -1;
}

ssize_t rtl8168_etn_mdns_offfload_state_show(
			struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "mdns_offfload_state = %d \n", mdns_offfload_state);
}

static inline unsigned char *_rtl8168_set_be_int(
			unsigned char *buf, unsigned int val)
{
	unsigned int *p_int_ptr = (unsigned int *)buf;
	*p_int_ptr =	cpu_to_be32(val);
	return (buf + 4);
}

static inline unsigned char *_rtl8168_set_be_short(
			unsigned char *buf, unsigned short val)
{
	unsigned short *p_short_ptr = (unsigned short *)buf;
	*p_short_ptr =	cpu_to_be16(val);
	return (buf + 2);
}

static inline unsigned char *_rtl8168_set_string(
			unsigned char *buf, unsigned char *data, unsigned int len)
{
	memcpy(buf, data, len);
	return (buf + len);
}

static unsigned char *_rtl8168_set_mdns_offload_global_param
(struct net_device *ndev, unsigned char *buffer)
{
	unsigned short global_flags = 0;
	unsigned char *cur_ptr = buffer;
	cur_ptr = _rtl8168_set_string(cur_ptr, "GLOB", 4);
	cur_ptr = _rtl8168_set_string(cur_ptr, ndev->dev_addr, 6);
	cur_ptr = _rtl8168_set_string(cur_ptr, g_wol_ipv4_addr, 4);
	cur_ptr = _rtl8168_set_string(cur_ptr, (unsigned char *)g_wol_ipv6_addr, 16);

	switch (passthrough_behaviour) {
		case FORWARD_ALL:
			global_flags |= 0x0;
			break;
		case DROP_ALL:
			global_flags |= 0x1;
			break;
		case PASSTHROUGH_LIST:
		default:
			global_flags |= 0x2;
			break;
	}
	if (mdns_offfload_state == MdnsOffloadState_ENABLE) {
		global_flags = global_flags | (0x1 << 2);
	}

	cur_ptr = _rtl8168_set_be_short(cur_ptr, global_flags);
	return cur_ptr;
}

static unsigned char *_rtl8168_set_mdns_offload_raw_packet_pattern(
			struct net_device *ndev, unsigned char *buffer)
{
	unsigned int pattern_cnt = 0;
	unsigned char *cur_ptr = buffer;
	unsigned int i = 0;
	unsigned int list_size = mdns_data_list.list_size;

	if (list_size > MDNS_PACKET_MAX) {
		list_size = 0;
	}

	cur_ptr += 8;
	for (i = 0; i < list_size; i++) {
		unsigned int n;
		unsigned int pattern_size = 0;
		unsigned short material_size = 0;
		unsigned char *bak_ptr = cur_ptr;

		if (!mdns_data_list.list[i].type_size
				|| mdns_data_list.list[i].type_size > MDNS_TYPE_MAX
				|| !mdns_data_list.list[i].packet_size
				|| mdns_data_list.list[i].packet_size > MDNS_MAX_PACKET_SIZE) {
			continue;
		}

		material_size = mdns_data_list.list[i].type_size * 4;
		pattern_size = ETN_ROUND_UP(material_size + mdns_data_list.list[i].packet_size, 4);

		cur_ptr = _rtl8168_set_string(cur_ptr, "PACT", 4);
		cur_ptr = _rtl8168_set_be_int(cur_ptr, pattern_size);
		cur_ptr = _rtl8168_set_be_short(cur_ptr, material_size);
		cur_ptr = _rtl8168_set_be_short(cur_ptr, mdns_data_list.list[i].packet_size);

		for (n = 0; n < mdns_data_list.list[i].type_size; n++) {
			cur_ptr = _rtl8168_set_be_short(cur_ptr,
											mdns_data_list.list[i].type[n].qtype);
			cur_ptr = _rtl8168_set_be_short(cur_ptr,
											mdns_data_list.list[i].type[n].nameoffset);
		}
		cur_ptr = _rtl8168_set_string(cur_ptr,
										mdns_data_list.list[i].packet, mdns_data_list.list[i].packet_size);

		cur_ptr = bak_ptr + 12 + pattern_size;

		pattern_cnt++;
	}

	buffer = _rtl8168_set_string(buffer, "RAWP", 4);
	buffer = _rtl8168_set_be_int(buffer, pattern_cnt);

	return cur_ptr;
}

static unsigned char *_rtl8168_set_mdns_offload_passthrough_pattern(
			struct net_device *ndev, unsigned char *buffer)
{
	unsigned int pattern_cnt = 0;
	unsigned char *cur_ptr = buffer;
	unsigned int i = 0;
	unsigned int list_size = passthrough_list.list_size;
	if (list_size > MDNS_PASSTHROUGH_MAX) {
		list_size = 0;
	}

	cur_ptr += 8;
	for (i = 0; i < list_size; i++) {
		unsigned char *bak_ptr = cur_ptr;
		unsigned int size = 0;

		if (!passthrough_list.list[i].qname_len
				|| passthrough_list.list[i].qname_len > 255) {
			continue;
		}

		size = ETN_ROUND_UP(passthrough_list.list[i].qname_len, 4);

		cur_ptr = _rtl8168_set_string(cur_ptr, "QNME", 4);

		cur_ptr = _rtl8168_set_be_int(cur_ptr, size);

		cur_ptr = _rtl8168_set_be_int(cur_ptr, passthrough_list.list[i].qname_len);

		cur_ptr = _rtl8168_set_string(cur_ptr, passthrough_list.list[i].qname, passthrough_list.list[i].qname_len);

		cur_ptr = bak_ptr + 12 + size;

		pattern_cnt++;
	}

	buffer = _rtl8168_set_string(buffer, "PAST", 4);
	buffer = _rtl8168_set_be_int(buffer, pattern_cnt);
	return cur_ptr;
}

static unsigned char *_rtl8168_set_mdns_wakeup_frame_pattern(
			struct net_device *ndev, unsigned char *buffer)
{
	unsigned int pattern_cnt = 0;
	unsigned char *cur_ptr = buffer;
	unsigned int i = 0;
	unsigned int list_size = wakeup_source_list.list_size;
	if (list_size > MAX_PORT_WAKE_SOURCE_CNT) {
		list_size = 0;
	}

	cur_ptr += 8;
	for (i = 0; i < list_size; i++) {
		unsigned char *bak_ptr = cur_ptr;
		unsigned int size = 0;
		size = 4;
		cur_ptr = _rtl8168_set_string(cur_ptr, "WPRT", 4);
		cur_ptr = _rtl8168_set_be_int(cur_ptr, size);
		cur_ptr = _rtl8168_set_be_int(cur_ptr, size);
		cur_ptr = _rtl8168_set_be_short(cur_ptr, wakeup_source_list.list[i].flags);
		cur_ptr = _rtl8168_set_be_short(cur_ptr, wakeup_source_list.list[i].port);
		cur_ptr = bak_ptr + 12 + size;
		pattern_cnt++;
	}

	buffer = _rtl8168_set_string(buffer, "WFRM", 4);
	buffer = _rtl8168_set_be_int(buffer, pattern_cnt);
	return cur_ptr;
}

void rtl8168_set_vcpu_mdns_offload_params(struct net_device *ndev)
{

	unsigned char *tmp = NULL;
	unsigned char *param_buf = (unsigned char *)phys_to_virt(ETH_VCPU_MDNS_OFFLOAD_BUFFER_START_ADDR);
	if (!param_buf) {
		eth_err("%s can not mapping phy addr %x to virt\n", __func__,
				ETH_VCPU_MDNS_OFFLOAD_BUFFER_START_ADDR);
		return;
	}
	tmp = param_buf;

	tmp = _rtl8168_set_mdns_offload_global_param(ndev, tmp);
	tmp = _rtl8168_set_mdns_offload_raw_packet_pattern(ndev, tmp);
	tmp = _rtl8168_set_mdns_offload_passthrough_pattern(ndev, tmp);
	tmp = _rtl8168_set_mdns_wakeup_frame_pattern(ndev, tmp);
}

static void rtl_upload_mdns_pkt_task(struct work_struct *work)
{
	unsigned char *param_buf = NULL;
	unsigned int pkt_size = 0;
	struct sk_buff *skb = NULL;

	param_buf = (unsigned char *)phys_to_virt(ETH_VCPU_MDNS_OFFLOAD_BUFFER_START_ADDR);
	if(!param_buf)
		return;

	if(memcmp(param_buf, "MNPT", 4) != 0)
		return;
	memset(param_buf, 0x0, 4);

	pkt_size = be32_to_cpu(*((unsigned int *)(param_buf + 4)));
	if(pkt_size <= 64 || pkt_size > 1536)
		return;
	memset(param_buf + 4, 0x0, 4);

	skb = netdev_alloc_skb_ip_align(g_upload_worker.dev, pkt_size);
	if (!skb)
		return;
	memcpy(skb->data, param_buf + 8, pkt_size);
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb_put(skb, pkt_size);
	skb->protocol = eth_type_trans(skb, g_upload_worker.dev);
	netif_receive_skb(skb);
	eth_info("[mdsn_offload]upload mdns packet, size:%u\n", pkt_size);

}

void rtl8168_schedule_mdns_packet_upload_work(struct net_device *dev)
{
	if(!g_need_upload_mdns_packet)
		return;
	g_need_upload_mdns_packet = false;
	schedule_work(&g_upload_worker.work);
}

void rtl8168_check_mdns_packet_upload_flag(struct net_device *dev)
{
	unsigned int wake_reason = 0x0;
	if(rtd_inl(ETH_WAKE_DUMMY_REG) & ETH_WOL_DUMMY_WAKE_REASON_EFFECTED)
		wake_reason = rtd_inl(ETH_WAKE_DUMMY_REG) & ETH_WOL_DUMMY_WAKE_REASON_MASK;
	if(wake_reason == 0x20)
		g_need_upload_mdns_packet = true;
	else
		g_need_upload_mdns_packet = false;
}

void rtl8168_init_mdns_packet_upload_work(struct net_device *dev)
{
	g_upload_worker.dev = dev;
	INIT_WORK(&g_upload_worker.work, rtl_upload_mdns_pkt_task);
}

void rtl8168_cancel_mdns_packet_upload_work(struct net_device *dev)
{
	cancel_work_sync(&g_upload_worker.work);
}

static void rtl8168_write_hw_array(struct rtl8168_private *tp, unsigned short start_addr, 
							unsigned char *data, unsigned int len)
{
	unsigned short val = 0x0;
	unsigned int loop = 0;
	unsigned int i = 0;
	if(start_addr % 2) {
		start_addr = start_addr & 0xFFFE;
		val = r8168_mac_ocp_read(tp, start_addr);
		val = ((val & 0x00FF) | (data[0] << 8));
		r8168_mac_ocp_write(tp, start_addr, val);
		data++;
		len--;
		start_addr += 2;
	}
	loop = (len >> 1);
	for(i = 0; i < loop; i++) {
		val = (data[0] | (data[1] << 8));
		r8168_mac_ocp_write(tp, start_addr, val);
		start_addr += 2;
		data += 2;
		len -= 2;
	}
	if(len)
		r8168_mac_ocp_write(tp, start_addr, data[0]);
}

struct hw_mdns_offload_raw_packet_info
{
	unsigned int max_avai_buffer_len;
	unsigned int total_packet_cnt;
	unsigned int total_packet_len;
	unsigned int total_rr_cnt;
	unsigned int total_rr_len;
	unsigned int last_rr_len;
	unsigned int start_addr;
	unsigned char is_overflow;
	unsigned char	raw_packet_table[MDNS_PACKET_MAX];
};

static unsigned int _rtl8168_set_hw_mdns_offload_passthrough_pattern(struct rtl8168_private *tp)
{
	unsigned int pattern_cnt = 0;
	unsigned int i = 0;
	unsigned int len = 0;
	eth_info("start to set passthrought pattern, pattern list size:%u\n", passthrough_list.list_size);
	for (i = 0; i < passthrough_list.list_size; i++) {
		if (!passthrough_list.list[i].qname_len
			|| passthrough_list.list[i].qname_len > 255)
			continue;
		passthrough_list.list[i].qname[passthrough_list.list[i].qname_len] = 0x0;
		eth_info("passthrough pattern[%d], start_addr: 0x%x, qname len 0x%x(include 0x00)\n", 
					(i + 1), rtl8168_get_hw_mdns_offload_passthrough_list_content_addr(tp) + len, passthrough_list.list[i].qname_len + 1);
		rtl8168_write_hw_array(tp, rtl8168_get_hw_mdns_offload_passthrough_list_content_addr(tp) + len, 
								passthrough_list.list[i].qname, passthrough_list.list[i].qname_len + 1);
		r8168_mac_ocp_write(tp, OFFLOAD_PASSTHROUGH_LENGTH_TABLE_ADDR + i * 2, 
								cpu_to_le32(passthrough_list.list[i].qname_len + 1));
		len += (passthrough_list.list[i].qname_len + 1);
		pattern_cnt++;
		if(pattern_cnt >= 8)
			break;
		
	}
	eth_info("end to set passthrought pattern, successed pattern cnt:%u, total len:0x%x\n", pattern_cnt, len);
	r8168_mac_ocp_write(tp, OFFLOAD_PASSTHROUGH_COUNT_ADDR, cpu_to_le32(pattern_cnt));
	return len;
}

static unsigned int _rtl8168_set_hw_mdns_offload_raw_packet_rr_pattern(struct rtl8168_private *tp, 
				struct hw_mdns_offload_raw_packet_info *packet_info,
				struct MdnsProtocolData *protocol_data)
{
	unsigned int n = 0;
	unsigned int cur_rr_cnt = 0;
	unsigned char rName[258];//max 255 + 0x00 + type(2byte)
	unsigned int rr_len = 0;
	unsigned int rr_start_addr = packet_info->start_addr;
	
	for(n = 0; n < protocol_data->type_size; n++) {
		int rlen = rtk_8168_mdns_parse_field(protocol_data->packet, protocol_data->packet_size,
					protocol_data->type[n].nameoffset, rName);
		if(rlen <= 0 || rlen > 255)
			continue;
		rName[rlen] = 0;
		rName[rlen + 1] = ((protocol_data->type[n].qtype & 0xFF00) >> 8);
		rName[rlen + 2] = protocol_data->type[n].qtype & 0xFF;
		rlen += 3;
		if(packet_info->total_packet_len + packet_info->total_rr_len + rlen > packet_info->max_avai_buffer_len) {
				packet_info->is_overflow = true;
				break;
		}
		eth_info("parsed one rr qname, start addr: 0x%x, len:0x%x(include 0x00 + 2byte qtype)\n", rr_start_addr, rlen);
		rtl8168_write_hw_array(tp, rr_start_addr, rName, rlen);
		rr_start_addr+= rlen;
		cur_rr_cnt++;	
		rr_len += rlen;
	}
	if(!packet_info->is_overflow && cur_rr_cnt) {
		packet_info->total_rr_cnt += cur_rr_cnt;
		packet_info->total_rr_len += rr_len;
		packet_info->start_addr = rr_start_addr;
		packet_info->last_rr_len = rr_len;
	}
	return cur_rr_cnt;
}

static void _rtl8168_set_hw_mdns_offload_raw_packet_pattern(struct rtl8168_private *tp, unsigned int rr_start_addr)
{
	struct hw_mdns_offload_raw_packet_info packet_info;
	unsigned int i = 0;
	unsigned int rawpacket_start_addr = 0;
	memset(&packet_info, 0, sizeof(packet_info));
	packet_info.max_avai_buffer_len = MAX_OFFLOAD_DATA_ADDR - rr_start_addr;
	packet_info.start_addr = rr_start_addr;
	eth_info("start to set rr pattern,start addr:0x%x, list size:%u, avail buffer:0x%x\n", 
					rr_start_addr, mdns_data_list.list_size, packet_info.max_avai_buffer_len);
		
	for (i = 0; i < mdns_data_list.list_size; i++) {
		unsigned int cur_rr_cnt = 0;
		if (!mdns_data_list.list[i].type_size
			|| mdns_data_list.list[i].type_size > MDNS_TYPE_MAX
			|| !mdns_data_list.list[i].packet_size
			|| mdns_data_list.list[i].packet_size > MDNS_MAX_PACKET_SIZE)
			continue;
		eth_info("set rr pattern %u, matrial size:%u, packet size:%u\n", (i + 1), mdns_data_list.list[i].type_size, mdns_data_list.list[i].packet_size);
		cur_rr_cnt = _rtl8168_set_hw_mdns_offload_raw_packet_rr_pattern(tp, &packet_info, &mdns_data_list.list[i]);
		if(packet_info.is_overflow)
			break;
		if(cur_rr_cnt != 0) {
			if(packet_info.total_packet_len + packet_info.total_rr_len + mdns_data_list.list[i].packet_size > packet_info.max_avai_buffer_len) {
				packet_info.total_rr_cnt -= cur_rr_cnt;
				packet_info.total_rr_len -= packet_info.last_rr_len;
				packet_info.is_overflow = true;
				break;
			}
			r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_TABLE_ADDR + packet_info.total_packet_cnt * 4, cpu_to_le32(cur_rr_cnt));
			r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_TABLE_ADDR + 0x2 + packet_info.total_packet_cnt * 4, cpu_to_le32(mdns_data_list.list[i].packet_size));
			packet_info.raw_packet_table[i] = 1;
			packet_info.total_packet_len += mdns_data_list.list[i].packet_size;
			packet_info.total_packet_cnt++;
		}
		
	}
	eth_info("total packet set:%u, total rr cnt:%u\n", packet_info.total_packet_cnt, packet_info.total_rr_cnt);
	r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_COUNT_ADDR, cpu_to_le32(packet_info.total_packet_cnt));
	r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_START_ADDR, cpu_to_le32(rr_start_addr + packet_info.total_rr_len));
	r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_RR_COUNT_ADDR, cpu_to_le32(packet_info.total_rr_cnt));
	r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_RR_START_ADDR, cpu_to_le32(rr_start_addr));

	rawpacket_start_addr = rr_start_addr + packet_info.total_rr_len;
	for(i = 0; i < mdns_data_list.list_size; i++) {
		if(packet_info.raw_packet_table[i]) {
			eth_info("set rawpackt[%u] data start addr:0x%x, packet size:%u\n", (i + 1), rawpacket_start_addr, mdns_data_list.list[i].packet_size);
			rtl8168_write_hw_array(tp, rawpacket_start_addr , mdns_data_list.list[i].packet, mdns_data_list.list[i].packet_size);
			rawpacket_start_addr += mdns_data_list.list[i].packet_size;
		}
	}

}

static void rtl8168_set_hw_mdns_offload_mac_ip_param(struct rtl8168_private *tp)
{
	struct net_device *ndev = tp->dev;

	eth_info("ipaddr=%d.%d.%d.%d\n", g_wol_ipv4_addr[0],	g_wol_ipv4_addr[1], g_wol_ipv4_addr[2], g_wol_ipv4_addr[3]);	

	//set ip
	r8168_mac_ocp_write(tp, OFFLOAD_IPV4_ADDR_ADDR + 0x0, (g_wol_ipv4_addr[1] << 8) | g_wol_ipv4_addr[0]);
	r8168_mac_ocp_write(tp, OFFLOAD_IPV4_ADDR_ADDR + 0x2, (g_wol_ipv4_addr[3] << 8) | g_wol_ipv4_addr[2]);
	//set ipv6
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0x0, g_wol_ipv6_addr[0]);
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0x2, g_wol_ipv6_addr[1]);	
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0x4, g_wol_ipv6_addr[2]);
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0x6, g_wol_ipv6_addr[3]);	
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0x8 ,g_wol_ipv6_addr[4]);
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0xa, g_wol_ipv6_addr[5]);	
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0xc ,g_wol_ipv6_addr[6]);
	r8168_mac_ocp_write(tp, OFFLOAD_IPV6_ADDR_ADDR + 0xe, g_wol_ipv6_addr[7]);	
	//set mac
	r8168_mac_ocp_write(tp, OFFLOAD_MAC_ADDR_ADDR + 0x0, (ndev->dev_addr[1] << 8) | ndev->dev_addr[0]);
	r8168_mac_ocp_write(tp, OFFLOAD_MAC_ADDR_ADDR + 0x2, (ndev->dev_addr[3] << 8) | ndev->dev_addr[2]);
	r8168_mac_ocp_write(tp, OFFLOAD_MAC_ADDR_ADDR + 0x4, (ndev->dev_addr[5] << 8) | ndev->dev_addr[4]);
	//set mac_id_vlan
	r8168_mac_ocp_write(tp, OFFLOAD_VLAN0_MAC_ADDR_ADDR + 0x0, (ndev->dev_addr[1] << 8) | ndev->dev_addr[0]); 
	r8168_mac_ocp_write(tp, OFFLOAD_VLAN0_MAC_ADDR_ADDR + 0x2, (ndev->dev_addr[3] << 8) | ndev->dev_addr[2]);
	r8168_mac_ocp_write(tp, OFFLOAD_VLAN0_MAC_ADDR_ADDR + 0x4, (ndev->dev_addr[5] << 8) | ndev->dev_addr[4]);	
}

static void _rtl8168_set_hw_mdns_offload_global_param(struct rtl8168_private *tp)
{
	unsigned int passthrough_param = 0;
	switch (passthrough_behaviour) {
	case FORWARD_ALL:
			passthrough_param = 0x2;
			break;
	case DROP_ALL:
			passthrough_param = 0x1;
			break;
		case PASSTHROUGH_LIST:
		default:
			passthrough_param = 0x0;
			break;
	}
	r8168_mac_ocp_write(tp, OFFLOAD_WAKE_REASON_ADDR, 0);
	if(mdns_offfload_state != MdnsOffloadState_DISSABLE)
		r8168_mac_ocp_write(tp, OFFLOAD_PASSBEHAVIOUR_ADDR, cpu_to_le32(passthrough_param));
	else
		r8168_mac_ocp_write(tp, OFFLOAD_PASSBEHAVIOUR_ADDR, cpu_to_le32(0x2));
	r8168_mac_ocp_write(tp, OFFLOAD_WAKE_UP_PACKET_LEN_ADDR, 0);
	r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_COUNT_ADDR, 0x0);
	r8168_mac_ocp_write(tp, OFFLOAD_RAWPACKET_RR_COUNT_ADDR, 0x0);
	r8168_mac_ocp_write(tp, OFFLOAD_PASSTHROUGH_COUNT_ADDR, 0x0);
	r8168_mac_ocp_write(tp, OFFLOAD_RESPONSE_COUNTER_ADDR, 0x0);
	r8168_mac_ocp_write(tp, OFFLOAD_MISS_COUNTER_ADDR, 0x0);
	
}

void rtl8168_set_hw_mdns_offload_params(struct rtl8168_private *tp)
{
	_rtl8168_set_hw_mdns_offload_global_param(tp);
	if(mdns_offfload_state != MdnsOffloadState_DISSABLE) {
		unsigned int offset = _rtl8168_set_hw_mdns_offload_passthrough_pattern(tp);
		_rtl8168_set_hw_mdns_offload_raw_packet_pattern(tp, rtl8168_get_hw_mdns_offload_passthrough_list_content_addr(tp) + offset);
	}
}

void rtk8168_hw_mdns_standby_pre_oob_setting(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	int count;

	/* (1) Set Now_is_OOB = 0 (IO offset 0xd3 bit 7 set to 0b) */
	RTL_W8(MCU, RTL_R8(MCU) & ~NOW_IS_OOB);

	/* (2) Flow control related (only for OOB) */	
	r8168_mac_ocp_write(tp, 0xC0A0, 0x03);
	r8168_mac_ocp_write(tp, 0xC0A2, 0x180);
	r8168_mac_ocp_write(tp, 0xC0A4, 0x4A);
	r8168_mac_ocp_write(tp, 0xC0A8, 0x5A);
			
	/* (3)Turn off RCR (IO offset 0x44 set to 0) */
	RTL_W32(RxConfig, 0x00000000);

	/* (4) Set rxdv_gated_en (IO 0xF2 bit3 = 1) */
	RTL_W32(MISC, RTL_R32(MISC) | RXDV_GATED_EN);

	/* (5) check FIFO empty (IO 0xD2 bit[12:13]) */
	count = 0;
	while(1)
	{
		if (RTL_R8(MCU) & TX_EMPTY)
		{
			if (RTL_R8(MCU) & RX_EMPTY)
				break;
		}
		usleep_range(1000, 1000);
		count++;
		if(count > 2000) {
			eth_debug("check Tx/RX FIFO empty fail:%x\n", RTL_R8(MCU));
			break;
		}
	}

	/* (6) disable Tx/Rx enable = 0 (IO 0x36 bit[10:11]=0b) */
	RTL_W8(ChipCmd, RTL_R8(ChipCmd) & ~(CmdTxEnb | CmdRxEnb));

	/* (7) check link_list ready =1 (IO 0xD2 bit9=1) */
	count = 0;
	while(1) {
		if (RTL_R8(MCU) & LINK_LIST_RDY)
			break;
		usleep_range(1000, 1000);
		count++;
		if(count > 2000) {
			eth_err("check link_list ready fail != 1-%x\n", RTL_R8(MCU));
			break;
		}
	}

	/* (8) set re_ini_ll =1 (MACOCP : 0xE8DE bit15=1) */
	r8168_mac_ocp_write(tp, 0xE8DE, r8168_mac_ocp_read(tp, 0xE8DE) | (1 << 15));

	/* (9) check link_list ready =1 (IO 0xD2 bit9=1) */
	count = 0;
	while(1) {
		if (RTL_R8(MCU) & LINK_LIST_RDY)
			break;
		
		usleep_range(1000, 1000);
		count++;
		if(count > 2000) {
			eth_err("check link_list ready fail != 2-%x\n", RTL_R8(MCU));
			break;
		}
	}

	/* (10) Setting RMS (IO offset 0xdb-0xda set to 0x05F3) */
	RTL_W16(RxMaxSize, 0x05F3);

	rtl8168_set_hw_mdns_offload_mac_ip_param(tp);
	/* (14) Enable now_is_oob (IO offset 0xd3 bit 7 set to 1b) */
	RTL_W8(MCU, RTL_R8(MCU) | NOW_IS_OOB | DIS_MCU_CLROOB);
	/* (15) set MACOCP 0xE8DE bit14 mcu_borw_en to 1b for modifying ShareFIFO's points */
	r8168_mac_ocp_write(tp, 0xE8DE, r8168_mac_ocp_read(tp, 0xE8DE) | (1 << 14));//mcu_borw_en
	RTL_W8(Cfg9346, Cfg9346_Unlock);
	RTL_W8(Config5, RTL_R8(Config5) | LanWake);
	RTL_W8(Config3, RTL_R8(Config3) | MagicPacket);
	RTL_W8(Cfg9346, Cfg9346_Lock);
	
}

void rtk8168_hw_mdns_standby_post_oob_setting(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	r8168_mac_ocp_write(tp, 0xC010, 0xC);//enable rx packet filetr
	r8168_mac_ocp_write(tp, 0xC0B6, 0x0403);//cfg_en_rxtp_r
	r8168_mac_ocp_write(tp, 0xCDB2, 0xE914);//rsvd5_port
	r8168_mac_ocp_write(tp, 0xC0C0, 0x20);//proxy_udp_port_sel
	r8168_mac_ocp_write(tp, 0xC0BE, 0x100);//udp_match_en
	r8168_mac_ocp_write(tp, 0xCD00, 0xFFFF);
	r8168_mac_ocp_write(tp, 0xCD02, 0xFFFF);
	r8168_mac_ocp_write(tp, 0xCD04, 0xFFFF);
	r8168_mac_ocp_write(tp, 0xCD06, 0xFFFF);
	r8168_mac_ocp_write(tp, 0xC0B4, 0x0);//FTR_MCU
	r8168_mac_ocp_write(tp, 0xC0B4, r8168_mac_ocp_read(tp, 0xC0B4) | 0x21);//FTR_MCU enable	
	/*	(17) Set rxdv_gated_en = 0 (IO 0xF2 bit3=0) */
	RTL_W32(MISC, RTL_R32(MISC) & ~RXDV_GATED_EN);
	/*	(18) Turn on RCR (IO offset 0x44 set to 0x0e) */
	r8168_mac_ocp_write(tp, 0xC010, 0xE);//enable rx packet filetr
	r8168_mac_ocp_write(tp, 0xEA18, 0x0064);//Set 10M idle IFG		
}

void rtk8168_hw_mdns_standby_settings(struct rtl8168_private *tp)
{
	rtl8168_mac_hw_mdns_offloading_setting(tp);
}

void rtl8168_get_hw_mdns_wakeup_information(struct rtl8168_private *tp, unsigned char *buf, unsigned int buf_len)
{
	unsigned int packet_len = r8168_mac_ocp_read(tp, OFFLOAD_WAKE_UP_PACKET_LEN_ADDR);
	unsigned int wake_reason= r8168_mac_ocp_read(tp, OFFLOAD_WAKE_REASON_ADDR);
	unsigned int response_counter =	r8168_mac_ocp_read(tp, OFFLOAD_RESPONSE_COUNTER_ADDR);
	unsigned int miss_counter =	r8168_mac_ocp_read(tp, OFFLOAD_MISS_COUNTER_ADDR);
	unsigned int i = 0;

	buf[0] = 0;
	eth_err("packet_len = %u, wake_reason = %u, response_counter:%u, miss_counter:%u\n", packet_len, 
							wake_reason, response_counter, miss_counter);
	if((wake_reason != 1 && wake_reason != 2)
		|| packet_len < 60 || packet_len > 1536) {
		snprintf(buf + strlen(buf), buf_len - strlen(buf), "not wake by mdns offloading function\n");
		return ;
	} else {
		snprintf(buf + strlen(buf), buf_len - strlen(buf), "wake_reason:%u(1:passthrough wake, 2:forward_all), wake_packet_len:%d-\n", 
			wake_reason,packet_len);
	}
	//packet_len = packet_len - 4;//crc len
	packet_len = packet_len + (packet_len % 2);
	for(i = 0; i < packet_len; i = i + 2) {
		unsigned short val = r8168_mac_ocp_read(tp, OFFLOAD_WAKE_UP_PACKET_CONTENT_ADDR + i);
		snprintf(buf + strlen(buf), buf_len - strlen(buf),	"0x%02x, 0x%02x, ", val & 0xff, (val >> 8) & 0xFF);
	}
}


/**************************************Wake port releated******************************************************/
#define BIT_15	(1 << 15)
#define BIT_14	(1 << 14)
#define BIT_13	(1 << 13)
#define BIT_12	(1 << 12)
#define BIT_11	(1 << 11)
#define BIT_10	(1 << 10)
#define BIT_9	(1 << 9)
#define BIT_8	(1 << 8)
#define BIT_7	(1 << 7)
#define BIT_6	(1 << 6)
#define BIT_5	(1 << 5)
#define BIT_4	(1 << 4)
#define BIT_3	(1 << 3)
#define BIT_2	(1 << 2)
#define BIT_1	(1 << 1)
#define BIT_0	(1 << 0)

static unsigned short CRC16_CCITT_WIFI(unsigned char	data, unsigned short	CRC)
{
	unsigned char		shift_in, DataBit, CRC_BIT11, CRC_BIT4, CRC_BIT15;
	unsigned char		index;
	unsigned short	CRC_Result;
 
	for (index = 0; index < 8; index++)
	{
		CRC_BIT15 = ((CRC & BIT_15) ? 1 : 0);
		DataBit = (data & (BIT_0 << index) ? 1 : 0);
		shift_in = CRC_BIT15 ^ DataBit;
		//printf("CRC_BIT15=%d, DataBit=%d, shift_in=%d \n",CRC_BIT15,DataBit,shift_in);
 
		CRC_Result = CRC << 1;
		//set BIT0 
		//	printf("CRC =%x\n",CRC_Result);
		//CRC bit 0 =shift_in,
		if (shift_in == 0)
			CRC_Result &= (~BIT_0);
		else
			CRC_Result |= BIT_0;
		//printf("CRC =%x\n",CRC_Result);
 
		CRC_BIT11 = ((CRC & BIT_11) ? 1 : 0) ^ shift_in;
		if (CRC_BIT11 == 0)
			CRC_Result &= (~BIT_12);
		else
			CRC_Result |= BIT_12;
		//printf("bit12 CRC =%x\n",CRC_Result);
 
		CRC_BIT4 = ((CRC & BIT_4) ? 1 : 0) ^ shift_in;
		if (CRC_BIT4 == 0)
			CRC_Result &= (~BIT_5);
		else
			CRC_Result |= BIT_5;
		//printf("bit5 CRC =%x\n",CRC_Result);
 
		CRC = CRC_Result;
	}
 
	return CRC;
}

static unsigned short get_crc16(unsigned char *Pattern, unsigned short length)
{
	unsigned int i = 0;
	unsigned short crc_wifi = 0xffff;
	for (i = 0; i < length; i++)
	{
		crc_wifi = CRC16_CCITT_WIFI(Pattern[i], crc_wifi);
	}
	return crc_wifi;
}

static unsigned int do_test_bit(unsigned char *bits_array, unsigned int bit_index)
{
	unsigned int byte_ofst = bit_index / 8;
	unsigned int bit_ofst = bit_index % 8;
	return !!(bits_array[byte_ofst] & (1 << bit_ofst));
}

void rtk8168_get_pkt_crc16(unsigned char *pkt, unsigned int pkt_len, 
			unsigned char *mask_array,	unsigned short *crc_value)
{
	unsigned char buf[128];
	unsigned int buf_len = 0;
	unsigned int len = 0;
	unsigned int i = 0;
	
	len = (pkt_len > 128) ? 128 : pkt_len;
	memset(buf, 0, len);

	for (i = 0; i < len; i++) {
		if (do_test_bit(mask_array, i)) {
			buf[buf_len] = pkt[i];
			buf_len++;
		} 
	}
	*crc_value = get_crc16(buf, buf_len);
}
#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD

void rtk8168_set_wake_port_pattern(struct rtl8168_private *tp)
{
	int i;
	if(!wakeup_source_list.list_size || wakeup_source_list.list_size > MAX_PORT_WAKE_SOURCE_CNT)
		return;
	eth_info("set wake port pattern\n");
	
	for(i = 0; i < wakeup_source_list.list_size; i	= i + 4) {
		r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_MASK_SELECTION_REG + i / 2, 0x0000);
	}

	//set wol_mask
	for(i = 0; i < 64; i++)
	{
		int index = i / 2;
		int j;
		for(j = 0; j < wakeup_source_list.list_size; j = j + 4) {
			if((i % 2) == 0) {
				int n;
				int k;
				u16 tmp = 0;
				n = ((wakeup_source_list.list_size - j) >= 4) ? 4 : (wakeup_source_list.list_size - j);
				for(k = 0; k < n; k++) {
					tmp = tmp << 4;
					tmp = tmp | (wakeup_source_list.list[j + n - k - 1].wol_mask[index] & 0xF);
				}
				r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_MASK_REG + i * 16 + j / 2, tmp);
			} else {
				int n;
				int k;
				u16 tmp = 0;
				n = ((wakeup_source_list.list_size - j) >= 4) ? 4 : (wakeup_source_list.list_size - j);
				for(k = 0; k < n; k++) {
					tmp = tmp << 4;
					tmp = tmp | ((wakeup_source_list.list[j + n - k - 1].wol_mask[index] >> 4) & 0xF);
				}
				r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_MASK_REG + i * 16 + j / 2, tmp);
			}
		}
	}

	//set wol crc
	for(i=0;i<wakeup_source_list.list_size;i++)
	{
		if(i < 8)
			r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_CRC1_REG + i * 2, wakeup_source_list.list[i].crc_value);
		else
			r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_CRC2_REG + (i -8) * 2, wakeup_source_list.list[i].crc_value);
	}
}
#else
void rtk8168_set_wake_port_pattern(struct rtl8168_private *tp)
{
	unsigned int i = 0;
	unsigned int j = 0; 

	if(!wakeup_source_list.list_size || wakeup_source_list.list_size > MAX_PORT_WAKE_SOURCE_CNT)
		return;
#if 0
	eth_info("set wake port pattern\n");
	for(i=0;i<16;i++) {
		temp = ((u32)wol_mask[2][i]<<16) | ((u32)wol_mask[1][i]<<8) | (u32)wol_mask[0][i];
		rtl_eri_write(tp, i*8, ERIAR_MASK_1111, temp, ERIAR_EXGMAC);
	}
	// set mask
	rtl_eri_write(tp, 0x80, ERIAR_MASK_1111, ((u32)wol_pattern[1] << 16) | (u32)wol_pattern[0], ERIAR_EXGMAC);
	rtl_eri_write(tp, 0x84, ERIAR_MASK_0011, (u32)wol_pattern[2], ERIAR_EXGMAC);
#else
	eth_info("set wake port pattern\n");
	for(i=0;i<16;i++) {
		for(j = 0; j < wakeup_source_list.list_size; j = j + 4) {
			u32 eri_mask = 0;
			u32 val = 0;
			if(j + 4 <= wakeup_source_list.list_size) {
				eri_mask = ERIAR_MASK_1111;
				val = ((u32)wakeup_source_list.list[j].wol_mask[i]<<0) | ((u32)wakeup_source_list.list[j + 1].wol_mask[i]<<8) 
							| ((u32)wakeup_source_list.list[j + 2].wol_mask[i]<<16) |((u32)wakeup_source_list.list[j + 3].wol_mask[i]<<24) ;
			} else if(j + 3 <= wakeup_source_list.list_size) {
				eri_mask = ERIAR_MASK_0111;
				val = ((u32)wakeup_source_list.list[j].wol_mask[i]<<0) | ((u32)wakeup_source_list.list[j + 1].wol_mask[i]<<8) 
							| ((u32)wakeup_source_list.list[j + 2].wol_mask[i]<<16);
			} else if(j + 2 <= wakeup_source_list.list_size) {
				eri_mask = ERIAR_MASK_0011;
				val = ((u32)wakeup_source_list.list[j].wol_mask[i]<<0) | ((u32)wakeup_source_list.list[j + 1].wol_mask[i]<<8);
			} else {
				eri_mask = ERIAR_MASK_0001;
				val = ((u32)wakeup_source_list.list[j].wol_mask[i]<<0);
			}
			rtl_eri_write(tp,	(i * 8) + (j / 4) * 4, eri_mask, val, ERIAR_EXGMAC);
		}
	}
	

	for(i = 0; i < wakeup_source_list.list_size; i = i + 2) {
		if(i + 2 <= wakeup_source_list.list_size) {
			rtl_eri_write(tp, 0x80 + i * 2, ERIAR_MASK_1111, ((u32)wakeup_source_list.list[i + 1].crc_value << 16) 
						| ((u32)wakeup_source_list.list[i].crc_value), ERIAR_EXGMAC);
		} else {
			rtl_eri_write(tp, 0x80 + i * 2, ERIAR_MASK_0011, (u32)wakeup_source_list.list[i].crc_value, ERIAR_EXGMAC);
		}
	}
#endif
}
#endif

