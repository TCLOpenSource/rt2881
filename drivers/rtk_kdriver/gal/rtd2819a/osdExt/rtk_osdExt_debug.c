#include <rbus/dc2h2_cap_reg.h> 
#include <rbus/osd_sr45_reg.h>
#include <rbus/osd_sr_reg.h>
#include <rbus/ppoverlay_reg.h> 


#include "rmm/pageremap.h" //dvr_malloc
#include "rtk_osdExt_debug.h"


#include "rtk_osdExt_main.h"
#include "rtk_osdExt_driver.h"
#include "rtk_osdExt_reg.h"
#include "rtk_osdExt_sr.h"
#include "rtk_osdExt_rbus.h"

#include "rtk_gdma_fence.h"

#include "gal/rtk_gdma_export_user.h"

#include "gal/rtk_gdma_driver.h"

#include <linux/fs.h>
#include <linux/module.h>


int gDebugGDMAExt_loglevel = 2; // default to 2  GDMA_EXT_LOG_ERROR

extern int gVsyncSource_id;
extern int gDTG_APP_TYPE_osd;

extern gdma_dev *gdma_devices;

extern GDMAExt_device_cb g_osd_cb[GDMA_PLANE_ALL_MAXNUM];
extern int g_osdExt_stop_update;

//extern void getDispSize(VO_RECTANGLE *disp_rec);

extern int GDMAExt_modify_mixer(GDMA_MODIFY_MIXER_CMD* mixer_cmd);

#define ALIGN_size(x, y) ((((x) + (y - 1)) / y) * y)
//align osd pitch to 16 
#define ALIGN_PICTH  (16)

typedef struct {
	int buf_id;		//start from 0
	//unsigned long* vir_addr;
	void* vir_addr;
	void* uncache_vir_addr;
	unsigned long phy_addr;
	int buf_width;
	int buf_height;
	int buf_pitch;
	unsigned int buf_size;
	SOURCE_TYPE src_type;
	VO_OSD_COLOR_FORMAT color_fmt;
} GDMA_TEST_COLOR_BUF_INFO;

#define TEST_COLORBUF_MAX_SIZE (5)
GDMA_TEST_COLOR_BUF_INFO g_colorBuf[TEST_COLORBUF_MAX_SIZE];

static int g_DBG_PTG_BarWidth=0;
static int g_DBG_PTG_ColorShift = 0;

static int g_DBG_PTG_ColorPattern_idx = 1;


//#define OSD_EXT_FILE_TEST

#ifdef OSD_EXT_FILE_TEST
static struct file* dbg_file_open(const char* path, int flags, int rights)
{
        struct file* filp = NULL;
        int err = 0;

        filp = filp_open(path, flags, rights);
        if(IS_ERR(filp)) {
                err = PTR_ERR(filp);

			rtd_pr_gdma_err("dbg_file_open fail when open:%s %d\n", path, err);
            return NULL;
        }
		
        return filp;
}

static void file_close(struct file* file)
{
        filp_close(file, NULL);
}
 

static void read_test_file_1(char* path, int size, int  buf_idx)
{
	loff_t  offset = 0;
	char *buf_st = NULL;
	struct file * filep;


	 buf_st = (char *)g_colorBuf[buf_idx].vir_addr;

	if( buf_st == NULL ) {
		rtd_pr_gdma_crit("can't map mem 0x%llx %d\n", g_colorBuf[buf_idx].phy_addr, size);
		return;
	}
	
	filep = dbg_file_open(path,  O_RDWR | O_CREAT, 0644);

	if( filep != NULL )
	{
	     //char arr[100];
     
	     kernel_read(filep, buf_st, size, &offset); 

	     //memset(arr, '\0', 100);
	     
	     rtd_pr_gdma_crit(" read file %s  addr:0x%llx %d\n", path, g_colorBuf[buf_idx].phy_addr, size ); 

	     file_close(filep);

   }

	 
	 // vir_addr should be "void*"" type
	dmac_flush_range((void*)g_colorBuf[buf_idx].vir_addr, (void*)(g_colorBuf[buf_idx].vir_addr + g_colorBuf[buf_idx].buf_size)) ;
	//outer_flush_range is empty
	



}


#endif//OSD_EXT_FILE_TEST

#if 1
void gdmaExt_osd_winfo_dump( int plane )
{
	gdma_dbg_osd_wi_c_RBUS dbg_wi_attr;
	
	// order: osd 4, 5, 1 
    unsigned int base_regs[] = { GDMA4_OSD4_CTRL_reg,  GDMA5_OSD5_CTRL_reg, GDMA_OSD1_CTRL_reg } ; 
	unsigned int dbg_winBase_regs[] = { GDMA4_DBG_OSD_WI_0_reg, GDMA5_DBG_OSD5_WI_0_reg, GDMA_DBG_OSD_WI_0_reg};
	unsigned int osd_wi_regs[] = { GDMA4_OSD4_WI_reg, GDMA5_OSD5_WI_reg, GDMA_OSD1_WI_reg};

	unsigned int win_reg_addr = 0;
	unsigned long regvalue = 0;
	int index = 0;

	unsigned int base_reg_addr = 0;
	
	GDMA_WIN* winfo = NULL;

	VO_RECTANGLE res;
	GDMAExt_getDispSize( (GDMA_DISPLAY_PLANE)(plane), &res );

	GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP, "\n====osd:%d panel info: [%d*%d]\n", plane, res.width, res.height);


	if( plane == GDMA_PLANE_OSD4) {
		index = 0;
	}
	else if ( plane == GDMA_PLANE_OSD5 ) {
		index = 1;
	}
	else {
		//GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP," invalid plane: %d \n", plane);
		index = 2;
	}


    {
		winfo = NULL;
		win_reg_addr = osd_wi_regs[index];
        regvalue = rtd_inl( win_reg_addr);
		if( regvalue )
			winfo = (GDMA_WIN*)phys_to_virt(regvalue);

		if(winfo == NULL) {
			//rtd_pr_gdma_err(KERN_ERR"\n====OSD-%d is None\n\n",osdn);
            GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP," OSD winInfo 0x%lx is None\n\n", win_reg_addr);

			return ;
		}

		//osd 1/4/5 have same wininfo 
		GDMAEXT_DUMP( "====OSD-%d info===\n", plane );
		GDMAEXT_DUMP( "DDR winfo addr= 0x(%lx) phy:0x%lx\n", winfo, regvalue);
		GDMAEXT_DUMP( "winfo canvas= (%d,%d,%d,%d), offsetXY=(%d,%d)\n",
			winfo->winXY.x,winfo->winXY.y,
			winfo->winWH.width,winfo->winWH.height,
			winfo->objOffset.objXoffset,winfo->objOffset.objYoffset);
		GDMAEXT_DUMP( "\twinfo pitch= (%d) addr=0x%lx\n",
			winfo->pitch, winfo->top_addr);
		GDMAEXT_DUMP( "\tFormat=0x%lx imgComp=%d(%d)\n",
			winfo->attr.type,
			winfo->attr.IMGcompress?1:0, winfo->attr.compress);


		base_reg_addr = dbg_winBase_regs[index];
		// 0xb81C4240(GDMA4_DBG_OSD_WI_C_reg) 

		GDMAEXT_DUMP("WIN dbg: [%d %d %d %d] pitch:%d offset:%d %d\n", 
			GDMA4_DBG_OSD_WI_4_get_xb( OSD_RTD_INL(base_reg_addr + 4) ),
			GDMA4_DBG_OSD_WI_4_get_yb( OSD_RTD_INL(base_reg_addr + 4) ),
            GDMA4_DBG_OSD_WI_8_get_width( OSD_RTD_INL(base_reg_addr + 8) ),
            GDMA4_DBG_OSD_WI_8_get_height( OSD_RTD_INL(base_reg_addr + 8) ) ,
			GDMA4_DBG_OSD_WI_20_get_pitch( OSD_RTD_INL(base_reg_addr + 0x20) ),
			GDMA4_DBG_OSD_WI_24_get_x_init_offset( OSD_RTD_INL(base_reg_addr + 0x24)),
			GDMA4_DBG_OSD_WI_24_get_y_init_offset( OSD_RTD_INL(base_reg_addr + 0x24))

		);

	GDMAEXT_DUMP(" addr: [0x%x 0x%x 0x%x 0x%x ] \n", 
			 OSD_RTD_INL(base_reg_addr + 0x18) ,
			 OSD_RTD_INL(base_reg_addr + 0x1c) ,
             OSD_RTD_INL(base_reg_addr + 0x28) ,
             OSD_RTD_INL(base_reg_addr + 0x2c) 
			);

		regvalue = OSD_RTD_INL(base_reg_addr + 0xC  );
		dbg_wi_attr.regValue = regvalue;
		GDMAEXT_DUMP(" attr: \n baseReg:0x%x Attr: colorType=0x%x imgComp:%d(comp:%d) PreCLUT:0x%x littleEndian:0x%x clut_fmt:0x%x alpha:0x%x objType:0x%x alphaEn:0x%x alphaType:0x%x rgbOrder:0x%x extendMode:0x%x\n", 
					base_reg_addr, 
			dbg_wi_attr.colortype, dbg_wi_attr.img_compress, dbg_wi_attr.compress, 
			dbg_wi_attr.keep_prev_clut, dbg_wi_attr.endian, dbg_wi_attr.clut_format, 
			dbg_wi_attr.alpha,  dbg_wi_attr.object_type, dbg_wi_attr.const_alpha_enable, 
			dbg_wi_attr.alpha_replace_type, dbg_wi_attr.rgb_order, dbg_wi_attr.extension_mode);

		regvalue = OSD_RTD_INL(base_reg_addr + 0x14 );

		GDMAEXT_DUMP(" WI_14(colr): en:%d %d   %d %d \n", 
			GDMA4_DBG_OSD_WI_14_get_color_range_en( regvalue ),
            GDMA4_DBG_OSD_WI_14_get_color_key_en( regvalue ),
			GDMA4_DBG_OSD_WI_14_get_key_min_29_24( regvalue ),
            GDMA4_DBG_OSD_WI_14_get_key_min( regvalue ) );


		regvalue = rtd_inl( base_regs[index] );
		switch(plane)
		{
			case GDMA_PLANE_OSD4:
				GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP, "Enable = %d, VFlip = %d\n",
					GDMA4_OSD4_CTRL_get_osd4_en(regvalue)?1:0,
					GDMA4_OSD4_CTRL_get_rotate(regvalue)?1:0);

					//SR dump 
					if( OSD_SR_OSD_SR_1_Scaleup_Ctrl1_get_h_zoom_en(rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg)) ) {
						GDMAEXT_DUMP("\n====OSDSR-4 : X=%d, Width=%d\n",
						OSD_SR45_OSD_SR_4_H_Location_Ctrl_get_osd_h_sta(rtd_inl(OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg)),
						OSD_SR45_OSD_SR_4_H_Location_Ctrl_get_osd_width(rtd_inl(OSD_SR45_OSD_SR_4_H_Location_Ctrl_reg)));
					}
					else {
						GDMAEXT_DUMP("\n====OSDSR-4 : h zoom NO. 0x%08x\n", rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl1_reg) );

					}

					if(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_get_v_zoom_en(rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg))) {
						GDMAEXT_DEBUG(KERN_ERR"\n====OSDSR-4 : Y=%d, Height=%d\n",
						OSD_SR45_OSD_SR_4_V_Location_Ctrl_get_osd_v_sta(rtd_inl(OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg) ),
						OSD_SR45_OSD_SR_4_V_Location_Ctrl_get_osd_height(rtd_inl(OSD_SR45_OSD_SR_4_V_Location_Ctrl_reg)) );
					}
					else {
						GDMAEXT_DUMP("\n====OSDSR-4 : v zoom NO. 0x%08x\n", rtd_inl(OSD_SR45_OSD_SR_4_Scaleup_Ctrl0_reg) );
					}


						// [FIXME] show SR ?
				break;
			case GDMA_PLANE_OSD5:
				GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP, "Enable = %d, VFlip = %d\n",
					GDMA5_OSD5_CTRL_get_osd5_en(regvalue)?1:0,
					GDMA5_OSD5_CTRL_get_rotate(regvalue)?1:0);
				
				//SR dump 
				if( OSD_SR_OSD_SR_1_Scaleup_Ctrl1_get_h_zoom_en(rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg)) ) {
					GDMAEXT_DUMP("\n====OSDSR-5 : X=%d, W=%d\n",
					OSD_SR45_OSD_SR_4_H_Location_Ctrl_get_osd_h_sta(rtd_inl(OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg)),
					OSD_SR45_OSD_SR_4_H_Location_Ctrl_get_osd_width(rtd_inl(OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg)));
				}
				else {
					GDMAEXT_DUMP("\n====OSDSR-5 : h zoom NO. 0x%08x\n", rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg) );

				}

				if(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_get_v_zoom_en(rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg))) {
					GDMAEXT_DEBUG(KERN_ERR"\n====OSDSR-5 : Y=%d, H=%d\n",
					OSD_SR45_OSD_SR_4_V_Location_Ctrl_get_osd_v_sta(rtd_inl(OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg) ),
					OSD_SR45_OSD_SR_4_V_Location_Ctrl_get_osd_height(rtd_inl(OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg)) );
				}
				else {
					GDMAEXT_DUMP("\n====OSDSR-4 : v zoom NO. 0x%08x\n", rtd_inl(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg) );
				}


				break;

			default: //assume osd 1
				GDMA_EXT_LOG(GDMA_EXT_LOG_DUMP, "Enable = %d, VFlip = %d\n",
					GDMA_OSD1_CTRL_get_osd1_en(regvalue)?1:0,
					GDMA_OSD1_CTRL_get_rotate(regvalue)?1:0);

				if(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_get_h_zoom_en(rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl1_reg))) {
					rtd_pr_gdma_err(KERN_ERR"\n====OSDSR-1 : X=%d, W=%d\n",
						OSD_SR_OSD_SR_1_H_Location_Ctrl_get_osd_h_sta(rtd_inl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg)),
						OSD_SR_OSD_SR_1_H_Location_Ctrl_get_osd_width(rtd_inl(OSD_SR_OSD_SR_1_H_Location_Ctrl_reg)));
				}

				if(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_get_v_zoom_en(rtd_inl(OSD_SR_OSD_SR_1_Scaleup_Ctrl0_reg))) {
					rtd_pr_gdma_err(KERN_ERR"\n====OSDSR-1 : Y=%d, H=%d\n",
						OSD_SR_OSD_SR_1_V_Location_Ctrl_get_osd_v_sta(rtd_inl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg)),
						OSD_SR_OSD_SR_1_V_Location_Ctrl_get_osd_height(rtd_inl(OSD_SR_OSD_SR_1_V_Location_Ctrl_reg)));
				}

				if(OSD_SR_OSD_SR_2_Scaleup_Ctrl1_get_h_zoom_en(rtd_inl(OSD_SR_OSD_SR_2_Scaleup_Ctrl1_reg))) {
					rtd_pr_gdma_err(KERN_ERR"\n====OSDSR-2 : X=%d, W=%d\n",
						OSD_SR_OSD_SR_2_H_Location_Ctrl_get_osd_h_sta(rtd_inl(OSD_SR_OSD_SR_2_H_Location_Ctrl_reg)),
						OSD_SR_OSD_SR_2_H_Location_Ctrl_get_osd_width(rtd_inl(OSD_SR_OSD_SR_2_H_Location_Ctrl_reg)));
				}

				if(OSD_SR_OSD_SR_2_Scaleup_Ctrl0_get_v_zoom_en(rtd_inl(OSD_SR_OSD_SR_2_Scaleup_Ctrl0_reg))) {
					rtd_pr_gdma_err(KERN_ERR"\n====OSDSR-2 : Y=%d, H=%d\n",
						OSD_SR_OSD_SR_2_V_Location_Ctrl_get_osd_v_sta(rtd_inl(OSD_SR_OSD_SR_2_V_Location_Ctrl_reg)),
						OSD_SR_OSD_SR_2_V_Location_Ctrl_get_osd_height(rtd_inl(OSD_SR_OSD_SR_2_V_Location_Ctrl_reg)));
				}

				break;

		}

	}


	
}
#endif//0

//note buffer is allocated static inside 

void dump_colorbuffer_info(int buf_id)
{

	GDMAEXT_DUMP( "colorbuf %d vir:%p phy:0x%lx  [%d %d %d]\n", buf_id, g_colorBuf[buf_id].vir_addr, 
	g_colorBuf[buf_id].phy_addr, g_colorBuf[buf_id].buf_width, g_colorBuf[buf_id].buf_height, g_colorBuf[buf_id].buf_pitch);

}

/*


param g_DBG_PTG_ColorShift
	0: no shift 
	1: color pattern shift 1 color
	


 param g_DBG_PTG_BarWidth
	0: same color spread  width/num_colors  pixels 
	n: each color spread N pixels 
	
*/

static unsigned int g_org_color_data[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF };  //first is alpha
 //last is alpha. should be default { R, G, B, Black, White }
static unsigned int g_org_color_data_1[] = { 0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0x000000FF, 0xFFFFFFFF };

// if phy_addr ==0, do alloc
void do_fill_colorbar_pseudo(int width, int height, int buf_id, unsigned long phy_addr, SOURCE_TYPE src_type, VO_OSD_COLOR_FORMAT color_fmt)
{
	int pixel_bits = 32;
	//if( src_type ==  SRC_NORMAL_PQDC) 
	if( src_type ==  SRC_NORMAL_PQDC) {

		g_colorBuf[buf_id].vir_addr = 0;
	}
	else 
	{

		//g_colorBuf[buf_id].vir_addr = 0;
	}

		g_colorBuf[buf_id].phy_addr = phy_addr;
			//g_colorBuf[buf_id].uncache_vir_addr = dvr_remap_uncached_memory(g_colorBuf[buf_id].phy_addr, width*height* 4, __builtin_return_address(0));

		g_colorBuf[buf_id].buf_width = width;
		g_colorBuf[buf_id].buf_height = height;
			//g_colorBuf[buf_id].buf_pitch = ALIGN_size(g_colorBuf[buf_id].buf_height, ALIGN_PICTH );  //need enlarge buf total size
		g_colorBuf[buf_id].buf_pitch = width * pixel_bits / 8;;
		g_colorBuf[buf_id].buf_size = g_colorBuf[buf_id].buf_pitch * height;
			
		g_colorBuf[buf_id].src_type = src_type;//default
		g_colorBuf[buf_id].color_fmt = color_fmt; 

}

void do_fill_colorbar( int width, int height, int buf_id )
{
	unsigned int* dst_ptr = NULL;
	unsigned int* dst_ptr_tmp = NULL;
	void* color_addr = NULL;
	
	unsigned int i, j, color_idx = 0;
	unsigned int new_idx = 0;
			unsigned int start_x, end_x;
	unsigned int num_colors = 0;
	unsigned int seg_size = 0;
	int tmp_color_idx = 0;
	

	//unsigned int color_data[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF};
	VO_OSD_COLOR_FORMAT color_fmt = VO_OSD_COLOR_FORMAT_RGBA8888_LITTLE;
	int pixel_bits = 32;

	unsigned int* color_data = NULL;
	unsigned int* ptr_org_color_data = NULL;

	if( g_DBG_PTG_ColorPattern_idx == 1 ) {
		num_colors = sizeof(g_org_color_data_1)/sizeof(g_org_color_data_1[0]);
		ptr_org_color_data = (unsigned int*)&g_org_color_data_1;
	}
	else {
		//default
		num_colors = sizeof(g_org_color_data)/sizeof(g_org_color_data[0]);

		ptr_org_color_data = (unsigned int*)&g_org_color_data;
	}
		
 	seg_size = width / num_colors;

	color_data = kmalloc(sizeof(int)*num_colors, GFP_KERNEL);

	if( color_data == NULL ) {
		GDMAEXT_DUMP( "fail! color pattern %d [%d %d]\n",  g_DBG_PTG_ColorPattern_idx, width, height );
				
		return;
	}

	// if( g_DBG_PTG_ColorShift != 0 ) 
	{
		GDMAEXT_DUMP( "%d color pat:\n", __LINE__);

		for( i=0; i < num_colors; i++) {
			new_idx = (i + g_DBG_PTG_ColorShift)%num_colors;

			color_data[i] = ptr_org_color_data[ new_idx ];
			GDMAEXT_DUMP( "i=%d(%d) 0x%08x ", i, new_idx, color_data[i]  );
		}
		GDMAEXT_DUMP("#%d color pat end", num_colors );
	}

	if( g_colorBuf[buf_id].buf_width != width || g_colorBuf[buf_id].buf_height != height )
	{
		// dvr_malloc_uncached_specific
		unsigned int size = width*height* 4;
			color_addr = dvr_malloc_specific( size, GFP_DCU2_FIRST);

			if( color_addr == NULL) {
				GDMAEXT_DUMP( "%s,fail! alloc memory fail %d %d\n",__func__, width, height );
				
				return;
			}
			// gdrop_counter_phy = dvr_to_phys(gdrop_counter_vir);

			if(  g_colorBuf[buf_id].vir_addr ) {
				//free old
				dvr_free( g_colorBuf[buf_id].vir_addr );

				GDMAEXT_DUMP( "free old color buf vir:%p phy:0x%lx  [%d %d %d]\n", g_colorBuf[buf_id].vir_addr, 
					g_colorBuf[buf_id].phy_addr, g_colorBuf[buf_id].buf_width, g_colorBuf[buf_id].buf_height, g_colorBuf[buf_id].buf_pitch);

			}

			 g_colorBuf[buf_id].vir_addr = color_addr;
			 g_colorBuf[buf_id].phy_addr = dvr_to_phys( color_addr );
			//g_colorBuf[buf_id].uncache_vir_addr = dvr_remap_uncached_memory(g_colorBuf[buf_id].phy_addr, width*height* 4, __builtin_return_address(0));

			g_colorBuf[buf_id].buf_width = width;
			g_colorBuf[buf_id].buf_height = height;
			//g_colorBuf[buf_id].buf_pitch = ALIGN_size(g_colorBuf[buf_id].buf_height, ALIGN_PICTH );  //need enlarge buf total size
			g_colorBuf[buf_id].buf_pitch = width * pixel_bits / 8;
			g_colorBuf[buf_id].buf_size = size;
			
			g_colorBuf[buf_id].color_fmt = color_fmt;
			g_colorBuf[buf_id].src_type = SRC_NORMAL;//default
	}
	
	//color_addr = g_colorBuf[buf_id].uncache_vir_addr ;
	color_addr = g_colorBuf[buf_id].vir_addr ;


	dst_ptr = (unsigned int*)color_addr;  //for write each 8 byte pixel 

	//dst_ptr = (unsigned int*)0x10800000;	//test only 
	

	GDMAEXT_DUMP(" %s bufID: %d seg_size:%d dst_ptr:%p [%d %d] barWidth:%d ColorShift:%d ptgIdx:%d\n", __FUNCTION__,buf_id, seg_size, dst_ptr, width, height,
		g_DBG_PTG_BarWidth, g_DBG_PTG_ColorShift, g_DBG_PTG_ColorPattern_idx
	);

  if( g_DBG_PTG_BarWidth != 0 ) 
  {

		color_idx = 0;

	do {

		start_x = color_idx * g_DBG_PTG_BarWidth;
		end_x = start_x + g_DBG_PTG_BarWidth;
		
		if( end_x >= width)
			end_x = width;


		for( i= start_x ; i <end_x ; ++i)
		{
			for( j=0; j < height; ++j)
			{
			
				*(dst_ptr+ i+ j*(width) ) = color_data[color_idx]; 
			}
			
		}
		GDMAEXT_DUMP("Astart_x:%d end_x:%d color:0x%08x\n", start_x, end_x, color_data[color_idx] );

		if( ++color_idx >= num_colors )
			color_idx = 0;


	} while( end_x >= width);


  }
  else 
  {
	  // g_DBG_PTG_BarWidth = 0
	for( color_idx=0; color_idx < num_colors; ++color_idx)
	{
		start_x = color_idx * seg_size;
		//end_x = start_x + (color_idx +1 * seg_size);
		end_x = start_x + (  seg_size );
		
		if( end_x >= width)
			end_x = width;
		
		
		//color bar, row have same color
		for( i= start_x ; i <end_x ; ++i)
		{
			for( j=0; j < height; ++j)
			{
				dst_ptr_tmp = dst_ptr + i+ j*(width);

				tmp_color_idx = color_idx;

				#if 0
					*dst_ptr_tmp = color_data[tmp_color_idx];

				#else

				if( j < height/4 ) {
					tmp_color_idx = color_idx;
				}
				else if ( j <= height/2) {
					//shift one color 
					
					tmp_color_idx = tmp_color_idx +1;

				}
				else if ( j <= (height/4)*3 ) {
					tmp_color_idx = tmp_color_idx + 2;
				}
				else if ( j <= height) {
					tmp_color_idx = tmp_color_idx + 3;
				}

				tmp_color_idx  = tmp_color_idx % num_colors;


				//*dst_ptr_tmp = color_data[color_idx]; 
				*dst_ptr_tmp = color_data[tmp_color_idx]; 

				#endif//

				

				if( i ==0 && j ==0 ) {

					GDMAEXT_DUMP(" kk 0 addr:%p 0x%x org:0x%x data:0x%08x\n", dst_ptr_tmp,  dst_ptr_tmp, dst_ptr, *dst_ptr_tmp );

				}
			}

			#if 0
			if( j <= 3) {
				GDMAEXT_DUMP("jPos %d %d , ptr:0x%x \n", i, j, dst_ptr_tmp );
			}
			#endif//
			
		}
		GDMAEXT_DUMP("Bstart_x:%d end_x:%d color:0x%08x\n", start_x, end_x, color_data[tmp_color_idx] );

	}
  }


  {
    // vir_addr should be "void*"" type
	dmac_flush_range((void*)g_colorBuf[buf_id].vir_addr, (void*)(g_colorBuf[buf_id].vir_addr + g_colorBuf[buf_id].buf_size)) ;
	//outer_flush_range is empty
	//outer_flush_range((phys_addr_t)g_colorBuf[buf_id].phy_addr, (phys_addr_t)(g_colorBuf[buf_id].phy_addr + g_colorBuf[buf_id].buf_size) );
  }

	if(color_data) {
		kfree(color_data);
	}

}

typedef enum {
	TFBC_GRP_0_LOSELESSS = 0, //GRP_1_LOSELESS
	TFBC_GRP_0_LOSSY75 = 1,
	TFBC_GRP_0_LOSSY50 = 2,
	TFBC_GRP_0_LOSSY25 = 3,

	#if 1
	// rtd2875q need work around
	TFBC_GRP_1_LOSSY50 = 0x2,
	TFBC_GRP_1_LOSSY37 = 0x1,
	#else
	TFBC_GRP_1_LOSSY50 = 0x1,
	TFBC_GRP_1_LOSSY37 = 0x2,
	#endif//

	TFBC_GRP_1_LOSSY25 = 0x3,
	
} RTK_TFBC_COMPRESS_RATIO;


	const GDMA_LAYER_OBJECT_V3 gdma_form0 = {
    .src_type = SRC_NORMAL, .format = VO_OSD_COLOR_FORMAT_ARGB8888_LITTLE, .fbdc_format = FBDC_COLOR_FORMAT_U8,
    .context = 0, .key_en = 0, .colorkey = -1, .alpha = 0,
    .x = 0, .y = 0, .width = 0, .height = 0, .address = 0x0, .pitch = 0x0, .dst_x = 0, .dst_y = 0, .dst_width = 0, .dst_height = 0,
    .picLayout = INBAND_CMD_GRAPHIC_2D_MODE, .handle = 0x0,
    .plane_ar = { .value = 0x00FF00FF }, .plane_gb = { .value = 0x00FF00FF },
    .clear_x = { .value = 0x0 }, .clear_y = { .value = 0x0 }, .show = 1, .syncstamp = 0, .compressed = 0, .decompress = 0,
    .scale_factor = 1, .paletteIndex = 0 };

#define VALID_GDMA_MAGIC_KEY(key)    (key == 0x65776871)
#define SET_GDMA_MAGIC_KEY(key)    (key = 0x65776871)


extern int GDMAExt_startHandleLayers_internal( GRAPHIC_LAYERS_OBJECT_V3*  data );

// p2: buf_idx
void do_send_picture( int p1, int p2 )
{
	VO_GRAPHIC_PLANE plane = VO_GRAPHIC_OSD4;
	GRAPHIC_LAYERS_OBJECT_V3   gdma_layers_info;
	GDMA_LAYER_OBJECT_V3* pdl;
//	int layer_idx = 0;
	int buf_idx = p2;

	plane = (VO_GRAPHIC_PLANE)p1;

	memset(&gdma_layers_info, 0, sizeof(gdma_layers_info) );

	gdma_layers_info.layer_num = 1;
    gdma_layers_info.cookie = -1;
	
	gdma_layers_info.expectedPresentTime = -1;
	gdma_layers_info.version = GDMA_GRAPHIC_LAYERS_OBJECT_V3_VER;
	gdma_layers_info.size = sizeof( gdma_layers_info ); 


	SET_GDMA_MAGIC_KEY(gdma_layers_info.gdma_fence_magic_key);
    gdma_layers_info.gdma_fence_handle = 0;  //gdma_fence *gf. set to null currently 

	pdl = &gdma_layers_info.layer[0];

	
	
	//p2 is buf_idx 
	attach_buf_to_plane (plane, pdl, buf_idx );

	//call 
	if( plane == VO_GRAPHIC_OSD1 || plane == VO_GRAPHIC_OSD2 ) {

		GDMA_ReceiveGraphicLayers_V3_core(&gdma_layers_info);
	}
	else {
		GDMAExt_startHandleLayers_internal(&gdma_layers_info);
	}

}

/*


  buf_idx: global buffer 
*/
void attach_buf_to_plane(VO_GRAPHIC_PLANE plane, GDMA_LAYER_OBJECT_V3 *pdl, int buf_idx )
{
	//GRAPHIC_LAYERS_OBJECT_V3 gdma_layers_info;
	GDMA_DISPLAY_PLANE disPlane = GDMA_PLANE_OSD4;


	unsigned long buf_phy_addr;
	int buf_width;
	int buf_height;
	int buf_pitch;

	VO_RECTANGLE disp_rec;

		if (plane == VO_GRAPHIC_OSD1)
			disPlane = GDMA_PLANE_OSD1;
		else if (plane == VO_GRAPHIC_OSD2)
			disPlane = GDMA_PLANE_OSD2;
		else if ( plane == VO_GRAPHIC_OSD4)
			disPlane = GDMA_PLANE_OSD4;
		else if ( plane == VO_GRAPHIC_OSD5)
			disPlane = GDMA_PLANE_OSD5;

	GDMAExt_getDispSize(disPlane, &disp_rec);

	
	*pdl = gdma_form0;
	//GDMA_LAYER_OBJECT_V3* o5_layer;

	buf_phy_addr = g_colorBuf[buf_idx].phy_addr;
	buf_width = g_colorBuf[buf_idx].buf_width,
	buf_height = g_colorBuf[buf_idx].buf_height; 
	buf_pitch = g_colorBuf[buf_idx].buf_pitch;

	pdl->format =  g_colorBuf[buf_idx].color_fmt;

	// VO_GRAPHIC_OSD4, etc
	pdl->param_osd.osd_plane = plane;

   	pdl->pitch = buf_pitch;

	pdl->address = (unsigned int) buf_phy_addr;

	pdl->addr_0 = 0 ;
	pdl->size_0 = 0;
	pdl->addr_1 = 0 ;
	pdl->size_1 =0;
	pdl->addr_2 = 0 ;
	pdl->size_2 = 0 ;
	
	pdl->x = 0;
	pdl->y = 0;
	pdl->width = buf_width;
	pdl->height = buf_height;
	
	
	pdl->dst_x =  0;
	pdl->dst_y = 0;

	pdl->dst_width = disp_rec.width;
	pdl->dst_height = disp_rec.height;
	

//	pdl->scale_factor = (int)((float) pdl->width / (float) pdl->dst_width);
	pdl->scale_factor = (int)(pdl->dst_width / pdl->width);

	//pdl->format = (VO_OSD_COLOR_FORMAT)VO_OSD_COLOR_FORMAT_RGBA8888_LITTLE;
	
	
	//hwc2_setCompressInfo( psLayer->buffer, pdl);


	pdl->src_type = g_colorBuf[buf_idx].src_type;

	if( pdl->src_type == SRC_NORMAL_PQDC )
	{
		//8888
		//pdl->src_type = SRC_NORMAL;
		pdl->format = VO_OSD_COLOR_FORMAT_PQDC;
		pdl->decompress = 1;
		pdl->compressed_ratio = 0;

		//pdl->pitch set inside GDMAExt_ReceivePicture
	}
	else if (pdl->src_type == SRC_FBDC ) {
		
	    pdl->fbdc_format = FBDC_COLOR_FORMAT_U8U8U8U8;
		pdl->decompress = 1;

		pdl->compressed_ratio = TFBC_GRP_0_LOSELESSS;
	}
	else 
	{
		//8888
		pdl->src_type = SRC_NORMAL;
		//pdl->format = VO_OSD_COLOR_FORMAT_RGBA8888_LITTLE;
		
		pdl->decompress = 0;
		pdl->compressed_ratio = 0;
	}

	pdl->alpha=0x0;

	GDMAEXT_DUMP("attach_buf_to_plane: plane:%d OBJ:%p bufIdx:%d type:%d addr:0x%lx [%d %d %d]\n", plane, pdl, buf_idx,
		pdl->src_type,
		buf_phy_addr, buf_width, buf_height, buf_pitch );


}

// DEBUG_PARAM_1_COLOR
void debug_fill_colorbar( int param_2)
{
  int buf_idx = 0;

	if(param_2 == 1 ) {
		
		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "do debug_fill_colorbar 1920, 1080\n" );

		do_fill_colorbar(1920, 1080, buf_idx) ;
	}
	else if ( param_2 == 2) {

		do_fill_colorbar(1920 + 20, 1080, buf_idx) ;  //width + 20 pixel

		GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "do debug_fill_colorbar 1920+20, 1080\n" );

	}
	else if ( param_2 == 3) {

		do_fill_colorbar(960, 1080, buf_idx) ; 
	}
	else if ( param_2 == 4) {

		do_fill_colorbar(960/2, 1080/2, buf_idx) ; 

	}
	else if ( param_2 == 5 ) {

		buf_idx = 0;
		do_fill_colorbar(3840, 2160, buf_idx) ; 

		dump_colorbuffer_info(buf_idx);

	}
	else if ( param_2 == 6 ) {

		buf_idx = 0;
		do_fill_colorbar( 640, 360, buf_idx) ; 

		dump_colorbuffer_info(buf_idx);

	}
	else if ( param_2 == 7 ) {

		buf_idx = 0;
		do_fill_colorbar( 640, 480, buf_idx) ; 

		dump_colorbuffer_info(buf_idx);

	}
	else if ( param_2 == 31) {

		buf_idx = 1;
		do_fill_colorbar(1920, 1080, buf_idx) ; 

		dump_colorbuffer_info(buf_idx);

	}
	else if ( param_2 == 33) {

		buf_idx = 1;
		do_fill_colorbar(960, 1080, buf_idx) ; 

		dump_colorbuffer_info(buf_idx);
	}
	else if ( param_2 == 34) {

		buf_idx = 1;
		do_fill_colorbar(960/2, 1080/2, buf_idx) ;
		
		dump_colorbuffer_info(buf_idx);
	}

}


void GDMAExt_DebugTest(int cmd1, int p1, int p2)
{
    gdma_dev *gdma = &gdma_devices[0];

	int onlinePlane[] = { GDMA_PLANE_OSD4, GDMA_PLANE_OSD5, GDMA_PLANE_OSD1 };
    int onlineMaxNum = sizeof(onlinePlane)/sizeof(onlinePlane[0]) ;

    int disPlane= 0;
    int onlineIdx = 0;
	int i;
	int buf_idx = 0;
	unsigned long phy_1 = 0 ;
	int width = 1920;
	int height = 1080;
	int size = 0;


	char* argb10_fname[] = {"/data/1_red.raw", "/data/2_green.raw", "/data/3_blue.raw"};
	char* argb10_480_fname[] = {"/data/1_red_green.raw", "/data/2_r_b_g.raw"};
	


    switch(cmd1)
    {
        case GDMAExt_DEBUG_CMD_DUMP:
        {
		switch(p1)
		{
			case 1: 
			{

				extern int GDMA_OSD1_DevCB_DumpStatus(GDMA_DISPLAY_PLANE plane, int level);

				GDMA_OSD1_DevCB_DumpStatus(GDMA_PLANE_OSD1, GDMA_EXT_LOG_ALL);

				for (onlineIdx = 0; onlineIdx < onlineMaxNum; onlineIdx++) {

					disPlane = onlinePlane[onlineIdx];

					if( g_osd_cb[disPlane].dumpStatus != NULL ) {
						g_osd_cb[disPlane].dumpStatus(disPlane, GDMA_EXT_LOG_ALL );
					}

					if( g_osd_cb[disPlane].dumpD2 != NULL ) {
						g_osd_cb[disPlane].dumpD2(disPlane, NULL );
					}

				}


				GDMAEXT_DUMP("\nosdMode:0x%x vsyncSrc:%d dtgMode:%d\n",  GDMAExt_getOSDMode(),  gVsyncSource_id, gDTG_APP_TYPE_osd );
				GDMAExt_dumpOSDMode();

			}
				break; 

			case 2: //dumpColorBuf info
			{
				GDMAEXT_DUMP("%d %d \n\n", p1 ,p2);

				for (i = 0;  i < TEST_COLORBUF_MAX_SIZE; i++) {
					GDMAEXT_DUMP("color buf:%d phy:0x%lx (%p) [%d %d %d]\n", i, g_colorBuf[i].phy_addr, g_colorBuf[i].vir_addr,
						g_colorBuf[i].buf_width, g_colorBuf[i].buf_height, g_colorBuf[i].buf_pitch
					);

				}


			}
			break;


		}

        }
        break;

		case GDMAExt_DEBUG_CMD_FILL: {
			
			switch(p1)
			{
				case DEBUG_PARAM_1_COLOR:
					debug_fill_colorbar( p2 );
					
					break;
				case DEBUG_PARAM_1_COLOR_PTG_SHIFT:

					g_DBG_PTG_ColorShift = p2;
			
					GDMAEXT_DUMP(" set ptg shift to %d\n",  p2 );
					break;
				
				case DEBUG_PARAM_1_COLOR_PTG_BarWidth:
					g_DBG_PTG_BarWidth = p2;

					GDMAEXT_DUMP(" set ptg barWidth to %d\n",  p2 );
					break;
				case DEBUG_PARAM_1_COLOR_PTG_PATTERN_IDX :
					g_DBG_PTG_ColorPattern_idx = p2;
					GDMAEXT_DUMP(" set ptg index to %d\n",  p2 );

					break;

			}


		}
		break;

		case GDMAExt_DEBUG_CMD_SEND:{
			int plane = p1;

			if( plane >= 1 && plane <= 5 ) {
				do_send_picture(p1, p2);
			}
			else
			{



			}


		}
		break;


		case GDMAExt_DEBUG_CMD_BATCH:
		{	

			switch(p1)
			{
				case 1: //osd4	4 1 0
					// extT=2 1 3
					debug_fill_colorbar( 3 );
					// extT=3 4 0
					if( p2 <= 2 )
						disPlane = p2;
					else { 
						disPlane = 4;
					}

					buf_idx = 0;

					do_send_picture(disPlane, buf_idx );

					g_DBG_PTG_ColorShift++;
				break;

				case 2:  //osd5		4 2 0
					// extT=2 1 33
					debug_fill_colorbar( 33 );
					// extT=3 5 1

					if( p2 <= 2 )
						disPlane = p2;
					else { 
						disPlane = 5;
					}

					
					buf_idx = 1;
					do_send_picture(disPlane, buf_idx );

					g_DBG_PTG_ColorShift++;
				break;
				case 3:  // 4 3 4,  4 3 5   (4k2k to osd X )

					debug_fill_colorbar( 5 );
					// extT=3 5 1

					
					disPlane = p2;
					
					
					buf_idx = 0;
					do_send_picture(disPlane, buf_idx );

					g_DBG_PTG_ColorShift++;
					break;

				case 4:  // 4 4 4,  4 4 5   ( 640 x 360 to osd X )

					debug_fill_colorbar( 6 );
					// extT=3 5 1

					
					disPlane = p2;
					
					
					buf_idx = 0;
					do_send_picture(disPlane, buf_idx );

					g_DBG_PTG_ColorShift++;
					break;


				case 5:  // 4 5 4,  4 5 5   ( 640 x 480 to osd X )

					debug_fill_colorbar( 7 );
					// extT=3 5 1

					
					disPlane = p2;
					
					
					buf_idx = 0;
					do_send_picture(disPlane, buf_idx );

					g_DBG_PTG_ColorShift++;
					break;

				
				case 21: //osd4
					// extT=2 1 3
					debug_fill_colorbar( 4 );
					// extT=3 4 0

					if( p2 <= 2 )
						disPlane = p2;
					else { 
						disPlane = 4;
					}

					buf_idx = 0;
					do_send_picture(disPlane, buf_idx );

					g_DBG_PTG_ColorShift++;
				break;

				case 22:  //osd5
					// extT=2 1 33
					debug_fill_colorbar( 34 );
					// extT=3 5 1
					if( p2 <= 2 )
						disPlane = p2;
					else { 
						disPlane = 5;
					}
					
					buf_idx = 1;
					do_send_picture(disPlane, buf_idx );

					g_DBG_PTG_ColorShift++;
				break;


				case 23:  //enable pqdc
				{
					unsigned int target_addr[] = { DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg,
						DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg};

					phy_1 = rtd_inl(target_addr[0]);
					

					buf_idx = 3;

					//debug_fill_colorbar( 35 );
					do_fill_colorbar_pseudo	(960, 1080, buf_idx, phy_1, SRC_NORMAL_PQDC, VO_OSD_COLOR_FORMAT_ARGB8888 ) ;

					// extT=3 5 1
					disPlane = 5;
					
					do_send_picture(disPlane, buf_idx );
				}
					break;
					
				case 24: //pqc 1920 x 2160  
				{
					unsigned int target_addr[] = { DC2H2_CAP_DC2H2_CAP0_BLK0_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK1_CTRL0_reg,
						DC2H2_CAP_DC2H2_CAP0_BLK2_CTRL0_reg, DC2H2_CAP_DC2H2_CAP0_BLK3_CTRL0_reg};

					phy_1 = rtd_inl(target_addr[0]);
					

					buf_idx = 3;

					//debug_fill_colorbar( 35 );
					do_fill_colorbar_pseudo	(1920, 2160, buf_idx, phy_1, SRC_NORMAL_PQDC, VO_OSD_COLOR_FORMAT_ARGB8888 ) ;

					// extT=3 5 1
					disPlane = 5;
					
					GDMA_OSD5_PQDC_Init(1);

					do_send_picture(disPlane, buf_idx );
				}

					break;


				case 31: // A2RGB10  1080  osd5
				case 32:  //480  osd5
				case 33:  //480 osd 4
				{
					int num_files = 0;
					char** fname = argb10_fname;

					#ifndef OSD_EXT_FILE_TEST
						GDMAEXT_DUMP("load file disabled!! can exec %d", __LINE__);
						break;
					#endif//

					if( p1 == 32 || p1 ==33) { // 
						width = 480;
						height = 540;

						fname = argb10_480_fname;
						num_files = sizeof(argb10_480_fname)/ sizeof(argb10_480_fname[0]);
					}
					else {
						width = 1920;
						height = 1080;

						num_files = sizeof(argb10_fname)/ sizeof(argb10_fname[0]);
					}
					
					size = width*height*4;

					if (p1 == 33 ) {
						disPlane = 4;
					} else {
						disPlane = 5;
					}

					buf_idx = 0;
					
					if( g_colorBuf[buf_idx].buf_width != width ||  g_colorBuf[buf_idx].buf_height != height) {
						//ALLOC 1 time only. pixel is 32 bits ,so can use default fill_colorbar
						do_fill_colorbar( width, height, buf_idx ) ;
					}

				    if (p2 < num_files ) {
						GDMAEXT_DUMP("load %s\n", fname[p2]);

						#ifdef OSD_EXT_FILE_TEST
							read_test_file_1(fname[p2], size, buf_idx );
						#endif//
					} else {
						GDMAEXT_DUMP("invalid p2 %d\n", p2);
					}

					g_colorBuf[buf_idx].src_type = SRC_NORMAL;
					g_colorBuf[buf_idx].color_fmt = VO_OSD_COLOR_FORMAT_ARGB2101010_LITTLE; 

					//do_fill_colorbar_pseudo(width, height, buf_idx, g_colorBuf[buf_idx].phy_addr, SRC_NORMAL, VO_OSD_COLOR_FORMAT_A2RGB10);
					//do_fill_colorbar_pseudo	(960, 1080, buf_idx, phy_1, SRC_NORMAL_TFBC, VO_OSD_COLOR_FORMAT_A2RGB10 ) ;
					do_send_picture(disPlane, buf_idx);

				}
				break;

				case 41:
				{
					buf_idx = 1;
					do_fill_colorbar(2560, 2160, buf_idx) ;
					dump_colorbuffer_info(buf_idx);

					disPlane = 5;
					do_send_picture(disPlane, buf_idx );


				}



				break;

			}

		}
		break;


		case GDMAExt_DEBUG_CMD_MIXER:
		{
			GDMA_MODIFY_MIXER_CMD mixer_cmd = {0};

			
			switch(p1)
			{
				case 1:
					//4,5 mixer on
					mixer_cmd.plane = VO_GRAPHIC_OSD4;
					mixer_cmd.enable = 1;
					GDMAExt_modify_mixer(&mixer_cmd);

					mixer_cmd.plane = VO_GRAPHIC_OSD5;
					mixer_cmd.enable = 1;
					GDMAExt_modify_mixer(&mixer_cmd);

					if( g_osd_cb[GDMA_PLANE_OSD4].setMixerDone) {
						g_osd_cb[GDMA_PLANE_OSD4].setMixerDone(GDMA_PLANE_OSD4, 1, gdma);
					}

					if( g_osd_cb[GDMA_PLANE_OSD5].setMixerDone) {
						g_osd_cb[GDMA_PLANE_OSD5].setMixerDone(GDMA_PLANE_OSD5, 1, gdma);
					}

					break;

				case 2:
					//4,5 mixer off
					mixer_cmd.plane = VO_GRAPHIC_OSD4;
					mixer_cmd.enable = 1;
					GDMAExt_modify_mixer(&mixer_cmd);

					mixer_cmd.plane = VO_GRAPHIC_OSD5;
					mixer_cmd.enable = 1;
					GDMAExt_modify_mixer(&mixer_cmd);

					if( g_osd_cb[GDMA_PLANE_OSD4].setMixerDone) {
						g_osd_cb[GDMA_PLANE_OSD4].setMixerDone(GDMA_PLANE_OSD4, 1, gdma);
					}

					if( g_osd_cb[GDMA_PLANE_OSD5].setMixerDone) {
						g_osd_cb[GDMA_PLANE_OSD5].setMixerDone(GDMA_PLANE_OSD5, 1, gdma);
					}

					break;


			}






		}//end of GDMAExt_DEBUG_CMD_MIXER
			break;

		case GDMAExt_DEBUG_CMD_DEBUG:
		{
			switch(p1)
			{
				case 1: // 9 1 0
					g_osdExt_stop_update = p2;
						GDMAEXT_DUMP("set osdExt no update %d\n", g_osdExt_stop_update);
					break;
				case 2:	// 9 2 4, 9 2 5    //olsd mode setting. parameter changed
				{
					GDMAEXT_OSD_MODE mode = GDMAEXT_OSD_MODE_DEFAULT; 

					if( p2 == 4 ) {
						mode = GDMAEXT_OSD_MODE_OSD4;
					}
					else if( p2 == 5 ) {
						mode = GDMAEXT_OSD_MODE_OSD5;
					}

					GDMAExt_setOSDMode(mode);
					GDMAEXT_DUMP("set osdMode to %d %d\n", p2, GDMAExt_getOSDMode() );


					rtd_outl(SYS_REG_INT_CTRL_SCPU_reg, SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_write_data(0));
					//clear osd 1
					rtd_outl(GDMA_OSD_INTST_reg, ~1);
					rtd_outl(GDMA_DMA_INTST_reg, ~1);
				}

					break;
				case 3:  // 9 3
				{
					GDMAEXT_OSD_MODE mode = p2; 

					GDMAExt_setOSDMode(mode);

					if( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD1) ) 
					{
						 if( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4 | GDMAEXT_OSD_MODE_OSD5) ) {
							GDMAEXT_DUMP("set osdMode to 1+4+5 %d\n", p2 );
						 }
						 else if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4) ) {
							GDMAEXT_DUMP("set osdMode to 1+4 %d\n", p2 );
						 }
						 else if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD5) ) {
							GDMAEXT_DUMP("set osdMode to 1+5 %d\n", p2 );
						 }

						//switch back to osd 1? 
						 GDMAExt_SwitchVsyncSource( GDMA_PLANE_OSD1) ;
					}
					else
					{ // no osd 1 
						if( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4 | GDMAEXT_OSD_MODE_OSD5) ) {
							GDMAEXT_DUMP("set osdMode to 4+5 %d\n", p2 );

							GDMAExt_SwitchVsyncSource( GDMA_PLANE_OSD4 ) ;
						 }
						 else if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD4) ) {
							GDMAEXT_DUMP("set osdMode to 4 %d\n", p2 );

							GDMAExt_SwitchVsyncSource( GDMA_PLANE_OSD4 ) ;
						 }
						 else if ( GDMAOSD_MOD_HAS_MODE(mode, GDMAEXT_OSD_MODE_OSD5) ) {
							GDMAEXT_DUMP("set osdMode to 5 %d\n", p2 );

							GDMAExt_SwitchVsyncSource( GDMA_PLANE_OSD5 ) ;
						 }

							#if 0
							rtd_outl(SYS_REG_INT_CTRL_SCPU_reg, SYS_REG_INT_CTRL_SCPU_osd_int_scpu_routing_en_mask | SYS_REG_INT_CTRL_SCPU_write_data(0));
							//clear osd 1
							rtd_outl(GDMA_OSD_INTST_reg, ~1);
							rtd_outl(GDMA_DMA_INTST_reg, ~1);
							#else
							{
								ppoverlay_osddtg_control_RBUS reg;
								reg.regValue = rtd_inl(PPOVERLAY_OSDDTG_CONTROL_reg); 
								reg.osddtg_en = 0;
								OSD_RTD_OUTL(PPOVERLAY_OSDDTG_CONTROL_reg,  reg.regValue );
							}
							#endif//

					} 
					
				}

					break;

				case 11:  //enable identity  9 11 5
					disPlane = p2;
					gdma->enable_identity[disPlane] = 1;

					GDMAEXT_DUMP("osd %d enable identity\n", disPlane);
					{
							osd_sr45_osd_sr_5_scaleup_ctrl0_RBUS osd_sr_scaleup_ctrl0_reg;
							osd_sr45_osd_sr_5_scaleup_ctrl1_RBUS osd_sr_scaleup_ctrl1_reg;
							osd_sr45_osd_sr_5_h_location_ctrl_RBUS osd_sr_h_location_ctrl_reg;
							osd_sr45_osd_sr_5_v_location_ctrl_RBUS osd_sr_v_location_ctrl_reg;
							osd_sr45_osd_sr_5_ctrl_RBUS osd_sr_ctrl_reg;

						osd_sr_scaleup_ctrl0_reg.v_zoom_en = 1;
						osd_sr_scaleup_ctrl0_reg.ver_factor = 0x0000ffff;
						

						osd_sr_scaleup_ctrl1_reg.h_zoom_en = 1;
						osd_sr_scaleup_ctrl1_reg.hor_factor = 0x0000ffff;
						

						width = 1920;
						height = 1080;

						osd_sr_h_location_ctrl_reg.osd_width =  width;
						osd_sr_v_location_ctrl_reg.osd_height = height;
						
						osd_sr_ctrl_reg.identity_en = 1;
						


						OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Scaleup_Ctrl0_reg	, osd_sr_scaleup_ctrl0_reg.regValue);
						OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Scaleup_Ctrl1_reg, osd_sr_scaleup_ctrl1_reg.regValue);
						OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_V_Location_Ctrl_reg, osd_sr_v_location_ctrl_reg.regValue);
						OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_H_Location_Ctrl_reg, osd_sr_h_location_ctrl_reg.regValue);
						OSD_RTD_OUTL(OSD_SR45_OSD_SR_5_Ctrl_reg, osd_sr_ctrl_reg.regValue);

						GDMA_OSD5_preFrame_SetPrecrop( width, height );
						GDMA_OSD5_DevCB_set_ProgDone (GDMA_PLANE_OSD5, GMDA_OSD_DONE_MODE_SINGLE, NULL);
						GDMA_OSD5_DevCB_setMixerDone(GDMA_PLANE_OSD5,  1, NULL );


					}

					break;
				case 12:  //disable identity
					disPlane = p2;
					gdma->enable_identity[disPlane] = 0;
					GDMAEXT_DUMP("osd %d disable identity\n", disPlane);
					break;


				case 91: //enable clk  9 91 4
					disPlane = p2;
					GDMAExt_ClkEnable(disPlane, 1);
					break;

				case 92: //disable clk  9 92 4
					disPlane = p2;
					GDMAExt_ClkEnable(disPlane, 0);

					break;

				
				case 201:
					GDMA_OSD5_preFrame_SetPrecrop(3840, 2160);
					break;
				case 202:
					GDMA_OSD5_preFrame_SetPrecrop(1920, 2160);
					break;
				case 203:
					GDMA_OSD5_preFrame_SetPrecrop(1920, 1080);
					break;



		}


		} //end of GDMAExt_DEBUG_CMD_DEBUG
		break;

        default:
            GDMA_EXT_LOG(GDMA_EXT_LOG_ERROR, "invalid extDBG cmd %d p:%d %d\n", cmd1, p1, p2 );
    }


	GDMAEXT_DUMP("exec cmd %d p: %d %d\n", cmd1, p1, p2 );

}

