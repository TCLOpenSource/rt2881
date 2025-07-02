#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/kfifo.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <linux/bitops.h>
#include <linux/ioctl.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <rbus/iso_misc_reg.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/pcbMgr.h>
#include <rtk_kdriver/i2c-rtk-api.h>
#include <rtk_kdriver/rtd_log/rtd_module_log.h>
int rtk_pwr_monitor_getValue_VI(unsigned char *pwr_value)
{
	unsigned long long pmic_power_info = 0;
	//unsigned int total_power = 0;
	//unsigned int total_valtage = 0;
	//unsigned int total_current = 0;
	unsigned char pWritingBytes[3] = {0x05, 0x20, 0x00};
	unsigned char data[2] = {0};
	unsigned char wdata[1] = {0x3};
	unsigned char slaveaddr[3] = {0};
	unsigned char busid = 0;
	unsigned char pwr_offset = 0;
	int i;
	if(pwr_value == NULL) {
		rtd_pr_pwr_monitor_err("Get empty pointer to pass power info\n");
		return -1;
	}
	//  It needs PMIC_POWER_INFO in bootcode
	//  [0:7] bus id
	//  [15:8] Core addr
	//  [23:16] SCPU addr
	//  [31:24] ddr ADDR；
	//
	if (pcb_mgr_get_enum_info_byname("PMIC_POWER_INFO", &pmic_power_info) == 0){
		rtd_pr_pwr_monitor_info("Get pcb = %x\n", pmic_power_info);

		busid = pmic_power_info & 0xff;
		slaveaddr[0] = (pmic_power_info >> 8) & 0xff;
		slaveaddr[1] = (pmic_power_info >> 16) & 0xff;
		slaveaddr[2] = (pmic_power_info >> 24) & 0xff;
		//slaveaddr[2] = slaveaddr[1] = slaveaddr[0];
		//rtd_pr_pwr_monitor_err("[PM_MON] get power info %lx\n", pinc_power_info);	
		// for test
		//for(i = 0; i < 12; i++) 
		//	pm_value[i] = i;
		for(i = 0; i < 3; i++) {
			//rtd_pr_pwr_monitor_err("[PM_MON] I2C slave %x\n", slaveaddr[i]);
			// IC calibration
			if(i2c_master_send_ex(busid, slaveaddr[i], &pWritingBytes[0], 3) < 0){
				rtd_pr_pwr_monitor_err("I2C calibration fail\n");
				return -1;
			}			
			// Voltage
			wdata[0] = 0x2;
			if(i2c_master_recv_ex(busid, slaveaddr[i], wdata, 1, data, 2) < 0) {
				rtd_pr_pwr_monitor_err("I2C read voltage fail\n");
				return -1;
			}
			pwr_value[pwr_offset++] = data[0];
			pwr_value[pwr_offset++] = data[1];
			//total_valtage = (data[0] << 8);
			//total_valtage |= data[1];
			//RTK_USB_HUB_WARNING("Typec total voltage = %d mV\n", (total_valtage >> 1));
			// Current
			wdata[0] = 0x4;
			if(i2c_master_recv_ex(busid, slaveaddr[i], wdata, 1, data, 2) < 0){
				rtd_pr_pwr_monitor_err("I2C read current fail\n");
				return -1;		
			}
			//total_current = (data[0] << 8);
			//total_current |= data[1];
			pwr_value[pwr_offset++] = data[0];
			pwr_value[pwr_offset++] = data[1];
			//RTK_USB_HUB_WARNING("Typec total current = %d mA\n", total_current);
			// Power Watt
			//wdata[0] = 0x3;
			//ret = i2c_master_recv_ex(0, 0x40, wdata, 1, data, 2);
			//total_power = (data[0] << 8);
			//total_power |= data[1];
			//total_power = (unsigned int)(total_power * 20);
			//RTK_USB_HUB_WARNING("Typec total power = %d mW\n", total_power);
		}
		return 0;
	}
	else {
		rtd_pr_pwr_monitor_err("Not get PMIC_POWER_INFO in pcb info\n");
	}
	return -1;
}

int rtk_pwr_monitor_getValue_All(unsigned char *pwr_value)
{
	unsigned long long pmic_power_info = 0;
	//unsigned int total_power = 0;
	//unsigned int total_valtage = 0;
	//unsigned int total_current = 0;
	unsigned char pWritingBytes[3] = {0x05, 0x20, 0x00};
	unsigned char data[2] = {0};
	unsigned char wdata[1] = {0x3};
	unsigned char slaveaddr[3] = {0};
	unsigned char busid = 0;
	unsigned char pwr_offset = 0;
	int i;
	if(pwr_value == NULL) {
		rtd_pr_pwr_monitor_err("Get empty pointer to pass power info\n");
		return -1;
	}
	//  It needs PMIC_POWER_INFO in bootcode
	//  [0:7] bus id
	//  [15:8] Core addr
	//  [23:16] SCPU addr
	//  [31:24] ddr ADDR；
	//
	if (pcb_mgr_get_enum_info_byname("PMIC_POWER_INFO", &pmic_power_info) == 0){
		rtd_pr_pwr_monitor_info("Get pcb = %x\n", pmic_power_info);

		busid = pmic_power_info & 0xff;
		slaveaddr[0] = (pmic_power_info >> 8) & 0xff;
		slaveaddr[1] = (pmic_power_info >> 16) & 0xff;
		slaveaddr[2] = (pmic_power_info >> 24) & 0xff;
		//slaveaddr[2] = slaveaddr[1] = slaveaddr[0];
		//rtd_pr_pwr_monitor_err("[PM_MON] get power info %lx\n", pinc_power_info);	
		// for test
		//for(i = 0; i < 12; i++) 
		//	pm_value[i] = i;
		for(i = 0; i < 3; i++) {
			//rtd_pr_pwr_monitor_err("[PM_MON] I2C slave %x\n", slaveaddr[i]);
			// IC calibration
			if(i2c_master_send_ex(busid, slaveaddr[i], &pWritingBytes[0], 3) < 0){
				rtd_pr_pwr_monitor_err("I2C calibration fail\n");
				return -1;
			}			
			// Voltage
			wdata[0] = 0x2;
			if(i2c_master_recv_ex(busid, slaveaddr[i], wdata, 1, data, 2) < 0) {
				rtd_pr_pwr_monitor_err("I2C read voltage fail\n");
				return -1;
			}
			pwr_value[pwr_offset++] = data[0];
			pwr_value[pwr_offset++] = data[1];
			//total_valtage = (data[0] << 8);
			//total_valtage |= data[1];
			//RTK_USB_HUB_WARNING("Typec total voltage = %d mV\n", (total_valtage >> 1));
			// Current
			wdata[0] = 0x4;
			if(i2c_master_recv_ex(busid, slaveaddr[i], wdata, 1, data, 2) < 0){
				rtd_pr_pwr_monitor_err("I2C read current fail\n");
				return -1;		
			}
			//total_current = (data[0] << 8);
			//total_current |= data[1];
			pwr_value[pwr_offset++] = data[0];
			pwr_value[pwr_offset++] = data[1];
			//RTK_USB_HUB_WARNING("Typec total current = %d mA\n", total_current);
			// Power Watt
			
			wdata[0] = 0x3;
				if(i2c_master_recv_ex(busid, slaveaddr[i], wdata, 1, data, 2) < 0){
				rtd_pr_pwr_monitor_err("I2C read power fail\n");
				return -1;		
			}
			pwr_value[pwr_offset++] = data[0];
			pwr_value[pwr_offset++] = data[1];

			//total_power = (data[0] << 8);
			//total_power |= data[1];
			//total_power = (unsigned int)(total_power * 20);
			//RTK_USB_HUB_WARNING("Typec total power = %d mW\n", total_power);
		}
		return 0;
	}
	else {
		rtd_pr_pwr_monitor_err("Not get PMIC_POWER_INFO in pcb info\n");
	}
	return -1;
}

EXPORT_SYMBOL(rtk_pwr_monitor_getValue_VI);
EXPORT_SYMBOL(rtk_pwr_monitor_getValue_All);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Weihao Lo <weihao@realtek.com>");
