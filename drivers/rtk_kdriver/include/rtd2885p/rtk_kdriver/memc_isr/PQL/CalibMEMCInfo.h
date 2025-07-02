#ifndef _CALIBMEMCINFO_H
#define _CALIBMEMCINFO_H

#ifdef __cplusplus
extern "C" {
#endif
#define TAG_NAME_MEMC "MEMC"

#define _MIN_(x,y) (((x)<=(y))? (x) : (y))
#define _MAX_(x,y) (((x)>=(y))? (x) : (y))

typedef struct
{
	unsigned int 	u24_Gain_Scale;
	unsigned char	u8_Gain_Shift;
	int				s32_Offset;
}_CALIB_SINGLE_PARAM;

typedef struct
{
	unsigned int 	u10_Gain_Scale_X;
	unsigned char	u4_Gain_Shift_X;
	unsigned int 	u10_Gain_Scale_Y;
	unsigned char	u4_Gain_Shift_Y;
}_CALIB_RESOLUTION_PARAM;

typedef enum{
	UHD_PARAM_To_CUR_RES,
	UHD_RATIO_To_CUR_RES,
	CALIB_PARAM_RES_NUM,
}_CALIB_PARAM_RES_INDEX;

typedef enum{
	PARAM_DIRECTION_X,
	PARAM_DIRECTION_Y,
	PARAM_DIRECTION_AREA,
	PARAM_DIRECTION_NUM,
}_CALIB_PARAM_DERECTION_INDEX;

typedef enum{
	CALIB_PARAM_APL = 0,
	CALIB_PARAM_DTL,
	CALIB_PARAM_SAD,
	CALIB_PARAM_SC,
	CALIB_PARAM_MV,
	CALIB_PARAM_MVRANGE,
	CALIB_PARAM_GMV_CNT,
	CALIB_PARAM_LOGO,
	CALIB_PARAM_DHLOGO_SAD_TH,
	CALIB_PARAM_DHLOGO_MV_TH,
	CALIB_PARAM_DHLOGO_MV_DIFF_TH,
	CALIB_PARAM_PERIODIC_CNT,
	CALIB_PARAM_NUM,
}_CALIB_PARAM_INDEX;

void CalibMEMCInfo_init(void);
unsigned int ReadComReg_Calib_Data(unsigned int u32_Data, unsigned char param_index, int u32_Min, unsigned int u32_Max, bool u1_RegionData);
unsigned int Calib_Data(unsigned int u32_Data, unsigned int u32_Gain, unsigned char u8_Shift);
int Calib_MV(int s32_Data);
unsigned int Convert_UHD_Param_To_Current_RES(unsigned int Param, unsigned char Param_Direction);
unsigned int Convert_Current_RES_RATIO_To_UHD(unsigned int Ratio, unsigned char Param_Direction);

#ifdef __cplusplus
}
#endif

#endif

