#ifndef __QUICK_SHOW_IPC_H__
#define __QUICK_SHOW_IPC_H__
#include <rbus/stb_reg.h>
#include <rbus/scpu_core_reg.h>

#define QS_STATUS_reg                        (STB_SW_DATA5_reg)
#define QS_STATUS_active_shift               (0)
#define QS_STATUS_active_mask                (0x00000001)
#define QS_STATUS_hdmi_shift                 (1)
#define QS_STATUS_hdmi_mask                  (0x00000002)
#define QS_STATUS_dp_shift                   (2)
#define QS_STATUS_dp_mask                    (0x00000004)
#define QS_STATUS_panel_shift                (3)
#define QS_STATUS_panel_mask                 (0x00000008)
#define QS_STATUS_scaler_shift               (4)
#define QS_STATUS_scaler_mask                (0x00000010)
#define QS_STATUS_i2c_shift                  (5)
#define QS_STATUS_i2c_mask                   (0x00000020)
#define QS_STATUS_pq_shift                   (6)
#define QS_STATUS_pq_mask                    (0x00000040)
#define QS_STATUS_acpu_shift                 (7)
#define QS_STATUS_acpu_mask                  (0x00000080)
#define QS_STATUS_vcpu_shift                 (8)
#define QS_STATUS_vcpu_mask                  (0x00000100)
#define QS_STATUS_amp_shift                  (9)
#define QS_STATUS_amp_mask                   (0x00000200)
#define QS_STATUS_hp_shift                   (10)
#define QS_STATUS_hp_mask                    (0x00000400)
#define QS_STATUS_portnum_shift              (11)
#define QS_STATUS_portnum_mask               (0x00001800)
#define QS_STATUS_typec_shift                (13)
#define QS_STATUS_typec_mask                 (0x00002000)
#define QS_STATUS_volume_shift               (14)
#define QS_STATUS_volume_mask                (0x001FC000)
#define QS_STATUS_vo_shift                   (21)
#define QS_STATUS_vo_mask                    (0x00200000)
#define QS_STATUS_mute_shift                 (22)
#define QS_STATUS_mute_mask                  (0x00400000)
#define QS_STATUS_detect_done_shift          (23)
#define QS_STATUS_detect_done_mask           (0x00800000)
#define QS_STATUS_magic_shift                (24)
#define QS_STATUS_magic_mask                 (0xFF000000)
#define QS_STATUS_magic_valid_data           (0xDD)


#define QS_STATUS_2_reg                      (STB_SW_DATA6_reg)
#define QS_STATUS_2_PIC_MODE_GAME_shift      (0)
#define QS_STATUS_2_PIC_MODE_GAME_mask       (0x00000001)
#define QS_STATUS_2_PIC_MODE_PC_shift        (1)
#define QS_STATUS_2_PIC_MODE_PC_mask         (0x00000006)
#define QS_STATUS_2_FREESYNC_EN_shift        (4)
#define QS_STATUS_2_FREESYNC_EN_mask         (0x00000010)
#define QS_STATUS_2_HP_CHECK_shift           (5)
#define QS_STATUS_2_HP_CHECK_mask            (0x00000020)
#define QS_STATUS_2_AT_shift                 (6)
#define QS_STATUS_2_AT_mask                  (0x00000040)
#define QS_STATUS_2_HDMI_HDCP_sts_shift      (7)
#define QS_STATUS_2_HDMI_HDCP_sts_mask       (0x00000080)
#define QS_STATUS_2_gdma_shift               (8)
#define QS_STATUS_2_gdma_mask                (0x00000100)
#define QS_STATUS_2_stop_qs_source_shift     (9)
#define QS_STATUS_2_stop_qs_source_mask      (0x00000200)
#define QS_STATUS_2_uboot_done_shift         (10)
#define QS_STATUS_2_uboot_done_mask          (0x00000400)
#define QS_STATUS_2_qs_done_shift            (11)
#define QS_STATUS_2_qs_done_mask             (0x00000800)
#define QS_STATUS_2_balance_shift            (12)
#define QS_STATUS_2_balance_mask             (0x0000F000)
#define QS_STATUS_2_balance_lr_shift         (16)
#define QS_STATUS_2_balance_lr_mask          (0x00010000)
#define QS_STATUS_2_ECO_MODE_shift           (17)
#define QS_STATUS_2_ECO_MODE_mask            (0x00020000)
#define QS_STATUS_2_LOW_LATENCY_MODE_shift   (18)
#define QS_STATUS_2_LOW_LATENCY_MODE_mask    (0x000c0000)


#define QS_INFO_ADDR_REG                     (QS_STATUS_2_reg)
#define QS_INFO_ADDR_MASK                   ~(0x100000-1)   // 1M alignment
#define QS_INFO_LEN                          (0x1000)       // 4KB


#define db_per_balance                       (25)   // FIXME: should refer to header of AP

#define HDCP14_KEY_START_ADDR 0x95fff400  //mapping to 0x15fff400
#define HDCP22_KEY_START_ADDR 0x95fff000  //mapping to 0x15fff000


#define RMCA_SHM_ADDR_RECORD_DUMMY         SCPU_CORE_SC_DUMMY10_reg

enum qs_shm_magic {
    QS_SETTING_MAGIC = 0x23792379,
    QS_ENV_MAGIC,
    QS_PQ_SETTING_MAGIC,
    QS_PQ_TABLE_MAGIC,
    QS_DTB_MAGIC,
    QS_PQ_PANEL_MAGIC,
    QS_PQ_SETTING_V2_MAGIC,
    QS_PQ_BIN_MAGIC,
    QS_PQ_ICM_MAGIC,
    QS_PQ_HDR_MAGIC,
    QS_PANEL_INI_MAGIC,
    QS_PANEL_BIN_MAGIC,
    QS_OD_BIN_MAGIC,
    QS_ABOBERGB_BIN_MAGIC,
    QS_BT2020_BIN_MAGIC,
    QS_DCIP3_BIN_MAGIC,
    QS_NATIVE_BIN_MAGIC,
    QS_SRGB_BIN_MAGIC,
    QS_LD_BIN_MAGIC,
    QS_BLPF_BIN_MAGIC,
    QS_NNSR_BIN_MAGIC,
    QS_BOOTLOGO_MAGIC,
    QS_AQ_BIN_MAGIC,
    QS_PANEL_TMP_INI_MAGIC,
    QS_AT_IP_SETTING_MAGIC,
    QS_DAP_BIN_MAGIC,
};


struct qs_info {
     void *setting_ini_addr;
     int setting_ini_len;
     enum qs_shm_magic setting_ini_magic;

     void *env_addr;
     int env_len;
     enum qs_shm_magic env_magic;

     void *pq_setting_addr;
     int pq_setting_len;
     enum qs_shm_magic pq_setting_magic;

     void *pq_table_addr;
     int pq_table_len;
     enum qs_shm_magic pq_table_magic;

     unsigned int platform_model;
     unsigned long long ddr_size;
     unsigned int dtb_addr;

     void *pq_panel_addr;
     int pq_panel_len;
     enum qs_shm_magic pq_panel_magic;

     void *pq_setting_v2_addr;
     int pq_setting_v2_len;
     enum qs_shm_magic pq_setting_v2_magic;

     void *pq_bin_addr;
     int pq_bin_len;
     enum qs_shm_magic pq_bin_magic;

     void *pq_icm_addr;
     int pq_icm_len;
     enum qs_shm_magic pq_icm_magic;

     void *pq_hdr_addr;
     int pq_hdr_len;
     enum qs_shm_magic pq_hdr_magic;

     void *panel_ini_addr;
     int panel_ini_len;
     enum qs_shm_magic panel_ini_magic;

     void *panel_bin_addr;
     int panel_bin_len;
     enum qs_shm_magic panel_bin_magic;

     void *nnsr_bin_addr;
     int nnsr_bin_len;
     enum qs_shm_magic nnsr_bin_magic;

     void *od_bin_addr;
     int od_bin_len;
     enum qs_shm_magic od_bin_magic;

     void *abobergb_bin_addr;
     int abobergb_bin_len;
     enum qs_shm_magic abobergb_bin_magic;

     void *bt2020_bin_addr;
     int bt2020_bin_len;
     enum qs_shm_magic bt2020_bin_magic;

     void *dcip3_bin_addr;
     int dcip3_bin_len;
     enum qs_shm_magic dcip3_bin_magic;

     void *native_bin_addr;
     int native_bin_len;
     enum qs_shm_magic native_bin_magic;

     void *srgb_bin_addr;
     int srgb_bin_len;
     enum qs_shm_magic srgb_bin_magic;

     void *ld_bin_addr;
     int ld_bin_len;
     enum qs_shm_magic ld_bin_magic;

     void *blpf_bin_addr;
     int blpf_bin_len;
     enum qs_shm_magic blpf_bin_magic;

     void *bootlogo_bin_addr;
     int bootlogo_bin_len;
     enum qs_shm_magic bootlogo_bin_magic;

     void *aq_bin_addr;
     int aq_bin_len;
     enum qs_shm_magic aq_bin_magic;

     void *panel_tmp_ini_addr;
     int panel_tmp_ini_len;
     enum qs_shm_magic panel_tmp_ini_magic;

     void *at_ip_setting_addr;
     int at_ip_setting_len;
     enum qs_shm_magic at_ip_setting_magic;

     int boot_logo_show;
     int eco_mode;

     void *dap_bin_addr;
     int dap_bin_len;
     enum qs_shm_magic dap_bin_magic;
};


#endif  /* __QUICK_SHOW_IPC_H__ */
