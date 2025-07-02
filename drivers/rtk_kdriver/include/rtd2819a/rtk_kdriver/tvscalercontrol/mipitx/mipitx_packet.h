#ifndef __MIPITX_PACKET_H__
#define __MIPITX_PACKET_H__
#include <tvscalercontrol/mipitx/mipitx.h>


/* MIPI DSI Processor-to-Peripheral transaction types */
enum {
    MIPITX_DSI_V_SYNC_START = 0x01,
    MIPITX_DSI_V_SYNC_END = 0x11,
    MIPITX_DSI_H_SYNC_START = 0x21,
    MIPITX_DSI_H_SYNC_END = 0x31,

    MIPITX_DSI_COLOR_MODE_OFF = 0x02,
    MIPITX_DSI_COLOR_MODE_ON = 0x12,
    MIPITX_DSI_SHUTDOWN_PERIPHERAL = 0x22,
    MIPITX_DSI_TURN_ON_PERIPHERAL = 0x32,

    MIPITX_DSI_GENERIC_SHORT_WRITE_0_PARAM = 0x03,
    MIPITX_DSI_GENERIC_SHORT_WRITE_1_PARAM = 0x13,
    MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM = 0x23,

    MIPITX_DSI_GENERIC_READ_REQUEST_0_PARAM = 0x04,
    MIPITX_DSI_GENERIC_READ_REQUEST_1_PARAM = 0x14,
    MIPITX_DSI_GENERIC_READ_REQUEST_2_PARAM = 0x24,

    MIPITX_DSI_DCS_SHORT_WRITE = 0x05,
    MIPITX_DSI_DCS_SHORT_WRITE_PARAM = 0x15,

    MIPITX_DSI_DCS_READ = 0x06,

    MIPITX_DSI_DCS_COMPRESSION_MODE  = 0x07,
    MIPITX_DSI_PPS_LONG_WRITE = 0x0A,

    MIPITX_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE = 0x37,

    MIPITX_DSI_END_OF_TRANSMISSION = 0x08,

    MIPITX_DSI_NULL_PACKET = 0x09,
    MIPITX_DSI_BLANKING_PACKET = 0x19,
    MIPITX_DSI_GENERIC_LONG_WRITE = 0x29,
    MIPITX_DSI_DCS_LONG_WRITE = 0x39,

    MIPITX_DSI_LOOSELY_PACKED_PIXEL_STREAM_YCBCR20 = 0x0c,
    MIPITX_DSI_PACKED_PIXEL_STREAM_YCBCR24 = 0x1c,
    MIPITX_DSI_PACKED_PIXEL_STREAM_YCBCR16 = 0x2c,

    MIPITX_DSI_PACKED_PIXEL_STREAM_30 = 0x0d,
    MIPITX_DSI_PACKED_PIXEL_STREAM_36 = 0x1d,
    MIPITX_DSI_PACKED_PIXEL_STREAM_YCBCR12 = 0x3d,

    MIPITX_DSI_PACKED_PIXEL_STREAM_16 = 0x0e,
    MIPITX_DSI_PACKED_PIXEL_STREAM_18 = 0x1e,
    MIPITX_DSI_PIXEL_STREAM_3BYTE_18 = 0x2e,
    MIPITX_DSI_PACKED_PIXEL_STREAM_24 = 0x3e,
};

enum {
	MIPITX_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT	= 0x02,
	MIPITX_DSI_RX_END_OF_TRANSMISSION			= 0x08,
	MIPITX_DSI_RX_GENERIC_SHORT_READ_RESPONSE_1BYTE	= 0x11,
	MIPITX_DSI_RX_GENERIC_SHORT_READ_RESPONSE_2BYTE	= 0x12,
	MIPITX_DSI_RX_GENERIC_LONG_READ_RESPONSE		= 0x1a,
	MIPITX_DSI_RX_DCS_LONG_READ_RESPONSE		= 0x1c,
	MIPITX_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE	= 0x21,
	MIPITX_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE	= 0x22,
};

/* MIPI DCS commands */
enum {
    MIPITX_DCS_NOP = 0x00,
    MIPITX_DCS_SOFT_RESET = 0x01,
    MIPITX_DCS_GET_DISPLAY_ID = 0x04,
    MIPITX_DCS_GET_RED_CHANNEL = 0x06,
    MIPITX_DCS_GET_GREEN_CHANNEL = 0x07,
    MIPITX_DCS_GET_BLUE_CHANNEL = 0x08,
    MIPITX_DCS_GET_DISPLAY_STATUS = 0x09,
    MIPITX_DCS_GET_POWER_MODE = 0x0A,
    MIPITX_DCS_GET_ADDRESS_MODE = 0x0B,
    MIPITX_DCS_GET_PIXEL_FORMAT = 0x0C,
    MIPITX_DCS_GET_DISPLAY_MODE = 0x0D,
    MIPITX_DCS_GET_SIGNAL_MODE = 0x0E,
    MIPITX_DCS_GET_DIAGNOSTIC_RESULT = 0x0F,
    MIPITX_DCS_ENTER_SLEEP_MODE = 0x10,
    MIPITX_DCS_EXIT_SLEEP_MODE = 0x11,
    MIPITX_DCS_ENTER_PARTIAL_MODE = 0x12,
    MIPITX_DCS_ENTER_NORMAL_MODE = 0x13,
    MIPITX_DCS_EXIT_INVERT_MODE = 0x20,
    MIPITX_DCS_ENTER_INVERT_MODE = 0x21,
    MIPITX_DCS_SET_GAMMA_CURVE = 0x26,
    MIPITX_DCS_SET_DISPLAY_OFF = 0x28,
    MIPITX_DCS_SET_DISPLAY_ON = 0x29,
    MIPITX_DCS_SET_COLUMN_ADDRESS = 0x2A,
    MIPITX_DCS_SET_PAGE_ADDRESS = 0x2B,
    MIPITX_DCS_WRITE_MEMORY_START = 0x2C,
    MIPITX_DCS_WRITE_LUT = 0x2D,
    MIPITX_DCS_READ_MEMORY_START = 0x2E,
    MIPITX_DCS_SET_PARTIAL_AREA = 0x30,
    MIPITX_DCS_SET_SCROLL_AREA = 0x33,
    MIPITX_DCS_SET_TEAR_OFF = 0x34,
    MIPITX_DCS_SET_TEAR_ON = 0x35,
    MIPITX_DCS_SET_ADDRESS_MODE = 0x36,
    MIPITX_DCS_SET_SCROLL_START = 0x37,
    MIPITX_DCS_EXIT_IDLE_MODE = 0x38,
    MIPITX_DCS_ENTER_IDLE_MODE = 0x39,
    MIPITX_DCS_SET_PIXEL_FORMAT = 0x3A,
    MIPITX_DCS_WRITE_MEMORY_CONTINUE = 0x3C,
    MIPITX_DCS_READ_MEMORY_CONTINUE = 0x3E,
    MIPITX_DCS_SET_TEAR_SCANLINE = 0x44,
    MIPITX_DCS_GET_SCANLINE = 0x45,
    MIPITX_DCS_SET_DISPLAY_BRIGHTNESS = 0x51,
    MIPITX_DCS_GET_DISPLAY_BRIGHTNESS = 0x52,
    MIPITX_DCS_WRITE_CONTROL_DISPLAY = 0x53,
    MIPITX_DCS_GET_CONTROL_DISPLAY = 0x54,
    MIPITX_DCS_WRITE_POWER_SAVE = 0x55,
    MIPITX_DCS_GET_POWER_SAVE = 0x56,
    MIPITX_DCS_SET_CABC_MIN_BRIGHTNESS = 0x5E,
    MIPITX_DCS_GET_CABC_MIN_BRIGHTNESS = 0x5F,
    MIPITX_DCS_READ_DDB_START = 0xA1,
    MIPITX_DCS_READ_DDB_CONTINUE = 0xA8,
};

typedef enum _MIPITX_DSI_MSG_EVENT{
    HBK_PACKET_MSG_EVENT = 0,
    VBK_PACKET_MSG_EVENT,
}MIPITX_DSI_MSG_EVENT;


/**
 * struct mipi_dsi_msg - read/write DSI buffer
 * @channel: virtual channel id
 * @type: payload data type
 * @event: dsi msg event
 * @tx_len: length of @tx_buf
 * @tx_buf: data to be written
 * @rx_len: length of @rx_buf
 * @rx_buf: data to be read, or NULL
 */
typedef struct _MIPITX_DSI_MSG {
	UINT8 channel;
	UINT8 type;
	MIPITX_DSI_MSG_EVENT event;
       UINT8 rx_cmd;

	UINT32 tx_len;
	const UINT8 *tx_buf;

	UINT32 rx_len;
	UINT8 rx_buf[16];
}MIPITX_DSI_MSG;


extern UINT8 mipitx_dsi_tx_format_is_long(UINT8 type);
extern UINT8 mipitx_dsi_tx_format_is_short(UINT8 type);
extern UINT8 mipitx_dsi_rx_format_is_long(UINT8 type);
extern UINT8 mipitx_dsi_rx_format_is_short(UINT8 type);
extern int mipitx_dsi_msg_write(MIPITX_DSI_MSG *msg);
extern int mipitx_dsi_msg_send(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *buf, UINT32 size);
extern int mipitx_dsi_generic_write(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *buf, UINT32 size);
extern int mipitx_dsi_dcs_write(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *buf, UINT32 buf_size, UINT8 param_num);
extern int mipitx_dsi_dcs_read(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *tx_buf, UINT32 tx_size, UINT8 *rx_buf, UINT32 rx_size);
extern int mipitx_dsi_shutdown_peripheral(void);
extern int mipitx_dsi_turn_on_peripheral(void);
extern int mipitx_dsi_set_maximum_return_packet_size(UINT16 value);
extern int mipitx_dsi_dcs_nop(void);
extern int mipitx_dsi_dcs_soft_reset(void);
extern int mipitx_dsi_dcs_get_power_mode(UINT8 *mode);
extern int mipitx_dsi_dcs_get_pixel_format(UINT8 *format);
extern int mipitx_dsi_dcs_enter_sleep_mode(void);
extern int mipitx_dsi_dcs_exit_sleep_mode(void);
extern int mipitx_dsi_dcs_set_display_off(void);
extern int mipitx_dsi_dcs_set_display_on(void);
extern int mipitx_dsi_dcs_set_column_address(UINT16 start, UINT16 end);
extern int mipitx_dsi_dcs_set_page_address(UINT16 start, UINT16 end);
extern int mipitx_dsi_dcs_set_tear_off(void);
extern int mipitx_dsi_dcs_set_tear_on(UINT8 hsync);
extern int mipitx_dsi_dcs_set_pixel_format(UINT8 format);
extern int mipitx_dsi_dcs_set_tear_scanline(UINT16 line);
extern int mipitx_dsi_dcs_set_display_brightness(UINT16 brightness);
extern int mipitx_dsi_dcs_get_display_brightness(UINT16 *brightness);

#endif

