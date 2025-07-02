#ifndef __PANEL_COLOR_INI_CONTAINER_H__
#define __PANEL_COLOR_INI_CONTAINER_H__

#include "OsdIniDef.h"


DEFINE_STRUCTURE(PANEL_GammaTBLData,
    unsigned int rdata[GAMMA_TABLE_RGB_DATA_SIZE];
    unsigned int gdata[GAMMA_TABLE_RGB_DATA_SIZE];
    unsigned int bdata[GAMMA_TABLE_RGB_DATA_SIZE];
)

DEFINE_STRUCTURE(PANEL_ColorChromaticity,
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
)

DEFINE_STRUCTURE(PANEL_ColorGammaTblData,
    unsigned int gammaBitNum;
    PANEL_GammaTBLData gammaTableData[GAMMA_TABLE_CNT];
    PANEL_ColorChromaticity colorChromaticity;
)

#endif // __PANEL_COLOR_INI_CONTAINER_H__