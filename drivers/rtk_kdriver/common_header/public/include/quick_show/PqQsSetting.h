#ifndef __PQ_QS_SETTING_H__
#define __PQ_QS_SETTING_H__

#include "PanelColorIniContainer.h"
#include "OsdIniContainer.h"

// For Quick Show Developer Reference+
// enum WIDE_MODE
// {
//     WIDE_MODE_WIDE_ZOOM,  // 0: Forsaken
//     WIDE_MODE_UNUSED,     // 1: Forsaken
//     WIDE_MODE_FULL,       // 2: Full
//     WIDE_MODE_ZOOM,       // 3: Forsaken
//     WIDE_MODE_ORIGINAL,   // 4: Pixel to Pixel mode
//     WIDE_MODE_4_3,        // 5: 4:3
//     WIDE_MODE_ZOOM1,      // 6: Zoom, v:90,h:90
//     WIDE_MODE_ZOOM2,      // 7: Wide Zoom, v:80,h:80
//     WIDE_MODE_PANORAMA,   // 8: PANORAMA
//     WIDE_MODE_FIT,
// };

// enum DATA_RANGE_MODE_ENUM
// {
//     DATA_RANGE_AUTO_MODE = 0,
//     DATA_RANGE_FULL_MODE,
//     DATA_RANGE_LIMITED_MODE,
//     DATA_RANGE_MODE_MAX,
// };


// For Quick Show Developer Reference-
DEFINE_STRUCTURE(PqQuickShow_PictureMode,
    unsigned char picMode; // ref: HAL_VPQ_PICTURE_MODE
    unsigned char pcGamingMode; // 1: game mode; 0: pc mode
)

DEFINE_STRUCTURE(PqQuickShow_PictureModeData,
    OSD_PictureModeData defData; // normal timing
    OSD_PictureModeData hdrData; // HDR: HDR10, HDR10_PLUS, HLG, PRIME
)

DEFINE_STRUCTURE(PqQuickShow_PQSrcData,
    PqQuickShow_PictureMode picMode; // user selected picmode
    unsigned char pcMode; // 0: unknown, 1: video mode, 2: pc mode, 3: auto mode
    unsigned char wideMode; // ref: WIDE_MODE for different port
    unsigned char lowLatencyMode; // 0: off, 1: on, 2:auto
    int gainOverDriver;
    bool enVrr;
    bool enDeFlick;
    unsigned int dynaBlackEq;
    unsigned int shadowDetail;
    unsigned int range; // 0: DATA_RANGE_MODE_ENUM::DATA_RANGE_AUTO_MODE
    PqQuickShow_PictureModeData userSelModeData;
    PqQuickShow_PictureModeData pcModeData;
    PqQuickShow_PictureModeData gameModeData;
    OSD_ColorTempData colorTempData[COLOR_TEMP_MODE_ITEM_CNT]; // 0: User, 1~9
    OSD_ColorCurveData colorCurve;  // DP and TypeC need to use different setting
    OSD_TenPointsGamma tenPointsGamma;
    OSD_ThreeD_LUT threeDLut;
)

DEFINE_STRUCTURE(PqQuickShow_PQ_QsSettings,
    PqQuickShow_PQSrcData hdmiData[MAX_PORT_NUM];
    PqQuickShow_PQSrcData dpData[MAX_PORT_NUM];
    unsigned int uCRC32;
)

DEFINE_STRUCTURE(PqQuickShow_Panel_QsSettings,
    PANEL_ColorGammaTblData colorGammaData;
    unsigned int uCRC32;
)

// ****************************************************************
// **********below is for pq qs driver define**********************
// ****************************************************************
// struture define should be same as TV servervice define
// PqQuickShow_PQSrcData = PQSrcData_Ver2
// PqQuickShow_Panel_QsSettings = PqPanelColorData
/********************** QS bin ****************************************/
typedef struct {
    unsigned char picMode;          // ref: HAL_VPQ_PICTURE_MODE
    unsigned char pcGamingMode; // 1: game mode; 0: pc mode
} PictureMode;

// Please take care this struct, we should add a new item
// in the tail to avoid mismatched data in DB.
typedef struct {
    int Brightness;
    int Contrast;
    int Saturation;
    int Hue;
    int Sharpness;
    int ColorTemperature;
    int Gamma;
    int BlueStretch;
    int MpegNR;
    int NoiseReduction;
    int DCC_Index;
    int DCC_Level;
    int WhiteExtension;
    int BlackExtension;
    int Backlight;
    int ICM_Index;
    int LC_Index;
    int LC_Level;
    int ColorSpace;
    int LD;
    int After_Filter_En;
    int HDR10_Curve;
    int HDR10_Dynamic_En;
    int HLG_Lut_Offset;
    int MEMC_Real_Cinema;
    int MEMC_Level;
    int Low_Blue_Level;
    int Smooth_Gradation;
    int DBC_En;
    int Overscan_En;
    int RFRC;
    int DarkDetail;
} PictureMode_Data;

typedef struct {
    PictureMode_Data defData; // normal timing
    PictureMode_Data hdrData; // HDR: HDR10, HDR10_PLUS, HLG, PRIME
} PictureModeData_Ver2;

typedef struct {
    uint32_t redGain;
    uint32_t greenGain;
    uint32_t blueGain;
    uint32_t redOffset;
    uint32_t greenOffset;
    uint32_t blueOffset;
    uint32_t gamma_curve_index;
} DRV_ColorTempDataEx;

#ifdef OSD_FOUR_LEVEL_MAPPING_ENABLE
typedef struct {
    unsigned char Brightness_0;
    unsigned char Brightness_25;
    unsigned char Brightness_50;
    unsigned char Brightness_75;
    unsigned char Brightness_100;
    unsigned char Contrast_0;
    unsigned char Contrast_25;
    unsigned char Contrast_50;
    unsigned char Contrast_75;
    unsigned char Contrast_100;
    unsigned char Saturation_0;
    unsigned char Saturation_25;
    unsigned char Saturation_50;
    unsigned char Saturation_75;
    unsigned char Saturation_100;
    unsigned char Hue_0;
    unsigned char Hue_25;
    unsigned char Hue_50;
    unsigned char Hue_75;
    unsigned char Hue_100;
    unsigned char Sharpness_0;
    unsigned char Sharpness_25;
    unsigned char Sharpness_50;
    unsigned char Sharpness_75;
    unsigned char Sharpness_100;
    unsigned char Backlight_0;
    unsigned char Backlight_25;
    unsigned char Backlight_50;
    unsigned char Backlight_75;
    unsigned char Backlight_100;
} DRV_StructColorDataFacModeType;

typedef struct
{
    unsigned char LpfGain_0;
    unsigned char LpfGain_25;
    unsigned char LpfGain_50;
    unsigned char LpfGain_75;
    unsigned char LpfGain_100;
}LPF_Gain;
#else
typedef struct {
    unsigned char Brightness_0;
    unsigned char Brightness_50;
    unsigned char Brightness_100;
    unsigned char Contrast_0;
    unsigned char Contrast_50;
    unsigned char Contrast_100;
    unsigned char Saturation_0;
    unsigned char Saturation_50;
    unsigned char Saturation_100;
    unsigned char Hue_0;
    unsigned char Hue_50;
    unsigned char Hue_100;
    unsigned char Sharpness_0;
    unsigned char Sharpness_50;
    unsigned char Sharpness_100;
    unsigned char Backlight_0;
    unsigned char Backlight_50;
    unsigned char Backlight_100;
} DRV_StructColorDataFacModeType;

typedef struct {
    unsigned char LpfGain_0;
    unsigned char LpfGain_50;
    unsigned char LpfGain_100;
} LPF_Gain;
#endif

typedef struct {
    DRV_StructColorDataFacModeType basicCurveData;
    LPF_Gain                       advCurveData;
} ColorCurveData;

typedef struct {
    int R;
    int G;
    int B;
} DRV_RGB;

typedef struct {
    DRV_RGB data[10];
} TenPointsGamma;

/*typedef struct {
    RGB data[10];
}Gamma10P;*/
typedef TenPointsGamma Gamma10P;

typedef struct {
    int EN;
    DRV_RGB red;
    DRV_RGB green;
    DRV_RGB blue;
    DRV_RGB yellow;
    DRV_RGB magenta;
    DRV_RGB cyan;
} LUT_3D;

typedef struct
{
    DRV_RGB red;
    DRV_RGB green;
    DRV_RGB blue;
    DRV_RGB yellow;
    DRV_RGB magenta;
    DRV_RGB cyan;
} ThreeD_LUT_HAL;

typedef struct
{
    int32_t enable;
    ThreeD_LUT_HAL data;
} ThreeD_LUT;

typedef struct {
    PictureMode picMode; // user selected picmode
    unsigned char pcMode; // 0: unknown, 1: video mode, 2: pc mode, 3: auto mode
    unsigned char wideMode; // ref: WIDE_MODE for different port
    unsigned char lowLatencyMode; // 0: off, 1: on, 2:auto
    int gainOverDriver;
    bool enVrr;
    bool enDeFlick;
    unsigned int dynaBlackEq;
    unsigned int shadowDetail;
    unsigned int range;
    PictureModeData_Ver2 userSelModeData;
    PictureModeData_Ver2 pcModeData;
    PictureModeData_Ver2 gameModeData;
    DRV_ColorTempDataEx colorTempData[COLOR_TEMP_MODE_ITEM_CNT];// 0: User, 1~9
    ColorCurveData colorCurve; // DP and TypeC need to use different setting
    TenPointsGamma tenPointsGamma;
    ThreeD_LUT threeDLut;
} PQSrcData_Ver2;

struct PQ_QsSettings_ver2
{
    PQSrcData_Ver2 hdmiData[MAX_PORT_NUM];
    PQSrcData_Ver2 dpData[MAX_PORT_NUM];
    unsigned int crc;
};

/**********************panel ini ****************************************/
typedef struct {
    unsigned int rdata[GAMMA_TABLE_RGB_DATA_SIZE];
    unsigned int gdata[GAMMA_TABLE_RGB_DATA_SIZE];
    unsigned int bdata[GAMMA_TABLE_RGB_DATA_SIZE];
} GammaTBLData;

typedef struct {
    unsigned int RedX;
    unsigned int RedY;
    unsigned int GreenX;
    unsigned int GreenY;
    unsigned int BlueX;
    unsigned int BlueY;
    unsigned int WhiteX;
    unsigned int WhiteY;
    unsigned int LumaMax;
    unsigned int LumaMin;
} ColorChromaticity;

typedef struct {
    unsigned int mGammaBitNum;
    GammaTBLData mGammaTableData[GAMMA_TABLE_CNT];
    ColorChromaticity mColorChromaticity;
} ColorGammaTblData;

typedef struct{
    ColorGammaTblData mColorGammaData;
    unsigned int crc;
} PqPanelColorData;

#if 1
// ****************************************************************
// assert processsor, check PqQuickShow_PQSrcData = PQSrcData_Ver2?
// assert processsor, check PqQuickShow_Panel_QsSettings = PqPanelColorData?
// ****************************************************************
// struture define should be same as TV servervice define
// PqQuickShow_PQSrcData = PQSrcData_Ver2
// PqQuickShow_Panel_QsSettings = PqPanelColorData
#define COMPARE_STRUCT_SIZE(A, B) \
    typedef char A##_size_check[sizeof(A) == sizeof(B) ? 1 : -1]

COMPARE_STRUCT_SIZE(PqQuickShow_PQSrcData, PQSrcData_Ver2);
COMPARE_STRUCT_SIZE(PqQuickShow_Panel_QsSettings, PqPanelColorData);
#endif
#endif // __PQ_QS_SETTING_H__
