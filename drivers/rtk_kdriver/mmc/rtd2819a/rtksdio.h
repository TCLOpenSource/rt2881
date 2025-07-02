/*
 *  Copyright (C) 2010 Realtek Semiconductors, All Rights Reserved.
 */




//rename this file to "rtkemmc_51.h"

#ifndef __RTKSDIO_H
#define __RTKSDIO_H

#include <linux/version.h>

#ifdef CONFIG_RTK_KDRIVER_SUPPORT
#include <rtk_kdriver/rtk_crt.h>
#else
#include <mach/rtk_crt.h>
#endif

#include <rtk_kdriver/io.h>
#include "rtkemmc_dbg.h"

#include "../../../mmc/core/card.h"
#include "../../../mmc/core/host.h"
#include "../../../mmc/core/core.h"
#include "../../../mmc/core/mmc_ops.h"


/************************************************************************
 *  Definition
 ************************************************************************/
#define MARK_FOR_MAC9   //There is some thing need to fix. keep this...

/************************************************************************
 * REALTEK CONTROL REGISTER ACCESSING METHOD
*************************************************************************/
//#define RBUS_BE_MODE
#ifndef RBUS_BE_MODE

/* Normal RBUS usage */
#define em_outb(addr,value)       rtd_outb(addr, value)
#define em_inb(addr)              rtd_inb(addr)
//#define em_maskb(reg,msk,sht,val) rtkem_maskb(reg,msk,sht,val)
#define em_outw(addr,value)       rtd_outw(addr,value)
#define em_inw(addr)              rtd_inw(addr)
//#define em_maskw(reg,msk,sht,val) rtkem_maskw(reg,msk,sht,val
#define em_outl(addr,value)       rtd_outl(addr,value)
#define em_inl(addr)              rtd_inl(addr)

#define em_maskl(reg,msk,sht,val)  \
        em_outl(reg, (em_inl(reg) & ~(msk<<sht))|(val<<sht))

#else

/* RBUS BE function usage */
void em51_outl_be(unsigned int addr,unsigned int val);
unsigned int em51_inl_be(unsigned int addr);
void em51_outw_be(unsigned int addr,unsigned short val);
unsigned short em51_inw_be(unsigned int addr);
void em51_outb_be(unsigned int addr,unsigned char val);
unsigned short em51_inb_be(unsigned int addr);

#define em_outb(addr,value)       em51_outb_be(addr,value)
#define em_inb(addr)              em51_inb_be(addr)
#define em_maskb(reg,msk,sht,val) \
                em51_outb_be(reg, (em51_inb_be(reg) & ~(msk<<sht))|(val<<sht))
#define em_outw(addr,value)       em51_outw_be(addr,value)
#define em_inw(addr)              em51_inw_be(addr)
#define em_maskw(reg,msk,sht,val) \
                em51_outw_be((em51_inw_be(reg) & ~(msk<<sht))|(val<<sht), reg)
#define em_outl(addr,value)       em51_outl_be(addr,value)
#define em_inl(addr)              em51_inl_be(addr)
#define em_maskl(reg,msk,sht,val) \
                em51_outl_be((em51_inl_be(reg) & ~(msk<<sht))|(val<<sht), reg)


static
void em51_outl_be(unsigned int addr,unsigned int val)
{
    rtkem_outl(addr,val);
}

static
unsigned int em51_inl_be(unsigned int addr)
{
    unsigned int val;
    val = rtkem_inl(addr);
    return val;
}

static
void em51_outw_be(unsigned int addr,unsigned short val)
{
    unsigned int offset;
    unsigned int vaild_byte;
    unsigned int val_32;

    if((addr >= DWC51_BASE) && (addr < DWC51_BASE_END)){
        val_32 = (unsigned int )val;
        offset = addr & 0x3;
        if(offset == 0){
            vaild_byte = 0x0011;
        }else if(offset == 2){
            vaild_byte = 0x1100;
            val_32 = (val_32 << 16);
        }else{
//            EMMC_PRINTS("error addr setting in \"");
//            EMMC_PRINTS(__FUNCTION__);
//            EMMC_PRINTS(" \" skip access\n");
//            EMMC_PRINTS_HEX("(offset=", offset);
            return;
        }

        rtkem_outl(EM51_RBUS_BE_CTRL,vaild_byte|RBUS_BE_EN);
        rtkem_outl(addr & 0xffffffc,val_32);
        rtkem_outl(EM51_RBUS_BE_CTRL,0);
    }else{
//        EMMC_PRINTS("error addr range setting in \"");
//        EMMC_PRINTS(__FUNCTION__);
//        EMMC_PRINTS(" \" skip access\n");
//        EMMC_PRINTS_HEX("(offset=", offset);
    }
}

static
unsigned short em51_inw_be(unsigned int addr)
{
    unsigned int offset;
    unsigned int vaild_byte;
    unsigned short val;
    unsigned int val_32;

    if((addr >= DWC51_BASE) && (addr < DWC51_BASE_END)){
        offset = addr & 0x3;
        if(offset == 0)
            vaild_byte = 0x0011;
        else if(offset == 2)
            vaild_byte = 0x1100;
        else{
//            EMMC_PRINTS("error addr setting in \"");
//            EMMC_PRINTS(__FUNCTION__);
//            EMMC_PRINTS(" \" skip access\n");
//            EMMC_PRINTS_HEX("(offset=", offset);
            val = 0xdead;
            goto OUT;
        }
        rtkem_outl(EM51_RBUS_BE_CTRL,vaild_byte|RBUS_BE_EN);
        val_32 = rtkem_inl(addr & 0xffffffc);
        val = (unsigned short)(val_32 >> (offset*8));
        rtkem_outl(EM51_RBUS_BE_CTRL,0);
    }else{
//        EMMC_PRINTS("error addr range setting in \"");
//        EMMC_PRINTS(__FUNCTION__);
//        EMMC_PRINTS(" \" skip access\n");
//        EMMC_PRINTS_HEX("(offset=", offset);
        val = 0xdead;
    }
OUT:
    return val;
}

static
void em51_outb_be(unsigned int addr,unsigned char val)
{
    unsigned int offset;
    unsigned int vaild_byte;
    unsigned int val_32;

    if((addr >= DWC51_BASE) && (addr < DWC51_BASE_END)){
        val_32 = (unsigned int)val;
        offset = addr & 0x3;
        if(offset == 0){
            vaild_byte = 0x0001;
        }else if(offset == 1){
            vaild_byte = 0x0010;
        }else if(offset == 2){
            vaild_byte = 0x0100;
        }else{
            vaild_byte = 0x1000;
        }
        val_32 = (val_32 << (offset*8));
        rtkem_outl(EM51_RBUS_BE_CTRL,vaild_byte|RBUS_BE_EN);
        rtkem_outl(addr & 0xffffffc,val_32);
        rtkem_outl(EM51_RBUS_BE_CTRL,0);
    }else{
//        EMMC_PRINTS("error addr range setting in \"");
//        EMMC_PRINTS(__FUNCTION__);
//        EMMC_PRINTS(" \" skip access\n");
//        EMMC_PRINTS_HEX("(offset=", offset);
    }

}

static
unsigned short em51_inb_be(unsigned int addr)
{
    unsigned int offset;
    unsigned int vaild_byte;
    unsigned char val;
    unsigned int val_32;

    if((addr >= DWC51_BASE) && (addr < DWC51_BASE_END)){
        offset = addr & 0x3;
         if(offset == 0)
            vaild_byte = 0x0001;
        else if(offset == 1)
            vaild_byte = 0x0010;
        else if(offset == 2)
            vaild_byte = 0x0100;
        else
            vaild_byte = 0x1000;

        rtkem_outl(EM51_RBUS_BE_CTRL,vaild_byte|RBUS_BE_EN);
        val_32 = rtkem_inl(addr & 0xffffffc);
        val = (unsigned char)(val_32 << (offset*8));
        rtkem_outl(EM51_RBUS_BE_CTRL,0);

        return val;
    }else{
//        EMMC_PRINTS("error addr range setting in \"");
//        EMMC_PRINTS(__FUNCTION__);
//        EMMC_PRINTS(" \" skip access\n");
//        EMMC_PRINTS_HEX("(offset=", offset);
        return 0xde;
    }
}

#endif // of #ifdef RBUS_BE_MODE


/* all error bit define at "Device Status". */
#define RESP_ERR_BIT    ( BIT(31)|BIT(30)|BIT(29)|BIT(28)|   \
                BIT(27)|BIT(26)|BIT(24)|BIT(23)|   \
                BIT(22)|BIT(21)|BIT(20)|BIT(19)|   \
                BIT(16)|BIT(15)|BIT(13)|BIT(7)  )

/* fatal error, if these bits alert the command should skip retry. not figure out yet. */
#define RESP_ERR_FATAL  ( BIT(31)|BIT(30)|BIT(29)|BIT(28)|   \
                BIT(27)|BIT(26)|BIT(24)|BIT(23)|   \
                BIT(22)|BIT(21)|BIT(20)|BIT(19)|   \
                BIT(16)|BIT(15)|BIT(13)|BIT(7)  )

/*  ===== macros and funcitons for Realtek CR ===== */
/* for SD/MMC usage */
#define ON                      0
#define OFF                     1

#define MMC_EN_SSCG             BIT(0)

typedef struct  {
    u32                 ctl;
    u8                  ncode_t;
    u8                  sdiv2;
    u8                  r_phase;
    u8                  w_phase;
    u8                  icp;
    u8                  pi_isel;
    u16                 ncode_ssc;
    u16                 fcode_ssc;
    u32                 range_ssc;
}mmc_param_t;


typedef struct  {
#define PHASE_SAVED     BIT(0)
#define PHASE_CHANGED   BIT(1)
#define DSTUNE_CHANGED  BIT(2)
#define PHASE_FORCE     BIT(3)
    u32 ctl;
    u32 test_ctl_old;

    u32 fix_wphase_old;
    u32 fix_dstune_old;

    u8  wphase;
    u8  rphase;
    u8  cphase;
    u8  ds_mode;
    u8  ds_tune;

    u8  wphase_old;
    u8  rphase_old;
    u8  cphase_old;
    u8  ds_mode_old;
    u8  ds_tune_old;
    u8  ds_result_old;
}mmc_phase_t;

/* tasklet state */
#define STATE_IDLE          0
#define STATE_SENDING_CMD   1
#define STATE_SENDING_DATA  2
#define STATE_PRE_COMPLETE  3
#define STATE_REQUEST_END   4
#define STATE_ERROR_HADLE   5
#define STATE_WAIT_HOST     6
#define STATE_SEND_SECOND   7

/* recover method (recover_mothod) */
#define RECOVER_NO_NEED         (0)
#define RECOVER_OVER_CRT_RST        (1)
#define RECOVER_OVER_DWC_RST        (2) //cmd+data
#define RECOVER_OVER_DWC_CMD_RST    (3) //cmd only
#define RECOVER_OVER_DWC_ALL_RST    (4) // Reset For All.
                                        //  If this bit is set to 1,
                                        //  theHost Driver must issue reset command and
                                        //  reinitialize the card. */
#define RECOVER_OVER_UNKNOW         (5)

 typedef struct{
    u8 mid;
    u8 config;
    u8  pnm[6];
    u8  legacy25_cmd_w;
    u8  legacy25_dq_w;
    u8  legacy25_dq_r;
    u8  legacy25_ds;

    u8  HS50_cmd_w;
    u8  HS50_dq_w;
    u8  HS50_dq_r;
    u8  HS50_ds;

    u8  HS200_cmd_w;
    u8  HS200_dq_w;
    u8  HS200_dq_r;
    u8  HS200_ds;

    u8  HS400_cmd_w;
    u8  HS400_dq_w;
    u8  HS400_dq_r;
    u8  HS400_ds;
} vender_info_t;

struct sd_cmd_pkt {
    struct mmc_host     *mmc;       /* MMC structure */
    //struct rtksd_host   *sdport;
    struct mmc_command  *sbc;    /* cmd->opcode; cmd->arg; cmd->resp; cmd->data */
    struct mmc_command  *cmd;
    struct mmc_command  *stop;
    struct mmc_data     *data;
    unsigned char       *dma_buffer;
    u16                 byte_count;
    u16                 block_count;
    u32                 total_byte_cnt;
    u32                 scrip_cnt;

    u32                 cmd_flag;
    u32            		xfermode;

#define AUTO_NONE       (0)
#define AUTO_CMD12      BIT(0)
#define AUTO_CMD23      BIT(1)
    u8                  auto_cmd_type;
    u32                 flags;      // seems useless now but in raw driver
    u32                 tmout;      // Upper limit of command timeout

#define DATA_IRQ_BIT        (11)    // for debug, force go to tasklet to handle data process
#define CMD_IRQ_BIT         (10)    // or debug, force go to tasklet to handle cmd process
#define CMD_TMOUT_BIT       (9)     // seted if enter timeout func
#define SECOND_CMD_BIT      (8)     // manual second cmd for manual cmd23 or cmd12
#define DATA_UNEXPT_BIT     (7)     // useless now
#define CMD_UNEXPT_BIT      (6)     // seted if into unexpect flow in tasklet
#define SANITIZE_BIT        (5)     // sanitize case. what action needs to do?
#define CMD_ERR_BIT         (4)     // seted if the cmd stage error alerted.
#define REQ_FINISH_BIT      (3)     // seted if request have done, Avoid reports again in tasklet.
#define TIMER_STOP_BIT      (2)     // seted if decide to stop timer
#define IRQ_ALERT_BIT       (1)     // seted if command process has entered interrupt mode
#define SG_MAPPED_BIT       (0)     // seted if sg list mapped. clear it when unmap.
    unsigned long       cmd_status;

    u32 auto_cmd_stat;   // rec AUTO_CMD_STAT_R
    u32 adma_err_stat;   // rec ADMA_ERR_STAT_R



};

struct rtksd_host {
    struct device       *dev;           /* kernel device pointer */
    struct mmc_request  *mrq;           /* Current request */
    struct mmc_host     *mmc;           /* MMC structure */
    struct sd_cmd_pkt   cmd_info;
    struct cqhci_host *cq_host;
    u32                 log_state;      /* log_func state */

    u8                  cmd_opcode;
    u8                  int_mode;

    u32                 cur_PLL;    //PLL clock
    u32                 cur_clock;  //current MMC bus clock
    u32                 cur_div;    //current logic div of PLL clock.
    u32                 cur_width;  //pair with bus_width,for display only
    u32                 bus_width;  //pair with cur_width,for HW setting
    u8                  cur_timing;
    u32                 cur_driving;
    struct mmc_ios curr_ios;

    u8                  cur_w_phase;
    u8                  cur_w_ph400;
    u8                  cur_r_pha50;
    u8                  cur_r_phase;
    u8                  cur_c_phase;

    u8                  tud_r_pha50;
    u8                  tud_r_pha200;

    u32                 cur_ds_tune;
    u32                 pre_ds_tune;


    u32                 mid;
    u8                  pnm[7];
    u8                  reset_event;

    u8                  wp;
    struct rtk_host_ops *ops;

    void __iomem        *base;

    spinlock_t          lock;

    struct tasklet_struct polling_tasklet;
    struct timer_list   timer;
    struct resource     *res;
    int                 irq;

    struct  dma_pool    *scrip_pool;
    void                *scrip_buf;
    dma_addr_t          scrip_buf_phy;

#ifdef MONI_MEM_TRASH
    u8                  *cmp_buf;
    dma_addr_t          phy_addr;
#endif

    u32                 int_status;         // interrupt reason
    unsigned long       completed_events;   //status has handle done
    unsigned long       pending_events;     //status need be handled
    u32                 task_state;

    u8                  vender_phase_num;
    vender_info_t       *vender_phase_ptr;

    /* for bootcode pre setting parameter *** */
    mmc_param_t *mmc_boot_param;
    u32 *mmc_io_drv;
    u32 mmc_drv;
    u32 mmc_dstune;
    u32 mmc_burst_blk;
    /* for bootcode pre setting parameter &&& */

    #define MOD_OPENEND     (0)
    #define MOD_PREDEF      (1)
    u8                  access_mode;    //pre-define or open-end

    u32 busy_cnt;

    //bool                hs50_tuned;
    //bool                hs200_tuned;
    //bool                hs400_tuned;
    bool cqe_on;


#define PORT_HS50_TUNED         (12) /*  */
#define PORT_HS200_TUNED        (11) /*  */
#define PORT_HS400_TUNED        (10) /*  */
#define PORT_CARD_DETECTED      (9) /* Card is detected */
#define PORT_FOPEN_LOG          (8) /* open command log */
#define PORT_USER_PARTITION     (7) /* card is working on normal partition */
#define PORT_TUNING_STATE       (6) /* Host is processing phase tuning */
#define PORT_RECOVER_STATE      (5) /* Host is processing recover flow */
#define PORT_FTIMER_CHECK       (4) /* enable timerout check func */
#define PORT_SKIP_PHASETUNE     (3)
#define PORT_SKIP_DSTUNE        (2)
#define PORT_OCCUPY             (1)
#define PORT_IS_RPMB            (0) /* flag to check if working in RPMB */
    unsigned long       port_status;

};

#define DRIVING_COMMON          (0)
#define DRIVING_HS200           (1)
#define DRIVING_HS400           (2)
#define DRIVING_HS50            (3)
#define DRIVING_25M             (4)

static const char *const dw_driving_tlb[6] = {
    "COMMON",
    "HS200",
    "HS400",
    "HS50",
    "Legarcy",
    "unknow"
};


#define CLK_200Khz      200000
#define CLK_400Khz      400000

#define CLK_10Mhz       10000000
#define CLK_15Mhz       15000000
#define CLK_20Mhz       20000000
#define CLK_25Mhz       25000000
#define CLK_30Mhz       30000000
#define CLK_40Mhz       40000000
#define CLK_50Mhz       50000000
#define CLK_60Mhz       60000000
#define CLK_70Mhz       70000000
#define CLK_80Mhz       80000000
#define CLK_90Mhz       90000000

#define CLK_100Mhz      100000000
#define CLK_110Mhz      110000000
#define CLK_120Mhz      120000000
#define CLK_130Mhz      130000000
#define CLK_140Mhz      140000000
#define CLK_150Mhz      150000000
#define CLK_160Mhz      160000000
#define CLK_170Mhz      170000000
#define CLK_180Mhz      180000000
#define CLK_190Mhz      190000000
#define CLK_200Mhz      200000000


/* send status event */
#define STATE_IDLE          0
#define STATE_READY         1
#define STATE_IDENT         2
#define STATE_STBY          3
#define STATE_TRAN          4
#define STATE_DATA          5
#define STATE_RCV           6
#define STATE_PRG           7
#define STATE_DIS           8


static const char *const state_tlb[9] = {
    "STATE_IDLE",
    "STATE_READY",
    "STATE_IDENT",
    "STATE_STBY",
    "STATE_TRAN",
    "STATE_DATA",
    "STATE_RCV",
    "STATE_PRG",
    "STATE_DIS"
};

#define BIT_WIDTH_CNT (4)
static const char *const bit_tlb[4] = {
    "1bit",
    "4bits",
    "8bits",
    "unknow"
};


#define MAX_TUNING_STEP         (32)    //skip 28~31 step
#define MAX_DS_TUNE             (32)    //Only 0~27 are available even hardware has 32 step


/* interrup action */
#define INT_ACT_DISABLE         (0)
#define INT_ACT_ENABLE          (1)
#define INT_GET_CLR             (2)
#define INT_GET_STA             (3)
#define INT_CLR_RSN             (4)
#define MODE_POLLING            (0)
#define MODE_INTERRUPT          (1)

/************************************************************************
 *  buffer define
 ************************************************************************/
#ifdef  ADMA_LENG_26BIT
#define EMMC_MAX_SCRIPT_BLK         (0x1FFFF)
#else
#define EMMC_MAX_SCRIPT_BLK         (0x70)  /* HW support to 0x7F */
#endif
#define EMMC_MAX_SCRIPT_BYTE         (EMMC_MAX_SCRIPT_BLK*0x200)

#define EMMC_SCRIPT_LINE_BYTE_NUM   (8)

#define EMMC_SCRIPT_BUF_SIZE        (4096)
#define EMMC_MAX_SCRIPT_LINE        (EMMC_SCRIPT_BUF_SIZE/EMMC_SCRIPT_LINE_BYTE_NUM)

#include "rtk_mmc_2819a.h" /* mac9 */


#endif
