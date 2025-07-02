/*=========================================================================== */
/*o------ initial function ------o */
/*=========================================================================== */
#ifndef _OSD_SR_DRIVER_H
#define _OSD_SR_DRIVER_H

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef enum _OSD_SR_CH_SEL {
    OSD1_SR,
    OSD2_SR,
    OSD3_SR,
    OSD4_SR,
    OSD5_SR,
} OSD_SR_CH_SEL;

typedef enum _PANEL_PIXEL_NUMBER {
    PANEL_1PIXEL = 0,
    PANEL_2PIXEL,
    PANEL_4PIXEL,
} PANEL_PIXEL_NUMBER;

typedef struct {
	short sr_maxmin_range;
	short sr_lpf_range;
	short sr_edge_range;
} DRV_OSDSR_3x5;

typedef struct {
	short sr_maxmin_limit_en;
	short sr_maxmin_shift;
	short sr_var_thd;
} DRV_OSDSR_overshoot;

typedef struct {
	short sr_lpf_range_en;
	short sr_lpf_range_thd;
	short sr_lpf_range_gain;
	short sr_lpf_range_step;
} DRV_OSDSR_lpf_range;

typedef struct {
	short sr_lpf_edge_en;
	short sr_lpf_edge_thd;
	short sr_lpf_edge_gain;
	short sr_lpf_edge_step;
	short sr_delta_gain;
} DRV_OSDSR_lpf_edge;

typedef struct {
	short sr_lpmm_en;
	short sr_lpmm_1px;
	short sr_lpmm_corner;
} DRV_OSDSR_lpmm;

typedef struct {
	short sr_lpf_gain;
	short v_3x5;
	short v_overshoot;
	short v_lpf_range;
	short v_lpf_edge;
	short v_lpmm;
} DRV_OSDSR_table;

DRV_OSDSR_table* osd_sr_get__gal_osdsr_table_txt(void);
void osd_sr_set_table_gal( unsigned int offset , DRV_OSDSR_table* x );

void drv_superresolution_init(void);
void drv_superresolution_LPF_Gain(GDMA_DISPLAY_PLANE plane);
void drv_superresolution_ch_switch(GDMA_DISPLAY_PLANE src, OSD_SR_CH_SEL dest);

void drv_scaleup_init(void);
int drv_scaleup(uint8_t enable, GDMA_DISPLAY_PLANE disPlane);

int osd_sr_init_module(void);
void osd_sr_cleanup_module(void);
void osd_sr_db_apply(void);
int osd_sr_configLocation(GDMA_DISPLAY_PLANE disPlane, int x, int y);
int osd_sr_check_onoff_status(GDMA_DISPLAY_PLANE disPlane);
void osd_sr_set_lpf_gain( unsigned int offset , short sr_lpf_gain );
void osd_sr_update_txt( short* x );

#endif		/* end of _OSD_SR_DRIVER_H */
