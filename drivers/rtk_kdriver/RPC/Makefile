#KBUILD_CFLAGS += -Werror

KBUILD_CPPFLAGS += -I drivers/rtk_kdriver/include/
KBUILD_CPPFLAGS += -I drivers/rtk_kdriver/common/include/
KBUILD_CPPFLAGS += -I drivers/rtk_kdriver/common/IPC/generate/include/system
KBUILD_CPPFLAGS += -I drivers/rtk_kdriver/common/IPC/include
KBUILD_CPPFLAGS += -I drivers/rtk_kdriver/include/rtk_kdriver
KBUILD_CPPFLAGS += -I drivers/rtk_kdriver

obj-$(CONFIG_RTK_KDRV_RPC) += rtk-rpc.o
rtk-rpc-objs = RPCDriver.o RPCintr.o RPCpoll.o RPCkern.o RPClog.o ../misc/$(CHIP_NAME)/rtk_avcpu.o

