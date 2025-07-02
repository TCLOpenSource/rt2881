/*
 *
 * Copyright (C) 2021, Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 */

/********************** IOCTL  ************************/
#define RTK_CIUSB_IOC_STATUS            0x40046301
#define RTK_CIUSB_IOC_INFO              0x40046302
#define RTK_CIUSB_IOC_RESET             0x40046303
#define RTK_CIUSB_IOC_SET_PARAM         0x40046304
#define RTK_CIUSB_IOC_RESET_STATISTIC   0x40046305

#define FRAGMENT_HEADER_MAGIC_SIZE      (1234*1024)

#define RTK_CIUSB_STS_CNNECTED     (0x0001 << 0)
#define RTK_CIUSB_STS_AFTER_STR    (0x0001 << 1)
#define RTK_CIUSB_STS_ENABLE_RW    (0x0001 << 2)

#define CIUSB_INTF_CMD_IDX          0
#define CIUSB_INTF_MEDIA_IDX        1
#define CIUSB_INTF_COMM_IDX         2

#define CIUSB_NAME                  "ciusb"
#define CIUSB_DEVICE_COMMAND_NAME   "ciusb0"
#define CIUSB_DEVICE_MEDIA_NAME     "ciusb1"
#define CIUSB_DEVICE_COMM_NAME      "ciusb2"

typedef enum {
    CIUSB_INTF_NONE     = 0,
    CIUSB_INTF_CMD      = (1<<CIUSB_INTF_CMD_IDX),
    CIUSB_INTF_MEDIA    = (1<<CIUSB_INTF_MEDIA_IDX),
    CIUSB_INTF_COMM     = (1<<CIUSB_INTF_COMM_IDX),
    CIUSB_INTF_NUM      = 3,
} CIUSB_INTF_TYPE;

typedef struct {
    uint32_t vendor_id;
    uint32_t product_id;
    uint32_t serial_num;
    uint32_t packet_size_r;
    uint32_t packet_size_w;
    uint64_t time_read_wait;
    uint64_t time_write_wait;
} CIUSB_IOC_INFO;

typedef struct {
    uint32_t read_timeout;
    uint32_t write_timeout;
    uint32_t return_FH;
}CIUSB_IOC_PARAM;

