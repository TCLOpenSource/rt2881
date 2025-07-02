#include <linux/string.h>
//#include "rtk_kdriver/rtd_types.h"
#include "mach.h"
#include "usb_mac.h"
#include "msg_q_function_api.h"
#include "usb_uac_ctrl.h"
#include "usb_uac_api.h"
//#include <mach/rtk_platform.h>
#include "rtk_io.h"
#include "usb_buf_mgr.h"
#include "usb_utility_config_bank37.h"
//#include <rbus/auc_in_reg.h>
//#include <rbus/auc_out1_reg.h>
//#include <rbus/auc_out2_reg.h>
#include <rtk_kdriver/io.h>
#include "rtk_usb_device_dbg.h"

//#include "rtk_kdriver/audio_sw/rtk_audio_sw.h"

//#pragma nogcse
//PRAGMA_CODESEG(BANK37);
//PRAGMA_CONSTSEG(BANK37);

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define  SYS_REG_SYS_CLKEN1_reg                                                  0xB8000114
#define  SYS_REG_SYS_CLKEN2_reg                                                  0xB8000118

#define DWC3_DEPCMD(n)    (0x070c + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define USB_UAC_IN0_ARBUS_CMD_ADDR  DWC3_DEPCMD(5)
#define USB_UAC_OUT0_ARBUS_CMD_ADDR DWC3_DEPCMD(3)
#define USB_UAC_OUT1_ARBUS_CMD_ADDR DWC3_DEPCMD(4)

#define UAC_IN_MAX_DATA_BUFFER_SIZE (uac_in_driver_setting_table[_48K_24B].dma_buffer_size * uac_in_driver_setting_table[_48K_24B].in_trb_cnt)
//#define UAC_IN_MAX_DATA_BUFFER_SIZE  11760

#define UAC_IN_MAX_TRB_BUFFER_SIZE ((uac_in_driver_setting_table[_48K_24B].in_trb_cnt + USB_UAC_TRB_LINK_CNT) * USB_UAC_TRB_SIZE)
//#define UAC_IN_MAX_TRB_BUFFER_SIZE  656

#define UAC_OUT_MAX_DATA_BUFFER_SIZE (uac_in_driver_setting_table[_192K_16B].dma_buffer_size * uac_in_driver_setting_table[_192K_16B].out_trb_cnt)
//#define UAC_OUT_MAX_DATA_BUFFER_SIZE 3088

#define UAC_OUT_MAX_TRB_BUFFER_SIZE ((uac_in_driver_setting_table[_48K_24B].out_trb_cnt + USB_UAC_TRB_LINK_CNT) * USB_UAC_TRB_SIZE)
//#define UAC_OUT_MAX_TRB_BUFFER_SIZE 144

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern UINT32 dwc_temp1;
struct cntrl_range_lay3 r;
struct cntrl_cur_lay3 c;
extern UINT32 reg;

UINT32 uac_device_to_host_buffer_addr = 0;
UINT8 *uac_device_to_host_buffer_addr_vir_non_cache = 0;
UINT32 uac_device_to_host_trb_addr    = 0;
UINT8 *uac_device_to_host_trb_addr_vir_non_cache    = 0;
UINT8  uac_device_to_host_trb_cnt     = 8;

UINT32 uac_device_to_host_buffer_addr_2 = 0;
UINT8 *uac_device_to_host_buffer_addr_2_vir_non_cache = 0;
UINT32 uac_device_to_host_trb_addr_2    = 0;
UINT8 *uac_device_to_host_trb_addr_2_vir_non_cache    = 0;
UINT8  uac_device_to_host_trb_cnt_2     = 8;

UINT32 uac_host_to_device_buffer_addr = 0;
UINT8 *uac_host_to_device_buffer_addr_vir_non_cache = 0;
UINT32 uac_host_to_device_trb_addr    = 0;
UINT8 *uac_host_to_device_trb_addr_vir_non_cache    = 0;
UINT8  uac_host_to_device_trb_cnt     = 10;

UINT32 uac_mps_size = 0;
UINT8 u8j;
UINT32 uac_temp1;
UINT32 uac_temp2;
UINT32 g_UAC_IN_base_mps_size;
UINT32 g_UAC_OUT0_base_mps_size;
UINT32 g_UAC_OUT1_base_mps_size;

extern UAC_TYPE_SETTING UAC_IN_freq_type;
extern UAC_TYPE_SETTING UAC_OUT0_freq_type;
extern UAC_TYPE_SETTING UAC_OUT1_freq_type;

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
#if 0
typedef struct UAC_IN_DRIVER_SETTING{
    UAC_TYPE_SETTING     format;
    UINT16               dma_buffer_size;
    UINT16               out_trb_cnt;
    UINT16               out_bInterval;
    UINT16               in_trb_cnt;
    UINT16               in_bInterval;
    UINT16               dma_buf_step;    //48K:304  96K:592  192K:592
    UINT8                output_format;   //0: 24bit 2: 16bit
    UINT8                sample_rate;     //0: 48K 1:96K 2: 192K
}UAC_IN_DRIVER_SETTING;

UAC_IN_DRIVER_SETTING uac_in_driver_setting_table[] ={
        //format, //dma_buffer_size  //out trb_cnt  bInterval  //input trb_cnt  bInterval  //dma_buf_step  //output_format  //sample_rate
    { _UAC_NONE_,                 0,             0,         0,               0,         0,              0,               0,             0},
    {  _192K_24B,       (192+1)*6/2,             4,         4,              20,         8,            592,               0,             2},
    {   _96K_24B,          (96+1)*6,             4,         8,              20,         8,            592,               0,             1},
    {   _48K_24B,          (48+1)*6,             8,         8,              40,         8,            304,               0,             0},
    {  _192K_16B,          (48+1)*6,             4,         4,              10,         8,            592,               1,             2},
    {   _96K_16B,          (48+1)*6,             4,         8,              20,         8,            592,               1,             1},
    {   _48K_16B,          (48+1)*6,             8,         8,              40,         8,            304,               1,             0},
};
#endif

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************

void init_uac_host_to_device_sram(UAC_TYPE_SETTING uac_in_freq_type)
{
    UINT8 uac_temp;
    UINT8 *uac_temp1;
    UINT8 *uac_temp2;

    RTK_USB_DEVICE_DBG_SIMPLE( "init_uac_host_to_device_sram Ln%d\n", __LINE__);

    //for(uac_temp1= uac_host_to_device_buffer_addr; uac_temp1<uac_host_to_device_buffer_addr + 0x1c000; uac_temp1=uac_temp1+4) {
    //    rtd_mem_outl(uac_temp1, 0x00000000);
    //}
    if(usb_sw_cfg_GetUAC_DownStream_Enable_BANK37()==_FUNCTION_DISABLE)
        return;

    if(uac_host_to_device_trb_addr == 0)
        return;

    for(uac_temp1= uac_host_to_device_trb_addr_vir_non_cache; uac_temp1< (uac_host_to_device_trb_addr_vir_non_cache + uac_host_to_device_trb_cnt*(UINT32)0x10); uac_temp1=uac_temp1+4) {
        rtd_mem_outl(uac_temp1, 0x00000000);
    }

    uac_temp2 = uac_host_to_device_trb_addr_vir_non_cache;
    uac_mps_size = uac_host_to_device_buffer_addr;

    if(uac_in_freq_type == _48K_16B) {
        g_UAC_IN_base_mps_size = UAC_MPS_SIZE;
    } else if(uac_in_freq_type == _96K_16B) {
        g_UAC_IN_base_mps_size = UAC_MPS_SIZE_96K16B;
    } else if(uac_in_freq_type == _192K_16B) {
        g_UAC_IN_base_mps_size = UAC_MPS_SIZE_192K16B;
    }

    for(uac_temp= 0; uac_temp<uac_host_to_device_trb_cnt; uac_temp++) {
        rtd_mem_outl(uac_temp2 +  0, uac_mps_size);
        rtd_mem_outl(uac_temp2 +  4, 0x00000000);
        rtd_mem_outl(uac_temp2 +  8, g_UAC_IN_base_mps_size);
        rtd_mem_outl(uac_temp2 + 12, (DWC3_TRB_CTRL_HWO | DWC3_TRBCTL_ISOCHRONOUS_FIRST | DWC3_TRB_CTRL_CSP /*| DWC3_TRB_CTRL_ISP_IMI*/));
        uac_temp2 = uac_temp2 + 0x10;
        uac_mps_size = uac_mps_size + g_UAC_IN_base_mps_size;
    }

    RTK_USB_DEVICE_DBG_SIMPLE( "init_uac_host_to_device_sram Ln%d\n", __LINE__);
    // fill link trb
    uac_temp = uac_host_to_device_trb_cnt* (UINT32)16 + 0;
    rtd_mem_outl(uac_host_to_device_trb_addr_vir_non_cache + uac_temp + 0, uac_host_to_device_trb_addr);
    rtd_mem_outl(uac_host_to_device_trb_addr_vir_non_cache + uac_temp + 4, 0x00000000);
    rtd_mem_outl(uac_host_to_device_trb_addr_vir_non_cache + uac_temp + 8, 0x00000000);
    rtd_mem_outl(uac_host_to_device_trb_addr_vir_non_cache + uac_temp + 12, DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);
    RTK_USB_DEVICE_DBG_SIMPLE( "init_uac_host_to_device_sram Ln%d\n", __LINE__);
}

void init_uac_device_to_host_sram(UAC_TYPE_SETTING uac_out0_freq_type)
{
    UINT8 *uac_temp1;
    //for(uac_temp1= uac_device_to_host_buffer_addr; uac_temp1<uac_device_to_host_trb_addr; uac_temp1=uac_temp1+4) {
    //    rtd_mem_outl(uac_temp1, 0x00000000);
    //}

    uac_temp2 = uac_device_to_host_buffer_addr;
    uac_temp1 = uac_device_to_host_trb_addr_vir_non_cache;

    if(uac_out0_freq_type == _48K_16B) {
        g_UAC_OUT0_base_mps_size = UAC_MPS_SIZE;
    } else if(uac_out0_freq_type == _96K_16B) {
        g_UAC_OUT0_base_mps_size = UAC_MPS_SIZE_96K16B;
    } else if(uac_out0_freq_type == _192K_16B) {
        g_UAC_OUT0_base_mps_size = UAC_MPS_SIZE_192K16B;
    }

    // normal use
    for(u8j= 0; u8j<uac_device_to_host_trb_cnt; u8j++) {
        rtd_mem_outl(uac_temp1 + (UINT32)0x0, uac_temp2);
        rtd_mem_outl(uac_temp1 + (UINT32)0x4, 0x00000000);
        rtd_mem_outl(uac_temp1 + (UINT32)0x8, 0x00000000);
        rtd_mem_outl(uac_temp1 + (UINT32)0xc, (DWC3_TRBCTL_ISOCHRONOUS_FIRST | DWC3_TRB_CTRL_CSP));
        uac_temp1 = uac_temp1 + (UINT32)0x10;
        uac_temp2 = uac_temp2 + (UINT32)g_UAC_OUT0_base_mps_size;
    }

    // fill dnummy trb
    uac_temp1 = uac_device_to_host_trb_addr_vir_non_cache - (UINT32)((uac_device_to_host_trb_cnt/2)*0x10);
    for(u8j = 0; u8j < (uac_device_to_host_trb_cnt/2); u8j++) {
        rtd_mem_outl(uac_temp1 + (UINT32)0x0, uac_temp2);
        rtd_mem_outl(uac_temp1 + (UINT32)0x4, 0x00000000);
        rtd_mem_outl(uac_temp1 + (UINT32)0x8, g_UAC_OUT0_base_mps_size);
        rtd_mem_outl(uac_temp1 + (UINT32)0xc, (DWC3_TRBCTL_ISOCHRONOUS_FIRST | DWC3_TRB_CTRL_CSP | DWC3_TRB_CTRL_HWO /*| DWC3_TRB_CTRL_ISP_IMI*/));
        uac_temp1 = uac_temp1  + (UINT32)0x10;
    }

    // fill link trb
    rtd_mem_outl(uac_device_to_host_trb_addr_vir_non_cache + uac_device_to_host_trb_cnt* (UINT32)16 + 0, uac_device_to_host_trb_addr);
    rtd_mem_outl(uac_device_to_host_trb_addr_vir_non_cache + uac_device_to_host_trb_cnt* (UINT32)16 + 4, 0x00000000);
    rtd_mem_outl(uac_device_to_host_trb_addr_vir_non_cache + uac_device_to_host_trb_cnt* (UINT32)16 + 8, 0x00000000);
    rtd_mem_outl(uac_device_to_host_trb_addr_vir_non_cache + uac_device_to_host_trb_cnt* (UINT32)16 + 12, DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);
}

void init_uac_device_to_host_sram_2(UAC_TYPE_SETTING uac_out2_freq_type)
{
    UINT8 *uac_temp1;

    //for(uac_temp1= uac_device_to_host_buffer_addr_2; uac_temp1<uac_device_to_host_trb_addr_2; uac_temp1=uac_temp1+4) {
    //    rtd_mem_outl(uac_temp1, 0x00000000);
    //}

    uac_temp2 = uac_device_to_host_buffer_addr_2;
    uac_temp1 = uac_device_to_host_trb_addr_2_vir_non_cache;

    if(uac_out2_freq_type == _48K_16B) {
        g_UAC_OUT1_base_mps_size = UAC_MPS_SIZE;
    } else if(uac_out2_freq_type == _96K_16B) {
        g_UAC_OUT1_base_mps_size = UAC_MPS_SIZE_96K16B;
    } else if(uac_out2_freq_type == _192K_16B) {
        g_UAC_OUT1_base_mps_size = UAC_MPS_SIZE_192K16B;
    }

    // normal use
    for(u8j= 0; u8j<uac_device_to_host_trb_cnt_2; u8j++) {
        rtd_mem_outl(uac_temp1 + (UINT32)0x0, uac_temp2);
        rtd_mem_outl(uac_temp1 + (UINT32)0x4, 0x00000000);
        rtd_mem_outl(uac_temp1 + (UINT32)0x8, 0x00000000);
        rtd_mem_outl(uac_temp1 + (UINT32)0xc, (DWC3_TRBCTL_ISOCHRONOUS_FIRST | DWC3_TRB_CTRL_CSP));
        uac_temp1 = uac_temp1 + (UINT32)0x10;
        uac_temp2 = uac_temp2 + (UINT32)g_UAC_OUT1_base_mps_size;
    }

    // fill dnummy trb
    uac_temp1 = uac_device_to_host_trb_addr_2_vir_non_cache - (UINT32)((uac_device_to_host_trb_cnt_2/2)*0x10);
    for(u8j = 0; u8j < (uac_device_to_host_trb_cnt_2/2); u8j++) {
        rtd_mem_outl(uac_temp1 + (UINT32)0x0, uac_temp2);
        rtd_mem_outl(uac_temp1 + (UINT32)0x4, 0x00000000);
        rtd_mem_outl(uac_temp1 + (UINT32)0x8, g_UAC_OUT1_base_mps_size);
        rtd_mem_outl(uac_temp1 + (UINT32)0xc, (DWC3_TRBCTL_ISOCHRONOUS_FIRST | DWC3_TRB_CTRL_CSP | DWC3_TRB_CTRL_HWO /*| DWC3_TRB_CTRL_ISP_IMI*/));
        uac_temp1 = uac_temp1  + (UINT32)0x10;
    }

    // fill link trb
    rtd_mem_outl(uac_device_to_host_trb_addr_2_vir_non_cache + uac_device_to_host_trb_cnt_2* (UINT32)16 + 0, uac_device_to_host_trb_addr_2);
    rtd_mem_outl(uac_device_to_host_trb_addr_2_vir_non_cache + uac_device_to_host_trb_cnt_2* (UINT32)16 + 4, 0x00000000);
    rtd_mem_outl(uac_device_to_host_trb_addr_2_vir_non_cache + uac_device_to_host_trb_cnt_2* (UINT32)16 + 8, 0x00000000);
    rtd_mem_outl(uac_device_to_host_trb_addr_2_vir_non_cache + uac_device_to_host_trb_cnt_2* (UINT32)16 + 12, DWC3_TRBCTL_LINK_TRB | DWC3_TRB_CTRL_HWO);
}

void usb_uac_init_1_1(UINT32 UAC_out0_base_mps_size)
{
    uac_device_to_host_trb_addr = uac_device_to_host_buffer_addr + (UINT32)UAC_out0_base_mps_size * (UINT32)uac_device_to_host_trb_cnt + 0x80;
    RTK_USB_DEVICE_DBG_SIMPLE("usb_uac_init_1_1 uac_device_to_host_trb_addr=%d\n", uac_device_to_host_trb_addr);
    uac_device_to_host_trb_addr = uac_device_to_host_trb_addr + 0xff;
    uac_device_to_host_trb_addr = uac_device_to_host_trb_addr & 0xffffff00;
    uac_device_to_host_trb_addr_vir_non_cache = (uac_device_to_host_trb_addr - uac_device_to_host_buffer_addr) + uac_device_to_host_buffer_addr_vir_non_cache;
}

void usb_uac_init_1_2(void)
{
    uac_device_to_host_buffer_addr_2 = (UINT32)uac_device_to_host_trb_addr + (UINT32)USB_TRB_SIZE * (UINT32)(uac_device_to_host_trb_cnt + 1);
    RTK_USB_DEVICE_DBG_SIMPLE("usb_uac_init_1_2 uac_device_to_host_buffer_addr_2=%d\n", uac_device_to_host_buffer_addr_2);
    uac_device_to_host_buffer_addr_2 = uac_device_to_host_buffer_addr_2 + 0xff;
    uac_device_to_host_buffer_addr_2 = uac_device_to_host_buffer_addr_2 & 0xffffff00;
    uac_device_to_host_buffer_addr_2_vir_non_cache = (uac_device_to_host_buffer_addr_2 - uac_device_to_host_buffer_addr) + uac_device_to_host_buffer_addr_vir_non_cache;
}

void usb_uac_init_2_1(UINT32 UAC_out0_base_mps_size)
{
    uac_device_to_host_trb_addr_2 = uac_device_to_host_buffer_addr_2 + (UINT32)UAC_out0_base_mps_size * (UINT32)uac_device_to_host_trb_cnt + 0x80;
    RTK_USB_DEVICE_DBG_SIMPLE("usb_uac_init_2_1 uac_device_to_host_trb_addr_2=%d\n", uac_device_to_host_trb_addr_2);
    uac_device_to_host_trb_addr_2 = uac_device_to_host_trb_addr_2 + 0xff;
    uac_device_to_host_trb_addr_2 = uac_device_to_host_trb_addr_2 & 0xffffff00;
    uac_device_to_host_trb_addr_2_vir_non_cache = (uac_device_to_host_trb_addr_2 - uac_device_to_host_buffer_addr) + uac_device_to_host_buffer_addr_vir_non_cache;
}

void usb_uac_init_2_2(void)
{
        uac_host_to_device_buffer_addr = (UINT32)uac_device_to_host_trb_addr_2 + (UINT32)USB_TRB_SIZE * (UINT32)(uac_device_to_host_trb_cnt + 1);
        RTK_USB_DEVICE_DBG_SIMPLE("usb_uac_init_2_2 uac_host_to_device_buffer_addr=%d\n", uac_host_to_device_buffer_addr);
        uac_host_to_device_buffer_addr_vir_non_cache = (uac_host_to_device_buffer_addr - uac_device_to_host_buffer_addr) + uac_device_to_host_buffer_addr_vir_non_cache;
}

UINT8 usb_uac_out0_sram_init(UAC_TYPE_SETTING uac_out_freq_type) __banked
{

#ifdef UAC_AVSYNC_SUPPORT
    UINT32 temp_AVSYNC_addr = 0;
#if UAC_OUT2_CONFIG
    // AVSYNC2
    usb_request_usbbuf(UAC_AVSYNC_DDR_BUF, &uac_temp1, &uac_temp1, &uac_temp2);
    uac_temp1 = uac_temp1 + 0x80000;

    // clear dummy register
    for(temp_AVSYNC_addr = (UINT32)uac_temp1; temp_AVSYNC_addr < (UINT32)(uac_temp1 + 0x100); temp_AVSYNC_addr = temp_AVSYNC_addr + 4) {
        rtd_mem_outl((UINT32)temp_AVSYNC_addr, 0x00000000);
    }

    rtd_outl(AUC_OUT2_AUC_OUT2_AVS_DMA_CTRL1_reg, uac_temp1 + 0x80000);
    rtd_outl(AUC_OUT2_AUC_OUT2_AVS_DMA_CTRL2_reg, uac_temp1);              // base ptr
    rtd_outl(AUC_OUT2_AUC_OUT2_AVS_WR_CTRL2_reg, uac_temp1 + 0x100);       // write ptr
    rtd_outl(AUC_OUT2_AUC_OUT2_AVS_RD_CTRL2_reg, uac_temp1);               // read ptr
#endif
#endif

    init_uac_device_to_host_sram(uac_out_freq_type);
#if 0
#ifdef UAC_OUT__48K_16BIT
    {   //UAC OUT - 48K 16bit
#if UAC_OUT2_CONFIG
        rtd_outl(AUC_OUT2_AUC_OUT2_TRB_RD_CTRL1_reg, 0x00000500);
        rtd_outl(AUC_OUT2_AUC_OUT2_TRB_WR_CTRL1_reg, 0x00000500);
        rtd_outl(AUC_OUT2_AUC_OUT2_ARBUS_CTRL3_reg, 0x00050407);  //resource
        rtd_outl(AUC_OUT2_AUC_OUT2_ARBUS_CTRL4_reg, 0xb805889c);  //cmd address
        rtd_outl(AUC_OUT2_AUC_OUT2_ARBUS_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_OUT2_AUC_OUT2_WDMA_WR_CTRL1_reg, 0x00000000);
        //rtd_outl(AUC_OUT2_AUC_OUT2_CTRL1_reg, 0x08080000); // trb_num=8 48K 8sof
        //rtd_outl(AUC_OUT2_AUC_OUT2_CTRL1_reg, 0x10080000); // trb_num=16 48K 8sof
        rtd_outl(AUC_OUT2_AUC_OUT2_CTRL1_reg, AUC_OUT2_AUC_OUT2_CTRL1_trb_num(uac_device_to_host_trb_cnt_2)|AUC_OUT2_AUC_OUT2_CTRL1_si_sof_num(8));

        if(uac_out_freq_type == _48K_16B) {
            rtd_outl(AUC_OUT2_AUC_OUT2_WDMA_WR_CTRL1_reg, 0x00d00000); // 208byte 48k 
            rtd_outl(AUC_OUT2_AUC_OUT2_TRB_WR_HW_SET1_reg, 0x000000d0); // 208byte 48k
        }
        else if (uac_out_freq_type == _96K_16B) {
            rtd_outl(AUC_OUT2_AUC_OUT2_WDMA_WR_CTRL1_reg, 0x01900000); // 384-byte 96K 16-bit
            rtd_outl(AUC_OUT2_AUC_OUT2_TRB_WR_HW_SET1_reg, 0x00000190); // 384-byte 96K 16-bit
        }
        else if (uac_out_freq_type == _192K_16B) {
            rtd_outl(AUC_OUT2_AUC_OUT2_WDMA_WR_CTRL1_reg, 0x03100000); // 768-byte 192K 16-bit
            rtd_outl(AUC_OUT2_AUC_OUT2_TRB_WR_HW_SET1_reg, 0x00000310); // 768-byte 192K 16-bit
        }
        rtd_outl(AUC_OUT2_AUC_OUT2_TRB_WR_HW_SET2_reg, 0x00000469); //IMI ISO first  CSP HWO 
        rtd_outl(AUC_OUT2_AUC_OUT2_TRB_WR_CTRL2_reg, uac_device_to_host_trb_addr); // TRB Start Address
        rtd_outl(AUC_OUT2_AUC_OUT2_TRB_DMA_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_OUT2_AUC_OUT2_WDMA_CTRL2_reg, uac_device_to_host_buffer_addr);  // WDMA start address
        rtd_outl(AUC_OUT2_AUC_OUT2_WDMA_DMA_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_OUT2_AUC_OUT2_CTRL2_reg, 0x00000102); // 16bit 2ch
        //rtd_outl(AUC_OUT2_AUC_OUT2_CTRL1_reg, 0x08010001); // for audio uac out test
#endif
    }
#endif
#endif
    return 0;
}

UINT8 usb_uac_out1_sram_init(UAC_TYPE_SETTING uac_out_freq_type) __banked
{
#ifdef UAC_AVSYNC_SUPPORT
#if UAC_OUT1_CONFIG
    // AVSYNC1
    usb_request_usbbuf(UAC_AVSYNC_DDR_BUF, &uac_temp1, &uac_temp1, &uac_temp2);
    rtd_outl(AUC_OUT1_AUC_OUT1_AVS_DMA_CTRL1_reg, uac_temp1 + 0x80000);
    rtd_outl(AUC_OUT1_AUC_OUT1_AVS_DMA_CTRL2_reg, uac_temp1);               // base ptr
    rtd_outl(AUC_OUT1_AUC_OUT1_AVS_WR_CTRL2_reg, uac_temp1 + 0x100);       // write ptr
    rtd_outl(AUC_OUT1_AUC_OUT1_AVS_RD_CTRL2_reg, uac_temp1);               // read ptr
#endif
#endif

    init_uac_device_to_host_sram_2(uac_out_freq_type);
#if 0
#if UAC_OUT__48K_16BIT
    {   //UAC OUT - 48K 16bit
#if UAC_OUT1_CONFIG
        rtd_outl(AUC_OUT1_AUC_OUT1_TRB_RD_CTRL1_reg, 0x00000500);
        rtd_outl(AUC_OUT1_AUC_OUT1_TRB_WR_CTRL1_reg, 0x00000500);
        rtd_outl(AUC_OUT1_AUC_OUT1_ARBUS_CTRL3_reg, 0x00040407);  //resource
        rtd_outl(AUC_OUT1_AUC_OUT1_ARBUS_CTRL4_reg, 0xb805887c);  //cmd address
        rtd_outl(AUC_OUT1_AUC_OUT1_ARBUS_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_OUT1_AUC_OUT1_WDMA_WR_CTRL1_reg, 0x00000000);
        //rtd_outl(AUC_OUT1_AUC_OUT1_CTRL1_reg, 0x08080000); // trb_num=8 48K 8sof
        //rtd_outl(AUC_OUT1_AUC_OUT1_CTRL1_reg, 0x10080000); // trb_num=16 48K 8sof
        rtd_outl(AUC_OUT1_AUC_OUT1_CTRL1_reg, AUC_OUT2_AUC_OUT2_CTRL1_trb_num(uac_device_to_host_trb_cnt)|AUC_OUT2_AUC_OUT2_CTRL1_si_sof_num(8));

        if(uac_out_freq_type == _48K_16B) {
            rtd_outl(AUC_OUT1_AUC_OUT1_WDMA_WR_CTRL1_reg, 0x00d00000); // 208byte 48k 
            rtd_outl(AUC_OUT1_AUC_OUT1_TRB_WR_HW_SET1_reg, 0x000000d0); // 208byte 48k
        }
        else if (uac_out_freq_type == _96K_16B) {
            rtd_outl(AUC_OUT1_AUC_OUT1_WDMA_WR_CTRL1_reg, 0x01900000); // 384-byte 96K 16-bit
            rtd_outl(AUC_OUT1_AUC_OUT1_TRB_WR_HW_SET1_reg, 0x00000190); // 384-byte 96K 16-bit
        }
        else if (uac_out_freq_type == _192K_16B) {
            rtd_outl(AUC_OUT1_AUC_OUT1_WDMA_WR_CTRL1_reg, 0x03100000); // 768-byte 192K 16-bit
            rtd_outl(AUC_OUT1_AUC_OUT1_TRB_WR_HW_SET1_reg, 0x00000310); // 768-byte 192K 16-bit
        }
        rtd_outl(AUC_OUT1_AUC_OUT1_TRB_WR_HW_SET2_reg, 0x00000469); //IMI ISO first  CSP HWO 
        rtd_outl(AUC_OUT1_AUC_OUT1_TRB_WR_CTRL2_reg, uac_device_to_host_trb_addr_2); // TRB Start Address
        rtd_outl(AUC_OUT1_AUC_OUT1_TRB_DMA_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_OUT1_AUC_OUT1_WDMA_CTRL2_reg, uac_device_to_host_buffer_addr_2);  // WDMA start address
        rtd_outl(AUC_OUT1_AUC_OUT1_WDMA_DMA_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_OUT1_AUC_OUT1_CTRL2_reg, 0x00000102); // 16bit 2ch
        //rtd_outl(AUC_OUT1_AUC_OUT1_CTRL1_reg, 0x08010001); // for audio uac out test
#endif
    }
#endif
#endif
    return 0;
}

UINT8 usb_uac_in_sram_init(UAC_TYPE_SETTING uac_in_freq_type) __banked
{
    init_uac_host_to_device_sram(uac_in_freq_type);
#if 0
    #if UAC_IN__48K_16BIT
    {   //UAC IN - 48K 16bit
        rtd_outl(AUC_IN_AUC_IN_TRB_RD_CTRL1_reg, 0x00000500); // swap
        rtd_outl(AUC_IN_AUC_IN_TRB_WR_CTRL1_reg, 0x00000500); // swap
        rtd_outl(AUC_IN_AUC_IN_ARBUS_CTRL3_reg, 0x00030407); // resource
        rtd_outl(AUC_IN_AUC_IN_ARBUS_CTRL4_reg, 0x1805886c); // cmd address
        rtd_outl(AUC_IN_AUC_IN_ARBUS_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_IN_AUC_IN_RDMA_RD_CTRL1_reg, 0x00000500);
        uac_temp1 = 0x00080000;
        uac_temp1 = uac_temp1 + (((UINT32)uac_host_to_device_trb_cnt/2)<<8);
        uac_temp1 = uac_temp1 + (((UINT32)uac_host_to_device_trb_cnt)<<24);
        rtd_outl(AUC_IN_AUC_IN_CTRL1_reg, uac_temp1);

        if(uac_in_freq_type == _48K_16B) {
            rtd_outl(AUC_IN_AUC_IN_RDMA_RD_CTRL1_reg, 0x00d00500); // 208-byte 48K 16-bit
            rtd_outl(AUC_IN_AUC_IN_TRB_WR_HW_SET1_reg, 0x000000d0); // 208-byte 48K 16-bit
            rtd_outl(AUC_IN_AUC_IN_CTRL2_reg, 0x0001000a); // 16bit 2ch continue (0xd0 - 192)
        }
        else if (uac_in_freq_type == _96K_16B) {
            rtd_outl(AUC_IN_AUC_IN_RDMA_RD_CTRL1_reg, 0x01900500); // 384-byte 96K 16-bit
            rtd_outl(AUC_IN_AUC_IN_TRB_WR_HW_SET1_reg, 0x00000190); // 384-byte 96K 16-bit
            rtd_outl(AUC_IN_AUC_IN_CTRL2_reg, 0x0001000a); // 16bit 2ch continue (0x190 - 384)
        }
        else if (uac_in_freq_type == _192K_16B) {
            rtd_outl(AUC_IN_AUC_IN_RDMA_RD_CTRL1_reg, 0x03100500); // 768-byte 192K 16-bit
            rtd_outl(AUC_IN_AUC_IN_TRB_WR_HW_SET1_reg, 0x00000310); // 768-byte 192K 16-bit
            rtd_outl(AUC_IN_AUC_IN_CTRL2_reg, 0x0001000a); // 16bit 2ch continue (0x310 - 768)
        }
        rtd_outl(AUC_IN_AUC_IN_TRB_WR_HW_SET2_reg, 0x00000069); //IMI ISO first  CSP HWO
        rtd_outl(AUC_IN_AUC_IN_TRB_WR_CTRL2_reg, uac_host_to_device_trb_addr); // TRB Start Address
        rtd_outl(AUC_IN_AUC_IN_TRB_DMA_CTRL1_reg, 0x00000001);
        rtd_outl(AUC_IN_AUC_IN_RDMA_CTRL2_reg, uac_host_to_device_buffer_addr);  // RDMA start address
        rtd_outl(AUC_IN_AUC_IN_RDMA_DMA_CTRL1_reg, 0x00000001);
    }
#endif
#endif
        return 0;
}

UINT8 usb_uac_alloc(UAC_TYPE_SETTING uac_in_freq_type, UAC_TYPE_SETTING uac_out0_freq_type) __banked
{
    UINT32 UAC_in_base_mps_size;
    UINT32 UAC_out0_base_mps_size;
    UINT8 *temp_usb_addr;

    // Allocate SRAM address (uac_device_to_host_buffer_addr = 0x1a300000)
    //usb_request_usbbuf(UAC_DDR_BUF, &uac_device_to_host_buffer_addr, &uac_device_to_host_buffer_addr, &uac_temp1);
    //if(uac_device_to_host_buffer_addr == 0)
    usb_request_usbbuf(UAC_SRAM_BUF, &uac_device_to_host_buffer_addr, &uac_device_to_host_buffer_addr_vir_non_cache, &uac_temp1);

    if(uac_in_freq_type == _48K_16B) {
        UAC_in_base_mps_size = UAC_MPS_SIZE;
    }
    else if(uac_in_freq_type == _96K_16B) {
        UAC_in_base_mps_size = UAC_MPS_SIZE_96K16B;
    }
    else if(uac_in_freq_type == _192K_16B) {
        UAC_in_base_mps_size = UAC_MPS_SIZE_192K16B;
    }

    if(uac_out0_freq_type == _48K_16B) {
        UAC_out0_base_mps_size = UAC_MPS_SIZE;
    }
    else if(uac_out0_freq_type == _96K_16B) {
        UAC_out0_base_mps_size = UAC_MPS_SIZE_96K16B;
    }
    else if(uac_out0_freq_type == _192K_16B) {
        UAC_out0_base_mps_size = UAC_MPS_SIZE_192K16B;
    }

    // clear dummy register
    for(temp_usb_addr = uac_device_to_host_buffer_addr_vir_non_cache; temp_usb_addr < (uac_device_to_host_buffer_addr_vir_non_cache + 0x100); temp_usb_addr = temp_usb_addr + 4) {
        rtd_mem_outl(temp_usb_addr, 0x00000000);
    }

    // uac_device_to_host_trb_addr = 0x1a300000 + (0xd0 * 16) + 0x100
    //uac_device_to_host_trb_addr = uac_device_to_host_buffer_addr + (UINT32)UAC_out0_base_mps_size * (UINT32)uac_device_to_host_trb_cnt + 0x100;
    usb_uac_init_1_1(UAC_out0_base_mps_size);

    // uac_host_to_device_buffer_addr = 0x1a300e00
    usb_uac_init_1_2();

    // uac_device_to_host_trb_addr = 0x1a300000 + (0xd0 * 16) + 0x100
    //uac_device_to_host_trb_addr_2 = uac_device_to_host_buffer_addr_2 + (UINT32)UAC_out0_base_mps_size * (UINT32)uac_device_to_host_trb_cnt + 0x100;
    usb_uac_init_2_1(UAC_out0_base_mps_size);

    // uac_host_to_device_buffer_addr = 0x1a300e00
    usb_uac_init_2_2();

    // uac_host_to_device_trb_addr = 0x1a300f00 + (0xd0 * 20) + 0x100
    uac_host_to_device_trb_addr = uac_host_to_device_buffer_addr + (UINT32)UAC_in_base_mps_size * (UINT32)uac_host_to_device_trb_cnt;
    uac_host_to_device_trb_addr_vir_non_cache = (uac_host_to_device_trb_addr - uac_device_to_host_buffer_addr) + uac_device_to_host_buffer_addr_vir_non_cache;

    pm_printk(LOGGER_INFO, " [UAC] init (D2H data = %x %px),", uac_device_to_host_buffer_addr  , uac_device_to_host_buffer_addr_vir_non_cache);
    pm_printk(LOGGER_INFO, " (D2H trb = %x %px),",             uac_device_to_host_trb_addr     , uac_device_to_host_trb_addr_vir_non_cache);
    pm_printk(LOGGER_INFO, " (D2H data2 = %x %px),",           uac_device_to_host_buffer_addr_2, uac_device_to_host_buffer_addr_2_vir_non_cache);
    pm_printk(LOGGER_INFO, " (D2H trb2 = %x %px),",            uac_device_to_host_trb_addr_2   , uac_device_to_host_trb_addr_2_vir_non_cache);
    pm_printk(LOGGER_INFO, " (H2D data = %x %px),",            uac_host_to_device_buffer_addr  , uac_host_to_device_buffer_addr_vir_non_cache);
    pm_printk(LOGGER_INFO, " (H2D trb = %x %px)\n",            uac_host_to_device_trb_addr     , uac_host_to_device_trb_addr_vir_non_cache);

    return 0;
}

UINT8 usb_uac_in_connect(UAC_TYPE uac_type) __banked
{
    // Device -> Host
#if 0
    if(uac_type == _UAC_OUT0) {
        dwc_temp1 = rtd_inl(AUC_OUT2_AUC_OUT2_CTRL1_reg);
        rtd_outl(AUC_OUT2_AUC_OUT2_CTRL1_reg, dwc_temp1 | 1);
    }
    else {
        dwc_temp1 = rtd_inl(AUC_OUT1_AUC_OUT1_CTRL1_reg);
        rtd_outl(AUC_OUT1_AUC_OUT1_CTRL1_reg, dwc_temp1 | 1);
    }
#endif
    return 0;
}

UINT8 usb_uac_in_disconnect(UAC_TYPE uac_type) __banked
{
    // Device -> Host
#if 0
    if(uac_type == _UAC_OUT0) {
        dwc_temp1 = rtd_inl(AUC_OUT2_AUC_OUT2_CTRL1_reg);
        rtd_outl(AUC_OUT2_AUC_OUT2_CTRL1_reg, dwc_temp1 & 0xfffffffe);
    }
    else {
        dwc_temp1 = rtd_inl(AUC_OUT1_AUC_OUT1_CTRL1_reg);
        rtd_outl(AUC_OUT1_AUC_OUT1_CTRL1_reg, dwc_temp1 & 0xfffffffe);
    }
#endif
    return 0;
}

UINT8 usb_uac_out_connect(UAC_TYPE uac_type) __banked
{
    // Host -> Device
#if 0
    dwc_temp1 = rtd_inl(AUC_IN_AUC_IN_CTRL1_reg);
    rtd_outl(AUC_IN_AUC_IN_CTRL1_reg, dwc_temp1 | 1);
#endif
    return 0;
}

UINT8 usb_uac_out_disconnect(UAC_TYPE uac_type) __banked
{
    // Host -> Device
#if 0
    dwc_temp1 = rtd_inl(AUC_IN_AUC_IN_CTRL1_reg);
    rtd_outl(AUC_IN_AUC_IN_CTRL1_reg, dwc_temp1 & 0xfffffffe);
#endif
    return 0;
}

void usb_uac_init(void) __banked
{
    if((rtd_inl(SYS_REG_SYS_CLKEN2_reg) & _BIT18) == _BIT18) {
        if((rtd_inl(SYS_REG_SYS_CLKEN1_reg) & _BIT16) == _BIT16) {
            //rtd_outl(0xb8069174, 0x00000010); // usb sent mframe
#ifndef CONFIG_USB_CDC_ONLY
            UAC_IN_freq_type = _48K_16B;
            UAC_OUT0_freq_type = _48K_16B;
            UAC_OUT1_freq_type = _48K_16B;

            if(usb_sw_cfg_GetUAC_Enable_BANK37() == _FUNCTION_ENABLE) {

                if(usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK37() == _AUDIO_48K_ONLY)
                    usb_uac_alloc(_48K_16B, _48K_16B);  // Max. alloc
                else if(usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK37() == _AUDIO_WITH_96K)
                    usb_uac_alloc(_96K_16B, _96K_16B);  // Max. alloc
                else if(usb_sw_cfg_GetAudio_PCM_Audio_Function_BANK37() == _AUDIO_WITH_192K)
                    usb_uac_alloc(_192K_16B, _192K_16B);  // Max. alloc

                usb_uac_in_sram_init(UAC_IN_freq_type);
                usb_uac_out0_sram_init(UAC_OUT0_freq_type);
                usb_uac_out1_sram_init(UAC_OUT1_freq_type);
            }
#endif
        }
    }
}

