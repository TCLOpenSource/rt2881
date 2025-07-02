/**
 * core.h - DesignWare USB3 DRD Core Header
 */

#ifndef __USB_REG_H
#define __USB_REG_H

#include "usb_cdc.h"

#if 0
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define cpu_to_le16(x)  (x)
#define cpu_to_le32(x)   (x)
#define le16_to_cpu(x)   (x)
#define le32_to_cpu(x)   (x)
#endif

#define bool UINT8

#define true 1
#define false 0

#if 0
#define EINVAL 1
#define ETIMEDOUT 1
#define EAGAIN 1
#define EOPNOTSUPP 1
#define ECONNRESET 1
#endif

#ifdef CONFIG_ARCH_5281
//#define dvr_to_phys(x)  (((UINT32)x >= 0xA0000000) ? ((UINT32)x - 0xA0000000) : ((UINT32)x - 0x80000000))
#else
//#define dvr_to_phys(x)  ((UINT32)x) //(((UINT32)x >= 0xA0000000) ? ((UINT32)x - 0xA0000000) : ((UINT32)x - 0x80000000))
#endif
#define DWC3_MSG_MAX	500

#define USB_EP_COUNT 14

/* Global constants */
#define DWC3_EP0_BOUNCE_SIZE	512
#define DWC3_ENDPOINTS_NUM	32
#define DWC3_XHCI_RESOURCES_NUM	2

#define DWC3_SCRATCHBUF_SIZE	4096	/* each buffer is assumed to be 4KiB */
#define DWC3_EVENT_SIZE		4	/* bytes */
#define DWC3_EVENT_MAX_NUM	64	/* 2 events/endpoint */
#define DWC3_EVENT_BUFFERS_SIZE	(DWC3_EVENT_SIZE * DWC3_EVENT_MAX_NUM)
#define DWC3_EVENT_TYPE_MASK	0xfe
#define EVENT_MAGIC  0x2379beef

#define DWC3_EVENT_TYPE_DEV	0
#define DWC3_EVENT_TYPE_CARKIT	3
#define DWC3_EVENT_TYPE_I2C	4

#define DWC3_DEVICE_EVENT_DISCONNECT		0
#define DWC3_DEVICE_EVENT_RESET			1
#define DWC3_DEVICE_EVENT_CONNECT_DONE		2
#define DWC3_DEVICE_EVENT_LINK_STATUS_CHANGE	3
#define DWC3_DEVICE_EVENT_WAKEUP		4
#define DWC3_DEVICE_EVENT_HIBER_REQ		5
#define DWC3_DEVICE_EVENT_EOPF			6
#define DWC3_DEVICE_EVENT_SOF			7
#define DWC3_DEVICE_EVENT_ERRATIC_ERROR		9
#define DWC3_DEVICE_EVENT_CMD_CMPL		10
#define DWC3_DEVICE_EVENT_OVERFLOW		11

#define DWC3_GEVNTCOUNT_MASK	0xfffc
#define DWC3_GEVNTCOUNT_BUSY_MASK	0x80000000
#define DWC3_GSNPSID_MASK	0xffff0000
#define DWC3_GSNPSREV_MASK	0xffff

/* DWC3 registers memory space boundries */
#define DWC3_XHCI_REGS_START		0x0
#define DWC3_XHCI_REGS_END		0x7fff
#define DWC3_GLOBALS_REGS_START		0xc100
#define DWC3_GLOBALS_REGS_END		0xc6ff
#define DWC3_DEVICE_REGS_START		0xc700
#define DWC3_DEVICE_REGS_END		0xcbff
#define DWC3_OTG_REGS_START		0xcc00
#define DWC3_OTG_REGS_END		0xccff

/* Global Registers */
#define DWC3_GSBUSCFG0		0x0000+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GSBUSCFG1		0x0004+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GTXTHRCFG		0x0008+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GRXTHRCFG		0x000c+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GCTL		0x0010+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GEVTEN		0x0014+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GSTS		0x0018+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GSNPSID		0x0020+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GGPIO		0x0024+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GUID		0x0028+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GUCTL		0x002c+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GBUSERRADDR0	0x0030+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GBUSERRADDR1	0x0034+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GPRTBIMAP0		0x0038+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GPRTBIMAP1		0x003c+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS0		0x0040+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS1		0x0044+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS2		0x0048+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS3		0x004c+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS4		0x0050+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS5		0x0054+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS6		0x0058+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GHWPARAMS7		0x005c+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GDBGFIFOSPACE	0x0060+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GDBGLTSSM		0x0064+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GPRTBIMAP_HS0	0x0080+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GPRTBIMAP_HS1	0x0084+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GPRTBIMAP_FS0	0x0088+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GPRTBIMAP_FS1	0x008c+USB_DWC3_DEV_GLOBAL_REG_START

#define DWC3_VER_NUMBER		0x00a0+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_VER_TYPE		0x00a4+USB_DWC3_DEV_GLOBAL_REG_START

#define DWC3_GUSB2PHYCFG(n)	(0x0100 + ((n) * 0x04))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GUSB2I2CCTL(n)	(0x0140 + ((n) * 0x04))+USB_DWC3_DEV_GLOBAL_REG_START

#define DWC3_GUSB2PHYACC(n)	(0x0180 + ((n) * 0x04))+USB_DWC3_DEV_GLOBAL_REG_START

#define DWC3_GUSB3PIPECTL(n)	(0x01c0 + ((n) * 0x04))+USB_DWC3_DEV_GLOBAL_REG_START

#define DWC3_GTXFIFOSIZ(n)	(0x0200 + ((n) * 0x04))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GRXFIFOSIZ(n)	(0x0280 + ((n) * 0x04))+USB_DWC3_DEV_GLOBAL_REG_START

#define DWC3_GEVNTADRLO(n)	(0x0300 + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GEVNTADRHI(n)	(0x0304 + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GEVNTSIZ(n)	(0x0308 + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GEVNTCOUNT(n)	(0x030c + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START

#define DWC3_GHWPARAMS8		0x0500+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_GFLADJ		0x0530+USB_DWC3_DEV_GLOBAL_REG_START

/* Device Registers */
#define DWC3_DCFG		0x0600+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DCTL		0x0604+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DEVTEN		0x0608+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DSTS		0x060c+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DGCMDPAR		0x0610+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DGCMD		0x0614+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DALEPENA		0x0620+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DEPCMDPAR2(n)	(0x0700 + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DEPCMDPAR1(n)	(0x0704 + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DEPCMDPAR0(n)	(0x0708 + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DEPCMD(n)		(0x070c + ((n) * 0x10))+USB_DWC3_DEV_GLOBAL_REG_START
#define DWC3_DEV_IMOD(n)	(0x0900 + ((n) * 0x4))+USB_DWC3_DEV_GLOBAL_REG_START


/* Bit fields */

/* Global Configuration Register */
#define DWC3_GCTL_PWRDNSCALE(n)	((UINT32)(n) << 19)
#define DWC3_GCTL_U2RSTECN	(0x10000)
#define DWC3_GCTL_RAMCLKSEL(x)	((UINT32)((x) & DWC3_GCTL_CLK_MASK) << 6)
#define DWC3_GCTL_CLK_BUS	(0)
#define DWC3_GCTL_CLK_PIPE	(1)
#define DWC3_GCTL_CLK_PIPEHALF	(2)
#define DWC3_GCTL_CLK_MASK	(3)

#define DWC3_GCTL_PRTCAP(n)	(UINT32)(((n) & ((UINT32)3 << 12)) >> 12)
#define DWC3_GCTL_PRTCAPDIR(n)	((UINT32)(n) << 12)
#define DWC3_GCTL_PRTCAP_HOST	1
#define DWC3_GCTL_PRTCAP_DEVICE	2
#define DWC3_GCTL_PRTCAP_OTG	3

#define DWC3_GCTL_CORESOFTRESET		(0x800)
#define DWC3_GCTL_SOFITPSYNC		(0x400)
#define DWC3_GCTL_SCALEDOWN(n)		((UINT32)(n) << 4)
#define DWC3_GCTL_SCALEDOWN_MASK	DWC3_GCTL_SCALEDOWN(3)
#define DWC3_GCTL_DISSCRAMBLE		(0x8)
#define DWC3_GCTL_U2EXIT_LFPS		(0x4)
#define DWC3_GCTL_GBLHIBERNATIONEN	(0x2)
#define DWC3_GCTL_DSBLCLKGTNG		(0x1)

/* Global USB2 PHY Configuration Register */
#define DWC3_GUSB2PHYCFG_PHYSOFTRST	(0x80000000)
#define DWC3_GUSB2PHYCFG_SUSPHY		(0x40)
#define DWC3_GUSB2PHYCFG_ULPI_UTMI	(0x10)
#define DWC3_GUSB2PHYCFG_ENBLSLPM	(0x100)

/* Global USB2 PHY Vendor Control Register */
#define DWC3_GUSB2PHYACC_NEWREGREQ	(0x2000000)
#define DWC3_GUSB2PHYACC_BUSY		(0x800000)
#define DWC3_GUSB2PHYACC_WRITE		(0x400000)
#define DWC3_GUSB2PHYACC_ADDR(n)	((UINT32)(n) << 16)
#define DWC3_GUSB2PHYACC_EXTEND_ADDR(n)	((UINT32)(n) << 8)
#define DWC3_GUSB2PHYACC_DATA(n)	((n) & 0xff)

/* Global USB3 PIPE Control Register */
#define DWC3_GUSB3PIPECTL_PHYSOFTRST	(0x80000000)
#define DWC3_GUSB3PIPECTL_U2SSINP3OK	(0x20000000)
#define DWC3_GUSB3PIPECTL_REQP1P2P3	(0x1000000)
#define DWC3_GUSB3PIPECTL_DEP1P2P3(n)	((UINT32)(n) << 19)
#define DWC3_GUSB3PIPECTL_DEP1P2P3_MASK	DWC3_GUSB3PIPECTL_DEP1P2P3(7)
#define DWC3_GUSB3PIPECTL_DEP1P2P3_EN	DWC3_GUSB3PIPECTL_DEP1P2P3(1)
#define DWC3_GUSB3PIPECTL_DEPOCHANGE	(0x40000)
#define DWC3_GUSB3PIPECTL_SUSPHY	(0x20000)
#define DWC3_GUSB3PIPECTL_LFPSFILT	(0x200)
#define DWC3_GUSB3PIPECTL_RX_DETOPOLL	(0x100)
#define DWC3_GUSB3PIPECTL_TX_DEEPH_MASK	DWC3_GUSB3PIPECTL_TX_DEEPH(3)
#define DWC3_GUSB3PIPECTL_TX_DEEPH(n)	((UINT32)(n) << 1)

/* Global TX Fifo Size Register */
#define DWC3_GTXFIFOSIZ_TXFDEF(n)	((n) & 0xffff)
#define DWC3_GTXFIFOSIZ_TXFSTADDR(n)	((n) & 0xffff0000)

/* Global Event Size Registers */
#define DWC3_GEVNTSIZ_INTMASK		(0x80000000)
#define DWC3_GEVNTSIZ_SIZE(n)		((n) & 0xffff)

/* Global HWPARAMS1 Register */
#define DWC3_GHWPARAMS1_EN_PWROPT(n)	((UINT32)((n) & ((UINT32)3 << 24)) >> 24)
#define DWC3_GHWPARAMS1_EN_PWROPT_NO	0
#define DWC3_GHWPARAMS1_EN_PWROPT_CLK	1
#define DWC3_GHWPARAMS1_EN_PWROPT_HIB	2
#define DWC3_GHWPARAMS1_PWROPT(n)	((UINT32)(n) << 24)
#define DWC3_GHWPARAMS1_PWROPT_MASK	DWC3_GHWPARAMS1_PWROPT(3)

/* Global HWPARAMS3 Register */
#define DWC3_GHWPARAMS3_SSPHY_IFC(n)		((n) & 3)
#define DWC3_GHWPARAMS3_SSPHY_IFC_DIS		0
#define DWC3_GHWPARAMS3_SSPHY_IFC_ENA		1
#define DWC3_GHWPARAMS3_HSPHY_IFC(n)		((UINT32)((n) & ((UINT32)3 << 2)) >> 2)
#define DWC3_GHWPARAMS3_HSPHY_IFC_DIS		0
#define DWC3_GHWPARAMS3_HSPHY_IFC_UTMI		1
#define DWC3_GHWPARAMS3_HSPHY_IFC_ULPI		2
#define DWC3_GHWPARAMS3_HSPHY_IFC_UTMI_ULPI	3
#define DWC3_GHWPARAMS3_FSPHY_IFC(n)		((UINT32)((n) & ((UINT32)3 << 4)) >> 4)
#define DWC3_GHWPARAMS3_FSPHY_IFC_DIS		0
#define DWC3_GHWPARAMS3_FSPHY_IFC_ENA		1

/* Global HWPARAMS4 Register */
#define DWC3_GHWPARAMS4_HIBER_SCRATCHBUFS(n)	(((UINT32)(n) & ((UINT32)0x0f << 13)) >> 13)
#define DWC3_MAX_HIBER_SCRATCHBUFS		15

/* Global HWPARAMS6 Register */
#define DWC3_GHWPARAMS6_EN_FPGA			(0x80)

/* Global Frame Length Adjustment Register */
#define DWC3_GFLADJ_30MHZ_SDBND_SEL		(0x80)
#define DWC3_GFLADJ_30MHZ_MASK			0x3f

/* Device Configuration Register */
#define DWC3_DCFG_DEVADDR(addr)	((UINT32)(addr) << 3)
#define DWC3_DCFG_DEVADDR_MASK	DWC3_DCFG_DEVADDR(0x7f)

#define DWC3_DCFG_SPEED_MASK	(7)
#define DWC3_DSTS_SUPERSPEED_PLUS	(5)
#define DWC3_DCFG_SUPERSPEED	(4)
#define DWC3_DCFG_HIGHSPEED	(0)
#define DWC3_DCFG_FULLSPEED2	(1)
#define DWC3_DCFG_LOWSPEED	(2)
#define DWC3_DCFG_FULLSPEED1	(3)

#define DWC3_DCFG_LPM_CAP	(0x400000)

/* Device Control Register */
#define DWC3_DCTL_RUN_STOP	(0x80000000)
#define DWC3_DCTL_CSFTRST	(0x40000000)
#define DWC3_DCTL_LSFTRST	(0x20000000)

#define DWC3_DCTL_HIRD_THRES_MASK	((UINT32)0x1f << 24)
#define DWC3_DCTL_HIRD_THRES(n)	((UINT32)(n) << 24)

#define DWC3_DCTL_APPL1RES	(0x800000)

/* These apply for core versions 1.87a and earlier */
#define DWC3_DCTL_TRGTULST_MASK		((UINT32)0x0f << 17)
#define DWC3_DCTL_TRGTULST(n)		((UINT32)(n) << 17)
#define DWC3_DCTL_TRGTULST_U2		(DWC3_DCTL_TRGTULST(2))
#define DWC3_DCTL_TRGTULST_U3		(DWC3_DCTL_TRGTULST(3))
#define DWC3_DCTL_TRGTULST_SS_DIS	(DWC3_DCTL_TRGTULST(4))
#define DWC3_DCTL_TRGTULST_RX_DET	(DWC3_DCTL_TRGTULST(5))
#define DWC3_DCTL_TRGTULST_SS_INACT	(DWC3_DCTL_TRGTULST(6))

/* These apply for core versions 1.94a and later */
#define DWC3_DCTL_LPM_ERRATA_MASK	DWC3_DCTL_LPM_ERRATA(0xf)
#define DWC3_DCTL_LPM_ERRATA(n)		((UINT32)(n) << 20)

#define DWC3_DCTL_KEEP_CONNECT		((UINT32)1 << 19)
#define DWC3_DCTL_L1_HIBER_EN		((UINT32)1 << 18)
#define DWC3_DCTL_CRS			((UINT32)1 << 17)
#define DWC3_DCTL_CSS			((UINT32)1 << 16)

#define DWC3_DCTL_INITU2ENA		((UINT32)1 << 12)
#define DWC3_DCTL_ACCEPTU2ENA		((UINT32)1 << 11)
#define DWC3_DCTL_INITU1ENA		((UINT32)1 << 10)
#define DWC3_DCTL_ACCEPTU1ENA		((UINT32)1 << 9)
#define DWC3_DCTL_TSTCTRL_MASK		((UINT32)0xf << 1)

#define DWC3_DCTL_ULSTCHNGREQ_MASK	((UINT32)0x0f << 5)
#define DWC3_DCTL_ULSTCHNGREQ(n) (((UINT32)(n) << 5) & DWC3_DCTL_ULSTCHNGREQ_MASK)

#define DWC3_DCTL_ULSTCHNG_NO_ACTION	(DWC3_DCTL_ULSTCHNGREQ(0))
#define DWC3_DCTL_ULSTCHNG_SS_DISABLED	(DWC3_DCTL_ULSTCHNGREQ(4))
#define DWC3_DCTL_ULSTCHNG_RX_DETECT	(DWC3_DCTL_ULSTCHNGREQ(5))
#define DWC3_DCTL_ULSTCHNG_SS_INACTIVE	(DWC3_DCTL_ULSTCHNGREQ(6))
#define DWC3_DCTL_ULSTCHNG_RECOVERY	(DWC3_DCTL_ULSTCHNGREQ(8))
#define DWC3_DCTL_ULSTCHNG_COMPLIANCE	(DWC3_DCTL_ULSTCHNGREQ(10))
#define DWC3_DCTL_ULSTCHNG_LOOPBACK	(DWC3_DCTL_ULSTCHNGREQ(11))

/* Device Event Enable Register */
#define DWC3_DEVTEN_VNDRDEVTSTRCVEDEN	(0x1000)
#define DWC3_DEVTEN_EVNTOVERFLOWEN	(0x200)
#define DWC3_DEVTEN_CMDCMPLTEN		(0x400)
#define DWC3_DEVTEN_ERRTICERREN		(0x200)
#define DWC3_DEVTEN_SOFEN		(0x80)
#define DWC3_DEVTEN_EOPFEN		(0x40)
#define DWC3_DEVTEN_HIBERNATIONREQEVTEN	(0x20)
#define DWC3_DEVTEN_WKUPEVTEN		(0x10)
#define DWC3_DEVTEN_ULSTCNGEN		(0x8)
#define DWC3_DEVTEN_CONNECTDONEEN	(0x4)
#define DWC3_DEVTEN_USBRSTEN		(0x2)
#define DWC3_DEVTEN_DISCONNEVTEN	(0x1)

/* Device Status Register */
#define DWC3_DSTS_DCNRD			((UINT32)1 << 29)

/* This applies for core versions 1.87a and earlier */
#define DWC3_DSTS_PWRUPREQ		((UINT32)1 << 24)

/* These apply for core versions 1.94a and later */
#define DWC3_DSTS_RSS			((UINT32)1 << 25)
#define DWC3_DSTS_SSS			((UINT32)1 << 24)

#define DWC3_DSTS_COREIDLE		((UINT32)1 << 23)
#define DWC3_DSTS_DEVCTRLHLT		((UINT32)1 << 22)

#define DWC3_DSTS_USBLNKST_MASK		((UINT32)0x0f << 18)
#define DWC3_DSTS_USBLNKST(n)		((UINT32)((n) & DWC3_DSTS_USBLNKST_MASK) >> 18)

#define DWC3_DSTS_RXFIFOEMPTY		((UINT32)1 << 17)

#define DWC3_DSTS_SOFFN_MASK		((UINT32)0x3fff << 3)
#define DWC3_DSTS_SOFFN(n)		(((UINT32)(n) & DWC3_DSTS_SOFFN_MASK) >> 3)

#define DWC3_DSTS_CONNECTSPD		(7)

#define DWC3_DSTS_SUPERSPEED_PLUS		(5)
#define DWC3_DSTS_SUPERSPEED		(4)
#define DWC3_DSTS_HIGHSPEED		(0)
#define DWC3_DSTS_FULLSPEED2		(1)
#define DWC3_DSTS_LOWSPEED		(2)
#define DWC3_DSTS_FULLSPEED1		(3)

/* Device Generic Command Register */
#define DWC3_DGCMD_SET_LMP		0x01
#define DWC3_DGCMD_SET_PERIODIC_PAR	0x02
#define DWC3_DGCMD_XMIT_FUNCTION	0x03

/* These apply for core versions 1.94a and later */
#define DWC3_DGCMD_SET_SCRATCHPAD_ADDR_LO	0x04
#define DWC3_DGCMD_SET_SCRATCHPAD_ADDR_HI	0x05

#define DWC3_DGCMD_SELECTED_FIFO_FLUSH	0x09
#define DWC3_DGCMD_ALL_FIFO_FLUSH	0x0a
#define DWC3_DGCMD_SET_ENDPOINT_NRDY	0x0c
#define DWC3_DGCMD_RUN_SOC_BUS_LOOPBACK	0x10

#define DWC3_DGCMD_STATUS(n)		(((UINT32)(n) >> 12) & 0x0F)
#define DWC3_DGCMD_CMDACT		(0x400)
#define DWC3_DGCMD_CMDIOC		(0x100)

/* Device Generic Command Parameter Register */
#define DWC3_DGCMDPAR_FORCE_LINKPM_ACCEPT	(1)
#define DWC3_DGCMDPAR_FIFO_NUM(n)		((n))
#define DWC3_DGCMDPAR_RX_FIFO			(0)
#define DWC3_DGCMDPAR_TX_FIFO			(0x20)
#define DWC3_DGCMDPAR_LOOPBACK_DIS		(0)
#define DWC3_DGCMDPAR_LOOPBACK_ENA		(1)

/* Device Endpoint Command Register */
#define DWC3_DEPCMD_PARAM_SHIFT		16
#define DWC3_DEPCMD_PARAM(x)		((UINT32)(x) << DWC3_DEPCMD_PARAM_SHIFT)
#define DWC3_DEPCMD_GET_RSC_IDX(x)	(((UINT32)(x) >> DWC3_DEPCMD_PARAM_SHIFT) & 0x7f)
#define DWC3_DEPCMD_STATUS(x)		(((UINT32)(x) >> 12) & 0x0F)
#define DWC3_DEPCMD_HIPRI_FORCERM	(0x800)
#define DWC3_DEPCMD_CMDACT		(0x400)
#define DWC3_DEPCMD_CMDIOC		(0x100)

#define DWC3_DEPCMD_DEPSTARTCFG		(0x09)
#define DWC3_DEPCMD_ENDTRANSFER		(0x08)
#define DWC3_DEPCMD_UPDATETRANSFER	(0x07)
#define DWC3_DEPCMD_STARTTRANSFER	(0x06)
#define DWC3_DEPCMD_CLEARSTALL		(0x05)
#define DWC3_DEPCMD_SETSTALL		(0x04)
/* This applies for core versions 1.90a and earlier */
#define DWC3_DEPCMD_GETSEQNUMBER	(0x03)
/* This applies for core versions 1.94a and later */
#define DWC3_DEPCMD_GETEPSTATE		(0x03)
#define DWC3_DEPCMD_SETTRANSFRESOURCE	(0x02)
#define DWC3_DEPCMD_SETEPCONFIG		(0x01)

/* The EP number goes 0..31 so ep0 is always out and ep1 is always in */
#define DWC3_DALEPENA_EP(n)		((UINT32)1 << (n))

#define DWC3_DEPCMD_TYPE_CONTROL	0
#define DWC3_DEPCMD_TYPE_ISOC		1
#define DWC3_DEPCMD_TYPE_BULK		2
#define DWC3_DEPCMD_TYPE_INTR		3


/* Interrupt Moderation */
#define DWC3_DEV_IMOD_INTERVAL_MASK  0xffff
#define DWC3_DEV_IMOD_INTERVAL(x)  (x & DWC3_DEV_IMOD_INTERVAL_MASK)



/* Structures */

//struct dwc3_trb;

/**
 * struct dwc3_event_buffer - Software event buffer representation
 * @buf: _THE_ buffer
 * @length: size of this buffer
 * @lpos: event offset
 * @count: cache of last read event count register
 * @flags: flags related to this event buffer
 * @dma: dma_addr_t
 * @dwc: pointer to DWC controller
 */
struct dwc3_event_buffer {
	UINT8		*buf;
	UINT32		lpos;
	UINT32		count;
	UINT32		flags;

#define DWC3_EVENT_PENDING	BIT(0)
};

#define DWC3_EP_FLAG_STALLED	(0x0)
#define DWC3_EP_FLAG_WEDGED	(0x1)

#define DWC3_EP_DIRECTION_TX	true
#define DWC3_EP_DIRECTION_RX	false

#define DWC3_TRB_NUM		32
#define DWC3_TRB_MASK		(DWC3_TRB_NUM - 1)

/**
 * struct dwc3_ep - device side endpoint representation
 * @endpoint: usb endpoint
 * @request_list: list of requests for this endpoint
 * @req_queued: list of requests on this ep which have TRBs setup
 * @trb_pool: array of transaction buffers
 * @trb_pool_dma: dma address of @trb_pool
 * @free_slot: next slot which is going to be used
 * @busy_slot: first slot which is owned by HW
 * @desc: usb_endpoint_descriptor pointer
 * @dwc: pointer to DWC controller
 * @saved_state: ep state saved during hibernation
 * @flags: endpoint flags (wedged, stalled, ...)
 * @number: endpoint number (1 - 15)
 * @type: set to bmAttributes & USB_ENDPOINT_XFERTYPE_MASK
 * @resource_index: Resource transfer index
 * @interval: the interval on which the ISOC transfer is started
 * @name: a human readable name e.g. ep1out-bulk
 * @direction: true for TX, false for RX
 * @stream_capable: true when streams are enabled
 */
struct dwc3_ep {
	struct dwc3_trb		*trbs;
	UINT32		trbs_addr;
               const struct usb_endpoint_descriptor	*desc;
	//const struct usb_ss_ep_comp_descriptor	*comp_desc;
	//UINT32			free_slot;
	UINT8		flags;
#define DWC3_EP_ENABLED		(0x1)
#define DWC3_EP_STALL		(0x2)
#define DWC3_EP_WEDGE		(0x4)
#define DWC3_EP_BUSY		(0x10)
#define DWC3_EP_PENDING_REQUEST	(0x20)
#define DWC3_EP_MISSED_ISOC	(0x40)

	/* This last one is specific to EP0 */
#define DWC3_EP0_DIR_IN		(0x80000000)

	UINT8			number;
	UINT8			type;
	UINT8			resource_index;
//	UINT32			interval;

	unsigned		direction:1;
	unsigned		stream_capable:1;
               unsigned		maxburst:5;
               unsigned		maxpacket:16;
               UINT16                        transfer_size;
};

enum dwc3_phy {
	DWC3_PHY_UNKNOWN = 0,
	DWC3_PHY_USB3,
	DWC3_PHY_USB2,
};

enum dwc3_ep0_next {
	DWC3_EP0_UNKNOWN = 0,
	DWC3_EP0_COMPLETE,
	DWC3_EP0_NRDY_DATA,
	DWC3_EP0_NRDY_STATUS,
};

enum dwc3_ep0_state {
	EP0_UNCONNECTED		= 0,
	EP0_SETUP_PHASE,
	EP0_DATA_PHASE,
	EP0_STATUS_PHASE,
};

enum dwc3_link_state {
	/* In SuperSpeed */
	DWC3_LINK_STATE_U0		= 0x00, /* in HS, means ON */
	DWC3_LINK_STATE_U1		= 0x01,
	DWC3_LINK_STATE_U2		= 0x02, /* in HS, means SLEEP */
	DWC3_LINK_STATE_U3		= 0x03, /* in HS, means SUSPEND */
	DWC3_LINK_STATE_SS_DIS		= 0x04,
	DWC3_LINK_STATE_RX_DET		= 0x05, /* in HS, means Early Suspend */
	DWC3_LINK_STATE_SS_INACT	= 0x06,
	DWC3_LINK_STATE_POLL		= 0x07,
	DWC3_LINK_STATE_RECOV		= 0x08,
	DWC3_LINK_STATE_HRESET		= 0x09,
	DWC3_LINK_STATE_CMPLY		= 0x0a,
	DWC3_LINK_STATE_LPBK		= 0x0b,
	DWC3_LINK_STATE_RESET		= 0x0e,
	DWC3_LINK_STATE_RESUME		= 0x0f,
	DWC3_LINK_STATE_MASK		= 0x0f,
};

/* TRB Length, PCM and Status */
#define DWC3_TRB_SIZE_MASK	(0x00ffffff)
#define DWC3_TRB_SIZE_LENGTH(n)	((n) & DWC3_TRB_SIZE_MASK)
#define DWC3_TRB_SIZE_PCM1(n)	((UINT32)((n) & 0x03) << 24)
#define DWC3_TRB_SIZE_TRBSTS(n)	((UINT32)((n) & ((UINT32)0x0f << 28)) >> 28)

#define DWC3_TRBSTS_OK			0
#define DWC3_TRBSTS_MISSED_ISOC		1
#define DWC3_TRBSTS_SETUP_PENDING	2
#define DWC3_TRB_STS_XFER_IN_PROG	4

/* TRB Control */
#define DWC3_TRB_CTRL_TRBCTL(n)		((UINT32)((n) & 0x3f) << 4)
#define DWC3_TRB_CTRL_SID_SOFN(n)	((UINT32)((n) & 0xffff) << 14)

#define DWC3_TRBCTL_NORMAL		0x10
#define DWC3_TRBCTL_CONTROL_SETUP	DWC3_TRB_CTRL_TRBCTL(2)
#define DWC3_TRBCTL_CONTROL_STATUS2	DWC3_TRB_CTRL_TRBCTL(3)
#define DWC3_TRBCTL_CONTROL_STATUS3	DWC3_TRB_CTRL_TRBCTL(4)
#define DWC3_TRBCTL_CONTROL_DATA	DWC3_TRB_CTRL_TRBCTL(5)
#define DWC3_TRBCTL_ISOCHRONOUS_FIRST	DWC3_TRB_CTRL_TRBCTL(6)
#define DWC3_TRBCTL_ISOCHRONOUS		DWC3_TRB_CTRL_TRBCTL(7)
#define DWC3_TRBCTL_LINK_TRB		DWC3_TRB_CTRL_TRBCTL(8)

/**
 * struct dwc3_trb - transfer request block (hw format)
 * @bpl: DW0-3
 * @bph: DW4-7
 * @size: DW8-B
 * @trl: DWC-F
 */
#pragma pack(push,1)
struct dwc3_trb {
	UINT32		bpl;
	UINT32		bph;
	UINT32		size;
	UINT32		ctrl;
};
#pragma pack(pop)

/**
 * dwc3_hwparams - copy of HWPARAMS registers
 * @hwparams0 - GHWPARAMS0
 * @hwparams1 - GHWPARAMS1
 * @hwparams2 - GHWPARAMS2
 * @hwparams3 - GHWPARAMS3
 * @hwparams4 - GHWPARAMS4
 * @hwparams5 - GHWPARAMS5
 * @hwparams6 - GHWPARAMS6
 * @hwparams7 - GHWPARAMS7
 * @hwparams8 - GHWPARAMS8
 */
struct dwc3_hwparams {
	UINT32	hwparams0;
	UINT32	hwparams1;
	UINT32	hwparams2;
	UINT32	hwparams3;
	UINT32	hwparams4;
	UINT32	hwparams5;
	UINT32	hwparams6;
	UINT32	hwparams7;
	UINT32	hwparams8;
};

/* HWPARAMS0 */
#define DWC3_MODE(n)		((n) & 0x7)

#define DWC3_MDWIDTH(n)		((UINT32)((n) & 0xff00) >> 8)

/* HWPARAMS1 */
#define DWC3_NUM_INT(n)		((UINT32)((n) & ((UINT32)0x3f << 15)) >> 15)

/* HWPARAMS3 */
#define DWC3_NUM_EPS(hwparams3)		(((UINT32)(hwparams3) & (0x3F000)) >> 12)
#define DWC3_NUM_IN_EPS(hwparams3)	(((UINT32)(hwparams3) &	(0x7C0000)) >> 18)

/* HWPARAMS7 */
#define DWC3_RAM1_DEPTH(n)	((n) & 0xffff)

/**
 * struct dwc3 - representation of our controller
 * @ctrl_req: usb control request which is used for ep0
 * @ep0_trb: trb which is used for the ctrl_req
 * @ep0_bounce: bounce buffer for ep0
 * @setup_buf: used while precessing STD USB requests
 * @ctrl_req_addr: dma address of ctrl_req
 * @ep0_trb: dma address of ep0_trb
 * @ep0_usb_req: dummy req used while handling STD USB requests
 * @ep0_bounce_addr: dma address of ep0_bounce
 * @scratch_addr: dma address of scratchbuf
 * @lock: for synchronizing
 * @dev: pointer to our struct device
 * @xhci: pointer to our xHCI child
 * @event_buffer_list: a list of event buffers
 * @gadget: device side representation of the peripheral controller
 * @gadget_driver: pointer to the gadget driver
 * @regs: base address for our registers
 * @regs_size: address space size
 * @nr_scratch: number of scratch buffers
 * @num_event_buffers: calculated number of event buffers
 * @u1u2: only used on revisions <1.83a for workaround
 * @maximum_speed: maximum speed requested (mainly for testing purposes)
 * @revision: revision register contents
 * @dr_mode: requested mode of operation
 * @usb2_phy: pointer to USB2 PHY
 * @usb3_phy: pointer to USB3 PHY
 * @usb2_generic_phy: pointer to USB2 PHY
 * @usb3_generic_phy: pointer to USB3 PHY
 * @ulpi: pointer to ulpi interface
 * @dcfg: saved contents of DCFG register
 * @gctl: saved contents of GCTL register
 * @isoch_delay: wValue from Set Isochronous Delay request;
 * @u2sel: parameter from Set SEL request.
 * @u2pel: parameter from Set SEL request.
 * @u1sel: parameter from Set SEL request.
 * @u1pel: parameter from Set SEL request.
 * @num_out_eps: number of out endpoints
 * @num_in_eps: number of in endpoints
 * @ep0_next_event: hold the next expected event
 * @ep0state: state of endpoint zero
 * @link_state: link state
 * @speed: device speed (super, high, full, low)
 * @mem: points to start of memory which is used for this struct.
 * @hwparams: copy of hwparams registers
 * @root: debugfs root folder pointer
 * @regset: debugfs pointer to regdump file
 * @test_mode: true when we're entering a USB test mode
 * @test_mode_nr: test feature selector
 * @lpm_nyet_threshold: LPM NYET response threshold
 * @hird_threshold: HIRD threshold
 * @hsphy_interface: "utmi" or "ulpi"
 * @delayed_status: true when gadget driver asks for delayed status
 * @ep0_bounced: true when we used bounce buffer
 * @ep0_expect_in: true when we expect a DATA IN transfer
 * @has_hibernation: true when dwc3 was configured with Hibernation
 * @has_lpm_erratum: true when core was configured with LPM Erratum. Note that
 *			there's now way for software to detect this in runtime.
 * @is_utmi_l1_suspend: the core asserts output signal
 * 	0	- utmi_sleep_n
 * 	1	- utmi_l1_suspend_n
 * @is_fpga: true when we are using the FPGA board
 * @needs_fifo_resize: not all users might want fifo resizing, flag it
 * @pullups_connected: true when Run/Stop bit is set
 * @resize_fifos: tells us it's ok to reconfigure our TxFIFO sizes.
 * @setup_packet_pending: true when there's a Setup Packet in FIFO. Workaround
 * @start_config_issued: true when StartConfig command has been issued
 * @three_stage_setup: set if we perform a three phase setup
 * @usb3_lpm_capable: set if hadrware supports Link Power Management
 * @disable_scramble_quirk: set if we enable the disable scramble quirk
 * @u2exit_lfps_quirk: set if we enable u2exit lfps quirk
 * @u2ss_inp3_quirk: set if we enable P3 OK for U2/SS Inactive quirk
 * @req_p1p2p3_quirk: set if we enable request p1p2p3 quirk
 * @del_p1p2p3_quirk: set if we enable delay p1p2p3 quirk
 * @del_phy_power_chg_quirk: set if we enable delay phy power change quirk
 * @lfps_filter_quirk: set if we enable LFPS filter quirk
 * @rx_detect_poll_quirk: set if we enable rx_detect to polling lfps quirk
 * @dis_u3_susphy_quirk: set if we disable usb3 suspend phy
 * @dis_u2_susphy_quirk: set if we disable usb2 suspend phy
 * @dis_enblslpm_quirk: set if we clear enblslpm in GUSB2PHYCFG,
 *                      disabling the suspend signal to the PHY.
 * @tx_de_emphasis_quirk: set if we enable Tx de-emphasis quirk
 * @tx_de_emphasis: Tx de-emphasis value
 * 	0	- -6dB de-emphasis
 * 	1	- -3.5dB de-emphasis
 * 	2	- No de-emphasis
 * 	3	- Reserved
 */
struct dwc3 {
        //UINT32 regs;
        struct usb_ctrlrequest *ep0_ctrl_req;
        UINT32 ep0_ctrl_req_addr;    //PHY
        unsigned short is_selfpowered;
        struct dwc3_trb *ep0_trbs;
        UINT32 ep0_trbs_addr;
        unsigned char *ep0_setup_buf;
        UINT32 ep0_setup_buf_addr;
        UINT32 ep0_descriptor_table_buf;
        unsigned short ep0_setup_buf_len;
        unsigned short ep0_actual_buf_len;
        unsigned char ep0_last_callback_id;
        #define USB_CDC_SET_LINE_CODING_ID      1
        #define USB_UAC_SET_CUR_ID              2
        #define USB_UVC_EX_SET_CUR_ID_CS1       3
        #define USB_UVC_EX_SET_CUR_ID_CS2       4
        #define USB_UVC_PROCESSING_UINT_SET_CUR 5
        #define USB_UVC_EX_SET_CUR_ID_CS3       6
        #define USB_UVC_SET_PROBE               7
        #define USB_UVC_SET_COMMIT              8
        #define USB_UVC_EX_SET_CUR_ID_CS4       9
        #define USB_HID_SET_REPORT              10
        enum usb_device_state		state;
//	UINT32			revision;

#define DWC3_REVISION_173A	0x5533173a
#define DWC3_REVISION_175A	0x5533175a
#define DWC3_REVISION_180A	0x5533180a
#define DWC3_REVISION_183A	0x5533183a
#define DWC3_REVISION_185A	0x5533185a
#define DWC3_REVISION_187A	0x5533187a
#define DWC3_REVISION_188A	0x5533188a
#define DWC3_REVISION_190A	0x5533190a
#define DWC3_REVISION_194A	0x5533194a
#define DWC3_REVISION_200A	0x5533200a
#define DWC3_REVISION_202A	0x5533202a
#define DWC3_REVISION_210A	0x5533210a
#define DWC3_REVISION_220A	0x5533220a
#define DWC3_REVISION_230A	0x5533230a
#define DWC3_REVISION_240A	0x5533240a
#define DWC3_REVISION_250A	0x5533250a
#define DWC3_REVISION_260A	0x5533260a
#define DWC3_REVISION_270A	0x5533270a
#define DWC3_REVISION_280A	0x5533280a

/*
 * NOTICE: we're using bit 31 as a "is usb 3.1" flag. This is really
 * just so dwc31 revisions are always larger than dwc3.
 */
#define DWC3_REVISION_IS_DWC31		0x80000000
#define DWC3_USB31_REVISION_110A	(0x3131302a | DWC3_REVISION_IS_USB31)

	UINT8			num_eps;
	UINT8			num_in_eps;
	UINT8			speed;
	UINT8			ep_out_in_number;
	UINT16			maxpacket;
	UINT8			dwc_use_cdc;
	UINT8			dwc_need_reconnect;
	struct dwc3_ep                      eps[USB_EP_COUNT];
	UINT32 eps_addr;

	enum dwc3_ep0_next	ep0_next_event;
	enum dwc3_ep0_state	ep0state;

        //unsigned		delayed_status:1;
	unsigned		ep0_bounced:1;
	unsigned		ep0_expect_in:1;
	unsigned		is_utmi_l1_suspend:1;
	unsigned		needs_fifo_resize:1;
	unsigned		resize_fifos:1;
	unsigned		setup_packet_pending:1;
	unsigned		start_config_issued:1;
	unsigned		three_stage_setup:1;
               struct dwc3_event_buffer g_event_buf;
    int irq_usb;
    int irq_uvc;
    int irq_uvc_vgip;
    /* device lock */
    spinlock_t     usb_device_mac_lock;
    UINT8 uvc_plant_cnt;
    UINT8 wdma_wdone;
    UINT8 timer_done;
    UINT8 frame_lc_done;
    UINT8 uvc_show_log;
};

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
#pragma pack(push,1)
#ifdef CONFIG_ARCH_5281
struct dwc3_event_type {
	u32	is_devspec:1;
	u32	type:7;
	u32	reserved8_31:24;
};
#else
struct dwc3_event_type {
	UINT32	is_devspec:1;
	UINT32	type:7;
	UINT32	reserved8_31:8;
};
#endif
#pragma pack(pop)

#define DWC3_DEPEVT_XFERCOMPLETE	0x01
#define DWC3_DEPEVT_XFERINPROGRESS	0x02
#define DWC3_DEPEVT_XFERNOTREADY	0x03
#define DWC3_DEPEVT_RXTXFIFOEVT		0x04
#define DWC3_DEPEVT_STREAMEVT		0x06
#define DWC3_DEPEVT_EPCMDCMPLT		0x07

/**
 * struct dwc3_event_depvt - Device Endpoint Events
 * @one_bit: indicates this is an endpoint event (not used)
 * @endpoint_number: number of the endpoint
 * @endpoint_event: The event we have:
 *	0x00	- Reserved
 *	0x01	- XferComplete
 *	0x02	- XferInProgress
 *	0x03	- XferNotReady
 *	0x04	- RxTxFifoEvt (IN->Underrun, OUT->Overrun)
 *	0x05	- Reserved
 *	0x06	- StreamEvt
 *	0x07	- EPCmdCmplt
 * @reserved11_10: Reserved, don't use.
 * @status: Indicates the status of the event. Refer to databook for
 *	more information.
 * @parameters: Parameters of the current event. Refer to databook for
 *	more information.
 */
#pragma pack(push,1)
struct dwc3_event_depevt {
	UINT16	one_bit:1;
	UINT16	endpoint_number:5;
	UINT16	endpoint_event:4;
	UINT16	reserved11_10:2;
	UINT16	status:4;

/* Within XferNotReady */
#define DEPEVT_STATUS_TRANSFER_ACTIVE	((UINT32)1 << 3)

/* Within XferComplete */
#define DEPEVT_STATUS_BUSERR	((UINT32)1 << 0)
#define DEPEVT_STATUS_SHORT	((UINT32)1 << 1)
#define DEPEVT_STATUS_IOC	((UINT32)1 << 2)
#define DEPEVT_STATUS_LST	((UINT32)1 << 3)

/* Stream event only */
#define DEPEVT_STREAMEVT_FOUND		1
#define DEPEVT_STREAMEVT_NOTFOUND	2

/* Control-only Status */
#define DEPEVT_STATUS_CONTROL_DATA	1
#define DEPEVT_STATUS_CONTROL_STATUS	2

	//UINT32	parameters:16;
};
#pragma pack(pop)

/**
 * struct dwc3_event_devt - Device Events
 * @one_bit: indicates this is a non-endpoint event (not used)
 * @device_event: indicates it's a device event. Should read as 0x00
 * @type: indicates the type of device event.
 *	0	- DisconnEvt
 *	1	- USBRst
 *	2	- ConnectDone
 *	3	- ULStChng
 *	4	- WkUpEvt
 *	5	- Reserved
 *	6	- EOPF
 *	7	- SOF
 *	8	- Reserved
 *	9	- ErrticErr
 *	10	- CmdCmplt
 *	11	- EvntOverflow
 *	12	- VndrDevTstRcved
 * @reserved15_12: Reserved, not used
 * @event_info: Information about this event
 * @reserved31_25: Reserved, not used
 */
#pragma pack(push,1)
struct dwc3_event_devt {
	UINT32	one_bit:1;
	UINT32	device_event:7;
	UINT32	type:4;
	UINT32	reserved15_12:4;
	UINT32	event_info:9;
	UINT32	reserved31_25:7;
};
#pragma pack(pop)

/**
 * struct dwc3_event_gevt - Other Core Events
 * @one_bit: indicates this is a non-endpoint event (not used)
 * @device_event: indicates it's (0x03) Carkit or (0x04) I2C event.
 * @phy_port_number: self-explanatory
 * @reserved31_12: Reserved, not used.
 */
#pragma pack(push,1)
struct dwc3_event_gevt {
	UINT32	one_bit:1;
	UINT32	device_event:7;
	UINT32	phy_port_number:4;
	UINT32	reserved31_12:4;
};
#pragma pack(pop)

/**
 * union dwc3_event - representation of Event Buffer contents
 * @raw: raw 32-bit event
 * @type: the type of the event
 * @depevt: Device Endpoint Event
 * @devt: Device Event
 * @gevt: Global Event
 */
union dwc3_event {
	UINT32				raw;
	struct dwc3_event_type		type;
	struct dwc3_event_depevt	depevt;
	struct dwc3_event_devt		devt;
	struct dwc3_event_gevt		gevt;
};

/**
 * struct dwc3_gadget_ep_cmd_params - representation of endpoint command
 * parameters
 * @param2: third parameter
 * @param1: second parameter
 * @param0: first parameter
 */
struct dwc3_gadget_ep_cmd_params {
	UINT32	param2;
	UINT32	param1;
	UINT32	param0;
};

/*
 * DWC3 Features to be used as Driver Data
 */

#define DWC3_HAS_PERIPHERAL		BIT(0)
#define DWC3_HAS_XHCI			BIT(1)
#define DWC3_HAS_OTG			BIT(3)


/* DEPCFG parameter 1 */
#define DWC3_DEPCFG_INT_NUM(n)              ((UINT32)(n) << 0)
#define DWC3_DEPCFG_XFER_COMPLETE_EN        ((UINT32)1 << 8)
#define DWC3_DEPCFG_XFER_IN_PROGRESS_EN     ((UINT32)1 << 9)
#define DWC3_DEPCFG_XFER_NOT_READY_EN       ((UINT32)1 << 10)
#define DWC3_DEPCFG_FIFO_ERROR_EN           ((UINT32)1 << 11)
#define DWC3_DEPCFG_STREAM_EVENT_EN         ((UINT32)1 << 13)
#define DWC3_DEPCFG_BINTERVAL_M1(n)         ((UINT32)(n) << 16)
#define DWC3_DEPCFG_STREAM_CAPABLE          ((UINT32)1 << 24)
#define DWC3_DEPCFG_EP_NUMBER(n)            ((UINT32)(n) << 25)
#define DWC3_DEPCFG_BULK_BASED              ((UINT32)1 << 30)
#define DWC3_DEPCFG_FIFO_BASED              ((UINT32)1 << 31)

/* DEPCFG parameter 0 */
#define DWC3_DEPCFG_EP_TYPE(n)                      ((UINT32)(n) << 1)
#define DWC3_DEPCFG_MAX_PACKET_SIZE(n)              ((UINT32)(n) << 3)
#define DWC3_DEPCFG_FIFO_NUMBER(n)                  ((UINT32)(n) << 17)
#define DWC3_DEPCFG_BURST_SIZE(n)                   ((UINT32)(n) << 22)
#define DWC3_DEPCFG_DATA_SEQ_NUM(n)                 ((UINT32)(n) << 26)
/* This applies for core versions earlier than 1.94a */
#define DWC3_DEPCFG_IGN_SEQ_NUM                     ((UINT32)1 << 31)
/* These apply for core versions 1.94a and later */
#define DWC3_DEPCFG_ACTION_INIT                     ((UINT32)0 << 30)
#define DWC3_DEPCFG_ACTION_RESTORE                  ((UINT32)1 << 30)
#define DWC3_DEPCFG_ACTION_MODIFY                   ((UINT32)2 << 30)

/* DEPXFERCFG parameter 0 */
#define DWC3_DEPXFERCFG_NUM_XFER_RES(n)             (UINT32)((n) & 0xffff)

#define USB_GADGET_DELAYED_STATUS       0x7fff	/* Impossibly large value */

#endif /* __USB_REG_H */
