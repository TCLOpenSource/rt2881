#ifndef	__DOLBYVISION_COMMON_h__
#define	__DOLBYVISION_COMMON_h__

//hal/kadp/kdriver use same structure
typedef enum {
	DOLBY_VISION_BACKLIGHT_PWM = 0,    //dolby set pwm backlight(default)
	DOLBY_VISION_BACKLIGHT_LOCAL_DIMMING,         //dolby set ld backlight
	DOLBY_VISION_BACKLIGHT_GLOBAL_ADIM_PDIM   //dolby set adim, pdim global dimming
} DV_BL_TYPE;



typedef struct _KADP_VPQ_DOBYBAKLIGHT_INI{
	int enable_global_dimming;
	int ott_24_delay_frame;
	int ott_60_delay_frame;
	int hdmi_24_delay_frame;
	int hdmi_60_delay_frame;
	int point0_p;
	int point0_a;
	int point25_p;
	int point25_a;
	int point50_p;
	int point50_a;
	int point75_p;
	int point75_a;
	int point100_p;
	int point100_a;
	int point_turn_p;
	int point_turn_a;
	int point_turn_ui;
	int ui_volue;
	int picmode_offset;
}KADP_VPQ_DOBYBAKLIGHT_INI;



#endif
