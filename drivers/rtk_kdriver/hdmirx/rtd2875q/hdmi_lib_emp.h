#ifndef __HDMI_LIB_EMP_H_
#define __HDMI_LIB_EMP_H_


#define EM_CVTEM_INFO_OFST              0
#define EM_VTEM_INFO_OFST               192
#define EM_VSEM_INFO_OFST               224
#define EM_SBTM_INFO_OFST               320

#define EM_CVTEM_INFO_LEN               192
#define EM_VTEM_INFO_LEN                32
#define EM_VSEM_INFO_LEN                32
#define EM_SBTM_INFO_LEN                32

#define EM_HDR_EMP_CNT_OFST             0
#define EM_HDR_EMP_INFO_OFST            4
#define MAX_EM_HDR_INFO_LEN             736

#define NO_HDR_EMP_PKT_CNT_MAX         10
#define NO_VTEM_PKT_CNT_MAX         18
#define NO_CVTEM_PKT_CNT_MAX         10

typedef struct {
    unsigned char   pps[128];
    unsigned short  hfront;
    unsigned short  hsync;
    unsigned short  hback;
    unsigned short  hcative_bytes;
} CVTEM_INFO;

typedef struct {
    unsigned char sync: 1;          // when this flag is set, data will be received only in FAPA region
    unsigned char recognize_tag: 1; // when this flag is set, data_set_tag will be checked
    unsigned char recognize_oui: 1; // when this flag is set, oui will be checked
    unsigned char reserved: 5;
    unsigned char org_id;
    unsigned char data_set_tag[2];
    unsigned char oui[3];
} EMP_PARAM;

////////////////////////////////////////////////////////////////
extern void _dump_cvtem_info(CVTEM_INFO *p_info);
extern int _parse_cvtem_info(unsigned char pkt_buf[EM_CVTEM_INFO_LEN], CVTEM_INFO *p_info);

// HDR
extern int lib_hdm_hdr_emp_sram_sel(unsigned char nport, unsigned char hd21);
extern int lib_hdmi_set_em_hdr_config(unsigned char nport, const EMP_PARAM *p_emp_cfg);
extern int lib_hdmi_is_em_hdr_received(unsigned char nport);
extern int lib_hdmi_clear_em_hdr_received_status(unsigned char nport);
extern int lib_hdmi_read_em_hdr_packet(unsigned char port, unsigned char pkt_buf[MAX_EM_HDR_INFO_LEN]);

// CVTEM : Compressed Video Transport Metadata
extern int lib_hdmi_is_em_cvtem_received(unsigned char nport);
extern int lib_hdmi_clear_em_cvtem_received_status(unsigned char nport);
extern int lib_hdmi_read_em_cvtem_packet(unsigned char port, unsigned char pkt_buf[EM_CVTEM_INFO_LEN]);
extern int lib_hdmi_clear_em_cvtem_sram(unsigned char nport);
extern int _parse_cvtem_info(unsigned char pkt_buf[EM_CVTEM_INFO_LEN], CVTEM_INFO *p_info);
extern void _dump_cvtem_info(CVTEM_INFO *p_info);

// VRR : Video Timing Extended Data Block
extern int lib_hdmi_is_em_vrr_received(unsigned char nport);
extern int lib_hdmi_clear_em_vrr_received_status(unsigned char nport);
extern int lib_hdmi_read_em_vrr_packet(unsigned char port, unsigned char pkt_buf[EM_VTEM_INFO_LEN]);

// VSEM : Vendor Specific Metadata
extern int lib_hdmi_set_em_vsem_type(unsigned char nport, const EMP_PARAM *p_emp_cfg);
extern int lib_hdmi_is_em_vsem_received(unsigned char nport);
extern int lib_hdmi_clear_em_vsem_received_status(unsigned char nport);
extern int lib_hdmi_read_em_vsem_packet(unsigned char port, unsigned char pkt_buf[EM_VSEM_INFO_LEN]);
extern int lib_hdmi_clear_em_vsem_sram(unsigned char nport);

// SBTM
extern int lib_hdmi_is_em_sbtm_received(unsigned char nport);
extern int lib_hdmi_clear_em_sbtm_received_status(unsigned char nport);
extern int lib_hdmi_read_em_sbtm_packet(unsigned char port, unsigned char pkt_buf[EM_SBTM_INFO_LEN]);
extern int lib_hdmi_clear_em_sbtm_sram(unsigned char nport);

//EMP error status
extern void lib_hdmi_emp_enable_all_mode(unsigned char nport, unsigned int enable);


// VTEM
extern int lib_hdmi_clear_em_vtem_sram(unsigned char nport);
extern int lib_hdmi_clear_em_vtem_received_status(unsigned char nport);
extern int lib_hdmi_is_em_vtem_received(unsigned char nport);
extern int lib_hdmi_read_em_vtem_packet(unsigned char nport, unsigned char pkt_buf[EM_VTEM_INFO_LEN]);

#endif

