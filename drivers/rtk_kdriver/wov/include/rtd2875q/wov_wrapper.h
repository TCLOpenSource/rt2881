#ifndef _RTK_WOV_WRAPPER_H_
#define _RTK_WOV_WRAPPER_H_

#include <linux/delay.h>

#include <rtk_kdriver/io.h>

static void __attribute__((unused)) enable_audio_data_path(void)
{
    rtd_outl(0xb8060044, 0x20001); // wov clken=1
    rtd_outl(0xb8060034, 0x40001); // wov reset=1

    //=====[DMIC IO]PinMux=====//

    //=====[AUDIO Clk Setting]=====//
    rtd_part_outl(0xb8006004, 13, 12, 0x2); // sel_ts_ck_source:PLL_AUDIO
    rtd_part_outl(0xb8006004, 15, 14, 0x3); // sel_ts_ck_source:pre_512fs_ts:98M
    rtd_part_outl(0xb8006000, 27, 26, 0x1); // div_pre_512fs_ts:pre_512fs_ts/2

    //=====[DMIC setting]=====//
    rtd_part_outl(0xb8006030, 31, 31, 0x1); // vr_adc_3_en
    rtd_part_outl(0xb8006030, 26, 26, 0x1); // vr_adc_2_en
    rtd_part_outl(0xb8006030, 20, 17, 0x5); // adc2:16khz
    rtd_part_outl(0xb8006030, 16, 13, 0x5); // adc3:16khz
    rtd_part_outl(0xb8006030, 4, 1, 0x3);   // 2.048Mhz
    rtd_part_outl(0xb8006030, 0, 0, 0x1);   // vr_dmic_1_en

    rtd_outl(0xb806064c, 0x80000000); // 1: dmic clk from audio; 0: dmic clk from wov

    //=====[Data Path]=====//
    rtd_part_outl(0xb80065a0, 15, 12, 0x8); // 0x8:dmic0; 0x9:dmic1
    //	rtd_part_outl(0xb80065a0,15,12,0x9); //0x8:dmic0; 0x9:dmic1
    //	rtd_outl(0xb8006610,0x00000280); //src: from FIFO ; 1ch, disable TS_DMA_w
    rtd_outl(0xb8006610, 0x00000080); // src: from FIFO ; 1ch, disable TS_DMA_w

    // Set ch0~1 start address
    // rtd_outl(0xb800661C,0x00400008); //ch0
    // rtd_outl(0xb800661C,0x00410009); //ch1
    rtd_outl(0xb800661C, 0x1a600008); // ch0
    //	rtd_outl(0xb800661C,0x1a610009); //ch0

    // for 15s data ring buffer
    /* 0x3B18 * 64 = 0xEC600 bytes
     * 1s data = 16000*32bit/8 = 0xFA00 bytes
     * 0xEC600 / 0xFA00 = 15
     */
    rtd_outl(0xb8006618, 0x3B180000);
    // rtd_outl(0xb8006618, 0x13880000); // 5s

    rtd_part_outl(0xb8006b00, 23, 17, 0x49); // adc gain
    rtd_part_outl(0xb8006b00, 16, 10, 0x49); // adc gain
    rtd_part_outl(0xb8006b00, 3, 0, 0x5); // adc boost gain
    // rtd_part_outl(0xb8006b00, 9, 8, 0x3); // mute

    rtd_part_outl(0xb8006b04, 23, 17, 0x49); // adc gain
    rtd_part_outl(0xb8006b04, 16, 10, 0x49); // adc gain
    rtd_part_outl(0xb8006b04, 3, 0, 0x5); // adc boost gain
    // rtd_part_outl(0xb8006b04, 9, 8, 0x3); // mute

    rtd_maskl(0xb8006668, 0xFFFFFFFE, 0x00000001); // wDMA_enable
    rtd_maskl(0xb8006610, 0xffffffdf, 0x00000020); // TS Dma_soft_wt_rst =1 reset
    //	rtd_outl(0xb8006610,0x00000290); //[11:9]=01ch, [5]TS Dma_soft_wt_rst =0 normal, [4]Enable_ts_wDMA
    rtd_outl(0xb8006610, 0x00000090); //[11:9]=01ch, [5]TS Dma_soft_wt_rst =0 normal, [4]Enable_ts_wDMA
}

#endif /* _RTK_WOV_WRAPPER_H_ */