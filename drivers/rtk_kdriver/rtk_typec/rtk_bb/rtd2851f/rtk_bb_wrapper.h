#ifndef __RTK_BB_WRAPPER_H__
#define __RTK_BB_WRAPPER_H__
#define MAX_USB_BB_DEVICE_NUM 1
#define BB_WRITE_SRAM_US 4
#define BB_DELAY(x) 

void UsbBillboard_CRT_ON_OFF(struct USB_BB_PRIVATE_DATA *private_data, bool on);
bool is_UsbBillboard_CRT_ON(struct USB_BB_PRIVATE_DATA *private_data);
void UsbBillboard_init_extra_settings(struct USB_BB_PRIVATE_DATA *private_data);
bool rtk_bb_is_typec_attached(struct USB_BB_PRIVATE_DATA *private_data);
bool rtk_bb_is_failed_in_typec_altmode(struct USB_BB_PRIVATE_DATA *private_data);
#endif