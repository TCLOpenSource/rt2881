#ifndef __OSD_INI_CONTAINER_H__
#define __OSD_INI_CONTAINER_H__

#include "OsdIniDef.h"

DEFINE_STRUCTURE(OSD_ColorCurveData,
    unsigned char data[COLOR_CURVE_TYPE_ITEM_CNT][COLOR_CURVE_DATA_SIZE];
)

DEFINE_STRUCTURE(OSD_ColorTempData,
    int32_t data[COLOR_TEMP_DATA_SIZE];
)

DEFINE_STRUCTURE(OSD_ColorTempDataSet,
    OSD_ColorTempData colorTempMode[COLOR_TEMP_MODE_ITEM_CNT];
)

DEFINE_STRUCTURE(OSD_PictureModeData,
    int32_t data[PICTURE_DATA_ITEM_CNT];
)

DEFINE_STRUCTURE(OSD_PictureDataSet,
    OSD_PictureModeData sdrData[PICTURE_MODE_ITEM_CNT];
    OSD_PictureModeData hdrData[PICTURE_MODE_ITEM_CNT]; // {HDR10, HDR10_PLUS, HLG, PRIME};
    OSD_PictureModeData dolbyData[PICTURE_DOLBY_MODE_ITEM_CNT];
)

DEFINE_STRUCTURE(OsdIni,
    OSD_ColorCurveData   colorCurveDataSet[SRC_TYPE_ITEM_CNT];
    OSD_ColorTempDataSet colorTempDataSet[SRC_TYPE_ITEM_CNT];
    OSD_PictureDataSet   pictureDataSet[SRC_TYPE_ITEM_CNT];
)

DEFINE_STRUCTURE(OSD_RGB,
    int32_t R;
    int32_t G;
    int32_t B;
)

DEFINE_STRUCTURE(OSD_TenPointsGamma,
    OSD_RGB data[10];
)

DEFINE_STRUCTURE(OSD_ThreeD_LUT,
    int32_t enable;
    OSD_RGB data[LUT_COLOR_CNT];
)

DEFINE_STRUCTURE(OSD_MemcData,
    uint32_t dejudderLevel;
    uint32_t deblurLevel;
    uint32_t motionLevel;
)

DEFINE_STRUCTURE(OsdMinorIni,
    OSD_TenPointsGamma tenPointsGamma;
    OSD_ThreeD_LUT threeDLut;
    OSD_MemcData memc[MEMC_LEVEL_CNT];
)

#endif // __OSD_INI_CONTAINER_H__