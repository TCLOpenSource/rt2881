#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/regulator/consumer.h>
#include <rbus/usb3_device_top_reg.h> /* for U3 wrapper register define */
#include <rbus/sys_reg_reg.h>
#include <rtk_kdriver/rtk_otp_region_api.h>
#include <rtk_kdriver/io.h>
#include <linux/delay.h>
#include "xhci-plat-drd.h"
#include "xhci-plat-drd-hw-wrapper.h"

void rtk_xhci_drd_mdio_write(
        struct rtk_xhci_drd *drd,
        unsigned char           port,
        unsigned char           phy_addr,
        unsigned char           reg_addr,
        unsigned short          val
);
unsigned short rtk_xhci_drd_mdio_read(
        struct rtk_xhci_drd *drd,
        unsigned char       port,
        unsigned char       phy_addr,
        unsigned char       reg_addr
);

static void rtk_xhci_drd_crt(bool on)
{
	if(on) {
		rtd_outl(SYS_REG_SYS_SRST3_reg,SYS_REG_SYS_SRST3_rstn_usb2_phy_p0_mask |SYS_REG_SYS_SRST3_write_data(0));
		udelay(USB_PHY_DELAY);
		
		rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb3_dev_dwc_mask | SYS_REG_SYS_SRST6_rstn_usb3_dev_wrapper_mask);
		rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb3_dev_dwc_mask | SYS_REG_SYS_CLKEN6_clken_usb3_dev_wrapper_mask);

		udelay(USB_PHY_DELAY);
		
		rtd_outl(SYS_REG_SYS_SRST3_reg,SYS_REG_SYS_SRST3_rstn_usb2_phy_p0_mask | SYS_REG_SYS_SRST3_write_data(1));
		
		udelay(USB_PHY_DELAY);
		
		rtd_outl(SYS_REG_SYS_CLKEN6_reg, SYS_REG_SYS_CLKEN6_clken_usb3_dev_dwc_mask | SYS_REG_SYS_CLKEN6_clken_usb3_dev_wrapper_mask | SYS_REG_SYS_CLKEN6_write_data_mask);
		rtd_outl(SYS_REG_SYS_SRST6_reg, SYS_REG_SYS_SRST6_rstn_usb3_dev_dwc_mask | SYS_REG_SYS_SRST6_rstn_usb3_dev_wrapper_mask | SYS_REG_SYS_SRST6_write_data_mask);
	}
}

static void rtk_xhci_usb2_phy_load(struct platform_device *pdev)
{
	
}

int rtk_xhci_drd_mac_init(struct platform_device *pdev)
{
        struct rtk_xhci_drd*drd = platform_get_drvdata(pdev);
        struct xhci_drd_priv *priv = drd_to_xhci_drd_priv(drd);

        xhci_drd_info("--------- %s start ------------ \n", __func__);
		rtk_xhci_drd_crt(true);
        // UTMI reset
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB2_PHY_UTMI_REG, 0x3);

        // UTMI release
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB2_PHY_UTMI_REG, 0x0);

        // PHY ignores suspend signal and always keep UTMI clock active
        rtd_setbits(priv->wrapper_regs.XHCI_USB3_TOP_USB_TMP_1_REG,
                    USB3_DEVICE_TOP_USB_TMP_1_test_reg_1(1) | USB3_DEVICE_TOP_USB_TMP_1_usb_ref_sel(0xd));

        //axi init
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_GSBUSCFG0_REG, 0x0000001e);


        // bit3=1, port0-UTMI clcok=30MHz
        rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_GUSB2PHYCFG0_REG, rtd_inl(priv->wrapper_regs.XHCI_USB3_TOP_GUSB2PHYCFG0_REG) | (1 << 3));

		rtk_xhci_usb2_phy_load(pdev);
        xhci_drd_info("--------- %s finish ------------ \n", __func__);
        return 0;
}

int rtk_xhci_drd_phy_init_post(struct platform_device *pdev)
{
        return 0;
}

int rtk_xhci_drd_mac_postinit(struct platform_device *pdev)
{
        struct rtk_xhci_drd *drd = platform_get_drvdata(pdev);
        struct xhci_drd_priv *priv = drd_to_xhci_drd_priv(drd);
		rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x3);
		rtd_outl(priv->wrapper_regs.XHCI_USB3_TOP_USB3_PHY_PIPE_REG, 0x0);
        return 0;
}

void rtk_xhci_set_global_interrupts(bool on)
{
        if(on)
                rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb3_drd_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_3_write_data_mask);
        else
                rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb3_drd_int_scpu_routing_en_mask);

}