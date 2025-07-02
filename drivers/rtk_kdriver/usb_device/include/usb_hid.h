#ifndef ___USB_HID_H
#define ___USB_HID_H

#define USB_DT_REPORT_DESCRIPTOR_TYPE 0x22

#define HID_EP_ADDRESS 0x84
#define HID_EP_NUMBER 9

void rtk_dwc3_get_hid_report_descriptor(void) __banked;
void rtk_hid_set_report_callback(void) __banked;
void rtk_hid_control_interface(void) __banked;
void gen_descriptor_table_HID(void) __banked;

#endif /* __USB_HID_H */
