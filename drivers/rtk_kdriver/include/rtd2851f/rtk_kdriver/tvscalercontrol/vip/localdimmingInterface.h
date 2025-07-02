/* 20201217 Local Dimming Interface header file*/
#ifndef __LOCALDIMMINGINTERFACE_H__
#define __LOCALDIMMINGINTERFACE_H__


#ifdef __cplusplus
extern "C" {
#endif
/* some include about scaler*/
#include <scaler/vipCommon.h>
#define DEBUG_LDINTERFACE 0
#include <base_types.h>


/*NOT in table*/
typedef struct {
        UINT32 send_trigger;
} DRV_LDINTERFACE_Trigger;

/*Use Calculate :NOT in table*/
typedef struct {
        UINT32 cs_hold_time;
        UINT32 sck_hold_time;
        UINT32 sck_h;
        UINT32 sck_l;
        UINT32 data_send_delay;
        UINT32 each_unit_delay;
        UINT32 multi_unit_delay;
        UINT16 vsync_d;
        UINT16 cs_end_delay_time;
} DRV_LDINTERFACE_TimingReg;

/*Read Only:Status /Debug ---NOT in Table (rtd2851f[mac8p] is one port only)*/
typedef struct {
        UINT16 rx_done;
        UINT16 rx_time_out_pending;
        UINT16 tx_done;
        UINT16 cmd_error_status;
        UINT16 ld_timing_error;
        UINT16 ld_spi_timing_Error;
} DRV_LDINTERFACE_Status;

/*Sram RW Settings---NOT in Table*/
typedef struct {
        UINT16 data_rw_pointer;
        UINT16 data_rw_pointer_set ;
        UINT16 data_rw_data_port;
        UINT16 packet_in_hw_mask;
} DRV_LDINTERFACE_SRam_Debug;

/*Sram format ----NOT in table*/
/*sram: data part -->separate part: header&tailer*/
typedef struct {
        UINT16 h_region;
        UINT16 v_region;
} DRV_LDINTERFACE_SRam_SeperateFormat;


typedef struct {
	unsigned short SRAM_Position;
	unsigned short SRAM_Length;
	unsigned short *SRAM_Value;
} DRV_LD_LDSPI_INDEXSRAM_TYPE;
typedef enum {
        POWER_PROTECTION_MODE = 0,	// high initial current
        BOOST_MODE,	// low initial current
} Boost_Mode ;

/**
 * @brief load all settings
 */

extern void drvif_LdInterface_Test(void);

/**
 * @brief load all settings
 */
void drvif_set_LdInterface_Table(DRV_LDINTERFACE_Table *ptr);
void drvif_set_LdInterface_ScanMode(UINT8 enable);
unsigned char drvif_get_LdInterface_ScanMode(void);
/**
 * @brief set Ldspi enable
 */
void drvif_set_LdInterface_Enable(UINT8 enable);
/**
 * @brief ldspi start en/disable : for change settings need disable ldspi.
 */
void drvif_set_LdInterface_StartEn(UINT8 enable);
/**
 * @brief when trigger mode, send trigger
 */
void drvif_set_LdInterface_SendTrigger(void);

/**
 * @brief set Ldspi basic info
 */
void drvif_set_LdInterface_BasicCtrl(DRV_LDINTERFACE_Basic_Ctrl *ptr);
void drvif_get_LdInterface_BasicCtrl(DRV_LDINTERFACE_Basic_Ctrl *ptr);
void drvif_set_LdInterface_LdMode(UINT8 ld_mode);
void drvif_set_LdInterface_Output(DRV_LDINTERFACE_Output *ptr);//input is real output unit
void drvif_set_LdInterface_DataUnits(UINT32 data_unit);//input is real output unit
/**
 * @brief Controls about SPI read. Default is TX :froce tx mode
 */
void drvif_set_LdInterface_shift_swap(DRV_LDINTERFACE_SHIFT_SWAP *ptr);

void drvif_set_LdInterface_Read(DRV_LDINTERFACE_Read *ptr);

void drvif_set_LdInterface_Inv(DRV_LDINTERFACE_INV *ptr);

void drvif_set_LdInterface_Repeat(DRV_LDINTERFACE_Repeat *ptr);

void drvif_set_LdInterface_Reproduce(DRV_LDINTERFACE_Reproduce *ptr);

void drvif_set_LdInterface_IntEn(DRV_LDINTERFACE_IntEn *ptr);

void drvif_set_LdInterface_TimingReal(DRV_LDINTERFACE_TimingReal *ptr);
void drvif_set_LdInterface_TimingReg(DRV_LDINTERFACE_TimingReg *ptrReg);


/**
 * @brief Clear rx fifo. Do rx clear after finish get rx datas.
 * include rx_done clear
 */
void drvif_LdInterface_Read_Clear(void);
/**
 * @brief Clear status.
 */
void drvif_LdInterface_Status_Clear(void);

/**
 * @brief print current info
 */
void   drvif_get_LdInterface_Table(DRV_LDINTERFACE_Table *ptr); //cal Reg 2 N0~N9
 /**
 * @brief when trigger mode, check if trigger cleared:
     * @return 1:enable 0:disable
 */
UINT8  drvif_get_LdInterface_Enable(void);
/**
 * @brief when trigger mode, check if trigger cleared:
     * @return 1:cleared 0:unClear
 */
UINT8  drvif_get_LdInterface_CheckTriggerClear(void);
/**
 * @brief: check rx done status: for recrive rx units
     * @return 1:rx_done   0:no rx received
 */
UINT8 drvif_get_LdInterface_rxdone(void);

/**
 * @brief get rx units:
     * @return rx unit infos
 */
UINT8*  drvif_get_LdInterface_RxUnits(int rxcnt);

void drvif_get_LdInterface_Output(DRV_LDINTERFACE_Output *ptrReg);
void drvif_get_LdInterface_TimingReg(DRV_LDINTERFACE_TimingReg *ptrReg);
void drvif_get_LdInterface_TimingReal(DRV_LDINTERFACE_TimingReal *ptrReal);

/*Calculate Part: Timing*/
void drvif_cal_LdInterface_TimingReg( DRV_LDINTERFACE_TimingReal *ptrReal,DRV_LDINTERFACE_TimingReg *ptrReg);//test ok
void drvif_cal_LdInterface_TimingReal(DRV_LDINTERFACE_TimingReal *ptrReal,DRV_LDINTERFACE_TimingReg *ptrReg);

/*Read Only: Status*/
/**
 * @brief get tx status:
 * @param :status info stored in *ptr
 */
void drvif_get_LdInterface_Status(DRV_LDINTERFACE_Status *ptr);

void drvif_color_set_LDInterface_SPI_IndexSRAM_Data_Continuous(DRV_LD_LDSPI_INDEXSRAM_TYPE *LDSPI_IndexSRAM, unsigned char Wait_Tx_Done);

void printTableInfo(DRV_LDINTERFACE_Table *ptr);


#ifdef __cplusplus
}
#endif

#endif /* __LOCALDIMMINGINTERFACE_H__*/



