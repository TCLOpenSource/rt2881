/******************************************************************************
 *
 *   Realtek Video driver
 *
 *   Copyright(c) 2014 Realtek Semiconductor Corp. All rights reserved.
 *
 *   @author FengYi Lu mail:a0945150@realtek.com
 *
 *****************************************************************************/


#ifndef _LINUX_RTKVDEC_H_
#define _LINUX_RTKVDEC_H_

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include <generated/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/list.h>
#include <linux/types.h>

#ifndef RTKVDEC_MAJOR
#define RTKVDEC_MAJOR 0 /* dynamic major by default */
#endif

#define RTKVDEC_SUSPEND           100
#define RTKVDEC_RESUME            101
#define RTKVDEC_RESET_PREPARE     102
#define RTKVDEC_RESET_DONE        103

#define DUMP_MEMORY_SIZE 1024*1024
#define DUMP_GET_MAX_SZIE 512*1024
#define DUMP_ES_SIZE 16*1024*1024

#define RTKVDEC_DCU1 0x0
#define RTKVDEC_DCU1_FIRST 0x1
#define RTKVDEC_DCU2 0x2
#define RTKVDEC_DCU2_FIRST 0x3
#define RTKVDEC_VCREATE_ALLOC 0x4
#define RTKVDEC_ENCODER_ALLOC 0x5
#define RTKVDEC_VBM_ALLOC 0x6
#define RTKVDEC_VBM_FREE  0x7
#define RTKVDEC_DCU1_LIMIT 0x8
#define RTKVDEC_DECIMATE_LUMA 0x9
#define RTKVDEC_VE_SRAM 0xa
#define RTKVDEC_VBM_DECIMATE 0xb
#define RTKVDEC_DCU2_FIRST_FLUSH 0xc
#define RTKVDEC_COMMON_ALLOC 0xff

#define RTKVDEC_PROC_DIR		"rtkvdec"
#define RTKVDEC_PROC_ENTRY      	"statistics"
#define RTKVDEC_MAX_CMD_LENGTH 256

#define RTKVDEC_MAX_SVPMEM_NUM 8
#define RTKVDEC_SVPMEM_SIZE_4M 0x400000
#define RTKVDEC_SVPMEM_SIZE_8M 0x800000
#define RTKVDEC_SVPMEM_SIZE_12M 0xc00000
#define RTKVDEC_SVPMEM_SIZE_16M 0x1000000
#define RTKVDEC_SVPMEM_SIZE_32M 0x2000000

#define RTKVDEC_MAX_VBM_MEM_NUM 20

#define RTKVDEC_MAX_IBBUF_NUM 4
#define RTKVDEC_IBBUF_SIZE 0x40000

/// please use "kill -l" on the board to select available signal number
#define RTKVDEC_SIG_NUM_CRC_IMPORT		62
#define RTKVDEC_SIG_NUM_CRC_EXPORT		63
#define RTKVDEC_SIG_NUM_DUMP_ES			64

/*
 *=====================================================
 * include video struct
 *=====================================================
 */

#if 0
/* TVE board type */
enum TVE_BOARD_TYPE {
	TVE_BOARD_CLASSIC_TYPE,
	TVE_BOARD_EXTERNAL_SCART_TYPE,
	TVE_BOARD_AVHDD_TYPE,
	TVE_BOARD_CUSTOM_1_TYPE,
	TVE_BOARD_CUSTOM_2_TYPE,
	TVE_BOARD_CUSTOM_3_TYPE,
	TVE_BOARD_CUSTOM_4_TYPE,
	TVE_BOARD_CUSTOM_5_TYPE,
	TVE_BOARD_CUSTOM_6_TYPE,
	TVE_BOARD_CUSTOM_7_TYPE,
	TVE_BOARD_CUSTOM_8_TYPE,
	TVE_BOARD_CUSTOM_9_TYPE
} ;
typedef enum TVE_BOARD_TYPE TVE_BOARD_TYPE;


struct VIDEO_INIT_DATA {
	enum TVE_BOARD_TYPE boardType ;
};
typedef struct VIDEO_INIT_DATA VIDEO_INIT_DATA;

#endif

typedef struct {
	int filterType ;
	int handle ;
	int dst_filterType ;
	int dst_handle ;
} VDEC_FLOW_CMD_T ;

typedef enum {
	SET_BS_RB = 0,  	/* Init bitstream  ring buffer */
	SET_CMD_RB,     	/* Init Inband CMD ring buffer */
	SET_CC_RB,      	/* Init CC         ring buffer */
	SET_REFCLOCK,   	/* Set refClock */
	SET_DECODEMODE, 	/* Set Decode Mode */
	SET_SPEED       	/* Set Speed */
} VDEC_SET_T;

typedef struct {
	VDEC_SET_T      eType ;
	int     	Data ;
	int     	Handle ;
} VDEC_SET_CMD_T ;

#if 0
struct VIDEO_RPC_DEBUG_MEMORY {
	long videoFirmwareVersion ;
} ;
typedef struct VIDEO_RPC_DEBUG_MEMORY VIDEO_RPC_DEBUG_MEMORY;
#endif

#define ULong 	uint64_t

typedef struct DEBUG_BUFFER_HEADER {
	ULong magic;
	ULong size;
	ULong rd;
	ULong wr;
} DEBUG_BUFFER_HEADER;

typedef struct VDEC_FRAMEINFO_STRUCT {
	pid_t pid ;
	unsigned int frame_dropped ;
	unsigned int frame_displaying ;
	unsigned int frame_matched ;
	unsigned int frame_freerun ;
} VDEC_FRAMEINFO_STRUCT;

typedef struct VDEC_FRAMEINFO_LIST {
	VDEC_FRAMEINFO_STRUCT frame_info ;
	struct list_head buffer_list;
} VDEC_FRAMEINFO_LIST;

typedef struct VDEC_SVPMEM_STRUCT {
	pid_t pid ;
	unsigned int addr ;
	unsigned int size ;
} VDEC_SVPMEM_STRUCT;

typedef struct VDEC_DVR_MALLOC_STRUCT {
	unsigned long Memory;
	unsigned long PhyAddr ;
	unsigned long VirtAddr ;
} VDEC_DVR_MALLOC_STRUCT;

typedef struct VDEC_VBM_MEM_LIST {
	unsigned int used;
	unsigned long addr ;
	unsigned int size ;
#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
	unsigned long aligned_addr;
	unsigned int aligned_size;
#endif
} VDEC_VBM_MEM_LIST;

typedef enum {
	VDEC_SVP_BUF = 0,
	VDEC_INBAND_BUF
} VDEC_COBUF_TYPE;

typedef enum {
	VDEC_DUMP_DEFAULT = -1,
	VDEC_DUMP_LOG,
	VDEC_DUMP_SET_USER_TASK,
	VDEC_DUMP_RESET_USER_TASK,
	VDEC_DUMP_SET_MMAP_USER_CTX,
	VDEC_DUMP_RESET_MMAP_USER_CTX,
	VDEC_DUMP_FILE_PATH,
	VDEC_DUMP_IMPORT_FILE_SIZE,
	VDEC_DUMP_IMPORT_FILE_LOAD,
	VDEC_DUMP_DBG_BUF_HDR,
	VDEC_DUMP_ES_SIG_START,
	VDEC_DUMP_ES_SIG_STOP,
	VDEC_DUMP_CRC_EXPORT_SIG_START,
	VDEC_DUMP_CRC_EXPORT_SIG_STOP,
	VDEC_DUMP_CRC_IMPORT_SIG_START,
	VDEC_DUMP_CRC_IMPORT_SIG_STOP,
} VDEC_DUMP_CMD;

typedef struct VDEC_DUMP {
	VDEC_DUMP_CMD cmd;
	unsigned int dataAddr;
	unsigned int dataSize;
} VDEC_DUMP;

typedef enum {
	VDEC_DUMP_SIG_DEFAULT = -1,
	VDEC_DUMP_SIG_FILE_OPEN,
	VDEC_DUMP_SIG_FILE_LOAD,
	VDEC_DUMP_SIG_FILE_CLOSE,
} VDEC_DUMP_SIG_CMD;

/* Use 'v' as magic number */
#define VDEC_IOC_MAGIC  'v'
#define VDEC_IOC_INIT       _IO  (VDEC_IOC_MAGIC, 1)
#define VDEC_IOC_DEINIT     _IO  (VDEC_IOC_MAGIC, 2)
#define VDEC_IOC_OPEN       _IOWR(VDEC_IOC_MAGIC, 3, int)
#define VDEC_IOC_CLOSE      _IOW (VDEC_IOC_MAGIC, 4, int)
#define VDEC_IOC_PLAY       _IOW (VDEC_IOC_MAGIC, 5, int)
#define VDEC_IOC_PAUSE      _IOW (VDEC_IOC_MAGIC, 6, int)
#define VDEC_IOC_FLUSH      _IOW (VDEC_IOC_MAGIC, 7, int)
#define VDEC_IOC_STOP       _IOW (VDEC_IOC_MAGIC, 8, int)
#define VDEC_IOC_CONNECT    _IOW (VDEC_IOC_MAGIC, 9, int)
#define VDEC_IOC_SET        _IOW (VDEC_IOC_MAGIC,10, int)
#define VDEC_IOC_GET        _IOR (VDEC_IOC_MAGIC,11, int)
#define VDEC_IOC_ENVDBG     _IOW (VDEC_IOC_MAGIC,12, VDEC_DBG_STRUCT)
#define VDEC_IOC_DISVDBG    _IO  (VDEC_IOC_MAGIC,13)
#define VDEC_IOC_FRAMEINFO  _IOW (VDEC_IOC_MAGIC,14, int)
#define VDEC_IOC_ALLOCSVPM  _IOWR(VDEC_IOC_MAGIC,15, int)
#define VDEC_IOC_FREESVPM   _IOW (VDEC_IOC_MAGIC,16, int)
#define VDEC_IOC_ALLOC      _IOW (VDEC_IOC_MAGIC,17, int)
#define VDEC_IOC_FREE       _IOW (VDEC_IOC_MAGIC,18, int)
#define VDEC_IOC_HALLOG     _IOW (VDEC_IOC_MAGIC,19, int)
#define VDEC_IOC_ALLOC_IBBUF  _IOWR(VDEC_IOC_MAGIC,20, int)
#define VDEC_IOC_FREE_IBBUF   _IOW (VDEC_IOC_MAGIC,21, int)
#define VDEC_IOC_QVCDONE    _IO  (VDEC_IOC_MAGIC,22)
#define VDEC_IOC_ALLOCSVPMP _IOWR(VDEC_IOC_MAGIC,24, int)
#define VDEC_IOC_QUERY_NOV  _IO  (VDEC_IOC_MAGIC,28)
#define VDEC_IOC_DUMP     	_IOWR(VDEC_IOC_MAGIC,100, VDEC_DUMP)

void rtkvdec_vtm_state(void) ;
void rtkvdec_send_sharedmemory(void) ;
void rtkvdec_show_sharedmemory(void) ;
int rtkvdec_cobuffer_alloc(VDEC_COBUF_TYPE type, int port) ;
int rtkvdec_cobuffer_free (VDEC_COBUF_TYPE type, unsigned int phy_addr) ;
//void rtkvdec_DCMTCallbackFunction(void);

/* end of _LINUX_RTKVDEC_H_ */
#endif
