#ifndef __RTK_USB_HUB_INIT__
#define __RTK_USB_HUB_INIT__
void rtk_usb_hub_usp_enable(struct device *dev, bool enable);
void rtk_usb_hub_dsp_enable(struct device *dev, bool enable);
void rtk_usb_hub_specail_dsp_enable(struct device *dev, unsigned char port, bool enable);
#endif	/* __RTK_USB_HUB_INIT__ */
