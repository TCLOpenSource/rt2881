#
# Makefile for the Realtek HDMI drivers.
#
KBUILD_CFLAGS += -Wall -Werror
obj-y += hdmi_debug.o
ifeq (y,$(filter y, $(CONFIG_RTK_KDRV_HDMI_ENABLE_PHY)))
obj-y += hdmi_phy_dfe.o
obj-y += hdmi_phy.o
obj-y += hdmi_phy_2p1.o
endif
obj-y += hdmi_reg.o
obj-y += hdmi_drvif.o
obj-y += hdmiHdmi.o
#obj-y += hdmiMS.o
obj-y += hdmi_mac.o
obj-y += hdmi_info_packet.o
obj-y += hdmi_emp.o
obj-y += hdmi_ddc.o
obj-y += hdmi_cec.o
obj-y += hdmi_hdcp.o
obj-y += hdmi_hdcp2p2.o 
obj-y += hdmi_scdc.o
obj-y += hdmi_audio.o
obj-y += hdmi_hdr.o
obj-y += hdmi_dsc.o
obj-y += hdmi_arc.o
obj-y += hdmi_table.o
obj-y += hdmi_power_saving.o

ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_HDMI21
obj-y += hdmi_mac_2p1.o
endif

# Video frontend interface
obj-y += hdmi_vfe.o 
obj-y += hdmi_vfe_config.o 
obj-y += hdmi_edid.o
obj-y += cea_861.o

# Sys fs
obj-y += hdmi-sysfs.o

ifdef CONFIG_RTK_KDRV_HDMI_HDCP_REPEATER_ENABLE
obj-y += hdmi_hdcp_repeater.o
obj-y += hdcp14_repeater/
endif

# HDMI RX Built-In test
obj-$(CONFIG_HDMI_RX_BUILT_IN_TEST) += bist/
