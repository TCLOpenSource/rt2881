#ifndef _DPRX_FUN_H
#define _DPRX_FUN_H

#include "rtk_kdriver/dprx/dprx_types.h"
#include <measure/rtk_measure.h>

typedef unsigned char DPRX_bool;

#define CURRENT_DPRX_PORT   0xFF
/**********************************************************************************************
*
*   External Funtion Declarations
*
**********************************************************************************************/
//Timing info for scaler
extern DPRX_bool drvif_Dprx_GetPortRawTimingInfo(unsigned char port, DPRX_TIMING_INFO_T* timing_info);
#define drvif_Dprx_GetRawTimingInfo(timing_info)   drvif_Dprx_GetPortRawTimingInfo(CURRENT_DPRX_PORT, timing_info) // Use Current Port as input.


/*------------------------------------------------------
 * Func : drvif_Dprx_SetVideoCommonSel
 *
 * Desc : Set video common of the connected DPRX port
 *        (some soc supports multiple video common)
 *
 * Parm : sel : video common sel
 *         DP_VIDEO_COMMON_SEL_NONE : none (off, not output)
 *         DP_VIDEO_COMMON_SEL_1 : video common 1
 *         DP_VIDEO_COMMON_SEL_2 : video common 2
 *         DP_VIDEO_COMMON_SEL_3 : video common 3
 *         DP_VIDEO_COMMON_SEL_4 : video common 4
 *
 * Retn : TRUE  : DPRX status check is OK
 *        FALSE : some error detected
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_SetPortVideoCommonSel(unsigned char port, DP_VIDEO_COMMON_SEL sel);
#define drvif_Dprx_SetVideoCommonSel(sel)   drvif_Dprx_SetPortVideoCommonSel(CURRENT_DPRX_PORT, sel) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortColorSpace
 *
 * Desc : Get Color space info of selected DPRX port
 *
 * Parm : port : selected port
 *
 * Retn : DP_COLOR_SPACE_RGB
 *        DP_COLOR_SPACE_YUV444
 *        DP_COLOR_SPACE_YUV422
 *        DP_COLOR_SPACE_YUV420
 *------------------------------------------------------*/
extern DP_COLOR_SPACE_E drvif_Dprx_GetPortColorSpace(unsigned char port);
#define drvif_Dprx_GetColorSpace()   drvif_Dprx_GetPortColorSpace(CURRENT_DPRX_PORT) // Use Current Port as input.


/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortColorDepth
 *
 * Desc : Get Color depth info of selected DPRX port
 *
 * Parm : port : selected port
 *
 * Retn : DP_COLOR_DEPTH_8B
 *        DP_COLOR_DEPTH_10B
 *        DP_COLOR_DEPTH_12B
 *        DP_COLOR_DEPTH_16B
 *------------------------------------------------------*/
extern DP_COLOR_DEPTH_E drvif_Dprx_GetPortColorDepth(unsigned char port);
#define drvif_Dprx_GetColorDepth()   drvif_Dprx_GetPortColorDepth(CURRENT_DPRX_PORT) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortInputType
 *
 * Desc : Get Cable Type of selected DPRX port
 *
 * Parm : port : selected port
 *
 * Retn : DP_TYPE_USB_TYPE_C
 *        DP_TYPE_DP
 *------------------------------------------------------*/
extern DP_TYPE drvif_Dprx_GetPortInputType(unsigned char port);
#define drvif_Dprx_GetInputType()   drvif_Dprx_GetPortInputType(CURRENT_DPRX_PORT) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_CheckPortMode
 *
 * Desc : Check Dprx status of selected DPRX port. this function is
 *        called by VSC periodically to check DPRX status
 *
 * Parm : port : selected port
 *
 * Retn : TRUE  : DPRX status check is OK
 *        FALSE : some error detected
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_CheckPortMode(unsigned char port);
#define drvif_Dprx_CheckMode()   drvif_Dprx_CheckPortMode(CURRENT_DPRX_PORT) // Use Current Port as input.
/*------------------------------------------------------
 * Func : drvif_Dprx_GetAviInfoFrame
 *
 * Desc : Get AVI Info Frame of currenct DPRX port
 *
 * Parm : pAviInfo : AVI info frame output (16 bytes)
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortAviInfoFrame(unsigned char port, DPRX_AVI_INFO_T *pAviInfo);
#define drvif_Dprx_GetAviInfoFrame(pAviInfo)   drvif_Dprx_GetPortAviInfoFrame(CURRENT_DPRX_PORT, pAviInfo) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_GetDrmInfoFrame
 *
 * Desc : Get DRM Info Frame of currenct DPRX port
 *
 * Parm : pDrmInfo: pointer of DPRX_DRM_INFO_T variable from caller
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortDrmInfoFrame(unsigned char port, DPRX_DRM_INFO_T *pDrmInfo);
#define drvif_Dprx_GetDrmInfoFrame(pDrmInfo)   drvif_Dprx_GetPortDrmInfoFrame(CURRENT_DPRX_PORT, pDrmInfo) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortSdpData
 *
 * Desc : get sdp data
 *
 * Parm : p_data : info of dolby vsif data
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortSdpData(unsigned char port, DP_SDP_TYPE type, DP_SDP_DATA_T* p_data);
#define drvif_Dprx_GetSDPData(type, p_data)   drvif_Dprx_GetPortSdpData(CURRENT_DPRX_PORT, type, p_data) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortPPSData
 *
 * Desc : get pps data
 *
 * Parm : ch :
 *        p_data : info of dsc pps
 *
 * Retn : 1 : PPS present
 *        0 : pps absent
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortPPSData(unsigned char ch, DP_PPS_DATA_T* p_pps);
#define drvif_Dprx_GetPPSData(p_pps)   drvif_Dprx_GetPortPPSData(CURRENT_DPRX_PORT, p_pps)

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPixelEncodingFormat
 *
 * Desc : Get pixel encoding format of currenct DPRX port
 *
 * Parm : pPixelEnFormat : pointer of pixel encoding format
 *
 * Retn : 1  : successed
 *        0 : failed
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortPixelEncodingFormat(unsigned char port, DP_PIXEL_ENCODING_FORMAT_T *pPixelEnFormat);
#define drvif_Dprx_GetPixelEncodingFormat(pPixelEnFormat)   drvif_Dprx_GetPortPixelEncodingFormat(CURRENT_DPRX_PORT, pPixelEnFormat) // Use Current Port as input.

typedef enum {
    DPRX_MODE_RAG_DEFAULT    = 0x0,
    DPRX_MODE_RAG_LIMIT  ,
    DPRX_MODE_RAG_FULL  ,
    DPRX_MODE_RAG_UNKNOW
} DPRX_RGB_YUV_RANGE_MODE_T;


//-------------------------------------------------------------------------------
// DRR Related
//-------------------------------------------------------------------------------

/*------------------------------------------------------
 * Func : drvif_Dprx_GetDrrMode
 *
 * Desc : get drr mode
 *
 * Parm : N/A
 *
 * Retn : DP_DRR_MODE_FRR
 *        DP_DRR_MODE_AMD_FREE_SYNC
 *        DP_DRR_MODE_ADAPTIVE_SYNC
 *------------------------------------------------------*/
DP_DRR_MODE_E drvif_Dprx_GetPortDrrMode(unsigned char port);
#define drvif_Dprx_GetDrrMode()   drvif_Dprx_GetPortDrrMode(CURRENT_DPRX_PORT) // Use Current Port as input.


//-------------------------------------------------------------------------------
// AMD Free Sync Related
//-------------------------------------------------------------------------------

/*------------------------------------------------------
 * Func : drvif_Dprx_GetAMDFreeSyncEnable
 *
 * Desc : get AMD free Sync Status
 *
 * Parm : N/A
 *
 * Retn : 0 : AMD feesync disabled, others : AMD free sync enabled
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortAMDFreeSyncEnable(unsigned char port);
#define drvif_Dprx_GetAMDFreeSyncEnable()   drvif_Dprx_GetPortAMDFreeSyncEnable(CURRENT_DPRX_PORT) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_GetAMDFreeSyncStatus
 *
 * Desc : get AMD Free Sync Status
 *
 * Parm : p_info : info of AMD free sync status
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortAMDFreeSyncStatus(unsigned char port, DP_AMD_FREE_SYNC_INFO_T* p_info);
#define drvif_Dprx_GetAMDFreeSyncStatus(p_info)   drvif_Dprx_GetPortAMDFreeSyncStatus(CURRENT_DPRX_PORT, p_info) // Use Current Port as input.

//-------------------------------------------------------------------------------
// DPRX Video Watch Dog Related
//-------------------------------------------------------------------------------

/*------------------------------------------------------
 * Func : drvif_Dprx_SetVideoWatchDagEnable
 *
 * Desc : Disable/Enable Video Watch Dog Enable
 *
 * Parm : enable: Enable/Disable Video WatchDog
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_SetPortVideoWatchDagEnable(unsigned char port, unsigned char enable);
#define drvif_Dprx_SetVideoWatchDagEnable(enable)   drvif_Dprx_SetPortVideoWatchDagEnable(CURRENT_DPRX_PORT, enable) // Use Current Port as input.

//-------------------------------------------------------------------------------
// DPRX Bypass API for HDMI TX
//-------------------------------------------------------------------------------

/*------------------------------------------------------
 * Func : drvif_Dprx_SetPortHdmitxBypassEn
 *
 * Desc : Enable DPRX bypass to HDMI TX enable
 *
 * Parm : ch:
 *        enable : 0 : disable hdmi tx bypass
 *                 1 : enable hdmi tx bypass
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_SetPortHdmitxBypassEn(unsigned char ch, unsigned char enable);
#define drvif_Dprx_SetHdmitxBypassEn(enable)   drvif_Dprx_SetPortHdmitxBypassEn(CURRENT_DPRX_PORT, enable) // Use Current Port as input.

/*------------------------------------------------------
 * Func : drvif_Dprx_GetPortRawTimingInfoForHdmiTX
 *
 * Desc : get DPRX Raw Timing for HDMI TX
 *
 * Parm : ch:
 *        p_timing_info : raw timing
 *
 * Retn : 0 : fail, 1 : success
 *------------------------------------------------------*/
extern DPRX_bool drvif_Dprx_GetPortRawTimingInfoForHdmiTX(unsigned char ch, MEASURE_TIMING_T* p_timing_info);
#define drvif_Dprx_GetRawTimingInfoForHdmiTX(p_timing_info)   drvif_Dprx_GetPortRawTimingInfoForHdmiTX(CURRENT_DPRX_PORT, p_timing_info) // Use Current Port as input.

#endif //_DPRX_FUN_H
