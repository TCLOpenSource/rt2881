KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/demodcomm
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/i2c
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/lib
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/isdb_terr
KBUILD_CPPFLAGS  += -I drivers/rtk_kdriver/demod/demod_ext/CXD2878Family_refcode/isdb_sat

CXD2878Family_refcode-objs := sony_demod.o \
sony_common.o \
sony_stopwatch_port.o \
sony_integ.o \
					
obj-y += CXD2878Family_refcode.o 
obj-y += i2c/
obj-y += lib/
obj-y += isdb_terr/
obj-y += isdb_sat/
