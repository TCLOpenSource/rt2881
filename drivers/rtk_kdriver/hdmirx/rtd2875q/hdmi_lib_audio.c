/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file:    hdmi_lib_audio.c
 *
 * author:  qibin_huang@apowertec.com
 * date:2024/01/08
 * version:     1.0
 *
 *============================================================*/

/*========================Header Files============================*/
#include "hdmi_common.h"
#include "hdmi_reg.h"

// hdmi audio
void lib_hdmi_audio_pop_n_cts(unsigned char nport)
{
    hdmi_mask(HDMI_HDMI_ACRCR_reg, ~(HDMI_HDMI_ACRCR_pucnr_mask), (HDMI_HDMI_ACRCR_pucnr_mask));
}

unsigned char lib_hdmi_audio_pop_n_cts_done(unsigned char nport)
{
    return ((hdmi_in(HDMI_HDMI_ACRCR_reg) & (HDMI_HDMI_ACRCR_pucnr_mask)) == 0);
}

unsigned int lib_hdmi_audio_get_n(unsigned char nport)
{
    if(nport >= HDMI_PORT_TOTAL_NUM){
        HDMI_WARN("[%s][p%d] wrong input port \n", __func__, nport);
        return 0;
    }

    return HDMI_HDMI_ACRSR1_get_n(hdmi_in(HDMI_HDMI_ACRSR1_reg));
}

unsigned int lib_hdmi_audio_get_cts(unsigned char nport)
{
    if(nport >= HDMI_PORT_TOTAL_NUM){
        HDMI_WARN("[%s][p%d] wrong input port \n", __func__, nport);
        return 0;
    }

    return HDMI_HDMI_ACRSR0_get_cts(hdmi_in(HDMI_HDMI_ACRSR0_reg));
}


void lib_hdmi_audio_cts_bound(unsigned char nport, unsigned int clk)
{
    unsigned long cts_up, cts_low;

    if (clk > 5600) { //6G timing
        cts_up = 0xfffff ;//990000;
        cts_low = 445500 * 7 / 8 ;
    }
    else if (clk > 2750) {   //   3G timing
        cts_up = 421875 * 9 / 8 ; //990000;
        cts_low = 222750 * 7 / 8 ;
    }
    else if (clk > 1380) {   //148M
        cts_up = 421875 * 9 / 8  ; //990000;
        cts_low = 140625 * 7 / 8 ;
    }
    else if (clk > 660) {
        cts_up = 234375 * 9 / 8  ; //990000;
        cts_low = 74250 * 7 / 8 ;
    }
    else {
        cts_up = 60060 * 9 / 8  ; //990000;
        cts_low = 25200 * 7 / 8 ;
    }
    hdmi_mask(HDMI_AUDIO_CTS_UP_BOUND_reg, ~(HDMI_AUDIO_CTS_UP_BOUND_cts_up_bound_mask), HDMI_AUDIO_CTS_UP_BOUND_cts_up_bound(cts_up));
    hdmi_mask(HDMI_AUDIO_CTS_LOW_BOUND_reg, ~HDMI_AUDIO_CTS_LOW_BOUND_cts_low_bound_mask, HDMI_AUDIO_CTS_LOW_BOUND_cts_low_bound(cts_low));   //CTS low boundary set 20000
}


void lib_hdmi_audio_n_bound(unsigned char nport, unsigned int freq)
{
    int N_up, N_low;

    if (freq > 190000) { //192k
        N_up = 46592 * 9 / 8 ; //;
        N_low = 20480 * 7 / 8 ;
    }
    else if (freq > 170000) {  //   176.4
        N_up = 71344 * 9 / 8 ; //990000;
        N_low = 17836 * 7 / 8 ;
    }
    else if (freq > 80000) {  //   96 k 88.2k
        N_up = 35672 * 9 / 8  ; //990000;
        N_low = 8918 * 7 / 8 ;
    }
    else {   //32k  44.1k  48k
        N_up = 17836 * 9 / 8  ; //990000;
        N_low = 3072 * 7 / 8 ;
    }
    hdmi_mask(HDMI_AUDIO_N_UP_BOUND_reg, ~(HDMI_AUDIO_N_UP_BOUND_n_up_bound_mask), HDMI_AUDIO_N_UP_BOUND_n_up_bound(N_up));
    hdmi_mask(HDMI_AUDIO_N_LOW_BOUND_reg, ~HDMI_AUDIO_N_LOW_BOUND_n_low_bound_mask, HDMI_AUDIO_N_LOW_BOUND_n_low_bound(N_low));   //CTS low boundary set 20000
    AUDIO_FLOW_PRINTF("Port:%d, freq = %d Audio_N_Bound H = %d , L =%d\n", nport, freq, N_up, N_low);
}


unsigned char lib_hdmi_audio_is_fifo_overflow(unsigned char nport)
{
    return HDMI_HDMI_AUDSR_get_aud_fifof(hdmi_in(HDMI_HDMI_AUDSR_reg));
}

unsigned char lib_hdmi_audio_get_channel_mode(unsigned char nport)
{
    return HDMI_HDMI_AUDSR_get_aud_ch_mode(hdmi_in(HDMI_HDMI_AUDSR_reg));
}

void lib_hdmi_audio_wclr_fifo_overflow(unsigned char nport)
{
    hdmi_out(HDMI_HDMI_AUDSR_reg, HDMI_HDMI_AUDSR_aud_fifof_mask);
}


unsigned char lib_hdmi_audio_is_output(unsigned char nport)
{
    return HDMI_HDMI_AUDCR_get_aud_en(hdmi_in(HDMI_HDMI_AUDCR_reg));
}


void lib_hdmi_audio_output(unsigned char nport, unsigned char on)
{
    if (on) {
        if (!lib_hdmi_audio_is_output(nport)) {
            hdmi_mask(HDMI_HDMI_AUDCR_reg, ~HDMI_HDMI_AUDCR_aud_en_mask, HDMI_HDMI_AUDCR_aud_en(1));
            hdmi_mask(HDMI_HDMI_AUDCR_reg, ~HDMI_HDMI_AUDCR_flat_en_mask, HDMI_HDMI_AUDCR_flat_en(1));

            hdmi_out(HDMI_HDMI_AUDSR_reg, HDMI_HDMI_AUDSR_aud_fifof_mask);  // clear audio fifo over flow status
            AUDIO_FLOW_PRINTF("Port:%d, on=%d, Audio output start\n", nport, on);
        }
    }
    else {
        if (lib_hdmi_audio_is_output(nport)) {
            hdmi_mask(HDMI_HDMI_AUDCR_reg, ~HDMI_HDMI_AUDCR_aud_en_mask, HDMI_HDMI_AUDCR_aud_en(0));
            hdmi_mask(HDMI_HDMI_AUDCR_reg, ~HDMI_HDMI_AUDCR_flat_en_mask, HDMI_HDMI_AUDCR_flat_en(0));
            hdmi_out(HDMI_HDMI_AUDSR_reg, HDMI_HDMI_AUDSR_aud_fifof_mask);  // clear audio fifo over flow status
            AUDIO_FLOW_PRINTF("Port:%d, on=%d,  Audio output stopped\n", nport, on);
        }
    }
}

void lib_hdmi_audio_clear_hbr_status(unsigned char nport)
{
    hdmi_out(HDMI_High_Bit_Rate_Audio_Packet_reg, HDMI_High_Bit_Rate_Audio_Packet_hbr_audio_mode_mask);
}


unsigned char lib_hdmi_audio_get_hbr_mode(unsigned char nport)
{
    return HDMI_High_Bit_Rate_Audio_Packet_get_hbr_audio_mode(hdmi_in(HDMI_High_Bit_Rate_Audio_Packet_reg));
}


void lib_hdmi_audio_dbg_reg_dump(unsigned char nport)
{
    HDMI_EMG("###################Audio Register Start ###################\n");

    HDMI_EMG("ASR0=%x ASR1=%x \n", hdmi_in(HDMI_HDMI_ASR0_reg), hdmi_in(HDMI_HDMI_ASR1_reg));
    HDMI_EMG("ACRCR=%x ACRSR0=%x ACRSR1=%x \n", hdmi_in(HDMI_HDMI_ACRCR_reg), hdmi_in(HDMI_HDMI_ACRSR0_reg), hdmi_in(HDMI_HDMI_ACRSR1_reg));
    HDMI_EMG("CTS_UP_BOUND=%x  CTS_LOW_BOUND=%x \n", hdmi_in(HDMI_AUDIO_CTS_UP_BOUND_reg), hdmi_in(HDMI_AUDIO_CTS_LOW_BOUND_reg));
    HDMI_EMG("N_UP_BOUND=%x  N_LOW_BOUND=%x \n", hdmi_in(HDMI_AUDIO_N_UP_BOUND_reg), hdmi_in(HDMI_AUDIO_N_LOW_BOUND_reg));
    HDMI_EMG("INTCR=%x SR_CHG=%x \n", hdmi_in(HDMI_HDMI_INTCR_reg), hdmi_in(HDMI_Audio_Sample_Rate_Change_IRQ_reg));

    HDMI_EMG("###################Audio Register End###################\n");
}

