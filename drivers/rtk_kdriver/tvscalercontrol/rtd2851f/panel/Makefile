#
# Makefile for the Realtek PANEL drivers.
#
obj-y := panelapi.o panel_himax_romcode.o panel_dlg.o panel_dlg_customer.o hsr_parse.o

ifeq (y,$(filter y, $(CONFIG_CUSTOMER_TV010)))
obj-y += panelapi_extern_tv010.o
endif
