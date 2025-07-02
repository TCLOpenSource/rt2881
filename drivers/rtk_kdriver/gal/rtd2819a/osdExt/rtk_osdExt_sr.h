

#ifndef _RTK_OSDEXT_SR_H__
#define _RTK_OSDEXT_SR_H__





#ifdef RTK_GDMA_TEST_OSD1
int GDMA_OSD1_DevCB_initSR(void);
int GDMA_OSD1_DevCB_preset_SR(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic);
int GDMA_OSD1_DevCB_set_SR(uint8_t enable, GDMA_DISPLAY_PLANE disPlane, GDMA_PIC_DATA* curPic);
#endif//


int GDMA_OSD4_DevCB_initSR(void);
int GDMA_OSD4_DevCB_preset_SR(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic);
int GDMA_OSD4_DevCB_set_SR(uint8_t enable, GDMA_DISPLAY_PLANE disPlane, GDMA_PIC_DATA* curPic);

//int GDMAExt_SR_PreScale(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic );


int GDMA_OSD5_DevCB_initSR(void);
int GDMA_OSD5_DevCB_preset_SR(int enable, VO_RECTANGLE srcWin, VO_RECTANGLE dispWin, GDMA_PIC_DATA* curPic);
int GDMA_OSD5_DevCB_set_SR(uint8_t enable, GDMA_DISPLAY_PLANE disPlane, GDMA_PIC_DATA* curPic);

#endif//