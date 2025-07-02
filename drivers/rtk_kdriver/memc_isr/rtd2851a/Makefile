# Makefile for the PQ ISR.
ifneq ($(PATCHLEVEL),15)
KBUILD_CPPFLAGS += -I drivers/rtk_kdriver/memc_isr/$(CHIP_NAME)
obj-y := scalerMEMC.o Common/ Driver/ HAL/ MID/ Platform/ PQL/
endif
KBUILD_CFLAGS += -Werror
