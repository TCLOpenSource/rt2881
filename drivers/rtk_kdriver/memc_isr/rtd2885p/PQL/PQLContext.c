
#include <memc_isr/include/memc_lib.h>
#include "memc_isr/PQL/PQLPlatformDefs.h"
#include "memc_isr/include/PQLAPI.h"
#include "memc_isr/HAL/hal_me.h"
#include "memc_isr/HAL/hal_mc.h"
#include "memc_isr/HAL/hal_post.h"
#include "memc_isr/PQL/PQLContext.h"
#include "memc_isr/PQL/FRC_SceneAnalysis.h"
#include "memc_isr/PQL/MESceneAnalysis.h"
#include "memc_isr/PQL/ME_VstCtrl.h"
#include "memc_isr/PQL/Periodic_Manage.h"
#include "memc_isr/PQL/Patch_Manage.h"
#include <rbus/mdomain_disp_reg.h>
#include <rbus/ppoverlay_reg.h>

#include <tvscalercontrol/scaler/scalerstruct.h>
#include <tvscalercontrol/panel/panelapi.h>
#include "memc_isr/PQL/CalibMEMCInfo.h"
#include "memc_isr/PQL/BasicInfoCheck.h"
#include "memc_reg_def.h"
#include "memc_isr/include/memc_identify_ctrl.h"
#ifdef CONFIG_ARM64 //ARM32 compatible
//#include "io.h"

// for register dump
#include <tvscalercontrol/io/ioregdrv.h>



#undef rtd_outl
#define rtd_outl(x, y)     								IoReg_Write32(x,y)
#undef rtd_inl
#define rtd_inl(x)     									IoReg_Read32(x)
#undef rtd_maskl
#define rtd_maskl(x, y, z)     							IoReg_Mask32(x,y,z)
#undef rtd_setbits
#define rtd_setbits(offset, Mask) rtd_outl(offset, (rtd_inl(offset) | Mask))
#undef rtd_clearbits
#define rtd_clearbits(offset, Mask) rtd_outl(offset, ((rtd_inl(offset) & ~(Mask))))
#else
#include "io.h"
#endif
#define _ABS_(x)   ((x)<=0? (-1*(x)) : (x))
#define _MIN_(x,y) (((x)<=(y))? (x) : (y))
#define _MAX_(x,y) (((x)>=(y))? (x) : (y))
#define _ABS_DIFF_(x, y)	((x)<(y)? ((y)-(x)) : ((x)-(y)))

#define FILE_HEADER_SIZE		64
#define PAGE_HEADER_SIZE		16
#define PAGE_CONTENT_SIZE		256
//#define PAGE_ADDRESS_OFFSET		4
#define PAGE_SIZE_OFFSET		8

#ifdef BUILD_QUICK_SHOW
extern unsigned int Get_DISPLAY_REFRESH_RATE(void);
#endif
extern bool g_KERNEL_AFTER_QS_MEMC_FLG;
extern unsigned char u8HDF_Array[FILE_HEADER_SIZE+ 42*(PAGE_HEADER_SIZE+PAGE_CONTENT_SIZE)];
//extern webos_info_t  webos_tooloption;
extern unsigned char Get_TIMING_PROTECT_PANEL_TYPE(void);
///////////////////////////////////////////////////////////////////////
//Global parameters & contexts
_PQLPARAMETER	*s_pParam = NULL;

_PQLCONTEXT s_Context;

#define	_VIDEO_N	CAD_N
#define	_22_N		CAD_N
#define	_32_N		CAD_N
#define	_32322_N	CAD_N
#define	_334_N		CAD_N
#define	_22224_N	CAD_N
#define	_2224_N	CAD_N
#define	_3223_N	CAD_N
#define	_55_N		CAD_N
#define	_66_N		CAD_N
#define	_44_N		CAD_N
#define	_1112_N	CAD_N
#define	_11112_N	CAD_N
#define	_122_N		CAD_N
#define	_11i23_N	CAD_N
#define	_321_N	CAD_N
#define	_1225_N	CAD_N
#define	_1225_P2_N	CAD_N
#define	_12214_N	CAD_N

#define	_VIDEO_T	CAD_T
#define	_22_T		CAD_T
#define	_32_T		CAD_T
#define	_32322_T	CAD_T
#define	_334_T		CAD_T
#define	_22224_T	CAD_T
#define	_2224_T		CAD_T
#define	_3223_T		CAD_T
#define	_55_T		CAD_T
#define	_66_T		CAD_T
#define	_44_T		CAD_T
#define	_1112_T		CAD_T
#define	_11112_T	CAD_T
#define	_122_T		CAD_T
#define	_11i23_T	CAD_T
#define	_321_T	CAD_T
#define	_1225_T	CAD_T
#define	_1225_P2_T	CAD_T
#define	_12214_T	CAD_T

#define	_VIDEO_F	CAD_F
#define	_22_F		CAD_F
#define	_32_F		CAD_F
#define	_32322_F	CAD_F
#define	_334_F		CAD_F
#define	_22224_F	CAD_F
#define	_2224_F		CAD_F
#define	_3223_F		CAD_F
#define	_55_F		CAD_F
#define	_66_F		CAD_F
#define	_44_F		CAD_F
#define	_1112_F		CAD_F
#define	_11112_F	CAD_F
#define	_122_F		CAD_F
#define	_11i23_F	CAD_F
#define	_321_F	CAD_F
#define	_1225_F	CAD_F
#define	_1225_P2_F	CAD_F
#define	_12214_F	CAD_F



#if (!Pro_tv006)//for m7

const unsigned char g_FRCCADEN_120HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_T, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_T, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_T,_321_N, _1225_T, _1225_P2_T, _12214_T},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_T, _66_T, _44_T, _1112_F, _11112_T, _122_N, _11i23_N,_321_T, _1225_N, _1225_P2_N, _12214_N},
};

const unsigned char g_FRCCADEN_240HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_T, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_T, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_T,_321_N, _1225_T, _1225_P2_T, _12214_T},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_T, _66_T, _44_T, _1112_F, _11112_T, _122_N, _11i23_N,_321_T, _1225_N, _1225_P2_N, _12214_N},
};


#if (!Pro_tv002 & Pro_tv030)
const unsigned char g_FRCCADEN_60HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_F, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_F, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_T,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_T, _66_T, _44_T, _1112_T, _11112_T, _122_N, _11i23_N,_321_T, _1225_N, _1225_P2_N, _12214_N},
};
#else
const unsigned char g_FRCCADEN_60HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_F, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_F, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_T,_321_N, _1225_N, _1225_P2_N, _12214_N},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_T, _66_T, _44_T, _1112_T, _11112_T, _122_N, _11i23_N,_321_T, _1225_N, _1225_P2_N, _12214_N},
};
#endif



#else//for k8


const unsigned char g_FRCCADEN_120HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_F, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_F, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_T,_321_N},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_F, _66_F, _44_F, _1112_F, _11112_T, _122_N, _11i23_N,_321_T},
};

const unsigned char g_FRCCADEN_240HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_F, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_F, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_T,_321_N},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_F, _66_F, _44_F, _1112_F, _11112_T, _122_N, _11i23_N,_321_T},
};


#if (!Pro_tv002 & Pro_tv030)
const unsigned char g_FRCCADEN_60HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_F, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_F, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_F,_321_N},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_T, _66_F, _44_F, _1112_F, _11112_T, _122_N, _11i23_N,_321_T},
};
#else
const unsigned char g_FRCCADEN_60HZOUT_T[_PQL_IN_96HZ][_FRC_CADENCE_NUM_] =
{
	//_PQL_IN_24HZ,
	{_VIDEO_T, _22_F, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_25HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_30HZ,
	{_VIDEO_T, _22_F, _32_F, _32322_N, _334_N, _22224_N, _2224_F, _3223_F, _55_N, _66_N, _44_N, _1112_T, _11112_T, _122_N, _11i23_N,_321_N},
	//_PQL_IN_48HZ,
	{_VIDEO_T, _22_N, _32_N, _32322_N, _334_N, _22224_N, _2224_N, _3223_N, _55_N, _66_N, _44_N, _1112_N, _11112_N, _122_N, _11i23_N,_321_N},
	//_PQL_IN_50HZ,
	{_VIDEO_T, _22_T, _32_N, _32322_N, _334_F, _22224_N, _2224_N, _3223_N, _55_F, _66_F, _44_F, _1112_N, _11112_N, _122_T, _11i23_F,_321_N},
	//_PQL_IN_60HZ,
	{_VIDEO_T, _22_T, _32_T, _32322_T, _334_F, _22224_T, _2224_T, _3223_T, _55_F, _66_F, _44_F, _1112_F, _11112_T, _122_N, _11i23_N,_321_T},
};
#endif


#endif
void DebugCommandCheck(VOID);
//////////////////////////////////////////////////////////////////////////////////////
extern unsigned char cinema_out_frame_rate_change_flag;//k23
extern DEBUG_SCALER_VSC_STRUCT DbgSclrFlgTkr;
extern unsigned int GS_image_hsize;
extern unsigned int GS_image_vsize;
extern unsigned int MEMC_error_status_print_flag;
extern unsigned char MEMC_Lib_get_memc_PowerSaving_Mode(VOID);
#define OLED_DISP_FRAMERATE_LIMIT_120HZ 224400 // 27000000/224400=120.32Hz
#define OLED_DISP_FRAMERATE_LIMIT_60HZ 448600 // 27000000/448600=60.18Hz

VOID LowFrameProc_RTK2885p(void)
{
#if Pro_tv010
	static bool lastFrameLow = false;
	if(s_Context._output_filmDetectctrl.u1_LowFrameCase){
		WriteRegister(KMC_BI_bi_top_01_reg,0,0,0x0);				// local FB
		WriteRegister(MC_MC_28_reg,14,14,0x1);						// local FB
		WriteRegister(MC_MC_28_reg,15,22,0x0);						// local FB
		WriteRegister(KME_DEHALO_KME_DEHALO_10_reg,0,0,0x0);		// dehalo
		//WriteRegister(HARDWARE_HARDWARE_00_reg,0,0,0);				// rim en
		WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_54_reg,18,18,0);	// logo en
		WriteRegister(KME_TOP_KME_TOP_04_reg,29,29,0);				// meander

		//WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_20_reg, 0, 17,0x3edf6);//rand candidate's offset
		//WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_2C_reg, 0, 17,0x3edf6);//rand candidate's offset
		WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_20_reg, 0, 17,0x32c36);
		WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_2C_reg, 0, 17,0x32c36);
		lastFrameLow = true;
	}
	else if(lastFrameLow){
		WriteRegister(KMC_BI_bi_top_01_reg,0,0,0x1);				// local FB
		WriteRegister(MC_MC_28_reg,14,14,0x0);						// local FB
		WriteRegister(MC_MC_28_reg,15,22,0x0);						// local FB
		WriteRegister(KME_DEHALO_KME_DEHALO_10_reg,0,0,0x1);		// dehalo
		//WriteRegister(HARDWARE_HARDWARE_00_reg,0,0,1);				// rim en
		WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_54_reg,18,18,1);	// logo en
		WriteRegister(KME_TOP_KME_TOP_04_reg,29,29,1);				// meander

		WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_20_reg, 0, 17,0x39e3f);//rand candidate's offset
		WriteRegister(KME_VBUF_TOP_KME_VBUF_TOP_2C_reg, 0, 17,0x39e3f);//rand candidate's offset
		lastFrameLow = false;
	}
#endif
}

VOID LowFrameProc_RTK2885pp(void)
{
	//CONFIG_NEED_TUNNING
}

VOID LowFrameProc(void)
{
	if(RUN_MERLIN8P()){
		LowFrameProc_RTK2885pp();
	}else if(RUN_MERLIN8()){
		LowFrameProc_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8
}


unsigned int dma_cnt=0;

unsigned char DMA_Check_RTK2885pp(int cnt_2, int repeat_mode_chg_cnt, int repeat_mode_pre)
{
	unsigned int bBGEn = 0, MEMC_BGEn = 0, MEMC_MuxEn = 0;
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl_reg;
	M8P_rthw_lbmc_lbmc_err_RBUS M8P_rthw_lbmc_lbmc_err_reg;
	M8P_rthw_lbmc_lbmc_err1_RBUS M8P_rthw_lbmc_lbmc_err1;
	M8P_rthw_lbmc_lbmc_err2_RBUS M8P_rthw_lbmc_lbmc_err2;
	M8P_rthw_lbmc_lbmc_err3_RBUS M8P_rthw_lbmc_lbmc_err3;
	M8P_rthw_me_dma_rtme_dma_status_RBUS M8P_rthw_me_dma_rtme_dma_status_reg;
	M8P_rthw_me_dma_dm0_wr_status_RBUS M8P_rthw_me_dma_dm0_wr_status;
	M8P_rthw_me_dma_dm1_wr_status_RBUS M8P_rthw_me_dma_dm1_wr_status;
	M8P_rthw_me_dma_dm2_wr_status_RBUS M8P_rthw_me_dma_dm2_wr_status;
	M8P_rthw_me_dma_dm3_rd_status_RBUS M8P_rthw_me_dma_dm3_rd_status;
	M8P_rthw_me_dma_dm4_rd_status_RBUS M8P_rthw_me_dma_dm4_rd_status;
	M8P_rthw_me_dma_dm5_rd_status_RBUS M8P_rthw_me_dma_dm5_rd_status;
	M8P_rthw_me_dma_dm6_rd_status_RBUS M8P_rthw_me_dma_dm6_rd_status;
	M8P_rthw_me_dma_dm7_rd_status_RBUS M8P_rthw_me_dma_dm7_rd_status;
	M8P_rthw_me_dma_dm0_wr_status1_RBUS M8P_rthw_me_dma_dm0_wr_status1;
	M8P_rthw_me_dma_dm1_wr_status1_RBUS M8P_rthw_me_dma_dm1_wr_status1;
	M8P_rthw_me_dma_dm2_wr_status1_RBUS M8P_rthw_me_dma_dm2_wr_status1;
	M8P_rthw_me_dma_dm3_rd_status1_RBUS M8P_rthw_me_dma_dm3_rd_status1;
	M8P_rthw_me_dma_dm4_rd_status1_RBUS M8P_rthw_me_dma_dm4_rd_status1;
	M8P_rthw_me_dma_dm5_rd_status1_RBUS M8P_rthw_me_dma_dm5_rd_status1;
	M8P_rthw_me_dma_dm6_rd_status1_RBUS M8P_rthw_me_dma_dm6_rd_status1;
	M8P_rthw_me_dma_dm7_rd_status1_RBUS M8P_rthw_me_dma_dm7_rd_status1;
	M8P_rthw_mc_dma_mc_dma_index_RBUS M8P_rthw_mc_dma_mc_dma_index_reg;

	M8P_rthw_mc_dma_mc_lf_dma_wr_status_RBUS M8P_rthw_mc_dma_mc_lf_dma_wr_status;
	M8P_rthw_mc_dma_mc_hf_dma_wr_status_RBUS M8P_rthw_mc_dma_mc_hf_dma_wr_status;
	M8P_rthw_mc_dma_mc_lf_i_dma_rd_status_RBUS M8P_rthw_mc_dma_mc_lf_i_dma_rd_status;
	M8P_rthw_mc_dma_mc_hf_i_dma_rd_status_RBUS M8P_rthw_mc_dma_mc_hf_i_dma_rd_status;
	M8P_rthw_mc_dma_mc_lf_p_dma_rd_status_RBUS M8P_rthw_mc_dma_mc_lf_p_dma_rd_status;
	M8P_rthw_mc_dma_mc_hf_p_dma_rd_status_RBUS M8P_rthw_mc_dma_mc_hf_p_dma_rd_status;
	M8P_rthw_mc_dma_mc_dma_crc_cmp_RBUS M8P_rthw_mc_dma_mc_dma_crc_cmp;

	M8P_rthw_me_dma_rtme_dm0_ovf_RBUS M8P_rthw_me_dma_rtme_dm0_ovf;
	M8P_rthw_me_dma_rtme_dm1_ovf_RBUS M8P_rthw_me_dma_rtme_dm1_ovf;
	M8P_rthw_me_dma_rtme_dm2_ovf_RBUS M8P_rthw_me_dma_rtme_dm2_ovf;
	M8P_rthw_me_dma_rtme_dm3_unf_RBUS M8P_rthw_me_dma_rtme_dm3_unf;
	M8P_rthw_me_dma_rtme_dm4_unf_RBUS M8P_rthw_me_dma_rtme_dm4_unf;
	M8P_rthw_me_dma_rtme_dm5_unf_RBUS M8P_rthw_me_dma_rtme_dm5_unf;
	M8P_rthw_me_dma_rtme_dm6_unf_RBUS M8P_rthw_me_dma_rtme_dm6_unf;
	M8P_rthw_me_dma_rtme_dm7_unf_RBUS M8P_rthw_me_dma_rtme_dm7_unf;
	M8P_rthw_me_dma_rtme_dm0_os_RBUS M8P_rthw_me_dma_rtme_dm0_os;
	M8P_rthw_me_dma_rtme_dm1_os_RBUS M8P_rthw_me_dma_rtme_dm1_os;
	M8P_rthw_me_dma_rtme_dm2_os_RBUS M8P_rthw_me_dma_rtme_dm2_os;
	M8P_rthw_me_dma_rtme_dm3_os_RBUS M8P_rthw_me_dma_rtme_dm3_os;
	M8P_rthw_me_dma_rtme_dm4_os_RBUS M8P_rthw_me_dma_rtme_dm4_os;
	M8P_rthw_me_dma_rtme_dm5_os_RBUS M8P_rthw_me_dma_rtme_dm5_os;
	M8P_rthw_me_dma_rtme_dm6_os_RBUS M8P_rthw_me_dma_rtme_dm6_os;
	M8P_rthw_me_dma_rtme_dm7_os_RBUS M8P_rthw_me_dma_rtme_dm7_os;
	M8P_rthw_me_dma_rtme_dm0_us_RBUS M8P_rthw_me_dma_rtme_dm0_us;
	M8P_rthw_me_dma_rtme_dm1_us_RBUS M8P_rthw_me_dma_rtme_dm1_us;
	M8P_rthw_me_dma_rtme_dm2_us_RBUS M8P_rthw_me_dma_rtme_dm2_us;
	M8P_rthw_me_dma_rtme_dm3_us_RBUS M8P_rthw_me_dma_rtme_dm3_us;
	M8P_rthw_me_dma_rtme_dm4_us_RBUS M8P_rthw_me_dma_rtme_dm4_us;
	M8P_rthw_me_dma_rtme_dm5_us_RBUS M8P_rthw_me_dma_rtme_dm5_us;
	M8P_rthw_me_dma_rtme_dm6_us_RBUS M8P_rthw_me_dma_rtme_dm6_us;
	M8P_rthw_me_dma_rtme_dm7_us_RBUS M8P_rthw_me_dma_rtme_dm7_us;
	M8P_rthw_me_bist_bisr_control_kme_top_bist_mode_0_RBUS M8P_rthw_me_bist_bisr_control_kme_top_bist_mode_0;

	unsigned char cnt_1 = 0, repeat_mode = 0;

	bBGEn = rtd_inl(PPOVERLAY_Main_Display_Control_RSV_reg) & _BIT1;
	MEMC_BGEn = HAL_POST_GetMuteScreenEnable();
	MEMC_MuxEn = PPOVERLAY_MEMC_MUX_CTRL_get_memc_outmux_sel(rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg));
	sys_reg_sys_clken3_reg.regValue = rtd_inl(SYS_REG_SYS_CLKEN3_reg);
	repeat_mode = HAL_MC_GetRepeatMode();

	M8P_rtmc_lbmc_lbmc_ctrl_reg.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
	M8P_rthw_lbmc_lbmc_err_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_ERR_reg);
	M8P_rthw_lbmc_lbmc_err1.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_ERR1_reg);
	M8P_rthw_lbmc_lbmc_err2.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_ERR2_reg);
	M8P_rthw_lbmc_lbmc_err3.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_ERR3_reg);
	M8P_rthw_mc_dma_mc_dma_index_reg.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_DMA_index_reg);
	M8P_rthw_me_dma_dm0_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_status_reg);
	M8P_rthw_me_dma_dm1_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_status_reg);
	M8P_rthw_me_dma_dm2_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_status_reg);
	M8P_rthw_me_dma_dm3_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_status_reg);
	M8P_rthw_me_dma_dm4_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_status_reg);
	M8P_rthw_me_dma_dm5_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_status_reg);
	M8P_rthw_me_dma_dm6_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_status_reg);
	M8P_rthw_me_dma_dm7_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_status_reg);
	M8P_rthw_me_dma_dm0_wr_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_status1_reg);
	M8P_rthw_me_dma_dm1_wr_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_status1_reg);
	M8P_rthw_me_dma_dm2_wr_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_status1_reg);
	M8P_rthw_me_dma_dm3_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_status1_reg);
	M8P_rthw_me_dma_dm4_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_status1_reg);
	M8P_rthw_me_dma_dm5_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_status1_reg);
	M8P_rthw_me_dma_dm6_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_status1_reg);
	M8P_rthw_me_dma_dm7_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_status1_reg);
	M8P_rthw_me_dma_rtme_dma_status_reg.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DMA_STATUS_reg);

	M8P_rthw_mc_dma_mc_lf_dma_wr_status.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_LF_DMA_WR_status_reg);
	M8P_rthw_mc_dma_mc_hf_dma_wr_status.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_HF_DMA_WR_status_reg);
	M8P_rthw_mc_dma_mc_lf_i_dma_rd_status.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_status_reg);
	M8P_rthw_mc_dma_mc_hf_i_dma_rd_status.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_HF_I_DMA_RD_status_reg);
	M8P_rthw_mc_dma_mc_lf_p_dma_rd_status.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_LF_P_DMA_RD_status_reg);
	M8P_rthw_mc_dma_mc_hf_p_dma_rd_status.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_HF_P_DMA_RD_status_reg);
	M8P_rthw_mc_dma_mc_dma_crc_cmp.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_DMA_CRC_CMP_reg);

	M8P_rthw_me_dma_rtme_dm0_ovf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM0_OVF_reg);
	M8P_rthw_me_dma_rtme_dm1_ovf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM1_OVF_reg);
	M8P_rthw_me_dma_rtme_dm2_ovf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM2_OVF_reg);
	M8P_rthw_me_dma_rtme_dm3_unf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM3_UNF_reg);
	M8P_rthw_me_dma_rtme_dm4_unf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM4_UNF_reg);
	M8P_rthw_me_dma_rtme_dm5_unf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM5_UNF_reg);
	M8P_rthw_me_dma_rtme_dm6_unf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM6_UNF_reg);
	M8P_rthw_me_dma_rtme_dm7_unf.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM7_UNF_reg);
	M8P_rthw_me_dma_rtme_dm0_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM0_OS_reg);
	M8P_rthw_me_dma_rtme_dm1_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM1_OS_reg);
	M8P_rthw_me_dma_rtme_dm2_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM2_OS_reg);
	M8P_rthw_me_dma_rtme_dm3_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM3_OS_reg);
	M8P_rthw_me_dma_rtme_dm4_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM4_OS_reg);
	M8P_rthw_me_dma_rtme_dm5_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM5_OS_reg);
	M8P_rthw_me_dma_rtme_dm6_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM6_OS_reg);
	M8P_rthw_me_dma_rtme_dm7_os.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM7_OS_reg);
	M8P_rthw_me_dma_rtme_dm0_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM0_US_reg);
	M8P_rthw_me_dma_rtme_dm1_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM1_US_reg);
	M8P_rthw_me_dma_rtme_dm2_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM2_US_reg);
	M8P_rthw_me_dma_rtme_dm3_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM3_US_reg);
	M8P_rthw_me_dma_rtme_dm4_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM4_US_reg);
	M8P_rthw_me_dma_rtme_dm5_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM5_US_reg);
	M8P_rthw_me_dma_rtme_dm6_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM6_US_reg);
	M8P_rthw_me_dma_rtme_dm7_us.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DM7_US_reg);

	M8P_rthw_me_bist_bisr_control_kme_top_bist_mode_0.regValue = rtd_inl(M8P_RTHW_ME_BIST_BISR_CONTROL_KME_TOP_BIST_MODE_0_reg);

	if(repeat_mode == 0){
	
		//rdma_datmem_overflow/wdma_datmem_underflow
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_rtme_dma_status_reg.regValue)>>0)&0xffffffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] me_share_dm_error_flag (b809fdd0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dma_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DMA_STATUS_reg, M8P_rthw_me_dma_rtme_dma_status_reg.regValue);
			cnt_1++;
		}

		//me ack num error
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm0_wr_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809f270 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm0_wr_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM0_WR_status1_reg, M8P_rthw_me_dma_dm0_wr_status1.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm1_wr_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809f370 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm1_wr_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM1_WR_status1_reg, M8P_rthw_me_dma_dm1_wr_status1.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm2_wr_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809f4b0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm2_wr_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM2_WR_status1_reg, M8P_rthw_me_dma_dm2_wr_status1.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm3_rd_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fa60 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm3_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM3_RD_status1_reg, M8P_rthw_me_dma_dm3_rd_status1.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm4_rd_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fb50 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm4_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM4_RD_status1_reg, M8P_rthw_me_dma_dm4_rd_status1.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm5_rd_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fc90 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm5_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM5_RD_status1_reg, M8P_rthw_me_dma_dm5_rd_status1.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm6_rd_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fdc0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm6_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM6_RD_status1_reg, M8P_rthw_me_dma_dm6_rd_status1.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm7_rd_status1.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fa10 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm7_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM7_RD_status1_reg, M8P_rthw_me_dma_dm7_rd_status1.regValue);
			cnt_1++;
		}

		//me zero length
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm0_wr_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809f26c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm0_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM0_WR_status_reg, M8P_rthw_me_dma_dm0_wr_status.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm1_wr_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809f36c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm1_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM1_WR_status_reg, M8P_rthw_me_dma_dm1_wr_status.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm2_wr_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809f4ac = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm2_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM2_WR_status_reg, M8P_rthw_me_dma_dm2_wr_status.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm3_rd_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fa5c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm3_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM3_RD_status_reg, M8P_rthw_me_dma_dm3_rd_status.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm4_rd_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fb4c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm4_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM4_RD_status_reg, M8P_rthw_me_dma_dm4_rd_status.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm5_rd_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fc8c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm5_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM5_RD_status_reg, M8P_rthw_me_dma_dm5_rd_status.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm6_rd_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fdbc = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm6_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM6_RD_status_reg, M8P_rthw_me_dma_dm6_rd_status.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm7_rd_status.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fa0c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm7_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_DM7_RD_status_reg, M8P_rthw_me_dma_dm7_rd_status.regValue);
			cnt_1++;
		}

		//me1/me2 underflow
	#if 0//RBUS_NOT_READY
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_top2_kme_buff_st.regValue)>>0)&0xffffffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] lbme_underflorw_status (b809bf48 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_top2_kme_buff_st.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_TOP2_KME_BUFF_ST_reg, M8P_rthw_me_top2_kme_buff_st.regValue);
			cnt_1++;
		}
	#endif

		//dma_index_conflict
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_mc_dma_mc_dma_index_reg.regValue)>>12)&0x3) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] mc_rw_conflict_status (b80997b8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_dma_index_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_DMA_index_reg, M8P_rthw_mc_dma_mc_dma_index_reg.regValue);
			cnt_1++;
		}

	//MC
		if(((M8P_rthw_mc_dma_mc_lf_dma_wr_status.regValue)&0x7f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_mc_dma_mc_lf_dma_wr_status (b8099664 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_lf_dma_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_LF_DMA_WR_status_reg, M8P_rthw_mc_dma_mc_lf_dma_wr_status.regValue);
			cnt_1++;
		}
		if(((M8P_rthw_mc_dma_mc_hf_dma_wr_status.regValue)&0x7f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_mc_dma_mc_hf_dma_wr_status (b80996e0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_hf_dma_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_HF_DMA_WR_status_reg, M8P_rthw_mc_dma_mc_hf_dma_wr_status.regValue);
			cnt_1++;
		}
		if(((M8P_rthw_mc_dma_mc_lf_i_dma_rd_status.regValue)&0x7f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_mc_dma_mc_lf_i_dma_rd_status (b8099720 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_lf_i_dma_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_LF_I_DMA_RD_status_reg, M8P_rthw_mc_dma_mc_lf_i_dma_rd_status.regValue);
			cnt_1++;
		}
		if(((M8P_rthw_mc_dma_mc_hf_i_dma_rd_status.regValue)&0x7f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_mc_dma_mc_hf_i_dma_rd_status (b8099740 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_hf_i_dma_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_HF_I_DMA_RD_status_reg, M8P_rthw_mc_dma_mc_hf_i_dma_rd_status.regValue);
			cnt_1++;
		}
		if(((M8P_rthw_mc_dma_mc_lf_p_dma_rd_status.regValue)&0x7f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_mc_dma_mc_lf_p_dma_rd_status (b8099760 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_lf_p_dma_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_LF_P_DMA_RD_status_reg, M8P_rthw_mc_dma_mc_lf_p_dma_rd_status.regValue);
			cnt_1++;
		}
		if(((M8P_rthw_mc_dma_mc_hf_p_dma_rd_status.regValue)&0x7f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_mc_dma_mc_hf_p_dma_rd_status (b8099780 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_hf_p_dma_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_HF_P_DMA_RD_status_reg, M8P_rthw_mc_dma_mc_hf_p_dma_rd_status.regValue);
			cnt_1++;
		}
		if((((M8P_rthw_mc_dma_mc_dma_crc_cmp.regValue)>>8)&0xffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_mc_dma_mc_dma_crc_cmp (b80997f0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_dma_crc_cmp.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_MC_DMA_MC_DMA_CRC_CMP_reg, M8P_rthw_mc_dma_mc_dma_crc_cmp.regValue);
			cnt_1++;
		}

	//LBMC
		//underflow/ not enough
		if((((M8P_rthw_lbmc_lbmc_err_reg.regValue)>>0)&0xfffffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] mc_read_dma_error_flag (b8099f84 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_LBMC_LBMC_ERR_reg, M8P_rthw_lbmc_lbmc_err_reg.regValue);
			cnt_1++;
		}
		if(M8P_rthw_lbmc_lbmc_err1.not_enough_cur != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_lbmc_lbmc_err1.not_enough_cur = %d !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err1.not_enough_cur, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_LBMC_LBMC_ERR1_reg, M8P_rthw_lbmc_lbmc_err1.regValue);
			cnt_1++;
		}
		if(M8P_rthw_lbmc_lbmc_err1.not_enough_write != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_lbmc_lbmc_err1.not_enough_write = %d !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err1.not_enough_write, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_LBMC_LBMC_ERR1_reg, M8P_rthw_lbmc_lbmc_err1.regValue);
			cnt_1++;
		}
		if(M8P_rthw_lbmc_lbmc_err2.not_enough_target != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_lbmc_lbmc_err2.not_enough_target = %d !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err2.not_enough_target, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_LBMC_LBMC_ERR2_reg, M8P_rthw_lbmc_lbmc_err2.regValue);
			cnt_1++;
		}
		if(M8P_rthw_lbmc_lbmc_err2.underflow_cur != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_lbmc_lbmc_err2.underflow_cur = %d !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err2.underflow_cur, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_LBMC_LBMC_ERR2_reg, M8P_rthw_lbmc_lbmc_err2.regValue);
			cnt_1++;
		}
		if(M8P_rthw_lbmc_lbmc_err3.underflow_write != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_lbmc_lbmc_err3.underflow_write = %d !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err3.underflow_write, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_LBMC_LBMC_ERR3_reg, M8P_rthw_lbmc_lbmc_err3.regValue);
			cnt_1++;
		}
		if(M8P_rthw_lbmc_lbmc_err3.underflow_target != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_lbmc_lbmc_err3.underflow_target = %d !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err3.underflow_target, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_LBMC_LBMC_ERR3_reg, M8P_rthw_lbmc_lbmc_err3.regValue);
			cnt_1++;
		}

	//ME underflow, overflow
		if(M8P_rthw_me_dma_rtme_dm0_ovf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm0_ovf (b809fe30 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm0_ovf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM0_OVF_reg, M8P_rthw_me_dma_rtme_dm0_ovf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm1_ovf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm1_ovf (b809fe34 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm1_ovf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM1_OVF_reg, M8P_rthw_me_dma_rtme_dm1_ovf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm2_ovf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm2_ovf (b809fe38 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm2_ovf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM2_OVF_reg, M8P_rthw_me_dma_rtme_dm2_ovf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm3_unf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm3_unf (b809fe3c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm3_unf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM3_UNF_reg, M8P_rthw_me_dma_rtme_dm3_unf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm4_unf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm4_unf (b809fe40 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm4_unf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM4_UNF_reg, M8P_rthw_me_dma_rtme_dm4_unf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm5_unf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm5_unf (b809fe44 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm5_unf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM5_UNF_reg, M8P_rthw_me_dma_rtme_dm5_unf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm6_unf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm6_unf (b809fe48 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm6_unf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM6_UNF_reg, M8P_rthw_me_dma_rtme_dm6_unf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm7_unf.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm7_unf (b809fe4c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm7_unf.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM7_UNF_reg, M8P_rthw_me_dma_rtme_dm7_unf.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm0_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm0_os (b809fe50 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm0_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM0_OS_reg, M8P_rthw_me_dma_rtme_dm0_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm1_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm1_os (b809fe54 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm1_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM1_OS_reg, M8P_rthw_me_dma_rtme_dm1_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm2_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm2_os (b809fe58 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm2_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM2_OS_reg, M8P_rthw_me_dma_rtme_dm2_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm3_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm3_os (b809fe5c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm3_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM3_OS_reg, M8P_rthw_me_dma_rtme_dm3_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm4_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm4_os (b809fe60 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm4_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM4_OS_reg, M8P_rthw_me_dma_rtme_dm4_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm5_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm5_os (b809fe64 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm5_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM5_OS_reg, M8P_rthw_me_dma_rtme_dm5_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm6_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm6_os (b809fe68 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm6_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM6_OS_reg, M8P_rthw_me_dma_rtme_dm6_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm7_os.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm7_os (b809fe6c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm7_os.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM7_OS_reg, M8P_rthw_me_dma_rtme_dm7_os.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm0_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm0_us (b809fe70 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm0_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM0_US_reg, M8P_rthw_me_dma_rtme_dm0_us.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm1_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm1_us (b809fe74 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm1_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM1_US_reg, M8P_rthw_me_dma_rtme_dm1_us.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm2_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm2_us (b809fe78 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm2_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM2_US_reg, M8P_rthw_me_dma_rtme_dm2_us.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm3_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm3_us (b809fe7c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm3_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM3_US_reg, M8P_rthw_me_dma_rtme_dm3_us.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm4_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm4_us (b809fe80 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm4_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM4_US_reg, M8P_rthw_me_dma_rtme_dm4_us.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm5_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm5_us (b809fe84 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm5_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM5_US_reg, M8P_rthw_me_dma_rtme_dm5_us.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm6_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm6_us (b809fe88 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm6_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM6_US_reg, M8P_rthw_me_dma_rtme_dm6_us.regValue);
			cnt_1++;
		}
		if(M8P_rthw_me_dma_rtme_dm7_us.regValue != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_dma_rtme_dm7_us (b809fe8c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dm7_us.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_DMA_RTME_DM7_US_reg, M8P_rthw_me_dma_rtme_dm7_us.regValue);
			cnt_1++;
		}

	//den
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_bist_bisr_control_kme_top_bist_mode_0.regValue)>>0)&0xffffffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice("[DMA Check] M8P_rthw_me_bist_bisr_control_kme_top_bist_mode_0 (b809e000 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_bist_bisr_control_kme_top_bist_mode_0.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(M8P_RTHW_ME_BIST_BISR_CONTROL_KME_TOP_BIST_MODE_0_reg, M8P_rthw_me_bist_bisr_control_kme_top_bist_mode_0.regValue);
			cnt_1++;
		}

	}
	else{
		if(MEMC_LibGetMEMCLowDelayModeEnable() == FALSE && MEMC_LibGetMEMC_PCModeEnable() == FALSE){
			if(M8P_rtmc_lbmc_lbmc_ctrl_reg.lbmcpc_mode_sel_p){ // PZ
				if(M8P_rthw_lbmc_lbmc_err_reg.hf_p_not_enough || M8P_rthw_lbmc_lbmc_err_reg.hf_p_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfh_p_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfh_p_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfl_p_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfl_p_underflow){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice("[DMA Check][PZ] mc_read_dma_error_flag (b8099f84 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_LBMC_LBMC_ERR_reg, M8P_rthw_lbmc_lbmc_err_reg.regValue);
					cnt_1++;
				}
			}else{ // IZ
				if(M8P_rthw_lbmc_lbmc_err_reg.hf_i_not_enough || M8P_rthw_lbmc_lbmc_err_reg.hf_i_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfh_i_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfh_i_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfl_i_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfl_i_underflow){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice("[DMA Check][IZ] mc_read_dma_error_flag (b8099f84 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_LBMC_LBMC_ERR_reg, M8P_rthw_lbmc_lbmc_err_reg.regValue);
					cnt_1++;
				}
			}
		}
	}

	return cnt_1;
}

unsigned char DMA_Check_RTK2885p(int cnt_2, int repeat_mode_chg_cnt, int repeat_mode_pre)
{
	unsigned int bBGEn = 0, MEMC_BGEn = 0, MEMC_MuxEn = 0;
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	lbmc_lbmc_24_RBUS lbmc_lbmc_24_reg;
	kme_dm_top0_kme_dm_error_flag_RBUS kme_dm_top0_kme_dm_error_flag_reg;
	kme_dm_top2_nousemv_12_end_address7_RBUS kme_dm_top2_nousemv_12_end_address7_reg;
	kme_dm_top2_mv_dm_error_flag_RBUS kme_dm_top2_mv_dm_error_flag_reg;
	me_share_dma_kme_dma_status_RBUS me_share_dma_kme_dma_status_reg;
	kme_dm_top2_kme_me_shr_st_RBUS kme_dm_top2_kme_me_shr_st_reg;
	mvinfo_dma_kmvinfo_status_RBUS mvinfo_dma_kmvinfo_status_reg;
	//dbus_wrapper_irq_status_RBUS dbus_wrapper_irq_status_reg; // fix me : rbus time out
	me_share_dma_me_dma_rd_status_RBUS me_share_dma_me_dma_rd_status_reg;
	me_share_dma_me_dma_rd_status1_RBUS me_share_dma_me_dma_rd_status1_reg;
	mvinfo_dma_mvinfo_wr_status_RBUS mvinfo_dma_mvinfo_wr_status_reg;
	mvinfo_dma_mvinfo_wr_status1_RBUS mvinfo_dma_mvinfo_wr_status1_reg;
	mvinfo_dma_mvinfo_rd_status_RBUS mvinfo_dma_mvinfo_rd_status_reg;
	mvinfo_dma_kmvinfo_dm_RBUS mvinfo_dma_kmvinfo_dm_reg;
	kme_bist_kme_buff_st_RBUS kme_bist_kme_buff_st_reg;
	mc_dma_mc_dma_index_RBUS mc_dma_mc_dma_index_reg;
	lbmc_lbmc_e8_RBUS lbmc_lbmc_e8_reg;
	unsigned char cnt_1 = 0;

	bBGEn = rtd_inl(PPOVERLAY_Main_Display_Control_RSV_reg) & _BIT1;
	ReadRegister(KPOST_TOP_KPOST_TOP_60_reg, 30, 30, &MEMC_BGEn);
	MEMC_MuxEn = PPOVERLAY_MEMC_MUX_CTRL_get_memc_outmux_sel(rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg));

	//for oled 120Hz panel, display vtotal < 2244(120.32Hz) will happen black video
	//if((webos_tooloption.eBackLight == 2) && (Get_DISPLAY_REFRESH_RATE() == 120) && (u32_oFrameRate < OLED_DISP_FRAMERATE_LIMIT_120HZ))
	//if((webos_tooloption.eBackLight == 2) && (Get_DISPLAY_REFRESH_RATE() >= 120) && (u32_oFrameRate < OLED_DISP_FRAMERATE_LIMIT_120HZ))//k23
		//rtd_pr_memc_notice( "[FR] out FrameRate Over Range!!!(%dHz(%d) -> %dHz(%d))(90K = %d)\n\r", u32_oFrameRateHz_pre, u32_oFrameRate_Pre, u32_oFrameRateHz, u32_oFrameRate, rtd_inl(0xB801B6B8));
	//for oled 60Hz panel, display vtotal < 2243(60.18Hz) will happen black video
	//if((webos_tooloption.eBackLight == 2) && (Get_DISPLAY_REFRESH_RATE() == 60) && (u32_oFrameRate < OLED_DISP_FRAMERATE_LIMIT_60HZ))
		//rtd_pr_memc_notice( "[FR] out FrameRate Over Range!!!(%dHz(%d) -> %dHz(%d))(90K = %d)\n\r", u32_oFrameRateHz_pre, u32_oFrameRate_Pre, u32_oFrameRateHz, u32_oFrameRate, rtd_inl(0xB801B6B8));

	//input size monitor
	sys_reg_sys_clken3_reg.regValue = rtd_inl(SYS_REG_SYS_CLKEN3_reg);
	lbmc_lbmc_24_reg.regValue = rtd_inl(LBMC_LBMC_24_reg);
	kme_dm_top0_kme_dm_error_flag_reg.regValue = rtd_inl(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg);
	kme_dm_top2_nousemv_12_end_address7_reg.regValue = rtd_inl(KME_DM_TOP2_NOUSEMV_12_END_ADDRESS7_reg);
	kme_dm_top2_mv_dm_error_flag_reg.regValue = rtd_inl(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg);
	me_share_dma_kme_dma_status_reg.regValue = rtd_inl(ME_SHARE_DMA_KME_DMA_STATUS_reg);
	kme_dm_top2_kme_me_shr_st_reg.regValue = rtd_inl(KME_DM_TOP2_KME_ME_SHR_ST_reg);
	mvinfo_dma_kmvinfo_status_reg.regValue = rtd_inl(MVINFO_DMA_KMVINFO_STATUS_reg);
	me_share_dma_me_dma_rd_status_reg.regValue = rtd_inl(ME_SHARE_DMA_ME_DMA_RD_status_reg);
	me_share_dma_me_dma_rd_status1_reg.regValue = rtd_inl(ME_SHARE_DMA_ME_DMA_RD_status1_reg);
	mvinfo_dma_mvinfo_wr_status_reg.regValue = rtd_inl(MVINFO_DMA_MVINFO_WR_status_reg);
	mvinfo_dma_mvinfo_wr_status1_reg.regValue = rtd_inl(MVINFO_DMA_MVINFO_WR_status1_reg);
	mvinfo_dma_mvinfo_rd_status_reg.regValue = rtd_inl(MVINFO_DMA_MVINFO_RD_status_reg);
	mvinfo_dma_kmvinfo_dm_reg.regValue = rtd_inl(MVINFO_DMA_KMVINFO_DM_reg);
	kme_bist_kme_buff_st_reg.regValue = rtd_inl(KME_BIST_KME_BUFF_ST_reg);
	mc_dma_mc_dma_index_reg.regValue = rtd_inl(MC_DMA_MC_DMA_index_reg);
	lbmc_lbmc_e8_reg.regValue = rtd_inl(LBMC_LBMC_E8_reg);

	#if 1
	if(lbmc_lbmc_24_reg.lbmcpc_mode == 0){
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top0_kme_dm_error_flag_reg.regValue)>>9)&0x7fffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] kme_dm_error_flag (b809c0f8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top0_kme_dm_error_flag_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg, kme_dm_top0_kme_dm_error_flag_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top2_nousemv_12_end_address7_reg.regValue)>>7)&0x1ff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] kmv_dm_error_flag (b809c2f4 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top2_nousemv_12_end_address7_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(KME_DM_TOP2_NOUSEMV_12_END_ADDRESS7_reg, kme_dm_top2_nousemv_12_end_address7_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top2_mv_dm_error_flag_reg.regValue)>>16)&0x7f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] kmv_dm_error_flag (b809c2f8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top2_mv_dm_error_flag_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg, kme_dm_top2_mv_dm_error_flag_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((me_share_dma_kme_dma_status_reg.regValue)>>0)&0xffffffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] me_share_dm_error_flag (b809b7c0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", me_share_dma_kme_dma_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(ME_SHARE_DMA_KME_DMA_STATUS_reg, me_share_dma_kme_dma_status_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top2_kme_me_shr_st_reg.regValue)>>0)&0x7) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] me_share_dm_error_flag (b809c290 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top2_kme_me_shr_st_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(KME_DM_TOP2_KME_ME_SHR_ST_reg, kme_dm_top2_kme_me_shr_st_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_kmvinfo_status_reg.regValue)>>0)&0x7) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mv_info_error_flag (b809b1a0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_kmvinfo_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(MVINFO_DMA_KMVINFO_STATUS_reg, mvinfo_dma_kmvinfo_status_reg.regValue);
			cnt_1++;
		}
		if(MEMC_Lib_get_memc_PowerSaving_Mode()== 0){
		#if 0 // fix me : rbus time out
			if(sys_reg_sys_clken3_reg.clken_memc_me && (((dbus_wrapper_irq_status_reg.regValue)>>0)&0x3fff) != 0){
				if(cnt_2 < 20){
					rtd_pr_memc_notice( "[DMA Check] mv_info_error_flag (b809e0c8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", dbus_wrapper_irq_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
				}
				rtd_outl(DBUS_WRAPPER_irq_status_reg, dbus_wrapper_irq_status_reg.regValue);
				cnt_1++;
			}
		#endif
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((me_share_dma_me_dma_rd_status_reg.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] me_dma_rd_status (b809b7e4 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", me_share_dma_me_dma_rd_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(ME_SHARE_DMA_ME_DMA_RD_status_reg, me_share_dma_me_dma_rd_status_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((me_share_dma_me_dma_rd_status1_reg.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] me_dma_rd_status1 (b809b7e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", me_share_dma_me_dma_rd_status1_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(ME_SHARE_DMA_ME_DMA_RD_status1_reg, me_share_dma_me_dma_rd_status1_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_mvinfo_wr_status_reg.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status (b809b12c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_mvinfo_wr_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(MVINFO_DMA_MVINFO_WR_status_reg, mvinfo_dma_mvinfo_wr_status_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_mvinfo_wr_status1_reg.regValue)>>31)&0x1) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809b130 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_mvinfo_wr_status1_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(MVINFO_DMA_MVINFO_WR_status1_reg, mvinfo_dma_mvinfo_wr_status1_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_mvinfo_rd_status_reg.regValue)>>0)&0x1f) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809b178 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_mvinfo_rd_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(MVINFO_DMA_MVINFO_RD_status_reg, mvinfo_dma_mvinfo_rd_status_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_kmvinfo_dm_reg.regValue)>>0)&0xff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809b1f4 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_kmvinfo_dm_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(MVINFO_DMA_KMVINFO_DM_reg, mvinfo_dma_kmvinfo_dm_reg.regValue);
			cnt_1++;
		}
		if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_bist_kme_buff_st_reg.regValue)>>0)&0xffffffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] lbme_underflorw_status (b809f048 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_bist_kme_buff_st_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(KME_BIST_KME_BUFF_ST_reg, kme_bist_kme_buff_st_reg.regValue);
			cnt_1++;
		}

		if(sys_reg_sys_clken3_reg.clken_memc_me && (((mc_dma_mc_dma_index_reg.regValue)>>12)&0x3) != 0 && get_scaler_qms_mode_flag() == FALSE){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mc_rw_conflict_status (b809aeb8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mc_dma_mc_dma_index_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(MC_DMA_MC_DMA_index_reg, mc_dma_mc_dma_index_reg.regValue);
			cnt_1++;
		}
		if((((lbmc_lbmc_e8_reg.regValue)>>0)&0xfffffff) != 0){
			if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
				rtd_pr_memc_notice( "[DMA Check] mc_read_dma_error_flag (b80999e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", lbmc_lbmc_e8_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			}
			rtd_outl(LBMC_LBMC_E8_reg, lbmc_lbmc_e8_reg.regValue);
			cnt_1++;
		}

	}
	else{
		if(MEMC_LibGetMEMCLowDelayModeEnable() == FALSE && MEMC_LibGetMEMC_PCModeEnable() == FALSE){
			if(lbmc_lbmc_24_reg.lbmcpc_mode_sel_p){ // PZ
				if(lbmc_lbmc_e8_reg.hf_p_not_enough || lbmc_lbmc_e8_reg.hf_p_underflow || lbmc_lbmc_e8_reg.lfh_p_not_enough || lbmc_lbmc_e8_reg.lfh_p_underflow || lbmc_lbmc_e8_reg.lfl_p_not_enough || lbmc_lbmc_e8_reg.lfl_p_underflow){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check][PZ] mc_read_dma_error_flag (b80999e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", lbmc_lbmc_e8_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(LBMC_LBMC_E8_reg, lbmc_lbmc_e8_reg.regValue);
					cnt_1++;
				}
			}else{ // IZ
				if(lbmc_lbmc_e8_reg.hf_i_not_enough || lbmc_lbmc_e8_reg.hf_i_underflow || lbmc_lbmc_e8_reg.lfh_i_not_enough || lbmc_lbmc_e8_reg.lfh_i_underflow || lbmc_lbmc_e8_reg.lfl_i_not_enough || lbmc_lbmc_e8_reg.lfl_i_underflow){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check][IZ] mc_read_dma_error_flag (b80999e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", lbmc_lbmc_e8_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(LBMC_LBMC_E8_reg, lbmc_lbmc_e8_reg.regValue);
					cnt_1++;
				}
			}
		}
	}
	#endif

	return cnt_1;
}

VOID Debug_Msg_Print(_PQLPARAMETER* g_pParam, _PQLCONTEXT* pContext)
{
	const _PQLCONTEXT *s_pContext = GetPQLContext();
	//*******************************//
	//********FrameRate Change********//
	unsigned int in_phase=0;
	unsigned int out_phase=0;

	unsigned int u32_iFrameRateHz_pre = 0;
	unsigned int u32_iFrameRateHz = 0;
	unsigned int u32_oFrameRateHz_pre = 0;
	unsigned int u32_oFrameRateHz = 0;
	unsigned int u32_iFrameRateTh = (27000000/50 - 27000000/60)/2;
	unsigned int u32_oFrameRateTh = (27000000/110 - 27000000/120)/2;
	unsigned int u32_iFrameRate = 0;
	unsigned int u32_oFrameRate = 0;
	static unsigned int u32_iFrameRate_Pre = 0;
	static unsigned int u32_oFrameRate_Pre = 0;
	unsigned char u1_trig_inPhase_noMatch, u1_trig_outPhase_noMatch, u1_trig_iFrameRate_noMatch, u1_trig_oFrameRate_noMatch, u1_IOPhase_N2M_logEn, u1_IOPhase_N2M_FrameRate_logEn;
	unsigned int ippre_mute_hde_min_abn = 0, ippre_mute_hde_max_abn = 0, ippre_mute_htotal_abn = 0, ippre_mute_vde_min_abn = 0, ippre_mute_vde_max_abn = 0, ippre_mute_vact = 0, ippre_mute_hact = 0;
	unsigned int mc_wrt_idx = 0, mc_read_i_idx = 0, mc_read_p_idx = 0, bBGEn = 0, MEMC_BGEn = 0, MEMC_MuxEn = 0, MEMC_Mux_BGEn;
	ppoverlay_display_timing_ctrl1_RBUS display_timing_ctrl1_reg;
#if 0//remove dummy variable
	sys_reg_sys_clken3_RBUS sys_reg_sys_clken3_reg;
	kme_dm_top0_kme_dm_error_flag_RBUS kme_dm_top0_kme_dm_error_flag_reg;
	kme_dm_top2_nousemv_12_end_address7_RBUS kme_dm_top2_nousemv_12_end_address7_reg;
	kme_dm_top2_mv_dm_error_flag_RBUS kme_dm_top2_mv_dm_error_flag_reg;
	me_share_dma_kme_dma_status_RBUS me_share_dma_kme_dma_status_reg;
	kme_dm_top2_kme_me_shr_st_RBUS kme_dm_top2_kme_me_shr_st_reg;
	mvinfo_dma_kmvinfo_status_RBUS mvinfo_dma_kmvinfo_status_reg;
	//dbus_wrapper_irq_status_RBUS dbus_wrapper_irq_status_reg; // fix me : rbus time out
	me_share_dma_me_dma_rd_status_RBUS me_share_dma_me_dma_rd_status_reg;
	me_share_dma_me_dma_rd_status1_RBUS me_share_dma_me_dma_rd_status1_reg;
	mvinfo_dma_mvinfo_wr_status_RBUS mvinfo_dma_mvinfo_wr_status_reg;
	mvinfo_dma_mvinfo_wr_status1_RBUS mvinfo_dma_mvinfo_wr_status1_reg;
	mvinfo_dma_mvinfo_rd_status_RBUS mvinfo_dma_mvinfo_rd_status_reg;
	mvinfo_dma_kmvinfo_dm_RBUS mvinfo_dma_kmvinfo_dm_reg;
	kme_bist_kme_buff_st_RBUS kme_bist_kme_buff_st_reg;
	mc_dma_mc_dma_index_RBUS mc_dma_mc_dma_index_reg;
#endif
	memc_mux_memc_mux_func_ctrl_RBUS memc_mux_memc_mux_func_ctrl_reg;
#if 0//remove dummy variable
	lbmc_lbmc_e8_RBUS lbmc_lbmc_e8_reg;
#endif
	M8P_rthw_ippre_top_ippre1_44_RBUS M8P_rthw_ippre_top_ippre1_44_reg;
#if 0//remove dummy variable
	M8P_rtmc_lbmc_lbmc_ctrl_RBUS M8P_rtmc_lbmc_lbmc_ctrl_reg;
	M8P_rthw_me_dma_rtme_dma_status_RBUS M8P_rthw_me_dma_rtme_dma_status_reg;
	M8P_rthw_me_dma_dm0_wr_status_RBUS M8P_rthw_me_dma_dm0_wr_status;
	M8P_rthw_me_dma_dm1_wr_status_RBUS M8P_rthw_me_dma_dm1_wr_status;
	M8P_rthw_me_dma_dm2_wr_status_RBUS M8P_rthw_me_dma_dm2_wr_status;
	M8P_rthw_me_dma_dm3_rd_status_RBUS M8P_rthw_me_dma_dm3_rd_status;
	M8P_rthw_me_dma_dm4_rd_status_RBUS M8P_rthw_me_dma_dm4_rd_status;
	M8P_rthw_me_dma_dm5_rd_status_RBUS M8P_rthw_me_dma_dm5_rd_status;
	M8P_rthw_me_dma_dm6_rd_status_RBUS M8P_rthw_me_dma_dm6_rd_status;
	M8P_rthw_me_dma_dm7_rd_status_RBUS M8P_rthw_me_dma_dm7_rd_status;
	M8P_rthw_me_dma_dm0_wr_status1_RBUS M8P_rthw_me_dma_dm0_wr_status1;
	M8P_rthw_me_dma_dm1_wr_status1_RBUS M8P_rthw_me_dma_dm1_wr_status1;
	M8P_rthw_me_dma_dm2_wr_status1_RBUS M8P_rthw_me_dma_dm2_wr_status1;
	M8P_rthw_me_dma_dm3_rd_status1_RBUS M8P_rthw_me_dma_dm3_rd_status1;
	M8P_rthw_me_dma_dm4_rd_status1_RBUS M8P_rthw_me_dma_dm4_rd_status1;
	M8P_rthw_me_dma_dm5_rd_status1_RBUS M8P_rthw_me_dma_dm5_rd_status1;
	M8P_rthw_me_dma_dm6_rd_status1_RBUS M8P_rthw_me_dma_dm6_rd_status1;
	M8P_rthw_me_dma_dm7_rd_status1_RBUS M8P_rthw_me_dma_dm7_rd_status1;
#endif
	#if 0 //RBUS_NOT_READY
	M8P_rthw_me_top2_kme_buff_st_RBUS M8P_rthw_me_top2_kme_buff_st;
	#endif
#if 0//remove dummy variable
	M8P_rthw_mc_dma_mc_dma_index_RBUS M8P_rthw_mc_dma_mc_dma_index_reg;
	M8P_rthw_lbmc_lbmc_err_RBUS M8P_rthw_lbmc_lbmc_err_reg;
#endif
	unsigned char cnt_1 = 0, input_size_err_cnt = 0;
	static int cnt_2 = 0, repeat_mode_pre = 0, repeat_mode_chg_cnt = 0;
	unsigned int log_en = 0, repeat_mode = 0;
#if 0//remove dummy variable
	unsigned int lbmcpc_mode_sel_p = 0;
#endif
	PQL_INPUT_FRAME_RATE in_fmRate = 0;
	PQL_OUTPUT_FRAME_RATE  out_fmRate = 0;

	//if ((webos_tooloption.eModelModuleType == module_LGD) && (webos_tooloption.eBackLight == direct ) && (webos_tooloption.eLEDBarType == local_dim_block_32)){
	if(Get_TIMING_PROTECT_PANEL_TYPE()){
		u32_oFrameRateTh = (27000000/116 - 27000000/120)/2;
	}

	in_fmRate = s_pContext->_external_data._input_frameRate;
	out_fmRate = s_pContext->_external_data._output_frameRate;
	in_phase = HAL_MC_GetKphaseInPhase();
	out_phase = HAL_MC_GetKphaseOutPhase();
	RTKReadRegister(PPOVERLAY_uzudtg_DVS_cnt_reg, &u32_iFrameRate);
	RTKReadRegister(PPOVERLAY_memcdtg_DVS_cnt_reg, &u32_oFrameRate);
	bBGEn = rtd_inl(PPOVERLAY_Main_Display_Control_RSV_reg) & _BIT1;
	MEMC_Mux_BGEn = PPOVERLAY_MEMC_MUX_CTRL_BG_get_memc_out_bg_en(rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_BG_reg));
	MEMC_BGEn = HAL_POST_GetMuteScreenEnable();
	MEMC_MuxEn = PPOVERLAY_MEMC_MUX_CTRL_get_memc_outmux_sel(rtd_inl(PPOVERLAY_MEMC_MUX_CTRL_reg));
	display_timing_ctrl1_reg.regValue = rtd_inl(PPOVERLAY_Display_Timing_CTRL1_reg); // d-domain clock
	ReadRegister(SOFTWARE1_SOFTWARE1_60_reg, 25, 25, &log_en);

	u32_iFrameRateHz_pre    = (u32_iFrameRate_Pre==0) ? 0: (27000000*10/u32_iFrameRate_Pre);
	u32_iFrameRateHz        =  (u32_iFrameRate==0) ? 0: (27000000*10/u32_iFrameRate);
	u32_oFrameRateHz_pre    =  (u32_oFrameRate_Pre==0) ? 0: (27000000*10/u32_oFrameRate_Pre);
	u32_oFrameRateHz        =  (u32_oFrameRate==0) ? 0: (27000000*10/u32_oFrameRate);

	u1_trig_inPhase_noMatch		= (in_phase >= s_pContext->_output_frc_phtable.u8_sys_N_pre);
	u1_trig_outPhase_noMatch		= (out_phase > s_pContext->_output_frc_phtable.u8_sys_M_pre);
	u1_trig_iFrameRate_noMatch	= (u32_iFrameRate>u32_iFrameRate_Pre)? ((u32_iFrameRate -u32_iFrameRate_Pre) > u32_iFrameRateTh) : ((u32_iFrameRate_Pre -u32_iFrameRate) > u32_iFrameRateTh);
	u1_trig_oFrameRate_noMatch    = (u32_oFrameRate>u32_oFrameRate_Pre)? ((u32_oFrameRate -u32_oFrameRate_Pre) > u32_oFrameRateTh) : ((u32_oFrameRate_Pre -u32_oFrameRate) > u32_oFrameRateTh);

	u1_IOPhase_N2M_logEn = g_pParam->_param_filmDetectctrl.u1_DbgPrt_FRChg_En;
	u1_IOPhase_N2M_FrameRate_logEn = 1;
	
	memc_mux_memc_mux_func_ctrl_reg.regValue = rtd_inl(MEMC_MUX_MEMC_MUX_FUNC_CTRL_reg);
	
	if(memc_mux_memc_mux_func_ctrl_reg.memc_mux_en == 0){
		return;
	}

	if(u1_trig_inPhase_noMatch && u1_IOPhase_N2M_logEn)
		rtd_pr_memc_notice("[MEMC self check][%s][%d] inPhase >= N!!!(inPh = %d, N = %d)[DTG in/out : %d/%d][MEMC in/out : %d/%d]\n\r", __FUNCTION__, __LINE__, in_phase, s_pContext->_output_frc_phtable.u8_sys_N_pre, (u32_iFrameRateHz/10), (u32_oFrameRateHz/10), in_fmRate, out_fmRate);
	if(u1_trig_outPhase_noMatch && u1_IOPhase_N2M_logEn)
		rtd_pr_memc_notice("[MEMC self check][%s][%d] outPhase > M!!!(outPh = %d, M = %d)[DTG in/out : %d/%d][MEMC in/out : %d/%d]\n\r", __FUNCTION__, __LINE__, out_phase, s_pContext->_output_frc_phtable.u8_sys_M_pre, (u32_iFrameRateHz/10), (u32_oFrameRateHz/10), in_fmRate, out_fmRate);
	if(u1_trig_iFrameRate_noMatch  && u1_IOPhase_N2M_FrameRate_logEn)
		rtd_pr_memc_notice("[FR] in FrameRate Change!!!(%dHz(%d) -> %dHz(%d))(90K = %d)\n\r", u32_iFrameRateHz_pre, u32_iFrameRate_Pre, u32_iFrameRateHz, u32_iFrameRate, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	if(u1_trig_oFrameRate_noMatch  && u1_IOPhase_N2M_FrameRate_logEn)
		rtd_pr_memc_notice("[FR] out FrameRate Change!!!(%dHz(%d) -> %dHz(%d))(90K = %d)\n\r", u32_oFrameRateHz_pre, u32_oFrameRate_Pre, u32_oFrameRateHz, u32_oFrameRate, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));

	//k23
	if(u1_trig_oFrameRate_noMatch && 955 <= u32_oFrameRateHz_pre && u32_oFrameRateHz_pre <= 965 
		&& 1195<=u32_oFrameRateHz && u32_oFrameRateHz <= 1205 
		&& 235<=u32_iFrameRateHz && u32_iFrameRateHz <= 245){
		cinema_out_frame_rate_change_flag = 1;
	}
	else{
		cinema_out_frame_rate_change_flag = 0;
	}


	//for oled 120Hz panel, display vtotal < 2244(120.32Hz) will happen black video
	//if((webos_tooloption.eBackLight == 2) && (Get_DISPLAY_REFRESH_RATE() == 120) && (u32_oFrameRate < OLED_DISP_FRAMERATE_LIMIT_120HZ))
	//if((webos_tooloption.eBackLight == 2) && (Get_DISPLAY_REFRESH_RATE() >= 120) && (u32_oFrameRate < OLED_DISP_FRAMERATE_LIMIT_120HZ))//k23
		//rtd_pr_memc_notice( "[FR] out FrameRate Over Range!!!(%dHz(%d) -> %dHz(%d))(90K = %d)\n\r", u32_oFrameRateHz_pre, u32_oFrameRate_Pre, u32_oFrameRateHz, u32_oFrameRate, rtd_inl(0xB801B6B8));
	//for oled 60Hz panel, display vtotal < 2243(60.18Hz) will happen black video
	//if((webos_tooloption.eBackLight == 2) && (Get_DISPLAY_REFRESH_RATE() == 60) && (u32_oFrameRate < OLED_DISP_FRAMERATE_LIMIT_60HZ))
		//rtd_pr_memc_notice( "[FR] out FrameRate Over Range!!!(%dHz(%d) -> %dHz(%d))(90K = %d)\n\r", u32_oFrameRateHz_pre, u32_oFrameRate_Pre, u32_oFrameRateHz, u32_oFrameRate, rtd_inl(0xB801B6B8));

	//input size monitor
	if(RUN_MERLIN8P()){
		ReadRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,13,13, &ippre_mute_hde_min_abn);
		ReadRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,14,14, &ippre_mute_hde_max_abn);
		ReadRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,15,15, &ippre_mute_htotal_abn);
		ReadRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,16,16, &ippre_mute_vde_min_abn);
		ReadRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,17,17, &ippre_mute_vde_max_abn);
		M8P_rthw_ippre_top_ippre1_44_reg.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_44_reg);
		ippre_mute_vact = M8P_rthw_ippre_top_ippre1_44_reg.regr_ippre_mute_vact;
		ippre_mute_hact = M8P_rthw_ippre_top_ippre1_44_reg.regr_ippre_mute_hact;
	}else if(RUN_MERLIN8()){
		ReadRegister(IPPRE_IPPRE_F4_reg,13,13, &ippre_mute_hde_min_abn);
		ReadRegister(IPPRE_IPPRE_F4_reg,14,14, &ippre_mute_hde_max_abn);
		ReadRegister(IPPRE_IPPRE_F4_reg,15,15, &ippre_mute_htotal_abn);
		ReadRegister(IPPRE_IPPRE_F4_reg,16,16, &ippre_mute_vde_min_abn);
		ReadRegister(IPPRE_IPPRE_F4_reg,17,17, &ippre_mute_vde_max_abn);
		ReadRegister(IPPRE1_IPPRE1_D0_reg,16,28, &ippre_mute_vact);
		ReadRegister(IPPRE1_IPPRE1_D0_reg,0,12, &ippre_mute_hact);
	}//RUN_MERLIN8P & RUN_MERLIN8
#if 0//remove dummy variable
	sys_reg_sys_clken3_reg.regValue = rtd_inl(SYS_REG_SYS_CLKEN3_reg);
#endif
	repeat_mode = HAL_MC_GetRepeatMode();
#if 0//remove dummy variable
	lbmcpc_mode_sel_p = HAL_MC_GetPZSel();
	if(RUN_MERLIN8P()){
		M8P_rtmc_lbmc_lbmc_ctrl_reg.regValue = rtd_inl(M8P_RTMC_LBMC_LBMC_CTRL_reg);
		M8P_rthw_me_dma_rtme_dma_status_reg.regValue = rtd_inl(M8P_RTHW_ME_DMA_RTME_DMA_STATUS_reg);
	}else if(RUN_MERLIN8()){
		kme_dm_top0_kme_dm_error_flag_reg.regValue = rtd_inl(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg);
		kme_dm_top2_nousemv_12_end_address7_reg.regValue = rtd_inl(KME_DM_TOP2_NOUSEMV_12_END_ADDRESS7_reg);
		kme_dm_top2_mv_dm_error_flag_reg.regValue = rtd_inl(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg);;
		me_share_dma_kme_dma_status_reg.regValue = rtd_inl(ME_SHARE_DMA_KME_DMA_STATUS_reg);
		kme_dm_top2_kme_me_shr_st_reg.regValue = rtd_inl(KME_DM_TOP2_KME_ME_SHR_ST_reg);
		mvinfo_dma_kmvinfo_status_reg.regValue = rtd_inl(MVINFO_DMA_KMVINFO_STATUS_reg);
	}//RUN_MERLIN8P & RUN_MERLIN8
#endif
	if(MEMC_Lib_get_memc_PowerSaving_Mode()== 0){
		//dbus_wrapper_irq_status_reg.regValue = rtd_inl(DBUS_WRAPPER_irq_status_reg); // fix me : rbus time out
	}

#if 0//remove dummy variable
	if(RUN_MERLIN8P()){
		#if 0//RBUS_NOT_READY
		M8P_rthw_me_top2_kme_buff_st.regValue = rtd_inl(M8P_RTHW_ME_TOP2_KME_BUFF_ST_reg);
		#endif
		M8P_rthw_mc_dma_mc_dma_index_reg.regValue = rtd_inl(M8P_RTHW_MC_DMA_MC_DMA_index_reg);
		M8P_rthw_lbmc_lbmc_err_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_ERR_reg);
		M8P_rthw_me_dma_dm0_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_status_reg);
		M8P_rthw_me_dma_dm1_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_status_reg);
		M8P_rthw_me_dma_dm2_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_status_reg);
		M8P_rthw_me_dma_dm3_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_status_reg);
		M8P_rthw_me_dma_dm4_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_status_reg);
		M8P_rthw_me_dma_dm5_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_status_reg);
		M8P_rthw_me_dma_dm6_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_status_reg);
		M8P_rthw_me_dma_dm7_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_status_reg);
		M8P_rthw_me_dma_dm0_wr_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_status1_reg);
		M8P_rthw_me_dma_dm1_wr_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_status1_reg);
		M8P_rthw_me_dma_dm2_wr_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_status1_reg);
		M8P_rthw_me_dma_dm3_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_status1_reg);
		M8P_rthw_me_dma_dm4_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_status1_reg);
		M8P_rthw_me_dma_dm5_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_status1_reg);
		M8P_rthw_me_dma_dm6_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_status1_reg);
		M8P_rthw_me_dma_dm7_rd_status1.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_status1_reg);
	}else if(RUN_MERLIN8()){
		me_share_dma_me_dma_rd_status_reg.regValue = rtd_inl(ME_SHARE_DMA_ME_DMA_RD_status_reg);
		me_share_dma_me_dma_rd_status1_reg.regValue = rtd_inl(ME_SHARE_DMA_ME_DMA_RD_status1_reg);
		mvinfo_dma_mvinfo_wr_status_reg.regValue = rtd_inl(MVINFO_DMA_MVINFO_WR_status_reg);
		mvinfo_dma_mvinfo_wr_status1_reg.regValue = rtd_inl(MVINFO_DMA_MVINFO_WR_status1_reg);
		mvinfo_dma_mvinfo_rd_status_reg.regValue = rtd_inl(MVINFO_DMA_MVINFO_RD_status_reg);
		mvinfo_dma_kmvinfo_dm_reg.regValue = rtd_inl(MVINFO_DMA_KMVINFO_DM_reg);
		kme_bist_kme_buff_st_reg.regValue = rtd_inl(KME_BIST_KME_BUFF_ST_reg);
		mc_dma_mc_dma_index_reg.regValue = rtd_inl(MC_DMA_MC_DMA_index_reg);
		lbmc_lbmc_e8_reg.regValue = rtd_inl(LBMC_LBMC_E8_reg);
	}//RUN_MERLIN8P & RUN_MERLIN8
#endif

	if(repeat_mode != repeat_mode_pre){
		repeat_mode_chg_cnt = 16; // 8(buffer) * 2 (output_isr)
	}

	if((log_en==1) || (display_timing_ctrl1_reg.disp_en == 1 && ((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY, SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) || MEMC_error_status_print_flag) && MEMC_MuxEn) || ((u1_trig_iFrameRate_noMatch || u1_trig_oFrameRate_noMatch)&&(g_KERNEL_AFTER_QS_MEMC_FLG==false || u32_oFrameRate_Pre!=0))){
		if(ippre_mute_hde_min_abn){
			rtd_pr_memc_notice( "[inpur size error] hde_min_abn(b80991f4[13]) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			if(RUN_MERLIN8P()){
				WriteRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,13,13, 0x1);
			}else if(RUN_MERLIN8()){
				WriteRegister(IPPRE_IPPRE_F4_reg,13,13, 0x1);
			}//RUN_MERLIN8P & RUN_MERLIN8
			input_size_err_cnt++;
		}
		if(ippre_mute_hde_max_abn){
			rtd_pr_memc_notice( "[inpur size error] hde_max_abn(b80991f4[14]) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			if(RUN_MERLIN8P()){
				WriteRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,14,14, 0x1);
			}else if(RUN_MERLIN8()){
				WriteRegister(IPPRE_IPPRE_F4_reg,14,14, 0x1);
			}//RUN_MERLIN8P & RUN_MERLIN8
			input_size_err_cnt++;
		}
		if(ippre_mute_htotal_abn){
			//memc 24hz, htotal 5500 will print this log continously.
			//rtd_pr_memc_notice( "[inpur size error] htotal_abn(b80991f4[15]) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			if(RUN_MERLIN8P()){
				WriteRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,15,15, 0x1);
			}else if(RUN_MERLIN8()){
				WriteRegister(IPPRE_IPPRE_F4_reg,15,15, 0x1);
			}//RUN_MERLIN8P & RUN_MERLIN8
			input_size_err_cnt++;
		}
		if(ippre_mute_vde_min_abn){
			rtd_pr_memc_notice( "[inpur size error] vde_min_abn(b80991f4[16]) !!!(BG=%d/%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", bBGEn, MEMC_BGEn, MEMC_Mux_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			if(RUN_MERLIN8P()){
				WriteRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,16,16, 0x1);
			}else if(RUN_MERLIN8()){
				WriteRegister(IPPRE_IPPRE_F4_reg,16,16, 0x1);
			}//RUN_MERLIN8P & RUN_MERLIN8
			input_size_err_cnt++;
		}
		if(ippre_mute_vde_max_abn){
			rtd_pr_memc_notice( "[inpur size error] vde_max_abn(b80991f4[17]) !!!(BG=%d/%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", bBGEn, MEMC_BGEn, MEMC_Mux_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
			if(RUN_MERLIN8P()){
				WriteRegister(M8P_RTHW_IPPRE_TOP_IPPRE_08_reg,17,17, 0x1);
			}else if(RUN_MERLIN8()){
				WriteRegister(IPPRE_IPPRE_F4_reg,17,17, 0x1);
			}//RUN_MERLIN8P & RUN_MERLIN8
			input_size_err_cnt++;
		}
		if(ippre_mute_hact != GS_image_hsize/2 && GS_image_hsize!=0){
			input_size_err_cnt++;
			rtd_pr_memc_notice( "[inpur size error] hact (b80995d0[0:12] = %d) !!!(BG=%d/%d/%d)(MEMC_Mux=%d)(90K = %d)  GS_image_hsize=%d\n\r", ippre_mute_hact, bBGEn, MEMC_BGEn, MEMC_Mux_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg), GS_image_hsize);
		}
		if(ippre_mute_vact != GS_image_vsize && GS_image_vsize!=0){//if(ippre_mute_vact != 2160 && ippre_mute_vact != 1088)
			input_size_err_cnt++;
			rtd_pr_memc_notice( "[inpur size error] vact (b80995d0[16:28] = %d) !!!(BG=%d/%d/%d)(MEMC_Mux=%d)(90K = %d)  GS_image_vsize=%d\n\r", ippre_mute_vact, bBGEn, MEMC_BGEn, MEMC_Mux_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg), GS_image_vsize);
		}

		if((input_size_err_cnt !=0) && MEMC_Mux_BGEn){
			MEMC_LibBlueScreen(1);
		}

		#if 1
		#if 1
		if(RUN_MERLIN8P()){
			cnt_1 = DMA_Check_RTK2885pp(cnt_2, repeat_mode_chg_cnt, repeat_mode_pre);
		}else if(RUN_MERLIN8()){
			cnt_1 = DMA_Check_RTK2885p(cnt_2, repeat_mode_chg_cnt, repeat_mode_pre);
		}//RUN_MERLIN8P & RUN_MERLIN8
		#else//old
		if(repeat_mode == 0){
			if(RUN_MERLIN8P()){
				//rdma_datmem_overflow/wdma_datmem_underflow
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_rtme_dma_status_reg.regValue)>>0)&0xffffffff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice("[DMA Check] me_share_dm_error_flag (b809fdd0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_rtme_dma_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_RTME_DMA_STATUS_reg, M8P_rthw_me_dma_rtme_dma_status_reg.regValue);
					cnt_1++;
				}

				//me ack num error
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm0_wr_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809f270 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm0_wr_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM0_WR_status1_reg, M8P_rthw_me_dma_dm0_wr_status1.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm1_wr_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809f370 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm1_wr_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM1_WR_status1_reg, M8P_rthw_me_dma_dm1_wr_status1.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm2_wr_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809f4b0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm2_wr_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM2_WR_status1_reg, M8P_rthw_me_dma_dm2_wr_status1.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm3_rd_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fa60 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm3_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM3_RD_status1_reg, M8P_rthw_me_dma_dm3_rd_status1.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm4_rd_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fb50 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm4_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM4_RD_status1_reg, M8P_rthw_me_dma_dm4_rd_status1.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm5_rd_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fc90 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm5_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM5_RD_status1_reg, M8P_rthw_me_dma_dm5_rd_status1.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm6_rd_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fdc0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm6_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM6_RD_status1_reg, M8P_rthw_me_dma_dm6_rd_status1.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm7_rd_status1.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809fa10 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm7_rd_status1.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM7_RD_status1_reg, M8P_rthw_me_dma_dm7_rd_status1.regValue);
					cnt_1++;
				}

				//me zero length
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm0_wr_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809f26c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm0_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM0_WR_status_reg, M8P_rthw_me_dma_dm0_wr_status.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm1_wr_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809f36c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm1_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM1_WR_status_reg, M8P_rthw_me_dma_dm1_wr_status.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm2_wr_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809f4ac = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm2_wr_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM2_WR_status_reg, M8P_rthw_me_dma_dm2_wr_status.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm3_rd_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fa5c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm3_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM3_RD_status_reg, M8P_rthw_me_dma_dm3_rd_status.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm4_rd_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fb4c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm4_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM4_RD_status_reg, M8P_rthw_me_dma_dm4_rd_status.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm5_rd_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fc8c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm5_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM5_RD_status_reg, M8P_rthw_me_dma_dm5_rd_status.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm6_rd_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fdbc = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm6_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM6_RD_status_reg, M8P_rthw_me_dma_dm6_rd_status.regValue);
					cnt_1++;
				}
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_dma_dm7_rd_status.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809fa0c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_dma_dm7_rd_status.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_DMA_DM7_RD_status_reg, M8P_rthw_me_dma_dm7_rd_status.regValue);
					cnt_1++;
				}

				//me1/me2 underflow
				#if 0//RBUS_NOT_READY
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_me_top2_kme_buff_st.regValue)>>0)&0xffffffff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice("[DMA Check] lbme_underflorw_status (b809bf48 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_me_top2_kme_buff_st.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_ME_TOP2_KME_BUFF_ST_reg, M8P_rthw_me_top2_kme_buff_st.regValue);
					cnt_1++;
				}
				#endif

				//dma_index_conflict
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((M8P_rthw_mc_dma_mc_dma_index_reg.regValue)>>12)&0x3) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice("[DMA Check] mc_rw_conflict_status (b80997b8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_mc_dma_mc_dma_index_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_MC_DMA_MC_DMA_index_reg, M8P_rthw_mc_dma_mc_dma_index_reg.regValue);
					cnt_1++;
				}
				//underflow/ not enough
				if((((M8P_rthw_lbmc_lbmc_err_reg.regValue)>>0)&0xfffffff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice("[DMA Check] mc_read_dma_error_flag (b8099f84 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(M8P_RTHW_LBMC_LBMC_ERR_reg, M8P_rthw_lbmc_lbmc_err_reg.regValue);
					cnt_1++;
				}
			}else if(RUN_MERLIN8()){//Merlin8
				//last/overflow error
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top0_kme_dm_error_flag_reg.regValue)>>9)&0x7fffff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] kme_dm_error_flag (b809c0f8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top0_kme_dm_error_flag_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg, kme_dm_top0_kme_dm_error_flag_reg.regValue);
					cnt_1++;
				}
				//last error
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top2_nousemv_12_end_address7_reg.regValue)>>7)&0x1ff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] kmv_dm_error_flag (b809c2f4 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top2_nousemv_12_end_address7_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(KME_DM_TOP2_NOUSEMV_12_END_ADDRESS7_reg, kme_dm_top2_nousemv_12_end_address7_reg.regValue);
					cnt_1++;
				}
				//last/overflow error
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top2_mv_dm_error_flag_reg.regValue)>>16)&0x7f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] kmv_dm_error_flag (b809c2f8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top2_mv_dm_error_flag_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg, kme_dm_top2_mv_dm_error_flag_reg.regValue);
					cnt_1++;
				}
				//rdma_datmem_overflow/wdma_datmem_underflow
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((me_share_dma_kme_dma_status_reg.regValue)>>0)&0xffffffff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] me_share_dm_error_flag (b809b7c0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", me_share_dma_kme_dma_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(ME_SHARE_DMA_KME_DMA_STATUS_reg, me_share_dma_kme_dma_status_reg.regValue);
					cnt_1++;
				}
				//me_pqdc_ipme_fifo_udflw_ro
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_dm_top2_kme_me_shr_st_reg.regValue)>>0)&0x7) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] me_share_dm_error_flag (b809c290 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_dm_top2_kme_me_shr_st_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(KME_DM_TOP2_KME_ME_SHR_ST_reg, kme_dm_top2_kme_me_shr_st_reg.regValue);
					cnt_1++;
				}
				//mv info error
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_kmvinfo_status_reg.regValue)>>0)&0x7) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mv_info_error_flag (b809b1a0 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_kmvinfo_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(MVINFO_DMA_KMVINFO_STATUS_reg, mvinfo_dma_kmvinfo_status_reg.regValue);
					cnt_1++;
				}
				if(MEMC_Lib_get_memc_PowerSaving_Mode()== 0){
					#if 0 // fix me : rbus time out
					if(sys_reg_sys_clken3_reg.clken_memc_me && (((dbus_wrapper_irq_status_reg.regValue)>>0)&0x3fff) != 0){
						if(cnt_2 < 20){
							rtd_pr_memc_notice( "[DMA Check] mv_info_error_flag (b809e0c8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", dbus_wrapper_irq_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
						}
						rtd_outl(DBUS_WRAPPER_irq_status_reg, dbus_wrapper_irq_status_reg.regValue);
						cnt_1++;
					}
					#endif
				}
				//mv share zero length
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((me_share_dma_me_dma_rd_status_reg.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] me_dma_rd_status (b809b7e4 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", me_share_dma_me_dma_rd_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(ME_SHARE_DMA_ME_DMA_RD_status_reg, me_share_dma_me_dma_rd_status_reg.regValue);
					cnt_1++;
				}
				//me share ack num error
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((me_share_dma_me_dma_rd_status1_reg.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] me_dma_rd_status1 (b809b7e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", me_share_dma_me_dma_rd_status1_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(ME_SHARE_DMA_ME_DMA_RD_status1_reg, me_share_dma_me_dma_rd_status1_reg.regValue);
					cnt_1++;
				}
				//mv info wr zero length
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_mvinfo_wr_status_reg.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status (b809b12c = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_mvinfo_wr_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(MVINFO_DMA_MVINFO_WR_status_reg, mvinfo_dma_mvinfo_wr_status_reg.regValue);
					cnt_1++;
				}
				//mv info ack num error
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_mvinfo_wr_status1_reg.regValue)>>31)&0x1) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_wr_status1 (b809b130 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_mvinfo_wr_status1_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(MVINFO_DMA_MVINFO_WR_status1_reg, mvinfo_dma_mvinfo_wr_status1_reg.regValue);
					cnt_1++;
				}
				//mv info rd zero length
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_mvinfo_rd_status_reg.regValue)>>0)&0x1f) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809b178 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_mvinfo_rd_status_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(MVINFO_DMA_MVINFO_RD_status_reg, mvinfo_dma_mvinfo_rd_status_reg.regValue);
					cnt_1++;
				}
				//dm_in_share00_trig0_err
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((mvinfo_dma_kmvinfo_dm_reg.regValue)>>0)&0xff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mvinfo_rd_status (b809b1f4 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mvinfo_dma_kmvinfo_dm_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(MVINFO_DMA_KMVINFO_DM_reg, mvinfo_dma_kmvinfo_dm_reg.regValue);
					cnt_1++;
				}
				//me1/me2 underflow
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((kme_bist_kme_buff_st_reg.regValue)>>0)&0xffffffff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] lbme_underflorw_status (b809f048 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", kme_bist_kme_buff_st_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(KME_BIST_KME_BUFF_ST_reg, kme_bist_kme_buff_st_reg.regValue);
					cnt_1++;
				}

				//dma_index_conflict
				if(sys_reg_sys_clken3_reg.clken_memc_me && (((mc_dma_mc_dma_index_reg.regValue)>>12)&0x3) != 0 && get_scaler_qms_mode_flag() == FALSE){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mc_rw_conflict_status (b809aeb8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", mc_dma_mc_dma_index_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(MC_DMA_MC_DMA_index_reg, mc_dma_mc_dma_index_reg.regValue);
					cnt_1++;
				}
				//underflow/ not enough
				if((((lbmc_lbmc_e8_reg.regValue)>>0)&0xfffffff) != 0){
					if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
						rtd_pr_memc_notice( "[DMA Check] mc_read_dma_error_flag (b80999e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", lbmc_lbmc_e8_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
					}
					rtd_outl(LBMC_LBMC_E8_reg, lbmc_lbmc_e8_reg.regValue);
					cnt_1++;
				}
			}//RUN_MERLIN8P & RUN_MERLIN8
		}
		else{
			if(MEMC_LibGetMEMCLowDelayModeEnable() == FALSE && MEMC_LibGetMEMC_PCModeEnable() == FALSE){
				if(RUN_MERLIN8P()){
					if(M8P_rtmc_lbmc_lbmc_ctrl_reg.lbmcpc_mode_sel_p){ // PZ
						if(M8P_rthw_lbmc_lbmc_err_reg.hf_p_not_enough || M8P_rthw_lbmc_lbmc_err_reg.hf_p_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfh_p_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfh_p_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfl_p_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfl_p_underflow){
							if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
								rtd_pr_memc_notice("[DMA Check][PZ] mc_read_dma_error_flag (b8099f84 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
							}
							rtd_outl(M8P_RTHW_LBMC_LBMC_ERR_reg, M8P_rthw_lbmc_lbmc_err_reg.regValue);
							cnt_1++;
						}
					}else{ // IZ
						if(M8P_rthw_lbmc_lbmc_err_reg.hf_i_not_enough || M8P_rthw_lbmc_lbmc_err_reg.hf_i_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfh_i_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfh_i_underflow || M8P_rthw_lbmc_lbmc_err_reg.lfl_i_not_enough || M8P_rthw_lbmc_lbmc_err_reg.lfl_i_underflow){
							if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
								rtd_pr_memc_notice("[DMA Check][IZ] mc_read_dma_error_flag (b8099f84 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", M8P_rthw_lbmc_lbmc_err_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
							}
							rtd_outl(M8P_RTHW_LBMC_LBMC_ERR_reg, M8P_rthw_lbmc_lbmc_err_reg.regValue);
							cnt_1++;
						}
					}
				}else if(RUN_MERLIN8()){
					if(lbmcpc_mode_sel_p){ // PZ
						if(lbmc_lbmc_e8_reg.hf_p_not_enough || lbmc_lbmc_e8_reg.hf_p_underflow || lbmc_lbmc_e8_reg.lfh_p_not_enough || lbmc_lbmc_e8_reg.lfh_p_underflow || lbmc_lbmc_e8_reg.lfl_p_not_enough || lbmc_lbmc_e8_reg.lfl_p_underflow){
							if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
								rtd_pr_memc_notice( "[DMA Check][PZ] mc_read_dma_error_flag (b80999e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", lbmc_lbmc_e8_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
							}
							rtd_outl(LBMC_LBMC_E8_reg, lbmc_lbmc_e8_reg.regValue);
							cnt_1++;
						}
					}else{ // IZ
						if(lbmc_lbmc_e8_reg.hf_i_not_enough || lbmc_lbmc_e8_reg.hf_i_underflow || lbmc_lbmc_e8_reg.lfh_i_not_enough || lbmc_lbmc_e8_reg.lfh_i_underflow || lbmc_lbmc_e8_reg.lfl_i_not_enough || lbmc_lbmc_e8_reg.lfl_i_underflow){
							if(cnt_2 < 20 && repeat_mode_chg_cnt == 0){
								rtd_pr_memc_notice( "[DMA Check][IZ] mc_read_dma_error_flag (b80999e8 = %x) !!!(BG=%d/%d)(MEMC_Mux=%d)(90K = %d)\n\r", lbmc_lbmc_e8_reg.regValue, bBGEn, MEMC_BGEn, MEMC_MuxEn, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
							}
							rtd_outl(LBMC_LBMC_E8_reg, lbmc_lbmc_e8_reg.regValue);
							cnt_1++;
						}
					}
				}//RUN_MERLIN8P & RUN_MERLIN8
			}
		}
		#endif //old

		if(cnt_1 > 0){
			dma_cnt=cnt_1;
		}
		else{
			dma_cnt=0;
		}


		if(cnt_1 > 0){
			cnt_2++;
		}
		else{
			if(cnt_2 >= 1)
				cnt_2--;
			else
				cnt_2=0;
		}

		if(cnt_2 >= 1000)
			cnt_2 = 1000;

		if(repeat_mode_chg_cnt > 0){
			repeat_mode_chg_cnt--;
		}
		
		#endif
	}

	mc_wrt_idx = HAL_MC_GetKphaseMCWriteIndex();
	mc_read_i_idx = HAL_MC_GetKphaseMCRead_i_Index();
	mc_read_p_idx = HAL_MC_GetKphaseMCRead_p_Index();
	if(mc_wrt_idx == mc_read_i_idx ||mc_wrt_idx == mc_read_p_idx){
		//rtd_pr_memc_notice( "[MC idx error] wrt=%d, read_i=%d, read_p=%d (90K = %d)\n\r", (rtd_inl(KPHASE_kphase_90_reg)>>0)&0xf, (rtd_inl(KPHASE_kphase_90_reg)>>4)&0xf, (rtd_inl(KPHASE_kphase_90_reg)>>8)&0xf, rtd_inl(TIMER_SCPU_CLK90K_LO_reg));
	}


	u32_iFrameRate_Pre      = u32_iFrameRate;
	u32_oFrameRate_Pre      = u32_oFrameRate;
	repeat_mode_pre = repeat_mode;
	//**************************//

}

VOID PQL_CRTCStatus_Init_RTK2885p(VOID)
{
	MEMC_MiddleWare_CRTC_StatusInit_RTK2885p(&(s_Context._output_me_sceneAnalysis));
}

VOID PQL_CRTCStatus_Init_RTK2885pp(VOID)
{
	//empty
}

VOID PQL_CRTCStatus_Init(VOID)
{
	if(RUN_MERLIN8P()){
		PQL_CRTCStatus_Init_RTK2885pp();
	}else if(RUN_MERLIN8()){
		PQL_CRTCStatus_Init_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8

}

VOID PQL_Init_By_ChangeSize_RTK2885p(VOID)
{
	ReadComReg_Init_RTK2885p(&(s_Context._output_read_comreg));
	
	//o------ new periodic algo ------o
#if CONFIG_REPEAT_NEW_ALOG
	Periodic_Init_RTK2885p(&(s_Context._output_periodic_manage));
	PRD_Init_RTK2885p(&(s_Context._output_prd_manage));
#endif

	//o------ basic info check ------o
	BasicInfoCheck_Init();
}

VOID PQL_Init_By_ChangeSize_RTK2885pp(VOID)
{
#ifndef BUILD_QUICK_SHOW
	ReadComReg_Init_RTK2885pp(&(s_Context._output_read_comreg));

	//o------ new periodic algo ------o
	PeriodicCtrl_Init_RTK2885pp(&(s_Context._output_periodic_ctrl));

	//o------ basic info check ------o
	BasicInfoCheck_Init();
#endif
}

VOID PQL_Init_By_ChangeSize(VOID)
{
	if(RUN_MERLIN8P()){
		PQL_Init_By_ChangeSize_RTK2885pp();
	}else if(RUN_MERLIN8()){
		PQL_Init_By_ChangeSize_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8
}

VOID PQL_ContextInit_RTK2885p(VOID)
{
#ifndef _WIN32
	s_pParam = GetPQLParameter();
#endif

	PQL_MessageInit(s_pParam,&s_Context);

	/////////////////////////////////////
	CalibMEMCInfo_init();
	RimCtrl_Init_RTK2885p(&(s_Context._output_rimctrl));
	FilmDetect_Init(&(s_Context._output_filmDetectctrl));
	FBLevelCtrl_Init_RTK2885p(&(s_Context._output_fblevelctrl));

	Dh_close_Init_RTK2885p(&(s_Context._output_dh_close));
	Mc_lbmcSwitch_Init_RTK2885p(&(s_Context._output_mc_lbmcswitch));
	FRC_phTable_Init(&(s_Context._output_frc_phtable));
	FRC_LogoDet_Init_RTK2885p(&(s_Context._output_FRC_LgDet));

	FRC_SceneAnalysis_Init_RTK2885p(&s_Context._output_frc_sceneAnalysis);

	Me_sceneAnalysis_Init_RTK2885p(&(s_Context._output_me_sceneAnalysis));
	Wrt_ComReg_Init_RTK2885p(&(s_Context._output_wrt_comreg));

	PatchManage_Init_RTK2885p(&(s_Context._output_Patch_Manage));
	ME_Vst_Init_RTK2885p(&(s_Context._output_me_vst_ctrl));


	//PQL_ContextProc_ReadParam_RTK2885p(s_pParam);
#ifndef BUILD_QUICK_SHOW
	memc_identify_ctrl_init();
	Logo_DDR_init_RTK2885p();
#endif
}

VOID PQL_ContextInit_RTK2885pp(VOID)
{
#ifndef _WIN32
	s_pParam = GetPQLParameter();
#endif

	PQL_MessageInit(s_pParam,&s_Context);

	/////////////////////////////////////
	FilmDetect_Init(&(s_Context._output_filmDetectctrl));
	FRC_phTable_Init(&(s_Context._output_frc_phtable));

#ifndef BUILD_QUICK_SHOW
	CalibMEMCInfo_init();
	RimCtrl_Init_RTK2885pp(&(s_Context._output_rimctrl));
	FBLevelCtrl_Init_RTK2885pp(&(s_Context._output_fblevelctrl));
	LBMCSwitchCtrl_Init_RTK2885pp(&(s_Context._output_mc_lbmcswitch));
	LogoCtrl_Init_RTK2885pp(&(s_Context._output_logo_ctrl));
	FRC_SceneAnalysis_Init_RTK2885pp(&s_Context._output_frc_sceneAnalysis);
	Me_sceneAnalysis_Init_RTK2885pp(&(s_Context._output_me_sceneAnalysis));
	Wrt_ComReg_Init_RTK2885pp(&(s_Context._output_wrt_comreg));
	PatchCtrl_Init_RTK2885pp(&(s_Context._output_patch_ctrl));
	SObjCtrl_Init_RTK2885pp(&(s_Context._output_sobj_ctrl));
	FadeInOutCtrl_Init_RTK2885pp(&(s_Context._output_fadeinout_ctrl));
	FlickerCtrl_Init_RTK2885pp(&(s_Context._output_flicker_ctrl));
	//PQL_ContextProc_ReadParam_RTK2885pp(s_pParam);
	memc_identify_ctrl_init();
#endif
}

VOID PQL_ContextInit(VOID)
{
	if(RUN_MERLIN8P()){
		PQL_ContextInit_RTK2885pp();
	}else if(RUN_MERLIN8()){
		PQL_ContextInit_RTK2885p();
	}//RUN_MERLIN8P & RUN_MERLIN8
}

BOOL PQL_ReadHDF(unsigned int uiAddr, int ibitStart, int ibitEnd, unsigned int *puiVal)
{
	BOOL bRet = TRUE;
	unsigned int u32Mask = 0xffffffff;

	unsigned int u32PageSize = 0, u32HdfPoint = 0;
	unsigned int uiAddr_page, uiAddr_content;
	unsigned char u8_page_mapping[0x47] = {  0,  1,  2,  3,  4,255,  5,  6,255,  7,  8,  9,255,255,255,255,
                                   255,255,255,255,255,255,255,255,255,255,255,255,255,255, 10,255,
                                    11,255, 12, 13, 14, 15, 16,255,255, 17,255, 18, 19, 20, 21, 22,
                                    23, 24, 25,255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,255,
                                    37, 38,255,255, 39, 40, 41};

	u32PageSize = (u8HDF_Array[FILE_HEADER_SIZE + PAGE_SIZE_OFFSET + 3] << 24) +
				  (u8HDF_Array[FILE_HEADER_SIZE + PAGE_SIZE_OFFSET + 2] << 16) +
				  (u8HDF_Array[FILE_HEADER_SIZE + PAGE_SIZE_OFFSET + 1] << 8) +
				   u8HDF_Array[FILE_HEADER_SIZE + PAGE_SIZE_OFFSET];

	uiAddr_page     = (uiAddr & 0x0000FF00)>>8;
	uiAddr_content  = (uiAddr & 0x000000FF);

	*puiVal = 0xDEADDEAD;

	if (/*uiAddr_page >= 0x00 &&*/ uiAddr_page <= 0x46)
	{
		if (u8_page_mapping[uiAddr_page] != 255)
		{
			uiAddr_page = u8_page_mapping[uiAddr_page];
		}
		else
		{
			uiAddr_page = 0;
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	u32HdfPoint = FILE_HEADER_SIZE + PAGE_HEADER_SIZE + uiAddr_page*(PAGE_HEADER_SIZE + u32PageSize) + uiAddr_content;
	//ROSPrintf("%x => %x\n\r", uiAddr, u32HdfPoint);

	if (ibitEnd < ibitStart)
	{
		return FALSE;
	}

	*puiVal = (u8HDF_Array[u32HdfPoint + 3] << 24) +
			  (u8HDF_Array[u32HdfPoint + 2] << 16) +
			  (u8HDF_Array[u32HdfPoint + 1] << 8) +
			  (u8HDF_Array[u32HdfPoint + 0] << 0);

	if ((uiAddr % 4 == 0) && (ibitEnd - ibitStart == 31))
	{
		return bRet;
	}
	else
	{
		// get mask
		u32Mask = u32Mask << (31 - ibitEnd);
		u32Mask = u32Mask >> (31 - ibitEnd);
		u32Mask = u32Mask >> ibitStart;
		u32Mask = u32Mask << ibitStart;

		// set mask
		*puiVal &= u32Mask;
		*puiVal = *puiVal >> ibitStart;

		return bRet;
	}
}

VOID PQL_PhaseTableInit(VOID)
{
#ifndef _WIN32
	s_pParam = GetPQLParameter();
#endif
	PQL_MessageInit(s_pParam,&s_Context);
	FRC_PhaseTable_Init(&(s_Context._output_frc_phtable));
	//k2 method, k3 remove but keep code
	//PatchManage_Init_RTK2885p(&(s_Context._output_Patch_Manage));
	//k2 method, k3 remove but keep code
	//Me_sceneAnalysis_Init(&(s_Context._output_me_sceneAnalysis));
}

VOID PQL_ContextProc_oneFifth_outputInterrupt_RTK2885p(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	ReadRegister(HARDWARE_HARDWARE_63_reg,29,29, &u32_rb_data);

	if(s_pParam == NULL){
		return;
	}

	if(u32_rb_data)
	{
		PQL_ContextProc_ReadParam_RTK2885p(s_pParam);

		ReadComReg_Proc_oneFifth_OutInterrupt_RTK2885p(&(s_pParam->_param_read_comreg),&(s_Context._output_read_comreg));

		RimCtrl_Proc_RTK2885p(&(s_pParam->_param_rimctrl),&(s_Context._output_rimctrl)); //K6_BRING_UP//

		RimCtrl_wrtAction_RTK2885p(&(s_pParam->_param_wrt_comreg));
		
		Clear_LogoRim_WrtAction_RTK2885p(&(s_Context._output_rimctrl), &(s_Context._output_wrt_comreg));

	}
}

VOID PQL_ContextProc_oneFifth_outputInterrupt_RTK2885pp(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	ReadRegister(HARDWARE_HARDWARE_63_reg,29,29, &u32_rb_data);

	if(s_pParam == NULL){
		return;
	}

	if(u32_rb_data)
	{
		PQL_ContextProc_ReadParam_RTK2885pp(s_pParam);

		ReadComReg_Proc_oneFifth_OutInterrupt_RTK2885pp(&(s_pParam->_param_read_comreg),&(s_Context._output_read_comreg));

		RimCtrl_Proc_RTK2885pp(&(s_pParam->_param_rimctrl),&(s_Context._output_rimctrl));

		RimCtrl_wrtAction_RTK2885pp(&(s_pParam->_param_wrt_comreg));
	}	
}

VOID PQL_ContextProc_oneFifth_outputInterrupt(int iSliceIdx)
{
	if(RUN_MERLIN8P()){
		PQL_ContextProc_oneFifth_outputInterrupt_RTK2885pp(iSliceIdx);
	}else if(RUN_MERLIN8()){
		PQL_ContextProc_oneFifth_outputInterrupt_RTK2885p(iSliceIdx);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

VOID PQL_ContextProc_outputInterrupt_Hsync_RTK2885p(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	UINT8 u8_scan_sel = HAL_ME_GetStatusScanSel();
	ReadRegister(HARDWARE_HARDWARE_63_reg,30,30, &u32_rb_data);
	u8_scan_sel = (u8_scan_sel<5) ? u8_scan_sel : 0;

	if(s_pParam == NULL){
		return;
	}

	if(u32_rb_data && (u8_scan_sel==0))
	{
		ReadComReg_Proc_inputInterrupt_Hsync_RTK2885p(&(s_pParam->_param_read_comreg),&(s_Context._output_read_comreg));
	}	
	
}

VOID PQL_ContextProc_outputInterrupt_Hsync_RTK2885pp(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	ReadRegister(HARDWARE_HARDWARE_63_reg,30,30, &u32_rb_data);

	if(s_pParam == NULL){
		return;
	}

	if(u32_rb_data)
	{
		MEMC_Lib_debug_message_print_HS_RTK2885pp();
	}
}

VOID PQL_ContextProc_outputInterrupt_Hsync(int iSliceIdx)
{
	if(RUN_MERLIN8P()){
		PQL_ContextProc_outputInterrupt_Hsync_RTK2885pp(iSliceIdx);
	}else if(RUN_MERLIN8()){
		PQL_ContextProc_outputInterrupt_Hsync_RTK2885p(iSliceIdx);
	}//RUN_MERLIN8P & RUN_MERLIN8

}

VOID PQL_ContextProc_inputInterrupt_RTK2885p(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	unsigned int MEMC_RepeatMode = HAL_MC_GetRepeatMode();
	ReadRegister(HARDWARE_HARDWARE_63_reg,30,30, &u32_rb_data);

	if(s_pParam == NULL){
		return;
	}

	RTICEDebug_OnlineMeasure(0);

	if(u32_rb_data)
	{
		Logo_DDR_Set_update_flag_RTK2885p(1);

		ReadComReg_Proc_inputInterrupt_RTK2885p(&(s_pParam->_param_read_comreg),&(s_Context._output_read_comreg));
		FilmDetectCtrl_Proc(&(s_pParam->_param_filmDetectctrl),&(s_Context._output_filmDetectctrl));//60HZ

		FRC_phTable_Proc(&(s_pParam->_param_frc_phtable), &(s_Context._output_frc_phtable));
		MEMC_Lib_Style_Select();

		if(MEMC_RepeatMode==false){
			FRC_SceneAnalysis_Proc_InputIntp_RTK2885p(&(s_pParam->_param_frc_sceneAnalysis), &(s_Context._output_frc_sceneAnalysis));

			FRC_LogoDet_Proc_RTK2885p(&(s_pParam->_param_FRC_LGDet), &(s_Context._output_FRC_LgDet));
			Mc_lbmcSwitch_Proc_RTK2885p(&(s_pParam->_param_mc_lbmcswitch), &(s_Context._output_mc_lbmcswitch));
			//MEMC_MiddleWare_Reg_Status_Cad1(&(s_Context._output_me_sceneAnalysis));
			MEMC_Panning_Detect_RTK2885p(&(s_Context._output_me_sceneAnalysis));
			MEMC_MainObject_Move_detect_RTK2885p(&(s_pParam->_param_me_sceneAnalysis), &(s_Context._output_me_sceneAnalysis));
		}
		Wrt_ComReg_Proc_intputInterrupt_RTK2885p(&(s_pParam->_param_wrt_comreg), &(s_Context._output_wrt_comreg));

		#if PQL_DYNAMIC_ADJUST_FIX_ISSUE
		// still pattern detect
		if(MEMC_RepeatMode==false){
			FRC_Still_Pattern_Detect(&(s_pParam->_param_filmDetectctrl), &(s_Context._output_filmDetectctrl), &(s_Context._output_frc_sceneAnalysis));
		}
		#endif

		MEMC_MotionInfo_SLD_RTK2885p(&(s_Context._output_me_sceneAnalysis));
		if(MEMC_RepeatMode==false){
			Logo_DDR_Proc_RTK2885p(&(s_Context._output_logo_ctrl));
			Identify_ctrl_Self_Check();
		}
	}
}
extern unsigned char MEMC_Pixel_LOGO_For_SW_SLD;
VOID PQL_ContextProc_inputInterrupt_RTK2885pp(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	unsigned int MEMC_RepeatMode = HAL_MC_GetRepeatMode();
	ReadRegister(HARDWARE_HARDWARE_63_reg,30,30, &u32_rb_data);

	if(s_pParam == NULL){
		return;
	}

	RTICEDebug_OnlineMeasure(0);

	if(u32_rb_data)
	{
		ReadComReg_Proc_inputInterrupt_RTK2885pp(&(s_pParam->_param_read_comreg), &(s_Context._output_read_comreg));

		FilmDetectCtrl_Proc(&(s_pParam->_param_filmDetectctrl), &(s_Context._output_filmDetectctrl));//60HZ
		FRC_phTable_Proc(&(s_pParam->_param_frc_phtable), &(s_Context._output_frc_phtable));
		MEMC_Lib_Style_Select();

		LogoCtrl_Proc_RTK2885pp(&(s_pParam->_param_logo_ctrl), &(s_Context._output_logo_ctrl));

		if(MEMC_RepeatMode==false){
			FRC_SceneAnalysis_Proc_InputIntp_RTK2885pp(&(s_pParam->_param_frc_sceneAnalysis), &(s_Context._output_frc_sceneAnalysis));
			LBMCSwitchCtrl_Proc_RTK2885pp(&(s_pParam->_param_mc_lbmcswitch), &(s_Context._output_mc_lbmcswitch));
		}

		SObjCtrl_Proc_RTK2885pp(&(s_pParam->_param_sobj_ctrl), &(s_Context._output_sobj_ctrl));
		FadeInOutCtrl_Proc_RTK2885pp(&(s_pParam->_param_fadeinout_ctrl), &(s_Context._output_fadeinout_ctrl));

		Wrt_ComReg_Proc_intputInterrupt_RTK2885pp(&(s_pParam->_param_wrt_comreg), &(s_Context._output_wrt_comreg));

		#if PQL_DYNAMIC_ADJUST_FIX_ISSUE
		// still pattern detect
		FRC_Still_Pattern_Detect(&(s_pParam->_param_filmDetectctrl), &(s_Context._output_filmDetectctrl), &(s_Context._output_frc_sceneAnalysis));
		#endif

		if (MEMC_Pixel_LOGO_For_SW_SLD == 2)
		{
			// MEMC_MotionInfo_SLD(&(s_Context._output_me_sceneAnalysis));
		}
		if(MEMC_RepeatMode==false){
			Identify_ctrl_Self_Check();
		}
	}
	
}
VOID PQL_ContextProc_inputInterrupt(int iSliceIdx)
{
	if(RUN_MERLIN8P()){
		PQL_ContextProc_inputInterrupt_RTK2885pp(iSliceIdx);
	}else if(RUN_MERLIN8()){
		PQL_ContextProc_inputInterrupt_RTK2885p(iSliceIdx);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

VOID PQL_Get_Basic_Info(void)
{
	// show basic info
	BasicInfoCheck_Show_Basic_Info();
	BasicInfoCheck_Show_APL_Info();
	BasicInfoCheck_Show_DTL_Info();
	BasicInfoCheck_Show_SAD_Info();
}

extern unsigned int g_HisDiff;
extern unsigned int g_HueDiff;
extern unsigned char MEMC_Lib_get_YHistBigChg_cnt(void);
VOID PQL_ContextProc_outputInterrupt_RTK2885p(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	_PQLPARAMETER *PatternParam = GetPQLParameter(); //for golf pattern
	unsigned int log_en = 0, time_1 = 0, time_2 = 0, time_3 = 0, time_4 = 0, time_5 = 0, time_6 = 0, time_7 = 0, time_8 = 0, time_9 = 0, time_10 = 0;
	unsigned int time_11 = 0, time_12 = 0, time_13 = 0, time_14 = 0;
	unsigned int MEMC_RepeatMode = HAL_MC_GetRepeatMode();
	ReadRegister(HARDWARE_HARDWARE_63_reg,31,31, &u32_rb_data);
	ReadRegister(SOFTWARE2_SOFTWARE2_04_reg, 31, 31, &log_en);

	if(s_pParam == NULL){
		return;
	}

	if(MEMC_RepeatMode==false){
		// manually adjust the CSC color matrix for small object
		MEMC_Lib_Manual_colormatrix_RTK2885p(&(s_Context._output_me_sceneAnalysis));
	}


	//Debug_OnlineMeasure();
		time_1 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
	RTICEDebug_OnlineMeasure(1);


	if(u32_rb_data)
	{
		time_2 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		if(PatternParam->_param_read_comreg.read_enable) {
			ReadComReg_Proc_outputInterrupt_RTK2885p(&(s_pParam->_param_read_comreg),&(s_Context._output_read_comreg));
		}
		BasicInfoCheck_Proc();
		AutoRegression_Proc();

		if(MEMC_RepeatMode==false){
			PQL_Get_Basic_Info();
			
#ifdef _WIN32
			PQL_ContextProc_oneFifth_outputInterrupt(0);
			PQL_ContextProc_inputInterrupt(0);
#endif

			time_3 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			PQL_MessageProc();
			time_4 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			Me_sceneAnalysis_Proc_OutputIntp_RTK2885p(&(s_pParam->_param_me_sceneAnalysis),&(s_Context._output_me_sceneAnalysis), iSliceIdx);
			time_5 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			FRC_SceneAnalysis_Proc_RTK2885p(&(s_pParam->_param_frc_sceneAnalysis), &(s_Context._output_frc_sceneAnalysis));

			//FRC_phTable_Proc(&(s_pParam->_param_frc_phtable), &(s_Context._output_frc_phtable));
			time_6 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			PatchManage_Proc_RTK2885p(&(s_pParam->_param_Patch_Manage), &(s_Context._output_Patch_Manage));
			#if IC_K5LP
			ME_Vst_Proc_RTK2885p(&(s_pParam->_param_me_vst_ctrl), &(s_Context._output_me_vst_ctrl));
			#endif

#if CONFIG_REPEAT_NEW_ALOG
			//o------ new periodic algo ------o
			Periodic_Proc_RTK2885p(&(s_pParam->_param_periodic_manage), &(s_Context._output_periodic_manage));
			PRD_Proc_RTK2885p(&(s_pParam->_param_prd_manage), &(s_Context._output_prd_manage), &(s_Context._output_periodic_manage));
#endif
			//o------ Merlin5 new add ------o
			time_7 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			MC_RMV_blending_RTK2885p(&(s_pParam->_param_bRMV_rFB_ctrl), &(s_Context._output_bRMV_rFB_ctrl));
			time_8 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			MC_ReginaolFB_RTK2885p(&(s_pParam->_param_bRMV_rFB_ctrl), &(s_Context._output_bRMV_rFB_ctrl));
			//o-----------------------------o
			time_9 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
			FRC_FastMotion_Detect(&(s_pParam->_param_frc_sceneAnalysis), &(s_Context._output_frc_sceneAnalysis)); //fastmotion, front of fb & dh_close
		}
		time_10 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		FBLevelCtrl_Proc_RTK2885p(&(s_pParam->_param_fblevelctrl),&(s_Context._output_fblevelctrl));

		time_11 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		if(MEMC_RepeatMode==false){
			Dh_close_Proc_RTK2885p(&(s_pParam->_param_dh_close), &(s_Context._output_dh_close));
		}

		time_12 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		Wrt_ComReg_Proc_outputInterrupt_RTK2885p(&(s_pParam->_param_wrt_comreg), &(s_Context._output_wrt_comreg));

		//*************************//
		//Debug Msg
		time_13 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
		Debug_Msg_Print(s_pParam, &s_Context);
		//*************************//
		
		if(MEMC_RepeatMode==false){
			BasicInfoCheck_SC_Check();
			BasicInfoCheck_RealPattern_check();
		}
		time_14 = rtd_inl(TIMER_SCPU_CLK90K_LO_reg);
#if CONFIG_REPEAT_NEW_ALOG		
		//o------ for regional_32 debug show ------o
		Region32_indicator(&(s_pParam->_param_prd_manage), &(s_Context._output_prd_manage), &(s_Context._output_periodic_manage));
#endif
	}
	if(log_en){
		rtd_pr_memc_notice("[%s][%d][APL,%d,][SAD,%d,][SAD_diff_th,%d,][TC,%d,][motion,%d,][hist,%d,%d,][YHistBigChg_cnt,%d,]\n\r", __FUNCTION__, __LINE__,
			rtd_inl(KME_ME1_TOP6_KME_ME1_TOP6_E4_reg), rtd_inl(KME_ME1_TOP6_KME_ME1_TOP6_D4_reg), rtd_inl(KME_ME1_TOP1_ME1_SCENE1_07_reg),
			rtd_inl(KME_ME1_TOP6_KME_ME1_TOP6_E0_reg), rtd_inl(KME_IPME_KME_IPME_A8_reg), g_HisDiff, g_HueDiff, MEMC_Lib_get_YHistBigChg_cnt());
//		rtd_pr_memc_notice("[%s][%d][APL,%d,][SAD,%d,][SAD_diff_th,%d,][TC,%d,][motion,%d,]\n\r", __FUNCTION__, __LINE__,
//			s_Context._output_read_comreg.u26_me_aAPLi_rb, s_Context._output_read_comreg.u30_me_aSAD_rb, rtd_inl(KME_ME1_TOP1_ME1_SCENE1_07_reg),
//			s_Context._output_read_comreg.u27_me_aTC_rb, s_Context._output_read_comreg.u27_ipme_aMot_rb);
	}
	if(log_en){
		unsigned int me1_pre_cnt = 0, me2_pre_cnt = 0, mc_pre_cnt = 0, dh_pre_cnt = 0, logo_pre_cnt = 0;
		unsigned int me1_hold_cnt = 0, me2_hold_cnt = 0, mc_hold_cnt = 0, dh_hold_cnt = 0, logo_hold_cnt = 0;
		unsigned int mc_sc_mode = 0, mc_sc_on = 0, mc_sc_fb = 0, mc_bld_bypass = 0, izpz_bld = 0, izpz_mode = 0, u32_RB_sc = 0;
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_01_reg, 0, 4, &me1_pre_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_01_reg, 5, 9, &me2_pre_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_01_reg, 10, 14, &mc_pre_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_01_reg, 15, 19, &dh_pre_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_01_reg, 20, 24, &logo_pre_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_02_reg, 0, 7, &me1_hold_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_02_reg, 8, 15, &me2_hold_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_02_reg, 16, 23, &mc_hold_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_02_reg, 24, 31, &dh_hold_cnt);
		ReadRegister(KME_ME1_TOP1_ME1_SCENE1_03_reg, 0, 7, &logo_hold_cnt);
		ReadRegister(MC_MC_30_reg, 8, 8, &mc_sc_mode);
		ReadRegister(MC_MC_30_reg, 9, 9, &mc_sc_on);
		ReadRegister(MC_MC_30_reg, 10, 10, &mc_sc_fb);
		ReadRegister(MC_MC_B0_reg, 0, 2, &mc_bld_bypass);
		ReadRegister(MC2_MC2_24_reg, 16, 16, &izpz_bld);
		ReadRegister(MC2_MC2_24_reg, 17, 19, &izpz_mode);
		ReadRegister(KME_ME1_TOP1_ME1_WLC1_00_reg, 0, 5, &u32_RB_sc);

		rtd_pr_memc_notice("[%s][%d][,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,][,%d,%d,%d,%d,%d,]\n\r", __FUNCTION__, __LINE__,
			me1_pre_cnt, me2_pre_cnt, mc_pre_cnt, dh_pre_cnt, logo_pre_cnt,
			me1_hold_cnt, me2_hold_cnt, mc_hold_cnt, dh_hold_cnt, logo_hold_cnt,
			(u32_RB_sc&0x1),(u32_RB_sc&0x2)>>1,(u32_RB_sc&0x4)>>2,(u32_RB_sc&0x8)>>3,(u32_RB_sc&0x10)>>4);
	}
}

VOID PQL_ContextProc_outputInterrupt_RTK2885pp(int iSliceIdx)
{
	unsigned int u32_rb_data = 0xFFFFFFFF;
	unsigned int log_en = 0;
	ReadRegister(HARDWARE_HARDWARE_63_reg,31,31, &u32_rb_data);
	ReadRegister(SOFTWARE2_SOFTWARE2_04_reg, 31, 31, &log_en);

	if(s_pParam == NULL){
		return;
	}

	// manually adjust the CSC color matrix for small object
	// MEMC_Lib_Manual_colormatrix(&(s_Context._output_me_sceneAnalysis));

	//Debug_OnlineMeasure();
	RTICEDebug_OnlineMeasure(1);

	if(u32_rb_data)
	{
		ReadComReg_Proc_outputInterrupt_RTK2885pp(&(s_pParam->_param_read_comreg),&(s_Context._output_read_comreg));

		//FIX_MISMATCH
		//BasicInfoCheck_Proc();
		//AutoRegression_Proc();
		//PQL_Get_Basic_Info();

	#ifdef _WIN32
		// PQL_ContextProc_oneFifth_outputInterrupt(0);
	#endif

		PQL_MessageProc();
		Me_sceneAnalysis_Proc_OutputIntp_RTK2885pp(&(s_pParam->_param_me_sceneAnalysis),&(s_Context._output_me_sceneAnalysis), iSliceIdx);
		FRC_SceneAnalysis_Proc_RTK2885pp(&(s_pParam->_param_frc_sceneAnalysis), &(s_Context._output_frc_sceneAnalysis));
		PatchCtrl_Proc_RTK2885pp(&(s_pParam->_param_patch_ctrl),&(s_Context._output_patch_ctrl));
		// PatchManage_Proc_RTK2885p(&(s_pParam->_param_Patch_Manage), &(s_Context._output_Patch_Manage));
		FlickerCtrl_Proc_RTK2885pp(&(s_pParam->_param_flicker_ctrl),&(s_Context._output_flicker_ctrl));
		PeriodicCtrl_Proc_RTK2885pp(&(s_pParam->_param_periodic_ctrl),&(s_Context._output_periodic_ctrl));

		FRC_FastMotion_Detect(&(s_pParam->_param_frc_sceneAnalysis), &(s_Context._output_frc_sceneAnalysis)); //fastmotion, front of fb & dh_close

		FBLevelCtrl_Proc_RTK2885pp(&(s_pParam->_param_fblevelctrl),&(s_Context._output_fblevelctrl));

		Wrt_ComReg_Proc_outputInterrupt_RTK2885pp(&(s_pParam->_param_wrt_comreg), &(s_Context._output_wrt_comreg));

		//FIX_MISMATCH
		//BasicInfoCheck_RealPattern_check();

		//*************************//
		//Debug Msg
		Debug_Msg_Print(s_pParam, &s_Context);
		//*************************//
	}

	if(log_en){
		rtd_pr_memc_notice("[%s][%d][APL,%d,][SAD,%d,][TC,%d,][motion,%d,][hist,%d,%d,][YHistBigChg_cnt,%d,]\n\r", __FUNCTION__, __LINE__,
			rtd_inl(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_A4_reg), rtd_inl(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_94_reg), 
			rtd_inl(M8P_RTME_HME1_TOP7_KME_ME1_TOP6_A0_reg), rtd_inl(M8P_RTME_IPHME_TOP0_IPHME_TOP_04_reg), g_HisDiff, g_HueDiff, MEMC_Lib_get_YHistBigChg_cnt());
	}
}

VOID PQL_ContextProc_outputInterrupt(int iSliceIdx)
{
	if(RUN_MERLIN8P()){
		PQL_ContextProc_outputInterrupt_RTK2885pp(iSliceIdx);
	}else if(RUN_MERLIN8()){
		PQL_ContextProc_outputInterrupt_RTK2885p(iSliceIdx);
	}//RUN_MERLIN8P & RUN_MERLIN8
}

CAD_SUPPORT_TYPE PQL_FRCCaseSupportJudge(unsigned char CurCadence)
{
	const _PQLCONTEXT     *s_pContext = GetPQLContext();

	unsigned char u1_func_en = s_pParam->_param_filmDetectctrl.u1_FrcCadSupport_En;

	PQL_OUTPUT_FRAME_RATE  u8_out_fmRate = s_pContext->_external_data._output_frameRate;
	PQL_INPUT_FRAME_RATE	u8_in_fmRate = s_pContext->_external_data._input_frameRate;


	if(!u1_func_en)
		return 1;

	if(CurCadence>=_FRC_CADENCE_NUM_)
		return 0;

	if(u8_in_fmRate>=_PQL_IN_96HZ)
		u8_in_fmRate = _PQL_IN_60HZ;

	if(u8_out_fmRate<=_PQL_OUT_60HZ)
		return g_FRCCADEN_60HZOUT_T[u8_in_fmRate][CurCadence];
	else if(u8_out_fmRate >= _PQL_OUT_200HZ){
		return g_FRCCADEN_240HZOUT_T[u8_in_fmRate][CurCadence];
	}
	else
		return g_FRCCADEN_120HZOUT_T[u8_in_fmRate][CurCadence];

}

///////////////////////////////////////////
const _PQLCONTEXT *GetPQLContext(void)
{
	return (const _PQLCONTEXT *)&s_Context;
}
 _PQLCONTEXT *GetPQLContext_m(void)
{
	return ( _PQLCONTEXT *)&s_Context;
}
 
#if 1//#if RTK_MEMC_Performance_tunging_from_tv001
BOOL GetStillFrameFlag(void)
{
	return s_Context._output_frc_sceneAnalysis.u1_still_frame;
}
#endif

 
////////////////////////////////////////////////////////////////////////////////////////////////////
enum _SERVERCOMMAND {SCMD_VOID,SCMD_RESET,SCMD_BASEADDR,SCMD_TARGETSIZE,
	SCMD_SETPARA,SCMD_GETPARA,SCMD_GETDATA};

//command mailbox registers
//#define _CMDREG_0 0x00004500
//#define _CMDREG_1 0x00004504
//#define _CMDREG_2 0x00004508
//#define _CMDREG_3 0x0000450C

#define _CMDREG_0 HARDWARE_HARDWARE_00_reg
#define _CMDREG_1 HARDWARE_HARDWARE_01_reg
#define _CMDREG_2 HARDWARE_HARDWARE_02_reg
#define _CMDREG_3 HARDWARE_HARDWARE_03_reg


VOID DebugCommandCheck(void)
{
	unsigned int uiCmd;
	static unsigned int suiCount = 0;
	suiCount++;
	WriteRegister(_CMDREG_3, 0, 31, suiCount);
	ReadRegister(_CMDREG_0,0,31,&uiCmd);

	if (uiCmd & 0xFF)
	{//if tx ready
		switch ((uiCmd>>16) & 0xFF)
		{
		case SCMD_RESET:

			break;

		case SCMD_TARGETSIZE:
			WriteRegister(_CMDREG_1,0,31,(unsigned int)sizeof(_PQLPARAMETER));
			WriteRegister(_CMDREG_2,0,31,(unsigned int)sizeof(_PQLCONTEXT));
			break;

		case SCMD_BASEADDR:
#ifndef CONFIG_ARM64 //ARM32 compatible
			WriteRegister(_CMDREG_1,0,31,(unsigned int)s_pParam);
			WriteRegister(_CMDREG_2,0,31,(unsigned int)&s_Context);
#endif //FIX_ME_INCORRECT_MEMORY_ADDRESS_USE
			break;

		case SCMD_GETPARA:
			{
#ifndef CONFIG_ARM64 //ARM32 compatible
				unsigned int uiAddr;
				//unsigned int uiData0 = 0;
				unsigned int uiData = 0;
				int i;
				ReadRegister(_CMDREG_1,0,31,&uiAddr);
				/*if (uiAddr >= (unsigned int)s_pParam && uiAddr < (unsigned int)s_pParam+sizeof(_PQLPARAMETER))
				{
				   uiData0 = *(volatile unsigned long *)uiAddr;
				}
 				uiData = 0;
 				for (i = 3; i >= 0; i--) {
 					uiData <<= 8;
 					uiData += (uiData0&0xFF);
					uiData0 >>= 8;
 				}*/
 				uiData = 0;
				for (i = 3; i >= 0; i--) {
					uiData <<= 8;
					uiData += *(volatile unsigned char *)(uiAddr+i);
				}
				WriteRegister(_CMDREG_2,0,31,uiData);
				//LogPrintf(DBG_MSG, "status address: \n",uiAddr);
				//LogPrintf(DBG_MSG, "status val: \n",uiData);
#endif //FIX_ME_INCORRECT_MEMORY_ADDRESS_USE
			}
		    break;

		case SCMD_GETDATA:
			{
#ifndef CONFIG_ARM64 //ARM32 compatible
				unsigned int uiAddr;
				//unsigned int uiData0 = 0;
				unsigned int uiData = 0;
                int i;
				ReadRegister(_CMDREG_1,0,31,&uiAddr);
				/*if (uiAddr >= (unsigned int)&s_Context && uiAddr < (unsigned int)&s_Context+sizeof(_PQLCONTEXT))
				{
					uiData0 = *(volatile unsigned long *)uiAddr;
				}
 				uiData = 0;
 				for (i = 3; i >= 0; i--) {
 					uiData <<= 8;
 					uiData += (uiData0&0xFF);
					uiData0 >>= 8;
 				}*/
 				uiData = 0;
				for (i = 3; i >= 0; i--) {
					uiData <<= 8;
					uiData += *(volatile unsigned char *)(uiAddr+i);
				}

				WriteRegister(_CMDREG_2,0,31,uiData);
				//LogPrintf(DBG_MSG, "status address: \n",uiAddr);
				//LogPrintf(DBG_MSG, "status val: \n",uiData);
#endif //FIX_ME_INCORRECT_MEMORY_ADDRESS_USE
			}
			break;
		case SCMD_SETPARA:
			{
#ifndef CONFIG_ARM64 //ARM32 compatible
				unsigned int uiAddr;
				unsigned int uiData;
				unsigned int uiShiftData;
				unsigned int uiPackData0;

				ReadRegister(_CMDREG_1,0,31,&uiAddr);
				ReadRegister(_CMDREG_2,0,31,&uiData);

                uiPackData0 = *(volatile unsigned long *)(uiAddr&0xFFFFFFFC);
				//ReadRegister(uiAddr&0xFFFFFFFC,0,31,&uiPackData0);

				switch (uiCmd>>24) {
				case 1: {
					uiShiftData = uiData&0xFF;
					switch (uiAddr&0x3) {
					case 3:
						uiPackData0 &= 0xFFFFFF00;
						uiPackData0 |= uiShiftData;
						break;
					case 2:
						uiPackData0 &= 0xFFFF00FF;
						uiPackData0 |= (uiShiftData<<8);
						break;
					case 1:
						uiPackData0 &= 0xFF00FFFF;
						uiPackData0 |= (uiShiftData<<16);
						break;
					case 0:
						uiPackData0 &= 0xFFFFFF;
						uiPackData0 |= (uiShiftData<<24);
						break;
					}
						}
						break;
				case 2: {
					uiShiftData = ((uiData&0xFF)<<8)|((uiData>>8)&0xFF);
					switch (uiAddr&0x3) {
					case 2:
						uiPackData0 &= 0xFFFF0000;
						uiPackData0 |= uiShiftData;
						break;
					case 0:
						uiPackData0 &= 0xFFFF;
						uiPackData0 |= (uiShiftData<<16);
						break;
					}
						}
						break;
				case 4: {
					uiShiftData = ((uiData&0xFF)<<24)|((uiData&0xFF00)<<8)|((uiData
						&0xFF0000)>>8)|((uiData&0xFF000000)>>24);
					uiPackData0 = uiShiftData;
						}
						break;
				}
				//LogPrintf(DBG_MSG, "para address: \n",uiAddr);
				//LogPrintf(DBG_MSG, "para size: \n",uiCmd>>24);
				//LogPrintf(DBG_MSG, "para val: \n",uiPackData0);
			//	WriteRegister((uiAddr&0xFFFFFFFC),0,31,uiPackData0);

				uiAddr &= 0xFFFFFFFC;
				if (uiAddr >= (unsigned int)s_pParam && uiAddr < (unsigned int)s_pParam+sizeof(_PQLPARAMETER))
				{//protection
				   //WriteRegister((uiAddr&0xFFFFFFFC),0,31,uiPackData0);
				   *(volatile unsigned long *)uiAddr = uiPackData0;
				}
#endif //FIX_ME_INCORRECT_MEMORY_ADDRESS_USE
			}
			break;
		}//switch case

		//acknowledgement
		uiCmd &= 0xFF0000;
		uiCmd |= 0x0100;
		WriteRegister(_CMDREG_0,0,31,uiCmd);
	}

	WriteRegister(HARDWARE_HARDWARE_04_reg, 0, 31, suiCount);
}

#if REMOVE_REDUNDANT_API
BOOL PQL_GetMEMCActSafeStatus(VOID)
{
	static unsigned int blending_cnt = 0;
	unsigned int blending_mode = 0;
	ReadRegister(MC_MC_B0_reg,0,2, &blending_mode);

	if (blending_mode == 0)
	{
		if (blending_cnt > 9)
			blending_cnt = 9;
		else
			blending_cnt++;
	}
	else
	{
		blending_cnt = 0;
	}

	if(blending_cnt == 9)
		return TRUE;
	else
		return FALSE;
}
#endif

VOID Debug_OnlineMeasure(void)
{
	M8P_rthw_lbmc_lbmc_i_lf_get_num_RBUS M8P_rthw_lbmc_lbmc_i_lf_get_num_reg;
	M8P_rthw_lbmc_lbmc_p_lf_get_num_RBUS M8P_rthw_lbmc_lbmc_p_lf_get_num_reg;
	M8P_rthw_lbmc_lbmc_i_hf_get_num_RBUS M8P_rthw_lbmc_lbmc_i_hf_get_num_reg;
	M8P_rthw_lbmc_lbmc_p_hf_get_num_RBUS M8P_rthw_lbmc_lbmc_p_hf_get_num_reg;
	M8P_rthw_ippre_top_ippre1_b0_RBUS M8P_rthw_ippre_top_ippre1_b0;
	M8P_rthw_ippre_top_ippre1_bc_RBUS M8P_rthw_ippre_top_ippre1_bc;
	M8P_rthw_ippre_top_ippre1_b8_RBUS M8P_rthw_ippre_top_ippre1_b8;
	M8P_rthw_post_top_kpost_top_04_RBUS M8P_rthw_post_top_kpost_top_04;
	M8P_rthw_post_top_kpost_top_bc_RBUS M8P_rthw_post_top_kpost_top_bc;
	M8P_rthw_post_top_kpost_top_b8_RBUS M8P_rthw_post_top_kpost_top_b8;
	M8P_rthw_me_dma_dm0_wr_status_RBUS M8P_rthw_me_dma_dm0_wr_status;
	M8P_rthw_me_dma_dm1_wr_status_RBUS M8P_rthw_me_dma_dm1_wr_status;
	M8P_rthw_me_dma_dm2_wr_status_RBUS M8P_rthw_me_dma_dm2_wr_status;
	M8P_rthw_me_dma_dm3_rd_status_RBUS M8P_rthw_me_dma_dm3_rd_status;
	M8P_rthw_me_dma_dm4_rd_status_RBUS M8P_rthw_me_dma_dm4_rd_status;
	M8P_rthw_me_dma_dm5_rd_status_RBUS M8P_rthw_me_dma_dm5_rd_status;
	M8P_rthw_me_dma_dm6_rd_status_RBUS M8P_rthw_me_dma_dm6_rd_status;
	M8P_rthw_me_dma_dm7_rd_status_RBUS M8P_rthw_me_dma_dm7_rd_status;

	unsigned int u32_En = 0;
	unsigned int u32_En_pre = 0;

	unsigned int u32_InVtotal = 0,  u32_OutVtotal = 0;
	unsigned int u32_InHtotal = 0,  u32_OutHtotal = 0;
	unsigned int u32_InVact = 0,  u32_OutVact = 0;
	unsigned int u32_InHact = 0,  u32_OutHact = 0;
	unsigned char u8_ErrorFlag = 0;
	unsigned int u32_IzLFData = 0, u32_IzLFLine = 0, u32_IzHFData = 0, u32_IzHFLine = 0;
	unsigned int u32_PzLFData = 0, u32_PzLFLine = 0, u32_PzHFData = 0, u32_PzHFLine = 0;

	static unsigned int pre_InVtotal = 0;
	static unsigned int pre_OutVtotal = 0;
	static unsigned int pre_InHtotal = 0;
	static unsigned int pre_OutHtotal = 0;
	static unsigned int pre_InVact = 0;
	static unsigned int pre_OutVact = 0;
	static unsigned int pre_InHact = 0;
	static unsigned int pre_OutHact = 0;
	static unsigned int pre_IzLFData = 0;
	static unsigned int pre_IzLFLine = 0;
	static unsigned int pre_IzHFData = 0;
	static unsigned int pre_IzHFLine = 0;
	static unsigned int pre_PzLFData = 0;
	static unsigned int pre_PzLFLine = 0;
	static unsigned int pre_PzHFData = 0;
	static unsigned int pre_PzHFLine = 0;

	unsigned int masterDTG_dh_total, masterDTG_dv_total, uzuDTG_dh_total, uzuDTG_dv_total;
	unsigned int uzuDTG_dh_den_sta, uzuDTG_dh_den_end, uzuDTG_dv_den_sta, uzuDTG_dv_den_end;
	unsigned int memcDTG_dh_total, memcDTG_dv_total;
	unsigned int memc_dh_den_sta, memc_dh_den_end, memc_dv_den_sta, memc_dv_den_end;

	static unsigned int pre_masterDTG_dh_total = 0;
	static unsigned int pre_masterDTG_dv_total = 0;
	static unsigned int pre_uzuDTG_dh_total = 0;
	static unsigned int pre_uzuDTG_dv_total = 0;
	static unsigned int pre_uzuDTG_dh_den_sta = 0;
	static unsigned int pre_uzuDTG_dh_den_end = 0;
	static unsigned int pre_uzuDTG_dv_den_sta = 0;
	static unsigned int pre_uzuDTG_dv_den_end = 0;
	static unsigned int pre_memcDTG_dh_total = 0;
	static unsigned int pre_memcDTG_dv_total = 0;
	static unsigned int pre_memc_dh_den_sta = 0;
	static unsigned int pre_memc_dh_den_end = 0;
	static unsigned int pre_memc_dv_den_sta = 0;
	static unsigned int pre_memc_dv_den_end = 0;

	unsigned int kme_overflow_error_0, kme_overflow_error_4, kme_overflow_error_5, kme_overflow_error_8, kme_overflow_error_10, kme_overflow_error_12, kme_overflow_error_14;
	unsigned int dm0_uf, dm1_uf, dm2_uf, dm3_of, dm4_of, dm5_of, dm6_of, dm7_of;
// MEMC_K6L_BRING_UP //	unsigned int kmc_00_wram_err, kmc_01_wram_err;

	static unsigned int FrameCnt = 0;
	static unsigned int FirstFrame = 1;//first readback data is wrong, ignore it

	u32_En = 1;
	if(RUN_MERLIN8P()){
		M8P_rthw_ippre_top_ippre1_b0.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_B0_reg);
		M8P_rthw_post_top_kpost_top_04.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_04_reg);
		u32_En_pre = M8P_rthw_ippre_top_ippre1_b0.kmc_in_measure_en;
	}else if(RUN_MERLIN8()){
		ReadRegister(KMC_TOP_kmc_top_18_reg,6,6, &u32_En_pre);
	}//RUN_MERLIN8P & RUN_MERLIN8

	//if(u32_En_pre != u32_En) //fix me
	{
		if(RUN_MERLIN8P()){
			M8P_rthw_ippre_top_ippre1_b0.kmc_in_measure_en = u32_En;
			M8P_rthw_post_top_kpost_top_04.post_out_measure_en = u32_En;

			M8P_rthw_ippre_top_ippre1_b0.kmc_in_measure_sel = 0x7;
			M8P_rthw_post_top_kpost_top_04.post_out_measure_sel = 0x7;

			M8P_rthw_ippre_top_ippre1_b0.kmc_int_pulse = 1; //WriteRegister(KMC_TOP_kmc_top_18_reg, 7, 18, 0x121);
			M8P_rthw_ippre_top_ippre1_b0.res1 = 0x90; //WriteRegister(KMC_TOP_kmc_top_18_reg, 7, 18, 0x121);
			M8P_rthw_post_top_kpost_top_04.post_out_vtotal_int = 0x121;
			rtd_outl(M8P_RTHW_IPPRE_TOP_IPPRE1_B0_reg, M8P_rthw_ippre_top_ippre1_b0.regValue);
			rtd_outl(M8P_RTHW_POST_TOP_KPOST_TOP_04_reg, M8P_rthw_post_top_kpost_top_04.regValue);
		}else if(RUN_MERLIN8()){
			WriteRegister(KMC_TOP_kmc_top_18_reg, 6, 6, u32_En);
			WriteRegister(KPOST_TOP_KPOST_TOP_04_reg, 19, 19, u32_En);

			WriteRegister(KMC_TOP_kmc_top_18_reg, 3, 5, 0x7);
			WriteRegister(KPOST_TOP_KPOST_TOP_04_reg, 16, 18, 0x7);

			WriteRegister(KMC_TOP_kmc_top_18_reg, 7, 18, 0x121);
			WriteRegister(KPOST_TOP_KPOST_TOP_04_reg, 20, 31, 0x121);
		}//RUN_MERLIN8P & RUN_MERLIN8
	}

	if(u32_En == 1)
	{
	//[MEMC_OnlineMeasure]--------
		if(RUN_MERLIN8P()){
			M8P_rthw_ippre_top_ippre1_bc.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_BC_reg);
			M8P_rthw_ippre_top_ippre1_b8.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_B8_reg);
			M8P_rthw_post_top_kpost_top_bc.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_BC_reg);
			M8P_rthw_post_top_kpost_top_b8.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_B8_reg);

			u32_InVtotal = M8P_rthw_ippre_top_ippre1_bc.regr_kmc_in_vtot_measure;
			u32_InHtotal = M8P_rthw_ippre_top_ippre1_bc.regr_kmc_in_htot_measure;
			u32_InVact = M8P_rthw_ippre_top_ippre1_b8.regr_kmc_in_vact_measure;
			u32_InHact = M8P_rthw_ippre_top_ippre1_b8.regr_kmc_in_hact_measure;

			u32_OutVtotal = M8P_rthw_post_top_kpost_top_bc.regr_post_out_vtot_measure;
			u32_OutHtotal = M8P_rthw_post_top_kpost_top_bc.regr_post_out_htot_measure;
			u32_OutVact = M8P_rthw_post_top_kpost_top_b8.regr_post_out_vact_measure;
			u32_OutHact = M8P_rthw_post_top_kpost_top_b8.regr_post_out_hact_measure;

			M8P_rthw_lbmc_lbmc_i_lf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_I_LF_GET_NUM_reg);
			M8P_rthw_lbmc_lbmc_p_lf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_P_LF_GET_NUM_reg);
			M8P_rthw_lbmc_lbmc_i_hf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_I_HF_GET_NUM_reg);
			M8P_rthw_lbmc_lbmc_p_hf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_P_HF_GET_NUM_reg);

			u32_IzLFData = M8P_rthw_lbmc_lbmc_i_lf_get_num_reg.regr_lbmc_i_lf_get_data_num;
			u32_IzLFLine = M8P_rthw_lbmc_lbmc_i_lf_get_num_reg.regr_lbmc_i_lf_get_line_num;
			u32_IzHFData = M8P_rthw_lbmc_lbmc_p_lf_get_num_reg.regr_lbmc_p_lf_get_data_num;
			u32_IzHFLine = M8P_rthw_lbmc_lbmc_p_lf_get_num_reg.regr_lbmc_p_lf_get_line_num;
			u32_PzLFData = M8P_rthw_lbmc_lbmc_i_hf_get_num_reg.regr_lbmc_i_hf_get_data_num;
			u32_PzLFLine = M8P_rthw_lbmc_lbmc_i_hf_get_num_reg.regr_lbmc_i_hf_get_line_num;
			u32_PzHFData = M8P_rthw_lbmc_lbmc_p_hf_get_num_reg.regr_lbmc_p_hf_get_data_num;
			u32_PzHFLine = M8P_rthw_lbmc_lbmc_p_hf_get_num_reg.regr_lbmc_p_hf_get_line_num;
		}else if(RUN_MERLIN8()){
			ReadRegister(KMC_TOP_kmc_top_F8_reg,14,26, &u32_InVtotal);
			ReadRegister(KMC_TOP_kmc_top_F8_reg,0,13, &u32_InHtotal);
			ReadRegister(KMC_TOP_kmc_top_F4_reg,16,28, &u32_InVact);
			ReadRegister(KMC_TOP_kmc_top_F4_reg,0,12, &u32_InHact);

			ReadRegister(KPOST_TOP_KPOST_TOP_F0_reg,14,26, &u32_OutVtotal);
			ReadRegister(KPOST_TOP_KPOST_TOP_F0_reg,0,13, &u32_OutHtotal);
			ReadRegister(KPOST_TOP_KPOST_TOP_EC_reg,13,25, &u32_OutVact);
			ReadRegister(KPOST_TOP_KPOST_TOP_EC_reg,0,12, &u32_OutHact);

			ReadRegister(LBMC_LBMC_A0_reg,0,15, &u32_IzLFData);
			ReadRegister(LBMC_LBMC_A0_reg,16,31, &u32_IzLFLine);
			ReadRegister(LBMC_LBMC_A8_reg,0,15, &u32_IzHFData);
			ReadRegister(LBMC_LBMC_A8_reg,16,31, &u32_IzHFLine);
			ReadRegister(LBMC_LBMC_A4_reg,0,15, &u32_PzLFData);
			ReadRegister(LBMC_LBMC_A4_reg,16,31, &u32_PzLFLine);
			ReadRegister(LBMC_LBMC_AC_reg,0,15, &u32_PzHFData);
			ReadRegister(LBMC_LBMC_AC_reg,16,31, &u32_PzHFLine);
		}//RUN_MERLIN8P & RUN_MERLIN8

	//[MEMC_dtg]-------------
		masterDTG_dh_total = (rtd_inl(PPOVERLAY_DH_Total_Last_Line_Length_reg)>>16)&0x1FFF;
		masterDTG_dv_total = (rtd_inl(PPOVERLAY_DV_total_reg)&0x1FFF);
		uzuDTG_dh_total = (rtd_inl(PPOVERLAY_uzudtg_DH_TOTAL_reg)&0x1FFF);
		uzuDTG_dv_total = (rtd_inl(PPOVERLAY_uzudtg_DV_TOTAL_reg)&0x1FFF);

		uzuDTG_dh_den_sta = (rtd_inl(PPOVERLAY_DH_DEN_Start_End_reg)>>16)&0x1FFF;
		uzuDTG_dh_den_end = (rtd_inl(PPOVERLAY_DH_DEN_Start_End_reg)&0x1FFF);
		uzuDTG_dv_den_sta = (rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg)>>16)&0x1FFF;
		uzuDTG_dv_den_end = (rtd_inl(PPOVERLAY_DV_DEN_Start_End_reg)&0x1FFF);

		memcDTG_dh_total = (rtd_inl(PPOVERLAY_memcdtg_DH_TOTAL_reg)&0x1FFF);
		memcDTG_dv_total = (rtd_inl(PPOVERLAY_memcdtg_DV_TOTAL_reg)&0x1FFF);
		memc_dh_den_sta = (rtd_inl(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg)>>16)&0x1FFF;
		memc_dh_den_end = (rtd_inl(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg)&0x1FFF);
		memc_dv_den_sta =  (rtd_inl(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg)>>16)&0x1FFF;
		memc_dv_den_end = (rtd_inl(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg)&0x1FFF);

	//[MEMC_overflow]-------------
	//DMA_CHECK
		if(RUN_MERLIN8P()){
			M8P_rthw_me_dma_dm0_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_status_reg);
			M8P_rthw_me_dma_dm1_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_status_reg);
			M8P_rthw_me_dma_dm2_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_status_reg);
			M8P_rthw_me_dma_dm3_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_status_reg);
			M8P_rthw_me_dma_dm4_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_status_reg);
			M8P_rthw_me_dma_dm5_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_status_reg);
			M8P_rthw_me_dma_dm6_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_status_reg);
			M8P_rthw_me_dma_dm7_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_status_reg);
			dm0_uf = M8P_rthw_me_dma_dm0_wr_status.data_fifo_underflow_err;
			dm1_uf = M8P_rthw_me_dma_dm1_wr_status.data_fifo_underflow_err;
			dm2_uf = M8P_rthw_me_dma_dm2_wr_status.data_fifo_underflow_err;
			dm3_of = M8P_rthw_me_dma_dm3_rd_status.data_fifo_overflow_err;
			dm4_of = M8P_rthw_me_dma_dm4_rd_status.data_fifo_overflow_err;
			dm5_of = M8P_rthw_me_dma_dm5_rd_status.data_fifo_overflow_err;
			dm6_of = M8P_rthw_me_dma_dm6_rd_status.data_fifo_overflow_err;
			dm7_of = M8P_rthw_me_dma_dm7_rd_status.data_fifo_overflow_err;
		}else if(RUN_MERLIN8()){
			ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,25,25, &kme_overflow_error_0);
			ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,26,26, &kme_overflow_error_4);
			ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,27,27, &kme_overflow_error_5);
			ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,28,28, &kme_overflow_error_8);
			ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,29,29, &kme_overflow_error_10);
			ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,30,30, &kme_overflow_error_12);
			ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,31,31, &kme_overflow_error_14);
		}//RUN_MERLIN8P & RUN_MERLIN8
// MEMC_K6L_BRING_UP //		ReadRegister(0xb80994fc,6,6, &kmc_00_wram_err);
// MEMC_K6L_BRING_UP //		ReadRegister(0xb80994fc,7,7, &kmc_01_wram_err);

	}

	if(u32_En == 1 && FirstFrame == 0)
	{
	//[MEMC_OnlineMeasure]--------
		if((u32_InVtotal <= 0x8b9 || u32_InVtotal>=0x8d9) && (pre_InVtotal != u32_InVtotal))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_InVtotal = %d -> %d, MC_InFrameCnt = %d \r\n", pre_InVtotal, u32_InVtotal, FrameCnt);
			u8_ErrorFlag += (1<<0);
		}
		if((u32_OutVtotal <= 0x8b9 || u32_OutVtotal>=0x8d9) && ( pre_OutVtotal != u32_OutVtotal))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_OutVtotal = %d -> %d, MC_OutFrameCnt = %d \r\n", pre_OutVtotal, u32_OutVtotal, FrameCnt);
			u8_ErrorFlag += (1<<1);
		}
		if((u32_InHtotal <= 0x888 || u32_InHtotal>=0x8a8) && ( pre_InHtotal != u32_InHtotal))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_InHtotal = %d -> %d, MC_InFrameCnt = %d \r\n", pre_InHtotal, u32_InHtotal, FrameCnt);
			u8_ErrorFlag += (1<<2);
		}
		if((u32_OutHtotal <= 0x43c || u32_OutHtotal>=0x45c) && ( pre_OutHtotal != u32_OutHtotal))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_OutHtotal = %d -> %d, MC_OutFrameCnt = %d \r\n", pre_OutHtotal, u32_OutHtotal, FrameCnt);
			u8_ErrorFlag += (1<<3);
		}
		if((u32_InVact <= 0x860 ||u32_InVact>=0x880) && ( pre_InVact != u32_InVact))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_InVact = %d, %d -> MC_InFrameCnt = %d \r\n", pre_InVact, u32_InVact, FrameCnt);
			u8_ErrorFlag += (1<<4);
		}
		if((u32_OutVact <= 0x860 || u32_OutVact>= 0x880) && ( pre_OutVact != u32_OutVact))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_OutVact = %d -> %d, MC_OutFrameCnt = %d \r\n", pre_OutVact, u32_OutVact, FrameCnt);
			u8_ErrorFlag += (1<<5);
		}
		if((u32_InHact <= 0x770 || u32_InHact>=0x790) && ( pre_InHact != u32_InHact))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_InHact = %d -> %d, MC_InFrameCnt = %d \r\n", pre_InHact, u32_InHact, FrameCnt);
			u8_ErrorFlag += (1<<6);
		}
		if((u32_OutHact <= 0x3b0 || u32_OutHact>= 0x3d0) && (pre_OutHact != u32_OutHact))
		{
			rtd_pr_memc_debug("[MEMC_OnlineMeasure] MC_OutHact = %d -> %d, MC_OutFrameCnt = %d \r\n", pre_OutHact, u32_OutHact, FrameCnt);
			u8_ErrorFlag += (1<<7);
		}

		if(u8_ErrorFlag > 0)
		{
			rtd_pr_memc_debug("[MEMC Online Measure] IzLFDataNum = %d , IzLFLineNum = %d , IzHFDataNum = %d , IzHFLineNum = %d, PzLFDataNum = %d , PzLFLineNum = %d , PzHFDataNum = %d , PzHFLineNum = %d",
				u32_IzLFData, u32_IzLFLine, u32_IzHFData, u32_IzHFLine, u32_PzLFData, u32_PzLFLine, u32_PzHFData, u32_PzHFLine);
		}


		{
			pre_InVtotal = u32_InVtotal;
			pre_OutVtotal = u32_OutVtotal ;
			pre_InHtotal = u32_InHtotal;
			pre_OutHtotal = u32_OutHtotal;
			pre_InVact = u32_InVact;
			pre_OutVact = u32_OutVact;
			pre_InHact = u32_InHact;
			pre_OutHact = u32_OutHact;
			pre_IzLFData = u32_IzLFData;
			pre_IzLFLine = u32_IzLFLine;
			pre_IzHFData = u32_IzHFData;
			pre_IzHFLine = u32_IzHFLine;
			pre_PzLFData = u32_PzLFData;
			pre_PzLFLine = u32_PzLFLine;
			pre_PzHFData = u32_PzHFData;
			pre_PzHFLine = u32_PzHFLine;
		}

	//[MEMC_dtg]-------------
		if((masterDTG_dh_total<=4383 || masterDTG_dh_total>=4415) && (pre_masterDTG_dh_total != masterDTG_dh_total))
		{
			rtd_pr_memc_debug("[MEMC_dtg] masterDTG_dh_total = %d -> %d, FrameCnt = %d \r\n", pre_masterDTG_dh_total, masterDTG_dh_total, FrameCnt);
		}

		if((masterDTG_dv_total<=2233 || masterDTG_dv_total>=2265) && (pre_masterDTG_dv_total != masterDTG_dv_total))
		{
			rtd_pr_memc_debug("[MEMC_dtg] masterDTG_dv_total = %d -> %d, FrameCnt = %d \r\n", pre_masterDTG_dv_total, masterDTG_dv_total, FrameCnt);
		}

		if((uzuDTG_dh_total<=4383 || uzuDTG_dh_total>=4415) && (pre_uzuDTG_dh_total != uzuDTG_dh_total))
		{
			rtd_pr_memc_debug("[MEMC_dtg] uzuDTG_dh_total = %d -> %d, FrameCnt = %d \r\n", pre_uzuDTG_dh_total, uzuDTG_dh_total, FrameCnt);
		}

		if((uzuDTG_dv_total<=2233 || uzuDTG_dv_total>=2265) && (pre_uzuDTG_dv_total != uzuDTG_dv_total))
		{
			rtd_pr_memc_debug("[MEMC_dtg] uzuDTG_dv_total = %d -> %d, FrameCnt = %d \r\n", pre_uzuDTG_dv_total, uzuDTG_dv_total, FrameCnt);
		}


		if(((uzuDTG_dh_den_end - uzuDTG_dh_den_sta)<=3824 || (uzuDTG_dh_den_end - uzuDTG_dh_den_sta)>=3856) && ((pre_uzuDTG_dh_den_end - pre_uzuDTG_dh_den_sta) != (uzuDTG_dh_den_end - uzuDTG_dh_den_sta)))
		{
			rtd_pr_memc_debug("[MEMC_dtg] uzuDTG_dh_den_sta = %d -> %d, uzuDTG_dh_den_end = %d -> %d, end-sta = %d, FrameCnt = %d \r\n", pre_uzuDTG_dh_den_sta, uzuDTG_dh_den_sta, pre_uzuDTG_dh_den_end, uzuDTG_dh_den_end, uzuDTG_dh_den_end - uzuDTG_dh_den_sta, FrameCnt);
		}


		if(((uzuDTG_dv_den_end-uzuDTG_dv_den_sta)<=2144 || (uzuDTG_dv_den_end-uzuDTG_dv_den_sta)>=2176) && ((pre_uzuDTG_dv_den_end-pre_uzuDTG_dv_den_sta) != (uzuDTG_dv_den_end-uzuDTG_dv_den_sta)))
		{
			rtd_pr_memc_debug("[MEMC_dtg] uzuDTG_dv_den_sta = %d -> %d, uzuDTG_dv_den_end = %d -> %d, end-sta = %d, FrameCnt = %d \r\n", pre_uzuDTG_dv_den_sta, uzuDTG_dv_den_sta, pre_uzuDTG_dv_den_end, uzuDTG_dv_den_end, uzuDTG_dv_den_end-uzuDTG_dv_den_sta, FrameCnt);
		}

		if((memcDTG_dh_total<=4383 || memcDTG_dh_total>=4415) && (pre_memcDTG_dh_total != memcDTG_dh_total))
		{
			rtd_pr_memc_debug("[MEMC_dtg] memcDTG_dh_total = %d -> %d, FrameCnt = %d \r\n", pre_memcDTG_dh_total, memcDTG_dh_total, FrameCnt);
		}


		if((memcDTG_dv_total<=2233 || memcDTG_dv_total>=2265) && (pre_memcDTG_dv_total != memcDTG_dv_total))
		{
			rtd_pr_memc_debug("[MEMC_dtg] memcDTG_dv_total = %d -> %d, FrameCnt = %d \r\n", pre_memcDTG_dv_total, memcDTG_dv_total, FrameCnt);
		}

		if(((memc_dh_den_end-memc_dh_den_sta)<=3824 || (memc_dh_den_end-memc_dh_den_sta)>=3856) && ((pre_memc_dh_den_end-pre_memc_dh_den_sta) != (memc_dh_den_end-memc_dh_den_sta)))
		{
			rtd_pr_memc_debug("[MEMC_dtg] memc_dh_den_sta = %d -> %d, memc_dh_den_end = %d -> %d, end-sta = %d, FrameCnt = %d \r\n", pre_memc_dh_den_sta, memc_dh_den_sta, pre_memc_dh_den_end, memc_dh_den_end, (memc_dh_den_end-memc_dh_den_sta), FrameCnt);
		}

		if(((memc_dv_den_end-memc_dv_den_sta)<=2144 || (memc_dv_den_end-memc_dv_den_sta)>=2176) && ((pre_memc_dv_den_end-pre_memc_dv_den_sta) != (memc_dv_den_end-memc_dv_den_sta)))
		{
			rtd_pr_memc_debug("[MEMC_dtg] memc_dv_den_sta = %d -> %d, memc_dv_den_end = %d -> %d, end-sta = %d, FrameCnt = %d \r\n", pre_memc_dv_den_sta, memc_dv_den_sta, pre_memc_dv_den_end, memc_dv_den_end, memc_dv_den_end-memc_dv_den_sta, FrameCnt);
		}

		{
			pre_masterDTG_dh_total = masterDTG_dh_total;
			pre_masterDTG_dv_total = masterDTG_dv_total;
			pre_uzuDTG_dh_total = uzuDTG_dh_total;
			pre_uzuDTG_dv_total = uzuDTG_dv_total;
			pre_uzuDTG_dh_den_sta = uzuDTG_dh_den_sta;
			pre_uzuDTG_dh_den_end = uzuDTG_dh_den_end;
			pre_uzuDTG_dv_den_sta = uzuDTG_dv_den_sta;
			pre_uzuDTG_dv_den_end = uzuDTG_dv_den_end;
			pre_memcDTG_dh_total = memcDTG_dh_total;
			pre_memcDTG_dv_total = memcDTG_dv_total;
			pre_memc_dh_den_sta = memc_dh_den_sta;
			pre_memc_dh_den_end = memc_dh_den_end;
			pre_memc_dv_den_sta = memc_dv_den_sta;
			pre_memc_dv_den_end = memc_dv_den_end;
		}

	//[MEMC_overflow]-------------
#if 0//TBD
		static BOOL wram_err_flag = 0;
		unsigned int me1_ip_dummy0_29=0;
		ReadRegister(KME_ME1_TOP0_KME_ME1_TOP0_30_reg,29,29,&me1_ip_dummy0_29);

		if(me1_ip_dummy0_29)
		{
			if(wram_err_flag)
			{
				WriteRegister(0xb80994fc, 6, 7, 1);
				WriteRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg, 25, 31, 1);
				wram_err_flag = 0;
			}

			if((kme_overflow_error_0 ||kme_overflow_error_4 ||kme_overflow_error_5 ||
				kme_overflow_error_8 ||kme_overflow_error_10 ||kme_overflow_error_12 ||
				kme_overflow_error_14 ||kmc_00_wram_err || kmc_01_wram_err) && !wram_err_flag)
			{
				extern void smooth_toggle_set_screen_freeze(void);
				smooth_toggle_set_screen_freeze();
				wram_err_flag = 1;
				WriteRegister(0xb80994fc, 6, 7, 0);
				WriteRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg, 25, 31, 0);
			}

		}
		else
		{
			wram_err_flag = 0;
		}

		if(me1_ip_dummy0_29)
		{
			if(u8_ErrorFlag)
			{
				extern void smooth_toggle_set_screen_freeze(void);
				smooth_toggle_set_screen_freeze();
			}
		}
		else
		{
			u8_ErrorFlag = 0;
		}
#endif

		FrameCnt++;
	}
	else if (u32_En == 1 && FirstFrame == 1)
	{
		FirstFrame = 0;
		FrameCnt++;
	}
	else
	{
		FirstFrame = 1;
		FrameCnt = 0;
	}
}


VOID RTICEDebug_OnlineMeasure(unsigned char int_type)
{
	/*
		_BIT31: dump log mode (0: each isr, 1: diff pre value)
		_BIT30 ~ _BIT24: (value*4) of log print timer
		_BIT23: m domain blk print

		// DUMP control
		_BIT18: KMC_fail
		_BIT17: KMC_headerfail
		_BIT16: KMV_overflow_error
		_BIT15: KME_overflow_error
		_BIT14: SUB Active Start/End
		_BIT13: SUB Background Start/ENd
		_BIT12: SUB DEN Start/End
		_BIT11: MAIN Active Start/End
		_BIT10: MAIN Background Start/End
		_BIT09: MAIN DEN Start/End
		_BIT08: LBMC HF num
		_BIT07: LBMC LF num
		_BIT06: KPost Out Size
		_BIT05: KMC In Size
		_BIT04: MEMC DEN Start/End
		_BIT03: MEMC DTG Size
		_BIT02: UZU DEN Start/End
		_BIT01: UZU DTG Size
		_BIT00: Master DTG Size
	*/
	M8P_rthw_ippre_top_ippre1_b8_RBUS M8P_rthw_ippre_top_ippre1_b8;
	M8P_rthw_ippre_top_ippre1_bc_RBUS M8P_rthw_ippre_top_ippre1_bc;
	M8P_rthw_post_top_kpost_top_b8_RBUS M8P_rthw_post_top_kpost_top_b8;
	M8P_rthw_post_top_kpost_top_bc_RBUS M8P_rthw_post_top_kpost_top_bc;
	M8P_rthw_lbmc_lbmc_i_lf_get_num_RBUS M8P_rthw_lbmc_lbmc_i_lf_get_num_reg;
	M8P_rthw_lbmc_lbmc_p_lf_get_num_RBUS M8P_rthw_lbmc_lbmc_p_lf_get_num_reg;
	M8P_rthw_lbmc_lbmc_i_hf_get_num_RBUS M8P_rthw_lbmc_lbmc_i_hf_get_num_reg;
	M8P_rthw_lbmc_lbmc_p_hf_get_num_RBUS M8P_rthw_lbmc_lbmc_p_hf_get_num_reg;

	unsigned int u32_reg_value = 0;
	unsigned char u8_dump_mode = 0;
	static unsigned short u16_dump_timer = 0;
	unsigned char u1_dump_mdomain_blk = 0;
	static unsigned short blk_repeated_cnt = 0, blk_dropped_cnt = 0;

	unsigned char u1_bit00_en = 0;
	unsigned char u1_bit01_en = 0;
	unsigned char u1_bit02_en = 0;
	unsigned char u1_bit03_en = 0;
	unsigned char u1_bit04_en = 0;
	unsigned char u1_bit05_en = 0;
	unsigned char u1_bit06_en = 0;
	unsigned char u1_bit07_en = 0;
	unsigned char u1_bit08_en = 0;
	unsigned char u1_bit09_en = 0;
	unsigned char u1_bit10_en = 0;
	unsigned char u1_bit11_en = 0;
	unsigned char u1_bit12_en = 0;
	unsigned char u1_bit13_en = 0;
	unsigned char u1_bit14_en = 0;
	unsigned char u1_bit15_en = 0;
	unsigned char u1_bit16_en = 0;
	unsigned char u1_bit17_en = 0;
	unsigned char u1_bit18_en = 0;

	if(RUN_MERLIN8P()){
		ReadRegister(RTICE_DEBUG_REG, 0, 31, &u32_reg_value);
	}else if(RUN_MERLIN8()){
		ReadRegister(KME_ME1_TOP0_KME_ME1_TOP0_30_reg, 0, 31, &u32_reg_value);
	}//RUN_MERLIN8P & RUN_MERLIN8

	u8_dump_mode = u32_reg_value>>31;
	if (u16_dump_timer == 0)
	{
		u16_dump_timer = ((u32_reg_value>>24) & 0x7f) * 32;
		if(RUN_MERLIN8P()){
			WriteRegister(RTICE_DEBUG_REG, 24, 30, 0);
		}else if(RUN_MERLIN8()){
			WriteRegister(KME_ME1_TOP0_KME_ME1_TOP0_30_reg, 24, 30, 0);
		}//RUN_MERLIN8P & RUN_MERLIN8

		blk_repeated_cnt = 0;
		blk_dropped_cnt = 0;
	}
	u1_dump_mdomain_blk = (u32_reg_value>>23) & 0x1;

	u1_bit00_en = (u32_reg_value>>0) & 0x00000001;
	u1_bit01_en = (u32_reg_value>>1) & 0x00000001;
	u1_bit02_en = (u32_reg_value>>2) & 0x00000001;
	u1_bit03_en = (u32_reg_value>>3) & 0x00000001;
	u1_bit04_en = (u32_reg_value>>4) & 0x00000001;
	u1_bit05_en = (u32_reg_value>>5) & 0x00000001;
	u1_bit06_en = (u32_reg_value>>6) & 0x00000001;
	u1_bit07_en = (u32_reg_value>>7) & 0x00000001;
	u1_bit08_en = (u32_reg_value>>8) & 0x00000001;
	u1_bit09_en = (u32_reg_value>>9) & 0x00000001;
	u1_bit10_en = (u32_reg_value>>10) & 0x00000001;
	u1_bit11_en = (u32_reg_value>>11) & 0x00000001;
	u1_bit12_en = (u32_reg_value>>12) & 0x00000001;
	u1_bit13_en = (u32_reg_value>>13) & 0x00000001;
	u1_bit14_en = (u32_reg_value>>14) & 0x00000001;
	u1_bit15_en = (u32_reg_value>>15) & 0x00000001;
	u1_bit16_en = (u32_reg_value>>16) & 0x00000001;
	u1_bit17_en = (u32_reg_value>>17) & 0x00000001;
	u1_bit18_en = (u32_reg_value>>18) & 0x00000001;

	if (u16_dump_timer > 0)
	{
		u16_dump_timer--;

		if (int_type == 0) // input interrupt domain
		{
#if 0 //for merlin8/8p one-bin image, mark the mdomain access
			if (u1_dump_mdomain_blk)
			{
				unsigned int value, blk_value;
				static unsigned char pre_blk_value, max_blk_value;

				RTKReadRegister(MDOMAIN_DISP_DDR_MainStatus_reg, &value);
				blk_value = ((value>>4)&0x3);
				//rtd_pr_memc_debug("[M-domain blk]: %d\r\n", blk_value);

				if (blk_value > max_blk_value)
				{
					max_blk_value = blk_value;
				}

				if (pre_blk_value == blk_value)
				{
					rtd_pr_memc_debug("[M-domain blk]: repeated %d->%d\r\n", pre_blk_value, blk_value);
					blk_repeated_cnt++;
				}

				if ((pre_blk_value!=max_blk_value && (blk_value-pre_blk_value)!=1) || (pre_blk_value==max_blk_value && blk_value!=0))
				{
					rtd_pr_memc_debug("[M-domain blk]: dropped %d->%d\r\n", pre_blk_value, blk_value);
					blk_dropped_cnt++;
				}

				pre_blk_value = blk_value;

				if (u16_dump_timer == 0)
				{
					rtd_pr_memc_debug("[M-domain blk]: %d repeated\r\n", blk_repeated_cnt);
					rtd_pr_memc_debug("[M-domain blk]: %d dropped\r\n", blk_dropped_cnt);
				}
			}
#endif

			if (u1_bit00_en) // Master DTG Size
			{
				unsigned int value=0, tmp=0, tmp2=0, tmp3=0;

				RTKReadRegister(PPOVERLAY_DH_Total_Last_Line_Length_reg, &value);
				tmp = (value>>16) & 0x1fff;
				tmp2 = value & 0x1fff;
				RTKReadRegister(PPOVERLAY_DV_total_reg, &value);
				tmp3 = value & 0x1fff;

				rtd_pr_memc_debug("[MEMC][Master DTG Size] DH/DH_last_line/DV total: %d, %d, %d\r\n", tmp, tmp2, tmp3);
			}

			if (u1_bit01_en) // UZU DTG Size
			{
				unsigned int value=0, tmp=0, tmp2=0, tmp3=0;

				RTKReadRegister(PPOVERLAY_uzudtg_DH_TOTAL_reg, &value);
				tmp = value & 0x1fff;
				tmp2 = (value>>16) & 0x1fff;
				RTKReadRegister(PPOVERLAY_uzudtg_DV_TOTAL_reg, &value);
				tmp3 = value & 0x1fff;

				rtd_pr_memc_debug("[MEMC][UZU DTG Size] DH/DH_last_line/DV total: %d, %d, %d\r\n", tmp, tmp2, tmp3);
			}

			if (u1_bit02_en) // UZU DEN Start/End
			{
				unsigned int value=0, tmp=0, tmp2=0, tmp3=0, tmp4=0;

				RTKReadRegister(PPOVERLAY_DH_DEN_Start_End_reg, &value);
				tmp = (value>>16) & 0x1fff;
				tmp2 = value & 0x1fff;
				RTKReadRegister(PPOVERLAY_DV_DEN_Start_End_reg, &value);
				tmp3 = (value>>16) & 0x1fff;
				tmp4 = value & 0x1fff;

				rtd_pr_memc_debug("[MEMC][UZU DEN] DH start/end: %d, %d, %d\r\n", tmp, tmp2, (tmp2-tmp));
				rtd_pr_memc_debug("[MEMC][UZU DEN] DV start/end: %d, %d, %d\r\n", tmp3, tmp4, (tmp4-tmp3));
			}

			if (u1_bit03_en) // MEMC DTG Size
			{
				unsigned int value=0, tmp=0, tmp2=0, tmp3=0;

				RTKReadRegister(PPOVERLAY_memcdtg_DH_TOTAL_reg, &value);
				tmp = value & 0x1fff;
				tmp2 = (value>>16) & 0x1fff;
				RTKReadRegister(PPOVERLAY_memcdtg_DV_TOTAL_reg, &value);
				tmp3 = value & 0x1fff;

				rtd_pr_memc_debug("[MEMC][MEMC DTG Size] DH/DH_last_line/DV total: %d, %d, %d\r\n", tmp, tmp2, tmp3);
			}

			if (u1_bit04_en) // MEMC DEN Start/End
			{
				unsigned int value=0, tmp=0, tmp2=0, tmp3=0, tmp4=0;

				RTKReadRegister(PPOVERLAY_memcdtg_DH_DEN_Start_End_reg, &value);
				tmp = (value>>16) & 0x1fff;
				tmp2 = value & 0x1fff;
				RTKReadRegister(PPOVERLAY_memcdtg_DV_DEN_Start_End_reg, &value);
				tmp3 = (value>>16) & 0x1fff;
				tmp4 = value & 0x1fff;

				rtd_pr_memc_debug("[MEMC][MEMC DEN] DH start/end: %d, %d, %d\r\n", tmp, tmp2, (tmp2-tmp));
				rtd_pr_memc_debug("[MEMC][MEMC DEN] DV start/end: %d, %d, %d\r\n", tmp3, tmp4, (tmp4-tmp3));
			}

			if (u1_bit05_en) // KMC In Size
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0;

				if(RUN_MERLIN8P()){
					M8P_rthw_ippre_top_ippre1_b8.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_B8_reg);
					M8P_rthw_ippre_top_ippre1_bc.regValue = rtd_inl(M8P_RTHW_IPPRE_TOP_IPPRE1_BC_reg);
					tmp = M8P_rthw_ippre_top_ippre1_b8.regr_kmc_in_hact_measure;
					tmp2 = M8P_rthw_ippre_top_ippre1_b8.regr_kmc_in_vact_measure;
					tmp3 = M8P_rthw_ippre_top_ippre1_bc.regr_kmc_in_htot_measure;
					tmp4 = M8P_rthw_ippre_top_ippre1_bc.regr_kmc_in_vtot_measure;
				}else if(RUN_MERLIN8()){
					ReadRegister(KMC_TOP_kmc_top_F4_reg,0,12, &tmp);
					ReadRegister(KMC_TOP_kmc_top_F4_reg,16,28, &tmp2);
					ReadRegister(KMC_TOP_kmc_top_F8_reg,0,13, &tmp3);
					ReadRegister(KMC_TOP_kmc_top_F8_reg,14,26, &tmp4);
				}//RUN_MERLIN8P & RUN_MERLIN8

				rtd_pr_memc_debug("[MEMC][KMC In Size] hact/vact/htot/vtot %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4);
			}
		}
		else // oututp interrupt domain
		{
			if (u1_bit06_en) // KPost Out Size
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0;

				if(RUN_MERLIN8P()){
					M8P_rthw_post_top_kpost_top_b8.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_B8_reg);
					M8P_rthw_post_top_kpost_top_bc.regValue = rtd_inl(M8P_RTHW_POST_TOP_KPOST_TOP_BC_reg);
					tmp = M8P_rthw_post_top_kpost_top_b8.regr_post_out_hact_measure;
					tmp2 = M8P_rthw_post_top_kpost_top_b8.regr_post_out_vact_measure;
					tmp3 = M8P_rthw_post_top_kpost_top_bc.regr_post_out_htot_measure;
					tmp4 = M8P_rthw_post_top_kpost_top_bc.regr_post_out_vtot_measure;
				}else if(RUN_MERLIN8()){
					ReadRegister(KPOST_TOP_KPOST_TOP_EC_reg,0,12, &tmp);
					ReadRegister(KPOST_TOP_KPOST_TOP_EC_reg,13,25, &tmp2);
					ReadRegister(KPOST_TOP_KPOST_TOP_F0_reg,0,13, &tmp3);
					ReadRegister(KPOST_TOP_KPOST_TOP_F0_reg,14,26, &tmp4);
				}//RUN_MERLIN8P & RUN_MERLIN8

				rtd_pr_memc_debug("[MEMC][KPost Out Size] hact/vact/htot/vtot %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4);
			}

			if (u1_bit07_en) // LBMC LF num
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0;

				if(RUN_MERLIN8P()){
					M8P_rthw_lbmc_lbmc_i_lf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_I_LF_GET_NUM_reg);
					M8P_rthw_lbmc_lbmc_p_lf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_P_LF_GET_NUM_reg);
					tmp = M8P_rthw_lbmc_lbmc_i_lf_get_num_reg.regr_lbmc_i_lf_get_data_num;
					tmp2 = M8P_rthw_lbmc_lbmc_i_lf_get_num_reg.regr_lbmc_i_lf_get_line_num;
					tmp3 = M8P_rthw_lbmc_lbmc_p_lf_get_num_reg.regr_lbmc_p_lf_get_data_num;
					tmp4 = M8P_rthw_lbmc_lbmc_p_lf_get_num_reg.regr_lbmc_p_lf_get_line_num;
				}else if(RUN_MERLIN8()){
					ReadRegister(LBMC_LBMC_A0_reg,0,15, &tmp);
					ReadRegister(LBMC_LBMC_A0_reg,16,31, &tmp2);
					ReadRegister(LBMC_LBMC_A4_reg,0,15, &tmp3);
					ReadRegister(LBMC_LBMC_A4_reg,16,31, &tmp4);
				}//RUN_MERLIN8P & RUN_MERLIN8

				rtd_pr_memc_debug("[MEMC][LBMC LF num] i_data/i_line/p_data/p_line: %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4);
			}

			if (u1_bit08_en) // LBMC HF num
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0;

				if(RUN_MERLIN8P()){
					M8P_rthw_lbmc_lbmc_i_hf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_I_HF_GET_NUM_reg);
					M8P_rthw_lbmc_lbmc_p_hf_get_num_reg.regValue = rtd_inl(M8P_RTHW_LBMC_LBMC_P_HF_GET_NUM_reg);
					tmp = M8P_rthw_lbmc_lbmc_i_hf_get_num_reg.regr_lbmc_i_hf_get_data_num;
					tmp2 = M8P_rthw_lbmc_lbmc_i_hf_get_num_reg.regr_lbmc_i_hf_get_line_num;
					tmp3 = M8P_rthw_lbmc_lbmc_p_hf_get_num_reg.regr_lbmc_p_hf_get_data_num;
					tmp4 = M8P_rthw_lbmc_lbmc_p_hf_get_num_reg.regr_lbmc_p_hf_get_line_num;
				}else if(RUN_MERLIN8()){
					ReadRegister(LBMC_LBMC_A8_reg,0,15, &tmp);
					ReadRegister(LBMC_LBMC_A8_reg,16,31, &tmp2);
					ReadRegister(LBMC_LBMC_AC_reg,0,15, &tmp3);
					ReadRegister(LBMC_LBMC_AC_reg,16,31, &tmp4);
				}//RUN_MERLIN8P & RUN_MERLIN8

				rtd_pr_memc_debug("[MEMC][LBMC HF num] i_data/i_line/p_data/p_line: %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4);
			}

			if (u1_bit09_en) // MAIN DEN Start/End
			{
			}
			if (u1_bit10_en) // MAIN Background Start/End
			{
			}
			if (u1_bit11_en) // MAIN Active Start/End
			{
			}
			if (u1_bit12_en) //  SUB DEN Start/End
			{
			}
			if (u1_bit13_en) // SUB Background Start/ENd
			{
			}
			if (u1_bit14_en) // SUB Active Start/End
			{
			}

			if (u1_bit15_en) // KME_overflow_error
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0, tmp5=0, tmp6=0, tmp7=0;
				M8P_rthw_me_dma_dm0_wr_status_RBUS M8P_rthw_me_dma_dm0_wr_status;
				M8P_rthw_me_dma_dm1_wr_status_RBUS M8P_rthw_me_dma_dm1_wr_status;
				M8P_rthw_me_dma_dm2_wr_status_RBUS M8P_rthw_me_dma_dm2_wr_status;
				M8P_rthw_me_dma_dm3_rd_status_RBUS M8P_rthw_me_dma_dm3_rd_status;
				M8P_rthw_me_dma_dm4_rd_status_RBUS M8P_rthw_me_dma_dm4_rd_status;
				M8P_rthw_me_dma_dm5_rd_status_RBUS M8P_rthw_me_dma_dm5_rd_status;
				M8P_rthw_me_dma_dm6_rd_status_RBUS M8P_rthw_me_dma_dm6_rd_status;
				M8P_rthw_me_dma_dm7_rd_status_RBUS M8P_rthw_me_dma_dm7_rd_status;
				
				if(RUN_MERLIN8P()){
					M8P_rthw_me_dma_dm0_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM0_WR_status_reg);
					M8P_rthw_me_dma_dm1_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM1_WR_status_reg);
					M8P_rthw_me_dma_dm2_wr_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM2_WR_status_reg);
					M8P_rthw_me_dma_dm3_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM3_RD_status_reg);
					M8P_rthw_me_dma_dm4_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM4_RD_status_reg);
					M8P_rthw_me_dma_dm5_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM5_RD_status_reg);
					M8P_rthw_me_dma_dm6_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM6_RD_status_reg);
					M8P_rthw_me_dma_dm7_rd_status.regValue = rtd_inl(M8P_RTHW_ME_DMA_DM7_RD_status_reg);

					rtd_pr_memc_debug("[MEMC][overflow] %d, %d, %d, %d, %d, %d, %d, %d\r\n", 
						M8P_rthw_me_dma_dm0_wr_status.data_fifo_underflow_err,
						M8P_rthw_me_dma_dm1_wr_status.data_fifo_underflow_err,
						M8P_rthw_me_dma_dm2_wr_status.data_fifo_underflow_err,
						M8P_rthw_me_dma_dm3_rd_status.data_fifo_overflow_err,
						M8P_rthw_me_dma_dm4_rd_status.data_fifo_overflow_err,
						M8P_rthw_me_dma_dm5_rd_status.data_fifo_overflow_err,
						M8P_rthw_me_dma_dm6_rd_status.data_fifo_overflow_err,
						M8P_rthw_me_dma_dm7_rd_status.data_fifo_overflow_err);
				}else if(RUN_MERLIN8()){
					ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,25,25, &tmp);
					ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,26,26, &tmp2);
					ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,27,27, &tmp3);
					ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,28,28, &tmp4);
					ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,29,29, &tmp5);
					ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,30,30, &tmp6);
					ReadRegister(KME_DM_TOP0_KME_DM_ERROR_FLAG_reg,31,31, &tmp7);

					rtd_pr_memc_debug("[MEMC][KME overflow] 00/04/05/08/10/12/14: %d, %d, %d, %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7);
				}//RUN_MERLIN8P & RUN_MERLIN8
			}

			if (u1_bit16_en) // KMV_overflow_error
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0, tmp5=0;

				//MERLIN8P_MV_RELATE
				//DMA_CHECK
				if(RUN_MERLIN8P()){
					//show by u1_bit15_en
				}else if(RUN_MERLIN8()){
					ReadRegister(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg,19,19, &tmp);
					ReadRegister(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg,20,20, &tmp2);
					ReadRegister(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg,21,21, &tmp3);
					ReadRegister(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg,22,22, &tmp4);
// MEMC_K6L_BRING_UP //				ReadRegister(KME_DM_TOP2_MV_DM_ERROR_FLAG_reg,23,23, &tmp5);

					rtd_pr_memc_debug("[MEMC][KMV overflow] 01/02/04/05/12: %d, %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4, tmp5);
				}//RUN_MERLIN8P & RUN_MERLIN8
			}

			if (u1_bit17_en) // KMC_headerfail
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0, tmp5=0, tmp6=0, tmp7=0, tmp8=0;

// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,10,10, &tmp);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,11,11, &tmp2);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,12,12, &tmp3);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,13,13, &tmp4);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,14,14, &tmp5);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,15,15, &tmp6);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,16,16, &tmp7);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,17,17, &tmp8);

				rtd_pr_memc_debug("[MEMC][KMC headerfail] 0~8: %d, %d, %d, %d, %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8);
			}

			if (u1_bit18_en) // KMC_fail
			{
				unsigned int tmp=0, tmp2=0, tmp3=0, tmp4=0, tmp5=0, tmp6=0, tmp7=0, tmp8=0;

// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,18,18, &tmp);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,19,19, &tmp2);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,20,20, &tmp3);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,21,21, &tmp4);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,22,22, &tmp5);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,23,23, &tmp6);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,24,24, &tmp7);
// MEMC_K6L_BRING_UP //				ReadRegister(0xb80994fc,25,25, &tmp8);

				rtd_pr_memc_debug("[MEMC][KMC fail] 0~8: %d, %d, %d, %d, %d, %d, %d, %d\r\n", tmp, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8);
			}
		}


	}

}

//#endif
