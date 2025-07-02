#
#

## HDMITX LIB
obj-y += hdmitx_Convert hdmitx_lib_ex hdmitx_lunchbank6_ex hdmitx_lunchbank8_ex hdmitx_lunchbank9_ex hdmitx_lunchbank38_ex
obj-$(CONFIG_HDMITX_TX_SCRIPT_SUPPORT) += hdmitx_script

include $(RULES_MAKES)

clean:
	-rm -f lib/*.o lib/*.a lib/*~
