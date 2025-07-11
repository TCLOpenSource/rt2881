/* 20140523 Local Dimming header file*/
#ifndef __LOCALDIMMING_H__
#define __LOCALDIMMING_H__

#ifdef __cplusplus
extern "C" {
#endif
/* some include about scaler*/
#include <scaler/vipCommon.h>

typedef struct {
	unsigned char ctrlMode;
	unsigned char bCtrl;
	unsigned short width;
	unsigned short height;
	unsigned short top;
	unsigned short bottom;
	unsigned short left;
	unsigned short right;

} DRV_HAL_VPQ_LED_LDCtrlDemoMode;

typedef enum {
	LED_ONOFF = 0,	// each led block flicker sequentially.
	LED_H_ONOFF_EXT,	// led horizontal moving box pattern on for external localdimming chip.
	LED_V_ONOFF_EXT,	// led vertical moving box pattern on for external localdimming chip.
	SPLIT_SCREEN,	// half of screen is made from white pattern, and show local dimming effect.
	LED_ONOFF_SPEED_SLOW,
	LED_ONOFF_SPEED_MED,
	LED_ONOFF_SPEED_QUICK,
	DEMOTYPE_SIZE_MAX
} LOCALDIMMING_DEMO_TYPE_T;


typedef enum {
	HAL_BL_DIRECT_M = 0,
	HAL_BL_DIRECT_L,
	HAL_BL_EDGE_LED,
	HAL_BL_ALEF_LED,
	HAL_BL_OLED,
	HAL_BL_END,
} HAL_BACKLIGHT_TYPE_T;


typedef enum {
	HAL_LED_BAR_6 = 0,
	HAL_LED_BAR_12,
	HAL_LED_BAR_16,
	HAL_LED_BAR_20,
	HAL_LED_BAR_240,
	HAL_LED_BAR_MAX
} HAL_LED_BAR_TYPE_T;

typedef enum {
	HAL_LDIM_NONE			= 0,		/* Not support Local dimming.*/
	HAL_LDIM_INTERNAL		= 1,		/* Use internal Local dimming block.*/
	HAL_LDIM_EXTERNAL		= 2,		/* Use external Local dimming IC.*/
} HAL_LDIM_IC_T;

typedef enum {
	HAL_WCG_PANEL_LED				= 0,
	HAL_WCG_PANEL_LED_ULTRAHD		= 1,
	HAL_WCG_PANEL_OLED				= 2,
	HAL_WCG_PANEL_OLED_ULTRAHD		= 3
} HAL_WCG_PANEL_TYPE_T;

typedef struct {
	unsigned char hal_inch;				/* panel size			ex) 47, 55*/
	unsigned char hal_bl_type;			/* led backlight type		ex) alef, edge*/
	unsigned char hal_bar_type;			/* led bar type			ex) h6,h12, v12*/
	unsigned char hal_maker;			/* panel maker			ex) lgd, auo*/
	HAL_LDIM_IC_T hal_icType;	/* localdimming control type	ex) internal localdiming block*/
	HAL_WCG_PANEL_TYPE_T hal_panel_type;
} HAL_LED_PANEL_INFO_T, KADP_LED_PANEL_INFO_T;

/* RTK style for KADP_LED_DB_LUT_T*/
#pragma pack(1)
typedef struct {
	unsigned char hal_ld_maxgain;
	unsigned char hal_ld_avegain;
	unsigned char hal_ld_histshiftbit;
	unsigned char hal_ld_hist0gain;
	unsigned char hal_ld_hist1gain;
	unsigned char hal_ld_hist2gain;
	unsigned char hal_ld_hist3gain;
	unsigned char hal_ld_hist4gain;
	unsigned char hal_ld_hist5gain;
	unsigned char hal_ld_hist6gain;
	unsigned char hal_ld_hist7gain;
} KADP_LED_BACKLIGHT_DECISION;

typedef struct
{
	unsigned char hal_ld_spatialnewcoef00;
	unsigned char hal_ld_spatialnewcoef01;
	unsigned char hal_ld_spatialnewcoef02;
	unsigned char hal_ld_spatialnewcoef03;
	unsigned char hal_ld_spatialnewcoef04;
	unsigned char hal_ld_spatialnewcoef05;
	unsigned char hal_ld_spatialnewcoef10;
	unsigned char hal_ld_spatialnewcoef11;
	unsigned char hal_ld_spatialnewcoef12;
	unsigned char hal_ld_spatialnewcoef13;
	unsigned char hal_ld_spatialnewcoef14;
	unsigned char hal_ld_spatialnewcoef15;

} KADP_LED_SPATIAL_FILTER;

typedef struct {

	unsigned char hal_ld_tmp_pos0thd;
	unsigned char hal_ld_tmp_pos1thd;
	unsigned char hal_ld_tmp_posmingain;
	unsigned char hal_ld_tmp_posmaxgain;
	unsigned char hal_ld_tmp_neg0thd;
	unsigned char hal_ld_tmp_neg1thd;
	unsigned char hal_ld_tmp_negmingain;
	unsigned char hal_ld_tmp_negmaxgain;
	unsigned char hal_ld_tmp_maxdiff;
	unsigned char hal_ld_tmp_scenechangegain1;

} KADP_LED_TEMPORAL_FILTER;

typedef struct {
	unsigned char hal_ld_spatialremapen;
	unsigned short hal_ld_spatialremaptab[65];
} KADP_LED_REMAP_CURVE;

typedef struct {
	unsigned int hal_ld_comp_2Dtable[17][17];
} KADP_LED_DATA_COMP;

typedef struct
{
	unsigned int lutVersion;
	unsigned int LD_UI_Index; //UI index
	KADP_LED_BACKLIGHT_DECISION hal_backlight_decision;
	KADP_LED_SPATIAL_FILTER hal_spatial_filter;
	KADP_LED_TEMPORAL_FILTER hal_temporal_filter;
	KADP_LED_REMAP_CURVE hal_remap_curve;
	KADP_LED_DATA_COMP hal_data_comp_table;
} KADP_LED_DB_LUT_T;
#pragma pack()

typedef struct {
	unsigned char ld_SPI_EN;
	unsigned char ld_mode;
	unsigned char ld_Pin_port_sel;
	unsigned char ld_Separate_two_block;
	unsigned char ld_Send_follow_Vsync;
	unsigned char ld_Data_endian;
	unsigned short ld_Output_units;
	unsigned char ld_Output_Data_format;
	unsigned short ld_SCK_H;
	unsigned short ld_SCK_L;
	unsigned int ld_Data_send_Delay;
	unsigned int ld_Each_unit_delay;
	unsigned short ld_Vsync_d;
	unsigned short ld_Data_hold_time;
} DRV_LD_InterFace_Init;

typedef struct {
	unsigned char ld_Region_Size;
	unsigned short ld_Start_ID_1st_byte;
	unsigned char ld_h_Region;
	unsigned char ld_v_Region;
} DRV_LD_Data_Output_Index_SRAM;

typedef struct {
	DRV_LD_InterFace_Init LD_InterFace_Init;
	DRV_LD_Data_Output_Index_SRAM LD_Data_Output_Index_SRAM;
} DRV_Local_Dimming_InterFace_Table;

/*	remove to vipcommon
typedef struct {
	unsigned int LD_Backlight_Profile_Interpolation_table_H[BL_Profile_Table_NUM][BL_Profile_Table_COLUMN];
	unsigned int LD_Backlight_Profile_Interpolation_table_V[BL_Profile_Table_NUM][BL_Profile_Table_COLUMN];
} SLR_VIP_TABLE_BLPF;
*/

typedef enum {
	BL_REGION_108 = 0,
	BL_REGION_180,
	BL_REGION_240,
	BL_REGION_12,
	BL_REGION_18,
	BL_REGION_END,
} LOCALDIMMING_REGION_SIZE;

typedef struct {
	unsigned short SRAM_Position;
	unsigned short SRAM_Length;
	unsigned short *SRAM_Value;	
}  DRV_LD_LDSPI_DATASRAM_TYPE;

typedef enum {
	BLPF_EDGE_6,
	BLPF_EDGE_12,
	BLPF_EDGE_MAX,
	BLPF_DIRECT_32 = BLPF_EDGE_MAX,
	BLPF_DIRECT_50,
	BLPF_DIRECT_90,
	BLPF_MAX,
}  DRV_BLPF_TYPE;

typedef struct {
	unsigned int bpl_rate;
	unsigned int APL;
	unsigned int bv_data[120];
} BPL_OUTPUT_DATA;

struct VIP_Smart_LD_interface {
	BPL_OUTPUT_DATA (*get_LG_Smart_LD_calculation)(unsigned int BPL_mode, unsigned int BV_data[120], unsigned int LD_zone);
};

typedef struct {
	unsigned short AlgoSRAM[40][24];
	unsigned int AlgoSRAM_avg;
}LED_AlgoSRAM;

typedef struct {
    unsigned short ai_brightness;
    unsigned short sensor_level;
}LED_BPL_info;

typedef struct {
	unsigned int max_table[2400]; 
	unsigned int avg_table[2400]; 
	unsigned int WholeBPL;
	unsigned int Global_APL;
	unsigned int Leakage_Level;
}LED_BV_max_avg_table;

typedef struct{
	unsigned int Leakage_TH1;
	unsigned int Leakage_TH2;
}DRV_LD_Leakage_TH;

void drvif_color_set_LD_interrupt_Enable(unsigned char enable);
void drvif_color_set_Smart_LD_hw_softwareMode_Enable(unsigned char enable);
void drvif_color_set_LD_Enable(unsigned char enable);
void drvif_color_set_LD_Global_Ctrl(DRV_LD_Global_Ctrl *ptr);
void drvif_color_set_LD_Backlight_Decision(DRV_LD_Backlight_Decision *ptr);
void drvif_color_set_LD_Spatial_Filter(DRV_LD_Spatial_Filter *ptr);
void drvif_color_set_LD_Spatial_Remap(DRV_LD_Spatial_Remap *ptr);
void drvif_color_set_LD_Boost(DRV_LD_Boost *ptr);
void drvif_color_set_LD_Temporal_Filter(DRV_LD_Temporal_Filter *ptr);
void drvif_color_set_LD_Backlight_Final_Decision(DRV_LD_Backlight_Final_Decision *ptr);
void drvif_color_get_LD_Backlight_Final_Decision(DRV_LD_Backlight_Final_Decision *ptr);
void drvif_color_set_LD_Data_Compensation(DRV_LD_Data_Compensation *ptr);
void drvif_color_set_LD_Data_Compensation_NewMode_2DTable(DRV_LD_Data_Compensation_NewMode_2DTable *ptr);
void drvif_color_set_LD_Backlight_Profile_Interpolation(DRV_LD_Backlight_Profile_Interpolation *ptr);
void drvif_color_get_LD_Backlight_Profile_Interpolation(DRV_LD_Backlight_Profile_Interpolation *ptr);
void drvif_color_set_LD_BL_Profile_Interpolation_Table(unsigned int *ptr);
void drvif_color_set_LD_BL_Profile_Interpolation_Table_HV(unsigned int *Hptr, unsigned int *Vptr);
void drvif_color_set_LD_Demo_Window(DRV_LD_Demo_Window *ptr);
void drvif_color_set_LD_Backlight_Profile_Interpolation_Table(int ptr[6][4][20]);
void drvif_color_get_LD_Global_Ctrl(DRV_LD_Global_Ctrl *ptr);
void drvif_color_get_LD_Spatial_Filter(DRV_LD_Spatial_Filter *ptr);
unsigned char drvif_color_get_LD_APL_ave( unsigned int* APL_data );
unsigned char drvif_color_get_LD_APL_max( unsigned int* APL_data );
unsigned char drvif_color_get_LD_APL_max_avg(void);
unsigned char drvif_color_get_LD_8bin( unsigned short blk_start, unsigned short read_num, unsigned int** blk_hist );
unsigned char drvif_color_get_LD_8bin_max_ave( unsigned short blk_start, unsigned short read_num, unsigned int** blk_hist, unsigned int* blk_max, unsigned int* blk_ave );

void drvif_HAL_VPQ_LED_LDEnable(unsigned char bCtrl);
void drvif_HAL_VPQ_LED_LDEnablePixelCompensation(unsigned char bCtrl);
void drvif_HAL_VPQ_LED_LDCtrlDemoMode(DRV_HAL_VPQ_LED_LDCtrlDemoMode *ptr);
void drvif_HAL_VPQ_LED_LDCtrlSPI(unsigned char *LDCtrlSPI);
unsigned char drvif_color_get_LD_GetAPL_TV006(unsigned short *BLValue);
void drvif_color_set_LDSPI_DataSRAM_Data_Continuous(DRV_LD_LDSPI_DATASRAM_TYPE *LDSPI_DataSRAM, unsigned char Wait_Tx_Done);

char drvif_color_Set_Local_Dimming_Object_Gain_DMA_Enable(unsigned char Enable, unsigned int phy_Addr, unsigned short* vir_Addr, unsigned short H_Num, unsigned short V_Num, unsigned char line_mode, unsigned char bit_gain, unsigned char gain_shift);
char drvif_color_Set_Local_Dimming_Object_Gain_Addr_Boundary(unsigned int addr_up_limit, unsigned int addr_low_limit);
void drvif_color_Set_Local_Dimming_Full(unsigned char nValue);
void drvif_color_Set_Local_Dimming_Off(unsigned char nValue);
void drvif_color_Local_Dimming_Init(unsigned char nValue);
void drvif_color_LD_HDR_OverCurren_Demo(unsigned char Enable);
void drvif_color_set_LDSPI_TxDoneIntEn(unsigned char enable);
void drvif_color_set_LD_AlgoSRAM_SWMode_En(unsigned char enable);
char drvif_color_get_LD_AlgoSRAM(unsigned int length, unsigned short *BLtable);
void drvif_color_set_LD_Algo_sram(LED_AlgoSRAM* ptr);
void drvif_color_set_Smart_LD_hw_softwareMode_Enable(unsigned char enable);
void drvif_color_set_LED_BPL_DATA(unsigned short ai_brightness, unsigned short sensor_level);
void drvif_color_LED_BPL_calculate(unsigned char Enable);
void drvif_color_set_Smart_LD(struct VIP_Smart_LD_interface *pvip_smart_ld);
void drvif_color_Smart_LD(unsigned char Enable);
void drvif_color_get_LD_Backlight_Decision(DRV_LD_Backlight_Decision *ptr);

void drvif_color_get_LD_Avg_Bv(DRV_LD_Average_Bv *ptr);
void drvif_color_set_LD_3x3LPF(DRV_LD_Backlight_LPF *ptr);
void drvif_color_get_LD_3x3LPF(DRV_LD_Backlight_LPF *ptr);
void drvif_color_set_LD_Spatial_Remap3(DRV_LD_Spatial_Remap3 *ptr);
void drvif_color_get_LD_Spatial_Remap3(DRV_LD_Spatial_Remap3 *ptr);
void drvif_color_set_LD_Global_BV_Gain(DRV_LD_Global_Gain *ptr);
void drvif_color_get_LD_Global_BV_Gain(DRV_LD_Global_Gain *ptr);
void drvif_color_set_LD_AI_BV_Gain(DRV_LD_Global_Gain *ptr);
void drvif_color_get_LD_AI_BV_Gain(DRV_LD_Global_Gain *ptr);
void drvif_color_set_LD_ABI_BV_Gain(DRV_LD_Global_Gain *ptr);
void drvif_color_get_LD_ABI_BV_Gain(DRV_LD_Global_Gain *ptr);
void drvif_color_set_LD_Spatial_Remap2(DRV_LD_Spatial_Remap2 *ptr);
void drvif_color_set_LD_Pixel_Bv_Gain_Offset(DRV_LD_Bl_Pixel_Bv_Gain_Offset *ptr);
void drvif_color_get_LD_Pixel_Bv_Gain_Offset(DRV_LD_Bl_Pixel_Bv_Gain_Offset *ptr);

unsigned short drvif_color_get_LD_RGB_Max(void);
unsigned char drvif_color_get_LD_RGB_Max_Hist(unsigned short RGB_Max_Hist[16]);
unsigned char drvif_color_get_LD_APL_ave_by_num( unsigned short* APL_data, unsigned short array_num );
unsigned char drvif_color_get_LD_APL_max_by_num( unsigned short* APL_data, unsigned short array_num );

void drvif_color_set_leakage_th(DRV_LD_Leakage_TH *ptr);
void drvif_color_get_leakage_th(DRV_LD_Leakage_TH *ptr);
unsigned int drvif_color_get_LD_Leakage_Level_HW(void);
void drvif_color_get_LD_Avg_Bv_pif2(DRV_LD_Average_Bv *ptr);
void drvif_HAL_VPQ_LED_LDCtrlDemoMode_new(DRV_HAL_VPQ_LED_LDCtrlDemoMode *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __LOCALDIMMING_H__*/


