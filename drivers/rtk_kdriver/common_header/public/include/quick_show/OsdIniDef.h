#ifndef __OSD_INI_DEFINE_H__
#define __OSD_INI_DEFINE_H__

#include "StructSignature.h"

#define MAX_PORT_NUM 4
// Osd Table Ini
#define OSD_DATA_CATOGORY_ITEM_CNT 4
#define SRC_TYPE_ITEM_CNT 11
#define INFO_ITEM_CNT 5
#define COLOR_CURVE_TYPE_ITEM_CNT 7
#ifdef OSD_FOUR_LEVEL_MAPPING_ENABLE
#define COLOR_CURVE_DATA_SIZE 5
#else
#define COLOR_CURVE_DATA_SIZE 3
#endif
#define COLOR_TEMP_MODE_ITEM_CNT 10
#define COLOR_TEMP_DATA_SIZE 7
#define PICTURE_MODE_ITEM_CNT 13
#define PICTURE_DOLBY_MODE_ITEM_CNT 7
#define PICTURE_DATA_ITEM_CNT 32

// Panel Color Ini
#define PANEL_DATA_CATOGORY_ITEM_CNT 13
#define GAMMA_BIT_NUM_ITEM_CNT 1
#define GAMMA_TABLE_ITEM_CNT 3
#define COLOR_CHROMATICITY_ITEM_CNT 10
#define GAMMA_TABLE_RGB_DATA_SIZE 1024
#define GAMMA_TABLE_CNT 10

// Osd Minor Ini
#define TEN_POINTS_GAMMA 10
#define LUT_COLOR_CNT 6
#define RGB_CNT 3
#define MEMC_TAG_CNT 4
#define MEMC_LEVEL_CNT 5

#define SRC_TYPE_ITEM \
    X(Default) \
    X(DP) \
    X(TypeC) \
    X(HDMI) \
    X(VO) \
    X(Miracast) \
    X(AirPlay) \
    X(ATV) \
    X(DTV) \
    X(AV) \
    X(YPbPr) \

#define OSD_DATA_CATOGORY_ITEM \
    X(OSD_INI_INFO) \
    X(ColorDataFacCurve) \
    X(ColorTempData) \
    X(PictureModeData) \

#define INFO_ITEM \
    X(ProjectID) \
    X(OEM) \
    X(Version) \
    X(PanelID) \
    X(OEMVersion) \

#define COLOR_CURVE_DATA_ITEM \
    X(Brightness) \
    X(Contrast) \
    X(Saturation) \
    X(Hue) \
    X(Sharpness) \
    X(Backlight) \
    X(LpfGain) \

#define COLOR_TEMP_MODE_ITEM \
    X(User) \
    X(1) \
    X(2) \
    X(3) \
    X(4) \
    X(5) \
    X(6) \
    X(7) \
    X(8) \
    X(9) \

#define PICTURE_MODE_ITEM \
    X(Standard) \
    X(Dynamic) \
    X(Movie) \
    X(ECO) \
    X(Game) \
    X(Gentle) \
    X(Vivid) \
    X(Sport) \
    X(PC) \
    X(LowBlue) \
    X(ImaxEnh) \
    X(User) \
    X(FilmMaker)

#define PICTURE_DOLBY_MODE_ITEM \
    X(Bright) \
    X(Normal) \
    X(Dark) \
    X(IQ) \
    X(Game) \
    X(FilmMaker) \
    X(IQ_FilmMaker) \

// Please take care this enum, we should add a new item
// in the tail to avoid mismatched data in DB.
#define PICTURE_DATA_ITEM \
    X(Brightness) \
    X(Contrast) \
    X(Saturation) \
    X(Hue) \
    X(Sharpness) \
    X(ColorTemperature) \
    X(Gamma) \
    X(BlueStretch) \
    X(MpegNR) \
    X(NoiseReduction) \
    X(DCC_Index) \
    X(DCC_Level) \
    X(WhiteExtension) \
    X(BlackExtension) \
    X(Backlight) \
    X(ICM_Index) \
    X(LC_Index) \
    X(LC_Level) \
    X(ColorSpace) \
    X(LD) \
    X(After_Filter_En) \
    X(HDR10_Curve) \
    X(HDR10_Dynamic_En) \
    X(HLG_Lut_Offset) \
    X(MEMC_Real_Cinema)\
    X(MEMC_Level)\
    X(Low_Blue_Level) \
    X(Smooth_Gradation) \
    X(DBC_En) \
    X(Overscan_En) \
    X(RFRC) \
    X(DarkDetail) \

#define COLOR_TEMP_DATA_ITEM \
    X(RedGain) \
    X(GreenGain) \
    X(BlueGain) \
    X(RedOffset) \
    X(GreenOffset) \
    X(BlueOffset) \
    X(GammaCurveIndex) \

#define PANEL_DATA_CATOGORY_ITEM \
    X(PANEL_COLOR_INI_INFO) \
    X(GAMMA_BIT_NUMBER) \
    X(PRESET_GAMMA_TABLE_0) \
    X(PRESET_GAMMA_TABLE_1) \
    X(PRESET_GAMMA_TABLE_2) \
    X(PRESET_GAMMA_TABLE_3) \
    X(PRESET_GAMMA_TABLE_4) \
    X(PRESET_GAMMA_TABLE_5) \
    X(PRESET_GAMMA_TABLE_6) \
    X(PRESET_GAMMA_TABLE_7) \
    X(PRESET_GAMMA_TABLE_8) \
    X(PRESET_GAMMA_TABLE_9) \
    X(COLOR_CHROMATICITY) \

#define GAMMA_BIT_NUM_ITEM \
    X(Gamma_bit) \

#define GAMMA_TABLE_DATA_ITEM \
    X(R_VALUE) \
    X(G_VALUE) \
    X(B_VALUE) \

#define COLOR_CHROMATICITY_INFO_ITEM \
    X(Panel_Red_X) \
    X(Panel_Red_Y) \
    X(Panel_Green_X) \
    X(Panel_Green_Y) \
    X(Panel_Blue_X) \
    X(Panel_Blue_Y) \
    X(Panel_White_X) \
    X(Panel_White_Y) \
    X(Luma_Max) \
    X(Luma_Min) \

#define MINOR_MEMC_DATA_TAG \
    X(LEVEL) \
    X(DEJUDDER_LEVEL) \
    X(DEBLUR_LEVEL) \
    X(MOTION_LEVEL) \

#define MINOR_LUT_COLOR \
    X(Red) \
    X(Green) \
    X(Blue) \
    X(Yellow) \
    X(Magenta) \
    X(Cyan) \

#define MACROSTR(k) #k

typedef enum
{
#define X(Enum)       InputSrc_##Enum,
    SRC_TYPE_ITEM
#undef X
} SourceTypeEnum;

typedef enum
{
#define X(Enum)       OsdCat_##Enum,
    OSD_DATA_CATOGORY_ITEM
#undef X
} OsdDataCatogoryEnum;

typedef enum{
#define X(Enum)       InfoData_##Enum,
    INFO_ITEM
#undef X
} InfoDataEnum;

typedef enum{
#define X(Enum)       ColorCurve_##Enum,
    COLOR_CURVE_DATA_ITEM
#undef X
} ColorCurveEnum;

typedef enum{
#define X(Enum)       ColorTempData_##Enum,
    COLOR_TEMP_DATA_ITEM
#undef X
} ColorTempDataItemEnum;

typedef enum{
#define X(Enum)       ColorTemp_##Enum,
    COLOR_TEMP_MODE_ITEM
#undef X
} ColorTempModeEnum;

typedef enum{
#define X(Enum)       Picture_##Enum,
    PICTURE_MODE_ITEM
#undef X
} PictureModeEnum;

typedef enum{
#define X(Enum)       PictureDolby_##Enum,
    PICTURE_DOLBY_MODE_ITEM
#undef X
} PictureDolbyModeEnum;

typedef enum{
#define X(Enum)       PictureData_##Enum,
    PICTURE_DATA_ITEM
#undef X
} PictureDataItemEnum;

typedef enum{
#define X(Enum)       Panel_##Enum,
    PANEL_DATA_CATOGORY_ITEM
#undef X
} PanelDataCatogoryEnum;

typedef enum{
#define X(Enum)       LutColor_##Enum,
    MINOR_LUT_COLOR
#undef X
} LutColorEnum;

// Signature for enum
// #ifdef X
// #undef X
// #endif
// #define X(Enum) #Enum ";"
// static const char generate_ColorCurveEnum_signature[] =
//     COLOR_CURVE_DATA_ITEM
// ;

// static const char generate_ColorTempModeEnum_signature[] =
//     COLOR_TEMP_MODE_ITEM
// ;

// static const char generate_PictureModeEnum_signature[] =
//     PICTURE_MODE_ITEM
// ;

// static const char generate_PictureDolbyModeEnum_signature[] =
//     PICTURE_DOLBY_MODE_ITEM
// ;

// static const char generate_PictureDataItemEnum_signature[] =
//     PICTURE_DATA_ITEM
// ;
// #undef X

#endif // __OSD_INI_DEFINE_H__