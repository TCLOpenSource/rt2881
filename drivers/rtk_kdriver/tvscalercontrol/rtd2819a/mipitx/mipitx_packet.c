#include <rbus/mipi_dphy_reg.h>
#include <rbus/mipi_dsi_reg.h>
#include <rbus/sfg_reg.h>

#include <tvscalercontrol/mipitx/mipitx.h>
#include <tvscalercontrol/mipitx/mipitx_packet.h>
#include <tvscalercontrol/mipitx/mipitx_dsi.h>


#ifndef IRQ_HANDLED
#define IRQ_HANDLED  1
#define IRQ_NONE     0
#endif

#define VBK_HSDT_MAX_SIZE 141
#define HBK_HSDT_MAX_SIZE 13

MIPITX_CONFIG_PARAMETER default_mipitx_parameter;
MIPITX_DSI_MSG receive_msg = {0};

UINT8 mipitx_dsi_tx_format_is_long(UINT8 type)
{
    switch (type) {
        case MIPITX_DSI_PPS_LONG_WRITE:
        case MIPITX_DSI_NULL_PACKET:
        case MIPITX_DSI_BLANKING_PACKET:
        case MIPITX_DSI_GENERIC_LONG_WRITE:
        case MIPITX_DSI_DCS_LONG_WRITE:
        case MIPITX_DSI_LOOSELY_PACKED_PIXEL_STREAM_YCBCR20:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_YCBCR24:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_YCBCR16:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_30:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_36:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_YCBCR12:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_16:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_18:
        case MIPITX_DSI_PIXEL_STREAM_3BYTE_18:
        case MIPITX_DSI_PACKED_PIXEL_STREAM_24:
            return true;
    }
        return false;
}

UINT8 mipitx_dsi_tx_format_is_short(UINT8 type)
{
    switch (type) {
        case MIPITX_DSI_V_SYNC_START:
        case MIPITX_DSI_V_SYNC_END:
        case MIPITX_DSI_H_SYNC_START:
        case MIPITX_DSI_H_SYNC_END:
        case MIPITX_DSI_END_OF_TRANSMISSION:
        case MIPITX_DSI_COLOR_MODE_OFF:
        case MIPITX_DSI_COLOR_MODE_ON:
        case MIPITX_DSI_SHUTDOWN_PERIPHERAL:
        case MIPITX_DSI_TURN_ON_PERIPHERAL:
        case MIPITX_DSI_GENERIC_SHORT_WRITE_0_PARAM:
        case MIPITX_DSI_GENERIC_SHORT_WRITE_1_PARAM:
        case MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM:
        case MIPITX_DSI_GENERIC_READ_REQUEST_0_PARAM:
        case MIPITX_DSI_GENERIC_READ_REQUEST_1_PARAM:
        case MIPITX_DSI_GENERIC_READ_REQUEST_2_PARAM:
        case MIPITX_DSI_DCS_SHORT_WRITE:
        case MIPITX_DSI_DCS_SHORT_WRITE_PARAM:
        case MIPITX_DSI_DCS_READ:
        case MIPITX_DSI_DCS_COMPRESSION_MODE:
        case MIPITX_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE:
            return true;
    }
    return false;
}

UINT8 mipitx_dsi_rx_format_is_long(UINT8 type)
{
    switch (type) {
        case MIPITX_DSI_RX_GENERIC_LONG_READ_RESPONSE:
        case MIPITX_DSI_RX_DCS_LONG_READ_RESPONSE:
            return true;
    }
        return false;
}

UINT8 mipitx_dsi_rx_format_is_short(UINT8 type)
{
    switch (type) {
        case MIPITX_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT:
        case MIPITX_DSI_RX_END_OF_TRANSMISSION:
        case MIPITX_DSI_RX_GENERIC_SHORT_READ_RESPONSE_1BYTE:
        case MIPITX_DSI_RX_GENERIC_SHORT_READ_RESPONSE_2BYTE:
        case MIPITX_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE:
        case MIPITX_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE:
            return true;
    }
    return false;
}




int mipitx_dsi_msg_write(MIPITX_DSI_MSG *msg)
{
    if(msg == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_msg_write msg null\n");
        return MIPITX_ERR;
    }

    switch(msg->event) {
        case HBK_PACKET_MSG_EVENT:
            return mipitx_dsi_hbk_packet_write(msg);
        case VBK_PACKET_MSG_EVENT:
            return mipitx_dsi_vbk_packet_write(msg);
        default:
            return MIPITX_ERR;
        }

}

int mipitx_dsi_msg_send(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *buf, UINT32 size)
{
    MIPITX_DSI_MSG msg = {0};

    if(buf == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_msg_send buf null\n");
        return MIPITX_ERR;
    }
    msg.channel = channel,
    msg.event = event;
    msg.type = buf[0];
    msg.tx_buf = buf + 1;
    msg.tx_len = size - 1;

    return mipitx_dsi_msg_write(&msg);
}

int mipitx_dsi_generic_write(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *buf, UINT32 size)
{
    MIPITX_DSI_MSG msg = {0};

    if(buf == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_generic_write buf null\n");
        return MIPITX_ERR;
    }

    msg.channel = channel;
    msg.event = event;
    msg.tx_buf = buf;

    switch (size) {
        case 0:
            msg.type = MIPITX_DSI_GENERIC_SHORT_WRITE_0_PARAM;
            msg.tx_len = 2;
            break;
        case 1:
            msg.type = MIPITX_DSI_GENERIC_SHORT_WRITE_1_PARAM;
            msg.tx_len = 2;
            break;
        case 2:
            msg.type = MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM;
            msg.tx_len = 2;
            break;
        default:
            msg.type = MIPITX_DSI_GENERIC_LONG_WRITE;
            msg.tx_len = size;
            break;
    }

    return mipitx_dsi_msg_write(&msg);
}

int mipitx_dsi_dcs_write(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *buf, UINT32 buf_size, UINT8 param_num)
{
    MIPITX_DSI_MSG msg = {0};

    if(buf == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_dcs_write buf null\n");
        return MIPITX_ERR;
    }

    msg.channel = channel;
    msg.event = event;
    msg.tx_buf = buf;

    switch (param_num) {
        case 0:
            msg.type = MIPITX_DSI_DCS_SHORT_WRITE;
            msg.tx_len = 2;
            break;
        case 1:
            msg.type = MIPITX_DSI_DCS_SHORT_WRITE_PARAM;
            msg.tx_len = 2;
            break;
        default:
            msg.type = MIPITX_DSI_DCS_LONG_WRITE;
            msg.tx_len = buf_size;
            break;
    }

    return mipitx_dsi_msg_write(&msg);
}


int mipitx_dsi_dcs_read(UINT8 channel, MIPITX_DSI_MSG_EVENT event, UINT8 *tx_buf, UINT32 tx_size, UINT8 *rx_buf, UINT32 rx_size)
{

    if((tx_buf == NULL) || (rx_buf == NULL)) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_dcs_read buf null\n");
        return MIPITX_ERR;
    }

    receive_msg.channel = channel;
    receive_msg.event = event;
    receive_msg.type = MIPITX_DSI_DCS_READ;
    receive_msg.tx_buf = tx_buf;
    receive_msg.tx_len = tx_size;


    if(mipitx_dsi_msg_write(&receive_msg) == MIPITX_ERR)
        return MIPITX_ERR;

    mipitx_bta_apply();

    if(mipitx_dsi_rx_format_is_short(receive_msg.rx_cmd) && (rx_size <=2)) {
        if(rx_size ==1) {
            rx_buf[0] = receive_msg.tx_buf[0];
        } else  if(rx_size == 2){
            rx_buf[0] = receive_msg.tx_buf[0];
            rx_buf[1] = receive_msg.tx_buf[1];
        }
    } else if(mipitx_dsi_rx_format_is_long(receive_msg.rx_cmd) && (rx_size == receive_msg.rx_len)){
       UINT8 i = 0;
       while(rx_size) {
            rx_buf[i]  = receive_msg.tx_buf[i];
            i ++;
            rx_size --;
       }
    } else {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_dcs_read buf no match\n");
        return MIPITX_ERR;
    }

    return MIPITX_OK;
    
}


int mipitx_dsi_shutdown_peripheral(void)
{
    UINT8 dsi_buf[] = {MIPITX_DSI_SHUTDOWN_PERIPHERAL, 0x00, 0x00};

    return mipitx_dsi_msg_send(0, VBK_PACKET_MSG_EVENT, dsi_buf, sizeof(dsi_buf));

}

int mipitx_dsi_turn_on_peripheral(void)
{
    UINT8 dsi_buf[] = {MIPITX_DSI_TURN_ON_PERIPHERAL, 0x00, 0x00};

    return mipitx_dsi_msg_send(0, VBK_PACKET_MSG_EVENT, dsi_buf, sizeof(dsi_buf));
}

int mipitx_dsi_set_maximum_return_packet_size(UINT16 value)
{
    UINT8 dsi_buf[3] = {0};
    dsi_buf[0] = MIPITX_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE;
    dsi_buf[1] = value & 0xff;
    dsi_buf[2] = (value >> 8) & 0xff;

    return mipitx_dsi_msg_send(0, VBK_PACKET_MSG_EVENT, dsi_buf, sizeof(dsi_buf));
}

int mipitx_dsi_dcs_nop(void)
{
    // nop cmd+no parameters
    UINT8 dcs_buf[] = {MIPITX_DCS_NOP, 0x00};

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 0);
}

int mipitx_dsi_dcs_soft_reset(void)
{
    // soft_reset cmd+no parameters
    UINT8 dcs_buf[] = {MIPITX_DCS_SOFT_RESET, 0x00};

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 0);
}

int mipitx_dsi_dcs_get_power_mode(UINT8 *mode)
{
    // get_power_mode cmd+0 parameters
    UINT8 dcs_buf[2] = {0};
    dcs_buf[0] = MIPITX_DCS_GET_POWER_MODE;
    dcs_buf[1] = 0x00;

    return mipitx_dsi_dcs_read(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), mode, sizeof(*mode));
}

int mipitx_dsi_dcs_get_pixel_format(UINT8 *format)
{
    // get_pixel_format cmd+0 parameters
    UINT8 dcs_buf[2] = {0};
    dcs_buf[0] = MIPITX_DCS_GET_PIXEL_FORMAT;
    dcs_buf[1] = 0x00;

    return mipitx_dsi_dcs_read(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), format, sizeof(*format));
}

int mipitx_dsi_dcs_enter_sleep_mode(void)
{
    // enter_sleep_mode cmd+0 parameters
    UINT8 dcs_buf[] = {MIPITX_DCS_ENTER_SLEEP_MODE, 0x00};

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 0);
}

int mipitx_dsi_dcs_exit_sleep_mode(void)
{
    // exit_sleep_mode cmd+0 parameters
    UINT8 dcs_buf[] = {MIPITX_DCS_EXIT_SLEEP_MODE, 0x00};

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 0);
}

int mipitx_dsi_dcs_set_display_off(void)
{
    // set_display_off cmd+0 parameters
    UINT8 dcs_buf[] = {MIPITX_DCS_SET_DISPLAY_OFF, 0x00};

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 0);
}

int mipitx_dsi_dcs_set_display_on(void)
{
    // set_display_on cmd+0 parameters
    UINT8 dcs_buf[] = {MIPITX_DCS_SET_DISPLAY_ON, 0x00};

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 0);
}

int mipitx_dsi_dcs_set_column_address(UINT16 start, UINT16 end)
{
    // set_column_address+cw 4
    UINT8 dcs_buf[5] = {0};
    dcs_buf[0] = MIPITX_DCS_SET_COLUMN_ADDRESS;
    dcs_buf[1] = (start >> 8) & 0xff;
    dcs_buf[2] = start & 0xff;
    dcs_buf[3] = (end >> 8) & 0xff;
    dcs_buf[4] = end & 0xff;

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 4);
}

int mipitx_dsi_dcs_set_page_address(UINT16 start, UINT16 end)
{
    // set_page_address+cw 4
    UINT8 dcs_buf[5] = {0};
    dcs_buf[0] = MIPITX_DCS_SET_PAGE_ADDRESS;
    dcs_buf[1] = (start >> 8) & 0xff;
    dcs_buf[2] = start & 0xff;
    dcs_buf[3] = (end >> 8) & 0xff;
    dcs_buf[4] = end & 0xff;

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 4);
}

int mipitx_dsi_dcs_set_tear_off(void)
{
    // set_tear_off cmd+0 parameters
    UINT8 dcs_buf[] = {MIPITX_DCS_SET_TEAR_OFF, 0x00};

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 0);
}

int mipitx_dsi_dcs_set_tear_on(UINT8 hsync)
{
    // set_tear_on cmd+1 parameters
    UINT8 dcs_buf[2] = {0};
    dcs_buf[0] = MIPITX_DCS_SET_TEAR_ON;
    dcs_buf[1] = hsync & 0x01;

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 1);
}

int mipitx_dsi_dcs_set_pixel_format(UINT8 format)
{
    // set_pixel_format cmd+1 parameters
    UINT8 dcs_buf[2] = {0};
    dcs_buf[0] = MIPITX_DCS_SET_PIXEL_FORMAT;
    dcs_buf[1] = format;

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 1);

}

int mipitx_dsi_dcs_set_tear_scanline(UINT16 line)
{
    // set_tear_scanline cmd+two parameters
    UINT8 dcs_buf[3] = {0};
    dcs_buf[0] = MIPITX_DCS_SET_TEAR_SCANLINE;
    dcs_buf[1] = (line >> 8) & 0xff;
    dcs_buf[2] = line & 0xff;

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 2);
}

int mipitx_dsi_dcs_set_display_brightness(UINT16 brightness)
{
    // set_display_brightness cmd+two parameters
    UINT8 dcs_buf[3] = {0};
    dcs_buf[0] = MIPITX_DCS_SET_DISPLAY_BRIGHTNESS;
    dcs_buf[1] = brightness & 0xff;
    dcs_buf[2] = (brightness >> 8) & 0xff;

    return mipitx_dsi_dcs_write(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), 2);
}

int mipitx_dsi_dcs_get_display_brightness(UINT16 *brightness)
{
    // get_pixel_format cmd+0 parameters
    UINT8 dcs_buf[2] = {0};
    dcs_buf[0] = MIPITX_DCS_GET_DISPLAY_BRIGHTNESS;
    dcs_buf[1] = 0x00;

    return mipitx_dsi_dcs_read(0, VBK_PACKET_MSG_EVENT, dcs_buf, sizeof(dcs_buf), (UINT8 *)brightness, sizeof(*brightness));
}