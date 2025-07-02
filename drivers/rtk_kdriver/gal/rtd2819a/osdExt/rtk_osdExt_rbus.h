#ifndef __RTK_OSDEXT_RBUS_H__
#define __RTK_OSDEXT_RBUS_H__



#if 0
#include <rbus/rtd2819a/sys_reg_reg.h>


#include <rbus/rtd2819a/gdma4_reg.h>
#include <rbus/rtd2819a/gdma5_reg.h>

#include <rbus/rtd2819a/osd_sr45_reg.h>

#include <rbus/rtd2819a/osdovl4_reg.h>
#include <rbus/rtd2819a/osdovl5_reg.h>
#else
	//common/include/rbus
#include <rbus/sys_reg_reg.h>

#include <rbus/gdma_reg.h>
#include <rbus/gdma4_reg.h>
#include <rbus/gdma5_reg.h>

#include <rbus/osd_sr45_reg.h>

#include <rbus/osdovl4_reg.h>
#include <rbus/osdovl5_reg.h>

#endif//


typedef struct {



} GDMA_REG_MIXER_CONTENT;


typedef struct {



} GDMA_REG_SR_CONTENT;


typedef struct {
	//OSD_DISPLAY_PLANE disp_plane;

    /*  in GDMA */
	gdma4_osd4_ctrl_RBUS osd_ctrl_reg; 	//gdma_osd1_ctrl_RBUS osd_ctrl_reg

	//gdma_line_buffer_end_RBUS lb_end_reg;
	gdma4_osd4_RBUS  osd_reg;	// gdma_osd1_RBUS osd_reg;
	gdma4_osd4_wi_RBUS osd_wi_reg;				//	gdma_osd1_wi_RBUS osd_wi_reg;
	gdma4_osd4_size_RBUS osd_size_reg;			//  gdma_osd1_size_RBUS osd_size_reg;
	gdma4_osd4_clear1_RBUS osd_clear1_reg;		// gdma_osd1_clear1_RBUS osd_clear1_reg;
	gdma4_osd4_clear2_RBUS osd_clear2_reg;		//gdma_osd1_clear2_RBUS osd_clear2_reg;

	/*  in osd_sr */
	osd_sr45_osd_sr_4_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;	//osd_sr_osd_sr_1_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
	osd_sr45_osd_sr_4_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;

	
	osd_sr45_osd_sr_4_h_location_ctrl_RBUS osd_sr_h_location_ctrl_reg;
	osd_sr45_osd_sr_4_v_location_ctrl_RBUS osd_sr_v_location_ctrl_reg;
	
	osd_sr45_osd_sr_4_ctrl_RBUS osd_sr_ctrl_reg;		//osd_sr_osd_sr_1_ctrl_RBUS osd_sr_ctrl_reg;
	osd_sr45_osd_sr_4_filter_ctrl0_RBUS osd_sr_filter_ctrl_reg;	// osd_sr_osd_sr_1_filter_ctrl0_RBUS osd_sr_filter_ctrl_reg;
	osd_sr45_osd_sr_4_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;	//osd_sr_osd_sr_1_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;
	osd_sr45_osd_sr_4_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;//osd_sr_osd_sr_1_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;

	/*  in Mixer */
	osdovl4_mixer4_layer_sel_RBUS mixer_layer_sel_reg;		//osdovl_mixer_layer_sel_RBUS mixer_layer_sel_reg;

	osdovl4_mixer4_c0_plane_alpha1_RBUS mixer_c0_plane_alpha1_reg;	//osdovl_mixer_c0_plane_alpha1_RBUS mixer_c0_plane_alpha1_reg;
	osdovl4_mixer4_c0_plane_alpha2_RBUS mixer_c0_plane_alpha2_reg;  //osdovl_mixer_c0_plane_alpha2_RBUS mixer_c0_plane_alpha2_reg;

	osdovl4_mixer4_c1_plane_alpha1_RBUS mixer_c1_plane_alpha1_reg;  // 	osdovl_mixer_c1_plane_alpha1_RBUS mixer_c1_plane_alpha1_reg;
	osdovl4_mixer4_c1_plane_alpha2_RBUS mixer_c1_plane_alpha2_reg; // 	osdovl_mixer_c1_plane_alpha2_RBUS mixer_c1_plane_alpha2_reg;
	
} GDMA_4_REG_CONTENT;


typedef struct {
	//OSD_DISPLAY_PLANE disp_plane;
	
    /*  in GDMA */
	gdma5_osd5_ctrl_RBUS osd_ctrl_reg; 	//gdma_osd1_ctrl_RBUS osd_ctrl_reg

	//gdma_line_buffer_end_RBUS lb_end_reg;
	gdma5_osd5_RBUS  osd_reg;	// gdma_osd1_RBUS osd_reg;
	gdma5_osd5_wi_RBUS osd_wi_reg;				//	gdma_osd1_wi_RBUS osd_wi_reg;
	gdma5_osd5_size_RBUS osd_size_reg;			//  gdma_osd1_size_RBUS osd_size_reg;
	gdma5_osd5_clear1_RBUS osd_clear1_reg;		// gdma_osd1_clear1_RBUS osd_clear1_reg;
	gdma5_osd5_clear2_RBUS osd_clear2_reg;		//gdma_osd1_clear2_RBUS osd_clear2_reg;

	gdma5_osd5_pq_decmp_RBUS pq_decmp_reg;
	gdma5_osd5_pq_decmp_pair_RBUS pq_decmp_pair_reg;

	/*  in osd_sr */
	osd_sr45_osd_sr_5_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;	//osd_sr_osd_sr_1_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
	osd_sr45_osd_sr_5_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;

	osd_sr45_osd_sr_5_h_location_ctrl_RBUS osd_sr_h_location_ctrl_reg;
	osd_sr45_osd_sr_5_v_location_ctrl_RBUS osd_sr_v_location_ctrl_reg;
	
	osd_sr45_osd_sr_5_ctrl_RBUS osd_sr_ctrl_reg;		//osd_sr_osd_sr_1_ctrl_RBUS osd_sr_ctrl_reg;
	osd_sr45_osd_sr_5_filter_ctrl0_RBUS osd_sr_filter_ctrl_reg;	// osd_sr_osd_sr_1_filter_ctrl0_RBUS osd_sr_filter_ctrl_reg;
	osd_sr45_osd_sr_5_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;	//osd_sr_osd_sr_1_gainy_ctrl0_RBUS osd_sr_gainy_ctrl0_reg;
	osd_sr45_osd_sr_5_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;//osd_sr_osd_sr_1_gainmap_ctrl0_RBUS osd_sr_gainmap_ctrl_reg;
	

	/*  in Mixer */
	osdovl5_mixer5_layer_sel_RBUS mixer_layer_sel_reg;		//osdovl_mixer_layer_sel_RBUS mixer_layer_sel_reg;
	
	osdovl5_mixer5_c0_plane_alpha1_RBUS mixer_c0_plane_alpha1_reg;	//osdovl_mixer_c0_plane_alpha1_RBUS mixer_c0_plane_alpha1_reg;
	osdovl5_mixer5_c0_plane_alpha2_RBUS mixer_c0_plane_alpha2_reg;  //osdovl_mixer_c0_plane_alpha2_RBUS mixer_c0_plane_alpha2_reg;


} GDMA_5_REG_CONTENT;

#endif// __RTK_OSDEXT_RBUS_H__
