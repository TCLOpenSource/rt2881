#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/usb/phy.h>
#include <linux/slab.h>
#include <linux/acpi.h>
#include <linux/version.h>
#include <rtk_kdriver/rtk_otp_region_api.h>
#include <rtk_kdriver/io.h>
#include <rbus/usb3_top_reg.h>
#include <rtk_kdriver/rtk_crt.h>
#include <rbus/efuse_reg.h>
#include <mach/rtk_platform.h>
#include "xhci.h"
#include "xhci-plat.h"
#include "rtk_usb_wifi.h"
#include "usb-platform-def.h"

extern int is_OLED_PANEL(void);

struct XHCI_PORT_Z0_TUNING_INFO 
{
	struct usb_hcd *hcd;
	unsigned int port;
	unsigned int z0_value;
	
} xhci_port_z0_tuning_info = {
	NULL,
	0,
	0xffffffff
};
void xhci_usb2_save_z0_tuning_value(struct usb_hcd *hcd, unsigned int  port, unsigned int z0_value)
{
	xhci_port_z0_tuning_info.hcd = hcd;
	xhci_port_z0_tuning_info.port = port;
	xhci_port_z0_tuning_info.z0_value = z0_value;
}
EXPORT_SYMBOL(xhci_usb2_save_z0_tuning_value);


void xhci_init_wrapper_regs(struct usb_hcd *hcd, unsigned int reg_start)
{
        struct xhci_plat_priv *priv = hcd_to_xhci_priv(hcd);

        //xhci some global regs
        priv->wrapper_regs.XHCI_USB3_TOP_GSBUSCFG0_REG = reg_start + 0xA0008100;
        priv->wrapper_regs.XHCI_USB3_TOP_GSBUSCFG1_REG = reg_start + 0xA0008104;
        priv->wrapper_regs.XHCI_USB3_TOP_GCTL_REG = reg_start + 0xA0008110;
	 priv->wrapper_regs.XHCI_USB3_TOP_GUCTL1_REG = reg_start + 0xA000811C;	
        priv->wrapper_regs.XHCI_USB3_TOP_GUSB2PHYCFG0_REG = reg_start + 0xA0008200;
        priv->wrapper_regs.XHCI_USB3_TOP_GUSB2PHYCFG1_REG = reg_start + 0xA0008204;

        //usb3 wrapper regs
        priv->wrapper_regs.XHCI_USB3_TOP_WRAP_CTR_REG = reg_start + 0xA000A000 + 0x0;
        priv->wrapper_regs.XHCI_USB3_TOP_GNR_INT_REG = reg_start + 0xA000A000 + 0x4;
        priv->wrapper_regs.XHCI_USB3_TOP_USB2_PHY_UTMI_REG = reg_start + 0xA000A000 + 0x8;
        priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG = reg_start + 0xA000A000 + 0xc;
        priv->wrapper_regs.XHCI_USB3_TOP_MDIO_CTR_REG = reg_start + 0xA000A000 + 0x10;
        priv->wrapper_regs.XHCI_USB3_TOP_VSTATUS0_OUT_REG = reg_start + 0xA000A000 + 0x14;
        priv->wrapper_regs.XHCI_USB3_TOP_SLP_BACK0_EN_REG  = reg_start + 0xA000A000 + 0x18;
        priv->wrapper_regs.XHCI_USB3_TOP_SLP_BACK0_CTR_REG  = reg_start + 0xA000A000 + 0x1c;
        priv->wrapper_regs.XHCI_USB3_TOP_SLP_BACK0_ST_REG  = reg_start + 0xA000A000 + 0x20;
        priv->wrapper_regs.XHCI_USB3_TOP_PHY2_SLB0_EN_REG  = reg_start + 0xA000A000 + 0x24;
        priv->wrapper_regs.XHCI_USB3_TOP_PHY2_SLB0_ST_REG  = reg_start + 0xA000A000 + 0x28;
        priv->wrapper_regs.XHCI_USB3_TOP_SLP_BACK1_CTR_REG  = reg_start + 0xA000A000 + 0x2c;
        priv->wrapper_regs.XHCI_USB3_TOP_SLP_BACK1_ST_REG  = reg_start + 0xA000A000 + 0x30;
        priv->wrapper_regs.XHCI_USB3_TOP_PHY2_SLB1_EN_REG  = reg_start + 0xA000A000 + 0x34;
        priv->wrapper_regs.XHCI_USB3_TOP_PHY2_SLB1_ST_REG  = reg_start + 0xA000A000 + 0x38;

        priv->wrapper_regs.XHCI_USB3_TOP_MDIO_CTR_PORT1_REG  = reg_start + 0xA000A000 + 0x4C;

        priv->wrapper_regs.XHCI_USB3_TOP_USB2_SPD_CTR_REG  = reg_start + 0xA000A000 + 0x60;
        priv->wrapper_regs.XHCI_USB3_TOP_PHY3_SLB_EN_REG  = reg_start + 0xA000A000 + 0x64;
        priv->wrapper_regs.XHCI_USB3_TOP_PHY3_SLB_CT_REG  = reg_start + 0xA000A000 + 0x68;
        priv->wrapper_regs.XHCI_USB3_TOP_PHY3_SLB_ST_REG  = reg_start + 0xA000A000 + 0x6c;
        priv->wrapper_regs.XHCI_USB3_TOP_USB_DBG_REG  = reg_start + 0xA000A000 + 0x70;
        priv->wrapper_regs.XHCI_USB3_TOP_USB_SCTCH_REG  = reg_start + 0xA000A000 + 0x74;
        priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_SP_0_REG  = reg_start + 0xA000A000 + 0x78;

        priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_0_REG  = reg_start + 0xA000A000 + 0x80;
        priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_1_REG  = reg_start + 0xA000A000 + 0x84;
        priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_2_REG  = reg_start + 0xA000A000 + 0x88;
        priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_3_REG  = reg_start + 0xA000A000 + 0x8C;
        priv->wrapper_regs.XHCI_USB3_TOP_HMAC_CTR0_REG  = reg_start + 0xA000A000 + 0x90;
        priv->wrapper_regs.XHCI_USB3_TOP_HMAC_CTR1_REG  = reg_start + 0xA000A000 + 0x94;
        priv->wrapper_regs.XHCI_USB3_TOP_MAC3_HST_ST_REG  = reg_start + 0xA000A000 + 0x98;
        priv->wrapper_regs.XHCI_USB3_TOP_USB2_PHY0_REG  = reg_start + 0xA000A000 + 0x9C;

        priv->wrapper_regs.XHCI_USB3_TOP_USB2_PHY1_REG  = reg_start + 0xA000A000 + 0xa0;

        priv->wrapper_regs.XHCI_USB3_TOP_RAM_CTR_REG  = reg_start + 0xA000A000 + 0xac;
        priv->wrapper_regs.XHCI_USB3_TOP_RAM_ADDR_REG  = reg_start + 0xA000A000 + 0xb0;
        priv->wrapper_regs.XHCI_USB3_TOP_RAM_WDATA_REG  = reg_start + 0xA000A000 + 0xb4;
        priv->wrapper_regs.XHCI_USB3_TOP_RAM_RDATA_REG = reg_start + 0xA000A000 + 0xb8;
        priv->wrapper_regs.XHCI_USB3_TOP_USB3_OVR_CT_REG = reg_start + 0xA000A000 + 0xc0;
        priv->wrapper_regs.XHCI_USB3_TOP_ANA_PHY0_REG = reg_start + 0xA000A000 + 0xc4;
        priv->wrapper_regs.XHCI_USB3_TOP_GBL_USB_CT_REG = reg_start + 0xA000A000 + 0xcc;

        priv->wrapper_regs.XHCI_USB3_TOP_CRC_CNT_1_REG = reg_start + 0xA000A000 + 0x154;
        priv->wrapper_regs.XHCI_USB3_TOP_CRC_CNT_2_REG = reg_start + 0xA000A000 + 0x158;
}

/**********************************XHCI PHY RELEATED*********************************************/
#define USB_PHY_DELAY    25
#ifndef CONFIG_XHCI_USING_SPECIAL_PHY_SETTING_FLOW
void xhci_usb3_mdio_write(
        struct xhci_hcd *xhci,
        unsigned char           port,
        unsigned char           phy_addr,
        unsigned char           reg_addr,
        unsigned short          val
)
{
        unsigned int port_mdio_reg = 0;
        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);
        xhci_plat_dbg("mdio_wr(port_%u pg_%d, reg_%02x)=0x%04x\n", port, phy_addr, reg_addr, val);

        if(port >= MAX_XHCI_USB3_PHY_NUM)
        {
                xhci_plat_warn("xhci only support max %u ports\n", MAX_XHCI_USB3_PHY_NUM);
                return;
        }

        port_mdio_reg = (port == 0) ? priv->wrapper_regs.XHCI_USB3_TOP_MDIO_CTR_REG : priv->wrapper_regs.XHCI_USB3_TOP_MDIO_CTR_PORT1_REG;

        rtd_outl(port_mdio_reg, USB3_TOP_MDIO_CTR_data(val) |
                 USB3_TOP_MDIO_CTR_phy_addr(phy_addr) |
                 USB3_TOP_MDIO_CTR_phy_reg_addr(reg_addr) |
                 USB3_TOP_MDIO_CTR_mclk_rate(3) |      // clk_sys/4
                 USB3_TOP_MDIO_CTR_mdio_rdwr(1));

        do
        {
                udelay(USB_PHY_DELAY);
        }
        while ((rtd_inl(port_mdio_reg) & USB3_TOP_MDIO_CTR_mdio_busy_mask));   // wait busy done
}

unsigned short xhci_usb3_mdio_read(
        struct xhci_hcd *xhci,
        unsigned char       port,
        unsigned char       phy_addr,
        unsigned char       reg_addr
)
{
        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);
        unsigned int port_mdio_reg = 0;
        unsigned long ctrl = USB3_TOP_MDIO_CTR_phy_addr(phy_addr) |
                             USB3_TOP_MDIO_CTR_phy_reg_addr(reg_addr) |
                             USB3_TOP_MDIO_CTR_mclk_rate(3) |
                             USB3_TOP_MDIO_CTR_mdio_rdwr(0);

        if(port >= MAX_XHCI_USB3_PHY_NUM)
        {
                xhci_plat_warn("xhci only support max %u ports\n", MAX_XHCI_USB3_PHY_NUM);
                return 0;
        }

        port_mdio_reg = (port == 0) ? priv->wrapper_regs.XHCI_USB3_TOP_MDIO_CTR_REG : priv->wrapper_regs.XHCI_USB3_TOP_MDIO_CTR_PORT1_REG;

        //xhci_drd_dbg("mdio_ctrl=%08lx\n", ctrl);
        rtd_outl(port_mdio_reg, ctrl);
        do
        {
                udelay(USB_PHY_DELAY);
        }
        while((rtd_inl(port_mdio_reg) & USB3_TOP_MDIO_CTR_mdio_busy_mask));   // wait busy done
        return rtd_inl(port_mdio_reg) >> 16;
}
#else
extern void xhci_usb3_mdio_write(
        struct xhci_hcd *xhci,
        unsigned char           port,
        unsigned char           phy_addr,
        unsigned char           reg_addr,
        unsigned short          val
);
extern unsigned short xhci_usb3_mdio_read(
        struct xhci_hcd *xhci,
        unsigned char       port,
        unsigned char       phy_addr,
        unsigned char       reg_addr
);
#endif
int xhci_usb3_phy_init(struct platform_device *pdev)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_hcd *xhci = hcd_to_xhci(hcd);
        struct device_node *p_node;
        U3_PHY_REGISTER *p_reg_array;
        int array_size ;
        int i;
        int ret;
        char *property_name = "usb3phys";
        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);
        unsigned int port_mask = 0;

        port_mask = (1 << priv->usb3_port_cnt) - 1;

        p_node =  pdev->dev.of_node;

        if (!p_node)
        {
                xhci_plat_err("%s get of device fail\n",__func__);
                return -ENODEV;
        }

        array_size = of_property_count_elems_of_size(p_node, property_name, 16);
        if(array_size <= 0)
        {
                xhci_plat_err("%s %s has no value",__func__, property_name);
                return -ENODEV;
        }

        p_reg_array = (U3_PHY_REGISTER *)kzalloc(sizeof(U3_PHY_REGISTER) * array_size, GFP_KERNEL);
        if(!p_reg_array)
        {
                xhci_plat_err("%s no memory",__func__);
                return -ENOMEM;
        }
        for(i = 0; i < array_size; i++)
        {
                unsigned int port;
                unsigned int page;
                unsigned int reg;
                unsigned int val;
                if(of_property_read_u32_index(p_node, property_name, 4 * i, &port)
                   || of_property_read_u32_index(p_node, property_name, 4 * i +1, &page)
                   || of_property_read_u32_index(p_node, property_name, 4 * i + 2, &reg)
                   || of_property_read_u32_index(p_node, property_name, 4 * i + 3, &val))
                {
                        xhci_plat_err("%s dts error",__func__);
                        ret = -EIO;
                        goto EXIT;
                }
                p_reg_array[i].port= port;
                p_reg_array[i].page= page;
                p_reg_array[i].reg = reg;
                p_reg_array[i].val = val;
        }

        xhci_plat_info("--------------- [%s] start ---------------\n", __func__);

        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, port_mask);  // pipe reset u3 phy
        udelay(400);

        for (i = 0; i < array_size; i++)
        {
                //xhci_plat_info(xhci, "%s port %x %x %x\n", __func__, p_reg_array[i].page, p_reg_array[i].reg, p_reg_array[i].val);
                xhci_usb3_mdio_write(xhci, p_reg_array[i].port, p_reg_array[i].page, p_reg_array[i].reg, p_reg_array[i].val);
        }
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0);  // pipe release u3 phy
        udelay(400);

        xhci_usb3_phy_init_post(pdev);

        xhci_plat_info("--------------- [%s] complete ---------------\n", __func__);
        ret = 0;
EXIT:
        kfree(p_reg_array);
        return ret;
}

/* ============================================================================
The table of Z0 VS resistor

Z0 gear | resistor
0x6     | 0R~1.5R
0x5     | 1.6R~3R
0x4     | 3.1R~5.1R
============================================================================ */
static unsigned char xhci_z0_resister_2_gear(unsigned char resistor)
{

        unsigned char nible1;
        unsigned char nible2;

        nible1 = (resistor & 0xf0)>>4;
        nible2 = (resistor & 0xf);
        if( (nible1>9) || (nible2>9) )
                return 0;

        nible1 = (nible1*10)+ nible2;
        if(nible1 <= 15)
                return Z0_GEAR(6);
        else if(nible1 <= 30)
                return Z0_GEAR(5);
        else if(nible1 <= 51)
                return Z0_GEAR(4);
        else
                return 0;
}

static int xhci_usb2_adjust_phy_params(struct xhci_hcd *xhci, U2_PHY_REGISTER*p_reg, unsigned int ofst, int count)
{
        int i;
        unsigned long long z0_value_form_uboot = 0;

        xhci_plat_info("Check PCB enum of USB ofst = %u, count = %d.\n", ofst, count);
        if(pcb_mgr_get_enum_info_byname(Z0_XHCI_PHY_NAME, &z0_value_form_uboot) == 0)
        {
                xhci_plat_info("%s is 0x%0llx", Z0_XHCI_PHY_NAME, z0_value_form_uboot);
        }
        else
        {
                xhci_plat_info("There is no Z0 setting form PCB enum.\n");
                return 0;
        }

        p_reg += ofst;
        for(i=0; i<count; i++)
        {
                unsigned char new_sis = 0;
                unsigned char new_z0 = 0;

                new_sis = (unsigned char)((z0_value_form_uboot >> (8*i))&0xff);
                //xhci_plat_info(xhci, "port%d resistor is %d.%dR.\n",i+1,(new_sis&0xf0)>>4,new_sis&0xf);
                new_z0 = xhci_z0_resister_2_gear(new_sis);
                if(!new_z0)
                {
                        xhci_plat_info("There is no Z0 of port%d.\n",i+1);
                        continue;
                }
                //xhci_plat_info(xhci, "port%d old Z0 is 0x%x,new Z0 is 0x%x\n", i+1, p_reg->val,new_z0);
                p_reg->val = new_z0;
                xhci_plat_info("port%d Z0 is 0x%x", i+1, p_reg->val);

                p_reg++;
        }

        return 0;
}


/*-----------------------------------------------
 * Func : _get_xhci_usb2_phy
 *
 * Desc : read usb2 phy register with UTMI
 *
 * Parm : port1     : port index (1~4)
 *        phy_reg   : phy register address
 *
 * Retn : others : phy register value
 *        -1 : failed
 *-----------------------------------------------*/
static int _get_xhci_usb2_phy(struct xhci_hcd *xhci, int port1, int phy_reg)
{
        int phy_value;
        struct xhci_global_regs __iomem *global_regs = xhci_to_xhci_priv(xhci)->global_regs;
        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);
        int timeout_cnt;

        if (port1 > priv->usb2_port_cnt || port1 < 1)
        {
                xhci_plat_warn("get_usb2_phy failed, invalid port - %d\n", port1);
                return -ENODEV;
        }

        if (phy_reg < 0xe0 || phy_reg > 0xf7)
        {
                xhci_plat_warn("get_usb2_phy failed, invalid phy_reg 0x%x\n", phy_reg);
                return -ENXIO;
        }

        writel(0x02000000 | (phy_reg & 0x0f) << 8, &global_regs->gusb2phyacc[port1 - 1]);
        timeout_cnt = 10000;
        while (!IS_VSTS_DONE(readl(&global_regs->gusb2phyacc[port1 - 1])))
        {
                udelay(1);
                if(--timeout_cnt < 0)
                        break;
        }

        writel(0x02000000 | (phy_reg & 0xd0) << 4, &global_regs->gusb2phyacc[port1 - 1]);
        timeout_cnt = 10000;
        while (!IS_VSTS_DONE(readl(&global_regs->gusb2phyacc[port1 - 1])))
        {
                udelay(1);
                if(--timeout_cnt < 0)
                        break;
        }

        writel(0x02000000 | (phy_reg & 0xd0) << 4, &global_regs->gusb2phyacc[port1 - 1]);
        timeout_cnt = 10000;
        while (!IS_VSTS_DONE(readl(&global_regs->gusb2phyacc[port1 - 1])))
        {
                udelay(1);
                if(--timeout_cnt < 0)
                        break;
        }

        phy_value = readl(&global_regs->gusb2phyacc[port1 - 1]);

        return (phy_value & 0xff);
}


/*-----------------------------------------------
 * Func : _set_xhci_usb2_phy
 *
 * Desc : set usb2 phy register with UTMI
 *
 * Parm : port1     : port index
 *        phy_reg   : phy register address
 *        phy_value : phy register value
 *
 * Retn : 0 : successed,
 *       -1 : failed
 *-----------------------------------------------*/
static int _set_xhci_usb2_phy(struct xhci_hcd *xhci, int port1, int phy_reg, int phy_value)
{
        int value = 0;
        struct xhci_global_regs __iomem *global_regs = xhci_to_xhci_priv(xhci)->global_regs;
        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);
        int timeout_cnt;

        if (port1 > priv->usb2_port_cnt || port1 < 1)
        {
                xhci_plat_warn("%s failed, invalid port - %d\n", __func__, port1);
                return -ENODEV;
        }

        if (phy_reg < 0xe0 || phy_reg > 0xf7)
        {
                xhci_plat_warn("%s failed, invalid phy_reg 0x%x\n", __func__, phy_reg);
                return -ENXIO;
        }

        // write value
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_VSTATUS0_OUT_REG, phy_value << ((port1 - 1) * 8));
        udelay(USB_PHY_DELAY);

        writel(0x02000000 | (phy_reg & 0x0f) << 8, &global_regs->gusb2phyacc[port1 - 1]);
        timeout_cnt = 10000;
        while (!IS_VSTS_DONE(readl(&global_regs->gusb2phyacc[port1 - 1])))
        {
                udelay(1);
                if(--timeout_cnt < 0)
                        break;
        }

        writel(0x02000000 | (phy_reg & 0xf0) << 4, &global_regs->gusb2phyacc[port1 - 1]);
        timeout_cnt = 10000;
        while (!IS_VSTS_DONE(readl(&global_regs->gusb2phyacc[port1 - 1])))
        {
                udelay(1);
                if(--timeout_cnt < 0)
                        break;
        }

        value = _get_xhci_usb2_phy(xhci, port1, phy_reg);
        if (value < 0)
                return value;

        if ((phy_value & 0xff) != value)
                xhci_plat_warn("set_usb2_phy, port %d phy_reg 0x%.2x set 0x%x get 0x%x <--- not matched\n",
                               port1, phy_reg, phy_value & 0xff, value);
        else
                xhci_plat_dbg("set_usb2_phy, port %d phy_reg 0x%.2x set 0x%x get 0x%x\n",
                              port1, phy_reg, phy_value & 0xff, value);

        return 0;
}

#include <linux/mutex.h>
static DEFINE_MUTEX(g_xhci_phy_set_lock);

int set_xhci_usb2_phy_reg(struct xhci_hcd *xhci, unsigned char phy, unsigned char page, unsigned char addr, unsigned char val)
{
        int ret = 0;

        if (page > 3)
        {
                xhci_plat_warn("%s failed, invalid page - %d\n", __func__, page);
                return -ENXIO;
        }
		mutex_lock(&g_xhci_phy_set_lock);
        ret = _set_xhci_usb2_phy(xhci, phy, 0xF4, (page << 5) | 0x9b); // bit [6:5] : page select = page
        if (ret)
			goto EXIT;

        ret = _set_xhci_usb2_phy(xhci, phy, addr, val);
EXIT:
		mutex_unlock(&g_xhci_phy_set_lock);
		return ret;
}
EXPORT_SYMBOL(set_xhci_usb2_phy_reg);

int xhci_hcd_set_phy_reg(struct usb_hcd *hcd, unsigned char phy, unsigned char page, unsigned char addr, unsigned char val)
{
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	return set_xhci_usb2_phy_reg(xhci, phy, page, addr, val);
}
EXPORT_SYMBOL(xhci_hcd_set_phy_reg);

int get_xhci_usb2_phy_reg(struct xhci_hcd *xhci, unsigned char phy, unsigned char page, unsigned char addr)
{
        int ret = 0;

        if (page > 3)
        {
                xhci_plat_warn("%s failed, invalid page - %d\n", __func__, page);
                return -ENXIO;
        }
		mutex_lock(&g_xhci_phy_set_lock);
        ret = _set_xhci_usb2_phy(xhci, phy, 0xF4, (page<<5) | 0x9b);  // bit [6:5] : page select = page
        if (ret)
                goto EXIT;

        ret = _get_xhci_usb2_phy(xhci, phy, addr);
EXIT:
		mutex_unlock(&g_xhci_phy_set_lock);
		return ret;				
}
EXPORT_SYMBOL(get_xhci_usb2_phy_reg);

int xhci_hcd_get_phy_reg(struct usb_hcd *hcd, unsigned char phy, unsigned char page, unsigned char addr)
{
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	return get_xhci_usb2_phy_reg(xhci, phy, page, addr);
}
EXPORT_SYMBOL(xhci_hcd_get_phy_reg);

void xhci_hcd_disconnect_usb2_port(struct usb_hcd *hcd, unsigned int port_num)
{
	struct xhci_hcd *xhci = NULL;
	unsigned int val = 0;
	if(!hcd)
		return;
	xhci = hcd_to_xhci(hcd);
	if(port_num == 0 || port_num > xhci->usb2_rhub.num_ports)
		return;
	val = readl( xhci->usb2_rhub.ports[port_num - 1]->addr);
	writel(val & (~(1 << 9)), xhci->usb2_rhub.ports[port_num - 1]->addr);
	ndelay(1000);
	writel(val | (1 << 9), xhci->usb2_rhub.ports[port_num - 1]->addr);
}
EXPORT_SYMBOL(xhci_hcd_disconnect_usb2_port);

/*-----------------------------------------------
 * Func : _usb2_load_phy_setting
 *
 * Desc : load usb2 phy setting
 *
 * Parm : port1 : port index (1~4)
 *        p_reg : register list
 *        n_reg : register value
 *
 * Retn : N/A
 *-----------------------------------------------*/
static int _usb2_load_phy_setting(
        struct xhci_hcd      *xhci,
        U2_PHY_REGISTER*    p_reg,
        unsigned int        n_reg
)
{
        int err = 0;
        while (n_reg--)
        {
                //xhci_plat_warn(xhci, "%s port %x %x %x",__func__, p_reg->port1, p_reg->addr, p_reg->val);
                err = _set_xhci_usb2_phy(xhci, p_reg->port1,  p_reg->addr, p_reg->val);
                if (err)
                        break;
                p_reg++;
        }

        return err;
}

int xhci_usb2_set_phy_from_otp(struct platform_device *pdev)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_OTP)
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_hcd *xhci = hcd_to_xhci(hcd);
        struct device_node *p_node;
        int i;
        int ret = -1;
        int otp_val_array_size;

        p_node =  pdev->dev.of_node;

        otp_val_array_size = of_property_count_elems_of_size(p_node, "usb2_otp_values", 20);
        if(otp_val_array_size <= 0)
        {
                xhci_plat_err("usb2 no otp info\n");
                return 0;
        }

        for(i = 0; i < otp_val_array_size; i++)
        {
                unsigned int otp_field_val;
                unsigned int otp_field_done;
                unsigned int otp_offset;
                unsigned int otp_bits;
                unsigned int page;
                unsigned int reg;
                unsigned int bits_offset;
                const char *otp_field_name = NULL;
                const char *otp_filed_done_name = NULL;
                if(of_property_read_string_index(p_node, "usb2_otp_names", 2 * i,  &otp_field_name) != 0)
                {
                        xhci_plat_err("usb2 get otp field name failed\n");
                        goto EXIT;
                }

                if(of_property_read_string_index(p_node, "usb2_otp_names", 2 * i + 1,  &otp_filed_done_name) != 0)
                {
                        xhci_plat_err("usb2 get otp field done name failed\n");
                        goto EXIT;
                }

                if(rtk_otp_field_read_by_name(otp_filed_done_name, (unsigned char *)&otp_field_done, sizeof(otp_field_done)) != true)
                {
                        xhci_plat_err("get %s from otp failed\n", otp_filed_done_name);
                        ret = -EIO;
                        goto EXIT;
                }

                if(!otp_field_done)
                        continue;

                if(rtk_otp_field_read_by_name(otp_field_name, (unsigned char *)&otp_field_val, sizeof(otp_field_val)) != true)
                {
                        xhci_plat_err("get %s from otp failed\n", otp_field_name);
                        ret = -EIO;
                        goto EXIT;
                }

                if(of_property_read_u32_index(p_node, "usb2_otp_values", 5 * i, &otp_offset)
                   || of_property_read_u32_index(p_node, "usb2_otp_values", 5 * i + 1, &otp_bits)
                   || of_property_read_u32_index(p_node, "usb2_otp_values", 5 * i + 2, &page)
                   || of_property_read_u32_index(p_node, "usb2_otp_values", 5 * i + 3, &reg)
                   || of_property_read_u32_index(p_node, "usb2_otp_values", 5 * i + 4, &bits_offset))
                {
                        ret = -EIO;
                        goto EXIT;
                }

                xhci_plat_info("usb2 otp info : name=%s, done_name=%s, done=%x,  value=%x, offset=%x, otp_bits=%x, page=%x, reg=%x, bits_offset=%x\n",
                               otp_field_name, otp_filed_done_name, otp_field_done, otp_field_val, otp_offset, otp_bits, page, reg, bits_offset);

                {
                        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);
                        int portnum = 0;
                        for (portnum = 0; portnum < priv->usb2_port_cnt; portnum++)
                        {
                                unsigned int val;
                                unsigned int tmp_otp;
                                unsigned int mask = ((1 << otp_bits) - 1);
                                val = get_xhci_usb2_phy_reg(xhci, portnum + 1, page, reg);
                                tmp_otp = otp_field_val >> (portnum * otp_bits + otp_offset);
                                tmp_otp = tmp_otp & mask;
                                /*
                                    If you need to modify the setting value of a port,
                                    change it here
                                */
                                val = (val & ~(mask << bits_offset)) | (tmp_otp << bits_offset);
                                ret = set_xhci_usb2_phy_reg(xhci, portnum + 1, page, reg, val);
                                if (ret)
                                        goto EXIT;
                                xhci_plat_info("set port(%d) swing level to 0x%x \n",
                                               (portnum + 1),
                                               (get_xhci_usb2_phy_reg(xhci, portnum + 1, page, reg) & (mask << bits_offset)) >> bits_offset);
                        }
                }
        }
        ret = 0;
EXIT:
        return ret;
#else
        xhci_plat_warn("CONFIG_RTK_KDRV_OTP not define\n");
        return 0;
#endif
}

#if 0
static inline int xhci_usb3_set_phy_from_otp(struct platform_device *pdev)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_OTP)
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_hcd *xhci = hcd_to_xhci(hcd);
        struct device_node *p_node;
        int i;
        int ret = -1;
        int otp_val_array_size;

        p_node =  pdev->dev.of_node;

        otp_val_array_size = of_property_count_elems_of_size(p_node, "usb3_otp_values", 20);
        if(otp_val_array_size <= 0)
        {
                xhci_plat_err("usb3 no otp info\n");
                return 0;
        }

        for(i = 0; i < otp_val_array_size; i++)
        {
                unsigned int otp_field_val;
                unsigned int otp_field_done;
                unsigned int otp_offset;
                unsigned int otp_bits;
                unsigned int page;
                unsigned int reg;
                unsigned int bits_offset;
                const char *otp_field_name = NULL;
                const char *otp_filed_done_name = NULL;
                if(of_property_read_string_index(p_node, "usb3_otp_names", 2 * i,  &otp_field_name) != 0)
                {
                        xhci_plat_err("usb3 get otp field name failed\n");
                        goto EXIT;
                }

                if(of_property_read_string_index(p_node, "usb3_otp_names", 2 * i + 1,  &otp_filed_done_name) != 0)
                {
                        xhci_plat_err("usb3 get otp field done name failed\n");
                        goto EXIT;
                }

                if(rtk_otp_field_read_by_name(otp_filed_done_name, (unsigned char *)&otp_field_done, sizeof(otp_field_done)) != true)
                {
                        xhci_plat_err("get %s from otp failed\n", otp_filed_done_name);
                        ret = -EIO;
                        goto EXIT;
                }

                if(!otp_field_done)
                        continue;

                if(rtk_otp_field_read_by_name(otp_field_name, (unsigned char *)&otp_field_val, sizeof(otp_field_val)) != true)
                {
                        xhci_plat_err("get %s from otp failed\n", otp_field_name);
                        ret = -EIO;
                        goto EXIT;
                }

                if(of_property_read_u32_index(p_node, "usb3_otp_values", 5 * i, &otp_offset)
                   || of_property_read_u32_index(p_node, "usb3_otp_values", 5 * i + 1, &otp_bits)
                   || of_property_read_u32_index(p_node, "usb3_otp_values", 5 * i + 2, &page)
                   || of_property_read_u32_index(p_node, "usb3_otp_values", 5 * i + 3, &reg)
                   || of_property_read_u32_index(p_node, "usb3_otp_values", 5 * i + 4, &bits_offset))
                {
                        ret = -EIO;
                        goto EXIT;
                }

                xhci_plat_info(xhci, "usb3 otp info : name=%s, done_name=%s, done=%x,  value=%x, offset=%x, otp_bits=%x, page=%x, reg=%x, bits_offset=%x\n",
                               otp_field_name, otp_filed_done_name, otp_field_done, otp_field_val, otp_offset, otp_bits, page, reg, bits_offset);

                {
                        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);
                        int portnum = 0;
                        for (portnum = 0; portnum < priv->usb3_port_cnt; portnum++)
                        {
                                unsigned int val;
                                unsigned int tmp_otp;
                                unsigned int mask = ((1 << otp_bits) - 1);
                                tmp_otp = otp_field_val >> (portnum * otp_bits + otp_offset);
                                val = mdio_read(xhci, portnum, page, reg);
                                tmp_otp = tmp_otp & mask;
                                /*
                                    If you need to modify the setting value of a port,
                                    change it here
                                */
                                val = (val & ~(mask << bits_offset)) | (tmp_otp << bits_offset);
                                mdio_write(xhci, portnum, page, reg, val);
                                xhci_plat_info("%s phy=%x\n", __func__, mdio_read(xhci, portnum, page, reg));
                        }
                }
        }
        ret = 0;
EXIT:
        return ret;
#else
        xhci_plat_warn("CONFIG_RTK_KDRV_OTP not define\n");
        return 0;
#endif
}
#endif


int xhci_usb2_phy_init(struct platform_device *pdev)
{
        int ret = 0;
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_hcd *xhci = hcd_to_xhci(hcd);
        struct device_node *p_node;
        U2_PHY_REGISTER *p_reg_array;
        int array_size ;
        unsigned int Z0_ofst;
        unsigned int Z0_count;
        int i;
        char *propert_name = "usb2phys";
#ifdef CONFIG_CUSTOMER_TV006
        if (get_platform() == PLATFORM_KXLP)    //KxLP_MODEL
        {
                xhci_plat_info("%s(%d)KxLP\n",__func__,__LINE__);
                propert_name = "usb2phys_lp";
        }
        else    //KxHP_MODEL
        {
                if(is_OLED_PANEL() > 0)     //OLED_PANEL
                {
                        xhci_plat_info("%s(%d)KxHP & OLED\n",__func__,__LINE__);
                        propert_name = "usb2phys_oled";
                }
        }
#endif
        p_node =  pdev->dev.of_node;
        if (!p_node)
        {
                xhci_plat_err("%s get of device fail\n",__func__);
                return -ENODEV;
        }

        array_size = of_property_count_elems_of_size(p_node, propert_name, 12);
        if(array_size <= 0)
        {
                xhci_plat_warn("%s %s has no value",__func__, propert_name);
                return -ENODEV;
        }
        p_reg_array = (U2_PHY_REGISTER *)kzalloc(sizeof(U2_PHY_REGISTER) * array_size, GFP_KERNEL);
        if(!p_reg_array)
        {
                xhci_plat_warn("%s no memory",__func__);
                return -ENOMEM;
        }
        for(i = 0; i < array_size; i++)
        {
                unsigned int port;
                unsigned int addr;
                unsigned int val;
                if(of_property_read_u32_index(p_node, propert_name, 3 * i, &port)
                   || of_property_read_u32_index(p_node, propert_name, 3 * i + 1, &addr)
                   || of_property_read_u32_index(p_node, propert_name, 3 * i + 2, &val))
                {
                        ret = -EIO;
                        goto EXIT;
                }
                p_reg_array[i].port1= port;
                p_reg_array[i].addr = addr;
                p_reg_array[i].val = val;
        }


        if(of_property_read_u32_index(p_node, "usb2physZ0", 0, &Z0_ofst) == 0
           && of_property_read_u32_index(p_node, "usb2physZ0", 1, &Z0_count) == 0)
        {
                if((Z0_ofst < array_size)
                   && ((Z0_ofst + Z0_count) <= array_size))
                        xhci_usb2_adjust_phy_params(xhci, p_reg_array, Z0_ofst, Z0_count);
        }

        ret = _usb2_load_phy_setting(xhci, p_reg_array, array_size);
#ifdef XHCI_PORT_Z0_TUNING_DATA_EFFECTED
	 if(hcd == xhci_port_z0_tuning_info.hcd 
	 	&& xhci_port_z0_tuning_info.port != 0 
	 	&& xhci_port_z0_tuning_info.z0_value != 0xFFFFFFFF) {
		unsigned int mask = ((1 << XHCI_PORT_Z0_BIT_NUM) - 1);
		unsigned int val = xhci_hcd_get_phy_reg(hcd,  xhci_port_z0_tuning_info.port, XHCI_PORT_Z0_PAGE, XHCI_PORT_Z0_REG);
		val = (val & ~(mask << XHCI_PORT_Z0_BIT_OFFSET)) 
				  	| (xhci_port_z0_tuning_info.z0_value << XHCI_PORT_Z0_BIT_OFFSET);
		xhci_hcd_set_phy_reg(hcd,  xhci_port_z0_tuning_info.port, XHCI_PORT_Z0_PAGE, XHCI_PORT_Z0_REG, val);
		xhci_plat_info("[WIFI] port[%u] init tuning  z0 to %x\n", xhci_port_z0_tuning_info.port, xhci_port_z0_tuning_info.z0_value);
		
	 }
#endif
EXIT:
        kfree(p_reg_array);
        return ret;
}

int xhci_usb2_set_swing(struct platform_device *pdev, unsigned int swing_val)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct xhci_hcd *xhci = hcd_to_xhci(hcd);
        int portnum = 0, error;
        unsigned int val;
        struct xhci_plat_priv *priv = xhci_to_xhci_priv(xhci);

        for (portnum = 0; portnum < priv->usb2_port_cnt; portnum++)
        {
                val = get_xhci_usb2_phy_reg(xhci, portnum + 1, 0, 0xe4);
                val &= ~0x0f;
                val |= (swing_val & 0x0f);
                error = set_xhci_usb2_phy_reg(xhci, portnum + 1, 0, 0xe4, val);
                if (error)
                        goto out;
                xhci_plat_info("set port(%d) swing value (0x%x) from TOOL_OBJ \n",
                               (portnum + 1),(get_xhci_usb2_phy_reg(xhci, portnum + 1, 0, 0xe4) & 0x0f));
        }

out:
        return error;
}