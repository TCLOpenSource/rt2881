#ifndef _AUDIO_CMD_ID_H_
#define _AUDIO_CMD_ID_H_

/*
 * Ioctl definitions
 */

/* Use 'a' as magic number */
#define RTKAUDIO_IOC_MAGIC  'a'
/*
 * S means "Set"                : through a ptr,
 * T means "Tell"               : directly with the argument value
 * G means "Get"                : reply by setting through a pointer
 * Q means "Query"              : response is on the return value
 * X means "eXchange"           : G and S atomically
 * H means "sHift"              : T and Q atomically
 */
#define RTKAUDIO_IOC_INIT                        _IO(RTKAUDIO_IOC_MAGIC, 1)
#define RTKAUDIO_IOC_DEINIT                      _IO(RTKAUDIO_IOC_MAGIC, 2)
#define RTKAUDIO_IOC_OPEN                        _IOW(RTKAUDIO_IOC_MAGIC, 3, int)
#define RTKAUDIO_IOC_CLOSE                       _IOW(RTKAUDIO_IOC_MAGIC, 4, int)
#define RTKAUDIO_IOC_CONNECT                     _IOW(RTKAUDIO_IOC_MAGIC, 5, int)
#define RTKAUDIO_IOC_SET                         _IOW(RTKAUDIO_IOC_MAGIC, 6, int)
#define RTKAUDIO_IOC_GET                         _IOR(RTKAUDIO_IOC_MAGIC, 7, int)
#define RTKAUDIO_IOC_GET_ALL                     _IOR(RTKAUDIO_IOC_MAGIC, 8, int)
#define RTKAUDIO_IOC_FW_REMOTE_MALLOC_SUM        _IOR(RTKAUDIO_IOC_MAGIC, 9, int)
#define RTKAUDIO_IOC_ALLOC                       _IOW(RTKAUDIO_IOC_MAGIC, 10, int)
#define RTKAUDIO_IOC_FREE                        _IOW(RTKAUDIO_IOC_MAGIC, 11, int)
#define RTKAUDIO_IOC_ALLOC_SUM                   _IOR(RTKAUDIO_IOC_MAGIC, 12, int)
#define RTKAUDIO_IOC_AUDIO_CONFIG                _IOWR(RTKAUDIO_IOC_MAGIC, 13, int)
#define RTKAUDIO_IOC_READ_REG                    _IOR(RTKAUDIO_IOC_MAGIC, 14, int)
#define RTKAUDIO_IOC_WRITE_REG                   _IOW(RTKAUDIO_IOC_MAGIC, 15, int)
#define RTKAUDIO_IOC_OMX_CREATE_DECODER          _IO(RTKAUDIO_IOC_MAGIC, 16)
#define RTKAUDIO_IOC_OMX_GET_OFFLOAD_DECODER     _IOR(RTKAUDIO_IOC_MAGIC, 17, int)
#define RTKAUDIO_IOC_OMX_RELEASE_OFFLOAD_DECODER _IO(RTKAUDIO_IOC_MAGIC, 18)
#define RTKAUDIO_IOC_GET_FW_CAPABILITY           _IOR(RTKAUDIO_IOC_MAGIC, 19, int)
#define RTKAUDIO_IOC_SET_DMX_MODE                _IOW(RTKAUDIO_IOC_MAGIC, 20, int)
#define RTKAUDIO_IOC_SET_WITH_RETURN             _IOWR(RTKAUDIO_IOC_MAGIC,21, int)

typedef enum {
    SET_TP_OPEN,
    SET_TP_CLOSE,
    SET_ADC_OPEN,
    SET_ADC_CLOSE,
    SET_HDMI_OPEN,
    SET_HDMI_CLOSE,
    SET_DP_OPEN,
    SET_DP_CLOSE,
    SET_HDMI_OPEN_PORT,
    SET_HDMI_CLOSE_PORT,
    SET_AAD_OPEN,
    SET_AAD_CLOSE,
    SET_ADEC_OPEN,
    SET_ADEC_CLOSE,
    SET_SNDOUT_OPEN,
    SET_SNDOUT_CLOSE,
    SET_TP_CONNECT,
    SET_TP_DISCONNECT,
    SET_ADC_CONNECT,
    SET_ADC_DISCONNECT,
    SET_HDMI_CONNECT,
    SET_HDMI_DISCONNECT,
    SET_DP_CONNECT,
    SET_DP_DISCONNECT,
    SET_HDMI_CONNECT_PORT,
    SET_HDMI_DISCONNECT_PORT,
    SET_AAD_CONNECT,
    SET_AAD_DISCONNECT,
    SET_ADEC_CONNECT,
    SET_ADEC_DISCONNECT,
    SET_AMIX_CONNECT,
    SET_AMIX_DISCONNECT,
    SET_SE_CONNECT,
    SET_SE_DISCONNECT,
    SET_SNDOUT_CONNECT,
    SET_SNDOUT_DISCONNECT,
    SET_STOP_DECODING,
    SET_START_DECODING,
    SET_MAIN_DECODER_OUTPUT,
    SET_MAIN_AUDIO_OUTPUT,
    SET_HDMI_AUDIO_RETURN_CHANNEL,
    SET_HDMI_AUDIO_ENHANCE_RETURN_CHANNEL,
    SET_DOLBY_DRCMODE,
    SET_DOLBY_DOWNMIXMODE,
    SET_DOLBY_DUALMONO_OUTPUTMODE,
    SET_AD_MAIN,
    SET_TRICK_MODE,
    SET_DECODER_INPUT_GAIN,
    SET_GLOBAL_DECODER_GAIN,
    SET_DECODER_DELAY_TIME,
    SET_MIXER_INPUT_GAIN,
    SET_PBOUT_VOLUME,
    SET_TSOUT_VOLUME,
    SET_SPKOUT_VOLUME,
    SET_SPDIFOUT_VOLUME,
    SET_HPOUT_VOLUME,
    SET_SCARTOUT_VOLUME,
    SET_AD_VOLUME,
    SET_DECODER_INPUT_MUTE,
    SET_MIXER_INPUT_MUTE,
    SET_PBOUT_MUTE,
    SET_TSOUT_MUTE,
    SET_SPKOUT_MUTE,
    SET_SPDIFOUT_MUTE,
    SET_HPOUT_MUTE,
    SET_SPKOUT_DELAY_TIME,
    SET_SPDIFOUT_DELAYTIME,
    SET_HPOUT_DELAY_TIME,
    SET_SCARTOUT_DELAY_TIME,
    SET_SPDIFOUT_TYPE,
    SET_ARC_OUTPUT_TYPE,
    SET_ATMOS_ENCODE,
    SET_SPDIF_COPY_INFO,
    SET_SPDIF_CATEGORY_CODE,
    SET_SIF_INPUT_SOURCE,
    SET_SIF_NO_SIGNAL_MUTE,
    SET_SIF_AUTO_CHANGE_SOUND_MODE,
    SET_SIF_AUTO_CHANGE_SOUND_STD,
    SET_SIF_FIRSTTIME_PLAYMONO,
    SET_SIF_FW_PRIORITY,
    SET_SIF_MIS_PRIORITY,
    SET_SIF_HIGH_DEVMODE,
    SET_SIF_BAND_SETUP,
    SET_SIF_MODE_SETUP,
    SET_SIF_USER_ANALOG_MODE,
    SET_SIF_A2_THRESHOLD_LEVEL,
    SET_SIF_NICAM_THRESHOLD_LEVEL,
    SET_SIF_AUDIO_EQ_MODE,
    SET_SIF_OVER_DEVIATION,
    SET_PCM_VOLUME,
    SET_PCM_VOLUME_BYPIN,
    SET_PCM_MUTE_BYPIN,
    SET_PAUSE_DECODING,
    SET_PAUSE_AND_STOP_DECODING,
    SET_RESUME_DECODING,
    SET_RESUME_AND_RUN_DECODING,
    SET_FLUSH_DECODING,
    SET_DBXTV_SOUND_EFFECT,
    SET_INPUT_VOLUME,
    SET_TRACK_MODE,
    SET_VOLUME_WITH_FADE,
    SET_SPDIF_ENABLE,
    SET_DTS_DECODER_PARAM,
    SET_ANALOG_ADJ_DB,
    SET_DOLBY_OTT_MODE,
    SET_SOUND_EFFECT_LEVEL,
    SET_LOW_LATENCY_MODE,
    SET_AEC_REFERENCE_SOURCE,
    SET_AI_SOUND_ENABLE,
    SET_BT_MIC_ENABLE,
    SET_AC4_AUTO_PRESENTATION_FIRST_LANGUAGE,
    SET_AC4_AUTO_PRESENTATION_SECOND_LANGUAGE,
    SET_AC4_AUTO_PRESENTATION_AD_MIXING,
    SET_AC4_AUTO_PRESENTATION_AD_TYPE,
    SET_AC4_AUTO_PRESENTATION_PRIORITIZE_AD_TYPE,
    SET_AC4_AUTO_PRESENTATION_GROUP_INDEX,
    SET_AC4_DIALOG_ENHANCEMENT_GAIN,
    SET_PRESCALE_VOL,
    SET_I2SIN_OPEN,
    SET_I2SIN_CLOSE,
    SET_I2SIN_CONNECT,
    SET_I2SIN_DISCONNECT,
    SET_UACIN_OPEN,
    SET_UACIN_CLOSE,
    SET_UACIN_CONNECT,
    SET_UACIN_DISCONNECT,
    SET_SPDIFIN_OPEN,
    SET_SPDIFIN_CLOSE,
    SET_SPDIFIN_CONNECT,
    SET_SPDIFIN_DISCONNECT,
    SET_UACOUT1_MUTE,
    SET_UACOUT2_MUTE,
    SET_UACOUT1_VOLUME,
    SET_UACOUT2_VOLUME,
    SET_SNDOUT_MIXSRC,
    SET_HDMI_FULLCAP_PORT,
    SET_SNDOUT_MAIN_DEVICE,
    SET_MULTIVIEW_MAIN_AUDIO_OUTPUT,
    SET_DP_CONNECT_PORT,
    SET_DP_DISCONNECT_PORT,
    SET_DP_OPEN_PORT,
    SET_DP_CLOSE_PORT,
    MAX_SET_SUB_CONTROL_NUM
}RTKAUDIO_SET_SUB_CONTROL_TYPE;

typedef enum {
    GET_DECODE_TYPE,
    GET_ADEC_STATUS,
    GET_TP_ES_INFO,
    GET_TP_AUDIO_PTS,
    GET_TP_AUDIO_PTS64,
    GET_TP_BUFFER_STATUS,
    GET_DECODER_INPUT_GAIN,
    GET_MIXER_INPUT_GAIN,
    GET_DECODER_INPUT_MUTE,
    GET_MIXER_INPUT_MUTE,
    GET_SPKOUT_MUTE_STATUS,
    GET_SPDIFOUT_MUTE_STATUS,
    GET_HPOUT_MUTE_STATUS,
    GET_SCARTOUT_MUTE_STATUS,
    GET_STATUS_INFO,
    GET_SNDOUT_LATENCY,
    GET_DIGITAL_INFO,
    GET_ES_INFO,
    GET_SIF_SOUND_INFO,
    GET_SIF_BAND_DETECT,
    GET_SIF_CHECK_NICAM_DIGIT,
    GET_SIF_CHECK_AVAILABLE_SYSTEM,
    GET_SIF_CHECK_A2DK,
    GET_SIF_A2_STEREO_LEVEL,
    GET_SIF_NICAM_THRESHOLD_LEVEL,
    GET_SIF_NICAM_STABLE,
    GET_SIF_CURRENT_ANALOG_MODE,
    GET_SIF_SOUND_STANDARD,
    GET_SIF_DETECT_COMPLETE,
    MAX_GET_SUB_CONTROL_NUM
}RTKAUDIO_GET_SUB_CONTROL_TYPE;

/* RTKAudio control status */
typedef enum {
    RESOURCE_STATUS,
    MAIN_FLOW_STATUS,
    SUB_FLOW_STATUS,
    AIN_STATUS,
    SE_STATUS,
    AIN_AGENT_STATUS,
    PPAO_AGENT_STATUS,
    DECODER_STATUS,
    MAIN_DECODER_STATUS,
    ALSA_DEV_INFO,
    GST_DEBUG_INFO,
    VIRTUALX_CMD_INFO,
    REGION_INFO,
    INIT_VOICEAP_CMD_Q,
    ACMD_INFO,
    VOICE_CREATE_HANDLE,
    TRUVOLUMEHD_CMD_INFO,
    GET_OPTEE_CARVEOUT_MEM_INFO,
    QUICKSHOW_CMD_INFO,
    EQ_BASS_CMD_INFO,
    SET_RHAL_INFO,
    GET_RHAL_INFO,
    SET_CHANNEL_OUT_SWAP,
    SET_SEND_SPECTRUM_DATA,
    SET_AUDIO_AUTH_KEY,
    SET_MODEL_HASH,
    SET_AUDIO_CAPABILITY,
    SET_AVSYNC_OFFSET,
    SET_INIT_RING_BUFFER_HEADER,
    SET_SPDIFOUT_PIN_SRC,
    SET_SWP_SRS_TRUSURROUNDHD,
    SET_TRUVOLUME,
    SET_TRUVOLUMEHD_PARAM,
    SET_VX_PARAM,
    SET_DBXTV_TABLE,
    SET_DAP_TUNING_DATA,
    GET_SIF_DETECT_SOUND_SYSTEM,
    GET_TP_IS_ES_EXIST,
    GET_HDMI_GET_COPY_INFO,
    GET_HDMI_AUDIO_MODE,
    GET_PRIVATE_INFO,
    GET_SIF_IS_EXIST,
    GET_SIF_DETECT_SOUND_SYSTEM_STD,
    GET_DAP_PARAM,
    GET_TRUVOLUMEHD_PARAM,
    GET_DBXTV_DEBUG_CMD,
    GET_VX_PARAM,
    AIDK_CMD_INFO,
    INIT_SPK_COUNTER_SHARE_BUF,
    UNINIT_SPK_COUNTER_SHARE_BUF,
    GET_SPK_COUNTER,
    MAX_CONTROL_NUM,
} RTKAUDIO_CONTROL_TYPE;

/* IOCTL return structure */
struct ret_info {
    unsigned int private_info[16];
};
typedef struct ret_info ret_info_t;

/* IOCTL set/get control structure */
struct cntl_info {
    int resource_type;
    unsigned int addr;
    int size;
};
typedef struct cntl_info cntl_info_t;

struct cntl_info_with_return {
    int resource_type;
    unsigned int addr;
    int size;
    int result;// ioctrl result
};
typedef struct cntl_info_with_return cntl_info_with_return_t;


/* GST info */
struct gst_info {
    int gst_open;
    unsigned long long decoded_size;
    unsigned long long undecoded_size;
};
typedef struct gst_info gst_info_t;

/* Virtual X cmd info */
struct virtualx_cmd_info {
    unsigned int index;
    unsigned int result;
    unsigned int type;
    unsigned int size;
    unsigned int data[60];
};
typedef struct virtualx_cmd_info virtualx_cmd_info;

/* Truvolume HD cmd info */
struct truvolumehd_cmd_info {
    int result;
    int type;
    int size;
    int data[60];
};

typedef struct truvolumehd_cmd_info truvolumehd_cmd_info;

typedef enum {
    ACMD_CREATE,
    ACMD_DESTROY,
    ACMD_INIT_RING_BUFFER_HRADER,
    ACMD_PP_INIT_PIN,
    ACMD_PRIVATE_INFO,
    ACMD_SET_REF_CLOCK,
    ACMD_SWITCH_FOCUS,
    ACMD_CONNECT,
    ACMD_RUN,
    ACMD_PAUSE,
    ACMD_FLUSH,
    ACMD_STOP,
    ACMD_SET_DUAL_MONO_OUTMODE,
    ACMD_GET_AUDIO_FORMAT_INFO,
    ACMD_SET_SEEKING,
    ACMD_CREATE_HAL_DECODER,
    ACMD_DESTROY_HAL_DECODER,
    ACMD_INIT_HAL_DECODER_OUT_RING_BUFFER_HRADER,
    ACMD_INIT_HAL_DECODER_ICQ_RING_BUFFER_HRADER,
    ACMD_INIT_HAL_DECODER_DSQ_RING_BUFFER_HRADER,
    ACMD_DISCONNECT,
    ACMD_INIT_RING_BUFFER_HRADER_WITH_PID,
} RTKAUDIO_ACMD_TYPE;

/* OMX CMD info */
struct omx_rpc_cmd {
    int cmd_type;
    unsigned int addr;
    int size;
};
typedef struct omx_rpc_cmd omx_rpc_cmd_t;

struct quickshow_cmd_info
{
    unsigned int result;
    unsigned int type;
    unsigned int size;
    unsigned int data[25];
};
typedef struct quickshow_cmd_info quickshow_cmd_info;

struct eq_bass_cmd_info {
    unsigned int type;
    unsigned int size;
    unsigned int data[6];
};
typedef struct eq_bass_cmd_info eq_bass_cmd_info;

struct wifi_speaker_info {
    long long timestamp;
    long long system_time;
    int base_tick;
    int overflow_count;
};
typedef struct wifi_speaker_info wifi_speaker_info;

typedef enum {
    AIDK_CHANNEL_MODE,          /* size: 1 * unsigned int */
    AIDK_ATMOS_LOCK,            /* size: 1 * unsigned int */
    AIDK_STEREO_OUT_DRC,        /* size: 4 * unsigned int */
    AIDK_MULTI_OUT_DRC,         /* size: 3 * unsigned int */
    AIDK_AAC_PRL,               /* size: 1 * unsigned int */
    AIDK_AC4_DE,                /* size: 1 * unsigned int */
    AIDK_DAP_DE,                /* size: 1 * unsigned int */
    AIDK_VIRTUALIZER_MODE,      /* size: 2 * unsigned int */
    AIDK_VOLUME_LEVELER_MODE,   /* size: 1 * unsigned int */
    AIDK_VOLUME_LEVELER_AMOUNT, /* size: 1 * unsigned int */
    AIDK_CONTINUOUS_OUT,        /* size: 1 * unsigned int */
    AIDK_AUDIO_DUMP,            /* size: 1 * unsigned int */
    AIDK_AUDIO_DUMP_MODE,       /* size: 1 * unsigned int */
    AIDK_AC4_PRES_PREF,         /* size: 4 * unsigned int */
    AIDK_DAP_SPK_VIRT_ANGLE,    /* size: 1 * unsigned int */
    AIDK_DAP_SPK_VIRT_FREQ,     /* size: 1 * unsigned int */
    AIDK_DAP_VIRT_SURR_BOOST,   /* size: 1 * unsigned int */
    AIDK_AD_ON,                 /* size: 1 * unsigned int */
    AIDK_ATMOS_ON,              /* size: 1 * unsigned int */
    AIDK_DAP_SET_TUNING_FILE,   /* size: by input file size */
    AIDK_AC4_MIX_LEVEL,         /* size: 2 * unsigned int */
    AIDK_SET_BS_PASSTHROUGH,    /* size: 1 * unsigned int */
    AIDK_GET_HIGHEST_CAP,       /* size: 1 * unsigned int */
    AIDK_GET_ATMOS_INDICATOR,   /* size: 1 * unsigned int */
    AIDK_AC4_PRESENTATION_ID,   /* size: 1 * int */
    AIDK_AC4_SHORT_PROGRAM_ID,  /* size: 1 * int */
    AIDK_DAP_SET_PARAMS,        /* size: DAP_PARAM defined in AudioInbandAPI.h */
    AIDK_DAP_GET_PARAMS,        /* size: DAP_PARAM defined in AudioInbandAPI.h */
    AIDK_FLAP_SET_SCONF_FILE,   /* size: by input file size */
    AIDK_FLAP_SET_DCONF_FILE,   /* size: by input file size */
    AIDK_ENABLE_FLEX_MODE,      /* size: 1 * unsigned int */
    AIDK_ENFORCE_SINGLE_OA_ELEMENT, /* size: 1 * unsigned int */
    AIDK_SET_FLEXR_CENTER_EXTRACT_MODE, /* size: 1 * int */
    AIDK_INFO_MAX,
} AIDK_INFO_TYPE;

typedef enum {
    AIDK_AUTO,
    AIDK_FIVE_POINT_ONE,
} AIDK_OUTPUT_CHANNEL_MODE;

typedef enum {
    AIDK_DRC_LINE,
    AIDK_DRC_RF,
} AIDK_DRC_MODE;

typedef enum {
    AIDK_DOWNMIX_LTRT,
    AIDK_DOWNMIX_LORO,
    AIDK_DOWNMIX_ARIB,
} AIDK_DOWNMIX_MODE;

typedef enum {
    AIDK_SPEAKER,
    AIDK_HEADPHONE,
} AIDK_DEVICE_TYPE;

typedef enum {
    VIRTUAL_DISABLE,
    VIRTUAL_ENABLE,
    VIRTUAL_ENABLE_FOR_ATMOS,
} AIDK_VIRT_MODE;

typedef enum {
    VL_DISABLE,
    VL_ENABLE,
    VL_ENABLE_FOR_UNLEVELED,
} AIDK_VL_MODE;

typedef enum {
    AIDK_DUMP_WRITE,
    AIDK_DUMP_APPEND,
} AIDK_DUMP_MODE;

typedef enum {
    AIDK_AC4_PRES_LANGUAGE,
    AIDK_AC4_PRES_ASSOCIATED_TYPE,
} AIDK_AC4_PRES_MODE;

typedef enum {
    AIDK_PRES_ASSOC_NONE,
    AIDK_PRES_ASSOC_VISUALLY_IMPAIRED,
    AIDK_PRES_ASSOC_HEARING_IMPAIRED,
    AIDK_PRES_ASSOC_COMMENTARY,
} AIDK_PRES_ASSOC_TYPE;

typedef enum {
    AIDK_NORMAL,
    AIDK_MUTE_MAIN,
    AIDK_MUTE_ASSOCIATED,
} AIDK_MIX_LEVEL_TYPE;

typedef enum {
    AIDK_HIGHEST_CAP_STEREO,
    AIDK_HIGHEST_CAP_SURROUND,
    AIDK_HIGHEST_CAP_ATMOS,
} AIDK_HIGHEST_CAP;

struct aidk_cmd_info {
    unsigned int type; /* enum AIDK_INFO_TYPE */
    unsigned int size;
    unsigned int addr;
};
typedef struct aidk_cmd_info aidk_cmd_info;

struct aidk_stereo_drc {
    unsigned int boost;
    unsigned int cut;
    AIDK_DRC_MODE drc_mode;
    AIDK_DOWNMIX_MODE dmx_mode;
};
typedef struct aidk_stereo_drc aidk_stereo_drc;

struct aidk_multiout_drc {
    unsigned int boost;
    unsigned int cut;
    AIDK_DRC_MODE drc_mode;
};
typedef struct aidk_multiout_drc aidk_multiout_drc;

struct aidk_virt_mode {
    AIDK_DEVICE_TYPE dev;
    AIDK_VIRT_MODE mode;
};
typedef struct aidk_virt_mode aidk_virt_mode;

struct aidk_ac4_pref {
    AIDK_AC4_PRES_MODE pres_mode;
    AIDK_PRES_ASSOC_TYPE pres_assoc_type;
    char first_lang[4];
    char second_lang[4];
};
typedef struct aidk_ac4_pref aidk_ac4_pref;

struct aidk_ac4_mixlevel {
    AIDK_MIX_LEVEL_TYPE type;
    unsigned int level;
};
typedef struct aidk_ac4_mixlevel aidk_ac4_mixlevel;
#endif

