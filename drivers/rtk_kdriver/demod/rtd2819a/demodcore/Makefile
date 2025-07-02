KBUILD_CPPFLAGS  += -I$(srctree)/drivers/rtk_kdriver/demod
KBUILD_CPPFLAGS  += -I$(srctree)/drivers/rtk_kdriver/demod/$(CHIP_NAME)/demodcomm

demodcore-objs := foundation.o \
					math_mpi.o \
					atsc_demod_rtk_r_base.o \
					atsc_demod_rtk_r.o \
					atsc3_demod_rtk_r.o \
          atsc3_demod_rtk_r_base.o \
					isdbt_demod_rtk_r_base.o \
					isdbt_demod_rtk_r.o \
					qam_demod_rtk_r_base.o \
					qam_demod_rtk_r.o \
					ifd_new.o \
					ifd_agc.o \
					ifd_cr.o \
					ifd_video.o \
					ifd_table.o \
					ifd_analog.o \

obj-y += demodcore.o 



