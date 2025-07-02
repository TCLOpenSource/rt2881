#include "usb_mac.h"
#include "usb_uvc_api.h"
#include "usb_utility.h"
#include "user_data.h"
#include "usb_uvc_utility.h"

// for uvc
#include <rtk_kdriver/io.h>
#include <rbus/uvc_dma_reg.h>
#include <rbus/uvc_vgip_reg.h>
#include "rtk_usb_device_dbg.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK37);
//PRAGMA_CONSTSEG(BANK37);
#define code

#define  SYS_REG_SYS_SRST2_reg                                                   0xB8000108
#define  SYS_REG_SYS_SRST3_reg                                                   0xB800010C
#define  SYS_REG_SYS_CLKEN1_reg                                                  0xB8000114
#define  SYS_REG_SYS_CLKEN2_reg                                                  0xB8000118
#define  SYS_REG_SYS_SRST6_reg                                                   0xB8000128
#define  SYS_REG_SYS_CLKEN4_reg                                                  0xB8000130
#define  SYS_REG_SYS_CLKEN6_reg                                                  0xB8000138


extern UINT8 u8_uvc_temp;
extern UINT8 g_recv_buf[150];
extern struct dwc3 g_rtk_dwc3;
extern UINT8 ret;
extern UINT8 g_uvc_set_cur_last_control_selector;
extern UINT8 g_last_uvc_error_val;
extern struct DMEM_COPY_TO_SRAM_STRUCT dwc_info;

extern UINT32 u32_uvc_temp;
extern UINT32 u32_uvc_temp_2;
extern UINT32 u32_uvc_temp_3;
extern UINT32 u32_uvc_temp_4;

extern UINT32 uvc_address_header_buffer;
extern UINT32 uvc_address_data_buffer;
extern UINT32 uvc_address_trb_buffer;

extern UINT32 uvc_trb_size;

extern UINT8  usb_uvc_inited;
extern UINT8  usb_uvc_connected;
extern UINT8  usb_uvc_format;
extern UINT8  usb_uvc_frame;
extern UINT16 usb_uvc_width;
extern UINT16 usb_uvc_length;
extern UINT16 usb_uvc_freq;

extern UINT8 usb_uvc_format_real;
extern UINT16 usb_uvc_width_real;
extern UINT16 usb_uvc_length_real;
extern UINT16 usb_uvc_freq_real;

extern UINT16 i;
extern struct dwc3 g_rtk_dwc3;
extern UINT8 only_support_cdc;
extern UINT8 *p_dwc_temp1_u8;
extern UINT8 uvc_temp_data[32];  // 0: use flash data  1:vid 2:vid 3:pid 4:pid
extern UINT32 dwc_temp1;
extern struct usb_device_descriptor g_dev_descriptor;
extern struct DMEM_COPY_TO_SRAM_STRUCT dwc_info;
extern struct SRAM_COPY_TO_DMEM_STRUCT dwc_copy_to_dmem_info;
extern UINT16 total_length;
extern UINT16 uvc_vs_length;

extern UINT8  dwc_u8_DT_from_flash;

extern UINT8 trb_cnt;

extern UINT8 usb_link_state_count;
extern unsigned long long usb_phy_vir_offset;
extern UINT8 *p_dwc_temp1_u8_1;

UINT8 dwc_buf[256];



UINT8 uvc_use_timestamp = 0;

const UINT8 format_mapping_table_utility[] = {_NONE, _YUYV, _MotionJPEG, _NV12, _I420, _RGB24, _RGB32, _P010};

void gen_descriptor_table_uvc_P010(void);
void gen_descriptor_table_uvc_RGB32(void);
void gen_descriptor_table_uvc_RGB24(void);
void gen_descriptor_table_uvc_M420(void);
void gen_descriptor_table_uvc_I420(void);
void gen_descriptor_table_uvc_NV12(void);
void gen_descriptor_table_uvc_MJPEG(void);
void gen_descriptor_table_uvc_YUY2(void);
void usb_uvc_get_active_resolution(UINT32 *width, UINT32 *length);
void rtk_usb_device_enable_interrupt(void);
void rtk_usb_device_disable_interrupt(void);

void usb_uvc_set_header_timestamp(UINT8 with_timestamp)
{
    uvc_use_timestamp = with_timestamp;
}

void calculate_uncomp_last_size_01(void)
{
    //pm_printk(LOGGER_FATAL, "calculate_uncomp_last_size usb_uvc_length=%d usb_uvc_width=%d\n", usb_uvc_length_real, usb_uvc_width_real);
    //pm_printk(LOGGER_FATAL, "calculate_uncomp_last_size dividend=%d divisor=%d\n", u32_uvc_temp_3, u32_uvc_temp_4);

    u32_uvc_temp = ((UINT32)usb_uvc_width_real * (UINT32)usb_uvc_length_real * (UINT32)u32_uvc_temp_3 / (UINT32)u32_uvc_temp_4 / (UINT32)16372);
    u32_uvc_temp = u32_uvc_temp * (UINT32)16372;
    u32_uvc_temp = ( (UINT32)usb_uvc_width_real * (UINT32)usb_uvc_length_real * (UINT32)u32_uvc_temp_3 / (UINT32)u32_uvc_temp_4) - (UINT32)u32_uvc_temp;
    //pm_printk("remain data size = %d\n", u32_uvc_temp);

    u32_uvc_temp_2 = ( u32_uvc_temp + (UINT32)USB_UVC_HEADER_REAL_SIZE ) / uvc_trb_size;
}

void calculate_uncomp_last_size_02(void)
{
    u32_uvc_temp = u32_uvc_temp - (uvc_trb_size - (UINT32)USB_UVC_HEADER_REAL_SIZE ) * u32_uvc_temp_2;
}

void calculate_uncomp_last_size_03(void)
{
    if(u32_uvc_temp_2 <= 1)
        u32_uvc_temp_2 = 0;
    else
        u32_uvc_temp_2 = u32_uvc_temp_2 - 1;

    u32_uvc_temp = u32_uvc_temp - u32_uvc_temp_2*(UINT32)USB_UVC_HEADER_REAL_SIZE;

    pm_printk(LOGGER_FATAL, "calculate_uncomp_last_size = %x\n", u32_uvc_temp);
}

UINT32 calculate_uncomp_last_size(void)
{
    calculate_uncomp_last_size_01();
    calculate_uncomp_last_size_02();
    calculate_uncomp_last_size_03();
    return u32_uvc_temp;
}

UINT32 calculate_wack_num_per_frame(void)
{
    //pm_printk(LOGGER_FATAL, "calculate_wack_num_per_frame usb_uvc_length=%d\n", usb_uvc_length_real);
    //pm_printk(LOGGER_FATAL, "calculate_wack_num_per_frame usb_uvc_width=%d\n", usb_uvc_width_real);
    //pm_printk(LOGGER_FATAL, "calculate_wack_num_per_frame dividend=%d\n", u32_uvc_temp_3);
    //pm_printk(LOGGER_FATAL, "calculate_wack_num_per_frame divisor=%d\n", u32_uvc_temp_4);
    u32_uvc_temp = ((UINT32)usb_uvc_width_real * (UINT32)usb_uvc_length_real * (UINT32)u32_uvc_temp_3 / (UINT32)u32_uvc_temp_4 / (UINT32)16372) * (UINT32)16384 /(UINT32)16;
    u32_uvc_temp = u32_uvc_temp + ((((UINT32)usb_uvc_width_real * (UINT32)usb_uvc_length_real * (UINT32)u32_uvc_temp_3 / (UINT32)u32_uvc_temp_4 ) % (UINT32)16372) + (UINT32)15) / (UINT32)16;
    pm_printk(LOGGER_FATAL, "calculate_wack_num_per_frame = %x\n", (UINT32)u32_uvc_temp);
    return u32_uvc_temp;
}

void common_setting_uvc_clk(void)
{
    u32_uvc_temp = rtd_inl(0xb8000204);
    u32_uvc_temp = u32_uvc_temp & 0xEFFFFFFF;
    rtd_outl(0xb8000204, u32_uvc_temp);

    u32_uvc_temp = rtd_inl(0xb8000230);
    u32_uvc_temp = u32_uvc_temp & 0x0FFFFFFF;
    rtd_outl(0xb8000230, u32_uvc_temp);
}

void common_setting(void)  __banked
{
    UINT32 uvc_active_width, uvc_active_length;
    UINT32 temp;
    // DS control
    rtd_outl(UVC_DMA_UVC_DS_Ctrl_1_reg      , UVC_DMA_UVC_DS_Ctrl_1_ds_image_width(usb_uvc_width_real) |
                                              UVC_DMA_UVC_DS_Ctrl_1_ds_image_height(usb_uvc_length_real));
    // Aspect Ratio Control
    usb_uvc_get_active_resolution(&uvc_active_width, &uvc_active_length);

    rtd_outl(UVC_DMA_UVC_UGLY_DEN_CTRL_reg, UVC_DMA_UVC_UGLY_DEN_CTRL_num_total_input_line(uvc_active_length)|
                                            UVC_DMA_UVC_UGLY_DEN_CTRL_ugly_den_en(1)|
                                            UVC_DMA_UVC_UGLY_DEN_CTRL_add_line_length(usb_uvc_width_real));
    temp = (usb_uvc_length_real - uvc_active_length) /2;
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_LINE_reg, UVC_DMA_UVC_UGLY_DEN_LINE_num_line_bvact(temp)|
                                            UVC_DMA_UVC_UGLY_DEN_LINE_num_line_fvact(temp));
    temp = (usb_uvc_width_real - uvc_active_width) /2 /2;
    rtd_outl(UVC_DMA_UVC_UGLY_DEN_PXL_reg, UVC_DMA_UVC_UGLY_DEN_PXL_num_pxl_bvact(temp)|
                                           UVC_DMA_UVC_UGLY_DEN_PXL_num_pxl_fhact(temp));

    //Double Buffer Control
    rtd_outl(UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_reg, UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_read_sel(1)|
                                                 UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_apply(1)|
                                                 UVC_DMA_UVC_DOUBLE_BUFFER_CTRL_db_en(1));

    // Timer
    temp = 27000*usb_uvc_freq_real/100;
    rtd_outl(UVC_DMA_UVC_TIMER_CTRL_reg, UVC_DMA_UVC_TIMER_CTRL_uvc_timer_num(temp)|
                                         UVC_DMA_UVC_TIMER_CTRL_uvc_timer_usb_rdone(0)|
                                         UVC_DMA_UVC_TIMER_CTRL_uvc_timer_mode(0)|
                                         UVC_DMA_UVC_TIMER_CTRL_uvc_timer_en(1));
    // Interrupt
    rtk_usb_device_enable_interrupt();
}

void uvc_reset(void)
{
}

void disable_UVC_TimingGen(UINT8 is_set_uvc_flag_gen_postpone) __banked
{
}

UINT8 usb_uvc_disconnect(UINT8 ep, UINT8 clean_information) __banked
{
    pm_printk(LOGGER_ERROR, "[UVC] usb_uvc_disconnect=%x\n", (UINT32)ep);

    if(usb_uvc_inited == 0) {
        //pm_printk(LOGGER_ERROR, "[UVC] already init, just return\n");
        return 0;
    }

    if(usb_uvc_connected == 0) {
        //pm_printk(LOGGER_ERROR, "[UVC] already disconenct, just return\n");
        return 0;
    }

    //usb_uvc_in_disable();

#if 0
    rtd_outl(VUC_VUC_WTCTL_RCTL1_reg,     (UINT32)VUC_VUC_WTCTL_RCTL1_wtctl_rptr_update_en(0));
    rtd_outl(VUC_VUC_WTCTL_WCTL_reg,      (UINT32)VUC_VUC_WTCTL_WCTL_wtctl_wptr_update_en(0));
    rtd_outl(VUC_UVC_ARBUS_CTRL1_reg,     (UINT32)VUC_UVC_ARBUS_CTRL1_arbus_en(0));
    rtd_outl(VUC_TRBDMA_DMA_CTRL1,        (UINT32)VUC_TRBDMA_DMA_CTRL1_trbdma_enable(0));
    rtd_outl(VUC_VUC_TRB_GEN_Ctrl_reg,    (UINT32)VUC_VUC_TRB_GEN_Ctrl_trb_check_en(0) + VUC_VUC_TRB_GEN_Ctrl_trb_gen_en(0) );
    rtd_outl(VUC_HEADER_DMA_CTRL1_reg,    (UINT32)VUC_HEADER_DMA_CTRL1_headerdma_enable(0));
    rtd_outl(VUC_VUC_CAP_DMA_WR_Ctrl_reg, VUC_VUC_CAP_DMA_WR_Ctrl_cap_dma_enable(0));
    // Disable Debug Interrupt
    rtd_outl(VUC_VUC_top_Ctrl_reg, VUC_VUC_top_Ctrl_vuc_top_int_en(0));
#endif
    disable_UVC_TimingGen(1);

    uvc_reset();

    if(clean_information) {
        usb_uvc_width  = 0;
        usb_uvc_length = 0;
        usb_uvc_freq   = 0;
        usb_uvc_format = 0;
        usb_uvc_frame  = 0;
        usb_uvc_width_real  = 0;
        usb_uvc_length_real = 0;
        usb_uvc_freq_real   = 0;
        usb_uvc_format_real = 0;
    }

    usb_uvc_connected = 0;
    usb_link_state_count = 0;
    return 0;
}

void usb_uvc_reinit_disconnect(void) __banked
{
}

#ifdef CONFIG_RTK_KDRV_RTICE_AT_COMMAND

extern AT_UVC_TIMING_INFO_T* p_uvc_timing_info;

UINT32 AT_UVC_GetTimingInfo_core(void)
{
    if(usb_uvc_connected == 1) {
        p_uvc_timing_info->weight = usb_uvc_width_real;                           // weight
        p_uvc_timing_info->height = usb_uvc_length_real;                          // high
        p_uvc_timing_info->Freq   = usb_uvc_freq_real;                            // freq
        p_uvc_timing_info->format = format_mapping_table_utility[usb_uvc_format_real];    // format
    } else {
        p_uvc_timing_info->weight = 0;                               // weight
        p_uvc_timing_info->height = 0;                               // high
        p_uvc_timing_info->Freq   = 0;                               // freq
        p_uvc_timing_info->format = format_mapping_table_utility[0]; // format
    }
    return 0;
}

void AT_UVC_GetTimingInfo_cb(void) __banked
{
    //__xdata UINT8 *const cmd = AT_ACQUIRE_R_TPDU();

    // 0. the cmd[0] is 0xa0, means the 0xA0 AT command TPDU.
    /* here to handle the input argument from TPDU cmd.
     * a) please keep cmd[0] constant,
     * b) cmd_index is A0_TPDU_CMD_INDEX(cmd), argument data is A0_TPDU_DATA(cmd)
    */
    //pm_printk(LOGGER_ERROR, "%s AAA size: %d \n", __func__, (UINT32)sizeof(AT_UVC_TIMING_INFO_T));

    // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
    p_uvc_timing_info = (AT_UVC_TIMING_INFO_T *) AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
    if (AT_UVC_GetTimingInfo_core())
    {
        pm_printk(LOGGER_ERROR, "get timing NG\n");
    }
    else
    {
        //pm_printk(LOGGER_ERROR, "already get timing info\n");
    }

    // 2. response the CMD with data SIZE
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(AT_UVC_TIMING_INFO_T)/*data_size*/);

    //pm_printk(LOGGER_ERROR, " CCC \n");

    return;
}

void AT_UVC_GetVideoTimingInfo_cb(void) __banked
{
    //__xdata UINT8 *const cmd = AT_ACQUIRE_R_TPDU();

    // 0. the cmd[0] is 0xa0, means the 0xA0 AT command TPDU.
    /* here to handle the input argument from TPDU cmd.
     * a) please keep cmd[0] constant,
     * b) cmd_index is A0_TPDU_CMD_INDEX(cmd), argument data is A0_TPDU_DATA(cmd)
    */
    //pm_printk(LOGGER_ERROR, "%s AAA size: %d \n", __func__, (UINT32)sizeof(AT_UVC_TIMING_INFO_T));

    // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
    p_uvc_timing_info = (AT_UVC_TIMING_INFO_T *) AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
    if (AT_UVC_GetTimingInfo_core())
    {
        pm_printk(LOGGER_ERROR, "get timing NG\n");
    }
    else
    {
        //pm_printk(LOGGER_ERROR, "already get timing info\n");
        p_uvc_timing_info->Freq = p_uvc_timing_info->Freq;
    }

    // 2. response the CMD with data SIZE
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(AT_UVC_TIMING_INFO_T)/*data_size*/);

    //pm_printk(LOGGER_ERROR, " CCC \n");

    return;
}

void AT_UVC_GetVideoFormatInfo_cb(void) __banked
{
    //__xdata UINT8 *const cmd = AT_ACQUIRE_R_TPDU();

    // 0. the cmd[0] is 0xa0, means the 0xA0 AT command TPDU.
    /* here to handle the input argument from TPDU cmd.
     * a) please keep cmd[0] constant,
     * b) cmd_index is A0_TPDU_CMD_INDEX(cmd), argument data is A0_TPDU_DATA(cmd)
    */
    //pm_printk(LOGGER_ERROR, "%s AAA size: %d \n", __func__, (UINT32)sizeof(AT_UVC_TIMING_INFO_T));

    // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
    p_uvc_timing_info = (AT_UVC_TIMING_INFO_T *) AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());
    if (AT_UVC_GetTimingInfo_core())
    {
        pm_printk(LOGGER_ERROR, "get format NG\n");
    }
    else
    {
        //pm_printk(LOGGER_ERROR, "already get format info\n");
        p_uvc_timing_info->Freq = p_uvc_timing_info->Freq;
    }

    // 2. response the CMD with data SIZE
    at_r_tpdu_send(AT_ACQUIRE_R_TPDU(), sizeof(AT_UVC_TIMING_INFO_T)/*data_size*/);

    //pm_printk(LOGGER_ERROR, " CCC \n");

    return;
}

UINT32 AT_UVC_SetTimingInfo_core(void)
{
#if 0
    struct usb_ctrlrequest ctrl_req;

    ctrl_req.bRequest     = USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE;
    ctrl_req.bRequestType = UVC_SET_CUR;
    ctrl_req.wValue       = 0x0200;
    ctrl_req.wIndex       = 0;
    ctrl_req.wLength      = 64;

    for(u8_uvc_temp = UVC_NONE; u8_uvc_temp<=UVC_MJPEG; u8_uvc_temp++) {
        if( format_mapping_table_utility[u8_uvc_temp] == p_timing_info->format)
            break;
    }

    usb_uvc_format = u8_uvc_temp;
    switch (p_timing_info->weight) {
        case 1920:
            usb_uvc_frame = 0;
            break;
        case 640:
            usb_uvc_frame = 1;
            break;
        default:
            usb_uvc_frame = 0;
    }

    rtk_uvc_stream_interface(&ctrl_req);
#endif
    return 0;
}

void AT_UVC_SetTimingInfo_cb(void) __banked
{
    //__xdata UINT8 *const cmd = AT_ACQUIRE_R_TPDU();

    // 0. the cmd[0] is 0xa0, means the 0xA0 AT command TPDU.
    /* here to handle the input argument from TPDU cmd.
     * a) please keep cmd[0] constant,
     * b) cmd_index is A0_TPDU_CMD_INDEX(cmd), argument data is A0_TPDU_DATA(cmd)
     */
    //pm_printk(LOGGER_ERROR, "%s AAA size: %d \n", __func__, (UINT32)sizeof(AT_UVC_TIMING_INFO_T));

    // 1. take use the container of TPDU as A0 R_TPDU to response the return data.
    p_uvc_timing_info = (AT_UVC_TIMING_INFO_T *) AT_R_TPDU_DATA(AT_ACQUIRE_R_TPDU());

    AT_UVC_SetTimingInfo_core();
    rtice_pure_ack(AT_ACQUIRE_R_TPDU());

    //pm_printk(0, " CCC \n");

    return;
}
#endif

#if 0
void uvc_auto_recovery(void)
{
    u32_uvc_temp_4 = rtd_inl(VUC_TRBDMA_DMA_status2_reg);
    u32_uvc_temp_4 = u32_uvc_temp_4 - 0x30;
    if( u32_uvc_temp_4 < uvc_address_trb_buffer) {
        u32_uvc_temp_4 = uvc_address_trb_buffer + ((16384/uvc_trb_size)+1)*2*0x10;
    }

    rtd_mem_outl(u32_uvc_temp_4 + 0x08, 0x01);
    rtd_mem_outl(u32_uvc_temp_4 + 0x0c, 0x15);
    rtd_mem_outl(u32_uvc_temp_4 + 0x18, 0x01);
    rtd_mem_outl(u32_uvc_temp_4 + 0x1c, 0x11);

    u32_uvc_temp_4 = rtd_inl(0xb8058800 + 0x10 * UVC_EP_NUMBER + 0xc);
    u32_uvc_temp_4 = u32_uvc_temp_4 | 0x400;
    rtd_outl(0xb8058800 + 0x10 * UVC_EP_NUMBER + 0xc, u32_uvc_temp_4);

    pm_printk(0, "run recovery!!!\n");
}
#endif

UINT8 usb_uvc_check_trb_hwo_full(void) __banked
{
    RTK_USB_DEVICE_DBG_SIMPLE( "usb_uvc_check_trb_hwo_full need to implement\n");
    return 0;
}

#define UVC_REF_XTAL_CNT_TABLE_CNT 11

typedef struct UVC_TG_REF_XTAL{
    UINT16 vFreq;
    UINT32 XTAL_CNT;
}UVC_TG_REF_XTAL_STRUCT;

code UVC_TG_REF_XTAL_STRUCT uvc_ref_xtal_cnt_table[] = {
#if 0
    {2397,  0xFFFFF}, // 25.75MHz
    {2400,  0xFFFFF}, // 25.75MHz
    {2500,  0xFFFFF}, // 25.75MHz
    {2997,  0xDBF24},
    {3000,  0xDBBA0},
    {5000,  0x83D60},
    {5994,  0x6DF92},
    {6000,  0x6DDD0},
    {12000, 0x36EE8},
    {14400, 0x2DC6C},
    {24000, 0x1B774},
#else
    {2397,  0xFFFFF}, // 25.75MHz
    {2400,  0xFFFFF}, // 25.75MHz
    {2500,  0xFFFFF}, // 25.75MHz
    {2997,  0xDC385},
    {3000,  0xDC000},
    {5000,  0x84000},
    {5994,  0x6E1C3},
    {6000,  0x6E000},
    {12000, 0x37000},
    {14400, 0x2DD56},
    {24000, 0x1B800},
#endif
};

void usb_uvc_setting_framerate_xtal_cnt(void) __banked
{
#if 0
    if( VUC_VUC_HEADER_Ctrl_get_pts_scr_en(rtd_inl(VUC_VUC_HEADER_Ctrl_reg)) == 0 ) {
        for(u32_uvc_temp_4=0; u32_uvc_temp_4<UVC_REF_XTAL_CNT_TABLE_CNT; u32_uvc_temp_4++) {
            if(usb_uvc_freq_real == uvc_ref_xtal_cnt_table[u32_uvc_temp_4].vFreq) {
                u32_uvc_temp_2 = rtd_inl(PPOVERLAY_DC2HLK_DC2HLK_XTAL_VSYNC_reg);
                u32_uvc_temp_2 = u32_uvc_temp_2 & 0xFFF00000;
                u32_uvc_temp_2 = u32_uvc_temp_2 + uvc_ref_xtal_cnt_table[u32_uvc_temp_4].XTAL_CNT;
                rtd_outl(PPOVERLAY_DC2HLK_DC2HLK_XTAL_VSYNC_reg, u32_uvc_temp_2);
                pm_printk(0, "uvc timing cnt %x\n", u32_uvc_temp_2);
                break;
            }
        }
    }
#endif
}

void usb_uvc_get_frame_info_from_file_table(void)
{
    total_length = 0;
    uvc_vs_length = 0;

    p_dwc_temp1_u8_1 = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
    rtk_dwc3_get_config_file_name_by_speed();

    switch(usb_uvc_format) {
        case 1:
            gen_descriptor_table_uvc_YUY2();
            break;
        case 2:
            gen_descriptor_table_uvc_MJPEG();
            break;
        case 3:
        default:
            gen_descriptor_table_uvc_NV12();
            break;
        case 4:
            gen_descriptor_table_uvc_I420();
            break;
            gen_descriptor_table_uvc_M420();
        case 5:
            gen_descriptor_table_uvc_RGB24();
            break;
        case 6:
            uvc_temp_data[BUFFER_INDEX_ID_VS_UVC_OFFSET_LOW] = total_length & 0xff;
            uvc_temp_data[BUFFER_INDEX_ID_VS_UVC_OFFSET_HI]  = (total_length>>8) & 0xff;
            gen_descriptor_table_uvc_RGB32();
            break;
        case 7:
            gen_descriptor_table_uvc_P010();
            break;
    }

    // load Video Streaming MJPEG Format Type Descriptor
    p_dwc_temp1_u8_1 = (void*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset);
    memcpy(dwc_buf, p_dwc_temp1_u8_1, 0x10);

    // load Video Streaming MJPEG Frame Type Descriptor
    p_dwc_temp1_u8_1 = p_dwc_temp1_u8_1 + dwc_buf[0];

    while(p_dwc_temp1_u8_1 < (UINT8*)(g_rtk_dwc3.ep0_setup_buf_addr + usb_phy_vir_offset + total_length)) {
        memcpy(dwc_buf, p_dwc_temp1_u8_1, 0x10);
#if 0
        pm_printk(0,"VS Uncompressed Frame Type Descriptor \n");
        pm_printk(0, "%d ", (UINT32)dwc_buf[0]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[1]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[2]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[3]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[4]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[5]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[6]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[7]);
        pm_printk(0, "%d ", (UINT32)dwc_buf[8]);
        pm_printk(0, "%d\n", (UINT32)dwc_buf[9]);
#endif
        p_dwc_temp1_u8_1 = p_dwc_temp1_u8_1 + dwc_buf[0];
        if(usb_uvc_frame == dwc_buf[3]) {
            usb_uvc_width  = (dwc_buf[6]<< (UINT32)8) + dwc_buf[5];
            usb_uvc_length = (dwc_buf[8]<< (UINT32)8) + dwc_buf[7];
            pm_printk(0, "usb uvc find frame x=%d y=%d\n", (UINT32)usb_uvc_width, (UINT32)usb_uvc_length);
            break;
        }
    }

}

void usb_uvc_get_frame_info_from_default_table(void)
{
    if(g_rtk_dwc3.speed == USB_SPEED_HIGH) {
        switch(usb_uvc_frame) {
            case 1:
            default:
                usb_uvc_width  = 1280;
                usb_uvc_length = 720;
                break;
            case 2:
                usb_uvc_width  = 640;
                usb_uvc_length = 480;
                break;
            case 3:
                usb_uvc_width  = 720;
                usb_uvc_length = 480;
                break;
            case 4:
                usb_uvc_width  = 720;
                usb_uvc_length = 576;
                break;
            case 5:
                usb_uvc_width  = 1920;
                usb_uvc_length = 1080;
                break;
        }
    } else {
        switch(usb_uvc_frame) {
            case 1:
                usb_uvc_width  = 1280;
                usb_uvc_length = 720;
                break;
            case 2:
                usb_uvc_width  = 1920;
                usb_uvc_length = 1080;
                break;
            case 3:
                usb_uvc_width  = 2560;
                usb_uvc_length = 1440;
                break;
            case 4:
            default:
                usb_uvc_width  = 3840;
                usb_uvc_length = 2160;
                break;
            case 5:
                usb_uvc_width  = 640;
                usb_uvc_length = 480;
                break;
            case 6:
                usb_uvc_width  = 720;
                usb_uvc_length = 480;
                break;
            case 7:
                usb_uvc_width  = 720;
                usb_uvc_length = 576;
                break;
        }
    }
}

UINT8 usb_uvc_check_format_frame_info(void) __banked
{
    rtk_dwc3_check_DT();

    if ( dwc_u8_DT_from_flash == 0) {
        usb_uvc_get_frame_info_from_default_table();
    } else {
        usb_uvc_get_frame_info_from_file_table();
    }

    return 0;
}

void usb_uvc_enable_gating(void) __banked
{
    pm_printk(LOGGER_ERROR, "Enable UVC gating\n");
}

void usb_uvc_get_active_resolution(UINT32 *width, UINT32 *length)
{
    //*width = uvc_info.IPH_ACT_WID;
    //*length = uvc_info.IPV_ACT_LEN;
}

void rtk_usb_device_enable_interrupt(void)
{
    UINT32 temp;
    //VGIP Interrupt
    temp = rtd_inl(UVC_VGIP_UVC_VGIP_LC_reg);
    temp = temp | UVC_VGIP_UVC_VGIP_LC_uvc_vlcen(1) |
                  UVC_VGIP_UVC_VGIP_LC_uvc_vlc_mode(1)|
                  UVC_VGIP_UVC_VGIP_LC_uvc_vln(usb_uvc_length_real/2);
    rtd_outl(UVC_VGIP_UVC_VGIP_LC_reg, temp);

    //Timer Interrupt
    temp = rtd_inl(UVC_DMA_UVC_top_int_en_reg);
    temp = temp | UVC_DMA_UVC_top_int_en_uvc_timer_int_en(1);
    rtd_outl(UVC_DMA_UVC_top_int_en_reg, temp);

    // Wdone Interrupt
    temp = rtd_inl(UVC_DMA_UVC_top_int_en_reg);
    temp = temp | UVC_DMA_UVC_top_int_en_uvc_cap_dma_int_en(1);
    rtd_outl(UVC_DMA_UVC_top_int_en_reg, temp);

    // Top
    temp = rtd_inl(UVC_DMA_UVC_top_Ctrl_reg);
    temp = temp | UVC_DMA_UVC_top_Ctrl_uvc_top_int_en(1);
    rtd_outl(UVC_DMA_UVC_top_Ctrl_reg, temp);
}

void rtk_usb_device_disable_interrupt(void)
{
    UINT32 temp;

    // Top
    temp = rtd_inl(UVC_DMA_UVC_top_Ctrl_reg);
    temp = temp & ~UVC_DMA_UVC_top_Ctrl_uvc_top_int_en_mask; 
    rtd_outl(UVC_DMA_UVC_top_Ctrl_reg, temp);

    //VGIP Interrupt
    temp = rtd_inl(UVC_VGIP_UVC_VGIP_LC_reg);
    temp = temp & ~UVC_VGIP_UVC_VGIP_LC_uvc_vlcen_mask; 
    rtd_outl(UVC_VGIP_UVC_VGIP_LC_reg, temp);

    //Timer Interrupt
    temp = rtd_inl(UVC_DMA_UVC_top_int_en_reg);
    temp = temp & ~UVC_DMA_UVC_top_int_en_uvc_timer_int_en_mask; 
    rtd_outl(UVC_DMA_UVC_top_int_en_reg, temp);

    // Wdone Interrupt
    temp = rtd_inl(UVC_DMA_UVC_top_int_en_reg);
    temp = temp & ~UVC_DMA_UVC_top_int_en_uvc_cap_dma_int_en_mask; 
    rtd_outl(UVC_DMA_UVC_top_int_en_reg, temp);
}

