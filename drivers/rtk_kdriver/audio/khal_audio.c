#ifndef _RHAL_AUDIO__
#define _RHAL_AUDIO__

#include <linux/string.h>
#include <linux/unistd.h>
#include <linux/delay.h>

#include "rtkdemux_export.h"

#include "hresult.h"
#include "audio_flow.h"
#include <linux/slab.h>
#include <linux/kthread.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/quick_show/quick_show.h>
#include <rtk_kdriver/rtkaudio.h>
#include <rtk_kdriver/rmm/pageremap.h>
#include <rtk_kdriver/video_common/rtk_video_common_drvif.h>

#include "audio_rpc.h"
#include "audio_inc.h"
#include "kadp_audio.h"

#include "rtk_kdriver/rtkaudio_debug.h"
#include "audio_base.h"


#include <hal_audio.h>

/************************************************************************
 *  Config Definitions
 ************************************************************************/

typedef enum DTS_PARSER_SYNC_TYPE {
    DTS_PARSER_SYNC_TYPE_STANDALONE_CORE = 0,
    DTS_PARSER_SYNC_TYPE_STANDALONE_EXSS,
    DTS_PARSER_SYNC_TYPE_CORE_PLUS_EXSS,
    DTS_PARSER_SYNC_TYPE_STANDALONE_METADATA,
    DTS_PARSER_SYNC_TYPE_FTOC
} DTS_PARSER_SYNC_TYPE;

/* use for avoid pop noise when adjust volume */
#define AVOID_USELESS_RPC

#define AMIXER_AUTO_MUTE

/* use hybrid volume control (positive @dec and negative @PP) for AD gain */
#define SUPPORT_AD_POSITIVE_GAIN

/* End of Config Definitions
 ************************************************************************/

extern bool ARC_offon;
extern bool eARC_offon;
extern uint32_t rtkaudio_alloc_uncached(int size, unsigned char** uncached_addr_info);
extern int rtkaudio_free(unsigned int phy_addr);

extern int KADP_SDEC_DisConnect(DEMUX_CHANNEL_T ch, RSDEC_ENUM_TYPE_DEST_T con_dest);
extern int KADP_SDEC_Connect(DEMUX_CHANNEL_T ch, RSDEC_ENUM_TYPE_DEST_T con_dest, DEMUX_STRUCT_CONNECT_RESULT_T *pBufInfo);
extern int KADP_SDEC_PrivateInfo (DEMUX_CHANNEL_T ch, RSDEC_ENUM_TYPE_DEST_T dest, DEMUX_PRIVATEINFO__T infoId, ULONG *pInfoData, ULONG infoSize);

//=========================================================
extern int gUSER_CONFIG;

typedef enum {
    ENUM_DTS_PARAM_TBHDX_APP_FIRST_PARAM = ENUM_DTS_PARAM_TBHDX_FIRST_PARAM + 0x1000,
    ENUM_DTS_PARAM_TBHDX_APP_SPKSIZE_I32,           /**< TruBass HDx Speaker Custom Size. Type: int. Range: 40 ~ 600. Default: 80. */
    ENUM_DTS_PARAM_TBHDX_APP_HPRATIO_F32,           /**< TruBass HDx HP ratio. Type: dtsFloat32. Range: 0 ~ 1.0. Default: 0.5f. */
    ENUM_DTS_PARAM_TBHDX_APP_EXTBASS_F32,           /**< TruBass HDX ExtBass. Type: dtsFloat32. Range: 0 ~ 1.0. Default: 0.8f. */
    ENUM_DTS_PARAM_TBHDX_APP_LAST_PARAM
} TBHDX_APPT_TYPE;

typedef enum {
    RHAL_NOT_INIT = 0xff,
    RHAL_FULL,
    RHAL_LITE,
} RHAL_TYPE;
static RHAL_TYPE rhal_type = RHAL_NOT_INIT;

BOOLEAN IsAudioInitial(void)
{
    if(rhal_type == RHAL_NOT_INIT)
    {
        ERROR("audio not yet initialmodule");
        return FALSE;
    }
    return TRUE;
}
BOOLEAN IsRHALFullMode(void)
{
    if(rhal_type != RHAL_FULL)
    {
        ERROR("audio not initial in Full support mode rhal_type %d\n", rhal_type);
        return FALSE;
    }
    return TRUE;
}
#define LITE_HAL_INIT_OK IsAudioInitial
#define AUDIO_HAL_CHECK_INITIAL_OK IsRHALFullMode

DTV_STATUS_T RAL_AUDIO_FinalizeModule(void);

#define HAL_MAX_OUTPUT (10)
#define HAL_MAX_RESOURCE_NUM (HAL_AUDIO_RESOURCE_MAX) /* max resource in hal_audio.h */

#define AUD_AIN_MAX (5)

#define AUD_AOUT_MAX AUD_ADEC_MAX
#define AUD_ADEC_INIT_MAX (HAL_AUDIO_ADEC1 + 1)
#define AUD_ADEC_MAX (HAL_AUDIO_ADEC_MAX + 1)
#define AUD_AMIX_MAX (HAL_AUDIO_MIXER_MAX + 1)

#define AUD_MAX_CHANNEL (8)
#define AUD_MAX_DELAY (1000) /* ms */
/*

 open <->  connect    <->      run <-> pause (pause_and_stopflow)
 ^          ^                   ^
 |          |                   |

 close <-> disconnect  <->     stop <-> pause

*/
typedef  enum
{
    HAL_AUDIO_RESOURCE_CLOSE            = 0,
    HAL_AUDIO_RESOURCE_OPEN             = 1,
    HAL_AUDIO_RESOURCE_CONNECT          = 2,
    HAL_AUDIO_RESOURCE_DISCONNECT       = 3,
    HAL_AUDIO_RESOURCE_STOP             = 4,
    HAL_AUDIO_RESOURCE_PAUSE            = 5,
    HAL_AUDIO_RESOURCE_RUN              = 6,
    HAL_AUDIO_RESOURCE_PAUSE_AND_STOP   = 7,
    HAL_AUDIO_RESOURCE_STATUS_UNKNOWN   = 8,
} HAL_AUDIO_RESOURCE_STATUS;

typedef struct {
    ENUM_AUDIO_INFO_TYPE infoType;
    int iptSrc;
    int sndOut;
    int arcMode;
    int latency;
} AUDIO_LATENCY_INFO;

typedef enum {
    SOURCE_UNKNOWN = 0,
    SOURCE_DTV,
    SOURCE_GST_HALF_TUNNEL,
    SOURCE_GST_FULL_TUNNEL,
    SOURCE_GST_NONE_TUNNEL,
    SOURCE_HDMI,
    SOURCE_ATV,
    SOURCE_ADC,
    SOURCE_OFFLOAD,
    SOURCE_ATSC30,
    SOURCE_GST_DIRECT_MEDIA,
    SOURCE_MAX,
} AUDIO_SOURCE;

/**
 * @brief input source type for audio latency time
 */
typedef enum
{
    DELAY_INPUT_SRC_UNKNOWN = 0, // DEFAULT
    DELAY_INPUT_SRC_TP      = 1,
    DELAY_INPUT_SRC_ADC     = 2,
    DELAY_INPUT_SRC_AAD     = 3,
    DELAY_INPUT_SRC_HDMI    = 4,
    DELAY_INPUT_SRC_MEDIA   = 5,
    DELAY_INPUT_SRC_MAX     = 6,
} HAL_AUDIO_DELAY_INPUT_SRC;

// All resource
const CHAR *ResourceName[] = {
    "SDEC0\0",
    "SDEC1\0",
    "ATP0\0",
    "ATP1\0",
    "ADC\0",
    "HDMI\0",
    "AAD\0",
    "SYSTEM\0",
    "ADEC0\0",
    "ADEC1\0",
    "AENC0\0",
    "AENC1\0",
    "SE\0",
    "SPK\0",
    "SPDIF\0",
    "SB_SPDIF\0",
    "SB_PCM\0",
    "SB_CANVAS\0",
    "HP\0",
    "SCART\0",
    "AMIXER\0",
    "MIXER1\0",
    "MIXER2\0",
    "MIXER3\0",
    "MIXER4\0",
    "MIXER5\0",
    "MIXER6\0",
    "MIXER7\0",
    "SPDIF_ES\0",
    "HDMI0\0",
    "HDMI1\0",
    "HDMI2\0",
    "HDMI3\0",
    "SWITCH\0",
    "DP0\0",
    "DP1\0",
    "ADEC2\0",
    "ADEC3\0",
    "ADEC4\0",
    "ADEC5\0",
    "ADEC6\0",
    "ADEC7\0",
    "I2SIN\0",
    "UACIN\0",
    "SPDIFIN\0",
    "MAX\0",
    "unknown\0",// unknown
};

const CHAR* ResourceStatusSrting[] = {
    "CLOSE",
    "OPEN",
    "CONNECT",
    "DISCONNECT",
    "STOP",
    "PAUSE",
    "RUN",
    "UNKNOWN",// unknown
};

typedef struct
{
    CHAR name[12];
    HAL_AUDIO_RESOURCE_STATUS connectStatus[HAL_MAX_OUTPUT];
    HAL_AUDIO_RESOURCE_T inputConnectResourceId[HAL_MAX_OUTPUT]; //HAL_AUDIO_RESOURCE_T
    HAL_AUDIO_RESOURCE_T outputConnectResourceID[HAL_MAX_OUTPUT];//HAL_AUDIO_RESOURCE_T
    SINT32 numOptconnect;
    SINT32 numIptconnect;
    SINT32 maxVaildIptNum; // output moduel used
} HAL_AUDIO_MODULE_STATUS;

static const CHAR* GetResourceString(HAL_AUDIO_RESOURCE_T resId);
static const CHAR* GetResourceStatusString(SINT32  statusId);

typedef struct
{
    UINT8 IsAINExist;
    UINT8 IsDECExist;
    UINT8 IsPPAOExist;
    UINT8 usedAud_Ainidx;
    UINT8 usedAud_decidx;
    UINT8 usedAud_Aoutidx;
    UINT8 IsDTV0SourceRead; // ATP
    UINT8 IsDTV1SourceRead;
} HAL_AUDIO_FLOW_STATUS;

static void UpdateADECStatus(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
static BOOLEAN AddAndConnectDecAinToFlow(FlowManager* pFlowManager, HAL_AUDIO_ADEC_INDEX_T adecIndex, Base* pAin, HAL_AUDIO_FLOW_STATUS* pFlowStatus);
static BOOLEAN AddAndConnectPPAOToFlow(FlowManager* pFlowManager, HAL_AUDIO_ADEC_INDEX_T adecIndex, Base* pPPAO, HAL_AUDIO_FLOW_STATUS* pFlowStatus);

typedef struct
{
    UINT32 decMute;
    UINT32 decDelay;
    UINT32 decVol[AUD_MAX_CHANNEL];
    UINT32 decFormat;
} ADSP_DEC_INFO;

typedef struct
{
    BOOLEAN spk_mute;
    BOOLEAN spdif_mute;
    BOOLEAN spdifes_mute;
    BOOLEAN hp_mute;
    BOOLEAN scart_mute;
} ADSP_SNDOUT_INFO;

typedef struct
{
    SINT32 spdifouttype;
    BOOLEAN spdifESMute;
    BOOLEAN decInMute;
    BOOLEAN userSetRun; // internal will auto stop flow, use this flag to store user setting.
    HAL_AUDIO_VOLUME_T decInVolume;
    HAL_AUDIO_VOLUME_T decOutVolume[AUD_MAX_CHANNEL];
    HAL_AUDIO_DOLBY_DRC_MODE_T drcMode;
    HAL_AUDIO_DOWNMIX_MODE_T downmixMode;
    BOOLEAN decAtvMute;
    SINT32 speed;
} HAL_AUDIO_DEC_MODULE_STATUS;

HAL_AUDIO_DEC_MODULE_STATUS Aud_decstatus[AUD_ADEC_MAX];

ADSP_SNDOUT_INFO adsp_sndout_info;

SINT32 Aud_mainDecIndex     = 0;
SINT32 Aud_prevMainDecIndex = -1;
SINT32 Aud_descriptionMode  = 0;
BOOLEAN m_IsSetMainDecOptByMW = FALSE;

static unsigned long dbxTablePhyAddr = 0;
static UINT8* dbxTableAddr = NULL;

//=========================================================
// SW balance
static SINT32 m_volume_comp[2] = {-160, -160}; //spkr, hp
static SINT32 m_balance = 0;
static SINT32 m_pre_bal = 0;
#define CH_L (0x1<<0)
#define CH_R (0x1<<1)
#define CH_ALL (0xFF)
#define GAIN_L (0)
#define GAIN_R (1)
#define HWGAIN_MAX 240
#define HWGAIN_MIN -999
#define PARAM_IDX_SPKR 0
#define PARAM_IDX_HP 1
#define PARAM_IDX_MAX 2
//=========================================================
#define MAIN_AIN_ADC_PIN (9)

typedef struct
{
    SINT32 ainPinStatus[MAIN_AIN_ADC_PIN];
    UINT32 usedAud_Ain[5];
    UINT32 hdmifullcapport;
} HAL_AUDIO_AIN_MODULE_STATUS;

HAL_AUDIO_MODULE_STATUS ResourceStatus[HAL_MAX_RESOURCE_NUM];
HAL_AUDIO_FLOW_STATUS FlowStatus[AUD_ADEC_MAX];
HAL_AUDIO_AIN_MODULE_STATUS AinStatus;
ADSP_DEC_INFO AudioDecInfo[AUD_ADEC_MAX];
ADSP_DEC_INFO MediaAudioDecInfo;

HAL_AUDIO_ADEC_INFO_T Aud_Adec_info[AUD_ADEC_MAX];
UINT32 _AudioSPDIFMode = ENABLE_DOWNMIX;
UINT32 _AudioARCMode = ENABLE_DOWNMIX;
UINT32 _ARC_Enable = FALSE;
UINT32 _EARC_Enable = FALSE;

static UINT32 Sndout_Devices = 0;

#define HAL_DEC_MAX_OUTPUT_NUM (7) // dec maybe connect with spdif, spdif_ES, HP, SE(speak), scart, SB_PCM(BT), SB_CANVAS

#define AUDIO_CHECK_ADC_PIN_OPEN_NOTAVAILABLE(status)       ((status != HAL_AUDIO_RESOURCE_CLOSE) && ( status != HAL_AUDIO_RESOURCE_OPEN))
#define AUDIO_CHECK_ADC_PIN_CLOSE_NOTAVAILABLE(status)      ((status != HAL_AUDIO_RESOURCE_OPEN) && (status != HAL_AUDIO_RESOURCE_DISCONNECT) && (status != HAL_AUDIO_RESOURCE_OPEN))
#define AUDIO_CHECK_ADC_PIN_CONNECT_NOTAVAILABLE(status)    ((status != HAL_AUDIO_RESOURCE_OPEN) && (status != HAL_AUDIO_RESOURCE_DISCONNECT))
#define AUDIO_CHECK_ADC_PIN_DISCONNECT_NOTAVAILABLE(status) (status != HAL_AUDIO_RESOURCE_CONNECT)

#define IsHDMIportConnected(portnum) (ResourceStatus[HAL_AUDIO_RESOURCE_HDMI0+portnum].connectStatus[0] == HAL_AUDIO_RESOURCE_CONNECT)
#define IsDPportConnected(portnum) (ResourceStatus[HAL_AUDIO_RESOURCE_DP+portnum].connectStatus[0] == HAL_AUDIO_RESOURCE_CONNECT)

#define IsHDMIportRunning(portnum) (ResourceStatus[HAL_AUDIO_RESOURCE_HDMI0+portnum].connectStatus[0] == HAL_AUDIO_RESOURCE_RUN)
#define IsHDMIFullCapPort(portnum) (AinStatus.hdmifullcapport == portnum)

#define AUDIO_HAL_CHECK_AMIX_PLAY_NOTAVAILABLE(res) \
    ((res.connectStatus[0] != HAL_AUDIO_RESOURCE_CONNECT) && \
     (res.connectStatus[0] != HAL_AUDIO_RESOURCE_STOP))

#define AUDIO_HAL_CHECK_AMIX_STOP_NOTAVAILABLE(res) \
    ((res.connectStatus[0] != HAL_AUDIO_RESOURCE_RUN) && \
     (res.connectStatus[0] != HAL_AUDIO_RESOURCE_PAUSE))

#define AUDIO_HAL_CHECK_AMIX_PAUSE_NOTAVAILABLE(res) \
    (res.connectStatus[0] != HAL_AUDIO_RESOURCE_RUN)

#define AUDIO_HAL_CHECK_AMIX_RESUME_NOTAVAILABLE(res) \
    (res.connectStatus[0] != HAL_AUDIO_RESOURCE_PAUSE)

#define AUDIO_HAL_CHECK_PLAY_NOTAVAILABLE(res, inputID) \
    ((res.connectStatus[inputID] != HAL_AUDIO_RESOURCE_CONNECT) && \
     (res.connectStatus[inputID] != HAL_AUDIO_RESOURCE_STOP))

#define AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(res, inputID) \
    ((res.connectStatus[inputID] != HAL_AUDIO_RESOURCE_RUN) && \
     (res.connectStatus[inputID] != HAL_AUDIO_RESOURCE_PAUSE))

#define AUDIO_HAL_CHECK_PAUSE_NOTAVAILABLE(res, inputID) \
    (res.connectStatus[inputID] != HAL_AUDIO_RESOURCE_RUN)

#define AUDIO_HAL_CHECK_PAUSE_AND_STOP_NOTAVAILABLE(res, inputID) \
    (res.connectStatus[inputID] != HAL_AUDIO_RESOURCE_RUN)

#define AUDIO_HAL_CHECK_RESUME_NOTAVAILABLE(res, inputID) \
    (res.connectStatus[inputID] != HAL_AUDIO_RESOURCE_PAUSE)

#define AUDIO_HAL_CHECK_ISATRUNSTATE(res, inputID) \
    ((res.connectStatus[inputID] == HAL_AUDIO_RESOURCE_RUN) || \
    (res.connectStatus[inputID] == HAL_AUDIO_RESOURCE_PAUSE))

#define AUDIO_HAL_CHECK_ISATSTOPSTATE(res, inputID) \
    ((res.connectStatus[inputID] == HAL_AUDIO_RESOURCE_CONNECT) || \
    (res.connectStatus[inputID] == HAL_AUDIO_RESOURCE_STOP))

#define AUDIO_HAL_CHECK_ISATPAUSESTATE(res, inputID) \
    ((res.connectStatus[inputID] == HAL_AUDIO_RESOURCE_PAUSE))

#define AUDIO_HAL_CHECK_ISATPAUSEANDSTOPSTATE(res, inputID) \
    ((res.connectStatus[inputID] == HAL_AUDIO_RESOURCE_PAUSE_AND_STOP))

struct AUDIO_QUICK_SHOW_INIT_TABLE
{
    int quick_show_status;
    int ain_inst;
    int ain_out_rh;
    int ain_out_buf;
    int dec_inst;
    int dec_out_rh[8];
    int dec_out_buf[8];
    int dec_icq_rh;
    int dec_icq_buf;
    int ao_inst;
};
typedef struct AUDIO_QUICK_SHOW_INIT_TABLE AUDIO_QUICK_SHOW_INIT_TABLE;

//--- spdif out channel status setting ---//
// copy from cppaout.h
#define HAL_SPDIF_CONSUMER_USE          0x0
#define HAL_SPDIF_PROFESSIONAL_USE      0x1

#define HAL_SPDIF_COPYRIGHT_NEVER       0x0
#define HAL_SPDIF_COPYRIGHT_FREE        0x1

#define HAL_SPDIF_CATEGORY_GENERAL      0x00
#define HAL_SPDIF_CATEGORY_EUROPE       0x0C
#define HAL_SPDIF_CATEGORY_USA          0x64
#define HAL_SPDIF_CATEGORY_JAPAN        0x04

#define HAL_SPDIF_CATEGORY_L_BIT_IS_1   0x80
#define HAL_SPDIF_CATEGORY_L_BIT_IS_0   0x00

#define HAL_SPDIF_WORD_LENGTH_NONE      0x0
#define HAL_SPDIF_WORD_LENGTH_16        0x2
#define HAL_SPDIF_WORD_LENGTH_18        0x4
#define HAL_SPDIF_WORD_LENGTH_19        0x8
#define HAL_SPDIF_WORD_LENGTH_20_0      0xA
#define HAL_SPDIF_WORD_LENGTH_17        0xC
#define HAL_SPDIF_WORD_LENGTH_20_1      0x3
#define HAL_SPDIF_WORD_LENGTH_22        0x5
#define HAL_SPDIF_WORD_LENGTH_23        0x9
#define HAL_SPDIF_WORD_LENGTH_24        0xB
#define HAL_SPDIF_WORD_LENGTH_21        0xD
//======================================================

static HAL_AUDIO_COMMON_INFO_T g_AudioStatusInfo;

typedef struct HAL_AUDIO_SIF_INFO
{
    HAL_AUDIO_SIF_INPUT_T               sifSource;                  /* Currnet SIF Source Input Status */
    HAL_AUDIO_SIF_TYPE_T                curSifType;                 /* Currnet SIF Type Status */
    BOOLEAN                             bHighDevOnOff;              /* Currnet High DEV ON/OFF Status */
    HAL_AUDIO_SIF_SOUNDSYSTEM_T         curSifBand;                 /* Currnet SIF Sound Band(Sound System) */
    HAL_AUDIO_SIF_STANDARD_T            curSifStandard;             /* Currnet SIF Sound Standard */
    HAL_AUDIO_SIF_EXISTENCE_INFO_T      curSifIsA2;                 /* Currnet SIF A2 Exist Status */
    HAL_AUDIO_SIF_EXISTENCE_INFO_T      curSifIsNicam;              /* Currnet SIF NICAM Exist Status */
    HAL_AUDIO_SIF_MODE_SET_T            curSifModeSet;              /* Currnet SIF Sound Mode Set Status */
    HAL_AUDIO_SIF_MODE_GET_T            curSifModeGet;              /* Currnet SIF Sound Mode Get Status */
} HAL_AUDIO_SIF_INFO_T;

static HAL_AUDIO_SIF_INFO_T g_AudioSIFInfo = {HAL_AUDIO_SIF_INPUT_INTERNAL, HAL_AUDIO_SIF_TYPE_NONE, FALSE, HAL_AUDIO_SIF_SYSTEM_UNKNOWN, HAL_AUDIO_SIF_MODE_DETECT,
                                              HAL_AUDIO_SIF_DETECTING_EXSISTANCE,HAL_AUDIO_SIF_DETECTING_EXSISTANCE, HAL_AUDIO_SIF_SET_PAL_UNKNOWN,HAL_AUDIO_SIF_GET_PAL_UNKNOWN};

HAL_AUDIO_VOLUME_T g_mixer_gain[AUD_AMIX_MAX] = {{.mainVol = 0x7F, .fineVol = 0x0}};
HAL_AUDIO_VOLUME_T g_mixer_out_gain = {.mainVol = 0x7F, .fineVol = 0x0}; // only one ?

BOOLEAN g_mixer_user_mute[AUD_AMIX_MAX] = {FALSE};
BOOLEAN g_mixer_curr_mute[AUD_AMIX_MAX] = {FALSE};

#define DB2MIXGAIN_TABLE_SIZE (52)
const UINT32 dB2mixgain_table[DB2MIXGAIN_TABLE_SIZE] = {
    0x7FFFFFFF , 0x721482BF , 0x65AC8C2E , 0x5A9DF7AA , //  0  ~ -3dB
    0x50C335D3 , 0x47FACCF0 , 0x3FFFFFFF , 0x392CED8D , // -4  ~ -7dB
    0x32F52CFE , 0x2D6A866F , 0x287A26C4 , 0x241346F5 , // -8  ~ -11dB
    0x2026F30F , 0x1CA7D767 , 0x198A1357 , 0x16C310E3 , // -12 ~ -15dB
    0x144960C5 , 0x12149A5F , 0x101D3F2D , 0xE5CA14C  , // -16 ~ -19dB
    0xCCCCCCC  , 0xB687379  , 0xA2ADAD1  , 0x90FCBF7  , // -20 ~ -23dB
    0x8138561  , 0x732AE17  , 0x66A4A52  , 0x5B7B15A  , // -24 ~ -27dB
    0x518847F  , 0x48AA70B  , 0x40C3713  , 0x39B8718  , // -28 ~ -31dB
    0x337184E  , 0x2DD958A  , 0x28DCEBB  , 0x246B4E3  , // -32 ~ -35dB
    0x207567A  , 0x1CEDC3C  , 0x19C8651  , 0x16FA9BA  , // -36 ~ -39dB
    0x147AE14  , 0x1240B8C  , 0x1044914  , 0xE7FACB   , // -40 ~ -43dB
    0xCEC089   , 0xB8449B   , 0xA43AA1   , 0x925E89   , // -44 ~ -47dB
    0x8273A6   , 0x7443E7   , 0x679F1B   , 0x5C5A4F     // -48 ~ -51dB
};
const UINT32 dB2mixgain_volumeid_table[DB2MIXGAIN_TABLE_SIZE] = {
    389, 385, 381, 377, 373, 369, 365, 361, 357, 353, //  0 ~ -9dB
    349, 345, 341, 337, 333, 329, 325, 321, 317, 313, // -10 ~ -19dB
    309, 305, 301, 297, 293, 289, 285, 281, 277, 273, // -20 ~ -29dB
    269, 265, 261, 257, 253, 249, 245, 241, 237, 233, // -30 ~ -39dB
    229, 225, 221, 217, 213, 209, 205, 201, 197, 193, // -40 ~ -49dB
    189, 185                                          // -50 ~ -51dB
};
#define ADEC_DSP_MIX_GAIN_MUTE (0x00000000)
#define ADEC_DSP_MIX_GAIN_0DB  (0x7FFFFFFF)
HAL_AUDIO_VOLUME_T currMainVol = {.mainVol = 0x7F, .fineVol = 0x0};
HAL_AUDIO_VOLUME_T currADVol = {.mainVol = 0x7F, .fineVol = 0x0};

static Base* Aud_MainAin = NULL;
static Base* Aud_dec[AUD_ADEC_MAX] = {NULL};
static Base* Aud_Ain[AUD_AIN_MAX] = {NULL};
static Base* Aud_Aout[AUD_ADEC_MAX] = {NULL};
static Base* Aud_DTV[AUD_ADEC_MAX] = {NULL};
static FlowManager* Aud_flow[AUD_ADEC_MAX] = {NULL};

static CHAR AUD_StringBuffer[128];

const CHAR *SRCTypeName[] = {
    "UNKNOWN",     /* = 0  */
    "PCM",         /* = 1  */
    "AC3",         /* = 2  */
    "EAC3",        /* = 3  */
    "MPEG",        /* = 4  */
    "AAC",         /* = 5  */
    "HEAAC",       /* = 6  */
    "DRA",         /* = 7  */
    "MP3",         /* = 8  */
    "DTS",         /* = 9  */
    "SIF",         /* = 10 */
    "SIF_BTSC",    /* = 11 */
    "SIF_A2",      /* = 12 */
    "DEFAULT",     /* = 13 */
    "NONE",        /* = 14 */
    "DTS_HD_MA",   /* = 15 */
    "DTS_EXPRESS", /* = 16 */
    "DTS_CD",      /* = 17 */
    "EAC3_ATMOS",  /* = 18 */
    "AC4",         /* = 19 */
    "AC4_ATMOS",   /* = 20 */
    "MPEG_H",      /* = 21 */
    "MAT",         /* = 22 */
    "MAT_ATMOS",   /* = 23 */
    "OPUS",        /* = 24 */
    "VORBIS",      /* = 25 */
    "TRUEHD",      /* = 26 */
    "FLAC",        /* = 27 */
    "RA",          /* = 28 */
    "DTS_X",       /* = 29 */
};

typedef struct {
    int ch01_scale_a;
    int ch01_scale_b;
    int ch23_scale_a;
    int ch23_scale_b;
    int ch45_scale_a;
    int ch45_scale_b;
    int ch67_scale_a;
    int ch67_scale_b;
} Prescale;

Prescale pre_scale;

/***********************************************************************************
 * Static resource checking function
 **********************************************************************************/
static const CHAR* GetResourceString(HAL_AUDIO_RESOURCE_T resId)
{
    if ((resId >= 0) && (resId <= HAL_AUDIO_RESOURCE_MAX))
        return ResourceName[resId];

    return ResourceName[HAL_AUDIO_RESOURCE_MAX];
}

static const CHAR* GetResourceStatusString(SINT32 statusId)
{
    SINT32 maxid = (sizeof(ResourceStatusSrting)/sizeof(ResourceStatusSrting[0]));
    if((statusId >= 0) && (statusId < maxid))
        return ResourceStatusSrting[statusId];
    else
        return ResourceStatusSrting[maxid -1];
}

static const CHAR* GetSRCTypeName(HAL_AUDIO_SRC_TYPE_T SRCType)
{
    if(SRCType < 0 || SRCType > HAL_AUDIO_SRC_TYPE_DTS_X)
        return SRCTypeName[0];
    else
        return SRCTypeName[SRCType];
}

static inline HAL_AUDIO_ADEC_INDEX_T res2adec(HAL_AUDIO_RESOURCE_T res_id)
{
    if(res_id < HAL_AUDIO_RESOURCE_ADEC2)
        return (HAL_AUDIO_ADEC_INDEX_T)(res_id - HAL_AUDIO_RESOURCE_ADEC0);
    else
        return (HAL_AUDIO_ADEC_INDEX_T)(res_id - HAL_AUDIO_RESOURCE_ADEC2 + 2);
}

static inline HAL_AUDIO_RESOURCE_T adec2res(HAL_AUDIO_ADEC_INDEX_T dec_id)
{
    if(dec_id <= HAL_AUDIO_ADEC1)
        return (HAL_AUDIO_RESOURCE_T)(dec_id + HAL_AUDIO_RESOURCE_ADEC0);
    else
        return (HAL_AUDIO_RESOURCE_T)(dec_id + HAL_AUDIO_RESOURCE_ADEC2 - 2);
}

static inline HAL_AUDIO_RESOURCE_T dp2res(HAL_AUDIO_HDMI_INDEX_T dpport)
{
    return (HAL_AUDIO_RESOURCE_T)(dpport + HAL_AUDIO_RESOURCE_DP);
}

static inline HAL_AUDIO_DP_INDEX_T res2dp(HAL_AUDIO_RESOURCE_T id)
{
    return (HAL_AUDIO_DP_INDEX_T)(id - HAL_AUDIO_RESOURCE_DP);
}

static inline HAL_AUDIO_RESOURCE_T hdmi2res(HAL_AUDIO_HDMI_INDEX_T hdmiport)
{
    return (HAL_AUDIO_RESOURCE_T)(hdmiport + HAL_AUDIO_RESOURCE_HDMI0);
}

static inline HAL_AUDIO_HDMI_INDEX_T res2hdmi(HAL_AUDIO_RESOURCE_T id)
{
    return (HAL_AUDIO_HDMI_INDEX_T)(id - HAL_AUDIO_RESOURCE_HDMI0);
}

static inline HAL_AUDIO_MIXER_INDEX_T res2amixer(HAL_AUDIO_RESOURCE_T res_id)
{
    return (HAL_AUDIO_MIXER_INDEX_T)(res_id - HAL_AUDIO_RESOURCE_MIXER0);
}

static inline HAL_AUDIO_RESOURCE_T amixer2res(HAL_AUDIO_MIXER_INDEX_T amixer_id)
{
    return (HAL_AUDIO_RESOURCE_T)(amixer_id + HAL_AUDIO_RESOURCE_MIXER0);
}

static inline BOOLEAN IsI2SINSource(HAL_AUDIO_RESOURCE_T res_id)
{
    return (res_id == HAL_AUDIO_RESOURCE_I2SIN);
}

static inline BOOLEAN IsUACINSource(HAL_AUDIO_RESOURCE_T res_id)
{
    return (res_id == HAL_AUDIO_RESOURCE_UACIN);
}

static inline BOOLEAN IsSPDIFINSource(HAL_AUDIO_RESOURCE_T res_id)
{
    return (res_id == HAL_AUDIO_RESOURCE_SPDIFIN);
}

static inline BOOLEAN IsAMIXSource(HAL_AUDIO_RESOURCE_T res_id)
{
    if((res_id <= HAL_AUDIO_RESOURCE_MIXER7) && (res_id >= HAL_AUDIO_RESOURCE_MIXER0))
        return TRUE;
    else
        return FALSE;
}

static inline BOOLEAN IsADCSource(HAL_AUDIO_RESOURCE_T res_id)
{
    return (res_id == HAL_AUDIO_RESOURCE_ADC);
}

static inline BOOLEAN IsHDMISource(HAL_AUDIO_RESOURCE_T res_id)
{
    if (res_id == HAL_AUDIO_RESOURCE_HDMI)
        return TRUE;
    else if ((res_id >= HAL_AUDIO_RESOURCE_HDMI0) && (res_id <= HAL_AUDIO_RESOURCE_SWITCH))
        return TRUE;
    else
        return FALSE;
}

static inline BOOLEAN IsDPSource(HAL_AUDIO_RESOURCE_T res_id)
{
    if ((res_id >= HAL_AUDIO_RESOURCE_DP) && (res_id <= HAL_AUDIO_RESOURCE_DP1))
        return TRUE;
    else
        return FALSE;
}

static inline BOOLEAN IsATVSource(HAL_AUDIO_RESOURCE_T res_id)
{
    return (res_id == HAL_AUDIO_RESOURCE_AAD);
}

static inline BOOLEAN IsSystemSource(HAL_AUDIO_RESOURCE_T res_id)
{
    return (res_id == HAL_AUDIO_RESOURCE_SYSTEM);
}

static inline BOOLEAN IsDTVSource(HAL_AUDIO_RESOURCE_T res_id)
{
    if(res_id == HAL_AUDIO_RESOURCE_ATP0 || res_id == HAL_AUDIO_RESOURCE_ATP1)
        return TRUE;
    else
        return FALSE;
}

static inline BOOLEAN IsAinSource(HAL_AUDIO_RESOURCE_T res_id)
{
    if(IsADCSource(res_id))
        return TRUE;
    else if(IsHDMISource(res_id))
        return TRUE;
    else if(IsATVSource(res_id))
        return TRUE;
    else if(IsDPSource(res_id))
        return TRUE;
    else if(IsI2SINSource(res_id))
        return TRUE;
    else if(IsSPDIFINSource(res_id))
        return TRUE;
    else
        return FALSE;
}

static inline BOOLEAN IsADECSource(HAL_AUDIO_RESOURCE_T res_id)
{
    if(res_id == HAL_AUDIO_RESOURCE_ADEC0 || res_id == HAL_AUDIO_RESOURCE_ADEC1
        || (res_id >= HAL_AUDIO_RESOURCE_ADEC2 && res_id <= HAL_AUDIO_RESOURCE_ADEC7))
        return TRUE;
    else
        return FALSE;
}

static inline BOOLEAN IsAOutSource(HAL_AUDIO_RESOURCE_T res_id)
{
    BOOLEAN reval;
    switch (res_id)
    {
        case HAL_AUDIO_RESOURCE_SE:
        case HAL_AUDIO_RESOURCE_OUT_SPK:
        case HAL_AUDIO_RESOURCE_OUT_SPDIF:
        case HAL_AUDIO_RESOURCE_OUT_SB_SPDIF:
        case HAL_AUDIO_RESOURCE_OUT_SB_PCM:
        case HAL_AUDIO_RESOURCE_OUT_SB_CANVAS:
        case HAL_AUDIO_RESOURCE_OUT_HP:
        case HAL_AUDIO_RESOURCE_OUT_SCART:
        case HAL_AUDIO_RESOURCE_OUT_SPDIF_ES:
            reval = TRUE;
            break;
        default:
            reval = FALSE;
            break;
    }
    return reval;
}

static inline BOOLEAN IsValidADECIpts(HAL_AUDIO_RESOURCE_T res_id)
{
    if(IsAinSource(res_id))
        return TRUE;
    else if(IsDTVSource(res_id))
        return TRUE;
    else if(IsSystemSource(res_id))
        return TRUE;
    else
        return FALSE;
}

static inline BOOLEAN IsValidAOutInstance(UINT32 index)
{
    return (Aud_Aout[index] != NULL);
}

static inline BOOLEAN IsValidADECInstance(UINT32 index)
{
    return (Aud_dec[index] != NULL);
}

static inline BOOLEAN RangeCheck(UINT32 target, UINT32 min, UINT32 max)
{
    return (target >= min && target < max);
}
/*********** End of Resource related static functions *****************************/

/**********************************************************************************
 * Static variables access functions
 **********************************************************************************/
static inline UINT32 GetDecInMute(UINT32 index)
{
    return Aud_decstatus[index].decInMute;
}

static inline UINT32 GetDecESMute(UINT32 index)
{
    return Aud_decstatus[index].spdifESMute;
}

static inline HAL_AUDIO_VOLUME_T GetDecInVolume(UINT32 index)
{
    return Aud_decstatus[index].decInVolume;
}

static inline HAL_AUDIO_VOLUME_T GetDecOutVolume(UINT32 index, UINT32 ch)
{
    return Aud_decstatus[index].decOutVolume[ch];
}

static inline void SetDecInMute(UINT32 index, UINT32 bMute)
{
    Aud_decstatus[index].decInMute = bMute;
}

static inline void SetDecInVolume(UINT32 index, HAL_AUDIO_VOLUME_T vol)
{
    Aud_decstatus[index].decInVolume = vol;
}

static inline void SetDecOutVolume(UINT32 index, UINT32 ch, HAL_AUDIO_VOLUME_T vol)
{
    Aud_decstatus[index].decOutVolume[ch] = vol;
}

static inline void SetDecESMute(UINT32 index, UINT32 bMute)
{
    Aud_decstatus[index].spdifESMute = bMute;
}

static inline void SetSPDIFOutType(UINT32 index, UINT32 type)
{
    Aud_decstatus[index].spdifouttype = type;
}

static inline void SetDecUserState(UINT32 index, UINT32 state)
{
    Aud_decstatus[index].userSetRun = state;
}
static inline UINT32 GeDectUserState(UINT32 adecIndex)
{
    return Aud_decstatus[adecIndex].userSetRun;
}

static inline UINT32 GetCurrentFormat(UINT32 adecIndex)
{
    return Aud_Adec_info[adecIndex].curAdecFormat;
}

static inline void SetDecDrcMode(UINT32 adecIndex, HAL_AUDIO_DOLBY_DRC_MODE_T mode)
{
    Aud_decstatus[adecIndex].drcMode = mode;
}

static inline HAL_AUDIO_DOLBY_DRC_MODE_T GetDecDrcMode(UINT32 adecIndex)
{
    return Aud_decstatus[adecIndex].drcMode;
}


static inline void SetDecSpeed(UINT32 adecIndex, SINT32 mode)
{
    Aud_decstatus[adecIndex].speed = mode;
}

static inline SINT32 GetDecSpeed(UINT32 adecIndex)
{
    return Aud_decstatus[adecIndex].speed;
}

static inline void SetDecDownMixMode(UINT32 adecIndex, HAL_AUDIO_DOWNMIX_MODE_T mode)
{
    Aud_decstatus[adecIndex].downmixMode = mode;
}

static inline HAL_AUDIO_DOWNMIX_MODE_T GetDecDownMixMode(UINT32 adecIndex)
{
    return Aud_decstatus[adecIndex].downmixMode;
}

static inline UINT32 GetAmixerUserMute(HAL_AUDIO_MIXER_INDEX_T index)
{
    return g_mixer_user_mute[index];
}

static inline void SetAmixerUserMute(HAL_AUDIO_MIXER_INDEX_T index, UINT32 bMute)
{
    g_mixer_user_mute[index] = bMute;
}
/*********** End of Static variables access functions ****************************/

/**********************************************************************************
 * HAL Volume and ADSP Gain related functions
 **********************************************************************************/
SINT32 Volume_Compare(HAL_AUDIO_VOLUME_T v1, HAL_AUDIO_VOLUME_T v2)
{
    if((v1.mainVol == v2.mainVol) && (v1.fineVol == v2.fineVol))
        return TRUE;
    else
        return FALSE;
}

HAL_AUDIO_VOLUME_T Volume_Add(HAL_AUDIO_VOLUME_T v1, HAL_AUDIO_VOLUME_T v2)
{
    HAL_AUDIO_VOLUME_T result;
    SINT16 mainVol, fineVol;

    mainVol = (SINT16)(v1.mainVol - 127) + (SINT16)(v2.mainVol - 127);
    fineVol = v1.fineVol + v2.fineVol;
    if(fineVol >= 16)
    {
        mainVol += 1;
        fineVol -= 16;
    }

    if(mainVol < -127)
    {
        result.mainVol = 0x0;
        result.fineVol = 0x0;
    }
    else if(mainVol > 127)
    {
        result.mainVol = 0xFF;
        result.fineVol = 0x0;
    }
    else
    {
        result.mainVol = (UINT8)mainVol + 0x7F;
        result.fineVol = (UINT8)fineVol;
    }
    return result;
}

SINT32 Volume_to_DSPGain(HAL_AUDIO_VOLUME_T volume)
{
    SINT32 dsp_gain = ENUM_AUDIO_DVOL_K0p0DB;
    /* SW dsp_gain scale: 0.25 dB
     * mainVol scale: 1 dB
     * fineVol scale: 0.0625 dB
     */
    dsp_gain += ((volume.mainVol - 127)*4);
    dsp_gain += (volume.fineVol/4);

    if(dsp_gain > ENUM_AUDIO_DVOL_K30p0DB)
    {
        dsp_gain = ENUM_AUDIO_DVOL_K30p0DB;
    }
    else if(dsp_gain < ENUM_AUDIO_DVOL_KMINUS72p0DB)
    {
        dsp_gain = ENUM_AUDIO_DVOL_KMINUS72p0DB;
    }
    return dsp_gain;
}

UINT32 Volume_to_MixerGain(HAL_AUDIO_VOLUME_T volume)
{
    SINT32 Adsp_gainume_dB = (SINT32)volume.mainVol-0x7F;
    if(Adsp_gainume_dB > 0)
    {
        return dB2mixgain_volumeid_table[0]; //return 0 dB volumeid
    }
    else if(Adsp_gainume_dB <= -DB2MIXGAIN_TABLE_SIZE) /* under -51dB */
    {
        return dB2mixgain_volumeid_table[32]; //return -32 dB volumeid
    }
    else
    {
        return dB2mixgain_volumeid_table[(-Adsp_gainume_dB)];
    }
}

// inputConnect -> current Connect
static void CleanConnectInputSourceAndStatus(HAL_AUDIO_MODULE_STATUS resourceStatus[HAL_MAX_RESOURCE_NUM],
                                             HAL_AUDIO_RESOURCE_T currentConnect,
                                             HAL_AUDIO_RESOURCE_T inputConnect)
{
    SINT32 i, index;

    // current  module 's input pin
    index = resourceStatus[currentConnect].numIptconnect;

    if(index <=  0)
    {
        AUDIO_FATAL("Inputs of %s = (%d)\n", GetResourceString(currentConnect), index);
        return;
    }

    for( i = 0; i < resourceStatus[currentConnect].numIptconnect; i++)
    {
        if(resourceStatus[currentConnect].inputConnectResourceId[i] == inputConnect)
        {
            break;
        }
    }

    for(; i < (resourceStatus[currentConnect].numIptconnect -1); i++)
    {
        resourceStatus[currentConnect].inputConnectResourceId[i] = resourceStatus[currentConnect].inputConnectResourceId[i+1];
        resourceStatus[currentConnect].connectStatus[i] = resourceStatus[currentConnect].connectStatus[i+1];
    }

    if(resourceStatus[currentConnect].numIptconnect <= 0)
    {
        ERROR( "%s  %d %d\n", __func__, currentConnect, inputConnect);
    }

    resourceStatus[currentConnect].inputConnectResourceId[index-1] = HAL_AUDIO_RESOURCE_NO_CONNECTION;
    resourceStatus[currentConnect].connectStatus[index-1] = HAL_AUDIO_RESOURCE_DISCONNECT;
    resourceStatus[currentConnect].numIptconnect--;

    if(HAL_AUDIO_RESOURCE_NO_CONNECTION == inputConnect) // AMXI no connectinput
        return;

    // input module 's output pin
    index = resourceStatus[inputConnect].numOptconnect;

    if(index <=  0)
    {
        AUDIO_FATAL("Outputs of %s = (%d)\n", GetResourceString(inputConnect), index);
        AUDIO_FATAL("[%s] %s -> %s\n", __func__,
                GetResourceString(inputConnect), GetResourceString(currentConnect));
        return ;//  input module has no output pin
    }

    for( i = 0; i < resourceStatus[inputConnect].numOptconnect; i++)
    {
        if(resourceStatus[inputConnect].outputConnectResourceID[i] == currentConnect)
        {
            break;
        }
    }

    for(; i < (resourceStatus[inputConnect].numOptconnect -1); i++)
    {
        resourceStatus[inputConnect].outputConnectResourceID[i] = resourceStatus[inputConnect].outputConnectResourceID[i+1];
    }

    resourceStatus[inputConnect].outputConnectResourceID[index -1] = HAL_AUDIO_RESOURCE_NO_CONNECTION;
    resourceStatus[inputConnect].numOptconnect--;
}

static SINT32 GetConnectInputSourceIndex(HAL_AUDIO_MODULE_STATUS resourceStatus, HAL_AUDIO_RESOURCE_T searchId)
{
    SINT32 i;

    // check if reconnect
    for(i = 0; i< resourceStatus.numIptconnect; i++)
    {
        if(resourceStatus.inputConnectResourceId[i] == searchId) // at connect status
        {
            return i;
        }
    }

    return -1;
}

static BOOLEAN inline SetResourceOpen(HAL_AUDIO_RESOURCE_T id)
{
    SINT32 i = 0;

    // check all status is correct
    for(i = 0; i < HAL_MAX_OUTPUT; i++)
    {
        if((ResourceStatus[id].connectStatus[i] != HAL_AUDIO_RESOURCE_CLOSE)&&
            (ResourceStatus[id].connectStatus[i] != HAL_AUDIO_RESOURCE_OPEN))
        {
            ERROR("open %s failed status is at %s",
                    GetResourceString(id), GetResourceStatusString(ResourceStatus[id].connectStatus[0]));
            return FALSE;
        }
    }

    for(i = 0; i < HAL_MAX_OUTPUT; i++)
    {
        ResourceStatus[id].connectStatus[i] = HAL_AUDIO_RESOURCE_OPEN;
    }

    return TRUE;
}

static BOOLEAN inline SetResourceDisconnect(HAL_AUDIO_RESOURCE_T id, HAL_AUDIO_RESOURCE_T inputConnect)
{
    SINT32 i;

    if((i = GetConnectInputSourceIndex(ResourceStatus[id], inputConnect)) == -1)
    {
        ERROR("(%s)has no pin connected to %s", GetResourceString(id),  GetResourceString(inputConnect));
        return FALSE;
    }

    if((ResourceStatus[id].connectStatus[i] != HAL_AUDIO_RESOURCE_CONNECT) &&
       (ResourceStatus[id].connectStatus[i] != HAL_AUDIO_RESOURCE_STOP))
    {
        ERROR("%s disconnect pin id %s failed status is at %s",GetResourceString(id),
                    GetResourceString(ResourceStatus[id].inputConnectResourceId[i]), GetResourceStatusString((SINT32)ResourceStatus[id].connectStatus[i]));
    }

    CleanConnectInputSourceAndStatus(ResourceStatus, id, inputConnect);

    return TRUE;
}

static void SetConnectSourceAndStatus(HAL_AUDIO_MODULE_STATUS resourceStatus[HAL_MAX_RESOURCE_NUM],
                                      HAL_AUDIO_RESOURCE_T currentConnect,
                                      HAL_AUDIO_RESOURCE_T inputConnect)
{
    SINT32 i, index;

    // set current module's input id
    index = resourceStatus[currentConnect].numIptconnect;

    // check if has been connected
    i = GetConnectInputSourceIndex(resourceStatus[currentConnect], inputConnect);
    resourceStatus[currentConnect].inputConnectResourceId[index] = inputConnect;
    resourceStatus[currentConnect].connectStatus[index]          = HAL_AUDIO_RESOURCE_CONNECT;
    resourceStatus[currentConnect].numIptconnect++;

    // set input module's output Id
    if(HAL_AUDIO_RESOURCE_NO_CONNECTION == inputConnect)
        return;

    index = resourceStatus[inputConnect].numOptconnect;

    if(index >= HAL_DEC_MAX_OUTPUT_NUM)
    {
        AUDIO_FATAL("Outputs of %s = (%d)\n", GetResourceString(inputConnect), index);
        return;
    }
    resourceStatus[inputConnect].outputConnectResourceID[index] = currentConnect;
    resourceStatus[inputConnect].numOptconnect++;

    return;
}

static void GetConnectOutputSource(HAL_AUDIO_RESOURCE_T resID,
                                   HAL_AUDIO_RESOURCE_T* outputConnectResId,
                                   SINT32 numOutputConnectArray,
                                   SINT32* totalOutputConnectResource)
{
    SINT32 minArraySize;
    SINT32 i;

    *totalOutputConnectResource = ResourceStatus[resID].numOptconnect;

    if(numOutputConnectArray <= *totalOutputConnectResource)
        minArraySize = numOutputConnectArray;
    else
        minArraySize = *totalOutputConnectResource;

    for(i = 0; i < minArraySize; i++)
    {
        outputConnectResId[i] = ResourceStatus[resID].outputConnectResourceID[i];
    }
    return;
}

AUDIO_DEC_TYPE Convert2AudioDecType(HAL_AUDIO_SRC_TYPE_T audioSrcType)
{
    AUDIO_DEC_TYPE reType = AUDIO_UNKNOWN_TYPE;
    switch(audioSrcType)
    {
        default:
        case HAL_AUDIO_SRC_TYPE_UNKNOWN:
        case HAL_AUDIO_SRC_TYPE_NONE:
            ERROR("unknow type %x\n", audioSrcType);
            break;

        case HAL_AUDIO_SRC_TYPE_PCM: // need other infomation
            ERROR("unknow type %x\n", audioSrcType);
            break;

        case HAL_AUDIO_SRC_TYPE_AC3:
            reType = AUDIO_AC3_DECODER_TYPE;
            break;

        case HAL_AUDIO_SRC_TYPE_EAC3:
        case HAL_AUDIO_SRC_TYPE_EAC3_ATMOS:
            reType = AUDIO_DDP_DECODER_TYPE;
            break;

        case HAL_AUDIO_SRC_TYPE_MP3:
        case HAL_AUDIO_SRC_TYPE_MPEG:
            reType = AUDIO_MPEG_DECODER_TYPE;
            break;

        case HAL_AUDIO_SRC_TYPE_AAC:
        case HAL_AUDIO_SRC_TYPE_HEAAC:
            reType = AUDIO_AAC_DECODER_TYPE; // LATM , ADTS ?
            break;

        case HAL_AUDIO_SRC_TYPE_DRA:
            reType = AUDIO_DRA_DECODER_TYPE;
            break;

        case HAL_AUDIO_SRC_TYPE_MPEG_H:
            ERROR("not yet implemnet type %x\n", audioSrcType);
            break;

        case HAL_AUDIO_SRC_TYPE_AC4:
        case HAL_AUDIO_SRC_TYPE_AC4_ATMOS:
            reType = AUDIO_AC4_DECODER_TYPE;
            break;

        case HAL_AUDIO_SRC_TYPE_DTS:
            reType = AUDIO_DTS_DECODER_TYPE; // tv030 need to support DTS
            break;

        case HAL_AUDIO_SRC_TYPE_DTS_HD_MA:
            ERROR("not yet implemnet type %x\n", audioSrcType);
            break;

         case HAL_AUDIO_SRC_TYPE_DTS_EXPRESS:
            ERROR("not yet implemnet type %x\n", audioSrcType);
            break;

         case HAL_AUDIO_SRC_TYPE_DTS_CD:
            ERROR("not yet implemnet type %x\n", audioSrcType);
            break;
    }

    return reType;
}

HAL_AUDIO_RESOURCE_T GetNonOutputModuleSingleInputResource(HAL_AUDIO_MODULE_STATUS resourceStatus)
{
    return resourceStatus.inputConnectResourceId[0];
}

void FillDecInput(HAL_AUDIO_RESOURCE_T adecId)
{
    HAL_AUDIO_RESOURCE_T decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[adecId]);

    if(decIptResId < HAL_MAX_RESOURCE_NUM)
    {
        HAL_AUDIO_RESOURCE_T sdecResId = GetNonOutputModuleSingleInputResource(ResourceStatus[decIptResId]);

        // drow pipe 1
        if(sdecResId < HAL_MAX_RESOURCE_NUM)
        {
            strncat(AUD_StringBuffer, GetResourceString(sdecResId), sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
            strncat(AUD_StringBuffer, "---", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
        }

        strncat(AUD_StringBuffer, GetResourceString(decIptResId), sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
        strncat(AUD_StringBuffer, "---", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);

        // draw pipe 3
        strncat(AUD_StringBuffer, GetResourceString(adecId), sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
    }
    else
    {
        strncat(AUD_StringBuffer, "no input---", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
        strncat(AUD_StringBuffer, GetResourceString(adecId), sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
    }
}

void ShowFlow(HAL_AUDIO_RESOURCE_T adecResId, HAL_AUDIO_RESOURCE_STATUS newStatus, SINT32 forcePrint)
{
    SINT32 i;
    SINT32 mainDecResId;
    HAL_AUDIO_RESOURCE_T decId[2];

    mainDecResId = adec2res((HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex);

    decId[0] = HAL_AUDIO_RESOURCE_ADEC0;
    decId[1] = HAL_AUDIO_RESOURCE_ADEC1;
    for(i = 0; i < 2; i++)
    {
        SINT32 DecResId =  decId[i];
        memset(AUD_StringBuffer, 0, sizeof(AUD_StringBuffer));// clean
        if(mainDecResId == DecResId)
        {
            strncat(AUD_StringBuffer, "Main:: ", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
        }
        else
        {
            if(mainDecResId != -1)
            {
                strncat(AUD_StringBuffer, " Sub:: ", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
            }
            else
            {
                strncat(AUD_StringBuffer, "Unknown Main ID:: ", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
            }
        }

        FillDecInput(decId[i]);

        if(ResourceStatus[decId[i]].connectStatus[0] == HAL_AUDIO_RESOURCE_RUN)
        {
            if(decId[i] == adecResId)
            {
                if(newStatus == HAL_AUDIO_RESOURCE_RUN)
                    strncat(AUD_StringBuffer, "(Run  to  Run)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else if(newStatus == HAL_AUDIO_RESOURCE_STOP)
                    strncat(AUD_StringBuffer, "(Run  to  Stop)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else if(newStatus == HAL_AUDIO_RESOURCE_PAUSE)
                    strncat(AUD_StringBuffer, "(Run  to  Pause)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else
                    strncat(AUD_StringBuffer, "(Running State)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
            }
            else
                strncat(AUD_StringBuffer, "(Running State)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
        }
        else if(ResourceStatus[decId[i]].connectStatus[0] == HAL_AUDIO_RESOURCE_PAUSE)
        {
            if(decId[i] == adecResId)
            {
                if(newStatus == HAL_AUDIO_RESOURCE_RUN)
                    strncat(AUD_StringBuffer, "(Pause  to Run)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else if(newStatus == HAL_AUDIO_RESOURCE_STOP)
                    strncat(AUD_StringBuffer, "(Pause  to  Stop)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else if(newStatus == HAL_AUDIO_RESOURCE_PAUSE)
                    strncat(AUD_StringBuffer, "(Pause  to  Pause)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else
                    strncat(AUD_StringBuffer, "(Pause   State)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
            }
            else
                strncat(AUD_StringBuffer, "(Pause   State)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
        }
        else
        {
            if(decId[i] == adecResId)
            {
                if(newStatus == HAL_AUDIO_RESOURCE_RUN)
                    strncat(AUD_StringBuffer, "(Stop  to  Run)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else if(newStatus == HAL_AUDIO_RESOURCE_STOP)
                    strncat(AUD_StringBuffer, "(Stop  to  Stop)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else if(newStatus == HAL_AUDIO_RESOURCE_PAUSE)
                    strncat(AUD_StringBuffer, "(Stop  to  Pause)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
                else
                    strncat(AUD_StringBuffer, "(Stop    State)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
            }
            else
                strncat(AUD_StringBuffer, "(Stop    State)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);

            if(GeDectUserState(i) == 1)
                strncat(AUD_StringBuffer, "(MiddleWare setting is Run)", sizeof(AUD_StringBuffer) - strlen(AUD_StringBuffer) - 1);
        }
        if(forcePrint)
            INFO("%s\n", AUD_StringBuffer);
        else
            INFO("%s\n", AUD_StringBuffer);
    }
}

static void GetRingBufferInfo(RINGBUFFER_HEADER* pRingBuffer, ULONG* pbase,
    ULONG* prp, ULONG* pwp , ULONG* psize
    )
{
    ULONG base, rp, wp, size;

    INFO("ring buffer %p ", pRingBuffer);
    if(pRingBuffer == NULL)
        return;

    rp   = IPC_ReadU32((BYTE*) &(pRingBuffer->readPtr[0]));
    base = IPC_ReadU32((BYTE*) &(pRingBuffer->beginAddr));
    wp   = IPC_ReadU32((BYTE*) &(pRingBuffer->writePtr));
    size  = IPC_ReadU32((BYTE*) &(pRingBuffer->size));
    INFO(" baddr %x rp %x wp %x size %x",  base, rp, wp, size);
    //assert(rp < (base) + (size));//rhal do
    //assert(wp < (base) + (size));//hhal do

    if(pbase)
        *pbase = base;

    if(pwp)
        *pwp = wp;

    if(prp)
        *prp = rp;

    if(psize)
        *psize = size;
}

DTV_STATUS_T AUDIO_Get_MediaSourceLatency(AUDIO_LATENCY_INFO *latency_info)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 ret = 0;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id  = AUDIO_CONFIG_CMD_GET_LATENCY;
    audioConfig.value[0] = latency_info->iptSrc;
    audioConfig.value[1] = latency_info->sndOut;

    ret = KADP_AUDIO_AudioConfig(&audioConfig);
    if(ret != 0){
        ERROR("[%s %d] fail\n",__func__,__LINE__);
        return NOT_OK;
    }

    latency_info->latency = audioConfig.value[2];

    return OK;
}

HAL_AUDIO_SRC_TYPE_T Convert2HalAudioDecType(AUDIO_DEC_TYPE audioSrcType, SLONG reserved, SLONG virtualizer_mode)
{
    HAL_AUDIO_SRC_TYPE_T  src_type = HAL_AUDIO_SRC_TYPE_UNKNOWN;
    SINT32 version = 0;
    UINT32 T1_CC;
    UINT32 strm_type;
    UINT32 nuframe_type;
    switch(audioSrcType)
    {
        default:
        case AUDIO_UNKNOWN_TYPE:
            break;
        case AUDIO_MPEG_DECODER_TYPE:
            src_type = HAL_AUDIO_SRC_TYPE_MPEG;
            break;
        case AUDIO_AC3_DECODER_TYPE:
            src_type = HAL_AUDIO_SRC_TYPE_AC3;
            break;
        case AUDIO_LPCM_DECODER_TYPE:
             src_type = HAL_AUDIO_SRC_TYPE_PCM;
            break;
        case AUDIO_DTS_DECODER_TYPE:
        case AUDIO_DTS_HD_DECODER_TYPE:
            // reserved[1]: 0x80=LBR, 0x100=XLL reference from dtshd_dec_api_common.h
            /*
            typedef enum DTS_DECINFO_STRMTYPE_MASK {               DTS Bitstream Type         OSD Display
                DTSXDECSTRMTYPE_UNKNOWN              = 0x00000000, DTS:X (P2)                 DTS:X
                DTSXDECSTRMTYPE_DTS_LEGACY           = 0x00000001, DTS Core                   DTS
                DTSXDECSTRMTYPE_DTS_ES_MATRIX        = 0x00000002, DTS-ES Matrix              DTS
                DTSXDECSTRMTYPE_DTS_ES_DISCRETE      = 0x00000004, DTS-ES Discrete            DTS
                DTSXDECSTRMTYPE_DTS_9624             = 0x00000008, DTS 96/24                  DTS
                DTSXDECSTRMTYPE_DTS_ES_8CH_DISCRETE  = 0x00000010, DTS-ES Discrete            DTS
                DTSXDECSTRMTYPE_DTS_HIRES            = 0x00000020, DTS-HD HiRes               DTS-HD
                DTSXDECSTRMTYPE_DTS_MA               = 0x00000040, DTS-HD Master Audio        DTS-HD
                DTSXDECSTRMTYPE_DTS_LBR              = 0x00000080, DTS Express                DTS-HD
                DTSXDECSTRMTYPE_DTS_LOSSLESS         = 0x00000100, DTS-HD Master Audio        DTS-HD
                DTSXDECSTRMTYPE_DTS_UHD              = 0x00000200, DTS:X (P1)                 DTS:X
                DTSXDECSTRMTYPE_DTS_UHD_MA           = 0x00000400, DTS:X Master Audio (P1)    DTS:X
                DTSXDECSTRMTYPE_DTS_UHD_GAME         = 0x00000800  DTS:X (P1)                 DTS:X
            } DTSXDECSTRMTYPE;
            */
            T1_CC = (reserved & 0xf0000000) >> 28;
            nuframe_type = (reserved & 0x0f000000) >> 24;
            strm_type = reserved & 0x00ffffff;
            //INFO("%s T1_CC=%d, nuframe_type=%d, strm_type=%d\n", __func__, T1_CC, nuframe_type, strm_type);

            if(strm_type == 0x20 || strm_type == 0x80) {
                src_type = HAL_AUDIO_SRC_TYPE_DTS_EXPRESS;
            } else if(strm_type == 0x40 || strm_type == 0x100) {
                src_type = HAL_AUDIO_SRC_TYPE_DTS_HD_MA;
            } else if((strm_type == 0 && nuframe_type == DTS_PARSER_SYNC_TYPE_FTOC )
                || strm_type == 0x200 || strm_type == 0x400 || strm_type == 0x800) {
                src_type = HAL_AUDIO_SRC_TYPE_DTS_X;
            } else {
                src_type = HAL_AUDIO_SRC_TYPE_DTS;
            }
            break;
        case AUDIO_WMA_DECODER_TYPE:
            break;
        case AUDIO_AAC_DECODER_TYPE:
            //-- reserved[1] = ((VERSION<<8) & 0xFF00) | (FORMAT & 0x00FF)
            // bit [0:7]  for format  /* LOAS/LATM = 0x0, ADTS = 0x1 */
            // bit [8:15]  for version   /* AAC = 0x0, HE-AACv1 = 0x1, HE-AACv2 = 0x2 */
            version =  (reserved >> 8) & 0xFF ;
             if(version != 0)
                src_type = HAL_AUDIO_SRC_TYPE_HEAAC;
             else
                src_type = HAL_AUDIO_SRC_TYPE_AAC;
            break;
        case AUDIO_VORBIS_DECODER_TYPE:
            break;
        case AUDIO_DV_DECODER_TYPE:
            break;
        case AUDIO_DDP_DECODER_TYPE:
            version = (reserved & 0x1) && (virtualizer_mode != 0);
            if (version) {
                src_type = HAL_AUDIO_SRC_TYPE_EAC3_ATMOS;
            } else {
                src_type = HAL_AUDIO_SRC_TYPE_EAC3;
            }
            break;
        case AUDIO_AC4_DECODER_TYPE:
            version = (reserved & 0x1) && (virtualizer_mode != 0);
            if (version) {
                src_type = HAL_AUDIO_SRC_TYPE_AC4_ATMOS;
            } else {
                src_type = HAL_AUDIO_SRC_TYPE_AC4;
            }
            break;
        case AUDIO_MLP_DECODER_TYPE:
        case AUDIO_MAT_DECODER_TYPE:
            version = (reserved & 0x1) && (virtualizer_mode != 0);
            if(version)
                src_type = HAL_AUDIO_SRC_TYPE_MAT_ATMOS;
            else
                src_type = HAL_AUDIO_SRC_TYPE_MAT;
            break;
        case AUDIO_WMA_PRO_DECODER_TYPE:
            break;
        case AUDIO_MP4AAC_DECODER_TYPE:
            break;
        case AUDIO_MP3_PRO_DECODER_TYPE:
            break;
        case AUDIO_MP4HEAAC_DECODER_TYPE:
            break;
        case AUDIO_RAW_AAC_DECODER_TYPE:
            break;
        case AUDIO_RA1_DECODER_TYPE:
            break;
        case AUDIO_RA2_DECODER_TYPE:
            break;
        case AUDIO_ATRAC3_DECODER_TYPE:
            break;
        case AUDIO_COOK_DECODER_TYPE:
            break;
        case AUDIO_LSD_DECODER_TYPE:
            break;
        case AUDIO_ADPCM_DECODER_TYPE:
            break;
        case AUDIO_FLAC_DECODER_TYPE:
            break;
        case AUDIO_ULAW_DECODER_TYPE:
            break;
        case AUDIO_ALAW_DECODER_TYPE:
            break;
        case AUDIO_ALAC_DECODER_TYPE:
            break;
        case AUDIO_DTS_HIGH_RESOLUTION_DECODER_TYPE:
            break;
        case AUDIO_DTS_LBR_DECODER_TYPE:
            break;
        case AUDIO_DTS_MASTER_AUDIO_DECODER_TYPE:
            break;
        case AUDIO_AMRNB_DECODER_TYPE:
            break;
        case AUDIO_MIDI_DECODER_TYPE:
            break;
        case AUDIO_APE_DECODER_TYPE:
            break;
        case AUDIO_AVS_DECODER_TYPE:
            break;
        case AUDIO_NELLYMOSER_DECODER_TYPE:
            break;
        case AUDIO_WMA_LOSSLESS_DECODER_TYPE:
            break;
        case AUDIO_UNCERTAIN_DECODER_TYPE:
            break;
        case AUDIO_UNCERTAIN_HDMV_DECODER_TYPE:
            break;
        case AUDIO_ILBC_DECODER_TYPE:
            break;
        case AUDIO_SILK_DECODER_TYPE:
            break;
        case AUDIO_AMRWB_DECODER_TYPE:
            break;
        case AUDIO_G729_DECODER_TYPE:
            break;
        case AUDIO_DRA_DECODER_TYPE:
             src_type = HAL_AUDIO_SRC_TYPE_DRA;
            break;
        case AUDIO_OPUS_DECODER_TYPE:
            break;
    }

   return src_type;
}

static BOOLEAN inline SetResourceConnect(HAL_AUDIO_RESOURCE_T id, HAL_AUDIO_RESOURCE_T inputConnect)
{
    SINT32 i;
    SINT32 useInputPinId;

    if(IsAOutSource(id)) //  connect according to pin
    {
        // check nerver been connected
        if((i = GetConnectInputSourceIndex(ResourceStatus[id], inputConnect)) != -1)
        {
            ERROR("%s pin %d has connected to %s\n",
                    GetResourceString(id), i, GetResourceString(inputConnect));
            return FALSE;
        }
    }
    else
    {
        // normal case only need to check  connectStatus[0]
        if((ResourceStatus[id].connectStatus[0] != HAL_AUDIO_RESOURCE_OPEN) &&
           (ResourceStatus[id].connectStatus[0] != HAL_AUDIO_RESOURCE_DISCONNECT))
        {
            ERROR("connect %s failed status is %d\n",
                    GetResourceString(id), ResourceStatus[id].connectStatus[0]);
            return FALSE;
        }
    }

    useInputPinId = ResourceStatus[id].numIptconnect;
    SetConnectSourceAndStatus(ResourceStatus, id, inputConnect);

    return TRUE;
}

static BOOLEAN inline SetResourceClose(HAL_AUDIO_RESOURCE_T id)
{
    SINT32 i, statusFailed;

    statusFailed = 0;
    // check all status is at right status
    for(i = 0; i < HAL_MAX_OUTPUT; i++)
    {
        if((ResourceStatus[id].connectStatus[i] != HAL_AUDIO_RESOURCE_OPEN) &&
           (ResourceStatus[id].connectStatus[i] != HAL_AUDIO_RESOURCE_DISCONNECT) &&
           (ResourceStatus[id].connectStatus[i] != HAL_AUDIO_RESOURCE_CLOSE))
        {
            ERROR("close %s failed input pin (%d) status is %s\n",
                    GetResourceString(id), i, GetResourceStatusString(ResourceStatus[id].connectStatus[i]));

            if(ResourceStatus[id].connectStatus[i] == HAL_AUDIO_RESOURCE_CONNECT)
            {
                ERROR("close %s failed input pin (%d) still connect to  %s\n",
                    GetResourceString(id), i, GetResourceString(ResourceStatus[id].inputConnectResourceId[i]));
            }

            statusFailed = 1;
        }
    }

    if(statusFailed )
        return FALSE;

    for(i = 0; i < HAL_MAX_OUTPUT; i++)
    {
        ResourceStatus[id].connectStatus[i] = HAL_AUDIO_RESOURCE_CLOSE;
    }

    return TRUE;
}

void DeleteDbxTvTable(void)
{
    if (dbxTableAddr != NULL) {
        rtkaudio_free(dbxTablePhyAddr);
        dbxTableAddr = NULL;
    }
}

void Init_ATV_Driver(HAL_AUDIO_SIF_TYPE_T eSifType)
{
    KADP_AUDIO_HW_IOCT_Init(AUDIO_USER_CONFIG_TV003);

    KADP_Audio_HwpAADSIFInit(NULL);//check initial value....
    if (rhal_type == RHAL_LITE)
        g_AudioSIFInfo.curSifType = HAL_AUDIO_SIF_DVB_CN_SELECT;
    else g_AudioSIFInfo.curSifType = eSifType;
    KADP_Audio_HwpCurSifType( (ATV_AUDIO_SIF_TYPE_T) eSifType);
    KADP_Audio_AtvSetDeviationMethod(ATV_DEV_CHANGE_BY_USER);
    KADP_Audio_AtvEnableAutoChangeSoundModeFlag(1);//enable
}

void DeInit_ATV_Driver(void)
{
    KADP_Audio_AtvInit(NULL);//check initial value....
    g_AudioSIFInfo.curSifType     = HAL_AUDIO_SIF_TYPE_NONE;
    KADP_Audio_HwpCurSifType( (ATV_AUDIO_SIF_TYPE_T) g_AudioSIFInfo.curSifType);
    KADP_Audio_HwpAADSIFFinalize();
}

void DeInit_Audio_Driver(void)
{
	DeleteDbxTvTable();
}

static void InitialResourceStatus(void)
{
    SINT32 i,j;

    for(i =0; i < HAL_MAX_RESOURCE_NUM; i++)
    {
        memcpy(ResourceStatus[i].name, ResourceName[i], strlen(ResourceName[i]));
        for(j=0; j < HAL_MAX_OUTPUT; j++)
        {
            ResourceStatus[i].inputConnectResourceId[j] = HAL_AUDIO_RESOURCE_NO_CONNECTION;
            ResourceStatus[i].outputConnectResourceID[j] = HAL_AUDIO_RESOURCE_NO_CONNECTION;
            ResourceStatus[i].connectStatus[j] = HAL_AUDIO_RESOURCE_CLOSE;
        }

        ResourceStatus[i].numOptconnect = 0;
        ResourceStatus[i].numIptconnect = 0;

        if(IsAOutSource((HAL_AUDIO_RESOURCE_T)i))
            ResourceStatus[i].maxVaildIptNum = 10;//adec 0 , 1, amix0~7
        else
            ResourceStatus[i].maxVaildIptNum = 1; // normal module
    }

    Aud_mainDecIndex = 0;
    Aud_prevMainDecIndex = -1;
    Aud_descriptionMode = 0;

    for(i =0; i < AUD_ADEC_MAX; i++)
    {
        memset(&FlowStatus[i], 0, sizeof(HAL_AUDIO_FLOW_STATUS));
        memset(&Aud_Adec_info[i], 0,  sizeof(HAL_AUDIO_ADEC_INFO_T));
    }

    memset(&g_AudioStatusInfo,0, sizeof(g_AudioStatusInfo));

    for(i =0; i < AUD_ADEC_MAX; i++)
    {
        SetDecDrcMode(i, HAL_AUDIO_DOLBY_RF_MODE);// initial
        SetDecDownMixMode(i, HAL_AUDIO_LORO_MODE);
        SetDecSpeed(i, 256);
    }
}

static void InitialADCStatus(void)
{
    SINT32 i;
    for(i = 0; i < MAIN_AIN_ADC_PIN; i++)
    {
        AinStatus.ainPinStatus[i] = HAL_AUDIO_RESOURCE_CLOSE;
    }
}

static void InitialDecStatus(void)
{
    SINT32 i;
    SINT32 j;
    HAL_AUDIO_VOLUME_T default_volume = {.mainVol = 0x7F, .fineVol = 0x0};
    for(i = 0; i < AUD_ADEC_MAX; i++)
    {
        SetSPDIFOutType(i, HAL_AUDIO_SPDIF_PCM);
        SetDecInMute(i, FALSE);
        SetDecESMute(i, FALSE);
        SetDecInVolume(i, default_volume);
        for(j = 0; j < AUD_MAX_CHANNEL; j++)
            SetDecOutVolume(i, j, default_volume);
    }
}

static UINT32 ADSP_TSK_GetStarted(void)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_GET_TASK_STARTED;
    return rtkaudio_send_audio_config(&audioConfig);
}

static DTV_STATUS_T CreateAINFilter(void)
{
    Aud_MainAin = new_AIN(0);
    if(Aud_MainAin == NULL)
    {
        ERROR("create main AIN failed\n");
        return NOT_OK;
    }
    Aud_Ain[0] = Aud_MainAin;

    return OK;
}

static DTV_STATUS_T CreateAINFilterQuickShow(QuickShowCreateParam param)
{
    Aud_MainAin = new_AIN_QuickShow(0, param);
    if(Aud_MainAin == NULL)
    {
        ERROR("create main AIN failed\n");
        return NOT_OK;
    }
    Aud_Ain[0] = Aud_MainAin;

    return OK;
}

static void DeleteAINFilter(void)
{
    UINT32 i;
    for(i = 0; i < AUD_AIN_MAX; i++)
    {
        if(Aud_Ain[i] != NULL)
        {
            Aud_Ain[i]->Stop(Aud_Ain[i]);
            Aud_Ain[i]->Delete(Aud_Ain[i]);
            Aud_Ain[i] = NULL;
        }
    }

    if(Aud_MainAin != NULL){
        Aud_MainAin = NULL;
    }
}

static DTV_STATUS_T CreateFlowManagerFilter(void)
{
    UINT32 i;
    for (i = 0; i < AUD_ADEC_INIT_MAX; i++)
    {
        if(Aud_flow[i] == NULL)
        {
            Aud_flow[i] = new_flow();
            if(Aud_flow[i] == NULL)
            {
                ERROR("create audio flow DEC%d failed\n", i);
                return NOT_OK;
            }
        }
    }
    return OK;
}

static void DeleteFlowManagerFilter(void)
{
    UINT32 i;
    for(i = 0; i < AUD_ADEC_MAX; i++)
    {
        if(Aud_flow[i] != NULL)
        {
            Aud_flow[i]->Stop(Aud_flow[i]);
            Aud_flow[i]->Delete(Aud_flow[i]);
            Aud_flow[i] = NULL;
        }
    }
}

static DTV_STATUS_T CreatePPAOFilter(void)
{
    if(Aud_Aout[0] == NULL)
    {
        Aud_Aout[0] = new_PPAO(PPAO_FULL, 0);
        if(Aud_Aout[0] == NULL)
        {
            ERROR("create audio ppao failed\n");
            return NOT_OK;
        }
    }

    if(Aud_Aout[1] == NULL)
    {
        Aud_Aout[1] = new_PPAO(PPAO_FULL, 1);
        if(Aud_Aout[1] == NULL)
        {
            ERROR("create audio sub ppao failed\n");
            return NOT_OK;
        }
    }
    return OK;
}

static DTV_STATUS_T CreatePPAOFilterQuickShow(QuickShowCreateParam param)
{
    if(Aud_Aout[0] == NULL)
    {
        Aud_Aout[0] = new_PPAO_QuickShow(PPAO_FULL, param);
        if(Aud_Aout[0] == NULL)
        {
            ERROR("create audio ppao failed\n");
            return NOT_OK;
        }
    }

    if(Aud_Aout[1] == NULL)
    {
        Aud_Aout[1] = new_PPAO_QuickShow(PPAO_FULL, param);
        if(Aud_Aout[1] == NULL)
        {
            ERROR("create audio sub ppao failed\n");
            return NOT_OK;
        }
    }
    return OK;
}

static DTV_STATUS_T DeletePPAOFilter(void)
{
    UINT32 i;
    for(i = 0; i < AUD_AOUT_MAX; i++)
    {
        if(Aud_Aout[i] != NULL)
        {
            Aud_Aout[i]->Stop(Aud_Aout[i]);
            Aud_Aout[i]->Delete(Aud_Aout[i]);
            Aud_Aout[i] = NULL;
        }
    }

    return OK;
}

static DTV_STATUS_T CreateDecFilter(void)
{
    UINT32 i;
    for(i = 0; i < AUD_ADEC_INIT_MAX; i++)
    {
        if(Aud_dec[i] == NULL)
        {
            Aud_dec[i] = new_DEC(i);
            if(Aud_dec[i] == NULL)
            {
                ERROR("create audio dec %d filter failed\n", i);
                return NOT_OK;
            }
        }

        Aud_DTV[i] = new_DtvCom();
        if(Aud_DTV[i] == NULL)
        {
            ERROR("create audio dtv dec %d filter failed\n", i);
            return NOT_OK;
        }
    }

    return OK;
}

static DTV_STATUS_T CreateDecFilterQuickShow(QuickShowCreateParam param)
{
    UINT32 i;
    for(i = 0; i < AUD_ADEC_INIT_MAX; i++)
    {
        if(Aud_dec[i] == NULL)
        {
            if(i == 0)
                Aud_dec[i] = new_DEC_QuickShow(param);
            else
                Aud_dec[i] = new_DEC(i);

            if(Aud_dec[i] == NULL)
            {
                ERROR("create QuickShow audio dec %d filter failed\n", i);
                return NOT_OK;
            }
        }

        Aud_DTV[i] = new_DtvCom();
        if(Aud_DTV[i] == NULL)
        {
            ERROR("create QuickShow audio dtv dec %d filter failed\n", i);
            return NOT_OK;
        }
    }

    return OK;
}

static void DeleteDecFilter(void)
{
    UINT32 i;
    for(i = 0; i < AUD_ADEC_MAX; i++)
    {
        if(Aud_dec[i] != NULL)
        {
            Aud_dec[i]->Stop(Aud_dec[i]);
            Aud_dec[i]->Delete(Aud_dec[i]);
            Aud_dec[i] = NULL;
        }

        if(Aud_DTV[i])
        {
            Aud_DTV[i]->Stop(Aud_DTV[i]);
            Aud_DTV[i]->Delete(Aud_DTV[i]);
        }
        Aud_DTV[i] = NULL;
    }
}

SINT32 GetCurrentADCConnectPin(void)
{
    SINT32 revalue = -1;
    SINT32 i;

    for(i = 0; i < MAIN_AIN_ADC_PIN; i++)
    {
        INFO("ADC pin %d is at  %s state\n", i, GetResourceStatusString(AinStatus.ainPinStatus[i]));
        if(AinStatus.ainPinStatus[i] == HAL_AUDIO_RESOURCE_CONNECT)
        {
            if(revalue != -1)
            {
                ERROR("Error ADC too much connect %d %d\n", revalue, i);
                return -1;
            }
            revalue =  i;
        }
    }

    return revalue;
}

SINT32 GetCurrentHDMIConnectPin(void)
{
    SINT32 revalue = -1;
    SINT32 i;

    if(ResourceStatus[HAL_AUDIO_RESOURCE_HDMI].connectStatus[0] == HAL_AUDIO_RESOURCE_CONNECT)
    {
        revalue =  HAL_AUDIO_RESOURCE_HDMI;
    }

    for(i = HAL_AUDIO_RESOURCE_HDMI0; i <= HAL_AUDIO_RESOURCE_SWITCH; i++)
    {
        if(ResourceStatus[i].connectStatus[0] == HAL_AUDIO_RESOURCE_CONNECT)
        {
            if(revalue != -1)
            {
                ERROR("Error HDMI too much connect %d %d\n", revalue, i);
                return -1;
            }
            revalue =  i;
        }
    }

    if(revalue == -1)
        INFO("no  HDMI connect\n");
    return revalue;
}

SINT32 GetCurrentDPConnectPin(void)
{
    SINT32 revalue = -1;

    if (ResourceStatus[HAL_AUDIO_RESOURCE_DP].connectStatus[0] == HAL_AUDIO_RESOURCE_CONNECT
        || ResourceStatus[HAL_AUDIO_RESOURCE_DP1].connectStatus[0] == HAL_AUDIO_RESOURCE_CONNECT) {
        revalue = HAL_AUDIO_RESOURCE_DP;
    }

    if (revalue == -1) {
        INFO("no display port connect\n");
    }
    return revalue;
}

BOOLEAN IsResourceRunningByProcess(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_MODULE_STATUS resourceStatus)
{
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    // check resource status
    if((resourceStatus.connectStatus[0] == HAL_AUDIO_RESOURCE_RUN)||
        resourceStatus.connectStatus[0] == HAL_AUDIO_RESOURCE_PAUSE)
    {
        // check if flow has module or not
        Base* pFlowBaseObj;
        if (Aud_flow[adecIndex] == NULL) {
            ERROR("[%s] Error Aud_flow%d instance NULL\n", __func__, adecIndex);
            return FALSE;
        }
        pFlowBaseObj = Aud_flow[adecIndex]->pBaseObj;

        if(list_empty(&pFlowBaseObj->flowList))
        {
            INFO("[%s] ADEC%d is running but no module found in FlowManager", __func__, adecIndex);
            return FALSE;
        }
        else
            return TRUE;
    }

    return FALSE;
}

/*! Get ADEC volume setting by DecInVol, DecOutVol, DecAdsp_gain
 */
void ADEC_Calculate_DSPGain(UINT32 adecIndex, SINT32 dsp_gain[AUD_MAX_CHANNEL])
{
    SINT32 i;
    HAL_AUDIO_VOLUME_T tempVol = GetDecInVolume(adecIndex);

    for(i = 0; i < AUD_MAX_CHANNEL; i++)
    {
        dsp_gain[i] = Volume_to_DSPGain(Volume_Add(GetDecOutVolume(adecIndex, i), tempVol));
    }
    return;
}

typedef enum {
    SNDOUT_NO_OUTPUT  = 0x0000,
    SNDOUT_SPK        = 0x0001,
    SNDOUT_OPTIC      = 0x0002,
    SNDOUT_OPTIC_LG   = 0x0004,
    SNDOUT_BLUETOOTH  = 0x0008,
    SNDOUT_HP         = 0x0010,
    SNDOUT_ARC        = 0x0020,
    SNDOUT_WISA       = 0x0040,
    SNDOUT_SE_BT      = 0x0080,
    SNDOUT_MAX_OUTPUT = 0x0100,
} SNDOUT_DEVICE;

/****************************************************************************************
 * Static Audio DSP communication functions
 ****************************************************************************************/

static long SendRPC_AudioConfig(AUDIO_CONFIG_COMMAND_RTKAUDIO *audioConfig)
{
    UINT32 ret;
    ret = rtkaudio_send_audio_config(audioConfig);
    if(ret != S_OK){
        ERROR("%s ret!=S_OK, %x\n",__func__,ret);
    }
    return ret;
}

static KADP_STATUS_T ADSP_SNDOut_AddDevice(HAL_AUDIO_SNDOUT_T opt_id)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 sndout_id;
    if(opt_id == HAL_AUDIO_SPDIF) {
        sndout_id = SNDOUT_OPTIC | SNDOUT_ARC;
    } else if (opt_id == HAL_AUDIO_HP) {
        sndout_id = SNDOUT_HP;
    } else if (opt_id == HAL_AUDIO_SPK) {
        sndout_id = SNDOUT_SPK;
    } else {
        return KADP_NOT_OK;
    }
    if(Sndout_Devices & sndout_id) {
        DEBUG("output type %d already open\n",opt_id);
        return KADP_OK;
    }
    Sndout_Devices |= sndout_id;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SNDOUT_DEVICE;
    audioConfig.value[0] = Sndout_Devices;
    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;
    return KADP_OK;
}

static KADP_STATUS_T ADSP_SNDOut_RemoveDevice(HAL_AUDIO_SNDOUT_T opt_id)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 sndout_id;
    if(opt_id == HAL_AUDIO_SPDIF) {
        sndout_id = SNDOUT_OPTIC | SNDOUT_ARC;
    } else if (opt_id == HAL_AUDIO_HP) {
        sndout_id = SNDOUT_HP;
    } else if (opt_id == HAL_AUDIO_SPK) {
        sndout_id = SNDOUT_SPK;
    } else {
        return KADP_NOT_OK;
    }
    if((Sndout_Devices & sndout_id) == 0)
    {
        DEBUG("output type %d already close\n",opt_id);
        return KADP_OK;
    }
    Sndout_Devices &= (~sndout_id);

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SNDOUT_DEVICE;
    audioConfig.value[0] = Sndout_Devices;
    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;
    return KADP_OK;
}

static KADP_STATUS_T ADSP_DEC_SetDelay(UINT32 index, UINT32 delay)
{
    UINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    Base* pPPAO;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    if(delay > AUD_MAX_DELAY)
    {
        ERROR("Set (ADEC%d) delay %d ms, exceed max delayTime\n", index, delay);
        return KADP_NOT_OK;
    }

    if (Aud_Aout[index] == NULL) {
        ERROR("%s error, Aud_Aout%d is NULL\n", index);
        return KADP_NOT_OK;
    }
    pPPAO = Aud_Aout[index];

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_DELAY;
    audioConfig.value[0] = pPPAO->GetAgentID(pPPAO);
    audioConfig.value[1] = ENUM_DEVICE_DECODER;
    audioConfig.value[2] = pPPAO->GetInPinID(pPPAO);
    audioConfig.value[3] = 0xFF;
    for(i = 0; i < AUD_MAX_CHANNEL; i++)
        audioConfig.value[4+i] = delay;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;

    AudioDecInfo[index].decDelay = delay;
    return KADP_OK;
}

static KADP_STATUS_T ADSP_DEC_SetVolume(UINT32 index, SINT32 ch_vol[AUD_MAX_CHANNEL])
{
    UINT32 i, update = FALSE;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    if (!IsValidADECInstance(index)) {
        ERROR("%s Aud_dec[%d] NULL\n", __func__, index);
        return KADP_NOT_OK;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_VOLUME;
    audioConfig.value[0] = Aud_dec[index]->GetAgentID(Aud_dec[index]);
    //audioConfig.value[1] = ENUM_DEVICE_DECODER;
    if(index == 0)
        audioConfig.value[1] = ENUM_DEVICE_DECODER0;
    else
        audioConfig.value[1] = ENUM_DEVICE_DECODER1;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = 0xFF;
    for(i = 0; i < AUD_MAX_CHANNEL; i++)
    {
        if(ch_vol[i] != (SINT32)AudioDecInfo[index].decVol[i])
            update = TRUE;
        audioConfig.value[4+i] = ch_vol[i];
        AudioDecInfo[index].decVol[i] = ch_vol[i];
        DEBUG("adec %d channel %d = %x\n", index, i, ch_vol[i]);
     }
#ifdef AVOID_USELESS_RPC
    if(update == FALSE) return KADP_OK;
#endif
    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;

    return KADP_OK;
}

static KADP_STATUS_T ADSP_DEC_GetMute(UINT32 index)
{
    return AudioDecInfo[index].decMute;
}

static KADP_STATUS_T ADSP_DEC_SetMute(UINT32 index, BOOLEAN bMute)
{
    UINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    if (!IsValidADECInstance(index)) {
        ERROR("%s Aud_dec[%d] NULL\n", __func__, index);
        return KADP_NOT_OK;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    INFO("Set (ADEC%d) %s\n", index, (bMute)? "MUTE":"UN-MUTE");

#ifdef AVOID_USELESS_RPC
    if((bMute) == ADSP_DEC_GetMute(index))
    {
        INFO("Skip this time mute\n");

        return KADP_OK;
    }
#endif

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_MUTE;
    audioConfig.value[0] = Aud_dec[index]->GetAgentID(Aud_dec[index]);
    if(index == 0)
        audioConfig.value[1] = ENUM_DEVICE_DECODER0;
    else
        audioConfig.value[1] = ENUM_DEVICE_DECODER1;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = 0xFF;
    for(i = 0; i < 8; i++)
        audioConfig.value[4+i] = (bMute);

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;

    AudioDecInfo[index].decMute = (bMute);
    return KADP_OK;
}

static BOOLEAN ADSP_AMIXER_GetMute(UINT32 mixerIndex)
{
    return  g_mixer_curr_mute[mixerIndex];
}

static KADP_STATUS_T ADSP_AMIXER_SetMute(HAL_AUDIO_MIXER_INDEX_T mixerIndex, BOOLEAN bMute)
{
    UINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

#ifdef AVOID_USELESS_RPC
    if(bMute == ADSP_AMIXER_GetMute(mixerIndex))
    {
        return KADP_OK;
    }
#endif
    INFO("Set (AMIXER%d) %s\n", mixerIndex, (bMute)? "MUTE":"UN-MUTE");

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_MUTE;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = ENUM_DEVICE_FLASH_PIN;
    audioConfig.value[2] = (UINT32)mixerIndex;
    audioConfig.value[3] = 0xFF;

    for(i = 0; i < 8; i++)
    {
        audioConfig.value[4+i] = (bMute);
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;

    g_mixer_curr_mute[mixerIndex]= (bMute);
    return KADP_OK;
}

static KADP_STATUS_T ADSP_DEC_SetADMode(BOOLEAN bOnOff, UINT32 mainIndex, UINT32 subStreamId)
{
    DUAL_DEC_INFO decADMode;
    UINT32 info_type = INFO_AUDIO_MIX_INFO;
    UINT32 i;

    decADMode.bEnable     = bOnOff;
    decADMode.subStreamId = subStreamId;
    for(i = 0; i < AUD_ADEC_INIT_MAX; i++)
    {
        if (!IsValidADECInstance(i)) {
            ERROR("%s Aud_dec[%d] NULL\n", __func__, i);
            return KADP_NOT_OK;
        }
        decADMode.subDecoderMode = (bOnOff && i != mainIndex)? DEC_IS_SUB : DEC_IS_MAIN;
        Aud_dec[i]->PrivateInfo(Aud_dec[i], info_type, (BYTE*)&decADMode, sizeof(decADMode));
    }
    return KADP_OK;
}

static KADP_STATUS_T ADSP_SetRawMode(int mode)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SPDIF;
    audioConfig.value[0] = audioConfig.value[1] = mode;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = TRUE;
    if (SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        ERROR("[%s,%d] failed\n", __func__, __LINE__);
        return KADP_NOT_OK;
    }
    return KADP_OK;
}

static KADP_STATUS_T ADSP_SetDTSSupport(int mode)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_DTS_SPDIF_RAW;
    audioConfig.value[0] = mode;
    if (SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        ERROR("[%s,%d] failed\n", __func__, __LINE__);
        return KADP_NOT_OK;
    }
    return KADP_OK;
}

static KADP_STATUS_T ADSP_SetDTSHDSupport(int mode)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_DTS_HDMI_RAW;
    audioConfig.value[0] = mode;
    if (SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        ERROR("[%s,%d] arc auto aac failed\n", __func__, __LINE__);
        return KADP_NOT_OK;
    }
    return KADP_OK;
}

void Update_RawMode_by_connection(void)
{
    if(_ARC_Enable || _EARC_Enable) {
        ADSP_SetRawMode(_AudioARCMode);
    } else {
        ADSP_SetRawMode(_AudioSPDIFMode);
    }
    return;
}

static BOOLEAN GetCurrSNDOutMute(UINT32 dev_id)
{
    switch(dev_id)
    {
        case ENUM_DEVICE_SPEAKER:
            return adsp_sndout_info.spk_mute;
        case ENUM_DEVICE_SPDIF:
            return adsp_sndout_info.spdif_mute;
        case ENUM_DEVICE_SPDIF_ES:
            return adsp_sndout_info.spdifes_mute;
        case ENUM_DEVICE_HEADPHONE:
            return adsp_sndout_info.hp_mute;
        case ENUM_DEVICE_SCART:
            return adsp_sndout_info.scart_mute;
        default:
            return FALSE;
    }
}

static void SetCurrSNDOutMute(UINT32 dev_id, BOOLEAN mute)
{
    switch(dev_id)
    {
        case ENUM_DEVICE_SPEAKER:
            adsp_sndout_info.spk_mute = mute; break;
        case ENUM_DEVICE_SPDIF:
            adsp_sndout_info.spdif_mute = mute; break;
        case ENUM_DEVICE_SPDIF_ES:
            adsp_sndout_info.spdifes_mute = mute; break;
        case ENUM_DEVICE_HEADPHONE:
            adsp_sndout_info.hp_mute = mute; break;
        case ENUM_DEVICE_SCART:
            adsp_sndout_info.scart_mute = mute; break;
        default:
            return;
    }
    return;
}

static KADP_STATUS_T ADSP_SNDOut_SetMute(UINT32 dev_id,  BOOLEAN bMute)
{
    UINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    CHAR dev_name[ENUM_DEVICE_MAX][8] = {
        "NONE", "ENC", "DEC", "SPDIF", "SPK", "HP", "SCART", "PCMCap", "Mixer", "Flash", "SPDIFES", "DEC0", "DEC1"};

    INFO("Set (%s) %s\n", dev_name[dev_id],(bMute)? "MUTE":"UN-MUTE" );
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_MUTE;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = dev_id;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = 0xFF;
    for(i = 0; i < 8; i++)
    {
        audioConfig.value[4+i] = (bMute);
    }
    SetCurrSNDOutMute(dev_id, (bMute));
    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;
    return KADP_OK;
}

static KADP_STATUS_T SNDOut_SetMute(UINT32 dev_id, BOOLEAN bMute)
{
#ifdef AVOID_USELESS_RPC
    if((bMute) == GetCurrSNDOutMute(dev_id)) {
        return KADP_OK;
    }
#endif
    return ADSP_SNDOut_SetMute(dev_id, (bMute));
}

static KADP_STATUS_T ADSP_SNDOut_SetDelay(UINT32 dev_id, UINT32 delay)
{
    UINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    CHAR dev_name[ENUM_DEVICE_MAX][8] = {
        "NONE", "ENC", "DEC", "SPDIF", "SPK", "HP", "SCART", "PCMCap", "Mixer", "Flash"};

    if(delay > AUD_MAX_DELAY)
    {
        ERROR("Set (%s) delay %d ms, exceed max delayTime\n", dev_name[dev_id], delay);
        return KADP_NOT_OK;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_DELAY;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = dev_id;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = 0x03;
    for(i = 0; i < 8; i++)
    {
        audioConfig.value[4+i] = delay;
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;
    return KADP_OK;
}

static KADP_STATUS_T ADSP_SNDOut_SetVolume(UINT32 dev_id, UINT32 dsp_gain)
{
    UINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_VOLUME;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = dev_id;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = 0xFF;
    for(i = 0; i < 8; i++)
    {
        audioConfig.value[4+i] = dsp_gain;
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;
    return KADP_OK;
}

static void update_prescale_params(SINT32 vol, UINT32 ch_id)
{
    SINT32 a, b;
	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	a = vol / 6;
	b = vol % 6;
	if (b < 0) {
		a--;
		b += 6;
	}

	if (ch_id&APP_CH_ID_CH0 || ch_id&APP_CH_ID_CH1) {
		pre_scale.ch01_scale_a = a;
		pre_scale.ch01_scale_b = b;
	}

	if (ch_id&APP_CH_ID_CH2 || ch_id&APP_CH_ID_CH3) {
		pre_scale.ch23_scale_a = a;
		pre_scale.ch23_scale_b = b;
	}

	if (ch_id&APP_CH_ID_CH4 || ch_id&APP_CH_ID_CH5) {
		pre_scale.ch45_scale_a = a;
		pre_scale.ch45_scale_b = b;
	}

	if (ch_id&APP_CH_ID_CH6 || ch_id&APP_CH_ID_CH7) {
		pre_scale.ch67_scale_a = a;
		pre_scale.ch67_scale_b = b;
	}
}

static KADP_STATUS_T ADSP_SNDOut_SetPreScaleVolume(UINT32 dev_id, SINT32 vol)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_PRESCALE_VOLUME;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = dev_id;
    audioConfig.value[2] = vol;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;
    return KADP_OK;
}

static KADP_STATUS_T ADSP_SNDOut_SetFineVolume(UINT32 dev_id, UINT32 dsp_gain, UINT32 dsp_fine, BOOLEAN b_spcf_ch, UINT32 dev_ch)
{
    UINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_VOLUME;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = dev_id;
    audioConfig.value[2] = 0;
    if(b_spcf_ch) {
        audioConfig.value[3] = dev_ch;
    } else {
        audioConfig.value[3] = 0xFF;
    }
    for(i = 0; i < 8; i++)
    {
        audioConfig.value[4+i] = dsp_gain;
    }

    audioConfig.value[12] = dsp_fine%4;  // 0~3

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return KADP_NOT_OK;
    return KADP_OK;
}

/*! Set PP mixer mode and apply mixer gain to un-focus pin
 */
static KADP_STATUS_T ADSP_ADMix_ConfigMixer(BOOLEAN bOnOff, UINT32 mixer_main_gain, UINT32 mixer_ad_gain)
{
    DUAL_DEC_MIXING audioMixing;
    UINT32 info_type = INFO_AUDIO_START_MIXING;

    if (Aud_Aout[0] == NULL || Aud_Aout[1] == NULL) {
        ERROR("%s mix fail (main Aout:%p,sub Aout:%p)\n", __func__, Aud_Aout[0], Aud_Aout[1]);
        return KADP_ERROR;
    }
    audioMixing.mode   = bOnOff;

    INFO("%s  ADMix = %d, mixer_main_gain = 0x%x, mixer_ad_gain = 0x%x\n", __func__, bOnOff, mixer_main_gain, mixer_ad_gain);

    if (!bOnOff) {
        audioMixing.volume = ADEC_DSP_MIX_GAIN_0DB;
        Aud_Aout[0]->PrivateInfo(Aud_Aout[0], info_type, (BYTE*)&audioMixing, sizeof(audioMixing));
        Aud_Aout[1]->PrivateInfo(Aud_Aout[1], info_type, (BYTE*)&audioMixing, sizeof(audioMixing));
    } else {
        audioMixing.volume = mixer_main_gain;
        Aud_Aout[0]->PrivateInfo(Aud_Aout[0], info_type, (BYTE*)&audioMixing, sizeof(audioMixing));

        audioMixing.volume = mixer_ad_gain;
        Aud_Aout[1]->PrivateInfo(Aud_Aout[1], info_type, (BYTE*)&audioMixing, sizeof(audioMixing));
    }

    return KADP_OK;
}

static HAL_AUDIO_DSPGAIN_T HWGain_to_DSPGain(SINT32 dbGains)
{
    SINT32 dsp_gain;
    HAL_AUDIO_DSPGAIN_T gain;
    HAL_AUDIO_VOLUME_T volume;

    /*!
      UINT8	mainVol;	// 1 dB step, -127 ~ +30 dB.
      UINT8	fineVol;  	// 1/16 dB step, 0dB ~ 15/16dB
      */
    if (dbGains >= 0) {
        volume.mainVol = dbGains / 8 + 127;
        volume.fineVol = (dbGains % 8) * 2;
    } else if (dbGains % 8 == 0) {
        volume.mainVol = dbGains / 8 + 127;
        volume.fineVol = 0;
    } else {
        volume.mainVol = dbGains / 8 + 127 - 1;
        volume.fineVol = 16 + ((dbGains % 8) * 2);
    }
    dsp_gain = Volume_to_DSPGain(volume);
    gain.dsp_gain = dsp_gain;
    gain.dsp_fine = volume.fineVol;
    AUDIO_DEBUG("%s dbGains=%d -> %d,%d\n", __func__, dbGains, gain.dsp_gain, gain.dsp_fine);

    return gain;
}

static void setQuickShowParam(QuickShowCreateParam *param, AUDIO_QUICK_SHOW_PARAM p, UINT32 *nonCachedAddr)
{
    param->ain_instanceID  = p.value[1];
    param->dec_instanceID  = p.value[2];
    param->aout_instanceID = p.value[3];
    param->ain_rh_phyAddr  = p.value[4];
    param->icq_rh_phyAddr  = p.value[5];
    param->dec_rh_phyAddr  = p.value[6];
    param->ain_buf_size    = p.value[7];
    param->icq_buf_size    = p.value[8];
    param->dec_buf_size    = p.value[9];

    param->ain_nonCachedAddr = nonCachedAddr;
    param->icq_nonCachedAddr = param->ain_nonCachedAddr + (param->icq_rh_phyAddr - param->ain_rh_phyAddr);
    param->dec_nonCachedAddr = param->icq_nonCachedAddr + (param->dec_rh_phyAddr - param->icq_rh_phyAddr);

    INFO("[QuickShow][%s] ain_rh_phyAddr:%x, icq_rh_phyAddr:%x, dec_rh_phyAddr:%x", __func__, param->ain_rh_phyAddr, param->icq_rh_phyAddr, param->dec_rh_phyAddr);
    INFO("[QuickShow][%s] ain_buf_size:%x, icq_buf_size:%x, dec_buf_size:%x", __func__, param->ain_buf_size, param->icq_buf_size, param->dec_buf_size);
    INFO("[QuickShow][%s] ain_nonCachedAddr:%x, icq_nonCachedAddr:%x, dec_nonCachedAddr:%x", __func__, param->ain_nonCachedAddr, param->icq_nonCachedAddr, param->dec_nonCachedAddr);
};

DTV_STATUS_T Init_Flow_Control_QuickShow(RTKAUDIO_QS_TYPE *qs_type)
{
    AUDIO_QUICK_SHOW_PARAM p;
    QuickShowCreateParam param;
    UINT32 *nonCachedAddr;
    UINT32 carvedout_size, carvedout_start_addr;
    SINT32 i;
    *qs_type = rtkaudio_quickshow_init_table(&p);

    carvedout_start_addr = p.value[4];
    carvedout_size = p.value[0] - p.value[4];
    nonCachedAddr = (UINT32 *)phys_to_virt(ntohl(carvedout_start_addr));
    INFO("[QuickShow][%s] nonCachedAddr:%x, carvedout_size:%x\n", __func__, nonCachedAddr, carvedout_size);

    setQuickShowParam(&param, p, nonCachedAddr);

    if(CreateAINFilterQuickShow(param) != OK)
    {
        ERROR("initial AIN Filter Failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    if(CreateFlowManagerFilter() != OK)
    {
        ERROR("create FlowManager failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    if(CreatePPAOFilterQuickShow(param) != OK)
    {
        ERROR("create PPAO failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    if(CreateDecFilterQuickShow(param) != OK)
    {
        ERROR("create DEC filter failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    InitialResourceStatus();
    InitialADCStatus();
    InitialDecStatus();

    ADSP_SNDOut_AddDevice(HAL_AUDIO_SPK);

    for(i = 0; i <AUD_AMIX_MAX; i++)
    {
        g_mixer_gain[i].mainVol = 0x7F;
        g_mixer_gain[i].fineVol = 0x0;
    }

    AinStatus.hdmifullcapport = 0;

    return OK;
}

DTV_STATUS_T Init_Lite_Control(void)
{
    if(Aud_Aout[0] == NULL)
    {
        Aud_Aout[0] = new_PPAO(AO_ONLY, 0);
        if(Aud_Aout[0] == NULL)
        {
            ERROR("create audio ppao failed\n");
            return NOT_OK;
        }
    }
    return OK;
}

DTV_STATUS_T DeInit_Lite_Control(void)
{
    if(Aud_Aout[0] != NULL)
    {
        Aud_Aout[0]->Stop(Aud_Aout[0]);
        Aud_Aout[0]->Delete(Aud_Aout[0]);
        Aud_Aout[0] = NULL;
    }
    return OK;
}

DTV_STATUS_T RAL_AUDIO_SetAudioDescriptionMode(SINT32 mainIndex, BOOLEAN bOnOff)
{
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    /* Set Dual-Decode mode */
    ADSP_DEC_SetADMode(bOnOff, mainIndex, 0);/* current HAL API doesn't support substream ID */
    /* Set Mixer mode */
    if (ADSP_ADMix_ConfigMixer(bOnOff, Volume_to_MixerGain(currMainVol), Volume_to_MixerGain(currADVol)) != KADP_OK) {
        ERROR("%s mix fail\n", __func__);
        return NOT_OK;
    }

    Aud_descriptionMode = bOnOff;

    return OK;
}

DTV_STATUS_T RAL_AUDIO_GetDecodingType(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T *pAudioType)
{
    AUDIO_RPC_DEC_FORMAT_INFO dec_fomat;
    UINT32 retStatus;
    HAL_AUDIO_RESOURCE_T curResourceId;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if(pAudioType == NULL)
        return NOT_OK;

    if (!IsValidADECInstance(adecIndex)) {
        ERROR("%s Aud_dec[%d] NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    curResourceId = adec2res(adecIndex);

    if(AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0))
    {
        DEBUG("%s play check failed %d\n",
                __func__, ResourceStatus[curResourceId].connectStatus[0]);
        return NOT_OK;
    }

    retStatus = ((DEC*)Aud_dec[adecIndex]->pDerivedObj)->GetAudioFormatInfo(Aud_dec[adecIndex], &dec_fomat);

    if (retStatus != S_OK)
    {
        ( *pAudioType= Aud_Adec_info[adecIndex].curAdecFormat); //WOSQRTK-3050 , return default setting
        INFO("get return fail return %s\n",  GetSRCTypeName(*pAudioType));
        return OK;
    }

    *pAudioType = Convert2HalAudioDecType(dec_fomat.type, dec_fomat.reserved[1], dec_fomat.reserved[2]);

    if (dec_fomat.type == AUDIO_UNKNOWN_TYPE)
    {
        (*pAudioType = Aud_Adec_info[adecIndex].curAdecFormat); //WOSQRTK-3050 , return default setting
        INFO("get return format is unknown return %s\n ", GetSRCTypeName(*pAudioType) );
        return OK;
    }

    if (AudioDecInfo[adecIndex].decFormat != *pAudioType) {
        INFO("[ADEC-%d]: format change to %s, reserved[1] %d, reserved[2] %d\n ",
            adecIndex, GetSRCTypeName(*pAudioType), dec_fomat.reserved[1], dec_fomat.reserved[2]);
        AudioDecInfo[adecIndex].decFormat = *pAudioType;
    }

    return OK;
}

DTV_STATUS_T RAL_AUDIO_GetESInfo(int adecIndex, HAL_AUDIO_ES_INFO_T *pAudioESInfo, UINT16 *sampleRate)
{
    #define AAC_CHANNEL_DUALMONO (MODE_11)
    #define AAC_CHANNEL_MONO (MODE_10)
    #define AAC_CHANNEL_STEREO (MODE_20)
    #define AAC_CHANNEL_MULTI (MODE_32)

    AUDIO_RPC_DEC_FORMAT_INFO dec_fomat;
    UINT32 ret = 0;
    *sampleRate = 48000;

    memset(pAudioESInfo, 0, sizeof(HAL_AUDIO_ES_INFO_T));

    if (RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) {
        if (Aud_dec[adecIndex] == NULL) {
            ERROR("%s Aud_dec[%d] NULL\n", __func__, adecIndex);
            return NOT_OK;
        }
        ret = ((DEC*)Aud_dec[adecIndex]->pDerivedObj)->GetAudioFormatInfo(Aud_dec[adecIndex], &dec_fomat);
    } else {
        void *data_cma = NULL; //uncached vir addr
        unsigned long data_cma_phy = 0; //physical addr
        void *vir_addr = NULL; //cached vir addr
        AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

        vir_addr = get_rpc_mem(&data_cma);
        data_cma_phy = (unsigned long)dvr_to_phys(vir_addr);

        if (data_cma == NULL) {
            ERROR("[%s %d] RTKAudio_Malloc data size=%d fail\n",__func__,__LINE__,sizeof(AUDIO_RPC_DEC_FORMAT_INFO));
            return NOT_OK;
        }

        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_MEDIA_FORMAT;
        audioConfig.value[0] = data_cma_phy;
        audioConfig.value[1] = sizeof(AUDIO_RPC_DEC_FORMAT_INFO);

        ret = KADP_AUDIO_AudioConfig(&audioConfig);

        if(ret != 0){
            put_rpc_mem(vir_addr);
            ERROR("[%s %d] fail\n",__func__,__LINE__);
            return NOT_OK;
        }
        ret = S_OK;
        memcpy(&dec_fomat, (void *)data_cma, sizeof(AUDIO_RPC_DEC_FORMAT_INFO));
        put_rpc_mem(vir_addr);
    }

    if (ret == E_FAIL) {
        dec_fomat.type = AUDIO_UNKNOWN_TYPE;
        pAudioESInfo->audioMode = HAL_AUDIO_MODE_UNKNOWN;
        INFO("[WARNING] GetAudioFormatInfo fail, return unknown %x, ch %x\n", dec_fomat.type, dec_fomat.nChannels );

        if (RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) {
            if ((AudioDecInfo[adecIndex].decFormat != pAudioESInfo->adecFormat)) {
                INFO("[ADEC-%d]: format change to %s\n", adecIndex, GetSRCTypeName(pAudioESInfo->adecFormat));
                AudioDecInfo[adecIndex].decFormat = pAudioESInfo->adecFormat;
            }
        } else {
            if (MediaAudioDecInfo.decFormat != pAudioESInfo->adecFormat) {
                INFO("[MediaAdec]: format change to %s\n", GetSRCTypeName(pAudioESInfo->adecFormat));
                MediaAudioDecInfo.decFormat = pAudioESInfo->adecFormat;
            }
        }

        return OK;
    }

    if(dec_fomat.type == AUDIO_UNKNOWN_TYPE && dec_fomat.nChannels != 0)
    {
        pAudioESInfo->audioMode = HAL_AUDIO_MODE_UNKNOWN;

        if (RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) {
            if (AudioDecInfo[adecIndex].decFormat != pAudioESInfo->adecFormat) {
                INFO("[ADEC-%d]: format change to %s\n ", adecIndex, GetSRCTypeName(pAudioESInfo->adecFormat));
                AudioDecInfo[adecIndex].decFormat = pAudioESInfo->adecFormat;
            }
        } else {
            if (MediaAudioDecInfo.decFormat != pAudioESInfo->adecFormat) {
                INFO("[MediaAdec]: format change to %s\n ", GetSRCTypeName(pAudioESInfo->adecFormat));
                MediaAudioDecInfo.decFormat = pAudioESInfo->adecFormat;
            }
        }
        return OK;
    }

    pAudioESInfo->adecFormat = Convert2HalAudioDecType(dec_fomat.type, dec_fomat.reserved[1], dec_fomat.reserved[2]);

    if (RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) {
        if (AudioDecInfo[adecIndex].decFormat != pAudioESInfo->adecFormat) {
            INFO("[ADEC-%d]: format change to %s, reserved[1] %d, reserved[2] %d\n ",
                 adecIndex, GetSRCTypeName(pAudioESInfo->adecFormat), (int)dec_fomat.reserved[1], (int)dec_fomat.reserved[2]);
            AudioDecInfo[adecIndex].decFormat = pAudioESInfo->adecFormat;
        }
    } else {
        if (MediaAudioDecInfo.decFormat != pAudioESInfo->adecFormat) {
            INFO("[MediaAdec]: format change to %s, reserved[1] %d, reserved[2] %d\n ",
                                GetSRCTypeName(pAudioESInfo->adecFormat), (int)dec_fomat.reserved[1], (int)dec_fomat.reserved[2]);
            MediaAudioDecInfo.decFormat = pAudioESInfo->adecFormat;
        }
    }

    *sampleRate = dec_fomat.nSamplesPerSec;

    if(dec_fomat.nChannels == 0)
        pAudioESInfo->audioMode = HAL_AUDIO_MODE_UNKNOWN;
    else if(dec_fomat.nChannels == 1)
        pAudioESInfo->audioMode = HAL_AUDIO_MODE_MONO;
    else if(dec_fomat.nChannels == 2)
        pAudioESInfo->audioMode = HAL_AUDIO_MODE_STEREO;
    else if(dec_fomat.nChannels >  2)
        pAudioESInfo->audioMode = HAL_AUDIO_MODE_MULTI;

    if(dec_fomat.type == AUDIO_MPEG_DECODER_TYPE)
    {
#if 0
        // bit [0:7]  for mpegN
        SINT32 mpegN;
        mpegN =  (dec_fomat.reserved[0] & 0xFF);
#endif
        // bit [8:15]  for layerN
        SINT32 layerN, stereoMode;
        layerN = (dec_fomat.reserved[0] & 0xFF00) >> 8;
        stereoMode = (dec_fomat.reserved[0] & 0xFF0000) >> 16;

        if (layerN == 3)
        {
            //pAudioESInfo->adecFormat = HAL_AUDIO_SRC_TYPE_MP3; // keep mpeg
        }

        pAudioESInfo->mpegESInfo.bitRate = (dec_fomat.nAvgBytesPerSec) / 4000; // from UTAudioDebug.cpp
        pAudioESInfo->mpegESInfo.sampleRate = (dec_fomat.nSamplesPerSec / 1000);
        pAudioESInfo->mpegESInfo.layer = layerN;
        pAudioESInfo->mpegESInfo.channelNum = dec_fomat.nChannels;

        // bit [16:23] for mode (0x0:stereo,0x1:joint stereo,0x2:dual,0x3:mono)

        if(stereoMode == 1)
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_JOINT_STEREO;
        else if(stereoMode == 2)
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_DUAL_MONO;
        else if(stereoMode == 3)
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_MONO;
        else // 0
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_STEREO;
    }
    else if((dec_fomat.type == AUDIO_AC3_DECODER_TYPE) || (dec_fomat.type == AUDIO_DDP_DECODER_TYPE))
    {
        SINT32 acmode;
        pAudioESInfo->ac3ESInfo.bitRate = (dec_fomat.nAvgBytesPerSec) / 4000; // from UTAudioDebug.cpp
        pAudioESInfo->ac3ESInfo.sampleRate =  (dec_fomat.nSamplesPerSec / 1000);
        pAudioESInfo->ac3ESInfo.channelNum = dec_fomat.nChannels;
        if (dec_fomat.type == AUDIO_DDP_DECODER_TYPE)
            pAudioESInfo->ac3ESInfo.EAC3 = 1;
        else
            pAudioESInfo->ac3ESInfo.EAC3 = 0;

        // reserved[0] has extra info:
        // bit [0:7]  for lfeon
        // bit [8:15]  for acmod
        //SINT32 acmode = (dec_fomat.reserved[0] & 0xFF) >> 8;
        //fix coverity:220,Event result_independent_of_operands:
        acmode = (dec_fomat.reserved[0] & 0xFF00) >> 8;

        if(acmode == 0) // 1+1
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_DUAL_MONO;
        }
        else if(acmode == 1)
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_MONO;
            if(dec_fomat.nChannels != 1)
            {
                pAudioESInfo->ac3ESInfo.channelNum = 1;
            }
        }
        else if(acmode == 2)
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_STEREO;
            if(dec_fomat.nChannels != 2)
            {
                pAudioESInfo->ac3ESInfo.channelNum = 2;
            }
        }
        else
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_MULTI;
        }
    }
    else if(dec_fomat.type == AUDIO_AAC_DECODER_TYPE)
    {
        //version;              /* AAC = 0x0, HE-AACv1 = 0x1, HE-AACv2 = 0x2 */
        //transmissionformat;   /* LOAS/LATM = 0x0, ADTS = 0x1*/
        SINT32 channel_mode, version, format;
        channel_mode = (dec_fomat.reserved[0] & 0xFF00) >> 8;
        version = (dec_fomat.reserved[1] & 0xFF00) >> 8;
        format = (dec_fomat.reserved[1] & 0xFF);

        if(channel_mode == AAC_CHANNEL_DUALMONO) // mode11
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_DUAL_MONO;
            if(dec_fomat.nChannels != 2)
            {
                dec_fomat.nChannels = 2;
            }
        }
        else  if(channel_mode == AAC_CHANNEL_MONO) // mode10
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_MONO;
            if(dec_fomat.nChannels != 1)
            {
                dec_fomat.nChannels = 1;
            }
        }
        else if(channel_mode == AAC_CHANNEL_STEREO) // mode120
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_STEREO;
            if(dec_fomat.nChannels != 2)
            {
                dec_fomat.nChannels = 2;
            }
        }
        else
        {
            pAudioESInfo->audioMode = HAL_AUDIO_MODE_MULTI;
        }

        if(version == 0)
            pAudioESInfo->heAAcEsInfo.version  = 0; // AAC
        else if(version == 1)
            pAudioESInfo->heAAcEsInfo.version  = 1; // he AAC v1
        else
            pAudioESInfo->heAAcEsInfo.version  = 2; // he AAC v2

        if(format  == 0)
            pAudioESInfo->heAAcEsInfo.transmissionformat   = 0;
        else if(format  == 1)
            pAudioESInfo->heAAcEsInfo.transmissionformat   = 1;
        else
        {
            pAudioESInfo->heAAcEsInfo.transmissionformat   = 0;
        }
        pAudioESInfo->heAAcEsInfo.channelNum = dec_fomat.nChannels;
    }
    else if(dec_fomat.type == AUDIO_RAW_AAC_DECODER_TYPE)
    {
        return NOT_OK;
    }
    else if(dec_fomat.type == AUDIO_DRA_DECODER_TYPE)
    {
        // reserved[0] has extra info:
        // bit [0:15]  for lfeon
        // bit [16:31]  for main channel number
        SLONG tmp = dec_fomat.reserved[0];
        SINT32 lfeon = (SINT32)(tmp & 0x0000FFFF);
        SINT32 main_channel = (SINT32)((tmp>>16) & 0x0000FFFF);
        SINT32 *ptr;
        if(dec_fomat.nChannels != (main_channel + lfeon))
        {
            ERROR("[ERROR]:  dec_fomat.nChannels = %d, main_channel+lfeon = %d\n", dec_fomat.nChannels,(main_channel + lfeon));
            return NOT_OK;
        }

        ptr = (SINT32*)(&pAudioESInfo->audioMode);
        if(lfeon == 1)
        {
            if(main_channel == 2)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_2_1_FL_FR_LFE;
            else if(main_channel == 3)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_3_1_FL_FR_RC_LFE;
            else if(main_channel == 4)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_4_1_FL_FR_RL_RR_LFE;
            else if(main_channel == 5)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_5_1_FL_FR_FC_RL_RR_LFE;
            else
            {
                ERROR("Invalid DRA feedback info: lfeon=%d, main_channel=%d\n", (SINT32)lfeon, (SINT32)main_channel);
                return NOT_OK;
            }
        }
        else
        {
            if(main_channel == 1)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_MONO;
            else if(main_channel == 2)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_STEREO;
            else if(main_channel == 3)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_3_0_FL_FR_RC;
            else if(main_channel == 4)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_4_0_FL_FR_RL_RR;
            else if(main_channel == 5)
              *ptr = (SINT32)HAL_AUDIO_CH_MODE_5_0_FL_FR_FC_RL_RR;
            else
            {
                ERROR("Invalid DRA feedback info: lfeon=%d, main_channel=%d\n", (SINT32)lfeon, (SINT32)main_channel);
                return NOT_OK;
            }
        }
    }

    return OK;
}

DTV_STATUS_T DeInit_Flow_Control(void)
{
    // auto connect (KTASKWBS-469)
    INFO("%s Enter.\n", __func__);

    DeleteFlowManagerFilter();
    DeleteAINFilter();
    DeletePPAOFilter();
    DeleteDecFilter();
    return OK;
}

DTV_STATUS_T RAL_AUDIO_FinalizeModule(void)
{
    INFO("%s", __func__);

    if(rhal_type == RHAL_LITE) {
        DeInit_Lite_Control();
    } else {
        DeInit_Flow_Control();
    }
    DeInit_Audio_Driver();
    DeInit_ATV_Driver();

    rhal_type = RHAL_NOT_INIT;
    INFO("%s finish\n", __func__);

    return OK;
}

DTV_STATUS_T Init_Flow_Control(void)
{
    int Aud_wait_cnt = 10;
    SINT32 i;
    INFO("%s Enter.\n", __func__);

    while(ADSP_TSK_GetStarted() != ST_AUD_TSK_STRTD && Aud_wait_cnt > 0)
    {
        Aud_wait_cnt--;
        AUDIO_INFO("FW Task not init finished, count down %d\n", Aud_wait_cnt);
        msleep(20);
    }

    if(CreateAINFilter() != OK)
    {
        ERROR("initial AIN Filter Failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    if(CreateFlowManagerFilter() != OK)
    {
        ERROR("create FlowManager failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    if(CreatePPAOFilter() != OK)
    {
        ERROR("create PPAO failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    if(CreateDecFilter() != OK)
    {
        ERROR("create DEC filter failed\n");
        RAL_AUDIO_FinalizeModule();
        return NOT_OK;
    }

    InitialResourceStatus();
    InitialADCStatus();
    InitialDecStatus();

    ADSP_SNDOut_AddDevice(HAL_AUDIO_SPK);

    for(i = 0; i <AUD_AMIX_MAX; i++)
    {
        g_mixer_gain[i].mainVol = 0x7F;
        g_mixer_gain[i].fineVol = 0x0;
    }

    AinStatus.hdmifullcapport = 0;

    return OK;
}

DTV_STATUS_T RHAL_QS_AUDIO_InitializeModule(HAL_AUDIO_SIF_TYPE_T eSifType)
{
    SINT64 timerstamp[2] = {0};
    SINT32 i, hdmi_port;
    SINT32 ch_vol[AUD_MAX_CHANNEL];
    HAL_AUDIO_FLOW_STATUS* pCurFlowStatus;
    HAL_AUDIO_RESOURCE_T curDecResourceId, decIptResId;
    RTKAUDIO_QS_TYPE qs_type = RTKAUDIO_QS_HDMI;
    HAL_AUDIO_RESOURCE_T hdmi_port_source = HAL_AUDIO_RESOURCE_HDMI0;

    INFO("%s %d\n", __func__, eSifType);
    if(rhal_type != RHAL_NOT_INIT)
    {
        ERROR("%s retry\n", __func__);
        return OK;
    }

    hdmi_port = 0;
    timerstamp[0] = pli_getPTS();

    if(Init_Flow_Control_QuickShow(&qs_type) != OK)
        return NOT_OK;
    rhal_type = RHAL_FULL;

    for(i = HAL_AUDIO_MIXER0; i < AUD_AMIX_MAX; i++)
    {
        ADSP_AMIXER_SetMute((HAL_AUDIO_MIXER_INDEX_T)i, TRUE);// default to mute
    }

    for(i = 0; i < AUD_MAX_CHANNEL; i++)
        ch_vol[i] = ENUM_AUDIO_DVOL_K0p0DB;
    for(i = 0; i < AUD_ADEC_INIT_MAX; i++)
    {
        ADSP_DEC_SetVolume(i, ch_vol);
    }

    /* Init status for QS flow */
    if (qs_type == RTKAUDIO_QS_DP) {
        SetResourceOpen(HAL_AUDIO_RESOURCE_DP);
        SetResourceOpen(HAL_AUDIO_RESOURCE_ADEC0);
        SetResourceConnect(HAL_AUDIO_RESOURCE_DP, HAL_AUDIO_RESOURCE_NO_CONNECTION);
        SetResourceConnect(HAL_AUDIO_RESOURCE_ADEC0, HAL_AUDIO_RESOURCE_DP);
        UpdateADECStatus(HAL_AUDIO_RESOURCE_ADEC0, HAL_AUDIO_RESOURCE_DP);
        SetDecInMute(0, FALSE);

        AUDIO_INFO("%s QS resource setting is DP\n", __FUNCTION__);
    } else {
        hdmi_port = get_QS_portnum();
        if (hdmi_port < 0) {
            hdmi_port = 0;
        }

        hdmi_port_source = hdmi_port_source + hdmi_port;
        SetResourceOpen(hdmi_port_source);
        SetResourceOpen(HAL_AUDIO_RESOURCE_ADEC0);
        SetResourceConnect(hdmi_port_source, HAL_AUDIO_RESOURCE_NO_CONNECTION);
        SetResourceConnect(HAL_AUDIO_RESOURCE_ADEC0, hdmi_port_source);
        UpdateADECStatus(HAL_AUDIO_RESOURCE_ADEC0, hdmi_port_source);
        SetDecInMute(0, FALSE);

        AUDIO_INFO("%s QS resource setting is HDMI%d (%d)\n", __FUNCTION__, hdmi_port, hdmi_port_source);
    }

    pCurFlowStatus = &FlowStatus[HAL_AUDIO_ADEC0];
    Aud_mainDecIndex = 0;
    // analog flow
    AddAndConnectDecAinToFlow(Aud_flow[HAL_AUDIO_ADEC0], (HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex, Aud_Ain[HAL_AUDIO_ADEC0], pCurFlowStatus);
    /* Connect to PPAO */
    AddAndConnectPPAOToFlow(Aud_flow[HAL_AUDIO_ADEC0], (HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex, Aud_Aout[HAL_AUDIO_ADEC0], pCurFlowStatus);

    // update adec info status
    Aud_Adec_info[Aud_mainDecIndex].prevAdecFormat = Aud_Adec_info[Aud_mainDecIndex].curAdecFormat;
    Aud_Adec_info[Aud_mainDecIndex].curAdecFormat  = HAL_AUDIO_SRC_TYPE_PCM;
    Aud_Adec_info[Aud_mainDecIndex].bAdecStart     = TRUE;
    /* update resource status */

    curDecResourceId = adec2res((HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex);
    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curDecResourceId]);// dec input source
    ResourceStatus[curDecResourceId].connectStatus[0] = HAL_AUDIO_RESOURCE_RUN;

    Aud_flow[0]->ShowCurrentExitModule(Aud_flow[0], 0);

    timerstamp[1] = pli_getPTS();
    INFO("%s start:%lld\n", __func__, timerstamp[0]);
    INFO("%s   end:%lld\n", __func__, timerstamp[1]);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_Init_ATV_Driver(HAL_AUDIO_SIF_TYPE_T eSifType)
{
    INFO("%s Enter.\n", __func__);
    Init_ATV_Driver(eSifType);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_InitializeModule(HAL_AUDIO_SIF_TYPE_T eSifType)
{
    SINT64 timerstamp[2] = {0};
    INFO("%s %d\n", __func__, eSifType);
    if(rhal_type != RHAL_NOT_INIT)
    {
        ERROR("%s retry\n", __func__);
        return OK;
    }

    timerstamp[0] = pli_getPTS();

    if(eSifType == HAL_AUDIO_LITE_INIT)
    {
        if(Init_Lite_Control() != OK)
            return NOT_OK;
        rhal_type = RHAL_LITE;
    }
    else
    {
        SINT32 ch_vol[AUD_MAX_CHANNEL];
        SINT32 i;

        if(Init_Flow_Control() != OK) {
            return NOT_OK;
        }

        rhal_type = RHAL_FULL;
        // initial state is at disconnected so need to mute it.
        //ADSP_SNDOut_SetMute(ENUM_DEVICE_SPEAKER,  TRUE);
        //ADSP_SNDOut_SetMute(ENUM_DEVICE_HEADPHONE,  TRUE);
        //ADSP_SNDOut_SetMute(ENUM_DEVICE_SCART,  TRUE);
        //ADSP_SNDOut_SetMute(ENUM_DEVICE_SPDIF,  TRUE);
        //ADSP_SNDOut_SetMute(ENUM_DEVICE_SPDIF_ES,  TRUE);

        for(i = HAL_AUDIO_MIXER0; i < AUD_AMIX_MAX; i++)
        {
            ADSP_AMIXER_SetMute((HAL_AUDIO_MIXER_INDEX_T)i, TRUE);// default to mute
        }

        for(i = 0; i < AUD_MAX_CHANNEL; i++)
            ch_vol[i] = ENUM_AUDIO_DVOL_K0p0DB;
        for(i = 0; i < AUD_ADEC_INIT_MAX; i++)
        {
            ADSP_DEC_SetVolume(i, ch_vol);
        }
    }

    timerstamp[1] = pli_getPTS();
    INFO("%s start:%lld\n", __func__, timerstamp[0]);
    INFO("%s   end:%lld\n", __func__, timerstamp[1]);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_FinalizeModule(void)
{
    return RAL_AUDIO_FinalizeModule();
}

DTV_STATUS_T RHAL_AUDIO_I2SIN_Open(void)
{
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(HAL_AUDIO_RESOURCE_I2SIN) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_I2SIN_Close(void)
{
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(HAL_AUDIO_RESOURCE_I2SIN) != TRUE)
    {
        return NOT_OK;
    }

    return NOT_OK;
}

DTV_STATUS_T RHAL_AUDIO_UACIN_Open(void)
{
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(HAL_AUDIO_RESOURCE_UACIN) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_UACIN_Close(void)
{
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(HAL_AUDIO_RESOURCE_UACIN) != TRUE)
    {
        return NOT_OK;
    }

    return NOT_OK;
}

DTV_STATUS_T RHAL_AUDIO_SPDIFIN_Open(void)
{
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(HAL_AUDIO_RESOURCE_SPDIFIN) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SPDIFIN_Close(void)
{
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(HAL_AUDIO_RESOURCE_SPDIFIN) != TRUE)
    {
        return NOT_OK;
    }

    return NOT_OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_Open(HAL_AUDIO_TP_INDEX_T tpIndex)
{
    HAL_AUDIO_RESOURCE_T id = (HAL_AUDIO_RESOURCE_T)(HAL_AUDIO_RESOURCE_ATP0 + tpIndex);
    INFO("%s port %d\n", __func__, tpIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(tpIndex > HAL_AUDIO_TP_MAX)
    {
        ERROR("error tp port %d\n", tpIndex);
        return NOT_OK;
    }

    if(SetResourceOpen(id) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_Close(HAL_AUDIO_TP_INDEX_T tpIndex)
{
    HAL_AUDIO_RESOURCE_T id = (HAL_AUDIO_RESOURCE_T)(HAL_AUDIO_RESOURCE_ATP0 + tpIndex);
    INFO("%s port %d\n", __func__, tpIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(tpIndex > HAL_AUDIO_TP_MAX)
    {
        ERROR("error tp port %d\n", tpIndex);
        return NOT_OK;
    }

    if(SetResourceClose(id) != TRUE)
    {
        return NOT_OK;
    }

    RAL_AUDIO_SetAudioDescriptionMode(Aud_mainDecIndex, FALSE);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ADC_Open(HAL_AUDIO_BBADC_SRC portNum)
{
    INFO("%s port %d\n", __func__, portNum);
    if(portNum >= (AUDIO_BBADC_SRC_AIO2_PORT_NUM + 1))
        return NOT_OK;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(AUDIO_CHECK_ADC_PIN_OPEN_NOTAVAILABLE(AinStatus.ainPinStatus[portNum]))
    {
        ERROR("Error ADC pin %d is still at %s state\n",portNum, GetResourceStatusString(AinStatus.ainPinStatus[portNum]));
        return NOT_OK;
    }

    if(SetResourceOpen(HAL_AUDIO_RESOURCE_ADC) != TRUE)
    {
        return NOT_OK;
    }

    AinStatus.ainPinStatus[portNum] = HAL_AUDIO_RESOURCE_OPEN;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ADC_Close(HAL_AUDIO_BBADC_SRC portNum)
{
    INFO("%s port %d\n", __func__, portNum);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(portNum >= (AUDIO_BBADC_SRC_AIO2_PORT_NUM + 1))
        return NOT_OK;

    if(AUDIO_CHECK_ADC_PIN_CLOSE_NOTAVAILABLE(AinStatus.ainPinStatus[portNum]))
    {
        ERROR("Error ADC pin %d is still at %s state\n",portNum, GetResourceStatusString(AinStatus.ainPinStatus[portNum]));
        return NOT_OK;
    }

    if(SetResourceClose(HAL_AUDIO_RESOURCE_ADC) != TRUE)
    {
        return NOT_OK;
    }

    AinStatus.ainPinStatus[portNum] = HAL_AUDIO_RESOURCE_CLOSE;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_Open(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(HAL_AUDIO_RESOURCE_HDMI) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_Close(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(HAL_AUDIO_RESOURCE_HDMI) != TRUE)
    {
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DP_Open(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(HAL_AUDIO_RESOURCE_DP) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DP_Close(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(HAL_AUDIO_RESOURCE_DP) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DP_OpenPort(HAL_AUDIO_DP_INDEX_T DPIndex)
{
    HAL_AUDIO_RESOURCE_T id = (HAL_AUDIO_RESOURCE_T)(HAL_AUDIO_RESOURCE_DP + DPIndex);
    INFO("%s port %d\n", __func__, DPIndex);

    if(DPIndex > HAL_AUDIO_DP_MAX)
    {
        ERROR("error DP port %d\n", DPIndex);
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(id) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DP_ClosePort(HAL_AUDIO_DP_INDEX_T DPIndex)
{
    HAL_AUDIO_RESOURCE_T id = (HAL_AUDIO_RESOURCE_T) (HAL_AUDIO_RESOURCE_DP + DPIndex);
    INFO("%s port %d\n", __func__, DPIndex);

    if(DPIndex > HAL_AUDIO_DP_MAX)
    {
        ERROR("error DP port %d\n", DPIndex);
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(id) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_OpenPort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex)
{
    HAL_AUDIO_RESOURCE_T id = (HAL_AUDIO_RESOURCE_T)(HAL_AUDIO_RESOURCE_HDMI0 + hdmiIndex);
    INFO("%s port %d\n", __func__, hdmiIndex);

    if(hdmiIndex > HAL_AUDIO_HDMI_MAX)
    {
        ERROR("error hdmi port\n");
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(id) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_ClosePort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex)
{
    HAL_AUDIO_RESOURCE_T id = (HAL_AUDIO_RESOURCE_T) (HAL_AUDIO_RESOURCE_HDMI0 + hdmiIndex);
    INFO("%s port %d\n", __func__, hdmiIndex);

    if(hdmiIndex > HAL_AUDIO_HDMI_MAX)
    {
        ERROR("error hdmi port\n");
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(id) != TRUE)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AAD_Open(void)
{
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceOpen(HAL_AUDIO_RESOURCE_AAD) != TRUE)
    {
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AAD_Close(void)
{
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceClose(HAL_AUDIO_RESOURCE_AAD) != TRUE)
    {
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SYSTEM_Open(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(SetResourceOpen(HAL_AUDIO_RESOURCE_SYSTEM) != TRUE)
    {
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SYSTEM_Close(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(SetResourceClose(HAL_AUDIO_RESOURCE_SYSTEM) != TRUE)
    {
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ADEC_Open(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    INFO("%s ADEC%d\n", __func__, adecIndex);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;
    if(SetResourceOpen(adec2res(adecIndex)) != TRUE)
    {
        return NOT_OK;
    }

    if (Aud_dec[adecIndex] == NULL) {
        Aud_dec[adecIndex] = new_DEC(adecIndex);
        if (Aud_dec[adecIndex] == NULL) {
            ERROR("new_DEC idx %d fail\n", adecIndex);
            return NOT_OK;
        }
        INFO("To New a Aud_dec[%d]\n", adecIndex);
    }

    SetSPDIFOutType(adecIndex, HAL_AUDIO_SPDIF_PCM);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ADEC_Close(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    INFO("%s ADEC%d\n", __func__, adecIndex);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;
    if(SetResourceClose(adec2res(adecIndex)) != TRUE)
    {
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_Open(HAL_AUDIO_AENC_INDEX_T aencIndex)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_Close(HAL_AUDIO_AENC_INDEX_T aencIndex)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SE_Open(void)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SE_Close(void)
{
    return OK;
}

static SINT32 ConvertSNDOUTIndexToResourceId(HAL_AUDIO_SNDOUT_T soundOutType)
{
    SINT32 id;
    switch(soundOutType)
    {
        case HAL_AUDIO_NO_OUTPUT:
        default:
            id = -1;
            ERROR("unknow sndout id %d\n", soundOutType);
            break;
        case HAL_AUDIO_SPK:
            id = HAL_AUDIO_RESOURCE_OUT_SPK;
            break;
        case HAL_AUDIO_SPDIF:
            id = HAL_AUDIO_RESOURCE_OUT_SPDIF;
            break;
        case HAL_AUDIO_SB_SPDIF:
            id = HAL_AUDIO_RESOURCE_OUT_SB_SPDIF;
            break;
        case HAL_AUDIO_SB_PCM:
            id = HAL_AUDIO_RESOURCE_OUT_SB_PCM;
            break;
        case HAL_AUDIO_SB_CANVAS:
            id = HAL_AUDIO_RESOURCE_OUT_SB_CANVAS;
            break;
        case HAL_AUDIO_HP:
            id = HAL_AUDIO_RESOURCE_OUT_HP;
            break;
        case HAL_AUDIO_SCART:
            id = HAL_AUDIO_RESOURCE_OUT_SCART;
            break;
        case HAL_AUDIO_SPDIF_ES:
            id = HAL_AUDIO_RESOURCE_OUT_SPDIF_ES;
            break;
    }
    return id;
}

DTV_STATUS_T RHAL_AUDIO_SNDOUT_Open(HAL_AUDIO_SNDOUT_T soundOutType)
{
    SINT32 id;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = ConvertSNDOUTIndexToResourceId(soundOutType);
    INFO("%s %s\n", __func__, GetResourceString((HAL_AUDIO_RESOURCE_T) (id)));

    if(id >= 0)
    {
        /* SPK already open at init */
        if((HAL_AUDIO_RESOURCE_T)id == HAL_AUDIO_RESOURCE_OUT_SPK)
        {
            INFO("SPK already open at init\n");
            return OK;
        }
        else if(SetResourceOpen((HAL_AUDIO_RESOURCE_T)id) != TRUE)
        {
            return NOT_OK;
        }
        ADSP_SNDOut_AddDevice(soundOutType);
        return OK;
    }
    else
    {
        ERROR("unknow sndout type %d\n", soundOutType);
        return NOT_OK;
    }
    return NOT_OK;
}

DTV_STATUS_T RHAL_AUDIO_SNDOUT_Close(HAL_AUDIO_SNDOUT_T soundOutType)
{
    SINT32 id = (SINT32)ConvertSNDOUTIndexToResourceId(soundOutType);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    INFO("%s %s\n", __func__, GetResourceString((HAL_AUDIO_RESOURCE_T)id));

    if(id >= 0)
    {
        /* SPK should have never been closed */
        if((HAL_AUDIO_RESOURCE_T)id == HAL_AUDIO_RESOURCE_OUT_SPK)
        {
            return OK;
        }
        if(SetResourceClose((HAL_AUDIO_RESOURCE_T)id) != TRUE)
        {
            return NOT_OK;
        }
        ADSP_SNDOut_RemoveDevice(soundOutType);
        return OK;
    }
    else
    {
        return NOT_OK;
    }
}

DTV_STATUS_T RHAL_AUDIO_SNDOUT_MixSrc(HAL_AUDIO_SNDOUT_T sndoutdevice, HAL_AUDIO_MIX_INDEX_T mixsrc)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND));

    INFO("%s sndoutdevice=%d, mixsrc=%d\n", __func__, sndoutdevice, mixsrc);

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_SNDOUT_MIXTYPE;
    audioConfig.value[0] = sndoutdevice;
    audioConfig.value[1] = mixsrc;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

void PrintRingBuffer(RINGBUFFER_HEADER* pRingBuffer, ULONG phyaddress)
{
    ULONG pbase, prp, pwp, size;

    INFO("ring buffer %p\n", pRingBuffer);
    if(pRingBuffer == NULL)
        return;

    prp   = IPC_ReadU32((BYTE*) &(pRingBuffer->readPtr[0]));
    pbase = IPC_ReadU32((BYTE*) &(pRingBuffer->beginAddr));
    pwp   = IPC_ReadU32((BYTE*) &(pRingBuffer->writePtr));
    size  = IPC_ReadU32((BYTE*) &(pRingBuffer->size));
    INFO("phy %x -> baddr %x rp %x wp %x size %x\n", phyaddress, pbase, prp, pwp, size);
}

/* RTWTV-219
it is audio description path, example
ADTU0(1) - SDEC0(1) - ATP1(1) - ADEC0(1) - SE - SPK
ADTU0(1) - SDEC0(1) - ATP0(1) - ADEC1(1) - SE - SPK
*/

/* Connect & Disconnect */
DTV_STATUS_T RHAL_AUDIO_I2SIN_Connect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_I2SIN;

    if(SetResourceConnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_I2SIN_Disconnect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_I2SIN;

    if(SetResourceDisconnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_UACIN_Connect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_UACIN;

    if(SetResourceConnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_UACIN_Disconnect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_UACIN;

    if(SetResourceDisconnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SPDIFIN_Connect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_SPDIFIN;

    if(SetResourceConnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SPDIFIN_Disconnect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_SPDIFIN;

    if(SetResourceDisconnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    HAL_AUDIO_RESOURCE_T outputId;
    SINT32 totalOutputConnectResource;
    DEMUX_CHANNEL_T sdec_channel;
    RSDEC_ENUM_TYPE_DEST_T sdec_dest_type;
    DEMUX_STRUCT_CONNECT_RESULT_T AddressInfo;
    SINT32 atpindex;

    INFO("%s port %d %d\n", __func__, currentConnect, inputConnect);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if((currentConnect != HAL_AUDIO_RESOURCE_ATP0) && (currentConnect != HAL_AUDIO_RESOURCE_ATP1))
    {
        ERROR("error connect index %d\n",currentConnect);
        return NOT_OK;
    }

    if((inputConnect != HAL_AUDIO_RESOURCE_SDEC0) && (inputConnect != HAL_AUDIO_RESOURCE_SDEC1))
    {
        ERROR("error connect input index %d\n",inputConnect);
        return NOT_OK;
    }

    GetConnectOutputSource(inputConnect, &outputId, 1, &totalOutputConnectResource);// tp's iutput is sdec

    if(totalOutputConnectResource > 1)// sdec's output
    {
        ERROR("sdec output connect too much %d != 1\n", totalOutputConnectResource);
        return NOT_OK;
    }

    if(SetResourceConnect(currentConnect, inputConnect) != TRUE)
        return NOT_OK;

    GetConnectOutputSource(currentConnect, &outputId, 1, &totalOutputConnectResource);// tp's output is dec

    if(IsADECSource(outputId))// tp's output is dec
    {
        SINT32 adecIndex = res2adec(outputId);
        Aud_Adec_info[adecIndex].prevTPInputPort = Aud_Adec_info[adecIndex].curTPInputPort;
        Aud_Adec_info[adecIndex].curTPInputPort  = inputConnect;
    }

    if(inputConnect == HAL_AUDIO_RESOURCE_SDEC0)
        sdec_channel = DEMUX_CH_A;
    else
        sdec_channel = DEMUX_CH_B;

    if(currentConnect == HAL_AUDIO_RESOURCE_ATP0)
    {
        sdec_dest_type = RSDEC_ENUM_TYPE_DEST_ADEC0;
    }
    else
    {
        sdec_dest_type = RSDEC_ENUM_TYPE_DEST_ADEC1;
    }

    if(KADP_SDEC_Connect(sdec_channel, sdec_dest_type, &AddressInfo) != OK)
    {
        ERROR("sdec connect failed\n");
        return NOT_OK;
    }

    atpindex = currentConnect - HAL_AUDIO_RESOURCE_ATP0;

    ((DtvCom*)Aud_DTV[atpindex]->pDerivedObj)->SetSDECInfo(Aud_DTV[atpindex], (UINT32)sdec_channel, (UINT32)sdec_dest_type);

    INFO("IBand addr %x size %x\n", AddressInfo.ibRingHeader, AddressInfo.ibHeaderSize);
    INFO("BS addr %x size %x\n", AddressInfo.bsRingHeader, AddressInfo.bsHeaderSize);
    INFO("Ref addr %x size %x\n", AddressInfo.refClock, AddressInfo.refClockHeaderSize);

    ((DtvCom*)Aud_DTV[atpindex]->pDerivedObj)->SetICQRingBufPhyAddress(Aud_DTV[atpindex], AddressInfo.ibRingHeader, (void*)__va(AddressInfo.ibRingHeader), 0, 0);
    //PrintRingBuffer((RINGBUFFER_HEADER*)nonCacheAddress, (ULONG) AddressInfo.ibRingHeader);

    ((DtvCom*)Aud_DTV[atpindex]->pDerivedObj)->SetBSRingBufPhyAddress(Aud_DTV[atpindex], AddressInfo.bsRingHeader, (void*)__va(AddressInfo.bsRingHeader), 0, 0);
    //PrintRingBuffer((RINGBUFFER_HEADER*)nonCacheAddress, (ULONG) AddressInfo.bsRingHeader);

    ((DtvCom*)Aud_DTV[atpindex]->pDerivedObj)->SetRefClockPhyAddress(Aud_DTV[atpindex], AddressInfo.refClock, (void*)__va(AddressInfo.refClock), 0, 0);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    SINT32 atpindex;
    HAL_AUDIO_RESOURCE_T id = currentConnect;
    INFO("%s port %d %d\n", __func__, currentConnect, inputConnect);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if((currentConnect != HAL_AUDIO_RESOURCE_ATP0) && (currentConnect != HAL_AUDIO_RESOURCE_ATP1))
    {
        ERROR("error connect index %d\n",currentConnect);
        return NOT_OK;
    }

    if((inputConnect != HAL_AUDIO_RESOURCE_SDEC0) && (inputConnect != HAL_AUDIO_RESOURCE_SDEC1))
    {
        ERROR("error disconnect input index %d\n",inputConnect);
        return NOT_OK;
    }

    if(SetResourceDisconnect(id, inputConnect) != TRUE)
        return NOT_OK;

    atpindex = currentConnect - HAL_AUDIO_RESOURCE_ATP0;

    // clean
    ((DtvCom*)Aud_DTV[atpindex]->pDerivedObj)->SetICQRingBufPhyAddress(Aud_DTV[atpindex],0, 0, 0, 0);
    ((DtvCom*)Aud_DTV[atpindex]->pDerivedObj)->SetBSRingBufPhyAddress(Aud_DTV[atpindex], 0, 0, 0,0);
    ((DtvCom*)Aud_DTV[atpindex]->pDerivedObj)->SetRefClockPhyAddress(Aud_DTV[atpindex], 0, 0, 0, 0);

    if(IsDTVSource(currentConnect))// tp's output is dec
    {
        DEMUX_CHANNEL_T sdec_channel;
        RSDEC_ENUM_TYPE_DEST_T sdec_dest;

        if(inputConnect == HAL_AUDIO_RESOURCE_SDEC0)
            sdec_channel = DEMUX_CH_A;
        else
            sdec_channel = DEMUX_CH_B;

        if(currentConnect == HAL_AUDIO_RESOURCE_ATP0)
        {
            sdec_dest = RSDEC_ENUM_TYPE_DEST_ADEC0;
        }
        else
        {
            sdec_dest = RSDEC_ENUM_TYPE_DEST_ADEC1;
        }

        KADP_SDEC_DisConnect(sdec_channel, sdec_dest);
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ADC_Connect(HAL_AUDIO_BBADC_SRC portNum)
{
    SINT32 i;
    HAL_AUDIO_RESOURCE_T optResourceId[2];
    SINT32 totalOutputConnectResource;
    INFO("%s port %d\n", __func__, portNum);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(portNum >= (AUDIO_BBADC_SRC_AIO2_PORT_NUM + 1))
        return NOT_OK;

    // check if other pins are also at connect status
    for(i = 0; i < MAIN_AIN_ADC_PIN; i++)
    {
        if(AinStatus.ainPinStatus[i] == HAL_AUDIO_RESOURCE_CONNECT)
        {
            ERROR("Error ADC too much connect %d %d\n",i, AinStatus.ainPinStatus[i]);
            return NOT_OK;
        }
    }

    if( AUDIO_CHECK_ADC_PIN_CONNECT_NOTAVAILABLE(AinStatus.ainPinStatus[portNum]))
    {
         ERROR("Error ADC pin %d  is still at %s state\n",portNum, GetResourceStatusString(AinStatus.ainPinStatus[portNum]));
         return NOT_OK;
    }

    GetConnectOutputSource(HAL_AUDIO_RESOURCE_ADC, optResourceId, 2, &totalOutputConnectResource);
    if(totalOutputConnectResource > 2)
    {
        ERROR("ADC output connect error %d\n", totalOutputConnectResource);
        return NOT_OK;
    }

    if(SetResourceConnect(HAL_AUDIO_RESOURCE_ADC, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    for(i = 0; i < totalOutputConnectResource; i++)
    {
        if(IsADECSource(optResourceId[i]))
        {
            HAL_AUDIO_ADEC_INDEX_T adecIndex = res2adec(optResourceId[i]);
            Aud_Adec_info[adecIndex].prevADCPortNum = Aud_Adec_info[adecIndex].curADCPortNum;
            Aud_Adec_info[adecIndex].curADCPortNum  = portNum;
        }
    }

    AinStatus.ainPinStatus[portNum] = HAL_AUDIO_RESOURCE_CONNECT;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ADC_Disconnect(HAL_AUDIO_BBADC_SRC portNum)
{
    INFO("%s port %d\n", __func__,  portNum);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(portNum >= (AUDIO_BBADC_SRC_AIO2_PORT_NUM + 1))
        return NOT_OK;

    if(AUDIO_CHECK_ADC_PIN_DISCONNECT_NOTAVAILABLE(AinStatus.ainPinStatus[portNum]))
    {
        ERROR("Error ADC pin %d is still at %s state\n",portNum, GetResourceStatusString(AinStatus.ainPinStatus[portNum]));
        return NOT_OK;
    }

    if(SetResourceDisconnect(HAL_AUDIO_RESOURCE_ADC, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    AinStatus.ainPinStatus[portNum] = HAL_AUDIO_RESOURCE_DISCONNECT;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_Connect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_HDMI;

    if(ResourceStatus[id].connectStatus[0] == HAL_AUDIO_RESOURCE_CONNECT)
    {
        ERROR("Error HDMI too much connect %d %d\n", id, ResourceStatus[id].connectStatus[0]);
        return NOT_OK;
    }

    if(SetResourceConnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_Disconnect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_HDMI;

    if(SetResourceDisconnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DP_Connect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_DP;

    if(SetResourceConnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DP_Disconnect(void)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = HAL_AUDIO_RESOURCE_DP;

    if(SetResourceDisconnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}
DTV_STATUS_T RHAL_AUDIO_DP_ConnectPort(HAL_AUDIO_DP_INDEX_T DPIndex)
{
    SINT32 i;
    HAL_AUDIO_RESOURCE_T optResourceId[2];
    SINT32 totalOutputConnectResource;
    HAL_AUDIO_RESOURCE_T id;

    INFO("%s %d\n", __func__, DPIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(DPIndex > HAL_AUDIO_DP_MAX)
    {
        ERROR("error DP port\n");
        return NOT_OK;
    }
    id = (HAL_AUDIO_RESOURCE_T)(HAL_AUDIO_RESOURCE_DP + DPIndex);

    if (IsDPportConnected(DPIndex)) {
        ERROR("DP port %d connect fail, %s status is at %s \n", DPIndex, GetResourceString(id),
                GetResourceStatusString(ResourceStatus[id].connectStatus[0]));

        return NOT_OK;
    }

    GetConnectOutputSource(id, optResourceId, 2, &totalOutputConnectResource);
    if(totalOutputConnectResource > 2)
    {
        ERROR("DPport output connect error %d\n", totalOutputConnectResource);
        return NOT_OK;
    }

    for(i = 0; i < totalOutputConnectResource; i++)
    {
        if(IsADECSource(optResourceId[i]))
        {
            HAL_AUDIO_ADEC_INDEX_T adecIndex = res2adec(optResourceId[i]);
            Aud_Adec_info[adecIndex].prevHDMIPortNum = Aud_Adec_info[adecIndex].curHDMIPortNum;
            Aud_Adec_info[adecIndex].curHDMIPortNum  = (HAL_AUDIO_HDMI_INDEX_T)DPIndex;
        }
    }

    if(SetResourceConnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DP_DisconnectPort(HAL_AUDIO_DP_INDEX_T DPIndex)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s %d\n", __func__ , DPIndex);

    if(DPIndex > HAL_AUDIO_DP_MAX)
    {
        ERROR("error DP port\n");
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = (HAL_AUDIO_RESOURCE_T)((UINT32)HAL_AUDIO_RESOURCE_DP + (UINT32)DPIndex);

    if(SetResourceDisconnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_ConnectPort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex)
{
    SINT32 i;
    HAL_AUDIO_RESOURCE_T optResourceId[2];
    SINT32 totalOutputConnectResource;
    HAL_AUDIO_RESOURCE_T id;

    INFO("%s %d\n", __func__, hdmiIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(hdmiIndex > HAL_AUDIO_HDMI_MAX)
    {
        ERROR("error hdmi port\n");
        return NOT_OK;
    }
    id = (HAL_AUDIO_RESOURCE_T)(HAL_AUDIO_RESOURCE_HDMI0 + hdmiIndex);

    if (IsHDMIportConnected(hdmiIndex)) {
        ERROR("HDMI port %d connect fail, %s status is at %s \n", hdmiIndex, GetResourceString(id),
                GetResourceStatusString(ResourceStatus[id].connectStatus[0]));

        return NOT_OK;
    }

    GetConnectOutputSource(id, optResourceId, 2, &totalOutputConnectResource);
    if(totalOutputConnectResource > 2)
    {
        ERROR("hdmiport output connect error %d\n", totalOutputConnectResource);
        return NOT_OK;
    }

    for(i = 0; i < totalOutputConnectResource; i++)
    {
        if(IsADECSource(optResourceId[i]))
        {
            HAL_AUDIO_ADEC_INDEX_T adecIndex = res2adec(optResourceId[i]);
            Aud_Adec_info[adecIndex].prevHDMIPortNum = Aud_Adec_info[adecIndex].curHDMIPortNum;
            Aud_Adec_info[adecIndex].curHDMIPortNum  = hdmiIndex;
        }
    }

    if(SetResourceConnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_DisconnectPort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex)
{
    HAL_AUDIO_RESOURCE_T id;
    INFO("%s %d\n", __func__ , hdmiIndex);

    if(hdmiIndex > HAL_AUDIO_HDMI_MAX)
    {
        ERROR("error hdmi port\n");
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    id = (HAL_AUDIO_RESOURCE_T)((UINT32)HAL_AUDIO_RESOURCE_HDMI0 + (UINT32)hdmiIndex);

    if(SetResourceDisconnect(id, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AAD_Connect(void)
{
    INFO("%s Enter.\n", __func__);

    if (rhal_type == RHAL_LITE) //for sina
        KADP_Audio_AtvEnterAtvSource();// need to modify later
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(SetResourceConnect(HAL_AUDIO_RESOURCE_AAD, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;

    if (rhal_type != RHAL_LITE)
        KADP_Audio_AtvEnterAtvSource();// need to modify later
    //g_AudioSIFInfo.sifSource      = HAL_AUDIO_SIF_INPUT_INTERNAL; //No need to reset when AAD Connect
    //g_AudioSIFInfo.curSifType     = HAL_AUDIO_SIF_TYPE_NONE; //clear on Finalize...
    g_AudioSIFInfo.bHighDevOnOff  = FALSE;
    g_AudioSIFInfo.curSifBand     = HAL_AUDIO_SIF_SYSTEM_UNKNOWN;
    g_AudioSIFInfo.curSifStandard = HAL_AUDIO_SIF_MODE_DETECT;
    g_AudioSIFInfo.curSifIsA2     = HAL_AUDIO_SIF_DETECTING_EXSISTANCE;
    g_AudioSIFInfo.curSifIsNicam  = HAL_AUDIO_SIF_DETECTING_EXSISTANCE;
    g_AudioSIFInfo.curSifModeSet  = HAL_AUDIO_SIF_SET_PAL_UNKNOWN;
    g_AudioSIFInfo.curSifModeGet  = HAL_AUDIO_SIF_GET_PAL_UNKNOWN;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AAD_Disconnect(void)
{
    INFO("%s Enter.\n", __func__);

    // To make sure ATV thread cannot invoke RAL_ATVSetDecoderXMute callback and cause deadlock by //AUDIO_FUNC_CALL()
    KADP_Audio_AtvPauseTvStdDetection(true);

    if (rhal_type == RHAL_LITE)
        KADP_Audio_AtvCleanTVSourceData();
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
    {
        // release sem
        KADP_Audio_AtvPauseTvStdDetection(false);
        return NOT_OK;
    }

    if(SetResourceDisconnect(HAL_AUDIO_RESOURCE_AAD, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
    {
        // release sem
        KADP_Audio_AtvPauseTvStdDetection(false);
        return NOT_OK;
    }

    if (rhal_type != RHAL_LITE)
        KADP_Audio_AtvCleanTVSourceData();

    // release sem
    KADP_Audio_AtvPauseTvStdDetection(false);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SYSTEM_Connect(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(SetResourceConnect(HAL_AUDIO_RESOURCE_SYSTEM, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;
    return OK;// or NOT_OK
}

DTV_STATUS_T RHAL_AUDIO_SYSTEM_Disconnect(void)
{
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(SetResourceDisconnect(HAL_AUDIO_RESOURCE_SYSTEM, HAL_AUDIO_RESOURCE_NO_CONNECTION) != TRUE)
        return NOT_OK;
    return OK;// or NOT_OK
}

static void UpdateADECStatus(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    HAL_AUDIO_ADEC_INDEX_T adecIndex = res2adec(currentConnect);
    HAL_AUDIO_IN_PORT_T tempport;

    if(IsDTVSource(inputConnect))
    {
        if((ResourceStatus[inputConnect].connectStatus[0] != HAL_AUDIO_RESOURCE_CONNECT))  // check ATP is at connect status
        {

        }
        else
        {
            // at connect status
            HAL_AUDIO_RESOURCE_T atpInputSource;
            atpInputSource = GetNonOutputModuleSingleInputResource(ResourceStatus[inputConnect]);
            if((atpInputSource == HAL_AUDIO_RESOURCE_SDEC0) || (atpInputSource == HAL_AUDIO_RESOURCE_SDEC1))
            {
                Aud_Adec_info[adecIndex].prevTPInputPort = Aud_Adec_info[adecIndex].curTPInputPort;
                Aud_Adec_info[adecIndex].curTPInputPort  = atpInputSource;
            }
        }

        tempport = HAL_AUDIO_IN_PORT_TP;
    }
    else if(IsATVSource(inputConnect))
    {
        tempport = HAL_AUDIO_IN_PORT_SIF;
    }
    else if(IsADCSource(inputConnect))
    {
        if((ResourceStatus[inputConnect].connectStatus[0] != HAL_AUDIO_RESOURCE_CONNECT))// check at connect status
        {
            //  adc connect will update the port status
        }
        else
        {   // ADC is at  connected status
            Aud_Adec_info[adecIndex].prevADCPortNum = Aud_Adec_info[adecIndex].curADCPortNum;
            Aud_Adec_info[adecIndex].curADCPortNum  = GetCurrentADCConnectPin();
        }

        tempport = HAL_AUDIO_IN_PORT_ADC;
    }
    else if(IsSystemSource(inputConnect))
    {
        tempport = HAL_AUDIO_IN_PORT_SYSTEM;
    }
    else if(IsDPSource(inputConnect))
    {
        tempport = HAL_AUDIO_IN_PORT_DP;
    }
    else if(IsHDMISource(inputConnect))
    {
        tempport = HAL_AUDIO_IN_PORT_HDMI;

        if((ResourceStatus[inputConnect].connectStatus[0] != HAL_AUDIO_RESOURCE_CONNECT))// check at connect status
        {
            //  hdmi connect will update the port status
        }
        else
        {   // ADC is at  connected status
            if(inputConnect >= HAL_AUDIO_RESOURCE_HDMI0)
            {
            	SINT32 hdmiconnectsource;
                Aud_Adec_info[adecIndex].prevHDMIPortNum = Aud_Adec_info[adecIndex].curHDMIPortNum;
                hdmiconnectsource = GetCurrentHDMIConnectPin();
                if(hdmiconnectsource == (SINT32)HAL_AUDIO_RESOURCE_HDMI)
                {
                    INFO("unknow hdmi port\n");
                    Aud_Adec_info[adecIndex].curHDMIPortNum = HAL_AUDIO_HDMI0;
                }
                else
                    Aud_Adec_info[adecIndex].curHDMIPortNum = (HAL_AUDIO_HDMI_INDEX_T)(hdmiconnectsource - HAL_AUDIO_RESOURCE_HDMI0);
            }
        }
    }
    else
    {
        ERROR("error ipt index %d\n", inputConnect);
        return;
    }
    Aud_Adec_info[adecIndex].prevAdecInputPort = Aud_Adec_info[adecIndex].curAdecInputPort;
    Aud_Adec_info[adecIndex].curAdecInputPort  = tempport;
}

DTV_STATUS_T RHAL_AUDIO_ADEC_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    INFO("%s cur %s ipt %s\n", __func__,
                GetResourceString(currentConnect), GetResourceString(inputConnect));

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!IsADECSource(currentConnect))
    {
        ERROR("[Error] %s != ADEC\n", GetResourceString(currentConnect));
        return NOT_OK;
    }

    if(IsValidADECIpts(inputConnect) != TRUE)
    {
        ERROR("[Error] Invalid input %s\n", GetResourceString(inputConnect));
        return NOT_OK;
    }

    if(SetResourceConnect(currentConnect, inputConnect) != TRUE)
        return NOT_OK;

    UpdateADECStatus(currentConnect, inputConnect);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ADEC_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    INFO("%s cur %s ipt %s\n", __func__,
                GetResourceString(currentConnect), GetResourceString(inputConnect));

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!IsADECSource(currentConnect))
        return NOT_OK;

    if(SetResourceDisconnect(currentConnect, inputConnect) != TRUE)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AMIX_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AMIX_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SE_Connect(HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SE_Disconnect(HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SNDOUT_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SNDOUT_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect)
{
    return OK;
}

static BOOLEAN AddAndConnectDecAinToFlow(FlowManager* pFlowManager, HAL_AUDIO_ADEC_INDEX_T adecIndex, Base* pAin, HAL_AUDIO_FLOW_STATUS* pFlowStatus)
{
    if(pFlowManager == NULL)
        return FALSE;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return FALSE;

    if(pAin == NULL)
        return FALSE;

    if(pFlowStatus == NULL)
        return FALSE;

    pFlowStatus->IsAINExist = TRUE;
    pFlowStatus->IsDECExist = TRUE;
    pFlowManager->Connect(pFlowManager, pAin, Aud_dec[adecIndex]);
    return TRUE;
}

static BOOLEAN AddAndConnectPPAOToFlow(FlowManager* pFlowManager, HAL_AUDIO_ADEC_INDEX_T adecIndex, Base* pPPAO, HAL_AUDIO_FLOW_STATUS* pFlowStatus)
{
    if(pFlowManager == NULL)
        return FALSE;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return FALSE;

    if (pPPAO == NULL)
        return FALSE;

    if(pFlowStatus == NULL)
        return FALSE;

    pFlowStatus->IsPPAOExist = TRUE;
    pFlowManager->Connect(pFlowManager, Aud_dec[adecIndex], pPPAO);
    return TRUE;
}

static BOOLEAN AddAndConnectDTVSourceFilterDecToFlow(FlowManager* pFlowManager, HAL_AUDIO_ADEC_INDEX_T adecIndex, Base* pAud_DTV, HAL_AUDIO_FLOW_STATUS* pFlowStatus)
{
    HAL_AUDIO_RESOURCE_T curResourceId, decIptResId;

    if(pFlowManager == NULL)
        return FALSE;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return FALSE;

    if(pFlowStatus == NULL)
        return FALSE;

    curResourceId = adec2res(adecIndex);
    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curResourceId]);// dec input source

    if(decIptResId == HAL_AUDIO_RESOURCE_ATP0)
        pFlowStatus->IsDTV0SourceRead = TRUE;
    else if(decIptResId == HAL_AUDIO_RESOURCE_ATP1)
        pFlowStatus->IsDTV1SourceRead = TRUE;
    else
    {
        AUDIO_FATAL("[AUDH-FATAL] unknow atp resource id %d\n", decIptResId);
    }

    pFlowStatus->IsDECExist = TRUE;

    return TRUE;
}

static BOOLEAN SwitchADCFocus(Base* pAin)
{
    SINT32 adcInputPin;
    if((adcInputPin = GetCurrentADCConnectPin()) >= 0)
    {
        AUDIO_IPT_SRC audioInput;
        memset(&audioInput, 0, sizeof(AUDIO_IPT_SRC));

        audioInput.focus[0] = AUDIO_IPT_SRC_BBADC;
        audioInput.focus[1] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[2] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[3] = AUDIO_IPT_SRC_UNKNOWN;

        if(adcInputPin == AUDIO_BBADC_SRC_AIO_PORT_NUM)
            audioInput.mux_in = AUDIO_BBADC_SRC_AIO1;
        else if(adcInputPin == AUDIO_BBADC_SRC_AIN1_PORT_NUM)
            audioInput.mux_in = AUDIO_BBADC_SRC_AIN1;
        else if(adcInputPin == AUDIO_BBADC_SRC_AIN2_PORT_NUM)
            audioInput.mux_in = AUDIO_BBADC_SRC_AIN2;
        else if(adcInputPin == AUDIO_BBADC_SRC_AIN3_PORT_NUM)
            audioInput.mux_in = AUDIO_BBADC_SRC_AIN3;
        else
        {
            audioInput.mux_in = AUDIO_BBADC_SRC_MUTE_ALL;
            INFO("ain mut in set mute %x\n",  audioInput.mux_in);
        }
        INFO("ADC connect pin %d  set fw ain mut in index = %x\n",  adcInputPin, audioInput.mux_in);

        pAin->SwitchFocus(pAin, &audioInput);
        return TRUE;
    }
    else
    {
        ERROR("find adc pin failed\n");
        return FALSE;
    }
}

#if !defined(CONFIG_ARCH_RTK2819A)
static BOOLEAN SwitchHDMIFocus(Base* pAin)
{
    SINT32 hdmiInputPin;
    if((hdmiInputPin = GetCurrentHDMIConnectPin()) >= 0)
    {
        AUDIO_IPT_SRC audioInput;
        memset(&audioInput, 0, sizeof(AUDIO_IPT_SRC));

        audioInput.focus[0] = AUDIO_IPT_SRC_SPDIF;
        audioInput.focus[1] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[2] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[3] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.mux_in = AUDIO_SPDIFI_SRC_HDMI;
        pAin->SwitchFocus(pAin, &audioInput);
        return TRUE;
    }
    else
    {
        ERROR("find hdmi pin failed\n");
        return FALSE;
    }
}
static BOOLEAN SwitchDPFocus(Base* pAin)
{
    if (GetCurrentDPConnectPin() >= 0) {
        AUDIO_IPT_SRC audioInput;
        memset(&audioInput, 0, sizeof(AUDIO_IPT_SRC));

        audioInput.focus[0] = AUDIO_IPT_SRC_SPDIF;
        audioInput.focus[1] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[2] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[3] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.mux_in = AUDIO_SPDIFI_SRC_DISPLAY_PORT;
        pAin->SwitchFocus(pAin, &audioInput);
        return TRUE;
    }
    else
    {
        ERROR("find display port pin failed\n");
        return FALSE;
    }
}
#endif
static BOOLEAN SwitchATVFocus(Base* pAin)
{
    AUDIO_IPT_SRC audioInput;
    memset(&audioInput, 0, sizeof(AUDIO_IPT_SRC));

    audioInput.focus[0] = AUDIO_IPT_SRC_ATV;
    audioInput.focus[1] = AUDIO_IPT_SRC_UNKNOWN;
    audioInput.focus[2] = AUDIO_IPT_SRC_UNKNOWN;
    audioInput.focus[3] = AUDIO_IPT_SRC_UNKNOWN;
    pAin->SwitchFocus(pAin, &audioInput);

    return TRUE;
}

static BOOLEAN SwitchI2SINFocus(Base* pAin)
{
    AUDIO_IPT_SRC audioInput;
    memset(&audioInput, 0, sizeof(AUDIO_IPT_SRC));

    audioInput.focus[0] = AUDIO_IPT_SRC_I2S_PRI_CH12;
    audioInput.focus[1] = AUDIO_IPT_SRC_UNKNOWN;
    audioInput.focus[2] = AUDIO_IPT_SRC_UNKNOWN;
    audioInput.focus[3] = AUDIO_IPT_SRC_UNKNOWN;
    pAin->SwitchFocus(pAin, &audioInput);

    return TRUE;
}

static BOOLEAN IsAINFilterUsed(int idx)
{
    if(AinStatus.usedAud_Ain[idx])
        return TRUE;
    else
        return FALSE;
}

static UINT32 Query_Aud_Ain_Object_id(UINT32 startid)
{
    UINT32 i;

    for (i = startid; i < AUD_AIN_MAX; i++)
    {
        if (Aud_Ain[i] && !IsAINFilterUsed(i)) {
            AinStatus.usedAud_Ain[i] = TRUE;
            INFO("Found a available AIN%d\n", i);
            break;
        } else if (Aud_Ain[i] == NULL) {
            Aud_Ain[i] = new_AIN(i);
            if (Aud_Ain[i] == NULL) {
                ERROR("new_AIN idx %d fail\n", i);
                i = AUD_AIN_MAX;
                return i;
            }
            AinStatus.usedAud_Ain[i] = TRUE;
            INFO("To New a AIN%d\n", i);
            break;
        }
    }

    return i;
}

#if defined(CONFIG_ARCH_RTK2819A)
static UINT32 SetVideoCommonId(SINT32 instanceID, UINT32 port, UINT32 source, UINT32 mux_in)
{
    struct AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
    AUDIO_RPC_PRIVATEINFO_RETURNVAL ret;
    VIDEO_COMMON_PORT video_common_id;

    memset(&parameter, 0, sizeof(struct AUDIO_RPC_PRIVATEINFO_PARAMETERS));
    memset(&ret, 0, sizeof(AUDIO_RPC_PRIVATEINFO_RETURNVAL));

    video_common_id = drvif_video_common_get_common_port(source, port);
    parameter.instanceID = instanceID;
    parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_VIDEO_COMMON_ID;
    parameter.privateInfo[0] = video_common_id;
    parameter.privateInfo[1] = mux_in;

    if (KADP_AUDIO_PrivateInfo(&parameter, &ret) != KADP_OK) {
        ERROR("[%s,%d] KADP failed\n", __func__, __LINE__);
        return NOT_OK;
    }

    INFO("set video common id:%d, port:%d, source:%d, mux_in:%d instanceID:%x Success.\n", video_common_id, port, source, mux_in, instanceID);
    return OK;
}
#endif

BOOLEAN RemoveFilter(HAL_AUDIO_FLOW_STATUS* prevStatus, FlowManager* pFlowManager)
{
    if(prevStatus->IsAINExist)
    {
        UINT32 idx = prevStatus->usedAud_Ainidx;
        INFO("remove Aud_Ain[%d]\n", idx);
        pFlowManager->Remove(pFlowManager, Aud_Ain[idx]);
        prevStatus->IsAINExist = 0;
        AinStatus.usedAud_Ain[idx] = FALSE;
    }

    if(prevStatus->IsDTV0SourceRead)
    {
        /* Reset external reference clock */
        pFlowManager->SetExtRefClock(pFlowManager, 0);
    }
    prevStatus->IsDTV0SourceRead = 0;

    if(prevStatus->IsDTV1SourceRead)
    {
        /* Reset external reference clock */
        pFlowManager->SetExtRefClock(pFlowManager, 0);
    }
    prevStatus->IsDTV1SourceRead = 0;

    if(prevStatus->IsDECExist)
    {
        UINT32 idx = prevStatus->usedAud_decidx;
        INFO("remove Aud_dec[%d]\n", idx);
        pFlowManager->Remove(pFlowManager, Aud_dec[idx]);
        prevStatus->IsDECExist = 0;
    }

    if(prevStatus->IsPPAOExist)
    {
        UINT32 idx = prevStatus->usedAud_Aoutidx;
        INFO("remove Aud_Aout[%d]\n", idx);
        pFlowManager->Remove(pFlowManager, Aud_Aout[idx]);
        prevStatus->IsPPAOExist = 0;
    }

    return TRUE;
}

DTV_STATUS_T RAL_AUDIO_StopDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    HAL_AUDIO_FLOW_STATUS* pCurFlowStatus;
    FlowManager* pFlow;
    HAL_AUDIO_RESOURCE_T curResourceId;
    HAL_AUDIO_RESOURCE_T decIptResId;
    INFO("%s ADEC%d\n", __func__, adecIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    curResourceId = adec2res(adecIndex);

    if(AUDIO_HAL_CHECK_ISATSTOPSTATE(ResourceStatus[curResourceId], 0))
    {
        INFO("ADEC %d is at stop state\n", adecIndex);
        return OK;
    }

    if(AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0)) // decoder only has one input
    {
        ERROR("%s still at %s status\n", GetResourceString(curResourceId),
                GetResourceStatusString(ResourceStatus[curResourceId].connectStatus[0]));
        return NOT_OK;
    }

    if (adecIndex == Aud_mainDecIndex) {
        INFO("before Stop Decoding flow DEC%d (main) info::\n", Aud_mainDecIndex);
        Aud_flow[Aud_mainDecIndex]->ShowCurrentExitModule(Aud_flow[Aud_mainDecIndex], 0);
    } else {
        INFO("before Stop Decoding flow DEC%d (sub) info::\n", adecIndex);
        Aud_flow[adecIndex]->ShowCurrentExitModule(Aud_flow[adecIndex], 0);
    }

    ShowFlow(adec2res(adecIndex), HAL_AUDIO_RESOURCE_STOP, 0);
    ResourceStatus[curResourceId].connectStatus[0] = HAL_AUDIO_RESOURCE_STOP;

    // update adec status info
    Aud_Adec_info[adecIndex].bAdecStart = FALSE;
    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curResourceId]);

    pFlow = Aud_flow[adecIndex];
    pCurFlowStatus = &FlowStatus[adecIndex];

    pFlow->Stop(pFlow);

    if(IsATVSource(decIptResId))
    {
        KADP_Audio_AtvSetAudioInHandle(0); // remove AIN filter
        g_AudioSIFInfo.curSifStandard = HAL_AUDIO_SIF_MODE_DETECT;
        KADP_Audio_AtvStopDecoding();
    }

    if(IsAinSource(decIptResId))
    {
        AUDIO_IPT_SRC audioInput;
        UINT32 idx = pCurFlowStatus->usedAud_Ainidx;
        memset(&audioInput, 0, sizeof(AUDIO_IPT_SRC));

        audioInput.focus[0] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[1] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[2] = AUDIO_IPT_SRC_UNKNOWN;
        audioInput.focus[3] = AUDIO_IPT_SRC_UNKNOWN;

        if (Aud_Ain[idx])
            Aud_Ain[idx]->SwitchFocus(Aud_Ain[idx], &audioInput); // unfocus
        else
            INFO("SwitchFocus Aud_Ain[%d] fail\n", idx);
    }

    RemoveFilter(pCurFlowStatus, pFlow);

    if (adecIndex == Aud_mainDecIndex) {
        INFO("after Stop Decoding flow DEC%d (main) info::\n", Aud_mainDecIndex);
        Aud_flow[Aud_mainDecIndex]->ShowCurrentExitModule(Aud_flow[Aud_mainDecIndex], 0);
    } else {
        INFO("after Stop Decoding flow DEC%d (sub) info::\n", adecIndex);
        Aud_flow[adecIndex]->ShowCurrentExitModule(Aud_flow[adecIndex], 0);
    }

    if (IsAinSource(decIptResId) == TRUE) {
        HAL_AUDIO_ADEC_INDEX_T idx = pCurFlowStatus->usedAud_Ainidx;
        if(Aud_Ain[idx] != NULL && (idx != 0)) //Do not Delete main AIN, only dynamic created is required.
        {
            INFO("remove usedAud_Ain[%d] on ADEC%d\n", idx, adecIndex);
            Aud_Ain[idx]->Stop(Aud_Ain[idx]);
            Aud_Ain[idx]->Delete(Aud_Ain[idx]);
            Aud_Ain[idx] = NULL;
        }
    }

    if (adecIndex >= AUD_ADEC_INIT_MAX) {
        if(Aud_dec[adecIndex] != NULL)
        {
            Aud_dec[adecIndex]->Stop(Aud_dec[adecIndex]);
            Aud_dec[adecIndex]->Delete(Aud_dec[adecIndex]);
            Aud_dec[adecIndex] = NULL;
        }

        if(Aud_Aout[adecIndex] != NULL)
        {
            Aud_Aout[adecIndex]->Stop(Aud_Aout[adecIndex]);
            Aud_Aout[adecIndex]->Delete(Aud_Aout[adecIndex]);
            Aud_Aout[adecIndex] = NULL;
        }
    }

    return OK;
}

static DTV_STATUS_T RAL_AUDIO_StartDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T audioType, SINT32 force2reStart)
{
    HAL_AUDIO_RESOURCE_T curDecResourceId, decIptResId;
    HAL_AUDIO_FLOW_STATUS* pCurFlowStatus;
    Base* pAin = NULL;
    Base* pPPAo;
    FlowManager* pFlow;
    SINT32 errnum;

    INFO("%s ADEC%d type %s mainDecIndex=%d\n", __func__, adecIndex, GetSRCTypeName(audioType), Aud_mainDecIndex);

    curDecResourceId = adec2res(adecIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if(audioType > HAL_AUDIO_SRC_TYPE_OPUS)
    {
        ERROR("unknow dec type %d\n", audioType);
        return NOT_OK;
    }

    ShowFlow(adec2res(adecIndex), HAL_AUDIO_RESOURCE_RUN, 0);

    // open ADEC ready check
    if(AUDIO_HAL_CHECK_ISATRUNSTATE(ResourceStatus[curDecResourceId], 0))
    {
        if((force2reStart == 0) && audioType == GetCurrentFormat(adecIndex))
        {
            INFO("ADEC %d is at run state and format %d is same\n", adecIndex, audioType);
            return OK;
        }
        else
        {
            INFO("ADEC %d is at run state and format change(%d->%d) , need auto stop\n", adecIndex, GetCurrentFormat(adecIndex), audioType);
            RAL_AUDIO_StopDecoding(adecIndex);
        }
    }

    if(AUDIO_HAL_CHECK_PLAY_NOTAVAILABLE(ResourceStatus[curDecResourceId], 0))
    {
        ERROR("%s status is at %s\n", GetResourceString(curDecResourceId),
                GetResourceStatusString(ResourceStatus[curDecResourceId].connectStatus[0]));
        return NOT_OK;
    }

    if (Aud_flow[adecIndex] == NULL) {
        Aud_flow[adecIndex] = new_flow();
        if (Aud_flow[adecIndex] == NULL) {
            ERROR("new_flow idx %d fail\n", adecIndex);
            return NOT_OK;
        }
        INFO("To New a Aud_flow[%d]\n", adecIndex);
    }

    pFlow = Aud_flow[adecIndex];
    pCurFlowStatus = &FlowStatus[adecIndex];
    if(pCurFlowStatus == NULL || pFlow == NULL)
    {
        ERROR("Flow usage is NULL check pCurFlowStatus:%x, pFlow:%x\n", pCurFlowStatus, pFlow);
        errnum = 0;
        goto error;
    }

    if (adecIndex == Aud_mainDecIndex) {
        INFO("before Start Decoding flow DEC%d (main) info::\n", Aud_mainDecIndex);
        Aud_flow[Aud_mainDecIndex]->ShowCurrentExitModule(Aud_flow[Aud_mainDecIndex], 0);
    } else {
        INFO("before Start Decoding flow DEC%d (sub) info::\n", adecIndex);
        Aud_flow[adecIndex]->ShowCurrentExitModule(Aud_flow[adecIndex], 0);
    }

    pFlow->Stop(pFlow);
    RemoveFilter(pCurFlowStatus, pFlow);

    /* Check Input source and Error handling */
    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curDecResourceId]);// dec input source

    if (IsAinSource(decIptResId) == TRUE) {
        UINT32 i;

#if defined(CONFIG_ARCH_RTK2819A)
        if (IsHDMISource(decIptResId) || IsDPSource(decIptResId)) {
            UINT32 source;
            UINT32 port;
            UINT32 mux_in;
            SINT32 instanceID;

            if (IsHDMISource(decIptResId)) {
                port = res2hdmi(decIptResId);
                source = VIDEO_COMMON_HDMIRX;
                mux_in = AUDIO_SPDIFI_SRC_HDMI;
            } else {
                port = res2dp(decIptResId);
                source = VIDEO_COMMON_DPRX;
                mux_in = AUDIO_SPDIFI_SRC_DISPLAY_PORT;
            }

            if (IsHDMIFullCapPort(port) && AinStatus.usedAud_Ain[0] == FALSE) {
                INFO("%s port %d is HDMI fullcaps fullcaps and not used\n", __func__, port);
                if (Aud_Ain[0] == NULL) {
                    Aud_Ain[0] = new_AIN(0);
                    if (Aud_Ain[0] == NULL) {
                        ERROR("new_AIN idx 0 fail\n");
                        return NOT_OK;
                    }
                }
                AinStatus.usedAud_Ain[0] = TRUE;
                instanceID = Aud_Ain[0]->instanceID;
                i = 0;
            } else {
                INFO("%s port %d is not HDMI fullcaps\n", __func__, port);
                i = Query_Aud_Ain_Object_id(1);
                if (i == AUD_AIN_MAX) {
                    ERROR("%s fail, Aud_Ain not available\n", __func__);
                    return NOT_OK;
                }
                instanceID = Aud_Ain[i]->instanceID;
            }

            if (SetVideoCommonId(instanceID, port, source, mux_in) == NOT_OK)
                return NOT_OK;
        }else {
            i = Query_Aud_Ain_Object_id(1);
            if (i == AUD_AIN_MAX) {
                errnum = 1;
                goto error;
            }
        }
#else
        i = Query_Aud_Ain_Object_id(0);
        if (i == AUD_AIN_MAX) {
            errnum = 1;
            goto error;
        }
#endif
        pAin = Aud_Ain[i];
        pCurFlowStatus->usedAud_Ainidx = i;
        pCurFlowStatus->IsAINExist = TRUE;

        if (pAin == NULL) {
            ERROR("Flow usage is NULL (pAin NULL)");
            errnum = 2;
            goto error;
        }
        INFO("used Aud_Ain idx=%d\n", i);
    }

    if (Aud_dec[adecIndex] == NULL) {
        ERROR("Aud_dec idx %d is NULL\n", adecIndex);
        errnum = 3;
        goto error;
    }
    pCurFlowStatus->usedAud_decidx = adecIndex;
    INFO("used Aud_dec idx=%d\n", adecIndex);
    if (Aud_Aout[adecIndex] == NULL) {
        Aud_Aout[adecIndex] = new_PPAO(PPAO_FULL, adecIndex);
        if (Aud_Aout[adecIndex] == NULL) {
            ERROR("new_PPAO idx %d fail\n", adecIndex);
            errnum = 4;
            goto error;
        }
        INFO("To New a Aud_Aout[%d]\n", adecIndex);
    }
    pCurFlowStatus->usedAud_Aoutidx = adecIndex;
    INFO("used Aud_Aout idx=%d\n", adecIndex);

    pPPAo = Aud_Aout[adecIndex];

    if(pPPAo == NULL )
    {
        ERROR("Flow usage is NULL, check pPPAo\n");
        errnum = 5;
        goto error;
    }

    if(IsAinSource(decIptResId) || IsDTVSource(decIptResId) || IsSystemSource(decIptResId))
    {
        INFO("dec input is %s\n", GetResourceString(decIptResId));
    }
    else
    {
        ERROR("[Error] Invalid dec input src %s\n", GetResourceString(decIptResId));
        errnum = 6;
        goto error;
    }
    /* End of Input source checking */

    /* Connect to Input Source */
    if(IsAinSource(decIptResId) == TRUE)
    {
        AddAndConnectDecAinToFlow(pFlow, adecIndex, pAin, pCurFlowStatus);

        if(IsADCSource(decIptResId) && SwitchADCFocus(pAin) != TRUE)
        {   errnum = 7;
            goto error;
        }
#if !defined(CONFIG_ARCH_RTK2819A)
        else if(IsHDMISource(decIptResId))
        {
            if(SwitchHDMIFocus(pAin) != TRUE) {
                errnum= 8;
                goto error;
            }
        }
        else if(IsDPSource(decIptResId))
        {
            if(SwitchDPFocus(pAin) != TRUE) {
                errnum= 9;
                goto error;
            }
        }
#endif
        else if(IsATVSource(decIptResId))
        {
            SwitchATVFocus(pAin);

            if (adecIndex == Aud_mainDecIndex)
                KADP_Audio_AtvSetAudioInHandle(pAin->GetAgentID(pAin));
            else
                KADP_Audio_AtvSetSubAudioInHandle(pAin->GetAgentID(pAin));// sub ain

            g_AudioSIFInfo.curSifStandard = HAL_AUDIO_SIF_MODE_DETECT;
            KADP_Audio_AtvStartDecoding();
        }
        else if(IsI2SINSource(decIptResId))
        {
             if(SwitchI2SINFocus(pAin) != TRUE) {
                errnum= 10;
                goto error;
            }
        }
    }
    else if(IsDTVSource(decIptResId))
    {
        SINT32 atpindex = decIptResId - HAL_AUDIO_RESOURCE_ATP0;
        DEC* pDEC = (DEC*)Aud_dec[atpindex]->pDerivedObj;
        DtvCom* pDtvCom = (DtvCom*)Aud_DTV[atpindex]->pDerivedObj;

        AddAndConnectDTVSourceFilterDecToFlow(pFlow, adecIndex, Aud_DTV[atpindex], pCurFlowStatus);

        pFlow->SetExtRefClock(pFlow, pDtvCom->GetRefClockPhyAddress(Aud_DTV[atpindex]));
        pDEC->InitBSRingBuf(Aud_dec[adecIndex], pDtvCom->GetBSRingBufPhyAddress(Aud_DTV[atpindex]));
        pDEC->InitICQRingBuf(Aud_dec[adecIndex], pDtvCom->GetICQRingBufPhyAddress(Aud_DTV[atpindex]));
    }

    /* Connect to PPAO */
    AddAndConnectPPAOToFlow(pFlow, adecIndex, pPPAo, pCurFlowStatus);

#if !defined(CONFIG_ARCH_RTK2819A)
    if (adecIndex == Aud_mainDecIndex)
    {
        pPPAo->SwitchFocus(pPPAo, NULL);//mac9q move to RAL_AUDIO_SetMainDecoderOutput
        //wait AO finish swithing focus
        msleep(20);
    }
#endif
    // update adec info status
    Aud_Adec_info[adecIndex].prevAdecFormat = Aud_Adec_info[adecIndex].curAdecFormat;
    Aud_Adec_info[adecIndex].curAdecFormat  = audioType;
    Aud_Adec_info[adecIndex].bAdecStart     = TRUE;

    /* update resource status */
    ResourceStatus[curDecResourceId].connectStatus[0] = HAL_AUDIO_RESOURCE_RUN;

    pFlow->SetRate(pFlow, GetDecSpeed(adecIndex));

    pFlow->Flush(pFlow);
    pFlow->Run(pFlow);

    if(IsDTVSource(decIptResId))
    {
        SINT32 newfmt[9] = {0};
        Base* pDtv = Aud_DTV[decIptResId - HAL_AUDIO_RESOURCE_ATP0];
        DEMUX_CHANNEL_T demux_ch = (DEMUX_CHANNEL_T)((DtvCom*)pDtv->pDerivedObj)->GetSDECChannel(pDtv);
        RSDEC_ENUM_TYPE_DEST_T sdec_dest = (RSDEC_ENUM_TYPE_DEST_T)((DtvCom*)pDtv->pDerivedObj)->GetSDECDest(pDtv);
        // AUDIO_DEC_NEW_FORMAT:: audio type + private info[8]
        newfmt[0] = Convert2AudioDecType(audioType);
        if(newfmt[0] == AUDIO_UNKNOWN_TYPE)
            return NOT_OK;

        //newfmt[1] = (newfmt[0] == AUDIO_AAC_DECODER_TYPE)? 1 : 0; // aac latm
        newfmt[7] = 1 << 1; // DTV mode
        KADP_SDEC_PrivateInfo(demux_ch, sdec_dest, DEMUX_PRIVATEINFO_AUDIO_FORMAT_, (ULONG*)newfmt, sizeof(newfmt));
    }

    if (adecIndex == Aud_mainDecIndex) {
        INFO("after Start Decoding flow DEC%d (main) info::\n", Aud_mainDecIndex);
        Aud_flow[Aud_mainDecIndex]->ShowCurrentExitModule(Aud_flow[Aud_mainDecIndex], 0);
    } else {
        INFO("after Start Decoding flow DEC%d (sub) info::\n", adecIndex);
        Aud_flow[adecIndex]->ShowCurrentExitModule(Aud_flow[adecIndex], 0);
    }

    return OK;

error:
    ERROR("%s fail errnum=%d\n", __func__, errnum);
    return NOT_OK;
}

DTV_STATUS_T RHAL_AUDIO_StopDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    DTV_STATUS_T status;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    status = RAL_AUDIO_StopDecoding(adecIndex);
    SetDecUserState(adecIndex, 0);
    return status;
}

DTV_STATUS_T RHAL_AUDIO_StartDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T audioType)
{
    DTV_STATUS_T status;

    INFO("%s ADEC%d type %d\n", __func__, adecIndex, audioType);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    status =  RAL_AUDIO_StartDecoding(adecIndex, audioType, 0);
    SetDecUserState(adecIndex, TRUE);
    return status ;
}

DTV_STATUS_T RAL_AUDIO_SetMainDecoderOutput(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
#if defined(CONFIG_ARCH_RTK2819A)
    Base* pPPAo;
#else
    HAL_AUDIO_RESOURCE_T id;
    SINT32 isNeedAutoStart = 0;
    SINT32 isNeedSubAutoStart = 0;
#endif
    INFO("%s to ADEC%d\n", __func__, adecIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

#if defined(CONFIG_ARCH_RTK2819A)
    if (!IsValidAOutInstance(adecIndex)) {
        ERROR("%s Aud_Aout[%d] NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    pPPAo = Aud_Aout[adecIndex];
    pPPAo->SwitchFocus(pPPAo, NULL);
#else
    if((SINT32)adecIndex != Aud_mainDecIndex)
    {
        HAL_AUDIO_RESOURCE_T id;
        // change main flow

        // stop main
        if((Aud_mainDecIndex != -1))
        {
            HAL_AUDIO_RESOURCE_T id = adec2res((HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex);
            // The dec might be started by other process, os check here. We'll do auto re-start if it is own by this process
            if(IsResourceRunningByProcess((HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex,ResourceStatus[id]))
            {
                if(ResourceStatus[id].connectStatus[0] == HAL_AUDIO_RESOURCE_RUN)
                    isNeedSubAutoStart = 1; // no sub dec type , so need to check sub decoder is at run state
                RAL_AUDIO_StopDecoding((HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex);
            }
        }

        // stop current sub
        id = adec2res(adecIndex);

        if(ResourceStatus[id].connectStatus[0] == HAL_AUDIO_RESOURCE_RUN)
        {
            isNeedAutoStart = 1;
            RAL_AUDIO_StopDecoding((HAL_AUDIO_ADEC_INDEX_T)adecIndex);
        }
    }

    id = adec2res(adecIndex);
#endif
    Aud_prevMainDecIndex = Aud_mainDecIndex;
    Aud_mainDecIndex     = adecIndex;

#if 1 /* Inform ADSP about the dec port for SPDIF output */
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SPDIF_OUTPUT_SWITCH;
    audioConfig.value[0] = Aud_mainDecIndex;
    if(SendRPC_AudioConfig(&audioConfig) != S_OK) {
        ERROR("[%s,%d] SPDIF out switch failed\n", __func__, __LINE__);
        return NOT_OK;
    }
#endif

#if !defined(CONFIG_ARCH_RTK2819A)
    SetSPDIFOutType(adecIndex, HAL_AUDIO_SPDIF_AUTO);

    // need to add auto deceoding
    if(isNeedAutoStart == 1)
    {
        if(AUDIO_HAL_CHECK_PLAY_NOTAVAILABLE(ResourceStatus[id], 0))
        {
            ERROR("dec failed status != stop %d\n", ResourceStatus[id].connectStatus[0]);
            return  NOT_OK;
        }
        RAL_AUDIO_StartDecoding((HAL_AUDIO_ADEC_INDEX_T)adecIndex, Aud_Adec_info[adecIndex].curAdecFormat, 0);
    }
    // need to add auto deceoding
    if(isNeedSubAutoStart == 1)
    {
        RAL_AUDIO_StartDecoding((HAL_AUDIO_ADEC_INDEX_T)Aud_prevMainDecIndex, Aud_Adec_info[Aud_prevMainDecIndex].curAdecFormat, 0);
    }
#endif
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetMainDecoderOutput(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    INFO("%s Enter.\n", __func__);
    m_IsSetMainDecOptByMW = TRUE;

    return RAL_AUDIO_SetMainDecoderOutput(adecIndex);
}

DTV_STATUS_T RHAL_AUDIO_SetMainAudioSndoutDevice(HAL_AUDIO_SNDOUT_T sndoutdevice)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND));

	INFO("%s Enter. sndoutdevice:%d\n", __func__, sndoutdevice);

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_MAINAUDIO_DEVICE;
    audioConfig.value[0] = sndoutdevice;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetMultiViewMainAudioOutput(HAL_AUDIO_MIX_INDEX_T audiomixIndex)
{
    HAL_AUDIO_ADEC_INDEX_T adecIndex = HAL_AUDIO_ADEC0;
    UINT32 valid_mix_mask = AUDIO_MIX_ALL;  // Valid bits mask
    UINT32 valid_dec_mask = 0xFF;
    INFO("%s Enter.\n", __func__);

    m_IsSetMainDecOptByMW = TRUE;
    if ((audiomixIndex & ~valid_mix_mask) != 0) {
        ERROR("Error: audioIndex %u is not valid.\n", audiomixIndex);
        return NOT_OK;
    }

    if (!(audiomixIndex & valid_dec_mask))
    {
        AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
        INFO("%s to audioIndex(%d), force SPDIF/ARC output PCM!!!\n", __func__,audiomixIndex);
        #if 1 /* Inform ADSP about the dec port for SPDIF/ARC output */
        audioConfig.msg_id = AUDIO_CONFIG_CMD_SPDIF_OUTPUT_SWITCH;
        audioConfig.value[0] = audiomixIndex;
        if(SendRPC_AudioConfig(&audioConfig) != S_OK) {
            ERROR("[%s,%d] SPDIF out switch failed\n", __func__, __LINE__);
            return NOT_OK;
        }
        #endif
        return OK;
    }

    adecIndex = (audiomixIndex == AUDIO_MIX_ADEC0) ? HAL_AUDIO_ADEC0 : HAL_AUDIO_ADEC1;

    return RAL_AUDIO_SetMainDecoderOutput(adecIndex);
}

DTV_STATUS_T RHAL_AUDIO_SetMainAudioOutput(HAL_AUDIO_INDEX_T audioIndex)
{
    HAL_AUDIO_ADEC_INDEX_T adecIndex = HAL_AUDIO_ADEC0;
    INFO("%s Enter.\n", __func__);

    m_IsSetMainDecOptByMW = TRUE;

    if (audioIndex > HAL_AUDIO_INDEX_MAX)
    {
        ERROR("Audio Index out of range!!!\n");
        return NOT_OK;
    }

    if((audioIndex != HAL_AUDIO_INDEX0) && (audioIndex != HAL_AUDIO_INDEX1))
    {
        AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
        INFO("%s to audioIndex(%d), force SPDIF/ARC output PCM!!!\n", __func__,audioIndex);
        #if 1 /* Inform ADSP about the dec port for SPDIF/ARC output */
        audioConfig.msg_id = AUDIO_CONFIG_CMD_SPDIF_OUTPUT_SWITCH;
        audioConfig.value[0] = audioIndex;
        if(SendRPC_AudioConfig(&audioConfig) != S_OK) {
            ERROR("[%s,%d] SPDIF out switch failed\n", __func__, __LINE__);
            return NOT_OK;
        }
        #endif
        return OK;
    }

    adecIndex = (audioIndex == HAL_AUDIO_INDEX0) ? HAL_AUDIO_ADEC0 : HAL_AUDIO_ADEC1;

    return RAL_AUDIO_SetMainDecoderOutput(adecIndex);
}


DTV_STATUS_T RHAL_AUDIO_PauseAdec(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    HAL_AUDIO_RESOURCE_T curResourceId;

    INFO("%s ADEC%d\n", __func__, adecIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    ShowFlow(adec2res(adecIndex), HAL_AUDIO_RESOURCE_PAUSE, 0);

    curResourceId = adec2res(adecIndex);
    if(AUDIO_HAL_CHECK_ISATPAUSESTATE(ResourceStatus[curResourceId], 0))
    {
        return OK;
    }

    if(!AUDIO_HAL_CHECK_ISATRUNSTATE(ResourceStatus[curResourceId], 0))
    {
        ERROR("%s status is at %s\n", GetResourceString(curResourceId),
                GetResourceStatusString(ResourceStatus[curResourceId].connectStatus[0]));
        return NOT_OK;
    }

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }
    Aud_flow[adecIndex]->Pause(Aud_flow[adecIndex]);
    ResourceStatus[curResourceId].connectStatus[0] = HAL_AUDIO_RESOURCE_PAUSE;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ResumeAdec(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    HAL_AUDIO_RESOURCE_T curResourceId;
    INFO("%s ADEC%d\n", __func__, adecIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    curResourceId = adec2res(adecIndex);

    if(!AUDIO_HAL_CHECK_ISATPAUSESTATE(ResourceStatus[curResourceId], 0))
    {
        ERROR("%s status is at %s", GetResourceString(curResourceId),
                GetResourceStatusString(ResourceStatus[curResourceId].connectStatus[0]));
        return NOT_OK;
    }

    ShowFlow(adec2res(adecIndex), HAL_AUDIO_RESOURCE_RUN, 0);

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    Aud_flow[adecIndex]->Run(Aud_flow[adecIndex]);
    ResourceStatus[adec2res(adecIndex)].connectStatus[0] = HAL_AUDIO_RESOURCE_RUN;

    return OK;
}

/// for system request ,only stop flow directly  no need to delete it.
// pair of HAL_AUDIO_ResumeAndRunAdec
// pvr use
DTV_STATUS_T RHAL_AUDIO_PauseAndStopFlowAdec(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    HAL_AUDIO_RESOURCE_T curResourceId;
    INFO("%s adec index %d\n", __func__,adecIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    curResourceId = adec2res(adecIndex);

    ShowFlow(adec2res(adecIndex), HAL_AUDIO_RESOURCE_PAUSE, 0);

    if(AUDIO_HAL_CHECK_ISATPAUSEANDSTOPSTATE(ResourceStatus[curResourceId], 0))
    {
        return OK;
    }

    if(AUDIO_HAL_CHECK_PAUSE_AND_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0))
    {
        ERROR("%s status is at %s", GetResourceString(curResourceId),
                GetResourceStatusString(ResourceStatus[curResourceId].connectStatus[0]));
        return NOT_OK;
    }

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    // only stop and no delete flow
    Aud_flow[adecIndex]->Stop(Aud_flow[adecIndex]);
    ResourceStatus[curResourceId].connectStatus[0] = HAL_AUDIO_RESOURCE_PAUSE_AND_STOP;

    return OK;
}

// pair HAL_AUDIO_PauseAndStopFlowAdec
// pvr use
DTV_STATUS_T RHAL_AUDIO_ResumeAndRunAdec(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    HAL_AUDIO_SRC_TYPE_T audioType = HAL_AUDIO_SRC_TYPE_UNKNOWN;
    HAL_AUDIO_RESOURCE_T curDecResourceId, decIptResId;
    HAL_AUDIO_RESOURCE_T curResourceId;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    INFO("%s ADEC%d type %s\n", __func__, adecIndex, GetSRCTypeName(audioType));
    curResourceId = adec2res(adecIndex);

    if(!AUDIO_HAL_CHECK_ISATPAUSEANDSTOPSTATE(ResourceStatus[curResourceId], 0))
    {
        ERROR("%s status is at %s", GetResourceString(curResourceId),
                GetResourceStatusString(ResourceStatus[curResourceId].connectStatus[0]));
        return NOT_OK;
    }

    ShowFlow(adec2res(adecIndex), HAL_AUDIO_RESOURCE_RUN, 0);

    audioType = Aud_Adec_info[adecIndex].curAdecFormat;

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    Aud_flow[adecIndex]->Flush(Aud_flow[adecIndex]);
    Aud_flow[adecIndex]->Run(Aud_flow[adecIndex]);

    curDecResourceId = adec2res(adecIndex);
    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curDecResourceId]);// dec input source
    if(IsDTVSource(decIptResId))
    {
        SINT32 newfmt[9] = {0};
        Base* pDtv = Aud_DTV[decIptResId - HAL_AUDIO_RESOURCE_ATP0];
        DEMUX_CHANNEL_T demux_ch = (DEMUX_CHANNEL_T)((DtvCom*)pDtv->pDerivedObj)->GetSDECChannel(pDtv);
        RSDEC_ENUM_TYPE_DEST_T sdec_dest = (RSDEC_ENUM_TYPE_DEST_T)((DtvCom*)pDtv->pDerivedObj)->GetSDECDest(pDtv);
        // AUDIO_DEC_NEW_FORMAT:: audio type + private info[8]
        newfmt[0] = Convert2AudioDecType(audioType);
        if(newfmt[0] == AUDIO_UNKNOWN_TYPE)
            return NOT_OK;

        newfmt[1] = (newfmt[0] == AUDIO_AAC_DECODER_TYPE)? 1 : 0; // aac latm
        newfmt[7] = 1 << 1; // DTV mode
        KADP_SDEC_PrivateInfo(demux_ch, sdec_dest, DEMUX_PRIVATEINFO_AUDIO_FORMAT_, (ULONG*)newfmt, sizeof(newfmt));
    }
    ResourceStatus[adec2res(adecIndex)].connectStatus[0] = HAL_AUDIO_RESOURCE_RUN;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_FlushAdec(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    FlowManager* pFlow;
    INFO("%s adec index %d\n", __func__,adecIndex);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    pFlow = Aud_flow[adecIndex];
    pFlow->Flush(pFlow);

    return OK;
}

HAL_AUDIO_HDMI_TYPE_T ConvertSRCType2HDMIMode(HAL_AUDIO_SRC_TYPE_T src_type)
{
    HAL_AUDIO_HDMI_TYPE_T HDMIMode;
    switch(src_type)
    {
    case HAL_AUDIO_SRC_TYPE_PCM:
        HDMIMode = HAL_AUDIO_HDMI_PCM;
        break;
    case HAL_AUDIO_SRC_TYPE_AC3:
        HDMIMode = HAL_AUDIO_HDMI_AC3;
        break;
    case HAL_AUDIO_SRC_TYPE_MPEG:
        HDMIMode = HAL_AUDIO_HDMI_MPEG;
        break;
    case HAL_AUDIO_SRC_TYPE_AAC:
        HDMIMode = HAL_AUDIO_HDMI_AAC;
        break;
    case HAL_AUDIO_SRC_TYPE_DTS:
        HDMIMode = HAL_AUDIO_HDMI_DTS;
        break;
    case HAL_AUDIO_SRC_TYPE_DTS_HD_MA:
        HDMIMode = HAL_AUDIO_HDMI_DTS_HD_MA;
        break;
    case HAL_AUDIO_SRC_TYPE_DTS_EXPRESS:
        HDMIMode = HAL_AUDIO_HDMI_DTS_EXPRESS;
        break;
    case HAL_AUDIO_SRC_TYPE_DTS_CD:
        HDMIMode = HAL_AUDIO_HDMI_DTS_CD;
        break;
    case HAL_AUDIO_SRC_TYPE_EAC3:
        HDMIMode = HAL_AUDIO_HDMI_EAC3;
        break;
    default:
        HDMIMode = HAL_AUDIO_HDMI_DEFAULT;
        break;
    }
    return HDMIMode;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_SetFullCapPort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex)
{

    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND));
    INFO("%s set %d port\n", __func__, hdmiIndex);

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_HDMI_FULLCAP_PORT;
    audioConfig.value[0] = hdmiIndex;
    AinStatus.hdmifullcapport = hdmiIndex;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RAL_AUDIO_HDMI_GetAudioMode(HAL_AUDIO_HDMI_TYPE_T *pHDMIMode)
{
    HAL_AUDIO_SRC_TYPE_T code_type;
    HAL_AUDIO_RESOURCE_T upstreamResourceId0, upstreamResourceId1;
    HAL_AUDIO_ADEC_INDEX_T adecIndex = HAL_AUDIO_ADEC_MAX;
    DTV_STATUS_T Ret = NOT_OK;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(pHDMIMode == NULL)
        return NOT_OK;

    /* finding the decoder which is used by checking the upstream source is HDMI or not. */
    upstreamResourceId0 = GetNonOutputModuleSingleInputResource(ResourceStatus[HAL_AUDIO_RESOURCE_ADEC0]);
    upstreamResourceId1 = GetNonOutputModuleSingleInputResource(ResourceStatus[HAL_AUDIO_RESOURCE_ADEC1]);

    if (IsHDMISource(upstreamResourceId0) && IsHDMISource(upstreamResourceId1)) {
        ERROR("%s:%d Two decoders are both connected to HDMI.\n", __func__,__LINE__);
        return NOT_PERMITTED;
    } else if (IsDPSource(upstreamResourceId0) && IsDPSource(upstreamResourceId1)) {
        ERROR("%s:%d Two decoders are both connected to DP.\n", __func__,__LINE__);
        return NOT_PERMITTED;
    } else if(IsHDMISource(upstreamResourceId0) || IsDPSource(upstreamResourceId0)) {
        adecIndex = HAL_AUDIO_ADEC0;
    } else if(IsHDMISource(upstreamResourceId1) || IsDPSource(upstreamResourceId1)) {
        adecIndex = HAL_AUDIO_ADEC1;
    } else {
        *pHDMIMode = HAL_AUDIO_HDMI_NO_AUDIO;
        return OK;
    }

    if (AUDIO_HAL_CHECK_ISATSTOPSTATE(ResourceStatus[adec2res(adecIndex)], 0)) {
        *pHDMIMode = HAL_AUDIO_HDMI_NO_AUDIO;
        return OK;
    } else {
        BOOLEAN isESExist = FALSE;
        FlowManager *pFlow = NULL;
        UINT32 b_AO_Underflow = 0;

        if (!IsValidADECInstance(adecIndex)) {
            ERROR("%s Aud_dec[%d] NULL\n", __func__, adecIndex);
            return NOT_OK;
        }

        if (Aud_flow[adecIndex] == NULL) {
            ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
            return NOT_OK;
        }
        pFlow = Aud_flow[adecIndex];

        if(pFlow->extRefClock_phy) {
            /* For DTV source, use external reference clock */
            REFCLOCK *dtv_Refclock = NULL;
            dtv_Refclock = (REFCLOCK *)phys_to_virt(pFlow->extRefClock_phy);
            IPC_WriteU32((BYTE*)&b_AO_Underflow, dtv_Refclock->AO_Underflow);

        } else {
            /* use internal reference clock */
            IPC_WriteU32((BYTE*)&b_AO_Underflow, pFlow->pRefClock->m_core->AO_Underflow);
        }
        isESExist = !(b_AO_Underflow);

        if (isESExist == 0) {
            *pHDMIMode = HAL_AUDIO_HDMI_NO_AUDIO;
            return OK;
        }

        Ret = RAL_AUDIO_GetDecodingType(adecIndex, &code_type);

        if(Ret == OK) {
            if (code_type == HAL_AUDIO_SRC_TYPE_UNKNOWN) {
                *pHDMIMode = HAL_AUDIO_HDMI_NO_AUDIO;
                return OK;
            } else {
                *pHDMIMode = ConvertSRCType2HDMIMode(code_type);
                return OK;
            }
        } else {
            ERROR("%s Fail to get the decoding type!!!\n", __func__);
            *pHDMIMode = HAL_AUDIO_HDMI_NO_AUDIO;
            return NOT_OK;
        }
    }
}

DTV_STATUS_T RHAL_AUDIO_HDMI_GetAudioMode(HAL_AUDIO_HDMI_TYPE_T *pHDMIMode)
{
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    return RAL_AUDIO_HDMI_GetAudioMode(pHDMIMode);
}

DTV_STATUS_T RHAL_AUDIO_HDMI_SetAudioReturnChannel(BOOLEAN bOnOff)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    INFO("%s bOnOff=%d\n", __func__, bOnOff);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    if(bOnOff) {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id    = AUDIO_CONFIG_CMD_HDMI_ARC;
        audioConfig.value[0] = TRUE;
        audioConfig.value[1] = 1<<AUDIO_HDMI_CODING_TYPE_DDP;
        audioConfig.value[2] = FALSE;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] hdmi arc on failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    } else {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id    = AUDIO_CONFIG_CMD_HDMI_ARC;
        audioConfig.value[0] = FALSE;
        audioConfig.value[1] = FALSE;
        audioConfig.value[2] = FALSE;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] hdmi arc off failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    _ARC_Enable = bOnOff;
    ARC_offon = bOnOff;

    Update_RawMode_by_connection();
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_SetEnhancedAudioReturnChannel(BOOLEAN bOnOff)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    INFO("%s bOnOff=%d\n", __func__, bOnOff);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    if(bOnOff) {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id    = AUDIO_CONFIG_CMD_HDMI_EARC;
        audioConfig.value[0] = TRUE;
        audioConfig.value[1] = 1<<AUDIO_HDMI_CODING_TYPE_DDP;
        audioConfig.value[2] = FALSE;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] hdmi earc on failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    } else {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id    = AUDIO_CONFIG_CMD_HDMI_EARC;
        audioConfig.value[0] = FALSE;
        audioConfig.value[1] = FALSE;
        audioConfig.value[2] = FALSE;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] hdmi earc off failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    _EARC_Enable = bOnOff;
    eARC_offon = bOnOff;

    Update_RawMode_by_connection();
    return OK;
}

DTV_STATUS_T RAL_AUDIO_HDMI_GetCopyInfo(HAL_AUDIO_SPDIF_COPYRIGHT_T *pCopyInfo)
{
	UINT8 copyright = 0;
    UINT8 category_code = 0;
    KADP_AO_SPDIF_CHANNEL_STATUS_BASIC channel_status;

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;
    if(pCopyInfo == NULL)
    {
        ERROR("address error\n");
        return NOT_OK;
    }

    memset(&channel_status, 0, sizeof(KADP_AO_SPDIF_CHANNEL_STATUS_BASIC));
    if (KADP_AUDIO_GetAudioSpdifChannelStatus(&channel_status, AUDIO_IN))
    {
        ERROR("[%s,%d] get spdif channel status failed\n", __func__, __LINE__);
        return NOT_OK;
    }
    copyright = channel_status.copyright;
    category_code = channel_status.category_code;

    if(copyright == HAL_SPDIF_COPYRIGHT_FREE)
    {
        *pCopyInfo  = HAL_AUDIO_SPDIF_COPY_FREE; /* cp-bit : 1, L-bit : 0 */
    }
    else if( (category_code & HAL_SPDIF_CATEGORY_L_BIT_IS_1) == HAL_SPDIF_CATEGORY_L_BIT_IS_1)
    {
        *pCopyInfo = HAL_AUDIO_SPDIF_COPY_NEVER;
    }
    else
    {
        *pCopyInfo = HAL_AUDIO_SPDIF_COPY_ONCE; /* cp-bit : 0, L-bit : 0 */
    }

    *pCopyInfo = HAL_AUDIO_SPDIF_COPY_NEVER;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_HDMI_GetCopyInfo(HAL_AUDIO_SPDIF_COPYRIGHT_T *pCopyInfo)
{
    AUDIO_VERBOSE("%s %p ", __func__, pCopyInfo);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;
    if(pCopyInfo == NULL)
    {
        ERROR("address error == NULL\n");
        return NOT_OK;
    }
    return RAL_AUDIO_HDMI_GetCopyInfo(pCopyInfo);
}

DTV_STATUS_T RHAL_AUDIO_SetDolbyDRCMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_DOLBY_DRC_MODE_T drcMode)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    INFO("%s ADEC%d mode %d\n", __func__, adecIndex, drcMode);

    if(HAL_AUDIO_DOLBY_LINE_MODE == drcMode)
    {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_COMP;
        audioConfig.value[0] = COMP_LINEOUT;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set compressmode failed\n", __func__, __LINE__);
            return NOT_OK;
        }

        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_SCALE;
        audioConfig.value[0] = COMP_SCALE_FULL;
        audioConfig.value[1] = COMP_SCALE_FULL;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set scalehilo failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    else if(HAL_AUDIO_DOLBY_RF_MODE == drcMode)
    {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_COMP;
        audioConfig.value[0] = COMP_RF;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set compressmode failed\n", __func__, __LINE__);
            return NOT_OK;
        }

        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_SCALE;
        audioConfig.value[0] = COMP_SCALE_FULL;
        audioConfig.value[1] = COMP_SCALE_FULL;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set scalehilo failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    else if(HAL_AUDIO_DOLBY_DRC_OFF == drcMode)
    {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_COMP;
        audioConfig.value[0] = COMP_LINEOUT;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set compressmode failed\n", __func__, __LINE__);
            return NOT_OK;
        }

        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_SCALE;
        audioConfig.value[0] = COMP_SCALE_NONE;
        audioConfig.value[1] = COMP_SCALE_NONE;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set scalehilo failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    else
    {
        ERROR("unknow drc type %d\n", drcMode);
        return NOT_OK;
    }

    SetDecDrcMode((SINT32)adecIndex, drcMode);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDownMixMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_DOWNMIX_MODE_T downmixMode)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;

    INFO("%s ADEC%d mode %d\n", __func__, adecIndex, downmixMode);

    if(HAL_AUDIO_LORO_MODE == downmixMode)
    {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_DOWNMIXMODE;
        audioConfig.value[0] = MODE_STEREO;
        audioConfig.value[1] = LFE_OFF;
        audioConfig.value[2] = 0x00002379;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set dd donwmix failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    else if(HAL_AUDIO_LTRT_MODE == downmixMode)
    {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_DOWNMIXMODE;
        audioConfig.value[0] = MODE_DOLBY_SURROUND;
        audioConfig.value[1] = LFE_OFF;
        audioConfig.value[2] = 0x00002379;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set dd donwmix failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    else if(HAL_AUDIO_ARIB_MODE == downmixMode)
    {
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DD_DOWNMIXMODE;
        audioConfig.value[0] = MODE_ARIB;
        audioConfig.value[1] = LFE_OFF;
        audioConfig.value[2] = 0x00002379;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set dd donwmix failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    else
    {
        ERROR("unknow downmix type %d\n", downmixMode);
        return NOT_OK;
    }

    SetDecDownMixMode((SINT32)adecIndex, downmixMode);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetDecodingType(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T *pAudioType)
{
    return RAL_AUDIO_GetDecodingType(adecIndex, pAudioType);
}

static SINT32 ConvetChannelStatusSampleRate(UINT32 sampleRateIndex)
{
    SINT32 SR_table[] = {44100, 0, 48000, 32000, 22050, 0,
                       24000, 0, 88200, 0, 96000, 0, 176400, 0, 192000 };

    //fix coverity:272, Event overrun-local: Overrunning array
    //if(sampleRateIndex > (sizeof(SR_table)/ sizeof(SR_table[0])))
    if(sampleRateIndex >= (sizeof(SR_table)/ sizeof(SR_table[0])))
    {
        ERROR("Error sample rate index %d\n", sampleRateIndex);
        return 0;
    }

    return (SR_table[sampleRateIndex]);
}

BOOLEAN RAL_AUDIO_TP_IsESExist(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    HAL_AUDIO_RESOURCE_T decIptResId;
    HAL_AUDIO_RESOURCE_T curResourceId = adec2res(adecIndex);
    BOOLEAN isESExist = FALSE;
    FlowManager *pFlow = NULL;
    UINT32 b_AO_Underflow = 0;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if (!IsValidADECInstance(adecIndex)) {
        ERROR("%s Aud_dec[%d] NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0)) // at run state,  decoder can only have one input
    {
        ERROR("%s is not at run state %d\n",
                GetResourceString(curResourceId), ResourceStatus[curResourceId].connectStatus[0]); // decrease regular print
        return FALSE;
    }

    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curResourceId]);// dec input source
    if(IsDTVSource(decIptResId) != TRUE)
    {
        ERROR("%s is not at DTV mode (input = %s)\n",
                GetResourceString(curResourceId), GetResourceString(decIptResId)); // decrease regular print
        return FALSE;
    } else {
        if (Aud_flow[adecIndex] == NULL) {
            ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
            return NOT_OK;
        }
        pFlow = Aud_flow[adecIndex];

        if(pFlow->extRefClock_phy) {
            /* For DTV source, use external reference clock */
            REFCLOCK *dtv_Refclock = NULL;
            dtv_Refclock = (REFCLOCK *)phys_to_virt(pFlow->extRefClock_phy);
            IPC_WriteU32((BYTE*)&b_AO_Underflow, dtv_Refclock->AO_Underflow);
            isESExist = !(b_AO_Underflow);
        } else {
            ERROR("Can not find reference clock at DTV mode)\n");
            return FALSE;
        }
    }

    INFO("%s from ADEC%d = %s\n", __func__, adecIndex, isESExist? "TRUE":"FALSE");// decrease regular print
    return isESExist;
}

DTV_STATUS_T RHAL_AUDIO_GetAdecStatus(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_ADEC_INFO_T *pAudioAdecInfo)
{
    HAL_AUDIO_RESOURCE_T curResourceId;
    HAL_AUDIO_RESOURCE_T decIptResId;

    INFO("%s ADEC%d %p\n", __func__, adecIndex, pAudioAdecInfo);//decrease regular print

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if(pAudioAdecInfo == NULL)
        return NOT_OK;

    curResourceId = adec2res(adecIndex);
    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curResourceId]);// dec input source

    if(IsDTVSource(decIptResId) == TRUE)
        Aud_Adec_info[adecIndex].bAdecESExist = RAL_AUDIO_TP_IsESExist(adecIndex);
    else
        Aud_Adec_info[adecIndex].bAdecESExist = FALSE;

    if(IsHDMISource(decIptResId) || IsDPSource(decIptResId))
    {
        KADP_AO_SPDIF_CHANNEL_STATUS_BASIC channel_status;
        HAL_AUDIO_HDMI_TYPE_T audiomode;
        RAL_AUDIO_HDMI_GetAudioMode(&audiomode);

        memset(&channel_status, 0, sizeof(KADP_AO_SPDIF_CHANNEL_STATUS_BASIC));
        if (KADP_AUDIO_GetAudioSpdifChannelStatus(&channel_status, AUDIO_IN))
        {
            ERROR("[%s,%d] get spdif channel status failed\n", __func__, __LINE__);
            return NOT_OK;
        }

        Aud_Adec_info[adecIndex].curHdmiAudioType = audiomode;
        Aud_Adec_info[adecIndex].curHdmiSamplingFreq = (HAL_AUDIO_SAMPLING_FREQ_T)ConvetChannelStatusSampleRate(channel_status.sampling_freq);
        RAL_AUDIO_HDMI_GetCopyInfo(&(Aud_Adec_info[adecIndex].curHdmiCopyInfo));
    }
    else
    {
        Aud_Adec_info[adecIndex].curHdmiAudioType = HAL_AUDIO_HDMI_DEFAULT;
        Aud_Adec_info[adecIndex].curHdmiSamplingFreq = HAL_AUDIO_SAMPLING_FREQ_NONE;
        Aud_Adec_info[adecIndex].curHdmiCopyInfo = HAL_AUDIO_SPDIF_COPY_FREE;
    }

    if(IsATVSource(decIptResId))
    {
        Aud_Adec_info[adecIndex].sifSource      = g_AudioSIFInfo.sifSource;
        Aud_Adec_info[adecIndex].curSifType     = g_AudioSIFInfo.curSifType;
        Aud_Adec_info[adecIndex].bHighDevOnOff  = g_AudioSIFInfo.bHighDevOnOff;
        Aud_Adec_info[adecIndex].curSifBand     = g_AudioSIFInfo.curSifBand;
        Aud_Adec_info[adecIndex].curSifStandard = g_AudioSIFInfo.curSifStandard;
        Aud_Adec_info[adecIndex].curSifIsA2     = g_AudioSIFInfo.curSifIsA2;
        Aud_Adec_info[adecIndex].curSifIsNicam  = g_AudioSIFInfo.curSifIsNicam;
        Aud_Adec_info[adecIndex].curSifModeSet  = g_AudioSIFInfo.curSifModeSet;
        Aud_Adec_info[adecIndex].curSifModeGet  = g_AudioSIFInfo.curSifModeGet;
    }
    else
    {
        Aud_Adec_info[adecIndex].sifSource      = HAL_AUDIO_SIF_INPUT_INTERNAL;
        Aud_Adec_info[adecIndex].curSifType     = HAL_AUDIO_SIF_TYPE_NONE;
        Aud_Adec_info[adecIndex].bHighDevOnOff  = FALSE;
        Aud_Adec_info[adecIndex].curSifBand     = HAL_AUDIO_SIF_SYSTEM_UNKNOWN;
        Aud_Adec_info[adecIndex].curSifStandard = HAL_AUDIO_SIF_MODE_DETECT;
        Aud_Adec_info[adecIndex].curSifIsA2     = HAL_AUDIO_SIF_DETECTING_EXSISTANCE;
        Aud_Adec_info[adecIndex].curSifIsNicam  = HAL_AUDIO_SIF_DETECTING_EXSISTANCE;
        Aud_Adec_info[adecIndex].curSifModeSet  = HAL_AUDIO_SIF_SET_PAL_UNKNOWN;
        Aud_Adec_info[adecIndex].curSifModeGet  = HAL_AUDIO_SIF_GET_PAL_UNKNOWN;
    }

    memcpy(pAudioAdecInfo, &Aud_Adec_info[adecIndex], sizeof(HAL_AUDIO_ADEC_INFO_T));
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDualMonoOutMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_DUALMONO_MODE_T outputMode)
{
    Base *pDec;
    const CHAR modeName[4][4] = {"LR","LL","RR","MIX"};

    INFO("%s ADEC%d to %s mode\n",__func__, adecIndex, modeName[outputMode]);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if (!IsValidADECInstance(adecIndex)) {
        ERROR("%s Aud_dec[%d] NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    if(outputMode > HAL_AUDIO_DUALMONO_MODE_MIX)
        return NOT_OK;

    pDec = Aud_dec[adecIndex];

    if(outputMode == HAL_AUDIO_DUALMONO_MODE_LR)
        ((DEC*)pDec->pDerivedObj)->SetChannelSwap(pDec, AUDIO_CHANNEL_OUT_SWAP_STEREO);
    else if(outputMode == HAL_AUDIO_DUALMONO_MODE_LL)
        ((DEC*)pDec->pDerivedObj)->SetChannelSwap(pDec, AUDIO_CHANNEL_OUT_SWAP_L_TO_R);
    else if(outputMode == HAL_AUDIO_DUALMONO_MODE_RR)
        ((DEC*)pDec->pDerivedObj)->SetChannelSwap(pDec, AUDIO_CHANNEL_OUT_SWAP_R_TO_L);
    else  //HAL_AUDIO_DUALMONO_MODE_MIX
        ((DEC*)pDec->pDerivedObj)->SetChannelSwap(pDec, AUDIO_CHANNEL_OUT_SWAP_LR_MIXED);

    Aud_Adec_info[adecIndex].curAdecDualmonoMode = outputMode;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_GetESInfo(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_ES_INFO_T *pAudioESInfo)
{
    HAL_AUDIO_RESOURCE_T decIptResId;
    HAL_AUDIO_RESOURCE_T curResourceId = adec2res(adecIndex);
    UINT16 sampleRate;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if(pAudioESInfo == NULL) {
        INFO("%s from %s fail pAudioESInfo null\n", __func__, GetResourceString(curResourceId));
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0))
    {
        INFO("%s is not at run state %d\n",
                GetResourceString(curResourceId), ResourceStatus[curResourceId].connectStatus[0]);
        return NOT_OK;
    }

    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curResourceId]);// dec input source
    if(IsDTVSource(decIptResId) != TRUE)
    {
        INFO("%s is not at DTV mode (input = %s)\n",
                GetResourceString(curResourceId), GetResourceString(decIptResId));
        return NOT_OK;
    }

    RAL_AUDIO_GetESInfo(adecIndex, pAudioESInfo, &sampleRate);

    return OK;
}

BOOLEAN RHAL_AUDIO_TP_IsESExist(HAL_AUDIO_ADEC_INDEX_T adecIndex)
{
    return RAL_AUDIO_TP_IsESExist(adecIndex);
}

DTV_STATUS_T RHAL_AUDIO_TP_GetAudioPTS(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT32 *pPts)
{
    FlowManager *pFlow = NULL;
    UINT64 audioSystemPTS64 = 0;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if (!IsValidADECInstance(adecIndex)) {
        ERROR("%s Aud_dec[%d] NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    if(pPts == NULL)
        return NOT_OK;

    if(Aud_Adec_info[adecIndex].bAdecStart != TRUE)
    {
        ERROR("ADEC%d is not running\n", adecIndex);
        return NOT_OK;
    }

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }
    pFlow = Aud_flow[adecIndex];

    if(pFlow->extRefClock_phy) {
        /* For DTV source, use external reference clock */
        REFCLOCK *dtv_Refclock = NULL;
        dtv_Refclock = (REFCLOCK *)phys_to_virt(pFlow->extRefClock_phy);
        IPC_WriteU64((BYTE*)&audioSystemPTS64, dtv_Refclock->audioSystemPTS);
    } else {
        /* use internal reference clock */
        IPC_WriteU64((BYTE*)&audioSystemPTS64, pFlow->pRefClock->m_core->audioSystemPTS);
    }
    *pPts = (UINT32) audioSystemPTS64;

    INFO("%s ADEC%d PTS: %x\n", __func__, adecIndex, *pPts);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_GetAudioPTS64(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT64 *pPts)
{
    FlowManager *pFlow = NULL;
    UINT64 audioSystemPTS64 = 0;
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if (!IsValidADECInstance(adecIndex)) {
        ERROR("%s Aud_dec[%d] NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    if(pPts == NULL)
        return NOT_OK;

    if(Aud_Adec_info[adecIndex].bAdecStart != TRUE)
    {
        ERROR("ADEC%d is not running\n", adecIndex);
        return NOT_OK;
    }

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }
    pFlow = Aud_flow[adecIndex];

    if(pFlow->extRefClock_phy) {
        /* For DTV source, use external reference clock */
        REFCLOCK *dtv_Refclock = NULL;
        dtv_Refclock = (REFCLOCK *)phys_to_virt(pFlow->extRefClock_phy);
        IPC_WriteU64((BYTE*)&audioSystemPTS64, dtv_Refclock->audioSystemPTS);
    } else {
        /* use internal reference clock */
        IPC_WriteU64((BYTE*)&audioSystemPTS64, pFlow->pRefClock->m_core->audioSystemPTS);
    }
    *pPts = (UINT64) audioSystemPTS64;

    INFO("%s ADEC%d PTS: %lld\n", __func__, adecIndex, *pPts);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_SetAudioDescriptionMain(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff)
{
    SINT32 id;
    INFO("%s ADEC%d bOnOff=%d\n", __func__, adecIndex, bOnOff);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    id = adec2res(adecIndex);

    if(AUDIO_HAL_CHECK_PLAY_NOTAVAILABLE(ResourceStatus[id], 0))  // // decoder can only have one input
    {
        ERROR("dec description failed status (%d)!= stop\n",
                      ResourceStatus[id].connectStatus[0]);
        return NOT_OK;
    }

    RAL_AUDIO_SetAudioDescriptionMode(Aud_mainDecIndex, bOnOff);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetTrickMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_TRICK_MODE_T eTrickMode)
{
    FlowManager *pFlow;
    SINT32 rate = 256;

    INFO("%s Enter. adecIndex %d, Mode eTrickMode %d\n", __func__, adecIndex, eTrickMode);

    if (AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if (!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if (Aud_flow[adecIndex] == NULL) {
        ERROR("ERROR:%s Aud_flow%d NULL\n", __func__, adecIndex);
        return NOT_OK;
    }

    /* 256 = 1x */
    switch(eTrickMode) {
        case HAL_AUDIO_TRICK_SLOW_MOTION_0P25X:
            rate = 64;
            break;
        case HAL_AUDIO_TRICK_SLOW_MOTION_0P50X:
            rate = 128;
            break;
        case HAL_AUDIO_TRICK_SLOW_MOTION_0P75X:
            rate = 192;
            break;
        case HAL_AUDIO_TRICK_FAST_FORWARD_1P25X:
            rate = 320;
            break;
        case HAL_AUDIO_TRICK_FAST_FORWARD_1P50X:
            rate = 384;
            break;
        case HAL_AUDIO_TRICK_FAST_FORWARD_1P75X:
            rate = 448;
            break;
        case HAL_AUDIO_TRICK_FAST_FORWARD_2P00X:
            rate = 512;
            break;
        case HAL_AUDIO_TRICK_NORMAL_PLAY:
        default:
            rate = 256;
        break;
    }

    pFlow = Aud_flow[adecIndex];
    pFlow->SetRate(pFlow, rate);
    SetDecSpeed(adecIndex, rate);

    INFO("%s Exit.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TP_GetBufferStatus(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT32 *pMaxSize, UINT32 *pFreeSize)
{
    HAL_AUDIO_RESOURCE_T curResourceId, decIptResId, atpIptResId;
    ULONG base , rp , wp, size;
    DTV_STATUS_T retValue = NOT_OK;
    INFO("%s Enter.\n", __func__);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    // check is dtv flow
    curResourceId = adec2res(adecIndex);
    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curResourceId]);// dec input source
    if(IsDTVSource(decIptResId))
    {
        atpIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[decIptResId]);// dec input source

        if((atpIptResId == HAL_AUDIO_RESOURCE_SDEC0) || (atpIptResId == HAL_AUDIO_RESOURCE_SDEC1))
        {
            SINT32 atpindex = decIptResId - HAL_AUDIO_RESOURCE_ATP0;

            RINGBUFFER_HEADER* pRingBufferHeader;
            DtvCom* pDtvCom = (DtvCom*)Aud_DTV[atpindex]->pDerivedObj;
            pRingBufferHeader = (RINGBUFFER_HEADER*)(pDtvCom->GetBSRingBufVirAddress(Aud_DTV[atpindex]));

            GetRingBufferInfo(pRingBufferHeader, &base, &rp, &wp, &size);

            if(wp  >= rp )
            {
                if(pFreeSize)
                    *pFreeSize = size - (wp - rp) -4;// for 4 byte aligm
            }
            else
            {
                if(pFreeSize)
                    *pFreeSize = (rp - wp) - 4 ; // for 4 byte aligm
            }

            if(pMaxSize)
                *pMaxSize = size;

            retValue =  OK;
        }
    }
    return retValue;
}

/* Volume, Mute & Delay */
DTV_STATUS_T RHAL_AUDIO_SetDecoderInputGain(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_VOLUME_T volume)
{
    SINT32 ch_vol[AUD_MAX_CHANNEL];

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;

    INFO("%s ADEC%d = main %d fine %d\n", __func__,
            adecIndex, volume.mainVol, volume.fineVol);

#ifdef AVOID_USELESS_RPC
    if (Volume_Compare(volume, GetDecInVolume(adecIndex)))
    {
        INFO("skip ADEC%d  volume %d ==  GetDecInVolume %d\n",
            adecIndex, volume.mainVol, (((HAL_AUDIO_VOLUME_T)GetDecInVolume(adecIndex)).mainVol));
            return OK;
    }
#endif
    SetDecInVolume(adecIndex, volume);

    ADEC_Calculate_DSPGain(adecIndex, ch_vol);
    if(ADSP_DEC_SetVolume(adecIndex, ch_vol) != KADP_OK)
        return NOT_OK;

    return OK;
}


DTV_STATUS_T RHAL_AUDIO_GetDecoderInputGain(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_VOLUME_T *volume)
{
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(volume == NULL) return NOT_OK;

    *volume = GetDecInVolume(adecIndex);
    INFO("%s ADEC%d = main %d fine %d\n", __func__,
            adecIndex, volume->mainVol, volume->fineVol);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetMixerInputGain(HAL_AUDIO_MIXER_INDEX_T mixerIndex, HAL_AUDIO_VOLUME_T *volume)
{
    if(!RangeCheck(mixerIndex, 0, AUD_AMIX_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(volume == NULL) return NOT_OK;

    *volume = g_mixer_gain[mixerIndex];
    INFO("%s Mixer%d = main %d fine %d\n", __func__,
            mixerIndex, volume->mainVol, volume->fineVol);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetDecoderInputMute(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN *bOnOff)
{
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(bOnOff == NULL) return NOT_OK;

    *bOnOff = (BOOLEAN)GetDecInMute(adecIndex);

    INFO("%s ADEC%d bOnOff=%d\n", __func__, adecIndex, *bOnOff);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetMixerInputMute(HAL_AUDIO_MIXER_INDEX_T mixerIndex, BOOLEAN *bOnOff)
{
    if(!RangeCheck(mixerIndex, 0, AUD_AMIX_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(bOnOff == NULL) return NOT_OK;

    *bOnOff = (BOOLEAN)GetAmixerUserMute(mixerIndex);
    INFO("%s Mixer%d bOnOff=%d\n", __func__, mixerIndex, *bOnOff);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetGlobalDecoderGain(HAL_AUDIO_VOLUME_T volume)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 ch_id;
    INFO("%s main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    ch_id = KADP_AUDIO_CH_ID_ALL;
    audioConfig.msg_id = AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_PB_VOLUME;

    audioConfig.value[0] = ch_id;
    audioConfig.value[1] = Volume_to_DSPGain(volume);

    if (SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        ERROR("[%s,%d] ao dec multi ch pb failed\n", __func__, __LINE__);
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDecoderDelayTime(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT32 delayTime)
{
    INFO("%s ADEC%d delayTime=%d\n", __func__, adecIndex, delayTime);
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;

    if(ADSP_DEC_SetDelay(adecIndex, delayTime) != KADP_OK) return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetMixerInputGain(HAL_AUDIO_MIXER_INDEX_T mixerIndex, HAL_AUDIO_VOLUME_T volume)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
	UINT32 i;
	SINT32 dsp_gain;

    if(!RangeCheck(mixerIndex, 0, AUD_AMIX_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    INFO("%s AMIXER%d = main %d fine %d\n", __func__,
            mixerIndex, volume.mainVol, volume.fineVol);

    dsp_gain = Volume_to_DSPGain(Volume_Add(volume, g_mixer_out_gain));

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_VOLUME;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = ENUM_DEVICE_FLASH_PIN;
    audioConfig.value[2] = (UINT32)mixerIndex;
    audioConfig.value[3] = 0xFF;
    for(i = 0; i < AUD_MAX_CHANNEL; i++)
    {
        audioConfig.value[4+i] = dsp_gain;
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    g_mixer_gain[mixerIndex] = volume;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetPBOutBalance(SINT32 dbGains, UINT32 ch_id)
{
    SINT32 set_dbGains;
    HAL_AUDIO_DSPGAIN_T set_dspGains;
    UINT32 dev_ch = CH_ALL;
    UINT32 dev_id = ENUM_DEVICE_NONE;
    UINT32 param_idx = PARAM_IDX_MAX;

    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(ch_id & APP_CH_ID_CH0) {
        dev_id = ENUM_DEVICE_SPEAKER;
        param_idx = PARAM_IDX_SPKR;
    }
    else if(ch_id & APP_CH_ID_CH2) {
        dev_id = ENUM_DEVICE_HEADPHONE;
        param_idx = PARAM_IDX_HP;
    }

    if(dev_id != ENUM_DEVICE_NONE && param_idx < PARAM_IDX_MAX) {
        if (dbGains >= APP_MAX_BAL) {
            dbGains = APP_MAX_BAL;
        } else if (dbGains <= APP_MIN_BAL) {
            dbGains = APP_MIN_BAL;
        }

       m_pre_bal = m_balance;
        m_balance = dbGains;
        if(m_balance > 0) {
             set_dbGains = m_volume_comp[param_idx] - m_balance;
             dev_ch = CH_L;
        }else if(m_balance < 0) {
             set_dbGains = m_volume_comp[param_idx] + m_balance;
             dev_ch = CH_R;
        }else {
             set_dbGains = m_volume_comp[param_idx];
             dev_ch = CH_ALL;
        }

        if(set_dbGains < HWGAIN_MIN) {
           set_dbGains = HWGAIN_MIN;
        }
        else if(set_dbGains > HWGAIN_MAX) {
           set_dbGains = HWGAIN_MAX;
        }

        set_dspGains = HWGain_to_DSPGain(set_dbGains);
        AUDIO_INFO("[AUDH] %s dev id %x dev ch %x HW dbGains %d,%d,%d\n", __FUNCTION__, dev_id, dev_ch, set_dbGains, set_dspGains.dsp_gain, set_dspGains.dsp_fine);

        if(dev_ch == CH_ALL) {
            if(ADSP_SNDOut_SetFineVolume(dev_id, set_dspGains.dsp_gain, set_dspGains.dsp_fine, FALSE, dev_ch) != KADP_OK) {
                AUDIO_ERROR("[AUDH] %s dev id %x fail\n", __func__, dev_id);
                return NOT_OK;
           }
        } else {
            if(ADSP_SNDOut_SetFineVolume(dev_id, set_dspGains.dsp_gain, set_dspGains.dsp_fine, TRUE, dev_ch) != KADP_OK) {
                AUDIO_ERROR("[AUDH] %s dev id %x fail\n", __func__, dev_id);
                return NOT_OK;
            }
        }

        if(m_pre_bal > 0 && m_balance < 0) { //restore L
             set_dbGains = m_volume_comp[param_idx];
             dev_ch = CH_L;
             if(set_dbGains < HWGAIN_MIN) {
                set_dbGains = HWGAIN_MIN;
             }
             else if(set_dbGains > HWGAIN_MAX) {
                set_dbGains = HWGAIN_MAX;
             }
             set_dspGains = HWGain_to_DSPGain(set_dbGains);
             if(ADSP_SNDOut_SetFineVolume(dev_id, set_dspGains.dsp_gain, set_dspGains.dsp_fine, TRUE, dev_ch) != KADP_OK) {
                 AUDIO_ERROR("[AUDH] %s dev id %x fail\n", __func__, dev_id);
                 return NOT_OK;
             }
        } else if(m_pre_bal < 0 && m_balance > 0) { //restore R
             set_dbGains = m_volume_comp[param_idx];
             dev_ch = CH_R;
             if(set_dbGains < HWGAIN_MIN) {
                set_dbGains = HWGAIN_MIN;
             }
             else if(set_dbGains > HWGAIN_MAX) {
                set_dbGains = HWGAIN_MAX;
             }
             set_dspGains = HWGain_to_DSPGain(set_dbGains);
             if(ADSP_SNDOut_SetFineVolume(dev_id, set_dspGains.dsp_gain, set_dspGains.dsp_fine, TRUE, dev_ch) != KADP_OK) {
                 AUDIO_ERROR("[AUDH] %s dev id %x fail\n", __func__, dev_id);
                 return NOT_OK;
             }
        }
    }

    return OK;
}
EXPORT_SYMBOL(RHAL_AUDIO_SetPBOutBalance);

DTV_STATUS_T RHAL_AUDIO_SetPreScaleVolume(SINT32 vol, UINT32 ch_id)
{
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s HW vol:%d, ch_id:%d\n", __FUNCTION__, vol, ch_id);

    if (ch_id & APP_CH_ID_CH0) {
        if(ADSP_SNDOut_SetPreScaleVolume(ENUM_DEVICE_SPEAKER, vol) != KADP_OK) {
            AUDIO_ERROR("%s Speaker fail\n", __func__);
            return NOT_OK;
        }
    }

    if (ch_id & APP_CH_ID_CH2) {
        if(ADSP_SNDOut_SetPreScaleVolume(ENUM_DEVICE_HEADPHONE, vol) != KADP_OK) {
            AUDIO_ERROR("%s HP fail\n", __func__);
            return NOT_OK;
        }
    }

    if (ch_id & APP_CH_ID_CH4) {
        if(ADSP_SNDOut_SetPreScaleVolume(ENUM_DEVICE_SCART, vol) != KADP_OK) {
            AUDIO_ERROR("%s SCART fail\n", __func__);
            return NOT_OK;
        }
    }

    if (ch_id & APP_CH_ID_CH6) {
        if(ADSP_SNDOut_SetPreScaleVolume(ENUM_DEVICE_PCM_CAPTURE, vol) != KADP_OK) {
            AUDIO_ERROR("%s SCART fail\n", __func__);
            return NOT_OK;
        }
    }

    update_prescale_params(vol, ch_id);

    return OK;
}
EXPORT_SYMBOL(RHAL_AUDIO_SetPreScaleVolume);

DTV_STATUS_T RHAL_AUDIO_GetPreScaleVolume(UINT32 ch_id, SINT32 *a, SINT32 *b)
{
	if (ch_id&APP_CH_ID_CH0 || ch_id&APP_CH_ID_CH1) {
		*a = pre_scale.ch01_scale_a;
		*b = pre_scale.ch01_scale_b;
	}

	if (ch_id&APP_CH_ID_CH2 || ch_id&APP_CH_ID_CH3) {
		*a = pre_scale.ch23_scale_a;
		*b = pre_scale.ch23_scale_b;
	}

	if (ch_id&APP_CH_ID_CH4 || ch_id&APP_CH_ID_CH5) {
		*a = pre_scale.ch45_scale_a;
		*b = pre_scale.ch45_scale_b;
	}

	if (ch_id&APP_CH_ID_CH6 || ch_id&APP_CH_ID_CH7) {
		*a = pre_scale.ch67_scale_a;
		*b = pre_scale.ch67_scale_b;
	}
    INFO("%s ch_id:%d, a:%d, b:%d\n", __FUNCTION__, ch_id, *a, *b);

    return OK;
}
EXPORT_SYMBOL(RHAL_AUDIO_GetPreScaleVolume);

DTV_STATUS_T RHAL_AUDIO_SetPBOutVolume(SINT32 dbGains, UINT32 ch_id)
{
	HAL_AUDIO_DSPGAIN_T gain;
	int i;
	UINT32 kStatus = 0;
	HAL_AUDIO_DSPGAIN_T set_dspGains[2];
	UINT32 dev_ch = CH_ALL;
	SINT32 set_dbGains[2] = {HWGAIN_MIN, HWGAIN_MIN};
	UINT32 dev_id = ENUM_DEVICE_NONE;
	UINT32 param_idx = PARAM_IDX_MAX;
	gain = HWGain_to_DSPGain(dbGains);


	INFO("%s Enter.\n", __func__);
	if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

	INFO("%s HW dbGains %d,%d,%d, ch_id %d\n", __FUNCTION__, dbGains, gain.dsp_gain, gain.dsp_fine, ch_id);

	if (ch_id & APP_CH_ID_CH0) {
		dev_id = ENUM_DEVICE_SPEAKER;
		param_idx = PARAM_IDX_SPKR;
	}
	else if (ch_id & APP_CH_ID_CH2) {
		dev_id = ENUM_DEVICE_HEADPHONE;
		param_idx = PARAM_IDX_HP;
	}
	else if (ch_id & APP_CH_ID_CH4){
		dev_id = ENUM_DEVICE_SCART;
	}

	if(dev_id == ENUM_DEVICE_SPEAKER || dev_id == ENUM_DEVICE_HEADPHONE) {
		m_volume_comp[param_idx] = dbGains;
		if(m_balance > 0) {
			set_dbGains[GAIN_L] = m_volume_comp[param_idx] - m_balance;
			set_dbGains[GAIN_R] = m_volume_comp[param_idx];
			dev_ch = (CH_L|CH_R);
		} else if(m_balance < 0) {
			set_dbGains[GAIN_L] = m_volume_comp[param_idx];
			set_dbGains[GAIN_R] = m_volume_comp[param_idx] + m_balance;
			dev_ch = (CH_L|CH_R);
		} else {
			dev_ch = CH_ALL;
		}
		for(i = 0; i < 2; i++) {
			if(set_dbGains[i] < HWGAIN_MIN) {
				set_dbGains[i] = HWGAIN_MIN;
			}
			else if(set_dbGains[i] > HWGAIN_MAX) {
				set_dbGains[i] = HWGAIN_MAX;
			}
		}
		set_dspGains[GAIN_L] = HWGain_to_DSPGain(set_dbGains[0]);
		set_dspGains[GAIN_R] = HWGain_to_DSPGain(set_dbGains[1]);
		if(dev_ch == CH_ALL) {
			if(ADSP_SNDOut_SetFineVolume(dev_id, gain.dsp_gain, gain.dsp_fine, FALSE, dev_ch) != KADP_OK) {
				AUDIO_ERROR("[AUDH] %s dev id %x fail\n", __func__, dev_id);
				return NOT_OK;
			}
		} else {
			AUDIO_INFO("[AUDH] %s dev id %x HW dbGains L %d,%d,%d, R %d,%d,%d\n", __FUNCTION__, dev_id, set_dbGains[GAIN_L], set_dspGains[GAIN_L].dsp_gain, set_dspGains[GAIN_L].dsp_fine, set_dbGains[GAIN_R], set_dspGains[GAIN_R].dsp_gain, set_dspGains[GAIN_R].dsp_fine);
			kStatus = ADSP_SNDOut_SetFineVolume(dev_id, set_dspGains[GAIN_L].dsp_gain, set_dspGains[GAIN_L].dsp_fine, TRUE, CH_L);
			kStatus |= ADSP_SNDOut_SetFineVolume(dev_id, set_dspGains[GAIN_R].dsp_gain, set_dspGains[GAIN_R].dsp_fine, TRUE, CH_R);
			kStatus |= ADSP_SNDOut_SetFineVolume(dev_id, gain.dsp_gain, gain.dsp_fine, TRUE, CH_ALL & 0xFC);
			if(kStatus != KADP_OK) {
				AUDIO_ERROR("[AUDH] %s dev id %x fail\n", __func__, dev_id);
				return NOT_OK;
			}
		}
	} else if(dev_id == ENUM_DEVICE_SCART) {
		if(ADSP_SNDOut_SetFineVolume(dev_id, gain.dsp_gain, gain.dsp_fine, FALSE, dev_ch) != KADP_OK) {
			AUDIO_ERROR("[AUDH] %s dev id %x fail\n", __func__, dev_id);
			return NOT_OK;
		}
	}

    return OK;
}
EXPORT_SYMBOL(RHAL_AUDIO_SetPBOutVolume);

DTV_STATUS_T RHAL_AUDIO_SetTSOutVolume(SINT32 dbGains)
{
    HAL_AUDIO_DSPGAIN_T gain;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    gain = HWGain_to_DSPGain(dbGains);
    INFO("%s HW dbGains %d,%d,%d\n", __FUNCTION__, dbGains, gain.dsp_gain, gain.dsp_fine);

    if(ADSP_SNDOut_SetFineVolume(ENUM_DEVICE_SPDIF, gain.dsp_gain, gain.dsp_fine, FALSE, CH_ALL) != KADP_OK) {
        AUDIO_ERROR("SetFineVolume SPDIF_ES fail\n");
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetPBOutMute(BOOLEAN bOnOff, UINT32 ch_id)
{
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s HW Mute:%d, ch_id:%d\n", __FUNCTION__, bOnOff, ch_id);

    if (ch_id & APP_CH_ID_CH0)
        ADSP_SNDOut_SetMute(ENUM_DEVICE_SPEAKER, bOnOff);

    if (ch_id & APP_CH_ID_CH2)
        ADSP_SNDOut_SetMute(ENUM_DEVICE_HEADPHONE, bOnOff);

    if (ch_id & APP_CH_ID_CH4)
        ADSP_SNDOut_SetMute(ENUM_DEVICE_SCART, bOnOff);

    return OK;
}
EXPORT_SYMBOL(RHAL_AUDIO_SetPBOutMute);

DTV_STATUS_T RHAL_AUDIO_SetTSOutMute(BOOLEAN bOnOff)
{
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s HW Mute:%d\n", __FUNCTION__, bOnOff);

    ADSP_SNDOut_SetMute(ENUM_DEVICE_SPDIF, bOnOff);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSPKOutVolume(HAL_AUDIO_VOLUME_T volume)//PB
{
    SINT32 dsp_gain;
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);

#ifdef AVOID_USELESS_RPC
    if (Volume_Compare(volume, g_AudioStatusInfo.curSPKOutVolume))
    {
        INFO("%s Skip volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);
        return OK;
    }
#endif
    dsp_gain = Volume_to_DSPGain(volume);
    if(ADSP_SNDOut_SetFineVolume(ENUM_DEVICE_SPEAKER, dsp_gain, volume.fineVol, FALSE, CH_ALL) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curSPKOutVolume.mainVol = volume.mainVol;
    g_AudioStatusInfo.curSPKOutVolume.fineVol = volume.fineVol;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetUACOut1Volume(HAL_AUDIO_VOLUME_T volume)
{
    SINT32 dsp_gain;
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);

#ifdef AVOID_USELESS_RPC
    if (Volume_Compare(volume, g_AudioStatusInfo.curUACOutVolume[0]))
    {
        INFO("%s Skip volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);
        return OK;
    }
#endif
    dsp_gain = Volume_to_DSPGain(volume);
    if(ADSP_SNDOut_SetFineVolume(ENUM_DEVICE_UACOUT1, dsp_gain, volume.fineVol, FALSE, CH_ALL) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curUACOutVolume[0].mainVol = volume.mainVol;
    g_AudioStatusInfo.curUACOutVolume[0].fineVol = volume.fineVol;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetUACOut2Volume(HAL_AUDIO_VOLUME_T volume)
{
    SINT32 dsp_gain;
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);

#ifdef AVOID_USELESS_RPC
    if (Volume_Compare(volume, g_AudioStatusInfo.curUACOutVolume[1]))
    {
        INFO("%s Skip volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);
        return OK;
    }
#endif
    dsp_gain = Volume_to_DSPGain(volume);
    if(ADSP_SNDOut_SetFineVolume(ENUM_DEVICE_UACOUT2, dsp_gain, volume.fineVol, FALSE, CH_ALL) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curUACOutVolume[1].mainVol = volume.mainVol;
    g_AudioStatusInfo.curUACOutVolume[1].fineVol = volume.fineVol;

    return OK;
}

UINT32 RHAL_AUDIO_UACOUT_Run(HAL_AUDIO_UACOUT_INFO_T *uacinfo)
{
    UINT32 res;
    UINT32 instanceID;
    Base* pBaseObj = Aud_Aout[uacinfo->ao_focus];

    instanceID = (pBaseObj->GetAgentID(pBaseObj)&0xFFFFF000)|(uacinfo->outPinID&0xFFF);
    res = RTKAUDIO_RPC_TOAGENT_RUN_SVC(&instanceID);

    pBaseObj->state = STATE_RUN;
    if(res != S_OK) ERROR("[%s][pBaseObj]RPC return != S_OK\n",__FUNCTION__);
    else DEBUG("[%s] [pBaseObj]RPC return = S_OK\n",__FUNCTION__);
    return res;
}

UINT32 RHAL_AUDIO_UACOUT_Stop(HAL_AUDIO_UACOUT_INFO_T *uacinfo)
{
    UINT32 res;
    UINT32 instanceID;
    Base* pBaseObj = Aud_Aout[uacinfo->ao_focus];

    instanceID = (pBaseObj->GetAgentID(pBaseObj)&0xFFFFF000)|(uacinfo->outPinID&0xFFF);
    res = RTKAUDIO_RPC_TOAGENT_STOP_SVC(&instanceID);

    pBaseObj->state = STATE_STOP;
    if(res != S_OK) ERROR("[%s][pBaseObj]RPC return != S_OK\n",__FUNCTION__);
    else DEBUG("[%s] [pBaseObj]RPC return = S_OK\n",__FUNCTION__);
    return res;
}

DTV_STATUS_T RHAL_AUDIO_SetASRCDrift(float speed, float count_duration, float stc_duration, int pinId)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    float sp = speed;
    float count = count_duration;
    float stc = stc_duration;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_ASRC_DRIFT;
    audioConfig.value[0] = *(unsigned int *)&sp;
    audioConfig.value[1] = *(unsigned int *)&count;
    audioConfig.value[2] = *(unsigned int *)&stc;
    audioConfig.value[3] = pinId;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

int RHAL_AUDIO_UACOUT_INIT_RINGBUFFER(HAL_AUDIO_UACOUT_INFO_T *uacinfo)
{
    struct AUDIO_RPC_PRIVATEINFO_PARAMETERS rpc_parameter;
    struct AUDIO_RPC_PRIVATEINFO_RETURNVAL rpc_ret;
    AUDIO_RPC_RINGBUFFER_HEADER header;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    void *vir_addr = NULL;
    void *unvir_addr = NULL;
    unsigned long rpc_res;
    unsigned long ret = S_OK;
    RINGBUFFER_HEADER *ao_in_ring[2];
    RINGBUFFER_HEADER *ao_in_ring_phy[2];
    unsigned int *ao_in_ring_begin[2];
    UINT32 ao_instanceID;
    UINT32 ao_focus = Aud_mainDecIndex;
    UINT32 i;
    UINT32 ring_size;
    UINT32 channels = 2;

    INFO("%s Enter\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    /* Get Avalaible Outpin */
    memset(&rpc_parameter, 0, sizeof(rpc_parameter));
    memset(&rpc_ret, 0, sizeof(rpc_ret));
    rpc_parameter.type = ENUM_PRIVATEINFO_AUDIO_GET_AVAILABLE_AO_OUTPUT_PIN;

    if (KADP_AUDIO_PrivateInfo(&rpc_parameter, &rpc_ret) != KADP_OK) {
        AUDIO_ERROR("[%s,%d] get pin failed\n", __FUNCTION__, __LINE__);
        return NOT_OK;
    }

    uacinfo->outPinID = rpc_ret.privateInfo[0];
    uacinfo->ao_focus = ao_focus;
    ao_instanceID = ((PPAO*)Aud_Aout[ao_focus]->pDerivedObj)->GetAOAgentID(Aud_Aout[ao_focus]);
    AUDIO_INFO("[UACOUT]outPinID=%d,ao_instanceID=%d, ao_focus=%d\n", uacinfo->outPinID, ao_instanceID, ao_focus);

    ring_size = 24 * 1024;
    for (i = 0; i < channels; i++) {
        vir_addr = dvr_malloc_uncached_specific(sizeof(RINGBUFFER_HEADER), GFP_DCU1, &unvir_addr);
        if (!vir_addr) {
            rtd_pr_adsp_err("[UACOUT]alloc memory RINGBUFFER_HEADER fail\n");
            return -ENOMEM;
        }
        memset(unvir_addr, 0, sizeof(RINGBUFFER_HEADER));
        ao_in_ring[i] = unvir_addr;

        uacinfo->pHaddr[i] = (unsigned long) dvr_to_phys(unvir_addr);
        uacinfo->vHaddr[i] = (unsigned long)unvir_addr;
        rtd_pr_adsp_info("[UACOUT] RINGBUFFER_HEADER ch:%d uncached paddr=%x,vaddr=%x\n", i, uacinfo->pHaddr[i], uacinfo->vHaddr[i]);
        vir_addr = dvr_malloc_uncached_specific(24 * 1024, GFP_DCU1, &unvir_addr);
        if (!vir_addr) {
            rtd_pr_adsp_err("[UACOUT]alloc memory ringbuffer fail\n");
            return -ENOMEM;
        }
        memset(unvir_addr, 0, 24 * 1024);
        ao_in_ring_begin[i] = unvir_addr;

        ao_in_ring[i]->beginAddr = htonl((unsigned long) dvr_to_phys(ao_in_ring_begin[i]));
        ao_in_ring[i]->size = htonl((ring_size));
        ao_in_ring[i]->readPtr[0] = ao_in_ring[i]->beginAddr;
        ao_in_ring[i]->readPtr[1] = ao_in_ring[i]->beginAddr;
        ao_in_ring[i]->readPtr[2] = ao_in_ring[i]->beginAddr;
        ao_in_ring[i]->readPtr[3] = ao_in_ring[i]->beginAddr;
        ao_in_ring[i]->writePtr = ao_in_ring[i]->beginAddr;
        ao_in_ring[i]->numOfReadPtr = htonl(1);

        uacinfo->pBaddr[i] = (unsigned long) dvr_to_phys(ao_in_ring_begin[i]);
        uacinfo->vBaddr[i] = (unsigned long)ao_in_ring_begin[i];
        rtd_pr_adsp_info("[UACOUT] ringbuffer ch:%d uncached paddr=%x,vaddr=%x\n", i, uacinfo->pBaddr[i], uacinfo->vBaddr[i]);
    }

    header.instanceID  = ao_instanceID;
    header.listSize = channels;
    header.pinID  = uacinfo->outPinID;
    header.readIdx = -1;

    ao_in_ring_phy[0] = (RINGBUFFER_HEADER *) dvr_to_phys(ao_in_ring[0]);
    ao_in_ring_phy[1] = (RINGBUFFER_HEADER *) dvr_to_phys(ao_in_ring[1]);

    header.pRingBufferHeaderList[0] = (unsigned long)ao_in_ring_phy[0];
    header.pRingBufferHeaderList[1] = (unsigned long)ao_in_ring_phy[1];

    rpc_res = RTKAUDIO_RPC_TOAGENT_INIT_RING_BUFFER_HEADER_SVC(&header);
    if(rpc_res != S_OK)
        ret = -EFAULT;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_AO_OUTPUT_CONFIG;
    audioConfig.value[0] = uacinfo->channel;
    audioConfig.value[1] = uacinfo->samplerate;
    audioConfig.value[2] = uacinfo->outPinID;
    audioConfig.value[3] = uacinfo->interleave;
	rtd_pr_adsp_info("[UACOUT]set ch:%d,samplerate:%d,outpin:%d,interleave:%d\n",
		audioConfig.value[0], audioConfig.value[1], audioConfig.value[2], audioConfig.value[3]);
    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    return ret;
}

DTV_STATUS_T SetSPDIFOutVolume(HAL_AUDIO_VOLUME_T volume)
{
    SINT32 dsp_gain;
    INFO("%s volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);

#ifdef AVOID_USELESS_RPC
    if (Volume_Compare(volume, g_AudioStatusInfo.curSPDIFOutVolume))
    {
        INFO("%s Skip volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);
        return OK;
    }
#endif
    dsp_gain = Volume_to_DSPGain(volume);
    if(ADSP_SNDOut_SetFineVolume(ENUM_DEVICE_SPDIF, dsp_gain, volume.fineVol, FALSE, CH_ALL) != KADP_OK) {
        ERROR("ADSP_SNDOut_SetFineVolume dsp_gain=%d,volume.fineVol=%d\n", dsp_gain, volume.fineVol);
        return NOT_OK;
    }

    g_AudioStatusInfo.curSPDIFOutVolume.mainVol = volume.mainVol;
    g_AudioStatusInfo.curSPDIFOutVolume.fineVol = volume.fineVol;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSPDIFOutVolume(HAL_AUDIO_VOLUME_T volume)
{
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    return SetSPDIFOutVolume(volume);
}

int AudioSW_SPDIF_Volume(int dbGain_index)
{
    HAL_AUDIO_VOLUME_T volume;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    /*!
      UINT8	mainVol;	// 1 dB step, -127 ~ +30 dB.
      UINT8	fineVol;  	// 1/16 dB step, 0dB ~ 15/16dB
      */
    if (dbGain_index >= 0) {
        volume.mainVol = dbGain_index / 8 + 127;
        volume.fineVol = (dbGain_index % 8) * 2;
    } else if (dbGain_index % 8 == 0) {
        volume.mainVol = dbGain_index / 8 + 127;
        volume.fineVol = 0;
    } else {
        volume.mainVol = dbGain_index / 8 + 127 - 1;
        volume.fineVol = 16 + ((dbGain_index % 8) * 2);
    }
    SetSPDIFOutVolume(volume);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetHPOutVolume(HAL_AUDIO_VOLUME_T volume, BOOLEAN bForced)
{
    SINT32 dsp_gain;
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);

#ifdef AVOID_USELESS_RPC
    if (Volume_Compare(volume, g_AudioStatusInfo.curHPOutVolume))
    {
        INFO("%s Skip volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);
        return OK;
    }
#endif

    dsp_gain = Volume_to_DSPGain(volume);
    if(ADSP_SNDOut_SetFineVolume(ENUM_DEVICE_HEADPHONE, dsp_gain, volume.fineVol, FALSE, CH_ALL) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curHPOutVolume.mainVol = volume.mainVol;
    g_AudioStatusInfo.curHPOutVolume.fineVol = volume.fineVol;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSCARTOutVolume(HAL_AUDIO_VOLUME_T volume, BOOLEAN bForced)
{
    SINT32 dsp_gain;
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);

#ifdef AVOID_USELESS_RPC
    if (Volume_Compare(volume, g_AudioStatusInfo.curSCARTOutVolume))
    {
        INFO("%s Skip volume = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);
        return OK;
    }
#endif

    dsp_gain = Volume_to_DSPGain(volume);
    if(ADSP_SNDOut_SetVolume(ENUM_DEVICE_SCART, dsp_gain) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curSCARTOutVolume.mainVol = volume.mainVol;
    g_AudioStatusInfo.curSCARTOutVolume.fineVol = volume.fineVol;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetAudioDescriptionVolume(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_VOLUME_T volume)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    HAL_AUDIO_VOLUME_T volume_0dB = {.mainVol=0x7F, .fineVol=0x0};

    int balance_adjustment;
    INFO("%s volume = main %d, fine %d\n", __func__, volume.mainVol, volume.fineVol);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;

    balance_adjustment = (int)(volume.mainVol - volume_0dB.mainVol);

    if(balance_adjustment < -32) {
        balance_adjustment = -32;
    } else if(balance_adjustment > 32) {
        balance_adjustment = 32;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_USER_BALANCE_ADJUSTMENT;
    audioConfig.value[0] = (u_int)balance_adjustment;

    /* UI send main volume 95 ~ 159, mapping to Main/AD gain value */
    // UI setting AD level 0, Mainvol = ADvol = volume_0dB
    if (volume.mainVol == 127) {
        currMainVol = volume_0dB;
        currADVol = volume_0dB;
    // UI setting AD level positive value, decrease the Mainvol value.
    } else if (volume.mainVol > 127) {
        volume.mainVol = 127 - (volume.mainVol - 127);
        currMainVol = volume;
        currADVol = volume_0dB;
    // UI setting AD level negtive value, decrease the ADvol value.
    } else {
        volume.mainVol = volume.mainVol;
        currMainVol = volume_0dB;
        currADVol = volume;
    }
    INFO("%s  currMainVol = 0x%x, currADVol = 0x%x\n", __func__, currMainVol.mainVol, currADVol.mainVol);

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    if(Aud_descriptionMode == FALSE)
    {
        ERROR("[%s] Audio Descriptioin mode is OFF, return NOT_OK\n", __func__);
        return NOT_OK;
    }

    ADSP_ADMix_ConfigMixer(Aud_descriptionMode, Volume_to_MixerGain(currMainVol), Volume_to_MixerGain(currADVol));

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDecoderInputMute(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff)
{
    INFO("%s ADEC%d onoff=%d\n", __func__, adecIndex, bOnOff);

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;

    if(ADSP_DEC_SetMute(adecIndex, bOnOff) != KADP_OK)
        return NOT_OK;

    SetDecInMute(adecIndex, (UINT32)bOnOff);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDecoderInputESMute(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetMixerInputMute(HAL_AUDIO_MIXER_INDEX_T mixerIndex, BOOLEAN bOnOff)
{
    HAL_AUDIO_RESOURCE_T resID;
    INFO("%s AMIXER%d onoff=%d\n", __func__, mixerIndex, bOnOff);
    if(!RangeCheck(mixerIndex, 0, AUD_AMIX_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;

    SetAmixerUserMute(mixerIndex, (UINT32)bOnOff);

    resID = amixer2res(mixerIndex);
    ADSP_AMIXER_SetMute(mixerIndex, GetAmixerUserMute(mixerIndex));

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSPKOutMute(BOOLEAN bOnOff)
{
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(SNDOut_SetMute(ENUM_DEVICE_SPEAKER, bOnOff) != KADP_OK) {
        ERROR("%s SNDOut_SetMute fail\n", __func__);
        return NOT_OK;
    }

    g_AudioStatusInfo.curSPKMuteStatus = bOnOff;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSPDIFOutMute(BOOLEAN bOnOff)
{
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(SNDOut_SetMute(ENUM_DEVICE_SPDIF, bOnOff) != KADP_OK)
        return NOT_OK;
    if(SNDOut_SetMute(ENUM_DEVICE_SPDIF_ES, bOnOff) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curSPDIFMuteStatus = bOnOff;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetHPOutMute(BOOLEAN bOnOff)
{
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(SNDOut_SetMute(ENUM_DEVICE_HEADPHONE, bOnOff) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curHPMuteStatus = bOnOff;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSCARTOutMute(BOOLEAN bOnOff)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 i;

    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_MUTE;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = ENUM_DEVICE_SCART;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = 0xFF;
    for(i = 0; i < 8; i++)
    {
        audioConfig.value[4+i] = bOnOff;
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    g_AudioStatusInfo.curSCARTMuteStatus = bOnOff;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetUACOut1Mute(BOOLEAN bOnOff)
{
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(SNDOut_SetMute(ENUM_DEVICE_UACOUT1, bOnOff) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curUACOUTMuteStatus[0] = bOnOff;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetUACOut2Mute(BOOLEAN bOnOff)
    {
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(SNDOut_SetMute(ENUM_DEVICE_UACOUT2, bOnOff) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curUACOUTMuteStatus[1] = bOnOff;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetSPKOutMuteStatus(BOOLEAN *pOnOff)
{
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    if(pOnOff == NULL) return NOT_OK;

    *pOnOff = g_AudioStatusInfo.curSPKMuteStatus;
    INFO("%s pOnOff=%d\n", __func__, *pOnOff);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetSPDIFOutMuteStatus(BOOLEAN *pOnOff)
{
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    if(pOnOff == NULL) return NOT_OK;

    *pOnOff = g_AudioStatusInfo.curSPDIFMuteStatus;
    INFO("%s pOnOff=%d\n", __func__, *pOnOff);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetHPOutMuteStatus(BOOLEAN *pOnOff)
{
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    if(pOnOff == NULL) return NOT_OK;

    *pOnOff = g_AudioStatusInfo.curHPMuteStatus;
    INFO("%s pOnOff=%d\n", __func__, *pOnOff);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetSCARTOutMuteStatus(BOOLEAN *pOnOff)
{
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    if(pOnOff == NULL) return NOT_OK;

    *pOnOff = g_AudioStatusInfo.curSCARTMuteStatus;
    INFO("%s pOnOff=%d\n", __func__, *pOnOff);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSPKOutDelayTime(UINT32 delayTime, BOOLEAN bForced)
{
    INFO("%s delayTime %d\n", __func__, delayTime);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(ADSP_SNDOut_SetDelay(ENUM_DEVICE_SPEAKER, delayTime) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curSPKOutDelay = delayTime;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSPDIFOutDelayTime(UINT32 delayTime, BOOLEAN bForced)
{
    INFO("%s delayTime %d\n", __func__, delayTime);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(ADSP_SNDOut_SetDelay(ENUM_DEVICE_SPDIF, delayTime) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curSPDIFOutDelay = delayTime;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetHPOutDelayTime(UINT32 delayTime, BOOLEAN bForced)
{
    INFO("%s delayTime %d\n", __func__, delayTime);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(ADSP_SNDOut_SetDelay(ENUM_DEVICE_HEADPHONE, delayTime) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curHPOutDelay = delayTime;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSCARTOutDelayTime(UINT32 delayTime, BOOLEAN bForced)
{
    INFO("%s delayTime %d\n", __func__, delayTime);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(ADSP_SNDOut_SetDelay(ENUM_DEVICE_SCART, delayTime) != KADP_OK)
        return NOT_OK;

    g_AudioStatusInfo.curSCARTOutDelay = delayTime;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetStatusInfo(HAL_AUDIO_COMMON_INFO_T *pAudioStatusInfo)
{
    KADP_AO_SPDIF_CHANNEL_STATUS_BASIC sc;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    if(pAudioStatusInfo == NULL) return NOT_OK;

    if(g_AudioStatusInfo.curAudioSpdifMode == HAL_AUDIO_SPDIF_PCM)
        g_AudioStatusInfo.bCurAudioSpdifOutPCM  = TRUE;
    else
        g_AudioStatusInfo.bCurAudioSpdifOutPCM  = FALSE;

    memset(&sc, 0, sizeof(KADP_AO_SPDIF_CHANNEL_STATUS_BASIC));
    if (KADP_AUDIO_GetAudioSpdifChannelStatus(&sc, AUDIO_OUT))
    {
        ERROR("[%s,%d] get spdif channel status failed\n", __func__, __LINE__);
        return NOT_OK;
    }

    if(sc.data_type == 0) // pcm
        g_AudioStatusInfo.bAudioSpdifOutPCM  = TRUE;
    else
        g_AudioStatusInfo.bCurAudioSpdifOutPCM  = FALSE;

    memcpy(pAudioStatusInfo, &g_AudioStatusInfo, sizeof(HAL_AUDIO_COMMON_INFO_T));
    return OK;
}

static inline HAL_AUDIO_DELAY_INPUT_SRC res2DlyIpt(HAL_AUDIO_RESOURCE_T res_id)
{
   HAL_AUDIO_DELAY_INPUT_SRC delay_input_src = DELAY_INPUT_SRC_UNKNOWN;

   switch(res_id)
   {
       case HAL_AUDIO_RESOURCE_ATP0:
       case HAL_AUDIO_RESOURCE_ATP1:
           delay_input_src = DELAY_INPUT_SRC_TP;
           break;
       case HAL_AUDIO_RESOURCE_ADC:
           delay_input_src = DELAY_INPUT_SRC_ADC;
           break;
       case HAL_AUDIO_RESOURCE_AAD:
           delay_input_src = DELAY_INPUT_SRC_AAD;
           break;
       case HAL_AUDIO_RESOURCE_HDMI:
       case HAL_AUDIO_RESOURCE_HDMI0:
       case HAL_AUDIO_RESOURCE_HDMI1:
       case HAL_AUDIO_RESOURCE_HDMI2:
       case HAL_AUDIO_RESOURCE_HDMI3:
           delay_input_src = DELAY_INPUT_SRC_HDMI;
           break;
       default:
           break;
   }

   return delay_input_src;
}

static inline AUDIO_SOURCE ipt2audiosrc(HAL_AUDIO_DELAY_INPUT_SRC input_src)
{
    AUDIO_SOURCE ipt_src = SOURCE_UNKNOWN;

    switch(input_src) {
        case DELAY_INPUT_SRC_HDMI:
            ipt_src = SOURCE_HDMI;
            break;
        case DELAY_INPUT_SRC_TP:
            ipt_src = SOURCE_DTV;
            break;
        case DELAY_INPUT_SRC_MEDIA:
            ipt_src = SOURCE_GST_HALF_TUNNEL;
            break;
        case DELAY_INPUT_SRC_AAD:
            ipt_src = SOURCE_ATV;
            break;
        case DELAY_INPUT_SRC_ADC:
            ipt_src = SOURCE_ADC;
            break;
        default:
            ipt_src = SOURCE_UNKNOWN;
            ERROR("[%s,%d] ipt_src %d not support !!!\n", __func__, __LINE__, input_src);
            break;
    }
    return ipt_src;
}

static inline SNDOUT_DEVICE soundOutType2sndoutdevice(HAL_AUDIO_SNDOUT_T soundOutType)
{
    SNDOUT_DEVICE sndOut;

    switch(soundOutType) {
        case HAL_AUDIO_SPK:
            sndOut = SNDOUT_SPK;
            break;
        case HAL_AUDIO_SPDIF:
            sndOut = SNDOUT_ARC;
            break;
        case HAL_AUDIO_BT:
            sndOut = SNDOUT_BLUETOOTH;
            break;
        case HAL_AUDIO_SB_SPDIF:
        case HAL_AUDIO_SB_PCM:
        case HAL_AUDIO_SB_CANVAS:
        case HAL_AUDIO_HP:
        case HAL_AUDIO_SCART:
        case HAL_AUDIO_SPDIF_ES:
            sndOut = SNDOUT_SPK;
            break;
        default:
            sndOut = SNDOUT_NO_OUTPUT;
            ERROR("[%s,%d] soundOutType %d not support !!!\n", __func__, __LINE__, soundOutType);
            break;
    }
    return sndOut;
}

DTV_STATUS_T RHAL_AUDIO_SNDOUT_GetLatency(HAL_AUDIO_SNDOUT_T soundOutType, int *latency_ms)
{
    HAL_AUDIO_ES_INFO_T AudioESInfo;
    AUDIO_LATENCY_INFO latency_info;
    UINT16 sampleRate = 0;
    HAL_AUDIO_DELAY_INPUT_SRC input_src;
    HAL_AUDIO_RESOURCE_T decIptResId;
    HAL_AUDIO_ADEC_INDEX_T input_port = (HAL_AUDIO_ADEC_INDEX_T)Aud_mainDecIndex;
    HAL_AUDIO_RESOURCE_T curResourceId = adec2res(input_port);
    struct AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
    AUDIO_RPC_PRIVATEINFO_RETURNVAL ret;

    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    if(latency_ms == NULL) return NOT_OK;

    memset(&parameter, 0, sizeof(struct AUDIO_RPC_PRIVATEINFO_PARAMETERS));
    memset(&ret, 0, sizeof(AUDIO_RPC_PRIVATEINFO_RETURNVAL));

    decIptResId = GetNonOutputModuleSingleInputResource(ResourceStatus[curResourceId]);// dec input source
    input_src = res2DlyIpt(decIptResId);
    latency_info.iptSrc = ipt2audiosrc(input_src);
    latency_info.sndOut = soundOutType2sndoutdevice(soundOutType);

    if (latency_info.sndOut == SNDOUT_NO_OUTPUT) {
        ERROR("[%s,%d] soundOutType %d not support\n", __func__, __LINE__, soundOutType);
        return NOT_OK;
    }

    if(AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0) /*&& adecIndex == Aud_mainDecIndex*/) {
        DTV_STATUS_T result = AUDIO_Get_MediaSourceLatency(&latency_info);
        if (result != OK) {
            ERROR("[%s,%d] Get Latency failed\n", __func__, __LINE__);
            return NOT_OK;
        }
        *latency_ms = latency_info.latency;
        INFO("latency %d,in:%d,out:%d is not in run state\n",*latency_ms, latency_info.iptSrc, latency_info.sndOut);

        return OK;
    } else {
        if (!IsValidADECInstance(input_port)) {
            ERROR("%s Aud_dec[%d] NULL\n", __func__, input_port);
            return NOT_OK;
        }
        parameter.instanceID = Aud_dec[input_port]->GetAgentID(Aud_dec[input_port]);
        parameter.type = ENUM_PRIVATEINFO_AUDIO_GET_SNDOUT_LATENCY;
        parameter.privateInfo[0] = latency_info.iptSrc;
        parameter.privateInfo[1] = latency_info.sndOut;

        if (KADP_AUDIO_PrivateInfo(&parameter, &ret) != KADP_OK) {
            ERROR("[%s,%d] Get Latency failed\n", __func__, __LINE__);
            return NOT_OK;
        }
        *latency_ms = ret.privateInfo[0];
        INFO("latency %d,in:%d,out:%d\n",*latency_ms, latency_info.iptSrc, latency_info.sndOut);
    }

    memset(&AudioESInfo, 0, sizeof(HAL_AUDIO_ES_INFO_T));
    RAL_AUDIO_GetESInfo(Aud_mainDecIndex, &AudioESInfo, &sampleRate);
    if (AudioESInfo.adecFormat == HAL_AUDIO_SRC_TYPE_UNKNOWN || AudioESInfo.audioMode == HAL_AUDIO_MODE_UNKNOWN || sampleRate == 0) {
        INFO("dec not ready (ch=%d, fs=%d), use predict value",AudioESInfo.audioMode, sampleRate);
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SPDIF_SetOutputType(HAL_AUDIO_SPDIF_MODE_T eSPDIFMode, BOOLEAN bForced)
{
    INFO("%s %d\n", __func__, eSPDIFMode);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    switch(eSPDIFMode)
    {
    case HAL_AUDIO_SPDIF_AUTO:
    case HAL_AUDIO_SPDIF_AUTO_NODTS:
        _AudioSPDIFMode = NON_PCM_OUT_EN_AUTO; break;
    case HAL_AUDIO_SPDIF_AUTO_AAC:
    case HAL_AUDIO_SPDIF_AUTO_AAC_NODTS:
        _AudioSPDIFMode = NON_PCM_OUT_EN_AUTO_AAC; break;
    case HAL_AUDIO_SPDIF_FORCE_AC3:
    case HAL_AUDIO_SPDIF_FORCE_AC3_DTS:
        _AudioSPDIFMode = NON_PCM_OUT_EN_AUTO_FORCED_AC3; break;
    case HAL_AUDIO_SPDIF_BYPASS:
    case HAL_AUDIO_SPDIF_BYPASS_NODTS:
        _AudioSPDIFMode = NON_PCM_OUT_EN_AUTO_BYPASS; break;
    case HAL_AUDIO_SPDIF_BYPASS_AAC:
    case HAL_AUDIO_SPDIF_BYPASS_AAC_NODTS:
        _AudioSPDIFMode = NON_PCM_OUT_EN_AUTO_BYPASS_AAC; break;
    case HAL_AUDIO_SPDIF_PCM:
        _AudioSPDIFMode = ENABLE_DOWNMIX;
        break;
    default:
        ERROR("error type %d\n", eSPDIFMode);
        return NOT_OK;
    }

    //control DTS bypass
    switch(eSPDIFMode)
    {
    case HAL_AUDIO_SPDIF_PCM:
    case HAL_AUDIO_SPDIF_FORCE_AC3:
    case HAL_AUDIO_SPDIF_BYPASS_NODTS:
    case HAL_AUDIO_SPDIF_AUTO_NODTS:
    case HAL_AUDIO_SPDIF_BYPASS_AAC_NODTS:
    case HAL_AUDIO_SPDIF_AUTO_AAC_NODTS:
        //muted DTS so amp will not show DTS logo.
        ADSP_SetDTSSupport(FALSE);
        break;
    default:
        //turn on DTS bypass so amp show DTS logo.
        ADSP_SetDTSSupport(TRUE);
        break;
    }

    Update_RawMode_by_connection();
    g_AudioStatusInfo.curAudioSpdifMode = eSPDIFMode;

    return OK;
}

// for TB24 ARC DD+ output 2016/06/4 by
DTV_STATUS_T RHAL_AUDIO_ARC_SetOutputType(HAL_AUDIO_ARC_MODE_T eARCMode, BOOLEAN bForced)
{
    INFO("%s %d\n", __func__, eARCMode);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    switch(eARCMode)
    {
    case HAL_AUDIO_ARC_AUTO:
    case HAL_AUDIO_ARC_AUTO_NODTS:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO; break;
    case HAL_AUDIO_ARC_AUTO_AAC:
    case HAL_AUDIO_ARC_AUTO_AAC_NODTS:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_AAC; break;
    case HAL_AUDIO_ARC_AUTO_EAC3:
    case HAL_AUDIO_ARC_AUTO_EAC3_NODTS:
    case HAL_AUDIO_ARC_AUTO_EAC3_DTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_DDP; break;
    case HAL_AUDIO_ARC_AUTO_EAC3_AAC:
    case HAL_AUDIO_ARC_AUTO_EAC3_AAC_NODTS:
    case HAL_AUDIO_ARC_AUTO_EAC3_AAC_DTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_DDP_AAC; break;
    case HAL_AUDIO_ARC_AUTO_MAT:
    case HAL_AUDIO_ARC_AUTO_MAT_NODTS:
    case HAL_AUDIO_ARC_AUTO_MAT_NODTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_MAT; break;
    case HAL_AUDIO_ARC_AUTO_MAT_AAC:
    case HAL_AUDIO_ARC_AUTO_MAT_AAC_NODTS:
    case HAL_AUDIO_ARC_AUTO_MAT_AAC_NODTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_MAT_AAC; break;
    case HAL_AUDIO_ARC_FORCE_AC3:
    case HAL_AUDIO_ARC_FORCE_AC3_DTS:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_FORCED_AC3; break;
    case HAL_AUDIO_ARC_FORCE_EAC3:
    case HAL_AUDIO_ARC_FORCE_EAC3_DTS:
    case HAL_AUDIO_ARC_FORCE_EAC3_DTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_FORCED_DDP; break;
    case HAL_AUDIO_ARC_BYPASS_NODTS:
    case HAL_AUDIO_ARC_BYPASS:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_BYPASS; break;
    case HAL_AUDIO_ARC_BYPASS_EAC3_NODTS:
    case HAL_AUDIO_ARC_BYPASS_EAC3:
    case HAL_AUDIO_ARC_BYPASS_EAC3_DTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_BYPASS_DDP; break;
    case HAL_AUDIO_ARC_FORCE_MAT:
    case HAL_AUDIO_ARC_FORCE_MAT_DTS:
    case HAL_AUDIO_ARC_FORCE_MAT_NODTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_FORCED_MAT; break;
    case HAL_AUDIO_ARC_BYPASS_MAT_NODTS:
    case HAL_AUDIO_ARC_BYPASS_MAT:
    case HAL_AUDIO_ARC_BYPASS_MAT_NODTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_BYPASS_MAT; break;
    case HAL_AUDIO_ARC_BYPASS_AAC_NODTS:
    case HAL_AUDIO_ARC_BYPASS_AAC:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_BYPASS_AAC; break;
    case HAL_AUDIO_ARC_BYPASS_EAC3_AAC_NODTS:
    case HAL_AUDIO_ARC_BYPASS_EAC3_AAC:
    case HAL_AUDIO_ARC_BYPASS_EAC3_AAC_DTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_BYPASS_DDP_AAC; break;
    case HAL_AUDIO_ARC_BYPASS_MAT_AAC_NODTS:
    case HAL_AUDIO_ARC_BYPASS_MAT_AAC:
    case HAL_AUDIO_ARC_BYPASS_MAT_AAC_NODTSHD:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_BYPASS_MAT_AAC; break;
    case HAL_AUDIO_ARC_BYPASS_MAT_TRUEHD:
    case HAL_AUDIO_ARC_BYPASS_MAT_TRUEHD_NODTSHD:
    case HAL_AUDIO_ARC_BYPASS_MAT_TRUEHD_NODTS:
        _AudioARCMode = NON_PCM_OUT_EN_AUTO_BYPASS_MAT_TRUEHD; break;
    case HAL_AUDIO_ARC_PCM:
        _AudioARCMode = ENABLE_DOWNMIX; break;
    default:
        ERROR("error type %d\n", eARCMode);
        return NOT_OK;
    }
    //control DTS bypass
    switch(eARCMode)
    {
    case HAL_AUDIO_ARC_FORCE_AC3:
    case HAL_AUDIO_ARC_FORCE_EAC3:
    case HAL_AUDIO_ARC_FORCE_MAT:
    case HAL_AUDIO_ARC_BYPASS_NODTS:
    case HAL_AUDIO_ARC_BYPASS_EAC3_NODTS:
    case HAL_AUDIO_ARC_BYPASS_EAC3_AAC_NODTS:
    case HAL_AUDIO_ARC_BYPASS_MAT_NODTS:
    case HAL_AUDIO_ARC_BYPASS_MAT_AAC_NODTS:
    case HAL_AUDIO_ARC_BYPASS_AAC_NODTS:
    case HAL_AUDIO_ARC_AUTO_NODTS:
    case HAL_AUDIO_ARC_AUTO_EAC3_NODTS:
    case HAL_AUDIO_ARC_AUTO_EAC3_AAC_NODTS:
    case HAL_AUDIO_ARC_AUTO_MAT_NODTS:
    case HAL_AUDIO_ARC_AUTO_MAT_AAC_NODTS:
    case HAL_AUDIO_ARC_BYPASS_MAT_TRUEHD_NODTS:
    case HAL_AUDIO_ARC_AUTO_AAC_NODTS:
        //muted DTS so amp will not show DTS logo.
        ADSP_SetDTSSupport(FALSE);
        ADSP_SetDTSHDSupport(FALSE);
        break;
    case HAL_AUDIO_ARC_AUTO_EAC3_DTSHD:
    case HAL_AUDIO_ARC_AUTO_EAC3_AAC_DTSHD:
    case HAL_AUDIO_ARC_FORCE_EAC3_DTSHD:
    case HAL_AUDIO_ARC_BYPASS_EAC3_DTSHD:
    case HAL_AUDIO_ARC_BYPASS_EAC3_AAC_DTSHD:
        ADSP_SetDTSSupport(TRUE);
        ADSP_SetDTSHDSupport(TRUE);
        break;
    case HAL_AUDIO_ARC_FORCE_MAT_NODTSHD:
    case HAL_AUDIO_ARC_BYPASS_MAT_NODTSHD:
    case HAL_AUDIO_ARC_BYPASS_MAT_AAC_NODTSHD:
    case HAL_AUDIO_ARC_BYPASS_MAT_TRUEHD_NODTSHD:
    case HAL_AUDIO_ARC_AUTO_MAT_NODTSHD:
    case HAL_AUDIO_ARC_AUTO_MAT_AAC_NODTSHD:
    case HAL_AUDIO_ARC_AUTO_EAC3:
    case HAL_AUDIO_ARC_AUTO_EAC3_AAC:
    case HAL_AUDIO_ARC_BYPASS_EAC3:
    case HAL_AUDIO_ARC_BYPASS_EAC3_AAC:
        //muted DTSHD so amp will not show DTS HD logo.
        ADSP_SetDTSSupport(TRUE);
        ADSP_SetDTSHDSupport(FALSE);
        break;
    default:
        //turn on DTS bypass so amp show DTS logo.
        ADSP_SetDTSSupport(TRUE);
        ADSP_SetDTSHDSupport(TRUE);
        break;
    }

    Update_RawMode_by_connection();
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_ATMOS_EncodeOnOff(BOOLEAN bOnOff)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_EAC3_ATMOS_ENCODE_ONOFF;
    audioConfig.value[0] = (u_int)bOnOff;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SPDIF_SetCopyInfo(HAL_AUDIO_SPDIF_COPYRIGHT_T copyInfo)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s %d\n", __func__, copyInfo);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    if(copyInfo > HAL_AUDIO_SPDIF_COPY_NEVER)
    {
        ERROR("error type %d\n", HAL_AUDIO_SPDIF_COPY_NEVER);
        return NOT_OK;
    }

    g_AudioStatusInfo.curSpdifCopyInfo = copyInfo;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_SPDIF_CS_INFO;
    audioConfig.value[0] = TRUE;
    audioConfig.value[1] = HAL_SPDIF_CONSUMER_USE;
    audioConfig.value[5] = HAL_SPDIF_WORD_LENGTH_16; // 16 bit
    audioConfig.value[3] = g_AudioStatusInfo.curSpdifCategoryCode;
    if(g_AudioStatusInfo.curSpdifCopyInfo == HAL_AUDIO_SPDIF_COPY_FREE)
    {
        audioConfig.value[2] = HAL_SPDIF_COPYRIGHT_FREE;
        audioConfig.value[4] = HAL_SPDIF_CATEGORY_L_BIT_IS_0;
    }
    else
    {
        audioConfig.value[2] = HAL_SPDIF_COPYRIGHT_NEVER;
        if(g_AudioStatusInfo.curSpdifCopyInfo == HAL_AUDIO_SPDIF_COPY_ONCE)
        {
            audioConfig.value[4] = HAL_SPDIF_CATEGORY_L_BIT_IS_0;
        }
        else
        {
            //RTWTV-247 nerver is the same with  no more
            audioConfig.value[4] = HAL_SPDIF_CATEGORY_L_BIT_IS_1;  //HAL_AUDIO_SPDIF_COPY_NEVER
        }
    }

    if (SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        ERROR("[%s,%d] set channel status failed\n", __func__, __LINE__);
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SPDIF_SetCategoryCode(UINT8 categoryCode)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s %d\n", __func__, categoryCode);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;
    categoryCode = (categoryCode & 0x7F);// category occupy 7 bits7

    g_AudioStatusInfo.curSpdifCategoryCode = categoryCode;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_SPDIF_CS_INFO;
    audioConfig.value[0] = TRUE;
    audioConfig.value[1] = HAL_SPDIF_CONSUMER_USE; // CONSUMER
    audioConfig.value[5] = HAL_SPDIF_WORD_LENGTH_16; // 16 bit
    audioConfig.value[3] = g_AudioStatusInfo.curSpdifCategoryCode;
    if(g_AudioStatusInfo.curSpdifCopyInfo == HAL_AUDIO_SPDIF_COPY_FREE)
    {
        audioConfig.value[2] = HAL_SPDIF_COPYRIGHT_FREE;
        audioConfig.value[4] = HAL_SPDIF_CATEGORY_L_BIT_IS_1;
    }
    else
    {
        audioConfig.value[2] = HAL_SPDIF_COPYRIGHT_NEVER;
        if(g_AudioStatusInfo.curSpdifCopyInfo == HAL_AUDIO_SPDIF_COPY_ONCE)
        {
            audioConfig.value[4] = HAL_SPDIF_CATEGORY_L_BIT_IS_0;
        }
        else
        {
            audioConfig.value[4] = HAL_SPDIF_CATEGORY_L_BIT_IS_1;  //HAL_AUDIO_SPDIF_COPY_NEVER
        }
    }

    if (SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        ERROR("[%s,%d] set channel status failed\n", __func__, __LINE__);
        return NOT_OK;
    }

    return OK;
}

/* AAD */
DTV_STATUS_T RHAL_AUDIO_SIF_SetInputSource(HAL_AUDIO_SIF_INPUT_T sifSource)
{
    //HAL enum format to RTK format
    SIF_INPUT_SOURCE  rtk_sif_input_source;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    switch(sifSource)
    {
        case HAL_AUDIO_SIF_INPUT_EXTERNAL:
            rtk_sif_input_source = SIF_FROM_SIF_ADC;
            break;
        case HAL_AUDIO_SIF_INPUT_INTERNAL:
            rtk_sif_input_source = SIF_FROM_IFDEMOD;
            break;
        default:
            ERROR("[%s] sif input %d\n",__func__,sifSource);
            return API_INVALID_PARAMS;
    }
    INFO("[%s] sif input %d %d\n",__func__,sifSource, rtk_sif_input_source);

    KADP_Audio_HwpSetSIFDataSource(rtk_sif_input_source);
    g_AudioSIFInfo.sifSource  = sifSource;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_DetectSoundSystemStd(HAL_AUDIO_SIF_SOUNDSYSTEM_T setSoundSystem, BOOLEAN bManualMode,
                                                            HAL_AUDIO_SIF_SOUNDSYSTEM_T *pDetectSoundSystem, HAL_AUDIO_SIF_STANDARD_T *pDetectSoundStd,
                                                            UINT32 *pSignalQuality)
{
	ATV_SOUND_STD_MAIN_SYSTEM atv_sound_std_main_system = ATV_SOUND_UNKNOWN_SYSTEM;
	ATV_SOUND_STD_MAIN_SYSTEM HWDetectSoundSystem = ATV_SOUND_UNKNOWN_SYSTEM;
	ATV_SOUND_STD HWDetectSoundStd = ATV_SOUND_STD_UNKNOWN;
	UINT32 pToneSNR = 0;
	SIF_INPUT_SOURCE  rtk_sif_input_source;

	INFO("%s Enter.\n", __func__);

	if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

	//set band internal/external
	switch(g_AudioSIFInfo.sifSource)
	{
		case HAL_AUDIO_SIF_INPUT_EXTERNAL:
			rtk_sif_input_source = SIF_FROM_SIF_ADC;
			break;
		case HAL_AUDIO_SIF_INPUT_INTERNAL:
			rtk_sif_input_source = SIF_FROM_IFDEMOD;
			break;
		default:
			ERROR("[%s] sif input %d\n",__func__, g_AudioSIFInfo.sifSource);
			return API_INVALID_PARAMS;
	}

	KADP_Audio_AtvSetScanStdFlag(true);
	//detect sound system
	switch(setSoundSystem)
	{
		case  HAL_AUDIO_SIF_SYSTEM_BG:
			atv_sound_std_main_system = ATV_SOUND_BG_SYSTEM;
			break;
		case  HAL_AUDIO_SIF_SYSTEM_I:
			atv_sound_std_main_system = ATV_SOUND_I_SYSTEM;
			break;
		case  HAL_AUDIO_SIF_SYSTEM_DK:
			KADP_Audio_AtvSetMtsPriority(ATV_MTS_PRIO_DK);
			atv_sound_std_main_system = ATV_SOUND_DK_SYSTEM;
			break;
		case  HAL_AUDIO_SIF_SYSTEM_L:
			KADP_Audio_AtvSetMtsPriority(ATV_MTS_PRIO_L);
			atv_sound_std_main_system = ATV_SOUND_L_SYSTEM;
			break;
		case  HAL_AUDIO_SIF_SYSTEM_MN:
			KADP_Audio_AtvSetMtsPriority(ATV_MTS_PRIO_BTSC);
			atv_sound_std_main_system = ATV_SOUND_MN_SYSTEM;
			break;
		case  HAL_AUDIO_SIF_SYSTEM_UNKNOWN:
			atv_sound_std_main_system = ATV_SOUND_AUTO_SYSTEM;
			break;
		default:
			ERROR("[%s] not in case1... %d\n",__func__,setSoundSystem);
			return API_INVALID_PARAMS;
	}

	KADP_Audio_HwpSetAtvAudioBand(rtk_sif_input_source, atv_sound_std_main_system);
	KADP_Audio_HwpSIFGetMainToneSNR(atv_sound_std_main_system, &HWDetectSoundSystem, &HWDetectSoundStd, &pToneSNR);
	switch (HWDetectSoundStd)
	{
		case ATV_SOUND_STD_MN_MONO:
			*pDetectSoundStd = HAL_AUDIO_SIF_MN_FM;
			break;
		case ATV_SOUND_STD_BTSC:
			*pDetectSoundStd = HAL_AUDIO_SIF_MN_BTSC;
			break;
		case ATV_SOUND_STD_A2_M:
			*pDetectSoundStd = HAL_AUDIO_SIF_MN_A2;
			break;
		case ATV_SOUND_STD_EIAJ:
			*pDetectSoundStd = HAL_AUDIO_SIF_MN_EIAJ;
			break;
		case ATV_SOUND_STD_BG_MONO:
			*pDetectSoundStd = HAL_AUDIO_SIF_BG_FM;
			break;
		case ATV_SOUND_STD_A2_BG:
			*pDetectSoundStd = HAL_AUDIO_SIF_BG_A2;
			break;
		case ATV_SOUND_STD_NICAM_BG:
			*pDetectSoundStd = HAL_AUDIO_SIF_BG_NICAM;
			break;
		case ATV_SOUND_STD_DK_MONO:
			*pDetectSoundStd = HAL_AUDIO_SIF_DK_FM;
			break;
		case ATV_SOUND_STD_A2_DK1:
			*pDetectSoundStd = HAL_AUDIO_SIF_DK1_A2;
			break;
		case ATV_SOUND_STD_A2_DK2:
			*pDetectSoundStd = HAL_AUDIO_SIF_DK2_A2;
			break;
		case ATV_SOUND_STD_A2_DK3:
			*pDetectSoundStd = HAL_AUDIO_SIF_DK3_A2;
			break;
		case ATV_SOUND_STD_NICAM_DK:
			*pDetectSoundStd = HAL_AUDIO_SIF_DK_NICAM;
			break;
		case ATV_SOUND_STD_AM_MONO:
			*pDetectSoundStd = HAL_AUDIO_SIF_L_AM;
			break;
		case ATV_SOUND_STD_NICAM_L:
			*pDetectSoundStd = HAL_AUDIO_SIF_L_NICAM;
			break;
		case ATV_SOUND_STD_FM_MONO_NO_I:
			*pDetectSoundStd = HAL_AUDIO_SIF_I_FM;
			break;
		case ATV_SOUND_STD_NICAM_I:
			*pDetectSoundStd = HAL_AUDIO_SIF_I_NICAM;
			break;
		case ATV_SOUND_STD_UNKNOWN:
		default:
			ERROR("[%s][%d] not in case... %d\n",__func__, __LINE__, HWDetectSoundStd);
			*pDetectSoundStd = HAL_AUDIO_SIF_UNKNOWN;
			break;
	}

	if(bManualMode == FALSE)
	{
		switch(HWDetectSoundSystem) //auto mode reuten HWDetectSoundSystem
		{
			case  ATV_SOUND_BG_SYSTEM:
				*pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_BG;
				break;
			case  ATV_SOUND_I_SYSTEM:
				*pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_I;
				break;
			case  ATV_SOUND_DK_SYSTEM:
				*pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_DK;
				break;
			case  ATV_SOUND_L_SYSTEM:
				*pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_L;
				break;
			case  ATV_SOUND_MN_SYSTEM:
				*pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_MN;
				break;
			case  ATV_SOUND_UNKNOWN_SYSTEM:
				*pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_UNKNOWN;
				break;
			default:
				ERROR("[%s] not in case... %d\n",__func__,HWDetectSoundSystem);
				return API_INVALID_PARAMS;
		}
	}
	else
	{
		*pDetectSoundSystem = setSoundSystem;//  manual mode setSoundSystem and *pDetectSoundSystem must be the same
	}

	*pSignalQuality = (UINT32)pToneSNR;
	return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_NoSignalMute(BOOLEAN Enable)
{
    int ret = 0;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if (Enable) ret = KADP_Audio_AtvConfigNoSignalNeed2Mute(true);
    else ret = KADP_Audio_AtvConfigNoSignalNeed2Mute(false);

    return (DTV_STATUS_T)ret;
}

DTV_STATUS_T RHAL_AUDIO_SIF_AutoChangeSoundMode(BOOLEAN Enable)
{
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    KADP_Audio_AtvEnableAutoChangeSoundModeFlag(Enable);

    return 0;
}
DTV_STATUS_T RHAL_AUDIO_SIF_AutoChangeSoundStd(BOOLEAN Enable)
{
    int ret = 0;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if (Enable) ret = KADP_Audio_AtvEnableAutoChangeStdFlag(true);
    else ret = KADP_Audio_AtvEnableAutoChangeStdFlag(false);

    return (DTV_STATUS_T)ret;
}

DTV_STATUS_T RHAL_AUDIO_SIF_FirstTimePlayMono(BOOLEAN Enable)
{
    int ret = 0;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if (Enable) ret = KADP_Audio_AtvForceFirstTimeMonoSoundFlag(true);
    else ret = KADP_Audio_AtvForceFirstTimeMonoSoundFlag(false);

    return (DTV_STATUS_T)ret;
}

DTV_STATUS_T RHAL_AUDIO_SIF_SetFwPriority(UINT16 Priority)
{
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(KADP_Audio_AtvSetFwPriority(Priority) < 0)
    {
        return NOT_OK;
    }

    return OK;
}

HAL_AUDIO_SIF_STANDARD_T Audio_SoundStd_KadpToHal(ATV_SOUND_STD KadpSoundStd)
{
	switch(KadpSoundStd)
	{
		case ATV_SOUND_STD_MN_MONO:
			return HAL_AUDIO_SIF_MN_FM;
		case ATV_SOUND_STD_BTSC:
			return HAL_AUDIO_SIF_MN_BTSC;
		case ATV_SOUND_STD_A2_M:
			return HAL_AUDIO_SIF_MN_A2;
		case ATV_SOUND_STD_EIAJ:
			return HAL_AUDIO_SIF_MN_EIAJ;
		case ATV_SOUND_STD_BG_MONO:
			return HAL_AUDIO_SIF_BG_FM;
		case ATV_SOUND_STD_A2_BG:
			return HAL_AUDIO_SIF_BG_A2;
		case ATV_SOUND_STD_NICAM_BG:
			return HAL_AUDIO_SIF_BG_NICAM;
		case ATV_SOUND_STD_DK_MONO:
			return HAL_AUDIO_SIF_DK_FM;
		case ATV_SOUND_STD_A2_DK1:
			return HAL_AUDIO_SIF_DK1_A2;
		case ATV_SOUND_STD_A2_DK2:
			return HAL_AUDIO_SIF_DK2_A2;
		case ATV_SOUND_STD_A2_DK3:
			return HAL_AUDIO_SIF_DK3_A2;
		case ATV_SOUND_STD_NICAM_DK:
			return HAL_AUDIO_SIF_DK_NICAM;
		case ATV_SOUND_STD_AM_MONO:
			return HAL_AUDIO_SIF_L_AM;
		case ATV_SOUND_STD_NICAM_L:
			return HAL_AUDIO_SIF_L_NICAM;
		case ATV_SOUND_STD_FM_MONO_NO_I:
			return HAL_AUDIO_SIF_I_FM;
		case ATV_SOUND_STD_NICAM_I:
			return HAL_AUDIO_SIF_I_NICAM;
		case ATV_SOUND_STD_UNKNOWN:
		default:
			ERROR("[%s][%s][%d] not in case...\n", __FILE__, __func__, __LINE__);
			return HAL_AUDIO_SIF_UNKNOWN;
	}
}

DTV_STATUS_T RHAL_Audio_SIF_SET_MTS_PRIORITY(HAL_SIF_MTS_PRIORITY_T Priority)
{
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(KADP_Audio_AtvSetMtsPriority((ATV_MTS_PRIORITY)Priority) < 0)
    {
        ERROR("[AudioHw_ERR][%s][%s][%d]\n", __FILE__, __func__, __LINE__);
        return NOT_OK;
    }

    return OK;
}

#if 0 //Seems no need
DTV_STATUS_T RHAL_Audio_ScanFmRadioMode(HAL_AUDIO_SIF_STANDARD_T *SoundSTD, HAL_AUDIO_SIF_CARRIER_INFO_T *SIF_CARRIER_INFO)
{
	ATV_SOUND_STD DetectSoundStd = ATV_SOUND_STD_UNKNOWN;
	ATV_Carrier_INFO Carrier_info;
    memset(&Carrier_info, 0, sizeof(ATV_Carrier_INFO));

	INFO("%s Enter.\n", __func__);

	if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

	if(KADP_Audio_AtvScanSoundStd_FMRadio(&DetectSoundStd, &Carrier_info) < 0)
	{
		ERROR("[AudioHw_ERR][%s][%s][%d]\n", __FILE__, __func__, __LINE__);
		return NOT_OK;
	}

	*SoundSTD = Audio_SoundStd_KadpToHal(DetectSoundStd);
	SIF_CARRIER_INFO->CARRIER_SHIFT_VALUE = (UINT8)Carrier_info.Carrier_shif_value;
	SIF_CARRIER_INFO->CARRIER_DEVIATION = (UINT8)Carrier_info.Carrier_deviation;
	return OK;
}
#endif

DTV_STATUS_T RHAL_Audio_ATV_VERIFY_SET_PATH(void)
{
    INFO("%s Enter.\n", __func__);

    if (KADP_Audio_ATV_VERIFY_SET_PATH() != KADP_OK)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_Audio_SetFmRadioMode(AUDIO_ATV_FM_RADIO_MODE Enable)
{
    AUDIO_ATV_FM_RADIO_MODE Mode;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if (Enable == HAL_AUDIO_ATV_FM_RADIO_SCAN)
        Mode = AUDIO_ATV_FM_RADIO_SCAN;
    else if (Enable == HAL_AUDIO_ATV_FM_RADIO_PLAY)
        Mode = AUDIO_ATV_FM_RADIO_PLAY;
    else Mode = AUDIO_ATV_FM_RADIO_STOP;

    if(KADP_Audio_SetFmRadioMode(Mode) < 0)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_GetSoundInfo(HAL_ATV_SOUND_INFO_T *SoundInfo)
{
    ATV_SOUND_INFO p_sound_info;
    INFO("%s Enter.\n", __func__);
    memset(&p_sound_info, 0, sizeof(ATV_SOUND_INFO));

    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;
    if(KADP_Audio_AtvGetSoundStd(&p_sound_info) < 0)
    {
        return NOT_OK;
    }

    SoundInfo->isNicamSystem = p_sound_info.isNicamSystem;
    SoundInfo->isPlayDigital = p_sound_info.isPlayDigital;
    SoundInfo->current_select = (HAL_ATV_SOUND_SELECT)p_sound_info.current_select;
    SoundInfo->sound_std = (HAL_AUDIO_SIF_STANDARD_T)p_sound_info.sound_std;
    SoundInfo->ana_soundmode = (HAL_ATV_SOUND_MODE)p_sound_info.ana_soundmode;
    SoundInfo->dig_soundmode = (HAL_ATV_SOUND_MODE)p_sound_info.dig_soundmode;
    SoundInfo->std_type = (HAL_ATV_MAIN_STD_TYPE_T)p_sound_info.std_type;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_SetHighDevMode(BOOLEAN bOnOff)
{
    SINT32 fm_down_6dB = 0;
    SINT32 am_down_6dB = 0;
    SINT32 am_wider_bw = 0;
    A2_BW_SEL_T  deviation_bw;
    A2_BW_SEL_T  deviation_bw_sub;

    INFO("%s Enter.\n", __func__);

    deviation_bw_sub = BANDWIDTH_HDV0_120KHZ;

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;
    if(bOnOff > 1)
    {
        ERROR("[%s] HighDevMode %d\n",__func__,bOnOff);
        return INVALID_PARAMS;
    }
    INFO("HighDev enable %d Stype %d\n", bOnOff, (SINT32)g_AudioSIFInfo.curSifType);

    if (bOnOff == TRUE)
    {
        KADP_Audio_AtvSetDevOnOff(TRUE);
        if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_KOREA_A2_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_ATSC_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV0_240KHZ;    //200Khz
        }
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_AJJA_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV1_740KHZ;    //540Khz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_IN_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV1_740KHZ;    //540Khz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_CN_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV1_480KHZ;    //384Khz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_ID_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV1_480KHZ;    //384Khz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if ( g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_BTSC_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_BTSC_BR_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_BTSC_US_SELECT)
            deviation_bw = BANDWIDTH_HDV0_370KHZ;
        else
            deviation_bw = BANDWIDTH_HDV0_120KHZ;    //100Khz
    }
    else
    {
        KADP_Audio_AtvSetDevOnOff(FALSE);
        if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_KOREA_A2_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_ATSC_SELECT)
            deviation_bw = BANDWIDTH_HDV0_240KHZ;    //200Khz
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_AJJA_SELECT)
        {
            deviation_bw     = BANDWIDTH_HDV0_370KHZ;    //100kHz
            //deviation_bw_sub = BANDWIDTH_HDV0_370KHZ;    //100kHz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_IN_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV0_370KHZ;    //100kHz
            //deviation_bw_sub = BANDWIDTH_HDV0_370KHZ;    //100kHz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_CN_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV0_370KHZ;    //100kHz
            //deviation_bw_sub = BANDWIDTH_HDV0_370KHZ;    //100kHz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if (g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_DVB_ID_SELECT)
        {
            deviation_bw = BANDWIDTH_HDV0_370KHZ;    //100kHz
            //deviation_bw_sub = BANDWIDTH_HDV0_370KHZ;    //100kHz
            fm_down_6dB = 1;
            am_down_6dB = 0;
            am_wider_bw = 0;
        }
        else if ( g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_BTSC_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_BTSC_BR_SELECT || g_AudioSIFInfo.curSifType == HAL_AUDIO_SIF_BTSC_US_SELECT)
            deviation_bw = BANDWIDTH_HDV0_370KHZ;

        else
            deviation_bw = BANDWIDTH_HDV0_120KHZ;  //50Khz
    }
    g_AudioSIFInfo.bHighDevOnOff = bOnOff;
    KADP_Audio_AtvSetDevBandWidth(deviation_bw, deviation_bw_sub);
    KADP_Audio_AtvSetFMoutDownGain(fm_down_6dB);
    //Audio_AtvSetAMoutDownGain(am_down_6dB);
    //Audio_AtvSetAMWideBW(am_wider_bw);
    return OK;

}

DTV_STATUS_T RHAL_AUDIO_SIF_SetBandSetup(HAL_AUDIO_SIF_TYPE_T eSifType, HAL_AUDIO_SIF_SOUNDSYSTEM_T sifBand)
{
    ATV_SOUND_STD_MAIN_SYSTEM atv_sound_std_main_system = ATV_SOUND_UNKNOWN_SYSTEM;

    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    INFO("[%s] SIF set band %d\n",__func__,(SINT32)sifBand);

    //if(eSifType == HAL_AUDIO_SIF_DVB_SELECT ||
    // eSifType == HAL_AUDIO_SIF_DVB_ID_SELECT ||
    // eSifType == HAL_AUDIO_SIF_DVB_IN_SELECT ||
    // eSifType == HAL_AUDIO_SIF_DVB_CN_SELECT ||
    // eSifType == HAL_AUDIO_SIF_DVB_AJJA_SELECT
    // )
    //{
    KADP_Audio_HwpSetChannelChange();
    KADP_Audio_HwpSetBandDelay();
    //}
    if(sifBand < HAL_AUDIO_SIF_SYSTEM_BG || sifBand > HAL_AUDIO_SIF_SYSTEM_MN)
    {
        ERROR("[%s] error sifBand %d\n",__func__,(SINT32)sifBand);
        return API_INVALID_PARAMS;
    }
    else
    {
          SIF_INPUT_SOURCE  rtk_sif_input_source;

          switch(g_AudioSIFInfo.sifSource)
        {
              case HAL_AUDIO_SIF_INPUT_EXTERNAL:
                  rtk_sif_input_source = SIF_FROM_SIF_ADC;
                  break;
              case HAL_AUDIO_SIF_INPUT_INTERNAL:
                  rtk_sif_input_source = SIF_FROM_IFDEMOD;
                  break;
              default:
                  ERROR("[%s] sif input %d\n",__func__,(SINT32)g_AudioSIFInfo.sifSource);
                  return API_INVALID_PARAMS;
        }
        switch(sifBand)
        {
            case  HAL_AUDIO_SIF_SYSTEM_BG:
                atv_sound_std_main_system = ATV_SOUND_BG_SYSTEM;
                break;
            case  HAL_AUDIO_SIF_SYSTEM_I:
                atv_sound_std_main_system = ATV_SOUND_I_SYSTEM;
                break;
            case  HAL_AUDIO_SIF_SYSTEM_DK:
                atv_sound_std_main_system = ATV_SOUND_DK_SYSTEM;
                break;
            case  HAL_AUDIO_SIF_SYSTEM_L:
                atv_sound_std_main_system = ATV_SOUND_L_SYSTEM;
                break;
            case  HAL_AUDIO_SIF_SYSTEM_MN:
                atv_sound_std_main_system = ATV_SOUND_MN_SYSTEM;
                break;
            case  HAL_AUDIO_SIF_SYSTEM_UNKNOWN:
                atv_sound_std_main_system = ATV_SOUND_UNKNOWN_SYSTEM;
                break;
            default:
                ERROR("[%s] not in case1... %d\n",__func__,(SINT32)sifBand);
                return API_INVALID_PARAMS;
        }

        switch (eSifType)
        {
            case  HAL_AUDIO_SIF_ATSC_SELECT:    //A2
            case  HAL_AUDIO_SIF_KOREA_A2_SELECT:
                break;
            case  HAL_AUDIO_SIF_BTSC_SELECT:
            case  HAL_AUDIO_SIF_BTSC_BR_SELECT:
            case  HAL_AUDIO_SIF_BTSC_US_SELECT:
                break;
            case  HAL_AUDIO_SIF_DVB_SELECT:
            case  HAL_AUDIO_SIF_DVB_ID_SELECT:
            case  HAL_AUDIO_SIF_DVB_IN_SELECT:
            case  HAL_AUDIO_SIF_DVB_CN_SELECT:
            case  HAL_AUDIO_SIF_DVB_AJJA_SELECT:
                INFO("DVB ASD func\n");
                //atv_sound_std_main_system = ATV_SOUND_AUTO_SYSTEM;
                break;

            case  HAL_AUDIO_SIF_TYPE_NONE:
            case  HAL_AUDIO_SIF_TYPE_MAX:
            default:
                ERROR("[%s] not in case2... %d\n",__func__,(SINT32)eSifType);
                return API_INVALID_PARAMS;

        }
        if(KADP_Audio_HwpSetAtvAudioBand(rtk_sif_input_source, atv_sound_std_main_system)== -1)
        {
            ERROR("[%s] KADP_Audio_HwpSetAtvAudioBand not success\n",__func__);
        }
        g_AudioSIFInfo.curSifBand= sifBand;
        g_AudioSIFInfo.curSifType = eSifType;
        KADP_Audio_HwpCurSifType( (ATV_AUDIO_SIF_TYPE_T) eSifType);
        return OK;
    }
}

DTV_STATUS_T RHAL_AUDIO_SIF_SetModeSetup(HAL_AUDIO_SIF_STANDARD_T sifStandard)
{
    ATV_SOUND_STD sound_std = ATV_SOUND_STD_UNKNOWN;
    ATV_SOUND_STD_MAIN_SYSTEM main_system = ATV_SOUND_UNKNOWN_SYSTEM;

    // To make sure ATV thread cannot invoke RAL_ATVSetDecoderXMute callback and cause deadlock by //AUDIO_FUNC_CALL()
    KADP_Audio_AtvPauseTvStdDetection(true);
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
    {
        KADP_Audio_AtvPauseTvStdDetection(false);
        return NOT_OK;
    }

    switch(sifStandard)
    {
      //BG
        case  HAL_AUDIO_SIF_BG_NICAM:
            sound_std = ATV_SOUND_STD_NICAM_BG;
            break;
        case  HAL_AUDIO_SIF_BG_FM:
            sound_std = ATV_SOUND_STD_BG_MONO;
            break;
        case  HAL_AUDIO_SIF_BG_A2:
            sound_std = ATV_SOUND_STD_A2_BG;
            break;
            //I
        case  HAL_AUDIO_SIF_I_NICAM:
            sound_std = ATV_SOUND_STD_NICAM_I;
            break;
        case  HAL_AUDIO_SIF_I_FM:
            sound_std = ATV_SOUND_STD_FM_MONO_NO_I;
            break;
            //DK
        case  HAL_AUDIO_SIF_DK_NICAM:
            sound_std = ATV_SOUND_STD_NICAM_DK;
            break;
        case  HAL_AUDIO_SIF_DK_FM:
            sound_std = ATV_SOUND_STD_DK_MONO;
            break;
        case  HAL_AUDIO_SIF_DK1_A2:
            sound_std = ATV_SOUND_STD_A2_DK1;
            break;
        case  HAL_AUDIO_SIF_DK2_A2:
            sound_std = ATV_SOUND_STD_A2_DK2;
            break;
        case  HAL_AUDIO_SIF_DK3_A2:
            sound_std = ATV_SOUND_STD_A2_DK3;
            break;
            //L
        case  HAL_AUDIO_SIF_L_NICAM:
            sound_std = ATV_SOUND_STD_NICAM_L;
            break;
        case  HAL_AUDIO_SIF_L_AM:
            sound_std = ATV_SOUND_STD_AM_MONO;
            break;
            //MN
        case  HAL_AUDIO_SIF_MN_A2:
            sound_std = ATV_SOUND_STD_A2_M;
            break;
        case  HAL_AUDIO_SIF_MN_BTSC:
            sound_std = ATV_SOUND_STD_BTSC;
            break;
        case  HAL_AUDIO_SIF_MN_EIAJ:
            ERROR("HAL_AUDIO_SIF_MN_EIAJ not support\n");
            //Release sem
            KADP_Audio_AtvPauseTvStdDetection(false);
            return API_INVALID_PARAMS;
        default:
            //Release sem
            KADP_Audio_AtvPauseTvStdDetection(false);
            return API_INVALID_PARAMS;
    }

    //INFO("[%s] HAL %d Drv 0x%x\n",__func__,sifStandard, sound_std);
    KADP_Audio_AtvSetSoundStd( main_system, sound_std);
    g_AudioSIFInfo.curSifStandard = sifStandard;

    //Release sem
    KADP_Audio_AtvPauseTvStdDetection(false);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_SetUserAnalogMode(HAL_AUDIO_SIF_MODE_SET_T sifAudioMode)
{
    SINT32 force_analogmode =0;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    switch(sifAudioMode)
    {
        //A2
        case  HAL_AUDIO_SIF_SET_PAL_MONO:
            KADP_Audio_AtvSetA2SoundSelect(0, 0);
            KADP_Audio_AtvSetNICAMSoundSelect(0, 0);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_STEREO:
            KADP_Audio_AtvSetA2SoundSelect(1, 0);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_DUALI:
            KADP_Audio_AtvSetA2SoundSelect(0, 0);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_DUALII:
            KADP_Audio_AtvSetA2SoundSelect(0, 1);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_DUALI_II:
            KADP_Audio_AtvSetA2SoundSelect(0, 2);
            break;
            //NICAM
        case  HAL_AUDIO_SIF_SET_PAL_NICAM_MONO:
            KADP_Audio_AtvSetNICAMSoundSelect(1, 0);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_NICAM_STEREO:
            KADP_Audio_AtvSetNICAMSoundSelect(1, 0);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_NICAM_DUALI:
            KADP_Audio_AtvSetNICAMSoundSelect(1, 0);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_NICAM_DUALII:
            KADP_Audio_AtvSetNICAMSoundSelect(1, 1);
            break;
        case  HAL_AUDIO_SIF_SET_PAL_NICAM_DUALI_II:
            KADP_Audio_AtvSetNICAMSoundSelect(1, 2);
            break;
            //A2
        case  HAL_AUDIO_SIF_SET_NTSC_A2_MONO:
            KADP_Audio_AtvSetA2SoundSelect(0, 0);
            break;
        case  HAL_AUDIO_SIF_SET_NTSC_A2_SAP:
            KADP_Audio_AtvSetA2SoundSelect(0, 1);//Lang B
            break;
        case  HAL_AUDIO_SIF_SET_NTSC_A2_STEREO:
            KADP_Audio_AtvSetA2SoundSelect(1, 0);
            break;
            //BTSC
        case  HAL_AUDIO_SIF_SET_NTSC_BTSC_MONO:
            KADP_Audio_AtvSetBTSCSoundSelect(0,0);
            break;
        case  HAL_AUDIO_SIF_SET_NTSC_BTSC_STEREO:
            KADP_Audio_AtvSetBTSCSoundSelect(1,0);
            break;
        case  HAL_AUDIO_SIF_SET_NTSC_BTSC_SAP_MONO:
            //assume SAP_1
            KADP_Audio_AtvSetBTSCSoundSelect(1,1);//
            break;
        case  HAL_AUDIO_SIF_SET_NTSC_BTSC_SAP_STEREO:
            //assume SAP_2
            KADP_Audio_AtvSetBTSCSoundSelect(1,1);
            break;

        case HAL_AUDIO_SIF_SET_PAL_MONO_FORCED:
            KADP_Audio_AtvSetA2SoundSelect(0, 0);
            KADP_Audio_AtvSetNICAMSoundSelect(0, 0);
            break;
        case HAL_AUDIO_SIF_SET_PAL_STEREO_FORCED:
            KADP_Audio_AtvSetA2SoundSelect(0, 0);
            break;
        case HAL_AUDIO_SIF_SET_PAL_NICAM_MONO_FORCED:
            KADP_Audio_AtvSetNICAMSoundSelect(0, 0);
            force_analogmode = 1;
            break;
        case HAL_AUDIO_SIF_SET_PAL_NICAM_STEREO_FORCED:
            KADP_Audio_AtvSetNICAMSoundSelect(0, 0);
            force_analogmode = 1;
            break;
        case HAL_AUDIO_SIF_SET_PAL_NICAM_DUAL_FORCED:
            KADP_Audio_AtvSetNICAMSoundSelect(0, 0);
            force_analogmode = 1;
            break;
            //-------------
        case HAL_AUDIO_SIF_SET_PAL_UNKNOWN:
        case HAL_AUDIO_SIF_SET_NTSC_A2_UNKNOWN:
        case HAL_AUDIO_SIF_SET_NTSC_BTSC_UNKNOWN:
        default:

            ERROR("[%s]Not Ready case or expected value %d\n",__func__,sifAudioMode);
            return API_INVALID_PARAMS;
    }
    INFO("[%s] Sound Sel %d\n",__func__,sifAudioMode);
    g_AudioSIFInfo.curSifModeSet = sifAudioMode;
    if(force_analogmode == 1)
    {
        KADP_Audio_AtvForceSoundSel(1);
    }
    else
    {
        KADP_Audio_AtvForceSoundSel(0);
    }
    return OK;
}

//UINT16 value has two value. Bit[7:0] is threshold high and low reg.
DTV_STATUS_T RHAL_AUDIO_SIF_SetA2ThresholdLevel(UINT16 thrLevel)
{
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    if(thrLevel > 100)
    {
        ERROR("[%s]A2 TH Not expected value %d\n",__func__,thrLevel);
        return API_INVALID_PARAMS;
    }

    KADP_Audio_HwpSIFSetA2StereoDualTH( (UINT32)thrLevel);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_SetNicamThresholdLevel(UINT16 thrLevel)
{
    UINT8  hi_th;
    UINT8  lo_th;

    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    //Bit[7:4] High threshold
    //Bit[3:0] Low threshold
    hi_th  = (thrLevel&0x00F0)>>4;
    lo_th  = thrLevel&0x000F;
    if(hi_th > 15)
    {
        ERROR("[%s]hi_th Not expected value %d\n",__func__,hi_th);
        return API_INVALID_PARAMS;
    }
    if(lo_th > 15)
    {
        ERROR("[%s]lo_th Not expected value %d\n",__func__,lo_th);
        return API_INVALID_PARAMS;
    }
    KADP_Audio_HwpSIFSetNicamTH(hi_th,lo_th);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_GetBandDetect(HAL_AUDIO_SIF_SOUNDSYSTEM_T soundSystem, UINT32 *pBandStrength)
{
    ATV_SOUND_STD_MAIN_SYSTEM atv_sound_std_main_system = ATV_SOUND_UNKNOWN_SYSTEM;
    ATV_SOUND_STD_MAIN_SYSTEM HWDetectSoundSystem = ATV_SOUND_UNKNOWN_SYSTEM;
    ATV_SOUND_STD HWDetectSoundStd = ATV_SOUND_STD_UNKNOWN;
    UINT32 pToneSNR = 0;

    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    switch(soundSystem)
    {
        case  HAL_AUDIO_SIF_SYSTEM_BG:
            atv_sound_std_main_system = ATV_SOUND_BG_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_I:
            atv_sound_std_main_system = ATV_SOUND_I_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_DK:
            atv_sound_std_main_system = ATV_SOUND_DK_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_L:
            atv_sound_std_main_system = ATV_SOUND_L_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_MN:
            atv_sound_std_main_system = ATV_SOUND_MN_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_UNKNOWN:
            atv_sound_std_main_system = ATV_SOUND_UNKNOWN_SYSTEM;
            break;
        default:
            ERROR("[%s] not in case1... %d\n",__func__,soundSystem);
            return API_INVALID_PARAMS;
    }
    KADP_Audio_HwpSIFGetMainToneSNR(atv_sound_std_main_system, &HWDetectSoundSystem, &HWDetectSoundStd, &pToneSNR);

    *pBandStrength = (UINT32)pToneSNR;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_DetectSoundSystem(HAL_AUDIO_SIF_SOUNDSYSTEM_T setSoundSystem, BOOLEAN bManualMode,
                            HAL_AUDIO_SIF_SOUNDSYSTEM_T *pDetectSoundSystem, UINT32 *pSignalQuality)
{
    ATV_SOUND_STD_MAIN_SYSTEM atv_sound_std_main_system = ATV_SOUND_UNKNOWN_SYSTEM;
    ATV_SOUND_STD_MAIN_SYSTEM HWDetectSoundSystem = ATV_SOUND_UNKNOWN_SYSTEM;
    ATV_SOUND_STD HWDetectSoundStd = ATV_SOUND_STD_UNKNOWN;
    UINT32 pToneSNR = 0;
    SIF_INPUT_SOURCE rtk_sif_input_source;

    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE)
     return NOT_OK;
    //set band internal/external
    switch(g_AudioSIFInfo.sifSource)
    {
        case HAL_AUDIO_SIF_INPUT_EXTERNAL:
            rtk_sif_input_source = SIF_FROM_SIF_ADC;
            break;
        case HAL_AUDIO_SIF_INPUT_INTERNAL:
            rtk_sif_input_source = SIF_FROM_IFDEMOD;
            break;
        default:
            ERROR("[%s] sif input %d\n",__func__, g_AudioSIFInfo.sifSource);
            return API_INVALID_PARAMS;
    }

    //detect sound system
    switch(setSoundSystem)
    {
        case  HAL_AUDIO_SIF_SYSTEM_BG:
            atv_sound_std_main_system = ATV_SOUND_BG_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_I:
            atv_sound_std_main_system = ATV_SOUND_I_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_DK:
            atv_sound_std_main_system = ATV_SOUND_DK_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_L:
            atv_sound_std_main_system = ATV_SOUND_L_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_MN:
            atv_sound_std_main_system = ATV_SOUND_MN_SYSTEM;
            break;
        case  HAL_AUDIO_SIF_SYSTEM_UNKNOWN:
            atv_sound_std_main_system = ATV_SOUND_AUTO_SYSTEM;
            break;
        default:
            ERROR("[%s] not in case1... %d\n",__func__,setSoundSystem);
            return API_INVALID_PARAMS;

    }
    //Audio_HwpSIFDetectedSoundSystem(&atv_sound_std_main_system);
    KADP_Audio_HwpSetAtvAudioBand(rtk_sif_input_source, atv_sound_std_main_system);
    KADP_Audio_HwpSIFGetMainToneSNR(atv_sound_std_main_system, &HWDetectSoundSystem, &HWDetectSoundStd, &pToneSNR);

    if(bManualMode == TRUE)
    {
        *pDetectSoundSystem = setSoundSystem;//  manual mode setSoundSystem and *pDetectSoundSystem must be the same
    }
    else
    {
        switch(HWDetectSoundSystem) //auto mode reuten HWDetectSoundSystem
        {
            case  ATV_SOUND_BG_SYSTEM:
                *pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_BG;
                break;
            case  ATV_SOUND_I_SYSTEM:
                *pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_I;
                break;
            case  ATV_SOUND_DK_SYSTEM:
                *pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_DK;
                break;
            case  ATV_SOUND_L_SYSTEM:
                *pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_L;
            break;
            case  ATV_SOUND_MN_SYSTEM:
                *pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_MN;
            break;
            case  ATV_SOUND_UNKNOWN_SYSTEM:
                *pDetectSoundSystem = HAL_AUDIO_SIF_SYSTEM_UNKNOWN;
            break;
            default:
                ERROR("[%s] not in case2... %d\n",__func__,HWDetectSoundSystem);
                return API_INVALID_PARAMS;
        }
    }

    *pSignalQuality = (UINT32)pToneSNR;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_CheckNicamDigital(HAL_AUDIO_SIF_EXISTENCE_INFO_T *pIsNicamDetect)
{
    SINT32 nicam_flag;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    KADP_Audio_HwpSIFCheckNicamDigital(&nicam_flag);
    if(nicam_flag == 1)//exist
    {
        *pIsNicamDetect = HAL_AUDIO_SIF_PRESENT;
        g_AudioSIFInfo.curSifIsNicam = HAL_AUDIO_SIF_PRESENT;
    }
    else if(nicam_flag == 2)// not exist
    {
        *pIsNicamDetect = HAL_AUDIO_SIF_ABSENT;
        g_AudioSIFInfo.curSifIsNicam = HAL_AUDIO_SIF_ABSENT;
    }
    else
    {
        g_AudioSIFInfo.curSifIsNicam = HAL_AUDIO_SIF_DETECTING_EXSISTANCE;
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_CheckAvailableSystem(HAL_AUDIO_SIF_AVAILE_STANDARD_T standard,
                                            HAL_AUDIO_SIF_EXISTENCE_INFO_T *pAvailability)
{
    ATV_SOUND_STD  sound_std = ATV_SOUND_STD_UNKNOWN;
    INFO("%s Enter.\n", __func__);// decrease print log
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    KADP_Audio_HwpSIFDetectedSoundStandard(&sound_std);

    *pAvailability = HAL_AUDIO_SIF_ABSENT;
    if(standard == HAL_AUDIO_SIF_NICAM)
    {
        if(sound_std == ATV_SOUND_STD_NICAM_BG || sound_std == ATV_SOUND_STD_NICAM_DK || sound_std == ATV_SOUND_STD_NICAM_I || sound_std == ATV_SOUND_STD_NICAM_L)
        {
            *pAvailability = HAL_AUDIO_SIF_PRESENT;
        }
    }
    else if(standard == HAL_AUDIO_SIF_A2)
    {
        if(sound_std == ATV_SOUND_STD_A2_M || sound_std == ATV_SOUND_STD_A2_BG || sound_std == ATV_SOUND_STD_A2_DK1 || sound_std == ATV_SOUND_STD_A2_DK2 || sound_std == ATV_SOUND_STD_A2_DK3)
        {
            *pAvailability = HAL_AUDIO_SIF_PRESENT;
        }
    }
    else if(standard == HAL_AUDIO_SIF_FM)
    {
        if(sound_std == ATV_SOUND_STD_AM_MONO ||
           sound_std == ATV_SOUND_STD_BG_MONO ||
           sound_std == ATV_SOUND_STD_DK_MONO ||
           sound_std == ATV_SOUND_STD_FM_MONO_NO_I ||
           sound_std == ATV_SOUND_STD_BTSC ||
           sound_std == ATV_SOUND_STD_MN_MONO)
        {
            //check BTSC: BTSC belong to FM???
            *pAvailability = HAL_AUDIO_SIF_PRESENT;
        }
    }

    else if(standard == HAL_AUDIO_SIF_DETECTING_AVALIBILITY)
    {
        ERROR("[%s] error para %d\n",__func__,standard);
        return API_INVALID_PARAMS;
    }
    else
    {
        ERROR("[%s] error para %d\n",__func__,standard);
        return API_INVALID_PARAMS;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_CheckA2DK(HAL_AUDIO_SIF_STANDARD_T standard,
                                    HAL_AUDIO_SIF_EXISTENCE_INFO_T *pAvailability)
{
    ATV_SOUND_STD sound_std = ATV_SOUND_STD_UNKNOWN;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    KADP_Audio_HwpSIFDetectedSoundStandard(&sound_std);


    g_AudioSIFInfo.curSifIsA2 = HAL_AUDIO_SIF_DETECTING_EXSISTANCE;
    switch(standard)
    {
        case  HAL_AUDIO_SIF_DK1_A2:
            if(sound_std == ATV_SOUND_STD_A2_DK1)
            {
                *pAvailability = HAL_AUDIO_SIF_PRESENT;
                g_AudioSIFInfo.curSifIsA2 = HAL_AUDIO_SIF_PRESENT;
            }
            else
            {
                *pAvailability = HAL_AUDIO_SIF_ABSENT;
                g_AudioSIFInfo.curSifIsA2 = HAL_AUDIO_SIF_ABSENT;
            }
            break;
        case  HAL_AUDIO_SIF_DK2_A2:
            if(sound_std == ATV_SOUND_STD_A2_DK2)
            {
                *pAvailability = HAL_AUDIO_SIF_PRESENT;
                g_AudioSIFInfo.curSifIsA2 = HAL_AUDIO_SIF_PRESENT;
            }
            else
            {
                *pAvailability = HAL_AUDIO_SIF_ABSENT;
                g_AudioSIFInfo.curSifIsA2 = HAL_AUDIO_SIF_ABSENT;
            }
            break;
        case  HAL_AUDIO_SIF_DK3_A2:
            if(sound_std == ATV_SOUND_STD_A2_DK3)
            {
                *pAvailability = HAL_AUDIO_SIF_PRESENT;
                g_AudioSIFInfo.curSifIsA2 = HAL_AUDIO_SIF_PRESENT;
            }
            else
            {
                *pAvailability = HAL_AUDIO_SIF_ABSENT;
                g_AudioSIFInfo.curSifIsA2 = HAL_AUDIO_SIF_ABSENT;
            }
            break;
        case  HAL_AUDIO_SIF_BG_NICAM:
        case  HAL_AUDIO_SIF_BG_FM:
        case  HAL_AUDIO_SIF_BG_A2:
        case  HAL_AUDIO_SIF_I_NICAM:
        case  HAL_AUDIO_SIF_I_FM:
        case  HAL_AUDIO_SIF_DK_NICAM:
        case  HAL_AUDIO_SIF_DK_FM:
        case  HAL_AUDIO_SIF_L_NICAM:
        case  HAL_AUDIO_SIF_L_AM:
        case  HAL_AUDIO_SIF_MN_A2:
        case  HAL_AUDIO_SIF_MN_BTSC:
        case  HAL_AUDIO_SIF_MN_EIAJ:
            ERROR("[%s] error para %d\n",__func__,standard);
            return API_INVALID_PARAMS;
        default:
            ERROR("[%s] error para %d\n",__func__,standard);
            return API_INVALID_PARAMS;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_GetA2StereoLevel(UINT16 *pLevel)
{
    UINT32  getA2Th = 0;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    KADP_Audio_HwpSIFGetA2StereoDualTH( &getA2Th);
    *pLevel = (UINT16)getA2Th;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_GetNicamThresholdLevel(UINT16 *pLevel)
{
    UINT8  hi_th = 0;
    UINT8  lo_th = 0;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    KADP_Audio_HwpSIFGetNicamTH( &hi_th, &lo_th);
    //Bit[7:4] High threshold
    //Bit[3:0] Low threshold
    *pLevel  = (hi_th<<4) | (lo_th);
    INFO("[%s] level %x hi %x lo %x\n",__func__, *pLevel, hi_th,lo_th);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_GetNicamStable(UINT16 *IsStable)
{
	SINT32 nicam_stable = 0;
	INFO("%s Enter.\n", __func__);
	if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
		return NOT_OK;

	KADP_Audio_AtvGetNicamSignalStable(&nicam_stable);
	*IsStable = nicam_stable;
	INFO("[%s] Nicam Stable = %d\n", __func__, *IsStable);
	return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_GetCurAnalogMode(HAL_AUDIO_SIF_MODE_GET_T *pSifAudioMode)
{
    ATV_SOUND_INFO sound_info;
    UINT8 isNTSC = 0;
    SINT32 nicam_stable = 0;
    //INFO("%s Enter.\n", __func__);

    memset(&sound_info, 0, sizeof(ATV_SOUND_INFO));
    //INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    KADP_Audio_AtvGetSoundStd(&sound_info);
    *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_UNKNOWN;//need add for PAL NTSC BTSC..........
    switch(sound_info.std_type)
    {
        case ATV_MAIN_STD_MONO:
            if( g_AudioSIFInfo.curSifType & (HAL_AUDIO_SIF_ATSC_SELECT | HAL_AUDIO_SIF_KOREA_A2_SELECT) )
                *pSifAudioMode = HAL_AUDIO_SIF_GET_NTSC_A2_MONO;
            if( g_AudioSIFInfo.curSifType & (HAL_AUDIO_SIF_BTSC_SELECT | HAL_AUDIO_SIF_BTSC_BR_SELECT | HAL_AUDIO_SIF_BTSC_US_SELECT) )
                *pSifAudioMode = HAL_AUDIO_SIF_GET_NTSC_BTSC_MONO;
            if( g_AudioSIFInfo.curSifType & (HAL_AUDIO_SIF_DVB_SELECT | HAL_AUDIO_SIF_DVB_ID_SELECT | HAL_AUDIO_SIF_DVB_IN_SELECT | HAL_AUDIO_SIF_DVB_CN_SELECT | HAL_AUDIO_SIF_DVB_AJJA_SELECT /*| HAL_AUDIO_SIF_DVB_AU_SELECT*/) )
                *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_MONO;
            else
            *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_MONO;
            break;
        case ATV_MAIN_STD_NICAM:
            if(sound_info.dig_soundmode  == ATV_SOUND_MODE_MONO)
            {
            	KADP_Audio_AtvGetNicamSignalStable(&nicam_stable);
                if( 1 ==  nicam_stable)
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_NICAM_MONO;
                }
                else
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_MONO;
                }
            }
            else if(sound_info.dig_soundmode  == ATV_SOUND_MODE_STEREO)
            {
                *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_NICAM_STEREO;
            }
            else if(sound_info.dig_soundmode  == ATV_SOUND_MODE_DUAL)
            {
                *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_NICAM_DUAL;
            }
            break;
        case ATV_MAIN_STD_A2:
            if((sound_info.sound_std == ATV_SOUND_STD_MN_MONO || sound_info.sound_std == ATV_SOUND_STD_BTSC || sound_info.sound_std == ATV_SOUND_STD_A2_M))
            {
                isNTSC = 1;
            }
            if(sound_info.ana_soundmode == ATV_SOUND_MODE_STEREO)
            {
                if(isNTSC == 1)
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_NTSC_A2_STEREO;
                }
                else
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_STEREO;
                }

            }
            else if(sound_info.ana_soundmode == ATV_SOUND_MODE_DUAL)
            {
                if(isNTSC == 1)
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_NTSC_A2_SAP;
                }
                else
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_DUAL;
                }
            }
            else if(sound_info.ana_soundmode == ATV_SOUND_MODE_MONO)
            {
                if(isNTSC == 1)
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_NTSC_A2_MONO;
                }
                else
                {
                    *pSifAudioMode = HAL_AUDIO_SIF_GET_PAL_MONO;
                }
            }
            break;
        case ATV_MAIN_STD_BTSC:
            if(sound_info.ana_soundmode == ATV_SOUND_MODE_MONO)
            {
                *pSifAudioMode =  HAL_AUDIO_SIF_GET_NTSC_BTSC_MONO;
            }
            else if(sound_info.ana_soundmode == ATV_SOUND_MODE_STEREO)
            {
                *pSifAudioMode = HAL_AUDIO_SIF_GET_NTSC_BTSC_STEREO;
            }
            else if(sound_info.ana_soundmode == ATV_SOUND_MODE_SAP_MONO)
            {
                *pSifAudioMode = HAL_AUDIO_SIF_GET_NTSC_BTSC_SAP_MONO;
            }
            else if(sound_info.ana_soundmode == ATV_SOUND_MODE_SAP_STEREO)
            {
                *pSifAudioMode =  HAL_AUDIO_SIF_GET_NTSC_BTSC_SAP_STEREO;
            }

            break;
        case ATV_MAIN_STD_UNKNOW:
        default:
            break;
    }
    g_AudioSIFInfo.curSifModeGet = *pSifAudioMode;
    if(sound_info.std_type == ATV_MAIN_STD_NICAM)
    {
        AUDIO_VERBOSE("[%s] Hal_mode %d Ni_drv_mod %d\n", __func__, (SINT32)*pSifAudioMode, (SINT32)sound_info.dig_soundmode);
    }
    else
    {
        AUDIO_VERBOSE("[%s] std_type %d Hal_mode %d drv_mod %d\n", __func__, sound_info.std_type, (INT32)*pSifAudioMode, (INT32)sound_info.ana_soundmode);
    }

    return OK;
}

BOOLEAN RHAL_AUDIO_SIF_IsSIFExist(void)
{
    ATV_SOUND_STD_MAIN_SYSTEM atv_sound_std_main_system = ATV_SOUND_UNKNOWN_SYSTEM;
    ATV_SOUND_STD_MAIN_SYSTEM HWDetectSoundSystem = ATV_SOUND_UNKNOWN_SYSTEM;
    ATV_SOUND_STD HWDetectSoundStd = ATV_SOUND_STD_UNKNOWN;
    UINT32 ToneSNR = 0;
    UINT32 passSNR = 0x1500;//dB

    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    for(atv_sound_std_main_system=ATV_SOUND_DK_SYSTEM; atv_sound_std_main_system<=ATV_SOUND_L_SYSTEM;)
    {
        KADP_Audio_HwpSIFGetMainToneSNR(atv_sound_std_main_system, &HWDetectSoundSystem, &HWDetectSoundStd, &ToneSNR);
        atv_sound_std_main_system =  (ATV_SOUND_STD_MAIN_SYSTEM)((SINT16)atv_sound_std_main_system+1);
        if(ToneSNR > passSNR)
        {
            INFO("[%s] tone %d pass %d\n", __func__, ToneSNR, passSNR);
            return TRUE;
        }

    }
    INFO("[%s] tone %d pass %d\n", __func__, ToneSNR, passSNR);
    return FALSE;
}

DTV_STATUS_T RHAL_AUDIO_SIF_SetAudioEQMode(BOOLEAN bOnOff)
{
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    INFO("[%s] This function does not use now.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_GetSoundStandard(HAL_AUDIO_SIF_STANDARD_T *standard)
{
    UINT32 ret = 0;
    ATV_SOUND_INFO p_sound_info;
    INFO("%s Enter.\n", __func__);
    memset(&p_sound_info, 0, sizeof(ATV_SOUND_INFO));
    ret = KADP_Audio_AtvGetSoundStd(&p_sound_info);
    if(ret != 0)
        return NOT_OK;

    switch(p_sound_info.sound_std)
    {
            //MN
        case  ATV_SOUND_STD_MN_MONO:
            *standard = HAL_AUDIO_SIF_MN_FM;
            break;
        case  ATV_SOUND_STD_BTSC:
            *standard = HAL_AUDIO_SIF_MN_BTSC;
            break;
        case  ATV_SOUND_STD_A2_M:
            *standard = HAL_AUDIO_SIF_MN_A2;
            break;
        case  ATV_SOUND_STD_EIAJ:
            *standard = HAL_AUDIO_SIF_MN_EIAJ;
            break;
            //BG
        case  ATV_SOUND_STD_BG_MONO:
            *standard = HAL_AUDIO_SIF_BG_FM;
            break;
        case  ATV_SOUND_STD_A2_BG:
            *standard = HAL_AUDIO_SIF_BG_A2;
            break;
        case  ATV_SOUND_STD_NICAM_BG:
            *standard = HAL_AUDIO_SIF_BG_NICAM;
            break;
            //DK
        case  ATV_SOUND_STD_DK_MONO:
            *standard = HAL_AUDIO_SIF_DK_FM;
            break;
        case  ATV_SOUND_STD_A2_DK1:
            *standard = HAL_AUDIO_SIF_DK1_A2;
            break;
        case  ATV_SOUND_STD_A2_DK2:
            *standard = HAL_AUDIO_SIF_DK2_A2;
            break;
        case  ATV_SOUND_STD_A2_DK3:
            *standard = HAL_AUDIO_SIF_DK3_A2;
            break;
        case  ATV_SOUND_STD_NICAM_DK:
            *standard = HAL_AUDIO_SIF_DK_NICAM;
            break;
            //L
        case  ATV_SOUND_STD_AM_MONO:
            *standard = HAL_AUDIO_SIF_L_AM;
            break;
        case  ATV_SOUND_STD_NICAM_L:
            *standard = HAL_AUDIO_SIF_L_NICAM;
            break;
            //I
        case  ATV_SOUND_STD_FM_MONO_NO_I:
            *standard = HAL_AUDIO_SIF_I_FM;
            break;
        case  ATV_SOUND_STD_NICAM_I:
            *standard = HAL_AUDIO_SIF_I_NICAM;
            break;
        case  ATV_SOUND_STD_UNKNOWN:
            *standard = HAL_AUDIO_SIF_UNKNOWN;
            break;
        default:
            ERROR("[%s] not in case... %d\n",__func__, *standard);
            return API_INVALID_PARAMS;
    }

    return OK;

}

DTV_STATUS_T RHAL_AUDIO_SIF_DetectComplete(SINT32 *isOff)
{
    UINT32 ret = 0;
    INFO("%s Enter.\n", __func__);
    ret = KADP_Audio_AtvGetCurrentDetectUpdate(isOff);
    if(ret != 0)
        return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SIF_SetOverDeviation(HAL_SIF_OVER_DEVIATION_E overDeviation)
{
	UINT32 ret = 0;
    INFO("%s Enter. overDeviation %d, gUSER_CONFIG %d\n", __func__, overDeviation, gUSER_CONFIG);
	switch(overDeviation)
	{
		case HAL_SIF_OVER_DEVIATION_50K:
			KADP_Audio_AtvSetDevOnOff(FALSE);
			KADP_Audio_AtvSetDevBandWidth(BANDWIDTH_HDV0_120KHZ, BANDWIDTH_HDV0_120KHZ);
			break;
		case HAL_SIF_OVER_DEVIATION_100K:
			KADP_Audio_AtvSetDevOnOff(FALSE);
			KADP_Audio_AtvSetDevBandWidth(BANDWIDTH_HDV0_240KHZ, BANDWIDTH_HDV0_120KHZ);
			break;
		case HAL_SIF_OVER_DEVIATION_150K:
			if (gUSER_CONFIG != AUDIO_USER_CONFIG_TV001)
			{
				KADP_Audio_AtvSetDevOnOff(FALSE);
				KADP_Audio_AtvSetDevBandWidth(BANDWIDTH_HDV0_370KHZ, BANDWIDTH_HDV0_120KHZ);
			}
			break;
		case HAL_SIF_OVER_DEVIATION_200K:
			KADP_Audio_AtvSetDevOnOff(TRUE);
			KADP_Audio_AtvSetDevBandWidth(BANDWIDTH_HDV1_240KHZ, BANDWIDTH_HDV0_120KHZ);
			break;
		case HAL_SIF_OVER_DEVIATION_384K:
			KADP_Audio_AtvSetDevOnOff(TRUE);
			KADP_Audio_AtvSetDevBandWidth(BANDWIDTH_HDV1_340KHZ, BANDWIDTH_HDV0_120KHZ);
			break;
		case HAL_SIF_OVER_DEVIATION_462K:
			if (gUSER_CONFIG != AUDIO_USER_CONFIG_TV001)
			{
				KADP_Audio_AtvSetDevOnOff(TRUE);
				KADP_Audio_AtvSetDevBandWidth(BANDWIDTH_HDV1_480KHZ, BANDWIDTH_HDV0_120KHZ);
			}
			break;
		case HAL_SIF_OVER_DEVIATION_540K:
			KADP_Audio_AtvSetDevOnOff(TRUE);
			KADP_Audio_AtvSetDevBandWidth(BANDWIDTH_HDV1_740KHZ, BANDWIDTH_HDV0_120KHZ);
			break;
		case HAL_SIF_OVER_DEVIATION_BUTT:
		default:
			break;
	}
	if(ret!= 0)
		return NOT_OK;
	return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_Start(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_AENC_ENCODING_FORMAT_T audioType)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_Stop(HAL_AUDIO_AENC_INDEX_T aencIndex)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_RegCallback(HAL_AUDIO_AENC_INDEX_T aencIndex, pfnAENCDataHandling pfnCallBack)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_SetInfo(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_AENC_INFO_T info)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_GetInfo(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_AENC_INFO_T *pInfo)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_SetVolume(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_VOLUME_T volume)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_CopyData(HAL_AUDIO_AENC_INDEX_T aencIndex, UINT8 *pDest, UINT8 *pBufAddr, UINT32 datasize, UINT8 *pRStart, UINT8 *pREnd)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AENC_ReleaseData(HAL_AUDIO_AENC_INDEX_T aencIndex, UINT8 *pBufAddr, UINT32 datasize)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_PCM_SetVolume(HAL_AUDIO_PCM_INPUT_T apcmIndex, HAL_AUDIO_VOLUME_T volume)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    SINT32 dsp_gain;
    UINT32 i;

    AUDIO_INFO("%s capture %d %x\n", __FUNCTION__, apcmIndex, volume.mainVol);

    if(!RangeCheck(apcmIndex, 0, HAL_AUDIO_PCM_INPUT_MAX)) return NOT_OK;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if((apcmIndex != HAL_AUDIO_PCM_I2S) && (apcmIndex != HAL_AUDIO_PCM_SB_PCM))
    {
        AUDIO_ERROR("error capture source %x\n",  apcmIndex);
        return NOT_OK;
    }

    dsp_gain = Volume_to_DSPGain(volume);

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_VOLUME;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = ENUM_DEVICE_PCM_CAPTURE;
    audioConfig.value[2] = FALSE;
    audioConfig.value[3] = 0x03;
    for(i = 0; i < 2; i++)
    {
        audioConfig.value[4+i] = dsp_gain;
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_PCM_SetVolume_ByPin(HAL_AUDIO_PCM_INPUT_T apcmIndex, HAL_AUDIO_VOLUME_T volume, HAL_AUDIO_BT_OUT_PIN pinID)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    SINT32 dsp_gain;
    UINT32 i;

    INFO("%s capture %d %x pin %d\n", __func__, apcmIndex, volume.mainVol, pinID);

    if(!RangeCheck(apcmIndex, 0, HAL_AUDIO_PCM_INPUT_MAX)) return NOT_OK;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if((apcmIndex != HAL_AUDIO_PCM_I2S) && (apcmIndex != HAL_AUDIO_PCM_SB_PCM))
    {
        ERROR("error capture source %x\n",  apcmIndex);
        return NOT_OK;
    }

    if((pinID != HAL_AUDIO_BT_PCM_OUT) && (pinID != HAL_AUDIO_BT_PCM_OUT1) && (pinID != HAL_AUDIO_BT_PCM_OUT2) && (pinID != HAL_AUDIO_BT_PCM_OUT3))
    {
        ERROR("%s  error pinID  %d != %d or %d or %d or %d\n", __func__, pinID, PCM_OUT_RTK, PCM_OUT1, PCM_OUT2, PCM_OUT3);
        return NOT_OK;
    }

    dsp_gain = Volume_to_DSPGain(volume);
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_VOLUME;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = ENUM_DEVICE_PCM_CAPTURE;
    audioConfig.value[2] = (int)pinID;
    audioConfig.value[3] = 0x03;
    for(i = 0; i < 2; i++)
    {
        audioConfig.value[4+i] = dsp_gain;
    }

    if (SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_PCM_SetMute_ByPin(BOOLEAN bOnOff, HAL_AUDIO_BT_OUT_PIN pinID)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 i;

    INFO("%s capture %s pin %d\n", __func__, bOnOff ? "Mute" : "Unmute", pinID);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if((pinID != HAL_AUDIO_BT_PCM_OUT) && (pinID != HAL_AUDIO_BT_PCM_OUT1) && (pinID != HAL_AUDIO_BT_PCM_OUT2) && (pinID != HAL_AUDIO_BT_PCM_OUT3))
    {
        ERROR("%s  error pinID  %d != %d or %d or %d or %d\n", __func__, pinID, PCM_OUT_RTK, PCM_OUT1, PCM_OUT2, PCM_OUT3);
        return NOT_OK;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_MUTE;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = ENUM_DEVICE_PCM_CAPTURE;
    audioConfig.value[2] = (int)pinID;
    audioConfig.value[3] = 0x03;
    for(i = 0; i < 2; i++)
    {
        audioConfig.value[4+i] = bOnOff;
    }

    if (SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetSPKOutput(UINT8 i2sNumber, HAL_AUDIO_SAMPLING_FREQ_T samplingFreq)
{
    INFO("%s Enter.\n", __func__);
    return OK;
}

void ConvertHALVolume2Decimal(HAL_AUDIO_VOLUME_T halVol, SINT32* mainVolume, SINT32* fineVolume)
{
    SINT32 mainVol, fineVol;

    mainVol= (SINT32)halVol.mainVol - (SINT32)0x7F;
    fineVol = (SINT32)halVol.fineVol * 625;// 1/16 = 0.0625
    if( (mainVol < 0) && ( fineVol != 0 ))
    {
       mainVol = mainVol + 1;
       fineVol = 10000 - (fineVol);
    }
    *mainVolume = mainVol;
    *fineVolume = fineVol;
}

DTV_STATUS_T RHAL_AUDIO_AndroidDolbyDynamicRange(UINT32 bMode)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AndroidHEAACDynamicRange(UINT32 bMode)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AndroidMPEGAudioLevel(UINT32 bValue)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AndroidHEAACAudioLevel(UINT32 bValue)
{
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetDigitalInfo(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_DIGITAL_INFO_T *pAudioDigitalInfo)
{
    HAL_AUDIO_RESOURCE_T curResourceId = adec2res(adecIndex);
    HAL_AUDIO_ES_INFO_T AudioESInfo;
    UINT16 sampleRate;
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;
    if(pAudioDigitalInfo == NULL)
        return NOT_OK;

    memset(&AudioESInfo, 0, sizeof(HAL_AUDIO_ES_INFO_T));

    if(AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0) && adecIndex == Aud_mainDecIndex) {
        RAL_AUDIO_GetESInfo(-1, &AudioESInfo, &sampleRate);
    } else {
        RAL_AUDIO_GetESInfo(adecIndex, &AudioESInfo, &sampleRate);
    }

    pAudioDigitalInfo->adecFormat = AudioESInfo.adecFormat;
    pAudioDigitalInfo->audioMode  = AudioESInfo.audioMode;
    pAudioDigitalInfo->sampleRate = sampleRate;
    pAudioDigitalInfo->EAC3       = AudioESInfo.ac3ESInfo.EAC3;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DBXTV_SET_TABLE(UINT32* table, UINT32 size)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 ret = 0;
    INFO("%s Enter.\n", __func__);

    if (LITE_HAL_INIT_OK() != TRUE){
    	ERROR("[%s %d] Audio is not initialized\n",__func__,__LINE__);
    	return NOT_OK;
    }

    if (dbxTableAddr == NULL) {
        dbxTablePhyAddr = rtkaudio_alloc_uncached(size, &dbxTableAddr);
        if (dbxTableAddr == NULL) {
        	ERROR("[%s %d] rtkaudio_alloc_uncached dbx table fail\n",__func__,__LINE__);
        	return NOT_OK;
        }
    }

    memcpy(dbxTableAddr, table, size);

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_DBX_TAB_CONFIG;
    audioConfig.value[0] = dbxTablePhyAddr;
    audioConfig.value[1] = size;

    ret = KADP_AUDIO_AudioConfig(&audioConfig);

    if(ret != 0){
    	ERROR("[%s %d] KADP_AUDIO_DBXTV_SetTable fail\n",__func__,__LINE__);
    	return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DBXTV_SET_SOUND_EFFECT(HAL_AUDIO_DBXTV_PARAM_TYPE paramType, UINT8 mode)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 ret = 0;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE){
        ERROR("[%s %d] Audio is not initialized\n",__func__,__LINE__);
        return NOT_OK;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id  = AUDIO_CONFIG_CMD_DBX_CONFIG;
    audioConfig.value[0] = paramType;
    audioConfig.value[1] = mode;

    ret = KADP_AUDIO_AudioConfig(&audioConfig);
    if(ret != 0){
        ERROR("[error] [%s %d] KADP_AUDIO_DBXTV_SetSoundEffect fail\n",__func__,__LINE__);
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_DBXTV_DEBUG_CMD(UINT32 cmd, UINT32* param, UINT32 paramSize)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    UINT32 ret = 0;
    INFO("%s Enter.\n", __func__);

    if(LITE_HAL_INIT_OK() != TRUE){
        ERROR("[%s %d] Audio is not initialized\n",__func__,__LINE__);
        return NOT_OK;
    }

    if (paramSize <= 0 || paramSize > (sizeof(audioConfig.value)-4)) { //audioConfig.value[1]~value[14]
        ERROR("[%s %d] paramSize <= 0\n",__func__,__LINE__);
        return NOT_OK;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_DBX_DEBUG;
    audioConfig.value[0] = cmd;
    memcpy(&audioConfig.value[1], param, paramSize);

    ret = KADP_AUDIO_AudioConfig(&audioConfig);
    if(ret != 0){
        ERROR("[error] [%s %d] KADP_AUDIO_AudioConfig fail\n",__func__,__LINE__);
        return NOT_OK;
    }

    memcpy(param, &audioConfig.value[1], paramSize);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SwpSetSRS_TrusurroundHD(BOOLEAN para, HAL_APP_TSXT_CFG* pTSXT)
{
    AUDIO_RPC_TSXT info;
    INFO("%s Enter.\n", __func__);
    if(NULL == pTSXT) return NOT_OK;
    if(pTSXT->ModeType < 0 || pTSXT->ModeType > 11) return NOT_OK;
    if(pTSXT->TruBassGain < 0 || pTSXT->TruBassGain > 100) return NOT_OK;
    if(pTSXT->TruBassSize < 0 || pTSXT->TruBassSize > 7) return NOT_OK;
    if(pTSXT->FocusGain < 0 || pTSXT->FocusGain > 100) return NOT_OK;
    if(pTSXT->InputGain < 0 || pTSXT->InputGain > 100) return NOT_OK;
    if(pTSXT->IsTSXTEnable != 0 && pTSXT->IsTSXTEnable !=1) return NOT_OK;
    if(pTSXT->IsHeadphoneMode != 0 && pTSXT->IsHeadphoneMode !=1) return NOT_OK;
    if(pTSXT->IsTruBassEnable != 0 && pTSXT->IsTruBassEnable !=1) return NOT_OK;
    if(pTSXT->IsFocusEnable != 0 && pTSXT->IsFocusEnable !=1) return NOT_OK;
    if(pTSXT->DefinitionGain < 0 || pTSXT->DefinitionGain > 100) return NOT_OK;
    if(pTSXT->IsDefEnable != 0 && pTSXT->IsDefEnable !=1) return NOT_OK;
    //#ifdef ENABLE_SRS_SURROUND // gene modify => default enable it

    info.instanceID = -1; // do not care
    info.pp_Tsxt_Enable = para;

    info.ModeType = (UINT8 ) (pTSXT->ModeType);
    info.TruBassGain = (UINT8 ) pTSXT->TruBassGain;
    info.TruBassSize =(UINT8 )  pTSXT->TruBassSize;
    info.FocusGain = (UINT8 ) pTSXT->FocusGain;
    info.InputGain = (UINT8 ) pTSXT->InputGain;
    info.IsTSXTEnable = pTSXT->IsTSXTEnable;
    info.IsHeadphoneMode = (UINT8 ) pTSXT->IsHeadphoneMode;
    info.IsTruBassEnable = (UINT8 ) pTSXT->IsTruBassEnable;
    info.IsFocusEnable = (UINT8 ) pTSXT->IsFocusEnable;
    info.DefinitionGain = (UINT8 ) pTSXT->DefinitionGain;
    info.IsDefEnable = (UINT8 ) pTSXT->IsDefEnable;

    if (KADP_AUDIO_SwpSetSRS_TrusurroundHD(&info) != KADP_OK)
    {
        ERROR("[%s,%d] failed\n", __func__, __LINE__);
        return NOT_OK;
    }
	return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetInputVolume(HAL_AUDIO_INPUT_SOURCE_T eSource, SINT32 sVol)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    SINT32 volume = 0, ch = 0;

    INFO("%s %d\n", __func__, sVol);

    if(HAL_AUDIO_MIC_IN == eSource)
    {
        volume = AIN_VOL_0DB + (sVol - 30);
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_AI_VOLUME;
        audioConfig.value[0] = TRUE;
        audioConfig.value[1] = ENUM_AI_PB_PATH;

        for(ch=0;ch<8;ch++)
        {
            audioConfig.value[2 + ch] = volume;
        }

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set ai volume failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }
    else if(HAL_AUDIO_OTHER_IN == eSource)
    {
        volume = ENUM_AUDIO_DVOL_K0p0DB + (sVol - 30);
        memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
        audioConfig.msg_id = AUDIO_CONFIG_CMD_AO_DEC_VOLUME;
        audioConfig.value[0] = volume;

        if (SendRPC_AudioConfig(&audioConfig) != S_OK)
        {
            ERROR("[%s,%d] set ai volume failed\n", __func__, __LINE__);
            return NOT_OK;
        }
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_setTrackMode(HAL_TRACK_MODE eMode)
{
    AUDIO_AO_CHANNEL_OUT_SWAP mode;

    INFO("%s eMode %d\n", __func__,eMode);

    switch (eMode)
    {
        case  HAL_AUDIO_AO_CHANNEL_OUT_STEREO:
            mode = AUDIO_AO_CHANNEL_OUT_STEREO;
            break;
        case HAL_AUDIO_AO_CHANNEL_OUT_LR_MIXED:
            mode = AUDIO_AO_CHANNEL_OUT_LR_MIXED;
            break;
        case HAL_AUDIO_AO_CHANNEL_OUT_LR_SWAP:
            mode = AUDIO_AO_CHANNEL_OUT_LR_SWAP;
            break;
        case HAL_AUDIO_AO_CHANNEL_OUT_R_TO_L:
            mode = AUDIO_AO_CHANNEL_OUT_R_TO_L;
            break;
        case HAL_AUDIO_AO_CHANNEL_OUT_L_TO_R:
            mode = AUDIO_AO_CHANNEL_OUT_L_TO_R;
            break;
        default:
            break;
    }

    if(KADP_AUDIO_SetAudioOptChannelSwap(mode) != KADP_OK)
    {
        return NOT_OK;
    }

    return OK;
}

AUDIO_CONFIG_CMD_MSG ConvertConfigType(HAL_AUDIO_CONFIG_CMD_MSG msgID)
{
    AUDIO_CONFIG_CMD_MSG type;

    switch (msgID)
    {
        case HAL_AUDIO_CONFIG_CMD_SPEAKER:
            type = AUDIO_CONFIG_CMD_SPEAKER;
            break;
        case HAL_AUDIO_CONFIG_CMD_AGC:
            type = AUDIO_CONFIG_CMD_AGC;
            break;
        case HAL_AUDIO_CONFIG_CMD_SPDIF:
            type = AUDIO_CONFIG_CMD_SPDIF;
            break;
        case HAL_AUDIO_CONFIG_CMD_VOLUME:
            type = AUDIO_CONFIG_CMD_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_MUTE:
            type = AUDIO_CONFIG_CMD_MUTE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_MIC:
            type = AUDIO_CONFIG_CMD_AO_MIC;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_AUX:
            type = AUDIO_CONFIG_CMD_AO_AUX;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_ECHO_REVERB:
            type = AUDIO_CONFIG_CMD_AO_ECHO_REVERB;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_MIC1_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_MIC1_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_MIC2_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_MIC2_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_DEC_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_AUX_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_AUX_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_DAC_SWITCH:
            type = AUDIO_CONFIG_CMD_DAC_SWITCH;
            break;
        case HAL_AUDIO_CONFIG_CMD_DD_DUALMONO:
            type = AUDIO_CONFIG_CMD_DD_DUALMONO;
            break;
        case HAL_AUDIO_CONFIG_CMD_DD_SCALE:
            type = AUDIO_CONFIG_CMD_DD_SCALE;
            break;
        case HAL_AUDIO_CONFIG_CMD_DD_DOWNMIXMODE:
            type = AUDIO_CONFIG_CMD_DD_DOWNMIXMODE;
            break;
        case HAL_AUDIO_CONFIG_CMD_DD_COMP:
            type = AUDIO_CONFIG_CMD_DD_COMP;
            break;
        case HAL_AUDIO_CONFIG_CMD_DD_LFE:
            type = AUDIO_CONFIG_CMD_DD_LFE;
            break;
        case HAL_AUDIO_CONFIG_CMD_DD_STEREOMIX:
            type = AUDIO_CONFIG_CMD_DD_STEREOMIX;
            break;
        case HAL_AUDIO_CONFIG_CMD_DIGITAL_OUT:
            type = AUDIO_CONFIG_CMD_DIGITAL_OUT;
            break;
        case HAL_AUDIO_CONFIG_CMD_EXCLUSIVE:
            type = AUDIO_CONFIG_CMD_EXCLUSIVE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AC3_SPDIF_RAW:
            type = AUDIO_CONFIG_CMD_AC3_SPDIF_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_AC3_HDMI_RAW:
            type = AUDIO_CONFIG_CMD_AC3_HDMI_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_DTS_SPDIF_RAW:
            type = AUDIO_CONFIG_CMD_DTS_SPDIF_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_DTS_HDMI_RAW:
            type = AUDIO_CONFIG_CMD_DTS_HDMI_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_MPG_SPDIF_RAW:
            type = AUDIO_CONFIG_CMD_MPG_SPDIF_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_MPG_HDMI_RAW:
            type = AUDIO_CONFIG_CMD_MPG_HDMI_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_AAC_SPDIF_RAW:
            type = AUDIO_CONFIG_CMD_AAC_SPDIF_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_AAC_HDMI_RAW:
            type = AUDIO_CONFIG_CMD_AAC_HDMI_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_MLP_HDMI_RAW:
            type = AUDIO_CONFIG_CMD_MLP_HDMI_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_DDP_HDMI_RAW:
            type = AUDIO_CONFIG_CMD_DDP_HDMI_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_HDMI_CHANNEL_OUT:
            type = AUDIO_CONFIG_CMD_HDMI_CHANNEL_OUT;
            break;
        case HAL_AUDIO_CONFIG_CMD_FORCE_CHANNEL_CODEC:
            type = AUDIO_CONFIG_CMD_FORCE_CHANNEL_CODEC;
            break;
        case HAL_AUDIO_CONFIG_CMD_MLP_SPDIF_RAW:
            type = AUDIO_CONFIG_CMD_MLP_SPDIF_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_DDP_SPDIF_RAW:
            type = AUDIO_CONFIG_CMD_DDP_SPDIF_RAW;
            break;
        case HAL_AUDIO_CONFIG_CMD_MAX_OUTPUT_SAMPLERATE:
            type = AUDIO_CONFIG_CMD_MAX_OUTPUT_SAMPLERATE;
            break;
        case HAL_AUDIO_CONFIG_CMD_USB_DEVICE_SUPPORT_SAMPLERATE:
            type = AUDIO_CONFIG_CMD_USB_DEVICE_SUPPORT_SAMPLERATE;
            break;
        case HAL_AUDIO_CONFIG_CMD_KARAOKE_MODE:
            type = AUDIO_CONFIG_CMD_KARAOKE_MODE;
            break;
        case HAL_AUDIO_CONFIG_CMD_BRAZIL_LATM_AAC:
            type = AUDIO_CONFIG_CMD_BRAZIL_LATM_AAC;
            break;
        case HAL_AUDIO_CONFIG_CMD_DRC_PERCENT:
            type = AUDIO_CONFIG_CMD_DRC_PERCENT;
            break;
        case HAL_AUDIO_CONFIG_CMD_LICENSE_MODE:
            type = AUDIO_CONFIG_CMD_LICENSE_MODE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AUDIO_TYPE_PRIORITY:
            type = AUDIO_CONFIG_CMD_AUDIO_TYPE_PRIORITY;
            break;
        case HAL_AUDIO_CONFIG_CMD_SPDIF_OUTPUT_SWITCH:
            type = AUDIO_CONFIG_CMD_SPDIF_OUTPUT_SWITCH;
            break;
        case HAL_AUDIO_CONFIG_CMD_CUSTOMIZE_REQUEST:
            type = AUDIO_CONFIG_CMD_CUSTOMIZE_REQUEST;
            break;
        case HAL_AUDIO_CONFIG_CMD_DEC_PLAYBACK_RATE:
            type = AUDIO_CONFIG_CMD_DEC_PLAYBACK_RATE;
            break;
        case HAL_AUDIO_CONFIG_CMD_SAMPLERATE_TOLERANCE:
            type = AUDIO_CONFIG_CMD_SAMPLERATE_TOLERANCE;
            break;
        case HAL_AUDIO_CONFIG_CMD_KARAOKE_GENDER_CHANGE:
            type = AUDIO_CONFIG_CMD_KARAOKE_GENDER_CHANGE;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_DEC_DELAY_RP:
            type = AUDIO_CONFIG_CMD_SET_DEC_DELAY_RP;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_OUTPUT_CONFIG:
            type = AUDIO_CONFIG_CMD_AO_OUTPUT_CONFIG;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_AI_VOLUME:
            type = AUDIO_CONFIG_CMD_SET_AI_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_MUTE:
            type = AUDIO_CONFIG_CMD_AO_DEC_MUTE;
            break;
        case HAL_AUDIO_CONFIG_TFAGC_DEBUG_PRINT:
            type = AUDIO_CONFIG_TFAGC_DEBUG_PRINT;
            break;
        case HAL_AUDIO_CONFIG_ASF_DEBUG_PRINT:
            type = AUDIO_CONFIG_ASF_DEBUG_PRINT;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_OUTPUT_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_OUTPUT_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_KARAOKE_MODE_FORTV_PROJECT:
            type = AUDIO_CONFIG_CMD_KARAOKE_MODE_FORTV_PROJECT;
            break;
        case HAL_AUDIO_CONFIG_CMD_AVOUT_FROM_HW:
            type = AUDIO_CONFIG_CMD_AVOUT_FROM_HW;
            break;
        case HAL_AUDIO_CONFIG_CMD_CONFIG_AIO_AIO1:
            type = AUDIO_CONFIG_CMD_CONFIG_AIO_AIO1;
            break;
        case HAL_AUDIO_CONFIG_CMD_CONFIG_AIO_AIO2:
            type = AUDIO_CONFIG_CMD_CONFIG_AIO_AIO2;
            break;
        case HAL_AUDIO_CONFIG_CMD_CONFIG_AIO_HEADPHONE:
            type = AUDIO_CONFIG_CMD_CONFIG_AIO_HEADPHONE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_MUTE:
            type = AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_MUTE;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_SPDIF_CS_INFO:
            type = AUDIO_CONFIG_CMD_SET_SPDIF_CS_INFO;
            break;
        case HAL_AUDIO_CONFIG_CMD_HDMI_ARC:
            type = AUDIO_CONFIG_CMD_HDMI_ARC;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_PB_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_PB_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_PB_MUTE:
            type = AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_PB_MUTE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_TS_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_TS_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_TS_MUTE:
            type = AUDIO_CONFIG_CMD_AO_DEC_MULTI_CH_TS_MUTE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_FLASH_PB_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_FLASH_PB_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_FLASH_PB_MUTE:
            type = AUDIO_CONFIG_CMD_AO_FLASH_PB_MUTE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_FLASH_TS_VOLUME:
            type = AUDIO_CONFIG_CMD_AO_FLASH_TS_VOLUME;
            break;
        case HAL_AUDIO_CONFIG_CMD_AO_FLASH_TS_MUTE:
            type = AUDIO_CONFIG_CMD_AO_FLASH_TS_MUTE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AI_CEI_ENABLE:
            type = AUDIO_CONFIG_CMD_AI_CEI_ENABLE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AI_CEI_SETMODE:
            type = AUDIO_CONFIG_CMD_AI_CEI_SETMODE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AD_DESCRIPTOR:
            type = AUDIO_CONFIG_CMD_AD_DESCRIPTOR;
            break;
        case HAL_AUDIO_CONFIG_CMD_DEFAULT_DIALNORM:
            type = AUDIO_CONFIG_CMD_DEFAULT_DIALNORM;
            break;
        case HAL_AUDIO_CONFIG_CMD_AI_CEI_SET_PARAMETER:
            type = AUDIO_CONFIG_CMD_AI_CEI_SET_PARAMETER;
            break;
        case HAL_AUDIO_CONFIG_CMD_AI_CEI_GET_PARAMETER:
            type = AUDIO_CONFIG_CMD_AI_CEI_GET_PARAMETER;
            break;
        case HAL_AUDIO_CONFIG_CMD_AI_TRANSFER_MATRIX:
            type = AUDIO_CONFIG_CMD_AI_TRANSFER_MATRIX;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_VOLUME:
            type = AUDIO_CONFIG_CMD_SET_VOLUME;
            break;
        /*case 79:
            type = 79;
            break;*/
        case HAL_AUDIO_CONFIG_CMD_SET_MUTE:
            type = AUDIO_CONFIG_CMD_SET_MUTE;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_DELAY:
            type = AUDIO_CONFIG_CMD_SET_DELAY;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_SCART_ON:
            type = AUDIO_CONFIG_CMD_SET_SCART_ON;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_NO_DELAY_PARAM:
            type = AUDIO_CONFIG_CMD_SET_NO_DELAY_PARAM;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_SPDIF_CS_ALL:
            type = AUDIO_CONFIG_CMD_SET_SPDIF_CS_ALL;
            break;
        case HAL_AUDIO_CONFIG_CMD_ARC:
            type = AUDIO_CONFIG_CMD_ARC;
            break;
        case HAL_AUDIO_CONFIG_CMD_NTFY_HAL_HDMI_FMT:
            type = AUDIO_CONFIG_CMD_NTFY_HAL_HDMI_FMT;
            break;
        case HAL_AUDIO_CONFIG_CMD_AANDROID_DOLBY_DYN_RANGE:
            type = AUDIO_CONFIG_CMD_AANDROID_DOLBY_DYN_RANGE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AANDROID_HEAAC_DYN_RANGE:
            type = AUDIO_CONFIG_CMD_AANDROID_HEAAC_DYN_RANGE;
            break;
        case HAL_AUDIO_CONFIG_CMD_AANDROID_MPEG_AUD_LEVEL:
            type = AUDIO_CONFIG_CMD_AANDROID_MPEG_AUD_LEVEL;
            break;
        case HAL_AUDIO_CONFIG_CMD_AANDROID_HEAAC_AUD_LEVEL:
            type = AUDIO_CONFIG_CMD_AANDROID_HEAAC_AUD_LEVEL;
            break;
        case HAL_AUDIO_CONFIG_CMD_SET_PCM_INFO:
            type = AUDIO_CONFIG_CMD_SET_PCM_INFO;
            break;
        case HAL_AUDIO_CONFIG_CMD_GET_TASK_STARTED:
            type = AUDIO_CONFIG_CMD_GET_TASK_STARTED;
            break;
        case HAL_AUDIO_CONFIG_CMD_DBX_DEBUG:
            type = AUDIO_CONFIG_CMD_DBX_DEBUG;
            break;
        case HAL_AUDIO_CONFIG_CMD_DBX_CONFIG:
            type = AUDIO_CONFIG_CMD_DBX_CONFIG;
            break;
        case HAL_AUDIO_CONFIG_CMD_DBX_TAB_CONFIG:
            type = AUDIO_CONFIG_CMD_DBX_TAB_CONFIG;
            break;
        default:
            ERROR("unknown Config MsgID %d\n", msgID);
            type = (AUDIO_CONFIG_CMD_MSG) -1;
            break;
    }

    if (((u_int)type != (u_int)msgID) || (type == (AUDIO_CONFIG_CMD_MSG) -1))
    {
        ERROR("MsgID didn't match (%d,%d)\n", type, msgID);
        return (AUDIO_CONFIG_CMD_MSG) -1;
    }

    return type;
}

DTV_STATUS_T RHAL_AUDIO_SendConfig(HAL_AUDIO_CONFIG_COMMAND_T *audioConfig)
{
    SINT32 i;
    AUDIO_CONFIG_COMMAND_RTKAUDIO sendconfig;
    INFO("%s Enter.\n", __func__);

    if (audioConfig == NULL)
    {
        ERROR("%d Pointer is NULL\n", __LINE__);
        return NOT_OK;
    }

    sendconfig.msg_id = ConvertConfigType(audioConfig->msgID);

    if (sendconfig.msg_id == (AUDIO_CONFIG_CMD_MSG) -1)
    {
        ERROR("msdID didn't match\n");
        return NOT_OK;
    }

    for (i = 0; i < 15; i++)
    {
        sendconfig.value[i] = audioConfig->value[i];
    }

    if (SendRPC_AudioConfig(&sendconfig) != S_OK)
    {
        ERROR("%d AudioConfig is Failed\n", __LINE__);
        return NOT_OK;
    }

    return OK;
}

AUDIO_ENUM_PRIVAETINFO ConvertPrivateType(HAL_AUDIO_ENUM_PRIVAETINFO infotype)
{
    AUDIO_ENUM_PRIVAETINFO type;

    switch(infotype)
    {
        case HAL_PRIVATEINFO_AUDIO_FORMAT_PARSER_CAPABILITY:
            type = ENUM_PRIVATEINFO_AUDIO_FORMAT_PARSER_CAPABILITY;
            break;
        case HAL_PRIVATEINFO_AUDIO_DECODER_CAPABILITY:
            type = ENUM_PRIVATEINFO_AUDIO_DECODER_CAPABILITY;
            break;
        case HAL_PRIVATEINFO_AUDIO_CONFIG_CMD_BS_INFO:
            type = ENUM_PRIVATEINFO_AUDIO_CONFIG_CMD_BS_INFO;
            break;
        case HAL_PRIVATEINFO_AUDIO_CHECK_LPCM_ENDIANESS:
            type = ENUM_PRIVATEINFO_AUDIO_CHECK_LPCM_ENDIANESS;
            break;
        case HAL_PRIVATEINFO_AUDIO_CONFIG_CMD_AO_DELAY_INFO:
            type = ENUM_PRIVATEINFO_AUDIO_CONFIG_CMD_AO_DELAY_INFO;
            break;
        case HAL_PRIVATEINFO_AUDIO_AO_CHANNEL_VOLUME_LEVEL:
            type = ENUM_PRIVATEINFO_AUDIO_AO_CHANNEL_VOLUME_LEVEL;
            break;
        case HAL_PRIVATEINFO_AUDIO_GET_FLASH_PIN:
            type = ENUM_PRIVATEINFO_AUDIO_GET_FLASH_PIN;
            break;
        case HAL_PRIVATEINFO_AUDIO_RELEASE_FLASH_PIN:
            type = ENUM_PRIVATEINFO_AUDIO_RELEASE_FLASH_PIN;
            break;
        case HAL_PRIVATEINFO_AUDIO_GET_MUTE_N_VOLUME:
            type = ENUM_PRIVATEINFO_AUDIO_GET_MUTE_N_VOLUME;
            break;
        case HAL_PRIVATEINFO_AUDIO_AO_MONITOR_FULLNESS:
            type = ENUM_PRIVATEINFO_AUDIO_AO_MONITOR_FULLNESS;
            break;
        case HAL_PRIVATEINFO_AUDIO_CONTROL_FLASH_VOLUME:
            type = ENUM_PRIVATEINFO_AUDIO_CONTROL_FLASH_VOLUME;
            break;
        case HAL_PRIVATEINFO_AUDIO_CONTROL_DAC_SWITCH:
            type = ENUM_PRIVATEINFO_AUDIO_CONTROL_DAC_SWITCH;
            break;
        case HAL_PRIVATEINFO_AUDIO_CONTROL_AO_GIVE_UP_SET_CLOCK:
            type = ENUM_PRIVATEINFO_AUDIO_CONTROL_AO_GIVE_UP_SET_CLOCK;
            break;
        case HAL_PRIVATEINFO_AUDIO_DEC_DELAY_RP:
            type = ENUM_PRIVATEINFO_AUDIO_DEC_DELAY_RP;
            break;
        case HAL_PRIVATEINFO_AUDIO_AI_SET_OUT_FMT:
            type = ENUM_PRIVATEINFO_AUDIO_AI_SET_OUT_FMT;
            break;
        case HAL_PRIVATEINFO_AUDIO_AI_SWITCH_FOCUS:
            type = ENUM_PRIVATEINFO_AUDIO_AI_SWITCH_FOCUS;
            break;
        case HAL_PRIVATEINFO_AUDIO_SUPPORT_SAMPLERATE:
            type = ENUM_PRIVATEINFO_AUDIO_SUPPORT_SAMPLERATE;
            break;
        case HAL_PRIVATEINFO_AUDIO_GET_AVAILABLE_AO_OUTPUT_PIN:
            type = ENUM_PRIVATEINFO_AUDIO_GET_AVAILABLE_AO_OUTPUT_PIN;
            break;
        case HAL_PRIVATEINFO_AUDIO_GET_PCM_IN_PIN:
            type = ENUM_PRIVATEINFO_AUDIO_GET_PCM_IN_PIN;
            break;
        case HAL_PRIVATEINFO_AUDIO_RELEASE_PCM_IN_PIN:
            type = ENUM_PRIVATEINFO_AUDIO_RELEASE_PCM_IN_PIN;
            break;
        case HAL_PRIVATEINFO_AUDIO_FIRMWARE_CAPABILITY:
            type = ENUM_PRIVATEINFO_AUDIO_FIRMWARE_CAPABILITY;
            break;
        case HAL_PRIVATEINFO_AUDIO_SRC_PROCESS_DONE:
            type = ENUM_PRIVATEINFO_AUDIO_SRC_PROCESS_DONE;
            break;
        case HAL_PRIVATEINFO_AUDIO_DEC_SRC_ENABLE:
            type = ENUM_PRIVATEINFO_AUDIO_DEC_SRC_ENABLE;
            break;
        case HAL_PRIVATEINFO_AUDIO_QUERY_FLASH_PIN:
            type = ENUM_PRIVATEINFO_AUDIO_QUERY_FLASH_PIN;
            break;
        case HAL_PRIVATEINFO_AUDIO_SET_FLASH_PIN:
            type = ENUM_PRIVATEINFO_AUDIO_SET_FLASH_PIN;
            break;
        case HAL_PRIVATEINFO_AUDIO_SET_SYSTEM_PROCESS_PID:
            type = ENUM_PRIVATEINFO_AUDIO_SET_SYSTEM_PROCESS_PID;
            break;
        case HAL_PRIVATEINFO_AUDIO_CONFIG_VIRTUALX_PARAM:
            type = ENUM_PRIVATEINFO_AUDIO_CONFIG_VIRTUALX_PARAM;
            break;
        case HAL_PRIVATEINFO_AUDIO_GET_CURR_AO_VOLUME:
            type = ENUM_PRIVATEINFO_AUDIO_GET_CURR_AO_VOLUME;
            break;
        default:
            ERROR("unknown private info type %d\n", infotype);
            type = (AUDIO_ENUM_PRIVAETINFO) -1;
            break;
    }

    if (((u_int)type != (u_int)infotype) || (type == (AUDIO_ENUM_PRIVAETINFO) -1))
    {
        ERROR("private type didn't match (%d,%d)\n", type, infotype);
        return (AUDIO_ENUM_PRIVAETINFO) -1;
    }

    return type;
}

DTV_STATUS_T RHAL_AUDIO_PrivateInfo(HAL_AUDIO_RPC_PRIVATEINFO_PARAMETERS_T *parameter, HAL_AUDIO_RPC_PRIVATEINFO_RETURNVAL_T *ret)
{
    SINT32 i;
    AUDIO_RPC_PRIVATEINFO_PARAMETERS rpc_parameter;
    AUDIO_RPC_PRIVATEINFO_RETURNVAL rpc_ret;
    INFO("%s Enter.\n", __func__);
    if ((parameter == NULL) || (ret == NULL))
    {
        ERROR("%d Pointer is NULL\n", __LINE__);
        return NOT_OK;
    }

    rpc_parameter.instanceID = parameter->instanceID;
    rpc_parameter.type = ConvertPrivateType(parameter->type);

    if ((u_int)rpc_parameter.type == (u_int)(AUDIO_CONFIG_CMD_MSG) -1)
    {
        ERROR("private type didn't match\n");
        return NOT_OK;
    }

    for (i = 0; i < 16; i++) {
        rpc_parameter.privateInfo[i] = parameter->privateInfo[i];
    }

    if (KADP_AUDIO_PrivateInfo(&rpc_parameter, &rpc_ret) != KADP_OK)
    {
        ERROR("%d PrivateInfo is Failed\n", __LINE__);
        return NOT_OK;
    }

    ret->instanceID = rpc_ret.instanceID;
    for (i = 0; i < 16; i++) {
        ret->privateInfo[i] = rpc_ret.privateInfo[i];
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetAudioSpectrumData(HAL_AUDIO_SPECTRUM_CFG_T *config)
{
    AUDIO_SPECTRUM_CFG spec_config;
    INFO("%s Enter.\n", __func__);
    if (config == NULL)
    {
        ERROR("%d Pointer is NULL\n", __LINE__);
        return NOT_OK;
    }

    spec_config.enable = config->enable;
    spec_config.bandnum = config->bandnum;
    spec_config.data_addr = config->data_addr;
    spec_config.upper_bandlimit_addr = config->upper_bandlimit_addr;

    if (KADP_AUDIO_SetAudioSpectrumData(&spec_config) != KADP_OK)
    {
        ERROR("%d AudioSpectrumData is Failed\n", __LINE__);
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetModelHash(char *pmodel_hash)
{
    AUDIO_DUMMY_DATA model_hash;
    INFO("%s Enter.\n", __func__);

    if (pmodel_hash == NULL)
    {
        ERROR("%d Pointer is NULL\n", __LINE__);
        return NOT_OK;
    }

    memcpy(&model_hash, pmodel_hash, 32);

    if (KADP_AUDIO_SetModelHash(model_hash) != KADP_OK)
    {
        ERROR("SetModelHash is Failed\n");
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetAudioAuthorityKey(SLONG customer_key)
{
    INFO("%s Enter.\n", __func__);
    if (KADP_AUDIO_SetAudioAuthorityKey(customer_key) != KADP_OK)
    {
        ERROR("%d SetAudioAuthorityKey is Failed\n", __LINE__);
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetAudioCapability(AUDIO_CAP cap)
{
    AUDIO_LICENSE_STATUS_CONFIG license_config;
    AUDIO_DUMMY_DATA model_hash;
    INFO("%s Enter.\n", __func__);

    license_config.HighWord = cap.capability;
    memcpy(license_config.LowWord, cap.auth_key, sizeof(cap.auth_key));
    memcpy(&model_hash, &cap.devhash, 32);

    if (KADP_AUDIO_SetAudioCapability(license_config) != KADP_OK)
    {
        ERROR("SetAudioCapability is Failed\n");
        return NOT_OK;
    }

    if (KADP_AUDIO_SetModelHash(model_hash) != KADP_OK)
    {
        ERROR("SetAudioCapability is Failed\n");
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetAVSyncOffset(HAL_AUDIO_HDMI_OUT_VSDB_DATA_T *info)
{
    AUDIO_HDMI_OUT_VSDB_DATA data;
    INFO("%s Enter.\n", __func__);

    if (info == NULL)
    {
        ERROR("%d Pointer is NULL\n", __LINE__);
        return NOT_OK;
    }

    data.HDMI_VSDB_delay = info->HDMI_VSDB_delay;
    if (KADP_AUDIO_SetAVSyncOffset(&data) != KADP_OK)
    {
        ERROR("%d SetAVSyncOffset is Failed\n", __LINE__);
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetVolumeWithFade(HAL_AUDIO_VOLUME_T volume, UINT8 duration, UINT8 easing)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    SINT32 dsp_gain;
    UINT32 i;

    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    INFO("%s = main %d fine %d\n", __func__, volume.mainVol, volume.fineVol);

    dsp_gain = Volume_to_DSPGain(volume);
    INFO("Spk volume index = %d\n", dsp_gain);

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_VOLUME;
    audioConfig.value[0] = ((PPAO*)Aud_Aout[0]->pDerivedObj)->GetAOAgentID(Aud_Aout[0]);
    audioConfig.value[1] = ENUM_DEVICE_SPEAKER;
    audioConfig.value[2] = 0;
    audioConfig.value[3] = 0xF0 | (easing & 0x0F);
    audioConfig.value[4] = duration;
    for(i = 1; i < 8; i++)
    {
        audioConfig.value[4+i] = dsp_gain;
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        return NOT_OK;
    }

    return OK;
}


DTV_STATUS_T RHAL_AUDIO_SPDIF_SetEnable(BOOLEAN bOnOff)
{
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    if(TRUE == bOnOff)
        KADP_AUDIO_SetSpdifOutPinSrc(SPDIFO_SRC_FIFO);
    else
        KADP_AUDIO_SetSpdifOutPinSrc(SPDIFO_SRC_DISABLE);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDTSDecoderParam(HAL_AUDIO_DTS_DEC_PARAM_TYPE paramType, UINT32 paramValue)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s Enter.\n", __func__);
    if(LITE_HAL_INIT_OK() != TRUE) return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    switch(paramType)
    {
    case HAL_AUDIO_DTS_DEC_DRC:
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DTS_DEC_PARAM;
        audioConfig.value[0] = AUDIO_DTS_DEC_DRC;
        audioConfig.value[1] = paramValue;
        break;
    case HAL_AUDIO_DTS_DEC_LFEMIX:
        audioConfig.msg_id = AUDIO_CONFIG_CMD_DTS_DEC_PARAM;
        audioConfig.value[0] = AUDIO_DTS_DEC_LFEMIX;
        audioConfig.value[1] = paramValue;
        break;
    default:
        return NOT_OK;
    }

    if(SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetTruVolume(BOOLEAN para, HAL_AUDIO_TRUVOLUME_MODE_T* pTruVolume)
{
    AUDIO_RPC_TRUVOLUME info;
    INFO("%s Enter.\n", __func__);

    if (pTruVolume == NULL) {
        return NOT_OK;
    }

    info.instanceID = -1; /* do not care */

    info.pp_TruVol_Enable = para;
    info.pp_TruVol_Info.enable = pTruVolume->enable;
    info.pp_TruVol_Info.InputGain = pTruVolume->InputGain;
    info.pp_TruVol_Info.OutputGain = pTruVolume->OutputGain;
    info.pp_TruVol_Info.BypassGain = pTruVolume->BypassGain;
    info.pp_TruVol_Info.mode = pTruVolume->mode;
    info.pp_TruVol_Info.MaxGain = pTruVolume->MaxGain;
    info.pp_TruVol_Info.NormalizerEnable = pTruVolume->NormalizerEnable;
    info.pp_TruVol_Info.BlockSize = pTruVolume->BlockSize;

    if (KADP_AUDIO_SetTruVolume(&info) != KADP_OK)
    {
        ERROR("[%s,%d] failed\n", __func__, __LINE__);
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetESInfo(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_ES_INFO_T *pAudioESInfo)
{
    HAL_AUDIO_RESOURCE_T curResourceId = adec2res(adecIndex);
    UINT16 sampleRate;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;
    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX))
        return NOT_OK;

    if(pAudioESInfo == NULL)
        return NOT_OK;

    if(AUDIO_HAL_CHECK_STOP_NOTAVAILABLE(ResourceStatus[curResourceId], 0)) //decoder can only have one input
    {
        INFO("%s is not at run state %d\n",
                GetResourceString(curResourceId), ResourceStatus[curResourceId].connectStatus[0]);
        return NOT_OK;
    }

    RAL_AUDIO_GetESInfo(adecIndex,pAudioESInfo,&sampleRate);

    return OK;
}

void* dap_param_malloc(HAL_AUDIO_DAP_PARAM_TYPE paramType, int paramValueSize, AUDIO_RPC_PRIVATEINFO_PARAMETERS *parameter)
{
    BYTE *nonCachedAddr;
    UINT8 *p_return = NULL;
    UINT32 p_addr;

    memset(parameter, 0, sizeof(struct AUDIO_RPC_PRIVATEINFO_PARAMETERS));

    if(paramType == HAL_AUDIO_SET_DAP_ENABLE || paramType == HAL_AUDIO_GET_DAP_ENABLE){
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_ENABLE) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_ENABLE : ENUM_PRIVATEINFO_AUDIO_GET_DAP_ENABLE;
    } else if(paramType == HAL_AUDIO_SET_DAP_PARAM || paramType == HAL_AUDIO_GET_DAP_PARAM){
        if(paramValueSize != sizeof(HAL_DAP_PARAM)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_PARAM) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_PARAM : ENUM_PRIVATEINFO_AUDIO_GET_DAP_PARAM;
    } else if(paramType == HAL_AUDIO_SET_DAP_DIALOGUE_ENHANCER || paramType == HAL_AUDIO_GET_DAP_DIALOGUE_ENHANCER) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_DIALOGUE_ENHANCER)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_DIALOGUE_ENHANCER));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_DIALOGUE_ENHANCER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_DIALOGUE_ENHANCER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_DIALOGUE_ENHANCER;
    } else if(paramType == HAL_AUDIO_SET_DAP_VOLUME_LEVELER || paramType == HAL_AUDIO_GET_DAP_VOLUME_LEVELER) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_VOLUME_LEVELER)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_VOLUME_LEVELER));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_VOLUME_LEVELER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_VOLUME_LEVELER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_VOLUME_LEVELER;
    } else if(paramType == HAL_AUDIO_SET_DAP_INTELLIGENT_EQUALIZER || paramType == HAL_AUDIO_GET_DAP_INTELLIGENT_EQUALIZER) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_INTELLIGENT_EQUALIZER)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_INTELLIGENT_EQUALIZER));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_INTELLIGENT_EQUALIZER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_INTELLIGENT_EQUALIZER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_INTELLIGENT_EQUALIZER;
    } else if(paramType == HAL_AUDIO_SET_DAP_SURROUND_DECODER || paramType == HAL_AUDIO_GET_DAP_SURROUND_DECODER) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_SURROUND_DECODER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_SURROUND_DECODER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_SURROUND_DECODER;
    } else if(paramType == HAL_AUDIO_SET_DAP_MEDIA_INTELLIGENCE || paramType == HAL_AUDIO_GET_DAP_MEDIA_INTELLIGENCE) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_MEDIA_INTELLIGENCE)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_MEDIA_INTELLIGENCE));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_MEDIA_INTELLIGENCE) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_MEDIA_INTELLIGENCE : ENUM_PRIVATEINFO_AUDIO_GET_DAP_MEDIA_INTELLIGENCE;
    } else if(paramType == HAL_AUDIO_SET_DAP_SURROUND_VIRTUALIZER || paramType == HAL_AUDIO_GET_DAP_SURROUND_VIRTUALIZER) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_SURROUND_VIRTUALIZER)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_SURROUND_VIRTUALIZER));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_SURROUND_VIRTUALIZER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_SURROUND_VIRTUALIZER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_SURROUND_VIRTUALIZER;
    } else if(paramType == HAL_AUDIO_SET_DAP_GRAPHICAL_EQUALIZER || paramType == HAL_AUDIO_GET_DAP_GRAPHICAL_EQUALIZER) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_GRAPHICAL_EQUALIZER)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_GRAPHICAL_EQUALIZER));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_GRAPHICAL_EQUALIZER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_GRAPHICAL_EQUALIZER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_GRAPHICAL_EQUALIZER;
    } else if(paramType == HAL_AUDIO_SET_DAP_AUDIO_OPTIMIZER || paramType == HAL_AUDIO_GET_DAP_AUDIO_OPTIMIZER) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_AUDIO_OPTIMIZER)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_AUDIO_OPTIMIZER));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_AUDIO_OPTIMIZER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_AUDIO_OPTIMIZER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_AUDIO_OPTIMIZER;
    } else if(paramType == HAL_AUDIO_SET_DAP_AUDIO_REGULATOR || paramType == HAL_AUDIO_GET_DAP_AUDIO_REGULATOR) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_AUDIO_REGULATOR)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_AUDIO_REGULATOR));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_AUDIO_REGULATOR) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_AUDIO_REGULATOR : ENUM_PRIVATEINFO_AUDIO_GET_DAP_AUDIO_REGULATOR;
    } else if(paramType == HAL_AUDIO_SET_DAP_BASS_ENHANCER || paramType == HAL_AUDIO_GET_DAP_BASS_ENHANCER) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_BASS_ENHANCER)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_BASS_ENHANCER));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_BASS_ENHANCER) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_BASS_ENHANCER : ENUM_PRIVATEINFO_AUDIO_GET_DAP_BASS_ENHANCER;
    } else if(paramType == HAL_AUDIO_SET_DAP_VIRTUAL_BASS || paramType == HAL_AUDIO_GET_DAP_VIRTUAL_BASS) {
        if(paramValueSize != sizeof(HAL_DAP_PARAM_VIRTUAL_BASS)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(HAL_DAP_PARAM_VIRTUAL_BASS));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_VIRTUAL_BASS) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_VIRTUAL_BASS : ENUM_PRIVATEINFO_AUDIO_GET_DAP_VIRTUAL_BASS;
    } else if(paramType == HAL_AUDIO_SET_DAP_BASS_EXTRACTION || paramType == HAL_AUDIO_GET_DAP_BASS_EXTRACTION) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_BASS_EXTRACTION) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_BASS_EXTRACTION : ENUM_PRIVATEINFO_AUDIO_GET_DAP_BASS_EXTRACTION;
    } else if(paramType == HAL_AUDIO_SET_DAP_DMX_TYPE || paramType == HAL_AUDIO_GET_DAP_DMX_TYPE) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_DMX_TYPE) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_DMX_TYPE : ENUM_PRIVATEINFO_AUDIO_GET_DAP_DMX_TYPE;
    } else if(paramType == HAL_AUDIO_SET_DAP_VIRTUALIZER_ENABLE || paramType == HAL_AUDIO_GET_DAP_VIRTUALIZER_ENABLE) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_VIRTUALIZER_ENABLE) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_VIRTUALIZER_ENABLE : ENUM_PRIVATEINFO_AUDIO_GET_DAP_VIRTUALIZER_ENABLE;
    } else if(paramType == HAL_AUDIO_SET_DAP_HEADPHONE_REVERB || paramType == HAL_AUDIO_GET_DAP_HEADPHONE_REVERB) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_HEADPHONE_REVERB) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_HEADPHONE_REVERB : ENUM_PRIVATEINFO_AUDIO_GET_DAP_HEADPHONE_REVERB;
    } else if(paramType == HAL_AUDIO_SET_DAP_SPEAKER_START || paramType == HAL_AUDIO_GET_DAP_SPEAKER_START) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_SPEAKER_START) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_SPEAKER_START : ENUM_PRIVATEINFO_AUDIO_GET_DAP_SPEAKER_START;
    } else if(paramType == HAL_AUDIO_SET_DAP_DE_DUCKING || paramType == HAL_AUDIO_GET_DAP_DE_DUCKING) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_DE_DUCKING) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_DE_DUCKING : ENUM_PRIVATEINFO_AUDIO_GET_DAP_DE_DUCKING;
    } else if(paramType == HAL_AUDIO_SET_DAP_PREGAIN || paramType == HAL_AUDIO_GET_DAP_PREGAIN) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_PREGAIN) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_PREGAIN : ENUM_PRIVATEINFO_AUDIO_GET_DAP_PREGAIN;
    } else if(paramType == HAL_AUDIO_SET_DAP_POSTGAIN || paramType == HAL_AUDIO_GET_DAP_POSTGAIN) {
        if(paramValueSize != sizeof(int)){
            ERROR("[RHAL] DAP param size fail! type:%d(%x->%x) \n", paramType, paramValueSize, sizeof(int));
            return NULL;
        }

        parameter->type = (paramType == HAL_AUDIO_SET_DAP_POSTGAIN) ?
            ENUM_PRIVATEINFO_AUDIO_SET_DAP_POSTGAIN : ENUM_PRIVATEINFO_AUDIO_GET_DAP_POSTGAIN;
    } else {
        ERROR("[RHAL] error, DAP param unknow type:%d\n", paramType);
        return NULL;
    }

    p_addr = rtkaudio_alloc_uncached(paramValueSize, &p_return);
    nonCachedAddr = p_return;
    parameter->privateInfo[0] = p_addr;

    return p_return;
}

DTV_STATUS_T RHAL_AUDIO_SetDAPParam(HAL_AUDIO_DAP_PARAM_TYPE paramType, void *paramValue, unsigned int paramValueSize)
{
    /* Get Avalaible Outpin */
    struct AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
    AUDIO_RPC_PRIVATEINFO_RETURNVAL ret;
    void *p_dap_param = NULL;
    INFO("%s Enter.\n", __func__);
    if(paramValue == NULL){
        ERROR("[RHAL] Error, RHAL_AUDIO_SetDAPParam fail!\n");
        return NOT_OK;
    }

    p_dap_param = dap_param_malloc(paramType, paramValueSize, &parameter);
    if(p_dap_param == NULL){
        ERROR("[RHAL] Error, dap_rtkaudio_malloc fail! (set)\n");
        return NOT_OK;
    }

    memcpy(p_dap_param, paramValue, paramValueSize);
    KADP_AUDIO_PrivateInfo(&parameter, &ret);
    rtkaudio_free(parameter.privateInfo[0]);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_GetDAPParam(HAL_AUDIO_DAP_PARAM_TYPE paramType, void *paramValue, unsigned int paramValueSize)
{
    /* Get Avalaible Outpin */
    struct AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
    AUDIO_RPC_PRIVATEINFO_RETURNVAL ret;
    void *p_dap_param = NULL;
    INFO("%s Enter.\n", __func__);
    if(paramValue == NULL){
        ERROR("[RHAL] Error, RHAL_AUDIO_GetDAPParam fail!\n");
        return NOT_OK;
    }

    p_dap_param = dap_param_malloc(paramType, paramValueSize, &parameter);
    if(p_dap_param == NULL){
        ERROR("[RHAL] Error, dap_rtkaudio_malloc fail! (get)\n");
        return NOT_OK;
    }

    KADP_AUDIO_PrivateInfo(&parameter, &ret);
    memcpy(paramValue, p_dap_param, paramValueSize);
    rtkaudio_free(parameter.privateInfo[0]);

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDAPTuningData(void *pData, unsigned int dataBytes)
{
    UINT32 ret = 0;
    UINT8 *data_cma = NULL;
    UINT32 data_cma_phy = 0;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s Enter. dataBytes=%d\n", __func__, dataBytes);

    if (LITE_HAL_INIT_OK() != TRUE){
    	ERROR("[%s %d] Audio is not initialized\n",__func__,__LINE__);
    	return NOT_OK;
    }
    if(pData == NULL || dataBytes == 0) {
        ERROR("[%s] pData:%d, dataBytes:%d\n",__func__,pData, dataBytes);
        return NOT_OK;
    }

    data_cma_phy = rtkaudio_alloc_uncached(dataBytes, &data_cma);
    if (data_cma == NULL) {
    	ERROR("[%s %d] RTKAudio_Malloc data size=%d fail\n",__func__,__LINE__,dataBytes);
    	return NOT_OK;
    }

    memcpy(data_cma, pData, dataBytes);

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_DAP_TUNINGDATA;
    audioConfig.value[0] = data_cma_phy;
    audioConfig.value[1] = dataBytes;

    ret = KADP_AUDIO_AudioConfig(&audioConfig);

    if(ret != 0){
        rtkaudio_free(data_cma_phy);
        ERROR("[%s %d] fail\n",__func__,__LINE__);
        return NOT_OK;
    }
    rtkaudio_free(data_cma_phy);
    return OK;

}

DTV_STATUS_T RHAL_AUDIO_TruvolumeHD_SetParameter(HAL_AUDIO_TRUVOLUMEHD_PARAM_TYPE_T param_type, void* data)
{
    AUDIO_TRUVOLUMEHD_PARAM_INFO truvolumehd_param;
    KADP_STATUS_T ret;
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(data == NULL) return NOT_OK;

    if(param_type == HAL_AUDIO_TRUVOLUMEHD_PARAM_ENABLE){
        truvolumehd_param.ParameterType = (enum_dtsParamTruvolumeHDType)ENUM_DTS_PARAM_LOUDNESS_CONTROL_ENABLE_I32;
    }else if(param_type == HAL_AUDIO_TRUVOLUMEHD_PARAM_TARGET_LOUNDNESS){
        truvolumehd_param.ParameterType = (enum_dtsParamTruvolumeHDType)ENUM_DTS_PARAM_LOUDNESS_CONTROL_TARGET_LOUDNESS_I32;
    }else if(param_type == HAL_AUDIO_TRUVOLUMEHD_PARAM_PRESET){
        truvolumehd_param.ParameterType = (enum_dtsParamTruvolumeHDType)ENUM_DTS_PARAM_LOUDNESS_CONTROL_PRESET_I32;
    }
    truvolumehd_param.Value = *((int*)data);
    truvolumehd_param.ValueSize = sizeof(truvolumehd_param.Value);
    ret = KADP_AUDIO_SetTruvolumdHDParam(&truvolumehd_param);

    if (KADP_OK != ret)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_TruvolumeHD_GetParameter(HAL_AUDIO_TRUVOLUMEHD_PARAM_TYPE_T param_type, void* data)
{
    KADP_STATUS_T ret;
    AUDIO_TRUVOLUMEHD_PARAM_INFO truvolumehd_param;
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(data == NULL) return NOT_OK;

    if(param_type == HAL_AUDIO_TRUVOLUMEHD_PARAM_ENABLE) {
        truvolumehd_param.ParameterType = (enum_dtsParamTruvolumeHDType)ENUM_DTS_PARAM_LOUDNESS_CONTROL_ENABLE_I32;
    }else if(param_type == HAL_AUDIO_TRUVOLUMEHD_PARAM_TARGET_LOUNDNESS) {
        truvolumehd_param.ParameterType = (enum_dtsParamTruvolumeHDType)ENUM_DTS_PARAM_LOUDNESS_CONTROL_TARGET_LOUDNESS_I32;
    }if(param_type == HAL_AUDIO_TRUVOLUMEHD_PARAM_PRESET) {
        truvolumehd_param.ParameterType = (enum_dtsParamTruvolumeHDType)ENUM_DTS_PARAM_LOUDNESS_CONTROL_PRESET_I32;
    }
    truvolumehd_param.Value = 0;
    truvolumehd_param.ValueSize = sizeof(truvolumehd_param.Value);
    ret = KADP_AUDIO_GetTruvolumdHDParam(&truvolumehd_param);
    *((int*)data) = truvolumehd_param.Value;

    if (KADP_OK != ret)
        return NOT_OK;

    return OK;
}

int getVXEnumFrmRhalParamType(HAL_AUDIO_VX_PARAM_TYPE_T param_type)
{
    switch(param_type) {
        case HAL_AUDIO_VX_PARAM_VX_ENABLE:
            return ENUM_DTS_PARAM_VX_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_VX_INPUT_MODE:
            return ENUM_DTS_PARAM_VX_INPUT_MODE_I32;
        case HAL_AUDIO_VX_PARAM_VX_OUTPUT_MODE:
            return ENUM_DTS_PARAM_VX_OUTPUT_MODE_I32;
        case HAL_AUDIO_VX_PARAM_VX_HEADROOM_GAIN:
            return ENUM_DTS_PARAM_VX_HEADROOM_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_VX_PROC_OUTPUT_GAIN:
            return ENUM_DTS_PARAM_VX_PROC_OUTPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_VX_REFERENCE_LEVEL:
            return ENUM_DTS_PARAM_VX_REFERENCE_LEVEL_I32;
        case HAL_AUDIO_VX_PARAM_VX_GAIN_COMPENSATION:
            return ENUM_DTS_PARAM_VX_GAIN_COMPENSATION_I32;
        case HAL_AUDIO_VX_PARAM_VX_PROCESS_DISCARD:
            return ENUM_DTS_PARAM_VX_PROCESS_DISCARD_I32;
        case HAL_AUDIO_VX_PARAM_TSX_ENABLE:
            return ENUM_DTS_PARAM_TSX_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TSX_PASSIVEMATRIXUPMIX_ENABLE:
            return ENUM_DTS_PARAM_TSX_PASSIVEMATRIXUPMIX_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TSX_HEIGHT_UPMIX_ENABLE:
            return ENUM_DTS_PARAM_TSX_HEIGHT_UPMIX_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TSX_PHANTOM_CENTER_GAIN:
            return ENUM_DTS_PARAM_TSX_PHANTOM_CENTER_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TSX_CENTER_GAIN:
            return ENUM_DTS_PARAM_TSX_CENTER_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TSX_HORIZ_VIR_EFF_CTRL:
            return ENUM_DTS_PARAM_TSX_HORIZ_VIR_EFF_CTRL_I32;
        case HAL_AUDIO_VX_PARAM_TSX_HEIGHTMIX_COEFF:
            return ENUM_DTS_PARAM_TSX_HEIGHTMIX_COEFF_I32;
        case HAL_AUDIO_VX_PARAM_TSX_HEIGHTOUTCH_GAIN:
            return ENUM_DTS_PARAM_TSX_HEIGHTOUTCH_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TSX_LFE_GAIN:
            return ENUM_DTS_PARAM_TSX_LFE_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TSX_LRMIX_RATIO2CTRGAIN:
            return ENUM_DTS_PARAM_TSX_LRMIX_RATIO2CTRGAIN_I32;
        case HAL_AUDIO_VX_PARAM_TSX_PRECOND_FRONT:
            return ENUM_DTS_PARAM_TSX_PRECOND_FRONT_I32;
        case HAL_AUDIO_VX_PARAM_TSX_PRECOND_SURND:
            return ENUM_DTS_PARAM_TSX_PRECOND_SURND_I32;
        case HAL_AUDIO_VX_PARAM_TSX_TOPSPK_LOC_CTRL:
            return ENUM_DTS_PARAM_TSX_TOPSPK_LOC_CTRL_I32;
        case HAL_AUDIO_VX_PARAM_TSX_FRONT_WIDE_ENABLE:
            return ENUM_DTS_PARAM_TSX_FRONT_WIDE_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TSX_HEIGHT_VIRTUALIZER_ENABLE:
            return ENUM_DTS_PARAM_TSX_HEIGHT_VIRTUALIZER_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TSX_FRONT_SURROUND_ENABLE:
            return ENUM_DTS_PARAM_TSX_FRONT_SURROUND_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TSX_HEIGHT_DISCARD:
            return ENUM_DTS_PARAM_TSX_HEIGHT_DISCARD_I32;
        case HAL_AUDIO_VX_PARAM_VX_DC_ENABLE:
            return ENUM_DTS_PARAM_VX_DC_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_VX_DC_CONTROL:
            return ENUM_DTS_PARAM_VX_DC_CONTROL_I32;
        case HAL_AUDIO_VX_PARAM_VX_DEF_ENABLE:
            return  ENUM_DTS_PARAM_VX_DEF_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_VX_DEF_CONTROL:
            return ENUM_DTS_PARAM_VX_DEF_CONTROL_I32;
        case HAL_AUDIO_VX_PARAM_VX_CS2TO3_ENABLE:
            return ENUM_DTS_PARAM_VX_CS2TO3_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TSX_CUSTOMCOEFF_STU:
            return ENUM_DTS_PARAM_TSX_CUSTOMCOEFF_STU;
        case HAL_AUDIO_VX_PARAM_TSX_CONTROLS_STU:
            return ENUM_DTS_PARAM_TSX_CONTROLS_STU;
        case HAL_AUDIO_VX_PARAM_TSX_APP_LSTNR_DIST:
            return ENUM_DTS_PARAM_TSX_APP_LSTNR_DIST_F32;
        case HAL_AUDIO_VX_PARAM_TSX_APP_BTTMSPKTOCTR_DIST:
            return ENUM_DTS_PARAM_TSX_APP_BTTMSPKTOCTR_DIST_F32;
        case HAL_AUDIO_VX_PARAM_TSX_APP_TOPSPKTOCTR_DIST:
	        return ENUM_DTS_PARAM_TSX_APP_TOPSPKTOCTR_DIST_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_FRONT_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_MODE:
            return ENUM_DTS_PARAM_TBHDX_FRONT_MODE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_SPKSIZE:
            return ENUM_DTS_PARAM_TBHDX_FRONT_SPKSIZE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_DYNAMICS:
            return ENUM_DTS_PARAM_TBHDX_FRONT_DYNAMICS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_HP_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_FRONT_HP_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_HPORDER:
            return ENUM_DTS_PARAM_TBHDX_FRONT_HPORDER_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_BASSLVL:
            return ENUM_DTS_PARAM_TBHDX_FRONT_BASSLVL_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_FRONT_EXTBASS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_INPUT_GAIN:
            return ENUM_DTS_PARAM_TBHDX_FRONT_INPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_BYPASS_GAIN:
            return ENUM_DTS_PARAM_TBHDX_FRONT_BYPASS_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_FRONT_CUSTOMCOEFF_STU:
            return ENUM_DTS_PARAM_TBHDX_FRONT_CUSTOMCOEFF_STU;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_REAR_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_MODE:
            return ENUM_DTS_PARAM_TBHDX_REAR_MODE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_SPKSIZE:
            return ENUM_DTS_PARAM_TBHDX_REAR_SPKSIZE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_DYNAMICS:
            return ENUM_DTS_PARAM_TBHDX_REAR_DYNAMICS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_HP_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_REAR_HP_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_HPORDER:
            return ENUM_DTS_PARAM_TBHDX_REAR_HPORDER_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_BASSLVL:
            return ENUM_DTS_PARAM_TBHDX_REAR_BASSLVL_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_REAR_EXTBASS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_INPUT_GAIN:
            return ENUM_DTS_PARAM_TBHDX_REAR_INPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_BYPASS_GAIN:
            return ENUM_DTS_PARAM_TBHDX_REAR_BYPASS_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_REAR_CUSTOMCOEFF_STU:
            return ENUM_DTS_PARAM_TBHDX_REAR_CUSTOMCOEFF_STU;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_CENTER_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_SPKSIZE:
            return ENUM_DTS_PARAM_TBHDX_CENTER_SPKSIZE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_DYNAMICS:
            return ENUM_DTS_PARAM_TBHDX_CENTER_DYNAMICS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_HP_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_CENTER_HP_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_HPORDER:
            return ENUM_DTS_PARAM_TBHDX_CENTER_HPORDER_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_BASSLVL:
            return ENUM_DTS_PARAM_TBHDX_CENTER_BASSLVL_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_CENTER_EXTBASS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_INPUT_GAIN:
            return ENUM_DTS_PARAM_TBHDX_CENTER_INPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_BYPASS_GAIN:
            return ENUM_DTS_PARAM_TBHDX_CENTER_BYPASS_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CENTER_CUSTOMCOEFF_STU:
            return ENUM_DTS_PARAM_TBHDX_CENTER_CUSTOMCOEFF_STU;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_SRRND_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_MODE:
            return ENUM_DTS_PARAM_TBHDX_SRRND_MODE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_SPKSIZE:
            return ENUM_DTS_PARAM_TBHDX_SRRND_SPKSIZE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_DYNAMICS:
            return ENUM_DTS_PARAM_TBHDX_SRRND_DYNAMICS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_HP_ENABLE:
            return ENUM_DTS_PARAM_TBHDX_SRRND_HP_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_HPORDER:
            return ENUM_DTS_PARAM_TBHDX_SRRND_HPORDER_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_BASSLVL:
            return ENUM_DTS_PARAM_TBHDX_SRRND_BASSLVL_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_SRRND_EXTBASS_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_INPUT_GAIN:
            return ENUM_DTS_PARAM_TBHDX_SRRND_INPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_BYPASS_GAIN:
            return ENUM_DTS_PARAM_TBHDX_SRRND_BYPASS_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_SRRND_CUSTOMCOEFF_STU:
            return ENUM_DTS_PARAM_TBHDX_SRRND_CUSTOMCOEFF_STU;
        case HAL_AUDIO_VX_PARAM_TBHDX_DELAY_MATCHING_GAIN:
            return ENUM_DTS_PARAM_TBHDX_DELAY_MATCHING_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_PROCESS_DISCARD:
            return ENUM_DTS_PARAM_TBHDX_PROCESS_DISCARD_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_DELAY:
            return ENUM_DTS_PARAM_TBHDX_DELAY_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_CONTROLS_STU:
            return ENUM_DTS_PARAM_TBHDX_CONTROLS_STU;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_FRT_SPKSZIR:
            return ENUM_DTS_PARAM_TBHDX_APP_FRT_SPKSZIR_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_FRT_BASSCTRL:
            return ENUM_DTS_PARAM_TBHDX_APP_FRT_BASSCTRL_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_FRT_HPRATIO:
            return ENUM_DTS_PARAM_TBHDX_APP_FRT_HPRATIO_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_FRT_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_APP_FRT_EXTBASS_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_CTR_SPKSZIR:
            return ENUM_DTS_PARAM_TBHDX_APP_CTR_SPKSZIR_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_CTR_BASSCTRL:
            return ENUM_DTS_PARAM_TBHDX_APP_CTR_BASSCTRL_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_CTR_HPRATIO:
            return ENUM_DTS_PARAM_TBHDX_APP_CTR_HPRATIO_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_CTR_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_APP_CTR_EXTBASS_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_SUR_SPKSZIR:
            return ENUM_DTS_PARAM_TBHDX_APP_SUR_SPKSZIR_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_SUR_BASSCTRL:
            return ENUM_DTS_PARAM_TBHDX_APP_SUR_BASSCTRL_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_SUR_HPRATIO:
            return ENUM_DTS_PARAM_TBHDX_APP_SUR_HPRATIO_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_SUR_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_APP_SUR_EXTBASS_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_REAR_SPKSZIR:
            return ENUM_DTS_PARAM_TBHDX_APP_REAR_SPKSZIR_I32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_REAR_BASSCTRL:
            return ENUM_DTS_PARAM_TBHDX_APP_REAR_BASSCTRL_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_REAR_HPRATIO:
            return ENUM_DTS_PARAM_TBHDX_APP_REAR_HPRATIO_F32;
        case HAL_AUDIO_VX_PARAM_TBHDX_APP_REAR_EXTBASS:
            return ENUM_DTS_PARAM_TBHDX_APP_REAR_EXTBASS_F32;
        case HAL_AUDIO_VX_PARAM_MBHL_ENABLE:
            return ENUM_DTS_PARAM_MBHL_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_IO_MODE:
            return ENUM_DTS_PARAM_MBHL_IO_MODE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_ALG_DELAY:
            return ENUM_DTS_PARAM_MBHL_ALG_DELAY_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_ALG_GAIN:
            return ENUM_DTS_PARAM_MBHL_ALG_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_BYPASS_GAIN:
            return ENUM_DTS_PARAM_MBHL_BYPASS_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_REFERENCE_LEVEL:
            return ENUM_DTS_PARAM_MBHL_REFERENCE_LEVEL_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_VOLUME:
            return ENUM_DTS_PARAM_MBHL_VOLUME_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_VOLUME_STEP:
            return ENUM_DTS_PARAM_MBHL_VOLUME_STEP_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_BALANCE_STEP:
            return ENUM_DTS_PARAM_MBHL_BALANCE_STEP_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_OUTPUT_GAIN:
            return ENUM_DTS_PARAM_MBHL_OUTPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_BOOST:
            return ENUM_DTS_PARAM_MBHL_BOOST_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_THRESHOLD:
            return ENUM_DTS_PARAM_MBHL_THRESHOLD_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_SLOW_OFFSET:
            return ENUM_DTS_PARAM_MBHL_SLOW_OFFSET_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_FAST_ATTACK:
            return ENUM_DTS_PARAM_MBHL_FAST_ATTACK_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_FAST_RELEASE:
            return ENUM_DTS_PARAM_MBHL_FAST_RELEASE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_SLOW_ATTACK:
            return ENUM_DTS_PARAM_MBHL_SLOW_ATTACK_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_SLOW_RELEASE:
            return ENUM_DTS_PARAM_MBHL_SLOW_RELEASE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_DELAY:
            return ENUM_DTS_PARAM_MBHL_DELAY_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_ENVELOPE_FREQUENCY:
            return ENUM_DTS_PARAM_MBHL_ENVELOPE_FREQUENCY_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_MODE:
            return ENUM_DTS_PARAM_MBHL_MODE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_CROSS_LOW:
            return ENUM_DTS_PARAM_MBHL_CROSS_LOW_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_CROSS_MID:
            return ENUM_DTS_PARAM_MBHL_CROSS_MID_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_ATTACK:
            return ENUM_DTS_PARAM_MBHL_COMP_ATTACK_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_LOW_RELEASE:
            return ENUM_DTS_PARAM_MBHL_COMP_LOW_RELEASE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_LOW_RATIO:
            return ENUM_DTS_PARAM_MBHL_COMP_LOW_RATIO_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_LOW_THRESH:
            return ENUM_DTS_PARAM_MBHL_COMP_LOW_THRESH_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_LOW_MAKEUP:
            return ENUM_DTS_PARAM_MBHL_COMP_LOW_MAKEUP_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_MID_RELEASE:
            return ENUM_DTS_PARAM_MBHL_COMP_MID_RELEASE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_MID_RATIO:
            return ENUM_DTS_PARAM_MBHL_COMP_MID_RATIO_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_MID_THRESH:
            return ENUM_DTS_PARAM_MBHL_COMP_MID_THRESH_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_MID_MAKEUP:
            return ENUM_DTS_PARAM_MBHL_COMP_MID_MAKEUP_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_HIGH_RELEASE:
            return ENUM_DTS_PARAM_MBHL_COMP_HIGH_RELEASE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_HIGH_RATIO:
            return ENUM_DTS_PARAM_MBHL_COMP_HIGH_RATIO_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_HIGH_THRESH:
            return ENUM_DTS_PARAM_MBHL_COMP_HIGH_THRESH_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_HIGH_MAKEUP:
            return ENUM_DTS_PARAM_MBHL_COMP_HIGH_MAKEUP_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_GAIN_REDUCTION:
            return ENUM_DTS_PARAM_MBHL_GAIN_REDUCTION_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COMP_GAIN_REDUCTION:
            return ENUM_DTS_PARAM_MBHL_COMP_GAIN_REDUCTION_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_PROCESS_DISCARD:
            return ENUM_DTS_PARAM_MBHL_PROCESS_DISCARD_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_AC_ENABLE:
            return ENUM_DTS_PARAM_MBHL_AC_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_AC_LEVEL:
            return ENUM_DTS_PARAM_MBHL_AC_LEVEL_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_CP_ENABLE:
            return ENUM_DTS_PARAM_MBHL_CP_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_CP_LEVEL:
            return ENUM_DTS_PARAM_MBHL_CP_LEVEL_I32;
        case HAL_AUDIO_VX_PARAM_MBHL_COEF_STU:
            return ENUM_DTS_PARAM_MBHL_COEF_STU;
        case HAL_AUDIO_VX_PARAM_MBHL_CONTROLS_STU:
            return ENUM_DTS_PARAM_MBHL_CONTROLS_STU;
        case HAL_AUDIO_VX_PARAM_MBHL_APP_LOWCROSS:
            return ENUM_DTS_PARAM_MBHL_APP_LOWCROSS_F32;
        case HAL_AUDIO_VX_PARAM_MBHL_APP_MIDCROSS:
            return ENUM_DTS_PARAM_MBHL_APP_MIDCROSS_F32;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_ENABLE:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_ENABLE_I32_1;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_IO_MODE:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_IO_MODE_I32_1;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_TARGET_LOUDNESS:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_TARGET_LOUDNESS_I32_1;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_PRESET:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_PRESET_I32_1;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_DISCARD:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_DISCARD_I32_1;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_LATENCY_MODE:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_LATENCY_MODE_I32_1;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_DELAY:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_DELAY_I32_1;
        case HAL_AUDIO_VX_PARAM_LOUDNESS_CONTROL_CONTROLS_STU:
            return ENUM_DTS_PARAM_LOUDNESS_CONTROL_CONTROLS_STU_1;
        case HAL_AUDIO_VX_PARAM_GEQ10B_ENABLE:
            return ENUM_DTS_PARAM_GEQ10B_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_CHANNEL_CTRL_MASK:
            return ENUM_DTS_PARAM_GEQ10B_CHANNEL_CTRL_MASK_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_INPUT_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_INPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND0_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND0_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND1_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND1_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND2_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND2_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND3_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND3_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND4_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND4_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND5_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND5_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND6_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND6_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND7_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND7_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND8_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND8_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_BAND9_GAIN:
            return ENUM_DTS_PARAM_GEQ10B_BAND9_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_DISCARD:
            return ENUM_DTS_PARAM_GEQ10B_DISCARD_I32;
        case HAL_AUDIO_VX_PARAM_GEQ10B_CONTROLS_STU:
            return ENUM_DTS_PARAM_GEQ10B_CONTROLS_STU;
        case HAL_AUDIO_VX_PARAM_AEQ_ENABLE:
            return ENUM_DTS_PARAM_AEQ_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_CHANNEL_CTRL_MASK:
            return ENUM_DTS_PARAM_AEQ_CHANNEL_CTRL_MASK_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_INPUT_GAIN:
            return ENUM_DTS_PARAM_AEQ_INPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_OUTPUT_GAIN:
            return ENUM_DTS_PARAM_AEQ_OUTPUT_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_BYPASS_GAIN:
            return ENUM_DTS_PARAM_AEQ_BYPASS_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_CONTROLS_STU:
            return ENUM_DTS_PARAM_AEQ_CONTROLS_STU;
        case HAL_AUDIO_VX_PARAM_AEQ_CUSTOMCOEFF_STU:
            return ENUM_DTS_PARAM_AEQ_CUSTOMCOEFF_STU;
        case HAL_AUDIO_VX_PARAM_AEQ_PROCESS_DISCARD:
            return ENUM_DTS_PARAM_AEQ_PROCESS_DISCARD_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_CH_LINK:
            return ENUM_DTS_PARAM_APP_AEQ_CH_LINK_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_BAND_NUM:
            return ENUM_DTS_PARAM_APP_AEQ_BAND_NUM_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_BAND_EN:
            return ENUM_DTS_PARAM_APP_AEQ_BAND_EN_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_BAND_FREQ:
            return ENUM_DTS_PARAM_APP_AEQ_BAND_FREQ_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_BAND_GAIN:
            return ENUM_DTS_PARAM_APP_AEQ_BAND_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_BAND_Q_FACOTR:
            return ENUM_DTS_PARAM_APP_AEQ_BAND_Q_FACOTR_I32;
        case HAL_AUDIO_VX_PARAM_AEQ_BAND_TYPE:
            return ENUM_DTS_PARAM_APP_AEQ_BAND_TYPE_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_ENABLE:
            return ENUM_DTS_PARAM_MULTIRATE_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_FILTER_MODE:
            return ENUM_DTS_PARAM_MULTIRATE_FILTER_MODE_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_IN_MODE:
            return ENUM_DTS_PARAM_MULTIRATE_IN_MODE_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_OUT_MODE:
            return ENUM_DTS_PARAM_MULTIRATE_OUT_MODE_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_UPMIX_ENABLE:
            return ENUM_DTS_PARAM_MULTIRATE_UPMIX_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_DELAY_COMPENSATION:
            return ENUM_DTS_PARAM_MULTIRATE_DELAY_COMPENSATION_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_GAIN_COMPENSATION:
            return ENUM_DTS_PARAM_MULTIRATE_GAIN_COMPENSATION_I32;
        case HAL_AUDIO_VX_PARAM_MULTIRATE_CONTROLS_STU:
            return ENUM_DTS_PARAM_MULTIRATE_CONTROLS_STU;
        case HAL_AUDIO_VX_PARAM_ILDETECT_UPMIX_ENABLE_I32:
            return ENUM_DTS_PARAM_ILDETECT_UPMIX_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_ILDETECT_UPMIX_LOW_LEVEL_THRESHOLD_I32:
            return ENUM_DTS_PARAM_ILDETECT_UPMIX_LOW_LEVEL_THRESHOLD_I32;
        case HAL_AUDIO_VX_PARAM_ILDETECT_UPMIX_PEAK_HOLD_COUNT_I32:
            return ENUM_DTS_PARAM_ILDETECT_UPMIX_PEAK_HOLD_COUNT_I32;
        case HAL_AUDIO_VX_PARAM_ILDETECT_UPMIX_ORIGLR_MIXBACK_I32:
            return ENUM_DTS_PARAM_ILDETECT_UPMIX_ORIGLR_MIXBACK_I32;
        case HAL_AUDIO_VX_PARAM_ILDETECT_UPMIX_SURROUND_GAIN_I32:
            return ENUM_DTS_PARAM_ILDETECT_UPMIX_SURROUND_GAIN_I32;
        case HAL_AUDIO_VX_PARAM_ILDETECT_UPMIX_SRRND_DELAY_ENABLE_I32:
            return ENUM_DTS_PARAM_ILDETECT_UPMIX_SRRND_DELAY_ENABLE_I32;
        case HAL_AUDIO_VX_PARAM_ILDETECT_UPMIX_PROCESS_DISCARD_I32:
            return ENUM_DTS_PARAM_ILDETECT_UPMIX_PROCESS_DISCARD_I32;

        default:
            ERROR("Unknown ParamType %x\n", param_type);
            return -1;
    }
}

DTV_STATUS_T RHAL_AUDIO_VX_SetParameter(HAL_AUDIO_VX_PARAM_TYPE_T param_type, void* data)
{
    int vxEnum;
    KADP_STATUS_T ret = KADP_NOT_OK;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(data == NULL) return NOT_OK;
    if(param_type < 0) return NOT_OK;

    vxEnum = getVXEnumFrmRhalParamType(param_type);

    if ( (vxEnum >= ENUM_DTS_PARAM_VX_ENABLE_I32 && vxEnum <= ENUM_DTS_PARAM_VIRTUALXLIB1_LAST_PARAM) || (vxEnum >= ENUM_DTS_PARAM_TSX_APP_LSTNR_DIST_F32 && vxEnum <= ENUM_DTS_PARAM_TSX_APP_TOPSPKTOCTR_DIST_F32) ) {
            AUDIO_VIRTUALX_TRUSRNDX_PARAM_INFO trusrndx_param;
            memset(&trusrndx_param, 0, sizeof(AUDIO_VIRTUALX_TRUSRNDX_PARAM_INFO));

            trusrndx_param.ParameterType = (enum_dtsParamVirtualxlib1Type)vxEnum;
            trusrndx_param.Value[0] = *((int*)data);
            trusrndx_param.ValueSize = sizeof(trusrndx_param.Value[0]);
            ret = KADP_AUDIO_SetTruSrndXParam(&trusrndx_param);
    } else if ( (vxEnum >= ENUM_DTS_PARAM_TBHDX_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_TBHDX_LAST_PARAM) || (vxEnum >= ENUM_DTS_PARAM_TBHDX_APP_FRT_SPKSZIR_I32 && vxEnum <= ENUM_DTS_PARAM_TBHDX_APP_REAR_EXTBASS_F32) ) {
            AUDIO_VIRTUALX_TBHDX_PARAM_INFO tbhdx_param;
            memset(&tbhdx_param, 0, sizeof(AUDIO_VIRTUALX_TBHDX_PARAM_INFO));

            tbhdx_param.ParameterType = (enum_dtsParamTBHDxType)vxEnum;
            tbhdx_param.Value[0] = *((int*)data);
            tbhdx_param.ValueSize = sizeof(tbhdx_param.Value[0]);
            ret = KADP_AUDIO_SetTbhdXParam(&tbhdx_param);
    } else if ( (vxEnum >= ENUM_DTS_PARAM_MBHL_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_MBHL_LAST_PARAM) || ((vxEnum >= ENUM_DTS_PARAM_MBHL_APP_LOWCROSS_F32 && vxEnum <= ENUM_DTS_PARAM_MBHL_APP_MIDCROSS_F32)) ) {
            AUDIO_VIRTUALX_MBHL_PARAM_INFO mbhl_param;
            memset(&mbhl_param, 0, sizeof(AUDIO_VIRTUALX_MBHL_PARAM_INFO));

            mbhl_param.ParameterType = (enum_dtsParamMbhlType)vxEnum;
            mbhl_param.Value[0] = *((int*)data);
            mbhl_param.ValueSize = sizeof(mbhl_param.Value[0]);
            ret = KADP_AUDIO_SetMbhlParam(&mbhl_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_LOUDNESS_CONTROL_FIRST_PARAM_1 && vxEnum <= ENUM_DTS_PARAM_LOUDNESS_CONTROL_LAST_PARAM_1) {
            AUDIO_VIRTUALX_Loudness_Control_PARAM_INFO loudness_control_param;
            memset(&loudness_control_param, 0, sizeof(AUDIO_VIRTUALX_Loudness_Control_PARAM_INFO));

            loudness_control_param.ParameterType = (enum_dtsParamLoudnessControlType)vxEnum;
            loudness_control_param.Value[0] = *((int*)data);
            loudness_control_param.ValueSize = sizeof(loudness_control_param.Value[0]);
            ret = KADP_AUDIO_SetLoudnessControlParam(&loudness_control_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_GEQ10B_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_GEQ10B_LAST_PARAM) {
            AUDIO_VIRTUALX_Geq10b_PARAM_INFO geq_param;
            memset(&geq_param, 0, sizeof(AUDIO_VIRTUALX_Geq10b_PARAM_INFO));

            geq_param.ParameterType = (enum_dtsParamGeq10bType)vxEnum;
            geq_param.Value[0] = *((int*)data);
            geq_param.ValueSize = sizeof(geq_param.Value[0]);
            ret = KADP_AUDIO_SetGeqParam(&geq_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_AEQ_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_AEQ_LAST_PARAM) {
            AUDIO_VIRTUALX_Aeq_PARAM_INFO aeq_param;
            memset(&aeq_param, 0, sizeof(AUDIO_VIRTUALX_Aeq_PARAM_INFO));

            aeq_param.ParameterType = (enum_dtsParamAeqType)vxEnum;
            aeq_param.Value[0] = *((int*)data);
            aeq_param.ValueSize = sizeof(aeq_param.Value[0]);
            ret = KADP_AUDIO_SetAeqParam(&aeq_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_MULTIRATE_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_MULTIRATE_LAST_PARAM) {
            AUDIO_VIRTUALX_MultiRate_PARAM_INFO multi_rate_param;
            memset(&multi_rate_param, 0, sizeof(AUDIO_VIRTUALX_MultiRate_PARAM_INFO));

            multi_rate_param.ParameterType = (enum_dtsParamMultiRateType)vxEnum;
            multi_rate_param.Value[0] = *((int*)data);
            multi_rate_param.ValueSize = sizeof(multi_rate_param.Value[0]);
            ret = KADP_AUDIO_SetMultiRateParam(&multi_rate_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_ILDETECT_UPMIX_ENABLE_I32 && vxEnum <= ENUM_DTS_PARAM_ILDETECT_UPMIX_LAST_PARAM) {
            AUDIO_VIRTUALX_ILDetectUpmix_PARAM_INFO ildetect_upmix_param;
            memset(&ildetect_upmix_param, 0, sizeof(AUDIO_VIRTUALX_ILDetectUpmix_PARAM_INFO));

            ildetect_upmix_param.ParameterType = (enum_dtsParamILDetectUpmixType) vxEnum;
            ildetect_upmix_param.Value[0] = *((int*) data);
            ildetect_upmix_param.ValueSize = sizeof(ildetect_upmix_param.Value[0]);
            ret = KADP_AUDIO_SetILDetectUpmixParam(&ildetect_upmix_param);
    }

    if (KADP_OK != ret)
        return NOT_OK;

    return OK;
}
DTV_STATUS_T RHAL_AUDIO_VX_GetParameter(HAL_AUDIO_VX_PARAM_TYPE_T param_type, void* data)
{
    int vxEnum;
    KADP_STATUS_T ret = KADP_NOT_OK;

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) return NOT_OK;
    if(data == NULL) return NOT_OK;
    if(param_type < 0) return NOT_OK;

    vxEnum = getVXEnumFrmRhalParamType(param_type);
    if ( (vxEnum >= ENUM_DTS_PARAM_VX_ENABLE_I32 && vxEnum <= ENUM_DTS_PARAM_VIRTUALXLIB1_LAST_PARAM) || (vxEnum >= ENUM_DTS_PARAM_TSX_APP_LSTNR_DIST_F32 && vxEnum <= ENUM_DTS_PARAM_TSX_APP_TOPSPKTOCTR_DIST_F32) ) {
            AUDIO_VIRTUALX_TRUSRNDX_PARAM_INFO trusrndx_param;
            memset(&trusrndx_param, 0, sizeof(AUDIO_VIRTUALX_TRUSRNDX_PARAM_INFO));

            trusrndx_param.ParameterType = (enum_dtsParamVirtualxlib1Type)vxEnum;
            trusrndx_param.Value[0] = 0;
            trusrndx_param.ValueSize = sizeof(trusrndx_param.Value[0]);
            ret = KADP_AUDIO_GetTruSrndXParam(&trusrndx_param);
            *((int*)data) = trusrndx_param.Value[0];
    } else if ( (vxEnum >= ENUM_DTS_PARAM_TBHDX_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_TBHDX_LAST_PARAM) || (vxEnum >= ENUM_DTS_PARAM_TBHDX_APP_FRT_SPKSZIR_I32 && vxEnum <= ENUM_DTS_PARAM_TBHDX_APP_REAR_EXTBASS_F32) ) {
            AUDIO_VIRTUALX_TBHDX_PARAM_INFO tbhdx_param;
            memset(&tbhdx_param, 0, sizeof(AUDIO_VIRTUALX_TBHDX_PARAM_INFO));

            tbhdx_param.ParameterType =(enum_dtsParamTBHDxType)vxEnum;
            tbhdx_param.Value[0] = 0;
            tbhdx_param.ValueSize = sizeof(tbhdx_param.Value[0]);
            ret = KADP_AUDIO_GetTbhdXParam(&tbhdx_param);
            *((int*)data) = tbhdx_param.Value[0];
    } else if (vxEnum == ENUM_DTS_PARAM_MBHL_COMP_GAIN_REDUCTION_I32) {
            AUDIO_VIRTUALX_MBHL_PARAM_INFO mbhl_param;
            memset(&mbhl_param, 0, sizeof(AUDIO_VIRTUALX_MBHL_PARAM_INFO));

            mbhl_param.ParameterType = (enum_dtsParamMbhlType)vxEnum;
            mbhl_param.Value[0] = 0;
            mbhl_param.Value[1] = 0;
            mbhl_param.Value[2] = 0;
            mbhl_param.ValueSize = sizeof(mbhl_param.Value[0]) * 3;
            ret = KADP_AUDIO_GetMbhlParam(&mbhl_param);
            *((int*)data) = mbhl_param.Value[0];
            *(((int*)data) + 1) = mbhl_param.Value[1];
            *(((int*)data) + 2) = mbhl_param.Value[2];
    } else if (vxEnum >= ENUM_DTS_PARAM_MBHL_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_MBHL_LAST_PARAM) {
            AUDIO_VIRTUALX_MBHL_PARAM_INFO mbhl_param;
            memset(&mbhl_param, 0, sizeof(AUDIO_VIRTUALX_MBHL_PARAM_INFO));

            mbhl_param.ParameterType = (enum_dtsParamMbhlType)vxEnum;
            mbhl_param.Value[0] = 0;
            mbhl_param.ValueSize = sizeof(mbhl_param.Value[0]);
            ret = KADP_AUDIO_GetMbhlParam(&mbhl_param);
            *((int*)data) = mbhl_param.Value[0];
    } else if (vxEnum >= ENUM_DTS_PARAM_LOUDNESS_CONTROL_FIRST_PARAM_1 && vxEnum <= ENUM_DTS_PARAM_LOUDNESS_CONTROL_LAST_PARAM_1) {
            AUDIO_VIRTUALX_Loudness_Control_PARAM_INFO loudness_control_param;
            memset(&loudness_control_param, 0, sizeof(AUDIO_VIRTUALX_Loudness_Control_PARAM_INFO));

            loudness_control_param.ParameterType = (enum_dtsParamLoudnessControlType)vxEnum;
            loudness_control_param.Value[0] = *((int*)data);
            loudness_control_param.ValueSize = sizeof(loudness_control_param.Value[0]);
            ret = KADP_AUDIO_GetLoudnessControlParam(&loudness_control_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_GEQ10B_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_GEQ10B_LAST_PARAM) {
            AUDIO_VIRTUALX_Geq10b_PARAM_INFO geq_param;
            memset(&geq_param, 0, sizeof(AUDIO_VIRTUALX_Geq10b_PARAM_INFO));

            geq_param.ParameterType = (enum_dtsParamGeq10bType)vxEnum;
            geq_param.Value[0] = *((int*)data);
            geq_param.ValueSize = sizeof(geq_param.Value[0]);
            ret = KADP_AUDIO_GetGeqParam(&geq_param);
    } else if ( (vxEnum >= ENUM_DTS_PARAM_AEQ_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_AEQ_LAST_PARAM) || (vxEnum >= ENUM_DTS_PARAM_APP_AEQ_CH_LINK_I32 && vxEnum <= ENUM_DTS_PARAM_APP_AEQ_BAND_TYPE_I32) ) {
            AUDIO_VIRTUALX_Aeq_PARAM_INFO aeq_param;
            memset(&aeq_param, 0, sizeof(AUDIO_VIRTUALX_Aeq_PARAM_INFO));

            aeq_param.ParameterType = (enum_dtsParamAeqType)vxEnum;
            aeq_param.Value[0] = *((int*)data);
            aeq_param.ValueSize = sizeof(aeq_param.Value[0]);
            ret = KADP_AUDIO_GetAeqParam(&aeq_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_MULTIRATE_FIRST_PARAM && vxEnum <= ENUM_DTS_PARAM_MULTIRATE_LAST_PARAM) {
            AUDIO_VIRTUALX_MultiRate_PARAM_INFO multi_rate_param;
            memset(&multi_rate_param, 0, sizeof(AUDIO_VIRTUALX_MultiRate_PARAM_INFO));

            multi_rate_param.ParameterType = (enum_dtsParamMultiRateType)vxEnum;
            multi_rate_param.Value[0] = *((int*)data);
            multi_rate_param.ValueSize = sizeof(multi_rate_param.Value[0]);
            ret = KADP_AUDIO_GetMultiRateParam(&multi_rate_param);
    } else if (vxEnum >= ENUM_DTS_PARAM_ILDETECT_UPMIX_ENABLE_I32 && vxEnum <= ENUM_DTS_PARAM_ILDETECT_UPMIX_LAST_PARAM) {
            AUDIO_VIRTUALX_ILDetectUpmix_PARAM_INFO ildetect_upmix_param;
            memset(&ildetect_upmix_param, 0, sizeof(AUDIO_VIRTUALX_ILDetectUpmix_PARAM_INFO));

            ildetect_upmix_param.ParameterType = (enum_dtsParamILDetectUpmixType)vxEnum;
            ildetect_upmix_param.Value[0] = *((int*)data);
            ildetect_upmix_param.ValueSize = sizeof(ildetect_upmix_param.Value[0]);
            ret = KADP_AUDIO_GetILDetectUpmixParam(&ildetect_upmix_param);
    }

    if (KADP_OK != ret)
        return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetAnalogAdjDB(HAL_AUDIO_AREA_TYPE_T area_type)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s area_type %d\n", __func__, area_type);
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    /* check value */
    if(area_type < 0 || area_type >= HAL_AUDIO_AREA_MAX){
        ERROR("[%s] area_type out of range:%d\n", __func__, area_type);
        return NOT_OK;
    }

    audioConfig.msg_id = AUDIO_CONFIG_CMD_ANALOG_ADJ_DB;
    audioConfig.value[0] = area_type;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK)
    {
        return NOT_OK;
    }

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetDolbyOTTMode(BOOLEAN bIsOTTEnable, BOOLEAN bIsATMOSLockingEnable)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s OTT=%d, AtmosLock=%d\n", __func__, bIsOTTEnable, bIsATMOSLockingEnable);
    if(LITE_HAL_INIT_OK() != TRUE)
        return NOT_OK;

    if(bIsOTTEnable == FALSE && bIsATMOSLockingEnable == TRUE)  //not enabled, invalid
        return NOT_OK;

    audioConfig.msg_id = AUDIO_CONFIG_CMD_SET_DOLBY_OTT_MODE;
    audioConfig.value[0] = bIsOTTEnable;             /* OTT mode */
    audioConfig.value[1] = bIsATMOSLockingEnable;    /* ATMOS Locking */

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

/* AC-4 decoder */
/* AC-4 Auto Presenetation Selection. Refer to "Selection using system-level preferences" of "Dolby MS12 Multistream Decoder Implementation integration manual" */
static int IsValidLang(HAL_AUDIO_LANG_CODE_TYPE_T enCodeType, UINT32 lang)
{
    UINT8 ch_1, ch_2, ch_3, ch_4;

    if (enCodeType != HAL_AUDIO_LANG_CODE_ISO639_1 &&
        enCodeType != HAL_AUDIO_LANG_CODE_ISO639_2)
        return FALSE;

    ch_1 = (lang & 0xFF000000) >> 24;
    ch_2 = (lang & 0x00FF0000) >> 16;
    ch_3 = (lang & 0x0000FF00) >> 8;
    ch_4 = (lang & 0x000000FF);

    if ((ch_1 < 0x61) || (ch_1 > 0x7a)) return FALSE;
    if ((ch_2 < 0x61) || (ch_2 > 0x7a)) return FALSE;
    if (enCodeType == HAL_AUDIO_LANG_CODE_ISO639_1 && ch_3 != 0) return FALSE;
    else if (enCodeType == HAL_AUDIO_LANG_CODE_ISO639_2 &&
            ((ch_3 < 0x61) || (ch_3 > 0x7a)))
        return FALSE;
    if (ch_4 != 0) return FALSE;

    return TRUE;
}

DTV_STATUS_T RHAL_AUDIO_AC4_SetAutoPresentationFirstLanguage(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_LANG_CODE_TYPE_T enCodeType, UINT32 firstLang)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s firstLang %d, codetype %d\n", __func__, firstLang, enCodeType);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;

    if (!IsValidLang(enCodeType, firstLang))
        return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_FIRST_LANGUAGE;
    audioConfig.value[0] = (u_int)firstLang;
    audioConfig.value[1] = (u_int)enCodeType;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AC4_SetAutoPresentationSecondLanguage(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_LANG_CODE_TYPE_T enCodeType, UINT32 secondLang)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s secondLang %d, codetype %d\n", __func__, secondLang, enCodeType);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;

    if (!IsValidLang(enCodeType, secondLang))
        return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_SECOND_LANGUAGE;
    audioConfig.value[0] = (u_int)secondLang;
    audioConfig.value[1] = (u_int)enCodeType;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AC4_SetAutoPresentationADMixing(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bIsEnable)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s bIsEnable=%d\n", __func__, bIsEnable);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_ADMIXING;
    audioConfig.value[0] = (u_int)bIsEnable;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AC4_SetAutoPresentationADType(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_AC4_AD_TYPE_T enADType)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s ADType=%d\n", __func__, enADType);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;

    if(enADType < HAL_AUDIO_AC4_AD_TYPE_NONE || enADType > HAL_AUDIO_AC4_AD_TYPE_VO)
        return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_ADTYPE;
    audioConfig.value[0] = (u_int)enADType;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AC4_SetAutoPresentationPrioritizeADType(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bIsEnable)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s bIsEnable=%d\n", __func__, bIsEnable);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_PRIORITIZE_ADTYPE;
    audioConfig.value[0] = (u_int)bIsEnable;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AC4_SetPresentationGroupIndex(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT32 pres_group_idx)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s pres_group_idx=%d\n", __func__, pres_group_idx);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_PRESENTATION_GROUP_INDEX;
    audioConfig.value[0] = (u_int)pres_group_idx;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

/* AC-4 Dialogue Enhancement */
DTV_STATUS_T RHAL_AUDIO_AC4_SetDialogueEnhancementGain(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT8 dialEnhanceGain)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s dialEnhanceGain=%d\n", __func__, dialEnhanceGain);

    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if(!RangeCheck(adecIndex, 0, AUD_ADEC_MAX)) return NOT_OK;

    if(dialEnhanceGain > 12)
        return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));

    audioConfig.msg_id = AUDIO_CONFIG_CMD_AC4_DIALOGUE_ENHANCEMENT_GAIN;
    audioConfig.value[0] = (u_int)dialEnhanceGain;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;

    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SoundConfig_Data(UINT32* pData1, UINT32 size1, UINT32* pData2, UINT32 size2)
{
    UINT32 ret = 0;
    UINT8 *data1_cma = NULL, *data2_cma = NULL;
    UINT32 data1_cma_phy = 0, data2_cma_phy = 0;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s Enter.\n", __func__);

    if (LITE_HAL_INIT_OK() != TRUE){
        ERROR("[%s %d] Audio is not initialized\n",__func__,__LINE__);
        return NOT_OK;
    }
    if(pData1 == NULL || pData2 == NULL) {
        ERROR("[%s] pData1:%p, pData2:%p\n",__func__,pData1,pData2);
        return NOT_OK;
    }

    data1_cma_phy = rtkaudio_alloc_uncached(size1, &data1_cma);
    if (data1_cma == NULL) {
        ERROR("[%s %d] RTKAudio_Malloc data1 size=%d fail\n",__func__,__LINE__,size1);
        return NOT_OK;
    }
    data2_cma_phy = rtkaudio_alloc_uncached(size2, &data2_cma);
    if (data2_cma == NULL) {
        rtkaudio_free(data1_cma_phy);
        ERROR("[%s %d] RTKAudio_Malloc data2 size=%d fail\n",__func__,__LINE__,size2);
        return NOT_OK;
    }

    memcpy(data1_cma, pData1, size1);
    memcpy(data2_cma, pData2, size2);

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_CUSTOMIZE_REQUEST;
    audioConfig.value[0] = ENUM_CUSTOMIZE_SOUND_CONFIG_DATA;
    audioConfig.value[1] = data1_cma_phy;
    audioConfig.value[2] = size1;
    audioConfig.value[3] = data2_cma_phy;
    audioConfig.value[4] = size2;

    ret = KADP_AUDIO_AudioConfig(&audioConfig);

    if(ret != 0){
        rtkaudio_free(data1_cma_phy);
        rtkaudio_free(data2_cma_phy);
        ERROR("[%s %d] fail\n",__func__,__LINE__);
        return NOT_OK;
    }
    rtkaudio_free(data1_cma_phy);
    rtkaudio_free(data2_cma_phy);
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SoundEffect_Level(UINT32 param1, UINT32 param2, UINT32 param3)
{
    UINT32 ret = 0;
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s Enter.\n", __func__);

    if (LITE_HAL_INIT_OK() != TRUE){
        ERROR("[%s %d] Audio is not initialized\n",__func__,__LINE__);
        return NOT_OK;
    }

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_CUSTOMIZE_REQUEST;
    audioConfig.value[0] = ENUM_CUSTOMIZE_SOUND_EFFECT_LEVEL;
    audioConfig.value[1] = param1;
    audioConfig.value[2] = param2;
    audioConfig.value[3] = param3;

    ret = KADP_AUDIO_AudioConfig(&audioConfig);

    if(ret != 0){
        ERROR("[%s %d] fail\n",__func__,__LINE__);
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetLowLatencyMode(BOOLEAN bOnOff)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s Enter.\n", __func__);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_AO_LOW_LATENCY;
    audioConfig.value[0] = (u_int)bOnOff;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_SetAECReference_Source(HAL_AUDIO_AEC_SOURCE_T aec_source)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s aec_source=%d\n", __func__, aec_source);
    if(AUDIO_HAL_CHECK_INITIAL_OK() != TRUE)
        return NOT_OK;

    if((aec_source >= HAL_AUDIO_AEC_SOURCE_MAX) || (aec_source <= 0)) {
        ERROR("[%s] aec_source error:%d\n", __func__, aec_source);
        return NOT_OK;
    }
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_AEC_SOURCE;
    audioConfig.value[0] = (aec_source & HAL_AUDIO_AEC_I2S_EXTERNEL_LOOP) ? TRUE : FALSE;

    if(SendRPC_AudioConfig(&audioConfig) != S_OK) return NOT_OK;
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_AISound_Enable(BOOLEAN bOnOff)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if (AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) {
        return NOT_OK;
    }
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_AISOUND_CONTROL;
    audioConfig.value[0] = bOnOff;

    if (SendRPC_AudioConfig(&audioConfig) != S_OK) {
        return NOT_OK;
    }
    return OK;
}

DTV_STATUS_T RHAL_AUDIO_BTMic_Enable(BOOLEAN bOnOff)
{
    AUDIO_CONFIG_COMMAND_RTKAUDIO audioConfig;
    INFO("%s bOnOff=%d\n", __func__, bOnOff);
    if (AUDIO_HAL_CHECK_INITIAL_OK() != TRUE) {
        return NOT_OK;
    }
    memset(&audioConfig, 0, sizeof(AUDIO_CONFIG_COMMAND_RTKAUDIO));
    audioConfig.msg_id = AUDIO_CONFIG_CMD_BTMIC_CONTROL;
    audioConfig.value[0] = bOnOff;

    if (SendRPC_AudioConfig(&audioConfig) != S_OK) {
        return NOT_OK;
    }
    return OK;
}

void RHAL_AUDIO_SyncVolumeFromQs(UINT32 ch_id, SINT32 vol)
{
    if (vol > APP_MAX_VOL)
        vol = APP_MAX_VOL;

    if (vol < APP_MIN_VOL)
        vol = APP_MIN_VOL;

    if (ch_id&APP_CH_ID_CH0) { //spkr
        m_volume_comp[0] = vol;
    }

    if (ch_id&APP_CH_ID_CH2) { //hp
        m_volume_comp[1] = vol;
    }
}
EXPORT_SYMBOL(RHAL_AUDIO_SyncVolumeFromQs);

void RHAL_AUDIO_SyncBalanceFromQs(SINT32 balance)
{
    m_balance = balance;
}
EXPORT_SYMBOL(RHAL_AUDIO_SyncBalanceFromQs);

#endif /* _HAL_AUDIO_ */
