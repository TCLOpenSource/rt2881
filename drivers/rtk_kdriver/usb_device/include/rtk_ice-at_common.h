#ifndef __RTICE_AT_COMMON_H__
#define __RTICE_AT_COMMON_H__


#include "rtk_ice.h"
//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************
#ifndef UINT8
#define UINT8    unsigned char
#endif

#ifndef UINT8
#define UINT8    char
#endif

#ifndef UINT16
#define UINT16   unsigned short
#endif

#ifndef INT16
#define INT16    short
#endif

#ifndef UINT32
#define UINT32   unsigned int
#endif

#ifndef INT32
#define INT32    int
#endif


typedef struct{
	UINT8 	*cmd;
	UINT8 	*cmd_data;
	INT16 	command_data_len;
	UINT8 	*response_data;
	INT16 	ret;
	RTICE_CMD_CONNECT_TYPE connect_type;
} rtice_command_param_t;

//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern rtice_command_param_t gCMDParam;
//extern __xdata UINT8 *__pointer_to_at_cmd_param;
extern UINT8 *__pointer_to_rtice_cmd_param;



//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************

/* the 0xA0 TPDU of AT command service group
 +---------------------------------------------------------------------------+
 | B0   | B1~2       |    *      | B4-B7     | 4Bytes | * | 4Bytes   | 1Byte |
 +------+------------+-----------+-----------+--------+---+----------+-------+
 | 0xA0 | 4*(n+1) +X | 0-padding | CMD_INDEX | ARG[0] | * | ARG[n-1] | LRC   |
 +---------------------------------------------------------------------------+
 - all of CMD_INDEX and ARGS are INT32 type and little endian.
 - amount of ARGS could be 0 ~ n.
 - X=2 if (n+1)<0x20, otherwise X=1. (be compatible to general TPDU definition)
*/

/* the 0xA0 R-TPDU of AT command service group
 +----------------------------------------------------+
 | B0   | B1~2     |    *      | B4-starting  | 1Byte |
 +------+----------+-----------+--------------+-------+
 | 0xA0 | 4*(n)+x  | 0-padding | 4N data      | LRC   |
 +----------------------------------------------------+
 - x=2 if n<0x20, otherwise x=1. (be compatible to general TPDU definition)
*/

#define RTICE_AT_CMD(x)      			RTICE_OP_CODE(RTICE_CMD_GROUP_ID_AT, x)
#define RTICE_AT_CMD_RTK	 			RTICE_OP_CODE(RTICE_CMD_GROUP_ID_AT, 0)   //0xA0


#define RTICE_AT_ERR_ABORT				RTICE_FUNC_ERR(0)
#define RTICE_AT_ERR_TIMEOUT			RTICE_FUNC_ERR(1)
#define RTICE_AT_ERR_INVALID_BUS_ID		RTICE_FUNC_ERR(2)
#define RTICE_AT_ERR_INVALID_ADDRESS	RTICE_FUNC_ERR(3)


#if 0 //deprecated
#define A0_TPDU_CMD_INDEX(cmd) ((cmd)+4)  // to inde
#define A0_TPDU_DATA(cmd) ((cmd)+8)
#define A0_R_TPDU_DATA(cmd) ((cmd)+4)
#else
#define AT_TPDU_CMD_INDEX(cmd) ((cmd)+4)  // to inde
#define AT_TPDU_DATA(cmd) ((cmd)+8)
#define AT_R_TPDU_DATA(cmd) ((cmd)+4)
#endif

/* for the purpose to reduce DMEM usage, that allocats parameter variables in DMEM for each seperate functions:
 * these APIs were designed to acquire the pointer of the unique container of RTICE protocolm packet,
 *   which should be transfered between caller and callee function.
 * while our system is a non-preemptive and no-reentant, it is safe in practice.
 */

/* why rtk_ice-at has this global variable, __pointer_to_at_cmd_param, seperated to other RTICE service functions ?
 * because rtk_ice-at has different method to fill payload of packet...
 */

// to registry the packet container with semantics meaning
#define AT_REIGSTRY_TPDU(pcmd) do { __pointer_to_rtice_cmd_param = (UINT8 *)(pcmd); } while(0)
#define AT_REIGSTRY_R_TPDU(pcmd) do { __pointer_to_rtice_cmd_param = (UINT8 *)(pcmd); } while(0)

// to get the packet container
#define AT_ACQUIRE_TPDU() (__pointer_to_rtice_cmd_param)
#define AT_ACQUIRE_R_TPDU() (__pointer_to_rtice_cmd_param)

// to registry the packet container with semantics meaning
#define RTICE_REIGSTRY_TPDU(pcmd) do { __pointer_to_rtice_cmd_param = (UINT8 *)(pcmd); } while(0)
#define RTICE_REIGSTRY_R_TPDU(pcmd) do { __pointer_to_rtice_cmd_param = (UINT8 *)(pcmd); } while(0)

// to get the packet container 
#define RTICE_ACQUIRE_TPDU() (__pointer_to_rtice_cmd_param)
#define RTICE_ACQUIRE_R_TPDU() (__pointer_to_rtice_cmd_param)

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************

#define at_r_tpdu_send(cmd, data_size) do { /* AT_REIGSTRY_TPDU(cmd); */  __at_r_tpdu_send__no_reentrant(data_size); } while(0)
void __at_r_tpdu_send__no_reentrant(UINT16 data_size);


void __rtice_nack__no_reentrant(UINT8 err);
void __rtice_ack__no_reentrant(UINT16 len);
void __rtice_pure_ack__no_reentrant(void);

#define rtice_nack(cmd, err)  do { RTICE_REIGSTRY_TPDU(cmd);  __rtice_nack__no_reentrant(err); } while(0)
#define rtice_ack(cmd, len)  do { RTICE_REIGSTRY_TPDU(cmd);  __rtice_ack__no_reentrant(len); } while(0)
#define rtice_pure_ack(cmd)  do { RTICE_REIGSTRY_TPDU(cmd);  __rtice_pure_ack__no_reentrant(); } while(0)


#endif
