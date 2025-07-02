
#include <linux/of_irq.h>
#include "rtk_osdExt_driver.h"
#include "rtk_osdExt_reg.h"

#include <gal/rtk_gdma_export_user.h>// GRAPHIC_LAYERS_OBJECT, PICTURE_LAYERS_OBJECT





typedef int (*GDMA_DevCB_init)(GDMA_DISPLAY_PLANE plane, void *info);
typedef int (*GDMA_DevCB_deInit)(GDMA_DISPLAY_PLANE plane, void *info);
typedef int (*GDMA_DevCB_SR_Init)(void);

typedef int (*GDMA_DevCB_Update)(GDMA_DISPLAY_PLANE plane, void *info);
  typedef int (*GDMA_DevCB_SetGDMA)(GDMA_DISPLAY_PLANE plane, void *info);
  typedef int (*GDMA_DevCB_SetSR)(GDMA_DISPLAY_PLANE plane, void *info);
  typedef int (*GDMA_DevCB_SetMixer)(GDMA_DISPLAY_PLANE plane, void *info);
typedef int (*GDMA_DevCB_SetProgDone)(GDMA_DISPLAY_PLANE plane, GMDA_OSD_DONE_MODE mode, void *info);
typedef int (*GDMA_DevCB_SetMixerDone)(GDMA_DISPLAY_PLANE plane, int waitFinish, void *info);
typedef int (*GDMA_DevCB_get_isrStatus)(GDMA_DISPLAY_PLANE plane, void *info);
typedef int (*GDMA_DevCB_DumpStatus)(GDMA_DISPLAY_PLANE plane, int level);

  #if 1
  typedef int (*GDMA_DevCB_PreSet_GDMA)(GDMA_DISPLAY_PLANE plane, void *info);
  typedef int (*GDMA_DevCB_PreSet_SR)(GDMA_DISPLAY_PLANE plane, void *info);
  typedef int (*GDMA_DevCB_PreSet_Mixer)(GDMA_DISPLAY_PLANE plane, void *info);
  #endif

  typedef int (*GDMA_DevCB_PreSet_OSD)(GDMA_DISPLAY_PLANE plane, GDMA_PIC_DATA *curPic, VO_RECTANGLE* srcWin, VO_RECTANGLE* dispWin,
                                      struct gdma_receive_work *ptr_work);
  
  typedef int (*GDMA_DevCB_Set_D2)(GDMA_DISPLAY_PLANE plane, void *info);
  typedef int (*GDMA_DevCB_Dump_D2)(GDMA_DISPLAY_PLANE plane, void *info);

  typedef int (*GDMA_DevCB_Suspend)(GDMA_DISPLAY_PLANE plane, void *info);
  typedef int (*GDMA_DevCB_Resume)(GDMA_DISPLAY_PLANE plane, void *info);

/*
ret_status
  0:disabled
  1:enabled
*/
typedef int (*GDMA_DevCB_GetDriverStatus)(GDMA_DISPLAY_PLANE plane, GDMAExt_Status name, void *info, int* ret_status);

typedef struct 
{
  GDMA_DevCB_init init;
  GDMA_DevCB_deInit deInit;
  GDMA_DevCB_SR_Init initSR;

  #if 0
  GDMA_DevCB_PreSet_GDMA presetGDMA;
  GDMA_DevCB_PreSet_SR presetSR;
  GDMA_DevCB_PreSet_Mixer presetMixer;
  #endif//

  GDMA_DevCB_PreSet_OSD presetOSD;

  GDMA_DevCB_SetGDMA setGDMA;
  GDMA_DevCB_SetSR setSR;
  GDMA_DevCB_SetMixer setMixer;

  GDMA_DevCB_Suspend suspendOSD;
  GDMA_DevCB_Resume resumeOSD;

  GDMA_DevCB_GetDriverStatus getDriverStatus;
  GDMA_DevCB_DumpStatus  dumpStatus;


  GDMA_DevCB_SetMixerDone setMixerDone;
  GDMA_DevCB_SetProgDone set_ProgDone;
  GDMA_DevCB_get_isrStatus get_isrStatus;
  
  GDMA_DevCB_Set_D2 setD2;
  GDMA_DevCB_Dump_D2 dumpD2;

} GDMAExt_device_cb; 

int GDMAExt_init( struct platform_device* pdev );

int GDMAExt_init_main(gdma_dev* gdma);
int GDMAExt_init_selectClk(gdma_dev* gdma);

int GDMAExt_init_interrupt(GDMA_DISPLAY_PLANE plane, int enable ); //register side

void GDMAExt_getDispSize(GDMA_DISPLAY_PLANE plane, VO_RECTANGLE *disp_rec);

int GDMAExt_IS_EXIST(GDMA_DISPLAY_PLANE plane );

int GDMAExt_startHandleLayers( GRAPHIC_LAYERS_OBJECT_V3 *data );
int GDMAExt_ReceiveGraphicLayers(GRAPHIC_LAYERS_OBJECT_V3 *data, PICTURE_LAYERS_OBJECT* send);
int GDMAExt_ReceiveGraphicLayers_exp(GRAPHIC_LAYERS_OBJECT_V3 *data);

GDMAEXT_OSD_MODE GDMAExt_getOSDMode(void );
void GDMAExt_setOSDMode( GDMAEXT_OSD_MODE mode);

void GDMAExt_dumpOSDMode(void );

//int GDMAExt_ReceiveGraphicLayersV3_core(GRAPHIC_LAYERS_OBJECT_V3 *data);

int GDMAExt_ReceivePicture(PICTURE_LAYERS_OBJECT *data);
  int GDMAExt_PreOSDReg_Set(struct gdma_receive_work *ptr_work);

//internal ?
int GDMAExt_PreUpdate(struct gdma_receive_work *ptr_work);

int GDMAExt_enable_Sync2OSD( int enable);

int GDMAExt_disalbe_osdHW(int plane);

int GDMAExt_SwitchVsyncSource(GDMA_DISPLAY_PLANE plane );

int GDMAExt_Suspend(GDMA_DISPLAY_PLANE plane );
int GDMAExt_Resume(GDMA_DISPLAY_PLANE plane );


