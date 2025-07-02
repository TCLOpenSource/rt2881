#ifndef __XHCI_PLATFORM_DRD_HW_WRAPPER_H__
#define __XHCI_PLATFORM_DRD_HW_WRAPPER_H__
#include "xhci-plat-config.h"
#define USB_PHY_DELAY    25
#define CONFIG_XHCI_DRD_ONLY_MODE
#define DWC3_GLOBALS_REGS_START         0xC100
#define DWC3_WRAP_REGS_OFST          0x39000
#define CONFIG_XHCI_USING_SPECIAL_PHY_SETTING_FLOW
int rtk_xhci_drd_mac_init(struct platform_device *pdev);
int rtk_xhci_drd_phy_init_post(struct platform_device *pdev);
int rtk_xhci_drd_mac_postinit(struct platform_device *pdev);
void rtk_xhci_set_global_interrupts(bool on);
#endif
