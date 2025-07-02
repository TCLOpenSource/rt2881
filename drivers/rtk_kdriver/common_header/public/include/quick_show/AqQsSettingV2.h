#ifndef __AQ_QS_SETTING_V2_H__
#define __AQ_QS_SETTING_V2_H__
#include "StructSignature.h"

#define QS_AQ_SRC_MAX_PORT_NUM 4
#define QS_SHA256_SIZE 32
#define MS_MIN_MAX_CHANNEL     6                 /**< Minimum value for max. number of channels        */
#define MS_MAX_MAX_CHANNEL     8                 /**< Maximum value for max. number of channels        */
#define DAP_MAX_BANDS          (20)
#define DAP_IEQ_MAX_BANDS      DAP_MAX_BANDS
#define DAP_GEQ_MAX_BANDS      DAP_MAX_BANDS
#define DAP_REG_MAX_BANDS      DAP_MAX_BANDS
#define DAP_OPT_MAX_BANDS      DAP_MAX_BANDS
#define DAP_MAX_CHANNELS       (MS_MAX_MAX_CHANNEL)
#define AQ_QS_V2_MAGIC          0x51415351 //'Q' 'S' 'A' 'Q'

#define QS_CH_PAIR_NUM (10)

DEFINE_STRUCTURE(QS_AVC_CFG_INTERNAL,
    // identical to the order in register's field
    unsigned  vol_detect:2;   // Clayton 2013/8/23
    unsigned  noise_gain:2;     // refer to enum of APP_AVC_NOISE_GAIN
    unsigned  noise_thd:8;
    uint32_t  level_max;
    uint32_t  level_min;
    unsigned  step_up:2;      // refer to enum of APP_AVC_STEP_GAIN
    unsigned  step_down:2;      // refer to enum of APP_AVC_STEP_GAIN

    // identical to the order in register's field
    unsigned  thd_cnt:10;
    unsigned  win_zcross:10;
    unsigned  win_moniter:10;

    // identical to the order in register's field
    unsigned  avc_vol_amplif_enable:1;
    unsigned  avc1_coef:3;
    uint32_t  avc_small_level_max;
    uint32_t  avc_small_level_min;
    unsigned  avc_small_down_db:2;
    unsigned  avc_small_up_db:2;
    unsigned  avc_small_cnt_thre:8;

    // identical to the order in register's field
    unsigned  avc_lower_step_mag_thre:8;
    unsigned  avc_amplif_step_mag_thre:8;
    uint32_t  avc_small_level:16;

    // identical to the order in register's field  <AVC_gain_step_control_0>
    unsigned  avc_lower_step_cnt_low_thre:8;
    unsigned  avc_lower_step_cnt_hi_thre:8;
    unsigned  avc_amplif_step_cnt_low_thre:8;
    unsigned  avc_amplif_step_cnt_hi_thre:8;

    // identical to the order in register's field  <AVC_gain_step_control_1>
    unsigned  reserved:24;
    unsigned  avc_down_db_fine:2;
    unsigned  avc_up_db_fine:2;
    unsigned  avc_small_down_db_fine:2;
    unsigned  avc_small_up_db_fine:2;
)

DEFINE_STRUCTURE(QS_AVC_CFG,
    QS_AVC_CFG_INTERNAL avcCfgOn;
    QS_AVC_CFG_INTERNAL avcCfgOff;
)

DEFINE_STRUCTURE(QS_AVC2_CFG,
    // identical to the order in register's field
    unsigned  res1:1;      // Clayton 2013/1/29
    unsigned  avc2_vol_amplif_enable:1;
    unsigned  avc2_lpf_coef:3;
    unsigned  avc2_vol_detect:2;
    unsigned  fast_recov_ctrl:1;
    unsigned  slow_up_db:2;
    unsigned  fast_up_db:2;
    unsigned  down_db:2;
    unsigned  avc2_noise_gain:2;  // refer to enum of APP_AVC_NOISE_GAIN
    unsigned  avc2_noise_thre:16;

    // identical to the order in register's field
    unsigned  avc2_cnt_thre:10;
    unsigned  avc2_zc_win:10;
    unsigned  avc2_mon_win:10;

    // identical to the order in register's field
    unsigned  avc2_noise_thre_hi:16;
    unsigned  recov_win_max:8;
    unsigned  recov_win_min:8;

    // identical to the order in register's field
    uint32_t avc2_level_max:16;
    uint32_t avc2_level_min:16;

    // identical to the order in register's field  <AVC2_Control_5_ch01 >
    unsigned  avc2_channel:10;
    unsigned  slow_up_db_fine:2;
    unsigned  fast_up_db_fine:2;
    unsigned  down_db_fine:2;
    unsigned  avc2_small_up_db_fine:2;
    unsigned  avc2_small_down_db_fine:2;
    unsigned  avc2_small_up_db:2;
    unsigned  avc2_small_down_db:2;
    unsigned  avc2_small_cnt_thre:8;

    // identical to the order in register's field
    uint32_t avc2_small_level_max:16;
    uint32_t avc2_small_level_min:16;

    // identical to the order in register's field
    uint32_t res4:1;
    uint32_t avc2_small_level:31;
)

DEFINE_STRUCTURE(QS_LOUD_SET_BAND_ATTR,
    double FreqC;
    double BandWidth;
    double Gain;
)

typedef enum {
    QS_EQ_SAMPLE_FREQ_32K = 32000,
    QS_EQ_SAMPLE_FREQ_44K = 44100,
    QS_EQ_SAMPLE_FREQ_48K = 48000,
    QS_EQ_SAMPLE_FREQ_96K = 96000,
    QS_EQ_SAMPLE_FREQ_88K = 88200,
    QS_EQ_SAMPLE_FREQ_176K = 176000,
    QS_EQ_SAMPLE_FREQ_192K = 192000,
} QS_EQ_SAMPLE_FREQ;

DEFINE_STRUCTURE(QS_BASS,
    QS_LOUD_SET_BAND_ATTR attr;
    QS_EQ_SAMPLE_FREQ sampleFreq;
)

DEFINE_STRUCTURE(QS_TREBLE_BAND,
    QS_LOUD_SET_BAND_ATTR attr;
    QS_EQ_SAMPLE_FREQ sampleFreq;
)


DEFINE_STRUCTURE(QS_TREBLE_BAND2,
    QS_LOUD_SET_BAND_ATTR attr;
    QS_EQ_SAMPLE_FREQ sampleFreq;
)

DEFINE_STRUCTURE(QS_SE_CFG,
    // identical to the order in register's field
    unsigned  resvd1:17;
    unsigned  apf_en:1;
    unsigned  psb_en:1;
    unsigned  seb_en:1;
    unsigned  sel_l:1;
    unsigned  sel_r:1;
    unsigned  resvd2:1;
    unsigned  apf_coef:9;
    // identical to the order in register's field
    unsigned  resvd3:7;
    unsigned  depth1:9;
    unsigned  resvd4:7;
    unsigned  depth2:9;
)

DEFINE_STRUCTURE(QS_SPA_SETTING,
    int isSurround;
    QS_SE_CFG surrondCfg;
)

DEFINE_STRUCTURE(QS_DAP_PARAM_DIALOGUE_ENHANCER,
    int de_enable;
    int de_amount;

    /* for MS12_v1 */
    int de_ducking;
)

DEFINE_STRUCTURE(QS_DAP_PARAM_VOLUME_LEVELER,
    int leveler_amount;

    /* for MS12_v1 */
    int leveler_enable;
    int leveler_ignore_il;

    /* for MS12_v2 */
    int leveler_setting;

    int leveler_input;
    int leveler_output;
)

DEFINE_STRUCTURE(QS_DAP_PARAM_INTELLIGENT_EQUALIZER,
    int ieq_enable;
    int ieq_amount;
    int ieq_nb_bands;
    int a_ieq_band_center[DAP_IEQ_MAX_BANDS];
    int a_ieq_band_target[DAP_IEQ_MAX_BANDS];
)

DEFINE_STRUCTURE(QS_DAP_PARAM_MEDIA_INTELLIGENCE,
    int mi_ieq_enable;
    int mi_dv_enable;
    int mi_de_enable;
    int mi_surround_enable;
)

DEFINE_STRUCTURE(QS_DAP_PARAM_SURROUND_VIRTUALIZER,
    int speaker_angle;
    int surround_boost;

    /* for MS12_v1 */
    int virtualizer_enable;
    int headphone_reverb;
    int speaker_start;

    /* for MS12_v2 */
    int virtualizer_mode;
)

DEFINE_STRUCTURE(QS_DAP_PARAM_GRAPHICAL_EQUALIZER,
    int eq_enable;
    int eq_nb_bands;
    int a_geq_band_center[DAP_GEQ_MAX_BANDS];
    int a_geq_band_target[DAP_GEQ_MAX_BANDS];
)

DEFINE_STRUCTURE(QS_DAP_PARAM_AUDIO_OPTIMIZER,
    int optimizer_enable;
    int optimizer_nb_bands;
    int a_opt_band_center_freq[DAP_OPT_MAX_BANDS];
    int a_opt_band_gain[DAP_MAX_CHANNELS][DAP_OPT_MAX_BANDS];
)

DEFINE_STRUCTURE(QS_DAP_PARAM_AUDIO_REGULATOR,
    int reg_nb_bands;
    int a_reg_band_center[DAP_REG_MAX_BANDS];
    int a_reg_low_thresholds[DAP_REG_MAX_BANDS];
    int a_reg_high_thresholds[DAP_REG_MAX_BANDS];
    int a_reg_isolated_bands[DAP_REG_MAX_BANDS];

    int regulator_overdrive;
    int regulator_timbre;
    int regulator_distortion;
    int regulator_mode;
    int regulator_enable;
)

DEFINE_STRUCTURE(QS_DAP_PARAM_BASS_ENHANCER,
    int bass_enable;
    int bass_boost;
    int bass_cutoff;
    int bass_width;
)

DEFINE_STRUCTURE(QS_DAP_PARAM_VIRTUAL_BASS,
    int b_virtual_bass_enabled;
    int vb_mode;
    int vb_low_src_freq;
    int vb_high_src_freq;
    int vb_overall_gain;
    int vb_slope_gain;
    int vb_subgain[3];
    int vb_mix_low_freq;
    int vb_mix_high_freq;
)

DEFINE_STRUCTURE(QS_DAP_PARAM,
    int drc_type;
    int speaker_lfe;
    int pregain;
    int postgain;
    int systemgain;
    int surround_decoder_enable;
    int height_filter_mode;
    int calibration_boost;
    int leveler_input;
    int leveler_output;
    int modeler_enable;
    int modeler_calibration;
    int volmax_boost;

    /* QS_DAP_PARAM_DIALOGUE_ENHANCER This item is not used for the current Dolby Audio design.*/
    /* Please use QS_DIALOG_ENHANCEMENT */
    QS_DAP_PARAM_DIALOGUE_ENHANCER dialogue_enhancer;
    QS_DAP_PARAM_VOLUME_LEVELER volume_leveler;
    QS_DAP_PARAM_INTELLIGENT_EQUALIZER intelligent_equalizer;
    QS_DAP_PARAM_MEDIA_INTELLIGENCE media_intelligence;
    QS_DAP_PARAM_SURROUND_VIRTUALIZER surround_virtualizer;
    QS_DAP_PARAM_GRAPHICAL_EQUALIZER graphical_equalizer;
    QS_DAP_PARAM_AUDIO_OPTIMIZER audio_optimizer;
    QS_DAP_PARAM_AUDIO_REGULATOR audio_regulator;
    QS_DAP_PARAM_BASS_ENHANCER bass_enhancer;
    QS_DAP_PARAM_VIRTUAL_BASS virtual_bass;
)

DEFINE_STRUCTURE(QS_DAP_EXTEND_PARAM,
    /* for MS12_v2.6 */
    int mi_virtualizer_enable;
    int mi_steering_enable;
    int center_spreading_enable;
    int active_downmix_enable;
)

typedef enum {
    QS_EQ_ALL_LPF_BAND1 = 0,
    QS_EQ_ALL_BPF_BAND1 = 1,
    QS_EQ_ALL_BPF_BAND2 = 2,
    QS_EQ_ALL_BPF_BAND3 = 3,
    QS_EQ_ALL_BPF_BAND4 = 4,
    QS_EQ_ALL_BPF_BAND5 = 5,
    QS_EQ_ALL_BPF_BAND6 = 6,
    QS_EQ_ALL_BPF_BAND7 = 7,
    QS_EQ_ALL_BPF_BAND8 = 8,
    QS_EQ_ALL_BPF_BAND9 = 9,
    QS_EQ_ALL_BPF_BAND10 = 10,
    QS_EQ_ALL_BPF_BAND11 = 11,
    QS_EQ_ALL_BPF_BAND12 = 12,
    QS_EQ_ALL_BPF_BAND13 = 13,
    QS_EQ_ALL_BPF_BAND14 = 14,
    QS_EQ_ALL_HPF_BAND1 = 15,
} QS_EQ_BPF_ALL_BAND;

typedef enum {
    QS_EQ_HPF_SLOPE_20DB = 0,
    QS_EQ_HPF_SLOPE_40DB = 1,
} QS_EQ_HPF_SLOPE;

DEFINE_STRUCTURE(QS_EQ_SET_BAND_ATTR,
    double FreqC;
    double BandWidth;
    double Gain;
    QS_EQ_HPF_SLOPE hp_slope;
)

DEFINE_STRUCTURE(QS_EQ_BAND_FILTER,
    // BY PASS FILTER will be decided by sndMode including personal and others
    // If isSet is not set, we should not update to audio fw
    bool isSet[QS_EQ_ALL_HPF_BAND1+1];
    QS_EQ_SET_BAND_ATTR eqBandAttr[QS_EQ_ALL_HPF_BAND1+1];
    QS_EQ_SAMPLE_FREQ eqSampleFreq[QS_EQ_ALL_HPF_BAND1+1];
)

DEFINE_STRUCTURE(QS_EQ_SETTING,
    int isClarity;
    QS_EQ_SAMPLE_FREQ claritySampleFreq;
    QS_EQ_SET_BAND_ATTR clarityBand;
    QS_EQ_BAND_FILTER eqBandFilter;
)

DEFINE_STRUCTURE(QS_DAP_SETTING,
    int dapBassExtEnable;
    QS_DAP_PARAM param;
)

DEFINE_STRUCTURE(QS_DIALOG_ENHANCEMENT,
    bool enable;
    int gain;
)

DEFINE_STRUCTURE(QS_AQ_DATA,
    // AVC
    QS_AVC_CFG avcCfg[QS_AQ_SRC_MAX_PORT_NUM];

    // EQ
    QS_EQ_SETTING eqSetting[QS_AQ_SRC_MAX_PORT_NUM];

    // BASS
    QS_BASS bass[QS_AQ_SRC_MAX_PORT_NUM];

    // TREBLE
    QS_TREBLE_BAND trebleBand1[QS_AQ_SRC_MAX_PORT_NUM];
    QS_TREBLE_BAND2 trebleBand2[QS_AQ_SRC_MAX_PORT_NUM];

    // SPA
    QS_SPA_SETTING spaSetting[QS_AQ_SRC_MAX_PORT_NUM];

    // DAP
    QS_DAP_SETTING dapSetting[QS_AQ_SRC_MAX_PORT_NUM];

    // DIALOG_ENHANCEMENT
    QS_DIALOG_ENHANCEMENT dialogEnhancement[QS_AQ_SRC_MAX_PORT_NUM];

    // DAP EXTEND PARAM
    QS_DAP_EXTEND_PARAM dapExtendParam[QS_AQ_SRC_MAX_PORT_NUM];

    // reserved bytes for compatibility
    uint32_t reserved[112];
)

// This structure is compatible with QS_AQ_DATA for the shared memory's current source(and port).
DEFINE_STRUCTURE(QS_AQ_DATA_ITEM,
    // AVC
    QS_AVC_CFG avcCfg;

    // EQ
    QS_EQ_SETTING eqSetting;

    // BASS
    QS_BASS bass;

    // TREBLE
    QS_TREBLE_BAND trebleBand1;
    QS_TREBLE_BAND2 trebleBand2;

    // SPA
    QS_SPA_SETTING spaSetting;

    // DAP
    QS_DAP_SETTING dapSetting;

    // DIALOG_ENHANCEMENT
    QS_DIALOG_ENHANCEMENT dialogEnhancement;

    // DAP EXTEND PARAM
    QS_DAP_EXTEND_PARAM dapExtendParam;

    // reserved bytes for compatibility
    uint32_t reserved[27];
)

DEFINE_STRUCTURE(AQ_QsSettingHeader,
    // MAGIC {Q,S,A,Q}
    uint32_t magic;

    // size of AQ_QsSettingsData
    int dataSize;

    // SHA-256 of AQ_QsSettingsData
    uint8_t checksum[QS_SHA256_SIZE];

    // reserved
    uint32_t reserved[32];
)

typedef enum {
    QS_AUDIO_EFFECT_AVC2_CH01 = 0,
    QS_AUDIO_EFFECT_AVC2_CH23,
    QS_AUDIO_EFFECT_AVC2_CH45,
    QS_AUDIO_EFFECT_AVC2_CH67,
    QS_AUDIO_EFFECT_AVC2_MAX_NUM,
} QS_AUDIO_EFFECT_AVC2_INDEX;

DEFINE_STRUCTURE(AQ_QsSettingsData,
    // AVC
    QS_AVC2_CFG avc2Cfg[QS_AUDIO_EFFECT_AVC2_MAX_NUM];

    // SPA (surround and balance)
    int spkOutBalanceDbGain;
    int hpOutBalanceDbGain;

    // DAP
    int dapEnable;

    // AQ On/Off;
    unsigned char aqDisable;

    // AVC On/Off
    unsigned char avcEnable;

    // Force to rebuild the AQ bin if the version does no match to the QsSettingMgrAQV2::version[2]
    // version[0]: date (e.g., 20240510)
    // version[1]: update content (e.g., dap)
    unsigned int version[2];

    unsigned char dac0Enable;
    unsigned char dac1Enable;
    unsigned char i2s0Enable;
    unsigned char reserved1;

    int dac0Value;
    int dac1Value;
    int i2s0Value;

    unsigned char reserved2;
    unsigned char reserved3;

    // reserved bytes for compatibility,
    uint32_t reserved[25];

    // Depend on source
    QS_AQ_DATA aqHdmiData;
    QS_AQ_DATA aqDpData;
)

DEFINE_STRUCTURE(AQ_QsSettingsV2,
    AQ_QsSettingHeader header;
    AQ_QsSettingsData data;
)

DEFINE_STRUCTURE(AQ_QsDapBinHeader,
    // MAGIC {Q,S,A,Q}
    uint32_t magic;

    // size of dap bin
    int dataSize;

    // SHA-256 of dap bin
    uint8_t checksum[QS_SHA256_SIZE];

    // reserved
    uint32_t reserved[32];
)

DEFINE_STRUCTURE(AQ_QsDapBin,
    AQ_QsDapBinHeader header;
    char* binData;
)

DEFINE_STRUCTURE(QS_AQ_DAP_TUNINGDATA,
    char            p_tuning[16402];
    int             size;
)

DEFINE_STRUCTURE(QS_AQ_EFFECT_MEM,
    unsigned int            magic;
    unsigned int            version;
    unsigned int            share_info_size;
    unsigned char           share_info_sha256[32];
    unsigned int            setbit;
    unsigned int            qs_aq_disable;
    int                     reserved[32];                  // total = 180 bytes
    int                     qs_vol[QS_CH_PAIR_NUM];//ch_id (0:ch0/1, 1:ch2/3, 2:ch4/5, 3:ch6/7, 4:SPDIF/ARC)
    int                     qs_mute[QS_CH_PAIR_NUM];
    int                     qs_prescale[QS_CH_PAIR_NUM];
    int                     qs_balance[QS_CH_PAIR_NUM];   //  total = 180 + 160 = 340 bytes
    QS_AQ_DAP_TUNINGDATA    qs_dap_tuningdata;            //  total = 180 + 160 + 16408 = 16748 bytes
    QS_AQ_DATA_ITEM         qs_aq_data;                   //  total = 180 + 160 + 16408 + (offset(0x33c) + 4 DAP param) = 17580 bytes
)

#endif // __AQ_QS_SETTING_V2_H__
