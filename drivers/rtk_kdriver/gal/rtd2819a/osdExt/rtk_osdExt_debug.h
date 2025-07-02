
#include <rtd_log/rtd_module_log.h>
#include <gal/rtk_gdma_export.h>

#define GDMA_EXT_LOG_TMP (11)
#define GDMA_EXT_LOG_ALL (9)
#define GDMA_EXT_LOG_VERBOSE (7)
#define GDMA_EXT_LOG_DEBUG  (5)
#define GDMA_EXT_LOG_WARNING (3)
#define GDMA_EXT_LOG_ERROR (2)
#define GDMA_EXT_LOG_FATAL (1)
#define GDMA_EXT_LOG_DUMP (GDMA_EXT_LOG_FATAL)	

extern int gDebugGDMAExt_loglevel;

typedef enum 
{
	GDMAExt_DEBUG_CMD_DUMP=1,

	GDMAExt_DEBUG_CMD_FILL=2,	//fill color

	GDMAExt_DEBUG_CMD_SEND=3,

	GDMAExt_DEBUG_CMD_BATCH=4,
	
	GDMAExt_DEBUG_CMD_MIXER=5,

	GDMAExt_DEBUG_CMD_DEBUG=9,

} GDMAExt_DEBUG_CMD;

typedef enum 
{

	DEBUG_PARAM_1_COLOR=1,
	DEBUG_PARAM_1_SEND_OSD4=2,
	DEBUG_PARAM_1_SEND_OSD5=3,

	DEBUG_PARAM_1_COLOR_PTG_SHIFT=4,
	DEBUG_PARAM_1_COLOR_PTG_BarWidth=5,
	DEBUG_PARAM_1_COLOR_PTG_PATTERN_IDX=6,

} GDMAExt_DBG_PARAM_1;


#define GDMA_EXT_LOGT(level,fmt, arg...) do { \
		if( unlikely(gDebugGDMAExt_loglevel >= level) ) { rtd_pr_gdma_emerg("TT:%lld L%d "  fmt, ktime_to_us(ktime_get()), level, ##arg ); } \
	   } while(0) 

#define GDMA_EXT_LOG(level,fmt, arg...) do { \
		if( unlikely(gDebugGDMAExt_loglevel >= level) ) { rtd_pr_gdma_emerg("L%d "  fmt, level, ##arg ); } \
	   } while(0) 


#define GDMAEXT_ERROR(fmt, arg...) GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, fmt, arg)
#define GDMAEXT_WARNING(fmt, arg...) GDMA_EXT_LOG(GDMA_EXT_LOG_WARNING, fmt, arg)
#define GDMAEXT_DEBUG(fmt, arg...) GDMA_EXT_LOG(GDMA_EXT_LOG_DEBUG, fmt, arg)
#define GDMAEXT_LOG(fmt, arg...) GDMA_EXT_LOG(GDMA_EXT_LOG_VERBOSE, fmt, arg)
#define GDMAEXT_TMP(fmt, arg...) GDMA_EXT_LOG(GDMA_EXT_LOG_TMP, fmt, arg)
#define GDMAEXT_DUMP(fmt, arg...) GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP, fmt, arg)

#define GDMAEXT_DEBUGT(fmt, arg...) GDMA_EXT_LOGT(GDMA_EXT_LOG_DEBUG, fmt, arg)

void gdmaExt_osd_winfo_dump( int plane );

void attach_buf_to_plane(VO_GRAPHIC_PLANE plane, GDMA_LAYER_OBJECT_V3 *pdl, int buf_idx );