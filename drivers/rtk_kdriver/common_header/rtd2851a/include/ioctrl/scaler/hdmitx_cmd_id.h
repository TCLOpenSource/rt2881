#ifndef _HDMITX_DEVICE_ID_H_
#define _HDMITX_DEVICE_ID_H_



#ifndef BOOLEAN
#ifndef _EMUL_WIN
typedef unsigned int        __BOOLEAN;
#define BOOLEAN __BOOLEAN
#else
typedef unsigned char        __BOOLEAN;
#define BOOLEAN __BOOLEAN
#endif
#endif


/************************************************************************
 *  Definitions
 ************************************************************************/
/*-----------------------------------------------------------------------------
	System call
------------------------------------------------------------------------------*/
#define HDMITX_DEVICE_NUM        1
#define HDMITX_DEVICE_NAME       "hdmitxdev"
#define HDMITX_IOC_MAGIC         'x'

#define HDMITX_MAJOR 0   /* dynamic major by default */
#define HDMITX_NR_DEVS 1    /* se0 through se3 */

#define HDMITX_IOC_START_INDEX 0
typedef enum{
    KADP_HDMITX_MODE_PUREBYPASS = 0,
    KADP_HDMITX_MODE_CONVERT,
    KADP_HDMITX_MODE_PTG,
    KADP_HDMITX_MODE_MAX
}KADP_HDMITX_MODE_T;
typedef enum{
    KADP_HDMITX_INPUT_SRC_HDMIRX = 0,
    KADP_HDMITX_INPUT_SRC_DPRX,
    KADP_HDMITX_INPUT_SRC_DTG,
    KADP_HDMITX_INPUT_SRC_MAX
}KADP_HDMITX_INPUT_SRC_T;

typedef struct
{
    UINT8 hdmitx_port;
    KADP_HDMITX_MODE_T  mode;
    KADP_HDMITX_INPUT_SRC_T  input_src;
    UINT8 input_src_port;
}HDMITX_CONNECT_PARA_T;
typedef struct
{
    UINT8 hdmitx_port;
    UINT8 hpd_status;
}HDMITX_HPDSTATUS_PARA_T;
typedef struct
{
        UINT16 TxOutputHsta;     //Tx act h sta
        UINT16 TxOutputVsta;     //Tx act v sta
        UINT16 TxOutputHwid;	//Tx act h wid
        UINT16 TxOutputVlen;     //Tx act v length
        UINT16 TxOutputHTotal;   	   //Tx Horizontal Total Length
        UINT16 TxOutputVTotal;   	   //Tx Vertical Total Length
        UINT16 TxOutputHSync;   	   //TX output hsync width
        UINT16 TxOutputVSync;   	   //TX output vsync width
        UINT16 TxOutputTimingMode;	   //Tx timing table index
        UINT16 vFreq;                   // vFreq Vertical frequency
        UINT8 scanType;                // Scantype ( Progressive / Interlace )
        UINT8 VRR_mode;                // 0: non VRR, 1: Freesync, 2: G-sync//emp_all
        UINT8 colordepth;          // get color depth
        UINT8 colorspace;     // get color space
        UINT16 pixelFreq;    // TX timing mode pixel clock
}StructHDMITxOutputTimingInfo;

typedef struct
{
    UINT8 hdmitx_port;
    StructHDMITxOutputTimingInfo stTxOutputTimingInfo;
} KADP_VBE_HDMITX_TIMING_INFO_T;
typedef struct
{
    UINT8 hdmitx_port;
    UINT8 EDID[512];
    UINT16  len;
} KADP_VBE_HDMITX_EDID_INFO_T;
typedef struct
{
    UINT8 hdmitx_port;
    UINT8  HdcpVer;
} KADP_VBE_HDMITX_HDCP_VER_INFO_T;
typedef struct
{
    UINT8 hdmitx_port;
    UINT8  HdcpStatus;
} KADP_VBE_HDMITX_HDCP_STATUS_INFO_T;
typedef struct
{
    UINT8 hdmitx_port;
    UINT8  AudioEn;
} KADP_VBE_HDMITX_AUDIO_STATUS_INFO_T;

enum hdmitx_ioc_idx
{
    HDMITX_IOC_IDX_0 = HDMITX_IOC_START_INDEX,//0
    HDMITX_IOC_IDX_1,
    HDMITX_IOC_IDX_2,
    HDMITX_IOC_IDX_3,
    HDMITX_IOC_IDX_4,
    HDMITX_IOC_IDX_5,
    HDMITX_IOC_IDX_6,
    HDMITX_IOC_IDX_7,
    HDMITX_IOC_IDX_8,
    HDMITX_IOC_IDX_9,
    HDMITX_IOC_IDX_10,
    HDMITX_IOC_IDX_11,
    HDMITX_IOC_IDX_12,
    HDMITX_IOC_IDX_13,
    HDMITX_IOC_IDX_14,
    HDMITX_IOC_IDX_15,
    HDMITX_IOC_IDX_16,
    HDMITX_IOC_MAXNR,
};

#define HDMITX_IOC_INITIALIZE            _IO(HDMITX_IOC_MAGIC,  HDMITX_IOC_IDX_1)
#define HDMITX_IOC_DEINIT                _IO(HDMITX_IOC_MAGIC,  HDMITX_IOC_IDX_2)
#define HDMITX_IOC_ON                    _IO(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_3)
#define HDMITX_IOC_OFF                   _IO(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_4)
#define HDMITX_IOC_CONNECT               _IOW(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_5, HDMITX_CONNECT_PARA_T)
#define HDMITX_IOC_DISCONNECT            _IOW(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_6, UINT8)
#define HDMITX_IOC_RUN                   _IOW(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_7, UINT8)
#define HDMITX_IOC_POLLING_STATUS        _IOWR(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_8, HDMITX_HPDSTATUS_PARA_T)
#define HDMITX_IOC_GETTIMINGINFO         _IOWR(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_9, KADP_VBE_HDMITX_TIMING_INFO_T)
#define HDMITX_IOC_READEDID              _IOWR(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_10, KADP_VBE_HDMITX_EDID_INFO_T)
#define HDMITX_IOC_HDCP_GETAUTHVER       _IOWR(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_11, KADP_VBE_HDMITX_HDCP_VER_INFO_T)
#define HDMITX_IOC_HDCP_SETAUTHVER       _IOW(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_12, KADP_VBE_HDMITX_HDCP_VER_INFO_T)
#define HDMITX_IOC_HDCP_GETAUTHSTATUS    _IOWR(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_13, KADP_VBE_HDMITX_HDCP_STATUS_INFO_T)
#define HDMITX_IOC_HDCP_SETAUDIOEN       _IOW(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_14, KADP_VBE_HDMITX_AUDIO_STATUS_INFO_T)
#define HDMITX_IOC_GETTXPORTCNT          _IOWR(HDMITX_IOC_MAGIC, HDMITX_IOC_IDX_15, UINT8)


#endif

