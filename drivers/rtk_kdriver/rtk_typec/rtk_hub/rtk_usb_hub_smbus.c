#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <rtk_kdriver/io.h>
#include <linux/platform_device.h>
#include <rtk_kdriver/io.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/usb2_hub_reg.h>
#include "rtk_usb_hub_defines.h"
#include "rtk_usb_hub_smbus.h"
#include "rtk_usb_hub_debug.h"
extern bool usb_hub_is_in_suspend(void);
/*
* For Block Read:
* Set block_read =1. Set Slave_addr ,  cmdcode.
* Set en =1. Check en =0 and trans_err =0.
* Read Read_data_*.
* 
* For Block Write:
* Set block_write =1. Set Slave_addr ,  cmdcode,  wr_data_length, byte_cnt, Hub_addr,
* Set en =1. Check en =0 and trans_err =0.
* 
* For receive byte (non block read):
* Set non_block_read =1. Set Slave_addr  ,
* Set en =1. Check en =0 and trans_err =0.
* Read read_data0
*/

static bool rtk_smbus_transfer_type(struct device *dev, unsigned int type)
{
    struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    
    switch(type) {
    case RTK_SMBUS_BLOCK_WRITE_TYPE:
		rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 26, 24, 0x4); 
		break;
    case RTK_SMBUS_BLOCK_READ_TYPE:
		rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 26, 24, 0x2); 
		break;
    case RTK_SMBUS_NON_BLOCK_READ_TYPE:
		rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 26, 24, 0x1); 
		break;
    default:
		break;
    }
    return true;
}

static bool rtk_smbus_block_write(struct device *dev, unsigned char address_H, unsigned char address_L)
{
    unsigned int count = 0;
    struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 23, 8, (address_H << 8) | address_L);    // Hub_addr; write data offset bit[24:8]
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 6, 0, 0x6a);              // SALVE_addr; write data offset bit[24:8]
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 31, 24, 1);                              // Set byte_cnt to 1 (Hub return data number); write data offset bit[31:25]
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 23, 16, RTK_SMBUS_READ);                 // CMD code; write data offset bit[7:0]
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 7, 0, 0x3);                              // Wr_data_length; write data offset bit[15:8]

    rtk_smbus_transfer_type(dev, RTK_SMBUS_BLOCK_WRITE_TYPE);
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x1);

    while((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_start_mask) != 0) { // Start transfer done
        if(count ++ > 500 || usb_hub_is_in_suspend()) {
		rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x0); 
		return false;
        }
        msleep(1);
    }

   if((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_tran_err_mask) != 0) { // ACK or not
            RTK_USB_HUB_ERR("[USB Hub] SMBUS NACK\n");
            return false;
   }
   return true;
}

static bool rtk_smbus_receive_byte(struct device *dev, unsigned char *pByte)
{
    struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    unsigned int count = 0;
    if(!pByte)
         return false;
    rtk_smbus_transfer_type(dev, RTK_SMBUS_NON_BLOCK_READ_TYPE);
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x1);
    while((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_start_mask) != 0) { // Start transfer done
        if(count ++ > 500 || usb_hub_is_in_suspend()) {
		rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x0); 
		return false;
	}
	msleep(1);
    }
   if((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_tran_err_mask) != 0) { // ACK or not
            RTK_USB_HUB_ERR("[USB Hub] SMBUS NACK\n");
            return false;
   }
   *pByte = rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_DATA_1_reg) & 0xFF;
   return true;
}

static bool rtk_smbus_block_read(struct device *dev, unsigned int *pval)
{
    struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    unsigned int count = 0;
    int read_hub_return_number = 0;
    rtk_smbus_transfer_type(dev, RTK_SMBUS_BLOCK_READ_TYPE);
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 31, 24, 0x0); 
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 23, 16, RTK_SMBUS_BLOCK_READ);              // CMD code; write data offset bit[7:0]
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 7, 0, 0); 
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x1);

    while((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_start_mask) != 0) { // Start transfer done
        if(count ++ > 500 || usb_hub_is_in_suspend()) {
		rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x0); 
		return false;
	}
	msleep(1);
    }

   if((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_tran_err_mask) != 0) { // ACK or not
            RTK_USB_HUB_ERR("[USB Hub] SMBUS NACK\n");
            return false;
   }

    read_hub_return_number = rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_DATA_1_reg) & 0xFF;

    if(read_hub_return_number != 0x1) {
	 *pval = 0;
	 return false;
    } else {
    	 *pval = (rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_DATA_1_reg) >> 8) & 0xFF;
    	 return true;
    }
}

bool rtk_smbus_read_data_transfer(struct device *dev, unsigned char address_H, 
				unsigned char address_L, unsigned int *pVal)
{
    struct platform_device *pdev = container_of(dev, struct platform_device, dev);
    struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
    bool ret = false;
    unsigned char ping_status  = 0;
    unsigned int count = 0;
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 28, 28, 0x1);// Enable SMBUS
    // Block Write

    if(rtk_smbus_block_write(dev, address_H, address_L) != true) 
	goto EXIT;

    while((count++) < 0x10) {
	    // Receive Byte
	    if(rtk_smbus_receive_byte(dev, &ping_status) != true)
		goto EXIT;
	    if((ping_status & 0x3) == 0x1)
		break;
	    else if((ping_status & 0x3) == 0x3)
		goto EXIT;	
	    else if(usb_hub_is_in_suspend())
		goto EXIT;
    }
    if(count >= 0x10)
		goto EXIT;
    
    // Block Read
    if(rtk_smbus_block_read(dev, pVal) != true)
	goto EXIT;
    ret = true;
EXIT:	
    rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 28, 28, 0x0);// disable SMBUS
    return ret;
}

bool rtk_smbus_force_dsp_port_disconnect(struct device *dev, unsigned int port_index, bool disconnect)
{
	struct platform_device *pdev = container_of(dev, struct platform_device, dev);
	struct RTK_USB_HUB_PRIVATE *private_data = platform_get_drvdata(pdev);
	bool ret = false;
	unsigned int count = 0;
	RTK_USB_HUB_INFO("HUB[%u] port[%u] %s\n", private_data->id, port_index, disconnect ? "disconnect" : "connect");
	if(port_index > 3)
		return ret;

	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 28, 28, 0x1);// Enable SMBUS
	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 23, 8, (1 << port_index) | (disconnect ? 0x10 : 0x00));    // Hub_addr; write data offset bit[24:8]
	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 6, 0, 0x6a);              // SALVE_addr; write data offset bit[24:8]
	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 31, 24, 1);                              // Set byte_cnt to 1 (Hub return data number); write data offset bit[31:25]
	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 23, 16, RTK_SMBUS_FORCE_DSP_PORT_DISCONNECT);                 // CMD code; write data offset bit[7:0]
	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_1_reg, 7, 0, 0x1);                              // Wr_data_length; write data offset bit[15:8]

	rtk_smbus_transfer_type(dev, RTK_SMBUS_BLOCK_WRITE_TYPE);
	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x1);

	while((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_start_mask) != 0) { // Start transfer done
		if(count ++ > 500 || usb_hub_is_in_suspend()) {
			rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 27, 27, 0x0); 
			goto EXIT;
		}
		msleep(1);
	}

	if((rtd_inl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg) & USB2_HUB_SMBUS_CTRL_0_tran_err_mask) != 0) { // ACK or not
		goto EXIT;
	}
	ret = true;
EXIT:
	rtd_part_outl(private_data->wrapper_regs.USB_HUB_SMBUS_CTRL_0_reg, 28, 28, 0x0);// disable SMBUS
	return ret;
}