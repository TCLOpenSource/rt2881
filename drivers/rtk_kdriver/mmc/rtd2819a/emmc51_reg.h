/**************************************************************



***************************************************************/

#ifndef _EMMC51_REG_H_
#define _EMMC51_REG_H_

#ifndef BIT
#define BIT(x)               (1 <<x)
#endif

/* =============================================
 * emmc PLL and Phase relation
 * ============================================= */
//#define RTK_EM_DW_SRST          (0xb8000108)
//#define RTK_EM_DW_CLKEN         (0xb8000118)
//#define RSTN_EM_DW              (0x01UL<<23)
//#define CLKEN_EM_DW             (0x01UL<<23)
//
//#define EMMC_PLL_SET                    (0xb8000700)
//#define EMMC_PLL_CTRL                   (0xb8000704)
//#define EMMC_PLLPHASE_CTRL              (0xb8000708)
//#define EMMC_PLL_PHASE_INTERPOLATION    (0xb800070C)
//#define EMMC_PLL_SSC0                   (0xb8000710)
//#define EMMC_PLL_SSC1                   (0xb8000714)
//#define EMMC_PLL_SSC2                   (0xb8000718)
//#define EMMC_PLL_SSC3                   (0xb800071C)
//#define EMMC_PLL_SSC4                   (0xb8000720)
//#define EMMC_PLL_SSC5                   (0xb8000724)
//#define EMMC_PLL_SSC6                   (0xb8000728)


/* =============================================
 * Rbus Register mapping (0x1801_0800~0x1801_0BFF)
 * ============================================= */

/* ============================================= */
/* EMMC_WRAPPER register (0x1801_0800~0x1801_08FF) */
/* ============================================= */
#define EM51_BASE                   (0xb8010800)

/* ============================================= */
/* Non-security CPU */
/* ============================================= */
#define EM51_SCPU_SEL               (EM51_BASE+0x00UL)      //0x18010800
#define EM51_INT_STATUS             (EM51_BASE+0x04UL)      //0x18010804
#define EM51_INT_MASK               (EM51_BASE+0x08UL)      //0x18010808
#define EM51_INT_DUMMY0             (EM51_BASE+0x0CUL)      //0x1801080C

/* ============================================= */
/* Security CPU (KCPU) */
/* ============================================= */
#define EM51_KCPU_SEL               (EM51_BASE+0x10UL)      //0x18010810
#define EM51_START_PROTECT_ADDR     (EM51_BASE+0x14UL)      //0x18010814
#define EM51_END_PROTECT_ADDR       (EM51_BASE+0x18UL)      //0x18010818
#define EM51_INT_STATUS_KCPU        (EM51_BASE+0x1CUL)      //0x1801081C
#define EM51_INT_MASK_KCPU          (EM51_BASE+0x20UL)      //0x18010820
#define EM51_DUMMY0_KCPU            (EM51_BASE+0x24UL)      //0x18010824
#define EM51_DUMMY1_KCPU            (EM51_BASE+0x28UL)      //0x18010828
#define EM51_DUMMY2_KCPU            (EM51_BASE+0x2CUL)      //0x1801082C

/* =============================================
 * Only KCPU can access when kcpu_status = 1 (1801_0830~1801_08FF)
 *       All CPUs can access when kcpu_status = 0
 * ============================================= */
/* ============================================= */
/* WRAPPER */
/* ============================================= */
#define EM51_WARP_CTRL              (EM51_BASE+0x30UL)      //0x18010830
#define EM51_WARP_STATUS0           (EM51_BASE+0x34UL)      //0x18010834
#define EM51_WARP_STATUS1           (EM51_BASE+0x38UL)      //0x18010838
#define EM51_DEBUG_REG              (EM51_BASE+0x3CUL)      //0x1801083C

/* ============================================= */
/* MBIST */
/* ============================================= */
#define EM51_IP_BIST                (EM51_BASE+0x40UL)      //0x18010840
#define EM51_WARP_BIST              (EM51_BASE+0x44UL)      //0x18010844

/* ============================================= */
/* Self-Test */
/* ============================================= */
#define EM51_SELF_TEST              (EM51_BASE+0x44UL)      //0x1801084C

/* ============================================= */
/* DS tune */
/* ============================================= */
#define EM51_DS_TUNE_CTRL           (EM51_BASE+0x50UL)      //0x18010850

/* ============================================= */
/* EMMC PAD Control */
/* ============================================= */
#define EM51_CLK_CFG_0              (EM51_BASE+0xA0UL)      //0x180108A0
#define EM51_CLK_EXTRACFG_NP4E_0    (EM51_BASE+0xA4UL)      //0x180108A4
#define EM51_CFG_0                  (EM51_BASE+0xA8UL)      //0x180108A8
#define EM51_CFG_1                  (EM51_BASE+0xACUL)      //0x180108AC
#define EM51_EXTRACFG_NP4E_0        (EM51_BASE+0xB0UL)      //0x180108B0
#define EM51_EXTRACFG_NP4E_1        (EM51_BASE+0xB4UL)      //0x180108B4
#define EM51_EXTRACFG_NP4E_2        (EM51_BASE+0xB8UL)      //0x180108B8

/* ============================================= */
/* DS Debug */
/* ============================================= */
#define EM51_DS_RISING_FALLING_CNT  (EM51_BASE+0xBCUL)      //0x180108BC
#define EM51_RSTN_DEBUG_FOR_DS      (EM51_BASE+0xC0UL)      //0x180108C0

/* ============================================= */
/* EMMC PAD2 */
/* ============================================= */
#define EM51_33V_SEL                (EM51_BASE+0xC4UL)      //0x180108C4

/* ============================================= */
/* Debug for MP(INT) */
/* ============================================= */
#define EM51_INT_DEBUG_CTRL         (EM51_BASE+0xC8UL)      //0x180108C8

/* ============================================= */
/* Clkgen */
/* ============================================= */
#define EM51_CLKGEN_CTRL            (EM51_BASE+0xD0UL)      //0x180108D0

/* ============================================= */
/* CClkout Delay */
/* ============================================= */
#define EM51_CCLKOUT_CTRL           (EM51_BASE+0xD4UL)      //0x180108D4
#define EM51_RBUS_BE_CTRL           (EM51_BASE+0xD8UL)      //0x180108D8
#define EM51_DUMMY1                 (EM51_BASE+0xF0UL)      //0x180108F0
#define EM51_DUMMY2                 (EM51_BASE+0xF4UL)      //0x180108F4
#define EM51_DUMMY3                 (EM51_BASE+0xF8UL)      //0x180108F8
#define EM51_DUMMY4                 (EM51_BASE+0xFCUL)      //0x180108FC
#define EM51_DUMMY5                 (EM51_BASE+0xDCUL)      //0x180108DC

/* ============================================= */
/* Tune */
/* ============================================= */
#define EM51_TUNE_CTRL1             (EM51_BASE+0xE0UL)      //0x180108E0
#define EM51_TUNE_CTRL2             (EM51_BASE+0xE4UL)      //0x180108E4
#define EM51_TUNE_CTRL3             (EM51_BASE+0xE8UL)      //0x180108E8
#define EM51_TUNE_CTRL4             (EM51_BASE+0xECUL)      //0x180108EC


/*****************************************************************
 * DWC_Mobile_Storage register address mapping:
 *     0x1801_0900~0x1801_0BFF
 *
 * DWC_mshc_block_rigisters         0x000~0x0ff
 * DWC_mshc_vendor1_block_rigisters 0x100~0x148
 * DWC_mshc_vendor2_block_rigisters 0x180~0x1DF
 *
 *****************************************************************/
#define DWC51_BASE                 (0xb8010900)

/* DWC_mshc_block_rigisters         0x000~0x0ff */
#define DWC51_SDMASA_R                      (DWC51_BASE+0x000UL)     //0x18010900
#define DWC51_BLOCKSIZE_R16                 (DWC51_BASE+0x004UL)     //0x18010904 (2Bytes)
#define DWC51_BLOCKCOUNT_R16                (DWC51_BASE+0x006UL)     //0x18010906 (2Bytes)
#define DWC51_ARGUMENT_R                    (DWC51_BASE+0x008UL)     //0x18010908
#define DWC51_XFER_MODE_R16                 (DWC51_BASE+0x00CUL)     //0x1801090C (2Bytes)
#define DWC51_CMD_R16                       (DWC51_BASE+0x00EUL)     //0x1801090E (2Bytes)
#define DWC51_RESP01_R                      (DWC51_BASE+0x010UL)     //0x18010910
#define DWC51_RESP23_R                      (DWC51_BASE+0x014UL)     //0x18010914
#define DWC51_RESP45_R                      (DWC51_BASE+0x018UL)     //0x18010918
#define DWC51_RESP67_R                      (DWC51_BASE+0x01CUL)     //0x1801091C
#define DWC51_BUF_DATA_R                    (DWC51_BASE+0x020UL)     //0x18010920
#define DWC51_PSTATE_REG_R                  (DWC51_BASE+0x024UL)     //0x18010924
#define DWC51_HOST_CTRL1_R8                 (DWC51_BASE+0x028UL)     //0x18010928 (1Bytes)
#define DWC51_PWR_CTRL_R8                   (DWC51_BASE+0x029UL)     //0x18010929 (1Bytes)
#define DWC51_BGAP_CTRL_R8                  (DWC51_BASE+0x02AUL)     //0x1801092A (1Bytes)
#define DWC51_WUP_CTRL_R8                   (DWC51_BASE+0x02BUL)     //0x1801092B (1Bytes)
#define DWC51_CLK_CTRL_R16                  (DWC51_BASE+0x02CUL)     //0x1801092C (2Bytes)
#define DWC51_TOUT_CTRL_R8                  (DWC51_BASE+0x02EUL)     //0x1801092E (1Bytes)
#define DWC51_SW_RST_R8                     (DWC51_BASE+0x02FUL)     //0x1801092F (1Bytes)
#define DWC51_NORMAL_INT_STAT_R16           (DWC51_BASE+0x030UL)     //0x18010930 (2Bytes)
#define DWC51_ERROR_INT_STAT_R16            (DWC51_BASE+0x032UL)     //0x18010932 (2Bytes)
#define DWC51_NORMAL_INT_STAT_EN_R16        (DWC51_BASE+0x034UL)     //0x18010934 (2Bytes)
#define DWC51_ERROR_INT_STAT_EN_R16         (DWC51_BASE+0x036UL)     //0x18010936 (2Bytes)
#define DWC51_NORMAL_INT_SIGNAL_EN_R16      (DWC51_BASE+0x038UL)     //0x18010938 (2Bytes)
#define DWC51_ERROR_INT_SIGNAL_EN_R16       (DWC51_BASE+0x03AUL)     //0x1801093A (2Bytes)
#define DWC51_TOTAL_INT_STAT_R              (DWC51_NORMAL_INT_STAT_R16)         //0x18010930
#define DWC51_TOTAL_INT_STAT_EN_R           (DWC51_NORMAL_INT_STAT_EN_R16)      //0x18010934
#define DWC51_TOTAL_INT_SIGNAL_EN_R         (DWC51_NORMAL_INT_SIGNAL_EN_R16)    //0x18010938
#define DWC51_AUTO_CMD_STAT_R16             (DWC51_BASE+0x03CUL)     //0x1801093C (2Bytes)
#define DWC51_HOST_CTRL2_R16                (DWC51_BASE+0x03EUL)     //0x1801093E (2Bytes)
#define DWC51_CAPABILITIES1_R               (DWC51_BASE+0x040UL)     //0x18010940
#define DWC51_CAPABILITIES2_R               (DWC51_BASE+0x044UL)     //0x18010944
#define DWC51_CURR_CAPABILITIES1_R          (DWC51_BASE+0x048UL)     //0x18010948
#define DWC51_CURR_CAPABILITIES2_R          (DWC51_BASE+0x04CUL)     //0x1801094C
#define DWC51_FORCE_AUTO_CMD_STAT_R16       (DWC51_BASE+0x050UL)     //0x18010950 (2Bytes)
#define DWC51_FORCE_ERROR_INT_STAT_R16      (DWC51_BASE+0x052UL)     //0x18010952 (2Bytes)
#define DWC51_ADMA_ERR_STAT_R8               (DWC51_BASE+0x054UL)     //0x18010954 (1Bytes)
#define DWC51_ADMA_SA_LOW_R                 (DWC51_BASE+0x058UL)     //0x18010958
#define DWC51_ADMA_SA_HIGH_R                (DWC51_BASE+0x05CUL)     //0x1801095C
#define DWC51_PRESET_INIT_R16               (DWC51_BASE+0x060UL)     //0x18010960 (2Bytes)
#define DWC51_PRESET_DS_R16                 (DWC51_BASE+0x062UL)     //0x18010962 (2Bytes)
#define DWC51_PRESET_HS_R16                 (DWC51_BASE+0x064UL)     //0x18010964 (2Bytes)
#define DWC51_PRESET_SDR12_R16              (DWC51_BASE+0x066UL)     //0x18010966 (2Bytes)
#define DWC51_PRESET_SDR25_R16              (DWC51_BASE+0x068UL)     //0x18010968 (2Bytes)
#define DWC51_PRESET_SDR50_R16              (DWC51_BASE+0x06AUL)     //0x1801096A (2Bytes)
#define DWC51_PRESET_SDR104_R16             (DWC51_BASE+0x06CUL)     //0x1801096C (2Bytes)
#define DWC51_PRESET_DDR50_R16              (DWC51_BASE+0x06EUL)     //0x1801096E (2Bytes)
#define DWC51_PRESET_UHS2_R                 (DWC51_BASE+0x074UL)     //0x18010974
#define DWC51_ADMA_ID_LOW_R                 (DWC51_BASE+0x078UL)     //0x18010978
#define DWC51_ADMA_ID_HIGH_R                (DWC51_BASE+0x07CUL)     //0x1801097C
#define DWC51_UHS_II_BLOCK_SIZE_R           (DWC51_BASE+0x080UL)     //0x18010980 (UHS-II mode only)
#define DWC51_UHS_II_BLOCK_COUNT_R          (DWC51_BASE+0x084UL)     //0x18010984 (UHS-II mode only)
#define DWC51_UHS_II_COMMAND_PKT_0_3_R      (DWC51_BASE+0x088UL)     //0x18010988 (UHS-II mode only)
#define DWC51_UHS_II_COMMAND_PKT_4_7_R      (DWC51_BASE+0x08CUL)     //0x1801098C (UHS-II mode only)
#define DWC51_UHS_II_COMMAND_PKT_8_11_R     (DWC51_BASE+0x090UL)     //0x18010990 (UHS-II mode only)
#define DWC51_UHS_II_COMMAND_PKT_12_15_R    (DWC51_BASE+0x094UL)     //0x18010994 (UHS-II mode only)
#define DWC51_UHS_II_COMMAND_PKT_16_19_R    (DWC51_BASE+0x098UL)     //0x18010998 (UHS-II mode only)
#define DWC51_UHS_II_XFER_MODE_R16          (DWC51_BASE+0x09CUL)     //0x1801099C (UHS-II mode only) (2Bytes)
#define DWC51_UHS_II_CMD_R16                (DWC51_BASE+0x09EUL)     //0x1801099E (UHS-II mode only) (2Bytes)
#define DWC51_UHS_II_RESP_0_3_R             (DWC51_BASE+0x0A0UL)     //0x180109A0 (UHS-II mode only)
#define DWC51_UHS_II_RESP_4_7_R             (DWC51_BASE+0x0A4UL)     //0x180109A4 (UHS-II mode only)
#define DWC51_UHS_II_RESP_8_11_R            (DWC51_BASE+0x0A8UL)     //0x180109A8 (UHS-II mode only)
#define DWC51_UHS_II_RESP_12_15_R           (DWC51_BASE+0x0ACUL)     //0x180109AC (UHS-II mode only)
#define DWC51_UHS_II_RESP_16_19_R           (DWC51_BASE+0x0B0UL)     //0x180109B0 (UHS-II mode only)
#define DWC51_UHS_II_MSG_SEL_R              (DWC51_BASE+0x0B4UL)     //0x180109B4 (UHS-II mode only)
#define DWC51_UHS_II_MSG_R                  (DWC51_BASE+0x0B8UL)     //0x180109B8 (UHS-II mode only)
#define DWC51_UHS_II_DEV_INTR_STATUS_R16    (DWC51_BASE+0x0BCUL)     //0x180109BC (UHS-II mode only) (2Bytes)
#define DWC51_UHS_II_DEV_SEL_R16            (DWC51_BASE+0x0BEUL)     //0x180109BE (UHS-II mode only) (2Bytes)
#define DWC51_UHS_II_DEV_INR_CODE_R         (DWC51_BASE+0x0BFUL)     //0x180109BF (UHS-II mode only)
#define DWC51_UHS_II_SOFT_RESET_R16         (DWC51_BASE+0x0C0UL)     //0x180109C0 (UHS-II mode only) (2Bytes)
#define DWC51_UHS_II_TIMER_CNTRL_R16        (DWC51_BASE+0x0C2UL)     //0x180109C2 (UHS-II mode only) (2Bytes)
#define DWC51_UHS_II_ERR_INTR_STATUS_R      (DWC51_BASE+0x0C4UL)     //0x180109C4 (UHS-II mode only)
#define DWC51_UHS_II_ERR_INTR_STATUS_EN_R   (DWC51_BASE+0x0C8UL)     //0x180109C8 (UHS-II mode only)
#define DWC51_UHS_II_ERR_INTR_SIGNAL_EN_R   (DWC51_BASE+0x0CCUL)     //0x180109CC (UHS-II mode only)
#define DWC51_P_UHS_II_SETTINGS_R16         (DWC51_BASE+0x0E0UL)     //0x180109E0 (UHS-II mode only) (2Bytes)
#define DWC51_P_UHS_II_HOST_CAPAB_R16       (DWC51_BASE+0x0E2UL)     //0x180109E2 (UHS-II mode only) (2Bytes)
#define DWC51_P_UHS_II_TEST_R16             (DWC51_BASE+0x0E4UL)     //0x180109E4 (UHS-II mode only) (2Bytes)
#define DWC51_P_EMBEDDED_CNTRL_R16          (DWC51_BASE+0x0E6UL)     //0x180109E6 (2Bytes)
#define DWC51_P_VENDOR_SPECIFIC_AREA_R16    (DWC51_BASE+0x0E8UL)     //0x180109E8 (2Bytes)
#define DWC51_P_VENDOR2_SPECIFIC_AREA_R16   (DWC51_BASE+0x0EAUL)     //0x180109EA (2Bytes)
#define DWC51_SLOT_INTR_STATUS_R16          (DWC51_BASE+0x0FCUL)     //0x180109FC (2Bytes)
#define DWC51_HOST_CNTRL_VERS_R16           (DWC51_BASE+0x0FEUL)     //0x180109FE (2Bytes)


/* DWC_mshc_vendor1_block_rigisters 0x100~0x148 */
#define DWC51_MSHC_VER_ID_R                 (DWC51_BASE+0x100+0x00UL)   //0x18010A00
#define DWC51_MSHC_VER_TYPE_R               (DWC51_BASE+0x100+0x04UL)   //0x18010A04
#define DWC51_MSHC_CTRL_R                   (DWC51_BASE+0x100+0x08UL)   //0x18010A08
#define DWC51_MBIU_CTRL_R                   (DWC51_BASE+0x100+0x10UL)   //0x18010A10
#define DWC51_EMMC_CTRL_R16                 (DWC51_BASE+0x100+0x2CUL)   //0x18010A2C (2Bytes)
#define DWC51_BOOT_CTRL_R16                 (DWC51_BASE+0x100+0x2EUL)   //0x18010A2E (2Bytes)
#define DWC51_GP_IN_R                       (DWC51_BASE+0x100+0x30UL)   //0x18010A30
#define DWC51_GP_OUT_R                      (DWC51_BASE+0x100+0x34UL)   //0x18010A34
#define DWC51_AT_CTRL_R                     (DWC51_BASE+0x100+0x40UL)   //0x18010A40
#define DWC51_AT_STAT_R                     (DWC51_BASE+0x100+0x44UL)   //0x18010A44

/* DWC_mshc_vendor2_block_rigisters 0x180~0x1DF */
#define DWC51_CQ_BASE                       (DWC51_BASE+0x180+0x00UL)   //0x18010A80
#define DWC51_CQVER                         (DWC51_BASE+0x180+0x00UL)   //0x18010A80 CQHCI_VER
#define DWC51_CQCAP                         (DWC51_BASE+0x180+0x04UL)   //0x18010A84 CQHCI_CAP
#define DWC51_CQCFG                         (DWC51_BASE+0x180+0x08UL)   //0x18010A88 CQHCI_CFG
#define DWC51_CQCTL                         (DWC51_BASE+0x180+0x0cUL)   //0x18010A8C CQHCI_CTL
#define DWC51_CQIS                          (DWC51_BASE+0x180+0x10UL)   //0x18010A90 CQHCI_IS
#define DWC51_CQISE                         (DWC51_BASE+0x180+0x14UL)   //0x18010A94 CQHCI_ISTE
#define DWC51_CQISGE                        (DWC51_BASE+0x180+0x18UL)   //0x18010A98 CQHCI_ISGE
#define DWC51_CQIC                          (DWC51_BASE+0x180+0x1cUL)   //0x18010A9C CQHCI_IC
#define DWC51_CQTDLBA                       (DWC51_BASE+0x180+0x20UL)   //0x18010AA0 CQHCI_TDLBA
#define DWC51_CQTDLBAU                      (DWC51_BASE+0x180+0x24UL)   //0x18010AA4 CQHCI_TDLBAU
#define DWC51_CQTDBR                        (DWC51_BASE+0x180+0x28UL)   //0x18010AA8 CQHCI_TDBR
#define DWC51_CQTCN                         (DWC51_BASE+0x180+0x2cUL)   //0x18010AAC CQHCI_TCN
#define DWC51_CQDQS                         (DWC51_BASE+0x180+0x30UL)   //0x18010AB0 CQHCI_DQS
#define DWC51_CQDPT                         (DWC51_BASE+0x180+0x34UL)   //0x18010AB4 CQHCI_DPT
#define DWC51_CQTCLR                        (DWC51_BASE+0x180+0x38UL)   //0x18010AB8 CQHCI_TCLR
//CQHCI_TDPE ??
#define DWC51_CQSSC1                        (DWC51_BASE+0x180+0x40UL)   //0x18010AC0 CQHCI_SSC1
#define DWC51_CQSSC2                        (DWC51_BASE+0x180+0x44UL)   //0x18010AC4 CQHCI_SSC2
#define DWC51_CQCRDCT                       (DWC51_BASE+0x180+0x48UL)   //0x18010AC8 CQHCI_CRDCT
#define DWC51_CQRMEM                        (DWC51_BASE+0x180+0x50UL)   //0x18010AD0 CQHCI_RMEM
#define DWC51_CQTERRI                       (DWC51_BASE+0x180+0x54UL)   //0x18010AD4 CQHCI_TERRI
#define DWC51_CQCRI                         (DWC51_BASE+0x180+0x58UL)   //0x18010AD8 CQHCI_CRI
#define DWC51_CQCRA                         (DWC51_BASE+0x180+0x5cUL)   //0x18010ADC CQHCI_CRA

#define DWC51_BASE_END                      (DWC51_BASE+0x1DFUL)        //0x18010ADF

/* ============================================= */
/* ======      Register Bit define       ======  */
/* ============================================= */
/* ============================================= */
/* Non-security CPU */
/* ============================================= */
//#define EM51_SCPU_SEL               (EM51_BASE+0x00UL)     //0x18010800
#define SCPU_STOP                       BIT(2)
#define SCPU_STATUS                     BIT(1)
#define SCPU_SEL                        BIT(0)

//#define EM51_INT_STATUS             (EM51_BASE+0x04UL)     //0x18010804
#define SCPU_HIT_PROTECT_REGION         BIT(4)
#define SCPU_INTERRUPT                  BIT(3)
#define STOP_BY_KCPU_FORCE_SCPU         BIT(0)

//#define EM51_INT_MASK               (EM51_BASE+0x08UL)     //0x18010808
#define STOP_BY_KCPU_FORCE_MASK_SCPU    BIT(0)

//#define EM51_INT_DUMMY0             (EM51_BASE+0x0CUL)     //0x1801080C

/* ============================================= */
/* Security CPU (KCPU) */
/* ============================================= */
//#define EM51_KCPU_SEL               (EM51_BASE+0x10UL)     //0x18010810
#define KCPU_FORCE_EN                   BIT(3)
#define PROTECT_ADDR_EN                 BIT(2)
#define KCPU_STATUS                     BIT(1)
#define KCPU_SEL                        BIT(0)

//#define EM51_START_PROTECT_ADDR     (EM51_BASE+0x14UL)     //0x18010814
//#define EM51_END_PROTECT_ADDR       (EM51_BASE+0x18UL)     //0x18010818
//#define EM51_INT_STATUS_KCPU        (EM51_BASE+0x1CUL)     //0x1801081C
#define KCPU_INTERRUPT                  BIT(3)
#define PROTECT_HIT_KCPU                BIT(0)

//#define EM51_INT_MASK_KCPU          (EM51_BASE+0x20UL)     //0x18010820
#define PROTECT_HIT_MAKS_KCPU           BIT(0)

//#define EM51_DUMMY0_KCPU            (EM51_BASE+0x24UL)     //0x18010824
//#define EM51_DUMMY1_KCPU            (EM51_BASE+0x28UL)     //0x18010828
//#define EM51_DUMMY2_KCPU            (EM51_BASE+0x2CUL)     //0x1801082C

/* ============================================= */
/* WRAPPER */
/* ============================================= */
//#define EM51_WARP_CTRL              (EM51_BASE+0x30UL)     //0x18010830
#define WRAP_SRAM_RD_OPTION             BIT(31)
#define M_NBIGENDIAN                    BIT(10)
#define DEBUG_MUX_SHT                   (1)     //6:1
#define DEBUG_MUX_MASK                  (0x3FUL << DEBUG_MUX_SHT)
#define DEBUG_MUX_GET(x)                ((x & DEBUG_MUX_MASK) >> DEBUG_MUX_SHT)
#define DEBUG_MUX_SET(x,y)              (x = ((x & ~DEBUG_MUX_MASK) | (y << DEBUG_MUX_SHT)))
#define DEBUG_CTRL                      BIT(0)

//#define EM51_WARP_STATUS0           (EM51_BASE+0x34UL)     //0x18010834
#define DDR_BIT_7_4_TIE                 BIT(3)
#define RBUS_ERR                        BIT(2)
#define DBUS_DMA_BUSY                   BIT(1)
#define DBUS_WRITE_FLAG                 BIT(0)

//#define EM51_WARP_STATUS1           (EM51_BASE+0x38UL)     //0x18010838
#define ICTRL_STATE_SHT                 (16)    //18:16
#define ICTRL_STATE_MASK                (0x7UL << ICTRL_STATE_SHT)
#define ICTRL_STATE_GET(x)              ((x & ICTRL_STATE_MASK) >> ICTRL_STATE_SHT)
#define ICTRL_STATE_SET(x,y)            (x = ((x & ~ICTRL_STATE_MASK) | (y << ICTRL_STATE_SHT)))
#define TOP_SM_SHT                      (0)     //14:0
#define TOP_SM_MASK                     (0x7FFFUL << TOP_SM_SHT)
#define TOP_SM_GET(x)                   ((x & TOP_SM_MASK) >> TOP_SM_SHT)
#define TOP_SM_SET(x,y)                 (x = ((x & ~TOP_SM_MASK) | (y << TOP_SM_SHT)))

//#define EM51_DEBUG_REG              (EM51_BASE+0x3CUL)     //0x1801083C

/* ============================================= */
/* MBIST */
/* ============================================= */
//#define EM51_IP_BIST                (EM51_BASE+0x40UL)     //0x18010840
//#define EM51_WARP_BIST              (EM51_BASE+0x44UL)     //0x18010844

/* ============================================= */
/* Self-Test */
/* ============================================= */
//#define EM51_SELF_TEST              (EM51_BASE+0x44UL)     //0x1801084C

/* ============================================= */
/* DS tune */
/* ============================================= */
//#define EM51_DS_TUNE_CTRL           (EM51_BASE+0x50UL)     //0x18010850

/* ============================================= */
/* EMMC PAD Control */
/* ============================================= */
//#define EM51_CLK_CFG_0              (EM51_BASE+0xA0UL)    //0x180108A0
#define EMMC_RST_N_PU                   BIT(31)
#define EMMC_RST_N_PD                   BIT(30)
#define EMMC_RST_N_SR                   BIT(29)
#define EMMC_RST_N_SMT                  BIT(28)
#define EMMC_CLK_PU                     BIT(27)
#define EMMC_CLK_PD                     BIT(26)
#define EMMC_CLK_SR                     BIT(25)
#define EMMC_CLK_SMT                    BIT(24)
#define EMMC_CMD_PU                     BIT(23)
#define EMMC_CMD_PD                     BIT(22)
#define EMMC_CMD_SR                     BIT(21)
#define EMMC_CMD_SMT                    BIT(20)

//#define EM51_CLK_EXTRACFG_NP4E_0    (EM51_BASE+0xA4UL)    //0x180108A4
#define EMMC_RST_N_DS_SHT               (26)    //31:26
#define EMMC_RST_N_DS_MASK              (0x3FUL << EMMC_RST_N_DS_SHT)
#define EMMC_RST_N_DS_GET(x)            ((x & EMMC_RST_N_DS_MASK) >> EMMC_RST_N_DS_SHT)
#define EMMC_RST_N_DS_SET(x,y)          (x = ((x & ~EMMC_RST_N_DS_MASK) | (y << EMMC_RST_N_DS_SHT)))
#define EMMC_RST_N_DC_SHT               (23)    //25:23
#define EMMC_RST_N_DC_MASK              (0x7UL << EMMC_RST_N_DC_SHT)
#define EMMC_RST_N_DC_GET(x)            ((x & EMMC_RST_N_DC_MASK) >> EMMC_RST_N_DC_SHT)
#define EMMC_RST_N_DC_SET(x,y)          (x = ((x & ~EMMC_RST_N_DC_MASK) | (y << EMMC_RST_N_DC_SHT)))
#define EMMC_CLK_DS_SHT                 (17)    //22:17
#define EMMC_CLK_DS_MASK                (0x3FUL << EMMC_CLK_DS_SHT)
#define EMMC_CLK_DS_GET(x)              ((x & EMMC_CLK_DS_MASK) >> EMMC_CLK_DS_SHT)
#define EMMC_CLK_DS_SET(x,y)            (x = ((x & ~EMMC_CLK_DS_MASK) | (y << EMMC_CLK_DS_SHT)))
#define EMMC_CLK_DC_SHT                 (14)    //16:14
#define EMMC_CLK_DC_MASK                (0x7UL << EMMC_CLK_DC_SHT
#define EMMC_CLK_DC_GET(x)              ((x & EMMC_CLK_DC_MASK) >> EMMC_CLK_DC_SHT)
#define EMMC_CLK_DC_SET(x,y)            (x = ((x & ~EMMC_CLK_DC_MASK) | (y << EMMC_CLK_DC_SHT)))
#define EMMC_CMD_DS_SHT                 (8)     //13:8
#define EMMC_CMD_DS_MASK                (0x3FUL << EMMC_CMD_DS_SHT)
#define EMMC_CMD_DS_GET(x)              ((x & EMMC_CMD_DS_MASK) >> EMMC_CMD_DS_SHT)
#define EMMC_CMD_DS_SET(x,y)            (x = ((x & ~EMMC_CMD_DS_MASK) | (y << EMMC_CMD_DS_SHT)))
#define EMMC_CMD_DC_SHT                 (5)     //7:5
#define EMMC_CMD_DC_MASK                (0x7UL << EMMC_CMD_DC_SHT)
#define EMMC_CMD_DC_GET(x)              ((x & EMMC_CMD_DC_MASK) >> EMMC_CMD_DC_SHT)
#define EMMC_CMD_DC_SET(x,y)            (x = ((x & ~EMMC_CMD_DC_MASK) | (y << EMMC_CMD_DC_SHT)))

//#define EM51_CFG_0                  (EM51_BASE+0xA8UL)    //0x180108A8
#define EMMC_DS_PU                      BIT(31)
#define EMMC_DS_PD                      BIT(30)
#define EMMC_DS_SR                      BIT(29)
#define EMMC_DS_SMT                     BIT(28)
#define EMMC_D5_PU                      BIT(27)
#define EMMC_D5_PD                      BIT(26)
#define EMMC_D5_SR                      BIT(25)
#define EMMC_D5_SMT                     BIT(24)
#define EMMC_D3_PU                      BIT(23)
#define EMMC_D3_PD                      BIT(22)
#define EMMC_D3_SR                      BIT(21)
#define EMMC_D3_SMT                     BIT(20)
#define EMMC_D4_PU                      BIT(19)
#define EMMC_D4_PD                      BIT(18)
#define EMMC_D4_SR                      BIT(17)
#define EMMC_D4_SMT                     BIT(16)
#define EMMC_D0_PU                      BIT(15)
#define EMMC_D0_PD                      BIT(14)
#define EMMC_D0_SR                      BIT(13)
#define EMMC_D0_SMT                     BIT(12)
#define EMMC_D1_PU                      BIT(11)
#define EMMC_D1_PD                      BIT(10)
#define EMMC_D1_SR                      BIT(9)
#define EMMC_D1_SMT                     BIT(8)
#define EMMC_D2_PU                      BIT(7)
#define EMMC_D2_PD                      BIT(6)
#define EMMC_D2_SR                      BIT(5)
#define EMMC_D2_SMT                     BIT(4)
#define EMMC_D7_PU                      BIT(3)
#define EMMC_D7_PD                      BIT(2)
#define EMMC_D7_SR                      BIT(1)
#define EMMC_D7_SMT                     BIT(0)

//#define EM51_CFG_1                  (EM51_BASE+0xACUL)    //0x180108AC
#define EMMC_D6_PU                      BIT(31)
#define EMMC_D6_PD                      BIT(30)
#define EMMC_D6_SR                      BIT(29)
#define EMMC_D6_SMT                     BIT(28)

//#define EM51_EXTRACFG_NP4E_0        (EM51_BASE+0xB0UL)    //0x180108B0
#define EMMC_DS_DS_SHT                  (26)    //31:26
#define EMMC_DS_DS_MASK                 (0x3FUL << EMMC_DS_DS_SHT)
#define EMMC_DS_DS_GET(x)               ((x & EMMC_DS_DS_MASK) >> EMMC_DS_DS_SHT)
#define EMMC_DS_DS_SET(x,y)             (x = ((x & ~EMMC_DS_DS_MASK) | (y << EMMC_DS_DS_SHT)))
#define EMMC_DS_DC_SHT                  (23)   //25:23
#define EMMC_DS_DC_MASK                 (0x7UL << EMMC_DS_DC_SHT)
#define EMMC_DS_DC_GET(x)               ((x & EMMC_DS_DC_MASK) >> EMMC_DS_DC_SHT)
#define EMMC_DS_DC_SET(x,y)             (x = ((x & ~EMMC_DS_DC_MASK) | (y << EMMC_DS_DC_SHT)))
#define EMMC_D5_DS_SHT                  (17)    //22:17
#define EMMC_D5_DS_MASK                 (0x3FUL << EMMC_D5_DS_SHT)
#define EMMC_D5_DS_GET(x)               ((x & EMMC_D5_DS_MASK) >> EMMC_D5_DS_SHT)
#define EMMC_D5_DS_SET(x,y)             (x = ((x & ~EMMC_D5_DS_MASK) | (y << EMMC_D5_DS_SHT)))
#define EMMC_D5_DC_SHT                  (14)   //16:14
#define EMMC_D5_DC_MASK                 (0x7UL << EMMC_D5_DC_SHT)
#define EMMC_D5_DC_GET(x)               ((x & EMMC_D5_DC_MASK) >> EMMC_D5_DC_SHT)
#define EMMC_D5_DC_SET(x,y)             (x = ((x & ~EMMC_D5_DC_MASK) | (y << EMMC_D5_DC_SHT)))
#define EMMC_D3_DS_SHT                  (8)   //13:8
#define EMMC_D3_DS_MASK                 (0x3FUL << EMMC_D3_DS_SHT)
#define EMMC_D3_DS_GET(x)               ((x & EMMC_D3_DS_MASK) >> EMMC_D3_DS_SHT)
#define EMMC_D3_DS_SET(x,y)             (x = ((x & ~EMMC_D3_DS_MASK) | (y << EMMC_D3_DS_SHT)))
#define EMMC_D3_DC_SHT                  (5)   //7:5
#define EMMC_D3_DC_MASK                 (0x7UL << EMMC_D3_DC_SHT)
#define EMMC_D3_DC_GET(x)               ((x & EMMC_D3_DC_MASK) >> EMMC_D3_DC_SHT)
#define EMMC_D3_DC_SET(x,y)             (x = ((x & ~EMMC_D3_DC_MASK) | (y << EMMC_D3_DC_SHT)))

//#define EM51_EXTRACFG_NP4E_1        (EM51_BASE+0xB4UL)    //0x180108B4
#define EMMC_D4_DS_SHT                  (26)    //31:26
#define EMMC_D4_DS_MASK                 (0x3FUL << EMMC_D4_DS_SHT)
#define EMMC_D4_DS_GET(x)               ((x & EMMC_D4_DS_MASK) >> EMMC_D4_DS_SHT)
#define EMMC_D4_DS_SET(x,y)             (x = ((x & ~EMMC_D4_DS_MASK) | (y << EMMC_D4_DS_SHT)))
#define EMMC_D4_DC_SHT                  (23)    //25:23
#define EMMC_D4_DC_MASK                 (0x7UL << EMMC_D4_DC_SHT)
#define EMMC_D4_DC_GET(x)               ((x & EMMC_D4_DC_MASK) >> EMMC_D4_DC_SHT)
#define EMMC_D4_DC_SET(x,y)             (x = ((x & ~EMMC_D4_DC_MASK) | (y << EMMC_D4_DC_SHT)))
#define EMMC_D0_DS_SHT                  (17)   //22:17
#define EMMC_D0_DS_MASK                 (0x3FUL << EMMC_D0_DS_SHT)
#define EMMC_D0_DS_GET(x)               ((x & EMMC_D0_DS_MASK) >> EMMC_D0_DS_SHT)
#define EMMC_D0_DS_SET(x,y)             (x = ((x & ~EMMC_D0_DS_MASK) | (y << EMMC_D0_DS_SHT)))
#define EMMC_D0_DC_SHT                  (14)   //16:14
#define EMMC_D0_DC_MASK                 (0x7UL << EMMC_D0_DC_SHT)
#define EMMC_D0_DC_GET(x)               ((x & EMMC_D0_DC_MASK) >> EMMC_D0_DC_SHT)
#define EMMC_D0_DC_SET(x,y)             (x = ((x & ~EMMC_D0_DC_MASK) | (y << EMMC_D0_DC_SHT)))
#define EMMC_D1_DS_SHT                  (8)    //13:8
#define EMMC_D1_DS_MASK                 (0x3FUL << EMMC_D1_DS_SHT)
#define EMMC_D1_DS_GET(x)               ((x & EMMC_D1_DS_MASK) >> EMMC_D1_DS_SHT)
#define EMMC_D1_DS_SET(x,y)             (x = ((x & ~EMMC_D1_DS_MASK) | (y << EMMC_D1_DS_SHT)))
#define EMMC_D1_DC_SHT                  (5)    //7:5
#define EMMC_D1_DC_MASK                 (0x7UL << EMMC_D1_DC_SHT)
#define EMMC_D1_DC_GET(x)               ((x & EMMC_D1_DC_MASK) >> EMMC_D1_DC_SHT)
#define EMMC_D1_DC_SET(x,y)             (x = ((x & ~EMMC_D1_DC_MASK) | (y << EMMC_D1_DC_SHT)))

//#define EM51_EXTRACFG_NP4E_2        (EM51_BASE+0xB8UL)    //0x180108B8
#define EMMC_D2_DS_SHT                  (26)   //31:26
#define EMMC_D2_DS_MASK                 (0x3FUL << EMMC_D2_DS_SHT)
#define EMMC_D2_DS_GET(x)               ((x & EMMC_D2_DS_MASK) >> EMMC_D2_DS_SHT)
#define EMMC_D2_DS_SET(x,y)             (x = ((x & ~EMMC_D2_DS_MASK) | (y << EMMC_D2_DS_SHT)))
#define EMMC_D2_DC_SHT                  (23)   //25:23
#define EMMC_D2_DC_MASK                 (0x7FUL << EMMC_D2_DC_SHT)
#define EMMC_D2_DC_GET(x)               ((x & EMMC_D2_DC_MASK) >> EMMC_D2_DC_SHT)
#define EMMC_D2_DC_SET(x,y)             (x = ((x & ~EMMC_D2_DC_MASK) | (y << EMMC_D2_DC_SHT)))
#define EMMC_D7_DS_SHT                  (17)   //22:17
#define EMMC_D7_DS_MASK                 (0x3FUL << EMMC_D7_DS_SHT)
#define EMMC_D7_DS_GET(x)               ((x & EMMC_D7_DS_MASK) >> EMMC_D7_DS_SHT)
#define EMMC_D7_DS_SET(x,y)             (x = ((x & ~EMMC_D7_DS_MASK) | (y << EMMC_D7_DS_SHT)))
#define EMMC_D7_DC_SHT                  (14)   //16:14
#define EMMC_D7_DC_MASK                 (0x7UL << EMMC_D7_DC_SHT)
#define EMMC_D7_DC_GET(x)               ((x & EMMC_D7_DC_MASK) >> EMMC_D7_DC_SHT)
#define EMMC_D7_DC_SET(x,y)             (x = ((x & ~EMMC_D7_DC_MASK) | (y << EMMC_D7_DC_SHT)))
#define EMMC_D6_DS_SHT                  (8)    //13:8
#define EMMC_D6_DS_MASK                 (0x3FUL << EMMC_D6_DS_SHT)
#define EMMC_D6_DS_GET(x)               ((x & EMMC_D6_DS_MASK) >> EMMC_D6_DS_SHT)
#define EMMC_D6_DS_SET(x,y)             (x = ((x & ~EMMC_D6_DS_MASK) | (y << EMMC_D6_DS_SHT)))
#define EMMC_D6_DC_SHT                  (5)    //7:5
#define EMMC_D6_DC_MASK                 (0x7UL << EMMC_D6_DC_SHT)
#define EMMC_D6_DC_GET(x)               ((x & EMMC_D6_DC_MASK) >> EMMC_D6_DC_SHT)
#define EMMC_D6_DC_SET(x,y)             (x = ((x & ~EMMC_D6_DC_MASK) | (y << EMMC_D6_DC_SHT)))

/* ============================================= */
/* DS Debug */
/* ============================================= */
//#define EM51_DS_RISING_FALLING_CNT  (EM51_BASE+0xBCUL)    //0x180108BC
#define DS_CNT_EN                       BIT(28)
#define DS_FALLING_CNT_SHT              (12)    //21:12
#define DS_FALLING_CNT_MASK             (0x3FFUL << DS_FALLING_CNT_SHT)
#define DS_FALLING_CNT_GET(x)           ((x & DS_FALLING_CNT_MASK) >> DS_FALLING_CNT_SHT)
#define DS_FALLING_CNT_SET(x,y)         (x = ((x & ~DS_FALLING_CNT_MASK) | (y << DS_FALLING_CNT_SHT)))
#define DS_RISING_CNT_SHT               (0) //9:0
#define DS_RISING_CNT_MASK              (0x3FFUL << DS_RISING_CNT_SHT)
#define DS_RISING_CNT_GET(x)            ((x & DS_RISING_CNT_MASK) >> DS_RISING_CNT_SHT)
#define DS_RISING_CNT_SET(x,y)          (x = ((x & ~DS_RISING_CNT_MASK) | (y << DS_RISING_CNT_SHT)))

//#define EM51_RSTN_DEBUG_FOR_DS      (EM51_BASE+0xC0UL)    //0x180108C0
#define EMMC_RSTN_SEL                   BIT(0)

/* ============================================= */
/* EMMC PAD2 */
/* ============================================= */
//#define EM51_33V_SEL                (EM51_BASE+0xC4UL)    //0x180108C4
#define EMMC_IN33SEL                    BIT(0)  //default: 1; 0:1.8V; 1:3.3V
#define EMMC_3V3                        (1)
#define EMMC_1V8                        (0)

/* ============================================= */
/* Debug for MP(INT) */
/* ============================================= */
//#define EM51_INT_DEBUG_CTRL         (EM51_BASE+0xC8UL)    //0x180108C8
#define EMMC_INT_DBG_SECU_INT           BIT(2)
#define EMMC_INT_DBG_INT                BIT(1)
#define EMMC_INT_DBG_EN                 BIT(0)

/* ============================================= */
/* Clkgen */
/* ============================================= */
//#define EM51_CLKGEN_CTRL            (EM51_BASE+0xD0UL)    //0x180108D0
#define EMMC_CLKGEN_DIV_SEL_SHT         (2)     //4:2
#define EMMC_CLKGEN_DIV_SEL_MASK        (0x7UL << EMMC_CLKGEN_DIV_SEL_SHT)
#define EMMC_CLKGEN_DIV_SEL_GET(x)      ((x & EMMC_CLKGEN_DIV_SEL_MASK) >> EMMC_CLKGEN_DIV_SEL_SHT)
#define EMMC_CLKGEN_DIV_SEL_SET(x,y)    ((x & ~EMMC_CLKGEN_DIV_SEL_MASK) | (y << EMMC_CLKGEN_DIV_SEL_SHT))
#define PLL_DIV_2                       (0)
#define PLL_DIV_4                       (1)
#define PLL_DIV_8                       (2)
#define PLL_DIV_16                      (3)
#define PLL_DIV_200                     (4)
#define PLL_DIV_256                     (5)
#define PLL_DIV_512                     (6)
#define PLL_DIV_NON                     (7)
#define EMMC_CLKGEN_CLK_DIV_EN          BIT(1)
#define EMMC_CLKGEN_CLK_CARD_SEL        BIT(0)

/* ============================================= */
/* CClkout Delay */
/* ============================================= */
//#define EM51_CCLKOUT_CTRL           (EM51_BASE+0xD4UL)    //0x180108D4
#define CCLK_OUT_DELAY_EN               BIT(8)
#define CCLK_OUT_DELAY_COUNT_SHT        (0)     //7:0
#define CCLK_OUT_DELAY_COUNT_MASK       (0xFFUL << CCLK_OUT_DELAY_COUNT_SHT)
#define CCLK_OUT_DELAY_COUNT_GET(x)     ((x & CCLK_OUT_DELAY_COUNT_MASK) >> CCLK_OUT_DELAY_COUNT_SHT)
#define CCLK_OUT_DELAY_COUNT_SET(x,y)   (x = ((x & ~CCLK_OUT_DELAY_COUNT_MASK) | (y << CCLK_OUT_DELAY_COUNT_SHT)))

//#define EM51_RBUS_BE_CTRL           (EM51_BASE+0xD8UL)      //0x180108D8
#define RBUS_BE_EN                      BIT(4)
#define RBUS_BE_VAL_SHT                 BIT(0)  //3:0
#define RBUS_BE_VAL_MASK                (0xFFUL << RBUS_BE_VAL_SHT)
#define RBUS_BE_VAL_GET(x)              ((x & RBUS_BE_VAL_MASK) >> RBUS_BE_VAL_SHT)
#define RBUS_BE_VAL_SET(x,y)            (x = ((x & ~RBUS_BE_VAL_MASK) | (y << RBUS_BE_VAL_SHT)))

//#define EM51_DUMMY1                 (EM51_BASE+0xF0UL)      //0x180108F0
//#define EM51_DUMMY2                 (EM51_BASE+0xF4UL)      //0x180108F4
//#define EM51_DUMMY3                 (EM51_BASE+0xF8UL)      //0x180108F8
//#define EM51_DUMMY4                 (EM51_BASE+0xFCUL)      //0x180108FC
//#define EM51_DUMMY5                 (EM51_BASE+0xDCUL)      //0x180108DC


/* ============================================= */
/* Tune */
/* ============================================= */
//#define EM51_TUNE_CTRL1             (EM51_BASE+0xE0UL)    //0x180108E0
#define CMD_PH_SEL_SHT                  (0)         //4:0
#define CMD_PH_SEL_MASK                 (0x1FUL << CMD_PH_SEL_SHT)
#define CMD_PH_SEL_GET(x)               ((x & CMD_PH_SEL_MASK) >> CMD_PH_SEL_SHT)
#define CMD_PH_SEL_SET(x,y)             (x = ((x & ~CMD_PH_SEL_MASK) | (y << CMD_PH_SEL_SHT)))

//#define EM51_TUNE_CTRL2             (EM51_BASE+0xE4UL)    //0x180108E4
#define DATA3_PH_SEL_SHT                (24)    //28:24
#define DATA3_PH_SEL_MASK               (0x1FUL << DATA3_PH_SEL_SHT)
#define DATA3_PH_SEL_GET(x)             ((x & DATA3_PH_SEL_MASK) >> DATA3_PH_SEL_SHT)
#define DATA3_PH_SEL_SET(x,y)           (x = ((x & ~DATA3_PH_SEL_MASK) | (y << DATA3_PH_SEL_SHT)))
#define DATA2_PH_SEL_SHT                (16)    //20:16
#define DATA2_PH_SEL_MASK               (0x1FUL << DATA2_PH_SEL_SHT)
#define DATA2_PH_SEL_GET(x)             ((x & DATA2_PH_SEL_MASK) >> DATA2_PH_SEL_SHT)
#define DATA2_PH_SEL_SET(x,y)           (x = ((x & ~DATA2_PH_SEL_MASK) | (y << DATA2_PH_SEL_SHT)))
#define DATA1_PH_SEL_SHT                (8)     //12:8
#define DATA1_PH_SEL_MASK               (0x1FUL << DATA1_PH_SEL_SHT)
#define DATA1_PH_SEL_GET(x)             ((x & DATA1_PH_SEL_MASK) >> DATA1_PH_SEL_SHT)
#define DATA1_PH_SEL_SET(x,y)           (x = ((x & ~DATA1_PH_SEL_MASK) | (y << DATA1_PH_SEL_SHT)))
#define DATA0_PH_SEL_SHT                (0)     //4:0
#define DATA0_PH_SEL_MASK               (0x1FUL << DATA0_PH_SEL_SHT)
#define DATA0_PH_SEL_GET(x)             ((x & DATA0_PH_SEL_MASK) >> DATA0_PH_SEL_SHT)
#define DATA0_PH_SEL_SET(x,y)           (x = ((x & ~DATA0_PH_SEL_MASK) | (y << DATA0_PH_SEL_SHT)))

//#define EM51_TUNE_CTRL3             (EM51_BASE+0xE8UL)    //0x180108E8
#define DATA7_PH_SEL_SHT                (24)    //28:24
#define DATA7_PH_SEL_MASK               (0x1FUL << DATA7_PH_SEL_SHT)
#define DATA7_PH_SEL_GET(x)             ((x & DATA7_PH_SEL_MASK) >> DATA7_PH_SEL_SHT)
#define DATA7_PH_SEL_SET(x,y)           (x = ((x & ~DATA7_PH_SEL_MASK) | (y << DATA7_PH_SEL_SHT)))
#define DATA6_PH_SEL_SHT                (16)    //20:16
#define DATA6_PH_SEL_MASK               (0x1FUL << DATA6_PH_SEL_SHT)
#define DATA6_PH_SEL_GET(x)             ((x & DATA6_PH_SEL_MASK) >> DATA6_PH_SEL_SHT)
#define DATA6_PH_SEL_SET(x,y)           (x = ((x & ~DATA6_PH_SEL_MASK) | (y << DATA6_PH_SEL_SHT)))
#define DATA5_PH_SEL_SHT                (8)     //12:8
#define DATA5_PH_SEL_MASK               (0x1FUL << DATA5_PH_SEL_SHT)
#define DATA5_PH_SEL_GET(x)             ((x & DATA5_PH_SEL_MASK >> DATA5_PH_SEL_SHT)
#define DATA5_PH_SEL_SET(x,y)           (x = ((x & ~DATA5_PH_SEL_MASK) | (y << DATA5_PH_SEL_SHT)))
#define DATA4_PH_SEL_SHT                (0)     //4:0
#define DATA4_PH_SEL_MASK               (0x1FUL << DATA4_PH_SEL_SHT)
#define DATA4_PH_SEL_GET(x)             ((x & DATA4_PH_SEL_MASK) >> DATA4_PH_SEL_SHT)
#define DATA4_PH_SEL_SET(x,y)           (x = ((x & ~DATA4_PH_SEL_MASK) | (y << DATA4_PH_SEL_SHT)))

//#define EM51_TUNE_CTRL4             (EM51_BASE+0xECUL)    //0x180108EC
#define DS2_PH_SEL_SHT                  (0)     //4:0
#define DS2_PH_SEL_MASK                 (0x1FUL << DS2_PH_SEL_SHT)
#define DS2_PH_SEL_GET(x)               ((x & DS2_PH_SEL_MASK) >> DS2_PH_SEL_SHT)
#define DS2_PH_SEL_SET(x,y)             (x = ((x & ~DS2_PH_SEL_MASK) | (y << DS2_PH_SEL_SHT)))


/* ============================================= */
/* **********   DWC_Mobile_Storage   **********  */
/* ============================================= */
/* ============================================= */
/* DWC_mshc_block_rigisters         0x000~0x0ff  */
/* ============================================= */
//#define DWC51_SDMASA_R                          (DWC51_BASE+0x000UL)     //0x18010900
//#define DWC51_BLOCKSIZE_R16                       (DWC51_BASE+0x004UL)     //0x18010904
//#define RSVD_BLOCKSIZE15        15
#define SDMA_BUF_BDARY_SHT              (12)    //14:12
#define SDMA_BUF_BDARY_MASK             (0x7UL << SDMA_BUF_BDARY_SHT)
#define SDMA_BUF_BDARY_GET(x)           ((x & SDMA_BUF_BDARY_MASK) >> SDMA_BUF_BDARY_SHT)
#define SDMA_BUF_BDARY_SET(x,y)         (x = ((x & ~SDMA_BUF_BDARY_MASK) | (y << SDMA_BUF_BDARY_SHT)))
#define XFER_BLOCK_SIZE_SHT             (0)     //11:0
#define XFER_BLOCK_SIZE_MASK            (0xFFFUL << XFER_BLOCK_SIZE_SHT)
#define XFER_BLOCK_SIZE_GET(x)          ((x & XFER_BLOCK_SIZE_MASK) >> XFER_BLOCK_SIZE_SHT)
#define XFER_BLOCK_SIZE_SET(x,y)        (x = ((x & ~XFER_BLOCK_SIZE_MASK) | (y << XFER_BLOCK_SIZE_SHT)))


//#define DWC51_BLOCKCOUNT_R16                      (DWC51_BASE+0x006UL)     //0x18010906 (2Bytes)
//#define DWC51_ARGUMENT_R                          (DWC51_BASE+0x008UL)     //0x18010908
//#define DWC51_XFER_MODE_R16                       (DWC51_BASE+0x00CUL)     //0x1801090C (2Bytes)
//#define RSVD                    15:9
#define RESP_INT_DISABLE                BIT(8)
#define RESP_ERR_CHK_ENABLE             BIT(7)
#define RESP_TYPE                       BIT(6)
#define MULTI_BLK_SEL                   BIT(5)
#define DATA_XFER_DIR                   BIT(4)
#define AUTO_CMD_ENABLE_SHT             (2)     //3:2
#define AUTO_CMD_ENABLE_MASK            (0x3UL << AUTO_CMD_ENABLE_SHT)
#define AUTO_CMD_ENABLE_GET(x)          ((x & AUTO_CMD_ENABLE_MASK) >> AUTO_CMD_ENABLE_SHT)
#define AUTO_CMD_ENABLE_SET(x,y)        (x = ((x & ~AUTO_CMD_ENABLE_MASK) | (y << AUTO_CMD_ENABLE_SHT)))
#define AUTO_CMD_DISABLED   (0x0)
#define AUTO_CMD12_ENABLED  (0x1)
#define AUTO_CMD23_ENABLED  (0x2)
#define AUTO_CMD_AUTO_SEL   (0x3)
#define BLOCK_COUNT_ENABLE              BIT(1)
#define DMA_ENABLE                      BIT(0)

/*
typedef union{
    unsigned short  xfermode_r;
    struct{
        unsigned short dma_en:1;            //bit[0]
        unsigned short blk_cnt_en:1;        //bit[1]
        unsigned short auto_cmd_en:2;       //bit[2:3]
        unsigned short data_xfer_dir:1;     //bit[4]
        unsigned short multi_blk_sel:1;     //bit[5]
        unsigned short resp_type:1;         //bit[6]
        unsigned short resp_err_chk_en:1;   //bit[7]
        unsigned short resp_int_dis:1;      //bit[8]
        unsigned short rsvd:7;              //bit[9:15]
    };
}xfermode_r_t;
*/

//#define DWC51_CMD_R16                             (DWC51_BASE+0x00EUL)     //0x1801090E (2Bytes)
//#define RSVD                    15:14
#define CMD_INDEX_SHT                   (8)     //13:8
#define CMD_INDEX_MASK                  (0x3FUL << CMD_INDEX_SHT)
#define CMD_INDEX_GET(x)                ((x & CMD_INDEX_MASK) >> CMD_INDEX_SHT)
#define CMD_INDEX_SET(x,y)              (x = ((x & ~CMD_INDEX_MASK) | (y << CMD_INDEX_SHT)))
#define CMD_TYPE_SHT                    (6)     //7:6
#define CMD_TYPE_MASK                   (0x3UL << CMD_TYPE_SHT)
#define CMD_TYPE_GET(x)                 ((x & CMD_TYPE_MASK) >> CMD_TYPE_SHT)
#define CMD_TYPE_SET(x,y)               (x = ((x & ~CMD_TYPE_MASK) | (y << CMD_TYPE_SHT)))
#define ABORT_CMD                       (0x3)
#define RESUME_CMD                      (0x2)
#define SUSPEND_CMD                     (0x1)
#define NORMAL_CMD                      (0x0)
#define DATA_PRESENT_SEL                BIT(5)
#define CMD_IDX_CHK_ENABLE              BIT(4)
#define CMD_CRC_CHK_ENABLE              BIT(3)
#define SUB_CMD_FLAG                    BIT(2)
#define RESP_TYPE_SELECT_SHT            (0)     //1:0
#define RESP_TYPE_SELECT_MASK           (0x3UL << RESP_TYPE_SELECT_SHT)
#define RESP_TYPE_SELECT_GET(x)         ((x & RESP_TYPE_SELECT_MASK) >> RESP_TYPE_SELECT_SHT)
#define RESP_TYPE_SELECT_SET(x,y)       (x = ((x & ~RESP_TYPE_SELECT_MASK) | (y << RESP_TYPE_SELECT_SHT)))
#define NO_RESP      (0x0)
#define RESP_LEN_136 (0x1)
#define RESP_LEN_48  (0x2)
#define RESP_LEN_48B (0x3)

//#define DWC51_RESP01_R                          (DWC51_BASE+0x010UL)     //0x18010910
//#define DWC51_RESP23_R                          (DWC51_BASE+0x014UL)     //0x18010914
//#define DWC51_RESP45_R                          (DWC51_BASE+0x018UL)     //0x18010918
//#define DWC51_RESP67_R                          (DWC51_BASE+0x01CUL)     //0x1801091C
//#define DWC51_BUF_DATA_R                        (DWC51_BASE+0x020UL)     //0x18010920
//#define DWC51_PSTATE_REG_R                      (DWC51_BASE+0x024UL)     //0x18010924
#define UHS2_IF_DETECT                  BIT(31)
#define LANE_SYNC                       BIT(30)
#define IN_DORMANT_ST                   BIT(29)
#define SUB_CMD_STAT                    BIT(28)
#define CMD_ISSUE_ERR                   BIT(27)
//#define RSVD_26             BIT(26)
#define HOST_REG_VOL                    BIT(25)
#define CMD_LINE_LVL                    BIT(24)
#define DAT_3_0_SHT                     (20)    //23:20
#define DAT_3_0_MASK                    (0xFUL << DAT_3_0_SHT)
#define DAT_3_0_GET(x)                  ((x & DAT_3_0_MASK) >> DAT_3_0_SHT)
#define DAT_3_0_SET(x,y)                (x = ((x & ~DAT_3_0_MASK) | (y << DAT_3_0_SHT)))
#define WR_PROTECT_SW_LVL               BIT(19)
#define CARD_DETECT_PIN_LEVEL           BIT(18)
#define CARD_STABLE                     BIT(17)
#define CARD_INSERTED                   BIT(16)
//#define RSVD_15_12          15:12
#define BUF_RD_ENABLE                   BIT(11)
#define BUF_WR_ENABLE                   BIT(10)
#define RD_XFER_ACTIVE                  BIT(9)
#define WR_XFER_ACTIVE                  BIT(8)
#define DAT_7_4_SHT                     (4)     //7:4
#define DAT_7_4_MASK                    (0xFUL << DAT_7_4_SHT)
#define DAT_7_4_GET(x)                  ((x & DAT_7_4_MASK) >> DAT_7_4_SHT)
#define DAT_7_4_SET(x,y)                (x = ((x & ~DAT_7_4_MASK) | (y << DAT_7_4_SHT)))
#define RE_TUNE_REQ                     BIT(3)
#define DAT_LINE_ACTIVE                 BIT(2)
#define CMD_INHIBIT_DAT                 BIT(1)
#define CMD_INHIBIT                     BIT(0)
#define HOST_INHIBIT_MASK   (CMD_INHIBIT|CMD_INHIBIT_DAT)
#define HOST_IDLE_MASK      (CMD_INHIBIT|CMD_INHIBIT_DAT|DAT_LINE_ACTIVE)


//#define DWC51_HOST_CTRL1_R8                      (DWC51_BASE+0x028UL)     //0x18010928 (1Bytes)
#define CARD_DETECT_SIG_SEL             BIT(7)
#define CARD_DETECT_TEST_LVL            BIT(6)
#define EXT_DAT_XFER                    BIT(5)
#define DAT_XFER_WIDTH8                 (EXT_DAT_XFER)
#define DMA_SEL_SHT                     (3)     //4:3
#define DMA_SEL_MASK                    (0x3UL << DMA_SEL_SHT)
#define DMA_SEL_GET(x)                  ((x & DMA_SEL_MASK) >> DMA_SEL_SHT)
#define DMA_SEL_SET(x,y)                (x = ((x & ~DMA_SEL_MASK) | (y << DMA_SEL_SHT)))
#define DMA_SEL(x)                      (DMA_SEL_MASK & ((x) << DMA_SEL_SHT))
#define DMA_SEL_ADMA2_32                (2)
#define DMA_SEL_ADMA2_64                (3)
#define HIGH_SPEED_EN                   BIT(2)
#define DAT_XFER_WIDTH                  BIT(1)
#define DAT_XFER_WIDTH4                 (DAT_XFER_WIDTH)
#define DAT_XFER_WIDTH1                 (0)
#define LED_CTRL                        BIT(0)


//#define DWC51_PWR_CTRL_R8                        (DWC51_BASE+0x029UL)     //0x18010929 (1Bytes)
#define SD_BUS_VOL_VDD2_SHT             (5)     //7:5
#define SD_BUS_VOL_VDD2_MASK            (0x7 << SD_BUS_VOL_VDD2_SHT)
#define SD_BUS_VOL_VDD2_GET(x)          ((x & SD_BUS_VOL_VDD2_MASK) >> SD_BUS_VOL_VDD2_SHT)
#define SD_BUS_VOL_VDD2_SET(x,y)        (x = ((x & ~SD_BUS_VOL_VDD2_MASK) | (y << SD_BUS_VOL_VDD2_SHT)))
#define SD_BUS_PWR_VDD2                 BIT(4)
#define SD_BUS_VOL_VDD1_SHT             (1)     //3:1
#define SD_BUS_VOL_VDD1_MASK            (0x7 << SD_BUS_VOL_VDD1_SHT)
#define SD_BUS_VOL_VDD1_GET(x)          ((x & SD_BUS_VOL_VDD1_MASK) >> SD_BUS_VOL_VDD1_SHT)
#define SD_BUS_VOL_VDD1_SET(x,y)        (x = ((x & ~SD_BUS_VOL_VDD1_MASK) | (y << SD_BUS_VOL_VDD1_SHT)))
#define SD_BUS_VOL_VDD1(x)              (SD_BUS_VOL_VDD1_MASK & ((x) << SD_BUS_VOL_VDD1_SHT))
#define eMMC_3V3                        (7)
#define eMMC_1V8                        (6)
#define eMMC_1V2                        (5)
#define SD_BUS_PWR_VDD1                 BIT(0)


//#define DWC51_BGAP_CTRL_R                       (DWC51_BASE+0x02AUL)     //0x1801092A (1Bytes)
//#define RSVD_7_4                7:4
#define INT_AT_BGAP                     BIT(3)
#define RD_WAIT_CTRL                    BIT(2)
#define CONTINUE_REQ                    BIT(1)
#define STOP_BG_REQ                     BIT(0)


//#define DWC51_WUP_CTRL_R                        (DWC51_BASE+0x02BUL)     //0x1801092B (1Bytes)
//#define RSVD_7_3                7:3
#define CARD_REMOVAL_WUP_CTRL           BIT(2)
#define CARD_INSERT                     BIT(1)
#define CARD_INT                        BIT(0)


//#define DWC51_CLK_CTRL_R16                        (DWC51_BASE+0x02CUL)     //0x1801092C (2Bytes)
#define FREQ_SEL_SHT                    (8)     //15:8
#define FREQ_SEL_MASK                   (0xFFUL << FREQ_SEL_SHT)
#define FREQ_SEL_GET(x)                 ((x & FREQ_SEL_MASK) >> FREQ_SEL_SHT)
#define FREQ_SEL_SET(x,y)               (x = ((x & ~FREQ_SEL_MASK) | (y << FREQ_SEL_SHT)))
#define UPPER_FREQ_SEL_SHT              (6)     //7:6
#define UPPER_FREQ_SEL_MASK             (0x3UL << UPPER_FREQ_SEL_SHT)
#define UPPER_FREQ_SEL_GET(x)           ((x & UPPER_FREQ_SEL_MASK) >> UPPER_FREQ_SEL_SHT)
#define UPPER_FREQ_SEL_SET(x,y)         (x = ((x & ~UPPER_FREQ_SEL_MASK) | (y << UPPER_FREQ_SEL_SHT)))
#define CLK_GEN_SELECT                  BIT(5)
//#define RSVD_4                          BIT(4)
#define PLL_ENABLE                      BIT(3)
#define SD_CLK_EN                       BIT(2)
#define INTERNAL_CLK_STABLE             BIT(1)
#define INTERNAL_CLK_EN                 BIT(0)


//#define DWC51_TOUT_CTRL_R8                       (DWC51_BASE+0x02EUL)     //0x1801092E (1Bytes)
//#define RSVD_7_4            7:4
#define TOUT_CNT_SHT                    (0)     //3:0
#define TOUT_CNT_MASK                   (0xFUL << TOUT_CNT_SHT)
#define TOUT_CNT_GET(x)                 ((x & TOUT_CNT_MASK) >> TOUT_CNT_SHT)
#define TOUT_CNT_SET(x,y)               (x = ((x & ~TOUT_CNT_MASK) | (y << TOUT_CNT_SHT)))
#define TMCLK_x_2P13        (0)
#define TMCLK_x_2P14        (1)
#define TMCLK_x_2P15        (2)
#define TMCLK_x_2P16        (3)
#define TMCLK_x_2P17        (4)
#define TMCLK_x_2P18        (5)
#define TMCLK_x_2P19        (6)
#define TMCLK_x_2P20        (7)
#define TMCLK_x_2P21        (8)
#define TMCLK_x_2P22        (9)
#define TMCLK_x_2P23        (10)
#define TMCLK_x_2P24        (11)
#define TMCLK_x_2P25        (12)
#define TMCLK_x_2P26        (13)
#define TMCLK_x_2P27        (14)


//#define DWC51_SW_RST_R8                      (DWC51_BASE+0x02FUL)     //0x1801092F
//#define RSVD_7_3                7:3
#define SW_RST_DAT                      BIT(2)
#define SW_RST_CMD                      BIT(1)
#define SW_RST_ALL                      BIT(0)


//#define DWC51_NORMAL_INT_STAT_R16                 (DWC51_BASE+0x030UL)     //0x18010930 (2Bytes)
//#define DWC51_NORMAL_INT_STAT_EN_R16              (DWC51_BASE+0x034UL)     //0x18010934 (2Bytes)
//#define DWC51_NORMAL_INT_SIGNAL_EN_R16            (DWC51_BASE+0x038UL)     //0x18010938 (2Bytes)
#define ERR_INTERRUPT                   BIT(15)
#define CQE_EVENT                       BIT(14)
#define FX_EVENT                        BIT(13) // <== This interrupt is used with response check function. We may no need.
#define RE_TUNE_EVENT                   BIT(12)
#define INT_C                           BIT(11)
#define INT_B                           BIT(10)
#define INT_A                           BIT(9)
#define CARD_INTERRUPT                  BIT(8)
#define CARD_REMOVAL_INT_STAT           BIT(7)
#define CARD_INSERTION                  BIT(6)
#define BUF_RD_READY                    BIT(5)  // <== This bit is set if the Buffer Read Enable changes from 0 to 1. for PIO mode
#define BUF_WR_READY                    BIT(4)  // <== This bit is set if the Buffer Write Enable changes from 0 to 1. for PIO mode
#define DMA_INTERRUPT                   BIT(3)
#define BGAP_EVENT                      BIT(2)  // <== This bit is set when both read/write transaction is stopped at block gap due to a Stop at Block Gap Request. we has no use BLOCK GAP func
#define XFER_COMPLETE                   BIT(1)
#define CMD_COMPLETE                    BIT(0)

#if 0
#define MMC_INT_EVENT   \
            (CMD_COMPLETE | XFER_COMPLETE | BGAP_EVENT |  \
             BUF_WR_READY | BUF_RD_READY | FX_EVENT | \
             CQE_EVENT | ERR_INTERRUPT)
#else
#define MMC_INT_EVENT   \
            (CMD_COMPLETE | XFER_COMPLETE | \
            CQE_EVENT | ERR_INTERRUPT)
#endif
//#define DWC51_ERROR_INT_STAT_R16                  (DWC51_BASE+0x032UL)     //0x18010932 (2Bytes)
//#define DWC51_ERROR_INT_STAT_EN_R16               (DWC51_BASE+0x036UL)     //0x18010936 (2Bytes)
//#define DWC51_ERROR_INT_SIGNAL_EN_R16             (DWC51_BASE+0x03AUL)     //0x1801093A (2Bytes)
#define VENDOR_ERR3                     BIT(15)
#define VENDOR_ERR2                     BIT(14)
#define VENDOR_ERR1                     BIT(13)
#define WRAP_STA0_RBUS                  (VENDOR_ERR1)
#define BOOT_ACK_ERR                    BIT(12)
#define RESP_ERR                        BIT(11)     //for resp error check func only
#define TUNING_ERR                      BIT(10)
#define ADMA_ERR                        BIT(9)
#define AUTO_CMD_ERR                    BIT(8)
#define CUR_LMT_ERR                     BIT(7)
#define DATA_END_BIT_ERR                BIT(6)
#define DATA_CRC_ERR                    BIT(5)
#define DATA_TOUT_ERR                   BIT(4)
#define CMD_IDX_ERR                     BIT(3)
#define CMD_END_BIT_ERR                 BIT(2)
#define CMD_CRC_ERR                     BIT(1)
#define CMD_TOUT_ERR                    BIT(0)

/* XXX_ERR_H: if the status of err was read out simultaneously with cmd.
             it need shift 16bit.  */
#define VENDOR_ERR3_H                  (VENDOR_ERR3 << 16)
#define VENDOR_ERR2_H                  (VENDOR_ERR2 << 16)
#define VENDOR_ERR1_H                  (VENDOR_ERR1 << 16)
#define WRAP_STA0_RBUS_H                (VENDOR_ERR1_H)
#define BOOT_ACK_ERR_H                 (BOOT_ACK_ERR << 16)
#define RESP_ERR_H                     (RESP_ERR << 16)
#define TUNING_ERR_H                   (TUNING_ERR << 16)
#define ADMA_ERR_H                     (ADMA_ERR << 16)
#define AUTO_CMD_ERR_H                 (AUTO_CMD_ERR << 16)
#define CUR_LMT_ERR_H                  (CUR_LMT_ERR << 16)
#define DATA_END_BIT_ERR_H             (DATA_END_BIT_ERR << 16)
#define DATA_CRC_ERR_H                 (DATA_CRC_ERR << 16)
#define DATA_TOUT_ERR_H                (DATA_TOUT_ERR << 16)
#define CMD_IDX_ERR_H                  (CMD_IDX_ERR << 16)
#define CMD_END_BIT_ERR_H              (CMD_END_BIT_ERR << 16)
#define CMD_CRC_ERR_H                  (CMD_CRC_ERR << 16)
#define CMD_TOUT_ERR_H                 (CMD_TOUT_ERR << 16)


#define ERR_CMD_REASON_MASK            (CMD_CRC_ERR|CMD_END_BIT_ERR|CMD_IDX_ERR)    /*not include timeout case */
#define ERR_DATA_REASON_MASK           (DATA_CRC_ERR|DATA_END_BIT_ERR| \
                                        ADMA_ERR|TUNING_ERR|WRAP_STA0_RBUS)         /*not include timeout case */
#define ERR_UNEXPECT_MASK              (CUR_LMT_ERR|RESP_ERR|BOOT_ACK_ERR)

#define ERR_CMD_REASON_MASK_H          (ERR_CMD_REASON_MASK<<16)
#define ERR_DATA_REASON_MASK_H         (ERR_DATA_REASON_MASK<<16)
#define ERR_UNEXPECT_MASK_H            (ERR_UNEXPECT_MASK<<16)

/* for bit operation */
#define ERR_INTERRUPT_BIT               (15)
#define CQE_EVENT_BIT                   (14)
#define FX_EVENT_BIT                    (13)
#define RE_TUNE_EVENT_BIT               (12)
#define INT_C_BIT                       (11)
#define INT_B_BIT                       (10)
#define INT_A_BIT                       (9)     //used as wrap_sta0: RBUS_ERR
#define CARD_INTERRUPT_BIT              (8)
#define CARD_REMOVAL_INT_STAT_BIT       (7)
#define CARD_INSERTION_BIT              (6)
#define BUF_RD_READY_BIT                (5)
#define BUF_WR_READY_BIT                (4)
#define DMA_INTERRUPT_BIT               (3)
#define BGAP_EVENT_BIT                  (2)
#define XFER_COMPLETE_BIT               (1)
#define CMD_COMPLETE_BIT                (0)

#define VENDOR_ERR3_BIT                 (15)
#define VENDOR_ERR2_BIT                 (14)
#define VENDOR_ERR1_BIT                 (13)
#define WRAP_STA0_RBUS_BIT             (VENDOR_ERR1_BIT)    // data error
#define BOOT_ACK_ERR_BIT                (12)    // unexp error
#define RESP_ERR_STA_BIT                (11)    // unexp error   //modify name for RESP_ERR_BIT
#define TUNING_ERR_BIT                  (10)    // data error
#define ADMA_ERR_BIT                    (9)     // data erro
#define AUTO_CMD_ERR_BIT                (8)     // data error; for cmd12, it's before data;for cmd23, it's before data
#define CUR_LMT_ERR_BIT                 (7)     // unexp error
#define DATA_END_BIT_ERR_BIT            (6)     // data error
#define DATA_CRC_ERR_BIT                (5)     // data error
#define DATA_TOUT_ERR_BIT               (4)     // data error
#define CMD_IDX_ERR_BIT                 (3)     // cmd error
#define CMD_END_BIT_ERR_BIT             (2)     // cmd error
#define CMD_CRC_ERR_BIT                 (1)     // cmd error
#define CMD_TOUT_ERR_BIT                (0)     // cmd error


/* XXX_ERR_H_BIT: if the status of err was read out simultaneously with cmd.
             it need add 16.  */
#define VENDOR_ERR3_H_BIT               (VENDOR_ERR3_BIT+16)
#define VENDOR_ERR2_H_BIT               (VENDOR_ERR2_BIT+16)
#define VENDOR_ERR1_H_BIT               (VENDOR_ERR1_BIT+16)
#define WRAP_STA0_RBUS_H_BIT             (VENDOR_ERR1_H_BIT)
#define BOOT_ACK_ERR_H_BIT              (BOOT_ACK_ERR_BIT+16)
#define RESP_ERR_STA_H_BIT              (RESP_ERR_BIT+16)   //modify name for RESP_ERR_BIT
#define TUNING_ERR_H_BIT                (TUNING_ERR_BIT+16)
#define ADMA_ERR_H_BIT                  (ADMA_ERR_BIT+16)
#define AUTO_CMD_ERR_H_BIT              (AUTO_CMD_ERR_BIT+16)
#define CUR_LMT_ERR_H_BIT               (CUR_LMT_ERR_BIT+16)
#define DATA_END_BIT_ERR_H_BIT          (DATA_END_BIT_ERR_BIT+16)
#define DATA_CRC_ERR_H_BIT              (DATA_CRC_ERR_BIT+16)
#define DATA_TOUT_ERR_H_BIT             (DATA_TOUT_ERR_BIT+16)
#define CMD_IDX_ERR_H_BIT               (CMD_IDX_ERR_BIT+16)
#define CMD_END_BIT_ERR_H_BIT           (CMD_END_BIT_ERR_BIT+16)
#define CMD_CRC_ERR_H_BIT               (CMD_CRC_ERR_BIT+16)
#define CMD_TOUT_ERR_H_BIT              (CMD_TOUT_ERR_BIT+16)


//#define DWC51_AUTO_CMD_STAT_R16                   (DWC51_BASE+0x03CUL)     //0x1801093C (2Bytes)
//#define RSVD_15_8                       15:8
#define CMD_NOT_ISSUED_AUTO_CMD12       BIT(7)
#define RSVD_6                          BIT(6)
#define AUTO_CMD_RESP_ERR               BIT(5)
#define AUTO_CMD_IDX_ERR                BIT(4)
#define AUTO_CMD_EBIT_ERR               BIT(3)
#define AUTO_CMD_CRC_ERR                BIT(2)
#define AUTO_CMD_TOUT_ERR               BIT(1)
#define AUTO_CMD12_NOT_EXEC             BIT(0)  // this is not error. it's alert information

#define ERR_AUTO_CMD23_MASK               (AUTO_CMD_TOUT_ERR|AUTO_CMD_CRC_ERR| \
                                            AUTO_CMD_EBIT_ERR|AUTO_CMD_EBIT_ERR|AUTO_CMD_IDX_ERR)
#define ERR_AUTO_CMD12_MASK               (ERR_AUTO_CMD23_MASK|CMD_NOT_ISSUED_AUTO_CMD12)

//#define DWC51_HOST_CTRL2_R16                      (DWC51_BASE+0x03EUL)     //0x1801093E (2Bytes)
#define PRESET_VAL_ENABLE               BIT(15)
#define ASYNC_INT_ENABLE                BIT(14)
#define ADDRESSING                      BIT(13)
#define HOST_VER4_ENABLE                BIT(12)
#define CMD23_ENABLE                    BIT(11)
#define ADMA2_LEN_MODE                  BIT(10)
#define ADMA2_LEN_26BIT                 (ADMA2_LEN_MODE)
#define RSVD_9                          BIT(9)
#define UHS2_IF_ENABLE                  BIT(8)
#define SAMPLE_CLK_SEL                  BIT(7)
#define EXEC_TUNING                     BIT(6)
#define DRV_STRENGTH_SEL_SHT            (4)     //5:4
#define DRV_STRENGTH_SEL_MASK           (0x3UL << DRV_STRENGTH_SEL_SHT)
#define DRV_STRENGTH_SEL_GET(x)         ((x & DRV_STRENGTH_SEL_MASK) >> DRV_STRENGTH_SEL_SHT)
#define DRV_STRENGTH_SEL_SET(x,y)       (x = ((x & ~DRV_STRENGTH_SEL_MASK) | (y << DRV_STRENGTH_SEL_SHT)))
#define DRV_STRENGTH_SEL(x)             (DRV_STRENGTH_SEL_MASK & ((x) << DRV_STRENGTH_SEL_SHT))
#define SIGNALING_EN                    BIT(3)
#define MMC_MODE_SEL_SHT                (0)     //2:0 - UHS_MODE_SEL rename to --
#define MMC_MODE_SEL_MASK               (0x7UL << MMC_MODE_SEL_SHT)
#define MMC_MODE_SEL_GET(x)             ((x & MMC_MODE_SEL_MASK) >> MMC_MODE_SEL_SHT)
#define MMC_MODE_SEL_SET(x,y)           ((x & ~MMC_MODE_SEL_MASK) | (y << MMC_MODE_SEL_SHT))
#define MMC_MODE_SEL(x)                 (MMC_MODE_SEL_MASK & ((x) << MMC_MODE_SEL_SHT))
#define MMC_MODE_LEGACY                 (0)
#define MMC_MODE_HS_SDR                 (1)
#define MMC_MODE_HS200                  (3)
#define MMC_MODE_HS_DDR                 (4)
#define MMC_MODE_HS400                  (7)


//#define DWC51_CAPABILITIES1_R                   (DWC51_BASE+0x040UL)     //0x18010940
#define SLOT_TYPE_R_SHT                 (30)    //31:30
#define SLOT_TYPE_R_MASK                (0x3UL << SLOT_TYPE_R_SHT)
#define SLOT_TYPE_R_GET(x)              ((x & SLOT_TYPE_R_MASK) >> SLOT_TYPE_R_SHT)
#define SLOT_TYPE_R_SET(x,y)            (x = ((x & ~SLOT_TYPE_R_MASK) | (y << SLOT_TYPE_R_SHT)))
#define ASYNC_INT_SUPPORT               BIT(29)
#define SYS_ADDR_64_V3                  BIT(28)
#define SYS_ADDR_64_V4                  BIT(27)
#define VOLT_18                         BIT(26)
#define VOLT_30                         BIT(25)
#define VOLT_33                         BIT(24)
#define SUS_RES_SUPPORT                 BIT(23)
#define SDMA_SUPPORT                    BIT(22)
#define HIGH_SPEED_SUPPORT              BIT(21)
//#define RSVD_20                     20
#define ADMA2_SUPPORT                   BIT(19)
#define Embedded_8_BIT                  BIT(18)
#define MAX_BLK_LEN_SHT                 (16)    //17:16
#define MAX_BLK_LEN_MASK                (0x3UL << MAX_BLK_LEN_SHT)
#define MAX_BLK_LEN_GET(x)              ((x & MAX_BLK_LEN_MASK) >> MAX_BLK_LEN_SHT)
#define MAX_BLK_LEN_SET(x,y)            (x = ((x & ~MAX_BLK_LEN_MASK) | (y << MAX_BLK_LEN_SHT)))
#define BASE_CLK_FREQ_SHT               (8)         //15:8
#define BASE_CLK_FREQ_MASK              (0xFFUL << BASE_CLK_FREQ_SHT)
#define BASE_CLK_FREQ_GET(x)            ((x & BASE_CLK_FREQ_MASK) >> BASE_CLK_FREQ_SHT)
#define BASE_CLK_FREQ_SET(x,y)          (x = ((x & ~BASE_CLK_FREQ_MASK) | (y << BASE_CLK_FREQ_SHT)))
#define TOUT_CLK_UNIT                   BIT(7)
//#define RSVD_6                      6
#define TOUT_CLK_FREQ_SHT               (0)     //5:0
#define TOUT_CLK_FREQ_MASK              (0x3FUL << TOUT_CLK_FREQ_SHT)
#define TOUT_CLK_FREQ_GET(x)            ((x & TOUT_CLK_FREQ_MASK) >> TOUT_CLK_FREQ_SHT)
#define TOUT_CLK_FREQ_SET(x,y)          (x = ((x & ~TOUT_CLK_FREQ_MASK) | (y << TOUT_CLK_FREQ_SHT)))


//#define DWC51_CAPABILITIES2_R                   (DWC51_BASE+0x044UL)     //0x18010944
//#define RSVD_62_63                  31:30
//#define RSVD_61                     29
#define VDD2_18V_SUPPORT                BIT(28)
#define ADMA3_SUPPORT                   BIT(27)
//#define RSVD_56_58                  26:24
#define CLK_MUL_SHT                     (16)    //23:16
#define CLK_MUL_MASK                    (0xFFUL << CLK_MUL_SHT)
#define CLK_MUL_GET(x)                  ((x & CLK_MUL_MASK) >> CLK_MUL_SHT)
#define CLK_MUL_SET(x,y)                (x = ((x & ~CLK_MUL_MASK) | (y << CLK_MUL_SHT)))
#define RE_TUNING_MODES_SHT             (14)         //15:14
#define RE_TUNING_MODES_MASK            (0x3UL << RE_TUNING_MODES_SHT)
#define RE_TUNING_MODES_GET(x)          ((x & RE_TUNING_MODES_MASK) >> RE_TUNING_MODES_SHT
#define RE_TUNING_MODES_SET(x,y)        (x = ((x & ~RE_TUNING_MODES_MASK) | (y << RE_TUNING_MODES_SHT)))
#define USE_TUNING_SDR50                BIT(13)
//#define RSVD_44                     12
#define RETUNE_CNT_SHT                  (8)     //11:8
#define RETUNE_CNT_MASK                 (0xFUL << RETUNE_CNT_SHT)
#define RETUNE_CNT_GET(x)               ((x & RETUNE_CNT_MASK) >> RETUNE_CNT_SHT)
#define RETUNE_CNT_SET(x,y)             (x = ((x & ~RETUNE_CNT_MASK) | (y << RETUNE_CNT_SHT)))
//#define RSVD_39                     7
#define DRV_TYPED                       BIT(6)
#define DRV_TYPEC                       BIT(5)
#define DRV_TYPEA                       BIT(4)
#define UHS2_SUPPORT                    BIT(3)
#define DDR50_SUPPORT                   BIT(2)
#define SDR104_SUPPORT                  BIT(1)
#define SDR50_SUPPORT                   BIT(0)


//#define DWC51_CURR_CAPABILITIES1_R              (DWC51_BASE+0x048UL)     //0x18010948
//#define RSVD_31_24                  31:24
#define MAX_CUR_18V_SHT                 (16)    //23:16
#define MAX_CUR_18V_MASK                (0xFFUL << MAX_CUR_18V_SHT)
#define MAX_CUR_18V_GET(x)              ((x & MAX_CUR_18V_MASK) >> MAX_CUR_18V_SHT)
#define MAX_CUR_18V_SET(x,y)            (x = ((x & ~MAX_CUR_18V_MASK) | (y << MAX_CUR_18V_SHT)))
#define MAX_CUR_30V_SHT                 (8)     //15:8
#define MAX_CUR_30V_MASK                (0xFFUL << MAX_CUR_30V_SHT)
#define MAX_CUR_30V_GET(x)              ((x & MAX_CUR_30V_MASK) >> MAX_CUR_30V_SHT)
#define MAX_CUR_30V_SET(x,y)            (x = ((x & ~MAX_CUR_30V_MASK) | (y << MAX_CUR_30V_SHT)))
#define MAX_CUR_33V_SHT                 (0)     //7:0
#define MAX_CUR_33V_MASK                (0xFFUL << MAX_CUR_33V_SHT)
#define MAX_CUR_33V_GET(x)              ((x & MAX_CUR_33V_MASK) >> MAX_CUR_33V_SHT)
#define MAX_CUR_33V_SET(x,y)            (x = ((x & ~MAX_CUR_33V_MASK) | (y << MAX_CUR_33V_SHT)))


//#define DWC51_CURR_CAPABILITIES2_R              (DWC51_BASE+0x04CUL)     //0x1801094C
//#define RSVD_63_40                  31:8
#define MAX_CUR_VDD2_18V_SHT            (0)     //7:0
#define MAX_CUR_VDD2_18V_MASK           (0xFFUL << MAX_CUR_VDD2_18V_SHT)
#define MAX_CUR_VDD2_18V_GET(x)         ((x & MAX_CUR_VDD2_18V_MASK) >> MAX_CUR_VDD2_18V_SHT)
#define MAX_CUR_VDD2_18V_SET(x,y)       (x = ((x & ~MAX_CUR_VDD2_18V_MASK) | (y << MAX_CUR_VDD2_18V_SHT)))


//#define DWC51_FORCE_AUTO_CMD_STAT_R             (DWC51_BASE+0x050UL)     //0x18010950 (2Bytes)
//#define RSVD_15_8                       15:8
#define FORCE_CMD_NOT_ISSUED_AUTO_CMD12 BIT(7)
//#define RSVD_6                          6
#define FORCE_AUTO_CMD_RESP_ERR         BIT(5)
#define FORCE_AUTO_CMD_IDX_ERR          BIT(4)
#define FORCE_AUTO_CMD_EBIT_ERR         BIT(3)
#define FORCE_AUTO_CMD_CRC_ERR          BIT(2)
#define FORCE_AUTO_CMD_TOUT_ERR         BIT(1)
#define FORCE_AUTO_CMD12_NOT_EXEC       BIT(0)


//#define DWC51_FORCE_ERROR_INT_STAT_R16            (DWC51_BASE+0x052UL)     //0x18010952 (2Bytes)
#define FORCE_VENDOR_ERR3               BIT(15)
#define FORCE_VENDOR_ERR2               BIT(14)
#define FORCE_VENDOR_ERR1               BIT(13)
#define FORCE_BOOT_ACK_ERR              BIT(12)
#define FORCE_RESP_ERR                  BIT(11)
#define FORCE_TUNING_ERR                BIT(10)
#define FORCE_ADMA_ERR                  BIT(9)
#define FORCE_AUTO_CMD_ERR              BIT(8)
#define FORCE_CUR_LMT_ERR               BIT(7)
#define FORCE_DATA_END_BIT_ERR          BIT(6)
#define FORCE_DATA_CRC_ERR              BIT(5)
#define FORCE_DATA_TOUT_ERR             BIT(4)
#define FORCE_CMD_IDX_ERR               BIT(3)
#define FORCE_CMD_END_BIT_ERR           BIT(2)
#define FORCE_CMD_CRC_ERR               BIT(1)
#define FORCE_CMD_TOUT_ERR              BIT(0)

//#define DWC51_ADMA_ERR_STAT_R8                   (DWC51_BASE+0x054UL)     //0x18010954
//#define RSVD_7_3                        7:3
#define ADMA_LEN_ERR                    BIT(2)
#define ADMA_ERR_STATES_SHT             (0)     //1:0
#define ADMA_ERR_STATES_MASK            (0x3UL << ADMA_ERR_STATES_SHT)
#define ADMA_ERR_STATES_GET(x)          ((x & ADMA_ERR_STATES_MASK) >> ADMA_ERR_STATES_SHT)
#define ADMA_ERR_STATES_SET(x,y)        (x = ((x & ~ADMA_ERR_STATES_MASK) | (y << ADMA_ERR_STATES_SHT)))

//#define DWC51_ADMA_SA_LOW_R                     (DWC51_BASE+0x058UL)     //0x18010958
//#define DWC51_ADMA_SA_HIGH_R                    (DWC51_BASE+0x05CUL)     //0x1801095C

//#define DWC51_PRESET_INIT_R                     (DWC51_BASE+0x060UL)     //0x18010960 (2Bytes)
//#define DWC51_PRESET_DS_R                       (DWC51_BASE+0x062UL)     //0x18010962 (2Bytes)
//#define DWC51_PRESET_HS_R                       (DWC51_BASE+0x064UL)     //0x18010964 (2Bytes)
//#define DWC51_PRESET_SDR12_R                    (DWC51_BASE+0x066UL)     //0x18010966 (2Bytes)
//#define DWC51_PRESET_SDR25_R                    (DWC51_BASE+0x068UL)     //0x18010968 (2Bytes)
//#define DWC51_PRESET_SDR50_R                    (DWC51_BASE+0x06AUL)     //0x1801096A (2Bytes)
//#define DWC51_PRESET_SDR104_R                   (DWC51_BASE+0x06CUL)     //0x1801096C (2Bytes)
//#define DWC51_PRESET_DDR50_R                    (DWC51_BASE+0x06EUL)     //0x1801096E (2Bytes)
//#define DWC51_PRESET_UHS2_R                     (DWC51_BASE+0x074UL)     //0x18010974 (2Bytes)
//The format of register from 0x18010960 to 0x18010974 are the same.
#define DRV_SEL_VAL_SHT                 (14)    //15:14
#define DRV_SEL_VAL_MASK                (0x3UL << DRV_SEL_VAL_SHT)
#define DRV_SEL_VAL_GET(x)              ((x & DRV_SEL_VAL_MASK) >> DRV_SEL_VAL_SHT)
#define DRV_SEL_VAL_SET(x,y)            (x = ((x & ~DRV_SEL_VAL_MASK) | (y << DRV_SEL_VAL_SHT)))
//#define RSVD_13_11              13:11
#define CLK_GEN_SEL_VAL                 BIT(10)
#define FREQ_SEL_VAL_SHT                (0)     //9:0
#define FREQ_SEL_VAL_MASK               (0x3FFUL << FREQ_SEL_VAL_SHT)
#define FREQ_SEL_VAL_GET(x)             ((x & FREQ_SEL_VAL_MASK) >> FREQ_SEL_VAL_SHT)
#define FREQ_SEL_VAL_SET(x,y)           (x = ((x & ~FREQ_SEL_VAL_MASK) | (y << FREQ_SEL_VAL_SHT)))


//#define DWC51_ADMA_ID_LOW_R                     (DWC51_BASE+0x078UL)     //0x18010978
//#define DWC51_ADMA_ID_HIGH_R                    (DWC51_BASE+0x07CUL)     //0x1801097C
//#define DWC51_UHS_II_BLOCK_SIZE_R               (DWC51_BASE+0x080UL)     //0x18010980
//#define RSVD_15                     15
#define UHS_II_SDMA_BOUND_SHT           (12)    //14:12
#define UHS_II_SDMA_BOUND_MASK          (0x7UL << UHS_II_SDMA_BOUND_SHT)
#define UHS_II_SDMA_BOUND_GET(x)        ((x & UHS_II_SDMA_BOUND_MASK) >> UHS_II_SDMA_BOUND_SHT)
#define UHS_II_SDMA_BOUND_SET(x,y)      (x = ((x & ~UHS_II_SDMA_BOUND_MASK) | (y << UHS_II_SDMA_BOUND_SHT)))
#define UHS_II_BLK_SIZE_SHT             (0)     //11:0
#define UHS_II_BLK_SIZE_MASK            (0xFFFUL << UHS_II_BLK_SIZE_SHT)
#define UHS_II_BLK_SIZE_GET(x)          ((x & UHS_II_BLK_SIZE_MASK) >> UHS_II_BLK_SIZE_SHT)
#define UHS_II_BLK_SIZE_SET(x,y)        (x = ((x & ~UHS_II_BLK_SIZE_MASK) | (y << UHS_II_BLK_SIZE_SHT)))


//#define DWC51_UHS_II_BLOCK_COUNT_R              (DWC51_BASE+0x084UL)     //0x18010984
//#define DWC51_UHS_II_COMMAND_PKT_0_3_R          (DWC51_BASE+0x088UL)     //0x18010988
//#define DWC51_UHS_II_COMMAND_PKT_4_7_R          (DWC51_BASE+0x08CUL)     //0x1801098C
//#define DWC51_UHS_II_COMMAND_PKT_8_11_R         (DWC51_BASE+0x090UL)     //0x18010990
//#define DWC51_UHS_II_COMMAND_PKT_12_15_R        (DWC51_BASE+0x094UL)     //0x18010994
//#define DWC51_UHS_II_COMMAND_PKT_16_19_R        (DWC51_BASE+0x098UL)     //0x18010998
//#define DWC51_UHS_II_XFER_MODE_R                (DWC51_BASE+0x09CUL)     //0x1801099C
#define HALF_FULL_SEL                   BIT(15)
#define EBSY_WAIT                       BIT(14)
//#define RESERVED_13_9           13:9
#define RESP_INTR_DISABLE               BIT(8)
#define RESP_ERR_CHK_EN                 BIT(7)
#define RESP_TYP                        BIT(6)
#define BLK_BYTE_MODE                   BIT(5)
#define DATA_XFER_DIR                   BIT(4)
//#define RESERVED_3_2            3:2
#define BLK_CNT_EN                      BIT(1)
#define DMA_EN                          BIT(0)


//#define DWC51_UHS_II_CMD_R                      (DWC51_BASE+0x09EUL)     //0x1801099E
//#define RESERVED_15_13              15:13
#define UHS_II_CMD_PKT_LEN_SHT          (8)     //12:8
#define UHS_II_CMD_PKT_LEN_MASK         (0x1FUL << UHS_II_CMD_PKT_LEN_SHT)
#define UHS_II_CMD_PKT_LEN_GET(x)       ((x & UHS_II_CMD_PKT_LEN_MASK) >> UHS_II_CMD_PKT_LEN_SHT)
#define UHS_II_CMD_PKT_LEN_SET(x,y)     (x = ((x & ~UHS_II_CMD_PKT_LEN_MASK) | (y << UHS_II_CMD_PKT_LEN_SHT)))
#define UHS_II_CMD_TYP_SHT              (6)     //7:6
#define UHS_II_CMD_TYP_MASK             (0x3UL << UHS_II_CMD_TYP_SHT)
#define UHS_II_CMD_TYP_GET(x)           ((x & UHS_II_CMD_TYP_MASK) >> UHS_II_CMD_TYP_SHT)
#define UHS_II_CMD_TYP_SET(x,y)         (x = ((x & ~UHS_II_CMD_TYP_MASK) | (y << UHS_II_CMD_TYP_SHT)))
#define DATA_PRESENT                    BIT(5)
//#define RESERVED_4_3                4:3
#define SUB_CMD_FLAG                    BIT(2)
//#define RESERVED_1_0                1:0


//#define DWC51_UHS_II_RESP_0_3_R                 (DWC51_BASE+0x0A0UL)     //0x180109A0
//#define DWC51_UHS_II_RESP_4_7_R                 (DWC51_BASE+0x0A4UL)     //0x180109A4
//#define DWC51_UHS_II_RESP_8_11_R                (DWC51_BASE+0x0A8UL)     //0x180109A8
//#define DWC51_UHS_II_RESP_12_15_R               (DWC51_BASE+0x0ACUL)     //0x180109AC
//#define DWC51_UHS_II_RESP_16_19_R               (DWC51_BASE+0x0B0UL)     //0x180109B0
//#define DWC51_UHS_II_MSG_SEL_R                  (DWC51_BASE+0x0B4UL)     //0x180109B4
//#define RESERVED_7_2            7:2
#define UHS_II_MSG_SEL_SHT              (0)     //1:0
#define UHS_II_MSG_SEL_MASK             (0x3UL << UHS_II_MSG_SEL_SHT)
#define UHS_II_MSG_SEL_GET(x)           ((x & UHS_II_MSG_SEL_MASK) >> UHS_II_MSG_SEL_SHT)
#define UHS_II_MSG_SEL_SET(x,y)         (x = ((x & ~UHS_II_MSG_SEL_MASK) | (y << UHS_II_MSG_SEL_SHT)))


//#define DWC51_UHS_II_MSG_R                      (DWC51_BASE+0x0B8UL)     //0x180109B8
//#define DWC51_UHS_II_DEV_INTR_STATUS_R          (DWC51_BASE+0x0BCUL)     //0x180109BC (2Bytes)
#define DEV_INTR_STATUS15               BIT(15)
#define DEV_INTR_STATUS14               BIT(14)
#define DEV_INTR_STATUS13               BIT(13)
#define DEV_INTR_STATUS12               BIT(12)
#define DEV_INTR_STATUS11               BIT(11)
#define DEV_INTR_STATUS10               BIT(10)
#define DEV_INTR_STATUS9                BIT(9)
#define DEV_INTR_STATUS8                BIT(8)
#define DEV_INTR_STATUS7                BIT(7)
#define DEV_INTR_STATUS6                BIT(6)
#define DEV_INTR_STATUS5                BIT(5)
#define DEV_INTR_STATUS4                BIT(4)
#define DEV_INTR_STATUS3                BIT(3)
#define DEV_INTR_STATUS2                BIT(2)
#define DEV_INTR_STATUS1                BIT(1)
#define DEV_INTR_STATUS0                BIT(0)


//#define DWC51_UHS_II_DEV_SEL_R                  (DWC51_BASE+0x0BEUL)     //0x180109BE (2Bytes)
#define INT_MSG_EN                      BIT(7)
//#define RESERVED_6_4            6:4
#define DEV_SEL_SHT                     (0)     //3:0
#define DEV_SEL_MASK                    (0xFUL << DEV_SEL_SHT)
#define DEV_SEL_GET(x)                  ((x & DEV_SEL_MASK) >> DEV_SEL_SHT)
#define DEV_SEL_SET(x,y)                (x = ((x & ~DEV_SEL_MASK) | (y << DEV_SEL_SHT)))


//#define DWC51_UHS_II_DEV_INR_CODE_R             (DWC51_BASE+0x0BFUL)     //0x180109BF
//#define DWC51_UHS_II_SOFT_RESET_R               (DWC51_BASE+0x0C0UL)     //0x180109C0 (2Bytes)
//#define RESERVED_15_2           15:2
#define HOST_SD_TRAN_RST                BIT(1)
#define HOST_FULL_RST                   BIT(0)


//#define DWC51_UHS_II_TIMER_CNTRL_R              (DWC51_BASE+0x0C2UL)     //0x180109C2 (2Bytes)
//#define RESERVED_15_8               15:8
#define TIMER_CNT_DEADLOCK_SHT          (4)     //7:4
#define TIMER_CNT_DEADLOCK_MASK         (0xFUL << TIMER_CNT_DEADLOCK_SHT)
#define TIMER_CNT_DEADLOCK_GET(x)       ((x & TIMER_CNT_DEADLOCK_MASK) >> TIMER_CNT_DEADLOCK_SHT)
#define TIMER_CNT_DEADLOCK_SET(x,y)     (x = ((x & ~TIMER_CNT_DEADLOCK_MASK) | (y << TIMER_CNT_DEADLOCK_SHT)))
#define TIMEOUT_CNT_CMD_RES_SHT         (0)     //3:0
#define TIMEOUT_CNT_CMD_RES_MASK        (0xFUL << TIMEOUT_CNT_CMD_RES_SHT)
#define TIMEOUT_CNT_CMD_RES_GET(x)      ((x & TIMEOUT_CNT_CMD_RES_MASK) >> TIMEOUT_CNT_CMD_RES_SHT)
#define TIMEOUT_CNT_CMD_RES_SET(x,y)    (x = ((x & ~TIMEOUT_CNT_CMD_RES_MASK) | (y << TIMEOUT_CNT_CMD_RES_SHT)))


//#define DWC51_UHS_II_ERR_INTR_STATUS_R          (DWC51_BASE+0x0C4UL)     //0x180109C4
//#define DWC51_UHS_II_ERR_INTR_STATUS_EN_R       (DWC51_BASE+0x0C8UL)     //0x180109C8
//#define DWC51_UHS_II_ERR_INTR_SIGNAL_EN_R       (DWC51_BASE+0x0CCUL)     //0x180109CC
// The format of register from 0x180109C4 to 0x180109CC are the same.
#define VENDOR_SPECIFIC_ERR4            BIT(31)
#define VENDOR_SPECIFIC_ERR3            BIT(30)
#define VENDOR_SPECIFIC_ERR2            BIT(29)
#define VENDOR_SPECIFIC_ERR1            BIT(28)
#define VENDOR_SPECIFIC_ERR0            BIT(27)
//#define RESERVED_26_18              26:18
#define TIMEOUT_DEADLOCK                BIT(17)
#define TIMEOUT_CMD_RES                 BIT(16)
#define ADMA_ERR_UHS_II_ERR             BIT(15)
//#define RESERVED_14_9               14:9
#define EBSY_ERR                        BIT(8 )
#define UNRECOVERABLE_ERR               BIT(7 )
//#define RESERVED_6                  6
#define TID_ERR                         BIT(5)
#define FRAMING_ERR                     BIT(4)
#define CRC_ERR                         BIT(3)
#define RETRY_EXPIRED                   BIT(2)
#define RES_PKT_ERR                     BIT(1)
#define HEADER_ERR                      BIT(0)


//#define DWC51_P_UHS_II_SETTINGS_R               (DWC51_BASE+0x0E0UL)     //0x180109E0 (2Bytes)
//#define DWC51_P_UHS_II_HOST_CAPAB               (DWC51_BASE+0x0E2UL)     //0x180109E2 (2Bytes)
//#define DWC51_P_UHS_II_TEST                     (DWC51_BASE+0x0E4UL)     //0x180109E4 (2Bytes)
//#define DWC51_P_EMBEDDED_CNTRL                  (DWC51_BASE+0x0E6UL)     //0x180109E6 (2Bytes)
//#define DWC51_P_VENDOR_SPECIFIC_AREA            (DWC51_BASE+0x0E8UL)     //0x180109E8 (2Bytes)
//#define DWC51_P_VENDOR2_SPECIFIC_AREA           (DWC51_BASE+0x0EAUL)     //0x180109EA (2Bytes)
//#define DWC51_SLOT_INTR_STATUS_R                (DWC51_BASE+0x0FCUL)     //0x180109FC (2Bytes)
#define RESERVED_15_8_SHT               (8)     //15:8
#define RESERVED_15_8_MASK              (0xFFUL << RESERVED_15_8_SHT)
#define RESERVED_15_8_GET(x)            ((x & RESERVED_15_8_MASK) >> RESERVED_15_8_SHT)
#define RESERVED_15_8_SET(x,y)          (x = ((x & ~RESERVED_15_8_MASK) | (y << RESERVED_15_8_SHT)))
#define INTR_SLOT_SHT                   (0)     //7:0
#define INTR_SLOT_MASK                  (0xFFUL << INTR_SLOT_SHT)
#define INTR_SLOT_GET(x)                ((x & INTR_SLOT_MASK) >> INTR_SLOT_SHT)
#define INTR_SLOT_SET(x,y)              (x = ((x & ~INTR_SLOT_MASK) | (y << INTR_SLOT_SHT)))


//#define DWC51_HOST_CNTRL_VERS_R                 (DWC51_BASE+0x0FEUL)     //0x180109FE (2Bytes)
#define VENDOR_VERSION_NUM_SHT          (8)     //15:8
#define VENDOR_VERSION_NUM_MASK         (0xFFUL << VENDOR_VERSION_NUM_SHT)
#define VENDOR_VERSION_NUM_GET(x)       ((x & VENDOR_VERSION_NUM_MASK) >> VENDOR_VERSION_NUM_SHT)
#define VENDOR_VERSION_NUM_SET(x,y)     (x = ((x & ~VENDOR_VERSION_NUM_MASK) | (y << VENDOR_VERSION_NUM_SHT)))
#define SPEC_VERSION_NUM_SHT            (0)    //7:0
#define SPEC_VERSION_NUM_MASK           (0xFFUL << SPEC_VERSION_NUM_SHT)
#define SPEC_VERSION_NUM_GET(x)         ((x & SPEC_VERSION_NUM_MASK) >> SPEC_VERSION_NUM_SHT)
#define SPEC_VERSION_NUM_SET(x,y)       (x = ((x & ~SPEC_VERSION_NUM_MASK) | (y << SPEC_VERSION_NUM_SHT)))


/* ============================================= */
/* DWC_mshc_vendor1_block_rigisters 0x100~0x148  */
/* ============================================= */
//#define DWC51_MSHC_VER_ID_R     (DWC51_BASE+0x100+0x00UL)   //0x18010A00
//#define DWC51_MSHC_VER_TYPE_R   (DWC51_BASE+0x100+0x04UL)   //0x18010A04
//#define DWC51_MSHC_CTRL_R       (DWC51_BASE+0x100+0x08UL)   //0x18010A08
#define NEDGE_SMPL_EN                   BIT(7)
#define PEDGE_DRV_EN                    BIT(6)
#define SLV_ERR_RESP_NONEXIS_REG        BIT(5)
#define SW_CG_DIS                       BIT(4)
//#define RSVD1                   3:2
#define NEGEDGE_DATAOUT_EN              BIT(1)
#define CMD_CONFLICT_CHECK              BIT(0)


//#define DWC51_MBIU_CTRL_R       (DWC51_BASE+0x100+0x10UL)   //0x18010A10
//#define RSVD                    7:4
#define BURST_INCR16_EN                 BIT(3)
#define BURST_INCR8_EN                  BIT(2)
#define BURST_INCR4_EN                  BIT(1)
#define UNDEFL_INCR_EN                  BIT(0)


//#define DWC51_EMMC_CTRL_R16       (DWC51_BASE+0x100+0x2CUL)   //0x18010A2C (2Bytes)
//#define RSVD                    15:11
#define CQE_PREFETCH_DISABLE            BIT(10)
#define CQE_ALGO_SEL(x)                 BIT(9)
#define ENH_STROBE_ENABLE(x)            BIT(8)
//#define Rsvd                    7:4
#define EMMC_RST_N_OE                   BIT(3)
#define EMMC_RST_N                      BIT(2)
#define DISABLE_DATA_CRC_CHK            BIT(1)
#define CARD_IS_EMMC                    BIT(0)


//#define DWC51_BOOT_CTRL_R16       (DWC51_BASE+0x100+0x2EUL)   //0x18010A2E (2Bytes)
#define BOOT_TOUT_CNT_SHT               (12)    //15:12
#define BOOT_TOUT_CNT_MASK              (0xFUL << BOOT_TOUT_CNT_SHT)
#define BOOT_TOUT_CNT_GET(x)            ((x & BOOT_TOUT_CNT_MASK) >> BOOT_TOUT_CNT_SHT)
#define BOOT_TOUT_CNT_SET(x,y)          (x = ((x & ~BOOT_TOUT_CNT_MASK) | (y << BOOT_TOUT_CNT_SHT)))
//#define RSVD_11_9               11:9
#define BOOT_ACK_ENABLE                 BIT(8)
#define VALIDATE_BOOT                   BIT(7)
//#define RSVD_6_1                6:1
#define MAN_BOOT_EN                     BIT(0)


//#define DWC51_GP_IN_R           (DWC51_BASE+0x100+0x30UL)   //0x18010A30
//#define DWC51_GP_OUT_R          (DWC51_BASE+0x100+0x34UL)   //0x18010A34
//#define DWC51_AT_CTRL_R         (DWC51_BASE+0x100+0x40UL)   //0x18010A40
#define SWIN_TH_VAL_SHT                 (24)    //31:24
#define SWIN_TH_VAL_MASK                (0xFFUL << SWIN_TH_VAL_SHT)
#define SWIN_TH_VAL_GET(x)              ((x & SWIN_TH_VAL_MASK) >> SWIN_TH_VAL_SHT)
#define SWIN_TH_VAL_SET(x,y)            (x = ((x & ~SWIN_TH_VAL_MASK) | (y << SWIN_TH_VAL_SHT)))
//#define Rsvd                        23:21
#define POST_CHANGE_DLY_SHT             (19)    //20:19
#define POST_CHANGE_DLY_MASK            (0x3UL << POST_CHANGE_DLY_SHT)
#define POST_CHANGE_DLY_GET(x)          ((x & POST_CHANGE_DLY_MASK) >> POST_CHANGE_DLY_SHT)
#define POST_CHANGE_DLY_SET(x,y)        (x = ((x & ~POST_CHANGE_DLY_MASK) | (y << POST_CHANGE_DLY_SHT)))
#define PRE_CHANGE_DLY_SHT              (17)    //18:17
#define PRE_CHANGE_DLY_MASK             (0x3UL << PRE_CHANGE_DLY_SHT)
#define PRE_CHANGE_DLY_GET(x)           ((x & PRE_CHANGE_DLY_MASK) >> PRE_CHANGE_DLY_SHT)
#define PRE_CHANGE_DLY_SET(x,y)         (x = ((x & ~PRE_CHANGE_DLY_MASK) | (y << PRE_CHANGE_DLY_SHT)))
#define TUNE_CLK_STOP_EN                BIT(16)
//#define RSDV3                   15:12
#define WIN_EDGE_SEL_SHT                (8)     //11:8
#define WIN_EDGE_SEL_MASK               (0xFUL << WIN_EDGE_SEL_SHT)
#define WIN_EDGE_SEL_GET(x)             ((x & WIN_EDGE_SEL_MASK) >> WIN_EDGE_SEL_SHT)
#define WIN_EDGE_SEL_SET(x,y)           (x = ((x & ~WIN_EDGE_SEL_MASK) | (y << WIN_EDGE_SEL_SHT)))
//#define RSDV2                       7:5
#define SW_TUNE_EN                      BIT(4)
#define RPT_TUNE_ERR                    BIT(3)
#define SWIN_TH_EN                      BIT(2)
#define CI_SEL                          BIT(1)
#define AT_EN                           BIT(0)


//#define DWC51_AT_STAT_R         (DWC51_BASE+0x100+0x44UL)   //0x18010A44
//#define RSDV1                   31:24
#define L_EDGE_PH_CODE_SHT              (16)    //23:16
#define L_EDGE_PH_CODE_MASK             (0xFFUL << L_EDGE_PH_CODE_SHT)
#define L_EDGE_PH_CODE_GET(x)           ((x & L_EDGE_PH_CODE_MASK) >> L_EDGE_PH_CODE_SHT)
#define L_EDGE_PH_CODE_SET(x,y)         (x = ((x & ~L_EDGE_PH_CODE_MASK) | (y << L_EDGE_PH_CODE_SHT)))
#define R_EDGE_PH_CODE_SHT              (8)     //15:8
#define R_EDGE_PH_CODE_MASK             (0xFFUL << R_EDGE_PH_CODE_SHT)
#define R_EDGE_PH_CODE_GET(x)           ((x & R_EDGE_PH_CODE_MASK) >> R_EDGE_PH_CODE_SHT)
#define R_EDGE_PH_CODE_SET(x,y)         (x = ((x & ~R_EDGE_PH_CODE_MASK) | (y << R_EDGE_PH_CODE_SHT)))
#define CENTER_PH_CODE_SHT              (0)     //7:0
#define CENTER_PH_CODE_MASK             (0xFFUL << CENTER_PH_CODE_SHT)
#define CENTER_PH_CODE_GET(x)           ((x & CENTER_PH_CODE_MASK) >> CENTER_PH_CODE_SHT)
#define CENTER_PH_CODE_SET(x,y)         (x = ((x & ~CENTER_PH_CODE_MASK) | (y << CENTER_PH_CODE_SHT)))


/* ============================================= */
/* DWC_mshc_vendor2_block_rigisters 0x180~0x1DF  */
/* ============================================= */
//#define DWC51_CQVER             (DWC51_BASE+0x180+0x00UL)   //0x18010A80
//#define EMMMC_VER_RSVD          31:12
#define EMMC_VER_MAJOR_SHT              (8)     //11:8
#define EMMC_VER_MAJOR_MASK             (0xFUL << EMMC_VER_MAJOR_SHT)
#define EMMC_VER_MAJOR_GET(x)           ((x & EMMC_VER_MAJOR_MASK) >> EMMC_VER_MAJOR_SHT)
#define EMMC_VER_MAJOR_SET(x,y)         (x = ((x & ~EMMC_VER_MAJOR_MASK) | (y << EMMC_VER_MAJOR_SHT)))
#define EMMC_VER_MINOR_SHT              (4)     //7:4
#define EMMC_VER_MINOR_MASK             (0xFUL << EMMC_VER_MINOR_SHT)
#define EMMC_VER_MINOR_GET(x)           ((x & EMMC_VER_MINOR_MASK) >> EMMC_VER_MINOR_SHT)
#define EMMC_VER_MINOR_SET(x,y)         (x = ((x & ~EMMC_VER_MINOR_MASK) | (y << EMMC_VER_MINOR_SHT)))
#define EMMC_VER_SUFFIX_SHT             (0)     // 3:0
#define EMMC_VER_SUFFIX_MASK            (0xFUL << EMMC_VER_SUFFIX_SHT)
#define EMMC_VER_SUFFIX_GET(x)          ((x & EMMC_VER_SUFFIX_MASK) >> EMMC_VER_SUFFIX_SHT)
#define EMMC_VER_SUFFIX_SET(x,y)        ((x & ~EMMC_VER_SUFFIX_MASK) | (y << EMMC_VER_SUFFIX_SHT))


//#define DWC51_CQCAP             (DWC51_BASE+0x180+0x04UL)   //0x18010A84
//#define CQCCAP_RSVD3            31:29
#define CRYPTO_SUPPORT                  BIT(28)
//#define CQCCAP_RSVD2            27:16
#define ITCFMUL_SHT                     (12)    //15:12
#define ITCFMUL_MASK                    (0xFUL << ITCFMUL_SHT)
#define ITCFMUL_GET(x)                  ((x & ITCFMUL_MASK) >> ITCFMUL_SHT)
#define ITCFMUL_SET(x,y)                (x = ((x & ~ITCFMUL_MASK) | (y << ITCFMUL_SHT)))
//#define CQCCAP_RSVD1            11:10
#define ITCFVAL_SHT                     (0)     //9:0
#define ITCFVAL_MASK                    (0x3FFUL << ITCFVAL_SHT)
#define ITCFVAL_GET(x)                  ((x & ITCFVAL_MASK) >> ITCFVAL_SHT)
#define ITCFVAL_SET(x,y)                (x = ((x & ~ITCFVAL_MASK) | (y << ITCFVAL_SHT)))


//#define DWC51_CQCFG             (DWC51_BASE+0x180+0x08UL)   //0x18010A88
//#define CQCCFG_RSVD3            31:13
#define DCMD_EN                         BIT(12)
//#define CQCCFG_RSVD2            11:9
#define TASK_DESC_SIZE                  BIT(8)
//#define CQCCFG_RSVD1            7:2
#define CR_GENERAL_EN                   BIT(1)
#define CQ_EN                           BIT(0)


//#define DWC51_CQCTL             (DWC51_BASE+0x180+0x0cUL)   //0x18010A8C
//#define CQCTL_RSVD2             31:9
#define CLR_ALL_TASKS                   BIT(8)
//#define CQCTL_RSVD1             7:1
#define HALT                            BIT(0)


//#define DWC51_CQIS              (DWC51_BASE+0x180+0x10UL)   //0x18010A90
//#define DWC51_CQISE             (DWC51_BASE+0x180+0x14UL)   //0x18010A94
//#define DWC51_CQISGE            (DWC51_BASE+0x180+0x18UL)   //0x18010A98
//#define CQISGE_RSVD1            31:6
#define CQIS_ICCE                            BIT(5)
#define CQIS_GCE                             BIT(4)
#define CQIS_TCL                             BIT(3)
#define CQIS_RED                             BIT(2)
#define CQIS_TCC                             BIT(1)
#define CQIS_HAC                             BIT(0)
#define CMDQ_ALL_INT_EVENT              (CQIS_ICCE|CQIS_GCE|CQIS_TCL|CQIS_RED|CQIS_TCC|CQIS_HAC)

//#define DWC51_CQIC              (DWC51_BASE+0x180+0x1cUL)   //0x18010A9C
#define INTC_EN                         BIT(31)
//#define CQIC_RSVD3              30:21
#define INTC_STAT                       BIT(20)
//#define CQIC_RSVD2              19:17
#define INTC_RST                        BIT(16)
#define INTC_TH_WEN                     BIT(15)
//#define CQIC_RSVD1              14:13
#define INTC_TH_SHT                     (8)     //12:8
#define INTC_TH_MASK                    (0x1FUL << INTC_TH_SHT)
#define INTC_TH_GET(x)                  ((x & INTC_TH_MASK) >> INTC_TH_SHT)
#define INTC_TH_SET(x,y)                (x = ((x & ~INTC_TH_MASK) | (y << INTC_TH_SHT)))
#define TOUT_VAL_WEN                    BIT(7)
#define TOUT_VAL_SHT                    (0)     //6:0
#define TOUT_VAL_MASK                   (0x7FUL << TOUT_VAL_SHT)
#define TOUT_VAL_GET(x)                 ((x & TOUT_VAL_MASK) >> TOUT_VAL_SHT)
#define TOUT_VAL_SET(x,y)               (x = ((x & ~TOUT_VAL_MASK) | (y << TOUT_VAL_SHT)))


//#define DWC51_CQTDLBA           (DWC51_BASE+0x180+0x20UL)   //0x18010AA0
//#define DWC51_CQTDLBAU          (DWC51_BASE+0x180+0x24UL)   //0x18010AA4
//#define DWC51_CQTDBR            (DWC51_BASE+0x180+0x28UL)   //0x18010AA8
//#define DWC51_CQTCN             (DWC51_BASE+0x180+0x2cUL)   //0x18010AAC
//#define DWC51_CQDQS             (DWC51_BASE+0x180+0x30UL)   //0x18010AB0
//#define DWC51_CQDPT             (DWC51_BASE+0x180+0x34UL)   //0x18010AB4
//#define DWC51_CQTCLR            (DWC51_BASE+0x180+0x38UL)   //0x18010AB8
//#define DWC51_CQSSC1            (DWC51_BASE+0x180+0x40UL)   //0x18010AC0
//RSVD_20_31              31:20
#define SQSCMD_BLK_CNT_SHT              (16)    //19:16
#define SQSCMD_BLK_CNT_MASK             (0xFUL << TRANS_ERR_TASKID_SHT)
#define SQSCMD_BLK_CNT_GET(x)           ((x & TRANS_ERR_TASKID_MASK) >> TRANS_ERR_TASKID_SHT)
#define SQSCMD_BLK_CNT_SET(x,y)         (x = ((x & ~TRANS_ERR_TASKID_MASK) | (y << TRANS_ERR_TASKID_SHT)))
#define SQSCMD_IDLE_TMR_SHT             (0)     //15:0
#define SQSCMD_IDLE_TMR_MASK            (0xFFFFUL << SQSCMD_IDLE_TMR_SHT)
#define SQSCMD_IDLE_TMR_GET(x)          ((x & SQSCMD_IDLE_TMR_MASK) >> SQSCMD_IDLE_TMR_SHT)
#define SQSCMD_IDLE_TMR_SET(x,y)        (x = ((x & ~SQSCMD_IDLE_TMR_MASK) | (y << SQSCMD_IDLE_TMR_SHT)))


//#define DWC51_CQSSC2            (DWC51_BASE+0x180+0x44UL)   //0x18010AC4
//#define RSVD_16_31              31:16
#define SQSCMD_RCA_SHT                  (0)     //15:0
#define SQSCMD_RCA_MASK                 (0xFFFFUL << SQSCMD_RCA_SHT)
#define SQSCMD_RCA_GET(x)               ((x & SQSCMD_RCA_MASK) >> SQSCMD_RCA_SHT)
#define SQSCMD_RCA_SET(x,y)             (x = ((x & ~SQSCMD_RCA_MASK) | (y << SQSCMD_RCA_SHT)))


//#define DWC51_CQCRDCT           (DWC51_BASE+0x180+0x48UL)   //0x18010AC8
//#define DWC51_CQRMEM            (DWC51_BASE+0x180+0x50UL)   //0x18010AD0
//#define DWC51_CQTERRI           (DWC51_BASE+0x180+0x54UL)   //0x18010AD4
#define TRANS_ERR_FIELDS_VALID          BIT(31)
#define TRANS_ERR_TASKID_SHT            (24)    //28:24
#define TRANS_ERR_TASKID_MASK           (0x1FUL << TRANS_ERR_TASKID_SHT)
#define TRANS_ERR_TASKID_GET(x)         ((x & TRANS_ERR_TASKID_MASK) >> TRANS_ERR_TASKID_SHT)
#define TRANS_ERR_TASKID_SET(x,y)       (x = ((x & ~TRANS_ERR_TASKID_MASK) | (y << TRANS_ERR_TASKID_SHT)))
#define TRANS_ERR_CMD_INDX_SHT          (16)    //21:16
#define TRANS_ERR_CMD_INDX_MASK         (0x3FUL << TRANS_ERR_CMD_INDX_SHT)
#define TRANS_ERR_CMD_INDX_GET(x)       ((x & TRANS_ERR_CMD_INDX_MASK) >> TRANS_ERR_CMD_INDX_SHT)
#define TRANS_ERR_CMD_INDX_SET(x,y)     (x = ((x & ~TRANS_ERR_CMD_INDX_MASK) | (y << TRANS_ERR_CMD_INDX_SHT)))
#define RESP_ERR_FIELDS_VALID           BIT(15)
#define RESP_ERR_TASKID_SHT             (8)     //12:8
#define RESP_ERR_TASKID_MASK            (0x1FUL << RESP_ERR_TASKID_SHT)
#define RESP_ERR_TASKID_GET(x)          ((x & RESP_ERR_TASKID_MASK) >> RESP_ERR_TASKID_SHT)
#define RESP_ERR_TASKID_SET(x,y)        (x = ((x & ~RESP_ERR_TASKID_MASK) | (y << RESP_ERR_TASKID_SHT)))
#define RESP_ERR_CMD_INDX_SHT           (0)     //5:0
#define RESP_ERR_CMD_INDX_MASK          (0x3FUL << RESP_ERR_CMD_INDX_SHT)
#define RESP_ERR_CMD_INDX_GET(x)        ((x & RESP_ERR_CMD_INDX_MASK) >> RESP_ERR_CMD_INDX_SHT)
#define RESP_ERR_CMD_INDX_SET(x,y)      (x = ((x & ~RESP_ERR_CMD_INDX_MASK) | (y << RESP_ERR_CMD_INDX_SHT)))


//#define DWC51_CQCRI             (DWC51_BASE+0x180+0x58UL)   //0x18010AD8
#define CMD_RESP_INDX_SHT               (0)     //5:0
#define CMD_RESP_INDX_MASK              (0x3FUL << CMD_RESP_INDX_SHT)
#define CMD_RESP_INDX_GET(x)            ((x & CMD_RESP_INDX_MASK) >> CMD_RESP_INDX_SHT)
#define CMD_RESP_INDX_SET(x,y)          (x = ((x & ~CMD_RESP_INDX_MASK) | (y << CMD_RESP_INDX_SHT)))


//#define DWC51_CQCRA             (DWC51_BASE+0x180+0x5cUL)   //0x18010ADC

#endif  //#define _EMMC51_REG_H_

