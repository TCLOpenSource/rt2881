ifeq ($(CONFIG_RTK_KDRV_DEMOD), y)

KBUILD_CPPFLAGS  += -I$(srctree)/drivers/rtk_kdriver/japan4k_dmx/$(CHIP_NAME)
KBUILD_CPPFLAGS  += -I$(srctree)/drivers/rtk_kdriver/demod/demod_ext
KBUILD_CPPFLAGS  += -I$(srctree)/drivers/rtk_kdriver/demod/demod_ext/demodcomm

KBUILD_CPPFLAGS  += -I$(srctree)/drivers/rtk_kdriver/common/include/rbus
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2856Family_refcode/
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2856Family_refcode/i2c
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2856Family_refcode/isdb_terr
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2856Family_refcode/isdb_sat
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/i2c
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/isdb_terr
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/isdb_sat

extdemod-objs := ExtDemodMgr.o \
                 demod_ext_debug.o \
                 demod_ext.o \
                 demod_ext_all.o \
		 demod_ext_common.o \
		 demodcomm/foundation.o \
		 demodcomm/comm.o \
                 demodcomm/tuner.o \
		 demodcomm/tv_osal.o \
                 demod_rtk_s_isdbt_cxd2856.o \
                 demod_rtk_s_isdbt_cxd2878.o \
                 demod_rtk_s_isdbs_cxd2856.o \
                 demod_rtk_s_isdbs_cxd2878.o \
                 demod_rtk_s_isdbs3_cxd2878.o \
                                     					
obj-y += extdemod.o 
obj-y += CXD2856Family_refcode/
obj-y += CXD2878Family_refcode/

endif
