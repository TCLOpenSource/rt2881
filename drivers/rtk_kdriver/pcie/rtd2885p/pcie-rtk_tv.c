/*
 * PCIe host controller driver for Realtek TV SoC
 *
 * Copyright (C) 2021 Realtek Corporation.
 *
 */

#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/phy/phy.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/random.h>
#include <linux/reset.h>
#include <linux/resource.h>
#include <linux/types.h>

#include <rtk_kdriver/io.h>
#include <rbus/pcie_wrap_reg.h>
//#include <rbus/gpio_reg.h>
#include <rbus/sb2_reg.h>
#include <rtk_kdriver/rtk_gpio.h>
#include <pcbMgr.h>
#include "../../pci/controller/dwc/pcie-designware.h"
#include "pcie-rtk_tv.h"
#include <mach/rtk_platform.h>
#include <mach/platform.h>
#include <linux/version.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include <rtk_kdriver/pcbMgr.h>
#include <rbus/M8P_dc_sys_reg.h>
#include "optee_smc.h"

#define MER8_CHANGE     //register change on merlin8
//#define MARK_FOR_FPGA   //mark gpio setting for FPGA

/*
MEMIO 3G~4G for PCIe
Note3: GPU: 0xFF7F_0000 ~ 0xFFFE_FFFF,can't use
Note4: ROM: 0xFFFF_0000 ~ 0xFFFF_FFFF,can't use
3G ==> 0xc0000000

pcie_start_addr ~ (pcie_start_addr + 4k - 1) is for DBI to access RC configuration space,
  if access over 4k range must set host_sys_bus_dbi_addr_31_12.
(pcie_start_addr + 4k) ~ (pcie_start_addr + 8k - 1) is for XALI to access EP configuration space
(pcie_start_addr + 8k) ~ pcie_end_addr is for XALI to access EP memory space

dbi_req_base_addr = reg_pcie_start_addr
cfg_req_base_addr = reg_pcie_start_addr + 4096
mem_req_base_addr = reg_pcie_start_addr + 8192

*/

#define RTK_PCI_REGION_LENGTH   (0x02000000)    //total 32M for now
#define RTK_PCI_REGION_START    (0xfc000000)    //(4G-64M)//(0xc0000000)
#define RTK_PCI_REGION_END      (RTK_PCI_REGION_START+RTK_PCI_REGION_LENGTH-1)


//#define REASSIGN_ID

#define PCIE_RTK_VID        (0x10ECUL )
#define PCIE_RTK_PID        (0x2001UL )

#define PCIE_PIN_RESET_PCB_NAME  "PIN_PCIE_RST"

struct device *dev_local = NULL;
struct dw_pcie *pci_local = NULL;
struct pcie_port *pp_local = NULL;
u32 pcie_enable;
static u32 pcie_link_speed = GEN2;
u32 pcie_rw_req_dealy = 0;
u32 pcie_disable_rbus_timeout = 1;  //check it on real IC
u32 pcie_enable_reginfo = 0;
static DEFINE_SPINLOCK(rtk_pcie_send_req_lock);

static int rtk_pcie_set_link_speed(struct dw_pcie *pci, e_pciegen speed, u32 initiate_speed_change);
static void rtk_pcie_perst_hold(void);
static void rtk_pcie_perst_release(void);
void write_pcie_mdio(u16 mdio_addr, u16 data);
#ifdef CONFIG_RTK_KDRV_PCIE_MODULE
static int parse_pcie_enable_handler(void);
#endif
struct rtk_pcie_dw {
    struct device *dev;

    struct resource *xali_res;
    struct resource *dbi_res;
    struct resource *wrap_res;
    void __iomem *xali_base;
    void __iomem *wrap_base;

    struct clk *core_clk;
    struct reset_control *core_apb_rst;
    struct reset_control *core_rst;
    struct dw_pcie pci;
    u32 region_start;
    u32 region_length;
    u32 msi_mask;
    u32 ic_ver;
    struct dentry *debugfs;
};


static inline struct rtk_pcie_dw *to_rtk_pcie(struct dw_pcie *pci)
{
    return container_of(pci, struct rtk_pcie_dw, pci);
}

static __maybe_unused
u32 pci_bit_mask(u8 byte_mask)
{
    int i;
    u32 mask = 0;

    for (i = 0; i < 4; i++) {
        if ((byte_mask >> i) & 0x1)
            mask |= (0xFF << (i << 3));
    }

    return mask;
}

static __maybe_unused
u32 pci_bit_shift(u32 addr)
{
    return ((addr & 0x3) << 3);
}

static __maybe_unused
u32 pci_byte_mask(u32 addr, u8 size)
{
    u8 offset = (addr & 0x03);

    switch (size) {
    case 0x01:
        return 0x1 << offset;
    case 0x02:
        if (offset <= 2)
            return 0x3 << offset;

        PCIE_ALERT("compute config mask - data cross dword boundrary (addr=0x%x, length=2)\n", addr);
        break;

    case 0x03:
        if (offset <= 1)
            return 0x7 << offset;

        PCIE_ALERT("compute config mask - data cross dword boundrary (addr=0x%x, length=3)\n",  addr);
        break;

    case 0x04:
        if (offset == 0)
            return 0xF;

        PCIE_ALERT("compute config mask - data cross dword boundrary (addr=0x%x, length=4)\n", addr);
        break;

    default:
        PCIE_ALERT("compute config mask failed - size %d should between 1~4\n", size);
    }
    return 0;
}

static __maybe_unused
u32 _pci_address_conversion(struct pci_bus *bus,
                        unsigned int devfn, int reg)
{
    int busno = bus->number;
    int dev = PCI_SLOT(devfn);
    int func = PCI_FUNC(devfn);

    return (busno << 24) | (dev << 19) | (func << 16) | reg;
}

static int rtk_pcie_int_crt_enable(u32 enable)
{
    u32 en_val;

    en_val = (enable)?1:0;
#if 0   //def MER8_CHANGE
    rtd_outl(0xb80002A4,
        BIT(24)| 1);
#else
    rtd_outl(SYS_REG_INT_CTRL_SCPU_2_reg,
        SYS_REG_INT_CTRL_SCPU_2_pcie_int_scpu_routing_en(1)|
        SYS_REG_INT_CTRL_SCPU_2_write_data(en_val));
#endif
    return 0;
}


int rtk_pcie_int_INTx_misc_enable(u32 enable)
{
#if 0   //no used now
    u32 en_val;

    en_val = (enable)?1:0;

    rtd_outl(PCIE_WRAP_PCIE_SII_INT_CTRL1_reg,
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_bw_mgt_msi_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_bw_mgt_int_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_link_auto_bw_msi_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_link_auto_bw_int_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_hp_msi_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_hp_int_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_hp_pme_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_intd_deasserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_intc_deasserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_intb_deasserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_inta_deasserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_intd_asserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_intc_asserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_intb_asserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_radm_inta_asserted_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_pme_msi_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_pme_int_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_aer_rc_err_msi_en(en_val) |
        PCIE_WRAP_PCIE_SII_INT_CTRL1_cfg_aer_rc_err_int_en(en_val));
#endif
    return 0;
}


static void rtk_pcie_int_msi_sta_clr(void)
{
    //for merlin8, it's should no needed to clear
/*
    rtd_outl(PCIE_WRAP_PCIE_INT_PENDING_STATUS1_reg,
        PCIE_WRAP_PCIE_INT_PENDING_STATUS1_int_pending(1));
    rtd_outl(PCIE_WRAP_PCIE_MSI_STATUS2_reg,
        PCIE_WRAP_PCIE_MSI_STATUS2_msi_ctrl_int(1));
*/
}


void rtk_pcie_int_msi_sta_clr_exp(void)
{
    rtk_pcie_int_msi_sta_clr();
}
EXPORT_SYMBOL(rtk_pcie_int_msi_sta_clr_exp);

static void rtk_pcie_int_msi_enable(struct pcie_port* pp, u32 enable)
{
    u32 en_val;

    en_val = (enable)?1:0;
    //if(en_val)
    //    dw_pcie_msi_init(pp);

    rtd_outl(PCIE_WRAP_PCIE_MSI_CTRL1_reg,
        PCIE_WRAP_PCIE_MSI_CTRL1_msi_ctrl_int_en(en_val));
}

static irqreturn_t rtk_pcie_irq_handler(int irq, void *arg)
{
    u32 reg_info;
    u32 reg_addr;

    //PCIE_SII_INT_STATUS1
    reg_addr = PCIE_WRAP_PCIE_SII_INT_STATUS1_reg;  //0xb807E194;
    reg_info = rtd_inl(reg_addr);
    rtd_outl(reg_addr,0xffffffff);

    reg_addr = PCIE_WRAP_PCIE_SII_INT_STATUS2_reg;  //0xb807E198;
    reg_info = rtd_inl(reg_addr);
    rtd_outl(reg_addr,0xffffffff);

    reg_addr = PCIE_WRAP_PCIE_INT_PENDING_STATUS1_reg;  //0xb807E2A4;
    reg_info = rtd_inl(reg_addr);

    return IRQ_HANDLED;

}

static void rtk_pcie_enable_interrupts(struct pcie_port *pp)
{
    struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
    __maybe_unused struct rtk_pcie_dw *pcie = to_rtk_pcie(pci);

    rtk_pcie_int_crt_enable(1);

    rtk_pcie_int_INTx_misc_enable(1);   //for test disable all now

    if (IS_ENABLED(CONFIG_PCI_MSI)){
        rtk_pcie_int_msi_enable(pp,1);
    }

    return;
}

/************************************************************************
 * phy reg
 * [31:16]: value
 * [15: 8]: addr
 *      ==> bit14:  0->Gen1;1->Gen2;
 *      ==> bit13:  0->page0;1->page1;
 * [ 7: 0]: 0x0d(write)
 ************************************************************************/
#define PCIE_WAIT_PHY_TIME (500)

#define MDIO_GEN1   (0)
#define MDIO_GEN2   BIT(6)
#define MDIO_PAGE0  (0)
#define MDIO_PAGE1  BIT(5)

static void rtk_pcie_phy_setting(struct pcie_port *pp)
{
#define PHY_SETTING_31212
#if defined (PHY_SETTING_31212)
    // PCIE PHY Parameter_fine tune Gen1_2003/1212 /////
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE0)+0x01, 0x2850);   //page0, reg 0x1=0x2850
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE0)+0x06, 0x0017);   //page0, reg 0x6=0x0017
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE0)+0x0a, 0x9270);   //page0, reg 0xa=0x9270
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE0)+0x0e, 0x1001);   //page0, reg 0xe=0x1001
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE0)+0x1e, 0x66eb);   //page0, reg 0x1e=0x66eb
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE1)+0x00, 0xc4c7);   //page1, reg 0x0=0xc4c7
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE1)+0x01, 0x577a);   //page1, reg 0x1=0x577a
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN1|MDIO_PAGE1)+0x04, 0x4724);   //page1, reg 0x4=0x4724
    udelay(PCIE_WAIT_PHY_TIME);

    // PCIE PHY Parameter_fine tune Gen1_2003/1212 /////
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE0)+0x01, 0x284a);   //page0, reg 0x1=0x284a
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE0)+0x06, 0x0017);   //page0, reg 0x6=0x0017
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE0)+0x07, 0x2e00);   //page0, reg 0x7=0x2e00
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE0)+0x0a, 0x9250);   //page0, reg 0xa=0x9250
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE0)+0x0e, 0x1001);   //page0, reg 0xe=0x1001
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE0)+0x1c, 0xc000);   //page0, reg 0x1c=0xc000
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE0)+0x1e, 0x6eeb);   //page0, reg 0x1e=0x6eeb
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE1)+0x00, 0xc4c9);   //page1, reg 0x0=0xc4c9
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE1)+0x01, 0x579a);   //page1, reg 0x1=0x579a
    udelay(PCIE_WAIT_PHY_TIME);
    write_pcie_mdio((MDIO_GEN2|MDIO_PAGE1)+0x0f, 0x7008);   //page1, reg 0xf=0x7008
    udelay(PCIE_WAIT_PHY_TIME);

#else
    // PCIE PHY Parameter_fine tune Gen1_11/12 /////
    rtd_outl(0xb807e280,0xa852010d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x6042020d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x001d060d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0xa2700a0d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x0b66230d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x4f10240d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0xf802280d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x1d602a0d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0xa8012b0d);
    udelay(PCIE_WAIT_PHY_TIME);
    //// Mark2_PCIE PHY Parameter_fine tune Gen2_11/12 /////
    rtd_outl(0xb807e280,0xa848410d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x6042420d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x001d460d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0xa2704a0d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x0b66630d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x4f20640d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0xf802680d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0x1d606a0d);
    udelay(PCIE_WAIT_PHY_TIME);
    rtd_outl(0xb807e280,0xa8016b0d);
    udelay(PCIE_WAIT_PHY_TIME);
#endif
}

#define MDIO_WR     (0x0DUL)
#define MDIO_RD     (0x0CUL)

#define  get_mdio_data(data)    ((0xFFFF0000&(data))>>16)
#define  mdio_data(data)        (0xFFFF0000&((data)<<16))

//#define CALIBRA_DEBUG
#ifdef CALIBRA_DEBUG
#define CAL_DEBUG(fmt, args...)      \
    PCIE_ALERT(fmt , ## args)
#else
#define CAL_DEBUG(fmt, args...)
#endif

u16 read_pcie_mdio(u16 mdio_addr)
{
    u32 reg_info = 0;
    u32 data;

    reg_info = ((u32)mdio_addr << 8) | MDIO_RD;
    rtd_outl(PCIE_WRAP_PCIE_MDIO_CTRL1_reg,reg_info);
    udelay(100);

    data = rtd_inl(PCIE_WRAP_PCIE_MDIO_CTRL1_reg);
    CAL_DEBUG("R mdio [0x%08x] 0x%08x=0x%08x\n",reg_info, (u32)mdio_addr,data);

    return (u16)get_mdio_data(data);
}

void write_pcie_mdio(u16 mdio_addr, u16 data)
{
    u32 reg_info;

    reg_info = mdio_data((u32) data) | ((u32) mdio_addr << 8) | MDIO_WR;
    CAL_DEBUG("W mdio [0x%08x] 0x%08x=0x%08x\n",reg_info, (u32)mdio_addr,(u32)data);
    rtd_outl(PCIE_WRAP_PCIE_MDIO_CTRL1_reg,reg_info);
    udelay(100);
#ifdef CALIBRA_DEBUG
    read_pcie_mdio(u16 mdio_addr);  //for deug
#endif
    return;
}

void write_pcie_mdio_mask(u16 mdio_addr, u16 mask, u16 sht, u16 val)
{
    u16 reg_info;

    reg_info = read_pcie_mdio(mdio_addr);
    CAL_DEBUG("W_M read mdio 0x%08x=0x%08x\n",(u32)mdio_addr,(u32)reg_info);
    reg_info &= ~(mask << sht);
    reg_info |= (val << sht);
    CAL_DEBUG("W_M mdio 0x%08x mask=0x%08x sht=0x%08x val=0x%08x info=0x%08x\n",
        (u32)mdio_addr,(u32)mask,(u32)sht,(u32)val,(u32)reg_info);
    write_pcie_mdio(mdio_addr,reg_info);

    return;
}

u16 read_pcie_mdio_mask(u16 mdio_addr, u16 mask, u16 sht)
{
    u16 reg_info;

    reg_info = read_pcie_mdio(mdio_addr);
    CAL_DEBUG("R_M read mdio 0x%08x=0x%08x\n",(u32)mdio_addr,(u32)reg_info);
    reg_info = (reg_info >> sht) & mask;
    CAL_DEBUG("R_M mdio 0x%08x mask=0x%08x sht=0x%08x info=0x%08x\n",
        (u32)mdio_addr,(u32)mask,(u32)sht,(u32)reg_info);

    return (u16)reg_info;
}

static __maybe_unused
u32 wait_pcie_mdio_mask(u16 mdio_addr, u16 mask, u16 sht, u16 val, u32 x100us)
{
    u32 loop = x100us;
    u16 reg1;
    u16 ret = 0;

    while(1){   //waiting 0x1F/0x5F Bit[7] ==0
        reg1 = read_pcie_mdio_mask(mdio_addr, mask, sht);
        CAL_DEBUG("%s(%d)reg1=0x%x val=0x%x\n",__func__,__LINE__,reg1,val);
        if(reg1 == val ){
            ret = 1;
            break;
        }
        udelay(50);
        loop--;
        if(loop ==0 ){  //(50+50)us * loop
            CAL_DEBUG("%s(%d)wait 0x%04x mask=0x%x sht=%d val=0x%x timeout!!!\n",__func__,__LINE__);
            break;
        }
    }
    return ret;
}

#define SIMPLE_CAL_FLOW //skip OOBS refine
static void rtk_pcie_phy_calibration(struct pcie_port *pp,e_pciegen gen)
{
    u32 reg1;
    u32 offset = 0;
#ifndef SIMPLE_CAL_FLOW
    u32 oobs1;
#endif
    PCIE_ALERT("%s(%d)calibrate Gen%d\n",__func__,__LINE__,(u32)gen);
    if((gen != GEN1) && (gen != GEN2)){
        PCIE_ALERT("unsupported GEN%d, skip calibration.\n",(u32)gen);
        return;
    }

    rtk_pcie_perst_hold();   // holding device

    if(gen == GEN2)
        offset = 0x40;

    //OOBS_SEL=0x0(OOBS Auto);OOBS_Forcecal=[1]
    write_pcie_mdio(0x09 + offset,0x720c);   //0x09 = 0x720c
    //OOBS_Forcecal=[0]
    write_pcie_mdio(0x09 + offset,0x700c);   //0x09 = 0x700c
    //OOBS_Forcecal=[1]
    write_pcie_mdio(0x09 + offset,0x720c);   //0x09 = 0x720c
    //RX_TEST_EN=[1]
    write_pcie_mdio_mask(0x0d + offset, 1, 6, 1);    //0x0D[6]=[1]
    //0x19/0x59 Bit[2]=[1] ; 0x10/0x50=0x03C4
    write_pcie_mdio_mask(0x19 + offset, 1, 2, 1);   //0x19[2]=[1]
    write_pcie_mdio(0x10 + offset,0x03C4);          //0x10 = 0x03C4

#ifndef SIMPLE_CAL_FLOW
    //RX_Debug port[7] = RXIDLE; 0x1F/0x5F Bit[6]
    reg1 = wait_pcie_mdio_mask(0x1f + offset, 1, 6, 0, 20);
    if(reg1 == 0 ){
        PCIE_ALERT("%s(%d)wait RXIDLE fail\n",__func__,__LINE__);
        goto CAL_FAIL;
    }
    mdelay(1);

    //0x19/x049 Bit[2]=[1] ; 0x10/0x50=0x03C4
    write_pcie_mdio_mask(0x19 + offset, 1, 2, 1);   //0x19/0x49 Bit[2]=[1]
    write_pcie_mdio(0x10 + offset,0x03c4);          //0x10/0x50 = 0x03c4
    //get OOBS Debug port 0x1F/0x5F Bit[12:8]
    oobs1 = read_pcie_mdio_mask(0x1f + offset, 0x1f, 8);
    PCIE_ALERT("oobs1=0x%08x\n",oobs1);
    //fill OOBS to 0x03/0x43[5:1]
    write_pcie_mdio_mask(0x03 + offset, 0x1f, 1, oobs1);
    //0x09/0x49 OOBS_SEL=0x1(OOBS Manual)
    write_pcie_mdio(0x09 + offset,0x721C);   //0x09 = 0x721C
    //wait 0x1F/0x5F Bit[15] =[1]
#endif

    if(gen == GEN1){
        reg1 = wait_pcie_mdio_mask(0x1f + offset, 1, 15, 1, 40);
        if(reg1 == 0 ){
            PCIE_ALERT("%s(%d)wait FILTER_OUT_EQ fail\n",__func__,__LINE__);
            goto CAL_FAIL;
        }
    }else{
        PCIE_ALERT("Gen%d skip check Front End Offset\n",gen);
    }

    mdelay(1);

    //0x19/0x59 Bit[2]=[0] ; 0x10/0x50=0x0C 0x0D/0x4D[6]=[0]
    write_pcie_mdio_mask(0x19 + offset, 1, 2, 0);   //0x19/0x59 Bit[2]=[0]
    write_pcie_mdio(0x10 + offset,0x0C);            //0x10/0x50 = 0x0C
    write_pcie_mdio_mask(0x0D + offset, 1, 6, 0);   //0x0D/0x4D[6]=[0]
    //0x1F/0x5F Bit[4:1] RX_OFFSET_CODE
    reg1 = read_pcie_mdio_mask(0x1f + offset, 0xF, 1);
    PCIE_ALERT("RX_OFFSET_CODE=0x%08x\n",reg1);

    return;

CAL_FAIL:
    PCIE_ALERT("phy calibrattion fail\n");
    return;

}

static
u32 rtk_pcie_read_dbi(struct dw_pcie *pci, void __iomem *base,
                u32 reg, size_t size)
{
    u64 dbase = 0;
    u32 offset = 0;
    u32 reginfo = 0;

    if(reg > 0xFFF){
        WARN(1,"reg over range ");
    }

    dbase = (u64)base - (u64)(pci->dbi_base);
    rtd_outl(PCIE_WRAP_PCIE_HOST_SYS_BUS_CTRL4_reg, (u32)dbase);  //0xB807E03C

    offset = reg & 0x00000fff;
    if (size == 4){
        reginfo = readl(pci->dbi_base+offset);
    }else if (size == 2){
        reginfo = readw(pci->dbi_base+offset);
    }else if (size == 1){
        reginfo = readb(pci->dbi_base+offset);
    }else{
        BUG_ON(1);
    }
#ifdef RTK_PCIE_DEBUG
    if(dbase){
        PCIE_ALERT("%s(%d)reg=0x%08x size=%d val=0x%08x base=0x%llx, dbi_base=0x%llx dbase=0x%llx \n",
            __func__,__LINE__,reg,size,reginfo,(u64)base, (u64)pci->dbi_base,dbase);
    }
#endif

    if(pcie_enable_reginfo){
        PCIE_EMERG("%s(%d)reg=0x%08x size=%d val=0x%08x base=0x%llx, dbi_base=0x%llx dbase=0x%llx \n",
            __func__,__LINE__,reg,size,reginfo,(u64)base, (u64)pci->dbi_base,dbase);
    }
    return reginfo;
}

static
void rtk_pcie_write_dbi(struct dw_pcie *pci, void __iomem *base,
                u32 reg, size_t size, u32 val)
{
    u64 dbase = 0;
    u32 offset = 0;

    if(reg > 0xFFF){
        WARN(1,"reg over range ");
    }

    dbase = (u64)base - (u64)(pci->dbi_base);
    rtd_outl(PCIE_WRAP_PCIE_HOST_SYS_BUS_CTRL4_reg, (u32)dbase);  //0xb807E03C

    if(pcie_enable_reginfo){
        PCIE_ALERT("%s(%d)reg=0x%08x size=%d val=0x%08x base=0x%llx, dbi_base=0x%llx dbase=0x%llx \n",
            __func__,__LINE__,reg,size,val,(u64)base, (u64)pci->dbi_base,dbase);
    }
#ifdef RTK_PCIE_DEBUG
    if(dbase){
        PCIE_ALERT("%s(%d)reg=0x%08x size=%d val=0x%08x base=0x%llx, dbi_base=0x%llx dbase=0x%llx \n",
            __func__,__LINE__,reg,size,val,(u64)base, (u64)pci->dbi_base,dbase);
    }
#endif
    offset = reg & 0x00000fff;
    if (size == 4){
        writel((u32)val, pci->dbi_base+offset);
    }else if (size == 2){
        writew((u16)val, pci->dbi_base+offset);
    }else if (size == 1){
        writeb((u8)val, pci->dbi_base+offset);
    }else{
        BUG_ON(1);
    }

    return;
}

static int send_rd_requset(struct pcie_port *pp,PCIE_REQ_CTX *req_ctx,int size)
{
    u32 slv_sb_ctrl5 = 0;
    u32 xali_addr_base;
    void __iomem *xali_addr;
    u32 resp;
    unsigned long flags;
    u32 sb2_reg;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 9, 0))
    struct dw_pcie *pci = NULL;
    struct rtk_pcie_dw *pcie = NULL;

    pci = to_dw_pcie_from_pp(pp);
    pcie = to_rtk_pcie(pci);
#endif
    spin_lock_irqsave(&rtk_pcie_send_req_lock, flags);

    if (pcie_disable_rbus_timeout) {
        sb2_reg = (rtd_inl(SB2_INV_INTEN_reg) &
            (SB2_INV_INTEN_to_en_mask | SB2_INV_INTEN_to_en1_mask | SB2_INV_INTEN_to_end_mask));
        rtd_outl(SB2_INV_INTEN_reg, sb2_reg);  // disable rbus timeout
    }

    //set sideband signals
    slv_sb_ctrl5 =
        BIT(24) |   /* PCIE_WRAP_PCIE_AXI_SLAVE_SB_CTRL1_update_slv_awmisc_info_tlp_type(data) */
        ((req_ctx->tlp_type     & 0x1f)<< 0) |
        ((req_ctx->np           & 0x01)<< 5) |
        ((req_ctx->a_ep         & 0x01)<< 6) |
        ((req_ctx->ns           & 0x01)<< 8) |
        ((req_ctx->ro           & 0x01)<< 9) |
        ((req_ctx->tc           & 0x01)<<10) |
        ((req_ctx->msg_code     & 0x01)<<13) |
        ((req_ctx->dbi_access   & 0x01)<<21) |
        ((req_ctx->atu_bypass   & 0x01)<<22);

    rtd_outl(PCIE_WRAP_PCIE_AXI_SLAVE_SB_CTRL5_reg, slv_sb_ctrl5);

    //set xali address
    xali_addr_base = (req_ctx->addr & 0xfffff000);
    rtd_outl(PCIE_WRAP_PCIE_HOST_SYS_BUS_CTRL5_reg,xali_addr_base);  //0xb807E040

    udelay(pcie_rw_req_dealy);

    //initialte read request
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
    xali_addr = pp->va_cfg0_base + (req_ctx->addr & 0xfff);
#else
    xali_addr = pcie->xali_base + (req_ctx->addr & 0xfff);
#endif

    if (size == 4){
        req_ctx->data = readl(xali_addr);
    }else if (size == 2){
        req_ctx->data = readw(xali_addr);
    }else if (size == 1){
        req_ctx->data = readb(xali_addr);
    }else{
        BUG_ON(1);
    }

    resp= rtd_inl(PCIE_WRAP_PCIE_AXI_SLAVE_SB_STATUS1_reg);
    req_ctx->resp = (resp>>16) & 0x7ff;
    //fixme: check if TLP ok!!

    if (pcie_disable_rbus_timeout)
        rtd_outl(SB2_INV_INTEN_reg, sb2_reg | SB2_INV_INTEN_write_data_mask);  // restore rbus timeout setting

    spin_unlock_irqrestore(&rtk_pcie_send_req_lock, flags);
    return 0;
}


static int send_wr_requset(struct pcie_port *pp,PCIE_REQ_CTX *req_ctx,int size)
{
    u32 slv_sb_ctrl1 = 0;
    u32 slv_sb_ctrl2 = 0;
    u32 slv_sb_ctrl3 = 0;
    u32 slv_sb_ctrl4 = 0;
    u32 xali_addr_base;
    void __iomem *xali_addr;
    u32 resp;
    unsigned long flags;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 9, 0))
    struct dw_pcie *pci = NULL;
    struct rtk_pcie_dw *pcie = NULL;

    pci = to_dw_pcie_from_pp(pp);
    pcie = to_rtk_pcie(pci);
#endif

    slv_sb_ctrl1 =
         BIT(24) |
        ((req_ctx->tlp_type     & 0x1f)<< 0) |
        ((req_ctx->np           & 0x01)<< 5) |
        ((req_ctx->a_ep         & 0x01)<< 6) |
        ((req_ctx->ns           & 0x01)<< 8) |
        ((req_ctx->ro           & 0x01)<< 9) |
        ((req_ctx->tc           & 0x01)<<10) |
        ((req_ctx->msg_code     & 0x01)<<13) |
        ((req_ctx->dbi_access   & 0x01)<<21);

    slv_sb_ctrl2 = req_ctx->hdr_3rd_dw;
    slv_sb_ctrl3 = req_ctx->hdr_4th_dw;

    slv_sb_ctrl4 =
        ((req_ctx->p_tag        & 0xff)<< 0) |
        ((req_ctx->atu_bypass   & 0x01)<< 8) |
        ((req_ctx->d_ep         & 0x01)<< 9) |
        ((req_ctx->silentDrop   & 0x01)<<10);

    spin_lock_irqsave(&rtk_pcie_send_req_lock, flags);

    rtd_outl(PCIE_WRAP_PCIE_AXI_SLAVE_SB_CTRL1_reg, slv_sb_ctrl1);
    rtd_outl(PCIE_WRAP_PCIE_AXI_SLAVE_SB_CTRL2_reg, slv_sb_ctrl2);
    rtd_outl(PCIE_WRAP_PCIE_AXI_SLAVE_SB_CTRL3_reg, slv_sb_ctrl3);
    rtd_outl(PCIE_WRAP_PCIE_AXI_SLAVE_SB_CTRL4_reg, slv_sb_ctrl4);

    //set xali address
    xali_addr_base = (req_ctx->addr & 0xfffff000);

    //initialte write request
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
    xali_addr = pp->va_cfg0_base + (req_ctx->addr & 0xfff);
#else
    xali_addr = pcie->xali_base + (req_ctx->addr & 0xfff);
#endif
    rtd_outl(PCIE_WRAP_PCIE_HOST_SYS_BUS_CTRL5_reg,xali_addr_base);  //0xb807E040

    udelay(pcie_rw_req_dealy);

    if (size == 4){
        writel(req_ctx->data, xali_addr);
    }else if (size == 2){
        writew(req_ctx->data, xali_addr);
    }else if (size == 1){
        writeb(req_ctx->data, xali_addr);
    }else{
        BUG_ON(1);
    }

    resp= rtd_inl(PCIE_WRAP_PCIE_AXI_SLAVE_SB_STATUS1_reg);
    req_ctx->resp = resp & 0x7ff;

    //fixme: check if TLP ok!!
    spin_unlock_irqrestore(&rtk_pcie_send_req_lock, flags);

    return 0;
}

static int rtk_pcie_send_cfg_rd(struct pcie_port *pp, struct pci_bus *bus,
    unsigned int devfn, int addr, int size, u32 *data)
{
    int ret;
    u32 type;
    PCIE_REQ_CTX ctx = {0};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
    if (pci_is_root_bus(bus->parent))
#else
    if (bus->parent->number == pp->root_bus_nr)
#endif
    {
        type = PCIE_ATU_TYPE_CFG0;
    }else{
        type = PCIE_ATU_TYPE_CFG1;
    }

    ctx.addr = ((bus->number << 24) |
                (PCI_SLOT(devfn)<<19) |
                (PCI_FUNC(devfn)<<16) |
                addr);
    ctx.is_read = 1;
    ctx.tlp_type = type;
    ret = send_rd_requset(pp, &ctx,size);

    *data = ctx.data;

    return ret;
}

static int rtk_pcie_send_cfg_wr(struct pcie_port *pp, struct pci_bus *bus,
    unsigned int devfn, int addr, int size, u32 data)
{
    int ret;
    u32 type;
    u32 data_new = 0;
    u32 addr_new = 0;
    struct dw_pcie *pci = NULL;
    struct rtk_pcie_dw *pcie = NULL;
    __maybe_unused u32 reginfo = 0;
    PCIE_REQ_CTX ctx = {0};

    pci = to_dw_pcie_from_pp(pp);
    pcie = to_rtk_pcie(pci);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
    if (pci_is_root_bus(bus->parent))
        type = PCIE_ATU_TYPE_CFG0;
    else
        type = PCIE_ATU_TYPE_CFG1;
#else
    if (bus->parent->number == pp->root_bus_nr) {
        type = PCIE_ATU_TYPE_CFG0;
    } else {
        type = PCIE_ATU_TYPE_CFG1;
    }
#endif

    addr_new = addr;
    data_new = data;

    ctx.data = data_new;
    ctx.is_read = 0;
    ctx.tlp_type = type;
    ctx.addr = ((bus->number << 24) |
                (PCI_SLOT(devfn)<<19) |
                (PCI_FUNC(devfn)<<16) |
                addr_new);
    ret = send_wr_requset(pp,&ctx,size);

    return ret;
}

int rtk_pcie_send_mem_rd(u32 addr, int size, u32 *data)
{
    WARN(1,"no more support\n");
    return 0;
}
EXPORT_SYMBOL(rtk_pcie_send_mem_rd);

int rtk_pcie_send_mem_wr(u32 addr, int size, u32 data)
{
    WARN(1,"no more support\n");
    return 0;
}
EXPORT_SYMBOL(rtk_pcie_send_mem_wr);

void rtk_pcie_rc_write(u32 reg,u32 val)
{
    rtd_outl(reg, val);
    PCIE_FORCE("%s(%d)set 0x%x=0x%x\n",__func__,__LINE__,reg,val);
}
EXPORT_SYMBOL(rtk_pcie_rc_write);

u32 rtk_pcie_rc_read(u32 reg)
{
    u32 reginfo;
    reginfo = rtd_inl(reg);
    PCIE_FORCE("%s(%d)get 0x%x=0x%x\n",__func__,__LINE__,reg,reginfo);
    return reginfo;
}
EXPORT_SYMBOL(rtk_pcie_rc_read);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
static int rtk_pcie_rd_other_conf(struct pci_bus *bus,
    unsigned int devfn, int where, int size, u32 *val)
#else
static int rtk_pcie_rd_other_conf(struct pcie_port *pp, struct pci_bus *bus,
    unsigned int devfn, int where, int size, u32 *val)
#endif
{
    int ret = 0;
    ret = rtk_pcie_send_cfg_rd(bus->sysdata, bus, devfn, where, size, val);

    if(pcie_enable_reginfo){
        PCIE_ALERT("%s(%d)where=0x%08x size=0x%08x val=0x%08x\n",
        __func__,__LINE__,where,size,*val);
    }

    return ret;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
static int rtk_pcie_wr_other_conf(struct pci_bus *bus,
    unsigned int devfn, int where, int size, u32 val)
#else
static int rtk_pcie_wr_other_conf(struct pcie_port *pp, struct pci_bus *bus,
    unsigned int devfn, int where, int size, u32 val)
#endif
{
    int ret = 0;
    if(pcie_enable_reginfo){
        PCIE_ALERT("%s(%d)where=0x%08x size=0x%08x val=0x%08x\n",
        __func__,__LINE__,where,size,val);
    }
    ret = rtk_pcie_send_cfg_wr(bus->sysdata, bus, devfn, where, size, val);

    return ret;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
static int rtk_pcie_rd_own_conf(struct pci_bus *bus,
    unsigned int devfn, int where, int size, u32 *val)
#else
static int rtk_pcie_rd_own_conf(struct pcie_port *pp,
    int where, int size,u32 *val)
#endif
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
    struct dw_pcie *pci = to_dw_pcie_from_pp(bus->sysdata);

    PCIE_FORCE("%s(%d)devfn=0x%08x where=0x%08x\n",__func__,__LINE__,devfn,where);
    if (PCI_SLOT(devfn) || PCI_FUNC(devfn)){
        *val = 0xffffffff;
        return PCIBIOS_DEVICE_NOT_FOUND;
    }
#else
    struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

#endif
    *val = rtk_pcie_read_dbi(pci, pci->dbi_base, where, size);

    return PCIBIOS_SUCCESSFUL;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
static int rtk_pcie_wr_own_conf(struct pci_bus *bus,
    unsigned int devfn, int where, int size, u32 val)
#else
static int rtk_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
                   u32 val)
#endif
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
    struct dw_pcie *pci = to_dw_pcie_from_pp(bus->sysdata);

    PCIE_FORCE("%s(%d)devfn=0x%08x where=0x%08x\n",__func__,__LINE__,devfn,where);
    if (PCI_SLOT(devfn) || PCI_FUNC(devfn)){
        return PCIBIOS_DEVICE_NOT_FOUND;
    }
#else
    struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

#endif
    rtk_pcie_write_dbi(pci, pci->dbi_base, where, size, val);

    return PCIBIOS_SUCCESSFUL;
}


static int rtk_pci_set_max_payload_size(struct dw_pcie *pci){
    int ret = 0;
    u32 max_payload_size;
    u32 reg_info;

    /*
     PCIE_CX_MAX_MTU_SEL:
            0 : 128, 1: 256, 2: 512, 3:1024, 4: 2048, 5: 4096
     HW fixed at 256. So, select 1.
    */
            max_payload_size = PCIE_CX_MAX_MTU_SEL;

    reg_info  = dw_pcie_readl_dbi(pci,PCIE_DEVICE_CONTROL_DEVICE_STATUS);
    reg_info &= ~(PCIE_CAP_MAX_PAYLOAD_SIZE_CS_MASK|PCIE_CAP_MAX_READ_REQ_SIZE_MASK);
    reg_info |= (max_payload_size << PCIE_CAP_MAX_PAYLOAD_SIZE_CS_POSITION);
    reg_info |= (max_payload_size << PCIE_CAP_MAX_READ_REQ_SIZE_POSITION);
    dw_pcie_writel_dbi(pci,PCIE_DEVICE_CONTROL_DEVICE_STATUS,reg_info);

    PCIE_ALERT("%s(%d)max_payload_size = %d, (reg_info=%08lx)\n",
        __func__,__LINE__, max_payload_size, reg_info);

    return ret;

}

static int rtk_pcie_chg_ID(struct pcie_port *pp)
{
#ifdef REASSIGN_ID
    u32 val;
    struct dw_pcie *pci = NULL;

    pci = to_dw_pcie_from_pp(pp);
    /* re-assign PID/VID */
    dw_pcie_dbi_ro_wr_en(pci);
    val = dw_pcie_readl_dbi(pci, PCI_VENDOR_ID);
    val = (PCIE_RTK_PID << 16) | PCIE_RTK_VID;
    dw_pcie_writel_dbi(pci, PCI_VENDOR_ID, val);
    dw_pcie_dbi_ro_wr_dis(pci);
#endif
    return 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
static struct pci_ops rtk_pci_ops;
static struct pci_ops rtk_child_pci_ops;
#endif
static int rtk_pcie_dw_host_init(struct pcie_port *pp)
{
    struct dw_pcie *pci = NULL;
    struct rtk_pcie_dw *pcie = NULL;
    int ret = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
    pp->bridge->ops = &rtk_pci_ops;
    pp->bridge->child_ops = &rtk_child_pci_ops;
#endif
    pci = to_dw_pcie_from_pp(pp);
    pcie = to_rtk_pcie(pci);

    rtk_pcie_set_link_speed(pci, pcie_link_speed, 0);
    PCIE_ALERT("%s(%d)target link speed is Gen%d\n",__func__,__LINE__,pcie_link_speed);
    /* all setting should be ready before */
    if (dw_pcie_wait_for_link(pci)) {
        /* have anything need to do?? */
        PCIE_ALERT("%s(%d)link fail\n",__func__,__LINE__);
        ret = -1;
        goto OUT;
    }

    rtk_pci_set_max_payload_size(pci);
    dw_pcie_setup_rc(pp);
    rtk_pcie_chg_ID(pp);
    rtk_pcie_enable_interrupts(pp);

OUT:
    return ret;
}

static int rtk_pcie_set_link_speed( struct dw_pcie *pci,
        e_pciegen speed, u32 initiate_speed_change)
{
    int ret = 0;
    u32 val;

    val = dw_pcie_readl_dbi(pci, PCIE_LINK_CONTROL2_LINK_STATUS2_REG);
    val &= ~PCIE_CAP_TARGET_LINK_SPEED_MASK;
    val |= (speed << PCIE_CAP_TARGET_LINK_SPEED_POSITION);
    dw_pcie_writel_dbi(pci, PCIE_LINK_CONTROL2_LINK_STATUS2_REG, val);

#if 0
    if(initiate_speed_change) {
         rt_pci_reg_rmw(PCIE_GEN2_CTRL_OFF,
                (0 << PCIE_PL_DIRECT_SPEED_CHANGE_POSITION),
                PCIE_PL_DIRECT_SPEED_CHANGE_MASK);
         rt_pci_reg_rmw(PCIE_GEN2_CTRL_OFF,
                (1 << PCIE_PL_DIRECT_SPEED_CHANGE_POSITION),
                PCIE_PL_DIRECT_SPEED_CHANGE_MASK);
    }

#endif
    return ret;

}

static int rtk_pcie_link_up(struct dw_pcie *pci)
{
    int ret = 0;
    __maybe_unused u32 temp;
    u32 current_link_rate;
    uint32_t reg_info;

    // 1. Wait SMLH link is up
    // 2. Wait RDLH link is up
    // 3. Wait Expected Data Rate
    // 4. Wait LTSSM to L0 for 50ms

    temp = dw_pcie_readl_dbi(pci, PCIE_LINK_CONTROL_LINK_STATUS_REG);
    current_link_rate =  (temp &  PCIE_CAP_LINK_SPEED_MASK) >> PCIE_CAP_LINK_SPEED_POSITION;
    reg_info = rtd_inl(PCIE_WRAP_PCIE_SII_PM_STATUS1_reg);
    reg_info = PCIE_WRAP_PCIE_SII_PM_STATUS1_get_smlh_ltssm_state(reg_info);

    if(reg_info == S_L0){   //link success
        ret = 1;
        if( (current_link_rate == 0) || (current_link_rate > GEN5)){
            PCIE_ALERT("%s(%d)current link rate unknow(%x)\n",__func__,__LINE__,current_link_rate);
        }
        PCIE_ALERT("%s(%d)current link at Gen%x\n",__func__,__LINE__,current_link_rate);
    }

    return ret;
}


static const struct dw_pcie_ops rtk_dw_pcie_ops = {
    .link_up   = rtk_pcie_link_up,
    .read_dbi  = rtk_pcie_read_dbi,
    .write_dbi = rtk_pcie_write_dbi,
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
static struct dw_pcie_host_ops rtk_pcie_dw_host_ops = {
    .host_init      = rtk_pcie_dw_host_init,
};

static struct pci_ops rtk_pci_ops = {
    .map_bus = NULL,
    .read = rtk_pcie_rd_own_conf,
    .write = rtk_pcie_wr_own_conf,
};

static struct pci_ops rtk_child_pci_ops = {
    .map_bus = NULL,
    .read = rtk_pcie_rd_other_conf,
    .write = rtk_pcie_wr_other_conf,
};
#else
static struct dw_pcie_host_ops rtk_pcie_dw_host_ops = {
    .rd_other_conf  = rtk_pcie_rd_other_conf,
    .wr_other_conf  = rtk_pcie_wr_other_conf,
    .rd_own_conf    = rtk_pcie_rd_own_conf,
    .wr_own_conf    = rtk_pcie_wr_own_conf,
    .host_init      = rtk_pcie_dw_host_init,
};
#endif    //#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))

static int rtk_pcie_dw_parse_dt(struct rtk_pcie_dw *pcie)
{
    /* Parse keyword from dts
       linux-5.xx\arch\arm64\boot\dts\rtk_tv\rtdXXXX.dts */

    struct device_node *np = pcie->dev->of_node;
    int ret = 0;
    ret = of_property_read_u32(np, "realtek,region-start", &pcie->region_start);
    if (ret < 0) {
        PCIE_ALERT("Failed to read region-start (%d)\n", ret);
        goto ERR_OUT;
    }
    PCIE_ALERT("region-start: 0x%08x\n", pcie->region_start);

    ret = of_property_read_u32(np, "realtek,region-length", &pcie->region_length);
    if (ret < 0) {
        PCIE_ALERT("Failed to read rtk,region-length (%d)\n", ret);
        goto ERR_OUT;
    }

ERR_OUT:
    WARN((ret < 0), "NO PCIe ranges resource.\n");
    return ret;

}

static int rtk_pci_clk_sel(u32 enable)
{
    u32 reg_info;
    u32 en_val;

    // Note: 0 for PCIE enable; 1 for USB3
    en_val = (enable)? 0:1;
    PCIE_ALERT("%s(%d)PCIE %s\n",__func__,__LINE__,(en_val)?"dis-select":"selected");

    reg_info = rtd_inl(SYS_REG_SYS_CLKSEL_reg);
    reg_info &= ~SYS_REG_SYS_CLKSEL_pcie_usb3_sel_mask;
    reg_info |= SYS_REG_SYS_CLKSEL_pcie_usb3_sel(en_val);
    rtd_outl(SYS_REG_SYS_CLKSEL_reg,reg_info);
    return 0;
}

/* ==================================================================
    async-reset
    rst 0 ==> clk 1 ==> delay ==> clk 0 ==> rst 1 ==> clk 1
================================================================== */
static int rtk_pci_crt_enable(u32 enable)
{
    u32 en_val;

    en_val = (enable)? 1:0;

    ///////////PCIe CRT /////
    if(en_val){
        rtd_outl( SYS_REG_SYS_SRST3_reg,
            SYS_REG_SYS_SRST3_rstn_pcie(1) | SYS_REG_SYS_SRST3_write_data(0));
        rtd_outl( SYS_REG_SYS_CLKEN3_reg,
            SYS_REG_SYS_CLKEN3_clken_pcie(1) | SYS_REG_SYS_CLKEN3_write_data(1));
        udelay(50);
        rtd_outl( SYS_REG_SYS_CLKEN3_reg,
            SYS_REG_SYS_CLKEN3_clken_pcie(1) | SYS_REG_SYS_CLKEN3_write_data(0));
        rtd_outl( SYS_REG_SYS_SRST3_reg,
            SYS_REG_SYS_SRST3_rstn_pcie(1) | SYS_REG_SYS_SRST3_write_data(1));
        rtd_outl( SYS_REG_SYS_CLKEN3_reg,
            SYS_REG_SYS_CLKEN3_clken_pcie(1) | SYS_REG_SYS_CLKEN3_write_data(1));
    }else{
        rtd_outl( SYS_REG_SYS_SRST3_reg,
            SYS_REG_SYS_SRST3_rstn_pcie(1) | SYS_REG_SYS_SRST3_write_data(0));
        rtd_outl( SYS_REG_SYS_CLKEN3_reg,
            SYS_REG_SYS_CLKEN3_clken_pcie(1) | SYS_REG_SYS_CLKEN3_write_data(0));
    }

    return 0;
}

static int rtk_pci_set_app_ltssm_enable(u32 enable)
{
    u32 reg_info;
    u32 en_val;

    en_val = (enable)? 1:0;

    reg_info = rtd_inl(PCIE_WRAP_PCIE_SII_GEN_CORE_CTRL1_reg);
    reg_info &= ~PCIE_WRAP_PCIE_SII_GEN_CORE_CTRL1_app_ltssm_enable_mask;
    reg_info |= PCIE_WRAP_PCIE_SII_GEN_CORE_CTRL1_app_ltssm_enable(en_val);
    rtd_outl(PCIE_WRAP_PCIE_SII_GEN_CORE_CTRL1_reg,reg_info);

    return 0;
}

static int rtk_pci_mdio_enable(u32 enable)
{
    u32 en_val;

    en_val = (enable)? 1:0;

    //pcie_mdrst_n bit1=1
    rtd_outl( PCIE_WRAP_PCIE_CLKRST_CTRL1_reg,
            PCIE_WRAP_PCIE_CLKRST_CTRL1_pcie_mdrst_n(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_write_data(en_val));

    return 0;
}

static void rtk_pci_set_bus_endian(e_bus_endian endian)
{
    u32 reg_info;

    reg_info = rtd_inl( PCIE_WRAP_PCIE_MEM_BUS_CTRL1_reg);
    reg_info &= ~PCIE_WRAP_PCIE_MEM_BUS_CTRL1_dw_endian_swap_mask;
    reg_info |= PCIE_WRAP_PCIE_MEM_BUS_CTRL1_dw_endian_swap(endian);
    rtd_outl( PCIE_WRAP_PCIE_MEM_BUS_CTRL1_reg,reg_info);
}

#define PCIE_WRAP_PCIE_SCPU_RBUS_TIMEOUT_CTRL   (0xB807E310)
static int rtk_pci_wrap_enable(u32 enable)
{
    int ret = 0;
    u32 reg_info;
    u32 en_val;
    u32 wloop;

    en_val = (enable)? 1:0;

    ///////////PCIe WRAP /////
    // pow_pcie bit0=1
    reg_info = rtd_inl( PCIE_WRAP_PCIE_PIPE_CTRL1_reg);
    reg_info &= ~PCIE_WRAP_PCIE_PIPE_CTRL1_pow_pcie_mask;
    reg_info |= PCIE_WRAP_PCIE_PIPE_CTRL1_pow_pcie(en_val);
    rtd_outl( PCIE_WRAP_PCIE_PIPE_CTRL1_reg,reg_info);

    // sys_aux_pwr_det bit0=1
    reg_info = rtd_inl( PCIE_WRAP_PCIE_SII_PM_CTRL1_reg);
    reg_info &= ~PCIE_WRAP_PCIE_SII_PM_CTRL1_sys_aux_pwr_det_mask;
    reg_info |= PCIE_WRAP_PCIE_SII_PM_CTRL1_sys_aux_pwr_det(en_val);
    rtd_outl( PCIE_WRAP_PCIE_SII_PM_CTRL1_reg,reg_info);

    // aux bit8=1,mstr bit11=1,slv bit12=1,dbi bit13=1
    rtd_outl(PCIE_WRAP_PCIE_CLKRST_CTRL1_reg,
            PCIE_WRAP_PCIE_CLKRST_CTRL1_dbi_aclken(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_slv_aclken(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_mstr_aclken(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_aux_clken(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_write_data(en_val));
    udelay(50);

    // perst_n bit2=1
    rtd_outl(PCIE_WRAP_PCIE_CLKRST_CTRL1_reg,
            PCIE_WRAP_PCIE_CLKRST_CTRL1_perst_n(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_write_data(en_val));
    udelay(50);

    // power_up_rst_n bit3=1,button_rst_n bit4=1
    rtd_outl(PCIE_WRAP_PCIE_CLKRST_CTRL1_reg,
            PCIE_WRAP_PCIE_CLKRST_CTRL1_power_up_rst_n(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_button_rst_n(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_write_data(en_val));
    udelay(50);

    // core_clken bit9=1
    rtd_outl(PCIE_WRAP_PCIE_CLKRST_CTRL1_reg,
            PCIE_WRAP_PCIE_CLKRST_CTRL1_core_clken(1) |
            PCIE_WRAP_PCIE_CLKRST_CTRL1_write_data(en_val));
    udelay(50);

    // set avoid rbus timeout
    if (get_mach_type() == MACH_ARCH_RTK2885P2) {
        PCIE_ALERT("PCIe rbus timeout enable\n");
        rtd_outl(PCIE_WRAP_PCIE_SCPU_RBUS_TIMEOUT_CTRL, 0x07);
    }else{
        PCIE_ALERT("no PCIe rbus timeout\n");
    }

    //change DDR bus little endian
    rtk_pci_set_bus_endian(BUS_LITTLE_EDN);

    if(en_val){
        u32 rdy_info;

        wloop = 0;    //wait 5ms for clk ready
        do {
            rdy_info = rtd_inl(PCIE_WRAP_PCIE_PIPE_STATUS1_reg) & 0x01;
            if(wloop > 100){
                PCIE_ALERT("Pcie clock can't ready\n");
                ret = -1;
                goto OUT;
            }
            wloop ++;
            udelay(50);

        }while(!rdy_info);
    }

OUT:
    return ret;
}


static unsigned char g_pice_reset_pin_present = 0;
/* reset hold is level low */
static void rtk_pcie_perst_hold(void)
{
#ifndef  MARK_FOR_FPGA
    if (g_pice_reset_pin_present)
    {
        PCIE_ALERT("hold PCIe reset pin!!!!\n");
		IO_Direct_Set(PCIE_PIN_RESET_PCB_NAME, 0);
    }
    else
    {
        PCIE_ALERT("hold PCIe reset failed, pcie reset gpio absent !!!!!\n");
    }
#endif
}

/* reset release is level high */
static void rtk_pcie_perst_release(void)
{
#ifndef  MARK_FOR_FPGA
    if (g_pice_reset_pin_present)
    {
        PCIE_ALERT("release PCIe reset pin!!!!\n");
		IO_Direct_Set(PCIE_PIN_RESET_PCB_NAME, 1);
    }
    else
    {
        PCIE_ALERT("release PCIe reset failed, pcie reset gpio absent !!!!!\n");
    }
#endif
}


static int rtk_pcie_load_pcb_cfg(void)
{
#ifndef  MARK_FOR_FPGA
    unsigned long long param = 0;

    if (pcb_mgr_get_enum_info_byname(PCIE_PIN_RESET_PCB_NAME, &param) == 0)
    {
        g_pice_reset_pin_present = 1;
		PCIE_ALERT("PIN_PCIE_RST == %llx\n", param);
    }
    else
#endif
    {
        PCIE_ALERT("Can't find PIN_PCIE_RST, use default setting\n");
        g_pice_reset_pin_present = 0;
    }
    return 1;
}

/*
 * Set PCIe MMIO range
 *
 * Call register usage:
 * a0   Function ID
 * a1   start address
 * a2   end address
 * a3-7 Preserved
 *
 * Normal return register usage:
 * a0   OPTEE_SMC_RETURN_OK
 * a1-7 Preserved
 *
 * Not idle return register usage:
 * a0   OPTEE_SMC_RETURN_EBADCMD
 * a1-7 Preserved
*/
#define OPTEE_SMC_FUNCID_PCIE_MMIO  29
#define OPTEE_SMC_PCIE_MMIO \
    OPTEE_SMC_FAST_CALL_VAL(OPTEE_SMC_FUNCID_PCIE_MMIO)

#define ACT0_CLR_MMIO_REG   (0)
#define ACT1_SET_MMIO_REG   (1)
#define ACT2_GET_MMIO_REG   (2)

static int rtk_pcie_set_mem_region(struct pcie_port *pp,int enable)
{
    struct dw_pcie *pci = NULL;
    struct rtk_pcie_dw *pcie = NULL;
    struct arm_smccc_res res;

    pci = to_dw_pcie_from_pp(pp);
    pcie = to_rtk_pcie(pci);

    PCIE_FORCE("RTK_PCI_REGION_START=0x%08x RTK_PCI_REGION_END=0x%08x\n",
        RTK_PCI_REGION_START,RTK_PCI_REGION_END);
    PCIE_FORCE("dbi_res start=0x%08x\n",pcie->dbi_res->start);

    arm_smccc_smc(OPTEE_SMC_PCIE_MMIO,ACT1_SET_MMIO_REG,
            RTK_PCI_REGION_START,RTK_PCI_REGION_END,0,0,0,0,&res);
    PCIE_FORCE("OPTEE result: a1=0x%08x a2=0x%08x a3=0x%08x\n",res.a1,res.a2,res.a3);

    return 0;
}


static int rtk_pcie_config_controller(struct rtk_pcie_dw *pcie)
{
    struct dw_pcie *pci = &pcie->pci;
    struct pcie_port *pp = &pci->pp;
    int ret = 0;

    /* Set PHY MUX for PCIE */
    rtk_pci_clk_sel(1);

    /* PCIe CRT */
    rtk_pci_crt_enable(1);

    /* enable pcie_mdrst_n */
    rtk_pci_mdio_enable(1);

    /* set PCIE PHY */
    rtk_pcie_phy_setting(pp);

    /* PCIe WRAP */
    ret = rtk_pci_wrap_enable(1);
    if(ret){
        goto fail_wrap;
    }

    /* calibrate PCIE PHY */
    rtk_pcie_phy_calibration(pp,GEN1);
    rtk_pcie_phy_calibration(pp,GEN2);

    // release perst#
    //msleep(50);
    rtk_pcie_perst_release();

    // app_ltssm_enable bit0=1
    rtk_pci_set_app_ltssm_enable(1);

    //To setting mem mapping base range, should git this from dtb information later.
    /* set mem mapping base  */
    rtk_pcie_set_mem_region(pp,1);

    return ret;

fail_wrap:

    return ret;
}

static int __deinit_controller(struct rtk_pcie_dw *pcie)
{
    /* PCIe WRAP */
    rtk_pci_wrap_enable(0);
    //disable mdio
    rtk_pci_mdio_enable(0);
    /* PCIe CRT */
    rtk_pci_crt_enable(0);

    return 0;
}


static int rtk_pcie_init_controller(struct rtk_pcie_dw *pcie)
{
    struct dw_pcie *pci = &pcie->pci;
    struct pcie_port *pp = &pci->pp;
    int ret;
    pp->ops = &rtk_pcie_dw_host_ops;

    ret = dw_pcie_host_init(pp);
    if (ret < 0) {
        dev_err(pcie->dev, "Failed to add PCIe port: %d\n", ret);
        goto fail_host_init;
    }

    return 0;

fail_host_init:
    return ret;

}


static int rtk_pcie_deinit_controller(struct rtk_pcie_dw *pcie)
{
    dw_pcie_host_deinit(&pcie->pci.pp);

    return 0;

}


static int rtk_pcie_config_rp(struct rtk_pcie_dw *pcie)
{
    struct pcie_port *pp = &pcie->pci.pp;
    struct device *dev = pcie->dev;
    char *name;
    int ret = 0;

    if (IS_ENABLED(CONFIG_PCI_MSI)) {
            pp->msi_irq = of_irq_get_byname(dev->of_node, "msi");
            if (!pp->msi_irq) {
                    dev_err(dev, "Failed to get MSI interrupt\n");
                    return -ENODEV;
            }
    }

    ret = rtk_pcie_config_controller(pcie);
    if (ret != 0){
        return ret;
    }
	
    ret = rtk_pcie_init_controller(pcie);
    if(ret != 0) {
           goto fail_host_init;
    }
    name = devm_kasprintf(dev, GFP_KERNEL, "%pOFP", dev->of_node);
    if (!name) {
            ret = -ENOMEM;
            goto fail_kasprintf;
    }
    return ret;

fail_kasprintf:
    rtk_pcie_deinit_controller(pcie);
fail_host_init:
    return ret;

}


//===========================================================================
// Func : pcie_atr_show_debug_ctrl
//===========================================================================

static ssize_t pcie_atr_show_debug_ctrl(
    struct device*              dev,
    struct device_attribute*    attr,
    char*                       buf
    )
{
    int  n;
    int  count = PAGE_SIZE;
    char *ptr = buf;

    if (strcmp(attr->attr.name, "rw_req_delay")==0)
    {
        n = scnprintf(ptr, count, "%d\n", pcie_rw_req_dealy);
        ptr+=n; count-=n;
    }
    else if (strcmp(attr->attr.name, "disable_rbus_timeout")==0)
    {
        n = scnprintf(ptr, count, "%d\n", pcie_disable_rbus_timeout);
        ptr+=n; count-=n;
    }
#ifndef  MARK_FOR_FPGA
    else if (strcmp(attr->attr.name, "pcie_reset_gpio")==0)
    {
        if (!g_pice_reset_pin_present)
        {
            n = scnprintf(ptr, count, "no pcie_reset_gpio exist\n");
            ptr+=n; count-=n;
        }
        else
        {
            n = scnprintf(ptr, count, "pcie_reset_gpio exist\n");
            ptr+=n; count-=n;
        }
    }
#endif
    else if (strcmp(attr->attr.name, "en_reg_info")==0)
    {
        n = scnprintf(ptr, count, "pcie_enable_reginfo=%d\n",pcie_enable_reginfo);
        ptr+=n; count-=n;
    }
    else if (strcmp(attr->attr.name, "msi_status")==0)
    {
        struct rtk_pcie_dw *pcie = dev_get_drvdata(dev);
        struct dw_pcie *pci = &pcie->pci;
        struct pcie_port *pp = &pci->pp;
        u32 msi_mask = pp->irq_mask[0];
        u64 msi_target = (u64)pp->msi_data;

        n = scnprintf(ptr, count,
                "MSI_MASK=0x%08x MSI_ENABLE=0x%08x MSI_STATUS=0x%08x\n"
                "MSI_TARGET=0x%08x%08x \n",
                dw_pcie_readl_dbi(pci, PCIE_MSI_INTR0_MASK),    //0x82C
                dw_pcie_readl_dbi(pci, PCIE_MSI_INTR0_ENABLE),  //0x828
                dw_pcie_readl_dbi(pci, PCIE_MSI_INTR0_STATUS),  //0x830
                dw_pcie_readl_dbi(pci, PCIE_MSI_ADDR_HI),       //0x824
                dw_pcie_readl_dbi(pci, PCIE_MSI_ADDR_LO));      //0x820
        ptr+=n; count-=n;
        n = scnprintf(ptr, count,
                "msi_mask=0x%08x msi_target=0x%08x%08x\n",
                msi_mask,
                upper_32_bits(msi_target),lower_32_bits(msi_target));
        ptr+=n; count-=n;
    }

    return ptr - buf;
}

static void rtk_pcie_show_perst(void)
{
    u32 reg;

    //set mis GPIO2 low
    reg = 0xb801BD18;
    PCIE_ALERT("%s(%d)GPIO2 level:0x%08x=0x%08x\n",__func__,__LINE__,reg,rtd_inl(reg));

    //set mis GPIO2 output
    reg = 0xb801BD00;
    PCIE_ALERT("%s(%d)GPIO2 dir: 0x%08x=0x%08x\n",__func__,__LINE__,reg,rtd_inl(reg));
    PCIE_ALERT("%s(%d)0x%08x=0x%08x\n",__func__,__LINE__,reg,rtd_inl(reg));

    //set mis GPIO2 pinmux
    reg = 0xb8000800;
    PCIE_ALERT("%s(%d)GPIO2 pinmux: 0x%08x=0x%08x\n",__func__,__LINE__,reg,rtd_inl(reg));
}

static ssize_t pcie_attr_store_debug_ctrl(struct device *dev,
                   struct device_attribute *attr,
                   const char *buf, size_t count)
{
    struct rtk_pcie_dw *pcie = dev_get_drvdata(dev);
    struct dw_pcie *pci = &pcie->pci;
    //struct pcie_port *pp = &pci->pp;
    unsigned int val = 0;
    u32 dbi_addr, dbi_val;

    if (strcmp(attr->attr.name, "rw_req_delay")==0)
    {
        if (sscanf(buf, "%5d", &val)==1)
            pcie_rw_req_dealy = (val > 100) ? 100 : val;
    }
    else if (strcmp(attr->attr.name, "disable_rbus_timeout")==0)
    {
        if (sscanf(buf, "%d", &val)==1)
            pcie_disable_rbus_timeout = (val) ? 1 : 0;
    }
    else if (strcmp(attr->attr.name, "en_reg_info")==0)
    {
        if (sscanf(buf, "%x", &val)==1){
            PCIE_ALERT("val=0x%08x\n",val);
            pcie_enable_reginfo = (val) ? 1 : 0;
            PCIE_ALERT("pcie_enable_reginfo=%d\n",pcie_enable_reginfo);
        }else{
            PCIE_ALERT("pcie_enable_reginfo no input\n");
        }

    }
    else if (strcmp(attr->attr.name, "pcie_reset_gpio")==0)
    {
        rtk_pcie_show_perst();
        if (sscanf(buf, "%x", &val)==1){
            PCIE_ALERT("val=0x%08x\n",val);
            if(((val) ? 1 : 0) == 1){
                PCIE_ALERT("pull perst high\n");
                rtk_pcie_perst_release();
            }else{
                PCIE_ALERT("pull perst low\n");
                rtk_pcie_perst_hold();
            }
        }else{
            PCIE_ALERT("no val input\n");
        }
        rtk_pcie_show_perst();
    }
    else if (strcmp(attr->attr.name, "dbi_set")==0)
    {
        if (sscanf(buf, "%x %[^=]=%x", &dbi_addr, &dbi_val)==1){
            dw_pcie_writel_dbi(pci, dbi_addr, dbi_val);
            PCIE_ALERT("DBI 0x%x=0x%x\n",dbi_addr,dw_pcie_readl_dbi(pci, dbi_addr));
        }else{
            PCIE_ALERT("buf=%s\n",buf);
        }
    }
    else if (strcmp(attr->attr.name, "dbi_get")==0)
    {
        if (sscanf(buf, "%x", &dbi_addr)==1){
            PCIE_ALERT("DBI 0x%x=0x%x\n",dbi_addr,dw_pcie_readl_dbi(pci, dbi_addr));
        }else{
            PCIE_ALERT("buf=%s\n",buf);
        }
    }
    else
    {
        return -1;
    }

    return count;
}

static DEVICE_ATTR(rw_req_delay, 0644, pcie_atr_show_debug_ctrl, pcie_attr_store_debug_ctrl);
static DEVICE_ATTR(pcie_reset_gpio, 0644, pcie_atr_show_debug_ctrl, pcie_attr_store_debug_ctrl);
static DEVICE_ATTR(disable_rbus_timeout, 0444, pcie_atr_show_debug_ctrl, NULL);
static DEVICE_ATTR(en_reg_info, 0644, pcie_atr_show_debug_ctrl, pcie_attr_store_debug_ctrl);
static DEVICE_ATTR(msi_status, 0444, pcie_atr_show_debug_ctrl, NULL);
static DEVICE_ATTR(dbi_get, 0644, pcie_atr_show_debug_ctrl, pcie_attr_store_debug_ctrl);
static DEVICE_ATTR(dbi_set, 0644, pcie_atr_show_debug_ctrl, pcie_attr_store_debug_ctrl);


static int rtk_pcie_dw_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct rtk_pcie_dw *pcie;
    struct resource *dbi_res;
    struct pcie_port *pp;
    struct dw_pcie *pci;
    u32 ic_ver;
    int ret;

#ifdef MARK_FOR_FPGA
    pcie_enable = 1;
    ic_ver = VERSION_A; //get_ic_version();
#else
    //pcie_enable = 1;
    ic_ver = get_ic_version();
#endif
    pcie_enable_reginfo = 0;

    PCIE_ALERT("RTK TV PCIE (Ver. %d) 2023/03/22 18:50\n",ic_ver);
    if(!pcie_enable){
        PCIE_ALERT("need bootcode paramter \"pcie_enable\" to enable PCIE\n");
        return -ENODEV;
    }

    rtk_pcie_load_pcb_cfg();
    rtk_pcie_perst_hold();

    pcie = devm_kzalloc(dev, sizeof(*pcie), GFP_KERNEL);
    if (!pcie){
        return -ENOMEM;
    }

    pcie->ic_ver = ic_ver;
    pci = &pcie->pci;

    pci->dev = &pdev->dev;
    pci->ops = &rtk_dw_pcie_ops;
    pp = &pci->pp;
    pcie->dev = &pdev->dev;

    dev_local = dev;
    pci_local = pci;
    pp_local = pp;

    ret = rtk_pcie_dw_parse_dt(pcie);
    if (ret < 0) {
        dev_err(dev, "Failed to parse device tree: %d\n", ret);
        return ret;
    }

    dbi_res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
    if (!dbi_res) {
        dev_err(dev, "Failed to find \"dbi\" region\n");
        return -ENODEV;
    }
    pcie->dbi_res = dbi_res;
    PCIE_ALERT("dbi_res=0x%08x\n",pcie->dbi_res->start);

    pci->dbi_base = devm_ioremap_resource(dev, dbi_res);
    if (IS_ERR(pci->dbi_base)){
        return PTR_ERR(pci->dbi_base);
    }
    PCIE_ALERT("dbi_base=0x%llx\n",(u64)pci->dbi_base);
    pcie->xali_res = platform_get_resource_byname(pdev, IORESOURCE_MEM,"config");
    if (!pcie->xali_res) {
        dev_err(dev, "Failed to find \"xali\" region\n");
        return -ENODEV;
    }
    PCIE_ALERT("xali_res=0x%08x\n",pcie->xali_res->start);
    /* The ioremap move to DWC */

    pcie->wrap_res = platform_get_resource_byname(pdev, IORESOURCE_MEM,"wrap");
    if (!pcie->wrap_res) {
        dev_err(dev, "Failed to find \"wrap\" region\n");
        return -ENODEV;
    }
    pcie->wrap_base = devm_ioremap_resource(dev, pcie->wrap_res);

    if (IS_ERR(pcie->wrap_base)){
        return PTR_ERR(pcie->wrap_base);
    }

    pp->irq = platform_get_irq_byname(pdev, "intr");
    if (pp->irq < 0) {
        dev_err(dev, "failed to get irq\n");
        return pp->irq;
    }
    ret = devm_request_irq(dev, pp->irq, rtk_pcie_irq_handler,
                IRQF_SHARED, "rtk-pcie-intr", pcie);
    if (ret) {
        dev_err(dev, "failed to request irq\n");
        return ret;
    }

    if (IS_ENABLED(CONFIG_PCI_MSI)) {
        pp->msi_irq = platform_get_irq_byname(pdev, "msi");
        if (pp->msi_irq < 0) {
            dev_err(dev, "failed to get msi irq\n");
            return pp->msi_irq;
        }
    }

    platform_set_drvdata(pdev, pcie);

    ret = rtk_pcie_config_rp(pcie);
    if (ret && ret != -ENOMEDIUM){
        rtk_pci_clk_sel(0);
        __deinit_controller(pcie);
        goto fail;
    }

    device_create_file(&pdev->dev, &dev_attr_rw_req_delay);
    device_create_file(&pdev->dev, &dev_attr_pcie_reset_gpio);
    device_create_file(&pdev->dev, &dev_attr_disable_rbus_timeout);
    device_create_file(&pdev->dev, &dev_attr_en_reg_info);
    device_create_file(&pdev->dev, &dev_attr_msi_status);
    device_create_file(&pdev->dev, &dev_attr_dbi_get);
    device_create_file(&pdev->dev, &dev_attr_dbi_set);

    return 0;

fail:
    /* anything need to disable ?? */
    return ret;
}

static int __exit rtk_pcie_dw_remove(struct platform_device *pdev)
{
    struct rtk_pcie_dw *pcie = platform_get_drvdata(pdev);

    device_remove_file(&pdev->dev, &dev_attr_rw_req_delay);
    device_remove_file(&pdev->dev, &dev_attr_pcie_reset_gpio);
    device_remove_file(&pdev->dev, &dev_attr_disable_rbus_timeout);
    device_remove_file(&pdev->dev, &dev_attr_en_reg_info);
    device_remove_file(&pdev->dev, &dev_attr_msi_status);
    device_remove_file(&pdev->dev, &dev_attr_dbi_get);
    device_remove_file(&pdev->dev, &dev_attr_dbi_set);

    rtk_pcie_deinit_controller(pcie);
    __deinit_controller(pcie);
    pm_runtime_put_sync(pcie->dev);
    pm_runtime_disable(pcie->dev);

    return 0;
}

static int rtk_pcie_dw_save_msi(struct device *dev)
{
    struct rtk_pcie_dw *pcie = dev_get_drvdata(dev);
    struct dw_pcie *pci = &pcie->pci;
    struct pcie_port *pp = &pci->pp;

    if (pp->has_msi_ctrl) {
        pcie->msi_mask = pp->irq_mask[0];
        PCIE_ALERT("msi_mask=0x%08x\n",pcie->msi_mask);
    }else{
        PCIE_ALERT("no MSI enable\n");
    }

    return 0;
}

static int rtk_pcie_dw_restore_msi(struct device *dev)
{
    struct rtk_pcie_dw *pcie = dev_get_drvdata(dev);
    struct dw_pcie *pci = &pcie->pci;
    struct pcie_port *pp = &pci->pp;

    if (pp->has_msi_ctrl) {
        PCIE_ALERT("msi_mask=0x%08x\n",pcie->msi_mask);
        pp->irq_mask[0] = pcie->msi_mask;
        dw_pcie_writel_dbi(pci, PCIE_MSI_INTR0_MASK,pcie->msi_mask);
        /* The PCIE_MSI_INTR0_ENABLE was enabled at dw_pcie_setup_rc. */
        dw_pcie_writel_dbi(pci, PCIE_MSI_INTR0_ENABLE,~0);
    }else{
        PCIE_ALERT("no MSI enable\n");
    }

    return 0;
}

static int rtk_pcie_dw_suspend_late(struct device *dev)
{
    /* reset PCIE CRT and hold clock?? */
    rtk_pcie_dw_save_msi(dev);

    return 0;

}

static int rtk_pcie_dw_suspend_noirq(struct device *dev)
{
    struct rtk_pcie_dw *pcie = dev_get_drvdata(dev);
    struct dw_pcie *pci = &pcie->pci;
    struct pcie_port *pp = &pci->pp;

    /* save any information that needed */
    rtk_pcie_perst_hold();

    rtk_pcie_int_INTx_misc_enable(0);
    rtk_pcie_int_msi_enable(pp,0);
    rtk_pcie_int_crt_enable(0);

    dw_pcie_writel_dbi(pci, PCIE_MSI_INTR0_ENABLE,0);

    return 0;

}


static int rtk_pcie_dw_resume_noirq(struct device *dev)
{
    struct rtk_pcie_dw *pcie = dev_get_drvdata(dev);
    int ret;

    ret = rtk_pcie_config_controller(pcie);
    if (ret < 0)
            return ret;

    ret = rtk_pcie_dw_host_init(&pcie->pci.pp);
    if (ret < 0) {
            dev_err(dev, "Failed to init host: %d\n", ret);
            goto fail_host_init;
    }

    /* Restore MSI mask after host init */
    rtk_pcie_dw_restore_msi(dev);

    return 0;

fail_host_init:
    return ret;

}

static int rtk_pcie_dw_resume_early(struct device *dev)
{
    /* Disable HW_HOT_RST mode */
    return 0;
}

static void rtk_pcie_dw_shutdown(struct platform_device *pdev)
{
    struct rtk_pcie_dw *pcie = platform_get_drvdata(pdev);

	rtk_pcie_perst_hold();

    disable_irq(pcie->pci.pp.irq);
    if (IS_ENABLED(CONFIG_PCI_MSI))
            disable_irq(pcie->pci.pp.msi_irq);

    __deinit_controller(pcie);

}

static const struct of_device_id rtk_pcie_dw_of_match[] = {
    { .compatible = "realtek,rtk-pcie" },
    {},
};

static const struct dev_pm_ops rtk_pcie_dw_pm_ops = {
    .suspend_late   = rtk_pcie_dw_suspend_late,
    .suspend_noirq  = rtk_pcie_dw_suspend_noirq,
    .resume_noirq   = rtk_pcie_dw_resume_noirq,
    .resume_early   = rtk_pcie_dw_resume_early,
};

static struct platform_driver rtk_pcie_dw_driver = {
    .probe = rtk_pcie_dw_probe,
    .remove = __exit_p(rtk_pcie_dw_remove),
    .shutdown = rtk_pcie_dw_shutdown,
    .driver = {
        .name   = "rtk-pcie",
        .pm = &rtk_pcie_dw_pm_ops,
        .of_match_table = rtk_pcie_dw_of_match,
    },
};
static int __init rtk_pcie_plat_init(void)
{

		
#ifdef CONFIG_RTK_KDRV_PCIE_MODULE
        parse_pcie_enable_handler();
#endif
        return platform_driver_register(&rtk_pcie_dw_driver);
}

module_init(rtk_pcie_plat_init);


static void __exit rtk_pcie_plat_exit(void)
{
	platform_driver_unregister(&rtk_pcie_dw_driver);
}
module_exit(rtk_pcie_plat_exit);

MODULE_DEVICE_TABLE(of, rtk_pcie_dw_of_match);

MODULE_AUTHOR("Alexkh <alexkh@realtek.com>");
MODULE_DESCRIPTION("Realtek PCIe host controller driver");
MODULE_LICENSE("GPL v2");

#ifndef CONFIG_RTK_KDRV_PCIE_MODULE
u32 pcie_enable = 1;
static int rtk_pcie_enable_chk(char *buf)
{
    PCIE_ALERT("\n=== bootcode enable PCIE ===\n");
    pcie_enable  = 1;

    return 0;
}

early_param("pcie_enable",rtk_pcie_enable_chk);

static int rtk_pcie_show_dbi(char *buf)
{
    PCIE_ALERT("\n=== bootcode enable show dbi ===\n");
    //pcie_show_dbi = 1;
    pcie_enable_reginfo = 1;

    return 0;
}

early_param("pcie_show_dbi",rtk_pcie_show_dbi);

static int rtk_pcie_link_speed_chk(char *buf)
{
    int val;

    PCIE_ALERT("\n=== bootcode force PCIE link speed (%s)===\n", buf);

    if (sscanf(buf, "%d", &val)==1)
    {
        switch (val)
        {
        case GEN1:
        case GEN2:
            pcie_link_speed  = val;
            break;
        default:
            PCIE_ALERT("%s(%d) invalid link speed - %d, use default %d\n",__func__,__LINE__, val, pcie_link_speed);
        }
    }

    return 0;
}

early_param("pcie_link_speed",rtk_pcie_link_speed_chk);

#else
u32 pcie_enable = 1;
static int parse_pcie_enable_handler(void)
{
    if(rtk_parse_commandline("pcie_enable") == false)
    {
        PCIE_ERR("Error : can't get pcie_enable from bootargs\n");
        return -1;
    }

    pcie_enable = 1;
    PCIE_INFO("pcie_enable:%u\n", pcie_enable);
    return 0;
}

module_param_named(pcie_link_speed, pcie_link_speed, uint, S_IRUGO | S_IWUSR);
#endif