#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <io.h>
#include <linux/freezer.h>
#if IS_ENABLED(CONFIG_RTK_HDMI_RX)
#include <rtk_kdriver/measure/rtk_measure.h>
#include <rtk_kdriver/tvscalercontrol/hdmirx/hdmifun.h>
#include <rbus/hdmi_top_common_reg.h>
#include <rbus/hdmi_dp_common_misc_reg.h>
#include <rbus/hdmi_p3_reg.h>
#include <rbus/hdmi21_p3_reg.h>
#endif

#include <rbus/hdmitx_phy_reg.h>
#include <rbus/hdmitx_vgip_reg.h>
#include <rbus/hdmitx20_mac1_reg.h>
#include <rbus/dsce_misc_reg.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll_reg_reg.h>
#include <rbus/timer_reg.h>


#include <rbus/dsce_reg.h>
#include <rbus/hdmitx_misc_reg.h>
#include <rbus/hdmitx20_mac0_reg.h>
#include <rbus/hdmitx20_on_region_reg.h>
#include <rbus/hdmitx21_mac_reg.h>
#include <rbus/hdmitx_vgip_reg.h>

#include <hdcp/hdcptx/hdcptx_export.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h> 
#include <tvscalercontrol/hdmitx/hdmitx_lib.h>
#include <tvscalercontrol/hdmitx/hdmitx.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_share.h>
#include <tvscalercontrol/txpre/txpre_mode.h>
#include <tvscalercontrol/txpre/txpre.h>
#include <rtk_kdriver/tvscalercontrol/hdmirx/hdmifun.h>

#include <scaler/scalerCommon.h>

#include <mach/platform.h>
#include <mach/rtk_platform.h>


// [MARK2] module init
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/freezer.h>

int hdmitx_major   = HDMITX_MAJOR;
int hdmitx_minor   = 0 ;
int hdmitx_nr_devs = HDMITX_NR_DEVS;
#ifndef UT_flag
static dev_t hdmitx_devno = 0;//hdmitx device number
static struct cdev hdmitx_cdev;
#endif // #ifndef UT_flag
struct semaphore HDMITX_Semaphore;
static struct semaphore HDMITXBlock_Semaphore;
module_param(hdmitx_major, int, S_IRUGO);
module_param(hdmitx_minor, int, S_IRUGO);
module_param(hdmitx_nr_devs, int, S_IRUGO);

UINT8 Hdmitx_PtgMode = 0;
StructTxPreInfo txpre_info = {0};

static struct class *hdmitx_class = NULL;
static struct platform_device *hdmitx_platform_devs = NULL;
wait_queue_head_t hdmitx_thd_wait_qu;

static bool hdmitx_tsk_running_flag = FALSE;//Record vsc_scaler_tsk status. True: Task is running
static struct task_struct *p_hdmitx_tsk = NULL;


#define HDMITX_POLLING_TIME_MS 10

//extern void ScalerHdmiTxSetVTEMpacektEn(UINT8 enable);
//extern void ScalerHdmiTxSetVRRFlagEn(UINT8 enable);
extern INT8 ScalerHdmiPhyTx0PowerOff(void);
extern INT32 hdmitx_init(void);
extern void HDMITX_drv_menuCmd_setTxPtgTimingMode(UINT8 timeMode);
extern void HDMITX_drv_menuCmd_set_colorFormat(I3DDMA_COLOR_SPACE_T colorfmt);
extern void HDMITX_drv_menuCmd_set_colorDepth(I3DDMA_COLOR_DEPTH_T colordepth);

extern void HDMITX_drv_menuCmd_set_frlMode(EnumHdmi21FrlType frlmode);
extern void HDMITX_drv_menuCmd_set_bypassLT(UINT8 bypass_lt_en);
extern void hdmitx_AP_Init_event_handler(UINT32 param);
extern unsigned char txpre_config(StructTxPreInfo * txpre_info);
extern void HDMITX_InitTxPreInfo(StructTxPreInfo *txpre_info);
extern INT32 hdmitx_state_event_handler(void *data);
extern MEASURE_TIMING_T timing_info;
extern UINT8 sendApHdmiConnect;



char timingName[TX_TIMING_NUM][50] = {
    "TX_TIMING_DEFAULT",
    "TX_TIMING_640X480P60",
    "TX_TIMING_720X480P60",
    "TX_TIMING_720X480P120",
    "TX_TIMING_720X480P240",
    "TX_TIMING_720X576P50",
    "TX_TIMING_720X576P100",
    "TX_TIMING_720X576P200",
    "TX_TIMING_1440X288P50",
    "TX_TIMING_1440X240P60",
    "TX_TIMING_2880X288P50",
    "TX_TIMING_1440X576P50",
    "TX_TIMING_2880X576P50",
    "TX_TIMING_2880X240P60",
    "TX_TIMING_1440X480P60",
    "TX_TIMING_2880X480P60",
    "TX_TIMING_720P24",
    "TX_TIMING_720P25",
    "TX_TIMING_720P30",
    "TX_TIMING_720P48",
    "TX_TIMING_720P50",
    "TX_TIMING_720P60",
    "TX_TIMING_720P100",
    "TX_TIMING_720P120",
    "TX_TIMING_1080P24",
    "TX_TIMING_1080P25",
    "TX_TIMING_1080P30",
    "TX_TIMING_1080P48",
    "TX_TIMING_1080P50",
    "TX_TIMING_1080P60",
    "TX_TIMING_1080P100",
    "TX_TIMING_1080P120",
    "TX_TIMING_1080P144_CVT_RB",
    "TX_TIMING_1080P240_CVT",
    "TX_TIMING_1080P240_CVT_RB",
    "TX_TIMING_1680X720P24",
    "TX_TIMING_1680X720P25",
    "TX_TIMING_1680X720P30",
    "TX_TIMING_1680X720P48",
    "TX_TIMING_1680X720P50",
    "TX_TIMING_1680X720P60",
    "TX_TIMING_1680X720P100",
    "TX_TIMING_1680X720P120",
    "TX_TIMING_2560X1080P24",
    "TX_TIMING_2560X1080P25",
    "TX_TIMING_2560X1080P30",
    "TX_TIMING_2560X1080P48",
    "TX_TIMING_2560X1080P50",
    "TX_TIMING_2560X1080P60",
    "TX_TIMING_2560X1080P100",
    "TX_TIMING_2560X1080P120",
    "TX_TIMING_2560X1080P144_CVT_RB",
    "TX_TIMING_2560X1440P60_CVT_RB",
    "TX_TIMING_2560X1440P60",
    "TX_TIMING_2560X1440P120_CVT_RB",
    "TX_TIMING_2560X1440P120_CVT",
    "TX_TIMING_2560X1440P144_CVT_CAL",
    "TX_TIMING_2560X1440P144_TMDS",
    "TX_TIMING_2560X1440P144_CVT_RBV2",
    "TX_TIMING_4K2KP24",
    "TX_TIMING_4K2KP25",
    "TX_TIMING_4K2KP30",
    "TX_TIMING_4K2KP48",
    "TX_TIMING_4K2KP50",
    "TX_TIMING_4K2KP60",
    "TX_TIMING_4K2KP100",
    "TX_TIMING_4K2KP120",
    "TX_TIMING_4K2KP144_CVT_RB",
    "TX_TIMING_4K2KP144_CVT_RBv2",
    "TX_TIMING_4K2KP144_CVT_VUP",
    "TX_TIMING_4K2KP144_CVT",
    "TX_TIMING_4K2KP60_CVT",
    "TX_TIMING_4K2KP60_CVT_RB",
    "TX_TIMING_4K2KP60_CVT_RBv2",
    "TX_TIMING_4K2KP120_CVT",
    "TX_TIMING_4K2KP120_CVT_RB",
    "TX_TIMING_4K2KP120_CVT_RBv2",
    "TX_TIMING_2560X1440P240_CVT_RB",
    "TX_TIMING_2560X1440P240_CVT_RBv2",
    "TX_TIMING_2560X1440P240_861_OVT",
    "TX_TIMING_2K1KP360_CVT_RB",
    "TX_TIMING_2K1KP360_CVT_RBv2",
    "TX_TIMING_2K1KP360_861_OVT",
    "TX_TIMING_1280x720P144_CVT_RB",
    "TX_TIMING_1280x720P144_CVT_RBv2",
    "TX_TIMING_2560x1440P165_CVT_RB",
    "TX_TIMING_2560x1440P165_CVT_RBv2",
    "TX_TIMING_2560x1440P170_CVT_RB",
    "TX_TIMING_2560x1440P170_CVT_RBv2",
    "TX_TIMING_2560x1440P180_CVT_RB",
    "TX_TIMING_2560x1440P180_CVT_RBv2",
    "TX_TIMING_3440x1440P60_CVT_RB",
    "TX_TIMING_3440x1440P60_CVT_RBv2",
    "TX_TIMING_3440x1440P120_CVT_RB",
    "TX_TIMING_3440x1440P120_CVT_RBv2",
    "TX_TIMING_3440x1440P144_CVT_RB",
    "TX_TIMING_3440x1440P144_CVT_RBv2",
    "TX_TIMING_3440x1440P165_CVT_RB",
    "TX_TIMING_3440x1440P165_CVT_RBv2",
    "TX_TIMING_3440x1440P170_CVT_RB",
    "TX_TIMING_3440x1440P170_CVT_RBv2",
    "TX_TIMING_3440x1440P180_CVT_RB",
    "TX_TIMING_3440x1440P180_CVT_RBv2",
    "TX_TIMING_3440x1440P240_CVT_RB",
    "TX_TIMING_3440x1440P240_CVT_RBv2",
    //interlace
    #if 0
    "TX_TIMING_1440X480I120",
    "TX_TIMING_1440X480I240",
    "TX_TIMING_1440X576I100",
    "TX_TIMING_1440X576I200",
    "TX_TIMING_2880X576I50",
    "TX_TIMING_2880X480I60",
    #endif
    "TX_TIMING_1440X480I60",
    "TX_TIMING_1440X576I50",
    "TX_TIMING_1080I50",
    "TX_TIMING_1080I60",
    "TX_TIMING_1080I100",
    "TX_TIMING_1080I120",
    //DMT", CVT", CVT RB V1/V2
    "TX_TIMING_1280x768P60_CVT_RB",
    "TX_TIMING_1280x768P60_DMT",
    "TX_TIMING_1280x800P60_CVT_RB",
    "TX_TIMING_1280x800P60_DMT",
    "TX_TIMING_1280x960P60_DMT",
    "TX_TIMING_1280x1024P60_DMT",
    "TX_TIMING_1360x768P60_DMT",
    "TX_TIMING_1366x768P60_CVT_RB",
    "TX_TIMING_1366x768P60_DMT",
    "TX_TIMING_1400x1050P60_CVT_RB",
    "TX_TIMING_1400x1050P60_DMT",
    "TX_TIMING_1440x900P60_CVT_RB",
    "TX_TIMING_1440x900P60_DMT",
    "TX_TIMING_1600x900P60_CVT_RB",
    "TX_TIMING_1600x1200P60_DMT",
    "TX_TIMING_1680x1050P60_CVT_RB",
    "TX_TIMING_1680x1050P60_DMT",
    "TX_TIMING_1792x1344P60_DMT",
    "TX_TIMING_1856x1392P60_DMT",
    "TX_TIMING_1920x1200P60_CVT_RB",
    "TX_TIMING_1920x1200P60_DMT",
    "TX_TIMING_1920x1440P60_DMT",
    "TX_TIMING_2048x1152P60_CVT_RB",
    "TX_TIMING_2560x1600P60_CVT_RB",
    "TX_TIMING_2560x1600P60_DMT",
    "TX_TIMING_3440x1440P60_CVT",
    #if 0
    "TX_TIMING_1600x1200P65_DMT",
    "TX_TIMING_1600x1200P70_DMT",
    "TX_TIMING_1280x768P75_DMT",
    "TX_TIMING_1280x800P75_DMT",
    "TX_TIMING_1280x1024P75_DMT",
    "TX_TIMING_1400x1050P75_DMT",
    "TX_TIMING_1440x900P75_DMT",
    "TX_TIMING_1600x1200P75_DMT",
    "TX_TIMING_1680x1050P75_DMT",
    "TX_TIMING_1792x1344P75_DMT",
    "TX_TIMING_1856x1392P75_DMT",
    "TX_TIMING_1920x1200P75_DMT",
    "TX_TIMING_1920x1440P75_DMT",
    "TX_TIMING_2560x1600P75_DMT",
    "TX_TIMING_1280x768P85_DMT",
    "TX_TIMING_1280x800P85_DMT",
    "TX_TIMING_1280x960P85_DMT",
    "TX_TIMING_1280x1024P85_DMT",
    "TX_TIMING_1400x1050P85_DMT",
    "TX_TIMING_1440x900P85_DMT",
    "TX_TIMING_1600x1200P85_DMT",
    "TX_TIMING_1680x1050P85_DMT",
    "TX_TIMING_1920x1200P85_DMT",
    "TX_TIMING_2560x1600P85_DMT",
    #endif
    "TX_TIMING_1280x768P120_CVT_RB",
    "TX_TIMING_1280x800P120_CVT_RB",
    "TX_TIMING_1280x960P120_CVT_RB",
    "TX_TIMING_1280x1024P120_CVT_RB",
    "TX_TIMING_1400x1050P120_CVT_RB",
    "TX_TIMING_1440x900P120_CVT_RB",
    "TX_TIMING_1600x1200P120_CVT_RB",
    "TX_TIMING_1680x1050P120_CVT_RB",
    "TX_TIMING_1792x1344P120_CVT_RB",
    "TX_TIMING_1856x1392P120_CVT_RB",
    "TX_TIMING_1920x1200P120_CVT_RB",
    "TX_TIMING_1920x1440P120_CVT_RB",
    "TX_TIMING_2560x1600P120_CVT_RB",
    "TX_TIMING_3440x1440P120_CVT",
    "TX_TIMING_2560x1600P144_DMT_CAL",
    "TX_TIMING_3840X1080P24",
    "TX_TIMING_3840X1080P25",
    "TX_TIMING_3840X1080P30",
    "TX_TIMING_3840X1080P48",
    "TX_TIMING_3840X1080P50",
    "TX_TIMING_3840X1080P60",
    "TX_TIMING_3840X1080P100",
    "TX_TIMING_3840X1080P120",
    "TX_TIMING_3840X1080P144",
    "TX_TIMING_3840X1080P200",
    "TX_TIMING_3840X1080P240",
    "TX_TIMING_3440x1440P100_CVT",
    "TX_TIMING_4096X2160P24",
    "TX_TIMING_4096X2160P25",
    "TX_TIMING_4096X2160P30",
    "TX_TIMING_4096X2160P48",
    "TX_TIMING_4096X2160P50",
    "TX_TIMING_4096X2160P60",
    "TX_TIMING_4096X2160P100",
    "TX_TIMING_4096X2160P120",
    "TX_TIMING_4096X2160P144_CVT_RB",
    "TX_TIMING_5120X2160P24",
    "TX_TIMING_5120X2160P25",
    "TX_TIMING_5120X2160P30",
    "TX_TIMING_5120X2160P48",
    "TX_TIMING_5120X2160P50",
    "TX_TIMING_5120X2160P60",
    "TX_TIMING_5120X2160P100",
    "TX_TIMING_5120X2160P120",

    "TX_TIMING_5120X1440P24",
    "TX_TIMING_5120X1440P25",
    "TX_TIMING_5120X1440P30",
    "TX_TIMING_5120X1440P48",
    "TX_TIMING_5120X1440P50",
    "TX_TIMING_5120X1440P60",
    "TX_TIMING_5120X1440P100",
    "TX_TIMING_5120X1440P120",

    "TX_TIMING_5120X2880P24",
    "TX_TIMING_5120X2880P25",
    "TX_TIMING_5120X2880P30",
    "TX_TIMING_5120X2880P48",
    "TX_TIMING_5120X2880P50",
    "TX_TIMING_5120X2880P60",

    "TX_TIMING_DSCE_4K60",
    "TX_TIMING_DSCE_4K120",
    "TX_TIMING_DSCE_4K144_RBV1",
};

void showTimingList(void) {
    int i = 0;
    NoteMessageHDMITx ("Hdmitx timing list:\r\n");
    NoteMessageHDMITx ("===================================================\r\n");

    for(i = 0; i < TX_TIMING_NUM; i++) {
            NoteMessageHDMITx ("[%d] %s\r\n", i, timingName[i]);
    }

    NoteMessageHDMITx ("===================================================\r\n");

}

unsigned int txclk_test = 594000000;
unsigned int bypass_port;
unsigned int reg_val = 1;

void setHdmitxCmd(char *cmd_buf)
{

    // HDMITX shell command
    if (strncmp(cmd_buf, "tx_timelist", 11) == 0) {
        NoteMessageHDMITx("[HDMITX] tx_timeList!\n");
        showTimingList();
    }
    else if (strncmp(cmd_buf, "bypass_port=", 12) == 0) {
        sscanf (&cmd_buf[12],"%d", &bypass_port);
        NoteMessageHDMITx("[HDMITX] bypass_port=%d\n", bypass_port);
        drvif_hdmi_pkt_bypass_to_hdmitx_en(bypass_port, 1);
        drvif_hdmi_bypass_port_set(bypass_port, 0);
    }
    else if (strncmp(cmd_buf, "set_reg=", 8) == 0) {
        sscanf (&cmd_buf[8],"%d", &reg_val);
        NoteMessageHDMITx("[HDMITX] set_reg=%d\n", reg_val);
        rtd_outl(0xB80B2548, reg_val);
    }
    else if (strncmp(cmd_buf, "tx_timing=", 10) == 0) {
        unsigned int timeMode;
        sscanf (&cmd_buf[10],"%d", &timeMode);
        NoteMessageHDMITx("[HDMITX] tx_timing=%d\n", timeMode);
        HDMITX_drv_menuCmd_setTxPtgTimingMode(timeMode);

        if(g_scaler_display_info.input_src == DRIVER_HDMITX_INPUT_SRC_DTG){
            set_hdmitxOutputMode(TX_OUT_SOURCE_MODE);
        }else if(g_scaler_display_info.input_src == DRIVER_HDMITX_INPUT_SRC_DPRX){
            set_hdmitxOutputMode(TX_OUT_SOURCE_MODE);
            rtd_maskl(0xB804C0D4, ~HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel_mask, HDMI_DP_COMMON_MISC_COMMON_PORT_SEL_common_bypass_sel(4));
        }else{
            set_hdmitxOutputMode(TX_OUT_THROUGH_MODE);
            drvif_Hdmi_TimingInfo_for_hdmitx(bypass_port,&timing_info);
        }
    }
    else if(strncmp(cmd_buf, "tx_bypasslt=", 12) == 0) {
        unsigned int bypassLT;
        sscanf (&cmd_buf[12],"%d", &bypassLT);
        NoteMessageHDMITx("[HDMITX] tx_bypass=%d\n", bypassLT);
        HDMITX_drv_menuCmd_set_bypassLT(bypassLT);
    }
    else if(strncmp(cmd_buf, "tx_frlmode=", 11) == 0) {
        unsigned int frlMode;
        sscanf (&cmd_buf[11],"%d", &frlMode);
        NoteMessageHDMITx("[HDMITX] tx_frlmode=%d\n", frlMode);
        HDMITX_drv_menuCmd_set_frlMode(frlMode);
    }
    else if(strncmp(cmd_buf, "tx_colorfmt=", 12) == 0) {
        unsigned int colorfmt;
        sscanf (&cmd_buf[12],"%d", &colorfmt);
        NoteMessageHDMITx("[HDMITX] tx_colorfmt=%d\n", colorfmt);
        HDMITX_drv_menuCmd_set_colorFormat(colorfmt);
    }
    else if(strncmp(cmd_buf, "tx_colordepth=", 14) == 0) {
        unsigned int colordepth;
        sscanf (&cmd_buf[14],"%d", &colordepth);
        NoteMessageHDMITx("[HDMITX] tx_colordepth=%d\n", colordepth);
        HDMITX_drv_menuCmd_set_colorDepth(colordepth);
    }
    else if(strncmp(cmd_buf, "tx_ptgmode=", 11) == 0) {
        unsigned int tx_ptgmode;
        sscanf (&cmd_buf[11],"%d", &tx_ptgmode);
        NoteMessageHDMITx("[HDMITX] tx_ptgmode=%d\n", tx_ptgmode);
        Hdmitx_PtgMode = tx_ptgmode;
    }
    else if(strncmp(cmd_buf, "tx_inputsrc=", 12) == 0) {
        unsigned int tx_inputsrc;
        sscanf (&cmd_buf[12],"%d", &tx_inputsrc);
        NoteMessageHDMITx("[HDMITX] tx_inputsrc=%d\n", tx_inputsrc);
        g_scaler_display_info.input_src = tx_inputsrc;
    }
    else if(strncmp(cmd_buf, "tx_inputclk=", 12) == 0) {
        unsigned int tx_inputclk;
        sscanf (&cmd_buf[12],"%d", &tx_inputclk);
        NoteMessageHDMITx("[HDMITX] tx_inputclk=%d\n", tx_inputclk);
        txclk_test = tx_inputclk;
    }
    else if (strncmp(cmd_buf, "getrxtiming", 11) == 0) {
        NoteMessageHDMITx("[HDMITX] getrxtiming port=%d\n", bypass_port);
        drvif_Hdmi_TimingInfo_for_hdmitx(bypass_port,&timing_info);
    }
    else if (strncmp(cmd_buf, "reruntxpre", 10) == 0) {
        NoteMessageHDMITx("[HDMITX] reruntxpre\n");
        txpre_config(&txpre_info);
    }


    return;
}

int HDMITX_event_handler(void *data)
{

    DebugMessageHDMITx("[HDMITX] Handler start...@%s\n", __FUNCTION__);

    // event check & process
    while(1){
        //
        ROSTimeDly(100);
    }

    DebugMessageHDMITx("[HDMITX] Done!!@%s\n", __FUNCTION__);
    return 0;
}


static int hdmitx_run_tsk(void* arg)
{
    unsigned int polling_cycle_us;
    set_freezable();
    while (!kthread_freezable_should_stop(NULL) && (hdmitx_tsk_running_flag == TRUE)) {
        polling_cycle_us = HDMITX_POLLING_TIME_MS * 1000;
        if((polling_cycle_us> 0) && (polling_cycle_us <64000)) {
            wait_event_interruptible_hrtimeout(hdmitx_thd_wait_qu,(!hdmitx_tsk_running_flag),ktime_set(0,polling_cycle_us*NSEC_PER_USEC));
        }
        else {
            msleep(10);
        }
        hdmitx_state_event_handler(arg);	
    }

    FatalMessageHDMITx("[HDMITX] hdmitx_run_tsk thread terminated\n");
    #ifdef HDMITX_LINUX_PLATFORM
    /* Wait until we are told to stop */
    for (;;) {
        set_current_state(TASK_INTERRUPTIBLE);
        if (kthread_freezable_should_stop(NULL))
            break;
        schedule();
    }
    __set_current_state(TASK_RUNNING);
    #else
    hdmi_task = NULL;
    #endif

    return 0;
}

static void create_hdmitx_tsk(void)
{
    int err;
    if (hdmitx_tsk_running_flag == FALSE) {
        NoteMessageHDMITx("create_hdmitx_tsk a()\n");

        p_hdmitx_tsk = kthread_create(hdmitx_run_tsk, NULL, "hdmitx_tsk");

        if (p_hdmitx_tsk) {
            wake_up_process(p_hdmitx_tsk);
            hdmitx_tsk_running_flag = TRUE;
        } else {
            err = PTR_ERR(p_hdmitx_tsk);
            FatalMessageHDMITx("[HDMITX] Unable to start create_hdmitx_tsk (err_id = %d)./n", err);
        }
    }
}

static void delete_hdmitx_tsk(void)
{
    int ret;
    if (hdmitx_tsk_running_flag) {
        if(p_hdmitx_tsk) {
            ret = kthread_stop(p_hdmitx_tsk);
            if (!ret) {
                p_hdmitx_tsk = NULL;
                hdmitx_tsk_running_flag = FALSE;
                wake_up(&hdmitx_thd_wait_qu);
                FatalMessageHDMITx("[HDMITX] hdmitx_scaler_tsk thread stopped\n");
            }
        }
    }
}


void hdmitx_sys_clock_reset(void)
{

   FatalMessageHDMITx("[HDMITX] %s\n", __FUNCTION__);
   NoteMessageHDMITx("[HDMITX][Now] CRT RST/CLK=%x/%x\n", rtd_inl(SYS_REG_SYS_SRST6_reg), rtd_inl(SYS_REG_SYS_CLKEN6_reg));
    // rtd_outl(SYS_REG_SYS_SRST6_reg, (UINT32)(SYS_REG_SYS_SRST6_rstn_dsce_mask|SYS_REG_SYS_SRST6_rstn_hdmitx_mask| SYS_REG_SYS_SRST6_rstn_hdmitx_phy_mask| SYS_REG_SYS_SRST6_rstn_hdmitx_off_mask));//Reset bit of hdmi dsce/hdmi hdmitx
    rtd_outl(SYS_REG_SYS_CLKEN6_reg, (UINT32)(SYS_REG_SYS_CLKEN6_clken_dsce_mask|SYS_REG_SYS_CLKEN6_clken_hdmitx_mask|SYS_REG_SYS_CLKEN6_write_data_mask));//clk bit of hdmi dsce/hdmitx
    // rtd_outl(SYS_REG_SYS_CLKEN6_reg, (UINT32)(SYS_REG_SYS_CLKEN6_clken_dsce_mask|SYS_REG_SYS_CLKEN6_clken_hdmitx_mask));//clk bit of hdmi dsce/hdmitx
    rtd_outl(SYS_REG_SYS_SRST6_reg, (UINT32)(SYS_REG_SYS_SRST6_rstn_dsce_mask|SYS_REG_SYS_SRST6_rstn_hdmitx_mask| SYS_REG_SYS_SRST6_rstn_hdmitx_phy_mask| SYS_REG_SYS_SRST6_rstn_hdmitx_off_mask|SYS_REG_SYS_SRST6_write_data_mask));//Reset bit of hdmi dsce/hdmi hdmitx
    rtd_outl(SYS_REG_SYS_CLKEN6_reg, (UINT32)(SYS_REG_SYS_CLKEN6_clken_dsce_mask|SYS_REG_SYS_CLKEN6_clken_hdmitx_mask|SYS_REG_SYS_CLKEN6_write_data_mask));//clk bit of hdmi dsce/hdmitx
    NoteMessageHDMITx("[HDMITX][New] CRT RST/CLK=%x/%x\n", rtd_inl(SYS_REG_SYS_SRST6_reg), rtd_inl(SYS_REG_SYS_CLKEN6_reg));

    // [RL6583-3452] fix HDMITX 2.0 6G Guard Bang Disparity
    rtd_maskl(HDMITX20_MAC1_VFIFO_8_reg, ~HDMITX20_MAC1_VFIFO_8_tmds_disp_cnt_sel_mask, HDMITX20_MAC1_VFIFO_8_tmds_disp_cnt_sel_mask);
    // src clock
    rtd_setbits(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_in_clk_en_mask);
    FatalMessageHDMITx("[HDMITX] %s Done\n", __FUNCTION__);

    return;
}

void hdmitx_sys_clock_disable(void)
{
   FatalMessageHDMITx("[HDMITX] %s\n", __FUNCTION__);
   NoteMessageHDMITx("[HDMITX][Now] CRT RST/CLK=%x/%x\n", rtd_inl(SYS_REG_SYS_SRST6_reg), rtd_inl(SYS_REG_SYS_CLKEN6_reg));
   // src clock
    rtd_clearbits(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_in_clk_en_mask);
    rtd_outl(SYS_REG_SYS_CLKEN6_reg, (UINT32)(SYS_REG_SYS_CLKEN6_clken_dsce_mask|SYS_REG_SYS_CLKEN6_clken_hdmitx_mask));//clk bit of hdmi dsce/hdmitx
    rtd_outl(SYS_REG_SYS_SRST6_reg, (UINT32)(SYS_REG_SYS_SRST6_rstn_dsce_mask|SYS_REG_SYS_SRST6_rstn_hdmitx_mask));//Reset bit of hdmi dsce/hdmi hdmitx
    NoteMessageHDMITx("[HDMITX][New] CRT RST/CLK=%x/%x\n", rtd_inl(SYS_REG_SYS_SRST6_reg), rtd_inl(SYS_REG_SYS_CLKEN6_reg));
    FatalMessageHDMITx("[HDMITX] %s Done\n", __FUNCTION__);

    return;
}

#ifndef UT_flag
int hdmitx_open(struct inode *inode, struct file *filp) {
    return 0;
}

ssize_t  hdmitx_read(struct file *filep, char *buffer, size_t count, loff_t *offp)
{
    return 0;

}

ssize_t hdmitx_write(struct file *filep, const char *buffer, size_t count, loff_t *offp)
{
    long ret = count;
    char cmd_buf[100] = {0};

    if (count >= 100)
        return -EFAULT;

    if (copy_from_user(cmd_buf, buffer, count)) {
        ret = -EFAULT;
    }

    if(count > 0) {
       cmd_buf[count] = '\0';
    }

    setHdmitxCmd(cmd_buf); // HDMITX shell command
    return ret;

}

int hdmitx_release(struct inode *inode, struct file *filep)
{
    return 0;
}
void hdmitx_shutdown(struct platform_device *dev)
{
    ScalerHdmiPhyTx0PowerOff();
}

int hdmitx_suspend(struct device *p_dev)
{
    ScalerHdmiPhyTx0PowerOff();
    return 0;
}

static int  hdmitx_resume(struct device *p_dev)
{
    hdmitx_sys_clock_reset();
    return 0;

}
#endif // #ifndef UT_flag
UINT8 bFirstHdmiTxOn = 0;
extern void hdmitx_AP_Init_event_handler(UINT32 param);
extern void hdmitx_AP_ReInit_event_handler(UINT32 param);
extern void Scaler_reset_hdmitx_state(void);
extern UINT8 hdmitx_AP_SetTxConnect_handler(UINT8 hdmitx_port,UINT8 mode, UINT8 input_src, UINT8 input_src_port);
extern StructHDMITxOutputTimingInfo* p_getTxOutputTimingInfo;
extern void hdmitx_Scaler_Txrun_event_handler(UINT32 param);
extern UINT8 ScalerHdmiTxGetHPDStatus(void);
extern void hdmitx_AP_NoSignal_event_handler(UINT32 param);

void hdmitx_ApSetTxInit(void){
    hdmitx_sys_clock_reset();
    return;
}

void hdmitx_ApSetTxDeinit(void){
    hdmitx_sys_clock_disable();
    bFirstHdmiTxOn =0;
    return;
}

void hdmitx_ApSetTxOpen(void){
    if(!bFirstHdmiTxOn){
        hdmitx_AP_Init_event_handler(0);
        bFirstHdmiTxOn = 1;
    }else{
        hdmitx_AP_ReInit_event_handler(1);
    }
    return;
}

void hdmitx_ApSetTxClose(void){
    Scaler_reset_hdmitx_state();
    return;
}
void hdmitx_ApGetTxHdcpVer(UINT8 hdmitx_port,UINT8 *HdcpVer){
    *HdcpVer = 0;
    return;
}
void hdmitx_ApSetTxHdcpVer(UINT8 hdmitx_port,UINT8 HdcpVer){
    HDMITX_HDCP_Set_AuthVersion(hdmitx_port, HdcpVer);
    return;
}

void hdmitx_ApGetTxHdcpStatus(UINT8 hdmitx_port,UINT8 *HdcpStatus){
    *HdcpStatus = 0;
    return;
}

void hdmitx_ApSetTxAudioEn(UINT8 hdmitx_port,UINT8 AudioEn){
    return;
}

UINT8 hdmitx_ApSetTxConnect(UINT8 hdmitx_port,UINT8 mode, UINT8 input_src, UINT8 input_src_port){
    UINT8 ret = 1;
    ret = hdmitx_AP_SetTxConnect_handler(hdmitx_port, mode, input_src, input_src_port);
    return ret;
}

void hdmitx_ApSetTxDisconnect(UINT8 hdmitx_port){
    FatalMessageHDMITx("[HDMITX] hdmitx_ApSetTxDisconnect\n");
    hdmitx_AP_NoSignal_event_handler(0);
    return;
}

void hdmitx_ApSetTxRun(UINT8 hdmitx_port){

    rtd_maskl(HDMITX_VGIP_HDMITX_VGIP_CTRL_reg, ~HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_ivrun_mask, HDMITX_VGIP_HDMITX_VGIP_CTRL_hdmitx_ivrun(1)); // disable vgip for PTG mode

    hdmitx_Scaler_Txrun_event_handler(p_getTxOutputTimingInfo->TxOutputTimingMode);
    return;
}

void hdmitx_ApGetTxHpdStatus(HDMITX_HPDSTATUS_PARA_T *hpdStatusPara){
    if(!sendApHdmiConnect){
        hpdStatusPara->hpd_status = 0;
    }else{
        hpdStatusPara->hpd_status = ScalerHdmiTxGetHPDStatus();
    }

    return;
}

UINT8 hdmitx_ApGetTxPortCnt(void){
    if(ScalerHdmiTxGetHPDStatus())
        return 1;
    return 0;
}

long hdmitx_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
    int retval = 0;

    if (_IOC_TYPE(cmd) != HDMITX_IOC_MAGIC || _IOC_NR(cmd) > HDMITX_IOC_MAXNR) return -ENOTTY ;

    switch (cmd)
    {
        case HDMITX_IOC_INITIALIZE:
        {
            hdmitx_ApSetTxInit();
            break;
        }
        case HDMITX_IOC_DEINIT:
        {
            hdmitx_ApSetTxDeinit();
            break;
        }
        case HDMITX_IOC_ON:
        {
            hdmitx_ApSetTxOpen();
            break;
        }
        case HDMITX_IOC_OFF:
        {
            hdmitx_ApSetTxClose();
            break;
        }
        case HDMITX_IOC_CONNECT:
        {
            HDMITX_CONNECT_PARA_T connectPara;
            if(copy_from_user((void *)&connectPara, (const void __user *)arg, sizeof(HDMITX_CONNECT_PARA_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_CONNECT copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                NoteMessageHDMITx("[HDMITX]AP bypass src = %d port=%d\n", connectPara.input_src,connectPara.input_src_port);
                g_scaler_display_info.input_src = connectPara.input_src;
                if(hdmitx_ApSetTxConnect(connectPara.hdmitx_port, connectPara.mode, connectPara.input_src, connectPara.input_src_port)==FALSE)
                    retval = -1;
            }
            break;
        }
        case HDMITX_IOC_DISCONNECT:
        {
            UINT8 port;
            if (copy_from_user(&port, (unsigned char __user *)arg, sizeof(UINT8)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_DISCONNECT copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                hdmitx_ApSetTxDisconnect(port);
            }
            break;
        }
        case HDMITX_IOC_RUN:
        {
            UINT8 port;
            if (copy_from_user(&port, (unsigned char __user *)arg, sizeof(UINT8)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_RUN copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                hdmitx_ApSetTxRun(port);
            }
            break;
        }
        case HDMITX_IOC_POLLING_STATUS:
        {
            HDMITX_HPDSTATUS_PARA_T hpdStatusPara;
            if(copy_from_user((void *)&hpdStatusPara, (const void __user *)arg, sizeof(HDMITX_HPDSTATUS_PARA_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_POLLING_STATUS copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                hdmitx_ApGetTxHpdStatus(&hpdStatusPara);
                if (copy_to_user((void __user *)arg, (const void * )&hpdStatusPara, sizeof(HDMITX_HPDSTATUS_PARA_T)))
                {
                    FatalMessageHDMITx("[ERR] copy_to_user error\n");
                    retval = -EFAULT;
                }

            }

            break;
        }
        case HDMITX_IOC_GETTIMINGINFO:
        {
            KADP_VBE_HDMITX_TIMING_INFO_T hdmitxTimingInfo;
            if(copy_from_user((void *)&hdmitxTimingInfo, (const void __user *)arg, sizeof(KADP_VBE_HDMITX_TIMING_INFO_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_GETTIMINGINFO copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                APGetHdmiTxOutPutInfo(hdmitxTimingInfo.hdmitx_port, &(hdmitxTimingInfo.stTxOutputTimingInfo));
                if (copy_to_user((void __user *)arg, (const void * )&hdmitxTimingInfo, sizeof(KADP_VBE_HDMITX_TIMING_INFO_T)))
                {
                        FatalMessageHDMITx("[ERR] copy_to_user error\n");
                        retval = -EFAULT;
                }

            }

            break;
        }
        case HDMITX_IOC_READEDID:
        {
            KADP_VBE_HDMITX_EDID_INFO_T hdmitxEdidInfo;

            if (copy_from_user((void *)&hdmitxEdidInfo, (const void __user *)arg, sizeof(KADP_VBE_HDMITX_EDID_INFO_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_READEDID copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                if(APGetHdmiTxEdid(&hdmitxEdidInfo.len, hdmitxEdidInfo.EDID)){
                    if (copy_to_user((void __user *)arg, (const void * )&hdmitxEdidInfo, sizeof(KADP_VBE_HDMITX_EDID_INFO_T)))
                    {
                        FatalMessageHDMITx("[ERR] copy_to_user error\n");
                        retval = -EFAULT;
                    }
                
                }else{
                        retval = -EFAULT;
                }
            }
        break;
        }
        case HDMITX_IOC_HDCP_GETAUTHVER:
        {
            KADP_VBE_HDMITX_HDCP_VER_INFO_T hdmitxHdcpVerInfo;
            if (copy_from_user((void *)&hdmitxHdcpVerInfo, (const void __user *)arg, sizeof(KADP_VBE_HDMITX_HDCP_VER_INFO_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_HDCP_GETAUTHVER copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                hdmitx_ApGetTxHdcpVer(hdmitxHdcpVerInfo.hdmitx_port, &hdmitxHdcpVerInfo.HdcpVer);
                if (copy_to_user((void __user *)arg, (const void * )&hdmitxHdcpVerInfo, sizeof(KADP_VBE_HDMITX_HDCP_VER_INFO_T)))
                {
                    FatalMessageHDMITx("[ERR] copy_to_user error\n");
                    retval = -EFAULT;
                }
            }
            break;
        }
        case HDMITX_IOC_HDCP_SETAUTHVER:
        {
            KADP_VBE_HDMITX_HDCP_VER_INFO_T hdmitxHdcpVerInfo;
            if (copy_from_user((void *)&hdmitxHdcpVerInfo, (const void __user *)arg, sizeof(KADP_VBE_HDMITX_HDCP_VER_INFO_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_HDCP_GETAUTHVER copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                hdmitx_ApSetTxHdcpVer(hdmitxHdcpVerInfo.hdmitx_port, hdmitxHdcpVerInfo.HdcpVer);
            }
            break;
        }

        case HDMITX_IOC_HDCP_GETAUTHSTATUS:
        {
            KADP_VBE_HDMITX_HDCP_STATUS_INFO_T hdmitxHdcpStatusInfo;
            if (copy_from_user((void *)&hdmitxHdcpStatusInfo, (const void __user *)arg, sizeof(KADP_VBE_HDMITX_HDCP_STATUS_INFO_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_HDCP_GETAUTHSTATUS copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                hdmitx_ApGetTxHdcpStatus(hdmitxHdcpStatusInfo.hdmitx_port, &hdmitxHdcpStatusInfo.HdcpStatus);
                if (copy_to_user((void __user *)arg, (const void * )&hdmitxHdcpStatusInfo, sizeof(KADP_VBE_HDMITX_HDCP_STATUS_INFO_T)))
                {
                    FatalMessageHDMITx("[ERR] copy_to_user error\n");
                    retval = -EFAULT;
                }
            }
            break;
        }
        case HDMITX_IOC_HDCP_SETAUDIOEN:
        {
            KADP_VBE_HDMITX_AUDIO_STATUS_INFO_T hdmitxHAuidoInfo;
            if (copy_from_user((void *)&hdmitxHAuidoInfo, (const void __user *)arg, sizeof(KADP_VBE_HDMITX_AUDIO_STATUS_INFO_T)))
            {
                retval = -EFAULT;
                FatalMessageHDMITx("scaler HDMITX ioctl code=HDMITX_IOC_HDCP_GETAUTHVER copy_from_user failed!!!!!!!!!!!!!!!\n");
            }
            else
            {
                hdmitx_ApSetTxAudioEn(hdmitxHAuidoInfo.hdmitx_port, hdmitxHAuidoInfo.AudioEn);
            }
            break;
        }
        case HDMITX_IOC_GETTXPORTCNT:
        {
            UINT8 TxPortCnt;

            TxPortCnt = hdmitx_ApGetTxPortCnt();
            if (copy_to_user((void __user *)arg, (const void * )&TxPortCnt, sizeof(UINT8)))
            {
                FatalMessageHDMITx("[ERR] copy_to_user error\n");
                retval = -EFAULT;
            }

            break;
        }

    }
    return retval;


}
#if defined(CONFIG_COMPAT) && defined(CONFIG_ARM64)
long hdmitx_compat_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
    return hdmitx_ioctl(file,cmd,arg);
}
#endif


struct file_operations hdmitx_fops= {
    .owner =    THIS_MODULE,
    .open  =    hdmitx_open,
    .release =  hdmitx_release,
    .read  =    hdmitx_read,
    .write = 	hdmitx_write,
    .unlocked_ioctl = 	hdmitx_ioctl,
#if defined(CONFIG_COMPAT) && defined(CONFIG_ARM64)
    .compat_ioctl = hdmitx_compat_ioctl,
#endif
};

#ifdef CONFIG_PM
static const struct dev_pm_ops hdmitx_pm_ops =
{
    .suspend    = hdmitx_suspend,
    .resume     = hdmitx_resume,

};
#endif

static const struct of_device_id hdmitx_of_match[] = {
    {
        .compatible = "realtek,hdmi_tx",
    },
    {},
};

MODULE_DEVICE_TABLE(of, hdmitx_of_match);
static struct platform_driver hdmitx_device_driver = {
    .shutdown	= hdmitx_shutdown,
    .driver = {
        .name       = HDMITX_DEVICE_NAME,
        .bus        = &platform_bus_type,
#ifdef CONFIG_PM
    .pm        = &hdmitx_pm_ops,
#endif
    .of_match_table = of_match_ptr(hdmitx_of_match),
    },
} ;
#include "rbus/ppoverlay_txpretg_reg.h"

static void panel_enable_ddr_crash_hdmi_tx_forceBg(void)
{
    ppoverlay_txpretg_txpretg_watchdog_ctrl_RBUS ppoverlay_txpretg_txpretg_watchdog_ctrl_reg;
    ppoverlay_txpretg_txpretg_dispd_irq_record_RBUS ppoverlay_txpretg_txpretg_dispd_irq_record_reg;

    //clear status
    ppoverlay_txpretg_txpretg_dispd_irq_record_reg.regValue = rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_DISPD_IRQ_RECORD_reg);
    ppoverlay_txpretg_txpretg_dispd_irq_record_reg.ddr_irq_record_and = 1;
    ppoverlay_txpretg_txpretg_dispd_irq_record_reg.ddr_irq_record_or = 1;
    rtd_outl(PPOVERLAY_TXPRETG_TXPRETG_DISPD_IRQ_RECORD_reg,ppoverlay_txpretg_txpretg_dispd_irq_record_reg.regValue);

    //enable DDR crash error watch dog enable
    ppoverlay_txpretg_txpretg_watchdog_ctrl_reg.regValue = rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg);
    ppoverlay_txpretg_txpretg_watchdog_ctrl_reg.wde_ddr_protection = 1;
    rtd_outl(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg,ppoverlay_txpretg_txpretg_watchdog_ctrl_reg.regValue);
}

static char *hdmitx_devnode(struct device *dev, umode_t *mode)
{
    *mode =0666;
    return NULL;
}


// txpre online measure start ============================================>
unsigned char txpre_online_triggle = FALSE;         //If true means txpre online measure happens
static bool txpre_onms_tsk_running_flag = FALSE;    //Record txpre_onms_tsk status. True: Task is running
static struct task_struct *p_txpre_onms_tsk = NULL;
DECLARE_WAIT_QUEUE_HEAD(TXPRE_ONMS_WAIT_QUEUE);     //TXPre Online measure task wait queue
static DEFINE_SPINLOCK(TXPre_ONMS_Spinlock);        //Spin lock no context switch. control txpre online measure flag

void txpre_trigger_onms_handler(void)
{//This is for online measure isr used. notice online error
    spin_lock(&TXPre_ONMS_Spinlock);
    txpre_online_triggle = TRUE;
    spin_unlock(&TXPre_ONMS_Spinlock);

    wake_up(&TXPRE_ONMS_WAIT_QUEUE);
}

void txpre_ctrl_muteon(void)
{
    ppoverlay_txpretg_txpretg_watchdog_ctrl_RBUS txpretg_watchdog_ctrl_reg;

    rtd_pr_hdmitx_notice("TXpre Online measure Error Ack\n");

    if(g_scaler_display_info.input_src == TXPRE_VGIP_SRC_HDMI) {
        txpretg_watchdog_ctrl_reg.regValue = rtd_inl(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg);
        txpretg_watchdog_ctrl_reg.dout_force_bg = 1;
        rtd_outl(PPOVERLAY_TXPRETG_TXPRETG_watchdog_CTRL_reg, txpretg_watchdog_ctrl_reg.regValue);
        ScalerHdmiRepeaterLibSetTxAvMute(true);
        txpre_clear_onms5_status();
    }
}

static int txpre_onms_tsk(void *p)
{//This task is used to online measure error handler
    unsigned long flags;//for spin_lock_irqsave
    rtd_pr_hdmitx_notice("txpre_onms_tsk()\n");

    current->flags &= ~PF_NOFREEZE;
    while (1) {
        wait_event_freezable_timeout(TXPRE_ONMS_WAIT_QUEUE, txpre_online_triggle, 50);//Timeout is 500ms

        if(txpre_online_triggle) {
            spin_lock_irqsave(&TXPre_ONMS_Spinlock, flags);
            txpre_online_triggle = FALSE;
            spin_unlock_irqrestore(&TXPre_ONMS_Spinlock, flags);
            txpre_ctrl_muteon();
        }

        if (freezing(current)) {
            try_to_freeze();
        }

        if (kthread_should_stop()) {
            break;
        }
    }

    rtd_pr_hdmitx_notice("\r\n####txpre_onms_tsk: exit...####\n");
    do_exit(0);
    return 0;
}

static void create_txpre_onms_tsk(void)
{
    int err;
    if (txpre_onms_tsk_running_flag == FALSE) {
        p_txpre_onms_tsk = kthread_create(txpre_onms_tsk, NULL, "txpre_onms_tsk");

        if (p_txpre_onms_tsk) {
            wake_up_process(p_txpre_onms_tsk);
            txpre_onms_tsk_running_flag = TRUE;
        } else {
            err = PTR_ERR(p_txpre_onms_tsk);
            rtd_pr_hdmitx_err("Unable to start txpre_onms_tsk (err_id = %d)./n", err);
        }
    }
}

static void delete_txpre_onms_tsk(void)
{//delete txpre_onms_tsk
    int ret;
    if (txpre_onms_tsk_running_flag) {
        if(p_txpre_onms_tsk){
            ret = kthread_stop(p_txpre_onms_tsk);
            if (!ret) {
                p_txpre_onms_tsk = NULL;
                txpre_onms_tsk_running_flag = FALSE;
                rtd_pr_hdmitx_err("txpre_onms_tsk thread stopped\n");
            }
        }else{
            rtd_pr_hdmitx_err("txpre_onms_tsk thread stopped already\n");
        }
    }
}
// txpre online measure end ==============================================<

int hdmitx_init_module(void)
{
    int result;
    int devno;
    dev_t dev = 0;
#if 0//IS_ENABLED(CONFIG_RTK_KDRV_DC_MEMORY_TRASH_DETCTER)
    /******************DCMT register callback func in init flow******************/
    DCMT_DEBUG_INFO_REGISTER(sb4_dc2h_mdscpu, sb4_dc2h_debug_info);
    FatalMessageHDMITx("DCMT_DEBUG_INFO_REGISTER(sb4_dc2h_mdscpu, sb4_dc2h_debug_info)\n");
#endif
    FatalMessageHDMITx("\n\n\n\n *****************  hdmitx init module  *********************\n\n\n\n");
    if (hdmitx_major) {
        dev = MKDEV(hdmitx_major, hdmitx_minor);
        result = register_chrdev_region(dev, hdmitx_nr_devs, HDMITX_DEVICE_NAME);
    } else {
        result = alloc_chrdev_region(&dev, hdmitx_minor, hdmitx_nr_devs,HDMITX_DEVICE_NAME);
        hdmitx_major = MAJOR(dev);
    }
    if (result < 0) {
        FatalMessageHDMITx("hdmitx: can't get major %d\n", hdmitx_major);
        return result;
    }

    FatalMessageHDMITx("hdmitx init module major number = %d\n", hdmitx_major);

    hdmitx_class = class_create(THIS_MODULE,HDMITX_DEVICE_NAME);
    if (IS_ERR(hdmitx_class))
    {
        FatalMessageHDMITx("hdmitx: can not create class...\n");
        result = PTR_ERR(hdmitx_class);
        goto fail_class_create;
    }
    hdmitx_class->devnode = hdmitx_devnode;

    hdmitx_platform_devs = platform_device_register_simple(HDMITX_DEVICE_NAME, -1, NULL, 0);

    if((result=platform_driver_register(&hdmitx_device_driver)) != 0){
        FatalMessageHDMITx("hdmitx: can not register platform driver...\n");
        result = -ENOMEM;
        goto fail_platform_driver_register;
    }

    devno = MKDEV(hdmitx_major, hdmitx_minor);
    cdev_init(&hdmitx_cdev, &hdmitx_fops);
    hdmitx_cdev.owner = THIS_MODULE;
    hdmitx_cdev.ops = &hdmitx_fops;
    result = cdev_add (&hdmitx_cdev, devno, 1);
    if (result<0)
    {
        FatalMessageHDMITx("hdmitx: can not add character device...\n");
        goto fail_cdev_init;
    }
    device_create(hdmitx_class, NULL, MKDEV(hdmitx_major, 0), NULL, HDMITX_DEVICE_NAME);
    sema_init(&HDMITX_Semaphore, 1);
    sema_init(&HDMITXBlock_Semaphore, 1);

    init_waitqueue_head(&hdmitx_thd_wait_qu);

    FatalMessageHDMITx("[HDMITX] start create_hdmitx_tsk\n");
    hdmitx_sys_clock_reset();
    hdmitx_init();
    panel_enable_ddr_crash_hdmi_tx_forceBg();
    //hdmitx_AP_Init_event_handler(0);
    create_hdmitx_tsk();
    create_txpre_onms_tsk();
    return 0;
fail_cdev_init:
    platform_driver_unregister(&hdmitx_device_driver);
fail_platform_driver_register:
    platform_device_unregister(hdmitx_platform_devs);
    hdmitx_platform_devs = NULL;
    class_destroy(hdmitx_class);
fail_class_create:
    hdmitx_class = NULL;
    unregister_chrdev_region(hdmitx_devno, 1);
    return result;
}


void __exit hdmitx_cleanup_module(void)
{
    dev_t devno = MKDEV(hdmitx_major, hdmitx_minor);

    delete_hdmitx_tsk();
    delete_txpre_onms_tsk();
    hdmitx_sys_clock_disable();
    FatalMessageHDMITx("rtice clean module hdmitx_major = %d\n", hdmitx_major);
    device_destroy(hdmitx_class, MKDEV(hdmitx_major, 0));
    class_destroy(hdmitx_class);
    hdmitx_class = NULL;
    cdev_del(&hdmitx_cdev);
    /* device driver removal */
    if(hdmitx_platform_devs) {
        platform_device_unregister(hdmitx_platform_devs);
        hdmitx_platform_devs = NULL;
    }
    platform_driver_unregister(&hdmitx_device_driver);
    /* cleanup_module is never called if registering failed */
    unregister_chrdev_region(devno, hdmitx_nr_devs);
    return;
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(hdmitx_init_module);
module_exit(hdmitx_cleanup_module);
#endif

