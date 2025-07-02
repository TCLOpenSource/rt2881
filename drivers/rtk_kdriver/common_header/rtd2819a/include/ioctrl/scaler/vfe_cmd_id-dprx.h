#ifndef __VFE_CMD_ID_DPRX_H__
#define __VFE_CMD_ID_DPRX_H__

//==============================================================
// DPRX VFE DATA TYPES
//==============================================================

//-------------------------------------------------------------------------------
// VFE_DPRX_CTRL_T : 32 bits command for DPRX VFE control
//-------------------------------------------------------------------------------
// a control command is used to trigger hw operations, and
// it is write only.
//
// [31:20]   cmd  (12 bits)  : which operations to do
// [19:16]   port (4 bits)   : which port to associate with the cmd.
// [15: 0]   value (16 bits) : reserved
//-------------------------------------------------------------------------------
typedef unsigned int        VFE_DPRX_CTRL_T;

#define _VFE_DPRX_CTRL_CMD(cmd, port, value)      (((cmd & 0xFFF)<<20) | ((port & 0xF)<<16) | (value & 0xFFFF))

#define GET_VFE_DPRX_CTRL_CMD(x)                 ((x >> 20) & 0xFFF)
#define GET_VFE_DPRX_CTRL_PORT(x)                ((x >> 16) & 0xF)
#define GET_VFE_DPRX_CTRL_VALUE(x)               (x  & 0xFFFF)

//-------------------------------------------------------------------------------
// VFE_DPRX_ATTR : Attribute of a DPRX VFE
//
// 64 bits deivce attribute. An attr is indiciated by a
// 16 bits attr id and go with either a 32 bits data
// (data_array=0) or a data array (data_array=1) that up
// to 32767 bytes.
//
// [63:48]  attr : 16 bits attribute id
//
// [47:47]  data_array
//              0 : data is a 32 bits interger
//              1 : data is a 32 bits pointer
//
// [46: 32] array_size (15 bits) : (valid when data_array=1)
//
// [31:  0] data  / array address
//-------------------------------------------------------------------------------

typedef struct
{
    unsigned int attr:12;       // see ATTR (the highest 2 bits are reserved for attribute check)
    unsigned int port:4;
    unsigned int data_array:1;  // 0 : data, 1 : pointer
    unsigned int data_len:15;   // number of bytes : 0 ~ 0x7FFF (32767)
    union
    {
        unsigned int data;          // data_array = 0
        PBYTES       p_data;        // data_array = 1  : pointer of data array
    };
}VFE_DPRX_ATTR;

enum
{
    VFE_DPRX_ATTR_CAP_RO = (1<<0),
    VFE_DPRX_ATTR_CAP_WO = (1<<1),
    VFE_DPRX_ATTR_CAP_RW = (VFE_DPRX_ATTR_CAP_RO | VFE_DPRX_ATTR_CAP_WO),
};

#define _VFE_DPRX_ATTR(rw_cap, attr_id)   (((rw_cap & 0x3)<<10) | (attr_id & 0x3FF))
#define _VFE_DPRX_RO_ATTR(attr_id)         _VFE_DPRX_ATTR(VFE_DPRX_ATTR_CAP_RO, attr_id)
#define _VFE_DPRX_WO_ATTR(attr_id)         _VFE_DPRX_ATTR(VFE_DPRX_ATTR_CAP_WO, attr_id)
#define _VFE_DPRX_RW_ATTR(attr_id)         _VFE_DPRX_ATTR(VFE_DPRX_ATTR_CAP_RW, attr_id)

#define ATTR_MASK                         0xFFF
#define GET_ATTR_CAP(attr)                ((attr >>10) & 0x3)
#define IS_ATTR_READABLE(attr)            (GET_ATTR_CAP(attr) & VFE_DPRX_ATTR_CAP_RO)
#define IS_ATTR_WRITABLE(attr)            (GET_ATTR_CAP(attr) & VFE_DPRX_ATTR_CAP_WO)
#define GET_ATTR_ID(attr)                 (attr & 0x3FF)

//===============================================================================
// DPRX IOCTL CMD ID
//===============================================================================

#define VFE_IOC_DPRX_VFE_CTRL_IDX           VFE_IOC_DPRX_IDX_0
#define VFE_IOC_DPRX_HPD_DET_CTRL_IDX       VFE_IOC_DPRX_IDX_1
#define VFE_IOC_DPRX_EDID_CTRL_IDX          VFE_IOC_DPRX_IDX_2
#define VFE_IOC_DPRX_HDCP_CTRL_IDX          VFE_IOC_DPRX_IDX_3
#define VFE_IOC_DPRX_VIDEO_CTRL_IDX         VFE_IOC_DPRX_IDX_4
#define VFE_IOC_DPRX_AUDIO_CTRL_IDX         VFE_IOC_DPRX_IDX_5
#define VFE_IOC_DPRX_SDP_CTRL_IDX           VFE_IOC_DPRX_IDX_6

#define VFE_IOC_DPRX_VFE_CTRL               _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_VFE_CTRL_IDX,     VFE_DPRX_CTRL_T)
#define VFE_IOC_DPRX_VFE_GET_ATTR           _IOR(VFE_IOC_MAGIC,  VFE_IOC_DPRX_VFE_CTRL_IDX,     VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_VFE_SET_ATTR           _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_VFE_CTRL_IDX,     VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_HPD_DET_CTRL           _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_HPD_DET_CTRL_IDX, VFE_DPRX_CTRL_T)
#define VFE_IOC_DPRX_HPD_DET_GET_ATTR       _IOR(VFE_IOC_MAGIC,  VFE_IOC_DPRX_HPD_DET_CTRL_IDX, VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_HPD_DET_SET_ATTR       _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_HPD_DET_CTRL_IDX, VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_EDID_CTRL              _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_EDID_CTRL_IDX,    VFE_DPRX_CTRL_T)
#define VFE_IOC_DPRX_EDID_GET_ATTR          _IOR(VFE_IOC_MAGIC,  VFE_IOC_DPRX_EDID_CTRL_IDX,    VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_EDID_SET_ATTR          _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_EDID_CTRL_IDX,    VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_HDCP_CTRL              _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_HDCP_CTRL_IDX,    VFE_DPRX_CTRL_T)
#define VFE_IOC_DPRX_HDCP_GET_ATTR          _IOR(VFE_IOC_MAGIC,  VFE_IOC_DPRX_HDCP_CTRL_IDX,    VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_HDCP_SET_ATTR          _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_HDCP_CTRL_IDX,    VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_VIDEO_CTRL             _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_VIDEO_CTRL_IDX,   VFE_DPRX_CTRL_T)
#define VFE_IOC_DPRX_VIDEO_GET_ATTR         _IOR(VFE_IOC_MAGIC,  VFE_IOC_DPRX_VIDEO_CTRL_IDX,   VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_VIDEO_SET_ATTR         _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_VIDEO_CTRL_IDX,   VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_AUDIO_CTRL             _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_AUDIO_CTRL_IDX,   VFE_DPRX_CTRL_T)
#define VFE_IOC_DPRX_AUDIO_GET_ATTR         _IOR(VFE_IOC_MAGIC,  VFE_IOC_DPRX_AUDIO_CTRL_IDX,   VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_AUDIO_SET_ATTR         _IOW(VFE_IOC_MAGIC,  VFE_IOC_DPRX_AUDIO_CTRL_IDX,   VFE_DPRX_ATTR)
#define VFE_IOC_DPRX_SDP_GET_ATTR           _IOR(VFE_IOC_MAGIC,  VFE_IOC_DPRX_SDP_CTRL_IDX,     VFE_DPRX_ATTR)

#define GET_DPRX_VFE_IOC_IDX(x)             _IOC_NR(x)

//-------------------------------------------------------------------------------
// DPRX VFE related
//-------------------------------------------------------------------------------
//  Control Command
enum
{
    DPRX_VFE_CTRL_INIT = 0,
    DPRX_VFE_CTRL_UNINIT,
    DPRX_VFE_CTRL_OPEN,
    DPRX_VFE_CTRL_CLOSE,
    DPRX_VFE_CTRL_CONNECT,
    DPRX_VFE_CTRL_DISCONNECT,
    DPRX_VFE_CTRL_MAX,
};

typedef enum
{
    KADP_VFE_DPRX_WAKEUP_MODE_NO_WAKEUP = 0,
    KADP_VFE_DPRX_WAKEUP_MODE_BY_AUX,
    KADP_VFE_DPRX_WAKEUP_MODE_BY_SIGNAL,
    KADP_VFE_DPRX_WAKEUP_MODE_BY_AUX_OR_SIGNAL,
}KADP_VFE_DPRX_WAKEUP_MODE_E;

#define VFE_DPRX_CTRL_INIT                  _VFE_DPRX_CTRL_CMD(DPRX_VFE_CTRL_INIT, 0, 0)
#define VFE_DPRX_CTRL_QS_INIT               _VFE_DPRX_CTRL_CMD(DPRX_VFE_CTRL_INIT, 1, 0)
#define VFE_DPRX_CTRL_UNINIT                _VFE_DPRX_CTRL_CMD(DPRX_VFE_CTRL_UNINIT, 0, 0)
#define VFE_DPRX_CTRL_OPEN                  _VFE_DPRX_CTRL_CMD(DPRX_VFE_CTRL_OPEN, 0, 0)
#define VFE_DPRX_CTRL_CLOSE                 _VFE_DPRX_CTRL_CMD(DPRX_VFE_CTRL_CLOSE, 0, 0)
#define VFE_DPRX_CTRL_CONNECT(port)         _VFE_DPRX_CTRL_CMD(DPRX_VFE_CTRL_CONNECT, port, 0)
#define VFE_DPRX_CTRL_DISCONNECT(port)      _VFE_DPRX_CTRL_CMD(DPRX_VFE_CTRL_DISCONNECT, port, 0)


typedef struct
{
    // SDP data
    unsigned char                           hb[4];
    unsigned char                           pb[32];
    // extra information
    unsigned int                          seq_num;
    unsigned int                          time_stamp;
}KADP_VFE_DPRX_PORT_SDP_DATA_T;

typedef enum
{
    KADP_VFE_DPRX_PORT_SDP_INFOFRAME_VSC = 0,
    KADP_VFE_DPRX_PORT_SDP_INFOFRAME_HDR,
    KADP_VFE_DPRX_PORT_SDP_INFOFRAME_SPD,
    KADP_VFE_DPRX_PORT_SDP_INFOFRAME_DVS,
    KADP_VFE_DPRX_PORT_SDP_INFOFRAME_ADP_SYNC,
    KADP_VFE_DPRX_PORT_SDP_INFOFRAME_VSC_EXT_VESA,
    KADP_VFE_DPRX_PORT_SDP_INFOFRAME_AVI,
}KADP_VFE_DPRX_PORT_SDP_TYPE_E;

#define VFE_DPRX_GET_SDP_DATA(type)         _VFE_DPRX_RO_ATTR(type)

// attributes
enum
{
    DPRX_VFE_ATTR_RX_PORT_CNT = 0,          // number of DPRx Port
    DPRX_VFE_ATTR_RX_FAST_SWITCH_SUPPORT,
    DPRX_VFE_ATTR_PORT_CLOCK_STATUS,        // going to be removed from v0.0.2
    DPRX_VFE_ATTR_PORT_CAPABILITY,
    DPRX_VFE_ATTR_PORT_LINK_STATUS,
    DPRX_VFE_ATTR_PORT_STREAM_TYPE,
    DPRX_VFE_ATTR_PORT_WAKE_UP_BY_AUX,
    DPRX_VFE_ATTR_PORT_SET_WAKE_UP_MODE,
    DPRX_VFE_ATTR_PORT_SET_VRR_ENABLE,
};

#define VFE_DPRX_VFE_RO_ATTR_RX_PORT_CNT        _VFE_DPRX_RO_ATTR(DPRX_VFE_ATTR_RX_PORT_CNT)
#define VFE_DPRX_VFE_RO_ATTR_RX_FAST_SWITCH_SUPPORT        _VFE_DPRX_RO_ATTR(DPRX_VFE_ATTR_RX_FAST_SWITCH_SUPPORT)
#define VFE_DPRX_VFE_RO_ATTR_PORT_CLOCK_STATUS  _VFE_DPRX_RO_ATTR(DPRX_VFE_ATTR_PORT_CLOCK_STATUS)
#define VFE_DPRX_VFE_RO_ATTR_PORT_CAPABILITY    _VFE_DPRX_RO_ATTR(DPRX_VFE_ATTR_PORT_CAPABILITY)
#define VFE_DPRX_VFE_RO_ATTR_PORT_LINK_STATUS   _VFE_DPRX_RO_ATTR(DPRX_VFE_ATTR_PORT_LINK_STATUS)
#define VFE_DPRX_VFE_RO_ATTR_PORT_STREAM_TYPE   _VFE_DPRX_RO_ATTR(DPRX_VFE_ATTR_PORT_STREAM_TYPE)
#define VFE_DPRX_VFE_RW_ATTR_PORT_WAKE_UP_BY_AUX   _VFE_DPRX_RW_ATTR(DPRX_VFE_ATTR_PORT_WAKE_UP_BY_AUX)
#define VFE_DPRX_VFE_RW_ATTR_PORT_SET_WAKE_UP_MODE   _VFE_DPRX_RW_ATTR(DPRX_VFE_ATTR_PORT_SET_WAKE_UP_MODE)
#define VFE_DPRX_VFE_RW_ATTR_PORT_SET_VRR_ENABLE     _VFE_DPRX_RW_ATTR(DPRX_VFE_ATTR_PORT_SET_VRR_ENABLE)

// data types

typedef enum
{
    KADP_VFE_DPRX_PORT_TYPE_NONE = 0,
    KADP_VFE_DPRX_PORT_TYPE_DP = 1,
    KADP_VFE_DPRX_PORT_TYPE_MDP = 2,
    KADP_VFE_DPRX_PORT_TYPE_TYPEC = 3,
    KADP_VFE_DPRX_PORT_TYPE_MAX,
}KADP_VFE_DPRX_PORT_TYPE_E;

typedef enum
{
    KADP_VFE_DPRX_PORT_LANE_CNT_NONE = 0,
    KADP_VFE_DPRX_PORT_LANE_CNT_1 = 1,
    KADP_VFE_DPRX_PORT_LANE_CNT_2 = 2,
    KADP_VFE_DPRX_PORT_LANE_CNT_4 = 4,
}KADP_VFE_DPRX_PORT_LANE_CNT_E;

typedef enum
{
    KADP_VFE_DPRX_PORT_LINK_RATE_RBR = 0,
    KADP_VFE_DPRX_PORT_LINK_RATE_HBR1,
    KADP_VFE_DPRX_PORT_LINK_RATE_HBR2,
    KADP_VFE_DPRX_PORT_LINK_RATE_HBR3,
    KADP_VFE_DPRX_PORT_LINK_RATE_UHBR10,
    KADP_VFE_DPRX_PORT_LINK_RATE_UHBR13P5,
    KADP_VFE_DPRX_PORT_LINK_RATE_UHBR20,
    KADP_VFE_DPRX_PORT_LINK_RATE_MAX,
}KADP_VFE_DPRX_PORT_LINK_RATE_E;

#define KADP_VFE_DPRX_PORT_LINK_RATE_MASK(x)       (1<<x)
#define KADP_VFE_DPRX_PORT_LINK_RATE_RBR_MASK      KADP_VFE_DPRX_PORT_LINK_RATE_MASK(KADP_VFE_DPRX_PORT_LINK_RATE_RBR)
#define KADP_VFE_DPRX_PORT_LINK_RATE_HBR1_MASK     KADP_VFE_DPRX_PORT_LINK_RATE_MASK(KADP_VFE_DPRX_PORT_LINK_RATE_HBR1)
#define KADP_VFE_DPRX_PORT_LINK_RATE_HBR2_MASK     KADP_VFE_DPRX_PORT_LINK_RATE_MASK(KADP_VFE_DPRX_PORT_LINK_RATE_HBR2)
#define KADP_VFE_DPRX_PORT_LINK_RATE_HBR3_MASK     KADP_VFE_DPRX_PORT_LINK_RATE_MASK(KADP_VFE_DPRX_PORT_LINK_RATE_HBR3)
#define KADP_VFE_DPRX_PORT_LINK_RATE_UHBR10_MASK   KADP_VFE_DPRX_PORT_LINK_RATE_MASK(KADP_VFE_DPRX_PORT_LINK_RATE_UHBR10)
#define KADP_VFE_DPRX_PORT_LINK_RATE_UHBR13P5_MASK KADP_VFE_DPRX_PORT_LINK_RATE_MASK(KADP_VFE_DPRX_PORT_LINK_RATE_UHBR13P5)
#define KADP_VFE_DPRX_PORT_LINK_RATE_UHBR20_MASK   KADP_VFE_DPRX_PORT_LINK_RATE_MASK(KADP_VFE_DPRX_PORT_LINK_RATE_UHBR20)

typedef enum
{
    KADP_VFE_DPRX_PORT_CAP_DSC_1P3 = 0,
    KADP_VFE_DPRX_PORT_CAP_FEC,
    KADP_VFE_DPRX_PORT_CAP_MAX,
}KADP_VFE_DPRX_PORT_CAP_E;

#define KADP_VFE_DPRX_PORT_CAP_MASK(x)           (1<<x)
#define KADP_VFE_DPRX_PORT_CAP_DSC_1P3_MASK      KADP_VFE_DPRX_PORT_CAP_MASK(KADP_VFE_DPRX_PORT_CAP_DSC_1P3)
#define KADP_VFE_DPRX_PORT_CAP_FEC_MASK          KADP_VFE_DPRX_PORT_CAP_MASK(KADP_VFE_DPRX_PORT_CAP_FEC)

typedef struct
{
    KADP_VFE_DPRX_PORT_TYPE_E      type;
    KADP_VFE_DPRX_PORT_LANE_CNT_E  max_lane_cnt;
    unsigned int                   link_rate_mask : 16;   // bit mask with HAL_VFE_DPRX_PORT_LINK_RATE_XXX_MASK
    unsigned int                   capablity_mask : 16;   // bit mask with HAL_VFE_DPRX_PORT_CAP_XXX_MASK
}KADP_VFE_DPRX_PORT_CAP_T;


typedef struct
{
    unsigned int valid:1;
    unsigned int lock:1;
    unsigned int reserved:14;
    unsigned int error_cnt:16;
} KADP_VFE_DPRX_PORT_LANE_STATUS_T;

typedef struct
{
    KADP_VFE_DPRX_PORT_LANE_CNT_E    lane_mode;
    KADP_VFE_DPRX_PORT_LINK_RATE_E   link_rate;
    unsigned int                     ch_aligned:1;
    unsigned int                     reserved:31;
    KADP_VFE_DPRX_PORT_LANE_STATUS_T lane_status[4];
} KADP_VFE_DPRX_PORT_LINK_STATUS_T;

typedef enum
{
    KADP_VFE_DPRX_STREAM_TYPE_UNKNOWN =0,
    KADP_VFE_DPRX_STREAM_TYPE_AUDIO_ONLY,        // DP Stram is audio only
    KADP_VFE_DPRX_STREAM_TYPE_VIDEO_ONLY,        // DP Stram is video only
    KADP_VFE_DPRX_STREAM_TYPE_AUDIO_VIDEO,       // DP Stram has audio and video
    KADP_VFE_DPRX_STREAM_TYPE_MAX,
} KADP_VFE_DPRX_STREAM_TYPE_E;

//-------------------------------------------------------------------------------
// DPRX HPD/DET related
//-------------------------------------------------------------------------------


// Controls
enum
{
    DPRX_HPD_CTRL_ENABLE_HPD = 0,    // enable/disable of all dprx port
    DPRX_HPD_CTRL_SET_HPD,           // enable/disable of a given dprx port
    DPRX_HPD_CTRL_HPD_TOGGLE,        // toggle hpd of a given dprx port
    DPRX_HPD_CTRL_MAX,
};

#define VFE_DPRX_HPD_CTRL_ENABLE_HPD(on)        _VFE_DPRX_CTRL_CMD(DPRX_HPD_CTRL_ENABLE_HPD, 0, on)
#define VFE_DPRX_HPD_CTRL_SET_HPD(port, hpd)    _VFE_DPRX_CTRL_CMD(DPRX_HPD_CTRL_SET_HPD,    port, hpd)
#define VFE_DPRX_HPD_CTRL_HPD_TOGGLE            _VFE_DPRX_CTRL_CMD(DPRX_HPD_CTRL_HPD_TOGGLE, 0, 0)

// Attributes
enum
{
    DPRX_HPD_ATTR_HPD_LOW_DELAY = 0,
    DPRX_HPD_ATTR_DPRX_CONNECT_STATE,
    DPRX_HPD_ATTR_MAX,
};

#define VFE_DPRX_HPD_RW_ATTR_HPD_LOW_DELAY      _VFE_DPRX_RW_ATTR(DPRX_HPD_ATTR_HPD_LOW_DELAY)
#define VFE_DPRX_HPD_RO_ATTR_DPRX_CONNECT_STATE _VFE_DPRX_RO_ATTR(DPRX_HPD_ATTR_DPRX_CONNECT_STATE)

//-------------------------------------------------------------------------------
// DPRX EDID related
//-------------------------------------------------------------------------------


// Controls
enum
{
    // N/A reserved
    DPRX_EDID_CTRL_MAX,
};

// Attributes
enum
{
    DPRX_EDID_ATTR_DPRX_EDID,
    DPRX_EDID_ATTR_MAX,
};

#define VFE_DPRX_EDID_RW_ATTR_DPRX_EDID     _VFE_DPRX_RW_ATTR(DPRX_EDID_ATTR_DPRX_EDID)

#define IS_VALID_EDID_SIZE(x)               ((x==128) || (x==256) || (x==384)|| (x==512))
#define IS_VALID_DPRX_EDID_ATTR_DATA(attr)  ((attr).data_array==1 && IS_VALID_EDID_SIZE((attr).data_len))

//-------------------------------------------------------------------------------
// vfe_dprx_drv_read_edid :
//-------------------------------------------------------------------------------
//   VFE_DPRX_ATTR attr;
//
//     attr.attr       = VFE_DPRX_EDID_RW_ATTR_DPRX_EDID;
//     attr.port       = 0/1/2/3...;
//     attr.data_array = 1;
//     attr.data_len   = sizeof(edid);   //should be 128/256/384/512
//     attr.p_data     = edid;
//
//     if (ioctl(vfe_fd, VFE_IOC_DPRX_EDID_SET_ATTR, &attr) == 0)
//         return KADP_OK ;
//
//-------------------------------------------------------------------------------
// vfe_dprx_drv_write_edid :
//-------------------------------------------------------------------------------
//   VFE_DPRX_ATTR attr;
//
//     attr.attr       = VFE_DPRX_EDID_RW_ATTR_DPRX_EDID;
//     attr.port       = 0/1/2/3...;
//     attr.data_array = 1;
//     attr.data_len   = sizeof(edid);   //should be 128/256/384/512
//     attr.p_data     = edid;
//
//     if (ioctl(vfe_fd, VFE_IOC_DPRX_EDID_SET_ATTR, &attr) == 0)
//         return KADP_OK ;
//
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// DPRX HDCP related
//-------------------------------------------------------------------------------

// Controls
enum
{
    DPRX_HDCP_CTRL_FORCE_REAUTH,
    DPRX_HDCP_CTRL_MAX,
};

#define VFE_DPRX_HDCP_CTRL_FORCE_REAUTH      _VFE_DPRX_CTRL_CMD(DPRX_HDCP_CTRL_FORCE_REAUTH, 0, 0)

// Attributes
enum
{
    DPRX_HDCP_ATTR_CAPABILITY,
    DPRX_HDCP_ATTR_HDCP2_ENABLE,         // enable / disable hdcp
    DPRX_HDCP_ATTR_HDCP1x_KEY,           // hdcp 1.x key
    DPRX_HDCP_ATTR_HDCP2x_KEY,           // hdcp 2.x key
    DPRX_HDCP_ATTR_DPRX_HDCP_STATUS,
    DPRX_HDCP_ATTR_MAX,
};

#define DPRX_HDCP1X_KEY_SIZE        325
#define DPRX_HDCP2X_KEY_SIZE        878

#define VFE_DPRX_HDCP_RO_ATTR_CAPABILITY     _VFE_DPRX_RO_ATTR(DPRX_HDCP_ATTR_CAPABILITY)
#define VFE_DPRX_HDCP_RW_ATTR_HDCP2_ENABLE   _VFE_DPRX_RW_ATTR(DPRX_HDCP_ATTR_HDCP2_ENABLE)
#define VFE_DPRX_HDCP_WO_ATTR_HDCP1x_KEY     _VFE_DPRX_WO_ATTR(DPRX_HDCP_ATTR_HDCP1x_KEY)
#define VFE_DPRX_HDCP_WO_ATTR_HDCP2x_KEY     _VFE_DPRX_WO_ATTR(DPRX_HDCP_ATTR_HDCP2x_KEY)
#define VFE_DPRX_HDCP_RO_DPRX_HDCP_STATUS    _VFE_DPRX_RO_ATTR(DPRX_HDCP_ATTR_DPRX_HDCP_STATUS)

//----------------------------------------------
// data types
//----------------------------------------------

typedef enum
{
    VFE_DP_HDCP_VER_HDCP13 = 0,
    VFE_DP_HDCP_VER_HDCP22,
    VFE_DP_HDCP_VER_HDCP23,
}VFE_DP_HDCP_VER_E;

typedef enum
{
    VFE_DP_HDCP_MODE_OFF = 0,
    VFE_DP_HDCP_MODE_HDCP_1P3,
    VFE_DP_HDCP_MODE_HDCP_2P2,
    VFE_DP_HDCP_MODE_HDCP_2P3,
}VFE_DP_HDCP_MODE;

typedef enum
{
    VFE_DP_HDCP_STATE_IDEL = 0,
    VFE_DP_HDCP_STATE_AUTH_START,
    VFE_DP_HDCP_STATE_AUTH_DONE,
}VFE_DP_HDCP_STATE;

typedef enum
{
    VFE_DP_HDCP_CAPBILITY_HDCP13 = (0x1 << VFE_DP_HDCP_VER_HDCP13),
    VFE_DP_HDCP_CAPBILITY_HDCP22 = (0x1 << VFE_DP_HDCP_VER_HDCP22),
    VFE_DP_HDCP_CAPBILITY_HDCP23 = (0x1 << VFE_DP_HDCP_VER_HDCP23),
    VFE_DP_HDCP_CAPBILITY_HDCP_REPEATER = (0x1 << 7),
}VFE_DP_HDCP_CAPABILITY_E;

#define VFE_HDCP1X_KSV_INFO_LEN     5
#define VFE_HDCP1X_KEY_SIZE         325

#define VFE_HDCP2X_RECEIVER_ID_LEN  5
#define VFE_HDCP2X_KEY_SIZE         878

typedef struct
{
    VFE_DP_HDCP_MODE    mode;       // current HDCP mode
    VFE_DP_HDCP_STATE   state;      // hdcp state

    struct
    {
        unsigned char   aksv[VFE_HDCP1X_KSV_INFO_LEN];
        unsigned char   bksv[VFE_HDCP1X_KSV_INFO_LEN];
    }hdcp1x;

    struct
    {
        unsigned char   receiverid[VFE_HDCP2X_RECEIVER_ID_LEN];
    }hdcp2x;

}VFE_DPRX_HDCP_STATUS_T;


//-------------------------------------------------------------------------------
// vfe_dprx_drv_get_hdcp_capability :
//-------------------------------------------------------------------------------
//   VFE_DPRX_ATTR attr;
//
//     attr.attr       = VFE_DPRX_HDCP_RO_ATTR_CAPABILITY;
//     attr.port       = x;  //don't care
//     attr.data_type  = 0;
//     attr.data_len   = 0;
//     attr.data       = 0;
//
//     if (ioctl(vfe_fd, VFE_IOC_DPRX_HDCP_GET_ATTR, &attr) == 0)
//         return KADP_OK ;
//
//     hdcp_cap = attr.data
//-------------------------------------------------------------------------------
// vfe_dprx_drv_write_hdcp_key :
//-------------------------------------------------------------------------------
//   VFE_DPRX_ATTR attr;
//
//     attr.attr       = VFE_DPRX_HDCP_RO_ATTR_HDCP1x_KEY;
//     attr.port       = x;  //don't care
//     attr.data_type  = 1;
//     attr.data_len   = HDCP1X_KEY_SIZE;
//     attr.data       = p_key;
//
//     if (ioctl(vfe_fd, VFE_IOC_DPRX_HDCP_SET_ATTR, &attr) == 0)
//         return KADP_OK ;
//
//     attr.attr       = VFE_DPRX_HDCP_RO_ATTR_HDCP2x_KEY;
//     attr.port       = x;  //don't care
//     attr.data_type  = 1;
//     attr.data_len   = HDCP2X_KEY_SIZE;
//     attr.data       = p_key;
//
//     if (ioctl(vfe_fd, VFE_IOC_DPRX_HDCP_SET_ATTR, &attr) == 0)
//         return KADP_OK ;
//
//-------------------------------------------------------------------------------
// vfe_dprx_drv_get_hdcp_status :
//-------------------------------------------------------------------------------
//     VFE_DPRX_ATTR attr;
//
//     attr.attr       = VFE_DPRX_HDCP_RO_DPRX_HDCP_STATUS;
//     attr.port       = 0/1/2/3;
//     attr.data_type  = 1;
//     attr.data_len   = sizeof(DPRX_HDCP_STATUS_T);
//     attr.data       = (DPRX_HDCP_STATUS_T *) p_status;
//
//     if (ioctl(vfe_fd, VFE_IOC_DPRX_HDCP_GET_ATTR, &attr) == 0)
//         return KADP_OK ;
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// DPRX VIDEO related
//-------------------------------------------------------------------------------


// Controls
enum
{
    DPRX_VIDEO_CTRL_MAX,
};

// Attributes
enum
{
    DPRX_VIDEO_ATTR_VIDEO_TIMING,
    DPRX_VIDEO_ATTR_AVI_INFO,
    DPRX_VIDEO_ATTR_DRM_INFO,
    DPRX_VIDEO_ATTR_PIXEL_ENCODING_FORMAT,
    DPRX_VIDEO_ATTR_MAX,
};

#define VFE_DPRX_VIDEO_RO_ATTR_VIDEO_TIMING  _VFE_DPRX_RO_ATTR(DPRX_VIDEO_ATTR_VIDEO_TIMING)
#define VFE_DPRX_VIDEO_RO_ATTR_AVI_INFO  _VFE_DPRX_RO_ATTR(DPRX_VIDEO_ATTR_AVI_INFO)
#define VFE_DPRX_VIDEO_RO_ATTR_DRM_INFO  _VFE_DPRX_RO_ATTR(DPRX_VIDEO_ATTR_DRM_INFO)
#define VFE_DPRX_VIDEO_RO_ATTR_PIXEL_ENCODING_FORMAT  _VFE_DPRX_RO_ATTR(DPRX_VIDEO_ATTR_PIXEL_ENCODING_FORMAT)
// data types

typedef enum
{
    KADP_VFE_DPRX_PTG_REGEN_HV_MODE = 0,
    KADP_VFE_DPRX_PTG_REGEN_NO_VSYNC_MODE,
    KADP_VFE_DPRX_PTG_REGEN_RESERVE_0,
    KADP_VFE_DPRX_PTG_REGEN_RESERVE_1,
}KADP_VFE_DPRX_PTG_REGEN_E;

typedef enum
{
    KADP_VFE_DPRX_COLOR_SPACE_RGB = 0,
    KADP_VFE_DPRX_COLOR_SPACE_YUV444,
    KADP_VFE_DPRX_COLOR_SPACE_YUV422,
    KADP_VFE_DPRX_COLOR_SPACE_YUV420,
    KADP_VFE_DPRX_COLOR_SPACE_YONLY,
    KADP_VFE_DPRX_COLOR_SPACE_RAW,
    KADP_VFE_DPRX_COLOR_SPACE_UNKNOWN,
}KADP_VFE_DPRX_COLOR_SPACE_E;


typedef enum
{
    KADP_VFE_DPRX_COLOR_DEPTH_6B = 6,
    KADP_VFE_DPRX_COLOR_DEPTH_7B = 7,
    KADP_VFE_DPRX_COLOR_DEPTH_8B = 8,
    KADP_VFE_DPRX_COLOR_DEPTH_10B = 10,
    KADP_VFE_DPRX_COLOR_DEPTH_12B = 12,
    KADP_VFE_DPRX_COLOR_DEPTH_14B = 14,
    KADP_VFE_DPRX_COLOR_DEPTH_16B = 16,
    KADP_VFE_DPRX_COLOR_DEPTH_UNKNOWN = 0,
}KADP_VFE_DPRX_COLOR_DEPTH_E;

typedef enum
{
    KADP_VFE_DPRX_DRR_MODE_FRR = 0,              // FIX Refresh Rate
    KADP_VFE_DPRX_DRR_MODE_AMD_FREE_SYNC = 1,    // AMD Free Sync
    KADP_VFE_DPRX_DRR_MODE_ADAPTIVE_SYNC,        // DP Adaptive Sync
    KADP_VFE_DPRX_DRR_MODE_UNKNWON,
}KADP_VFE_DPRX_DRR_MODE_E;

typedef enum
{
    KADP_VFE_DPRX_COLORIMETRY_LEGACY_RGB = 0,
    KADP_VFE_DPRX_COLORIMETRY_SRGB,
    KADP_VFE_DPRX_COLORIMETRY_XRGB,
    KADP_VFE_DPRX_COLORIMETRY_SCRGB,
    KADP_VFE_DPRX_COLORIMETRY_ADOBE_RGB,
    KADP_VFE_DPRX_COLORIMETRY_DCI_P3,
    KADP_VFE_DPRX_COLORIMETRY_CUSTOM_COLOR_PROFILE,
    KADP_VFE_DPRX_COLORIMETRY_BT_2020_RGB,
    KADP_VFE_DPRX_COLORIMETRY_BT_601,
    KADP_VFE_DPRX_COLORIMETRY_BT_709,
    KADP_VFE_DPRX_COLORIMETRY_XV_YCC_601,
    KADP_VFE_DPRX_COLORIMETRY_XV_YCC_709,
    KADP_VFE_DPRX_COLORIMETRY_S_YCC_601,
    KADP_VFE_DPRX_COLORIMETRY_OP_YCC_601,
    KADP_VFE_DPRX_COLORIMETRY_BT_2020_YCCBCCRC,
    KADP_VFE_DPRX_COLORIMETRY_BT_2020_YCBCR,
    KADP_VFE_DPRX_COLORIMETRY_DICOM_PS314,
    KADP_VFE_DPRX_COLORIMETRY_YONLY,
    KADP_VFE_DPRX_COLORIMETRY_RAW,
    KADP_VFE_DPRX_COLORIMETRY_UNKNOWN,
}KADP_VFE_DPRX_COLORIMETRY_E;

typedef enum
{
    KADP_VFE_DPRX_DYNAMIC_RANGE_VESA =0, //full range
    KADP_VFE_DPRX_DYNAMIC_RANGE_CTA,        //limited range
}KADP_VFE_DPRX_DYNAMIC_RANGE_E;

typedef enum
{
    KADP_VFE_DPRX_CONTENT_TYPE_NOT_DEFINED =0,
    KADP_VFE_DPRX_CONTENT_TYPE_GRAPHICS,
    KADP_VFE_DPRX_CONTENT_TYPE_PHOTO,
    KADP_VFE_DPRX_CONTENT_TYPE_VIDEO,
    KADP_VFE_DPRX_CONTENT_TYPE_GAME,
}KADP_VFE_DPRX_CONTENT_TYPE_E;

typedef struct
{
    KADP_VFE_DPRX_COLOR_SPACE_E  color_space;
    KADP_VFE_DPRX_COLOR_DEPTH_E  color_depth;
    KADP_VFE_DPRX_PTG_REGEN_E ptg_mode;

    unsigned short htotal;      // htotal (unit:pixel) HDE rising to HDE rising
    unsigned short hporch;      // hproch (unit:pixel) HDE falling to HDE rising
    unsigned short hstart;      // hstart (unit:pixel) HS rising edge to HDE rising
    unsigned short hsync;       // hsync  (unit:pixel) HS rising to HS falling
    unsigned short hact;        // hact   (unit:pixel) HDE rising to HDE falling
    unsigned short vtotal;      // vtotal (unit:line)  VDE rising to VDE rising
    unsigned short vporch;      // vporch (unit:line)  VDE falling to VDE rising
    unsigned short vstart;      // vporch (unit:line)  VS rising to VDE rising
    unsigned short vsync;       // vsync  (unit:line)  VS rising to VS falling
    unsigned short vact;        // vact   (unit:line)  VDE rising to VDE falling

    unsigned short hfreq_hz;    // H freq
    unsigned short vfreq_hz_x100;  // vfreq  (unit: 0.01Hz)

    unsigned short hs_polarity : 1;  // hs polarith : 0 : negative, 1 : positive
    #define HS_POL_POSITIVE  1
    #define HS_POL_NEGAITVE  0

    unsigned short vs_polarity : 1;  // vs polarith : 0 : negative, 1 : positive
    #define VS_POL_POSITIVE  1
    #define VS_POL_NEGAITVE  0

    unsigned short is_interlace : 1;
    unsigned short is_dsc : 1;
    unsigned short drr_mode : 4;  // see DP_DRR_MODE_E
    unsigned short is_dolby_hdr : 1;
    unsigned short ext_flags : 7;
    #define KADP_VFE_DPRX_TIMING_OVER_DISP_LIMIT  0x1    // video is not able to be playback by DISP
    #define KADP_VFE_DPRX_TIMING_BYPASSABLE       0x2    // video is able to bypass without DISP

    unsigned short curr_vfreq_hz_x100;  // current vfreq (unit: 0.01Hz) only valid when drr_mode != DP_DRR_MODE_FRR
    unsigned char isALLM;
    unsigned short pixel_mode;
    #define DPRX_OUT_PIXEL_MODE_1P   1
    #define DPRX_OUT_PIXEL_MODE_2P   2
    #define DPRX_OUT_PIXEL_MODE_4P   4
}KADP_VFE_DPRX_TIMING_INFO_T;

typedef enum
{
    KADP_HAL_DPRX_DRM_EOTF_SDR_LUMINANCE_RANGE,
    KADP_HAL_DPRX_DRM_EOTF_HDR_LUMINANCE_RANGE,
    KADP_HAL_DPRX_DRM_EOTF_SMPTE_ST_2084,
    KADP_HAL_DPRX_DRM_EOTF_HLG,
    KADP_HAL_DPRX_DRM_EOTF_RESERVED_4,
    KADP_HAL_DPRX_DRM_EOTF_RESERVED_5,
    KADP_HAL_DPRX_DRM_EOTF_RESERVED_6,
    KADP_HAL_DPRX_DRM_EOTF_RESERVED_7,
    KADP_HAL_DPRX_DRM_EOTF_MAX,
}KADP_HAL_DPRX_DRM_EOTF_T;

typedef enum
{
    KADP_HAL_DPRX_DRM_META_TYPE1,
    KADP_HAL_DPRX_DRM_META_RESERVED1,
    KADP_HAL_DPRX_DRM_META_RESERVED2,
    KADP_HAL_DPRX_DRM_META_RESERVED3,
    KADP_HAL_DPRX_DRM_META_RESERVED4,
    KADP_HAL_DPRX_DRM_META_RESERVED5,
    KADP_HAL_DPRX_DRM_META_RESERVED6,
    KADP_HAL_DPRX_DRM_META_RESERVED7,
}KADP_HAL_DPRX_DRM_META_DESC_T;

typedef struct{
    unsigned char  nVersion;
    unsigned char  nLength;
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
}KADP_VFE_DPRX_DRM_INFO_T;

typedef struct
{
    KADP_VFE_DPRX_COLOR_SPACE_E  color_space;
    KADP_VFE_DPRX_COLOR_DEPTH_E  color_depth;
    KADP_VFE_DPRX_COLORIMETRY_E  colorimetry;
    KADP_VFE_DPRX_DYNAMIC_RANGE_E  dynamic_range;
    KADP_VFE_DPRX_CONTENT_TYPE_E  content_type;
}KADP_VFE_DPRX_PIXEL_ENCODING_FORMAT_T;

typedef enum
{
    KADP_VFE_DPRX_PACKET_STATUS_NOT_RECEIVED,   /**< DPRX packet status */
    KADP_VFE_DPRX_PACKET_STATUS_STOPPED,
    KADP_VFE_DPRX_PACKET_STATUS_UPDATED,
    KADP_VFE_DPRX_PACKET_STATUS_MAX
} KADP_HAL_VFE_DPRX_PACKET_STATUS_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_CSC_RGB,      /**< DPRX CSC info */
    KADP_VFE_DPRX_AVI_CSC_YCBCR422,
    KADP_VFE_DPRX_AVI_CSC_YCBCR444,
    KADP_VFE_DPRX_AVI_CSC_YCBCR420
} KADP_HAL_VFE_DPRX_AVI_CSC_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_ACTIVE_INFO_INVALID,      /**< DPRX info */
    KADP_VFE_DPRX_AVI_ACTIVE_INFO_VALID
} KADP_HAL_VFE_DPRX_AVI_ACTIVE_INFO_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_BAR_INFO_INVALID,         /**< DPRX info */
    KADP_VFE_DPRX_AVI_BAR_INFO_VERTICALVALID,
    KADP_VFE_DPRX_AVI_BAR_INFO_HORIZVALID,
    KADP_VFE_DPRX_AVI_BAR_INFO_VERTHORIZVALID
} KADP_HAL_VFE_DPRX_AVI_BAR_INFO_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_SCAN_INFO_NODATA,         /**< DPRX info */
    KADP_VFE_DPRX_AVI_SCAN_INFO_OVERSCANNED,
    KADP_VFE_DPRX_AVI_SCAN_INFO_UNDERSCANNED,
    KADP_VFE_DPRX_AVI_SCAN_INFO_FUTURE
} KADP_HAL_VFE_DPRX_AVI_SCAN_INFO_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_COLORIMETRY_NODATA,      /**< DPRX info */
    KADP_VFE_DPRX_AVI_COLORIMETRY_SMPTE170,
    KADP_VFE_DPRX_AVI_COLORIMETRY_ITU709,
    KADP_VFE_DPRX_AVI_COLORIMETRY_FUTURE
} KADP_HAL_VFE_DPRX_AVI_COLORIMETRY_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_PICTURE_ARC_NODATA,       /**< DPRX info */
    KADP_VFE_DPRX_AVI_PICTURE_ARC_4_3,
    KADP_VFE_DPRX_AVI_PICTURE_ARC_16_9,
    KADP_VFE_DPRX_AVI_PICTURE_ARC_FUTURE
} KADP_HAL_VFE_DPRX_AVI_PICTURE_ARC_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_ACTIVE_FORMAT_ARC_PICTURE,        /**< DPRX info */
    KADP_VFE_DPRX_AVI_ACTIVE_FORMAT_ARC_4_3CENTER,
    KADP_VFE_DPRX_AVI_ACTIVE_FORMAT_ARC_16_9CENTER,
    KADP_VFE_DPRX_AVI_ACTIVE_FORMAT_ARC_14_9CENTER,
    KADP_VFE_DPRX_AVI_ACTIVE_FORMAT_ARC_OTHER
} KADP_HAL_VFE_DPRX_AVI_ACTIVE_FORMAT_ARC_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_SCALING_NOSCALING,        /**< DPRX info */
    KADP_VFE_DPRX_AVI_SCALING_HSCALING,
    KADP_VFE_DPRX_AVI_SCALING_VSCALING,
    KADP_VFE_DPRX_AVI_SCALING_HVSCALING
} KADP_HAL_VFE_DPRX_AVI_SCALING_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_IT_CONTENT_NODATA,        /**< DPRX info */
    KADP_VFE_DPRX_AVI_IT_CONTENT_ITCONTENT
} KADP_HAL_VFE_DPRX_AVI_IT_CONTENT_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_XVYCC601,    /**< DPRX info */
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_XVYCC709,
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_sYCC601,
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_AdobeYCC601,
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_AdobeRGB,
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_ITURBT2020YCCBCCRC,
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_ITURBT2020RGBorYCBCR,
    KADP_VFE_DPRX_AVI_EXT_COLORIMETRY_RESERVED
} KADP_HAL_VFE_DPRX_AVI_EXT_COLORIMETRY_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_RGB_QUANTIZATION_RANGE_DEFAULT,           /**< DPRX info */
    KADP_VFE_DPRX_AVI_RGB_QUANTIZATION_RANGE_LIMITEDRANGE,
    KADP_VFE_DPRX_AVI_RGB_QUANTIZATION_RANGE_FULLRANGE,
    KADP_VFE_DPRX_AVI_RGB_QUANTIZATION_RANGE_RESERVED
} KADP_HAL_VFE_DPRX_AVI_RGB_QUANTIZATION_RANGE_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_YCC_QUANTIZATION_RANGE_LIMITEDRANGE,  /**< DPRX info */
    KADP_VFE_DPRX_AVI_YCC_QUANTIZATION_RANGE_FULLRANGE,
    KADP_VFE_DPRX_AVI_YCC_QUANTIZATION_RANGE_RESERVED
} KADP_HAL_VFE_DPRX_AVI_YCC_QUANTIZATION_RANGE_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_CONTENT_TYPE_GRAPHICS,
    KADP_VFE_DPRX_AVI_CONTENT_TYPE_PHOTO,
    KADP_VFE_DPRX_AVI_CONTENT_TYPE_CINEMA,
    KADP_VFE_DPRX_AVI_CONTENT_TYPE_GAME,
    KADP_VFE_DPRX_AVI_CONTENT_TYPE_MAX
} KADP_HAL_VFE_DPRX_AVI_CONTENT_TYPE_T;

typedef enum
{
    KADP_VFE_DPRX_AVI_ADDITIONAL_COLORIMETRY_DCI_P3_D65,
    KADP_VFE_DPRX_AVI_ADDITIONAL_COLORIMETRY_DCI_P3_THEATER,
    KADP_VFE_DPRX_AVI_ADDITIONAL_COLORIMETRY_RESERVED,
    KADP_VFE_DPRX_AVI_ADDITIONAL_COLORIMETRY_MAX
} KADP_HAL_VFE_DPRX_AVI_ADDITIONAL_COLORIMETRY_T;

typedef struct
{
    unsigned char type;            /**< packet type */
    unsigned char version;         /**< packet version */
    unsigned char length;          /**< packet length */
} KADP_HAL_VFE_DPRX_IN_PACKET_T;

typedef struct
{
    KADP_HAL_VFE_DPRX_AVI_CSC_T ePixelEncoding;                 /**< DPRX info */
    KADP_HAL_VFE_DPRX_AVI_ACTIVE_INFO_T eActiveInfo;        /**< DPRX info */   /* A0 */
    KADP_HAL_VFE_DPRX_AVI_BAR_INFO_T   eBarInfo;                /**< DPRX info */   /* B1B0 */
    KADP_HAL_VFE_DPRX_AVI_SCAN_INFO_T   eScanInfo;           /**< DPRX info */   /* S1S0 */
    KADP_HAL_VFE_DPRX_AVI_COLORIMETRY_T eColorimetry;       /**< DPRX info */   /* C1C0 */
    KADP_HAL_VFE_DPRX_AVI_PICTURE_ARC_T ePictureAspectRatio;    /**< DPRX info */	/* M1M0 */
    KADP_HAL_VFE_DPRX_AVI_ACTIVE_FORMAT_ARC_T eActiveFormatAspectRatio;/**< DPRX info */    /* R3R0 */
    KADP_HAL_VFE_DPRX_AVI_SCALING_T eScaling;                       /**< DPRX info */   /* SC1SC0 */

    unsigned char VideoIdCode;                                                      /**< DPRX info */   /* VICn */
    unsigned char PixelRepeat;                                                       /**< DPRX info */

    KADP_HAL_VFE_DPRX_AVI_IT_CONTENT_T eITContent;              /**< DPRX info */   /*ITC */
    KADP_HAL_VFE_DPRX_AVI_EXT_COLORIMETRY_T eExtendedColorimetry;	    /**< DPRX info */	/* EC2EC1EC0 */
    KADP_HAL_VFE_DPRX_AVI_RGB_QUANTIZATION_RANGE_T eRGBQuantizationRange;   /**< DPRX info */	/* Q1Q0 */
    KADP_HAL_VFE_DPRX_AVI_YCC_QUANTIZATION_RANGE_T eYCCQuantizationRange;   /**< DPRX info */	/* YQ1YQ0 */
    KADP_HAL_VFE_DPRX_AVI_CONTENT_TYPE_T eContentType;                                          /**< DPRX info */   /* Contents Type */

    /* bar info */
    unsigned short TopBarEndLineNumber;                         /**< DPRX info */
    unsigned short BottomBarStartLineNumber;                /**< DPRX info */
    unsigned short LeftBarEndPixelNumber;                       /**< DPRX info */
    unsigned short RightBarEndPixelNumber;                    /**< DPRX info */

    KADP_HAL_VFE_DPRX_AVI_ADDITIONAL_COLORIMETRY_T eAdditionalColorimetry;      /**< DPRX info */ /*ACE3ACE0*/

    unsigned char f14;          /**< DPRX info */  /*F143F140*/

    KADP_HAL_VFE_DPRX_PACKET_STATUS_T packetStatus; /**< DPRX info */
    KADP_HAL_VFE_DPRX_IN_PACKET_T packet;                    /**< DPRX info */
} KADP_VFE_DPRX_AVI_INFO_T;

//-------------------------------------------------------------------------------
// DPRX AUDIO related
//-------------------------------------------------------------------------------


// Controls
enum
{
    DPRX_AUDIO_CTRL_MAX,
};

// Attributes
enum
{
    DPRX_AUDIO_ATTR_AUDIO_STATUS,
    DPRX_AUDIO_ATTR_MAX,
};

#define VFE_DPRX_AUDIO_RO_ATTR_AUDIO_STATUS  _VFE_DPRX_RO_ATTR(DPRX_AUDIO_ATTR_AUDIO_STATUS)  // get audio status

// data types

typedef enum
{
    KADP_VFE_DPRX_AUDIO_CODING_NO_AUDIO = 0,
    KADP_VFE_DPRX_AUDIO_CODING_LPCM,
    KADP_VFE_DPRX_AUDIO_CODING_UNKNOWN,
}KADP_VFE_DPRX_AUDIO_CODING_TYPE;

typedef struct
{
    KADP_VFE_DPRX_AUDIO_CODING_TYPE coding_type;
    unsigned char ch_num;
    unsigned int  sampling_frequency_khz_x100;
}KADP_VFE_DPRX_AUDIO_STATUS_T;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      DPRX IOCTL END                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //__VFE_CMD_ID_DPRX_H__
