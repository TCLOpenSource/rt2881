#
#

#subdir-ccflags-y +=  -I../include

## HDMITX 2.0
obj-y += ScalerHdmiMacTx0 ScalerHdmiMacTx ScalerHdmiMacTx0_EXINT0 ScalerHdmiMacTx0FillPacket

## HDMITX 2.1
obj-y += ScalerHdmi21MacTx ScalerHdmi21MacTx0 ScalerHdmi21MacTx0FillPacket ScalerHdmi21TxLinkTraining

## TX packet bypass control
obj-y += ScalerHdmiMacTx0PktBypass

## Scaler
obj-y += ScalerTx_ctrl ScalerTx_init ScalerTx

obj-y += hdmitx_driver hdmitx_init hdmitx_ctrl hdmitx_lib Core_qevent_hdmitx_handler

obj-y += hdmitx

obj-y += hdmitx_vbedev
obj-$(CONFIG_HDMITX_TX_MENU_TEST_SUPPORT) += hdmitx_menuTest
include $(RULES_MAKES)

clean:
	-rm -f src/*.o src/*.a src/*~
