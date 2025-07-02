#ifndef __USB_UTILITY_H__
#define __USB_UTILITY_H__

//****************************************************************************
// USB INTERFACE
//****************************************************************************
// g_recv_buf[0]     : id number
// g_recv_buf[1]     : id number
// g_recv_buf[2]     : table_offset
// g_recv_buf[3]     : table_offset
// g_recv_buf[4]     : table_size
// g_recv_buf[5]     : table_size
// g_recv_buf[6]     : VS DT offset
// g_recv_buf[7]     : VS DT offset
// g_recv_buf[10-30] : file name
// g_recv_buf[31]    : use flash or not
// g_recv_buf[64]    : uvc commit
// g_recv_buf[74]    : uvc probe

#define BUFFER_INDEX_ID_NUMBER_LOW               0
#define BUFFER_INDEX_ID_NUMBER_HI                1
#define BUFFER_INDEX_ID_TABLE_OFFSET_LOW         2
#define BUFFER_INDEX_ID_TABLE_OFFSET_HI          3
#define BUFFER_INDEX_ID_TABLE_SIZE_LOW           4
#define BUFFER_INDEX_ID_TABLE_SIZE_HI            5
#define BUFFER_INDEX_ID_VS_DT_OFFSET_LOW         6
#define BUFFER_INDEX_ID_VS_DT_OFFSET_HI          7
#define BUFFER_INDEX_ID_TABLE_VERION_SUB         8
#define BUFFER_INDEX_ID_TABLE_VERION_MAIN        9
#define BUFFER_INDEX_ID_FILENAME                 10

#define BUFFER_INDEX_ID_UVC_PROBE                 64
#define BUFFER_INDEX_ID_UVC_COMMIT                74

#define BUFFER_INDEX_ID_VS_CDC_OFFSET_LOW        0
#define BUFFER_INDEX_ID_VS_CDC_OFFSET_HI         1
#define BUFFER_INDEX_ID_VS_UAC_OFFSET_LOW        2
#define BUFFER_INDEX_ID_VS_UAC_OFFSET_HI         3
#define BUFFER_INDEX_ID_VS_UVC_OFFSET_LOW        4
#define BUFFER_INDEX_ID_VS_UVC_OFFSET_HI         5
#define BUFFER_INDEX_ID_VS_UVC_MJ_COLOR_MATCHING_OFFSET_LOW 6
#define BUFFER_INDEX_ID_VS_UVC_MJ_COLOR_MATCHING_OFFSET_HI  7

#define DESCRIPTOR_TABLE_ID_INDEX_SN                           0
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_HOST_TO_DEVICE_0_DT_96   1
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_0_DT_96   2
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_1_DT_96   3
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_only_DT   4
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_HOST_TO_DEVICE_only_DT   5
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_ALL_DT    6
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_D2H_H2D_only_DT          7
/*
#define DESCRIPTOR_TABLE_ID_INDEX_VENDOR_ID                    1
#define DESCRIPTOR_TABLE_ID_INDEX_PRODUCT_ID                   2
#define DESCRIPTOR_TABLE_ID_INDEX_MANUFACTURER_NAME            3
#define DESCRIPTOR_TABLE_ID_INDEX_PRODUCT_NAME                 4
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_NAME                     5
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_HOST_TO_DEVICE_NAME_0    6
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_NAME_0    7
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_NAME_1    8
*/
#define DESCRIPTOR_TABLE_ID_INDEX_DEVICE_DT                    9
#define DESCRIPTOR_TABLE_ID_INDEX_CONFIG_DT                    10
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_COMMON_CONTROL_DT        11
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_STREAM_DT                12
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_NV12_DT                  13
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_M420_DT                  14
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_I420_DT                  15
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_YUY2_DT                  16
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_RGB24_DT                 17
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_RGB32_DT                 18
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_P010_DT                  19
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_MJPEG_DT                 20
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_COLOR_MATCH_DT           21
#define DESCRIPTOR_TABLE_ID_INDEX_UVC_END_DT                   22
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_HOST_TO_DEVICE_0_DT      23
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_0_DT      24
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_1_DT      25
#define DESCRIPTOR_TABLE_ID_INDEX_CDC_DT                       26
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_HOST_TO_DEVICE_0_DT_48   27
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_0_DT_48   28
#define DESCRIPTOR_TABLE_ID_INDEX_UAC_DEVICE_TO_HOST_1_DT_48   29

#define USB_AT_CMD_DATA_MAX_SIZE 128

//****************************************************************************
// UAC INTERFACE
//****************************************************************************
#define UAC_IAD_STRING_DESCRIPTOR       0x8
#define UAC_OUT1_STRING_DESCRIPTOR      0x9
#define UAC_OUT2_STRING_DESCRIPTOR      0xA

#define UAC_OUT1_INTERFACE_STRING_DESCRIPTOR 0xc
#define UAC_OUT2_INTERFACE_STRING_DESCRIPTOR 0xd

#define UAC_TYPE_MICROPHONE             0x0201
#define UAC_TYPE_DIGITAL_AUDIO          0x0602

void rtk_dwc3_check_DT(void);
void dwc3_copy_to_memory2_utility(void);
void dwc3_copy_from_memory2_utility(void);
void rtk_dwc3_get_config_file_name_by_speed(void);
UINT8 get_id_data_to_sram(void);

typedef struct {
    UINT32     length;     // length
    UINT8     data[128];   // data
} AT_USB_USER_DATA_T;


#endif // __USB_UTILITY_H__
