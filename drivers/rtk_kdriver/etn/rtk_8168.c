/*
 * rtk_8168.c: RealTek 8168 ethernet driver. 
 *
 * Copyright (c) 2002 ShuChen <shuchen@realtek.com.tw>
 * Copyright (c) 2003 - 2007 Francois Romieu <romieu@fr.zoreil.com>
 * Copyright (c) a lot of people too. Please respect their work.
 *
 * See MAINTAINERS file for support contact information. 
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/crc32.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/prefetch.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <mach/platform.h>
#include <rtk_kdriver/rtk_crt.h>
#include <linux/dma-direct.h>
#include <rbus/emcu_reg.h>
#if IS_ENABLED(CONFIG_RTK_KDRV_EMCU)
#include <rtk_kdv_emcu.h>
#endif
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
#include <linux/sched/signal.h>
#endif

#if IS_ENABLED(CONFIG_RTK_KDRV_SB2)
#include <rtk_sb2_callback.h>
#endif

#include <rtd_log/rtd_module_log.h> 

#include <asm/io.h>
#include <asm/irq.h>
#include <rtk_kdriver/io.h>

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/version.h>
#include <rbus/plletn_reg.h>
#include <rbus/stb_reg.h>
#include <rtk_kdriver/rtk_otp_region_api.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include "rtk_8168_def.h"
#include "rtk_8168_mdns_offload.h"
#include "rtk_8168_inc.h"
#include <rbus/sb2_reg.h>

#define REG_WOW_GMAC_CLK			PLLETN_WOW_CLK_reg

#define REG_SHARE_CLOCK			STB_ST_SRST1_reg
#define SHARE_REG_CLOCK_BIT		10
#define SHARE_REG_WOL_EVENT		EMCU_ST_IP_reg
#define SHARE_REG_WOL_EVENT_BIT	9

#define TAG_NAME		"ETH"
#define RTL8168_VERSION "2.3LK-NAPI"
#define MODULENAME "r8168"
#define PFX MODULENAME ": "

/* Maximum number of multicast addresses to filter (vs. Rx-all-multicast).
	The RTL chips use a 64 element hash table based on the Ethernet CRC. */
static const int multicast_filter_limit = 32;
static int rx_buf_sz = 16383;
static int use_dac;
static struct {
	u32 msg_enable;
} debug = { -1 };



MODULE_AUTHOR("Realtek and the Linux r8168 crew <netdev@vger.kernel.org>");
MODULE_DESCRIPTION("RealTek RTL-8168 Gigabit Ethernet driver");
module_param(use_dac, int, 0);
MODULE_PARM_DESC(use_dac, "Enable PCI DAC. Unsafe on 32 bit PCI slot.");
module_param_named(debug, debug.msg_enable, int, 0);
MODULE_PARM_DESC(debug, "Debug verbosity level (0=none, ..., 16=all)");
MODULE_LICENSE("GPL");
MODULE_VERSION(RTL8168_VERSION);

#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
#define NUM_WOL_PATTERN	2
#define WOL_MASK_SIZE		16
#define PACKET_PATTERN_SIZE		136
u8 wol_mask[NUM_WOL_PATTERN][WOL_MASK_SIZE]={
	//airplay magic packet
	{
		0x3f,0x70,0x80,0x00,
		0x00,0xfc,0xff,0xff,
		0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0x3f,
	},
	//case 1 :: iphone pattern
	{
	0x00,0x70,0x80,0x00,	
	0x3c,0x30,0x00,0x00,	
	0x00,0x00,0x00,0x00,	
	0x00,0x00,0x00,0x00,
	},
		
};

u16 wol_pattern[NUM_WOL_PATTERN] = {
			0xffff,//need to runtime count
			0x7284,
};

u16 packet_start_code[NUM_WOL_PATTERN] = 
{
	0x0000,
	0x0000
};

u8 mask_selection_code[NUM_WOL_PATTERN] = 
{
	0x00,
	0x00
};

#else
#define NUM_WOL_PATTERN	2
#define WOL_MASK_SIZE		16
u8 wol_mask[NUM_WOL_PATTERN][WOL_MASK_SIZE]={
	//airplay magic packet
	{
		0x3f,0x70,0x80,0x00,
		0x00,0xfc,0xff,0xff,
		0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0x3f,
	},
	//case 1 :: iphone pattern
	{
	0x00,0x70,0x80,0x00,	
	0x3c,0x30,0x00,0x00,	
	0x00,0x00,0x00,0x00,	
	0x00,0x00,0x00,0x00,
	},
};

u16 wol_pattern[NUM_WOL_PATTERN] = {
			0xffff,//need to runtime count
			0x7284,
};
#endif

#define AIRPLAY_MAIGIC_PACKET { \
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x01, 0x02, 0x03, 0x04, \
	0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, \
	0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, \
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, \
	0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, \
}

void rtk8168_get_pkt_crc16(unsigned char *pkt, unsigned int pkt_len, 
			unsigned char *mask_array,	unsigned short *crc_value);

u16 rtl8168_get_airplay_magic_packet(struct rtl8168_private *tp, u8 *mask_array)
{
	u16 crc = 0xffff;
	struct net_device *ndev = tp->dev;
	u8 packet[128] = AIRPLAY_MAIGIC_PACKET;
	int i	= 0;
	for(i = 0; i < 11; i++) {
		u8	*buf = packet + 60 + i * 6;
		memcpy(buf, ndev->dev_addr, 6);
	}
	rtk8168_get_pkt_crc16(packet, 128,mask_array, &crc);
	return crc;
}



static int eth_init = 1;
static u32 tapbin_val = 0;
static u32 snr_debug = RTL8168_SNR_DISABLE;
static rtl_avg_link avg_link;
static struct net_device *ndev;
static u32 g_is_new_wol_flow = 0;
static u32 g_vcpu_mdns_offloading_wol_flow = 0;
static u32 g_hw_mdns_offloading_wol_flow = 0;
static unsigned int g_resume_first_check_delay = 0;

#ifdef ENABLE_WOL
static void rtl_speed_down(struct rtl8168_private *tp);
#endif
#ifdef ENABLE_EEEP
static void rtl8168_mac_eeep_patch(struct rtl8168_private *tp);
#endif
#ifdef CONFIG_PM
struct platform_device *network_devs;
int rtl8168_suspend(struct platform_device *dev, pm_message_t state);
int rtl8168_resume(struct platform_device *dev);
#endif

#ifdef ENABLE_EEEP
static void r8168soc_eee_init(struct rtl8168_private *tp, bool enable);
#endif
static void rtl8168_phy_int_disable(struct rtl8168_private *tp);
static void rtl8168_phy_int_enable(struct rtl8168_private *tp);
static void rtl8168_phy_reset(struct net_device *dev, struct rtl8168_private *tp);

#if IS_ENABLED(CONFIG_RTK_KDRV_SB2)
static void rtl8168_rbus_timeout_debug(void);
SB2_DEBUG_INFO_DECLARE(eth, rtl8168_rbus_timeout_debug);

static void rtl8168_rbus_timeout_debug(void)
{
	sb2_timeout_info timeout_info;
	u32 wol_mode = 1;

	if(!isSb2Timeout("ETN")) {
		eth_err("NOT ETN module!\n");
		return;
	}
	
	get_sb2_timeout_info(&timeout_info);
	eth_err("%s\n", __func__);
	eth_err("Clock(%08x): %08X, reset(%08x): %08X, ctrl(%08x): %08X\n", 
		STB_ST_CLKEN0_reg, rtd_inl(STB_ST_CLKEN0_reg), 
		STB_ST_SRST0_reg, rtd_inl(STB_ST_SRST0_reg),
		STB_ST_CTRL0_reg, rtd_inl(STB_ST_CTRL0_reg));

	CRT_CLK_OnOff(ETH, CLK_ON, &wol_mode);
	
	eth_err("PLLETN CLK(%08x): %08X, RST(%08x): %08X, INFO(%08x): %08X\n", 
		PLLETN_GETN_CLKEN_reg,rtd_inl(PLLETN_GETN_CLKEN_reg), 
		PLLETN_GETN_RST_reg, rtd_inl(PLLETN_GETN_RST_reg), 
		PLLETN_ETN_phy_info_reg_reg, rtd_inl(PLLETN_ETN_phy_info_reg_reg));

	return;
}
#endif

static void rtl_lock_work(struct rtl8168_private *tp)
{
	mutex_lock(&tp->wk.mutex);
}

static void rtl_unlock_work(struct rtl8168_private *tp)
{
	mutex_unlock(&tp->wk.mutex);
}

struct rtl_cond {
	bool (*check)(struct rtl8168_private *tp);
	const char *msg;
};

static void rtl_udelay(unsigned int d)
{
	udelay(d);
}

static bool rtl_loop_wait(struct rtl8168_private *tp, const struct rtl_cond *c,
			void (*delay)(unsigned int),
			unsigned int d, int n, bool high)
{
	int i;

	for (i = 0; i < n; i++) {
		delay(d);
		if (c->check(tp) == high)
			return true;
	}
	eth_err("%s == %d (loop: %d, delay: %d).\n",
			c->msg, !high, n, d);
	return false;
}

static bool rtl_udelay_loop_wait_high(struct rtl8168_private *tp,
		const struct rtl_cond *c,
		unsigned int d, int n)
{
	return rtl_loop_wait(tp, c, rtl_udelay, d, n, true);
}

static bool rtl_udelay_loop_wait_low(struct rtl8168_private *tp,
		const struct rtl_cond *c,
		unsigned int d, int n)
{
	return rtl_loop_wait(tp, c, rtl_udelay, d, n, false);
}

#if 0
static bool rtl_msleep_loop_wait_high(struct rtl8168_private *tp,
		const struct rtl_cond *c,
		unsigned int d, int n)
{
	return rtl_loop_wait(tp, c, msleep, d, n, true);
}


static bool rtl_msleep_loop_wait_low(struct rtl8168_private *tp,
		const struct rtl_cond *c,
		unsigned int d, int n)
{
	return rtl_loop_wait(tp, c, msleep, d, n, false);
}
#endif


#define DECLARE_RTL_COND(name)				\
	static bool name ## _check(struct rtl8168_private *);	\
\
static const struct rtl_cond name = {			\
	.check	= name ## _check,			\
	.msg	= #name					\
};							\
\
static bool name ## _check(struct rtl8168_private *tp)

DECLARE_RTL_COND(rtl_eriar_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R32(ERIAR) & ERIAR_FLAG;
}

static bool rtl_ocp_reg_failure(struct rtl8168_private *tp, u32 reg)
{
	if (reg & 0xffff0001) {
		eth_err("Invalid ocp reg %x!\n", reg);
		return true;
	}
	return false;
}
#ifdef CONFIG_ETN_DIABLE_RBUS_TIMEOUT
void r8168_mac_ocp_write(struct rtl8168_private *tp, u32 reg, u32 data)
{
	u32 ioaddr = tp->base_addr;
	u32 sb2_reg;

	if (rtl_ocp_reg_failure(tp, reg))
		return;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	RTL_W32(OCPDR, OCPAR_FLAG | (reg << 15) | data);
	
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
}

u16 r8168_mac_ocp_read(struct rtl8168_private *tp, u32 reg)
{
	u32 ioaddr = tp->base_addr;
	u16 val = 0x0;
	u32 sb2_reg;

	if (rtl_ocp_reg_failure(tp, reg))
		return 0;

	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	RTL_W32(OCPDR, reg << 15);

	val = RTL_R32(OCPDR);
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
	return val;
}
#else
void r8168_mac_ocp_write(struct rtl8168_private *tp, u32 reg, u32 data)
{
	u32 ioaddr = tp->base_addr;

	if (rtl_ocp_reg_failure(tp, reg))
		return;

	RTL_W32(OCPDR, OCPAR_FLAG | (reg << 15) | data);

}

u16 r8168_mac_ocp_read(struct rtl8168_private *tp, u32 reg)
{
	u32 ioaddr = tp->base_addr;

	if (rtl_ocp_reg_failure(tp, reg))
		return 0;

	RTL_W32(OCPDR, reg << 15);

	return RTL_R32(OCPDR);
}
#endif

#if 0
#define MDIO_LOCK
#define MDIO_UNLOCK
#else
#define MDIO_WAIT_TIMEOUT	100
#define MDIO_LOCK							\
do {									\
	u32 wait_cnt = 0;						\
	u32 log_de4e = 0;						\
									\
	/* disable EEE IMR */						\
	r8168_mac_ocp_write(tp, 0xE044,					\
		(r8168_mac_ocp_read(tp, 0xE044) &				\
		~(BIT(3) | BIT(2) | BIT(1) | BIT(0))));			\
	/* disable timer 2 */						\
	r8168_mac_ocp_write(tp, 0xE404,					\
		(r8168_mac_ocp_read(tp, 0xE404) | BIT(9)));			\
	/* wait MDIO channel is free */					\
	log_de4e = BIT(0) & r8168_mac_ocp_read(tp, 0xDE4E);			\
	log_de4e = (log_de4e << 1) |					\
		(BIT(0) & r8168_mac_ocp_read(tp, 0xDE4E));			\
	/* check if 0 for continuous 2 times */				\
	while (0 != (((0x1 << 2) - 1) & log_de4e)) {			\
		wait_cnt++;						\
		udelay(1);						\
		log_de4e = (log_de4e << 1) | (BIT(0) &			\
			r8168_mac_ocp_read(tp, 0xDE4E));			\
		if (wait_cnt > MDIO_WAIT_TIMEOUT)			\
			break;						\
	}								\
	/* enter driver mode */						\
	r8168_mac_ocp_write(tp, 0xDE42,					\
		r8168_mac_ocp_read(tp, 0xDE42) | BIT(0));			\
	if (wait_cnt > MDIO_WAIT_TIMEOUT)				\
		pr_err("%s:%d: MDIO lock failed\n", __func__, __LINE__);\
} while (0)

#define MDIO_UNLOCK							\
do {									\
	/* exit driver mode */						\
	r8168_mac_ocp_write(tp, 0xDE42,					\
			r8168_mac_ocp_read(tp, 0xDE42) & ~BIT(0));		\
	/* enable timer 2 */						\
	r8168_mac_ocp_write(tp, 0xE404,					\
		(r8168_mac_ocp_read(tp, 0xE404) & ~BIT(9)));			\
	/* enable EEE IMR */						\
	r8168_mac_ocp_write(tp, 0xE044,					\
		(r8168_mac_ocp_read(tp, 0xE044) |				\
		BIT(3) | BIT(2) | BIT(1) | BIT(0)));			\
} while (0)
#endif


DECLARE_RTL_COND(rtl_int_phyar_cond)
{
	return r8168_mac_ocp_read(tp, 0xDE0A) & BIT(14);
}
DECLARE_RTL_COND(rtl_phyar_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R32(PHYAR) & 0x80000000;
}

static void __int_set_phy_addr(struct rtl8168_private *tp, int phy_addr)
{
#if 0 //TODO
	u32 tmp;

	tmp = readl(tp->mmio_clkaddr + ISO_PWRCUT_ETN);
	tmp &= ~(BIT(20) | BIT(19) | BIT(18) | BIT(17) | BIT(16));
	tmp |= (phy_addr << 16);
	writel(tmp, tp->mmio_clkaddr + ISO_PWRCUT_ETN);
#endif
}


static inline int __int_mdio_read(struct rtl8168_private *tp, int reg)
{
	u32 ioaddr = tp->base_addr;
	int value;

	/* read reg */
	RTL_W32(PHYAR, 0x0 | (reg & 0x1f) << 16);

	value = rtl_udelay_loop_wait_high(tp, &rtl_phyar_cond, 25, 10000) ?
		RTL_R32(PHYAR) & 0xffff : ~0;

	/* According to hardware specs a 20us delay is required after read
	* complete indication, but before sending next command.
	*/
	udelay(20);

	return value;
}

static inline void __int_mdio_write(struct rtl8168_private *tp, int reg, int value)
{
	u32 ioaddr = tp->base_addr;

	/* write reg */
	RTL_W32(PHYAR, 0x80000000 | (reg & 0x1f) << 16 | (value & 0xffff));

	rtl_udelay_loop_wait_low(tp, &rtl_phyar_cond, 25, 10000);
	/* According to hardware specs a 20us delay is required after write
	* complete indication, but before sending next command.
	*/
	udelay(20);
}
#ifdef CONFIG_ETN_DIABLE_RBUS_TIMEOUT
static int int_mdio_read(struct rtl8168_private *tp, int page, int reg)
{
	int value;
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	//MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__int_mdio_write(tp, 0x1f, page);

	/* read reg */
	value = __int_mdio_read(tp, reg);
	
	//MDIO_UNLOCK;
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
	return value;
}

static void int_mdio_write(struct rtl8168_private *tp, int page, int reg, int value)
{
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	//MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__int_mdio_write(tp, 0x1f, page);

	/* write reg */
	__int_mdio_write(tp, reg, value);

	//MDIO_UNLOCK;
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
		
}
#else
static int int_mdio_read(struct rtl8168_private *tp, int page, int reg)
{
	int value;

	//MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__int_mdio_write(tp, 0x1f, page);

	/* read reg */
	value = __int_mdio_read(tp, reg);

	//MDIO_UNLOCK;
	return value;
}

static void int_mdio_write(struct rtl8168_private *tp, int page, int reg, int value)
{

	//MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__int_mdio_write(tp, 0x1f, page);

	/* write reg */
	__int_mdio_write(tp, reg, value);

	//MDIO_UNLOCK;
}
#endif

DECLARE_RTL_COND(rtl_ext_phyar_cond)
{
	return r8168_mac_ocp_read(tp, 0xDE2A) & BIT(14);
}
DECLARE_RTL_COND(rtl_ephyar_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R32(EPHYAR) & EPHYAR_FLAG;
}

static void __ext_set_phy_addr(struct rtl8168_private *tp, int phy_addr)
{
#if 0	//TODO
	u32 tmp;

	tmp = readl(tp->mmio_clkaddr + ISO_PWRCUT_ETN);
	tmp &= ~(BIT(25) | BIT(24) | BIT(23) | BIT(22) | BIT(21));
	tmp |= (phy_addr << 21);
	writel(tmp, tp->mmio_clkaddr + ISO_PWRCUT_ETN);
#endif
}
static inline int __ext_mdio_read(struct rtl8168_private *tp, int reg)
{
	int value;
	u32 ioaddr = tp->base_addr;

	/* read reg */
	RTL_W32(EPHYAR, (reg & EPHYAR_REG_MASK) << EPHYAR_REG_SHIFT);

	value = rtl_udelay_loop_wait_high(tp, &rtl_ephyar_cond, 10, 100) ?
		RTL_R32(EPHYAR) & EPHYAR_DATA_MASK : ~0;

	return value;
}

static inline void __ext_mdio_write(struct rtl8168_private *tp, int reg, int value)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(EPHYAR, EPHYAR_WRITE_CMD | (value & EPHYAR_DATA_MASK) |
			(reg & EPHYAR_REG_MASK) << EPHYAR_REG_SHIFT);

	rtl_udelay_loop_wait_low(tp, &rtl_ephyar_cond, 10, 100);

	udelay(10);
}
#ifdef CONFIG_ETN_DIABLE_RBUS_TIMEOUT
static int ext_mdio_read(struct rtl8168_private *tp, int page, int reg)
{
	int value;
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__ext_mdio_write(tp, 0x1f, page);

	/* read reg */
	value = __ext_mdio_read(tp, reg);

	MDIO_UNLOCK;

	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
	return value;
}


static void ext_mdio_write(struct rtl8168_private *tp, int page, int reg, int value)
{
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__ext_mdio_write(tp, 0x1f, page);

	/* write reg */
	__ext_mdio_write(tp, reg, value);

	MDIO_UNLOCK;
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
		
}
#else
static int ext_mdio_read(struct rtl8168_private *tp, int page, int reg)
{
	int value;
	MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__ext_mdio_write(tp, 0x1f, page);

	/* read reg */
	value = __ext_mdio_read(tp, reg);

	MDIO_UNLOCK;
	return value;
}


static void ext_mdio_write(struct rtl8168_private *tp, int page, int reg, int value)
{

	MDIO_LOCK;

	/* write page */
	if (page != CURRENT_MDIO_PAGE)
		__ext_mdio_write(tp, 0x1f, page);

	/* write reg */
	__ext_mdio_write(tp, reg, value);

	MDIO_UNLOCK;
}
#endif

static void rtl_init_mdio_ops(struct rtl8168_private *tp)
{
	struct mdio_ops *ops = &tp->mdio_ops;

	if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
		/* set int PHY addr */
		__int_set_phy_addr(tp, INT_PHY_ADDR);

		ops->write	= int_mdio_write;
		ops->read	= int_mdio_read;
	} else {
		/* set ext PHY addr */
		__ext_set_phy_addr(tp, EXT_PHY_ADDR);

		ops->write	= ext_mdio_write;
		ops->read	= ext_mdio_read;
	}
}

static void rtl_phy_write(struct rtl8168_private *tp, int page, int reg, u32 val)
{
	tp->mdio_ops.write(tp, page, reg, val);
}

static int rtl_phy_read(struct rtl8168_private *tp, int page, int reg)
{
	return tp->mdio_ops.read(tp, page, reg);
}

static void rtl_mdio_write(struct net_device *dev, int phy_id, int location, int val)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	rtl_phy_write(tp, 0, location, val);
}

static int rtl_mdio_read(struct net_device *dev, int phy_id, int location)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	return rtl_phy_read(tp, 0, location);
}

//for comptable wrapper file
void rtl_writephy(struct rtl8168_private *tp, int location, u32 val)
{
	if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
		__int_mdio_write(tp, location, val);
	} else {
		__ext_mdio_write(tp, location, val);
	}
}

int rtl_readphy(struct rtl8168_private *tp, int location)
{
	if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
		return __int_mdio_read(tp, location);
	} else {
		return __ext_mdio_read(tp, location);
	}
}
#if 0
static inline void rtl_patchphy(struct rtl8168_private *tp, int page,
				int reg_addr, int value)
{
	rtl_phy_write(tp, page, reg_addr,
		rtl_phy_read(tp, page, reg_addr) | value);
}

static inline void rtl_w1w0_phy(struct rtl8168_private *tp, int page,
				int reg_addr, int p, int m)
{
	rtl_phy_write(tp, page, reg_addr,
		(rtl_phy_read(tp, page, reg_addr) | p) & ~m);
}
#endif

void int_mmd_write(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr, u32 value)
{
	MDIO_LOCK;

	/* set page 0 */
	__int_mdio_write(tp, 0x1f, 0);
	/* address mode */
	__int_mdio_write(tp, 13, (0x1f & dev_addr));
	/* write the desired address */
	__int_mdio_write(tp, 14, reg_addr);
	/* data mode, no post increment */
	__int_mdio_write(tp, 13, ((0x1f & dev_addr) | (0x1 << 14)));
	/* write the content of the selected register */
	__int_mdio_write(tp, 14, value);

	MDIO_UNLOCK;

}

u32 int_mmd_read(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr)
{
	u32 value;

	MDIO_LOCK;

	/* set page 0 */
	__int_mdio_write(tp, 0x1f, 0);
	/* address mode */
	__int_mdio_write(tp, 13, (0x1f & dev_addr));
	/* write the desired address */
	__int_mdio_write(tp, 14, reg_addr);
	/* data mode, no post increment */
	__int_mdio_write(tp, 13, ((0x1f & dev_addr) | (0x1 << 14)));
	/* read the content of the selected register */
	value = __int_mdio_read(tp, 14);

	MDIO_UNLOCK;


	return value;
}

void ext_mmd_write(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr, u32 value)
{

	MDIO_LOCK;

	/* set page 0 */
	__ext_mdio_write(tp, 0x1f, 0);
	/* address mode */
	__ext_mdio_write(tp, 13, (0x1f & dev_addr));
	/* write the desired address */
	__ext_mdio_write(tp, 14, reg_addr);
	/* data mode, no post increment */
	__ext_mdio_write(tp, 13, ((0x1f & dev_addr) | (0x1 << 14)));
	/* write the content of the selected register */
	__ext_mdio_write(tp, 14, value);

	MDIO_UNLOCK;

}

u32 ext_mmd_read(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr)
{
	u32 value;


	MDIO_LOCK;

	/* set page 0 */
	__ext_mdio_write(tp, 0x1f, 0);
	/* address mode */
	__ext_mdio_write(tp, 13, (0x1f & dev_addr));
	/* write the desired address */
	__ext_mdio_write(tp, 14, reg_addr);
	/* data mode, no post increment */
	__ext_mdio_write(tp, 13, ((0x1f & dev_addr) | (0x1 << 14)));
	/* read the content of the selected register */
	value = __ext_mdio_read(tp, 14);

	MDIO_UNLOCK;

	return value;
}

static void rtl_init_mmd_ops(struct rtl8168_private *tp)
{
	struct mmd_ops *ops = &tp->mmd_ops;

	if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
		ops->write	= int_mmd_write;
		ops->read	= int_mmd_read;
	} else {
		ops->write	= ext_mmd_write;
		ops->read	= ext_mmd_read;
	}
}

void rtl_mmd_write(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr, u32 value)
{
	tp->mmd_ops.write(tp, dev_addr, reg_addr, value);
}

u32 rtl_mmd_read(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr)
{
	return tp->mmd_ops.read(tp, dev_addr, reg_addr);
}
void rtl_eri_write(struct rtl8168_private *tp, int addr, u32 mask,
		u32 val, int type)
{
	u32 ioaddr = tp->base_addr;

	BUG_ON((addr & 3) || (mask == 0));
	RTL_W32(ERIDR, val);
	RTL_W32(ERIAR, ERIAR_WRITE_CMD | type | mask | addr);

	rtl_udelay_loop_wait_low(tp, &rtl_eriar_cond, 100, 100);
}


u32 rtl_eri_read(struct rtl8168_private *tp, int addr, int type)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(ERIAR, ERIAR_READ_CMD | type | ERIAR_MASK_1111 | addr);

	return rtl_udelay_loop_wait_high(tp, &rtl_eriar_cond, 100, 100) ?
		RTL_R32(ERIDR) : ~0;
}

void rtl_w1w0_eri(struct rtl8168_private *tp, int addr, u32 mask, u32 p,
		u32 m, int type)
{
	u32 val;

	val = rtl_eri_read(tp, addr, type);
	rtl_eri_write(tp, addr, mask, (val & ~m) | p, type);
}
#ifdef CONFIG_ETN_DIABLE_RBUS_TIMEOUT
static u16 rtl_get_events(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	u16 status = 0;
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	status = RTL_R16(IntrStatus);
	rmb();
	
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
	return status;
}

static void rtl_ack_events(struct rtl8168_private *tp, u16 bits)
{
	u32 ioaddr = tp->base_addr;
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	
	RTL_W16(IntrStatus, bits);
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
}

static void rtl_irq_disable(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout

	RTL_W16(IntrMask, 0);
	
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
}

static void rtl_irq_enable(struct rtl8168_private *tp, u16 bits)
{
	u32 ioaddr = tp->base_addr;
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout

	RTL_W16(IntrMask, bits);
	
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
}
#else
static u16 rtl_get_events(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	u16 status = RTL_R16(IntrStatus);
	rmb();

	return status;
}

static void rtl_ack_events(struct rtl8168_private *tp, u16 bits)
{
	u32 ioaddr = tp->base_addr;

	RTL_W16(IntrStatus, bits);
}

static void rtl_irq_disable(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	RTL_W16(IntrMask, 0);
}

static void rtl_irq_enable(struct rtl8168_private *tp, u16 bits)
{
	u32 ioaddr = tp->base_addr;

	RTL_W16(IntrMask, bits);
}
#endif
#define RTL_EVENT_NAPI_RX	(RxOK | RxErr)
#define RTL_EVENT_NAPI_TX	(TxOK | TxErr)
#define RTL_EVENT_NAPI		(RTL_EVENT_NAPI_RX | RTL_EVENT_NAPI_TX)

static void rtl_irq_enable_all(struct rtl8168_private *tp)
{
	rtl_irq_enable(tp, RTL_EVENT_NAPI | tp->event_slow);
}

static void rtl8168_irq_mask_and_ack(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	rtl_irq_disable(tp);
	rtl_ack_events(tp, RTL_EVENT_NAPI | tp->event_slow);
	RTL_R8(ChipCmd);
}

static unsigned int rtl8168_xmii_reset_pending(struct rtl8168_private *tp)
{
	int ret;

	ret = rtl_phy_read(tp, 0, MII_BMCR);
	return ret & BMCR_RESET;
}
#ifdef CONFIG_ETN_DIABLE_RBUS_TIMEOUT
static unsigned int rtl8168_xmii_link_ok(u32 ioaddr)
{
	unsigned int val = 0;
	u32 sb2_reg;
	sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
	val = (RTL_R8(PHYstatus) & LinkStatus);
	rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting
	return val;
}
#else
static unsigned int rtl8168_xmii_link_ok(u32 ioaddr)
{
	return RTL_R8(PHYstatus) & LinkStatus;
}
#endif
static void rtl8168_xmii_reset_enable(struct rtl8168_private *tp)
{
	unsigned int val;

	val = rtl_phy_read(tp, 0, MII_BMCR) | BMCR_RESET;
	rtl_phy_write(tp, 0, MII_BMCR, val & 0xffff);
}

/*
 * switch phy page
 */
void rtl8168_xmii_set_page(struct rtl8168_private *tp, u16 page)
{
	int phy_register = 0x1f;
	u32 phy_write_data = 0x0000;

	phy_write_data |= page;

	rtl_writephy(tp, phy_register, phy_write_data);
}

/*
 * enable ALDPS mode automatically
 * 1. switch page 0x0a43
 * 2. set reg 24(0x18) bit[12]=1,bit[2]=1,bit[1]=1
 * 3. disconnect cable
 * 4. check reg 26(0x1a) bit[14]=1 if phy is in ALDPS mode
 */
static void rtl8168_xmii_set_aldps(struct rtl8168_private *tp, u32 enable)
{
	if(tp->output_mode == OUTPUT_EMBEDDED_PHY) {
		int phy_register = 0x00;
		u32 phy_write_data = 0x0000;
		int phy_read_data = 0x0000;

		phy_write_data = 0x0000;
		phy_register = 0x18;
		phy_read_data = rtl_phy_read(tp, 0x0a43, phy_register);
		if(enable) {
			phy_write_data = phy_read_data | (0x1000 | 0x0004 | 0x0002);
		} else {
			phy_write_data = 0x2198;
		}
		rtl_phy_write(tp, 0x0a43, phy_register, phy_write_data);
	} else {
		if(enable)
			rtl_phy_write(tp, 0x0a43, 24,
				rtl_phy_read(tp, 0x0a43, 24) | BIT(2));
		else
			rtl_phy_write(tp, 0x0a43, 24,
				rtl_phy_read(tp, 0x0a43, 24) & (~BIT(2)));
	}
}

static bool rtl8168_xmii_is_aldps_mode(struct rtl8168_private *tp)
{
	int phy_register = 0x00;
	int phy_read_data = 0x0000;

	phy_register = 0x1a;
	phy_read_data = rtl_phy_read(tp, 0x0a43, phy_register);

	if(phy_read_data & (1 << 14))
		return true;
	else
		return false;
}

/*
 * enhance GETN performance
 * 1. Set tapbin R
 * 2. Adjust PGA gain manually
 */
static void rtl8168_xmii_enhance_performance(struct rtl8168_private *tp, u32 enable)
{
	int phy_register = 0x00;
	u32 phy_write_data = 0x0000;
	u32 phy_read_data = 0x0000;

	/* Set tapbin R */
	if(tapbin_val) {
		/* set	bit[4] force R enable, set r value bit[3:0]=1 */
		phy_write_data = tapbin_val | BIT(4);
		phy_register = 0x14;
		rtl_phy_write(tp, 0x0bc0, phy_register, phy_write_data);
		/* read reg20 bit[15:0] */
		phy_read_data = rtl_phy_read(tp, 0x0bc0, phy_register);
		eth_info("Tapbin val = 0x%04x\n", phy_read_data);
		if (phy_read_data != phy_write_data) {
			eth_warning("==> %s:%s Set tapbin R Fail (0x%04x)\n", tp->dev->name, __func__, phy_read_data);
		}
	}
}

static void rtl_link_chg_patch(struct rtl8168_private *tp)
{
	struct net_device *dev = tp->dev;

	if (!netif_running(dev))
		return;
}

static void rtl_read_snr_val(struct rtl8168_private *tp)
{
	u32 phy_read_data = 0x0000;
	int i = 0;

	phy_read_data = rtl_phy_read(tp, 0x0a43, 0x1a);
	eth_err("0x%4x, %s\n", phy_read_data, (phy_read_data & BIT(1))? "MDI":"MDIX");

	for(i = 0; i < 3; i++) {
		phy_read_data = rtl_phy_read(tp, 0x0aa0, 0x10);
		eth_err("MDI SNR = 0x%4x, %s\n", phy_read_data, (phy_read_data == 0x8000)? "No value":(phy_read_data > 0x50 && phy_read_data < 0x414)? "in range":"out of range");
	}

}

static void rtl_get_phy_status(struct rtl8168_private *tp)
{
	rtl_phy_status *phyStatus = &tp->phy_status;

	/* page 0xa40, reg 0 */
	phyStatus->phy_set = rtl_phy_read(tp, 0x0a40, 0x00);

	/* phy link status, page 0x40, reg 1 */
	if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
		phyStatus->link_status = rtl_phy_read(tp, 0x0a40, 0x01);
		phyStatus->link_status = rtl_phy_read(tp, 0x0a40, 0x01);
	} else {
		phyStatus->link_status = r8168_mac_ocp_read(tp, 0xEA3C);
		phyStatus->link_status = r8168_mac_ocp_read(tp, 0xEA3C);
		phyStatus->link_status = (phyStatus->link_status >> 1);
		
	}
	
	/* 10/100 adv ability, page 0x40, reg 4 */
	phyStatus->adv_ability = rtl_phy_read(tp, 0x0a40, 0x04);

	/* phy status, page 0xa42, reg16 */
	phyStatus->phy_status = rtl_phy_read(tp, 0x0a42, 0x10);

	/* 10/100 adv ability, page 0x40, reg 4 */
	phyStatus->adv_ability = rtl_phy_read(tp, 0x0a40, 0x04);

	/* phy status, page 0xa42, reg16 */
	phyStatus->phy_status = rtl_phy_read(tp, 0x0a42, 0x10);

	/* link speed, duplex */
	phyStatus->link_speed = rtl_phy_read(tp, 0x0a43, 0x1a);
	phyStatus->link_speed = rtl_phy_read(tp, 0x0a43, 0x1a);

	/* lock main, page 0xa46, reg 21 */
	phyStatus->lock_main = rtl_phy_read(tp, 0x0a46, 0x15);

	/* pcs_state, page 0xa60, reg16 */
	phyStatus->pcs_state = rtl_phy_read(tp, 0x0a60, 0x10);
}

static void rtl_read_phy_status(struct rtl8168_private *tp)
{
	rtl_phy_status *phyStatus = &tp->phy_status;
	u32 ioaddr = tp->base_addr;

	rtl_get_phy_status(tp);

	eth_info("MAC link status = 0x%08x, link %s\n",
			RTL_R8(PHYstatus), tp->link_ok(ioaddr) ? "up":"down");
	eth_info("Phy link status = 0x%4x, link %s, %uMbps, %s-duplex\n",
			phyStatus->link_status, phyStatus->link_status & BIT(2) ? "up":"down",
			phyStatus->link_speed & BIT(5) ? 1000 : phyStatus->link_speed & BIT(4) ? 100 : 10,
			phyStatus->link_speed & BIT(3) ? "full": "half");

	eth_info("Phy settings = 0x%4x, adv_ability = 0x%4x, giga_adv_ability = 0x%04x\n",
			phyStatus->phy_set, phyStatus->adv_ability, phyStatus->giga_adv_ability);

	eth_info("Phy status = 0x%4x, lock_main_uc = 0x%4x, %s, pcs_state = 0x%4x\n" ,
			phyStatus->phy_status, phyStatus->lock_main, phyStatus->lock_main & BIT(1) ? "abnormal":"normal",
			phyStatus->pcs_state);

	/* CN don't recommend to clear lock_main_uc */
	if((tp->link_recovery_enable) && (phyStatus->lock_main & BIT(1))) {
		rtl_phy_write(tp, 0x0a46, 0x15, phyStatus->lock_main & ~BIT(1));
		phyStatus->lock_main = rtl_phy_read(tp, 0x0a46, 0x15);
		eth_info("recovery lock_main_uc = 0x%04x\n", phyStatus->lock_main);
		tp->link_recovery_count++;
	}
}

static void __rtl8168_check_link_status(struct net_device *dev,
		struct rtl8168_private *tp,
		u32 ioaddr, bool pm)
{
	u32 i = 0;
	u32 current_link_status = RTL8168_LINK_UNKNOWN;
	u32 current_link_count = 0;
	u32 phy_read_data = 0;

	//eth_debug("==> %s:status check start %d \n",tp->dev->name, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

	if(g_resume_first_check_delay>0){
		//eth_debug("delay %d ms\n",g_resume_first_check_delay);
		msleep(g_resume_first_check_delay);
		g_resume_first_check_delay=0;
	}

#if NWAY_ENABLE
	for (i = 0; i < 5; i++) {
		if (tp->link_ok(ioaddr)) {
			current_link_count++;
		} else {
			if (current_link_count != 0) {
				current_link_count--;
			}
		}
		if(current_link_count >= 3) {
			current_link_status = RTL8168_LINK_UP;
			break;
		} else {
			current_link_status = RTL8168_LINK_DOWN;
		}
		msleep(50);
	}
#else
	if(tp->autoneg == AUTONEG_ENABLE) {
		for (i = 0; i < 5; i++) {
			if (tp->link_ok(ioaddr)) {
				current_link_count++;
			} else {
				if (current_link_count != 0) {
					current_link_count--;
				}
			}
			if(current_link_count >= 3) {
				current_link_status = RTL8168_LINK_UP;
				break;
			} else {
				current_link_status = RTL8168_LINK_DOWN;
			}
			msleep(50);
		}
	} else {
		if (tp->link_ok(ioaddr)) {
			current_link_status = RTL8168_LINK_UP;
		} else {
			current_link_status = RTL8168_LINK_DOWN;
		}
	}
#endif

	if (current_link_status == tp->link_status) {
		if(current_link_status == RTL8168_LINK_UP)
			rtl8168_schedule_mdns_packet_upload_work(tp->dev);
		//eth_debug("==> %s:%s not change \n",tp->dev->name,__FUNCTION__);
		return;
	}

	//eth_debug("==>	%s:%s done %d \n",tp->dev->name,__FUNCTION__, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

	if (current_link_status == RTL8168_LINK_UP) {
		tp->link_status = RTL8168_LINK_UP;

		rtl_link_chg_patch(tp);
		/* This is to cancel a scheduled suspend if there's one. */
		if (pm)
			pm_request_resume(&tp->dev->dev);
		netif_carrier_on(dev);
		rtl8168_schedule_mdns_packet_upload_work(tp->dev);
		//eth_debug("==> %s:carrier on %d \n",tp->dev->name, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

		if (net_ratelimit()) {
			tp->link_up_time = jiffies_to_msecs(jiffies) - tp->link_start_time;
			rtl8168_phy_int_disable(tp);
			phy_read_data = rtl_phy_read(tp, 0x0a43, 0x1a);
			phy_read_data = rtl_phy_read(tp, 0x0a43, 0x1a);
			eth_info("[ETH] link up, %uMbps, %s-duplex\n",
					phy_read_data & BIT(5) ? 1000 : phy_read_data & BIT(4) ? 100 : 10,
					phy_read_data & BIT(3) ? "full": "half");
#ifdef CONFIG_RTK_KDRV_8168_EVENT
			kobject_uevent(&tp->dev->dev.kobj, KOBJ_LINKUP);
#endif
			if(snr_debug == RTL8168_SNR_ENABLE) {
				rtl_read_snr_val(tp);
			}

			if(avg_link.avg_linkup_enable && avg_link.retry_count < 100) {
				eth_info("Average linkup test: count=%d\n", avg_link.retry_count);
				avg_link.avg_linkup_time += tp->link_up_time;
				rtl8168_phy_reset(dev, tp);
				avg_link.retry_count++;
			}

			rtl8168_phy_int_enable(tp);
		}
	} else {
		tp->link_status = RTL8168_LINK_DOWN;
		tp->link_start_time = jiffies_to_msecs(jiffies);

		netif_carrier_off(dev);
		eth_info("[ETH] link down\n");
#ifdef CONFIG_RTK_KDRV_8168_EVENT
		kobject_uevent(&tp->dev->dev.kobj, KOBJ_LINKDOWN);
#endif
		if (pm)
			pm_schedule_suspend(&tp->dev->dev, 5000);
	}
}

static void rtl8168_check_link_status(struct net_device *dev,
		struct rtl8168_private *tp,
		u32 ioaddr)
{
	__rtl8168_check_link_status(dev, tp, ioaddr, false);
}

#define WAKE_ANY (WAKE_PHY | WAKE_MAGIC | WAKE_UCAST | WAKE_BCAST | WAKE_MCAST)

#ifdef ENABLE_WOL
static u32 __rtl8168_get_wol(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	u8 options;
	u32 wolopts = 0;

	options = RTL_R8(Config1);
	if (!(options & PMEnable))
		return 0;

	options = RTL_R8(Config3);
	if (options & LinkUp)
		wolopts |= WAKE_PHY;
	if (options & MagicPacket)
		wolopts |= WAKE_MAGIC;

	options = RTL_R8(Config5);
	if (options & UWF)
		wolopts |= WAKE_UCAST;
	if (options & BWF)
		wolopts |= WAKE_BCAST;
	if (options & MWF)
		wolopts |= WAKE_MCAST;

	return wolopts;
}
#endif

#if 1
static void rtl8168_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	rtl_lock_work(tp);

	wol->supported = WAKE_ANY;
	wol->wolopts = __rtl8168_get_wol(tp);

	rtl_unlock_work(tp);
}

static void __rtl8168_set_wol(struct rtl8168_private *tp, u32 wolopts)
{
	u32 ioaddr = tp->base_addr;
	unsigned int i;
	static const struct {
		u32 opt;
		u16 reg;
		u8	mask;
	} cfg[] = {
		{ WAKE_PHY,	Config3, LinkUp },
		{ WAKE_MAGIC, Config3, MagicPacket },
		{ WAKE_UCAST, Config5, UWF },
		{ WAKE_BCAST, Config5, BWF },
		{ WAKE_MCAST, Config5, MWF },
		{ WAKE_ANY,	Config5, LanWake }
	};
	u8 options;

	RTL_W8(Cfg9346, Cfg9346_Unlock);

	for (i = 0; i < ARRAY_SIZE(cfg); i++) {
		options = RTL_R8(cfg[i].reg) & ~cfg[i].mask;
		if (wolopts & cfg[i].opt)
			options |= cfg[i].mask;
		RTL_W8(cfg[i].reg, options);
	}

	options = RTL_R8(Config2) & ~PME_SIGNAL;
	if (wolopts)
		options |= PME_SIGNAL;
	RTL_W8(Config2, options);

	RTL_W8(Cfg9346, Cfg9346_Lock);
}

static int rtl8168_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	rtl_lock_work(tp);

	if (wol->wolopts)
		tp->features |= RTL_FEATURE_WOL;
	else
		tp->features &= ~RTL_FEATURE_WOL;
	__rtl8168_set_wol(tp, wol->wolopts);

	rtl_unlock_work(tp);

	/*device_set_wakeup_enable(&tp->pci_dev->dev, wol->wolopts); */

	return 0;
}
#endif

static void rtl8168_get_drvinfo(struct net_device *dev,
		struct ethtool_drvinfo *info)
{
	strlcpy(info->driver, MODULENAME, sizeof(info->driver));
	strlcpy(info->version, RTL8168_VERSION, sizeof(info->version));
}

static int rtl8168_get_regs_len(struct net_device *dev)
{
	return R8168_REGS_SIZE;
}

static int rtl8168_set_speed_xmii(struct net_device *dev,
		u8 autoneg, u16 speed, u8 duplex, u32 adv)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	int giga_ctrl, bmcr;
	int rc = -EINVAL;

	if (autoneg == AUTONEG_ENABLE) {
		int auto_nego;

		auto_nego = rtl_phy_read(tp, 0, MII_ADVERTISE);
		auto_nego &= ~(ADVERTISE_10HALF | ADVERTISE_10FULL |
				ADVERTISE_100HALF | ADVERTISE_100FULL);

		if (adv & ADVERTISED_10baseT_Half)
			auto_nego |= ADVERTISE_10HALF;
		if (adv & ADVERTISED_10baseT_Full)
			auto_nego |= ADVERTISE_10FULL;
		if (adv & ADVERTISED_100baseT_Half)
			auto_nego |= ADVERTISE_100HALF;
		if (adv & ADVERTISED_100baseT_Full)
			auto_nego |= ADVERTISE_100FULL;

		auto_nego |= ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM;

		giga_ctrl = rtl_phy_read(tp, 0, MII_CTRL1000);
		giga_ctrl &= ~(ADVERTISE_1000FULL | ADVERTISE_1000HALF);

#if 1
		/* The 8100e/8101e/8102e do Fast Ethernet only. */
		if (tp->mii.supports_gmii) {
			if (adv & ADVERTISED_1000baseT_Half)
				giga_ctrl |= ADVERTISE_1000HALF;
			if (adv & ADVERTISED_1000baseT_Full)
				giga_ctrl |= ADVERTISE_1000FULL;
		} else if (adv & (ADVERTISED_1000baseT_Half |
					ADVERTISED_1000baseT_Full)) {
			eth_info(
					"PHY does not support 1000Mbps\n");
			goto out;
		}
#endif
		bmcr = BMCR_ANENABLE | BMCR_ANRESTART;

		rtl_phy_write(tp, 0, MII_ADVERTISE, auto_nego);
		rtl_phy_write(tp, 0, MII_CTRL1000, giga_ctrl);
	} else {
		giga_ctrl = 0;

		if (speed == SPEED_10)
			bmcr = 0;
		else if (speed == SPEED_100)
			bmcr = BMCR_SPEED100;
		else
			goto out;

		if (duplex == DUPLEX_FULL)
			bmcr |= BMCR_FULLDPLX;
	}

	rtl_phy_write(tp, 0, MII_BMCR, bmcr);

	rc = 0;
out:
	return rc;
}

static int rtl8168_set_speed(struct net_device *dev,
		u8 autoneg, u16 speed, u8 duplex, u32 advertising)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	int ret;

	ret = tp->set_speed(dev, autoneg, speed, duplex, advertising);
	if (ret < 0)
		goto out;

#if NWAY_ENABLE
	/*
	if (netif_running(dev)) {
	mod_timer(&tp->timer, jiffies + RTL8168_PHY_TIMEOUT);
	}
	*/
#else
	if ((netif_running(dev)) && (autoneg == AUTONEG_DISABLE)) {
		//eth_debug("==> %s:%s %d \n",tp->dev->name,__FUNCTION__,rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
		mod_timer(&tp->timer, jiffies + RTL8168_PHY_TIMEOUT);
	}
#endif
out:
	return ret;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0))
static int rtl8168_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	int ret;

	del_timer_sync(&tp->timer);
	del_timer_sync(&tp->link_check_timer);

	rtl_lock_work(tp);
	rtl8168_phy_int_disable(tp);
	ret = rtl8168_set_speed(dev, cmd->autoneg, ethtool_cmd_speed(cmd),
			cmd->duplex, cmd->advertising);
	rtl8168_phy_int_enable(tp);
	rtl_unlock_work(tp);

	return ret;
}
#endif

static netdev_features_t rtl8168_fix_features(struct net_device *dev,
		netdev_features_t features)
{
	if (dev->mtu > TD_MSS_MAX)
		features &= ~NETIF_F_ALL_TSO;

	if (dev->mtu > JUMBO_1K)
		features &= ~NETIF_F_IP_CSUM;

	return features;
}

static void __rtl8168_set_features(struct net_device *dev,
		netdev_features_t features)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	netdev_features_t changed = features ^ dev->features;
	u32 ioaddr = tp->base_addr;

	if (!(changed & (NETIF_F_RXALL | NETIF_F_RXCSUM |
					NETIF_F_HW_VLAN_CTAG_RX)))
		return;

	if (changed & (NETIF_F_RXCSUM | NETIF_F_HW_VLAN_CTAG_RX)) {
		if (features & NETIF_F_RXCSUM)
			tp->cp_cmd |= RxChkSum;
		else
			tp->cp_cmd &= ~RxChkSum;

		if (dev->features & NETIF_F_HW_VLAN_CTAG_RX)
			tp->cp_cmd |= RxVlan;
		else
			tp->cp_cmd &= ~RxVlan;

		RTL_W16(CPlusCmd, tp->cp_cmd);
		RTL_R16(CPlusCmd);
	}
	if (changed & NETIF_F_RXALL) {
		int tmp = (RTL_R32(RxConfig) & ~(AcceptErr | AcceptRunt));
		if (features & NETIF_F_RXALL)
			tmp |= (AcceptErr | AcceptRunt);
		RTL_W32(RxConfig, tmp);
	}
}

static int rtl8168_set_features(struct net_device *dev,
		netdev_features_t features)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	rtl_lock_work(tp);
	__rtl8168_set_features(dev, features);
	rtl_unlock_work(tp);

	return 0;
}


static inline u32 rtl8168_tx_vlan_tag(struct sk_buff *skb)
{
	return (skb_vlan_tag_present(skb)) ?
		TxVlanTag | swab16(skb_vlan_tag_get(skb)) : 0x00;
}

static void rtl8168_rx_vlan_tag(struct RxDesc *desc, struct sk_buff *skb)
{
	u32 opts2 = le32_to_cpu(desc->opts2);

	if (opts2 & RxVlanTag)
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), swab16(opts2 & 0xffff));
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0))
static int rtl8168_gset_xmii(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	mii_ethtool_gset(&tp->mii, cmd);

	return 0;
}
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0))
static int rtl8168_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	int rc;

	rtl_lock_work(tp);
	rtl8168_phy_int_disable(tp);
	rc = tp->get_settings(dev, cmd);
	rtl8168_phy_int_enable(tp);
	rtl_unlock_work(tp);

	return rc;
}
#endif
static void rtl8168_get_regs(struct net_device *dev, struct ethtool_regs *regs,
		void *p)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	if (regs->len > R8168_REGS_SIZE)
		regs->len = R8168_REGS_SIZE;

	rtl_lock_work(tp);
	memcpy_fromio(p, (void *)((unsigned long)tp->base_addr), regs->len);
	rtl_unlock_work(tp);
}

static u32 rtl8168_get_msglevel(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	return tp->msg_enable;
}

static void rtl8168_set_msglevel(struct net_device *dev, u32 value)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	tp->msg_enable = value;
}

static const char rtl8168_gstrings[][ETH_GSTRING_LEN] = {
	"tx_packets",
	"rx_packets",
	"tx_errors",
	"rx_errors",
	"rx_missed",
	"align_errors",
	"tx_single_collisions",
	"tx_multi_collisions",
	"unicast",
	"broadcast",
	"multicast",
	"tx_aborted",
	"tx_underrun",
};

static int rtl8168_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
		case ETH_SS_STATS:
			return ARRAY_SIZE(rtl8168_gstrings);
		default:
			return -EOPNOTSUPP;
	}
}

DECLARE_RTL_COND(rtl_counters_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R32(CounterAddrLow) & CounterDump;
}

static void rtl8168_update_counters(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	u32 ioaddr = tp->base_addr;
	struct device *d = &tp->dev->dev;
	struct rtl8168_counters *counters;
	dma_addr_t paddr;
	u32 cmd;

	/*
	* Some chips are unable to dump tally counters when the receiver
	* is disabled.
	*/
	if ((RTL_R8(ChipCmd) & CmdRxEnb) == 0)
		return;

	counters = dma_alloc_coherent(d, sizeof(*counters), &paddr, GFP_ATOMIC);
	if (!counters)
		return;
	/* dma_alloc_cohernet use GFP_ATOMIC, memory need to reset 0 */
	memset(counters, 0x0, sizeof(*counters));

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	RTL_W32(CounterAddrHigh, (u64)paddr >> 32);
#else
	RTL_W32(CounterAddrHigh, 0);
#endif
	cmd = (u64)paddr & DMA_BIT_MASK(32);
	RTL_W32(CounterAddrLow, cmd);
	RTL_W32(CounterAddrLow, cmd | CounterDump);

	if (rtl_udelay_loop_wait_low(tp, &rtl_counters_cond, 10, 1000))
		memcpy(&tp->counters, counters, sizeof(*counters));

	RTL_W32(CounterAddrLow, 0);
	RTL_W32(CounterAddrHigh, 0);

	dma_free_coherent(d, sizeof(*counters), counters, paddr);
}

static void rtl8168_get_ethtool_stats(struct net_device *dev,
		struct ethtool_stats *stats, u64 *data)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	ASSERT_RTNL();

	rtl8168_update_counters(dev);

	data[0] = le64_to_cpu(tp->counters.tx_packets);
	data[1] = le64_to_cpu(tp->counters.rx_packets);
	data[2] = le64_to_cpu(tp->counters.tx_errors);
	data[3] = le32_to_cpu(tp->counters.rx_errors);
	data[4] = le16_to_cpu(tp->counters.rx_missed);
	data[5] = le16_to_cpu(tp->counters.align_errors);
	data[6] = le32_to_cpu(tp->counters.tx_one_collision);
	data[7] = le32_to_cpu(tp->counters.tx_multi_collision);
	data[8] = le64_to_cpu(tp->counters.rx_unicast);
	data[9] = le64_to_cpu(tp->counters.rx_broadcast);
	data[10] = le32_to_cpu(tp->counters.rx_multicast);
	data[11] = le16_to_cpu(tp->counters.tx_aborted);
	data[12] = le16_to_cpu(tp->counters.tx_underun);
}

static void rtl8168_get_strings(struct net_device *dev, u32 stringset, u8 *data)
{
	switch(stringset) {
		case ETH_SS_STATS:
			memcpy(data, *rtl8168_gstrings, sizeof(rtl8168_gstrings));
			break;
	}
}

static const struct ethtool_ops rtl8168_ethtool_ops = {
	.get_drvinfo		= rtl8168_get_drvinfo,
	.get_regs_len		= rtl8168_get_regs_len,
	.get_link		= ethtool_op_get_link,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0))
	.get_settings		= rtl8168_get_settings,
	.set_settings		= rtl8168_set_settings,
#endif
	.get_msglevel		= rtl8168_get_msglevel,
	.set_msglevel		= rtl8168_set_msglevel,
	.get_regs		= rtl8168_get_regs,
	.get_wol		= rtl8168_get_wol,
	.set_wol		= rtl8168_set_wol,
	.get_strings		= rtl8168_get_strings,
	.get_sset_count		= rtl8168_get_sset_count,
	.get_ethtool_stats	= rtl8168_get_ethtool_stats,
	.get_ts_info		= ethtool_op_get_ts_info,
};

static int rtl8168_check_mac_version(struct rtl8168_private *tp, struct net_device *dev)
{
	u32 ioaddr = tp->base_addr;
	u32 mask = 0x7cf00000;
	u32 val = 0x50900000;	/* 8168GU */
	u32 reg;

	reg = RTL_R32(TxConfig);
	if ((reg & mask) != val)
		return 0;

	return 1;
}

#if NWAY_ENABLE
static void rtl_phy_work(struct rtl8168_private *tp)
{
	struct timer_list *timer = &tp->timer;
	u32 ioaddr = tp->base_addr;
	unsigned long timeout = RTL8168_PHY_TIMEOUT;

	rtl8168_phy_int_disable(tp);
	if (tp->phy_reset_pending(tp)) {
		/*
		* A busy loop could burn quite a few cycles on nowadays CPU.
		* Let's delay the execution of the timer for a few ticks.
		*/
		timeout = HZ/10;
		rtl8168_phy_int_enable(tp);
		goto out_mod_timer;
	}

	rtl8168_phy_int_enable(tp);
	if (tp->link_ok(ioaddr))
		return;

	eth_warning("PHY reset until link up\n");

	rtl8168_phy_int_disable(tp);
	tp->phy_enhance_performance(tp, 1);
#ifdef ENABLE_ALDPS
	tp->phy_set_aldps(tp, 1);
#endif
	tp->phy_reset_enable(tp);
	rtl8168_phy_int_enable(tp);

out_mod_timer:
	mod_timer(timer, jiffies + timeout);
}
#endif

static void rtl_schedule_task(struct rtl8168_private *tp, enum rtl_flag flag)
{
	if (!test_and_set_bit(flag, tp->wk.flags))
		schedule_work(&tp->wk.work);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
static void rtl8168_phy_timer(struct timer_list* param_timer)
{
	struct rtl8168_private *tp = container_of(param_timer,struct rtl8168_private,timer);
	rtl_schedule_task(tp, RTL_FLAG_TASK_PHY_PENDING);
}
#else
static void rtl8168_phy_timer(unsigned long __opaque)
{
	struct net_device *dev = (struct net_device *)__opaque;
	struct rtl8168_private *tp = netdev_priv(dev);
	rtl_schedule_task(tp, RTL_FLAG_TASK_PHY_PENDING);
}
#endif


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
static void rtl8168_link_timer(struct timer_list* param_timer)
{
	struct rtl8168_private *tp = container_of(param_timer,struct rtl8168_private,link_check_timer);

	if(tp->link_ok(tp->base_addr))
		return;

	rtl8168_phy_int_disable(tp);
	/* Read link status, speed, duplex, and other phy status */
	rtl_read_phy_status(tp);
	rtl8168_phy_int_enable(tp);
}
#else
static void rtl8168_link_timer(unsigned long __opaque)
{
	struct net_device *dev = (struct net_device *)__opaque;
	struct rtl8168_private *tp = netdev_priv(dev);
	if(tp->link_ok(tp->base_addr))
		return;

	rtl8168_phy_int_disable(tp);
	/* Read link status, speed, duplex, and other phy status */
	rtl_read_phy_status(tp);
	rtl8168_phy_int_enable(tp);
}
#endif
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
static void rtl8168_rx_timer(struct timer_list* param_timer)
{
	struct rtl8168_private *tp = from_timer(tp,param_timer,rx_timer);
	unsigned long flags;
	local_irq_save(flags);
	rtl_irq_disable(tp);
	wmb();
	napi_schedule(&tp->napi);
	local_irq_restore(flags);
	mod_timer(&tp->rx_timer, jiffies + RTL8168_RX_TIMEOUT);
}
#else
static void rtl8168_rx_timer(unsigned long __opaque)
{
	struct net_device *dev = (struct net_device *)__opaque;
	struct rtl8168_private *tp = netdev_priv(dev);
	unsigned long flags;
	local_irq_save(flags);
	rtl_irq_disable(tp);
	wmb();
	napi_schedule(&tp->napi);
	local_irq_restore(flags);
	mod_timer(&tp->rx_timer, jiffies + RTL8168_RX_TIMEOUT);
}
#endif
#endif

DECLARE_RTL_COND(rtl_phy_reset_cond)
{
	return tp->phy_reset_pending(tp);
}

static void rtl8168_phy_reset(struct net_device *dev,
		struct rtl8168_private *tp)
{
	//eth_debug("==> %s:%s %d \n",tp->dev->name,__FUNCTION__, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

	tp->phy_reset_enable(tp);
	//rtl_msleep_loop_wait_low(tp, &rtl_phy_reset_cond, 1, 100);
	rtl_udelay_loop_wait_low(tp, &rtl_phy_reset_cond, 500, 200);
}

#ifdef ENABLE_ALDPS
static void rtl8168_phy_set_aldps(struct rtl8168_private *tp, u32 enable)
{
	tp->phy_set_aldps(tp, enable);
}
#endif

static void rtl8168_phy_enhance_performance(struct rtl8168_private *tp, u32 enable)
{
	tp->phy_enhance_performance(tp, enable);
}

DECLARE_RTL_COND(rtl_phy_status_cond)
{
	if((rtl_phy_read(tp, 0x0a42, 0x10) & 0x3) == 0x3) {
		return 1;
	}
	return 0;
}

#include "rtk_8168_wrapper.c"

static void rtl8168_init_phy(struct net_device *dev, struct rtl8168_private *tp)
{
	/*rtl_hw_phy_config(dev); */

	/* Exit oob */
	u32 ioaddr = tp->base_addr;
	RTL_W8(MCU, RTL_R8(MCU) & ~NOW_IS_OOB);

#if NWAY_ENABLE
	rtl8168_phy_enhance_performance(tp, 1);
#ifdef ENABLE_ALDPS
	rtl8168_phy_set_aldps(tp, 1);
#endif
	rtl8168_phy_reset(dev, tp);

	//eth_debug("\033[7;41m %s:%s N-way %d \033[m \n",tp->dev->name,__FUNCTION__, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

	rtl8168_set_speed(dev, AUTONEG_ENABLE, SPEED_1000, DUPLEX_FULL,
			ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
			ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full |
			(tp->mii.supports_gmii ?
			ADVERTISED_1000baseT_Half |
			ADVERTISED_1000baseT_Full : 0));
#else
#ifdef ENABLE_ALDPS
	rtl8168_phy_set_aldps(tp, 1);
#endif
	rtl8168_phy_enhance_performance(tp, 1);

	//eth_debug("\033[7;41m ==> %s:%s force 100 %d \033[m \n",tp->dev->name,__FUNCTION__, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

	rtl8168_set_speed(dev, AUTONEG_DISABLE, SPEED_100, DUPLEX_FULL, ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
			ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full);
#endif

	/*
	if (rtl_tbi_enabled(tp))
	netif_info(tp, link, dev, "TBI auto-negotiating\n");
	*/
}

static void rtl_rar_set(struct rtl8168_private *tp, u8 *addr)
{
	u32 ioaddr = tp->base_addr;

	rtl_lock_work(tp);

	RTL_W8(Cfg9346, Cfg9346_Unlock);

	RTL_W32(MAC4, addr[4] | addr[5] << 8);
	RTL_R32(MAC4);

	RTL_W32(MAC0, addr[0] | addr[1] << 8 | addr[2] << 16 | addr[3] << 24);
	RTL_R32(MAC0);

	RTL_W8(Cfg9346, Cfg9346_Lock);

	rtl_unlock_work(tp);
}

static int rtl_set_mac_address(struct net_device *dev, void *p)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	rtl_rar_set(tp, dev->dev_addr);

	return 0;
}

static int rtl8168_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	struct mii_ioctl_data *data = if_mii(ifr);

	return netif_running(dev) ? tp->do_ioctl(tp, data, cmd) : -ENODEV;
}

static int rtl_xmii_ioctl(struct rtl8168_private *tp,
		struct mii_ioctl_data *data, int cmd)
{

	switch (cmd) {
		case SIOCGMIIPHY:
			data->phy_id = 32; /* Internal PHY */
			return 0;

		case SIOCGMIIREG:
			rtl8168_phy_int_disable(tp);
			data->val_out = rtl_phy_read(tp, 0, data->reg_num & 0x1f);
			rtl8168_phy_int_enable(tp);
			return 0;

		case SIOCSMIIREG:
			rtl8168_phy_int_disable(tp);
			rtl_phy_write(tp, 0, data->reg_num & 0x1f, data->val_in);
			rtl8168_phy_int_enable(tp);
			return 0;
	}
	return -EOPNOTSUPP;
}

#if 0
static void rtl_disable_msi(struct pci_dev *pdev, struct rtl8168_private *tp)
{
	if (tp->features & RTL_FEATURE_MSI) {
		tp->features &= ~RTL_FEATURE_MSI;
	}
}
#endif



#ifdef ENABLE_WOL
static void rtl_speed_down(struct rtl8168_private *tp)
{
	u32 adv;
	int lpa;

	lpa = rtl_phy_read(tp, 0, MII_LPA);

	if (lpa & (LPA_10HALF | LPA_10FULL))
		adv = ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full;
	else if (lpa & (LPA_100HALF | LPA_100FULL))
		adv = ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
			ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full;
	else
		adv = ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
			ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full |
			(tp->mii.supports_gmii ?
			ADVERTISED_1000baseT_Half |
			ADVERTISED_1000baseT_Full : 0);

	//rtl8168_set_speed(tp->dev, AUTONEG_ENABLE, SPEED_1000, DUPLEX_FULL, adv);
	rtl8168_set_speed(tp->dev, AUTONEG_DISABLE, SPEED_10, DUPLEX_FULL, adv);
}
#endif



#ifdef ENABLE_WOL
static void rtl_wpd_set(struct rtl8168_private *tp)
{
	u32 tmp;
	u32 i;

	/* clear WPD registers */
	r8168_mac_ocp_write(tp, 0xD23A, 0);
	r8168_mac_ocp_write(tp, 0xD23C, 0);
	r8168_mac_ocp_write(tp, 0xD23E, 0);
	for (i = 0; i < 128; i += 2)
		r8168_mac_ocp_write(tp, 0xD240 + i, 0);



	tmp = r8168_mac_ocp_read(tp, 0xC0C2);
	tmp &= ~BIT(4);
	r8168_mac_ocp_write(tp, 0xC0C2, tmp);



	/* enable WPD */
	tmp = r8168_mac_ocp_read(tp, 0xC0C2);
	tmp |= BIT(0);
	r8168_mac_ocp_write(tp, 0xC0C2, tmp);
}

static void rtl_wol_suspend_quirk(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(RxConfig, RTL_R32(RxConfig) |
			AcceptBroadcast | AcceptMulticast | AcceptMyPhys);
}

static bool rtl_wol_pll_power_down(struct rtl8168_private *tp)
{
	if (!(__rtl8168_get_wol(tp) & WAKE_ANY))
		return false;

#ifndef ENABLE_EEEP
	rtl8168_phy_int_disable(tp);
	/* fix 10M */
	rtl_speed_down(tp);
	rtl8168_phy_int_enable(tp);
#endif
	rtl_wol_suspend_quirk(tp);

	return true;
}
//#endif

#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
static void rtl_set_wol_pattern(struct rtl8168_private *tp)
{
	int i;

	eth_info("set wol pattern\n");
	//set wol_mask_selection
	for(i = 0; i < NUM_WOL_PATTERN; i	= i + 4) {
		u16 tmp = 0;
		int n;
		int j;
		n = (( NUM_WOL_PATTERN	- i) >= 4) ? 4 : ( NUM_WOL_PATTERN	- i);
		for(j = 0;j < n;j++) {
			tmp = (tmp << 4);
			tmp = tmp | (mask_selection_code[i + n - j - 1] & 0xF);
		}
		r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_MASK_SELECTION_REG + i / 2, tmp);
	}

	//set wol_mask
	for(i = 0; i < (WOL_MASK_SIZE * 2); i++)
	{
		int index = i / 2;
		int j;
		for(j = 0; j < NUM_WOL_PATTERN; j = j + 4) {
			if((i % 2) == 0) {
				int n;
				int k;
				u16 tmp = 0;
				n = ((NUM_WOL_PATTERN - j) >= 4) ? 4 : (NUM_WOL_PATTERN - j);
				for(k = 0; k < n; k++) {
					tmp = tmp << 4;
					tmp = tmp | (wol_mask[j + n - k - 1][index] & 0xF);
				}
				r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_MASK_REG + i * 16 + j / 2, tmp);
			} else {
				int n;
				int k;
				u16 tmp = 0;
				n = ((NUM_WOL_PATTERN - j) >= 4) ? 4 : (NUM_WOL_PATTERN - j);
				for(k = 0; k < n; k++) {
					tmp = tmp << 4;
					tmp = tmp | ((wol_mask[j + n - k - 1][index] >> 4) & 0xF);
				}
				r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_MASK_REG + i * 16 + j / 2, tmp);
			}
		}
	}

	//set wol crc
	for(i=0;i<NUM_WOL_PATTERN;i++)
	{
		if(i < 8)
			r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_CRC1_REG + i * 2, wol_pattern[i]);
		else
			r8168_mac_ocp_write(tp, CRC_PATTERN_WOL_CRC2_REG + (i -8) * 2, wol_pattern[i]);
	}
#if 0
	//set wol start code and packet pattern
	for(i=0;i<NUM_WOL_PATTERN;i++)
	{
		int j = 0;
		r8168_mac_ocp_write(tp, CRC_PATTERN_PACKET_START_REG + i * 138, packet_start_code[i]);		
		for(j = 0; j	< (PACKET_PATTERN_SIZE / 2); j++) {
			r8168_mac_ocp_write(tp, CRC_PATTERN_PACKET_PATTREN_REG + i * 138 +	j * 2,
				packet_pattern[i][2 * j] | (packet_pattern[i][2 * j + 1] << 8));
		}		

	}
	//enable crc exactly match function
	r8168_mac_ocp_write(tp, 0xc0c2, r8168_mac_ocp_read(tp, 0xc0c2) | 0x8);
#endif
}
#else
static void rtl_set_wol_pattern(struct rtl8168_private *tp)
{
	int i	= 0;
	u32 temp = 0;

	eth_info("set wol pattern\n");

	for(i=0;i<16;i++) {

		u32 eri_mask = 0;
		u32 val = 0;

		eth_info("wol_mask[0][%d] mask0=0x%02x temp=0x%08x\n",i,wol_mask[0][i],temp);
		eth_info("wol_mask[1][%d] mask0=0x%02x temp=0x%08x\n",i,wol_mask[1][i],temp);

		eri_mask = ERIAR_MASK_0011;
		val = ((u32)wol_mask[0][i]<<0) | ((u32)wol_mask[1][i]<<8);


		rtl_eri_write(tp,	(i * 8), eri_mask, val, ERIAR_EXGMAC);
	}

	// set mask
	for(i = 0; i < NUM_WOL_PATTERN; i = i + 2) {
		if(i + 2 <= NUM_WOL_PATTERN) {
			rtl_eri_write(tp, 0x80 + i * 2, ERIAR_MASK_1111, ((u32)wol_pattern[i + 1] << 16) 
						| ((u32)wol_pattern[i]), ERIAR_EXGMAC);
		} else {
			rtl_eri_write(tp, 0x80 + i * 2, ERIAR_MASK_0011, (u32)wol_pattern[i], ERIAR_EXGMAC);
		}
	}
}

#endif

static void rtl_wol_set(struct rtl8168_private *tp, u32 enable)
{
	u32 ioaddr = tp->base_addr;
	//u32 wol_mode = 1;

	eth_info("%s features=0x%x\n", __func__,tp->features);
	if (enable == RTL8168_WOL_ENABLE)
	{
		if(g_is_new_wol_flow || g_vcpu_mdns_offloading_wol_flow || g_hw_mdns_offloading_wol_flow) {
			rtd_outl(ETH_WAKE_DUMMY_REG, rtd_inl(ETH_WAKE_DUMMY_REG) | ETH_WOL_DUMMY_WPD_DETECT_ENABLE);
			if(g_vcpu_mdns_offloading_wol_flow)
				rtd_outl(ETH_WAKE_DUMMY_REG, rtd_inl(ETH_WAKE_DUMMY_REG) | ETH_WOL_DUMMY_VCPU_MDNS_OFFLOADING_ENABLE);
			else if(g_hw_mdns_offloading_wol_flow)
				rtd_outl(ETH_WAKE_DUMMY_REG, rtd_inl(ETH_WAKE_DUMMY_REG) | ETH_WOL_DUMMY_HW_MDNS_OFFLOADING_ENABLE);
			rtl_wpd_set(tp);
		}

		/* Clear ETN WOL Status */
		RTL_W32(OCPDR, 0xf0031e00);

#ifdef ENABLE_ALDPS
		rtl8168_phy_int_disable(tp);
		/* Diable Aldps mode */
		rtl8168_phy_set_aldps(tp, 0);
		rtl8168_phy_int_enable(tp);
#endif
		if(tp->features & RTL_FEATURE_WOL_PATTERN) {
			if(!g_hw_mdns_offloading_wol_flow) {
					wol_pattern[0] = rtl8168_get_airplay_magic_packet(tp, wol_mask[0]);
				rtl_set_wol_pattern(tp);
			}
			else {
				rtl8168_airplay_to_crc_pattern(tp);
				rtk8168_set_wake_port_pattern(tp);
			}
		}

		if(tp->features & RTL_FEATURE_WOL_PATTERN) {
			if(g_vcpu_mdns_offloading_wol_flow)
				rtl8168_set_vcpu_mdns_offload_params(tp->dev);
		}

		if(!g_hw_mdns_offloading_wol_flow) {
			/* <1> Enable WOL IP setting for MAC */
			RTL_W8(Cfg9346, Cfg9346_Unlock);
			RTL_W8(MCU, RTL_R8(MCU) | NOW_IS_OOB | DIS_MCU_CLROOB); /* enable now_is_oob */
#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
			r8168_mac_ocp_write(tp, 0xE8DE, r8168_mac_ocp_read(tp, 0xE8DE) | (1 << 14));//mcu_borw_en
#endif
			RTL_W8(Config5, RTL_R8(Config5) | LanWake);
			RTL_W8(Config3, RTL_R8(Config3) | MagicPacket);
			RTL_W8(Cfg9346, Cfg9346_Lock);
		}

		/* Set PLETN WOW_GMAC_CLKSEL */
		if (tp->output_mode == OUTPUT_EMBEDDED_PHY)
			rtd_outl(REG_WOW_GMAC_CLK, 0x1);


		if(tp->features & RTL_FEATURE_WOL_PATTERN) {
			if(g_hw_mdns_offloading_wol_flow) {
				rtk8168_hw_mdns_standby_pre_oob_setting(tp);
				rtk8168_hw_mdns_standby_settings(tp);
				printf_domain_debug_log();
			}
		}

		if (rtl_wol_pll_power_down(tp))
		{		
#ifdef ENABLE_EEEP
			if(tp->output_mode == OUTPUT_EMBEDDED_PHY) {
				if(g_hw_mdns_offloading_wol_flow)	{
					rtl8168_phy_int_disable(tp);
					/* <2> Enable EEE */
					rtl8168_phy_eee_enable(tp, true);
					/* <3> Force 10M */
					rtl_speed_down(tp);
					rtl8168_phy_int_enable(tp);
				} else {
					rtl8168_phy_int_disable(tp);
					/* <2> Enable EEEP */
					rtl8168_mac_eeep_enable(tp);
					/* <3> Force 10M */
					rtl_speed_down(tp);
					rtl8168_phy_int_enable(tp);
					/* <4> EEEP patch code */
					rtl8168_mac_eeep_patch_disable(tp);
					rtl8168_mac_eeep_patch(tp);
				}
			} else {
#if 0				
				r8168soc_eee_init(tp, true);
#else
				rtl8168_phy_int_disable(tp);
				/* <2> Enable EEEP */
				rtl8168_mac_eeep_enable(tp);
				rtl8168_phy_int_enable(tp);
#endif
			}
#endif
#ifdef ENABLE_ALDPS
			rtl8168_phy_int_disable(tp);
			/* <5> Enable Aldps mode */
			rtl8168_phy_set_aldps(tp, 1);
			rtl8168_phy_int_enable(tp);
#endif
			if(tp->features & RTL_FEATURE_WOL_PATTERN) {
				if(g_hw_mdns_offloading_wol_flow) {	
					rtk8168_hw_mdns_standby_post_oob_setting(tp);
				}
			}
			rtd_outl(PLLETN_GETN_RST_reg, PLLETN_GETN_RST_rstn_getn_sys_mask);//add to advoid	mac to issue abnoramal access to ddr when wake up	by mdns
			return;
		}
		else
			eth_err("wol setting failed\n");
	} else {
		return;
	}
}
#endif

#ifdef ENABLE_WOW
// ToDo: WOW
static void rtl_realwow_set(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	// isram patch
	RTL_W32(OCPDR,0xFC00E008);
	RTL_W32(OCPDR,0xFC01E02C);
	RTL_W32(OCPDR,0xFC02E032);
	RTL_W32(OCPDR,0xFC03E06A);
	RTL_W32(OCPDR,0xFC04E0A1);
	RTL_W32(OCPDR,0xFC05E0A3);
	RTL_W32(OCPDR,0xFC06E0A5);
	RTL_W32(OCPDR,0xFC07E0A7);
	RTL_W32(OCPDR,0xFC08C208);
	RTL_W32(OCPDR,0xFC097340);
	RTL_W32(OCPDR,0xFC0A49B7);
	RTL_W32(OCPDR,0xFC0BF009);
	RTL_W32(OCPDR,0xFC0C1D02);
	RTL_W32(OCPDR,0xFC0D8D40);
	RTL_W32(OCPDR,0xFC0EC203);
	RTL_W32(OCPDR,0xFC0FBA00);
	RTL_W32(OCPDR,0xFC10C0BC);
	RTL_W32(OCPDR,0xFC11100C);
	RTL_W32(OCPDR,0xFC12D2E4);
	RTL_W32(OCPDR,0xFC13C0C4);
	RTL_W32(OCPDR,0xFC14C5FE);
	RTL_W32(OCPDR,0xFC1574A0);
	RTL_W32(OCPDR,0xFC1649C0);
	RTL_W32(OCPDR,0xFC17F010);
	RTL_W32(OCPDR,0xFC1874A2);
	RTL_W32(OCPDR,0xFC1976A4);
	RTL_W32(OCPDR,0xFC1A4034);
	RTL_W32(OCPDR,0xFC1BF804);
	RTL_W32(OCPDR,0xFC1C0601);
	RTL_W32(OCPDR,0xFC1D9EA4);
	RTL_W32(OCPDR,0xFC1EE009);
	RTL_W32(OCPDR,0xFC1F1D02);
	RTL_W32(OCPDR,0xFC208D40);
	RTL_W32(OCPDR,0xFC21C5F2);
	RTL_W32(OCPDR,0xFC2264A1);
	RTL_W32(OCPDR,0xFC234845);
	RTL_W32(OCPDR,0xFC248CA1);
	RTL_W32(OCPDR,0xFC25C606);
	RTL_W32(OCPDR,0xFC26BE00);
	RTL_W32(OCPDR,0xFC271C20);
	RTL_W32(OCPDR,0xFC288C41);
	RTL_W32(OCPDR,0xFC29C603);
	RTL_W32(OCPDR,0xFC2ABE00);
	RTL_W32(OCPDR,0xFC2B104E);
	RTL_W32(OCPDR,0xFC2C0C80);
	RTL_W32(OCPDR,0xFC2DC605);
	RTL_W32(OCPDR,0xFC2E1D22);
	RTL_W32(OCPDR,0xFC2F8DC0);
	RTL_W32(OCPDR,0xFC30C603);
	RTL_W32(OCPDR,0xFC31BE00);
	RTL_W32(OCPDR,0xFC32C0BC);
	RTL_W32(OCPDR,0xFC330D28);
	RTL_W32(OCPDR,0xFC347236);
	RTL_W32(OCPDR,0xFC350208);
	RTL_W32(OCPDR,0xFC36C323);
	RTL_W32(OCPDR,0xFC37C724);
	RTL_W32(OCPDR,0xFC3864E0);
	RTL_W32(OCPDR,0xFC397522);
	RTL_W32(OCPDR,0xFC3A26D5);
	RTL_W32(OCPDR,0xFC3B050C);
	RTL_W32(OCPDR,0xFC3C31EC);
	RTL_W32(OCPDR,0xFC3D7620);
	RTL_W32(OCPDR,0xFC3E48EF);
	RTL_W32(OCPDR,0xFC3F403E);
	RTL_W32(OCPDR,0xFC40F80A);
	RTL_W32(OCPDR,0xFC41E81D);
	RTL_W32(OCPDR,0xFC421401);
	RTL_W32(OCPDR,0xFC43F107);
	RTL_W32(OCPDR,0xFC441C10);
	RTL_W32(OCPDR,0xFC45C518);
	RTL_W32(OCPDR,0xFC468CA1);
	RTL_W32(OCPDR,0xFC47C602);
	RTL_W32(OCPDR,0xFC48BE00);
	RTL_W32(OCPDR,0xFC490A6A);
	RTL_W32(OCPDR,0xFC4AC310);
	RTL_W32(OCPDR,0xFC4BC710);
	RTL_W32(OCPDR,0xFC4C64E1);
	RTL_W32(OCPDR,0xFC4D31EC);
	RTL_W32(OCPDR,0xFC4E403E);
	RTL_W32(OCPDR,0xFC4FF807);
	RTL_W32(OCPDR,0xFC50E80E);
	RTL_W32(OCPDR,0xFC511401);
	RTL_W32(OCPDR,0xFC52F104);
	RTL_W32(OCPDR,0xFC53C509);
	RTL_W32(OCPDR,0xFC541E00);
	RTL_W32(OCPDR,0xFC559EA4);
	RTL_W32(OCPDR,0xFC56C602);
	RTL_W32(OCPDR,0xFC57BE00);
	RTL_W32(OCPDR,0xFC580D6E);
	RTL_W32(OCPDR,0xFC596808);
	RTL_W32(OCPDR,0xFC5A6848);
	RTL_W32(OCPDR,0xFC5B6800);
	RTL_W32(OCPDR,0xFC5CD2E4);
	RTL_W32(OCPDR,0xFC5DC0C4);
	RTL_W32(OCPDR,0xFC5EB405);
	RTL_W32(OCPDR,0xFC5FB407);
	RTL_W32(OCPDR,0xFC600C01);
	RTL_W32(OCPDR,0xFC61A154);
	RTL_W32(OCPDR,0xFC62A1DC);
	RTL_W32(OCPDR,0xFC63402F);
	RTL_W32(OCPDR,0xFC64F105);
	RTL_W32(OCPDR,0xFC651400);
	RTL_W32(OCPDR,0xFC66F1FA);
	RTL_W32(OCPDR,0xFC671C01);
	RTL_W32(OCPDR,0xFC68E002);
	RTL_W32(OCPDR,0xFC691C00);
	RTL_W32(OCPDR,0xFC6AB007);
	RTL_W32(OCPDR,0xFC6BB005);
	RTL_W32(OCPDR,0xFC6CFF80);
	RTL_W32(OCPDR,0xFC6DC532);
	RTL_W32(OCPDR,0xFC6E74A0);
	RTL_W32(OCPDR,0xFC6F49C0);
	RTL_W32(OCPDR,0xFC70F01E);
	RTL_W32(OCPDR,0xFC71C52F);
	RTL_W32(OCPDR,0xFC7273A0);
	RTL_W32(OCPDR,0xFC731300);
	RTL_W32(OCPDR,0xFC74F104);
	RTL_W32(OCPDR,0xFC7573A2);
	RTL_W32(OCPDR,0xFC761300);
	RTL_W32(OCPDR,0xFC77F014);
	RTL_W32(OCPDR,0xFC78C52A);
	RTL_W32(OCPDR,0xFC7974A0);
	RTL_W32(OCPDR,0xFC7A49C0);
	RTL_W32(OCPDR,0xFC7BF11B);
	RTL_W32(OCPDR,0xFC7CC525);
	RTL_W32(OCPDR,0xFC7D76A0);
	RTL_W32(OCPDR,0xFC7E74A2);
	RTL_W32(OCPDR,0xFC7F0601);
	RTL_W32(OCPDR,0xFC803720);
	RTL_W32(OCPDR,0xFC819EA0);
	RTL_W32(OCPDR,0xFC829CA2);
	RTL_W32(OCPDR,0xFC83C51D);
	RTL_W32(OCPDR,0xFC8473A2);
	RTL_W32(OCPDR,0xFC854023);
	RTL_W32(OCPDR,0xFC86F810);
	RTL_W32(OCPDR,0xFC87F304);
	RTL_W32(OCPDR,0xFC8873A0);
	RTL_W32(OCPDR,0xFC894033);
	RTL_W32(OCPDR,0xFC8AF80C);
	RTL_W32(OCPDR,0xFC8BC514);
	RTL_W32(OCPDR,0xFC8C1C02);
	RTL_W32(OCPDR,0xFC8D8CA1);
	RTL_W32(OCPDR,0xFC8EC515);
	RTL_W32(OCPDR,0xFC8F76A8);
	RTL_W32(OCPDR,0xFC904862);
	RTL_W32(OCPDR,0xFC9148E0);
	RTL_W32(OCPDR,0xFC929EA8);
	RTL_W32(OCPDR,0xFC93C602);
	RTL_W32(OCPDR,0xFC94BE00);
	RTL_W32(OCPDR,0xFC9500E0);
	RTL_W32(OCPDR,0xFC961C01);
	RTL_W32(OCPDR,0xFC97C50B);
	RTL_W32(OCPDR,0xFC988CA1);
	RTL_W32(OCPDR,0xFC99C60B);
	RTL_W32(OCPDR,0xFC9A1F20);
	RTL_W32(OCPDR,0xFC9B9FC0);
	RTL_W32(OCPDR,0xFC9C48F5);
	RTL_W32(OCPDR,0xFC9D9FC0);
	RTL_W32(OCPDR,0xFC9EE7F0);
	RTL_W32(OCPDR,0xFC9FC0BC);
	RTL_W32(OCPDR,0xFCA0D2C8);
	RTL_W32(OCPDR,0xFCA1D2CC);
	RTL_W32(OCPDR,0xFCA2C0C4);
	RTL_W32(OCPDR,0xFCA3E420);
	RTL_W32(OCPDR,0xFCA4C0F0);
	RTL_W32(OCPDR,0xFCA5C602);
	RTL_W32(OCPDR,0xFCA6BE00);
	RTL_W32(OCPDR,0xFCA70000);
	RTL_W32(OCPDR,0xFCA8C602);
	RTL_W32(OCPDR,0xFCA9BE00);
	RTL_W32(OCPDR,0xFCAA0000);
	RTL_W32(OCPDR,0xFCABC602);
	RTL_W32(OCPDR,0xFCACBE00);
	RTL_W32(OCPDR,0xFCAD0000);
	RTL_W32(OCPDR,0xFCAEC602);
	RTL_W32(OCPDR,0xFCAFBE00);
	RTL_W32(OCPDR,0xFCB00000);

	// bp base address
	RTL_W32(OCPDR,0xFC268000);
	// bp 0~3
	RTL_W32(OCPDR,0xFC280FD3);
	RTL_W32(OCPDR,0xFC2A0D23);
	RTL_W32(OCPDR,0xFC2C0F95);
	RTL_W32(OCPDR,0xFC2E009B);
}

static void rtl_wow_set(struct rtl8168_private *tp, u32 enable)
{
	u32 ioaddr = tp->base_addr;
	u32 data_read_u32 = 0;
	u32 data_write_u32 = 0;
	int reg_addr_int = 0;
	u32 reg_addr_u32 = 0;
	u32 reg_mask_u32 = 0;
	u32 u32i = 0;
	u8 MACID[6] = {0x00, 0x11, 0x22, 0x33, 0x17, 0x34};
	u8 OOB_IPv4[4] = {172, 22, 56, 23};
	u8 NS0_MACID[6] = {0x00, 0x11, 0x22, 0x33, 0x17, 0x34};
	u16 UDP_PORT[2] = {1734, 3417};
	u16 ACKMISS_TIME = 0xFFFF;
	u8 ack_len = 0xFF;
	u8 wake_len = 0xFF;
	u8 tx1_len = 0xFF;
	u8 tx2_len = 0xFF;
	u16 txpkt_timer = 0x0080; //tx packet timer (in units of 8ms) ~1024ms


	if (enable == 1)
	{
		/* (1) Set Now_is_OOB = 0 (IO offset 0xd3 bit 7 set to 0b) */
		RTL_W8(MCU, RTL_R8(MCU) & ~NOW_IS_OOB);

		/* (2) Flow control related (only for OOB) */
		/* I. ERI 0xC8 => 0x03	credit full */
		reg_addr_int = 0x0C8;
		reg_mask_u32 = ERIAR_MASK_0001;
		data_write_u32 = 0x00000003;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		/* II. ERI 0xCA => 0x180	point full */
		reg_addr_int = 0x0CA;
		reg_mask_u32 = ERIAR_MASK_0011;
		data_write_u32 = 0x00000180;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		/* III. ERI 0xCC => 0x4A	credit near full */
		reg_addr_int = 0x0CC;
		reg_mask_u32 = ERIAR_MASK_0001;
		data_write_u32 = 0x0000004A;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		/* IV. ERI 0xD0 => 0x5A	credit near empty */
		reg_addr_int = 0x0D0;
		reg_mask_u32 = ERIAR_MASK_0001;
		data_write_u32 = 0x0000005A;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);

		/* (3)Turn off RCR (IO offset 0x44 set to 0) */
		RTL_W32(RxConfig, 0x00000000);

		/* (4) Set rxdv_gated_en (IO 0xF2 bit3 = 1) */
		RTL_W32(MISC, RTL_R32(MISC) | RXDV_GATED_EN);

		/* (5) check FIFO empty (IO 0xD2 bit[12:13]) */
		while(1)
		{
			if (RTL_R8(MCU) & TX_EMPTY)
			{
				if (RTL_R8(MCU) & RX_EMPTY)
					break;
			}
			eth_debug("check Tx/RX FIFO empty fail\n");
			msleep(1);
		}

		/* (6) disable Tx/Rx enable = 0 (IO 0x36 bit[10:11]=0b) */
		RTL_W32(ChipCmd, RTL_R32(ChipCmd) & ~(CmdTxEnb | CmdRxEnb));

		/* (7) check link_list ready =1 (IO 0xD2 bit9=1) */
		while(1) {
			if (RTL_R16(MCU) & LINK_LIST_RDY)
				break;
			eth_debug("check link_list ready fail != 1\n");
			msleep(1);
		}

		/* (8) set re_ini_ll =1 (MACOCP : 0xE8DE bit15=1) */
		reg_addr_u32 = 0x0000E8DE;
		data_read_u32 = r8168_mac_ocp_read(tp, reg_addr_u32);
		data_write_u32 = data_read_u32 | 0x00001000;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);

		/* (9) check link_list ready =1 (IO 0xD2 bit9=1) */
		while(1) {
			if (RTL_R16(MCU) & LINK_LIST_RDY)
				break;
			eth_debug("check link_list ready fail != 1\n");
			msleep(1);
		}

		/* (10) Setting RMS (IO offset 0xdb-0xda set to 0x05F3) */
		RTL_W32(RxMaxSize, 0x05F3);

		/* (11) OOB parameter setting */
		/* b. MACID_VLAN(MAC needs the offload information from upper layer) */
		/*	1. VLAN ID: ERI offset 0xf1-0Xf0 set 0xFFFF */
		reg_addr_int = 0x0F0;
		reg_mask_u32 = ERIAR_MASK_0011;
		data_write_u32 = 0x0000FFFF;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		//			/*	2. MACID : ERI offset 0xf7-0xf2 */
		for(u32i = 0; u32i < 6; u32i++)
		{
			reg_addr_int = 0x0F2 + u32i;
			reg_mask_u32 = ERIAR_MASK_0001;
			data_write_u32 = MACID[u32i];
			rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		}
		/*
		reg_addr_int = 0x0F2;
		reg_mask_u32 = ERIAR_MASK_1111;
		data_write_u32 = 0xFFFFFFFF;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		reg_addr_int = 0x0F6;
		reg_mask_u32 = ERIAR_MASK_0011;
		data_write_u32 = 0x0000FFFF;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		*/
		/* d. OOB_IPv4 (IPv4 Address needs the offload information from upper layer) */
		//			/*	1. OOB_IPv4 : ERI offset 0x1f3-0x1f0 */
		for(u32i = 0; u32i < 4; u32i++)
		{
			reg_addr_int = 0x1F0 + u32i;
			reg_mask_u32 = ERIAR_MASK_0001;
			data_write_u32 = OOB_IPv4[u32i];
			rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		}
		/*
		reg_addr_int = 0x1F0;
		reg_mask_u32 = ERIAR_MASK_1111;
		data_write_u32 = 0xFFFFFFFF;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		*/
		/* e. NS_MACID */
		//			/*	1. NS0_MACID : ERI offset 0x25d-0x258 */
		for(u32i = 0; u32i < 6; u32i++)
		{
			reg_addr_int = 0x0F2 + u32i;
			reg_mask_u32 = ERIAR_MASK_0001;
			data_write_u32 = NS0_MACID[u32i];
			rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		}
		/*
		reg_addr_int = 0x258;
		reg_mask_u32 = ERIAR_MASK_1111;
		data_write_u32 = 0xFFFFFFFF;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		reg_addr_int = 0x25C;
		reg_mask_u32 = ERIAR_MASK_0011;
		data_write_u32 = 0x0000FFFF;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		*/

		/* (12) Enable VLAN De-tagging (IO offset 0xE0 bit6 set to 1) */
		RTL_W32(CPlusCmd, RTL_R32(CPlusCmd) | RxVlan);

		/* (13-B) Realwow2 setting */
		/* 13B-1. clear previous wakeup type: OCP 0xC0C4 = 0x00FF */
		reg_addr_u32 = 0x0000C0C4;
		data_write_u32 = 0x000000FF;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		/* 13B-2. clear previous ack miss counter: OCP 0xD2E8 = 0x0000 */
		reg_addr_u32 = 0x0000D2E8;
		data_write_u32 = 0x00000000;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		//			/* 13B-3. UDP port: OCP 0xCDB2 = SERVER1_PORT */
		reg_addr_u32 = 0x0000CDB2;
		data_write_u32 = UDP_PORT[0];
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		//			/* 13B-4. UDP port: OCP 0xCDB4 = SERVER2_PORT */
		reg_addr_u32 = 0x0000CDB4;
		data_write_u32 = UDP_PORT[1];
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		/* 13B-5. port enable: OCP 0xC0BE bit11=1, bit6=1, bit5=1 */
		reg_addr_u32 = 0x0000C0BE;
		data_read_u32 = r8168_mac_ocp_read(tp, reg_addr_u32);
		data_write_u32 = data_read_u32 | 0x00000860;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		/* 13B-6. port enable: OCP 0xC0C0 bit6=1, bit5=1 */
		reg_addr_u32 = 0x0000C0C0;
		data_read_u32 = r8168_mac_ocp_read(tp, reg_addr_u32);
		data_write_u32 = data_read_u32 | 0x00000060;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		/* 13B-7. rsvd_proxy_udp enable: OCP 0xC0B6 bit10=1 */
		reg_addr_u32 = 0x0000C0B6;
		data_read_u32 = r8168_mac_ocp_read(tp, reg_addr_u32);
		data_write_u32 = data_read_u32 | 0x00000400;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		//			/* 13B-8. ack miss timer: OCP 0xD2E6 = ACKMISS_TIME */
		reg_addr_u32 = 0x0000D2E6;
		data_write_u32 = ACKMISS_TIME;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		/* 13B-9. Disable ftr_mcu_en(ERI 0xDC[0] = 0) */
		reg_addr_int = 0x0F0;
		reg_mask_u32 = ERIAR_MASK_0001;
		data_read_u32 = rtl_eri_read(tp, 0x0DC, ERIAR_EXGMAC);
		data_write_u32 = data_read_u32 & (~0x00000001);
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		/* 13B-10. Enable ftr_mcu_en(ERI 0xDC[0] = 1) */
		reg_addr_int = 0x0F0;
		reg_mask_u32 = ERIAR_MASK_0001;
		data_read_u32 = rtl_eri_read(tp, 0x0DC, ERIAR_EXGMAC);
		data_write_u32 = data_read_u32 | 0x00000001;
		rtl_eri_write(tp, reg_addr_int, reg_mask_u32, data_write_u32, ERIAR_EXGMAC);
		//			/* 13B-11. rx length: OCP 0x6800 = ACKLEN[7:0]_WAKELEN[7:0] */
		reg_addr_u32 = 0x00006800;
		data_write_u32 = ack_len << 8 | wake_len;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		//			/* 13B-12. tx length: OCP 0x6802 = TX2LEN[7:0]_TX1LEN[7:0] */
		reg_addr_u32 = 0x00006802;
		data_write_u32 = tx2_len << 8 | tx1_len;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		//			/* 13B-13. wake packet UDP payload (64 bytes): OCP 0x6808-6847 */
		for (u32i = 0; u32i < 32; u32i++)
		{
			reg_addr_u32 = 0x00006808 + (u32i*2);
			data_write_u32 = 0x0000FFFF;
			r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		}
		//			/* 13B-14. ack packet UDP payload (48 bytes): OCP 0x6848-6877 */
		for (u32i = 0; u32i < 24; u32i++)
		{
			reg_addr_u32 = 0x00006848 + (u32i*2);
			data_write_u32 = 0x0000FFFF;
			r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		}
		//			/* 13B-15. tx1 whole packet excluding crc (100 bytes) : OCP 0x6880-68E3 */
		for (u32i = 0; u32i < 50; u32i++)
		{
			reg_addr_u32 = 0x00006880 + (u32i*2);
			data_write_u32 = 0x0000FFFF;
			r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		}
		//			/* 13B-16. tx2 whole packet excluding crc (100 bytes) : OCP 0x68E4-6947 */
		for (u32i = 0; u32i < 50; u32i++)
		{
			reg_addr_u32 = 0x000068E4 + (u32i*2);
			data_write_u32 = 0x0000FFFF;
			r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		}
		/* 13B-17. ack miss wakeup enable/disable : OCP 0xD2E4 bit 0 */
		reg_addr_u32 = 0x0000D2E4;
		data_read_u32 = r8168_mac_ocp_read(tp, reg_addr_u32);
		data_write_u32 = data_read_u32 | 0x00000001;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		/* 13B-18. RealWOWv2	enable/disable : OCP 0xC0BC = 0x0100 */
		reg_addr_u32 = 0x0000C0BC;
		data_write_u32 = 0x00000100;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		//			/* 13B-19. tx packet timer (in units of 8ms): OCP 0xE42A = TX_TIMER */
		reg_addr_u32 = 0x0000E42A;
		data_write_u32 = txpkt_timer;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);
		/* 13B-20. tx packet timer enable: OCP 0xE428 = 0x0019 */
		reg_addr_u32 = 0x0000E428;
		data_write_u32 = 0x00000019;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);

		/* (14) Enable now_is_oob (IO offset 0xd3 bit 7 set to 1b) */
		RTL_W8(MCU, RTL_R8(MCU) | NOW_IS_OOB | DIS_MCU_CLROOB);

		/* (15) set MACOCP 0xE8DE bit14 mcu_borw_en to 1b for modifying ShareFIFO's points */
		reg_addr_u32 = 0x0000E8DE;
		data_read_u32 = r8168_mac_ocp_read(tp, reg_addr_u32);
		data_write_u32 = data_read_u32 | 0x00004000;
		r8168_mac_ocp_write(tp, reg_addr_u32, data_write_u32);

		/* (16) Patch code to share FIFO if need (Yang Chang-Shiuan's patch code and BPs) */
		rtl_realwow_set(tp);

		/*	(17) Set rxdv_gated_en = 0 (IO 0xF2 bit3=0) */
		RTL_W8(MISC, RTL_R8(MISC) & ~RXDV_GATED_EN);

		/*	(18) Turn on RCR (IO offset 0x44 set to 0x0e) */
		RTL_W8(RxConfig, AcceptBroadcast | AcceptMulticast | AcceptMyPhys);


		/* enable WOL IP setting for PHY */
		//rtl_writephy(tp, 0x1f, 0x0bc4);
		//rtl_writephy(tp, 0x15, 0x0203);
		/* enable WOL IP setting for MAC */
		//RTL_W8(Cfg9346, Cfg9346_Unlock);
		/* enable now_is_oob */
		//RTL_W8(MCU, RTL_R8(MCU) | NOW_IS_OOB | DIS_MCU_CLROOB);
		//RTL_W8(Config5, RTL_R8(Config5) | LanWake);
		//RTL_W8(Config3, RTL_R8(Config3) | MagicPacket);
		//RTL_W8(Cfg9346, Cfg9346_Lock);
		//if (rtl_wol_pll_power_down(tp))
		//	return;
		//else
		//	eth_err("wol setting failed\n");
	} else {
	}

}
#endif

static void r8168_phy_power_up(struct rtl8168_private *tp)
{
	rtl8168_phy_int_disable(tp);
	rtl_phy_write(tp, 0, MII_BMCR, BMCR_ANENABLE);
	rtl8168_phy_int_enable(tp);
}

static void r8168_phy_power_down(struct rtl8168_private *tp)
{
	rtl8168_phy_int_disable(tp);
	rtl_phy_write(tp, 0, MII_BMCR, BMCR_PDOWN);
	rtl8168_phy_int_enable(tp);
}

static int _r8168_is_wol_enable(struct rtl8168_private *tp)
{
	u32 emcu_wol= 0;
	u32 feature = tp->features & RTL_FEATURE_WOL;

#if IS_ENABLED(CONFIG_RTK_KDRV_EMCU)
	if(!g_is_new_wol_flow)
		emcu_wol= emcu_WOL_event();
#endif
	eth_debug("%s feature=%u RTL_FEATURE_WOL=%d emcu_NET_event=%d\n"
			,__func__
			,tp->features
			,feature
			,emcu_wol);
	return feature || emcu_wol;
}


static bool r8168_is_link_up(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	if(tp->link_ok(ioaddr))
		return true;
	else
		return false;
}

static void r8168_pll_power_down(struct rtl8168_private *tp)
{
#if defined(ENABLE_WOL) || defined(ENABLE_WOW)
	if((BIT(SHARE_REG_CLOCK_BIT) != (rtd_inl(REG_SHARE_CLOCK) & BIT(SHARE_REG_CLOCK_BIT))))
	{
		eth_warning("%s, clock of reg[0x%08x] bit[%u] is disable, so no wol/wow\n",__func__,REG_SHARE_CLOCK,SHARE_REG_CLOCK_BIT);
		r8168_phy_power_down(tp);
	}
	// check wol enable/disable
	// read WOW/WOL flag in the share memory
	else if (_r8168_is_wol_enable(tp)
		&& r8168_is_link_up(tp))
	{
#ifdef ENABLE_WOL
		rtl_wol_set(tp, RTL8168_WOL_ENABLE);
#endif
#ifdef ENABLE_WOW
		rtl_wow_set(tp, RTL8168_WOL_ENABLE);
#endif
	}
	else
	{
		eth_info("[ETH] %s: r8168 phy power down\n", __func__);
		r8168_phy_power_down(tp);
	}
#else
		r8168_phy_power_down(tp);
#endif
}

static void r8168_pll_power_up(struct rtl8168_private *tp)
{
	r8168_phy_power_up(tp);
}

static bool rtl_pll_check_power_on(struct rtl8168_private *tp)
{
	int ret = 0;

	rtl8168_phy_int_disable(tp);
	ret = rtl_phy_read(tp, 0, MII_BMCR);
	rtl8168_phy_int_enable(tp);

	if (BMCR_PDOWN & ret)
		return true;
	else
		return false;
}

static void rtl_generic_op(struct rtl8168_private *tp,
		void (*op)(struct rtl8168_private *))
{
	if (op)
		op(tp);
}

static void rtl_pll_power_down(struct rtl8168_private *tp)
{
	rtl_generic_op(tp, tp->pll_power_ops.down);
}

static void rtl_pll_power_up(struct rtl8168_private *tp)
{
	rtl_generic_op(tp, tp->pll_power_ops.up);
}

static void rtl_init_pll_power_ops(struct rtl8168_private *tp)
{
	struct pll_power_ops *ops = &tp->pll_power_ops;

	ops->down	= r8168_pll_power_down;
	ops->up		= r8168_pll_power_up;
}

static void rtl_init_rxcfg(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(RxConfig, RX128_INT_EN | RX_DMA_BURST | RX_EARLY_OFF);
}

static void rtl8168_init_ring_indexes(struct rtl8168_private *tp)
{
	tp->dirty_tx = tp->cur_tx = tp->cur_rx = 0;
}

static void rtl_init_jumbo_ops(struct rtl8168_private *tp)
{
	struct jumbo_ops *ops = &tp->jumbo_ops;

	/* No action needed for jumbo frames with r8168soc. */
	ops->disable	= NULL;
	ops->enable	= NULL;
}

DECLARE_RTL_COND(rtl_chipcmd_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R8(ChipCmd) & CmdReset;
}

static void rtl_hw_reset(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	RTL_W8(ChipCmd, CmdReset);

	rtl_udelay_loop_wait_low(tp, &rtl_chipcmd_cond, 100, 100);
}

static void rtl_rx_close(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(RxConfig, RTL_R32(RxConfig) & ~RX_CONFIG_ACCEPT_MASK);
}

DECLARE_RTL_COND(rtl_txcfg_empty_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R32(TxConfig) & TXCFG_EMPTY;
}

static void rtl8168_hw_reset(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	/* Disable interrupts */
	rtl8168_irq_mask_and_ack(tp);

	rtl_rx_close(tp);

	RTL_W8(ChipCmd, RTL_R8(ChipCmd) | StopReq);
	eth_debug("%s, ChipCmd = %08x\n", __func__, RTL_R8(ChipCmd));
	rtl_udelay_loop_wait_high(tp, &rtl_txcfg_empty_cond, 100, 666);

	rtl_hw_reset(tp);
}

static void rtl_set_rx_tx_config_registers(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;

	/* Set DMA burst size and Interframe Gap Time */
	RTL_W32(TxConfig, (TX_DMA_BURST << TxDMAShift) |
			(InterFrameGap << TxInterFrameGapShift));
}

static void rtl_hw_start(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	tp->hw_start(dev);

	rtl_irq_enable_all(tp);
}

static void rtl_set_rx_tx_desc_registers(struct rtl8168_private *tp,
		u32 ioaddr)
{
	/*
	* Magic spell: some iop3xx ARM board needs the TxDescAddrHigh
	* register to be written before TxDescAddrLow to work.
	* Switching from MMIO to I/O access fixes the issue as well.
	*/
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	RTL_W32(TxDescStartAddrHigh, ((u64) tp->TxPhyAddr) >> 32);
#else
	RTL_W32(TxDescStartAddrHigh, 0);
#endif

	RTL_W32(TxDescStartAddrLow, ((u64) tp->TxPhyAddr) & DMA_BIT_MASK(32));
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	RTL_W32(RxDescAddrHigh, ((u64) tp->RxPhyAddr) >> 32);
#else
	RTL_W32(RxDescAddrHigh, 0);
#endif
	RTL_W32(RxDescAddrLow, ((u64) tp->RxPhyAddr) & DMA_BIT_MASK(32));
}

static void rtl_set_rx_max_size(u32 ioaddr, unsigned int rx_buf_sz)
{
	/* Low hurts. Let's disable the filtering. */
	RTL_W16(RxMaxSize, rx_buf_sz + 1);
}

static void rtl_set_rx_mode(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	u32 ioaddr = tp->base_addr;
	u32 mc_filter[2];	/* Multicast hash filter */
	int rx_mode;
	u32 tmp = 0;
	u32 data = 0;

	if (dev->flags & IFF_PROMISC) {
		/* Unconditionally log net taps. */
		eth_notice("Promiscuous mode enabled\n");
		rx_mode =
			AcceptBroadcast | AcceptMulticast | AcceptMyPhys |
			AcceptAllPhys;
		mc_filter[1] = mc_filter[0] = 0xffffffff;
	} else if ((netdev_mc_count(dev) > multicast_filter_limit) ||
			(dev->flags & IFF_ALLMULTI)) {
		/* Too many to filter perfectly -- accept all multicasts. */
		rx_mode = AcceptBroadcast | AcceptMulticast | AcceptMyPhys;
		mc_filter[1] = mc_filter[0] = 0xffffffff;
	} else {
		struct netdev_hw_addr *ha;

		rx_mode = AcceptBroadcast | AcceptMyPhys;
		mc_filter[1] = mc_filter[0] = 0;
		netdev_for_each_mc_addr(ha, dev) {
			int bit_nr = ether_crc(ETH_ALEN, ha->addr) >> 26;
			mc_filter[bit_nr >> 5] |= 1 << (bit_nr & 31);
			rx_mode |= AcceptMulticast;
		}
	}

	if (dev->features & NETIF_F_RXALL)
		rx_mode |= (AcceptErr | AcceptRunt);

	tmp = (RTL_R32(RxConfig) & ~RX_CONFIG_ACCEPT_MASK) | rx_mode;

	data = mc_filter[0];
	mc_filter[0] = swab32(mc_filter[1]);
	mc_filter[1] = swab32(data);

	RTL_W32(MAR0 + 4, mc_filter[1]);
	RTL_W32(MAR0 + 0, mc_filter[0]);

	RTL_W32(RxConfig, tmp);
}

#if 0
static void rtl_csi_write(struct rtl8168_private *tp, int addr, int value)
{
	if (tp->csi_ops.write)
		tp->csi_ops.write(tp, addr, value);
}

static u32 rtl_csi_read(struct rtl8168_private *tp, int addr)
{
	return tp->csi_ops.read ? tp->csi_ops.read(tp, addr) : ~0;
}

static void rtl_csi_access_enable(struct rtl8168_private *tp, u32 bits)
{
	u32 csi;

	csi = rtl_csi_read(tp, 0x070c) & 0x00ffffff;
	rtl_csi_write(tp, 0x070c, csi | bits);
}

static void rtl_csi_access_enable_1(struct rtl8168_private *tp)
{
	rtl_csi_access_enable(tp, 0x17000000);
}


DECLARE_RTL_COND(rtl_csiar_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R32(CSIAR) & CSIAR_FLAG;
}

static void r8168_csi_write(struct rtl8168_private *tp, int addr, int value)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(CSIDR, value);
	RTL_W32(CSIAR, CSIAR_WRITE_CMD | (addr & CSIAR_ADDR_MASK) |
			CSIAR_BYTE_ENABLE << CSIAR_BYTE_ENABLE_SHIFT);

	rtl_udelay_loop_wait_low(tp, &rtl_csiar_cond, 10, 100);
}

static u32 r8168_csi_read(struct rtl8168_private *tp, int addr)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(CSIAR, (addr & CSIAR_ADDR_MASK) |
			CSIAR_BYTE_ENABLE << CSIAR_BYTE_ENABLE_SHIFT);

	return rtl_udelay_loop_wait_high(tp, &rtl_csiar_cond, 10, 100) ?
		RTL_R32(CSIDR) : ~0;
}

static void rtl_init_csi_ops(struct rtl8168_private *tp)
{
	struct csi_ops *ops = &tp->csi_ops;

	ops->write	= r8168_csi_write;
	ops->read	= r8168_csi_read;
}

struct ephy_info {
	unsigned int offset;
	u16 mask;
	u16 bits;
};

static void rtl_ephy_init(struct rtl8168_private *tp, const struct ephy_info *e,
		int len)
{
	u16 w;

	while (len-- > 0) {
		w = (rtl_ephy_read(tp, e->offset) & ~e->mask) | e->bits;
		rtl_ephy_write(tp, e->offset, w);
		e++;
	}
}
#endif

#define R8168_CPCMD_QUIRK_MASK (\
		EnableBist | \
		Mac_dbgo_oe | \
		Force_half_dup | \
		Force_rxflow_en | \
		Force_txflow_en | \
		Cxpl_dbg_sel | \
		ASF | \
		PktCntrDisable | \
		Mac_dbgo_sel)

#if 0
static void rtl_hw_start_8168g_1(struct rtl8169_private *tp)
{
	u32 ioaddr = tp->base_addr;

	RTL_W32(TxConfig, RTL_R32(TxConfig) | TXCFG_AUTO_FIFO);

	rtl_eri_write(tp, 0xc8, ERIAR_MASK_0101, 0x080002, ERIAR_EXGMAC);
	rtl_eri_write(tp, 0xcc, ERIAR_MASK_0001, 0x38, ERIAR_EXGMAC);
	rtl_eri_write(tp, 0xd0, ERIAR_MASK_0001, 0x48, ERIAR_EXGMAC);
	rtl_eri_write(tp, 0xe8, ERIAR_MASK_1111, 0x00100006, ERIAR_EXGMAC);

	/*rtl_csi_access_enable_1(tp); */

	/*rtl_tx_performance_tweak(pdev, 0x5 << MAX_READ_REQUEST_SHIFT); */

	rtl_w1w0_eri(tp, 0xdc, ERIAR_MASK_0001, 0x00, 0x01, ERIAR_EXGMAC);
	rtl_w1w0_eri(tp, 0xdc, ERIAR_MASK_0001, 0x01, 0x00, ERIAR_EXGMAC);
	rtl_eri_write(tp, 0x2f8, ERIAR_MASK_0011, 0x1d8f, ERIAR_EXGMAC);

	RTL_W8(ChipCmd, CmdTxEnb | CmdRxEnb);
	RTL_W32(MISC, RTL_R32(MISC) & ~RXDV_GATED_EN);
	RTL_W8(MaxTxPacketSize, EarlySize);

	rtl_eri_write(tp, 0xc0, ERIAR_MASK_0011, 0x0000, ERIAR_EXGMAC);
	rtl_eri_write(tp, 0xb8, ERIAR_MASK_0011, 0x0000, ERIAR_EXGMAC);

	/* Adjust EEE LED frequency */
	RTL_W8(EEE_LED, RTL_R8(EEE_LED) & ~0x07);

	rtl_w1w0_eri(tp, 0x2fc, ERIAR_MASK_0001, 0x01, 0x06, ERIAR_EXGMAC);
	rtl_w1w0_eri(tp, 0x1b0, ERIAR_MASK_0011, 0x0000, 0x1000, ERIAR_EXGMAC);
}

static void rtl_hw_start_8168g_2(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	static const struct ephy_info e_info_8168g_2[] = {
		{ 0x00, 0x0000,	0x0008 },
		{ 0x0c, 0x3df0,	0x0200 },
		{ 0x19, 0xffff,	0xfc00 },
		{ 0x1e, 0xffff,	0x20eb }
	};

	rtl_hw_start_8168g_1(tp); /*? share tx/rx fifo enable?	*/

	/* disable aspm and clock request before access ephy */
	RTL_W8(Config2, RTL_R8(Config2) & ~ClkReqEn); /*? */
	RTL_W8(Config5, RTL_R8(Config5) & ~ASPM_en); /*? */
	/*rtl_ephy_init(tp, e_info_8168g_2, ARRAY_SIZE(e_info_8168g_2)); */
}
#endif
static void rtl_hw_start_8168(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	u32 ioaddr = tp->base_addr;
	u32 wol_mode = 0;

	RTL_W8(Cfg9346, Cfg9346_Unlock);

	RTL_W8(MaxTxPacketSize, TxPacketMax);
	
	rtl8168_set_tx_tdfnr(tp);
	
	rtl_set_rx_max_size(ioaddr, rx_buf_sz);

	tp->cp_cmd |= RTL_R16(CPlusCmd) | PktCntrDisable | INTT_1;

	RTL_W16(CPlusCmd, tp->cp_cmd); /*? no need */

	RTL_W16(IntrMitigate, 0x5151); /*? no need */

	rtl_set_rx_tx_desc_registers(tp, ioaddr);

	rtl_set_rx_tx_config_registers(tp);

	RTL_R8(IntrMask);

	/*	rtl_hw_start_8168g_2(tp); //? no need */
	RTL_W32(MISC, RTL_R32(MISC) & ~RXDV_GATED_EN);

	RTL_W8(Cfg9346, Cfg9346_Lock);

	RTL_W8(ChipCmd, CmdTxEnb | CmdRxEnb);

	rtl_set_rx_mode(dev);

	RTL_W16(MultiIntr, RTL_R16(MultiIntr) & 0xF000); /*? no need */

	/* Enable SCPU interrupt and disable KCPU interrupt */
	CRT_CLK_OnOff(ETH, CLK_ON, &wol_mode);
	rtl8168_enable_mac_phy_connect(tp);
}

static int rtl8168_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu < ETH_ZLEN ||
			new_mtu > JUMBO_9K)
		return -EINVAL;

	dev->mtu = new_mtu;
	netdev_update_features(dev);

	return 0;
}

static inline void rtl8168_make_unusable_by_asic(struct RxDesc *desc)
{
	desc->addr = cpu_to_le64(0x0badbadbadbadbadull);
	desc->opts1 &= ~cpu_to_le32(DescOwn | RsvdMask);
}

static void rtl8168_free_rx_databuff(struct rtl8168_private *tp,
		void **data_buff, struct RxDesc *desc)
{
	dma_unmap_single(&tp->dev->dev, le64_to_cpu(desc->addr), rx_buf_sz,
			DMA_FROM_DEVICE);

	kfree(*data_buff);
	*data_buff = NULL;
	rtl8168_make_unusable_by_asic(desc);
}

static inline void rtl8168_mark_to_asic(struct RxDesc *desc, u32 rx_buf_sz)
{
	u32 eor = le32_to_cpu(desc->opts1) & RingEnd;

	desc->opts1 = cpu_to_le32(DescOwn | eor | rx_buf_sz);
}

static inline void rtl8168_map_to_asic(struct RxDesc *desc, dma_addr_t mapping,
		u32 rx_buf_sz)
{
	desc->addr = cpu_to_le64(mapping);
	wmb();
	rtl8168_mark_to_asic(desc, rx_buf_sz);
}

static inline void *rtl8168_align(void *data)
{
	return (void *)ALIGN((long)data, 16);
}

static struct sk_buff *rtl8168_alloc_rx_data(struct rtl8168_private *tp,
		struct RxDesc *desc)
{
	void *data;
	dma_addr_t mapping;
	struct device *d = &tp->dev->dev;
	struct net_device *dev = tp->dev;
	int node = dev->dev.parent ? dev_to_node(dev->dev.parent) : -1;

	data = kmalloc_node(rx_buf_sz, GFP_KERNEL, node);
	if (!data)
		return NULL;

	if (rtl8168_align(data) != data) {
		kfree(data);
		data = kmalloc_node(rx_buf_sz + 15, GFP_KERNEL, node);
		if (!data)
			return NULL;
	}

	mapping = dma_map_single(d, rtl8168_align(data), rx_buf_sz,
			DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(d, mapping))) {
		if (net_ratelimit())
			eth_err("Failed to map RX DMA!\n");
		goto err_out;
	}

	rtl8168_map_to_asic(desc, mapping, rx_buf_sz);
	return data;

err_out:
	kfree(data);
	return NULL;
}

static void rtl8168_rx_clear(struct rtl8168_private *tp)
{
	unsigned int i;

	for (i = 0; i < NUM_RX_DESC; i++) {
		if (tp->Rx_databuff[i]) {
			rtl8168_free_rx_databuff(tp, tp->Rx_databuff + i,
					tp->RxDescArray + i);
		}
	}
}

static inline void rtl8168_mark_as_last_descriptor(struct RxDesc *desc)
{
	desc->opts1 |= cpu_to_le32(RingEnd);
}

static int rtl8168_rx_fill(struct rtl8168_private *tp)
{
	unsigned int i;

	for (i = 0; i < NUM_RX_DESC; i++) {
		void *data;

		if (tp->Rx_databuff[i])
			continue;

		data = rtl8168_alloc_rx_data(tp, tp->RxDescArray + i);
		if (!data) {
			rtl8168_make_unusable_by_asic(tp->RxDescArray + i);
			goto err_out;
		}
		tp->Rx_databuff[i] = data;
	}

	rtl8168_mark_as_last_descriptor(tp->RxDescArray + NUM_RX_DESC - 1);
	return 0;

err_out:
	rtl8168_rx_clear(tp);
	return -ENOMEM;
}

static int rtl8168_init_ring(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	rtl8168_init_ring_indexes(tp);

	memset(tp->tx_skb, 0x0, NUM_TX_DESC * sizeof(struct ring_info));
	memset(tp->Rx_databuff, 0x0, NUM_RX_DESC * sizeof(void *));

	return rtl8168_rx_fill(tp);
}

static void rtl8168_unmap_tx_skb(struct net_device *dev, struct ring_info *tx_skb,
		struct TxDesc *desc)
{
	unsigned int len = tx_skb->len;

	dma_unmap_single(&dev->dev, le64_to_cpu(desc->addr), len, DMA_TO_DEVICE);

	/*desc->opts1 = 0x00;
	desc->opts2 = 0x00;
	desc->addr = 0x00;*/
	tx_skb->len = 0;
}

static void rtl8168_tx_clear_range(struct rtl8168_private *tp, u32 start,
		unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; i++) {
		unsigned int entry = (start + i) % NUM_TX_DESC;
		struct ring_info *tx_skb = tp->tx_skb + entry;
		unsigned int len = tx_skb->len;

		if (len) {
			struct sk_buff *skb = tx_skb->skb;

			rtl8168_unmap_tx_skb(tp->dev, tx_skb,
					tp->TxDescArray + entry);
			if (skb) {
				tp->dev->stats.tx_dropped++;
				dev_kfree_skb(skb);
				tx_skb->skb = NULL;
			}
		}
	}
}

static void rtl8168_tx_clear(struct rtl8168_private *tp)
{
	rtl8168_tx_clear_range(tp, tp->dirty_tx, NUM_TX_DESC);
	tp->cur_tx = tp->dirty_tx = 0;
}

static void rtl_reset_work(struct rtl8168_private *tp)
{
	struct net_device *dev = tp->dev;
	int i;
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
	del_timer_sync(&tp->rx_timer); 
#endif
	napi_disable(&tp->napi);
	netif_stop_queue(dev);
	//synchronize_sched();
	synchronize_irq(dev->irq);

	rtl8168_hw_reset(tp);

	for (i = 0; i < NUM_RX_DESC; i++)
		rtl8168_mark_to_asic(tp->RxDescArray + i, rx_buf_sz);

	rtl8168_tx_clear(tp);
	if(tp->TxDescArray)
		memset(tp->TxDescArray, 0x0, R8168_TX_RING_BYTES);
	rtl8168_init_ring_indexes(tp);

	napi_enable(&tp->napi);
	rtl_hw_start(dev);
	netif_wake_queue(dev);
	rtl8168_check_link_status(dev, tp, tp->base_addr);
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
	mod_timer(&tp->rx_timer, jiffies + RTL8168_RX_TIMEOUT); 
#endif	
}

static void rtl8168_tx_index_mismatch_recovery(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	u32 ioaddr = tp->base_addr;
	if(tp->dirty_tx && !(RTL_R8(TxPoll) & NPQ)) {
		unsigned int dirty_tx;
		unsigned int pre_tx;
		unsigned int dirty_entry;
		unsigned int pre_entry;
		struct TxDesc *pre_desc;
		struct TxDesc *dirty_desc;

		eth_err("tx timeout, need to recovery\n");
		dirty_tx = tp->dirty_tx;
		pre_tx = tp->dirty_tx - 1;
		dirty_entry = dirty_tx % NUM_TX_DESC;
		pre_entry = pre_tx % NUM_TX_DESC;
		dirty_desc = tp->TxDescArray + dirty_entry;
		pre_desc = tp->TxDescArray + pre_entry;

		pre_desc->addr = dirty_desc->addr;
		pre_desc->opts2 = dirty_desc->opts2;
		if(dirty_entry == (NUM_TX_DESC - 1))
			pre_desc->opts1 = (dirty_desc->opts1 | cpu_to_le32(FirstFrag | LastFrag)) & (~(cpu_to_le32(RingEnd)));
		else if(pre_entry == (NUM_TX_DESC - 1))
			pre_desc->opts1 = (dirty_desc->opts1 | cpu_to_le32(FirstFrag | LastFrag)) | ((cpu_to_le32(RingEnd)));
		else
			pre_desc->opts1 = (dirty_desc->opts1 | cpu_to_le32(FirstFrag | LastFrag));

		wmb();
		RTL_W8(TxPoll, NPQ);
	}
}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static void rtl8168_tx_timeout(struct net_device *dev, unsigned int txqueue)
#else
static void rtl8168_tx_timeout(struct net_device *dev)
#endif
{
	struct rtl8168_private *tp = netdev_priv(dev);
	struct TxDesc* txd = NULL;
	int i = 0;
	int pp= 0;
	pp=tp->cur_tx % NUM_TX_DESC;
	eth_info("cur_tx=%u\n",pp);
	for(i= 0;i < NUM_TX_DESC;i++)
	{
		txd = tp->TxDescArray+i;
		if(txd)
		eth_info("tx desc[%d] addr=0x%llx opt1=0x%x opt2=0x%x\n"
				,i
				,txd->addr
				,le32_to_cpu(txd->opts1)
				,le32_to_cpu(txd->opts2));
	}

	rtl_schedule_task(tp, RTL_FLAG_TASK_RESET_PENDING);
}

static int rtl8168_xmit_frags(struct rtl8168_private *tp, struct sk_buff *skb,
		u32 *opts)
{
	struct skb_shared_info *info = skb_shinfo(skb);
	unsigned int cur_frag, entry;
	struct TxDesc * txd = NULL;
	struct device *d = &tp->dev->dev;

	entry = tp->cur_tx;
	for (cur_frag = 0; cur_frag < info->nr_frags; cur_frag++) {
		const skb_frag_t *frag = info->frags + cur_frag;
		dma_addr_t mapping;
		u32 status, len;
		void *addr;

		entry = (entry + 1) % NUM_TX_DESC;

		txd = tp->TxDescArray + entry;
		len = skb_frag_size(frag);
		addr = skb_frag_address(frag);
		mapping = dma_map_single(d, addr, len, DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(d, mapping))) {
			if (net_ratelimit())
				eth_err(
						"Failed to map TX fragments DMA!\n");
			goto err_out;
		}

		/* Anti gcc 2.95.3 bugware (sic) */
		status = opts[0] | len |
			(RingEnd * !((entry + 1) % NUM_TX_DESC));

		txd->opts1 = cpu_to_le32(status);
		txd->opts2 = cpu_to_le32(opts[1]);
		txd->addr = cpu_to_le64(mapping);

		tp->tx_skb[entry].len = len;
	}

	if (cur_frag) {
		tp->tx_skb[entry].skb = skb;
		txd->opts1 |= cpu_to_le32(LastFrag);
	}

	return cur_frag;

err_out:
	rtl8168_tx_clear_range(tp, tp->cur_tx + 1, cur_frag);
	return -EIO;
}

static bool rtl_skb_pad(struct sk_buff *skb)
{
	if (skb_padto(skb, ETH_ZLEN))
		return false;
	skb_put(skb, ETH_ZLEN - skb->len);
	return true;
}

static bool rtl_test_hw_pad_bug(struct rtl8168_private *tp, struct sk_buff *skb)
{
	return skb->len < ETH_ZLEN && false;
}

static inline bool rtl8168_tso_csum(struct rtl8168_private *tp,
		struct sk_buff *skb, u32 *opts)
{
	const struct rtl_tx_desc_info *info = &tx_desc_info;
	u32 mss = skb_shinfo(skb)->gso_size;
	int offset = info->opts_offset;

	if (mss) {
		opts[0] |= TD_LSO;
		opts[offset] |= min(mss, TD_MSS_MAX) << info->mss_shift;
	} else if (skb->ip_summed == CHECKSUM_PARTIAL) {
		const struct iphdr *ip = ip_hdr(skb);

		if (unlikely(rtl_test_hw_pad_bug(tp, skb)))
			return skb_checksum_help(skb) == 0 && rtl_skb_pad(skb);

		if (ip->protocol == IPPROTO_TCP)
			opts[offset] |= info->checksum.tcp;
		else if (ip->protocol == IPPROTO_UDP)
			opts[offset] |= info->checksum.udp;
		else
			WARN_ON_ONCE(1);
	} else {
		if (unlikely(rtl_test_hw_pad_bug(tp, skb)))
			return rtl_skb_pad(skb);
	}
	return true;
}

static netdev_tx_t rtl8168_start_xmit(struct sk_buff *skb,
		struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	unsigned int entry = tp->cur_tx % NUM_TX_DESC;
	struct TxDesc *txd = tp->TxDescArray + entry;
	u32 ioaddr = tp->base_addr;
	struct device *d = &tp->dev->dev;
	dma_addr_t mapping;
	u32 status, len;
	u32 opts[2];
	int frags;

	if (unlikely(!TX_FRAGS_READY_FOR(tp, skb_shinfo(skb)->nr_frags))) {
		eth_err("BUG! Tx Ring full when queue awake!\n");
		goto err_stop_0;
	}

	if (unlikely(le32_to_cpu(txd->opts1) & DescOwn)) {
		eth_err("DescOwn bit is set, Des = 0x%08x, entry = %d\n", le32_to_cpu(txd->opts1), entry);
		goto err_stop_0;
	}

	opts[1] = cpu_to_le32(rtl8168_tx_vlan_tag(skb));
	opts[0] = DescOwn;

	if (!rtl8168_tso_csum(tp, skb, opts)) /*? sw checksum */
		goto err_update_stats;

	len = skb_headlen(skb);
	mapping = dma_map_single(d, skb->data, len, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(d, mapping))) {
		if (net_ratelimit())
			eth_err("Failed to map TX DMA!\n");
		goto err_dma_0;
	}

	tp->tx_skb[entry].len = len;
	txd->addr = cpu_to_le64(mapping);

	frags = rtl8168_xmit_frags(tp, skb, opts);
	if (frags < 0)
		goto err_dma_1;
	else if (frags)
		opts[0] |= FirstFrag;
	else {
		opts[0] |= FirstFrag | LastFrag;
		tp->tx_skb[entry].skb = skb;
	}

	txd->opts2 = cpu_to_le32(opts[1]);

	skb_tx_timestamp(skb);

	wmb();

	/* Anti gcc 2.95.3 bugware (sic) */
	status = opts[0] | len | (RingEnd * !((entry + 1) % NUM_TX_DESC));
	txd->opts1 = cpu_to_le32(status);

	tp->cur_tx += frags + 1;

	wmb();

	RTL_W8(TxPoll, NPQ);

	if (!TX_FRAGS_READY_FOR(tp, MAX_SKB_FRAGS)) {
		/* Avoid wrongly optimistic queue wake-up: rtl_tx thread must
		* not miss a ring update when it notices a stopped queue.
		*/
		smp_wmb();
		netif_stop_queue(dev);
		/* Sync with rtl_tx:
		* - publish queue status and cur_tx ring index (write barrier)
		* - refresh dirty_tx ring index (read barrier).
		* May the current thread have a pessimistic view of the ring
		* status and forget to wake up queue, a racing rtl_tx thread
		* can't.
		*/
		smp_mb();
		if (TX_FRAGS_READY_FOR(tp, MAX_SKB_FRAGS))
			netif_wake_queue(dev);
		else
			rtl8168_tx_index_mismatch_recovery(dev);
	}

	return NETDEV_TX_OK;

err_dma_1:
	rtl8168_unmap_tx_skb(dev, tp->tx_skb + entry, txd);
err_dma_0:
	dev_kfree_skb(skb);
err_update_stats:
	dev->stats.tx_dropped++;
	return NETDEV_TX_OK;

err_stop_0:
	netif_stop_queue(dev);
	dev->stats.tx_dropped++;
	return NETDEV_TX_BUSY;
}

#if 0
static void rtl8168_pcierr_interrupt(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	struct pci_dev *pdev = tp->pci_dev;
	u16 pci_status, pci_cmd;

	pci_read_config_word(pdev, PCI_COMMAND, &pci_cmd);
	pci_read_config_word(pdev, PCI_STATUS, &pci_status);

	netif_err(tp, intr, dev, "PCI error (cmd = 0x%04x, status = 0x%04x)\n",
			pci_cmd, pci_status);

	/*
	* The recovery sequence below admits a very elaborated explanation:
	* - it seems to work;
	* - I did not see what else could be done;
	* - it makes iop3xx happy.
	*
	* Feel free to adjust to your needs.
	*/
	if (pdev->broken_parity_status)
		pci_cmd &= ~PCI_COMMAND_PARITY;
	else
		pci_cmd |= PCI_COMMAND_SERR | PCI_COMMAND_PARITY;

	pci_write_config_word(pdev, PCI_COMMAND, pci_cmd);

	pci_write_config_word(pdev, PCI_STATUS,
			pci_status & (PCI_STATUS_DETECTED_PARITY |
				PCI_STATUS_SIG_SYSTEM_ERROR | PCI_STATUS_REC_MASTER_ABORT |
				PCI_STATUS_REC_TARGET_ABORT | PCI_STATUS_SIG_TARGET_ABORT));

	/* The infamous DAC f*ckup only happens at boot time */
	if ((tp->cp_cmd & PCIDAC) && !tp->cur_rx) {
		u32 ioaddr = tp->base_addr;

		netif_info(tp, intr, dev, "disabling PCI DAC\n");
		tp->cp_cmd &= ~PCIDAC;
		RTL_W16(CPlusCmd, tp->cp_cmd);
		dev->features &= ~NETIF_F_HIGHDMA;
	}

	rtl8168_hw_reset(tp);

	/*rtl_schedule_task(tp, RTL_FLAG_TASK_RESET_PENDING); */
}
#endif

static void rtl_tx(struct net_device *dev, struct rtl8168_private *tp)
{
	unsigned int dirty_tx, tx_left;

	dirty_tx = tp->dirty_tx;
	smp_rmb();
	tx_left = tp->cur_tx - dirty_tx;

	while (tx_left > 0) {
		unsigned int entry = dirty_tx % NUM_TX_DESC;
		struct ring_info *tx_skb = tp->tx_skb + entry;
		u32 status;

		rmb();
		status = le32_to_cpu(tp->TxDescArray[entry].opts1);
		if (status & DescOwn)
			break;
		rtl8168_unmap_tx_skb(dev, tx_skb,
				tp->TxDescArray + entry);
		if (status & LastFrag) {
			u64_stats_update_begin(&tp->tx_stats.syncp);
			tp->tx_stats.packets++;
			tp->tx_stats.bytes += tx_skb->skb->len;
			u64_stats_update_end(&tp->tx_stats.syncp);
			dev_kfree_skb(tx_skb->skb);
			tx_skb->skb = NULL;
		}
		dirty_tx++;
		tx_left--;
	}

	if (tp->dirty_tx != dirty_tx) {
		tp->dirty_tx = dirty_tx;
		/* Sync with rtl8168_start_xmit:
		* - publish dirty_tx ring index (write barrier)
		* - refresh cur_tx ring index and queue status (read barrier)
		* May the current thread miss the stopped queue condition,
		* a racing xmit thread can only have a right view of the
		* ring status.
		*/
		smp_mb();
		if (netif_queue_stopped(dev) &&
				TX_FRAGS_READY_FOR(tp, MAX_SKB_FRAGS)) {
			netif_wake_queue(dev);
		}
		/*
		* 8168 hack: TxPoll requests are lost when the Tx packets are
		* too close. Let's kick an extra TxPoll request when a burst
		* of start_xmit activity is detected (if it is not detected,
		* it is slow enough). -- FR
		*/
		if (tp->cur_tx != dirty_tx) {
			u32 ioaddr = tp->base_addr;

			RTL_W8(TxPoll, NPQ);
		}
	}
}

static inline int rtl8168_fragmented_frame(u32 status)
{
	return (status & (FirstFrag | LastFrag)) != (FirstFrag | LastFrag);
}

static inline void rtl8168_rx_csum(struct sk_buff *skb, u32 opts1)
{
	u32 status = opts1 & RxProtoMask;

	if (((status == RxProtoTCP) && !(opts1 & TCPFail)) ||
			((status == RxProtoUDP) && !(opts1 & UDPFail)))
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else
		skb_checksum_none_assert(skb);
}

static struct sk_buff *rtl8168_try_rx_copy(void *data,
		struct rtl8168_private *tp,
		int pkt_size,
		dma_addr_t addr)
{
	struct sk_buff *skb;
	struct device *d = &tp->dev->dev;

	data = rtl8168_align(data);
	dma_sync_single_for_cpu(d, addr, pkt_size, DMA_FROM_DEVICE);
	prefetch(data);
	skb = netdev_alloc_skb_ip_align(tp->dev, pkt_size);
	if (skb)
		memcpy(skb->data, data, pkt_size);
	dma_sync_single_for_device(d, addr, pkt_size, DMA_FROM_DEVICE);

	return skb;
}

static int rtl_rx(struct net_device *dev, struct rtl8168_private *tp, u32 budget)
{
	unsigned int cur_rx, rx_left;
	unsigned int count;

	cur_rx = tp->cur_rx;

	for (rx_left = min(budget, NUM_RX_DESC); rx_left > 0; rx_left--, cur_rx++) {
		unsigned int entry = cur_rx % NUM_RX_DESC;
		struct RxDesc *desc = tp->RxDescArray + entry;
		u32 status;

		rmb();
		status = le32_to_cpu(desc->opts1) & tp->opts1_mask;

		if (status & DescOwn)
			break;
		if (unlikely(status & RxRES)) {
			eth_info("Rx ERROR. status = %08x\n",
					status);
			dev->stats.rx_errors++;
			if (status & (RxRWT | RxRUNT))
				dev->stats.rx_length_errors++;
			if (status & RxCRC)
				dev->stats.rx_crc_errors++;
			if (status & RxFOVF) {
				rtl_schedule_task(tp, RTL_FLAG_TASK_RESET_PENDING);
				dev->stats.rx_fifo_errors++;
			}
			if ((status & (RxRUNT | RxCRC)) &&
					!(status & (RxRWT | RxFOVF)) &&
					(dev->features & NETIF_F_RXALL))
				goto process_pkt;
		} else {
			struct sk_buff *skb;
			dma_addr_t addr;
			int pkt_size;

process_pkt:
			addr = le64_to_cpu(desc->addr);
			if (likely(!(dev->features & NETIF_F_RXFCS)))
				pkt_size = (status & 0x00003fff) - 4;
			else
				pkt_size = status & 0x00003fff;
			if(pkt_size <= 0) {
				unsigned int i = 0;
				eth_info("Rx ivalid pkt[%u], opts=%x, opts_mask=%x\n",entry, le32_to_cpu(desc->opts1), tp->opts1_mask);
				for(i = 0; i < 5; i++) {
					unsigned int c_id = (entry + i) % NUM_RX_DESC;
					eth_info("pkt[%d] opts1=%x, opts2=%x, addr=%llx\n",c_id, le32_to_cpu(desc->opts1), 
						le32_to_cpu(desc->opts2), le64_to_cpu(desc->addr));
				}
				goto release_descriptor;
			}

			/*
			* The driver does not support incoming fragmented
			* frames. They are seen as a symptom of over-mtu
			* sized frames.
			*/
			if (unlikely(rtl8168_fragmented_frame(status))) {
				dev->stats.rx_dropped++;
				dev->stats.rx_length_errors++;
				goto release_descriptor;
			}

			skb = rtl8168_try_rx_copy(tp->Rx_databuff[entry],
					tp, pkt_size, addr);
			if (!skb) {
				dev->stats.rx_dropped++;
				goto release_descriptor;
			}

			rtl8168_rx_csum(skb, status);
			skb_put(skb, pkt_size);
			skb->protocol = eth_type_trans(skb, dev);

			rtl8168_rx_vlan_tag(desc, skb);

			napi_gro_receive(&tp->napi, skb);

			u64_stats_update_begin(&tp->rx_stats.syncp);
			tp->rx_stats.packets++;
			tp->rx_stats.bytes += pkt_size;
			u64_stats_update_end(&tp->rx_stats.syncp);
		}
release_descriptor:
		desc->opts2 = 0;
		wmb();
		rtl8168_mark_to_asic(desc, rx_buf_sz);
	}

	count = cur_rx - tp->cur_rx;
	tp->cur_rx = cur_rx;

	return count;
}

static irqreturn_t rtl8168_interrupt(int irq, void *dev_instance)
{
	struct net_device *dev = dev_instance;
	struct rtl8168_private *tp = netdev_priv(dev);
	int handled = 0;
	u16 status;
	status = rtl_get_events(tp);
	if (status && status != 0xffff) {
		status &= RTL_EVENT_NAPI | tp->event_slow;
		if (status) {
			handled = 1;

			rtl_irq_disable(tp);
			wmb();
			napi_schedule(&tp->napi);
		}
	}
	return IRQ_RETVAL(handled);
}

/*
 * Workqueue context.
 */
static void rtl_slow_event_work(struct rtl8168_private *tp)
{
	struct net_device *dev = tp->dev;
	u16 status;

	status = rtl_get_events(tp) & tp->event_slow;
	rtl_ack_events(tp, status);

	/*if (unlikely(status & SYSErr)) */
	/*	rtl8168_pcierr_interrupt(dev); */

	if (status & LinkChg)
	{
		__rtl8168_check_link_status(dev, tp, tp->base_addr, true);
		if(tp->link_ok(tp->base_addr))
		{
			rtl_schedule_task(tp, RTL_FLAG_TASK_RESET_PENDING);
		}
	}
	rtl_irq_enable_all(tp);
}

#if !NWAY_ENABLE
static void rtl_phy_work(struct rtl8168_private *tp)
{
	struct timer_list *timer = &tp->timer;
	u32 ioaddr = tp->base_addr;
	unsigned long timeout = RTL8168_PHY_TIMEOUT;
	struct net_device *dev = tp->dev;

	rtl8168_phy_int_disable(tp);
	if (tp->phy_reset_pending(tp)) {
		/*
		* A busy loop could burn quite a few cycles on nowadays CPU.
		* Let's delay the execution of the timer for a few ticks.
		*/
		timeout = HZ/10;
		rtl8168_phy_int_enable(tp);
		goto out_mod_timer;
	}

	rtl8168_phy_int_enable(tp);
	if (tp->link_ok(ioaddr))
		return;

	eth_warning("PHY reset until link up\n");

	rtl8168_phy_int_disable(tp);
#ifdef ENABLE_ALDPS
	tp->phy_set_aldps(tp, 1);
#endif
	tp->phy_enhance_performance(tp, 1);

	if(tp->autoneg == AUTONEG_DISABLE)
	{
		rtl8168_phy_reset(dev, tp);

		//eth_debug("==> %s:%s N-way %d \n",tp->dev->name,__FUNCTION__, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

		rtl8168_set_speed(dev, AUTONEG_ENABLE, SPEED_100, DUPLEX_FULL,
				ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
				ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full |
				(tp->mii.supports_gmii ?
				ADVERTISED_1000baseT_Half |
				ADVERTISED_1000baseT_Full : 0));

		tp->autoneg = AUTONEG_ENABLE;
		rtl8168_phy_int_enable(tp);

		//rtl_pll_power_up(tp);
		return;
	}
	rtl8168_phy_int_enable(tp);

out_mod_timer:
	mod_timer(timer, jiffies + timeout);
}
#endif

static void rtl_task(struct work_struct *work)
{
	static const struct {
		int bitnr;
		void (*action)(struct rtl8168_private *);
	} rtl_work[] = {
		/* XXX - keep rtl_slow_event_work() as first element. */
		{ RTL_FLAG_TASK_SLOW_PENDING,	rtl_slow_event_work },
		{ RTL_FLAG_TASK_RESET_PENDING,	rtl_reset_work },
		{ RTL_FLAG_TASK_PHY_PENDING,	rtl_phy_work }
	};
	struct rtl8168_private *tp =
		container_of(work, struct rtl8168_private, wk.work);
	struct net_device *dev = tp->dev;
	int i;

	rtl_lock_work(tp);

	if (!netif_running(dev) ||
			!test_bit(RTL_FLAG_TASK_ENABLED, tp->wk.flags))
		goto out_unlock;

	for (i = 0; i < ARRAY_SIZE(rtl_work); i++) {
		bool pending;

		pending = test_and_clear_bit(rtl_work[i].bitnr, tp->wk.flags);
		if (pending)
			rtl_work[i].action(tp);
	}

out_unlock:
	rtl_unlock_work(tp);
}

static int rtl8168_poll(struct napi_struct *napi, int budget)
{
	struct rtl8168_private *tp = container_of(napi, struct rtl8168_private, napi);
	struct net_device *dev = tp->dev;
	u16 enable_mask = RTL_EVENT_NAPI | tp->event_slow;
	int work_done= 0;
	u16 status;

	/* Check interrupt status */
	status = rtl_get_events(tp);
	rtl_ack_events(tp, status & ~tp->event_slow);

	work_done = rtl_rx(dev, tp, (u32) budget);

	if (status & RTL_EVENT_NAPI_TX)
		rtl_tx(dev, tp);

	if (status & tp->event_slow) {
		enable_mask &= ~tp->event_slow;

		rtl_schedule_task(tp, RTL_FLAG_TASK_SLOW_PENDING);
	}

	if (work_done < budget) {
		napi_complete(napi);

		rtl_irq_enable(tp, enable_mask);
	}

	return work_done;
}

static void rtl8168_rx_missed(struct net_device *dev, u32 ioaddr)
{
	/*	struct rtl8168_private *tp = netdev_priv(dev); */

	return;

	dev->stats.rx_missed_errors += (RTL_R32(RxMissed) & 0xffffff);
	RTL_W32(RxMissed, 0);
}

static void rtl8168_down(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	u32 ioaddr = tp->base_addr;

#if !NWAY_ENABLE
	del_timer_sync(&tp->timer);
#endif
	del_timer_sync(&tp->link_check_timer);
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
	del_timer_sync(&tp->rx_timer);
#endif
	napi_disable(&tp->napi);
	netif_stop_queue(dev);

	rtl8168_hw_reset(tp);
	/*
	* At this point device interrupts can not be enabled in any function,
	* as netif_running is not true (rtl8168_interrupt, rtl8168_reset_task)
	* and napi is disabled (rtl8168_poll).
	*/
	rtl8168_rx_missed(dev, ioaddr);

	/* Give a racing hard_start_xmit a few cycles to complete. */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	synchronize_rcu();
#else
	synchronize_sched();
#endif

	rtl8168_tx_clear(tp);

	rtl8168_rx_clear(tp);

#if NWAY_ENABLE
	rtl_pll_power_down(tp);
#endif
}

static int rtl8168_close(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	u32 wol_mode = 0;

	eth_info("%s\n", __func__);
	del_timer_sync(&tp->link_check_timer);
	//eth_debug("==> %s:%s %d \n",tp->dev->name,__FUNCTION__,rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
	del_timer_sync(&tp->rx_timer);
#endif
	/*pm_runtime_get_sync(&pdev->dev); */

	/* Update counters before going down */
	rtl8168_update_counters(dev);

	rtl_lock_work(tp);
	clear_bit(RTL_FLAG_TASK_ENABLED, tp->wk.flags);

	rtl8168_down(dev);
	rtl_unlock_work(tp);

	free_irq(dev->irq, dev);

	CRT_CLK_OnOff(ETH, CLK_OFF, &wol_mode);

	dma_free_coherent(&dev->dev, R8168_RX_RING_BYTES, tp->RxDescArray,
			tp->RxPhyAddr);
	dma_free_coherent(&dev->dev, R8168_TX_RING_BYTES, tp->TxDescArray,
			tp->TxPhyAddr);
	tp->TxDescArray = NULL;
	tp->RxDescArray = NULL;
	/*pm_runtime_put_sync(&pdev->dev); */

	return 0;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void rtl8168_netpoll(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	rtl8168_interrupt(tp->pci_dev->irq, dev);
}
#endif

static int rtl_open(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	struct timer_list *timer = &tp->link_check_timer;
	u32 ioaddr = tp->base_addr;
	int retval = -ENOMEM;

	eth_info("%s\n", __func__);
	tp->link_start_time = jiffies_to_msecs(jiffies);
	//eth_debug("==> %s:%s %d \n",tp->dev->name,__FUNCTION__,rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

	/*pm_runtime_get_sync(&pdev->dev);	*/

	/*
	* Rx and Tx descriptors needs 256 bytes alignment.
	* dma_alloc_coherent provides more.
	*/
	tp->TxDescArray = dma_alloc_coherent(&dev->dev, R8168_TX_RING_BYTES,
			&tp->TxPhyAddr, GFP_ATOMIC);
	if (!tp->TxDescArray) {
		eth_err("TX can't allocate continue memory\n");
		goto err_pm_runtime_put;
	}
	/* dma_alloc_cohernet use GFP_ATOMIC, memory need to reset 0 */
	memset(tp->TxDescArray, 0x0, R8168_TX_RING_BYTES);

	tp->RxDescArray = dma_alloc_coherent(&dev->dev, R8168_RX_RING_BYTES,
			&tp->RxPhyAddr, GFP_ATOMIC);
	if (!tp->RxDescArray) {
		eth_err("RX can't allocate continue memory\n");
		goto err_free_tx_0;
	}
	/* dma_alloc_cohernet use GFP_ATOMIC, memory need to reset 0 */
	memset(tp->RxDescArray, 0x0, R8168_RX_RING_BYTES);

	retval = rtl8168_init_ring(dev);
	if (retval < 0)
		goto err_free_rx_1;

	INIT_WORK(&tp->wk.work, rtl_task);

	smp_mb();

	retval = request_irq(dev->irq, rtl8168_interrupt, 0, dev->name, dev);
	if (retval < 0)
		goto err_release_fw_2;

	rtl_lock_work(tp);

	set_bit(RTL_FLAG_TASK_ENABLED, tp->wk.flags);

	napi_enable(&tp->napi);

#ifdef ENABLE_EEEP
	if(tp->output_mode == OUTPUT_EMBEDDED_PHY)
		rtl8168_mac_eeep_patch_disable(tp);
	else
		r8168soc_eee_init(tp, false);
#endif

	rtl8168_phy_int_disable(tp);
	rtl_udelay_loop_wait_high(tp, &rtl_phy_status_cond, 100, 20);
	rtl8168_init_phy(dev, tp);
	rtl8168_phy_int_enable(tp);

	__rtl8168_set_features(dev, dev->features);

#if NWAY_ENABLE
	rtl_pll_power_up(tp);
#endif

	rtl_hw_start(dev);

	netif_start_queue(dev);

	rtl_unlock_work(tp);

	tp->saved_wolopts = 0;
	/*pm_runtime_put_noidle(&pdev->dev); */

	rtl8168_check_link_status(dev, tp, ioaddr);
	/* Check link status timer */
	mod_timer(timer, jiffies + RTL8168_LINK_TIMEOUT);
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
	mod_timer(&tp->rx_timer, jiffies + RTL8168_RX_TIMEOUT);
#endif
#if !NWAY_ENABLE
	if(tp->link_ok(tp->base_addr))
	{
		rtl_schedule_task(tp, RTL_FLAG_TASK_RESET_PENDING);
	}
#endif

out:
	return retval;

err_release_fw_2:
	rtl8168_rx_clear(tp);
err_free_rx_1:
	dma_free_coherent(&dev->dev, R8168_RX_RING_BYTES, tp->RxDescArray,
			tp->RxPhyAddr);
	tp->RxDescArray = NULL;
err_free_tx_0:
	dma_free_coherent(&dev->dev, R8168_TX_RING_BYTES, tp->TxDescArray,
			tp->TxPhyAddr);
	tp->TxDescArray = NULL;
err_pm_runtime_put:
	/*	pm_runtime_put_noidle(&pdev->dev); */
	goto out;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
static void
#else
static struct rtnl_link_stats64 *
#endif
rtl8168_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *stats)
{
	struct rtl8168_private *tp = netdev_priv(dev);
	u32 ioaddr = tp->base_addr;
	unsigned int start;

	if (netif_running(dev))
		rtl8168_rx_missed(dev, ioaddr);

	do {
		start = u64_stats_fetch_begin_irq(&tp->rx_stats.syncp);
		stats->rx_packets = tp->rx_stats.packets;
		stats->rx_bytes	= tp->rx_stats.bytes;
	} while (u64_stats_fetch_retry_irq(&tp->rx_stats.syncp, start));


	do {
		start = u64_stats_fetch_begin_irq(&tp->tx_stats.syncp);
		stats->tx_packets = tp->tx_stats.packets;
		stats->tx_bytes	= tp->tx_stats.bytes;
	} while (u64_stats_fetch_retry_irq(&tp->tx_stats.syncp, start));

	stats->rx_dropped	= dev->stats.rx_dropped;
	stats->tx_dropped	= dev->stats.tx_dropped;
	stats->rx_length_errors = dev->stats.rx_length_errors;
	stats->rx_errors	= dev->stats.rx_errors;
	stats->rx_crc_errors	= dev->stats.rx_crc_errors;
	stats->rx_fifo_errors	= dev->stats.rx_fifo_errors;
	stats->rx_missed_errors = dev->stats.rx_missed_errors;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0))
	return stats;
#endif
}

static struct kobject *eth_kobj;

static ssize_t rtl8168_link_recovery_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	return sprintf(buf, "link_recovery_enable = %d, link_recovery_count = %d\n", tp->link_recovery_enable, tp->link_recovery_count);
}

static ssize_t rtl8168_link_recovery_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	sscanf(buf, "%d", &tp->link_recovery_enable);

	return count;
}

#ifdef ENABLE_WOL
static ssize_t rtl8168_wol_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	u32 tmp;
	struct rtl8168_private *tp = netdev_priv(ndev);
	u32 wol_status = rtd_inl(SHARE_REG_WOL_EVENT);
	buf[0] = 0;

	snprintf(buf+ strlen(buf), PAGE_SIZE - strlen(buf), "wol_enable = %d, wol_interrupt = 0x%08x	\
				\n%s\n",
				_r8168_is_wol_enable(tp)	,
				wol_status,
				wol_status & BIT(SHARE_REG_WOL_EVENT_BIT)?
					"Receive wakeup event!!":"Doesn't receive wakeup event!!");
	
	tmp = r8168_mac_ocp_read(tp, 0xC0C2);
	if ((tmp & BIT(4)) != 0 &&	(tmp & BIT(0)) != 0) {
		tmp = r8168_mac_ocp_read(tp, 0xD23A) & 0x0F01;
		snprintf(buf+ strlen(buf), PAGE_SIZE - strlen(buf), "ETH Wake Reason : %s\n", (tmp & 0x1) ? "Magic Packet" : "mDNS Packet");
		return strlen(buf);
	}
	if(g_hw_mdns_offloading_wol_flow) {	
		rtl8168_get_hw_mdns_wakeup_information(tp, buf + strlen(buf), PAGE_SIZE - strlen(buf));
	}
	return strlen(buf);
}

static ssize_t rtl8168_wol_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	u32 ioaddr					= tp->base_addr;
	u32 wol_enable				= 0;
#ifdef CONFIG_PM
	pm_message_t pmstate;
#endif

	sscanf(buf, "%d", &wol_enable);
	if(wol_enable)
	tp->features = tp->features | RTL_FEATURE_WOL;
	else
	tp->features = tp->features & (~RTL_FEATURE_WOL);

	if(wol_enable) {
#ifdef CONFIG_PM
	pmstate.event = PM_EVENT_SUSPEND;
		/* Set share memory to 8051, for test */
		//rtd_setbits(REG_SHARE_WOL_CAP, BIT(WOL_CAP_BIT));
		rtl8168_suspend(network_devs, pmstate);
#endif
	}
	else {
		if(tp->output_mode == OUTPUT_EMBEDDED_PHY) {
			/* Clear wol interrupt status */
			RTL_W32(OCPDR, 0x6e410000);
			RTL_W32(RxConfig, 0x00000000);
			RTL_W32(OCPDR, 0x6e400000);
			RTL_W32(RDSAR1, 0x32000021);
			RTL_W32(OCPDR, 0x746f0000);
			RTL_W32(OCPDR, 0xee400001);
			RTL_W32(OCPDR, 0xf0031e00);
			rtd_outl(REG_WOW_GMAC_CLK, 0x0);
		} else {
			u32 wol_mode = 1;
			RTL_W32(OCPDR, 0x6e410000);
			RTL_W32(RxConfig, 0x00000000);
			RTL_W32(OCPDR, 0x6e400000);
			RTL_W32(RDSAR1, 0x32000021);
			RTL_W32(OCPDR, 0x746f0000);
			RTL_W32(OCPDR, 0xee400001);
			RTL_W32(OCPDR, 0xf0031e00);
			CRT_CLK_OnOff(ETH, CLK_ON, &wol_mode);
		}
#ifdef CONFIG_PM
		rtl8168_resume(network_devs);
#endif
	}

	return count;
}
#endif

#ifdef ENABLE_ALDPS
static ssize_t rtl8168_aldps_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	bool is_in_aldps_mode = false;
	rtl8168_phy_int_disable(tp);
	is_in_aldps_mode = rtl8168_xmii_is_aldps_mode(tp);
	rtl8168_phy_int_enable(tp);

	return snprintf(buf, PAGE_SIZE, "Ethernet link is %s aldps mode\n", is_in_aldps_mode ? "in" : "not in");
}

static ssize_t rtl8168_aldps_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	unsigned int on = 0;

	if (kstrtouint(buf, 10, &on) != 0)
		return -EINVAL;
	rtl8168_phy_int_disable(tp);
	rtl8168_phy_set_aldps(tp, on ? 1 : 0);
	rtl8168_phy_int_enable(tp);
	return count;
}
#endif

static ssize_t rtl8168_tapbin_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	int phy_register = 0x00;
	u32 phy_read_data = 0x0000;
	rtl8168_phy_int_disable(tp);
	/* change Page to 0x0bc0 */
	phy_register = 0x14;
	/* read reg20 bit[15:0] */
	phy_read_data = rtl_phy_read(tp, 0x0bc0, phy_register);
	phy_read_data = rtl_phy_read(tp, 0x0bc0, phy_register);
	rtl8168_phy_int_enable(tp);
	eth_info("Tapbin val = 0x%04x\n", phy_read_data);

	return sprintf(buf, "tapbin_val = 0x%04x\n", phy_read_data);
}

static ssize_t rtl8168_tapbin_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	int phy_register = 0x00;
	u32 phy_write_data = 0x0000;
	u32 phy_read_data = 0x0000;

	sscanf(buf, "0x%04x", &tapbin_val);
	rtl8168_phy_int_disable(tp);
	if(tapbin_val) {
		/* change Page to 0x0bc0 */
		/* set	bit[4] force R enable, set r value bit[3:0]=1 */
		phy_write_data = tapbin_val | BIT(4);
		phy_register = 0x14;
		rtl_phy_write(tp, 0x0bc0, phy_register, phy_write_data);
		/* read reg20 bit[15:0] */
		phy_read_data = rtl_phy_read(tp, 0x0bc0, phy_register);
		eth_info("Tapbin val = 0x%04x\n", phy_read_data);
	}
	rtl8168_phy_int_enable(tp);

	return count;
}
static ssize_t rtl8168_snr_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	rtl8168_phy_int_disable(tp);
	rtl_read_snr_val(tp);
	rtl8168_phy_int_enable(tp);

	return sprintf(buf, "snr_debug = %d\n", snr_debug);
}

static ssize_t rtl8168_snr_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	sscanf(buf, "%d", &snr_debug);

	return count;
}

static ssize_t rtl8168_phy_status_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	rtl_phy_status *phyStatus = &tp->phy_status;
	u32 ioaddr = tp->base_addr;

	rtl8168_phy_int_disable(tp);
	rtl_get_phy_status(tp);
	rtl8168_phy_int_enable(tp);

	return sprintf(buf, "MAC link status = 0x%08x, link %s		\
				\nPHY link status = 0x%4x, link %s	\
				\n%uMbps, %s-duplex			\
				\nPhy settings = 0x%4x			\
				\nadv_ability = 0x%4x, giga_adv_ability = 0x%04x	\
				\nPhy status = 0x%4x			\
				\nlock_main = 0x%4x, %s			\
				\npcs_state = 0x%4x\n",
			RTL_R8(PHYstatus), tp->link_ok(ioaddr) ? "up":"down",
			phyStatus->link_status, phyStatus->link_status & BIT(2) ? "up":"down",
			phyStatus->link_speed & BIT(5) ? 1000 : phyStatus->link_speed & BIT(4) ? 100 : 10,
			phyStatus->link_speed & BIT(3) ? "full": "half",
			phyStatus->phy_set, phyStatus->adv_ability, phyStatus->giga_adv_ability,
			phyStatus->phy_status, phyStatus->lock_main, phyStatus->lock_main & BIT(1) ? "abnormal":"normal",
			phyStatus->pcs_state);
}

static ssize_t rtl8168_linkup_time_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	return sprintf(buf, "Linkup time = %dms\n", tp->link_up_time);
}

static ssize_t rtl8168_avg_linkup_time_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	u32 avg_time = 0;

	if(avg_link.retry_count < 100) {
		return sprintf(buf, "Still running test: %d/100\n", avg_link.retry_count);
	}

	avg_time = avg_link.avg_linkup_time/100;

	return sprintf(buf, "Average linkup time = %dms\n", avg_time);
}

static ssize_t rtl8168_avg_linkup_time_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	sscanf(buf, "%d", &avg_link.avg_linkup_enable);
	rtl8168_phy_int_disable(tp);
	if(avg_link.avg_linkup_enable) {
		avg_link.retry_count = 0;
		avg_link.avg_linkup_time = 0;
		rtl8168_phy_reset(ndev, tp);
	}
	rtl8168_phy_int_enable(tp);
	return count;
}

static ssize_t rtl8168_tally_counter_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	/* Update Tally Counter */
	rtl8168_update_counters(ndev);
	return sprintf(buf, "TX packets:%lld, RX packets:%lld \
			\nTX errors:%lld, RX errors:%d \
			\nRX missed:%d, Align errors:%d \
			\nTX one-collision:%d, TX multi-collision:%d \
			\nRX unicast:%lld, RX broadcast:%lld RX multicast:%d \
			\nTX aborted:%d, TX underun:%d\n",
			le64_to_cpu(tp->counters.tx_packets),
			le64_to_cpu(tp->counters.rx_packets),
			le64_to_cpu(tp->counters.tx_errors),
			le32_to_cpu(tp->counters.rx_errors),
			le16_to_cpu(tp->counters.rx_missed),
			le16_to_cpu(tp->counters.align_errors),
			le32_to_cpu(tp->counters.tx_one_collision),
			le32_to_cpu(tp->counters.tx_multi_collision),
			le64_to_cpu(tp->counters.rx_unicast),
			le64_to_cpu(tp->counters.rx_broadcast),
			le32_to_cpu(tp->counters.rx_multicast),
			le16_to_cpu(tp->counters.tx_aborted),
			le16_to_cpu(tp->counters.tx_underun));
}

static ssize_t rtl8168_tally_counter_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	u32 ioaddr = tp->base_addr;
	u32 clear;

	sscanf(buf, "%d", &clear);

	if(clear) {
		/* Clear Tally Counter */
		RTL_W32(CounterAddrLow, BIT(0));
		eth_info("Clear Tally Counter\n");
	}

	return count;
}
static ssize_t rtl8168_mac_addr_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	return sprintf(buf, "%s: RTL8168 at 0x%08x, "
			"%02x:%02x:%02x:%02x:%02x:%02x, "
			"IRQ %d\n",
			ndev->name,
			tp->base_addr,
			ndev->dev_addr[0], ndev->dev_addr[1],
			ndev->dev_addr[2], ndev->dev_addr[3],
			ndev->dev_addr[4], ndev->dev_addr[5], ndev->irq);
}

static ssize_t rtl8168_wol_pattern_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	u32 enable;

	sscanf(buf, "%d", &enable);

	if(enable) {
		eth_info("enable wol pattern\n");
		tp->features = tp->features | RTL_FEATURE_WOL_PATTERN;
	}
	else
	{
		eth_info("disable wol pattern\n");
		tp->features = tp->features & ~RTL_FEATURE_WOL_PATTERN;
	}

	return count;
}

static ssize_t rtl8168_wol_pattern_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
#ifndef RTL_USE_NEW_CRC_WAKE_UP_METHOD
	int i = 0;
	buf[0] = 0;
	for(i = 0; i < NUM_WOL_PATTERN; i++) {
		int j = 0;
		snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "wol pattern %d, crc: 0x%04x, mask:", i, wol_pattern[i]);
		for(j = 0; j < 16; j++)
			snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "0x%02x,", wol_mask[i][j]);
		snprintf(buf + strlen(buf), PAGE_SIZE - strlen(buf), "\n");
	}
	return strlen(buf);
#else
	//TODO:we can show something, but Does it really have help for us?
	return 0;
#endif
}

static ssize_t rtl8168_mac_reg_dump_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	u32 ioaddr = tp->base_addr;
	int i=0;

	for(i=0;i<0x100;i++)
	{
		eth_warning("mac addr[0x%02x]=0x%08x\n",i,RTL_R8(i));
	}

	return 0;
}
static ssize_t rtl8168_desc_dump_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	struct TxDesc* txd = NULL;
	struct RxDesc* rxd = NULL;
	int i = 0;
	int pp= 0;
	pp=tp->cur_tx%NUM_TX_DESC;
	eth_info("cur_tx=%u\n",pp);
	pp=tp->cur_rx%NUM_RX_DESC;
	eth_info("cur_rx=%u\n",pp);
	for(i=0;i<NUM_TX_DESC;i++)
	{
		txd = tp->TxDescArray+i;
		if(txd)
		eth_info("tx desc[%d] addr=0x%llx opt1=0x%x opt2=0x%x\n"
				,i
				,txd->addr
				,le32_to_cpu(txd->opts1)
				,le32_to_cpu(txd->opts2));
	}

	for(i=0; i<NUM_RX_DESC; i++)
	{
		rxd = tp->RxDescArray+i;
		if(rxd)
		eth_info("rx desc[%d] addr=0x%llx opt1=0x%x opt2=0x%x\n"
				,i
				,rxd->addr
				,le32_to_cpu(rxd->opts1)
				,le32_to_cpu(rxd->opts2));
	}
	return 0;
}

static ssize_t rtl8168_wol_last_wake_reason_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	unsigned int wake_reason = 0;
	if(rtd_inl(ETH_WAKE_DUMMY_REG) & ETH_WOL_DUMMY_WAKE_REASON_EFFECTED)
		wake_reason = rtd_inl(ETH_WAKE_DUMMY_REG) & ETH_WOL_DUMMY_WAKE_REASON_MASK;
	return snprintf(buf, PAGE_SIZE, "%x\n", wake_reason);
}

static ssize_t rtl8168_wol_enable_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	return snprintf(buf, PAGE_SIZE, "wol:%s\n", (tp->features & RTL_FEATURE_WOL) ? "enable" : "disable");
}

static ssize_t rtl8168_wol_enable_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	struct rtl8168_private *tp = netdev_priv(ndev);
	u32 wol_enable				= 0;

	sscanf(buf, "%d", &wol_enable);
	if(wol_enable)
	tp->features = tp->features | RTL_FEATURE_WOL;
	else
	tp->features = tp->features & (~RTL_FEATURE_WOL);
	return count;
}

static ssize_t rtl8168_new_wol_flow_enable_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return snprintf(buf, PAGE_SIZE, "new_wol_flow:%u\n", g_is_new_wol_flow);
}

static ssize_t rtl8168_new_wol_flow_enable_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	u32 new_wol_flow				= 0;

	sscanf(buf, "%d", &new_wol_flow);
	if(new_wol_flow)
	g_is_new_wol_flow = 1;
	else
	g_is_new_wol_flow = 0;
	return count;
}


static ssize_t rtl8168_vcpu_mdns_offloading_flow_enable_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return snprintf(buf, PAGE_SIZE, "g_vcpu_mdns_offloading_wol_flow:%u\n", g_vcpu_mdns_offloading_wol_flow);
}

static ssize_t rtl8168_vcpu_mdns_offloading_flow_enable_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{

	u32 val				= 0;

	sscanf(buf, "%u", &val);
	if(val)
	g_vcpu_mdns_offloading_wol_flow = 1;
	else
	g_vcpu_mdns_offloading_wol_flow = 0;

	return count;
}

static ssize_t rtl8168_hw_mdns_offloading_flow_enable_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return snprintf(buf, PAGE_SIZE, "g_hw_mdns_offloading_wol_flow:%u\n", g_hw_mdns_offloading_wol_flow);
}

static ssize_t rtl8168_hw_mdns_offloading_flow_enable_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	u32 val				= 0;

	sscanf(buf, "%u", &val);
	if(val)
	g_hw_mdns_offloading_wol_flow = 1;
	else
	g_hw_mdns_offloading_wol_flow = 0;

	return count;
}

static ssize_t rtl8168_mdns_offloading_flow_enable_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
#ifdef CONFIG_SUPPORT_HW_MDNS_OFFLOADING_FUNCTION
	return snprintf(buf, PAGE_SIZE, "g_mdns_offloading_wol_flow:%u\n", g_hw_mdns_offloading_wol_flow);
#else
	return snprintf(buf, PAGE_SIZE, "g_mdns_offloading_wol_flow:%u\n", g_vcpu_mdns_offloading_wol_flow);
#endif
}

static ssize_t rtl8168_mdns_offloading_flow_enable_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	u32 val				= 0;

	sscanf(buf, "%u", &val);
#ifdef CONFIG_SUPPORT_HW_MDNS_OFFLOADING_FUNCTION	
	if(val)
	g_hw_mdns_offloading_wol_flow = 1;
	else
	g_hw_mdns_offloading_wol_flow = 0;
#else
	if(val)
	g_vcpu_mdns_offloading_wol_flow = 1;
	else
	g_vcpu_mdns_offloading_wol_flow = 0;	
#endif
	return count;
}

static struct kobj_attribute rtl8168_link_recovery =
__ATTR(link_recovery, S_IRUGO | S_IWUSR, rtl8168_link_recovery_show, rtl8168_link_recovery_store);
#ifdef ENABLE_WOL
static struct kobj_attribute rtl8168_wol =
__ATTR(wol, S_IRUGO | S_IWUSR, rtl8168_wol_show, rtl8168_wol_store);
#endif
#ifdef ENABLE_ALDPS
static struct kobj_attribute rtl8168_aldps =
__ATTR(aldps, S_IRUGO | S_IWUSR, rtl8168_aldps_show, rtl8168_aldps_store);
#endif
static struct kobj_attribute rtl8168_tapbin =
__ATTR(tapbin, S_IRUGO | S_IWUSR, rtl8168_tapbin_show, rtl8168_tapbin_store);
static struct kobj_attribute rtl8168_snr =
__ATTR(snr, S_IRUGO | S_IWUSR, rtl8168_snr_show, rtl8168_snr_store);
static struct kobj_attribute rtl8168_phy_status =
__ATTR(phy_status, S_IRUGO | S_IWUSR, rtl8168_phy_status_show, NULL);
static struct kobj_attribute rtl8168_linkup_time =
__ATTR(linkup_time, S_IRUGO | S_IWUSR, rtl8168_linkup_time_show, NULL);
static struct kobj_attribute rtl8168_avg_linkup_time =
__ATTR(avg_linkup_time, S_IRUGO | S_IWUSR, rtl8168_avg_linkup_time_show, rtl8168_avg_linkup_time_store);
static struct kobj_attribute rtl8168_tally_counter =
__ATTR(tally_counter, S_IRUGO | S_IWUSR, rtl8168_tally_counter_show, rtl8168_tally_counter_store);
static struct kobj_attribute rtl8168_mac_addr =
__ATTR(mac_addr, S_IRUGO | S_IWUSR, rtl8168_mac_addr_show, NULL);
static struct kobj_attribute rtl8168_desc_dump =
__ATTR(desc_dump, S_IRUGO, rtl8168_desc_dump_show, NULL);
static struct kobj_attribute rtl8168_mac_reg_dump =
__ATTR(mac_reg_dump, S_IRUGO, rtl8168_mac_reg_dump_show, NULL);
static struct kobj_attribute rtl8168_wol_pattern =
__ATTR(wol_pattern, S_IRUGO | S_IWUSR, rtl8168_wol_pattern_show, rtl8168_wol_pattern_store);
static struct kobj_attribute rtl8168_wol_last_wake_reason =
__ATTR(wol_last_wake_reason, S_IRUGO | S_IWUSR, rtl8168_wol_last_wake_reason_show, NULL);
static struct kobj_attribute rtl8168_wol_enable =
__ATTR(wol_enable, S_IRUGO | S_IWUSR, rtl8168_wol_enable_show, rtl8168_wol_enable_store);
static struct kobj_attribute rtl8168_new_wol_flow_enable =
__ATTR(new_wol_flow_enable, S_IRUGO | S_IWUSR, rtl8168_new_wol_flow_enable_show, rtl8168_new_wol_flow_enable_store);
static struct kobj_attribute rtl8168_vcpu_mdns_offloading_flow_enable =
__ATTR(vcpu_mdns_offloading_flow_enable, S_IRUGO | S_IWUSR, rtl8168_vcpu_mdns_offloading_flow_enable_show, rtl8168_vcpu_mdns_offloading_flow_enable_store);
static struct kobj_attribute rtl8168_wol_mdns_offload_protocol_data =
__ATTR(wol_mdns_offload_protocol_data, S_IRUGO | S_IWUSR, rtl8168_wol_mdns_offload_protocol_data_show, rtl8168_wol_mdns_offload_protocol_data_store);
static struct kobj_attribute rtl8168_wol_mdns_offload_passthrough =
__ATTR(wol_mdns_offload_passthrough, S_IRUGO | S_IWUSR, rtl8168_wol_mdns_offload_passthrough_show, rtl8168_wol_mdns_offload_passthrough_store);
static struct kobj_attribute rtl8168_wol_mdns_offload_wake_port =
__ATTR(wol_mdns_offload_wake_port, S_IRUGO | S_IWUSR, rtl8168_wol_mdns_offload_wake_port_show, rtl8168_wol_mdns_offload_wake_port_store);
static struct kobj_attribute rtl8168_wol_mdns_offload_passthrough_behaviour =
__ATTR(wol_mdns_offload_passthrough_behaviour, S_IRUGO | S_IWUSR, rtl8168_wol_mdns_offload_passthrough_behaviour_show, rtl8168_wol_mdns_offload_passthrough_behaviour_store);
static struct kobj_attribute rtl8168_etn_mdns_offfload_state =
__ATTR(wol_mdns_setOffloadState, S_IRUGO | S_IWUSR, rtl8168_etn_mdns_offfload_state_show, rtl8168_etn_mdns_offfload_state_store);
static struct kobj_attribute rtl8168_wol_ipv4 =
__ATTR(wol_ipv4, S_IRUGO | S_IWUSR, rtl8168_wol_ipv4_show, rtl8168_wol_ipv4_store);
static struct kobj_attribute rtl8168_wol_ipv6 =
__ATTR(wol_ipv6, S_IRUGO | S_IWUSR, rtl8168_wol_ipv6_show, rtl8168_wol_ipv6_store);
static struct kobj_attribute rtl8168_hw_mdns_offloading_flow_enable =
__ATTR(hw_mdns_offloading_flow_enable, S_IRUGO | S_IWUSR, rtl8168_hw_mdns_offloading_flow_enable_show, rtl8168_hw_mdns_offloading_flow_enable_store);
static struct kobj_attribute rtl8168_mdns_offloading_flow_enable =
__ATTR(mdns_offloading_flow_enable, S_IRUGO | S_IWUSR, rtl8168_mdns_offloading_flow_enable_show, rtl8168_mdns_offloading_flow_enable_store);

static struct attribute *attrs[] = {
	&rtl8168_link_recovery.attr,
#ifdef ENABLE_WOL
	&rtl8168_wol.attr,
#endif
#ifdef ENABLE_ALDPS
	&rtl8168_aldps.attr,
#endif
	&rtl8168_tapbin.attr,
	&rtl8168_snr.attr,
	&rtl8168_phy_status.attr,
	&rtl8168_linkup_time.attr,
	&rtl8168_avg_linkup_time.attr,
	&rtl8168_tally_counter.attr,
	&rtl8168_mac_addr.attr,
	&rtl8168_desc_dump.attr,
	&rtl8168_mac_reg_dump.attr,
	&rtl8168_wol_pattern.attr,
	&rtl8168_wol_last_wake_reason.attr,
	&rtl8168_wol_enable.attr,
	&rtl8168_new_wol_flow_enable.attr,
	&rtl8168_vcpu_mdns_offloading_flow_enable.attr,
	&rtl8168_wol_mdns_offload_protocol_data.attr,
	&rtl8168_wol_mdns_offload_passthrough.attr,
	&rtl8168_wol_mdns_offload_wake_port.attr,
	&rtl8168_wol_mdns_offload_passthrough_behaviour.attr,
	&rtl8168_etn_mdns_offfload_state.attr,
	&rtl8168_wol_ipv4.attr,
	&rtl8168_wol_ipv6.attr,
	&rtl8168_hw_mdns_offloading_flow_enable.attr,
	&rtl8168_mdns_offloading_flow_enable.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group eth_attr_group = {
	.attrs = attrs,
};

#ifdef CONFIG_PM
static struct task_struct *link_tsk;
static wait_queue_head_t wait;
static struct completion comp;

static int __rtl8168_link_recovery(void *arg)
{
	struct rtl8168_private *tp = (struct rtl8168_private*)arg;
	u32 ioaddr = tp->base_addr;
	u32 phyStatus = 0;
	u32 lockMainUC = 0;
	u32 ret = 0;
	do {
		phyStatus = RTL_R8(PHYstatus);
		rtl8168_phy_int_disable(tp);
		lockMainUC = rtl_phy_read(tp, 0x0a46, 0x15);
		rtl8168_phy_int_enable(tp);
		//eth_info("[ETH][%d] Phy status = 0x%x, lock_main_uc = 0x%04x\n", __LINE__, phyStatus, lockMainUC);

		if(lockMainUC & BIT(1)) {
			eth_info("[ETH][%d] Phy status = 0x%x, lock_main_uc = 0x%04x\n", __LINE__, phyStatus, lockMainUC);
			rtl8168_phy_int_disable(tp);
			rtl_phy_write(tp, 0x0a46, 0x15, lockMainUC & ~BIT(1));
			rtl8168_phy_int_enable(tp);
			lockMainUC = rtl_phy_read(tp, 0x0a46, 0x15);
			phyStatus = RTL_R8(PHYstatus);
			eth_info("[ETH][%d] Phy status = 0x%x, lock_main_uc = 0x%04x\n", __LINE__, phyStatus, lockMainUC);
			tp->link_recovery_count++;
			break;
		}
		ret = wait_event_interruptible_timeout(wait, (phyStatus & BIT(1)) == BIT(1), 1*HZ);

		if (signal_pending(current)) {
			eth_err("[ETH]%s:received a SIGKILL signal\n", __func__);
			break;
		}

		if(kthread_should_stop()) {
			eth_err("[ETH] thread stop\n");
			break;
		}
	} while(!(phyStatus & BIT(1)));

	eth_info("[ETH] resume thread end: %s leaved\n", __func__);
	complete_and_exit(&comp, 0);

	return 0;
}
#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
static void rtl8168_protocol_offload(struct rtl8168_private *tp);
#endif
static void rtl8168_net_suspend(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	//clear wake detect information
	rtl8168_cancel_mdns_packet_upload_work(dev);
	rtd_outl(ETH_WAKE_DUMMY_REG, rtd_inl(ETH_WAKE_DUMMY_REG) & (~0x4000FFFF));
	if(g_hw_mdns_offloading_wol_flow && g_vcpu_mdns_offloading_wol_flow)
		g_vcpu_mdns_offloading_wol_flow = false;

	if (!netif_running(dev)|| !test_bit(RTL_FLAG_TASK_ENABLED, tp->wk.flags))
	{
		eth_info("[ETH] %s: netif not running \n", __func__);
		r8168_phy_power_down(tp);
		return;
	}
	netif_device_detach(dev);
	netif_stop_queue(dev);

	rtl_lock_work(tp);
	napi_disable(&tp->napi);
	clear_bit(RTL_FLAG_TASK_ENABLED, tp->wk.flags);
	rtl_unlock_work(tp);
	
#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
	rtl8168_protocol_offload(tp);
#else
	rtl8168_hw_reset(tp);
#endif

	rtl_pll_power_down(tp);
}

//static int rtl8168_suspend(struct platform_device *dev, pm_message_t state) // RTK_patch: export this api
int rtl8168_suspend(struct platform_device *dev, pm_message_t state) // for WOL of the warm standby flow (suspend/resume)
{
	struct net_device *ndev = platform_get_drvdata(dev);
	struct rtl8168_private *tp = netdev_priv(ndev);

	eth_info("===>>>%s %d\n", __func__,__LINE__);
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
	del_timer_sync(&tp->rx_timer);
#endif
	eth_info("===>>>%s %d\n", __func__,__LINE__);

	if(link_tsk && tp->link_recovery_enable) {
		if(!completion_done(&comp)) {
			kthread_stop(link_tsk);
			wait_for_completion(&comp);
	eth_info("===>>>%s %d\n", __func__,__LINE__);
		}
		link_tsk = NULL;
	}

	eth_info("===>>>%s %d\n", __func__,__LINE__);
	rtl8168_net_suspend(ndev);
	eth_info("===>>>%s %d\n", __func__,__LINE__);

	return 0;
}

static void __rtl8168_resume(struct net_device *dev)
{
	struct rtl8168_private *tp = netdev_priv(dev);

	g_resume_first_check_delay=3500;
	tp->link_start_time = jiffies_to_msecs(jiffies);

	netif_device_attach(dev);

	rtl_pll_power_up(tp);
	rtl_rar_set(tp, tp->dev->dev_addr);

	rtl_lock_work(tp);
	napi_enable(&tp->napi);
	set_bit(RTL_FLAG_TASK_ENABLED, tp->wk.flags);
	rtl_unlock_work(tp);

	rtl_udelay_loop_wait_high(tp, &rtl_phy_status_cond, 100, 20);
	rtl_schedule_task(tp, RTL_FLAG_TASK_RESET_PENDING);
}

//static int rtl8168_resume(struct platform_device *dev) // RTK_patch: export this api
int rtl8168_resume(struct platform_device *dev) // for WOL of the warm standby flow (suspend/resume)
{
	struct net_device *ndev = platform_get_drvdata(dev);
	struct rtl8168_private *tp = netdev_priv(ndev);
	int ret;

	eth_info("%s sll\n", __func__);
	if(!(rtd_inl(PLLETN_GETN_RST_reg) & PLLETN_GETN_RST_rstn_getn_sys_mask))
		rtd_outl(PLLETN_GETN_RST_reg, PLLETN_GETN_RST_rstn_getn_sys_mask | PLLETN_GETN_RST_write_data_mask);

	rtl8168_check_mdns_packet_upload_flag(ndev);
	rtl8168_pre_phy_init(tp);

#ifdef CONFIG_CUSTOMER_TV006
	/* Customize LED */
	rtd_outl((SW_DEF_BASE_ADDR + CustomLED), 0x000670ca);
#endif
	/* mdc clock frequency increase */
	rtd_outl(SW_DEF_BASE_ADDR+OCPDR,0xef080040);

	if (netif_running(ndev))
		__rtl8168_resume(ndev);
	else if (!rtl_pll_check_power_on(tp))
	{
		eth_info("[ETH] : suspend power off to resume power on %s \n", __func__);
		rtl_pll_power_up(tp);
		rtl_rar_set(tp, tp->dev->dev_addr);
	}
#ifdef ENABLE_EEEP
	if(tp->output_mode == OUTPUT_EMBEDDED_PHY)
		rtl8168_mac_eeep_patch_disable(tp);
	else
		r8168soc_eee_init(tp, false);
#endif	
	rtl8168_phy_int_disable(tp);
	rtl8168_init_phy(ndev, tp);
	rtl8168_phy_int_enable(tp);


	if(tp->link_recovery_enable) {
		init_completion(&comp);
		init_waitqueue_head(&wait);

		link_tsk = kthread_run(__rtl8168_link_recovery, tp, "link_recovery");
		if (IS_ERR(link_tsk)) {
			ret = PTR_ERR(link_tsk);
			link_tsk = NULL;
			goto out;
		}
		eth_info("Create link recovery thread\n");

		return 0;
out:
		eth_err("create link recovery kthread failed\n");
		return ret;
	}

	return 0;
}

#if 0
static __maybe_unused int rtl8168_runtime_suspend(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);
	struct rtl8168_private *tp = netdev_priv(ndev);

	if (!tp->TxDescArray)
		return 0;

	rtl_lock_work(tp);
	tp->saved_wolopts = __rtl8168_get_wol(tp);
	__rtl8168_set_wol(tp, WAKE_ANY);
	rtl_unlock_work(tp);

	rtl8168_net_suspend(ndev);

	return 0;
}

static __maybe_unused int rtl8168_runtime_resume(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);
	struct rtl8168_private *tp = netdev_priv(ndev);

	if (!tp->TxDescArray)
		return 0;

	rtl_lock_work(tp);
	__rtl8168_set_wol(tp, tp->saved_wolopts);
	tp->saved_wolopts = 0;
	rtl_unlock_work(tp);

	rtl8168_mac_eeep_patch_disable(tp);
	rtl8168_init_phy(ndev, tp);

	__rtl8168_resume(ndev);

	return 0;
}

static __maybe_unused int rtl8168_runtime_idle(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);
	struct rtl8168_private *tp = netdev_priv(ndev);

	return tp->TxDescArray ? -EBUSY : 0;
}
#endif
#endif

static void rtl_shutdown(struct platform_device *dev)
{
	struct net_device *ndev = platform_get_drvdata(dev);
	//struct rtl8168_private *tp = netdev_priv(ndev);
	/*struct device *d = &pdev->dev; */

	/*pm_runtime_get_sync(d); */

	rtl8168_net_suspend(ndev);
#if 0
	/* Restore original MAC address */
	rtl_rar_set(tp, ndev->perm_addr);

	rtl8168_hw_reset(tp);

	if (system_state == SYSTEM_POWER_OFF) {
		if (__rtl8168_get_wol(tp) & WAKE_ANY) {
			rtl_wol_suspend_quirk(tp);
		}
	}
#endif
	/*pm_runtime_put_noidle(d); */
}

static const struct net_device_ops rtl_netdev_ops = {
	.ndo_open		= rtl_open,
	.ndo_stop		= rtl8168_close,
	.ndo_get_stats64	= rtl8168_get_stats64,
	.ndo_start_xmit		= rtl8168_start_xmit,
	.ndo_tx_timeout		= rtl8168_tx_timeout,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_change_mtu		= rtl8168_change_mtu,
	.ndo_fix_features	= rtl8168_fix_features,
	.ndo_set_features	= rtl8168_set_features,
	.ndo_set_mac_address	= rtl_set_mac_address,
	.ndo_do_ioctl		= rtl8168_ioctl,
	.ndo_set_rx_mode	= rtl_set_rx_mode,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= rtl8168_netpoll,
#endif

};

#if 0
/* Cfg9346_Unlock assumed. */
static unsigned rtl_try_msi(struct rtl8168_private *tp,
		unsigned features)
{
	u32 ioaddr = tp->base_addr;
	unsigned msi = 0;
	u8 cfg2;

	cfg2 = RTL_R8(Config2) & ~MSIEnable;
	if (features & RTL_FEATURE_MSI) {
#if 0
		cfg2 |= MSIEnable;
		msi = RTL_FEATURE_MSI;
#endif
	}

	return msi;
}
#endif

#if defined(ENABLE_WOW) || defined(RTL_USE_NEW_CRC_WAKE_UP_METHOD)
DECLARE_RTL_COND(rtl_link_list_ready_cond)
{
	u32 ioaddr = tp->base_addr;

	return RTL_R8(MCU) & LINK_LIST_RDY;
}
#endif

DECLARE_RTL_COND(rtl_rxtx_empty_cond)
{
	u32 ioaddr = tp->base_addr;

	return (RTL_R8(MCU) & RXTX_EMPTY) == RXTX_EMPTY;
}

#ifdef RTL_USE_NEW_CRC_WAKE_UP_METHOD
/* protocol offload driver flow */
static void rtl8168_protocol_offload(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
	u32 tmp;

	/* Disable interrupts */
	rtl8168_irq_mask_and_ack(tp);

	/* Set Now_is_OOB = 0 */
	RTL_W8(MCU, RTL_R8(MCU) & ~NOW_IS_OOB);

	/* Flow control related (only for OOB) */
	/* RX FIFO threshold */
	r8168_mac_ocp_write(tp, 0xC0A0, 0x0003);
	r8168_mac_ocp_write(tp, 0xC0A2, 0x0180);
	r8168_mac_ocp_write(tp, 0xC0A4, 0x004A);
	r8168_mac_ocp_write(tp, 0xC0A8, 0x005A);

	/* Turn off RCR (IO offset 0x44 set to 0) */
	rtl_rx_close(tp);

	RTL_W8(ChipCmd, RTL_R8(ChipCmd) | StopReq);
	eth_debug("%s, ChipCmd = %08x\n", __func__, RTL_R8(ChipCmd));
	rtl_udelay_loop_wait_high(tp, &rtl_txcfg_empty_cond, 100, 666);

	rtl_hw_reset(tp);

	/* Set rxdv_gated_en (IO 0xF2 bit3 = 1) */
	RTL_W32(MISC, RTL_R32(MISC) | RXDV_GATED_EN);

	/* check FIFO empty (IO 0xD2 bit[12:13]) */
	if (!rtl_udelay_loop_wait_high(tp, &rtl_rxtx_empty_cond, 100, 42))
		return;

	/* disable Tx/Rx enable = 0 (IO 0x36 bit[10:11]=0b) */
	RTL_W8(ChipCmd, RTL_R8(ChipCmd) & ~(CmdTxEnb | CmdRxEnb));
	usleep_range(1000, 1100);

	/* check link_list ready =1 (IO 0xD2 bit9=1) */
	if (!rtl_udelay_loop_wait_high(tp, &rtl_link_list_ready_cond, 100, 42))
		return;

	/* set re_ini_ll =1 (MACOCP : 0xE8DE bit15=1) */
	tmp = r8168_mac_ocp_read(tp, 0xE8DE);
	tmp |= BIT(15);
	r8168_mac_ocp_write(tp, 0xE8DE, tmp);

	/* check link_list ready =1 (IO 0xD2 bit9=1) */
	if (!rtl_udelay_loop_wait_high(tp, &rtl_link_list_ready_cond, 100, 42))
		return;

	/* Setting RMS (IO offset 0xdb-0xda set to 0x05F3) */
	rtl_set_rx_max_size(ioaddr, 0x05F3);

	/* Enable VLAN De-tagging (IO offset 0xE0 bit6 set to 1) */
	RTL_W16(CPlusCmd, RTL_R16(CPlusCmd) | RxVlan);

	/* Enable now_is_oob (IO offset 0xd3 bit 7 set to 1b) */
	RTL_W8(MCU, RTL_R8(MCU) | NOW_IS_OOB);

	/*	set	MACOCP 0xE8DE bit14 mcu_borw_en to 1b for modifying ShareFIFO's points */
	tmp = r8168_mac_ocp_read(tp, 0xE8DE);
	tmp |= BIT(14); /* borrow 8K SRAM */
	r8168_mac_ocp_write(tp, 0xE8DE, tmp);

	/* Patch code to share FIFO if need */

	/* Set rxdv_gated_en = 0 (IO 0xF2 bit3=0) */
	RTL_W32(MISC, RTL_R32(MISC) & ~RXDV_GATED_EN);

	/* Turn on RCR (IO offset 0x44 set to 0x0e) */
	tmp = RTL_R32(RxConfig) & ~RX_CONFIG_ACCEPT_MASK;
	tmp |= AcceptBroadcast | AcceptMulticast | AcceptMyPhys;
	RTL_W32(RxConfig, tmp);

	/* Set Multicast Registers to accept all addresses */
	RTL_W32(MAR0, 0xFFFFFFFF);
	RTL_W32(MAR0 + 4, 0xFFFFFFFF);
}
#endif
static void rtl_hw_init_8168g(struct rtl8168_private *tp)
{
	u32 ioaddr = tp->base_addr;
#ifdef ENABLE_WOW
	u32 data;
#endif

	RTL_W32(MISC, RTL_R32(MISC) | RXDV_GATED_EN);

	/* Check TX/RX fifo empty */
	if (!rtl_udelay_loop_wait_high(tp, &rtl_txcfg_empty_cond, 100, 42))
		return;

	if (!rtl_udelay_loop_wait_high(tp, &rtl_rxtx_empty_cond, 100, 42))
		return;

	/* Disable TX/RX */
	RTL_W8(ChipCmd, RTL_R8(ChipCmd) & ~(CmdTxEnb | CmdRxEnb));
	msleep(1);

#ifdef ENABLE_WOW
	/* Exit oob */
	RTL_W8(MCU, RTL_R8(MCU) & ~NOW_IS_OOB);

	data = r8168_mac_ocp_read(tp, 0xe8de);
	data &= ~(1 << 14);
	r8168_mac_ocp_write(tp, 0xe8de, data);

	if (!rtl_udelay_loop_wait_high(tp, &rtl_link_list_ready_cond, 100, 42))
		return;

	data = r8168_mac_ocp_read(tp, 0xe8de);
	data |= (1 << 15);
	r8168_mac_ocp_write(tp, 0xe8de, data);

	if (!rtl_udelay_loop_wait_high(tp, &rtl_link_list_ready_cond, 100, 42))
		return;
#endif
}
static void rtl_hw_initialize(struct rtl8168_private *tp)
{
	rtl_hw_init_8168g(tp);
}

#ifdef CONFIG_RTK_KDRV_R8168
static int __init rtl8168_enable(char *cpu)
{
	if(strcmp(cpu, "scpu") == 0 || strcmp(cpu, "SCPU") == 0) {
		eth_init = 1;
	}
	else {
		eth_init = 0;
	}
	return 1;
}
__setup("eth_cpu=", rtl8168_enable);

static int __init rtl8168_set_tapbin(char *val)
{
	char *ptr;

	tapbin_val = simple_strtoul(val, &ptr, 16);
	return 1;
}
__setup("tapbin=", rtl8168_set_tapbin);

static int __init rtl8168_snr_debug(char *val)
{
	if(strncmp(val, "1", 1) == 0) {
		snr_debug = RTL8168_SNR_ENABLE;
	}else {
		snr_debug = RTL8168_SNR_DISABLE;
	}
	return 1;
}
__setup("snr_debug=", rtl8168_snr_debug);
#else
int parse_rtl8168_enable(void)
{
	char strings[16] = {0};

	if(rtk_parse_commandline_equal("eth_cpu", strings,sizeof(strings)) == 0)
	{
		eth_err("Error : can't get eth_cpu from bootargs\n");
		return -1;
	}

	if(strcmp(strings, "scpu") == 0 || strcmp(strings, "SCPU") == 0) {
		eth_init = 1;
	}
	else {
		eth_init = 0;
	}

	return 0;
}

int parse_rtl8168_set_tapbin(void)
{
	int err	= 0;
	ulong val	= 0;
	char strings[16] = {0};

	if(rtk_parse_commandline_equal("tapbin", strings,sizeof(strings)) == 0)
	{
		eth_err("Error : can't get set_tapbin from bootargs\n");
		return -1;
	}

	err = kstrtoul(strings, 16, &val);
	if(err)
		return err;
	tapbin_val = (u32)val;

	return 0;
}

int parse_rtl8168_snr_debug(void)
{
	int err	= 0;
	ulong val	= 0;
	char strings[16] = {0};

	if(rtk_parse_commandline_equal("snr_debug", strings,sizeof(strings)) == 0)
	{
		eth_err("Error : can't get snr_debug from bootargs\n");
		return -1;
	}

	err = kstrtoul(strings, 16, &val);
	if(err)
		return err;
	snr_debug = val?RTL8168_SNR_ENABLE:RTL8168_SNR_DISABLE;

	return 0;
}
#endif
#ifdef ENABLE_EEEP
static void r8168soc_eee_init(struct rtl8168_private *tp, bool enable)
{
#if 0
	if (tp->output_mode == OUTPUT_EMBEDDED_PHY) 
		return;
	if (enable) {
		if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
			/* enable eee auto fallback function */
			rtl_phy_write(tp, 0x0a4b, 17,
				rtl_phy_read(tp, 0x0a4b, 17) | BIT(2));
			/* 1000M/100M EEE capability */
			rtl_mmd_write(tp, 0x7, 60, (BIT(2) | BIT(1)));
			/* 10M EEE */
			rtl_phy_write(tp, 0x0a43, 25,
				rtl_phy_read(tp, 0x0a43, 25) | BIT(4) | BIT(2));
			/* keep RXC in LPI mode */
			rtl_mmd_write(tp, 0x3, 0,
				rtl_mmd_read(tp, 0x3, 0) & ~BIT(10));
		} else { /* RGMII */
			/* enable eee auto fallback function */
			rtl_phy_write(tp, 0x0a4b, 17,
				rtl_phy_read(tp, 0x0a4b, 17) | BIT(2));
			/* 1000M & 100M EEE capability */
			rtl_mmd_write(tp, 0x7, 60, (BIT(2) | BIT(1)));
			/* 10M EEE */
			rtl_phy_write(tp, 0x0a43, 25,
				rtl_phy_read(tp, 0x0a43, 25) | BIT(4) | BIT(2));
			/* stop RXC in LPI mode */
			rtl_mmd_write(tp, 0x3, 0,
				rtl_mmd_read(tp, 0x3, 0) | BIT(10));
		}
		/* EEE Tw_sys_tx timing adjustment,
		* make sure MAC would send data after FEPHY wake up
		*/
		/* default 0x001F, 100M EEE */
		r8168_mac_ocp_write(tp, 0xe04a, 0x002f);
		/* default 0x0011, 1000M EEE */
		r8168_mac_ocp_write(tp, 0xe04c, 0x001a);
		/* EEEP Tw_sys_tx timing adjustment,
		* make sure MAC would send data after FEPHY wake up
		*/
		/* default 0x3f, 10M EEEP */
		if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
			/* FEPHY needs 160us */
			r8168_mac_ocp_write(tp, 0xe08a, 0x00a8);
		} else {	/* RGMII, GPHY needs 25us */
			r8168_mac_ocp_write(tp, 0xe08a, 0x0020);
		}
		/* default 0x0005, 100M/1000M EEEP */
		r8168_mac_ocp_write(tp, 0xe08c, 0x0008);
		/* EEE MAC mode */
		r8168_mac_ocp_write(tp, 0xe040,
			r8168_mac_ocp_read(tp, 0xe040) | BIT(1) | BIT(0));
		/* EEE+ MAC mode */
		r8168_mac_ocp_write(tp, 0xe080, r8168_mac_ocp_read(tp, 0xe080) | BIT(1));
	} else {
		if (tp->output_mode == OUTPUT_EMBEDDED_PHY) {
			/* disable eee auto fallback function */
			rtl_phy_write(tp, 0x0a4b, 17,
				rtl_phy_read(tp, 0x0a4b, 17) & ~BIT(2));
			/* 100M & 1000M EEE capability */
			rtl_mmd_write(tp, 0x7, 60, 0);
			/* 10M EEE */
			rtl_phy_write(tp, 0x0a43, 25,
				rtl_phy_read(tp, 0x0a43, 25) & ~(BIT(4) | BIT(2)));
			/* keep RXC in LPI mode */
			rtl_mmd_write(tp, 0x3, 0,
				rtl_mmd_read(tp, 0x3, 0) & ~BIT(10));
		} else { /* RGMII */
			/* disable eee auto fallback function */
			rtl_phy_write(tp, 0x0a4b, 17,
				rtl_phy_read(tp, 0x0a4b, 17) & ~BIT(2));
			/* 100M & 1000M EEE capability */
			rtl_mmd_write(tp, 0x7, 60, 0);
			/* 10M EEE */
			rtl_phy_write(tp, 0x0a43, 25,
				rtl_phy_read(tp, 0x0a43, 25) & ~(BIT(4) | BIT(2)));
			/* keep RXC in LPI mode */
			rtl_mmd_write(tp, 0x3, 0,
				rtl_mmd_read(tp, 0x3, 0) & ~BIT(10));
		}
		/* reset to default value */
		r8168_mac_ocp_write(tp, 0xe040, r8168_mac_ocp_read(tp, 0xe040) | BIT(13));
		/* EEE MAC mode */
		r8168_mac_ocp_write(tp, 0xe040,
			r8168_mac_ocp_read(tp, 0xe040) & ~(BIT(1) | BIT(0)));
		/* EEE+ MAC mode */
		r8168_mac_ocp_write(tp, 0xe080, r8168_mac_ocp_read(tp, 0xe080) & ~BIT(1));
		/* EEE Tw_sys_tx timing restore */
		/* default 0x001F, 100M EEE */
		r8168_mac_ocp_write(tp, 0xe04a, 0x001f);
		/* default 0x0011, 1000M EEE */
		r8168_mac_ocp_write(tp, 0xe04c, 0x0011);
		/* EEEP Tw_sys_tx timing restore */
		/* default 0x3f, 10M EEEP */
		r8168_mac_ocp_write(tp, 0xe08a, 0x003f);
		/* default 0x0005, 100M/1000M EEEP */
		r8168_mac_ocp_write(tp, 0xe08c, 0x0005);
	}
#endif
}
#endif

static int rtl_init_one(struct platform_device *pdev)
{
	struct rtl8168_private *tp;
	struct mii_if_info *mii;
	char *env_base;
	unsigned features = RTL_TP_FEATURE;
	u32 ioaddr;
	/*int i; */
	int rc = 0;
	int irq = 0;
	const char dev_name[] = "eth0";
	u32 voltage = 1;
	u32 tx_delay = 0;
	u32 rx_delay = 0;
	
	u32 output_mode = 0;
	u32 ext_phy_id = 1;
#ifdef CONFIG_RTK_KDRV_R8168_MODULE
	parse_rtl8168_enable();
	parse_rtl8168_set_tapbin();
	parse_rtl8168_snr_debug();
#endif
	if(eth_init == 0) {
		eth_info("SCPU does not enable rtl8168 driver\n");
		return 1;
	}

	if (netif_msg_drv(&debug)) {
		eth_info("%s Gigabit Ethernet driver %s loaded\n",
				MODULENAME, RTL8168_VERSION);
	}

	if (of_property_read_u32(pdev->dev.of_node, "output-mode", &output_mode))
		eth_debug("%s can't get output mode", __func__);
	if (of_property_read_u32(pdev->dev.of_node, "ext-phy-id", &ext_phy_id))
		eth_debug("%s can't get RGMII external PHY ID", __func__);
	if (of_property_read_u32(pdev->dev.of_node, "voltage", &voltage))
		eth_debug("%s can't get voltage", __func__);
	if (of_property_read_u32(pdev->dev.of_node, "tx-delay", &tx_delay))
		eth_debug("%s can't get TX delay", __func__);
	if (of_property_read_u32(pdev->dev.of_node, "rx-delay", &rx_delay))
		eth_debug("%s can't get RX delay", __func__);
	eth_info("output mode = %u\n", output_mode);
	
	ndev = alloc_etherdev(sizeof (*tp));
	if (!ndev) {
		rc = -ENOMEM;
		goto out;
	}

	irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
	if(!irq) {
		eth_err("No eth irq found!\n");
		rc = -ENODEV;
		goto err_irq;
	}

	ndev->netdev_ops = &rtl_netdev_ops;
	ndev->irq = irq;
	memset(ndev->name, 0, sizeof(ndev->name));
	strncpy(ndev->name, dev_name, sizeof(ndev->name)-1);
	tp = netdev_priv(ndev);
	tp->dev = ndev;
	tp->msg_enable = netif_msg_init(debug.msg_enable, R8168_MSG_DEFAULT);
	tp->link_recovery_enable = RTL8168_LINK_RECOVERY_DISABLE;	/* CN don't recommend to clear lock_main_uc */
	tp->link_recovery_count = 0;
	tp->features =	tp->features & (~RTL_FEATURE_WOL);
	tp->features = tp->features | RTL_FEATURE_WOL_PATTERN;
	tp->output_mode = output_mode;
	tp->ext_phy_id = ext_phy_id;
	tp->ext_phy = false;
	tp->voltage = voltage;
	tp->tx_delay = tx_delay;
	tp->rx_delay = rx_delay;

	mii = &tp->mii;
	mii->dev = ndev;
	mii->mdio_read = rtl_mdio_read;
	mii->mdio_write = rtl_mdio_write;
	mii->phy_id_mask = 0x1f;
	mii->reg_num_mask = 0x1f;
	mii->supports_gmii = !!(features & RTL_FEATURE_GMII);

	tp->cp_cmd = RxChkSum;
	ioaddr = SW_DEF_BASE_ADDR;
	tp->base_addr = ioaddr;	

	rtl_init_mdio_ops(tp);
	rtl_init_mmd_ops(tp);
	rtl_init_pll_power_ops(tp);
	rtl_init_jumbo_ops(tp);
	/* after r8168soc_mdio_init(),
	* RGMII : tp->ext_phy == true	==> external MDIO,
	* RMII	: tp->ext_phy == false ==> no MDIO,
	* FE PHY: tp->ext_phy == false ==> internal MDIO
	*/

	ndev->features |= NETIF_F_HIGHDMA;



	/* Check chip attached to board */
	if (!(rtl8168_check_mac_version(tp, ndev))) {
		eth_err("mac version is incorrect\n");
		goto err_out_msi;
	}

	ndev->dev.coherent_dma_mask = DMA_BIT_MASK(32);
	ndev->dev.dma_mask = &ndev->dev.coherent_dma_mask;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)) && defined(CONFIG_ARM64)
	arch_setup_dma_ops(&ndev->dev, 0, 0, NULL, false);
#endif
	if(dma_set_mask(&ndev->dev, DMA_BIT_MASK(32))) {
		eth_err("DMA not supported\n");
	}

	/* Initialize RX config */
	rtl_init_rxcfg(tp);

	/* Disable interrupt */
	rtl_irq_disable(tp);

	/* Check TX/RX FIFO and Disable TX/RX */
	rtl_hw_initialize(tp);

	/* Software reset */
	rtl_hw_reset(tp);

	/* Clear interrupt status */
	rtl_ack_events(tp, 0xffff);

	/*rtl_init_csi_ops(tp); */

#if 0
	/* For WOL ?? */
	RTL_W8(Cfg9346, Cfg9346_Unlock);
	RTL_W8(Config1, RTL_R8(Config1) | PMEnable);
	RTL_W8(Config5, RTL_R8(Config5) & PMEStatus);

	/*disable magic packet WOL */
	RTL_W8(Config3, RTL_R8(Config3) & ~MagicPacket);
	if ((RTL_R8(Config3) & (LinkUp | MagicPacket)) != 0)
		tp->features |= RTL_FEATURE_WOL;
	if ((RTL_R8(Config5) & (UWF | BWF | MWF)) != 0)
		tp->features |= RTL_FEATURE_WOL;
	tp->features |= rtl_try_msi(tp, features);
	RTL_W8(Cfg9346, Cfg9346_Lock);
#endif

	tp->set_speed = rtl8168_set_speed_xmii;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0))
	tp->get_settings = rtl8168_gset_xmii;
#endif
	tp->phy_reset_enable = rtl8168_xmii_reset_enable;
	tp->phy_set_aldps = rtl8168_xmii_set_aldps;
	tp->phy_enhance_performance = rtl8168_xmii_enhance_performance;
	tp->phy_reset_pending = rtl8168_xmii_reset_pending;
	tp->link_ok = rtl8168_xmii_link_ok;
	tp->do_ioctl = rtl_xmii_ioctl;

	mutex_init(&tp->wk.mutex);

#if 1
	/* default MAC address */
	((u16 *) (ndev->dev_addr))[0] = 0x0000;
	((u16 *) (ndev->dev_addr))[1] = 0x0000;
	((u16 *) (ndev->dev_addr))[2] = 0x0000;
	((u16 *) (ndev->dev_addr))[3] = 0x0000;

	/* read MAC address from FLASH */
	env_base = platform_info.ethaddr;

	/*eth_info("MAC address = 0x%s\n",env_base); */
	ndev->dev_addr[0] = (u8) simple_strtoul(env_base, NULL, 16);
	ndev->dev_addr[1] = (u8) simple_strtoul(env_base + 3, NULL, 16);
	ndev->dev_addr[2] = (u8) simple_strtoul(env_base + 6, NULL, 16);
	ndev->dev_addr[3] = (u8) simple_strtoul(env_base + 9, NULL, 16);
	ndev->dev_addr[4] = (u8) simple_strtoul(env_base + 12, NULL, 16);
	ndev->dev_addr[5] = (u8) simple_strtoul(env_base + 15, NULL, 16);
	ndev->dev_addr[6] = (u8) (0x00);
	ndev->dev_addr[7] = (u8) (0x00);
#else
	/* default MAC address */
	RTL_W32(MAC0, 0x33221100);
	RTL_W32(MAC4, 0x00005544);
	{
		int i;
		for (i = 0; i < ETH_ALEN; i++)
			ndev->dev_addr[i] = RTL_R8(MAC0 + i);
	}

#endif
	eth_info("%s: RTL8168 at 0x%08x, "
			"%02x:%02x:%02x:%02x:%02x:%02x, "
			"IRQ %d\n",
			ndev->name,
			tp->base_addr,
			ndev->dev_addr[0], ndev->dev_addr[1],
			ndev->dev_addr[2], ndev->dev_addr[3],
			ndev->dev_addr[4], ndev->dev_addr[5], ndev->irq);

	netdev_set_default_ethtool_ops(ndev, &rtl8168_ethtool_ops);
	ndev->watchdog_timeo = RTL8168_TX_TIMEOUT;

	netif_napi_add(ndev, &tp->napi, rtl8168_poll, R8168_NAPI_WEIGHT);

	/* don't enable SG, IP_CSUM and TSO by default - it might not work
	* properly for all devices */
	ndev->features |= RTL_NETIF_FEATURES;

	ndev->hw_features = NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_TSO |
		NETIF_F_RXCSUM | NETIF_F_HW_VLAN_CTAG_TX |
		NETIF_F_HW_VLAN_CTAG_RX;
	ndev->vlan_features = NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_TSO |
		NETIF_F_HIGHDMA;

	ndev->hw_features |= NETIF_F_RXALL;
	ndev->hw_features |= NETIF_F_RXFCS;

	tp->hw_start = rtl_hw_start_8168;
	tp->event_slow = SYSErr | LinkChg | RxOverflow;

	tp->opts1_mask = ~(RxBOVF | RxFOVF);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	timer_setup(&tp->timer, rtl8168_phy_timer,0);
#else
	init_timer(&tp->timer);
	tp->timer.data = (unsigned long) ndev;
	tp->timer.function = rtl8168_phy_timer; /* call rtl_task	*/
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	timer_setup(&tp->link_check_timer, rtl8168_link_timer,0);
#else
	init_timer(&tp->link_check_timer);
	tp->link_check_timer.data = (unsigned long) ndev;
	tp->link_check_timer.function = rtl8168_link_timer;
#endif

	SET_NETDEV_DEV(ndev, &pdev->dev);

	rc = register_netdev(ndev);
	if (rc < 0)
		goto err_out_msi;

#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
	timer_setup(&tp->rx_timer, rtl8168_rx_timer,0);
#else
	init_timer(&tp->rx_timer);
	tp->rx_timer.data = (unsigned long) ndev;
	tp->rx_timer.function = rtl8168_rx_timer;
#endif
#endif

	platform_set_drvdata(pdev, ndev);
	/*device_set_wakeup_enable(&pdev->dev, tp->features & RTL_FEATURE_WOL); */

	tp->link_status = RTL8168_LINK_DOWN;
#if NWAY_ENABLE
	tp->autoneg = AUTONEG_ENABLE;
#else
	tp->autoneg = AUTONEG_DISABLE;
#endif

	netif_carrier_off(ndev);

#ifdef CONFIG_PM
	network_devs = pdev;

#ifdef CONFIG_CUSTOMER_TV006
	device_enable_async_suspend(&pdev->dev);
#endif
#endif
	eth_kobj = kobject_create_and_add("etn", NULL);
	if (!eth_kobj) {
		rc = -ENOMEM;
		goto err_kobject_create;
	}

	rc = sysfs_create_group(eth_kobj, &eth_attr_group);
	if (rc != 0) {
		goto err_sysfs_create_group;
	}

#if IS_ENABLED (CONFIG_RTK_KDRV_SB2)
	SB2_DEBUG_INFO_REGISTER(eth, rtl8168_rbus_timeout_debug);
#endif
	rtl8168_init_mdns_packet_upload_work(ndev);

out:
	return rc;

err_sysfs_create_group:
	kobject_put(eth_kobj);
err_kobject_create:
err_out_msi:
	netif_napi_del(&tp->napi);
err_irq:
	free_netdev(ndev);
	/*rtl_disable_msi(pdev, tp); */
	goto out;
}

static int rtl_remove_one(struct platform_device *pdev)
{
	struct rtl8168_private *tp = netdev_priv(ndev);

	cancel_work_sync(&tp->wk.work);
	rtl8168_cancel_mdns_packet_upload_work(ndev);

	netif_napi_del(&tp->napi);

	unregister_netdev(ndev);

	/* restore original MAC address */
	rtl_rar_set(tp, ndev->perm_addr);

	free_netdev(ndev);

	platform_set_drvdata(pdev, NULL);

#if IS_ENABLED (CONFIG_RTK_KDRV_SB2)
	SB2_DEBUG_INFO_UNREGISTER(eth, rtl8168_rbus_timeout_debug);
#endif
	
	return 0;
}

static const struct of_device_id rtl8168_dt_ids[] = {
	{ .compatible = "realtek,rtk_8168", },
	{},
};

MODULE_DEVICE_TABLE(of, rtl8168_dt_ids);

static struct platform_driver rtl8168_driver = {
	.probe		= rtl_init_one,
	.remove		= rtl_remove_one,
	.shutdown	= rtl_shutdown,
#ifdef CONFIG_PM
	.suspend	= rtl8168_suspend,
	.resume		= rtl8168_resume,
#endif
	.driver = {
		.name		= MODULENAME,
		.owner		= THIS_MODULE,
		.of_match_table = of_match_ptr(rtl8168_dt_ids),
	},
};

module_platform_driver(rtl8168_driver);
