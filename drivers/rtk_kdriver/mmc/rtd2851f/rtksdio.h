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
#include "rtksdio_dbg.h"

#include "../../../mmc/core/card.h"
#include "../../../mmc/core/host.h"
#include "../../../mmc/core/core.h"
#include "../../../mmc/core/mmc_ops.h"

#include <rbus/sys_reg_reg.h>
#include <rbus/pll27x_reg_reg.h>

typedef void (*set_gpio_func_t)(u32 gpio_num,u8 ctl);


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

/**********************************************************************
**
** parse phase param from cmdline
**
**********************************************************************/
enum mmc_mode {
	MMC_LEGACY_MODE = 0,
	MMC_HS50_MODE,
	MMC_HS200_MODE,
	MMC_HS400_MODE,
	MMC_MAX_MODE,
};

struct phase_st{
	unsigned int cmd_phase;
	unsigned int data_rphase;
	unsigned int ds_phase;
	unsigned int data_wphase;

	unsigned int reserved[3];
	unsigned int valid_flag;
#define PHASE_VAL_ENABLE_FLAG		0xE0
#define PHASE_VAL_DISABLE_FLAG		0x00
};


/*
 * Clock rates
 */
#define cr_readl(offset)        rtd_inl(offset)
#define cr_writel(val, offset)  rtd_outl(offset, val)

static inline void reg_maskl(const unsigned int mask,
                const unsigned int val, __u32 __iomem regs)
{
    cr_writel((cr_readl(regs) & ~mask) | val,regs);
}

/*  ===== macros and funcitons for Realtek CR ===== */
/* for SD/MMC usage */
#define ON                      0
#define OFF                     1

#define GPIO_H      (0x1<<0)
#define GPIO_L      (0)
#define DIR_OUT     (0x1<<1)
#define DIR_IN      (0)
#define DIR_SKIP    (0x1<<2)
#define MUX_SKIP    (0x1<<3)
#define ISO_FLAG    (0x800UL)

/* MMC response type */
#define SD_R0                   (RESP_TYPE_NON|CRC7_CHK_DIS)
#define SD_R1                   (RESP_TYPE_6B)
#define SD_R1b                  (RESP_TYPE_6B)
#define SD_R2                   (RESP_TYPE_17B)
#define SD_R3                   (RESP_TYPE_6B|CRC7_CHK_DIS)
#define SD_R4                   (RESP_TYPE_6B)
#define SD_R5                   (RESP_TYPE_6B)
#define SD_R6                   (RESP_TYPE_6B)
#define SD_R7                   (RESP_TYPE_6B)
#define SD_R_NO                 (0xFF)

/* rtflags */
#define RTKCR_FCARD_DETECTED    BIT(0)      /* Card is detected */
#define RTKCR_FOPEN_LOG         BIT(1)      /* open command log */
#define RTKCR_USER_PARTITION    BIT(2)      /* card is working on normal partition */
#define RTKCR_IDENT_STATE       BIT(3)      /* card is identification mode */
#define RTKCR_FCARD_POWER       BIT(4)      /* Card is power on */
#define RTKCR_FHOST_POWER       BIT(5)      /* Host is power on */
#define RTKCR_TUNING_STATE      BIT(6)      /* Host is processing phase tuning */
#define RTKCR_RECOVER_STATE     BIT(7)      /* Host is processing recover flow */
#define RTKCR_FOPEN_INT         BIT(8)      /* open interrupt */
#define RTKCR_CMD23_ALERT       BIT(9)      /* Host is processing cmd23 flow */
#define RTKCR_FTIMER_CHECK      BIT(10)     /* enable timerout check func */


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


#define STATE_IDLE          0
#define STATE_SENDING_CMD   1
#define STATE_SENDING_DATA  2
#define STATE_DATA_BUSY     3
#define STATE_POST_POLLING  4
#define STATE_SENDING_STOP  5
#define STATE_DATA_ERROR    6
#define STATE_DATA_COMPLETE 7
#define STATE_WAIT_STOP     8
#define STATE_REQUEST_END   9

struct rtk_host_status{
    u32 config1;
    u32 config2;
    u32 config3;
    u32 int_mask;
};

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

struct rtksd_host {
    struct mmc_host     *mmc;           /* MMC structure */
    struct mmc_command  *cmd;
    u32                 rtflags;        /* Driver states */
    u32                 log_state;      /* log_func state */
    u8                  ins_event;
    u8                  cmd_opcode;
    u8                  int_mode;

#define EVENT_NON               (0)
#define EVENT_INSER             BIT(0)
#define EVENT_REMOV             BIT(1)
#define EVENT_SKIP_PHASETUNE    BIT(4)
#define EVENT_SKIP_DSTUNING     BIT(5)
    u32                 cur_clock;
    u32                 cur_width;
    u32                 cur_PLL;
    u32                 cur_div;
    u8                  cur_w_phase;
    u8                  cur_w_ph400;
    u8                  cur_r_pha50;
    u8                  cur_r_phase;
    u8                  cur_c_phase;
    u8                  tud_r_pha50;
    u8                  tud_r_pha200;
    u32                 cur_ds_tune;
    u32                 pre_ds_tune;
    u8                  cur_timing;
    bool                hs50_tuned;
    bool                hs200_tuned;
    bool                hs400_tuned;
    struct rtk_host_status cur_host_status;
    u32                 mid;
    u8                  pnm[7];

    u8                  reset_event;
    struct mmc_request  *mrq;            /* Current request */
    u8                  wp;
    struct rtk_host_ops *ops;
    struct semaphore    sem;
    struct semaphore    sem_op_end;

    void __iomem        *base;
    u32 base_io;
    spinlock_t          lock;
    unsigned int        ns_per_clk;
    struct workqueue_struct *cr_workqueue;
    struct delayed_work cr_work;
    struct tasklet_struct req_end_tasklet;
    struct tasklet_struct polling_tasklet;
    struct sd_cmd_pkt   *cmd_info;
    struct sd_cmd_pkt   *stop_info;
    struct timer_list   timer;
    struct timer_list   plug_timer;
    struct completion   *int_waiting;
    struct device       *dev;
    struct resource     *res;
    int                 irq;
    u8                  *tmp_buf;
    dma_addr_t          tmp_buf_phy_addr;
    struct  dma_pool    *scrip_pool;
    void                *scrip_buf;
    dma_addr_t          scrip_buf_phy;

#ifdef MONI_MEM_TRASH
    u8                  *cmp_buf;
    dma_addr_t          phy_addr;
#endif

    u8                  pinmux_type;
    u32                 test_count;
    u32                 int_status;
    u32                 int_status_old;
    u32                 sd_status;
    u32                 sd_status2;
    u32                 sd_trans;
    u32                 wrap_sta;
    u32                 gpio_isr_info;
    u32                 task_state;
    u32                 tmout;
    u8                  vender_phase_num;
    vender_info_t *vender_phase_ptr;

    /* for bootcode pre setting parameter *** */
    mmc_param_t *mmc_boot_param;
    u32 *mmc_io_drv;
    u32 mmc_drv;
    u32 mmc_dstune;
    u32 mmc_burst_blk;
    /* for bootcode pre setting parameter &&& */

    unsigned long       timeend;
};

struct rtk_host_ops {
    irqreturn_t (*func_irq)(int irq, void *dev);
    int (*re_init_proc)(struct mmc_card *card);
    int (*card_det)(struct rtksd_host *sdport);
    void (*card_power)(struct rtksd_host *sdport,u8 status);
    void (*chk_card_insert)(struct rtksd_host *rtkhost);
    void (*set_crt_muxpad)(struct rtksd_host *rtkhost);
    void (*set_clk)(struct rtksd_host *rtkhost,u32 mmc_clk);
    void (*reset_card)(struct rtksd_host *rtkhost);
    void (*reset_host)(struct rtksd_host *rtkhost,u8 save_en);
    void (*set_IO_driving)(struct rtksd_host *rtkhost,u32 type_sel);
    void (*bus_speed_down)(struct rtksd_host *sdport);
    u32 (*get_cmdcode)(u32 opcode );
    u32 (*get_r1_type)(u32 opcode );
    u32 (*chk_cmdcode)(struct mmc_command* cmd);
    u32 (*chk_r1_type)(struct mmc_command* cmd);
    void (*hold_card)(struct rtksd_host *sdport);
    int (*set_bits)(struct rtksd_host *sdport,u32 bus_wid);
    int (*set_DDR)(struct rtksd_host *sdport,u32 ddr_mode);
};

struct ms_cmd_pkt {
    struct rtksd_host   *sdport;
    struct ms_command   *mscmd;
    struct mmc_data     *data;

    unsigned char       *dma_buffer;    //data transfer address
    u16                 cmdcode;        //rtk ms operation code
    u16                 trans_bytes;    //register transfer bytes
    u16                 block_count;    //data block transfer count
};

struct sd_cmd_pkt {
    struct mmc_host     *mmc;       /* MMC structure */
    struct rtksd_host   *sdport;
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
    u32                 cmdcode;
    u32                 flags;
    u8                  rsp_para;
    u8                  rsp_len;
    u32                 timeout;
    u8                  sbc_flag;
    u8                  predefined_read;
};

#define DRIVING_COMMON          0
#define DRIVING_HS200           1
#define DRIVING_HS400           2
#define DRIVING_HS50            3
#define DRIVING_25M             4

/* CRT_SYS_CLKSEL setting bit *** */
#define EMMC_SOURCECLKSEL_SHT       (27)
#define MASK_EMMC_SOURCECLKSEL      (0x01UL<<EMMC_SOURCECLKSEL_SHT)
#define CR_SRC_CLK_250M             (0x00UL<<EMMC_SOURCECLKSEL_SHT)
#define CR_SRC_CLK_295M             (0x01UL<<EMMC_SOURCECLKSEL_SHT)

#define SD_CLKSEL_SHT               (20)

#define SD_CLKSEL_MASK              (0x07UL<<SD_CLKSEL_SHT)
#define SD_CLOCK_SPEED_GAP          (CARD_SWITCHCLOCK_25MHZ<<SD_CLKSEL_SHT)

#define CARD_SWITCHCLOCK_75MHZ      (0x00UL)
#define CARD_SWITCHCLOCK_50MHZ      (0x01UL)
#define CARD_SWITCHCLOCK_37MHZ      (0x02UL)
#define CARD_SWITCHCLOCK_30MHZ      (0x03UL)
#define CARD_SWITCHCLOCK_25MHZ      (0x04UL)
#define CARD_SWITCHCLOCK_18MHZ      (0x05UL)
#define CARD_SWITCHCLOCK_15MHZ      (0x06UL)

#define LOWEST_CLOCK    CARD_SWITCHCLOCK_15MHZ

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

#define RTK_CLK_STEP   7

static const
u32 rtk_clock_tbl[RTK_CLK_STEP] = {
    75000000,
    50000000,
    37000000,
    30000000,
    25000000,
    18000000,
    15000000
};

#define RTK_DIV_CLK_128  (rtk_clock_tbl[RTK_CLK_STEP-1]>>7) //clock divide with 128
#define RTK_DIV_CLK_256  (rtk_clock_tbl[RTK_CLK_STEP-1]>>8) //clock divide with 256

#define RCA_SHIFTER         16

#define BYTE_CNT            0x200
#define RTK_NORMAL_SPEED    0x00
#define RTK_HIGH_SPEED      0x01
#define RTK_1_BITS          0x00
#define RTK_4_BITS          0x10
#define RTK_BITS_MASK       0x30
#define RTK_SPEED_MASK      0x01
#define RTK_PHASE_MASK      0x06

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

static const char *const bit_tlb[4] = {
    "1bit",
    "4bits",
    "8bits",
    "unknow"
};

static const char *const clk_tlb[7] = {
    "75MHz",
    "50MHz",
    "37MHz",
    "30MHz",
    "25MHz",
    "18MHz",
    "15MHz"
};

/* data read cmd */
static const u8  opcode_r_type[16] = {
    0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,0
};

/* data write cmd */
static const u8  opcode_w_type[16] = {
    1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0
};

/* data xfer cmd */
static const u8 opcode_d_type[16] = {
    1,1,1,0,0,1,1,0,0,1,1,0,1,1,1,0
};

static const char *const cmdcode_tlb[16] = {
    "N_W",      /* 0 */
    "AW3",      /* 1 */
    "AW4",      /* 2 */
    "UNK",      /* 3 */
    "UNK",      /* 4 */
    "AR3",      /* 5 */
    "AR4",      /* 6 */
    "UNK",      /* 7 */
    "SGR",      /* 8 */
    "AW1",      /* 9 */
    "AW2",      /* 10 */
    "UNK",      /* 11 */
    "N_R",      /* 12 */
    "AR1",      /* 13 */
    "AR2",      /* 14 */
    "UNK",      /* 15 */
};

#define CARD_STA_ERR_MASK ((1<<31)|(1<<30)|(1<<29)|(1<<28)|(1<<27)|(1<<26)|(1<<24)|(1<<23)|(1<<22)|(1<<21)|(1<<20)|(1<<19)|(1<<18)|(1<<17)|(1<<16)|(1<<15)|(1<<13)|(1<<7))

#define CLK_PIN_IDX     0
#define CMD_PIN_IDX     1
#define RST_PIN_IDX     2   // for eMMC
#define WP_PIN_IDX      RST_PIN_IDX //for CARD-Reader
#define CD_PIN_IDX      3
#define D0_PIN_IDX      4
#define D1_PIN_IDX      5
#define D2_PIN_IDX      6
#define D3_PIN_IDX      7
#define D4_PIN_IDX      8
#define D5_PIN_IDX      9
#define D6_PIN_IDX      10
#define D7_PIN_IDX      11

#define MAX_TUNING_STEP         (32)    //skip 28~31 step
#define MAX_DS_TUNE             (32)    //Only 0~27 are available even hardware has 32 step

/* One git use */
#if defined(CONFIG_ARCH_RTK6748) || defined(CONFIG_ARCH_RTK2885M)
#include "rtk_mmc_6748.h" /* M6 and M7 */
#elif defined(CONFIG_ARCH_RTK6702)
#include "rtk_mmc_6702.h" /* MK2 */
#elif defined(CONFIG_ARCH_RTK2851A)
#include "rtk_mmc_2851a.h" /* mac7 */
#elif defined(CONFIG_ARCH_RTK2851F)
#include "rtk_mmc_2851f.h" /* mac8 */
#elif defined(CONFIG_ARCH_RTK2885P)
#include "rtk_mmc_2885p.h" /* mer8 */
#endif

/*
Pin mux info :
    xxxx_pinmux_main.doc
    xxxx_pinmux_pmm.doc

GPIO info :
    xxxx_DesignSpec_MISC_GPIO.doc
    xxxx_DesignSpec_ISO_MISC_GPIO.doc
*/


#endif
