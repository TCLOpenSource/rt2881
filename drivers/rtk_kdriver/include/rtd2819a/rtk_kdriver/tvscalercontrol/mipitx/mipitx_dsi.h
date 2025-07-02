#ifndef _MIPITX_DSI_H_
#define _MIPITX_DSI_H_
#include <tvscalercontrol/mipitx/mipitx_packet.h>

typedef enum _MIPITX_DSI_APPLY_EVENT{
    BTA_APPLY_EVENT = 0,
    CAL_APPLY_EVENT,
    EXIT_ULPS_APPLY_EVENT,
    ESC_ULPS_APPLY_EVENT,
    ESC_TRIGGER_APPLY_EVENT,
    ESC_LPDT_APPLY_EVENT,
    HBK_HSDT_APPLY_EVENT,
    VBK_HSDT_APPLY_EVENT,
    EXIT_ULPS_CK_LANE_APPLY_EVENT,
    ESC_ULPS_CK_LANE_APPLY_EVENT,
}MIPITX_DSI_APPLY_EVENT;

typedef enum _MIPITX_ESCAPE_TRIGGER_MODE{
    RESET_TRIGGER_MODE = 0x1,
    UNKNOWN3_MODE = 0x2,
    UNKNOWN4_MODE = 0x4,
    UNKNOWN5_MODE = 0x8,
}MIPITX_ESCAPE_TRIGGER_MODE;

extern void mipitx_dsi_config(void);
extern int mipitx_dsi_vbk_packet_write(MIPITX_DSI_MSG *msg);
extern int mipitx_dsi_hbk_packet_write(MIPITX_DSI_MSG *msg);
extern int mipitx_dsi_msg_read(MIPITX_DSI_MSG *msg);
extern void mipitx_lprx_interrput_enable(UINT8 enable);
extern void mipitx_lprx_rec_trigger_handle(void);
extern void mipitx_lprx_rec_data_handle(MIPITX_DSI_MSG *msg);
extern UINT32 mipitx_lprx_ISR(void);
extern int mipitx_bta_apply(void);
extern int mipitx_wait_dsi_event_apply(MIPITX_DSI_APPLY_EVENT event);
extern int mipitx_command_mode_trigger_init(void);
extern int mipitx_command_mode_trigger_vsync(void);
extern int mipitx_command_mode_event_apply(MIPITX_DSI_APPLY_EVENT event);
extern int mipitx_enable_dcs_header(UINT8 enable);
extern int mipitx_command_mode_escape_trigger(MIPITX_ESCAPE_TRIGGER_MODE mode);

#endif
