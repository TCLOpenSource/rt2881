#ifndef BUILD_QUICK_SHOW 
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <rtk_kdriver/io.h>
#include <generated/autoconf.h>
#include <linux/i2c.h>
#include <rtk_kdriver/i2c-rtk-api.h>
#else
#include "no_os/slab.h"
#include <string.h>
#endif
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
#include <scalercommon/scalerCommon.h>
#endif

#include <tvscalercontrol/panel/panelapi.h>
//#include <tvscalercontrol/panel/panelTconCtrlAPI.h>
#include <tvscalercontrol/panel/panel.h>
#include <tvscalercontrol/io/ioregdrv.h>

#include <tvscalercontrol/hdmirx/hdmifun.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#include <scaler_vbedev.h>

#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerDrvCommon.h>
#else
#include <scalercommon/scalerDrvCommon.h>
#endif
#include <mach/platform.h>
#include <rbus/mdomain_cap_reg.h>
//#include <rbus/rbus_DesignSpec_MISC_GPIOReg.h>
#include <rtd_log/rtd_module_log.h>
#include <rtk_kdriver/rtk_gpio.h>
#include <pcbMgr.h>
#include <tvscalercontrol/panel/panelapi.h>





#ifdef CONFIG_PCBMGR
#include <Platform_Lib/Board/pcbMgr.h>
	static unsigned long long m_PIN_PANEL_ON_OFF=0, m_PIN_BL_ON_OFF=0;
#endif

#ifdef ENABLE_DCR
      static int m_PIN_BL_ADJ=-1;//,m_PIN_BL_ON_OFF=0,m_BACKLIGHT_ON=0;
#endif

//USER: Vincent_Lee  DATE_2012_5_18  TODO: Add soft I2C for Gamma IC control (BUF_16821)
#ifdef OPEN_CELL_PANEL
#include <Platform_Lib/softi2c/softi2c.h>
#endif

//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/ppoverlay_reg.h>
#include <rbus/ldspi_reg.h>
//#include <rbus/scaler/rbusPifReg.h>
#include <rbus/pif_reg.h>
#include <tvscalercontrol/scalerdrv/adjust.h>
#include <tvscalercontrol/scalerdrv/framesync.h>
//#include <rbus/scaler/tve_reg.h>

//Leo Chen+
//#include <sys/ioctl.h>
//add by 4K1K 240Hz
#include <rbus/pinmux_reg.h>
#include <rbus/osdovl_reg.h>

#define BL_LEVEL_UI_MAX_DEFAULT					100	//10
#define BL_LEVEL_UI_MIN_DEFAULT					0
#define BL_LEVEL_DCR_MAX_DEFAULT				100
#define BL_LEVEL_DCR_MIN_DEFAULT				0

#define BL_LEVEL_OPC_MAX_DEFAULT				100
#define BL_LEVEL_OPC_MIN_DEFAULT				0

#define BL_LEVEL_ACT_MAX_DEFAULT				255
#define BL_LEVEL_ACT_MIN_DEFAULT				0

//#ifdef ENABLE_DCR
#define DCR_NODE_NUM                                                5
//#endif

#define CONFIG_PICASSO_DISP_DEN_STA_HPOS_3DMODE (16)
#define CONFIG_PICASSO_DISP_DEN_END_HPOS_3DMODE (960+16)
//#define CONFIG_DISP_HORIZONTAL_TOTAL1_3DMODE (1060 - 70)
#define CONFIG_DISP_HORIZONTAL_TOTAL_3DMODE (1060)
//#define CONFIG_DISP_DEN_STA_HPOS1_3DMODE (98 - 70)
//#define CONFIG_DISP_DEN_STA_HPOS_3DMODE (15)
//#define CONFIG_DISP_DEN_END_HPOS1_3DMODE (960+(98-70))
//#define CONFIG_DISP_DEN_END_HPOS_3DMODE (960+15)
#define CONFIG_DISP_ACT_END_HPOS_3DMODE (960)
#define CONFIG_DISP_HSYNC_LASTLINE_3DMODE (1060)

#define PANEL_FRONT_PORCH_MIN		3
#if 0
//change this static variable to be global for let SetupFunctions.cpp can access this information
unsigned int 					m_backlightLevelUiMax = BL_LEVEL_UI_MAX_DEFAULT;
static unsigned int 					m_backlightLevelUiMin = BL_LEVEL_UI_MIN_DEFAULT;
static unsigned int 					m_backlightLevelActMax = BL_LEVEL_ACT_MAX_DEFAULT;
static unsigned int 					m_backlightLevelActMin = BL_LEVEL_ACT_MIN_DEFAULT;
//change this static variable to be global for let SetupFunctions.cpp can access this information
#endif
#if 0
unsigned int 					m_isIncreaseMode = TRUE;
static void 					(*m_callbackSetPwmDuty)(UINT8 value) = NULL;
void (*m_callbackPanelInit)(void)=NULL ; // skyworth yf 20111125
void (*m_callbackTconControl)(MILLET_I2C_INPUT_FORMAT mode) = NULL;
#endif
PANEL_CONFIG_PARAMETER *pPanelCOnfigParameter = NULL;
PANEL_PIXEL_MODE PanelPixelModeCount = PANEL_1_PIXEL;
#if 0
static unsigned int 					m_origbacklightLevel = 255;
static	bool							m_blockbacklight = 0;
#endif
#ifdef ENABLE_DCR
static UINT8                                *DCR_TABLE = NULL;    //20101111 added by Leo Chen
static UINT8                                m_backlight_level_from_user=0;
#endif

static UINT8                                panel_sel_table = 0;
static UINT8                                force_oled_type = 0;
static UINT8                                panel_backlihgt_str_ctrl_mode = _BACKLIGHT_STR_CTRL_MODE_BY_KERNEL;

typedef enum _DCR_PARAM {
    DCR_MV_REF,
    DCR_BL_REF,
    DCR_BL_DUTY_REF,
    DCR_DUTY_REF,
    DCR_MODE_SELECT,
    DCR_PARAM_MAX
} DCR_PARAM;

UINT32 LVDS_4_PORT_MAPPING_TABLE[24] ={	// mapping LVDS a b c d port,ex:0x0123,port a = 0,port b = 1,port c = 2,port d = 3
	0x0123,	0x0132,	0x0213,	0x0231,
	0x0312,	0x0321,	0x1023,	0x1032,
	0x1203,	0x1230,	0x1320,	0x1302,
	0x2013,	0x2031,	0x2103,	0x2130,
	0x2301,	0x2310,	0x3012,	0x3021,
	0x3102,	0x3120,	0x3210,	0x3201,
};


STRUCT_PANEL_2K1K_COMMON_SETTING Panel_2k1k_common_param[] =
{
	{
		2,				 //disp_2k1k_port;
		154,            		 //disp_2k1k_clock_max;
		135,           		 //disp_2k1k_clock_min;
		148*1000000,    	 //disp_2k1k_clock_typical;
		2200,           		//disp_2k1k_horizontal_total;
		1125,           		//disp_2k1k_vertical_total;
		1308,           		//disp_2k1k_vertical_total_50hz_min;
		1380,           		//disp_2k1k_vertical_total_50hz_max;
		1100,           		//disp_2k1k_vertical_total_60hz_min;
		1149,           		//disp_2k1k_vertical_total_60hz_max;

		31,				//disp_2k1k_hsync_width;
		5,              		//disp_2k1k_vsync_length;
		140,            		//disp_2k1k_den_sta_hpos;
		2060,           		//disp_2k1k_den_end_hpos;
		16,             		//disp_2k1k_den_sta_vpos;
		1096,           		//disp_2k1k_den_end_vpos;
		0,              		//disp_2k1k_act_sta_hpos;
		1920,           		//disp_2k1k_act_end_hpos;
		0,              		//disp_2k1k_act_sta_vpos;
		1080,           		//disp_2k1k_act_end_vpos;
		2200,			//disp_2k1k_hsync_lastline;
		0x23010000,		//disp_2k1k_port_config1;
		0x00000000,		//disp_2k1k_port_config2;
	},	// CMI_Millet3_v500_4k2k
	{
		1,				 //disp_2k1k_port;
		154,            		 //disp_2k1k_clock_max;
		135,           		 //disp_2k1k_clock_min;
		148*1000000,    	 //disp_2k1k_clock_typical;
		2200,           		//disp_2k1k_horizontal_total;
		1125,           		//disp_2k1k_vertical_total;
		1308,           		//disp_2k1k_vertical_total_50hz_min;
		1380,           		//disp_2k1k_vertical_total_50hz_max;
		1100,           		//disp_2k1k_vertical_total_60hz_min;
		1149,           		//disp_2k1k_vertical_total_60hz_max;

		31,				//disp_2k1k_hsync_width;
		5,              		//disp_2k1k_vsync_length;
		140,            		//disp_2k1k_den_sta_hpos;
		2060,           		//disp_2k1k_den_end_hpos;
		16,             		//disp_2k1k_den_sta_vpos;
		1096,           		//disp_2k1k_den_end_vpos;
		0,              		//disp_2k1k_act_sta_hpos;
		1920,           		//disp_2k1k_act_end_hpos;
		0,              		//disp_2k1k_act_sta_vpos;
		1080,           		//disp_2k1k_act_end_vpos;
		2200,			//disp_2k1k_hsync_lastline;
		0x00010000,		//disp_2k1k_port_config1;
		0x00000000,		//disp_2k1k_port_config2;
	},	// PA168
	{
		2,				 //disp_2k1k_port;
		154,            		 //disp_2k1k_clock_max;
		135,           		 //disp_2k1k_clock_min;
		148*1000000,    	 //disp_2k1k_clock_typical;
		2200,           		//disp_2k1k_horizontal_total;
		1125,           		//disp_2k1k_vertical_total;
		1308,           		//disp_2k1k_vertical_total_50hz_min;
		1380,           		//disp_2k1k_vertical_total_50hz_max;
		1100,           		//disp_2k1k_vertical_total_60hz_min;
		1149,           		//disp_2k1k_vertical_total_60hz_max;

		31,				//disp_2k1k_hsync_width;
		5,              		//disp_2k1k_vsync_length;
		140,            		//disp_2k1k_den_sta_hpos;
		2060,           		//disp_2k1k_den_end_hpos;
		16,             		//disp_2k1k_den_sta_vpos;
		1096,           		//disp_2k1k_den_end_vpos;
		0,              		//disp_2k1k_act_sta_hpos;
		1920,           		//disp_2k1k_act_end_hpos;
		0,              		//disp_2k1k_act_sta_vpos;
		1080,           		//disp_2k1k_act_end_vpos;
		2200,			//disp_2k1k_hsync_lastline;
		0x10320000,		//disp_2k1k_port_config1;
		0x00000000,		//disp_2k1k_port_config2;
	},	// CMI_Millet3_v500_4k2k_p1302
	{
		2,				 //disp_2k1k_port;
		154,					 //disp_2k1k_clock_max;
		135,				 //disp_2k1k_clock_min;
		148*1000000,		 //disp_2k1k_clock_typical;
		2200,					//disp_2k1k_horizontal_total;
		1125,					//disp_2k1k_vertical_total;
		1308,					//disp_2k1k_vertical_total_50hz_min;
		1380,					//disp_2k1k_vertical_total_50hz_max;
		1100,					//disp_2k1k_vertical_total_60hz_min;
		1149,					//disp_2k1k_vertical_total_60hz_max;

		31, 			//disp_2k1k_hsync_width;
		5,						//disp_2k1k_vsync_length;
		140,					//disp_2k1k_den_sta_hpos;
		2060,					//disp_2k1k_den_end_hpos;
		16, 					//disp_2k1k_den_sta_vpos;
		1096,					//disp_2k1k_den_end_vpos;
		0,						//disp_2k1k_act_sta_hpos;
		1920,					//disp_2k1k_act_end_hpos;
		0,						//disp_2k1k_act_sta_vpos;
		1080,					//disp_2k1k_act_end_vpos;
		2200,			//disp_2k1k_hsync_lastline;
		0x01230123, 	//disp_2k1k_port_config1;
		0x01230123, 	//disp_2k1k_port_config2;
	}, // INX8901
	{
		2,				 //disp_2k1k_port;
		154,					 //disp_2k1k_clock_max;
		135,				 //disp_2k1k_clock_min;
		148*1000000,		 //disp_2k1k_clock_typical;
		2200,					//disp_2k1k_horizontal_total;
		1125,					//disp_2k1k_vertical_total;
		1308,					//disp_2k1k_vertical_total_50hz_min;
		1380,					//disp_2k1k_vertical_total_50hz_max;
		1100,					//disp_2k1k_vertical_total_60hz_min;
		1149,					//disp_2k1k_vertical_total_60hz_max;

		31, 			//disp_2k1k_hsync_width;
		5,						//disp_2k1k_vsync_length;
		140,					//disp_2k1k_den_sta_hpos;
		2060,					//disp_2k1k_den_end_hpos;
		16, 					//disp_2k1k_den_sta_vpos;
		1096,					//disp_2k1k_den_end_vpos;
		0,						//disp_2k1k_act_sta_hpos;
		1920,					//disp_2k1k_act_end_hpos;
		0,						//disp_2k1k_act_sta_vpos;
		1080,					//disp_2k1k_act_end_vpos;
		2200,			//disp_2k1k_hsync_lastline;
		0x32100000, 	//disp_2k1k_port_config1;
		0x00000000, 	//disp_2k1k_port_config2;
	}

};

#if 0
typedef enum _DCR_MODE{
    DCR_MASTER,
    DCR_SLAVE,
    DCR_MODE_MAX,
}DCR_MODE;
#endif
//#endif
extern void vbe_read_PMIC(void);
void Panel_InitParameter(PANEL_CONFIG_PARAMETER *parameter)
{
#if 0
    if (pPanelCOnfigParameter!=NULL){
        kfree(pPanelCOnfigParameter);
        pPanelCOnfigParameter=NULL;
    }
#endif

    if (parameter != NULL)
    {
        if(pPanelCOnfigParameter == NULL)
        {
            pPanelCOnfigParameter = (PANEL_CONFIG_PARAMETER *)kmalloc(sizeof(PANEL_CONFIG_PARAMETER), GFP_KERNEL);
        }

        if(pPanelCOnfigParameter)
        {
            memset(pPanelCOnfigParameter, 0, sizeof(PANEL_CONFIG_PARAMETER));
            memcpy(pPanelCOnfigParameter, parameter, sizeof(PANEL_CONFIG_PARAMETER));

            rtd_pr_vbe_info("[PANEL] sPanelName = %s\n", pPanelCOnfigParameter->sPanelName);
        }
    }

#ifndef BUILD_QUICK_SHOW
    //if(Get_PANEL_TYPE_SUPPORT_PMIC())
    //    vbe_read_PMIC();
#endif
}
void Panel_releaseParameter(void)
{
	if (pPanelCOnfigParameter!=NULL){
		kfree(pPanelCOnfigParameter);
		pPanelCOnfigParameter=NULL;
	}
}
void Panel_2k1k_table_sel(UINT8 table)
{
	if(table >= PANNEL_DYNAMIC_TABLE_MAX){
		rtd_pr_vbe_debug("Panel_2k1k_table_sel: Table Idx error \n");
		panel_sel_table = 0;
	}else{
		rtd_pr_vbe_debug("Panel_2k1k_table_sel: Table Idx (%d) \n", table);
		panel_sel_table = table;
	}
}

void Panel_force_set_oled_type(UINT8 oledType)
{
    force_oled_type = oledType;
}

UINT8 Panel_force_get_oled_type(void)
{
    return force_oled_type;
}

#ifdef ENABLE_FACTORY_PARTITION
unsigned char Panel_GetDefaultParameter(PANEL_CONFIG_PARAMETER* pParam)
{
	if (NULL == pParam)
		return false;

	pParam->iCONFIG_DISPLAY_PORT = CONFIG_DISPLAY_PORT;
	pParam->iCONFIG_DISPLAY_COLOR_BITS = CONFIG_DISPLAY_COLOR_BITS;
	pParam->iCONFIG_DISPLAY_EVEN_RSV1_BIT = CONFIG_DISPLAY_EVEN_RSV1_BIT;
	pParam->iCONFIG_DISPLAY_ODD_RSV1_BIT = CONFIG_DISPLAY_ODD_RSV1_BIT;
	pParam->iCONFIG_DISPLAY_BITMAPPING_TABLE = CONFIG_DISPLAY_BITMAPPING_TABLE;
	pParam->iCONFIG_DISPLAY_PORTAB_SWAP = CONFIG_DISPLAY_PORTAB_SWAP;

	pParam->iCONFIG_DISPLAY_RED_BLUE_SWAP = CONFIG_DISPLAY_RED_BLUE_SWAP;
	pParam->iCONFIG_DISPLAY_MSB_LSB_SWAP = CONFIG_DISPLAY_MSB_LSB_SWAP;
	pParam->iCONFIG_DISPLAY_SKEW_DATA_OUTPUT = CONFIG_DISPLAY_SKEW_DATA_OUTPUT;
	pParam->iCONFIG_DISPLAY_OUTPUT_INVERSE = CONFIG_DISPLAY_OUTPUT_INVERSE;
	pParam->iCONFIG_DISPLAY_VERTICAL_SYNC_NORMAL = CONFIG_DISPLAY_VERTICAL_SYNC_NORMAL;
	pParam->iCONFIG_DISPLAY_HORIZONTAL_SYNC_NORMAL = CONFIG_DISPLAY_HORIZONTAL_SYNC_NORMAL;
	pParam->iCONFIG_DISPLAY_RATIO_4X3 = CONFIG_DISPLAY_RATIO_4X3;

	pParam->iCONFIG_DISPLAY_CLOCK_MAX = CONFIG_DISPLAY_CLOCK_MAX;
	pParam->iCONFIG_DISPLAY_CLOCK_MIN = CONFIG_DISPLAY_CLOCK_MIN;
	pParam->iCONFIG_DISPLAY_REFRESH_RATE = CONFIG_DISPLAY_REFRESH_RATE;
	pParam->iCONFIG_DISPLAY_CLOCK_TYPICAL = CONFIG_DISPLAY_CLOCK_TYPICAL;

	pParam->iCONFIG_DISP_HORIZONTAL_TOTAL = CONFIG_DISP_HORIZONTAL_TOTAL;
	pParam->iCONFIG_DISP_VERTICAL_TOTAL = CONFIG_DISP_VERTICAL_TOTAL;

	pParam->iCONFIG_DISP_HSYNC_WIDTH = CONFIG_DISP_HSYNC_WIDTH;
	pParam->iCONFIG_DISP_VSYNC_LENGTH = CONFIG_DISP_VSYNC_LENGTH;

	pParam->iCONFIG_DISP_DEN_STA_HPOS = CONFIG_DISP_DEN_STA_HPOS;
	pParam->iCONFIG_DISP_DEN_END_HPOS = CONFIG_DISP_DEN_END_HPOS;
	pParam->iCONFIG_DISP_DEN_STA_VPOS = CONFIG_DISP_DEN_STA_VPOS;
	pParam->iCONFIG_DISP_DEN_END_VPOS = CONFIG_DISP_DEN_END_VPOS;

	pParam->iCONFIG_DISP_ACT_STA_HPOS = CONFIG_DISP_ACT_STA_HPOS;
	pParam->iCONFIG_DISP_ACT_END_HPOS = CONFIG_DISP_ACT_END_HPOS;
	pParam->iCONFIG_DISP_ACT_STA_VPOS = CONFIG_DISP_ACT_STA_VPOS;
	pParam->iCONFIG_DISP_ACT_END_VPOS = CONFIG_DISP_ACT_END_VPOS;

	pParam->iCONFIG_DISP_HSYNC_LASTLINE = CONFIG_DISP_HSYNC_LASTLINE;
	pParam->iCONFIG_DISP_DCLK_DELAY = CONFIG_DISP_DCLK_DELAY;

	pParam->_iCONFIG_DISP_ACT_STA_BIOS = _CONFIG_DISP_ACT_STA_BIOS;
	pParam->iCONFIG_DEFAULT_DPLL_M_DIVIDER = CONFIG_DEFAULT_DPLL_M_DIVIDER;
	pParam->iCONFIG_DEFAULT_DPLL_N_DIVIDER = CONFIG_DEFAULT_DPLL_N_DIVIDER;

	pParam->iPANEL_TO_LVDS_ON_ms = PANEL_TO_LVDS_ON_ms;
	pParam->iLVDS_TO_LIGHT_ON_ms = LVDS_TO_LIGHT_ON_ms;

	pParam->iLIGHT_TO_LDVS_OFF_ms = LIGHT_TO_LDVS_OFF_ms;
	pParam->iLVDS_TO_PANEL_OFF_ms = LVDS_TO_PANEL_OFF_ms;
	pParam->iPANEL_OFF_TO_ON_ms = PANEL_OFF_TO_ON_ms;

	pParam->iCONFIG_BACKLIGHT_PWM_FREQ = CONFIG_BACKLIGHT_PWM_FREQ;
	pParam->iCONFIG_BACKLIGHT_PWM_DUTY = CONFIG_BACKLIGHT_PWM_DUTY;
	pParam->iFIX_LAST_LINE_ENABLE = FIX_LAST_LINE_ENABLE;
	pParam->iFIX_LAST_LINE_4X_ENABLE = FIX_LAST_LINE_4X_ENABLE;
	pParam->iVFLIP = CONFIG_VFLIP_ON;
	pParam->iPICASSO_CONTROL_ON = CONFIG_PICASSO_CONTROL_ON;

	pParam->i3D_DISPLAY_SUPPORT = CONFIG_3D_DISPLAY_SUPPORT_ON;
	pParam->i3D_LINE_ALTERNATIVE_SUPPORT = CONFIG_3D_LINE_ALTERNATIVE_ON;
	pParam->i3D_PR_OUTPUT_LR_SWAP = CONFIG_3D_PR_OUTPUT_LR_SWAP;
	pParam->i3D_SG_OUTPUT_120HZ_ON = CONFIG_3D_SG_OUTPUT_120HZ_ON;
	pParam->i3D_SG_24HZ_OUTPUT_FHD_ON = CONFIG_3D_SG_24HZ_OUTPUT_FHD_ON;
	pParam->iSCALER_2D_3D_CVT_HWSHIFT_ENABLE = CONFIG_SCALER_2D_3D_CVT_HWSHIFT_ON;

	pParam->iCONFIG_PANEL_TYPE = CONFIG_PANEL_TYPE;
	pParam->iCONFIG_PANEL_CUSTOM_INDEX = CONFIG_PANEL_CUSTOM_INDEX;
#ifdef CONFIG_CUSTOMER_TV010
	pParam->iCONFIG_BACKLIGHT_DUTYMAX = 200;
	pParam->iCONFIG_BACKLIGHT_DUTYMIN = 100;
#endif
	strncpy(pParam->sPanelName, PANEL_NAME, 31);

/* not in PANEL_CONFIG_PARAMETER
#define CONFIG_DISP_VERTICAL_TOTAL_50Hz_MIN 	776
#define CONFIG_DISP_VERTICAL_TOTAL_50Hz_MAX 	836
#define CONFIG_DISP_VERTICAL_TOTAL_60Hz_MIN 	776
#define CONFIG_DISP_VERTICAL_TOTAL_60Hz_MAX 	836

 #define BACKLIGHTLEVEL_9WINDOWS				50
 #define BACKLIGHTLEVEL_NORMAL					100
*/
	return true;
}
#endif

PANEL_CONFIG_PARAMETER* Panel_GetPanelConfigParameter(void)
{
	return pPanelCOnfigParameter;
}


#ifdef ENABLE_FACTORY_PANEL_SETTING

void Panel_SyncDefaultParameter(void)
{
	PANEL_CONFIG_PARAMETER parameter;

	memset(&parameter,0,sizeof(PANEL_CONFIG_PARAMETER));
	parameter.iCONFIG_BACKLIGHT_PWM_DUTY=CONFIG_BACKLIGHT_PWM_DUTY;
	parameter.iCONFIG_BACKLIGHT_PWM_FREQ=CONFIG_BACKLIGHT_PWM_FREQ;
	parameter.iCONFIG_DEFAULT_DPLL_M_DIVIDER=CONFIG_DEFAULT_DPLL_M_DIVIDER;
	parameter.iCONFIG_DEFAULT_DPLL_N_DIVIDER=CONFIG_DEFAULT_DPLL_N_DIVIDER;
	parameter.iCONFIG_DISPLAY_BITMAPPING_TABLE=CONFIG_DISPLAY_BITMAPPING_TABLE;
	parameter.iCONFIG_DISPLAY_CLOCK_MAX=CONFIG_DISPLAY_CLOCK_MAX;
	parameter.iCONFIG_DISPLAY_CLOCK_TYPICAL=CONFIG_DISPLAY_CLOCK_TYPICAL;
	parameter.iCONFIG_DISPLAY_COLOR_BITS=CONFIG_DISPLAY_COLOR_BITS;
	parameter.iCONFIG_DISPLAY_EVEN_RSV1_BIT=CONFIG_DISPLAY_EVEN_RSV1_BIT;
#ifdef CONFIG_DISPLAY_HORIZONTAL_SYNC_NORMAL
	parameter.iCONFIG_DISPLAY_HORIZONTAL_SYNC_NORMAL=CONFIG_DISPLAY_HORIZONTAL_SYNC_NORMAL;
#endif
	parameter.iCONFIG_DISPLAY_MSB_LSB_SWAP=CONFIG_DISPLAY_MSB_LSB_SWAP;
	parameter.iCONFIG_DISPLAY_ODD_RSV1_BIT=CONFIG_DISPLAY_ODD_RSV1_BIT;
	parameter.iCONFIG_DISPLAY_OUTPUT_INVERSE=CONFIG_DISPLAY_OUTPUT_INVERSE;
	parameter.iCONFIG_DISPLAY_PORT=CONFIG_DISPLAY_PORT;
	parameter.iCONFIG_DISPLAY_PORTAB_SWAP=CONFIG_DISPLAY_PORTAB_SWAP;
	parameter.iCONFIG_DISPLAY_RATIO_4X3=CONFIG_DISPLAY_RATIO_4X3;
	//parameter.iCONFIG_DISPLAY_CLOCK_INVERSE=CONFIG_DISPLAY_CLOCK_INVERSE;
	parameter.iCONFIG_DISPLAY_RED_BLUE_SWAP=CONFIG_DISPLAY_RED_BLUE_SWAP;
	parameter.iCONFIG_DISPLAY_REFRESH_RATE=CONFIG_DISPLAY_REFRESH_RATE;
	parameter.iCONFIG_DISPLAY_SKEW_DATA_OUTPUT=CONFIG_DISPLAY_SKEW_DATA_OUTPUT;
#ifdef CONFIG_DISPLAY_VERTICAL_SYNC_NORMAL
	parameter.iCONFIG_DISPLAY_VERTICAL_SYNC_NORMAL=CONFIG_DISPLAY_VERTICAL_SYNC_NORMAL;
#endif
	parameter.iCONFIG_DISP_ACT_END_HPOS=CONFIG_DISP_ACT_END_HPOS;
	parameter.iCONFIG_DISP_ACT_END_VPOS=CONFIG_DISP_ACT_END_VPOS;
	parameter.iCONFIG_DISP_ACT_STA_HPOS=CONFIG_DISP_ACT_STA_HPOS;
	parameter.iCONFIG_DISP_ACT_STA_VPOS=CONFIG_DISP_ACT_STA_VPOS;
	parameter.iCONFIG_DISP_DCLK_DELAY=CONFIG_DISP_DCLK_DELAY;
	parameter.iCONFIG_DISP_DEN_END_HPOS=CONFIG_DISP_DEN_END_HPOS;
	parameter.iCONFIG_DISP_DEN_END_VPOS=CONFIG_DISP_DEN_END_VPOS;
	parameter.iCONFIG_DISP_DEN_STA_HPOS=CONFIG_DISP_DEN_STA_HPOS;
	parameter.iCONFIG_DISP_DEN_STA_VPOS=CONFIG_DISP_DEN_STA_VPOS;
	parameter.iCONFIG_DISP_HORIZONTAL_TOTAL=(CONFIG_DISP_HORIZONTAL_TOTAL & (~_BIT0)) ; //Dh total need as even
	parameter.iCONFIG_DISP_HSYNC_LASTLINE=CONFIG_DISP_HSYNC_LASTLINE;
	parameter.iCONFIG_DISP_HSYNC_WIDTH=CONFIG_DISP_HSYNC_WIDTH;
	parameter.iCONFIG_DISP_VERTICAL_TOTAL=CONFIG_DISP_VERTICAL_TOTAL;
	parameter.iCONFIG_DISP_VSYNC_LENGTH=CONFIG_DISP_VSYNC_LENGTH;
	parameter.iLIGHT_TO_LDVS_OFF_ms=LIGHT_TO_LDVS_OFF_ms;
	parameter.iLVDS_TO_LIGHT_ON_ms=LVDS_TO_LIGHT_ON_ms;
	parameter.iLVDS_TO_PANEL_OFF_ms=LVDS_TO_PANEL_OFF_ms;
	parameter.iPANEL_OFF_TO_ON_ms=PANEL_OFF_TO_ON_ms;
	parameter.iPANEL_TO_LVDS_ON_ms=PANEL_TO_LVDS_ON_ms;
	parameter._iCONFIG_DISP_ACT_STA_BIOS=_CONFIG_DISP_ACT_STA_BIOS;
	parameter.iFIX_LAST_LINE_ENABLE = FIX_LAST_LINE_ENABLE;
	parameter.iFIX_LAST_LINE_4X_ENABLE = FIX_LAST_LINE_4X_ENABLE;
	parameter.iVFLIP = CONFIG_VFLIP_ON;
	parameter.iPICASSO_CONTROL_ON = CONFIG_PICASSO_CONTROL_ON;

	parameter.iCONFIG_DISPLAY_PORT_CONFIG1 = CONFIG_DISPLAY_PORT_CONFIG1;
	parameter.iCONFIG_DISPLAY_PORT_CONFIG2 = CONFIG_DISPLAY_PORT_CONFIG2;
//#ifdef VBY_ONE_PANEL
	//#if defined(ENABLE_AOCUI)
	parameter.iCONFIG_SR_MODE = CONFIG_SR_MODE;
	parameter.iCONFIG_SR_PIXEL_MODE = CONFIG_SR_PIXEL_MODE;
	//#endif
	parameter.iCONFIG_SFG_SEG_NUM = CONFIG_SFG_SEG_NUM;
	parameter.iCONFIG_SFG_PORT_NUM = CONFIG_SFG_PORT_NUM;
//#endif

	parameter.iCONFIG_PANEL_TYPE = CONFIG_PANEL_TYPE;
	parameter.iCONFIG_PANEL_CUSTOM_INDEX = CONFIG_PANEL_CUSTOM_INDEX;
#ifdef CONFIG_CUSTOMER_TV010
	parameter.iCONFIG_BACKLIGHT_DUTYMAX = 200;
	parameter.iCONFIG_BACKLIGHT_DUTYMIN = 100;
#endif
	Panel_InitParameter(&parameter);
}
#endif


unsigned char Get_DISPLAY_Is_Dynamic_2D_3D_Timing(void)
{
	unsigned char result=0;

	return result;
}

unsigned char Get_DISPLAY_Is_Dynamic_DisplayTiming(void)
{
	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K_BOOTCODE_2K) ||
		(Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K_BOOTCODE_4K))
	{
		return TRUE;
	}
	else if(Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES()){
		return TRUE;
	}
	else{
		return FALSE;
	}

}

void Panel_Set_STR_BackLight_Ctrl_Mode(unsigned char enable)
{
	rtd_pr_vbe_notice("Panel_Set_STR_BackLight_Ctrl_Mode : %d\n",enable);

	if(enable == 1)
		panel_backlihgt_str_ctrl_mode = _BACKLIGHT_STR_CTRL_MODE_BY_AP;
	else
		panel_backlihgt_str_ctrl_mode = _BACKLIGHT_STR_CTRL_MODE_BY_KERNEL;
}

unsigned char Panel_Get_STR_BackLight_Ctrl_Mode(void)
{
	return panel_backlihgt_str_ctrl_mode;
}

// Support LVDS 2k1k output for 50/60Hz input source
unsigned char GET_DISPLAY_2K1K_OUTPUT_ENABLE(void)
{
	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K_BOOTCODE_2K) ||
		(Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K_BOOTCODE_4K))// 4k2k panel output lvds 2k1k
		return TRUE;

	return FALSE;
}


#define _DISPLAY_2K1K_WINDOW_MODE	0 // 0: 4k2k, 1: 2k1k in window display mode (Main page)
unsigned char Get_DISPLAY_2K1K_windows_mode(void)	 // 2k1k window display 4k2k resolution
{
	// Window mode
	if(Scaler_DispGetRatioMode() == SLR_RATIO_CUSTOM){
		StructSrcRect m_dispwin = Scaler_DispWindowGet();
		if((m_dispwin.src_wid<0xF00)||(m_dispwin.src_height<0x870)){
			return TRUE;
		}
	}
	return FALSE;
}

unsigned char Get_DISPLAY_2k1k_output_mode_status(void)
{
	unsigned char result=FALSE;

#ifdef CONFIG_VBY_ONE_M4_DEMO
	return FALSE; ///ben_demo1
#endif

	if(((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K_BOOTCODE_2K) ||
		(Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K_BOOTCODE_4K))	||
		(Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES() == 1))// 4k2k panel output lvds 2k1k
	{
		if(Scaler_DispGetInfoPtr() == NULL){
			rtd_pr_vbe_debug("\n[WARN] NULL PTR@%s\n", __FUNCTION__);
			return FALSE;
		}
		// Display output in 2k1k mode if IVS > 30Hz (4ch LVDS support timing up to 4k2k@30Hz)
		if( ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) < 10000 ) || ( Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000) >= V_FREQ_30000_pOFFSET ) ||
                        ((Scaler_InputSrcGetMainChType() != _SRC_HDMI)&&(Scaler_InputSrcGetMainChType() != _SRC_VO)&&(Scaler_InputSrcGetMainChType() != _SRC_YPBPR)))
			result = TRUE;
#ifndef BUILD_QUICK_SHOW
		else // 1080p24 / 1080p30 go 2k1k if go through I3DDMA or SG 2Dcvt3D
		if(((Scaler_DispGetInputInfo(SLR_INPUT_THROUGH_I3DDMA) == TRUE) || (drvif_scaler3d_decide_2d_cvt_3d_SG_frc_in_mDomain() == TRUE)) && drvif_scaelr3d_decide_is_3D_display_mode())
			result = TRUE;
#endif
	}

	return result;
}


unsigned int Get_DISPLAY_PORT(void)
{
	// LVDS 2k1k output mode
//	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_port;

	// 0: single port, 1: double port, 2: four port
	if (pPanelCOnfigParameter!=NULL)
		return (unsigned int)pPanelCOnfigParameter->iCONFIG_DISPLAY_PORT;

	return CONFIG_DISPLAY_PORT;
}
unsigned int Get_DISPLAY_COLOR_BITS(void)
{
	// 0: 30bits, 1:24 bits, 2: 18bits
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_COLOR_BITS;

 	return CONFIG_DISPLAY_COLOR_BITS;
}

unsigned int Get_DISPLAY_EVEN_RSV1_BIT(void)
{
	// 0: Indicate 0, 1: Indicate 1
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_EVEN_RSV1_BIT;

	return CONFIG_DISPLAY_EVEN_RSV1_BIT;
 }

unsigned int Get_DISPLAY_ODD_RSV1_BIT(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_ODD_RSV1_BIT;

	return CONFIG_DISPLAY_ODD_RSV1_BIT;
 } 	// 0: Indicate 0, 1: Indicate 1

unsigned int Get_DISPLAY_BITMAPPING_TABLE(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_BITMAPPING_TABLE;

	return CONFIG_DISPLAY_BITMAPPING_TABLE;
} // 0: Table1, 1:Table2


unsigned int Get_DISPLAY_PORTAB_SWAP(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_PORTAB_SWAP ;

	return CONFIG_DISPLAY_PORTAB_SWAP ; // inverse
}	// 0: No Swap, 1: Swap

unsigned int Get_DISPLAY_RED_BLUE_SWAP(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_RED_BLUE_SWAP;

	return CONFIG_DISPLAY_RED_BLUE_SWAP;
}	// 0: No Swap, 1: Swap

unsigned int Get_DISPLAY_MSB_LSB_SWAP(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_MSB_LSB_SWAP;

	return CONFIG_DISPLAY_MSB_LSB_SWAP;
}	// 0: No Swap, 1: Swap

unsigned int Get_DISPLAY_SKEW_DATA_OUTPUT(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_SKEW_DATA_OUTPUT;

	return CONFIG_DISPLAY_SKEW_DATA_OUTPUT;
} 	// 0: Disable, 1: Skew data output


unsigned int Get_DISPLAY_OUTPUT_INVERSE(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_OUTPUT_INVERSE;

	return CONFIG_DISPLAY_OUTPUT_INVERSE;
 }	// 0: No Swap, 1: Swap
/*
 unsigned int Get_DISPLAY_CLOCK_DELAY()
{
	// return 0;
	return CONFIG_DISPLAY_CLOCK_DELAY;
 }	// 0: No Swap, 1: Swap
*/
/*
 * Display Sync Output polarity
 */
//frank@0826 mark for panel parameter send from unzipper
//#if !CONFIG_PANEL_AUT315XW02 && !CONFIG_PANEL_AUT315XW01 && !CONFIG_PANEL_CMO_V315B3 && !CONFIG_PANEL_CMO_V315B1 && !CONFIG_PANEL_AUT32PV320TVM && !CONFIG_PANEL_AUT37PV370TVM && !CONFIG_PANEL_CMO_V420H1 && !CONFIG_PANEL_CMO_V420H1_L15 && !CONFIG_PANEL_LG_47 && !CONFIG_PANEL_LG_32 && !CONFIG_PANEL_SAMSUNG_32 && !CONFIG_PANEL_SAMSUNG_46 && !CONFIG_PANEL_HT_32 && !CONFIG_PANEL_CMO_V420B1_LH1 && !CONFIG_PANEL_CMO_V320B1L02 && !CONFIG_PANEL_CMO_V260B1L02 && !CONFIG_PANEL_CMO_V216 && !CONFIG_PANEL_LG_V420V02 && !CONFIG_PANEL_LG_420WUNSAA1 && !CONFIG_PANEL_CMO_V320B1L07 && !CONFIG_PANEL_CPT_32 && !CONFIG_PANEL_AU_T420HW02V4
unsigned int Get_DISPLAY_VERTICAL_SYNC_NORMAL(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_VERTICAL_SYNC_NORMAL;

 	return CONFIG_DISPLAY_VERTICAL_SYNC_NORMAL;
}

unsigned int Get_DISPLAY_HORIZONTAL_SYNC_NORMAL(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_HORIZONTAL_SYNC_NORMAL;

	return CONFIG_DISPLAY_HORIZONTAL_SYNC_NORMAL;
}
//#endif

unsigned int Get_DISPLAY_RATIO_4X3(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_RATIO_4X3;

	return CONFIG_DISPLAY_RATIO_4X3;
}

//frank@0826 mark for panel parameter send from unzipper
//#if !CONFIG_PANEL_LG_V420V02 && !CONFIG_PANEL_AU_M190PW01
unsigned int Get_DISPLAY_CLOCK_MAX(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
		//return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_clock_max;

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_CLOCK_MAX;

	return CONFIG_DISPLAY_CLOCK_MAX;
}
//#endif

unsigned int Get_DISPLAY_CLOCK_MIN(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
		//return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_clock_min;

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_CLOCK_MIN;

	return CONFIG_DISPLAY_CLOCK_MIN;
}

unsigned int Get_DISPLAY_HSR_MODE(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_HSR;
    }
    else
    {
        return 0;
    }
}

unsigned char Get_DISPLAY_HSR_ENABLE(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_HSR_ENABLE;
    }
    else
    {
        return 0;
    }
}

unsigned int Get_DISPLAY_HSR_SUPPORT_ALL_TIMING(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_HSR_SUPPORT_ALL_TIMING;
    }
    else
    {
        return 0;
    }
}

unsigned int Get_DISPLAY_HSR_DOWN_SAMPLE(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_HSR_DOWN_SAMPLE;
    }
    else
    {
        return 0;
    }
}

char *Get_DISPLAY_PANEL_NAME(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->sPanelName;
    }
    else
    {
        return "";
    }
}

unsigned char Get_DISPLAY_SSCG_ENABLE(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_SSCG_ENABLE;
    }
    else
    {
        return 0;
    }
}

unsigned int Get_DISPLAY_SSCG_PERCENT(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_SSCG_PERCENT;
    }
    else
    {
        return 0;
    }
}

unsigned int Get_DISPLAY_SSCG_PERIOD(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_SSCG_PERIOD;
    }
    else
    {
        return 0;
    }
}

unsigned int Get_DISPLAY_REFRESH_RATE(void)
{
	extern unsigned char vbe_disp_get_force_60hz_panel_type(void);

	if(vbe_disp_get_force_60hz_panel_type())
    {
		return 60;
	}

    if (pPanelCOnfigParameter!=NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_DISPLAY_REFRESH_RATE;
    }

	return CONFIG_DISPLAY_REFRESH_RATE;
}
#ifndef BUILD_QUICK_SHOW 
EXPORT_SYMBOL(Get_DISPLAY_REFRESH_RATE);
#endif

//#if !CONFIG_PANEL_AU170_480p && !CONFIG_PANEL_AUT315XW01 && !CONFIG_PANEL_AUT32PV320TVM && !CONFIG_PANEL_AUM24HW01 && !CONFIG_PANEL_CMO_V420H1 && !CONFIG_PANEL_LG_47 && !CONFIG_PANEL_LG_32 && !CONFIG_PANEL_SAMSUNG_32 && !CONFIG_PANEL_SAMSUNG_46 && !CONFIG_PANEL_HT_32 && !CONFIG_PANEL_CMO_V420B1_LH1 && !CONFIG_PANEL_AU_M220EW01 && !CONFIG_PANEL_CMO_V260B1L02 && !CONFIG_PANEL_CMO_V216 && !CONFIG_PANEL_LG_V420V02 && !CONFIG_PANEL_CMO_V320B1L07 && !CONFIG_PANEL_AU_M190PW01 && !CONFIG_PANEL_CPT_32 && !CONFIG_PANEL_MT_19AW01
unsigned int Get_DISPLAY_CLOCK_TYPICAL(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
		//return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_clock_typical;

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_CLOCK_TYPICAL;

	return CONFIG_DISPLAY_CLOCK_TYPICAL;
}
//#endif
#ifndef BUILD_QUICK_SHOW 
EXPORT_SYMBOL(Get_DISPLAY_CLOCK_TYPICAL);
#endif

#if 0
unsigned int Get_DISPLAY_CLOCK_INVERSE()
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_CLOCK_INVERSE;
	return CONFIG_DISPLAY_CLOCK_INVERSE;
}
#endif

unsigned int Get_DISPLAY_PORT_CONFIG1(void)
{

#ifdef CONFIG_VBY_ONE_M4_DEMO
	return 0x32100000;
#endif

#ifndef CONFIG_DISPLAY_PORT_CONFIG1	//for compatible with macAuthur and Magellan
	#if (CONFIG_DISPLAY_PORTAB_SWAP)
		#define CONFIG_DISPLAY_PORT_CONFIG1 0x10000000
	#else
		#define CONFIG_DISPLAY_PORT_CONFIG1 0x01000000
	#endif
#endif

	// LVDS 2k1k output mode
//	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_port_config1;

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_PORT_CONFIG1;

	return CONFIG_DISPLAY_PORT_CONFIG1;
}

unsigned int Get_DISPLAY_PORT_CONFIG2(void)
{
#ifndef CONFIG_DISPLAY_PORT_CONFIG2	//for compatible with macAuthur and Magellan
#define CONFIG_DISPLAY_PORT_CONFIG2 0
#endif

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_port_config2;

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISPLAY_PORT_CONFIG2;

	return CONFIG_DISPLAY_PORT_CONFIG2;
}

//#ifdef VBY_ONE_PANEL
//#if defined(ENABLE_AOCUI)
unsigned int Get_DISPLAY_SR_MODE(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_SR_MODE;

	return CONFIG_SR_MODE;
}

unsigned int Get_DISPLAY_SR_PIXEL_MODE(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_SR_PIXEL_MODE;

	return CONFIG_SR_PIXEL_MODE;
}
//#endif
unsigned int Get_DISPLAY_SFG_SEG_NUM(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_SFG_SEG_NUM;

	return CONFIG_SFG_SEG_NUM;
}

unsigned int Get_DISPLAY_SFG_PORT_NUM(void)
{
	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_SFG_PORT_NUM;

	return CONFIG_SFG_PORT_NUM;
}
//#endif
//extern int is_OLED_PANEL(void);
unsigned int Get_DISPLAY_PANEL_OLED_TYPE(void)
{

     if(Panel_force_get_oled_type()==1){
        rtd_pr_vbe_notice("[%s %d]force run oled panel\n", __FUNCTION__, __LINE__);
         return TRUE;
     }
#if 1 // use tool option to get oled

     if (pPanelCOnfigParameter!=NULL){
        if(pPanelCOnfigParameter->iCONFIG_PANEL_TYPE == P_VBY1_4K2K){
            return pPanelCOnfigParameter->iCONFIG_CUSTOMER_PANEL_ID;
        }else{
            return FALSE;
        }
     }
#endif
    //check oled panel type from hwopt 
    //if(is_OLED_PANEL()==TRUE)//strcmp(webos_strToolOption.eBackLight, "oled") == 0)
    //    return TRUE;

    return FALSE;
}


unsigned int Get_PANEL_iCONFIG_CUSTOMER_PANEL_ID(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_CUSTOMER_PANEL_ID;
	else
		return FALSE;
}

unsigned int Get_DISPLAY_PANEL_MPLUS_RGBW(void)
{
	 if (pPanelCOnfigParameter!=NULL){
        if(pPanelCOnfigParameter->iCONFIG_PANEL_TYPE == P_EPI_TYPE){
		    return FALSE;
	     }
		 else
	     	return FALSE;
	 }
	 return FALSE;
}

unsigned int Get_DISPLAY_PANEL_BOW_RGBW(void)
{
	if (pPanelCOnfigParameter!=NULL){
	   if(pPanelCOnfigParameter->iCONFIG_PANEL_TYPE == P_CEDS){
		   return FALSE;
		}
		else
		   return FALSE;
	}
	return FALSE;
}

unsigned int Get_DISPLAY_PANEL_TYPE(void)
{
 #ifdef CONFIG_VBY_ONE_M4_DEMO
            return P_VBY1_4K2K;
 #endif

    if (pPanelCOnfigParameter!=NULL)
            return pPanelCOnfigParameter->iCONFIG_PANEL_TYPE;

    return CONFIG_PANEL_TYPE;
}


unsigned int Get_DISPLAY_PANEL_CUSTOM_INDEX(void)
{
#ifdef CONFIG_VBY_ONE_M4_DEMO
	return VBY_ONE_PANEL_INX8901_4K2K_VBY1;
#endif

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_PANEL_CUSTOM_INDEX;

	return CONFIG_PANEL_CUSTOM_INDEX;
}


/*
 * Display total window setup
 */

unsigned int Get_DISP_HORIZONTAL_TOTAL(void)
{
#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		//UINT32 regVal = IoReg_Read32(TVE_SYNCGEN_P_reg);
		//return (TVE_SYNCGEN_P_get_h_thr(regVal)+1);
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_HTOTAL);
	}
#endif

#if defined(CONFIG_PANEL_lg_LC230EUE_SEA1)
	unsigned int freq;

	freq = Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000);
	if ((freq < 58000)&&(freq > 55000))
		return 2200;
	else if ((freq < 51000)&&(freq > 49000))
		return 2398;
//	else if ((freq < 26000)&&(freq > 23000))
//		return 2399;
	//else if (freq > 65000)
	//	return 2149;
	else if (freq > 65000)
		return 2242;
	else
	{
	// Issue description	:	0019759: [UI] (Degrade) OSD is not shown in HDMI 800x600 @ 60Hz
	// Issue severity level	:	***
	// Issue resolved name	:	LewisLee
	// Issue date			:	2012/06/30
//		return 2250;
		return (2246);
	}
#else //#if defined(CONFIG_PANEL_lg_LC230EUE_SEA1)
	#if 0
	//USER:LewisLee DATE:2012/06/26
	//apply it later
	if(_TRUE == Scaler_Get_DHtotal_ChangeFlag())
	{
		if(1 == Get_DISPLAY_PORT()) // dual port
			return (Scaler_Get_Dynamic_DHtotal_Value() & ~_BIT0) ;
		else// if(0 == Get_DISPLAY_PORT()) // dual port
			return (Scaler_Get_Dynamic_DHtotal_Value() | _BIT0) ;
	}
	#endif
    #ifdef CONFIG_ENABLE_FLL_DYNAMIC_ADJUST_DHTOTAL
	if(_ENABLE == Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_Enable_Flag())
	{
		if(0 == Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_CurrentValue())
		{
			//It means not change DH total
		}
		else// if(0 != Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_CurrentValue())
		{
			return Scaler_Get_EnterFRC_FLL_Dynamic_Adjust_DHTotoal_CurrentValue();
		}
	}
    #endif //#ifdef CONFIG_ENABLE_FLL_DYNAMIC_ADJUST_DHTOTAL

    #ifdef CONFIG_ENABLE_FRAMESYNC_MODE_DYNAMIC_ADJUST_DHTOTAL
	if(_ENABLE == Scaler_Get_FrameSync_Dynamic_Adjust_DHTotoal_Enable_Flag())
	{
		if(0 == Scaler_Get_FrameSync_Dynamic_Adjust_DHTotoal_CurrentValue())
		{
			//It means not change DH total
		}
		else// if(0 != Scaler_Get_FrameSync_Dynamic_Adjust_DHTotoal_CurrentValue())
		{
			return Scaler_Get_FrameSync_Dynamic_Adjust_DHTotoal_CurrentValue();
		}
	}
    #endif //#ifdef CONFIG_ENABLE_FRAMESYNC_MODE_DYNAMIC_ADJUST_DHTOTAL

    #if 1
	if(Get_DISPLAY_PORT() == 1|| Get_DISPLAY_PORT() == 2) // dual port
	{
		// LVDS 2k1k output mode
//		if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
		if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
			return (Panel_2k1k_common_param[panel_sel_table].disp_2k1k_horizontal_total & ~_BIT0);

		//Dual port need set as even value
		if (pPanelCOnfigParameter!=NULL)
			return ((pPanelCOnfigParameter->iCONFIG_DISP_HORIZONTAL_TOTAL) & ~_BIT0) ;
		else
			return ((CONFIG_DISP_HORIZONTAL_TOTAL) & ~_BIT0) ;
	}
	else// if(0 == Get_DISPLAY_PORT()) // single port
	{
		//Single port need set as odd value
		if (pPanelCOnfigParameter!=NULL)
			return ((pPanelCOnfigParameter->iCONFIG_DISP_HORIZONTAL_TOTAL) |_BIT0) ;
		else
			return ((CONFIG_DISP_HORIZONTAL_TOTAL) |_BIT0) ;
	}
    #else //#if 1
	//USER:LewisLee DATE:2012/06/21
	//it should apply at dual port panel
	//we apply at all panel
	if (pPanelCOnfigParameter!=NULL)
		return ((pPanelCOnfigParameter->iCONFIG_DISP_HORIZONTAL_TOTAL) & ~_BIT0) ;
	else
		return ((CONFIG_DISP_HORIZONTAL_TOTAL) & ~_BIT0) ;
    #endif //#if 1


#endif //#if defined(CONFIG_PANEL_lg_LC230EUE_SEA1)
}
#ifndef BUILD_QUICK_SHOW 
EXPORT_SYMBOL(Get_DISP_HORIZONTAL_TOTAL);
#endif
unsigned int Get_DISP_VERTICAL_TOTAL(void)
{

#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_VTOTAL);
	}
#endif

	//return  1065;
	//Setting value = (real value - 1)
#if defined(CONFIG_PANEL_lg_LC230EUE_SEA1)
	unsigned int freq;

	freq = Scaler_DispGetInputInfo(SLR_INPUT_V_FREQ_1000);
	if ((freq < 51000)&&(freq > 49000))
		return 1350;
//	else if ((freq < 26000)&&(freq > 23000))
//		return 1350;
	else if (freq > 65000)
		return 1100;
	else
		return 1118;
#else

	#if 0
	//USER:LewisLee DATE:2012/06/26
	//apply it later
	if(_TRUE == Scaler_Get_DVtotal_ChangeFlag())
	{
		return Scaler_Get_Dynamic_DVtotal_Value();
	}
	#endif
	// LVDS 2k1k output mode
//	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_vertical_total;

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_VERTICAL_TOTAL;

	return CONFIG_DISP_VERTICAL_TOTAL;
#endif
}
#ifndef BUILD_QUICK_SHOW 
EXPORT_SYMBOL(Get_DISP_VERTICAL_TOTAL);
#endif
/*
 * Display Sync Width setup
 */
unsigned int Get_DISP_HSYNC_WIDTH(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_hsync_width;

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_HSYNC_WIDTH;

	return CONFIG_DISP_HSYNC_WIDTH;
}

unsigned int Get_DISP_VSYNC_LENGTH(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_vsync_length;

	 if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_VSYNC_LENGTH;

	return CONFIG_DISP_VSYNC_LENGTH;
}

/*
 * Display Enable window setup
 */
unsigned int Get_DISP_DEN_STA_HPOS(void)
{
#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_HDENSTART);
	}
#endif

#if 1
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return (Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_sta_hpos & (~_BIT0));

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_HPOS&(~_BIT0);

	return (CONFIG_DISP_DEN_STA_HPOS&(~_BIT0));
#else //#if 1
	// [Code Sync][CSW][098/02/12][1]  Add protection for panel H DEN start to be odd value
	if (Get_DISPLAY_PORT() == 1){ // dual port
		 if (pPanelCOnfigParameter!=NULL)
			return pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_HPOS&(~_BIT0);
		else
			return (CONFIG_DISP_DEN_STA_HPOS&(~_BIT0));//return CONFIG_DISP_DEN_STA_HPOS;
	}else{
/*
		 if (pPanelCOnfigParameter!=NULL)
			return (((pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_HPOS) & ~(unsigned int)_BIT0) + 1);
		else
			return (((CONFIG_DISP_DEN_STA_HPOS) & ~(unsigned int)_BIT0) + 1);// & 0xfffffffe) + 1);
*/
#if defined(CONFIG_PANEL_AUT240XVN01)
		 if (pPanelCOnfigParameter!=NULL)
			return ((pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_HPOS) /*| _BIT0*/);
		else
			return ((CONFIG_DISP_DEN_STA_HPOS) /*| _BIT0*/);
#else
		 if (pPanelCOnfigParameter!=NULL)
			return ((pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_HPOS) | _BIT0);
		else
			return ((CONFIG_DISP_DEN_STA_HPOS) | _BIT0);
#endif
	}
#endif //#if 1
}

unsigned int Get_DISP_DEN_END_HPOS(void)
{
#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_HDENEND);
	}
#endif
#if 1
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return (Get_DISP_DEN_STA_HPOS() + Panel_2k1k_common_param[panel_sel_table].disp_2k1k_act_end_hpos);

	if (pPanelCOnfigParameter!=NULL)
		return (Get_DISP_DEN_STA_HPOS() + pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_HPOS)&(~_BIT0);

	return (Get_DISP_DEN_STA_HPOS() + CONFIG_DISP_ACT_END_HPOS)&(~_BIT0);
#else //#if 1
	// [Code Sync][CSW][098/02/12][1]  Add protection for panel H DEN start to be odd value
	if (Get_DISPLAY_PORT() == 1){ // dual port
		if (pPanelCOnfigParameter!=NULL)
			return (Get_DISP_DEN_STA_HPOS() + pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_HPOS)&(~_BIT0);
		return (Get_DISP_DEN_STA_HPOS() + CONFIG_DISP_ACT_END_HPOS)&(~_BIT0);
	}else{//frank@11152010 change code to solve 32" panel problem
/*
		if (pPanelCOnfigParameter!=NULL)
			return ((Get_DISP_DEN_STA_HPOS() + pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_HPOS)&(~_BIT0))+1;
		return ((Get_DISP_DEN_STA_HPOS() + CONFIG_DISP_ACT_END_HPOS)&(~_BIT0))+1;
*/
#if defined(CONFIG_PANEL_AUT240XVN01)
		if (pPanelCOnfigParameter!=NULL)
			return ((Get_DISP_DEN_STA_HPOS() + pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_HPOS) /*| _BIT0*/);
		return ((Get_DISP_DEN_STA_HPOS() + CONFIG_DISP_ACT_END_HPOS) /*| _BIT0*/);
#else
		if (pPanelCOnfigParameter!=NULL)
			return ((Get_DISP_DEN_STA_HPOS() + pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_HPOS) | _BIT0);
		return ((Get_DISP_DEN_STA_HPOS() + CONFIG_DISP_ACT_END_HPOS) | _BIT0);
#endif
	}
#endif //#if 1
}

unsigned int Get_DISP_DEN_STA_VPOS(void)
{

#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_VDENSTART);
	}
#endif

#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT
	UINT16 FrontPorch = 0;

	// LVDS 2k1k output mode
//	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status()){	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status()){

		FrontPorch = Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN() - Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_sta_vpos;

		if(FrontPorch < PANEL_FRONT_PORCH_MIN)
			return (Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_sta_vpos - (PANEL_FRONT_PORCH_MIN - FrontPorch));

		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_sta_vpos;
	}

	if (pPanelCOnfigParameter!=NULL)
	{
		FrontPorch = Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN() - pPanelCOnfigParameter->iCONFIG_DISP_DEN_END_VPOS;

		if(FrontPorch < PANEL_FRONT_PORCH_MIN)
			return pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_VPOS - (PANEL_FRONT_PORCH_MIN - FrontPorch);

		return pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_VPOS;
	}

	FrontPorch = Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN() - CONFIG_DISP_DEN_END_VPOS;

	if(FrontPorch < PANEL_FRONT_PORCH_MIN)
		return CONFIG_DISP_DEN_STA_VPOS - (PANEL_FRONT_PORCH_MIN - FrontPorch);

	return CONFIG_DISP_DEN_STA_VPOS;
#endif //#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT

#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT_BY_ADJUST_DENVPOS
	if(_ENABLE == Scaler_Get_Dynamic_Adjust_Display_DenVPos_Flag())
	{
		if((0 == Scaler_Get_Dynamic_Adjust_Display_DenVPos_Start())
			|| (0 == Scaler_Get_Dynamic_Adjust_Display_DenVPos_End()))
		{
			//use original setting
		}
		else
		{
			return Scaler_Get_Dynamic_Adjust_Display_DenVPos_Start();
		}
	}
#endif //#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT_BY_ADJUST_DENVPOS

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_sta_vpos;

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_DEN_STA_VPOS;

	return CONFIG_DISP_DEN_STA_VPOS;
}

unsigned int Get_DISP_DEN_END_VPOS(void)
{
#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_VDENEND);
	}
#endif

#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT
	UINT16 FrontPorch = 0;

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status()){	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status()){
		FrontPorch = Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN() - Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_end_vpos;

		if(FrontPorch < PANEL_FRONT_PORCH_MIN)
			return (Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_end_vpos - (PANEL_FRONT_PORCH_MIN - FrontPorch));

		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_end_vpos;
	}

	if (pPanelCOnfigParameter!=NULL)
	{
		FrontPorch = Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN() - pPanelCOnfigParameter->iCONFIG_DISP_DEN_END_VPOS;

		if(FrontPorch < PANEL_FRONT_PORCH_MIN)
			return pPanelCOnfigParameter->iCONFIG_DISP_DEN_END_VPOS - (PANEL_FRONT_PORCH_MIN - FrontPorch);

		return pPanelCOnfigParameter->iCONFIG_DISP_DEN_END_VPOS;
	}

	FrontPorch = Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN() - CONFIG_DISP_DEN_END_VPOS;

	if(FrontPorch < PANEL_FRONT_PORCH_MIN)
		return CONFIG_DISP_DEN_END_VPOS - (PANEL_FRONT_PORCH_MIN - FrontPorch);

	return CONFIG_DISP_DEN_END_VPOS;
#endif //#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT

#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT_BY_ADJUST_DENVPOS
	if(_ENABLE == Scaler_Get_Dynamic_Adjust_Display_DenVPos_Flag())
	{
		if((0 == Scaler_Get_Dynamic_Adjust_Display_DenVPos_Start())
			|| (0 == Scaler_Get_Dynamic_Adjust_Display_DenVPos_End()))
		{
			//use original setting
		}
		else
		{
			return Scaler_Get_Dynamic_Adjust_Display_DenVPos_End();
		}
	}
#endif //#ifdef CONFIG_ENABLE_PANEL_DISPLAY_PORCH_PROTECT_BY_ADJUST_DENVPOS

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_den_end_hpos;

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_DEN_END_VPOS;

	return CONFIG_DISP_DEN_END_VPOS;
}


/*
 * Display active window setup
 */
unsigned int Get_DISP_ACT_STA_HPOS(void)
{

#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return 0;
	}
#endif

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_act_sta_hpos;

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_ACT_STA_HPOS;

	return CONFIG_DISP_ACT_STA_HPOS;
}



unsigned int Get_DISP_ACT_END_HPOS_PRE(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_act_end_hpos;

	if(pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_HPOS;

	return CONFIG_DISP_ACT_END_HPOS;
}




unsigned int Get_DISP_ACT_END_HPOS(void)
{

#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_WID);
	}
#endif

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_act_end_hpos;

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_HPOS;

	return CONFIG_DISP_ACT_END_HPOS;
}

unsigned int Get_DISP_ACT_STA_VPOS(void)
{
#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return 0;
	}
#endif

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_act_sta_vpos;

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_ACT_STA_VPOS;

	return CONFIG_DISP_ACT_STA_VPOS;
}
unsigned int Get_DISP_ACT_END_VPOS(void)
{
#ifdef RUN_ON_TVBOX
	if(Get_DISPLAY_PANEL_TYPE() == P_DISP_TO_TVOX){
		return scalerdisplay_get_BoxDispInfo(_BOX_DISP_LEN);
	}
#endif

	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_act_end_vpos;

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_ACT_END_VPOS;

	return CONFIG_DISP_ACT_END_VPOS;
}

unsigned int Get_DISP_HSYNC_LASTLINE(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return (Panel_2k1k_common_param[panel_sel_table].disp_2k1k_hsync_lastline & ~_BIT0);

	if (pPanelCOnfigParameter!=NULL)
	{
		//Setting value = ( real value -1)
		return (pPanelCOnfigParameter->iCONFIG_DISP_HSYNC_LASTLINE & ~_BIT0);
	}

	return (CONFIG_DISP_HSYNC_LASTLINE & ~_BIT0);
}

unsigned int Get_DISP_DCLK_DELAY(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DISP_DCLK_DELAY;

	return CONFIG_DISP_DCLK_DELAY;
 }


unsigned int Get_DISP_ACT_STA_BIOS(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->_iCONFIG_DISP_ACT_STA_BIOS;

	return _CONFIG_DISP_ACT_STA_BIOS;
 }

unsigned int Get_DEFAULT_DPLL_M_DIVIDER(void)
{
#ifdef CONFIG_CUSTOMER_TV030
	return 0;
#else
	//0x68;  //hpwang 20071119 set default DCLK=(27*104/8)/2/2=87.75MHz
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DEFAULT_DPLL_M_DIVIDER;

	return CONFIG_DEFAULT_DPLL_M_DIVIDER;
#endif
}

unsigned int Get_DEFAULT_DPLL_N_DIVIDER(void)
{
#ifdef CONFIG_CUSTOMER_TV030
	return 0;
#else
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iCONFIG_DEFAULT_DPLL_N_DIVIDER;

	return CONFIG_DEFAULT_DPLL_N_DIVIDER;
#endif
}

unsigned int Get_PANEL_TO_LVDS_ON_ms(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iPANEL_TO_LVDS_ON_ms;

	return PANEL_TO_LVDS_ON_ms;
}


unsigned int Get_LVDS_TO_LIGHT_ON_ms(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iLVDS_TO_LIGHT_ON_ms;

	return LVDS_TO_LIGHT_ON_ms;
}


unsigned int Get_LIGHT_TO_LDVS_OFF_ms(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iLIGHT_TO_LDVS_OFF_ms;

	return LIGHT_TO_LDVS_OFF_ms;
}

unsigned int Get_LVDS_TO_PANEL_OFF_ms(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iLVDS_TO_PANEL_OFF_ms;

	return LVDS_TO_PANEL_OFF_ms;
}


unsigned int Get_PANEL_OFF_TO_ON_ms(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iPANEL_OFF_TO_ON_ms;

	return PANEL_OFF_TO_ON_ms;
}

unsigned int Get_PANEL_FIX_LAST_LINE_ENABLE(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iFIX_LAST_LINE_ENABLE;

	return FIX_LAST_LINE_ENABLE;
}


unsigned int Get_PANEL_FIX_LAST_LINE_4X_ENABLE(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iFIX_LAST_LINE_4X_ENABLE;

	return FIX_LAST_LINE_4X_ENABLE;
}
extern unsigned char Get_vbe_mirror_vflip(void);
unsigned char Get_PANEL_VFLIP_ENABLE(void)
{
	unsigned char enable=Get_vbe_mirror_vflip();
	if(enable!=2)
	{
		return enable;
	}
	else if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iVFLIP;
	return CONFIG_VFLIP_ON;
}

unsigned char Get_PANEL_PICASSO_ENABLE(void)
{
#ifdef CONFIG_CUSTOMER_TV030
	return 0;
#else
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->iPICASSO_CONTROL_ON;

	return CONFIG_PICASSO_CONTROL_ON;
#endif
}

unsigned char Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->i3D_DISPLAY_SUPPORT;

	return CONFIG_3D_DISPLAY_SUPPORT_ON;
}

unsigned char Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->i3D_LINE_ALTERNATIVE_SUPPORT;

	return CONFIG_3D_LINE_ALTERNATIVE_ON;
}

unsigned char Get_PANEL_3D_PR_OUTPUT_LR_SWAP(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->i3D_PR_OUTPUT_LR_SWAP;

	return CONFIG_3D_PR_OUTPUT_LR_SWAP;
}

unsigned char Get_PANEL_3D_SG_OUTPUT_120HZ_ENABLE(void)
{
	// [PA168] 2k1k 2D mode display in 60Hz
//	if(Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K){
		if(Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES()){
			if(Get_DISPLAY_2k1k_output_mode_status())
				return TRUE;
 			else
				return FALSE;
 		}else if(Get_DISPLAY_PANEL_CUSTOM_INDEX() == P_PA168_AU_55){
			if(Get_DISPLAY_2k1k_output_mode_status() && !drvif_scaelr3d_decide_is_3D_display_mode())
				return FALSE;
			else
				return TRUE;
		}

//	}

	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->i3D_SG_OUTPUT_120HZ_ON;

	return CONFIG_3D_SG_OUTPUT_120HZ_ON;
}

unsigned char Get_PANEL_3D_SG_24HZ_OUTPUT_FHD_ENABLE(void)
{
	if (pPanelCOnfigParameter!=NULL)
		return pPanelCOnfigParameter->i3D_SG_24HZ_OUTPUT_FHD_ON;

	return CONFIG_3D_SG_24HZ_OUTPUT_FHD_ON;
}


unsigned char Get_PANEL_TYPE_IS_IWATT_LOCAL_DIMMING(void)
{

	if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_2K1K) &&
		((Get_DISPLAY_PANEL_CUSTOM_INDEX() == P_IWATT7018_LG_42_SFK1)||
		(Get_DISPLAY_PANEL_CUSTOM_INDEX() == P_IWATT7018_LG_47_SFK1)))
		   return TRUE;
	else
		return FALSE;
}


// [PR to SG] for PR -> SG 3D TCON, 3D need timing free run@60Hz
unsigned char Get_PANEL_3D_PR_TO_SG_OUTPUT_FRC_ENABLE(void)
{
	if(Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE() && Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE()){
		//if(Get_DISPLAY_PANEL_SG_3D_IR_INDEX() == SG_3D_IR_CMI_V390HK1_LS6)
		//	return TRUE;
	}

	return FALSE;
}


UINT8 Get_PANEL_TYPE_SUPPORT_PMIC(void)
{
	if((Get_DISPLAY_PANEL_TYPE() == P_EPI_TYPE) ||
		(Get_DISPLAY_PANEL_TYPE() == P_CEDS) ||
		(Get_DISPLAY_PANEL_TYPE() == P_CSPI) ||
		(Get_DISPLAY_PANEL_TYPE() == P_CMPI) || 
		(Get_DISPLAY_PANEL_TYPE() == P_ISP))
		return TRUE;
	else
		return FALSE;
}


// =========add for PCID ================
unsigned char Get_PCID_ENABEL(void)
{
 #ifdef CONFIG_PCID_ENABEL
	   return CONFIG_PCID_ENABEL;
 #else
	   return 0; //default value
 #endif
}

// =========add for open cell panel od ================

unsigned char Get_OD_ENABEL(void)
{
 #ifdef CONFIG_OD_ENABEL
	   return CONFIG_OD_ENABEL;
 #else
	   return 0; //default value
 #endif
}

unsigned char Get_OD_MODE(void)
{
 #ifdef CONFIG_OD_MODE
   return CONFIG_OD_MODE;
 #else
   return 0; //default value
   #endif
}

unsigned char Get_OD_GAIN(void)
{
 #ifdef CONFIG_OD_GAIN
   return CONFIG_OD_GAIN;
 #else
   return 0;//default value
   #endif
}

unsigned char Get_OD_BITMODE(void)
{
 #ifdef CONFIG_OD_BITMODE
   return CONFIG_OD_BITMODE;
 #else
   return 0;//default value
   #endif
}

unsigned char Get_OD_COMP_MODE(void)
{
 #ifdef CONFIG_OD_COMP_MODE
   return CONFIG_OD_COMP_MODE;
 #else
   return 0;//default value
   #endif
}
unsigned char Get_PANEL_SCALER_2D_3D_CVT_HWSHIFT_ENABLE(void)
{
	if (pPanelCOnfigParameter!=NULL){
		return pPanelCOnfigParameter->iSCALER_2D_3D_CVT_HWSHIFT_ENABLE;
	}
	return CONFIG_SCALER_2D_3D_CVT_HWSHIFT_ON;
}
unsigned char Set_PANEL_3D_PIN(int enable)
{
#ifdef CONFIG_PANEL_auo_TPT420H2HVD01C1B_3DPR
	if (enable)
	{
		IO_Direct_Set("PIN_EN_3D", 1);
	}
	else
	{
		IO_Direct_Set("PIN_EN_3D", 0);
	}
#elif defined(CONFIG_PANEL_CMI_V390HK1_LS6)
	IO_Set(PIN_2D_3D_SELECT, enable!=0);
#endif
	return TRUE;
}
unsigned int Get_PANEL_DISP_VERTICAL_TOTAL_50Hz_MIN(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_vertical_total_50hz_min;

	if (pPanelCOnfigParameter!=NULL){
		return pPanelCOnfigParameter->iCONFIG_DISP_VERTICAL_TOTAL_50Hz_MIN;
	}
	return CONFIG_DISP_VERTICAL_TOTAL_50Hz_MIN;
}

unsigned int Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MIN(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_vertical_total_60hz_min;

	if (pPanelCOnfigParameter!=NULL){
		return pPanelCOnfigParameter->iCONFIG_DISP_VERTICAL_TOTAL_60Hz_MIN;
	}
	return CONFIG_DISP_VERTICAL_TOTAL_60Hz_MIN;
}

unsigned int Get_PANEL_DISP_VERTICAL_TOTAL_50Hz_MAX(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_vertical_total_50hz_max;

	if (pPanelCOnfigParameter!=NULL){
		return pPanelCOnfigParameter->iCONFIG_DISP_VERTICAL_TOTAL_50Hz_MAX;
	}
	return CONFIG_DISP_VERTICAL_TOTAL_50Hz_MAX;
}

unsigned int Get_PANEL_DISP_VERTICAL_TOTAL_60Hz_MAX(void)
{
	// LVDS 2k1k output mode
	//if((Get_DISPLAY_PANEL_TYPE() == P_LVDS_4K_2K) && Get_DISPLAY_2k1k_output_mode_status())	// 4k2k panel output lvds 2k1k
	if(Get_DISPLAY_Is_Dynamic_DisplayTiming() && Get_DISPLAY_2k1k_output_mode_status())
		return Panel_2k1k_common_param[panel_sel_table].disp_2k1k_vertical_total_60hz_max;

	if (pPanelCOnfigParameter!=NULL){
		return pPanelCOnfigParameter->iCONFIG_DISP_VERTICAL_TOTAL_60Hz_MAX;
	}
	return CONFIG_DISP_VERTICAL_TOTAL_60Hz_MAX;
}
#if 0
unsigned char Get_PANEL_3D_TIMING_TYPE(void)
{

	if (pPanelCOnfigParameter!=NULL){
		return pPanelCOnfigParameter->i3D_DISPLAY_TIMING;
	}
	return CONFIG_PANEL_3D_TIMING;
}
#endif

unsigned char Get_TIMING_PROTECT_PANEL_TYPE(void)
{
#if 0
	extern unsigned char V4l2_memc_motion_pro;

//	rtd_pr_vbe_notice("[%s] V4l2_memc_motion_pro=%d \n", __FUNCTION__, V4l2_memc_motion_pro);
	if ((webos_tooloption.eModelModuleType == module_LGD) && (webos_tooloption.eBackLight == direct ) && (webos_tooloption.eLEDBarType == local_dim_block_32))
	{
//		rtd_pr_vbe_notice("[%s] LD32 Panel.\n", __FUNCTION__);
		return 1;
	}
	else if((V4l2_memc_motion_pro) && (strcmp(webos_strToolOption.eBackLight, "oled") == 0))
	{
//		rtd_pr_vbe_notice("[%s] OLED Motion Pro Panel.\n", __FUNCTION__);
		return 1;
	}
	else{
		return 0;
	}
#else
	return 0;
#endif
}


// 3D panel type: 0:2D only, 1:SG, 2:PR
unsigned char Get_PANEL_3D_PANEL_TYPE(void)
{
	if(!Get_PANEL_3D_DISPLAY_SUPPORT_ENABLE())
		return 0;
	else if(Get_PANEL_3D_LINE_ALTERNATIVE_SUPPORT_ENABLE())
		return 2;
	else
		return 1;
}

unsigned char Get_PANEL_TYPE_IS_USED_TCON_CONTROL(void)
{
	if(Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES()){
		return 1;
	}
	else if(Get_DISPLAY_Is_Dynamic_DisplayTiming()){
		return 1;
	}
	else
		return 0;
}

unsigned char Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES(void)
{
	if((Get_DISPLAY_PANEL_CUSTOM_INDEX() >=  DYNAMIC_OUTPUT_PANEL_SERIES_BEGIN) &&
		   (Get_DISPLAY_PANEL_CUSTOM_INDEX() < DYNAMIC_OUTPUT_PANEL_SERIES_MAX))
		return 1;
	else
		return 0;
}
unsigned char Get_PMIC_Name(unsigned char *name)
{
    if(name != NULL)
        strncpy(name, pPanelCOnfigParameter->sPmicName, PMIC_NAME_SIZE);
    else
        return 0;
    return 1;
}
unsigned char Get_PMIC_I2C_PORT(void)
{
    if(pPanelCOnfigParameter != NULL)
        return pPanelCOnfigParameter->iCONFIG_PMIC_I2C_PORT;
    return 0;
}
unsigned char Get_PMIC_BIN_SIZE(void)
{
    if(pPanelCOnfigParameter != NULL)
        return pPanelCOnfigParameter->iCONFIG_PMIC_BIN_SIZE;
    return 0;
}

unsigned char Get_PMIC_I2c_Addr(void)
{
    if (pPanelCOnfigParameter != NULL){
        return pPanelCOnfigParameter->iCONFIG_PMIC_I2C_ADDR;
    }
    return 0;
}

unsigned char Panel_SetBackLightMode(unsigned char enable)
{
	int pinindex = 0;
	int ret = 0;
	int invert = 0;
	RTK_GPIO_ID s_gpio;
	unsigned long long param;

	if (pcb_mgr_get_enum_info_byname("PIN_BL_ON_OFF", &param) == 0)
	{
		if (GET_PIN_TYPE(param) == PCB_PIN_TYPE_ISO_GPIO) {
			pinindex = GET_PIN_INDEX(param);
			invert = GET_GPIO_INVERT(param);
			enable = (invert) ? (!enable) : (enable);
			s_gpio = rtk_gpio_id(ISO_GPIO, pinindex);

			rtd_pr_vbe_warn("ISO_GPIO gpio_num:%d\n",pinindex);
			rtd_pr_vbe_warn("enable:%d\n",enable);

			ret = rtk_gpio_output(s_gpio, enable);
			rtk_gpio_set_dir(s_gpio,1);

			if (ret < 0)
				return 0;
		}
		else if (GET_PIN_TYPE(param) == PCB_PIN_TYPE_GPIO) {
			pinindex = GET_PIN_INDEX(param);
			invert = GET_GPIO_INVERT(param);
			enable = (invert) ? (!enable) : (enable);
			s_gpio = rtk_gpio_id(MIS_GPIO, pinindex);

			rtd_pr_vbe_warn("MISC_GPIO gpio_num:%d\n",pinindex);

			ret = rtk_gpio_output(s_gpio, enable);
			rtk_gpio_set_dir(s_gpio,1);

			if (ret < 0)
				return 0;
		}
	}
	return 1;
}


extern UINT8 Scaler_MEMC_GetDynamicOnOff(void);
unsigned char Get_MEMC_Enable_Dynamic(void)
{
	return 1;
/*
#ifndef CONFIG_MEMC_BYPASS
	if (pPanelCOnfigParameter!=NULL){
		if((pPanelCOnfigParameter->iCONFIG_PANEL_TYPE == P_VBY1_4K2K) ||
			(pPanelCOnfigParameter->iCONFIG_PANEL_TYPE == P_EPI_TYPE) ||
			(pPanelCOnfigParameter->iCONFIG_PANEL_TYPE == P_VBY1_TO_HDMI)){
#ifdef MEMC_DYNAMIC_ONOFF
				if((IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg) & _BIT1) && (Scaler_MEMC_GetDynamicOnOff() == 1))
					return 0;
				else if(!(IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_0_reg) & _BIT1) && (Scaler_MEMC_GetDynamicOnOff() == 0))
					return 1;
				else
#endif
					return 1;
			}
		else
			return 0;
	}
	return 0;
#else
	return 0;
#endif
*/

}

unsigned char Get_PANEL_DITHER_ENABLE(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_DITHER_ENABLE;
    }

    return 0;
}

unsigned char Get_PANEL_PANEL_DITHER_TEMPORAL_ENABLE(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_DITHER_TEMPORAL_ENABLE;
    }

    return 0;
}

unsigned char Get_PANEL_PANEL_DITHER_BIT_TABLE_SELECT(void)
{
    if(pPanelCOnfigParameter  != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_DITHER_BIT_TABLE_SELECT;
    }

    return 0;
}

unsigned char Get_PANEL_PANEL_DITHER_OLED_TH_EN(void)
{
    if(pPanelCOnfigParameter  !=  NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_DITHER_OLED_TH_EN;
    }

    return 0;
}

unsigned short Get_PANEL_PANEL_DITHER_OLED_TH(void)
{
    if(pPanelCOnfigParameter != NULL)
    {
        return pPanelCOnfigParameter->iCONFIG_PANEL_DITHER_OLED_TH;
    }

    return 0;
}


#if 0
#define MAX_ACT_LEVEL	0xFF

#if !defined(TV013UI_1) && !defined(TV010UI_1) // yf 20110704

unsigned int Panel_GetBackLightLevel(void)
{

#if defined(BUILD_TV013_1_ATV) || defined(BUILD_SIRIUS_TV013_1)	//zhongju 2012-06-15
	if(m_isIncreaseMode)
		return m_origbacklightLevel*100/255;
	else
		return (100-m_origbacklightLevel*100/255);
#else

	return Scaler_GetBacklight();
#endif

}
#endif

#if  defined(BUILD_TV030_2)
unsigned char FAC_SetBackLightLevel(unsigned int level)
{
	rtd_pr_vbe_info("\033[1;31m FAC_SetBackLightLevel(unsigned int level) level = %d \033[m\n",level);

	unsigned int mappingValue;


	UINT16 backlight_level;
	if(m_blockbacklight)
		return TRUE;
	StructColorDataFacModeType *pColorFacTable;

	if(level > 100)
		level = 100;

	pColorFacTable = fwif_color_get_color_fac_mode(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_DATA_ARRAY_IDX), 0);
	rtd_pr_vbe_info("\n AAAA====  \n");
	backlight_level = level;

	if (pColorFacTable != NULL)
	{
		backlight_level = fw_OsdMapToRegValue(SLR_MAIN_DISPLAY, 5, level);
		rtd_pr_vbe_info("\n AAAA==== backlight_level = %d \n",backlight_level);
		level = backlight_level;

		if (m_isIncreaseMode)
			mappingValue = backlight_level;
		else
			mappingValue = 255 - backlight_level;

		mappingValue &= MAX_ACT_LEVEL;
	}

#ifdef ENABLE_DCR
       INT8 ret=0;
#endif




//	IoGpio_SetPwmDuty(PIN_BL_ADJ, mappingValue);
#ifdef PIN_BL_ADJ
	IO_PWM_SetDuty(PIN_BL_ADJ, mappingValue);
#endif
	m_origbacklightLevel = mappingValue;

	// back light on
//	IoGpio_SetPinLevel(PIN_BL_ON_OFF, _BACKLIGHT_ON);
#if !defined(BUILD_TV057_1_ATV)
#ifdef PIN_BL_ON_OFF
	#if defined TV015_216_ATV_V1 //eric 20120615
		#ifdef TV015_216_ATV_ALLINONE
			IO_Direct_Set("PIN_BL_ON_OFF", 1);	//All_In_One
  		#else
			IO_Direct_Set("PIN_BL_ON_OFF", 0);
  		#endif
	#else
		IO_Set(PIN_BL_ON_OFF, 1);
	#endif

#endif
#endif
	//IO_Set

#ifdef ENABLE_DCR
//fix me

       //Leo Chen+ for DCR
#ifdef CONFIG_PCBMGR
	if (m_PIN_BL_ADJ < 0) {
		unsigned long long value = 0;
		int index, type;
		if (pcb_mgr_get_enum_info_byname("PIN_BL_ADJ", &value) < 0) {
			m_PIN_BL_ADJ = -1;
			//rtd_pr_vbe_info("%s %d\n", __FUNCTION__, __LINE__);
		} else {
			if (PCB_PIN_TYPE_ISO_GPIO == GET_PIN_TYPE(value) || PCB_PIN_TYPE_ISO_PWM==GET_PIN_TYPE(value) || PCB_PIN_TYPE_ISO_UNIPWM==GET_PIN_TYPE(value) )
				type = 1;
			else
				type = 0;
			index = GET_PIN_INDEX(value);
			m_PIN_BL_ADJ =  ((type & 0xFFFF) << 16) | (index & 0xFFFF);
			//rtd_pr_vbe_info("%s %d, index=%d\n", __FUNCTION__, __LINE__, index);

		}
	}

#endif

       //m_PIN_BL_ON_OFF=PIN_BL_ON_OFF; //mark for compiler error
       //m_BACKLIGHT_ON=_BACKLIGHT_ON; //mark for compiler error
       m_backlight_level_from_user=level;



	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_PIN_BL_ADJ, m_PIN_BL_ADJ);
       if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ADJ\n" );
		return FALSE;
	}
	*/
		if (m_PIN_BL_ADJ >= 0) {
			if (0 != (ret = Scaler_SendRPC(SCALERIOC_PIN_BL_ADJ, m_PIN_BL_ADJ, RPC_SEND_VALUE_ONLY)))
			{
				rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ADJ\n" );
				return FALSE;

			}
		}
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_IS_INCREASE_MODE, m_isIncreaseMode, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_isIncreaseMode\n" );
			return FALSE;
		}

		if (0 != (ret = Scaler_SendRPC(SCALERIOC_BL_LV_FROM_USER, m_backlight_level_from_user, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlight_level_from_user\n" );
			return FALSE;
		}


       //DCR end
#endif

	if(m_callbackSetPwmDuty != NULL)
		m_callbackSetPwmDuty(mappingValue);


	return TRUE;

}
#endif

#if defined(TV003_ADTV)
unsigned char Panel_SetBackLightLevel(BACKLIGHT_MODE mode, unsigned int level)
{
    if (mode == BACKLIGHT_DCR)
    {
        rtd_pr_vbe_info("\033[1;37;46m This project does not support %d backlight mode!\033[m\n", mode);
        return FALSE;
    }

    if(m_callbackSetPwmDuty == NULL)
    {
#if defined(BOARD_ID_RTD2986IK_TV003_ATSC_V1) && defined(ENABLE_TCON_MODULE)
        IO_PWM_SetDuty(PIN_DIMMER, level);
#else
#ifdef PIN_BL_ADJ
        IO_PWM_SetDuty(PIN_BL_ADJ, level);
#endif
#endif
    }
    else
        m_callbackSetPwmDuty(level);

    return TRUE;
}
#else
unsigned char Panel_SetBackLightLevel(BACKLIGHT_MODE mode, unsigned int level)
{
	unsigned int mappingValue;
	//unsigned int address,shiftBits = 0;
	//unsigned int andMask,orMask ;

	if(m_blockbacklight)	return TRUE;
#ifndef  BUILD_TV030_2
	if(Get_PANEL_TYPE_IS_IWATT_LOCAL_DIMMING())
	{
		unsigned char i;
		unsigned int bri_buf[18];

//		ld_Init();

		bri_buf[0] = 0x00001000;
		bri_buf[1] = 7;
		rtd_pr_vbe_info("\n Back Light Level = %d  ", level);
		for(i = 2 ; i <= 17 ; i++)
		{
			bri_buf[i] = 0xfff - ((100 - level)*32);
		}

		ld_Write(&bri_buf[0], 18, LD_WRITE_MODE_FOLLOW_VSYNC);	//LD_WRITE_MODE_NOT_FOLLOW_VSYNC

#if 0
			rtd_pr_vbe_info("\n  BBL[1] = %x  \n", ld_Read(0x0004));
			rtd_pr_vbe_info("BBL[2] = %x  \n", ld_Read(0x0008));
			rtd_pr_vbe_info("BBL[3] = %x  \n", ld_Read(0x000c));
			rtd_pr_vbe_info("BBL[4] = %x  \n", ld_Read(0x0010));
			rtd_pr_vbe_info("BBL[5] = %x  \n", ld_Read(0x0014));
			rtd_pr_vbe_info("BBL[6] = %x  \n", ld_Read(0x0018));
			rtd_pr_vbe_info("BBL[7] = %x  \n", ld_Read(0x001c));
			rtd_pr_vbe_info("BBL[8] = %x  \n", ld_Read(0x0020));
			rtd_pr_vbe_info("BBL[9] = %x  \n", ld_Read(0x0024));
			rtd_pr_vbe_info("BBL[10] = %x  \n", ld_Read(0x0028));
			rtd_pr_vbe_info("BBL[11] = %x  \n", ld_Read(0x002c));
			rtd_pr_vbe_info("BBL[12] = %x  \n", ld_Read(0x0030));
			rtd_pr_vbe_info("BBL[13] = %x  \n", ld_Read(0x0034));
			rtd_pr_vbe_info("BBL[14] = %x  \n", ld_Read(0x0038));
			rtd_pr_vbe_info("BBL[15] = %x  \n", ld_Read(0x003c));
			rtd_pr_vbe_info("BBL[16] = %x  \n", ld_Read(0x0040));
#endif
		return TRUE;
	}
#endif

#ifdef  BUILD_TV030_2
	UINT16 backlight_level;
	short startPoint = 0, endPoint = 0;
	StructColorDataFacModeType *pColorFacTable;
	pColorFacTable = fwif_color_get_color_fac_mode(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_DATA_ARRAY_IDX), 0);
	backlight_level = level;
	if (pColorFacTable != NULL)
	{
		backlight_level = fw_OsdMapToRegValue(SLR_MAIN_DISPLAY, FAC_BACKLIGHT, backlight_level);
		level = backlight_level;
	}
#else

	UINT16 backlight_level;
	short startPoint = 0, endPoint = 0;
	StructColorDataFacModeType *pColorFacTable;
	pColorFacTable = fwif_color_get_color_fac_mode(Scaler_DispGetInputInfoByDisp((SCALER_DISP_CHANNEL)SLR_MAIN_DISPLAY,SLR_INPUT_DATA_ARRAY_IDX), 0);
	backlight_level = level;

#if  defined(TV020_STYLE_PQ)||defined(BUILD_TV030_2)
	if (pColorFacTable != NULL)
	{
		backlight_level = fw_OsdMapToRegValue(SLR_MAIN_DISPLAY, FAC_BACKLIGHT, backlight_level);
		level = backlight_level;
	}

#else

	if (pColorFacTable != NULL){
		if (backlight_level < 50)
		{
			startPoint = pColorFacTable->Backlight_0;
			endPoint = pColorFacTable->Backlight_50;
		}
		else
		{
			startPoint = pColorFacTable->Backlight_50;
			endPoint = pColorFacTable->Backlight_100;
			backlight_level -= 50;
		}
		//rtd_pr_vbe_info("Panel_SetBackLightLevel:::level=%d\n", level);
		//rtd_pr_vbe_info("Panel_SetBackLightLevel:::Backlight_0=%d\n", pColorFacTable->Backlight_0);
		//rtd_pr_vbe_info("Panel_SetBackLightLevel:::Backlight_50=%d\n", pColorFacTable->Backlight_50);
		//rtd_pr_vbe_info("Panel_SetBackLightLevel:::Backlight_100=%d\n", pColorFacTable->Backlight_100);
		backlight_level = startPoint + ((endPoint-startPoint)*backlight_level/50);
		level = backlight_level;
		//rtd_pr_vbe_info("Panel_SetBackLightLevel:::level=%d\n", level);
	}
#endif

#endif

#ifdef ENABLE_DCR
       INT8 ret=0;
#endif

#if  defined(BUILD_TV030_2)
	mappingValue = backlight_level;
#else
	//Scaler_SetBackLight((UINT8)level); //yf 20110704
	if (mode == BACKLIGHT_USER)
	{
		if ((level > m_backlightLevelUiMax) || (level < m_backlightLevelUiMin))
			return FALSE;

		// 0.mapping
		if (m_isIncreaseMode)
			mappingValue = (level-m_backlightLevelUiMin)*(m_backlightLevelActMax - m_backlightLevelActMin)/(m_backlightLevelUiMax-m_backlightLevelUiMin) + m_backlightLevelActMin;
		else
			mappingValue = m_backlightLevelActMax - (level-m_backlightLevelUiMin)*(m_backlightLevelActMax - m_backlightLevelActMin)/(m_backlightLevelUiMax-m_backlightLevelUiMin);

		mappingValue &= MAX_ACT_LEVEL;
		//rtd_pr_vbe_info("m_backlightLevelUiMax = %x\n", m_backlightLevelUiMax);
		//rtd_pr_vbe_info("m_backlightLevelUiMin = %x\n", m_backlightLevelUiMin);
		//rtd_pr_vbe_info("m_backlightLevelActMax = %x\n", m_backlightLevelActMax);
		//rtd_pr_vbe_info("m_backlightLevelActMin = %x\n", m_backlightLevelActMin);
	}
	else if(mode == BACKLIGHT_OPC)
	{
		// KWarning: checked ok by ghyu
		if ((level > BL_LEVEL_OPC_MAX_DEFAULT) || (level < BL_LEVEL_OPC_MIN_DEFAULT))
			return FALSE;

		// 0.mapping
		if (m_isIncreaseMode)
			mappingValue = (level-BL_LEVEL_OPC_MIN_DEFAULT)*(m_backlightLevelActMax - m_backlightLevelActMin)/(BL_LEVEL_OPC_MAX_DEFAULT-BL_LEVEL_OPC_MIN_DEFAULT) + m_backlightLevelActMin;
		else
			mappingValue = m_backlightLevelActMax - (level-BL_LEVEL_OPC_MIN_DEFAULT)*(m_backlightLevelActMax - m_backlightLevelActMin)/(BL_LEVEL_OPC_MAX_DEFAULT-BL_LEVEL_OPC_MIN_DEFAULT);

		mappingValue &= MAX_ACT_LEVEL;
	}
	else
	{
		// KWarning: checked ok by ghyu
		if ((level > BL_LEVEL_DCR_MAX_DEFAULT) || (level < BL_LEVEL_DCR_MIN_DEFAULT))
			return FALSE;

		// 0.mapping
		if (m_isIncreaseMode)
			mappingValue = (level-BL_LEVEL_DCR_MIN_DEFAULT)*(m_backlightLevelActMax - m_backlightLevelActMin)/(BL_LEVEL_DCR_MAX_DEFAULT-BL_LEVEL_DCR_MIN_DEFAULT) + m_backlightLevelActMin;
		else
			mappingValue = m_backlightLevelActMax - (level-BL_LEVEL_DCR_MIN_DEFAULT)*(m_backlightLevelActMax - m_backlightLevelActMin)/(BL_LEVEL_DCR_MAX_DEFAULT-BL_LEVEL_DCR_MIN_DEFAULT);

		mappingValue &= MAX_ACT_LEVEL;
	}

	//rtd_pr_vbe_info("level=%x\n", level);
	//rtd_pr_vbe_info("mapping=%x\n", mappingValue);

#ifdef ENABLE_DCR
	//rtd_pr_vbe_info("panelAPI\n");
#endif

#endif
//	IoGpio_SetPwmDuty(PIN_BL_ADJ, mappingValue);
#ifdef PIN_BL_ADJ
#if defined(BUILD_TV005_1_ATV) || defined(BUILD_TV057_1_ATV)||defined(BUILD_TV005_1_ISDB)
	if(m_callbackSetPwmDuty == NULL)
	{
		#if defined(BUILD_TV057_1_ATV)
		//IO_PWM_SetDuty(PIN_BL_ADJ, mappingValue);
		#else
		IO_PWM_SetDuty(PIN_BL_ADJ, mappingValue);
		#endif
	}
#else	//BUILD_TV005_1_ATV
	IO_PWM_SetDuty(PIN_BL_ADJ, mappingValue);
#endif	//BUILD_TV005_1_ATV
#endif
	m_origbacklightLevel = mappingValue;

	// back light on
//	IoGpio_SetPinLevel(PIN_BL_ON_OFF, _BACKLIGHT_ON);
#if !defined(BUILD_TV057_1_ATV)
#ifdef PIN_BL_ON_OFF
	#if defined TV015_216_ATV_V1 //eric 20120615
		#ifdef TV015_216_ATV_ALLINONE
			IO_Direct_Set("PIN_BL_ON_OFF", 1);	//All_In_One
  		#else
			IO_Direct_Set("PIN_BL_ON_OFF", 0);
  		#endif
	#else
		IO_Set(PIN_BL_ON_OFF, 1);
	#endif

#endif
#endif
	//IO_Set

#ifdef ENABLE_DCR
//fix me

       //Leo Chen+ for DCR
#ifdef CONFIG_PCBMGR
	if (m_PIN_BL_ADJ < 0) {
		unsigned long long value = 0;
		int index, type;
		if (pcb_mgr_get_enum_info_byname("PIN_BL_ADJ", &value) < 0) {
			m_PIN_BL_ADJ = -1;
			//rtd_pr_vbe_info("%s %d\n", __FUNCTION__, __LINE__);
		} else {
			if (PCB_PIN_TYPE_ISO_GPIO == GET_PIN_TYPE(value) || PCB_PIN_TYPE_ISO_PWM==GET_PIN_TYPE(value) || PCB_PIN_TYPE_ISO_UNIPWM==GET_PIN_TYPE(value) )
				type = 1;
			else
				type = 0;
			index = GET_PIN_INDEX(value);
			m_PIN_BL_ADJ =  ((type & 0xFFFF) << 16) | (index & 0xFFFF);
			//rtd_pr_vbe_info("%s %d, index=%d\n", __FUNCTION__, __LINE__, index);

		}
	}
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);

#endif

       //m_PIN_BL_ON_OFF=PIN_BL_ON_OFF; //mark for compiler error
       //m_BACKLIGHT_ON=_BACKLIGHT_ON; //mark for compiler error
       m_backlight_level_from_user=level;



	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_PIN_BL_ADJ, m_PIN_BL_ADJ);
       if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ADJ\n" );
		return FALSE;
	}
	*/
		if (m_PIN_BL_ADJ >= 0) {
			if (0 != (ret = Scaler_SendRPC(SCALERIOC_PIN_BL_ADJ, m_PIN_BL_ADJ, RPC_SEND_VALUE_ONLY)))
			{
				rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ADJ\n" );
				return FALSE;

			}
		}
#if 0
	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_PIN_BL_ON_OFF, m_PIN_BL_ON_OFF);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ON_OFF\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_PIN_BL_ON_OFF, m_PIN_BL_ON_OFF, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ON_OFF\n" );
			return FALSE;
		}


	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_BACKLIGHT_ON, m_BACKLIGHT_ON);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_BACKLIGHT_ON\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_BACKLIGHT_ON, m_BACKLIGHT_ON, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_BACKLIGHT_ON\n" );
			return FALSE;
		}

#endif
	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_IS_INCREASE_MODE, m_isIncreaseMode);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_isIncreaseMode\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_IS_INCREASE_MODE, m_isIncreaseMode, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_isIncreaseMode\n" );
			return FALSE;
		}

	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_BL_LV_FROM_USER, m_backlight_level_from_user);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlight_level_from_user\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_BL_LV_FROM_USER, m_backlight_level_from_user, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlight_level_from_user\n" );
			return FALSE;
		}


       //DCR end
#endif

	if(m_callbackSetPwmDuty != NULL)
		m_callbackSetPwmDuty(mappingValue);




	return TRUE;

}
#endif

#if defined (BUILD_TV030_2)
unsigned char Panel_SetNatureLightBackLightLevel(unsigned int level)
{
	unsigned int mappingValue;
	//unsigned int address,shiftBits = 0;
	//unsigned int andMask,orMask ;

	if(m_blockbacklight)	return TRUE;

#ifdef ENABLE_DCR
       INT8 ret=0;
#endif
	//Scaler_SetBackLight((UINT8)level); //yf 20110704
	       mappingValue = level;

		if(level > m_backlightLevelActMax)
			mappingValue = m_backlightLevelActMax;

		if(level < m_backlightLevelActMin)
			mappingValue = m_backlightLevelActMin;


		mappingValue &= MAX_ACT_LEVEL;
		//rtd_pr_vbe_info("m_backlightLevelActMax = %x\n", m_backlightLevelActMax);
		//rtd_pr_vbe_info("m_backlightLevelActMax = %x\n", m_backlightLevelActMin);
		//rtd_pr_vbe_info("mappingValue = %d\n", mappingValue);



	//rtd_pr_vbe_info("level=%x\n", level);
	//rtd_pr_vbe_info("mapping=%x\n", mappingValue);

#ifdef ENABLE_DCR
	//rtd_pr_vbe_info("panelAPI\n");
#endif

//	IoGpio_SetPwmDuty(PIN_BL_ADJ, mappingValue);
#ifdef PIN_BL_ADJ
#if defined(BUILD_TV005_1_ATV) || defined(BUILD_TV057_1_ATV)||defined(BUILD_TV005_1_ISDB)
	if(m_callbackSetPwmDuty == NULL)
	{
		#if defined(BUILD_TV057_1_ATV)
		//IO_PWM_SetDuty(PIN_BL_ADJ, mappingValue);
		#else
		IO_PWM_SetDuty(PIN_BL_ADJ, mappingValue);
		#endif
	}
#else	//BUILD_TV005_1_ATV
	IO_PWM_SetDuty(PIN_BL_ADJ, mappingValue);
#endif	//BUILD_TV005_1_ATV
#endif
	m_origbacklightLevel = mappingValue;

	// back light on
//	IoGpio_SetPinLevel(PIN_BL_ON_OFF, _BACKLIGHT_ON);
#if !defined(BUILD_TV057_1_ATV)
#ifdef PIN_BL_ON_OFF
	#if defined TV015_216_ATV_V1 //eric 20120615
		#ifdef TV015_216_ATV_ALLINONE
			IO_Direct_Set("PIN_BL_ON_OFF", 1);	//All_In_One
  		#else
			IO_Direct_Set("PIN_BL_ON_OFF", 0);
  		#endif
	#else
		IO_Set(PIN_BL_ON_OFF, 1);
	#endif

#endif
#endif
	//IO_Set

#ifdef ENABLE_DCR
//fix me

       //Leo Chen+ for DCR
#ifdef CONFIG_PCBMGR
	if (m_PIN_BL_ADJ < 0) {
		unsigned long long value = 0;
		int index, type;
		if (pcb_mgr_get_enum_info_byname("PIN_BL_ADJ", &value) < 0) {
			m_PIN_BL_ADJ = -1;
			//rtd_pr_vbe_info("%s %d\n", __FUNCTION__, __LINE__);
		} else {
			if (PCB_PIN_TYPE_ISO_GPIO == GET_PIN_TYPE(value) || PCB_PIN_TYPE_ISO_PWM==GET_PIN_TYPE(value) || PCB_PIN_TYPE_ISO_UNIPWM==GET_PIN_TYPE(value) )
				type = 1;
			else
				type = 0;
			index = GET_PIN_INDEX(value);
			m_PIN_BL_ADJ =  ((type & 0xFFFF) << 16) | (index & 0xFFFF);
			//rtd_pr_vbe_info("%s %d, index=%d\n", __FUNCTION__, __LINE__, index);

		}
	}
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);
	//rtd_pr_vbe_info("\033[1;41m m_PIN_BL_ADJ = %d\033[m\n", m_PIN_BL_ADJ);

#endif

       //m_PIN_BL_ON_OFF=PIN_BL_ON_OFF; //mark for compiler error
       //m_BACKLIGHT_ON=_BACKLIGHT_ON; //mark for compiler error
       m_backlight_level_from_user=level;



	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_PIN_BL_ADJ, m_PIN_BL_ADJ);
       if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ADJ\n" );
		return FALSE;
	}
	*/
		if (m_PIN_BL_ADJ >= 0) {
			if (0 != (ret = Scaler_SendRPC(SCALERIOC_PIN_BL_ADJ, m_PIN_BL_ADJ, RPC_SEND_VALUE_ONLY)))
			{
				rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ADJ\n" );
				return FALSE;

			}
		}
#if 0
	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_PIN_BL_ON_OFF, m_PIN_BL_ON_OFF);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ON_OFF\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_PIN_BL_ON_OFF, m_PIN_BL_ON_OFF, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_PIN_BL_ON_OFF\n" );
			return FALSE;
		}


	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_BACKLIGHT_ON, m_BACKLIGHT_ON);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_BACKLIGHT_ON\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_BACKLIGHT_ON, m_BACKLIGHT_ON, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_BACKLIGHT_ON\n" );
			return FALSE;
		}

#endif
	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_IS_INCREASE_MODE, m_isIncreaseMode);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_isIncreaseMode\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_IS_INCREASE_MODE, m_isIncreaseMode, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_isIncreaseMode\n" );
			return FALSE;
		}

	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_BL_LV_FROM_USER, m_backlight_level_from_user);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlight_level_from_user\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_BL_LV_FROM_USER, m_backlight_level_from_user, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlight_level_from_user\n" );
			return FALSE;
		}


       //DCR end
#endif

	if(m_callbackSetPwmDuty != NULL)
		m_callbackSetPwmDuty(mappingValue);

	return TRUE;

}
#endif
/**
 * Panel_SetBackLightRange
 * mapping uiMax to actMax,uiMin to actMin,and use uiStep as step size when use Panel_SetBackLightLevel
 *
 * @param {uiMax : 0~255,default 100}
 * @param {uiMin : 0~255,default 0}
 * @param {uiMax : 0~255,default 255}
 * @param {uiMin : 0~255,default 0}
 * @return {TRUE/FALSE }
 *
 */
unsigned char Panel_SetBackLightMappingRange(unsigned int 	uiMax,
									unsigned int 	uiMin,
									unsigned int 	actMax,
									unsigned int 	actMin,
									unsigned char 	increaseMode)
{
#ifdef ENABLE_DCR
       INT8 ret=0;
#endif
	if( (actMax>255) || (actMin>255) || (actMax<=actMin))
		return FALSE;

	if( (uiMax>255) || (uiMin>255) || (uiMax<=uiMin) )
		return FALSE;

	m_backlightLevelUiMax = uiMax;
	m_backlightLevelUiMin = uiMin;
	m_backlightLevelActMax = actMax;
	m_backlightLevelActMin = actMin;
	m_isIncreaseMode = increaseMode;
#ifdef ENABLE_DCR
//fix me
	/*
      ret = ioctl(Scaler_GetDev(), SCALERIOC_BL_LV_ACT_MAX, m_backlightLevelActMax);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlightLevelActMax\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_BL_LV_ACT_MAX, m_backlightLevelActMax, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlightLevelActMax\n" );
			return FALSE;
		}


	/*
       ret = ioctl(Scaler_GetDev(), SCALERIOC_BL_LV_ACT_MIN, m_backlightLevelActMin);
	if (ret== -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlightLevelActMin\n" );
		return FALSE;
	}
	*/
		if (0 != (ret = Scaler_SendRPC(SCALERIOC_BL_LV_ACT_MIN, m_backlightLevelActMin, RPC_SEND_VALUE_ONLY)))
		{
			rtd_pr_vbe_info("open /dev/scaler fail, cannot set m_backlightLevelActMin\n" );
			return FALSE;
		}


#endif
	return TRUE;
}

unsigned char Panel_GetBackLightMappingRange(unsigned int *uiMax,unsigned int *uiMin,unsigned int *actMax,unsigned int *actMin, unsigned char *increaseMode)
{
	*uiMax			= m_backlightLevelUiMax;
	*uiMin			= m_backlightLevelUiMin;
	*actMax			= m_backlightLevelActMax;
	*actMin			= m_backlightLevelActMin;
	*increaseMode 	= m_isIncreaseMode;

	return TRUE;
}

void Panel_SetPwmDutyCbFunc(void (*callback)(UINT8 value))
{
	m_callbackSetPwmDuty = callback;
}
void Panel_InitCbFunc(void (*callback)(void)) // skyworth yf 20111125
{
	m_callbackPanelInit = callback;
}
void Panel_TconControlCbFunc(void (*callback)(MILLET_I2C_INPUT_FORMAT mode)) // skyworth yf 20111125
{
	m_callbackTconControl = callback;
}
#endif
#if 0
unsigned char Panel_TurnOff(void)
{
    unsigned long long info;

    IO_Direct_Set("PIN_BL_ON_OFF", 0);
#ifdef BUILD_TV013_1
	if (pcb_mgr_get_enum_info_byname("PIN_LED_ON_OFF", &info) == 0)//hardy20140226
	{
		IO_Direct_Set("PIN_LED_ON_OFF", 1);
		IO_Config(info);
		IO_Set(info, 1);//hardy20140226
	}
#endif

#if defined(BOARD_ID_RTD2986IK_TV003_ATSC_V1) && defined(ENABLE_TCON_MODULE)
#else
    pcb_mgr_get_enum_info_byname("PIN_BL_ADJ", &info);
    IO_PWM_SetDuty(info, 0);
#endif

   if (pPanelCOnfigParameter!=NULL)
    msleep(pPanelCOnfigParameter->iLIGHT_TO_LDVS_OFF_ms);
   else
   	msleep(LIGHT_TO_LDVS_OFF_ms);

    rtd_outl(0xb8000974, rtd_inl(0xb8000974)&~(_BIT4|_BIT5|_BIT6|_BIT7));
    rtd_outl(0xb8000964, rtd_inl(0xb8000964)&~(_BIT6|_BIT7));
    rtd_outl(0xb801BCE0, rtd_inl(0xb801BCE0)&~(0xff));

   if (pPanelCOnfigParameter!=NULL)
    msleep(pPanelCOnfigParameter->iLVDS_TO_PANEL_OFF_ms);
   else
   	msleep(LVDS_TO_PANEL_OFF_ms);

    IO_Direct_Set("PIN_LVDS_ON_OFF", 0);

	//msleep(250); //don't need it, because poweroff process is over 250ms
	return TRUE;
}

unsigned char Panel_TurnOn(void)
{
        unsigned long long info;
#ifdef BUILD_TV013_1
	if (pcb_mgr_get_enum_info_byname("PIN_LED_ON_OFF", &info) == 0)
	{
		rtd_pr_vbe_info("Panel_TurnOn()--2\n");
		IO_Config(info);
		IO_Set(info, 0);//hardy20140226
	}
#endif
	IO_Direct_Set("PIN_LVDS_ON_OFF", 1);

       if (pPanelCOnfigParameter!=NULL)
        msleep(pPanelCOnfigParameter->iPANEL_TO_LVDS_ON_ms);
	   else
	   	msleep(PANEL_TO_LVDS_ON_ms);

        rtd_outl(0xb8000974, rtd_inl(0xb8000974)|(_BIT4|_BIT5|_BIT6|_BIT7));
        rtd_outl(0xb8000964, rtd_inl(0xb8000964)|(_BIT6|_BIT7));
        rtd_outl(0xb801BCE0, rtd_inl(0xb801BCE0)|(0xff));

		if (pPanelCOnfigParameter!=NULL)
        msleep(pPanelCOnfigParameter->iLVDS_TO_LIGHT_ON_ms);
		else
		msleep(LVDS_TO_LIGHT_ON_ms);

	// set 4k2k timing if using millet3 & act input is 4k2k
	if(Get_PANEL_TYPE_IS_MILLET3_4K2K_SERIES() &&
		(Scaler_DispGetInputInfo(SLR_INPUT_IPH_ACT_WID) > 2048)
		&& (Scaler_DispGetInputInfo(SLR_INPUT_IPV_ACT_LEN) > 1200))
	{
		if(m_callbackTconControl != NULL)
			m_callbackTconControl(_INPUT_4K_2K);
		//drvif_scalerdisplay_I2CSettingToMillet(_INPUT_4K_2K);
		msleep(500); //prevent dirty screen.
	}

        pcb_mgr_get_enum_info_byname("PIN_BL_ADJ", &info);
        IO_PWM_SetDuty(info, m_origbacklightLevel);

        IO_Direct_Set("PIN_BL_ON_OFF", 1);

	if(Get_PANEL_TYPE_IS_IWATT_LOCAL_DIMMING())
		ld_Init();

	//msleep(250); //don't need it, because poweroff process is over 250ms

	return TRUE;
}
#endif

#if 0
#ifdef ENABLE_DCR
#ifdef TPV_New_DCR
unsigned char drvif_color_DCRTable(UINT8 *ptr)
{rtd_pr_vbe_info("drvif_color_DCRTable  TPV_New_DCR\n" );
	int ret;
	if(!ptr)
		return FALSE;
	DCR_TABLE = ptr;
	unsigned int m_DCR_Max_Value=0;
	UINT8 idx_r, idx_c,idx_t,offset;
	SCALERDRV_DCRTABLE struDcr_Data;
	struDcr_Data.nTPV_New_DCR=1;
	struDcr_Data.nDCRDefault=1;//on
	//set dcr table value
	for(idx_t=0;idx_t<2;idx_t++)
		for(idx_r=0;idx_r<DCR_TABLE_ROW_NUM;idx_r++)
			for(idx_c=0;idx_c<DCR_TABLE_COL_NUM;idx_c++)
			{
				offset=idx_t*DCR_TABLE_ROW_NUM*DCR_TABLE_COL_NUM+idx_r*DCR_TABLE_COL_NUM+idx_c;
				struDcr_Data.DCR_Table[idx_t][idx_r][idx_c] = *(DCR_TABLE+offset);
			}
	#if 0
	for(idx_t=0;idx_t<2;idx_t++)
		for(idx_r=0;idx_r<DCR_TABLE_ROW_NUM;idx_r++)
		{
			rtd_pr_vbe_info("TPV_New_DCR %d	%d	%d	%d	%d\n", struDcr_Data.DCR_Table[idx_t][idx_r][0],struDcr_Data.DCR_Table[idx_t][idx_r][1],struDcr_Data.DCR_Table[idx_t][idx_r][2],struDcr_Data.DCR_Table[idx_t][idx_r][3],struDcr_Data.DCR_Table[idx_t][idx_r][4]);
		}
	#endif
	memcpy((unsigned int*)Scaler_GetShareMemVirAddr(SCALERIOC_DCR_TABLE), &struDcr_Data, sizeof(SCALERDRV_DCRTABLE));
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_DCR_TABLE)))
	{
		rtd_pr_vbe_info("ret=%d, set DCR_Table  to driver fail !!!\n", ret );
		return FALSE;
	}
	//send osd BL value
	m_DCR_Max_Value=Scaler_GetBacklight();
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_DCR_MAX_VALUE, m_DCR_Max_Value, RPC_SEND_VALUE_ONLY)))
	{
		rtd_pr_vbe_info("ret=%d, set SCALERIOC_DCR_MAX_VALUE  to driver fail !!!\n", ret );
		return FALSE;
	}
	return TRUE;

}

#else

unsigned char drvif_color_DCRTable(UINT8 *ptr)   //20101111 added by Leo Chen
{
	if(!ptr)
		return FALSE;

	DCR_TABLE = ptr;

	int ret, i, j;
	SCALERDRV_DCRTABLE dcr_table;
       unsigned int m_DCR_Max_Value=0;

#if defined(MACARTHUR)
	dcr_table.nTPV_New_DCR=0;
#endif


#ifdef TPV_New_DCR
	dcr_table.nTPV_New_DCR=0;
#endif
/*	if ( Scaler_GetDev() == -1 ) {
		rtd_pr_vbe_info("open /dev/scaler fail, cannot set DCR_Table\n" );
		return FALSE;
	}*/

	// backlight for TPV 2958i, elieli 20130305
#ifdef BUILD_TV015_5_ISDB
		dcr_table.nTPV_New_DCR = 2;
		for ( i=0, j=0 ; i<DCR_TABLE_ROW_NUM_TPV2958i; ++i, j+=DCR_TABLE_COL_NUM_TPV2958i) {
			dcr_table.DCR_TABLE_TPV2958i[i][0] = *(DCR_TABLE+j);
			dcr_table.DCR_TABLE_TPV2958i[i][1] = *(DCR_TABLE+j+1);
			dcr_table.DCR_TABLE_TPV2958i[i][2] = *(DCR_TABLE+j+2);
			dcr_table.DCR_TABLE_TPV2958i[i][3] = *(DCR_TABLE+j+3);
			dcr_table.DCR_TABLE_TPV2958i[i][4] = *(DCR_TABLE+j+4);
		}
#else

	for ( i=0, j=0 ; i<DCR_MAX; ++i, j+=DCR_NODE_NUM) {
		dcr_table.DCR_TABLE[i][0] = *(DCR_TABLE+j);
		dcr_table.DCR_TABLE[i][1] = *(DCR_TABLE+j+1);
		dcr_table.DCR_TABLE[i][2] = *(DCR_TABLE+j+2);
		dcr_table.DCR_TABLE[i][3] = *(DCR_TABLE+j+3);
		dcr_table.DCR_TABLE[i][4] = *(DCR_TABLE+j+4);
	}

		rtd_pr_vbe_info(" [DCR] dcr_table.DCR_TABLE[0][0] =  %d\n",dcr_table.DCR_TABLE[0][0]);

#endif
//fix me
/*	if ( (ret = ioctl(Scaler_GetDev(), SCALERIOC_DCR_TABLE, &dcr_table)) < 0 ) {
		rtd_pr_vbe_info("ret=%d, set DCR_Table  to driver fail !!!\n", ret );
		return FALSE;
	}*/

	memcpy((unsigned int*)Scaler_GetShareMemVirAddr(SCALERIOC_DCR_TABLE), &dcr_table, sizeof(SCALERDRV_DCRTABLE));
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_DCR_TABLE)))
	{
		rtd_pr_vbe_info("ret=%d, set DCR_Table  to driver fail !!!\n", ret );
		return FALSE;
	}



       /* DCR_MODE */
       rtd_pr_vbe_info("[DCR] select_dcr_mode=%d\n", dcr_table.DCR_TABLE[4][0]);

       switch(dcr_table.DCR_TABLE[4][0])
       {
            case DCR_MASTER:
                    m_DCR_Max_Value=BL_LEVEL_UI_MAX_DEFAULT;
                    break;
            case DCR_SLAVE:
                    m_DCR_Max_Value=BL_LEVEL_UI_MAX_DEFAULT;//m_backlight_level_from_user;	//20120623 roger, give default value
                    break;
            default:
                break;
       }
	rtd_pr_vbe_info("[DCR] m_DCR_Max_Value=%d\n", m_DCR_Max_Value);

//fix me       ret = ioctl(Scaler_GetDev(), SCALERIOC_DCR_MAX_VALUE, m_DCR_Max_Value);
	if (0 != (ret = Scaler_SendRPC(SCALERIOC_DCR_MAX_VALUE, m_DCR_Max_Value, RPC_SEND_VALUE_ONLY)))
	{
		rtd_pr_vbe_info("ret=%d, set SCALERIOC_DCR_MAX_VALUE  to driver fail !!!\n", ret );
		return FALSE;
	}


	return TRUE;

}
#endif
#endif
#endif

//USER: Vincent_Lee  DATE: 2012/4/28  TODO: Add TCON for open cell panel
#ifdef OPEN_CELL_PANEL



//USER: Vincent_Lee  DATE_2012_5_18  TODO: Add soft I2C for Gamma IC control (BUF_16821)
#if (defined(CONFIG_PANEL_AUO_42_T420HW09))
const UINT8 Gamma_Table[] = {

			0x03, 0xc7,	//DAC1
			0x03, 0xa4,	//DAC2
			0x03, 0x1a,	//DAC3
			0x02, 0xbb,	//DAC4
			0x02, 0x79,	//DAC5
			0x02, 0x29,	//DAC6
			0x02, 0x27,	//DAC7
			0x01, 0xf7,	//DAC8
			0x01, 0xf6,	//DAC9
			0x01, 0x36,	//DAC10
			0x00, 0xfe,	//DAC11
			0x00, 0xa2,	//DAC12
			0x00, 0x0f,	//DAC13
			0x00, 0x0c,	//DAC14
			0x00, 0x03,	//DAC15
			0x00, 0x00,	//DAC16

			0x01, 0xa2,	//Vcom1
			0x01, 0xa2	//Vcom2
};

#elif (defined(CONFIG_PANEL_CMI_39_V390HJ1_PE1) )
const UINT8 Gamma_Table[] = {
			0x03, 0xdb,	//DAC1
			0x03, 0x15,	//DAC2
			0x02, 0xef,	//DAC3
			0x02, 0xb1,	//DAC4
			0x02, 0xa0,	//DAC5
			0x02, 0x18,	//DAC6
			0x02, 0x44,	//DAC7
			0x01, 0xec,	//DAC8
			0x01, 0xb0,	//DAC9
			0x01, 0x8f,	//DAC10
			0x01, 0x79,	//DAC11
			0x01, 0x2a,	//DAC12
			0x00, 0xff,	//DAC13
			0x00, 0x1c,	//DAC14
			0x00, 0x00,	//DAC15
			0x00, 0x00,	//DAC16

			0x01, 0xcc,	//Vcom1
			0x01, 0xcc	//Vcom2
};

#else

// Add default table
const UINT8 Gamma_Table[] = {
			0x02, 0x00,	//DAC1
			0x02, 0x00,	//DAC2
			0x02, 0x00,	//DAC3
			0x02, 0x00,	//DAC4
			0x02, 0x00,	//DAC5
			0x02, 0x00,	//DAC6
			0x02, 0x00,	//DAC7
			0x02, 0x00,	//DAC8
			0x02, 0x00,	//DAC9
			0x02, 0x00,	//DAC10
			0x02, 0x00,	//DAC11
			0x02, 0x00,	//DAC12
			0x02, 0x00,	//DAC13
			0x02, 0x00,	//DAC14
			0x02, 0x00,	//DAC15
			0x02, 0x00,	//DAC16

			0x02, 0x00,	//Vcom1
			0x02, 0x00	//Vcom2
};

#endif

void Panel_Gamma_Init(void)
{
	UINT8 i;
	UINT8 init_gamma = 0;
	UINT8 buff[36];

	Softi2c_Gamma_Read(0x00, &buff[0], 14);

	for(i = 0; i < 28; i++)
	{
		if(buff[i] != Gamma_Table[i])
		{
			init_gamma = 1;
			break;
		}
	}

	if(init_gamma == 1)
	{
		rtd_pr_vbe_debug("\n\n\n----lhh---write gamma ic----\n\n\n");

		for(i = 0 ; i < 14 ; i++)
		{
			Softi2c_Gamma_Write(i, &Gamma_Table[i * 2], 1, 0x40);
			msleep(20);
		}
		Softi2c_Gamma_Write(0x12, &Gamma_Table[32], 1, 0x40);
		msleep(20);
		Softi2c_Gamma_Write(0x13, &Gamma_Table[34], 1, 0x40);
		msleep(20);


		Softi2c_Gamma_Read(0x00, &buff[0], 14);
		Softi2c_Gamma_Read(0x12, &buff[32], 2);
		for(i = 0 ; i < 14 ; i++)
		{
			rtd_pr_vbe_debug("\n---lhh---buf[%d] = %x,%x", i, buff[i*2], buff[i*2 +1]);
		}
		rtd_pr_vbe_debug("\n---lhh---buf[V1] = %x,%x", buff[32], buff[33]);
		rtd_pr_vbe_debug("\n---lhh---buf[V2] = %x,%x", buff[34], buff[35]);

	}
}
#endif

#if 0
void Panel_SetBackLightBlock(bool YesNo)
{
	m_blockbacklight = YesNo;
}
#endif

//USER:LewisLee DATE:2013/04/26
//For IT6263, we output LVDS signal, it will converter to HDMI output
UINT8 Panel_LVDS_To_Hdmi_Converter_Parameter(void)
{
//#ifdef CONFIG_ENABLE_LVDS_CONVERTER_HDMI
	ppoverlay_dh_total_last_line_length_RBUS dh_total_last_line_length_reg;
	UINT32 DH_Total = 0;
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
	pif_lvds_ctrl3_RBUS  lvds_ctrl3;

	if(Get_DISPLAY_PANEL_TYPE() != P_LVDS_TO_HDMI)
		return _FALSE;

//	if(_DISABLE == Scaler_Get_LVDS_Converter_To_HDMI_Enable())
//		return _FALSE;


#ifdef CONFIG_DUAL_CHANNEL
	//sub display not apply
	if(SLR_SUB_DISPLAY == Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY))
		return _FALSE;
#endif

	if(_TRUE == Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_FRAMESYNC))
	{
		rtd_pr_vbe_debug("FrameSync Line Buffer mode, exit Panel_LVDS_To_Hdmi_Converter_Parameter\n");
		return _FALSE;
	}

	rtd_pr_vbe_debug("Enter Panel_LVDS_To_Hdmi_Converter_Parameter\n");

	//Step 1 : Last Line Pixel = DH total
	dh_total_last_line_length_reg.regValue = IoReg_Read32(PPOVERLAY_DH_Total_Last_Line_Length_reg);
	DH_Total = dh_total_last_line_length_reg.dh_total;
	dh_total_last_line_length_reg.dh_total_last_line = DH_Total;
	IoReg_Write32(PPOVERLAY_DH_Total_Last_Line_Length_reg, dh_total_last_line_length_reg.regValue);
//	rtd_pr_vbe_info("PPOVERLAY_DH_Total_Last_Line_Length_reg : %x\n", IoReg_Read32(PPOVERLAY_DH_Total_Last_Line_Length_reg));

	//Step 2 : Set to Free Run
	framesync_set_enterlastline_at_frc_mode_flg(_FALSE);

	display_timing_ctrl1_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL1_reg);
	display_timing_ctrl1_reg.disp_frc_on_fsync = _DISABLE;
	display_timing_ctrl1_reg.disp_line_4x = _DISABLE;
	display_timing_ctrl1_reg.disp_fix_last_line_new = _DISABLE;
	display_timing_ctrl1_reg.disp_fsync_en = _DISABLE;
	IoReg_Write32(PPOVERLAY_Display_Timing_CTRL1_reg, display_timing_ctrl1_reg.regValue);

	//Step 3 : Disable Spectrum
	drv_adjust_spreadspectrumrange(0);

	//Sttep4 adjust Bitmap format
	lvds_ctrl3.regValue = IoReg_Read32(PIF_LVDS_CTRL3_reg);
	lvds_ctrl3.lvds_map = 0;
	IoReg_Write32(PIF_LVDS_CTRL3_reg, lvds_ctrl3.regValue);


	return _TRUE;
//#endif //#ifdef CONFIG_ENABLE_LVDS_CONVERTER_HDMI
//	return _FALSE;
}

static PANEL_PMIC_SETTING_INDEX st_panel_pmic_index;
#define PANEL_PMIC_I2C		PCB_I2C_3

void Set_PANEL_PMIC_Type(PANEL_PMIC_SETTING_INDEX pmic_idx)
{
	st_panel_pmic_index = pmic_idx;
}

PANEL_PMIC_SETTING_INDEX Get_PANEL_PMIC_Type(void)
{
	return st_panel_pmic_index;
}

#ifndef BUILD_QUICK_SHOW 
void panel_setup_PMIC(void)
{
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
	unsigned char pmic_1F_36_data[2][2] = {{0x00, 0x1F}, {0x01, 0x36}};
	unsigned char driver_addr = 0x30;
#endif
	PANEL_PMIC_SETTING_INDEX pmicIdx = 0;

	pmicIdx = Get_PANEL_PMIC_Type();

	rtd_pr_vbe_notice("panel_setup_PMIC = %d \n", pmicIdx);

	pmicIdx = PMIC_1F_36;


	switch (pmicIdx) {
	case PMIC_1F_36:
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_1F_36_data[0][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_1F_36_data[1][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
#endif
	break;
#if 0
	case PMIC_1F_42:
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_1F_42_data[0][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_1F_42_data[1][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
	break;

	case PMIC_F_42:
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_F_42_data[0][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_F_42_data[1][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
	break;

	case PMIC_F_65:
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_F_65_data[0][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_F_65_data[1][0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
	break;

	case PMIC_25:
		if(i2c_master_send_ex(PANEL_PMIC_I2C, driver_addr, &pmic_25_data[0], 2) < 0){
			rtd_pr_vbe_err("PMIC err @ _line : %d \n", __LINE__);
		}
	break;
#endif
	default:
		// don't set PMIC
	break;
	}

}


int panel_get_PLC_curve( unsigned char* PLC_curve )
{
	int i;
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
	unsigned char PLC_addr = 0x78;
#endif
	unsigned char addr[1] = {0};
	unsigned char data[1] = {0};

	for( i=0; i<8; i++ )
	{
		addr[0] = 3+i;

		//if ( i2c_master_send_ex(PANEL_PMIC_I2C, PLC_addr, addr, 1) < 0 ){
		//	rtd_pr_vbe_emerg("I2C Write Address Reg:0x%x failed\n",addr[0]);
		//	return (-1);
		//}
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
		if( i2c_master_recv_ex(PANEL_PMIC_I2C, PLC_addr, addr, 1, data, 1) < 0 ){
			rtd_pr_vbe_emerg("I2C Read Reg:0x%x failed\n",addr[0]);;
			return (-1);
		}
#endif

		PLC_curve[i] = data[0];
	}

	//rtd_pr_vbe_emerg("[panel_get_PLC_curve] PLC_curve: %d, %d, %d, %d, %d, %d, %d, %d\n",
	//	PLC_curve[0], PLC_curve[1], PLC_curve[2], PLC_curve[3], PLC_curve[4], PLC_curve[5], PLC_curve[6], PLC_curve[7] );

	return 0;
}

int panel_set_PLC_curve( unsigned char* PLC_curve )
{
	int i;
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
	unsigned char PLC_addr = 0x78;
#endif
	unsigned char data[2] = {0};

	//rtd_pr_vbe_emerg("[panel_set_PLC_curve] set PLC_curve: %d, %d, %d, %d, %d, %d, %d, %d\n",
	//	PLC_curve[0], PLC_curve[1], PLC_curve[2], PLC_curve[3], PLC_curve[4], PLC_curve[5], PLC_curve[6], PLC_curve[7] );

	for( i=0; i<8; i++ )
	{
		data[0] = 3+i;
		data[1] = PLC_curve[i];
#if IS_ENABLED(CONFIG_RTK_KDRV_I2C)
		if ( i2c_master_send_ex(PANEL_PMIC_I2C, PLC_addr, data, 2) < 0 ){
			rtd_pr_vbe_emerg("I2C Write Address Reg:0x%x failed\n",data[0]);
			return (-1);
		}
#endif
	}

	return 0;
}

#endif

/*========================Functions===============================*/


void ld_Init(void)
{
		ldspi_out_data_ctrl_RBUS  ld_interface_out_data_ctrl_reg;
		ldspi_timing_ctrl_RBUS ld_interface_timing_ctrl_reg;
		ldspi_send_delay_RBUS ld_interface_send_delay_reg;
		ldspi_data_unit_delay_RBUS ld_interface_data_unit_delay_reg;
		ldspi_v_sync_duty_RBUS  ld_interface_v_sync_duty_reg;
		//ldspi_data_h_time_RBUS 	ld_interface_data_h_time_reg;
		ldspi_ld_ctrl_RBUS ld_interface_ld_ctrl_reg;

		rtd_pr_vbe_debug("\n ld_Init !!! \n");


		ld_interface_out_data_ctrl_reg.regValue = rtd_inl(LDSPI_Out_data_CTRL_reg);
		ld_interface_out_data_ctrl_reg.data_endian = 1;
		ld_interface_out_data_ctrl_reg.output_data_format = 2;
		ld_interface_out_data_ctrl_reg.output_units = 0;
		rtd_outl(LDSPI_Out_data_CTRL_reg, ld_interface_out_data_ctrl_reg.regValue);

		ld_interface_timing_ctrl_reg.regValue = rtd_inl(LDSPI_Timing_CTRL_reg);
		ld_interface_timing_ctrl_reg.sck_h = 0xff;
		ld_interface_timing_ctrl_reg.sck_l = 0xff;
		rtd_outl(LDSPI_Timing_CTRL_reg,ld_interface_timing_ctrl_reg.regValue);

		ld_interface_send_delay_reg.regValue = rtd_inl(LDSPI_Send_delay_reg);
		ld_interface_send_delay_reg.data_send_delay = 0x6c8;
		rtd_outl(LDSPI_Send_delay_reg, ld_interface_send_delay_reg.regValue);

		ld_interface_data_unit_delay_reg.regValue = rtd_inl(LDSPI_Data_unit_delay_reg);
		ld_interface_data_unit_delay_reg.each_unit_delay = 0;
		rtd_outl(LDSPI_Data_unit_delay_reg, ld_interface_data_unit_delay_reg.regValue);

		ld_interface_v_sync_duty_reg.regValue = rtd_inl(LDSPI_V_sync_duty_reg);
		ld_interface_v_sync_duty_reg.vsync_d = 0xff;
		//ld_interface_v_sync_duty_reg.hsync_d = 0;
		rtd_outl(LDSPI_V_sync_duty_reg,ld_interface_v_sync_duty_reg.regValue);

		//marked due to merlin4 remove it, check it later
		//ld_interface_data_h_time_reg.regValue = rtd_inl(LDSPI_Data_h_time_reg);
		//ld_interface_data_h_time_reg.data_hold_time = 0xff;//0x1fd;
		//rtd_outl(LDSPI_Data_h_time_reg,ld_interface_data_h_time_reg.regValue);

		ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
		ld_interface_ld_ctrl_reg.ld_mode = LD_AS_MODE;
		ld_interface_ld_ctrl_reg.start_enable = 0;
		ld_interface_ld_ctrl_reg.send_trigger = 0;
//->?(weihao)		ld_interface_ld_ctrl_reg.ld_pin_port_sel = 1;
		ld_interface_ld_ctrl_reg.send_follow_vsync = 0;
		rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);

}

int ld_Write(unsigned int* data, unsigned int length,LD_WRITE_MODE mode)
{
#ifdef K24_PQ_COMPILE
	ldspi_ld_ctrl_RBUS ld_interface_ld_ctrl_reg;
	ldspi_out_data_ctrl_RBUS  ld_interface_out_data_ctrl_reg;
	ldspi_as_wr_ctrl_RBUS ld_interface_as_wr_ctrl_reg;
	unsigned int  i = 0;
	unsigned int timeoutcount = 0;

//	rtd_pr_vbe_info("\n ld_Write !!! \n");

	rtd_outl(LDSPI_Outindex_Addr_CTRL_reg,0x00000000);  //Reset OUT Inedx addr
	for(i=0;i<length;i++)
		rtd_outl(LDSPI_Outindex_Rwport_reg,i);  //OUT Inedx RW port

	rtd_outl(LDSPI_Data_Addr_CTRL_reg,0x00000000);  //Reset Data addr
	for(i=0;i<length;i++)
		rtd_outl(LDSPI_Data_Rwport_reg,*(data+i));

	ld_interface_out_data_ctrl_reg.regValue = rtd_inl(LDSPI_Out_data_CTRL_reg);
	ld_interface_out_data_ctrl_reg.output_units = (length-1); // data units
	rtd_outl(LDSPI_Out_data_CTRL_reg, ld_interface_out_data_ctrl_reg.regValue);

	ld_interface_as_wr_ctrl_reg.regValue = rtd_inl(LDSPI_AS_WR_CTRL_reg);
	ld_interface_as_wr_ctrl_reg.rwcmd = 0;
	ld_interface_as_wr_ctrl_reg.rw_cmd_sel = 0;
	ld_interface_as_wr_ctrl_reg.rw_bit_sent_sel = 0;
	rtd_outl(LDSPI_AS_WR_CTRL_reg,ld_interface_as_wr_ctrl_reg.regValue);

	ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
	ld_interface_ld_ctrl_reg.start_enable = 1;
	rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);

	ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
	if(mode == LD_WRITE_MODE_FOLLOW_VSYNC){
		rtd_pr_vbe_debug("\n ld_Write LD_WRITE_MODE_FOLLOW_VSYNC !!! \n");

		ld_interface_ld_ctrl_reg.send_follow_vsync = 1;
	}else{
		rtd_pr_vbe_debug("\n ld_Write  LD_WRITE_MODE_NOT_FOLLOW_VSYNC!!! \n");

		ld_interface_ld_ctrl_reg.send_follow_vsync = 0;
		ld_interface_ld_ctrl_reg.send_trigger = 1;
	}
	rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);


	if(mode == LD_WRITE_MODE_FOLLOW_VSYNC)
		msleep(30);	// wait v-sync occurt
	else{


		timeoutcount = 0x7ffff;
		do {
			ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
			if(ld_interface_ld_ctrl_reg.send_trigger == 0) {
				break;
			}
		} while(timeoutcount-- != 0);

		if(timeoutcount == 0)
			rtd_pr_vbe_debug("\n local dimming Write Error !!!\n ");

		ld_interface_ld_ctrl_reg.start_enable = 0;
		rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);	// disable local dimming interface
	}

#endif
	return 0;
}

unsigned int ld_Read(unsigned int addr)
{
#ifdef _VIP_Mer7_Compile_Error__
	ldspi_ld_ctrl_RBUS ld_interface_ld_ctrl_reg;
	ldspi_out_data_ctrl_RBUS  ld_interface_out_data_ctrl_reg;
	ldspi_as_wr_ctrl_RBUS ld_interface_as_wr_ctrl_reg;
	ldspi_rx_dataw0_RBUS ld_interface_rx_dataw0_reg;
//	ldspi_rx_dataw1_RBUS ld_interface_rx_dataw1_reg;
	unsigned int ReadData = 0;
	unsigned int timeoutcount = 0;

//	rtd_pr_vbe_info("\n ld_Read !!! \n");

	ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
	ld_interface_ld_ctrl_reg.start_enable = 0;
	ld_interface_ld_ctrl_reg.send_trigger = 0;
	ld_interface_ld_ctrl_reg.send_follow_vsync = 0;
	rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);	// disable local dimming interface

	rtd_outl(LDSPI_Outindex_Addr_CTRL_reg,0x00000000);  //Reset OUT Inedx addr
	rtd_outl(LDSPI_Outindex_Rwport_reg,0x00);  //OUT Inedx RW port
	rtd_outl(LDSPI_Outindex_Rwport_reg,0x01);

	rtd_outl(LDSPI_Data_Addr_CTRL_reg,0x00000000);  //Reset Data addr
	rtd_outl(LDSPI_Data_Rwport_reg,addr);
	rtd_outl(LDSPI_Data_Rwport_reg,0x00);

	ld_interface_out_data_ctrl_reg.regValue = rtd_inl(LDSPI_Out_data_CTRL_reg);
	ld_interface_out_data_ctrl_reg.output_units = 1; // data units
	rtd_outl(LDSPI_Out_data_CTRL_reg, ld_interface_out_data_ctrl_reg.regValue);

	ld_interface_as_wr_ctrl_reg.regValue = rtd_inl(LDSPI_AS_WR_CTRL_reg);
	ld_interface_as_wr_ctrl_reg.rwcmd = 1;
	ld_interface_as_wr_ctrl_reg.rw_cmd_sel = 1;
	ld_interface_as_wr_ctrl_reg.rw_bit_sent_sel = 1;
	rtd_outl(LDSPI_AS_WR_CTRL_reg,ld_interface_as_wr_ctrl_reg.regValue);

	ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
	ld_interface_ld_ctrl_reg.start_enable = 1;
	rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);	// enable local dimming


	ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
	ld_interface_ld_ctrl_reg.send_follow_vsync = 0;
	ld_interface_ld_ctrl_reg.send_trigger = 1;
	rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);

	timeoutcount = 0x7ffff;
	do {
		ld_interface_ld_ctrl_reg.regValue = rtd_inl(LDSPI_LD_CTRL_reg);
		if(ld_interface_ld_ctrl_reg.send_trigger == 0) {
			break;
		}
	} while(timeoutcount-- != 0);

	if(timeoutcount == 0)
		rtd_pr_vbe_debug("\n local dimming Read Error !!!\n ");

	ld_interface_ld_ctrl_reg.start_enable = 0;
	rtd_outl(LDSPI_LD_CTRL_reg, ld_interface_ld_ctrl_reg.regValue);	// disable local dimming interface

	ld_interface_rx_dataw0_reg.regValue = rtd_inl(LDSPI_RX_dataW0_reg);
	ReadData =  (ld_interface_rx_dataw0_reg.regValue >> 16 ) & 0xffff;

	return ReadData;
#else
	return 0;
#endif
}

#if 0
//#if defined(TV003_ADTV)
void Panel_TurnOn_Backlight(void)
{
	IO_Direct_Set("PIN_BL_ON_OFF", 1);
}

void Panel_TurnOff_Backlight(void)
{
	IO_Direct_Set("PIN_BL_ON_OFF", 0);
}
#endif
//#endif
extern platform_info_t platform_info;
unsigned short Get_DEFAULT_DISP_ACT_END_VPOS(void)
{
    PANEL_CONFIG_PARAMETER *panelConfigParameter = (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;

    if(panelConfigParameter != NULL)
    {
        return panelConfigParameter->iCONFIG_DISP_ACT_END_VPOS;
    }

    return CONFIG_DISP_DEN_END_VPOS;
}

unsigned short Get_DEFAULT_DISP_ACT_STA_VPOS(void)
{
    PANEL_CONFIG_PARAMETER *panelConfigParameter = (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;

    if(panelConfigParameter != NULL)
    {
        return panelConfigParameter->iCONFIG_DISP_ACT_STA_VPOS;
    }

    return CONFIG_DISP_DEN_STA_VPOS;
}

unsigned short Get_DEFAULT_DISP_ACT_END_HPOS(void)
{
    PANEL_CONFIG_PARAMETER *panelConfigParameter = (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;

    if(panelConfigParameter != NULL)
    {
        return panelConfigParameter->iCONFIG_DISP_ACT_END_HPOS;
    }

    return CONFIG_DISP_DEN_END_HPOS;
}

unsigned short Get_DEFAULT_DISP_ACT_STA_HPOS(void)
{
    PANEL_CONFIG_PARAMETER *panelConfigParameter = (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;

    if(panelConfigParameter != NULL)
    {
        return panelConfigParameter->iCONFIG_DISP_ACT_STA_HPOS;
    }

    return CONFIG_DISP_DEN_STA_HPOS;
}

unsigned short Get_DEFAULT_DISPLAY_REFRESH_RATE(void)
{
    PANEL_CONFIG_PARAMETER *panelConfigParameter = (PANEL_CONFIG_PARAMETER *)&platform_info.panel_parameter;

    if(panelConfigParameter != NULL)
    {
        return panelConfigParameter->iCONFIG_DISPLAY_REFRESH_RATE;
    }

    return CONFIG_DISPLAY_REFRESH_RATE;
}

void set_panel_pixel_mode(void)
{
	unsigned long long dclk = (unsigned long long)Get_DISP_HORIZONTAL_TOTAL() * (unsigned long long)Get_DISP_VERTICAL_TOTAL() * (unsigned long long)Get_DISPLAY_REFRESH_RATE();

	if( dclk <= (unsigned long long)Get_DISPLAY_CLOCK_TYPICAL() )
		PanelPixelModeCount = PANEL_1_PIXEL;
	else
		PanelPixelModeCount = PANEL_2_PIXEL;
}

PANEL_PIXEL_MODE get_panel_pixel_mode(void)
{
	return PanelPixelModeCount;
}

int Check_PANEL_OLED_type(void)
{
	if(pPanelCOnfigParameter == NULL)
		return false;

	if(pPanelCOnfigParameter->iCONFIG_PANEL_CUSTOM_INDEX == VBY_ONE_PANEL_KONKA_OLED_4K2K_120HZ_1S_16L
		|| (pPanelCOnfigParameter->iCONFIG_PANEL_CUSTOM_INDEX >= VBY_ONE_OLED_PANEL_BEGIN &&
			pPanelCOnfigParameter->iCONFIG_PANEL_CUSTOM_INDEX <= VBY_ONE_OLED_PANEL_MAX)
	)
		return true;
	else
		return false;
}

extern unsigned char vbe_disp_get_vo_tracking_d_panel_type(void);
unsigned char Get_VO_Tracking_D_PANEL_TYPE(void)
{
	if(vbe_disp_get_vo_tracking_d_panel_type()==0)
		return 0;

	return 1;
}

