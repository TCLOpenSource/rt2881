#
# Makefile for the Realtek HDMI drivers.
#
KBUILD_CFLAGS += -Wall -Werror

ifeq ($(CHIP_NAME), rtd2885p)
ifeq (y,$(filter y, $(CONFIG_RTK_KDRV_HDMI_ENABLE_PHY)))
obj-y += $(CHIP_NAME)/hdmi_phy_dfe.o
obj-y += $(CHIP_NAME)/hdmi_phy.o
obj-y += $(CHIP_NAME)/hdmi_phy_2p1.o
obj-y += $(CHIP_NAME)/hdmi_reg.o
obj-y += $(CHIP_NAME)/hdmi_mac.o
obj-y += $(CHIP_NAME)/hdmi_table.o
obj-y += $(CHIP_NAME)/hdmi_power_saving.o
endif
ifdef CONFIG_RTK_KDRV_HDMI_ENABLE_HDMI21
obj-y += $(CHIP_NAME)/hdmi_mac_2p1.o
endif
endif

obj-y += common/hdmi_debug.o
obj-y += common/hdmi_drvif.o
obj-y += common/hdmiHdmi.o
obj-y += common/hdmi_info_packet.o
obj-y += common/hdmi_emp.o
obj-y += common/hdmi_ddc.o
obj-y += common/hdmi_cec.o
obj-y += common/hdmi_hdcp.o
obj-y += common/hdmi_hdcp2p2.o
obj-y += common/hdmi_scdc.o
obj-y += common/hdmi_audio.o
obj-y += common/hdmi_hdr.o
obj-y += common/hdmi_dsc.o
obj-y += common/hdmi_arc.o




# Video frontend interface
obj-y += common/hdmi_vfe.o
obj-y += common/hdmi_vfe_config.o
obj-y += common/hdmi_edid.o
obj-y += common/cea_861.o

# Sys fs
obj-y += common/hdmi-sysfs.o

ifdef CONFIG_RTK_KDRV_HDMI_HDCP_REPEATER_ENABLE
obj-y += hdmi_hdcp_repeater.o
obj-y += hdcp14_repeater/
endif

# HDMI RX Built-In test
obj-$(CONFIG_HDMI_RX_BUILT_IN_TEST) += bist/
