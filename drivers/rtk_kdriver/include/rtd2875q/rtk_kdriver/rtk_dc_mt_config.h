#ifndef __DC_MT_CFG__
#define __DC_MT_CFG__

#include <rbus/dc_sys_reg.h>
//#include <rbus/dc2_sys_reg.h>


/*-------------------------------------------------------------
 * RTD299o DC SYS register
 *-------------------------------------------------------------*/
/*#define DC_MT_ENTRY_START                    0
#define DC_MT2_ENTRY_START                  4
#define DC_MT3_ENTRY_START					8
#define DC_MTEX_ENTRY_START                	12
#define DC_MTEX2_ENTRY_START             	14
#define DC_MT_ENTRY_COUNT                   20*/

#define DCMT_MT3


#define DC_MT_ENTRY_START                   0
#define DC_MT_ENTRY_END                     3
#define DC_MT2_ENTRY_START                  4
#define DC_MT2_ENTRY_END                    7
#define DC_MT3_ENTRY_START                  8
#define DC_MT3_ENTRY_END                    11
#define DC_MTEX_ENTRY_START                	12
#define DC_MTEX_ENTRY_END                	15
#define DC_MTEX2_ENTRY_START             	16
#define DC_MTEX2_ENTRY_END             		19
#define DC_MT_ENTRY_COUNT                   (DC_MTEX2_ENTRY_END+1)


#define DC_MT_OFST(i)                       (i<<2)
// DC1
#define SET_DC_MT_TYPE(i,val)               rtd_outl(DC_SYS_DC_MT_TYPE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC_MT_MODE(i,val)               rtd_outl(DC_SYS_DC_MT_MODE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC_MT_SADDR(i,val)              rtd_outl(DC_SYS_DC_MT_SADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC_MT_EADDR(i,val)              rtd_outl(DC_SYS_DC_MT_EADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC_MT_TABLE(i,val)              rtd_outl(DC_SYS_DC_MT_TABLE_0_reg       + DC_MT_OFST(i), val)
#define SET_DC_MT_ADDCMD_HI(i,val)          rtd_outl(DC_SYS_DC_MT_ADDCMD_HI_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_MT_ADDCMD_LO(i,val)          rtd_outl(DC_SYS_DC_MT_ADDCMD_LO_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS2_MT_ADDCMD_HI(i,val)          rtd_outl(DC_SYS_DC_MT_ADDCMD_HI_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS2_MT_ADDCMD_LO(i,val)          rtd_outl(DC_SYS_DC_MT_ADDCMD_LO_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS3_MT_ADDCMD_HI(i,val)          rtd_outl(DC_SYS_DC_MT_ADDCMD_HI_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS3_MT_ADDCMD_LO(i,val)          rtd_outl(DC_SYS_DC_MT_ADDCMD_LO_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_INT_ENABLE(val)              rtd_outl(DC_SYS_DC_int_enable_reg, val)
#define SET_DC_INT_STATUS(val)              rtd_outl(DC_SYS_DC_int_status_reg, val)
#define SET_DC_SYS2_INT_ENABLE(val)              rtd_outl(DC_SYS_DC_int_enable_SYS2_reg, val)
#define SET_DC_SYS2_INT_STATUS(val)              rtd_outl(DC_SYS_DC_int_status_SYS2_reg, val)
#define SET_DC_SYS3_INT_ENABLE(val)              rtd_outl(DC_SYS_DC_int_enable_SYS3_reg, val)
#define SET_DC_SYS3_INT_STATUS(val)              rtd_outl(DC_SYS_DC_int_status_SYS3_reg, val)

#define GET_DC_MT_TYPE(i)                   rtd_inl(DC_SYS_DC_MT_TYPE_0_reg         + DC_MT_OFST(i))
#define GET_DC_MT_MODE(i)                   rtd_inl(DC_SYS_DC_MT_MODE_0_reg         + DC_MT_OFST(i))
#define GET_DC_MT_SADDR(i)                  rtd_inl(DC_SYS_DC_MT_SADDR_0_reg        + DC_MT_OFST(i))
#define GET_DC_MT_EADDR(i)                  rtd_inl(DC_SYS_DC_MT_EADDR_0_reg        + DC_MT_OFST(i))
#define GET_DC_MT_TABLE(i)                  rtd_inl(DC_SYS_DC_MT_TABLE_0_reg        + DC_MT_OFST(i))
#define GET_DC_MT_ADDCMD_HI(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_HI_0_reg    + DC_MT_OFST(i))
#define GET_DC_MT_ADDCMD_LO(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_LO_0_reg    + DC_MT_OFST(i))
#define GET_DC_MT_ADDCMD_SA(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_SA_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS2_MT_ADDCMD_HI(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_HI_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS2_MT_ADDCMD_LO(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_LO_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS2_MT_ADDCMD_SA(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_SA_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS3_MT_ADDCMD_HI(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_HI_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS3_MT_ADDCMD_LO(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_LO_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS3_MT_ADDCMD_SA(i)              rtd_inl(DC_SYS_DC_MT_ADDCMD_SA_SYS3_0_reg    + DC_MT_OFST(i))

#define GET_DC_INT_ENABLE()                 rtd_inl(DC_SYS_DC_int_enable_reg)
#define GET_DC_INT_STATUS()                 rtd_inl(DC_SYS_DC_int_status_reg)
#define GET_DC_SYS2_INT_ENABLE()                 rtd_inl(DC_SYS_DC_int_enable_SYS2_reg)
#define GET_DC_SYS2_INT_STATUS()                 rtd_inl(DC_SYS_DC_int_status_SYS2_reg)
#define GET_DC_SYS3_INT_ENABLE()                 rtd_inl(DC_SYS_DC_int_enable_SYS3_reg)
#define GET_DC_SYS3_INT_STATUS()                 rtd_inl(DC_SYS_DC_int_status_SYS3_reg)

#define SET_DC_MTEX_TYPE(i,val)               rtd_outl(DC_SYS_DC_MTEX_TYPE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC_MTEX_MODE(i,val)               rtd_outl(DC_SYS_DC_MTEX_MODE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC_MTEX_SADDR(i,val)              rtd_outl(DC_SYS_DC_MTEX_SADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC_MTEX_EADDR(i,val)              rtd_outl(DC_SYS_DC_MTEX_EADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC_MTEX_TABLE0(i,val)              rtd_outl(DC_SYS_DC_MTEX_TABLE0_0_reg       + DC_MT_OFST(i), val)
#define SET_DC_MTEX_TABLE1(i,val)              rtd_outl(DC_SYS_DC_MTEX_TABLE1_0_reg       + DC_MT_OFST(i), val)
#define SET_DC_MTEX_ADDCMD_HI0(i,val)          rtd_outl(DC_SYS_DC_MTEX_ADDCMD_HI0_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_MTEX_ADDCMD_HI1(i,val)          rtd_outl(DC_SYS_DC_MTEX_ADDCMD_HI1_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_MTEX_ADDCMD_LO(i,val)          rtd_outl(DC_SYS_DC_MTEX_ADDCMD_LO_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS2_MTEX_ADDCMD_HI0(i,val)          rtd_outl(DC_SYS_DC_MTEX_ADDCMD_HI0_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS2_MTEX_ADDCMD_LO(i,val)          rtd_outl(DC_SYS_DC_MTEX_ADDCMD_LO_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS3_MTEX_ADDCMD_HI0(i,val)          rtd_outl(DC_SYS_DC_MTEX_ADDCMD_HI0_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_SYS3_MTEX_ADDCMD_LO(i,val)          rtd_outl(DC_SYS_DC_MTEX_ADDCMD_LO_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC_MTEX_INT_ENABLE(val)              rtd_outl(DC_SYS_DC_MTEX_int_enable_reg, val)
#define SET_DC_MTEX_INT_STATUS(val)              rtd_outl(DC_SYS_DC_MTEX_int_status_reg, val)
#define SET_DC_SYS2_MTEX_INT_ENABLE(val)              rtd_outl(DC_SYS_DC_MTEX_int_enable_SYS2_reg, val)
#define SET_DC_SYS2_MTEX_INT_STATUS(val)              rtd_outl(DC_SYS_DC_MTEX_int_status_SYS2_reg, val)
#define SET_DC_SYS3_MTEX_INT_ENABLE(val)              rtd_outl(DC_SYS_DC_MTEX_int_enable_SYS3_reg, val)
#define SET_DC_SYS3_MTEX_INT_STATUS(val)              rtd_outl(DC_SYS_DC_MTEX_int_status_SYS3_reg, val)

#define GET_DC_MTEX_TYPE(i)                   rtd_inl(DC_SYS_DC_MTEX_TYPE_0_reg         + DC_MT_OFST(i))
#define GET_DC_MTEX_MODE(i)                   rtd_inl(DC_SYS_DC_MTEX_MODE_0_reg         + DC_MT_OFST(i))
#define GET_DC_MTEX_SADDR(i)                  rtd_inl(DC_SYS_DC_MTEX_SADDR_0_reg        + DC_MT_OFST(i))
#define GET_DC_MTEX_EADDR(i)                  rtd_inl(DC_SYS_DC_MTEX_EADDR_0_reg        + DC_MT_OFST(i))
#define GET_DC_MTEX_TABLE0(i)                  rtd_inl(DC_SYS_DC_MTEX_TABLE0_0_reg        + DC_MT_OFST(i))
#define GET_DC_MTEX_TABLE1(i)                  rtd_inl(DC_SYS_DC_MTEX_TABLE1_0_reg        + DC_MT_OFST(i))
#define GET_DC_MTEX_ADDCMD_HI0(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_HI0_0_reg    + DC_MT_OFST(i))
#define GET_DC_MTEX_ADDCMD_HI1(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_HI1_0_reg    + DC_MT_OFST(i))
#define GET_DC_MTEX_ADDCMD_LO(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_LO_0_reg    + DC_MT_OFST(i))
#define GET_DC_MTEX_ADDCMD_SA(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_SA_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS2_MTEX_ADDCMD_HI0(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_HI0_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS2_MTEX_ADDCMD_LO(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_LO_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS2_MTEX_ADDCMD_SA(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_SA_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS3_MTEX_ADDCMD_HI0(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_HI0_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS3_MTEX_ADDCMD_LO(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_LO_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC_SYS3_MTEX_ADDCMD_SA(i)              rtd_inl(DC_SYS_DC_MTEX_ADDCMD_SA_SYS3_0_reg    + DC_MT_OFST(i))

#define GET_DC_MTEX_INT_ENABLE()                 rtd_inl(DC_SYS_DC_MTEX_int_enable_reg)
#define GET_DC_MTEX_INT_STATUS()                 rtd_inl(DC_SYS_DC_MTEX_int_status_reg)
#define GET_DC_SYS2_MTEX_INT_ENABLE()                 rtd_inl(DC_SYS_DC_MTEX_int_enable_SYS2_reg)
#define GET_DC_SYS2_MTEX_INT_STATUS()                 rtd_inl(DC_SYS_DC_MTEX_int_status_SYS2_reg)
#define GET_DC_SYS3_MTEX_INT_ENABLE()                 rtd_inl(DC_SYS_DC_MTEX_int_enable_SYS3_reg)
#define GET_DC_SYS3_MTEX_INT_STATUS()                 rtd_inl(DC_SYS_DC_MTEX_int_status_SYS3_reg)

// DC2
#define SET_DC2_MT_TYPE(i,val)               rtd_outl(DC2_SYS_DC_MT_TYPE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC2_MT_MODE(i,val)               rtd_outl(DC2_SYS_DC_MT_MODE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC2_MT_SADDR(i,val)              rtd_outl(DC2_SYS_DC_MT_SADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC2_MT_EADDR(i,val)              rtd_outl(DC2_SYS_DC_MT_EADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC2_MT_TABLE(i,val)              rtd_outl(DC2_SYS_DC_MT_TABLE_0_reg       + DC_MT_OFST(i), val)
#define SET_DC2_MT_ADDCMD_HI(i,val)          rtd_outl(DC2_SYS_DC_MT_ADDCMD_HI_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_MT_ADDCMD_LO(i,val)          rtd_outl(DC2_SYS_DC_MT_ADDCMD_LO_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS2_MT_ADDCMD_HI(i,val)     rtd_outl(DC2_SYS_DC_MT_ADDCMD_HI_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS2_MT_ADDCMD_LO(i,val)     rtd_outl(DC2_SYS_DC_MT_ADDCMD_LO_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS3_MT_ADDCMD_HI(i,val)     rtd_outl(DC2_SYS_DC_MT_ADDCMD_HI_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS3_MT_ADDCMD_LO(i,val)     rtd_outl(DC2_SYS_DC_MT_ADDCMD_LO_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_INT_ENABLE(val)              rtd_outl(DC2_SYS_DC_int_enable_reg , val)
#define SET_DC2_INT_STATUS(val)              rtd_outl(DC2_SYS_DC_int_status_reg , val)
#define SET_DC2_SYS2_INT_ENABLE(val)         rtd_outl(DC2_SYS_DC_int_enable_SYS2_reg, val)
#define SET_DC2_SYS2_INT_STATUS(val)         rtd_outl(DC2_SYS_DC_int_status_SYS2_reg, val)
#define SET_DC2_SYS3_INT_ENABLE(val)         rtd_outl(DC2_SYS_DC_int_enable_SYS3_reg, val)
#define SET_DC2_SYS3_INT_STATUS(val)         rtd_outl(DC2_SYS_DC_int_status_SYS3_reg, val)

#define GET_DC2_MT_TYPE(i)                   rtd_inl(DC2_SYS_DC_MT_TYPE_0_reg      + DC_MT_OFST(i))
#define GET_DC2_MT_MODE(i)                   rtd_inl(DC2_SYS_DC_MT_MODE_0_reg      + DC_MT_OFST(i))
#define GET_DC2_MT_SADDR(i)                  rtd_inl(DC2_SYS_DC_MT_SADDR_0_reg     + DC_MT_OFST(i))
#define GET_DC2_MT_EADDR(i)                  rtd_inl(DC2_SYS_DC_MT_EADDR_0_reg     + DC_MT_OFST(i))
#define GET_DC2_MT_TABLE(i)                  rtd_inl(DC2_SYS_DC_MT_TABLE_0_reg     + DC_MT_OFST(i))
#define GET_DC2_MT_ADDCMD_HI(i)              rtd_inl(DC2_SYS_DC_MT_ADDCMD_HI_0_reg + DC_MT_OFST(i))
#define GET_DC2_MT_ADDCMD_LO(i)              rtd_inl(DC2_SYS_DC_MT_ADDCMD_LO_0_reg + DC_MT_OFST(i))
#define GET_DC2_MT_ADDCMD_SA(i)              rtd_inl(DC2_SYS_DC_MT_ADDCMD_SA_0_reg + DC_MT_OFST(i))
#define GET_DC2_SYS2_MT_ADDCMD_HI(i)         rtd_inl(DC2_SYS_DC_MT_ADDCMD_HI_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS2_MT_ADDCMD_LO(i)         rtd_inl(DC2_SYS_DC_MT_ADDCMD_LO_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS2_MT_ADDCMD_SA(i)         rtd_inl(DC2_SYS_DC_MT_ADDCMD_SA_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS3_MT_ADDCMD_HI(i)         rtd_inl(DC2_SYS_DC_MT_ADDCMD_HI_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS3_MT_ADDCMD_LO(i)         rtd_inl(DC2_SYS_DC_MT_ADDCMD_LO_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS3_MT_ADDCMD_SA(i)         rtd_inl(DC2_SYS_DC_MT_ADDCMD_SA_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC2_INT_ENABLE()                 rtd_inl(DC2_SYS_DC_int_enable_reg)
#define GET_DC2_INT_STATUS()                 rtd_inl(DC2_SYS_DC_int_status_reg)
#define GET_DC2_SYS2_INT_ENABLE()            rtd_inl(DC2_SYS_DC_int_enable_SYS2_reg)
#define GET_DC2_SYS2_INT_STATUS()            rtd_inl(DC2_SYS_DC_int_status_SYS2_reg)
#define GET_DC2_SYS3_INT_ENABLE()            rtd_inl(DC2_SYS_DC_int_enable_SYS3_reg)
#define GET_DC2_SYS3_INT_STATUS()            rtd_inl(DC2_SYS_DC_int_status_SYS3_reg)

#define SET_DC2_MTEX_TYPE(i,val)               rtd_outl(DC2_SYS_DC_MTEX_TYPE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_MODE(i,val)               rtd_outl(DC2_SYS_DC_MTEX_MODE_0_reg        + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_SADDR(i,val)              rtd_outl(DC2_SYS_DC_MTEX_SADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_EADDR(i,val)              rtd_outl(DC2_SYS_DC_MTEX_EADDR_0_reg       + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_TABLE0(i,val)              rtd_outl(DC2_SYS_DC_MTEX_TABLE0_0_reg       + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_TABLE1(i,val)              rtd_outl(DC2_SYS_DC_MTEX_TABLE1_0_reg       + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_ADDCMD_HI0(i,val)          rtd_outl(DC2_SYS_DC_MTEX_ADDCMD_HI0_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_ADDCMD_HI1(i,val)          rtd_outl(DC2_SYS_DC_MTEX_ADDCMD_HI1_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_ADDCMD_LO(i,val)          rtd_outl(DC2_SYS_DC_MTEX_ADDCMD_LO_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS2_MTEX_ADDCMD_HI0(i,val)          rtd_outl(DC2_SYS_DC_MTEX_ADDCMD_HI0_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS2_MTEX_ADDCMD_LO(i,val)          rtd_outl(DC2_SYS_DC_MTEX_ADDCMD_LO_SYS2_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS3_MTEX_ADDCMD_HI0(i,val)          rtd_outl(DC2_SYS_DC_MTEX_ADDCMD_HI0_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_SYS3_MTEX_ADDCMD_LO(i,val)          rtd_outl(DC2_SYS_DC_MTEX_ADDCMD_LO_SYS3_0_reg   + DC_MT_OFST(i), val)
#define SET_DC2_MTEX_INT_ENABLE(val)              rtd_outl(DC2_SYS_DC_MTEX_int_enable_reg, val)
#define SET_DC2_MTEX_INT_STATUS(val)              rtd_outl(DC2_SYS_DC_MTEX_int_status_reg, val)
#define SET_DC2_SYS2_MTEX_INT_ENABLE(val)              rtd_outl(DC2_SYS_DC_MTEX_int_enable_SYS2_reg, val)
#define SET_DC2_SYS2_MTEX_INT_STATUS(val)              rtd_outl(DC2_SYS_DC_MTEX_int_status_SYS2_reg, val)
#define SET_DC2_SYS3_MTEX_INT_ENABLE(val)              rtd_outl(DC2_SYS_DC_MTEX_int_enable_SYS3_reg, val)
#define SET_DC2_SYS3_MTEX_INT_STATUS(val)              rtd_outl(DC2_SYS_DC_MTEX_int_status_SYS3_reg, val)

#define GET_DC2_MTEX_TYPE(i)                   rtd_inl(DC2_SYS_DC_MTEX_TYPE_0_reg         + DC_MT_OFST(i))
#define GET_DC2_MTEX_MODE(i)                   rtd_inl(DC2_SYS_DC_MTEX_MODE_0_reg         + DC_MT_OFST(i))
#define GET_DC2_MTEX_SADDR(i)                  rtd_inl(DC2_SYS_DC_MTEX_SADDR_0_reg        + DC_MT_OFST(i))
#define GET_DC2_MTEX_EADDR(i)                  rtd_inl(DC2_SYS_DC_MTEX_EADDR_0_reg        + DC_MT_OFST(i))
#define GET_DC2_MTEX_TABLE0(i)                  rtd_inl(DC2_SYS_DC_MTEX_TABLE0_0_reg        + DC_MT_OFST(i))
#define GET_DC2_MTEX_TABLE1(i)                  rtd_inl(DC2_SYS_DC_MTEX_TABLE1_0_reg        + DC_MT_OFST(i))
#define GET_DC2_MTEX_ADDCMD_HI0(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_HI0_0_reg    + DC_MT_OFST(i))
#define GET_DC2_MTEX_ADDCMD_HI1(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_HI1_0_reg    + DC_MT_OFST(i))
#define GET_DC2_MTEX_ADDCMD_LO(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_LO_0_reg    + DC_MT_OFST(i))
#define GET_DC2_MTEX_ADDCMD_SA(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_SA_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS2_MTEX_ADDCMD_HI0(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_HI0_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS2_MTEX_ADDCMD_LO(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_LO_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS2_MTEX_ADDCMD_SA(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_SA_SYS2_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS3_MTEX_ADDCMD_HI0(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_HI0_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS3_MTEX_ADDCMD_LO(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_LO_SYS3_0_reg    + DC_MT_OFST(i))
#define GET_DC2_SYS3_MTEX_ADDCMD_SA(i)              rtd_inl(DC2_SYS_DC_MTEX_ADDCMD_SA_SYS3_0_reg    + DC_MT_OFST(i))

#define GET_DC2_MTEX_INT_ENABLE()                 rtd_inl(DC2_SYS_DC_MTEX_int_enable_reg)
#define GET_DC2_MTEX_INT_STATUS()                 rtd_inl(DC2_SYS_DC_MTEX_int_status_reg)
#define GET_DC2_SYS2_MTEX_INT_ENABLE()                 rtd_inl(DC2_SYS_DC_MTEX_int_enable_SYS2_reg)
#define GET_DC2_SYS2_MTEX_INT_STATUS()                 rtd_inl(DC2_SYS_DC_MTEX_int_status_SYS2_reg)
#define GET_DC2_SYS3_MTEX_INT_ENABLE()                 rtd_inl(DC2_SYS_DC_MTEX_int_enable_SYS3_reg)
#define GET_DC2_SYS3_MTEX_INT_STATUS()                 rtd_inl(DC2_SYS_DC_MTEX_int_status_SYS3_reg)

#define DC_MT_MODE_WE5          (1<<2)
#define DC_MT_MODE_MODE(x)      (x & 0x3)

#define DC_MT_TABLE_ENTRY(x)    (x&0xFF)
#define DC_MT_TABLE_MODULE0(x)  (DC_MT_TABLE_ENTRY(x)<<24)
#define DC_MT_TABLE_MODULE1(x)  (DC_MT_TABLE_ENTRY(x)<<16)
#define DC_MT_TABLE_MODULE2(x)  (DC_MT_TABLE_ENTRY(x)<<8)
#define DC_MT_TABLE_MODULE3(x)  (DC_MT_TABLE_ENTRY(x))

#define DC_MT_ACCESS_TYPE(x)        (x&0x03)
#define DC_MT_ACCESS_TYPE0(x)   (DC_MT_ACCESS_TYPE(x)<<30)
#define DC_MT_ACCESS_TYPE1(x)   (DC_MT_ACCESS_TYPE(x)<<28)
#define DC_MT_ACCESS_TYPE2(x)   (DC_MT_ACCESS_TYPE(x)<<26)
#define DC_MT_ACCESS_TYPE3(x)   (DC_MT_ACCESS_TYPE(x)<<24)

#define DC_MT_ADDCMD_HI_EVER_TRAP            (1 << 22)

#define DC_INT_EN_RD_TAG_MISMATCH_INT_EN     (0x1UL<<23)
#define DC_INT_EN_ERR_SEQ_SA_ODD_INT_EN      (0x1UL<<22)
#define DC_INT_EN_ERR_SEQ_BL_ZERO_INT_EN     (0x1UL<<21)
#define DC_INT_EN_ERR_SEQ_BL_ODD_INT_EN      (0x1UL<<20)
#define DC_INT_EN_ERR_BLK_8X2_ON_64_INT_EN   (0x1UL<<19)
#define DC_INT_EN_ERR_BLK_SIZE_EXCEED_INT_EN (0x1UL<<18)
#define DC_INT_EN_ERR_BLK_H_ZERO_INT_EN      (0x1UL<<17)
#define DC_INT_EN_ERR_BLK_W_ZERO_INT_EN      (0x1UL<<16)
#define DC_INT_EN_MT_3_MOD3_INT_EN           (0x1UL<<12)
#define DC_INT_EN_MT_3_MOD2_INT_EN           (0x1UL<<11)
#define DC_INT_EN_MT_3_MOD1_INT_EN           (0x1UL<<10)
#define DC_INT_EN_MT_2_MOD3_INT_EN           (0x1UL<<9)
#define DC_INT_EN_MT_2_MOD2_INT_EN           (0x1UL<<8)
#define DC_INT_EN_MT_2_MOD1_INT_EN           (0x1UL<<7)
#define DC_INT_EN_MT_1_MOD3_INT_EN           (0x1UL<<6)
#define DC_INT_EN_MT_1_MOD2_INT_EN           (0x1UL<<5)
#define DC_INT_EN_MT_1_MOD1_INT_EN           (0x1UL<<4)
#define DC_INT_EN_MT_0_MOD3_INT_EN           (0x1UL<<3)
#define DC_INT_EN_MT_0_MOD2_INT_EN           (0x1UL<<2)
#define DC_INT_EN_MT_0_MOD1_INT_EN           (0x1UL<<1)
#define DC_INT_EN_WRITE_DATA                 (0x1UL<<0)

#define DC_INT_EN_MT_0_INTS             (DC_INT_EN_MT_0_MOD1_INT_EN | DC_INT_EN_MT_0_MOD2_INT_EN | DC_INT_EN_MT_0_MOD3_INT_EN)
#define DC_INT_EN_MT_1_INTS             (DC_INT_EN_MT_1_MOD1_INT_EN | DC_INT_EN_MT_1_MOD2_INT_EN | DC_INT_EN_MT_1_MOD3_INT_EN)
#define DC_INT_EN_MT_2_INTS             (DC_INT_EN_MT_2_MOD1_INT_EN | DC_INT_EN_MT_2_MOD2_INT_EN | DC_INT_EN_MT_2_MOD3_INT_EN)
#define DC_INT_EN_MT_3_INTS             (DC_INT_EN_MT_3_MOD1_INT_EN | DC_INT_EN_MT_3_MOD2_INT_EN | DC_INT_EN_MT_3_MOD3_INT_EN)

#define DC_INT_STATUS_RD_TAG_MISMATCH_INT       (0x1UL<<23)
#define DC_INT_STATUS_ERR_SEQ_SA_ODD_INT        (0x1UL<<22)
#define DC_INT_STATUS_ERR_SEQ_BL_ZERO_INT       (0x1UL<<21)
#define DC_INT_STATUS_ERR_SEQ_BL_ODD_INT        (0x1UL<<20)
#define DC_INT_STATUS_ERR_BLK_8X2_ON_64_INT     (0x1UL<<19)
#define DC_INT_STATUS_ERR_BLK_SIZE_EXCEED_INT   (0x1UL<<18)
#define DC_INT_STATUS_ERR_BLK_H_ZERO_INT        (0x1UL<<17)
#define DC_INT_STATUS_ERR_BLK_W_ZERO_INT        (0x1UL<<16)
#define DC_INT_STATUS_MT_3_MOD3_INT             (0x1UL<<12)
#define DC_INT_STATUS_MT_3_MOD2_INT             (0x1UL<<11)
#define DC_INT_STATUS_MT_3_MOD1_INT             (0x1UL<<10)
#define DC_INT_STATUS_MT_2_MOD3_INT             (0x1UL<<9)
#define DC_INT_STATUS_MT_2_MOD2_INT             (0x1UL<<8)
#define DC_INT_STATUS_MT_2_MOD1_INT             (0x1UL<<7)
#define DC_INT_STATUS_MT_1_MOD3_INT             (0x1UL<<6)
#define DC_INT_STATUS_MT_1_MOD2_INT             (0x1UL<<5)
#define DC_INT_STATUS_MT_1_MOD1_INT             (0x1UL<<4)
#define DC_INT_STATUS_MT_0_MOD3_INT             (0x1UL<<3)
#define DC_INT_STATUS_MT_0_MOD2_INT             (0x1UL<<2)
#define DC_INT_STATUS_MT_0_MOD1_INT             (0x1UL<<1)
#define DC_INT_STATUS_WRITE_DATA                (0x1UL<<0)

#define DC_INT_STATUS_MOD1_INT              (0x1UL<<0)
#define DC_INT_STATUS_MOD2_INT              (0x1UL<<1)
#define DC_INT_STATUS_MOD3_INT              (0x1UL<<2)
#define DC_INT_STATUS_INTS                  (DC_INT_STATUS_MOD1_INT | DC_INT_STATUS_MOD2_INT | DC_INT_STATUS_MOD3_INT)

#define DC_INT_STATUS_MTx_MOD3_INT(i)       (DC_INT_STATUS_MOD3_INT<<((3*i) +1))
#define DC_INT_STATUS_MTx_MOD2_INT(i)       (DC_INT_STATUS_MOD2_INT<<((3*i) +1))
#define DC_INT_STATUS_MTx_MOD1_INT(i)       (DC_INT_STATUS_MOD1_INT<<((3*i) +1))

#define DC_INT_STATUS_MTx_INT_MASK(i)       (DC_INT_STATUS_INTS << ((3*i) +1))
#define DC_INT_STATUS_MTx_INT_MASK_NO_WRITE_DATA(i)       (DC_INT_STATUS_INTS << (3*i))


/*Merlin9 module str*/

/*Merlin9 module str*/
#define MODULE_TVSB2_DIW            MODULE_ID(0, 0)
#define MODULE_TVSB2_DIR            MODULE_ID(0, 1)
#define MODULE_TVSB2_M_CAP          MODULE_ID(0, 2)
#define MODULE_TVSB2_M_DISP         MODULE_ID(0, 3)
#define MODULE_TVSB2_S_CAP          MODULE_ID(0, 4)
#define MODULE_TVSB2_S_DISP         MODULE_ID(0, 5)
#define MODULE_TVSB2_VO1_Y          MODULE_ID(0, 6)
#define MODULE_TVSB2_VO1_C          MODULE_ID(0, 7)
#define MODULE_TVSB2_VO2_Y          MODULE_ID(0, 8)
#define MODULE_TVSB2_VO2_C          MODULE_ID(0, 9)
#define MODULE_TVSB2_I3DDMA_Y       MODULE_ID(0, 0xa)
//#define MODULE_TVSB2_DEXC_W         MODULE_ID(0, 0xb)
//#define MODULE_TVSB2_DEXC_R         MODULE_ID(0, 0xc)
#define MODULE_TVSB2_SNR_SSB          MODULE_ID(0, 0xd)
#define MODULE_TVSB2_3DTABLE          MODULE_ID(0, 0xe)
#define MODULE_TVSB2                  MODULE_ID(0, 0xf)
#define MODULE_TVSB2_SNR_R  MODULE_TVSB2_SNR_SSB



#define MODULE_TVSB1_VD             MODULE_ID(1, 0)
#define MODULE_TVSB1_AUDIO_W          MODULE_ID(1, 1)
#define MODULE_TVSB1_AUDIO_R        MODULE_ID(1, 2)
#define MODULE_TVSB1_VBI            MODULE_ID(1, 3)
#define MODULE_TVSB1                MODULE_ID(1, 0xf)

#define MODULE_SB1_MD_KC                        MODULE_ID(2, 1)
#define MODULE_SB1_USB                          MODULE_ID(2, 2)
#define MODULE_SB1_RBUSDMA                      MODULE_ID(2, 3)
#define MODULE_SB1_USB2                         MODULE_ID(2, 4)
#define MODULE_SB1_ETN                          MODULE_ID(2, 5)
#define MODULE_SB1_UART_DMA                     MODULE_ID(2, 6)
#define MODULE_SB1_EMMC_KC                      MODULE_ID(2, 7)
#define MODULE_SB1_MD_NKC_NVC                   MODULE_ID(2, 9)
#define MODULE_SB1_MD_VC                        MODULE_ID(2, 0xa)
#define MODULE_SB1_USB_OTG                      MODULE_ID(2, 0xc)
//#define MODULE_SB1_TP                          MODULE_ID(2, 0xb)
#define MODULE_SB1_EMMC_NKC                     MODULE_ID(2, 0xe)
#define MODULE_SB1                              MODULE_ID(2, 0xf)


#define MODULE_SB2_VCPU                 MODULE_ID(3, 3)
#define MODULE_SB2_LZMA                 MODULE_ID(3, 4)
#define MODULE_SB2_UART                 MODULE_ID(3, 5)
#define MODULE_SB2_HOST_IF              MODULE_ID(3, 6)
#define MODULE_SB2_VCPU2                MODULE_ID(3, 7)
#define MODULE_SB2_RTMMU                MODULE_ID(3, 0xA)
#define MODULE_SB2                      MODULE_ID(3, 0xf)



#define MODULE_SB1_SSB1_TP2_KC                MODULE_ID(4, 4)
#define MODULE_SB1_SSB1_TP_KC                 MODULE_ID(4, 6)
#define MODULE_SB1_SSB1_TP2_NKC               MODULE_ID(4, 0xa)
#define MODULE_SB1_SSB1_TP_NKC                 MODULE_ID(4, 0xb)
#define MODULE_SB1_SSB1                       MODULE_ID(4, 0xf)



#define MODULE_VE_VM1                           MODULE_ID(0x5 ,0)
#define MODULE_VE_VM2                           MODULE_ID(0x5 ,1)
#define MODULE_VE_PRD                           MODULE_ID(0x5 ,2)
#define MODULE_VE_DBK                           MODULE_ID(0x5 ,3)
#define MODULE_VE_MVD                           MODULE_ID(0x5 ,4)
#define MODULE_VE_NDBP2                         MODULE_ID(0x5 ,5)
#define MODULE_VE_R1SEQ                         MODULE_ID(0x5 ,6)
#define MODULE_VE_BS                            MODULE_ID(0x5 ,7)
#define MODULE_VE_R1BLK                         MODULE_ID(0x5 ,8)
#define MODULE_VE_VEP2COM                       MODULE_ID(0x5 ,9)
#define MODULE_VE_VECOM                         MODULE_ID(0x5 ,0xa)
#define MODULE_VE_CABP2                         MODULE_ID(0x5 ,0xb)
#define MODULE_VE_BSP2                          MODULE_ID(0x5 ,0xc)
#define MODULE_VE_LR_SAO                        MODULE_ID(0x5 ,0xd)
#define MODULE_VE_SEG                           MODULE_ID(0x5 ,0xe)
#define MODULE_VE                               MODULE_ID(0x5 ,0xf)


#define MODULE_TVSB3_SEQ0               MODULE_ID(0x6 ,0)
#define MODULE_TVSB3_SEQ1               MODULE_ID(0x6 ,1)
#define MODULE_TVSB3_CP_KC              MODULE_ID(0x6 ,2)
#define MODULE_TVSB3_CP_NKC             MODULE_ID(0x6 ,3)
#define MODULE_TVSB3_DDR_TR_GEN         MODULE_ID(0x6 ,6)
#define MODULE_TVSB3_SE0                MODULE_ID(0x6 ,8)
#define MODULE_TVSB3_SE1                MODULE_ID(0x6 ,9)
#define MODULE_TVSB3                    MODULE_ID(0x6 ,0xf)                                      

#define MODULE_GPU_NKC                          MODULE_ID(0x7 ,0)
#define MODULE_GPU_KC                           MODULE_ID(0x7 ,1)
#define MODULE_GPU10                            MODULE_ID(0x7 ,2)
#define MODULE_GPU11                            MODULE_ID(0x7 ,3)
#define MODULE_GPU                              MODULE_ID(0x7 ,0xf)


#define MODULE_TVSB5_DEMOD                      MODULE_ID(0x8 ,0)
#define MODULE_TVSB5_DEBUGDMA                   MODULE_ID(0x8 ,1)
#define MODULE_TVSB5_DEMOD2                     MODULE_ID(0x8 ,2)
#define MODULE_TVSB5_USB3                       MODULE_ID(0x8 ,3)
#define MODULE_TVSB5_DDR_TR_GEN                 MODULE_ID(0x8 ,4)
//#define MODULE_TVSB5_PCIE                             MODULE_ID(0x8 ,5)
#define MODULE_TVSB5                            MODULE_ID(0x8, 0xf)

#define MODULE_ACPU_NS                          MODULE_ID(0x9 ,0)
#define MODULE_ACPU_S                           MODULE_ID(0x9 ,1)
#define MODULE_ACPU                                MODULE_ID(0x9 ,0xf) 


#define MODULE_VE2_VM1                           MODULE_ID(0xa ,0)
#define MODULE_VE2_VM2                           MODULE_ID(0xa ,0x1)
#define MODULE_VE2_DBK                           MODULE_ID(0xa ,0x3)
#define MODULE_VE2_BS                            MODULE_ID(0xa ,0x7)
#define MODULE_VE2_IME                           MODULE_ID(0xa ,0xa)
#define MODULE_VE2_NDB                           MODULE_ID(0xa ,0xd)
#define MODULE_VE2_FME                           MODULE_ID(0xa ,0xe)
#define MODULE_VE2                              MODULE_ID(0xa ,0xf)
 

#define MODULE_TVSB4_ODW                                MODULE_ID(0xb ,0)
#define MODULE_TVSB4_ODR                                MODULE_ID(0xb ,1)
#define MODULE_TVSB4_DC2H                               MODULE_ID(0xb ,2)
#define MODULE_TVSB4_DDR_TR_GEN                         MODULE_ID(0xb ,3)
#define MODULE_TVSB4_OSD1                               MODULE_ID(0xb ,4)
#define MODULE_TVSB4_OSD2                               MODULE_ID(0xb ,5)
#define MODULE_TVSB4_OSD3                               MODULE_ID(0xb, 6)
#define MODULE_TVSB4_DEMURA                             MODULE_ID(0xb ,7)
#define MODULE_TVSB4_DMATO3DLUT                         MODULE_ID(0xb, 8)
#define MODULE_TVSB4_SRNN_SSB_R                         MODULE_ID(0xb ,0xb)
#define MODULE_TVSB4_D_PQMASK_D                         MODULE_ID(0xb,0xc)
#define MODULE_TVSB4_RTK_LD_R                           MODULE_ID(0xb,0xc)
#define MODULE_TVSB4_SLD_W                              MODULE_ID(0xb,0xe)
#define MODULE_TVSB4                                    MODULE_ID(0xb, 0xf)

#define MODULE_MEMC_ME_W                       MODULE_ID(0xc, 0)
#define MODULE_MEMC_ME_R                       MODULE_ID(0xc, 1)
#define MODULE_MEMC_MC_LF_W                    MODULE_ID(0xc, 2)
#define MODULE_MEMC_MC_HF_W                    MODULE_ID(0xc, 3)
#define MODULE_MEMC_MC_LFI_R                   MODULE_ID(0xc, 4)
#define MODULE_MEMC_MC_HFI_R                   MODULE_ID(0xc, 5)
#define MODULE_MEMC_MC_LFP_R                   MODULE_ID(0xc, 6)
#define MODULE_MEMC_MC_HFP_R                   MODULE_ID(0xc, 7)
#define MODULE_MEMC_ME_SHR_W                   MODULE_ID(0xc, 8)
#define MODULE_MEMC_ME_SHR_R                   MODULE_ID(0xc, 9)
#define MODULE_MEMC_MVINFO_W                   MODULE_ID(0xc, 0xa)
#define MODULE_MEMC_MVINFO_R                   MODULE_ID(0xc, 0xb)
#define MODULE_MEMC                            MODULE_ID(0xc, 0xf)


#define MODULE_TVSB7_ME                         MODULE_ID(0xd, 0)
#define MODULE_TVSB7_VDE                        MODULE_ID(0xd, 1)
#define MODULE_TVSB7_MF2                        MODULE_ID(0xd, 2)
#define MODULE_TVSB7_CDF_ALF                    MODULE_ID(0xd, 3)
#define MODULE_TVSB7_MF1                        MODULE_ID(0xd, 4)
#define MODULE_TVSB7_MNHP2                      MODULE_ID(0xd, 5)
#define MODULE_TVSB7_CMP                        MODULE_ID(0xd, 6)
#define MODULE_TVSB7_MF0                        MODULE_ID(0xd, 7)
#define MODULE_TVSB7_VDE2                       MODULE_ID(0xd, 8)
#define MODULE_TVSB7_DMVR                       MODULE_ID(0xd, 9)
#define MODULE_TVSB7_DDR_TR_GEN                 MODULE_ID(0xd, 0xe)
#define MODULE_TVSB7                            MODULE_ID(0xd, 0xf)



#define MODULE_SCPU_NS              			MODULE_ID(0xF, 0)
#define MODULE_SCPU_S               			MODULE_ID(0xF, 1)

#define MODULE_ANY                              MODULE_ID(0xf, 0xf)



#define  DCMT_MODULE_DEMOD       MODULE_TVSB5_DEMOD
#define  DCMT_MODULE_DEMOD2      MODULE_TVSB5_DEMOD2
#define  DCMT_MODULE_CP_KC       MODULE_TVSB3_CP_KC 
#define  DCMT_MODULE_M_CAP       MODULE_TVSB2_M_CAP
//#define  DCMT_MODULE_M_CAP_C   MODULE_TVSB2_M_CAP_C
#define  DCMT_MODULE_M_DISP      MODULE_TVSB2_M_DISP
//#define  DCMT_MODULE_M_DISP_C    MODULE_TVSB2_M_DISP_C
#define  DCMT_MODULE_S_CAP       MODULE_TVSB2_S_CAP
//#define  DCMT_MODULE_S_CAP_C     MODULE_TVSB2_S_CAP_C
#define  DCMT_MODULE_S_DISP      MODULE_TVSB2_S_DISP
//#define  DCMT_MODULE_S_DISP_C    MODULE_TVSB2_S_DISP_C
#define  DCMT_MODULE_SNR         MODULE_TVSB2_SNR_R
#define  DCMT_MODULE_DMATO3DLUT  MODULE_TVSB4_DMATO3DLUT 
#define  DCMT_MODULE_TP_KC       MODULE_SB1_SSB1_TP_KC
#define  DCMT_MODULE_TP_NKC      MODULE_SB1_SSB1_TP_NKC      
#define  DCMT_MODULE_MD_KC       MODULE_SB1_MD_KC


#endif //__DC_MT_CFG__
