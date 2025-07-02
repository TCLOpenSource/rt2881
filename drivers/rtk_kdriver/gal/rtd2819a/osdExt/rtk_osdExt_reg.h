#ifndef __RTK_OSDEXT_REG_H__
#define __RTK_OSDEXT_REG_H__

// common_header/rbus/rtd6951



#include <gal/rtk_gdma_driver.h> //rtk_gdma_driver.h already include "rtk_osdExt_rbus.h"

// osd 1: GDMA_REG_CONTENT



typedef enum {
  GMDA_OSD_DONE_MODE_SINGLE,

  GMDA_OSD_DONE_MODE_SYNC45,

 
} GMDA_OSD_DONE_MODE;

int GDMAExt_init_register_callback(void);

void GDMAExt_config_fbc( GDMA_DISPLAY_PLANE disPlane );
void GDMAExt_getDispSize(GDMA_DISPLAY_PLANE plane, VO_RECTANGLE *disp_rec);


void GDMAExt_config_line_buffer( GDMA_DISPLAY_PLANE plane, int is_4k );
void GDMAExt_config_mid_blend(void );

void GDMAExt_ClkEnable(GDMA_DISPLAY_PLANE plane, int enable);

int GDMAExt_modify_mixer(GDMA_MODIFY_MIXER_CMD* mixer_cmd);


//void gdmaExt_config_video(int enable);

//OSD4
int GDMA_OSD4_DevCB_set_ProgDone(GDMA_DISPLAY_PLANE disPlane, GMDA_OSD_DONE_MODE mode, void *info);

//OSD5
int GDMA_OSD5_DevCB_init(GDMA_DISPLAY_PLANE plane, void *info);

int GDMA_OSD5_DevCB_set_PresetOSD(GDMA_DISPLAY_PLANE plane, GDMA_PIC_DATA *curPic, VO_RECTANGLE* srcWin, VO_RECTANGLE* dispWin,
                                 struct gdma_receive_work *ptr_work);

int GDMA_OSD5_DevCB_SetGDMA(GDMA_DISPLAY_PLANE disPlane, void *info);

int GDMA_OSD5_DevCB_set_ProgDone(GDMA_DISPLAY_PLANE disPlane, GMDA_OSD_DONE_MODE mode, void *info);
int GDMA_OSD5_DevCB_setMixerDone(GDMA_DISPLAY_PLANE disPlane, int waitFinish, void *info );

int GDMA_OSD5_DevCB_DumpStatus(GDMA_DISPLAY_PLANE plane, int level);



void GDMA_OSD5_preFrame_SetPrecrop( int width, int height );

int GDMA_OSD4_DevCB_ConfigVideoMixerEnable(bool flag);

int GDMA_OSD4_DevCB_ConfigVideoColorRange (CONFIG_VIDEO_COLORKEY_PARA video_color);

void GDMA_OSD5_PQDC_Enable(int enable, GDMA_PIC_DATA *curPic );

void GDMA_OSD5_PQDC_Set( GDMA_WIN *win );
void GDMA_OSD5_PQDC_Init(int enable);

#endif//__RTK_OSDEXT_REG_H__
