KBUILD_CPPFLAGS += -I ./
KBUILD_CPPFLAGS += -I ../include/


rtk-video_common-objs := $(SRC)/rtk_video_common-priv.o
rtk-video_common-objs += $(SRC)/rtk_video_common_drvif.o
rtk-video_common-objs += $(SRC)/rtk_video_common.o
rtk-video_common-objs += $(SRC)/rtk_video_common_module.o
rtk-video_common-objs += $(SRC)/rtk_video_common_sysfs.o

obj-$(CONFIG_RTK_VIDEO_COMMON) += rtk-video_common.o