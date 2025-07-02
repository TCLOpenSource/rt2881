

/*
 * Realtek MMC driver
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/mbus.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include  <linux/dmapool.h>
#include <linux/scatterlist.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/mmc/host.h>
#include <asm/unaligned.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/time.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

#include "cqhci.h"
#include "emmc51_reg.h"
#include <rbus/misc_reg.h>
#include "rtksdio.h"
#include "rtksd_ops.h"
#include <mach/platform.h>
#include <mach/pcbMgr.h>
#include <rbus/sb2_reg.h>
#include <rtk_kdriver/rtk_otp_region_api.h>
#include <rtk_kdriver/rtk-kdrv-common.h>

#include "rtk_mmc.h"
#include <rtk_kdriver/rtk_semaphore.h>


/************************************************************************
 *  Local Define
 ************************************************************************/
#define DRIVER_NAME "rtkemmc_dw"
#define BANNER      "Realtek eMMC Driver"
#define VERSION "$Id: rtkemmc_51.c RTK_2819A 2024-08-14 19:45 $"

#define EXT_CSD_ENH_START_ADDR          136     /* R/W, 4 bytes */
#define EXT_CSD_ENH_SIZE_MULT           140     /* R/W, 3 bytes */
#define EXT_CSD_WR_REL_SET              167     /* R/W ifHS_CTRL_REL=1 */


#ifndef CONFIG_MMC_RTKEMMC_HK_ATTR
#define CONFIG_MMC_RTKEMMC_HK_ATTR
#endif

// #if defined(CONFIG_RTKEMMC_HS200) || defined(CONFIG_RTKEMMC_HS200_MODULE)
// #define EN_EMMC_HS200
// #endif

// #if defined(CONFIG_RTKEMMC_HS400) || defined(CONFIG_RTKEMMC_HS400_MODULE)
// #define EN_EMMC_HS400
// #endif

/* #define EN_SHOW_DATA */

#define USE_FIXED_W_PHASE

/* rtk_emmc_bus_wid control flag*/
#define EMMC_DIS_50M_TUNE   BIT(14)

#define EMMC_EN_SCAN_TOOL   BIT(15)
#define EMMC_EN_PARAM       BIT(16)
#define EMMC_DIS_SSCG       BIT(17)
#define EMMC_EN_IO_DRV      BIT(18)

#define EMMC_FIX_DSTUNE     BIT(20)
#define EMMC_SKIP_TUNE      BIT(21)
#define EMMC_ALWAYS_CLK     BIT(22)
#define EMMC_BURST_BLK      BIT(23)

#define EMMC_RW_REG         BIT(24)
#define EMMC_RESTORE        BIT(25)
#define EMMC_RCV_FLOW       BIT(26)
#define EMMC_SHOW_CRC       BIT(27)

#define EMMC_TUNE_WPH       BIT(28)
#define EMMC_FIX_WPHASE     BIT(29)
#define EMMC_CHK_BUF        BIT(30)
#define EMMC_FIX_STRENGTH   BIT(31)


#define FIX_WPHASE50           BIT(0)
#define FIX_RPHASE50           BIT(1)
#define FIX_WPHASE200          BIT(2)
#define FIX_WPHASE400          BIT(3)
#define FIX_CPHASE200          BIT(4)
#define FIX_RPHASE400          BIT(5)
#define FIX_RPHASE200          BIT(6)

/* Low speed phase (<50M) */
#define MMC_LOW_SPEED_PHASE    0x64

//#define DEBUG_BUFFER

/*
 Beware!!! Only phase below are available (from DIC's suggestion)
 WPhase : 4~13 / 20~31
 RPhase : 3~12 / 20~28
 CPhase : 4~31
 DS     : 0~28
 */
#define VAILD_DEFAULT_WPHASE_ZONE   (0x7ffefffe)    //31:0 = [0111 1111 1111 1110 1111 1111 1111 1110]

#define VAILD_WPHASE_ZONE           (0xfff03ff0)    //31:0 = [1111 1111 1111 0000 0011 1111 1111 0000]
#define VAILD_RPHASE_ZONE           (0x0ff80ff8)    //31:0 = [0000 1111 1111 1000 0000 1111 1111 1000]
#define VAILD_CPHASE_ZONE           (0xfffffff0)    //31:0 = [1111 1111 1111 1111 1111 1111 1111 0000]
#define VAILD_DSTUNE_ZONE           (0x0fffffff)    //31:0 = [0000 1111 1111 1111 1111 1111 1111 1111]

/* default phase */
#define DEFAULT_R_PHASE     (6)
#define DEFAULT_C_PHASE     (18)
#define DEFAULT_W_PHASE     (18)

/* ===================================================================
* 1) MMC_DEMO_DFT => setting for demo board. If the vendor of mmc is unknow, then use this.
*        Only one set of parameters in the table can set this flag.
* 2) MMC_VEND_DFT => Specify vendor defaults. Only one set of the same provider can set this flag.
* 3) If there are multiple sets of parameters for the same supplier, the PNM field should be set.
* 4) The count of members of vendor_phase_tbl can't be over 32.
 =================================================================== */
#define MMC_DEMO_DFT        (BIT(0))
#define MMC_VEND_DFT        (BIT(1))
#define MMC_PD_ACCESS       (BIT(4))

vender_info_t vendor_phase_tbl[] = {
    { 0x11,  MMC_VEND_DFT,          /* KIOXIA, Pre-define access or default */
        "008GB1",    /* PNM[0:5] */
        18, 18,  6,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        18, 18,  6,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        //18, 18,  6, 12,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        10, 18, 26, 19,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        12, 25, 23, 19 },   /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x15,  MMC_VEND_DFT,           /* Samsung, Pre-define access or default */
        {0,  0,  0,  0,  0,  0},     /* PNM[0:5] */
        18, 18,  8,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        18, 18,  6,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        16, 16,  6, 12,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        16, 10,  6, 12 },   /* hs_400     -> cmd_w,dq_w,dq_r,ds */
    { 0x00,  MMC_DEMO_DFT,          /* Defalut as KIOXIA, Pre-define access or default */
        {0,  0,  0,  0,  0,  0},    /* PNM[0:5] */
        18, 18,  6,  0,     /* legacy_25M -> cmd_w,dq_w,dq_r,rev */
        18, 18,  6,  0,     /* hs_50      -> cmd_w,dq_w,dq_r,rev */
        18, 18,  6, 12,     /* hs_200     -> cmd_w,dq_w,dq_r,ds */
        18, 10,  6, 12 }    /* hs_400     -> cmd_w,dq_w,dq_r,ds */
};


static u32 rtk_fix_rphase = 0;

#define SET_FIX_WPHA400(x,y)    (y = ((y & ~0x3f) | (0xc0|x)))
#define SET_FIX_WPHA200(x,y)    (y = ((y & ~(0x3f<< 8)) | ((0xc0|x)<< 8)))
#define SET_FIX_RPHA400(x)      (rtk_fix_rphase = ((rtk_fix_rphase & ~0x3f) | (0xc0|x)))
#define SET_FIX_RPHA200(x)      (rtk_fix_rphase = ((rtk_fix_rphase & ~0x3f00) | ((0xc0|x)<<8)))
#define SET_FIX_CPHA200(x,y)    (y = ((y & ~(0x3f<<16)) | ((0xc0|x)<<16)))
#define SET_FIX_STRENGTH(x,y)   (y = ((y & ~(0xf <<24)) | (x<<24)))         //24~27

#define GET_FIX_WPHA400(y)      ((y & 0xc0) == 0xc0)?(y & 0x3f):0xff
#define GET_FIX_WPHA200(y)      (((y >> 8) & 0xc0) == 0xc0)?((y >>8) & 0x3f):0xff
#define GET_FIX_RPHA400()       ((rtk_fix_rphase & 0xc0) == 0xc0)?(rtk_fix_rphase & 0x3f):0xff
#define GET_FIX_RPHA200()       ((rtk_fix_rphase & 0xc000) == 0xc000) ? ((rtk_fix_rphase & 0x3f00)>>8):0xff

#define GET_FIX_CPHA200(y)      (((y >> 16) & 0xc0) == 0xc0)?((y >>16) & 0x3f):0xff
#define GET_FIX_STRENGTH(y)     ((y >>24) & 0xf)

#define SET_FIX_RPHA50(x,y)    (y = ((y & ~0x3f) | (0xc0|x)))
#define SET_FIX_WPHA50(x,y)    (y = ((y & ~(0x3f<< 8)) | ((0xc0|x)<< 8)))
#define GET_FIX_RPHA50(y)      ((y & 0xc0) == 0xc0)?(y & 0x3f):0xff
#define GET_FIX_WPHA50(y)      (((y >> 8) & 0xc0) == 0xc0)?((y >>8) & 0x3f):0xff

#define PAD_SET_CNT (7)

/* CMD23 by SW flow or Auto stop by IP */
//#define RTK_CMD23_USE
/* We check product name to determinate which read flow be used */
//#define AUTO_SEND_STOP
/* Directly ISR routine */

typedef u8 Phase_Arry[MAX_TUNING_STEP];

/************************************************************************
 * DWC spec:
 * Transfer Complete
 * This bit is set when a read/write transfer
 * and a command with status busy is completed.
 ************************************************************************/
#define NEED_WAIT_XFER(cmd_flag)    \
        (((cmd_flag & DATA_PRESENT_SEL)?1:0) || \
         ((cmd_flag & RESP_TYPE_SELECT_MASK) == RESP_LEN_48B))

#define IS_R1B_CMD(cmd_flag)    \
         ((cmd_flag & RESP_TYPE_SELECT_MASK) == RESP_LEN_48B)

#define IS_DATA_CMD(cmd_flag)    \
        ((cmd_flag & DATA_PRESENT_SEL)?1:0)

/************************************************************************
 *  Local veriable
 ************************************************************************/
static int maxfreq = 0;
static int nodma;
struct mmc_host *mmc_host_local;
#ifdef CONFIG_MMC_RTKEMMC_PLUS_MODULE
EXPORT_SYMBOL(mmc_host_local);
#endif
static u32 rtk_emmc_test_ctl =0;
static u32 rtk_emmc_log_state = 0;
static u32 rtk_fix_wphase = 0;
static unsigned long rtk_emmc_err_cnt = 0;
u32 rtk_emmc_raw_op;
#ifdef CONFIG_MMC_RTKEMMC_PLUS_MODULE
EXPORT_SYMBOL(rtk_emmc_raw_op);
#endif
static mmc_phase_t tuning_phase;
static u32 rtk_fix_50M_phase = 0;
static u32 fix_phase_flag = 0;

//static bool downspeed_test = false;
//static u8 HS200_fail_count = 0;

// debug
//static __attribute__((unused))bool HS400_fix_test = false;
//static u8 rtk_emmc_debug_log = 0;

/* for caculation about cmd finish time */
atomic64_t cmd_time_ns_old;
u32 cmd_record = 0;
u32 total_byte_record = 0;
u32 ext_tmout_ms = 0;
EXPORT_SYMBOL(ext_tmout_ms);

/************************************************************************
 *
 ************************************************************************/
#define PLL_CLK_CNT (10)
static const char *const dw_clk_tlb[PLL_CLK_CNT] = {
    "200Khz",
    "25Mhz",
    "50Mhz",
    "100Mhz",
    "150Mhz",
    "160Mhz",
    "170Mhz",
    "180Mhz",
    "190Mhz",
    "200Mhz"
};

/************************************************************************
 *
 ************************************************************************/
#define DIV_TBL_CNT (8)
static __attribute__((unused))
const char *const div_tlb[DIV_TBL_CNT] = {
	"2",
	"4",
	"8",
	"16",
	"200",
	"256",
	"512",
	"NONE"
};


/************************************************************************
 *  Local Function Protal Type
 ************************************************************************/
static u32 dwc51_int_act(u32 act,u32 event,u32 mode);
static void dw_em_set_IO_driving(struct rtksd_host *sdport, unsigned int type_sel);
static void rtkcr_display_version(void);
static void dw_em_request(struct mmc_host *host, struct mmc_request *mrq);
static int dw_em_get_ro(struct mmc_host *mmc);
static void dw_em_set_ios(struct mmc_host *host, struct mmc_ios *ios);
static int dw_em_execute_tuning(struct mmc_host *mmc, u32 opcode);
static int mmc_send_data_cmd(struct mmc_host * mmc, u32 hc_cmd,
                 u32 cmd_arg, u32 blk_cnt, unsigned char *buffer);
static void set_cmd_info(struct rtksd_host *sdport,struct mmc_command * cmd,
             struct sd_cmd_pkt *cmd_info, u32 opcode, u32 arg,
             u32 rsp_para);

//static int dw_em_stop_transmission(struct rtksd_host *sdport);
static u32 dw_em_get_cmd_timeout(struct sd_cmd_pkt *cmd_info);

static void dwc51_set_scrip_base(dma_addr_t scrip_base);
static void show_ext_csd(u8 *ext_csd);
static void dwc51_reset_crt(struct rtksd_host *sdport,u32 sync);
static void dw_em_hw_reset(struct mmc_host *host);
static int dw_em_reset_dwc51(struct rtksd_host *sdport,u8 event,u8 save_en);
static void dw_em_reset_host(struct rtksd_host *sdport,u8 save_en);
static int dwc51_reset_IP(struct rtksd_host *sdport,u8 event);
static void dwc51_set_div(struct rtksd_host *sdport,u32 em_div);
static void dwc51_IP_config(struct rtksd_host *sdport);
static int rtkem_clr_sta(struct rtksd_host *sdport);

int dw_em_prepare_hs400_tuning(struct mmc_host *host, struct mmc_ios *ios);
int dw_em_drive_strength (struct mmc_card *card,
        unsigned int max_dtr, int host_drv,
        int card_drv, int *drv_type);
static void dw_em_set_ds_delay(struct rtksd_host *sdport, int ds_delay);
static int dw_em_ds_tuning(struct mmc_host *host, u32 ctl);
static int dw_em_save_pad(struct rtksd_host *sdport,u32* pad_arry,int cnt);
static int dw_em_restore_pad(struct rtksd_host *sdport,u32* pad_arry);
static int rtkemmc_disable_int(struct rtksd_host * sdport);
static int rtkemmc_enable_int(struct rtksd_host * sdport);
static void rtk_init_card(struct mmc_host *host, struct mmc_card *card);
static int rtk_match_device(struct rtksd_host *sdport);

static void rtksd_request_done(struct rtksd_host *sdport);
static int dwc51_chk_xfer_done(struct sd_cmd_pkt *cmd_info);
static int dwc51_chk_err_tpye(struct sd_cmd_pkt *cmd_info);
static void rtksd_show_finish_status(struct rtksd_host *sdport);
static void rtksd_clear_event(struct rtksd_host *sdport);
static int rtksd_host_recovery(struct rtksd_host *sdport,u32 recover_mothod);
static void dwc51_set_timing(struct rtksd_host *sdport,u32 timing);
static int dwc51_set_bits(struct rtksd_host *sdport, u32 bus_width);
static int rtksd_send_command(struct rtksd_host *sdport, struct mmc_command *cmd);

#ifdef  EN_SHOW_DATA
static void show_data(u8 *buf, unsigned int len);
#endif

static
const struct mmc_host_ops rtkemmc_ops = {
    .request            = dw_em_request,
    .get_ro             = dw_em_get_ro,
    .set_ios            = dw_em_set_ios,
    .init_card          = rtk_init_card,
    .execute_tuning     = dw_em_execute_tuning,
    .hw_reset           = dw_em_hw_reset,
    .prepare_hs400_tuning   = dw_em_prepare_hs400_tuning,
    .hs400_downgrade    = mmc_execute_tuning_50,
    .hs400_complete     = mmc_execute_tuning_400,
    .select_drive_strength  = dw_em_drive_strength,
};

/************************************************************************
 *  external Function Protal Type
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
//extern s32 export_raw_dw_em_read(u8 *buffer, u64 address, u32 size);
//extern s32 export_raw_dw_em_write(u8 *buffer, u64 address, u32 size);
//extern s32 export_raw_dw_em_init(void);
//extern int export_raw_emmc_partition_change(u32 part_num);
#endif

/************************************************************************
 * Function Body
 ************************************************************************/
/************************************************************************
 * The value 'threshold' shoud be modify by device attr func later
 * FORCE_ERROR_INT:     force trigger normal error that assigned event.
 * FORCE_ERR_AUTO_CMD:  force trigger auto cmd error that assigned event.
 * FORCE_ERROR_RANDOM:  force trigger normal error event
 *                          that selected over random generator .
 ***********************************************************************/
//#define ERR_INJECT_ENABLE
#ifdef ERR_INJECT_ENABLE
#define FORCE_ERROR_INT     (1)     //FORCE_ERROR_INT_STAT
#define FORCE_ERR_AUTO_CMD  (2)     //FORCE_AUTO_CMD_STAT
#define FORCE_ERROR_RANDOM  (3)     //for normal error
static __attribute__((unused))
void dwc51_err_injecter(unsigned int threshold,
                 unsigned int type, unsigned event )
{
    int rand_value;

    get_random_bytes(&rand_value, sizeof(rand_value));
    if ((rand_value % 100) < threshold) {
        if(type == FORCE_ERROR_INT){
            em_outw(DWC51_FORCE_ERROR_INT_STAT_R16,event);
            EM_ALERT("FORCE_ERROR_INT=0x%08x!!!",event);
        }else if(type == FORCE_ERR_AUTO_CMD){
            em_outw(DWC51_FORCE_AUTO_CMD_STAT_R16,event);
            EM_ALERT("FORCE_AUTO_CMD=0x%08x!!!!!!!!!",event);
        }else if(type == FORCE_ERROR_RANDOM){
            get_random_bytes(&rand_value, sizeof(rand_value));
            rand_value = (rand_value % 10); //err bit 0~9
            rand_value = (BIT(0) << rand_value);
            em_outw(DWC51_FORCE_ERROR_INT_STAT_R16,rand_value);
            EM_ALERT("FORCE_ERROR_INT=0x%08x!!!!!!!!!",rand_value);
        }
    }

}
#endif

/************************************************************************
 *
 ***********************************************************************/
#ifdef EMMC_SHOW_REG
void dwc51_chk_reg(const char* func,unsigned int line)
{
    int i;
    unsigned int regbase;
    EM_INFO("%s(%d) call:\n",func,line);

    regbase = 0xb8010800;
    for(i=0; i<4; i++) {
        EM_INFO("0x%08x=0x%08x\n",(regbase + (i*4)), em_inl(regbase + (i*4)));
    }
    regbase = 0xb8010830;
    for(i=0; i<12; i++) {
        EM_INFO("0x%08x=0x%08x\n",(regbase + (i*4)), em_inl(regbase + (i*4)));
    }
    regbase = 0xb80108A0;
    for(i=0; i<24; i++) {
        EM_INFO("0x%08x=0x%08x\n",(regbase + (i*4)), em_inl(regbase + (i*4)));
    }
    regbase = 0xb8010900;
    for(i=0; i<0x40; i++) {
        EM_INFO("0x%08x=0x%08x\n",(regbase + (i*4)), em_inl(regbase + (i*4)));
    }

}
#else
#define dwc51_chk_reg(x,y)
#endif

/************************************************************************
 *
 ************************************************************************/
static
void rtk_init_card(struct mmc_host *host, struct mmc_card *card)
{
    struct rtksd_host *sdport = mmc_priv(host);

    mmcdbg("entry\n");

    sdport->mid = (unsigned char)(card->raw_cid[0] >> 24 & 0xFF);
    sdport->pnm[0] = (unsigned char)(card->raw_cid[0] & 0xFF);
    sdport->pnm[1] = (unsigned char)(card->raw_cid[1] >> 24 & 0xFF);
    sdport->pnm[2] = (unsigned char)(card->raw_cid[1] >> 16 & 0xFF);
    sdport->pnm[3] = (unsigned char)(card->raw_cid[1] >> 8 & 0xFF);
    sdport->pnm[4] = (unsigned char)(card->raw_cid[1] & 0xFF);
    sdport->pnm[5] = (unsigned char)(card->raw_cid[2] >> 24 & 0xFF);
    sdport->pnm[6] = 0;
    EM_INFO("mid: 0x%x Product name: \"%s\"[%x%x%x%x%x%x]\n",
                sdport->mid,sdport->pnm,
                sdport->pnm[0],sdport->pnm[1],sdport->pnm[2],
                sdport->pnm[3],sdport->pnm[4],sdport->pnm[5]);
    rtk_match_device(sdport);
}

/************************************************************************
 *
 ************************************************************************/
// bool get_mmc_tuning_50_done(struct mmc_host *host)
// {
// 	struct rtksd_host *sdport = mmc_priv(host);
// 	return sdport->hs50_tuned;
// }

/************************************************************************
 *
 ************************************************************************/
// void set_mmc_tuning_50_done(struct mmc_host *host,bool val)
// {
// 	struct rtksd_host *sdport = mmc_priv(host);
// 	sdport->hs50_tuned = val;
// }

/************************************************************************
 *
 ************************************************************************/
// bool get_mmc_tuning_200_done(struct mmc_host *host)
// {
// 	struct rtksd_host *sdport = mmc_priv(host);
// 	return sdport->hs200_tuned;
// }

/************************************************************************
 *
 ************************************************************************/
// void set_mmc_tuning_200_done(struct mmc_host *host,bool val)
// {
// 	struct rtksd_host *sdport = mmc_priv(host);
// 	sdport->hs200_tuned = val;
// }

/************************************************************************
 *
 ************************************************************************/
// bool get_mmc_tuning_400_done(struct mmc_host *host)
// {
// 	struct rtksd_host *sdport = mmc_priv(host);
// 	return sdport->hs400_tuned;
// }

/************************************************************************
 *
 ************************************************************************/
// void set_mmc_tuning_400_done(struct mmc_host *host,bool val)
// {
// 	struct rtksd_host *sdport = mmc_priv(host);

//     if(val)
//         set_bit(PORT_HS400_TUNED,&sdport->port_status);
//     else
//         clear_bit(PORT_HS400_TUNED,&sdport->port_status);
// }


/************************************************************************
 *
 ************************************************************************/
static
int dwc51_get_cpu_occupy(struct rtksd_host *sdport)
//int dw_em_get_occupy(struct rtksd_host *sdport)
{
    unsigned int reginfo;

    mmcdbg("entry\n");
    if(rtk_emmc_raw_op) {
        EM_ALERT("raw driver action! don't occupy SCPU sel\n");
        WARN(1,"%s: raw driver action! don't occupy SCPU sel\n",DRIVER_NAME);
        return -1;
    }
    reginfo = em_inl(EM51_SCPU_SEL);
    em_outl(EM51_SCPU_SEL, reginfo|SCPU_SEL );
    set_bit(PORT_OCCUPY,&sdport->port_status);

    return 0;
}

/************************************************************************
 *
 ************************************************************************/
static
int dwc51_release_cpu_occupy(struct rtksd_host *sdport)
//int dw_em_release_occupy(struct rtksd_host *sdport)
{
    /* release scpu */
    unsigned int reginfo;

    mmcdbg("entry\n");
    reginfo = em_inl(EM51_SCPU_SEL);
    if(rtk_emmc_raw_op) {
        em_outl(EM51_SCPU_SEL, reginfo & ~SCPU_SEL);    /* release SCPU */
        EM_ALERT("raw driver action! release SCPU sel\n");
        WARN(1,"%s: raw driver action! release SCPU sel\n",DRIVER_NAME);
        return 0;
    }

    if(reginfo & SCPU_SEL){
        if(test_bit(PORT_OCCUPY,&sdport->port_status)){
            em_outl(EM51_SCPU_SEL, reginfo & ~SCPU_SEL);    /* release SCPU */
        }else{
            /* for debug check */
            EM_ERR("call release but no occupy??\n");
        }
    }
    clear_bit(PORT_OCCUPY,&sdport->port_status);

    return 0;
}


/************************************************************************
 *
 ************************************************************************/
static
void dw_em_show_phase(void)
{
    u32 reg4,reg5;

    reg4 = (u32)em_inl(EMMC_PLLPHASE_CTRL);
    reg5 = (u32)em_inl(EMMC_PLL_PHASE_INTERPOLATION);

    EM_ALERT("Wphase=%d Rphase=%d Cphase=%d\n",
            (reg4&PHASE_W_MASK),
            ((reg4>>PHASE_R_SHT)&PHASE_R_MASK),
            ((reg5>>PHASE_C_SHT)&PHASE_C_MASK));

}

/************************************************************************
 * DWC51_RESP01_R:  39- 08 bits of the Response
 * DWC51_RESP23_R:  71- 40 bits of the Response
 * DWC51_RESP45_R: 103- 72 bits of the Response
 * DWC51_RESP67_R: 135-104 bits of the Response
 *
 * R1 = Device status, 39 - 8 bits of the Response.
 * R2 = CID/CSD, 127 - 1 bits of the Response.
 * So, In case of R2, the response data should be rearrange.
 ************************************************************************/
static
void dwc51_read_rsp( u32 * rsp, u32 reg_count )
//void dw_em_read_rsp( u32 * rsp, u32 reg_count )
{
//#define SHOW_RESP
    int i;
    volatile unsigned int rsp_tmp[5] = {0};

    if ( reg_count == 16 ){
        mmcdbg("reg_count=%d\n", reg_count);
        for(i=0;i<4;i++){
            rsp_tmp[i+1] = em_inl(DWC51_RESP01_R + (i*4));
        }
        rsp[0] = ((rsp_tmp[4]<<8)&0xffffff00) | ((rsp_tmp[3]>>24) & 0xff);
        rsp[1] = ((rsp_tmp[3]<<8)&0xffffff00) | ((rsp_tmp[2]>>24) & 0xff);
        rsp[2] = ((rsp_tmp[2]<<8)&0xffffff00) | ((rsp_tmp[1]>>24) & 0xff);
        rsp[3] = ((rsp_tmp[1]<<8)&0xffffff00);
    } else {
        mmcdbg("reg_count=%d\n", reg_count);
        rsp_tmp[0] = em_inl(DWC51_RESP01_R);
        /* For Auto CMD, the 32-bit response (bits 39-8 of the
            Response Field) is updated in the RESP67_R register. */
        if ( reg_count == 2 ){
            rsp_tmp[1] = em_inl(DWC51_RESP67_R);
        }
        for(i=0;i<4;i++){
            rsp[i] = rsp_tmp[i];
        }
    }
#ifdef SHOW_RESP
        EM_ERR("rsp[0]=0x%08x\n", rsp[0]);
        EM_ERR("rsp[1]=0x%08x\n", rsp[1]);
        EM_ERR("rsp[2]=0x%08x\n", rsp[2]);
        EM_ERR("rsp[3]=0x%08x\n", rsp[3]);
#endif
}

/************************************************************************
 *
 ************************************************************************/
/* X mode error */
#define RESP_X_MODD   ( BIT(31)|BIT(30)|BIT(27)|BIT(26)|BIT(24)|BIT(23)|  \
            BIT(22)|BIT(21)|BIT(19)|BIT(16)|BIT(15)|BIT(7) )

static const char *const sta_err_bit_tlb[32] = {
    "","","","","","","",           //0~6
    "SWITCH_ERROR[X] ",             //7
    "","","","","",                 //8~12
    "ERASE_RESET ",                 //13
    "",                             //14
    "WP_ERASE_SKIP[X] ",            //15
    "CID/CSD_OVERWRITE[X] ",        //16
    "","",                          //17~18
    "ERROR[X] ",                    //19
    "CC_ERROR ",                    //20
    "DEVICE_ECC_FAILED[X] ",        //21
    "ILLEGAL_COMMAND[Rx] ",         //22
    "COM_CRC_ERROR[Rx] ",           //23
    "LOCK_UNLOCK_FAILED[X] ",       //24
    "",                             //25
    "WP_VIOLATION[X] ",             //26
    "ERASE_PARAM[X] ",              //27
    "ERASE_SEQ_ERROR ",             //28
    "BLOCK_LEN_ERROR ",             //29
    "ADDRESS_MISALIGN[X] ",         //30
    "ADDRESS_OUT_OF_RANGE[X] "      //31
};


/************************************************************************
 * Note:
 *   [X] denote this error is X mode error bit.
 *   It's detected at pervious cmd.
 ************************************************************************/
static
int dw_em_show_rsp_err_reason(u32 err_reason)
{
    unsigned int err_bit;
    unsigned int i;

    mmcdbg("entry\n");
    err_bit = (err_reason & RESP_ERR_BIT);
    if(err_bit){
        for(i=0; i<32; i++){
            if((err_bit >> i) & 0x01U){
                EM_ERR("resp err: %s\n",sta_err_bit_tlb[i]);
            }
        }
    }

    return 0;
}


/************************************************************************
 *
 ************************************************************************/
static
int dwc51_chk_rsp(struct sd_cmd_pkt * cmd_info)
//int dw_em_chk_rsp(u32 * rsp, u32 cmd_flag, struct sd_cmd_pkt * cmd_info)
{
    int ret_err = 0;
    u32 cmd_flag = 0;
    u32 xfermode = 0;
    u32 cmd_idx = 0;
    u32 *rsp = cmd_info->cmd->resp;

    mmcdbg("entry\n");
    cmd_flag = cmd_info->cmd_flag;
    xfermode = cmd_info->xfermode;

    cmd_idx = CMD_INDEX_GET(cmd_flag);

    if(RESP_TYPE_SELECT_GET(cmd_flag) == RESP_LEN_136 ){
        dwc51_read_rsp(rsp, 16);
        /* only resp == 6 is possible have card status */

    }else{
        if(xfermode & MULTI_BLK_SEL){
            if(xfermode & AUTO_CMD_ENABLE_MASK){
                dwc51_read_rsp(rsp, 2);
            }else{
                dwc51_read_rsp(rsp, 6);
            }
        }else{
            dwc51_read_rsp(rsp, 6);
        }
        if( (cmd_flag & CMD_CRC_CHK_ENABLE) |
            (cmd_flag & CMD_IDX_CHK_ENABLE) )
        {
            if( (cmd_idx != MMC_GO_IRQ_STATE)){
                /*
                 Only R1/R1b should check Card_Status.
                 It can't diff R1/R1b/R5/R6/R7.
                 but only CMD40 have definded, so check it here.
                */
                if(rsp[0] & RESP_ERR_BIT){
                    EM_ERR("response[0] alert err bit! (0x%08x)",rsp[0]);
                    ret_err = BIT(0);
                }
                /* how to check resp of auto cmd? */
                if(xfermode & AUTO_CMD_ENABLE_MASK){
                    if(rsp[1] & RESP_ERR_BIT){
                        EM_ERR("response[1] alert err bit! (0x%08x)",rsp[1]);
                        ret_err |= BIT(1);
                    }
                }
            }
        }
    }
    if(ret_err){
        /* response will be check by high-level, print information just for debug. */
        EM_ERR("rsp error !!![cmd%u]\n",cmd_idx);
        EM_ERR("rsp[0]=0x%08x\n",rsp[0]);
        dw_em_show_rsp_err_reason(rsp[0]);
        if(xfermode & (AUTO_CMD12_ENABLED|AUTO_CMD23_ENABLED)){
            EM_ERR("rsp[1]=0x%08x\n",rsp[1]);
            dw_em_show_rsp_err_reason(rsp[1]);
        }
    }
    return ret_err;
}

/************************************************************************
 *   0x0 :  50ohm x1
 *   0x1 :  33ohm x1.5
 *   0x2 :  66ohm x0.75
 *   0x3 : 100ohm x0.5
 *   0x4 :  40ohm x1.2
 ************************************************************************/
int dw_em_drive_strength (struct mmc_card *card,
        unsigned int max_dtr, int host_drv,
        int card_drv, int *drv_type)
{
    struct rtksd_host *sdport = NULL;
    int drv_stg;

    sdport = mmc_priv(card->host);

    /* according to Scott's advice, use 1.2(0x4) */
    if(max_dtr > 52000000){
        if(rtk_emmc_test_ctl & EMMC_FIX_STRENGTH){
            drv_stg = GET_FIX_STRENGTH(rtk_fix_wphase);
            EM_ALERT("bootcode set device strength : %d\n",drv_stg);
        }else{
						drv_stg = 0x4;
        }
    }else{
        drv_stg = 0;
    }

    EM_INFO("set drive strength to %d(mid=0x%x)\n",drv_stg,sdport->mid);
    return drv_stg;
}

/************************************************************************
 *
 ************************************************************************/
int dw_em_prepare_hs400_tuning(struct mmc_host *host, struct mmc_ios *ios)
{
    struct rtksd_host *sdport;
    u32 ds_tune;

    sdport = mmc_priv(host);
    if (test_bit(PORT_SKIP_DSTUNE,&sdport->port_status)) {
        ds_tune = sdport->pre_ds_tune;
        mmcmsg1("load previous ds=%d\n",ds_tune);
    } else {
        ds_tune = sdport->vender_phase_ptr->HS400_ds;
        mmcmsg1("load default ds=%d\n",ds_tune);
    }
    dw_em_set_ds_delay(sdport, ds_tune);

    return 0;
}

/************************************************************************
 * need ask DIC provide vaild table
 ************************************************************************/
static
int dw_em_chk_phase_vaild(u32 phase,u32 vaild){
    if(vaild & (0x01UL<<phase)){
        return 1;
    }
    return 0;
}

/************************************************************************
 *
 ************************************************************************/
#define GET_REG_RPHASE(x)   ((x >> 10) & 0x3f)
#define GET_REG_WPHASE(x)   (x & 0x3f)
#define GET_REG_CPHASE(x)   ((x >> 10) & 0x3f)

static
int dw_em_init_phase(struct rtksd_host *sdport, u32 w_phase,u32 r_phase,u32 c_phase)
{
    /*
     eMMC PLL Phase:
          setting on romcode\src\arch\reset\drivers\crt\crt.c +239

      PLL27X_REG_EMMC_PLLPHASE_CTRL_reg[ 5: 0]==> clk vs data write phase
      PLL27X_REG_EMMC_PLLPHASE_CTRL_reg[15:10]==> clk vs data read phase
      PLL27X_REG_EMMC_PLL_PHASE_INTERPOLATION_reg[15:10]==> clk vs cmd  write phase
    */
    u32 reg1 = 0;
    u32 reg2 = 0;

    mmcmsg1("w_phase=%u r_phase=%u c_phase=%u\n",w_phase,r_phase,c_phase);

    if ((w_phase != sdport->cur_w_phase) ||
        (r_phase != sdport->cur_r_phase)){
        reg1 = em_inl(EMMC_PLLPHASE_CTRL);
        reg1 &= ~((PHASE_R_MASK<<PHASE_R_SHT) | PHASE_W_MASK);
        reg1 |= ((r_phase << PHASE_R_SHT) | w_phase);

        em_outl(EMMC_PLLPHASE_CTRL,reg1);
        reg1 =  em_inl(EMMC_PLLPHASE_CTRL);
        sdport->cur_w_phase = GET_REG_WPHASE(reg1);
        sdport->cur_r_phase = GET_REG_RPHASE(reg1);
    }
    if(c_phase != sdport->cur_c_phase){
        em_maskl(EMMC_PLL_PHASE_INTERPOLATION, PHASE_C_MASK, PHASE_C_SHT ,c_phase);
        reg2 = em_inl(EMMC_PLL_PHASE_INTERPOLATION);
        sdport->cur_c_phase =  GET_REG_CPHASE(reg2);
    }

    mmcmsg1("w_phase=%u r_phase=%u c_phase=%u\n",
        sdport->cur_w_phase,sdport->cur_r_phase,sdport->cur_c_phase);
     return 0;

}

/************************************************************************
 *
 ************************************************************************/
__attribute__((unused))
static
int dw_em_set_PLL_SSC(struct rtksd_host *sdport,
    u32 ncode ,u32 fcode ,u32 rang)
{
    int ret;
    unsigned int loop;

    ret = -1;

    EM_INFO("%s: Going to enable SSC!!!\n",DRIVER_NAME);
    em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_EN_MASK, EMMC_PLL_SSC0_OC_EN_SHIFT ,0);       /* PLLEMMC_OC_EN = 0 */

    em_maskl(EMMC_PLL_SSC3, EMMC_PLL_SSC3_NCODE_MASK, EMMC_PLL_SSC3_NCODE_SHIFT,ncode);   /* Ncode_ssc = veriable */
    em_maskl(EMMC_PLL_SSC3, EMMC_PLL_SSC3_FCODE_MASK, EMMC_PLL_SSC3_FCODE_SHIFT,fcode);   /* Fcode_ssc = veriable */
    em_maskl(EMMC_PLL_SSC5, EMMC_PLL_SSC5_DOT_GRAN_MASK, EMMC_PLL_SSC5_DOT_GRAN_SHIFT,4);         /* dot_gran=4 */
    em_maskl(EMMC_PLL_SSC5, EMMC_PLL_SSC5_GRAN_EST_MASK, EMMC_PLL_SSC5_GRAN_EST_SHIFT,rang);   /* Gran_est=49166(30k) */

    em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_EN_MASK, EMMC_PLL_SSC0_OC_EN_SHIFT ,1);       /* PLLEMMC_OC_EN = 1 */

    loop = 200000;
    while(loop--){  /* polling PLLEMMC_Oc_done flag */
        if(em_inl(EMMC_PLL_SSC6) & BIT(EMMC_PLL_SSC6_OC_DONE_SHIFT)){
            break;
        }
    }

    em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_EN_MASK, EMMC_PLL_SSC0_OC_EN_SHIFT ,0);       /* PLLEMMC_OC_EN = 0 */
    if(loop){
        em_maskl(EMMC_PLL_SSC5, EMMC_PLL_SSC5_EN_SSC_MASK, EMMC_PLL_SSC5_EN_SSC_SHIFT ,1);   /* En_ssc =1 */
        ret = 0;
    }else{
        EM_ERR("SSC enable fail!!!\n");
    }

    return ret;
}

/************************************************************************
 *
 Available phase margin
    Read    3~12 / 20~28
    Write   4~13 / 20~31
    Cmd     4~31
    DS      0~27

 typedef struct{
    u32                 ctl;
    u8                  ncode_t;
    u8                  sdiv2;
    u8                  r_phase;
    u8                  w_phase;
    u8                  icp;
    u8                  pi_isel;
    u16                 ncode;
    u16                 fcode;
    u32                 range;
}  mmc_param_t;
 ************************************************************************/
mmc_param_t mmc_defaul_tparam[16]= {
/*  ctl/ncode_t/sdiv2/r_phase/w_phase/icp/pi_isel/ncode_ssc/fcode_ssc/range_ssc              */
    { 1,    40,    0,    0xff,   0xff,  2,     3,    0x27,    0x11E,   0x5DAA }, /* 193.5Mhz SSC: 2% 30KHz */
    { 0,    39,    0,    0xff,   0xff,  2,     3,       0,        0,        0 }, /* 189Mhz   */
    { 0,    37,    0,    0xff,   0xff,  2,     3,       0,        0,        0 }, /* 180Mhz   */
    { 0,    35,    0,    0xff,   0xff,  1,     3,       0,        0,        0 }, /* 171Mhz   */
    { 0,    33,    0,    0xff,   0xff,  1,     3,       0,        0,        0 }, /* 162Mhz   */
    { 0,    30,    0,    0xff,   0xff,  1,     2,       0,        0,        0 }, /* 148.5Mhz */
    { 0,    28,    0,    0xff,   0xff,  1,     2,       0,        0,        0 }, /* 139.5Mhz */
    { 0,    26,    0,    0xff,   0xff,  1,     2,       0,        0,        0 }, /* 130.5Mhz */
    { 0,    24,    0,    0xff,   0xff,  1,     2,       0,        0,        0 }, /* 121.5Mhz */
    { 0,    21,    0,    0xff,   0xff,  1,     2,       0,        0,        0 }, /* 108Mhz   */
    { 0,    41,    1,    0xff,   0xff,  2,     3,       0,        0,        0 }, /* 99Mhz    */
    { 0,    37,    1,    0xff,   0xff,  2,     3,       0,        0,        0 }, /* 90Mhz    */
    { 0,    33,    1,    0xff,   0xff,  1,     3,       0,        0,        0 }, /* 81Mhz    */
    { 0,    29,    1,    0xff,   0xff,  1,     2,       0,        0,        0 }, /* 72Mhz    */
    { 0,    23,    1,    0xff,   0xff,  1,     2,       0,        0,        0 }, /* 58.5Mhz  */
    { 0,    19,    1,    0xff,   0xff,  0,     1,       0,        0,        0 }  /* 49.5Mhz include under */
};

/************************************************************************
 *
 * PLL Clock Formula:
 *
 *   PLLEMMC_NCODE_T = 0x1800_0720[23:16]
 *   PLLSN           = 0x1800_0700[10:8] <==1(currently)
 *   Note : max PLLSN is 4
 *   PLLSDIV2        = 0x1800_0700[17]   <==0(currently)
 *   Fout = 27 *( PLLEMMC_NCODE_T +3)/ (PLLSN+2)/( PLLSDIV2+1)/2
 *        = 27 *( PLLEMMC_NCODE_T +3)/ 6
 *   PLLEMMC_NCODE_T = (((6 * Fout )/27)-3)
 *
 * if PLLSN is 4
 *   PLLEMMC_NCODE_T = (((12 * Fout )/27)-3)
 *
 *                      PLLSN=  [1]    [4]
 * 200Mhz : 0x1800_0720[23:16] = 41     85
 * 190Mhz : 0x1800_0720[23:16] = 39 81
 * 180Mhz : 0x1800_0720[23:16] = 37 77
 * 170Mhz : 0x1800_0720[23:16] = 35 72
 * 160Mhz : 0x1800_0720[23:16] = 32 68
 * 150Mhz : 0x1800_0720[23:16] = 30 63
 * 140Mhz : 0x1800_0720[23:16] = 28 59
 * 130Mhz : 0x1800_0720[23:16] = 26 54
 * 120Mhz : 0x1800_0720[23:16] = 24 50
 * 110Mhz : 0x1800_0720[23:16] = 21 45
 * 100Mhz : 0x1800_0720[23:16] = 19 41
 *  90Mhz : 0x1800_0720[23:16] = 17 37
 *  80Mhz : 0x1800_0720[23:16] = 15 32
 *  70Mhz : 0x1800_0720[23:16] = 12 28
 *  60Mhz : 0x1800_0720[23:16] = 10 23
 *  50Mhz : 0x1800_0720[23:16] =  8 19
 *
 * ICP setting :
 * under 50Mhz =>                 => ICP=? => 0x18000700[16:12]=0x00
 * 50,60,70Mhz => VCO=100,120,140 => ICP=5 => 0x18000700[16:12]=0x01
 * 80,90,100,110, 120,130,140Mhz
 *             => VCO=160,180,200,220,240,260,280
 *                                => ICP=10 => 0x18000700[16:12]=0x03
 * 150,160,170,180,190,200Mhz
 *             => VCO=300,320,340,360,380,400
 *                                => ICP=20 => 0x18000700[16:12]=0x07
 *
 ************************************************************************/
#define SSC_EN BIT(EMMC_PLL_SSC5_EN_SSC_SHIFT)
static
int dw_em_set_PLL_clk(struct rtksd_host *sdport, u32 targ_clk,u32 clk_div,u32 init)
{
    int err = 0;
    unsigned int pllemmc_ncode_t = 0;
    unsigned int loop = 20000;
    unsigned int icp = 0;
    unsigned int sdiv2 = 0;
    unsigned int sdiv2_old = 0;
    unsigned int pi_isel = 0;
    unsigned int r_phase = 0;
    unsigned int pll_sta = 0;
    unsigned int ncode_ssc = 0;
    unsigned int fcode_ssc = 0;
    unsigned int ssc_rang = 0;
    unsigned char idx;
    mmc_param_t *tmp_param_ptr;

    mmcmsg1("targ_clk = %d\n",targ_clk);
    if (targ_clk >= CLK_200Mhz){
        idx = 0;
    }else if (targ_clk >= CLK_190Mhz){
        idx = 1;
    }else if (targ_clk >= CLK_180Mhz){
        idx = 2;
    }else if (targ_clk >= CLK_170Mhz){
        idx = 3;
    }else if (targ_clk >= CLK_160Mhz){
        idx = 4;
    }else if (targ_clk >= CLK_150Mhz){
        idx = 5;
    }else if (targ_clk >= CLK_140Mhz){
        idx = 6;
    }else if (targ_clk >= CLK_130Mhz){
        idx = 7;
    }else if (targ_clk >= CLK_120Mhz){
        idx = 8;
    }else if (targ_clk >= CLK_110Mhz){
        idx = 9;
    }else if (targ_clk >= CLK_100Mhz){
        idx = 10;
    }else if (targ_clk >= CLK_90Mhz){
        idx = 11;
    }else if (targ_clk >= CLK_80Mhz){
        idx = 12;
    }else if (targ_clk >= CLK_70Mhz){
        idx = 13;
    }else if (targ_clk >= CLK_60Mhz){
        idx = 14;
    }else {
        idx = 15;
    }

    if(rtk_emmc_test_ctl & EMMC_EN_PARAM){
        EM_ALERT("%s(%d)Use Bootcode pre-setting parameter\n",__func__,__LINE__);
        tmp_param_ptr = sdport->mmc_boot_param;
    }else {

        tmp_param_ptr = mmc_defaul_tparam;
    }

    pllemmc_ncode_t = tmp_param_ptr[idx].ncode_t;
    sdiv2           = tmp_param_ptr[idx].sdiv2;
    r_phase         = tmp_param_ptr[idx].r_phase;
    icp             = tmp_param_ptr[idx].icp;
    pi_isel         = tmp_param_ptr[idx].pi_isel;

    ncode_ssc = tmp_param_ptr[idx].ncode_ssc;
    fcode_ssc = tmp_param_ptr[idx].fcode_ssc;
    ssc_rang  = tmp_param_ptr[idx].range_ssc;

    /* disable SSCG func by pre-setting of bootcode */
    if(rtk_emmc_test_ctl & EMMC_DIS_SSCG){
        pll_sta &= ~SSC_EN;
    }else{
        if(tmp_param_ptr[idx].ctl & MMC_EN_SSCG)
            pll_sta |= SSC_EN;
    }

    if(rtk_emmc_test_ctl & EMMC_EN_PARAM){
        EM_ALERT("%s(%d)idx%d==================================\n",__func__,__LINE__,idx);
        EM_ALERT("    pllemmc_ncode_t=%u(0x%x)\n",pllemmc_ncode_t,pllemmc_ncode_t);
        EM_ALERT("    r_phase=%u(0x%x)\n",r_phase,r_phase);
        EM_ALERT("    icp=%u(0x%x)\n",icp,icp);

        if(pll_sta & SSC_EN){
            EM_ALERT("SSCG enable !!!\n");
            EM_ALERT("    ncode_ssc=%u(0x%x)\n",ncode_ssc,ncode_ssc);
            EM_ALERT("    fcode_ssc=%u(0x%x)\n",fcode_ssc,fcode_ssc);
            EM_ALERT("    ssc_rang=%u(0x%x)\n",ssc_rang,ssc_rang);
        }
    }

    if(init){
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_ALL_RSTB_MASK, EMMC_PLL_SET_ALL_RSTB_SHFIT ,0);    /* PLLALLRSTB = 0 */
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_CKSSC_INV_MASK, EMMC_PLL_SET_CKSSC_INV_SHFIT ,0);   /* PLL_EMMC_CKSSC_INV = 0 */
        em_maskl(EMMC_PLL_SSC0,EMMC_PLL_SSC0_OC_STEP_MASK, EMMC_PLL_SSC0_OC_STEP_SHIFT ,1);   /* PLLEMMC_OC_STEP_SET = 1 */
        em_maskl(EMMC_PLL_CTRL,EMMC_PLL_CTRL_FREEZE_MASK, EMMC_PLL_CTRL_FREEZE_SHFIT ,1);  /* PLLEMMC_FREEZE */
        em_maskl(EMMC_PLL_CTRL,EMMC_PLL_CTRL_RSVD_L_MASK, EMMC_PLL_CTRL_RSVD_L_SHFIT ,0);   /* LDO RSVD[0]=0; RSVD[0]=0x18000704[8] */
        em_maskl(EMMC_PLL_CTRL,EMMC_PLL_CTRL_RSVD_H_MASK, EMMC_PLL_CTRL_RSVD_H_SHFIT ,1);  /* LDO RSVD[5:4]=1; RSVD[5:4]=0x18000704[13:12] */

        em_maskl(EMMC_PLL_CTRL, EMMC_PLL_CTRL_POWER_MASK, EMMC_PLL_CTRL_POWER_SHFIT ,2);   /* LDO power on, PLLEMMC power on */
        em_maskl(EMMC_PLL_SSC2, EMMC_PLL_SSC2_EN_PI_DEBUG_MASK, EMMC_PLL_SSC2_EN_PI_DEBUG_SHIFT ,1);   /* En_pi_debug, relative PI_CUR_SEL*/
        em_maskl(EMMC_PLL_SSC4, EMMC_PLL_SSC4_NCODE_MASK, EMMC_PLL_SSC4_NCODE_SHIFT,pllemmc_ncode_t); /* To setting PLLEMMC_NCODE_T */
        em_maskl(EMMC_PLL_SSC5, EMMC_PLL_SSC5_EN_SSC_MASK, EMMC_PLL_SSC5_EN_SSC_SHIFT ,0);   /* En_ssc =0; disable SSC */

        udelay(5);

        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSN_MASK, EMMC_PLL_SET_PLLSN_SHFIT ,1);    /* PLLSN = 1, */
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSCP_MASK, EMMC_PLL_SET_PLLSCP_SHFIT,0);    /* CP */
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSCS_MASK, EMMC_PLL_SET_PLLSCS_SHFIT,3);    /* CS */
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSSI_MASK, EMMC_PLL_SET_PLLSSI_SHFIT,icp);  /* PLL Charger Pump Current IchDpll[16:12] (PLLSSI) */
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSDIV2_MASK, EMMC_PLL_SET_PLLSDIV2_SHFIT,sdiv2);    /* PLLSDIV2 = 0, 27x(19+3)/3/2=99MHz */
        em_maskl(EMMC_PLL_SSC1, EMMC_PLL_SSC1_PI_CUR_SEL_MASK, EMMC_PLL_SSC1_PI_CUR_SEL_SHIFT,pi_isel);  /* PI_CUR_SEL */
        udelay(5);

        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_ALL_RSTB_MASK, EMMC_PLL_SET_ALL_RSTB_SHFIT ,1);    /* PLLALLRSTB = 1 */
        em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_EN_MASK, EMMC_PLL_SSC0_OC_EN_SHIFT ,1);   /* PLLEMMC_OC_EN = 1 */
    }else{
        sdiv2_old = (em_inl(EMMC_PLL_SET)>>EMMC_PLL_SET_PLLSDIV2_SHFIT) & EMMC_PLL_SET_PLLSDIV2_MASK;
        em_maskl(EMMC_PLL_CTRL, EMMC_PLL_CTRL_FREEZE_MASK, EMMC_PLL_CTRL_FREEZE_SHFIT ,1);  /* PLLEMMC_FREEZE */
        em_maskl(EMMC_PLL_SSC5, EMMC_PLL_SSC5_EN_SSC_MASK, EMMC_PLL_SSC5_EN_SSC_SHIFT ,0);   /* En_ssc =0; disable SSC */
        em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_STEP_MASK, EMMC_PLL_SSC0_OC_STEP_SHIFT ,0x200);   /* PLLEMMC_OC_STEP_SET = 0x200 */
        em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_EN_MASK, EMMC_PLL_SSC0_OC_EN_SHIFT ,0);   /* PLLEMMC_OC_EN = 0 */
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSN_MASK, EMMC_PLL_SET_PLLSN_SHFIT ,1);    /* PLLSN = 1, */
        em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSSI_MASK, EMMC_PLL_SET_PLLSSI_SHFIT,icp);  /* PLL Charger Pump Current IchDpll[16:12] (PLLSSI) */
        em_maskl(EMMC_PLL_SSC1, EMMC_PLL_SSC1_PI_CUR_SEL_MASK, EMMC_PLL_SSC1_PI_CUR_SEL_SHIFT,pi_isel);  /* PI_CUR_SEL */
        if (sdiv2 > sdiv2_old) {
            /* high --> low */
            em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSDIV2_MASK, EMMC_PLL_SET_PLLSDIV2_SHFIT,sdiv2);    /* PLLSDIV2 = 0, 27x(19+3)/3/2=99MHz */
            em_maskl(EMMC_PLL_SSC4, EMMC_PLL_SSC4_NCODE_MASK, EMMC_PLL_SSC4_NCODE_SHIFT,pllemmc_ncode_t); /* To setting PLLEMMC_NCODE_T */
        }else{
            /* low --> high */
            em_maskl(EMMC_PLL_SSC4, EMMC_PLL_SSC4_NCODE_MASK, EMMC_PLL_SSC4_NCODE_SHIFT,pllemmc_ncode_t); /* To setting PLLEMMC_NCODE_T */
            em_maskl(EMMC_PLL_SET, EMMC_PLL_SET_PLLSDIV2_MASK, EMMC_PLL_SET_PLLSDIV2_SHFIT,sdiv2);    /* PLLSDIV2 = 0, 27x(19+3)/3/2=99MHz */
        }
        em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_EN_MASK, EMMC_PLL_SSC0_OC_EN_SHIFT ,1);   /* PLLEMMC_OC_EN = 1 */
    }

    while(--loop){  /* polling PLLEMMC_Oc_done flag */
        if(em_inl(EMMC_PLL_SSC6) & BIT(EMMC_PLL_SSC6_OC_DONE_SHIFT)){
            break;
        }
        /* max waiting for 100ms = 20000*5us */
        udelay(5);
    }

    em_maskl(EMMC_PLL_SSC0, EMMC_PLL_SSC0_OC_EN_MASK, EMMC_PLL_SSC0_OC_EN_SHIFT ,0);   /* PLLEMMC_OC_EN = 0 */
    udelay(150);
    em_maskl(EMMC_PLL_CTRL, EMMC_PLL_CTRL_FREEZE_MASK, EMMC_PLL_CTRL_FREEZE_SHFIT ,0);  /* release PLLEMMC_FREEZE */

    if(!loop){
        err = -1;
        goto OUT;
    }

    /* To enable SSC, just care about PLL clock, no matter divider or else. */
    if(pll_sta & SSC_EN){
        dw_em_set_PLL_SSC(sdport, ncode_ssc, fcode_ssc ,ssc_rang);
    }

    sdport->cur_PLL = targ_clk;

OUT:

    return err;

}


/************************************************************************
 *  Notice: em_div must be even value.
 ************************************************************************/
static
void dwc51_set_div(struct rtksd_host *sdport,u32 em_div)
//void dw_em_set_div(struct rtksd_host *sdport,u32 em_div)
{
    u32 reginfo = 0;
    u32 low_speed = 0;

    // if(sdport->cur_div == em_div){
    //     mmcdbg("clock div is no change skip it 0x%x\n",em_div);
    //     return;
    // }
    //sdport->cur_div = em_div;   // it sets in tail. , move it out

    mmcmsg1("clock div %s(%d)\n",
        (em_div < DIV_TBL_CNT)? div_tlb[em_div]: "unknown",em_div);

    reginfo = em_inl(EM51_CLKGEN_CTRL) & ~EMMC_CLKGEN_DIV_SEL_MASK;
    if(em_div < PLL_DIV_NON){
        reginfo = (EMMC_CLKGEN_DIV_SEL_SET(reginfo,em_div) | EMMC_CLKGEN_CLK_DIV_EN);
    }else{
        //EMMC_CLKGEN_DIV_SEL_SET(reginfo,0);
        reginfo &= ~(EMMC_CLKGEN_CLK_DIV_EN|EMMC_CLKGEN_DIV_SEL_MASK);
    }
    mmcmsg1("%s(%d)reginfo=0x%08x\n", __func__,__LINE__, reginfo);
    em_outl(EM51_CLKGEN_CTRL,reginfo);

    /* low speed phase function */
    low_speed = em_inl(EM51_CCLKOUT_CTRL);
    low_speed &= ~(CCLK_OUT_DELAY_EN|CCLK_OUT_DELAY_COUNT_MASK);
    if((em_div >= PLL_DIV_200) && (em_div < PLL_DIV_NON)){
        unsigned int tmp_low_speed_phase = 100;

        mmcmsg1("low_speed=0x%08x\n",low_speed);
        low_speed |= (CCLK_OUT_DELAY_EN | tmp_low_speed_phase);
        em_outl(EM51_CCLKOUT_CTRL,low_speed);
    }else{
        mmcmsg1("clear low_speed_phase\n");
        em_outl(EM51_CCLKOUT_CTRL,low_speed);
    }

    sdport->cur_div = em_div;

    return;
}

/************************************************************************
 *
 ************************************************************************/
static
void dw_em_select_phase (struct rtksd_host *sdport) {
    u32 r_phase = 0;
    u32 w_phase = 0;
    u32 c_phase = 0;
    u32 d_strobe = 0;

    if (sdport->cur_timing == MMC_TIMING_MMC_HS400) {

        if (fix_phase_flag & FIX_RPHASE400) {
            r_phase = GET_FIX_RPHA400();
            EM_ALERT("%s(%d)Use Bootcode pre-setting parameter\n",__func__,__LINE__);
            EM_ALERT("Set HS400 cmd read phase: %d\n", r_phase);
        } else if (test_bit(PORT_HS200_TUNED,&sdport->port_status)) {
            /* If clock get from PLL directly ,use new phase setting clock. */
            r_phase = sdport->tud_r_pha200;
        } else {
            r_phase = sdport->vender_phase_ptr->HS400_dq_r;
        }
        w_phase = sdport->vender_phase_ptr->HS400_dq_w;
        c_phase = sdport->vender_phase_ptr->HS400_cmd_w;
        d_strobe = sdport->vender_phase_ptr->HS400_ds;
        dw_em_set_ds_delay(sdport, d_strobe);
        mmcmsg1("cmd_wrphase=%d dq_wphase=%d dq_rphase=%d d_strobe=%d\n",
            c_phase,w_phase,r_phase,d_strobe);

    } else if (sdport->cur_timing ==  MMC_TIMING_MMC_HS200) {
        if (fix_phase_flag & FIX_RPHASE200) {
            r_phase = GET_FIX_RPHA200();
            EM_ALERT("%s(%d)Use Bootcode pre-setting parameter\n",__func__,__LINE__);
            EM_ALERT("Set HS200 read phase: %d\n", r_phase);
        } else {
            mmcmsg1("\n");
            r_phase = sdport->vender_phase_ptr->HS200_dq_r;
        }
        w_phase = sdport->vender_phase_ptr->HS200_dq_w;
        c_phase = sdport->vender_phase_ptr->HS200_cmd_w;
        d_strobe = sdport->vender_phase_ptr->HS200_ds;
        mmcmsg1("cmd_wrphase=%d dq_wphase=%d dq_rphase=%d d_strobe=%d\n",
            c_phase,w_phase,r_phase,d_strobe);
    }else if((sdport->cur_timing ==  MMC_TIMING_MMC_HS) ||
             (sdport->cur_timing ==  MMC_TIMING_SD_HS) ||
             (sdport->cur_timing ==  MMC_TIMING_UHS_DDR50) ){

        if(fix_phase_flag & FIX_RPHASE50) {
            /* Manual set 50M read phase */
            r_phase = GET_FIX_RPHA50(rtk_fix_50M_phase);
            EM_ALERT("%s(%d)Use Bootcode pre-setting parameter\n",__func__,__LINE__);
            EM_ALERT("Set HS50 read phase: %d\n", r_phase);
        } else if (test_bit(PORT_HS50_TUNED,&sdport->port_status)) {
            mmcmsg1("\n");
            r_phase = sdport->tud_r_pha50;
        }else{
            mmcmsg1("\n");
            r_phase = sdport->vender_phase_ptr->HS50_dq_r;
        }
        if(fix_phase_flag & FIX_WPHASE50){
            /* Manual set 50M read phase */
            w_phase = GET_FIX_WPHA50(rtk_fix_50M_phase);
            EM_ALERT("%s(%d)Use Bootcode pre-setting parameter\n",__func__,__LINE__);
            EM_ALERT("Set HS50 write phase: %d\n", w_phase);
        }else{
            mmcmsg1("\n");
            w_phase = sdport->vender_phase_ptr->HS50_dq_w;
        }
        c_phase = sdport->vender_phase_ptr->HS50_cmd_w;
        //d_strobe = sdport->vender_phase_ptr->HS50_ds;
        mmcmsg1("cmd_wrphase=%d dq_wphase=%d dq_rphase=%d d_strobe=%d\n",
            c_phase,w_phase,r_phase,d_strobe);
    }else{
        /* legacy mode */
        if(sdport->cur_clock > CLK_400Khz ){
            r_phase = sdport->vender_phase_ptr->legacy25_dq_r;
            w_phase = sdport->vender_phase_ptr->legacy25_dq_w;
            c_phase = sdport->vender_phase_ptr->legacy25_cmd_w;
            //d_strobe = sdport->vender_phase_ptr->legacy25_ds;
            mmcmsg1("cmd_wrphase=%d dq_wphase=%d dq_rphase=%d d_strobe=%d\n",
                c_phase,w_phase,r_phase,d_strobe);
        }else{
            /* identify stage */
            r_phase = DEFAULT_R_PHASE;
            w_phase = DEFAULT_W_PHASE;
            c_phase = DEFAULT_C_PHASE;
            mmcmsg1("cmd_wrphase=%d dq_wphase=%d dq_rphase=%d d_strobe=%d\n",
                c_phase,w_phase,r_phase,d_strobe);
        }
    }

    dw_em_init_phase(sdport,w_phase,r_phase,c_phase);

}

/************************************************************************
 * 1. set PLL               ==> CRT
 * 2. set phase             ==> CRT
 * 3. set logic clock div   ==> wrap
 *
 * Nott: after reset CRT. the setting of div lost.
 ************************************************************************/
static
void dw_em_set_clk(struct rtksd_host *sdport,u32 em_clock)
{
    u32 base_clk = 0;
    u32 clk_div = 0;
    u32 seted_clk = 0;
    u32 idx = 0;
    u32 quirk_num = 0;

    quirk_num = sdport->mid;

    mmcmsg1("em_clock=%d\n",em_clock);

    if (em_clock <= CLK_400Khz ) {
        /* select 195KHz(50M/256) */
        idx = 0;
        seted_clk = CLK_200Khz;
        base_clk  = CLK_50Mhz;
        clk_div  = PLL_DIV_256;
    } else if ((em_clock >= CLK_25Mhz)
        &&(em_clock < CLK_50Mhz)) {
        /* 25Khz ~ 49Mhz = 25Mhz */
        idx = 1;
        seted_clk = CLK_25Mhz;
        base_clk = CLK_50Mhz;
        clk_div = PLL_DIV_2;
    } else if (em_clock < CLK_100Mhz) {
        /* 50Mhz ~ 99Mhz = 50Mhz */
        idx = 2;
        seted_clk = CLK_50Mhz;
        base_clk = CLK_50Mhz;
        clk_div = PLL_DIV_NON;
    } else if (em_clock < CLK_150Mhz) {
        /* 100Mhz ~ 149Mhz = 100Mhz */
        idx = 3;
        seted_clk = CLK_100Mhz;
        base_clk  = CLK_100Mhz;
        clk_div   = PLL_DIV_NON;
    } else if (em_clock <= CLK_150Mhz) {
        /* 150Mhz */
        idx = 4;
        seted_clk = CLK_150Mhz;
        base_clk = CLK_150Mhz;
        clk_div = PLL_DIV_NON;
    } else if (em_clock <= CLK_160Mhz) {
        /* 160Mhz */
        idx = 5;
        seted_clk = CLK_160Mhz;
        base_clk  = CLK_160Mhz;
        clk_div   = PLL_DIV_NON;
    } else if (em_clock <= CLK_170Mhz) {
        /* 170Mhz */
        idx = 6;
        seted_clk = CLK_170Mhz;
        base_clk = CLK_170Mhz;
        clk_div = PLL_DIV_NON;
    } else if (em_clock <= CLK_180Mhz) {
        /* 180Mhz */
        idx = 7;
        seted_clk = CLK_180Mhz;
        base_clk  = CLK_180Mhz;
        clk_div   = PLL_DIV_NON;
    } else if (em_clock <= CLK_190Mhz) {
        /* 190Mhz */
        idx = 8;
        seted_clk = CLK_190Mhz;
        base_clk = CLK_190Mhz;
        clk_div = PLL_DIV_NON;
    } else if (em_clock <= CLK_200Mhz) {
        /* 200Mhz */
        idx = 9;
        seted_clk = CLK_200Mhz;
        base_clk  = CLK_200Mhz;
        clk_div   = PLL_DIV_NON;
    }else{
        EM_NOTICE("emmc: unknow clock target %u, force to 25Mhz\n",
               em_clock);
        /* 25Mhz */
        idx = 1;
        seted_clk = CLK_25Mhz;
        base_clk = CLK_50Mhz;
        clk_div = PLL_DIV_2;
        WARN_ON(1);
    }

    mmcpll("seted_clk=%d base_clk=%d clk_div=%s(%d)\n",
            seted_clk,base_clk,
            (clk_div < DIV_TBL_CNT)? div_tlb[clk_div]: "unknown",clk_div);


    if(sdport->cur_clock == seted_clk){
        mmcpll("cur_clock == seted_clk; do nothing\n");
        /* do nothing */
    }else{


        mmcpll("cur_clock=%u seted_clk=%u; set always\n",
                sdport->cur_clock,seted_clk );
        /* is need to disable when set PLL ?? */
        //rtkemmc_disable_int(sdport);    //pair with below
        dw_em_set_PLL_clk(sdport,base_clk,clk_div,0);
        dwc51_set_div(sdport,clk_div);
        EM_NOTICE("EMMC: Clock change to %d Hz from %d Hz\n",
                seted_clk,sdport->cur_clock);
        sdport->cur_clock = seted_clk;
        //rtkemmc_enable_int(sdport);   //pair with above
    }

    /*set phase here */
    dw_em_select_phase(sdport);

}

/************************************************************************
  IP configure
  Set Common Parameters for all Versions:
   PWR_CTRL_R.SD_BUS_VOL_VDD1
   TOUT_CTRL_R.TOUT_CNT
   HOST_CTRL2_R.UHS2_IF_ENABLE=0
   EMMC_CTRL_R.CARD_IS_EMMC=1
  Set CLK_CTRL_R
    [See Host Controller Clock Setup Sequence]
 ************************************************************************/
static
void dwc51_IP_config(struct rtksd_host *sdport)
//void dw_em_IP_config(struct rtksd_host *sdport)
{
    EM_ALERT("%s(%d)[Alexkh]\n",__func__,__LINE__);
    //EMMC_FUNC();
    //PWR_CTRL_R.SD_BUS_VOL_VDD1 set 1V8 for eMMC
    // !!! But it doesn't seem to be needed here? move to ios
    em_outb(DWC51_PWR_CTRL_R8,
        SD_BUS_VOL_VDD1(eMMC_1V8) | SD_BUS_PWR_VDD1);

    // TOUT_CTRL_R[3:0]
    // TOUT CNT=14: TMCLK x 2^27: Data Timeout Counter Value
    em_outb(DWC51_TOUT_CTRL_R8, TMCLK_x_2P27);

    //HOST_CTRL2_R.UHS2_IF_ENABLE=0
    em_outw(DWC51_HOST_CTRL2_R16, 0);

    // EMMC_CTRL_R[3:2][0]
    // [0], CARD_IS_EMMC=1, 0x1(EMMC_CARD): Card connected to MSHC is an eMMC card
    // move 'reset pin' control to reset_card
    em_outw(DWC51_EMMC_CTRL_R16, CARD_IS_EMMC);

    /* ==========================
      !!! to use ADMA2 as default DMA mode.
      If It need to change the DMA mode in the future,
      It should open another function to control.
     ========================== */
    // HOST_CTRL1_R[7:0]
    // [4:3], DMA SEL=10, 0x2 : 0x2 ADMA2 is selected
    em_outb(DWC51_HOST_CTRL1_R8,DMA_SEL(DMA_SEL_ADMA2_32));

    // clear & disable interrupt
    // select poiing mode in romcode and enable what MMC need to check.
    //dwc51_int_act(INT_ACT_ENABLE, MMC_INT_EVENT, MODE_POLLING);
    //dwc51_int_act(INT_ACT_ENABLE, MMC_INT_EVENT, MODE_INTERRUPT);
    rtkemmc_enable_int(sdport);
    return;
}

/************************************************************************
 *
 ************************************************************************/
static
void dwc51_set_scrip_base(dma_addr_t scrip_base)
//void dw_em_set_scrip_base(dma_addr_t scrip_base)
{
   u16 reginfo16;
#ifdef EMMC_DEGUG
    u32 scrip_base_h;
    u32 scrip_base_l;

    scrip_base_h = (u32)((scrip_base & 0xffffffff00000000) >> 32);
    scrip_base_l = (u32)(scrip_base & 0xffffffff);

    //EM_INFO("scrip_base_64=0x%08x%08x\n",scrip_base_h,scrip_base_l);
#endif

    mmcdscp("scrip_base=0x%lx\n",scrip_base);
    reginfo16 = em_inw(DWC51_HOST_CTRL2_R16);
#ifdef ADMA_LENG_26BIT
    // enable ADMA2 Length to 26-bit.
    reginfo16 |= ADMA2_LEN_26BIT;
#else
    // enable ADMA2 Length to 16-bit.
    reginfo16 &= ~ADMA2_LEN_26BIT;
#endif
    em_outw(DWC51_HOST_CTRL2_R16,reginfo16);
    mmcdscp("DWC51_HOST_CTRL2_R16=0x%08x\n",em_inw(DWC51_HOST_CTRL2_R16));

    /* set Descriptor List Base Address Register */
    em_outl(DWC51_ADMA_SA_HIGH_R,(scrip_base >> 32) & 0xffffffffUL);
    em_outl(DWC51_ADMA_SA_LOW_R,scrip_base & 0xffffffffUL);

    //EM_INFO("DWC51_ADMA_SA_HIGH_R=0x%08x\n",em_inl(DWC51_ADMA_SA_HIGH_R));
    //EM_INFO("DWC51_ADMA_SA_LOW_R=0x%08x\n",em_inl(DWC51_ADMA_SA_LOW_R));

}

/************************************************************************
 * depend on
 * JESD84-B51-1.pdf chapter 6.15.9 H/W Reset Operation
 * tRSTW : RST_n pulse width; min 1[us]
 ************************************************************************/
static
void dwc51_reset_card(struct rtksd_host *sdport)
//void dw_em_reset_card(struct rtksd_host *sdport)
{
    unsigned short reginfo16;
    //EMMC_FUNC();
    mmcdbg("entry\n");
    reginfo16 = em_inw(DWC51_EMMC_CTRL_R16);
    // [3], EMMC_RST_N_OE=1, 0x1(ENABLE): sd_rst_n_oe is 1
    // [2], EMMC_RST_N=1, 0x1(RST_DEASSERT): Reset to eMMC device is deasserted
    reginfo16 &= ~EMMC_RST_N;
    reginfo16 |= EMMC_RST_N_OE;

    /* card reset should be implement here. This reg is GPIO control only.  */
    em_outw(DWC51_EMMC_CTRL_R16,reginfo16|EMMC_RST_N);
    udelay(20);
    em_outw(DWC51_EMMC_CTRL_R16,reginfo16);
    udelay(20);
    em_outw(DWC51_EMMC_CTRL_R16,reginfo16|EMMC_RST_N);

    /* ==================================================
     *  Toggle 74 clock here :
     *  The toggle clock will be send out before cmd0.
     *  for at least 74 clocl cycle: 250KHz for identification stage
     *      1/250KHz = 4us
     *      74*4 = 296us => chose 350us for save
     * ================================================== */
    udelay(350);

}

/************************************************************************
 * Only DWC IP reset, no other packages
    SW_RST_DAT                      BIT(2)
    SW_RST_CMD                      BIT(1)
    SW_RST_ALL                      BIT(0)
 ************************************************************************/
int dwc51_reset_IP(struct rtksd_host *sdport,u8 event)
//int dw_em_reset_IP(void)
{
    int ret = 0;
    u32 loop;

    mmcdbg("entry\n");   //mmcdbg;mmcdbg

    sdport->cur_timing = 0xff;
    sdport->bus_width = 0xff;

    //CLK_CTRL_R
    //[2], SD_CLK_EN=1, SD/eMMC Clock Enable
    //[0], INTERNAL_CLK_EN=1
    em_outw(DWC51_CLK_CTRL_R16, SD_CLK_EN|INTERNAL_CLK_EN);

    // SW_RST R[2:1], SW_RST_DAT=1, SW_RST_CMD=1
    // [2], Software Reset For DAT line
    // [1], Software Reset For CMD line
    em_outb(DWC51_SW_RST_R8, event);

    // CLK_CTRL_R[1:0], INTERNAL_CLK_STABLE=1, INTERNAL_CLK_EN=1
    // This bit enables the Host Driver to check the clock stability
    // twice after the Internal Clock Enable bit is set and after the
    // PLL Enable bit is set.
    // intclk_stable=1, card_clk_stable=1
    // check Software Reset done
    loop = 500;
    do{
        if(!loop){
            ret = -1;
            WARN(1,"%s: eMMC SW reset timeout!!!\n",DRIVER_NAME);
            break;
        }
        loop--;
        udelay(10);
    }while (em_inb(DWC51_SW_RST_R8) & event);

    return ret;
}

/************************************************************************
 * There are supporting actions.
 *  reset dwc IP => config dwc IP
 *
 ************************************************************************/
static
int dw_em_reset_dwc51(struct rtksd_host *sdport,u8 event,u8 save_en)
//void dw_em_reset_host_IP(struct rtksd_host *sdport,u8 save_en)
{
    int ret = 0;
    u8 cur_timing;
    u32 bus_width;
    //unsigned long flags;

    mmcdbg("entry\n");

    //spin_lock_irqsave(&sdport->lock,flags);
    if(save_en){
        /* store currently IP setting */
        bus_width = sdport->bus_width;
        cur_timing = sdport->cur_timing;
    }else{
        bus_width = MMC_TIMING_LEGACY;
        cur_timing = DAT_XFER_WIDTH1;
    }

    /* reset synopsys eMMC51 IP */
    ret = dwc51_reset_IP(sdport,event);
    dwc51_IP_config(sdport);


        /* restore previously IP setting */
    dwc51_set_timing(sdport,cur_timing);
    dwc51_set_bits(sdport,bus_width);


    //spin_unlock_irqrestore(&sdport->lock, flags);
    mmcdbg("\n");
    return ret;

}


/************************************************************************
 *
 ************************************************************************/
__attribute__((unused))
static
int dw_em_save_pad(struct rtksd_host *sdport,u32* pad_arry,int cnt)
{
    mmcdbg("entry\n");
    if(cnt != PAD_SET_CNT){
        return -1;
    }

    pad_arry[0] = em_inl(RTK_IOPAD_CFG1);
    pad_arry[1] = em_inl(RTK_IOPAD_CFG2);
    pad_arry[2] = em_inl(RTK_IOPAD_CFG3);
    pad_arry[3] = em_inl(RTK_IOPAD_SET1);
    pad_arry[4] = em_inl(RTK_IOPAD_SET2);
    pad_arry[5] = em_inl(RTK_IOPAD_SET3);
    pad_arry[6] = em_inl(RTK_IOPAD_SET4);

    return 0;
}

/************************************************************************
 *
 ************************************************************************/
__attribute__((unused))
static
int dw_em_restore_pad(struct rtksd_host *sdport,u32* pad_arry)
{
    mmcdbg("entry\n");
    em_outl(RTK_IOPAD_CFG1, pad_arry[0]);   //rst,clk,cmd,
    em_outl(RTK_IOPAD_CFG2, pad_arry[1]);   //ds,d5,d3,d4,d0,d1,d2,d7
    em_outl(RTK_IOPAD_CFG3, pad_arry[2]);   //d6

    em_outl(RTK_IOPAD_SET1, pad_arry[3]);   //rst/clk/cmd
    em_outl(RTK_IOPAD_SET2, pad_arry[4]);   //ds/d5/d3
    em_outl(RTK_IOPAD_SET3, pad_arry[5]);   //d4/d0/d1
    em_outl(RTK_IOPAD_SET4, pad_arry[6]);   //d2/d7/d6

    return 0;
}

/************************************************************************
 eMMC Pad driving:
 DRIVE_LEVEL is means driving strength,
 As the level number is bigger, the strength is greater

 ************************************************************************/
/* Future Work : This definition may be written in each platform header */
#define DRIVE_LEVEL7      (0x3fUL)
#define DRIVE_LEVEL6_CLK_DUTY      (0x9bUL)
#define DRIVE_LEVEL6      (0x1bUL)
#define DRIVE_LEVEL5      (0x2dUL)
#define DRIVE_LEVEL4      (0x09UL)
#define DRIVE_LEVEL4_2    (0x36UL)
#define DRIVE_LEVEL3      (0x12UL)
#define DRIVE_LEVEL2      (0x24UL)
#define DRIVE_LEVEL1      (0x0UL)

#define IO_PAD_SET1_HS400 ((DRIVE_LEVEL4<<26)|(DRIVE_LEVEL4<<17)|(DRIVE_LEVEL4<<8))   //rst/clk/cmd
#define IO_PAD_SET2_HS400 ((DRIVE_LEVEL4<<26)|(DRIVE_LEVEL4<<17)|(DRIVE_LEVEL4<<8))   //ds/d5/d3
#define IO_PAD_SET3_HS400 ((DRIVE_LEVEL4<<26)|(DRIVE_LEVEL4<<17)|(DRIVE_LEVEL4<<8))   //d4/d0/d1
#define IO_PAD_SET4_HS400 ((DRIVE_LEVEL4<<26)|(DRIVE_LEVEL4<<17)|(DRIVE_LEVEL4<<8))   //d2/d7/d6
#define IO_PAD_SET5_HS400 0

#define IO_PAD_SET1_HS200 IO_PAD_SET1_HS400
#define IO_PAD_SET2_HS200 IO_PAD_SET2_HS400
#define IO_PAD_SET3_HS200 IO_PAD_SET3_HS400
#define IO_PAD_SET4_HS200 IO_PAD_SET4_HS400
#define IO_PAD_SET5_HS200 IO_PAD_SET5_HS400

/*
 with device driving:HS50 : 0x0(x1)
 */
#define IO_PAD_SET1_HS50 ((DRIVE_LEVEL1<<26)|(DRIVE_LEVEL1<<17)|(DRIVE_LEVEL1<<8))
#define IO_PAD_SET2_HS50 ((DRIVE_LEVEL1<<26)|(DRIVE_LEVEL1<<17)|(DRIVE_LEVEL1<<8))
#define IO_PAD_SET3_HS50 ((DRIVE_LEVEL1<<26)|(DRIVE_LEVEL1<<17)|(DRIVE_LEVEL1<<8))
#define IO_PAD_SET4_HS50 ((DRIVE_LEVEL1<<26)|(DRIVE_LEVEL1<<17)|(DRIVE_LEVEL1<<8))
#define IO_PAD_SET5_HS50 ((DRIVE_LEVEL1<<26)|(DRIVE_LEVEL1<<17)|(DRIVE_LEVEL1<<8))

void dw_em_set_IO_driving(struct rtksd_host *sdport, unsigned int driving)
{
    u32 io_pad_setting1;
    u32 io_pad_setting2;
    u32 io_pad_setting3;
    u32 io_pad_setting4;


    mmcmsg1("pad driving change to %s mode.\n",dw_driving_tlb[driving]);
    sdport->cur_driving = driving;
    /*
     Pad config:
     set emmc pull up/pull down/slew rate/smt trigger
    */
    em_outl(EM51_33V_SEL,EMMC_1V8);       //select  1.8v
    em_outl(RTK_IOPAD_CFG1,0x91900000);   //rst,clk,cmd,
    em_outl(RTK_IOPAD_CFG2,0x59999999);   //ds,d5,d3,d4,d0,d1,d2,d7
    em_outl(RTK_IOPAD_CFG3,0x90000000);   //d6

    if(driving == DRIVING_HS400){
        io_pad_setting1 = (IO_PAD_SET1_HS400 | (0x2UL<<14));  //clk duty cycle +5% : b80108A4[16:14]=010
        io_pad_setting2 = IO_PAD_SET2_HS400;
        io_pad_setting3 = IO_PAD_SET3_HS400;
        io_pad_setting4 = IO_PAD_SET4_HS400;

    }else if(driving == DRIVING_HS200){
        io_pad_setting1 = IO_PAD_SET1_HS200;
        io_pad_setting2 = IO_PAD_SET2_HS200;
        io_pad_setting3 = IO_PAD_SET3_HS200;
        io_pad_setting4 = IO_PAD_SET4_HS200;

    }else{
        io_pad_setting1 = IO_PAD_SET1_HS50;
        io_pad_setting2 = IO_PAD_SET2_HS50;
        io_pad_setting3 = IO_PAD_SET3_HS50;
        io_pad_setting4 = IO_PAD_SET4_HS50;

    }

    if(rtk_emmc_test_ctl & EMMC_EN_IO_DRV){
        u32 *io_drv_ptr;

        EM_ALERT("%s(%d)Use Bootcode pre-setting parameter\n",__func__,__LINE__);

        io_drv_ptr = sdport->mmc_io_drv;

        io_pad_setting1 = io_drv_ptr[0];
        io_pad_setting2 = io_drv_ptr[1];
        io_pad_setting3 = io_drv_ptr[2];
        io_pad_setting4 = io_drv_ptr[3];
    }

    /*
     eMMC Pad driving/duty cycle:
    */
    em_outl(RTK_IOPAD_SET1, io_pad_setting1);   //rst/clk/cmd
    em_outl(RTK_IOPAD_SET2, io_pad_setting2);   //ds/d5/d3
    em_outl(RTK_IOPAD_SET3, io_pad_setting3);   //d4/d0/d1
    em_outl(RTK_IOPAD_SET4, io_pad_setting4);   //d2/d7/d6

    if(rtk_emmc_test_ctl & EMMC_EN_IO_DRV){
        EM_ALERT("0x%08x=0x%08x 0x%08x=0x%08x 0x%08x=0x%08x 0x%08x=0x%08x\n",
            RTK_IOPAD_SET1,(u32)em_inl(RTK_IOPAD_SET1),
            RTK_IOPAD_SET2,(u32)em_inl(RTK_IOPAD_SET2),
            RTK_IOPAD_SET3,(u32)em_inl(RTK_IOPAD_SET3),
            RTK_IOPAD_SET4,(u32)em_inl(RTK_IOPAD_SET4));
    }

}

/************************************************************************
 * Only reset CRT, no other packages
 * after crt reset, need:
 * - restore wrap, include pad driving / Divide frequency
 * - restore DWC IP, include bit width / timing mode
 ************************************************************************/
static
void dwc51_reset_crt(struct rtksd_host *sdport,u32 sync)
{
    sdport->cur_div = 0xff;     //wrap setting
    sdport->cur_driving = 0xff; //wrap setting
    sdport->bus_width = 0xff;   //dwc setting
    sdport->cur_timing = 0xff;  //dwc setting

    /*
      reset eMMC module over CRT.
     */
    mmcdbg("%s mode\n",(sync)?"sync":"non-sync");

    if(sync){
        /* crt sync reset */
        em_outl(RTK_EM_DW_SRST, RSTN_EM_DW);
        em_outl(RTK_EM_DW_CLKEN, (CLKEN_EM_DW | 0x01U));
        udelay(10);   //use read bace to replace delay
        //em_inl(RTK_EM_DW_SRST);
        //em_inl(RTK_EM_DW_CLKEN);
        em_outl(RTK_EM_DW_CLKEN, CLKEN_EM_DW);
    }else{
        em_outl(RTK_EM_DW_CLKEN, CLKEN_EM_DW);
        em_outl(RTK_EM_DW_SRST, RSTN_EM_DW);
        udelay(5); //use read bace to replace delay
        //em_inl(RTK_EM_DW_SRST);
        //em_inl(RTK_EM_DW_CLKEN);

    }

    em_outl(RTK_EM_DW_SRST, (RSTN_EM_DW | 0x01UL));
    em_outl(RTK_EM_DW_CLKEN, (CLKEN_EM_DW | 0x01U));

}

/************************************************************************
 * There are supporting actions.
 * IP status backup (anything need to save?)
 * => pad driving save
 * => crt rest => reset dwc IP => config dwc IP
 * => pad driving save => IP status restore
 ************************************************************************/
static
void dw_em_reset_host(struct rtksd_host *sdport,u8 save_en)
{
    u32 cur_div = 0;
    u32 cur_driving = 0;
    u32 bus_width = 0;
    u8 cur_timing = 0;

    mmcdbg("%s mode\n",(save_en)?"save":"non-save");

    if(save_en){
        mmcdbg("\n");
        /* is there anything else need to save? */
        cur_div = sdport->cur_div;          //wrap
        cur_driving = sdport->cur_driving;  //wrap
        bus_width = sdport->bus_width;      //dwc
        cur_timing = sdport->cur_timing;    //dwc
     }

    /* reset eMMC module over crt. */
    dwc51_reset_crt(sdport,0);

    if(dwc51_reset_IP(sdport,(u8)(SW_RST_DAT|SW_RST_CMD))){
        EM_ERR("DWC IP reset fail !!!!!\n");
        WARN(1,"%s: DWC IP reset fail !!!!!\n",DRIVER_NAME);
    }
    dwc51_IP_config(sdport);

    if(save_en){
        dwc51_set_timing(sdport,cur_timing);
        dwc51_set_bits(sdport,bus_width);
        dwc51_set_div(sdport,cur_div);
        dw_em_set_IO_driving(sdport, cur_driving);
    }


}


/************************************************************************
  DWC51_PSTATE_REG_R
 ************************************************************************/
#define DATA_PAD_CHK    (BIT(1))
#define CMD_PAD_CHK     (BIT(0))
#define ALL_PAD_CHK     (DATA_PAD_CHK|CMD_PAD_CHK)
static __attribute__((unused))
unsigned int dwc51_chk_pad_lvl(unsigned int pad_lvl)
{
    unsigned int reginfo = 0;
    unsigned int pad_info = 0;

    mmcdbg("entry\n");
    reginfo = em_inl(DWC51_PSTATE_REG_R);
    if(pad_lvl & CMD_PAD_CHK ){
        if(reginfo & CMD_LINE_LVL){
            pad_info |= 0x100;
        }
    }
    if(pad_lvl & DATA_PAD_CHK ){
        pad_info |= DAT_3_0_GET(reginfo);
        pad_info |= (DAT_7_4_GET(reginfo) << 4);
    }

    /* the return value bit9=cmd, bit(x)=data(x) */
    return pad_info;

}

/************************************************************************
  DWC51_PSTATE_REG_R
        DAT_LINE_ACTIVE     BIT(2)
        CMD_INHIBIT_DAT     BIT(1)  //for cmd + dat
        CMD_INHIBIT         BIT(0)  //for cmd only
        HOST_INHIBIT_MASK   (CMD_INHIBIT_DAT|CMD_INHIBIT)
        HOST_IDLE_MASK      (DAT_LINE_ACTIVE|CMD_INHIBIT_DAT|CMD_INHIBIT)

    Alexkh: Does it need to add a task state for the wait?
 ************************************************************************/
static
int dwc51_wait_host_busy(unsigned int chk_mode,unsigned int wait_ms)
{
    unsigned int busy = 1;
    unsigned int loop = 0;
    mmcdbg("entry\n");
    loop = wait_ms * 100;
    do{
        busy = em_inl(DWC51_PSTATE_REG_R) & chk_mode;
        if(busy & ( loop > 0)){
            if(loop%100 == 0)
                EM_ERR(" host busy=0x%08x wait loop=%u\n",busy,loop);
            udelay(10);
            loop--;
        }
    }while(busy & (loop > 0));

    if(busy)
        return 1;
    else
        return 0;

}

/************************************************************************
  DWC51_PSTATE_REG_R
    DAT_3_0 BIT[23:20]
        - DAT[3:0] Line Signal Level
            This bit is used to check the DAT line level to recover from
            errors and for debugging. These bits reflect the value of the
            sd_dat_in (lower nibble) signal.
 ************************************************************************/
static __attribute__((unused))
int dwc51_wait_card_busy(unsigned int wait_ms)
//u32 dw_em_chk_card_busy(void)
{
    unsigned int loop = 0;
    unsigned int busy = 1;

    mmcdbg("entry\n");
    loop = wait_ms*100;
    do{
        busy = (em_inl(DWC51_PSTATE_REG_R) & BIT(20));
        if(busy){
            EM_ERR("card busy!!!\n");
            /* "Busy" signaling (by pulling DAT0 line low) */
            break;
        }
        if(loop > 0){
            udelay(10);
            loop--;
        }
    }while(loop);

    if(busy)
        return  0;
    else
        return  1;

}

/************************************************************************
 HOST_CTRL2_R
 [7] SAMPLE CLK SEL
 [6] EXEC TUNING
 [2:0] UHS MODE SEL
    eMMC Speed Mode (eMMC mode only):
        0x0: Legacy
        0x1: High Speed SDR
        0x2: Reserved
        0x3: HS200
        0x4: High Speed DDR
        0x5: Reserved
        0x6: Reserved
        0x7: HS400
 ************************************************************************/
static
void dwc51_set_timing(struct rtksd_host *sdport,u32 timing)
{
    unsigned short reginfo16;
    unsigned short cur_timing;

    mmcmsg1("entry\n");
    // if(timing == sdport->cur_timing ){
    //     mmcdbg("timing no change, skip it.\n");
    //     return;
    // }

    sdport->cur_timing = timing;

    reginfo16 = em_inw(DWC51_HOST_CTRL2_R16);
    cur_timing = MMC_MODE_SEL_GET(reginfo16);
    mmcmsg1("reginfo16=0x%08x cur_timing=0x%x\n", reginfo16,cur_timing);

    if(timing == MMC_TIMING_MMC_HS400){
        if(cur_timing == MMC_MODE_HS400){
            mmcmsg1("cur_timing is MMC_MODE_HS400. skip it.\n");
            goto out;
        }
        mmcmsg1("MMC_TIMING_MMC_HS400\n");
        reginfo16 = MMC_MODE_SEL_SET(reginfo16,MMC_MODE_HS400);
    }else if(timing == MMC_TIMING_MMC_HS200){
        if(cur_timing == MMC_MODE_HS200){
            mmcmsg1("cur_timing is MMC_MODE_HS200. skip it.\n");
            goto out;
        }
        reginfo16 = MMC_MODE_SEL_SET(reginfo16,MMC_MODE_HS200);
        mmcmsg1("MMC_TIMING_MMC_HS200\n");
    }else if(timing == MMC_TIMING_MMC_HS){
        if(cur_timing == MMC_MODE_HS200){
            mmcmsg1("cur_timing is MMC_MODE_HS200. skip it.\n");
            goto out;
        }
        reginfo16 = MMC_MODE_SEL_SET(reginfo16,MMC_MODE_HS_SDR);
        mmcmsg1("MMC_TIMING_MMC_HS\n");
    }else if(timing == MMC_TIMING_MMC_DDR52){
        if(cur_timing == MMC_MODE_HS_DDR){
            mmcmsg1("cur_timing is MMC_MODE_HS_DDR. skip it.\n");
            goto out;
        }
        reginfo16 = MMC_MODE_SEL_SET(reginfo16,MMC_MODE_HS_DDR);
        mmcmsg1("MMC_TIMING_MMC_DDR52\n");
    }else if(timing == MMC_TIMING_LEGACY){
        if(cur_timing == MMC_MODE_HS_DDR){
            mmcmsg1("cur_timing is MMC_TIMING_LEGACY. skip it.\n");
            goto out;
        }
        reginfo16 = MMC_MODE_SEL_SET(reginfo16,MMC_MODE_LEGACY);
        mmcmsg1("MMC_TIMING_LEGACY\n");
    }else{
        mmcmsg1("unknow timing mode. skip it.\n");
        goto out;
    }
    mmcmsg1("reginfo16=0x%08x\n", reginfo16);

    em_outw(DWC51_HOST_CTRL2_R16,reginfo16);
out:
    return;
}


/************************************************************************
 *
 ************************************************************************/
#if 0
static
int dw_em_set_DDR400(struct rtksd_host *sdport, u32 ddr_mode)
{
//does it need still?
}
#endif

/************************************************************************
 *  if reset IP, this reg should be recoard and restore
 ************************************************************************/
#if 0
static
int dw_em_set_DDR(struct rtksd_host *sdport, u32 ddr_mode)
{
// does it need still?
}
#endif

/************************************************************************
  HOST_CTRL1 R
 [5] EXT DAT XFER
 0x1 (EIGHT BIT): 8-bit Bus Width
 [1] DAT XFER WIDTH
 0x1 (FOUR BIT): 4-bit mode
 0x0 (ONE BIT): 1-bit mode

 Note: if reset IP, this reg should be recoard and restore
 ************************************************************************/
static
int dwc51_set_bits(struct rtksd_host *sdport, u32 bus_width)
//int dw_em_set_bits(struct rtksd_host *sdport, u32 bus_width)
{
    unsigned char reginfo8;
    unsigned char current8;
    unsigned char bcnt = 0;
    // if(bus_width == sdport->bus_width){
    //     mmcdbg("bus_width no change, skip!(bus_width=0x%x)\n",  //mmcdbg
    //         bus_width);
    //     return 0;
    // }

    if(bus_width == MMC_BUS_WIDTH_8){
        bcnt = 2;
    }else if(bus_width == MMC_BUS_WIDTH_4){
        bcnt = 1;
    }else if(bus_width == MMC_BUS_WIDTH_1){
        bcnt = 0;
    }else{
        bcnt = 3;
    }
    sdport->cur_width = bus_width;

    current8 = reginfo8 = em_inb(DWC51_HOST_CTRL1_R8);
    mmcmsg1("bus_width=%s(%d); bfore: reginfo8=0x%08x\n",
            bit_tlb[bcnt],bus_width,reginfo8);

    // HOST_CTRL1_R[7:0]
    // [5], EXT_DAT_XFER=0x1, 0x1 (EIGHT_BIT): 8-bit Bus Width
    reginfo8 &= ~(DAT_XFER_WIDTH8|DAT_XFER_WIDTH4);
    mmcmsg1("after set: reginfo8=0x%08x\n", reginfo8);

    if(bus_width & MMC_BUS_WIDTH_8){
        sdport->cur_width = 8;
        if(current8 & DAT_XFER_WIDTH8 ){
            goto skip_out;
        }else{
            reginfo8 |= DAT_XFER_WIDTH8;
            mmcmsg1("8 bits mode\n");
        }
    }else if(bus_width & MMC_BUS_WIDTH_4){
        sdport->cur_width = 4;
        if(current8 & DAT_XFER_WIDTH4 ){
            goto skip_out;
        }else{
            reginfo8 |= DAT_XFER_WIDTH4;
            mmcmsg1("4 bits mode\n");
        }
    }else{ // 1-bit; MMC_BUS_WIDTH_1
        sdport->cur_width = 1;
        if(current8 & (DAT_XFER_WIDTH8 | DAT_XFER_WIDTH4 )){
            reginfo8 |= DAT_XFER_WIDTH1;
            mmcmsg1("%s: 1 bits mode\n",DRIVER_NAME);
        }else{
            goto skip_out;
        }
    }
    em_outb(DWC51_HOST_CTRL1_R8,reginfo8);

skip_out:
    mmcmsg1("DWC51_HOST_CTRL1_R8=0x%08x\n", em_inl(DWC51_HOST_CTRL1_R8));
    return 0;
}


/************************************************************************
 * reference .\linux-3.10.0\drivers\mmc\host\dw_mmc.c
 * static u32 dw_mci_prepare_command(struct mmc_host *mmc, struct mmc_command *cmd)
 ************************************************************************/
static
u32 dwc51_prepare_command(struct sd_cmd_pkt * cmd_info)
//u32 dw_em_prepare_command(struct sd_cmd_pkt * cmd_info)
{
    struct mmc_command *cmd = cmd_info->cmd;
    u32 cmd_idx  = 0;
    u32 rsp_typ  = 0;
    u32 cmd_typ  = 0;

    u32 cmd_flag = 0;
    u32 xfermode = 0;
    u32 wrt_typ  = 0;
    u32 blk_cnt = 0;

    cmd_idx = (u32)(cmd->opcode);
    rsp_typ = (u32)mmc_resp_type(cmd);
    cmd_typ = (u32)mmc_cmd_type(cmd);
    blk_cnt = cmd_info->block_count;

    if(cmd->data)
        wrt_typ = (u32)cmd->data->flags;

//#define SHOW_PREPARE_CMD
#ifdef SHOW_PREPARE_CMD
    EM_ALERT("cmd_idx=%u rsp_typ=0x%08x cmd_typ=0x%08x\n",
            cmd_idx,rsp_typ,cmd_typ);
#endif
    cmd_flag |=  (cmd_idx << CMD_INDEX_SHT);
    cmd_flag |=  (NORMAL_CMD << CMD_TYPE_SHT);

    /*Is it necessary to set this bit to see whether it is the main cmd or sub cmd in PSTATE_REG?
       need to ask DIC.
    */

    if (cmd_typ == MMC_CMD_ADTC ){
        cmd_flag |= DATA_PRESENT_SEL;
        xfermode |= BLOCK_COUNT_ENABLE;
        xfermode |= DMA_ENABLE;

        if(blk_cnt > 1){
            xfermode |= MULTI_BLK_SEL;

            if(cmd_info->auto_cmd_type & AUTO_CMD23){
                xfermode |= (AUTO_CMD23_ENABLED<<AUTO_CMD_ENABLE_SHT);
                mmcdbg("AUTO_CMD23 set, xfermode=0x%08x\n",xfermode);
            } else if(cmd_info->auto_cmd_type & AUTO_CMD12){
                xfermode |= (AUTO_CMD12_ENABLED<<AUTO_CMD_ENABLE_SHT);
                mmcdbg("AUTO_CMD12 set, xfermode=0x%08x\n",xfermode);
            }
        }
        if(wrt_typ & MMC_DATA_READ)
        {
            xfermode |= DATA_XFER_DIR; //0x1:read; 0x0: write
            mmcdbg("xfermode=0x%08x\n", xfermode);
        }
    }

    if(rsp_typ & MMC_RSP_PRESENT){
        if (rsp_typ & MMC_RSP_OPCODE){
            cmd_flag |= CMD_IDX_CHK_ENABLE;
        }
        if (rsp_typ & MMC_RSP_CRC){
            cmd_flag |= CMD_CRC_CHK_ENABLE;
        }
        if(rsp_typ & MMC_RSP_136){
            cmd_flag |= RESP_LEN_136;
        }else if(rsp_typ & MMC_RSP_BUSY){
            cmd_flag |= RESP_LEN_48B;
        }else{
            cmd_flag |= RESP_LEN_48;
        }
    }
#ifdef SHOW_PREPARE_CMD
    EM_ALERT("xfermode=0x%08x ; cmd_flag=0x%08x\n", xfermode,cmd_flag);
#endif
    /* set xfermode reg here */
    em_outw(DWC51_XFER_MODE_R16,xfermode);

    cmd_info->cmd_flag = cmd_flag;
    cmd_info->xfermode = xfermode;

    return 0;
}

/************************************************************************
 * Note : If relative interrupt are disable,
 *         these value of regiter will be zero.
 * Keep the function here for debugging in the future.
 ************************************************************************/
#if 0
void show_int_sta(const char *c_func,
        const int c_line,
        struct rtksd_host *sdport,
        u32 ctl)
{
#ifdef MMC_DEBUG
    //need modify for new IP
#endif
}
#endif

/************************************************************************
 * 1. if sanitize(0x3a) operation, waitting until finish.
 * 2. if power notication(0x22) operation,
 *        force PON always long and waitting 3 sec (spec is max 2.55 sec).
 * 3. if tuning state(PORT_TUNING_STATE),need to check Nac value in spec.
 ************************************************************************/
#define PON_TYPE_SHORT  (0x00000200)    //EXT_CSD_POWER_OFF_SHORT   2
#define PON_TYPE_LONG   (0x00000300)    //EXT_CSD_POWER_OFF_LONG    3
static
void dw_em_set_timer(struct sd_cmd_pkt * cmd_info)
{
    struct rtksd_host *sdport = NULL;
    u32 cmd_idx = cmd_idx = CMD_INDEX_GET(cmd_info->cmd_flag);
    u32 cmd_arg = cmd_info->cmd->arg;
    u32 timeend = cmd_info->tmout;

    mmcdbg("entry\n");
    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);

    /* Check if any special case needs to be handled first. */
    if(cmd_idx == MMC_SWITCH){
        if(cmd_arg==0x03a50101 ){
            /* sanitize(0x3a) */
            set_bit(SANITIZE_BIT, &cmd_info->cmd_status);
            goto SKIP_TIMER;
        }else if((cmd_arg & 0xffff0000) == 0x03220000 ){
            /* power notication(0x22) operation */
            unsigned int pon_type = (cmd_arg & 0x0000ff00);

            if((pon_type == PON_TYPE_LONG) || (pon_type == PON_TYPE_SHORT)){
                cmd_info->cmd->arg = (cmd_arg & 0xffff00ff) | PON_TYPE_LONG;
                timeend = msecs_to_jiffies(3000);
            }
        }
    } else if (test_bit(PORT_TUNING_STATE,&sdport->port_status)) {
        /* tuning state,need to check Nac value */
        timeend = msecs_to_jiffies(150);
    }

    mod_timer(&sdport->timer, (jiffies + timeend ) );

SKIP_TIMER:
    return;

}


/************************************************************************
 *
 *
 ************************************************************************/
//int dw_em_TriggerXferCmd(u32 cmd_flag,struct sd_cmd_pkt * cmd_info)
static
int dwc51_TriggerXferCmd(struct sd_cmd_pkt *cmd_info)
{
    struct rtksd_host *sdport = NULL;

    mmcdbg("entry\n");

    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);
    sdport->int_status = 0;
    sdport->completed_events = 0;
    sdport->pending_events = 0;
    sdport->task_state = STATE_SENDING_CMD;

    /* prepare timeout handler  */
    dw_em_set_timer(cmd_info);

    /* check reg for debug status */
    dwc51_chk_reg(__func__,__LINE__);

    /* clear int */
    dwc51_int_act(INT_GET_CLR, 0, 0);

#ifdef ERR_INJECT_ENABLE
    /* error inject for stability test  */
    {
        dwc51_err_injecter(30,
                FORCE_ERROR_INT,FORCE_ERROR_INT);
        //dwc51_err_injecter(30,
        //      FORCE_ERR_AUTO_CMD,FORCE_ERR_AUTO_CMD);
        //dwc51_err_injecter(30,
        //      FORCE_ERROR_RANDOM,0);
    }
#endif
    /* trigger cmd here and wait interrupt. go !!! */
    mmcdbg("cmd trigger cmd_flag=0x%08x\n",cmd_info->cmd_flag);
    smp_wmb();
    em_outw(DWC51_CMD_R16,cmd_info->cmd_flag);
    /* see you at interrupt sub-routing. */

    return 0;

}

/************************************************************************
    mid 0x11 ==> Toshiba/Kioxia
    mid 0x13 ==> Micron
    mid 0x15 ==> Samsung
    mid 0x90 ==> Hynix
    mid 0x45 ==> Sandisk
 ************************************************************************/
static
int rtk_match_device(struct rtksd_host *sdport)
{
    int i;
    int pnm_hit = 0;
    int final_vendor = 0xff;
    int default_vendor = 0xff;
    int mmc_vendor_cnt = 0;
    unsigned char *tmp_tbl = NULL;

    EM_ALERT("size of vendor_phase_tbl is %d \n",sizeof(vendor_phase_tbl));
    EM_ALERT("size of vender_info_t is %d \n",sizeof(vender_info_t));

    mmc_vendor_cnt = sizeof(vendor_phase_tbl)/sizeof(vender_info_t);
    if(mmc_vendor_cnt == 1)
            return 0;

    tmp_tbl = kzalloc(sizeof(vendor_phase_tbl), GFP_KERNEL);
    if(tmp_tbl == NULL){
        WARN(1, "%s: malloc tbl fail !!\n",DRIVER_NAME);
        return 0;
    }

    for(i=0; i<mmc_vendor_cnt ; i++){
        // serach default demo borad setting
        if(vendor_phase_tbl[i].config & MMC_DEMO_DFT){
            default_vendor = i;
        }
        //check vendor
        if(sdport->mid == vendor_phase_tbl[i].mid){
            tmp_tbl[i] = 1;

            /* check pnm */
            if(strncmp ( sdport->pnm, vendor_phase_tbl[i].pnm, 6) == 0){
                EM_INFO("Product name %s match\n",sdport->pnm);
                pnm_hit = 1;
                default_vendor = final_vendor = i;
                break;
            }
        }
    }
    if(default_vendor == 0xff){
        WARN(1, "%s: Can't find default phase setting\n",DRIVER_NAME);
        default_vendor = 0;
    }
    if(!pnm_hit){
        /* serach default vendor setting */
        for(i=0; i<mmc_vendor_cnt ; i++){
            if(tmp_tbl[i]){
                if(vendor_phase_tbl[i].config & MMC_VEND_DFT){
                    final_vendor = i;
                    break;
                }
            }
        }
    }
    if(final_vendor == 0xff){
        final_vendor = default_vendor;
    }

    // Ensure final_vendor is within the 8-bit range and a valid index
    if ((final_vendor < 0) || (final_vendor >= mmc_vendor_cnt)) {
        WARN(1, "Invalid final_vendor value\n");
        final_vendor = 0; // Setting to a safe default value
    }

    sdport->vender_phase_num = (u8)final_vendor;
    sdport->vender_phase_ptr = &vendor_phase_tbl[sdport->vender_phase_num];

    if(tmp_tbl)
        kfree(tmp_tbl);
#if 0
    EM_ALERT("tbl_num%d mid=0x%x \n",final_vendor,sdport->mid);
    EM_ALERT("legacy_25M-> cmd_wrphase=%d dq_wphase=%d  dq_rphase=%d  d_strobe=%d  \n",
        sdport->vender_phase_ptr->legacy25_cmd_w,sdport->vender_phase_ptr->legacy25_dq_w,
        sdport->vender_phase_ptr->legacy25_dq_r,sdport->vender_phase_ptr->legacy25_ds);
    EM_ALERT("hs_50-> cmd_wphase=%d dq_wphase=%d  dq_rphase=%d  d_strobe=%d  \n",
        sdport->vender_phase_ptr->HS50_cmd_w,sdport->vender_phase_ptr->HS50_dq_w,
        sdport->vender_phase_ptr->HS50_dq_r,sdport->vender_phase_ptr->HS50_ds);
    EM_ALERT("hs_200-> cmd_wphase=%d dq_wphase=%d  dq_rphase=%d  d_strobe=%d  \n",
        sdport->vender_phase_ptr->HS200_cmd_w,sdport->vender_phase_ptr->HS200_dq_w,
        sdport->vender_phase_ptr->HS200_dq_r,sdport->vender_phase_ptr->HS200_ds);
    EM_ALERT("hs_400-> cmd_wphase=%d dq_wphase=%d  dq_rphase=%d  d_strobe=%d  \n",
        sdport->vender_phase_ptr->HS400_cmd_w,sdport->vender_phase_ptr->HS400_dq_w,
        sdport->vender_phase_ptr->HS400_dq_r,sdport->vender_phase_ptr->HS400_ds);
#endif
    return 0;
}

/************************************************************************
 *
 ************************************************************************/
#if 0
static  __attribute__((unused))
void dwc51_auto_cmd_set(struct rtk_cmd_info* cmd_info)
{
#if 0    //AUTO_CMD_ENABLE
    e_device_type* sdport = &emmc_card;

    if(sdport->rtflags & RTKCR_CMD23_ALERT){
        /* there is cmd23 send before, so skip auto cmd. */
        sdport->rtflags &= ~RTKCR_CMD23_ALERT;
    }else if(cmd_info->xfer_flag & RTK_MMC_CMD23_ALERT){
        //no one set RTK_MMC_CMD23_ALERT before, fix later
        /* No stop cmd needed at RPPMB cmd sequence.
           no matter how many block access, send cmd23 always */
#ifdef AUTO_CMD23_EN
        cmd_info->xfer_flag |= RTK_MMC_AUTO_CMD23;
        EM_INFO("alert cmd23 for RPMB request\n");
#endif
    }else if(cmd_info->block_count > 1){
        /* Only multi-block access requires cmd23 or cmd12. */
#ifdef AUTO_CMD23_EN
        cmd_info->xfer_flag |= RTK_MMC_AUTO_CMD23;
        EM_INFO("alert cmd23\n");
#elif defined(AUTO_CMD12_EN)
        cmd_info->xfer_flag |= RTK_MMC_AUTO_CMD12;
        EM_INFO("alert cmd12\n");
#endif
    }
#else
    EM_INFO("no AUTO_CMD_ENABLE !!!\n");
#endif
}
#endif

/************************************************************************
 *
 ************************************************************************/
static
int dwc51_SendCmd(struct sd_cmd_pkt *cmd_info)
//int dw_em_SendCmd(struct sd_cmd_pkt *cmd_info)
{
    struct rtksd_host *sdport   = NULL;
    struct mmc_command *cmd     = cmd_info->cmd;
    u32 cmd_idx                 = (u32)(cmd->opcode);
    u32 cmd_arg                 = (u32)(cmd->arg);
    u32 *rsp                    = cmd->resp;

    mmcdbg("entry\n");
    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);
    if(rsp == NULL) {
        BUG_ON(1);
    }
    if ((cmd_idx == MMC_SET_BLOCK_COUNT) &&
        (test_bit(PORT_IS_RPMB,&sdport->port_status))) {
        cmd_arg |= BIT(31);
    }

    dw_em_get_cmd_timeout(cmd_info);
    dwc51_prepare_command(cmd_info);

    /* reduce log message when write operation in retry*/
    if (test_bit(PORT_FOPEN_LOG,&sdport->port_status)) {
        EM_ALERT("cmd_idx=%02u cmd_arg=0x%08x cmd_flag=0x%08x xfermode=0x%08x retries=%u\n",
               cmd_idx, cmd_arg, cmd_info->cmd_flag,cmd_info->xfermode, cmd->retries);
        //EM_ALERT("        ds_tune=%u\n",
        //    (cr_readl(EM_HALF_CYCLE_CAL_RESULT) & 0x1f));
    }

    /* This format is for TV006 requset. */
    if (sdport->log_state == 1) {
        if (cmd_idx == 35) {
            EM_WARNING("[EMMC ERASE]: start addr: 0x%x\n", cmd_arg);
        } else if (cmd_idx == 36) {
            EM_WARNING("[EMMC ERASE]: end addr: 0x%x\n", cmd_arg);
        } else if (cmd_idx == 6) {
            if ((cmd_arg & 0xffffff00) == 0x3200100)
                EM_WARNING("[EMMC FLUSH]: 0x%x\n", cmd_arg);
        }
    }

    em_outw(DWC51_BLOCKSIZE_R16,0);
    em_outw(DWC51_BLOCKCOUNT_R16,0);
    em_outl(DWC51_ARGUMENT_R, cmd_arg);

    dwc51_TriggerXferCmd(cmd_info);

    return 0;

}

/************************************************************************
 *
 ************************************************************************/
static
int dwc51_Stream_Cmd(struct sd_cmd_pkt* cmd_info)
//int dw_em_Stream_Cmd(struct sd_cmd_pkt * cmd_info)
{
    struct rtksd_host *sdport = NULL;
    struct mmc_command *cmd    = cmd_info->cmd;
    u32  cmd_idx        = (u32)(cmd->opcode);
    u32  cmd_arg        = (u32)(cmd->arg);
    u32 *rsp            = cmd->resp;
    u32  byte_count     = (u32)(cmd_info->byte_count);
    u32  blk_cnt        = (u32)(cmd_info->block_count);
    u32  total_byte_cnt = (u32)(cmd_info->total_byte_cnt);
    //int ret_err = 0;

    mmcdbg("entry\n");
    if(rsp == NULL) {
        BUG_ON(1);
    }

    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);

    dw_em_get_cmd_timeout(cmd_info);
    dwc51_prepare_command(cmd_info);

    if (test_bit(PORT_FOPEN_LOG,&sdport->port_status) ||
        test_bit(PORT_RECOVER_STATE,&sdport->port_status)) {
        EM_ALERT("cmd_idx=%02u cmd_arg=0x%08x cmd_flag=0x%08x xfermode=0x%08x retries=%u\n"
            "            blk_siz=0x%08x total_byte_cnt=0x%08x\n",
            cmd_idx, cmd_arg, cmd_info->cmd_flag, cmd_info->xfermode, cmd->retries,
            (u32) byte_count, (u32) total_byte_cnt);
    }

    /* This format is for TV006 requset. */
    if( (sdport->log_state == 1)
     && ( (cmd_idx == 18) || (cmd_idx == 25)
       ||(cmd_idx == 17) || (cmd_idx == 24))) {
        EM_WARNING("[EMMC %s]: addr: 0x%x; size: 0x%x; {cmd%u}\n",
            ((cmd_idx == 18) || (cmd_idx == 17))? " Read" : "Write",
            cmd_arg, total_byte_cnt,cmd_idx);
    }

    em_outw(DWC51_BLOCKSIZE_R16,byte_count);
    em_outw(DWC51_BLOCKCOUNT_R16,blk_cnt);
    em_outl(DWC51_ARGUMENT_R, cmd_arg);

    dwc51_TriggerXferCmd(cmd_info);

    return 0;

}


/************************************************************************
 *
 ************************************************************************/
static
u32 dw_em_get_cmd_timeout(struct sd_cmd_pkt *cmd_info)
{
    struct rtksd_host *sdport   = NULL;
    struct mmc_command* cmd     = cmd_info->cmd;
    u16 block_count             = cmd_info->block_count;
    u32 tmout = 0;

    mmcdbg("entry\n");
    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);

    /*
     * Dont do conversion too early
     * We want to know real value user want to set.
     */
    if(cmd_info->cmd->data){
        if(block_count == 1)
            tmout = 500;
        else
            tmout = 1000+ (block_count/2);
    }else{
        if (mmc_resp_type(cmd) & MMC_RSP_BUSY) {
            /*
             if response type is R1b,
             Host should wait long time for card busy.
             */
            tmout = (cmd->busy_timeout)? cmd->busy_timeout:1000;
            mmcdbg("r1b alert, tmout=%d\n",tmout+2000);
        } else {
            tmout = (200);
        }
    }
    if(IS_R1B_CMD(cmd_info->cmd_flag)){
        mmcdbg("tmout=%d\n",tmout);
    }
    mmcdbg("tmout=%d\n",tmout);
    cmd_info->tmout = tmout+ext_tmout_ms;
    return 0;
}


/************************************************************************
 * ref. DWC_mshc_databook_WM-6615_eMMC5.1.pdf
 * ch. A.8.1 ADMA2 Operation
 * 64-bit ADMA2 Descriptor Line (32-bit Addressing Mode)
 * One Descriptor Line is 8 Bytes
 * 26-bit mode: 0x03ff_ffff bytes => 0x1_ffff blocks
 * 16-bit mode: 0xffff bytes      => 0x7f blocks (used here)
 *
 * Note: data book P.94
 * While using DMA, the host memory data buffer size and
 * start address must not exceed 128 MB.
 * If it exceeds, the data buffer must be split using two descriptors
 * such that an AXI/AHB transfer attempting
 * to cross the limit (of 128 MB) is not generated.
 ************************************************************************/
#define DESP_VAILD              (BIT(0))
#define DESP_END                (BIT(1))
#define DESP_INTF               (BIT(2))
#define DSP_TYPE_TRAN           (4 << 3) //Transfer data of one descriptor line
#define DSP_TYPE_LINK           (6 << 3) //Link to another descriptor:
                                         //     Use this parameter if the descriptor
                                         //     is not continuous and points to another location.

#define _128MB		0x8000000
#define ADDR_ALIGN_BOUNDARY_MORE(addr, size) 	(((addr - 1) & ~((size) - 1)) + size)
#define CROSSES_128MB_BOUNDARY(start, length) \
    ((((start) >> 27) != (((start) + (length) - 1) >> 27)) ? 1 : 0)

#define BOUNDARY_SHIFT(boundary) __builtin_ctz(boundary)
#define CROSSES_BOUNDARY_SHIFT(start, length, shift) \
    ((((start) >> (shift)) != (((start) + (length) - 1) >> (shift))) ? 1 : 0)

static
int dwc51_sg_make_script(struct scatterlist *sg,
             int dma_nents, struct sd_cmd_pkt *cmd_info)
//int dw_em_sg_make_script(struct scatterlist *sg,
//             int dma_nents, struct sd_cmd_pkt *cmd_info)
{
    int err = 0;
    u32 i;
    u32 length;
    u32 scrip_cnt;
    u32 mem_addr;
    u32 byte_cnt;
    u32 *desp_line = NULL;
    u32 *desp_line_last = NULL;
    struct rtksd_host *sdport;
    u32 aglin_addr = 0;

    mmcdscp("entry\n");
    mmcdscp("sg=0x%px dma_nents=%d\n",sg,dma_nents);

    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);
    cmd_info->scrip_cnt      = 0;
    cmd_info->block_count    = 0;
    cmd_info->total_byte_cnt = 0;

    desp_line = sdport->scrip_buf;
    mmcdscp("des_base=0x" PRIxPTR_FORMAT "\n",(uintptr_t)desp_line);
    for (i = 0; i < dma_nents; i++,sg++) {
        length = sg_dma_len(sg);
        mem_addr = sg_dma_address(sg);
        scrip_cnt = 0;
        cmd_info->total_byte_cnt += length;
        while(length)
        {
            desp_line[0] = 0;
            desp_line[1] = 0;
            mmcdscp("sg%d length=0x%08x; mem_addr=0x%08x\n",
                    i,length,mem_addr);
            mmcdscp("addr of desp_line[0]=0x%08x desp_line[1]=0x%08x\n",
                &desp_line[0],&desp_line[1]);
            /* setting transfer parameter  */
            desp_line[0] |= DESP_VAILD;
            //desp_line[0] |= DESP_INTF;    //no interrupt trigger
            desp_line[0] |= DSP_TYPE_TRAN;

            if(CROSSES_128MB_BOUNDARY(mem_addr, length)){
                aglin_addr = ADDR_ALIGN_BOUNDARY_MORE(mem_addr, _128MB);
                byte_cnt = (aglin_addr - mem_addr);
            }else{
                if(length > EMMC_MAX_SCRIPT_BYTE){
                    byte_cnt = EMMC_MAX_SCRIPT_BYTE;
                }else{
                    byte_cnt = length;
                    desp_line_last = desp_line;
                }
            }
            desp_line[0] |= ((byte_cnt & 0x03ff0000) >> 16) << 6;
            desp_line[0] |= (byte_cnt & 0x0000ffff) << 16;
            desp_line[1] = mem_addr;

            mmcdscp("desp%03d: 0x%08x; 0x%08x\n",scrip_cnt,desp_line[1],desp_line[0]);
            desp_line += 2;
            length -= byte_cnt;
            mem_addr += byte_cnt;
            scrip_cnt++;

            mmcdscp("length=0x%08x; mem_addr=0x%08x\n",
                length,mem_addr);

            if(length == 0){
                mmcdscp("sg%d descripter build finish\n",i);
                break;
            }
        }
        cmd_info->scrip_cnt += scrip_cnt;

    }
    desp_line_last[0] |= DESP_END;

//#define CHANGE_ENDIAN
#ifdef CHANGE_ENDIAN
    desp_line = sdport->scrip_buf;
    for (i = 0; i < cmd_info->scrip_cnt; i++,sg++) {
            mmcdscp("change endian\n");
            dwc51_4byte_swap(&desp_line[0]);
            dwc51_4byte_swap(&desp_line[1]);
            desp_line += 2;
    }
#endif

    cmd_info->byte_count = 0x200;
    cmd_info->block_count  = (cmd_info->total_byte_cnt >> 9);
    if (cmd_info->total_byte_cnt & 0x1FF){
        /* The case that block is not 512 bytes. */
        if(cmd_info->block_count){
            EM_ALERT("non 512 align but more than 512B transfer !!!\n");
            WARN(1,"%s: non 512 align but more than 512B transfer !!!\n",DRIVER_NAME);
        }else{
            mmcdscp("non block transfer\n");
            cmd_info->byte_count = cmd_info->total_byte_cnt;
            cmd_info->block_count = 1;
        }
    }
    mmcdscp("block_count=0x%08x total_byte_cnt=0x%lx \n",
        cmd_info->block_count, cmd_info->total_byte_cnt);

    if((cmd_info->scrip_cnt) > EMMC_MAX_SCRIPT_LINE ){
        WARN(1, "%s: scrip length more then %d(%d)\n",
                DRIVER_NAME,EMMC_MAX_SCRIPT_LINE,cmd_info->scrip_cnt);
        err = -1;
    }

    dmac_flush_range(sdport->scrip_buf, sdport->scrip_buf + EMMC_SCRIPT_BUF_SIZE);
    outer_flush_range(sdport->scrip_buf_phy, sdport->scrip_buf_phy +EMMC_SCRIPT_BUF_SIZE );
    dwc51_set_scrip_base(sdport->scrip_buf_phy);

    wmb(); /* drain writebuffer */

    return err;
}

int export_em_sg_make_script(struct scatterlist *sg,
             int dma_nents, struct sd_cmd_pkt *cmd_info)
{

    return dwc51_sg_make_script(sg,dma_nents,cmd_info);
}
EXPORT_SYMBOL(export_em_sg_make_script);

/************************************************************************
 * interrupt reg operate
 * act: only one act per call
 *      INT_ACT_DISABLE ==> disable int
 *      INT_ACT_ENABLE  ==> enable int, must set int event you want to trigger.
 *      INT_GET_CLR ==> get int status. then clr pending bit
 *      INT_GET_STA ==> get int status. no clr ststus
 *      INT_RSN_CLR ==> clr pending bit, need to assige event at field event.
 *
 * mode: MODE_POLLING / MODE_INTERRUPT
 *
 * Note: Don't call dwc51_int_act(INT_GET,X,X) twice in the same function,
 *          unless you are sure that all pending bits have been processed.
 ************************************************************************/
static
u32 dwc51_int_act(u32 act,u32 event,u32 mode){
    u32 reg_info;
    u32 tol_status = 0;

    mmcdbg("entry\n");

    /* !!! To operate NORMAL_INT & ERROR_INT in same time. */
    // NORMAL_INT_STAT_R[15:0] => 0x18010930
    // ERROR_INT_STAT_R[15:0]  => 0x18010932
    if(act == INT_CLR_RSN){
        em_outl(DWC51_TOTAL_INT_STAT_R, event);
        tol_status = em_inl(DWC51_TOTAL_INT_STAT_R);
        mmcdbg("clear event=0x%08x after_stat=0x%08x\n",event,tol_status);
        goto OUT;
    }

    tol_status = em_inl(DWC51_TOTAL_INT_STAT_R);
    mmcdbg("Total_INT_STAT_R=0x%08x INT_STAT_EN=0x%08x INT_SIGNAL_EN=0x%08x\n",
        tol_status, em_inl(DWC51_TOTAL_INT_STAT_EN_R), em_inl(DWC51_TOTAL_INT_SIGNAL_EN_R));
    if(act == INT_GET_STA){
        goto OUT;
    }else if(act == INT_GET_CLR ){
        /* alexkh: this func diff with bootcode, sync later. */
        em_outl(DWC51_TOTAL_INT_STAT_R, tol_status);
        goto OUT;
    }

    if(act == INT_ACT_ENABLE ){
        reg_info = event;   //for now "0xffff7ebf"
        mmcdbg("event=0x%08x\n",reg_info);
    }else{  // INT_ACT_DISABLE
        reg_info = 0;
        mmcdbg("event=0x%08x\n",reg_info);
    }
    mmcdbg("event=0x%08x\n",reg_info);
    // NORMAL_INT_STAT_EN_R[15:0] => 0x18010934
    // [8], CARD_INTERRUPT_STAT_EN=0
    // [6], CARD_INSERTION_STAT_EN=0
    // ERROR_INT_STAT_EN_R[15:0]  => 0x18010936
    em_outl(DWC51_TOTAL_INT_STAT_EN_R, reg_info);

    if(mode == MODE_INTERRUPT ){
        mmcdbg("event=0x%08x\n",reg_info);
        // NORMAL_INT_SIGNAL_EN_R[15:0] => 0x18010938
        // [8], CARD_INTERRUPT_SIGNAL_EN=0
        // [6], CARD_INSERTION_SIGNAL_EN=0
        // ERROR_INT_SIGNAL_EN_R[15:0]  => 0x1801093A
        em_outl(DWC51_TOTAL_INT_SIGNAL_EN_R, reg_info);
    }
OUT:
    return tol_status;

}


/************************************************************************
 *  This func is used for red mmc fix pattern func to verify data.
 *  if block header is 0x2379fbeef then scan data.
 ************************************************************************/
#ifdef DEBUG_BUFFER
/*
* This is reference function below
* lib/scatterlist.c
* static size_t sg_copy_buffer(struct scatterlist *sgl, unsigned int nents,
*           void *buf, size_t buflen, int to_buffer);
*/
static
void emmc_vreify_buf(struct sd_cmd_pkt *cmd_info, int to_buffer)
{
    int i,j;
    struct scatterlist *sg;
    u32* dma_addr_virt = NULL;
    u32 dma_leng;
    int miter_cnt;
    int blk_cnt;
    int show_err;
    struct mmc_host *host;
    struct sg_mapping_iter sg_miter;
    unsigned long flags;
    unsigned int sg_flags = SG_MITER_ATOMIC;
    EM_ALERT("%s(%d)[Alexkh]\n",__func__,__LINE__);
    host = cmd_info->sdport->mmc;
    sg = cmd_info->data->sg;

    if (to_buffer)
        sg_flags |= SG_MITER_FROM_SG;
    else
        sg_flags |= SG_MITER_TO_SG;

    sg_miter_start(&sg_miter, cmd_info->data->sg,cmd_info->data->sg_len, sg_flags);

    local_irq_save(flags);

    miter_cnt = 0;
    while(sg_miter_next(&sg_miter)){
        blk_cnt = 0;
        dma_addr_virt = (u32*)sg_miter.addr;
        dma_leng = sg_miter.length;
        blk_cnt = (dma_leng>>9);
        ++miter_cnt;
        for(i=0; i< blk_cnt; i++ ){
            if(dma_addr_virt[(i*128)+0] == 0x2379beef){
                show_err = 0;
                j = 0;
                for(j=1; j< 128; j++ ){
                    if((dma_addr_virt[(i*128)+j] & 0xffff0000) != 0xbeef0000){
                        EM_ALERT("error hit!!! dma_addr_phy[%x]=0x%08x, (virt_addr=0x%p, phy_addr=0x%08x)\n",
                                (i*128)+j, dma_addr_virt[(i*128)+j],
                                &(dma_addr_virt[(i*128)+j]),
                                virt_to_phys((void*)&(dma_addr_virt[(i*128)+j])));
                        show_err = 1;
                        break;
                    }
                }

                if(show_err){
                    if(i==0){
                        EM_ALERT("##(%s)## miter_cnt=%d\n",(cmd_info->data->flags & MMC_DATA_READ)?"R":"W",miter_cnt);
                        EM_ALERT("dma_addr_virt =0x%p len=0x%x blk_cnt=%d\n",dma_addr_virt,dma_leng,blk_cnt);
                    }

                    EM_ALERT("virt_addr=0x%p, phy_addr=0x%08x\n",
                                &(dma_addr_virt[(i*128)]),
                                virt_to_phys((void*)&(dma_addr_virt[(i*128)])));
                    EM_ALERT("dma_addr_phy[0]=0x%08x [1]=0x%08x [2]=0x%08x [3]=0x%08x\n",
                        dma_addr_virt[(i*128)+0],dma_addr_virt[(i*128)+1],
                        dma_addr_virt[(i*128)+2],dma_addr_virt[(i*128)+3]);
//#define SHOW_BLK_DATA
#ifdef SHOW_BLK_DATA
                    for(j=0; j< (128/8); j++ ){
                            u32 addr_sht = (i*128)+(j*8);
                            EM_ALERT("[%03x-%03x] %08x %08x %08x %08x %08x %08x %08x %08x",
                                 (j*32)+0,(j*32)+31,
                                 dma_addr_virt[addr_sht+0],dma_addr_virt[addr_sht+1],
                                 dma_addr_virt[addr_sht+2],dma_addr_virt[addr_sht+3],
                                 dma_addr_virt[addr_sht+4],dma_addr_virt[addr_sht+5],
                                 dma_addr_virt[addr_sht+6],dma_addr_virt[addr_sht+7]);
                            EM_ALERT("\n");
                    }
                    EM_ALERT("\n");
#endif
                }
            }
        }
    }

    sg_miter_stop(&sg_miter);

    local_irq_restore(flags);
}

#endif

/************************************************************************
 *
 ************************************************************************/
static
int dw_em_Stream(struct sd_cmd_pkt *cmd_info)
{
    int ret_err=0;
    struct mmc_host *host;
    struct rtksd_host *sdport;
    struct scatterlist *sg;
    u32 dir = 0;
    int dma_nents = 0;

    mmcdbg("entry\n");

    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);
    host   = sdport->mmc;

    if(sdport->scrip_buf == NULL){
       /* Scripter base is setting when srcipter buffer alloced.
        * check where is dw_em_set_scrip_base(). */
        BUG_ON(1);
    }

    if(cmd_info->data->flags & MMC_DATA_READ){
        dir = DMA_FROM_DEVICE;
    }else{
        dir = DMA_TO_DEVICE;
    }

    cmd_info->data->bytes_xfered=0;
    dma_nents = dma_map_sg( mmc_dev(sdport->mmc), cmd_info->data->sg,
            cmd_info->data->sg_len,  dir);
    set_bit(SG_MAPPED_BIT, &cmd_info->cmd_status);

    if((dma_nents < 0) || (dma_nents > host->max_segs) ){
        WARN(1, "%s: Unexpected scatter&gather case. dma_nents=%d\n",
                DRIVER_NAME,dma_nents);
        ret_err = -ENOMEM;
        cmd_info->cmd->error = ret_err;
        goto fail_out;
    }

    sg = cmd_info->data->sg;
    if(dwc51_sg_make_script(sg,dma_nents,cmd_info)){
        EM_ERR("make script fail!\n");
        ret_err = -ENOMEM;
        cmd_info->cmd->error = ret_err;
        goto fail_out;
    }

#ifdef DEBUG_BUFFER
    if( (rtk_emmc_test_ctl & EMMC_CHK_BUF) &&
        (cmd_info->data->flags & MMC_DATA_WRITE)){
        emmc_vreify_buf(cmd_info,1);
    }
#endif

    dwc51_Stream_Cmd(cmd_info);

#ifdef DEBUG_BUFFER
    /* Alexkh: for read case, this need move to bottom hakf*/
    if( (rtk_emmc_test_ctl & EMMC_CHK_BUF) &&
        (cmd_info->data->flags & MMC_DATA_READ)){
        emmc_vreify_buf(cmd_info,0);
    }
#endif

fail_out:
    return ret_err;

}


/************************************************************************
 *
 ************************************************************************/
static
void rtkem_polling_tasklet(unsigned long param)
{
    struct rtksd_host *sdport = NULL;
    struct sd_cmd_pkt *cmd_info = NULL;
    struct mmc_request  *mrq = NULL;
    u32  state = 0;
    u32  prev_state = 0;
    u32 int_status = 0;
    u32 recover_mothod = 0;
    unsigned long flags;

    mmcdbg("entry\n"); //mmcdbg;mmctask
    sdport = (struct rtksd_host *)param;

    spin_lock_irqsave(&sdport->lock, flags);

    cmd_info = &sdport->cmd_info;
    if(IS_R1B_CMD(cmd_info->cmd_flag)){
        /*debug check */
        EM_ALERT("no any R1B cmd can come here!!!\n"); //mmcdbg;mmctask
        WARN(1,"%s: no any R1B cmd can come here!!!\n", DRIVER_NAME);
    }

    state = sdport->task_state;

    if(test_bit(REQ_FINISH_BIT, &cmd_info->cmd_status)){
        mmcdbg("REQ_FINISH_BIT alert already\n");
        sdport->task_state = STATE_IDLE;
        spin_unlock_irqrestore(&sdport->lock, flags);
        return;
    }

    /* debug only */
    if(test_bit(CMD_IRQ_BIT,&cmd_info->cmd_status)){
        mmctask("force CMD_IRQ_BIT\n");     //mmcdbg;mmctask
        clear_bit(CMD_IRQ_BIT,&cmd_info->cmd_status);
    }
    if(test_bit(DATA_IRQ_BIT,&cmd_info->cmd_status)){
        mmctask("force DATA_IRQ_BIT\n");    //mmcdbg;mmctask
        clear_bit(DATA_IRQ_BIT,&cmd_info->cmd_status);
    }

    do {
        prev_state = state;

        switch (state) {
        case STATE_IDLE:
            mmcdbg("what happen?! STATE_IDLE\n");
            break;

        case STATE_SENDING_CMD:
            int_status   = sdport->int_status;
            //if(test_bit(ERR_INTERRUPT_BIT,&sdport->pending_events)){
            if(test_bit(CMD_ERR_BIT,&cmd_info->cmd_status)){
                /* error bit alert */
                if(int_status & ~(ERR_CMD_REASON_MASK_H | 0x0000ffff) ){
                    /* debug only,
                    Thecmd stage err alert. it's shuold not have data relative error. */
                    EM_ALERT("The non-data cmd have data relative error!(0x%08x)\n",int_status);
                    WARN(1,"%s: The non-data cmd have data relative error!(0x%08x)\n",
                        DRIVER_NAME,int_status);
                }
                mmcdbg("cmd stage err\n");
                state = STATE_PRE_COMPLETE;
                break;

            }else{
                if(test_bit(CMD_COMPLETE_BIT,&sdport->pending_events)){
                    EM_ALERT("Should not CMD_COMPLETE_BIT pending, (0x%08x)\n",
                        sdport->pending_events);
                    WARN(1,"%s: Should not CMD_COMPLETE_BIT pending, (0x%08x)\n",
                        DRIVER_NAME,sdport->pending_events);
                }

                if(test_bit(CMD_COMPLETE_BIT,&sdport->completed_events)){
                    /* cmd completed */

                    if(NEED_WAIT_XFER(cmd_info->cmd_flag)){
                        /* data cmd or R1B.*/
                        state = STATE_SENDING_DATA;
                        mmctask("state = STATE_SENDING_DATA\n");
                    }else{
                        state = STATE_PRE_COMPLETE;

                    }
                    break;
                }else{
                    /* no err & no CMD_COMPLETE pandind & complete bit.
                        check just for debug.....  */
                    state = STATE_PRE_COMPLETE;
                    set_bit(CMD_UNEXPT_BIT,&cmd_info->cmd_status);
                    EM_ALERT("no err \n"
                            "& no CMD_COMPLETE panding bit "
                            "& no CMD_COMPLETE complete bit!!!\n");
                    WARN(1, "%s: no err & no CMD_COMPLETE panding & complete bit\n",DRIVER_NAME);
                    break;
                }
            }

        case STATE_SENDING_DATA:
            if(test_bit(CMD_ERR_BIT,&cmd_info->cmd_status)){
                /* just for debug */
                state = STATE_PRE_COMPLETE;
                set_bit(CMD_UNEXPT_BIT,&cmd_info->cmd_status);
                EM_ALERT("cmd stage err, it should not come here.\n");
                WARN(1,"%s: cmd stage err, it should not come here.\n",DRIVER_NAME);
                break;
            }

            if(NEED_WAIT_XFER(cmd_info->cmd_flag) == 0){
                /* non-data cmd or no R1B.*/
                /* no xfer cmd come here!!!, this should not be the case
                    just for debug */
                state = STATE_PRE_COMPLETE;
                set_bit(CMD_UNEXPT_BIT,&cmd_info->cmd_status);
                EM_ALERT("no data cmd come here!!!, this should not be the case!!!\n");
                WARN(1,"%s: no data cmd come here!!!, this should not be the case!!!\n",
                    DRIVER_NAME);
                break;
            }

            if(test_bit(ERR_INTERRUPT_BIT,&sdport->pending_events)){
                /* error bit alert */
                mmctask("error bit alert\n");
                /*DATA_XFER_DIR: 0x1: read; 0x0: write */
                if(cmd_info->xfermode & DATA_XFER_DIR){
                    /* case read */
                    mmctask("case read STATE_PRE_COMPLETE\n");
                    state = STATE_PRE_COMPLETE;
                    break;
                }else{
                    /* case write */
                    if(test_bit(XFER_COMPLETE_BIT,&sdport->pending_events)){
                        /* finally, even error happened, the xfer is done.*/
                        mmctask("case write,even error happened, the xfer is done\n");
                        state = STATE_PRE_COMPLETE;
                        break;
                    }else{
                        /* write cmd waiting for xfer-done anyway.*/
                        mmctask("case write,waiting for xfer-done anyway.\n");
                        prev_state = state;
                        tasklet_schedule(&sdport->polling_tasklet);
                        break;
                    }
                }
            }else{
                /* no err alert */
                if(test_bit(CMD_COMPLETE_BIT,&sdport->completed_events)) {
                    /* cmd completed */
                    if(test_bit(XFER_COMPLETE_BIT,&sdport->pending_events)){
                        /* xfer pending alert */
                        if(dwc51_chk_xfer_done(cmd_info)){
                            /* data xfer finished */
                            set_bit(XFER_COMPLETE_BIT,&sdport->completed_events);
                            clear_bit(XFER_COMPLETE_BIT,&sdport->pending_events);
                            /* no err & data xfer finish.*/
                            mmctask("no err & data xfer finish.\n");
                            state = STATE_PRE_COMPLETE;
                            break;
                        }else{
                            /* xfer done, but some status not finish.
                                polling on next round */
                            state = STATE_SENDING_DATA;
                            prev_state = state;
                            mmctask("xfer done,polling on next round\n");
                            tasklet_schedule(&sdport->polling_tasklet);
                            break;
                        }
                    }else{
                        /* no xfer pending alert */
                        /* must xfer pending alert can come here.*/
                        state = STATE_PRE_COMPLETE;
                        set_bit(CMD_UNEXPT_BIT,&cmd_info->cmd_status);
                        EM_ALERT("pending_events=0x%lx completed_events=0x%lx\n",
                            sdport->pending_events,sdport->completed_events);
                        WARN(1,"%s: pending_events=0x%lx completed_events=0x%lx\n",
                                DRIVER_NAME,sdport->pending_events,sdport->completed_events);
                        break;
                    }
                }else{
                    /* no cmd completed */
                    /* The cmd must be complete
                        and  the xfer must be pending!!!
                        warn on for debug */
                    state = STATE_PRE_COMPLETE;
                    EM_ALERT("pending_events=0x%lx completed_events=0x%lx\n",
                        sdport->pending_events,sdport->completed_events);
                    WARN(1,"%s: pending_events=0x%lx completed_events=0x%lx\n",
                            DRIVER_NAME,sdport->pending_events,sdport->completed_events);
                    break;

                }

            }

        case STATE_PRE_COMPLETE:
            /* prepare the cmd finish.
                set error reason properly.
                check if need to recover.  */
            mmctask("STATE_PRE_COMPLETE\n");
            set_bit(TIMER_STOP_BIT,&cmd_info->cmd_status);
            del_timer(&sdport->timer);
            recover_mothod = dwc51_chk_err_tpye(cmd_info);
            if(recover_mothod){
                mmctask("STATE_ERROR_HADLE\n");
                state = STATE_ERROR_HADLE;
                break;
            }

            if(test_bit(SECOND_CMD_BIT,&cmd_info->cmd_status)){
                mmcdbg("handle secend cmd\n");
                state = STATE_WAIT_HOST;
                sdport->busy_cnt = 0;
                break;
            }else{
                state = STATE_REQUEST_END;
                /* fall through */
            }

        case STATE_REQUEST_END:
            mmctask("STATE_REQUEST_END\n");
            rtksd_show_finish_status(sdport);
            rtksd_clear_event(sdport);
            rtksd_request_done(sdport);
            sdport->task_state = STATE_IDLE;
            mrq = sdport->mrq;
            sdport->mrq = NULL;
            spin_unlock_irqrestore(&sdport->lock, flags);
            mmc_request_done(sdport->mmc, mrq);
            /* return out */
            mmcdbg("exit\n"); //mmcdbg;mmctask
            return;

        case STATE_ERROR_HADLE:
            /* does anything need to handle? */
            mmcdbg("STATE_ERROR_HADLE\n");

            rtksd_host_recovery(sdport,recover_mothod);

            state = STATE_REQUEST_END;
            break;

        case STATE_WAIT_HOST:
            if(em_inl(DWC51_PSTATE_REG_R) & HOST_IDLE_MASK){
                /* host bust still, call next round */
                if ((sdport->busy_cnt <5) || (!(sdport->busy_cnt % 10))) {
                    mmcmsg1("wait host busy. (%u)\n",sdport->busy_cnt);
                }
                sdport->busy_cnt++;
                state = STATE_WAIT_HOST;
                prev_state = state;
                tasklet_schedule(&sdport->polling_tasklet);
                break;
            }else{
                state = STATE_SEND_SECOND;
                sdport->busy_cnt = 0;
                /* fall through */
            }
        case STATE_SEND_SECOND:
            if(test_bit(SECOND_CMD_BIT,&cmd_info->cmd_status)){

                if(cmd_info->cmd == sdport->mrq->sbc ){
                    /* manual cmd23 done */
                    //spin_unlock_irqrestore(&sdport->lock, flags);
                    //dwc51_wait_host_busy(HOST_IDLE_MASK,50);
                    mmcdbg("manual cmd23 done,prepare main cmd\n");
                    memset(cmd_info, 0, sizeof(struct sd_cmd_pkt));

                    if(rtksd_send_command(sdport, sdport->mrq->cmd)){
                        /* send second cmd fail, but no any hw act,
                            go to finish request directly*/
                        EM_ALERT("send second  cmd fail.\n");
                        state = STATE_REQUEST_END;
                        break;
                    }

                    /* Beware: Don't change any status after call rtksd_send_command(). */
                    mmcdbg("exit\n"); //mmcdbg;mmctask
                    spin_unlock_irqrestore(&sdport->lock, flags);
                    return;

                }else if(cmd_info->cmd == sdport->mrq->stop){
                    /* main cmd done, act cmd12. */
                    /* no support manual cmd12 now, should not has this case */
                    EM_ALERT("no support manual cmd12 now, should not has this case!\n");
                    WARN(1,"%s: no support manual cmd12 now, should not has this case!\n,",
                        DRIVER_NAME);

                }else{
                    /* should not has this case */
                    EM_ALERT("should not has this case\n");
                    WARN(1,"%s: should not has this case\n",DRIVER_NAME);

                }

            }else{
                /* no SECOND_CMD_BIT but goto STATE_SEND_SECOND??  */
                EM_ALERT("no SECOND_CMD_BIT but goto STATE_SEND_SECOND??\n");
                WARN(1,"%s: no SECOND_CMD_BIT but goto STATE_SEND_SECOND??\n",DRIVER_NAME);

            }
            /* all these unexpect status, finish tasklet. for debug  */
            state = STATE_IDLE;
            sdport->task_state = prev_state = state;
            mmcdbg("exit\n"); //mmcdbg;mmctask
            return;

        }

    } while (state != prev_state);

    sdport->task_state = state;
    spin_unlock_irqrestore(&sdport->lock, flags);
    mmcdbg("exit\n"); //mmcdbg;mmctask

}

/************************************************************************
 * clear error bit
 * this func must be called after rtksd_show_finish_status();
 ************************************************************************/
static
void rtksd_clear_event(struct rtksd_host *sdport)
{
    struct sd_cmd_pkt * cmd_info = &sdport->cmd_info;

    clear_bit(ERR_INTERRUPT_BIT,&sdport->pending_events);
    clear_bit(DATA_UNEXPT_BIT,&cmd_info->cmd_status);
    clear_bit(CMD_UNEXPT_BIT,&cmd_info->cmd_status);
    clear_bit(CMD_ERR_BIT,&cmd_info->cmd_status);

}

/************************************************************************
 * request done.
 * report cmd finished and release SCPU occupy
 * Don't call mmc_request_done() another way!!!
 * All message of process can show here.
 * Note: Cause by mmc_request_done() reason,
 *          don't call this func in spinlock
 ************************************************************************/
static
void rtksd_request_done(struct rtksd_host *sdport)
{
    struct sd_cmd_pkt * cmd_info;

    mmcdbg("entry\n");

    cmd_info = &sdport->cmd_info;
    if(test_bit(REQ_FINISH_BIT, &cmd_info->cmd_status)){
        /* just for debug.
            There should not be a situation where
            REQ_FINISH_BIT is alert, but it comes here. */
        WARN(1,"%s: REQ_FINISH_BIT alert already!\n",DRIVER_NAME);
    }

    /* only ADTC cmd needs this. */
    if(test_bit(SG_MAPPED_BIT, &cmd_info->cmd_status)){
        mmcdbg("dma_unmap_sg\n");
        if(cmd_info->data->flags & MMC_DATA_READ){
            dma_unmap_sg( mmc_dev(sdport->mmc), cmd_info->data->sg,
                    cmd_info->data->sg_len,  DMA_FROM_DEVICE);
        }else{
            dma_unmap_sg( mmc_dev(sdport->mmc), cmd_info->data->sg,
                    cmd_info->data->sg_len,  DMA_TO_DEVICE);
        }
        clear_bit(SG_MAPPED_BIT, &cmd_info->cmd_status); //maybe no needs clear bit here

#ifdef DEBUG_BUFFER
        if ((rtk_emmc_test_ctl & EMMC_CHK_BUF) &&
            (cmd_info->data->flags & MMC_DATA_READ)) {
            emmc_vreify_buf(cmd_info,0);
        }
#endif

    }

    dwc51_release_cpu_occupy(sdport);
    set_bit(REQ_FINISH_BIT,&cmd_info->cmd_status);

    mmcdbg("exit.\n");
	_rtd_hwsem_unlock(SEMA_HW_SEM_6, SEMA_HW_SEM_6_SCPU_0);
}

/************************************************************************
 *
 ************************************************************************/
static
int rtksd_send_command(struct rtksd_host *sdport, struct mmc_command *cmd)
{
    struct sd_cmd_pkt *cmd_info = NULL;
    int ret_err = 0;
    //unsigned long flags;

    mmcdbg("entry\n");
    BUG_ON(!sdport);
    BUG_ON(!cmd);

    //spin_lock_irqsave(&sdport->lock,flags);

    cmd_info = &sdport->cmd_info;
    cmd_info->cmd = cmd;

    cmd->error = -EINPROGRESS;
    if (cmd->data){
        mmcdbg("call dw_em_Stream\n");
        cmd->data->error = -EINPROGRESS;
        cmd_info->data = cmd->data;
        ret_err = dw_em_Stream(cmd_info);
    }else{
        mmcdbg("call dwc51_SendCmd\n");
        dwc51_SendCmd(cmd_info);
    }

    if(cmd->opcode == MMC_SWITCH){
         if((cmd->arg & 0xffff00ff) == 0x03b30001) {
            /* partition switch */
            if((cmd->arg & 0x0000ff00) == 0x300){
                /* switch to rpmb  */
                set_bit(PORT_IS_RPMB,&sdport->port_status);
            }else{
                /* switch out rpmb  */
                clear_bit(PORT_IS_RPMB,&sdport->port_status);

            }
         }
    }

    //spin_unlock_irqrestore(&sdport->lock, flags);

    return ret_err;

}

/************************************************************************
 *
 ************************************************************************/
static
void dw_em_request(struct mmc_host *host, struct mmc_request *mrq)
{
    struct rtksd_host *sdport = NULL;
    struct mmc_command *cmd = NULL;
    struct sd_cmd_pkt *cmd_info = NULL;
    unsigned long flags;

    mmcdbg("entry\n");
    sdport = mmc_priv(host);

    BUG_ON(!mrq);
    BUG_ON(!(mrq->cmd));
    BUG_ON(!sdport);
    BUG_ON(sdport->mrq);

    /*
     * The flag "rtk_emmc_raw_op" is for store log while kernel panic.
     * When this flag alert, skip all request come from system.
     */
    while(rtk_emmc_raw_op){
        EM_ERR("wait RAW func finish!!!!!!!!!!!!!!!!!!!\n");
        msleep(10);
    }

	_rtd_hwsem_lock(SEMA_HW_SEM_6, SEMA_HW_SEM_6_SCPU_0);

    /* check busy before lock
        CMD_INHIBIT for cmd
        HOST_IDLE_MASK for cmd & dtat
        Think about. Does It need to wait for all signals to idle? */
    // wait 50ms for cmd & data ready
    if (dwc51_wait_host_busy(HOST_IDLE_MASK,50)) {
        EM_ERR("host abnormal!\n");
        mrq->cmd->error = -EIO;
        mrq->cmd->retries = 0;
        goto busy_out;
    }

    spin_lock_irqsave(&sdport->lock,flags);

    sdport->mrq = mrq;
    if (!test_bit(PORT_CARD_DETECTED,&sdport->port_status)) {
        EM_ERR("card not present!\n");
        mrq->cmd->error = -ENOMEDIUM;
        mrq->cmd->retries = 0;
        goto fail_out;
    }

    cmd_info = &sdport->cmd_info;
    memset(cmd_info, 0, sizeof(struct sd_cmd_pkt));

    cmd = mrq->cmd;
    mmcmsg1("main cmd_idx=%2u arg=0x%08x\n",
            mrq->cmd->opcode,mrq->cmd->arg);

    /* use auto23 currently. */
    if(mrq->sbc){
       /* 5.4 rpmb must use sbc control */
        cmd_info->sbc = mrq->sbc;
        if(mrq->sbc->opcode != MMC_SET_BLOCK_COUNT ){
            /* kernel bug case */
            WARN(1,"%s: sbc cmd_idx=%2u arg=0x%08x\n",
                    DRIVER_NAME,mrq->sbc->opcode,mrq->sbc->arg);
        }else{
            mmcdbg("sbc cmd_idx=%2u arg=0x%08x\n",
                    mrq->sbc->opcode,mrq->sbc->arg);

            if (test_bit(PORT_IS_RPMB,&sdport->port_status)) {
                set_bit(SECOND_CMD_BIT,&cmd_info->cmd_status);
                cmd = mrq->sbc;
                mmcmsg1("rpmb manual cmd23\n");
            }else{
                if(mrq->sbc->arg & 0xffff0000){
                    /* cmd23 have more argument need to handle
                        alert second cmd flag. */
                    set_bit(SECOND_CMD_BIT,&cmd_info->cmd_status);
                    cmd = mrq->sbc;
                    mmcmsg1("manual cmd23\n");
                }else{
                    mmcdbg("auto cmd23\n");
                    cmd_info->auto_cmd_type = AUTO_CMD23;
                }
            }
        }
    }else if(mrq->stop){
        EM_ALERT("stop cmd_idx=%2u arg=0x%08x\n",
                mrq->stop->opcode,mrq->stop->arg);
        WARN(1,"%s: no support open-end method now!!!\n",DRIVER_NAME);
        cmd_info->stop = mrq->stop;

        /* run main cmd first at open-end mathod. */
        /* skip auto stop if auto cmd23 has alerted  */
        mmcdbg("auto cmd12\n");
        cmd_info->auto_cmd_type = AUTO_CMD12;
    }

    if(dwc51_get_cpu_occupy(sdport)){
        EM_ERR("can't get occupy!\n");
        mrq->cmd->error = -EIO;
        goto fail_out;
    }

    if( rtksd_send_command(sdport, cmd)){

        goto fail_out;
    }

    /* The driver accepts this task and triggers HW process.
       The remaining work will be handled by the interrupt function.
       Or the timeout processing mechanism. */
    mmcdbg("exit\n");
    spin_unlock_irqrestore(&sdport->lock, flags);
    return;

fail_out:
    /* if fail. report done to mmc core.
        Returning from here means that there is actually no hardware operation. */
	rtksd_request_done(sdport);
    sdport->mrq = NULL;
    spin_unlock_irqrestore(&sdport->lock, flags);
busy_out:
    mmc_request_done(sdport->mmc, mrq);
    mmcdbg("exit\n");
    return;

}

/************************************************************************
 * Call by MMC Core
 ************************************************************************/
static
void dw_em_hw_reset(struct mmc_host *host)
{
    struct rtksd_host *sdport;
    sdport = mmc_priv(host);
    mmcdbg("entry- MMC core ops \n");

    dw_em_reset_host(sdport,0);
    rtkemmc_disable_int(sdport);    // pair with following
    rtkem_clr_sta(sdport);

    dwc51_reset_card(sdport);

    rtkemmc_enable_int(sdport);     // pair with above
    mmcdbg("exit\n");

}

/************************************************************************
 *
 ************************************************************************/
static
void dw_em_set_ios(struct mmc_host *host, struct mmc_ios *ios)
{
    struct rtksd_host *sdport;
    u32 tmp_clock = 0;
    u32 tmp_bits = 0;
    u32 driving = 0;

    mmcmsg1("entry\n");
    mmcmsg1("bus_mode   = %u\n",ios->bus_mode);
    mmcmsg1("clock      = %u\n",ios->clock);
    mmcmsg1("timing     = %u\n",ios->timing);
    mmcmsg1("bus_width  = %u\n",ios->bus_width);
    mmcmsg1("power_mode = %u\n",ios->power_mode);

    sdport = mmc_priv(host);
    driving = DRIVING_COMMON;

    rtkemmc_disable_int(sdport);    // pair with following

    if (ios->bus_mode == MMC_BUSMODE_PUSHPULL){
        mmcdbg("MMC_BUSMODE_PUSHPULL\n");
        if (ios->bus_width == MMC_BUS_WIDTH_8){
            tmp_bits = MMC_BUS_WIDTH_8;
            mmcdbg("MMC_BUS_WIDTH_8\n");
        }else if (ios->bus_width == MMC_BUS_WIDTH_4){
            tmp_bits = MMC_BUS_WIDTH_4;
            mmcdbg("MMC_BUS_WIDTH_4\n");
        }else{
            tmp_bits = MMC_BUS_WIDTH_1;
            mmcdbg("MMC_BUS_WIDTH_1\n");
        }

        tmp_clock = ios->clock;
        if (ios->timing == MMC_TIMING_MMC_HS400) {
            mmcmsg1("MMC_TIMING_MMC_HS400\n");
            //dw_em_set_ds_delay(sdport, sdport->cur_ds_tune);
            if (tmp_clock > CLK_200Mhz) {
                tmp_clock = CLK_200Mhz;
            }
            driving = DRIVING_HS400;
        } else if (ios->timing == MMC_TIMING_MMC_HS200) {
            mmcmsg1("MMC_TIMING_MMC_HS200\n");
            if (tmp_clock > CLK_200Mhz) {
                tmp_clock = CLK_200Mhz;
            }
            driving = DRIVING_HS200;
        } else if (ios->timing == MMC_TIMING_UHS_DDR50) {
            mmcmsg1("MMC_TIMING_UHS_DDR50\n");
            if(tmp_clock > CLK_50Mhz){
                tmp_clock = CLK_50Mhz;
            }
            driving = DRIVING_HS50;
        } else if ((ios->timing == MMC_TIMING_SD_HS) ||
            (ios->timing == MMC_TIMING_MMC_HS)) {
            mmcmsg1("MMC_TIMING_MMC_HS\n");
            if(tmp_clock > CLK_50Mhz){
                tmp_clock = CLK_50Mhz;
            }
            driving = DRIVING_HS50;
        } else {
            mmcmsg1("MMC_TIMING_LEGACY\n");
            if(tmp_clock>CLK_25Mhz){
                tmp_clock = CLK_25Mhz;
            }
            driving = DRIVING_25M;
        }

    } else {
        /* MMC_BUSMODE_OPENDRAIN */
        mmcmsg1("MMC_BUSMODE_OPENDRAIN\n");
        tmp_clock = CLK_200Khz;
        tmp_bits = MMC_BUS_WIDTH_1;
    }

    mmcmsg1("tmp_clock=%u tmp_bits=%u driving=%u\n",tmp_clock,tmp_bits,driving);

    /* go into func dw_em_set_clk() to check real clock */
    dw_em_set_clk(sdport,tmp_clock);
    if(tmp_bits != sdport->bus_width){
        dwc51_set_bits(sdport, tmp_bits);
    }
    if(driving != sdport->cur_driving){
        dw_em_set_IO_driving(sdport,driving);
    }
    if(ios->timing != sdport->cur_timing ){
        dwc51_set_timing(sdport,ios->timing);
    }

    /* eMMC Power control */
    switch (ios->power_mode) {
    case MMC_POWER_UP:
    case MMC_POWER_ON:
        /* seems the DWC51_PWR_CTRL_R8 can't be disable adn enable simplely,
            need discuss with DIC */
        mmcmsg1("power_mode = MMC_POWER_UP DWC51_PWR_CTRL_R8=0x%x\n",em_inb(DWC51_PWR_CTRL_R8));
        //PWR_CTRL_R.SD_BUS_VOL_VDD1 set 1V8 for eMMC
        // !!! But it doesn't seem to be needed here? move to ios
        //em_outb(DWC51_PWR_CTRL_R8,SD_BUS_VOL_VDD1(eMMC_1V8) | SD_BUS_PWR_VDD1);
        break;
    case MMC_POWER_OFF:
        mmcmsg1("power_mode = MMC_POWER_OFF\n");
        /* Power down slot */
        //em_outb(DWC51_PWR_CTRL_R8,0);
        break;
    default:
        break;
    }

    rtkemmc_enable_int(sdport); // pair with above

}

/************************************************************************
 *
 ************************************************************************/
static
void rtkem_chk_card_insert(struct rtksd_host *sdport)
{
    struct mmc_host *host=sdport->mmc;

    mmcdbg("entry n");
    host->ops = &rtkemmc_ops;

    /* enable MMC pin_mux first */
    set_emmc_pin_mux();

    /* CRT reset & IP config */
    dw_em_reset_host(sdport,0);

    /* enable PLL of eMMC */
    dw_em_set_clk(sdport,CLK_200Khz);

    /* pad driveig  */
    dw_em_set_IO_driving(sdport,DRIVING_COMMON);

    set_bit(PORT_CARD_DETECTED,&sdport->port_status);
    mmcdbg("exit n");

}

/************************************************************************
 * After an error occurs, is there any action required to reset the host?
 ************************************************************************/
static
int rtksd_host_recovery(struct rtksd_host *sdport,u32 recover_mothod)
{
    /* is there any thing to do?? */
    if(!recover_mothod){
        EM_ERR("no assigne recovery mode. who call this...\n");
        return 0;
    }

    if(recover_mothod == RECOVER_OVER_CRT_RST){
        dw_em_reset_host(sdport,1);
        EM_ERR("RECOVER_OVER_CRT_RST\n");
    }else if(recover_mothod == RECOVER_OVER_DWC_RST){
        dw_em_reset_dwc51(sdport,(u8)(SW_RST_DAT),1);
        EM_ERR("RECOVER_OVER_DWC_RST\n");
    }else if(recover_mothod == RECOVER_OVER_DWC_CMD_RST){
        //dw_em_reset_dwc51(sdport,(u8)SW_RST_CMD,1);
        EM_ERR("RECOVER_OVER_DWC_CMD_RST(skip SW_RST_CMD)\n");
    }else if(recover_mothod == RECOVER_OVER_DWC_ALL_RST){
        /* I think that we don't need this case.*/
        //dw_em_reset_dwc51(sdport,(u8)SW_RST_ALL,0);
        EM_ERR("RECOVER_OVER_DWC_ALL_RST\n");
    }else{
        EM_ERR("unknow recover mode. (recover_mothod=0x%x)\n",
                recover_mothod);
    }

    return 0;
}

/************************************************************************
 * notice!!! call this func before any recovery process.
 * Currently, the status will be printed only when an error occurs,
 * but for debugging reason,
 * it is still called before the successful command ends.
 ************************************************************************/
static
void rtksd_show_finish_status(struct rtksd_host *sdport)
{
    /* is there any thing to show?? */
    struct sd_cmd_pkt *cmd_info = NULL;
    u32 cmd_idx = 0;
    u32 cmd_flag = 0;
    u32 xfermode = 0;
    u32 int_status = 0;
    u32 err_status = 0;
    u32 pstate = 0;
    u32 wrap_sta0 = 0;
    bool show = false;

    cmd_info = &sdport->cmd_info;
    if (test_bit(ERR_INTERRUPT_BIT,&sdport->pending_events)) {
        EM_ERR("Cmd fail!!! (err alert)\n");
        show = true;
    }else if(test_bit(CMD_TMOUT_BIT,&cmd_info->cmd_status)){
        EM_ERR("Cmd fail!!! (timeout)\n");
        show = true;
    }

    if (show) {
        int_status = sdport->int_status;
        err_status = ((int_status >> 16)& 0xFFFFU);
        xfermode = cmd_info->xfermode;
        cmd_flag = cmd_info->cmd_flag;
        cmd_idx = CMD_INDEX_GET(cmd_flag);
        pstate = em_inl(DWC51_PSTATE_REG_R);    // can record to avoid read too much??
        wrap_sta0 = em_inl(EM51_WARP_STATUS0);  // can record to avoid read too much??

        if (!test_bit(PORT_TUNING_STATE,&sdport->port_status)) {
            EM_ERR("Cmd fail!!! \n");
            EM_ERR("    cmd_idx=%d cmd_arg=0x%08x cmd_flag=0x%08x xfermode=0x%08x\n"
                    "        err_status=0x%08x int_status=0x%08x loop_cnt=0x%x\n"
                    "        pstate=0x%08x wrap_sta0=0x%08x\n",
                cmd_idx,cmd_info->cmd->arg,cmd_flag,xfermode,
                err_status,int_status,cmd_info->tmout,
                pstate,wrap_sta0);
            if (err_status & ADMA_ERR) {
                EM_ERR("    ADMA err status=0x%02x\n", cmd_info->adma_err_stat);
            }
            if (xfermode & AUTO_CMD_ENABLE_MASK) {
                if(err_status & AUTO_CMD_ERR){
                    EM_ERR("    AUTO_CMD err status=0x%02x\n", cmd_info->auto_cmd_stat);
                }
            }
        }
    }
}

/************************************************************************
 *
 ************************************************************************/
static
void rtksd_timeout_timer(struct timer_list *t)
{
    struct rtksd_host *sdport = NULL;
    struct sd_cmd_pkt *cmd_info = NULL;
    struct mmc_request  *mrq = NULL;
    u32 recover_mothod = 0;
    unsigned long flags;

    EM_ERR("%s(%d):entry\n",__func__,__LINE__);

    sdport = from_timer(sdport, t, timer);
    cmd_info = &sdport->cmd_info;

    if(test_bit(TIMER_STOP_BIT,&cmd_info->cmd_status)){
        //clear_bit(TIMER_STOP_BIT,&cmd_info->cmd_status);
        /* I don't want this timer wakeup again in this cmd life cycle. */
        EM_INFO("timer has been stoped!\n");
        return;
    }
    //if (rtk_emmc_debug_log){
    //    EM_INFO("%s:running\n", __func__);
    //}

    spin_lock_irqsave(&sdport->lock,flags);

    set_bit(CMD_TMOUT_BIT,&cmd_info->cmd_status);

    /* simple recover flow */
    recover_mothod = dwc51_chk_err_tpye(cmd_info);
    rtksd_host_recovery(sdport,recover_mothod);

    /*in some case, I think It need to check status even timeout.
        but skip now just report error.  */
    rtksd_show_finish_status(sdport);
    rtksd_clear_event(sdport);
	rtksd_request_done(sdport);
    mrq = sdport->mrq;
    sdport->mrq = NULL;
    spin_unlock_irqrestore(&sdport->lock, flags);
    mmc_request_done(sdport->mmc, mrq);
    mmcdbg("exit\n");
    return;

}


/************************************************************************
 * 1. It's check error reason then assign err type report to mmc core.
 * 2. recover host over reset dwc or CRT.
 *
 * I'm not sure yet what situation it need to reset the dwc or need to reset the CRT.
 * Or no reset action is needed.
 * The only reset action currently is to reset dwc when an error occurs in data.
 * Other reset requirements will be tested after the IC comes back.
 ************************************************************************/
static
int dwc51_chk_err_tpye(struct sd_cmd_pkt *cmd_info){
    u32 int_status = 0;
    u32 auto_cmd_sta = 0;
    u32 err_status_h = 0;
    u32 xfermode = 0;
    u32 auto_cmd_mode = 0;

    int recover_mothod = 0;
    int cmd_err = 0;
    int data_err = 0;
    struct rtksd_host *sdport   = NULL;
    struct mmc_command *cmd = cmd_info->cmd;
    struct mmc_command *sbc = NULL;
    struct mmc_command *stop = NULL;
    struct mmc_data *data = cmd->data;

    mmcdbg("ertry\n");
    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);

    if(test_bit(CMD_COMPLETE_BIT,&sdport->pending_events)){
        xfermode = cmd_info->xfermode;
        err_status_h = int_status = sdport->int_status;
        auto_cmd_sta = cmd_info->auto_cmd_stat;
        mmcdbg("int_status=0x%08x \n",int_status);

        /* assign cmd error status of sub-cmd  */
        if(!test_bit(SECOND_CMD_BIT,&cmd_info->cmd_status)){
            auto_cmd_mode = AUTO_CMD_ENABLE_GET(xfermode);
            if(auto_cmd_mode == AUTO_CMD23_ENABLED){
                sbc = cmd_info->sbc;
                BUG_ON(!sbc);
                mmcdbg("AUTO_CMD23_ENABLED\n");
                if(auto_cmd_sta & AUTO_CMD_TOUT_ERR)
                    sbc->error = -ETIMEDOUT;
                else if(auto_cmd_sta & ERR_AUTO_CMD23_MASK )
                    sbc->error = -EIO;
                else
                    sbc->error = 0;
            }else if(auto_cmd_mode == AUTO_CMD12_ENABLED){
                mmcdbg("AUTO_CMD12_ENABLED\n");
                stop = cmd_info->stop;
                BUG_ON(!stop);
                if(auto_cmd_sta & AUTO_CMD_TOUT_ERR)
                    stop->error = -ETIMEDOUT;
                else if(auto_cmd_sta & ERR_AUTO_CMD12_MASK )
                    stop->error = -EIO;
                else if(auto_cmd_sta & AUTO_CMD12_NOT_EXEC){
                    EM_INFO("The main cmd failed, no issue auto cmd12");
                    stop->error = 0;
                }else
                    stop->error = 0;
            }

        }

        /* main cmd error type assign */
        if(err_status_h & CMD_TOUT_ERR_H){
            cmd_err = -ETIMEDOUT;
            mmcdbg("CMD ETIMEDOUT\n");
        }else if(err_status_h & (CMD_CRC_ERR_H | CMD_IDX_ERR_H)){
            cmd_err  = -EILSEQ;
            mmcdbg("CMD EILSEQ\n");
        }else if(err_status_h & CMD_END_BIT_ERR_H){
            cmd_err  = -EIO;
            mmcdbg("CMD EIO\n");
        }else{
            cmd_err  = 0;
            mmcdbg("CMD NO ERR\n");
        }

        /* data error type assign */
        if (err_status_h & DATA_TOUT_ERR_H){
            data_err = -ETIMEDOUT;
            recover_mothod = RECOVER_OVER_DWC_RST;
            mmcdbg("data ETIMEDOUT\n");
        }else if (err_status_h & ERR_DATA_REASON_MASK_H){
            data_err = -EILSEQ;
            //recover_mothod = RECOVER_OVER_CRT_RST;    //which is good ?
            recover_mothod = RECOVER_OVER_DWC_RST;
            mmcdbg("data EILSEQ\n");
        }else{
            data_err = 0;
            mmcdbg("data NO ERR\n");
        }

        /* unexpect irq error bit */
        if (err_status_h & ERR_UNEXPECT_MASK_H){
            EM_ALERT("what pappen?? unexpect=0x%08x\n",
                    (err_status_h & ERR_UNEXPECT_MASK_H));
            //cmd_err = 0;
            //data_err = 0;
            /* Do not overwrite acknowledged error status */
            WARN(1,"%s: what pappen?? unexpect=0x%08x\n",
                    DRIVER_NAME,(err_status_h & ERR_UNEXPECT_MASK_H));
        }
    }

    if(test_bit(CMD_TMOUT_BIT,&cmd_info->cmd_status)){
        if(!cmd_err)
            cmd_err = -ETIMEDOUT;
        if(!data_err)
            data_err = -ETIMEDOUT;

        recover_mothod = RECOVER_OVER_DWC_RST;
    }

    /* just for debug */
    if(test_bit(ERR_INTERRUPT_BIT,&sdport->pending_events)){
        if(!(cmd_err | data_err)){
            WARN(1,"%s: error alert but no err? err_status=0x%08x\n",
                    DRIVER_NAME,(err_status_h >> 16) &  0xffff);

        }
    }

    /* just for debug */
    if(test_bit(CMD_UNEXPT_BIT,&cmd_info->cmd_status)){
        if(!cmd_err)
            cmd_err  = -EIO;
    }
    /* just for debug */
    if(test_bit(DATA_UNEXPT_BIT,&cmd_info->cmd_status)){
        if(!data_err)
         data_err = -EILSEQ;
    }

    cmd->error = cmd_err;
    mmcdbg("cmd->error = %d\n",cmd->error);
    if(data){
        data->error = data_err;
        if(!data->error){
            data->bytes_xfered = cmd_info->total_byte_cnt;
        }
        mmcdbg("data->error = %d\n",data->error);
    }
    if(in_irq()){
        /* In irq, It not handle error status, just set err=0. */
        if(data_err | data_err) {
            WARN(1,"%s: In irq, It should not error status.\n",DRIVER_NAME);
        }
    }

    return recover_mothod;
}


/************************************************************************
 * return 0; if
 *      1. RUBS err of wrap. set err panding bit.
 *      2. warp dma not idle.
 *      3. IP not idle
 ************************************************************************/
static
int dwc51_chk_xfer_done(struct sd_cmd_pkt *cmd_info){

    u32 pstate = 0;
    u32 wrap_sta0 = 0;
    int xfer_done = 0;
    struct rtksd_host *sdport = NULL;

    sdport = container_of(cmd_info,struct rtksd_host,cmd_info);
    pstate = em_inl(DWC51_PSTATE_REG_R);
    wrap_sta0 = em_inl(EM51_WARP_STATUS0);


    if(IS_R1B_CMD(cmd_info->cmd_flag)){
        xfer_done = 1;
         mmcdbg("r1b xfer done\n");
    }else{
        if(wrap_sta0 & RBUS_ERR){
            /*there is no idea what situation is occur this error. print message */
            sdport->int_status |= WRAP_STA0_RBUS_H;
            mmcdbg("RBUS error!!!\n");
        }

        if( pstate & HOST_IDLE_MASK){
            mmcdbg("Present State busy...(pstate=0x%08x)\n",pstate);
        }else if(wrap_sta0 & (DBUS_DMA_BUSY | DBUS_WRITE_FLAG)){
            mmcdbg("dbus busy...(wrap_sta0=0x%08x)\n",wrap_sta0);
        }else{
            xfer_done = 1;
        }

        if(in_irq()){
            /* for debug only */
            set_bit(DATA_IRQ_BIT,&cmd_info->cmd_status);
#if 0
            /* for debug only, force go to tasklet */
        xfer_done = 0;
#endif
        }

    }

    return xfer_done;
}


/************************************************************************
 * check cmdq status
 * cmdq need rtksd_host, may not need sd_cmd_pkt.....
 ************************************************************************/
static
int dwc51_chk_cmdq_done(struct rtksd_host *sdport){
    u32 int_status = 0;
    int cmdq_done = 0;
    int cmd_error = 0;
    int data_error = 0;
    unsigned long flags;
    //struct cqhci_host *cq_host = NULL;

    //cq_host = sdport->cq_host;

    spin_lock_irqsave(&sdport->lock, flags);
    int_status = sdport->int_status;

    if(int_status & ERR_CMD_REASON_MASK_H ){
        cmd_error = 1;
    }
    if(int_status & ERR_DATA_REASON_MASK_H ){
        data_error = 1;
    }

    spin_unlock_irqrestore(&sdport->lock, flags);
    cmdq_done = cqhci_irq(sdport->mmc, 0, cmd_error, data_error);

    return cmdq_done;
}


/************************************************************************
 *
 ************************************************************************/
static
irqreturn_t dw_em_irq(int irq, void *dev) {
    struct rtksd_host *sdport = dev;
    struct sd_cmd_pkt *cmd_info = &sdport->cmd_info;
    struct mmc_request  *mrq = NULL;
    int irq_handled = 0;
    u32 int_status = 0;
    u32 reg1,reg2;
    unsigned long flags;

    spin_lock_irqsave(&sdport->lock, flags);
    mmcdbg("entry\n"); //mmcirq;mmcdbg

    reg1 = em_inl(RTK_EM_DW_SRST);
    reg2 = em_inl(RTK_EM_DW_CLKEN);

    if( !(reg1 & RSTN_EM_DW)
     || !(reg2 & CLKEN_EM_DW) )
    {
        EM_ERR("%s: Clock has been disable, no interrupt service\n",
                DRIVER_NAME);

#define SHOW_CRT_CLK_INFO
#ifdef SHOW_CRT_CLK_INFO
        EM_ERR("%s: RTK_EM_DW_SRST=0x%08x RTK_EM_DW_CLKEN=0x%08x\n",
                DRIVER_NAME,reg1,reg2);
#endif
    }else

    {
        int_status = dwc51_int_act(INT_GET_STA, 0, 0);
        mmcdbg("int_status=0x%08x\n",int_status);


        set_bit(IRQ_ALERT_BIT,&cmd_info->cmd_status);
        if(int_status & MMC_INT_EVENT){
            mmcirq("MMC_INT_EVENT\n");
            irq_handled = 1;
            /* move the action that update 'int_status' to last.
                make sure the "dport->int_status"
                in other palace is the final.
                "insdport->int_status |= int_status;""
                */
            if(int_status & ERR_INTERRUPT ){
                mmcirq("ERR_INTERRUPT\n");
                set_bit(ERR_INTERRUPT_BIT,&sdport->pending_events);
                if(int_status & ERR_CMD_REASON_MASK_H){
                    set_bit(CMD_ERR_BIT,&cmd_info->cmd_status);
                }
                /* rec status of ADMA,
                    but it's no need to check ADMA at cmd state  */
                if(int_status & ADMA_ERR_H){
                     mmcirq("ADMA_ERR\n");
                    cmd_info->adma_err_stat = em_inb(DWC51_ADMA_ERR_STAT_R8);
                }
                /* rec status of auto cmd */
                if(int_status & AUTO_CMD_ERR_H){
                    mmcirq("AUTO_CMD_ERR\n");
                    cmd_info->auto_cmd_stat = em_inw(DWC51_AUTO_CMD_STAT_R16);
                }
            }

            /* Because It's not sure whether It can still read the status
                of amda and auto after clearing the interrupt pending bit,
                so I keep it here before clearing the interrupt. */
            dwc51_int_act(INT_CLR_RSN, int_status, 0);

            /* cmdq on,handle cmdq event only */
            if (int_status & CQE_EVENT ) {
                if (!sdport->cqe_on) {
                    mmcmsg3("no cqe on but CQE_EVENT !?!?!?\n");
                    //WARN(1,"%s: no cqe on but CQE_EVENT !?!?!?\n",DRIVER_NAME);
                }

                if (test_bit(ERR_INTERRUPT_BIT,&sdport->pending_events)) {
                    /* alexkh: what should we do if error occur while cmdq? */
                    mmcmsg3("CQE_EVENT !?!?!?\n");
                    WARN(1,"%s: Error occur while cmdq !!!\n",DRIVER_NAME);
                } else {
                    mmcmsg3("CQE_EVENT alert\n");
                    //set_bit(CQE_EVENT_BIT,&sdport->pending_events);
                }
                sdport->int_status |= int_status;
                spin_unlock_irqrestore(&sdport->lock, flags);
                dwc51_chk_cmdq_done(sdport);
                mmcmsg3("cqe-exit\n");
                return IRQ_HANDLED;
            }

            if(!sdport->mrq){
                mmcmsg3("cqe-int_status=0x%08x\n",int_status);
                spin_unlock_irqrestore(&sdport->lock, flags);
                return IRQ_HANDLED;
            }

            /* read again & check, just for debug */
            if(int_status & ADMA_ERR_H){
                /* After confirming the hardware behavior, this check can be removed. */
                if(em_inb(DWC51_ADMA_ERR_STAT_R8) != cmd_info->adma_err_stat){
                    WARN(1,"%s: ADMA_ERR\n",DRIVER_NAME);
                }
            }
            if(int_status & AUTO_CMD_ERR_H){
                /* After confirming the hardware behavior, this check can be removed. */
                if(em_inb(DWC51_AUTO_CMD_STAT_R16) != cmd_info->auto_cmd_stat){
                    WARN(1,"%s: AUTO_CMD_ERR\n",DRIVER_NAME);
                }
            }

            /* This interrupt is used with response check function. We may no need. */
            if(int_status & FX_EVENT ){
                set_bit(FX_EVENT_BIT,&sdport->pending_events);
                WARN(1,"%s: FX_EVENT\n",DRIVER_NAME);
            }

            /* check cmd state */
            if(int_status & CMD_COMPLETE ){
                dwc51_chk_rsp(cmd_info);

                if(test_bit(CMD_ERR_BIT,&cmd_info->cmd_status)){
                    set_bit(CMD_COMPLETE_BIT,&sdport->pending_events);
                    /* cmd stage err alert go to err handler.
                        If there is an error in the cmd stage,
                            it should not be allowed to enter the data stage.
                        alexkh: If an error occurs in the cmd stage,
                            will cmd done still be executed??? */
                }else{
                    set_bit(CMD_COMPLETE_BIT,&sdport->completed_events);
                    /* no cmd stage err */
                    if(NEED_WAIT_XFER(cmd_info->cmd_flag)){
                        /* data cmd or R1B & no cmd stage err.
                            maybe data stage err, leave to data stage to handle
                            no need to check cmd busy, go to check xfer done. */
                        ;
                    }else{
                        /* non-xfer cmd & no cmd stage err. */
                        /* cmd state finish */
                        /* debug only,
                            The non-xfer cmd shuold not have data relative error. */
                        if(int_status & ~(ERR_CMD_REASON_MASK_H | 0x0000ffff) ){
                            mmcirq("The non-data cmd have data relative error!(0x%08x)\n",int_status);
                            WARN(1,"%s: The non-data cmd have data relative error!(0x%08x)\n",
                                DRIVER_NAME,int_status);
                        }

                        if(test_bit(SECOND_CMD_BIT,&cmd_info->cmd_status)){
                            ;   /* have secned cmd need to send */
                            mmcdbg("have second cmd need to send\n");
                        }else{
                            /* no err & non-xfer cmd.*/
                            /* cmd finished */
                            goto request_done_out;
                        }
                    }
                }
            }

            /* check xfer done */
            if(int_status & XFER_COMPLETE ){

                if(test_bit(CMD_ERR_BIT,&cmd_info->cmd_status)){
                    EM_ALERT("cmd stage err, it should not come here.\n");
                    WARN(1,"%s: cmd stage err, it should not come here.\n",DRIVER_NAME);
                }else{
                    /* cmd should complete already */
                    if( test_bit(CMD_COMPLETE_BIT,&sdport->completed_events) ){

                        /* data finished.*/
                        if(dwc51_chk_xfer_done(cmd_info)){
                            /* xfer finished */
                            /* if xfer done, set CMD_COMPLETE_BIT anyway. */
                            set_bit(XFER_COMPLETE_BIT,&sdport->completed_events);
                            if(test_bit(ERR_INTERRUPT_BIT,&sdport->pending_events)){
                                /* err alert & xfer done.*/
                                ;
                            }else{
                                /* no err & xfer finish.*/
                                goto request_done_out;
                            }
                        }else{
                            /* xfer done, but some status not finish.
                                Is this really the case? */
                            set_bit(XFER_COMPLETE_BIT,&sdport->pending_events);
                            mmcirq("xfer done, but some status not finish\n");
                        }
                    }else{
                        EM_ALERT("XFER_COMPLETE but no any CMD_COMPLETE_BIT ???\n"
                                "        int_status=0x%08x(0x%08x) cmd_status=0x%lx\n"
                                "        pending=0x%lx complete=0x%lx\n",
                                int_status,sdport->int_status,cmd_info->cmd_status,
                                sdport->pending_events,sdport->completed_events);
                        WARN(1,"%s: XFER_COMPLETE but no CMD_COMPLETE_BIT pending???\n",DRIVER_NAME);
                    }
                }

            }

            /* This bit is set when both read/write transaction
                is stopped at block gap due to a Stop at Block Gap Request.
                we has no use BLOCK GAP func */
            if(int_status & BGAP_EVENT ){
                mmcirq("BGAP_EVENT !?!?!?\n");
                set_bit(BGAP_EVENT,&sdport->pending_events);
            }

            /* This bit is set if the Buffer Read Enable changes from 0 to 1.
                for PIO mode. */
            if(int_status & BUF_RD_READY ){
                mmcirq("BUF_RD_READY !?!?!?\n");
                set_bit(BUF_RD_READY,&sdport->pending_events);
            }

            /* This bit is set if the Buffer Write Enable changes from 0 to 1.
                for PIO mode */
            if(int_status & BUF_WR_READY ){
                mmcirq("BUF_WR_READY !?!?!?\n");
                set_bit(BUF_WR_READY,&sdport->pending_events);
            }

            /* If there are any pending_events, or second cmd
                schedule the tasklet to handle them. */
            if ((sdport->pending_events) ||
                (test_bit(SECOND_CMD_BIT,&cmd_info->cmd_status))) {
                if (!test_bit(XFER_COMPLETE_BIT,&sdport->pending_events) &&
                    !test_bit(SECOND_CMD_BIT,&cmd_info->cmd_status)) {
                    /* debug check*/
                    EM_ALERT("only XFER_COMPLETE_BIT pending  or second cmd can pass now.\n");
                    EM_ALERT("pending=0x08x cmd_status=0x08x.\n",
                        sdport->pending_events,cmd_info->cmd_status);
                    WARN(1,"%s: only XFER_COMPLETE_BIT pending can pass now.\n",DRIVER_NAME);
                }

                tasklet_schedule(&sdport->polling_tasklet);
            }

        }
        sdport->int_status |= int_status;
    }

    spin_unlock_irqrestore(&sdport->lock, flags);
    goto finish;

request_done_out:
    sdport->int_status |= int_status;
    set_bit(TIMER_STOP_BIT,&cmd_info->cmd_status);
    del_timer(&sdport->timer);
    /* In irq, It not handle error status, just set err=0. */
    dwc51_chk_err_tpye(cmd_info);
    rtksd_show_finish_status(sdport);
    rtksd_clear_event(sdport);
	rtksd_request_done(sdport);
    mrq = sdport->mrq;
    sdport->mrq = NULL;
    spin_unlock_irqrestore(&sdport->lock, flags);
    mmc_request_done(sdport->mmc, mrq);

finish:
    mmcdbg("exit\n");
    if(irq_handled)
        return IRQ_HANDLED;
    else
        return IRQ_NONE;

}

/************************************************************************
 * check read only func
 ************************************************************************/
static
int dw_em_get_ro(struct mmc_host *mmc)
{
#if 0
    struct rtksd_host *sdport = mmc_priv(mmc);
    struct mmc_card *card = mmc->card;
#endif

    return 0;
}

/************************************************************************
 *
 ************************************************************************/
static
int rtksd_switch_user_partition(struct mmc_card *card)
{
#if 1
    /* alexkh: this func needs to modify for bottom half mode. */
    EM_ALERT("this func needs to modify for bottom half mode\n");
    return 0;
#else
    struct mmc_command *cmd = NULL;
    struct sd_cmd_pkt *cmd_info = NULL;
    struct rtksd_host *sdport = NULL;
    int err = 0;
    EM_ALERT("%s(%d)[Alexkh]\n",__func__,__LINE__);
    sdport = mmc_priv(card->host);

    cmd = kmalloc(sizeof(struct mmc_command), GFP_KERNEL);
    if (!cmd) {
        EM_ALERT("No mem can be allocated\n");
        err = -1;
        goto ERR;
    }
    cmd_info = &sdport->cmd_info;
    sdport->cmd = cmd;

    memset(cmd, 0, sizeof(struct mmc_command));
    memset(cmd_info, 0, sizeof(struct sd_cmd_pkt));

    set_cmd_info(sdport,cmd,cmd_info,
             MMC_SWITCH, 0x03b30001, (MMC_CMD_AC | MMC_RSP_R1B));
    err = dwc51_SendCmd(cmd_info);
ERR:
    if(cmd)
        kfree(cmd);
    if(err){
        EM_WARNING( "%s: MMC_SWITCH fail\n", DRIVER_NAME);
    }
    return err;
#endif
}

/************************************************************************
 *
 ************************************************************************/
#ifdef CONFIG_MMC_RTKEMMC_HK_ATTR

#define NORMAL_PART 0
#define BOOT1_PART  1
#define BOOT2_PART  2
#define GP1_PART    3
#define GP2_PART    4
#define GP3_PART    5
#define GP4_PART    6

#if 0
static int rtksd_switch_partition(struct rtksd_host *sdport,u8 acc_part)
{
    struct mmc_command *cmd = NULL;
    struct sd_cmd_pkt *cmd_info = NULL;
    int err;

    cmd = kmalloc(sizeof(struct mmc_command), GFP_KERNEL);
    cmd_info = &sdport->cmd_info;
    sdport->cmd = cmd;

    memset(cmd, 0, sizeof(struct mmc_command));
    memset(cmd_info, 0, sizeof(struct sd_cmd_pkt));

    /* set_cmd_info() */
    cmd->opcode         = MMC_SWITCH;
    cmd->arg            = 0x03b30001 | (acc_part << 8);
    cmd->flags          = (MMC_CMD_AC | MMC_RSP_R1B);
    cmd_info->cmd       = cmd;



    err = dwc51_SendCmd(cmd_info);

    kfree(cmd);
    kfree(cmd_info);

    if(err){
        EM_WARNING( "%s: MMC_SWITCH fail\n", DRIVER_NAME);
    }
    return err;

}
#endif


#endif

/************************************************************************
 *
 ************************************************************************/
//static
//int dw_em_stop_transmission(struct rtksd_host *sdport)
//{
//    struct mmc_command *cmd = NULL;
//    struct sd_cmd_pkt *cmd_info = NULL;
//    int err = 0;
//
//    cmd = kmalloc(sizeof(struct mmc_command), GFP_KERNEL);
//    if (!cmd) {
//        EM_ALERT("No mem can be allocated\n");
//        err = -1;
//        goto ERR;
//    }
//    cmd_info = kmalloc(sizeof(struct sd_cmd_pkt), GFP_KERNEL);
//    if (!cmd_info) {
//        EM_ALERT("No mem can be allocated\n");
//        err = -1;
//        goto ERR;
//    }
//    sdport->cmd = cmd;
//
//    memset(cmd, 0, sizeof(struct mmc_command));
//    memset(cmd_info, 0, sizeof(struct sd_cmd_pkt));
//    /*
//        cmd12 : R1 for read
//                R1b for write
//    */
//
//    set_cmd_info(sdport,cmd,cmd_info,
//             MMC_STOP_TRANSMISSION, 0x00, (MMC_RSP_R1B | MMC_CMD_AC));
//    err = dwc51_SendCmd(cmd_info);
//ERR:
//    if(cmd)
//        kfree(cmd);
//    if(cmd_info)
//        kfree(cmd_info);
//
//    if(err){
//        EM_WARNING( "%s: MMC_STOP_TRANSMISSION fail\n",
//            DRIVER_NAME);
//    }
//    return err;
//
//}

/************************************************************************
 *
 ************************************************************************/
static __attribute__((unused))
void set_cmd_info(struct rtksd_host *sdport,struct mmc_command * cmd,
          struct sd_cmd_pkt *cmd_info, u32 opcode, u32 arg,
          u32 rsp_para)
{
    //memset(cmd, 0, sizeof(struct mmc_command));
    //memset(cmd_info, 0, sizeof(struct sd_cmd_pkt));
    EM_ALERT("%s(%d)[Alexkh]\n",__func__,__LINE__);
    cmd->opcode         = opcode;
    cmd->arg            = arg;
    cmd->flags          = rsp_para;
    cmd_info->cmd       = cmd;


}

/************************************************************************
 * For reference
 *
 * 4Bits Pattern
 * [000]=ff [001]=0f [002]=ff [003]=00 [004]=ff [005]=cc [006]=c3 [007]=cc
 * [008]=c3 [009]=3c [010]=cc [011]=ff [012]=fe [013]=ff [014]=fe [015]=ef
 * [016]=ff [017]=df [018]=ff [019]=dd [020]=ff [021]=fb [022]=ff [023]=fb
 * [024]=bf [025]=ff [026]=7f [027]=ff [028]=77 [029]=f7 [030]=bd [031]=ef
 * [032]=ff [033]=f0 [034]=ff [035]=f0 [036]=0f [037]=fc [038]=cc [039]=3c
 * [040]=cc [041]=33 [042]=cc [043]=cf [044]=ff [045]=ef [046]=ff [047]=ee
 * [048]=ff [049]=fd [050]=ff [051]=fd [052]=df [053]=ff [054]=bf [055]=ff
 * [056]=bb [057]=ff [058]=f7 [059]=ff [060]=f7 [061]=7f [062]=7b [063]=de
 *
 * =======================================================================
 * 8Bits Pattern
 * [000]=ff [001]=ff [002]=00 [003]=ff [004]=ff [005]=ff [006]=00 [007]=00
 * [008]=ff [009]=ff [010]=cc [011]=cc [012]=cc [013]=33 [014]=cc [015]=cc
 * [016]=cc [017]=33 [018]=33 [019]=cc [020]=cc [021]=cc [022]=ff [023]=ff
 * [024]=ff [025]=ee [026]=ff [027]=ff [028]=ff [029]=ee [030]=ee [031]=ff
 * [032]=ff [033]=ff [034]=dd [035]=ff [036]=ff [037]=ff [038]=dd [039]=dd
 * [040]=ff [041]=ff [042]=ff [043]=bb [044]=ff [045]=ff [046]=ff [047]=bb
 * [048]=bb [049]=ff [050]=ff [051]=ff [052]=77 [053]=ff [054]=ff [055]=ff
 * [056]=77 [057]=77 [058]=ff [059]=77 [060]=bb [061]=dd [062]=ee [063]=ff
 * [064]=ff [065]=ff [066]=ff [067]=00 [068]=ff [069]=ff [070]=ff [071]=00
 * [072]=00 [073]=ff [074]=ff [075]=cc [076]=cc [077]=cc [078]=33 [079]=cc
 * [080]=cc [081]=cc [082]=33 [083]=33 [084]=cc [085]=cc [086]=cc [087]=ff
 * [088]=ff [089]=ff [090]=ee [091]=ff [092]=ff [093]=ff [094]=ee [095]=ee
 * [096]=ff [097]=ff [098]=ff [099]=dd [100]=ff [101]=ff [102]=ff [103]=dd
 * [104]=dd [105]=ff [106]=ff [107]=ff [108]=bb [109]=ff [110]=ff [111]=ff
 * [112]=bb [113]=bb [114]=ff [115]=ff [116]=ff [117]=77 [118]=ff [119]=ff
 * [120]=ff [121]=77 [122]=77 [123]=ff [124]=77 [125]=bb [126]=dd [127]=ee
 *
 ************************************************************************/
#define TUNING_BLK_CNT      1
#define TUNING_WRITE_FLAG   BIT(31)
#define TUNING_SKIP_FLAG    BIT(30)
#define TUNING_HS400        BIT(29)
#define TUNING_DIR_UP       BIT(28)
#define TUNING_SKIP_ALL     BIT(27)
#define TUNING_BLK_SHT      6
#define TUNING_CMD_MASK     (0x3fUL)            /* bit0~bit5 */
#define TUNING_BLK_MASK     (0xfUL << TUNING_BLK_SHT)   /* bit6~bit9 */
#define TUNING_TPHASE_SHT   6
#define TUNING_TPHASE_MASK  (0x3fUL << TUNING_TPHASE_SHT)   /* bit6~bit11 */
#define GET_TUNING_CMD(opcode)  (opcode & TUNING_CMD_MASK)
#define GET_TUNING_BLK(opcode)  ((opcode & TUNING_BLK_MASK) >> TUNING_BLK_SHT)
#define GET_TPHASE(opcode)  ((opcode & TUNING_TPHASE_MASK) >> TUNING_TPHASE_SHT)
#define TUNING_BLK_ADR      0x80 /* At write case, this address should be careful to erase data on eMMC */

/* #define CHECK_PATTERN_SELF */
#ifdef CHECK_PATTERN_SELF
/*
keep these struct and function for debug if needed
*/
static const unsigned char pattern_blk[64] = {
    0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc,
    0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
    0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb,
    0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
    0xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c,
    0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
    0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff,
    0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,

};

/************************************************************************
 *
 ************************************************************************/
static int dw_em_chk_pattern(struct rtksd_host *sdport,u8 *data)
{
    int i;
    int err = 0;
    EM_ALERT("%s(%d)[Alexkh]\n",__func__,__LINE__);
    if(sdport->cur_width == 8)
    {
        unsigned char nb1 = 0;
        unsigned char nb2 = 0;

        mmcdbg("%dbits pattern check\n",sdport->cur_width);
        for(i=0; i<64; i++){
            nb1 = (pattern_blk[i] & 0xf0) >> 4;
            nb1 = (nb1<<4)|nb1;
            nb2 = (pattern_blk[i] & 0xf);
            nb2 = (nb2<<4)|nb2;

            if((data[i*2] != nb1) || (data[(i*2)+1] != nb2)){
                mmcdbg("nb1=0x%x nb2=0x%x \n",nb1,nb2);
                mmcdbg("data[%x]=0x%x data[%x]=0x%x \n",
                    i,data[i*2],i+1,data[(i*2)+1]);
                err = -1;
                break;
            }
        }

    } else {
        mmcdbg("%dbits pattern check\n",sdport->cur_width);
        for(i=0; i<64; i++){
            if(data[i] != pattern_blk[i]){
                mmcdbg("data[%x]=0x%x pattern_blk[%x]=0x%x \n",
                    i,data[i],i,pattern_blk[i]);
                err = -1;
                break;
            }
        }
    }

    return err;
}

#endif

/************************************************************************
 *
 ************************************************************************/
static void rtkem_show_tuning_result(struct rtksd_host *sdport, u8* p,char* reason)
{
    EM_INFO("%s\n"
    "MMC phase  {00-31}[%d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d% d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d]\n",
        reason,
        p[ 0],p[ 1],p[ 2],p[ 3],p[ 4],p[ 5],p[ 6],p[ 7],
        p[ 8],p[ 9],p[10],p[11],p[12],p[13],p[14],p[15],
        p[16],p[17],p[18],p[19],p[20],p[21],p[22],p[23],
        p[24],p[25],p[26],p[27],p[28],p[29],p[30],p[31]
        );
}

/************************************************************************
 * check func with mer8
 ************************************************************************/
static
void dw_em_cal_max_phase_range(Phase_Arry p_arry,
                u32* phase_diff,u32* min_p)
{
    int i;
    u32 tmp_range = 0;
    u32 max_range = 0;
    u32 max_end = 0;

	for (i=0; i<MAX_TUNING_STEP; i++) {
		if (p_arry[i]){
			tmp_range++;
		}else{
			tmp_range = 0;
			continue;
		}

		if (tmp_range > max_range) {
			max_range = tmp_range;
			max_end = i;
		}
	}

	*phase_diff = max_range;
	*min_p = (max_end-max_range+1);
}

/************************************************************************
 *
 ************************************************************************/
#define MIN_RANGE_READ  5
#define MIN_RANGE_WRITE 4
static __attribute__((unused))
int dw_em_simple_tuning(struct mmc_host *mmc, u32 ctl)
{
#if 0
    EM_ALERT("%s(%d)skip now \n",__func__,__LINE__);
    return 0;
#else
    struct rtksd_host *sdport;
    int i;
    u32 tmp_phase = 0;
    u32 cur_phase = 0;
    u32 vaild_phase = 0;
    u32 phase_sht = 0;
    u32 phase_mask = 0;
    u32 min_p = 0;
    u32 max_p = 0;
    u32 phase_diff = 0;
    u32 tune_cmd = 0;
    u8 w_flag = 0;
    u8 p[32] = {0};
    int cmd_error = 0;
    int err = 0;
    u32 min_range = 0;

    sdport = mmc_priv(mmc);
    mmcmsg1( "cur bus width %d\n", sdport->cur_width );

    tune_cmd = GET_TUNING_CMD(ctl);

    if(ctl & TUNING_WRITE_FLAG)
        w_flag = 1;

    if(w_flag) {
        if(ctl & TUNING_HS400){
            EM_ALERT("%s(%d)HS400 tuning \n",__func__,__LINE__);
        }else{
            EM_ALERT("%s(%d)HS200 tuning \n",__func__,__LINE__);
        }
        if (tune_cmd == 24){
            phase_sht = PHASE_W_SHT;
            phase_mask = PHASE_W_MASK;
            vaild_phase = VAILD_WPHASE_ZONE;
            min_range = MIN_RANGE_WRITE;
        } else if (tune_cmd == 13){
            phase_sht = PHASE_C_SHT;
            phase_mask = PHASE_C_MASK;
            vaild_phase = VAILD_CPHASE_ZONE;
            /* We think command phase will have larger range */
            min_range = MIN_RANGE_READ;
        }
    } else {
        phase_sht = PHASE_R_SHT;
        phase_mask = PHASE_R_MASK;
        vaild_phase = VAILD_RPHASE_ZONE;
        min_range = MIN_RANGE_READ;
    }

    /* tuining start */
    if (tune_cmd == 13){
        mmcdbg("PHASE_REG(0x%08x)=0x%08x\n",
                EMMC_PLL_PHASE_INTERPOLATION, em_inl(EMMC_PLL_PHASE_INTERPOLATION));
        cur_phase = (em_inl(EMMC_PLL_PHASE_INTERPOLATION)>>phase_sht) & phase_mask;
        mmcdbg("initial cmd phase=%u(0x%x)\n",cur_phase, cur_phase);
    } else {
        mmcdbg("PHASE_REG(0x%08x)=0x%08x\n",
                EMMC_PLLPHASE_CTRL, em_inl(EMMC_PLLPHASE_CTRL));
        cur_phase = (em_inl(EMMC_PLLPHASE_CTRL)>>phase_sht) & phase_mask;
        mmcdbg("initial %s phase=%u(0x%x)\n",(w_flag)?"write":"read",cur_phase, cur_phase);
    }
#define SCAN_LOW_TO_HIGH
#ifdef  SCAN_LOW_TO_HIGH
    for (i=0; i < MAX_TUNING_STEP; i++) {
#else
    i = MAX_TUNING_STEP;
    while(i--){
#endif
        tmp_phase = i;
        mmcdbg( "tmp %s phase is %u(0x%x)\n",(w_flag)?"write":"read",tmp_phase,tmp_phase);

        if(!(ctl & TUNING_SKIP_FLAG)){
            if (tune_cmd == 13)
                em_maskl(EMMC_PLL_PHASE_INTERPOLATION,phase_mask,phase_sht ,tmp_phase);
            else
                em_maskl(EMMC_PLLPHASE_CTRL,phase_mask,phase_sht ,tmp_phase);
        }

        if(w_flag && tune_cmd != 13)
            mdelay(2);

        if(dw_em_chk_phase_vaild(i,vaild_phase)){
            set_bit(PORT_TUNING_STATE,&sdport->port_status);
            err = rtkmmc_send_tuning(mmc, tune_cmd, &cmd_error);
            clear_bit(PORT_TUNING_STATE,&sdport->port_status);
        }else{
            err = -1;
        }

        if(cmd_error)
            mmcdbg("tuning cmd fail!(phase %d)\n",i);

        if (!err){
            p[i] = true;
        }
    }

    if(!(ctl & TUNING_SKIP_FLAG)){
        rtkem_show_tuning_result(sdport,p,"tuning result: ");
    }

    /* find longest good phase */
    dw_em_cal_max_phase_range(p,&phase_diff,&min_p);

    /* We have used valid zones to filter phases, so don't restrict too much */
    if (phase_diff < 3) {
        err = -EIO;
        EM_NOTICE("good range too small,roll back to original phase %d(0x%x)\n",
            cur_phase,cur_phase);

        tmp_phase = cur_phase;
        if (tune_cmd == 13) {
            em_maskl(EMMC_PLL_PHASE_INTERPOLATION,phase_mask,phase_sht ,tmp_phase);
        } else {
            em_maskl(EMMC_PLLPHASE_CTRL,phase_mask,phase_sht ,tmp_phase);
            if (!w_flag) {
                clear_bit(PORT_HS200_TUNED,&sdport->port_status);
            }
        }
    }else{
        err = 0;

        if (ctl & TUNING_SKIP_FLAG) {
            /* restore origional phase */
            tmp_phase = cur_phase;
            if (tune_cmd == 13)
                em_maskl(EMMC_PLL_PHASE_INTERPOLATION,phase_mask,phase_sht ,tmp_phase);
            else
                em_maskl(EMMC_PLLPHASE_CTRL,phase_mask,phase_sht ,tmp_phase);
            EM_NOTICE("skip tuning!!!,keep original phase %d(0x%x)\n",
                cur_phase,cur_phase);
        }else{
            //u32 ds_tune_tmp;

            max_p = min_p + phase_diff - 1;
            if(max_p >= MAX_TUNING_STEP)
                max_p -= MAX_TUNING_STEP;

            tmp_phase = min_p + (phase_diff/2) - 1;

            if(tmp_phase >= MAX_TUNING_STEP)
                tmp_phase -= MAX_TUNING_STEP;

            if (tune_cmd == 13)
                em_maskl(EMMC_PLL_PHASE_INTERPOLATION,phase_mask,phase_sht ,tmp_phase);
            else
                em_maskl(EMMC_PLLPHASE_CTRL,phase_mask,phase_sht ,tmp_phase);
            //ds_tune_tmp = em_inl(EM_HALF_CYCLE_CAL_RESULT) & 0x1f; //fixme

            EM_NOTICE("%s: max_p=%u min_p=%u diff=%u\n",
                DRIVER_NAME,max_p,min_p,phase_diff);
            EM_NOTICE("============================================\n");
            if (tune_cmd == 13){
                EM_NOTICE("%s: final PHASE(%s) is %u; 0x%08x = 0x%08x\n",
                        DRIVER_NAME,"CMD",tmp_phase,
                        EMMC_PLL_PHASE_INTERPOLATION,em_inl(EMMC_PLL_PHASE_INTERPOLATION));
            }
            else {
                EM_NOTICE("%s: final PHASE(%s) is %u; 0x%08x = 0x%08x\n",
                        DRIVER_NAME,(w_flag)?"W":"R",tmp_phase,
                        EMMC_PLLPHASE_CTRL,em_inl(EMMC_PLLPHASE_CTRL));
                //EM_NOTICE("ds_tune=%u(show only)\n",ds_tune_tmp);
            }
            EM_NOTICE("============================================\n");

            if (tune_cmd == 13){
                /* We will use fix phase setting */
            } else if(w_flag){
                if(ctl & TUNING_HS400){
                    sdport->cur_w_ph400 = tmp_phase;
                    mmcdbg("cur_w_ph400 = 0x%08x\n",sdport->cur_w_ph400);
                }else{
                    sdport->cur_w_phase = tmp_phase;
                    mmcdbg("cur_w_phase = 0x%08x\n",sdport->cur_w_phase);
                }
            } else {
                sdport->cur_r_phase = tmp_phase;
                //sdport->pre_ds_tune = ds_tune_tmp;
                set_bit(PORT_HS200_TUNED,&sdport->port_status);
                sdport->tud_r_pha200 = tmp_phase;

                mmcdbg("%s(%d) ==>\n"
                    "        tud_r_pha200 =0x%08x; pre_ds_tune=0x%08x\n",
                    __func__,__LINE__,
                    sdport->tud_r_pha200,sdport->pre_ds_tune);

            }
        }
    }

    return err;
#endif
}

/************************************************************************
 *
 ************************************************************************/
static
void dw_em_set_ds_delay(struct rtksd_host *sdport, int ds_delay)
{
#if 0   //#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)[Alexkh]fix later\n",__func__,__LINE__);
#else
    unsigned int tmp_dstune;
    mmcmsg1("entry\n");
    if(rtk_emmc_test_ctl & EMMC_FIX_DSTUNE){
        EM_ALERT("%s(%d)Use Bootcode pre-setting parameter\n",
            __func__,__LINE__);
        tmp_dstune = sdport->mmc_dstune;
        EM_ALERT("%s(%d)dstune=%d\n",
                __func__,__LINE__,tmp_dstune);
    }else{
        mmcmsg1("ds_delay=%d\n",ds_delay);
        tmp_dstune = ds_delay;
    }
    em_outl(EM51_DS_TUNE_CTRL, tmp_dstune);
    sdport->cur_ds_tune = tmp_dstune;

#endif
}

/************************************************************************
 *
 ************************************************************************/
static __attribute__((unused))
int dw_em_50M_rphase_tuning(struct mmc_host *mmc, u32 ctl)
{
    struct rtksd_host *sdport = mmc_priv(mmc);
    u16 i=0;
    int err=0;
    u32 max_range = 0;
    u32 min_rphase50 = 0;
    u32 rphase50_middle = 0;
    Phase_Arry result = {0};
    int cmd_error = 0;
    u32 tmp_phase = 0;
    u32 ds_tune_tmp = 0;

    mmcdbg("entry \n");
    for (i=0; i < 32; i++) {
        if(dw_em_chk_phase_vaild(i,VAILD_RPHASE_ZONE)){
            em_maskl(EMMC_PLLPHASE_CTRL, PHASE_R_MASK, PHASE_R_SHT, i);
            set_bit(PORT_TUNING_STATE,&sdport->port_status);
            err = rtkmmc_send_tuning(mmc, MMC_READ_SINGLE_BLOCK, &cmd_error);
            clear_bit(PORT_TUNING_STATE,&sdport->port_status);

            if (!err)
                result[i] = 1;
        }
    }

    rtkem_show_tuning_result(sdport,result,"50M rphase tuning result: ");

    dw_em_cal_max_phase_range(result,&max_range,&min_rphase50);

    rphase50_middle = (min_rphase50 + (max_range)/2)-1;

    if (max_range < 3) {
        EM_ERR("rphase50 capable range %d is small\n", max_range);
        tmp_phase = sdport->vender_phase_ptr->HS50_dq_r;
        EM_NOTICE("good range too small,roll back to default phase %d(0x%x)\n",
            tmp_phase,tmp_phase);
        em_maskl(EMMC_PLLPHASE_CTRL, PHASE_R_MASK, PHASE_R_SHT, tmp_phase);
        sdport->cur_r_pha50 = tmp_phase;
        goto ERR;
    } else {
        em_maskl(EMMC_PLLPHASE_CTRL, PHASE_R_MASK, PHASE_R_SHT, rphase50_middle);
        EM_ERR("set rphase50 %d\n", rphase50_middle);
        sdport->cur_r_pha50 = rphase50_middle;
    }

    sdport->tud_r_pha50 = sdport->cur_r_pha50;
    //ds_tune_tmp = em_inl(EM_HALF_CYCLE_CAL_RESULT) & 0x1f;
    sdport->pre_ds_tune = ds_tune_tmp;

    return 0;
ERR:
    return -1;
}

/************************************************************************
 *
 ************************************************************************/
static __attribute__((unused))
int dw_em_ds_tuning(struct mmc_host *mmc, u32 ctl)
{
#if 0   //#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)skip now \n",__func__,__LINE__);
    return 0;
#else
    struct rtksd_host *sdport = mmc_priv(mmc);
    u16 i=0, j=0;
    int err=0;
    u32 max_range = 0;
    u32 min_ds = 0;
    u32 ds_middle = 0;
    u32 old_ds_tune = 0;
    Phase_Arry result = {0};  //always prepare 32 step for MAX_DS_TUNE
    int cmd_error = 0;

    EM_ALERT("%s(%d)[Alexkh]\n",__func__,__LINE__);
    old_ds_tune = sdport->cur_ds_tune;
    if(ctl & TUNING_SKIP_FLAG){
        EM_ALERT("%s(%d)cur_ds_tune:%d\n" ,
            __func__, __LINE__,sdport->cur_ds_tune);
        EM_ALERT("0x%08x=0x%08x 0x%08x=0x%08x\n",
            EMMC_PLLPHASE_CTRL,(u32)em_inl(EMMC_PLLPHASE_CTRL),
            EMMC_PLL_PHASE_INTERPOLATION,(u32)em_inl(EMMC_PLL_PHASE_INTERPOLATION));
        return 0;
    }

    for (i=0; i < MAX_DS_TUNE; i++) {
        if(dw_em_chk_phase_vaild(i,VAILD_DSTUNE_ZONE)){
            dw_em_set_ds_delay(sdport, i);
            set_bit(PORT_TUNING_STATE,&sdport->port_status);
            err = rtkmmc_send_tuning(mmc, MMC_READ_SINGLE_BLOCK, &cmd_error);
            clear_bit(PORT_TUNING_STATE,&sdport->port_status);

            if (!err)
                result[i] = 1;
        }
    }

    rtkem_show_tuning_result(sdport,result,"DS tuning result: ");

    for (i=0; i < MAX_DS_TUNE; i++) {
        int circum = 0;

        for (j=i; j < i+MAX_DS_TUNE; j++) {
            int convert_j =  (j >= MAX_DS_TUNE)?
                (j-MAX_DS_TUNE) : j;
            if (result[convert_j])
                circum++;
            else
                break;
        }

        if (circum > max_range) {
            max_range = circum;
            min_ds = i;
        }
    }

    ds_middle = (min_ds + (max_range)/2)-1;

    if (max_range < 3) {
        EM_ERR("ds capable range %d is small\n", max_range);
        dw_em_set_ds_delay(sdport, old_ds_tune);
        sdport->pre_ds_tune = sdport->cur_ds_tune;
        goto ERR;
    } else {

       dw_em_set_ds_delay(sdport, ds_middle);
       sdport->pre_ds_tune = sdport->cur_ds_tune;
       EM_INFO("set ds %d\n", ds_middle);
    }

    return 0;
ERR:
    return -1;
#endif
}

/************************************************************************
 *
 ************************************************************************/
static int dw_em_execute_tuning(struct mmc_host *mmc, u32 opcode)
{
#if 1
    mmcmsg1("skip tuning now\n");
    return 0;
#else
    struct rtksd_host *sdport;
    int err = 0;
    u32 tune_ctl = 0;
    u32 pre_ctl = 0;

    mmcmsg1("entry\n");
    sdport = mmc_priv(mmc);

    if (test_bit(PORT_SKIP_DSTUNE,&sdport->port_status) ||
        test_bit(PORT_SKIP_PHASETUNE,&sdport->port_status)) {
        /* if STR operation, skip tuning */
        EM_NOTICE("%s(%d) skip tuning flow!!!\n",__func__,__LINE__);
        /* if tuning prior at resume case, skip tuning  */
        pre_ctl |= TUNING_SKIP_ALL;
    } else {
        pre_ctl &= ~TUNING_SKIP_ALL;
    }

    if(rtk_emmc_test_ctl & EMMC_SKIP_TUNE){
        EM_ALERT("%s(%d)Use Bootcode pre-setting parameter. Skip tuning\n",
            __func__,__LINE__);
        pre_ctl |= TUNING_SKIP_FLAG;
    }

    if( (opcode == 21)
     || (opcode == 18) )
    {
        /*
         opcode 21 is HS200 tuning state
         opcode 18 is HS400 tuning state
         */

        /* enable DS-tune has moved to prepare_hs400_tuning() */
        tune_ctl = pre_ctl;

        if(opcode == 18 &&(rtk_emmc_test_ctl & EMMC_FIX_DSTUNE)){
            /* skip ds tuning if fix dstune */
            tune_ctl |= TUNING_SKIP_FLAG;
        }else if((opcode == 21) &&(fix_phase_flag & FIX_RPHASE200)){
            /* skip hs200 tuning if fix rphase200 */
            tune_ctl |= TUNING_SKIP_FLAG;
        }

        if(tune_ctl & TUNING_SKIP_ALL){
            if(opcode == 18){
                /* ds reload"dw_em_set_ds_delay()" at dw_em_prepare_hs400_tuning() & set at dw_em_set_ios()   */
                clear_bit(PORT_SKIP_DSTUNE,&sdport->port_status);
                EM_NOTICE("%s(%d) skip ds tuning flow!!!\n", __func__, __LINE__);
                EM_NOTICE("%s(%d)set ds %d\n", __func__, __LINE__, sdport->cur_ds_tune);
            }else{
                /* w_phase;r_phase */
                /* r_phase set at dw_em_set_PLL_clk() */
                clear_bit(PORT_SKIP_PHASETUNE,&sdport->port_status);
                EM_INFO("%s(%d)restore r_phase=%d\n",
                    __func__,__LINE__,sdport->tud_r_pha200);
            }
            err = 0;
            goto ERR_OUT;
        }

        /* read tuning first at HS200 */
        if(opcode == 18){
            tune_ctl |= 17;
            err = dw_em_ds_tuning(mmc, tune_ctl);
        }else{
            tune_ctl |= 21;
            err = dw_em_simple_tuning(mmc, tune_ctl);
        }

        if(err){
            goto ERR_OUT;
        }

        if(rtk_emmc_test_ctl & EMMC_TUNE_WPH){
             EM_ALERT("%s(%d)Write phase tuning start\n",__func__,__LINE__);
            /* write tuning  */
            tune_ctl = pre_ctl;
            tune_ctl |= TUNING_WRITE_FLAG;

            if(opcode == 18){
                tune_ctl |= TUNING_HS400;
            }
            tune_ctl &= ~(TUNING_CMD_MASK);
            tune_ctl |= 24; // tuning cmd
            err = dw_em_simple_tuning(mmc, tune_ctl);

            if(err){
                goto ERR_OUT;
            }
        }

        if(rtk_emmc_test_ctl & EMMC_TUNE_WPH){
             EM_ALERT("%s(%d)Command phase tuning start\n",__func__,__LINE__);
            /* actually, do commad tuning  */
            tune_ctl = pre_ctl;
            tune_ctl |= TUNING_WRITE_FLAG;

            if(opcode == 18){
                tune_ctl |= TUNING_HS400;
            }
            tune_ctl &= ~(TUNING_CMD_MASK);
            tune_ctl |= 13; // tuning cmd, send status
            err = dw_em_simple_tuning(mmc, tune_ctl);

            if(err){
                goto ERR_OUT;
            }
        }

    }else{
        err = -1;
    }

    /* force downspeed */
    if (downspeed_test){
        /* Downspeed HS200 -> HS50 */
        /* Make HS400 tuning fail, Downspeed to HS200 */
        if (opcode == 18)
            return (-1);
        /* Make HS200 tuning fail, Downspeed to HS50 */
        if (opcode == 21){
            err = (-1);
        }
    }

    if(!(rtk_emmc_test_ctl & EMMC_DIS_50M_TUNE)){
        if(opcode == MMC_RPHASE50_TUNING)
            err = dw_em_50M_rphase_tuning(mmc, tune_ctl);
    }
ERR_OUT:
    if (!test_bit(PORT_HS200_TUNED,&sdport->port_status)) {
        if (HS200_fail_count > 3){
            if (sdport->mmc->caps2 & MMC_CAP2_HS200_1_8V_SDR){
                EM_ALERT("%s: Disable HS200 mode\n",mmc_hostname(sdport->mmc));
                sdport->mmc->caps2 &= (~MMC_CAP2_HS200_1_8V_SDR);
                sdport->mmc->card->mmc_avail_type &= ~EXT_CSD_CARD_TYPE_HS200;
            }
        }
        else {
            HS200_fail_count++;
            err = 0;
        }
    } else {
        HS200_fail_count = 0;
    }
    return err;
#endif
}

/************************************************************************
 *
 ************************************************************************/
static int mmc_send_data_cmd(struct mmc_host *mmc, u32 hc_cmd,
                 u32 cmd_arg, u32 blk_cnt, unsigned char *buffer)
{
    int err = 0;
    struct rtksd_host *sdport;

    if(mmc == NULL)
        mmc = mmc_host_local;

    sdport = mmc_priv(mmc);

    if(!test_bit(PORT_TUNING_STATE,&sdport->port_status)){
        EM_NOTICE( "cmd=%u, arg=0x%08x, size=0x%08x, buf=0x%p\n",
            hc_cmd,cmd_arg,blk_cnt,buffer);
    }

    if(mmc){
        struct mmc_card *card = mmc->card;
        struct mmc_request mrq = {0};
        struct mmc_command cmd = {0};
        struct mmc_data data   = {0};
        struct scatterlist sg;

        sdport = mmc_priv(mmc);
        mmc_claim_host(mmc);

        cmd.opcode = hc_cmd;
        cmd.arg = cmd_arg;
        cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;

        if (blk_cnt) {
            data.sg = &sg;
            data.sg_len = 1;

            if(hc_cmd == 21){
                if(sdport->cur_width == 8){
                    /* 8bits 128bytes */
                    data.blksz = 128;
                }else if(sdport->cur_width ==4){
                    /* 4bits 64bytes */
                    data.blksz = 64;
                }else{
                    mmcdbg( "not supported parameter");
                    //spin_unlock_irqrestore(&sdport->lock, flags);
                    mmc_release_host(mmc);
                    goto ERR_OUT;
                }

            }else{
                data.blksz = 512;
            }
            data.blocks = blk_cnt;

            sg_init_one(data.sg, buffer, (blk_cnt * (data.blksz)));

            if( hc_cmd == MMC_WRITE_MULTIPLE_BLOCK ||
                hc_cmd == MMC_WRITE_BLOCK) {
                data.flags = MMC_DATA_WRITE;
            } else if (hc_cmd == MMC_SEND_EXT_CSD ||
                hc_cmd == MMC_READ_MULTIPLE_BLOCK ||
                hc_cmd == MMC_READ_SINGLE_BLOCK ||
                hc_cmd == MMC_SEND_TUNING_BLOCK_HS200) {
                data.flags = MMC_DATA_READ;
            }else{
                EM_ERR( "not supported command");
                //spin_unlock_irqrestore(&sdport->lock, flags);
                mmc_release_host(mmc);
                goto ERR_OUT;
            }

            if(card){
                mmc_set_data_timeout(&data, card);
            }
        }

        data.mrq = &mrq;
        cmd.mrq = &mrq;
        cmd.data = &data;

        if(test_bit(PORT_TUNING_STATE,&sdport->port_status))
            cmd.retries = 0;
        else
            cmd.retries = 3;

CMD_RETRY:
        mrq.data = &data;
        mrq.cmd = &cmd;

        if(sdport->mrq){
            sdport->mrq = NULL;
        }

        cmd.error = 0;
        data.error = 0;

        mmc_wait_for_req(mmc, &mrq);

        err = cmd.error;

        if(err && cmd.retries ){
            mmcdbg( "%s(%u)last retry %d counter.\n",
                __func__,__LINE__,cmd.retries);
            cmd.retries--;
            goto CMD_RETRY;
        }
        mmc_release_host(mmc);

    }else{
        err = -ENODEV;
    }

ERR_OUT:
    if(err)
        mmcdbg( "err=%d\n", err);
    return err;
}

/************************************************************************
 *
 ************************************************************************/
__attribute__((unused)) int export_mmc_send_data_cmd(u32 hc_cmd,
                 u32 cmd_arg, u32 blk_cnt, unsigned char *buffer)
{
    EM_ALERT("%s(%d)[Alexkh]\n",__func__,__LINE__);
    return mmc_send_data_cmd(NULL,hc_cmd,cmd_arg,blk_cnt,buffer);
}
EXPORT_SYMBOL(export_mmc_send_data_cmd);

#ifdef CONFIG_MMC_RTKEMMC_HK_ATTR
/************************************************************************
 *
 ************************************************************************/
static __attribute__((unused))
void show_ext_csd(u8 *ext_csd)
{
    int i;

    for (i = 0; i < 512; i += 8) {
        EM_NOTICE(
               "[%03u]=%02x [%03u]=%02x [%03u]=%02x [%03u]=%02x "
               "[%03u]=%02x [%03u]=%02x [%03u]=%02x [%03u]=%02x\n",
               i, *(ext_csd + i), i + 1, *(ext_csd + i + 1), i + 2,
               *(ext_csd + i + 2), i + 3, *(ext_csd + i + 3), i + 4,
               *(ext_csd + i + 4), i + 5, *(ext_csd + i + 5), i + 6,
               *(ext_csd + i + 6), i + 7, *(ext_csd + i + 7));
    }
    EM_NOTICE( "\n");
}

/************************************************************************
 *
 ************************************************************************/
#ifdef  EN_SHOW_DATA
static
void show_data(u8 *buf, unsigned int len)
{
    int i;

    for (i = 0; i < len; i += 8) {
        EM_NOTICE(
               "[%03u]=%02x [%03u]=%02x [%03u]=%02x [%03u]=%02x "
               "[%03u]=%02x [%03u]=%02x [%03u]=%02x [%03u]=%02x\n",
               i, *(buf + i), i + 1, *(buf + i + 1), i + 2,
               *(buf + i + 2), i + 3, *(buf + i + 3), i + 4,
               *(buf + i + 4), i + 5, *(buf + i + 5), i + 6,
               *(buf + i + 6), i + 7, *(buf + i + 7));
    }
    EM_NOTICE( "\n");
}
#endif

/************************************************************************
 *
 ************************************************************************/
__attribute__((unused))
static void hk_rw_reg(struct device *dev,
        size_t p_count, unsigned char *cr_param)
{
#if 1
    EM_NOTICE("%s:mark for fix rtkcr_chk_param()\n",
                DRIVER_NAME,rtk_emmc_log_state);
    return;
#else
    u32 param1;
    u32 param2;
    u32 param3;

    EM_NOTICE( "%s(%u)2015/10/19 16:25\n", __func__, __LINE__);

    rtkcr_chk_param(&param1,8,cr_param);
    rtkcr_chk_param(&param2,8,cr_param+9);
    rtkcr_chk_param(&param3,8,cr_param+18);

    if(p_count == 34){
        if(param1 == 0x2379beef){
            /* write reg */
            EM_NOTICE("REG[0x%08x] = 0x%08x(before)\n",param2,cr_readl(param2));
            em_outl(param2,param3);
            EM_NOTICE("REG[0x%08x] = 0x%08x\n",param2,cr_readl(param2));
        }else if(param3 == 0x2379beef){
            /* read reg */
            EM_NOTICE("REG[0x%08x] = 0x%08x\n",param2,cr_readl(param2));
        }
    }else{
#if 0       /* Just for remind, do not to display user guide */
        EM_NOTICE( "Command format:\n"
               "    echo rw_reg=param1,param2,param3 > emmc_hacking;\n"
               "        param1: if 0xffffffff is write; if 0x00000000 is read.\n"
               "        param2: register address.\n"
               "        param3: if write case, it's writed value, or nothing.\n"
               "  ex:\n"
               "   write case:"
               "    echo rw_reg=2379beef,b8010900,12345678 > emmc_hacking;\n"
               "   read case:"
               "    echo rw_reg=00000000,b8010900,2379beef > emmc_hacking;\n" );
#endif
    }

    return;
#endif
}

#endif

/* ========================================================
 * blk_addr    : eMMC read/write target address, block base.
 * data_size   : tarnsfer data size, block base.
 * buffer      : DMA address
 * rw_mode     : fast read or fast write
 ========================================================== */
#define FAST_READ   0x1278
#define FAST_WRITE  0x3478
extern int export_get_card_status(struct mmc_card *card, u32 *status, int retries);
static int mmc_fast_rw( u32 blk_addr,
               u32 data_size, unsigned char *buffer, u32 rw_mode)
{
    int err = 0;
    struct mmc_host * mmc = mmc_host_local;

    if(mmc){
        struct rtksd_host *sdport;
        struct mmc_card *card;
        struct mmc_request mrq = {0};
        struct mmc_command cmd = {0};
        struct mmc_data data   = {0};
        struct scatterlist sg;

        sdport = mmc_priv(mmc);
        mmc_claim_host(mmc);
        card = mmc->card;

        if (!test_bit(PORT_USER_PARTITION,&sdport->port_status)) {
            int try_loop = 3;
            do{
                err = 0;
                /* Switch to ser partition */
                err = mmc_switch(card, EXT_CSD_CMD_SET_NORMAL,
                        EXT_CSD_PART_CONFIG, 0,
                        card->ext_csd.part_time);
            }while(try_loop-- && err );
            if(err){
                mmc_release_host(mmc);
                goto ERR_OUT;
            } else {
                set_bit(PORT_USER_PARTITION,&sdport->port_status);
            }
        }

        if(data_size>1){

            if(rw_mode == FAST_WRITE)
                cmd.opcode = MMC_WRITE_MULTIPLE_BLOCK;
            else
                cmd.opcode = MMC_READ_MULTIPLE_BLOCK;

        }else{

            if(rw_mode == FAST_WRITE)
                cmd.opcode = MMC_WRITE_BLOCK;
            else
                cmd.opcode = MMC_READ_SINGLE_BLOCK;
        }

        cmd.arg = blk_addr;
        cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;

        if (data_size) {    /* date info setting */
            data.sg = &sg;
            data.sg_len = 1;
            data.blksz = 512;
            data.blocks = data_size;

            sg_init_one(data.sg, buffer, (data_size<<9));

            if(rw_mode == FAST_WRITE)
                data.flags = MMC_DATA_WRITE;
            else
                data.flags = MMC_DATA_READ;

            mmc_set_data_timeout(&data, card);
        }

        data.mrq = &mrq;
        cmd.mrq = &mrq;
        cmd.data = &data;
        cmd.retries =5;

CMD_RETRY:
        mrq.data = &data;
        mrq.cmd = &cmd;

        if(sdport->mrq){
            sdport->mrq = NULL;
        }
        cmd.error = 0;
        data.error = 0;
        mmc_wait_for_req(mmc, &mrq);
        err = cmd.error;
        if(err && cmd.retries){
            EM_INFO("%s(%u)last retry %d counter.\n",
                    __func__,__LINE__,cmd.retries);
            cmd.retries--;
            goto CMD_RETRY;
        }
        mmc_release_host(mmc);
    }else{
        err = -ENODEV;
    }

ERR_OUT:
    if(err)
        EM_INFO("err=%d\n", err);
    return err;
}

/************************************************************************
 *
 ************************************************************************/
#define MAX_XFER_BLK      0x400
static int mmc_fast_rw_loop(u32 blk_addr,
                u32 data_size, unsigned char *buffer, u32 rw_mode)
{
    int err = 0;

    u32 tmp_addr   = blk_addr;
    u32 tmp_size   = data_size;
    u32 org_size   = data_size;
    unsigned char * tmp_buf = buffer;
    u32 max_xfer_blk;

    do{
        /* max 1M bytes read/write per transfer */

        max_xfer_blk = MAX_XFER_BLK;

        if(data_size > max_xfer_blk){
            tmp_size = max_xfer_blk;
        }else{
            tmp_size = data_size;
        }

        err = mmc_fast_rw(tmp_addr,tmp_size,tmp_buf,rw_mode);

        if(err)
            break;

        if(data_size > max_xfer_blk){
            tmp_addr    += max_xfer_blk;
            data_size   -= max_xfer_blk;
            tmp_buf     += (max_xfer_blk<<9);
        }else{
            data_size = 0;
        }

    }while(data_size);

    if(err == 0)
         return org_size-data_size;
    else
        return err;
}

/************************************************************************
 *
 ************************************************************************/
int mmc_fast_read(u32 blk_addr, u32 data_size, unsigned char *buffer)
{
    int err = 0;

    err = mmc_fast_rw_loop(blk_addr,data_size,buffer,FAST_READ);
    return err;
}

EXPORT_SYMBOL(mmc_fast_read);

/************************************************************************
 *
 ************************************************************************/
int mmc_fast_write(u32 blk_addr, u32 data_size, unsigned char *buffer)
{
    int err = 0;

    if(blk_addr<0x20000){
        EM_INFO("target small then save area.\n");
        err = -1;
        return err;
    }
    err = mmc_fast_rw_loop(blk_addr,data_size,buffer,FAST_WRITE);
    return err;
}

EXPORT_SYMBOL(mmc_fast_write);

/************************************************************************
 *
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
void test_raw_emmc_partition_change(u32 mode)
{
#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)[Alexkh]fix later\n",__func__,__LINE__);
#else

    EM_ALERT( "%s(%d)\n",__func__,__LINE__);
    if(export_raw_emmc_partition_change(mode)){
        EM_ALERT( "%s(%d)change partition%u fail\n",
            __func__,__LINE__,mode);
    }
#endif
}
#endif

/************************************************************************
 *
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
void test_raw_emmc_read(u64 blk_addr, u32 blk_cnt)
{
#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)[Alexkh]fix later\n",__func__,__LINE__);
#else

    u8 *emmc_buf = NULL;
    int err;

    EM_ALERT( "%s(%d)\n",__func__,__LINE__);

    emmc_buf = kmalloc(blk_cnt*512, GFP_KERNEL);
    if(!emmc_buf){
        EM_ALERT( "%s(%d)malloc buffer fail!!!\n",__func__,__LINE__);
    }else{
        err = export_raw_dw_em_read(emmc_buf, (u64)(blk_addr*512),(blk_cnt*512));

        if(err){
            EM_ALERT( "%s(%d) fail!!!\n",__func__,__LINE__);
        }
        kfree(emmc_buf);
    }
#endif
}
#endif
/************************************************************************
 *
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
void test_raw_emmc_write(u64 blk_addr, u32 blk_cnt)
{
#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)[Alexkh]fix later\n",__func__,__LINE__);
#else
    u8 *emmc_buf = NULL;
    int err;

    EM_ALERT( "%s(%d)\n",__func__,__LINE__);

    emmc_buf = kmalloc(blk_cnt*512, GFP_KERNEL);
    if(!emmc_buf){
        EM_ALERT( "%s(%d)malloc buffer fail!!!\n",__func__,__LINE__);
    }else{
        err = export_raw_dw_em_write(emmc_buf, (u64)(blk_addr*512),(blk_cnt*512));

        if(err){
            EM_ALERT( "%s(%d) fail!!!\n",__func__,__LINE__);
        }
        kfree(emmc_buf);
    }
#endif
}
#endif

/************************************************************************
 *
 ************************************************************************/
#define MMC_PATTERN_1 0x00ff00ff
#define MMC_PATTERN_2 0x55aa55aa
#define MMC_PATTERN_3 0xff00ff00
#define MMC_PATTERN_4 0xaa55aa55

#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
__attribute__((unused))
static void mmc_fill_pattern(char *mmc_buf,int blk_cnt)
{
    int i;
    uint *ptr;

    /*
     change to 4 bytes, it's convenient to arrange pattern.
     Four types pattern to test.
     */
     ptr = (uint*)mmc_buf;
RE_FILL:
    blk_cnt -= 1;

    for(i=0;i< 128;i++){
        if(i<32)
            ptr[i] = MMC_PATTERN_1;
        else if(i<64)
            ptr[i] = MMC_PATTERN_2;
        else if(i<96)
            ptr[i] = MMC_PATTERN_3;
        else if(i<128)
            ptr[i] = MMC_PATTERN_4;
    }
    if(blk_cnt){
        ptr += 128;
        goto RE_FILL;
    }

}
#endif

/************************************************************************
 *
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
__attribute__((unused))
static int mmc_chk_pattern(char *mmc_buf,int blk_cnt)
{
    int i;
    int ret;
    uint *ptr;

    /*
     change to 4 bytes, it's convenient to arrange pattern.
     Four types pattern to test.
     */
    ret = 0;
    ptr = (uint*)mmc_buf;

NEXT_BLK_CHK:
    blk_cnt -= 1;
    for(i=0;i< 128;i++){
        if(i<32){
            if(ptr[i] != MMC_PATTERN_1){
                ret = -1;
                break;
            }
        }
        else if(i<64){
            if(ptr[i] != MMC_PATTERN_2){
                ret = -1;
                break;
            }
        }
        else if(i<96){
            if(ptr[i] != MMC_PATTERN_3){
                ret = -1;
                break;
            }
        }
        else if(i<128){
            if(ptr[i] != MMC_PATTERN_4){
                ret = -1;
                break;
            }
        }
    }

    if(blk_cnt && !ret){
        ptr += 128;
        goto NEXT_BLK_CHK;
    }

    return ret;
}
#endif

/************************************************************************
 *
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
__attribute__((unused))
static void mmc_show_blk(char *mmc_buf,int blk_cnt)
{
#ifdef DEBUG_PATTERN_EN

    int i;
    uint *ptr;

    ptr = (uint*)mmc_buf;

NEXT_BLK:
    blk_cnt -= 1;
    for(i=0;i<16;i++){
        EM_INFO("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
               ptr[(i*8)+0],ptr[(i*8)+1],ptr[(i*8)+2],ptr[(i*8)+3],
               ptr[(i*8)+4],ptr[(i*8)+5],ptr[(i*8)+6],ptr[(i*8)+7]);
    }

    if(blk_cnt){
        ptr += 128;
        goto NEXT_BLK;
    }
#endif
}
#endif

/************************************************************************
 *
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
void test_raw_emmc_compare(u64 blk_addr, u32 blk_cnt)
{
#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)[Alexkh]fix later\n",__func__,__LINE__);
#else
    u8 *emmc_buf = NULL;
    int err;

    EM_ALERT( "%s(%d)\n",__func__,__LINE__);

    emmc_buf = kmalloc(blk_cnt*512, GFP_KERNEL);
    if(!emmc_buf){
        EM_ALERT( "%s(%d)malloc buffer fail!!!\n",__func__,__LINE__);
    }else{
        memset(emmc_buf, 0, blk_cnt*0x200);
        mmc_fill_pattern(emmc_buf,blk_cnt);
        mmc_show_blk(emmc_buf,blk_cnt);
        err = export_raw_dw_em_write(emmc_buf, (u64)(blk_addr*512),(blk_cnt*512));

        if(err){
            EM_ALERT( "%s(%d) fail!!!\n",__func__,__LINE__);
            goto ERR_OUT;
        }

        memset(emmc_buf, 0, blk_cnt*0x200);
        mmc_show_blk(emmc_buf,blk_cnt);
        err = export_raw_dw_em_read(emmc_buf, (u64)(blk_addr*512),(blk_cnt*512));
        if(err){
            EM_ALERT( "%s(%d) fail!!!\n",__func__,__LINE__);
            goto ERR_OUT;
        }

        mmc_show_blk(emmc_buf,blk_cnt);
        err = mmc_chk_pattern(emmc_buf,blk_cnt);
        if (err){
            EM_ALERT( "%s(%d) compare fail\n",__func__,__LINE__);
        }

ERR_OUT:
        kfree(emmc_buf);
    }
#endif
}
#endif

/************************************************************************
 * Command Sample
 * //No use
 * echo raw_func=00000000,b800084c,00001000 > /sys/devices/platform/18010800.emmc/emmc_hacking
 * //change to user partition
 * echo raw_func=00000001,00000000,00001000 > /sys/devices/platform/18010800.emmc/emmc_hacking
 * //change to boot part 1
 * echo raw_func=00000001,00000001,00001000 > /sys/devices/platform/18010800.emmc/emmc_hacking
 * //change to boot part 2
 * echo raw_func=00000001,00000002,00001000 > /sys/devices/platform/18010800.emmc/emmc_hacking
 * //read from 0x1234 total 0x100blk
 * echo raw_func=00000002,00001234,00000100 > /sys/devices/platform/18010800.emmc/emmc_hacking
 * //write to 0x740000 total 0x100blk
 * echo raw_func=00000003,00740000,00000100 > /sys/devices/platform/18010800.emmc/emmc_hacking
 * //emmc init
 * echo raw_func=00000004,b800084c,00001000 > /sys/devices/platform/18010800.emmc/emmc_hacking
 * //emmc W/R compare use raw func. write to 0x740000 total 0x10blk then read back and compare.
 * echo raw_func=00000005,00740000,00000010 > /sys/devices/platform/18010800.emmc/emmc_hacking
 *
 ************************************************************************/
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
__attribute__((unused))
static void hk_raw_func(struct device *dev, size_t p_count,
            unsigned char *cr_param)
{
#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)[Alexkh]fix later\n",__func__,__LINE__);
#else
    u32 param1;     /* 1:change partition; 2:raw_read; 3:raw_write */
    u32 param2;     /* partition number or start blk address for read/write */
    u32 param3;     /* block size of read/write  */

    EM_ALERT( "%s(%u)2016/05/04 16:05\n", __func__, __LINE__);

    rtkcr_chk_param(&param1,8,cr_param);
    rtkcr_chk_param(&param2,8,cr_param+9);
    rtkcr_chk_param(&param3,8,cr_param+18);

    EM_NOTICE( "param1=0x%x\n", param1);
    EM_NOTICE( "param2=0x%x\n", param2);
    EM_NOTICE( "param3=0x%x\n", param3);

    switch(param1){
        case 1: //raw_partition
            test_raw_emmc_partition_change(param2);
            break;
        case 2: //raw_read
            test_raw_emmc_read((u64)param2, param3);
            break;
        case 3: //raw_write
            test_raw_emmc_write((u64)param2, param3);
            break;
        case 4: //raw_init
            export_raw_dw_em_init();
            break;
        case 5: //W/R compare
            test_raw_emmc_compare((u64)param2, param3);
            break;
        default:
            EM_ALERT( "No support\n");
            return;
    }
    return;
#endif
}
#endif

/************************************************************************
 * mmc device attribute
 ************************************************************************/

/************************************************************************
 *
 ************************************************************************/
#define TEST_BLK_SIZE 10
static ssize_t
cr_fast_RW_dev_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    struct mmc_host * host = dev_get_drvdata(dev);

    EM_INFO("%s(%u)\n", __func__, __LINE__);

    if(host && host->card){
        rtksd_switch_user_partition(host->card);
    }
    return sprintf(buf, "send SWITCH command\n");
}

static ssize_t
cr_fast_RW_dev_store(struct device *dev,
                     struct device_attribute *attr,
             const char *buf, size_t count)
{
#if 0   //fix rtkcr_parse_token later
    unsigned char *cr_param = NULL;

    EM_INFO("%s(%u)\n", __func__, __LINE__);
    EM_INFO("%s\n", buf);
    EM_INFO("count=%d\n", (int)count);

    cr_param=(char *)rtkcr_parse_token(buf,"cr_param");

    if(cr_param){
        u8 *emmc_buf;
        u32 param1; /* target emmc address */
        u32 param2; /* block number */
        u32 param3; /* 1: write; 0: read */

        rtkcr_chk_param(&param1,8,cr_param);
        rtkcr_chk_param(&param2,8,cr_param+9);
        rtkcr_chk_param(&param3,8,cr_param+18);
        EM_INFO("param1=0x%x param2=0x%x param3=0x%x\n",
                param1,param2,param3);

        emmc_buf = kmalloc(512*param2, GFP_KERNEL);
        if(!emmc_buf){
            EM_INFO("emmc_buf is NULL\n");
            goto ERR_OUT;
        }

        EM_INFO("emmc_buf=0x%p\n", emmc_buf);

        if(param3 == 1){
            mmc_fast_write(param1, param2, emmc_buf );
        }else{
            mmc_fast_read(param1, param2, emmc_buf );
        }
        kfree(emmc_buf);
    }else{
        EM_INFO("have no parameter searched.\n");
    }

ERR_OUT:
    /*
    return value must be equare or big then "count"
    to finish this attribute
    */
    if(cr_param)
        kfree(cr_param);
#endif
    return count;
}

DEVICE_ATTR(cr_fast_RW, S_IRUGO|S_IWUSR|S_IWGRP,
            cr_fast_RW_dev_show,cr_fast_RW_dev_store);

/************************************************************************
 *
 ************************************************************************/
static ssize_t
em_open_log_dev_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    return sprintf(buf,
                "key in \"echo 1 > device_node\em_open_log\" to enable log\n");
}

static ssize_t
em_open_log_dev_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    unsigned long tmp_long;
    struct mmc_host * host = dev_get_drvdata(dev);
    struct rtksd_host *sdport = mmc_priv(host);

    if (kstrtoul(buf, 10, &tmp_long)){
        return -EINVAL;
    }else{
        if(tmp_long == 0){
            clear_bit(PORT_FOPEN_LOG,&sdport->port_status);
        }else if(tmp_long == 1){
            set_bit(PORT_FOPEN_LOG,&sdport->port_status);
        }else{
            return -EINVAL;
        }
    }

    return count;
}

DEVICE_ATTR(em_open_log, S_IRUGO|S_IWUSR|S_IWGRP,
              em_open_log_dev_show,em_open_log_dev_store);

/************************************************************************
 *
 ************************************************************************/
#ifdef CONFIG_MMC_RTKEMMC_HK_ATTR
static ssize_t
emmc_hacking_dev_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{
    return sprintf(buf,
            "Supported hacking below:\n"
            "    set_wr_rel  : enable write reliability.\n"
            "    red_ext_csd : show ext_csd.\n"
            "    set_ehuser  : enable enhance user date area.\n");
}

static ssize_t
emmc_hacking_dev_store(struct device *dev,
                     struct device_attribute *attr,
               const char *buf, size_t count)
{
#if 0   //fix rtkcr_parse_token later
    unsigned char *cr_param = NULL;

    EM_INFO( "%s(%u)2013/08/15 17:30\n", __func__, __LINE__);
    EM_INFO( "count=%d\n", (int)count);

    cr_param=(char *)rtkcr_parse_token(buf,"rw_reg");
    if(cr_param){
        hk_rw_reg(dev,count,cr_param);
        kfree(cr_param);
        goto FINISH_OUT;
    }
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
    cr_param=(char *)rtkcr_parse_token(buf,"raw_func");
    if(cr_param){
        hk_raw_func(dev,count,cr_param);
        kfree(cr_param);
        goto FINISH_OUT;
    }
#endif
    EM_INFO("have no match command!!\n");

FINISH_OUT:
    /*
    return value must be equare or big then "count"
    to finish this attribute
    */
#endif
    return count;
}

DEVICE_ATTR(emmc_hacking, S_IRUGO|S_IWUSR|S_IWGRP,
            emmc_hacking_dev_show,emmc_hacking_dev_store);

#endif

/************************************************************************
 *
 ************************************************************************/
static ssize_t
emmc_errcnt_dev_show(struct device *dev, struct device_attribute *attr,
        char *buf)
{

    return sprintf(buf, "%s: eMMC have had %lu error since boot\n",
        DRIVER_NAME, rtk_emmc_err_cnt);
}

static ssize_t
emmc_errcnt_dev_store(struct device *dev,
                     struct device_attribute *attr,
              const char *buf, size_t count)
{
    unsigned long tmp_long;

    if (kstrtoul(buf, 10, &tmp_long)){
        return -EINVAL;
    }else{
        if(tmp_long == 0){
            rtk_emmc_err_cnt = 0;
            EM_INFO("%s(%u)reset error count to %u.\n",
                __func__, __LINE__,(unsigned int)rtk_emmc_err_cnt);
        }else{
            EM_INFO("%s(%u)No thing to do.\n",
                __func__, __LINE__);
        }
    }

    return count;
}

DEVICE_ATTR(emmc_errcnt, S_IRUGO|S_IWUSR|S_IWGRP,
            emmc_errcnt_dev_show, emmc_errcnt_dev_store);

/************************************************************************
 *
 ************************************************************************/
static ssize_t
em_chk_mode_dev_show(struct device *dev, struct device_attribute *attr,
            char *buf)
{
    struct mmc_host * host = dev_get_drvdata(dev);
    struct mmc_card *card = NULL;
    int type;
    struct rtksd_host *sdport = NULL;
    int  n = 0;
    int  count = PAGE_SIZE;
    char *ptr = buf;

    static const char *const card_type_tbl[5] = {
        "MMC ",
        "SD ",
        "SDIO ",
        "SD_COMBO ",
        "Unknow "
    };

    rtkcr_display_version();
    dw_em_show_phase();
    EM_ALERT("0x%08x=0x%08x 0x%08x=0x%08x 0x%08x=0x%08x\n",
            RTK_IOPAD_CFG1,em_inl(RTK_IOPAD_CFG1),
            RTK_IOPAD_CFG2,em_inl(RTK_IOPAD_CFG2),
            RTK_IOPAD_CFG3,em_inl(RTK_IOPAD_CFG3));
    EM_ALERT("0x%08x=0x%08x 0x%08x=0x%08x 0x%08x=0x%08x 0x%08x=0x%08x\n",
            RTK_IOPAD_SET1,em_inl(RTK_IOPAD_SET1),
            RTK_IOPAD_SET2,em_inl(RTK_IOPAD_SET2),
            RTK_IOPAD_SET3,em_inl(RTK_IOPAD_SET3),
            RTK_IOPAD_SET4,em_inl(RTK_IOPAD_SET4));

    if(host && host->card){
        sdport = mmc_priv(host);
        card = host->card;
        type = card->type;
        if(type >3)
            type = 4;
        n = snprintf(ptr, count,"%s: new %s%s%s%s card(%dHz)[%dbit] at address %04x\n",
            mmc_hostname(host),
            mmc_card_uhs(host->card) ? "ultra high speed " :
            (mmc_card_hs(host->card) ? "high speed " : ""),
            mmc_card_hs400(host->card) ? "HS400 " :
            (mmc_card_hs200(host->card) ? "HS200 " : ""),
            mmc_card_ddr52(host->card) ? "DDR " : "",
            card_type_tbl[type],
            sdport->cur_clock,
            sdport->cur_width,
            host->card->rca);
        ptr+=n; count-=n;

        if (sdport->int_mode){
            n = snprintf(ptr, count,"ISR mode enabled\n");
            ptr+=n; count-=n;
        }
        if (test_bit(PORT_FTIMER_CHECK,&sdport->port_status)) {
            n = snprintf(ptr, count,"Max Timeout Report: cmd%d total_byte:0x%08x max_timeout = %lldus\n",
                cmd_record, total_byte_record,atomic64_read(&cmd_time_ns_old)/1000);
            ptr+=n; count-=n;
        }
    }

    return ptr - buf;;
}

static ssize_t
em_chk_mode_dev_store(struct device *dev,
            struct device_attribute *attr,
            const char *buf, size_t count)
{
    EM_ALERT("%s(%u)\n", __func__, __LINE__);
    EM_ALERT("%s\n", buf);
    EM_ALERT("count=%d\n", (int)count);

    /*
    return value must be equare or big then "count"
    to finish this attribute
    */
    return count;
}

DEVICE_ATTR(em_chk_mode, S_IRUGO|S_IWUSR|S_IWGRP,
        em_chk_mode_dev_show,em_chk_mode_dev_store);


/************************************************************************
 *
 ************************************************************************/
static ssize_t
em_predefined_show(struct device *dev, struct device_attribute *attr,
            char *buf)
{
    return sprintf(buf, "show Predefined or OpenEnd. fixme late\n");
}

/************************************************************************
 *
 ************************************************************************/
static ssize_t
em_predefined_dev_store(struct device *dev,
            struct device_attribute *attr,
            const char *buf, size_t count)
{
    struct mmc_host * host = dev_get_drvdata(dev);
    struct rtksd_host *sdport = NULL;

    /* These setting is useless now. */
    sdport = mmc_priv(host);
    if (buf[0] == '1') {
        /* pre-define */
        sdport->access_mode = MOD_PREDEF;
    } else {
        /* open-end */
        sdport->access_mode = MOD_OPENEND;
    }

    return count;
}

DEVICE_ATTR(em_predefined_read, S_IRUGO|S_IWUSR|S_IWGRP,
              em_predefined_show,em_predefined_dev_store);


/************************************************************************
 * Bootcode command sample:
 *
 * env set mmc_param_1 pl200=00000001,53,0d,ff,07,0052,011e,0bba3 pl190=00000000,50,0d,ff,07,0000,0000,00000
 * env set mmc_param_2 pl180=00000000,4d,0d,ff,07,0000,0000,00000 pl170=00000000,48,0d,ff,03,0000,0000,00000
 * env set mmc_param_3 pl110=00000000,2d,23,ff,03,0000,0000,00000 pl100=00000000,29,25,ff,03,0000,0000,00000
 * env set mmc_param_4 pl050=00000000,13,0c,ff,01,0000,0000,00000
 * env set mmc_param_5 dstune=09
 * env set mmc_param_6 mmc_drv_io=73e9e9e9,fbfbfbfb,fbfbfbfb,80000000
 * env set mmc_param_7 mmc_drv=00040100
 *
 * env set bootargs emmc_bus=003C0001 emmc_clock=160000000 androidboot.console=ttyS0 console=ttyS0,115200
 *
 * Note : max mmc_param_NUM NUM=12
 ************************************************************************/
static
int rtkemmc_parse_pad_drv(struct rtksd_host * rtk_priv, char *drv_str)
{
    int err = -1;
    char *tmp;
    char *tmp_drv_str = drv_str;
    long driving = 0;
    int i=0;

    if (!rtk_priv)
        return err;

    rtk_priv->mmc_io_drv = kmalloc(sizeof(u32)*5, GFP_KERNEL); // 5 param at most
    if (rtk_priv->mmc_io_drv == NULL) {
        EM_ALERT("No mem can be allocated\n");
        return err;
    }

    for (i=0; i<5; i++) {
        tmp = strsep(&tmp_drv_str, ",");
        if (tmp == NULL || tmp[0] == '\0')
            continue;
        if (!kstrtol(tmp, 16, (long*)&driving))
            rtk_priv->mmc_io_drv[i] = (u32)driving;
    }

    EM_NOTICE("\n");
    for (i=0; i<5; i++)
        EM_NOTICE("mmc_io_drv[%d] = 0x%08x, ", i, rtk_priv->mmc_io_drv[i]);
    EM_NOTICE("\n");

    return 0;
}

/************************************************************************
 *
 ************************************************************************/
static int rtkemmc_parse_mmc_param(struct rtksd_host *sdport)
{
    char *opt = NULL;
    char *platform_mmc_param = NULL;

    long dstune = 0;
    long tphase = 0;
    //long card_drv = 0;
    long burst_blk = 0;
    char *str_tmp;

    { /* for debug, enable cmd flow print out. */
        //set_bit(PORT_FOPEN_LOG,&sdport->port_status);
    }

    if (strlen(platform_info.mmc_param) == 0) {
        EM_INFO("=== NO MMC PARAMETER in bootloader ===\n");
        return 0;
    }

    if (!sdport) {
        EM_ERR("%s rtkemmc initial error\n", __FUNCTION__);
        return -1;
    }

    /* log func stste */
    if(rtk_emmc_log_state){
        sdport->log_state = rtk_emmc_log_state;
        EM_ALERT("log_state is %u\n",sdport->log_state);
    }

    EM_NOTICE("=== mmc_param is \"%s\" ===\n", platform_info.mmc_param);

    platform_mmc_param = kmalloc(MMC_PARAM_LEN,GFP_KERNEL);
    if(platform_mmc_param == NULL)
        return 0;

    memcpy(platform_mmc_param,platform_info.mmc_param,MMC_PARAM_LEN);

    while ((opt= strsep(&platform_mmc_param, " ")) != NULL) {
        if (!*opt)
            continue;

        //if (!strncmp(opt, "emmc_debug_log", 14)) {
        //    rtk_emmc_debug_log = 1;
        //}
        else if (!strncmp(opt, "emmc_open_log", 13)) {
            set_bit(PORT_FOPEN_LOG,&sdport->port_status);
        }
        else if (!strncmp(opt, "timeout_chk", 11)) {
            set_bit(PORT_FTIMER_CHECK,&sdport->port_status);
        }
        else  if (!strncmp(opt, "emmc_crc", 8)) {
            rtk_emmc_test_ctl |= EMMC_SHOW_CRC;
        }
        else if (!strncmp(opt, "emmc_width=", 11)) {
            long bus_wid = 0;
            if (!kstrtol(opt + 11, 10, (long*)&bus_wid))
            {
                switch (bus_wid)
                {
                case 1:
                    sdport->mmc->caps &= ~MMC_CAP_8_BIT_DATA;
                    sdport->mmc->caps &= ~MMC_CAP_4_BIT_DATA;
                    break;
                case 4:
                    sdport->mmc->caps &= ~MMC_CAP_8_BIT_DATA;
                    sdport->mmc->caps |= MMC_CAP_4_BIT_DATA;
                    break;
                default:
                    sdport->mmc->caps |= MMC_CAP_8_BIT_DATA;
                    sdport->mmc->caps |= MMC_CAP_4_BIT_DATA;
                }
            }
        }
        else if (!strncmp(opt, "emmc_clock=", 11)) {
            long bus_clock = 0;
            if (!kstrtol(opt + 11, 10, (long*)&bus_clock)){

                EM_NOTICE("emmc_clock %ld\n", bus_clock);
                // check clock > 200MHz?
                if (bus_clock > 200000000)
                    bus_clock = 200000000;

                sdport->mmc->f_max = bus_clock;
            }
        }
        else if (!strncmp(opt, "dstune=", 7)) {
            if (kstrtol(opt + 7, 10, (long*)&dstune)){
                dstune = 0;
            } else {
                sdport->mmc_dstune = dstune;
                EM_NOTICE("dstune %ld\n", dstune);
                rtk_emmc_test_ctl |= EMMC_FIX_DSTUNE;
            }
        }
        else if (!strncmp(opt, "dsmode=", 7)) {
            EM_NOTICE("dsmode support no more on 2819A\n");

        }
        else if (!strncmp(opt, "pad_drv=", 8)) {
            str_tmp = strsep(&opt, "=");
            EM_NOTICE("\ndebug {str_tmp %s} {opt %s} \n", str_tmp, opt);
            if(!rtkemmc_parse_pad_drv(sdport, opt))
                rtk_emmc_test_ctl |= EMMC_EN_IO_DRV;
        }
        else if (!strncmp(opt, "burst_blk=", 10)) {
            if (kstrtol(opt + 10, 10, (long*)&burst_blk))
                burst_blk = 0;
            else {
                EM_NOTICE("burst_blk %ld\n", burst_blk);
                sdport->mmc->max_blk_count = burst_blk;
            }
        }
        else if (!strncmp(opt, "emmc_dis_sscg", 13)) {
            rtk_emmc_test_ctl |= EMMC_DIS_SSCG;
        }
        else if (!strncmp(opt, "emmc_always_clk", 15)) {
            rtk_emmc_test_ctl |= EMMC_ALWAYS_CLK;
        }
        else if (!strncmp(opt, "en_tune_wphase", 10)) {
            if(rtk_emmc_test_ctl & EMMC_FIX_WPHASE)
                EM_NOTICE("fix write phase enable; skip phase tuning\n");
            else
                rtk_emmc_test_ctl |= EMMC_TUNE_WPH;
        }
        else if (!strncmp(opt, "wphase400=", 10)) {
            tphase = 0;
            if (!kstrtol(opt + 10, 10, (long*)&tphase)){
                rtk_emmc_test_ctl |= EMMC_FIX_WPHASE;
                rtk_emmc_test_ctl &= ~EMMC_TUNE_WPH;
                fix_phase_flag |= FIX_WPHASE400;
                SET_FIX_WPHA400(tphase,rtk_fix_wphase);
                EM_NOTICE("get fix HS400 write phase %ld.(0x%08x)\n",
                        tphase,rtk_fix_wphase);
            }
        }
        else if (!strncmp(opt, "wphase200=", 10)) {
            tphase = 0;
            if (!kstrtol(opt + 10, 10, (long*)&tphase)){
                rtk_emmc_test_ctl |= EMMC_FIX_WPHASE;
                rtk_emmc_test_ctl &= ~EMMC_TUNE_WPH;
                fix_phase_flag |= FIX_WPHASE200;
                SET_FIX_WPHA200(tphase,rtk_fix_wphase);
                EM_NOTICE("get fix HS200 write phase %ld.(0x%08x)\n",
                        tphase,rtk_fix_wphase);
            }
        }
        else if (!strncmp(opt, "rphase400=", 10)) {
            tphase = 0;
            if (!kstrtol(opt + 10, 10, (long*)&tphase)){
                fix_phase_flag |= FIX_RPHASE400;
                SET_FIX_RPHA400(tphase);
                EM_NOTICE("get fix HS400 cmd read phase %ld.(0x%08x/0x%08x)\n",
                        tphase,rtk_fix_rphase,GET_FIX_RPHA400());
            }
        }
        else if (!strncmp(opt, "rphase200=", 10)) {
            tphase = 0;
            if (!kstrtol(opt + 10, 10, (long*)&tphase)){
                fix_phase_flag |= FIX_RPHASE200;
                SET_FIX_RPHA200(tphase);
                EM_NOTICE("get fix HS200 read phase %ld.(0x%08x/0x%08x)\n",
                        tphase,rtk_fix_rphase,GET_FIX_RPHA200());

            }
        }
        else if (!strncmp(opt, "cphase200=", 10)) {
            tphase = 0;
            if (!kstrtol(opt + 10, 10, (long*)&tphase)){
                rtk_emmc_test_ctl |= EMMC_FIX_WPHASE;
                rtk_emmc_test_ctl &= ~EMMC_TUNE_WPH;
                fix_phase_flag |= FIX_CPHASE200;
                SET_FIX_CPHA200(tphase,rtk_fix_wphase);
                EM_NOTICE("get fix HS200 cmd phase %ld.(0x%08x)\n",
                        tphase,rtk_fix_wphase);
            }
        }
        else if (!strncmp(opt, "wphase50=", 9)) {
            tphase = 0;
            if (!kstrtol(opt + 9, 10, (long*)&tphase)){
                fix_phase_flag |= FIX_WPHASE50;
                SET_FIX_WPHA50(tphase,rtk_fix_50M_phase);
                EM_NOTICE("get fix HS50 wphase phase %ld.(0x%08x)\n",
                        tphase,rtk_fix_wphase);
            }
        }
        else if (!strncmp(opt, "rphase50=", 9)) {
            tphase = 0;
            if (!kstrtol(opt + 9, 10, (long*)&tphase)){
                fix_phase_flag |= FIX_RPHASE50;
                SET_FIX_RPHA50(tphase,rtk_fix_50M_phase);
                EM_NOTICE("get fix HS50 read phase %ld.(0x%08x)\n",
                        tphase,rtk_fix_wphase);
            }
        }
        else if (!strncmp(opt, "dis_50m_tune", 12)) {
            rtk_emmc_test_ctl |= EMMC_DIS_50M_TUNE;
        }
        else if (!strncmp(opt, "en_phase_tool", 13)) {
            rtk_emmc_test_ctl |= EMMC_EN_SCAN_TOOL;
        }
        else if (!strncmp(opt, "emmc_en_hs50", 13)){
            sdport->mmc->caps2 &= ~(MMC_CAP2_HS200_1_8V_SDR
                    | MMC_CAP2_HS400_1_8V);
            EM_ALERT("bootcode select HS50 mode\n");
        }
        else if (!strncmp(opt, "emmc_en_hs200", 13)){
            sdport->mmc->caps2 &= ~(MMC_CAP2_HS200_1_8V_SDR
                    | MMC_CAP2_HS400_1_8V);
            sdport->mmc->caps2 |= MMC_CAP2_HS200_1_8V_SDR;
            EM_ALERT("bootcode select HS200 mode\n");
        }
        else if (!strncmp(opt, "emmc_en_hs400", 13)) {
            sdport->mmc->caps2 &= ~(MMC_CAP2_HS200_1_8V_SDR
                    | MMC_CAP2_HS400_1_8V);
            sdport->mmc->caps2 |= (MMC_CAP2_HS200_1_8V_SDR
                        | MMC_CAP2_HS400_1_8V);
            EM_ALERT("bootcode select HS400 mode\n");
            //HS400_fix_test = true;
        }
        else if (!strncmp(opt, "emmc_en_cmdq", 12)) {
            sdport->mmc->caps2 |= (MMC_CAP2_CQE | MMC_CAP2_CQE_DCMD);
            EM_ALERT("bootcode enable cmd queue func\n");
        }
        else if (!strncmp(opt, "emmc_skip_tune", 14)) {
            rtk_emmc_test_ctl |= EMMC_SKIP_TUNE;
        }
        else if (!strncmp(opt, "emmc_chk_buf", 12)) {
            EM_NOTICE("%s(%d)\n",__func__,__LINE__);
            rtk_emmc_test_ctl |= EMMC_CHK_BUF;
        }
        else if (!strncmp(opt, "drv_strength=", sizeof("drv_strength=")-1)) {
            long dstrength = 0;
            if (!kstrtol(opt + sizeof("drv_strength=")-1, 10, (long*)&dstrength)){
                rtk_emmc_test_ctl |= EMMC_FIX_STRENGTH;
                SET_FIX_STRENGTH(dstrength,rtk_fix_wphase);
                EM_NOTICE("get drv_strength=%ld.\n",
                        dstrength);
            }
        }
        else if (!strncmp(opt, "emmclog=", 8)) {
            /*
            For customer request, valid value from 0~15(0xf).
            1. without emmclog or emmclog=0, disable message function.
            2. emmclog=1, enable message for this request.
            3. emmclog=other_value , for future request.
             */
            long temp = 0;
            if (kstrtol(opt + 8, 10, (long*)&temp))
                rtk_emmc_log_state = 0;
            else {
                sdport->log_state = rtk_emmc_log_state = (u32)temp;
                EM_ERR("log_state is %u\n",sdport->log_state);
            }
        }
    }

    if(platform_mmc_param)
        kfree(platform_mmc_param);

    EM_NOTICE("\n");
    return 0;

}


/************************************************************************
 *
 ************************************************************************/
static
int rtkemmc_disable_int(struct rtksd_host * sdport)
{
    /* disable SCPU int */
    if(sdport->int_mode){
        mmcdbg("disable SCPU int\n");
        em_outl(EMMC_SCPU_INT,EMMC_SINT_MASK);
        dwc51_int_act(INT_ACT_DISABLE, 0, 0);
        dwc51_int_act(INT_GET_CLR, 0, 0);
    }else{
        mmcdbg("non-int mode\n");
    }

    return 0;
}

/************************************************************************
 *
 ************************************************************************/
static
int rtkemmc_enable_int(struct rtksd_host * sdport)
{
    if(sdport->int_mode){
        mmcdbg("int_mode\n");
        dwc51_int_act(INT_GET_CLR, 0, 0);
        dwc51_int_act(INT_ACT_ENABLE, MMC_INT_EVENT, MODE_INTERRUPT);
        /* enable SCPU int */
        em_outl(EMMC_SCPU_INT,EMMC_SINT_MASK|BIT(0));
    }else{
        mmcdbg("polling mode\n");
        dwc51_int_act(INT_ACT_ENABLE, MMC_INT_EVENT, MODE_POLLING);
    }

    return 0;
}

/************************************************************************
 * alexkh: Are there any others need to clear?
 * These parameter is for rtksd_host life time, but may be changed.
 ************************************************************************/
static
int rtkem_clr_sta(struct rtksd_host *sdport)
{
    sdport->cur_clock   = 0;
    sdport->cur_PLL     = 0;
    sdport->cur_width   = 0;
    sdport->bus_width   = 0xff;
    sdport->cur_div     = 0xff;
    sdport->cur_timing  = 0xff;
    sdport->cur_driving = 0xff;
    sdport->cur_w_phase = 0x55;
    sdport->cur_w_ph400 = 0x55;
    sdport->cur_r_phase = 0x55;
    sdport->cur_ds_tune = 0x55;

    return 0;
}

#define EN_RTK_EMMC_CQE
#ifdef EN_RTK_EMMC_CQE
/************************************************************************
 *
 ************************************************************************/
static
void rtk_cq_dumpregs(struct mmc_host *mmc)
{
    struct rtksd_host *sdport = mmc_priv(mmc);
    struct cqhci_host *cq_host = sdport->cq_host;
    int i;

    mmcmsg3("cq_host=0x%x\n",cq_host);

    dwc51_chk_reg(__func__,__LINE__);

    for(i=0;i<20;i++){
        EM_ERR("0x%08x=0x%08x (0x%08x)\n",
            DWC51_CQ_BASE+(i*4),
            em_inl(DWC51_CQ_BASE+(i*4)),
            cqhci_readl(cq_host,(i*4)));
    }

}

/************************************************************************
 *
 ************************************************************************/
static
void rtk_cq_update_dcmd_desc(struct mmc_host *mmc,
					 struct mmc_request *mrq, u64 *data)
{
#if 0
    struct rtksd_host *sdport = mmc_priv(mmc);
    struct cqhci_host *cq_host = sdport->cq_host;


    mmcmsg3("cq_host=0x%x\n",cq_host);
#endif
}

/************************************************************************
 *
 ************************************************************************/
u32 rtk_cq_readl(struct cqhci_host *host, int reg)
{
    u32 val;
    u32 cqreg = DWC51_CQ_BASE+reg ;

    val = em_inl(cqreg);
    return val;
}

/************************************************************************
 *
 ************************************************************************/
static
void rtk_cq_writel(struct cqhci_host *cq_host, u32 val, int reg)
{
    u32 cqreg = DWC51_CQ_BASE+reg ;
    em_outl(cqreg,val);
}

/************************************************************************
 *
 ************************************************************************/
static
void rtk_cq_pre_enable(struct mmc_host *mmc)
{
#if 0
    struct rtksd_host *sdport = mmc_priv(mmc);
    //struct cqhci_host *cq_host = sdport->cq_host;

    sdport->cqe_on = true;
    em_outl(EM51_SCPU_SEL,SCPU_SEL);

    mmcmsg3("cq_host=0x%x\n",cq_host);
#endif
}

/************************************************************************
 *
 ************************************************************************/
static void rtk_cq_enable(struct mmc_host *mmc)
{

    struct rtksd_host *sdport = mmc_priv(mmc);
    //struct cqhci_host *cq_host = sdport->cq_host;
    unsigned int reginfo;

    /* all cqe reg should not need to set by local driver. */
    sdport->cqe_on = true;

    mmcmsg3("The capability of DWC CMDQ config=0x%08x (0x%08x)\n",
        em_inl(DWC51_CQCAP),cqhci_readl(cq_host,CQHCI_CAP));   //0x000030C8

    em_outl(EM51_SCPU_SEL,SCPU_SEL);

    //disable cmdq first
    em_outl(DWC51_CQCFG, em_inl(DWC51_CQCFG) & ~CQ_EN ); //0x18010A88

    //1. clear data path SW_RST_R.SW_RST_DAT = 1
    em_outb(DWC51_SW_RST_R8,SW_RST_DAT|SW_RST_CMD);    //0x1801092F
    while(em_inb(DWC51_SW_RST_R8) & (SW_RST_DAT|SW_RST_CMD)){
        mmcmsg3("wait for SW_RST_DAT\n");
    }
    //mmcmsg3("DWC51_SW_RST_R8=0x%02x\n",em_inb(DWC51_SW_RST_R8));

    //2. Set Task Descriptor List Base Address Register (CQTDLBA)
//    dwc51_cmdq_set_scrip_base((unsigned int)cmdq_desp_ptr);

    //3. Set value corresponding to executed data byte length of one block to BLOCKSIZE_R
    em_outw(DWC51_BLOCKSIZE_R16,0x200); //0x18010904
    //mmcmsg3("DWC51_BLOCKSIZE_R16=0x%08x\n",em_inw(DWC51_BLOCKSIZE_R16));

    //4. Set the XFER_MODE_R
    mmcmsg3("DWC51_XFER_MODE_R16=0x%08x\n",em_inw(DWC51_XFER_MODE_R16));
    em_outw(DWC51_XFER_MODE_R16,(MULTI_BLK_SEL|BLOCK_COUNT_ENABLE|DMA_ENABLE)); //0x1801090C
    mmcmsg3("DWC51_XFER_MODE_R16=0x%08x\n",em_inw(DWC51_XFER_MODE_R16));

    //   Set DMA_SEL to ADMA2 only mode in the HOST_CTRL1_R
    reginfo = em_inb(DWC51_HOST_CTRL1_R8);
    mmcmsg3("DWC51_HOST_CTRL1_R8=0x%08x\n",reginfo);
    reginfo &= ~DMA_SEL_MASK;
    reginfo |= DMA_SEL(DMA_SEL_ADMA2_32);
    em_outb(DWC51_HOST_CTRL1_R8,reginfo); //0x18010928
    mmcmsg3("DWC51_HOST_CTRL1_R8=0x%08x\n",reginfo);

    //   Set BLOCK_CNT to 0
    //    ==> Set SDMASA_R (while using 32 bits)
    //    ==> Set BLOCKCOUNT_R to 0 (while using 16 bits)
    mmcmsg3("DWC51_BLOCKCOUNT_R16=0x%08x\n",em_inw(DWC51_BLOCKCOUNT_R16));
    em_outw(DWC51_BLOCKCOUNT_R16,0);    //0x18010906
    mmcmsg3("DWC51_BLOCKCOUNT_R16=0x%08x\n",em_inw(DWC51_BLOCKCOUNT_R16));

    //5. Set CQIC register to enable interrupt coalescing and
    //          to configure its timeout and threshold parameters INTC_TOUT, INTC_TH
    //alexkh: Do not enable function "Queuing Interrupt Coalescing" in this stage.
//    em_outl(DWC51_CQIC,0);  //0x18010A9C


    //6. Set CQIS, CQISE, CQISGE, NORMAL_INT_STAT_R, NORMAL_INT_EN_R,
    //          and NORMAL_INT_SIGNAL_EN_R registers to enable interrupts Disable generation of command complete
    //          and transfer complete interrupts using NORMAL_INT_STAT_EN_R
//    dwc51_int_act(INT_ENABLE,CMDQ_INT_EVENT,MODE_INTERRUPT);
//    dwc51_cmdq_int_act(INT_ENABLE,CQIS_ALL_EVENT,MODE_INTERRUPT);

    //7. Set CQSSC1 register to configure device queue status fetch polling time and block count.
    //   Set CQSSC2 to configure RCA
    //CQSSC1: default [19:16]:1; [15:0]: 0x1000
    em_outl(DWC51_CQSSC1,0x11000);    //0x18010AC0
    //mmcmsg3("DWC51_CQSSC1=0x%08x\n",em_inl(DWC51_CQSSC1));
//    em_outl(DWC51_CQSSC2,(card->rca ));    //0x18010AC4
    //mmcmsg3("DWC51_CQSSC2=0x%0808x\n",em_inl(DWC51_CQSSC2));

    //8. Set CQCFG register to configure DCMD_EN, TASK_DESC_SIZE and CQ_EN
    //DCMD_EN: SLOT31 for dircet cmd always
    //enable cmdq
    em_outl(DWC51_CQCFG, em_inl(DWC51_CQCFG) | CQ_EN ); //0x18010A88
    //mmcmsg3("DWC51_CQCFG=0x%08x\n",em_inl(DWC51_CQCFG));

}

/************************************************************************
 *
 ************************************************************************/
static void rtk_cq_disable(struct mmc_host *mmc, bool recovery)
{
#if 1
    struct rtksd_host *sdport = mmc_priv(mmc);
    //struct cqhci_host *cq_host = sdport->cq_host;
    sdport->cqe_on = false;
    mmcmsg3("cqe_on=%s\n",sdport->cqe_on?"true":"false");
#endif

#if 0
	struct msdc_host *host = mmc_priv(mmc);
	unsigned int val = 0;

	/* disable cmdq irq */
	sdr_clr_bits(host->base + MSDC_INTEN, MSDC_INT_CMDQ);
	/* disable busy check */
	sdr_clr_bits(host->base + MSDC_PATCH_BIT1, MSDC_PB1_BUSY_CHECK_SEL);

	val = readl(host->base + MSDC_INT);
	writel(val, host->base + MSDC_INT);

	if (recovery) {
		sdr_set_field(host->base + MSDC_DMA_CTRL,
			      MSDC_DMA_CTRL_STOP, 1);
		if (WARN_ON(readl_poll_timeout(host->base + MSDC_DMA_CTRL, val,
			!(val & MSDC_DMA_CTRL_STOP), 1, 3000)))
			return;
		if (WARN_ON(readl_poll_timeout(host->base + MSDC_DMA_CFG, val,
			!(val & MSDC_DMA_CFG_STS), 1, 3000)))
			return;
		msdc_reset_hw(host);
	}
#endif
}


/************************************************************************
 *
 ************************************************************************/
static
void rtk_cq_post_disable(struct mmc_host *mmc) {
    struct rtksd_host *sdport = mmc_priv(mmc);
    struct cqhci_host *cq_host = sdport->cq_host;
    unsigned reginfo;

    sdport->cqe_on = false;
    em_outl(EM51_SCPU_SEL,0);
    mmcmsg3("cq_host=0x%x\n",cq_host);

	reginfo = cqhci_readl(cq_host, CQHCI_CFG);
	reginfo &= ~CQHCI_ENABLE;
	cqhci_writel(cq_host, reginfo, CQHCI_CFG);

    em_outw(DWC51_XFER_MODE_R16,0); //0x1801090C
    //1. clear data path SW_RST_R.SW_RST_DAT = 1
    em_outb(DWC51_SW_RST_R8,SW_RST_DAT|SW_RST_CMD);    //0x1801092F
    while(em_inb(DWC51_SW_RST_R8) & (SW_RST_DAT|SW_RST_CMD)){
        mmcmsg3("wait for SW_RST_DAT\n");
    }

}

/************************************************************************
 * ref: DesignWare Cores Mobile Storage Host Controller
 *      A.8.3 Command Queueing Task Descriptor
 *      A.8.4 eMMC Data Structures
 * ref: JESD84-B51.pdf
 *      6.6.39 Command Queuing
 ************************************************************************/
static const struct cqhci_host_ops rtk_cmdq_ops = {
    .dumpregs           = rtk_cq_dumpregs,
    .read_l             = rtk_cq_readl,
    .write_l            = rtk_cq_writel,
	.enable             = rtk_cq_enable,
	.disable            = rtk_cq_disable,
	.pre_enable         = rtk_cq_pre_enable,
	.post_disable       = rtk_cq_post_disable,
    .update_dcmd_desc   = rtk_cq_update_dcmd_desc,
};

#endif  //??#ifdef CMDQ_ENABLE

/************************************************************************
 *
 ************************************************************************/
static
int __init rtkemmc_probe(struct platform_device *pdev)
{
    struct mmc_host *mmc = NULL;
    struct rtksd_host *sdport = NULL;
    struct resource *r;
    int ret;
    int irq;
    int att_err;

    atomic64_set( &cmd_time_ns_old, 0 );
    memset(&tuning_phase,0,sizeof(mmc_phase_t));

    att_err = device_create_file(&pdev->dev, &dev_attr_cr_fast_RW);
    att_err = device_create_file(&pdev->dev, &dev_attr_em_open_log);
    att_err = device_create_file(&pdev->dev, &dev_attr_em_chk_mode);
    att_err = device_create_file(&pdev->dev, &dev_attr_emmc_errcnt);
#ifdef CONFIG_MMC_RTKEMMC_HK_ATTR
    att_err = device_create_file(&pdev->dev, &dev_attr_emmc_hacking);
#endif
    att_err = device_create_file(&pdev->dev, &dev_attr_em_predefined_read);


    r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    irq = platform_get_irq(pdev, 0);
    EM_INFO("%s: IRQ %u\n", DRIVER_NAME, irq);

    if (!r || irq < 0)
        return -ENXIO;

    mmc = mmc_alloc_host(sizeof(struct rtksd_host), &pdev->dev);

    if (!mmc) {
        ret = -ENOMEM;
        goto out;
    }

    mmc_host_local = mmc;
    sdport = mmc_priv(mmc);
    memset(sdport, 0, sizeof(struct rtksd_host));

    sdport->mmc = mmc;
    sdport->dev = &pdev->dev;
    sdport->res = r;

    sdport->vender_phase_num = 0;
    sdport->vender_phase_ptr = &vendor_phase_tbl[sdport->vender_phase_num];

    sdport->int_mode = 1;

    sdport->base = devm_ioremap_resource(&pdev->dev, sdport->res);
    if (IS_ERR(sdport->base)) {
        ret = PTR_ERR(sdport->base);
        goto out;
    }

    rtkem_clr_sta(sdport);

    /* Need to check : DMA 64 or 32 bits*/
    /* Right now device-tree probed devices don't get dma_mask set. */
    if (!pdev->dev.dma_mask)
        pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask;
    if (!pdev->dev.coherent_dma_mask)
        pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);

    mmc->ocr_avail = MMC_VDD_30_31 | MMC_VDD_31_32   //0x40ff8080
        | MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;

    mmc->caps = MMC_CAP_NONREMOVABLE
        | MMC_CAP_4_BIT_DATA
        | MMC_CAP_8_BIT_DATA
        | MMC_CAP_MMC_HIGHSPEED
        | MMC_CAP_CMD23
        | MMC_CAP_WAIT_WHILE_BUSY
        /* | MMC_CAP_HW_RESET */
            ;

    mmc->caps2 = MMC_CAP2_NO_SDIO
                | MMC_CAP2_NO_SD
                | MMC_CAP2_FULL_PWR_CYCLE   //needed for mmc_poweroff_notify()

#ifdef EN_EMMC_HS200
        | MMC_CAP2_HS200_1_8V_SDR
#endif

#ifdef EN_EMMC_HS400
        | MMC_CAP2_HS400_1_8V
        /* | MMC_CAP2_HS400_ES */ //test later
#endif

#ifdef EN_RTK_EMMC_CQE
        //| (MMC_CAP2_CQE | MMC_CAP2_CQE_DCMD) //enable by parameter
#endif
            ;
// #if defined(CONFIG_RTKEMMC_HS200_MODULE) || defined(CONFIG_RTKEMMC_HS400_MODULE) \
//        ||  defined(CONFIG_RTKEMMC_HS200) || defined(CONFIG_RTKEMMC_HS400)
//         /* use config setting */
//         mmc->f_max = (CONFIG_RTKEMMC_HS200_CLOCK*1000000);
// #else
        mmc->f_max = CLK_200Mhz;
        //mmc->f_max = CLK_50Mhz;    /* RTK max bus clk is 50Mhz */
//#endif
    /* check mmc parameter from bootcode  */
    ret = rtkemmc_parse_mmc_param(sdport);
    if (ret) {
        EM_ERR("parse mmc parameter fail\n");
    }
    /* follow lowest clock 100KHz according spec, but we set to 200KHz*/
    mmc->f_min = CLK_200Khz;

    mmc->max_blk_size = 4096;
    mmc->max_segs = EMMC_MAX_SCRIPT_LINE;
    mmc->max_seg_size = EMMC_MAX_SCRIPT_BYTE;
    mmc->max_req_size = mmc->max_seg_size * mmc->max_segs;
    mmc->max_blk_count = mmc->max_req_size / 512;

    spin_lock_init(&sdport->lock);

    tasklet_init(&sdport->polling_tasklet, rtkem_polling_tasklet,
                (unsigned long)sdport);


#ifdef EN_RTK_EMMC_CQE
    //EM_INFO("enable EN_RTK_EMMC_CQE\n");
    if (mmc->caps2 & MMC_CAP2_CQE) {
        EM_INFO("caps2 MMC_CAP2_CQE alert\n");
		sdport->cq_host = devm_kzalloc(mmc->parent,
					     sizeof(*sdport->cq_host),
					     GFP_KERNEL);
		if (!sdport->cq_host) {
			ret = -ENOMEM;
            mmcmsg3("get cq_host fail\n");
			goto out;
		}
		//sdport->cq_host->caps |= CQHCI_TASK_DESC_SZ_128;
		sdport->cq_host->mmio = sdport->base + 0x180;
		sdport->cq_host->ops = &rtk_cmdq_ops;
		ret = cqhci_init(sdport->cq_host, mmc, false);
		if (ret) {
            mmcmsg3("cqhci_init fail\n");
			goto out;
        }
		mmc->max_segs = 128;
		/* cqhci 16bit length */
		/* 0 size, means 65536 so we don't have to -1 here */
		mmc->max_seg_size = 64 * 1024;
	}
#endif

    if(sdport->int_mode){
        ret = request_irq(irq, dw_em_irq, IRQF_SHARED, DRIVER_NAME, sdport);
        if (ret) {
            EM_ERR( "%s: cannot assign irq %d\n", DRIVER_NAME, irq);
            goto out;
        } else{
            sdport->irq = irq;
        }
    }

    timer_setup(&sdport->timer, rtksd_timeout_timer, 0);

    rtkem_chk_card_insert(sdport);

    /*The scripter are 8 bytes per line. align 64 byte */
    sdport->scrip_pool = dma_pool_create(DRIVER_NAME, sdport->dev,
                         EMMC_SCRIPT_BUF_SIZE, 64,
                         EMMC_SCRIPT_BUF_SIZE);
    if(!(sdport->scrip_pool)){
        EM_INFO( "%s: request Scripter Pool faill!\n",
               DRIVER_NAME);
        ret = -ENOMEM;
        goto out;
    }

    sdport->scrip_buf = dma_pool_alloc(sdport->scrip_pool,GFP_KERNEL ,
                     &sdport->scrip_buf_phy);
    if(!(sdport->scrip_buf)){
        EM_INFO( "%s: request Scripter Buffer fail!\n",DRIVER_NAME);
        dma_pool_destroy(sdport->scrip_pool);
        ret = -ENOMEM;
        goto out;
    }

    platform_set_drvdata(pdev, mmc);
    ret = mmc_add_host(mmc);
    if (ret){
        goto out;
    }

#ifdef CONFIG_CUSTOMER_TV006
    device_enable_async_suspend(&pdev->dev);
#endif

    EM_NOTICE( "%s: %s driver initialized\n",
               mmc_hostname(mmc), DRIVER_NAME);

    return 0;

out:
    if (sdport) {
        if (sdport->irq)
            free_irq(sdport->irq, sdport);

        if (sdport->base)
            devm_iounmap(&pdev->dev, sdport->base);
    }

    if (mmc)
        mmc_free_host(mmc);

    return ret;

}

/************************************************************************
 *
 ************************************************************************/
static
int __exit rtkem_remove(struct platform_device *pdev)
{
    struct mmc_host *mmc = platform_get_drvdata(pdev);

    device_remove_file(&pdev->dev, &dev_attr_cr_fast_RW);
    device_remove_file(&pdev->dev, &dev_attr_em_open_log);
    device_remove_file(&pdev->dev, &dev_attr_em_chk_mode);
    device_remove_file(&pdev->dev, &dev_attr_emmc_errcnt);
#ifdef CONFIG_MMC_RTKEMMC_HK_ATTR
    device_remove_file(&pdev->dev, &dev_attr_emmc_hacking);
#endif
    device_remove_file(&pdev->dev, &dev_attr_em_predefined_read);

    if (mmc) {
        struct rtksd_host *sdport = mmc_priv(mmc);

        rtkemmc_disable_int(sdport);
        tasklet_kill(&sdport->polling_tasklet);
        mmc_remove_host(mmc);
        if(!mmc){
            EM_NOTICE("eMMC host have removed.\n");
            mmc_host_local = NULL;
        }

        if(sdport->scrip_buf)
            dma_pool_free(sdport->scrip_pool, sdport->scrip_buf,
                         sdport->scrip_buf_phy);
        if(sdport->scrip_pool)
            dma_pool_destroy(sdport->scrip_pool);

        free_irq(sdport->irq, sdport);

        del_timer_sync(&sdport->timer);
        iounmap(sdport->base);

        release_resource(sdport->res);
        mmc_free_host(mmc);
    }
    platform_set_drvdata(pdev, NULL);
    return 0;
}

// #ifndef CONFIG_PM   //alexkh: for compile test
// #define CONFIG_PM
// #endif

#ifdef CONFIG_PM
/************************************************************************
 *
 ************************************************************************/
static
int rtkem_suspend(struct platform_device *dev, pm_message_t state)
{
    struct mmc_host *mmc = platform_get_drvdata(dev);
    int ret = 0;

    EM_NOTICE( "%s: Prepare to suspend...\n", DRIVER_NAME);

    if (mmc){
        struct rtksd_host *sdport = mmc_priv(mmc);

        rtkemmc_disable_int(sdport);
        tasklet_kill(&sdport->polling_tasklet);
        del_timer_sync(&sdport->timer);
        clear_bit(PORT_CARD_DETECTED,&sdport->port_status);

        EM_NOTICE( "%s: record:  r_phase50=%d r_phase200=%d ds_tune=%d\n",
            DRIVER_NAME,sdport->tud_r_pha50,sdport->tud_r_pha200 ,sdport->pre_ds_tune);

    }

#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)skip light sleep enable\n",__func__,__LINE__);
#else
    /*  what is this?? */
    /* light sleep enable */
    em_maskl(RTK_IP_MBIST, RTK_IP_MBIST_LS_MASK0, RTK_IP_MBIST_LS_SHIFT0, 1);
    em_maskl(RTK_IP_MBIST, RTK_IP_MBIST_LS_MASK1, RTK_IP_MBIST_LS_SHIFT1, 1);
    em_maskl(RTK_WRAP_MBIST, RTK_WRAP_MBIST_LS_MASK, RTK_WRAP_MBIST_LS_SHIFT, 1);
#endif

    return ret;
}


/************************************************************************
 *
 ************************************************************************/
static
int rtkem_resume(struct platform_device *dev)
{
    struct mmc_host *mmc = platform_get_drvdata(dev);
    int ret = 0;

    EM_NOTICE( "%s: wake up to resume...\n", DRIVER_NAME);
#ifdef MARK_FOR_MAC9
    EM_ALERT("%s(%d)skip light sleep disable\n",__func__,__LINE__);
#else
    /* light sleep disable */
    em_maskl(RTK_IP_MBIST, RTK_IP_MBIST_LS_MASK0, RTK_IP_MBIST_LS_SHIFT0, 0);
    em_maskl(RTK_IP_MBIST, RTK_IP_MBIST_LS_MASK1, RTK_IP_MBIST_LS_SHIFT1, 0);
    em_maskl(RTK_WRAP_MBIST, RTK_WRAP_MBIST_LS_MASK, RTK_WRAP_MBIST_LS_SHIFT, 0);
#endif
    if (mmc){
        struct rtksd_host *sdport = mmc_priv(mmc);

        tasklet_init(&sdport->polling_tasklet, rtkem_polling_tasklet,
                (unsigned long)sdport);

        /* enable PLL of eMMC */
        dw_em_set_clk(sdport,CLK_200Khz);

        dw_em_reset_host(sdport,0);
        dwc51_reset_card(sdport);
        rtkem_clr_sta(sdport);
        rtkemmc_enable_int(sdport);

        set_bit(PORT_CARD_DETECTED,&sdport->port_status);
        set_bit(PORT_SKIP_DSTUNE,&sdport->port_status);
        set_bit(PORT_SKIP_PHASETUNE,&sdport->port_status);

        EM_NOTICE( "%s: read back:  r_phase50=%d r_phase200=%d ds_tune=%d\n",
            DRIVER_NAME,sdport->tud_r_pha50,sdport->tud_r_pha200 ,sdport->pre_ds_tune);

    }

    EM_NOTICE("%s: resume finish\n",DRIVER_NAME);
    return ret;
}
#else
#define rtkem_suspend   NULL
#define rtkem_resume    NULL
#endif

#ifdef CONFIG_OF
static const struct of_device_id of_rtkemmc_ids[] = {
    { .compatible = "realtek,rtk-emmc" },
    {}
};

MODULE_DEVICE_TABLE(of, of_rtkemmc_ids);
#endif

static
struct platform_driver rtkemmc_driver = {
    .driver     = {
        .name   = DRIVER_NAME,
        .owner  = THIS_MODULE,
#ifdef CONFIG_OF
        .of_match_table = of_rtkemmc_ids,
#endif
    },
    .remove     = __exit_p(rtkem_remove),
    .suspend    = rtkem_suspend,
    .resume     = rtkem_resume,

};

/************************************************************************
 *
 ************************************************************************/
static
void rtkcr_display_version (void)
{
#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
    const __u8 *revision;
    const __u8 *date;
    const __u8 *time;
    char *running;
    char tmp[256] = {0};

    memcpy(tmp,VERSION,strlen(VERSION));
    running = tmp;

    strsep(&running, " ");
    strsep(&running, " ");
    revision = strsep(&running, " ");
    date = strsep(&running, " ");
    time = strsep(&running, " ");
    EM_NOTICE(BANNER " Rev:%s (%s %s)\n", revision, date, time);

#endif
}

#ifndef CONFIG_MMC_RTKEMMC_PLUS_MODULE
static int dw_em_log_func(char *buf)
{
#if 1
    EM_NOTICE("%s:mark for fix rtkcr_chk_param()\n",
                DRIVER_NAME,rtk_emmc_log_state);
    return 0;
#else
    /*
     For customer request, valid value from 0~15(0xf).
      1. without emmclog or emmclog=0, disable message function.
      2. emmclog=1, enable message for this request.
      3. emmclog=other_value , for future request.
     */
    rtkcr_chk_param(&rtk_emmc_log_state,1,buf+1);
    EM_NOTICE("************************************************\n");
    EM_NOTICE("%s:Bootcode setting emmc_log func is %u\n",
                DRIVER_NAME,rtk_emmc_log_state);
    return 0;
#endif
}
__setup("emmc_log",dw_em_log_func);

#else //CONFIG_MMC_RTKEMMC_PLUS_MODULE
void dw_em_log_func_module(void)
{
#if 1
    EM_NOTICE("%s:mark for fix rtkcr_chk_param()\n",
                DRIVER_NAME,rtk_emmc_log_state);
    return;
#else
    char strings[20];
    char *buf = strings;

    if(rtk_parse_commandline_equal("emmc_log", buf, sizeof(strings)) == 0){
        //can't get emmc_log from bootargs
        return;
    }else{
        EM_ERR("get emmc_log from bootargs\n");
    }

    /*
     For customer request, valid value from 0~15(0xf).
      1. without emmclog or emmclog=0, disable message function.
      2. emmclog=1, enable message for this request.
      3. emmclog=other_value , for future request.
     */
    rtkcr_chk_param(&rtk_emmc_log_state,1,buf+1);
    EM_NOTICE("************************************************\n");
    EM_NOTICE("%s:Bootcode setting emmc_log func is %u\n",
                DRIVER_NAME,rtk_emmc_log_state);
    return ;
#endif
}
#endif

static
int __init rtkemmc_init(void)
{
    int rc = 0;

    rtkcr_display_version();

    mmcdbg("mmc driver entry!!!!!!!!!!\n");

#ifdef CONFIG_MMC_RTKEMMC_PLUS_MODULE
    dw_em_log_func_module();
#endif

    rc = platform_driver_probe(&rtkemmc_driver, rtkemmc_probe);

    if (rc < 0){
        EM_NOTICE( "Realtek EMMC Controller Driver installation fails.\n\n");
        return -ENODEV;
    }else{
        EM_NOTICE( "Realtek EMMC Controller Driver is successfully installing.\n\n");
        return 0;
    }
}

static
void __exit rtkemmc_exit(void)
{
    platform_driver_unregister(&rtkemmc_driver);
}

/* allow emmc driver initialization earlier */
#ifdef CONFIG_MMC_RTKEMMC_PLUS_MODULE
module_init(rtkemmc_init);
#else

module_init(rtkemmc_init);
//subsys_initcall(rtkemmc_init);

#endif

module_exit(rtkemmc_exit);

/* maximum card clock frequency (default 50MHz) */
module_param(maxfreq, int, 0);

/* force PIO transfers all the time */
module_param(nodma, int, 0);

MODULE_AUTHOR("Elbereth");
MODULE_DESCRIPTION("Realtek EMMC Host Controller driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:rtkemmc");

