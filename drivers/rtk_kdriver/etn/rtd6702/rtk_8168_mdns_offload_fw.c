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

void rtl8168_mac_hw_mdns_offloading_setting(struct rtl8168_private *tp)
{	
}

unsigned int rtl8168_get_hw_mdns_offload_passthrough_list_content_addr(struct rtl8168_private *tp)
{
	return OFFLOAD_PASSTHROUGH_LIST_CONTENT_ADDR;
}