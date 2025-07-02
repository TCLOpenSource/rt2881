#ifndef __HDMI_LIB_INFO_PACKET_H_
#define __HDMI_LIB_INFO_PACKET_H_

typedef enum {
    INFOFRAME_READ_SUCCESS = 0,
    FVS_READ_NOT_READY = -0x1,
    FVS_READ_CHECKSUM_ERROR = 0x1,
    DVS_READ_NOT_READY = -0x2,
    DVS_READ_CHECKSUM_ERROR = 0x2,
    HDR10PVS_READ_NOT_READY = -0x4,
    HDR10PVS_READ_CHECKSUM_ERROR = 0x4,
    VSI_READ_NOT_READY = -0x8,
    VSI_READ_CHECKSUM_ERROR = 0x8,
    AVI_READ_NOT_READY = -0x10,
    AVI_READ_CHECKSUM_ERROR = 0x10,
    SPD_READ_NOT_READY = -0x20,
    SPD_READ_CHECKSUM_ERROR = 0x20,
    AUDIO_READ_NOT_READY = -0x40,
    AUDIO_READ_CHECKSUM_ERROR = 0x40,
    DRM_READ_NOT_READY = -0x80,
    DRM_READ_CHECKSUM_ERROR = 0x80,
    RESERVED_READ_NOT_READY = -0x100,
    RESERVED_READ_CHECKSUM_ERROR = 0x100,
} INFOFRAME_READ_RESULT;


// Common
extern void lib_hdmi_read_packet_sram(unsigned char nport, unsigned int start_addr, unsigned char len, unsigned char *pbuf);
extern int lib_hdmi_infoframe_packet_read(unsigned char type_code, unsigned char nport, unsigned int start_addr, unsigned char len, unsigned char *pbuf);


// FVS
extern int lib_hdmi_is_fvs_received(unsigned char nport);
extern int lib_hdmi_clear_fvs_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT lib_hdmi_read_fvs_info_packet(unsigned char port, unsigned char *pkt_buf);
extern unsigned char lib_hdmi_read_fvs_allm_mode(unsigned char nport);
extern unsigned char lib_hdmi_read_fvs_pa_status(unsigned char nport);
extern void lib_hdmi_clear_fvs_pa_status(unsigned char nport);

// DVS
extern int lib_hdmi_is_dvs_received(unsigned char nport);
extern int lib_hdmi_clear_dvs_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT lib_hdmi_read_dvs_info_packet(unsigned char port, unsigned char *pkt_buf);

// HDR10+ VS
extern int lib_hdmi_is_hdr10pvs_received(unsigned char nport);
extern int lib_hdmi_clear_hdr10pvs_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT lib_hdmi_read_hdr10pvs_info_packet(unsigned char port, unsigned char *pkt_buf);

// VS
extern int lib_hdmi_is_vsi_received(unsigned char nport);
extern int lib_hdmi_clear_vsi_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT lib_hdmi_read_vsi_info_packet(unsigned char port, unsigned char *pkt_buf);

// AVI
extern unsigned char lib_hdmi_is_avi_received(unsigned char nport);
extern void lib_hdmi_clear_avi_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT  lib_hdmi_read_avi_info_packet(unsigned char port, unsigned char *pkt_buf, unsigned char *p_bch);

// SPD
extern unsigned char lib_hdmi_is_spd_received(unsigned char nport);
extern void lib_hdmi_clear_spd_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT  lib_hdmi_read_spd_info_packet(unsigned char nport, unsigned char *pkt_buf, unsigned char *p_bch);

// AUD
extern unsigned char lib_hdmi_is_audiosampkt_received(unsigned char nport);
extern void lib_hdmi_clear_audiosampkt_received_status(unsigned char nport);

// HBR AUD
extern unsigned char lib_hdmi_is_hbr_audiostreampkt_received(unsigned char nport);
extern void lib_hdmi_clear_hbr_audiostreampkt_received_status(unsigned char nport);

// Audio
extern unsigned char lib_hdmi_is_audiopkt_received(unsigned char nport);
extern void lib_hdmi_clear_audiopkt_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT lib_hdmi_read_audio_packet(unsigned char port, unsigned char *pkt_buf);

// DRM
extern unsigned char lib_hdmi_is_drm_received(unsigned char nport);
extern void lib_hdmi_clear_drm_received_status(unsigned char nport);
extern INFOFRAME_READ_RESULT lib_hdmi_read_drm_packet(unsigned char port, unsigned char *pkt_buf, unsigned char *p_bch);

// RSV Packets
extern int lib_hdmi_set_rsv_packet_type(unsigned char nport, unsigned char index, unsigned char pkt_type);
extern int lib_hdmi_set_rsv_packet_type_ex(unsigned char nport, unsigned char index, unsigned char pkt_type, unsigned char check_oui, unsigned char oui_1);
extern int lib_hdmi_is_rsv_packet_received(unsigned char nport, unsigned char index);
extern int lib_hdmi_clear_rsv_packet_status(unsigned char nport, unsigned char index);
extern INFOFRAME_READ_RESULT lib_hdmi_read_reserved_packet(unsigned char nport, unsigned char index, unsigned char *pkt_buf, unsigned char len);

#define lib_hdmi_is_rsv_packet0_received(nport)         lib_hdmi_is_rsv_packet_received(nport, 0)
#define lib_hdmi_is_rsv_packet1_received(nport)         lib_hdmi_is_rsv_packet_received(nport, 1)
#define lib_hdmi_is_rsv_packet2_received(nport)         lib_hdmi_is_rsv_packet_received(nport, 2)
#define lib_hdmi_is_rsv_packet3_received(nport)         lib_hdmi_is_rsv_packet_received(nport, 3)

#define lib_hdmi_clear_rsv_packet0_status(nport)        lib_hdmi_clear_rsv_packet_status(nport, 0)
#define lib_hdmi_clear_rsv_packet1_status(nport)        lib_hdmi_clear_rsv_packet_status(nport, 1)
#define lib_hdmi_clear_rsv_packet2_status(nport)        lib_hdmi_clear_rsv_packet_status(nport, 2)
#define lib_hdmi_clear_rsv_packet3_status(nport)        lib_hdmi_clear_rsv_packet_status(nport, 3)

#define lib_hdmi_read_rsv0_packet(nport, buf, len)      lib_hdmi_read_reserved_packet(nport, 0, buf, len)
#define lib_hdmi_read_rsv1_packet(nport, buf, len)      lib_hdmi_read_reserved_packet(nport, 1, buf, len)
#define lib_hdmi_read_rsv2_packet(nport, buf, len)      lib_hdmi_read_reserved_packet(nport, 2, buf, len)
#define lib_hdmi_read_rsv3_packet(nport, buf, len)      lib_hdmi_read_reserved_packet(nport, 3, buf, len)


#endif

