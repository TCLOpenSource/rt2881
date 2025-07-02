#ifndef __VDEC_QC_DBG_H__
#define __VDEC_QC_DBG_H__

#define QC_VE_DBG_BASE					(0x01fff800)
#define QC_VE_DBG_BASE_PT1				(0x00003000)
#define QC_VE_DBG_BASE_PT2				(0x00000080)
#define QC_VE_DBG_BASE_PT3				(0x03204000)

typedef enum {
	QC_VE_DBG_VAL_STATUS = 0,
	QC_VE_DBG_VAL_PT_1, //pattern address QC_VE_DBG_BASE_PT1
	QC_VE_DBG_VAL_PT_2, //pattern address QC_VE_DBG_BASE_PT2
	QC_VE_DBG_VAL_PT_3, //pattern address QC_VE_DBG_BASE_PT3
	QC_VE_DBG_VAL_PC_1, //VC_TRACE_0
	QC_VE_DBG_VAL_PC_2, //VC_TRACE_1
	QC_VE_DBG_VAL_FPGA_DBG1, //RIF_REG_FPGA_DBG1_reg
	QC_VE_DBG_VAL_CNT_MAIN, //count for main function running
	QC_VE_DBG_VAL_CNT_P1,
	QC_VE_DBG_VAL_CNT_P2,
	QC_VE_DBG_VAL_CNT_P3,
	QC_VE_DBG_VAL_CNT_ISR,
	QC_VE_DBG_VAL_ERR_P1,
	QC_VE_DBG_VAL_ERR_P2,
	QC_VE_DBG_VAL_CRC_CNT,
	QC_VE_DBG_VAL_CRC_FW, //crc fw in the last frame
	QC_VE_DBG_VAL_CRC_GOLDEN, //crc golden in the last frame
	QC_VE_DBG_VAL_TOTAL
} QC_VE_DBG_VAL;

#if defined CONFIG_QC_VE1 || defined CONFIG_QC_VE2
static inline char *qcVeDbgValString[QC_VE_DBG_VAL_TOTAL] = 
{
	"status"
	, "pt_1"
	, "pt_2"
	, "pt_3"
	, "pc_1"
	, "pc_2"
	, "fpga_dbg1"
	, "cnt_main"
	, "cnt_p1"
	, "cnt_p2"
	, "cnt_p3"
	, "cnt_isr"
	, "err_p1"
	, "err_p2"
	, "crc_cnt"
	, "crc_fw"
	, "crc_gd"
};
#define QC_VE_DBG_PHY_TO_VIR_MASK		(0x00000000)
#else
#define QC_VE_DBG_PHY_TO_VIR_MASK		(0xa0000000)
#endif

#define QC_VE_DBG_BASE_VIR				(QC_VE_DBG_BASE | QC_VE_DBG_PHY_TO_VIR_MASK)

#if defined CONFIG_QC_VE1 || defined CONFIG_QC_VE2 || defined QCTEST
#define QC_VE_DBG_VAL_SET(IDX, VAL)	\
(((volatile unsigned int *)QC_VE_DBG_BASE_VIR)[(IDX)]) = (VAL)
#define QC_VE_DBG_VAL_GET(IDX)	\
(((volatile unsigned int *)QC_VE_DBG_BASE_VIR)[(IDX)])
#else
#define QC_VE_DBG_VAL_SET(...)
#define QC_VE_DBG_VAL_GET(...)
#endif

#endif