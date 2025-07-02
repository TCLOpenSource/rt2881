#ifndef __RTK_8168_INC_H__
#define __RTK_8168_INC_H__
#include <linux/mii.h>


#define CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
#define ENABLE_EEEP		1
#define ENABLE_ALDPS		1
#define ENABLE_WOL		1
//#define ENABLE_WOW		1

#define NWAY_ENABLE 1

#define CURRENT_MDIO_PAGE 0xFFFFFFFF
#define eth_debug(fmt, args...)\
	rtd_pr_eth_debug(fmt , ## args)
#define eth_info(fmt, args...)\
	rtd_pr_eth_info(fmt , ## args)
#define eth_notice(fmt, args...)\
	rtd_pr_eth_notice(fmt , ## args)
#define eth_warning(fmt, args...)\
	rtd_pr_eth_warn(fmt , ## args)
#define eth_err(fmt, args...)\
	rtd_pr_eth_err(fmt , ## args)

#define R8168_MSG_DEFAULT \
	(NETIF_MSG_DRV | NETIF_MSG_PROBE | NETIF_MSG_IFUP | NETIF_MSG_IFDOWN)

#define TX_SLOTS_AVAIL(tp) \
	((tp)->dirty_tx + NUM_TX_DESC - (tp)->cur_tx - 3072)

/* A skbuff with nr_frags needs nr_frags+1 entries in the tx queue */
#define TX_FRAGS_READY_FOR(tp, nr_frags) \
	(TX_SLOTS_AVAIL(tp) >= ((nr_frags) + 1))

#define MAX_READ_REQUEST_SHIFT	12
#define TX_DMA_BURST	4	/* Maximum PCI burst, '7' is unlimited */
#define InterFrameGap	0x03	/* 3 means InterFrameGap = the shortest one */

#define R8168_REGS_SIZE		256
#define R8168_NAPI_WEIGHT	64
#define NUM_TX_DESC	4096	/* Number of Tx descriptor registers */
#define NUM_RX_DESC	256U	/* Number of Rx descriptor registers */
#define R8168_TX_RING_BYTES	(NUM_TX_DESC * sizeof(struct TxDesc))
#define R8168_RX_RING_BYTES	(NUM_RX_DESC * sizeof(struct RxDesc))

#define RTL8168_TX_TIMEOUT	(6*HZ)
#if NWAY_ENABLE
#define RTL8168_PHY_TIMEOUT	(1*HZ)
#else
#define RTL8168_PHY_TIMEOUT	(3*HZ)
#endif
#define RTL8168_LINK_TIMEOUT	(8*HZ)
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY
#define RTL8168_RX_TIMEOUT	msecs_to_jiffies(1)
#endif
#define ERIAR_MASK_0001			(0x1 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0010			(0x2 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0011			(0x3 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0100			(0x4 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0101			(0x5 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_0111			(0x7 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_1000			(0x8 << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_1100			(0xc << ERIAR_MASK_SHIFT)
#define ERIAR_MASK_1111			(0xf << ERIAR_MASK_SHIFT)

struct TxDesc {
	__le32 opts1;
	__le32 opts2;
	__le64 addr;
};

struct RxDesc {
	__le32 opts1;
	__le32 opts2;
	__le64 addr;
};

struct ring_info {
	struct sk_buff	*skb;
	u32		len;
	u8		__pad[sizeof(void *) - sizeof(u32)];
};

enum features {
	RTL_FEATURE_WOL		= BIT(0),
	RTL_FEATURE_MSI		= BIT(1),
	RTL_FEATURE_GMII	= BIT(2),
	RTL_FEATURE_WOL_PATTERN = BIT(3),
};

struct rtl8168_counters {
	__le64	tx_packets;
	__le64	rx_packets;
	__le64	tx_errors;
	__le32	rx_errors;
	__le16	rx_missed;
	__le16	align_errors;
	__le32	tx_one_collision;
	__le32	tx_multi_collision;
	__le64	rx_unicast;
	__le64	rx_broadcast;
	__le32	rx_multicast;
	__le16	tx_aborted;
	__le16	tx_underun;
};

enum rtl_flag {
	RTL_FLAG_TASK_ENABLED,
	RTL_FLAG_TASK_SLOW_PENDING,
	RTL_FLAG_TASK_RESET_PENDING,
	RTL_FLAG_TASK_PHY_PENDING,
	RTL_FLAG_MAX
};

struct rtl8168_stats {
	u64			packets;
	u64			bytes;
	struct u64_stats_sync	syncp;
};

enum rtl_output_mode {
	OUTPUT_EMBEDDED_PHY,
	OUTPUT_RGMII_TO_MAC,
	OUTPUT_RGMII_TO_PHY,
	OUTPUT_RMII,
	OUTPUT_MAX
};

enum rtl8168_link_status {
	RTL8168_LINK_UP = 0,
	RTL8168_LINK_DOWN,
	RTL8168_LINK_UNKNOWN,
	RTL8168_LINK_MAX
};

enum rtl8168_link_recovery_enable {
	RTL8168_LINK_RECOVERY_DISABLE = 0,
	RTL8168_LINK_RECOVERY_ENABLE
};

#define POR_XV_MASK	0x00000111
#define POR_NUM		3
enum phy_addr_e {
	INT_PHY_ADDR = 1,	/* embedded PHY PHY ID */
	SERDES_DPHY_0 = 0,	/* embedded SerDes DPHY PHY ID 0 */
	SERDES_DPHY_1 = 1,	/* embedded SerDes DPHY PHY ID 1 */
	EXT_PHY_ADDR = 3,	/* external RTL8211FS SGMII PHY ID */
};

enum rtl8168_wol_enable {
	RTL8168_WOL_DISABLE = 0,
	RTL8168_WOL_ENABLE
};

enum rtl8168_snr_enable {
	RTL8168_SNR_DISABLE = 0,
	RTL8168_SNR_ENABLE
};

typedef struct {
	u32 avg_linkup_enable;
	u32 avg_linkup_time;
	u32 retry_count;
} rtl_avg_link;

typedef struct {
	u32 phy_set;		/*reg0*/
	u32 link_status;	/*reg1*/
	u32 adv_ability;	/*reg4*/
	u32 giga_adv_ability;	/* page 0xa41, reg 9 */
	u32 phy_status;		/* page 0xa42, reg 16 */
	u32 link_speed;		/* page 0xa43, reg 26 */
	u32 lock_main;		/* page 0xa46, reg 21 */
	u32 pcs_state;		/* page 0xa60, reg 16 */
} rtl_phy_status;

struct rtl8168_private {
	unsigned int base_addr;
	struct net_device *dev;
	struct napi_struct napi;
	u32 msg_enable;
	u32 cur_rx; /* Index into the Rx descriptor buffer of next Rx pkt. */
	u32 cur_tx; /* Index into the Tx descriptor buffer of next Rx pkt. */
	u32 dirty_tx;
	u32 link_recovery_enable;
	u32 link_recovery_count;
	u32 link_start_time;
	u32 link_up_time;
	struct rtl8168_stats rx_stats;
	struct rtl8168_stats tx_stats;
	struct TxDesc *TxDescArray;	/* 256-aligned Tx descriptor ring */
	struct RxDesc *RxDescArray;	/* 256-aligned Rx descriptor ring */
	dma_addr_t TxPhyAddr;
	dma_addr_t RxPhyAddr;
	void *Rx_databuff[NUM_RX_DESC];	/* Rx data buffers */
	struct ring_info tx_skb[NUM_TX_DESC];	/* Tx data buffers */
	struct timer_list timer;
	struct timer_list link_check_timer;
#ifdef CONFIG_SUPPORT_RX_TIMER_FOR_DELAY	
	struct timer_list rx_timer;	
#endif
	u16 cp_cmd;

	u16 event_slow;

	struct mdio_ops {
		void (*write)(struct rtl8168_private *tp, int page, int reg, int value);
		int (*read)(struct rtl8168_private *tp, int page, int reg);
	} mdio_ops;

	struct mmd_ops {
		void (*write)(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr, u32 value);
		u32 (*read)(struct rtl8168_private *tp, u32 dev_addr, u32 reg_addr);
	} mmd_ops;

	struct pll_power_ops {
		void (*down)(struct rtl8168_private *tp);
		void (*up)(struct rtl8168_private *tp);
	} pll_power_ops;

	struct jumbo_ops {
		void (*enable)(struct rtl8168_private *tp);
		void (*disable)(struct rtl8168_private *tp);
	} jumbo_ops;

	struct csi_ops {
		void (*write)(struct rtl8168_private *tp, int addr, int value);
		u32 (*read)(struct rtl8168_private *tp, int addr);
	} csi_ops;

	int (*set_speed)(struct net_device *dev, u8 aneg, u16 sp, u8 dpx, u32 adv);
	int (*get_settings)(struct net_device *dev, struct ethtool_cmd *cmd);
	void (*phy_reset_enable)(struct rtl8168_private *tp);
	void (*phy_set_aldps)(struct rtl8168_private *tp, u32 enable);
	void (*phy_enhance_performance)(struct rtl8168_private *tp, u32 enable);
	void (*hw_start)(struct net_device *dev);
	unsigned int (*phy_reset_pending)(struct rtl8168_private *tp);
	unsigned int (*link_ok)(u32);
	int (*do_ioctl)(struct rtl8168_private *tp, struct mii_ioctl_data *data, int cmd);

	struct {
		DECLARE_BITMAP(flags, RTL_FLAG_MAX);
		struct mutex mutex; /* mutex for work */
		struct work_struct work;
	} wk;

	unsigned int features;

	struct mii_if_info mii;
	struct rtl8168_counters counters;
	u32 saved_wolopts;
	u32 opts1_mask;

	u32 link_status;
	u32 autoneg;
	rtl_phy_status phy_status;
	/* 0:embedded PHY, 1:RGMII to MAC, 2:RGMII to PHY, 3:RMII */
	u8 output_mode;
	u8 voltage; /* 1:1.8V, 2: 2.5V, 3:3.3V */
	u8 tx_delay; /* 0: 0ns, 1: 2ns */
	u8 rx_delay; /* 0 ~ 7 x 0.5ns */	
	u8 ext_phy_id; /* 0 ~ 31 */
	bool ext_phy;

};

void r8168_mac_ocp_write(struct rtl8168_private *tp, u32 reg, u32 data);
u16 r8168_mac_ocp_read(struct rtl8168_private *tp, u32 reg);
void rtl_eri_write(struct rtl8168_private *tp, int addr, u32 mask, u32 val, int type);
u32 rtl_eri_read(struct rtl8168_private *tp, int addr, int type);
#define ETH_WOL_DUMMY_WAKE_REASON_MASK	0x3F
#define ETH_WOL_DUMMY_WAKE_REASON_EFFECTED		0x40
#define ETH_WOL_DUMMY_WPD_DETECT_ENABLE			0x80
#define ETH_WOL_DUMMY_VCPU_MDNS_OFFLOADING_ENABLE	(0x8000)
#define ETH_WOL_DUMMY_HW_MDNS_OFFLOADING_ENABLE	(0x10000000)
#endif
