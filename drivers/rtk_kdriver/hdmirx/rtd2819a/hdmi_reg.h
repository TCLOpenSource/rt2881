#define HD21_PORT HDMI_PORT0
#define HD20_PORT HDMI_PORT2

#ifndef uint32_t
typedef unsigned int __UINT32_T;
#define uint32_t __UINT32_T
#endif
#ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_PHY
#include "hdmi_reg_phy.h"   // for HDMI PHY
#include "hdmi_reg_phy_2p1.h"  //for HDMI2.1 PHY
#include "hdmi_reg_phy_dfe.h"   //for DFE PHY
#endif
#include "hdmi_reg_mac.h"   // for HDMI MAC

#include <rbus/pinmux_reg.h> // for ST 5VDET/HPD
#include <rbus/hdmi_hdmipll_reg.h>
#include <rbus/hdmi_p1_hdmipll_reg.h>
#include <rbus/hdmi_p2_hdmipll_reg.h>
#include <rbus/hdmi_p3_hdmipll_reg.h>
#include <rbus/hdmi_dprxpll_reg.h>
#include <rbus/hdmi_p1_dprxpll_reg.h>

#include <rbus/hdmi_port_common_reg.h>
#include <rbus/hdmi_port1_common_reg.h>
#include <rbus/hdmi_port2_common_reg.h>
#include <rbus/hdmi_port3_common_reg.h>

#include <rbus/hdmi_dp_common_reg.h>
#include <rbus/hdmi_dp_p1_common_reg.h>
#include <rbus/hdmi_dp_p2_common_reg.h>
#include <rbus/hdmi_dp_p3_common_reg.h>
#include <rbus/hdmi_top_common_reg.h>
