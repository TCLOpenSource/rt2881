#ifndef __RTK_OTG_PLAT_WRAPPER_H__
#define __RTK_OTG_PLAT_WRAPPER_H__

int rtk_otg_initial(struct platform_device *pdev);
void rtk_otg_crt_on_off(bool on);
void rtk_otg_plat_set_global_interrupts(struct platform_device *pdev, bool on);
void rtk_otg_plat_set_local_interrupts(struct platform_device *pdev, bool on);
char *rtk_otg_plat_get_platform_device_name(struct platform_device *pdev);
void rtk_otg_plat_enable_otg(struct platform_device *pdev, bool is_enable);
#endif