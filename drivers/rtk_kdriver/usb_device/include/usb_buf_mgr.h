#ifndef __USB_BUF_MGR__
#define __USB_BUF_MGR__

enum usb_buf_type {
	USB_SRAM_BUF,
	UAC_SRAM_BUF,
	UAC_DDR_BUF,
	UAC_AVSYNC_DDR_BUF,
	UVC_SRAM_BUF,
	USB_BUF_TYPE_NUM,
};

#define USB_UVC_SRAM_BUFFER_SIZE       ((UINT32)56304)
#define USB_UAC_SRAM_BUFFER_SIZE       ((UINT32)17392)
#define USB_UAC_DDR_BUFFER_SIZE        ((UINT32)41648)
#define USB_UAC_AVSYNC_DDR_BUFFER_SIZE ((UINT32)1024*1024)

UINT8 usb_request_usbbuf(enum usb_buf_type buf_type, UINT32 *phyaddr, UINT8 **viraddr, UINT32 *size);
UINT8 usbbuf_init(void);

#endif
