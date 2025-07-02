#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/delay.h>

#include <rbus/mipi_dsi_reg.h>
#include <rbus/sfg_reg.h>
#include <rbus/ppoverlay_reg.h>

#include <tvscalercontrol/mipitx/mipitx.h>
#include <tvscalercontrol/mipitx/mipitx_dsi.h>
#include <tvscalercontrol/mipitx/mipitx_packet.h>

#include <tvscalercontrol/io/ioregdrv.h>


#ifndef IRQ_HANDLED
#define IRQ_HANDLED  1
#define IRQ_NONE     0
#endif


#define VBK_HSDT_MAX_SIZE 141
#define HBK_HSDT_MAX_SIZE 13

extern MIPITX_DSI_MSG receive_msg;

void mipitx_dsi_config(void)
{
    UINT32 value = 0;
    UINT32 sfg_dly = 0;
    mipi_dsi_mipi_ctrl_0_RBUS mipi_dsi_mipi_ctrl_0_reg;
    mipi_dsi_mipi_ctrl_1_RBUS mipi_dsi_mipi_ctrl_1_reg;
    mipi_dsi_mipi_ctrl_2_RBUS mipi_dsi_mipi_ctrl_2_reg;
    mipi_dsi_mipi_ctrl_3_RBUS mipi_dsi_mipi_ctrl_3_reg;
    mipi_dsi_mipi_ctrl_7_RBUS mipi_dsi_mipi_ctrl_7_reg;

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    mipi_dsi_mipi_ctrl_0_reg.mipitx_en = 0;
    mipi_dsi_mipi_ctrl_0_reg.disp_en = 0;
    if((default_mipitx_parameter.lane_cnt == DUAL_PANEL_ONE_LANE) || (default_mipitx_parameter.lane_cnt == DUAL_PANEL_TWO_LANE) || (default_mipitx_parameter.lane_cnt == DUAL_PANEL_FOUR_LANE)) {
        mipi_dsi_mipi_ctrl_0_reg.dual_mode_en = 1;
    } else {
        mipi_dsi_mipi_ctrl_0_reg.dual_mode_en = 0;
    }

    if(default_mipitx_parameter.lane_cnt == DUAL_PANEL_ONE_LANE) {
        mipi_dsi_mipi_ctrl_0_reg.lane_cnt_sel = 0;
    } else if((default_mipitx_parameter.lane_cnt == SINGLE_PANEL_TWO_LANE) || (default_mipitx_parameter.lane_cnt == DUAL_PANEL_ONE_LANE)){
        mipi_dsi_mipi_ctrl_0_reg.lane_cnt_sel = 1;
    } else if((default_mipitx_parameter.lane_cnt == SINGLE_PANEL_FOUR_LANE) || (default_mipitx_parameter.lane_cnt == DUAL_PANEL_TWO_LANE)){
        mipi_dsi_mipi_ctrl_0_reg.lane_cnt_sel = 2;
    } else if((default_mipitx_parameter.lane_cnt == SINGLE_PANEL_EIGHT_LANE) || (default_mipitx_parameter.lane_cnt == DUAL_PANEL_FOUR_LANE)){
        mipi_dsi_mipi_ctrl_0_reg.lane_cnt_sel = 3;
    }

    mipi_dsi_mipi_ctrl_0_reg.color_depth = default_mipitx_parameter.color_depth;
    mipi_dsi_mipi_ctrl_0_reg.txrequesths_ck_lane = 1;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);

    mipi_dsi_mipi_ctrl_1_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_1_reg);
    // video_length: 1080*24/8
    value = default_mipitx_parameter.hwidth * default_mipitx_parameter.color_bits / 8;
    mipi_dsi_mipi_ctrl_1_reg.video_length = value; 
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_1_reg, mipi_dsi_mipi_ctrl_1_reg.regValue);

    mipi_dsi_mipi_ctrl_2_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_2_reg);
    // hsa_length: 18*24/8-4-6-4<0? 0:12*24/3/8-4-6-4
    value = default_mipitx_parameter.hsyncwidth * default_mipitx_parameter.color_bits / 8;
    if(value < 14)
        value = 0;
    else
        value -= 14;
    mipi_dsi_mipi_ctrl_2_reg.hsa_length = value;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_2_reg, mipi_dsi_mipi_ctrl_2_reg.regValue);

    mipi_dsi_mipi_ctrl_3_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_3_reg);
    // hbp_f_length:90*24/8-6-2 , hbp_b_length: 72*24/8-6-4
    value = default_mipitx_parameter.hbackporch* default_mipitx_parameter.color_bits / 8 - 10;
    mipi_dsi_mipi_ctrl_3_reg.hbp_b_length = value;
    value = default_mipitx_parameter.hfrontporch* default_mipitx_parameter.color_bits / 8 - 8;
    mipi_dsi_mipi_ctrl_3_reg.hbp_f_length = value;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_3_reg, mipi_dsi_mipi_ctrl_3_reg.regValue);

    mipi_dsi_mipi_ctrl_7_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_7_reg);
    // dsc_en Sync_delay: (sfg_dly*2*32/8-4)/lane_num-29+19; dsc_non Sync_delay: (sfg_dly*2*color_bits/8-4)/lane_num-29+19 
    if(SFG_SFG_CLKEN_CTRL1_get_sfg_top_bps_en(IoReg_Read32(SFG_SFG_CLKEN_CTRL1_reg)) == 1) {
        sfg_dly = SFG_SFG_CLKEN_CTRL1_get_sfg_in_dly_val(IoReg_Read32(SFG_SFG_CLKEN_CTRL1_reg));
    } else {
        sfg_dly = default_mipitx_parameter.htotal;
    }
    if(default_mipitx_parameter.dsc_en == 1) {
        value = (sfg_dly * 2 * 32 /8 -4) / default_mipitx_parameter.lane_cnt -29 +19;
    } else {
        value = (sfg_dly * 2 * default_mipitx_parameter.color_bits /8 -4) / default_mipitx_parameter.lane_cnt -29 +19;
    }
    mipi_dsi_mipi_ctrl_7_reg.sync_delay = value;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_7_reg, mipi_dsi_mipi_ctrl_7_reg.regValue);
    mdelay(10);

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    mipi_dsi_mipi_ctrl_0_reg.mipitx_en = 1;
    mipi_dsi_mipi_ctrl_0_reg.txrequesths_ck_lane = 0;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
    mdelay(10);

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    mipi_dsi_mipi_ctrl_0_reg.mipitx_en = 1;
    mipi_dsi_mipi_ctrl_0_reg.txrequesths_ck_lane = 1;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
    mdelay(10);

    mipitx_lprx_interrput_enable(true);

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    mipi_dsi_mipi_ctrl_0_reg.mipitx_en = 1;
    mipi_dsi_mipi_ctrl_0_reg.disp_en = 1;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);

    if(default_mipitx_parameter.is_cmd_panel) {
        mipitx_command_mode_trigger_init();
    }
}

int mipitx_dsi_vbk_packet_write(MIPITX_DSI_MSG *msg)
{
    UINT8 i =0;
    UINT8 di = 0; // byte0 DATA ID
    mipi_dsi_mipi_ctrl_0_RBUS mipi_dsi_mipi_ctrl_0_reg;
    mipi_dsi_mipi_ctrl_7_RBUS mipi_dsi_mipi_ctrl_7_reg;

    if(msg == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_vbk_hsdt_msg_write msg null\n");
        return MIPITX_ERR;
    }
    if(msg->tx_buf == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_vbk_hsdt_msg_write tx_buf null\n");
        return MIPITX_ERR;
    }
    if(msg->tx_len >= VBK_HSDT_MAX_SIZE) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_vbk_hsdt_msg_write tx_buf overflow\n");
        return MIPITX_ERR;
    }
    if (!mipitx_dsi_tx_format_is_short(msg->type) &&  !mipitx_dsi_tx_format_is_long(msg->type)) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_vbk_hsdt_msg_write format fail\n");
        return MIPITX_ERR;
    }

    di = ((msg->channel & 0x3) << 6) | (msg->type & 0x3f);
    if(mipitx_dsi_tx_format_is_short(msg->type)) {
        if(msg->tx_len != 2) {
            MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_vbk_hsdt_msg_write tx_len=%d fail\n", msg->tx_len);
            return MIPITX_ERR;
        }
        mipi_dsi_mipi_ctrl_7_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_7_reg);
        mipi_dsi_mipi_ctrl_7_reg.vbk_pkt_byte_num = 2;
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_7_reg, mipi_dsi_mipi_ctrl_7_reg.regValue);
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_12_reg, (UINT32)(((msg->tx_buf[1]<<16)&0xff0000) | ((msg->tx_buf[0]<<8)&0xff00) | (di&0xff)));
    } else {
        UINT8 cw1 = (msg->tx_len >> 0) & 0xff;
        UINT8 cw2 = (msg->tx_len >> 8) & 0xff;
        int size = 0;
        mipi_dsi_mipi_ctrl_7_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_7_reg);
        mipi_dsi_mipi_ctrl_7_reg.vbk_pkt_byte_num = msg->tx_len + 2; // size = DI+CW+SIZE-1
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_7_reg, mipi_dsi_mipi_ctrl_7_reg.regValue);

        if(msg->tx_len == 0) {
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_12_reg, (UINT32)(((cw2<<16)&0xff0000) | ((cw1<<8)&0xff00) | (di&0xff)));
            size = 0;
            i = 0;
        } else {
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_12_reg, (UINT32)(((msg->tx_buf[0]<<24)&0xff000000) |((cw2<<16)&0xff0000) | ((cw1<<8)&0xff00) | (di&0xff)));
            size = msg->tx_len - 1;
            i = 0;
        }
        while(size > 0) {
            if(size == 1)
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_13_reg + i, (UINT32)(msg->tx_buf[i+1] & 0xff));
            else if(size == 2)
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_13_reg + i, (UINT32)(((msg->tx_buf[i+2]<<8)&0xff00) | (msg->tx_buf[i+1] & 0xff)));
            else if(size == 3)
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_13_reg + i, (UINT32)(((msg->tx_buf[i+3]<<16)&0xff0000) | ((msg->tx_buf[i+2]<<8)&0xff00) | (msg->tx_buf[i+1] & 0xff)));
            else
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_13_reg + i, (UINT32)(((msg->tx_buf[i+4]<<24)&0xff000000) | ((msg->tx_buf[i+3]<<16)&0xff0000) | ((msg->tx_buf[i+2]<<8)&0xff00) | (msg->tx_buf[i+1] & 0xff)));
            
            MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_vbk_hsdt_msg_write send packet addr %x=%x\n", MIPI_DSI_MIPI_CTRL_13_reg + i, IoReg_Read32(MIPI_DSI_MIPI_CTRL_13_reg + i));
            size -= 4;
            i += 4;  
        }

    }

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);

    if(mipi_dsi_mipi_ctrl_0_reg.disp_en == 0) {
        return mipitx_command_mode_event_apply(ESC_LPDT_APPLY_EVENT);
    } else {
        return mipitx_command_mode_event_apply(VBK_HSDT_APPLY_EVENT);
    }

    return MIPITX_OK;
        
}

int mipitx_dsi_hbk_packet_write(MIPITX_DSI_MSG *msg)
{
    UINT8 i =0;
    UINT8 di = 0; // byte0 DATA ID
    mipi_dsi_mipi_ctrl_7_RBUS mipi_dsi_mipi_ctrl_7_reg;

    if(msg == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_hbk_hsdt_msg_write msg null\n");
        return MIPITX_ERR;
    }
    if(msg->tx_buf == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_hbk_hsdt_msg_write tx_buf null\n");
        return MIPITX_ERR;
    }
    if(msg->tx_len >= HBK_HSDT_MAX_SIZE) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_hbk_hsdt_msg_write tx_buf overflow\n");
        return MIPITX_ERR;
    }
    if (!mipitx_dsi_tx_format_is_short(msg->type) &&  !mipitx_dsi_tx_format_is_long(msg->type)) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_hbk_hsdt_msg_write format fail\n");
        return MIPITX_ERR;
    }

    di = ((msg->channel & 0x3) << 6) | (msg->type & 0x3f);
    if(mipitx_dsi_tx_format_is_short(msg->type)) {
        if(msg->tx_len != 2) {
            MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_hbk_hsdt_msg_write tx_len=%d fail\n", msg->tx_len);
            return MIPITX_ERR;
        }
        mipi_dsi_mipi_ctrl_7_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_7_reg);
        mipi_dsi_mipi_ctrl_7_reg.hbk_pkt_byte_num = 2;
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_7_reg, mipi_dsi_mipi_ctrl_7_reg.regValue);
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_12_reg, (UINT32)(((msg->tx_buf[1]<<16)&0xff0000) | ((msg->tx_buf[0]<<8)&0xff00) | (di&0xff)));
    } else {
        UINT8 cw1 = (msg->tx_len >> 0) & 0xff;
        UINT8 cw2 = (msg->tx_len >> 8) & 0xff;
        int size = 0;
        mipi_dsi_mipi_ctrl_7_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_7_reg);
        mipi_dsi_mipi_ctrl_7_reg.hbk_pkt_byte_num = msg->tx_len + 2; // size = DI+CW+SIZE-1
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_7_reg, mipi_dsi_mipi_ctrl_7_reg.regValue);

        if(msg->tx_len == 0) {
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_8_reg, (UINT32)(((cw2<<16)&0xff0000) | ((cw1<<8)&0xff00) | (di&0xff)));
            size = 0;
            i = 0;
        } else {
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_8_reg, (UINT32)(((msg->tx_buf[0]<<24)&0xff000000) |((cw2<<16)&0xff0000) | ((cw1<<8)&0xff00) | (di&0xff)));
            size = msg->tx_len - 1;
            i = 0;
        }
        while(size > 0) {
            if(size == 1)
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_9_reg + i, (UINT32)(msg->tx_buf[i+1] & 0xff));
            else if(size == 2)
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_9_reg + i, (UINT32)(((msg->tx_buf[i+2]<<8)&0xff00) | (msg->tx_buf[i+1] & 0xff)));
            else if(size == 3)
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_9_reg + i, (UINT32)(((msg->tx_buf[i+3]<<16)&0xff0000) | ((msg->tx_buf[i+2]<<8)&0xff00) | (msg->tx_buf[i+1] & 0xff)));
            else
                IoReg_Write32(MIPI_DSI_MIPI_CTRL_9_reg + i, (UINT32)(((msg->tx_buf[i+4]<<24)&0xff000000) | ((msg->tx_buf[i+3]<<16)&0xff0000) | ((msg->tx_buf[i+2]<<8)&0xff00) | (msg->tx_buf[i+1] & 0xff)));
            
            MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_hbk_hsdt_msg_write send packet addr %x=%x\n", MIPI_DSI_MIPI_CTRL_9_reg + i, IoReg_Read32(MIPI_DSI_MIPI_CTRL_9_reg + i));
            size -= 4;
            i += 4;  
        }

    }

    mipitx_command_mode_event_apply(HBK_HSDT_APPLY_EVENT);

    return MIPITX_OK;

}

int mipitx_dsi_msg_read(MIPITX_DSI_MSG *msg)
{
    if(msg == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_dsi_msg_readmsg null\n");
        return MIPITX_ERR;
    }

    switch(msg->rx_cmd) {
        case MIPITX_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT:
            MIPITX_PRINT_DEBUG("[mipitx] DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT [%x,%x]\n", msg->rx_buf[0], msg->rx_buf[1]);
            break;
        case MIPITX_DSI_RX_END_OF_TRANSMISSION:
            break;
        case MIPITX_DSI_RX_GENERIC_SHORT_READ_RESPONSE_1BYTE:
            break;
        case MIPITX_DSI_RX_GENERIC_SHORT_READ_RESPONSE_2BYTE:
            break;
        case MIPITX_DSI_RX_GENERIC_LONG_READ_RESPONSE:
            break;
        case MIPITX_DSI_RX_DCS_LONG_READ_RESPONSE:
            break;
        case MIPITX_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE:
            break;
        case MIPITX_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE:
            break;
        default:
            return MIPITX_ERR;
        }

    return MIPITX_OK;
}


void mipitx_lprx_interrput_enable(UINT8 enable)
{
    mipi_dsi_mipi_ctrl_64_RBUS mipi_dsi_mipi_ctrl_64_reg;

    mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
    mipi_dsi_mipi_ctrl_64_reg.lprx_rec_trigger_irq = 1;
    mipi_dsi_mipi_ctrl_64_reg.lprx_rec_data_irq = 1;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_64_reg, mipi_dsi_mipi_ctrl_64_reg.regValue);
    
    mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
    mipi_dsi_mipi_ctrl_64_reg.lprx_rec_trigger_irq_en = enable;
    mipi_dsi_mipi_ctrl_64_reg.lprx_rec_data_irq_en = enable;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_64_reg, mipi_dsi_mipi_ctrl_64_reg.regValue);
}

void mipitx_lprx_rec_trigger_handle(void)
{
    UINT8 trigger_cmd = 0;
    mipi_dsi_mipi_ctrl_64_RBUS mipi_dsi_mipi_ctrl_64_reg;
    mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
    trigger_cmd = mipi_dsi_mipi_ctrl_64_reg.lprx_trigger_cmd;

    MIPITX_PRINT_DEBUG("[mipitx] lprx receive trigger cmd=%d\n",trigger_cmd);

    switch(trigger_cmd) {
        case 1: // Recevived 4'b0001 for Reset Trigger, 01100010
            break;
        case 2: // Recevived 4'b0010 for TE Trigger, 01011101
//            vbe_disp_sw_trigger_vs_gen(); // SW mode
            break;
        case 4: // Recevived 4'b0100 for Ack Trigger, 00100001
            break;
        case 8: // Recevived 4'b1000 for Unknown-5, 10100000
            break;
        default:
            return;
    }
    
}

void mipitx_lprx_rec_data_handle(MIPITX_DSI_MSG *msg)
{
    UINT8 data_size = 0;
    UINT8 data_id = 0;
    mipi_dsi_mipi_ctrl_64_RBUS mipi_dsi_mipi_ctrl_64_reg;
    mipi_dsi_mipi_ctrl_68_RBUS mipi_dsi_mipi_ctrl_68_reg;
    mipi_dsi_mipi_ctrl_69_RBUS mipi_dsi_mipi_ctrl_69_reg;
    mipi_dsi_mipi_ctrl_70_RBUS mipi_dsi_mipi_ctrl_70_reg;
    mipi_dsi_mipi_ctrl_71_RBUS mipi_dsi_mipi_ctrl_71_reg;

    if(msg == NULL) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_lprx_rec_data_handle null\n");
        return;
    }

    mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
    data_size = mipi_dsi_mipi_ctrl_64_reg.lprx_rec_byte_num;

    MIPITX_PRINT_DEBUG("[mipitx] lprx receive data size=%d\n",data_size);

    if(data_size < 4 || data_size > 16) {
        MIPITX_PRINT_DEBUG("[mipitx] lprx receive data size unvaild =%d\n",data_size);
        return;
    }

    mipi_dsi_mipi_ctrl_68_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_68_reg);
    data_id = mipi_dsi_mipi_ctrl_68_reg.lprx_byte_00;
    //msg->channel = (data_id >> 6) & 0x3;
    msg->rx_cmd = data_id & 0x3f;

    if (!mipitx_dsi_rx_format_is_short(msg->rx_cmd) &&  !mipitx_dsi_rx_format_is_long(msg->rx_cmd)) {
        MIPITX_PRINT_DEBUG("[mipitx] lprx receive data type fail =%d\n",msg->rx_cmd);
        return;
    }

    if(mipitx_dsi_rx_format_is_short(msg->rx_cmd)) {
        if(data_size != 4) {
            MIPITX_PRINT_DEBUG("[mipitx] lprx receive data size and type no match =%d\n",data_size);
            return;
        }
        msg->rx_len = 2;
        msg->rx_buf[0] = mipi_dsi_mipi_ctrl_68_reg.lprx_byte_01;
        msg->rx_buf[1] = mipi_dsi_mipi_ctrl_68_reg.lprx_byte_02;
    } else {
        int cw = 0;
        cw = (mipi_dsi_mipi_ctrl_68_reg.lprx_byte_01 & 0xff) | ((mipi_dsi_mipi_ctrl_68_reg.lprx_byte_02 << 8) & 0xff00);
        msg->rx_len = cw;
        MIPITX_PRINT_DEBUG("[mipitx] lprx receive data long pkt CW =%d\n",msg->rx_len);
#if 0
        if(!cw)
            msg.rx_buf[0] = mipi_dsi_mipi_ctrl_68_reg.lprx_byte_03;
        else {
            MIPITX_PRINT_DEBUG("[mipitx] lprx receive data cw is null\n");
            return;
        }
#endif
        mipi_dsi_mipi_ctrl_69_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_69_reg);
        mipi_dsi_mipi_ctrl_70_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_70_reg);
        mipi_dsi_mipi_ctrl_71_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_71_reg);
        msg->rx_buf[0] = mipi_dsi_mipi_ctrl_69_reg.lprx_byte_04;
        msg->rx_buf[1] = mipi_dsi_mipi_ctrl_69_reg.lprx_byte_05;
        msg->rx_buf[2] = mipi_dsi_mipi_ctrl_69_reg.lprx_byte_06;
        msg->rx_buf[3] = mipi_dsi_mipi_ctrl_69_reg.lprx_byte_07;
        msg->rx_buf[4] = mipi_dsi_mipi_ctrl_70_reg.lprx_byte_08;
        msg->rx_buf[5] = mipi_dsi_mipi_ctrl_70_reg.lprx_byte_09;
        msg->rx_buf[6] = mipi_dsi_mipi_ctrl_70_reg.lprx_byte_10;
        msg->rx_buf[7] = mipi_dsi_mipi_ctrl_70_reg.lprx_byte_11;
        msg->rx_buf[8] = mipi_dsi_mipi_ctrl_71_reg.lprx_byte_12;
        msg->rx_buf[9] = mipi_dsi_mipi_ctrl_71_reg.lprx_byte_13;
        msg->rx_buf[10] = mipi_dsi_mipi_ctrl_71_reg.lprx_byte_14;
        msg->rx_buf[11] = mipi_dsi_mipi_ctrl_71_reg.lprx_byte_15;
    }

    mipitx_dsi_msg_read(msg);
}

UINT32 mipitx_lprx_ISR(void)
{
    UINT8 handle_lprx_rec_trigger_flag = 0;
    UINT8 handle_lprx_rec_data_flag = 0;
    mipi_dsi_mipi_ctrl_64_RBUS mipi_dsi_mipi_ctrl_64_reg;
    mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);

    if((mipi_dsi_mipi_ctrl_64_reg.lprx_rec_trigger_irq_en == 1) && (mipi_dsi_mipi_ctrl_64_reg.lprx_rec_trigger_irq== 1))
        handle_lprx_rec_trigger_flag = 1;
    if((mipi_dsi_mipi_ctrl_64_reg.lprx_rec_data_irq_en == 1) && (mipi_dsi_mipi_ctrl_64_reg.lprx_rec_data_irq== 1))
        handle_lprx_rec_data_flag = 1;

    if(handle_lprx_rec_trigger_flag) {
        mipitx_lprx_rec_trigger_handle();
        mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
        mipi_dsi_mipi_ctrl_64_reg.lprx_rec_trigger_irq = 1;
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_64_reg, mipi_dsi_mipi_ctrl_64_reg.regValue);

        return IRQ_HANDLED;
    }

    if(handle_lprx_rec_data_flag) {
        mipitx_lprx_rec_data_handle(&receive_msg);
        mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
        mipi_dsi_mipi_ctrl_64_reg.lprx_rec_data_irq = 1;
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_64_reg, mipi_dsi_mipi_ctrl_64_reg.regValue);

        return IRQ_HANDLED;
    }
    
    return IRQ_NONE;
}

int mipitx_bta_apply(void)
{
    return mipitx_command_mode_event_apply(BTA_APPLY_EVENT);
}         

int mipitx_wait_dsi_event_apply(MIPITX_DSI_APPLY_EVENT event)
{
    UINT32 timeoutcnt = 100000;
    UINT32 status = 0;
    do {
        switch(event) {
            case BTA_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_bta_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case CAL_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_cal_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case EXIT_ULPS_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_exit_ulps_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case ESC_ULPS_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_esc_ulps_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case ESC_TRIGGER_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_esc_trigger_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case ESC_LPDT_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_esc_lpdt_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case HBK_HSDT_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_hbk_hsdt_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case VBK_HSDT_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_vbk_hsdt_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case EXIT_ULPS_CK_LANE_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_exit_ulps_ck_lane_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            case ESC_ULPS_CK_LANE_APPLY_EVENT:
                status = MIPI_DSI_MIPI_CTRL_0_get_esc_ulps_ck_lane_apply(IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg));
                break;
            default:
                MIPITX_PRINT_DEBUG("[mipitx] invaild event [%d]\n",event);
                return MIPITX_ERR;
        }
        if(!status)
            break;
    } while(timeoutcnt--);

    if(timeoutcnt == 0) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_wait_dsi_event_apply [%d] timeout\n",event);
        return MIPITX_ERR;
    }
    return MIPITX_OK;
}

int mipitx_wait_all_lane_stop_state(void)
{
    UINT32 timeoutcnt = 100000;

    do {
        if((IoReg_Read32(MIPI_DSI_MIPI_CTRL_52_reg) & 0x3FF) == 0x3FF)
            break;
        
    } while (timeoutcnt --);

    if(timeoutcnt == 0) {
        MIPITX_PRINT_DEBUG("[mipitx] mipitx_wait_all_lane_stop_state [%x] timeout\n",IoReg_Read32(MIPI_DSI_MIPI_CTRL_52_reg));
        return MIPITX_ERR;
    }
    return MIPITX_OK;
}

int mipitx_command_mode_trigger_init(void)
{
    mipi_dsi_mipi_ctrl_0_RBUS mipi_dsi_mipi_ctrl_0_reg;
    mipi_dsi_mipi_ctrl_2_RBUS mipi_dsi_mipi_ctrl_2_reg;

    // set disp_en = 0, set force_disp_mode= 1, sel TE trigger dtg vsync
    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    mipi_dsi_mipi_ctrl_0_reg.disp_en = 0;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);

    mipi_dsi_mipi_ctrl_2_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_2_reg);
    mipi_dsi_mipi_ctrl_2_reg.force_disp_mode= 1;
    mipi_dsi_mipi_ctrl_2_reg.vs_te_pkt_trig_sel = 1; // sel TE trigger dtg vsync
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_2_reg, mipi_dsi_mipi_ctrl_2_reg.regValue);

    return MIPITX_OK;

}
int mipitx_command_mode_trigger_vsync(void)
{
    mipi_dsi_mipi_ctrl_64_RBUS mipi_dsi_mipi_ctrl_64_reg;
    mipi_dsi_mipi_ctrl_74_RBUS mipi_dsi_mipi_ctrl_74_reg;

    // 1.clear lprx_rec_trigger_irq, clear trig_cnt
    mipi_dsi_mipi_ctrl_64_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_64_reg);
    mipi_dsi_mipi_ctrl_64_reg.lprx_rec_trigger_irq = 1;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_64_reg, mipi_dsi_mipi_ctrl_64_reg.regValue);

    mipi_dsi_mipi_ctrl_74_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_74_reg);
    mipi_dsi_mipi_ctrl_74_reg.trig_cnt = 0xFFFFF;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_74_reg, mipi_dsi_mipi_ctrl_74_reg.regValue);

    // 2.send dcs tear_on command to rx
    if(mipitx_dsi_dcs_set_tear_on(0) == MIPITX_ERR) { // vsync tear on
        return MIPITX_ERR;
    }

    // 3.set bta_apply=1 and wait bta_apply=0
    if(mipitx_bta_apply() == MIPITX_ERR) {
        return MIPITX_ERR;
    }

    // 4. print trig_cnt
    MIPITX_PRINT_DEBUG("[mipitx] current trigger cnt=%x\n",IoReg_Read32(MIPI_DSI_MIPI_CTRL_74_reg) & MIPI_DSI_MIPI_CTRL_74_trig_cnt_mask);

    return MIPITX_OK;
}

int mipitx_command_mode_event_apply(MIPITX_DSI_APPLY_EVENT event)
{
    mipi_dsi_mipi_ctrl_0_RBUS mipi_dsi_mipi_ctrl_0_reg;

    switch(event) {
        case BTA_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.bta_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case CAL_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.cal_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case EXIT_ULPS_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.exit_ulps_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case ESC_ULPS_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.esc_ulps_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case ESC_TRIGGER_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.esc_trigger_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case ESC_LPDT_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.esc_lpdt_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case HBK_HSDT_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.hbk_hsdt_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case VBK_HSDT_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.vbk_hsdt_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case EXIT_ULPS_CK_LANE_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.exit_ulps_ck_lane_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        case ESC_ULPS_CK_LANE_APPLY_EVENT:
            mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
            mipi_dsi_mipi_ctrl_0_reg.esc_ulps_ck_lane_apply = 1;
            IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
            break;
        default:
            MIPITX_PRINT_DEBUG("[mipitx] invaild event [%d]\n",event);
            return MIPITX_ERR;
    }

    return mipitx_wait_dsi_event_apply(event);

}

int mipitx_enable_dcs_header(UINT8 enable)
{
    mipi_dsi_mipi_ctrl_0_RBUS mipi_dsi_mipi_ctrl_0_reg;

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    mipi_dsi_mipi_ctrl_0_reg.dcs_header_en = enable;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);

    return MIPITX_OK;
}

int mipitx_command_mode_ulps_enable(UINT8 enable)
{
    mipi_dsi_mipi_ctrl_0_RBUS mipi_dsi_mipi_ctrl_0_reg;

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    if(mipi_dsi_mipi_ctrl_0_reg.disp_en == 1) {
        mipi_dsi_mipi_ctrl_0_reg.disp_en = 0;
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
    }

    if(enable) {
        if(mipitx_command_mode_event_apply(ESC_ULPS_APPLY_EVENT) == MIPITX_ERR)
            return MIPITX_ERR;
        if(mipitx_command_mode_event_apply(ESC_ULPS_CK_LANE_APPLY_EVENT) == MIPITX_ERR)
            return MIPITX_ERR;
    } else {
        if(mipitx_command_mode_event_apply(EXIT_ULPS_CK_LANE_APPLY_EVENT) == MIPITX_ERR)
            return MIPITX_ERR;
        if(mipitx_command_mode_event_apply(EXIT_ULPS_APPLY_EVENT) == MIPITX_ERR)
            return MIPITX_ERR;
    }
    return MIPITX_OK;

}

int mipitx_command_mode_escape_trigger(MIPITX_ESCAPE_TRIGGER_MODE mode)
{
    mipi_dsi_mipi_ctrl_0_RBUS mipi_dsi_mipi_ctrl_0_reg;
    mipi_dsi_mipi_ctrl_2_RBUS mipi_dsi_mipi_ctrl_2_reg;

    mipi_dsi_mipi_ctrl_0_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_0_reg);
    if(mipi_dsi_mipi_ctrl_0_reg.disp_en == 1) {
        mipi_dsi_mipi_ctrl_0_reg.disp_en = 0;
        IoReg_Write32(MIPI_DSI_MIPI_CTRL_0_reg, mipi_dsi_mipi_ctrl_0_reg.regValue);
    }

    mipi_dsi_mipi_ctrl_2_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_2_reg);
    mipi_dsi_mipi_ctrl_2_reg.txtriggeresc = mode;
    IoReg_Write32(MIPI_DSI_MIPI_CTRL_2_reg, mipi_dsi_mipi_ctrl_2_reg.regValue);

    return mipitx_command_mode_event_apply(ESC_TRIGGER_APPLY_EVENT);

}

void vbe_disp_sw_trigger_vs_gen(void)
{
    sfg_sfg_clken_ctrl2_RBUS sfg_sfg_clken_ctrl2_reg;
    mipi_dsi_mipi_ctrl_74_RBUS mipi_dsi_mipi_ctrl_74_reg;
    ppoverlay_mipi_dtg_ctrl1_RBUS mipi_dtg_ctrl1_REG;
    unsigned int mipi_lptx_clk = 0;
    unsigned int irq_respond_frequency = 10000; // A default value always conforms to the conditions of SW triggering

    sfg_sfg_clken_ctrl2_reg.regValue = IoReg_Read32(SFG_SFG_CLKEN_CTRL2_reg);
    mipi_dsi_mipi_ctrl_74_reg.regValue = IoReg_Read32(MIPI_DSI_MIPI_CTRL_74_reg);

    // mipi_lptx_clk = rbus_clk ( 250MHz ) / mipi_lptx_div_sel
    // div_value = mipi_lptx_div_sel + 1
    mipi_lptx_clk = 250000000 / ( sfg_sfg_clken_ctrl2_reg.mipi_lptx_div_sel + 1 );
    if( mipi_dsi_mipi_ctrl_74_reg.trig_cnt != 0 )
        irq_respond_frequency = mipi_lptx_clk / mipi_dsi_mipi_ctrl_74_reg.trig_cnt ;

    // List an example panel and its expected method here
    // Assume that panel clock : 4400 x 2250 x 60 Hz / panel resolution = 3840 x 2160
    // Vporch is 90 line, and it's 60 x 2250 / 90 = 1500 Hz
    // If irq_respond_frequency > 1500Hz, we execute SW triggering
    if( irq_respond_frequency > 1500 )
    {
        mipi_dtg_ctrl1_REG.regValue = IoReg_Read32(PPOVERLAY_MIPI_DTG_CTRL1_reg);
        mipi_dtg_ctrl1_REG.mipi_vs_gen_en = 1;
        IoReg_Write32(PPOVERLAY_MIPI_DTG_CTRL1_reg, mipi_dtg_ctrl1_REG.regValue);
    }
}
