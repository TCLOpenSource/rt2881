
#ifndef BUILD_QUICK_SHOW
#include <linux/version.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/dma-buf.h>

#include <rtk_kdriver/rmm/rmm.h>
#include <rtk_kdriver/io.h>
#include <rtk_kdriver/quick_show/quick_show.h>
#include <linux/auth.h>
#include <linux/pageremap.h>

#include <mach/iomap.h>
#include <mach/common.h>
#include <mach/rtk_platform.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#else
#include <mach/rtk_platform.h>
#include <no_os/printk.h>
#include <malloc.h>
#endif

#include <rtk_kdriver/rtk_qos_export.h>
#include <rtd_log/rtd_module_log.h>
#include <rtk_kdriver/rmm/rmm_common.h>
#include <rbus/dc_mc_reg.h>
#include <rbus/dc_sys_reg.h>

unsigned long DRAM_size = 0;
EXPORT_SYMBOL(DRAM_size);

#ifdef ENABLE_DC_API_SUPPORT
RTK_IB_REGION rtk_ib_table[RTK_IB_REGION_TOTAL_NUM];
RTK_DC_INFO_t rtk_global_dram_info;
#endif


#ifndef BUILD_QUICK_SHOW
#if defined(CONFIG_REALTEK_MEMORY_MANAGEMENT)
#define LAST_IMAGE_SIZE 4*1024*1024
extern phys_addr_t __initdata reserved_dvr_start;
extern phys_addr_t __initdata reserved_dvr_size;
extern phys_addr_t __initdata reserved_last_image_start;
extern phys_addr_t __initdata reserved_last_image_size;
extern void *pAnimation;
extern void *pAnimation_1;
extern void *pLastImage;
#endif
#endif

/**
   list by address order,
   {addr, size}
 */
#define _TBD_ 0
unsigned long carvedout_buf[MM_LAYOUT_CARVEDOUT_NUM][CARVEDOUT_NUM][2] = {
  // MM_LAYOUT_DEFAULT
  {
	{0x00000000,  1.5*_MB_},							// CARVEDOUT_BOOTCODE
	{DEMOD_CARVED_OUT,  8*_MB_},					// CARVEDOUT_DEMOD
	{0x10000000,  0*_MB_},							// CARVEDOUT_AV_DMEM
	{0x12280000,  0*_MB_/*17920*_KB_*/},			// CARVEDOUT_VDEC_COMEM
	{AUDIO_BUFFER_START, AUDIO_BUFFER_SIZE},		// CARVEDOUT_AUDIO
	{0x15f00000,  0*_MB_},							// CARVEDOUT_K_BOOT
	{0x16000000, 18*_MB_},							// CARVEDOUT_K_OS (optee)
	{0x17c00000, 4*_MB_},							// CARVEDOUT_MAP_GIC
	{RBUS_BASE_PHYS, RBUS_BASE_SIZE},				// CARVEDOUT_MAP_RBUS
	{0x1a500000 - RTK_GPU_FW_SIZE, RTK_GPU_FW_SIZE}, // CARVEDOUT_GPU_FW
	{0x1a900000, 8*_MB_},							// CARVEDOUT_V_OS
	{0x1b180000,  0x00080000},						// CARVEDOUT_MAP_RPC
	{0x1b200000, 13*_MB_},							// CARVEDOUT_A_OS
	{CONST_LOGBUF_MEM_ADDR_START, CONST_LOGBUF_MEM_SIZE}, // CARVEDOUT_LOGBUF
	{0x1fc00000, 0x00008000},						// CARVEDOUT_ROMCODE
	{0x1fd00000, 0x010000},							// CARVEDOUT_RAMOOPS
	{0x1fff8000, 0x00002000},						// CARVEDOUT_IR_TABLE
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY_2
	{SVP_CPB_START,  SVP_CPB_SIZE},					// CARVEDOUT_VDEC_RINGBUF
	{GAL_MEM_ADDR_START, GAL_MEM_SIZE},				// CARVEDOUT_GAL
	{_TBD_, _TBD_},									// CARVEDOUT_SNAPSHOT
	{SCLAER_MODULE_START, SCALER_MODULE_SIZE},		// CARVEDOUT_SCALER
	{SCALER_MEMC_START, SCALER_MEMC_SIZE},			// CARVEDOUT_SCALER_MEMC
	{SCALER_MDOMAIN_START, SCALER_MDOMAIN_SIZE},	// CARVEDOUT_SCALER_MDOMAIN
	{SCALER_DI_NR_START, SCALER_DI_NR_SIZE},		// CARVEDOUT_SCALER_DI_NR
	{SCALER_NN_START, SCALER_NN_SIZE},				// CARVEDOUT_SCALER_NN
	{SCALER_VIP_START, SCALER_VIP_SIZE},			// CARVEDOUT_SCALER_VIP
	{SCALER_OD_START, SCALER_OD_SIZE},				// CARVEDOUT_SCALER_OD
#if !defined(CONFIG_VBM_CMA)
	{VDEC_BUFFER_START, VDEC_BUFFER_SIZE},			// CARVEDOUT_VDEC_VBM
#else
	{0, _TBD_},                                     // CARVEDOUT_VDEC_VBM
#endif
	{DEMUX_TP_BUFFER_START, DEMUX_TP_BUFFER_SIZE},	// CARVEDOUT_TP

    // for desired cma size calculation
	{0x09000000, 112*_MB_},							// CARVEDOUT_CMA_LOW
	{0, _TBD_},										// CARVEDOUT_CMA_HIGH
	{0, _TBD_},										// CARVEDOUT_CMA_GPU_4K
	{0, _TBD_},										// CARVEDOUT_CMA_3
	{0, _TBD_},										// CARVEDOUT_CMA_LOW_LIMIT
	{0, _TBD_},										// CARVEDOUT_CMA_BW
	{VDEC_BUFFER_START, VDEC_BUFFER_SIZE},			// CARVEDOUT_CMA_VBM

	{0, _TBD_},										// CARVEDOUT_GPU_RESERVED
  },

  // MM_LAYOUT_1G5
  {
	{0x00000000,  1.5*_MB_},							// CARVEDOUT_BOOTCODE
	{DEMOD_CARVED_OUT,  8*_MB_},					// CARVEDOUT_DEMOD
	{0x10000000,  0*_MB_},							// CARVEDOUT_AV_DMEM
	{0x12280000,  0*_MB_/*17920*_KB_*/},			// CARVEDOUT_VDEC_COMEM
	{AUDIO_BUFFER_START, AUDIO_BUFFER_SIZE},		// CARVEDOUT_AUDIO
	{0x15f00000,  0*_MB_},							// CARVEDOUT_K_BOOT
	{0x16000000, 18*_MB_},							// CARVEDOUT_K_OS (optee)
	{0x17c00000, 4*_MB_},							// CARVEDOUT_MAP_GIC
	{RBUS_BASE_PHYS, RBUS_BASE_SIZE},				// CARVEDOUT_MAP_RBUS
	{0x1a500000 - RTK_GPU_FW_SIZE, RTK_GPU_FW_SIZE}, // CARVEDOUT_GPU_FW
	{0x1a900000, 8*_MB_},							// CARVEDOUT_V_OS
	{0x1b180000,  0x00080000},						// CARVEDOUT_MAP_RPC
	{0x1b200000, 13*_MB_},							// CARVEDOUT_A_OS
	{DDR_1G5_CONST_LOGBUF_MEM_ADDR_START, DDR_1G5_CONST_LOGBUF_MEM_SIZE}, // CARVEDOUT_LOGBUF
	{0x1fc00000, 0x00008000},						// CARVEDOUT_ROMCODE
	{0x1fd00000, 0x010000},							// CARVEDOUT_RAMOOPS
	{0x1fff8000, 0x00002000},						// CARVEDOUT_IR_TABLE
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY_2
	{DDR_1G5_SVP_CPB_START,  DDR_1G5_SVP_CPB_SIZE}, // CARVEDOUT_VDEC_RINGBUF
	{DDR_1G5_GAL_MEM_ADDR_START, DDR_1G5_GAL_MEM_SIZE},			// CARVEDOUT_GAL
	{_TBD_, _TBD_},												// CARVEDOUT_SNAPSHOT
	{DDR_1G5_SCLAER_MODULE_START, DDR_1G5_SCALER_MODULE_SIZE},	// CARVEDOUT_SCALER
	{DDR_1G5_SCALER_MEMC_START, DDR_1G5_SCALER_MEMC_SIZE},		// CARVEDOUT_SCALER_MEMC
	{DDR_1G5_SCALER_MDOMAIN_START, DDR_1G5_SCALER_MDOMAIN_SIZE}, // CARVEDOUT_SCALER_MDOMAIN
	{DDR_1G5_SCALER_DI_NR_START, DDR_1G5_SCALER_DI_NR_SIZE},	// CARVEDOUT_SCALER_DI_NR
	{DDR_1G5_SCALER_NN_START, DDR_1G5_SCALER_NN_SIZE},			// CARVEDOUT_SCALER_NN
	{DDR_1G5_SCALER_VIP_START, DDR_1G5_SCALER_VIP_SIZE},		// CARVEDOUT_SCALER_VIP
	{DDR_1G5_SCALER_OD_START, DDR_1G5_SCALER_OD_SIZE},			// CARVEDOUT_SCALER_OD
#if !defined(CONFIG_VBM_CMA)
	{DDR_1G5_VDEC_BUFFER_START, DDR_1G5_VDEC_BUFFER_SIZE},		// CARVEDOUT_VDEC_VBM
#else
	{0, _TBD_},                                                 // CARVEDOUT_VDEC_VBM
#endif
	{DDR_1G5_DEMUX_TP_BUFFER_START, DDR_1G5_DEMUX_TP_BUFFER_SIZE}, // CARVEDOUT_TP

    // for desired cma size calculation
	{0x09000000, 112*_MB_},							// CARVEDOUT_CMA_LOW
	{0, _TBD_}, 									// CARVEDOUT_CMA_HIGH
	{0, _TBD_},										// CARVEDOUT_CMA_GPU_4K
	{0, _TBD_},										// CARVEDOUT_CMA_3
	{0, _TBD_},										// CARVEDOUT_CMA_LOW_LIMIT
	{0, _TBD_},										// CARVEDOUT_CMA_BW
	{DDR_1G5_VDEC_BUFFER_START, DDR_1G5_VDEC_BUFFER_SIZE},		// CARVEDOUT_CMA_VBM

	{0, _TBD_},										// CARVEDOUT_GPU_RESERVED
  },

  // MM_LAYOUT_1GB
  {
	{0x00000000,  1.5*_MB_},							// CARVEDOUT_BOOTCODE
	{DEMOD_CARVED_OUT,  8*_MB_},					// CARVEDOUT_DEMOD
	{0x10000000,  0*_MB_},							// CARVEDOUT_AV_DMEM
	{0x12280000,  0*_MB_/*17920*_KB_*/},			// CARVEDOUT_VDEC_COMEM
	{AUDIO_BUFFER_START, AUDIO_BUFFER_SIZE},		// CARVEDOUT_AUDIO
	{0x15f00000,  0*_MB_},							// CARVEDOUT_K_BOOT
	{0x16000000, 16*_MB_},							// CARVEDOUT_K_OS (optee)
	{0x17c00000, 4*_MB_},							// CARVEDOUT_MAP_GIC
	{RBUS_BASE_PHYS, RBUS_BASE_SIZE},				// CARVEDOUT_MAP_RBUS
	{0x1a500000 - RTK_GPU_FW_SIZE, RTK_GPU_FW_SIZE}, // CARVEDOUT_GPU_FW
	{0x1a900000, 8*_MB_},							// CARVEDOUT_V_OS
	{0x1b180000,  0x00080000},						// CARVEDOUT_MAP_RPC
	{0x1b200000, 13*_MB_},							// CARVEDOUT_A_OS
	{DDR_1GB_CONST_LOGBUF_MEM_ADDR_START, DDR_1GB_CONST_LOGBUF_MEM_SIZE}, // CARVEDOUT_LOGBUF
	{0x1fc00000, 0x00008000},						// CARVEDOUT_ROMCODE
	{0x1fd00000, 0x010000},							// CARVEDOUT_RAMOOPS
	{0x1fff8000, 0x00002000},						// CARVEDOUT_IR_TABLE
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY_2
	{DDR_1GB_SVP_CPB_START,  DDR_1GB_SVP_CPB_SIZE}, // CARVEDOUT_VDEC_RINGBUF
	{DDR_1GB_GAL_MEM_ADDR_START, DDR_1GB_GAL_MEM_SIZE}, // CARVEDOUT_GAL
	{_TBD_, _TBD_},									// CARVEDOUT_SNAPSHOT
	{DDR_1GB_SCLAER_MODULE_START, DDR_1GB_SCALER_MODULE_SIZE},	// CARVEDOUT_SCALER
	{DDR_1GB_SCALER_MEMC_START, DDR_1GB_SCALER_MEMC_SIZE},		// CARVEDOUT_SCALER_MEMC
	{DDR_1GB_SCALER_MDOMAIN_START, DDR_1GB_SCALER_MDOMAIN_SIZE}, // CARVEDOUT_SCALER_MDOMAIN
	{DDR_1GB_SCALER_DI_NR_START, DDR_1GB_SCALER_DI_NR_SIZE},	// CARVEDOUT_SCALER_DI_NR
	{DDR_1GB_SCALER_NN_START, DDR_1GB_SCALER_NN_SIZE},			// CARVEDOUT_SCALER_NN
	{DDR_1GB_SCALER_VIP_START, DDR_1GB_SCALER_VIP_SIZE},		// CARVEDOUT_SCALER_VIP
	{DDR_1GB_SCALER_OD_START, DDR_1GB_SCALER_OD_SIZE},			// CARVEDOUT_SCALER_OD
#if !defined(CONFIG_VBM_CMA)
	{DDR_1GB_VDEC_BUFFER_START, DDR_1GB_VDEC_BUFFER_SIZE},		// CARVEDOUT_VDEC_VBM
#else
	{0, _TBD_},                                                 // CARVEDOUT_VDEC_VBM
#endif
	{DDR_1GB_DEMUX_TP_BUFFER_START, DDR_1GB_DEMUX_TP_BUFFER_SIZE}, // CARVEDOUT_TP

    // for desired cma size calculation
	{0x09000000, 112*_MB_},							// CARVEDOUT_CMA_LOW
	{0, _TBD_},										// CARVEDOUT_CMA_HIGH
	{0, _TBD_},										// CARVEDOUT_CMA_GPU_4K
	{0, _TBD_},										// CARVEDOUT_CMA_3
	{0, _TBD_},										// CARVEDOUT_CMA_LOW_LIMIT
	{0, _TBD_},										// CARVEDOUT_CMA_BW
	{DDR_1GB_VDEC_BUFFER_START, DDR_1GB_VDEC_BUFFER_SIZE},		// CARVEDOUT_CMA_VBM

	{0, _TBD_},										// CARVEDOUT_GPU_RESERVED
  },

  // MM_LAYOUT_M7_8K
  {
	{0x00000000,  1.5*_MB_},							// CARVEDOUT_BOOTCODE
	{DEMOD_CARVED_OUT,  8*_MB_},					// CARVEDOUT_DEMOD
	{0x10000000,  0*_MB_},							// CARVEDOUT_AV_DMEM
	{0x12280000,  0*_MB_/*17920*_KB_*/},			// CARVEDOUT_VDEC_COMEM
	{AUDIO_BUFFER_START, AUDIO_BUFFER_SIZE},		// CARVEDOUT_AUDIO
	{0x15f00000,  0*_MB_},							// CARVEDOUT_K_BOOT
	{0x16000000, 18*_MB_},							// CARVEDOUT_K_OS (optee)
	{0x17c00000, 4*_MB_},							// CARVEDOUT_MAP_GIC
	{RBUS_BASE_PHYS, RBUS_BASE_SIZE},				// CARVEDOUT_MAP_RBUS
	{0x1a500000 - RTK_GPU_FW_SIZE, RTK_GPU_FW_SIZE}, // CARVEDOUT_GPU_FW
	{0x1a900000, 8*_MB_},							// CARVEDOUT_V_OS
	{0x1b180000,  0x00080000},						// CARVEDOUT_MAP_RPC
	{0x1b200000, 13*_MB_},							// CARVEDOUT_A_OS
	{M7_8K_CONST_LOGBUF_MEM_ADDR_START, M7_8K_CONST_LOGBUF_MEM_SIZE}, // CARVEDOUT_LOGBUF
	{0x1fc00000, 0x00008000},						// CARVEDOUT_ROMCODE
	{0x1fd00000, 0x010000},							// CARVEDOUT_RAMOOPS
	{0x1fff8000, 0x00002000},						// CARVEDOUT_IR_TABLE
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY_2
	{M7_8K_SVP_CPB_START,  M7_8K_SVP_CPB_SIZE},		// CARVEDOUT_VDEC_RINGBUF
	{M7_8K_GAL_MEM_ADDR_START, M7_8K_GAL_MEM_SIZE}, // CARVEDOUT_GAL
	{_TBD_, _TBD_},									// CARVEDOUT_SNAPSHOT
	{M7_8K_SCLAER_MODULE_START, M7_8K_SCALER_MODULE_SIZE},		// CARVEDOUT_SCALER
	{M7_8K_SCALER_MEMC_START, M7_8K_SCALER_MEMC_SIZE},			// CARVEDOUT_SCALER_MEMC
	{M7_8K_SCALER_MDOMAIN_START, M7_8K_SCALER_MDOMAIN_SIZE},	// CARVEDOUT_SCALER_MDOMAIN
	{M7_8K_SCALER_DI_NR_START, M7_8K_SCALER_DI_NR_SIZE},		// CARVEDOUT_SCALER_DI_NR
	{M7_8K_SCALER_NN_START, M7_8K_SCALER_NN_SIZE},				// CARVEDOUT_SCALER_NN
	{M7_8K_SCALER_VIP_START, M7_8K_SCALER_VIP_SIZE},			// CARVEDOUT_SCALER_VIP
	{M7_8K_SCALER_OD_START, M7_8K_SCALER_OD_SIZE},				// CARVEDOUT_SCALER_OD
	{M7_8K_VDEC_BUFFER_START, M7_8K_VDEC_BUFFER_SIZE},			// CARVEDOUT_VDEC_VBM
	{M7_8K_DEMUX_TP_BUFFER_START, M7_8K_DEMUX_TP_BUFFER_SIZE},	// CARVEDOUT_TP

    // for desired cma size calculation
	{0x09000000, 112*_MB_},							// CARVEDOUT_CMA_LOW
	{0, _TBD_},										// CARVEDOUT_CMA_HIGH
	{0, _TBD_},										// CARVEDOUT_CMA_GPU_4K
	{0, _TBD_},										// CARVEDOUT_CMA_3
	{0, _TBD_},										// CARVEDOUT_CMA_LOW_LIMIT
	{0, _TBD_},										// CARVEDOUT_CMA_BW
	{_TBD_,	_TBD_},									// CARVEDOUT_CMA_VBM

	{0, _TBD_},										// CARVEDOUT_GPU_RESERVED
  },

  // MM_LAYOUT_DEFAULT_NC
  {
	{0x00000000,  1.5*_MB_},							// CARVEDOUT_BOOTCODE
	{DEMOD_CARVED_OUT,  0*_MB_},					// CARVEDOUT_DEMOD
	{0x10000000,  0*_MB_},							// CARVEDOUT_AV_DMEM
	{0x12280000,  0*_MB_/*17920*_KB_*/},			// CARVEDOUT_VDEC_COMEM
	{AUDIO_BUFFER_START, AUDIO_BUFFER_SIZE},		// CARVEDOUT_AUDIO
	{0x15f00000,  0*_MB_},							// CARVEDOUT_K_BOOT
	{0x16000000, 18*_MB_},							// CARVEDOUT_K_OS (optee)
	{0x17c00000, 4*_MB_},							// CARVEDOUT_MAP_GIC
	{RBUS_BASE_PHYS, RBUS_BASE_SIZE},				// CARVEDOUT_MAP_RBUS
	{0x1a500000 - RTK_GPU_FW_SIZE, RTK_GPU_FW_SIZE}, // CARVEDOUT_GPU_FW
	{0x1a900000, 8*_MB_},							// CARVEDOUT_V_OS
	{0x1b180000,  0x00080000},						// CARVEDOUT_MAP_RPC
	{0x1b200000, 13*_MB_},							// CARVEDOUT_A_OS
	{CONST_LOGBUF_MEM_ADDR_START, 512*_KB_}, // CARVEDOUT_LOGBUF
	{0x1fc00000, 0x00008000},						// CARVEDOUT_ROMCODE
	{0x1fd00000, 0x010000},							// CARVEDOUT_RAMOOPS
	{0x1fff8000, 0x00002000},						// CARVEDOUT_IR_TABLE
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY_2
	{NC_SVP_CPB_START,  NC_SVP_CPB_SIZE},					// CARVEDOUT_VDEC_RINGBUF
	{GAL_MEM_ADDR_START, GAL_MEM_SIZE},				// CARVEDOUT_GAL
	{_TBD_, _TBD_},									// CARVEDOUT_SNAPSHOT
	{NC_SCLAER_MODULE_START, NC_SCALER_MODULE_SIZE},		// CARVEDOUT_SCALER
	{NC_SCALER_MEMC_START, NC_SCALER_MEMC_SIZE},			// CARVEDOUT_SCALER_MEMC
	{NC_SCALER_MDOMAIN_START, NC_SCALER_MDOMAIN_SIZE},	// CARVEDOUT_SCALER_MDOMAIN
	{NC_SCALER_DI_NR_START, NC_SCALER_DI_NR_SIZE},		// CARVEDOUT_SCALER_DI_NR
	{NC_SCALER_NN_START, NC_SCALER_NN_SIZE},				// CARVEDOUT_SCALER_NN
	{NC_SCALER_VIP_START, NC_SCALER_VIP_SIZE},			// CARVEDOUT_SCALER_VIP
	{NC_SCALER_OD_START, NC_SCALER_OD_SIZE},				// CARVEDOUT_SCALER_OD
#if !defined(CONFIG_VBM_CMA)
	{NC_VDEC_BUFFER_START, NC_VDEC_BUFFER_SIZE},			// CARVEDOUT_VDEC_VBM
#else
	{0, _TBD_},                                     // CARVEDOUT_VDEC_VBM
#endif
	{NC_DEMUX_TP_BUFFER_START, NC_DEMUX_TP_BUFFER_SIZE},	// CARVEDOUT_TP

    // for desired cma size calculation
	{0x09000000, 112*_MB_},							// CARVEDOUT_CMA_LOW
	{0, _TBD_},										// CARVEDOUT_CMA_HIGH
	{0, _TBD_},										// CARVEDOUT_CMA_GPU_4K
	{0, _TBD_},										// CARVEDOUT_CMA_3
	{0, _TBD_},										// CARVEDOUT_CMA_LOW_LIMIT
	{0, _TBD_},										// CARVEDOUT_CMA_BW
	{NC_VDEC_BUFFER_START, NC_VDEC_BUFFER_SIZE},			// CARVEDOUT_CMA_VBM

	{0, _TBD_},										// CARVEDOUT_GPU_RESERVED
  },

  // MM_LAYOUT_1G5_NC noncarveout
  {
	{0x00000000,  1.5*_MB_},							// CARVEDOUT_BOOTCODE
	{DEMOD_CARVED_OUT,  0*_MB_},					// CARVEDOUT_DEMOD
	{0x10000000,  0*_MB_},							// CARVEDOUT_AV_DMEM
	{0x12280000,  0*_MB_/*17920*_KB_*/},			// CARVEDOUT_VDEC_COMEM
	{AUDIO_BUFFER_START, AUDIO_BUFFER_SIZE},		// CARVEDOUT_AUDIO
	{0x15f00000,  0*_MB_},							// CARVEDOUT_K_BOOT
	{0x16000000, 18*_MB_},							// CARVEDOUT_K_OS (optee)
	{0x17c00000, 4*_MB_},							// CARVEDOUT_MAP_GIC
	{RBUS_BASE_PHYS, RBUS_BASE_SIZE},				// CARVEDOUT_MAP_RBUS
	{0x1a500000 - RTK_GPU_FW_SIZE, RTK_GPU_FW_SIZE}, // CARVEDOUT_GPU_FW
	{0x1a900000, 8*_MB_},							// CARVEDOUT_V_OS
	{0x1b180000,  0x00080000},						// CARVEDOUT_MAP_RPC
	{0x1b200000, 13*_MB_},							// CARVEDOUT_A_OS
	{NC_1G5_CONST_LOGBUF_MEM_ADDR_START, NC_1G5_CONST_LOGBUF_MEM_SIZE}, // CARVEDOUT_LOGBUF
	{0x1fc00000, 0x00008000},						// CARVEDOUT_ROMCODE
	{0x1fd00000, 0x010000},							// CARVEDOUT_RAMOOPS
	{0x1fff8000, 0x00002000},						// CARVEDOUT_IR_TABLE
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY
	{_TBD_, _TBD_},									// CARVEDOUT_DDR_BOUNDARY_2
	{NC_1G5_SVP_CPB_START,  NC_1G5_SVP_CPB_SIZE}, // CARVEDOUT_VDEC_RINGBUF
	{NC_1G5_GAL_MEM_ADDR_START, NC_1G5_GAL_MEM_SIZE},			// CARVEDOUT_GAL
	{_TBD_, _TBD_},												// CARVEDOUT_SNAPSHOT
	{NC_1G5_SCLAER_MODULE_START, NC_1G5_SCALER_MODULE_SIZE},	// CARVEDOUT_SCALER
	{NC_1G5_SCALER_MEMC_START, NC_1G5_SCALER_MEMC_SIZE},		// CARVEDOUT_SCALER_MEMC
	{NC_1G5_SCALER_MDOMAIN_START, NC_1G5_SCALER_MDOMAIN_SIZE}, // CARVEDOUT_SCALER_MDOMAIN
	{NC_1G5_SCALER_DI_NR_START, NC_1G5_SCALER_DI_NR_SIZE},	// CARVEDOUT_SCALER_DI_NR
	{NC_1G5_SCALER_NN_START, NC_1G5_SCALER_NN_SIZE},			// CARVEDOUT_SCALER_NN
	{NC_1G5_SCALER_VIP_START, NC_1G5_SCALER_VIP_SIZE},		// CARVEDOUT_SCALER_VIP
	{NC_1G5_SCALER_OD_START, NC_1G5_SCALER_OD_SIZE},			// CARVEDOUT_SCALER_OD
#if !defined(CONFIG_VBM_CMA)
	{NC_1G5_VDEC_BUFFER_START, NC_1G5_VDEC_BUFFER_SIZE},		// CARVEDOUT_VDEC_VBM
#else
	{0, _TBD_},                                                 // CARVEDOUT_VDEC_VBM
#endif
	{NC_1G5_DEMUX_TP_BUFFER_START, NC_1G5_DEMUX_TP_BUFFER_SIZE}, // CARVEDOUT_TP

    // for desired cma size calculation
	{0x09000000, 112*_MB_},							// CARVEDOUT_CMA_LOW
	{0, _TBD_}, 									// CARVEDOUT_CMA_HIGH
	{0, _TBD_},										// CARVEDOUT_CMA_GPU_4K
	{0, _TBD_},										// CARVEDOUT_CMA_3
	{0, _TBD_},										// CARVEDOUT_CMA_LOW_LIMIT
	{0, _TBD_},										// CARVEDOUT_CMA_BW
	{NC_1G5_VDEC_BUFFER_START, NC_1G5_VDEC_BUFFER_SIZE},		// CARVEDOUT_CMA_VBM

	{0, _TBD_},										// CARVEDOUT_GPU_RESERVED
  },

};
EXPORT_SYMBOL(carvedout_buf);

#ifdef ENABLE_DC_API_SUPPORT

static unsigned int __rtk_dc_get_small_dram_size(void)
{
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	if (dc_sys_misc.small_dram_4g)
		return 0x20000000; // 512MB
	if (dc_sys_misc.small_dram_2g)
		return 0x10000000; // 256MB
	if (dc_sys_misc.small_dram_1g)
		return 0x8000000; // 128MB
	if (dc_sys_misc.small_dram_512m)
		return 0x4000000; // 64MB

	return 0;
}

static unsigned int __rtk_dc_get_small_dram_num(void)
{
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	switch(dc_sys_misc.small_dram_num){
		case 0:
			return 2; // 2 small dram
		case 1:
			return 1; // 1 small dram
		default:
			return 0; // no small dram
	}

	return 0;
}

static unsigned int __rtk_dc_get_dram_num(void)
{
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	switch(dc_sys_misc.mem_num){
		case 0:
			return 1; // 1-ddr
		case 1:
			return 2; // 2-ddr
		case 2:
			return 3; // 3-ddr
		default:
			return 0; // none
	}

	return 0;
}

static unsigned int __rtk_dc_is_swapped(void)
{
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	if (dc_sys_misc.en_full_ddr_last == 1)
		return 1; // swapped
	else
		return 0; // non-swapped
}

static unsigned int __rtk_dc_offset_is_sw_mode(void)
{
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	if (dc_sys_misc.dc_offset_3ddr == 0) // without offset
		return 1; // sw mode
	else
		return 0; // hw mode
}

/*
 * 3ddr with 1 small dram
 * +------------+-------+
 * | small_size | delta |
 * +------------+-------+
 * |     4G     |  64   |
 * |     2G     |  32   |
 * |     1G     |  64   |
 * |   512M     |  32   |
 * +------------+-------+
 *
 * 3ddr with 2 small dram
 * +------------+-------+
 * | small_size | delta |
 * +------------+-------+
 * |     4G     |  32   |
 * |     2G     |  64   |
 * |     1G     |  32   |
 * |   512M     |  64   |
 * +------------+-------+
 */
static unsigned int __rtk_dc_get_sw_mode_offset(void)
{
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	if (dc_sys_misc.mem_num != 2) // dram number is not 3-ddr
		return 0;

	if (dc_sys_misc.en_ddr_diff_size == 0) // dram size is the same
		return 0;

	if (dc_sys_misc.en_full_ddr_last == 0) // dram is not swapped
		return 0;

	if (dc_sys_misc.dc_offset_3ddr != 0) // hw offset has applied
		return 0;

	if (dc_sys_misc.small_dram_num == 1) // 1 small dram
	{
		if (dc_sys_misc.small_dram_4g || dc_sys_misc.small_dram_1g) // small dram is 4G or 1G
			return 64;
		else
			return 32;
	}
	else // 2 small dram
	{
		if (dc_sys_misc.small_dram_4g || dc_sys_misc.small_dram_1g) // small dram is 4G or 1G
			return 32;
		else
			return 64;
	}

	return 0;
}

static unsigned int __rtk_dc_get_boundary(void)
{
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;
	int dram_num = 0;
	int small_dram_num = 0;
	unsigned int small_dram_size = 0;
	unsigned int boundary_addr = 0;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	if (dc_sys_misc.en_ddr_diff_size == 0)
		return 0;

	dram_num = __rtk_dc_get_dram_num();
	small_dram_num = __rtk_dc_get_small_dram_num();
	small_dram_size = __rtk_dc_get_small_dram_size();

	if (dc_sys_misc.en_full_ddr_last == 0) // non-swapped
	{
		boundary_addr = small_dram_size * dram_num;
	}
	else
	{
		if (dram_num == 3)
			boundary_addr = small_dram_size * (dram_num - small_dram_num);
		else if (dram_num == 2)
			boundary_addr = small_dram_size;
	}

	return boundary_addr;
}
static int init_dc_ib_table(RTK_IB_REGION *pIB_table, int *pFirst_id)
{
#define IB_SLICE_UNIT 128 // Unit: Byte
	unsigned int ib_region_set = rtd_inl(DC_SYS_DC_IB_REGION_SET_reg);
	int i, first_id = RTK_IB_REGION_TOTAL_NUM-1, is_IB2to1=0;
	unsigned int dc_size_tmp = 0;
	unsigned int total_ddr_size = 0xFFFFFFFF;
//region0
	pIB_table[0].region_mode = (RTK_IB_REGION_MODE)DC_SYS_DC_IB_REGION_SET_get_region_0_mode(ib_region_set);
	pIB_table[0].slice_size = IB_SLICE_UNIT << DC_SYS_DC_IB_REGION_SET_get_region_0_slice_basis(ib_region_set);
	pIB_table[0].start_addr = 0;

//region1
	pIB_table[1].region_mode = (RTK_IB_REGION_MODE)DC_SYS_DC_IB_REGION_SET_get_region_1_mode(ib_region_set);
	pIB_table[1].slice_size = IB_SLICE_UNIT << DC_SYS_DC_IB_REGION_SET_get_region_1_slice_basis(ib_region_set);
	pIB_table[1].start_addr = rtd_inl(DC_SYS_DC_IB_SEQ_BOUND_0_reg);

	pIB_table[0].end_addr = pIB_table[1].start_addr;

//region2
	pIB_table[2].region_mode = (RTK_IB_REGION_MODE)DC_SYS_DC_IB_REGION_SET_get_region_2_mode(ib_region_set);
	pIB_table[2].slice_size = IB_SLICE_UNIT << DC_SYS_DC_IB_REGION_SET_get_region_2_slice_basis(ib_region_set);
	pIB_table[2].start_addr = rtd_inl(DC_SYS_DC_IB_SEQ_BOUND_1_reg);

	pIB_table[1].end_addr = pIB_table[2].start_addr;

//region3
	pIB_table[3].region_mode = (RTK_IB_REGION_MODE)DC_SYS_DC_IB_REGION_SET_get_region_3_mode(ib_region_set);
	pIB_table[3].slice_size = IB_SLICE_UNIT << DC_SYS_DC_IB_REGION_SET_get_region_3_slice_basis(ib_region_set);
	pIB_table[3].start_addr = rtd_inl(DC_SYS_DC_IB_SEQ_BOUND_2_reg);

	pIB_table[2].end_addr = pIB_table[3].start_addr;

	pIB_table[3].end_addr = pIB_table[2].end_addr + (total_ddr_size - pIB_table[2].end_addr);
	dc_size_tmp = pIB_table[3].end_addr - pIB_table[3].start_addr;
	for(i=0; i< RTK_IB_REGION_TOTAL_NUM; i++){
		rtd_pr_rmm_info("IB[%d].mode=%x, slice_size=%dB, DC=(0x%08x-0x%08x)\n", i,
					pIB_table[i].region_mode,
					pIB_table[i].slice_size,
					pIB_table[i].start_addr,
            pIB_table[i].end_addr);
		if(first_id < RTK_IB_REGION_TOTAL_NUM && (pIB_table[i].start_addr != 0))
			first_id = i-1;
		if(pIB_table[i].region_mode == RTK_IB_REGION_MODE_IB_2_1)
			is_IB2to1 = 1;
		if(pIB_table[i].start_addr % 0x400 != 0)
			panic("IB boundary is NOT aligned by 1KB");
	}
	*pFirst_id = first_id;

	return is_IB2to1;
}

int get_dc_IBregionID(unsigned int dc_addr)
{
	int i = 0;
	RTK_IB_REGION *pIB_table = rtk_ib_table;
	unsigned int dram_base = 0, dram_end = 0;

	for(i = 0; i < RTK_IB_REGION_TOTAL_NUM; i++)
	{
		dram_end = pIB_table[i].end_addr;
		rtd_pr_rmm_debug("dc_ib:dram_end %x start %x\n", dram_end , (dram_base + pIB_table[i].start_addr));

		if(dc_addr >= (dram_base + pIB_table[i].start_addr) && dc_addr < dram_end) {
		return i;
		}
	}
	rtd_pr_rmm_debug("dc_ib: dc_addr %x\n", dc_addr);
	return DCIB_Invalid;
}
unsigned int rtk_get_dc_secure_aligned_size(const unsigned int dc_addr)
{
	int region_id = 0, first_region_id =0;
	unsigned int align_size = 1;
	if(!rtk_global_dram_info.if_init_ib_table){
		rtk_global_dram_info.has_IB2to1 = init_dc_ib_table(rtk_ib_table, &first_region_id);
		rtk_global_dram_info.dram_num =  __rtk_dc_get_dram_num();
		rtk_global_dram_info.if_init_ib_table = true;
	}
	region_id = get_dc_IBregionID(dc_addr);

	if(region_id == DCIB_Invalid)
		return align_size;
	rtd_pr_rmm_debug("dc_ib: get has ib2to1 %d num %d\n", rtk_global_dram_info.has_IB2to1,  rtk_global_dram_info.dram_num);
	if(rtk_global_dram_info.has_IB2to1 && rtk_ib_table[region_id].region_mode == RTK_IB_REGION_MODE_IB_2_1) {
		align_size = ALIGN_SIZE_3DDR;
	}
	else if(rtk_global_dram_info.has_IB2to1 && rtk_ib_table[region_id].region_mode != RTK_IB_REGION_MODE_IB_2_1) {
		align_size = ALIGN_SIZE_1_2DDR;
	}
	else if(rtk_global_dram_info.dram_num == 3){
		align_size = ALIGN_SIZE_3DDR;
	}
	else{
		align_size = ALIGN_SIZE_1_2DDR;
	}
	return align_size;
}

unsigned int rtk_get_dc_secure_aligned_addr(const unsigned int dc_addr, int is_aligned_up)
{
	int region_id = 0, first_region_id =0;
	unsigned int dc_region_start, align_size, tmp_offset;
	if(!rtk_global_dram_info.if_init_ib_table){
		rtk_global_dram_info.has_IB2to1 = init_dc_ib_table(rtk_ib_table, &first_region_id);
		rtk_global_dram_info.dram_num =  __rtk_dc_get_dram_num();
		rtk_global_dram_info.if_init_ib_table = true;
	}
	region_id = get_dc_IBregionID(dc_addr);

	if(region_id == DCIB_Invalid)
		return dc_addr;
	rtd_pr_rmm_debug("dc_ib: get has ib2to1 %d\n", rtk_global_dram_info.has_IB2to1);
	if(rtk_global_dram_info.has_IB2to1 && rtk_ib_table[region_id].region_mode == RTK_IB_REGION_MODE_IB_2_1) {
		dc_region_start = rtk_ib_table[region_id].start_addr;
		align_size = ALIGN_SIZE_3DDR;
	}
	else if(rtk_global_dram_info.has_IB2to1 && rtk_ib_table[region_id].region_mode != RTK_IB_REGION_MODE_IB_2_1) {
		dc_region_start = rtk_ib_table[region_id].start_addr;
		align_size = ALIGN_SIZE_1_2DDR;
	}
	else if(rtk_global_dram_info.dram_num == 3){
		dc_region_start = 0;
		align_size = ALIGN_SIZE_3DDR;
	}
	else{
		dc_region_start = 0;
		align_size = ALIGN_SIZE_1_2DDR;
	}

	tmp_offset= dc_addr - dc_region_start;
	if(is_aligned_up)
		tmp_offset = ALIGNED_UP(tmp_offset, align_size);
	else
		tmp_offset = ALIGNED_DOWN(tmp_offset, align_size);
	rtd_pr_rmm_debug("dc_ib: dc_addr %x get region_id =%d(mode %d) tmp_offset %x aligned %x start %x\n",dc_addr, region_id,rtk_ib_table[region_id].region_mode, tmp_offset,align_size, dc_region_start );

	return (dc_region_start + tmp_offset);
}

RTK_DC_RET_t rtk_dc_get_dram_info(RTK_DC_INFO_t *dram_info)
{
	if (dram_info == NULL) {
		rtd_pr_rmm_err("[ERR]%s(%d): dram_info is NULL\n", __FUNCTION__, __LINE__);
		return RTK_DC_RET_FAIL; // error
	}

	dram_info->dram_num = __rtk_dc_get_dram_num();
	dram_info->swap_en = __rtk_dc_is_swapped();
	dram_info->sw_mode = __rtk_dc_offset_is_sw_mode();
	dram_info->sw_offset = __rtk_dc_get_sw_mode_offset();
	dram_info->boundary = __rtk_dc_get_boundary();

	return RTK_DC_RET_SUCCESS;
}
EXPORT_SYMBOL(rtk_dc_get_dram_info);

RTK_DC_BW_TYPE_t rtk_dc_query_dram_region_by_addr(unsigned long phy_addr)
{
	RTK_DC_INFO_t dram_info;
	RTK_DC_RET_t ret;

	ret = rtk_dc_get_dram_info(&dram_info);

	if (ret != RTK_DC_RET_SUCCESS) {
		rtd_pr_rmm_err("[ERR]%s(%d): fail to get dram_info\n", __FUNCTION__, __LINE__);
		return RTK_DC_QUERY_FAIL; // error
	}

	if (dram_info.boundary == 0) // all dram size is the same
		return RTK_DC_HIGHER_BW;

	if ((dram_info.swap_en == 0) && (phy_addr < dram_info.boundary))
		return RTK_DC_HIGHER_BW;
	else if ((dram_info.swap_en == 1) && (phy_addr >= dram_info.boundary))
		return RTK_DC_HIGHER_BW;
	else
		return RTK_DC_LOWER_BW;
}
EXPORT_SYMBOL(rtk_dc_query_dram_region_by_addr);

#if 0 // test only
static int __init rtk_dram_info_dump(void)
{
	RTK_DC_INFO_t dram_info;
	RTK_DC_RET_t ret;

	rtd_pr_rmm_err("[RTK-DC] %s:\n", __FUNCTION__);

	ret = rtk_dc_get_dram_info(&dram_info);

	if (ret == RTK_DC_RET_SUCCESS)
	{
		rtd_pr_rmm_err("dram_info->dram_num is %x\n", dram_info.dram_num);
		rtd_pr_rmm_err("dram_info->swap_en is %x\n", dram_info.swap_en);
		rtd_pr_rmm_err("dram_info->sw_mode is %x\n", dram_info.sw_mode);
		rtd_pr_rmm_err("dram_info->sw_offset is %x\n", dram_info.sw_offset);
		rtd_pr_rmm_err("dram_info->boundary is %x\n", dram_info.boundary);
	}
	else
	{
		rtd_pr_rmm_err("fail to get dram info\n");
	}

	rtd_pr_rmm_err("0x12000000 is %s bw region\n", rtk_dc_query_dram_region_by_addr(0x12000000)?"lower":"higher");
	rtd_pr_rmm_err("0x20000000 is %s bw region\n", rtk_dc_query_dram_region_by_addr(0x20000000)?"lower":"higher");
	rtd_pr_rmm_err("0x21000200 is %s bw region\n", rtk_dc_query_dram_region_by_addr(0x21000200)?"lower":"higher");
	rtd_pr_rmm_err("0x28004000 is %s bw region\n", rtk_dc_query_dram_region_by_addr(0x28004000)?"lower":"higher");
	rtd_pr_rmm_err("0x34004200 is %s bw region\n", rtk_dc_query_dram_region_by_addr(0x34004200)?"lower":"higher");

	return 0;
}
late_initcall(rtk_dram_info_dump);
#endif
#endif // ENABLE_DC_API_SUPPORT

/* Decide if there are different ddr sizes */
static int is_ddr_diff_size_enabled(void)
{
#if defined (CONFIG_RTK_KDRIVER_SUPPORT) && defined (ENABLE_DC_API_SUPPORT)
	dc_sys_dc_sys_misc_RBUS dc_sys_misc;

	dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);

	return dc_sys_misc.en_ddr_diff_size;
#else
	return 0;
#endif
}

/* Get ddr boundary if there are different ddr sizes */
static unsigned long get_diff_ddr_boundary(void)
{
	__maybe_unused RTK_DC_RET_t ret;
	__maybe_unused RTK_DC_INFO_t dc_info;

	if (!is_ddr_diff_size_enabled())
		return 0;

#if defined (CONFIG_RTK_KDRIVER_SUPPORT) && defined (ENABLE_DC_API_SUPPORT)
	ret = rtk_dc_get_dram_info(&dc_info);
	if (ret == RTK_DC_RET_FAIL) {
		rtd_pr_rmm_err("warning, dram info incorrect\n");
		return 0;
	}
	return dc_info.boundary;
#else
	return 0;
#endif
}

/* Decide if ddr swap is enabled */
int is_ddr_swap(void)
{
#if defined (CONFIG_RTK_KDRIVER_SUPPORT) && defined (ENABLE_DC_API_SUPPORT)
	RTK_DC_RET_t ret;
	RTK_DC_INFO_t dc_info;

	ret = rtk_dc_get_dram_info(&dc_info);
	if (ret == RTK_DC_RET_FAIL) {
		rtd_pr_rmm_err("warning, dram info incorrect\n");
		return 0;
	}
	return dc_info.swap_en;
#else
	return 0;
#endif
}

unsigned long get_memc_start_address(void)
{
	unsigned long memc_size = 0;
	unsigned long memc_addr = 0;

	memc_size = carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void **)&memc_addr);

	return memc_addr;
}
EXPORT_SYMBOL(get_memc_start_address);

int get_memc_size(void)
{
	unsigned long memc_addr = 0;

	return carvedout_buf_query(CARVEDOUT_SCALER_MEMC,(void **) &memc_addr);
}
EXPORT_SYMBOL(get_memc_size);

unsigned long get_scaler_mdomain_start_address(void)
{
	unsigned long mdomain_size = 0;
	unsigned long mdomain_addr = 0;

	mdomain_size = carvedout_buf_query(CARVEDOUT_SCALER_MDOMAIN, (void **)&mdomain_addr);

	return mdomain_addr;
}
EXPORT_SYMBOL(get_scaler_mdomain_start_address);

int get_scaler_mdomain_size(void)
{
	unsigned long mdomain_addr = 0;

	return carvedout_buf_query(CARVEDOUT_SCALER_MDOMAIN, (void **)&mdomain_addr);
}
EXPORT_SYMBOL(get_scaler_mdomain_size);

unsigned long get_scaler_od_start_address(void)
{
	unsigned long od_size = 0;
	unsigned long od_addr = 0;

	od_size = carvedout_buf_query(CARVEDOUT_SCALER_OD, (void **)&od_addr);

	return od_addr;
}
EXPORT_SYMBOL(get_scaler_od_start_address);

int get_scaler_od_size(void)
{
	unsigned long od_addr = 0;

	return carvedout_buf_query(CARVEDOUT_SCALER_OD, (void **)&od_addr);
}
EXPORT_SYMBOL(get_scaler_od_size);

RTK_DC_COMMON_RET_t rtk_dc_get_dram_bitwidth(RTK_DC_COMMON_BITWIDTH_t *p_dram_bitwidth)
{
        dc_sys_dc_sys_misc_RBUS dc_sys_misc;
        dc_mc_mc_sys_2_RBUS dc_mc_mc_sys_2;
        unsigned char ddr_bitwidth = 0;

        dc_sys_misc.regValue = rtd_inl(DC_SYS_DC_SYS_MISC_reg);
        dc_mc_mc_sys_2.regValue = rtd_inl(DC_MC_MC_SYS_2_reg);

        if (p_dram_bitwidth == NULL) {
                rtd_pr_rmm_err("[ERR]%s(%d): p_dram_bitwidth is NULL\n", __FUNCTION__, __LINE__);
                return RTK_DC_COMMON_RET_FAIL; // error
        }

        ddr_bitwidth += (dc_sys_misc.mem_num ? 32 : 16);
        if(dc_mc_mc_sys_2.mcx2_en == 1) //dual mc
                ddr_bitwidth += (dc_sys_misc.mem_num_dc2 ? 32 : 16);

        if (ddr_bitwidth == 48) {
                *p_dram_bitwidth = RTK_DC_COMMON_BITWIDTH_48;
        }
        else if (ddr_bitwidth == 64) {
                *p_dram_bitwidth = RTK_DC_COMMON_BITWIDTH_64;
        }
        else if (ddr_bitwidth == 16) {
                *p_dram_bitwidth = RTK_DC_COMMON_BITWIDTH_16;
        }
        else {  //ddr_bitwidth == 32
                *p_dram_bitwidth = RTK_DC_COMMON_BITWIDTH_32;
        }

        return RTK_DC_COMMON_RET_SUCCESS;
}
EXPORT_SYMBOL(rtk_dc_get_dram_bitwidth);

inline bool carvedout_enable(void)
{
#define MAGIC_CAVEDOUT (0xdecade10)
    return rtd_inl(STB_SW_DATA9_reg) == MAGIC_CAVEDOUT ? 0 : 1;
}

unsigned int carvedout_buf_get_layout_idx(void)
{
    unsigned int layout_idx = MM_LAYOUT_DEFAULT;

	if (unlikely(DRAM_size==0)) {
		//debug test if DRAM_size was referred by before it's valid.
		rtd_pr_rmm_err("%s ERROR DRAM_size info is not available (%pS)\n", __func__, (void*)_RET_IP_);
	}

	if (DRAM_size > 0xa0000) {
		if (get_platform_model() == PLATFORM_MODEL_8K) {
			layout_idx = MM_LAYOUT_M7_8K;
		} else {
			layout_idx = MM_LAYOUT_DEFAULT; // 2G and over 2G
		}
	} else if (DRAM_size == 0x40000) { //(get_memory_size_total())
		layout_idx = MM_LAYOUT_1GB;
	} else if (DRAM_size == 0x60000) {
		if (carvedout_enable() == false)
			layout_idx = MM_LAYOUT_1G5_NC;
		else
			layout_idx = MM_LAYOUT_1G5;
	} else {
		if (mem_type_2mc == false)
			layout_idx = MM_LAYOUT_1G5;
		else if (carvedout_enable() == false)
			layout_idx = MM_LAYOUT_DEFAULT_NC;
	}

    return layout_idx;

}
EXPORT_SYMBOL(carvedout_buf_get_layout_idx);

unsigned long carvedout_buf_query(carvedout_buf_t what, void **addr)
{
	unsigned long size = 0;
	void *address = NULL;
	unsigned int layout_idx = carvedout_buf_get_layout_idx();

#ifdef RTK_MEM_LAYOUT_DEVICETREE

    //OBSOLETED can be removed after GKI completed.
	if (unlikely(DRAM_size==0)) {
        //debug test if DRAM_size was referred by before it's valid. [obsoleted]
		rtd_pr_rmm_err("%s ERROR DRAM_size info is not available (%pS)\n", __func__, (void*)_RET_IP_);
	}

#endif

	if (what >= CARVEDOUT_NUM) {
		rtd_pr_rmm_err("Input carvedout query index (%d) is out of range %d\n", what, CARVEDOUT_NUM);
		return 0;
	}

	/* this switch case is list by address order */
	switch (what)
	{
	case CARVEDOUT_DDR_BOUNDARY:
		if (is_ddr_diff_size_enabled()) {
			if (is_ddr_swap()) {
				size = PAGE_SIZE; // 4KB
			} else {
				size = PAGE_SIZE << pageblock_order; // 4MB
			}
			address = (void *)(get_diff_ddr_boundary() - size);
		} else {
			size = 0;
			address = (void *)0;
		}
		rtd_pr_rmm_info("[MEM]DDR boundary address = 0x%x, size = 0x%x\n", address, size);
		break;

	case CARVEDOUT_GPU_RESERVED:

		//[DTS][UBOOT] Multi-layout selection.

		if (DRAM_size > 0xa0000) {
			size = CMA_HIGHMEM_M7_8K;
		} else if (DRAM_size > 0x60000) {
			size = CMA_HIGHMEM_LARGE;
		} else {
			size = CMA_HIGHMEM_SMALL;
		}
		rtd_pr_rmm_debug("[MEM] Highmem size is 0x%lx (%ld MB)\n", size, size/1024/1024);
		break;

	default:
#if 1//ndef CONFIG_ARM64  //FIXME
		address = (void *)carvedout_buf[layout_idx][what][0];
		size = carvedout_buf[layout_idx][what][1];
#else
		size = fdt_get_carvedout_mem_info(carvedout_mem_dts_name[what], &address);
#endif

#if defined(CONFIG_REALTEK_SECURE) && !IS_ENABLED(CONFIG_RTK_KDRV_TEE)
		if (what == CARVEDOUT_K_OS)
			size = 32*_MB_;
#endif
#if !defined(CONFIG_REALTEK_SECURE_DDK)
		if (what == CARVEDOUT_GPU_FW)
			size = _TBD_;
#endif
#if !IS_ENABLED(CONFIG_RTK_KDRV_RPC)
		if (what == CARVEDOUT_MAP_RPC)
			size = _TBD_;
#endif
#if !IS_ENABLED(CONFIG_REALTEK_LOGBUF)
		if (what == CARVEDOUT_LOGBUF)
			size = _TBD_;
#endif
#if !defined(CONFIG_PSTORE)
		if (what == CARVEDOUT_RAMOOPS)
			size = _TBD_;
#endif
#if !defined(CONFIG_CUSTOMER_TV006)
		if (what == CARVEDOUT_GAL)
			size = _TBD_;
#endif
#if !defined(CONFIG_LG_SNAPSHOT_BOOT)
		if (what == CARVEDOUT_SNAPSHOT)
			size = _TBD_;
#endif
#if !defined(RTK_MEM_LAYOUT_DEVICETREE) && !defined(BUILD_QUICK_SHOW)
//FIXME this special hack is supposed to be deprecated.
#if defined(CONFIG_VBM_CMA)
		/*
		 * 8K model use carvedout vbm memory
		 * 4K model use       cma vbm memory
		 */
		if (what == CARVEDOUT_VDEC_VBM && layout_idx != MM_LAYOUT_M7_8K)
			size = _TBD_;
#else
		if (what == CARVEDOUT_CMA_VBM)
			size = _TBD_;
#endif
#endif
		break;

//  implement your own specific case
//	case :
//		break;
	}

	if (addr)
		*addr = address;

	return size;
}
EXPORT_SYMBOL(carvedout_buf_query);

#ifdef CONFIG_OPTEE_SECURE_SVP_PROTECTION
unsigned long carvedout_buf_query_secure(carvedout_buf_t what, void **addr)
{
	unsigned long size = 0, end = 0;
	void *address = NULL;
	unsigned int layout_idx = carvedout_buf_get_layout_idx();

#ifdef RTK_MEM_LAYOUT_DEVICETREE //OBSOLETED

	if (unlikely(DRAM_size==0)) {
		rtd_pr_rmm_err("%s ERROR DRAM_size info is not available (%pS)\n", __func__, (void*)_RET_IP_);
	}

#endif

	if (what >= CARVEDOUT_NUM) {
		rtd_pr_rmm_err("Input carvedout query index (%d) is out of range %d\n", what, CARVEDOUT_NUM);
		return 0;
	}

	/* this switch case is list by address order */
	switch (what)
	{
	/* defaut protect MEMC / MDOMAIN / OD / DI_NR*/
	case CARVEDOUT_SCALER_MEMC:
	case CARVEDOUT_SCALER_MDOMAIN:
	case CARVEDOUT_SCALER_OD:
	case CARVEDOUT_SCALER_DI_NR:
	case CARVEDOUT_SCALER_VIP:
		address = (void *)carvedout_buf[layout_idx][what][0];
		size = carvedout_buf[layout_idx][what][1];

		if(address > 0 && size > 0) {
			end = rtk_get_dc_secure_aligned_addr(((unsigned long)address + size), 0);
			address = (void *)(unsigned long)rtk_get_dc_secure_aligned_addr((unsigned long)address, 1);
			size = end - (unsigned long)address;
		}
		break;
	default:
#if 1//ndef CONFIG_ARM64 //FIXME
		address = (void *)carvedout_buf[layout_idx][what][0];
		size = carvedout_buf[layout_idx][what][1];
		if(address > 0 && size > 0) {
			end = rtk_get_dc_secure_aligned_addr(((unsigned long)address + size), 0);
			address = (void *)(unsigned long)rtk_get_dc_secure_aligned_addr((unsigned long)address, 1);
			size = end - (unsigned long)address;
		}
		rtd_pr_rmm_info("[MEM] carvedout query secure addr=%p size=%lx\n", address, size);
#else
		size = fdt_get_carvedout_mem_info(carvedout_mem_dts_name[what], &address);
#endif

#if defined(CONFIG_REALTEK_SECURE) && !IS_ENABLED(CONFIG_RTK_KDRV_TEE)
		if (what == CARVEDOUT_K_OS)
			size = 32*_MB_;
#endif
#if !defined(CONFIG_REALTEK_SECURE_DDK)
		if (what == CARVEDOUT_GPU_FW)
			size = _TBD_;
#endif
#if !IS_ENABLED(CONFIG_RTK_KDRV_RPC)
		if (what == CARVEDOUT_MAP_RPC)
			size = _TBD_;
#endif
#if !IS_ENABLED(CONFIG_REALTEK_LOGBUF)
		if (what == CARVEDOUT_LOGBUF)
			size = _TBD_;
#endif
#if !defined(CONFIG_PSTORE)
		if (what == CARVEDOUT_RAMOOPS)
			size = _TBD_;
#endif
#if !defined(CONFIG_CUSTOMER_TV006)
		if (what == CARVEDOUT_GAL)
			size = _TBD_;
#endif
#if !defined(CONFIG_LG_SNAPSHOT_BOOT)
		if (what == CARVEDOUT_SNAPSHOT)
			size = _TBD_;
#endif
#if !defined(RTK_MEM_LAYOUT_DEVICETREE) && !defined(BUILD_QUICK_SHOW)
#if defined(CONFIG_VBM_CMA)
		/* 
		 * 8K model use carvedout vbm memory 
		 * 4K model use       cma vbm memory
		 */
		if (what == CARVEDOUT_VDEC_VBM && layout_idx != MM_LAYOUT_M7_8K)
			size = _TBD_;
#else
		if (what == CARVEDOUT_CMA_VBM)
			size = _TBD_;
#endif
#endif
		break;

//  implement your own specific case
//	case :
//		break;
	}

	if (addr)
		*addr = address;

	return size;
}
#endif

unsigned long carvedout_buf_query_range(carvedout_buf_t idx_start, carvedout_buf_t idx_end, void **start, void **end)
{
	unsigned long size = 0;
	unsigned long min_addr = ULONG_MAX;
	unsigned long max_addr = 0;
	int i = 0;
	int idx_a, idx_b;

	if ((idx_start >= CARVEDOUT_NUM) || (idx_end >= CARVEDOUT_NUM)) {
		rtd_pr_rmm_err("Error! One of carvedout input range index (%d, %d) is out of bounds %d\n", idx_start, idx_end, CARVEDOUT_NUM);
		return 0;
	}

	// index rule : a < b
	idx_a = idx_start;
	idx_b = idx_end;
	if (idx_start > idx_end) { // swap
		idx_a = idx_end;
		idx_b = idx_start;
	}

	for (i = idx_a; i <= idx_b; i++) {
		unsigned long tmp_addr = 0;
		unsigned long tmp_size = 0;

		tmp_size = carvedout_buf_query((carvedout_buf_t) i, (void **)&tmp_addr);
		if (tmp_size) {
			min_addr = min(min_addr, tmp_addr);
			max_addr = max(max_addr, tmp_addr + tmp_size); // end address
			size += tmp_size;
			//rtd_pr_rmm_info("idx=%d, size=%lx, min_addr=%lx, max_addr=%lx\n", i, tmp_size, min_addr, max_addr);
		}
#ifdef CONFIG_VBM_CMA
		if (i == CARVEDOUT_VDEC_VBM) {
			tmp_size = carvedout_buf_query((carvedout_buf_t)CARVEDOUT_CMA_VBM , (void **)&tmp_addr);
			if (tmp_size) {
				min_addr = min(min_addr, tmp_addr);
				max_addr = max(max_addr, tmp_addr + tmp_size); // end address
				size += tmp_size;
				rtd_pr_rmm_info("idx=%d, size=%lx, min_addr=%lx, max_addr=%lx\n", i, tmp_size, min_addr, max_addr);
			}
		}
#endif
	}

	if (start && end) {
		*start = (void *)min_addr;
		*end   = (void *)max_addr;
		rtd_pr_rmm_info("carved-out total size 0x%lx (0x%lx@ %lx ~ %lx)\n", size, (unsigned long)(*end - *start), (unsigned long)*start, (unsigned long)*end);
	} else {
		rtd_pr_rmm_info("carved-out non given start or end address, size 0x%lx (0x%x ~ 0x%x)\n", size, (unsigned int)min_addr, (unsigned int)max_addr);
	}

	if (max_addr && ((max_addr - min_addr) != size)) {
		rtd_pr_rmm_info("carved-out mismatch? size (0x%lx, 0x%lx), choose min one\n", size, (max_addr - min_addr));
		size = min((max_addr - min_addr), size);
	}

	return size;
}

unsigned int carvedout_fallback_query(carvedout_buf_t what)
{
	switch (what)
	{
	case CARVEDOUT_TP:
		return CARVEDOUT_FALLBACK_TO_CMA;

	default:
		return CARVEDOUT_NO_FALLBACK;
	}
}

/*
 * return negative value for invalid query
 */
int carvedout_buf_query_is_in_range(unsigned long in_addr, void **start, void **end)
{
	int i = 0;
	carvedout_buf_t idx;
	carvedout_buf_t whitelist[] = {CARVEDOUT_BOOTCODE, CARVEDOUT_AUDIO, CARVEDOUT_VDEC_RINGBUF, CARVEDOUT_LOGBUF, CARVEDOUT_SCALER_NN, CARVEDOUT_VDEC_VBM, CARVEDOUT_TP};
	unsigned long size, addr;

	unsigned int whitelist_num = sizeof(whitelist)/sizeof(carvedout_buf_t);

	for (i = 0; i < whitelist_num; i++) { // except cma area
		idx = whitelist[i];
		size = carvedout_buf_query(idx, (void *)&addr);
		if ((idx == CARVEDOUT_BOOTCODE) && size) {
			if ((in_addr >= addr) && (in_addr < (addr + size))) {
				if (start && end) {
					*start = (void *)addr;
					*end   = (void *)(addr + 0x20000);
				}
				rtd_pr_rmm_debug("carvedout_bootcode query(%lx) in idx(%d), range(%lx-%lx)\n", in_addr, idx, addr, addr + size);
				return (int)idx;
			}
		}
		else if(addr && size) {
			if ((in_addr >= addr) && (in_addr < (addr + size))) {
				if (start && end) {
					*start = (void *)addr;
					*end   = (void *)(addr + size);
				}
				rtd_pr_rmm_debug("carvedout_buf query(%lx) in idx(%d), range(%lx-%lx)\n", in_addr, idx, addr, addr + size);
				return (int)idx;
			}
		}
	}

    return -1;
}
EXPORT_SYMBOL(carvedout_buf_query_is_in_range);

#ifndef BUILD_QUICK_SHOW
static const char * const realtek_carvedout_buf_of_match[] = {
	"realtek,rtd6748",
	NULL
};

static int carvedout_buf_of_compatible_match(struct device_node *device,
			       const char *const *compat)
{
	unsigned int tmp, score = 0;

	if (!compat)
		return 0;

	while (*compat) {
		tmp = of_device_is_compatible(device, *compat);
		if (tmp > score)
			score = tmp;
		compat++;
	}

	return score;
}

static int carvedout_buf_get_memory(struct device_node *np)
{
    static int cnt = 0;
	struct device_node *node;
	struct resource r;
	int i = 0, ret = 0;

	if (unlikely(DRAM_size!=0)) {
		rtd_pr_rmm_err("%s ERROR DRAM_size was updated before (%pS)\n", __func__, (void*)_RET_IP_);
	}

	node = of_parse_phandle(np, "memory-region", 0);
	if (!node) {
		rtd_pr_rmm_err("no memory-region\n");
		return -EINVAL;
	}

	for (;;i++) {
		size_t size;
		ret = of_address_to_resource(node, i, &r);
		if (ret) {
			rtd_pr_rmm_debug("resource[%d] error(%d), stop\n", i, ret);
			break;
		}
		if (i > 10) { // avoid lock
			rtd_pr_rmm_err("resource[%d] too much, break\n", i);
			break;
		}
		size = resource_size(&r);

		rtd_pr_rmm_info("mem=%lldMB @ 0x%08llx\n", size / 1024 / 1024, r.start);
		if (!cnt++)
			add_memory_size(GFP_DCU1, (unsigned long)size);
		else
			add_memory_size(GFP_DCU2, (unsigned long)size);

		DRAM_size += (size >> PAGE_SHIFT);
	}

	rtd_pr_rmm_info("%s DRAM_size %d\n", __func__, DRAM_size);

	return 0;
}


#if 0 // refer carvedout_buf_layout_build for logbuf size update
#ifdef CONFIG_REALTEK_LOGBUF
__attribute__((weak)) unsigned long rtdlog_get_buffer_size(void)
{
    rtd_pr_rmm_err("%s[weak] is not available\n", __func__);
    return 0;
}
#endif
#endif


//TODO: it could be a common code
int carvedout_buf_layout_build(void)
{
	int ret = 0;
	struct device_node *np, *child, *rmm;
	const char *comp;
	unsigned int layout_idx;
#ifdef RTK_MEM_LAYOUT_DEVICETREE
	const char *rmm_child;
#endif
	// compatible info for memory layout
	rmm = NULL;
	do  {
		// find next node reserved-memory, or reserved-memory@XXX
		rmm = of_find_node_by_name(rmm, "reserved-memory");
		if (rmm) {
			rtd_pr_rmm_info("fdt 0x%x %s\n", rmm, rmm->full_name);
			if (strcmp(rmm->full_name, "reserved-memory") == 0) {
				rtd_pr_rmm_notice("found 0x%x %s\n", rmm, rmm->full_name);
				break;
			}
		}
		else {
			rtd_pr_rmm_err("ERROR no reserved-memory found\n");
			goto err;
		}
	} while (rmm);

	comp = (const char*)of_get_property(rmm, "compatible", NULL);
	if (comp) {
		rtd_pr_rmm_notice("%s compatible %s\n", rmm->name ,comp);
	}
	else {
		rtd_pr_rmm_err("ERROR no %s compatible %s\n", rmm->name, comp);
	}

	np = of_find_node_by_name(NULL, "carvedout_buf");
	if (!carvedout_buf_of_compatible_match(np, realtek_carvedout_buf_of_match))
	{
		rtd_pr_rmm_err("ERROR no carvedout_buf match\n");
		ret = -EINVAL;
		goto err;
	}

	if (carvedout_buf_get_memory(np) < 0) {
		rtd_pr_rmm_err("ERROR no memory-region\n");
		ret = -EINVAL;
		goto err;
	}
 	layout_idx = carvedout_buf_get_layout_idx();
	rtd_pr_rmm_notice("carvedout_buf selected idx %d\n", layout_idx);

	for_each_child_of_node(np, child) {
		size_t size;
		struct resource r;
		struct device_node *node;
		const __be32 *addrp;
		unsigned int carvedout_idx = -1;
		unsigned long *carvedout;

		addrp = of_get_address(child, 0, NULL, NULL);
		if (!addrp) {
			rtd_pr_rmm_err("ERROR no carvedout_buf idx for %s\n", child->full_name);
			ret = -EINVAL;
			continue;
		} else {
			carvedout_idx = (unsigned int)be32_to_cpu(*addrp);
		}

#ifdef RTK_MEM_LAYOUT_DEVICETREE
		// For not using overlay. carvedout_buf mapping to regions by name instead of phandle.
		//  If not using overlay and not multiple dtbs, all layouts can be defined in single dts
		//  with different names (e.g. 'reserved-memory#1', 'reserverd-memory#2', etc) and let loader
		//  to fixup selected one to be native node 'reserved-memory' at final fdt. But carvedout_buf
		//  mapping cannot not use same phandle between nodes and has to be /path instead.
		rmm_child = (const char*)of_get_property(child, "memory-region", NULL);
		node = of_get_child_by_name(rmm, rmm_child);
#else
		node = of_parse_phandle(child, "memory-region", 0);
#endif
		if (!node) {
			rtd_pr_rmm_err("ERROR no memory-region for %s\n", child->full_name);
			ret = -EINVAL;
			continue;
		}

		ret = of_address_to_resource(node, 0, &r);
		if (ret) {
			rtd_pr_rmm_err("ERROR resource error(%d) for %s\n", ret, child->full_name);
			ret = -EINVAL;
			continue;
		}
		size = resource_size(&r);

		//! also not available if status not-'okay' or "disabled"
		if (!of_device_is_available(node)) {
#ifdef RTK_MEM_LAYOUT_DEVICETREE
			rtd_pr_rmm_notice("%s%s is disabled(size 0)\n", rmm_child, child->full_name);
#else
			rtd_pr_rmm_info("%s is disabled(size 0)\n", child->full_name);
#endif
			size = 0;
		} else if (strncmp("cma3", rmm_child, 4) == 0 && !of_get_property(node, "rtkbp", NULL)) {
			rtd_pr_rmm_notice("%s%s is not rtkbp\n", rmm_child, child->full_name);
			cma_unset_bp(DEV_CMA3);
		}

		/*
		 * legacy kdriver and it's user (e.g. quick-show) may still refer predefined static in builtin carvedout_buf[] for some reason
		 * in case somehow it's NOT sync'd with devicetree, here's a favor being a Canary during update traversal.
		 * Precodintion:
		 * 1. both devicetree(*.dtb) and carvedout_buf[] refer the same carvedout index (rmm_carvedout.h)
		 * 2. layouts listed in carvedout mapping on devicetree
		 */
		carvedout = carvedout_buf[layout_idx][carvedout_idx];
		if (carvedout[1]) {
			if ((carvedout[1] != (unsigned long)size) || (carvedout[0] != (unsigned long)r.start)) {
				rtd_pr_rmm_err("%s%s (0x%08x/0x%08x)->(0x%08x/0x%08x) Mismatched \n",
				  rmm_child, child->full_name, carvedout[0], carvedout[1], (unsigned long)r.start, (unsigned long)size);
			}
			if(CARVEDOUT_LOGBUF == carvedout_idx)
			{
			        carvedout[0] = (unsigned long)r.start;
			        carvedout[1] = (unsigned long)size;
			}
		}
		else if (size) {
			rtd_pr_rmm_notice("%s%s (0x%08x/0x%08x)->(0x%08x/0x%08x) Updated\n",
			  rmm_child, child->full_name, carvedout[0], carvedout[1], (unsigned long)r.start, (unsigned long)size);

			carvedout[0] = (unsigned long)r.start;
			carvedout[1] = (unsigned long)size;
		}

#if 0 //debug
	{
        unsigned long size, addr;
        size = carvedout_buf_query(carvedout_idx, (void *)&addr);
        rtd_pr_rmm_info("carvedout %d start(%lx)..(%lx), size(%ld_MB))\n", carvedout_idx, addr, addr + size, (size/1024/1024));
	}
#endif

#if 0 // RTK_MEM_LAYOUT_DEVICETREE
// if uboot cannot directly adjust the logbuf bufsize in fdt, here return(free) the unused reserved region back to buddy system
// ref: cma_init_reserved_areas(...)
//  1. MemTotal info will be update after __free
//  2. managed info should be updated manually
//  3. memblock reserved region may need to free
//  4. rtdlog_get_buffer_size( ) should be ready early.
#ifdef CONFIG_REALTEK_LOGBUF
        if (carvedout_buf_idx == CARVEDOUT_LOGBUF)
        {
			//? rtdlog_parse_bufsize should be prior than rmm init
            unsigned long bufsize = rtdlog_get_buffer_size();
            struct page *pg;
            int cnt, odr;

            if (size > bufsize)
            {
                rtd_pr_rmm_info("%s(%x/%x) size updated %x \n", child->name, r.start, size, bufsize);

                pg = pfn_to_page(PFN_UP(r.start + bufsize));
                //odr = min(MAX_ORDER-1, get_order(bufsize));
                odr = 0;
                cnt = (size - bufsize) / PAGE_SIZE;
                do {
                    while (cnt < (1<<odr)) {
                        odr--;
                    }
                    rtd_pr_rmm_debug("%s free %x order %d\n", child->name, PFN_PHYS(page_to_pfn(pg)), odr);

                    __ClearPageReserved(pg);
                    //set_page_count(pg, 0);
                    set_page_count(pg, 1); //set_page_refcounted(pg);

                    // it could also be 2 loop phases:
                    // 1.clear flags for each page, 2.free pages from high order to 0-order.
                    __free_pages(pg, odr);

                    pg = pg + (1<<odr);
                    cnt = cnt - (1<<odr);
                } while (cnt);

                adjust_managed_page_count(pg, (size - bufsize) / PAGE_SIZE); // although 'memtotal' will be updated after free but not 'managed'
                memblock_free(r.start+bufsize, size-bufsize); // although reserve was returned to buddy, but not memblock reserved list.
            }
            else {
                rtd_pr_rmm_err("%s size %d overflow\n", child->name, bufsize);
            }
            carvedout_buf[layout_idx][CARVEDOUT_LOGBUF][1] = bufsize;
        }
#endif
#endif //RTK_MEM_LAYOUT_DEVICETREE

	}

#ifdef RTK_MEM_LAYOUT_DEVICETREE
    //[TODO] not to use CARVEDOUT_SCALER anymore or only one united scaler region in devicetree
    carvedout_buf[layout_idx][CARVEDOUT_SCALER][0] = carvedout_buf[layout_idx][CARVEDOUT_SCALER_MEMC][0];
    carvedout_buf[layout_idx][CARVEDOUT_SCALER][1] = DO_ALIGNMENT( (
      carvedout_buf[layout_idx][CARVEDOUT_SCALER_MEMC][1] +
      carvedout_buf[layout_idx][CARVEDOUT_SCALER_MDOMAIN][1] +
      carvedout_buf[layout_idx][CARVEDOUT_SCALER_DI_NR][1] +
      carvedout_buf[layout_idx][CARVEDOUT_SCALER_OD][1] +
      carvedout_buf[layout_idx][CARVEDOUT_SCALER_VIP][1] +
      carvedout_buf[layout_idx][CARVEDOUT_SCALER_NN][1] ), 4*_MB_);
#endif

	if (ret)
		goto err;

#if 1 //debug
    {
        unsigned long size, addr;
        size = carvedout_buf_query(CARVEDOUT_LOGBUF, (void *)&addr);
        rtd_pr_rmm_info("carvedout logbuf start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void *)&addr);
        rtd_pr_rmm_info("carvedout memc start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_MDOMAIN, (void *)&addr);
        rtd_pr_rmm_info("carvedout mdomain start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_DI_NR, (void *)&addr);
        rtd_pr_rmm_info("carvedout di_nr start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_NN, (void *)&addr);
        rtd_pr_rmm_info("carvedout nn start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_VIP, (void *)&addr);
        rtd_pr_rmm_info("carvedout vip start(%lx)..(%lx), size(%ld_KB))\n", addr, addr + size, (size/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_OD, (void *)&addr);
        rtd_pr_rmm_info("carvedout od start(%lx)..(%lx), size(%ld_KB))\n", addr, addr + size, (size/1024));
        size = carvedout_buf_query(CARVEDOUT_VDEC_VBM, (void *)&addr);
        rtd_pr_rmm_info("carvedout vbm start(%lx)..(%lx), size(%ld_KB))\n", addr, addr + size, (size/1024));
        size = carvedout_buf_query(CARVEDOUT_TP, (void *)&addr);
        rtd_pr_rmm_info("carvedout tp start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_AUDIO, (void *)&addr);
        rtd_pr_rmm_info("carvedout audio start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_DDR_BOUNDARY, (void *)&addr);
        if (size)
            rtd_pr_rmm_info("carvedout ddr boundary start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
    }
#endif

	return 0;
err:
	return ret;
}
EXPORT_SYMBOL(carvedout_buf_layout_build);


/*
 * reclaim_logo_X weak override
 */
#if defined(CONFIG_REALTEK_MEMORY_MANAGEMENT) // legacy and built-in only

extern void free_reserved_memory(unsigned long rsv_addr, unsigned long rsv_size);

int reclaim_logo = 0; //TODO: it could be a generic logo reclaim solution not only for module

int reclaim_logo_memory(void)
{
	// legacy reserve_dvr_memory() from cma1

	rtd_pr_rmm_notice("%s[rtd2851c]\n", __func__);

	if (pAnimation) {
		dvr_free(pAnimation);
		pAnimation = 0;
	}
	if (pAnimation_1) {
		dvr_free(pAnimation_1);
		pAnimation_1 = 0;
	}

	return 0;
}
#endif


#ifndef CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE
/* RTK_patch: handle the power on video/logo/audio memory issue */
static int __init reserve_dvr_memory(void)
{
	rtd_pr_rmm_info("[POV] reserve_dvr_memory\n");

	/*
	 * if bootcode assign last_image= in kernel cmdline, use bootcode's memory setting.
	 * if kernel_cmd doesn't exist last_image=, last image buffer use 4MB after decode buffer region.
	 */
	if (reserved_dvr_size) {

#if IS_ENABLED(CONFIG_INIT_ON_ALLOC_DEFAULT_ON)
		// hack to temporarily disable CONFIG_INIT_ON_ALLOC_DEFAULT_ON enabled by GKI default
		//  carvedout/vbm reserve/reclaim is an one-time operation to sustain buffer hand-shaking
		//  before and after kernel, it's not supposed to do any harm to the optimization intention
		//  of static_key.
		bool key_enabled;
		key_enabled = static_key_enabled(&init_on_alloc.key);
		if (key_enabled) {
			static_key_disable(&init_on_alloc.key);
		}
#endif

		rtd_pr_rmm_info("[POV] reserved_dvr_start = 0x%x\n",(unsigned int)reserved_dvr_start);
		pAnimation = dvr_malloc_specific_addr(64*1024*1024,
											  GFP_DCU1, reserved_dvr_start);
		pAnimation_1 = dvr_malloc_specific_addr(reserved_dvr_size - 64*1024*1024,
												GFP_DCU1, reserved_dvr_start + 0x04000000);

		rtd_pr_rmm_info("[POV] pAnimation(%p/%p)\n", pAnimation, pAnimation_1);

		#ifdef CONFIG_LG_SNAPSHOT_BOOT
		register_cma_forbidden_region(__phys_to_pfn(reserved_dvr_start), reserved_dvr_size);
		#endif
		rtd_pr_rmm_info("[POV] reserved_last_image_start = 0x%x\n",(unsigned int)reserved_last_image_start);
		if (reserved_last_image_size) {
			pLastImage = dvr_malloc_specific_addr(reserved_last_image_size,
												  GFP_DCU1, reserved_last_image_start);
		}
		else {
			pLastImage = dvr_malloc_specific_addr(LAST_IMAGE_SIZE,
												  GFP_DCU1, reserved_dvr_start+reserved_dvr_size);
		}
		rtd_pr_rmm_info("[POV] pLastImage(%p)\n", pLastImage);

#if IS_ENABLED(CONFIG_INIT_ON_ALLOC_DEFAULT_ON)
		if (key_enabled) {
			static_key_enable(&init_on_alloc.key);
		}
#endif

	}
	return 0;
}
core_initcall_sync(reserve_dvr_memory);
#endif

#if defined(CONFIG_OF_FLATTREE) && defined(CONFIG_FLATMEM) && defined(CONFIG_FAKEMEMORY_GRAPHICS)

#ifdef CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE
#error "!FIXME, add fake memory no support for module build"
#endif

#ifndef MIN_MEMBLOCK_ADDR
#define MIN_MEMBLOCK_ADDR	__pa(PAGE_OFFSET)
#endif
#ifndef MAX_MEMBLOCK_ADDR
#define MAX_MEMBLOCK_ADDR	((phys_addr_t)~0)
#endif
// weak symbol
void __init early_init_dt_add_memory_arch(u64 base, u64 size)
{
	const u64 phys_offset = MIN_MEMBLOCK_ADDR;

	if (size < PAGE_SIZE - (base & ~PAGE_MASK)) {
		rtd_pr_rmm_warn("Ignoring memory block 0x%llx - 0x%llx\n",
			base, base + size);
		return;
	}

	if (!PAGE_ALIGNED(base)) {
		size -= PAGE_SIZE - (base & ~PAGE_MASK);
		base = PAGE_ALIGN(base);
	}
	size &= PAGE_MASK;

	if (base > MAX_MEMBLOCK_ADDR) {
		rtd_pr_rmm_warn("Ignoring memory block 0x%llx - 0x%llx\n",
				base, base + size);
		return;
	}

	if (base + size - 1 > MAX_MEMBLOCK_ADDR) {
		rtd_pr_rmm_warn("Ignoring memory range 0x%llx - 0x%llx\n",
				((u64)MAX_MEMBLOCK_ADDR) + 1, base + size);
		size = MAX_MEMBLOCK_ADDR - base + 1;
	}

	if (base + size < phys_offset) {
		rtd_pr_rmm_warn("Ignoring memory block 0x%llx - 0x%llx\n",
			   base, base + size);
		return;
	}
	if (base < phys_offset) {
		rtd_pr_rmm_warn("Ignoring memory range 0x%llx - 0x%llx\n",
			   base, phys_offset);
		size -= phys_offset - base;
		base = phys_offset;
	}

	if (base == 0x20000000)
		size = size + FAKEMEMORY_SIZE;

	rtd_pr_rmm_info("%s: mem=%ldMB @ 0x%08lx\n", __func__, (unsigned long)size / 1024 / 1024, (unsigned long)base);

	memblock_add(base, size);
}
#endif

int cma_info_proc_show(struct seq_file *m, void *v)
{
	rtk_record_list_dump();
	return 0;
}

static int cma_info_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, cma_info_proc_show, NULL);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static const struct proc_ops cma_info_proc_fops = {
    .proc_open       = cma_info_proc_open,
    .proc_read       = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release    = single_release,
};
#else
static const struct file_operations cma_info_proc_fops = {
    .open       = cma_info_proc_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};
#endif

static int __init proc_cma_info_init(void)
{
	struct proc_dir_entry *entry;

    entry = proc_create("cma_info", 0, NULL, &cma_info_proc_fops);
	if (!entry)
		return -1;

    return 0;
}

static int __init rmm_init(void)
{
#if defined(MODULE)
	extern void early_param_init(void);
	early_param_init();
#endif

#if defined(MODULE) || defined(RTK_MEM_LAYOUT_DEVICETREE)
	if (carvedout_buf_layout_build() < 0) {
		rtd_pr_rmm_err("carvedout_buf layout build fail\n");
		return -EINVAL;
	}
#endif

	if (auth_init() < 0)
		return -EINVAL;

	if (remap_init() < 0)
		return -EINVAL;

#ifdef CONFIG_REALTEK_MANAGE_OVERLAPPED_REGION
	init_overlapped_region(RBUS_BASE_PHYS >> PAGE_SHIFT, RBUS_BASE_SIZE >> PAGE_SHIFT);
#endif

	if (proc_cma_info_init() < 0)
		rtd_pr_rmm_err("%s create /proc ERR\n", __func__);
	else 
		rtd_pr_rmm_info("%s create /proc OK\n", __func__);

	return 0;
}

static void __exit rmm_exit(void)
{
#ifdef CONFIG_REALTEK_MANAGE_OVERLAPPED_REGION
	exit_overlapped_region();
#endif
	remap_exit();
	auth_exit();
}

#if defined(CONFIG_REALTEK_MEMORY_MANAGEMENT)
pure_initcall(rmm_init);
#elif defined(CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE)
module_init(rmm_init);
module_exit(rmm_exit);
#endif
#endif //BUILD_QUICK_SHOW

MODULE_AUTHOR("Realtek.com");
MODULE_LICENSE("Dual BSD/GPL");
