/***************************************************************************************************
  File        : tv_fe_types.h
  Description : type definitions for TV Frontend Control
  Author      : Kevin Wang (kevin_wang@realtek.com.tw)
****************************************************************************************************
    Update List :
----------------------------------------------------------------------------------------------------
    1.0b    |   20080721    | Modify SEC_FILTER_PARAM - Add Complete & pContext fof Section Data Delivery
----------------------------------------------------------------------------------------------------
    2.0     |   20080804    | Modify Return Value of Section Complete callback
            |               |     return sec_data
***************************************************************************************************/
#ifndef __TV_FE_TYPES_H__
#define __TV_FE_TYPES_H__

//#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FALSE
#      define  FALSE   (0)
#endif

/* support diff arch(arm64 or another) */
#ifdef CONFIG_ARM64
typedef unsigned int U32BITS;
typedef int S32BITS;
#define PT_U32BITS "%u"
#define PT_S32BITS "%d"
#define PT_H32BITS "%x"
#else
typedef unsigned long U32BITS;
typedef long S32BITS;
#define PT_U32BITS "%lu"
#define PT_S32BITS "%ld"
#define PT_H32BITS "%lx"
#endif

// Constants
#define TUNER_CTRL_OK               (0)
#define TUNER_CTRL_FAIL             (-1)
#define TUNER_CTRL_ENODEV           (-2)
#define TUNER_CTRL_BUFFER_FULL      (-3)
#define TUNER_CTRL_NO_SIGNAL        (-4)
#define TUNER_CTRL_SYNC_LOCK	    (-5)
#define TUNER_CTRL_FUNC_NOT_FINISH	    (-6)

#define DTV_SIGNAL_LOCK           1
#define DTV_SIGNAL_NOT_LOCK       0

#define DTV_BANDWIDTH_6M          6
#define DTV_BANDWIDTH_7M          7
#define DTV_BANDWIDTH_8M          8

enum {
	DTV_STREAM_START = 0,       // Start streming
	DTV_STREAM_STOP,            // Stop streaming
	DTV_STREAM_FLUSH,           // Flush all data in the buffer
	DTV_STREAM_MUTE,            // mute TS output (In this mode, all data will be ingored)
	DTV_STREAM_UNMUTE,          // mute TS output
	DTV_STREAM_CTRL_MAX,
};

#define TV_UPDATE_INTERVAL_INFINITY   0


/////////////////////////////////////////////////////
// Operation Mode
/////////////////////////////////////////////////////
typedef enum {
	TV_OP_MODE_NORMAL,
	TV_OP_MODE_SCANING,
	TV_OP_MODE_STANDBY,
	TV_OP_MODE_ATV_NORMAL,
	TV_OP_MODE_TP_ONLY,     // disalbe any frontned access except tp
} TV_OP_MODE;

/////////////////////////////////////////////////////
// Frontend Status
/////////////////////////////////////////////////////
typedef enum {
	FRONTEND_STATE_IDEL,
	FRONTEND_STATE_STANDBY,
	FRONTEND_STATE_UNLOCK,
	FRONTEND_STATE_LOCKED,
	FRONTEND_STATE_SCANNING,
} FRONTEND_STATE;

/////////////////////////////////////////////////////
// TV System
/////////////////////////////////////////////////////
typedef enum {
	// unknown
	TV_SYS_UNKNOWN,

	TV_SYS_ANALOG_SYS_MIN,
	// PAL
	TV_SYS_BG_PAL,
	TV_SYS_I_PAL,
	TV_SYS_DK_PAL,
	TV_SYS_BG_SECAM,
	TV_SYS_DK_SECAM,
	TV_SYS_L_SECAM,
	TV_SYS_LL_SECAM,

	// NTSC
	TV_SYS_M_NTSC,
	TV_SYS_M_PAL,
	TV_SYS_N_PAL, /* 10 */
	TV_SYS_ANALOG_SYS_MAX,

	//===========================
	// Digital Tv Standard Start
	//===========================
	TV_SYS_DIG_SYS_MIN,

	// DVB
	TV_SYS_DVBT_6M,
	TV_SYS_DVBT_7M,
	TV_SYS_DVBT_8M,

	TV_SYS_DVBC_6M,
	TV_SYS_DVBC_7M,
	TV_SYS_DVBC_8M,

	TV_SYS_DVBT2_1D7M,
	TV_SYS_DVBT2_5M,
	TV_SYS_DVBT2_6M,
	TV_SYS_DVBT2_7M, /* 20 */
	TV_SYS_DVBT2_8M,

	TV_SYS_DVBT2_1D7M_PLP,
	TV_SYS_DVBT2_5M_PLP,
	TV_SYS_DVBT2_6M_PLP,
	TV_SYS_DVBT2_7M_PLP,
	TV_SYS_DVBT2_8M_PLP,

	TV_SYS_AUTOSCAN_6M,
	TV_SYS_AUTOSCAN_7M,
	TV_SYS_AUTOSCAN_8M,

	TV_SYS_DVBS,
	TV_SYS_DVBS2, /* 30 */
	TV_SYS_AUTO_SATELLITE,

	// ATSC
	TV_SYS_ATSC,
	TV_SYS_OPENCABLE_64,
	TV_SYS_OPENCABLE_256,

	// DTMB
	TV_SYS_DTMB,
	TV_SYS_DTMB_6M,
	TV_SYS_DTMB_8M,

	// ISDB_T
	TV_SYS_ISDBT_6M,
	TV_SYS_ISDBT_7M,
	TV_SYS_ISDBT_8M,

	// ISDB-S
	TV_SYS_ISDBS_6M,

	// ABS
	TV_SYS_ABS_S,

	//ATSC 3p0
	TV_SYS_ATSC3P0_6M,
	TV_SYS_ATSC3P0_7M,
	TV_SYS_ATSC3P0_8M,

	//Ext demod ISDBT/S/S3
	TV_SYS_ISDBT_CXD2856,
	TV_SYS_ISDBT_CXD2878,
	TV_SYS_ISDBS_CXD2856,
	TV_SYS_ISDBS_CXD2878,
	TV_SYS_ISDBS3_CXD2878,

	TV_SYS_DIG_SYS_MAX,
	//===========================
	// Digital Tv Standard END....
	//===========================

	// pseudo TV systems
	TV_SYS_FILE,
	TV_SYS_MMTS,
	TV_SYS_RTSP,
	TV_SYS_CURRENT,             // current tv system
	TV_SYS_MAX_NUM

} TV_SYSTEM;

#define IsRawTSSys(x)           ((x >= TV_SYS_FILE)     && (x <= TV_SYS_MMTS))
#define IsAnalogSys(x)          ((x >= TV_SYS_ANALOG_SYS_MIN) && (x <= TV_SYS_ANALOG_SYS_MAX))
#define IsDigitalSys(x)         ((x > TV_SYS_DIG_SYS_MIN)  && (x < TV_SYS_DIG_SYS_MAX))
#define IsDvbSys(x)             ((x >= TV_SYS_DVBT_6M)  && (x <= TV_SYS_DVBC_8M))
#define IsDvbtSys(x)            ((x >= TV_SYS_DVBT_6M)  && (x <= TV_SYS_DVBT_8M))
#define IsDvbt2Sys(x)           ((x >= TV_SYS_DVBT2_1D7M) && (x <= TV_SYS_AUTOSCAN_8M))
#define IsDvbtEx(x)             ((x >= TV_SYS_AUTOSCAN_6M) && (x <= TV_SYS_AUTOSCAN_8M))
#define IsDvbcSys(x)            ((x >= TV_SYS_DVBC_6M)  && (x <= TV_SYS_DVBC_8M))
#define IsDvbsSys(x)            (x == TV_SYS_DVBS)
#define IsDvbs2Sys(x)           ((x >= TV_SYS_DVBS2) && (x <= TV_SYS_AUTO_SATELLITE))
#define IsAbssSys(x)            (x == TV_SYS_ABS_S)
#define IsDtmbSys(x)            ((x >= TV_SYS_DTMB) && (x <= TV_SYS_DTMB_8M))
#define IsAtscSys(x)            (x == TV_SYS_ATSC)
#define IsAtsc3p0Sys(x)            ((x >= TV_SYS_ATSC3P0_6M) && (x <= TV_SYS_ATSC3P0_8M))
#define IsIsdbtSys(x)           ((x >= TV_SYS_ISDBT_6M) && (x <= TV_SYS_ISDBT_8M))
#define IsOpenCableSys(x)       ((x >= TV_SYS_OPENCABLE_64)    && (x <= TV_SYS_OPENCABLE_256))
#define IsMNSys(x)              ((x >= TV_SYS_M_NTSC)  && (x <= TV_SYS_N_PAL))
#define IsRTSPSys(x)            (x == TV_SYS_RTSP)
#define IsExtIsdbSys(x)			((x >= TV_SYS_ISDBT_CXD2856) && (x <= TV_SYS_ISDBS3_CXD2878))

#define IsDigitalSysType(x)     ((x == TV_SYS_TYPE_ABS_S) || (x <= TV_SYS_TYPE_DVBS2) ||(x == TV_SYS_TYPE_DVBS)||(x == TV_SYS_TYPE_DVBT2)||(x == TV_SYS_TYPE_DVBT)||(x == TV_SYS_TYPE_DVBC)||(x == TV_SYS_TYPE_DTMB)||(x == TV_SYS_TYPE_ATSC)||(x == TV_SYS_TYPE_OPENCABLE)||(x == TV_SYS_TYPE_ISDBT)||(x == TV_SYS_TYPE_ISDBS)  )
#define IsAnalogSysType(x)      ((x == TV_SYS_TYPE_BGDKIL)  || (x == TV_SYS_TYPE_MN))

typedef enum {
	TV_SYS_TYPE_RAW_TS      = 0x80000000,
	TV_SYS_TYPE_RTSP        = 0x40000000,
	TV_SYS_TYPE_ABS_S       = 0x08000000,
	TV_SYS_TYPE_DVBS2       = 0x04000000,
	TV_SYS_TYPE_DVBS        = 0x02000000,
	TV_SYS_TYPE_DVBT2       = 0x01000000,
	TV_SYS_TYPE_DVBT        = 0x00800000,
	TV_SYS_TYPE_DVBC        = 0x00400000,
	TV_SYS_TYPE_DTMB        = 0x00200000,
	TV_SYS_TYPE_ATSC        = 0x00008000,
	TV_SYS_TYPE_ATSC3P0        = 0x00010000,
	TV_SYS_TYPE_OPENCABLE   = 0x00004000,
	TV_SYS_TYPE_ISDBT       = 0x00000800,
	TV_SYS_TYPE_ISDBS       = 0x00000400,
	TV_SYS_TYPE_BGDKIL      = 0x00000080,
	TV_SYS_TYPE_MN          = 0x00000040,
	TV_SYS_TYPE_ISDBT_CXD2856  = 0x00000020,
	TV_SYS_TYPE_ISDBT_CXD2878  = 0x00000021,
	TV_SYS_TYPE_ISDBS_CXD2856  = 0x00000022,
	TV_SYS_TYPE_ISDBS_CXD2878  = 0x00000023,
	TV_SYS_TYPE_ISDBS3_CXD2878 = 0x00000024,
	TV_SYS_TYPE_ISDBT_CXD2856_2nd  = 0x00000025,
	TV_SYS_TYPE_ISDBT_CXD2878_2nd  = 0x00000026,
	TV_SYS_TYPE_ISDBS_CXD2856_2nd  = 0x00000027,
	TV_SYS_TYPE_ISDBS_CXD2878_2nd  = 0x00000028,
	TV_SYS_TYPE_ISDBS3_CXD2878_2nd = 0x00000029,
	TV_SYS_TYPE_UNKNOWN     = 0x00000000,
	TV_SYS_TYPE_AUTOSCAN    = 0x00000001,

} TV_SYSTEM_TYPE;


/////////////////////////////////////////////////////
// System Modulation
/////////////////////////////////////////////////////
enum TV_MODULATION_E {
	TV_MODULATION_UNKNOWN,
	TV_MODULATION_NONE,
	TV_MODULATION_VSB,
	TV_MODULATION_ATSC3,
	TV_MODULATION_OFDM,
	TV_MODULATION_PSK,
	TV_MODULATION_QAM,
	TV_MODULATION_DVBC_QAM,
	TV_MODULATION_DTMB_OFDM,
	TV_MODULATION_DVBT2_OFDM,        // T2OFDM
	TV_MODULATION_DVBS,
	TV_MODULATION_DVBS2,
	TV_MODULATION_ISDBT,            // ISDB-T
	TV_MODULATION_ISDBS,				  // ISDB-S
	TV_MODULATION_ISDBS3,			// ISDB-S3
	TV_MODULATION_ABSS,             // ABS-S
	TV_MODULATION_RTSP,
	TV_MODULATION_ATV,
};


//------------------------------------------------
//  COFDM (used by DVB-T/ISDBT)
//------------------------------------------------

enum {
	HIERARCHY_MODE_HIGH = 0,
	HIERARCHY_MODE_LOW  = 0xff,
};


enum {
	OFDM_CONST_BPSK	= 0,
	OFDM_CONST_DQPSK	= 1,
	OFDM_CONST_4  =  4,
	OFDM_CONST_16 = 16,
	OFDM_CONST_32 = 32,
	OFDM_CONST_64 = 64,
	OFDM_CONST_128 = 128,
	OFDM_CONST_256 = 256,
};

enum {
	OFDM_ALPHA_0  =  0,
	OFDM_ALPHA_1  =  1,
	OFDM_ALPHA_2  =  2,
	OFDM_ALPHA_4  =  4,
};

enum {
	OFDM_GI_1_32  =  32,
	OFDM_GI_1_16  =  16,
	OFDM_GI_1_8   =  8,
	OFDM_GI_1_4   =  4,
	OFDM_GI_1_128   =  128,
	OFDM_GI_19_128   =  129,
	OFDM_GI_19_256   =  200,
};

enum {
	OFDM_FFT_1K   =  1,
	OFDM_FFT_2K   =  2,
	OFDM_FFT_4K   =  4,
	OFDM_FFT_8K   =  8,
	OFDM_FFT_16K   =  16,
	OFDM_FFT_32K   =  32,
};

enum {
	OFDM_CODE_RATE_1_2 = 0x12,
	OFDM_CODE_RATE_2_3 = 0x23,
	OFDM_CODE_RATE_3_4 = 0x34,
	OFDM_CODE_RATE_3_5 = 0x35,
	OFDM_CODE_RATE_4_5 = 0x45,
	OFDM_CODE_RATE_5_6 = 0x56,
	OFDM_CODE_RATE_7_8 = 0x78
};

enum {
	OFDM_BW_1p7M  =  1700000,
	OFDM_BW_5M = 5000000,
	OFDM_BW_6M = 6000000,
	OFDM_BW_7M = 7000000,
	OFDM_BW_8M = 8000000
};

// Interleaving length
enum {
	OFDM_INTERLEAVING_LEN_0 = 1,
	OFDM_INTERLEAVING_LEN_1 = 2,
	OFDM_INTERLEAVING_LEN_2 = 4,
	OFDM_INTERLEAVING_LEN_4 = 8,
	OFDM_INTERLEAVING_LEN_8,
	OFDM_INTERLEAVING_LEN_16,
};

//------------------------------------------------
//  DVB-T2
//------------------------------------------------

enum {
	DVBT2_L1_CONST_BPSK	= 0,
	DVBT2_L1_CONST_DQPSK	= 1,
	DVBT2_L1_CONST_QPSK	=  4,
	DVBT2_L1_CONST_16 	= 16,
	DVBT2_L1_CONST_64 	= 64,
};

enum {
	DVBT2_BASE_SISO = 0,
	DVBT2_BASE_MISO = 1,
	DVBT2_NON_DVBT2 = 2,
	DVBT2_LITE_SISO = 3,
	DVBT2_LITE_MISO = 4,
};//S1Signalling

enum {
	DVBT2_PAPR_NONE = 0,
	DVBT2_PAPR_ACE = 1,
	DVBT2_PAPR_TR = 2,
	DVBT2_PAPR_TR_ACE = 3,
};//PAPR

enum {
	DVBT2_PROFILE_BASE = 0,
	DVBT2_PROFILE_LITE = 1,
	DVBT2_PROFILE_ANY = 2,
};//T2 profile

/*DVB-T2 pilot pattern*/
enum {
	DVBT2_PP1 = 0,
	DVBT2_PP2,
	DVBT2_PP3,
	DVBT2_PP4,
	DVBT2_PP5,
	DVBT2_PP6,
	DVBT2_PP7,
	DVBT2_PP8,
	DVBT2_PP_RESERVED8,       /* Reserved */
	DVBT2_PP_RESERVED9,
	DVBT2_PP_RESERVED10,
	DVBT2_PP_RESERVED11,
	DVBT2_PP_RESERVED12,
	DVBT2_PP_RESERVED13,
	DVBT2_PP_RESERVED14,
	DVBT2_PP_RESERVED15,      /* Reserved */
	DVBT2_PP_UNKNOWN
};  //T2 pilot pattern


//------------------------------------------------
//  DVBS2 (used by DVB-S / DVB-S2)
//------------------------------------------------

// constellations : number of PSK
enum {
	DVBS2_MOD_QPSK = 0,
	DVBS2_MOD_8PSK,
	DVBS2_MOD_16APSK,
	DVBS2_MOD_32APSK,
	DVBS2_MOD_UNKNOWN
};

enum {
	DVBS2_CR_1_2 = 0,
	DVBS2_CR_2_3 = 1,
	DVBS2_CR_3_4 = 2,
	DVBS2_CR_5_6 = 3,
	DVBS2_CR_7_8 = 4,
	DVBS2_CR_3_5 = 5,
	DVBS2_CR_4_5 = 6,
	DVBS2_CR_8_9 = 7,
	DVBS2_CR_9_10= 8,
	DVBS2_CR_UNKNOWN = 9,
	DVBS2X_CR_DUMMY  = 10,
	DVBS2X_CR_13_45  = 11,
	DVBS2X_CR_9_20   = 12,
	DVBS2X_CR_90_180 = 13,
	DVBS2X_CR_96_180 = 14,
	DVBS2X_CR_11_20  = 15,
	DVBS2X_CR_100_180= 16,
	DVBS2X_CR_26_45  = 17,
	DVBS2X_CR_18_30  = 18,
	DVBS2X_CR_28_45  = 19,
	DVBS2X_CR_23_36  = 20,
	DVBS2X_CR_116_180= 21,
	DVBS2X_CR_20_30  = 22,
	DVBS2X_CR_124_180= 23,
	DVBS2X_CR_25_36  = 24,
	DVBS2X_CR_128_180= 25,
	DVBS2X_CR_13_18  = 26,
	DVBS2X_CR_22_30  = 27,
	DVBS2X_CR_135_180= 28,
	DVBS2X_CR_7_9    = 29,
	DVBS2X_CR_154_180= 30,
	DVBS2X_CR_UNKNOWN = 31
};

enum {
	DVBS_CR_1_2 = 0,
	DVBS_CR_2_3,
	DVBS_CR_3_4,
	DVBS_CR_5_6,
	DVBS_CR_7_8,
	DVBS_CR_UNKNOWN
};

enum {
	DVBS2_ROLL_OFF_35 = 0,
	DVBS2_ROLL_OFF_20,
	DVBS2_ROLL_OFF_25,
	DVBS2_ROLL_UNKNOWN
};

enum {
	DVBS2_PILOT_EN = 0,
	DVBS2_PILOT_DIS,
	DVBS2_PILOT_UNKNOWN
};

enum {
	DVBS2_SPEC_NORMAL = 0,
	DVBS2_SPEC_INVERSE,
	DVBS2_SPEC_UNKNOWN
};


//------------------------------------------------
//  QAM (used by DVB-C / ATSC)
//------------------------------------------------

// constellations : number of QAM
enum {
	QAM_CONST_4    =    4,      // QPSK
	QAM_CONST_16   =   16,
	QAM_CONST_32   =   32,
	QAM_CONST_64   =   64,
	QAM_CONST_128  =  128,
	QAM_CONST_256  =  256,
	QAM_CONST_512  =  512,
	QAM_CONST_1024 = 1024,
	QAM_CONST_AUTO = 1234,
};

// alpha rate : roll of factor
enum {
	QAM_ALPHA_12   =   12,
	QAM_ALPHA_13   =   13,
	QAM_ALPHA_15   =   15,
	QAM_ALPHA_18   =   18,
	QAM_ALPHA_20   =   20,
};

enum {
	QAM_SPEC_NORMAL   =   0,
	QAM_SPEC_INVERSE  =  1,
};

//------------------------------------------------
//  DTMB-OFDM (used by DTMB)
//------------------------------------------------
typedef enum {
	DTMB_OFDM_CARRIER_SINGLE,
	DTMB_OFDM_CARRIER_MULTI,

	DTMB_OFDM_CARRIER_INVALID
} DTMB_OFDM_CARRIER_MODE;

typedef enum {
	DTMB_OFDM_TIM_240,
	DTMB_OFDM_TIM_720,

	DTMB_OFDM_TIM_INVALID,
} DTMB_OFDM_TIME_INTERLEAVER_MODE_E;

// Forward Error Correction Rate (or Convolutional Rate)
typedef enum {
	DTMB_OFDM_FEC_4_OVER_10,  // 0.4
	DTMB_OFDM_FEC_6_OVER_10,  // 0.6
	DTMB_OFDM_FEC_8_OVER_10,  // 0.8

	DTMB_OFDM_FEC_INVALID
} DTMB_OFDM_FEC_RATE_E;

typedef enum {
	DTMB_OFDM_QAM_4_NR,
	DTMB_OFDM_QAM_4,
	DTMB_OFDM_QAM_16,
	DTMB_OFDM_QAM_32,
	DTMB_OFDM_QAM_64,

	DTMB_OFDM_QAM_INVALID
} DTMB_OFDM_CONSTELLATION_E;

// Pseudo-random Noise
typedef enum {
	DTMB_OFDM_PN_420,
	DTMB_OFDM_PN_595,
	DTMB_OFDM_PN_945,

	DTMB_OFDM_PN_INVALID
} DTMB_OFDM_PSEUDO_NOISE_E;

typedef enum {
	DTMB_OFDM_PN_SEQ_CONSTANT,
	DTMB_OFDM_PN_SEQ_VARIABLE,

	DTMB_OFDM_PN_SEQ_INVALID
} DTMB_OFDM_PSEUDO_NOISE_SEQUENCE_E;



//------------------------------------------------
//  ATSC3
//------------------------------------------------

typedef enum {
	ATSC3_MODE_8K = 0,
	ATSC3_MODE_16K,
	ATSC3_MODE_32K,
	ATSC3_MODE_RESERVED,
	ATSC3_MODE_UNKNOWN
} ATSC3_FFT_MODE_TYPE;


typedef enum {
	ATSC3_GI0_RESERVED = 0,
	ATSC3_GI1_192,
	ATSC3_GI2_384,
	ATSC3_GI3_512,
	ATSC3_GI4_768,
	ATSC3_GI5_1024,
	ATSC3_GI6_1536,
	ATSC3_GI7_2048,
	ATSC3_GI8_2432,
	ATSC3_GI9_3072,
	ATSC3_GI10_3648,
	ATSC3_GI11_4096,
	ATSC3_GI12_4864,
	ATSC3_GI13_RESERVED,
	ATSC3_GI14_RESERVED,
	ATSC3_GI15_RESERVED
} ATSC3_GUARD_INTERVAL_TYPE;

typedef enum {
	ATSC3_QPSK = 0,
	ATSC3_QAM_16,
	ATSC3_QAM_64,
	ATSC3_QAM_256,
	ATSC3_QAM_1024,
	ATSC3_QAM_4096,
	ATSC3_QAM_RESERVED6,
	ATSC3_QAM_RESERVED7,
	ATSC3_QAM_RESERVED8,
	ATSC3_QAM_RESERVED9,
	ATSC3_QAM_RESERVED10,
	ATSC3_QAM_RESERVED11,
	ATSC3_QAM_RESERVED12,
	ATSC3_QAM_RESERVED13,
	ATSC3_QAM_RESERVED14,
	ATSC3_QAM_RESERVED15
} ATSC3_CONSTELLATION_TYPE;


/*ATSC3 code rate type of data PLP*/
typedef enum {
	ATSC3_CODE_RATE_2_15 = 0,
	ATSC3_CODE_RATE_3_15,
	ATSC3_CODE_RATE_4_15,
	ATSC3_CODE_RATE_5_15,
	ATSC3_CODE_RATE_6_15,
	ATSC3_CODE_RATE_7_15,
	ATSC3_CODE_RATE_8_15,
	ATSC3_CODE_RATE_9_15,
	ATSC3_CODE_RATE_10_15,
	ATSC3_CODE_RATE_11_15,
	ATSC3_CODE_RATE_12_15,
	ATSC3_CODE_RATE_13_15,
	ATSC3_CODE_RATE_RESERVED12,
	ATSC3_CODE_RATE_RESERVED13,
	ATSC3_CODE_RATE_RESERVED14,
	ATSC3_CODE_RATE_RESERVED15
} ATSC3_CODE_RATE_TYPE;




/////////////////////////////////////////////////////
// TS_PARAM
// TS information
/////////////////////////////////////////////////////
typedef struct {
	unsigned char mode;
	unsigned char data_order : 1;   // 0 : bit 7 is MSB, 1 : bit 7 is LSB (Parallel)
	unsigned char datapin  : 1;     // 0 : MSB First,   1 : LSB First (Serial)
	unsigned char err_pol  : 1;     // 0 : Active High, 1 : Active Low
	unsigned char sync_pol : 1;     // 0 : Active High, 1 : Active Low
	unsigned char val_pol  : 1;     // 0 : Active High, 1 : Active Low
	unsigned char clk_pol  : 1;     // 0 : Latch On Rising Edge, 1 : Latch On Falling Edge
	unsigned char internal_demod_input : 1;  // used to enable internal demod input
	unsigned char strength_mode;
	unsigned char tsOutEn : 1;
	unsigned char portIdx;
} TS_PARAM;


enum {
	PARALLEL_MODE   = 0,
	SERIAL_MODE     = 1,
	MEMORY_MODE     = 2,
	USB_MODE        = 3,
};

enum {
	MSB_FIRST       = 0,
	LSB_FIRST       = 1
};

enum {
	MSB_D7          = 0,
	MSB_D0          = 1
};

enum {
	ACTIVE_HIGH     = 0,
	ACTIVE_LOW      = 1
};

enum {
	RISING_EDGE     = 0,
	FALLING_EDGE    = 1
};

enum {
	STRENGTH_NORMAL     = 0,
	STRENGTH_STRONG    = 1
};

typedef enum {
	LOOP_THROUGH_OFF = 0,
	LOOP_THROUGH_ON,
	LOOP_THROUGH_ON_HG,
} LOOP_THROUGH_MODE;

typedef enum {
	CLOCK_OUTPUT_DISABLE = 0,
	CLOCK_OUTPUT_ENABLE,
} CLOCK_OUTPUT_MODE;



/////////////////////////////////////////////////////
// TUNER_PARAM
// afc information
/////////////////////////////////////////////////////
typedef enum {
	TV_AFC_UNLOCK  = -1,
	TV_AFC_LOW     =  0,
	TV_AFC_OPTIMAL =  1,
	TV_AFC_HIGH    =  2
} TV_AFC;


typedef enum {
	REALTEKALL,
	MAC6
} IC_NAME;


typedef struct {
	unsigned short  id;
	TV_SYSTEM       std;
	U32BITS   step_freq;
	U32BITS   if_freq;
	unsigned char   if_inversion;
	unsigned char   if_agc_pola;       //agc gain, 0: postive, 1:negative
	U32BITS   min_freq;
	U32BITS   max_freq;
	U32BITS   lowband_midband_limit_freq;  // The limit frequency (in Hz) between low-band and mid-band.
	U32BITS   midband_highband_limit_freq; // The limit frequency (in Hz) between mid-band and high-band.
} TUNER_PARAM;


typedef struct {
	U32BITS   freq;               // frequency (Hz)
	unsigned char   inversion;          // inversion
	unsigned char   agc_pola;       //agc gain, 0: postive, 1:negative
} IF_PARAM;


typedef struct {
	unsigned char iq_pola;
	unsigned char adc_mode;
	unsigned char agc_pola;
} IQ_PARAM;

/////////////////////////////////////////////////////
// LNB Param
/////////////////////////////////////////////////////

#define LNB_SUPPORT             // flag to indicate LNB Support

typedef enum {
	LNB_MODE_OFF,
	LNB_MODE_13V,
	LNB_MODE_18V,
	LNB_MODE_13V_WITH_22K_BURST,
	LNB_MODE_18V_WITH_22K_BURST,
	LNB_MODE_MAX,
} LNB_MODE;


/////////////////////////////////////////////////////
// Blind Scan Param
/////////////////////////////////////////////////////

#define BLIND_SCAN_SUPPORT             // flag to indicate LNB Support

typedef struct {

	U32BITS   type;           // TV_SYSTEM_TYPE

	U32BITS   min_freq;       // start freq (in Hz)
	U32BITS   max_freq;       // stop  freq (in Hz)

	union {
		// type == TV_SYS_TYPE_DVBS || TV_SYS_TYPE_DVBS2
		struct {
			U32BITS   min_symb;   // minimum symbol rate (in symbols)
			U32BITS   max_symb;   // maximum symbol rate (in symbols)
			LNB_MODE        lnb_mode;   // lnb mode
		} dvb_sx;
	};

} BLIND_SCAN_PARAM;

/////////////////////////////////////////////////////
// FE_PARAM
// afc information
/////////////////////////////////////////////////////

typedef enum TV_MODULATION_E TV_MODULATION;

#define SC_AUTO_OFST         (0xFFFFFFFF)
#define SC_WAIT_FOREVER      (0xFFFFFFFF)

typedef enum {
	SCAN_RANGE_NONE,
	SCAN_RANGE_1_6,
	SCAN_RANGE_2_6,
	SCAN_RANGE_3_6,
	SCAN_RANGE_MAX,
} SCAN_RANGE;

#define SC_RANGE_0           SCAN_RANGE_NONE        // 0 no offset
#define SC_RANGE_1_6         SCAN_RANGE_1_6         // 0 +/- 166.67KHz
#define SC_RANGE_1_3         SCAN_RANGE_2_6         // 0 +/- 166.67KHz +/- 300KHz
#define SC_RANGE_1_2         SCAN_RANGE_3_6         // 0 +/- 166.67KHz +/- 300KHz +/- 500KHz

typedef enum {
	CH_NORMAL_MODE,
	CH_SCAN_MODE,
	CH_MANUAL_MODE,
	CH_UNKNOW,
} CH_MODE;

typedef	struct {
	unsigned char   demod_param_en;
	U32BITS   if_freq;               /* if frequency (Hz) */
	unsigned char   if_inversion;       /* 0: if non_inversion 1:if inversion */
	unsigned char   agc_pola;       /* postive agc gain, 0: postive, 1:negative */
} TUN_DEMOD_PARA;

typedef enum {
    ISDBS_TSID_TYPE_TSID,                /**< TSID(16bit) */
    ISDBS_TSID_TYPE_RELATIVE_TS_NUMBER,  /**< Relative TS number */
} ISDBS_TSID_TYPE;

//---------For Ext ISDB Demod----------------//
typedef enum {
	I2C_PORT1, //demod i2c addr1
	I2C_PORT2, //demod i2c addr2
	I2C_PORT3, //demod i2c addr3
} DEMOD_PORT;

typedef enum isdbs_modcod_t {
    ISDBS_MODCOD_RESERVED_0,             /**< Reserved 0 */
    ISDBS_MODCOD_BPSK_1_2,               /**< BPSK Code Rate 1/2 */
    ISDBS_MODCOD_QPSK_1_2,               /**< QPSK Code Rate 1/2 */
    ISDBS_MODCOD_QPSK_2_3,               /**< QPSK Code Rate 2/3 */
    ISDBS_MODCOD_QPSK_3_4,               /**< QPSK Code Rate 3/4 */
    ISDBS_MODCOD_QPSK_5_6,               /**< QPSK Code Rate 5/6 */
    ISDBS_MODCOD_QPSK_7_8,               /**< QPSK Code Rate 7/8 */
    ISDBS_MODCOD_TC8PSK_2_3,             /**< TC8PSK Code Rate 2/3 */
    ISDBS_MODCOD_RESERVED_8,             /**< Reserved 8 */
    ISDBS_MODCOD_RESERVED_9,             /**< Reserved 9 */
    ISDBS_MODCOD_RESERVED_10,            /**< Reserved 10 */
    ISDBS_MODCOD_RESERVED_11,            /**< Reserved 11 */
    ISDBS_MODCOD_RESERVED_12,            /**< Reserved 12 */
    ISDBS_MODCOD_RESERVED_13,            /**< Reserved 13 */
    ISDBS_MODCOD_RESERVED_14,            /**< Reserved 14 */
    ISDBS_MODCOD_UNUSED_15               /**< unused (1111) */
} isdbs_modcod_t;

typedef struct isdbs_tmcc_modcod_slot_info_t {
    isdbs_modcod_t          modCod;     /**< Modulation scheme and Code rate */
    unsigned char           slotNum;    /**< Slot number */
} isdbs_tmcc_modcod_slot_info_t;

typedef struct isdbs_tmcc_info_t {
    unsigned char                      changeOrder;               /**< Incremented each time TMCC is renewed */
    isdbs_tmcc_modcod_slot_info_t      modcodSlotInfo[4];         /**< Transmission mode and slot information */
    unsigned char                      relativeTSForEachSlot[48]; /**< Relative TS ID for each slot */
    unsigned short                     tsidForEachRelativeTS[8];  /**< Corresponding table between relative TS and TSID */
    unsigned char                      ewsFlag;                   /**< Emergency Warning Broadcasting */
    unsigned char                      uplinkInfo;                /**< Site diversity warning */
} isdbs_tmcc_info_t;

typedef enum isdbs3_mod_t {
    ISDBS3_MOD_RESERVED_0,                /**< Reserved 0 */
    ISDBS3_MOD_BPSK,                      /**< BPSK */
    ISDBS3_MOD_QPSK,                      /**< QPSK */
    ISDBS3_MOD_8PSK,                      /**< 8PSK */
    ISDBS3_MOD_16APSK,                    /**< 16APSK */
    ISDBS3_MOD_32APSK,                    /**< 32APSK */
    ISDBS3_MOD_RESERVED_6,                /**< Reserved 6 */
    ISDBS3_MOD_RESERVED_7,                /**< Reserved 7 */
    ISDBS3_MOD_RESERVED_8,                /**< Reserved 8 */
    ISDBS3_MOD_RESERVED_9,                /**< Reserved 9 */
    ISDBS3_MOD_RESERVED_10,               /**< Reserved 10 */
    ISDBS3_MOD_RESERVED_11,               /**< Reserved 11 */
    ISDBS3_MOD_RESERVED_12,               /**< Reserved 12 */
    ISDBS3_MOD_RESERVED_13,               /**< Reserved 13 */
    ISDBS3_MOD_RESERVED_14,               /**< Reserved 14 */
    ISDBS3_MOD_UNUSED_15                  /**< unused (1111) */
} isdbs3_mod_t;

typedef enum isdbs3_cod_t {
    ISDBS3_COD_RESERVED_0,                /**< Reserved 0 */
    ISDBS3_COD_41_120,                    /**< 41/120 (1/3) */
    ISDBS3_COD_49_120,                    /**< 49/120 (2/5) */
    ISDBS3_COD_61_120,                    /**< 61/120 (1/2) */
    ISDBS3_COD_73_120,                    /**< 73/120 (3/5) */
    ISDBS3_COD_81_120,                    /**< 81/120 (2/3) */
    ISDBS3_COD_89_120,                    /**< 89/120 (3/4) */
    ISDBS3_COD_93_120,                    /**< 93/120 (7/9) */
    ISDBS3_COD_97_120,                    /**< 97/120 (4/5) */
    ISDBS3_COD_101_120,                   /**< 101/120 (5/6) */
    ISDBS3_COD_105_120,                   /**< 105/120 (7/8) */
    ISDBS3_COD_109_120,                   /**< 109/120 (9/10) */
    ISDBS3_COD_RESERVED_12,               /**< Reserved 12 */
    ISDBS3_COD_RESERVED_13,               /**< Reserved 13 */
    ISDBS3_COD_RESERVED_14,               /**< Reserved 14 */
    ISDBS3_COD_UNUSED_15                  /**< unused (1111) */
} isdbs3_cod_t;

typedef enum isdbs3_stream_type_t {
    ISDBS3_STREAM_TYPE_RESERVED_0,               /**< Reserved 0 */
    ISDBS3_STREAM_TYPE_MPEG2_TS,                 /**< MPEG-2 TS  */
    ISDBS3_STREAM_TYPE_TLV,                      /**< TLV        */
    ISDBS3_STREAM_TYPE_NO_TYPE_ALLOCATED = 0xFF, /**< No type allocated (11111111) */
} isdbs3_stream_type_t;

typedef struct isdbs3_tmcc_modcod_slot_info_t {
    isdbs3_mod_t            mod;        /**< Modulation */
    isdbs3_cod_t            cod;        /**< Code rate */
    unsigned char           slotNum;    /**< Slot number */
    unsigned char           backoff;    /**< Backoff */
} isdbs3_tmcc_modcod_slot_info_t;

typedef struct isdbs3_tmcc_info_t {
    unsigned char                  changeOrder;                                   /**< Incremented each time TMCC is renewed */
    isdbs3_tmcc_modcod_slot_info_t modcodSlotInfo[8];                             /**< Transmission mode and slot information */
    isdbs3_stream_type_t           streamTypeForEachRelativeStream[16];           /**< Stream type and relative stream number information */
    unsigned short                 packetLengthForEachRelativeStream[16];         /**< Packet length */
    unsigned char                  syncPatternBitLengthForEachRelativeStream[16]; /**< Sync pattern bit length */
    unsigned int                   syncPatternForEachRelativeStream[16];          /**< Sync pattern */
    unsigned char                  relativeStreamForEachSlot[120];                /**< Relative stream number and slot information */
    unsigned short                 streamidForEachRelativeStream[16];             /**< Corresponding table between relative StreamID */
    unsigned char                  ewsFlag;                                       /**< Emergency Warning Broadcasting */
    unsigned char                  uplinkInfo;                                    /**< Site diversity warning */
} isdbs3_tmcc_info_t;

//---------For Ext ISDB Demod End------------//

typedef union {
	TV_MODULATION       mod;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		unsigned char   level;      // it should be 8 or 16
	} vsb;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		unsigned int   bandwidth;
		unsigned char  constellation[4];
		unsigned char   guard_interval[4];
		unsigned char   fft_mode[4];
		unsigned char   code_rate[4];
		unsigned char    plp_cnt;
		unsigned char    plp[64];
		unsigned char  bSpectrumInv;
	} atsc3;


	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		unsigned short  constellation;
	} psk;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		unsigned int   bandwidth;
		unsigned char   hierarchy;
		unsigned short  constellation;
		unsigned char   alpha;
		unsigned char   code_rate_hp;
		unsigned char   code_rate_lp;
		unsigned char   guard_interval;
		unsigned char   fft_mode;
		unsigned int  cell_ID;
		unsigned char  bSpectrumInv;
		unsigned char  bProfileHP;

	} ofdm;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		unsigned short   constellation;
		unsigned char    spectrum_inverse;
	} qam;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		unsigned short   constellation;
		U32BITS    symbol_rate;
		unsigned char    alpha;
		unsigned char    spectrum_inverse;
		U32BITS    freq_khz;
	} dvbc_qam;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		DTMB_OFDM_TIME_INTERLEAVER_MODE_E      tim;
		DTMB_OFDM_CARRIER_MODE                 carrier_mode;
		unsigned int guard_interval;

		struct {
			DTMB_OFDM_PSEUDO_NOISE_E           mode;
			DTMB_OFDM_PSEUDO_NOISE_SEQUENCE_E  seq;
		} pn;

		struct {
			DTMB_OFDM_FEC_RATE_E               rate;
			DTMB_OFDM_CONSTELLATION_E          constellation;
		} fec;

		unsigned int usrData;  // used to store device specific or user-defined data
	} dtmb_ofdm;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		U32BITS    symbol_rate;
		unsigned char    zapping_mode;
		unsigned char    modulation;
		unsigned char    code_rate;
		unsigned char    pilot_onoff;
		unsigned char    spec_invert;
		LNB_MODE         lnb_mode;
		unsigned char 	 DiseqcCmdCnt;
		unsigned char 	 DiseqcCmd[64];
		unsigned char 	 IsBlindsearch;
		U32BITS    freq_khz;
	} dvbs;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		U32BITS    symbol_rate;
		unsigned char    spectrum_inv: 1;
		LNB_MODE         lnb_mode;
	} abss;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		unsigned int   bandwidth;
		unsigned char   hierarchy;
		unsigned short  constellation;
		unsigned char   alpha;
		unsigned char   code_rate_hp;
		unsigned char   code_rate_lp;
		unsigned char   guard_interval;
		unsigned char   fft_mode;
		unsigned char    plp_cnt;
		unsigned char    plp[256];
		int plp_type[256];
		int plp_select;
		unsigned char profile;
		unsigned char fec_type;
		unsigned int cell_ID;
		unsigned char  bSpectrumInv;
		unsigned char  bProfileHP;
		unsigned char PilotPattern;
        unsigned char bwExt;

	} dvbt2_ofdm;

	struct {
		unsigned char bwExt;
		unsigned char s1;
		unsigned char s2;
		unsigned char fftMode;
		unsigned char gi;
		unsigned char papr;
		unsigned char mod;
		unsigned char code_rate;
		unsigned char pp;
		unsigned char version;
		int cell_ID;
	} dvbt2_L1pre;

	struct {
		U32BITS fefLength;
	} dvbt2_L1post;

	struct {
		unsigned char   tx_id_availability;
		unsigned int    cell_id;
		unsigned int    network_id;
		unsigned int    t2_system_id;
	} dvbt2_TXID;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		U32BITS    segment_num;
		U32BITS    subchannel_num;
		unsigned int trans_mode;
		unsigned int guard_interval;
		unsigned char  bSpectrumInv;

		struct isdbt_layer {
			unsigned int    segment_num;
			unsigned short constellation;
			unsigned char 	code_rate;
			unsigned int    time_intlv;

		} layer[3];
		DEMOD_PORT   portIdx;
		TV_SYSTEM    extSys;
	} isdbt;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		U32BITS       symbol_rate;
		LNB_MODE            lnb_mode;
		unsigned short      ts_id;
	} isdbs;

	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/
                U32BITS freq;

	} analog;

	struct {
		TV_MODULATION       mod;

		int              port;
		char             addr[16];	//IPV4
	} rtsp;
	
	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		U32BITS             freq_khz;
		//LNB_MODE            lnb_mode;
		unsigned short      ts_id;
		ISDBS_TSID_TYPE     ts_type;
		isdbs_modcod_t modcodH;
		isdbs_modcod_t modcodL;
		unsigned char       slotNumH;
		unsigned char       slotNumL;
		unsigned char       siteDiversityInfo;
		isdbs_tmcc_info_t   tmccInfo;
		DEMOD_PORT          portIdx;
		TV_SYSTEM           extSys;
	} isdbs_ext;
	
	struct {
		TV_MODULATION       mod;
		unsigned int scan_mode;      /* 0:normal mode, 1:scan mode, 10:DEMOD mode changed only, 20:Frontend mode changed*/

		U32BITS             freq_khz;
		//LNB_MODE            lnb_mode;
		unsigned short      ts_id;
		ISDBS_TSID_TYPE     ts_type;
		isdbs3_mod_t        modH;
		isdbs3_mod_t        modL;
		isdbs3_cod_t        codH;
		isdbs3_cod_t        codL;
		unsigned char       siteDiversityInfo;
		isdbs3_tmcc_info_t  tmccInfo;
		DEMOD_PORT          portIdx;
		TV_SYSTEM           extSys;
	} isdbs3_ext;
} TV_SIG_INFO;


typedef struct {
	U32BITS   snr;
	U32BITS           ber;
	U32BITS           per;
	unsigned char	iter;
	U32BITS   	ucblocks;
	unsigned char   	strength;
	unsigned char   	quality;
	S32BITS 	rflevel;
	U32BITS	     dbuvPower;
	U32BITS   	agc;
	unsigned char       nosig;
	unsigned char       trlock;
	unsigned char       inner;

	struct {
		U32BITS ber_num;
		U32BITS ber_den;
	} layer_ber;

	struct {
		U32BITS per_num;
		U32BITS per_den;
	} layer_per;

	unsigned int DemodFwVersion;
	unsigned char   emergency_flag;

	struct {
		U32BITS preBCHBerH;
		U32BITS preBCHBerL;
		U32BITS preLDPCBer[2];
		U32BITS postBCHFer[2];
	} ExtIsdbs3_Status;

	struct {
		unsigned char lock[3];        // 0 : Demod, 1 : TS, 2 : Early unlock (TODO)
		U32BITS preRSBER[3]; // 0 : high hierarchy,  1 : low hierarchy, 2 : TMCC  < x 10^7 >
		U32BITS dummy[1];    // PAD
		U32BITS PER[3];      // 0 : Layer A, 1 : Layer B, 2 : Layer C             < x 10^6 >
		unsigned char    TSIDError;
		S32BITS freq_offset;
		U32BITS isdbtPER[3];
		U32BITS isdbsPER[2];
		unsigned char demod_ret; //return value from ext demod
	} ExtIsdb_Status;
	unsigned char demod_i2cAddr;
} TV_SIG_QUAL;

typedef struct {
	unsigned int   IcName;
	unsigned int   IcVersion;
	unsigned int   PGAMode;
	unsigned int   PGAGain;
	unsigned int   IfPathType;
	unsigned int   Area;
} TV_DEMOD_INFO;

typedef enum {
	TV_QUAL_SNR,
	TV_QUAL_BEC,            // Bit error count
	TV_QUAL_BER,            // BER
	TV_QUAL_BER_L1,         // BER for ISDB-T LayerA/ DVB-T HP
	TV_QUAL_BER_L2,         // BER for ISDB-T LayerB/ DVB-T LP
	TV_QUAL_BER_L3,         // BER for ISDB-T LayerC
	TV_QUAL_PEC,            // packet error count
	TV_QUAL_PER,            // PER
	TV_QUAL_PER_L1,         // PER for ISDB-T LayerA/ DVB-T HP
	TV_QUAL_PER_L2,         // PER for ISDB-T LayerB/ DVB-T LP
	TV_QUAL_PER_L3,         // PER for ISDB-T LayerC
	TV_QUAL_ITER,
	TV_QUAL_UCBLOCKS,
	TV_QUAL_SIGNAL_STRENGTH,
	TV_QUAL_SIGNAL_QUALITY,
	TV_QUAL_SIGNAL_POWER,
	TV_QUAL_AGC,
	TV_QUAL_NOSIG,
	TV_QUAL_INNERLOCK,
	TV_QUAL_DEMODFWVERSION,
	TV_QUAL_EMERGENCY_FLAG,
	TV_QUAL_CUSTOMER_LG,
	TV_QUAL_PREBCHBER,
	TV_QUAL_PRERSBER,
	TV_QUAL_LOCKSTATUS,
	TV_QUAL_TSIDERROR,
	TV_QUAL_CARRIER_OFFSET,
	TV_QUAL_ISDB_PER,
	TV_QUAL_PRELDPCBER,
	TV_QUAL_POSTBCHFER,
} ENUM_TV_QUAL;

typedef struct {
	unsigned int freq;
	TV_SYSTEM TVSys;
	TV_SIG_INFO SigInfo; 

} RTK_DEMOD_ATS_GET_DUMMY;

/////////////////////////////////////// DISEQC Command ////////////////////////////////////////////
//#ifdef DISEQC_SUPPORT_EN

typedef enum {
	DISEQC_TONE_MODE_A,
	DISEQC_TONE_MODE_B,
	DISEQC_TONE_MODE_MAX,
} DISEQC_TONE_MODE;

//#endif

/////////////////////////////////////// Extra Command ////////////////////////////////////////////

#ifdef DEVICE_ATTR_EN

typedef struct {
	char*     name;           // name of the attribute
	char      mode;           // mode of attribute
	char*     desc;           // desc
	void*     own;            // owner of the attribute

#define DEV_ATTR_CAP_RD         0x1
#define DEV_ATTR_CAP_WR         0x2
#define DEV_ATTR_CAP_RW        (DEV_ATTR_CAP_RD | DEV_ATTR_CAP_WR)
	U32BITS   id;

} DEV_ATTR;

#define __ATTR(name,mode,desc, id)     { (char*)#name, mode, (char*) desc, NULL, (U32BITS) id }
#define __ATTR_RO(name, desc, id)      { (char*)#name, DEV_ATTR_CAP_RD, (char*) desc, NULL, (U32BITS) id }
#define __ATTR_WR(name, desc, id)      { (char*)#name, DEV_ATTR_CAP_WR, (char*) desc, NULL, (U32BITS) id }
#define __ATTR_RW(name, desc, id)      { (char*)#name, DEV_ATTR_CAP_RW, (char*) desc, NULL, (U32BITS) id }
#define __ATTR_NULL()                  { NULL,  0, 0, 0, 0 }

#define DEVICE_ATTR(name, mode, desc, id) \
    static DEV_ATTR dev_attr_##name = __ATTR(name, mode, desc, id)

#define DEV_ATTR_RO(name, desc, id)        DEVICE_ATTR(name, DEV_ATTR_CAP_RD, desc, id)
#define DEV_ATTR_WR(name, desc, id)        DEVICE_ATTR(name, DEV_ATTR_CAP_WR, desc, id)
#define DEV_ATTR_RW(name, desc, id)        DEVICE_ATTR(name, DEV_ATTR_CAP_RW, desc, id)


class Device
{
public:

	Device()
	{
		m_pAttrList  = NULL;     // AttributeList
		m_AttrCounts = 0;
	}

	virtual ~Device() {}

protected:
	DEV_ATTR*       m_pAttrList;
	int             m_AttrCounts;

public:

	virtual int EnumAttr(int ofst, DEV_ATTR* pAttr)
	{
		if (ofst >= m_AttrCounts)
			return TUNER_CTRL_FAIL;

		*pAttr = *(m_pAttrList + ofst);

		return TUNER_CTRL_OK;
	}

	virtual int ShowAttr(const DEV_ATTR* pAttr, char* pBuff, unsigned int BuffLen)
	{
		return TUNER_CTRL_FAIL;
	}

	virtual int StoreAttr(const DEV_ATTR* pAttr, const char* pBuff, unsigned int BuffLen)
	{
		return TUNER_CTRL_FAIL;
	}
};

#endif // __DEVICE_ATTR_EN__


/////////////////////////////////////// FE Command ////////////////////////////////////////////

typedef enum {
	// Please Add your command over here...
	TV_FECTL_INSPECT_SIGNAL,

	// Defined by Libar
	TV_FECTL_LIBAR_TUNER_INIT,
	TV_FECTL_LIBAR_TUNER_RESET,
	TV_FECTL_LIBAR_SHOW_STATUS_MIN_DVB_T,
	TV_FECTL_LIBAR_SHOW_STATUS_MIN_ATSC,

} ENUM_TV_FECTL;






// lgms_kimsuyoung_20090522, TUNER_JIG_ANDY
typedef struct {
	unsigned char       chip_id;
	int                 frq_cnt;        //center freq.
	unsigned char       frq_lock;   // =>1 ;lock
	unsigned int        bandwith;

	unsigned char       snr_cal;        // Quality(0~100%)
	unsigned char       ifagc_cal;  // Strength(0~100%)

	union {
		unsigned short  reg;
		struct {                    // low bit first
			unsigned short  fft     : 2;    // 1,0
			unsigned short  gi      : 2;    // 3,2
			unsigned short  lp      : 3;    // 6,,4
			unsigned short  hp      : 3;    // 9,,7
			unsigned short  hier    : 3;    //12,,10
			unsigned short  constel     : 2;    //14,13
			unsigned short  valid   : 1;    //15
		} bit;
	} TPS;

	int             frq_offset;
	unsigned short  agc_gain;       // 14bit
	unsigned char       snr;
	unsigned short  rs_ubc;
	unsigned int        ubc_total;          // ubc total


	unsigned int         rs_err_cnt;
	unsigned int         rs_err_per;
	unsigned int         vit_out_ber;       // Viterbi_output_BER
	unsigned int         vit_err_cnt;
	unsigned int         vit_err_per;
	unsigned int         vit_inp_ber;       // Viterbi_input_BER

	union {
		unsigned char   reg;
		struct {                    // low bit first
			unsigned char   agc     : 1;    // No/Yes
			unsigned char   sym     : 1;
			unsigned char   ofdm    : 1;    // No/Yes
			unsigned char   tps     : 1;
			unsigned char   res     : 1;    // No/Yes
			unsigned char   fec     : 1;
			unsigned char   full    : 1;    // No/Yes
			unsigned char   si      : 1;
		} bit;
	} STATUS0;                                                  //Data 5

} TU_JIG_STATUS;


typedef struct {
	unsigned char       status003;
	int                 frq_cnt;    //center freq.
	unsigned char       frq_lock;   // =>1 ;lock
	unsigned char       op_mode;    // 64QAM/256QAM/VSB

	unsigned char       snr_cal;    // Quality(0~100%)
	unsigned char       ifagc_cal;  // Strength(0~100%)
	U32BITS               ber;

	int                 frq_offset;
	short               ifacc;
	unsigned short      packet_err; //FEC Packet Error Counter(50c,50d)
	unsigned char       snr_raw;

	unsigned char       lock_cr;
	unsigned char       lock_tr;
	unsigned char       lock_viterbi;
	unsigned char       lock_fec;
	unsigned char       lock_mpeg;
	unsigned char       lock_sync;

} TU_JIG_STATUS_ATSC;


#ifdef __cplusplus
}
#endif

#endif  //__TV_FE_TYPES_H__
