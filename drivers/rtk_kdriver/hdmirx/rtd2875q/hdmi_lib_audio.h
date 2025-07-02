#ifndef __HDMI_LIB_AUDIO_H_
#define __HDMI_LIB_AUDIO_H_

extern void lib_hdmi_audio_pop_n_cts(unsigned char port);
extern unsigned char lib_hdmi_audio_pop_n_cts_done(unsigned char port);
extern unsigned int lib_hdmi_audio_get_n(unsigned char port);
extern unsigned int lib_hdmi_audio_get_cts(unsigned char port);
extern void lib_hdmi_audio_cts_bound(unsigned char nport, unsigned int clk);
extern void lib_hdmi_audio_n_bound(unsigned char nport, unsigned int freq);
extern unsigned char lib_hdmi_audio_get_channel_mode(unsigned char nport);
extern unsigned char lib_hdmi_audio_is_fifo_overflow(unsigned char port);
extern void lib_hdmi_audio_wclr_fifo_overflow(unsigned char port);
extern unsigned char lib_hdmi_audio_is_output(unsigned char port);
extern void lib_hdmi_audio_output(unsigned char port, unsigned char on);
extern void lib_hdmi_audio_clear_hbr_status(unsigned char nport);
extern unsigned char lib_hdmi_audio_get_hbr_mode(unsigned char nport);
extern void lib_hdmi_audio_dbg_reg_dump(unsigned char nport);
#endif

