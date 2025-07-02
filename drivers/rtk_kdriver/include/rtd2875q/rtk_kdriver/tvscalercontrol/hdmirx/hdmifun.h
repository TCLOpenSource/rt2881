#ifndef _HDMI_FUN_H
#define _HDMI_FUN_H

#include <ioctrl/scaler/vfe_cmd_id.h> // for vfe_hdmi_vsi_t

#include <rtk_kdriver/measure/rtk_measure.h>
/**********************************************************************************************
*
*   Marco or Definitions
*
**********************************************************************************************/

#define NEWBASE_FUNC //need to check
#define HDR10_MD_INFO_CHECK     1
//#define HDMI_MEASURE_I3DDMA_SMART_FIT 1
#define HDMI_PI_ERR_DET 1

#define USE_DEFAULT_HDCP14      0 //Enable HDCP temp tey for bring up

#define HDMI_WD_VIDEO_COND      _BIT0
#define HDMI_WD_AUDIO_COND      _BIT1
#define HDMI_WD_VSC_COND        _BIT2
#define HDMI_WD_ALL_COND        (HDMI_WD_VIDEO_COND | HDMI_WD_AUDIO_COND | HDMI_WD_VSC_COND)



/**********************************************************************************************
*
*   Enumerations or Structures
*
**********************************************************************************************/

typedef enum _HDMI_TABLE_TYPE {
    HDMI_CHANNEL0,
    HDMI_CHANNEL1,
    HDMI_CHANNEL2,
    HDMI_CHANNEL3,
    HDMI_CHANNEL,
    HDMI_HDCP_KEY_TABLE,
    CBUS_TABLE,
} HDMI_TABLE_TYPE;

//typedef void (*HDMI_MUX_ADVANCED_FUNCTION)(unsigned char index, unsigned int Value_1, unsigned int Value_2, unsigned int Value_3);


typedef  unsigned char HDMI_bool;
#if 0
typedef enum {
    MODE_DVI    = 0x0,    // DVI mode
    MODE_HDMI   = 0x1,    // HDMI 1.4/2.0
    MODE_HDMI21 = 0x2,    // HDMI 2.1
    MODE_UNKNOW = 0x3,
} HDMI_DVI_MODE_T;
#endif
typedef enum {
    MODE_RAG_DEFAULT    = 0x0,
    MODE_RAG_LIMIT,
    MODE_RAG_FULL,
    MODE_RAG_UNKNOW
} HDMI_RGB_YUV_RANGE_MODE_T;
#if 0
typedef enum {
    COLOR_RGB   = 0x00,
    COLOR_YUV422,
    COLOR_YUV444,
    COLOR_YUV420,
    COLOR_UNKNOW
} HDMI_COLOR_SPACE_T;

typedef enum {

    HDMI_COLOR_DEPTH_8B = 0,
    HDMI_COLOR_DEPTH_10B,
    HDMI_COLOR_DEPTH_12B,
    HDMI_COLOR_DEPTH_16B,

} HDMI_COLOR_DEPTH_T;
#endif


typedef enum _HDMI_OSD_MODE {
    HDMI_OSD_MODE_AUTO = 0,
    HDMI_OSD_MODE_HDMI,
    HDMI_OSD_MODE_DVI,
    HDMI_OSD_MODE_NO_SETTING
} HDMI_OSD_MODE;

#if 0
typedef enum {
    HDMI_COLORIMETRY_NOSPECIFIED = 0,
    HDMI_COLORIMETRY_601,
    HDMI_COLORIMETRY_709,
    HDMI_COLORIMETRY_XYYCC601,
    HDMI_COLORIMETRY_XYYCC709,
    HDMI_COLORIMETRY_SYCC601,
    HDMI_COLORIMETRY_ADOBE_YCC601,
    HDMI_COLORIMETRY_ADOBE_RGB,
} HDMI_COLORIMETRY_T;
#endif

typedef enum {
    HDMI_PCB_ARC_EARC_UNKNOW = -3,
    HDMI_PCB_ARC_EARC_MULTI = -2,
    HDMI_PCB_ARC_EARC_NONE = -1,
    HDMI_PCB_ARC_EARC_PORT0 = 0,
    HDMI_PCB_ARC_EARC_PORT1 = 1,
    HDMI_PCB_ARC_EARC_PORT2 = 2,
    HDMI_PCB_ARC_EARC_PORT3 = 3,
} HDMI_PCB_ARC_EARC_T;

typedef enum {
    HDMI_VTEM_NEXT_TFR_QMS_OFF = 0,  //QMS not active
    HDMI_VTEM_NEXT_TFR_23_976,       // 24/1.001
    HDMI_VTEM_NEXT_TFR_24,
    HDMI_VTEM_NEXT_TFR_25,
    HDMI_VTEM_NEXT_TFR_29_97,        // 30/1.001
    HDMI_VTEM_NEXT_TFR_30,
    HDMI_VTEM_NEXT_TFR_47_952,       // 48/1.001
    HDMI_VTEM_NEXT_TFR_48,
    HDMI_VTEM_NEXT_TFR_50,
    HDMI_VTEM_NEXT_TFR_59_94,        // 60/1.001
    HDMI_VTEM_NEXT_TFR_60,
    HDMI_VTEM_NEXT_TFR_100,
    HDMI_VTEM_NEXT_TFR_119_88,       // 120/1.001
    HDMI_VTEM_NEXT_TFR_120,
} HDMI_VTEM_NEXT_TFR;

#if 0

typedef struct {
    unsigned int v_total;
    unsigned int h_total;
    unsigned int v_act_len;
    unsigned int h_act_len;
    unsigned int v_act_sta;
    unsigned int h_act_sta;
    unsigned int v_freq;
    unsigned int h_freq;
    unsigned int polarity;
    unsigned int mode_id;
    unsigned int modetable_index;
    unsigned char is_interlace;
    unsigned int IVSyncPulseCount;  //Input VSYNC High Period Measurement Result
    unsigned int IHSyncPulseCount;  //Input HSYNC High Period Measurement Result

    HDMI_DVI_MODE_T     mode;
    HDMI_COLOR_DEPTH_T colordepth;
    HDMI_COLOR_SPACE_T colorspace;
    HDMI_COLORIMETRY_T  colorimetry;
    unsigned char pixel_repeat;

    HDMI_HVF_E hvf;
    HDMI_3D_T h3dformat;

    unsigned int tmds_clk_b;
    unsigned long pll_pixelclockx1024;

    // only for fram packing
    unsigned int v_active_space1;
    unsigned int v_active_space2;
    unsigned char avi_vic;
    unsigned char run_vrr;
    unsigned char run_dsc;    // 0: do not run dsc flow, 1: run dsc flow
    unsigned char fva_factor;    // 0: fva disabled, > 0: fva multiplier - 1
} MEASURE_TIMING_T;




typedef struct  {
    char *name;
    int progressive;
    unsigned int h_act_len;
    unsigned int v_act_len;
    unsigned int lr_v_act_len;
    unsigned int v_active_space1;
    unsigned int v_active_space2;
} HDMI_ACTIVE_SPACE_TABLE_T;
#endif


typedef struct {
    unsigned char   VSIF_TypeCode;
    unsigned char   VSIF_Version;
    unsigned char   Length;
    unsigned char   Checksum;
    unsigned char   Reg_ID[HAL_VFE_HDMI_VENDOR_SPECIFIC_REGID_LEN];
    unsigned char   Payload[HAL_VFE_HDMI_VENDOR_SPECIFIC_PAYLOAD_LEN];
} HDMI_VSI_T;

typedef struct {
    unsigned char type_code;
    unsigned char ver;
    unsigned char len;
    unsigned int    S: 2;
    unsigned int    B: 2;
    unsigned int    A: 1;
    unsigned int    Y: 3;
    unsigned int    R: 4;
    unsigned int    M: 2;
    unsigned int    C: 2;
    unsigned int    SC: 2;
    unsigned int    Q: 2;
    unsigned int    EC: 3;
    unsigned int    ITC: 1;
    unsigned int    VIC: 8;
    unsigned int    PR: 4;
    unsigned int    CN: 2;
    unsigned int    YQ: 2;
    unsigned char   ETB07_00;
    unsigned char   ETB15_08;
    unsigned char   SBB07_00;
    unsigned char   SBB15_08;
    unsigned char   ELB07_00;
    unsigned char   ELB15_08;
    unsigned char   SRB07_00;
    unsigned char   SRB15_08;
    unsigned char   F14: 4;   //byte14 [3:0]
    unsigned char   ACE: 4;   //byte14 [7:4]
} HDMI_AVI_T;

typedef struct {
    unsigned char   type_code;
    unsigned char   ver;
    unsigned char   len;
    unsigned char   VendorName[8];
    unsigned char   ProductDesc[16];
    unsigned char   SourceInfo;
} HDMI_SPD_T;

typedef struct {
    unsigned char type_code;
    unsigned char ver;
    unsigned char len;
    unsigned int    CC: 3;
    unsigned int    F13: 1;
    unsigned int    CT: 4;
    unsigned int    SS: 2;
    unsigned int    SF: 3;
    unsigned int    F27_25: 3;
    unsigned int    CXT: 5;
    unsigned int    F37_35: 3;
    unsigned int    CA: 8;
    unsigned int    LFEPBL: 2;
    unsigned int    F52: 1;
    unsigned int    LSV: 4;
    unsigned int    DM_INH: 1;
    unsigned char   F67_60;
    unsigned char   F77_70;
    unsigned char   F87_80;
    unsigned char   F97_90;
    unsigned char   F107_100;
} HDMI_AUDIO_T;

typedef struct {
    unsigned char type_code;
    unsigned char ver;
    unsigned char len;
    unsigned char  eEOTFtype;
    unsigned char  eMeta_Desc;
    unsigned short display_primaries_x0;
    unsigned short display_primaries_y0;
    unsigned short display_primaries_x1;
    unsigned short display_primaries_y1;
    unsigned short display_primaries_x2;
    unsigned short display_primaries_y2;
    unsigned short white_point_x;
    unsigned short white_point_y;
    unsigned short max_display_mastering_luminance;
    unsigned short min_display_mastering_luminance;
    unsigned short maximum_content_light_level;
    unsigned short maximum_frame_average_light_level;
    unsigned char  reserved;
} __attribute__((packed)) HDMI_DRM_T;

typedef struct {
    unsigned char   type_code;          //HB0[7:0]
    unsigned char   First;              //HB1[7]
    unsigned char   Last;               //HB1[6]
    unsigned char   Sequence_Index; //HB2[7:0]
    unsigned char   New;                //PB0[7]
    unsigned char   End;                //PB0[6]
    unsigned char   DS_Type;            //PB0[5:4]
    unsigned char   AFR;                //PB0[3]
    unsigned char   VFR;                //PB0[2]
    unsigned char   Sync;               //PB0[1]
    unsigned char   Organization_ID;    //PB2[7:0]
    unsigned short  Data_Set_Tag;       //PB3[7:0]:MSB, PB4[7:0]:LSB
    unsigned short  Data_Set_Length;    //PB5[7:0]:MSB, PB6[7:0]:LSB
    unsigned char   MD[273];            //size = 21 + (Sequence_Index - 1)*28
    //Max will have 7133 byte
    //if Sequence_Index = 0, First = 1, PB27[7:0] PB7[7:0], MD[20] ~ MD[0]
    //if First = 0, PB27[7:0] PB0[7:0], MD[27] ~ MD[0]
    //The 273 is short time patch, need to discuss
} __attribute__((packed)) HDMI_EM_T;

typedef struct {
    unsigned short   Extended_InfoFrame;                //Data_Set_Tag
    unsigned char    Extended_InfoFrame_Data[21];       //MD0 ~ MD20
    unsigned char    reserved;
} __attribute__((packed)) HDMI_EM_HDR_DYNAMIC_T;

typedef struct {
    unsigned char    FVA_Factor_M1;     //MD0[7:4]
    unsigned char    QMS_EN;            //MD0[2]
    unsigned char    M_CONST;           //MD0[1]
    unsigned char    VRR_EN;            //MD0[0]
    unsigned char    Base_Vfront;       //MD1[7:0]
    unsigned char    Next_TFR;          //MD2[7:3]
    unsigned char    RB;                //MD2[2]
    unsigned short   Base_Refresh_Rate; //MD2[1:0] ~ MD3[7:0]
    unsigned char    reserved;
} __attribute__((packed)) HDMI_EM_VTEM_T;

typedef struct {
    unsigned char    SBTM_ver;         //MD0[3:0]
    unsigned char    SBTM_mode;     //MD1[1:0]
    unsigned char    SBTM_type;       //MD1[3:2]
    unsigned char    GRDM_min;       //MD1[5:4]
    unsigned char    GRDM_lum;       //MD1[7:6]
    unsigned short   FrmPBLimitInt; //MD2[5:0] ~ MD3[7:0]
    unsigned char    reserved;
} __attribute__((packed)) HDMI_EM_SBTM_T;

typedef struct {
    unsigned char    RSV0_MD0;        //MD0[7:0]
    unsigned char    RSV0_MD1;        //MD1[7:0]
    unsigned char    RSV0_MD2;        //MD2[7:0]
    unsigned char    RSV0_MD3;        //MD3[7:0]
} __attribute__((packed)) HDMI_EM_RSV0_T;

typedef struct {
    unsigned char    RSV1_MD0;        //MD0[7:0]
    unsigned char    RSV1_MD1;        //MD1[7:0]
    unsigned char    RSV1_MD2;        //MD2[7:0]
    unsigned char    RSV1_MD3;        //MD3[7:0]
} __attribute__((packed)) HDMI_EM_RSV1_T;

typedef struct {
    unsigned char   ACP_Packet_Type;                  //HB0[7:0]
    unsigned char   ACP_Type;                               //HB1[7:0]
    unsigned char   Reserved_0;                             //HB2[7:0]
    unsigned char   ACP_Type_Dependent[28];     //PB0 ~ PB27
} __attribute__((packed)) HDMI_ACP_T;

typedef struct {
    unsigned char version;          // version 1 / version 2

    // PB6
    unsigned char freesync_supported : 1;
    unsigned char freesync_enabled : 1;
    unsigned char freesync_activate : 1;
    unsigned char native_color_space_active : 1;    // version==2 (PB9 present)
    unsigned char brightness_control_active : 1;    // version==2 (PB10 present)
    unsigned char local_diming_disalbe : 1;         // version==2
    unsigned char frame_type : 2; // version==3, 00 – Live, 01 – Live + Capture, 10 – Replay Frame, 11 – Reserved
    unsigned short min_refresh_rate;                 // PB7
    unsigned short max_refresh_rate;                 // PB8

    // PB9 : only valid when native_color_space_active is 1  (version==2)
    unsigned char srgb_eotf_active : 1;
    unsigned char bt709_eotf_active : 1;
    unsigned char gamma_2p2_eotf_active : 1;
    unsigned char gamma_2p6_eotf_active : 1;
    unsigned char pq_eotf_active : 1;               // version==3
    unsigned char fast_tranport_active : 1;         // version==3
    unsigned char reserved_pb9 : 2;

    // PB10 : only valid when native_color_space_active is 1  (version==2)
    unsigned char brightness_control;

    // PB13-14: Target Output Pixel Rate [kHz]  (version==3)
    unsigned int target_output_pixel_rate;

    // PB16: Fixed Rate Content Active  (version==3)
    unsigned char fixed_rate_content_active;

} __attribute__((packed)) HDMI_AMD_FREE_SYNC_INFO;

typedef enum {
    HDMI_QMS_FSM_NOT_SUPPORT, // QMS_EN=0, QMS not support,
    HDMI_QMS_FSM_VFREQ_CHANGE_START, // QMS_EN=1, m_const 1->0, v freq become unstable.scaler onms setting is the same with VRR flow.
    HDMI_QMS_FSM_VFREQ_CHANGE_RUNNING, // QMS_EN=1, const 0 -> 0,  V Freq still unstable
    HDMI_QMS_FSM_VFREQ_CHANGE_END, //  QMS_EN=1, m_const 0->1, v freq become unstable.
    HDMI_QMS_FSM_READY, //QMS_EN=1, m_const 1->1,
} HDMI_QMS_FSM_E;

typedef enum {
    HDMI_QMS_NEXT_TFR_NOT_ACTIVE = 0,
    HDMI_QMS_NEXT_TFR_23 = 1, // 24/1.001
    HDMI_QMS_NEXT_TFR_24, // 24HZ
    HDMI_QMS_NEXT_TFR_25, // 25HZ
    HDMI_QMS_NEXT_TFR_29, //30/1.001
    HDMI_QMS_NEXT_TFR_30, //30
    HDMI_QMS_NEXT_TFR_47, //48/1.001
    HDMI_QMS_NEXT_TFR_48,
    HDMI_QMS_NEXT_TFR_50,
    HDMI_QMS_NEXT_TFR_59,//60/1.001
    HDMI_QMS_NEXT_TFR_60,
    HDMI_QMS_NEXT_TFR_100,
    HDMI_QMS_NEXT_TFR_119, // 120/1.001
    HDMI_QMS_NEXT_TFR_120,
    HDMI_QMS_NEXT_TFR_NUM
} HDMI_QMS_NEXT_TFR_E;

typedef struct {
    HDMI_QMS_FSM_E qms_fsm;
    unsigned int current_vfreq;
    unsigned int pre_vfreq;
    unsigned char current_m_const;
    unsigned char pre_m_const;
    HDMI_QMS_NEXT_TFR_E next_tfr;
    unsigned int next_tfr_vfreq;
    unsigned char pre_qms_en;
    unsigned char current_qms_en;
    unsigned int qms_vs_cnt;
} HDMI_QMS_STATUS_T;


/**********************************************************************************************
*
*   Variables
*
**********************************************************************************************/
// Should Not declare variables here






/**********************************************************************************************
*
*   External Funtion Declarations
*
**********************************************************************************************/

// Generic
extern HDMI_DVI_MODE_T           drvif_Hdmi_Multi_port_GetHDMIMode(unsigned char ch);            // Get HDMI mode
extern HDMI_bool                 drvif_Hdmi_WatchDogApply(unsigned char Enable, unsigned char Condition);
extern HDMI_bool                 drvif_Hdmi_Multi_port_CheckMode(unsigned char ch);
extern unsigned char             drvif_Hdmi_GetCurrentPhysicalPort(void);
extern void                      drvif_Hdmi_Multi_port_OutputDisable(unsigned char ch, unsigned char on_off);
extern HDMI_PCB_ARC_EARC_T       drvif_Hdmi_get_pcb_earc_port_index(void);
extern unsigned int              drvif_Hdmi_Multi_port_CheckHdmiOnmsMask(unsigned char ch);
extern void                      drvif_Hdmi_SetVideoCommonSel(void);

#define drvif_get_current_port()            drvif_hdmi_get_current_display_port()
#define drvif_Hdmi_GetHDMIMode()            drvif_Hdmi_Multi_port_GetHDMIMode(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_CheckMode()              drvif_Hdmi_Multi_port_CheckMode(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_OutputDisable(on_off)    drvif_Hdmi_Multi_port_OutputDisable(HAL_VFE_HDMI_CURRENT_PORT, on_off)
#define drvif_Hdmi_CheckHdmiOnmsMask()      drvif_Hdmi_Multi_port_CheckHdmiOnmsMask(HAL_VFE_HDMI_CURRENT_PORT)


// MISC
extern void                      drvif_Hdmi_SetNoPolarityInverse(unsigned char on_off);
extern HDMI_bool                 drvif_Hdmi_IsHpdHigh(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Is5VHigh(unsigned char ch);

// Video Related
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetInterlace(unsigned char ch);
extern HDMI_COLOR_SPACE_T        drvif_Hdmi_Multi_port_GetRawColorSpace(unsigned char ch);

extern HDMI_COLOR_SPACE_T        drvif_Hdmi_Multi_port_GetColorSpace(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_EnableBypassYUV420Engine(unsigned char ch, unsigned char enable);
extern HDMI_COLORIMETRY_T        drvif_Hdmi_Multi_port_GetColorimetry(unsigned char ch);
extern HDMI_COLOR_DEPTH_T        drvif_Hdmi_Multi_port_GetColorDepth(unsigned char ch);
extern HDMI_RGB_YUV_RANGE_MODE_T drvif_Hdmi_Multi_port_GetRgbYuvRangeMode(unsigned char ch);
extern HDMI_3D_T                 drvif_Hdmi_Multi_port_GetReal3DFomat(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_CheckStableBeforeDisplay(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_IsAvmute(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_Set_Avmute_WatchDog(unsigned char ch, unsigned char enable);

#define drvif_Hdmi_GetInterlace()                     drvif_Hdmi_Multi_port_GetInterlace(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetRawColorSpace()                 drvif_Hdmi_Multi_port_GetRawColorSpace(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetColorSpace()                    drvif_Hdmi_Multi_port_GetColorSpace(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_EnableBypassYUV420Engine(enable)   drvif_Hdmi_Multi_port_EnableBypassYUV420Engine(HAL_VFE_HDMI_CURRENT_PORT, enable)
#define drvif_Hdmi_GetColorimetry()                   drvif_Hdmi_Multi_port_GetColorimetry(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetColorDepth()                    drvif_Hdmi_Multi_port_GetColorDepth(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetRgbYuvRangeMode()               drvif_Hdmi_Multi_port_GetRgbYuvRangeMode(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetReal3DFomat()                   drvif_Hdmi_Multi_port_GetReal3DFomat(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_CheckStableBeforeDisplay()         drvif_Hdmi_Multi_port_CheckStableBeforeDisplay(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_IsAvmute()                         drvif_Hdmi_Multi_port_IsAvmute(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_Set_Avmute_WatchDog(enable)        drvif_Hdmi_Multi_port_Set_Avmute_WatchDog(HAL_VFE_HDMI_CURRENT_PORT, enable)


// Audio Related
extern char                      drvif_Hdmi_IsAudioLock(unsigned char ch);
extern unsigned int              drvif_Hdmi_Audio_Get_N(unsigned char ch);
extern unsigned int              drvif_Hdmi_Audio_Get_Cts(unsigned char ch);


// Info Packet Related
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetVsInfoFrame(unsigned char ch, HDMI_VSI_T *pVsInfo);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetDolbyVsInfoFrame(unsigned char ch, vfe_hdmi_vsi_t *info_frame);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetHDR10pVsInfoFrame(unsigned char ch, vfe_hdmi_vsi_t *info_frame);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetAviInfoFrame(unsigned char ch, HDMI_AVI_T *pAviInfo);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetSpdInfoFrame(unsigned char ch, HDMI_SPD_T *pSpdInfo);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetDrmInfoFrame(unsigned char ch, HDMI_DRM_T *pDrmInfo);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetAudioInfoFrame(unsigned char ch, HDMI_AUDIO_T *pAudioInfo);
extern HDMI_bool                 drvif_Hdmi_Multi_port_IsDRMInfoReady(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetVRREnable(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetQMSEnable(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetMConst(unsigned char ch);
extern HDMI_VTEM_NEXT_TFR        drvif_Hdmi_Multi_port_GetNextTFR(unsigned char ch);
HDMI_QMS_STATUS_T               *drvif_Hdmi_Multi_port_GetQmsStatus(unsigned char ch, unsigned short *current_vfreq);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetALLMEnable(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetAMDFreeSyncStatus(unsigned char ch, HDMI_AMD_FREE_SYNC_INFO *p_info);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetAMDFreeSyncEnable(unsigned char ch);
extern HDMI_bool                 drvif_Hdmi_Multi_port_GetDolbyL11Content(unsigned char ch, unsigned char get_data[4]);
extern HDMI_bool                 drvif_Hdmi_SetDolbyVsifUpdateNoPktThreshold(unsigned char no_pkt_thd);
extern int                       drvif_Hdmi_Multi_port_GetEmpPacket(unsigned char ch, vfe_hdmi_em_type_e type, unsigned char *pkt_buf, unsigned short len);


#define drvif_Hdmi_GetVsInfoFrame(pVsInfo)              drvif_Hdmi_Multi_port_GetVsInfoFrame(HAL_VFE_HDMI_CURRENT_PORT, pVsInfo)
#define drvif_Hdmi_GetDolbyVsInfoFrame(info_frame)      drvif_Hdmi_Multi_port_GetDolbyVsInfoFrame(HAL_VFE_HDMI_CURRENT_PORT, info_frame)
#define drvif_Hdmi_GetHDR10pVsInfoFrame(info_frame)     drvif_Hdmi_Multi_port_GetHDR10pVsInfoFrame(HAL_VFE_HDMI_CURRENT_PORT, info_frame)
#define drvif_Hdmi_GetAviInfoFrame(pAviInfo)            drvif_Hdmi_Multi_port_GetAviInfoFrame(HAL_VFE_HDMI_CURRENT_PORT, pAviInfo)
#define drvif_Hdmi_GetSpdInfoFrame(pSpdInfo)            drvif_Hdmi_Multi_port_GetSpdInfoFrame(HAL_VFE_HDMI_CURRENT_PORT, pSpdInfo)
#define drvif_Hdmi_GetDrmInfoFrame(pDrmInfo)            drvif_Hdmi_Multi_port_GetDrmInfoFrame(HAL_VFE_HDMI_CURRENT_PORT, pDrmInfo)
#define drvif_Hdmi_GetAudioInfoFrame(pAudioInfo)        drvif_Hdmi_Multi_port_GetAudioInfoFrame(HAL_VFE_HDMI_CURRENT_PORT, pAudioInfo)
#define drvif_Hdmi_IsDRMInfoReady()                     drvif_Hdmi_Multi_port_IsDRMInfoReady(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetVRREnable()                       drvif_Hdmi_Multi_port_GetVRREnable(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetQMSEnable()                       drvif_Hdmi_Multi_port_GetQMSEnable(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetMConst()                          drvif_Hdmi_Multi_port_GetMConst(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetNextTFR()                         drvif_Hdmi_Multi_port_GetNextTFR(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetQmsStatus(current_vfreq)          drvif_Hdmi_Multi_port_GetQmsStatus(HAL_VFE_HDMI_CURRENT_PORT, current_vfreq)
#define drvif_Hdmi_GetALLMEnable()                      drvif_Hdmi_Multi_port_GetALLMEnable(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetAMDFreeSyncStatus(p_info)         drvif_Hdmi_Multi_port_GetAMDFreeSyncStatus(HAL_VFE_HDMI_CURRENT_PORT,  p_info)
#define drvif_Hdmi_GetAMDFreeSyncEnable()               drvif_Hdmi_Multi_port_GetAMDFreeSyncEnable(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_Hdmi_GetDolbyL11Content(get_data)         drvif_Hdmi_Multi_port_GetDolbyL11Content(HAL_VFE_HDMI_CURRENT_PORT, get_data)
#define drvif_Hdmi_GetEmpPacket(type, pkt_buf, len)     drvif_Hdmi_Multi_port_GetEmpPacket(HAL_VFE_HDMI_CURRENT_PORT, type, pkt_buf, len)

// HDCP related
extern HDMI_bool                 drvif_Hdmi_Multi_port_HdcpEnabled(unsigned char port);
#define drvif_Hdmi_HdcpEnabled()        drvif_Hdmi_Multi_port_HdcpEnabled(HAL_VFE_HDMI_CURRENT_PORT)

//Timing info
extern HDMI_bool drvif_Hdmi_Multi_port_GetRawTimingInfo(unsigned char port, MEASURE_TIMING_T *timing_info);

void drvif_Hdmi_set_infoframe_thread_stop(unsigned char ucStop);
extern void drvif_Hdmi_set_infoframe_thread_stop_nonseamphore_for_isr(unsigned char ucStop);// for ISR use

unsigned char drvif_Hdmi_get_infoframe_thread_stop(void);
extern void drvif_Hdmi_infoframe_update_for_isr(void);
extern unsigned char drvif_Hdmi_GetVsemIsDolby(unsigned char ch);
extern unsigned char drvif_Hdmi_get_dolby_vsem_infoframe(unsigned char ch, unsigned char *p_vsem);
extern unsigned int drvif_hdmi_get_no_dolby_vsem_emp_cnt(unsigned char ch);
extern void drvif_Hdmi_SetDynamicHdmiPhyThreadCycleTime(unsigned char ucMs);
extern void drvif_Hdmi_SetDynamicHdmiAudioThreadCycleTime(unsigned char ucMs);
extern void drvif_Hdmi_SetDynamicHdmiInfoPacketThreadCycleTime(unsigned char ucMs);
extern unsigned char drvif_hdmi_get_current_display_port(void);

extern void drvif_hdmi_pkt_bypass_to_hdmitx_en(unsigned char ch, unsigned char enable);
extern unsigned char drvif_Hdmi_Multi_port_GetFrlMode(unsigned char ch);

#define drvif_Hdmi_GetRawTimingInfo(timing_info)       drvif_Hdmi_Multi_port_GetRawTimingInfo(HAL_VFE_HDMI_CURRENT_PORT, timing_info)
#define drvif_Hdmi_GetFrlMode()                        drvif_Hdmi_Multi_port_GetFrlMode(HAL_VFE_HDMI_CURRENT_PORT)
/**********************************************************************************************
*
*   Leagacy API wrapper (The following API was still in-used, so we add io wrapper to covert it to new APIs)
*
**********************************************************************************************/

#define drvif_Multi_port_IsHDMI(ch)            ((drvif_Hdmi_Multi_port_GetHDMIMode(ch)==MODE_DVI) ? MODE_DVI : MODE_HDMI)  // for backward compatible
#define drvif_Multi_port_IsRGB_YUV_RANGE(ch)   drvif_Hdmi_Multi_port_GetRgbYuvRangeMode(ch)
#define hdmi_Multi_port_is_drm_info_ready(ch)  drvif_Hdmi_Multi_port_IsDRMInfoReady(ch)

#define drvif_IsHDMI()            drvif_Multi_port_IsHDMI(HAL_VFE_HDMI_CURRENT_PORT)
#define drvif_IsRGB_YUV_RANGE()   drvif_Multi_port_IsRGB_YUV_RANGE(HAL_VFE_HDMI_CURRENT_PORT)
#define hdmi_is_drm_info_ready()  hdmi_Multi_port_is_drm_info_ready(HAL_VFE_HDMI_CURRENT_PORT)


#endif //_HDMI_FUN_H
