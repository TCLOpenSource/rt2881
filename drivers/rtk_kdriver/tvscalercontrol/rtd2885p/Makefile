#
# Makefile for the Realtek scaler drivers.
#
ifneq ($(CONFIG_BYPASSMEMC_FOR_CODESIZE_SAVING),y)
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/memc_isr/$(CHIP_NAME)
endif

HDCP_PATH = ../../hdcp/

HDMIRX_PATH = ../../hdmirx/
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/hdmirx/$(CHIP_NAME)
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/hdmirx/common
KBUILD_CPPFLAGS += -I$(srctree)/drivers/rtk_kdriver/hdmirx/rtd7032

tv_scaler_drv-objs = scaler_vfedev.o \
                     scaler_vscdev.o \
                     scaler_vbedev.o \
                     scaler_vpqdev.o \
                     scaler_vpqdev_unittest.o \
                     scaler_vpqleddev.o \
                     scaler_vtdev.o \
                     scaler_vpqmemcdev.o \
                     ../common/scaler_svp.o

tv_scaler_drv-objs += $(HDCP_PATH)/scaler_hdcp2dev.o

ifeq (y,$(filter y, $(CONFIG_SUPPORT_SRC_DPRX)))
tv_scaler_drv-objs += scaler_vfedev-dprx.o
endif

#io/
tv_scaler_drv-objs += io/ioregdrv.o

#i3ddma/
tv_scaler_drv-objs += i3ddma/i3ddma.o \
                      i3ddma/i3ddmacolorfmtconvert.o \
                      i3ddma/i3ddma_drv.o \
                      i3ddma/i3ddma_hsd.o
					  




#vip/
tv_scaler_drv-objs += vip/ultrazoom.o \
                      vip/di_ma.o \
                      vip/scalerColor.o \
                      vip/dcc.o \
                      vip/color.o \
                      vip/film.o \
                      vip/icm.o \
                      vip/intra.o \
                      vip/nr.o \
                      vip/custom_sharpness.o \
                      vip/xc.o \
                      vip/peaking.o \
                      vip/gibi_od.o \
                      vip/pq_misc_table.o \
                      vip/pq_adaptive.o \
                      vip/localdimming.o \
                      vip/vip_panel_default.o \
                      vip/viptable.o \
                      vip/scalerColor_tv006.o \
                      vip/scalerColor_tv043.o \
                      vip/localcontrast.o \
                      vip/memc_mux.o \
                      vip/pq_3dlut_table.o \
                      vip/pq_rpc.o \
                      vip/still_logo_detect.o \
                      vip/tc_hdr.o \
					  vip/pcid.o \
                      vip/fcic.o \
                      vip/hdr_table.o \
                      vip/tc_math.o \
                      vip/tc_OEOTF_convert.o \
                      vip/module_test_table.o \
					  vip/valc.o \
                      vip/ST2094.o \
					  vip/ai_pq.o \
					  vip/srnn.o \
					  vip/srnn_table.o \
					  vip/frequency_detection.o \
					  vip/pdf.o \
					  vip/SIP.o \
					  vip/pqmask.o \
					  vip/dtm.o \
					  vip/GSR2.o \
					  vip/rgb_sh.o \
					  vip/cbrev.o \
					  vip/GSR_adapter.o \
					  vip/localdimmingInterface_table.o \
					  vip/localdimmingInterface.o \
					  vip/vip_ring_buffer.o \
					  vip/APLSatGain.o \
					  vip/HDR10_vivid.o \
					  vip/decoupling.o \

ifeq (y,$(filter y, $(CONFIG_RTK_PQMASK_DRV)))
tv_scaler_drv-objs += vip/scalerPQMaskCtrl.o \
					  vip/scalerPQMaskColorLib.o	
endif



ifeq (y,$(filter y, $(CONFIG_CUSTOMER_TV010)))
tv_scaler_drv-objs +=scaler_vpqdev_extern_tv010.o
else
tv_scaler_drv-objs +=scaler_vpqdev_extern.o scaler_vrdev.o
endif

tv_scaler_drv-objs += vdc/video.o \
                      vdc/video_table.o \
                      vdc/video_input.o \
                      vdc/yc_separation_vpq.o \
                      vdc/yc_separation_vpq_table.o \
                      vdc/video_factory.o \
                      vdc/video_sb2dbg.o \
                      vdc/video_debug.o

#adcSource/
ifeq (y,$(filter y, $(CONFIG_SUPPORT_SRC_ADC)))
ifeq (y,$(filter y, $(CONFIG_SUPPORT_SRC_VGA)))
tv_scaler_drv-objs += adcSource/vga.o
endif
tv_scaler_drv-objs += adcSource/ypbpr.o \
                      adcSource/adcctrl.o \
					  scaler_drv_verify/scalerdrv_verify_main.o \
					  scaler_drv_verify/scaler_verify_iptg.o \
					  scaler_drv_verify/scaler_capture_verify.o
endif

#panel/
tv_scaler_drv-objs += panel/panelapi.o \
				      panel/panel_dlg.o \
					  panel/panel_dlg_customer.o \
					  panel/hsr_parse.o \
                      panel/panel_himax_romcode.o		  
ifeq (y,$(filter y, $(CONFIG_CUSTOMER_TV010)))
tv_scaler_drv-objs += panel/panelapi_extern_tv010.o
endif	
			 

#scaler/
tv_scaler_drv-objs += scaler/ctrl/scalerstruct.o \
                      scaler/ctrl/scalercolorlib.o \
                      scaler/ctrl/scalerthread_rtice2ap.o \
                      scaler/ctrl/scalercolorlib_tv001.o \
                      scaler/ctrl/scalercolorlib_tv010.o \
                      scaler/ctrl/scalercolorlib_tv002.o \
					  scaler/ctrl/scalercolorlib_IPQ.o \
                      scaler/ctrl/scalercolor_engmenu.o
					  					  
#scalerdrv/
tv_scaler_drv-objs += scalerdrv/mode.o \
                      scalerdrv/framesync.o \
                      scalerdrv/power.o \
                      scalerdrv/scalerdrv.o \
                      scalerdrv/scalerclock.o \
                      scalerdrv/scalerdisplay.o \
                      scalerdrv/scalerip.o \
                      scalerdrv/scalermemory.o \
                      scalerdrv/nonlibdma_scalermemory.o \
					  scalerdrv/libdma_scalermemory.o \
                      scalerdrv/conversion_2d_3d.o \
                      scalerdrv/scaler_2dcvt3d_ctrl.o \
                      scalerdrv/adjust.o \
                      scalerdrv/inprocvideo.o \
                      scalerdrv/zoom_smoothtoggle.o \
                      scalerdrv/zoom_smoothtoggle_lib.o \
                      scalerdrv/pipmp.o \
                      scalerdrv/overscanTable.o \
                      scalerdrv/scaler_libdma.o \
                      scalerdrv/scaler_nonlibdma.o \
                      scalerdrv/fraction.o \
                      dtg/dtg.o


tv_scaler_drv-objs += scalerdrv/scaler_i2rnd.o
tv_scaler_drv-objs += scalerdrv/scaler_pst.o
tv_scaler_drv-objs += scalerdrv/scaler_hdr_rpc.o scalerdrv/scaler_hdr_ctrl.o
ifeq (y,$(filter y, $(CONFIG_SUPPORT_SRC_ADC)))
tv_scaler_drv-objs += scalerdrv/inprocvga.o \
                      scalerdrv/inprocypbpr.o \
					  scalerdrv/syncproc.o \
					  scalerdrv/auto.o
endif

#pcbsource
tv_scaler_drv-objs += pcbsource/pcbSource.o


#vdac/
tv_scaler_drv-objs += vdac/vdac.o

#avd/
tv_scaler_drv-objs += avd/avdctrl_vd.o \
                      avd/avdctrl.o
					  
#vbi/
tv_scaler_drv-objs += vbi/vbi_slicer_driver.o

#vo/
tv_scaler_drv-objs += vo/rtk_vo.o
					  
#vgip/
ifneq ($(CHIP_NAME),)
tv_scaler_drv-objs += ../../misc/$(CHIP_NAME)/rtk_vgip_isr.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerVIP.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerDCC.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerVideo.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerDI.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerVideoTable.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerAI.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerHist.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerAiDebug.o \
                      ../../vgip_isr/$(CHIP_NAME)/scalerVIPDOS.o


else
tv_scaler_drv-objs += ../misc/rtk_vgip_isr.o \
                      ../vgip_isr/scalerVIP.o \
                      ../vgip_isr/scalerDCC.o \
                      ../vgip_isr/scalerVideo.o \
                      ../vgip_isr/scalerDI.o \
                      ../vgip_isr/scalerAI.o \ 
					  ../vgip_isr/scalerHist.o \ 
					  ../vgip_isr/scalerAiDebug.o
endif


#ddomain/
ifneq ($(CHIP_NAME),)
tv_scaler_drv-objs += ../../misc/$(CHIP_NAME)/rtk_ddomain_isr.o
else
tv_scaler_drv-objs += ../misc/rtk_ddomain_isr.o
endif

#pif/
ifneq ($(CHIP_NAME),)
tv_scaler_drv-objs += ../../misc/$(CHIP_NAME)/rtk_pif_isr.o
else
tv_scaler_drv-objs += ../misc/rtk_pif_isr.o
endif


#memc/
ifneq ($(CONFIG_BYPASSMEMC_FOR_CODESIZE_SAVING),y)
ifneq ($(CHIP_NAME),)
tv_scaler_drv-objs += ../../misc/$(CHIP_NAME)/rtk_memc_isr.o \
					  ../../memc_isr/$(CHIP_NAME)/scalerMEMC.o \
					  ../../memc_isr/$(CHIP_NAME)/Common/kw_common.o \
					  ../../memc_isr/$(CHIP_NAME)/Common/kw_debug.o \
					  ../../memc_isr/$(CHIP_NAME)/Common/kw_uart.o \
					  ../../memc_isr/$(CHIP_NAME)/Driver/regio.o \
					  ../../memc_isr/$(CHIP_NAME)/Driver/uartinit.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_cpr.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_crtc.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_dehalo.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_dm.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_hdf.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_ip.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_mc.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_me.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_omet.o \
					  ../../memc_isr/$(CHIP_NAME)/HAL/hal_post.o \
					  ../../memc_isr/$(CHIP_NAME)/MID/mid_crtc.o \
					  ../../memc_isr/$(CHIP_NAME)/MID/mid_dm.o \
					  ../../memc_isr/$(CHIP_NAME)/MID/mid_memc.o \
					  ../../memc_isr/$(CHIP_NAME)/MID/mid_misc.o \
					  ../../memc_isr/$(CHIP_NAME)/MID/mid_mode.o \
					  ../../memc_isr/$(CHIP_NAME)/MID/mid_sys.o \
					  ../../memc_isr/$(CHIP_NAME)/Platform/memc_lib.o \
					  ../../memc_isr/$(CHIP_NAME)/Platform/memc_change_size.o \
					  ../../memc_isr/$(CHIP_NAME)/Platform/memc_identification.o \
					  ../../memc_isr/$(CHIP_NAME)/Platform/memc_identify_ctrl.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Dh_close.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FBlevelCtrl.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FilmDetcetCtrl.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FRC_decPhT_generate.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FRC_glb_Context.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FRC_LogoDetect.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FRC_phTable.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FRC_SceneAnalysis.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Mc_lbmcSwitch.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Patch_Manage.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PQL_context_ReadParam.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PQL_Glb_Param.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PQLAPI.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PQLContext.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PQLMessage.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Read_ComReg.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/RimCtrl.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Write_ComReg.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/MESceneAnalysis.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/ME_VstCtrl.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/IPR.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/BasicInfoCheck.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/CalibMEMCInfo.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/MEMC_ParamTable.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Periodic_Manage.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Logo_DDR.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/SObjCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PeriodicCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FlickerCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FlashCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FadeInOutCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/LogoCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/LBMCSwitchCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Read_ComReg_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/MESceneAnalysis_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PanningCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PatchCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/PQL_context_ReadParam_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/Write_ComReg_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/RimCtrl_extend.o \
					  ../../memc_isr/$(CHIP_NAME)/PQL/FBlevelCtrl_extend.o
else
tv_scaler_drv-objs += ../misc/rtk_memc_isr.o \
					  ../memc_isr/scalerMEMC.o \
					  ../memc_isr/Common/kw_common.o \
					  ../memc_isr/Common/kw_debug.o \
					  ../memc_isr/Common/kw_uart.o \
					  ../memc_isr/Driver/regio.o \
					  ../memc_isr/Driver/uartinit.o \
					  ../memc_isr/HAL/hal_cpr.o \
					  ../memc_isr/HAL/hal_crtc.o \
					  ../memc_isr/HAL/hal_dehalo.o \
					  ../memc_isr/HAL/hal_dm.o \
					  ../memc_isr/HAL/hal_hdf.o \
					  ../memc_isr/HAL/hal_ip.o \
					  ../memc_isr/HAL/hal_mc.o \
					  ../memc_isr/HAL/hal_me.o \
					  ../memc_isr/HAL/hal_omet.o \
					  ../memc_isr/HAL/hal_post.o \
					  ../memc_isr/MID/mid_crtc.o \
					  ../memc_isr/MID/mid_dm.o \
					  ../memc_isr/MID/mid_memc.o \
					  ../memc_isr/MID/mid_misc.o \
					  ../memc_isr/MID/mid_mode.o \
					  ../memc_isr/MID/mid_sys.o \
					  ../memc_isr/Platform/memc_lib.o \
					  ../memc_isr//Platform/memc_change_size.o \
					  ../memc_isr/Platform/memc_identification.o \
					  ../memc_isr/Platform/memc_identify_ctrl.o \
					  ../memc_isr/PQL/Dh_close.o \
					  ../memc_isr/PQL/FBlevelCtrl.o \
					  ../memc_isr/PQL/FilmDetcetCtrl.o \
					  ../memc_isr/PQL/FRC_decPhT_generate.o \
					  ../memc_isr/PQL/FRC_glb_Context.o \
					  ../memc_isr/PQL/FRC_LogoDetect.o \
					  ../memc_isr/PQL/FRC_phTable.o \
					  ../memc_isr/PQL/FRC_SceneAnalysis.o \
					  ../memc_isr/PQL/Mc_lbmcSwitch.o \
					  ../memc_isr/PQL/Patch_Manage.o \
					  ../memc_isr/PQL/PQL_context_ReadParam.o \
					  ../memc_isr/PQL/PQL_Glb_Param.o \
					  ../memc_isr/PQL/PQLAPI.o \
					  ../memc_isr/PQL/PQLContext.o \
					  ../memc_isr/PQL/PQLMessage.o \
					  ../memc_isr/PQL/Read_ComReg.o \
					  ../memc_isr/PQL/RimCtrl.o \
					  ../memc_isr/PQL/Write_ComReg.o \
					  ../memc_isr/PQL/MESceneAnalysis.o \
					  ../memc_isr/PQL/ME_VstCtrl.o \
					  ../memc_isr/PQL/IPR.o \
					  ../memc_isr/PQL/BasicInfoCheck.o \
					  ../memc_isr/PQL/CalibMEMCInfo.o \
					  ../memc_isr/PQL/MEMC_ParamTable.o \
					  ../memc_isr/PQL/Periodic_Manage.o \
					  ../memc_isr/PQL/Logo_DDR.o \
					  ../memc_isr/PQL/SObjCtrl_extend.o \
					  ../memc_isr/PQL/PeriodicCtrl_extend.o \
					  ../memc_isr/PQL/FlickerCtrl_extend.o \
					  ../memc_isr/PQL/FlashCtrl_extend.o \
					  ../memc_isr/PQL/FadeInOutCtrl_extend.o \
					  ../memc_isr/PQL/LogoCtrl_extend.o \
					  ../memc_isr/PQL/LBMCSwitchCtrl_extend.o \
					  ../memc_isr/PQL/Read_ComReg_extend.o \
					  ../memc_isr/PQL/MESceneAnalysis_extend.o \
					  ../memc_isr/PQL/PanningCtrl_extend.o \
					  ../memc_isr/PQL/PatchCtrl_extend.o \
					  ../memc_isr/PQL/PQL_context_ReadParam_extend.o \
					  ../memc_isr/PQL/Write_ComReg_extend.o \
					  ../memc_isr/PQL/RimCtrl_extend.o \
					  ../memc_isr/PQL/FBlevelCtrl_extend.o
endif
endif

#dolby/
ifeq (y,$(filter y, $(CONFIG_RTK_KDRV_DV)))
ifneq ($(CHIP_NAME),)
ifeq (y,$(filter y, $(CONFIG_DOLBY_IDK_5)))
tv_scaler_drv-objs += ../../dolby_idk_5/dolby_adapter.o \
                      ../../dolby_idk_5/dolby_facade.o \
                      ../../dolby_idk_5/dolby_device.o \
                      ../../dolby_idk_5/dolby_gdbs_delay.o \
                      ../../dolby_idk_5/dolby_debug.o
else
tv_scaler_drv-objs += ../../dolby_idk_1_6_1_1/dolby_adapter.o \
                      ../../dolby_idk_1_6_1_1/dolby_facade.o \
                      ../../dolby_idk_1_6_1_1/dolby_device.o \
                      ../../dolby_idk_1_6_1_1/dolby_gdbs_delay.o \
                      ../../dolby_idk_1_6_1_1/dolby_debug.o
endif
else
ifeq (y,$(filter y, $(CONFIG_DOLBY_IDK_5)))
tv_scaler_drv-objs += ../dolby_idk_5/dolby_adapter.o \
                      ../dolby_idk_5/dolby_facade.o \
                      ../dolby_idk_5/dolby_device.o \
                      ../dolby_idk_5/dolby_gdbs_delay.o \
                      ../dolby_idk_5/dolby_debug.o
else
tv_scaler_drv-objs += ../dolby_idk_1_6_1_1/dolby_adapter.o \
                      ../dolby_idk_1_6_1_1/dolby_facade.o \
                      ../dolby_idk_1_6_1_1/dolby_device.o \
                      ../dolby_idk_1_6_1_1/dolby_gdbs_delay.o \
                      ../dolby_idk_1_6_1_1/dolby_debug.o
endif
endif
endif


#hdcp2_2 /
ifeq (y,$(filter y, $(CONFIG_OPTEE_HDCP2)))
#do noting, mv hdcp2 to optee os
else
tv_scaler_drv-objs += $(HDCP_PATH)/hdcp2_2/core/crypto_spu.o \
                      $(HDCP_PATH)hdcp2_2/core/secure_flash.o \
                      $(HDCP_PATH)hdcp2_2/core/hdmi.o \
                      $(HDCP_PATH)hdcp2_2/core/bigdigits.o \
                      $(HDCP_PATH)hdcp2_2/core/crypto.o \
                      $(HDCP_PATH)hdcp2_2/core/hmac.o \
                      $(HDCP_PATH)hdcp2_2/core/hdcp2_rcp_api.o \
                      $(HDCP_PATH)hdcp2_2/core/sha2.o \
                      $(HDCP_PATH)hdcp2_2/rx/hdcp2_interface.o \
                      $(HDCP_PATH)hdcp2_2/rx/hdcp2_messages.o \
                      $(HDCP_PATH)hdcp2_2/rx/hdcp2_session.o \
                      $(HDCP_PATH)hdcp2_2/rx/spu_main.o \
                      $(HDCP_PATH)hdcp2_2/tx/test_hdcptx.o \
                      $(HDCP_PATH)hdcp2_2/tx/HDCPTX22.o \
                      $(HDCP_PATH)hdcp2_2/tx/HDMI_HDCP_API.o

endif

#hdmirx/
ifneq (n,$(filter n, $(CONFIG_RTK_HDMI_RX)))
   tv_scaler_drv-objs += $(HDMIRX_PATH)/common/hdmi_debug.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_phy_dfe.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_phy.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_phy_2p1.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_reg.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_mac.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_lib_audio.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_lib_info_packet.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_lib_emp.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_table.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_power_saving_phy.o \
                      $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_dsc_reg.o \
                      $(HDMIRX_PATH)/rtd7032/m8p_hdmi_reg.o \
                      $(HDMIRX_PATH)/rtd7032/m8p_hdmi_mac.o \
                      $(HDMIRX_PATH)/common/hdmi_power_saving.o \
                      $(HDMIRX_PATH)/common/hdmi_drvif.o \
                      $(HDMIRX_PATH)/common/hdmiHdmi.o \
                      $(HDMIRX_PATH)/common/hdmi_info_packet.o \
                      $(HDMIRX_PATH)/common/hdmi_emp.o \
                      $(HDMIRX_PATH)/common/hdmi_cec.o \
                      $(HDMIRX_PATH)/common/hdmi_ddc.o \
                      $(HDMIRX_PATH)/common/hdmi_hdcp.o \
                      $(HDMIRX_PATH)/common/hdmi_hdcp2p2.o  \
                      $(HDMIRX_PATH)/common/hdmi_scdc.o \
                      $(HDMIRX_PATH)/common/hdmi_audio.o \
                      $(HDMIRX_PATH)/common/hdmi_hdr.o \
                      $(HDMIRX_PATH)/common/hdmi_dsc.o \
                      $(HDMIRX_PATH)/common/hdmi_arc.o \
                      $(HDMIRX_PATH)/common/hdmi_vfe.o \
                      $(HDMIRX_PATH)/common/hdmi_vfe_config.o \
                      $(HDMIRX_PATH)/common/hdmi_edid.o \
                      $(HDMIRX_PATH)/common/cea_861.o \
                      $(HDMIRX_PATH)/common/hdmi-sysfs.o \
                      $(HDMIRX_PATH)/common/hdmi_main.o

  ifeq (y,$(filter y, $(CONFIG_RTK_KDRV_HDMI_ENABLE_HDMI21)))
    tv_scaler_drv-objs += $(HDMIRX_PATH)/$(CHIP_NAME)/hdmi_mac_2p1.o
    tv_scaler_drv-objs += $(HDMIRX_PATH)/rtd7032/m8p_hdmi_mac_2p1.o
  endif

  ifeq (y,$(filter y, $(CONFIG_RTK_KDRV_HDMI_HDCP_REPEATER_ENABLE)))
    tv_scaler_drv-objs += $(HDMIRX_PATH)/common/hdmi_hdcp_repeater.o \
                            $(HDMIRX_PATH)/hdcp14_repeater/hdcp14_api.o \
                            $(HDMIRX_PATH)/hdcp14_repeater/hdcp14_block_module.o \
                            $(HDMIRX_PATH)/hdcp14_repeater/hdcp14_lfsr.o \
                            $(HDMIRX_PATH)/hdcp14_repeater/sha1.o
  endif

  ifeq (y,$(filter y, $(CONFIG_HDMI_RX_BUILT_IN_TEST)))
   tv_scaler_drv-objs += $(HDMIRX_PATH)/bist/hdmi_test.o
  endif
endif




ifeq (m,$(filter m, $(CONFIG_SUPPORT_SCALER)))
tv_scaler_drv-objs += scaler_module.o
endif


obj-$(CONFIG_SUPPORT_SCALER) += tv_scaler_drv.o
 
ifeq (y,$(filter y, $(CONFIG_ENABLE_EDPTX)))
	obj-y +=edptx/
endif

#KBUILD_CFLAGS += -Wall -Werror

