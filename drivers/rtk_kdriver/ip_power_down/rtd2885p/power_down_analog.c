#include <rbus/stb_reg.h>
#include <rbus/emcu_reg.h>
#include <rtk_kdriver/io.h>
#include <linux/delay.h>
#include <rtd_log/rtd_module_log.h>
#if defined ScalerTimer_DelayXms
#undef ScalerTimer_DelayXms
#endif 
#define ScalerTimer_DelayXms(a) mdelay(a)

void ac_on_power_down_analog(void)
{
	rtd_pr_power_crit("Enter : %s %d \n", __FILE__, __LINE__);
#include "Merlin8_USB_Power_down.tbl"
#include "Merlin8_audiobb_earc_power_down.tbl"
	rtd_pr_power_crit("Exit : %s %d \n", __FILE__, __LINE__);
}
EXPORT_SYMBOL(ac_on_power_down_analog);
