#ifndef __M8P_HDMI_MAC_H_
#define __M8P_HDMI_MAC_H_

#include "hdmi_mac.h"
#include <rtk_kdriver/measure/rtk_measure.h>
#include "m8p_hdmi_reg.h"

#define WAIT_SCALER_READY               0


typedef enum {
    HD20_POLARITY_MANUAL_MEAS,
    HD20_POLARITY_AUDIO_MEAS,
} HD20_MEASURE_POLARITY_MEAS_MODE;

typedef enum {
    DROP_PKT_AUDIO_DATA,
    DROP_PKT_NON_AUDIO_DATA,
    DROP_PKT_HEAD,
    DROP_PKT_ALL
} DROP_PKT_TYPE;

typedef enum {
    HDMI20_MEASURE_SETTING_WRONG = 0, //timing change, restart detect mode
    HDMI20_MEASURE_WAIT_V_SYNC, //no vsync, do nothing, keep before state
    HDMI20_MEASURE_SUCCESS, //measure ok and do nothing, keep before state
} HDMI20_MEASURE_RESULT;


extern void lib_hdmi_video_pll_init(unsigned char nport);

extern void m8p_lib_hdmi_mac_crt_perport_off(unsigned char nport);
extern void m8p_lib_hdmi_mac_crt_perport_on(unsigned char nport);
extern void m8p_lib_hdmi_crt_off(unsigned char nport);
extern void m8p_lib_hdmi_crt_on(unsigned char nport);
extern void m8sp_lib_hdmi_crt_reset_hdmi_common_by_port(unsigned char nport);
extern void m8p_lib_hdmi_crt_off_hdmi_common_by_port(unsigned char nport);
extern unsigned char m8p_lib_hdmi_crt_is_hdmi_common_enabled_by_port(unsigned char nport);
extern void m8p_lib_hdmi_stb_wakeup_by_tmds_init(unsigned char nport, unsigned char en);
extern void m8p_lib_hdmi_mac_afifo_enable(unsigned char nport, unsigned char lane_mode);
void m8p_lib_hdmi_420_en(unsigned char nport, unsigned char en);
void m8p_lib_hdmi_set_420_vactive(unsigned char nport, unsigned int vactive);
void m8p_lib_hdmi_420_fva_ds_mode(unsigned char nport, unsigned char en);
unsigned char m8p_lib_hdmi_set_video_dprxpll(
            unsigned char           nport,
            unsigned int            b_clk,
            unsigned char           frl_mode,
            unsigned char           cd,
            unsigned char           is_interlace,
            unsigned char           is_420,
            unsigned long          *pll_pixelclockx1024
);
void m8p_lib_hdmi_crc_continue_mode_en(unsigned char nport, unsigned char enable);
void m8p_lib_hdmi_crc_auto_cmp_en(unsigned char nport, unsigned char enable);
int m8p_lib_hdmi_get_crc_auto_cmp_status(unsigned char nport);
int m8p_lib_hdmi_read_video_common_crc(unsigned char nport, unsigned char res_sel, unsigned int *p_crc, unsigned char is_continue_mode);
void m8p_lib_hdmi_set_hd20_21_ctrl_pixel_mode(unsigned char nport, unsigned char pixel_mode);
void m8p_lib_hdmi_dsc_clk_div_sel_by_pixel_mode(unsigned char nport, unsigned char pixel_mode);
unsigned char m8p_lib_hdmi_get_fw_debug_bit(unsigned char nport, unsigned int debug_bit);
void m8p_lib_hdmi_set_fw_debug_bit(unsigned char nport, unsigned int debug_bit, unsigned char on);
void m8p_lib_hdmi_drop_pkt_enable(unsigned char nport, DROP_PKT_TYPE type, unsigned char enable);
void m8p_lib_hdmi_pkt_to_hdmitx_output_enable(unsigned char nport, unsigned char enable);
void m8p_lib_hdmirx_to_hdmitx_pkt_port_sel(unsigned char port);
void m8p_lib_hdmi_hd20_fec_measure_enable(unsigned char nport, unsigned char enable);
unsigned char m8p_lib_hdmi_hd20_fec_is_measure_completed(unsigned char nport);
void m8p_lib_hdmi_hd20_fec_measure_restart(unsigned char nport);
void m8p_lib_hdmi_hd20_meas_clear_interlace_reg(unsigned char nport);
HDMI_INT_PRO_CHECK_RESULT m8p_lib_hdmi_hd20_meas_get_interlace(unsigned char nport);
unsigned int m8p_lib_hdmi_hd20_calc_hfreq(unsigned char nport);
void m8p_lib_hdmi_hd20_set_polarity_meas_mode(unsigned char nport, HD20_MEASURE_POLARITY_MEAS_MODE mode);
HD20_MEASURE_POLARITY_MEAS_MODE m8p_lib_hdmi_hd20_get_polarity_meas_mode(unsigned char nport);
extern HDMI20_MEASURE_RESULT m8p_lib_hdmi20_hd20_measure(unsigned char nport, MEASURE_TIMING_T *update_timing);
void m8p_lib_hdmi_hd20_clr_measure_last_vs_cnt(unsigned char nport);
void m8p_lib_hdmi_hd20_meas_set_frame_pop_mode(unsigned char nport, unsigned char mode);
unsigned char m8p_lib_hdmi_hd20_meas_get_frame_pop_mode(unsigned char nport);
void m8p_lib_hd20ms_timing_check_h(UINT8 port, MEASURE_TIMING_T *update_timing);
void m8p_lib_hd20ms_timing_check_v(UINT8 port, MEASURE_TIMING_T *update_timing);
void m8p_lib_hd20_measure(unsigned char port, MEASURE_TIMING_T *update_timing);
void m8p_lib_hdmi_output_disable(unsigned char port);
extern void m8p_lib_hdmi_clear_avmute(unsigned char nport);
extern unsigned char m8p_lib_hdmi_get_avmute(unsigned char nport);
extern void m8p_lib_hdmi_clear_hdmi_mode_status(unsigned char nport);
extern unsigned char m8p_lib_hdmi_get_hdmi_mode_status(unsigned char nport);
extern void m8p_lib_hdmi_clear_hdmi_no_clk_in_chg(unsigned char nport);
extern unsigned char m8p_lib_hdmi_get_hdmi_no_clk_in_chg(unsigned char nport);
extern void m8p_lib_hdmi_clear_hdmi_no_clk_in_chg_irq_en(unsigned char nport, unsigned char enable);

#endif

