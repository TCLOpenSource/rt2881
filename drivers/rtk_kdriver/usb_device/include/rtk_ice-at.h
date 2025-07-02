#ifndef __RTICE_AT_H__
#define __RTICE_AT_H__

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************
#define AT_LOCAL 0
#define AT_STARTL()
#define AT_ENDL()


/* just using a AT_COMMANDS macro to present a list of macro entries in order,
 * that will be replace with different declaration or definition in order.
 * reference to wiki page of x-macro, https://en.wikipedia.org/wiki/X_Macro
 */
#if 1

#define AT_COMMANDS(X) \
        AT_STARTL() \
                X(1, 0, AT0_Command_Debug_cb) \
        AT_ENDL()
		
#else
 
#define AT_COMMANDS(X) \
        AT_STARTL() \
                X(CONFIG_RTK_KDRV_RTICE_AT_COMMAND_TEST, AT0_COMMAND_DEBUG_KEY, AT0_Command_Debug_cb) \
                X(CONFIG_ENABLE_HDMIRX,         2,  AT_HdmiRX_GetRawTimingInfo_cb) \
                X(CONFIG_ENABLE_HDMITX,         3,  AT_HdmiTX_GetRawTimingInfo_cb) \
                X(CONFIG_ENABLE_DPRX,           4,  AT_DPRX_GetRawTimingInfo_cb) \
                X(CONFIG_ENABLE_TV_SCALER,      5,  AT_VGIP_GetRawTimingInfo_cb) \
                X(CONFIG_ENABLE_HDMIRX,         6,  AT_Get_HDMI_HPD_Status_cb) \
                X(CONFIG_ENABLE_DPRX,           7,  AT_Get_DPRX_HPD_Status_cb) \
                X(CONFIG_ENABLE_SQE,            8,  AT_Get_PQ_GeneralMode_Status_cb) \
                X(CONFIG_ENABLE_SQE,            9,  AT_PQ_GeneralMode_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           10,  AT_Get_SimpleNR_Status_cb) \
                X(CONFIG_ENABLE_SQE,           11,  AT_PQ_SimpleNR_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           12,  AT_Get_PQ_SimpleSharpness_Status_cb) \
                X(CONFIG_ENABLE_SQE,           13,  AT_PQ_SimpleSharpness_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           14,  AT_Get_PQ_GameMode_Status_cb) \
                X(CONFIG_ENABLE_SQE,           15,  AT_PQ_GameMode_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           16,  AT_Get_PQ_Color_Status_cb) \
                X(CONFIG_ENABLE_SQE,           17,  AT_PQ_Color_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           18,  AT_Get_PQ_Contrast_Status_cb) \
                X(CONFIG_ENABLE_SQE,           19,  AT_PQ_Contrast_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           20,  AT_Get_PQ_SRNN_HDMI_Status_cb) \
                X(CONFIG_ENABLE_SQE,           21,  AT_PQ_SRNN_HDMI_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           22,  AT_Get_PQ_GPNN_HDMI_Status_cb) \
                X(CONFIG_ENABLE_SQE,           23,  AT_PQ_GPNN_HDMI_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           24,  AT_Get_HDMI_2nd_UZU_Status_cb) \
                X(CONFIG_ENABLE_SQE,           25,  AT_HDMI_2nd_UZU_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           26,  AT_Get_HDMI_2nd_UZU_DI_Status_cb) \
                X(CONFIG_ENABLE_SQE,           27,  AT_HDMI_2nd_UZU_DI_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           28,  AT_Get_HDMI_UZD_TX_Status_cb) \
                X(CONFIG_ENABLE_SQE,           29,  AT_HDMI_UZD_TX_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           30,  AT_Get_HDR2SDR_Status_cb) \
                X(CONFIG_ENABLE_SQE,           31,  AT_HDR2SDR_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           32,  AT_Get_UVC_UZD_Status_cb) \
                X(CONFIG_ENABLE_SQE,           33,  AT_UVC_UZD_OnOff_cb) \
                X(CONFIG_USB,                  34,  AT_UVC_GetTimingInfo_cb) \
                X(CONFIG_USB,                  35,  AT_UVC_SetTimingInfo_cb) \
                X(CONFIG_ENABLE_HDMIRX,        36,  AT_Set_HDMIRX_HPD_cb) \
                X(CONFIG_ENABLE_MJPEG_ENC_DEC, 38,  AT_Get_ENC_Status_cb) \
                X(CONFIG_ENABLE_MJPEG_ENC_DEC, 39,  AT_ENC_OnOff_cb) \
                X(CONFIG_ENABLE_MJPEG_ENC_DEC, 40,  AT_Get_DEC_Status_cb) \
                X(CONFIG_ENABLE_MJPEG_ENC_DEC, 41,  AT_DEC_OnOff_cb) \
                X(CONFIG_MACH_DANTE,           44,  AT_Get_Booting_Status_cb) \
                X(CONFIG_MACH_DANTE,           45,  AT_Get_System_Info_cb) \
                X(CONFIG_MACH_DANTE,           46,  AT_Get_Current_Input_Source_cb) \
                X(CONFIG_MACH_DANTE,           47,  AT_Input_Source_Switch_cb) \
                X(CONFIG_ENABLE_HDMIRX,        48,  AT_Get_Dolby_Status_cb) \
                X(CONFIG_MACH_DANTE,           50,  AT_HDMI_Output_Resolution_Switch_cb) \
                X(CONFIG_MACH_DANTE,           53,  AT_Get_OSD_Status_cb) \
                X(CONFIG_MACH_DANTE,           54,  AT_Set_OSD_OnOff_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     57,  AT_Get_fsync_Status_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     58,  AT_Get_FRC_Status_cb) \
                X(CONFIG_ENABLE_SQE,           59,  AT_Get_SR_Status_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     61,  AT_Get_BlackScreen_Status_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     62,  AT_Set_BlackScreen_OnOff_cb) \
                X(CONFIG_ENABLE_HDMIRX,        65,  AT_SetHDMI_HDCP_OnOff_cb) \
                X(CONFIG_MACH_DANTE,           68,  AT_Set_System_Reboot_cb) \
                X(CONFIG_MACH_DANTE,           72,  AT_Set_Power_OnOff_cb) \
                X(CONFIG_ENABLE_SQE,           73,  AT_Get_Display_Width_Length_After_SRNN_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     74,  AT_Get_DC2H_TimingInfo_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     82,  AT_SCALER_VSC_TASK_EVENT_TYPE_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     83,  AT_SCALER_HDMI_TX_TASK_EVENT_TYPE_cb) \
                X(CONFIG_ENABLE_TV_SCALER,     84,  AT_SCALER_DC2H_TASK_EVENT_TYPE_cb) \
                X(CONFIG_MACH_DANTE,           85,  AT_Get_HDMITX_MODE_cb) \
                X(CONFIG_MACH_DANTE,           86,  AT_Set_HDMITX_MODE_cb) \
                X(CONFIG_ENABLE_HDMIRX,        87,  AT_Audio_Get_HdmiRX_AudioFormat_cb) \
                X(CONFIG_ENABLE_HDMIRX,        88,  AT_Audio_Get_HdmiRX_AudioSamplingRate_cb) \
                X(CONFIG_ENABLE_HDMIRX,        89,  AT_HdmiRX_Get_VideoStable_cb) \
                X(CONFIG_ENABLE_HDMIRX,        90,  AT_HdmiRX_Get_VIC_cb) \
                X(CONFIG_MACH_DANTE,           93,  AT_GetTimes_from_EachModule_to_ShowVideo_cb) \
                X(CONFIG_ENABLE_I2C,           94,  AT_System_Send_ExternalMCU_cb) \
                X(CONFIG_ENABLE_I2C,           95,  AT_System_Recv_ExternalMCU_cb) \
                X(CONFIG_ENABLE_GPIO,          96,  AT_Get_Gpio_cb) \
                X(CONFIG_ENABLE_GPIO,          97,  AT_Set_Gpio_cb) \
                X(CONFIG_ENABLE_LSADC,         98,  AT_LSADC_Get_cb) \
                X(CONFIG_ENABLE_HDMIRX,        99,  AT_Hdmi_Get_HDCP_Mode_cb) \
                X(CONFIG_MACH_DANTE,           100, AT_Get_FlashFileInfo_cb) \
                X(CONFIG_MACH_DANTE,           101, AT_Get_FlashData_cb) \
                X(CONFIG_MACH_DANTE,           103, AT_CSR_Get_CID_cb) \
                X(CONFIG_ENABLE_PWM,           104, AT_Set_PWM_Freq) \
                X(CONFIG_ENABLE_PWM,           105, AT_Get_PWM_Freq) \
                X(CONFIG_ENABLE_PWM,           106, AT_Set_PWM_Duty) \
                X(CONFIG_ENABLE_PWM,           107, AT_Get_PWM_Duty) \
                X(CONFIG_ENABLE_AMP,           108, AT_Get_Aux_In_Volume) \
                X(CONFIG_ENABLE_AMP,           109, AT_Set_Aux_In_Volume) \
                X(CONFIG_ENABLE_AMP,           110, AT_Get_Aux_Out_Volume) \
                X(CONFIG_ENABLE_AMP,           111, AT_Set_Aux_Out_Volume) \
                X(CONFIG_ENABLE_HDMIRX,        112, AT_Get_HDCP_Version_cb) \
                X(CONFIG_ENABLE_HDMIRX,        113, AT_Get_RX_TX_HDCP_Version_cb) \
                X(CONFIG_ENABLE_SQE,           114, AT_Set_PQ_OsdLevel_cb) \
        AT_ENDL()
#endif
/* declare the 'extern function' with list of AT_COMMANDS */
typedef void (* at_handler_t)(void);
#define _AT_EXTERN_DECLARE(kconfig, id, func)  extern void func(void);
#define AT_EXTERN_DECLARE()                     \
        AT_COMMANDS(_AT_EXTERN_DECLARE)

AT_EXTERN_DECLARE()


/* declare the 'enum' with list of AT_COMMANDS */
#define AT_SUB_IDX_OF(function) AT_SUBIDX_##function
#define _AT_ENUM_TYPEDEF(kconfig, id, func, ...)  AT_SUB_IDX_OF(func) = id,
#define AT_ENUM_TYPEDEF(type, typedef_name)     \
        typedef type {                          \
                AT_COMMANDS(_AT_ENUM_TYPEDEF)   \
         } typedef_name

AT_ENUM_TYPEDEF(enum AT_E, AT_ENUM);



#endif
