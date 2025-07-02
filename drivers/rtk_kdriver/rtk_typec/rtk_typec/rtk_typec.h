#ifndef __RTK_TYPEC_H__
#define __RTK_TYPEC_H__
#include <linux/i2c.h>
#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/usb/typec.h>

//****************************************************************************
// TYPEC REGISTER DEFINITIONS
//****************************************************************************
#define P66_0C_TPC_CC_CTRL          0xb8064430
#define P66_30_CC_HW_MODE_CTRL      0xb80644c0
#define ALT_MODE_CHECK_REGISTER     0xb80644c8
#define P66_33_CC_HW_FSM            0xb80644cc
#define P7F_02_10B_ADC_OUTPUT       0xb8065008
#define P7F_10_10B_ADC_A0_RSLT_H    0xb8065040
#define P7F_11_10B_ADC_A0_RSLT_L    0xb8065044

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
struct rtk_typec_info_rx_identity_reg {
	u8 status;
	struct usb_pd_identity identity;
	u32 vdo[3];
} __packed;


struct rtk_typec_info {
	struct device *dev;
	struct mutex lock; /* device lock */
	struct typec_capability  caps;
	struct typec_port *port;
	struct typec_partner *partner;
	struct usb_pd_identity partner_identity;
	struct usb_pd_identity cable_identity;
	int id;
	unsigned int last_connect_status;
};

#endif
