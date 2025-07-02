#include <linux/kernel.h>
#include <rtk_kdriver/io.h>
#include "rtk_typec.h"

#define RTK_TYPEC_0_TYPEC_CONNECT_STATUS_REG 0xB8060148
#define RTK_TYPEC_0_TYPEC_ALT_MODE_STATUS_REG 0xB80644C8


unsigned int rtk_typec_get_connect_status(unsigned int id)
{
	unsigned int connect_status = 0;
	switch(id) {
	case 0:
		connect_status = rtd_inl(RTK_TYPEC_0_TYPEC_CONNECT_STATUS_REG); 
		break;
	default:
		break;
	}
	return connect_status;
}

unsigned int rtk_typec_get_alt_mode_status(unsigned int id)
{
	unsigned int alt_mode_status = 0;
	switch(id) {
	case 0:
		alt_mode_status = rtd_inl(RTK_TYPEC_0_TYPEC_ALT_MODE_STATUS_REG); 
		break;
	default:
		break;
	}
	return alt_mode_status;
}