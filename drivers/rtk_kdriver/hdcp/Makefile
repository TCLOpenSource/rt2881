# Makefile for the Realtek HDCP OPTEE drivers.
#
#KBUILD_CFLAGS += -Wall -Werror
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/include/rtk_kdriver
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver

obj-$(CONFIG_RTK_HDCP_OPTEE) += hdcp_optee.o
hdcp_optee-objs += hdcp_optee_drv.o

ifneq (n,$(filter n, $(CONFIG_OPTEE_HDCP2)))
hdcp_optee-objs += hdcp2_optee.o
endif

ifneq (n,$(filter n, $(CONFIG_OPTEE_HDCP14)))
hdcp_optee-objs += hdcp14_optee.o
endif

ifeq ($(CONFIG_HDCPTX),y)
hdcp_optee-objs += hdcptx/hdcp2tx_hdmi.o \
                   hdcptx/hdcp2tx_common.o \
                   hdcptx/hdcptx_init.o \
                   hdcptx/hdcp2tx_sys.o \
                   hdcptx/hdcptx_common.o \
                   hdcptx/hdcp_cal.o \
                   hdcptx/hdcp14tx_hdmi.o \
                   hdcptx/hdcp14tx_sys.o

ifeq (y,$(filter y, $(CONFIG_OPTEE_HDCP2)))
hdcp_optee-objs += hdcp2_2/core/crypto_spu.o \
                   hdcp2_2/core/secure_flash.o \
                   hdcp2_2/core/bigdigits.o \
                   hdcp2_2/core/crypto.o \
                   hdcp2_2/core/hmac.o \
                   hdcp2_2/core/hdcp2_rcp_api.o \
                   hdcp2_2/core/sha2.o
endif
endif
