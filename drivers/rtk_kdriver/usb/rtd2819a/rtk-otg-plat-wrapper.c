#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include <rbus/sys_reg_reg.h>
#include <rtk_kdriver/io.h>
#include <rbus/usb2_otg_reg.h>
#include <rbus/usb2_otg_2_reg.h>
#include <rbus/usb2_top_reg.h>
#include "rtk-otg-plat.h"
#include "rtk-otg-plat-wrapper.h"

#define USB_PHY_DELAY                         120
#ifdef CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM
extern struct platform_device *get_ehci_platform_device(void);
#endif
extern bool rtk_otg_is_via_hub_port(void);

static void rtk_otg_1_crt_on_off(bool on)
{
        if(on)
        {
                /*---------------------------------------------------
                    * shutdown sequence :
                    *  1. mac clock off
                    *  2. mac reset off
                    *  3. phy reset off
                    *---------------------------------------------------*/

	        // clock off : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN6_reg,
	                SYS_REG_SYS_CLKEN6_clken_otg_2_mask |
	                SYS_REG_SYS_CLKEN6_write_data(0));
	        udelay(USB_PHY_DELAY);

	        // Reset MAC and OTG
	        rtd_outl(SYS_REG_SYS_SRST6_reg,
	                SYS_REG_SYS_SRST6_rstn_usb_otg_wrapper_2_mask |
	                SYS_REG_SYS_SRST6_rstn_usb_otg_dwc_2_mask |
	                SYS_REG_SYS_SRST6_write_data(0));
	        udelay(USB_PHY_DELAY);

#ifdef ENABLE_OTG_PHY_RESETING
	        // Reset PHY
	        rtd_outl(SYS_REG_SYS_SRST3_reg,
	            SYS_REG_SYS_SRST3_rstn_usb2_phy_p2_mask |
	            SYS_REG_SYS_SRST3_write_data(0));
	        udelay(USB_PHY_DELAY);
#endif
	        /*---------------------------------------------------
	            * init sequence :
	            *  1. phy reset release
	            *  2. mac clock on
	            *  3. mac clock off
	            *  4. mac reset on
	            *  5. mac clock on
	            *---------------------------------------------------*/
#ifdef ENABLE_OTG_PHY_RESETING
	        // Release USB Phy reset : USB Phy Reset Ready
	        rtd_outl(SYS_REG_SYS_SRST3_reg,
	                SYS_REG_SYS_SRST3_rstn_usb2_phy_p2_mask |
	                SYS_REG_SYS_SRST3_write_data(1));
	        udelay(USB_PHY_DELAY);
#endif
	        
	        // clock on : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN6_reg,
	                SYS_REG_SYS_CLKEN6_clken_otg_2_mask |
	                SYS_REG_SYS_CLKEN6_write_data(1));
	        udelay(USB_PHY_DELAY);

	        // clock off : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN6_reg,
	                SYS_REG_SYS_CLKEN6_clken_otg_2_mask |
	                SYS_REG_SYS_CLKEN6_write_data(0));
	        udelay(USB_PHY_DELAY);

	        // Relase USB MAC Reset : USB MAC Reset Ready
	        rtd_outl(SYS_REG_SYS_SRST6_reg,
	                SYS_REG_SYS_SRST6_rstn_usb_otg_wrapper_2_mask |
	                SYS_REG_SYS_SRST6_rstn_usb_otg_dwc_2_mask |
	                SYS_REG_SYS_SRST6_write_data(1));
	        udelay(USB_PHY_DELAY);

	        // clock on : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN6_reg,
	                SYS_REG_SYS_CLKEN6_clken_otg_2_mask |
	                SYS_REG_SYS_CLKEN6_write_data(1));
	        udelay(USB_PHY_DELAY);

        }
        else
        {
	        rtd_outl(SYS_REG_SYS_CLKEN6_reg,
	                SYS_REG_SYS_CLKEN6_clken_otg_2_mask |
	                SYS_REG_SYS_CLKEN6_write_data(0));

	        rtd_outl(SYS_REG_SYS_SRST6_reg,
	                SYS_REG_SYS_SRST6_rstn_usb_otg_wrapper_2_mask |
	                SYS_REG_SYS_SRST6_rstn_usb_otg_dwc_2_mask |
	                SYS_REG_SYS_SRST6_write_data(0));
#ifdef ENABLE_OTG_PHY_RESETING
		// Reset PHY
	        rtd_outl(SYS_REG_SYS_SRST3_reg,
	            SYS_REG_SYS_SRST3_rstn_usb2_phy_p2_mask |
	            SYS_REG_SYS_SRST3_write_data(0));
	        udelay(USB_PHY_DELAY);
#endif	
        }
}

void rtk_otg_2_crt_on_off(bool on)
{
        if(on)
        {
                /*---------------------------------------------------
                    * shutdown sequence :
                    *  1. mac clock off
                    *  2. mac reset off
                    *  3. phy reset off
                    *---------------------------------------------------*/

	        // clock off : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN5_reg,
	                SYS_REG_SYS_CLKEN5_clken_otg_mask |
	                SYS_REG_SYS_CLKEN5_write_data(0));
	        udelay(USB_PHY_DELAY);

	        // Reset MAC and OTG
	        rtd_outl(SYS_REG_SYS_SRST5_reg,
	                SYS_REG_SYS_SRST5_rstn_usb_otg_wrapper_mask |
	                SYS_REG_SYS_SRST5_rstn_usb_otg_dwc_mask |
	                SYS_REG_SYS_SRST5_write_data(0));
	        udelay(USB_PHY_DELAY);

#ifdef ENABLE_OTG_PHY_RESETING
	        // Reset PHY
	        rtd_outl(SYS_REG_SYS_SRST3_reg,
	            SYS_REG_SYS_SRST3_rstn_usb2_phy_p1_mask |
	            SYS_REG_SYS_SRST3_write_data(0));
	        udelay(USB_PHY_DELAY);
#endif
	        /*---------------------------------------------------
	            * init sequence :
	            *  1. phy reset release
	            *  2. mac clock on
	            *  3. mac clock off
	            *  4. mac reset on
	            *  5. mac clock on
	            *---------------------------------------------------*/
#ifdef ENABLE_OTG_PHY_RESETING
	        // Release USB Phy reset : USB Phy Reset Ready
	        rtd_outl(SYS_REG_SYS_SRST3_reg,
	                SYS_REG_SYS_SRST3_rstn_usb2_phy_p1_mask |
	                SYS_REG_SYS_SRST3_write_data(1));
	        udelay(USB_PHY_DELAY);
#endif
	        
	        // clock on : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN5_reg,
	                SYS_REG_SYS_CLKEN5_clken_otg_mask |
	                SYS_REG_SYS_CLKEN5_write_data(1));
	        udelay(USB_PHY_DELAY);

	        // clock off : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN5_reg,
	                SYS_REG_SYS_CLKEN5_clken_otg_mask |
	                SYS_REG_SYS_CLKEN5_write_data(0));
	        udelay(USB_PHY_DELAY);

	        // Relase USB MAC Reset : USB MAC Reset Ready
	        rtd_outl(SYS_REG_SYS_SRST5_reg,
	                SYS_REG_SYS_SRST5_rstn_usb_otg_wrapper_mask |
	                SYS_REG_SYS_SRST5_rstn_usb_otg_dwc_mask |
	                SYS_REG_SYS_SRST5_write_data(1));
	        udelay(USB_PHY_DELAY);

	        // clock on : MAC and OTG
	        rtd_outl(SYS_REG_SYS_CLKEN5_reg,
	                SYS_REG_SYS_CLKEN5_clken_otg_mask |
	                SYS_REG_SYS_CLKEN5_write_data(1));
	        udelay(USB_PHY_DELAY);

        }
        else
        {
	        rtd_outl(SYS_REG_SYS_CLKEN5_reg,
	                SYS_REG_SYS_CLKEN5_clken_otg_mask |
	                SYS_REG_SYS_CLKEN5_write_data(0));

	        rtd_outl(SYS_REG_SYS_SRST5_reg,
	                SYS_REG_SYS_SRST5_rstn_usb_otg_wrapper_mask |
	                SYS_REG_SYS_SRST5_rstn_usb_otg_dwc_mask |
	                SYS_REG_SYS_SRST5_write_data(0));
#ifdef ENABLE_OTG_PHY_RESETING
		// Reset PHY
	        rtd_outl(SYS_REG_SYS_SRST3_reg,
	            SYS_REG_SYS_SRST3_rstn_usb2_phy_p1_mask |
	            SYS_REG_SYS_SRST3_write_data(0));
	        udelay(USB_PHY_DELAY);
#endif	
        }

}

void rtk_otg_crt_on_off(bool on)
{
	rtk_otg_1_crt_on_off(on);
	rtk_otg_2_crt_on_off(on);
}
EXPORT_SYMBOL(rtk_otg_crt_on_off);

int rtk_otg_initial(struct platform_device *pdev)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
		if(private_data->is_enable) {
			if(rtk_otg_is_via_hub_port()) {
				rtd_outl(USB2_TOP_USB_OTG_REG_reg, 0x0);
    			rtd_outl(USB2_OTG_2_USB_OTG_REG_reg, USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1));	
			}
			else {
				rtd_outl(USB2_TOP_USB_OTG_REG_reg, USB2_TOP_USB_OTG_REG_otg_enable(0x1));
				rtd_outl(USB2_OTG_2_USB_OTG_REG_reg, 0x0);
			}
		} else {
			rtd_outl(USB2_TOP_USB_OTG_REG_reg, 0x0);
			rtd_outl(USB2_OTG_2_USB_OTG_REG_reg, rtd_inl(USB2_OTG_2_USB_OTG_REG_reg) & (~(USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1))));	
		}
	} else {
		if(private_data->is_enable) {
			rtd_outl(USB2_OTG_USB_OTG_REG_reg, USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1));
		} else {
			rtd_outl(USB2_OTG_USB_OTG_REG_reg, rtd_inl(USB2_OTG_USB_OTG_REG_reg) & (~(USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1))));	
		}
	}
	
	if(private_data->id == 0) {
		// pull reset signal of U2 P0
		rtd_outl(USB2_OTG_2_RESET_UTMI_reg, 
	                    USB2_OTG_RESET_UTMI_usb_ref_sel(0xd) | USB2_OTG_RESET_UTMI_reset_utmi_p0(0x1));
		udelay(USB_PHY_DELAY);

		// release reset signal
		rtd_outl(USB2_OTG_2_RESET_UTMI_reg, 
	                    USB2_OTG_RESET_UTMI_usb_ref_sel(0xd) | USB2_OTG_RESET_UTMI_reset_utmi_p0(0x0));

		udelay(USB_PHY_DELAY);
	} else {
		// pull reset signal of U2 P0
		rtd_outl(USB2_OTG_RESET_UTMI_reg, 
	                    USB2_OTG_RESET_UTMI_usb_ref_sel(0xd) | USB2_OTG_RESET_UTMI_reset_utmi_p0(0x1));
		udelay(USB_PHY_DELAY);

		// release reset signal
		rtd_outl(USB2_OTG_RESET_UTMI_reg, 
	                    USB2_OTG_RESET_UTMI_usb_ref_sel(0xd) | USB2_OTG_RESET_UTMI_reset_utmi_p0(0x0));

		udelay(USB_PHY_DELAY);
	}


	return 0;
}

void rtk_otg_plat_set_global_interrupts(struct platform_device *pdev, bool on)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
        if(on)
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb2_otg2_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_3_write_data_mask);
        else
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb2_otg2_int_scpu_routing_en_mask);

	} else {
		if(on)
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb2_otg_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_3_write_data_mask);
        else
			rtd_outl(SYS_REG_INT_CTRL_SCPU_3_reg, SYS_REG_INT_CTRL_SCPU_3_usb2_otg_int_scpu_routing_en_mask);
	}
}

void rtk_otg_plat_set_local_interrupts(struct platform_device *pdev, bool on)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
		if(on)
			rtd_outl(USB2_OTG_2_USB_CTR0_reg, rtd_inl(USB2_OTG_2_USB_CTR0_reg) | USB2_OTG_2_USB_CTR0_usb_top_int_en_mask);
        else
			rtd_outl(USB2_OTG_2_USB_CTR0_reg, rtd_inl(USB2_OTG_2_USB_CTR0_reg) & (~USB2_OTG_2_USB_CTR0_usb_top_int_en_mask));

	} else {
		if(on)
			rtd_outl(USB2_OTG_USB_CTR0_reg, rtd_inl(USB2_OTG_USB_CTR0_reg) | USB2_OTG_USB_CTR0_usb_top_int_en_mask);
        else
			rtd_outl(USB2_OTG_USB_CTR0_reg, rtd_inl(USB2_OTG_USB_CTR0_reg) & (~USB2_OTG_USB_CTR0_usb_top_int_en_mask));
	}
}

int rtk_otg_plat_pre_resume(struct platform_device *pdev)
{
#ifdef CONFIG_RTK_KDRV_EHCI_HCD_PLATFORM
		struct RTK_OTG_PRIVATE_DATA *private_data = platform_get_drvdata(pdev);
		if(private_data->id == 0) {
	        struct platform_device *ehci_platform_dev = get_ehci_platform_device();

	        if (ehci_platform_dev->dev.power.is_suspended)
	        {
	                device_pm_wait_for_dev(&pdev->dev, &ehci_platform_dev->dev);
	        }
		}
#endif
        return 0;
}

char *rtk_otg_plat_get_platform_device_name(struct platform_device *pdev)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0)
		return "180a0000.dwc2";
	else
		return "18090000.dwc22";
}


void rtk_otg_plat_enable_otg(struct platform_device *pdev, bool is_enable)
{
	struct RTK_OTG_PRIVATE_DATA *private_data = platform_get_drvdata(pdev);
	if(private_data->id == 0) {
		if(is_enable) {
			if(rtk_otg_is_via_hub_port()) {
				rtd_outl(USB2_TOP_USB_OTG_REG_reg, 0x0);
    			rtd_outl(USB2_OTG_2_USB_OTG_REG_reg, USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1));	
			}
			else {
				rtd_outl(USB2_TOP_USB_OTG_REG_reg, USB2_TOP_USB_OTG_REG_otg_enable(0x1));
				rtd_outl(USB2_OTG_2_USB_OTG_REG_reg, 0x0);
			}
		} else {
			rtd_outl(USB2_TOP_USB_OTG_REG_reg, 0x0);
			rtd_outl(USB2_OTG_2_USB_OTG_REG_reg, rtd_inl(USB2_OTG_2_USB_OTG_REG_reg) & (~(USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1))));	
		}
	} else {
		if(is_enable) {
			rtd_outl(USB2_OTG_USB_OTG_REG_reg, USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1));
		} else {
			rtd_outl(USB2_OTG_USB_OTG_REG_reg, rtd_inl(USB2_OTG_USB_OTG_REG_reg) & (~(USB2_OTG_USB_OTG_REG_otg_hub_enable(0x1))));	
		}
	}
	private_data->is_enable = is_enable;
}
