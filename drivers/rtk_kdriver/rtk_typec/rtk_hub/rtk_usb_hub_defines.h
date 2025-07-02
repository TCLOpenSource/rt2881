#ifndef __RTK_USB_HUB_DEFINES_H__
#define __RTK_USB_HUB_DEFINES_H__

#define MAX_HUB_TYPEA_PORT_NUM     4

struct USB_HUB_WRAPPER_REGS
{
	unsigned int USB_HUB_HUB_RAM_I_WR_ADDR_reg;
	unsigned int USB_HUB_HUB_RAM_I_WR_DATA_reg;
	unsigned int USB_HUB_HUB_RAM_I_RD_ADDR_reg;
	unsigned int USB_HUB_HUB_RAM_I_RD_DATA_reg;
	unsigned int USB_HUB_HUB_RAM_I_DATA_SEL_reg;
	unsigned int USB_HUB_HUB_CTRL0_reg;
	unsigned int USB_HUB_SMBUS_CTRL_0_reg;
	unsigned int USB_HUB_SMBUS_CTRL_1_reg;
	unsigned int USB_HUB_SMBUS_DATA_1_reg;
};

#define USB_HUB_MODE_CHECK_TIMEOUT  msecs_to_jiffies(10)

struct RTK_USB_HUB_PRIVATE
{
	struct kobject kobj;
	struct platform_device *pdev;
	struct task_struct *usb_hub_task;
	unsigned int io_addr;
	unsigned int id;
	bool usp_suspend_state;
	struct USB_HUB_WRAPPER_REGS wrapper_regs;
	unsigned char typea_port_switch_mode[MAX_HUB_TYPEA_PORT_NUM];
	struct timer_list mode_check_timer;
	unsigned char cur_mode; //0:host_mode, 1:device_mode
	unsigned char is_need_pop_bb;
	/*0:disable, 1:enable*/
	spinlock_t downstreamport_switch_lock;
	atomic_t dwonstreamport_should_state[MAX_HUB_TYPEA_PORT_NUM];
	atomic_t dwonstreamport_current_state[MAX_HUB_TYPEA_PORT_NUM];	

};

#endif
