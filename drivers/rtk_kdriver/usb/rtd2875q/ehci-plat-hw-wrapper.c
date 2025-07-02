#include <linux/acpi.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/timer.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/usb/ehci_pdriver.h>
#include <linux/completion.h>
#include <asm/delay.h>
#include <linux/mutex.h>
#include <rbus/usb2_top_reg.h>
#include <rbus/usb2_ex_top_reg.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/rtk_crt.h>
#include <rtk_kdriver/rtk_gpio.h>
#include "ehci-plat.h"

#define USB_PHY_DELAY                   50
#define USB_MAC_DELAY                           100

static DEFINE_MUTEX(crt_mutex);
static bool usb2_crt_power_on_done = false;

bool is_usb2_on(void)
{
    bool is_mac_clk_on = false, is_mac_release = false, is_phy_release = false;
    unsigned clk = 0, mac_rst = 0, phy_rst = 0;
    

    // clock: USB MAC
    clk = rtd_inl(SYS_REG_SYS_CLKEN2_reg);
    if ((clk & SYS_REG_SYS_CLKEN2_clken_usb2_dwc_mask)
	 && (clk & SYS_REG_SYS_CLKEN2_clken_usb2_ex_dwc_mask)
	 && (clk & SYS_REG_SYS_CLKEN2_clken_usb2_wrapper_mask)
	 && (clk & SYS_REG_SYS_CLKEN2_clken_usb2_ex_wrapper_mask)
	 && (rtd_inl(SYS_REG_SYS_CLKEN5_reg) & SYS_REG_SYS_CLKEN5_clken_usb_ex_rbus_mask))
        is_mac_clk_on = true;


    // reset: USB MAC
    mac_rst = rtd_inl(SYS_REG_SYS_SRST2_reg);
    if ((mac_rst & SYS_REG_SYS_SRST2_rstn_usb2_dwc_mask)
		&& (mac_rst & SYS_REG_SYS_SRST2_rstn_usb2_ex_dwc_mask)
		&& (mac_rst & SYS_REG_SYS_SRST2_rstn_usb2_wrapper_mask)
		&& (mac_rst & SYS_REG_SYS_SRST2_rstn_usb2_ex_wrapper_mask)
		&& (rtd_inl(SYS_REG_SYS_SRST5_reg) & SYS_REG_SYS_SRST5_rstn_usb_ex_rbus_mask))
        is_mac_release = true;

    // reset: USB PHY
    phy_rst = rtd_inl(SYS_REG_SYS_SRST3_reg);
    if ((phy_rst & SYS_REG_SYS_SRST3_rstn_usb2_phy_p2_mask) &&
            (phy_rst & SYS_REG_SYS_SRST3_rstn_usb2_phy_p1_mask) &&
             (phy_rst & SYS_REG_SYS_SRST3_rstn_usb2_phy_p0_mask))
        is_phy_release = true;

    return (is_mac_clk_on && is_mac_release && is_phy_release);
}
EXPORT_SYMBOL(is_usb2_on);


void usb2_crt_on(void)
{
        mutex_lock(&crt_mutex);

        if (!usb2_crt_power_on_done || !is_usb2_on())
        {
                CRT_CLK_OnOff(USB, CLK_ON, (void *)2);
                
                //keep pll alive
                rtd_setbits(USB2_TOP_USB_CTR0_reg,
                            USB2_TOP_USB_CTR0_pll_alive_enable_mask);
                rtd_setbits(USB2_EX_TOP_USB_CTR0_reg,
                            USB2_EX_TOP_USB_CTR0_pll_alive_enable_mask);				

                usb2_crt_power_on_done = true;
        }

        mutex_unlock(&crt_mutex);
}
EXPORT_SYMBOL(usb2_crt_on);


void usb2_crt_off(void)
{
        mutex_lock(&crt_mutex);
        usb2_crt_power_on_done = false;
        CRT_CLK_OnOff(USB, CLK_OFF, (void *)2);
        mutex_unlock(&crt_mutex);
}
EXPORT_SYMBOL(usb2_crt_off);

static int keep_pll_alive(struct platform_device *pdev)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);

        rtd_setbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CTR0_REG, USB2_TOP_USB_CTR0_pll_alive_enable_mask);

        return 0;
}

int ehci_usb_mac_init(struct platform_device *pdev)
{
        // In past experience, we will encounter rbus timeout when access oHCI regs if we dont set this.
        return keep_pll_alive(pdev);
}



static int ehci_less_one_sram_read(struct platform_device *pdev, bool enable)
{
#ifndef CONFIG_RTK_8KCODEC
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
        if (enable)
                rtd_setbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CTR0_REG, USB2_TOP_USB_CTR0_wrap_sram_rd_opt_mask);
        else
                rtd_clearbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CTR0_REG, USB2_TOP_USB_CTR0_wrap_sram_rd_opt_mask);
        return 0;
#else  // [H5XBU-698] fix send data fail when buffer is non-word aligned
        return 0;
#endif
}

static int ehci_utmi_release(struct platform_device *pdev)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
        unsigned int i = 0;
        unsigned long reset_utmi_regs[3] = {priv->wrapper_regs.EHCI_USB2_TOP_RESET_UTMI_REG,
                                            priv->wrapper_regs.EHCI_USB2_TOP_RESET_UTMI_2PORT_REG,
                                            priv->wrapper_regs.EHCI_USB2_TOP_RESET_UTMI_3PORT_REG
                                           };
        if(priv->usb2_port_cnt > 3)
                return -ENODEV;

        for(i  = 0; i < priv->usb2_port_cnt; i++)
                rtd_clearbits(reset_utmi_regs[i], USB2_TOP_RESET_UTMI_reset_utmi_p0_mask);
        udelay(USB_MAC_DELAY);
        return 0;
}

static int ehci_wrapper_interrupt_enable(struct platform_device *pdev, bool en)
{
        struct usb_hcd *hcd = platform_get_drvdata(pdev);
        struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
        if(en)
                rtd_setbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CTR0_REG, USB2_TOP_USB_CTR0_usb_top_int_en_mask);
        else
                rtd_clearbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CTR0_REG, USB2_TOP_USB_CTR0_usb_top_int_en_mask);
        return 0;
}


int ehci_usb_mac_post_init(struct platform_device *pdev, bool in_resume)
{
        int ret = 0;

        // Keep false to prevent data alignment issue that transfers size more than 258 bytes
        ret = ehci_less_one_sram_read(pdev, false);
        if (ret)
                goto out;

        ret = ehci_utmi_release(pdev);
        if (ret)
                goto out;

        ret = ehci_wrapper_interrupt_enable(pdev, true);
        if (ret)
                goto out;
        if(in_resume)
                msleep(100);
        return 0;
out:
        return ret;
}

/*********************************EHCI CRC Releated*****************************************************/

int get_usb2_wrap_crc_en(struct ehci_hcd *ehci)
{
        struct ehci_platform_priv *priv = ehci_to_ehci_priv(ehci);
        return USB2_TOP_USB_CRC_CNT_get_rx_crc_cnt_en(rtd_inl(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG));
}

int get_usb2_wrap_crc_rst(struct ehci_hcd *ehci)
{
        struct ehci_platform_priv *priv = ehci_to_ehci_priv(ehci);
        return USB2_TOP_USB_CRC_CNT_get_rx_crc_cnt_rst(rtd_inl(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG));
}


int enble_usb2_wrap_crc(struct ehci_hcd *ehci, bool on)
{
        struct ehci_platform_priv *priv = ehci_to_ehci_priv(ehci);
        if (on)
                rtd_setbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG, USB2_TOP_USB_CRC_CNT_rx_crc_cnt_en_mask);
        else
                rtd_clearbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG, USB2_TOP_USB_CRC_CNT_rx_crc_cnt_en_mask);
        return 0;
}


int reset_usb2_wrap_crc(struct ehci_hcd *ehci)
{
        struct ehci_platform_priv *priv = ehci_to_ehci_priv(ehci);
        rtd_setbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG, USB2_TOP_USB_CRC_CNT_rx_crc_cnt_rst_mask);
        udelay(10);
        rtd_clearbits(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG, USB2_TOP_USB_CRC_CNT_rx_crc_cnt_rst_mask);
        return 0;
}


int get_usb2_wrap_crc_err_cnt(struct ehci_hcd *ehci, int port1)
{
        struct ehci_platform_priv *priv = ehci_to_ehci_priv(ehci);
        int crc_cnt = -1;
        unsigned int data = 0;

        if(port1 < 1 || port1 > priv->usb2_port_cnt)
                return -1;

        if (!USB2_TOP_USB_CRC_CNT_get_rx_crc_cnt_en(rtd_inl(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG)))
                return -1;

        data = rtd_inl(priv->wrapper_regs.EHCI_USB2_TOP_USB_CRC_CNT_REG);
        crc_cnt = ((data >> (USB2_TOP_USB_CRC_CNT_rx_crc_cnt_port0_shift + ((port1 - 1) * 10))) & 0x3FF);
        return crc_cnt;
}

int ehci_register_usb_bus_nb(struct platform_device *pdev)
{
	return 0;
}

void ehci_unregister_usb_bus_nb(struct platform_device *pdev)
{

}

int ehci_register_thermal_callback(struct platform_device *pdev)
{
	return 0;
}

void ehci_unregister_thermal_callback(struct platform_device *pdev)
{

}

/**********************************EOS**************************************************/
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/freezer.h>

struct EHCI_EOS_POLL_CONTEXT
{
	struct task_struct *task;
};
struct EHCI_EOS_POLL_CONTEXT g_eos_poll_contexts[2];

static int ehci_usb2_eos_check_b5(struct ehci_hcd *ehci, int port)
{
	int i  =0;
	unsigned int count = 0;
	for(i = 0; i < 5; i++) {
		int val = get_ehci_usb2_phy_reg(ehci, port, 0, 0xb5);
		count +=  (!!(val & 0x80));
	}
	if(count == 5)
		return 1;
	else if(count == 0)
		return 0;
	else
		return 2;
}

static void ehci_usb2_set_phy_io_eos(struct ehci_hcd *ehci, int port, unsigned char val)
{
	//ehci_plat_info("eos controller %d port %d get 0xb5 = %x\n", index, port, val);
	if(val == 1) {
		val = get_ehci_usb2_phy_reg(ehci, port, 1, 0xE1);
		val = (val & (~0x3));
		set_ehci_usb2_phy_reg(ehci, port, 1, 0xE1, val);
	} else  if(val == 0) {
		val = get_ehci_usb2_phy_reg(ehci, port, 1, 0xE1);
			
		val = (val & (~0x3));
		set_ehci_usb2_phy_reg(ehci, port, 1, 0xE1, val);

		val = (val | 0x1);
		set_ehci_usb2_phy_reg(ehci, port, 1, 0xE1, val);
				
	}
}
#if 0
static int ehci_usb2_get_port_connection_status(struct ehci_hcd *ehci, int port_num)
{
	struct usb_hcd *hcd = ehci_to_hcd(ehci);
	struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
	unsigned int ehci_port_reg = priv->reg_start + 0xA0000000 + 0x54 + (port_num -1) * 4;
	unsigned int ohci_port_reg = priv->reg_start + 0xA0000000 + 0x454 + (port_num -1) * 4;
	unsigned int ehci_port_status = rtd_inl(ehci_port_reg);
	unsigned int ohci_port_status = rtd_inl(ohci_port_reg);
	//ehci_plat_info("eos controller %d port %d[%x = %x, %x = %x]\n", priv->id, port_num,
	//			ehci_port_reg, ehci_port_status, ohci_port_reg, ohci_port_status);
	if((ehci_port_status & 0x1)
		|| (ohci_port_status == 0x103)
		|| (ohci_port_status == 0x303))
		return 1;
	return 0;
}
#endif
int _ehci_eos_poll_process(void *arg)
{
	struct device *dev = (struct device *)arg;
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
	int last_status_array[8];
	int port_cnt = (priv->usb2_port_cnt > 8) ? 8 : priv->usb2_port_cnt;
	
	memset(last_status_array,  0xFF, sizeof(last_status_array));
	while(!kthread_should_stop()) {
		int port_num = 0;
		for(port_num = 0; port_num < port_cnt; port_num++) {
			int val = ehci_usb2_eos_check_b5(ehci, port_num + 1);
			//ehci_plat_info("eos controller %d port %d get 0xb5 = %x\n", priv->id, port_num + 1, val);
			if(last_status_array[port_num] != val) {
				ehci_usb2_set_phy_io_eos(ehci, port_num + 1, val);
				last_status_array[port_num] = val;
			}
		}
		msleep(500);
	}
	return 0;
}

int ehci_post_init_callback(struct platform_device *pdev)
{
	int port_num = 0;
	struct device *dev = &pdev->dev;
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
	if(priv->id >= 2 || priv->id < 0) {
		ehci_plat_err("%s id %d out of bound\n", __FUNCTION__, priv->id);
		return 0;
	}
	for(port_num = 0; port_num < priv->usb2_port_cnt; port_num++) {
		int val = ehci_usb2_eos_check_b5(ehci, port_num + 1);
		ehci_plat_info("eos controller %d port %d get 0xb5 = %x\n", priv->id, port_num + 1, val);
		ehci_usb2_set_phy_io_eos(ehci, port_num + 1, val);
	}
	
	g_eos_poll_contexts[priv->id].task= kthread_create(_ehci_eos_poll_process, &pdev->dev, "eos_poll%d", priv->id);
	if(IS_ERR(g_eos_poll_contexts[priv->id].task)) {
		ehci_plat_err("%s Create Eos Poll Thread fail \n", __FUNCTION__);
		g_eos_poll_contexts[priv->id].task = NULL;
		return 0;
	}
	if(g_eos_poll_contexts[priv->id].task)
    		wake_up_process(g_eos_poll_contexts[priv->id].task);	
	ehci_plat_info("%s id %d eos poll thread success\n", __FUNCTION__, priv->id);
	return 0;
}

void ehci_pre_exit_callback(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
	if(priv->id >= 2 || priv->id < 0) {
		ehci_plat_err("%s id %d out of bound\n", __FUNCTION__, priv->id);
		return ;
	}
	if(g_eos_poll_contexts[priv->id].task) {
		kthread_stop(g_eos_poll_contexts[priv->id].task);
		g_eos_poll_contexts[priv->id].task = NULL;
		ehci_plat_info("%s id %d eos poll thread stopped\n", __FUNCTION__, priv->id);
	}	
}

void ehci_pre_suspend_callback(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
	if(priv->id >= 2 || priv->id < 0) {
		ehci_plat_err("%s id %d out of bound\n", __FUNCTION__, priv->id);
		return ;
	}
	if(g_eos_poll_contexts[priv->id].task) {
		kthread_stop(g_eos_poll_contexts[priv->id].task);
		g_eos_poll_contexts[priv->id].task = NULL;
		ehci_plat_info("%s id %d eos poll thread stopped\n", __FUNCTION__, priv->id);
	}
}

int ehci_post_resume_callback(struct platform_device *pdev)
{
	int port_num = 0;
	struct device *dev = &pdev->dev;
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	struct ehci_platform_priv *priv = hcd_to_ehci_priv(hcd);
	if(priv->id >= 2 || priv->id < 0) {
		ehci_plat_err("%s id %d out of bound\n", __FUNCTION__, priv->id);
		return 0;
	}
	for(port_num = 0; port_num < priv->usb2_port_cnt; port_num++) {
		int val = ehci_usb2_eos_check_b5(ehci, port_num + 1);
		ehci_plat_info("eos controller %d port %d get 0xb5 = %x\n", priv->id, port_num + 1, val);
		ehci_usb2_set_phy_io_eos(ehci, port_num + 1, val);
	}
	
	g_eos_poll_contexts[priv->id].task= kthread_create(_ehci_eos_poll_process, &pdev->dev, "eos_poll%d", priv->id);
	if(IS_ERR(g_eos_poll_contexts[priv->id].task)) {
		ehci_plat_err("%s Create Eos Poll Thread fail \n", __FUNCTION__);
		g_eos_poll_contexts[priv->id].task = NULL;
		return 0;
	}
	if(g_eos_poll_contexts[priv->id].task)
    		wake_up_process(g_eos_poll_contexts[priv->id].task);
	ehci_plat_info("%s id %d eos poll thread success\n", __FUNCTION__, priv->id);
	return 0;
}

void ehci_platform_post_reset(struct usb_hcd *hcd)
{

}
