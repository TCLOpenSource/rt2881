#ifndef __RTICE_AT_CUSTOMER_H__
#define __RTICE_AT_CUSTOMER_H__


#define AT_LOCAL 0
#define AT_STARTL()
#define AT_ENDL()

#define _COMMAND_DEBUG_KEY_0 0
/* just using a CUST_COMMANDS macro to present a list of macro entries in order,
 * that will be replace with different declaration or definition in order.
 * reference to wiki page of x-macro, https://en.wikipedia.org/wiki/X_Macro
 */

#define AT_CSR_HdmiRX_GetRawTimingInfo_cb AT_HdmiRX_GetRawTimingInfo_cb
#define AT_CSR_SetHDMI_HDCP_OnOff_cb AT_SetHDMI_HDCP_OnOff_cb
#define AT_CSR_Audio_Get_HdmiRX_AudioFormat_cb AT_Audio_Get_HdmiRX_AudioFormat_cb
#define AT_CSR_Audio_Get_DPRX_AudioFormat_cb AT_Audio_Get_DPRX_AudioFormat_cb
#define AT_CSR_Audio_Get_HdmiRX_AudioSamplingRate_cb AT_Audio_Get_HdmiRX_AudioSamplingRate_cb
#define AT_CSR_Audio_Get_DPRX_AudioSamplingRate_cb AT_Audio_Get_DPRX_AudioSamplingRate_cb
#define AT_CSR_HdmiRX_Get_VideoStable_cb AT_HdmiRX_Get_VideoStable_cb
#define AT_CSR_HdmiRX_Get_VIC_cb AT_HdmiRX_Get_VIC_cb
#define AT_CSR_Get_System_Info_cb   AT_Get_System_Info_cb
#define AT_CSR_Set_System_Reboot_cb AT_Set_System_Reboot_cb
#define AT_CSR_Input_Source_Switch_cb AT_Input_Source_Switch_cb
#define AT_CSR_Get_Current_Input_Source_cb AT_Get_Current_Input_Source_cb
#define AT_CSR_Set_HDMIRX_HPD_cb AT_Set_HDMIRX_HPD_cb
#define AT_CSR_Get_HDMI_Cable_Connect_cb AT_Get_HDMI_Cable_Connect_cb
#define AT_CSR_EDID_DDC_Set_RX_table_cb AT_EDID_DDC_Set_RX_table_cb
#define AT_CSR_EDID_DDC_Get_RX_table_cb AT_EDID_DDC_Get_RX_table_cb
#define AT_CSR_HdmiRX_Get_SPD_cb AT_HdmiRX_Get_SPD_cb
#define AT_CSR_Get_HDRmetadata_cb AT_Get_HDRmetadata_cb
#define AT_CSR_Hdmi_Get_HDCP_Mode_cb   AT_Hdmi_Get_HDCP_Mode_cb
#define AT_CSR_Get_HDCP_Version_cb  AT_Get_HDCP_Version_cb
#define AT_CSR_Get_RX_TX_HDCP_Version_cb  AT_Get_RX_TX_HDCP_Version_cb
#define AT_CSR_Get_hdmirx_vtem_cb   AT_Get_hdmirx_vtem_cb
#define AT_CSR_Get_hdmirx_vfreq_real_time_cb   AT_Get_hdmirx_vfreq_real_time_cb
#define AT_CSR_Get_hdmirx_avi_infoframe_cb   AT_Get_hdmirx_avi_infoframe_cb
#define AT_CSR_Get_hdmirx_bit_error_cnt_cb   AT_Get_hdmirx_bit_error_cnt_cb

#define AT_CSR_HdmiTX_Get_HPD_Status_cb AT_HdmiTX_Get_HPD_Status_cb
#define AT_CSR_HdmiTX_Get_VRR_cb AT_HdmiTX_Get_VRR_cb
#define AT_CSR_HdmiTX_Get_EDID_Valid_cb AT_HdmiTX_Get_EDID_Valid_cb
#define AT_CSR_EDID_Get_TX_table_cb AT_EDID_Get_TX_table_cb

#define AT_CSR_USB_Set_SerialNum_cb AT_USB_Set_SerialNum_cb
#define AT_CSR_USB_Get_SerialNum_cb AT_USB_Get_SerialNum_cb
#define AT_CSR_UVC_Set_UVC_Name_cb AT_UVC_Set_Name_cb
#define AT_CSR_UVC_Get_UVC_Name_cb AT_UVC_Get_Name_cb
#define AT_CSR_UAC_Set_UAC_Name_cb AT_UAC_Set_Name_cb
#define AT_CSR_UAC_Get_UAC_Name_cb AT_UAC_Get_Name_cb
#define AT_CSR_UAC_OUT2_Set_UAC_Name_cb AT_UAC_OUT2_Set_Name_cb
#define AT_CSR_UAC_OUT2_Get_UAC_Name_cb AT_UAC_OUT2_Get_Name_cb
#define AT_CSR_UAC_IN_Set_UAC_Name_cb AT_UAC_IN_Set_Name_cb
#define AT_CSR_UAC_IN_Get_UAC_Name_cb AT_UAC_IN_Get_Name_cb
#define AT_CSR_USB_Set_VID_cb AT_USB_Set_VID_cb
#define AT_CSR_USB_Get_VID_cb AT_USB_Get_VID_cb
#define AT_CSR_USB_Set_PID_cb AT_USB_Set_PID_cb
#define AT_CSR_USB_Get_PID_cb AT_USB_Get_PID_cb
#define AT_CSR_USB_Get_HostProtocol_cb AT_USB_Get_HostProtocol_cb
#define AT_CSR_USB_Get_PowerStatus_cb AT_USB_Get_PowerStatus_cb
#define AT_CSR_USB_Get_UVCVideoTiming_cb AT_UVC_GetVideoTimingInfo_cb
#define AT_CSR_USB_Get_UVCVideoFormat_cb AT_UVC_GetVideoFormatInfo_cb
#define AT_CSR_USB_Set_ManufacturerName_cb AT_USB_Set_ManufacturerName_cb
#define AT_CSR_USB_Get_ManufacturerName_cb AT_USB_Get_ManufacturerName_cb
#define AT_CSR_USB_Set_ProductName_cb AT_USB_Set_ProductName_cb
#define AT_CSR_USB_Get_ProductName_cb AT_USB_Get_ProductName_cb
#define AT_CSR_USB_Set_EnableCDC_cb AT_USB_Set_EnableCDC_cb
#define AT_CSR_USB_Get_EnableCDC_cb AT_USB_Get_EnableCDC_cb
#define AT_CSR_USB_Set_VidPidBySpeed_cb AT_USB_Set_VidPidBySpeed_cb
#define AT_CSR_USB_Get_VidPidBySpeed_cb AT_USB_Get_VidPidBySpeed_cb

#define AT_CSR_USB_Get_Force_Speed_cb AT_USB_Get_Force_Speed_cb
#define AT_CSR_USB_Set_Force_Speed_cb AT_USB_Set_Force_Speed_cb

#define AT_CSR_USB_Get_LinkState_Count_cb AT_USB_Get_LinkState_Count_cb

#define AT_CSR_Set_EnableAccelerateMode_cb AT_Set_EnableAccelerateMode_cb

#define AT_CSR_Set_USB_decription_Index_cb AT_Set_USB_decription_Index_cb
#define AT_CSR_Get_USB_decription_Index_cb AT_Get_USB_decription_Index_cb

#define AT_CSR_System_Send_ExternalMCU_cb AT_System_Send_ExternalMCU_cb
#define AT_CSR_System_Recv_ExternalMCU_cb AT_System_Recv_ExternalMCU_cb

#define AT_CSR_GPIO_Get_cb AT_Get_Gpio_cb
#define AT_CSR_GPIO_Set_cb AT_Set_Gpio_cb
#define AT_CSR_LSADC_Get_cb AT_LSADC_Get_cb


#define AT_CSR_PWM_Set_Freq_cb  AT_Set_PWM_Freq
#define AT_CSR_PWM_Get_Freq_cb  AT_Get_PWM_Freq
#define AT_CSR_PWM_Set_Duty_cb  AT_Set_PWM_Duty
#define AT_CSR_PWM_Get_Duty_cb  AT_Get_PWM_Duty

#define AT_CSR_SQE_Set_Imetry_cb AT_SQE_Set_Imetry_cb
#define AT_CSR_SQE_Set_ToneMapping_cb AT_SQE_Set_ToneMapping_cb
#define AT_CSR_HDR2SDR_OnOff_cb AT_HDR2SDR_OnOff_cb
#define AT_CSR_HDR2SDR_GetStatus_cb AT_Get_HDR2SDR_Status_cb
#define AT_CSR_Get_ColorImetry_cb AT_Get_ColorImetry_cb
#define AT_CSR_Set_ColorImetry_cb AT_Set_ColorImetry_cb
#define AT_CSR_Get_ColorRange_cb AT_Get_ColorRange_cb
#define AT_CSR_Get_ColorRange_Setting_cb AT_Get_ColorRange_Setting_cb
#define AT_CSR_Set_ColorRange_cb AT_Set_ColorRange_cb
#define AT_CSR_Get_HDR2SDR_OnOff_Status_cb AT_Get_HDR2SDR_OnOff_Status_cb
#define AT_CSR_Get_Hdr2sdr_ColorParam AT_Get_Hdr2sdr_ColorParam_cb
#define AT_CSR_Set_Hdr2sdr_ColorParam AT_Set_Hdr2sdr_ColorParam_cb
#define AT_CSR_Set_SqeCustomerBaseSetting_cb AT_Set_SqeCustomerBaseSetting
#define AT_CSR_Get_SqeCustomerGroupSetting_cb AT_Get_SqeCustomerGroupSetting
#define AT_CSR_Set_SqeCustomerGroupSetting_cb AT_Set_SqeCustomerGroupSetting

#define AT_CSR_Timing_Switch_Test_cb AT_Timing_Switch_Test_cb

#define AT_CSR_Get_Aux_In_Volume_cb AT_Get_Aux_In_Volume
#define AT_CSR_Set_Aux_In_Volume_cb AT_Set_Aux_In_Volume
#define AT_CSR_Get_Aux_Out_Volume_cb AT_Get_Aux_Out_Volume
#define AT_CSR_Set_Aux_Out_Volume_cb AT_Set_Aux_Out_Volume

#define AT_CSR_Get_OSD_Mode_cb AT_Get_OSD_Mode
#define AT_CSR_Set_OSD_Mode_cb AT_Set_OSD_Mode
#define AT_CSR_Get_OSD_Mode_Level_cb AT_Get_OSD_Mode_Level
#define AT_CSR_Set_OSD_Mode_Level_cb AT_Set_OSD_Mode_Level

#define AT_CSR_Get_DPRX_HPD_Status_cb AT_Get_DPRX_HPD_Status_cb
#define AT_CSR_Set_DPRX_HPD_cb AT_Set_DPRX_HPD_cb
#define AT_CSR_DPRX_Get_VideoStable_cb AT_DPRX_Get_VideoStable_cb
#define AT_CSR_DPRX_GetRawTimingInfo_cb AT_DPRX_GetRawTimingInfo_cb
#define AT_CSR_DPRX_Get_Video_Vfreq_cb AT_DPRX_Get_Video_Vfreq_cb
#define AT_CSR_DPRX_Get_Bit_Error_Count_cb AT_DPRX_Get_Bit_Error_Count_cb
#define AT_CSR_DPRX_Get_HDCP_Mode_cb AT_DPRX_Get_HDCP_Mode_cb


#if 1
#define CUST_COMMANDS(X) \
        AT_STARTL() \
                X(CONFIG_RTK_KDRV_RTICE_AT_CSR_COMMAND_TEST, _COMMAND_DEBUG_KEY_0, AT0_CSR_Command_Debug_cb) \
		AT_ENDL()

#else
	
#define CUST_COMMANDS(X) \
        AT_STARTL() \
                X(CONFIG_RTK_KDRV_RTICE_AT_CSR_COMMAND_TEST, _COMMAND_DEBUG_KEY_0, AT0_CSR_Command_Debug_cb) \
                X(CONFIG_ENABLE_HDMIRX,                      3, AT_CSR_SetHDMI_HDCP_OnOff_cb) \
                X(CONFIG_ENABLE_HDMIRX,                      4, AT_CSR_Audio_Get_HdmiRX_AudioFormat_cb) \
                X(CONFIG_ENABLE_DPRX,                        5, AT_CSR_Audio_Get_DPRX_AudioFormat_cb) \
                X(CONFIG_ENABLE_HDMIRX,                      6, AT_CSR_Audio_Get_HdmiRX_AudioSamplingRate_cb) \
                X(CONFIG_ENABLE_DPRX,                        7, AT_CSR_Audio_Get_DPRX_AudioSamplingRate_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                    8, AT_CSR_Audio_Set_ADC_OnOff_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                    9, AT_CSR_Audio_Set_DACHP_OnOff_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   10, AT_CSR_Audio_Set_ADC_VolumeGain_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   11, AT_CSR_Audio_Get_ADC_VolumeGain_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   12, AT_CSR_Audio_Set_HdmiDPRX_VolumeGain_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   13, AT_CSR_Audio_Get_HdmiDPRX_VolumeGain_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   16, AT_CSR_Audio_Set_UAC_VolumeGain_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   17, AT_CSR_Audio_Get_UAC_VolumeGain_cb) \
                X(CONFIG_USB,                               18, AT_CSR_UVC_Set_UVC_Name_cb) \
                X(CONFIG_USB,                               19, AT_CSR_UVC_Get_UVC_Name_cb) \
                X(CONFIG_USB,                               20, AT_CSR_UAC_Set_UAC_Name_cb) \
                X(CONFIG_USB,                               21, AT_CSR_UAC_Get_UAC_Name_cb) \
                X(CONFIG_USB,                               22, AT_CSR_USB_Set_VID_cb) \
                X(CONFIG_USB,                               23, AT_CSR_USB_Get_VID_cb) \
                X(CONFIG_USB,                               24, AT_CSR_USB_Set_PID_cb) \
                X(CONFIG_USB,                               25, AT_CSR_USB_Get_PID_cb) \
                X(CONFIG_ENABLE_I2C,                        27, AT_CSR_System_Recv_ExternalMCU_cb) \
                X(CONFIG_ENABLE_I2C,                        28, AT_CSR_System_Send_ExternalMCU_cb) \
                X(CONFIG_ENABLE_SQE,                        31, AT_CSR_HDR2SDR_OnOff_cb) \
                X(1,                                        35, AT_CSR_Get_System_Info_cb) \
                X(CONFIG_ENABLE_EXT_MCU_UPGRADE_FW,         36, AT_CSR_Firmware_Upgrade_ExternalMCU_Img_cb) \
                X(1,                                        37, AT_CSR_Set_System_Reboot_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   38, AT_CSR_Audio_Set_UacOut2_MixerSource_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   39, AT_CSR_Audio_Get_UacOut2_MixerSource_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   40, AT_CSR_Audio_Set_DACHP_MixerSource_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   41, AT_CSR_Audio_Get_DACHP_MixerSource_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   42, AT_CSR_Audio_Set_I2SOut_MixerSource_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   43, AT_CSR_Audio_Get_I2SOut_MixerSource_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   44, AT_CSR_Audio_Set_UacOut1_Mute_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   45, AT_CSR_Audio_Get_UacOut1_Mute_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   46, AT_CSR_Audio_Set_UacOut2_Mute_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   47, AT_CSR_Audio_Get_UacOut2_Mute_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   48, AT_CSR_Audio_Set_DACHP_Mute_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   49, AT_CSR_Audio_Get_DACHP_Mute_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   50, AT_CSR_Audio_Set_I2SOut_Mute_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                   51, AT_CSR_Audio_Get_I2SOut_Mute_cb) \
                X(1,                                        52, AT_CSR_Input_Source_Switch_cb) \
                X(1,                                        53, AT_CSR_Get_Current_Input_Source_cb) \
                X(CONFIG_ENABLE_HDMIRX,                     54, AT_CSR_Get_HDMI_Cable_Connect_cb) \
                X(CONFIG_ENABLE_HDMIRX,                     55, AT_CSR_HdmiRX_GetRawTimingInfo_cb) \
                X(CONFIG_ENABLE_HDMIRX,                     56, AT_CSR_HdmiRX_Get_VideoStable_cb) \
                X(CONFIG_ENABLE_HDMIRX,                     60, AT_CSR_HdmiRX_Get_VIC_cb) \
                X(CONFIG_USB,                               64, AT_CSR_USB_Get_HostProtocol_cb) \
                X(CONFIG_ENABLE_HDMITX,                     65, AT_CSR_HdmiTX_Get_HPD_Status_cb) \
                X(CONFIG_ENABLE_HDMITX,                     66, AT_CSR_HdmiTX_Get_VRR_cb) \
                X(CONFIG_ENABLE_HDMITX,                     67, AT_CSR_HdmiTX_Get_EDID_Valid_cb) \
                X(CONFIG_USB,                               68, AT_CSR_USB_Get_UVCVideoTiming_cb) \
                X(CONFIG_USB,                               69, AT_CSR_USB_Get_UVCVideoFormat_cb) \
                X(1,                                        70, AT_CSR_UVC_Get_Err_status_cb) \
                X(CONFIG_ENABLE_PWM,                        71, AT_CSR_PWM_Set_Freq_cb) \
                X(CONFIG_ENABLE_PWM,                        72, AT_CSR_PWM_Get_Freq_cb) \
                X(CONFIG_ENABLE_PWM,                        73, AT_CSR_PWM_Set_Duty_cb) \
                X(CONFIG_ENABLE_PWM,                        74, AT_CSR_PWM_Get_Duty_cb) \
                X(CONFIG_ENABLE_HDMIRX,                     75, AT_CSR_HdmiRX_Get_SPD_cb) \
                X(CONFIG_USB,                               76, AT_CSR_USB_Get_PowerStatus_cb) \
                X(1,                                        77, AT_CSR_EDID_Set_EDID_mode_cb) \
                X(1,                                        78, AT_CSR_EDID_Get_EDID_mode_cb) \
                X(CONFIG_USB,                               79, AT_CSR_USB_Set_SerialNum_cb) \
                X(CONFIG_USB,                               80, AT_CSR_USB_Get_SerialNum_cb) \
                X(1,                                        81, AT_CSR_Flash_Set_CustomerProprietary_cb) \
                X(1,                                        82, AT_CSR_Flash_Get_CustomerProprietary_cb) \
                X(1,                                        84, AT_CSR_EDID_Flash_Get_table_cb) \
                X(1,                                        85, AT_CSR_EDID_Flash_Set_Default_table_cb) \
                X(1,                                        86, AT_CSR_EDID_Flash_Get_Default_table_cb) \
                X(CONFIG_ENABLE_HDMIRX,                     87, AT_CSR_EDID_DDC_Set_RX_table_cb) \
                X(CONFIG_ENABLE_HDMIRX,                     88, AT_CSR_EDID_DDC_Get_RX_table_cb) \
                X(CONFIG_ENABLE_HDMITX,                     89, AT_CSR_EDID_Get_TX_table_cb) \
                X(CONFIG_ENABLE_GPIO,                       90, AT_CSR_GPIO_Get_cb) \
                X(CONFIG_ENABLE_GPIO,                       91, AT_CSR_GPIO_Set_cb) \
                X(CONFIG_ENABLE_LSADC,                      92, AT_CSR_LSADC_Get_cb) \
                X(1,                                        93, AT_CSR_RestoreToDefault_cb) \
                X(CONFIG_USB,                               95, AT_CSR_USB_Get_ManufacturerName_cb) \
                X(CONFIG_USB,                               96, AT_CSR_USB_Set_ManufacturerName_cb) \
                X(CONFIG_USB,                               97, AT_CSR_USB_Get_ProductName_cb) \
                X(CONFIG_USB,                               98, AT_CSR_USB_Set_ProductName_cb) \
                X(CONFIG_ENABLE_SQE,                        99, AT_CSR_SQE_Set_Imetry_cb) \
                X(CONFIG_ENABLE_SQE,                       100, AT_CSR_SQE_Set_ToneMapping_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    101, AT_CSR_Get_HDRmetadata_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                  102, AT_CSR_Audio_Set_DAC_VolumeGain_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                  103, AT_CSR_Audio_Get_DAC_VolumeGain_cb) \
                X(CONFIG_USB,                              104, AT_CSR_USB_Set_EnableCDC_cb) \
                X(CONFIG_USB,                              105, AT_CSR_UAC_OUT2_Set_UAC_Name_cb) \
                X(CONFIG_USB,                              106, AT_CSR_UAC_OUT2_Get_UAC_Name_cb) \
                X(CONFIG_USB,                              107, AT_CSR_UAC_IN_Set_UAC_Name_cb) \
                X(CONFIG_USB,                              108, AT_CSR_UAC_IN_Get_UAC_Name_cb) \
                X(CONFIG_USB,                              109, AT_CSR_USB_Set_VidPidBySpeed_cb) \
                X(CONFIG_USB,                              110, AT_CSR_USB_Get_VidPidBySpeed_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    111, AT_CSR_Set_HDMIRX_HPD_cb) \
                X(1,                                       112, AT_CSR_Audio_Set_PartyChat_GameSound_VolumeGain_cb) \
                X(1,                                       113, AT_CSR_Audio_Get_PartyChat_GameSound_VolumeGain_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    114, AT_CSR_Hdmi_Get_HDCP_Mode_cb) \
                X(1,                                       115, AT_CSR_Audio_Get_PartyChat_Status_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                  116, AT_CSR_Audio_Get_ADC_OnOff_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                  117, AT_CSR_Audio_Get_DACHP_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,                       118, AT_CSR_HDR2SDR_GetStatus_cb) \
                X(1,                                       119, AT_CSR_Get_Customer_Ver_cb) \
                X(1,                                       120, AT_CSR_Get_Rescue_Ver_cb) \
                X(CONFIG_ENABLE_SQE,                       121, AT_CSR_Get_ColorImetry_cb) \
                X(CONFIG_ENABLE_SQE,                       122, AT_CSR_Set_ColorImetry_cb) \
                X(CONFIG_ENABLE_SQE,                       123, AT_CSR_Get_ColorRange_cb) \
                X(CONFIG_ENABLE_SQE,                       124, AT_CSR_Set_ColorRange_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    125, AT_CSR_Get_hdmirx_vtem_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    126, AT_CSR_Get_HDCP_Version_cb) \
                X(CONFIG_ENABLE_AMP,                       127, AT_CSR_Get_Aux_In_Volume_cb) \
                X(CONFIG_ENABLE_AMP,                       128, AT_CSR_Set_Aux_In_Volume_cb) \
                X(CONFIG_ENABLE_AMP,                       129, AT_CSR_Get_Aux_Out_Volume_cb) \
                X(CONFIG_ENABLE_AMP,                       130, AT_CSR_Set_Aux_Out_Volume_cb) \
                X(CONFIG_ENABLE_HDMITX,                    131, AT_CSR_Timing_Switch_Test_cb) \
                X(1,                                       132, AT_CSR_Set_LedLight_cb) \
                X(1,                                       133, AT_CSR_Get_LedLight_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    134, AT_CSR_Get_hdmirx_vfreq_real_time_cb) \
                X(1,                                       136, AT_CSR_Get_VRR_follow_mode_cb) \
                X(1,                                       137, AT_CSR_Set_VRR_follow_mode_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    138, AT_CSR_Get_RX_TX_HDCP_Version_cb) \
                X(CONFIG_USB,                              139, AT_CSR_USB_Get_EnableCDC_cb) \
                X(CONFIG_USB,                              140, AT_CSR_USB_Get_LinkState_Count_cb) \
                X(CONFIG_USB,                              141, AT_CSR_USB_Get_Force_Speed_cb) \
                X(CONFIG_USB,                              142, AT_CSR_USB_Set_Force_Speed_cb) \
                X(1,                                       143, AT_CSR_Get_Chip_Id_cb) \
                X(CONFIG_ENABLE_SQE,                       144, AT_CSR_Get_HDR2SDR_OnOff_Status_cb) \
                X(CONFIG_ENABLE_SQE,                       145, AT_CSR_Get_ColorRange_Setting_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    146, AT_CSR_Get_hdmirx_avi_infoframe_cb) \
                X(CONFIG_ENABLE_HDMIRX,                    147, AT_CSR_Get_hdmirx_bit_error_cnt_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                  148, Audio_CSR_Send_Non_Pcm_Data_cb) \
                X(CONFIG_ENABLE_SW_AUDIO,                  149, Audio_CSR_Get_Send_Non_Pcm_Data_On_off_cb) \
                X(1,                                       150, AT_CSR_DPRX_EDID_Get_EDID_mode_cb) \
                X(1,                                       151, AT_CSR_DPRX_EDID_Set_EDID_mode_cb) \
                X(1,                                       152, AT_CSR_DPRX_EDID_Flash_Set_Default_table_cb) \
                X(1,                                       153, AT_CSR_DPRX_EDID_Flash_Get_Default_table_cb) \
                X(1,                                       154, AT_CSR_DPRX_EDID_Flash_Get_table_cb) \
                X(CONFIG_ENABLE_SQE,                       155, AT_CSR_Get_Hdr2sdr_ColorParam) \
                X(CONFIG_ENABLE_SQE,                       156, AT_CSR_Set_Hdr2sdr_ColorParam) \
                X(1,                                       157, AT_CSR_Get_ReleaseMode_OnOff_cb) \
                X(1,                                       158, AT_CSR_Set_ReleaseMode_OnOff_cb) \
                X(1,                                       159, AT_CSR_Set_Uart_OnOff_cb) \
                X(1,                                       160, AT_CSR_Get_Uart_OnOff_cb) \
                X(CONFIG_ENABLE_OSD,                       161, AT_CSR_Get_OSD_Mode_cb) \
                X(CONFIG_ENABLE_OSD,                       162, AT_CSR_Set_OSD_Mode_cb) \
                X(CONFIG_ENABLE_OSD,                       163, AT_CSR_Get_OSD_Mode_Level_cb) \
                X(CONFIG_ENABLE_OSD,                       164, AT_CSR_Set_OSD_Mode_Level_cb) \
                X(CONFIG_ENABLE_SQE,                       167, AT_CSR_Set_SqeCustomerBaseSetting_cb) \
                X(CONFIG_ENABLE_SQE,                       171, AT_CSR_Get_SqeCustomerGroupSetting_cb) \
                X(CONFIG_ENABLE_SQE,                       172, AT_CSR_Set_SqeCustomerGroupSetting_cb) \
                X(CONFIG_USB,                              175, AT_CSR_Set_EnableAccelerateMode_cb) \
                X(CONFIG_USB,                              176, AT_CSR_Set_USB_decription_Index_cb) \
                X(CONFIG_USB,                              177, AT_CSR_Get_USB_decription_Index_cb) \
                X(CONFIG_ENABLE_DPRX,                      178, AT_CSR_DPRX_GetRawTimingInfo_cb) \
                X(CONFIG_ENABLE_DPRX,                      180, AT_CSR_DPRX_Get_HDCP_Mode_cb) \
                X(CONFIG_ENABLE_DPRX,                      181, AT_CSR_Get_DPRX_HPD_Status_cb) \
                X(CONFIG_ENABLE_DPRX,                      182, AT_CSR_DPRX_Get_VideoStable_cb) \
                X(CONFIG_ENABLE_DPRX,                      183, AT_CSR_Set_DPRX_HPD_cb) \
                X(CONFIG_ENABLE_DPRX,                      185, AT_CSR_DPRX_Get_Video_Vfreq_cb) \
                X(CONFIG_ENABLE_DPRX,                      189, AT_CSR_DPRX_Get_Bit_Error_Count_cb) \
        AT_ENDL()
#endif
/* declare the 'extern function' with list of CUST_COMMANDS */
typedef void (* cust_handler_t)(void);
#define _CUST_EXTERN_DECLARE(kconfig, id, func)  extern void func(void);
#define CUST_EXTERN_DECLARE()                     \
        CUST_COMMANDS(_CUST_EXTERN_DECLARE)

CUST_EXTERN_DECLARE()


/* declare the 'enum' with list of CUST_COMMANDS */
#define CUST_SUB_IDX_OF(function) CUST_SUBIDX_##function
#define _CUST_ENUM_TYPEDEF(kconfig, id, func, ...)  CUST_SUB_IDX_OF(func) = id,
#define CUST_ENUM_TYPEDEF(type, typedef_name)       \
        typedef type {                              \
                CUST_COMMANDS(_CUST_ENUM_TYPEDEF)   \
         } typedef_name

CUST_ENUM_TYPEDEF(enum CUST_E, CUST_ENUM);



/* the 0xA0 TPDU of CUST command service group
 +---------------------------------------------------------------------------+
 | B0   | B1~2       |    *      | B4-B7     | 4Bytes | * | 4Bytes   | 1Byte |
 +------+------------+-----------+-----------+--------+---+----------+-------+
 | 0xA1 | 4*(n+1) +X | 0-padding | CMD_INDEX | ARG[0] | * | ARG[n-1] | LRC   |
 +---------------------------------------------------------------------------+
 - all of CMD_INDEX and ARGS are INT32 type and little endian.
 - amount of ARGS could be 0 ~ n.
 - X=2 if (n+1)<0x20, otherwise X=1. (be compatible to general TPDU definition)
*/

/* the 0xA1 R-TPDU of CUST command service group
 +----------------------------------------------------+
 | B0   | B1~2     |    *      | B4-starting  | 1Byte |
 +------+----------+-----------+--------------+-------+
 | 0xA1 | 4*(n)+x  | 0-padding | 4N data      | LRC   |
 +----------------------------------------------------+
 - x=2 if n<0x20, otherwise x=1. (be compatible to general TPDU definition)
*/

#define RTICE_AT_CMD_CUSTOMER	 		RTICE_OP_CODE(RTICE_CMD_GROUP_ID_AT, 1)   //0xA1

#define RTICE_CUST_ERR_ABORT			RTICE_FUNC_ERR(0)
#define RTICE_CUST_ERR_TIMEOUT			RTICE_FUNC_ERR(1)
#define RTICE_CUST_ERR_INVALID_BUS_ID	RTICE_FUNC_ERR(2)
#define RTICE_CUST_ERR_INVALID_ADDRESS	RTICE_FUNC_ERR(3)





#endif
