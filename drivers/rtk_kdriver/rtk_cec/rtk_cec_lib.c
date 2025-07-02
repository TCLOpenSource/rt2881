#include <linux/slab.h>
#include <linux/string.h>
#include <rtk_kdriver/rtk_cec_lib.h>
#include "rtk_cec_msg.h"
#include "rtk_cec_defines.h"

// extern cec_device_st cec_device_list[];

int rtk_cec_lib_get_device_num(void)
{
	int num = rtk_cec_get_device_num();
	return num;
}
EXPORT_SYMBOL(rtk_cec_lib_get_device_num);

void rtk_cec_lib_get_device_list(CEC_DEVICE_INFO device_list[])
{
	int num = rtk_cec_get_device_num();
	int index = 0;
	int addr_index = 0;
	int cec_num_index = 0;
	cec_device_st all_cec_device_list[CEC_DEVICE_TOTAL_NUM];

	if(num <= 0)
		return ;

	rtk_cec_get_device(all_cec_device_list);

 	for(index = 0; index < CEC_DEVICE_TOTAL_NUM; index++)
 	{
		if(all_cec_device_list[index].connect_st == 1)
 		{
 			device_list[cec_num_index].logic_addr = all_cec_device_list[index].logic_addr;
 			
 			memset(device_list[cec_num_index].osd_name, '\0', sizeof(device_list[cec_num_index].osd_name));
 			strncpy(device_list[cec_num_index].osd_name, all_cec_device_list[index].osd_name, CEC_OSD_NAME_MAX_LENGTH);
 			device_list[cec_num_index].osd_name[CEC_OSD_NAME_MAX_LENGTH-1] = '\0';
 			
 			memset(device_list[cec_num_index].logic_addr_name, '\0', sizeof(device_list[cec_num_index].logic_addr_name));
 			strncpy(device_list[cec_num_index].logic_addr_name, all_cec_device_list[index].logic_addr_name, CEC_OSD_NAME_MAX_LENGTH);
 			device_list[cec_num_index].logic_addr_name[CEC_OSD_NAME_MAX_LENGTH-1] = '\0';
 			
			for(addr_index = 0 ; addr_index < 4; addr_index++)
			{
				device_list[cec_num_index].physical_addr[addr_index] = all_cec_device_list[index].physical_addr[addr_index];
			}
 			cec_num_index++;
 		}
 	}
}
EXPORT_SYMBOL(rtk_cec_lib_get_device_list);

void rtk_cec_lib_remove_device(unsigned char port)
{
	rtk_cec_remove_device(port);
}
EXPORT_SYMBOL(rtk_cec_lib_remove_device);

