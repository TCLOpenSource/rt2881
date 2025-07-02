#ifndef __RTK_TYPEC_INTF_H__
#define __RTK_TYPEC_INTF_H__

#define TYPEC_CONNECT_STATUS_PLUG_PRESENT(s) 			(!!((s) & BIT(0)))
#define TYPEC_CONNECT_STATUS_PORTROLE(s)				(!!((s) & BIT(1)))
#define TYPEC_CONNECT_STATUS_DATAROLE(s)				(!!((s) & BIT(2)))
#define TYPEC_CONNECT_STATUS_VCONNROLE(s)				(!!((s) & BIT(3)))
#define TYPEC_CONNECT_STATUS_POWER_OPMODE(s)		((((s) >> 4)& 0x3))

unsigned int rtk_typec_get_connect_status(int id);

#define TYPEC_ALT_MODE_STATUS_ORIENTATION(s)    (((s)& 0x1))

unsigned int rtk_typec_get_alt_mode_status(unsigned int id);
#endif