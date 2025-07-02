#ifndef _OPTEE_RT_MMU_PGTABLE_H
#define _OPTEE_RT_MMU_PGTABLE_H

typedef struct {
	unsigned int    HTotal;     /* h_threshold */
	unsigned int    VTotal;     /* v_threshold */
	unsigned int    HWidth;     /* h_active */
	unsigned int    VHeight;    /* v_active */
	unsigned int    HStartPos;  /* h_active_start */
	unsigned int    VStartPos;  /* v_active_start */
	unsigned int    isProg;     /* progressive or interlace */
	unsigned int    SampleRate; /* sample rate in 100Hz */
	unsigned int    FrameRate;  /* Frame Rate, //vfreq unit 0.001Hz */
} VO_TIMING;

typedef enum{
	CHROMA_YUV420 = 0,//v note. DON't change default value 420
	CHROMA_YUV422,
	CHROMA_YUV422V,
	CHROMA_YUV444,
	CHROMA_ARGB8888,
	CHROMA_RGB888,
	CHROMA_RGB565,
	CHROMA_GRAY,
	CHROMA_YUV411,
	VODMA_CHROMA_NUM
} VODMA_CHROMA_FORMAT;

typedef enum
{
    TIMING_MODE_480I = 0, 	// 858x525_720x480_60I,
    TIMING_MODE_480P, 		// 858x525_720x480_60P,
    TIMING_MODE_576I,		// 864x625_720x576_50I,
    TIMING_MODE_576P,		// 864x625_720x576_50P,
    TIMING_MODE_720P50,		// 1980x750_1280x720_50P,
    TIMING_MODE_720P60, 	// 1650x750_1280x720_60P,
    TIMING_MODE_1080I50, 	// 2200x1125_1920x1080_50I,
    TIMING_MODE_1080I60, 	// 2200x1125_1920x1080_60I,
    TIMING_MODE_1080P50, 	// 2200x1125_1920x1080_50P,
    TIMING_MODE_1080P60, 	// 2200x1125_1920x1080_60P,
    TIMING_MODE_SXGA,		// 1688X1066_1280X1024_60p,
    TIMING_MODE_2160P30,    // 4120x2192_3840x2160_30P,
    TIMING_MODE_2160P60,    // 4120x2192_3840x2160_60P,
    VODMA_TIMING_MODE_NUM
} VODMA_TIMING_MODE ;

typedef enum
{
    DMA_SEQUENTIAL_MODE = 0,
    DMA_BLOCK_MODE,
} VODMA_DMA_MODE;


int rt_mmu_pgtable_init(void);
void rt_mmu_pgtable_deinit(void);
int rt_mmu_pgtable_get_entries (unsigned long iova, unsigned long size, unsigned int *pgtable);
int rt_mmu_pgtable_set_entries (unsigned long iova, unsigned long size, unsigned int dma, unsigned int *pgtable);
int rt_mmu_pgtable_unset_entries (int module_id, unsigned int *pa_array);
int rt_mmu_pgtable_debug(unsigned long iova, unsigned long size, unsigned int *pgtable, int *result);

int mm_rt_mmu_sysfs(void);

#endif

